// AddEditRecipient.cpp : implementation file
//

#include "stdafx.h"
#include "efm.h"
#include "AddEditRecipient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddEditRecipient dialog


CAddEditRecipient::CAddEditRecipient(CWnd* pParent /*=NULL*/, int iMode, CString* data)
	: CDialog(CAddEditRecipient::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddEditRecipient)
	m_editbox2 = _T("");
	m_editbox1 = _T("");
	m_label1 = _T("");
	m_label2 = _T("");
	//}}AFX_DATA_INIT

	m_iMode = iMode; // mode is ADD or EDIT
	m_data = data; // save the pointer for passing back data
}


void CAddEditRecipient::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddEditRecipient)
	DDX_Text(pDX, IDC_EDIT2, m_editbox2);
	DDX_Text(pDX, IDC_EDIT1, m_editbox1);
	DDX_Text(pDX, LBL_ADDEDITRECIPIENT1, m_label1);
	DDX_Text(pDX, LBL_ADDEDITRECIPIENT2, m_label2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddEditRecipient, CDialog)
	//{{AFX_MSG_MAP(CAddEditRecipient)
	ON_BN_CLICKED(BTN_ACCEPT, OnAccept)
	ON_WM_CANCELMODE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(BTN_CANCEL, OnCancel)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddEditRecipient message handlers

void CAddEditRecipient::OnAccept() 
{
	// TODO: Add your control notification handler code here

	EndDialog(1);	
	
}

void CAddEditRecipient::OnCancel() 
{
	// TODO: Add your control notification handler code here
	EndDialog(-1);	
	
}

BOOL CAddEditRecipient::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// Separate the string we were passed in constructor into Name and Number/Email
	m_editbox1 = m_data->Left(m_data->Find("\t")); // get Name
	m_editbox2 = m_data->Mid( m_data->Find("\t")+1); // get number/email
	
	
	switch (m_iMode)
	{
		case ADD_EMAIL:
			this->SetWindowText("Add Recipient");
			m_label1 = "Name"; 
			m_label2 = "Email Address"; 
			break;
		case EDIT_EMAIL:
			this->SetWindowText("Edit Recipient");
			m_label1 = "Name"; 
			m_label2 = "Email Address"; 
			break;
		case ADD_SMS:
			this->SetWindowText("Add Recipient");
			m_label1 = "Name"; 
			m_label2 = "Phone Number"; 
			break;
		case EDIT_SMS:
			this->SetWindowText("Edit Recipient");
			m_label1 = "Name"; 
			m_label2 = "Phone Number"; 
			break;
	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddEditRecipient::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CAddEditRecipient::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
}

void CAddEditRecipient::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	UpdateData(TRUE);
	*m_data = m_editbox1 + CString("\t") + m_editbox2;
	
	
}

