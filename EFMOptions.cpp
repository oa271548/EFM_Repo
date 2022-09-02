// EFMOptions.cpp : implementation file
//

#include "stdafx.h"
#include "EFM.h"
#include "EFMOptions.h"
#include "Configuration.h"
#include "EFMDlg.h"
#include "WaitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CConfiguration config;
extern CEFMOptions* dlgOptions;

// Option Menu Index Definitions
#define	MENU_GENERAL			0
#define	MENU_HIGHFIELDALARM		1
#define	MENU_VERYHIGHFIELDALARM	2
#define MENU_LIGHTNINGALARM		3
#define MENU_DELTAALARM			4
#define	MENU_DATALOGGING		5
#define	MENU_DATASHARING		6
#define MENU_ALERTS				7
#define	MENU_IMAGECAPTURE		8


/////////////////////////////////////////////////////////////////////////////
// CEFMOptions dialog


CEFMOptions::CEFMOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CEFMOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEFMOptions)
	//}}AFX_DATA_INIT

	// Get pointer to main app so we can communicate
	m_pMainWnd = (CEFMDlg*)AfxGetMainWnd();
}


void CEFMOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEFMOptions)
	DDX_Control(pDX, TAB_GENERAL, m_tabGeneral);
	DDX_Control(pDX, LST_OPTIONS, m_lstOptions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEFMOptions, CDialog)
	//{{AFX_MSG_MAP(CEFMOptions)
	ON_NOTIFY(NM_CLICK, LST_OPTIONS, OnClickOptions)
	ON_BN_CLICKED(BTN_CANCEL, OnCancel)
	ON_BN_CLICKED(BTN_ACCEPT, OnAccept)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEFMOptions message handlers

void CEFMOptions::OnCancel()
{
	// TODO: Add your control notification handler code here
	dlgGeneral.DestroyWindow();
	EndDialog(1);	// Exit signaling the Cancel button was pressed
}

