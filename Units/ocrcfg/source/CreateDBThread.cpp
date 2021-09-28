//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:  Configurator
//--------------------------------------------------------------------------
//  Description: Thread for background database creation 
//--------------------------------------------------------------------------

#include "stdafx.h"

#include "resource.h"

#include "cmn/include/misc.h"
#include "cmn/include/Log.h"
#include "cmn/include/Exception.h"
#include "cmn/include/NetPath.h"
#include "cmn/include/VersionInfo.h"

#include "db/include/Constants.h"
#include "db/include/Storage.h"

#include "rgs/include/ServiceController.h"

#include "scm/include/SchemaDef.h"

#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/ConfigApp.h"
#include "wzr/include/Thread.h"
#include "ocrcfg/include/CreateDBThread.h"
#include "ocrcfg/include/misc.h"


#pragma warning(disable: 4800)


// CCreateDBThread

//IMPLEMENT_DYNCREATE(CCreateDBThread, CWinThread)

CCreateDBThread::CCreateDBThread(HWND hParentWnd, LPCTSTR pszSqlServerUserEnterName, LPCTSTR pszSqlServerHostName,
	const CDBDescription::Vector& rvecDatabases, COcrConfigData& rConfigData, CConfigXMLData& config, int* pnModificationFlags)
	: WZRThread(hParentWnd),	
	m_vecDatabases(rvecDatabases),
	m_rConfigData(rConfigData), 
	m_config(config),
	m_pnModificationFlags(pnModificationFlags)
{
	if(!m_pnModificationFlags)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_POINTER_TO_INT_IS_NULL);
		*pEx << _T("CCreateDBThread::CCreateDBThread");
		ThrowException(pEx);
	}
}

CCreateDBThread::~CCreateDBThread()
{
}

BOOL CCreateDBThread::InitInstance()
{
	CoInitialize(NULL);
    WZRThread::InitInstance();

//	BOOL	bRun = true;
	BOOL	bError				= FALSE;
	int		nRes				= IDNO;
	BOOL	bExist				= FALSE;
	BOOL	bCreated			= FALSE;
	BOOL	bRemoved            = FALSE;
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


		if (theApp.m_enScenario == CUseScenarioEnum::eusCreateNew ) // create database on server
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

		if (theApp.m_enScenario == CUseScenarioEnum::eusRemove) // register services
		{
			bRemoved = RemoveDB();
		}

		/*
		if ( m_config.IsRunSqlScripts() )
		{
			nScripts = RunSqlScripts();
		}
		*/
		if (bRemoved == TRUE)
		{
			for (auto pDatabase : m_vecDatabases)
			{
				CString str;
				str.FormatMessage(IDS_OCRCFG_DB_REMOVED, pDatabase->m_strDatabase, m_rConfigData.m_strSQLServerAddr);
				m_strThreadResults += str;
				m_strThreadResults += _T(" ");
			}
		}
		if ( bCreated == TRUE )
		{
			for (auto pDatabase : m_vecDatabases)
			{
				CString str;
				str.FormatMessage(IDS_OCRCFG_DB_CREATED, pDatabase->m_strDatabase, m_rConfigData.m_strSQLServerAddr);
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
            CMNLog(_T("[CCreateDBThread::InitInstance]: %s"), m_strThreadResults);
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
			CMNLog(_T("[CCreateDBThread::InitInstance]: %s"), m_strThreadResults);
		}
		/*
		if ( nScripts > 0 )
		{
			CString str;
			str.FormatMessage(IDS_OCRCFG_DB_SCRIPTS_APPLIED, m_vecDatabases[0]->m_strDatabase, nScripts);
			if ( m_strThreadResults.GetLength() ) m_strThreadResults += _T(" ");
			m_strThreadResults += str;
			CMNLog(_T("[CCreateDBThread::InitInstance]: %s"), m_strThreadResults);
		}
		*/
	}
	catch(CMNException* pEx)
	{	    
		//[Patch] «акрыть соединение если открыто
		//ƒеструктор m_storage не удал€ет ConnectionPool, а повторный запуск потока,
		//и соответственно новый вызов m_storage.Connect вызовет пересоздание ConnectionPool,
		//которое завершитс€ ошибкой.
		if(m_storage.ConnectionState()==1)
			m_storage.Disconnect();

		m_strError = CMNException::GetAllErrorMessages(pEx, _T("\n"));
        CMNLog(_T("[CCreateDBThread::InitInstance]: %s"), m_strError);
		bError = TRUE;
		pEx->Delete();
	}

	::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_FINISH, bError, NULL);	
	
	return TRUE;
}


