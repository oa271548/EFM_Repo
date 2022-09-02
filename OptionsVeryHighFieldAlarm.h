#if !defined(AFX_OPTIONSVERYHIGHFIELDALARM_H__96A8053F_7DBB_4D2C_9C08_16903DCFA2E5__INCLUDED_)
#define AFX_OPTIONSVERYHIGHFIELDALARM_H__96A8053F_7DBB_4D2C_9C08_16903DCFA2E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsVeryHighFieldAlarm.h : header file
//
#include "Configuration.h"
/////////////////////////////////////////////////////////////////////////////
// COptionsVeryHighFieldAlarm dialog

class COptionsVeryHighFieldAlarm : public CDialog
{
// Construction
public:
	CDialog * m_pMainWnd;
	bool SelectWAVFile(CString * wavPath);
	BOOL	m_lcBEnable[MAX_NUMOFDATASOURCES];
	BOOL	m_lcBShowColorLine[MAX_NUMOFDATASOURCES];
	double	m_lcdSetpoint[MAX_NUMOFDATASOURCES];
	int		m_lciDelay[MAX_NUMOFDATASOURCES];
	double	m_lcdDuration[MAX_NUMOFDATASOURCES];
	BOOL	m_lcBWAV[MAX_NUMOFDATASOURCES];
	CString	m_lctxtWAVFile[MAX_NUMOFDATASOURCES];
	double	m_lcdWAVRepeatInterval[MAX_NUMOFDATASOURCES];
	CBitmap m_bmpFolder;
	CBitmap m_bmpPlay;
	CBitmap m_bmpStop;

	int m_iCurrentSelectedDataSource;
	int m_iNumberOfDataSources;
	COptionsVeryHighFieldAlarm(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsVeryHighFieldAlarm)
	enum { IDD = IDD_OPTIONS_VERYHIGHFIELDALARM };
	CComboBox	m_cmbDataSourceNames;
	BOOL	m_BEnable;
	BOOL	m_BShowColorLine;
	double	m_dSetpoint;
	int		m_iDelay;
	double	m_dDuration;
	BOOL	m_BWAV;
	CString	m_txtWAVFile;
	double	m_dWAVRepeatInterval;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsVeryHighFieldAlarm)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsVeryHighFieldAlarm)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangeVhfaDatasourcenames();
	afx_msg void OnEnablevhfa();
	afx_msg void OnUpdateVhfaSetpoint();
	afx_msg void OnUpdateVhfaDelay();
	afx_msg void OnUpdateVhfaDuration();
	afx_msg void OnVhfaWav();
	afx_msg void OnUpdateVhfaWavfile();
	afx_msg void OnUpdateVhfaWavrepeatinterval();
	afx_msg void OnVhfaChoosewav();
	afx_msg void OnVhfaPlaywav();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedEnablevhfasetpointongraph();
	void EnableControls();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSVERYHIGHFIELDALARM_H__96A8053F_7DBB_4D2C_9C08_16903DCFA2E5__INCLUDED_)
