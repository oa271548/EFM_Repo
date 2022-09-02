// OptionsGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "EFM.h"
#include "OptionsGeneral.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CConfiguration config;
extern std::vector <tEFA20DEVICEINFO> vEFA20Devices;

/////////////////////////////////////////////////////////////////////////////
// COptionsGeneral dialog


COptionsGeneral::COptionsGeneral(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsGeneral::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsGeneral)
	m_szNumOfDataSources = _T("");
	m_szMaxDataSources = _T("");
	m_txtDataSourceName = _T("");
	m_iNetworkPort = 0;
	m_iBaudRate = 0;
	m_BDataSourceEnabled = FALSE;
	m_txtNetworkAddress = _T("");
	//}}AFX_DATA_INIT
}


void COptionsGeneral::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsGeneral)
	DDX_Control(pDX, CMB_EFA20SERIALNUMBER, m_cmbEFA20SerialNumber);
	DDX_Control(pDX, CMB_COMPORT, m_cmbComPort);
	DDX_Control(pDX, CMB_DATASOURCEPOINT, m_cmbDataSourcePoint);
	DDX_Control(pDX, CMB_DATASOURCES, m_cmbDataSources);
	DDX_Control(pDX, SPIN_NUMOFDATASOURCES, m_spinNumOfDataSourcesSelect);
	DDX_Text(pDX, LBL_NUMOFDATASOURCES, m_szNumOfDataSources);
	DDX_Text(pDX, LBL_MAXSOURCES, m_szMaxDataSources);
	DDX_Text(pDX, IDC_BAUDRATE, m_iBaudRate);
	DDX_Text(pDX, TXT_DATASOURCENAME, m_txtDataSourceName);
	DDX_Text(pDX, TXT_NETWORKPORT, m_iNetworkPort);
	DDX_Check(pDX, CHK_DATASOURCEENABLED, m_BDataSourceEnabled);
	DDX_Text(pDX, TXT_NETWORKADDRESS, m_txtNetworkAddress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsGeneral, CDialog)
	//{{AFX_MSG_MAP(COptionsGeneral)
	ON_EN_UPDATE(IDC_BAUDRATE, OnUpdateBaudRate)
	ON_NOTIFY(UDN_DELTAPOS, SPIN_NUMOFDATASOURCES, OnDeltaposNumofdatasources)
	ON_CBN_SELCHANGE(CMB_COMPORT, OnSelchangeComport)
	ON_CBN_SELCHANGE(CMB_DATASOURCEPOINT, OnSelchangeDatasourcepoint)
	ON_EN_UPDATE(TXT_DATASOURCENAME, OnUpdateDatasourcename)
	ON_CBN_SELCHANGE(CMB_DATASOURCES, OnSelchangeDatasources)
	ON_EN_UPDATE(TXT_NETWORKPORT, OnUpdateNetworkport)
	ON_BN_CLICKED(CHK_DATASOURCEENABLED, OnDatasourceenabled)
	ON_EN_SETFOCUS(TXT_DATASOURCENAME, OnSetfocusDatasourcename)
	ON_EN_UPDATE(TXT_NETWORKADDRESS, OnUpdateNetworkaddress)
	ON_CBN_SELCHANGE(CMB_EFA20SERIALNUMBER, OnSelchangeEfa20serialnumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsGeneral message handlers

void COptionsGeneral::OnUpdateBaudRate()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_lciBaudRate[m_cmbDataSources.GetCurSel()] = m_iBaudRate;
}