int CCreateDBThread::ExitInstance()
{
	::RevertToSelf();
	CoUninitialize();
	return WZRThread::ExitInstance();
}


//BEGIN_MESSAGE_MAP(CCreateDBThread, CWinThread)
//END_MESSAGE_MAP()

// Implementation

// TEMPLATES



bool CCreateDBThread::IsDBExist()
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

BOOL CCreateDBThread::CreateDB(BOOL bDeleteFirst)
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
		
		/* Old code - create database from scripts
		m_storage.CreateDatabase(m_vecDatabases[0]->m_strDatabase );

		DBScriptIDVector::iterator it;

		for (it = m_storage.m_vecScriptID.begin(); it != m_storage.m_vecScriptID.end(); it++)
		{
			CString str;
			str = LoadScriptFromResources(*it);
			m_storage.ExecuteCommand( str );
			int i = it - m_storage.m_vecScriptID.begin();
			::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_STAGE, (WPARAM)i, (LPARAM)m_storage.m_vecScriptID.size());
		}
		// Wait for a second to apply stored procedures
		::Sleep(1000);

		// Adding initial data (too big for ExecuteCommand)
		m_storage.ExecuteProcedure( _T("dbo.InsertInitialData") );

		m_storage.ExecuteProcedure( _T("dbo.AddInitialObjects") );

		m_storage.ExecuteProcedure( _T("dbo.InsertChipsetLibrary") );

		m_storage.InsertPicture(IDR_DB_DEMO_PICTURE);

		m_storage.ExecuteProcedure( _T("dbo.CreateObjectRelationViews") );

		m_storage.ExecuteProcedure( _T("dbo.CreateViews") );

		// set Database version
		HMODULE hModule = _AtlBaseModule.GetModuleInstance();
		CMNVersionInfo verInfo;
		verInfo.LoadVersionInfo( hModule );
		int nVersion = verInfo.GetMinorHi();
		m_storage.SetDatabaseVersion(nVersion);
		m_storage.GrantReadPermissionsForGuest();
		*/
	}
	m_storage.Disconnect();

	*m_pnModificationFlags |= IsCreateDB; 

	return TRUE;
}


BOOL CCreateDBThread::RemoveDB()
{
	BOOL bRemoved = FALSE;

	m_storage.Connect(cszDefSAMConnectionString, m_rConfigData.m_strSQLServerAddr, _T("master"));
		
	int nDatabaseNumber = 0;
	for (auto pDatabase : m_vecDatabases)
	{
		if (m_storage.IsDatabaseExist(pDatabase->m_strDatabase))
		{
			m_storage.DeleteDatabase(pDatabase->m_strDatabase);
			nDatabaseNumber++;
			::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_STAGE, (WPARAM)nDatabaseNumber, (LPARAM)m_vecDatabases.size());
			Sleep(500);
			bRemoved = TRUE;
		}		
	}
	m_storage.Disconnect();

	return bRemoved;
}


