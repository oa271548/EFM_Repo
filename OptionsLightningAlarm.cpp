// OptionsLightningAlarm.cpp : implementation file
//

#include "stdafx.h"
#include "EFM.h"
#include "OptionsLightningAlarm.h"
#include "EFMOptions.h"
#include "DlgAddRemoveRing.h"
#include "EFMDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CConfiguration config;
extern CEFMOptions * dlgOptions;

/////////////////////////////////////////////////////////////////////////////
// COptionsLightningAlarm dialog


COptionsLightningAlarm::COptionsLightningAlarm(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsLightningAlarm::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsLightningAlarm)
	m_BEnable = FALSE;
	m_BShowChartLightningDistance = FALSE;
	m_dDuration = 0.0;
	m_dWAVRepeatInterval = 0.0;
	m_BOnActivationWAV = FALSE;
	m_BDistanceWAV = FALSE;
	m_BRepeatWAV = FALSE;
	m_BAllClearWAV = FALSE;
	m_txtOnActivationWAVFile = _T("");
	m_txtDistanceWAVFiles = _T("");
	m_txtRepeatWAVFile = _T("");
	m_txtAllClearWAVFile = _T("");
	m_txtRangeRings = _T("");
	m_iRingRangeUnit = -1;
	m_BOnlyAnnounceCloser = FALSE;
	//}}AFX_DATA_INIT

	m_pMainWnd = (CEFMDlg*)AfxGetMainWnd();
}


void COptionsLightningAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsLightningAlarm)
	DDX_Control(pDX, CMB_COINCIDENCE_COMPORT, m_cmbCoincidenceComPort);
	DDX_Control(pDX, CMB_COINCIDENCE_DETECTION, m_cmbCoincidenceDetection);
	DDX_Control(pDX, CMB_LA_DATASOURCENAMES, m_cmbDataSourceNames);
	DDX_Check(pDX, CHK_ENABLELA, m_BEnable);
	DDX_Text(pDX, TXT_LA_DURATION, m_dDuration);
	DDX_Text(pDX, TXT_LA_WAVREPEATINTERVAL, m_dWAVRepeatInterval);
	DDX_Check(pDX, CHK_LA_ONACTIVATIONWAV, m_BOnActivationWAV);
	DDX_Check(pDX, CHK_LA_DISTANCEWAV, m_BDistanceWAV);
	DDX_Check(pDX, CHK_LA_REPEATWAV, m_BRepeatWAV);
	DDX_Check(pDX, CHK_LA_ALLCLEARWAV, m_BAllClearWAV);
	DDX_Text(pDX, TXT_LA_ONACTIVATIONWAVFILE, m_txtOnActivationWAVFile);
	DDX_Text(pDX, TXT_LA_DISTANCEWAVFILES, m_txtDistanceWAVFiles);
	DDX_Text(pDX, TXT_LA_REPEATWAVFILE, m_txtRepeatWAVFile);
	DDX_Text(pDX, TXT_LA_ALLCLEARWAVFILE, m_txtAllClearWAVFile);
	DDX_Text(pDX, LBL_LA_RINGS, m_txtRangeRings);
	DDX_Radio(pDX, RDO_LA_UNIT, m_iRingRangeUnit);
	DDX_Check(pDX, CHK_ONLYANNOUNCECLOSER, m_BOnlyAnnounceCloser);
	DDX_Check(pDX, CHK_SHOWCHARTLA, m_BShowChartLightningDistance);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsLightningAlarm, CDialog)
	//{{AFX_MSG_MAP(COptionsLightningAlarm)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(CMB_LA_DATASOURCENAMES, OnSelchangeLaDatasourcenames)
	ON_BN_CLICKED(CHK_ENABLELA, OnEnablela)
	ON_EN_UPDATE(TXT_LA_DURATION, OnUpdateLaDuration)
	ON_EN_UPDATE(TXT_LA_WAVREPEATINTERVAL, OnUpdateLaWavrepeatinterval)
	ON_BN_CLICKED(CHK_LA_ONACTIVATIONWAV, OnLaOnactivationwav)
	ON_EN_UPDATE(TXT_LA_ONACTIVATIONWAVFILE, OnUpdateLaOnactivationwavfile)
	ON_BN_CLICKED(CHK_LA_DISTANCEWAV, OnLaDistancewav)
	ON_EN_UPDATE(TXT_LA_DISTANCEWAVFILES, OnUpdateLaDistancewavfiles)
	ON_BN_CLICKED(CHK_LA_REPEATWAV, OnLaRepeatwav)
	ON_EN_UPDATE(TXT_LA_REPEATWAVFILE, OnUpdateLaRepeatwavfile)
	ON_BN_CLICKED(CHK_LA_ALLCLEARWAV, OnLaAllclearwav)
	ON_EN_UPDATE(TXT_LA_ALLCLEARWAVFILE, OnUpdateLaAllclearwavfile)
	ON_BN_CLICKED(RDO_LA_UNIT, OnLaUnit)
	ON_BN_CLICKED(RDO_LA_UNIT_KM, OnLaUnitKm)
	ON_BN_CLICKED(BTN_LARINGS_ADD, OnLaringsAdd)
	ON_BN_CLICKED(BTN_LARINGS_REMOVE, OnLaringsRemove)
	ON_BN_CLICKED(BTN_CHOOSEWAV_ONACTIVATION, OnChoosewavOnactivation)
	ON_BN_CLICKED(BTN_PLAYWAV_ONACTIVATION, OnPlaywavOnactivation)
	ON_BN_CLICKED(BTN_CHOOSEWAV_DISTANCE, OnChoosewavDistance)
	ON_BN_CLICKED(BTN_PLAYWAV_DISTANCE, OnPlaywavDistance)
	ON_BN_CLICKED(BTN_CHOOSEWAV_REPEAT, OnChoosewavRepeat)
	ON_BN_CLICKED(BTN_PLAYWAV_REPEAT, OnPlaywavRepeat)
	ON_BN_CLICKED(BTN_CHOOSEWAV_ALLCLEAR, OnChoosewavAllclear)
	ON_BN_CLICKED(BTN_PLAYWAV_ALLCLEAR, OnPlaywavAllclear)
	ON_CBN_SELCHANGE(CMB_COINCIDENCE_DETECTION, OnSelchangeCoincidenceDetection)
	ON_CBN_SELCHANGE(CMB_COINCIDENCE_COMPORT, OnSelchangeCoincidenceComport)
	ON_BN_CLICKED(CHK_ONLYANNOUNCECLOSER, OnOnlyannouncecloser)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(CHK_SHOWCHARTLA, &COptionsLightningAlarm::OnBnClickedShowChartLightningDistance)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsLightningAlarm message handlers

