#if !defined(AFX_OPTIONSDATALOGGING_H__33990D2E_E22D_4D61_B96B_4E14FE8C3731__INCLUDED_)
#define AFX_OPTIONSDATALOGGING_H__33990D2E_E22D_4D61_B96B_4E14FE8C3731__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDataLogging.h : header file
//

#include "Configuration.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsDataLogging dialog

class COptionsDataLogging : public CDialog
{
// Construction
public:
	CBitmap m_bmpFolder;
	int m_iCurrentSelectedDataSource;
	int m_lciNumberOfDataSources;
	CString m_laszDataSourceName[MAX_NUMOFDATASOURCES];
	BOOL m_laBEnableDataLogging[MAX_NUMOFDATASOURCES];
	CString m_laszDataLoggingPath[MAX_NUMOFDATASOURCES];
	BOOL m_laBHighResolutionLogging[MAX_NUMOFDATASOURCES];

	COptionsDataLogging(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDataLogging)
	enum { IDD = IDD_OPTIONS_DATALOGGING };
	CComboBox	m_cmbDataSourceNames;
	BOOL	m_BEnableDataLogging;
	CString	m_txtLogLocation;
	BOOL	m_BHighResolutionLogging;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDataLogging)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDataLogging)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangeDlDatasources();
	afx_msg void OnEnabledatalogging();
	afx_msg void OnLoglocation();
	afx_msg void OnHighresolution();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDATALOGGING_H__33990D2E_E22D_4D61_B96B_4E14FE8C3731__INCLUDED_)
