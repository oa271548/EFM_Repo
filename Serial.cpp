//	Serial.cpp - Implementation of the CSerial class
//
//	Copyright (C) 1999-2002 Ramon de Klein (R.de.Klein@iaf.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


//////////////////////////////////////////////////////////////////////
// Include the standard header files

#include "StdAfx.h"

//////////////////////////////////////////////////////////////////////
// Include module headerfile

#include "Serial.h"


//////////////////////////////////////////////////////////////////////
// Disable warning C4127: conditional expression is constant, which
// is generated when using the _RPTF and _ASSERTE macros.

#pragma warning(disable: 4127)


//////////////////////////////////////////////////////////////////////
// Enable debug memory manager

#ifdef _DEBUG

#ifdef THIS_FILE
#undef THIS_FILE
#endif

static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW

#endif


//////////////////////////////////////////////////////////////////////
// Code

CSerial::CSerial ()
	: m_lLastError(ERROR_SUCCESS)
	, m_hFile(0)
	, m_eEvent(EEvent::EEventNone)
	, m_dwEventMask(0)
	, m_hevtOverlapped(0)
{
}

CSerial::~CSerial ()
{
	// If the device is already closed,
	// then we don't need to do anything.
	if (m_hFile)
	{
		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::~CSerial - Serial port not closed\n");

		// Close implicitly
		Close();
	}
}

CSerial::EPort CSerial::CheckPort (LPCTSTR lpszDevice)
{
	// Try to open the device
	HANDLE hFile = ::CreateFile(lpszDevice, 
						   GENERIC_READ|GENERIC_WRITE, 
						   0, 
						   0, 
						   OPEN_EXISTING, 
						   FILE_FLAG_OVERLAPPED, 
						   0);

	// Check if we could open the device
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Display error
		switch (::GetLastError())
		{
		case ERROR_FILE_NOT_FOUND:
			// The specified COM-port does not exist
			return EPort::EPortNotAvailable;

		case ERROR_ACCESS_DENIED:
			// The specified COM-port is in use
			return EPort::EPortInUse;

		default:
			// Something else is wrong
			return EPort::EPortUnknownError;
		}
	}

	// Close handle
	::CloseHandle(hFile);

	// Port is available
	return EPort::EPortAvailable;
}

LONG CSerial::Open (LPCTSTR lpszDevice, DWORD dwInQueue, DWORD dwOutQueue)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the port isn't already opened
	if (m_hFile)
	{
		m_lLastError = ERROR_ALREADY_INITIALIZED;
		_RPTF0(_CRT_WARN,"CSerial::Open - Port already opened\n");
		return m_lLastError;
	}

	// Open the device
	m_hFile = ::CreateFile(lpszDevice,
						   GENERIC_READ|GENERIC_WRITE,
						   0,
						   0,
						   OPEN_EXISTING,
						   FILE_FLAG_OVERLAPPED,
						   0);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		// Reset file handle
		m_hFile = 0;

		// Display error
		m_lLastError = ::GetLastError();
		_RPTF0(_CRT_WARN, "CSerial::Open - Unable to open port\n");
		return m_lLastError;
	}

	// We cannot have an event handle yet
	_ASSERTE(m_hevtOverlapped == 0);

	// Create the event handle for internal overlapped operations (manual reset)
	m_hevtOverlapped = ::CreateEvent(0,true,false,NULL);
	if (m_hevtOverlapped == 0)
	{
		// Obtain the error information
		m_lLastError = ::GetLastError();
		_RPTF0(_CRT_WARN,"CSerial::Open - Unable to create event\n");

		// Close the port
		::CloseHandle(m_hFile);
		m_hFile = 0;

		// Return the error
		return m_lLastError;
	}

	// Setup the COM-port
	if (dwInQueue || dwOutQueue)
	{
		// Make sure the queue-sizes are reasonable sized. Win9X systems crash
		// if the input queue-size is zero. Both queues need to be at least
		// 16 bytes large.
		_ASSERTE(dwInQueue >= 16);
		_ASSERTE(dwOutQueue >= 16);

		if (!::SetupComm(m_hFile,dwInQueue,dwOutQueue))
		{
			// Display a warning
			long lLastError = ::GetLastError();
			_RPTF0(_CRT_WARN,"CSerial::Open - Unable to setup the COM-port\n");

			// Close the port
			Close();

			// Save last error from SetupComm
			m_lLastError = lLastError;
			return m_lLastError;	
		}
	}

	// Setup the default communication mask
	SetMask();

	// Non-blocking reads is default
	SetupReadTimeouts(EReadTimeout::EReadTimeoutNonblocking);

	// Setup the device for default settings
 	COMMCONFIG commConfig = {0};
	DWORD dwSize = sizeof(commConfig);
	commConfig.dwSize = dwSize;
	if (::GetDefaultCommConfig(lpszDevice,&commConfig,&dwSize))
	{
		// Set the default communication configuration
		if (!::SetCommConfig(m_hFile,&commConfig,dwSize))
		{
			// Display a warning
			_RPTF0(_CRT_WARN,"CSerial::Open - Unable to set default communication configuration.\n");
		}
	}
	else
	{
		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::Open - Unable to obtain default communication configuration.\n");
	}

	// Return successful
	return m_lLastError;
}

