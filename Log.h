// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__52DC3AB4_2216_4627_ADE4_1CE38CDCE2C5__INCLUDED_)
#define AFX_LOG_H__52DC3AB4_2216_4627_ADE4_1CE38CDCE2C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>

class CLog  
{
public:
	void SetLogOpen(bool bOpen);
	bool GetLogOpen();
	CFileException m_fe;
	void Start(CString szPath, CString ext);
	CLog(); // create by passing ".log" or ".efm" etc.
	virtual ~CLog();

	void Log(CTime time, CString str); // pass time(date) and string to log
	void CloseFile();

private:
	int flushcount;
	void OpenFile(CTime newdate);
	
	CString file_extension; // ".log" for event log or ".efm" for data, etc
	CString log_path;		// The path to the log file
	CStdioFile logfile;
	CTime date; // date of currently opened logfile
	CCriticalSection m_cslog;
	bool m_bLogOpen;
};

#endif // !defined(AFX_LOG_H__52DC3AB4_2216_4627_ADE4_1CE38CDCE2C5__INCLUDED_)
