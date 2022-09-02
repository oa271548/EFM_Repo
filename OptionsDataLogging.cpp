// OptionsDataLogging.cpp : implementation file
//

#include "stdafx.h"
#include "EFM.h"
#include "OptionsDataLogging.h"
#include "EFMOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CConfiguration config;
extern CEFMOptions * dlgOptions;

/////////////////////////////////////////////////////////////////////////////
// COptionsDataLogging dialog


COptionsDataLogging::COptionsDataLogging(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDataLogging::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDataLogging)
	m_BEnableDataLogging = FALSE;
	m_txtLogLocation = _T("");
	m_BHighResolutionLogging = FALSE;
	//}}AFX_DATA_INIT
}


void COptionsDataLogging::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDataLogging)
	DDX_Control(pDX, CMB_DL_DATASOURCES, m_cmbDataSourceNames);
	DDX_Check(pDX, CHK_ENABLEDATALOGGING, m_BEnableDataLogging);
	DDX_Text(pDX, TXT_LOGLOCATION, m_txtLogLocation);
	DDX_Check(pDX, CHK_HIGHRESOLUTION, m_BHighResolutionLogging);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDataLogging, CDialog)
	//{{AFX_MSG_MAP(COptionsDataLogging)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(CMB_DL_DATASOURCES, OnSelchangeDlDatasources)
	ON_BN_CLICKED(CHK_ENABLEDATALOGGING, OnEnabledatalogging)
	ON_BN_CLICKED(BTN_LOGLOCATION, OnLoglocation)
	ON_BN_CLICKED(CHK_HIGHRESOLUTION, OnHighresolution)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDataLogging message handlers

BOOL COptionsDataLogging::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int ctr;

	m_bmpFolder.LoadBitmap(BMP_FOLDER);
	CButton * pButton = (CButton* )GetDlgItem(BTN_LOGLOCATION);
	pButton->SetBitmap(m_bmpFolder);

	m_lciNumberOfDataSources = config.m_iNumberOfDataSources;
	
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		m_laszDataSourceName[ctr] = config.m_szDataSourceName[ctr];
		m_laBEnableDataLogging[ctr] = config.m_BEnableDataLogging[ctr];
		m_laszDataLoggingPath[ctr] = config.m_szDataLoggingPath[ctr];
		m_laBHighResolutionLogging[ctr] = config.m_BHighResolutionLogging[ctr];
	}

	for (ctr=0; ctr<m_lciNumberOfDataSources; ctr++)
	{
		m_cmbDataSourceNames.AddString(m_laszDataSourceName[ctr]);
	}

	// Load the first item
	m_iCurrentSelectedDataSource = 0;
	m_cmbDataSourceNames.SelectString(-1,m_laszDataSourceName[m_iCurrentSelectedDataSource]);
	m_BEnableDataLogging = m_laBEnableDataLogging[m_iCurrentSelectedDataSource];
	m_txtLogLocation = m_laszDataLoggingPath[m_iCurrentSelectedDataSource];
	m_BHighResolutionLogging = m_laBHighResolutionLogging[m_iCurrentSelectedDataSource];

	// Enable or disable depending on settings
	(GetDlgItem(TXT_LOGLOCATION))->EnableWindow(m_BEnableDataLogging);
	(GetDlgItem(BTN_LOGLOCATION))->EnableWindow(m_BEnableDataLogging);	
	// Enable/Disable High Resolution data logging
	(GetDlgItem(CHK_HIGHRESOLUTION))->EnableWindow(m_BEnableDataLogging);	
	(GetDlgItem(CHK_HIGHRESOLUTION))->EnableWindow(m_BEnableDataLogging);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDataLogging::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	int ctr;
	CString txt;

	dlgOptions->dlgGeneral.UpdateData(TRUE);

	switch (bShow)
	{
		case 1:	// Being Shown
			// Clear the Data Source Names combo box
			m_cmbDataSourceNames.ResetContent();
			// Update the number of data sources from the general box
			m_lciNumberOfDataSources = dlgOptions->dlgGeneral.m_lciNumberOfDataSources;
			// Update the local name array with the names from the general box
			for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
			{	m_laszDataSourceName[ctr] = dlgOptions->dlgGeneral.m_lcszDataSourceName[ctr];	}
			// Repopulate the Data Source Names combo box
			for (ctr=0; ctr<m_lciNumberOfDataSources; ctr++)
			{
				m_cmbDataSourceNames.AddString(m_laszDataSourceName[ctr]);
			}
			// Show the settings for the currently selected item
			m_iCurrentSelectedDataSource = 0;
			m_cmbDataSourceNames.SelectString(-1,m_laszDataSourceName[m_iCurrentSelectedDataSource]);
			m_txtLogLocation = m_laszDataLoggingPath[m_iCurrentSelectedDataSource];
			UpdateData(FALSE);
			
			break;
		case 0: // Being Hidden
			// Make sure the user entered a path if they enabled datalogging
			break;
	}
	
}

