//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:  Configurator
//--------------------------------------------------------------------------
//  Description: Thread for background operations
//--------------------------------------------------------------------------

#include "stdafx.h"

#include "resource.h"

#include "cmn/include/misc.h"
#include "cmn/include/Log.h"
#include "cmn/include/Exception.h"
#include "cmn/include/VersionInfo.h"

#include "db/include/Constants.h"
#include "db/include/Storage.h"

#include "rgs/include/ServiceController.h"

#include "scm/include/SchemaDef.h"

#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/ConfigApp.h"
#include "wzr/include/Thread.h"
#include "ocrcfg/include/RegServicesThread.h"
#include "ocrcfg/include/misc.h"


#pragma warning(disable: 4800)


// CRegServicesThread



//IMPLEMENT_DYNCREATE(CRegServicesThread, CWinThread)

CRegServicesThread::CRegServicesThread(HWND hParentWnd, CDeliceOcrModuleDesc::Vector rvecModules, COcrConfigData& rConfigData,
	CConfigXMLData& config, int* pnModificationFlags)
	: WZRThread(hParentWnd),
	//m_config(config),
	m_vecModules(rvecModules),
	m_rConfigData(rConfigData),
	m_pnModificationFlags(pnModificationFlags)
{	
	if(!m_pnModificationFlags)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_POINTER_TO_INT_IS_NULL);
		*pEx << _T("CRegServicesThread::CRegServicesThread");
		ThrowException(pEx);
	}	
}

CRegServicesThread::~CRegServicesThread()
{
}

BOOL CRegServicesThread::InitInstance()
{
	CoInitialize(NULL);
    CWinThread::InitInstance();

//	BOOL	bRun = true;
	BOOL	bError				= FALSE;
	int		nRes				= IDNO;
	BOOL	bExist				= FALSE;
	BOOL	bCreated			= FALSE;
	//BOOL	bOnlyUseDatabase	= FALSE; // do not create, but use selected database(s)
    //BOOL    bGranted			= FALSE;
	int		nCount = 0;
	CString strGrantError;

	//::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_START, NULL, NULL);
	try
	{		
		if (theApp.m_enScenario == CUseScenarioEnum::eusCreateNew ) // register services
		{
			//CheckSQLServerCompatibility();
			bExist = AreAnyServicesRegistered();
			if (bExist)
			{
				// who is creator of data base? I - ok, some one else - throw exception
				CString strServices = CheckOurServices();				

				if (theApp.m_bUnattendedMode)
					nRes = IDYES;
				else
				{
					CString strQuestion; 
					strQuestion.FormatMessage(IDS_OCRCFG_SVC_RECREATE_QUESTION, strServices, m_rConfigData.m_strComputerAddr);
					nRes = AfxMessageBox(strQuestion, MB_ICONQUESTION | MB_YESNO);
				}
			}

			if (bExist) // some serivces already registered
			{
				if (nRes == IDYES)
				{
					nCount = CreateServices(); // TRUE					
				}
			}
			else // no serivces already registered
			{
				nCount = CreateServices(); // FALSE				
			}
		}

		if (theApp.m_enScenario == CUseScenarioEnum::eusRemove) // unregister services
		{
			nCount = RemoveServices();					   
		}

		/*
		if ( m_config.IsRunSqlScripts() )
		{
			nScripts = RunSqlScripts();
		}
		*/
		/*
		if ( bCreated == TRUE )
		{
			for (auto pDatabase : m_vecDatabases)
			{
				CString str;
				str.FormatMessage(IDS_OCRCFG_DB_RESULTS, pDatabase->m_strDatabase, m_strSqlServerUserEnterName);
				m_strThreadResults += str;
				m_strThreadResults += _T(" ");
			}
    	    
            if (bGranted)
            {
				for (auto pDatabase : m_vecDatabases)
				{
					CString str;
					str.FormatMessage(IDS_OCRCFG_DB_GRANTED, m_strServiceLogin, pDatabase->m_strDatabase);
					m_strThreadResults += _T(" ");
					m_strThreadResults += str;
				}
            }
			else
			{
				if ( !strGrantError.IsEmpty() )
				{
					bError = TRUE;
					for (auto pDatabase : m_vecDatabases)
					{
						CString str;
						str.FormatMessage(IDS_OCRCFG_DB_GRANT_ERROR, m_strServiceLogin, pDatabase->m_strDatabase, strGrantError);
						m_strError += str;
						m_strError += _T(" ");
					}
				}
			}
            CMNLog(_T("[CRegServicesThread::InitInstance]: %s"), m_strThreadResults);
		}
		*/
		/*
		if ( bOnlyUseDatabase == TRUE )
		{
			for (auto pDatabase : m_vecDatabases)
			{
				CString str;
				str.FormatMessage(IDS_OCRCFG_DB_USE, pDatabase->m_strDatabase, m_strSqlServerUserEnterName);
				m_strThreadResults += str;
				m_strThreadResults += _T(" ");
			}
			CMNLog(_T("[CRegServicesThread::InitInstance]: %s"), m_strThreadResults);
		}
		*/
		
		if ( nCount > 0 )
		{
			CString str;
			switch (theApp.m_enScenario)
			{
			case CUseScenarioEnum::eusCreateNew:
				str.Format(IDS_OCRCFG_SVC_CREATED, (LPCTSTR)m_rConfigData.m_strSQLServerAddr, nCount);				
				break;
			case CUseScenarioEnum::eusRemove:
				str.Format(IDS_OCRCFG_SVC_REMOVED, (LPCTSTR)m_rConfigData.m_strSQLServerAddr, nCount);				
				break;
			}
			if ( m_strThreadResults.GetLength() ) 
				m_strThreadResults += _T(" ");
			m_strThreadResults += str;
			CMNLog(_T("[CRegServicesThread::InitInstance]: %s"), m_strThreadResults);
		}
		
	}
	catch(CMNException* pEx)
	{	    
		//[Patch] Закрыть соединение если открыто
		//Деструктор m_storage не удаляет ConnectionPool, а повторный запуск потока,
		//и соответственно новый вызов m_storage.Connect вызовет пересоздание ConnectionPool,
		//которое завершится ошибкой.
//		if(m_storage.ConnectionState()==1)
//			m_storage.Disconnect();

		m_strError = CMNException::GetAllErrorMessages(pEx, _T("\n"));
        CMNLog(_T("[CRegServicesThread::InitInstance]: %s"), m_strError);
		bError = TRUE;
		pEx->Delete();
	}

	::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_FINISH, bError, NULL);	
	
	return TRUE;
}


