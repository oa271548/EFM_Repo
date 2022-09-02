// Alerts.cpp: implementation of the CAlerts class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Alerts.h"
#include "EventLog.h"
#include "curl/curl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CEventLog m_EventLog;
extern CConfiguration config;	// Make global for easy access

UINT __cdecl AlertsSmsGatewayThread(void* pParam);
UINT __cdecl AlertsSmsGsmThread(void* pParam);
UINT __cdecl AlertsSmtpThread(void* pParam);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlerts::CAlerts()
{

}

CAlerts::~CAlerts()
{

}

void CAlerts::SendSmtpAlerts(int index, CString txt, bool show_results)
{
	for (int cnt=0; cnt < MAX_RECIPIENTS; cnt++)
	{
		if (config.bEmailRecipientEnabled[index][cnt])
		{
			SendSmtp(index, config.txtEmailRecipient[index][cnt], txt, show_results);
		} // if recipient enabled
	} // for each recipient
}

void CAlerts::SendSmsGsmAlerts(int index, CString txt, bool show_results)
{
	// Send message to all configured users
	for (int cnt=0; cnt < MAX_RECIPIENTS; cnt++)
	{
		if (config.bSmsRecipientEnabled[index][cnt])
		{
			SendSmsGsm(index, config.txtSmsRecipient[index][cnt], txt, show_results);
		} // if recipient enabled
	} // for each recipient
}

void CAlerts::SendSmsGatewayAlerts(int index, CString txt, bool show_results)
{
	for (int cnt=0; cnt < MAX_RECIPIENTS; cnt++)
	{
		if (config.bSmsRecipientEnabled[index][cnt])
		{
			SendSmsGateway(index, config.txtSmsRecipient[index][cnt], txt, show_results);
		} // if recipient enabled
	} // for each recipient
}


//
//  index: which EFM (0-3)
//  txt: what to send in email or SMS
//
//  Put one message onto list for each recipient
//
void CAlerts::SendAlerts(int index, CString txt)
{
	// SMTP EMAIL
	if (config.bSmtpEnabled[index])
	{
		SendSmtpAlerts(index, txt, false);
	} // id SMTP enabled

	// SMS GSM MODEM
	if (config.bSmsGsmEnabled[index])
	{
		SendSmsGsmAlerts(index, txt, false);
	} // if GSM modem enabled
	
	// SMS GATEWAY
	if (config.bSmsGatewayEnabled[index])
	{
		SendSmsGatewayAlerts(index, txt, false);
	} // if SMS gateway enabled

} // SendAlerts()



//
// Add a new text message to the SMS Gateway send list
//
void CAlerts::SendSmsGateway(int index, CString recipient, CString txt, bool show_results)
{
	tMESSAGE new_msg;
	new_msg.index = index;
	new_msg.text = txt;
	new_msg.recipient = recipient;
	new_msg.retry_count = config.iSmsGatewayRetries;
	new_msg.show_results = show_results;

	CSingleLock single(&m_csSmsGatewayList, TRUE);
	SmsGatewayList.AddTail(new_msg);
	single.Unlock();
}


//
// Add a new text message to the GSM modem send list
//
void CAlerts::SendSmsGsm(int index, CString recipient, CString txt, bool show_results)
{
	tMESSAGE new_msg;
	new_msg.index = index;
	new_msg.text = txt;
	new_msg.recipient = recipient;
	new_msg.retry_count = config.iSmsGsmRetries;
	new_msg.show_results = show_results;

	CSingleLock single(&m_csSmsGsmList, TRUE);
	SmsGsmList.AddTail(new_msg);
	single.Unlock();
}

//
// Add a new email to the SMTP send list
//
void CAlerts::SendSmtp(int index, CString recipient, CString txt, bool show_results)
{
	tMESSAGE new_msg;
	new_msg.index = index;
	new_msg.text = txt;
	new_msg.recipient = recipient;
	new_msg.retry_count = config.iSmtpRetries;
	new_msg.show_results = show_results;

	CSingleLock single(&m_csSmtpList, TRUE);
	SmtpList.AddTail(new_msg);
	single.Unlock();
}

