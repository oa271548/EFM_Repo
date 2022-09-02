#if !defined(AFX_EVENTLOGVIEWER_H__E7C2126F_309C_47A0_8013_48C0F4F06679__INCLUDED_)
#define AFX_EVENTLOGVIEWER_H__E7C2126F_309C_47A0_8013_48C0F4F06679__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventLogViewer.h : header file
//

#include "EFMDlg.h"

#define NUM_LOGLINES	20

/////////////////////////////////////////////////////////////////////////////
// CEventLogViewer dialog

class CEventLogViewer : public CDialog
{
// Construction
public:
	long PopulateLogListBox(long lPtr);
	CEFMDlg * pMyOwner = nullptr;
//	CFile m_fileEventLog;
	CEventLogViewer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEventLogViewer)
	enum { IDD = IDD_EVENTLOG };
	CListBox	m_lstEvents;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventLogViewer)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventLogViewer)
	afx_msg void OnClose();
	afx_msg void OnClearlogfile();
	virtual BOOL OnInitDialog();
	afx_msg void OnRefreshlog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg void OnLineup();
	afx_msg void OnPageup();
	afx_msg void OnPagedown();
	afx_msg void OnHead();
	afx_msg void OnTail();
	afx_msg void OnLinedown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	long m_lScrollBarDragPosition = 0;
	HACCEL m_hAccel = nullptr; // accelerator table
	long m_lLowerLimit = 0;
	long m_lFileSize = 0;
	long m_lCurrentFilePosition = 0;
	long SeekForeOneLine();
	long SeekBackOneLine();
	void LayoutControls();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTLOGVIEWER_H__E7C2126F_309C_47A0_8013_48C0F4F06679__INCLUDED_)
