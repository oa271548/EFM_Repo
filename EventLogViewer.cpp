// EventLogViewer.cpp : implementation file
//

#include "stdafx.h"
#include "EFM.h"
#include "EventLogViewer.h"
#include "Configuration.h"
#include "EventLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CConfiguration config;
extern CEventLog m_EventLog;

#define GUTTER	10
#define BUTTON_WIDTH	100
#define BUTTON_HEIGHT	30

/////////////////////////////////////////////////////////////////////////////
// CEventLogViewer dialog


CEventLogViewer::CEventLogViewer(CWnd* pParent /*=NULL*/)
	: CDialog(CEventLogViewer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventLogViewer)
	//}}AFX_DATA_INIT
}


void CEventLogViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventLogViewer)
	DDX_Control(pDX, LST_EVENTS, m_lstEvents);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEventLogViewer, CDialog)
	//{{AFX_MSG_MAP(CEventLogViewer)
	ON_BN_CLICKED(BTN_CLOSE, OnClose)
	ON_BN_CLICKED(BTN_CLEARLOGFILE, OnClearlogfile)
	ON_BN_CLICKED(BTN_REFRESHLOG, OnRefreshlog)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_DESTROY()
	ON_COMMAND(MSG_LINEUP, OnLineup)
	ON_COMMAND(MSG_PAGEUP, OnPageup)
	ON_COMMAND(MSG_PAGEDOWN, OnPagedown)
	ON_COMMAND(MSG_HEAD, OnHead)
	ON_COMMAND(MSG_TAIL, OnTail)
	ON_COMMAND(MSG_LINEDOWN, OnLinedown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventLogViewer message handlers

void CEventLogViewer::OnClose() 
{
	// TODO: Add your control notification handler code here
	EndDialog(0);
}

void CEventLogViewer::OnClearlogfile() 
{
	// TODO: Add your control notification handler code here
	int dlgRet = ::MessageBox(this->GetSafeHwnd(),"Are you sure you want to clear the event log?","Clear Log?",MB_YESNO|MB_ICONQUESTION);
	switch (dlgRet)
	{
		case IDYES:
			m_EventLog.m_csLog.Lock();
			m_EventLog.m_fileEventLog.Close();
			m_EventLog.m_fileEventLog.Open(m_EventLog.m_szFilename, CFile::modeWrite|CFile::modeCreate, NULL);
			m_EventLog.m_fileEventLog.Close();
			m_EventLog.m_fileEventLog.Open(m_EventLog.m_szFilename, CFile::modeReadWrite|CFile::shareDenyNone, NULL);
			m_lFileSize = static_cast<long>(m_EventLog.m_fileEventLog.GetLength());
			m_lCurrentFilePosition = static_cast<long>(m_EventLog.m_fileEventLog.SeekToEnd());
			m_EventLog.m_csLog.Unlock();
			// Update the scrollbar
			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_ALL;
			si.nMin = 0;
			si.nMax = 100;
			si.nPage = 1; //(int)((double)NUM_LOGLINES / (double)m_lFileSize);
			si.nPos = (int)(((double)m_lCurrentFilePosition / (double)m_lFileSize) * 100.0);
			si.nTrackPos = 0;
			((CScrollBar*)GetDlgItem(SB_LOGPOSITION))->SetScrollInfo(&si);
			// Clear the list box
			PopulateLogListBox(m_lCurrentFilePosition);
			break;
	}
}

BOOL CEventLogViewer::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Setup the initial control layouts
	LayoutControls();
	// Load the accelerator table
	m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_VIEWEVENTLOG));

	if (m_EventLog.OpenLogFile(CEventLog::ERead))
	{
		m_EventLog.m_bBeingViewed = true;
		m_lFileSize = static_cast<long>(m_EventLog.m_fileEventLog.GetLength());
		m_lCurrentFilePosition = static_cast<long>(m_EventLog.m_fileEventLog.SeekToEnd());
	}
	else
	{
		m_lFileSize = 0;
		m_lCurrentFilePosition = m_lFileSize;
	}

	// Display the log from the currently selected position
	PopulateLogListBox(m_lCurrentFilePosition);

	// Configure the scroll bar
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = 100;
	si.nPage = (int)((((double)m_lFileSize / 50.0) / (double)m_lFileSize) * 100.0);
	si.nPos = (int)(((double)m_lCurrentFilePosition / (double)m_lFileSize) * 100.0);
	si.nTrackPos = 0;
	((CScrollBar*)GetDlgItem(SB_LOGPOSITION))->SetScrollInfo(&si);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEventLogViewer::OnRefreshlog() 
{
	m_EventLog.m_csLog.Lock();
	m_lCurrentFilePosition = static_cast<long>(m_EventLog.m_fileEventLog.SeekToEnd());
	PopulateLogListBox(m_lCurrentFilePosition);
	m_EventLog.m_csLog.Unlock();

	// Update the scroll bar
	((CScrollBar*)GetDlgItem(SB_LOGPOSITION))->SetScrollPos((int)(((double)m_lCurrentFilePosition / (double)m_lFileSize) * 100.0));
}

