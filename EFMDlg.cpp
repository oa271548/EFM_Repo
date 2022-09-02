// EFMDlg.cpp : implementation file
//

// Software Version Number in VersionNo.h

#include "stdafx.h"
#include "EFM.h"
#include "EFMDlg.h"
#include "EFMOptions.h"
#include <winioctl.h>
#include "WaitDlg.h"
#include "Mmsystem.h" // for PlaySound()
#include <math.h>
#include "EventLog.h"	
#include "EventLogViewer.h"
#include "ClientsConnectedViewer.h"
//#include "LWS300.h"
#include "version.h"
//#include "windows.h"
//#include "stormpci_lts2.h"	// Coincidence Detection: StormTracker DLL
//#include "LD350.h"			// Coincidence Detection: LD350 DLL


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define HOURMULTIPLIER (1.0/24.0)
#define MINUTEMULTIPLIER (1.0/(24.0*60.0))
#define SECONDMULTIPLIER (1.0/(24.0*60.0*60.0))
#define TENTHSMULTIPLIER (1.0/(24.0*60.0*60.0*10.0))

#define LA_STARTUP_VAL 11  // we need 11 fresh values (3,2,1,0) in CheckForLightningAlarm()

CAlerts Alerts;  // email and sms class

CConfiguration config;	// Make global for easy access
CEFMOptions* dlgOptions;
CEventLog m_EventLog;   // global so Alerts can access 

CStringArray saFMData;

// EFA20 Device info
std::vector <tEFA20DEVICEINFO> vEFA20Devices;


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
		//{{AFX_DATA(CAboutDlg)
#ifdef __BRIGHTEX__
	enum { IDD = IDD_ABOUTBOX_BRIGHTEX };
#else
	enum { IDD = IDD_ABOUTBOX_BOLTEK };
#endif
	CString	m_txtVersion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_txtVersion = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, LBL_VERSION, m_txtVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Load the information
	try
	{
		version v;
		CString txtTemp;

		// Software Version - ignores the right two most version subnumbers
		txtTemp.Format("%s", v.get_product_version().c_str());
		txtTemp.Replace(", ", ".");
		int iDec = txtTemp.Find(".");
		int iDec1 = txtTemp.Find(".", iDec + 1);
		m_txtVersion.Format("Version %s", txtTemp.Left(iDec1));

		UpdateData(FALSE);
	}
	catch (const std::exception& e)
	{
		CString txtError;
		txtError.Format("There was an error getting the version information.\n\n%s", e.what());
		::MessageBox(this->GetSafeHwnd(), txtError, "Boltek LDN Detector", MB_OK | MB_ICONERROR);

		//std::cerr << e.what() << std::endl;
	}


	// If 100kV mode is enabled, change the version number to reflect
#ifdef __100kV__
	CString txtVersion, txtNewVersion;
	GetDlgItemText(LBL_VERSION, txtVersion);
	txtNewVersion.Format("%s-100kV", txtVersion);
	SetDlgItemText(LBL_VERSION, txtNewVersion);
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CEFMDlg dialog

CEFMDlg::CEFMDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CEFMDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEFMDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
#ifdef __BRIGHTEX__
	m_hIcon = AfxGetApp()->LoadIcon(IDI_BRIGHTEX);
#else
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
#endif

	m_pSoundHandlerThread = nullptr;
	m_aSoundHandlerPlaylist.RemoveAll();
	m_pDlgDebugger = nullptr;
}

void CEFMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEFMDlg)
	DDX_Control(pDX, KNOB_TIMESELECTOR, m_knobTimeSelector);
	DDX_Control(pDX, KNOB_SCALESELECTOR, m_knobScaleSelector);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEFMDlg, CDialog)
	//{{AFX_MSG_MAP(CEFMDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(MNU_FILE_EXIT, OnFileExit)
	ON_COMMAND(MNU_HELP_ABOUT, OnHelpAbout)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(MNU_TOOLS_OPTIONS, OnToolsOptions)
	ON_COMMAND(MNU_TOOLS_SHOWCONNECTEDCLIENTS, OnShowConnectedClients)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_COMMAND(MNU_FILE_OPEN, OnFileOpen)
	ON_COMMAND(MNU_FILE_VIEWEVENTLOG, OnFileVieweventlog)
	ON_WM_CTLCOLOR()
	ON_CONTROL(CVN_ViewPortChanged, CHARTVIEW_FIELDMILL1, OnViewPortChanged)
	ON_CONTROL(CVN_ViewPortChanged, CHARTVIEW_FIELDMILL2, OnViewPortChanged)
	ON_CONTROL(CVN_ViewPortChanged, CHARTVIEW_FIELDMILL3, OnViewPortChanged)
	ON_CONTROL(CVN_ViewPortChanged, CHARTVIEW_FIELDMILL4, OnViewPortChanged)
	ON_CONTROL(CVN_ViewPortChanged, CHARTVIEW_FIELDMILL5, OnViewPortChanged)
	ON_CONTROL(CVN_ViewPortChanged, CHARTVIEW_FIELDMILL6, OnViewPortChanged)
	ON_CONTROL(CVN_ViewPortChanged, CHARTVIEW_FIELDMILL7, OnViewPortChanged)
	ON_CONTROL(CVN_ViewPortChanged, CHARTVIEW_FIELDMILL8, OnViewPortChanged)
	ON_CONTROL(BN_CLICKED, CHARTVIEW_FIELDMILL1, OnClickFieldmill1)
	ON_CONTROL(BN_CLICKED, CHARTVIEW_FIELDMILL2, OnClickFieldmill2)
	ON_CONTROL(BN_CLICKED, CHARTVIEW_FIELDMILL3, OnClickFieldmill3)
	ON_CONTROL(BN_CLICKED, CHARTVIEW_FIELDMILL4, OnClickFieldmill4)
	ON_CONTROL(BN_CLICKED, CHARTVIEW_FIELDMILL5, OnClickFieldmill1)
	ON_CONTROL(BN_CLICKED, CHARTVIEW_FIELDMILL6, OnClickFieldmill2)
	ON_CONTROL(BN_CLICKED, CHARTVIEW_FIELDMILL7, OnClickFieldmill3)
	ON_CONTROL(BN_CLICKED, CHARTVIEW_FIELDMILL8, OnClickFieldmill4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEFMDlg message handlers

BOOL CEFMDlg::OnInitDialog()
{
	Chart::setLicenseCode("RDST-26P3-MMQB-8BL3-6575-8B6D");
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

#ifdef __BRIGHTEX__
	this->SetWindowText("Brightex Electric Field Monitor");
#else
	this->SetWindowText("Boltek Electric Field Monitor");
#endif

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	int ctr;
	m_hSPEAKER = NULL;
	m_bSpeakerOpen = false;
	saFMData.SetSize(MAX_NUMOFDATASOURCES);
	m_iChartSelected = 0;

	CString txt;

	m_txtStatusBarText = "";

	// Modify the titlebar to add the current software version
	try
	{
		version v;
		CString txtTemp;

		// Software Version - ignores the right two most version subnumbers
		txtTemp.Format("%s", v.get_product_version().c_str());
		txtTemp.Replace(", ", ".");
		int iDec = txtTemp.Find(".");
		int iDec1 = txtTemp.Find(".", iDec + 1);
		txtTemp.Format("%s", txtTemp.Left(iDec1));

		CString txtTitle;
		this->GetWindowText(txtTitle);
		txtTitle += " v";
		txtTitle += txtTemp;
		this->SetWindowText(txtTitle);
	}
	catch (const std::exception& e)
	{
		CString txtError;
		txtError.Format("There was an error getting the version information.\n\n%s", e.what());
		::MessageBox(this->GetSafeHwnd(), txtError, "EFM Error", MB_OK | MB_ICONERROR);
		//std::cerr << e.what() << std::endl;
	}

	// Initialize the Visual Alarms (All Off)
	// We can't call the "Disable***Alarm()" functions as the window 
	// and/or controls may not exist yet.  "OnCtlColor" hasn't been 
	// called yet and needs to be called first.  Then we can use the 
	// "Disable***Alarm()" functions.
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		m_dLastReading[ctr] = 0;
		m_bLAStatusCheck[ctr] = false;
		m_bVHFAStatusCheck[ctr] = false;
		m_bHFAStatusCheck[ctr] = false;
		m_bDeltaAlarmCheck[ctr] = false;
		m_bLightningAlarmCheck[ctr] = false;
		m_bVeryHighFieldAlarmCheck[ctr] = false;
		m_bHighFieldAlarmCheck[ctr] = false;

		tThInfo[ctr].m_iLACounter = 0;	//(int)(config.m_dLADuration * 60);
		tThInfo[ctr].m_iDACounter = 0;
		tThInfo[ctr].m_iVHFACounter = 0;	//(int)(config.m_dVHFADuration * 60);
		tThInfo[ctr].m_iHFACounter = 0;	//(int)(config.m_dHFADuration * 60);
		tThInfo[ctr].m_iLAWAVCounter = 0;	// No WAV message pending
		tThInfo[ctr].m_iDAWAVCounter = 0;
		tThInfo[ctr].m_iVHFAWAVCounter = 0;
		tThInfo[ctr].m_iHFAWAVCounter = 0;
		tThInfo[ctr].m_bPlayLAOnActivationWAV = false;
		tThInfo[ctr].m_bPlayLADistanceWAV = false;
		tThInfo[ctr].m_bPlayLAAllClearWAV = false;
		tThInfo[ctr].m_bPlayDAWAV = false;
		tThInfo[ctr].m_bPlayVHFAWAV = false;
		tThInfo[ctr].m_bPlayHFAWAV = false;
		tThInfo[ctr].m_iLightningDetectedAtCtr = 0;

		tThInfo[ctr].m_bDALastState = false;
		tThInfo[ctr].m_bVHFALastState = false;
		tThInfo[ctr].m_bHFALastState = false;
		tThInfo[ctr].m_bLALastState = false;
		tThInfo[ctr].m_bLoggedRotorFault = false;
		tThInfo[ctr].m_bLoggedSelfTestFault = false;
		tThInfo[ctr].m_bRotorFault = false;
		tThInfo[ctr].m_bSelfTestFault = false;

		tThInfo[ctr].ClrDeltaAlarmBuffer();		// Clear the Delta Alarm buffer

		tThInfo[ctr].m_bCDHardwareEventDetected = false;
		tThInfo[ctr].m_bCDThreadRunning = false;
		tThInfo[ctr].m_pCDThread = NULL;

		tThInfo[ctr].m_txtEFA20SerialNumber = "";

		memset(&tDataClientServer[ctr], 0, sizeof(tDATACLIENTSERVER));

	}

	// Load the configuration
	CRegistry reg;
	reg.SetRootKey(REG_ROOT_LOCATION);		// Set the default HKEY location
//	if (!reg.SetKey(REG_LOCATION, FALSE))	// SetKey: Read
//	{
//		// Registry keys don't exist, create some.
//		config.SaveConfiguration();
//	}
	// Registry keys already exist, read them. ** Default keys are created by InstallShield **
	config.LoadConfiguration();

	// Create/Continue the Event logging
	CString tmp;
	tmp.LoadString(STR_EVENTLOGFILENAME);
	txt.Format("%s%s", config.m_szDataLoggingPath[0], tmp);
	// Open the event log file
	m_EventLog.SetEventLogFilename(txt);
	m_EventLog.LogEvent("SYSTEM EFM Software Started.");

	//
	// Register WinDriver with the system
	//HANDLE hWD;
	//WD_LICENSE lic;
	//hWD = WD_Open();
	//if (hWD!=INVALID_HANDLE_VALUE)
	//{ 
	//	strcpy(lic.cLicense, "77f43e95087ec1cd118164fc2e4afe.Boltek Corp");
	//	WD_License(hWD,&lic);
	//}
	//WD_Close(hWD);

	// Subclass controls to arrays.  
	// The control ID's must be sequencial.
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		//m_stripChart[ctr].SubclassDlgItem(STRIP_FIELDMILL1+ctr,this);
		m_chartViewer[ctr].SubclassDlgItem(CHARTVIEW_FIELDMILL1 + ctr, this);
		m_chartLightningDistanceViewer[ctr].SubclassDlgItem(CHARTVIEW_FIELDMILL5 + ctr, this);
		m_numledFieldMillValue[ctr].SubclassDlgItem(NUMLED_FIELDMILL1 + ctr, this);

		m_pctHighFieldVisual[ctr].SubclassDlgItem(PCT_HF_FIELDMILL1 + ctr, this);
		m_pctVeryHighFieldVisual[ctr].SubclassDlgItem(PCT_VHF_FIELDMILL1 + ctr, this);
		m_pctLightningVisual[ctr].SubclassDlgItem(PCT_L_FIELDMILL1 + ctr, this);
		m_pctDeltaVisual[ctr].SubclassDlgItem(PCT_DELTA_FIELDMILL1 + ctr, this);
		m_pctStatusVisual[ctr].SubclassDlgItem(PCT_STATUS_FIELDMILL1 + ctr, this);

		m_laLightningAlarm[ctr].SubclassDlgItem(LALARM_FIELDMILL1 + ctr, this);

		m_tglMuteSound[ctr].SubclassDlgItem(TGL_MUTESOUND_FIELDMILL1 + ctr, this);

		// Make sure the Status control is off initially
		SetStatusText("");
		SetStatusControl(ctr, FALSE);
		// Set the initial sound settings
		tThInfo[ctr].m_bSoundOn = (m_tglMuteSound[ctr].GetValue() != 0);
	}


	// Make a black brush so that when CtlColor is called, the dialog box will 
	// think it has a black background.
	m_brushBackground.CreateSolidBrush(RGB(0, 0, 0));

	// Prepare the window with the controls
	LayoutControls();
	InitControls();

	// See if there are any EFA20's connected
	EFA20_DetectConnectedDevices();

	// Since Data Source 0 may not always be active (Source 3 could be the only 
	// one active), we need to find the first ENABLED data source.  Once we know 
	// the first, call it's function so it's the default selected.  If multiple 
	// sources are enable, it'll always go with the lowest index.
	// This is sloppy and can be looked at to be fixed for next release.
	// Finds the first active data source
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		if (config.m_BDataSourceEnabled[ctr] == TRUE)
		{
			break;
		}
	}
	// Calls the appropriate function.
	switch (ctr)
	{
	case 0:
		OnClickFieldmill1();
		break;
	case 1:
		OnClickFieldmill2();
		break;
	case 2:
		OnClickFieldmill3();
		break;
	case 3:
		OnClickFieldmill4();
		break;
	}
	MakeChartsReflectKnobs();

	// Start the acquisition threads if there are any enabled
	tServer.SetSize(MAX_NUMOFDATASOURCES);

	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		SetNextScheduledAlertTime(ctr); // scheduled status messages

		// If the source is tagged enabled, then start the thread(s) for it.
		if (config.m_BDataSourceEnabled[ctr] == TRUE)
		{
			tThInfo[ctr].m_bSignalLost = false;

			// Start the thread in the appropriate manner
			StartDAQThread(ctr, config.m_szDataSourcePoint[ctr]);

			if (config.m_BDataSharingEnabled[ctr])
			{
				CString newtitle;
				newtitle.Format("%s (%d Clients Connected)", config.m_szDataSourceName[ctr], tThInfo[ctr].ClientsCount());
				SetChartTitle(ctr, newtitle);
			}


		} // End "if (config.m_BDataSourceEnabled[ctr] == TRUE)"
	} // End "for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)"

	// Set timer to monitor signal status
	SetTimer(TMR_ONESEC, 1000, NULL);
	SetTimer(TMR_ONEFIFTHSEC, 200, NULL);

	// Start the sound player
	SoundHandlerStart();

	Alerts.StartThread();  // SMS and Email 

	// Play startup wav file
	PlayWAV(0, config.m_txtWAVStartup);

	//	DebugWindowShow(); // shows connected network clients

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEFMDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEFMDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEFMDlg::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void CEFMDlg::OnFileExit()
{
	// TODO: Add your command handler code here
	// Exits the program
	OnClose();
}

void CEFMDlg::OnHelpAbout()
{
	// TODO: Add your command handler code here
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CEFMDlg::OnDestroy()
{
	CDialog::OnDestroy();
	// TODO: Add your message handler code here
}

BOOL CEFMDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	CString txt;
	int iFieldMillID;
	CRect rect;
	//	int ctr;

	const int iFontScaleDivider = 40;

	if ((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) ||
		(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE))
	{
		return TRUE; // just ignore the enter key
	}

	switch (pMsg->message)
	{
	case WM_SETCONTROLCOLOURS:
		//m_stripChart[(int)pMsg->wParam].SetVariableColor((DWORD)pMsg->lParam);
		m_chartViewer[(int)pMsg->wParam].setDataLineColor((DWORD)pMsg->lParam);
		m_chartLightningDistanceViewer[(int)pMsg->wParam].setDataLineColor((DWORD)pMsg->lParam);
		DWORD nColour;
		nColour = static_cast<DWORD>(pMsg->lParam);
		m_numledFieldMillValue[(int)pMsg->wParam].SetOnColor(((nColour & 0xff0000) >> 16) +
			(nColour & 0xff00) + ((nColour & 0xff) << 16));
		if ((DWORD)pMsg->lParam == COLOUR_DISABLED)
		{
			m_laLightningAlarm[(int)pMsg->wParam].SetEnabled(false);
		}
		else
		{
			m_laLightningAlarm[(int)pMsg->wParam].SetEnabled(true);
		}
		return TRUE;	// We've processed this message
		break;
	case WM_SIGNALLOSTFAULT:
		iFieldMillID = (int)pMsg->lParam;
		PostMessage(WM_SETCONTROLCOLOURS, (WPARAM)iFieldMillID, (LPARAM)COLOUR_DISABLED); // grey out
		PostMessage(MSG_NETWORKSOURCE_SIGNALLOST, 0, (LPARAM)iFieldMillID); // display status bar
		// Play the Signal Lost Fault wav file
		if (tThInfo[iFieldMillID].m_bSoundOn)
		{
			PlayWAV(iFieldMillID, config.m_txtWAVDataLostFault);
		}
		// Try restarting the thread
		StopDAQThread(iFieldMillID, config.m_szDataSourcePoint[iFieldMillID]);
		StartDAQThread(iFieldMillID, config.m_szDataSourcePoint[iFieldMillID]);
		// only log if we aren't already in fault
		if (!tThInfo[iFieldMillID].m_bSignalLost)
		{
			// Event Log.
			txt.Format("%s Data Lost Fault.", config.m_szDataSourceName[iFieldMillID]);
			m_EventLog.LogEvent(txt);
			// Email & SMS Fault
			if (config.bAlertsFaultsActive)
			{
				Alerts.SendAlerts(iFieldMillID, "Data Lost Fault");
			}
			tThInfo[iFieldMillID].m_bSignalLost = true;
		}
		return TRUE;	// We've processed this message
		break;
	case MSG_STATUSBAR_SHOW:
		iFieldMillID = (int)pMsg->lParam;
		m_pctStatusVisual[iFieldMillID].GetClientRect(&rect);
		m_pctStatusVisual[iFieldMillID].SetFontSize(static_cast<float>(rect.Width()) / iFontScaleDivider);
		SetStatusControl(iFieldMillID, TRUE);
		m_pctStatusVisual[iFieldMillID].ShowWindow(TRUE);
		return TRUE;	// We've processed this message
		break;
	case MSG_STATUSBAR_HIDE:
		iFieldMillID = (int)pMsg->lParam;
		SetStatusControl(iFieldMillID, FALSE);
		m_pctStatusVisual[iFieldMillID].ShowWindow(FALSE);
		return TRUE;	// We've processed this message
		break;
	case MSG_NETWORKSOURCE_UNABLETOCONNECT:
		iFieldMillID = (int)pMsg->lParam;
		m_txtStatusBarText = "Unable to connect to client.";
		m_pctStatusVisual[iFieldMillID].GetClientRect(&rect);
		m_pctStatusVisual[iFieldMillID].SetFontSize(static_cast<float>(rect.Width()) / iFontScaleDivider);
		SetStatusControl(iFieldMillID, TRUE);
		m_pctStatusVisual[iFieldMillID].ShowWindow(TRUE);
		return TRUE;
		break;
	case MSG_NETWORKSOURCE_SIGNALLOST:
		iFieldMillID = (int)pMsg->lParam;
		m_txtStatusBarText = "Signal Lost";
		m_pctStatusVisual[iFieldMillID].GetClientRect(&rect);
		m_pctStatusVisual[iFieldMillID].SetFontSize(static_cast<float>(rect.Width()) / iFontScaleDivider);
		SetStatusControl(iFieldMillID, TRUE);
		m_pctStatusVisual[iFieldMillID].ShowWindow(TRUE);
		return TRUE;
		break;
	case MSG_SERIAL_ERROR_UNABLETOSETCOMTIMEOUTS:
		iFieldMillID = (int)pMsg->lParam;
		txt.Format("[%s] Unable to set connection timeouts on %s %s", config.m_szDataSourceName[iFieldMillID], config.m_szDataSourcePoint[iFieldMillID], config.m_szComPort[iFieldMillID]);
		m_EventLog.LogEvent(txt);
		return TRUE;
		break;
	case MSG_SERIAL_ERROR_UNABLETOSETCOMSTATE:
		iFieldMillID = (int)pMsg->lParam;
		txt.Format("[%s] Unable to set com state on %s %s", config.m_szDataSourceName[iFieldMillID], config.m_szDataSourcePoint[iFieldMillID], config.m_szComPort[iFieldMillID]);
		m_EventLog.LogEvent(txt);
		return TRUE;
		break;
	case MSG_SERIAL_ERROR_UNABLETOGETCOMSTATE:
		iFieldMillID = (int)pMsg->lParam;
		txt.Format("[%s] Unable to get com state on %s %s", config.m_szDataSourceName[iFieldMillID], config.m_szDataSourcePoint[iFieldMillID], config.m_szComPort[iFieldMillID]);
		m_EventLog.LogEvent(txt);
		return TRUE;
		break;
	case MSG_SERIAL_ERROR_UNABLETOCONNECT:
		iFieldMillID = (int)pMsg->lParam;
		txt.Format("[%s] Unable to connect on %s %s", config.m_szDataSourceName[iFieldMillID], config.m_szDataSourcePoint[iFieldMillID], config.m_szComPort[iFieldMillID]);
		m_EventLog.LogEvent(txt);
		return TRUE;
		break;

	} // End "switch (pMsg->message)"

	return CDialog::PreTranslateMessage(pMsg);
}

//http://social.msdn.microsoft.com/Forums/vstudio/en-US/09242de7-a092-4240-8d76-4958d4549ab4/communicating-with-a-serial-device-using-createfile
// http://msdn.microsoft.com/en-us/library/ms810467.aspx
UINT CEFMDlg::FieldMillSerial(LPVOID pParam)
{
	tTHREADINFO* pInfo = (tTHREADINFO*)pParam;

	// Thread is active
	pInfo->SetThreadAlive(true);

	HANDLE hSerial = NULL;
	CString txtComPort;
	OVERLAPPED osReader = { 0 };
	DCB dcb;
	BOOL BSuccess = FALSE;
	bool bDone = false;
	DWORD dwObj = 0;
	DWORD dwEvtMask = 0;
	HANDLE pH[2];
	DWORD dwRead = 0;
	const int MAX_READ_BUFFER = 2048;
	char szData[MAX_READ_BUFFER];
	DWORD      dwRes;           // result from WaitForSingleObject
	char efmstr[MAX_READ_BUFFER];
	int sentence_pos = 0;
	size_t iBytePtr;
	bool bStartup = true; // ignore the first sentence since it might be partial
	int iPtr = 0;

	txtComPort.Format("\\\\.\\%s", config.m_szComPort[pInfo->GetID()]);

	hSerial = CreateFile(txtComPort,					// communication port string (COMX)
		GENERIC_READ,					// read type
		0,								// comm devices must be opened with exclusive access
		0,								// no security attributes
		OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
		FILE_FLAG_OVERLAPPED,
		0);
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		// Unable to open the port
		CloseHandle(hSerial);
		pInfo->SetThreadAlive(false);
		TRACE("FieldMillSerial() Thread: %p Can't open COM port %s, Exiting Code 2\n", GetCurrentThreadId(), txtComPort);
		return 2;
	}

	// Set event mask to signal on a received character
	BSuccess = SetCommMask(hSerial, EV_RXCHAR);
	if (!BSuccess)
	{
		pInfo->SetThreadAlive(false);
		CloseHandle(hSerial);
		hSerial = NULL;
		return 3;
	}

	memset(&dcb, 0, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	BSuccess = GetCommState(hSerial, &dcb);
	if (!BSuccess)
	{
		pInfo->SetThreadAlive(false);
		CloseHandle(hSerial);
		hSerial = NULL;
		return 4;
	}

	dcb.BaudRate = CBR_9600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	BSuccess = SetCommState(hSerial, &dcb);
	if (!BSuccess)
	{
		pInfo->SetThreadAlive(false);
		CloseHandle(hSerial);
		hSerial = NULL;
		return 5;
	}

	// CONNECT OK
	TRACE("FieldMillSerial() Connected to source %s Thread: %p.\n", config.m_szDataSourceName[pInfo->GetID()], GetCurrentThreadId());
	// Green in the graph and the LED digits
	((CEFMDlg*)pInfo->pMyOwner)->PostMessage(WM_SETCONTROLCOLOURS, (WPARAM)pInfo->GetID(), (LPARAM)COLOUR_DISABLED);
	// Hide the status bar for this data source
	((CEFMDlg*)pInfo->pMyOwner)->PostMessage(MSG_STATUSBAR_HIDE, 0, (LPARAM)pInfo->GetID());
	// Construct and display the new title
	//CString newTitle = ((CEFMDlg*)pInfo->pMyOwner)->m_stripChart[pInfo->GetID()].GetCaption();
	CString newTitle = ((CEFMDlg*)pInfo->pMyOwner)->m_chartViewer[pInfo->GetID()].getCaption();
	//((CEFMDlg*)pInfo->pMyOwner)->m_stripChart[pInfo->GetID()].SetCaption(newTitle);
	((CEFMDlg*)pInfo->pMyOwner)->m_chartViewer[pInfo->GetID()].setCaption(newTitle);


	// Create the event object
	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Handle array
	pH[0] = pInfo->GetSerialThreadExit();
	pH[1] = osReader.hEvent;

	// Main Processing Loop
	while (!bDone)
	{
		// Issue the overlapped read command
		ReadFile(hSerial, szData, 10, &dwRead, &osReader);  // only read small blocks or else we block and get shoppy data

		dwRes = WaitForMultipleObjects(2, pH, FALSE, INFINITE);

		switch (dwRes)
		{
		case WAIT_OBJECT_0:
			// Time to exit
			bDone = true;
			break;
		case WAIT_OBJECT_0 + 1:
			if (!GetOverlappedResult(hSerial, &osReader, &dwRead, FALSE))
			{
				if (GetLastError() == ERROR_OPERATION_ABORTED)
				{
					// Read aborted - restart
					bDone = true;
				}
				else
				{
					// GetOverlappedResult (in Reader)
				}
			}
			else
			{
				// Read completed successfully
				if (dwRead)
				{
					//read something...
					for (iBytePtr = 0; iBytePtr < dwRead; iBytePtr++)
					{
						if ((szData[iBytePtr] != '$') && (szData[iBytePtr] != '>') && (szData[iBytePtr] != '<'))
						{
							efmstr[sentence_pos] = szData[iBytePtr]; // copy char from sentence
							sentence_pos++;
						}
						else
						{
							// we found a '$' or '<' or '>'
							efmstr[sentence_pos] = '\0'; // terminate sentence
							// "efmstr[]" holds a sentence at this point

							// If data sharing is enabled, send the sentence value down the pipe to the
							// server thread.  To allow network clients to use their alarms on the 
							// network data, we need to send all data (10 times a second) over the 
							// socket to the client.
							//saFMData[pInfo->GetID()] = efmstr;	//sentence;
							if (config.m_BDataSharingEnabled[pInfo->GetID()])
							{
								for (iPtr = 0; iPtr < MAX_NETWORKCLIENTS; iPtr++)
								{
									if (pInfo->clientsConnected[iPtr].GetIsAlive())
									{
										pInfo->clientsConnected[iPtr].FifoAdd(efmstr);
									}
								}
							} // if (config.m_BDataSharingEnabled[pInfo->GetID()])

							if (!bStartup)
							{
								// only show trace for solar efm.  it is choppier due to packetized data
								if (pInfo->GetID() == 3)
								{
									//										TRACE("%s: %s", config.m_szComPort[pInfo->GetID()], efmstr);
								}

								if (CEFMDlg::ProcessEFMSentence(efmstr, pInfo, &pInfo->logData) == false)
								{
									// There was a problem with the EFM sentence (malformed, out of sync, etc)
									TRACE("ProcessEFMSentence isn't happy: %s\n", efmstr);
									bStartup = true;
								}
							}
							else
							{
								bStartup = false; // ignore the first sentence since it might be partial
							}

							sentence_pos = 0; // start collecting at beginning again
							efmstr[sentence_pos++] = szData[iBytePtr]; // copy '$' from sentence
						}

					} // for (iBytePtr=0; iBytePtr<dwRead; iBytePtr++)
				} // if (dwRead)
				else
				{
					// some serial ports are always signalled with 0 bytes available causing 
					// this thread to eat all cpu time and lock up application
					Sleep(50);
				}
			}
			break;
		default:
			break;
		} // switch (dwRed)


	} // while (!bDone)

	// Since we're shutting down, disconnect any clients that may be connected
	for (iPtr = 0; iPtr < MAX_NETWORKCLIENTS; iPtr++)
	{
		if (pInfo->clientsConnected[iPtr].GetIsAlive())
		{
			SetEvent(pInfo->clientsConnected[iPtr].GetThreadExitHandle());
			while (pInfo->clientsConnected[iPtr].GetIsAlive())
			{
				TRACE("FieldMillSerial() is shutting down client threads\n");
				Sleep(100);
			}
		}
	} // while

	//
	// Close the log file
	//
	if (pInfo->logData.GetLogOpen())
	{
		pInfo->logData.CloseFile();
		pInfo->logData.SetLogOpen(false);
	}

	// Close the serial port handle
	CloseHandle(hSerial);
	hSerial = NULL;

	CloseHandle(osReader.hEvent);

	pInfo->SetThreadAlive(false);

	return 0;

} // FieldMillSerial THREAD

