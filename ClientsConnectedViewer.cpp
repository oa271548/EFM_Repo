// CClientsConnectedViewer.cpp : implementation file
//

#include "stdafx.h"
#include "ClientsConnectedViewer.h"

// CClientsConnectedViewer dialog

IMPLEMENT_DYNAMIC(CClientsConnectedViewer, CDialog)

CClientsConnectedViewer::CClientsConnectedViewer(tDATACLIENTSERVER* tDataClientServer, CWnd* pParent /*=nullptr*/)
	: CDialog(CClientsConnectedViewer::IDD, pParent), m_tDataClientServer(tDataClientServer), m_nCliensCount(0)
{
	
}

CClientsConnectedViewer::~CClientsConnectedViewer()
{
}

void CClientsConnectedViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, LST_CONNECTEDCLIENTS, m_lstConnectedClients);
}

void CClientsConnectedViewer::OnTimer(UINT_PTR nIDEvent)
{
	size_t nCountClients = 0;
	if(nIDEvent == TMR_ONEFIFTHSEC)
	{
		for (size_t iNumServer = 0; iNumServer < MAX_NUMOFDATASOURCES; ++iNumServer)
		{
			if (m_tDataClientServer[iNumServer].bStillListening)
			{
				nCountClients += m_tDataClientServer[iNumServer].pThreadInfo->ClientsCount();
			}
		}
		if (m_nCliensCount != nCountClients)
		{
			DrawListOfClients();
			m_nCliensCount = nCountClients;
		}
	}
	CDialog::OnTimer(nIDEvent);
}


BEGIN_MESSAGE_MAP(CClientsConnectedViewer, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CClientsConnectedViewer::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	

	m_lstConnectedClients.InsertColumn(0, "#", LVCFMT_LEFT, 25);
	m_lstConnectedClients.InsertColumn(1, "Data Source Name", LVCFMT_LEFT, 150);
	m_lstConnectedClients.InsertColumn(2, "IP Address", LVCFMT_LEFT, 150);
	m_lstConnectedClients.InsertColumn(3, "Connected date/time", LVCFMT_LEFT, 150);

	DrawListOfClients();
	SetTimer(TMR_ONEFIFTHSEC, 200, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CClientsConnectedViewer::OnCancel()
{
	KillTimer(TMR_ONEFIFTHSEC);
	CDialog::OnCancel();
}


void CClientsConnectedViewer::DrawListOfClients()
{
	size_t nCountClients = 0;
	m_lstConnectedClients.DeleteAllItems();
	for(size_t iNumServer = 0; iNumServer < MAX_NUMOFDATASOURCES; ++iNumServer)
	{
		if(m_tDataClientServer[iNumServer].bStillListening)
		{
			for(size_t iNumClient = 0; iNumClient < MAX_NETWORKCLIENTS; ++iNumClient)
			{
				auto& client = m_tDataClientServer[iNumServer].pThreadInfo->clientsConnected[iNumClient];
				if(client.GetIsAlive())
				{
					CString txt;
					txt.Format("%d", nCountClients + 1);
					m_lstConnectedClients.InsertItem(nCountClients, txt, 0);
					m_lstConnectedClients.SetItemText(nCountClients, 1, client.GetDataSourceName());
					m_lstConnectedClients.SetItemText(nCountClients, 2, client.GetIPAddress());
					CString timestr;
					timestr.Format("%02d/%02d/%d  %02d:%02d:%02d",
						client.GetTimeDateConnected().GetMonth(),
						client.GetTimeDateConnected().GetDay(),
						client.GetTimeDateConnected().GetYear(),
						client.GetTimeDateConnected().GetHour(),
						client.GetTimeDateConnected().GetMinute(),
						client.GetTimeDateConnected().GetSecond());
					m_lstConnectedClients.SetItemText(nCountClients, 3, timestr);
					++nCountClients;
				}
			}
		}
	}

	CString txt;
	txt.Format("%d", nCountClients);
	((CStatic*)GetDlgItem(LBL_CONNECTEDCLIENTCOUNT))->SetWindowText(txt);
	UpdateData(FALSE);
}

// CClientsConnectedViewer message handlers