BOOL COptionsLightningAlarm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int ctr;

	m_iNumberOfDataSources = config.m_iNumberOfDataSources;
	m_iCurrentSelectedDataSource = 0;

	// Populate the data source box with the data source names
	for (ctr=0; ctr<config.m_iNumberOfDataSources; ctr++)
	{
		m_cmbDataSourceNames.AddString(config.m_szDataSourceName[ctr]);
	}

	CoincidenceSetDetectionModes();
	CoincidenceFindAvailableComPorts();

	// Populate the local variable copies
	for (ctr=0; ctr<MAX_NUMOFDATASOURCES; ctr++)
	{
		m_lcBEnable[ctr] = config.m_BEnableLA[ctr];
		m_lcBShowChartLightningDistance[ctr] = config.m_BShowChartLightningDistance[ctr];
		m_lcdDuration[ctr] = config.m_dLADuration[ctr];
		m_lcdWAVRepeatInterval[ctr] = config.m_dLAWAVRepeatInterval[ctr];
		m_lcBOnActivationWAV[ctr] = config.m_BLAOnActivationEnable[ctr];
		m_lcBDistanceWAV[ctr] = config.m_BLADistanceEnable[ctr];
		m_lcBRepeatWAV[ctr] = config.m_BLARepeatEnable[ctr];
		m_lcBOnlyAnnounceCloser[ctr] = config.m_BLAOnlyAnnounceCloser[ctr];
		m_lcBAllClearWAV[ctr] = config.m_BLAAllClearEnable[ctr];
		m_lctxtOnActivationWAVFile[ctr] = config.m_txtLAOnActivationWAVFile[ctr];
		m_lctxtDistanceWAVFiles[ctr] = config.m_txtLADistanceWAVFiles[ctr];
		m_lctxtRepeatWAVFile[ctr] = config.m_txtLARepeatWAVFile[ctr];
		m_lctxtAllClearWAVFile[ctr] = config.m_txtLAAllClearWAVFile[ctr];
		m_lctxtRangeRings[ctr] = config.m_szLARingRanges[ctr];
		m_lciRingRangeUnit[ctr] = config.m_iLAUnit[ctr];
		m_lctxtLACDMode[ctr] = config.m_txtLACDMode[ctr];
		m_lctxtLACDComPort[ctr] = config.m_txtLACDComPort[ctr];
	}

	// Put the folder bmp on the buttons
	CButton * pButton;
	m_bmpFolder.LoadBitmap(BMP_FOLDER);
	pButton = (CButton* )GetDlgItem(BTN_CHOOSEWAV_ONACTIVATION);
	pButton->SetBitmap(m_bmpFolder);
	pButton = (CButton* )GetDlgItem(BTN_CHOOSEWAV_DISTANCE);
	pButton->SetBitmap(m_bmpFolder);
	pButton = (CButton* )GetDlgItem(BTN_CHOOSEWAV_REPEAT);
	pButton->SetBitmap(m_bmpFolder);
	pButton = (CButton* )GetDlgItem(BTN_CHOOSEWAV_ALLCLEAR);
	pButton->SetBitmap(m_bmpFolder);
	m_bmpPlay.LoadBitmap(BMP_PLAY);
	pButton = (CButton* )GetDlgItem(BTN_PLAYWAV_ONACTIVATION);
	pButton->SetBitmap(m_bmpPlay);
	pButton = (CButton* )GetDlgItem(BTN_PLAYWAV_DISTANCE);
	pButton->SetBitmap(m_bmpPlay);
	pButton = (CButton* )GetDlgItem(BTN_PLAYWAV_REPEAT);
	pButton->SetBitmap(m_bmpPlay);
	pButton = (CButton* )GetDlgItem(BTN_PLAYWAV_ALLCLEAR);
	pButton->SetBitmap(m_bmpPlay);

	m_bmpStop.LoadBitmap(BMP_STOP);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsLightningAlarm::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	dlgOptions->dlgGeneral.UpdateData(TRUE);

	switch (bShow)
	{
		case 1:	// Being Shown
			// Clear the Data Source Names combo box
			m_cmbDataSourceNames.ResetContent();
			// Repopulate the Data Source Names combo box
			for (size_t ctr = 0; ctr < dlgOptions->dlgGeneral.m_lciNumberOfDataSources; ++ctr)
			{
				m_cmbDataSourceNames.AddString(dlgOptions->dlgGeneral.m_lcszDataSourceName[ctr]);
			}
			// Show the settings for the currently selected item
			//m_iCurrentSelectedDataSource = 0;
			m_cmbDataSourceNames.SelectString(-1, dlgOptions->dlgGeneral.m_lcszDataSourceName[m_iCurrentSelectedDataSource]);

			OnSelchangeLaDatasourcenames();

			UpdateData(FALSE);
			
			break;
		case 0: // Being Hidden
			break;
	}	
}

