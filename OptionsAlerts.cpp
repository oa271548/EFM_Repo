// OptionsAlerts.cpp : implementation file
//

#include "stdafx.h"
#include "efm.h"
#include "EFMOptions.h"
#include "OptionsAlerts.h"
#include "ConfigEMailServer.h"
#include "ConfigGSMModem.h"
#include "ConfigGSMGateway.h"
#include "EMailRecipients.h"
#include "SMSRecipients.h"
#include "AddEditRecipient.h" // dialog to enter email & sms
#include "Alerts.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CConfiguration config;
extern CEFMOptions * dlgOptions;
extern CAlerts Alerts;  // email and sms class



/////////////////////////////////////////////////////////////////////////////
// COptionsAlerts dialog


COptionsAlerts::COptionsAlerts(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsAlerts::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsAlerts)
	m_bAlertsDAActive = FALSE;
	m_bAlertsDAClear = FALSE;
	m_bAlertsSmtpEnabled = FALSE;
	m_bAlertsFaultsClear = FALSE;
	m_bAlertsHFActive = FALSE;
	m_bAlertsHFClear = FALSE;
	m_bAlertsLAActive = FALSE;
	m_bAlertsLAClear = FALSE;
	m_bAlertsSmsGatewayEnabled = FALSE;
	m_bAlertsSmsGsmEnabled = FALSE;
	m_bAlertsVHFActive = FALSE;
	m_bAlertsVHFClear = FALSE;
	m_bAlertsFaultsActive = FALSE;
	m_bAlertsScheduled = FALSE;
	//}}AFX_DATA_INIT
}


void COptionsAlerts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsAlerts)
	DDX_Control(pDX, CMB_AL_DATASOURCENAMES, m_cmbDataSourceNames);
	DDX_Check(pDX, CHK_DA_ACTIVE, m_bAlertsDAActive);
	DDX_Check(pDX, CHK_DA_CLEAR, m_bAlertsDAClear);
	DDX_Check(pDX, CHK_EMAILALERT, m_bAlertsSmtpEnabled);
	DDX_Check(pDX, CHK_FAULTS_CLEAR, m_bAlertsFaultsClear);
	DDX_Check(pDX, CHK_HF_ACTIVE, m_bAlertsHFActive);
	DDX_Check(pDX, CHK_HF_CLEAR, m_bAlertsHFClear);
	DDX_Check(pDX, CHK_LA_ACTIVE, m_bAlertsLAActive);
	DDX_Check(pDX, CHK_LA_CLEAR, m_bAlertsLAClear);
	DDX_Check(pDX, CHK_SMSGATEWAYALERT, m_bAlertsSmsGatewayEnabled);
	DDX_Check(pDX, CHK_SMSGSMALERT, m_bAlertsSmsGsmEnabled);
	DDX_Check(pDX, CHK_VHF_ACTIVE, m_bAlertsVHFActive);
	DDX_Check(pDX, CHK_VHF_CLEAR, m_bAlertsVHFClear);
	DDX_Check(pDX, CHK_FAULTS_ACTIVE, m_bAlertsFaultsActive);
	DDX_DateTimeCtrl(pDX, IDC_SCHEDULEDTIMEPICKER, m_tAlertsScheduledTime);
	DDX_Check(pDX, CHK_SCHEDULED, m_bAlertsScheduled);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EMAILLIST, m_ctlEmailList); // outside of datamap
	DDX_Control(pDX, IDC_SMSLIST, m_ctlSmsList);		// outside of datamap
}


