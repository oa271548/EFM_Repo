#if !defined(AFX_SMSRECIPIENTS_H__F3E6321D_C9D5_44CF_B4AE_5F265A7DF820__INCLUDED_)
#define AFX_SMSRECIPIENTS_H__F3E6321D_C9D5_44CF_B4AE_5F265A7DF820__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SMSRecipients.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSMSRecipients dialog

class CSMSRecipients : public CDialog
{
// Construction
public:
	CSMSRecipients(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSMSRecipients)
	enum { IDD = IDD_SMSRECIPIENTS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMSRecipients)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSMSRecipients)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMSRECIPIENTS_H__F3E6321D_C9D5_44CF_B4AE_5F265A7DF820__INCLUDED_)
