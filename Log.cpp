// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Log.h"
#include "Configuration.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define FLUSH_INTERVAL 10

extern CConfiguration config;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::CLog()
{
	flushcount = FLUSH_INTERVAL;
	m_bLogOpen = false;
}

CLog::~CLog()
{
	// close log file if it was open
	if (logfile.m_pStream )
		logfile.Close();
}

void CLog::CloseFile()
{
	CSingleLock lock(&m_cslog, TRUE);

	// close log file if it was open
	if (logfile.m_pStream)
	{
		logfile.Close();
		m_bLogOpen = false;
	}
}

void CLog::Log(CTime time, CString str)
{
	CSingleLock lock(&m_cslog, TRUE);

	// Check if this data has same date as currently open file
	if (time.GetDay()!=date.GetDay()) {
		CloseFile();
		OpenFile(time);
	}
	// write string to log file
	logfile.WriteString(str);

	if (--flushcount <= 0) {
		logfile.Flush();
		flushcount = FLUSH_INTERVAL;
	}

}


void CLog::Start(CString szPath, CString ext)
{
	CSingleLock lock(&m_cslog, TRUE);

	file_extension = ext; // remember the file extension for tomorrows filename
	log_path = szPath;
	OpenFile(CTime::GetCurrentTime()); // get ready to log
}

void CLog::OpenFile(CTime newdate)
{
	CSingleLock lock(&m_cslog, TRUE);

	CString txt;

	// open log file
	date = newdate; // remember the date of the file we are opening

	CString filename;
	filename.Format("%s%02d%02d%02d%s", log_path, date.GetMonth(), date.GetDay(), date.GetYear(), file_extension.GetBuffer(10));

	if (!logfile.Open(filename, CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::modeNoTruncate, &m_fe))
	{
		// There was an error opening/creating the log file
		TCHAR szError[1024];
		m_fe.GetErrorMessage(szError, 1024);
		::MessageBox(NULL,szError,"Logging Error",MB_OK|MB_ICONWARNING);
		m_bLogOpen = false;
	}
	else
	{
		// No errors opening/creating the log file
		logfile.SeekToEnd();
		m_bLogOpen = true;
	}
	
}

bool CLog::GetLogOpen()
{
	CSingleLock lock(&m_cslog, TRUE);
	return m_bLogOpen;
}

void CLog::SetLogOpen(bool bOpen)
{
	CSingleLock lock(&m_cslog, TRUE);
	m_bLogOpen = bOpen;
}
