// Configuration.cpp: implementation of the CConfiguration class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EFM.h"
#include "Configuration.h"
#include "EventLog.h"	


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CEventLog m_EventLog;   // global so Alerts can access 


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfiguration::CConfiguration()
{
	int ctr;
	CString txt;

	//
	// Initial Default Values
	//
	m_Reg.SetRootKey(REG_ROOT_LOCATION);	// Set the default HKEY location

	// Read the installation directory
	m_Reg.SetKey(REG_LOCATION, TRUE);		// SetKey: Read
	m_szInstallDir = m_Reg.ReadString("InstallDir",m_szInstallDir);
	// Get data directory
	m_szDataDir = m_Reg.ReadString("DataDir",m_szDataDir);
	
	//
	// General
	//	
	// Number of data sources
	m_iNumberOfDataSources = 1;
	// Set default data source names
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{	m_szDataSourceName[ctr].Format("Data Source %d",ctr+1);	}
	// Set default Data Source enabled property to OFF
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{	m_BDataSourceEnabled[ctr] = FALSE;	}
	// Set default data source points
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{	m_szDataSourcePoint[ctr].Format("Serial");	}
	// Set default com port
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{	m_szComPort[ctr].Format("COM1");	}
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{	m_iBaudRate[ctr] = 9600;	}
	// Set default network IP address
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{	m_txtNetworkAddress[ctr] = "localhost";	}
	// Set default network port
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{	m_iNetworkPort[ctr] = 3000;	}
	// Set default serial numbers
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{	m_txtEFA20SerialNumber[ctr] = ""; }

	// HFA/VHFA/LA/DA
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// HFA
		m_BEnableHFA[ctr] = TRUE;
		m_BShowColorLineHFA[ctr] = FALSE;
		m_dHFASetpoint[ctr] = 5.0;
		m_iHFADelay[ctr] = 0;	
		m_dHFADuration[ctr] = 2.0;
		m_BHFAWAV[ctr] = TRUE;
		m_szHFAWAVFile[ctr].Format("%sWAV\\HighElectricField.wav", m_szInstallDir);
		m_dHFAWAVRepeatInterval[ctr] = 180;
		// VHFA
		m_BEnableVHFA[ctr] = TRUE;
		m_BShowColorLineVHFA[ctr] = FALSE;
		m_dVHFASetpoint[ctr] = 7.0;
		m_iVHFADelay[ctr] = 0;
		m_dVHFADuration[ctr] = 2.0;
		m_BVHFAWAV[ctr] = TRUE;
		m_szVHFAWAVFile[ctr].Format("%sWAV\\VeryHighElectricField.wav", m_szInstallDir);
		m_dVHFAWAVRepeatInterval[ctr] = 180;
		// LA
		m_BEnableLA[ctr] = TRUE;
		m_BShowChartLightningDistance[ctr] = FALSE;
		m_dLADuration[ctr] = 15.0;
		m_dLAWAVRepeatInterval[ctr] = 3.0;
		m_BLAOnActivationEnable[ctr] = FALSE;
		m_BLAOnlyAnnounceCloser[ctr] = TRUE;
		m_txtLAOnActivationWAVFile[ctr].Format("%sWAV\\Lightning.wav", m_szInstallDir);
		m_BLADistanceEnable[ctr] = TRUE;
		m_txtLADistanceWAVFiles[ctr].Format("%sWAV\\", m_szInstallDir);
		m_BLARepeatEnable[ctr] = TRUE;
		m_txtLARepeatWAVFile[ctr].Format("%sWAV\\LightningAlarmActive.wav", m_szInstallDir);
		m_BLAAllClearEnable[ctr] = TRUE;
		m_txtLAAllClearWAVFile[ctr].Format("%sWAV\\LightningAlarmClear.wav", m_szInstallDir);
		m_szLARingRanges[ctr] = DEFAULT_RINGRANGES;
		m_iLAUnit[ctr] = 0;
		m_txtLACDMode[ctr] = txtCOINCIDENCEMODES[0];
		m_txtLACDComPort[ctr] = "COM1";
		// DA
		m_BEnableDA[ctr] = TRUE;
		m_dDASetpoint[ctr] = 5.0;
		m_iDAInterval[ctr] = 1;	
		m_iDADelay[ctr] = 1;
		m_dDADuration[ctr] = 2.0;
		m_BDAWAV[ctr] = TRUE;
		m_szDAWAVFile[ctr].Format("%sWAV\\DeltaAlarm.wav", m_szInstallDir);
		m_dDAWAVRepeatInterval[ctr] = 180;
	}

	//
	// Data Logging
	//
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		m_BEnableDataLogging[ctr] = TRUE;
		m_BHighResolutionLogging[ctr] = FALSE;
		m_szDataLoggingPath[ctr] = m_szDataDir;
		if (m_szDataLoggingPath[ctr].Right(1) != '\\')
		{
			txt.Format("%s\\",m_szDataLoggingPath[ctr]);
			m_szDataLoggingPath[ctr] = txt;
		}
	}

	//
	// Data Sharing
	//
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		m_BDataSharingEnabled[ctr] = FALSE;
		m_iDataSharingPort[ctr] = 3000+ctr;
	}

	//
	// Alerts
	//
	// Events
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		bAlertsLAActive[ctr] = 1;
		bAlertsLAClear[ctr] = 1;
		bAlertsHFActive[ctr] = 1;
		bAlertsHFClear[ctr] = 1;
		bAlertsVHFActive[ctr] = 1;
		bAlertsVHFClear[ctr] = 1;
		bAlertsDAActive[ctr] = 1;
		bAlertsDAClear[ctr] = 1;
		bAlertsFaultsActive[ctr] = 1;
		bAlertsFaultsClear[ctr] = 1;
		bAlertsScheduled[ctr] = 1;
		iAlertsScheduledHour[ctr] = 12;
		iAlertsScheduledMins[ctr] = 0;
		iAlertsScheduledSecs[ctr] = 0;
	}
	// E-Mail
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		bSmtpEnabled[ctr] = 0;
	}
	txtSmtpServer = "";
	iSmtpServerPort = 25;
	iSmtpSecurityType = 0;
	txtSmtpUsername = "";
	txtSmtpPassword = "";
	txtSmtpSenderName = "";
	txtSmtpSenderEMail = "";
	iSmtpRetries = 2; // try 3 times to send alerts if send fail
	//
	// SMS - GSM
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		bSmsGsmEnabled[ctr] = 0;
	}
	txtSmsGsmComPort = "COM1";
	iSmsGsmBaudRate = 9600;
	txtSmsGsmModemInit = "";
	iSmsGsmRetries = 2; // try 3 times to send alerts if send fail
	// SMS - Gateway
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{
		bSmsGatewayEnabled[ctr] = 0;
	}

	if (iSmsGateWayProvider == 0)
	{
		txtSmsGatewayUrl = "http://api.clickatell.com/http/sendmsg?";
	}
	else if (iSmsGateWayProvider == 1)
	{
		txtSmsGatewayUrl = "https://api.twilio.com/2010-04-01/Accounts/";
	}
	txtSmsGatewayUsername = "username";
	txtSmsGatewayPassword = "password";
	txtSmsGatewayApiId = "1234567";

	iSmsGatewayRetries = 2; // try 3 times to send alerts if send fail
	//
	// Recipients
	int iRcp = 0;
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		for (iRcp=0; iRcp<MAX_RECIPIENTS; iRcp++)
		{
			txtEmailRecipient[ctr][iRcp] = "";
			txtEmailRecipientName[ctr][iRcp] = "";
			bEmailRecipientEnabled[ctr][iRcp] = FALSE;
			txtSmsRecipient[ctr][iRcp] = "";
			txtSmsRecipientName[ctr][iRcp] = "";
			bSmsRecipientEnabled[ctr][iRcp] = FALSE;
		}
	}

	//
	// System Sounds
	//
	m_txtWAVStartup.Format("%sWAV\\Boltek_EFM-100.wav", m_szInstallDir);
	m_txtWAVRotorFault.Format("%sWAV\\RotorFault.wav", m_szInstallDir);
	m_txtWAVDataLostFault.Format("%sWAV\\DataLostFault.wav", m_szInstallDir);
	m_txtWAVSelfTestFault.Format("%sWAV\\SelfTestFault.wav", m_szInstallDir);


}

