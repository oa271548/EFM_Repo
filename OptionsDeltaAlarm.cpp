// OptionsDeltaAlarm.cpp : implementation file
//

#include "stdafx.h"
#include "EFM.h"
#include "OptionsDeltaAlarm.h"
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
// COptionsDeltaAlarm dialog


COptionsDeltaAlarm::COptionsDeltaAlarm(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDeltaAlarm::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDeltaAlarm)
	m_BEnable = FALSE;
	m_dSetpoint = 0.0;
	m_iInterval = 0;
	m_dDuration = 0.0;
	m_BWAV = FALSE;
	m_txtWAVFile = _T("");
	m_dWAVRepeatInterval = 0.0;
	m_iDelay = 0;
	//}}AFX_DATA_INIT

	m_pMainWnd = (CEFMDlg*)AfxGetMainWnd();
}


void COptionsDeltaAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDeltaAlarm)
	DDX_Control(pDX, CMB_DA_DATASOURCENAMES, m_cmbDataSourceNames);
	DDX_Check(pDX, CHK_ENABLEDA, m_BEnable);
	DDX_Text(pDX, TXT_DA_SETPOINT, m_dSetpoint);
	DDX_Text(pDX, TXT_DA_INTERVAL, m_iInterval);
	DDX_Text(pDX, TXT_DA_DURATION, m_dDuration);
	DDX_Check(pDX, CHK_DA_WAV, m_BWAV);
	DDX_Text(pDX, TXT_DA_WAVFILE, m_txtWAVFile);
	DDX_Text(pDX, TXT_DA_WAVREPEATINTERVAL, m_dWAVRepeatInterval);
	DDX_Text(pDX, TXT_DA_DELAY, m_iDelay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDeltaAlarm, CDialog)
	//{{AFX_MSG_MAP(COptionsDeltaAlarm)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(CMB_DA_DATASOURCENAMES, OnSelchangeDaDatasourcenames)
	ON_BN_CLICKED(CHK_ENABLEDA, OnEnableda)
	ON_EN_UPDATE(TXT_DA_SETPOINT, OnUpdateDaSetpoint)
	ON_EN_UPDATE(TXT_DA_INTERVAL, OnUpdateDaInterval)
	ON_EN_UPDATE(TXT_DA_DURATION, OnUpdateDaDuration)
	ON_BN_CLICKED(CHK_DA_WAV, OnDaWav)
	ON_EN_UPDATE(TXT_DA_WAVFILE, OnUpdateDaWavfile)
	ON_EN_UPDATE(TXT_DA_WAVREPEATINTERVAL, OnUpdateDaWavrepeatinterval)
	ON_BN_CLICKED(BTN_DA_CHOOSEWAV, OnDaChoosewav)
	ON_BN_CLICKED(BTN_DA_PLAYWAV, OnDaPlaywav)
	ON_WM_CANCELMODE()
	ON_EN_UPDATE(TXT_DA_DELAY, OnUpdateDaDelay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDeltaAlarm message handlers

BOOL COptionsDeltaAlarm::OnInitDialog() 
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
		m_lcBEnable[ctr] = config.m_BEnableDA[ctr];
		m_lcdSetpoint[ctr] = config.m_dDASetpoint[ctr];
		m_lciInterval[ctr] = config.m_iDAInterval[ctr];
		m_lciDelay[ctr] = config.m_iDADelay[ctr];
		m_lcdDuration[ctr] = config.m_dDADuration[ctr];
		m_lcBWAV[ctr] = config.m_BDAWAV[ctr];
		m_lctxtWAVFile[ctr] = config.m_szDAWAVFile[ctr];
		m_lcdWAVRepeatInterval[ctr] = config.m_dDAWAVRepeatInterval[ctr];
	}

	// Put the folder bmp on the buttons
	CButton * pButton;
	m_bmpFolder.LoadBitmap(BMP_FOLDER);
	pButton = (CButton* )GetDlgItem(BTN_DA_CHOOSEWAV);
	pButton->SetBitmap(m_bmpFolder);
	m_bmpPlay.LoadBitmap(BMP_PLAY);
	pButton = (CButton* )GetDlgItem(BTN_DA_PLAYWAV);
	pButton->SetBitmap(m_bmpPlay);
	m_bmpStop.LoadBitmap(BMP_STOP);

	m_iCurrentSelectedDataSource = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDeltaAlarm::OnShowWindow(BOOL bShow, UINT nStatus) 
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

			OnSelchangeDaDatasourcenames();

			UpdateData(FALSE);
			
			break;
		case 0: // Being Hidden
			break;
	}
	
}