BEGIN_MESSAGE_MAP(COptionsAlerts, CDialog)
	//{{AFX_MSG_MAP(COptionsAlerts)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(BTN_EMAILCONFIGURE, OnEmailconfigure)
	ON_BN_CLICKED(BTN_SMSCONFIGURE, OnSmsconfigure)
	ON_CBN_SELCHANGE(CMB_AL_DATASOURCENAMES, OnSelChangeAlertsDatasourcenames)
	ON_CLBN_CHKCHANGE(IDC_EMAILLIST, OnCheckChangeEmailList)
	ON_LBN_DBLCLK(IDC_EMAILLIST, OnDblclkEmaillist)
	ON_LBN_DBLCLK(IDC_SMSLIST, OnDblclkSmslist)
	ON_BN_CLICKED(IDC_ADD_EMAIL, OnAddEmail)
	ON_BN_CLICKED(IDC_EDIT_EMAIL, OnEditEmail)
	ON_BN_CLICKED(IDC_DELETE_EMAIL, OnDeleteEmail)
	ON_BN_CLICKED(IDC_ADD_SMS, OnAddSms)
	ON_BN_CLICKED(IDC_EDIT_SMS, OnEditSms)
	ON_BN_CLICKED(IDC_DELETE_SMS, OnDeleteSms)
	ON_BN_CLICKED(IDC_EMAILTEST, OnEmailSendTestMessage)
	ON_BN_CLICKED(IDC_SMSGSMTEST, OnSmsGsmTestMessage)
	ON_BN_CLICKED(IDC_SMSGATEWAYTEST, OnSmsGatewayTestMessage)
	ON_BN_CLICKED(BTN_SMSCONFIGURE2, OnSmsGatewayConfigure)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsAlerts message handlers

BOOL COptionsAlerts::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	int ctr;

	// Bitmaps onto buttons
	m_bmpConfigure.LoadBitmap(BMP_CONFIGURE);
	CButton * pButton = (CButton*)GetDlgItem(BTN_EMAILCONFIGURE);
	pButton->SetBitmap(m_bmpConfigure);
	pButton = (CButton*)GetDlgItem(BTN_SMSCONFIGURE);
	pButton->SetBitmap(m_bmpConfigure);
	pButton = (CButton*)GetDlgItem(BTN_SMSCONFIGURE2);
	pButton->SetBitmap(m_bmpConfigure);
	
	// Scheduled Alert Time Picker to 24 hour time
	((CDateTimeCtrl*)GetDlgItem(IDC_SCHEDULEDTIMEPICKER))->SetFormat("HH:mm:ss");

	m_lciNumberOfDataSources = config.m_iNumberOfDataSources;

	// Populate the local variable copies
	// SMTP
	if (config.txtSmtpServer.GetLength() > 0)
	{
		GetDlgItem(LBL_SMTPSERVER)->SetWindowText(config.txtSmtpServer);
	}

	// SMS
	if (config.txtSmsGatewayUrl.GetLength() > 0)
	{
		GetDlgItem(LBL_SMSGATEWAY)->SetWindowText(config.txtSmsGatewayUrl);
	}

	if (config.txtSmsGsmComPort.GetLength() > 0)
	{
		GetDlgItem(LBL_SMSGSMMODEM)->SetWindowText(config.txtSmsGsmComPort);
	}

	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		m_laszDataSourceName[ctr] = config.m_szDataSourceName[ctr];

		// Events
		m_lcbAlertsLAActive[ctr] = config.bAlertsLAActive[ctr];
		m_lcbAlertsLAClear[ctr] = config.bAlertsLAClear[ctr];
		m_lcbAlertsHFActive[ctr] = config.bAlertsHFActive[ctr];
		m_lcbAlertsHFClear[ctr] = config.bAlertsHFClear[ctr];
		m_lcbAlertsVHFActive[ctr] = config.bAlertsVHFActive[ctr];
		m_lcbAlertsVHFClear[ctr] = config.bAlertsVHFClear[ctr];
		m_lcbAlertsDAActive[ctr] = config.bAlertsDAActive[ctr];
		m_lcbAlertsDAClear[ctr] = config.bAlertsDAClear[ctr];
		m_lcbAlertsFaultsActive[ctr] = config.bAlertsFaultsActive[ctr];
		m_lcbAlertsFaultsClear[ctr] = config.bAlertsFaultsClear[ctr];
		m_lcbAlertsScheduled[ctr] = config.bAlertsScheduled[ctr];
		m_lctAlertsScheduledTime[ctr] = CTime(2000, 1, 1,
												config.iAlertsScheduledHour[ctr],
												config.iAlertsScheduledMins[ctr],
												config.iAlertsScheduledSecs[ctr]);

		// Email & GSM enables
		m_lcbEmailEnabled[ctr] = config.bSmtpEnabled[ctr];
		m_lcbSmsGsmEnabled[ctr] = config.bSmsGsmEnabled[ctr];
		m_lcbSmsGatewayEnabled[ctr] = config.bSmsGatewayEnabled[ctr];

		for (int iRcp=0; iRcp<MAX_RECIPIENTS; iRcp++)
		{
			m_lcbEmailRecipientEnabled[ctr][iRcp] = config.bEmailRecipientEnabled[ctr][iRcp]; 
			m_lctxtEmailRecipient[ctr][iRcp] = config.txtEmailRecipient[ctr][iRcp]; // email addresses
			m_lctxtEmailRecipientName[ctr][iRcp] = config.txtEmailRecipientName[ctr][iRcp]; // comment person's name


			m_lcbSmsRecipientEnabled[ctr][iRcp] = config.bSmsRecipientEnabled[ctr][iRcp]; 
			m_lctxtSmsRecipient[ctr][iRcp] = config.txtSmsRecipient[ctr][iRcp]; // phone numbers
			m_lctxtSmsRecipientName[ctr][iRcp] = config.txtSmsRecipientName[ctr][iRcp]; // comment person's name  
		}

	}

	for (ctr=0; ctr<m_lciNumberOfDataSources; ctr++)
	{
		m_cmbDataSourceNames.AddString(m_laszDataSourceName[ctr]);
	}

	// Select the first item
	m_iCurrentSelectedDataSource = 0;
	m_cmbDataSourceNames.SelectString(-1,m_laszDataSourceName[m_iCurrentSelectedDataSource]);

	LoadRecipientListboxes();
	LoadCheckboxes();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
} // OnInitDialog()