void COptionsGeneral::OnDeltaposNumofdatasources(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);	// Need to get the value

	// Handle the spin button events
	switch (pNMUpDown->iDelta)
	{
		case -1:
			m_lciNumberOfDataSources += 1;	// Increment
			
			break;
		case 1:
			m_lciNumberOfDataSources -= 1;	// Decrement
			break;
	}

	// Apply limits
	(m_lciNumberOfDataSources > MAX_NUMOFDATASOURCES) ? m_lciNumberOfDataSources = MAX_NUMOFDATASOURCES: NULL;
	(m_lciNumberOfDataSources < 1) ? m_lciNumberOfDataSources = 1: NULL;

	if (pNMUpDown->iDelta == 1)
	{
		// If the number of data source points is being decreased, then 
		// disable the previous one.
		if (m_lcBDataSourceEnabled[m_lciNumberOfDataSources] == TRUE)
		{
			m_lcBDataSourceEnabled[m_lciNumberOfDataSources] = FALSE;
		}
	}
	else if (pNMUpDown->iDelta == -1)
	{
		// If the number of data source points is being increased, then 
		// enable the previous one.
		if (m_lcBDataSourceEnabled[m_lciNumberOfDataSources - 1] == FALSE)
		{
			m_lcBDataSourceEnabled[m_lciNumberOfDataSources - 1] = TRUE;
		}
	}

	// Update the label to show the user
	m_szNumOfDataSources.Format("%d",m_lciNumberOfDataSources);

	// Update the Data Sources combo box
	m_cmbDataSources.ResetContent();
	for (size_t ctr = 0; ctr < m_lciNumberOfDataSources; ++ctr)
	{
		m_cmbDataSources.AddString(m_lcszDataSourceName[ctr]);
	}
	// Select first data source
	m_cmbDataSources.SelectString(-1,m_lcszDataSourceName[0]);

	UpdateData(FALSE);		// Reflect the new value in the edit box
	
	*pResult = 0;
}

