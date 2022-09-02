// EventLog.cpp: implementation of the CEventLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EFM.h"
#include "EventLog.h"
#include "Configuration.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CConfiguration config;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventLog::CEventLog()
{
	m_szFilename = "";
	m_bBeingViewed = false;
}

CEventLog::~CEventLog()
{

}

BOOL CEventLog::OpenLogFile(int iReadWrite)
{
	BOOL ret = FALSE;

	// Check to see if the data path exists.  If it doesn't, then create it.
	DWORD dwAttr = GetFileAttributes(config.m_szDataDir);
	if (dwAttr == 0xffffffff)
	{
		::CreateDirectory(config.m_szDataDir, NULL);
	}

	if (!m_bBeingViewed)
	{
		ret = m_fileEventLog.Open(m_szFilename, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate|CFile::shareDenyNone, NULL);
	}

/*
	switch (iReadWrite)
	{
		case CEventLog::ERead:
			ret = m_fileEventLog.Open(m_szFilename, CFile::modeReadWrite|CFile::shareDenyNone, NULL);
			//m_fileEventLog.SeekToBegin();
			break;
		case CEventLog::EWrite:
			ret = m_fileEventLog.Open(m_szFilename, CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate|CFile::shareDenyNone, NULL);
			//m_fileEventLog.SeekToEnd();
			break;
		case CEventLog::ETruncate:
			ret = m_fileEventLog.Open(m_szFilename, CFile::modeWrite|CFile::modeCreate, NULL);
			break;
	}
*/
	return ret;
}

void CEventLog::CloseLogFile()
{
	if (!m_bBeingViewed)
	{
		m_fileEventLog.Close();
	}
}

void CEventLog::LogEvent(CString szEventText)
{
	CString txt;

	m_csLog.Lock();

	// Open the log file
	OpenLogFile(CEventLog::EWrite);

	DWORD dwCurrentPos = static_cast<DWORD>(m_fileEventLog.GetPosition());

	m_fileEventLog.SeekToEnd();

	CTime current_time(CTime::GetCurrentTime());
	txt.Format("%02d/%02d/%d %02d:%02d:%02d %s\n",current_time.GetMonth(),current_time.GetDay(),current_time.GetYear(),current_time.GetHour(),current_time.GetMinute(),current_time.GetSecond(),szEventText);
	m_fileEventLog.WriteString(txt);
	m_fileEventLog.Flush();
	
	m_fileEventLog.Seek(dwCurrentPos, CFile::begin);

	// Close the log file
	CloseLogFile();

	m_csLog.Unlock();
}


void CEventLog::SetEventLogFilename(CString szFilename)
{
	m_szFilename = szFilename;
}

CString CEventLog::GetEventLogFilename()
{
	return m_szFilename;
}
