//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Interface for thread for background operations
//--------------------------------------------------------------------------


#include "stdafx.h"

#include "cmn/include/Log.h"
#include "cmn/include/McException.h"
#include "cmn/include/RegKey.h"
#include "cmn/include/Misc.h"

#include "v/include/Constants.h"

#include "db/include/Constants.h"
#include "db/include/Storage.h"
#include "db/include/SqlBrowser.h"

#include "scm/include/SchemaDef.h"

#include "wzr/include/Page.h"
#include "wzr/include/Sheet.h"
#include "wzr/include/SelectServersDlg.h"
#include "wzr/include/SQLSrvAddrParser.h"
#include "wzr/include/Thread.h"

#include "ocrcfg/include/misc.h"
#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/ConfigApp.h"
#include "ocrcfg/include/RegServicesThread.h"
#include "ocrcfg/include/RegServicesPage.h"
#include "ocrcfg/resource/resource.h"
//#include "samcfg/resource/samcfg.rc"

#pragma warning(disable: 4800)

const TCHAR cszSearchItemName[] = _T("<Поиск...>");

// CRegServicesPage dialog

//IMPLEMENT_DYNAMIC(CRegServicesPage, WZRPage)

CRegServicesPage::CRegServicesPage(COcrConfigData& rConfigData, int* pnModificationFlags)
	: WZRPage(IDD_OCRCFG_REGSVC_PAGE, IDS_OCRCFG_REGSVC_TITLE, IDS_OCRCFG_REGSVC_SUBTITLE),
	m_bError(true),	
	m_rConfigData(rConfigData),
	m_pnModificationFlags(pnModificationFlags)
{    
	m_psp.dwFlags |= PSP_HASHELP;

	if(!m_pnModificationFlags)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_POINTER_TO_INT_IS_NULL);
		*pEx << _T("CRegServicesPage::CRegServicesPage");
		ThrowException(pEx);
	}

	if (m_vecModules.size() == 0)
	{
		// hard to stop
		m_vecModules.push_back(new CDeliceOcrServiceDesc(_T("DeliceCodesTechService2"),		_T("РИО - служба распознавания кодов2"), _T("Распознает штрих-коды"), 
			                                             _T("DeliceCodesService2"),			_T("DeliceCodesService2.exe") ));
		m_vecModules.push_back(new CDeliceOcrServiceDesc(_T("DeliceCopyTechService"),		_T("РИО - служба центральная транспортная"), _T("Копирует документы"), 
			                                             _T("DeliceCopyService"),			_T("DeliceCopyService.exe") ));		
		m_vecModules.push_back(new CDeliceOcrServiceDesc(_T("DeliceIndexingTechService"),	_T("РИО - служба индексации"), _T("Выделяет текстовые фрагменты из документа"), 
			                                             _T("DeliceIndexingService"),		_T("DeliceIndexingService.exe") ));		
		m_vecModules.push_back(new CDeliceOcrServiceDesc(_T("DelicePreOCRTechService"),		_T("РИО - служба предобработки изображений"), _T("Доворачивает изображения перед сканированием"), 
			                                             _T("DelicePreOCRService"),			_T("DelicePreOCRService.exe"), _T("DELICE_TECH_PROCESS"), _T("Connection"), _T("OCR")));

		// tech services, they share one single settings.ini file
		m_vecModules.push_back(new CDeliceOcrServiceDesc(_T("DeliceOcrTechService"),		_T("РИО - служба распознавания текста"), _T("Рапознает текст"), 
			                                             _T("TechServices"),				_T("DeliceOcrService.exe"), _T("DELICE_TECH_PROCESS"), _T("Connection"), _T("OCR")));

		m_vecModules.push_back(new CDeliceOcrServiceDesc(_T("DelicePdfTechsService"),		_T("РИО - служба сборки Pdf"), _T("Из отдельных обработанных страниц собирает финальный документ"), 
			                                             _T("TechServices"),	_T("DelicePdfService.exe"), _T("DELICE_TECH_PROCESS"), _T("Connection"), _T("Pdf")));

		m_vecModules.push_back(new CDeliceOcrServiceDesc(_T("DeliceTechPicturesSortTasksService"),	_T("РИО - служба определения вида документа"), _T("Производит сепарацию разных видом документов"), 
			                                             _T("TechServices"),	_T("DelicePicturesSortTechService.exe"), _T("DELICE_TECH_PROCESS"), _T("Connection"), _T("PicturesSort")));
		

		// the only one exception from the rules is DeliceControlPanel where we use [Settings] section
		m_vecModules.push_back(new CDeliceOcrAppDesc	(_T("DeliceControlPanel"),		_T("РИО - панель управления"),	_T(""),
														 _T("DeliceControlPanel"),		_T("DeliceControlPanel.exe"),	_T("DELICE_TECH_PROCESS"), _T("Settings"), _T("Settings")));
		m_vecModules.push_back(new CDeliceOcrAppDesc	(_T("DeliceProcess"),			_T("РИО - панель обработки данных"), _T(""),
														 _T("DeliceProcess"),			_T("DeliceProcess.exe"), _T("RIO_IPS"), _T("Connection"), _T("Settings")));
	}
}

