#if !defined(AFX_OPTIONSALERTS_H__C52DEC69_2F77_4C0F_AEFC_B0CE26F38521__INCLUDED_)
#define AFX_OPTIONSALERTS_H__C52DEC69_2F77_4C0F_AEFC_B0CE26F38521__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsAlerts.h : header file
//
#include "Configuration.h"
#include "TabCheckListBox.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsAlerts dialog

class COptionsAlerts : public CDialog
{
// Construction
public:
	int GetDialogUnitNumber(); // for CTabCheckListBox font size determination
	void SaveToActiveConfig();
	int m_iCurrentSelectedDataSource;
	int m_lciNumberOfDataSources;
	CString m_laszDataSourceName[MAX_NUMOFDATASOURCES];

	BOOL	m_lcbAlertsLAActive[MAX_NUMOFDATASOURCES];
	BOOL	m_lcbAlertsLAClear[MAX_NUMOFDATASOURCES];
	BOOL	m_lcbAlertsHFActive[MAX_NUMOFDATASOURCES];
	BOOL	m_lcbAlertsHFClear[MAX_NUMOFDATASOURCES];
	BOOL	m_lcbAlertsVHFActive[MAX_NUMOFDATASOURCES];
	BOOL	m_lcbAlertsVHFClear[MAX_NUMOFDATASOURCES];
	BOOL	m_lcbAlertsDAActive[MAX_NUMOFDATASOURCES];
	BOOL	m_lcbAlertsDAClear[MAX_NUMOFDATASOURCES];
	BOOL	m_lcbAlertsFaultsActive[MAX_NUMOFDATASOURCES];
	BOOL	m_lcbAlertsFaultsClear[MAX_NUMOFDATASOURCES];
	BOOL	m_lcbAlertsScheduled[MAX_NUMOFDATASOURCES];
	CTime	m_lctAlertsScheduledTime[MAX_NUMOFDATASOURCES]; 

	// E-Mail
	BOOL	m_lcbEmailRecipientEnabled[MAX_NUMOFDATASOURCES][MAX_RECIPIENTS]; 
	CString m_lctxtEmailRecipient[MAX_NUMOFDATASOURCES][MAX_RECIPIENTS]; // email addresses
	CString m_lctxtEmailRecipientName[MAX_NUMOFDATASOURCES][MAX_RECIPIENTS]; // comment person's name
	BOOL	m_lcbEmailEnabled[MAX_NUMOFDATASOURCES];
	// SMS
	BOOL	m_lcbSmsRecipientEnabled[MAX_NUMOFDATASOURCES][MAX_RECIPIENTS]; 
	CString m_lctxtSmsRecipient[MAX_NUMOFDATASOURCES][MAX_RECIPIENTS]; // phone numbers
	CString m_lctxtSmsRecipientName[MAX_NUMOFDATASOURCES][MAX_RECIPIENTS]; // comment person's name  
	// SMS - GSM
	BOOL	m_lcbSmsGsmEnabled[MAX_NUMOFDATASOURCES];
	// SMS - Gateway
	BOOL	m_lcbSmsGatewayEnabled[MAX_NUMOFDATASOURCES];


	void SaveRecipientListboxes();
	void LoadRecipientListboxes();
	void SaveCheckboxes();
	void LoadCheckboxes();

	COptionsAlerts(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsAlerts)
	enum { IDD = IDD_OPTIONS_ALERTS };
	CComboBox		m_cmbDataSourceNames;
	CTabCheckListBox	m_ctlEmailList;
	CTabCheckListBox	m_ctlSmsList;
	BOOL	m_bAlertsDAActive;
	BOOL	m_bAlertsDAClear;
	BOOL	m_bAlertsSmtpEnabled;
	BOOL	m_bAlertsFaultsClear;
	BOOL	m_bAlertsHFActive;
	BOOL	m_bAlertsHFClear;
	BOOL	m_bAlertsLAActive;
	BOOL	m_bAlertsLAClear;
	BOOL	m_bAlertsSmsGatewayEnabled;
	BOOL	m_bAlertsSmsGsmEnabled;
	BOOL	m_bAlertsVHFActive;
	BOOL	m_bAlertsVHFClear;
	BOOL	m_bAlertsFaultsActive;
	CTime	m_tAlertsScheduledTime;
	BOOL	m_bAlertsScheduled;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsAlerts)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsAlerts)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEmailconfigure();
	afx_msg void OnEmailrecipients();
	afx_msg void OnSmsconfigure();
	afx_msg void OnSmsrecipients();
	afx_msg void OnSelChangeAlertsDatasourcenames();
	afx_msg void OnCheckChangeEmailList();
	afx_msg void OnDblclkEmaillist();
	afx_msg void OnDblclkSmslist();
	afx_msg void OnAddEmail();
	afx_msg void OnEditEmail();
	afx_msg void OnDeleteEmail();
	afx_msg void OnAddSms();
	afx_msg void OnEditSms();
	afx_msg void OnDeleteSms();
	afx_msg void OnEmailSendTestMessage();
	afx_msg void OnSmsGsmTestMessage();
	afx_msg void OnSmsGatewayTestMessage();
	afx_msg void OnSmsGatewayConfigure();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CBitmap m_bmpConfigure;
	CBitmap m_bmpPeople;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSALERTS_H__C52DEC69_2F77_4C0F_AEFC_B0CE26F38521__INCLUDED_)
