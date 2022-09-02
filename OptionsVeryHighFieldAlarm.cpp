// OptionsVeryHighFieldAlarm.cpp : implementation file
//

#include "stdafx.h"
#include "EFM.h"
#include "OptionsVeryHighFieldAlarm.h"
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
// COptionsVeryHighFieldAlarm dialog


COptionsVeryHighFieldAlarm::COptionsVeryHighFieldAlarm(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsVeryHighFieldAlarm::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsVeryHighFieldAlarm)
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


void COptionsVeryHighFieldAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsVeryHighFieldAlarm)
	DDX_Control(pDX, CMB_VHFA_DATASOURCENAMES, m_cmbDataSourceNames);
	DDX_Check(pDX, CHK_ENABLEVHFA, m_BEnable);
	DDX_Check(pDX, CHK_ENABLEVHFASETPOINTONGRAPH, m_BShowColorLine);
	DDX_Text(pDX, TXT_VHFA_SETPOINT, m_dSetpoint);
	DDX_Text(pDX, TXT_VHFA_DELAY, m_iDelay);
	DDX_Text(pDX, TXT_VHFA_DURATION, m_dDuration);
	DDX_Check(pDX, CHK_VHFA_WAV, m_BWAV);
	DDX_Text(pDX, TXT_VHFA_WAVFILE, m_txtWAVFile);
	DDX_Text(pDX, TXT_VHFA_WAVREPEATINTERVAL, m_dWAVRepeatInterval);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsVeryHighFieldAlarm, CDialog)
	//{{AFX_MSG_MAP(COptionsVeryHighFieldAlarm)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(CMB_VHFA_DATASOURCENAMES, OnSelchangeVhfaDatasourcenames)
	ON_BN_CLICKED(CHK_ENABLEVHFA, OnEnablevhfa)
	ON_BN_CLICKED(CHK_ENABLEVHFASETPOINTONGRAPH, &COptionsVeryHighFieldAlarm::OnBnClickedEnablevhfasetpointongraph)
	ON_EN_UPDATE(TXT_VHFA_SETPOINT, OnUpdateVhfaSetpoint)
	ON_EN_UPDATE(TXT_VHFA_DELAY, OnUpdateVhfaDelay)
	ON_EN_UPDATE(TXT_VHFA_DURATION, OnUpdateVhfaDuration)
	ON_BN_CLICKED(CHK_VHFA_WAV, OnVhfaWav)
	ON_EN_UPDATE(TXT_VHFA_WAVFILE, OnUpdateVhfaWavfile)
	ON_EN_UPDATE(TXT_VHFA_WAVREPEATINTERVAL, OnUpdateVhfaWavrepeatinterval)
	ON_BN_CLICKED(BTN_VHFA_CHOOSEWAV, OnVhfaChoosewav)
	ON_BN_CLICKED(BTN_VHFA_PLAYWAV, OnVhfaPlaywav)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsVeryHighFieldAlarm message handlers

BOOL COptionsVeryHighFieldAlarm::OnInitDialog() 
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
		m_lcBEnable[ctr] = config.m_BEnableVHFA[ctr];
		m_lcBShowColorLine[ctr] = config.m_BShowColorLineVHFA[ctr];
		m_lcdSetpoint[ctr] = config.m_dVHFASetpoint[ctr];
		m_lciDelay[ctr] = config.m_iVHFADelay[ctr];
		m_lcdDuration[ctr] = config.m_dVHFADuration[ctr];
		m_lcBWAV[ctr] = config.m_BVHFAWAV[ctr];
		m_lctxtWAVFile[ctr] = config.m_szVHFAWAVFile[ctr];
		m_lcdWAVRepeatInterval[ctr] = config.m_dVHFAWAVRepeatInterval[ctr];
	}

	// Put the folder bmp on the buttons
	CButton * pButton;
	m_bmpFolder.LoadBitmap(BMP_FOLDER);
	pButton = (CButton* )GetDlgItem(BTN_VHFA_CHOOSEWAV);
	pButton->SetBitmap(m_bmpFolder);
	m_bmpPlay.LoadBitmap(BMP_PLAY);
	pButton = (CButton* )GetDlgItem(BTN_VHFA_PLAYWAV);
	pButton->SetBitmap(m_bmpPlay);
	m_bmpStop.LoadBitmap(BMP_STOP);
	
	m_iCurrentSelectedDataSource = 0;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsVeryHighFieldAlarm::EnableControls()
{

	bool bIsChecked = IsDlgButtonChecked(CHK_ENABLEVHFA) == BST_CHECKED;
	GetDlgItem(CHK_ENABLEVHFASETPOINTONGRAPH)->EnableWindow(bIsChecked ? TRUE : FALSE);
	if (!bIsChecked)
	{
		CheckDlgButton(CHK_ENABLEVHFASETPOINTONGRAPH, BST_UNCHECKED);
		m_lcBShowColorLine[m_iCurrentSelectedDataSource] = FALSE;
	}

}