void COptionsAlerts::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	int ctr;
	CString txt;

	dlgOptions->dlgGeneral.UpdateData(TRUE); // load the data source names

	switch (bShow)
	{
		case 1:	// Being Shown
			// Clear the Data Source Names combo box
			m_cmbDataSourceNames.ResetContent();
			// Update the number of data sources from the general box
			m_lciNumberOfDataSources = dlgOptions->dlgGeneral.m_lciNumberOfDataSources;
			// Update the local name array with the names from the general box
			for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
			{	
				m_laszDataSourceName[ctr] = dlgOptions->dlgGeneral.m_lcszDataSourceName[ctr];	
			}
			// Repopulate the Data Source Names combo box
			for (ctr=0; ctr<m_lciNumberOfDataSources; ctr++)
			{
				m_cmbDataSourceNames.AddString(m_laszDataSourceName[ctr]);
			}
			// Show the settings for the currently selected item
			m_iCurrentSelectedDataSource = 0;
			m_cmbDataSourceNames.SelectString(-1,m_laszDataSourceName[m_iCurrentSelectedDataSource]);

			OnSelChangeAlertsDatasourcenames();

			UpdateData(FALSE);
			
			break;
		case 0: // Being Hidden
			break;
	}
	
}



void COptionsAlerts::OnEmailconfigure() 
{
	// TODO: Add your control notification handler code here
	CConfigEMailServer dlgConfEMail(this);
	dlgConfEMail.DoModal();
	
	GetDlgItem(LBL_SMTPSERVER)->SetWindowText(config.txtSmtpServer);
}


void COptionsAlerts::OnSmsconfigure() 
{
	// TODO: Add your control notification handler code here
	CConfigGSMModem dlgConfGSM;
	dlgConfGSM.DoModal();	

	GetDlgItem(LBL_SMSGSMMODEM)->SetWindowText(config.txtSmsGsmComPort);
}


void COptionsAlerts::OnSmsGatewayConfigure() 
{
	// TODO: Add your control notification handler code here
	CConfigGSMGateway dlgConfGateway;
	dlgConfGateway.DoModal();	

	GetDlgItem(LBL_SMSGATEWAY)->SetWindowText(config.txtSmsGatewayUrl);
}


void COptionsAlerts::OnEmailrecipients() 
{
	// TODO: Add your control notification handler code here
	CEMailRecipients dlgEMailRecip;
	dlgEMailRecip.DoModal();	
}


void COptionsAlerts::OnSmsrecipients() 
{
	// TODO: Add your control notification handler code here
	CSMSRecipients dlgSMSRecip;
	dlgSMSRecip.DoModal();
}

