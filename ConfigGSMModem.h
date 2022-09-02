#if !defined(AFX_CONFIGGSMMODEM_H__A9C831B7_84F2_4C15_A5A7_E570C824B9A5__INCLUDED_)
#define AFX_CONFIGGSMMODEM_H__A9C831B7_84F2_4C15_A5A7_E570C824B9A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigGSMModem.h : header file
//

#include "SerialMFC.h"	// Added by ClassView


/////////////////////////////////////////////////////////////////////////////
// CConfigGSMModem dialog

class CConfigGSMModem : public CDialog
{
// Construction
public:
	CConfigGSMModem(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigGSMModem)
	enum { IDD = IDD_CONFIGUREGSMMODEM };
	CComboBox	m_cmbComPort;
	CString	m_txtBaudRate;
	CString	m_txtModemInit;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigGSMModem)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigGSMModem)
	afx_msg void OnSelchangeComport();
	afx_msg void OnAccept();
	afx_msg void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CSerialMFC m_serialCom;
	
	void ScanComPorts();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGGSMMODEM_H__A9C831B7_84F2_4C15_A5A7_E570C824B9A5__INCLUDED_)
