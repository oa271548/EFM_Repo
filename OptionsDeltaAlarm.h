#if !defined(AFX_OPTIONSDELTAALARM_H__B30223C1_E571_4494_9623_5DEB34388F1A__INCLUDED_)
#define AFX_OPTIONSDELTAALARM_H__B30223C1_E571_4494_9623_5DEB34388F1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDeltaAlarm.h : header file
//
#include "Configuration.h"
/////////////////////////////////////////////////////////////////////////////
// COptionsDeltaAlarm dialog

class COptionsDeltaAlarm : public CDialog
{
// Construction
public:
	CDialog* m_pMainWnd;
	bool SelectWAVFile(CString * wavPath);
	int m_iCurrentSelectedDataSource;
	int m_iNumberOfDataSources;
	COptionsDeltaAlarm(CWnd* pParent = NULL);   // standard constructor
	CBitmap m_bmpFolder;
	CBitmap m_bmpPlay;
	CBitmap m_bmpStop;
	BOOL	m_lcBEnable[MAX_NUMOFDATASOURCES];
	double	m_lcdSetpoint[MAX_NUMOFDATASOURCES];
	int		m_lciInterval[MAX_NUMOFDATASOURCES];
	int		m_lciDelay[MAX_NUMOFDATASOURCES];
	double	m_lcdDuration[MAX_NUMOFDATASOURCES];
	BOOL	m_lcBWAV[MAX_NUMOFDATASOURCES];
	CString	m_lctxtWAVFile[MAX_NUMOFDATASOURCES];
	double	m_lcdWAVRepeatInterval[MAX_NUMOFDATASOURCES];

// Dialog Data
	//{{AFX_DATA(COptionsDeltaAlarm)
	enum { IDD = IDD_OPTIONS_DELTAALARM };
	CComboBox	m_cmbDataSourceNames;
	BOOL	m_BEnable;
	double	m_dSetpoint;
	int		m_iInterval;
	double	m_dDuration;
	BOOL	m_BWAV;
	CString	m_txtWAVFile;
	double	m_dWAVRepeatInterval;
	int		m_iDelay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDeltaAlarm)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDeltaAlarm)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangeDaDatasourcenames();
	afx_msg void OnEnableda();
	afx_msg void OnUpdateDaSetpoint();
	afx_msg void OnUpdateDaInterval();
	afx_msg void OnUpdateDaDuration();
	afx_msg void OnDaWav();
	afx_msg void OnUpdateDaWavfile();
	afx_msg void OnUpdateDaWavrepeatinterval();
	afx_msg void OnDaChoosewav();
	afx_msg void OnDaPlaywav();
	afx_msg void OnUpdateDaDelay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDELTAALARM_H__B30223C1_E571_4494_9623_5DEB34388F1A__INCLUDED_)
