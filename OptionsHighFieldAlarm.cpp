// OptionsHighFieldAlarm.cpp : implementation file
//

#include "stdafx.h"
#include "EFM.h"
#include "OptionsHighFieldAlarm.h"
#include "EFMOptions.h"
#include "EFMDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CConfiguration config;
extern CEFMOptions * dlgOptions;

/////////////////////////////////////////////////////////////////////////////
// COptionsHighFieldAlarm dialog


COptionsHighFieldAlarm::COptionsHighFieldAlarm(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsHighFieldAlarm::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsHighFieldAlarm)
	m_BEnable = FALSE;
	m_BShowColorLine = FALSE;
	m_dSetpoint = 0.0;
	m_iDelay = 0;
	m_dDuration = 0.0;
	m_BWAV = FALSE;
	m_txtWAVFile = _T("");
	m_dWAVRepeatInterval = 0.0;
	//}}AFX_DATA_INIT

	m_pMainWnd = (CEFMDlg*)AfxGetMainWnd();
}


void COptionsHighFieldAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsHighFieldAlarm)
	DDX_Control(pDX, CMB_HFA_DATASOURCENAMES, m_cmbDataSourceNames);
	DDX_Check(pDX, CHK_ENABLEHFA, m_BEnable);
	DDX_Check(pDX, CHK_ENABLESETPOINTONGRAPH, m_BShowColorLine);
	DDX_Text(pDX, TXT_HFA_SETPOINT, m_dSetpoint);
	DDX_Text(pDX, TXT_HFA_DELAY, m_iDelay);
	DDX_Text(pDX, TXT_HFA_DURATION, m_dDuration);
	DDX_Check(pDX, CHK_HFA_WAV, m_BWAV);
	DDX_Text(pDX, TXT_HFA_WAVFILE, m_txtWAVFile);
	DDX_Text(pDX, TXT_HFA_WAVREPEATINTERVAL, m_dWAVRepeatInterval);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsHighFieldAlarm, CDialog)
	//{{AFX_MSG_MAP(COptionsHighFieldAlarm)
	ON_CBN_SELCHANGE(CMB_HFA_DATASOURCENAMES, OnSelchangeHfaDatasourcenames)
	ON_BN_CLICKED(CHK_ENABLEHFA, OnEnablehfa)
	ON_EN_UPDATE(TXT_HFA_SETPOINT, OnUpdateHfaSetpoint)
	ON_EN_UPDATE(TXT_HFA_DELAY, OnUpdateHfaDelay)
	ON_EN_UPDATE(TXT_HFA_DURATION, OnUpdateHfaDuration)
	ON_BN_CLICKED(CHK_HFA_WAV, OnHfaWav)
	ON_EN_UPDATE(TXT_HFA_WAVFILE, OnUpdateHfaWavfile)
	ON_EN_UPDATE(TXT_HFA_WAVREPEATINTERVAL, OnUpdateHfaWavrepeatinterval)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(BTN_HFA_CHOOSEWAV, OnHfaChoosewav)
	ON_BN_CLICKED(BTN_HFA_PLAYWAV, OnHfaPlaywav)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(CHK_ENABLESETPOINTONGRAPH, &COptionsHighFieldAlarm::OnBnClickedEnablesetpointongraph)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsHighFieldAlarm message handlers

