// EventLog.h: interface for the CEventLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTLOG_H__3CE0CA62_1F7E_4078_A2C2_1B03406CCEFB__INCLUDED_)
#define AFX_EVENTLOG_H__3CE0CA62_1F7E_4078_A2C2_1B03406CCEFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>

class CEventLog  
{
public:
	bool m_bBeingViewed;
	CString GetEventLogFilename();
	CString m_szFilename;
	CStdioFile m_fileEventLog;
	void SetEventLogFilename(CString szFilename);
	void LogEvent(CString szEventText);
	BOOL OpenLogFile(int iReadWrite);
	void CloseLogFile();
	CEventLog();
	virtual ~CEventLog();

	typedef enum
	{
		ERead   = 0,
		EWrite   = 1,
		ETruncate = 2
	} 
	EFileOpenMode;
	CCriticalSection m_csLog;

private:


};

#endif // !defined(AFX_EVENTLOG_H__3CE0CA62_1F7E_4078_A2C2_1B03406CCEFB__INCLUDED_)