// -----------------------------------------

// NEW NETWORK EFM THREAD

UINT CEFMDlg::FieldMillNetwork(LPVOID pParam)
{

	tTHREADINFO* pInfo = (tTHREADINFO*)pParam;
	char szData[1000];
	char efmstr[2000];
	int sentence_pos = 0;
	bool bConnected = false;
	struct timeval  tval;
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	int iPtr = 0;
	bool bDone = false;
	bool bStartup = true;

	TRACE("%s] FieldMillNetwork begin\n", config.m_szDataSourceName[pInfo->GetID()]);

	pInfo->SetThreadAlive(true);

	// TRY TO CONNECT
	int iRet = connect(pInfo->GetSocket(), (SOCKADDR*)&pInfo->connecttoAddr, sizeof(pInfo->connecttoAddr));
	if (iRet == SOCKET_ERROR)
	{
		// CAN'T CONNECT
		TRACE("%s] Unable to connect.\n", config.m_szDataSourceName[pInfo->GetID()]);
		// Grey out the graph and the LED digits
		((CEFMDlg*)pInfo->pMyOwner)->PostMessage(WM_SETCONTROLCOLOURS, (WPARAM)pInfo->GetID(), (LPARAM)COLOUR_DISABLED);
		// Construct and display the new title with the appropriate error message
		((CEFMDlg*)pInfo->pMyOwner)->PostMessage(MSG_NETWORKSOURCE_UNABLETOCONNECT, 0, (LPARAM)pInfo->GetID());
		bConnected = false;
	}
	else
	{
		// CONNECT OK
		TRACE("%s] Connected to source.\n", config.m_szDataSourceName[pInfo->GetID()]);
		// Green in the graph and the LED digits
		((CEFMDlg*)pInfo->pMyOwner)->PostMessage(WM_SETCONTROLCOLOURS, (WPARAM)pInfo->GetID(), (LPARAM)COLOUR_DISABLED);
		// Hide the status bar for this data source
		((CEFMDlg*)pInfo->pMyOwner)->PostMessage(MSG_STATUSBAR_HIDE, 0, (LPARAM)pInfo->GetID());
		// Construct and display the new title
		//CString newTitle = ((CEFMDlg*)pInfo->pMyOwner)->m_stripChart[pInfo->GetID()].GetCaption();
		CString newTitle = ((CEFMDlg*)pInfo->pMyOwner)->m_chartViewer[pInfo->GetID()].getCaption();
		//((CEFMDlg*)pInfo->pMyOwner)->m_stripChart[pInfo->GetID()].SetCaption(newTitle);
		((CEFMDlg*)pInfo->pMyOwner)->m_chartViewer[pInfo->GetID()].setCaption(newTitle);
		bConnected = true;
	}

	if (bConnected)
	{
		// CONNECTED - LOOP UNTIL EXIT TIME
		while (!pInfo->GetThreadExit())
		{
			int iBytesReceived = recv(pInfo->GetSocket(), (char*)szData, 14, 0);

			if (iBytesReceived > 0)
			{
				// READ NETWORK BYTES OK

				for (int iIdx = 0; iIdx < iBytesReceived; iIdx++)
				{
					// loop through received data looking for start characters
					if ((szData[iIdx] != '$') && (szData[iIdx] != '>') && (szData[iIdx] != '<'))
					{
						efmstr[sentence_pos] = szData[iIdx];
						sentence_pos++;
					} // if start char
					else
					{
						efmstr[sentence_pos] = '\0'; // terminate sentence
						// "efmstr[]" holds a sentence at this point

						// If data sharing is enabled, send the sentence value down the pipe to the
						// server thread.  To allow network clients to use their alarms on the 
						// network data, we need to send all data (10 times a second) over the 
						// socket to the client.
						//saFMData[pInfo->GetID()] = efmstr;	//sentence;
						if (config.m_BDataSharingEnabled[pInfo->GetID()])
						{
							for (iPtr = 0; iPtr < MAX_NETWORKCLIENTS; iPtr++)
							{
								if (pInfo->clientsConnected[iPtr].GetIsAlive())
								{
									pInfo->clientsConnected[iPtr].FifoAdd(efmstr);
								}
							}
						} // send to network clients

						if (!bStartup)
						{
							if (CEFMDlg::ProcessEFMSentence(efmstr, pInfo, &pInfo->logData) == false)
							{
								// There was a problem with the EFM sentence (malformed, out of sync, etc)
								// so ignore the next sentence until we can resync
								bStartup = true;
							}
						} // not startup
						else
						{
							bStartup = false; // ignore first sentence, could be partial
						}


						sentence_pos = 0; // start collecting at beginning again
						efmstr[sentence_pos++] = szData[iIdx]; // copy '$' from sentence
					} // else not start char
				} // for each char received
			} // if (iBytesReceived > 0)
			else
			{
				// FAILED TO READ NETWORK BYTES
				// iBytesReceived==0 for closed conx or -1 for failure
				TRACE("%s] failed to read from detector.\n", config.m_szDataSourceName[pInfo->GetID()]);
				closesocket(pInfo->GetSocket());
				// Signal the interface that we've lost our signal
				//((CEFMDlg*)pInfo->pMyOwner)->PostMessage(MSG_NETWORKSOURCE_SIGNALLOST, 0, (LPARAM)pInfo->GetID());
				pInfo->SetThreadExit();
			}

		} // while (!pInfo->GetThreadExit())
	} // if (bConnected)


	// ENDING THIS NETWORK CONNECTION THREAD

	// DISCONNECT CLIENTS
	for (iPtr = 0; iPtr < MAX_NETWORKCLIENTS; iPtr++)
	{
		if (pInfo->clientsConnected[iPtr].GetIsAlive())
		{
			SetEvent(pInfo->clientsConnected[iPtr].GetThreadExitHandle());
			while (pInfo->clientsConnected[iPtr].GetIsAlive())
			{
				// wait for shutdown
				TRACE("FieldMillNetwork() is shutting down client threads\n");
				Sleep(100);
			}
		}
	} // for (iPtr=0; iPtr<MAX_NETWORKCLIENTS; iPtr++)

	pInfo->SetThreadAlive(false);

	closesocket(pInfo->GetSocket());

	TRACE("%s] FieldMillNetwork end\n", config.m_szDataSourceName[pInfo->GetID()]);

	return 15;

} // NETWORK EFM THREAD

// -----------------------------------------

// EFA-20 THREAD

UINT CEFMDlg::FieldMillEFA20(LPVOID pParam)
{
	tTHREADINFO* pInfo = (tTHREADINFO*)pParam;
	FT_STATUS status;
	bool bDone = false;
	HANDLE m_eventRXChar;
	HANDLE pH[2];
	DWORD dwObj;
	DWORD RxBytes, TxBytes, EventDWord;
	DWORD dwBytesRead;
	unsigned char chData[1024];		// unsigned so the sign bit doesn't screw with us.
	char efmstr[2000];
	int sentence_pos = 0;
	int iPtr = 0;
	bool bStartup = true; // ignore the first sentence since it might be partial

	// Thread is active
	pInfo->SetThreadAlive(true);

	status = FT_OpenEx((PVOID)(LPCTSTR)pInfo->m_txtEFA20SerialNumber, FT_OPEN_BY_SERIAL_NUMBER, &pInfo->m_ftEFA20);
	if (status != FT_OK) { bDone = true; }
	status = FT_ResetDevice(pInfo->m_ftEFA20);
	if (status != FT_OK) { bDone = true; }
	status = FT_SetBaudRate(pInfo->m_ftEFA20, 9600);
	if (status != FT_OK) { bDone = true; }
	status = FT_SetDataCharacteristics(pInfo->m_ftEFA20, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
	if (status != FT_OK) { bDone = true; }
	status = FT_Purge(pInfo->m_ftEFA20, FT_PURGE_RX | FT_PURGE_TX);
	if (status != FT_OK) { bDone = true; }

	// Create the event for character reception
	m_eventRXChar = CreateEvent(NULL, FALSE, FALSE, NULL);
	// Create the event array
	pH[0] = pInfo->GetEFA20ThreadExit();
	pH[1] = m_eventRXChar;
	// Set event notification for character reception
	status = FT_SetEventNotification(pInfo->m_ftEFA20, FT_EVENT_RXCHAR, m_eventRXChar);
	if (status != FT_OK) { bDone = true; }

	//sentence = "$+02.26,0*CE\r\n";
	// 24 2B 30 32 2E 32 36 2C 30 2A

	while (!bDone)
	{
		// We can wait for INFINITE because we have a specific exiting event.
		dwObj = WaitForMultipleObjects(2, pH, FALSE, INFINITE);
		if (dwObj == WAIT_OBJECT_0 + 1)
		{
			// Received a character event
			// Check the status to see how many bytes we have.
			FT_GetStatus(pInfo->m_ftEFA20, &RxBytes, &TxBytes, &EventDWord);
			if (RxBytes > (sizeof(chData) - 20))
			{
				RxBytes = sizeof(chData) - 20; // 20 is arbitrary safety buffer
			}

			// Read the same number of bytes that GetStatus told us we had.
			status = FT_Read(pInfo->m_ftEFA20, chData, RxBytes, &dwBytesRead);
			if (status == FT_OK)
			{


				for (unsigned int iIdx = 0; iIdx < dwBytesRead; iIdx++)
				{
					if ((chData[iIdx] != '$') && (chData[iIdx] != '>') && (chData[iIdx] != '<'))
					{
						efmstr[sentence_pos] = chData[iIdx];
						sentence_pos++;
					}
					else
					{
						efmstr[sentence_pos] = '\0'; // terminate sentence
						// "efmstr[]" holds a sentence at this point

						// If data sharing is enabled, send the sentence value down the pipe to the
						// server thread.  To allow network clients to use their alarms on the 
						// network data, we need to send all data (10 times a second) over the 
						// socket to the client.
						//saFMData[pInfo->GetID()] = efmstr;	//sentence;
						if (config.m_BDataSharingEnabled[pInfo->GetID()])
						{
							for (iPtr = 0; iPtr < MAX_NETWORKCLIENTS; iPtr++)
							{
								if (pInfo->clientsConnected[iPtr].GetIsAlive())
								{
									pInfo->clientsConnected[iPtr].FifoAdd(efmstr);
								}
							}
						}

						if (!bStartup)
						{
							if (CEFMDlg::ProcessEFMSentence(efmstr, pInfo, &pInfo->logData) == false)
							{
								// There was a problem with the EFM sentence (malformed, out of sync, etc)
								// so ignore the next sentence until we can resync
								bStartup = true;
							}
						}
						else
						{
							bStartup = false; // ignore the first sentence since it might be partial
							// Hide the status bar for this data source, erase SIGNAL LOST message
							((CEFMDlg*)pInfo->pMyOwner)->PostMessage(MSG_STATUSBAR_HIDE, 0, (LPARAM)pInfo->GetID());
						}

						sentence_pos = 0; // start collecting at beginning again
						efmstr[sentence_pos++] = chData[iIdx]; // copy '$' from sentence
					}
				} //for (unsigned int iIdx=0; iIdx<dwBytesRead; iIdx++)
			} // if (status == FT_OK)
			else
			{
				// FT_IO_ERROR - suggests an error in the parameters of the function, or a 
				//				fatal error like a USB disconnect has occurred.

				TRACE("FT_IO_ERROR: %d\n", status);

				bDone = true;
			}
		}
		else
		{
			// Either we've been signaled to exit the thread, or something has gone wrong.
			// In either case, we want to exit the thread.
			bDone = true;
		}
	} // while (!bDone)

	// Since we're shutting down, disconnect any clients that may be connected
	for (iPtr = 0; iPtr < MAX_NETWORKCLIENTS; iPtr++)
	{
		if (pInfo->clientsConnected[iPtr].GetIsAlive())
		{
			SetEvent(pInfo->clientsConnected[iPtr].GetThreadExitHandle());
			while (pInfo->clientsConnected[iPtr].GetIsAlive())
			{
				TRACE("FieldMillEFA20() is shutting down client threads\n");
				Sleep(100);
			}
		}
	} // while

	//
	// Close the log file
	//
	if (pInfo->logData.GetLogOpen())
	{
		pInfo->logData.CloseFile();
		pInfo->logData.SetLogOpen(false);
	}

	status = FT_Purge(pInfo->m_ftEFA20, FT_PURGE_RX | FT_PURGE_TX);
	status = FT_Close(pInfo->m_ftEFA20);

	pInfo->SetThreadAlive(false);

	CloseHandle(m_eventRXChar);
	return 20;

} // EFA-20 THREAD


// -----------------------------------------


bool CEFMDlg::ProcessEFMSentence(CString sentence, tTHREADINFO* pI, CLog* lLog)
{
	TRACE("%d:[%s] attempting Lock\n", pI->iID, sentence);

	//	csPES.Lock();
	TRACE("%d locked\n", pI->iID);
	static CCriticalSection csPES;

	CSingleLock singleLock(&csPES, true);

	char cChecksum[2];
	//	char cDecimalChar[4];
	CString logLine;
	CString szFMR;
	double time;
	CString txt;
	bool bEverythingGood = false;


	// WE'VE GOT A DATA SENTENCE READY
	// Data ////////////
	//sentence = "$+02.26,0*CE\r\n";
	// 24 2B 30 32 2E 32 36 2C 30 2A
	////////////////////
	// Get the current time
	CTime current_time(CTime::GetCurrentTime());
	time = CTime_to_Double(current_time);

	// Make sure the sentence we're looking at is a real EFM sentence and is 
	// not malformed at all.
	if (CEFMDlg::ValidateEFMSentence(sentence) == true)
	{
		// Calculate the checksum to verify that the data hasn't been corrupted
		CString checksumCalculated = ((CEFMDlg*)pI->pMyOwner)->CalculateChecksum(sentence);
		CString checksumSentence = sentence.Mid(10, 2);
		if (!checksumCalculated.CompareNoCase(checksumSentence))
		{
			// PASSED CHECKSUM TEST
			//
			// Parse the sentence
			sentence = sentence.Right(sentence.GetLength() - 1);		//+02.26,0*CD
			// Depending on the user locale, we may need to change the decimal point into 
			// a comma.
//			GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_SDECIMAL,cDecimalChar,4);
//			if (cDecimalChar[0] == ',')
//			{
//				sentence.Replace('.',',');
//			}
			//-00.00
			txt = sentence.Left(6).Mid(1, 2);
			int a = atoi(txt);
			txt = sentence.Left(6).Mid(4, 2);
			int b = atoi(txt);
			double c = a + (b / 100.0);
			(sentence[0] == '-') ? c *= -1 : NULL;

			pI->dFieldMillReading = c;	//atof(sentence.Left(6));
			szFMR = sentence.Left(6);
#ifdef __100kV__
			pI->dFieldMillReading *= 5.0;
			if (pI->dFieldMillReading > 99.99) { pI->dFieldMillReading = 99.99; }
			else if (pI->dFieldMillReading < -99.99) { pI->dFieldMillReading = -99.99; }
			szFMR.Format("%+06.2f", pI->dFieldMillReading);
#endif
			sentence = sentence.Right(sentence.GetLength() - 7);		//0*CD
			pI->iRotorStatus = atoi(sentence.Left(1));
			sentence = sentence.Right(sentence.GetLength() - 2);		//CD
			cChecksum[0] = sentence.GetAt(0);
			cChecksum[1] = sentence.GetAt(1);

			// Set appropriate status flags for Rotor status and self test fault
			(pI->iRotorStatus & 0x01) ? pI->m_bRotorFault = true : pI->m_bRotorFault = false;
			(pI->iRotorStatus & 0x02) ? pI->m_bSelfTestFault = true : pI->m_bSelfTestFault = false;

			time = CTime_to_Double(current_time);
			if (time == pI->timelast) {
				pI->tenths += TENTHSMULTIPLIER;
				time += pI->tenths;
			}
			else {
				pI->tenths = 0;
				pI->timelast = time;
			}

			// Kill the redundant zero if there is one
			if (szFMR[1] == '0')
			{
				szFMR.Delete(1);
			}

			// Decrement the integration counter
			pI->iIntegrationValue--;
			if (pI->iIntegrationValue <= 0)
			{
				// Update the LED digits every second disregarding the graph
				// update knob.
				if (fabs(pI->dFieldMillReading) > 9.99)
				{
					pI->GetNumLED()->SetDigits(5);
				}
				else
				{
					pI->GetNumLED()->SetDigits(4);
				}
				pI->GetNumLED()->SetAlphaNumeric(szFMR);

				// Add the new value to the array of values
				//pI->values[(*pI->piGraphUpdateCounter)][0] = CTime_to_Double(current_time); // store this reading
				pI->values[(*pI->piGraphUpdateCounter)][0] = CTime_to_ChartDouble(); // store this reading
				pI->values[(*pI->piGraphUpdateCounter)][1] = pI->dFieldMillReading;					// store this reading

				pI->lightningvalues[(*pI->piGraphUpdateCounter)][0] = CTime_to_ChartDouble(); // store this reading
				int iDis = CEFMDlg::CheckForLightningAlarm(pI, time, pI->GetID(), pI->dFieldMillReading);
				if(iDis < 0 || iDis > 24)
					pI->lightningvalues[(*pI->piGraphUpdateCounter)][1] = Chart::NoValue;
				else
					pI->lightningvalues[(*pI->piGraphUpdateCounter)][1] = iDis;					// store this reading

				// Log the data (Logs twice a second - low resolution)
				if ((config.m_BEnableDataLogging[pI->GetID()] == TRUE) && (config.m_BHighResolutionLogging[pI->GetID()] == FALSE))
				{
					logLine.Format("%02d:%02d:%02d,%s,%d\n", current_time.GetHour(), current_time.GetMinute(), current_time.GetSecond(), szFMR, pI->iRotorStatus);
					// Only log if there's a log file open
					if (lLog->GetLogOpen())
					{
						lLog->Log(current_time, logLine);
					}
				}

				if ((*pI->piGraphUpdateCounter)++ >= (*pI->piGraphUpdatePeriod))
				{
					// Add the data to the graph
					////pI->GetStrip()->AddXY(0,time,pI->dFieldMillReading);
					//// Add the array of values to the graph
					//VARIANT varray;
					//SAFEARRAY *psafearray;
					//psafearray = (SAFEARRAY *) new char[sizeof(SAFEARRAY) + sizeof(SAFEARRAYBOUND)];
					//psafearray->cDims = 2;
					//psafearray->cbElements = 8;
					//psafearray->rgsabound[0].cElements = (*pI->piGraphUpdateCounter);
					//psafearray->rgsabound[0].lLbound = 0;
					//psafearray->rgsabound[1].cElements = 2;
					//psafearray->rgsabound[1].lLbound = 0;
					//psafearray->pvData = (void *) pI->values;
					//varray.vt = VT_ARRAY | VT_R8;
					//varray.parray = psafearray;
					//pI->GetStrip()->AddXYArray((short) 0, &varray);
					//delete psafearray;
					pI->GetChart()->addXYData(pI->values, *pI->piGraphUpdateCounter);
					//pI->GetLightningChart()->addXYData(pI->values, *pI->piGraphUpdateCounter);
					pI->GetLightningChart()->addXYData(pI->lightningvalues, *pI->piGraphUpdateCounter);
					(*pI->piGraphUpdateCounter) = 0;		// Reset the counter
				}

				// Check for alarms
				if (CEFMDlg::CheckForVeryHighFieldAlarm(fabs(pI->dFieldMillReading), pI))
				{
					pI->m_iVHFACounter = (int)(config.m_dVHFADuration[pI->GetID()] * 60);
				}
				else if (CEFMDlg::CheckForHighFieldAlarm(fabs(pI->dFieldMillReading), pI))
				{
					pI->m_iHFACounter = (int)(config.m_dHFADuration[pI->GetID()] * 60);
				}
				// Delta Alarm exists independantly of the other alarms
				if (CEFMDlg::CheckForDeltaAlarm(fabs(pI->dFieldMillReading), pI))
				{
					pI->m_iDACounter = (int)(config.m_dDADuration[pI->GetID()] * 60);
				}

				// Reset the integration value
				pI->iIntegrationValue = INTEGRATION_VALUE;
			} // End "if (pInfo->iIntegrationValue <= 0)"

			// Log the data (Logs twenty times a second - high resolution)
			if ((config.m_BEnableDataLogging[pI->GetID()] == TRUE) && (config.m_BHighResolutionLogging[pI->GetID()] == TRUE))
			{
				logLine.Format("%02d:%02d:%02d,%s,%d\n", current_time.GetHour(), current_time.GetMinute(), current_time.GetSecond(), szFMR, pI->iRotorStatus);
				// Only log if there's a log file open
				if (lLog->GetLogOpen())
				{
					lLog->Log(current_time, logLine);
				}
			}

			// We got a new valid reading, reset the timeout counter;
			pI->SetSignalLostCounter(0);
			if (pI->m_bSignalLost)
			{
				txt.Format("%s Signal Regained.", config.m_szDataSourceName[pI->GetID()]);
				m_EventLog.LogEvent(txt);
				// Email & SMS Fault
				if (config.bAlertsFaultsClear)
				{
					Alerts.SendAlerts(pI->GetID(), "Signal Regained. Data OK");
				}
				pI->m_bSignalLost = false;
			}

			// Check for Alarms
			int iDis = CEFMDlg::CheckForLightningAlarm(pI, time, pI->GetID(), pI->dFieldMillReading);
			if (iDis >= 0)
			{
				// We only want the lightning alarm to activate if it's detected within our range so compare
				// the distance we have with our configuration
				CString txtRings(config.m_szLARingRanges[pI->GetID()]);
				int iFurthestRange = atoi(txtRings.Right(txtRings.GetLength() - (txtRings.ReverseFind(',') + 1)));
				if (iDis < iFurthestRange)	// iFurthestRange contains the furthest range in our configuration
				{
					// The detected distance is within our outer-most range so we can activate the alarm
					pI->SetLightningDistance(iDis);

					txt.Format("%s Lightning Detected at %d %s.", config.m_szDataSourceName[pI->GetID()], pI->GetLightningDistance(),
						(config.m_iLAUnit[pI->GetID()] == MILES) ? "miles" : "km");
					m_EventLog.LogEvent(txt);

					// Activate the appropriate range ring
					((CEFMDlg*)pI->pMyOwner)->RRAdd(pI->GetID(), (unsigned int)pI->GetLightningDistance());

					pI->lightningvalues[(*pI->piGraphUpdateCounter)][0] = CTime_to_ChartDouble(); // store this reading
					pI->lightningvalues[(*pI->piGraphUpdateCounter)][1] = iDis;

					// Reset the lightning alarm counter
					pI->m_iLACounter = (int)(config.m_dLADuration[pI->GetID()] * 60);
					// Reset the repeat wav  interval counter
					pI->m_iLAWAVCounter = (int)(config.m_dLAWAVRepeatInterval[pI->GetID()] * 60.0);

					// Did Lightning Alarm just activate now?
					if (!pI->m_bLALastState)
					{
						// Alarm just activated - Log it
						txt.Format("%s Lightning Alarm Active.", config.m_szDataSourceName[pI->GetID()]);
						m_EventLog.LogEvent(txt);
						// Send Email & SMS
						if (config.bAlertsLAActive)
						{
							CString txt2;
							txt2.Format("Lightning Alarm Active\nLightning Detected at %d %s.",
								pI->GetLightningDistance(),
								(config.m_iLAUnit[pI->GetID()] == MILES) ? "miles" : "km");
							Alerts.SendAlerts(pI->GetID(), txt2);
						}
						// Play WAV Files
						pI->m_bPlayLAOnActivationWAV = true; // announce lightning detected
						pI->m_bPlayLADistanceWAV = true; // announce lightning distance
						pI->m_iClosestAnnouncedDistance = iDis; // remember that we announced this distance
						pI->m_iClosestAlertedDistance = iDis; // remember that we alerted this distance
						pI->m_bLALastState = true; // remember that lightning alarm is already active
					}
					else
					{
						// Lightning Alarm was active already
						// Send Email & SMS if new strike is approaching us
						if ((iDis < pI->m_iClosestAlertedDistance) && config.bAlertsLAActive)
						{
							CString txt2;
							txt2.Format("Lightning Alarm Active\nLightning Detected at %d %s.",
								pI->GetLightningDistance(),
								(config.m_iLAUnit[pI->GetID()] == MILES) ? "miles" : "km");
							Alerts.SendAlerts(pI->GetID(), txt2);
							pI->m_iClosestAlertedDistance = iDis; // remember that we alerted this distance
						}

						// Play WAV Files: Should we only announce closer strikes
						if (config.m_BLAOnlyAnnounceCloser[pI->GetID()])
						{
							// check of this strike closer than last announced strike
							if (iDis < pI->m_iClosestAnnouncedDistance)
							{
								// this strike is approacing our location (closer)
								pI->m_bPlayLADistanceWAV = true; // announce lightning distance
								pI->m_iClosestAnnouncedDistance = iDis; // remember that we announced this distance
							}
						}
						else
						{
							// We are announcing all strikes
							pI->m_bPlayLADistanceWAV = true; // announce lightning distance
						}
					}

				} // if (iDis < iFurthestRange)
			}
			else
			{
				// Alarm not active
				if (pI->m_bLALastState)
				{
				}
			}
		} // End "if (!checksumCalculated.CompareNoCase(checksumSentence))"
		else
		{
			// FAILED CHECKSUM TEST
			// Log the bad sentence to the events log
			sentence.Remove('\r');
			sentence.Remove('\n');
			txt.Format("[%s] Bad data sentence - failed checksum (%s)", config.m_szDataSourceName[pI->GetID()], sentence);
			//m_EventLog.LogEvent(txt);
		}
		// Processing Successful
		bEverythingGood = true;
	} // End if (CEFMDlg::ValidateEFMSentence(sentence) == true)
	else
	{
		// FAILED ValidateEFMSentence, could be a '<' or '>' self-test
		// Log the bad sentence to the events log
		sentence.Remove('\r');
		sentence.Remove('\n');
		if ((sentence[0] == '<') || (sentence[0] == '>'))
		{
			pI->EfmFirmwareVersion = sentence.Mid(8, 3); // f/w version is in self-test message

			// Self-test sentence
			txt.Format("[%s] EFM Self-Test (%s)", config.m_szDataSourceName[pI->GetID()], sentence);
			m_EventLog.LogEvent(txt);
			bEverythingGood = true;
		}
		else
		{
			// malformed sentence
			txt.Format("[%s] Bad data sentence - incorrect sentence structure (%s)", config.m_szDataSourceName[pI->GetID()], sentence);
			//m_EventLog.LogEvent(txt);
			bEverythingGood = false;
		}
	}

	TRACE("%d unlocked\n", pI->iID);

	return bEverythingGood;
}// ProcessEFMSentence(CString sentence, tTHREADINFO * pI, CLog * lLog)

void CEFMDlg::InitControls()
{
	int ctr;
	double dCapXPos, dCapYPos;

	dCapXPos = 0.0125;
	dCapYPos = 0.02;

	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		// Strip Chart Configuration
		//
		//m_stripChart[ctr].SetVariables(1); // show field, change in field, slope2 
		//m_stripChart[ctr].SetVariableID(1);
		//m_stripChart[ctr].SetVariableColor(RGB(255,128,64)); // orange
		//m_stripChart[ctr].SetVariableID(2);
		//m_stripChart[ctr].SetVariableColor(RGB(0,255,255)); // cyan
		//m_stripChart[ctr].SetVariableID(0);

		//m_stripChart[ctr].SetCaptions(1);
		//m_stripChart[ctr].SetCaptionX(dCapXPos);
		//m_stripChart[ctr].SetCaptionY(dCapYPos);
		//m_stripChart[ctr].SetCaption(config.m_szDataSourceName[ctr]);
		m_chartViewer[ctr].setCaption(config.m_szDataSourceName[ctr]);
		//m_stripChart[ctr].SetCaptionAlign(0);
		//m_stripChart[ctr].SetCaptionColor(RGB(0, 128, 255));
		m_chartViewer[ctr].setCaptionColor(RGB(0, 128, 255));
		//m_stripChart[ctr].SetVariableColor(COLOUR_DISABLED);
		m_chartViewer[ctr].setDataLineColor(COLOUR_DISABLED);
		CString txt = "Lightning distance";
		m_chartLightningDistanceViewer[ctr].setCaption(txt);
		m_chartLightningDistanceViewer[ctr].setCaptionColor(RGB(0, 128, 255));

		//m_stripChart[ctr].SetHandles(3); // handles on both axes
		//m_stripChart[ctr].SetXTicMode(1); // time_date mode
		//m_stripChart[ctr].SetLastX(CTime_to_Double(CTime::GetCurrentTime()));
		//m_stripChart[ctr].SetMaxBufferSize(60*60*24); // 1sample/sec * 60 sec/min * 60 min/hour * 24 hours
		//m_stripChart[ctr].SetTrackBackColor(RGB(0,0,0));
		//m_stripChart[ctr].SetBackColor(RGB(0,0,0));
		//m_stripChart[ctr].SetXTicColor(RGB(255,255,255));
		//m_stripChart[ctr].SetTrackTicColor(RGB(255,255,255));
		//m_stripChart[ctr].SetGridColor(RGB(128,128,128));
		//m_stripChart[ctr].SetOutlineWidth(0);
		//m_stripChart[ctr].SetBevelOuter(0);
		//m_stripChart[ctr].SetBevelInner(0);
		//m_stripChart[ctr].SetXMajorTics(20);
		//m_stripChart[ctr].SetXMinorTics(4);
		//m_stripChart[ctr].SetVariableFill(0);
		//m_stripChart[ctr].SetVariableLineWidth(2);
		//m_stripChart[ctr].SetTimeSecondsFormat("%H:%M:%S");
		//m_stripChart[ctr].SetTimeMinutesFormat("%H:%M");
		//m_stripChart[ctr].SetTimeHoursFormat("%H:%M");
		//m_stripChart[ctr].SetTimeDaysFormat("%m/%d/%y");
		//m_stripChart[ctr].SetTimeMonthsFormat("%b,%y");
		//m_stripChart[ctr].SetTimeYearsFormat("%Y");
		//m_stripChart[ctr].SetTimeDateFormat("%m/%d/%y"); // shows date along with recent data
		//m_stripChart[ctr].SetTimeDateFormat(""); // don't show date for today's data
		//m_stripChart[ctr].SetBorderWidth(0); 
		//m_stripChart[ctr].SetBevelInner(0);
		//m_stripChart[ctr].SetBevelOuter(0);
		//m_stripChart[ctr].SetBevelWidth(0);
		//m_stripChart[ctr].SetAreaTop(0.10);
		//m_stripChart[ctr].SetAreaBottom(0.85);
		//m_stripChart[ctr].SetAreaLeft(0.06);
		//m_stripChart[ctr].SetAreaRight(0.97);
		//m_stripChart[ctr].SetEnableTrackSizing(TRUE);
		//m_stripChart[ctr].SetXMajorTics(8);
		//m_stripChart[ctr].SetXMinorTics(5);

		// Library has DPI awareness, force this dpi or the size will not be correct
		m_chartViewer[ctr].setDPI(96);
		m_chartLightningDistanceViewer[ctr].setDPI(96);

		// Lightning Alarm Rings
		double dRings[32];		// We'll never have more than 32 rings
		CString txtRings(config.m_szLARingRanges[ctr]);
		int iRingCount = 0;
		char* tok = NULL;
		char* next_token = NULL;
		if (txtRings.GetLength() < 2)
		{
			config.m_szLARingRanges[ctr] = DEFAULT_RINGRANGES;
			txtRings = DEFAULT_RINGRANGES;
		}
		tok = strtok_s(txtRings.GetBuffer(txtRings.GetLength()), ",", &next_token);
		while (tok)
		{
			dRings[iRingCount] = atof(tok);
			iRingCount++;
			tok = strtok_s(NULL, ",", &next_token);
		}
		m_laLightningAlarm[ctr].SetDistanceRings(dRings, iRingCount);
		if (config.m_iLAUnit[ctr] == MILES)
		{
			m_laLightningAlarm[ctr].SetUnitText(MILES_TXT);
		}
		else if (config.m_iLAUnit[ctr] == KILOMETERS)
		{
			m_laLightningAlarm[ctr].SetUnitText(KILOMETERS_TXT);
		}
		else
		{
			m_laLightningAlarm[ctr].SetUnitText("");
		}
		m_laLightningAlarm[ctr].SetHighFieldAlarm(FALSE);
		m_laLightningAlarm[ctr].SetVeryHighFieldAlarm(FALSE);
		m_laLightningAlarm[ctr].SetLightningAlarm(FALSE, 0);
		m_laLightningAlarm[ctr].SetEnabled(false);

		//	m_stripChart[ctr].SetTrackMin(0);
		//	m_stripChart[ctr].SetTrackMax(4294967296);
		//	m_stripChart[ctr].SetTrackDisplayMin(0);
		//	m_stripChart[ctr].SetTrackDisplayMax(16384);

				// Numeric LEDs
				//
		m_numledFieldMillValue[ctr].SetBackColor(RGB(0, 0, 0)); // black gackground
		m_numledFieldMillValue[ctr].SetBevelInner(0);
		m_numledFieldMillValue[ctr].SetBevelOuter(0);
		m_numledFieldMillValue[ctr].SetBorderWidth(0);
		m_numledFieldMillValue[ctr].SetDecimalPosition(-5);
		m_numledFieldMillValue[ctr].SetDecimals(2);
		m_numledFieldMillValue[ctr].SetDecimalSize(7);
		m_numledFieldMillValue[ctr].SetBevelInner(0);
		m_numledFieldMillValue[ctr].SetDigits(4);
		m_numledFieldMillValue[ctr].SetDisplayMode(1); // alphanumeric
		m_numledFieldMillValue[ctr].SetFixedDecimal(true);
		m_numledFieldMillValue[ctr].SetItalicsOffset(0);
		m_numledFieldMillValue[ctr].SetLeadingOneDigit(true);
		m_numledFieldMillValue[ctr].SetLeadingPlusMinus(false);
		m_numledFieldMillValue[ctr].SetLeadingZeros(false);
		m_numledFieldMillValue[ctr].SetOffColor(RGB(0, 0, 0));
		m_numledFieldMillValue[ctr].SetOnColor(COLOUR_DISABLED);	//(RGB(128,128,128)); 
		m_numledFieldMillValue[ctr].SetOutline(false);
		m_numledFieldMillValue[ctr].SetSpacingHorizontal(9);
		m_numledFieldMillValue[ctr].SetSpacingVertical(4);
		m_numledFieldMillValue[ctr].SetAlphaNumeric("0.00");


		m_knobScaleSelector.SetSelections(9);
		// Add 8
		m_knobScaleSelector.SetSelectionID(8);
		m_knobScaleSelector.SetSelectionCaption("AUTO");
		m_knobScaleSelector.SetSelectionColor(RGB(128, 128, 128));
		m_knobScaleSelector.SetSelectionOffsetX(0);
		m_knobScaleSelector.SetSelectionOffsetY(0.3);

		// Modify 3
		m_knobScaleSelector.SetSelectionID(3);
		m_knobScaleSelector.SetSelectionOffsetX(0.5);
		m_knobScaleSelector.SetSelectionOffsetY(-0.3);
		m_knobScaleSelector.SetSelectionRadius(0.8);

		// Modify 4
		m_knobScaleSelector.SetSelectionID(4);
		m_knobScaleSelector.SetSelectionOffsetX(-0.5);
		m_knobScaleSelector.SetSelectionOffsetY(-0.3);
		m_knobScaleSelector.SetSelectionRadius(0.8);

		// Modify 7
		m_knobScaleSelector.SetSelectionID(7);
		m_knobScaleSelector.SetSelectionOffsetX(-0.2);
		m_knobScaleSelector.SetSelectionOffsetY(0);
		m_knobScaleSelector.SetSelectionRadius(1);

		// Knobbies
		//
#ifdef __100kV__
		m_knobScaleSelector.SetSelections(10);
		// Add 8 & 9
		m_knobScaleSelector.SetSelectionID(8);
		m_knobScaleSelector.SetSelectionCaption("50 kV/m");
		m_knobScaleSelector.SetSelectionColor(RGB(128, 128, 128));
		m_knobScaleSelector.SetSelectionAngle(325.00);
		m_knobScaleSelector.SetSelectionOffsetX(-0.17);
		m_knobScaleSelector.SetSelectionOffsetY(0.01);
		m_knobScaleSelector.SetSelectionRadius(1.07);
		m_knobScaleSelector.SetSelectionID(9);
		m_knobScaleSelector.SetSelectionCaption("100 kV/m");
		m_knobScaleSelector.SetSelectionColor(RGB(128, 128, 128));
		m_knobScaleSelector.SetSelectionAngle(360.00);
		m_knobScaleSelector.SetSelectionOffsetX(0.22);
		m_knobScaleSelector.SetSelectionOffsetY(0.29);
		m_knobScaleSelector.SetSelectionRadius(0.77);
		// Modify 7
		m_knobScaleSelector.SetSelectionID(7);
		m_knobScaleSelector.SetSelectionAngle(290.00);
		m_knobScaleSelector.SetSelectionOffsetX(-0.19);
		m_knobScaleSelector.SetSelectionOffsetY(0.01);
		m_knobScaleSelector.SetSelectionRadius(0.83);
#endif

	} // End "for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)"
	m_chartViewer[0].setDataLineColor(RGB(0, 255, 255));
	m_chartViewer[1].setDataLineColor(RGB(0, 0, 255));

}

