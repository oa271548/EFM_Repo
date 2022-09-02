// ConnectedClient.h: interface for the CConnectedClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTEDCLIENT_H__1BD81EB0_3887_480A_891F_BF81FE76728D__INCLUDED_)
#define AFX_CONNECTEDCLIENT_H__1BD81EB0_3887_480A_891F_BF81FE76728D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace std;

#include <afxmt.h>
#include "DlgDebugger.h"
#include <string>

class CConnectedClient  
{
public:
	void FifoClearAll();
	CString GetIPAddress();
	void SetIPAddress(CString txtIP);
	void SetTimeDateConnected(CTime time);
	CTime GetTimeDateConnected();
	void SetDataSourceName(CString txtDataSourceName);
	CString GetDataSourceName();
	CDlgDebugger * GetDebuggerWindow();
	void SetDebuggerWindow(CDlgDebugger * pDlg);
	int FifoGetSize();
	std::string FifoRemove();
	void FifoAdd(std::string txtData);
	void SetThreadExitHandle(HANDLE h);
	HANDLE GetThreadExitHandle();
	void SetThreadPtr(CWinThread * pThread);
	CWinThread * GetThreadPtr();
	void SetSocket(SOCKET sock);
	SOCKET GetSocket();
	void SetIsAlive(bool bAlive);
	bool GetIsAlive();

	CConnectedClient();
	CConnectedClient(const CConnectedClient & c);
	CConnectedClient& operator=(const CConnectedClient& rhs);
	virtual ~CConnectedClient();

private:
	CCriticalSection m_csIPAddress;
	CString m_txtIPAddress;
	CCriticalSection m_csDataSourceName;
	CString m_txtDataSourceName;
	CCriticalSection m_csTimeDateConnected;
	CTime m_tTimeDateConnected;
	CCriticalSection m_csDlgDebugger;
	CDlgDebugger * m_pDlgDebugger;
	CCriticalSection m_csFifo;
	vector <std::string> m_vFifo;
	CCriticalSection m_csIsAlive;
	bool m_bIsAlive;
	CCriticalSection m_csSocket;
	SOCKET m_socket;
	CCriticalSection m_csThread;
	CWinThread * m_pThread;
	CCriticalSection m_csThreadExit;
	HANDLE m_hThreadExit;
};

#endif // !defined(AFX_CONNECTEDCLIENT_H__1BD81EB0_3887_480A_891F_BF81FE76728D__INCLUDED_)