int CCreateDBThread::RunSqlScripts()
{
	int scriptCount = 0;
	/*
	const set<CString>& mapScriptFilePaths = m_config.GetScriptFilePaths();
	if ( mapScriptFilePaths.empty() )
		return 0;

	::PostMessage(m_hParentWnd, WM_OCRCFG_CUSTOMSCRIPT_EXCECUTION_START, NULL, NULL);

	m_storage.Connect(cszDefSAMConnectionString, m_strSqlServerHostName, m_strDatabase);	

	// run scripts	
	for (set<CString>::const_iterator it = mapScriptFilePaths.begin(); it != mapScriptFilePaths.end(); ++it)
	{
		CString strFullSqlFileName = *it;
		if(strFullSqlFileName.IsEmpty())
			continue ;

		CFile fileScript;
		CFileException ex;
		if(!fileScript.Open(strFullSqlFileName, CFile::modeRead | CFile::shareDenyWrite, &ex))
		{
			CString strErrorFormat, strErrorMsg;
			strErrorFormat.LoadString(IDE_OPENING_SQLSCRIPTFILE_ERROR);
			strErrorMsg.Format(strErrorFormat, strFullSqlFileName);
			AfxMessageBox(strErrorMsg, MB_OK | MB_ICONINFORMATION);
			break ;
		}

		UINT uiFileSize = (UINT) fileScript.GetLength();
		BYTE* pbBuf = new BYTE[uiFileSize + 2];
		memset(pbBuf, 0, uiFileSize + 2);
		UINT uiReaded = fileScript.Read(pbBuf, uiFileSize);
		fileScript.Close();
		if(uiReaded != uiFileSize)
		{
			CString strErrorFormat, strErrorMsg;
			strErrorFormat.LoadString(IDE_OPENING_SQLSCRIPTFILE_ERROR);

			strErrorMsg.Format(strErrorFormat, strFullSqlFileName);
			AfxMessageBox(strErrorMsg, MB_OK | MB_ICONINFORMATION);

			delete [] pbBuf;

			break ;
		}

		DWORD dwOffset = 0;
		if(pbBuf[0] == 0xff && pbBuf[1] == 0xfe) // skip an Unicode file prefix
			dwOffset = 2;

		CString strSqlScript = (LPCTSTR) (pbBuf + dwOffset);
		
		delete [] pbBuf;

		CString strCommand;
		strSqlScript.Replace(_T("\'"), _T("\'\'"));
		strCommand.Format( _T("EXECUTE(N\'%s\')"), strSqlScript );

		m_storage.ExecuteCommand(strCommand);

		int stage = scriptCount;
		if ( m_config.IsCreateServer() )
		{
			stage += DBStorage::m_vecScriptID.size();
		}
		::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_STAGE, stage, NULL);
		scriptCount++;
	}
	// Wait for a second to apply stored procedures
	::Sleep(1000);

	m_storage.Disconnect();
	*/
	return scriptCount;
}



CString CCreateDBThread::GetServiceLogin(CString strServiceName)
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
		CMNLog(_T("[CCreateDBThread::GetServiceLogin]: %s"), strError);
		pEx->Delete();
	}
	return strServiceLogin;
}


CString CCreateDBThread::GetCurrentUserName()
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


BOOL CCreateDBThread::GrantDBOwnerRole(CString& strError)
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
			CMNLog(_T("[CCreateDBThread::GrantDBOwnerRole]: %s"), strError);
			pEx->Delete();
			bGranted = FALSE;
		}
	}
	return bGranted;

	/*
	RGSServiceController   controller((bstr_t)L"", (bstr_t)cszSAMSerivceName); 
    try
    {
		controller.OpenSCMgr();
        controller.Open();
		if ( controller.QueryServiceConfig(&lpServiceConfig) == TRUE )
		{
			if ( lpServiceConfig->lpServiceStartName != NULL )
			{
				m_strServiceLogin = lpServiceConfig->lpServiceStartName;
			}
			else
			{
				bAddRole = FALSE;
			}
			controller.FreeConfigData( lpServiceConfig );
		}
		else
		{
			bAddRole = FALSE;
		}
		controller.Close();
        controller.CloseSCMgr();                    			

		if (bAddRole == FALSE) // no information about IM service
			return bGranted;

		if (CAtlModule::WordCmpI(m_strServiceLogin, _T("LocalSystem")) == 0)
		{
			return bGranted;
		}

		CString strSamServiceLogin;

		int nSlash = m_strServiceLogin.ReverseFind('\\');
		if ( nSlash != -1 )
		{
			CString strDomain = m_strServiceLogin.Left(nSlash);
			CString strUser = m_strServiceLogin.Mid(nSlash + 1);
			if ( ! strDomain.IsEmpty() )
			{
				if ( strDomain == _T(".") )
				{
					TCHAR szComputerName[MAX_PATH] = {0};
					DWORD dwBuffSize = MAX_PATH;
					GetComputerName( szComputerName, &dwBuffSize );
					strSamServiceLogin.Format(_T("%s\\%s"), szComputerName, (LPCTSTR)strUser);
				}
				else
				{
					// им€ может быть в формате DNS
					TCHAR szCompName[MAX_COMPUTERNAME_LENGTH + 1];
					DWORD dwCompSize = MAX_COMPUTERNAME_LENGTH + 1;
					BOOL bConvert = DnsHostnameToComputerName(strDomain, szCompName, &dwCompSize);
					if ( bConvert )
					{
						strSamServiceLogin.Format(_T("%s\\%s"), szCompName, (LPCTSTR)strUser);
					}
				}
			}
		}

		if ( strSamServiceLogin.IsEmpty() )
			strSamServiceLogin = m_strServiceLogin;	

		// get current account
		// check for the data space required
		if ( ::GetUserNameEx(NameSamCompatible, szCurrentUserLogin, &nSize) == FALSE )
		{
			DWORD	dwErr = ::GetLastError();
			if ( dwErr == ERROR_MORE_DATA )
			{
				szCurrentUserLogin = new TCHAR[nSize + 1];
				memset( szCurrentUserLogin, 0, (nSize + 1)* sizeof(TCHAR)); 
				if ( ::GetUserNameEx(NameSamCompatible, szCurrentUserLogin, &nSize) == TRUE )
				{
					if( CAtlModule::WordCmpI(strSamServiceLogin, szCurrentUserLogin)== 0)
					{
						bAddRole = FALSE;
					}
				}
				delete [] szCurrentUserLogin;
			}
		}
	*/    
}


