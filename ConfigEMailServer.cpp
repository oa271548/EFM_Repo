// ConfigEMailServer.cpp : implementation file
//

#include "stdafx.h"
#include "efm.h"
#include "OptionsAlerts.h"
#include "ConfigEMailServer.h"
#include "Configuration.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CConfiguration config;


/////////////////////////////////////////////////////////////////////////////
// CConfigEMailServer dialog


CConfigEMailServer::CConfigEMailServer(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigEMailServer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigEMailServer)
	m_lciSmtpServerPort = 25;
	m_lctxtSmtpPassword = _T("");
	m_lctxtSmtpSenderEMail = _T("");
	m_lctxtSmtpSenderName = _T("");
	m_lctxtSmtpServer = _T("");
	m_lctxtSmtpUsername = _T("");
	m_lciSmtpSecurityType = -1;
	//}}AFX_DATA_INIT
	parent = (COptionsAlerts*)pParent;
}


void CConfigEMailServer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigEMailServer)
	DDX_Text(pDX, TXT_SERVER_PORT, m_lciSmtpServerPort);
	DDX_Text(pDX, TXT_SMTP_PASSWORD, m_lctxtSmtpPassword);
	DDX_Text(pDX, TXT_SMTP_SENDER_EMAIL, m_lctxtSmtpSenderEMail);
	DDX_Text(pDX, TXT_SMTP_SENDER_NAME, m_lctxtSmtpSenderName);
	DDX_Text(pDX, TXT_SMTP_SERVER, m_lctxtSmtpServer);
	DDX_Text(pDX, TXT_SMTP_USERNAME, m_lctxtSmtpUsername);
	DDX_Radio(pDX, IDC_SECURITYTYPE1, m_lciSmtpSecurityType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigEMailServer, CDialog)
	//{{AFX_MSG_MAP(CConfigEMailServer)
	ON_BN_CLICKED(BTN_ACCEPT, OnAccept)
	ON_BN_CLICKED(BTN_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigEMailServer message handlers

BOOL CConfigEMailServer::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_lctxtSmtpServer = config.txtSmtpServer;
	m_lciSmtpServerPort = config.iSmtpServerPort;
	m_lciSmtpSecurityType = config.iSmtpSecurityType;
	m_lctxtSmtpUsername = config.txtSmtpUsername;
	m_lctxtSmtpPassword = config.txtSmtpPassword;
	m_lctxtSmtpSenderName = config.txtSmtpSenderName;
	m_lctxtSmtpSenderEMail = config.txtSmtpSenderEMail;
	
	UpdateData(FALSE); // load controls fom vars

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigEMailServer::OnAccept() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true); // load vars from controls

	// E-Mail
	config.txtSmtpServer = m_lctxtSmtpServer;
	config.iSmtpServerPort = m_lciSmtpServerPort;
	config.iSmtpSecurityType = m_lciSmtpSecurityType;
	config.txtSmtpUsername = m_lctxtSmtpUsername;
	config.txtSmtpPassword = m_lctxtSmtpPassword;
	config.txtSmtpSenderName = m_lctxtSmtpSenderName;
	config.txtSmtpSenderEMail = m_lctxtSmtpSenderEMail;

	// Save the configuration
	config.SaveConfiguration();

	EndDialog(1);	

}

void CConfigEMailServer::OnCancel() 
{
	// TODO: Add your control notification handler code here
	EndDialog(-1);	

}