void COptionsAlerts::LoadCheckboxes()
{
	// load checkboxes for active / clear notifications
	m_bAlertsLAActive  = m_lcbAlertsLAActive[m_iCurrentSelectedDataSource];
	m_bAlertsLAClear   = m_lcbAlertsLAClear [m_iCurrentSelectedDataSource];
	m_bAlertsHFActive  = m_lcbAlertsHFActive[m_iCurrentSelectedDataSource];
	m_bAlertsHFClear   = m_lcbAlertsHFClear [m_iCurrentSelectedDataSource];
	m_bAlertsVHFActive = m_lcbAlertsVHFActive[m_iCurrentSelectedDataSource];
	m_bAlertsVHFClear  = m_lcbAlertsVHFClear [m_iCurrentSelectedDataSource];
	m_bAlertsDAActive  = m_lcbAlertsDAActive[m_iCurrentSelectedDataSource];
	m_bAlertsDAClear   = m_lcbAlertsDAClear [m_iCurrentSelectedDataSource];
	m_bAlertsFaultsActive = m_lcbAlertsFaultsActive[m_iCurrentSelectedDataSource];
	m_bAlertsFaultsClear  = m_lcbAlertsFaultsClear [m_iCurrentSelectedDataSource];
	m_bAlertsScheduled = m_lcbAlertsScheduled[m_iCurrentSelectedDataSource];
	m_tAlertsScheduledTime = m_lctAlertsScheduledTime[m_iCurrentSelectedDataSource];
	// load checkboxes for email, sms global enable
	m_bAlertsSmtpEnabled = m_lcbEmailEnabled[m_iCurrentSelectedDataSource];
	m_bAlertsSmsGsmEnabled = m_lcbSmsGsmEnabled[m_iCurrentSelectedDataSource];
	m_bAlertsSmsGatewayEnabled = m_lcbSmsGatewayEnabled[m_iCurrentSelectedDataSource];
}

void COptionsAlerts::SaveCheckboxes()
{
	// save checkboxes for active / clear notifications
	m_lcbAlertsLAActive[m_iCurrentSelectedDataSource] = m_bAlertsLAActive;
	m_lcbAlertsLAClear [m_iCurrentSelectedDataSource] = m_bAlertsLAClear;
	m_lcbAlertsHFActive[m_iCurrentSelectedDataSource] = m_bAlertsHFActive;
	m_lcbAlertsHFClear [m_iCurrentSelectedDataSource] = m_bAlertsHFClear;
	m_lcbAlertsVHFActive[m_iCurrentSelectedDataSource] = m_bAlertsVHFActive;
	m_lcbAlertsVHFClear [m_iCurrentSelectedDataSource] = m_bAlertsVHFClear;
	m_lcbAlertsDAActive[m_iCurrentSelectedDataSource] = m_bAlertsDAActive;
	m_lcbAlertsDAClear [m_iCurrentSelectedDataSource] = m_bAlertsDAClear;
	m_lcbAlertsFaultsActive[m_iCurrentSelectedDataSource] = m_bAlertsFaultsActive ;
	m_lcbAlertsFaultsClear [m_iCurrentSelectedDataSource] = m_bAlertsFaultsClear;
	m_lcbAlertsScheduled [m_iCurrentSelectedDataSource] = m_bAlertsScheduled;
	m_lctAlertsScheduledTime [m_iCurrentSelectedDataSource] = m_tAlertsScheduledTime;

	// save checkboxes for email, sms global enable
	m_lcbEmailEnabled[m_iCurrentSelectedDataSource] = m_bAlertsSmtpEnabled;
	m_lcbSmsGsmEnabled[m_iCurrentSelectedDataSource] = m_bAlertsSmsGsmEnabled;
	m_lcbSmsGatewayEnabled[m_iCurrentSelectedDataSource] = m_bAlertsSmsGatewayEnabled;
}

