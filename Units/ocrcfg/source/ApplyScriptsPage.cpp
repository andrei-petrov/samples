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

#include "t/include/Misc.h"
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
#include "ocrcfg/include/ApplyScriptsThread.h"
#include "ocrcfg/include/ApplyScriptsPage.h"
#include "ocrcfg/resource/resource.h"


#pragma warning(disable: 4800)

const TCHAR cszSearchItemName[] = _T("<Поиск...>");

// CApplyScriptsPage dialog

//IMPLEMENT_DYNAMIC(CApplyScriptsPage, WZRPage)

CApplyScriptsPage::CApplyScriptsPage(COcrConfigData& rConfigData, int* pnModificationFlags)
	: WZRPage(IDD_OCRCFG_APPLYSCRIPTS_PAGE, IDS_OCRCFG_APPLYSCRIPTS_TITLE, IDS_OCRCFG_APPLYSCRIPTS_SUBTITLE),
	m_rConfigData(rConfigData),	
	m_pnModificationFlags(pnModificationFlags),
	m_bError(true)
{    
	m_psp.dwFlags |= PSP_HASHELP;

	if(!m_pnModificationFlags)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_POINTER_TO_INT_IS_NULL);
		*pEx << _T("CApplyScriptsPage::CApplyScriptsPage");
		ThrowException(pEx);
	}
}

CApplyScriptsPage::~CApplyScriptsPage()
{
	
}

BEGIN_MESSAGE_MAP(CApplyScriptsPage, WZRPage)
    //{{AFX_MSG_MAP(CApplyScriptsPage)    
	ON_MESSAGE		( PSM_QUERYSIBLINGS,						OnQuerySiblings)
    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_START,		OnScriptExcecutionStart  )
    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_STAGE,		OnScriptExcecutionStage  )
    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_FINISH,		OnScriptExcecutionFinish )
    //ON_MESSAGE      ( WM_OCRCFG_CUSTOMSCRIPT_EXCECUTION_START,	OnCustomScriptExcecutionStart )	
	ON_BN_CLICKED	( IDC_WZR_CHECK_SKIP, OnSkipBtnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CApplyScriptsPage::DoDataExchange(CDataExchange* pDX)
{
  	WZRPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CApplyScriptsPage)
	DDX_Control(pDX, IDC_OCRCFG_APPLYSCRIPTS_LIST, m_lbScripts);

	/*
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
		for (iItem = 0; iItem < m_lbScripts.GetCount(); iItem++)
		{
			bool bCheck = true;
			m_lbScripts.SetCheck(iItem, bCheck);
		}
	}
	*/
    //}}AFX_DATA_MAP
}


BOOL CApplyScriptsPage::OnInitDialog()
{
    WZRPage::OnInitDialog();

	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if (!pApp)
		return FALSE;

	CConfigXMLData& rXmlConfigData = pApp->GetConfigXMLData();
	   
	for (auto strPath : rXmlConfigData.GetScriptFilePaths())
	{
		CString strDirectory, strFileName;
		TSplitFullFileName(strPath, strDirectory, strFileName);
		m_lbScripts.AddString(strFileName);
	}

	return TRUE;
}


BOOL CApplyScriptsPage::OnSetActive()
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if ( ! pApp ) 
		return FALSE;

	UpdateData(FALSE);

    // in installer mode 'skip' checkbox is not visible
    if (pApp->m_bInstallerMode)
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


BOOL CApplyScriptsPage::OnKillActive()
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
			CMNLog(_T("[CApplyScriptsPage::OnKillActive]: %s"), strAllErrors);		
			pEx->Delete();
		}
	}
	*/
	return WZRPage::OnKillActive();
}

LRESULT CApplyScriptsPage::OnWizardBack()
{
	return IDD_OCRCFG_CREATEDB_PAGE;
}

LRESULT CApplyScriptsPage::OnWizardNext()
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

			//Вернуть предыдущие значения
			//if( m_strSqlServer.CompareNoCase( m_strPrevSqlServer ) != 0 
			//	|| m_strDatabase.CompareNoCase( m_strPrevDatabase ) != 0 )
			//{
			//	CAtlString strMessage;
			//	strMessage.LoadString(IDS_OCRCFG_DB_SKIP_STEP);
			//	CMNLog(_T("[CApplyScriptsPage::OnWizardNext]: %s"), strMessage);
			//	AfxMessageBox(strMessage);
			//	m_strSqlServer = m_strPrevSqlServer;
			//	m_strDatabase  = m_strPrevDatabase;
			//	UpdateData(FALSE);
			//	return -1;
			//}
			
			//OnKillActive();

			return IDD_OCRCFG_REGJOBS_PAGE;
		}
		
		ApplyScripts();
		//m_bError = false;
		return -1; // stay at this page until install completes
	}
	catch(TException* pTEx)
	{
		CAtlString strErrMessage = TException::GetAllErrorMessages(pTEx);
		CMNLog(_T("[CApplyScriptsPage::OnWizardNext]: %s"), strErrMessage);
		pTEx->ReportError();
		pTEx->Delete();
		return -1;
	}
}


void CApplyScriptsPage::SetButtons()
{
	/*
	if ( ! theApp.m_bUnattendedMode)
	{
		UpdateData();

		// check is sql server name and DB name fields is empty
		if(m_strSqlServer.IsEmpty() || m_strDatabase.IsEmpty())
		{
			// disable Next button
			m_pSheet->SetWizardButtons(PSWIZB_BACK);
		}
		else
		{
			// enable next button
			m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		}
	}
	*/
}

