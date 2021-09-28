//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:  Configurator
//--------------------------------------------------------------------------
//  Description: Thread for background database creation 
//--------------------------------------------------------------------------

#include "stdafx.h"

#include "resource.h"

#include "cmn/include/Exception.h"
#include "cmn/include/Log.h"
#include "cmn/include/misc.h"
#include "cmn/include/NetPath.h"
#include "cmn/include/VersionInfo.h"

#include "db/include/Constants.h"
#include "db/include/Storage.h"

#include "rgs/include/ServiceController.h"

#include "scm/include/SchemaDef.h"

#include "wzr/include/Thread.h"

#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/ConfigApp.h"
#include "ocrcfg/include/ApplyScriptsThread.h"
#include "ocrcfg/include/misc.h"


#pragma warning(disable: 4800)


// CApplyScriptsThread

//IMPLEMENT_DYNCREATE(CApplyScriptsThread, CWinThread)

CApplyScriptsThread::CApplyScriptsThread(HWND hParentWnd, //const CDBDescription::Vector& rvecDatabases, 
	COcrConfigData& rConfigData, CConfigXMLData& rXmlConfigData, int* pnModificationFlags)
	: WZRThread(hParentWnd),	
//	m_vecDatabases(rvecDatabases),
	m_rConfigData(rConfigData), 
	m_rXmlConfigData(rXmlConfigData),
	m_pnModificationFlags(pnModificationFlags)
{
	if(!m_pnModificationFlags)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_POINTER_TO_INT_IS_NULL);
		*pEx << _T("CApplyScriptsThread::CApplyScriptsThread");
		ThrowException(pEx);
	}
}

CApplyScriptsThread::~CApplyScriptsThread()
{
}

