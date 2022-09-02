#if !defined(AFX_OPTIONSIMAGECAPTURE_H__9109E72A_5BBA_49F6_AEE5_3C54643A8258__INCLUDED_)
#define AFX_OPTIONSIMAGECAPTURE_H__9109E72A_5BBA_49F6_AEE5_3C54643A8258__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsImageCapture.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsImageCapture dialog

class COptionsImageCapture : public CDialog
{
// Construction
public:
	COptionsImageCapture(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsImageCapture)
	enum { IDD = IDD_OPTIONS_IMAGECAPTURE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsImageCapture)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsImageCapture)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSIMAGECAPTURE_H__9109E72A_5BBA_49F6_AEE5_3C54643A8258__INCLUDED_)