WZRThread* CApplyScriptsPage::ConstructThread() const
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if ( ! pApp ) 
		return NULL;
	CConfigXMLData& rXmlConfigData = pApp->GetConfigXMLData();

	return new CApplyScriptsThread( GetSafeHwnd(), m_rConfigData, rXmlConfigData, m_pnModificationFlags );
}

LRESULT CApplyScriptsPage::ApplyScripts()
{	
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if ( !pApp )  
		return 0;
	CConfigXMLData& rXmlConfigData = pApp->GetConfigXMLData();

	int nScripts = 0;
		
	/*
	set<CString> setScripts = data.GetScriptFilePaths();
	DBStorage::FillScriptIDVector();	
	if ( data.IsCreateServer() )
	{		
		nScripts += DBStorage::m_vecScriptID.size();
	}
	*/
	//if ( data.IsRunSqlScripts() )
	{
		nScripts += rXmlConfigData.GetScriptFilePaths().size();
	}
	
	//m_ctlProgress.SetRange((short) 0, (short) ( nScripts > 0 ? (nScripts-1) : 0 ));	
	m_ctlProgress.SetRange((short)0, (short)(nScripts + 1));
    m_ctlProgress.SetStep(1);
	m_ctlProgress.SetPos(0);
    OnThreadStarted();
	
    return 0;
}

// CApplyScriptsPage message handlers


LRESULT CApplyScriptsPage::OnQuerySiblings(WPARAM wParam, LPARAM lParam)
{
    if (wParam == WM_OCRCFG_QUERY_APPLYSCRIPTS)
    {
        ::memcpy((void*) lParam, (void*) m_strThreadResults.GetString(), m_strThreadResults.GetLength() * sizeof(TCHAR));
		return 1; // answer
    }	
    return 0; // ignore
}

LRESULT CApplyScriptsPage::OnScriptExcecutionStart (WPARAM wParam, LPARAM lParam)
{
	m_strStage.FormatMessage(IDS_OCRCFG_SCRIPT_EXECUTION_PROCESS);
	UpdateData(FALSE);
	return 0;
}

LRESULT CApplyScriptsPage::OnScriptExcecutionStage (WPARAM wParam, LPARAM lParam)
{
	CString strFormat;
	DWORD dwPercent = (DWORD) wParam;	
    DWORD iPos = (DWORD)m_ctlProgress.GetPos();
	TRACE(_T("[CApplyScriptsPage::OnScriptExcecutionStage] %i, %i\n"), dwPercent, iPos);
    if (iPos < dwPercent)
    {
        m_ctlProgress.StepIt();
		TRACE(_T("[CApplyScriptsPage::OnScriptExcecutionStage] StepIt()\n"));
		ShowProgress();
    }
    return 0;
}

LRESULT CApplyScriptsPage::OnScriptExcecutionFinish (WPARAM wParam, LPARAM lParam)
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

	m_pSheet->SetActivePage(IDD_OCRCFG_REGJOBS_PAGE);
	/*
	if (data.IsDeployReports())
		m_pSheet->SetActivePage(iActivePage + 1);
	else if (data.IsImportSoftLibrary())
		m_pSheet->SetActivePage(iActivePage + 2);
	else
		m_pSheet->SetActivePage(iActivePage + 3);
	*/
	return 0;
}

/*
LRESULT CApplyScriptsPage::OnCustomScriptExcecutionStart(WPARAM wParam, LPARAM lParam)
{
	m_strStage.LoadString(IDS_OCRCFG_CUSTOMSCRIPT_EXECUTION);
	UpdateData(FALSE);

	return 0;
}
*/

void CApplyScriptsPage::OnNamesChange()
{
	SetButtons();
}

void CApplyScriptsPage::OnSkipBtnClick()
{
	UpdateData();

	if ( m_iSkipPage )
	{
		m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		/*
		// Дизэйбл кнопки next и отображение сообщения
		if( m_strSqlServer.CompareNoCase( m_strPrevSqlServer ) != 0 || m_strDatabase.CompareNoCase( m_strPrevDatabase ) != 0)
		{
			// disable Next button
			m_pSheet->SetWizardButtons(PSWIZB_BACK);
			UpdateData(FALSE);

			CAtlString strMessage;
			strMessage.LoadString(IDS_OCRCFG_DB_SKIP_STEP);
			CMNLog(_T("[CApplyScriptsPage::OnWizardNext]: %s"), strMessage);
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
				CMNLog(_T("[CApplyScriptsPage::OnWizardNext]: %s"), strMessage);
				AfxMessageBox(strMessage);
			}
		}
		*/
	}
	else
	{
		m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		/*
		//	enable next button
		if(!m_strSqlServer.IsEmpty() && !m_strDatabase.IsEmpty())
		{
			m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		}
		else
		{
			CAtlString strMessage;
			strMessage.LoadString(IDS_OCRCFG_DB_EMPTY_FIELD);
			CMNLog(_T("[CApplyScriptsPage::OnWizardNext]: %s"), strMessage);
			AfxMessageBox(strMessage);
		}
		*/
	}

	UpdateData(FALSE);
}