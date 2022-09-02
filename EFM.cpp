// EFM.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "EFM.h"
#include "EFMDlg.h"
#include "Configuration.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CConfiguration config;

/////////////////////////////////////////////////////////////////////////////
// CEFMApp

BEGIN_MESSAGE_MAP(CEFMApp, CWinApp)
	//{{AFX_MSG_MAP(CEFMApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEFMApp construction

CEFMApp::CEFMApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEFMApp object

CEFMApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CEFMApp initialization

BOOL CEFMApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	 // Create mutex so we can detect if we're already running
	hMutex = ::CreateMutex(NULL, TRUE, "c9fd7e39-079d-471e-a67a-f1efee433193");
	switch(::GetLastError())
	{
		case ERROR_SUCCESS:
			// Mutex created successfully. There is no instances running
			break;
		case ERROR_ALREADY_EXISTS:
			// Mutex already exists so there is a running instance of our app.
			::MessageBox(NULL,"There is already a copy of the EFM software running.","Application Already Running",MB_OK|MB_ICONINFORMATION);
			return FALSE;
			break;
		default:
			// Failed to create mutex by unknown reason
			::MessageBox(NULL,"There was an unknown error encountered with the EFM mutex.","EFM Error",MB_OK|MB_ICONERROR);
			return FALSE;
			break;
	}

	CEFMDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CEFMApp::OnIdle(LONG lCount) 
{
	// TODO: Add your specialized code here and/or call the base class
		
	return CWinApp::OnIdle(lCount);
}
