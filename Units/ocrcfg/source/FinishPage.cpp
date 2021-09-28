//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------

#include "stdafx.h"
#include <Winsvc.h>

#include "cmn/include/Log.h"
#include "cmn/include/Exception.h"
#include "rgs/include/ServiceController.h"

#include "scm/include/SchemaDef.h"

//#include "SamMgr/include/Constants.h"

#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/misc.h"
#include "ocrcfg/include/ConfigApp.h"
#include "wzr/include/Page.h"
#include "wzr/include/Sheet.h"
#include "ocrcfg/include/FinishPage.h"
#include "ocrcfg/resource/resource.h"

#define SERVER_STOPPED_TIMEOUT 20000

// CFinishPage dialog

//IMPLEMENT_DYNAMIC(CFinishPage, WZRPage)

CFinishPage::CFinishPage(  int* pnModificationFlags )
	: WZRPage( IDD_OCRCFG_FINISH_PAGE, 0, 0),
	m_pnModificationFlags(pnModificationFlags)
{
    //{{AFX_DATA_INIT(CFinishPage)    
    /*if (!theApp.m_bInstallerMode && !theApp.m_bUnattendedMode)
        m_iRestartService = 0;
    else
        m_iRestartService = 1;
		*/
	m_iUninstall = BST_UNCHECKED;

    //}}AFX_DATA_INIT    
    m_psp.dwFlags |= PSP_HIDEHEADER; // no header, just watermark on left part of the page    
	m_pWndTitle = NULL;

	if(!m_pnModificationFlags)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_POINTER_TO_INT_IS_NULL);
		*pEx << _T("CFinishPage::CFinishPage");
		ThrowException(pEx);
	}

}

CFinishPage::~CFinishPage()
{
}

//BEGIN_MESSAGE_MAP(CFinishPage, WZRPage)
//END_MESSAGE_MAP()

void CFinishPage::DoDataExchange(CDataExchange* pDX)
{
	WZRPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CFinishPage)
	DDX_Control	(pDX, IDC_OCRCFG_FINISH_CHECK_UNINSTALL, m_btnUninstall);														 
    DDX_Check	(pDX, IDC_OCRCFG_FINISH_CHECK_UNINSTALL, m_iUninstall);
    //}}AFX_DATA_MAP
}


BOOL CFinishPage::OnInitDialog()
{
    BOOL b = WZRPage::OnInitDialog();
    m_pWndTitle = GetDlgItem (IDC_OCRCFG_FINISH_STATIC_TITLE);
    if (m_pWndTitle)
    {        
        if (!m_fontTitle.CreateFont(0x12, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
                DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE, _T("Verdana")))
		{
			ASSERT(!"Cannot create font");
			return false;
		}

        m_pWndTitle->SetFont(&m_fontTitle, true);
    }
    return b;
}

BOOL CFinishPage::OnSetActive()
{
	CWnd* pWnd = NULL;
    BOOL b = WZRPage::OnSetActive();

	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if (!pApp)
		return FALSE;

    TCHAR szSiblingInfo[65536] = {0};
    
    LRESULT lRes;	

	//Собрать информацию со страниц конфигуратора
	int siblings[] = {	
						WM_OCRCFG_QUERY_CREDENTIALS,
						WM_OCRCFG_QUERY_CREATEFOLDER,
						WM_OCRCFG_QUERY_CREATEDB,
						WM_OCRCFG_QUERY_APPLYSCRIPTS,
						WM_OCRCFG_QUERY_REGJOBS,
						WM_OCRCFG_QUERY_REGSVC,
						WM_OCRCFG_QUERY_DEPLOYRPT,
						WM_OCRCFG_QUERY_SELECTSERVERS,
						WM_OCRCFG_QUERY_SOFTLIBRARY
						//WM_OCRCFG_QUERY_DCOM
					};

	for (int i=0; i<(sizeof(siblings)/sizeof(siblings[0])); i++)
	{
		int sibling_id = siblings[i];
		memset(szSiblingInfo, 0, sizeof(szSiblingInfo));
		lRes = QuerySiblings(sibling_id, (LPARAM)&szSiblingInfo);
		if ( lRes )
		{
			if ( ! m_strInfo.IsEmpty() && szSiblingInfo[0] != 0x0)
			{
				m_strInfo += L"\r\n\r\n";
			}

			m_strInfo += szSiblingInfo;
		}
	}

	if (m_strInfo.GetLength() == 0)
    {
        m_strInfo.LoadString(IDS_OCRCFG_FINISH_RESULTS);
    }
	//Отобразить информацию о результатах работы конфигуратора
    SetDlgItemText(IDC_OCRCFG_FINISH_EDIT_INFO, m_strInfo);

	// Удалять ли дистрибутив?
	if (pApp->m_enScenario == CUseScenarioEnum::eusCreateNew)
	{
		m_btnUninstall.ShowWindow(FALSE);
		m_btnUninstall.EnableWindow(FALSE);
		m_iUninstall = BST_UNCHECKED;
	}
	if (pApp->m_enScenario == CUseScenarioEnum::eusRemove)
	{
		m_btnUninstall.ShowWindow(TRUE);
		m_btnUninstall.EnableWindow(TRUE);
		m_iUninstall = BST_CHECKED;		
	}

	/*
	m_iRestartService = (theApp.m_bInstallerMode || theApp.m_bUnattendedMode 
						|| *m_pnModificationFlags & IsCreateDB 					
						|| *m_pnModificationFlags & IsModifiedDBName 			
						|| *m_pnModificationFlags & IsModifiedSqlServerName 	
						|| *m_pnModificationFlags & IsModifiedReportServerName	
						|| *m_pnModificationFlags & IsModifiedReportsWebCatalog)	
						? BST_CHECKED : BST_UNCHECKED;
						*/

	/*pWnd = GetDlgItem(IDC_OCRCFG_FINISH_CHECK_RESTART);
	if (pWnd)
	{
		//Если уже определено, что нужно перезапускать, то запретить пользователю опцию перезапуска
		pWnd->EnableWindow( m_iRestartService ? true : false );
	}*/

	UpdateData(FALSE);

	//Состояние кнопок
	pWnd = m_pSheet->GetDlgItem(IDCANCEL);
	if (pWnd)
	{
	   pWnd->EnableWindow(FALSE);
	}
	m_pSheet->SetWizardButtons(PSWIZB_FINISH);
	//::PostMessage(m_pSheet->GetSafeHwnd(), PSM_SETWIZBUTTONS, 0, m_dwWizardButtonsFlagNormal);

	if (theApp.m_bUnattendedMode)
		::PostMessage(m_pSheet->GetSafeHwnd(), PSM_PRESSBUTTON, PSBTN_FINISH, 0);
	return b;
}

