// WaitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EFM.h"
#include "WaitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg dialog


CWaitDlg::CWaitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaitDlg)
	m_lblWaitText = _T("");
	m_lblWaitSubText = _T("");
	//}}AFX_DATA_INIT

	m_fontMain.CreateFont(	32,                            // Height
							0,                             // Width
							0,                             // Escapement
							0,                             // Orientation
							FW_BOLD,                       // Weight
							FALSE,                         // Italic
							FALSE,                         // Underline
							0,                             // StrikeOut
							ANSI_CHARSET,                  // CharSet
							OUT_DEFAULT_PRECIS,            // OutPrecision
							CLIP_DEFAULT_PRECIS,           // ClipPrecision
							DEFAULT_QUALITY,               // Quality
							DEFAULT_PITCH | FF_SWISS,      // PitchAndFamily
							"Verdana");                    // Facename

	m_fontSub.CreateFont(	24,                            // Height
							0,                             // Width
							0,                             // Escapement
							0,                             // Orientation
							0,                       // Weight
							FALSE,                         // Italic
							FALSE,                         // Underline
							0,                             // StrikeOut
							ANSI_CHARSET,                  // CharSet
							OUT_DEFAULT_PRECIS,            // OutPrecision
							CLIP_DEFAULT_PRECIS,           // ClipPrecision
							DEFAULT_QUALITY,               // Quality
							DEFAULT_PITCH | FF_SWISS,      // PitchAndFamily
							"Verdana");                    // Facename
}


void CWaitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaitDlg)
	DDX_Text(pDX, LBL_WAITTEXT, m_lblWaitText);
	DDX_Text(pDX, LBL_WAITSUBTEXT, m_lblWaitSubText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaitDlg, CDialog)
	//{{AFX_MSG_MAP(CWaitDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg message handlers

void CWaitDlg::ShowSplashScreen(CString szMain, CString szSub)
{
	m_lblWaitText = szMain;
	m_lblWaitSubText = szSub;
	(GetDlgItem(LBL_WAITTEXT))->SetFont(&m_fontMain);
	(GetDlgItem(LBL_WAITSUBTEXT))->SetFont(&m_fontSub);
	UpdateData(FALSE);
	CenterWindow();
	ShowWindow(SW_SHOW);
	UpdateWindow();

}