void COptionsAlerts::SaveRecipientListboxes()
{
	int ctr;

	// Save the current email and sms list box data before we delete it and repopulate 
	// Email
	for (ctr=0; ctr< m_ctlEmailList.GetCount(); ctr++)
	{
		CString str;
		m_ctlEmailList.GetText(ctr, str);
		m_lctxtEmailRecipientName[m_iCurrentSelectedDataSource][ctr] = str.Left(str.Find("\t"));
		m_lctxtEmailRecipient[m_iCurrentSelectedDataSource][ctr] = str.Mid(str.Find("\t")+1); // skip tab deimeter
		m_lcbEmailRecipientEnabled[m_iCurrentSelectedDataSource][ctr] = m_ctlEmailList.GetCheck(ctr);
	}
	for (     ; ctr < MAX_RECIPIENTS; ctr++)
	{
		m_lctxtEmailRecipient[m_iCurrentSelectedDataSource][ctr] = "";
		m_lcbEmailRecipientEnabled[m_iCurrentSelectedDataSource][ctr] = 0;
	}

	// SMS
	for (ctr=0; ctr< m_ctlSmsList.GetCount(); ctr++)
	{
		CString str;
		m_ctlSmsList.GetText(ctr, str);
		m_lctxtSmsRecipientName[m_iCurrentSelectedDataSource][ctr] = str.Left(str.Find("\t"));
		m_lctxtSmsRecipient[m_iCurrentSelectedDataSource][ctr] = str.Mid(str.Find("\t")+1); // skip tab deimeter
		m_lcbSmsRecipientEnabled[m_iCurrentSelectedDataSource][ctr] = m_ctlSmsList.GetCheck(ctr);
	}
	for (     ; ctr < MAX_RECIPIENTS; ctr++)
	{
		m_lctxtSmsRecipient[m_iCurrentSelectedDataSource][ctr] = "";
		m_lcbSmsRecipientEnabled[m_iCurrentSelectedDataSource][ctr] = 0;
	}

} // SaveRecipientListboxes()

// For CTabCheckListBox font size determination
int COptionsAlerts::GetDialogUnitNumber()
{
	CRect rectList;
	m_ctlEmailList.GetWindowRect(&rectList);
	CFont* pFont = m_ctlEmailList.GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	int nBaseUnit = lf.lfWidth;
	if(nBaseUnit == 0)
		nBaseUnit = LOWORD(GetDialogBaseUnits());
	return (rectList.Width() *4) / nBaseUnit;
}


void COptionsAlerts::LoadRecipientListboxes()
{
	// Load the listboxes with current SMS and email recipients
	// *** for now we are only using the recipients phone number or email address.
	// *** ideally we would have two columns with a friendly name also
	int ctr;
	m_ctlEmailList.ResetContent();
	m_ctlSmsList.ResetContent();

	// Set Tab Stops
	int nTabStop = GetDialogUnitNumber() / 7; 
	LPINT lpTabStop = new int[2];
	lpTabStop[0] = nTabStop * 3;
	lpTabStop[1] = nTabStop * 4;
	m_ctlEmailList.SetTabStops(2, lpTabStop);
	m_ctlSmsList.SetTabStops(2, lpTabStop);
	delete []lpTabStop;


	for (ctr=0; ctr<MAX_RECIPIENTS; ctr++)
	{
		if (strlen(m_lctxtEmailRecipient[m_iCurrentSelectedDataSource][ctr]))
		{
			CString str;
			str.Format(m_lctxtEmailRecipientName[m_iCurrentSelectedDataSource][ctr] + CString("\t") +
					   m_lctxtEmailRecipient[m_iCurrentSelectedDataSource][ctr]);
			int pos = m_ctlEmailList.AddString(str);
			m_ctlEmailList.SetCheck(pos, m_lcbEmailRecipientEnabled[m_iCurrentSelectedDataSource][ctr]);
		} // if strlen()
		if (strlen(m_lctxtSmsRecipient[m_iCurrentSelectedDataSource][ctr]))
		{
			CString str;
			str.Format(m_lctxtSmsRecipientName[m_iCurrentSelectedDataSource][ctr] + CString("\t") +
					   m_lctxtSmsRecipient[m_iCurrentSelectedDataSource][ctr]);
			int pos = m_ctlSmsList.AddString(str);
			m_ctlSmsList.SetCheck(pos, m_lcbSmsRecipientEnabled[m_iCurrentSelectedDataSource][ctr]);
		} // if strlen()
	}
} // LoadRecipientListboxes()

//For Lighning Rings?
//http://www.codeproject.com/Articles/1796/XListCtrl-A-custom-draw-list-control-with-subitem


