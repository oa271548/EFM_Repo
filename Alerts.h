// Alerts.h: interface for the CAlerts class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALERTS_H__BDE5C63A_B04B_4888_B3F2_24E33B9DCC75__INCLUDED_)
#define AFX_ALERTS_H__BDE5C63A_B04B_4888_B3F2_24E33B9DCC75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WINSOCK2API_  // so that winsock2 is not included as well in CSmtp.h
#include "CSmtp.h"   // for sending email
#include "HTML.h"   // for ClickATell SMS gateway
#include "Gsm.h" // GSM modem on COM port
#include "Configuration.h"
#include <afxmt.h> // for CCriticalSection


struct tMESSAGE
{
	int index = 0; // which EFM (0-3)
	CString text; // text message to send in email or SMS
	CString recipient; // email address or phone number
	int retry_count = 0; // decremented to 0 if send fails
	bool show_results = false; // true only if we clicked "SEND TEST MESSAGE" button
};

enum tSMTP_STATE { IDLE, CONNECTING, SENDING, AWAITING_CONFIRMATION };

class CAlerts //: protected CSmtp, CHTML  
{
public:
	bool CheckConnectionSmtp();
	bool CheckConnectionSmsGsm();
	bool CheckConnectionSmsGateway();
	void KillThread();
	void StartThread();
	void SendAlerts(int DataSource, CString txt);
	void SendSmtpAlerts(int DataSource, CString txt, bool show_results);
	void SendSmsGsmAlerts(int DataSource, CString txt, bool show_results);
	void SendSmsGatewayAlerts(int DataSource, CString txt, bool show_results);
	CAlerts();
	virtual ~CAlerts();
	BOOL run_thread = FALSE;
	void ProcessSmtp();
	void ProcessSmsGsm();
	void ProcessSmsGateway();

private:
	CHTML html;
	//CSmtp smtp;
	CGsm  gsm;
	CCriticalSection m_csSmtpList;
	CCriticalSection m_csSmsGatewayList;
	CCriticalSection m_csSmsGsmList;
	CList<tMESSAGE,tMESSAGE&> SmtpList;
	CList<tMESSAGE,tMESSAGE&> SmsGatewayList;
	CList<tMESSAGE,tMESSAGE&> SmsGsmList;
	void SendSmtp(int index, CString recipient, CString msg, bool show_results);
	void SendSmsGsm(int index, CString recipient, CString msg, bool show_results);
	void SendSmsGateway(int index, CString recipient, CString msg,  bool show_results);
	void * thread_info = nullptr;
};

#endif // !defined(AFX_ALERTS_H__BDE5C63A_B04B_4888_B3F2_24E33B9DCC75__INCLUDED_)
