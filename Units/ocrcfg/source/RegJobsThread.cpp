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
#include "cmn/include/NetPath.h"
#include "cmn/include/VersionInfo.h"

#include "t/include/misc.h"

#include "db/include/Constants.h"
#include "db/include/Storage.h"

#include "rgs/include/ServiceController.h"

#include "scm/include/SchemaDef.h"

#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/ConfigApp.h"
#include "wzr/include/Thread.h"
#include "ocrcfg/include/RegJobsThread.h"
#include "ocrcfg/include/misc.h"


#pragma warning(disable: 4800)


// CRegJobsThread

//IMPLEMENT_DYNCREATE(CRegJobsThread, CWinThread)

CRegJobsThread::CRegJobsThread( HWND hParentWnd, const CDeliceOcrJobDesc::Vector rvecJobs, COcrConfigData& rConfigData, CConfigXMLData& config, int* pnModificationFlags )
: WZRThread(hParentWnd),
	m_rConfigData (rConfigData),
	//m_config(config),
	m_vecJobs(rvecJobs),
	m_pnModificationFlags(pnModificationFlags)
{	
	if(!m_pnModificationFlags)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_POINTER_TO_INT_IS_NULL);
		*pEx << _T("CRegJobsThread::CRegJobsThread");
		ThrowException(pEx);
	}
}

CRegJobsThread::~CRegJobsThread()
{
}

BOOL CRegJobsThread::InitInstance()
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
	int		nJobs = 0;
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


		if (theApp.m_enScenario == CUseScenarioEnum::eusCreateNew) // create jobs
		{
			//CheckSQLServerCompatibility();
			bExist = AreAnyJobExist();
			if (bExist)
			{
				// who is creator of data base? I - ok, some one else - throw exception
				CString strJobs = CheckOurJobs();				

				if (theApp.m_bUnattendedMode)
					nRes = IDYES;
				else
				{
					CString strQuestion; 
					strQuestion.FormatMessage(IDS_OCRCFG_JOBS_RECREATE_QUESTION, strJobs, m_rConfigData.m_strSQLServerAddr);
					nRes = AfxMessageBox(strQuestion, MB_ICONQUESTION | MB_YESNO);
				}
			}

			if (bExist)
			{
				if (nRes == IDYES)
				{
					nJobs = CreateJobs(); // TRUE
					//bGranted = GrantDBOwnerRole(strGrantError);
				}
				else
				{
					//bOnlyUseDatabase = TRUE;
				}
			}
			else
			{
				nJobs = CreateJobs(); // FALSE
				//bGranted = GrantDBOwnerRole(strGrantError);
			}
		}

		if (theApp.m_enScenario == CUseScenarioEnum::eusRemove) // register services
		{
			nJobs = RemoveJobs();
		}
		
		if ( nJobs > 0 )
		{
			CString str;
			switch (theApp.m_enScenario)
			{
			case CUseScenarioEnum::eusCreateNew:
				str.Format(IDS_OCRCFG_JOBS_CREATED, (LPCTSTR)m_rConfigData.m_strSQLServerAddr, nJobs);
				break;
			case CUseScenarioEnum::eusRemove:
				str.Format(IDS_OCRCFG_JOBS_REMOVED, (LPCTSTR)m_rConfigData.m_strSQLServerAddr, nJobs);
				break;
			}
			if ( m_strThreadResults.GetLength() ) 
				m_strThreadResults += _T(" ");
			m_strThreadResults += str;
			CMNLog(_T("[CRegJobsThread::InitInstance]: %s"), m_strThreadResults);
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
        CMNLog(_T("[CRegJobsThread::InitInstance]: %s"), m_strError);
		bError = TRUE;
		pEx->Delete();
	}

	::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_FINISH, bError, NULL);	
	
	return TRUE;
}


int CRegJobsThread::ExitInstance()
{
	::RevertToSelf();
	CoUninitialize();
	return CWinThread::ExitInstance();
}