BOOL COptionsLightningAlarm::PreTranslateMessage(MSG* pMsg) 
{
	if ((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) ||
		(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE))
	{
		return TRUE; // just ignore the enter key
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void COptionsLightningAlarm::OnSelchangeLaDatasourcenames() 
{
	m_iCurrentSelectedDataSource = m_cmbDataSourceNames.GetCurSel();

	m_BEnable = m_lcBEnable[m_iCurrentSelectedDataSource];
	m_BShowChartLightningDistance = m_lcBShowChartLightningDistance[m_iCurrentSelectedDataSource];
	m_dDuration = m_lcdDuration[m_iCurrentSelectedDataSource];
	m_dWAVRepeatInterval =m_lcdWAVRepeatInterval[m_iCurrentSelectedDataSource];
	m_BOnActivationWAV = m_lcBOnActivationWAV[m_iCurrentSelectedDataSource];
	m_BDistanceWAV = m_lcBDistanceWAV[m_iCurrentSelectedDataSource];
	m_BOnlyAnnounceCloser = m_lcBOnlyAnnounceCloser[m_iCurrentSelectedDataSource];
	m_BRepeatWAV = m_lcBRepeatWAV[m_iCurrentSelectedDataSource];
	m_BAllClearWAV = m_lcBAllClearWAV[m_iCurrentSelectedDataSource];
	m_txtOnActivationWAVFile = m_lctxtOnActivationWAVFile[m_iCurrentSelectedDataSource];
	m_txtDistanceWAVFiles = m_lctxtDistanceWAVFiles[m_iCurrentSelectedDataSource];
	m_txtRepeatWAVFile = m_lctxtRepeatWAVFile[m_iCurrentSelectedDataSource];
	m_txtAllClearWAVFile = m_lctxtAllClearWAVFile[m_iCurrentSelectedDataSource];
	m_txtRangeRings = m_lctxtRangeRings[m_iCurrentSelectedDataSource];
	m_iRingRangeUnit = m_lciRingRangeUnit[m_iCurrentSelectedDataSource];

	UpdateData(FALSE);
}

void COptionsLightningAlarm::OnEnablela() 
{
	UpdateData(TRUE);

	m_lcBEnable[m_iCurrentSelectedDataSource] = m_BEnable;	
}

void COptionsLightningAlarm::OnUpdateLaDuration() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lcdDuration[m_iCurrentSelectedDataSource] = m_dDuration;	
}

void COptionsLightningAlarm::OnUpdateLaWavrepeatinterval() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lcdWAVRepeatInterval[m_iCurrentSelectedDataSource] = m_dWAVRepeatInterval;
}

void COptionsLightningAlarm::OnLaOnactivationwav() 
{
	UpdateData(TRUE);

	m_lcBOnActivationWAV[m_iCurrentSelectedDataSource] = m_BOnActivationWAV;
}

void COptionsLightningAlarm::OnUpdateLaOnactivationwavfile() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lctxtOnActivationWAVFile[m_iCurrentSelectedDataSource] = m_txtOnActivationWAVFile;
}

void COptionsLightningAlarm::OnLaDistancewav() 
{
	UpdateData(TRUE);

	m_lcBDistanceWAV[m_iCurrentSelectedDataSource] = m_BDistanceWAV;
}

void COptionsLightningAlarm::OnUpdateLaDistancewavfiles() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lctxtDistanceWAVFiles[m_iCurrentSelectedDataSource] = m_txtDistanceWAVFiles;
}

void COptionsLightningAlarm::OnLaRepeatwav() 
{
	UpdateData(TRUE);

	m_lcBRepeatWAV[m_iCurrentSelectedDataSource] = m_BRepeatWAV;
}

void COptionsLightningAlarm::OnUpdateLaRepeatwavfile() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lctxtRepeatWAVFile[m_iCurrentSelectedDataSource] = m_txtRepeatWAVFile;	
}

void COptionsLightningAlarm::OnLaAllclearwav() 
{
	UpdateData(TRUE);

	m_lcBAllClearWAV[m_iCurrentSelectedDataSource] = m_BAllClearWAV;	
}

void COptionsLightningAlarm::OnUpdateLaAllclearwavfile() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	UpdateData(TRUE);

	m_lctxtAllClearWAVFile[m_iCurrentSelectedDataSource] = m_txtAllClearWAVFile;	
}

void COptionsLightningAlarm::OnLaUnit() 
{
	UpdateData(TRUE);

	m_lciRingRangeUnit[m_iCurrentSelectedDataSource] = m_iRingRangeUnit;	
}