void CEventLogViewer::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_lstEvents.m_hWnd))
	{
		LayoutControls();
	}	
}

void CEventLogViewer::LayoutControls()
{
	CRect rectWindow;

	int iScrollBarWidth = 18;

	// Get the size of the dialog box
	GetClientRect(&rectWindow);

	((CScrollBar*)GetDlgItem(SB_LOGPOSITION))->MoveWindow(rectWindow.Width()-GUTTER-iScrollBarWidth, GUTTER, iScrollBarWidth, rectWindow.Height()-BUTTON_HEIGHT-(int)(GUTTER*3.5));

	((CListBox*)GetDlgItem(LST_EVENTS))->MoveWindow(GUTTER, GUTTER, rectWindow.Width()-(GUTTER*2)-iScrollBarWidth, rectWindow.Height()-BUTTON_HEIGHT-(int)(GUTTER*3.5));
	
	((CStatic*)GetDlgItem(IDC_SEPARATOR))->MoveWindow(GUTTER, rectWindow.Height()-BUTTON_HEIGHT-(GUTTER*2), rectWindow.Width()-(GUTTER*2), (int)(GUTTER/2.0));

	((CButton*)GetDlgItem(BTN_CLOSE))->MoveWindow(GUTTER, rectWindow.Height()-BUTTON_HEIGHT-GUTTER, BUTTON_WIDTH, BUTTON_HEIGHT);

	((CButton*)GetDlgItem(BTN_REFRESHLOG))->MoveWindow((int)(rectWindow.Width()/2.0)-(int)(BUTTON_WIDTH/2.0), rectWindow.Height()-BUTTON_HEIGHT-GUTTER, BUTTON_WIDTH, BUTTON_HEIGHT);

	((CButton*)GetDlgItem(BTN_CLEARLOGFILE))->MoveWindow(rectWindow.Width()-BUTTON_WIDTH-GUTTER, rectWindow.Height()-BUTTON_HEIGHT-GUTTER, BUTTON_WIDTH, BUTTON_HEIGHT);

	RedrawWindow();
}

long CEventLogViewer::SeekBackOneLine()
{
	long lFilePosition = 0;
	char b[1];

	lFilePosition = m_lCurrentFilePosition;

	if ((lFilePosition-3) <= 0)
	{
		return static_cast<long>(m_EventLog.m_fileEventLog.Seek(0, CFile::begin));
	}

	lFilePosition = static_cast<long>(m_EventLog.m_fileEventLog.Seek(-3, CFile::current));
	m_EventLog.m_fileEventLog.Read(b, 1);
	while (b[0] != 0x0A)
	{
		if ((lFilePosition-2) <= 0)
		{
			return static_cast<long>(m_EventLog.m_fileEventLog.Seek(0, CFile::begin));
		}
		lFilePosition = static_cast<long>(m_EventLog.m_fileEventLog.Seek(-2, CFile::current));
		m_EventLog.m_fileEventLog.Read(b, 1);
	}
	// File pointer sitting at character after 0x0A

	return lFilePosition;
}

long CEventLogViewer::SeekForeOneLine()
{
	long lFilePosition;
	char b[1];

	lFilePosition = m_lCurrentFilePosition;

	if ((unsigned)(lFilePosition+1) >= m_EventLog.m_fileEventLog.GetLength())
	{
		return SeekBackOneLine();
	}
	m_EventLog.m_fileEventLog.Read(b, 1);
	lFilePosition++;
	while (b[0] != 0x0A)
	{
		if ((unsigned)(lFilePosition+1) >= m_EventLog.m_fileEventLog.GetLength())
		{
			return SeekBackOneLine();
		}
		m_EventLog.m_fileEventLog.Read(b, 1);
		lFilePosition++;
	}
	// File pointer sitting at character after 0x0A
	lFilePosition = static_cast<long>(m_EventLog.m_fileEventLog.Seek(0, CFile::current));

	return lFilePosition;
}

void CEventLogViewer::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CString txt;

	if (pScrollBar->GetSafeHwnd() == GetDlgItem(SB_LOGPOSITION)->GetSafeHwnd())
	{
		if (nSBCode == TB_LINEUP)
		{
			// Line Up
			OnLineup();
		}
		else if (nSBCode == TB_LINEDOWN)
		{
			// Line Down
			OnLinedown();
		}
		else if (nSBCode == TB_PAGEUP)
		{
			// Page Up
			OnPageup();
		}
		else if (nSBCode == TB_PAGEDOWN)
		{
			// Page Down
			OnPagedown();
		}
		else if (nSBCode == SB_THUMBTRACK)
		{
			m_lScrollBarDragPosition = (long)(((double)nPos / 100.0) * (double)m_lFileSize);
			// Update the scroll bar
			pScrollBar->SetScrollPos((int)(((double)m_lCurrentFilePosition / (double)m_lFileSize) * 100.0));
		}
		else if (nSBCode == SB_THUMBPOSITION)
		{
			m_EventLog.m_csLog.Lock();
			m_EventLog.m_fileEventLog.Seek(m_lScrollBarDragPosition, CFile::begin);
			if (nPos > 0)
			{
				m_lCurrentFilePosition = SeekBackOneLine();
			}
			else
			{
				m_lCurrentFilePosition = 0;
			}
			PopulateLogListBox(m_lCurrentFilePosition);
			m_EventLog.m_csLog.Unlock();

			// Update the scroll bar
			pScrollBar->SetScrollPos((int)(((double)m_lCurrentFilePosition / (double)m_lFileSize) * 100.0));
		}
	} // if (pScrollBar->GetSafeHwnd() == GetDlgItem(SB_LOGPOSITION)->GetSafeHwnd())

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CEventLogViewer::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// Close the log file
	m_EventLog.m_fileEventLog.Close(); // .CloseLogFile();
	m_EventLog.m_bBeingViewed = false;
}

