// EFMDlg.h : header file
//
//{{AFX_INCLUDES()
#include "numled.h"
#include "strip.h"
#include "selector.h"
#include "toggle.h"
#include <tuple>
//}}AFX_INCLUDES

#if !defined(AFX_EFMDLG_H__8CB8224A_4E1A_45DD_9558_1E853186E7D5__INCLUDED_)
#define AFX_EFMDLG_H__8CB8224A_4E1A_45DD_9558_1E853186E7D5__INCLUDED_

#include "SerialMFC.h"	// Added by ClassView
#include "ftd2xx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CEFMDlg dialog
#include "Configuration.h"
#include "speaker_lib.h"
#include "Log.h"
#include <afxmt.h>  // for CCriticalSection
#include <afxtempl.h>
#include "Percent.h"
#include "ConnectedClient.h"
#include "alerts.h"
#include "chart.h"


#define INTEGRATION_VALUE	10		// Count 10 readings for every 1 displayed

#define BIT_ROTORFAULT				1
#define	BIT_SELFTESTFAULT			2
#define	BIT_ERROROPENINGCOMPORT		4
#define	BIT_ERRORCONFIGURINGCOMPORT	8
#define	BIT_CLIENTSCONNECTED		16
#define	BIT_REMOTECONNECTIONCLOSED	32
#define	BIT_CONNECTIONFAILURE		64

#define MUTESOUND_BORDEROFFSET		15

//Rotor Fault)",config.m_szDataSourceName[ctr]);
//Self-Test Fault)",config.m_szDataSourceName[ctr]);
//Error opening com port (%s),config.m_szDataSourceName[pInfo->GetID()],config.m_szComPort[pInfo->GetID()]);
//Error setting up com port (%s))",config.m_szDataSourceName[pInfo->GetID()],config.m_szComPort[pInfo->GetID()]);
//%d Clients Connected)",config.m_szDataSourceName[m_iID],pClients.GetCount());
//%s)",config.m_szDataSourceName[m_iID],CloseErrorMessage(pMsg->lParam));
//%s)",config.m_szDataSourceName[m_iID],ConnectionErrorMessage(pMsg->lParam));


struct tTHREADINFO
{
	/***************************************************************
	* Quarantine - to be deleted
	***************************************************************/
	CWinThread* pSC = nullptr;
	/**************************************************************/

	int iID = 0;
	CCriticalSection m_csID;
	void	SetID(int id) { CSingleLock lock(&m_csID, TRUE); iID = id; }
	int		GetID() { CSingleLock lock(&m_csID, TRUE); return iID; }

	bool bExitFlag = false;
	CCriticalSection m_csExitFlag;
	void ClrThreadExit() { CSingleLock lock(&m_csExitFlag, TRUE); bExitFlag = false; }
	void SetThreadExit() { CSingleLock lock(&m_csExitFlag, TRUE); bExitFlag = true; }
	bool GetThreadExit() { CSingleLock lock(&m_csExitFlag, TRUE); return bExitFlag; }
	//	HANDLE hExitHandle;
	//	CCriticalSection m_csExitFlag;
	//	void   SetThreadExit()	{ CSingleLock lock(&m_csExitFlag, TRUE); SetEvent(hExitHandle); }
	//	HANDLE GetThreadExit()	{ CSingleLock lock(&m_csExitFlag, TRUE); return hExitHandle; }

	bool bThreadAlive = false;
	CCriticalSection m_csThreadAlive;
	void	SetThreadAlive(bool bAlive) { CSingleLock lock(&m_csThreadAlive, TRUE); bThreadAlive = bAlive; }
	bool	GetThreadAlive() { CSingleLock lock(&m_csThreadAlive, TRUE); return bThreadAlive; }