CConfiguration::~CConfiguration()
{

}

void CConfiguration::SaveConfiguration()
{
	int ctr;
	CString txt;

	m_Reg.SetRootKey(REG_ROOT_LOCATION);	// Set the default HKEY location
	
	//
	// General
	//
	m_Reg.SetKey(REG_GENERAL, TRUE);		// SetKey: Write
	m_Reg.WriteInt("NumberOfDataSources",m_iNumberOfDataSources);
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_GENERAL,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, TRUE);		// SetKey: Write
		m_Reg.WriteString("Data Source Name",m_szDataSourceName[ctr]);
		m_Reg.WriteInt("Data Source Enabled",m_BDataSourceEnabled[ctr]);
		m_Reg.WriteString("Data Source Point",m_szDataSourcePoint[ctr]);
		m_Reg.WriteString("Data Source Com Port",m_szComPort[ctr]);
		m_Reg.WriteString("Data Source Network Address",m_txtNetworkAddress[ctr]);
		m_Reg.WriteInt("Data Source Network Port",m_iNetworkPort[ctr]);
		m_Reg.WriteString("EFA20 Serial Number",m_txtEFA20SerialNumber[ctr]);
		m_Reg.WriteInt("Scale Knob Setting",m_iScaleKnobSetting[ctr]);
		m_Reg.WriteInt("Time Knob Setting",m_iTimeKnobSetting[ctr]);
	}

	// HFA
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_HFA,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, TRUE);		// SetKey: Write
		m_Reg.WriteInt("Enable HFA",(m_BEnableHFA[ctr]==TRUE)?1:0);
		m_Reg.WriteInt("HFA ShowColorLineOnChart", (m_BShowColorLineHFA[ctr] == TRUE) ? 1 : 0);
		m_Reg.WriteFloat("HFA Setpoint",m_dHFASetpoint[ctr]);
		m_Reg.WriteInt("HFA Delay",m_iHFADelay[ctr]);
		m_Reg.WriteFloat("HFA Duration",m_dHFADuration[ctr]);
		m_Reg.WriteInt("HFA WAV Enabled",(m_BHFAWAV[ctr]==TRUE)?1:0);
		m_Reg.WriteString("HFA WAV File",m_szHFAWAVFile[ctr]);
		m_Reg.WriteFloat("HFA WAV Repeat Interval",m_dHFAWAVRepeatInterval[ctr]);
	}
	// VHFA
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_VHFA,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, TRUE);		// SetKey: Write
		m_Reg.WriteInt("Enable VHFA",(m_BEnableVHFA[ctr]==TRUE)?1:0);
		m_Reg.WriteInt("VHFA ShowColorLineOnChart", (m_BShowColorLineVHFA[ctr] == TRUE) ? 1 : 0);
		m_Reg.WriteFloat("VHFA Setpoint",m_dVHFASetpoint[ctr]);
		m_Reg.WriteInt("VHFA Delay",m_iVHFADelay[ctr]);
		m_Reg.WriteFloat("VHFA Duration",m_dVHFADuration[ctr]);
		m_Reg.WriteInt("VHFA WAV Enabled",(m_BVHFAWAV[ctr]==TRUE)?1:0);
		m_Reg.WriteString("VHFA WAV File",m_szVHFAWAVFile[ctr]);
		m_Reg.WriteFloat("VHFA WAV Repeat Interval",m_dVHFAWAVRepeatInterval[ctr]);
	}
	// LA
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_LA,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, TRUE);		// SetKey: Write
		m_Reg.WriteInt("Enable LA",(m_BEnableLA[ctr]==TRUE)?1:0);
		m_Reg.WriteInt("Show Chart Lightning Distance", (m_BShowChartLightningDistance[ctr] == TRUE) ? 1 : 0);
		m_Reg.WriteFloat("LA Duration",m_dLADuration[ctr]);
		m_Reg.WriteFloat("LA WAV Repeat Interval",m_dLAWAVRepeatInterval[ctr]);
		m_Reg.WriteInt("OnActivation Enable",(m_BLAOnActivationEnable[ctr]==TRUE)?1:0);
		m_Reg.WriteInt("Only Announce Closer",(m_BLAOnlyAnnounceCloser[ctr]==TRUE)?1:0);
		m_Reg.WriteString("OnActivation WAV",m_txtLAOnActivationWAVFile[ctr]);
		m_Reg.WriteInt("Distance Enable",(m_BLADistanceEnable[ctr]==TRUE)?1:0);
		m_Reg.WriteString("Distance WAVs",m_txtLADistanceWAVFiles[ctr]);
		m_Reg.WriteInt("Repeat Enable",(m_BLARepeatEnable[ctr]==TRUE)?1:0);
		m_Reg.WriteString("Repeat WAV",m_txtLARepeatWAVFile[ctr]);
		m_Reg.WriteInt("AllClear Enable",(m_BLAAllClearEnable[ctr]==TRUE)?1:0);
		m_Reg.WriteString("AllClear WAV",m_txtLAAllClearWAVFile[ctr]);
		m_Reg.WriteString("Ring Ranges",m_szLARingRanges[ctr]);
		m_Reg.WriteInt("LA Unit",m_iLAUnit[ctr]);
		m_Reg.WriteString("CD Mode",m_txtLACDMode[ctr]);
		m_Reg.WriteString("CD Com Port",m_txtLACDComPort[ctr]);
	}
	// DA
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_DA,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, TRUE);		// SetKey: Write
		m_Reg.WriteInt("Enable DA",(m_BEnableDA[ctr]==TRUE)?1:0);
		m_Reg.WriteFloat("DA Setpoint",m_dDASetpoint[ctr]);
		m_Reg.WriteInt("DA Interval",m_iDAInterval[ctr]);
		m_Reg.WriteInt("DA Delay",m_iDADelay[ctr]);
		m_Reg.WriteFloat("DA Duration",m_dDADuration[ctr]);
		m_Reg.WriteInt("DA WAV Enabled",(m_BDAWAV[ctr]==TRUE)?1:0);
		m_Reg.WriteString("DA WAV File",m_szDAWAVFile[ctr]);
		m_Reg.WriteFloat("DA WAV Repeat Interval",m_dDAWAVRepeatInterval[ctr]);
	}

	// Data Logging
	m_Reg.SetKey(REG_DATALOGGING, TRUE);		// SetKey: Write
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_DATALOGGING,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, TRUE);		// SetKey: Write
		m_Reg.WriteInt("Enable Data Logging",m_BEnableDataLogging[ctr]);
		m_Reg.WriteString("Data Logging Path",m_szDataLoggingPath[ctr]);
		m_Reg.WriteInt("High Resolution Logging",m_BHighResolutionLogging[ctr]);
	}

	// Data Sharing
	m_Reg.SetKey(REG_DATASHARING, TRUE);		// SetKey: Write
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_DATASHARING,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, TRUE);		// SetKey: Read
		m_Reg.WriteInt("Data Sharing Enabled",m_BDataSharingEnabled[ctr]);
		m_Reg.WriteInt("Data Sharing Port",m_iDataSharingPort[ctr]);
	}

	//
	// Alerts
	//
	m_Reg.SetKey(REG_ALERTS, TRUE);		// SetKey: Write
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_ALERTS,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, TRUE);		// SetKey: Read
		m_Reg.WriteInt("Lightning Alarm Active",bAlertsLAActive[ctr]);
		m_Reg.WriteInt("Lightning Alarm Clear",bAlertsLAClear[ctr]);
		m_Reg.WriteInt("High Field Alarm Active",bAlertsHFActive[ctr]);
		m_Reg.WriteInt("High Field Alarm Clear",bAlertsHFClear[ctr]);
		m_Reg.WriteInt("Very High Field Alarm Active",bAlertsVHFActive[ctr]);
		m_Reg.WriteInt("Very High Field Alarm Clear",bAlertsVHFClear[ctr]);
		m_Reg.WriteInt("Delta Alarm Active",bAlertsDAActive[ctr]);
		m_Reg.WriteInt("Delta Alarm Clear",bAlertsDAClear[ctr]);
		m_Reg.WriteInt("Faults Active",bAlertsFaultsActive[ctr]);
		m_Reg.WriteInt("Faults Clear",bAlertsFaultsClear[ctr]);
		m_Reg.WriteInt("Scheduled",bAlertsScheduled[ctr]);
		m_Reg.WriteInt("ScheduledHour",iAlertsScheduledHour[ctr]);
		m_Reg.WriteInt("ScheduledMins",iAlertsScheduledMins[ctr]);
		m_Reg.WriteInt("ScheduledSecs",iAlertsScheduledSecs[ctr]);
	}
	// Email
	m_Reg.SetKey(REG_ALERTS_EMAIL, TRUE); // Set registry location: Write
	m_Reg.WriteString("SMTP Server", txtSmtpServer);
	m_Reg.WriteInt("SMTP Server Port", iSmtpServerPort);
	m_Reg.WriteInt("SMTP Security Type", iSmtpSecurityType);
	m_Reg.WriteString("Username", txtSmtpUsername);
	m_Reg.WriteString("Password", EncodePassword(txtSmtpPassword));
	m_Reg.WriteString("Sender Name", txtSmtpSenderName);
	m_Reg.WriteString("Sender EMail", txtSmtpSenderEMail);
	m_Reg.WriteInt("SMTP Retries", iSmtpRetries);
	int iRcp = 0;
	CString txtRecipKey;
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d", REG_ALERTS_EMAIL,ctr+1);
		m_Reg.SetKey(txt, TRUE); // Set registry location: Write
		for (iRcp=0; iRcp<MAX_RECIPIENTS; iRcp++)
		{
			m_Reg.WriteInt("SMTP Enabled", bSmtpEnabled[ctr]);
			txtRecipKey.Format("Recipient %d", iRcp);
			m_Reg.WriteString(txtRecipKey, txtEmailRecipient[ctr][iRcp]);
			txtRecipKey.Format("Recipient Name %d", iRcp);
			m_Reg.WriteString(txtRecipKey, txtEmailRecipientName[ctr][iRcp]);
			txtRecipKey.Format("Recipient Enabled %d", iRcp);
			m_Reg.WriteInt(txtRecipKey,bEmailRecipientEnabled[ctr][iRcp]);
		}
	}
	// SMS
	m_Reg.SetKey(REG_ALERTS_SMS, TRUE);	// Set registry location: Write
	// SMS GSM
	m_Reg.WriteString("GSM Com Port", txtSmsGsmComPort);
	m_Reg.WriteInt("GSM Baud Rate", iSmsGsmBaudRate);
	m_Reg.WriteString("GSM Modem Init", txtSmsGsmModemInit);
	m_Reg.WriteInt("GSM Retries", iSmsGsmRetries);
	// SMS Gateway
	m_Reg.WriteInt("Gateway Provider", iSmsGateWayProvider);
	m_Reg.WriteString("Gateway URL", txtSmsGatewayUrl);
	m_Reg.WriteString("Gateway Username", txtSmsGatewayUsername);
	m_Reg.WriteString("Gateway Password", EncodePassword(txtSmsGatewayPassword));
	m_Reg.WriteString("Gateway API ID", txtSmsGatewayApiId);
	m_Reg.WriteInt("Gateway Retries", iSmsGatewayRetries);

	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d", REG_ALERTS_SMS,ctr+1);
		m_Reg.SetKey(txt, TRUE); // Set registry location: Write
		for (iRcp=0; iRcp<MAX_RECIPIENTS; iRcp++)
		{
			m_Reg.WriteInt("GSM Enabled", bSmsGsmEnabled[ctr]);
			m_Reg.WriteInt("Gateway Enabled", bSmsGatewayEnabled[ctr]);
			txtRecipKey.Format("Recipient %d", iRcp);
			m_Reg.WriteString(txtRecipKey, txtSmsRecipient[ctr][iRcp]);
			txtRecipKey.Format("Recipient Name %d", iRcp);
			m_Reg.WriteString(txtRecipKey, txtSmsRecipientName[ctr][iRcp]);
			txtRecipKey.Format("Recipient Enabled %d", iRcp);
			m_Reg.WriteInt(txtRecipKey,bSmsRecipientEnabled[ctr][iRcp]);
		}
	}
	
	//
	// System Sounds
	//
	m_Reg.SetKey(REG_GENERAL, TRUE);		// SetKey: Read
	m_Reg.WriteString("WAV Startup", m_txtWAVStartup);
	m_Reg.WriteString("WAV Rotor Fault", m_txtWAVRotorFault);
	m_Reg.WriteString("WAV Data Lost Fault", m_txtWAVDataLostFault);
	m_Reg.WriteString("WAV Self Test Fault", m_txtWAVSelfTestFault);

	m_EventLog.LogEvent("Configuration Saved");

}

