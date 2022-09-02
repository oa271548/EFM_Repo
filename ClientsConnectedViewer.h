#pragma once

#include "EFM.h"
#include "EFMdlg.h"

// CClientsConnectedViewer dialog

class CClientsConnectedViewer : public CDialog
{
	DECLARE_DYNAMIC(CClientsConnectedViewer)

public:
	CClientsConnectedViewer(tDATACLIENTSERVER* tDataClientServer, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CClientsConnectedViewer();

// Dialog Data
	enum { IDD = IDD_CLIENTSCONNECTEDVIEWER };

	void DrawListOfClients();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	CListCtrl m_lstConnectedClients;
	tDATACLIENTSERVER* m_tDataClientServer;
	size_t m_nCliensCount;
};
