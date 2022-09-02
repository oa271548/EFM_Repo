#if !defined(AFX_WAITDLG_H__49EF2A1E_0A7C_48C4_855C_73DE9E8E69AA__INCLUDED_)
#define AFX_WAITDLG_H__49EF2A1E_0A7C_48C4_855C_73DE9E8E69AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaitDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg dialog

class CWaitDlg : public CDialog
{
// Construction
public:
	CFont m_fontMain;
	CFont m_fontSub;
	void ShowSplashScreen(CString szMain, CString szSub);
	CWaitDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaitDlg)
	enum { IDD = IDD_WAITDLG };
	CString	m_lblWaitText;
	CString	m_lblWaitSubText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaitDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAITDLG_H__49EF2A1E_0A7C_48C4_855C_73DE9E8E69AA__INCLUDED_)