//
//  Send any waiting SMS Gateway messages
//
void CAlerts::ProcessSmsGateway()
{
	tMESSAGE waiting_msg;

	CSingleLock single(&m_csSmsGatewayList, TRUE);

	if (!SmsGatewayList.IsEmpty())
	{
		CString url;
		CString timestr;
		// Get message to send
		waiting_msg = SmsGatewayList.RemoveHead();
		single.Unlock();

		CTime current_time(CTime::GetCurrentTime());
		timestr.Format("%02d/%02d/%d  %02d:%02d:%02d  ",
			current_time.GetMonth(),
			current_time.GetDay(),
			current_time.GetYear(),
			current_time.GetHour(),
			current_time.GetMinute(),
			current_time.GetSecond());

		if (config.iSmsGateWayProvider == 0)
		{
			long bytesReturned;
			char* resultBuffer; // contains "ID: <long hex id>" if successful
			char* headerBuffer; // header not used

			// build url
			url.Format("%suser=%s&password=%s&api_id=%s&to=%s&text=%s%%0D%s: %%0D%s",
				config.txtSmsGatewayUrl,
				config.txtSmsGatewayUsername,
				config.txtSmsGatewayPassword,
				config.txtSmsGatewayApiId,
				waiting_msg.recipient,
				// begin message
				timestr,
				config.m_szDataSourceName[waiting_msg.index],
				waiting_msg.text);
			url.Replace(" ", "+"); // no spaces allowed in url
			url.Replace("\n", "%0D"); // no newlines allowed in url
			resultBuffer = html.readUrl2(url, bytesReturned, &headerBuffer);
			if ((resultBuffer != NULL) && (bytesReturned != 0))
			{
				// data returned from gateway
				CString logtxt;
				if (strncmp(resultBuffer, "ID:", 3) == 0)
				{
					// ID indicates SUCCESS
					logtxt.Format("Gateway SMS \"%s: %s\" SENT to %s",
						config.m_szDataSourceName[waiting_msg.index],
						waiting_msg.text,
						waiting_msg.recipient);
					m_EventLog.LogEvent(logtxt);

					if (waiting_msg.show_results)
					{
						MessageBox(NULL, logtxt, "Send Test SMS", MB_OK | MB_SYSTEMMODAL | MB_ICONINFORMATION);
					}

				} // success
				else
				{
					// FAILURE sending
					logtxt.Format("Gateway SMS \"%s: %s\" FAILED to %s",
						config.m_szDataSourceName[waiting_msg.index],
						waiting_msg.text,
						waiting_msg.recipient);
					m_EventLog.LogEvent(logtxt);

					if (waiting_msg.show_results)
					{
						MessageBox(NULL, logtxt, "Send Test SMS", MB_OK | MB_SYSTEMMODAL | MB_ICONEXCLAMATION);
						waiting_msg.retry_count = 0;
					}

					// log message from sms gateway
					//logtxt.Format("SMS Gateway reports: %s",
					//	resultBuffer);
					//
					//m_EventLog.LogEvent(logtxt);
					//
					//if (waiting_msg.show_results)
					//{
					//	MessageBox(NULL, logtxt, "Send Test SMS", MB_OK | MB_SYSTEMMODAL | MB_ICONEXCLAMATION);
					//	waiting_msg.retry_count = 0;
					//}

					if (waiting_msg.retry_count > 0)
					{
						waiting_msg.retry_count--;
						single.Lock();
						SmsGatewayList.AddTail(waiting_msg); // retry
						single.Unlock();
					} // retry
				} // failure
			}
			else
			{
				// FAILURE connecting
				CString logtxt;
				logtxt.Format("Gateway SMS \"%s: %s\" FAILED to %s",
					config.m_szDataSourceName[waiting_msg.index],
					waiting_msg.text,
					waiting_msg.recipient);
				m_EventLog.LogEvent(logtxt);
				logtxt.Format("Error connecting to SMS Gateway");
				m_EventLog.LogEvent(logtxt);

				if (waiting_msg.show_results)
				{
					MessageBox(NULL, logtxt, "Send Test SMS", MB_OK | MB_SYSTEMMODAL | MB_ICONEXCLAMATION);
					waiting_msg.retry_count = 0;
				}

				// retry the message 
				if (waiting_msg.retry_count > 0)
				{
					waiting_msg.retry_count--;
					single.Lock();
					SmsGatewayList.AddTail(waiting_msg); // retry
					single.Unlock();
				} // failure connecting
			} // failed
		}
		else if (config.iSmsGateWayProvider == 1)
		{
			// build url
			CURL* curl;
			CURLcode res;
			curl_global_init(CURL_GLOBAL_ALL);
			curl = curl_easy_init();
			if (curl)
			{
				CString body;
				body.Format("%s%%0D%s: %%0D%s",
					timestr,
					config.m_szDataSourceName[waiting_msg.index],
					waiting_msg.text);
				body.Replace(" ", "+"); // no spaces allowed in url
				body.Replace("\n", "%0D"); // no newlines allowed in url

				std::string URL_DIRECT = config.txtSmsGatewayUrl + config.txtSmsGatewayUsername + "/Messages";
				std::string sParameters = "To=" + waiting_msg.recipient + "&From=" + config.txtSmsGatewayApiId + "&Body=" + body;
				curl_easy_setopt(curl, CURLOPT_POST, 1);
				curl_easy_setopt(curl, CURLOPT_URL, URL_DIRECT.c_str());
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sParameters.c_str());
				curl_easy_setopt(curl, CURLOPT_USERNAME, config.txtSmsGatewayUsername);
				curl_easy_setopt(curl, CURLOPT_PASSWORD, config.txtSmsGatewayPassword);
				res = curl_easy_perform(curl);
				CString logtxt;
				if (res == CURLE_OK)
				{
					long http_code = 0;
					curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
					// data returned from gateway
					if (http_code == 200 || http_code == 201)
					{
						// ID indicates SUCCESS
						logtxt.Format("Gateway SMS \"%s: %s\" SENT to %s",
							config.m_szDataSourceName[waiting_msg.index],
							waiting_msg.text,
							waiting_msg.recipient);
						m_EventLog.LogEvent(logtxt);

						if (waiting_msg.show_results)
						{
							MessageBox(NULL, logtxt, "Send Test SMS", MB_OK | MB_SYSTEMMODAL | MB_ICONINFORMATION);
						}

					} // success
					else
					{
						// FAILURE sending
						logtxt.Format("Gateway SMS \"%s: %s\" FAILED to %s",
							config.m_szDataSourceName[waiting_msg.index],
							waiting_msg.text,
							waiting_msg.recipient);
						m_EventLog.LogEvent(logtxt);

						if (waiting_msg.show_results)
						{
							MessageBox(NULL, logtxt, "Send Test SMS", MB_OK | MB_SYSTEMMODAL | MB_ICONEXCLAMATION);
							waiting_msg.retry_count = 0;
						}

						//// log message from sms gateway
						//logtxt.Format("SMS Gateway reports: %s",
						//	curl_easy_strerror(res));
						//
						//m_EventLog.LogEvent(logtxt);
						//
						//if (waiting_msg.show_results)
						//{
						//	MessageBox(NULL, logtxt, "Send Test SMS", MB_OK | MB_SYSTEMMODAL | MB_ICONEXCLAMATION);
						//	waiting_msg.retry_count = 0;
						//}

						if (waiting_msg.retry_count > 0)
						{
							waiting_msg.retry_count--;
							single.Lock();
							SmsGatewayList.AddTail(waiting_msg); // retry
							single.Unlock();
						} // retry
					} // failure
				}
				else
				{
					// FAILURE connecting
					CString logtxt;
					logtxt.Format("Gateway SMS \"%s: %s\" FAILED to %s",
						config.m_szDataSourceName[waiting_msg.index],
						waiting_msg.text,
						waiting_msg.recipient);
					m_EventLog.LogEvent(logtxt);
					logtxt.Format("Error connecting to SMS Gateway");
					m_EventLog.LogEvent(logtxt);

					if (waiting_msg.show_results)
					{
						MessageBox(NULL, logtxt, "Send Test SMS", MB_OK | MB_SYSTEMMODAL | MB_ICONEXCLAMATION);
						waiting_msg.retry_count = 0;
					}

					// retry the message 
					if (waiting_msg.retry_count > 0)
					{
						waiting_msg.retry_count--;
						single.Lock();
						SmsGatewayList.AddTail(waiting_msg); // retry
						single.Unlock();
					} // failure connecting
				} // failed
				curl_easy_cleanup(curl);
			}
			curl_global_cleanup();
		}
	} // list not empty

	single.Unlock();

} // ProcessSmsGateway()


