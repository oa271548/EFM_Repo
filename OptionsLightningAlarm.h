#if !defined(AFX_OPTIONSLIGHTNINGALARM_H__9B1D4CAB_6205_4D6A_B131_BA4C66E56918__INCLUDED_)
#define AFX_OPTIONSLIGHTNINGALARM_H__9B1D4CAB_6205_4D6A_B131_BA4C66E56918__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsLightningAlarm.h : header file
//
#include "Configuration.h"
/////////////////////////////////////////////////////////////////////////////
// COptionsLightningAlarm dialog

class COptionsLightningAlarm : public CDialog
{
// Construction
public:
	void CoincidenceSetDetectionModes();
	void CoincidenceFindAvailableComPorts();
	CDialog * m_pMainWnd;
	bool SelectWAVFile(CString * wavPath);
	BOOL	m_lcBEnable[MAX_NUMOFDATASOURCES];
	BOOL	m_lcBShowChartLightningDistance[MAX_NUMOFDATASOURCES];
	double	m_lcdDuration[MAX_NUMOFDATASOURCES];
	double	m_lcdWAVRepeatInterval[MAX_NUMOFDATASOURCES];
	BOOL	m_lcBDistanceEnunciation[MAX_NUMOFDATASOURCES];
	BOOL	m_lcBOnActivationWAV[MAX_NUMOFDATASOURCES];
	BOOL	m_lcBOnlyAnnounceCloser[MAX_NUMOFDATASOURCES];
	BOOL	m_lcBDistanceWAV[MAX_NUMOFDATASOURCES];
	BOOL	m_lcBRepeatWAV[MAX_NUMOFDATASOURCES];
	BOOL	m_lcBAllClearWAV[MAX_NUMOFDATASOURCES];
	CString	m_lctxtOnActivationWAVFile[MAX_NUMOFDATASOURCES];
	CString	m_lctxtDistanceWAVFiles[MAX_NUMOFDATASOURCES];
	CString	m_lctxtRepeatWAVFile[MAX_NUMOFDATASOURCES];
	CString	m_lctxtAllClearWAVFile[MAX_NUMOFDATASOURCES];
	CString	m_lctxtRangeRings[MAX_NUMOFDATASOURCES];
	int		m_lciRingRangeUnit[MAX_NUMOFDATASOURCES];
	CString	m_lctxtLACDMode[MAX_NUMOFDATASOURCES];
	CString	m_lctxtLACDComPort[MAX_NUMOFDATASOURCES];
	CBitmap m_bmpFolder;
	CBitmap m_bmpPlay;
	CBitmap m_bmpStop;

	int m_iCurrentSelectedDataSource;
	int m_iNumberOfDataSources;
	COptionsLightningAlarm(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsLightningAlarm)
	enum { IDD = IDD_OPTIONS_LIGHTNINGALARM };
	CComboBox	m_cmbCoincidenceComPort;
	CComboBox	m_cmbCoincidenceDetection;
	CComboBox	m_cmbDataSourceNames;
	BOOL	m_BEnable;
	double	m_dDuration;
	double	m_dWAVRepeatInterval;
	BOOL	m_BOnActivationWAV;
	BOOL	m_BDistanceWAV;
	BOOL	m_BRepeatWAV;
	BOOL	m_BAllClearWAV;
	CString	m_txtOnActivationWAVFile;
	CString	m_txtDistanceWAVFiles;
	CString	m_txtRepeatWAVFile;
	CString	m_txtAllClearWAVFile;
	CString	m_txtRangeRings;
	int		m_iRingRangeUnit;
	BOOL	m_BOnlyAnnounceCloser;
	BOOL	m_BShowChartLightningDistance{FALSE};
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsLightningAlarm)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsLightningAlarm)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangeLaDatasourcenames();
	afx_msg void OnEnablela();
	afx_msg void OnBnClickedShowChartLightningDistance();
	afx_msg void OnUpdateLaDuration();
	afx_msg void OnUpdateLaWavrepeatinterval();
	afx_msg void OnLaOnactivationwav();
	afx_msg void OnUpdateLaOnactivationwavfile();
	afx_msg void OnLaDistancewav();
	afx_msg void OnUpdateLaDistancewavfiles();
	afx_msg void OnLaRepeatwav();
	afx_msg void OnUpdateLaRepeatwavfile();
	afx_msg void OnLaAllclearwav();
	afx_msg void OnUpdateLaAllclearwavfile();
	afx_msg void OnLaUnit();
	afx_msg void OnLaUnitKm();
	afx_msg void OnLaringsAdd();
	afx_msg void OnLaringsRemove();
	afx_msg void OnChoosewavOnactivation();
	afx_msg void OnPlaywavOnactivation();
	afx_msg void OnChoosewavDistance();
	afx_msg void OnPlaywavDistance();
	afx_msg void OnChoosewavRepeat();
	afx_msg void OnPlaywavRepeat();
	afx_msg void OnChoosewavAllclear();
	afx_msg void OnPlaywavAllclear();
	afx_msg void OnSelchangeCoincidenceDetection();
	afx_msg void OnSelchangeCoincidenceComport();
	afx_msg void OnOnlyannouncecloser();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSLIGHTNINGALARM_H__9B1D4CAB_6205_4D6A_B131_BA4C66E56918__INCLUDED_)