	int iSignalLostCounter = 0;
	CCriticalSection m_csSignalLostCounter;
	void	SetSignalLostCounter(int slctr) { CSingleLock lock(&m_csSignalLostCounter, TRUE); iSignalLostCounter = slctr; }
	int		GetSignalLostCounter() { CSingleLock lock(&m_csSignalLostCounter, TRUE); return iSignalLostCounter; }
	void	IncSignalLostCounter() { CSingleLock lock(&m_csSignalLostCounter, TRUE); iSignalLostCounter++; }

	CStrip* pStrip = nullptr;
	CCriticalSection m_csStrip;

	void SetStrip(CStrip* pS)
	{
		//		TRACE("SetStrip() entry\n");
		CSingleLock lock(&m_csStrip, TRUE);
		pStrip = pS;
		//		TRACE("SetStrip() exit\n");
	}
	CStrip* GetStrip()
	{
		//		TRACE("GetStrip() entry\n");
		CSingleLock lock(&m_csStrip, TRUE);
		//		TRACE("GetStrip() exit\n");
		return pStrip;
	}

	EFMChart* pChart = nullptr;
	CCriticalSection m_csChart;
	void SetChart(EFMChart* pC)
	{
		CSingleLock lock(&m_csChart, TRUE);
		pChart = pC;
	}
	EFMChart* GetChart()
	{
		CSingleLock lock(&m_csChart, TRUE);
		return pChart;
	}

	CLightningDistanceChart* pLightningChart = nullptr;
	CCriticalSection m_csLightningChart;
	void SetLightningChart(CLightningDistanceChart* pC)
	{
		CSingleLock lock(&m_csLightningChart, TRUE);
		pLightningChart = pC;
	}
	CLightningDistanceChart* GetLightningChart()
	{
		CSingleLock lock(&m_csLightningChart, TRUE);
		return pLightningChart;
	}

	CNumLED* pNumLED = nullptr;
	CCriticalSection m_csNumLED;
	void		SetNumLED(CNumLED* pNL) { CSingleLock lock(&m_csNumLED, TRUE); pNumLED = pNL; }
	CNumLED* GetNumLED() { CSingleLock lock(&m_csNumLED, TRUE); return pNumLED; }

	int m_iLightningDistance = 0;
	CCriticalSection m_csLightningDistance;
	void	SetLightningDistance(int ld) { CSingleLock lock(&m_csLightningDistance, TRUE); m_iLightningDistance = ld; }
	int		GetLightningDistance() { CSingleLock lock(&m_csLightningDistance, TRUE); return m_iLightningDistance; }

	std::vector <double> m_vDeltaAlarmBuffer;
	CCriticalSection m_csDeltaAlarmBuffer;
	void	ClrDeltaAlarmBuffer() { CSingleLock lock(&m_csDeltaAlarmBuffer, TRUE); m_vDeltaAlarmBuffer.clear(); }



	int iIntegrationValue = 0;
	int* piGraphUpdatePeriod = nullptr;
	int* piGraphUpdateCounter = nullptr;
	double	dFieldMillReading = 0.;
	int iRotorStatus = 0;
	CLog logData;
	CDialog* pMyOwner = nullptr;

	bool m_bPlayLAOnActivationWAV = false;
	bool m_bPlayLADistanceWAV = false;
	bool m_bPlayLAAllClearWAV = false;
	bool m_bPlayDAWAV = false;
	bool m_bPlayVHFAWAV = false;
	bool m_bPlayHFAWAV = false;
	bool m_bLALastState = false;	// 
	bool m_bDALastState = false;
	bool m_bVHFALastState = false;	//  > For Alarm state transitions for event logging
	bool m_bHFALastState = false;	// 
	int m_iClosestAnnouncedDistance = 0;
	int m_iClosestAlertedDistance = 0;
	int m_iLACounter = 0;
	int m_iDACounter = 0;
	int m_iVHFACounter = 0;
	int m_iHFACounter = 0;
	int m_iDADelayCounter = 0;
	int m_iVHFADelayCounter = 0;
	int m_iHFADelayCounter = 0;
	int m_iLAWAVCounter = 0;
	int m_iDAWAVCounter = 0;
	int m_iVHFAWAVCounter = 0;
	int m_iHFAWAVCounter = 0;
	double tenths = 0.; // for displaying 10 readings per second BOB MOD
	double timelast = 0.; // for displaying 10 readings per second BOB MOD
	int iCheckLAStartup = 0; // counter so LA doesn't activate on startup BOB MOD
	double values[10][2] = {};// store efm readings between graph updates
	double lightningvalues[10][2] = {};// store lightnings readings between graph updates
	bool m_bRotorFault = false;
	bool m_bSelfTestFault = false;
	bool m_bSignalLost = false;
	bool m_bLoggedRotorFault = false;
	bool m_bLoggedSelfTestFault = false;