//
//  Send any waiting SMS GSM modem messages
//
void CAlerts::ProcessSmsGsm()
{
	tMESSAGE waiting_msg;

	CSingleLock single(&m_csSmsGsmList, TRUE);


	if (!SmsGsmList.IsEmpty())
	{
		CString msg_to_send;
		CString logtxt;
		CString timestr;
		CString errormsg;
		CString comport;

		// Get message to send
		waiting_msg = SmsGsmList.RemoveHead();
		single.Unlock();

		// Open GSM COM port
		comport.Format("\\\\.\\%s",config.txtSmsGsmComPort); // required for com ports > 8?
		if (!gsm.OpenPort(comport, config.iSmsGsmBaudRate, errormsg))
		{	
			// FAILED to open COM port
			gsm.ClosePort(); // just in case it opened but with an error

			logtxt.Format("GSM SMS \"%s: %s\" send FAILED to %s",
					config.m_szDataSourceName[waiting_msg.index],
					waiting_msg.text,
					waiting_msg.recipient);
			m_EventLog.LogEvent(logtxt);

			logtxt.Format("Error opening GSM COM port: %s, Error: %s", 
				config.txtSmsGsmComPort,
				errormsg);
			m_EventLog.LogEvent(logtxt);

			if (waiting_msg.show_results)
			{
				MessageBox(NULL, logtxt, "Send Test SMS", MB_OK|MB_SYSTEMMODAL|MB_ICONEXCLAMATION);
				waiting_msg.retry_count = 0;
			}

			// retry the message
			if (waiting_msg.retry_count > 0)
			{
				int temp = waiting_msg.retry_count;
				waiting_msg.retry_count--; 
				single.Lock();
				SmsGsmList.AddTail(waiting_msg); // retry
				single.Unlock();
			}

			return;
		}

		// Prepend timestamp to message
		CTime current_time(CTime::GetCurrentTime());
		timestr.Format("%02d/%02d/%d  %02d:%02d:%02d",
				current_time.GetMonth(),
				current_time.GetDay(),
				current_time.GetYear(),
				current_time.GetHour(),
				current_time.GetMinute(),
				current_time.GetSecond());

		msg_to_send.Format("%s\n%s:\n%s",
				timestr,
				config.m_szDataSourceName[waiting_msg.index],
				waiting_msg.text);

		if(gsm.SendMessage(
				waiting_msg.recipient,
				msg_to_send,
				errormsg))
		{
			// SUCCESS
			gsm.ClosePort(); 

			logtxt.Format("GSM SMS \"%s: %s\" SENT to %s",
							config.m_szDataSourceName[waiting_msg.index],
							waiting_msg.text, 
							waiting_msg.recipient);
							m_EventLog.LogEvent(logtxt);
			if (waiting_msg.show_results)
			{
				MessageBox(NULL, logtxt, "Send Test SMS" , MB_OK|MB_SYSTEMMODAL|MB_ICONINFORMATION);
			}
		} // send was successful
		else
		{
			// FAILED 
			gsm.ClosePort(); // just in case it opened but with an error

			errormsg.Replace('\n','|'); // sms command could have newlines
			errormsg.Replace('\r','|'); // sms command could have newlines
			logtxt.Format("GSM SMS \"%s: %s\" send FAILED to %s, Error: %s",
			config.m_szDataSourceName[waiting_msg.index],
			waiting_msg.text,
			waiting_msg.recipient,
			errormsg);
			m_EventLog.LogEvent(logtxt);

			if (waiting_msg.show_results)
			{
				MessageBox(NULL, logtxt, "Send Test SMS", MB_OK|MB_SYSTEMMODAL|MB_ICONEXCLAMATION);
				waiting_msg.retry_count = 0;
			}

			// retry the message
			if (waiting_msg.retry_count > 0)
			{
				waiting_msg.retry_count--; 
				single.Lock();
				SmsGsmList.AddTail(waiting_msg); // retry
				single.Unlock();
			}
		} // failed to send
	} // list not empty

	single.Unlock();

} // ProcessSmsGsm()

