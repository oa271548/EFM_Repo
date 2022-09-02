// PSOptions.cpp : implementation file
//

#include "stdafx.h"
#include "EFM.h"
#include "PSOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPSOptions

IMPLEMENT_DYNAMIC(CPSOptions, CPropertySheet)

CPSOptions::CPSOptions(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CPSOptions::CPSOptions(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CPSOptions::~CPSOptions()
{
}


BEGIN_MESSAGE_MAP(CPSOptions, CPropertySheet)
	//{{AFX_MSG_MAP(CPSOptions)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPSOptions message handlers