CRegServicesPage::~CRegServicesPage()
{
	
}

BEGIN_MESSAGE_MAP(CRegServicesPage, WZRPage)
    //{{AFX_MSG_MAP(CRegServicesPage)    
	ON_MESSAGE		( PSM_QUERYSIBLINGS,			OnQuerySiblings)
    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_START, OnScriptExcecutionStart  )
    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_STAGE, OnScriptExcecutionStage  )
    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_FINISH, OnScriptExcecutionFinish )
//    ON_MESSAGE      ( WM_OCRCFG_CUSTOMSCRIPT_EXCECUTION_START, OnCustomScriptExcecutionStart )

	ON_CONTROL(CLBN_CHKCHANGE, IDC_OCRCFG_REGSVC_LIST, OnNamesChange)
	ON_BN_CLICKED	( IDC_WZR_CHECK_SKIP, OnSkipBtnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CRegServicesPage::DoDataExchange(CDataExchange* pDX)
{
  	WZRPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRegServicesPage)	
	DDX_Control(pDX, IDC_OCRCFG_REGSVC_LIST, m_lbServices);
	int iItem = 0;
	if (pDX->m_bSaveAndValidate)
	{		
		for (iItem = 0; iItem < m_lbServices.GetCount(); iItem++)
		{
			bool bCheck = m_lbServices.GetCheck(iItem);
			m_vecModules[iItem]->m_bCheck = bCheck;
		}
	}
	else
	{
		for (iItem = 0; iItem < m_lbServices.GetCount(); iItem++)
		{
			bool bCheck = m_vecModules[iItem]->m_bCheck;			
			m_lbServices.SetCheck(iItem, bCheck);
		}
	}	
    //}}AFX_DATA_MAP
}


BOOL CRegServicesPage::OnInitDialog()
{
    WZRPage::OnInitDialog();

	for (auto spService : m_vecModules)
	{
		CString str = spService->m_strDisplayName;
		m_lbServices.AddString(str);
	}

	return TRUE;
}


BOOL CRegServicesPage::OnSetActive()
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if (!pApp)
		return FALSE;

	UpdateData(FALSE);

	if (pApp->m_enScenario == CUseScenarioEnum::eusRemove)
	{
		CString strTitle, strSubtitle, strSkipText;
		strTitle.LoadString(IDS_OCRCFG_UNREGSVC_TITLE);
		strSubtitle.LoadString(IDS_OCRCFG_UNREGSVC_SUBTITLE);
		PropSheet_SetHeaderTitle(m_pSheet->GetSafeHwnd(), m_pSheet->GetActiveIndex(), (const TCHAR*)strTitle);
		PropSheet_SetHeaderSubTitle(m_pSheet->GetSafeHwnd(), m_pSheet->GetActiveIndex(), (const TCHAR*)strSubtitle);

		CWnd *pControl = GetDlgItem(IDC_OCRCFG_REGSVC_LIST);
		if (pControl != NULL)
			pControl->EnableWindow(FALSE);

		m_rConfigData.LoadFromRegistry();
	}
	// in installer mode 'skip' checkbox is not visible, when removing also not visible
	if (pApp->m_bInstallerMode || pApp->m_enScenario == CUseScenarioEnum::eusRemove)
    {
        if (m_btnSkipPage.m_hWnd)
            m_btnSkipPage.ShowWindow(SW_HIDE);
    }

    if (pApp->m_bUnattendedMode)
	{
		::PostMessage(m_pSheet->GetSafeHwnd(), PSM_PRESSBUTTON, PSBTN_NEXT, 0); // and simulate next button press
	}

    return WZRPage::OnSetActive();
}