//
// |------------------|
// |                  |
// |      Charts      |
// |                  |
// |                  |
// |------------------| Visual
// |------------------|   Alarms
// |                  | Knobs & NumLEDs
// |------------------|
void CEFMDlg::LayoutControls()
{
	double dOverallChartHeight, dVisualAlarmHeight, dKnobLEDHeight, dLightningAlarmWidth;
	int ctr, iActiveFeeds, iControlID;
	int iHeight;		// Temp Variables

	const int iFontScaleDivider = 20;

	// These three value should equal 1.0
	dOverallChartHeight = 0.77;
	dVisualAlarmHeight = 0.05;
	dKnobLEDHeight = 0.18;

	dLightningAlarmWidth = 0.25;		// 25%

	iControlID = 0;
	iHeight = 0;

	// Get the size of the dialog box
	GetClientRect(&m_rectWindow);

	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		m_chartViewer[ctr].setHFASetPoint(config.m_dHFASetpoint[ctr]);
		m_chartViewer[ctr].setShowColorLineHFA(config.m_BShowColorLineHFA[ctr]);
		m_chartViewer[ctr].setVHFASetPoint(config.m_dVHFASetpoint[ctr]);
		m_chartViewer[ctr].setShowColorLineVHFA(config.m_BShowColorLineVHFA[ctr]);
	}

	// Find the number of data sources that are active
	iActiveFeeds = 0;
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		if (config.m_BDataSourceEnabled[ctr] == TRUE)
		{
			iActiveFeeds++;
		}
	}

	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		switch (iActiveFeeds)
		{
		case 0:	// No sources are active
			if (ctr == 0)
			{
				int iCH, iCW, iLAH, iLAW;

				if (config.m_BEnableLA[ctr])
				{
					iCH = (int)(m_rectWindow.Height() * dOverallChartHeight);
					iCW = (int)(m_rectWindow.Width() * (1.0 - dLightningAlarmWidth));
					iLAH = iCH;
					iLAW = (int)(m_rectWindow.Width() * dLightningAlarmWidth);
				}
				else
				{
					iCH = (int)(m_rectWindow.Height() * dOverallChartHeight);
					iCW = (int)(m_rectWindow.Width());
					iLAH = 0;
					iLAW = 0;
				}

				// StripChart
				//m_stripChart[ctr].MoveWindow(0,0,iCW,iCH);
				//m_stripChart[ctr].ShowWindow(TRUE);
				m_chartViewer[ctr].MoveWindow(0, 0, iCW, iCH);
				m_chartViewer[ctr].setSize(iCW, iCH);
				m_chartViewer[ctr].ShowWindow(TRUE);
				// Lightning Alarm
				m_laLightningAlarm[ctr].MoveWindow(iCW, 0, iLAW, iLAH);
				m_laLightningAlarm[ctr].ShowWindow(TRUE);

				// Visual Alarms
				int iVisualBar_Height = (int)(m_rectWindow.Height() * dVisualAlarmHeight);
				int iVisualBar_YPos = (int)(m_rectWindow.Height() * dOverallChartHeight);
				int iMuteSound_Height = iVisualBar_Height;
				int iMuteSound_Width = iVisualBar_Height;
				int iMuteSound_XPos = (int)(m_rectWindow.right - iMuteSound_Width) - MUTESOUND_BORDEROFFSET;
				int iHighField_Height = iVisualBar_Height;
				int iHighField_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
				int iHighField_XPos = 0;
				int iVeryHighField_Height = iVisualBar_Height;
				int iVeryHighField_Width = iHighField_Width;
				int iVeryHighField_XPos = 0;
				int iDelta_Height = iVisualBar_Height;
				int iDelta_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
				int iDelta_XPos = iHighField_Width;
				int iLightning_Height = iVisualBar_Height;
				int iLightning_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
				int iLightning_XPos = iHighField_Width + iDelta_Width;

				m_pctHighFieldVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width, iHighField_Height);
				m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
				m_pctHighFieldVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width) / iFontScaleDivider);
				m_pctVeryHighFieldVisual[ctr].MoveWindow(iVeryHighField_XPos, iVisualBar_YPos, iVeryHighField_Width, iVeryHighField_Height);
				m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
				m_pctVeryHighFieldVisual[ctr].SetFontSize(static_cast<float>(iVeryHighField_Width) / iFontScaleDivider);
				m_pctDeltaVisual[ctr].MoveWindow(iDelta_XPos, iVisualBar_YPos, iDelta_Width, iDelta_Height);
				m_pctDeltaVisual[ctr].ShowWindow(FALSE);
				m_pctDeltaVisual[ctr].SetFontSize(static_cast<float>(iDelta_Width) / iFontScaleDivider);
				m_pctLightningVisual[ctr].MoveWindow(iLightning_XPos, iVisualBar_YPos, iLightning_Width, iLightning_Height);
				m_pctLightningVisual[ctr].ShowWindow(FALSE);
				m_pctLightningVisual[ctr].SetFontSize(static_cast<float>(iLightning_Width) / iFontScaleDivider);
				m_pctStatusVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width + iDelta_Width + iLightning_Width, iHighField_Height);
				m_pctStatusVisual[ctr].ShowWindow(FALSE);
				m_pctStatusVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width + iDelta_Width + iLightning_Width) / iFontScaleDivider);
				// Sound Muter
				m_tglMuteSound[ctr].MoveWindow(iMuteSound_XPos, iVisualBar_YPos, iMuteSound_Width, iMuteSound_Height);
				m_tglMuteSound[ctr].ShowWindow(TRUE);

				// Knobs
				m_knobScaleSelector.MoveWindow(0, (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() * dKnobLEDHeight));
				m_knobTimeSelector.MoveWindow((int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() * dKnobLEDHeight));
				// Numeric LEDs
				m_numledFieldMillValue[ctr].MoveWindow((int)(m_rectWindow.Width() * 0.60), (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)(m_rectWindow.Width() * 0.40), (int)(m_rectWindow.Height() * dKnobLEDHeight));					m_numledFieldMillValue[ctr].ShowWindow(TRUE);
			}
			else
			{
				//m_stripChart[ctr].ShowWindow(FALSE);
				m_chartViewer[ctr].ShowWindow(FALSE);
				m_chartLightningDistanceViewer[ctr].ShowWindow(FALSE);
				m_pctLightningVisual[ctr].ShowWindow(FALSE);
				m_pctDeltaVisual[ctr].ShowWindow(FALSE);
				m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
				m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
				m_pctStatusVisual[ctr].ShowWindow(FALSE);
				m_numledFieldMillValue[ctr].ShowWindow(FALSE);
				m_tglMuteSound[ctr].ShowWindow(FALSE);
				m_laLightningAlarm[ctr].ShowWindow(FALSE);
			}
			break;
		case 1:	// One source is active
			if (config.m_BDataSourceEnabled[ctr] == TRUE)
			{
				int iCH, iCW, iLAH, iLAW;

				if (config.m_BEnableLA[ctr])
				{
					iCH = (int)(m_rectWindow.Height() * dOverallChartHeight);
					iCW = (int)(m_rectWindow.Width() * (1.0 - dLightningAlarmWidth));
					iLAH = iCH;
					iLAW = (int)(m_rectWindow.Width() * dLightningAlarmWidth);
				}
				else
				{
					iCH = (int)(m_rectWindow.Height() * dOverallChartHeight);
					iCW = (int)(m_rectWindow.Width());
					iLAH = 0;
					iLAW = 0;
				}

				// StripChart
				//m_stripChart[ctr].MoveWindow(0, 0, iCW, iCH);
				//m_stripChart[ctr].ShowWindow(TRUE);
				m_chartViewer[ctr].MoveWindow(0, 0, iCW, iCH);
				if (config.m_BShowChartLightningDistance[ctr] == TRUE)
					m_chartViewer[ctr].setSize(iCW / 2, iCH);
				else
					m_chartViewer[ctr].setSize(iCW, iCH);
				m_chartViewer[ctr].ShowWindow(TRUE);
				m_chartLightningDistanceViewer[ctr].MoveWindow(iCW / 2, 0, iCW, iCH);
				m_chartLightningDistanceViewer[ctr].setSize(iCW / 2, iCH);
				m_chartLightningDistanceViewer[ctr].ShowWindow(config.m_BShowChartLightningDistance[ctr]);
				// Lightning Alarm
				m_laLightningAlarm[ctr].MoveWindow(iCW, 0, iLAW, iLAH);
				m_laLightningAlarm[ctr].ShowWindow(TRUE);

				// Visual Alarms
				int iVisualBar_Height = (int)(m_rectWindow.Height() * dVisualAlarmHeight);
				int iVisualBar_YPos = (int)(m_rectWindow.Height() * dOverallChartHeight);
				int iMuteSound_Height = iVisualBar_Height;
				int iMuteSound_Width = iVisualBar_Height;
				int iMuteSound_XPos = (int)(m_rectWindow.right - iMuteSound_Width) - MUTESOUND_BORDEROFFSET;
				int iHighField_Height = iVisualBar_Height;
				int iHighField_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
				int iHighField_XPos = 0;
				int iVeryHighField_Height = iVisualBar_Height;
				int iVeryHighField_Width = iHighField_Width;
				int iVeryHighField_XPos = 0;
				int iDelta_Height = iVisualBar_Height;
				int iDelta_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
				int iDelta_XPos = iHighField_Width;
				int iLightning_Height = iVisualBar_Height;
				int iLightning_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
				int iLightning_XPos = iHighField_Width + iDelta_Width;

				m_pctHighFieldVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width, iHighField_Height);
				m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
				m_pctHighFieldVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width) / iFontScaleDivider);
				m_pctVeryHighFieldVisual[ctr].MoveWindow(iVeryHighField_XPos, iVisualBar_YPos, iVeryHighField_Width, iVeryHighField_Height);
				m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
				m_pctVeryHighFieldVisual[ctr].SetFontSize(static_cast<float>(iVeryHighField_Width) / iFontScaleDivider);
				m_pctDeltaVisual[ctr].MoveWindow(iDelta_XPos, iVisualBar_YPos, iDelta_Width, iDelta_Height);
				m_pctDeltaVisual[ctr].ShowWindow(FALSE);
				m_pctDeltaVisual[ctr].SetFontSize(static_cast<float>(iDelta_Width) / iFontScaleDivider);
				m_pctLightningVisual[ctr].MoveWindow(iLightning_XPos, iVisualBar_YPos, iLightning_Width, iLightning_Height);
				m_pctLightningVisual[ctr].ShowWindow(FALSE);
				m_pctLightningVisual[ctr].SetFontSize(static_cast<float>(iLightning_Width) / iFontScaleDivider);
				m_pctStatusVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width + iDelta_Width + iLightning_Width, iHighField_Height);
				m_pctStatusVisual[ctr].ShowWindow(FALSE);
				m_pctStatusVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width + iDelta_Width + iLightning_Width) / iFontScaleDivider);
				// Sound Muter
				m_tglMuteSound[ctr].MoveWindow(iMuteSound_XPos, iVisualBar_YPos, iMuteSound_Width, iMuteSound_Height);
				m_tglMuteSound[ctr].ShowWindow(TRUE);

				// Knobs
				m_knobScaleSelector.MoveWindow(0, (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() * dKnobLEDHeight));
				m_knobTimeSelector.MoveWindow((int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() * dKnobLEDHeight));
				// Numeric LEDs
				m_numledFieldMillValue[ctr].MoveWindow((int)(m_rectWindow.Width() * 0.60), (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)(m_rectWindow.Width() * 0.40), (int)(m_rectWindow.Height() * dKnobLEDHeight));					m_numledFieldMillValue[ctr].ShowWindow(TRUE);
			}
			else
			{
				//m_stripChart[ctr].ShowWindow(FALSE);
				m_chartViewer[ctr].ShowWindow(FALSE);
				m_chartLightningDistanceViewer[ctr].ShowWindow(FALSE);
				m_pctLightningVisual[ctr].ShowWindow(FALSE);
				m_pctDeltaVisual[ctr].ShowWindow(FALSE);
				m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
				m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
				m_pctStatusVisual[ctr].ShowWindow(FALSE);
				m_numledFieldMillValue[ctr].ShowWindow(FALSE);
				m_tglMuteSound[ctr].ShowWindow(FALSE);
				m_laLightningAlarm[ctr].ShowWindow(FALSE);
			}
			break;
		case 2:	// Two sources are active
			if (config.m_BDataSourceEnabled[ctr] == TRUE)
			{
				int iCH, iCW, iLAH, iLAW;
				if (iControlID == 0)
				{
					// First Control
					//
					if (config.m_BEnableLA[ctr])
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)(m_rectWindow.Width() * (1.0 - dLightningAlarmWidth));
						iLAH = iCH;
						iLAW = (int)(m_rectWindow.Width() * dLightningAlarmWidth);
					}
					else
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)(m_rectWindow.Width());
						iLAH = 0;
						iLAW = 0;
					}

					// StripChart
					//m_stripChart[ctr].MoveWindow(0,0,iCW,iCH);
					//m_stripChart[ctr].ShowWindow(TRUE);
					//m_chartViewer[ctr].MoveWindow(0, 0, iCW, iCH);
					//m_chartViewer[ctr].setSize(iCW, iCH);
					//m_chartViewer[ctr].ShowWindow(TRUE);
					m_chartViewer[ctr].MoveWindow(0, 0, iCW, iCH);
					if (config.m_BShowChartLightningDistance[ctr] == TRUE)
						m_chartViewer[ctr].setSize(iCW / 2, iCH);
					else
						m_chartViewer[ctr].setSize(iCW, iCH);
					m_chartViewer[ctr].ShowWindow(TRUE);
					m_chartLightningDistanceViewer[ctr].MoveWindow(iCW / 2, 0, iCW, iCH);
					m_chartLightningDistanceViewer[ctr].setSize(iCW / 2, iCH);
					m_chartLightningDistanceViewer[ctr].ShowWindow(config.m_BShowChartLightningDistance[ctr]);

					// Lightning Alarm
					m_laLightningAlarm[ctr].MoveWindow(iCW, 0, iLAW, iLAH);
					m_laLightningAlarm[ctr].ShowWindow(TRUE);

					// StripChart
					//iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
					//m_stripChart[ctr].MoveWindow(0,0,m_rectWindow.Width(),iHeight);
					//m_stripChart[ctr].ShowWindow(TRUE);

					// Visual Alarms
					int iVisualBar_Height = (int)(m_rectWindow.Height() * dVisualAlarmHeight);
					int iVisualBar_YPos = iHeight;
					int iMuteSound_Height = iVisualBar_Height;
					int iMuteSound_Width = iVisualBar_Height;
					int iMuteSound_XPos = (int)(m_rectWindow.right - iMuteSound_Width) - MUTESOUND_BORDEROFFSET;
					int iHighField_Height = iVisualBar_Height;
					int iHighField_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iHighField_XPos = 0;
					int iVeryHighField_Height = iVisualBar_Height;
					int iVeryHighField_Width = iHighField_Width;
					int iVeryHighField_XPos = 0;
					int iDelta_Height = iVisualBar_Height;
					int iDelta_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iDelta_XPos = iHighField_Width;
					int iLightning_Height = iVisualBar_Height;
					int iLightning_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iLightning_XPos = iHighField_Width + iDelta_Width;

					m_pctHighFieldVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width, iHighField_Height);
					m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctHighFieldVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width) / iFontScaleDivider);
					m_pctVeryHighFieldVisual[ctr].MoveWindow(iVeryHighField_XPos, iVisualBar_YPos, iVeryHighField_Width, iVeryHighField_Height);
					m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctVeryHighFieldVisual[ctr].SetFontSize(static_cast<float>(iVeryHighField_Width) / iFontScaleDivider);
					m_pctDeltaVisual[ctr].MoveWindow(iDelta_XPos, iVisualBar_YPos, iDelta_Width, iDelta_Height);
					m_pctDeltaVisual[ctr].ShowWindow(FALSE);
					m_pctDeltaVisual[ctr].SetFontSize(static_cast<float>(iDelta_Width) / iFontScaleDivider);
					m_pctLightningVisual[ctr].MoveWindow(iLightning_XPos, iVisualBar_YPos, iLightning_Width, iLightning_Height);
					m_pctLightningVisual[ctr].ShowWindow(FALSE);
					m_pctLightningVisual[ctr].SetFontSize(static_cast<float>(iLightning_Width) / iFontScaleDivider);
					m_pctStatusVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width + iDelta_Width + iLightning_Width, iHighField_Height);
					m_pctStatusVisual[ctr].ShowWindow(FALSE);
					m_pctStatusVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width + iDelta_Width + iLightning_Width) / iFontScaleDivider);
					// Sound Muter
					m_tglMuteSound[ctr].MoveWindow(iMuteSound_XPos, iVisualBar_YPos, iMuteSound_Width, iMuteSound_Height);
					m_tglMuteSound[ctr].ShowWindow(TRUE);

					// Numeric LEDs
					m_numledFieldMillValue[ctr].MoveWindow((int)(m_rectWindow.Width() * 0.60), (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)(m_rectWindow.Width() * 0.40), (int)((m_rectWindow.Height() * dKnobLEDHeight) / 2));
					m_numledFieldMillValue[ctr].ShowWindow(TRUE);
					iControlID++;
				}
				else
				{
					// Second Control
					//
					if (config.m_BEnableLA[ctr])
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)(m_rectWindow.Width() * (1 - dLightningAlarmWidth));
						iLAH = iCH;
						iLAW = (int)(m_rectWindow.Width() * dLightningAlarmWidth);
					}
					else
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)(m_rectWindow.Width());
						iLAH = 0;
						iLAW = 0;
					}

					// StripChart
					//m_stripChart[ctr].MoveWindow(0,(int)(iCH+(m_rectWindow.Height()*dVisualAlarmHeight)),iCW,iCH);
					//m_stripChart[ctr].ShowWindow(TRUE);
					m_chartViewer[ctr].MoveWindow(0, (int)(iCH + (m_rectWindow.Height() * dVisualAlarmHeight)), iCW, iCH);
					if (config.m_BShowChartLightningDistance[ctr] == TRUE)
						m_chartViewer[ctr].setSize(iCW / 2, iCH);
					else
						m_chartViewer[ctr].setSize(iCW, iCH);
					m_chartViewer[ctr].ShowWindow(TRUE);
					m_chartLightningDistanceViewer[ctr].MoveWindow(iCW / 2, (int)(iCH + (m_rectWindow.Height() * dVisualAlarmHeight)), iCW / 2, iCH);
					m_chartLightningDistanceViewer[ctr].setSize(iCW / 2, iCH);
					m_chartLightningDistanceViewer[ctr].ShowWindow(config.m_BShowChartLightningDistance[ctr]);
					// Lightning Alarm
					m_laLightningAlarm[ctr].MoveWindow(iCW, (int)(iCH + (m_rectWindow.Height() * dVisualAlarmHeight)), iLAW, iLAH);
					m_laLightningAlarm[ctr].ShowWindow(TRUE);

					// StripChart
					//iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
					//m_stripChart[ctr].MoveWindow(0,(int)(iHeight+(m_rectWindow.Height()*dVisualAlarmHeight)),m_rectWindow.Width(),iHeight);
					//m_stripChart[ctr].ShowWindow(TRUE);

					// Visual Alarms
					int iVisualBar_Height = (int)(m_rectWindow.Height() * dVisualAlarmHeight);
					int iVisualBar_YPos = (int)((iHeight + (m_rectWindow.Height() * dVisualAlarmHeight)) + iHeight);
					int iMuteSound_Height = iVisualBar_Height;
					int iMuteSound_Width = iVisualBar_Height;
					int iMuteSound_XPos = (int)(m_rectWindow.right - iMuteSound_Width) - MUTESOUND_BORDEROFFSET;
					int iHighField_Height = iVisualBar_Height;
					int iHighField_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iHighField_XPos = 0;
					int iVeryHighField_Height = iVisualBar_Height;
					int iVeryHighField_Width = iHighField_Width;
					int iVeryHighField_XPos = 0;
					int iDelta_Height = iVisualBar_Height;
					int iDelta_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iDelta_XPos = iHighField_Width;
					int iLightning_Height = iVisualBar_Height;
					int iLightning_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iLightning_XPos = iHighField_Width + iDelta_Width;

					m_pctHighFieldVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width, iHighField_Height);
					m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctHighFieldVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width) / iFontScaleDivider);
					m_pctVeryHighFieldVisual[ctr].MoveWindow(iVeryHighField_XPos, iVisualBar_YPos, iVeryHighField_Width, iVeryHighField_Height);
					m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctVeryHighFieldVisual[ctr].SetFontSize(static_cast<float>(iVeryHighField_Width) / iFontScaleDivider);
					m_pctDeltaVisual[ctr].MoveWindow(iDelta_XPos, iVisualBar_YPos, iDelta_Width, iDelta_Height);
					m_pctDeltaVisual[ctr].ShowWindow(FALSE);
					m_pctDeltaVisual[ctr].SetFontSize(static_cast<float>(iDelta_Width) / iFontScaleDivider);
					m_pctLightningVisual[ctr].MoveWindow(iLightning_XPos, iVisualBar_YPos, iLightning_Width, iLightning_Height);
					m_pctLightningVisual[ctr].ShowWindow(FALSE);
					m_pctLightningVisual[ctr].SetFontSize(static_cast<float>(iLightning_Width) / iFontScaleDivider);
					m_pctStatusVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width + iDelta_Width + iLightning_Width, iHighField_Height);
					m_pctStatusVisual[ctr].ShowWindow(FALSE);
					m_pctStatusVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width + iDelta_Width + iLightning_Width) / iFontScaleDivider);
					// Sound Muter
					m_tglMuteSound[ctr].MoveWindow(iMuteSound_XPos, iVisualBar_YPos, iMuteSound_Width, iMuteSound_Height);
					m_tglMuteSound[ctr].ShowWindow(TRUE);

					// Numeric LEDs
					m_numledFieldMillValue[ctr].MoveWindow((int)(m_rectWindow.Width() * 0.60), (int)((m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)) + ((m_rectWindow.Height() * dKnobLEDHeight) / 2)), (int)(m_rectWindow.Width() * 0.40), (int)((m_rectWindow.Height() * dKnobLEDHeight) / 2));
					m_numledFieldMillValue[ctr].ShowWindow(TRUE);
				}
				// Knobs
				m_knobScaleSelector.MoveWindow(0, (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() * dKnobLEDHeight));
				m_knobTimeSelector.MoveWindow((int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() * dKnobLEDHeight));
			}
			else
			{
				//m_stripChart[ctr].ShowWindow(FALSE);
				m_chartViewer[ctr].ShowWindow(FALSE);
				m_chartLightningDistanceViewer[ctr].ShowWindow(FALSE);
				m_pctLightningVisual[ctr].ShowWindow(FALSE);
				m_pctDeltaVisual[ctr].ShowWindow(FALSE);
				m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
				m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
				m_pctStatusVisual[ctr].ShowWindow(FALSE);
				m_numledFieldMillValue[ctr].ShowWindow(FALSE);
				m_tglMuteSound[ctr].ShowWindow(FALSE);
				m_laLightningAlarm[ctr].ShowWindow(FALSE);
			}
			break;
		case 3:	// Three sources are active
			if (config.m_BDataSourceEnabled[ctr] == TRUE)
			{
				int iCH, iCW, iLAH, iLAW;
				if (iControlID == 0)
				{
					// First Control
					//
					if (config.m_BEnableLA[ctr])
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)(m_rectWindow.Width() * (1 - dLightningAlarmWidth));
						iLAH = iCH;
						iLAW = (int)(m_rectWindow.Width() * dLightningAlarmWidth);
					}
					else
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)m_rectWindow.Width();
						iLAH = 0;
						iLAW = 0;
					}

					// StripChart
					//m_stripChart[ctr].MoveWindow(0,0,iCW,iCH);
					//m_stripChart[ctr].ShowWindow(TRUE);
					m_chartViewer[ctr].MoveWindow(0, 0, iCW, iCH);
					if (config.m_BShowChartLightningDistance[ctr] == TRUE)
						m_chartViewer[ctr].setSize(iCW / 2, iCH);
					else
						m_chartViewer[ctr].setSize(iCW, iCH);
					m_chartViewer[ctr].ShowWindow(TRUE);
					m_chartLightningDistanceViewer[ctr].MoveWindow(iCW / 2, 0, iCW, iCH);
					m_chartLightningDistanceViewer[ctr].setSize(iCW / 2, iCH);
					m_chartLightningDistanceViewer[ctr].ShowWindow(config.m_BShowChartLightningDistance[ctr]);
					// Lightning Alarm
					m_laLightningAlarm[ctr].MoveWindow(iCW, 0, iLAW, iLAH);
					m_laLightningAlarm[ctr].ShowWindow(TRUE);

					// StripChart
					//iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
					//m_stripChart[ctr].MoveWindow(0,0,m_rectWindow.Width(),iHeight);
					//m_stripChart[ctr].ShowWindow(TRUE);

					// Visual Alarms
					int iVisualBar_Height = (int)(m_rectWindow.Height() * dVisualAlarmHeight);
					int iVisualBar_YPos = iHeight;
					int iMuteSound_Height = iVisualBar_Height;
					int iMuteSound_Width = iVisualBar_Height;
					int iMuteSound_XPos = (int)(m_rectWindow.right - iMuteSound_Width) - MUTESOUND_BORDEROFFSET;
					int iHighField_Height = iVisualBar_Height;
					int iHighField_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iHighField_XPos = 0;
					int iVeryHighField_Height = iVisualBar_Height;
					int iVeryHighField_Width = iHighField_Width;
					int iVeryHighField_XPos = 0;
					int iDelta_Height = iVisualBar_Height;
					int iDelta_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iDelta_XPos = iHighField_Width;
					int iLightning_Height = iVisualBar_Height;
					int iLightning_Width = (int)((m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iLightning_XPos = iHighField_Width + iDelta_Width;

					m_pctHighFieldVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width, iHighField_Height);
					m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctHighFieldVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width) / iFontScaleDivider);
					m_pctVeryHighFieldVisual[ctr].MoveWindow(iVeryHighField_XPos, iVisualBar_YPos, iVeryHighField_Width, iVeryHighField_Height);
					m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctVeryHighFieldVisual[ctr].SetFontSize(static_cast<float>(iVeryHighField_Width) / iFontScaleDivider);
					m_pctDeltaVisual[ctr].MoveWindow(iDelta_XPos, iVisualBar_YPos, iDelta_Width, iDelta_Height);
					m_pctDeltaVisual[ctr].ShowWindow(FALSE);
					m_pctDeltaVisual[ctr].SetFontSize(static_cast<float>(iDelta_Width) / iFontScaleDivider);
					m_pctLightningVisual[ctr].MoveWindow(iLightning_XPos, iVisualBar_YPos, iLightning_Width, iLightning_Height);
					m_pctLightningVisual[ctr].ShowWindow(FALSE);
					m_pctLightningVisual[ctr].SetFontSize(static_cast<float>(iLightning_Width) / iFontScaleDivider);
					m_pctStatusVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width + iDelta_Width + iLightning_Width, iHighField_Height);
					m_pctStatusVisual[ctr].ShowWindow(FALSE);
					m_pctStatusVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width + iDelta_Width + iLightning_Width) / iFontScaleDivider);
					// Sound Muter
					m_tglMuteSound[ctr].MoveWindow(iMuteSound_XPos, iVisualBar_YPos, iMuteSound_Width, iMuteSound_Height);
					m_tglMuteSound[ctr].ShowWindow(TRUE);

					// Numeric LEDs
					m_numledFieldMillValue[ctr].MoveWindow((int)(m_rectWindow.Width() * 0.60), (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)(m_rectWindow.Width() * 0.40), (int)((m_rectWindow.Height() * dKnobLEDHeight) / 2));
					m_numledFieldMillValue[ctr].ShowWindow(TRUE);
					iControlID++;
				}
				else if (iControlID == 1)
				{
					// Second Control
					//
					if (config.m_BEnableLA[ctr])
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)((m_rectWindow.Width() / 2.0) * (1 - dLightningAlarmWidth));
						iLAH = iCH;
						iLAW = (int)((m_rectWindow.Width() / 2.0) * dLightningAlarmWidth);
					}
					else
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)(m_rectWindow.Width() / 2.0);
						iLAH = 0;
						iLAW = 0;
					}

					// StripChart
					//m_stripChart[ctr].MoveWindow(0,(int)(iHeight+(m_rectWindow.Height()*dVisualAlarmHeight)),iCW,iCH);
					//m_stripChart[ctr].ShowWindow(TRUE);
					m_chartViewer[ctr].MoveWindow(0, (int)(iHeight + (m_rectWindow.Height() * dVisualAlarmHeight)), iCW, iCH);
					if (config.m_BShowChartLightningDistance[ctr] == TRUE)
						m_chartViewer[ctr].setSize(iCW / 2, iCH);
					else
						m_chartViewer[ctr].setSize(iCW, iCH);
					m_chartViewer[ctr].ShowWindow(TRUE);

					m_chartLightningDistanceViewer[ctr].MoveWindow(iCW / 2, (int)(iCH + (m_rectWindow.Height() * dVisualAlarmHeight)), iCW / 2, iCH);
					m_chartLightningDistanceViewer[ctr].setSize(iCW / 2, iCH);
					m_chartLightningDistanceViewer[ctr].ShowWindow(config.m_BShowChartLightningDistance[ctr]);

					// Lightning Alarm
					m_laLightningAlarm[ctr].MoveWindow(iCW, (int)(iHeight + (m_rectWindow.Height() * dVisualAlarmHeight)), iLAW, iLAH);
					m_laLightningAlarm[ctr].ShowWindow(TRUE);

					// StripChart
					//iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
					//m_stripChart[ctr].MoveWindow(0,(int)(iHeight+(m_rectWindow.Height()*dVisualAlarmHeight)),m_rectWindow.Width()/2,iHeight);
					//m_stripChart[ctr].ShowWindow(TRUE);

					// Visual Alarms
					int iVisualBar_Height = (int)(m_rectWindow.Height() * dVisualAlarmHeight);
					int iVisualBar_YPos = (int)((iHeight + (m_rectWindow.Height() * dVisualAlarmHeight)) + iHeight);
					int iMuteSound_Height = iVisualBar_Height;
					int iMuteSound_Width = iVisualBar_Height;
					int iMuteSound_XPos = (int)((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET);
					int iHighField_Height = iVisualBar_Height;
					int iHighField_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iHighField_XPos = 0;
					int iVeryHighField_Height = iVisualBar_Height;
					int iVeryHighField_Width = iHighField_Width;
					int iVeryHighField_XPos = 0;
					int iDelta_Height = iVisualBar_Height;
					int iDelta_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iDelta_XPos = iHighField_Width;
					int iLightning_Height = iVisualBar_Height;
					int iLightning_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iLightning_XPos = iHighField_Width + iDelta_Width;

					m_pctHighFieldVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width, iHighField_Height);
					m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctHighFieldVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width) / iFontScaleDivider);
					m_pctVeryHighFieldVisual[ctr].MoveWindow(iVeryHighField_XPos, iVisualBar_YPos, iVeryHighField_Width, iVeryHighField_Height);
					m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctVeryHighFieldVisual[ctr].SetFontSize(static_cast<float>(iVeryHighField_Width) / iFontScaleDivider);
					m_pctDeltaVisual[ctr].MoveWindow(iDelta_XPos, iVisualBar_YPos, iDelta_Width, iDelta_Height);
					m_pctDeltaVisual[ctr].ShowWindow(FALSE);
					m_pctDeltaVisual[ctr].SetFontSize(static_cast<float>(iDelta_Width) / iFontScaleDivider);
					m_pctLightningVisual[ctr].MoveWindow(iLightning_XPos, iVisualBar_YPos, iLightning_Width, iLightning_Height);
					m_pctLightningVisual[ctr].ShowWindow(FALSE);
					m_pctLightningVisual[ctr].SetFontSize(static_cast<float>(iLightning_Width) / iFontScaleDivider);
					m_pctStatusVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width + iDelta_Width + iLightning_Width, iHighField_Height);
					m_pctStatusVisual[ctr].ShowWindow(FALSE);
					m_pctStatusVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width + iDelta_Width + iLightning_Width) / iFontScaleDivider);
					// Sound Muter
					m_tglMuteSound[ctr].MoveWindow(iMuteSound_XPos, iVisualBar_YPos, iMuteSound_Width, iMuteSound_Height);
					m_tglMuteSound[ctr].ShowWindow(TRUE);

					// Numeric LEDs
					m_numledFieldMillValue[ctr].MoveWindow((int)(m_rectWindow.Width() * 0.60), (int)((m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)) + ((m_rectWindow.Height() * dKnobLEDHeight) / 2)), (int)((m_rectWindow.Width() * 0.40) / 2.0), (int)((m_rectWindow.Height() * dKnobLEDHeight) / 2));
					m_numledFieldMillValue[ctr].ShowWindow(TRUE);
					iControlID++;
				}
				else if (iControlID == 2)
				{
					// Third Control
					//
					if (config.m_BEnableLA[ctr])
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)((m_rectWindow.Width() / 2.0) * (1 - dLightningAlarmWidth));
						iLAH = iCH;
						iLAW = (int)((m_rectWindow.Width() / 2.0) * dLightningAlarmWidth);
					}
					else
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)(m_rectWindow.Width() / 2.0);
						iLAH = 0;
						iLAW = 0;
					}

					// StripChart
					//m_stripChart[ctr].MoveWindow((int)(m_rectWindow.Width()/2.0),(int)(iHeight+(m_rectWindow.Height()*dVisualAlarmHeight)),iCW,iCH);
					//m_stripChart[ctr].ShowWindow(TRUE);
					m_chartViewer[ctr].MoveWindow((int)(m_rectWindow.Width() / 2.0), (int)(iHeight + (m_rectWindow.Height() * dVisualAlarmHeight)), iCW, iCH);
					if (config.m_BShowChartLightningDistance[ctr] == TRUE)
						m_chartViewer[ctr].setSize(iCW / 2, iCH);
					else
						m_chartViewer[ctr].setSize(iCW, iCH);
					m_chartViewer[ctr].ShowWindow(TRUE);
					m_chartLightningDistanceViewer[ctr].MoveWindow((int)(m_rectWindow.Width() / 2.0) + iCW / 2, (int)(iCH + (m_rectWindow.Height() * dVisualAlarmHeight)), iCW / 2, iCH);
					m_chartLightningDistanceViewer[ctr].setSize(iCW / 2, iCH);
					m_chartLightningDistanceViewer[ctr].ShowWindow(config.m_BShowChartLightningDistance[ctr]);

					// Lightning Alarm
					m_laLightningAlarm[ctr].MoveWindow((int)((m_rectWindow.Width() / 2.0) + iCW), (int)(iHeight + (m_rectWindow.Height() * dVisualAlarmHeight)), iLAW, iLAH);
					m_laLightningAlarm[ctr].ShowWindow(TRUE);

					// StripChart
					//iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
					//m_stripChart[ctr].MoveWindow((int)(m_rectWindow.Width()/2.0),(int)(iHeight+(m_rectWindow.Height()*dVisualAlarmHeight)),m_rectWindow.Width()/2,iHeight);
					//m_stripChart[ctr].ShowWindow(TRUE);

					// Visual Alarms
					int iVisualBar_Height = (int)(m_rectWindow.Height() * dVisualAlarmHeight);
					int iVisualBar_YPos = (int)((iHeight + (m_rectWindow.Height() * dVisualAlarmHeight)) + iHeight);
					int iMuteSound_Height = iVisualBar_Height;
					int iMuteSound_Width = iVisualBar_Height;
					int iMuteSound_XPos = (int)(m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET);
					int iHighField_Height = iVisualBar_Height;
					int iHighField_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iHighField_XPos = (int)(m_rectWindow.Width() / 2.0);
					int iVeryHighField_Height = iVisualBar_Height;
					int iVeryHighField_Width = iHighField_Width;
					int iVeryHighField_XPos = iHighField_XPos;
					int iDelta_Height = iVisualBar_Height;
					int iDelta_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iDelta_XPos = iHighField_XPos + iHighField_Width;
					int iLightning_Height = iVisualBar_Height;
					int iLightning_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iLightning_XPos = iHighField_XPos + iHighField_Width + iDelta_Width;

					m_pctHighFieldVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width, iHighField_Height);
					m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctHighFieldVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width) / iFontScaleDivider);
					m_pctVeryHighFieldVisual[ctr].MoveWindow(iVeryHighField_XPos, iVisualBar_YPos, iVeryHighField_Width, iVeryHighField_Height);
					m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctVeryHighFieldVisual[ctr].SetFontSize(static_cast<float>(iVeryHighField_Width) / iFontScaleDivider);
					m_pctDeltaVisual[ctr].MoveWindow(iDelta_XPos, iVisualBar_YPos, iDelta_Width, iDelta_Height);
					m_pctDeltaVisual[ctr].ShowWindow(FALSE);
					m_pctDeltaVisual[ctr].SetFontSize(static_cast<float>(iDelta_Width) / iFontScaleDivider);
					m_pctLightningVisual[ctr].MoveWindow(iLightning_XPos, iVisualBar_YPos, iLightning_Width, iLightning_Height);
					m_pctLightningVisual[ctr].ShowWindow(FALSE);
					m_pctLightningVisual[ctr].SetFontSize(static_cast<float>(iLightning_Width) / iFontScaleDivider);
					m_pctStatusVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width + iDelta_Width + iLightning_Width, iHighField_Height);
					m_pctStatusVisual[ctr].ShowWindow(FALSE);
					m_pctStatusVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width + iDelta_Width + iLightning_Width) / iFontScaleDivider);
					// Sound Muter
					m_tglMuteSound[ctr].MoveWindow(iMuteSound_XPos, iVisualBar_YPos, iMuteSound_Width, iMuteSound_Height);
					m_tglMuteSound[ctr].ShowWindow(TRUE);

					// Numeric LEDs
					m_numledFieldMillValue[ctr].MoveWindow((int)((m_rectWindow.Width() * 0.60) + ((m_rectWindow.Width() * 0.40) / 2.0)), (int)((m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)) + ((m_rectWindow.Height() * dKnobLEDHeight) / 2)), (int)((m_rectWindow.Width() * 0.40) / 2.0), (int)((m_rectWindow.Height() * dKnobLEDHeight) / 2));
					m_numledFieldMillValue[ctr].ShowWindow(TRUE);
				}
				// Knobs
				m_knobScaleSelector.MoveWindow(0, (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() * dKnobLEDHeight));
				m_knobTimeSelector.MoveWindow((int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() * dKnobLEDHeight));
			}
			else
			{
				//m_stripChart[ctr].ShowWindow(FALSE);
				m_chartViewer[ctr].ShowWindow(FALSE);
				m_chartLightningDistanceViewer[ctr].ShowWindow(FALSE);
				m_pctLightningVisual[ctr].ShowWindow(FALSE);
				m_pctDeltaVisual[ctr].ShowWindow(FALSE);
				m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
				m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
				m_pctStatusVisual[ctr].ShowWindow(FALSE);
				m_numledFieldMillValue[ctr].ShowWindow(FALSE);
				m_tglMuteSound[ctr].ShowWindow(FALSE);
				m_laLightningAlarm[ctr].ShowWindow(FALSE);
			}
			break;
		case 4:	// Four sources are active
			if (config.m_BDataSourceEnabled[ctr] == TRUE)
			{
				int iCH, iCW, iLAH, iLAW;
				if (iControlID == 0)
				{
					// First Control
					//
					if (config.m_BEnableLA[ctr])
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)((m_rectWindow.Width() / 2.0) * (1 - dLightningAlarmWidth));
						iLAH = iCH;
						iLAW = (int)((m_rectWindow.Width() / 2.0) * dLightningAlarmWidth);
					}
					else
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)(m_rectWindow.Width() / 2.0);
						iLAH = 0;
						iLAW = 0;
					}

					// StripChart
					//m_stripChart[ctr].MoveWindow(0,0,iCW,iCH);
					//m_stripChart[ctr].ShowWindow(TRUE);
					m_chartViewer[ctr].MoveWindow(0, 0, iCW, iCH);
					if (config.m_BShowChartLightningDistance[ctr] == TRUE)
						m_chartViewer[ctr].setSize(iCW / 2, iCH);
					else
						m_chartViewer[ctr].setSize(iCW, iCH);
					m_chartViewer[ctr].ShowWindow(TRUE);
					m_chartLightningDistanceViewer[ctr].MoveWindow(iCW / 2, 0, iCW, iCH);
					m_chartLightningDistanceViewer[ctr].setSize(iCW / 2, iCH);
					m_chartLightningDistanceViewer[ctr].ShowWindow(config.m_BShowChartLightningDistance[ctr]);

					// Lightning Alarm
					m_laLightningAlarm[ctr].MoveWindow(iCW, 0, iLAW, iLAH);
					m_laLightningAlarm[ctr].ShowWindow(TRUE);

					// StripChart
					//iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
					//m_stripChart[ctr].MoveWindow(0,0,(int)(m_rectWindow.Width()/2.0),iHeight);
					//m_stripChart[ctr].ShowWindow(TRUE);

					// Visual Alarms
					int iVisualBar_Height = (int)(m_rectWindow.Height() * dVisualAlarmHeight);
					int iVisualBar_YPos = iHeight;
					int iMuteSound_Height = iVisualBar_Height;
					int iMuteSound_Width = iVisualBar_Height;
					int iMuteSound_XPos = (int)((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET);
					int iHighField_Height = iVisualBar_Height;
					int iHighField_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iHighField_XPos = 0;
					int iVeryHighField_Height = iVisualBar_Height;
					int iVeryHighField_Width = iHighField_Width;
					int iVeryHighField_XPos = iHighField_XPos;
					int iDelta_Height = iVisualBar_Height;
					int iDelta_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iDelta_XPos = iHighField_Width;
					int iLightning_Height = iVisualBar_Height;
					int iLightning_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iLightning_XPos = iHighField_Width + iDelta_Width;

					m_pctHighFieldVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width, iHighField_Height);
					m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctHighFieldVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width) / iFontScaleDivider);
					m_pctVeryHighFieldVisual[ctr].MoveWindow(iVeryHighField_XPos, iVisualBar_YPos, iVeryHighField_Width, iVeryHighField_Height);
					m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctVeryHighFieldVisual[ctr].SetFontSize(static_cast<float>(iVeryHighField_Width) / iFontScaleDivider);
					m_pctDeltaVisual[ctr].MoveWindow(iDelta_XPos, iVisualBar_YPos, iDelta_Width, iDelta_Height);
					m_pctDeltaVisual[ctr].ShowWindow(FALSE);
					m_pctDeltaVisual[ctr].SetFontSize(static_cast<float>(iDelta_Width) / iFontScaleDivider);
					m_pctLightningVisual[ctr].MoveWindow(iLightning_XPos, iVisualBar_YPos, iLightning_Width, iLightning_Height);
					m_pctLightningVisual[ctr].ShowWindow(FALSE);
					m_pctLightningVisual[ctr].SetFontSize(static_cast<float>(iLightning_Width) / iFontScaleDivider);
					m_pctStatusVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width + iDelta_Width + iLightning_Width, iHighField_Height);
					m_pctStatusVisual[ctr].ShowWindow(FALSE);
					m_pctStatusVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width + iDelta_Width + iLightning_Width) / iFontScaleDivider);
					// Sound Muter
					m_tglMuteSound[ctr].MoveWindow(iMuteSound_XPos, iVisualBar_YPos, iMuteSound_Width, iMuteSound_Height);
					m_tglMuteSound[ctr].ShowWindow(TRUE);

					// Numeric LEDs
					m_numledFieldMillValue[ctr].MoveWindow((int)(m_rectWindow.Width() * 0.60), (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)((m_rectWindow.Width() * 0.40) / 2.0), (int)((m_rectWindow.Height() * dKnobLEDHeight) / 2));
					m_numledFieldMillValue[ctr].ShowWindow(TRUE);
					iControlID++;
				}
				else if (iControlID == 1)
				{
					// Second Control
					//
					if (config.m_BEnableLA[ctr])
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)((m_rectWindow.Width() / 2.0) * (1 - dLightningAlarmWidth));
						iLAH = iCH;
						iLAW = (int)((m_rectWindow.Width() / 2.0) * dLightningAlarmWidth);
					}
					else
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)(m_rectWindow.Width() / 2.0);
						iLAH = 0;
						iLAW = 0;
					}

					// StripChart
					//m_stripChart[ctr].MoveWindow((int)(m_rectWindow.Width()/2.0),0,iCW,iCH);
					//m_stripChart[ctr].ShowWindow(TRUE);
					m_chartViewer[ctr].MoveWindow((int)(m_rectWindow.Width() / 2.0), 0, iCW, iCH);
					if (config.m_BShowChartLightningDistance[ctr] == TRUE)
						m_chartViewer[ctr].setSize(iCW / 2, iCH);
					else
						m_chartViewer[ctr].setSize(iCW, iCH);
					m_chartViewer[ctr].ShowWindow(TRUE);
					m_chartLightningDistanceViewer[ctr].MoveWindow((int)(m_rectWindow.Width() / 2.0) + iCW / 2, 0, iCW, iCH);
					m_chartLightningDistanceViewer[ctr].setSize(iCW / 2, iCH);
					m_chartLightningDistanceViewer[ctr].ShowWindow(config.m_BShowChartLightningDistance[ctr]);


					// Lightning Alarm
					m_laLightningAlarm[ctr].MoveWindow((int)((m_rectWindow.Width() / 2.0) + iCW), 0, iLAW, iLAH);
					m_laLightningAlarm[ctr].ShowWindow(TRUE);

					// StripChart
					//iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
					//m_stripChart[ctr].MoveWindow((int)(m_rectWindow.Width()/2.0),0,(int)(m_rectWindow.Width()/2.0),iHeight);
					//m_stripChart[ctr].ShowWindow(TRUE);

					// Visual Alarms
					int iVisualBar_Height = (int)(m_rectWindow.Height() * dVisualAlarmHeight);
					int iVisualBar_YPos = iHeight;
					int iMuteSound_Height = iVisualBar_Height;
					int iMuteSound_Width = iVisualBar_Height;
					int iMuteSound_XPos = (int)(m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET);
					int iHighField_Height = iVisualBar_Height;
					int iHighField_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iHighField_XPos = (int)(m_rectWindow.Width() / 2.0);
					int iVeryHighField_Height = iVisualBar_Height;
					int iVeryHighField_Width = iHighField_Width;
					int iVeryHighField_XPos = iHighField_XPos;
					int iDelta_Height = iVisualBar_Height;
					int iDelta_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iDelta_XPos = iHighField_XPos + iHighField_Width;
					int iLightning_Height = iVisualBar_Height;
					int iLightning_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iLightning_XPos = iHighField_XPos + iHighField_Width + iDelta_Width;

					m_pctHighFieldVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width, iHighField_Height);
					m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctHighFieldVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width) / iFontScaleDivider);
					m_pctVeryHighFieldVisual[ctr].MoveWindow(iVeryHighField_XPos, iVisualBar_YPos, iVeryHighField_Width, iVeryHighField_Height);
					m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctVeryHighFieldVisual[ctr].SetFontSize(static_cast<float>(iVeryHighField_Width) / iFontScaleDivider);
					m_pctDeltaVisual[ctr].MoveWindow(iDelta_XPos, iVisualBar_YPos, iDelta_Width, iDelta_Height);
					m_pctDeltaVisual[ctr].ShowWindow(FALSE);
					m_pctDeltaVisual[ctr].SetFontSize(static_cast<float>(iDelta_Width) / iFontScaleDivider);
					m_pctLightningVisual[ctr].MoveWindow(iLightning_XPos, iVisualBar_YPos, iLightning_Width, iLightning_Height);
					m_pctLightningVisual[ctr].ShowWindow(FALSE);
					m_pctLightningVisual[ctr].SetFontSize(static_cast<float>(iLightning_Width) / iFontScaleDivider);
					m_pctStatusVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width + iDelta_Width + iLightning_Width, iHighField_Height);
					m_pctStatusVisual[ctr].ShowWindow(FALSE);
					m_pctStatusVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width + iDelta_Width + iLightning_Width) / iFontScaleDivider);
					// Sound Muter
					m_tglMuteSound[ctr].MoveWindow(iMuteSound_XPos, iVisualBar_YPos, iMuteSound_Width, iMuteSound_Height);
					m_tglMuteSound[ctr].ShowWindow(TRUE);

					// Numeric LEDs
					m_numledFieldMillValue[ctr].MoveWindow((int)((m_rectWindow.Width() * 0.60) + ((m_rectWindow.Width() * 0.40) / 2.0)), (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)((m_rectWindow.Width() * 0.40) / 2.0), (int)((m_rectWindow.Height() * dKnobLEDHeight) / 2));
					m_numledFieldMillValue[ctr].ShowWindow(TRUE);
					iControlID++;
				}
				else if (iControlID == 2)
				{
					// Third Control
					//
					if (config.m_BEnableLA[ctr])
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)((m_rectWindow.Width() / 2.0) * (1 - dLightningAlarmWidth));
						iLAH = iCH;
						iLAW = (int)((m_rectWindow.Width() / 2.0) * dLightningAlarmWidth);
					}
					else
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)(m_rectWindow.Width() / 2.0);
						iLAH = 0;
						iLAW = 0;
					}

					// StripChart
					//m_stripChart[ctr].MoveWindow(0,(int)(iHeight+(m_rectWindow.Height()*dVisualAlarmHeight)),iCW,iCH);
					//m_stripChart[ctr].ShowWindow(TRUE);
					m_chartViewer[ctr].MoveWindow(0, (int)(iHeight + (m_rectWindow.Height() * dVisualAlarmHeight)), iCW, iCH);
					if (config.m_BShowChartLightningDistance[ctr] == TRUE)
						m_chartViewer[ctr].setSize(iCW / 2, iCH);
					else
						m_chartViewer[ctr].setSize(iCW, iCH);
					m_chartViewer[ctr].ShowWindow(TRUE);

					m_chartLightningDistanceViewer[ctr].MoveWindow(iCW / 2, (int)(iCH + (m_rectWindow.Height() * dVisualAlarmHeight)), iCW / 2, iCH);
					m_chartLightningDistanceViewer[ctr].setSize(iCW / 2, iCH);
					m_chartLightningDistanceViewer[ctr].ShowWindow(config.m_BShowChartLightningDistance[ctr]);

					// Lightning Alarm
					m_laLightningAlarm[ctr].MoveWindow(iCW, (int)(iHeight + (m_rectWindow.Height() * dVisualAlarmHeight)), iLAW, iLAH);
					m_laLightningAlarm[ctr].ShowWindow(TRUE);

					// StripChart
					//iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
					//m_stripChart[ctr].MoveWindow(0,(int)(iHeight+(m_rectWindow.Height()*dVisualAlarmHeight)),m_rectWindow.Width()/2,iHeight);
					//m_stripChart[ctr].ShowWindow(TRUE);

					// Visual Alarms
					int iVisualBar_Height = (int)(m_rectWindow.Height() * dVisualAlarmHeight);
					int iVisualBar_YPos = (int)((iHeight + (m_rectWindow.Height() * dVisualAlarmHeight)) + iHeight);
					int iMuteSound_Height = iVisualBar_Height;
					int iMuteSound_Width = iVisualBar_Height;
					int iMuteSound_XPos = (int)((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET);
					int iHighField_Height = iVisualBar_Height;
					int iHighField_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iHighField_XPos = 0;
					int iVeryHighField_Height = iVisualBar_Height;
					int iVeryHighField_Width = iHighField_Width;
					int iVeryHighField_XPos = iHighField_XPos;
					int iDelta_Height = iVisualBar_Height;
					int iDelta_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iDelta_XPos = iHighField_Width;
					int iLightning_Height = iVisualBar_Height;
					int iLightning_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iLightning_XPos = iHighField_Width + iDelta_Width;

					m_pctHighFieldVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width, iHighField_Height);
					m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctHighFieldVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width) / iFontScaleDivider);
					m_pctVeryHighFieldVisual[ctr].MoveWindow(iVeryHighField_XPos, iVisualBar_YPos, iVeryHighField_Width, iVeryHighField_Height);
					m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctVeryHighFieldVisual[ctr].SetFontSize(static_cast<float>(iVeryHighField_Width) / iFontScaleDivider);
					m_pctDeltaVisual[ctr].MoveWindow(iDelta_XPos, iVisualBar_YPos, iDelta_Width, iDelta_Height);
					m_pctDeltaVisual[ctr].ShowWindow(FALSE);
					m_pctDeltaVisual[ctr].SetFontSize(static_cast<float>(iDelta_Width) / iFontScaleDivider);
					m_pctLightningVisual[ctr].MoveWindow(iLightning_XPos, iVisualBar_YPos, iLightning_Width, iLightning_Height);
					m_pctLightningVisual[ctr].ShowWindow(FALSE);
					m_pctLightningVisual[ctr].SetFontSize(static_cast<float>(iLightning_Width) / iFontScaleDivider);
					m_pctStatusVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width + iDelta_Width + iLightning_Width, iHighField_Height);
					m_pctStatusVisual[ctr].ShowWindow(FALSE);
					m_pctStatusVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width + iDelta_Width + iLightning_Width) / iFontScaleDivider);
					// Sound Muter
					m_tglMuteSound[ctr].MoveWindow(iMuteSound_XPos, iVisualBar_YPos, iMuteSound_Width, iMuteSound_Height);
					m_tglMuteSound[ctr].ShowWindow(TRUE);

					// Numeric LEDs
					m_numledFieldMillValue[ctr].MoveWindow((int)(m_rectWindow.Width() * 0.60), (int)((m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)) + ((m_rectWindow.Height() * dKnobLEDHeight) / 2)), (int)((m_rectWindow.Width() * 0.40) / 2.0), (int)((m_rectWindow.Height() * dKnobLEDHeight) / 2));
					m_numledFieldMillValue[ctr].ShowWindow(TRUE);
					iControlID++;
				}
				else if (iControlID == 3)
				{
					// Fourth Control
					//
					if (config.m_BEnableLA[ctr])
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)((m_rectWindow.Width() / 2.0) * (1 - dLightningAlarmWidth));
						iLAH = iCH;
						iLAW = (int)((m_rectWindow.Width() / 2.0) * dLightningAlarmWidth);
					}
					else
					{
						iCH = iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
						iCW = (int)(m_rectWindow.Width() / 2.0);
						iLAH = 0;
						iLAW = 0;
					}

					// StripChart
					//m_stripChart[ctr].MoveWindow((int)(m_rectWindow.Width()/2.0),(int)(iHeight+(m_rectWindow.Height()*dVisualAlarmHeight)),iCW,iCH);
					//m_stripChart[ctr].ShowWindow(TRUE);
					m_chartViewer[ctr].MoveWindow((int)(m_rectWindow.Width() / 2.0), (int)(iHeight + (m_rectWindow.Height() * dVisualAlarmHeight)), iCW, iCH);
					if (config.m_BShowChartLightningDistance[ctr] == TRUE)
						m_chartViewer[ctr].setSize(iCW / 2, iCH);
					else
						m_chartViewer[ctr].setSize(iCW, iCH);
					m_chartViewer[ctr].ShowWindow(TRUE);

					m_chartLightningDistanceViewer[ctr].MoveWindow((int)(m_rectWindow.Width() / 2.0) + iCW / 2, (int)(iCH + (m_rectWindow.Height() * dVisualAlarmHeight)), iCW / 2, iCH);
					m_chartLightningDistanceViewer[ctr].setSize(iCW / 2, iCH);
					m_chartLightningDistanceViewer[ctr].ShowWindow(config.m_BShowChartLightningDistance[ctr]);

					// Lightning Alarm
					m_laLightningAlarm[ctr].MoveWindow((int)((m_rectWindow.Width() / 2.0) + iCW), (int)(iHeight + (m_rectWindow.Height() * dVisualAlarmHeight)), iLAW, iLAH);
					m_laLightningAlarm[ctr].ShowWindow(TRUE);

					// StripChart
					//iHeight = (int)(m_rectWindow.Height() * ((dOverallChartHeight - dVisualAlarmHeight) / 2));
					//m_stripChart[ctr].MoveWindow((int)(m_rectWindow.Width()/2.0),(int)(iHeight+(m_rectWindow.Height()*dVisualAlarmHeight)),m_rectWindow.Width()/2,iHeight);
					//m_stripChart[ctr].ShowWindow(TRUE);

					// Visual Alarms
					int iVisualBar_Height = (int)(m_rectWindow.Height() * dVisualAlarmHeight);
					int iVisualBar_YPos = (int)((iHeight + (m_rectWindow.Height() * dVisualAlarmHeight)) + iHeight);
					int iMuteSound_Height = iVisualBar_Height;
					int iMuteSound_Width = iVisualBar_Height;
					int iMuteSound_XPos = (int)(m_rectWindow.Width() - iMuteSound_Width - MUTESOUND_BORDEROFFSET);
					int iHighField_Height = iVisualBar_Height;
					int iHighField_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iHighField_XPos = (int)(m_rectWindow.Width() / 2.0);
					int iVeryHighField_Height = iVisualBar_Height;
					int iVeryHighField_Width = iHighField_Width;
					int iVeryHighField_XPos = iHighField_XPos;
					int iDelta_Height = iVisualBar_Height;
					int iDelta_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iDelta_XPos = iHighField_XPos + iHighField_Width;
					int iLightning_Height = iVisualBar_Height;
					int iLightning_Width = (int)(((m_rectWindow.Width() / 2.0) - iMuteSound_Width - MUTESOUND_BORDEROFFSET) / 3.0);
					int iLightning_XPos = iHighField_XPos + iHighField_Width + iDelta_Width;

					m_pctHighFieldVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width, iHighField_Height);
					m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctHighFieldVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width) / iFontScaleDivider);
					m_pctVeryHighFieldVisual[ctr].MoveWindow(iVeryHighField_XPos, iVisualBar_YPos, iVeryHighField_Width, iVeryHighField_Height);
					m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
					m_pctVeryHighFieldVisual[ctr].SetFontSize(static_cast<float>(iVeryHighField_Width) / iFontScaleDivider);
					m_pctDeltaVisual[ctr].MoveWindow(iDelta_XPos, iVisualBar_YPos, iDelta_Width, iDelta_Height);
					m_pctDeltaVisual[ctr].ShowWindow(FALSE);
					m_pctDeltaVisual[ctr].SetFontSize(static_cast<float>(iDelta_Width) / iFontScaleDivider);
					m_pctLightningVisual[ctr].MoveWindow(iLightning_XPos, iVisualBar_YPos, iLightning_Width, iLightning_Height);
					m_pctLightningVisual[ctr].ShowWindow(FALSE);
					m_pctLightningVisual[ctr].SetFontSize(static_cast<float>(iLightning_Width) / iFontScaleDivider);
					m_pctStatusVisual[ctr].MoveWindow(iHighField_XPos, iVisualBar_YPos, iHighField_Width + iDelta_Width + iLightning_Width, iHighField_Height);
					m_pctStatusVisual[ctr].ShowWindow(FALSE);
					m_pctStatusVisual[ctr].SetFontSize(static_cast<float>(iHighField_Width + iDelta_Width + iLightning_Width) / iFontScaleDivider);
					// Sound Muter
					m_tglMuteSound[ctr].MoveWindow(iMuteSound_XPos, iVisualBar_YPos, iMuteSound_Width, iMuteSound_Height);
					m_tglMuteSound[ctr].ShowWindow(TRUE);

					// Numeric LEDs
					m_numledFieldMillValue[ctr].MoveWindow((int)((m_rectWindow.Width() * 0.60) + ((m_rectWindow.Width() * 0.40) / 2.0)), (int)((m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)) + ((m_rectWindow.Height() * dKnobLEDHeight) / 2)), (int)((m_rectWindow.Width() * 0.40) / 2.0), (int)((m_rectWindow.Height() * dKnobLEDHeight) / 2));
					m_numledFieldMillValue[ctr].ShowWindow(TRUE);
				}
				// Knobs
				m_knobScaleSelector.MoveWindow(0, (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() * dKnobLEDHeight));
				m_knobTimeSelector.MoveWindow((int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() - (m_rectWindow.Height() * dKnobLEDHeight)), (int)(m_rectWindow.Width() * 0.30), (int)(m_rectWindow.Height() * dKnobLEDHeight));
			}
			else
			{
				//m_stripChart[ctr].ShowWindow(FALSE);
				m_chartViewer[ctr].ShowWindow(FALSE);
				m_chartLightningDistanceViewer[ctr].ShowWindow(FALSE);
				m_pctLightningVisual[ctr].ShowWindow(FALSE);
				m_pctDeltaVisual[ctr].ShowWindow(FALSE);
				m_pctHighFieldVisual[ctr].ShowWindow(FALSE);
				m_pctVeryHighFieldVisual[ctr].ShowWindow(FALSE);
				m_pctStatusVisual[ctr].ShowWindow(FALSE);
				m_numledFieldMillValue[ctr].ShowWindow(FALSE);
				m_tglMuteSound[ctr].ShowWindow(FALSE);
				m_laLightningAlarm[ctr].ShowWindow(FALSE);
			}
			break;
		}
	}
}


