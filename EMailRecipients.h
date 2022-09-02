#if !defined(AFX_EMAILRECIPIENTS_H__4C1D077E_3135_4EE1_A269_1376E9AA31E9__INCLUDED_)
#define AFX_EMAILRECIPIENTS_H__4C1D077E_3135_4EE1_A269_1376E9AA31E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EMailRecipients.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEMailRecipients dialog

class CEMailRecipients : public CDialog
{
// Construction
public:
	CEMailRecipients(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEMailRecipients)
	enum { IDD = IDD_EMAILRECIPIENTS };
	CListCtrl	m_lstRecipients;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEMailRecipients)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEMailRecipients)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMAILRECIPIENTS_H__4C1D077E_3135_4EE1_A269_1376E9AA31E9__INCLUDED_)