void COptionsLightningAlarm::OnLaUnitKm() 
{
	UpdateData(TRUE);

	m_lciRingRangeUnit[m_iCurrentSelectedDataSource] = m_iRingRangeUnit;	
}

void COptionsLightningAlarm::OnLaringsAdd() 
{
	CDlgAddRemoveRing dlgRings(this, ADD, m_iRingRangeUnit);
	int iVal = dlgRings.DoModal();

	CString txtRings, tmp;

	bool bValueExists = false;
	int iIdx;

	if (iVal >= 0)
	{
		CUIntArray uaVals;
		char * tok = NULL;
		char seps[] = ",";
		char* next_token = NULL;
		// Parse the ranges into an array
		txtRings = m_lctxtRangeRings[m_iCurrentSelectedDataSource];
		tok = strtok_s(txtRings.GetBuffer(txtRings.GetLength()), seps, &next_token);
		uaVals.Add(atoi(tok));
		while (tok)
		{
			tok = strtok_s(NULL, seps, &next_token);
			if (tok)
			{
				uaVals.Add(atoi(tok));
			}
		}

		// Check to see if the value exists
		for (iIdx=0; iIdx<uaVals.GetSize(); iIdx++)
		{
			if ((unsigned)iVal == uaVals[iIdx])
			{
				bValueExists = true;
			}
		}

		if (!bValueExists)
		{
			// The value doesn't exist, so let's add it.
			uaVals.Add(iVal);

			// Sort the array
			for(int x=0; x<uaVals.GetSize(); x++)
			{
				int index_of_min = x;
				for(int y=x; y<uaVals.GetSize(); y++)
				{
					if(uaVals[index_of_min] > uaVals[y])
					{
						index_of_min = y;
					}
				}
				int temp = uaVals[x];

				uaVals[x] = uaVals[index_of_min];

				uaVals[index_of_min] = temp;
			}

			// Set the new ranges
			txtRings = "";
			for (iIdx=0; iIdx<uaVals.GetSize()-1; iIdx++)
			{
				tmp.Format("%d, ", uaVals.GetAt(iIdx));
				txtRings += tmp;
				//TRACE("%d,",uaVals.GetAt(iIdx));
			}
			tmp.Format("%d", uaVals.GetAt(iIdx));
			txtRings += tmp;
			//TRACE("%d",uaVals.GetAt(iIdx));

			// Update the current temporary settings 
			m_lctxtRangeRings[m_iCurrentSelectedDataSource] = txtRings;
			// Update the dialog box
			m_txtRangeRings = txtRings;

			UpdateData(FALSE);

		}
		else
		{
			// The value already exists so tell the user.
			::MessageBox(this->GetSafeHwnd(), "The range already exists.","Range Input",MB_OK);
		}
	} // if (iVal >= 0)

}