LONG CSerial::Close (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// If the device is already closed,
	// then we don't need to do anything.
	if (m_hFile == 0)
	{
		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::Close - Method called when device is not open\n");
		return m_lLastError;
	}

	// Free event handle
	::CloseHandle(m_hevtOverlapped);
	m_hevtOverlapped = 0;

	// Close COM port
	::CloseHandle(m_hFile);
	m_hFile = 0;

	// Return successful
	return m_lLastError;
}

LONG CSerial::Setup (EBaudrate eBaudrate, EDataBits eDataBits, EParity eParity, EStopBits eStopBits)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::Setup - Device is not opened\n");
		return m_lLastError;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::	GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::Setup - Unable to obtain DCB information\n");
		return m_lLastError;
	}

	// Set the new data
	dcb.BaudRate = DWORD(eBaudrate);
	dcb.ByteSize = BYTE(eDataBits);
	dcb.Parity   = BYTE(eParity);
	dcb.StopBits = BYTE(eStopBits);

	// Determine if parity is used
	dcb.fParity  = (eParity != EParity::EParNone);

	// Set the new DCB structure
	if (!::SetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::Setup - Unable to set DCB information\n");
		return m_lLastError;
	}

	// Return successful
	return m_lLastError;
}

LONG CSerial::SetEventChar (BYTE bEventChar, bool fAdjustMask)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::SetEventChar - Device is not opened\n");
		return m_lLastError;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::SetEventChar - Unable to obtain DCB information\n");
		return m_lLastError;
	}

	// Set the new event character
	dcb.EvtChar = char(bEventChar);

	// Adjust the event mask, to make sure the event will be received
	if (fAdjustMask)
	{
		// Enable 'receive event character' event.  Note that this
		// will generate an EEventNone if there is an asynchronous
		// WaitCommEvent pending.
		SetMask(GetEventMask() | static_cast<DWORD>(EEvent::EEventRcvEv));
	}

	// Set the new DCB structure
	if (!::SetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::SetEventChar - Unable to set DCB information\n");
		return m_lLastError;
	}

	// Return successful
	return m_lLastError;
}

LONG CSerial::SetMask (DWORD dwEventMask)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::SetMask - Device is not opened\n");
		return m_lLastError;
	}

	// Set the new mask. Note that this will generate an EEventNone
	// if there is an asynchronous WaitCommEvent pending.
	if (!::SetCommMask(m_hFile,dwEventMask))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::SetMask - Unable to set event mask\n");
		return m_lLastError;
	}

	// Save event mask and return successful
	m_dwEventMask = dwEventMask;
	return m_lLastError;
}

