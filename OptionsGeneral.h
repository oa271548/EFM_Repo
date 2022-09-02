#if !defined(AFX_OPTIONSGENERAL_H__C960FDAC_AC51_4F32_85ED_56248281F3C0__INCLUDED_)
#define AFX_OPTIONSGENERAL_H__C960FDAC_AC51_4F32_85ED_56248281F3C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsGeneral.h : header file
//

#include "Configuration.h"
#include "SerialMFC.h"	// Added by ClassView
#include "EFMDlg.h"


/////////////////////////////////////////////////////////////////////////////
// COptionsGeneral dialog

class COptionsGeneral : public CDialog
{
// Construction
public:
	int m_iDataSourceSelected;
	CSerialMFC m_serialCom;
	void ScanComPorts();
	void EFA20_DetectConnectedDevices();
	size_t m_lciNumberOfDataSources;
	CString m_lcszDataSourceName[MAX_NUMOFDATASOURCES];
	BOOL m_lcBDataSourceEnabled[MAX_NUMOFDATASOURCES];
	CString m_lcszDataSourcePoint[MAX_NUMOFDATASOURCES];
	CString m_lcszComPort[MAX_NUMOFDATASOURCES];
	CString m_lctxtNetworkAddress[MAX_NUMOFDATASOURCES];
	int m_lciNetworkPort[MAX_NUMOFDATASOURCES];
	int m_lciBaudRate[MAX_NUMOFDATASOURCES];
	CString m_lcszEFA20SerialNumber[MAX_NUMOFDATASOURCES];		// EFA20 Serial Number

	COptionsGeneral(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsGeneral)
	enum { IDD = IDD_OPTIONS_GENERAL };
	CComboBox	m_cmbEFA20SerialNumber;
	CComboBox	m_cmbComPort;
	CComboBox	m_cmbDataSourcePoint;
	CComboBox	m_cmbDataSources;
	CSpinButtonCtrl	m_spinNumOfDataSourcesSelect;
	CString	m_szNumOfDataSources;
	CString	m_szMaxDataSources;
	CString	m_txtDataSourceName;
	int		m_iNetworkPort;
	int		m_iBaudRate;
	BOOL	m_BDataSourceEnabled;
	CString	m_txtNetworkAddress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsGeneral)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsGeneral)
	afx_msg void OnUpdateBaudRate();
	afx_msg void OnDeltaposNumofdatasources(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComport();
	afx_msg void OnSelchangeDatasourcepoint();
	afx_msg void OnUpdateDatasourcename();
	afx_msg void OnSelchangeDatasources();
	afx_msg void OnUpdateNetworkport();
	afx_msg void OnDatasourceenabled();
	afx_msg void OnSetfocusDatasourcename();
	afx_msg void OnUpdateNetworkaddress();
	afx_msg void OnSelchangeEfa20serialnumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSGENERAL_H__C960FDAC_AC51_4F32_85ED_56248281F3C0__INCLUDED_)
