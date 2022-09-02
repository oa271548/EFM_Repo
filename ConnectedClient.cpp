// ConnectedClient.cpp: implementation of the CConnectedClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EFM.h"
#include "ConnectedClient.h"
#include <tuple>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnectedClient::CConnectedClient()
{
	SetSocket(0);
	SetThreadPtr(NULL);
	SetIsAlive(false);
	SetDebuggerWindow(NULL);
	SetIPAddress("0.0.0.0");
}

// Copy Constructor
CConnectedClient::CConnectedClient(const CConnectedClient & c)
{
	m_bIsAlive = c.m_bIsAlive;
	m_hThreadExit = c.m_hThreadExit;
	m_pThread = c.m_pThread;
	m_socket = c.m_socket;
	m_vFifo = c.m_vFifo;
}

// Assignment Operator =
CConnectedClient& CConnectedClient::operator=(const CConnectedClient& rhs) 
{
	m_bIsAlive = rhs.m_bIsAlive;
	m_hThreadExit = rhs.m_hThreadExit;
	m_pThread = rhs.m_pThread;
	m_socket = rhs.m_socket;
	m_vFifo = rhs.m_vFifo;
	return *this;
}

CConnectedClient::~CConnectedClient()
{

}

bool CConnectedClient::GetIsAlive()
{
	CSingleLock lock(&m_csIsAlive, TRUE);
	return m_bIsAlive;
}

void CConnectedClient::SetIsAlive(bool bAlive)
{
	CSingleLock lock(&m_csIsAlive, TRUE);
	m_bIsAlive = bAlive;
}

SOCKET CConnectedClient::GetSocket()
{
	CSingleLock lock(&m_csSocket, TRUE);
	return m_socket;
}

void CConnectedClient::SetSocket(SOCKET sock)
{
	CSingleLock lock(&m_csSocket, TRUE);
	m_socket = sock;
}

CWinThread * CConnectedClient::GetThreadPtr()
{
	CSingleLock lock(&m_csThread, TRUE);
	return m_pThread;
}

void CConnectedClient::SetThreadPtr(CWinThread *pThread)
{
	CSingleLock lock(&m_csThread, TRUE);
	m_pThread = pThread;
}

HANDLE CConnectedClient::GetThreadExitHandle()
{
	CSingleLock lock(&m_csThreadExit, TRUE);
	return m_hThreadExit;
}

void CConnectedClient::SetThreadExitHandle(HANDLE h)
{
	CSingleLock lock(&m_csThreadExit, TRUE);
	m_hThreadExit = h;
}

void CConnectedClient::FifoAdd(std::string txtData)
{
	CSingleLock lock(&m_csFifo, TRUE);
	m_vFifo.push_back(txtData);
}

std::string CConnectedClient::FifoRemove()
{
	CSingleLock lock(&m_csFifo, TRUE);
	std::string txtData;
	if (!m_vFifo.empty())
	{
		txtData = m_vFifo.at(0);
		m_vFifo.erase(m_vFifo.begin());
	}
	return txtData;
}

int CConnectedClient::FifoGetSize()
{
	CSingleLock lock(&m_csFifo, TRUE);
	return m_vFifo.size();
}

void CConnectedClient::SetDebuggerWindow(CDlgDebugger *pDlg)
{
	CSingleLock lock(&m_csDlgDebugger, TRUE);
	m_pDlgDebugger = pDlg;
}

CDlgDebugger * CConnectedClient::GetDebuggerWindow()
{
	CSingleLock lock(&m_csDlgDebugger, TRUE);
	return m_pDlgDebugger;
}

void CConnectedClient::SetIPAddress(CString txtIP)
{
	CSingleLock lock(&m_csIPAddress, TRUE);
	m_txtIPAddress = txtIP;
}

void CConnectedClient::SetTimeDateConnected(CTime time)
{
	CSingleLock lock(&m_csDataSourceName, TRUE);
	m_tTimeDateConnected = time;
}

CTime CConnectedClient::GetTimeDateConnected()
{
	CSingleLock lock(&m_csDataSourceName, TRUE);
	return m_tTimeDateConnected;
}

void CConnectedClient::SetDataSourceName(CString txtDataSourceName)
{
	CSingleLock lock(&m_csDataSourceName, TRUE);
	m_txtDataSourceName = txtDataSourceName;
}

CString CConnectedClient::GetDataSourceName()
{
	CSingleLock lock(&m_csDataSourceName, TRUE);
	return m_txtDataSourceName;
}

CString CConnectedClient::GetIPAddress()
{
	CSingleLock lock(&m_csIPAddress, TRUE);
	return m_txtIPAddress;
}

void CConnectedClient::FifoClearAll()
{
	CSingleLock lock(&m_csFifo, TRUE);
	m_vFifo.clear();
}
