#if !defined(AFX_OPTIONSDATASHARING_H__1A5F18B0_E0B5_443B_81F1_7819F08F3E1F__INCLUDED_)
#define AFX_OPTIONSDATASHARING_H__1A5F18B0_E0B5_443B_81F1_7819F08F3E1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDataSharing.h : header file
//

#include "Configuration.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsDataSharing dialog

class COptionsDataSharing : public CDialog
{
// Construction
public:
	int m_lciDataSharingPort[MAX_NUMOFDATASOURCES] = {};
	BOOL m_lcBDataSharingEnabled[MAX_NUMOFDATASOURCES] = {};
	int m_iCurrentSelectedDataSource = 0;
	int m_lciNumberOfDataSources = 0;
	COptionsDataSharing(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDataSharing)
	enum { IDD = IDD_OPTIONS_DATASHARING };
	CComboBox	m_cmbDataSourceNames;
	BOOL	m_BDataSharingEnabled = FALSE;
	int		m_iDataSharingPort = 0;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDataSharing)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDataSharing)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEnablesharing();
	afx_msg void OnSelchangeDatasourcenames();
	afx_msg void OnUpdateListenport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDATASHARING_H__1A5F18B0_E0B5_443B_81F1_7819F08F3E1F__INCLUDED_)