BOOL CRegServicesPage::OnKillActive()
{
	return WZRPage::OnKillActive();
}

LRESULT CRegServicesPage::OnWizardBack()
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());

	switch (pApp->m_enScenario)
	{
	case CUseScenarioEnum::eusCreateNew:
		return IDD_OCRCFG_REGJOBS_PAGE;
	case CUseScenarioEnum::eusRemove:
		return IDD_OCRCFG_WELCOME_PAGE;		
	}
	return -1;
}

LRESULT CRegServicesPage::OnWizardNext()
{
	try
	{
		if ( UpdateData(TRUE) == FALSE )
		{
			if ( ! m_iSkipPage )
			{
				ThrowException(new TException(IDE_OCRCFG_INCORRECT_INPUT));
			}
		}

		CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
		if ( ! pApp)
		{
			ThrowException(new TException(IDE_OCRCFG_ERROR_RECIEVE_POINTER));
		}
		CConfigXMLData& data = pApp->GetConfigXMLData();

		if ( m_iSkipPage )
		{
			m_bError = false;		

			return IDD_OCRCFG_FINISH_PAGE;
		}
		
		ProcessServices();

		return -1; // stay at this page until install completes
	}
	catch(TException* pTEx)
	{
		CAtlString strErrMessage = TException::GetAllErrorMessages(pTEx);
		CMNLog(_T("[CRegServicesPage::OnWizardNext]: %s"), strErrMessage);
		pTEx->ReportError();
		pTEx->Delete();
		return -1;
	}
}


void CRegServicesPage::SetButtons()
{
	if ( ! theApp.m_bUnattendedMode)
	{
		UpdateData();

		// disable Next button by default
		DWORD dwFlags = PSWIZB_BACK;		

		// check for at least one check
		for (int iItem = 0; iItem < m_lbServices.GetCount(); iItem++)
		{
			bool bCheck = m_lbServices.GetCheck(iItem);
			if (bCheck)
			{
				dwFlags = (PSWIZB_BACK | PSWIZB_NEXT);
				break;
			}
		}
		m_pSheet->SetWizardButtons(dwFlags);
	}
}

WZRThread* CRegServicesPage::ConstructThread() const
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if ( ! pApp ) 
		return NULL;
	CConfigXMLData& data = pApp->GetConfigXMLData();

	return new CRegServicesThread( GetSafeHwnd(), m_vecModules, m_rConfigData, data, m_pnModificationFlags );
}

LRESULT CRegServicesPage::ProcessServices()
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if ( ! pApp )  
		return 0;
	CConfigXMLData& data = pApp->GetConfigXMLData();
	//set<CString> setScripts = data.GetScriptFilePaths();
	/*
	DBStorage::FillScriptIDVector();
	int nScripts = 0;
	if ( data.IsCreateServer() )
	{		
		nScripts += DBStorage::m_vecScriptID.size();
	}
	if ( data.IsRunSqlScripts() )
	{
		nScripts += data.GetScriptFilePaths().size();
	}
	*/
	//m_ctlProgress.SetRange((short) 0, (short) ( nScripts > 0 ? (nScripts-1) : 0 ));

	m_ctlProgress.SetRange((short) 0, (short) m_vecModules.size() );  // CRegJobsThread::GetJobsCount()
    m_ctlProgress.SetStep(1);
	m_ctlProgress.SetPos(0);
    OnThreadStarted();

    return 0;
}


