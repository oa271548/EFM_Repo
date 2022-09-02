#if !defined(AFX_ADDEDITRECIPIENT_H__CA6577D3_7BA8_486B_BC08_1F00968DCAF8__INCLUDED_)
#define AFX_ADDEDITRECIPIENT_H__CA6577D3_7BA8_486B_BC08_1F00968DCAF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddEditRecipient.h : header file
//

#define ADD_EMAIL		0
#define	EDIT_EMAIL		1
#define ADD_SMS			2
#define	EDIT_SMS		3


/////////////////////////////////////////////////////////////////////////////
// CAddEditRecipient dialog

class CAddEditRecipient : public CDialog
{
// Construction
public:
	CAddEditRecipient(CWnd* pParent = NULL, int iMode = ADD_EMAIL, CString* data = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddEditRecipient)
	enum { IDD = IDD_ADDEDITRECIPIENT };
	CString	m_editbox2;
	CString	m_editbox1;
	CString	m_label1;
	CString	m_label2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddEditRecipient)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	int m_iMode;
	CString* m_data;

	// Generated message map functions
	//{{AFX_MSG(CAddEditRecipient)
	afx_msg void OnAccept();
	afx_msg void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDEDITRECIPIENT_H__CA6577D3_7BA8_486B_BC08_1F00968DCAF8__INCLUDED_)