BOOL COptionsHighFieldAlarm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int ctr;

	m_iNumberOfDataSources = config.m_iNumberOfDataSources;

	// Populate the data source box with the data source names
	for (ctr=0; ctr<config.m_iNumberOfDataSources; ctr++)
	{
		m_cmbDataSourceNames.AddString(config.m_szDataSourceName[ctr]);
	}

	// Populate the local variable copies
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		m_lcBEnable[ctr] = config.m_BEnableHFA[ctr];
		m_lcBShowColorLine[ctr] = config.m_BShowColorLineHFA[ctr];
		m_lcdSetpoint[ctr] = config.m_dHFASetpoint[ctr];
		m_lciDelay[ctr] = config.m_iHFADelay[ctr];
		m_lcdDuration[ctr] = config.m_dHFADuration[ctr];
		m_lcBWAV[ctr] = config.m_BHFAWAV[ctr];
		m_lctxtWAVFile[ctr] = config.m_szHFAWAVFile[ctr];
		m_lcdWAVRepeatInterval[ctr] = config.m_dHFAWAVRepeatInterval[ctr];
	}

	// Put the folder bmp on the buttons
	CButton * pButton;
	m_bmpFolder.LoadBitmap(BMP_FOLDER);
	pButton = (CButton* )GetDlgItem(BTN_HFA_CHOOSEWAV);
	pButton->SetBitmap(m_bmpFolder);
	m_bmpPlay.LoadBitmap(BMP_PLAY);
	pButton = (CButton* )GetDlgItem(BTN_HFA_PLAYWAV);
	pButton->SetBitmap(m_bmpPlay);
	m_bmpStop.LoadBitmap(BMP_STOP);

	m_iCurrentSelectedDataSource = 0;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsHighFieldAlarm::OnSelchangeHfaDatasourcenames() 
{
	m_iCurrentSelectedDataSource = m_cmbDataSourceNames.GetCurSel();

	m_BEnable = m_lcBEnable[m_iCurrentSelectedDataSource];
	m_BShowColorLine = m_lcBShowColorLine[m_iCurrentSelectedDataSource];
	m_dSetpoint = m_lcdSetpoint[m_iCurrentSelectedDataSource];
	m_iDelay = m_lciDelay[m_iCurrentSelectedDataSource];
	m_dDuration = m_lcdDuration[m_iCurrentSelectedDataSource];
	m_BWAV = m_lcBWAV[m_iCurrentSelectedDataSource];
	m_txtWAVFile = m_lctxtWAVFile[m_iCurrentSelectedDataSource];
	m_dWAVRepeatInterval = m_lcdWAVRepeatInterval[m_iCurrentSelectedDataSource];

	UpdateData(FALSE);
	EnableControls();
}

void COptionsHighFieldAlarm::EnableControls()
{
	bool bIsChecked = IsDlgButtonChecked(CHK_ENABLEHFA) == BST_CHECKED;
	GetDlgItem(CHK_ENABLESETPOINTONGRAPH)->EnableWindow(bIsChecked ? TRUE : FALSE);
	if (!bIsChecked)
	{
		CheckDlgButton(CHK_ENABLESETPOINTONGRAPH, BST_UNCHECKED);
		m_lcBShowColorLine[m_iCurrentSelectedDataSource] = FALSE;
	}
}

void COptionsHighFieldAlarm::OnEnablehfa() 
{
	UpdateData(TRUE);
	m_lcBEnable[m_iCurrentSelectedDataSource] = m_BEnable;
	m_BShowColorLine = m_BEnable;
	m_lcBShowColorLine[m_iCurrentSelectedDataSource] = m_BShowColorLine;
	UpdateData(FALSE);
	GetDlgItem(CHK_ENABLESETPOINTONGRAPH)->EnableWindow(m_BEnable);
}

void COptionsHighFieldAlarm::OnUpdateHfaSetpoint() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lcdSetpoint[m_iCurrentSelectedDataSource] = m_dSetpoint;
}

void COptionsHighFieldAlarm::OnUpdateHfaDelay() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lciDelay[m_iCurrentSelectedDataSource] = m_iDelay;	
}

void COptionsHighFieldAlarm::OnUpdateHfaDuration() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lcdDuration[m_iCurrentSelectedDataSource] = m_dDuration;	
}

void COptionsHighFieldAlarm::OnHfaWav() 
{
	UpdateData(TRUE);

	m_lcBWAV[m_iCurrentSelectedDataSource] = m_BWAV;
}

void COptionsHighFieldAlarm::OnUpdateHfaWavfile() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lctxtWAVFile[m_iCurrentSelectedDataSource] = m_txtWAVFile;
}