int CRegServicesThread::ExitInstance()
{
	CoUninitialize();
	return CWinThread::ExitInstance();
}


//BEGIN_MESSAGE_MAP(CRegServicesThread, CWinThread)
//END_MESSAGE_MAP()

// Implementation

// TEMPLATES

bool CRegServicesThread::AreAnyServicesRegistered()
{
	bool bExist = false;

	RGSServiceController    controller((bstr_t)m_rConfigData.m_strComputerAddr);
	controller.OpenSCMgr();

	for (auto spDeliceModule : m_vecModules)
	{	
		if (spDeliceModule->m_bCheck == false || spDeliceModule->m_bIsService == false)
			continue;

		try
		{			
			if (controller.Open(spDeliceModule->m_strName) != NULL)
			{
				bExist = true;
				break;
			}
		}
		catch (CMNException* pEx)
		{
			CString strError = CMNException::GetAllErrorMessages(pEx, _T("\n"));
			CMNLog(_T("[CRegServicesThread::AreAnyServicesRegistered]: %s"), strError);
			pEx->Delete();
		}
	}
	controller.CloseSCMgr();
	return bExist;
}


CString CRegServicesThread::CheckOurServices()
{
	CString strDisplayNames;

	RGSServiceController    controller((bstr_t)m_rConfigData.m_strComputerAddr);
	controller.OpenSCMgr();

	for (auto spDeliceModule : m_vecModules)
	{
		if (spDeliceModule->m_bCheck == false || spDeliceModule->m_bIsService == false)
			continue;

		try
		{
			if (controller.Open(spDeliceModule->m_strName) != NULL)
			{
				strDisplayNames += spDeliceModule->m_strDisplayName;
				strDisplayNames += _T(", ");
			}
		}
		catch (CMNException* pEx)
		{
			CString strError = CMNException::GetAllErrorMessages(pEx, _T("\n"));
			CMNLog(_T("[CRegServicesThread::CheckOurServices]: %s"), strError);
			pEx->Delete();
		}
	}
	controller.CloseSCMgr();

	int nLength = strDisplayNames.GetLength();
	if (nLength > 1)
	{
		if (strDisplayNames[nLength - 2] == WCHAR(','))
		{
			strDisplayNames.SetAt(nLength - 2, WCHAR(' '));
		}
	}
	return strDisplayNames;
}