double CEFMDlg::CTime_to_Double(CTime t)
{
	double x;

	x = (t.GetYear() - 2000) * 365;
	x += (t.GetMonth() - 1) * 31;
	x += (t.GetDay() - 1);
	x += t.GetHour() * HOURMULTIPLIER;
	x += t.GetMinute() * MINUTEMULTIPLIER;
	x += t.GetSecond() * SECONDMULTIPLIER;

	return x;
}

double CEFMDlg::CTime_to_ChartDouble()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	return Chart::chartTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,
		st.wSecond) + st.wMilliseconds / 1000.0;
}

void CEFMDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	// Check to see if a control has a window first before we try to resize
	// it.  This prevents the debug assertion failure when the window is 
	// intially created.
	//if (::IsWindow(m_stripChart->m_hWnd))
	if (::IsWindow(m_chartViewer->m_hWnd))
	{
		LayoutControls();
	}
}

void CEFMDlg::OnToolsOptions()
{
	// TODO: Add your command handler code here
	CEFMOptions dlgOptions;
	int dlgRet;

	dlgRet = dlgOptions.DoModal();		// Modalize the options dialog

}

void CEFMDlg::OnShowConnectedClients()
{
	// TODO: Add your command handler code here
	CClientsConnectedViewer dlg(tDataClientServer);
	int dlgRet;
	dlgRet = dlg.DoModal();		// Modalize the options dialog
}


