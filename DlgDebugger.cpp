// DlgDebugger.cpp : implementation file
//

#include "stdafx.h"
#include "EFM.h"
#include "DlgDebugger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDebugger dialog


CDlgDebugger::CDlgDebugger(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDebugger::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDebugger)
	//}}AFX_DATA_INIT
}


void CDlgDebugger::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDebugger)
	DDX_Control(pDX, LST_CONNECTEDCLIENTS, m_lstConnectedClients);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDebugger, CDialog)
	//{{AFX_MSG_MAP(CDlgDebugger)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDebugger message handlers

BOOL CDlgDebugger::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	((CStatic*)GetDlgItem(LBL_CONNECTEDCLIENTCOUNT))->SetWindowText("0");

	m_lstConnectedClients.InsertColumn(0, "#", LVCFMT_LEFT, 25);
	m_lstConnectedClients.InsertColumn(1, "IP Address", LVCFMT_LEFT, 75);
	m_lstConnectedClients.InsertColumn(2, "FIFO Size", LVCFMT_LEFT, 75);

	m_iIndex = 0;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDebugger::SetClientCount(int iCount)
{
	CSingleLock lock(&m_csClientCount, TRUE);
	CString txt;
	txt.Format("%d", iCount);

	((CStatic*)GetDlgItem(LBL_CONNECTEDCLIENTCOUNT))->SetWindowText(txt);
}

int CDlgDebugger::AddClient(CString txtIPAddress)
{
	CSingleLock lock(&m_csClientList, TRUE);

	CString txt;

	int iNumClients = m_lstConnectedClients.GetItemCount();

	txt.Format("%d", m_iIndex);
	m_lstConnectedClients.InsertItem(iNumClients, txt, 0);
	m_lstConnectedClients.SetItemText(iNumClients, 1, txtIPAddress);
	m_lstConnectedClients.SetItemText(iNumClients, 2, "0");

	m_iIndex++;

	return m_iIndex-1;	// Gives each client a unique id that they can use to update their debugging info.
}

void CDlgDebugger::RemoveClient(int iID)
{
	CSingleLock lock(&m_csClientList, TRUE);

	LVFINDINFO info;
	int iIndex;
	CString txt;

	memset(&info, 0x00, sizeof(LVFINDINFO));
	info.flags = LVFI_STRING;
	txt.Format("%d", iID);
	info.psz = txt;
	// Try to find the item
	iIndex = m_lstConnectedClients.FindItem(&info);
	if (iIndex >= 0)
	{
		// Item exists
		m_lstConnectedClients.DeleteItem(iIndex);
	} // if (iIndex < 0)
}

void CDlgDebugger::UpdateClient(int iID, int iFIFOSize)
{
	CSingleLock lock(&m_csClientList, TRUE);

	LVFINDINFO info;
	int iIndex;
	CString txt;

	memset(&info, 0x00, sizeof(LVFINDINFO));
	info.flags = LVFI_STRING;
	txt.Format("%d", iID);
	info.psz = txt;
	// Try to find the item
	iIndex = m_lstConnectedClients.FindItem(&info);
	if (iIndex >= 0)
	{
		// Item exists
		txt.Format("%d", iFIFOSize);
		m_lstConnectedClients.SetItemText(iIndex, 2, txt);
	} // if (iIndex < 0)
}
