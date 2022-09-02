#if !defined(AFX_OPTIONSHIGHFIELDALARM_H__DFB7C8F2_5113_4E1A_98F5_A1A35AFFF090__INCLUDED_)
#define AFX_OPTIONSHIGHFIELDALARM_H__DFB7C8F2_5113_4E1A_98F5_A1A35AFFF090__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsHighFieldAlarm.h : header file
//
#include "Configuration.h"
/////////////////////////////////////////////////////////////////////////////
// COptionsHighFieldAlarm dialog

class COptionsHighFieldAlarm : public CDialog
{
// Construction
public:
	CDialog* m_pMainWnd;
	bool SelectWAVFile(CString * wavPath);
	void EnableControls();
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
	COptionsHighFieldAlarm(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsHighFieldAlarm)
	enum { IDD = IDD_OPTIONS_HIGHFIELDALARM };
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
	//{{AFX_VIRTUAL(COptionsHighFieldAlarm)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsHighFieldAlarm)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeHfaDatasourcenames();
	afx_msg void OnEnablehfa();
	afx_msg void OnUpdateHfaSetpoint();
	afx_msg void OnUpdateHfaDelay();
	afx_msg void OnUpdateHfaDuration();
	afx_msg void OnHfaWav();
	afx_msg void OnUpdateHfaWavfile();
	afx_msg void OnUpdateHfaWavrepeatinterval();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHfaChoosewav();
	afx_msg void OnHfaPlaywav();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedEnablesetpointongraph();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSHIGHFIELDALARM_H__DFB7C8F2_5113_4E1A_98F5_A1A35AFFF090__INCLUDED_)
