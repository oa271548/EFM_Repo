#if !defined(AFX_EFMOPTIONS_H__5AABE3A3_99FC_4324_AE76_4ED0848B21F2__INCLUDED_)
#define AFX_EFMOPTIONS_H__5AABE3A3_99FC_4324_AE76_4ED0848B21F2__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EFMOptions.h : header file
//

#include "OptionsGeneral.h"
#include "OptionsDataLogging.h"
#include "OptionsImageCapture.h"
#include "OptionsDataSharing.h"
#include "OptionsHighFieldAlarm.h"	// Added by ClassView
#include "OptionsVeryHighFieldAlarm.h"	// Added by ClassView
#include "OptionsLightningAlarm.h"	// Added by ClassView
#include "OptionsDeltaAlarm.h"	// Added by ClassView
#include "OptionsAlerts.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CEFMOptions dialog

class CEFMOptions : public CDialog
{
// Construction
public:
	bool HasStuffChanged(int iFMID);
	BOOL	old_SourceEnabled[MAX_NUMOFDATASOURCES];
	CString old_SourcePoint[MAX_NUMOFDATASOURCES];
	CString old_SourceComPort[MAX_NUMOFDATASOURCES];
	CString	old_SourceNetAddress[MAX_NUMOFDATASOURCES];
	int		old_SourceNetPort[MAX_NUMOFDATASOURCES];
	int		old_SourceBaudRate[MAX_NUMOFDATASOURCES];
	CString old_EFA20SerialNumber[MAX_NUMOFDATASOURCES];
	BOOL	old_SourceSharingEnabled[MAX_NUMOFDATASOURCES];
	int		old_SourceSharingPort[MAX_NUMOFDATASOURCES];
	BOOL	old_DataLogEnabled[MAX_NUMOFDATASOURCES];
	BOOL	old_HighResolutionLogging[MAX_NUMOFDATASOURCES];

	bool StopServerThread(int idx);
	bool StartServerThread(int idx);
	CDialog* m_pMainWnd;
	CRect m_rectOptionSubWindow;
	COptionsGeneral dlgGeneral;
	COptionsDataLogging dlgDataLogging;
	COptionsImageCapture dlgImageCapture;
	COptionsDataSharing dlgDataSharing;
	COptionsLightningAlarm dlgLightningAlarm;
	COptionsVeryHighFieldAlarm dlgVeryHighFieldAlarm;
	COptionsHighFieldAlarm dlgHighFieldAlarm;
	COptionsDeltaAlarm dlgDeltaAlarm;
	COptionsAlerts dlgAlerts;
	CImageList m_ilMenu;
	CEFMOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEFMOptions)
	enum { IDD = IDD_OPTIONS };
	CTabCtrl	m_tabGeneral;
	CListCtrl	m_lstOptions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEFMOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEFMOptions)
	afx_msg void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickOptions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAccept();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFMOPTIONS_H__5AABE3A3_99FC_4324_AE76_4ED0848B21F2__INCLUDED_)