void COptionsLightningAlarm::OnLaringsRemove() 
{
	CString txtRings, tmp;
	int iIdx;
	bool bValueExists = false;

	CDlgAddRemoveRing dlgRings(this, REMOVE, m_iRingRangeUnit);
	int iVal = dlgRings.DoModal();

	if (iVal >= 0)
	{
		CUIntArray uaVals;
		char * tok = NULL;
		char seps[] = ",";
		char* next_token = NULL;
		// Parse the ranges into an array
		txtRings = m_lctxtRangeRings[m_iCurrentSelectedDataSource];
		tok = strtok_s(txtRings.GetBuffer(txtRings.GetLength()), seps, &next_token);
		uaVals.Add(atoi(tok));
		while (tok)
		{
			tok = strtok_s(NULL, seps, &next_token);
			if (tok)
			{
				uaVals.Add(atoi(tok));
			}
		}

		// Check to see if the value exists
		for (iIdx=0; iIdx<uaVals.GetSize(); iIdx++)
		{
			if ((unsigned)iVal == uaVals[iIdx])
			{
				// The value exists - remove it from the array
				if (uaVals.GetSize() <= 1)
				{
					::MessageBox(this->GetSafeHwnd(), "You must have at least one range.","Range Input",MB_OK);
					return;
				}
				uaVals.RemoveAt(iIdx);
				bValueExists = true;
			}
		}

		if (!bValueExists)
		{
			// The value does not exist
			::MessageBox(this->GetSafeHwnd(), "The range does not exist.","Range Input",MB_OK);
			return;
		}

		// The value existed so reconstruct the range ring line
		//
		// Sort the array
		for(int x=0; x<uaVals.GetSize(); x++)
		{
			int index_of_min = x;
			for(int y=x; y<uaVals.GetSize(); y++)
			{
				if(uaVals[index_of_min] > uaVals[y])
				{
					index_of_min = y;
				}
			}
			int temp = uaVals[x];

			uaVals[x] = uaVals[index_of_min];

			uaVals[index_of_min] = temp;
		}

		// Set the new ranges
		txtRings = "";
		for (iIdx=0; iIdx<uaVals.GetSize()-1; iIdx++)
		{
			tmp.Format("%d, ", uaVals.GetAt(iIdx));
			txtRings += tmp;
			//TRACE("%d,",uaVals.GetAt(iIdx));
		}
		tmp.Format("%d", uaVals.GetAt(iIdx));
		txtRings += tmp;
		//TRACE("%d",uaVals.GetAt(iIdx));

		// Update the current temporary settings 
		m_lctxtRangeRings[m_iCurrentSelectedDataSource] = txtRings;
		// Update the dialog box
		m_txtRangeRings = txtRings;

		UpdateData(FALSE);

	} // if (iVal >= 0)	
}

void COptionsLightningAlarm::OnChoosewavOnactivation() 
{
	SelectWAVFile(&m_txtOnActivationWAVFile);
	m_lctxtOnActivationWAVFile[m_iCurrentSelectedDataSource] = m_txtOnActivationWAVFile;
	UpdateData(FALSE);
}

void COptionsLightningAlarm::OnPlaywavOnactivation() 
{
	if ((m_txtOnActivationWAVFile.CompareNoCase("None Selected.") != 0) && (m_txtOnActivationWAVFile.IsEmpty() == FALSE))
	{
		// The Enabling/Disabling of the buttons is not necessary on 
		// the newer, fast machines; but for the slower ones they're 
		// necessary for safety.
		(GetDlgItem(BTN_PLAYWAV_ONACTIVATION))->EnableWindow(FALSE);
		((CEFMDlg*)m_pMainWnd)->PlayWAV(0, m_txtOnActivationWAVFile);
		(GetDlgItem(BTN_PLAYWAV_ONACTIVATION))->EnableWindow(TRUE);
	}
	else
	{
		::MessageBox(NULL,"Please select a WAV file first.","No WAV File Selected",MB_OK|MB_ICONERROR);
	}		
}

void COptionsLightningAlarm::OnChoosewavDistance() 
{

	LPMALLOC pMalloc;
	if (SHGetMalloc (&pMalloc) != NOERROR) return;

	BROWSEINFO bInfo;
	LPITEMIDLIST pidl;
	char szPath[_MAX_PATH + 1];

	memset(&bInfo, 0, sizeof(BROWSEINFO));

	bInfo.hwndOwner = NULL;
	bInfo.pszDisplayName = szPath;
	bInfo.lpszTitle = "Select Location";
	// 0x40 adds "Make New Folder" button
	bInfo.ulFlags = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS|BIF_EDITBOX|0x40;

	pidl = ::SHBrowseForFolder (&bInfo);

	if (pidl)	// User hits OK
	{
		::SHGetPathFromIDList(pidl, szPath);
		pMalloc->Free(pidl);
		// szPath contains the chosen path!
		if (lstrlen(szPath) == 0)
		{
			::MessageBox(NULL,"Please select a real folder.","Bad Folder Selection",MB_OK|MB_ICONERROR);
		}
		else
		{
			if (szPath[lstrlen(szPath)-1] != '\\')
			{
				lstrcat(szPath,"\\");
			}
			m_txtDistanceWAVFiles = szPath;
			m_lctxtDistanceWAVFiles[m_iCurrentSelectedDataSource] = m_txtDistanceWAVFiles;
		}
	}
	
	pMalloc->Release();
	UpdateData(FALSE);

}

