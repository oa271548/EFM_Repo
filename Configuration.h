// Configuration.h: interface for the CConfiguration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGURATION_H__559F4A4B_76EE_4319_8FE2_D5890E04F707__INCLUDED_)
#define AFX_CONFIGURATION_H__559F4A4B_76EE_4319_8FE2_D5890E04F707__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Registry.h"

#define PAD "GeTfsnikI8BNmeQMlpUfsEsqUi7cklaLdFbCvYoqSG5Bsk6dd318flpfvbssafsrajehlhoieoncpoq" // for encrypting passwords


#define	MAX_NUMOFDATASOURCES	4

#define MAX_RECIPIENTS	16	// email or sms notifications

#define REG_ROOT_LOCATION		HKEY_CURRENT_USER
#define REG_LOCATION			"Software\\Boltek Corp\\EFM-100"
#define	REG_GENERAL				"Software\\Boltek Corp\\EFM-100\\General"
#define	REG_HFA					"Software\\Boltek Corp\\EFM-100\\HFA"
#define	REG_VHFA				"Software\\Boltek Corp\\EFM-100\\VHFA"
#define	REG_LA					"Software\\Boltek Corp\\EFM-100\\LA"
#define	REG_DA					"Software\\Boltek Corp\\EFM-100\\DA"
#define	REG_DATALOGGING			"Software\\Boltek Corp\\EFM-100\\Data Logging"
#define	REG_DATASHARING			"Software\\Boltek Corp\\EFM-100\\Data Sharing"
#define	REG_ALERTS				"Software\\Boltek Corp\\EFM-100\\Alerts"
#define	REG_ALERTS_SMS			"Software\\Boltek Corp\\EFM-100\\Alerts\\SMS"
#define	REG_ALERTS_EMAIL		"Software\\Boltek Corp\\EFM-100\\Alerts\\EMail"

class CConfiguration  
{
public:
	CRegistry m_Reg;
	void	LoadConfiguration();
	void	SaveConfiguration();
	int		m_iScaleKnobSetting[MAX_NUMOFDATASOURCES];
	int		m_iTimeKnobSetting[MAX_NUMOFDATASOURCES];
	CString	m_szInstallDir;
	CString	m_szDataDir;

	// General
	int		m_iNumberOfDataSources;
	CString m_szDataSourceName[MAX_NUMOFDATASOURCES];
	BOOL	m_BDataSourceEnabled[MAX_NUMOFDATASOURCES];
	CString m_szDataSourcePoint[MAX_NUMOFDATASOURCES];
	CString m_szComPort[MAX_NUMOFDATASOURCES];
	int		m_iBaudRate[MAX_NUMOFDATASOURCES];
	CString m_txtNetworkAddress[MAX_NUMOFDATASOURCES];
	int		m_iNetworkPort[MAX_NUMOFDATASOURCES];
	CString m_txtEFA20SerialNumber[MAX_NUMOFDATASOURCES];

	// High Field Alarm
	BOOL	m_BEnableHFA[MAX_NUMOFDATASOURCES];
	BOOL	m_BShowColorLineHFA[MAX_NUMOFDATASOURCES];
	double	m_dHFASetpoint[MAX_NUMOFDATASOURCES];
	int		m_iHFADelay[MAX_NUMOFDATASOURCES];	
	double	m_dHFADuration[MAX_NUMOFDATASOURCES];
	BOOL	m_BHFAWAV[MAX_NUMOFDATASOURCES];
	CString	m_szHFAWAVFile[MAX_NUMOFDATASOURCES];
	double	m_dHFAWAVRepeatInterval[MAX_NUMOFDATASOURCES];
	
	// Very High Field Alarm
	BOOL	m_BEnableVHFA[MAX_NUMOFDATASOURCES];
	BOOL	m_BShowColorLineVHFA[MAX_NUMOFDATASOURCES];
	double	m_dVHFASetpoint[MAX_NUMOFDATASOURCES];
	int		m_iVHFADelay[MAX_NUMOFDATASOURCES];
	double	m_dVHFADuration[MAX_NUMOFDATASOURCES];
	BOOL	m_BVHFAWAV[MAX_NUMOFDATASOURCES];
	CString	m_szVHFAWAVFile[MAX_NUMOFDATASOURCES];
	double	m_dVHFAWAVRepeatInterval[MAX_NUMOFDATASOURCES];

	// Lightning Alarm
	BOOL	m_BEnableLA[MAX_NUMOFDATASOURCES];
	double	m_dLADuration[MAX_NUMOFDATASOURCES];
	double	m_dLAWAVRepeatInterval[MAX_NUMOFDATASOURCES];
	BOOL	m_BLAOnlyAnnounceCloser[MAX_NUMOFDATASOURCES];
	BOOL	m_BLAOnActivationEnable[MAX_NUMOFDATASOURCES];
	CString	m_txtLAOnActivationWAVFile[MAX_NUMOFDATASOURCES];
	BOOL	m_BLADistanceEnable[MAX_NUMOFDATASOURCES];
	CString	m_txtLADistanceWAVFiles[MAX_NUMOFDATASOURCES];
	BOOL	m_BLARepeatEnable[MAX_NUMOFDATASOURCES];
	CString	m_txtLARepeatWAVFile[MAX_NUMOFDATASOURCES];
	BOOL	m_BLAAllClearEnable[MAX_NUMOFDATASOURCES];
	CString	m_txtLAAllClearWAVFile[MAX_NUMOFDATASOURCES];
	CString m_szLARingRanges[MAX_NUMOFDATASOURCES];
	int		m_iLAUnit[MAX_NUMOFDATASOURCES];
	CString	m_txtLACDMode[MAX_NUMOFDATASOURCES];
	CString	m_txtLACDComPort[MAX_NUMOFDATASOURCES];
	BOOL	m_BShowChartLightningDistance[MAX_NUMOFDATASOURCES];