// CRegServicesPage message handlers

LRESULT CRegServicesPage::OnQuerySiblings(WPARAM wParam, LPARAM lParam)
{
    if (wParam == WM_OCRCFG_QUERY_REGSVC)
    {
        ::memcpy((void*) lParam, (void*) m_strThreadResults.GetString(), m_strThreadResults.GetLength() * sizeof(TCHAR));
		return 1; // answer
    }	
    return 0; // ignore
}

LRESULT CRegServicesPage::OnScriptExcecutionStart (WPARAM wParam, LPARAM lParam)
{
	switch (theApp.m_enScenario)
	{
	case CUseScenarioEnum::eusCreateNew:
		m_strStage.FormatMessage(IDS_OCRCFG_SVC_CREATE_PROCESS);		
		break;
	case CUseScenarioEnum::eusRemove:
		m_strStage.FormatMessage(IDS_OCRCFG_SVC_REMOVE_PROCESS);		
		break;
	}

	UpdateData(FALSE);
	return 0;
}

LRESULT CRegServicesPage::OnScriptExcecutionStage (WPARAM wParam, LPARAM lParam)
{
	CString strFormat;
	DWORD dwPercent = (DWORD) wParam;	
    DWORD iPos = (DWORD)m_ctlProgress.GetPos();
	TRACE(_T("[CRegServicesPage::OnScriptExcecutionStage] %i, %i\n"), dwPercent, iPos);
    if (iPos < dwPercent)
    {
        m_ctlProgress.StepIt();
		TRACE(_T("[CRegServicesPage::OnScriptExcecutionStage] StepIt()\n"));
		ShowProgress();
    }
    return 0;
}

LRESULT CRegServicesPage::OnScriptExcecutionFinish (WPARAM wParam, LPARAM lParam)
{
	m_bError = (BOOL) wParam;

	CString strError = m_pThread->GetErrorString();
	OnThreadFinished();

	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	if(!pApp)
		return -1;

	if(m_bError)
	{
		theApp.m_bInstallerMode = FALSE;

		if ( ! theApp.m_bUnattendedMode)
		{
			m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

			TException tex(IDE_OCRCFG_ERROR_STRING);
			tex << strError;
			tex.ReportError();

			return -1;
		}
		else
		{
			// record to log was made in CreateDBThread class
			//::PostMessage(m_pSheet->GetSafeHwnd(), WM_QUIT, 0, 0);
			::PostMessage(m_pSheet->GetSafeHwnd(), PSM_PRESSBUTTON, PSBTN_FINISH, 0);
			//return 0;
		}
	}

	
	CConfigXMLData& data = pApp->GetConfigXMLData();

	switch (pApp->m_enScenario)
	{
	case CUseScenarioEnum::eusCreateNew:
		m_pSheet->SetActivePage((UINT)IDD_OCRCFG_FINISH_PAGE);  // RegJobs
		break;
	case CUseScenarioEnum::eusRemove:
		m_pSheet->SetActivePage((UINT)IDD_OCRCFG_REGJOBS_PAGE); // CreateDB
		break;
	}
 	
	return 0;
}

