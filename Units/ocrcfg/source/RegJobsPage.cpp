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
#include "ocrcfg/include/RegJobsThread.h"
#include "ocrcfg/include/RegJobsPage.h"
#include "ocrcfg/resource/resource.h"
//#include "samcfg/resource/samcfg.rc"

#pragma warning(disable: 4800)

const TCHAR cszSearchItemName[] = _T("<Поиск...>");

// CRegJobsPage dialog

//IMPLEMENT_DYNAMIC(CRegJobsPage, WZRPage)

CRegJobsPage::CRegJobsPage(COcrConfigData& rConfigData, int* pnModificationFlags  )
	: WZRPage(IDD_OCRCFG_REGJOBS_PAGE, IDS_OCRCFG_REGJOBS_TITLE, IDS_OCRCFG_REGJOBS_SUBTITLE),
	m_bError(true),	
	m_rConfigData(rConfigData),
	m_pnModificationFlags(pnModificationFlags)
{    
	m_psp.dwFlags |= PSP_HASHELP;

	if(!m_pnModificationFlags)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_POINTER_TO_INT_IS_NULL);
		*pEx << _T("CRegJobsPage::CRegJobsPage");
		ThrowException(pEx);
	}

	if (m_vecJobs.size() == 0)
	{
		//m_vecJobs.push_back(new CDeliceOcrJobDesc(_T("copyTaskResultsJob"),		_T("copyTaskResultsJob"))	);
		m_vecJobs.push_back(new CDeliceOcrJobDesc(_T("setCodesTaskJob"),		_T("setCodesTaskJob"))		);
		m_vecJobs.push_back(new CDeliceOcrJobDesc(_T("setIndexingTaskJob"),		_T("setIndexingTaskJob"))	);
		m_vecJobs.push_back(new CDeliceOcrJobDesc(_T("setOcrTaskJob"),			_T("setOcrTaskJob"))        );
		m_vecJobs.push_back(new CDeliceOcrJobDesc(_T("setPdfFlagJob"),			_T("setPdfFlagJob"))		);
		m_vecJobs.push_back(new CDeliceOcrJobDesc(_T("setPdfTaskJob"),			_T("setPdfTaskJob"))		);
		m_vecJobs.push_back(new CDeliceOcrJobDesc(_T("setPicturesSortTaskJob"), _T("setPicturesSortTaskJob")) );
		m_vecJobs.push_back(new CDeliceOcrJobDesc(_T("RecalcStatistics"),		_T("RecalcStatistics"))	);
	}
}

CRegJobsPage::~CRegJobsPage()
{
	
}

BEGIN_MESSAGE_MAP(CRegJobsPage, WZRPage)
    //{{AFX_MSG_MAP(CRegJobsPage)    
	ON_MESSAGE		( PSM_QUERYSIBLINGS,			OnQuerySiblings)
    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_START, OnScriptExcecutionStart  )
    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_STAGE, OnScriptExcecutionStage  )
    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_FINISH, OnScriptExcecutionFinish )

	ON_CONTROL		(CLBN_CHKCHANGE, IDC_OCRCFG_REGSVC_LIST, OnNamesChange)

//    ON_MESSAGE      ( WM_OCRCFG_CUSTOMSCRIPT_EXCECUTION_START, OnCustomScriptExcecutionStart )
	ON_BN_CLICKED	( IDC_WZR_CHECK_SKIP, OnSkipBtnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CRegJobsPage::DoDataExchange(CDataExchange* pDX)
{
  	WZRPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRegJobsPage)
	DDX_Control(pDX, IDC_OCRCFG_REGJOBS_LIST, m_lbJobs);

	int iItem = 0;
	if (pDX->m_bSaveAndValidate)
	{
		for (iItem = 0; iItem < m_lbJobs.GetCount(); iItem++)
		{
			bool bCheck = m_lbJobs.GetCheck(iItem);
			m_vecJobs[iItem]->m_bCheck = bCheck;
		}
	}
	else
	{
		for (iItem = 0; iItem < m_lbJobs.GetCount(); iItem++)
		{
			bool bCheck = m_vecJobs[iItem]->m_bCheck;
			m_lbJobs.SetCheck(iItem, bCheck);
		}
	}

    //}}AFX_DATA_MAP
}