void CCreateDBThread::CheckIsOurDB()
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
				*pEx << value;
				ThrowException(pEx);
				// Disconnect  will bw implement in catch block
			}
		}
		m_storage.Disconnect();
	}	
}



void CCreateDBThread::CheckSQLServerCompatibility()
{
	//check condition
	m_storage.Connect(cszDefSAMConnectionString, m_rConfigData.m_strSQLServerAddr, _T("master"));

	m_storage.LoadServerOptions();
	m_storage.LoadServerCapabilities();

	bstr_t bstrVersionDetails;
	//	SQL Server Version
	int nVersion		= m_storage.GetServerVersion();
	int nProductVersion = m_storage.GetMajorVersion();
	if (nVersion < 14)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_SERVER_VERSION_TOO_OLD);
		(*pEx) << m_rConfigData.m_strSQLServerAddr;
		ThrowException(pEx);		
	}

	CString str		= m_storage.GetCollation();

	if (str.CompareNoCase(_T("Cyrillic_General_CI_AS")) != 0)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_SERVER_COLLATION_NOT_CYRILLIC);
		(*pEx) << m_rConfigData.m_strSQLServerAddr;
		ThrowException(pEx);
	}

	int	nEngineEdition = m_storage.GetEngineEdition();

	if (nEngineEdition != 2 && nEngineEdition != 3)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_SERVER_INEGRATED_SEC_ONLY);
		(*pEx) << m_rConfigData.m_strSQLServerAddr;
		ThrowException(pEx);
	}


	int	nIntSecOnly	= m_storage.GetIntegratedSecOnly();

	if (nIntSecOnly == 1)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_SERVER_INEGRATED_SEC_ONLY);
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
	/* temporary disable	
	if (m_storage.GetDatabaseMailStatus() != 1)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_SERVER_DATABASEMAIL_NOT_INSTALLED);
		(*pEx) << m_strSqlServerHostName;
		ThrowException(pEx);		
	}
	*/
//	Integration Services;

//  Be CAREFUL! ComputerAddr name may differ from SQLServerAddr when SQL instance name used 
	/*  Disabled due to OCRCONFIG-17
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
	*/ 
	
	auto tSQLAgentCaps = StartSQLAgent();

	m_storage.Disconnect();
}


tuple<int, int> CCreateDBThread::CheckIntegrationServices()
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
			controller.Start();
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


tuple<int, int> CCreateDBThread::StartSQLAgent()
{
	int nSQLAgentInstalled	= -1; // not determined
	int nSQLAgentRunning	= -1; // not determined

	//MsDtsServer140
	//SSISTELEMETRY140
	//SSISScaleOutMaster140
	//SSISScaleOutWorker140
	bstr_t	bstrSQLAgent = L"SQLSERVERAGENT";
	   
	RGSServiceController   controller((bstr_t)m_rConfigData.m_strComputerAddr, (bstr_t)bstrSQLAgent);
	bool bRes = controller.OpenSCMgr();
	if (bRes)
	{
		SC_HANDLE hService = controller.Open();
		if (hService)
		{			
			nSQLAgentInstalled = 1;
			controller.Start();
			SERVICE_STATUS	status;
			bRes = controller.QueryStatus(&status);
			if (bRes)
			{
				if (status.dwCurrentState & SERVICE_RUNNING)
				{
					nSQLAgentRunning = 1;
				}
				else
				{
					nSQLAgentRunning = 0;					
				}
			}
		}
		else
			nSQLAgentInstalled = 0;
		controller.Close();
	}
	controller.CloseSCMgr();

	CMNLog(_T("[CCreateDBThread::StartSQLAgent]: installed = %i, running = %i"), nSQLAgentInstalled, nSQLAgentRunning);

	return make_tuple(nSQLAgentInstalled, nSQLAgentRunning);
}


// CCreateDBThread message handlers