void COptionsDeltaAlarm::OnSelchangeDaDatasourcenames() 
{
	m_iCurrentSelectedDataSource = m_cmbDataSourceNames.GetCurSel();

	m_BEnable = m_lcBEnable[m_iCurrentSelectedDataSource];
	m_dSetpoint = m_lcdSetpoint[m_iCurrentSelectedDataSource];
	m_iInterval = m_lciInterval[m_iCurrentSelectedDataSource];
	m_iDelay = m_lciDelay[m_iCurrentSelectedDataSource];
	m_dDuration = m_lcdDuration[m_iCurrentSelectedDataSource];
	m_BWAV = m_lcBWAV[m_iCurrentSelectedDataSource];
	m_txtWAVFile = m_lctxtWAVFile[m_iCurrentSelectedDataSource];
	m_dWAVRepeatInterval = m_lcdWAVRepeatInterval[m_iCurrentSelectedDataSource];

	UpdateData(FALSE);
}

void COptionsDeltaAlarm::OnEnableda() 
{
	UpdateData(TRUE);

	m_lcBEnable[m_iCurrentSelectedDataSource] = m_BEnable;
}

void COptionsDeltaAlarm::OnUpdateDaSetpoint() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lcdSetpoint[m_iCurrentSelectedDataSource] = m_dSetpoint;	
}

void COptionsDeltaAlarm::OnUpdateDaInterval() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lciInterval[m_iCurrentSelectedDataSource] = m_iInterval;
}

void COptionsDeltaAlarm::OnUpdateDaDuration() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lcdDuration[m_iCurrentSelectedDataSource] = m_dDuration;	
}

void COptionsDeltaAlarm::OnDaWav() 
{
	UpdateData(TRUE);

	m_lcBWAV[m_iCurrentSelectedDataSource] = m_BWAV;
}

void COptionsDeltaAlarm::OnUpdateDaWavfile() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lctxtWAVFile[m_iCurrentSelectedDataSource] = m_txtWAVFile;	
}

void COptionsDeltaAlarm::OnUpdateDaWavrepeatinterval() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lcdWAVRepeatInterval[m_iCurrentSelectedDataSource] = m_dWAVRepeatInterval;
}

BOOL COptionsDeltaAlarm::PreTranslateMessage(MSG* pMsg) 
{
	if ((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) ||
		(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE))
	{
		return TRUE; // just ignore the enter key
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void COptionsDeltaAlarm::OnDaChoosewav() 
{
	SelectWAVFile(&m_txtWAVFile);
	m_lctxtWAVFile[m_iCurrentSelectedDataSource] = m_txtWAVFile;
	UpdateData(FALSE);	
}

void COptionsDeltaAlarm::OnDaPlaywav() 
{
	if ((m_txtWAVFile.CompareNoCase("None Selected.") != 0) && (m_txtWAVFile.IsEmpty() == FALSE))
	{
		// The Enabling/Disabling of the buttons is not necessary on 
		// the newer, fast machines; but for the slower ones they're 
		// necessary for safety.
		(GetDlgItem(BTN_DA_PLAYWAV))->EnableWindow(FALSE);
		((CEFMDlg*)m_pMainWnd)->PlayWAV(0, m_txtWAVFile);
		(GetDlgItem(BTN_DA_PLAYWAV))->EnableWindow(TRUE);
	}
	else
	{
		::MessageBox(NULL,"Please select a WAV file first.","No WAV File Selected",MB_OK|MB_ICONERROR);
	}
}

bool COptionsDeltaAlarm::SelectWAVFile(CString * wavPath)
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

void COptionsDeltaAlarm::OnUpdateDaDelay() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_lciDelay[m_iCurrentSelectedDataSource] = m_iDelay;	
}