void COptionsAlerts::OnSelChangeAlertsDatasourcenames() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// copy controls to mem vars 
	SaveRecipientListboxes();
	SaveCheckboxes();

	// Change us over to the new data source (EFM) selection
	m_iCurrentSelectedDataSource = m_cmbDataSourceNames.GetCurSel();

	LoadRecipientListboxes();
	LoadCheckboxes();

	UpdateData(FALSE);	// copy mem vars to controls

} // OnSelChangeAlertsDatasourcenames()


void COptionsAlerts::OnCheckChangeEmailList()
{

	TRACE("void COptionsAlerts::OnCheckChangeEmailList()\n");


}

BOOL COptionsAlerts::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if ((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) ||
		(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE))
	{
		return TRUE; // just ignore the enter key
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}


void COptionsAlerts::OnDblclkEmaillist() 
{
	// TODO: Add your control notification handler code here
}

void COptionsAlerts::OnDblclkSmslist() 
{
	// TODO: Add your control notification handler code here
}

void COptionsAlerts::OnAddEmail() 
{
	// TODO: Add your control notification handler code here
	CString data;

	if (m_ctlEmailList.GetCount() >= MAX_RECIPIENTS)
	{
		MessageBox("Maximum number of Recipients Reached.");
	}
	else
	{
		CAddEditRecipient dlgRecip(this, ADD_EMAIL, &data);
		if (dlgRecip.DoModal() == 1)
		{
			int pos = m_ctlEmailList.AddString(data);
			m_ctlEmailList.SetCheck(pos,1);
		}
	}
}

void COptionsAlerts::OnEditEmail() 
{
	// TODO: Add your control notification handler code here
	int sel;
	sel = m_ctlEmailList.GetCurSel();

	if (sel != LB_ERR)
	{
		CString data;
		m_ctlEmailList.GetText(sel, data); // pass the current value to edit dialog

		CAddEditRecipient dlgRecip(this, EDIT_EMAIL, &data);
		if (dlgRecip.DoModal() == 1)
		{
			m_ctlEmailList.DeleteString(sel); // out with the old
			int pos = m_ctlEmailList.AddString(data); // in with the new
			m_ctlEmailList.SetCheck(pos,1);
		}
	}
}


void COptionsAlerts::OnDeleteEmail() 
{
	// TODO: Add your control notification handler code here
	int sel;
	sel = m_ctlEmailList.GetCurSel();

	if (sel != LB_ERR)
	{
		CString str1, str2;
		m_ctlEmailList.GetText(sel, str1);
		str2.Format("Delete %s?", str1);
		if (MessageBox(str2, "Delete Email", MB_YESNO) == IDYES)
		{
			m_ctlEmailList.DeleteString(sel);
		}
	}
} // OnDeleteEmail() 


void COptionsAlerts::OnAddSms() 
{
	// TODO: Add your control notification handler code here
	CString data;

	if (m_ctlSmsList.GetCount() >= MAX_RECIPIENTS)
	{
		MessageBox("Maximum number of Recipients Reached.");
	}
	else
	{
		CAddEditRecipient dlgRecip(this, ADD_SMS, &data);
		if (dlgRecip.DoModal() == 1)
		{
			int pos = m_ctlSmsList.AddString(data);
			m_ctlSmsList.SetCheck(pos,1);
		}
	}
}

void COptionsAlerts::OnEditSms() 
{
	// TODO: Add your control notification handler code here
	int sel;
	sel = m_ctlSmsList.GetCurSel();

	if (sel != LB_ERR)
	{
		CString data;
		m_ctlSmsList.GetText(sel, data); // pass the current value to edit dialog

		CAddEditRecipient dlgRecip(this, EDIT_SMS, &data);
		if (dlgRecip.DoModal() == 1)
		{
			m_ctlSmsList.DeleteString(sel); // out with the old
			int pos = m_ctlSmsList.AddString(data); // in with the new
			m_ctlSmsList.SetCheck(pos,1);
		}
	}
}

void COptionsAlerts::OnDeleteSms() 
{
	// TODO: Add your control notification handler code here
	int sel;
	sel = m_ctlSmsList.GetCurSel();

	if (sel != LB_ERR)
	{
		CString str1, str2;
		m_ctlSmsList.GetText(sel, str1);
		str2.Format("Delete %s?", str1);
		if (MessageBox(str2, "Delete Phone Number", MB_YESNO) == IDYES)
		{
			m_ctlSmsList.DeleteString(sel);
		}
	}
} // OnDeleteSms() 