	bool m_bSoundOn = false;
	int m_iLightningDetectedAtCtr = 0;
	CString EfmFirmwareVersion; // EFM-200 reports f/w version in self-test, display in Config General



	///////////////////////////////////////////////////////////////////////////
	// Network
	///////////////////////////////////////////////////////////////////////////
	SOCKET mySocket;
	CCriticalSection m_csSocket;
	void	SetSocket(SOCKET socket) { CSingleLock lock(&m_csSocket, TRUE); mySocket = socket; }
	SOCKET	GetSocket() { CSingleLock lock(&m_csSocket, TRUE); return mySocket; }

	sockaddr_in connecttoAddr{};

	// Data Server
	CConnectedClient clientsConnected[MAX_NETWORKCLIENTS]{};
	CCriticalSection m_csClientsConnected;

	int ClientsCount()
	{
		CSingleLock lock(&m_csClientsConnected, TRUE);
		int iCount = 0;
		for (int iIdx = 0; iIdx < MAX_NETWORKCLIENTS; iIdx++)
		{
			if (clientsConnected[iIdx].GetIsAlive())
			{
				iCount++;
			}
		}
		return iCount;
	}

	int FindNextFreeClient()
	{
		CSingleLock lock(&m_csClientsConnected, TRUE);
		for (int iIdx = 0; iIdx < MAX_NETWORKCLIENTS; iIdx++)
		{
			if (!clientsConnected[iIdx].GetIsAlive())
			{
				return iIdx;
			}
		}
		return -1;	// No slots available
	}

	///////////////////////////////////////////////////////////////////////////
	// Data Source Point: Serial
	///////////////////////////////////////////////////////////////////////////
	HANDLE				m_hSerialThreadExit = nullptr;
	CCriticalSection	m_csSerialThreadExit;

	void   SetSerialThreadExit() { CSingleLock lock(&m_csSerialThreadExit, TRUE); SetEvent(m_hSerialThreadExit); }
	HANDLE GetSerialThreadExit() { CSingleLock lock(&m_csSerialThreadExit, TRUE); return m_hSerialThreadExit; }

	///////////////////////////////////////////////////////////////////////////
	// Data Source Point: EFA-20
	///////////////////////////////////////////////////////////////////////////
	FT_HANDLE			m_ftEFA20 = nullptr;				// EFA-20 Data Source Point Handle
	HANDLE				m_hEFA20ThreadExit = nullptr;
	CCriticalSection	m_csEFA20ThreadExit;
	CString				m_txtEFA20SerialNumber;

	void   SetEFA20ThreadExit() { CSingleLock lock(&m_csEFA20ThreadExit, TRUE); SetEvent(m_hEFA20ThreadExit); }
	HANDLE GetEFA20ThreadExit() { CSingleLock lock(&m_csEFA20ThreadExit, TRUE); return m_hEFA20ThreadExit; }