BEGIN_EVENTSINK_MAP(CEFMDlg, CDialog)
	//{{AFX_EVENTSINK_MAP(CEFMDlg)
	ON_EVENT(CEFMDlg, KNOB_SCALESELECTOR, 1 /* Change */, OnChangeScaleselector, VTS_NONE)
	ON_EVENT(CEFMDlg, KNOB_TIMESELECTOR, 1 /* Change */, OnChangeTimeselector, VTS_NONE)
	//ON_EVENT(CEFMDlg, STRIP_FIELDMILL1, -600 /* Click */, OnClickFieldmill1, VTS_NONE)
	//ON_EVENT(CEFMDlg, STRIP_FIELDMILL2, -600 /* Click */, OnClickFieldmill2, VTS_NONE)
	//ON_EVENT(CEFMDlg, STRIP_FIELDMILL3, -600 /* Click */, OnClickFieldmill3, VTS_NONE)
	//ON_EVENT(CEFMDlg, STRIP_FIELDMILL4, -600 /* Click */, OnClickFieldmill4, VTS_NONE)
	//ON_EVENT(CEFMDlg, STRIP_FIELDMILL1, -601 /* DblClick */, OnDblClickFieldmill1, VTS_NONE)
	//ON_EVENT(CEFMDlg, STRIP_FIELDMILL2, -601 /* DblClick */, OnDblClickFieldmill2, VTS_NONE)
	//ON_EVENT(CEFMDlg, STRIP_FIELDMILL3, -601 /* DblClick */, OnDblClickFieldmill3, VTS_NONE)
	//ON_EVENT(CEFMDlg, STRIP_FIELDMILL4, -601 /* DblClick */, OnDblClickFieldmill4, VTS_NONE)
	ON_EVENT(CEFMDlg, TGL_MUTESOUND_FIELDMILL1, -600 /* Click */, OnClickMutesoundFieldmill1, VTS_NONE)
	ON_EVENT(CEFMDlg, TGL_MUTESOUND_FIELDMILL2, -600 /* Click */, OnClickMutesoundFieldmill2, VTS_NONE)
	ON_EVENT(CEFMDlg, TGL_MUTESOUND_FIELDMILL3, -600 /* Click */, OnClickMutesoundFieldmill3, VTS_NONE)
	ON_EVENT(CEFMDlg, TGL_MUTESOUND_FIELDMILL4, -600 /* Click */, OnClickMutesoundFieldmill4, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CEFMDlg::OnChangeScaleselector()
{
	// TODO: Add your control notification handler code here

	// Save the knob's setting for future callback
	config.m_iScaleKnobSetting[m_iChartSelected] = m_knobScaleSelector.GetValue();

	switch (config.m_iScaleKnobSetting[m_iChartSelected])
	{
	case 0: // 0.5 kV/m
		//m_stripChart[m_iChartSelected].SetTrackMin(-0.5);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMin(-0.5);
		//m_stripChart[m_iChartSelected].SetTrackMax(0.5);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMax(0.5);
		m_chartViewer[m_iChartSelected].setYSpan(-0.5, 0.5);
		break;
	case 1: // 1 kV/m
		//m_stripChart[m_iChartSelected].SetTrackMin(-1.0);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMin(-1.0);
		//m_stripChart[m_iChartSelected].SetTrackMax(1.0);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMax(1.0);
		m_chartViewer[m_iChartSelected].setYSpan(-1.0, 1.0);
		break;
	case 2: // 2 kV/m
		//m_stripChart[m_iChartSelected].SetTrackMin(-2.0);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMin(-2.0);
		//m_stripChart[m_iChartSelected].SetTrackMax(2.0);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMax(2.0);
		m_chartViewer[m_iChartSelected].setYSpan(-2.0, 2.0);
		break;
	case 3: // 5 kV/m
		//m_stripChart[m_iChartSelected].SetTrackMin(-5.0);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMin(-5.0);
		//m_stripChart[m_iChartSelected].SetTrackMax(5.0);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMax(5.0);
		m_chartViewer[m_iChartSelected].setYSpan(-5.0, 5.0);
		break;
	case 4: // 7.5 kV/m
		//m_stripChart[m_iChartSelected].SetTrackMin(-7.5);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMin(-7.5);
		//m_stripChart[m_iChartSelected].SetTrackMax(7.5);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMax(7.5);
		m_chartViewer[m_iChartSelected].setYSpan(-7.5, 7.5);
		break;
	case 5: // 10 kV/m
		//m_stripChart[m_iChartSelected].SetTrackMin(-10.0);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMin(-10.0);
		//m_stripChart[m_iChartSelected].SetTrackMax(10.0);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMax(10.0);
		m_chartViewer[m_iChartSelected].setYSpan(-10.0, 10.0);
		break;
	case 6: // 15 kV/m
		//m_stripChart[m_iChartSelected].SetTrackMin(-15.0);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMin(-15.0);
		//m_stripChart[m_iChartSelected].SetTrackMax(15.0);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMax(15.0);
		m_chartViewer[m_iChartSelected].setYSpan(-15.0, 15.0);
		break;
	case 7: // 20 kV/m
		//// go a little beyond so we can open it up with the handles
		//m_stripChart[m_iChartSelected].SetTrackMin(-29.0);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMin(-20.0);
		//m_stripChart[m_iChartSelected].SetTrackMax(29.0);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMax(20.0);
		m_chartViewer[m_iChartSelected].setYSpan(-20.0, 20.0);
		break;
	case 8: // 50 kV/m
		//// go a little beyond so we can open it up with the handles
		//m_stripChart[m_iChartSelected].SetTrackMin(-50.0);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMin(-50.0);
		//m_stripChart[m_iChartSelected].SetTrackMax(50.0);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMax(50.0);
		// AUTO
		m_chartViewer[m_iChartSelected].enableAutoScale();
		break;
	case 9: // 100 kV/m
		//// go a little beyond so we can open it up with the handles
		//m_stripChart[m_iChartSelected].SetTrackMin(-100.00);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMin(-100.00);
		//m_stripChart[m_iChartSelected].SetTrackMax(100.00);
		//m_stripChart[m_iChartSelected].SetTrackDisplayMax(100.00);
		m_chartViewer[m_iChartSelected].setYSpan(-100.0, 100.0);
		break;
	} // End "switch (m_knobScaleSelector.GetValue())"

}

void CEFMDlg::OnChangeTimeselector()
{
	// TODO: Add your control notification handler code here
	double time = CTime_to_Double(CTime::GetCurrentTime());
	double span = 0.;
	//int ctr;

	// Save the knob's setting for future callback
	config.m_iTimeKnobSetting[m_iChartSelected] = m_knobTimeSelector.GetValue();

	switch (config.m_iTimeKnobSetting[m_iChartSelected])
	{
	case 0: // 5 Minutes
		m_iChartUpdatePeriod[m_iChartSelected] = 0; // update every half second (counts from 0)
		span = 1.0 / 24.0 / 60.0 * 5.5; // use 5.5 so strip changes scales and doesn't overlap legends
		break;
	case 1: // 15 Minutes
		m_iChartUpdatePeriod[m_iChartSelected] = 0; // update every half second (counts from 0)
		span = 1.0 / 24.0 / 60.0 * 15.0;
		break;
	case 2: // 30 Minutes
		m_iChartUpdatePeriod[m_iChartSelected] = 1; // update every 1 seconds (counts from 0)
		span = 1.0 / 24.0 / 60.0 * 30.0;
		break;
	case 3: // 1 Hour
		m_iChartUpdatePeriod[m_iChartSelected] = 3; // update every 2 seconds (counts from 0)
		span = 1.0 / (24.0);
		break;
	case 4: // 2 Hours
		m_iChartUpdatePeriod[m_iChartSelected] = 5; // update every 3 seconds (counts from 0)
		span = 1.0 / 24.0 * 2.0;
		break;
	case 5: // 4 Hour
		m_iChartUpdatePeriod[m_iChartSelected] = 7; // update every 4 seconds (counts from 0)
		span = 1.0 / 24.0 * 4.0;
		break;
	case 6: // 8 Hour
		m_iChartUpdatePeriod[m_iChartSelected] = 9; // update every 5 second (counts from 0)
		span = 1.0 / 24.0 * 8.0;
		break;
	case 7: // 24 Hours
		m_iChartUpdatePeriod[m_iChartSelected] = 9; // update every 7.5 seconds (counts from 0)
		span = 1.0;
		break;
	}

	// Reset the chart update counter
	m_iChartUpdateCounter[m_iChartSelected] = 0;

	// Update the selected chart with the new time scale
	//m_stripChart[m_iChartSelected].SetXSpan(span); //  1 day / 24 hours / 60 min/hour * 5 min
	//m_stripChart[m_iChartSelected].SetLastX(time);
	//m_stripChart[m_iChartSelected].SetDisplay(time,time-span); // set display x max,min
	//m_stripChart[m_iChartSelected].SetXDisplayMin(time-span); // set handle position
	m_chartViewer[m_iChartSelected].setXSpan(span);
	m_chartLightningDistanceViewer[m_iChartSelected].setXSpan(span);
}

void CEFMDlg::MakeChartsReflectKnobs()
{
	int ctr, iOrigChart;

	// Save the index of the currently selected chart
	iOrigChart = m_iChartSelected;

	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		if (config.m_BDataSourceEnabled[ctr] == TRUE)
		{
			m_iChartSelected = ctr;
			m_knobScaleSelector.SetValue(config.m_iScaleKnobSetting[m_iChartSelected]);
			m_knobTimeSelector.SetValue(config.m_iTimeKnobSetting[m_iChartSelected]);
			OnChangeScaleselector();	// Make the graph agree with the control
			OnChangeTimeselector();		// Make the graph agree with the control
		}
	}

	// Reset the selected chart back to the original value
	m_iChartSelected = iOrigChart;
	m_knobScaleSelector.SetValue(config.m_iScaleKnobSetting[m_iChartSelected]);
	m_knobTimeSelector.SetValue(config.m_iTimeKnobSetting[m_iChartSelected]);

}

void CEFMDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	int ctr;
	dAbsFMR = 0;
	CString txt;

	switch (nIDEvent)
	{
	case TMR_ONESEC:
		for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
		{
			if (config.m_BDataSourceEnabled[ctr] == TRUE)
			{
				if (!tThInfo[ctr].GetSignalLostCounter())
				{
					ProcessAlarms(ctr);
				}

				tThInfo[ctr].IncSignalLostCounter();
				if (tThInfo[ctr].GetSignalLostCounter() >= SIGNALLOST_TIMEOUT)
				{
					// Signal Lost
					this->PostMessage(WM_SIGNALLOSTFAULT, 0, (LPARAM)ctr);
				}

				// Decrement any range ring countdowns
				for (int iIdx = 0; iIdx < m_aRRCountdown[ctr].GetSize(); iIdx++)
				{
					m_aRRCountdown[ctr][iIdx].iCount--;
					if (m_aRRCountdown[ctr][iIdx].iCount <= 0)
					{
						// This timer has elapsed
						// Turn off the associated range ring
						// Remove this list entry
						m_laLightningAlarm[m_aRRCountdown[ctr][iIdx].iMillID].SetLightningAlarm(FALSE, m_aRRCountdown[ctr][iIdx].uiBin);
						m_aRRCountdown[ctr].RemoveAt(iIdx);
						break;
					}
				}

				// Process the alarm counters
				if (tThInfo[ctr].m_iLACounter)
				{
					tThInfo[ctr].m_iLACounter--;
					if (tThInfo[ctr].m_bLALastState && (tThInfo[ctr].m_iLACounter <= 0))
					{
						tThInfo[ctr].m_bPlayLAAllClearWAV = true;
					}
				}
				if (tThInfo[ctr].m_iVHFACounter)
				{
					tThInfo[ctr].m_iVHFACounter--;
				}
				if (tThInfo[ctr].m_iHFACounter) { tThInfo[ctr].m_iHFACounter--; }
				if (tThInfo[ctr].m_iDACounter) { tThInfo[ctr].m_iDACounter--; }

				if (tThInfo[ctr].m_iLAWAVCounter) { tThInfo[ctr].m_iLAWAVCounter--; }
				if (tThInfo[ctr].m_iDAWAVCounter) { tThInfo[ctr].m_iDAWAVCounter--; }
				if (tThInfo[ctr].m_iVHFAWAVCounter) { tThInfo[ctr].m_iVHFAWAVCounter--; }
				if (tThInfo[ctr].m_iHFAWAVCounter) { tThInfo[ctr].m_iHFAWAVCounter--; }
				if (tThInfo[ctr].m_iLightningDetectedAtCtr) { tThInfo[ctr].m_iLightningDetectedAtCtr--; }

				ProcessWAVAlarms(ctr);

				// If data sharing is enabled, set client count
				if (config.m_BDataSharingEnabled[ctr])
				{
					//						m_pDlgDebugger->SetClientCount(tThInfo[ctr].ClientsCount());
					CString newtitle;
					newtitle.Format("%s (%d Clients Connected)", config.m_szDataSourceName[ctr], tThInfo[ctr].ClientsCount());
					SetChartTitle(ctr, newtitle);
				}

				// time for scheduled alert?
				if (config.bAlertsScheduled[ctr])
				{
					if (CTime().GetCurrentTime() >= nextScheduledAlertTime[ctr])
					{
						SendScheduledAlerts(ctr);
						SetNextScheduledAlertTime(ctr);
					}
				} // scheduled alerts enabled


			} // if data source enabled
		} // for

		if (m_iRotorFaultWAVCounter) { m_iRotorFaultWAVCounter--; }
		if (m_iSelfTestFaultWAVCounter) { m_iSelfTestFaultWAVCounter--; }

		break;
		// End 1 Second Timer
	case TMR_ONEFIFTHSEC:
		m_chartViewer[0].updateViewPort(true, false);
		m_chartViewer[1].updateViewPort(true, false);
		m_chartViewer[2].updateViewPort(true, false);
		m_chartViewer[3].updateViewPort(true, false);
		m_chartLightningDistanceViewer[0].updateViewPort(true, false);
		m_chartLightningDistanceViewer[1].updateViewPort(true, false);
		m_chartLightningDistanceViewer[2].updateViewPort(true, false);
		m_chartLightningDistanceViewer[3].updateViewPort(true, false);
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

void CEFMDlg:: OnClickFieldmill1()
{
	// TODO: Add your control notification handler code here
	m_iChartSelected = 0;
	// Set the positions for the knobs
	m_knobScaleSelector.SetValue(config.m_iScaleKnobSetting[m_iChartSelected]);
	m_knobTimeSelector.SetValue(config.m_iTimeKnobSetting[m_iChartSelected]);
	// Make the chart scale settings agree with the settings on the knobs.

	// Change the colour of the chart caption so we know which is selected.
	//m_stripChart[0].SetCaptionColor(RGB(0,255,255));
	//m_stripChart[1].SetCaptionColor(RGB(0,128,255));
	//m_stripChart[2].SetCaptionColor(RGB(0,128,255));
	//m_stripChart[3].SetCaptionColor(RGB(0,128,255));
	m_chartViewer[0].setCaptionColor(RGB(0, 128, 255));
	m_chartViewer[1].setCaptionColor(RGB(0, 128, 255));
	m_chartViewer[2].setCaptionColor(RGB(0, 128, 255));
	m_chartViewer[3].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[0].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[1].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[2].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[3].setCaptionColor(RGB(0, 128, 255));
}

void CEFMDlg::OnClickFieldmill2()
{
	// TODO: Add your control notification handler code here
	m_iChartSelected = 1;
	// Set the positions for the knobs
	m_knobScaleSelector.SetValue(config.m_iScaleKnobSetting[m_iChartSelected]);
	m_knobTimeSelector.SetValue(config.m_iTimeKnobSetting[m_iChartSelected]);
	// Make the chart scale settings agree with the settings on the knobs.

	// Change the colour of the chart caption so we know which is selected.
	//m_stripChart[0].SetCaptionColor(RGB(0,128,255));
	//m_stripChart[1].SetCaptionColor(RGB(0,255,255));
	//m_stripChart[2].SetCaptionColor(RGB(0,128,255));
	//m_stripChart[3].SetCaptionColor(RGB(0,128,255));
	m_chartViewer[0].setCaptionColor(RGB(0, 128, 255));
	m_chartViewer[1].setCaptionColor(RGB(0, 255, 255));
	m_chartViewer[2].setCaptionColor(RGB(0, 128, 255));
	m_chartViewer[3].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[0].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[1].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[2].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[3].setCaptionColor(RGB(0, 128, 255));
}

void CEFMDlg::OnClickFieldmill3()
{
	// TODO: Add your control notification handler code here
	m_iChartSelected = 2;
	// Set the positions for the knobs
	m_knobScaleSelector.SetValue(config.m_iScaleKnobSetting[m_iChartSelected]);
	m_knobTimeSelector.SetValue(config.m_iTimeKnobSetting[m_iChartSelected]);
	// Make the chart scale settings agree with the settings on the knobs.

	// Change the colour of the chart caption so we know which is selected.
	//m_stripChart[0].SetCaptionColor(RGB(0,128,255));
	//m_stripChart[1].SetCaptionColor(RGB(0,128,255));
	//m_stripChart[2].SetCaptionColor(RGB(0,255,255));
	//m_stripChart[3].SetCaptionColor(RGB(0,128,255));
	m_chartViewer[0].setCaptionColor(RGB(0, 128, 255));
	m_chartViewer[1].setCaptionColor(RGB(0, 128, 255));
	m_chartViewer[2].setCaptionColor(RGB(0, 255, 255));
	m_chartViewer[3].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[0].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[1].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[2].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[3].setCaptionColor(RGB(0, 128, 255));
}

void CEFMDlg::OnClickFieldmill4()
{
	// TODO: Add your control notification handler code here
	m_iChartSelected = 3;
	// Set the positions for the knobs
	m_knobScaleSelector.SetValue(config.m_iScaleKnobSetting[m_iChartSelected]);
	m_knobTimeSelector.SetValue(config.m_iTimeKnobSetting[m_iChartSelected]);
	// Make the chart scale settings agree with the settings on the knobs.

	// Change the colour of the chart caption so we know which is selected.
	//m_stripChart[0].SetCaptionColor(RGB(0,128,255));
	//m_stripChart[1].SetCaptionColor(RGB(0,128,255));
	//m_stripChart[2].SetCaptionColor(RGB(0,128,255));
	//m_stripChart[3].SetCaptionColor(RGB(0,255,255));
	m_chartViewer[0].setCaptionColor(RGB(0, 128, 255));
	m_chartViewer[1].setCaptionColor(RGB(0, 128, 255));
	m_chartViewer[2].setCaptionColor(RGB(0, 128, 255));
	m_chartViewer[3].setCaptionColor(RGB(0, 255, 255));
	m_chartLightningDistanceViewer[0].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[1].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[2].setCaptionColor(RGB(0, 128, 255));
	m_chartLightningDistanceViewer[3].setCaptionColor(RGB(0, 128, 255));
}

void CEFMDlg::ActivateLightningAlarm(int iMillIndex, int iMode, int iDistance)
{
	// iMode: 1-Visual, 2-Audio
	if (iMode & 0x00000001)
	{
		// Set the chart colour and LED colour to the alarm colour
		this->PostMessage(WM_SETCONTROLCOLOURS, (WPARAM)iMillIndex, (LPARAM)RGB(255, 0, 255));

		m_pctLightningVisual[iMillIndex].ShowWindow(TRUE);
	}

	if (iMode & 0x00000002)
	{
	}

}

void CEFMDlg::ActivateHighFieldAlarm(int iMillIndex, int iMode)
{
	// iMode: 1-Visual, 2-Audio
	if (iMode & 0x00000001)
	{
		// Set the chart colour and LED colour to the alarm colour
		this->PostMessage(WM_SETCONTROLCOLOURS, (WPARAM)iMillIndex, (LPARAM)RGB(0, 255, 255));

		m_pctHighFieldVisual[iMillIndex].ShowWindow(TRUE);
		m_laLightningAlarm[iMillIndex].SetHighFieldAlarm(TRUE);
	}

	if (iMode & 0x00000002)
	{
	}

	if (iMode & 0x00000003)
	{
		m_pctHighFieldVisual[iMillIndex].ShowWindow(TRUE);
	}

}

void CEFMDlg::ActivateVeryHighFieldAlarm(int iMillIndex, int iMode)
{
	// iMode: 1-Visual, 2-Audio
	if (iMode & 0x00000001)
	{
		// Set the chart colour and LED colour to the alarm colour
		this->PostMessage(WM_SETCONTROLCOLOURS, (WPARAM)iMillIndex, (LPARAM)RGB(0, 0, 255));

		m_pctVeryHighFieldVisual[iMillIndex].ShowWindow(TRUE);
		m_laLightningAlarm[iMillIndex].SetVeryHighFieldAlarm(TRUE);
	}

	if (iMode & 0x00000002)
	{
	}

	if (iMode & 0x00000003)
	{
		m_pctVeryHighFieldVisual[iMillIndex].ShowWindow(TRUE);
	}
}

void CEFMDlg::ActivateDeltaAlarm(int iMillIndex, int iMode)
{
	tThInfo[iMillIndex].ClrDeltaAlarmBuffer();		// Clear the Delta Alarm buffer

	// iMode: 1-Visual, 2-Audio
	if (iMode & 0x00000001)
	{
		// Set the chart colour and LED colour to the alarm colour
		this->PostMessage(WM_SETCONTROLCOLOURS, (WPARAM)iMillIndex, (LPARAM)RGB(0, 255, 255));

		m_pctDeltaVisual[iMillIndex].ShowWindow(TRUE);
	}

	if (iMode & 0x00000002)
	{
	}

	if (iMode & 0x00000003)
	{
		m_pctDeltaVisual[iMillIndex].ShowWindow(TRUE);
	}
}

void CEFMDlg::DeactivateLightningAlarm(int iMillIndex, int iMode)
{
	int iActiveCount = 0;
	CString txt;

	tThInfo[iMillIndex].m_iLACounter = 0;

	// iMode: 1-Visual, 2-Audio
	if (iMode & 0x00000001)
	{
		// Return all chart and led colours back to normal
		//this->PostMessage(WM_SETCONTROLCOLOURS,(WPARAM)iMillIndex,(LPARAM)COLOUR_ENABLED);

		m_pctLightningVisual[iMillIndex].ShowWindow(FALSE);
	}

	if (iMode & 0x00000002)
	{
	}

	// Event Logging, log if alarm was active (not just re-clearing it)
	if (tThInfo[iMillIndex].m_bLALastState) {
		txt.Format("%s Lightning Alarm Clear.", config.m_szDataSourceName[iMillIndex]);
		m_EventLog.LogEvent(txt);
		// Email & SMS
		if (config.bAlertsLAClear)
		{
			Alerts.SendAlerts(iMillIndex, "Lightning Alarm Clear");
		}
	}
	tThInfo[iMillIndex].m_bLALastState = false;
}

void CEFMDlg::DeactivateHighFieldAlarm(int iMillIndex, int iMode)
{
	int iActiveCount = 0;
	CString txt;

	tThInfo[iMillIndex].m_iHFACounter = 0;

	// iMode: 1-Visual, 2-Audio
	if (iMode & 0x00000001)
	{
		// Return all chart and led colours back to normal
		//this->PostMessage(WM_SETCONTROLCOLOURS,(WPARAM)iMillIndex,(LPARAM)COLOUR_ENABLED);
		m_pctHighFieldVisual[iMillIndex].ShowWindow(FALSE);
		m_laLightningAlarm[iMillIndex].SetHighFieldAlarm(FALSE);
	}

	if (iMode & 0x00000002)
	{
	}

	if (iMode & 0x00000003)
	{
		m_pctHighFieldVisual[iMillIndex].ShowWindow(FALSE);

		// Event Logging, log if alarm was active (not just re-clearing it)
		if (tThInfo[iMillIndex].m_bHFALastState) {
			txt.Format("%s High Field Alarm Clear.", config.m_szDataSourceName[iMillIndex]);
			m_EventLog.LogEvent(txt);
			// Email & SMS
			if (config.bAlertsHFClear)
			{
				Alerts.SendAlerts(iMillIndex, "High Field Alarm Clear");
			}
		}
		tThInfo[iMillIndex].m_bHFALastState = false;
	}

}

void CEFMDlg::DeactivateVeryHighFieldAlarm(int iMillIndex, int iMode)
{
	int iActiveCount = 0;
	CString txt;

	tThInfo[iMillIndex].m_iVHFACounter = 0;

	// iMode: 1-Visual, 2-Audio
	if (iMode & 0x00000001)
	{
		// Return all chart and led colours back to normal
		//this->PostMessage(WM_SETCONTROLCOLOURS,(WPARAM)iMillIndex,(LPARAM)COLOUR_ENABLED);

		m_pctVeryHighFieldVisual[iMillIndex].ShowWindow(FALSE);
		m_laLightningAlarm[iMillIndex].SetVeryHighFieldAlarm(FALSE);
	}

	if (iMode & 0x00000002)
	{
		// Event Logging, log if alarm was active (not just re-clearing it)
		if (tThInfo[iMillIndex].m_bVHFALastState)
		{
			txt.Format("%s Very High Field Alarm Clear.", config.m_szDataSourceName[iMillIndex]);
			m_EventLog.LogEvent(txt);
			// Email & SMS
			if (config.bAlertsVHFClear)
			{
				Alerts.SendAlerts(iMillIndex, "Very High Field Alarm Clear");
			}

		}
		tThInfo[iMillIndex].m_bVHFALastState = false;

		/*		if (iMode & 0x00000003)
				{


					// Event Logging, log if alarm was active (not just re-clearing it)
					if (tThInfo[iMillIndex].m_bVHFALastState) {
						txt.Format("%s Very High Field Alarm Clear.",config.m_szDataSourceName[iMillIndex]);
						m_EventLog.LogEvent(txt);
					}
					tThInfo[iMillIndex].m_bVHFALastState = false;
				}
		*/
	}
}

void CEFMDlg::DeactivateDeltaAlarm(int iMillIndex, int iMode)
{
	int iActiveCount = 0;
	CString txt;

	tThInfo[iMillIndex].m_iDACounter = 0;

	// iMode: 1-Visual, 2-Audio
	if (iMode & 0x00000001)
	{
		// Return all chart and led colours back to normal
		//this->PostMessage(WM_SETCONTROLCOLOURS,(WPARAM)iMillIndex,(LPARAM)COLOUR_ENABLED);
		m_pctDeltaVisual[iMillIndex].ShowWindow(FALSE);
	}

	if (iMode & 0x00000002)
	{
	}

	if (iMode & 0x00000003)
	{
		m_pctDeltaVisual[iMillIndex].ShowWindow(FALSE);

		// Event Logging, log if alarm was active (not just re-clearing it)
		if (tThInfo[iMillIndex].m_bDALastState) {
			txt.Format("%s Delta Alarm Clear.", config.m_szDataSourceName[iMillIndex]);
			m_EventLog.LogEvent(txt);
			// Email & SMS
			if (config.bAlertsDAClear)
			{
				Alerts.SendAlerts(iMillIndex, "Delta Alarm Clear");
			}

		}
		tThInfo[iMillIndex].m_bDALastState = false;
	}

}

void CEFMDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	int dlgRet;
	//DWORD dwExitCode;
	int ctr;
	CString txt;

	dlgRet = ::MessageBox(NULL, "Are you sure you wish to exit?", "Exit EFM?", MB_YESNO | MB_ICONQUESTION | MB_SYSTEMMODAL);
	switch (dlgRet)
	{
	case IDYES:
		//
		// Exit program
		//
		CWaitDlg* m_pWaitDlg;
		//
		// Shut down all threads
		//
		m_pWaitDlg = new CWaitDlg;
		m_pWaitDlg->Create(IDD_WAITDLG);
		m_pWaitDlg->ShowSplashScreen("Please Wait...", "Shutting down communication");

		// Shut down sound player
		SoundHandlerStop();
		// Write the software exiting event
		m_EventLog.LogEvent("SYSTEM EFM Software Stopped.");

		// Cleanup winsock
		WSACleanup();

		// Stop data collection threads
		for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
		{
			//				if (config.m_BDataSourceEnabled[ctr] == TRUE)
			//				{
			//					if (config.m_BDataSharingEnabled[ctr] == TRUE)
			//					{
			//						ClientsKillAll(ctr);
			//					}
			StopDAQThread(ctr, config.m_szDataSourcePoint[ctr]);
			//				} // End "if (config.m_BDataSourceEnabled[ctr] == TRUE)"
		} // End "for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)"

		// Clear the arrays
		tServer.RemoveAll();
		//paNetworkClient.RemoveAll();

		// Kill the debug window if it's active
//			DebugWindowKill();

			//
			// Close Speaker
			//
		if (m_bSpeakerOpen)
		{
			//SPEAKER_ToneOff(m_hSPEAKER);
			//SPEAKER_Close(m_hSPEAKER);
			m_bSpeakerOpen = false;
		}
		// Destroy and delete the spash screen
		m_pWaitDlg->DestroyWindow();

		delete m_pWaitDlg;

		//
		// Save configuration
		//
		//m_pWaitDlg = new CWaitDlg;
		//m_pWaitDlg->Create(IDD_WAITDLG);
		//m_pWaitDlg->ShowSplashScreen("Please Wait...","Saving configuration");
		//config.SaveConfiguration();
		//m_pWaitDlg->DestroyWindow();
		//delete m_pWaitDlg;

		// Release the mutex so we can start the program again
		::ReleaseMutex(((CEFMApp*)AfxGetMainWnd())->hMutex);
		((CEFMApp*)AfxGetMainWnd())->hMutex = NULL;

		EndDialog(0);		// Exit successfully
		//CDialog::OnClose();
		break;
	}
}

void CEFMDlg::OnDblClickFieldmill1()
{
	// TODO: Add your control notification handler code here
	//m_stripChart[0].ShowPropertyPage();
}

void CEFMDlg::OnDblClickFieldmill2()
{
	// TODO: Add your control notification handler code here
	//m_stripChart[1].ShowPropertyPage();
}

void CEFMDlg::OnDblClickFieldmill3()
{
	// TODO: Add your control notification handler code here
	//m_stripChart[2].ShowPropertyPage();
}

void CEFMDlg::OnDblClickFieldmill4()
{
	// TODO: Add your control notification handler code here
	//m_stripChart[3].ShowPropertyPage();
}

void CEFMDlg::OnFileOpen()
{
	// TODO: Add your command handler code here
	CString strPath;
	char szTempFilePath[256];
	int dlgReturn;
	CFileDialog sDlg(true);
	CString txt;

	sDlg.m_ofn.lpstrTitle = _T("Open EFM Data File...");
	sDlg.m_ofn.lpstrFilter = _T("EFM-100/200 Data (*.efm)\0*.efm\0");
	sDlg.m_ofn.lpstrDefExt = _T("efm");		// Set the initial default extension
	// Default file open box to data logging directory
	sDlg.m_ofn.lpstrInitialDir = config.m_szDataLoggingPath[0];

	dlgReturn = sDlg.DoModal();
	if (dlgReturn == IDOK)
	{
		//strFileName = sDlg.GetFileName();		// Filename + extension
		strPath = sDlg.GetPathName();			// Path + filename + extension
		//strTitle = sDlg.GetFileTitle();			// Filename, no extension
		//strFileExtension = sDlg.GetFileExt();	// Extension, no filename

		// To add a parameter to an executable, there has to be a ' ' (space) 
		// as the first character or else the system thinks that the parameter 
		// is actually part of the executable name.
		strcpy_s(szTempFilePath, 256, " \"");
		strcat_s(szTempFilePath, 256, strPath);
		strcat_s(szTempFilePath, 256, "\"");

		STARTUPINFO         siStartupInfo;
		PROCESS_INFORMATION piProcessInfo;

		memset(&siStartupInfo, 0, sizeof(siStartupInfo));
		memset(&piProcessInfo, 0, sizeof(piProcessInfo));

		siStartupInfo.cb = sizeof(siStartupInfo);

		txt.Format("%sEFMView.exe", config.m_szInstallDir);
		if (CreateProcess(txt,		// Application name
			szTempFilePath,     // Application arguments
			0,
			0,
			FALSE,
			CREATE_DEFAULT_ERROR_MODE,
			0,
			0,                              // Working directory
			&siStartupInfo,
			&piProcessInfo) == FALSE)
		{
			// Could not start application -> call 'GetLastError()'
			LPVOID lpMsgBuf;
			if (!FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR)&lpMsgBuf,
				0,
				NULL))
			{
				// Handle the error.
				::MessageBox(NULL, "There was a problem formatting the error text.", "System Error", MB_OK | MB_ICONERROR);
				return;
			} // End "if (!FormatMessage..."
			::MessageBox(NULL, (LPCTSTR)lpMsgBuf, "Error Opening File", MB_OK | MB_ICONERROR);
			return;
		} // End "if (CreateProcess..."
	} // End "if (dlgReturn == IDOK)"
}

bool CEFMDlg::IsServerActive(int idx)
{
	return tDataClientServer[idx].bStillListening;
}

bool CEFMDlg::StartDAQThread(int idx, CString szStyle)
{
	CString txt, txtIP;
	CString szSerial, szNetwork, szEFA20, txtLWS200, txtLWS300;

	szSerial.LoadString(STR_SOURCEPOINT_SERIAL);
	szNetwork.LoadString(STR_SOURCEPOINT_NETWORK);
	szEFA20.LoadString(STR_SOURCEPOINT_EFA20);
	txtLWS200.LoadString(STR_SOURCEPOINT_LWS200);
	txtLWS300.LoadString(STR_SOURCEPOINT_LWS300);

	// Set the default chart title
	//m_stripChart[idx].SetCaption(config.m_szDataSourceName[idx]);
	m_chartViewer[idx].setCaption(config.m_szDataSourceName[idx]);

	//
	// Open the log file if logging is enabled
	//
	if (config.m_BEnableDataLogging[idx] == TRUE)
	{
		if (tThInfo[idx].logData.GetLogOpen())
		{
			tThInfo[idx].logData.CloseFile();
		}
		txt.Format("%s%s-", config.m_szDataLoggingPath[idx], config.m_szDataSourceName[idx]);
		//txt.Replace("\\","\\\\");
		tThInfo[idx].logData.Start(txt, ".efm");
	}

	tThInfo[idx].SetID(idx);		// Set the ID for this object
	tThInfo[idx].ClrThreadExit();	// Clear the thread exit flag
	//tThInfo[idx].SetStrip(&m_stripChart[idx]);
	tThInfo[idx].SetChart(&m_chartViewer[idx]);
	tThInfo[idx].SetLightningChart(&m_chartLightningDistanceViewer[idx]);
	tThInfo[idx].iIntegrationValue = INTEGRATION_VALUE;
	tThInfo[idx].SetSignalLostCounter(1);
	tThInfo[idx].SetNumLED(&m_numledFieldMillValue[idx]);
	tThInfo[idx].piGraphUpdatePeriod = &m_iChartUpdatePeriod[idx];
	tThInfo[idx].piGraphUpdateCounter = &m_iChartUpdateCounter[idx];
	tThInfo[idx].pMyOwner = this;
	tThInfo[idx].iCheckLAStartup = LA_STARTUP_VAL;	// BOB MOD
	tThInfo[idx].SetLightningDistance(-1);

	// * 10 because we check for alarms ten time per second
	// + 2 glitch filter
	tThInfo[idx].m_iVHFADelayCounter = config.m_iVHFADelay[idx] + 1;
	tThInfo[idx].m_iHFADelayCounter = config.m_iHFADelay[idx] + 1;
	m_iRotorFaultWAVCounter = 0;
	m_iSelfTestFaultWAVCounter = 0;

	// Clear the client socket list
	ClientsKillAll(idx);

	//
	// SERVER THREADS
	//
	// If enabled, start the Server Listen thread.
	if (config.m_BDataSharingEnabled[idx])
	{
		StartClientServer(idx);
	} // End "if (config.m_BDataSharingEnabled[idx])"

	// Depending on the source point for the field mill, launch the 
	// appropriate thread function.  The separate functions for each 
	// possible source are purely for ease of programming only.  I could 
	// have had a single thread function that handled all sources, but then 
	// I'd have one honkin' huge function.
	if (szStyle == szSerial)	//iMode == SERIAL)	//config.m_szDataSourcePoint[ctr].CompareNoCase("Serial/USB") == 0)
	{
		tThInfo[idx].m_hSerialThreadExit = CreateEvent(NULL, TRUE, FALSE, NULL);
		thRcvData[idx] = AfxBeginThread(CEFMDlg::FieldMillSerial, &tThInfo[idx]);
		//thRcvData[idx] = AfxBeginThread(CEFMDlg::FieldMillSerial,&tThInfo[idx],THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED,NULL);
		//thRcvData[idx]->m_bAutoDelete = FALSE;
		//thRcvData[idx]->ResumeThread();
	}
	else if (szStyle == szNetwork)	//iMode == NETWORK)	//config.m_szDataSourcePoint[ctr].CompareNoCase("Network") == 0)
	{
		closesocket(tThInfo[idx].GetSocket());
		tThInfo[idx].SetSocket(socket(AF_INET, SOCK_STREAM, 0));
		tThInfo[idx].connecttoAddr.sin_family = AF_INET;
		if (isalpha(config.m_txtNetworkAddress[idx].GetAt(0)))
		{
			txtIP = ResolveHostToIP(config.m_txtNetworkAddress[idx].GetBuffer(config.m_txtNetworkAddress[idx].GetLength()));
		}
		else
		{
			txtIP = config.m_txtNetworkAddress[idx];
		}
		InetPton(AF_INET, txtIP, &tThInfo[idx].connecttoAddr.sin_addr.s_addr);
		tThInfo[idx].connecttoAddr.sin_port = htons(config.m_iNetworkPort[idx]);

		tThInfo[idx].ClrThreadExit();
		thRcvData[idx] = AfxBeginThread(CEFMDlg::FieldMillNetwork, &tThInfo[idx]);
	}
	else if (szStyle == szEFA20)
	{
		// Verify the configured serial number is still connected to the system.
		// If it is, continue as normal - if not, disable the data source.
		bool bEFA20Exists = false;
		size_t iSN = 0;

		while (!bEFA20Exists && (iSN < vEFA20Devices.size()))
		{
			tEFA20DEVICEINFO di = vEFA20Devices[iSN];
			if (config.m_txtEFA20SerialNumber[idx].Compare(di.txtSerialNumber) == 0)
			{
				bEFA20Exists = true;
			}
			else
			{
				bEFA20Exists = false;
			}
			iSN++;
		}

		if (!bEFA20Exists)
		{
			// The EFA20 that was configured for this field mill is not connected to the system.
			txt.Format("EFA-20/21 S/N: %s is not connected to the computer.  This data source will be disabled.", config.m_txtEFA20SerialNumber[idx]);
			MessageBox(txt, "EFA-20/21 Error", MB_OK | MB_ICONERROR);
			config.m_BDataSourceEnabled[idx] = FALSE;
			LayoutControls();
		}
		else
		{
			tThInfo[idx].m_txtEFA20SerialNumber = config.m_txtEFA20SerialNumber[idx];

			tThInfo[idx].m_hEFA20ThreadExit = CreateEvent(NULL, TRUE, FALSE, NULL);
			thRcvData[idx] = AfxBeginThread(CEFMDlg::FieldMillEFA20, &tThInfo[idx]);
		}
	}
	/*	else if (szStyle == txtLWS200 || szStyle == txtLWS300)
		{
			thRcvData[idx] = AfxBeginThread(FieldMillLWSx00,&tThInfo[idx]);
		}
	*/
	return true;
}

bool CEFMDlg::StopDAQThread(int idx, CString szStyle)
{
	DWORD dwExitCode;
	CString szSerial, szNetwork, szEFA20, txtLWS200, txtLWS300;

	// If the data source isn't enabled, we don't need to run this function.
	if (config.m_BDataSourceEnabled[idx] == FALSE)
	{
		return false;
	}

	//
	// Close the log file if logging is enabled
	//
	if (config.m_BEnableDataLogging[idx] == TRUE)
	{
		// Close the log file if disabled
		if (tThInfo[idx].logData.GetLogOpen())
		{
			tThInfo[idx].logData.CloseFile();
		}
	}

	szSerial.LoadString(STR_SOURCEPOINT_SERIAL);
	szNetwork.LoadString(STR_SOURCEPOINT_NETWORK);
	szEFA20.LoadString(STR_SOURCEPOINT_EFA20);
	txtLWS200.LoadString(STR_SOURCEPOINT_LWS200);
	txtLWS200.LoadString(STR_SOURCEPOINT_LWS300);
	if (szStyle == szSerial)	//iMode == SERIAL)
	{
		if (tThInfo[idx].GetThreadAlive())
		{
			tThInfo[idx].SetSerialThreadExit();	// Signal to exit the thread
			dwExitCode = ::WaitForSingleObject((CWinThread*)thRcvData[idx]->m_hThread, 3000);
			if (dwExitCode == WAIT_TIMEOUT)
			{
				// We timed out and the thread didn't exit. Kill it.
				TRACE("%s ***TerminateThread(%p): Serial Source***.\n", config.m_szDataSourceName[idx], thRcvData[idx]->m_hThread);
				TerminateThread(thRcvData[idx]->m_hThread, 11);
			}
			thRcvData[idx] = NULL;	// Set to null
		}
		CloseHandle(tThInfo[idx].m_hSerialThreadExit);
	}
	else if (szStyle == szNetwork)	//iMode == NETWORK)
	{
		if (tThInfo[idx].GetThreadAlive())
		{
			tThInfo[idx].SetThreadExit();	// Signal to exit the thread
			closesocket(tThInfo[idx].GetSocket());
			dwExitCode = ::WaitForSingleObject((CWinThread*)thRcvData[idx]->m_hThread, 3000);
			if (dwExitCode == WAIT_TIMEOUT)
			{
				// We timed out and the thread didn't exit. Kill it.
				TRACE("%s] ***TerminateThread(): Network Source***.\n", config.m_szDataSourceName[idx]);
				TerminateThread(thRcvData[idx]->m_hThread, 16);
			}
			thRcvData[idx] = NULL;	// Set to null
		}
	}
	else if (szStyle == szEFA20)
	{
		if (tThInfo[idx].GetThreadAlive())
		{
			tThInfo[idx].SetEFA20ThreadExit();	// Signal to exit the thread
			dwExitCode = ::WaitForSingleObject((CWinThread*)thRcvData[idx]->m_hThread, 3000);
			if (dwExitCode == WAIT_TIMEOUT)
			{
				// We timed out and the thread didn't exit. Kill it.
				TRACE("%s] ***TerminateThread(): EFA20 Source***.\n", config.m_szDataSourceName[idx]);
				TerminateThread(thRcvData[idx]->m_hThread, 21);
			}
			thRcvData[idx] = NULL;	// Set to null
		}
		CloseHandle(tThInfo[idx].m_hEFA20ThreadExit);
	}
	else if (szStyle == txtLWS200 || szStyle == txtLWS300)
	{
		//		tThInfo[idx].SetThreadExit();	// Signal to exit the thread
		//		::WaitForSingleObject(thRcvData[idx]->m_hThread,INFINITE);
	}

	if (IsServerActive(idx))
	{
		StopClientServer(idx);
	}

	return true;
}

// LIGHTNING ALARM
//
// ORIGINAL LIGHTNING ALARM MODIFIED FOR DISTANCE CALCULATION
// Call with field mill number (0-3) and current field mill reading (-20.0 kV/m to +20.0 kV/m)
// Returns distance in miles 0-30 if lightning detected, else returns -1

// d = k / cuberoot(f)
// where:  d = distance to lightning in miles (called distance below)
//         k = distance constant (MILES_CONSTANT, KM_CONSTANT)
//         f = field step change (called integral below)

#define MILES_CONSTANT 10.0   // used to calculate miles to lightning
#define KM_CONSTANT (MILES_CONSTANT*1.609) // used to calculate miles to lightning
#define THRESHOLD 0.04 // minimum step change for detectable lightning

