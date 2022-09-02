// EMailRecipients.cpp : implementation file
//

#include "stdafx.h"
#include "efm.h"
#include "EMailRecipients.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEMailRecipients dialog


CEMailRecipients::CEMailRecipients(CWnd* pParent /*=NULL*/)
	: CDialog(CEMailRecipients::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEMailRecipients)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEMailRecipients::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEMailRecipients)
	DDX_Control(pDX, LST_RECIPIENTS, m_lstRecipients);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEMailRecipients, CDialog)
	//{{AFX_MSG_MAP(CEMailRecipients)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEMailRecipients message handlers

BOOL CEMailRecipients::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_lstRecipients.InsertColumn(0, "Name", LVCFMT_LEFT, 150);
	m_lstRecipients.InsertColumn(1, "EMail Address", LVCFMT_LEFT, 150);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