void COptionsHighFieldAlarm::OnUpdateHfaWavrepeatinterval() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lcdWAVRepeatInterval[m_iCurrentSelectedDataSource] = m_dWAVRepeatInterval;
}

BOOL COptionsHighFieldAlarm::PreTranslateMessage(MSG* pMsg) 
{
	if ((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) ||
		(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE))
	{
		return TRUE; // just ignore the enter key
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void COptionsHighFieldAlarm::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	dlgOptions->dlgGeneral.UpdateData(TRUE);

	switch (bShow)
	{
		case 1:	// Being Shown
			// Clear the Data Source Names combo box
			m_cmbDataSourceNames.ResetContent();
			// Repopulate the Data Source Names combo box
			for (size_t ctr = 0; ctr < dlgOptions->dlgGeneral.m_lciNumberOfDataSources; ++ctr)
			{
				m_cmbDataSourceNames.AddString(dlgOptions->dlgGeneral.m_lcszDataSourceName[ctr]);
			}
			// Show the settings for the currently selected item
			//m_iCurrentSelectedDataSource = 0;
			m_cmbDataSourceNames.SelectString(-1, dlgOptions->dlgGeneral.m_lcszDataSourceName[m_iCurrentSelectedDataSource]);

			OnSelchangeHfaDatasourcenames();

			UpdateData(FALSE);
			
			break;
		case 0: // Being Hidden
			break;
	}
	EnableControls();
}

void COptionsHighFieldAlarm::OnHfaChoosewav() 
{
	SelectWAVFile(&m_txtWAVFile);
	m_lctxtWAVFile[m_iCurrentSelectedDataSource] = m_txtWAVFile;
	UpdateData(FALSE);	
}

void COptionsHighFieldAlarm::OnHfaPlaywav() 
{
	if ((m_txtWAVFile.CompareNoCase("None Selected.") != 0) && (m_txtWAVFile.IsEmpty() == FALSE))
	{
		// The Enabling/Disabling of the buttons is not necessary on 
		// the newer, fast machines; but for the slower ones they're 
		// necessary for safety.
		(GetDlgItem(BTN_HFA_PLAYWAV))->EnableWindow(FALSE);
		((CEFMDlg*)m_pMainWnd)->PlayWAV(0, m_txtWAVFile);
		(GetDlgItem(BTN_HFA_PLAYWAV))->EnableWindow(TRUE);
	}
	else
	{
		::MessageBox(NULL,"Please select a WAV file first.","No WAV File Selected",MB_OK|MB_ICONERROR);
	}
}

bool COptionsHighFieldAlarm::SelectWAVFile(CString * wavPath)
{
	int dlgReturn;
	CString strPath;
	CFileDialog sDlg(true);	// Create in "Open" Mode

	sDlg.m_ofn.lpstrTitle = _T("Select WAV File...");
	sDlg.m_ofn.lpstrFilter = _T("WAV File (*.wav)\0*.wav\0");
	sDlg.m_ofn.lpstrDefExt = _T("wav");		// Set the initial default extension

	dlgReturn = sDlg.DoModal();	
	if (dlgReturn == IDOK)
	{
		// OK was pressed
		//strFileName = sDlg.GetFileName();		// Filename + extension
		strPath = sDlg.GetPathName();			// Path + filename + extension
		//strTitle = sDlg.GetFileTitle();			// Filename, no extension
		//strFileExtension = sDlg.GetFileExt();	// Extension, no filename
		
		*wavPath = strPath;		// Set the new wav filename and path
		return true;
	} // End "if (dlgReturn == IDOK)"
	else
	{
		// Cancel was pressed
		return false;	// Cancel, change nothing (heavy russian accent)
	}
}


void COptionsHighFieldAlarm::OnBnClickedEnablesetpointongraph()
{
	UpdateData(TRUE);
	m_lcBShowColorLine[m_iCurrentSelectedDataSource] = m_BShowColorLine;
}