LONG CSerial::WaitEvent (LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Check if time-outs are supported
	CheckTimeout(dwTimeout);

	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::WaitEvent - Device is not opened\n");
		return m_lLastError;
	}

	// Wait for the event to happen
	OVERLAPPED ovInternal;
	if (lpOverlapped == 0)
	{
		// Setup our own overlapped structure
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = m_hevtOverlapped;

		// Use our internal overlapped structure
		lpOverlapped = &ovInternal;
	}

	// Make sure the overlapped structure isn't busy
	_ASSERTE(HasOverlappedIoCompleted(lpOverlapped));

	// Wait for the COM event
	if (!::WaitCommEvent(m_hFile,LPDWORD(&m_eEvent),lpOverlapped))
	{
		// Set the internal error code
		long lLastError = ::GetLastError();

		// Overlapped operation in progress is not an actual error
		if (lLastError != ERROR_IO_PENDING)
		{
			// Save the error
			m_lLastError = lLastError;

			// Issue an error and quit
			_RPTF0(_CRT_WARN,"CSerial::WaitEvent - Unable to wait for COM event\n");
			return m_lLastError;
		}

		// We need to block if the client didn't specify an overlapped structure
		if (lpOverlapped == &ovInternal)
		{
			// Wait for the overlapped operation to complete
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// The overlapped operation has completed
				break;

			case WAIT_TIMEOUT:
				// Cancel the I/O operation
				CancelIo();

				// The operation timed out. Set the internal error code and quit
				m_lLastError = ERROR_TIMEOUT;
				return m_lLastError;

			default:
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				_RPTF0(_CRT_WARN,"CSerial::WaitEvent - Unable to wait until COM event has arrived\n");
				return m_lLastError;
			}
		}
	}
	else
	{
		// The operation completed immediatly. Just to be sure
		// we'll set the overlapped structure's event handle.
		::SetEvent(lpOverlapped->hEvent);
	}

	// Return successfully
	return m_lLastError;
}


LONG CSerial::SetupHandshaking (EHandshake eHandshake)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::SetupHandshaking - Device is not opened\n");
		return m_lLastError;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::SetupHandshaking - Unable to obtain DCB information\n");
		return m_lLastError;
	}

	// Set the handshaking flags
	switch (eHandshake)
	{
	case EHandshake::EHandshakeOff:
		dcb.fOutxCtsFlow = false;					// Disable CTS monitoring
		dcb.fOutxDsrFlow = false;					// Disable DSR monitoring
//		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR monitoring
		dcb.fDtrControl = DTR_CONTROL_ENABLE;		// Set DTR high
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
// 		dcb.fRtsControl = RTS_CONTROL_ENABLE;		// RTS high (Ready To Send)
 		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
		break;

	case EHandshake::EHandshakeHardware:
		dcb.fOutxCtsFlow = true;					// Enable CTS monitoring
		dcb.fOutxDsrFlow = true;					// Enable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;	// Enable DTR handshaking
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// Enable RTS handshaking
		break;

	case EHandshake::EHandshakeSoftware:
		dcb.fOutxCtsFlow = false;					// Disable CTS (Clear To Send)
		dcb.fOutxDsrFlow = false;					// Disable DSR (Data Set Ready)
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR (Data Terminal Ready)
		dcb.fOutX = true;							// Enable XON/XOFF for transmission
		dcb.fInX = true;							// Enable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
		break;

	default:
		// This shouldn't be possible
		_ASSERTE(false);
		m_lLastError = E_INVALIDARG;
		return m_lLastError;
	}

	// Set the new DCB structure
	if (!::SetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::SetupHandshaking - Unable to set DCB information\n");
		return m_lLastError;
	}

	// Return successful
	return m_lLastError;
}

LONG CSerial::SetupReadTimeouts (EReadTimeout eReadTimeout)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::SetupReadTimeouts - Device is not opened\n");
		return m_lLastError;
	}

	// Determine the time-outs
	COMMTIMEOUTS cto;
	if (!::GetCommTimeouts(m_hFile,&cto))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::SetupReadTimeouts - Unable to obtain timeout information\n");
		return m_lLastError;
	}

	// Set the new timeouts
	switch (eReadTimeout)
	{
	case EReadTimeout::EReadTimeoutBlocking:
		cto.ReadIntervalTimeout = 0;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
		break;
	case EReadTimeout::EReadTimeoutNonblocking:
		cto.ReadIntervalTimeout = MAXDWORD;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
		break;
	default:
		// This shouldn't be possible
		_ASSERTE(false);
		m_lLastError = E_INVALIDARG;
		return m_lLastError;
	}

	// Set the new DCB structure
	if (!::SetCommTimeouts(m_hFile,&cto))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::SetupReadTimeouts - Unable to set timeout information\n");
		return m_lLastError;
	}

	// Return successful
	return m_lLastError;
}