BOOL CApplyScriptsThread::InitInstance()
{
	CoInitialize(NULL);
    CWinThread::InitInstance();

//	BOOL	bRun = true;
	BOOL	bError				= FALSE;
	int		nRes				= IDNO;
	BOOL	bExist				= FALSE;
	BOOL	bCreated			= FALSE;
	BOOL	bOnlyUseDatabase	= FALSE; // do not create, but use selected database(s)
    BOOL    bGranted			= FALSE;
	int		nScripts = 0;
	CString strGrantError;

	::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_START, NULL, NULL);
	try
	{	
		CAtlString strDomain;
		CAtlString strUsername;

		CMNSplitExtendedName(m_rConfigData.m_strLogin, strDomain, strUsername);
		HANDLE	hToken;
		BOOL	bResult;

		bResult = ::LogonUser(strUsername, strDomain, m_rConfigData.m_strPassword, LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &hToken);
		CMNCheckLastError(bResult);
		bResult = ::ImpersonateLoggedOnUser(hToken);
		CMNCheckLastError(bResult);

		/*
		if ( m_config.IsCreateServer() ) // create database on server
		{			
			CheckSQLServerCompatibility();
			bExist = IsDBExist();
			if (bExist)
			{
				// who is creator of data base? I - ok, some one else - throw exception
				CheckIsOurDB();

				if (theApp.m_bUnattendedMode)
					nRes = IDYES;
				else
				{
					CString strDatabases;
					for (auto pDatabase : m_vecDatabases)
					{
						strDatabases += pDatabase->m_strDatabase;
						strDatabases += _T(",");
					}
					if (strDatabases[strDatabases.GetLength()-1] == TCHAR(','))
						strDatabases.SetAt(strDatabases.GetLength()-1, TCHAR(' '));

					CString strQuestion; 
					strQuestion.FormatMessage(IDS_OCRCFG_DB_RECREATE_QUESTION, strDatabases, m_rConfigData.m_strSQLServerAddr);
					nRes = AfxMessageBox(strQuestion, MB_ICONQUESTION | MB_YESNO);
				}
			}

			if (bExist)
			{
				if (nRes == IDYES)
				{
					bCreated = CreateDB(TRUE);
					bGranted = GrantDBOwnerRole(strGrantError);
				}
				else
				{
					bOnlyUseDatabase = TRUE;
				}
			}
			else
			{
				bCreated = CreateDB(FALSE);
				bGranted = GrantDBOwnerRole(strGrantError);
			}
		}
		*/
		
		//if ( m_config.IsRunSqlScripts() )
		{
			nScripts = RunSqlScripts();
			SetupInitialValues();
		}
		
		/*
		if ( bCreated == TRUE )
		{
			for (auto pDatabase : m_vecDatabases)
			{
				CString str;
				str.FormatMessage(IDS_OCRCFG_DB_RESULTS, pDatabase->m_strDatabase, m_rConfigData.m_strSQLServerAddr);
				m_strThreadResults += str;
				m_strThreadResults += _T(" ");
			}
    	    
            if (bGranted)
            {
				for (auto pDatabase : m_vecDatabases)
				{
					CString str;
					str.FormatMessage(IDS_OCRCFG_DB_GRANTED, m_rConfigData.m_strLogin, pDatabase->m_strDatabase);
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
						str.FormatMessage(IDS_OCRCFG_DB_GRANT_ERROR, m_rConfigData.m_strLogin, pDatabase->m_strDatabase, strGrantError);
						m_strError += str;
						m_strError += _T(" ");
					}
				}
			}
            CMNLog(_T("[CApplyScriptsThread::InitInstance]: %s"), m_strThreadResults);
		}
		
		if ( bOnlyUseDatabase == TRUE )
		{
			for (auto pDatabase : m_vecDatabases)
			{
				CString str;
				str.FormatMessage(IDS_OCRCFG_DB_USE, pDatabase->m_strDatabase, m_rConfigData.m_strSQLServerAddr);
				m_strThreadResults += str;
				m_strThreadResults += _T(" ");
			}
			CMNLog(_T("[CApplyScriptsThread::InitInstance]: %s"), m_strThreadResults);
		}
		*/
		
		if ( nScripts > 0 )
		{
			CString str;
			str.FormatMessage(IDS_OCRCFG_DB_SCRIPTS_APPLIED, m_rXmlConfigData.GetDBName(), nScripts);
			if ( m_strThreadResults.GetLength() ) 
				m_strThreadResults += _T(" "); // add once space after previous word
			m_strThreadResults += str;
			CMNLog(_T("[CApplyScriptsThread::InitInstance]: %s"), m_strThreadResults);
		}
	}
	catch(CMNException* pEx)
	{	    
		//[Patch] Закрыть соединение если открыто
		//Деструктор m_storage не удаляет ConnectionPool, а повторный запуск потока,
		//и соответственно новый вызов m_storage.Connect вызовет пересоздание ConnectionPool,
		//которое завершится ошибкой.
		if(m_storage.ConnectionState()==1)
			m_storage.Disconnect();

		m_strError = CMNException::GetAllErrorMessages(pEx, _T("\n"));
        CMNLog(_T("[CApplyScriptsThread::InitInstance]: %s"), m_strError);
		bError = TRUE;
		pEx->Delete();
	}

	::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_FINISH, bError, NULL);	
	
	return TRUE;
}


int CApplyScriptsThread::ExitInstance()
{
	::RevertToSelf();
	CoUninitialize();
	return CWinThread::ExitInstance();
}


//BEGIN_MESSAGE_MAP(CApplyScriptsThread, CWinThread)
//END_MESSAGE_MAP()

// Implementation

// TEMPLATES


int CApplyScriptsThread::RunSqlScripts()
{
	int scriptCount = 0;
	
	const set<CString>& mapScriptFilePaths = m_rXmlConfigData.GetScriptFilePaths();
	if ( mapScriptFilePaths.empty() )
		return 0;

	::PostMessage(m_hParentWnd, WM_OCRCFG_CUSTOMSCRIPT_EXCECUTION_START, NULL, NULL);

	// commect to the SQL server using database from xml file
	m_storage.Connect(cszDefSAMConnectionString, m_rConfigData.m_strSQLServerAddr, m_rXmlConfigData.GetDBName());

	// run scripts
	for (set<CString>::const_iterator it = mapScriptFilePaths.begin(); it != mapScriptFilePaths.end(); ++it)
	{
		CString strFullSqlFileName = *it;
		if(strFullSqlFileName.IsEmpty())
			continue;
	
		/*
		strSqlScript.Replace(_T("\'"), _T("\'\'"));
		strCommand.Format( _T("EXECUTE(N\'%s\')"), strSqlScript );
		*/

		CString strScript = TLoadScriptFromFile(strFullSqlFileName);
		CString strCommand;
		strScript.Replace(_T("GO\n"), _T("  "));	// GO is a specific command supported only in SQL Query editor		
		strScript.Replace(_T("GO\r\n"), _T("  "));	// GO is a specific command supported only in SQL Query editor		
		strCommand = strScript;

		m_storage.ExecuteCommand(strCommand);
		
		int stage = scriptCount;
		/*
		if ( m_config.IsCreateServer() )
		{
			stage += DBStorage::m_vecScriptID.size();
		}
		*/
		::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_STAGE, stage, NULL);
		scriptCount++;
		// Wait for a second to apply another script
		::Sleep(1000);
	}
	
	m_storage.Disconnect();
	
	return scriptCount;
}