BOOL CRegJobsPage::OnInitDialog()
{
    WZRPage::OnInitDialog();

	for (auto spJob : m_vecJobs)
	{
		CString str = spJob->m_strDisplayName;
		m_lbJobs.AddString(str);
	}

	return TRUE;
}


BOOL CRegJobsPage::OnSetActive()
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if ( ! pApp ) return FALSE;
		
	UpdateData(FALSE);

	if (pApp->m_enScenario == CUseScenarioEnum::eusRemove)
	{
		CString strTitle, strSubtitle;
		strTitle.LoadString(IDS_OCRCFG_UNREGJOBS_TITLE);
		strSubtitle.LoadString(IDS_OCRCFG_UNREGJOBS_SUBTITLE);
		PropSheet_SetHeaderTitle(m_pSheet->GetSafeHwnd(), m_pSheet->GetActiveIndex(), (const TCHAR*)strTitle);
		PropSheet_SetHeaderSubTitle(m_pSheet->GetSafeHwnd(), m_pSheet->GetActiveIndex(), (const TCHAR*)strSubtitle);

		CWnd *pControl = GetDlgItem(IDC_OCRCFG_REGJOBS_LIST);
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


BOOL CRegJobsPage::OnKillActive()
{
	/*
	if(!m_bError)
	{
		CMNRegKey keyRoot;
		try
		{
			keyRoot.OpenEx(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot, KEY_WRITE);		
			//CMNCheckWIN32Error( keyRoot.SetStringValue(cszSAMSqlServer, m_strSqlServer) );	
			CMNCheckWIN32Error( keyRoot.SetStringValue(cszSAMSqlServer, m_strSqlServerHostName));
			CMNCheckWIN32Error( keyRoot.SetStringValue(cszSAMDatabase, m_strDatabase) );	
			
		}
		catch(CMNException* pEx)
		{	
			CString strAllErrors = CMNException::GetAllErrorMessages(pEx, _T("\n"));
			CMNLog(_T("[CRegJobsPage::OnKillActive]: %s"), strAllErrors);		
			pEx->Delete();
		}
	}
	*/
	return WZRPage::OnKillActive();
}

LRESULT CRegJobsPage::OnWizardBack()
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());

	switch (pApp->m_enScenario)
	{
	case CUseScenarioEnum::eusCreateNew:
		return IDD_OCRCFG_APPLYSCRIPTS_PAGE;				
	case CUseScenarioEnum::eusRemove:
		return IDD_OCRCFG_REGSVC_PAGE;				
	}
	return -1;
}

LRESULT CRegJobsPage::OnWizardNext()
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

		if (m_iSkipPage)
		{
			m_bError = false;
			return IDD_OCRCFG_REGSVC_PAGE;
		}
		
		ProcessJobs();

		return -1; // stay at this page until install completes
	}
	catch(TException* pTEx)
	{
		CAtlString strErrMessage = TException::GetAllErrorMessages(pTEx);
		CMNLog(_T("[CRegJobsPage::OnWizardNext]: %s"), strErrMessage);
		pTEx->ReportError();
		pTEx->Delete();
		return -1;
	}
}


void CRegJobsPage::SetButtons()
{
	if ( ! theApp.m_bUnattendedMode)
	{
		UpdateData();

		// disable Next button by default
		DWORD dwFlags = PSWIZB_BACK;

		// check for at least one check
		for (int iItem = 0; iItem < m_lbJobs.GetCount(); iItem++)
		{
			bool bCheck = m_lbJobs.GetCheck(iItem);
			if (bCheck)
			{
				dwFlags = (PSWIZB_BACK | PSWIZB_NEXT);
				break;
			}
		}
		m_pSheet->SetWizardButtons(dwFlags);
	}
}

