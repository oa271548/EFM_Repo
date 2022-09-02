#if !defined(AFX_DLGDEBUGGER_H__CDE1041E_83C1_4147_AF7A_03DB400FC29D__INCLUDED_)
#define AFX_DLGDEBUGGER_H__CDE1041E_83C1_4147_AF7A_03DB400FC29D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDebugger.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDebugger dialog
#include <afxmt.h>

class CDlgDebugger : public CDialog
{
// Construction
public:
	void UpdateClient(int iID, int iFIFOSize);
	void RemoveClient(int iID);
	int AddClient(CString txtIPAddress);
	void SetClientCount(int iCount);


	CDlgDebugger(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDebugger)
	enum { IDD = IDD_DEBUGGER };
	CListCtrl	m_lstConnectedClients;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDebugger)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDebugger)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_iIndex;
	CCriticalSection m_csClientList;
	CCriticalSection m_csClientCount;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDEBUGGER_H__CDE1041E_83C1_4147_AF7A_03DB400FC29D__INCLUDED_)