void COptionsLightningAlarm::OnPlaywavDistance() 
{
	CString txt;
	int iDis;

	if ((m_txtDistanceWAVFiles.CompareNoCase("None Selected.") != 0) && (m_txtDistanceWAVFiles.IsEmpty() == FALSE))
	{
		// The Enabling/Disabling of the buttons is not necessary on 
		// the newer, fast machines; but for the slower ones they're 
		// necessary for safety.
		((CEFMDlg*)m_pMainWnd)->PlayWAV(0, m_txtDistanceWAVFiles+"LightningDetectedAt.wav");
		srand(static_cast<unsigned int>(time(NULL)));
		iDis = (rand() % (24 - 1 + 1) + 1);
		txt.Format("%d.wav", iDis);
		((CEFMDlg*)m_pMainWnd)->PlayWAV(0, m_txtDistanceWAVFiles+txt);
		//iDis = (rand() % (2 - 1 + 1) + 1);
		//if (iDis == 1)
		//{	txt = "Miles.wav";	}
		//else
		//{	txt = "Kilometres.wav";	}
		if (m_lciRingRangeUnit[m_iCurrentSelectedDataSource] == MILES)
		{
			txt = "Miles.wav";
		}
		else
		{
			txt = "Kilometres.wav";
		}
		((CEFMDlg*)m_pMainWnd)->PlayWAV(0, m_txtDistanceWAVFiles+txt);
	}
	else
	{
		::MessageBox(NULL,"Please set the location for your distance audio files.","No WAV File Location Selected",MB_OK|MB_ICONERROR);
	}		
}

void COptionsLightningAlarm::OnChoosewavRepeat() 
{
	SelectWAVFile(&m_txtRepeatWAVFile);
	m_lctxtRepeatWAVFile[m_iCurrentSelectedDataSource] = m_txtRepeatWAVFile;
	UpdateData(FALSE);
}

void COptionsLightningAlarm::OnPlaywavRepeat() 
{
	if ((m_txtRepeatWAVFile.CompareNoCase("None Selected.") != 0) && (m_txtRepeatWAVFile.IsEmpty() == FALSE))
	{
		// The Enabling/Disabling of the buttons is not necessary on 
		// the newer, fast machines; but for the slower ones they're 
		// necessary for safety.
		(GetDlgItem(BTN_PLAYWAV_REPEAT))->EnableWindow(FALSE);
		((CEFMDlg*)m_pMainWnd)->PlayWAV(0, m_txtRepeatWAVFile);
		(GetDlgItem(BTN_PLAYWAV_REPEAT))->EnableWindow(TRUE);
	}
	else
	{
		::MessageBox(NULL,"Please select a WAV file first.","No WAV File Selected",MB_OK|MB_ICONERROR);
	}	
}

void COptionsLightningAlarm::OnChoosewavAllclear() 
{
	SelectWAVFile(&m_txtAllClearWAVFile);
	m_lctxtAllClearWAVFile[m_iCurrentSelectedDataSource] = m_txtAllClearWAVFile;
	UpdateData(FALSE);
}

void COptionsLightningAlarm::OnPlaywavAllclear() 
{
	if ((m_txtAllClearWAVFile.CompareNoCase("None Selected.") != 0) && (m_txtAllClearWAVFile.IsEmpty() == FALSE))
	{
		// The Enabling/Disabling of the buttons is not necessary on 
		// the newer, fast machines; but for the slower ones they're 
		// necessary for safety.
		(GetDlgItem(BTN_PLAYWAV_ALLCLEAR))->EnableWindow(FALSE);
		((CEFMDlg*)m_pMainWnd)->PlayWAV(0, m_txtAllClearWAVFile);
		(GetDlgItem(BTN_PLAYWAV_ALLCLEAR))->EnableWindow(TRUE);
	}
	else
	{
		::MessageBox(NULL,"Please select a WAV file first.","No WAV File Selected",MB_OK|MB_ICONERROR);
	}		
}

