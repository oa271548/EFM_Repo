// ConfigGSMGateway.cpp : implementation file
//

#include "stdafx.h"
#include "efm.h"
#include "ConfigGSMGateway.h"
#include "Configuration.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigGSMGateway dialog


CConfigGSMGateway::CConfigGSMGateway(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigGSMGateway::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigGSMGateway)
	m_txtApiId = _T("");
	m_txtPassword = _T("");
	m_txtUserName = _T("");
	//}}AFX_DATA_INIT
}


void CConfigGSMGateway::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigGSMGateway)
	DDX_Radio(pDX, IDC_RADIOCLICKATELL, m_iProvider);
	DDX_Text(pDX, IDC_APIID, m_txtApiId);
	DDX_Text(pDX, IDC_PASSWORD, m_txtPassword);
	DDX_Text(pDX, IDC_USERNAME, m_txtUserName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigGSMGateway, CDialog)
	//{{AFX_MSG_MAP(CConfigGSMGateway)
	ON_BN_CLICKED(BTN_ACCEPT, OnAccept)
	ON_BN_CLICKED(BTN_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADIOCLICKATELL, &CConfigGSMGateway::OnBnClickedRadioclickatell)
	ON_BN_CLICKED(IDC_RADIOTWILIO, &CConfigGSMGateway::OnBnClickedRadiotwilio)
END_MESSAGE_MAP()

extern CConfiguration config;

/////////////////////////////////////////////////////////////////////////////
// CConfigGSMGateway message handlers


BOOL CConfigGSMGateway::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_txtApiId = config.txtSmsGatewayApiId;
	m_txtPassword = config.txtSmsGatewayPassword;
	m_txtUserName = config.txtSmsGatewayUsername;
	m_iProvider = config.iSmsGateWayProvider;
	if(m_iProvider == 1)
	{
		CString txt;
		txt.LoadString(STR_TWILIO_ACCOUNT_SID);
		((CStatic*)GetDlgItem(IDC_STATIC_USERNAME))->SetWindowText(txt);
		txt.LoadString(STR_TWILIO_AUTH_TOKEN);
		((CStatic*)GetDlgItem(IDC_STATIC_PASSWORD))->SetWindowText(txt);
		((CStatic*)GetDlgItem(IDC_STATIC_API_ID))->SetWindowPos(nullptr, 24, 100, 140, 23, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
		txt.LoadString(STR_TWILIO_PHONE_NUMBER);
		((CStatic*)GetDlgItem(IDC_STATIC_API_ID))->SetWindowText(txt);
		((CEdit*)GetDlgItem(IDC_APIID))->SetWindowPos(nullptr, 165, 96, 149, 23, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
	}
	UpdateData(FALSE); // load controls from vars

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigGSMGateway::OnAccept() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true); // load vars from controls

	config.txtSmsGatewayApiId = m_txtApiId;
	config.txtSmsGatewayPassword = m_txtPassword;
	config.txtSmsGatewayUsername = m_txtUserName;
	config.iSmsGateWayProvider = m_iProvider;
	if (m_iProvider == 0)
	{
		config.txtSmsGatewayUrl = "http://api.clickatell.com/http/sendmsg?";
	}
	else if(m_iProvider == 1)
	{
		config.txtSmsGatewayUrl = "https://api.twilio.com/2010-04-01/Accounts/";
	}

	// Save the configuration
	config.SaveConfiguration();

	EndDialog(1);	
}

void CConfigGSMGateway::OnCancel() 
{
	// TODO: Add your control notification handler code here
	EndDialog(-1);	
	
}


void CConfigGSMGateway::OnBnClickedRadioclickatell()
{
	CString txt;
	txt.LoadString(STR_CLICK_A_TELL_USERNAME);
	((CStatic*)GetDlgItem(IDC_STATIC_USERNAME))->SetWindowText(txt);
	txt.LoadString(STR_CLICK_A_TELL_PASSWORD);
	((CStatic*)GetDlgItem(IDC_STATIC_PASSWORD))->SetWindowText(txt);

	((CStatic*)GetDlgItem(IDC_STATIC_API_ID))->SetWindowPos(nullptr, 24, 100, 65, 23, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
	txt.LoadString(STR_CLICK_A_TELL_API_ID);
	((CStatic*)GetDlgItem(IDC_STATIC_API_ID))->SetWindowText(txt);
	((CEdit*)GetDlgItem(IDC_APIID))->SetWindowPos(nullptr, 110, 96, 149, 23, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
	UpdateData(TRUE);
}


void CConfigGSMGateway::OnBnClickedRadiotwilio()
{
	CString txt;
	txt.LoadString(STR_TWILIO_ACCOUNT_SID);
	((CStatic*)GetDlgItem(IDC_STATIC_USERNAME))->SetWindowText(txt);
	txt.LoadString(STR_TWILIO_AUTH_TOKEN);
	((CStatic*)GetDlgItem(IDC_STATIC_PASSWORD))->SetWindowText(txt);
	((CStatic*)GetDlgItem(IDC_STATIC_API_ID))->SetWindowPos(nullptr, 24, 100, 140, 23, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
	txt.LoadString(STR_TWILIO_PHONE_NUMBER);
	((CStatic*)GetDlgItem(IDC_STATIC_API_ID))->SetWindowText(txt);
	((CEdit*)GetDlgItem(IDC_APIID))->SetWindowPos(nullptr, 165, 96, 149, 23, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
	UpdateData(TRUE);
}