int CRegServicesThread::CreateServices()
{
	// we need to do several things: exceuite every binary with the /install option
	// correct Dispay Name and Description in SСM
	::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_START, NULL, NULL);

	// run scripts
	int nCount = 0;
	CString strDisplayNames;

	RGSServiceController    controller((bstr_t)m_rConfigData.m_strComputerAddr);
	controller.OpenSCMgr();
	SC_HANDLE	hService;

	
	for (auto spDeliceModule : m_vecModules)
	{		
		if (spDeliceModule->m_bCheck == false)
			continue;

		if (spDeliceModule->m_bIsService)
		{
			bool bStopped = true;
			bool bDeleted = true;
			// if registered try to stop and delete
			hService = controller.Open(spDeliceModule->m_strName);
			if (hService != NULL)
			{
				bStopped = controller.Stop(10000);	// 30 sec
				if (!bStopped)
				{
					CMNKillProcessByName(spDeliceModule->m_strModuleFileName);
					bStopped = true; // killed == stopped
				}
				if (bStopped == true)
					bDeleted = controller.Delete(); 	// delete it before reinstalling			
				controller.Close();
				CMNLog(_T("[CRegServicesThread::CreateServices]: stopping and deleting service = %s, stop = %i, delete = %i"), spDeliceModule->m_strName, bStopped, bDeleted);
			}
			
			// never try to reinstall registered and running service
			if (bStopped && bDeleted)
			{
				try
				{
					CString strFullModuleFileName = spDeliceModule->GetFullModuleFileName();
					// format Connection string using dabaase name coded into spDeliceModule and externaly entered SQLServerAddr				
					hService = controller.Create(spDeliceModule->m_strName, strFullModuleFileName, spDeliceModule->m_strDisplayName, m_rConfigData.m_strLogin, m_rConfigData.m_strPassword);
					CMNLog(_T("[CRegServicesThread::CreateServices]: installing service = %s, file = %s, result = %i"), spDeliceModule->m_strName, strFullModuleFileName, (hService != NULL));				
					WriteConnectionStringAndFlags(spDeliceModule);
					controller.SetDescription(spDeliceModule->m_strDescription);					
					controller.Start();
				}
				catch (CMNException* pEx)
				{
					TException* pHiLevelEx = new TException(IDE_OCRCFG_SERVICE_NOT_STARTED);
					*pHiLevelEx << (LPCTSTR)spDeliceModule->m_strName;
					*pHiLevelEx += *pEx;
					pHiLevelEx->ReportError();
					pHiLevelEx->Delete();
				}
			}
		}
		else
		{
			WriteConnectionStringAndFlags(spDeliceModule);
		}
		
		nCount++;
		::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_STAGE, nCount, NULL);
		::Sleep(1000); // Wait for a second to apply progres bar changes		
	}	
	controller.CloseSCMgr();

	return nCount;
}



int CRegServicesThread::RemoveServices()
{
	::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_START, NULL, NULL);

	// run scripts
	int nCount = 0;
	int nRemoved = 0;
	CString strDisplayNames;

	RGSServiceController    controller((bstr_t)m_rConfigData.m_strComputerAddr);
	controller.OpenSCMgr();
	SC_HANDLE	hService;


	for (auto spDeliceModule : m_vecModules)
	{
		if (spDeliceModule->m_bCheck == false)
			continue;

		if (spDeliceModule->m_bIsService)
		{
			bool bStopped = false;
			bool bDeleted = false;
			bool bKilled  = false;
			// if registered try to stop and delete
			hService = controller.Open(spDeliceModule->m_strName);
			if (hService != NULL)
			{
				bStopped = controller.Stop(10000);	// 30 sec
				if (!bStopped)
				{
					CMNKillProcessByName(spDeliceModule->m_strModuleFileName);
					bKilled = true;
				}
				if (bStopped || bKilled)
				{
					bDeleted = controller.Delete(); 	// delete it
					nRemoved++;
				}				
				controller.Close();
				CMNLog(_T("[CRegServicesThread::RemoveServices]: stopping and deleting service = %s, stoped = %i, killed = %i, deleted = %i"), spDeliceModule->m_strName, bStopped, bKilled, bDeleted);
			}
			else
				continue;
		}
		else
		{
			// find app main windows and say WM_CLOSE, WM_QUIT, etc.  if not responding, the terminate process
			//WriteConnectionStringAndFlags(spDeliceModule);
			continue;
		}

		nCount++;
		::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_STAGE, nCount, NULL);
		::Sleep(1000); // Wait for a second to apply progres bar changes
	}	
	controller.CloseSCMgr();

	return nRemoved;
}




void CRegServicesThread::WriteConnectionStringAndFlags(CDeliceOcrModuleDesc::Ptr spDeliceModule)
{	
	spDeliceModule->WriteConnectionString(m_rConfigData.m_strSQLServerAddr);
	spDeliceModule->WriteDebugFlag(false);
}


/*
TCHAR Text[MAX_LOADSTRING]; // переменная - куда читаем
 . . . 
// считываем.. если ключ будет не найден - присвоится значение Default
GetPrivateProfileString(_T("General"), _T("caption"), _T("Default"), Text, MAX_LOADSTRING, _T("./config.ini"));

// записываем

lstrcpy(Text, "Some value")

WritePrivateProfileString(_T("General"), _T("caption"), Text, _T("./config.ini"));

[General]
caption = "Some value"
*/

// CRegServicesThread message handlers



