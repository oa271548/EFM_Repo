#if !defined(AFX_PSOPTIONS_H__E87C4788_9F94_4494_AAFA_CBC93CF722E6__INCLUDED_)
#define AFX_PSOPTIONS_H__E87C4788_9F94_4494_AAFA_CBC93CF722E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PSOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPSOptions

class CPSOptions : public CPropertySheet
{
	DECLARE_DYNAMIC(CPSOptions)

// Construction
public:
	CPSOptions(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPSOptions(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPSOptions)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPSOptions();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPSOptions)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSOPTIONS_H__E87C4788_9F94_4494_AAFA_CBC93CF722E6__INCLUDED_)