void COptionsAlerts::OnEmailSendTestMessage() 
{
	// TODO: Add your control notification handler code here
	// must save first or the current screen recipients wont be active
	SaveToActiveConfig();		// we can only send messages using the current config object
	config.SaveConfiguration(); // we can only send messages using the current config object

	Alerts.SendSmtpAlerts(m_iCurrentSelectedDataSource, "Test Message", true); // true means popup results
}

void COptionsAlerts::OnSmsGsmTestMessage() 
{
	// TODO: Add your control notification handler code here
	// must save first or the current screen recipients wont be active
	SaveToActiveConfig();		// we can only send messages using the current config object
	config.SaveConfiguration(); // we can only send messages using the current config object

	Alerts.SendSmsGsmAlerts(m_iCurrentSelectedDataSource, "Test Message", true);
}

void COptionsAlerts::OnSmsGatewayTestMessage() 
{
	// TODO: Add your control notification handler code here
	// must save first or the current screen recipients wont be active
	SaveToActiveConfig();		// we can only send messages using the current config object
	config.SaveConfiguration(); // we can only send messages using the current config object

	Alerts.SendSmsGatewayAlerts(m_iCurrentSelectedDataSource, "Test Message", true);
}

void COptionsAlerts::SaveToActiveConfig()
{
	int ctr; 

	UpdateData(TRUE);	// copy controls to mem vars 
	SaveRecipientListboxes();
	SaveCheckboxes();

	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		// Events
		config.bAlertsLAActive[ctr] = m_lcbAlertsLAActive[ctr];
		config.bAlertsLAClear[ctr] = m_lcbAlertsLAClear[ctr];
		config.bAlertsHFActive[ctr] = m_lcbAlertsHFActive[ctr];
		config.bAlertsHFClear[ctr] = m_lcbAlertsHFClear[ctr];
		config.bAlertsVHFActive[ctr] = m_lcbAlertsVHFActive[ctr];
		config.bAlertsVHFClear[ctr] = m_lcbAlertsVHFClear[ctr];
		config.bAlertsDAActive[ctr] = m_lcbAlertsDAActive[ctr];
		config.bAlertsDAClear[ctr] = m_lcbAlertsDAClear[ctr];
		config.bAlertsFaultsActive[ctr] = m_lcbAlertsFaultsActive[ctr];
		config.bAlertsFaultsClear[ctr] = m_lcbAlertsFaultsClear[ctr];
		config.bAlertsScheduled[ctr] = m_lcbAlertsScheduled[ctr];
		config.iAlertsScheduledHour[ctr] = m_lctAlertsScheduledTime[ctr].GetHour();
		config.iAlertsScheduledMins[ctr] = m_lctAlertsScheduledTime[ctr].GetMinute();
		config.iAlertsScheduledSecs[ctr] = m_lctAlertsScheduledTime[ctr].GetSecond();
		// E-Mail
		config.bSmtpEnabled[ctr] = m_lcbEmailEnabled[ctr];
		// SMS - GSM
		config.bSmsGsmEnabled[ctr] = m_lcbSmsGsmEnabled[ctr];
		// SMS - Gateway
		config.bSmsGatewayEnabled[ctr] = m_lcbSmsGatewayEnabled[ctr];

		for (int recip=0; recip < MAX_RECIPIENTS; recip++)
		{
			// Email 
			config.bEmailRecipientEnabled[ctr][recip] = m_lcbEmailRecipientEnabled[ctr][recip]; 
			config.txtEmailRecipient[ctr][recip] = m_lctxtEmailRecipient[ctr][recip]; // email addresses
			config.txtEmailRecipientName[ctr][recip] = m_lctxtEmailRecipientName[ctr][recip]; // comment person's name
			// SMS
			config.bSmsRecipientEnabled[ctr][recip] = m_lcbSmsRecipientEnabled[ctr][recip]; 
			config.txtSmsRecipient[ctr][recip] = m_lctxtSmsRecipient[ctr][recip]; // phone numbers
			config.txtSmsRecipientName[ctr][recip] = m_lctxtSmsRecipientName[ctr][recip]; // comment person's name  
		}
	}

}
