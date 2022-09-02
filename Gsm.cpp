// Gsm.cpp: implementation of the CGsm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "AlertsDev.h"
#include "Gsm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGsm::CGsm()
{

}

CGsm::~CGsm()
{

}

BOOL CGsm::OpenPort(CString portname, int baudrate, CString& errormsg)
{
	if (serial.Open(portname) != ERROR_SUCCESS)
	{
		errormsg = "Open failed";
		ClosePort(); // just in case
		return false;	
	}

	CSerial::EBaudrate ebaud;

	switch (baudrate)
	{
	case 300:
		ebaud = CSerial::EBaudrate::EBaud300;
		break;
	case 600:
		ebaud = CSerial::EBaudrate::EBaud600;
		break;
	case 1200:
		ebaud = CSerial::EBaudrate::EBaud1200;
		break;
	case 2400:
		ebaud = CSerial::EBaudrate::EBaud2400;
		break;
	case 4800:
		ebaud = CSerial::EBaudrate::EBaud4800;
		break;
	case 9600:
		ebaud = CSerial::EBaudrate::EBaud9600;
		break;
	case 14400:
		ebaud = CSerial::EBaudrate::EBaud14400;
		break;
	case 19200:
		ebaud = CSerial::EBaudrate::EBaud19200;
		break;
	case 38400:
		ebaud = CSerial::EBaudrate::EBaud38400;
		break;
	case 56000:
		ebaud = CSerial::EBaudrate::EBaud56000;
		break;
	case 57600:
		ebaud = CSerial::EBaudrate::EBaud57600;
		break;
	case 115200:
		ebaud = CSerial::EBaudrate::EBaud115200;
		break;
	case 128000:
		ebaud = CSerial::EBaudrate::EBaud128000;
		break;
	case 256000:
		ebaud = CSerial::EBaudrate::EBaud256000;
		break;
	default:
		ClosePort(); // just in case
		return false;
	}

	if (serial.Setup(
			ebaud,
			CSerial::EDataBits::EData8,
			CSerial::EParity::EParNone,
			CSerial::EStopBits::EStop1) != ERROR_SUCCESS)
	{
		errormsg = "Set baud rate failed";
		ClosePort(); // just in case
		return false;	
	}

	if (serial.SetupHandshaking(CSerial::EHandshake::EHandshakeOff) != ERROR_SUCCESS)
	{
		errormsg = "Set handshaking failed";
		ClosePort(); // just in case
		return false;	
	}

	if (serial.SetRTS(true) != ERROR_SUCCESS)
	{
		errormsg = "Set RTS failed";
		ClosePort(); // just in case
		return false;	
	}

	if (serial.SetDTR(true) != ERROR_SUCCESS)
	{
		errormsg = "Set DTR failed";
		ClosePort(); // just in case
		return false;	
	}


	return true;
}

BOOL CGsm::ClosePort()
{
	return serial.Close();
}

BOOL CGsm::ExecCommand(CString command, int responsetimeout, CString& errormsg)
{
	DWORD byteswritten;
	CString commandCR; // append a <CR>

	commandCR.Format("%s\r", command);
	if (serial.Write(commandCR, &byteswritten, 0, responsetimeout) != ERROR_SUCCESS)
	{

		return false;
	}

	// ReadResponse() isn't waiting for a response so we need to wait here
	Sleep(responsetimeout); // wait

	CString response;
	response = ReadResponse(responsetimeout);

	// if   command     OK           or asking for SMS msg with ">"
	if ((response.Find("OK",0) < 0) && (response.Find(">",0) < 0))
	{
		if (response.GetLength() == 0)
		{
			errormsg.Format("command: \"%s\", No response from modem.", command);
		}
		else
		{
			errormsg.Format("command: \"%s\", response: \"%s\"", command, response);
		}
		return false;
	}


	return true;
}

BOOL CGsm::SendMessage(CString phonenum, CString message, CString& errormsg)
{
	if (!ExecCommand("AT", 300, errormsg))
	{
		return false;
	}

	if (!ExecCommand("AT+CMGF=1", 300, errormsg))
	{
		return false;
	}

	CString cmd;
	cmd.Format("AT+CMGS=\"%s\"", phonenum);
	if (!ExecCommand(cmd, 300, errormsg))
	{
		return false;
	}

	// send message, appened ^Z to terminate
	if (!ExecCommand(message + "\x1A\r", 5000, errormsg)) 
	{
		return false;
	}

	return true;
}



CString CGsm::ReadResponse(int timeout)
{
	char response[1000];
	unsigned long bytesread;

	if (serial.Read(response, 900, &bytesread, 0, timeout) != ERROR_SUCCESS)
	{
		return CString("");
	}

	response[bytesread] = 0; // null terminate
	return response;

}

