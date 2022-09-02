// OptionsDataSharing.cpp : implementation file
//

#include "stdafx.h"
#include "EFM.h"
#include "OptionsDataSharing.h"
#include "EFMOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CConfiguration config;
extern CEFMOptions * dlgOptions;

/////////////////////////////////////////////////////////////////////////////
// COptionsDataSharing dialog


COptionsDataSharing::COptionsDataSharing(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDataSharing::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDataSharing)
	m_BDataSharingEnabled = FALSE;
	m_iDataSharingPort = 0;
	//}}AFX_DATA_INIT
}


void COptionsDataSharing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDataSharing)
	DDX_Control(pDX, CMB_DATASOURCENAMES, m_cmbDataSourceNames);
	DDX_Check(pDX, CHK_ENABLESHARING, m_BDataSharingEnabled);
	DDX_Text(pDX, TXT_LISTENPORT, m_iDataSharingPort);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDataSharing, CDialog)
	//{{AFX_MSG_MAP(COptionsDataSharing)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(CHK_ENABLESHARING, OnEnablesharing)
	ON_CBN_SELCHANGE(CMB_DATASOURCENAMES, OnSelchangeDatasourcenames)
	ON_EN_UPDATE(TXT_LISTENPORT, OnUpdateListenport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDataSharing message handlers

BOOL COptionsDataSharing::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if ((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) ||
		(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE))
	{
		return TRUE; // just ignore the enter key
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL COptionsDataSharing::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int ctr;

	m_lciNumberOfDataSources = config.m_iNumberOfDataSources;

	for (ctr=0; ctr<config.m_iNumberOfDataSources; ctr++)
	{
		m_cmbDataSourceNames.AddString(config.m_szDataSourceName[ctr]);
	}
	
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		m_lcBDataSharingEnabled[ctr] = config.m_BDataSharingEnabled[ctr];
		m_lciDataSharingPort[ctr] = config.m_iDataSharingPort[ctr];
	}

	m_iCurrentSelectedDataSource = 0;
		
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void COptionsDataSharing::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	dlgOptions->dlgGeneral.UpdateData(TRUE);

	switch (bShow)
	{
		case 1:	// Being Shown
			// Clear the Data Source Names combo box
			m_cmbDataSourceNames.ResetContent();
			// Update the number of data sources from the general box
			m_lciNumberOfDataSources = dlgOptions->dlgGeneral.m_lciNumberOfDataSources;
			// Update the local name array with the names from the general box
			//for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
			//{	m_lcszDataSourceName[ctr] = dlgOptions->dlgGeneral.m_lcszDataSourceName[ctr];	}
			// Repopulate the Data Source Names combo box
			for (size_t ctr=0; ctr < dlgOptions->dlgGeneral.m_lciNumberOfDataSources; ++ctr)
			{
				m_cmbDataSourceNames.AddString(dlgOptions->dlgGeneral.m_lcszDataSourceName[ctr]);
			}
			// Show the settings for the currently selected item
			//m_iCurrentSelectedDataSource = 0;
			m_cmbDataSourceNames.SelectString(-1,dlgOptions->dlgGeneral.m_lcszDataSourceName[m_iCurrentSelectedDataSource]);

			// 
			OnSelchangeDatasourcenames();

			UpdateData(FALSE);
			
			break;
		case 0: // Being Hidden
			break;
	}

	// Disable data sharing for sources not being used.
	for (size_t ctr = m_lciNumberOfDataSources; ctr<=MAX_NUMOFDATASOURCES; ctr++)
	{
		m_lcBDataSharingEnabled[ctr] = FALSE;
	}

}

void COptionsDataSharing::OnEnablesharing() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_lcBDataSharingEnabled[m_iCurrentSelectedDataSource] = m_BDataSharingEnabled;

	(GetDlgItem(LBL_LISTENPORT))->EnableWindow(m_lcBDataSharingEnabled[m_iCurrentSelectedDataSource]);
	(GetDlgItem(TXT_LISTENPORT))->EnableWindow(m_lcBDataSharingEnabled[m_iCurrentSelectedDataSource]);
}

void COptionsDataSharing::OnSelchangeDatasourcenames() 
{
	// TODO: Add your control notification handler code here
	m_iCurrentSelectedDataSource = m_cmbDataSourceNames.GetCurSel();

	m_BDataSharingEnabled = m_lcBDataSharingEnabled[m_iCurrentSelectedDataSource];
	m_iDataSharingPort = m_lciDataSharingPort[m_iCurrentSelectedDataSource];

	(GetDlgItem(LBL_LISTENPORT))->EnableWindow(m_lcBDataSharingEnabled[m_iCurrentSelectedDataSource]);
	(GetDlgItem(TXT_LISTENPORT))->EnableWindow(m_lcBDataSharingEnabled[m_iCurrentSelectedDataSource]);

	UpdateData(FALSE);	
}

void COptionsDataSharing::OnUpdateListenport() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_lciDataSharingPort[m_iCurrentSelectedDataSource] = m_iDataSharingPort;
	
}
