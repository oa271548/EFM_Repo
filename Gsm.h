// Gsm.h: interface for the CGsm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GSM_H__BE198B0E_0A12_4A2C_9262_7DC98719E24E__INCLUDED_)
#define AFX_GSM_H__BE198B0E_0A12_4A2C_9262_7DC98719E24E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "serial.h"

class CGsm  
{
public:
	BOOL SendMessage(CString phonenum, CString message, CString& errormsg);
	BOOL ExecCommand(CString command, int responsetimeout, CString& errormsg);
	BOOL ClosePort();
	BOOL OpenPort(CString portname, int baudrate, CString& errormsg);
	CGsm();
	virtual ~CGsm();

private:
	CString ReadResponse(int timeout);
	CSerial serial;
};

#endif // !defined(AFX_GSM_H__BE198B0E_0A12_4A2C_9262_7DC98719E24E__INCLUDED_)