CSerial::EBaudrate CSerial::GetBaudrate (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetBaudrate - Device is not opened\n");
		return EBaudrate::EBaudUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetBaudrate - Unable to obtain DCB information\n");
		return EBaudrate::EBaudUnknown;
	}

	// Return the appropriate baudrate
	return EBaudrate(dcb.BaudRate);
}

CSerial::EDataBits CSerial::GetDataBits (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetDataBits - Device is not opened\n");
		return EDataBits::EDataUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetDataBits - Unable to obtain DCB information\n");
		return EDataBits::EDataUnknown;
	}

	// Return the appropriate bytesize
	return EDataBits(dcb.ByteSize);
}

CSerial::EParity CSerial::GetParity (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetParity - Device is not opened\n");
		return EParity::EParUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetParity - Unable to obtain DCB information\n");
		return EParity::EParUnknown;
	}

	// Check if parity is used
	if (!dcb.fParity)
	{
		// No parity
		return EParity::EParNone;
	}

	// Return the appropriate parity setting
	return EParity(dcb.Parity);
}

CSerial::EStopBits CSerial::GetStopBits (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetStopBits - Device is not opened\n");
		return EStopBits::EStopUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetStopBits - Unable to obtain DCB information\n");
		return EStopBits::EStopUnknown;
	}

	// Return the appropriate stopbits
	return EStopBits(dcb.StopBits);
}

DWORD CSerial::GetEventMask (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetEventMask - Device is not opened\n");
		return 0;
	}

	// Return the event mask
	return m_dwEventMask;
}

BYTE CSerial::GetEventChar (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetEventChar - Device is not opened\n");
		return 0;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetEventChar - Unable to obtain DCB information\n");
		return 0;
	}

	// Set the new event character
	return BYTE(dcb.EvtChar);
}

CSerial::EHandshake CSerial::GetHandshaking (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetHandshaking - Device is not opened\n");
		return EHandshake::EHandshakeUnknown;
	}

	// Obtain the DCB structure for the device
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetHandshaking - Unable to obtain DCB information\n");
		return EHandshake::EHandshakeUnknown;
	}

	// Check if hardware handshaking is being used
	if ((dcb.fDtrControl == DTR_CONTROL_HANDSHAKE) && (dcb.fRtsControl == RTS_CONTROL_HANDSHAKE))
		return EHandshake::EHandshakeHardware;

	// Check if software handshaking is being used
	if (dcb.fOutX && dcb.fInX)
		return EHandshake::EHandshakeSoftware;

	// No handshaking is being used
	return EHandshake::EHandshakeOff;
}

LONG CSerial::Write (const void* pData, size_t iLen, DWORD* pdwWritten, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Overlapped operation should specify the pdwWritten variable
	_ASSERTE(!lpOverlapped || pdwWritten);

	// Check if time-outs are supported
	CheckTimeout(dwTimeout);

	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Use our own variable for read count
	DWORD dwWritten;
	if (pdwWritten == 0)
	{
		pdwWritten = &dwWritten;
	}

	// Reset the number of bytes written
	*pdwWritten = 0;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::Write - Device is not opened\n");
		return m_lLastError;
	}

	// Wait for the event to happen
	OVERLAPPED ovInternal;
	if (lpOverlapped == 0)
	{
		// Setup our own overlapped structure
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = m_hevtOverlapped;

		// Use our internal overlapped structure
		lpOverlapped = &ovInternal;
	}

	// Make sure the overlapped structure isn't busy
	_ASSERTE(HasOverlappedIoCompleted(lpOverlapped));

	// Write the data
	if (!::WriteFile(m_hFile,pData,iLen,pdwWritten,lpOverlapped))
	{
		// Set the internal error code
		long lLastError = ::GetLastError();

		// Overlapped operation in progress is not an actual error
		if (lLastError != ERROR_IO_PENDING)
		{
			// Save the error
			m_lLastError = lLastError;

			// Issue an error and quit
			_RPTF0(_CRT_WARN,"CSerial::Write - Unable to write the data\n");
			return m_lLastError;
		}

		// We need to block if the client didn't specify an overlapped structure
		if (lpOverlapped == &ovInternal)
		{
			// Wait for the overlapped operation to complete
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// The overlapped operation has completed
				if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwWritten,FALSE))
				{
					// Set the internal error code
					m_lLastError = ::GetLastError();

					_RPTF0(_CRT_WARN,"CSerial::Write - Overlapped completed without result\n");
					return m_lLastError;
				}
				break;

			case WAIT_TIMEOUT:
				// Cancel the I/O operation
				CancelIo();

				// The operation timed out. Set the internal error code and quit
				m_lLastError = ERROR_TIMEOUT;
				return m_lLastError;

			default:
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				_RPTF0(_CRT_WARN,"CSerial::Write - Unable to wait until data has been sent\n");
				return m_lLastError;
			}
		}
	}
	else
	{
		// The operation completed immediatly. Just to be sure
		// we'll set the overlapped structure's event handle.
		::SetEvent(lpOverlapped->hEvent);
	}

	// Return successfully
	return m_lLastError;
}