//BEGIN_MESSAGE_MAP(CRegJobsThread, CWinThread)
//END_MESSAGE_MAP()

// Implementation

// TEMPLATES



bool CRegJobsThread::AreAnyJobExist()
{
	bool bExist = false;

	//check condition
	m_storage.Connect(cszDefSAMConnectionString, m_rConfigData.m_strSQLServerAddr, _T("msdb"));

	set<CAtlString> setResult;
	//Get Tables list
	setResult = m_storage.ListJobs();

	// C++ 11 like cycle
	for (auto spJobDesc : m_vecJobs)
	{
		if (spJobDesc->m_bCheck == false) // not checked at the interface
			continue;
		
		CAtlString str = spJobDesc->m_strName;
		if (setResult.end() == find(begin(setResult), end(setResult), str))
		{
			/*
			CMNException* pEx = new CMNException(IDE_OCRCFG_SERVER_JOBS_ABSENT);
			*pEx << spJobDesc->m_strName;
			ThrowException(pEx);
			// Disconnect  will bw implement in catch block
			*/
		}
		else
		{
			bExist = true;
		}
	}

	setResult.clear();
	m_storage.Disconnect();

	return bExist;
}


/*
CString  LoadScriptFromFile(LPCTSTR pszSqlFileName)
{
	CString strScript;

	CFile fileScript;
	CFileException ex;
	if (!fileScript.Open(pszSqlFileName, CFile::modeRead | CFile::shareDenyWrite, &ex))
	{
		CString strErrorMsg;		
		strErrorMsg.Format(IDE_OPENING_SQLSCRIPTFILE_ERROR, pszSqlFileName);
		AfxMessageBox(strErrorMsg, MB_OK | MB_ICONINFORMATION);		
		return strScript;
	}

	UINT	uiFileSize = (UINT)fileScript.GetLength();
	auto_ptr<BYTE> spBuf ( new BYTE[uiFileSize + 2] );

	//memset(pbBuf, 0, uiFileSize + 2);
	UINT	uiRead = fileScript.Read(spBuf.get(), uiFileSize);
	fileScript.Close();
	if (uiRead != uiFileSize)
	{
		CString strErrorMsg;	
		strErrorMsg.Format(IDE_OPENING_SQLSCRIPTFILE_ERROR, pszSqlFileName);
		AfxMessageBox(strErrorMsg, MB_OK | MB_ICONINFORMATION);		
		return strScript;
	}
		
	if ( (*spBuf == 0xFF) && ((*spBuf+1) == 0xFE) ) // Unicode
	{
		CMNLog(_T("[LoadScriptFromFile]: %s, %i, '%s'"), pszSqlFileName, uiFileSize, _T("unicode"));
		LPCWSTR psz = (LPCWSTR)spBuf.get();
		strScript = CString(psz + 1, uiFileSize / sizeof(WCHAR) - 1); // pass first OxFF 0xFE
	}
	else // ANSI
	{
		CMNLog(_T("[LoadScriptFromFile]: %s, %i, '%s'"), pszSqlFileName, uiFileSize, _T("ANSI"));
		LPCSTR psz = (LPCSTR)spBuf.get();
		strScript = CString(psz, uiFileSize);
	}	

	return strScript;
}
*/

