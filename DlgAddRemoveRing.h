#if !defined(AFX_DLGADDREMOVERING_H__03123AC0_8874_4C47_802F_4CD1B643C911__INCLUDED_)
#define AFX_DLGADDREMOVERING_H__03123AC0_8874_4C47_802F_4CD1B643C911__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddRemoveRing.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAddRemoveRing dialog

#define ADD		0
#define	REMOVE	1

#define MI	0
#define KM	1


class CDlgAddRemoveRing : public CDialog
{
// Construction
public:
	CDlgAddRemoveRing(CWnd* pParent = NULL, int iMode = ADD, int iDistanceUnit = MI);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAddRemoveRing)
	enum { IDD = IDD_ADDREMOVERINGS };
	CString	m_txtAddRemoveLabel;
	UINT	m_uiAddRemoveValue;
	CString	m_txtDistanceUnit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddRemoveRing)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAddRemoveRing)
	virtual BOOL OnInitDialog();
	afx_msg void OnAccept();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_iDistanceUnit;
	int m_iMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDREMOVERING_H__03123AC0_8874_4C47_802F_4CD1B643C911__INCLUDED_)
