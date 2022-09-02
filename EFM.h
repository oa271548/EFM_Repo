// EFM.h : main header file for the EFM application
//

#if !defined(AFX_EFM_H__0F6C42C2_1332_4DD2_B11C_C36C29C3BB4D__INCLUDED_)
#define AFX_EFM_H__0F6C42C2_1332_4DD2_B11C_C36C29C3BB4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

// Set version number in VersionNo.h
// Set Copyright date manually in IDD_ABOUTBOX_BOLTEK 
// Set Copyright date manually in IDD_ABOUTBOX_BRIGHTEX

// Uncomment this line to enable 100kV range
//#define __100kV__

// Uncomment this line to enable Brightex branding
//#define __BRIGHTEX__

#include "resource.h"		// main symbols


//#define	TMR_ALARMCHECK_DELAY	3

//#define	TMR_STOP_HFA_0			4
//#define	TMR_STOP_HFA_1			5
//#define	TMR_STOP_HFA_2			6
//#define	TMR_STOP_HFA_3			7
//#define	TMR_STOP_VHFA_0			8
//#define	TMR_STOP_VHFA_1			9
//#define	TMR_STOP_VHFA_2			10
//#define	TMR_STOP_VHFA_3			11
//#define	TMR_STOP_LA_0			12
//#define	TMR_STOP_LA_1			13
//#define	TMR_STOP_LA_2			14
//#define	TMR_STOP_LA_3			15

#define	ALARM_VISUAL	1
#define	ALARM_AUDIO		2
#define ALARM_ENNUN		3

#define SIGNALLOST_TIMEOUT		30	// These are in 
#define	ROTORFAULT_INTERVAL		30	//   seconds
#define	SELFTESTFAULT_INTERVAL		30	//   seconds

#define COLOUR_DISABLED			RGB(128,128,128)
#define COLOUR_ENABLED			RGB(0,255,0)
#define COLOUR_RED				RGB(0, 0, 255)
#define COLOUR_YELLOW			RGB(0, 255, 255)

#define	SERIAL			0
#define	NETWORK			1
#define EFA20			2

#define	SETMASK_FIELDMILL1		0x01
#define	SETMASK_FIELDMILL2		0x02
#define	SETMASK_FIELDMILL3		0x04
#define	SETMASK_FIELDMILL4		0x08

#define	CLEARMASK_FIELDMILL1	0xFF - SETMASK_FIELDMILL1
#define	CLEARMASK_FIELDMILL2	0xFF - SETMASK_FIELDMILL2
#define	CLEARMASK_FIELDMILL3	0xFF - SETMASK_FIELDMILL3
#define	CLEARMASK_FIELDMILL4	0xFF - SETMASK_FIELDMILL4

#define TMR_ONESEC									0
#define TMR_ONEFIFTHSEC								1

#define MSG_STATUSBAR_HIDE							WM_USER+1
#define MSG_STATUSBAR_SHOW							WM_USER+2

#define MSG_NETWORKSOURCE_UNABLETOCONNECT			WM_USER+3
#define MSG_NETWORKSOURCE_SIGNALLOST				WM_USER+4

#define MSG_SERIAL_ERROR_UNABLETOSETCOMTIMEOUTS		WM_USER+5
#define MSG_SERIAL_ERROR_UNABLETOSETCOMSTATE		WM_USER+6
#define MSG_SERIAL_ERROR_UNABLETOGETCOMSTATE		WM_USER+7
#define MSG_SERIAL_ERROR_UNABLETOCONNECT			WM_USER+8

#define DEFAULT_RINGRANGES		"6, 12, 18, 24"
#define MILES					0
#define KILOMETERS				1
#define MILES_TXT				"miles"
#define KILOMETERS_TXT			"kilometers"

#define LIGHTNINGDETECTEDAT_INTERVAL	30	// seconds

#define NUM_EFMSENTENCESPERSECOND		20	// 20 sentences per second

///////////////////////////////////////////////////////////////////////////////
// Maximum Number of Connected Clients                                       //
///////////////////////////////////////////////////////////////////////////////
const int MAX_NETWORKCLIENTS = 16;

const int MAX_COINCIDENCEMODES = 4;
const CString txtCOINCIDENCEMODES[MAX_COINCIDENCEMODES] = { "None", "StormTracker", "LD-250", "LD-350" };

///////////////////////////////////////////////////////////////////////////////
// Error Definitions                                                         //
///////////////////////////////////////////////////////////////////////////////
#define WM_ERR_NETWORK_CONNECT			0
#define WM_ERR_NETWORK_DISCONNECT		1

/*CString ERRORMSG[] = {	"A successful WSAStartup call must occur before using this function.",
						"The network subsystem has failed.",
						"A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.",
						"The descriptor s is not a socket.",
						"The argp parameter is not a valid part of the user address space.",
						"There was a problem connecting to the specified host."
					};
*/						

///////////////////////////////////////////////////////////////////////////////
// EFA20 Vendor ID and Product ID                                            //
///////////////////////////////////////////////////////////////////////////////
const int EFA20_VID = 0x0403;
const int EFA20_PID = 0xF245;

struct tEFA20DEVICEINFO
{
	int iFlags = 0;
	int iType = 0;
	int iID = 0;
	int iLocID = 0;
	CString txtSerialNumber;
	CString txtDescription;
	void * ftHandle = nullptr;
};


/////////////////////////////////////////////////////////////////////////////
// CEFMApp:
// See EFM.cpp for the implementation of this class
//

class CEFMApp : public CWinApp
{
public:
	HANDLE hMutex;
	CEFMApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEFMApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CEFMApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFM_H__0F6C42C2_1332_4DD2_B11C_C36C29C3BB4D__INCLUDED_)