BOOL CEFMOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	// Create the Image list and associate it with the CListCtrl
	m_ilMenu.Create(BMP_OPTIONMENU, 32, 1, RGB(255, 255, 255));
	m_lstOptions.SetImageList(&m_ilMenu, TVSIL_NORMAL);
	// Set up the menu icons
	m_lstOptions.SetIconSpacing(0, 5);
	m_lstOptions.InsertItem(MENU_GENERAL, "General", MENU_GENERAL);
	m_lstOptions.InsertItem(MENU_HIGHFIELDALARM, "High", MENU_HIGHFIELDALARM);
	m_lstOptions.InsertItem(MENU_VERYHIGHFIELDALARM, "Very High", MENU_VERYHIGHFIELDALARM);
	m_lstOptions.InsertItem(MENU_LIGHTNINGALARM, "Lightning", MENU_LIGHTNINGALARM);
	m_lstOptions.InsertItem(MENU_DELTAALARM, "Delta", MENU_DELTAALARM);
	m_lstOptions.InsertItem(MENU_DATALOGGING, "Data Logging", MENU_DATALOGGING);
	//m_lstOptions.InsertItem(MENU_IMAGECAPTURE,"Image Capture",MENU_IMAGECAPTURE);
	m_lstOptions.InsertItem(MENU_DATASHARING, "Data Sharing", MENU_DATASHARING);
	m_lstOptions.InsertItem(MENU_ALERTS, "Alerts", MENU_ALERTS);


	CTabCtrl* tabGeneral = (CTabCtrl*)GetDlgItem(TAB_GENERAL);
	tabGeneral->GetClientRect(&m_rectOptionSubWindow);

	dlgGeneral.Create(IDD_OPTIONS_GENERAL, tabGeneral);
	dlgHighFieldAlarm.Create(IDD_OPTIONS_HIGHFIELDALARM, tabGeneral);
	dlgVeryHighFieldAlarm.Create(IDD_OPTIONS_VERYHIGHFIELDALARM, tabGeneral);
	dlgLightningAlarm.Create(IDD_OPTIONS_LIGHTNINGALARM, tabGeneral);
	dlgDeltaAlarm.Create(IDD_OPTIONS_DELTAALARM, tabGeneral);
	dlgDataLogging.Create(IDD_OPTIONS_DATALOGGING, tabGeneral);
	dlgImageCapture.Create(IDD_OPTIONS_IMAGECAPTURE, tabGeneral);
	dlgDataSharing.Create(IDD_OPTIONS_DATASHARING, tabGeneral);
	dlgAlerts.Create(IDD_OPTIONS_ALERTS, tabGeneral);

	dlgGeneral.MoveWindow(m_rectOptionSubWindow.left, m_rectOptionSubWindow.top, m_rectOptionSubWindow.Width(), m_rectOptionSubWindow.Height());
	dlgHighFieldAlarm.MoveWindow(m_rectOptionSubWindow.left, m_rectOptionSubWindow.top, m_rectOptionSubWindow.Width(), m_rectOptionSubWindow.Height());
	dlgVeryHighFieldAlarm.MoveWindow(m_rectOptionSubWindow.left, m_rectOptionSubWindow.top, m_rectOptionSubWindow.Width(), m_rectOptionSubWindow.Height());
	dlgLightningAlarm.MoveWindow(m_rectOptionSubWindow.left, m_rectOptionSubWindow.top, m_rectOptionSubWindow.Width(), m_rectOptionSubWindow.Height());
	dlgDeltaAlarm.MoveWindow(m_rectOptionSubWindow.left, m_rectOptionSubWindow.top, m_rectOptionSubWindow.Width(), m_rectOptionSubWindow.Height());
	dlgDataLogging.MoveWindow(m_rectOptionSubWindow.left, m_rectOptionSubWindow.top, m_rectOptionSubWindow.Width(), m_rectOptionSubWindow.Height());
	//dlgImageCapture.MoveWindow(m_rectOptionSubWindow.left,m_rectOptionSubWindow.top,m_rectOptionSubWindow.Width(),m_rectOptionSubWindow.Height());
	dlgDataSharing.MoveWindow(m_rectOptionSubWindow.left, m_rectOptionSubWindow.top, m_rectOptionSubWindow.Width(), m_rectOptionSubWindow.Height());
	// Width 425, Height 291
	dlgAlerts.MoveWindow(m_rectOptionSubWindow.left, m_rectOptionSubWindow.top, m_rectOptionSubWindow.Width(), m_rectOptionSubWindow.Height());

	dlgOptions = this;	// Save a pointer to the options dialog

	dlgAlerts.ShowWindow(FALSE);
	dlgImageCapture.ShowWindow(FALSE);
	dlgDataSharing.ShowWindow(FALSE);
	dlgDataLogging.ShowWindow(FALSE);
	dlgHighFieldAlarm.ShowWindow(FALSE);
	dlgVeryHighFieldAlarm.ShowWindow(FALSE);
	dlgLightningAlarm.ShowWindow(FALSE);
	dlgDeltaAlarm.ShowWindow(FALSE);
	dlgGeneral.ShowWindow(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CEFMOptions::OnClickOptions(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

	int iOptionMenuItem;

	// Get the clicked item index
	iOptionMenuItem = m_lstOptions.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	switch (iOptionMenuItem)
	{
	case MENU_GENERAL:
		dlgGeneral.ShowWindow(TRUE);
		dlgHighFieldAlarm.ShowWindow(FALSE);
		dlgVeryHighFieldAlarm.ShowWindow(FALSE);
		dlgLightningAlarm.ShowWindow(FALSE);
		dlgDeltaAlarm.ShowWindow(FALSE);
		dlgDataLogging.ShowWindow(FALSE);
		dlgDataSharing.ShowWindow(FALSE);
		dlgImageCapture.ShowWindow(FALSE);
		dlgAlerts.ShowWindow(FALSE);
		break;
	case MENU_HIGHFIELDALARM:
		dlgGeneral.ShowWindow(FALSE);
		dlgHighFieldAlarm.ShowWindow(TRUE);
		dlgVeryHighFieldAlarm.ShowWindow(FALSE);
		dlgLightningAlarm.ShowWindow(FALSE);
		dlgDeltaAlarm.ShowWindow(FALSE);
		dlgDataLogging.ShowWindow(FALSE);
		dlgDataSharing.ShowWindow(FALSE);
		dlgImageCapture.ShowWindow(FALSE);
		dlgAlerts.ShowWindow(FALSE);
		break;
	case MENU_VERYHIGHFIELDALARM:
		dlgGeneral.ShowWindow(FALSE);
		dlgHighFieldAlarm.ShowWindow(FALSE);
		dlgVeryHighFieldAlarm.ShowWindow(TRUE);
		dlgLightningAlarm.ShowWindow(FALSE);
		dlgDeltaAlarm.ShowWindow(FALSE);
		dlgDataLogging.ShowWindow(FALSE);
		dlgDataSharing.ShowWindow(FALSE);
		dlgImageCapture.ShowWindow(FALSE);
		dlgAlerts.ShowWindow(FALSE);
		break;
	case MENU_LIGHTNINGALARM:
		dlgGeneral.ShowWindow(FALSE);
		dlgHighFieldAlarm.ShowWindow(FALSE);
		dlgVeryHighFieldAlarm.ShowWindow(FALSE);
		dlgLightningAlarm.ShowWindow(TRUE);
		dlgDeltaAlarm.ShowWindow(FALSE);
		dlgDataLogging.ShowWindow(FALSE);
		dlgDataSharing.ShowWindow(FALSE);
		dlgImageCapture.ShowWindow(FALSE);
		dlgAlerts.ShowWindow(FALSE);
		break;
	case MENU_DELTAALARM:
		dlgGeneral.ShowWindow(FALSE);
		dlgHighFieldAlarm.ShowWindow(FALSE);
		dlgVeryHighFieldAlarm.ShowWindow(FALSE);
		dlgLightningAlarm.ShowWindow(FALSE);
		dlgDeltaAlarm.ShowWindow(TRUE);
		dlgDataLogging.ShowWindow(FALSE);
		dlgDataSharing.ShowWindow(FALSE);
		dlgImageCapture.ShowWindow(FALSE);
		dlgAlerts.ShowWindow(FALSE);
		break;
	case MENU_DATALOGGING:
		dlgGeneral.ShowWindow(FALSE);
		dlgHighFieldAlarm.ShowWindow(FALSE);
		dlgVeryHighFieldAlarm.ShowWindow(FALSE);
		dlgLightningAlarm.ShowWindow(FALSE);
		dlgDeltaAlarm.ShowWindow(FALSE);
		dlgDataLogging.ShowWindow(TRUE);
		dlgDataSharing.ShowWindow(FALSE);
		dlgImageCapture.ShowWindow(FALSE);
		dlgAlerts.ShowWindow(FALSE);
		break;
	case MENU_DATASHARING:
		dlgGeneral.ShowWindow(FALSE);
		dlgHighFieldAlarm.ShowWindow(FALSE);
		dlgVeryHighFieldAlarm.ShowWindow(FALSE);
		dlgLightningAlarm.ShowWindow(FALSE);
		dlgDeltaAlarm.ShowWindow(FALSE);
		dlgDataLogging.ShowWindow(FALSE);
		dlgDataSharing.ShowWindow(TRUE);
		dlgImageCapture.ShowWindow(FALSE);
		dlgAlerts.ShowWindow(FALSE);
		break;
	case MENU_ALERTS:
		dlgGeneral.ShowWindow(FALSE);
		dlgHighFieldAlarm.ShowWindow(FALSE);
		dlgVeryHighFieldAlarm.ShowWindow(FALSE);
		dlgLightningAlarm.ShowWindow(FALSE);
		dlgDeltaAlarm.ShowWindow(FALSE);
		dlgDataLogging.ShowWindow(FALSE);
		dlgDataSharing.ShowWindow(FALSE);
		dlgImageCapture.ShowWindow(FALSE);
		dlgAlerts.ShowWindow(TRUE);
		break;
	case MENU_IMAGECAPTURE:
		//	dlgGeneral.ShowWindow(FALSE);
		//	dlgHighFieldAlarm.ShowWindow(FALSE);
		//	dlgVeryHighFieldAlarm.ShowWindow(FALSE);
		//	dlgLightningAlarm.ShowWindow(FALSE);
		//	dlgDeltaAlarm.ShowWindow(FALSE);
		//	dlgDataLogging.ShowWindow(FALSE);
		//	dlgDataSharing.ShowWindow(FALSE);
		//	dlgImageCapture.ShowWindow(TRUE);
		break;
	default:
		// Gets executed if user clicks on whitespace, so just ignore.
		break;
	}

	*pResult = 0;
}

void CEFMOptions::OnAccept()
{
	// TODO: Add your control notification handler code here
	int ctr, iFieldMillID;
	CString txt;

	dlgGeneral.UpdateData(TRUE);
	dlgHighFieldAlarm.UpdateData(TRUE);
	dlgVeryHighFieldAlarm.UpdateData(TRUE);
	dlgLightningAlarm.UpdateData(TRUE);
	dlgDeltaAlarm.UpdateData(TRUE);
	dlgDataLogging.UpdateData(TRUE);
	dlgDataSharing.UpdateData(TRUE);
	dlgAlerts.UpdateData(TRUE);
	dlgAlerts.SaveRecipientListboxes();

	// Shut off all alarms
	for (iFieldMillID = 0; iFieldMillID < MAX_NUMOFDATASOURCES; iFieldMillID++)
	{
		((CEFMDlg*)m_pMainWnd)->m_bRotorFaultStatusCheck[iFieldMillID] = false;
		((CEFMDlg*)m_pMainWnd)->m_bSelfTestFaultStatusCheck[iFieldMillID] = false;
		((CEFMDlg*)m_pMainWnd)->m_bSignalLostFaultStatusCheck[iFieldMillID] = false;
		((CEFMDlg*)m_pMainWnd)->m_bDeltaAlarmCheck[iFieldMillID] = false;
		((CEFMDlg*)m_pMainWnd)->m_bLightningAlarmCheck[iFieldMillID] = false;
		((CEFMDlg*)m_pMainWnd)->m_bVeryHighFieldAlarmCheck[iFieldMillID] = false;
		((CEFMDlg*)m_pMainWnd)->m_bHighFieldAlarmCheck[iFieldMillID] = false;
		((CEFMDlg*)m_pMainWnd)->DeactivateHighFieldAlarm(iFieldMillID, ALARM_ENNUN | ALARM_VISUAL | ALARM_AUDIO);
		((CEFMDlg*)m_pMainWnd)->DeactivateVeryHighFieldAlarm(iFieldMillID, ALARM_ENNUN | ALARM_VISUAL | ALARM_AUDIO);
		((CEFMDlg*)m_pMainWnd)->DeactivateLightningAlarm(iFieldMillID, ALARM_VISUAL | ALARM_AUDIO);
		((CEFMDlg*)m_pMainWnd)->DeactivateDeltaAlarm(iFieldMillID, ALARM_VISUAL | ALARM_AUDIO);

	}

	// Make a copy of the original enables
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		old_SourceEnabled[ctr] = config.m_BDataSourceEnabled[ctr];
		old_SourcePoint[ctr] = config.m_szDataSourcePoint[ctr];
		old_SourceComPort[ctr] = config.m_szComPort[ctr];
		old_SourceNetAddress[ctr] = config.m_txtNetworkAddress[ctr];
		old_SourceNetPort[ctr] = config.m_iNetworkPort[ctr];
		old_EFA20SerialNumber[ctr] = config.m_txtEFA20SerialNumber[ctr];
		old_SourceSharingEnabled[ctr] = config.m_BDataSharingEnabled[ctr];
		old_SourceSharingPort[ctr] = config.m_iDataSharingPort[ctr];
		old_DataLogEnabled[ctr] = config.m_BEnableDataLogging[ctr];
		old_HighResolutionLogging[ctr] = config.m_BHighResolutionLogging[ctr];
	}

	//
	// Save everything back to the configuration object
	//	
	// [General]
	config.m_iNumberOfDataSources = dlgGeneral.m_lciNumberOfDataSources;
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		config.m_szDataSourceName[ctr] = dlgGeneral.m_lcszDataSourceName[ctr];
	}
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		config.m_BDataSourceEnabled[ctr] = dlgGeneral.m_lcBDataSourceEnabled[ctr];
	}
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		config.m_szDataSourcePoint[ctr] = dlgGeneral.m_lcszDataSourcePoint[ctr];
	}
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		config.m_szComPort[ctr] = dlgGeneral.m_lcszComPort[ctr];
	}
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		config.m_iBaudRate[ctr] = dlgGeneral.m_lciBaudRate[ctr];
	}
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		config.m_txtNetworkAddress[ctr] = dlgGeneral.m_lctxtNetworkAddress[ctr];
	}
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		config.m_iNetworkPort[ctr] = dlgGeneral.m_lciNetworkPort[ctr];
	}
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		config.m_txtEFA20SerialNumber[ctr] = dlgGeneral.m_lcszEFA20SerialNumber[ctr];
	}
	// [HFA]
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		config.m_BEnableHFA[ctr] = dlgHighFieldAlarm.m_lcBEnable[ctr];
		config.m_BShowColorLineHFA[ctr] = dlgHighFieldAlarm.m_lcBShowColorLine[ctr];
		config.m_dHFASetpoint[ctr] = dlgHighFieldAlarm.m_lcdSetpoint[ctr];
		config.m_iHFADelay[ctr] = dlgHighFieldAlarm.m_lciDelay[ctr];
		config.m_dHFADuration[ctr] = dlgHighFieldAlarm.m_lcdDuration[ctr];
		config.m_BHFAWAV[ctr] = dlgHighFieldAlarm.m_lcBWAV[ctr];
		config.m_szHFAWAVFile[ctr] = dlgHighFieldAlarm.m_lctxtWAVFile[ctr];
		config.m_dHFAWAVRepeatInterval[ctr] = dlgHighFieldAlarm.m_lcdWAVRepeatInterval[ctr];
	}
	// [VHFA]
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		config.m_BEnableVHFA[ctr] = dlgVeryHighFieldAlarm.m_lcBEnable[ctr];
		config.m_BShowColorLineVHFA[ctr] = dlgVeryHighFieldAlarm.m_lcBShowColorLine[ctr];
		config.m_dVHFASetpoint[ctr] = dlgVeryHighFieldAlarm.m_lcdSetpoint[ctr];
		config.m_iVHFADelay[ctr] = dlgVeryHighFieldAlarm.m_lciDelay[ctr];
		config.m_dVHFADuration[ctr] = dlgVeryHighFieldAlarm.m_lcdDuration[ctr];
		config.m_BVHFAWAV[ctr] = dlgVeryHighFieldAlarm.m_lcBWAV[ctr];
		config.m_szVHFAWAVFile[ctr] = dlgVeryHighFieldAlarm.m_lctxtWAVFile[ctr];
		config.m_dVHFAWAVRepeatInterval[ctr] = dlgVeryHighFieldAlarm.m_lcdWAVRepeatInterval[ctr];
	}
	// [DA]
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		config.m_BEnableDA[ctr] = dlgDeltaAlarm.m_lcBEnable[ctr];
		config.m_dDASetpoint[ctr] = dlgDeltaAlarm.m_lcdSetpoint[ctr];
		config.m_iDAInterval[ctr] = dlgDeltaAlarm.m_lciInterval[ctr];
		config.m_iDADelay[ctr] = dlgDeltaAlarm.m_lciDelay[ctr];
		config.m_dDADuration[ctr] = dlgDeltaAlarm.m_lcdDuration[ctr];
		config.m_BDAWAV[ctr] = dlgDeltaAlarm.m_lcBWAV[ctr];
		config.m_szDAWAVFile[ctr] = dlgDeltaAlarm.m_lctxtWAVFile[ctr];
		config.m_dDAWAVRepeatInterval[ctr] = dlgDeltaAlarm.m_lcdWAVRepeatInterval[ctr];
	}
	// [LA]
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		config.m_BEnableLA[ctr] = dlgLightningAlarm.m_lcBEnable[ctr];
		config.m_BShowChartLightningDistance[ctr] = dlgLightningAlarm.m_lcBShowChartLightningDistance[ctr];
		config.m_dLADuration[ctr] = dlgLightningAlarm.m_lcdDuration[ctr];
		config.m_dLAWAVRepeatInterval[ctr] = dlgLightningAlarm.m_lcdWAVRepeatInterval[ctr];
		config.m_BLAOnlyAnnounceCloser[ctr] = dlgLightningAlarm.m_lcBOnlyAnnounceCloser[ctr];
		config.m_BLAOnActivationEnable[ctr] = dlgLightningAlarm.m_lcBOnActivationWAV[ctr];
		config.m_BLADistanceEnable[ctr] = dlgLightningAlarm.m_lcBDistanceWAV[ctr];
		config.m_BLARepeatEnable[ctr] = dlgLightningAlarm.m_lcBRepeatWAV[ctr];
		config.m_BLAAllClearEnable[ctr] = dlgLightningAlarm.m_lcBAllClearWAV[ctr];
		config.m_txtLAOnActivationWAVFile[ctr] = dlgLightningAlarm.m_lctxtOnActivationWAVFile[ctr];
		config.m_txtLADistanceWAVFiles[ctr] = dlgLightningAlarm.m_lctxtDistanceWAVFiles[ctr];
		config.m_txtLARepeatWAVFile[ctr] = dlgLightningAlarm.m_lctxtRepeatWAVFile[ctr];
		config.m_txtLAAllClearWAVFile[ctr] = dlgLightningAlarm.m_lctxtAllClearWAVFile[ctr];
		config.m_szLARingRanges[ctr] = dlgLightningAlarm.m_lctxtRangeRings[ctr];
		config.m_iLAUnit[ctr] = dlgLightningAlarm.m_lciRingRangeUnit[ctr];
		config.m_txtLACDMode[ctr] = dlgLightningAlarm.m_lctxtLACDMode[ctr];
		config.m_txtLACDComPort[ctr] = dlgLightningAlarm.m_lctxtLACDComPort[ctr];
	}

	// [Data Logging]
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		config.m_BEnableDataLogging[ctr] = dlgDataLogging.m_laBEnableDataLogging[ctr];
		config.m_szDataLoggingPath[ctr] = dlgDataLogging.m_laszDataLoggingPath[ctr];
		config.m_BHighResolutionLogging[ctr] = dlgDataLogging.m_laBHighResolutionLogging[ctr];
	}
	// [Data Sharing]
	// Make sure we auto-disable any source that isn't being used.
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		if (ctr < config.m_iNumberOfDataSources)
		{
			config.m_BDataSharingEnabled[ctr] = dlgDataSharing.m_lcBDataSharingEnabled[ctr];
			config.m_iDataSharingPort[ctr] = dlgDataSharing.m_lciDataSharingPort[ctr];
		}
		else
		{
			config.m_BDataSharingEnabled[ctr] = FALSE;
			config.m_iDataSharingPort[ctr] = dlgDataSharing.m_lciDataSharingPort[ctr];
		}
	}

	// Alerts
	//
	dlgAlerts.SaveToActiveConfig(); // copy current dialog settings to config

	// Save the configuration
	config.SaveConfiguration();

	// Sets up and shows the splash screen
	CWaitDlg* m_pWaitDlg;
	m_pWaitDlg = new CWaitDlg;
	m_pWaitDlg->Create(IDD_WAITDLG);
	m_pWaitDlg->ShowSplashScreen("Please Wait...", "Synchronizing Settings");

	// Shut everything down Synchronize settings with program
	//

	((CEFMDlg*)m_pMainWnd)->MakeChartsReflectKnobs();

	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{

		((CEFMDlg*)m_pMainWnd)->SetNextScheduledAlertTime(ctr); // in case we changged the scheduled status messages

		if (HasStuffChanged(ctr))
		{
			if (old_SourceEnabled[ctr])
			{
				((CEFMDlg*)m_pMainWnd)->StopDAQThread(ctr, old_SourcePoint[ctr]);	//config.m_szDataSourcePoint[ctr]);
			}
			if (config.m_BDataSourceEnabled[ctr])
			{
				((CEFMDlg*)m_pMainWnd)->StartDAQThread(ctr, config.m_szDataSourcePoint[ctr]);
			}
			//((CEFMDlg*)m_pMainWnd)->m_laLightningAlarm[ctr].
		}

		// Set the default chart title
		//((CEFMDlg*)m_pMainWnd)->m_stripChart[ctr].SetCaption(config.m_szDataSourceName[ctr]);
		((CEFMDlg*)m_pMainWnd)->m_chartViewer[ctr].setCaption(config.m_szDataSourceName[ctr]);

	} // End "for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)"



	((CEFMDlg*)m_pMainWnd)->LayoutControls();

	// Since Data Source 0 may not always be active (Source 3 could be the only 
	// one active), we need to find the first ENABLED data source.  Once we know 
	// the first, call it's function so it's the default selected.  If multiple 
	// sources are enabled, it'll always go with the lowest index.
	// This is sloppy and can be looked at to be fixed for next release.
	// Finds the first active data source
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		if (config.m_BDataSourceEnabled[ctr] == TRUE)
		{
			break;
		}
	}
	// Calls the appropriate function.  Since these functions are protected 
	// by another class, we need accessor functions.
	switch (ctr)
	{
	case 0:
		((CEFMDlg*)m_pMainWnd)->accessor_OnClickFieldmill1();
		break;
	case 1:
		((CEFMDlg*)m_pMainWnd)->accessor_OnClickFieldmill2();
		break;
	case 2:
		((CEFMDlg*)m_pMainWnd)->accessor_OnClickFieldmill3();
		break;
	case 3:
		((CEFMDlg*)m_pMainWnd)->accessor_OnClickFieldmill4();
		break;
	}

	// Reconfigure the ring ranges
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		double dRings[32];		// We'll never have more than 32 rings
		CString txtRings(config.m_szLARingRanges[ctr]);
		int iRingCount = 0;
		char* tok;
		char* next_token = nullptr;
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
		((CEFMDlg*)m_pMainWnd)->m_laLightningAlarm[ctr].SetDistanceRings(dRings, iRingCount);
		if (config.m_iLAUnit[ctr] == MILES)
		{
			((CEFMDlg*)m_pMainWnd)->m_laLightningAlarm[ctr].SetUnitText(MILES_TXT);
		}
		else if (config.m_iLAUnit[ctr] == KILOMETERS)
		{
			((CEFMDlg*)m_pMainWnd)->m_laLightningAlarm[ctr].SetUnitText(KILOMETERS_TXT);
		}
		else
		{
			((CEFMDlg*)m_pMainWnd)->m_laLightningAlarm[ctr].SetUnitText("");
		}
	}

	// Destroy and close the spash screen
	m_pWaitDlg->DestroyWindow();
	delete m_pWaitDlg;

	EndDialog(0);	// Exit signaling the Accept button was pressed


}