void COptionsVeryHighFieldAlarm::OnShowWindow(BOOL bShow, UINT nStatus) 
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

			OnSelchangeVhfaDatasourcenames();

			UpdateData(FALSE);
			
			break;
		case 0: // Being Hidden
			break;
	}
	EnableControls();
}

BOOL COptionsVeryHighFieldAlarm::PreTranslateMessage(MSG* pMsg) 
{
	if ((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) ||
		(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE))
	{
		return TRUE; // just ignore the enter key
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void COptionsVeryHighFieldAlarm::OnSelchangeVhfaDatasourcenames() 
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

void COptionsVeryHighFieldAlarm::OnEnablevhfa() 
{
	UpdateData(TRUE);
	m_lcBEnable[m_iCurrentSelectedDataSource] = m_BEnable;
	m_BShowColorLine = m_BEnable;
	m_lcBShowColorLine[m_iCurrentSelectedDataSource] = m_BShowColorLine;
	UpdateData(FALSE);
	GetDlgItem(CHK_ENABLEVHFASETPOINTONGRAPH)->EnableWindow(m_BEnable);
}

void COptionsVeryHighFieldAlarm::OnUpdateVhfaSetpoint() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lcdSetpoint[m_iCurrentSelectedDataSource] = m_dSetpoint;	
}

void COptionsVeryHighFieldAlarm::OnUpdateVhfaDelay() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lciDelay[m_iCurrentSelectedDataSource] = m_iDelay;	
}

void COptionsVeryHighFieldAlarm::OnUpdateVhfaDuration() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lcdDuration[m_iCurrentSelectedDataSource] = m_dDuration;	
}

void COptionsVeryHighFieldAlarm::OnVhfaWav() 
{
	UpdateData(TRUE);

	m_lcBWAV[m_iCurrentSelectedDataSource] = m_BWAV;
}

void COptionsVeryHighFieldAlarm::OnUpdateVhfaWavfile() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lctxtWAVFile[m_iCurrentSelectedDataSource] = m_txtWAVFile;	
}

void COptionsVeryHighFieldAlarm::OnUpdateVhfaWavrepeatinterval() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lcdWAVRepeatInterval[m_iCurrentSelectedDataSource] = m_dWAVRepeatInterval;	
}

void COptionsVeryHighFieldAlarm::OnVhfaChoosewav() 
{
	SelectWAVFile(&m_txtWAVFile);
	m_lctxtWAVFile[m_iCurrentSelectedDataSource] = m_txtWAVFile;
	UpdateData(FALSE);
}

void COptionsVeryHighFieldAlarm::OnVhfaPlaywav() 
{
	if ((m_txtWAVFile.CompareNoCase("None Selected.") != 0) && (m_txtWAVFile.IsEmpty() == FALSE))
	{
		// The Enabling/Disabling of the buttons is not necessary on 
		// the newer, fast machines; but for the slower ones they're 
		// necessary for safety.
		(GetDlgItem(BTN_VHFA_PLAYWAV))->EnableWindow(FALSE);
		((CEFMDlg*)m_pMainWnd)->PlayWAV(0, m_txtWAVFile);
		(GetDlgItem(BTN_VHFA_PLAYWAV))->EnableWindow(TRUE);
	}
	else
	{
		::MessageBox(NULL,"Please select a WAV file first.","No WAV File Selected",MB_OK|MB_ICONERROR);
	}	
}

bool COptionsVeryHighFieldAlarm::SelectWAVFile(CString * wavPath)
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


void COptionsVeryHighFieldAlarm::OnBnClickedEnablevhfasetpointongraph()
{
	UpdateData(TRUE);
	m_lcBShowColorLine[m_iCurrentSelectedDataSource] = m_BShowColorLine;
}