int CRegJobsThread::CreateJobs()
{
	::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_START, NULL, NULL);

	m_storage.Connect(cszDefSAMConnectionString, m_rConfigData.m_strSQLServerAddr, _T("master"));

	// run scripts
	int scriptCount = 0;	
	for (auto spJobDesc : m_vecJobs)
	{
		if (spJobDesc->m_bCheck == false) // not checked at the interface
			continue;

		CString strFullSqlFileName = SCMGetDefOCR_DbSqlDirValue();
		strFullSqlFileName += spJobDesc->m_strName;
		strFullSqlFileName += _T(".sql");
		
		if(strFullSqlFileName.IsEmpty())
			continue ;


		CString strScript  = TLoadScriptFromFile(strFullSqlFileName);
		CString strCommand;
		strScript.Replace(_T("GO\n"),   _T("  ")); // GO is a specific command supported only in SQL Query editor		
		strScript.Replace(_T("GO\r\n"), _T("  ")); // GO is a specific command supported only in SQL Query editor		
		strCommand = strScript;

		m_storage.ExecuteCommand(strCommand);
		CMNLog(_T("[CRegJobsThread::CreateJobs]: Job '%s' created"), spJobDesc->m_strDisplayName);

		scriptCount++;
		::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_STAGE, scriptCount, NULL);
		
	}
	// Wait for a second to apply stored procedures
	::Sleep(1000);

	m_storage.Disconnect();
	return scriptCount;
}



int CRegJobsThread::RemoveJobs()
{
	::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_START, NULL, NULL);

	m_storage.Connect(cszDefSAMConnectionString, m_rConfigData.m_strSQLServerAddr, _T("msdb"));

	// run scripts
	int scriptCount = 0;
	for (auto spJobDesc : m_vecJobs)
	{
		if (spJobDesc->m_bCheck == false) // not checked at the interface
			continue;
		
		CString strCommand;
		
		// first of all try to find job
		strCommand.Format( _T("SELECT job_id FROM msdb.dbo.sysjobs WHERE(name = N'%s')"), spJobDesc->m_strName );
		_RecordsetPtr piRecords = m_storage.ExecuteCommand(strCommand);
		
		if (piRecords->adoEOF || piRecords->BOF)
			continue;

		FieldsPtr	piFields	= piRecords->GetFields();
		FieldPtr	piField		= piFields->GetItem(CComVariant(0));
		if (piField) // job exists
		{
			CString strJobID = piField->GetValue();
			CMNLog(_T("[CRegJobsThread::CreateJobs]: Job '%s' exists"), spJobDesc->m_strDisplayName);

			strCommand.Format(_T("DECLARE @jobId binary(16) \
							SELECT @jobId = job_id FROM msdb.dbo.sysjobs WHERE(name = N'%s') \
							IF(@jobId IS NOT NULL) \
							BEGIN \
								EXEC msdb.dbo.sp_delete_job @jobId \
							END"), spJobDesc->m_strName);

			m_storage.ExecuteCommand(strCommand);
			CMNLog(_T("[CRegJobsThread::CreateJobs]: Job '%s' deleted"), spJobDesc->m_strDisplayName);
			scriptCount++;
			::PostMessage(m_hParentWnd, WM_OCRCFG_SCRIPT_EXCECUTION_STAGE, scriptCount, NULL);
			::Sleep(1000);
		}

	}
	// Wait for a second to apply stored procedures
	::Sleep(1000);

	m_storage.Disconnect();
	return scriptCount;
}



CString CRegJobsThread::CheckOurJobs()
{
	CString strJobs;
	
	//check condition
	m_storage.Connect(cszDefSAMConnectionString, m_rConfigData.m_strSQLServerAddr, _T("msdb"));

	set<CAtlString> setResult;
	//Get Tables list
	setResult = m_storage.ListJobs();
	
	// C++ 11 like cycle
	for (auto spJobDesc : m_vecJobs)
	{
		if (spJobDesc->m_bCheck == false) // not checked at the interface
			continue;
		
		CAtlString	str = spJobDesc->m_strName;
		if (setResult.end() == find(begin(setResult), end(setResult), str))
		{	
		}
		else
		{
			strJobs += spJobDesc->m_strName;
			strJobs += _T(", ");
		}
	}

	setResult.clear();
	m_storage.Disconnect();

	int nLength = strJobs.GetLength();
	if (nLength > 1)
	{
		if (strJobs[nLength - 2] == WCHAR(','))
		{
			strJobs.SetAt(nLength - 2, WCHAR(' '));
		}
	}

	return strJobs;
}

// CRegJobsThread message handlers