BOOL CFinishPage::OnWizardFinish()
{
	try
    {
		UpdateData(TRUE);

		if (m_iUninstall)
		{
			Uninstall();
			//RestartServer();	  			
		}						  
		else if( *m_pnModificationFlags & IsInstallReports ) 
		{
			//RefreshClient();
		}
	}							  
    catch(TException* pTEx)
    {   
		CString strError = TException::GetAllErrorMessages(pTEx, _T("\r\n"));
		CMNLog(_T("[CFinishPage::OnWizardFinish]: %s"), strError);

		if( pTEx->GetErrorCode() == IDE_OCRCFG_SERVER_STOPPED_TIMEOUT_EXPIRED )
		{
			TException* pHiLevelEx = new TException(IDE_OCRCFG_SERVER_STOPPED_TIMEOUT_EXPIRED);
			*pHiLevelEx += *pTEx;
			if ( ! theApp.m_bUnattendedMode )
			{
				pHiLevelEx->ReportError();
			}
			pHiLevelEx->Delete();

			return TRUE;
		}

		if ( ! theApp.m_bUnattendedMode )
		{
			pTEx->ReportError();
		}
		pTEx->Delete();
    }

    return TRUE;
}


void CFinishPage::Uninstall()
{
	TCHAR pszCmdLine[] = _T("MsiExec.exe /X{B8A022B9-9770-486B-98E9-3FCCE7A06F00}");	                            

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	
	// Start the child process. 
	if (!CreateProcess(NULL,	// No module name (use command line)
		pszCmdLine,    // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		;
	}

	// Wait until child process exits.
	//WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	/*
	if (!strProgramToRun.IsEmpty())
	{
		SHELLEXECUTEINFO execInfo;
		::memset(&execInfo, 0, sizeof(SHELLEXECUTEINFO));
		execInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		execInfo.lpFile = strProgramToRun;
		execInfo.lpVerb = _T("open");
		execInfo.nShow = SW_SHOWNORMAL;
		BOOL bRes = ::ShellExecuteEx(&execInfo);
	}
	*/
	return;
}


void CFinishPage::RefreshClient()
{
	/*
	CWnd* pIMWnd = CWnd::FindWindow(_T("IMMainFrameWnd"), NULL);

	if(pIMWnd && ::IsWindow(pIMWnd->GetSafeHwnd()))
	{
		while(FALSE == pIMWnd->PostMessage(WM_COMMAND, ID_SAMMGR_ACTIONS_REFRESH, 0))
		{
		}
	}
	*/
}

void CFinishPage::RestartServer()
{
	RGSServiceController   controller((bstr_t)L"", (bstr_t)cszSAMSerivceName); 
	/*
	CWnd* pIMWnd = CWnd::FindWindow(_T("IMMainFrameWnd"), NULL);
	if(pIMWnd && ::IsWindow(pIMWnd->GetSafeHwnd()))
	{
		while(FALSE == pIMWnd->PostMessage(WM_COMMAND, ID_SAMMGR_FILE_DISCONNECT, 0))
		{
		}
		Sleep(3000);
	}
	*/
    controller.OpenSCMgr();
    SC_HANDLE hService =controller.Open();
	if( !hService )
	{
		CMNCheckWIN32Error(GetLastError());;
	}

    m_pSheet->BeginWaitCursor();
    if( !controller.Stop(SERVER_STOPPED_TIMEOUT) )
	{
		ThrowException(new CMNException(IDE_OCRCFG_SERVER_STOPPED_TIMEOUT_EXPIRED));
	}
    controller.Start();
    m_pSheet->EndWaitCursor();
    controller.Close();
    controller.CloseSCMgr();                    
	/*
	if(pIMWnd && ::IsWindow(pIMWnd->GetSafeHwnd()))
	{
		while(FALSE == pIMWnd->PostMessage(WM_COMMAND, ID_SAMMGR_FILE_FORCECONNECT, 0))
		{
		}
	}
	*/
}