void CApplyScriptsThread::SetupInitialValues()
{
	// connect to SQL Server using daatabase from xml file
	m_storage.Connect(cszDefSAMConnectionString, m_rConfigData.m_strSQLServerAddr, m_rXmlConfigData.GetDBName());
	
	bstr_t bstrResult;
	//try
	{
		CAtlString strDomain, strName;
		CMNSplitExtendedName(m_rConfigData.m_strLogin, strDomain, strName);
		m_storage.AddUser	(strName, m_rConfigData.m_strLogin, bstrResult);
	}
	/*
	catch (TException* pEx)
	{
		pEx->ReportError();
		pEx->Delete();
	}
	*/

	m_storage.Disconnect();	
}


/*
bool CApplyScriptsThread::IsDBExist()
{
	bool bExist = false;
	m_storage.Connect(cszDefSAMConnectionString, m_rConfigData.m_strSQLServerAddr, _T("master"));
		
	for (auto pDatabase : m_vecDatabases)
	{
		bExist = bExist || m_storage.IsDatabaseExist(pDatabase->m_strDatabase);		
	}
	m_storage.Disconnect();
	return bExist;
}

BOOL CApplyScriptsThread::CreateDB(BOOL bDeleteFirst)
{
	m_storage.Connect(cszDefSAMConnectionString, m_rConfigData.m_strSQLServerAddr, _T("master"));

	if ( bDeleteFirst )
	{
		for (auto pDatabase : m_vecDatabases)
			m_storage.DeleteDatabase(pDatabase->m_strDatabase);
	}
	int nDatabaseNumber = 0;
	for (auto pDatabase : m_vecDatabases)
	{
		// New code. restore database from file		
		// RESTORE DATABASE[DELICE_TECH_PROCESS] FROM  DISK = 'C:\Install.OCR\delice_tech_process_etalon_20200907.bak' WITH  FILE = 1, 
		// MOVE 'DELICE_TECH_PROCESS_Data' TO 'C:\MSSQL\db\DELICE_TECH_PROCESS.mdf', 
		// MOVE 'DELICE_TECH_PROCESS_log' TO 'C:\MSSQL\logs\DELICE_TECH_PROCESS.ldf'

		CString strMoveDataTo	= m_storage.GetDefDataPath()	+ pDatabase->GetDatabaseDataFile();
		CString strMoveLogTo	= m_storage.GetDefLogPath()		+ pDatabase->GetDatabaseLogFile();

		CString strDatabaseFileName = SCMGetDefOCR_DbDataDirValue();
		strDatabaseFileName += pDatabase->m_strDatabaseBackupFileName;

		//SCMGetDefOCR_DbDataDirValue

		CString strSqlCommand;
		strSqlCommand.Format(_T("restore database [%s] from disk = '%s' with file=1, move '%s' to '%s', move '%s' to '%s'"),
			pDatabase->m_strDatabase,
			strDatabaseFileName,
			pDatabase->GetDatabaseData(),	strMoveDataTo,
			pDatabase->GetDatabaseLog(),	strMoveLogTo);

		m_storage.ExecuteCommand(strSqlCommand);
		nDatabaseNumber++;
		::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_STAGE, (WPARAM)nDatabaseNumber, (LPARAM)m_vecDatabases.size());
		::Sleep(2000);
		
	}
	m_storage.Disconnect();

	*m_pnModificationFlags |= IsCreateDB; 

	return TRUE;
}




CString CApplyScriptsThread::GetServiceLogin(CString strServiceName)
{
	LPQUERY_SERVICE_CONFIG	lpServiceConfig;
	RGSServiceController    controller((bstr_t)L"", (bstr_t)strServiceName);
	CString					strServiceLogin;

	try
	{
		controller.OpenSCMgr();
		controller.Open();
		if (controller.QueryServiceConfig(&lpServiceConfig) == TRUE)
		{
			if (lpServiceConfig->lpServiceStartName != NULL)
			{
				strServiceLogin = lpServiceConfig->lpServiceStartName;
			}			
			controller.FreeConfigData(lpServiceConfig);
		}
		controller.Close();
		controller.CloseSCMgr();
	}
	catch (CMNException* pEx)
	{
		CString strError = CMNException::GetAllErrorMessages(pEx, _T("\n"));
		CMNLog(_T("[CApplyScriptsThread::GetServiceLogin]: %s"), strError);
		pEx->Delete();
	}
	return strServiceLogin;
}


CString CApplyScriptsThread::GetCurrentUserName()
{
	LPTSTR	szCurrentUserLogin = NULL;
	ULONG	nSize = 0;
	CString strCurrentUserNname;

	if (::GetUserNameEx(NameSamCompatible, szCurrentUserLogin, &nSize) == FALSE)
	{
		DWORD	dwErr = ::GetLastError();
		if (dwErr == ERROR_MORE_DATA)
		{
			szCurrentUserLogin = new TCHAR[nSize + 1];
			memset(szCurrentUserLogin, 0, (nSize + 1) * sizeof(TCHAR));
			if (::GetUserNameEx(NameSamCompatible, szCurrentUserLogin, &nSize) == TRUE)
			{
				strCurrentUserNname = szCurrentUserLogin;
			}
			delete[] szCurrentUserLogin;
		}
	}
	return strCurrentUserNname;
}


BOOL CApplyScriptsThread::GrantDBOwnerRole(CString& strError)
{		
	ULONG					nLen = 0;
	BOOL					bAddRole = TRUE;
    BOOL                    bGranted = FALSE;

	
	for (auto pDatabase : m_vecDatabases)
	{
		try
		{
			m_storage.Connect(cszDefSAMConnectionString, m_rConfigData.m_strSQLServerAddr, pDatabase->m_strDatabase);
			if (bAddRole == TRUE)
			{
				// grant dbowner role for the service login account	entered at the first page	
				m_storage.CreateDBOwner(pDatabase->m_strDatabase, m_rConfigData.m_strLogin);				
				bGranted = TRUE;
			}
			m_storage.Disconnect();
		}
		catch (CMNException* pEx)
		{
			m_storage.Disconnect();
			strError = CMNException::GetAllErrorMessages(pEx, _T("\n"));
			CMNLog(_T("[CApplyScriptsThread::GrantDBOwnerRole]: %s"), strError);
			pEx->Delete();
			bGranted = FALSE;
		}
	}
	return bGranted;

}


void CApplyScriptsThread::CheckIsOurDB()
{
	//check condition
	for (auto pDatabase : m_vecDatabases)
	{
		m_storage.Connect(cszDefSAMConnectionString, m_rConfigData.m_strSQLServerAddr, pDatabase->m_strDatabase);

		set<CAtlString> setResult;
		//Get Tables list
		setResult = m_storage.ListTables();
		//for (set<CAtlString>::iterator it = m_vecDatabases[0]->m_setRecognizableTables.begin(); it != m_vecDatabases[0]->m_setRecognizableTables.end(); ++it)
		// C++ 11 like cycle
		for (auto value : pDatabase->m_setRecognizableTables)
		{
			if (setResult.end() == find(begin(setResult), end(setResult), value))
			{
				CMNException* pEx = new CMNException(IDE_OCRCFG_SERVER_DB_CREATOR_IS_UNKNOWN);
				*pEx << pDatabase->m_strDatabase;
				ThrowException(pEx);
				// Disconnect  will bw implement in catch block
			}
		}

		setResult.clear();
		//Get store procedure list
		setResult = m_storage.ListStoredProc();

		//Create result
		//for (set<CAtlString>::iterator it = m_vecDatabases[0]->m_setRecognizableProcedures.begin(); it != m_vecDatabases[0]->m_setRecognizableProcedures.end(); ++it)
		// C++ 11 like cycle
		for (auto value : pDatabase->m_setRecognizableProcedures)
		{
			if (setResult.end() == find(begin(setResult), end(setResult), value))
			{
				CMNException* pEx = new CMNException(IDE_OCRCFG_SERVER_DB_CREATOR_IS_UNKNOWN);
				*pEx << pDatabase->m_strDatabase;
				ThrowException(pEx);
				// Disconnect  will bw implement in catch block
			}
		}
		m_storage.Disconnect();
	}	
}



void CApplyScriptsThread::CheckSQLServerCompatibility()
{
	//check condition
	m_storage.Connect(cszDefSAMConnectionString, m_rConfigData.m_strSQLServerAddr, _T("master"));

	m_storage.LoadServerOptions();
	m_storage.LoadServerCapabilities();

	bstr_t bstrVersionDetails;
	//	SQL Server Version
	int nVersion = m_storage.GetServerVersion();
	if (nVersion < 14)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_SERVER_VERSION_TOO_OLD);
		(*pEx) << m_rConfigData.m_strSQLServerAddr;
		ThrowException(pEx);		
	}

	CString str	 = m_storage.GetCollation();

	if (str.CompareNoCase(_T("Cyrillic_General_CI_AS")) != 0)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_SERVER_COLLATION_NOT_CYRILLIC);
		(*pEx) << m_rConfigData.m_strSQLServerAddr;
		ThrowException(pEx);
	}

	//	FullTextSearch;
	//	Database Mail.
	if (m_storage.GetFullTextStatus() != 1)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_SERVER_FULLTEXTSEARCH_NOT_INSTALLED);
		(*pEx) << m_rConfigData.m_strSQLServerAddr;
		ThrowException(pEx);
	}
	// temporary disable	
	//if (m_storage.GetDatabaseMailStatus() != 1)
	if (0)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_SERVER_DATABASEMAIL_NOT_INSTALLED);
		(*pEx) << m_strSqlServerHostName;
		ThrowException(pEx);		
	}
	
//	Integration Services;

//  Be CAREFUL! ComputerAddr name may differ from SQLServerAddr when SQL instance name used 
	auto tIntegrationServicesCaps = CheckIntegrationServices();

	if (get<0>(tIntegrationServicesCaps) != 1)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_SERVER_INT_SERVICES_NOT_INSTALLED);
		(*pEx) << m_rConfigData.m_strComputerAddr;
		ThrowException(pEx);		
	}
	if (get<1>(tIntegrationServicesCaps) != 1)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_SERVER_INT_SERVICES_NOT_RUNNING);
		(*pEx) << m_rConfigData.m_strComputerAddr;
		ThrowException(pEx);
	}
		
	m_storage.Disconnect();
}


tuple<int, int> CApplyScriptsThread::CheckIntegrationServices()
{
	int nIntegrationServicesInstalled = -1; // not determined
	int nIntegrationServicesRunning   = -1; // not determined

	//MsDtsServer140
	//SSISTELEMETRY140
	//SSISScaleOutMaster140
	//SSISScaleOutWorker140
	bstr_t	bstrIntegrationService = L"MsDtsServer140";



	RGSServiceController   controller((bstr_t)m_rConfigData.m_strComputerAddr, (bstr_t)bstrIntegrationService);
	bool bRes = controller.OpenSCMgr();
	if(bRes)
	{
		SC_HANDLE hService = controller.Open();
		if (hService)
		{
			nIntegrationServicesInstalled = 1;
			SERVICE_STATUS	status;
			bRes = controller.QueryStatus(&status);
			if (bRes)
			{
				if (status.dwCurrentState & SERVICE_RUNNING)
				{
					nIntegrationServicesRunning = 1;
				}
				else
					nIntegrationServicesRunning = 0;
			}		
		}
		else
			nIntegrationServicesInstalled = 0;
		controller.Close();
	}	
	controller.CloseSCMgr();
	return make_tuple(nIntegrationServicesInstalled, nIntegrationServicesRunning);
}
*/
// CApplyScriptsThread message handlers
