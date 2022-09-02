#if !defined(AFX_CONFIGGSMGATEWAY_H__4EE2096E_7B9D_49D3_B460_B2EE78E445BD__INCLUDED_)
#define AFX_CONFIGGSMGATEWAY_H__4EE2096E_7B9D_49D3_B460_B2EE78E445BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigGSMGateway.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigGSMGateway dialog

class CConfigGSMGateway : public CDialog
{
// Construction
public:
	CConfigGSMGateway(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigGSMGateway)
	enum { IDD = IDD_CONFIGUREGSMGATEWAY };
	CString	m_txtApiId;
	CString	m_txtPassword;
	CString	m_txtUserName;
	int m_iProvider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigGSMGateway)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigGSMGateway)
	afx_msg void OnAccept();
	afx_msg void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioclickatell();
	afx_msg void OnBnClickedRadiotwilio();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGGSMGATEWAY_H__4EE2096E_7B9D_49D3_B460_B2EE78E445BD__INCLUDED_)