bool CEFMOptions::StartServerThread(int idx)
{
	((CEFMDlg*)m_pMainWnd)->StartClientServer(idx);
	return true;

}

bool CEFMOptions::StopServerThread(int idx)
{
	((CEFMDlg*)m_pMainWnd)->StopClientServer(idx);
	return true;
}

bool CEFMOptions::HasStuffChanged(int iFMID)
{
	if (old_SourceEnabled[iFMID] != config.m_BDataSourceEnabled[iFMID])
	{
		return true;
	}

	if (old_SourcePoint[iFMID] != config.m_szDataSourcePoint[iFMID])
	{
		return true;
	}

	if (old_SourceComPort[iFMID] != config.m_szComPort[iFMID])
	{
		return true;
	}

	if (old_SourceNetAddress[iFMID] != config.m_txtNetworkAddress[iFMID])
	{
		return true;
	}

	if (old_SourceNetPort[iFMID] != config.m_iNetworkPort[iFMID])
	{
		return true;
	}

	if (old_SourceSharingEnabled[iFMID] != config.m_BDataSharingEnabled[iFMID])
	{
		return true;
	}

	if (old_SourceSharingPort[iFMID] != config.m_iDataSharingPort[iFMID])
	{
		return true;
	}

	if (old_EFA20SerialNumber[iFMID] != config.m_txtEFA20SerialNumber[iFMID])
	{
		return true;
	}

	return false;
}