void COptionsDataLogging::OnSelchangeDlDatasources() 
{
	// TODO: Add your control notification handler code here
	m_iCurrentSelectedDataSource = m_cmbDataSourceNames.GetCurSel();

	m_BEnableDataLogging = m_laBEnableDataLogging[m_iCurrentSelectedDataSource];
	m_txtLogLocation = m_laszDataLoggingPath[m_iCurrentSelectedDataSource];
	m_BHighResolutionLogging = m_laBHighResolutionLogging[m_iCurrentSelectedDataSource];

	// Enable or disable depending on settings
	(GetDlgItem(TXT_LOGLOCATION))->EnableWindow(m_BEnableDataLogging);
	(GetDlgItem(BTN_LOGLOCATION))->EnableWindow(m_BEnableDataLogging);	

	// Enable/Disable High Resolution data logging
	(GetDlgItem(CHK_HIGHRESOLUTION))->EnableWindow(m_BEnableDataLogging);	
	(GetDlgItem(CHK_HIGHRESOLUTION))->EnableWindow(m_BEnableDataLogging);

	UpdateData(FALSE);	
}

void COptionsDataLogging::OnEnabledatalogging() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	m_laBEnableDataLogging[m_iCurrentSelectedDataSource] = m_BEnableDataLogging;

	// Enable/Disable Logging controls
	(GetDlgItem(TXT_LOGLOCATION))->EnableWindow(m_BEnableDataLogging);
	(GetDlgItem(BTN_LOGLOCATION))->EnableWindow(m_BEnableDataLogging);	

	// Enable/Disable High Resolution data logging
	(GetDlgItem(CHK_HIGHRESOLUTION))->EnableWindow(m_BEnableDataLogging);	
	(GetDlgItem(CHK_HIGHRESOLUTION))->EnableWindow(m_BEnableDataLogging);
}

void COptionsDataLogging::OnLoglocation() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	LPMALLOC pMalloc;
	if (SHGetMalloc (&pMalloc) != NOERROR) return;

	BROWSEINFO bInfo;
	LPITEMIDLIST pidl;
	char szPath[_MAX_PATH + 1];

	memset(&bInfo, 0, sizeof(BROWSEINFO));

	bInfo.hwndOwner = NULL;
	bInfo.pszDisplayName = szPath;
	bInfo.lpszTitle = "Select folder";
	// 0x40 adds "Make New Folder" button
	bInfo.ulFlags = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS|BIF_EDITBOX|0x40;

	pidl = ::SHBrowseForFolder (&bInfo);

	if (pidl)	// User hits OK
	{
		::SHGetPathFromIDList(pidl, szPath);
		pMalloc->Free(pidl);
		// szPath contains the chosen path!
		if (lstrlen(szPath) == 0)
		{
			::MessageBox(NULL,"Please select a real folder.","Bad Folder Selection",MB_OK|MB_ICONERROR);
		}
		else
		{
			if (szPath[lstrlen(szPath)-1] != '\\')
			{
				lstrcat(szPath,"\\");
			}
			m_txtLogLocation = szPath;
			m_laszDataLoggingPath[m_iCurrentSelectedDataSource] = szPath;

		}
	}
	
	pMalloc->Release();
	UpdateData(FALSE);

}

BOOL COptionsDataLogging::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if ((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) ||
		(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE))
	{
		return TRUE; // just ignore the enter key
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void COptionsDataLogging::OnHighresolution() 
{
	UpdateData(TRUE);
	
	m_laBHighResolutionLogging[m_iCurrentSelectedDataSource] = m_BHighResolutionLogging;
	
}