int CEFMDlg::CheckForLightningAlarm(tTHREADINFO* pI, double time, int iIndex, double dReading)
{
	static double field[MAX_NUMOFDATASOURCES][2]; // last two readings of field data
	static double slope[MAX_NUMOFDATASOURCES][4]; // slope of prev field data
	static double slope2[MAX_NUMOFDATASOURCES][3]; // slope of the slope

   // TRACE("%f\n", time);

	if (config.m_BEnableLA[iIndex])
	{
		// Age the readings
		field[iIndex][1] = field[iIndex][0];
		field[iIndex][0] = dReading;

		// Calculate slope of the recent data
		slope[iIndex][3] = slope[iIndex][2];
		slope[iIndex][2] = slope[iIndex][1];
		slope[iIndex][1] = slope[iIndex][0];
		slope[iIndex][0] = field[iIndex][0] - field[iIndex][1];

		// Calculate slope of slope of the recent data
		slope2[iIndex][2] = slope2[iIndex][1];
		slope2[iIndex][1] = slope2[iIndex][0];
		slope2[iIndex][0] = slope[iIndex][0] - slope[iIndex][1];

		// integrate previous four readings so we can filter out rain noise
		double integral = fabs(slope[iIndex][3] + slope[iIndex][2] + slope[iIndex][1] + slope[iIndex][0]);

		//pI->GetStrip()->AddXY(2, time+pI->tenths, slope2[iIndex][0]);  // slope of the slope CYAN
		//pI->GetStrip()->AddXY(1, time+pI->tenths, integral); // integral of 3 post event samples ORANGE
		//TRACE("slope2[%d]: %f integral[%d]: %f\n", iIndex, slope2[iIndex][2], iIndex, integral);

		// Don't alarm if program is just starting up (and field and slope data hasn't been set yet)
		// or if we just dumped our old data due to rain noise
		if (pI->iCheckLAStartup) {
			pI->iCheckLAStartup--;
			return -1; // no lightning
		}

		// Check magnitude of slope of slope data
		if (fabs(slope2[iIndex][2]) < THRESHOLD)
			return -1; // leave, no slope changes here

		pI->iCheckLAStartup = LA_STARTUP_VAL; // dump the old values since might contain rain noise

		// Possible lightning: Check that we have a step change in field (not rain noise)
		// Integrated slope of previous readings
		if (integral < THRESHOLD)
			return -1; // leave, no step changes here

		int distance;
		// calculate miles or km to strike
		if (config.m_iLAUnit[pI->GetID()])
		{
			distance = (int)(KM_CONSTANT * pow(integral, -1.0 / 3.0) - 1.6);	// -1 mile for safety
			TRACE("**********LIGHTNING WAS DETECTED AT %d KM!***********%f,\n", distance, integral);
		}
		else
		{
			distance = (int)(MILES_CONSTANT * pow(integral, -1.0 / 3.0) - 1);	// -1 mile for safety
			TRACE("**********LIGHTNING WAS DETECTED AT %d MILES!***********%f,\n", distance, integral);
		}

		return distance; // We Found Lightning! We passed all the tests.
	}
	return -1; // lightning alarm not enabled
}

/*
// Call with field mill number (0-3) and current field mill reading (-20.0 kV/m to +20.0 kV/m)
// Returns true if lightning detected, else returns false
bool CEFMDlg::OLD_CheckForLightningAlarm(tTHREADINFO *pI, double time, int iIndex, double dReading)
{
	static double field  [MAX_NUMOFDATASOURCES][2]; // last two readings of field data
	static double slope  [MAX_NUMOFDATASOURCES][4]; // slope of prev field data
	static double slope2 [MAX_NUMOFDATASOURCES][3]; // slope of the slope

//	TRACE("%f\n", time);

	if (config.m_BEnableLA[iIndex])
	{
		// With Rain Noise Filter
		//double dThreshold = fabs(config.m_dLASensitivity) + fabs(((config.m_dRainNoiseFilter/100.0)*dReading));
		// Without Rain Noise Filter
		double dThreshold = fabs(config.m_dLASensitivity);

		// Age the readings
		field[iIndex][1] = field[iIndex][0];
		field[iIndex][0] = dReading;

		// Calculate slope of the recent data
		slope[iIndex][3] = slope[iIndex][2];
		slope[iIndex][2] = slope[iIndex][1];
		slope[iIndex][1] = slope[iIndex][0];
		slope[iIndex][0] = field[iIndex][0]-field[iIndex][1];

		// Calculate slope of slope of the recent data
		slope2[iIndex][2] = slope2[iIndex][1];
		slope2[iIndex][1] = slope2[iIndex][0];
		slope2[iIndex][0] = slope[iIndex][0]-slope[iIndex][1];

		// integrate previous four readings so we can filter out rain noise
		double integral = slope[iIndex][3] + slope[iIndex][2] + slope[iIndex][1] + slope[iIndex][0];

	//	pI->GetStrip()->AddXY(2, time+pI->tenths, slope2[iIndex][0]);  // slope of the slope CYAN
	//	pI->GetStrip()->AddXY(1, time+pI->tenths, integral); // integral of 3 post event samples ORANGE

		TRACE("slope2[%d]: %f integral[%d]: %f\n", iIndex, slope2[iIndex][2], iIndex, integral);

		// Don't alarm if program is just starting up (and field and slope data hasn't been set yet)
		// or if we just dumped our old data due to rain noise
		if (pI->iCheckLAStartup) {
			pI->iCheckLAStartup--;
			return false;
		}

		// Check magnitude of slope of slope data
		if (fabs(slope2[iIndex][2]) < dThreshold)
			return false; // leave, no slope changes here

		pI->iCheckLAStartup = LA_STARTUP_VAL; // dump the old values since might contain rain noise

		// Possible lightning: Check that we have a step change in field (not rain noise)
		// Integrated slope of previous readings
		if ((fabs(integral)) < dThreshold)
			return false; // leave, no step changes here

		TRACE("**********LIGHTNING WAS DETECTED!***********\n");
		return true; // We Found Lightning! We passed all the tests.
	}

	return false;
}
*** END OLD LIGHTNING ALARM ***/



bool CEFMDlg::CheckForHighFieldAlarm(double dReading, tTHREADINFO* pTI)
{
	if (config.m_BEnableHFA[pTI->GetID()])
	{
		// If the VHFA is not active, then we don't care about it's setting.
		if (!config.m_BEnableVHFA[pTI->GetID()])
		{
			// In this case, anything above the HFA setpoint is alarm'able.
			if (dReading >= config.m_dHFASetpoint[pTI->GetID()])
			{
				if (pTI->m_iHFADelayCounter) { pTI->m_iHFADelayCounter--; }
				if (!pTI->m_iHFADelayCounter)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				pTI->m_iHFADelayCounter = config.m_iHFADelay[pTI->GetID()] + 1;
			}
		}
		else
		{
			// If VHFA is active, then we do care about it's setting.
			if ((dReading >= config.m_dHFASetpoint[pTI->GetID()]) && (dReading < config.m_dVHFASetpoint[pTI->GetID()]))
			{
				if (pTI->m_iHFADelayCounter) { pTI->m_iHFADelayCounter--; }
				if (!pTI->m_iHFADelayCounter)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				pTI->m_iHFADelayCounter = config.m_iHFADelay[pTI->GetID()] + 1;
			}
		}
	}

	return false;
}


bool CEFMDlg::CheckForVeryHighFieldAlarm(double dReading, tTHREADINFO* pTI)
{
	if (config.m_BEnableVHFA[pTI->GetID()])
	{
		if (dReading >= config.m_dVHFASetpoint[pTI->GetID()])
		{
			if (pTI->m_iVHFADelayCounter) { pTI->m_iVHFADelayCounter--; }
			if (!pTI->m_iVHFADelayCounter)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			pTI->m_iVHFADelayCounter = config.m_iVHFADelay[pTI->GetID()] + 1;
		}
	}

	return false;
}

bool CEFMDlg::CheckForDeltaAlarm(double dReading, tTHREADINFO* pTI)
{
	if (config.m_BEnableDA[pTI->GetID()])
	{
		// Since most of work with the Delta Alarm Buffer is done in this function, we'll 
		// just lock it here instead of using individual accessor functions.
		CSingleLock lock(&pTI->m_csDeltaAlarmBuffer, TRUE);

		// Add the new element to the buffer
		pTI->m_vDeltaAlarmBuffer.push_back(dReading);
		// Make sure the Delta Alarm buffer is always kept at the proper size.
		while (static_cast<int>(pTI->m_vDeltaAlarmBuffer.size()) > (config.m_iDAInterval[pTI->GetID()] * NUM_EFMSENTENCESPERSECOND))
		{
			// Remove the 1st (oldest) element from the buffer
			pTI->m_vDeltaAlarmBuffer.erase(pTI->m_vDeltaAlarmBuffer.begin() + 0);
		}

		if (pTI->m_vDeltaAlarmBuffer.size() == (config.m_iDAInterval[pTI->GetID()] * NUM_EFMSENTENCESPERSECOND))
		{
			// Check the beginning and end elements of the buffer to determine if the alarm
			// has to activate.
			double dOldest = pTI->m_vDeltaAlarmBuffer[0];
			double dNewest = pTI->m_vDeltaAlarmBuffer[pTI->m_vDeltaAlarmBuffer.size() - 1];
			double dXDelta = fabs(dOldest - dNewest);
			if (dXDelta >= config.m_dDASetpoint[pTI->GetID()])
			{
				// Activate the alarm
				if (pTI->m_iDADelayCounter) { pTI->m_iDADelayCounter--; }
				if (!pTI->m_iDADelayCounter)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				pTI->m_iDADelayCounter = config.m_iDADelay[pTI->GetID()] + 1;
			}
		}
	} // if (config.m_BEnableDA[pTI->GetID()])
	return false;
}

void CEFMDlg::ProcessAlarms(int iFID)
{
	int ctr;
	CString txt;

	ctr = iFID;

	if (config.m_BDataSourceEnabled[ctr])
	{
		// Processing must pass the rotor fault test and the self test fault checks
		// before it even thinks about alarm processing.
		if (tThInfo[ctr].m_bRotorFault)
		{
			// Set and bail
			if (!m_iRotorFaultWAVCounter && tThInfo[ctr].m_bSoundOn)
			{
				PlayWAV(iFID, config.m_txtWAVRotorFault);
			}
			m_iRotorFaultWAVCounter = ROTORFAULT_INTERVAL;
			this->PostMessage(WM_SETCONTROLCOLOURS, (WPARAM)ctr, (LPARAM)COLOUR_DISABLED);
			// Status Error
			SetStatusText("Rotor Fault");
			SetStatusControl(ctr, TRUE);
			// Log the fault
			if (!tThInfo[ctr].m_bLoggedRotorFault)
			{
				txt.Format("%s Rotor Fault.", config.m_szDataSourceName[ctr]);
				m_EventLog.LogEvent(txt);
				// Email & SMS Fault
				if (config.bAlertsFaultsActive)
				{
					Alerts.SendAlerts(ctr, "Rotor Fault");
				}

				tThInfo[ctr].m_bLoggedRotorFault = true;
			}
			return;
		}
		else
		{
			// No more rotor fault
			if (tThInfo[ctr].m_bLoggedRotorFault)
			{
				txt.Format("%s Rotor OK.", config.m_szDataSourceName[ctr]);
				m_EventLog.LogEvent(txt);
				// Email & SMS Fault
				if (config.bAlertsFaultsClear)
				{
					Alerts.SendAlerts(ctr, "Rotor OK");
				}
				tThInfo[ctr].m_bLoggedRotorFault = false;
				SetStatusText("");
				SetStatusControl(ctr, FALSE);
			}
		}

		if (tThInfo[ctr].m_bSelfTestFault)
		{
			// Set and bail
			if (!m_iSelfTestFaultWAVCounter && tThInfo[ctr].m_bSoundOn)
			{
				PlayWAV(iFID, config.m_txtWAVSelfTestFault);
			}
			m_iSelfTestFaultWAVCounter = SELFTESTFAULT_INTERVAL;
			this->PostMessage(WM_SETCONTROLCOLOURS, (WPARAM)ctr, (LPARAM)COLOUR_DISABLED);
			// Status Error
			SetStatusText("Self-Test Fault");
			SetStatusControl(ctr, TRUE);
			//txt.Format("%s (Self-Test Fault)",config.m_szDataSourceName[ctr]);
			//m_stripChart[ctr].SetCaption(txt);
			// There is currently no "Self Test Fault" WAV file
			// Log the fault
			if (!tThInfo[ctr].m_bLoggedSelfTestFault)
			{
				txt.Format("%s Self-Test Fault.", config.m_szDataSourceName[ctr]);
				m_EventLog.LogEvent(txt);
				// Email & SMS Fault
				if (config.bAlertsFaultsActive)
				{
					Alerts.SendAlerts(ctr, "Self-Test Fault");
				}

				tThInfo[ctr].m_bLoggedSelfTestFault = true;
			}
			return;
		}
		else
		{
			// No more self-test fault
			if (tThInfo[ctr].m_bLoggedSelfTestFault)
			{
				txt.Format("%s Self-Test OK.", config.m_szDataSourceName[ctr]);
				m_EventLog.LogEvent(txt);
				// Email & SMS Fault
				if (config.bAlertsFaultsClear)
				{
					Alerts.SendAlerts(ctr, "Self-Test OK");
				}

				tThInfo[ctr].m_bLoggedSelfTestFault = false;
				SetStatusText("");
				SetStatusControl(ctr, FALSE);
			}
		}

		// If neither the rotor fault flag or the self test fault flag are set, 
		// make sure the chart title just shows the name.
		if (!tThInfo[ctr].m_bRotorFault && !tThInfo[ctr].m_bSelfTestFault)
		{
			// Get the current caption so we don't erase something that's already there
			//txt = m_stripChart[ctr].GetCaption();
			txt = m_chartViewer[ctr].getCaption();
			//txt.Format("%s",config.m_szDataSourceName[ctr]);
			//m_stripChart[ctr].SetCaption(txt);
			m_chartViewer[ctr].setCaption(txt);
		}


		// if VHF alarm is waiting to timeout
		if (tThInfo[ctr].m_iVHFACounter)
		{
			ActivateVeryHighFieldAlarm(ctr, ALARM_ENNUN | ALARM_AUDIO);
			DeactivateHighFieldAlarm(ctr, ALARM_ENNUN | ALARM_AUDIO);

			// set and clear appropriate bits in playWAV flag
			if (config.m_BVHFAWAV[ctr])
			{
				if (tThInfo[ctr].m_bSoundOn)
				{
					tThInfo[ctr].m_bPlayVHFAWAV = true;
				}
				else
				{
					tThInfo[ctr].m_bPlayVHFAWAV = false;
				}
				tThInfo[ctr].m_bPlayHFAWAV = false;
			}

			// only turn red if no lightning alarm
			if (!tThInfo[ctr].m_iLACounter)
				ActivateVeryHighFieldAlarm(ctr, ALARM_VISUAL);

			// Log if alarm has just activated (not reactivating)
			if (!tThInfo[ctr].m_bVHFALastState)
			{
				// Alarm just activated
				txt.Format("%s Very High Field Alarm Active.", config.m_szDataSourceName[ctr]);
				m_EventLog.LogEvent(txt);
				// Send email and SMS
				if (config.bAlertsVHFActive)
				{
					Alerts.SendAlerts(ctr, "Very High Field Alarm Active");
				}
				tThInfo[ctr].m_bVHFALastState = true;
			}

		} // if VHF alarm is waiting to timeout
		else
		{ // no VHF alarm
			DeactivateVeryHighFieldAlarm(ctr, ALARM_ENNUN | ALARM_VISUAL | ALARM_AUDIO);

			// if HF alarm is waiting to timout
			if (tThInfo[ctr].m_iHFACounter)
			{
				ActivateHighFieldAlarm(ctr, ALARM_ENNUN | ALARM_AUDIO);
				DeactivateVeryHighFieldAlarm(ctr, ALARM_ENNUN | ALARM_AUDIO);

				// set and clear appropriate bits in playWAV flag
				if (config.m_BHFAWAV[ctr])
				{
					if (tThInfo[ctr].m_bSoundOn)
					{
						tThInfo[ctr].m_bPlayHFAWAV = true;
					}
					else
					{
						tThInfo[ctr].m_bPlayHFAWAV = false;
					}
					tThInfo[ctr].m_bPlayVHFAWAV = false;
				}

				// only turn yellow if no lightning alarm
				if (!tThInfo[ctr].m_iLACounter)
					ActivateHighFieldAlarm(ctr, ALARM_VISUAL);

				// Log if alarm has just activated (not reactivating)
				if (!tThInfo[ctr].m_bHFALastState)
				{
					// Alarm just activated
					txt.Format("%s High Field Alarm Active.", config.m_szDataSourceName[ctr]);
					m_EventLog.LogEvent(txt);
					// Email & SMS
					if (config.bAlertsHFActive)
					{
						Alerts.SendAlerts(ctr, "High Field Alarm Active");
					}
					tThInfo[ctr].m_bHFALastState = true;
				}

			}
			else
			{
				// HF alarm is NOT waiting to timout
				DeactivateHighFieldAlarm(ctr, ALARM_ENNUN | ALARM_VISUAL | ALARM_AUDIO);
				// clear appropriate bits in playWAV flag
				tThInfo[ctr].m_bPlayHFAWAV = false;
			} // HF alarm is not waiting to timeout
		} // no VHF alarm

		if (tThInfo[ctr].m_iDACounter)
		{
			ActivateDeltaAlarm(ctr, ALARM_ENNUN | ALARM_AUDIO);
			// Set and clear appropriate bits in the playWAV flag
			if (config.m_BDAWAV[ctr])
			{
				if (tThInfo[ctr].m_bSoundOn)
				{
					tThInfo[ctr].m_bPlayDAWAV = true;
				}
				else
				{
					tThInfo[ctr].m_bPlayDAWAV = false;
				}
			}
			// Log if alarm has just activated (not reactivating)
			if (!tThInfo[ctr].m_bDALastState)
			{
				// Alarm just activated
				txt.Format("%s Delta Alarm Active.", config.m_szDataSourceName[ctr]);
				m_EventLog.LogEvent(txt);
				// Email & SMS
				if (config.bAlertsDAActive)
				{
					Alerts.SendAlerts(ctr, "Delta Alarm Active");
				}
				tThInfo[ctr].m_bDALastState = true;
			}
		}
		else
		{
			// Delta alarm is NOT waiting to timout
			tThInfo[ctr].m_bPlayDAWAV = false;
		}


		// If everything's happy, turn green
		if (!tThInfo[ctr].m_iLACounter &&
			!tThInfo[ctr].m_iVHFACounter &&
			!tThInfo[ctr].m_iHFACounter &&
			!tThInfo[ctr].m_iDACounter)
		{
			DeactivateLightningAlarm(ctr, ALARM_VISUAL | ALARM_AUDIO);
			DeactivateVeryHighFieldAlarm(ctr, ALARM_ENNUN | ALARM_VISUAL | ALARM_AUDIO);
			DeactivateHighFieldAlarm(ctr, ALARM_ENNUN | ALARM_VISUAL | ALARM_AUDIO);
			DeactivateDeltaAlarm(ctr, ALARM_ENNUN | ALARM_VISUAL | ALARM_AUDIO);

			// clear appropriate bit in playWAV flags
			tThInfo[ctr].m_bPlayLAOnActivationWAV = false;
			tThInfo[ctr].m_bPlayLADistanceWAV = false;
			tThInfo[ctr].m_bPlayLAAllClearWAV = false;
			tThInfo[ctr].m_bPlayVHFAWAV = false;
			tThInfo[ctr].m_bPlayHFAWAV = false;
			tThInfo[ctr].m_bPlayDAWAV = false;

			// Everything's happy, turn green
			this->PostMessage(WM_SETCONTROLCOLOURS, (WPARAM)ctr, (LPARAM)COLOUR_ENABLED);
		}

		if (tThInfo[ctr].m_iLACounter)
		{
			ActivateLightningAlarm(ctr, ALARM_VISUAL | ALARM_AUDIO, tThInfo[ctr].GetLightningDistance());
		}
		else
		{
			DeactivateLightningAlarm(ctr, ALARM_ENNUN | ALARM_VISUAL | ALARM_AUDIO);
			// clear appropriate bit in playWAV flag
		}
	} // End "if (config.m_BDataSourceEnabled[ctr])"

}

void CEFMDlg::ProcessWAVAlarms(int iMID)
{
	CString txt;

	// LA OnActivation
	if (tThInfo[iMID].m_bSoundOn && config.m_BLAOnActivationEnable[iMID] && tThInfo[iMID].m_bPlayLAOnActivationWAV)
	{
		PlayWAV(iMID, config.m_txtLAOnActivationWAVFile[iMID]);
		tThInfo[iMID].m_bPlayLAOnActivationWAV = false;
	}

	// Distance Enunciation
	if (tThInfo[iMID].m_bSoundOn && config.m_BLADistanceEnable[iMID] && tThInfo[iMID].m_bPlayLADistanceWAV)
	{
		CString txtFilename;

		if (!tThInfo[iMID].m_iLightningDetectedAtCtr)
		{
			txtFilename.Format("%sLightningDetectedAt.wav", config.m_txtLADistanceWAVFiles[iMID]);
			PlayWAV(iMID, txtFilename);
		}

		txtFilename.Format("%s%d.wav", config.m_txtLADistanceWAVFiles[iMID], tThInfo[iMID].GetLightningDistance());
		PlayWAV(iMID, txtFilename);
		txtFilename.Format("%s%s.wav", config.m_txtLADistanceWAVFiles[iMID], (config.m_iLAUnit[iMID] == MILES) ? "Miles" : "Kilometres");
		PlayWAV(iMID, txtFilename);
		tThInfo[iMID].m_bPlayLADistanceWAV = false;
		tThInfo[iMID].m_iLightningDetectedAtCtr = LIGHTNINGDETECTEDAT_INTERVAL;
	}

	// LA AllClear
	if (tThInfo[iMID].m_bSoundOn && config.m_BLAAllClearEnable[iMID] && tThInfo[iMID].m_bPlayLAAllClearWAV)
	{
		PlayWAV(iMID, config.m_txtLAAllClearWAVFile[iMID]);
		tThInfo[iMID].m_bPlayLAAllClearWAV = false;
	}

	// LA Repeat
	if (tThInfo[iMID].m_bSoundOn && config.m_BLARepeatEnable[iMID] && tThInfo[iMID].m_iLACounter && (tThInfo[iMID].m_iLAWAVCounter <= 0))
	{
		PlayWAV(iMID, config.m_txtLARepeatWAVFile[iMID]);
		tThInfo[iMID].m_iLAWAVCounter = (int)(config.m_dLAWAVRepeatInterval[iMID] * 60.0);
	}

	// Very High Field WAV
	if (tThInfo[iMID].m_bSoundOn && tThInfo[iMID].m_bPlayVHFAWAV && !tThInfo[iMID].m_iVHFAWAVCounter)
	{
		PlayWAV(iMID, config.m_szVHFAWAVFile[iMID]);
		tThInfo[iMID].m_iVHFAWAVCounter = (int)config.m_dVHFAWAVRepeatInterval[iMID];
		tThInfo[iMID].m_bPlayVHFAWAV = false;
	}
	// High Field WAV
	if (tThInfo[iMID].m_bSoundOn && tThInfo[iMID].m_bPlayHFAWAV && !tThInfo[iMID].m_iHFAWAVCounter)
	{
		PlayWAV(iMID, config.m_szHFAWAVFile[iMID]);
		tThInfo[iMID].m_iHFAWAVCounter = (int)config.m_dHFAWAVRepeatInterval[iMID];
		tThInfo[iMID].m_bPlayHFAWAV = false;
	}

	// Delta WAV
	if (tThInfo[iMID].m_bSoundOn && tThInfo[iMID].m_bPlayDAWAV && !tThInfo[iMID].m_iDAWAVCounter)
	{
		PlayWAV(iMID, config.m_szDAWAVFile[iMID]);
		tThInfo[iMID].m_iDAWAVCounter = (int)config.m_dDAWAVRepeatInterval[iMID];
		tThInfo[iMID].m_bPlayDAWAV = false;
	}

}

void CEFMDlg::OnFileVieweventlog()
{
	// TODO: Add your command handler code here
	CEventLogViewer dlgEventLogViewer;
	dlgEventLogViewer.pMyOwner = this;

	int iDlgRet = dlgEventLogViewer.DoModal();
}


void CEFMDlg::SetStatusText(CString txtMsg)
{
	m_txtStatusBarText = txtMsg;
}

void CEFMDlg::SetStatusControl(int iID, BOOL BDisplay)
{
	CRect rect;
	const int iFontScaleDivider = 40;

	if (BDisplay)
	{
		// If we're displaying the status control then make sure
		// the alarm visuals are disabled.
		m_pctHighFieldVisual[iID].ShowWindow(FALSE);
		m_pctVeryHighFieldVisual[iID].ShowWindow(FALSE);
		m_pctLightningVisual[iID].ShowWindow(FALSE);
	}
	m_pctStatusVisual[iID].GetClientRect(&rect);
	m_pctStatusVisual[iID].SetFontSize(static_cast<float>(rect.Width()) / iFontScaleDivider);
	m_pctStatusVisual[iID].SetDigitalFormat(m_txtStatusBarText);
	m_pctStatusVisual[iID].ShowWindow(BDisplay);
}

void CEFMDlg::OnClickMutesoundFieldmill1()
{
	BOOL BVal = m_tglMuteSound[0].GetValue();
	tThInfo[0].m_bSoundOn = (BVal != 0);
}

void CEFMDlg::OnClickMutesoundFieldmill2()
{
	BOOL BVal = m_tglMuteSound[1].GetValue();
	tThInfo[1].m_bSoundOn = (BVal != 0);
}

void CEFMDlg::OnClickMutesoundFieldmill3()
{
	BOOL BVal = m_tglMuteSound[2].GetValue();
	tThInfo[2].m_bSoundOn = (BVal != 0);
}

void CEFMDlg::OnClickMutesoundFieldmill4()
{
	BOOL BVal = m_tglMuteSound[3].GetValue();
	tThInfo[3].m_bSoundOn = (BVal != 0);
}

HBRUSH CEFMDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_DLG)
	{
		return m_brushBackground;
	}

	return hbr;
}

CString CEFMDlg::CalculateChecksum(CString txtSentence)
{
	CString txtCS;
	short int siCS = 0;
	for (int iIdx = 0; iIdx < txtSentence.GetLength() - 4; iIdx++)
	{
		siCS += txtSentence.GetAt(iIdx);
	}
	txtCS.Format("%X", (siCS & 0x00FF));
	return txtCS;
}