bool COptionsLightningAlarm::SelectWAVFile(CString * wavPath)
{
	int dlgReturn;
	CString strPath;
	CFileDialog sDlg(true);	// Create in "Open" Mode

	sDlg.m_ofn.lpstrTitle = _T("Select WAV File...");
	sDlg.m_ofn.lpstrFilter = _T("WAV File (*.wav)\0*.wav\0");
	sDlg.m_ofn.lpstrDefExt = _T("wav");		// Set the initial default extension

	dlgReturn = sDlg.DoModal();	
	if (dlgReturn == IDOK)
	{
		// OK was pressed
		//strFileName = sDlg.GetFileName();		// Filename + extension
		strPath = sDlg.GetPathName();			// Path + filename + extension
		//strTitle = sDlg.GetFileTitle();			// Filename, no extension
		//strFileExtension = sDlg.GetFileExt();	// Extension, no filename
		
		*wavPath = strPath;		// Set the new wav filename and path
		return true;
	} // End "if (dlgReturn == IDOK)"
	else
	{
		// Cancel was pressed
		return false;	// Cancel, change nothing (heavy russian accent)
	}
}

void COptionsLightningAlarm::CoincidenceFindAvailableComPorts()
{
	int ctr, idx;
	CString txt;
	CSerial ser;

	// Scan ports 1-16
	for (ctr=1; ctr<=16; ctr++)
	{
		txt.Format("COM%d",ctr);
		if (ser.CheckPort(txt) == CSerial::EPort::EPortAvailable)
		{
			m_cmbCoincidenceComPort.AddString(txt);
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
					m_cmbCoincidenceComPort.AddString(txt);
					// We've found a data source point that uses the 
					// current com port.  We don't care if any 
					// other source points have the same port.
					break;
				}
			}

		}
	}
	int iLoc = m_cmbCoincidenceComPort.FindString(-1, config.m_txtLACDComPort[m_iCurrentSelectedDataSource].GetBuffer(config.m_txtLACDComPort[m_iCurrentSelectedDataSource].GetLength()));
	m_cmbCoincidenceComPort.SetCurSel(iLoc);
}

void COptionsLightningAlarm::CoincidenceSetDetectionModes()
{
	for (int iIdx=0; iIdx<MAX_COINCIDENCEMODES; iIdx++)
	{
		m_cmbCoincidenceDetection.AddString(txtCOINCIDENCEMODES[iIdx]);
	}
	int iLoc = m_cmbCoincidenceDetection.FindString(-1, config.m_txtLACDMode[m_iCurrentSelectedDataSource].GetBuffer(config.m_txtLACDMode[m_iCurrentSelectedDataSource].GetLength()));
	m_cmbCoincidenceDetection.SetCurSel(iLoc);
}

void COptionsLightningAlarm::OnSelchangeCoincidenceDetection() 
{
	CString txtMode;
	m_cmbCoincidenceDetection.GetLBText(m_cmbCoincidenceDetection.GetCurSel(), txtMode);
	m_lctxtLACDMode[m_iCurrentSelectedDataSource] = txtMode;	
}

void COptionsLightningAlarm::OnSelchangeCoincidenceComport() 
{
	CString txtPort;
	m_cmbCoincidenceComPort.GetLBText(m_cmbCoincidenceComPort.GetCurSel(), txtPort);
	m_lctxtLACDComPort[m_iCurrentSelectedDataSource] = txtPort;
}

void COptionsLightningAlarm::OnOnlyannouncecloser() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_lcBOnlyAnnounceCloser[m_iCurrentSelectedDataSource] = m_BOnlyAnnounceCloser;

}


void COptionsLightningAlarm::OnBnClickedShowChartLightningDistance()
{
	UpdateData(TRUE);

	m_lcBShowChartLightningDistance[m_iCurrentSelectedDataSource] = m_BShowChartLightningDistance;
}