	// Coincidence Detection
	// General to any hardware
	CWinThread* m_pCDThread = nullptr;
	HANDLE m_hCDThreadExit = nullptr;				// Signal for the CD thread to exit
	CCriticalSection m_csCDThreadExit;
	bool m_bCDThreadRunning = false;
	CCriticalSection m_csCDThreadRunning;
	int m_iCDEventDetectionCounter = 0;
	bool m_bCDHardwareEventDetected = false;	// true  - detected an event, false - did not detect an event
	CCriticalSection m_csCDEventDetected;

	void   SetCDThreadExit() { CSingleLock lock(&m_csCDThreadExit, TRUE); SetEvent(m_hCDThreadExit); }
	HANDLE GetCDThreadExit() { CSingleLock lock(&m_csCDThreadExit, TRUE); return m_hCDThreadExit; }

	void SetCDThreadRunning() { CSingleLock lock(&m_csCDThreadRunning, TRUE); m_bCDThreadRunning = true; }
	void ClrCDThreadRunning() { CSingleLock lock(&m_csCDThreadRunning, TRUE); m_bCDThreadRunning = false; }
	bool GetCDThreadRunning() { CSingleLock lock(&m_csCDThreadRunning, TRUE); return m_bCDThreadRunning; }

	void SetCDEventDetected() { CSingleLock lock(&m_csCDEventDetected, TRUE); m_bCDHardwareEventDetected = true; }
	void ClrCDEventDetected() { CSingleLock lock(&m_csCDEventDetected, TRUE); m_bCDHardwareEventDetected = false; }
	bool GetCDEventDetected() { CSingleLock lock(&m_csCDEventDetected, TRUE); return m_bCDHardwareEventDetected; }

};

struct tDATACLIENTSERVER
{
	int iID = 0;
	tTHREADINFO* pThreadInfo = nullptr;
	char szClientIP[32] = {};
	SOCKET mySocket;
	bool bStillListening = false;
	CWinThread* pThread = nullptr;
	int iListenPort = 0;
	CStrip* pStrip = nullptr;
	CDialog* pMyOwner = nullptr;
};

struct tRRCOUNTDOWN
{
	int iMillID = 0;
	unsigned int uiBin = 0;
	int iCount = 0;
};

struct tFILETOPLAY
{
	int iID = 0;
	CString txtFile;
};

#include "lightningalarm.h"	// Added by ClassView
#include "ChartViewer.h"

class CEFMDlg : public CDialog
{
	// Construction
public:
	void SendScheduledAlerts(int iMillIndex);
	CTime nextScheduledAlertTime[MAX_NUMOFDATASOURCES];
	void SetNextScheduledAlertTime(int iFieldMill);
	int EFA20_DetectConnectedDevices();
	static bool ValidateEFMSentence(CString txtSentence);
	CString ResolveIPToHost(char* szIP);
	CString ResolveHostToIP(char* szHostName);
	//	static UINT CD_LD350(LPVOID pParam);
	//	static UINT CD_StormTracker(LPVOID pParam);
	void CoincidenceDetectionStop(int iFieldMillID);
	void CoincidenceDetectionStart(int iFieldMillID);
	void DebugWindowKill();
	void DebugWindowShow();
	CDlgDebugger* m_pDlgDebugger;
	void ClientsKillAll(int iDataSource);
	static UINT ThreadConnectedClient(LPVOID pParam);
	tFILETOPLAY GetOldestWAV();
	CCriticalSection m_csSoundHandlerWAVFileList;
	CArray <tFILETOPLAY, tFILETOPLAY> m_aSoundHandlerPlaylist;
	HANDLE m_hSoundHandlerThreadExit;
	CWinThread* m_pSoundHandlerThread;
	static UINT SoundHandlerThread(LPVOID pParam);
	void SoundHandlerStart();
	void SoundHandlerStop();
	tFILETOPLAY SoundHandlerGetOldestWAV();
	void PlayWAV(int iID, CString txtFilename);