void CConfiguration::LoadConfiguration()
{
	int ctr;
	CString txt;

	m_Reg.SetRootKey(REG_ROOT_LOCATION);	// Set the default HKEY location

	// Get install directory
	m_Reg.SetKey(REG_LOCATION, TRUE);		// SetKey: Read
	m_szInstallDir = m_Reg.ReadString("InstallDir",m_szInstallDir);
	
	//
	// General
	//
	m_Reg.SetKey(REG_GENERAL, TRUE);		// SetKey: Read
	m_iNumberOfDataSources = m_Reg.ReadInt("NumberOfDataSources",m_iNumberOfDataSources);
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_GENERAL,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, FALSE);		// SetKey: Read

		m_szDataSourceName[ctr] = m_Reg.ReadString("Data Source Name",m_szDataSourceName[ctr]);
		m_BDataSourceEnabled[ctr] = m_Reg.ReadInt("Data Source Enabled",m_BDataSourceEnabled[ctr]);
		m_szDataSourcePoint[ctr] = m_Reg.ReadString("Data Source Point",m_szDataSourcePoint[ctr]);
		m_szComPort[ctr] = m_Reg.ReadString("Data Source Com Port",m_szComPort[ctr]);
		m_txtNetworkAddress[ctr] = m_Reg.ReadString("Data Source Network Address",m_txtNetworkAddress[ctr]);
		m_iNetworkPort[ctr] = m_Reg.ReadInt("Data Source Network Port",m_iNetworkPort[ctr]);
		m_txtEFA20SerialNumber[ctr] = m_Reg.ReadString("EFA20 Serial Number", m_txtEFA20SerialNumber[ctr]);
		m_iScaleKnobSetting[ctr] = m_Reg.ReadInt("Scale Knob Setting",m_iScaleKnobSetting[ctr]);
		m_iTimeKnobSetting[ctr] = m_Reg.ReadInt("Time Knob Setting",m_iTimeKnobSetting[ctr]);
	}

	// HFA
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_HFA,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, FALSE);		// SetKey: Read
		m_BEnableHFA[ctr] = m_Reg.ReadInt("Enable HFA",m_BEnableHFA[ctr]);
		if (m_BEnableHFA[ctr])
		{
			m_BShowColorLineHFA[ctr] = m_Reg.ReadInt("HFA ShowColorLineOnChart", TRUE);
		}
		else
		{
			m_BShowColorLineHFA[ctr] = m_Reg.ReadInt("HFA ShowColorLineOnChart", m_BShowColorLineHFA[ctr]);
		}
		m_dHFASetpoint[ctr] = m_Reg.ReadFloat("HFA Setpoint",m_dHFASetpoint[ctr]);
		m_iHFADelay[ctr] = m_Reg.ReadInt("HFA Delay",m_iHFADelay[ctr]);
		m_dHFADuration[ctr] = m_Reg.ReadFloat("HFA Duration",m_dHFADuration[ctr]);
		m_BHFAWAV[ctr] = m_Reg.ReadInt("HFA WAV Enabled",m_BHFAWAV[ctr]);
		m_szHFAWAVFile[ctr] = m_Reg.ReadString("HFA WAV File",m_szHFAWAVFile[ctr]);
		m_dHFAWAVRepeatInterval[ctr] = m_Reg.ReadFloat("HFA WAV Repeat Interval",m_dHFAWAVRepeatInterval[ctr]);
	}
	// VHFA
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_VHFA,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, FALSE);		// SetKey: Read
		m_BEnableVHFA[ctr] = m_Reg.ReadInt("Enable VHFA",m_BEnableVHFA[ctr]);

		if (m_BEnableVHFA[ctr])
		{
			m_BShowColorLineVHFA[ctr] = m_Reg.ReadInt("VHFA ShowColorLineOnChart", TRUE);
		}
		else
		{
			m_BShowColorLineVHFA[ctr] = m_Reg.ReadInt("VHFA ShowColorLineOnChart", m_BShowColorLineVHFA[ctr]);
		}

		m_dVHFASetpoint[ctr] = m_Reg.ReadFloat("VHFA Setpoint", m_dVHFASetpoint[ctr]);
		m_iVHFADelay[ctr] = m_Reg.ReadInt("VHFA Delay",m_iVHFADelay[ctr]);
		m_dVHFADuration[ctr] = m_Reg.ReadFloat("VHFA Duration",m_dVHFADuration[ctr]);
		m_BVHFAWAV[ctr] = m_Reg.ReadInt("VHFA WAV Enabled",m_BVHFAWAV[ctr]);
		m_szVHFAWAVFile[ctr] = m_Reg.ReadString("VHFA WAV File",m_szVHFAWAVFile[ctr]);
		m_dVHFAWAVRepeatInterval[ctr] = m_Reg.ReadFloat("VHFA WAV Repeat Interval",m_dVHFAWAVRepeatInterval[ctr]);
	}
	// LA
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_LA,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, FALSE);		// SetKey: Read
		m_BEnableLA[ctr] = m_Reg.ReadInt("Enable LA",m_BEnableLA[ctr]);
		m_BShowChartLightningDistance[ctr] = m_Reg.ReadInt("Show Chart Lightning Distance", m_BShowChartLightningDistance[ctr]);
		m_dLADuration[ctr] = m_Reg.ReadFloat("LA Duration",m_dLADuration[ctr]);
		m_dLAWAVRepeatInterval[ctr] = m_Reg.ReadFloat("LA WAV Repeat Interval",m_dLAWAVRepeatInterval[ctr]);
		m_BLAOnlyAnnounceCloser[ctr] = m_Reg.ReadInt("Only Announce Closer",m_BLAOnlyAnnounceCloser[ctr]);
		m_BLAOnActivationEnable[ctr] = m_Reg.ReadInt("OnActivation Enable",m_BLAOnActivationEnable[ctr]);
		m_txtLAOnActivationWAVFile[ctr] = m_Reg.ReadString("OnActivation WAV",m_txtLAOnActivationWAVFile[ctr]);
		m_BLADistanceEnable[ctr] = m_Reg.ReadInt("Distance Enable",m_BLADistanceEnable[ctr]);
		m_txtLADistanceWAVFiles[ctr] = m_Reg.ReadString("Distance WAVs",m_txtLADistanceWAVFiles[ctr]);
		m_BLARepeatEnable[ctr] = m_Reg.ReadInt("Repeat Enable",m_BLARepeatEnable[ctr]);
		m_txtLARepeatWAVFile[ctr] = m_Reg.ReadString("Repeat WAV",m_txtLARepeatWAVFile[ctr]);
		m_BLAAllClearEnable[ctr] = m_Reg.ReadInt("AllClear Enable",m_BLAAllClearEnable[ctr]);
		m_txtLAAllClearWAVFile[ctr] = m_Reg.ReadString("AllClear WAV",m_txtLAAllClearWAVFile[ctr]);
		m_szLARingRanges[ctr] = m_Reg.ReadString("Ring Ranges",m_szLARingRanges[ctr]);
		m_iLAUnit[ctr] = m_Reg.ReadInt("LA Unit",m_iLAUnit[ctr]);
		m_txtLACDMode[ctr] = m_Reg.ReadString("CD Mode", m_txtLACDMode[ctr]);
		m_txtLACDComPort[ctr] = m_Reg.ReadString("CD Com Port", m_txtLACDComPort[ctr]);
	}
	// DA
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_DA,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, FALSE);		// SetKey: Read
		m_BEnableDA[ctr] = m_Reg.ReadInt("Enable DA",m_BEnableDA[ctr]);
		m_dDASetpoint[ctr] = m_Reg.ReadFloat("DA Setpoint",m_dDASetpoint[ctr]);
		m_iDAInterval[ctr] = m_Reg.ReadInt("DA Interval",m_iDAInterval[ctr]);
		m_iDADelay[ctr] = m_Reg.ReadInt("DA Delay",m_iDADelay[ctr]);
		m_dDADuration[ctr] = m_Reg.ReadFloat("DA Duration",m_dDADuration[ctr]);
		m_BDAWAV[ctr] = m_Reg.ReadInt("DA WAV Enabled",m_BDAWAV[ctr]);
		m_szDAWAVFile[ctr] = m_Reg.ReadString("DA WAV File",m_szDAWAVFile[ctr]);
		m_dDAWAVRepeatInterval[ctr] = m_Reg.ReadFloat("DA WAV Repeat Interval",m_dDAWAVRepeatInterval[ctr]);
	}

	// Data Logging
	m_Reg.SetKey(REG_DATALOGGING, TRUE);		// SetKey: Read
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_DATALOGGING,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, TRUE);		// SetKey: Read
		m_BEnableDataLogging[ctr] = m_Reg.ReadInt("Enable Data Logging",m_BEnableDataLogging[ctr]);
		m_szDataLoggingPath[ctr] = m_Reg.ReadString("Data Logging Path",m_szDataLoggingPath[ctr]);
		m_BHighResolutionLogging[ctr] = m_Reg.ReadInt("High Resolution Logging",m_BHighResolutionLogging[ctr]);
	}

	// Data Sharing
	m_Reg.SetKey(REG_DATASHARING, TRUE);		// SetKey: Read
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_DATASHARING,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, TRUE);		// SetKey: Read
		m_BDataSharingEnabled[ctr] = m_Reg.ReadInt("Data Sharing Enabled",m_BDataSharingEnabled[ctr]);
		m_iDataSharingPort[ctr] = m_Reg.ReadInt("Data Sharing Port",m_iDataSharingPort[ctr]);
	}

	//
	// Alerts
	//
	m_Reg.SetKey(REG_ALERTS, TRUE);		// SetKey: Read
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d",REG_ALERTS,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, TRUE);		// SetKey: Read
		bAlertsLAActive[ctr] = m_Reg.ReadInt("Lightning Alarm Active",bAlertsLAActive[ctr]);
		bAlertsLAClear[ctr] = m_Reg.ReadInt("Lightning Alarm Clear",bAlertsLAClear[ctr]);
		bAlertsHFActive[ctr] = m_Reg.ReadInt("High Field Alarm Active",bAlertsHFActive[ctr]);
		bAlertsHFClear[ctr] = m_Reg.ReadInt("High Field Alarm Clear",bAlertsHFClear[ctr]);
		bAlertsVHFActive[ctr] = m_Reg.ReadInt("Very High Field Alarm Active",bAlertsVHFActive[ctr]);
		bAlertsVHFClear[ctr] = m_Reg.ReadInt("Very High Field Alarm Clear",bAlertsVHFClear[ctr]);
		bAlertsDAActive[ctr] = m_Reg.ReadInt("Delta Alarm Active",bAlertsDAActive[ctr]);
		bAlertsDAClear[ctr] = m_Reg.ReadInt("Delta Alarm Clear",bAlertsDAClear[ctr]);
		bAlertsFaultsActive[ctr] = m_Reg.ReadInt("Faults Active",bAlertsFaultsActive[ctr]);
		bAlertsFaultsClear[ctr] = m_Reg.ReadInt("Faults Clear",bAlertsFaultsClear[ctr]);
		bAlertsScheduled[ctr] = m_Reg.ReadInt("Scheduled",bAlertsScheduled[ctr]);
		iAlertsScheduledHour[ctr] = m_Reg.ReadInt("ScheduledHour",iAlertsScheduledHour[ctr]);
		iAlertsScheduledMins[ctr] = m_Reg.ReadInt("ScheduledMins",iAlertsScheduledMins[ctr]);
		iAlertsScheduledSecs[ctr] = m_Reg.ReadInt("ScheduledSecs",iAlertsScheduledSecs[ctr]);
	}

	// Smtp E-Mail
	m_Reg.SetKey(REG_ALERTS_EMAIL, TRUE);		// SetKey: Write
	txtSmtpServer = m_Reg.ReadString("SMTP Server", txtSmtpServer);
	iSmtpServerPort = m_Reg.ReadInt("SMTP Server Port", iSmtpServerPort);
	iSmtpSecurityType = m_Reg.ReadInt("SMTP Security Type", iSmtpSecurityType);
	txtSmtpUsername = m_Reg.ReadString("Username", txtSmtpUsername);
	txtSmtpPassword = DecodePassword(m_Reg.ReadString("Password", txtSmtpPassword));
	txtSmtpSenderName = m_Reg.ReadString("Sender Name", txtSmtpSenderName);
	txtSmtpSenderEMail = m_Reg.ReadString("Sender EMail", txtSmtpSenderEMail);
	iSmtpRetries = m_Reg.ReadInt("SMTP Retries", iSmtpRetries);
	int iRcp = 0;
	CString txtRecipKey;
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\DataSource%d", REG_ALERTS_EMAIL,ctr+1);
		// Set the new registry location
		m_Reg.SetKey(txt, TRUE);		// SetKey: Write
		bSmtpEnabled[ctr] = m_Reg.ReadInt("SMTP Enabled", bSmtpEnabled[ctr]);

		for (iRcp=0; iRcp<MAX_RECIPIENTS; iRcp++)
		{
			txtRecipKey.Format("Recipient %d", iRcp);
			txtEmailRecipient[ctr][iRcp] = m_Reg.ReadString(txtRecipKey, txtEmailRecipient[ctr][iRcp]);
			txtRecipKey.Format("Recipient Name %d", iRcp);
			txtEmailRecipientName[ctr][iRcp] = m_Reg.ReadString(txtRecipKey, txtEmailRecipientName[ctr][iRcp]);
			txtRecipKey.Format("Recipient Enabled %d", iRcp);
			bEmailRecipientEnabled[ctr][iRcp] = m_Reg.ReadInt(txtRecipKey, bEmailRecipientEnabled[ctr][iRcp]);
		}
	}
	// SMS
	m_Reg.SetKey(REG_ALERTS_SMS, TRUE);		// SetKey: Write
	// SMS - GSM
	txtSmsGsmComPort = m_Reg.ReadString("GSM Com Port", txtSmsGsmComPort);
	iSmsGsmBaudRate = m_Reg.ReadInt("GSM Baud Rate", iSmsGsmBaudRate);
	txtSmsGsmModemInit = m_Reg.ReadString("GSM Modem Init", txtSmsGsmModemInit);
	iSmsGsmRetries = m_Reg.ReadInt("GSM Retries", iSmsGsmRetries);
	// SMS - Gateway
	iSmsGateWayProvider = m_Reg.ReadInt("Gateway Provider", iSmsGateWayProvider);
	txtSmsGatewayUrl = m_Reg.ReadString("Gateway URL", txtSmsGatewayUrl);
	txtSmsGatewayUsername = m_Reg.ReadString("Gateway Username", txtSmsGatewayUsername);
	txtSmsGatewayPassword = DecodePassword(m_Reg.ReadString("Gateway Password", txtSmsGatewayPassword));
	txtSmsGatewayApiId = m_Reg.ReadString("Gateway API ID", txtSmsGatewayApiId);
	iSmsGatewayRetries = m_Reg.ReadInt("Gateway Retries", iSmsGatewayRetries);

	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Construct the registry location
		txt.Format("%s\\SMS\\DataSource%d", REG_ALERTS,ctr+1);
		m_Reg.SetKey(txt, TRUE); // Set the new registry location: Write
		bSmsGsmEnabled[ctr] = m_Reg.ReadInt("GSM Enabled", bSmsGsmEnabled[ctr]);
		bSmsGatewayEnabled[ctr] = m_Reg.ReadInt("Gateway Enabled", bSmsGatewayEnabled[ctr]);
	
		for (iRcp=0; iRcp<MAX_RECIPIENTS; iRcp++)
		{
			txtRecipKey.Format("Recipient %d", iRcp);
			txtSmsRecipient[ctr][iRcp] = m_Reg.ReadString(txtRecipKey, txtSmsRecipient[ctr][iRcp]);
			txtRecipKey.Format("Recipient Name %d", iRcp);
			txtSmsRecipientName[ctr][iRcp] = m_Reg.ReadString(txtRecipKey, txtSmsRecipientName[ctr][iRcp]);
			txtRecipKey.Format("Recipient Enabled %d", iRcp);
			bSmsRecipientEnabled[ctr][iRcp] = m_Reg.ReadInt(txtRecipKey, bSmsRecipientEnabled[ctr][iRcp]);
		}
	}

	//
	// System Sounds
	//
	m_Reg.SetKey(REG_GENERAL, TRUE);		// SetKey: Read
	m_txtWAVStartup = m_Reg.ReadString("WAV Startup",m_txtWAVStartup);
	m_txtWAVRotorFault = m_Reg.ReadString("WAV Rotor Fault",m_txtWAVRotorFault);
	m_txtWAVDataLostFault = m_Reg.ReadString("WAV Data Lost Fault",m_txtWAVDataLostFault);
	m_txtWAVSelfTestFault = m_Reg.ReadString("WAV Self Test Fault",m_txtWAVSelfTestFault);

}