//
//  Send any waiting emails
//
void CAlerts::ProcessSmtp()
{
	tMESSAGE waiting_msg;

	CSingleLock single(&m_csSmtpList, TRUE);

	if (!SmtpList.IsEmpty())
	{
		// Get message to send
		waiting_msg = SmtpList.RemoveHead();
		single.Unlock();
		bool bError = false;
		CString timestr;
		CString str;

		CTime current_time(CTime::GetCurrentTime());
		timestr.Format("%02d/%02d/%d  %02d:%02d:%02d",
				current_time.GetMonth(),
				current_time.GetDay(),
				current_time.GetYear(),
				current_time.GetHour(),
				current_time.GetMinute(),
				current_time.GetSecond());
		
		// if this is a lightning alarm truncate the subject, no '\n' allowed
		CString subject;
		subject = waiting_msg.text;
		subject.Replace("\n", ". "); // no newlines allowed in subject


		try
		{
			CSmtp smtp;
			smtp.ClearMessage();
			smtp.SetSMTPServer(config.txtSmtpServer, config.iSmtpServerPort);
			smtp.SetSecurityType((enum SMTP_SECURITY_TYPE)config.iSmtpSecurityType);
			smtp.SetLogin(config.txtSmtpUsername);
			smtp.SetPassword(config.txtSmtpPassword);
			smtp.SetSenderName(config.txtSmtpSenderName);
			smtp.SetSenderMail(config.txtSmtpSenderEMail);
			smtp.SetReplyTo(config.txtSmtpSenderEMail);
			smtp.SetSubject(subject);
			smtp.SetXPriority(CSmptXPriority::XPRIORITY_HIGH);
			smtp.SetXMailer("Boltek EFM-100");
			smtp.AddRecipient(waiting_msg.recipient);
			smtp.AddMsgLine(timestr); // timestamp
			str.Format("%s:", config.m_szDataSourceName[waiting_msg.index]);
			smtp.AddMsgLine(str); // EFM name
			smtp.AddMsgLine(waiting_msg.text); // alarm type
			smtp.Send();
		} // try

		catch(ECSmtp e)
		{
			// FAILURE
			CString logtxt;
			logtxt.Format("Email \"%s: %s\" FAILED to %s Error: \"%s\"", 
				config.m_szDataSourceName[waiting_msg.index],
				waiting_msg.text,
				waiting_msg.recipient,					
				e.GetErrorText().c_str());
			m_EventLog.LogEvent(logtxt);
			if (waiting_msg.show_results)
			{
				MessageBox(NULL, logtxt, "Send Test Email", MB_OK|MB_SYSTEMMODAL|MB_ICONEXCLAMATION);
				waiting_msg.retry_count = 0;
			}

			// retry the message
			if (waiting_msg.retry_count > 0)
			{
				waiting_msg.retry_count--; 
				single.Lock();
				SmtpList.AddTail(waiting_msg); // retry
				single.Unlock();
			}
			bError = true;
		} // catch

		if(!bError)
		{
			// SUCCESS
			CString logtxt;
			logtxt.Format("Email \"%s: %s\" SENT to %s", 
				config.m_szDataSourceName[waiting_msg.index],
				waiting_msg.text,
				waiting_msg.recipient);
			m_EventLog.LogEvent(logtxt);
			if (waiting_msg.show_results)
			{
				MessageBox(NULL, logtxt, "Send Test Email" , MB_OK|MB_SYSTEMMODAL|MB_ICONINFORMATION);
			}

		}
		else
		{
		} // if (!bError)
	} // if list isn't empty

	single.Unlock();


} // ProcessSmtp