	void RRAdd(int iMID, unsigned int uiDistance);
	CArray <tRRCOUNTDOWN, tRRCOUNTDOWN&> m_aRRCountdown[MAX_NUMOFDATASOURCES];
	void StopClientServer(int iIndex);
	void StartClientServer(int iIndex);
	CCriticalSection m_csChartChange[MAX_NUMOFDATASOURCES];
	void SetChartTitle(int iID, CString txtNewTitle);
	static UINT DataClientServer(LPVOID pParam);
	//static UINT FieldMillLWSx00(LPVOID pParam);
	CLightningAlarm m_laLightningAlarm[MAX_NUMOFDATASOURCES];
	static int ReceiveData(SOCKET sock, char* data, int len, int flags);

	CString CalculateChecksum(CString txtSentence);
	void SetStatusText(CString txtMsg);
	CString m_txtStatusBarText;
	void SetStatusControl(int iID, BOOL BDisplay);

	void ProcessWAVAlarms(int iMID);
	int m_iRotorFaultWAVCounter = 0;
	int m_iSelfTestFaultWAVCounter = 0;
	void ProcessAlarms(int iFID);
	static bool CheckForVeryHighFieldAlarm(double dReading, tTHREADINFO* pTI);
	static bool CheckForHighFieldAlarm(double dReading, tTHREADINFO* pTI);
	static bool CheckForDeltaAlarm(double dReading, tTHREADINFO* pTI);
	static int CheckForLightningAlarm(tTHREADINFO* pI, double time, int iIndex, double dReading);

	bool StopDAQThread(int idx, CString szStyle);
	bool StartDAQThread(int idx, CString szStyle);
	bool IsServerActive(int idx);
	bool IsAlarmsActive(int idx);
	bool m_bLAStatusCheck[MAX_NUMOFDATASOURCES]{};
	bool m_bVHFAStatusCheck[MAX_NUMOFDATASOURCES]{};
	bool m_bHFAStatusCheck[MAX_NUMOFDATASOURCES]{};
	bool m_bRotorFaultStatusCheck[MAX_NUMOFDATASOURCES]{};
	bool m_bSelfTestFaultStatusCheck[MAX_NUMOFDATASOURCES]{};
	bool m_bSignalLostFaultStatusCheck[MAX_NUMOFDATASOURCES]{};
	bool m_bDeltaAlarmCheck[MAX_NUMOFDATASOURCES]{};
	bool m_bLightningAlarmCheck[MAX_NUMOFDATASOURCES]{};
	bool m_bVeryHighFieldAlarmCheck[MAX_NUMOFDATASOURCES]{};
	bool m_bHighFieldAlarmCheck[MAX_NUMOFDATASOURCES]{};
	//	CPtrArray paNetworkClient;
	tDATACLIENTSERVER tDataClientServer[MAX_NUMOFDATASOURCES];
	CPtrArray tServer;	/* CWinThread * */
	double m_dLastReading[MAX_NUMOFDATASOURCES]{};	// Holds the previous reading
	double dAbsFMR = 0.;
	bool m_bSpeakerOpen = false;
	void ActivateDeltaAlarm(int iMillIndex, int iMode);
	void ActivateVeryHighFieldAlarm(int iMillIndex, int iMode);
	void ActivateHighFieldAlarm(int iMillIndex, int iMode);
	void ActivateLightningAlarm(int iMillIndex, int iMode, int iDistance);
	void DeactivateDeltaAlarm(int iMillIndex, int iMode);
	void DeactivateVeryHighFieldAlarm(int iMillIndex, int iMode);
	void DeactivateHighFieldAlarm(int iMillIndex, int iMode);
	void DeactivateLightningAlarm(int iMillIndex, int iMode);
	int m_iChartSelected = 0;
	int m_iChartUpdatePeriod[MAX_NUMOFDATASOURCES]{}; // how many seconds between graph updates (depends on graph scale)
	int m_iChartUpdateCounter[MAX_NUMOFDATASOURCES]{};
	SPEAKER_HANDLE m_hSPEAKER;	// CConfigureSound uses speaker
	int m_iBeepTimeOn = 0;
	int m_iBeepTimeOff = 0;
	int m_iBeepTimeOnTemp = 0;
	int m_iBeepTimeOffTemp = 0;
	int m_iBeepPitch = 0;
	int m_iBeepLevel = 0;
	void LayoutControls();
	void InitControls();
	CPercent m_pctDeltaVisual[MAX_NUMOFDATASOURCES];
	CPercent m_pctHighFieldVisual[MAX_NUMOFDATASOURCES];
	CPercent m_pctVeryHighFieldVisual[MAX_NUMOFDATASOURCES];
	CPercent m_pctLightningVisual[MAX_NUMOFDATASOURCES];
	CPercent m_pctStatusVisual[MAX_NUMOFDATASOURCES];
	void MakeChartsReflectKnobs();
	CBrush m_brushBackground;
	tTHREADINFO tThInfo[MAX_NUMOFDATASOURCES];
	CNumLED m_numledFieldMillValue[MAX_NUMOFDATASOURCES];
	int m_iChartX = 0;
	int m_iChartY = 0;
	int m_iNumLEDX = 0;
	int m_iNumLEDY = 0;
	CRect m_rectWindow;
	CToggle m_tglMuteSound[MAX_NUMOFDATASOURCES];
	//CStrip m_stripChart[MAX_NUMOFDATASOURCES];
	EFMChart m_chartViewer[MAX_NUMOFDATASOURCES];
	CLightningDistanceChart m_chartLightningDistanceViewer[MAX_NUMOFDATASOURCES];
	CWinThread* thRcvData[MAX_NUMOFDATASOURCES];
	static UINT FieldMillEFA20(LPVOID pParam);
	static UINT FieldMillNetwork(LPVOID pParam);
	static UINT FieldMillSerial(LPVOID pParam);
	static bool ProcessEFMSentence(CString sentence, tTHREADINFO* pI, CLog* lLog);
	static double CTime_to_Double(CTime t);
	static double CTime_to_ChartDouble();
	CEFMDlg(CWnd* pParent = nullptr);	// standard constructor