BOOL CEventLogViewer::PreTranslateMessage(MSG* pMsg) 
{
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
		{
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CEventLogViewer::OnLineup() 
{
	m_EventLog.m_csLog.Lock();
	m_lCurrentFilePosition = SeekBackOneLine();
	PopulateLogListBox(m_lCurrentFilePosition);
	m_EventLog.m_csLog.Unlock();

	// Update the scroll bar
	((CScrollBar*)GetDlgItem(SB_LOGPOSITION))->SetScrollPos((int)(((double)m_lCurrentFilePosition / (double)m_lFileSize) * 100.0));
}

void CEventLogViewer::OnPageup() 
{
	m_EventLog.m_csLog.Lock();
	for (int iIdx=0; iIdx<NUM_LOGLINES; iIdx++)
	{
		m_lCurrentFilePosition = SeekBackOneLine();
	}			
	PopulateLogListBox(m_lCurrentFilePosition);
	m_EventLog.m_csLog.Unlock();

	// Update the scroll bar
	((CScrollBar*)GetDlgItem(SB_LOGPOSITION))->SetScrollPos((int)(((double)m_lCurrentFilePosition / (double)m_lFileSize) * 100.0));
}

void CEventLogViewer::OnLinedown() 
{
	m_EventLog.m_csLog.Lock();
	m_lCurrentFilePosition = SeekForeOneLine();
	PopulateLogListBox(m_lCurrentFilePosition);
	m_EventLog.m_csLog.Unlock();

	// Update the scroll bar
	((CScrollBar*)GetDlgItem(SB_LOGPOSITION))->SetScrollPos((int)(((double)m_lCurrentFilePosition / (double)m_lFileSize) * 100.0));
}

void CEventLogViewer::OnPagedown() 
{
	m_EventLog.m_csLog.Lock();
	for (int iIdx=0; iIdx<NUM_LOGLINES; iIdx++)
	{
		m_lCurrentFilePosition = SeekForeOneLine();
	}
	PopulateLogListBox(m_lCurrentFilePosition);
	m_EventLog.m_csLog.Unlock();

	// Update the scroll bar
	((CScrollBar*)GetDlgItem(SB_LOGPOSITION))->SetScrollPos((int)(((double)m_lCurrentFilePosition / (double)m_lFileSize) * 100.0));
}

void CEventLogViewer::OnHead() 
{
	m_EventLog.m_csLog.Lock();
	m_lCurrentFilePosition = static_cast<long>(m_EventLog.m_fileEventLog.Seek(0, CFile::begin));
	PopulateLogListBox(m_lCurrentFilePosition);
	m_EventLog.m_csLog.Unlock();

	// Update the scroll bar
	((CScrollBar*)GetDlgItem(SB_LOGPOSITION))->SetScrollPos((int)(((double)m_lCurrentFilePosition / (double)m_lFileSize) * 100.0));
}

void CEventLogViewer::OnTail() 
{
	m_EventLog.m_csLog.Lock();
	m_lCurrentFilePosition = static_cast<long>(m_EventLog.m_fileEventLog.SeekToEnd());
	m_lCurrentFilePosition = SeekBackOneLine();
	PopulateLogListBox(m_lCurrentFilePosition);
	m_EventLog.m_csLog.Unlock();

	// Update the scroll bar
	((CScrollBar*)GetDlgItem(SB_LOGPOSITION))->SetScrollPos((int)(((double)m_lCurrentFilePosition / (double)m_lFileSize) * 100.0));
}


long CEventLogViewer::PopulateLogListBox(long lPtr)
{
	// lPtr is the address of the first character of the last line to be displayed in the listbox
	int i;
	CString txt;

	m_lstEvents.ResetContent();

	// Seek to the given location
	m_EventLog.m_fileEventLog.Seek(lPtr, CFile::begin);

	for (i=0; i<NUM_LOGLINES; i++)
	{
		m_lLowerLimit = SeekBackOneLine();
		if (m_lLowerLimit <= 0)
		{
			break;
		}
	}

	for (i=0; i<NUM_LOGLINES; i++)
	{
		if (m_EventLog.m_fileEventLog.ReadString(txt) != NULL)
		{
			m_lstEvents.AddString(txt);
		}
	}

	// Reposition the file pointer
	m_EventLog.m_fileEventLog.Seek(lPtr, CFile::begin);

	return 0;
}