/*
LRESULT CRegServicesPage::OnCustomScriptExcecutionStart(WPARAM wParam, LPARAM lParam)
{
	m_strStage.LoadString(IDS_OCRCFG_CUSTOMSCRIPT_EXECUTION);
	UpdateData(FALSE);

	return 0;
}


void CRegServicesPage::OnSQLComboChange()
{
	UpdateData();

	int iLasVal = 0;
	iLasVal = m_comboSqlServer.GetCurSel() - 1;

	// check is choosen string is cszSearchItemName
	CString strBuf;
	m_comboSqlServer.GetLBText(m_comboSqlServer.GetCurSel(), strBuf);
	if(strBuf == CString(cszSearchItemName))
	{
		// open dialog
		CSelectServerDlg SelectServerDlg;
		INT_PTR nRet = -1;
		nRet = SelectServerDlg.DoModal();

		switch ( nRet )
		{
			case -1: 
			{
				TException tex(IDE_OCRCFG_ERROR_CREATE_SEARCH_DIALOG);
				tex.ReportError();
				break;
			}
			//case IDABORT:
			//{
			//	// Do something
			//	break;
			//}
			case IDOK:
			{
				// transmit data to parent dialog
				// Get currenl list 
				set<CAtlString> setComboboxServerNames;
				for (int i = 0; i < m_comboSqlServer.GetCount(); i++)
				{
					CString strTemp;
					m_comboSqlServer.GetLBText(i, strTemp);
					if(strTemp == cszSearchItemName)
					{
						continue;
					}
					setComboboxServerNames.insert(CAtlString(strTemp));
				}
				// create list with unique items
				CString strTemp = SelectServerDlg.GetChoosenText();
				set<CAtlString,InsensitiveCompare>lstServers; //нечувствительный к регистру набор
				lstServers.insert(setComboboxServerNames.begin(), setComboboxServerNames.end());
				lstServers.insert(strTemp);
				// clear combobox
				m_comboSqlServer.ResetContent();
				// fill in combobox with new items
				for (set<CAtlString,InsensitiveCompare>::iterator it=lstServers.begin(); it!=lstServers.end(); ++it)
				{
					CString strServer = *it;
					m_comboSqlServer.AddString(strServer);
				}
				m_comboSqlServer.AddString(cszSearchItemName);

				// set cur sel element which was returned from dialog
				iLasVal = m_comboSqlServer.FindStringExact(0, strTemp);
				if(CB_ERR != iLasVal)
				{
					m_comboSqlServer.SetCurSel(iLasVal);
					m_comboSqlServer.GetLBText(m_comboSqlServer.GetCurSel(), strBuf);
				}

				break;
			}
			//case IDCANCEL:
			//{
			//	// Do something
			//	break;
			//}
			default:
			{
				m_comboSqlServer.SetCurSel(iLasVal);
				m_comboSqlServer.GetLBText(m_comboSqlServer.GetCurSel(), strBuf);
				break;
			}
		};
	}

	m_strSqlServer = strBuf;

	UpdateData(FALSE);

	SetButtons();
}
*/
void CRegServicesPage::OnNamesChange()
{
	SetButtons();
}

void CRegServicesPage::OnSkipBtnClick()
{
	UpdateData();

	if ( m_iSkipPage )
	{
/*
		// Дизэйбл кнопки next и отображение сообщения
		if( m_strSqlServer.CompareNoCase( m_strPrevSqlServer ) != 0 || m_strDatabase.CompareNoCase( m_strPrevDatabase ) != 0)
		{
			// disable Next button
			m_pSheet->SetWizardButtons(PSWIZB_BACK);
			UpdateData(FALSE);

			CAtlString strMessage;
			strMessage.LoadString(IDS_OCRCFG_DB_SKIP_STEP);
			CMNLog(_T("[CRegServicesPage::OnWizardNext]: %s"), strMessage);
			AfxMessageBox(strMessage);
		}
		else
		{
			// enable next button
			if(!m_strSqlServer.IsEmpty() && !m_strDatabase.IsEmpty())
			{
				m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
			}
			else
			{
				CAtlString strMessage;
				strMessage.LoadString(IDS_OCRCFG_DB_EMPTY_FIELD);
				CMNLog(_T("[CRegServicesPage::OnWizardNext]: %s"), strMessage);
				AfxMessageBox(strMessage);
			}
		}
*/
	}
	else
	{
		// enable next button
		//if(!m_strLogin.IsEmpty() && !m_strPassword.IsEmpty())
		{
			m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		}
		/*
		else
		{
			CAtlString strMessage;
			strMessage.LoadString(IDS_OCRCFG_DB_EMPTY_FIELD);
			CMNLog(_T("[CRegServicesPage::OnWizardNext]: %s"), strMessage);
			AfxMessageBox(strMessage);
		}*/
	}

	UpdateData(FALSE);
}