	// Accessors
	void accessor_OnClickFieldmill1() { OnClickFieldmill1(); }
	void accessor_OnClickFieldmill2() { OnClickFieldmill2(); }
	void accessor_OnClickFieldmill3() { OnClickFieldmill3(); }
	void accessor_OnClickFieldmill4() { OnClickFieldmill4(); }

	// Dialog Data
		//{{AFX_DATA(CEFMDlg)
	enum { IDD = IDD_EFM_DIALOG };
	CSelector	m_knobTimeSelector;
	CSelector	m_knobScaleSelector;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEFMDlg)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEFMDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnFileExit();
	afx_msg void OnHelpAbout();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnToolsOptions();
	afx_msg void OnShowConnectedClients();
	afx_msg void OnChangeScaleselector();
	afx_msg void OnChangeTimeselector();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClickFieldmill1();
	afx_msg void OnClickFieldmill2();
	afx_msg void OnClickFieldmill3();
	afx_msg void OnClickFieldmill4();
	afx_msg void OnClose();
	afx_msg void OnDblClickFieldmill1();
	afx_msg void OnDblClickFieldmill2();
	afx_msg void OnDblClickFieldmill3();
	afx_msg void OnDblClickFieldmill4();
	afx_msg void OnFileOpen();
	afx_msg void OnFileVieweventlog();
	afx_msg void OnClickMutesoundFieldmill1();
	afx_msg void OnClickMutesoundFieldmill2();
	afx_msg void OnClickMutesoundFieldmill3();
	afx_msg void OnClickMutesoundFieldmill4();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnViewPortChanged();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFMDLG_H__8CB8224A_4E1A_45DD_9558_1E853186E7D5__INCLUDED_)