void CAlerts::StartThread()
{
	run_thread =  TRUE;
	AfxBeginThread(AlertsSmsGatewayThread, this);
	AfxBeginThread(AlertsSmsGsmThread, this);
	AfxBeginThread(AlertsSmtpThread, this);
}

void CAlerts::KillThread()
{
	run_thread = FALSE;

}

UINT __cdecl AlertsSmsGatewayThread(void * pParam)
{
	CAlerts * pD = (CAlerts *) pParam;
	while (pD->run_thread)
	{
		Sleep(1000);
		pD->ProcessSmsGateway();
	}
	AfxEndThread(0);
	return 0;
}
UINT __cdecl AlertsSmsGsmThread(void * pParam)
{
	CAlerts * pD = (CAlerts *) pParam;

	while (pD->run_thread)
	{
		Sleep(1000);
		pD->ProcessSmsGsm();
	}

	AfxEndThread(0);
	return 0;
}
UINT __cdecl AlertsSmtpThread(void * pParam)
{
	CAlerts * pD = (CAlerts *) pParam;
	while (pD->run_thread)
	{
		Sleep(1000);
		pD->ProcessSmtp();
	}

	AfxEndThread(0);
	return 0;
}

bool CAlerts::CheckConnectionSmsGateway()
{

	return true;
}

bool CAlerts::CheckConnectionSmsGsm()
{

	return true;
}

bool CAlerts::CheckConnectionSmtp()
{

	return true;
}