BOOL COptionsGeneral::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	size_t ctr;
	CString cPort;

	//
	// This function is called once when the dialog is "create"d
	//
	// Make a local working copy of the data in the config object
	m_lciNumberOfDataSources = config.m_iNumberOfDataSources;
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{	m_lcszDataSourceName[ctr] = config.m_szDataSourceName[ctr];	}
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{	m_lcBDataSourceEnabled[ctr] = config.m_BDataSourceEnabled[ctr];	}
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{	m_lcszDataSourcePoint[ctr] = config.m_szDataSourcePoint[ctr];	}
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{	m_lcszComPort[ctr] = config.m_szComPort[ctr];	}
	for (ctr = 0; ctr < MAX_NUMOFDATASOURCES; ctr++)
	{	m_lciBaudRate[ctr] = config.m_iBaudRate[ctr];	}
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{	m_lctxtNetworkAddress[ctr] = config.m_txtNetworkAddress[ctr];	}
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{	m_lciNetworkPort[ctr] = config.m_iNetworkPort[ctr];	}
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{	m_lcszEFA20SerialNumber[ctr] = config.m_txtEFA20SerialNumber[ctr];	}

	// Number of Data Sources
	m_szNumOfDataSources.Format("%d",m_lciNumberOfDataSources);
	// Display maximum number of sources
	m_szMaxDataSources.Format("Max. %d",MAX_NUMOFDATASOURCES);
	
	// Add the data sources to the selection box
	for (ctr=0; ctr<m_lciNumberOfDataSources; ctr++)
	{
		m_cmbDataSources.AddString(m_lcszDataSourceName[ctr]);
	}

	// Preload the selectable source points
	CString txt;
	txt.LoadString(STR_SOURCEPOINT_EFA20);
	m_cmbDataSourcePoint.AddString(txt);
	txt.LoadString(STR_SOURCEPOINT_NETWORK);
	m_cmbDataSourcePoint.AddString(txt);
	txt.LoadString(STR_SOURCEPOINT_SERIAL);
	m_cmbDataSourcePoint.AddString(txt);
	//txt.LoadString(STR_SOURCEPOINT_LWS200);
	//m_cmbDataSourcePoint.AddString(txt);
	//txt.LoadString(STR_SOURCEPOINT_LWS300);
	//m_cmbDataSourcePoint.AddString(txt);

	// Populate the comport combo box
	ScanComPorts();

	// Populate the EFA20 combo box
	EFA20_DetectConnectedDevices();

	// Populate the EFA20 Serial Number drop list with available serial numbers
	for (auto& vEFA20Device : vEFA20Devices)
	{
		m_cmbEFA20SerialNumber.AddString(vEFA20Device.txtSerialNumber);
	}

	// Select first data source and load its settings
	m_cmbDataSources.SelectString(-1,m_lcszDataSourceName[0]);
	m_BDataSourceEnabled = m_lcBDataSourceEnabled[0];
	m_txtDataSourceName = m_lcszDataSourceName[0];
	m_cmbDataSourcePoint.SelectString(-1,m_lcszDataSourcePoint[0]);
	m_cmbComPort.SelectString(-1,m_lcszComPort[0]);
	m_iBaudRate = m_lciBaudRate[0];
	m_txtNetworkAddress = m_lctxtNetworkAddress[0];
	m_iNetworkPort = m_lciNetworkPort[0];
	m_cmbEFA20SerialNumber.SelectString(-1, m_lcszEFA20SerialNumber[0]);

	// Enable/Disable the appropriate controls
	m_cmbDataSourcePoint.GetLBText(m_cmbDataSourcePoint.GetCurSel(), cPort);
	if (cPort.CompareNoCase("Serial") == 0)
	{
		(GetDlgItem(TXT_NETWORKADDRESS))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKPORT))->EnableWindow(FALSE);
		(GetDlgItem(CMB_EFA20SERIALNUMBER))->EnableWindow(FALSE);
		(GetDlgItem(CMB_COMPORT))->EnableWindow(TRUE);
		(GetDlgItem(IDC_BAUDRATE))->EnableWindow(TRUE);
	}
	else if (cPort.CompareNoCase("Network") == 0)
	{
		(GetDlgItem(CMB_COMPORT))->EnableWindow(FALSE);
		(GetDlgItem(IDC_BAUDRATE))->EnableWindow(FALSE);
		(GetDlgItem(CMB_EFA20SERIALNUMBER))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKADDRESS))->EnableWindow(TRUE);
		(GetDlgItem(TXT_NETWORKPORT))->EnableWindow(TRUE);
	}
	else if (cPort.CompareNoCase("EFA-20/21") == 0)
	{
		(GetDlgItem(CMB_COMPORT))->EnableWindow(FALSE);
		(GetDlgItem(IDC_BAUDRATE))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKADDRESS))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKPORT))->EnableWindow(FALSE);
		(GetDlgItem(CMB_EFA20SERIALNUMBER))->EnableWindow(TRUE);
	}

	UpdateData(FALSE);

	// Display EFM-200 Firmware Version if set
	if (!((CEFMDlg*)AfxGetMainWnd())->tThInfo[m_cmbDataSources.GetCurSel()].EfmFirmwareVersion.IsEmpty())
	{
		CString str; 
		str = "EFM Firmware Version:    " + 
			((CEFMDlg*)AfxGetMainWnd())->tThInfo[m_cmbDataSources.GetCurSel()].EfmFirmwareVersion;
		(GetDlgItem(IDC_FIRMWARE_VERSION))->SetWindowText(str);
	}
	else
	{
		(GetDlgItem(IDC_FIRMWARE_VERSION))->SetWindowText("                                         ");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsGeneral::ScanComPorts()
{
	int ctr, idx;
	CString txt;

	// Scan ports 1-99
	for (ctr=1; ctr<=99; ctr++)
	{
		txt.Format("COM%d",ctr);
		if (m_serialCom.CheckPort(CString("\\\\.\\") + txt) == CSerial::EPort::EPortAvailable)
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

void COptionsGeneral::EFA20_DetectConnectedDevices()
{
	DWORD dwNumDevs = 0;
	FT_DEVICE_LIST_INFO_NODE* devInfo = nullptr;
	FT_STATUS ftStatus;

	// Clear devices list
	vEFA20Devices.clear();

	// Build a device information list
	ftStatus = FT_CreateDeviceInfoList(&dwNumDevs);
	if (ftStatus == FT_OK)
	{
		if (dwNumDevs > 0)
		{
			// Create enough space in memory for the number of connected devices
			devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE) * dwNumDevs);
			if (devInfo)
			{
				// Get the device information list 
				ftStatus = FT_GetDeviceInfoList(devInfo, &dwNumDevs);
				if (ftStatus == FT_OK)
				{
					for (int i = 0; i < static_cast<int>(dwNumDevs); i++)
					{
						if ((((devInfo[i].ID >> 16) & 0x0000FFFF) == EFA20_VID) &&
							((devInfo[i].ID & 0x0000FFFF) == EFA20_PID))
						{
							// This enumerated device is an EFA20
							tEFA20DEVICEINFO efa20;
							efa20.iFlags = devInfo[i].Flags;
							efa20.iType = devInfo[i].Type;
							efa20.iID = devInfo[i].ID;
							efa20.iLocID = devInfo[i].LocId;
							efa20.txtSerialNumber = devInfo[i].SerialNumber;
							efa20.txtDescription = devInfo[i].Description;
							efa20.ftHandle = devInfo[i].ftHandle;
							// Add it to the devices list
							vEFA20Devices.push_back(efa20);
						}
					} // for (int i=0; i<dwNumDevs; i++)
				} // if (ftStatus == FT_OK)

				// Delete the dynamic memory we allocated for the devices
				free(devInfo);
				devInfo = NULL;

			} // if (devInfo)
		} // if (dwNumDevs > 0)
	} // if (ftStatus == FT_OK)
}



void COptionsGeneral::OnSelchangeComport() 
{
	// TODO: Add your control notification handler code here
	CString sPoint;

	m_cmbComPort.GetLBText(m_cmbComPort.GetCurSel(), sPoint);
	m_lcszComPort[m_cmbDataSources.GetCurSel()] = sPoint;
	m_lciBaudRate[m_cmbDataSources.GetCurSel()] = m_iBaudRate;
}

void COptionsGeneral::OnSelchangeDatasourcepoint() 
{
	CString sPoint;
	CString txtDSSerialUSB, txtDSNetwork, txtDSLWS200, txtDSLWS300, txtDSEFA20;

	txtDSSerialUSB.LoadString(STR_SOURCEPOINT_SERIAL);
	txtDSNetwork.LoadString(STR_SOURCEPOINT_NETWORK);
	txtDSEFA20.LoadString(STR_SOURCEPOINT_EFA20);
	txtDSLWS200.LoadString(STR_SOURCEPOINT_LWS200);
	txtDSLWS300.LoadString(STR_SOURCEPOINT_LWS300);

	UpdateData(TRUE);

	// Enable/Disable the appropriate controls
	m_cmbDataSourcePoint.GetLBText(m_cmbDataSourcePoint.GetCurSel(), sPoint);
	if (sPoint.CompareNoCase(txtDSSerialUSB) == 0)
	{
		(GetDlgItem(TXT_NETWORKADDRESS))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKPORT))->EnableWindow(FALSE);
		(GetDlgItem(CMB_EFA20SERIALNUMBER))->EnableWindow(FALSE);
		(GetDlgItem(CMB_COMPORT))->EnableWindow(TRUE);
		(GetDlgItem(IDC_BAUDRATE))->EnableWindow(TRUE);
	}
	else if (sPoint.CompareNoCase(txtDSNetwork) == 0)
	{
		(GetDlgItem(CMB_COMPORT))->EnableWindow(FALSE);
		(GetDlgItem(IDC_BAUDRATE))->EnableWindow(FALSE);
		(GetDlgItem(CMB_EFA20SERIALNUMBER))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKADDRESS))->EnableWindow(TRUE);
		(GetDlgItem(TXT_NETWORKPORT))->EnableWindow(TRUE);
	}
	else if (sPoint.CompareNoCase(txtDSEFA20) == 0)
	{
		(GetDlgItem(CMB_COMPORT))->EnableWindow(FALSE);
		(GetDlgItem(IDC_BAUDRATE))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKADDRESS))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKPORT))->EnableWindow(FALSE);
		(GetDlgItem(CMB_EFA20SERIALNUMBER))->EnableWindow(TRUE);
	}
	else if (sPoint.CompareNoCase(txtDSLWS200) == 0)
	{
		(GetDlgItem(CMB_COMPORT))->EnableWindow(FALSE);
		(GetDlgItem(IDC_BAUDRATE))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKADDRESS))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKPORT))->EnableWindow(FALSE);
		(GetDlgItem(CMB_EFA20SERIALNUMBER))->EnableWindow(FALSE);
	}
	else if (sPoint.CompareNoCase(txtDSLWS300) == 0)
	{
		(GetDlgItem(CMB_COMPORT))->EnableWindow(FALSE);
		(GetDlgItem(IDC_BAUDRATE))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKADDRESS))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKPORT))->EnableWindow(FALSE);
		(GetDlgItem(CMB_EFA20SERIALNUMBER))->EnableWindow(FALSE);
	}

	// Set the new data source point
	m_lcszDataSourcePoint[m_cmbDataSources.GetCurSel()] = sPoint;

}