int CEFMDlg::ReceiveData(SOCKET sock, char* data, int len, int flags)
{
	// ************ setsockopt to set a timeout for recv

	char* position = data;
	int dataRemain = len;

	// Try to receive all data
	while (dataRemain > 0) {

		// Receive as much data as we can, up to len bytes
		int dataRx = recv(sock, position, dataRemain, flags);

		// dataRx returns number of bytes read or 0 for graceful close -1 on error
		// if -1 WSAGetLastError() would get the reason
		if (dataRx <= 0)
			break;

		// Move past the data sent
		position += dataRx;
		dataRemain -= dataRx;
	}

	return position - data;
}
/*
// LWS EFM THREAD
UINT CEFMDlg::FieldMillLWSx00(LPVOID pParam)
{
	tTHREADINFO * pInfo = (tTHREADINFO*)pParam;
	fd_set fdsReceive;
	struct timeval  tval;
	tval.tv_sec  = 0;
	tval.tv_usec = 0;


	if (!LWS300_Open())
	{
		::MessageBox(NULL, "Unable to open the LWS-300.", "LWS-300 Connection Error", MB_OK|MB_ICONERROR);
		tThInfo[idx].SetThreadExit();	// Signal to exit the thread
	}

	while (!pInfo->GetThreadExit())
	{
		if (LWS300_MessageReady())
		{
			LWS300_MessageDataType message;
			LWS300_GetMessageData(&message);
			if ( ( !strncmp((char*) &message, "$-",2) ) || (!strncmp((char*) &message, "$+",2) ))
			{
				// EFM String
				// If data sharing is enabled, send the sentence value down the pipe to the
				// server thread.  To allow network clients to use their alarms on the
				// network data, we need to send all data (10 times a second) over the
				// socket to the client.
				//saFMData[pInfo->GetID()].Format("%s", message.message);	//sentence;
				if (config.m_BDataSharingEnabled[pInfo->GetID()])
				{
					for (int iPtr=0; iPtr<pInfo->sockDataServer.GetSize(); iPtr++)
					{
						SOCKET s = pInfo->sockDataServer.GetAt(iPtr);
						bool bDropSocket = false;

						// Check to see if the client tried to send any data to us
						FD_ZERO(&fdsReceive);		// Clear the fd_set
						FD_SET(s, &fdsReceive);		// Add this socket to the set

						// Run select() on the socket to check its status
						select(0, &fdsReceive, NULL, NULL, &tval);
						if (FD_ISSET(s, &fdsReceive))
						{
							// The client tried to send data to us, or the connection
							// has been closed/reset/terminated - lop his nuts off.
							bDropSocket = true;
						}
						else
						{
							bDropSocket = false;
						}

						if (!bDropSocket)
						{
							if (send(s, message.message, strlen(message.message), 0) == SOCKET_ERROR)
							{
								bDropSocket = true;
							}
							else
							{
								bDropSocket = false;
							}
						}

						if (bDropSocket)
						{
							closesocket(s);
							pInfo->m_csDataServer.Lock();
							pInfo->sockDataServer.RemoveAt(iPtr);
							pInfo->m_csDataServer.Unlock();
							CString newtitle;
							newtitle.Format("%s (%d Clients Connected)",config.m_szDataSourceName[pInfo->GetID()],pInfo->sockDataServer.GetSize());
							((CEFMDlg*)pInfo->pMyOwner)->SetChartTitle(pInfo->GetID(), newtitle);
						}

/*						if (send(s, message.message, strlen(message.message), 0) == SOCKET_ERROR)
						{
							closesocket(s);
							pInfo->m_csDataServer.Lock();
							pInfo->sockDataServer.RemoveAt(iPtr);
							pInfo->m_csDataServer.Unlock();
							CString newtitle;
							newtitle.Format("%s (%d Clients Connected)",config.m_szDataSourceName[pInfo->GetID()],pInfo->sockDataServer.GetSize());
							((CEFMDlg*)pInfo->pMyOwner)->SetChartTitle(pInfo->GetID(), newtitle);
						}
end comment
					}
					// Send a message to the appropriate server
					//((CServerClass*)pInfo->pSC)->PostThreadMessage(WM_FIELDMILLDATA,0,(LPARAM)&saFMData[pInfo->GetID()]);
				}

				CEFMDlg::ProcessEFMSentence(saFMData[pInfo->GetID()], pInfo, &pInfo->logData);
			}
			else
			{
				// Message String
			}
		}
		::Sleep(10);
	}

	LWS300_Close();

	return 0;
}
*/
UINT CEFMDlg::DataClientServer(LPVOID pParam)
{
	tDATACLIENTSERVER* pMyInfo = (tDATACLIENTSERVER*)pParam;

	WSADATA wsaData;
	sockaddr_in local;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		pMyInfo->bStillListening = false;
		return 0;
	}

	pMyInfo->mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (pMyInfo->mySocket != INVALID_SOCKET)
	{
		local.sin_family = AF_INET;
		local.sin_addr.s_addr = INADDR_ANY;  //inet_addr(pParent->m_txtBindToIPAddress.GetBuffer(pParent->m_txtBindToIPAddress.GetLength()));
		local.sin_port = htons((u_short)pMyInfo->iListenPort);

		if (bind(pMyInfo->mySocket, (SOCKADDR*)&local, sizeof(local)) != SOCKET_ERROR)
		{
			if (listen(pMyInfo->mySocket, 10) != SOCKET_ERROR)
			{
				sockaddr_in from;
				int fromlen = sizeof(from);
				pMyInfo->bStillListening = true;

				// Loop creating a new thread for each connection
				while (pMyInfo->bStillListening)
				{
					SOCKET tempSocket;
					tempSocket = accept(pMyInfo->mySocket, (struct sockaddr*)&from, &fromlen);
					if (tempSocket != INVALID_SOCKET)
					{
						// Record the IP address of the last connected client.  We could 
						// store the ip address of all the connected clients but we don't 
						// need to.
						InetNtop(AF_INET, &from.sin_addr, pMyInfo->szClientIP, 32);
						//strcpy_s(pMyInfo->szClientIP, 32, inet_ntoa(from.sin_addr));

						// Find the next available client slot
						int iNextFreeSlot = pMyInfo->pThreadInfo->FindNextFreeClient();
						if (iNextFreeSlot < 0)
						{
							// There are no more available slots for clients, so disconnect this one.
							closesocket(tempSocket);
						}
						else
						{
							// Configure this slot for the new client
							pMyInfo->pThreadInfo->clientsConnected[iNextFreeSlot].SetIPAddress(pMyInfo->szClientIP);
							pMyInfo->pThreadInfo->clientsConnected[iNextFreeSlot].SetDataSourceName(config.m_szDataSourceName[pMyInfo->iID]);
							pMyInfo->pThreadInfo->clientsConnected[iNextFreeSlot].SetTimeDateConnected(CTime::GetCurrentTime());
							//	pMyInfo->pThreadInfo->clientsConnected[iNextFreeSlot].SetDebuggerWindow(((CEFMDlg*)pMyInfo->pMyOwner)->m_pDlgDebugger);
							pMyInfo->pThreadInfo->clientsConnected[iNextFreeSlot].SetSocket(tempSocket);
							pMyInfo->pThreadInfo->clientsConnected[iNextFreeSlot].SetThreadExitHandle(CreateEvent(NULL, TRUE, FALSE, NULL));
							pMyInfo->pThreadInfo->clientsConnected[iNextFreeSlot].SetThreadPtr(AfxBeginThread(CEFMDlg::ThreadConnectedClient, &pMyInfo->pThreadInfo->clientsConnected[iNextFreeSlot]));
							TRACE("New client -> Slot %d Thread %p\n", iNextFreeSlot, pMyInfo->pThreadInfo->clientsConnected[iNextFreeSlot].GetThreadPtr());

							// Update the number of connected clients in the debug window
							if (((CEFMDlg*)pMyInfo->pMyOwner)->m_pDlgDebugger)
							{
								((CEFMDlg*)pMyInfo->pMyOwner)->m_pDlgDebugger->SetClientCount(pMyInfo->pThreadInfo->ClientsCount());
							}

							// Update the number of connected clients on the appropriate stripchart control
							CString newtitle;
							newtitle.Format("%s (%d Clients Connected)", config.m_szDataSourceName[pMyInfo->iID], pMyInfo->pThreadInfo->ClientsCount());
							((CEFMDlg*)pMyInfo->pMyOwner)->SetChartTitle(pMyInfo->iID, newtitle);
						} // if (iNextFreeSlot < 0)

					} //if (tempSocket != INVALID_SOCKET)
					else
					{	// INVALID_SOCKET
						closesocket(tempSocket);
					}
				} // while (pMyInfo->bStillListening)
			} // if (listen(pMyInfo->mySocket, 10) != SOCKET_ERROR)
		} // if (bind(pMyInfo->mySocket, (SOCKADDR*)&local, sizeof(local)) != SOCKET_ERROR)
	} // if (pMyInfo->mySocket != INVALID_SOCKET)

	TRACE("DataClientServer() Thread Ending Code 0\n");
	closesocket(pMyInfo->mySocket);
	AfxEndThread(0);
	return 0;
}

void CEFMDlg::SetChartTitle(int iID, CString txtNewTitle)
{
	m_csChartChange[iID].Lock();
	//m_stripChart[iID].SetCaption(txtNewTitle);
	m_chartViewer[iID].setCaption(txtNewTitle);
	m_csChartChange[iID].Unlock();
}

void CEFMDlg::StartClientServer(int iIndex)
{
	tDataClientServer[iIndex].iID = iIndex;
	// Set the port we want this thread to listen on.
	tDataClientServer[iIndex].iListenPort = config.m_iDataSharingPort[iIndex];
	// Associate this field mill's thread info
	tDataClientServer[iIndex].pThreadInfo = &tThInfo[iIndex];
	// Set the pointer to our owner
	tDataClientServer[iIndex].pMyOwner = this;
	// Start the client listener thread.  
	tDataClientServer[iIndex].pThread = AfxBeginThread(CEFMDlg::DataClientServer, &tDataClientServer[iIndex]);
}

void CEFMDlg::StopClientServer(int iIndex)
{
	tDataClientServer[iIndex].bStillListening = false;
	closesocket(tDataClientServer[iIndex].mySocket);

	// Clear all the connected clients
//	for (int ptr=0; ptr<tThInfo[iIndex].sockDataServer.GetSize(); ptr++)
//	{
//		closesocket(tThInfo[iIndex].sockDataServer.GetAt(ptr));
//	}
//	tThInfo[iIndex].sockDataServer.RemoveAll();
}

void CEFMDlg::RRAdd(int iMID, unsigned int uiDistance)
{
	int iFoundIndex = 0;
	bool bFound = false;
	CUIntArray uaBins;
	char* tok = NULL;
	char seps[] = ",";
	CString txtRings;
	char* next_token = NULL;
	// Parse the ranges into an array
	txtRings = config.m_szLARingRanges[iMID];
	tok = strtok_s(txtRings.GetBuffer(txtRings.GetLength()), seps, &next_token);
	uaBins.Add(atoi(tok));
	while (tok)
	{
		//		TRACE("Adding a range ring.\n");
		tok = strtok_s(NULL, seps, &next_token);
		if (tok)
		{
			uaBins.Add(atoi(tok));
		}
	}

	// Each range represents a bin.  If a strike distance falls within a bin then that bin's
	// counter is reset to maximum.
	// - Determine which bin this strike falls within
	// - Is that bin already present?
	//     Yes
	//       Reset the counter to max value
	//     No
	//       Construct a new bin structure
	//       Set the counter to max value
	//       Add to array

	int iContainingBin = 0;
	for (int iBin = 0; iBin < uaBins.GetSize(); iBin++)
	{
		if (uiDistance <= uaBins.GetAt(iBin))
		{
			iContainingBin = uaBins.GetAt(iBin);
			break;
		}
	}

	// Is this bin already on the list?
	for (int iIdx = 0; iIdx < m_aRRCountdown[iMID].GetSize(); iIdx++)
	{
		if ((m_aRRCountdown[iMID][iIdx].iMillID == iMID) && (m_aRRCountdown[iMID][iIdx].uiBin == iContainingBin))
		{
			bFound = true;
			iFoundIndex = iIdx;
		}
	}

	if (bFound)
	{
		// Is on the list
		// Reset the counter
		m_aRRCountdown[iMID][iFoundIndex].iCount = (unsigned int)(config.m_dLADuration[iMID] * 60.0);
	}
	else
	{
		// Is not on the list
		// Create a new entry
		tRRCOUNTDOWN rr;
		rr.iMillID = iMID;
		rr.uiBin = iContainingBin;
		rr.iCount = (unsigned int)(config.m_dLADuration[iMID] * 60.0);
		m_aRRCountdown[iMID].Add(rr);
	}

	// Enable the appropriate ring
	m_laLightningAlarm[iMID].SetLightningAlarm(TRUE, iContainingBin);
}

UINT CEFMDlg::SoundHandlerThread(LPVOID pParam)
{
	CEFMDlg* pParent = (CEFMDlg*)pParam;
	bool bDone = false;
	DWORD dwObj;

	while (!bDone)
	{
		dwObj = ::WaitForSingleObject(pParent->m_hSoundHandlerThreadExit, 50);
		if (dwObj == WAIT_OBJECT_0)
		{
			bDone = true;
			continue;
		}

		if (pParent->m_aSoundHandlerPlaylist.GetSize() > 0)
		{
			// There are WAV file(s) waiting to be played
			tFILETOPLAY ftp = pParent->SoundHandlerGetOldestWAV();
			if ((pParent->tThInfo[ftp.iID].m_bSoundOn) || (ftp.iID == 0))	// '0' is unmutable system wav
			{
				::sndPlaySound(ftp.txtFile, SND_SYNC | SND_NODEFAULT);
			}
		}
	}
	AfxEndThread(0);
	return 0;
}

void CEFMDlg::SoundHandlerStart()
{
	if (!m_pSoundHandlerThread)
	{
		m_hSoundHandlerThreadExit = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_pSoundHandlerThread = AfxBeginThread(CEFMDlg::SoundHandlerThread, this);
	}
}

void CEFMDlg::SoundHandlerStop()
{
	if (m_pSoundHandlerThread)
	{
		// Clear the playlist
		m_csSoundHandlerWAVFileList.Lock();
		m_aSoundHandlerPlaylist.RemoveAll();
		m_csSoundHandlerWAVFileList.Unlock();
		// Set the event to make the player exit
		::SetEvent(m_hSoundHandlerThreadExit);
		// Wait for the thread to exit
		::WaitForSingleObject(m_pSoundHandlerThread->m_hThread, 5000);
	}
}

tFILETOPLAY CEFMDlg::SoundHandlerGetOldestWAV()
{
	tFILETOPLAY ftp;
	m_csSoundHandlerWAVFileList.Lock();
	ftp = m_aSoundHandlerPlaylist.GetAt(0);
	m_aSoundHandlerPlaylist.RemoveAt(0);
	m_csSoundHandlerWAVFileList.Unlock();
	return ftp;
}

void CEFMDlg::PlayWAV(int iID, CString txtFilename)
{
	m_csSoundHandlerWAVFileList.Lock();
	tFILETOPLAY ftp;
	ftp.iID = iID;
	ftp.txtFile = txtFilename;
	m_aSoundHandlerPlaylist.Add(ftp);
	m_csSoundHandlerWAVFileList.Unlock();
}

UINT CEFMDlg::ThreadConnectedClient(LPVOID pParam)
{
	TRACE("Begin ThreadConnectedClient %p\n", GetCurrentThreadId());

	CConnectedClient* pCC = (CConnectedClient*)pParam;
	DWORD dwObj;
	bool bDone = false;
	std::string txtData;
	fd_set fdsReceive;
	timeval  tval;
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	int iDebugID = -1;

	pCC->SetIsAlive(true);

	if (pCC->GetDebuggerWindow())
	{
		iDebugID = pCC->GetDebuggerWindow()->AddClient(pCC->GetIPAddress());
		// "iDebugID" is the ID that this client can use to update their debuggine window info.
	}

	while (!bDone)
	{
		dwObj = WaitForSingleObject(pCC->GetThreadExitHandle(), 50);
		switch (dwObj)
		{
		case WAIT_OBJECT_0:		// Time to exit
			bDone = true;
			break;
		case WAIT_TIMEOUT:
			while ((pCC->FifoGetSize() > 0) && !bDone)
			{
				// Get the oldest sentence off of our fifo and remove it
				txtData = pCC->FifoRemove();

				//if (pCC->GetDebuggerWindow())
				//{
				//	pCC->GetDebuggerWindow()->UpdateClient(iDebugID, pCC->FifoGetSize());
				//}

				bool bDropSocket = false;

				// Check to see if the client tried to send any data to us
				tval.tv_sec = 0;
				tval.tv_usec = 0;
				FD_ZERO(&fdsReceive);		// Clear the fd_set
				FD_SET(pCC->GetSocket(), &fdsReceive);		// Add this socket to the set

				// Run select() on the socket to check its status
				select(0, &fdsReceive, NULL, NULL, &tval);
				if (FD_ISSET(pCC->GetSocket(), &fdsReceive))
				{
					// The client tried to send data to us, or the connection 
					// has been closed/reset/terminated. Disconnect.
					bDropSocket = true;
				}
				else
				{
					bDropSocket = false;
				}

				if (!bDropSocket)
				{
					if (send(pCC->GetSocket(), txtData.data(), txtData.length(), 0) == SOCKET_ERROR)
					{
						bDropSocket = true;
					}
					else
					{
						bDropSocket = false;
					}
				}

				if (bDropSocket)
				{
					//TRACE("Dropped Socket: %s",config.m_szDataSourceName[pInfo->GetID()]);
//						TRACE("Dropped Socket\n");
					bDone = true;
				}
			} // while
			break;
		} // switch
	} // while
	closesocket(pCC->GetSocket());
	pCC->FifoClearAll();
	pCC->SetIsAlive(false);

	TRACE("End ThreadConnectedClient %p Exit code 30 IP: %s\n", GetCurrentThreadId(), pCC->GetIPAddress());

	//try
	//{
	//	CloseHandle(pCC->GetThreadExitHandle()); // CRASHED invalid handle
	//}
	//catch (...)
	//{
	//	TRACE("CATCH: ThreadConnectedClient() Invalid handle at CloseHandle(pCC->GetThreadExitHandle()) %d\n", pCC->GetThreadExitHandle());
	//}

	AfxEndThread(30);
	return 30;
}

void CEFMDlg::ClientsKillAll(int iDataSource)
{
	//	TRACE("Beginning ClientsKillAll\n");
	int iIdx;
	for (iIdx = 0; iIdx < MAX_NETWORKCLIENTS; iIdx++)
	{
		TRACE("ClientsKillAll doing GetIsAlive for source %d client index %d\n", iDataSource, iIdx);
		if (tThInfo[iDataSource].clientsConnected[iIdx].GetIsAlive())
		{
			//TRACE("Thread %d alive.\n",iIdx);
			HANDLE thr = tThInfo[iDataSource].clientsConnected[iIdx].GetThreadExitHandle();
			BOOL result = SetEvent(thr);
			TRACE("ClientsKillAll() Alive so SetEvent(%p) returned %d\n", thr, result);
			while (tThInfo[iDataSource].clientsConnected[iIdx].GetIsAlive())
			{
				TRACE("Clients Kill All: waiting for thread shutdown %d...", result);
				Sleep(100);
			}
		}
	} // while
	TRACE("Done ClientsKillAll\n");
}

void CEFMDlg::DebugWindowShow()
{
	if (!m_pDlgDebugger)
	{
		m_pDlgDebugger = new CDlgDebugger;
		m_pDlgDebugger->Create(IDD_DEBUGGER);
		m_pDlgDebugger->ShowWindow(SW_SHOW);
	}
}

void CEFMDlg::DebugWindowKill()
{
	if (m_pDlgDebugger)
	{
		m_pDlgDebugger->ShowWindow(SW_HIDE);
		m_pDlgDebugger->DestroyWindow();
		delete m_pDlgDebugger;
		m_pDlgDebugger = NULL;
	}
}

void CEFMDlg::CoincidenceDetectionStart(int iFieldMillID)
{
	// Start the appropriate hardware
	if (!config.m_txtLACDMode[iFieldMillID].CompareNoCase(txtCOINCIDENCEMODES[1]))
	{
		// StormTracker
		tThInfo[iFieldMillID].m_hCDThreadExit = CreateEvent(NULL, TRUE, FALSE, NULL);
		//		tThInfo[iFieldMillID].m_pCDThread = AfxBeginThread(CD_StormTracker, &tThInfo[iFieldMillID]);
	}
	else if (!config.m_txtLACDMode[iFieldMillID].CompareNoCase(txtCOINCIDENCEMODES[2]))
	{
		// LD-250
	}
	else if (!config.m_txtLACDMode[iFieldMillID].CompareNoCase(txtCOINCIDENCEMODES[3]))
	{
		// LD-350
	}
	else
	{
		// None
	}

}

void CEFMDlg::CoincidenceDetectionStop(int iFieldMillID)
{
	// Shut down the appropriate hardware
	if (!config.m_txtLACDMode[iFieldMillID].CompareNoCase(txtCOINCIDENCEMODES[1]))
	{
		// StormTracker
		tThInfo[iFieldMillID].SetCDThreadExit();

	}
	else if (!config.m_txtLACDMode[iFieldMillID].CompareNoCase(txtCOINCIDENCEMODES[2]))
	{
		// LD-250
	}
	else if (!config.m_txtLACDMode[iFieldMillID].CompareNoCase(txtCOINCIDENCEMODES[3]))
	{
		// LD-350
	}

}

/* Coincidence Detection Hardware Handlers
   -- Commented out because the hardware DLL's are NOT linked into the software.
   -- Don't forget to uncomment the function prototypes in the EFMDlg.h header file.
UINT CEFMDlg::CD_StormTracker(LPVOID pParam)
{
	tTHREADINFO * pInfo = (tTHREADINFO*)pParam;
	DWORD dwRet;
	bool bDone = false;
	StormPCI_tPACKEDDATA dummydata;

	pInfo->m_iCDEventDetectionCounter = -1;

	// Thread is running
	pInfo->SetCDThreadRunning();
	// Initialize to no event detected
	pInfo->ClrCDEventDetected();

	if (!StormPCI_OpenPciCard())
	{
		pInfo->ClrCDThreadRunning();
		bDone = true;
	}

	while (!bDone)
	{
		dwRet = WaitForSingleObject(pInfo->GetCDThreadExit(), 50);
		if (dwRet == WAIT_TIMEOUT)
		{
			// Timeout!
			if (pInfo->m_iCDEventDetectionCounter > 0)	{ pInfo->m_iCDEventDetectionCounter--; }
			if (pInfo->m_iCDEventDetectionCounter == 0)
			{
				pInfo->ClrCDEventDetected();
				pInfo->m_iCDEventDetectionCounter = -1;
			}

			// Look for a new strike
			if (StormPCI_StrikeReady())
			{
				StormPCI_GetBoardCapture(&dummydata);
				StormPCI_RestartBoard();
				pInfo->SetCDEventDetected();
				pInfo->m_iCDEventDetectionCounter = 5;
			}
		}
		else
		{
			// Either we were signaled to exit, or something else went wrong.
			// Shut down the thread.
			bDone = true;
		}
	} // while (!bDone)


	StormPCI_ClosePciCard();

	pInfo->ClrCDThreadRunning();

	return 0;
}

UINT CEFMDlg::CD_LD350(LPVOID pParam)
{
	return 0;
}
*/

CString CEFMDlg::ResolveHostToIP(char* szHostName)
{
	struct hostent* pHostEntry;
	struct in_addr in;
	char szIPAddress[_MAX_PATH];

	if (isalpha(szHostName[0]))
	{
		//getaddrinfo();
		if (!(pHostEntry = gethostbyname(szHostName)))
		{
			/* error */
			return "";
		}
		memcpy(&in.S_un.S_addr, pHostEntry->h_addr_list[0], sizeof(in_addr));
		InetNtop(AF_INET, &in, szIPAddress, _MAX_PATH);
		return szIPAddress;
	}
	else
	{
		return "";
	}
}

CString CEFMDlg::ResolveIPToHost(char* szIP)
{
	struct hostent* pHostEntry;
	char szHostAddress[_MAX_PATH];

	if (!isalpha(szIP[0]))
	{
		unsigned long lIPAddress;
		InetPton(AF_INET, szIP, &lIPAddress);
		if (!(pHostEntry = gethostbyaddr((char*)&lIPAddress, sizeof(unsigned long), AF_INET)))
		{
			/* error */
			return "";
		}
		strcpy_s(szHostAddress, _MAX_PATH, pHostEntry->h_name);
		return szHostAddress;
	}
	else
	{
		return "";
	}
}

bool CEFMDlg::ValidateEFMSentence(CString txtSentence)
{
	// $+00.18,0*CC[0x0D][0x0A]
	// Criteria:
	// [0] $
	// [1] +-
	// [2] 0123456789
	// [3] 0123456789
	// [4] .
	// [5] 0123456789
	// [6] 0123456789
	// [7] ,
	// [8] 012
	// [9] *
	// [A] 0123456789ABCDEF
	// [B] 0123456789ABCDEF
	// [C] [0x0D]
	// [D] [0x0A]

	bool bGoodSentence = false;

	if (txtSentence.GetLength() == 14)
	{
		if (txtSentence[0] == '$')
		{
			if (txtSentence[1] == '+' || txtSentence[1] == '-')
			{
				if (txtSentence[2] >= '0' && txtSentence[2] <= '9')
				{
					if (txtSentence[3] >= '0' && txtSentence[3] <= '9')
					{
						if (txtSentence[4] == '.')
						{
							if (txtSentence[5] >= '0' && txtSentence[5] <= '9')
							{
								if (txtSentence[6] >= '0' && txtSentence[6] <= '9')
								{
									if (txtSentence[7] == ',')
									{
										if (txtSentence[8] >= '0' && txtSentence[8] <= '9')
										{
											if (txtSentence[9] == '*')
											{
												if ((txtSentence[10] >= '0' && txtSentence[10] <= '9') || (txtSentence[10] >= 'A' && txtSentence[10] <= 'F'))
												{
													if ((txtSentence[11] >= '0' && txtSentence[11] <= '9') || (txtSentence[11] >= 'A' && txtSentence[11] <= 'F'))
													{
														if (txtSentence[12] == 0x0D)
														{
															if (txtSentence[13] == 0x0A)
															{
																bGoodSentence = true;
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return bGoodSentence;
}

int CEFMDlg::EFA20_DetectConnectedDevices()
{
	DWORD dwNumDevs = 0;
	FT_DEVICE_LIST_INFO_NODE* devInfo = nullptr;
	FT_STATUS ftStatus;

	// Clear devices list
	vEFA20Devices.clear();

	// Build a device information list
	ftStatus = FT_CreateDeviceInfoList(&dwNumDevs);
	if (ftStatus == FT_OK)
	{
		if (dwNumDevs > 0)
		{
			// Create enough space in memory for the number of connected devices
			devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE) * dwNumDevs);
			if (devInfo)
			{
				// Get the device information list 
				ftStatus = FT_GetDeviceInfoList(devInfo, &dwNumDevs);
				if (ftStatus == FT_OK)
				{
					for (int i = 0; i < static_cast<int>(dwNumDevs); i++)
					{
						if ((((devInfo[i].ID >> 16) & 0x0000FFFF) == EFA20_VID) &&
							((devInfo[i].ID & 0x0000FFFF) == EFA20_PID))
						{
							// This enumerated device is an EFA20
							tEFA20DEVICEINFO efa20;
							efa20.iFlags = devInfo[i].Flags;
							efa20.iType = devInfo[i].Type;
							efa20.iID = devInfo[i].ID;
							efa20.iLocID = devInfo[i].LocId;
							efa20.txtSerialNumber = devInfo[i].SerialNumber;
							efa20.txtDescription = devInfo[i].Description;
							efa20.ftHandle = devInfo[i].ftHandle;
							// Add it to the devices list
							vEFA20Devices.push_back(efa20);
						}
					} // for (int i=0; i<dwNumDevs; i++)
				} // if (ftStatus == FT_OK)

				// Delete the dynamic memory we allocated for the devices
				free(devInfo);
				devInfo = nullptr;

			} // if (devInfo)
		} // if (dwNumDevs > 0)
	} // if (ftStatus == FT_OK)

	return vEFA20Devices.size();
}


// set the CTime nextScheduledAlertTime[] the field mill
void CEFMDlg::SetNextScheduledAlertTime(int iFieldMill)
{
	CTime currentTime = CTime::GetCurrentTime();

	nextScheduledAlertTime[iFieldMill] = CTime(currentTime.GetYear(), currentTime.GetMonth(), currentTime.GetDay(),
		config.iAlertsScheduledHour[iFieldMill],
		config.iAlertsScheduledMins[iFieldMill],
		config.iAlertsScheduledSecs[iFieldMill]);

	// check if the time has passed for today
	if (currentTime >= nextScheduledAlertTime[iFieldMill])
	{
		nextScheduledAlertTime[iFieldMill] += CTimeSpan(1, 0, 0, 0); // schedule for tomorrow
	} // tomorrow

} //SetNextScheduledAlertTime()


void CEFMDlg::SendScheduledAlerts(int iMillIndex)
{
	CString txt;
	bool bActiveAlarms = 0;

	txt = "Daily Status Message. ";

	if (tThInfo[iMillIndex].m_bSignalLost)
	{
		txt += "DATA LOST FAULT. ";
		bActiveAlarms = 1; // don't say "No Active Alarms"
	}
	else
	{
		txt += "Data OK. ";

		if (tThInfo[iMillIndex].m_bRotorFault)
		{
			txt += "ROTOR FAULT. ";
			bActiveAlarms = 1; // don't say "No Active Alarms"
		}

		if (tThInfo[iMillIndex].m_bSelfTestFault)
		{
			txt += "SELF-TEST FAULT. ";
			bActiveAlarms = 1; // don't say "No Active Alarms"
		}

		if (tThInfo[iMillIndex].m_bLALastState)
		{
			txt += "LIGHTNING ALARM ACTIVE. ";
			bActiveAlarms = 1; // don't say "No Active Alarms"
		}

		if (tThInfo[iMillIndex].m_bDALastState)
		{
			txt += "DELTA ALARM ACTIVE. ";
			bActiveAlarms = 1; // don't say "No Active Alarms"
		}

		if (tThInfo[iMillIndex].m_bVHFALastState)
		{
			txt += "VERY HIGH FIELD ALARM ACTIVE. ";
			bActiveAlarms = 1; // don't say "No Active Alarms"
		}
		else if (tThInfo[iMillIndex].m_bHFALastState)
		{
			txt += "VERY HIGH FIELD ALARM ACTIVE. ";
			bActiveAlarms = 1; // don't say "No Active Alarms"
		}

		// NO ALARMS
		if (!bActiveAlarms)
		{
			txt += "No Active Alarms. ";
		}
	} // Data OK

	Alerts.SendAlerts(iMillIndex, txt);
}

//
// The ViewPortChanged event handler. This event occurs if the user scrolls or zooms in or 
// out the chart by dragging or clicking on the chart. It can also be triggered by calling
// CChartViewer.updateViewPort.
//
void CEFMDlg::OnViewPortChanged()
{
	if (m_chartViewer[0].needUpdateChart()) m_chartViewer[0].drawChart();
	if (m_chartViewer[1].needUpdateChart()) m_chartViewer[1].drawChart();
	if (m_chartViewer[2].needUpdateChart()) m_chartViewer[2].drawChart();
	if (m_chartViewer[3].needUpdateChart()) m_chartViewer[3].drawChart();
	if (m_chartLightningDistanceViewer[0].needUpdateChart()) m_chartLightningDistanceViewer[0].drawChart();
	if (m_chartLightningDistanceViewer[1].needUpdateChart()) m_chartLightningDistanceViewer[1].drawChart();
	if (m_chartLightningDistanceViewer[2].needUpdateChart()) m_chartLightningDistanceViewer[2].drawChart();
	if (m_chartLightningDistanceViewer[3].needUpdateChart()) m_chartLightningDistanceViewer[3].drawChart();
}