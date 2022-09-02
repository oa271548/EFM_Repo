// DlgAddRemoveRing.cpp : implementation file
//

#include "stdafx.h"
#include "EFM.h"
#include "DlgAddRemoveRing.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAddRemoveRing dialog


CDlgAddRemoveRing::CDlgAddRemoveRing(CWnd* pParent /*=NULL*/, int iMode /*=ADD*/, int iDistanceUnit /*=0*/)
	: CDialog(CDlgAddRemoveRing::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddRemoveRing)
	m_txtAddRemoveLabel = _T("");
	m_uiAddRemoveValue = 0;
	m_txtDistanceUnit = _T("");
	//}}AFX_DATA_INIT

	m_iMode = iMode;
	m_iDistanceUnit = iDistanceUnit;
	switch (m_iDistanceUnit)
	{
		case MI:
			m_txtDistanceUnit = "Mi";
			break;
		case KM:
			m_txtDistanceUnit = "Km";
			break;
	}
}


void CDlgAddRemoveRing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddRemoveRing)
	DDX_Text(pDX, LBL_ADDREMOVETEXT, m_txtAddRemoveLabel);
	DDX_Text(pDX, TXT_ADDREMOVEVALUE, m_uiAddRemoveValue);
	DDX_Text(pDX, LBL_DISTANCEUNIT, m_txtDistanceUnit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddRemoveRing, CDialog)
	//{{AFX_MSG_MAP(CDlgAddRemoveRing)
	ON_BN_CLICKED(BTN_ACCEPT, OnAccept)
	ON_BN_CLICKED(BTN_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddRemoveRing message handlers

BOOL CDlgAddRemoveRing::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	switch (m_iMode)
	{
		case ADD:
			this->SetWindowText("Add Range");
			m_txtAddRemoveLabel = "Enter the ring range to add:";
			break;
		case REMOVE:
			this->SetWindowText("Remove Range");
			m_txtAddRemoveLabel = "Enter the ring range to remove:";
			break;
	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAddRemoveRing::OnAccept() 
{
	UpdateData(TRUE);
	int iMaxDistance;
	CString txtError;

	if (m_iDistanceUnit == MI)
	{
		iMaxDistance = 24;
	}
	else if (m_iDistanceUnit == KM)
	{
		iMaxDistance = 38;
	}

	if ((m_uiAddRemoveValue > 0) && (m_uiAddRemoveValue <= (unsigned int)iMaxDistance))
	{
		EndDialog(m_uiAddRemoveValue);
	}
	else
	{
		txtError.Format("The maximum range value is %d %s.  Please enter a value within that range.", iMaxDistance, (m_iDistanceUnit==MI)?"miles":"kilometers");
		::MessageBox(this->GetSafeHwnd(), txtError, "Range Error", MB_OK|MB_ICONERROR);
	}
}

void CDlgAddRemoveRing::OnCancel() 
{
	EndDialog(-1);	
}