// We are using One-Time Pad algorithm (see Wikipedia)
CString CConfiguration::EncodePassword(CString& password)
{
	CString pad = PAD;
	CString result;

	char* presult   = result.GetBuffer(255);
	char* ppad      = pad.GetBuffer(255);
	int newchar;
	int length;

	length = password.GetLength();

	int pos;
	for (pos=0; pos < length; pos++)
	{
		// shift ASCII characters down to 0-94
		newchar = (password[pos]-' ') + (ppad[pos]-' '); 
		// check if we're now unprintable
		if (newchar > 94)
		{
			newchar -= 94;
		}
		presult[pos] = newchar+' '; // shift back up to ASCII
	}

	presult[pos] = 0; // null terminate

	return result;
}

CString CConfiguration::DecodePassword(CString& password)
{
	CString pad = PAD;
	CString result;

	char* presult   = result.GetBuffer(255);
	char* ppad      = pad.GetBuffer(255);
	int newchar;
	int length;

	length = password.GetLength();

	int pos;
	for (pos=0; pos < length; pos++)
	{
		// shift ASCII down to 0-94
		newchar = (password[pos]-' ') - (ppad[pos]-' '); 
		// check if we're now unprintable
		if (newchar < 0)
		{
			newchar += 94;
		}
		presult[pos] = newchar+' '; // shift back to ASCII
	}

	presult[pos] = 0; // null terminate

	return result;
}