void COptionsGeneral::OnUpdateDatasourcename() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	
	// Save what has been typed in thus far
	m_lcszDataSourceName[m_iDataSourceSelected] = m_txtDataSourceName;

	m_cmbDataSources.DeleteString(m_iDataSourceSelected);
	m_cmbDataSources.InsertString(m_iDataSourceSelected,m_lcszDataSourceName[m_iDataSourceSelected]);
	m_cmbDataSources.SetCurSel(m_iDataSourceSelected);

	UpdateData(FALSE);
}

void COptionsGeneral::OnSelchangeDatasources() 
{
	// TODO: Add your control notification handler code here
	CString sPoint;

	m_txtDataSourceName = m_lcszDataSourceName[m_cmbDataSources.GetCurSel()];
	m_BDataSourceEnabled = m_lcBDataSourceEnabled[m_cmbDataSources.GetCurSel()];
	m_cmbDataSourcePoint.SelectString(-1,m_lcszDataSourcePoint[m_cmbDataSources.GetCurSel()]);
	m_cmbComPort.SelectString(-1,m_lcszComPort[m_cmbDataSources.GetCurSel()]);
	m_iBaudRate = m_lciBaudRate[m_cmbDataSources.GetCurSel()];
	m_txtNetworkAddress = m_lctxtNetworkAddress[m_cmbDataSources.GetCurSel()];
	m_iNetworkPort = m_lciNetworkPort[m_cmbDataSources.GetCurSel()];
	m_cmbEFA20SerialNumber.SelectString(-1, m_lcszEFA20SerialNumber[m_cmbDataSources.GetCurSel()]);

	m_cmbDataSourcePoint.GetLBText(m_cmbDataSourcePoint.GetCurSel(), sPoint);
	if (sPoint.CompareNoCase("Serial") == 0)
	{
		(GetDlgItem(TXT_NETWORKADDRESS))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKPORT))->EnableWindow(FALSE);
		(GetDlgItem(CMB_EFA20SERIALNUMBER))->EnableWindow(FALSE);
		(GetDlgItem(CMB_COMPORT))->EnableWindow(TRUE);
		(GetDlgItem(IDC_BAUDRATE))->EnableWindow(TRUE);
	}
	else if (sPoint.CompareNoCase("Network") == 0)
	{
		(GetDlgItem(CMB_COMPORT))->EnableWindow(FALSE);
		(GetDlgItem(IDC_BAUDRATE))->EnableWindow(FALSE);
		(GetDlgItem(CMB_EFA20SERIALNUMBER))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKADDRESS))->EnableWindow(TRUE);
		(GetDlgItem(TXT_NETWORKPORT))->EnableWindow(TRUE);
	}
	else if (sPoint.CompareNoCase("EFA-20/21") == 0)
	{
		(GetDlgItem(CMB_COMPORT))->EnableWindow(FALSE);
		(GetDlgItem(IDC_BAUDRATE))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKADDRESS))->EnableWindow(FALSE);
		(GetDlgItem(TXT_NETWORKPORT))->EnableWindow(FALSE);
		(GetDlgItem(CMB_EFA20SERIALNUMBER))->EnableWindow(TRUE);
	}

	// Display EFM-200 Firmware Version if set
	if (!((CEFMDlg*)AfxGetMainWnd())->tThInfo[m_cmbDataSources.GetCurSel()].EfmFirmwareVersion.IsEmpty())
	{
		CString str; 
		str = "EFM-200 Firmware Version:  " + 
			((CEFMDlg*)AfxGetMainWnd())->tThInfo[m_cmbDataSources.GetCurSel()].EfmFirmwareVersion;
		(GetDlgItem(IDC_FIRMWARE_VERSION))->SetWindowText(str);
	}
	else
	{
		(GetDlgItem(IDC_FIRMWARE_VERSION))->SetWindowText("                                         ");
	}


	UpdateData(FALSE);
}

void COptionsGeneral::OnUpdateNetworkport() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_lciNetworkPort[m_cmbDataSources.GetCurSel()] = m_iNetworkPort;
}



BOOL COptionsGeneral::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if ((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) ||
		(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE))
	{
		return TRUE; // just ignore the enter key
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void COptionsGeneral::OnDatasourceenabled() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_lcBDataSourceEnabled[m_cmbDataSources.GetCurSel()] = m_BDataSourceEnabled;
}

void COptionsGeneral::OnSetfocusDatasourcename() 
{
	// TODO: Add your control notification handler code here

	// Save the index of the item we're modifying
	m_iDataSourceSelected = m_cmbDataSources.GetCurSel();

}

void COptionsGeneral::OnUpdateNetworkaddress() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	m_lctxtNetworkAddress[m_cmbDataSources.GetCurSel()] = m_txtNetworkAddress;
}


void COptionsGeneral::OnSelchangeEfa20serialnumber() 
{
	CString sn;

	m_cmbEFA20SerialNumber.GetLBText(m_cmbEFA20SerialNumber.GetCurSel(), sn);
	m_lcszEFA20SerialNumber[m_cmbDataSources.GetCurSel()] = sn;
}
