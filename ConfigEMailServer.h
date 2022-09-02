#if !defined(AFX_CONFIGEMAILSERVER_H__44DB3929_9121_4CD6_8D5D_07AF6DFE4C2E__INCLUDED_)
#define AFX_CONFIGEMAILSERVER_H__44DB3929_9121_4CD6_8D5D_07AF6DFE4C2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigEMailServer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigEMailServer dialog

class CConfigEMailServer : public CDialog
{
// Construction
public:
	COptionsAlerts* parent;
	CConfigEMailServer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigEMailServer)
	enum { IDD = IDD_CONFIGUREEMAILSERVER };
	CString			m_lctxtSmtpServer;
	CString			m_lctxtAlertsSmtpServerName;
	int				m_lciSmtpServerPort;
	int				m_lciSmtpSecurityType;
	CString			m_lctxtSmtpUsername;
	CString			m_lctxtSmtpPassword;
	CString			m_lctxtSmtpSenderName;
	CString			m_lctxtSmtpSenderEMail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigEMailServer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigEMailServer)
	virtual BOOL OnInitDialog();
	afx_msg void OnAccept();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGEMAILSERVER_H__44DB3929_9121_4CD6_8D5D_07AF6DFE4C2E__INCLUDED_)