WZRThread* CRegJobsPage::ConstructThread() const
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if ( ! pApp ) 
		return NULL;
	CConfigXMLData& data = pApp->GetConfigXMLData();

	return new CRegJobsThread( GetSafeHwnd(), m_vecJobs, m_rConfigData, data, m_pnModificationFlags );
}

LRESULT CRegJobsPage::ProcessJobs()
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
	m_ctlProgress.SetRange((short) 0, (short) ( nScripts > 0 ? (nScripts-1) : 0 ));
	*/
	m_ctlProgress.SetRange((short) 0, (short) m_vecJobs.size());  // CRegJobsThread::GetJobsCount()
    m_ctlProgress.SetStep(1);
	m_ctlProgress.SetPos(0);
    OnThreadStarted();

    return 0;
}

// CRegJobsPage message handlers


LRESULT CRegJobsPage::OnQuerySiblings(WPARAM wParam, LPARAM lParam)
{
    if (wParam == WM_OCRCFG_QUERY_REGJOBS)
    {
        ::memcpy((void*) lParam, (void*) m_strThreadResults.GetString(), m_strThreadResults.GetLength() * sizeof(TCHAR));
		return 1; // answer
    }	
    return 0; // ignore
}

LRESULT CRegJobsPage::OnScriptExcecutionStart (WPARAM wParam, LPARAM lParam)
{
	switch (theApp.m_enScenario)
	{
	case CUseScenarioEnum::eusCreateNew:
		m_strStage.FormatMessage(IDS_OCRCFG_JOBS_CREATE_PROCESS);
		break;
	case CUseScenarioEnum::eusRemove:
		m_strStage.FormatMessage(IDS_OCRCFG_JOBS_REMOVE_PROCESS);
		break;
	}

	UpdateData(FALSE);
	return 0;
}

LRESULT CRegJobsPage::OnScriptExcecutionStage (WPARAM wParam, LPARAM lParam)
{
	CString strFormat;
	DWORD dwPercent = (DWORD) wParam;	
    DWORD iPos = (DWORD)m_ctlProgress.GetPos();
	TRACE(_T("[CRegJobsPage::OnScriptExcecutionStage] %i, %i\n"), dwPercent, iPos);
    if (iPos < dwPercent)
    {
        m_ctlProgress.StepIt();
		TRACE(_T("[CRegJobsPage::OnScriptExcecutionStage] StepIt()\n"));
		ShowProgress();
    }
    return 0;
}

LRESULT CRegJobsPage::OnScriptExcecutionFinish (WPARAM wParam, LPARAM lParam)
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
		m_pSheet->SetActivePage((UINT)IDD_OCRCFG_REGSVC_PAGE);  // Reg Services
		break;
	case CUseScenarioEnum::eusRemove:
		m_pSheet->SetActivePage((UINT)IDD_OCRCFG_CREATEDB_PAGE); // CreateDB
		break;
	}

	return 0;
}

/*
LRESULT CRegJobsPage::OnCustomScriptExcecutionStart(WPARAM wParam, LPARAM lParam)
{
	m_strStage.LoadString(IDS_OCRCFG_CUSTOMSCRIPT_EXECUTION);
	UpdateData(FALSE);

	return 0;
}
*/

void CRegJobsPage::OnNamesChange()
{
	SetButtons();
}

void CRegJobsPage::OnSkipBtnClick()
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
			CMNLog(_T("[CRegJobsPage::OnWizardNext]: %s"), strMessage);
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
				CMNLog(_T("[CRegJobsPage::OnWizardNext]: %s"), strMessage);
				AfxMessageBox(strMessage);
			}
		}
		*/
		m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	}
	else
	{
		/*
		// enable next button
		if(!m_strSqlServer.IsEmpty() && !m_strDatabase.IsEmpty())
		{
			m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		}
		else
		{
			CAtlString strMessage;
			strMessage.LoadString(IDS_OCRCFG_DB_EMPTY_FIELD);
			CMNLog(_T("[CRegJobsPage::OnWizardNext]: %s"), strMessage);
			AfxMessageBox(strMessage);
		}
		*/
		m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	}

	UpdateData(FALSE);
}