LONG CSerial::Write (LPCSTR pString, DWORD* pdwWritten, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Check if time-outs are supported
	CheckTimeout(dwTimeout);

	// Determine the length of the string
	return Write(pString,strlen(pString),pdwWritten,lpOverlapped,dwTimeout);
}

LONG CSerial::Read (void* pData, size_t iLen, DWORD* pdwRead, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Check if time-outs are supported
	CheckTimeout(dwTimeout);

	// Overlapped operation should specify the pdwRead variable
	_ASSERTE(!lpOverlapped || pdwRead);

	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Use our own variable for read count
	DWORD dwRead;
	if (pdwRead == 0)
	{
		pdwRead = &dwRead;
	}

	// Reset the number of bytes read
	*pdwRead = 0;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::Read - Device is not opened\n");
		return m_lLastError;
	}

	// Wait for the event to happen
	OVERLAPPED ovInternal;
	if (lpOverlapped == 0)
	{
		// Setup our own overlapped structure
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = m_hevtOverlapped;

		// Use our internal overlapped structure
		lpOverlapped = &ovInternal;
	}

	// Make sure the overlapped structure isn't busy
	_ASSERTE(HasOverlappedIoCompleted(lpOverlapped));

#ifdef _DEBUG
	// The debug version fills the entire data structure with
	// 0xDC bytes, to catch buffer errors as soon as possible.
	memset(pData,0xDC,iLen);
#endif
	
	// Read the data
	if (!::ReadFile(m_hFile,pData,iLen,pdwRead,lpOverlapped))
	{
		// Set the internal error code
		long lLastError = ::GetLastError();

		// Overlapped operation in progress is not an actual error
		if (lLastError != ERROR_IO_PENDING)
		{
			// Save the error
			m_lLastError = lLastError;

			// Issue an error and quit
			_RPTF0(_CRT_WARN,"CSerial::Read - Unable to read the data\n");
			return m_lLastError;
		}

		// We need to block if the client didn't specify an overlapped structure
		if (lpOverlapped == &ovInternal)
		{
			// Wait for the overlapped operation to complete
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// The overlapped operation has completed
				if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwRead,FALSE))
				{
					// Set the internal error code
					m_lLastError = ::GetLastError();

					_RPTF0(_CRT_WARN,"CSerial::Read - Overlapped completed without result\n");
					return m_lLastError;
				}
				break;

			case WAIT_TIMEOUT:
				// Cancel the I/O operation
				CancelIo();

				// The operation timed out. Set the internal error code and quit
				m_lLastError = ERROR_TIMEOUT;
				return m_lLastError;

			default:
				// Set the internal error code
				m_lLastError = ::GetLastError();

				// Issue an error and quit
				_RPTF0(_CRT_WARN,"CSerial::Read - Unable to wait until data has been read\n");
				return m_lLastError;
			}
		}
	}
	else
	{
		// The operation completed immediatly. Just to be sure
		// we'll set the overlapped structure's event handle.
		::SetEvent(lpOverlapped->hEvent);
	}

	// Return successfully
	return m_lLastError;
}

LONG CSerial::Purge()
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::Purge - Device is not opened\n");
		return m_lLastError;
	}

	if (!::PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_RXCLEAR))
	{
		// Set the internal error code
		m_lLastError = ::GetLastError();
		_RPTF0(_CRT_WARN,"CSerial::Purge - Overlapped completed without result\n");
	}
	
	// Return successfully
	return m_lLastError;
}

