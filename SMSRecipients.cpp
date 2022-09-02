// SMSRecipients.cpp : implementation file
//

#include "stdafx.h"
#include "efm.h"
#include "SMSRecipients.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSMSRecipients dialog


CSMSRecipients::CSMSRecipients(CWnd* pParent /*=NULL*/)
	: CDialog(CSMSRecipients::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSMSRecipients)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSMSRecipients::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSMSRecipients)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSMSRecipients, CDialog)
	//{{AFX_MSG_MAP(CSMSRecipients)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSMSRecipients message handlers