	// Delta Alarm
	BOOL	m_BEnableDA[MAX_NUMOFDATASOURCES];
	double	m_dDASetpoint[MAX_NUMOFDATASOURCES];
	int		m_iDAInterval[MAX_NUMOFDATASOURCES];
	int		m_iDADelay[MAX_NUMOFDATASOURCES];
	double	m_dDADuration[MAX_NUMOFDATASOURCES];
	BOOL	m_BDAWAV[MAX_NUMOFDATASOURCES];
	CString	m_szDAWAVFile[MAX_NUMOFDATASOURCES];
	double	m_dDAWAVRepeatInterval[MAX_NUMOFDATASOURCES];

	// Data Logging
	BOOL	m_BEnableDataLogging[MAX_NUMOFDATASOURCES];
	CString	m_szDataLoggingPath[MAX_NUMOFDATASOURCES];
	BOOL	m_BHighResolutionLogging[MAX_NUMOFDATASOURCES];
	// Data Sharing
	BOOL	m_BDataSharingEnabled[MAX_NUMOFDATASOURCES];
	int		m_iDataSharingPort[MAX_NUMOFDATASOURCES];

	// Alerts
	//
	// Events
	BOOL    bAlertsLAActive[MAX_NUMOFDATASOURCES];
	BOOL    bAlertsLAClear[MAX_NUMOFDATASOURCES];
	BOOL    bAlertsHFActive[MAX_NUMOFDATASOURCES];
	BOOL    bAlertsHFClear[MAX_NUMOFDATASOURCES];
	BOOL    bAlertsVHFActive[MAX_NUMOFDATASOURCES];
	BOOL    bAlertsVHFClear[MAX_NUMOFDATASOURCES];
	BOOL    bAlertsDAActive[MAX_NUMOFDATASOURCES];
	BOOL    bAlertsDAClear[MAX_NUMOFDATASOURCES];
	BOOL    bAlertsFaultsActive[MAX_NUMOFDATASOURCES];
	BOOL    bAlertsFaultsClear[MAX_NUMOFDATASOURCES];
	BOOL	bAlertsScheduled  [MAX_NUMOFDATASOURCES];
	int		iAlertsScheduledHour[MAX_NUMOFDATASOURCES];
	int		iAlertsScheduledMins[MAX_NUMOFDATASOURCES];
	int		iAlertsScheduledSecs[MAX_NUMOFDATASOURCES];

	// E-Mail
	BOOL	bEmailRecipientEnabled[MAX_NUMOFDATASOURCES][MAX_RECIPIENTS]; 
	CString txtEmailRecipient[MAX_NUMOFDATASOURCES][MAX_RECIPIENTS]; // email addresses
	CString txtEmailRecipientName[MAX_NUMOFDATASOURCES][MAX_RECIPIENTS]; // comment person's name
	BOOL	bSmtpEnabled[MAX_NUMOFDATASOURCES];
	CString txtSmtpServer;
	int		iSmtpServerPort;
	int		iSmtpSecurityType;
	CString txtSmtpUsername;
	CString txtSmtpPassword;
	CString txtSmtpSenderName;
	CString txtSmtpSenderEMail;
	int		iSmtpRetries; // how many times should we retry to send emails and SMS's
	// SMS
	BOOL	bSmsRecipientEnabled[MAX_NUMOFDATASOURCES][MAX_RECIPIENTS]; 
	CString txtSmsRecipient[MAX_NUMOFDATASOURCES][MAX_RECIPIENTS]; // phone numbers
	CString txtSmsRecipientName[MAX_NUMOFDATASOURCES][MAX_RECIPIENTS]; // comment person's name  
	// SMS - GSM
	BOOL	bSmsGsmEnabled[MAX_NUMOFDATASOURCES];
	CString txtSmsGsmComPort;
	int		iSmsGsmBaudRate;
	CString txtSmsGsmModemInit;
	int		iSmsGsmRetries; // how many times should we retry to send emails and SMS's
	// SMS - Gateway
	BOOL	bSmsGatewayEnabled[MAX_NUMOFDATASOURCES];
	CString txtSmsGatewayUrl;
	CString txtSmsGatewayUsername;
	CString txtSmsGatewayPassword;
	CString txtSmsGatewayApiId;
	int		iSmsGateWayProvider;
	int		iSmsGatewayRetries; // how many times should we retry to send emails and SMS's

	
	// Non User Modifiable System Sounds
	//
	CString m_txtWAVStartup;
	CString m_txtWAVRotorFault;
	CString m_txtWAVDataLostFault;
	CString m_txtWAVSelfTestFault;

	CConfiguration();
	virtual ~CConfiguration();

private:
	CString EncodePassword(CString& password);
	CString DecodePassword(CString& password);

};

#endif // !defined(AFX_CONFIGURATION_H__559F4A4B_76EE_4319_8FE2_D5890E04F707__INCLUDED_)