LONG CSerial::Break (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::Break - Device is not opened\n");
		return m_lLastError;
	}

    // Set the RS-232 port in break mode for a little while
    ::SetCommBreak(m_hFile);
    ::Sleep(100);
    ::ClearCommBreak(m_hFile);

	// Return successfully
	return m_lLastError;
}

CSerial::EEvent CSerial::GetEventType (void)
{
#ifdef _DEBUG
	// Check if the event is within the mask
	if ((static_cast<int>(m_eEvent) & static_cast<int>(m_dwEventMask)) == 0)
		_RPTF2(_CRT_WARN,"CSerial::GetEventType - Event %08Xh not within mask %08Xh.\n", m_eEvent, m_dwEventMask);
#endif

	// Obtain the event (mask unwanted events out)
	EEvent eEvent = EEvent(static_cast<int>(m_eEvent) & static_cast<int>(m_dwEventMask));

	// Reset internal event type
	m_eEvent = EEvent::EEventNone;

	// Return the current cause
	return eEvent;
}

CSerial::EError CSerial::GetError (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Check if the device is open
	if (m_hFile == 0)
	{
		// Set the internal error code
		m_lLastError = ERROR_INVALID_HANDLE;

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetError - Device is not opened\n");
		return EError::EErrorUnknown;
	}

	// Obtain COM status
	DWORD dwErrors = 0;
	if (!::ClearCommError(m_hFile,&dwErrors,0))
	{
		// Set the internal error code
		m_lLastError = ::GetLastError();

		// Issue an error and quit
		_RPTF0(_CRT_WARN,"CSerial::GetError - Unable to obtain COM status\n");
		return EError::EErrorUnknown;
	}

	// Return the error
	return EError(dwErrors);
}

bool CSerial::GetCTS (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetCTS - Unable to obtain the modem status\n");
		return false;
	}

	// Determine if CTS is on
	return (dwModemStat & MS_CTS_ON) != 0;
}

bool CSerial::GetDSR (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetDSR - Unable to obtain the modem status\n");
		return false;
	}

	// Determine if DSR is on
	return (dwModemStat & MS_DSR_ON) != 0;
}

bool CSerial::GetRing (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetRing - Unable to obtain the modem status");
		return false;
	}

	// Determine if Ring is on
	return (dwModemStat & MS_RING_ON) != 0;
}

bool CSerial::GetRLSD (void)
{
	// Reset error state
	m_lLastError = ERROR_SUCCESS;

	// Obtain the modem status
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
	{
		// Obtain the error code
		m_lLastError = ::GetLastError();

		// Display a warning
		_RPTF0(_CRT_WARN,"CSerial::GetRLSD - Unable to obtain the modem status");
		return false;
	}

	// Determine if RLSD is on
	return (dwModemStat & MS_RLSD_ON) != 0;
}

#ifdef SERIAL_NO_CANCELIO

void CheckTimeout (DWORD dwTimeout)
{
	if ((dwTimeout != 0) && (dwTimeout != INFINITE))
	{
		// Quit application
		::MessageBox(0,_T("Timeouts are not supported, when SERIAL_WIN95_SUPPORT is defined"),_T("Serial library"), MB_ICONERROR | MB_TASKMODAL);
		::DebugBreak();
		::ExitProcess(0xFFFFFFF);
	}
}

BOOL CSerial::CancelIo (void)
{
	// CancelIo shouldn't have been called at this point
	::DebugBreak();
	return FALSE;
}

#endif	// SERIAL_NO_CANCELIO

LONG CSerial::SetDTR(bool bStatus)
{
    m_lLastError = ERROR_SUCCESS;
 
    if (!::EscapeCommFunction(m_hFile, bStatus ? SETDTR : CLRDTR))
    {
        m_lLastError = ::GetLastError();
        _RPTF0(_CRT_WARN, "CSerial::SetDTR - The extended function failed\n");
    }
 
    return m_lLastError;
}
 
LONG CSerial::SetRTS(bool bStatus)
{
    m_lLastError = ERROR_SUCCESS;
 
    if (!::EscapeCommFunction(m_hFile, bStatus ? SETRTS : CLRRTS))
    {
        m_lLastError = ::GetLastError();
        _RPTF0(_CRT_WARN, "CSerial::SetRTS - The extended function failed\n");
    }
 
    return m_lLastError;
}