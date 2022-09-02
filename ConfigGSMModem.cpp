// ConfigGSMModem.cpp : implementation file
//

#include "stdafx.h"
#include "efm.h"
#include "ConfigGSMModem.h"
#include "Configuration.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CConfiguration config;


/////////////////////////////////////////////////////////////////////////////
// CConfigGSMModem dialog


CConfigGSMModem::CConfigGSMModem(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigGSMModem::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigGSMModem)
	m_txtBaudRate = _T("");
	m_txtModemInit = _T("");
	//}}AFX_DATA_INIT
}


void CConfigGSMModem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigGSMModem)
	DDX_Control(pDX, CMB_COMPORT, m_cmbComPort);
	DDX_Text(pDX, IDC_BAUDRATE, m_txtBaudRate);
	DDX_Text(pDX, IDC_MODEMINIT, m_txtModemInit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigGSMModem, CDialog)
	//{{AFX_MSG_MAP(CConfigGSMModem)
	ON_CBN_SELCHANGE(CMB_COMPORT, OnSelchangeComport)
	ON_BN_CLICKED(BTN_ACCEPT, OnAccept)
	ON_BN_CLICKED(BTN_CANCEL, OnCancel)
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigGSMModem message handlers

BOOL CConfigGSMModem::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// save local copies in case we hit cancel
	m_txtBaudRate.Format("%d", config.iSmsGsmBaudRate);
	m_txtModemInit = config.txtSmsGsmModemInit;
	ScanComPorts(); // populate the dropdown list

	m_cmbComPort.SelectString(-1,config.txtSmsGsmComPort);

	UpdateData(FALSE); // load controls fom vars

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigGSMModem::ScanComPorts()
{
	int ctr, idx;
	CString txt;
	CSerial::EPort result;

	// Scan ports 1-99
	for (ctr=1; ctr<=99; ctr++)
	{
		txt.Format("COM%d",ctr);
		result = m_serialCom.CheckPort(CString("\\\\.\\") + txt);
		if ((result == CSerial::EPort::EPortAvailable)||(result == CSerial::EPort::EPortInUse))
		{
			m_cmbComPort.AddString(txt);
		}
		else
		{
			// If we're using the port, CheckPort() will not show it as 
			// available obviously, so we need to manually add it.
			for (idx=0; idx<MAX_NUMOFDATASOURCES; idx++)
			{
				if (config.m_szComPort[idx].CompareNoCase(txt) == 0)
				{
					// It's a port we're using, so add it in.
					m_cmbComPort.AddString(txt);
					// We've found a data source point that uses the 
					// current com port.  We don't care if any 
					// other source points have the same port.
					break;
				}
			}

		}
	}
}


void CConfigGSMModem::OnSelchangeComport() 
{
	// TODO: Add your control notification handler code here
}


void CConfigGSMModem::OnAccept() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true); // load vars from controls

	m_cmbComPort.GetLBText(m_cmbComPort.GetCurSel(), config.txtSmsGsmComPort);
	config.iSmsGsmBaudRate = atoi(m_txtBaudRate);
	config.txtSmsGsmModemInit = m_txtModemInit;

	// Save the configuration
	config.SaveConfiguration();

	EndDialog(1);	

}

void CConfigGSMModem::OnCancel() 
{
	// TODO: Add your control notification handler code here
	EndDialog(-1);	
	
}

