//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------

#include "stdafx.h"

#include <atlsafe.h>
#include <shlwapi.h>
#include <afxpriv.h>

#include "CMN/include/Log.h"
#include "CMN/include/Misc.h"
#include "cmn/include/RegKey.h"
#include "cmn/include/McException.h"
#include "cmn/include/VersionInfo.h"

#include "T/include/OleMessageFilter.h"

#include "rgs/include/wmihelper.h"

#include "scm/include/SchemaDef.h"

#include "wzr/include/Page.h"
#include "wzr/include/Sheet.h"

#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/SelectServersPage.h"
#include "ocrcfg/include/SoftLibraryPage.h"
#include "ocrcfg/include/DCOMPage.h"
#include "ocrcfg/include/WelcomePage.h"
#include "ocrcfg/include/CredentialsPage.h"
#include "ocrcfg/include/CreateFolderPage.h"
#include "ocrcfg/include/CreateDBPage.h"
#include "ocrcfg/include/ApplyScriptsPage.h"
#include "ocrcfg/include/RegJobsPage.h"
#include "ocrcfg/include/RegServicesPage.h"
#include "ocrcfg/include/DeployRptPage.h"
#include "ocrcfg/include/CommandLineInfo.h"
#include "ocrcfg/include/FinishPage.h"
#include "ocrcfg/include/ConfigApp.h"
#include "ocrcfg/include/misc.h"
#include "ocrcfg/resource/resource.h"


LPCTSTR g_lpszDefaultConfigFileName = _T("ocrcfg.xml");
LPCTSTR g_lpszMutexNameName = _T("OCRConfigMutex");


// CConfigApp
/*
BOOL CConfigApp::m_bConfigInstallerMode = false;
BOOL CConfigApp::m_bConfigReportPackMode = false;
BOOL CConfigApp::m_bImportExportOnlyMode = false;
BOOL CConfigApp::m_bDefaultMode = false;
*/
BEGIN_MESSAGE_MAP(CConfigApp, WZRApp)
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

// CConfigApp construction


CConfigApp::CConfigApp() : m_strLogDir(_T("ocrcfg.log"))    
{
	m_bInstallerMode	= FALSE;
	m_bUnattendedMode	= FALSE;
	m_bReportPackMode	= FALSE;
	m_bOSType			= false;
	m_enScenario		= CUseScenarioEnum::eusCreateNew;
}


// The one and only CConfigApp object

CConfigApp theApp;


// CConfigApp initialization

BOOL CConfigApp::InitInstance()
{	
	HANDLE hCnfgMutex = CreateMutex(NULL, FALSE, g_lpszMutexNameName);
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CAtlString strMsg;
		strMsg.LoadString(IDE_OCRCFG_SECOND_INSTANCE);
		AfxMessageBox(strMsg, MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	Base::InitInstance();

	CMNException::ms_bExtendedReporting = FALSE;
	CMNException::ms_bUseWindowsMessageBox = TRUE;
	CMNException::ms_bAllowWin32Formatting = TRUE;

	TSetComRaiseErrorHandler();

	GrantDebugPrivelegeToYourself();

	// detect OS
	OSVERSIONINFO osv;
	ZeroMemory(&osv, sizeof(OSVERSIONINFO));
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bGetOSOk = ::GetVersionEx( &osv );
	if ( bGetOSOk && (osv.dwPlatformId == VER_PLATFORM_WIN32_NT) )
	{
		if ( osv.dwMajorVersion >= 6 )
		{
			m_bOSType = true;
		}
		else
		{
			if ( CMNIs64BitWindows() )
			{	// XP & Server 2003 x64
				m_bOSType = true;
			}
			else
			{
				m_bOSType = false;
			}
		}
	}

	
    InitLog();
//  InitInspections();

//	AfxOleInit();
//	AfxEnableControlContainer();

	// Enable html help
	CString strHelpPath = CMNGetFullCurrentPath(AfxGetInstanceHandle(), _T("Help.chm"));
	DWORD dwHelpPathLen = strHelpPath.GetLength();
	m_bHelpMode = TRUE;
	if(m_pszHelpFilePath)
		delete m_pszHelpFilePath;
	m_pszHelpFilePath = new TCHAR[dwHelpPathLen + 1];
	memset((void*) m_pszHelpFilePath, 0, sizeof(TCHAR) * (dwHelpPathLen + 1));
	memcpy((void*) m_pszHelpFilePath, strHelpPath.GetBuffer(), sizeof(TCHAR) * (dwHelpPathLen + 1));

	EnableHtmlHelp();

	// this will work for CURRENT_USER registry hive, but here in configurator we work with local system hive
	SetRegistryKey(_T("DeliceInfo\\"));
	SetRigistryProfileName(_T("Configurator\\"));
	//

    // Register message filter
    if (m_pMessageFilter)
    {
        m_pMessageFilter->Revoke(); // but not release()!
        delete m_pMessageFilter; // do not forget to delete old one
    }
	m_pMessageFilter = new TOleMessageFilter;
    m_pMessageFilter->Register();
    m_pMessageFilter->EnableBusyDialog(FALSE);
    m_pMessageFilter->EnableNotRespondingDialog(FALSE);


	// Parse commmand line and extract params
	SamCfgCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	/*
    if(cmdInfo.IsQueryParamList())
    {
        AfxMessageBox(	_T("Список флагов и параметров запуска:\n\n")
                        _T("/deploy - запуск с заданным файлом настройки, синтаксис: /deploy file_name.xml.\n\n")
                        _T("/DeployReports -  используется в сочетании с \"/unattended\".") 
						_T("если данный флаг единственный, происходит запуск конфигуратора без чтения файла настроек,")
						_T("конфигуратор стартует со страницы публикации отчетов.\n\n")
						_T("/help или /? - Показать данное сообщение\n\n"
                        _T("/install - Запускает конфигуратор (обычно из инсталлятора), используя файл настроек ")
						_T("\"ocrcfg.xml\", он должен находится в одной директории с ")
						_T("запускаемым файлом. ")
						_T("Флаг \"/reportpack\" не будет учитываться, если он стоит перед данным флагом.\n\n")
                        _T("/reportpack - не подгружает файл настроек, стартует с первой страницы, ")
						_T("позволяя только подгузить репортпаки. Если флаг \"/unattended\" стоит ")
						_T("перед данным флагом, он не будет учитываться.\n\n")
                        _T("/unattended - То же что \"/install\", но кнопка \"Next\" нажимается автоматически ")
						_T("и не отображаются сообщения в случае возникновения ошибок. ")
						_T("Если перед данным флагом использовался флаг ")
						_T("\"/reportpack\", этот флаг учитываться не будет.\n\n")), MB_OK | MB_ICONINFORMATION);
		return FALSE;
    }
	*/
	m_bInstallerMode = cmdInfo.IsInstall();
	m_bUnattendedMode = cmdInfo.IsUnattended();
	m_bReportPackMode = cmdInfo.IsReportPack();
	// the variable m_bConfigInstallerMode uses for help display
	/*
	m_bConfigInstallerMode = m_bInstallerMode;
	m_bConfigReportPackMode = m_bReportPackMode;
	*/
	// read config XML file
	CString strConfigFilePath = cmdInfo.GetConfigFilePath();
	if(m_bInstallerMode || m_bUnattendedMode) 
		strConfigFilePath = g_lpszDefaultConfigFileName; 

	if(!strConfigFilePath.IsEmpty())
	{
		CString strConfigFullFilePath = SamCfgGetFileFullPath(strConfigFilePath);
		m_data.LoadConfig(strConfigFullFilePath);
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization


    CBitmap bmWatermark;
    CBitmap bmHeader;
    
    bmWatermark.LoadBitmap(IDB_OCRCFG_WATERMARK);
    bmHeader.LoadBitmap(IDB_OCRCFG_HEADER);

	// construct sheet title with version info
	CString strSheetTitle;
	strSheetTitle.LoadString(AFX_IDS_APP_TITLE);

	CString strVersionInfo;
	CMNVersionInfo vi;
	BOOL bVer = vi.LoadVersionInfo(::GetModuleHandle(NULL));
	if (bVer)
	{
		strVersionInfo.Format(_T("%d.%d.%d.%d"), vi.GetMajorHi(), vi.GetMajorLo(), vi.GetMinorHi(), vi.GetMinorLo());
	}
	strSheetTitle += _T(" ");
	strSheetTitle += strVersionInfo;

	WZRSheet sheet(strSheetTitle, NULL, 0, bmWatermark, NULL, bmHeader);
	/*
	try
	{
		IRptManPtr piRptMan;
		HRESULT hr = piRptMan.CreateInstance(__uuidof(RptMan));
		CMNCheckError(hr);
		m_spRptManGIT = piRptMan;
	}
	catch(TException* pTEx)
	{
		TException* pHiLevelEx = new TException(IDE_RPTMAN_LOAD_ERROR);
		*pHiLevelEx += *pTEx;
		CString strAllErrors = CMNException::GetAllErrorMessages(pHiLevelEx, _T("\n"));
		CMNLog(_T("[CConfigApp::InitInstance]: %s"), strAllErrors);
		if ( ! m_bUnattendedMode )
		{
			pHiLevelEx->ReportError();
		}
		pHiLevelEx->Delete();
	}
	*/
	// process DeployReports command line param
	if(cmdInfo.IsDeployReports())
	{
		m_data.DisableCreateServer();
		m_data.DisableRunSqlScripts();
		m_data.DisableImportSoftLibrary();
	}

	int nModificationFlags = 0; //Возможные значения см. в Constans.h в разделе Configurator Modification Flags

	COcrConfigData	dataConfig;


	try
	{
		// create wizard
		CWelcomePage		pageWelcome;
		CCredentialsPage	pageCredentials (dataConfig);
		CCreateFolderPage	pageCreateFolder(dataConfig);
		CCreateDBPage		pageCreateDB	(dataConfig, &nModificationFlags);
		CApplyScriptsPage	pageApplyScripts(dataConfig, &nModificationFlags);
		CRegJobsPage		pageRegJobs		(dataConfig, &nModificationFlags);
		CRegServicesPage	pageRegServices	(dataConfig, &nModificationFlags);
		//CDeployRptPage		pageDeployRpt	(&nModificationFlags);
		//CSelectServersPage	pageSelectServers(&nModificationFlags);
		//CSoftLibraryPage		pageSoftLibrary;
		//CDCOMPage				pageDCOM;
		CFinishPage			pageFinish		(&nModificationFlags);

		// при добавлении страниц проверить номер стартовой!
		sheet.AddPage(&pageWelcome);			// 0		
		sheet.AddPage(&pageCredentials);		// 1		
		sheet.AddPage(&pageCreateFolder);		// 2		
		sheet.AddPage(&pageCreateDB);			// 3
		sheet.AddPage(&pageApplyScripts);		// 4
		sheet.AddPage(&pageRegJobs);			// 5
		sheet.AddPage(&pageRegServices);		// 6
		//sheet.AddPage(&pageDeployRpt);		// 
		//sheet.AddPage(&pageSelectServers);	// 
		//sheet.AddPage(&pageSoftLibrary);		// 
		//sheet.AddPage(&pageDCOM);				// 
		sheet.AddPage(&pageFinish);				// 6
		
		for (int i = 0; i < sheet.GetPageCount(); i++)
		{
			CPropertyPage* pPage  = sheet.GetPage(i);			
			pPage->m_psp.pszTitle = (LPCTSTR) strSheetTitle;			
		}
		

		// set general sheet properties
		sheet.m_psh.dwFlags |= (PSH_WIZARD97 | PSH_HEADER);
		//sheet.m_psh.dwFlags &= (~PSH_HASHELP);
		sheet.m_psh.dwSize = sizeof(PROPSHEETHEADER);
		sheet.SetWizardMode();

		/*
		DWORD dwSelPage = 0;	// welcome by default
		if(!strConfigFilePath.IsEmpty())
		{
			if (m_data.IsCreateServer() || m_data.IsRunSqlScripts())
				dwSelPage = 3;
			else if (m_data.IsDeployReports())
				dwSelPage = 4;
			else if (m_data.IsImportSoftLibrary())
				dwSelPage = 5;
			else if(!m_data.IsCreateServer() && !m_data.IsRunSqlScripts()
					&& !m_data.IsDeployReports() && !m_data.IsImportSoftLibrary())
				dwSelPage = 6;
		}
		sheet.SetActivePage(dwSelPage);
		*/
		UINT nPageID = IDD_OCRCFG_WELCOME_PAGE;
		if (m_bInstallerMode == TRUE)
			nPageID = IDD_OCRCFG_CREDENTIALS_PAGE;
		sheet.SetActivePage(nPageID);
				
		m_pMainWnd = &sheet;

		INT_PTR nResponse = sheet.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
	}
	catch(CMNException* pEx)
	{
		TException* pHiLevelEx = new TException();
		*pHiLevelEx += *pEx;
		CAtlString strAllErrors = CMNException::GetErrorMessage(pEx);
		CMNLog(_T("[CConfigApp::InitInstance]: %s"), strAllErrors);
		pHiLevelEx->ReportError();
		pHiLevelEx->Delete();
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CConfigApp::ExitInstance()
{
    if(m_pMessageFilter)
    {
        delete m_pMessageFilter;
        m_pMessageFilter = NULL;
    }

	CRegKey		keyRoot;
	LONG		lRes = keyRoot.Open(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot );

	if (lRes == ERROR_SUCCESS)
	{
		keyRoot.SetStringValue(cszSAMLogDir, m_strLogDir);
        keyRoot.SetStringValue(cszSAMInspectionsDir, m_strInspectionsDir); 
	}

//	m_spRptManGIT.Revoke();

 //	FinishLang();
  	CMNLogClose();
	//AfxOleTerm();
	return Base::ExitInstance();
}

HRESULT CConfigApp::InitLog()
{
	HRESULT		hr = S_OK;
	TCHAR		szDefaultLogName[]	= _T("ocrcfg.log");

	CRegKey		keyRoot;
	LONG		lRes = keyRoot.Open(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot );

	if (lRes != ERROR_SUCCESS)
	{
		lRes = keyRoot.Create(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot); 
	}
	if (lRes == ERROR_SUCCESS)
	{
        TCHAR szBuf[MAX_PATH];
        ULONG nChars;        
        nChars		= sizeof(szBuf)/sizeof(TCHAR);

        lRes = keyRoot.QueryStringValue(cszSAMLogDir, szBuf, &nChars);        
		if (lRes == ERROR_SUCCESS)
			m_strLogDir	= szBuf;
		else
			m_strLogDir   = SCMGetDefSAMLogDirValue(); 
	}
	else
	{
		m_strLogDir   = SCMGetDefSAMLogDirValue();
	}
	hr = CMNCreateFullDirectory(m_strLogDir);
	if (FAILED(hr))
		return hr;

	CString strLogFileName;
	strLogFileName.Format(_T("%s\\%s"), m_strLogDir, szDefaultLogName);
	return CMNLogInit(strLogFileName);
}

HRESULT CConfigApp::InitInspections()
{       
	HRESULT		hr = S_OK;

	CRegKey		keyRoot;
	LONG		lRes = keyRoot.Open(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot );

	if (lRes != ERROR_SUCCESS)
	{
		lRes = keyRoot.Create(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot); 
	}
	if (lRes == ERROR_SUCCESS)
	{
        TCHAR szBuf[MAX_PATH];
        ULONG nChars = sizeof(szBuf) / sizeof(TCHAR);

        lRes = keyRoot.QueryStringValue(cszSAMInspectionsDir, szBuf, &nChars);        
		if (lRes == ERROR_SUCCESS)
			m_strInspectionsDir	= szBuf;
		else
			m_strInspectionsDir   = SCMGetDefSAMInspectionsDirValue(); 
	}
	else
	{
		m_strInspectionsDir   = SCMGetDefSAMInspectionsDirValue();
	}
	hr = S_OK; //CMNCreateFullDirectory(m_strInspectionsDir);
    return hr;
}
/*
IRptManPtr CConfigApp::GetRptMan()
{
	IRptManPtr piRptMan;
	m_spRptManGIT.CopyTo(&piRptMan);
	return piRptMan;
}
*/
//[d.titov] DO NOT REMOVE comments below
//void CConfigApp::ListLocalSqlServers(std::set<bstr_t>& lstServers)
//{
//	try
//	{
//		lstServers.clear();
//		const CString strDefaultInstance = _T("MSSQLSERVER");
//		RGSWMIHelper wmi;
//		
//
//		//Получить имя локального компьютера
//		bstr_t bstrComputer = "";
//		char cszHostName[128]="";
//		int iHostNameResult=gethostname(cszHostName,sizeof(cszHostName));
//		if(iHostNameResult==S_OK)
//		{
//			bstrComputer=bstr_t(cszHostName);
//		}
//
//		
//		bstr_t bstrBaseSQLNamespace = "root\\Microsoft\\SqlServer";
//		wmi.connect("\\\\"+bstrComputer, bstrBaseSQLNamespace);
//		TWMITableList rWmiTableList = wmi.ExecQuery( bstr_t("SELECT * FROM __NAMESPACE where NAME like \"ComputerManagement%\"") );
//		list<bstr_t> namespacesList;
//		for (TWMITableListIterator it = rWmiTableList.begin(); it != rWmiTableList.end(); ++it)
//		{
//			bstr_t bstrNsName = wmi.GetPropertyStrValue(*it, _T("Name"));
//			if ( bstrNsName.length() > 0 )
//			{
//				namespacesList.push_back( bstrNsName );
//			}
//		}
//		for (list<bstr_t>::iterator it = namespacesList.begin(); it != namespacesList.end(); ++it)
//		{
//			bstr_t bstrName = *it;
//			//
//			wmi.connect("\\\\"+bstrComputer, bstrBaseSQLNamespace + bstr_t("\\") + bstrName);
//			//
//			TWMITableList rServerSettings = wmi.ExecQuery( bstr_t("SELECT * FROM SqlService WHERE SqlServiceType=1") );
//			for(TWMITableListIterator itSettings = rServerSettings.begin(); itSettings != rServerSettings.end(); ++itSettings)
//			{
//				bstr_t bstrInstance = wmi.GetPropertyStrValue(*itSettings, _T("ServiceName"));
//				if ( bstrInstance.length() > 0 )
//				{
//					CAtlString _bstrInstance=bstrInstance;
//					int iFirst=_bstrInstance.Find(_T("$"))+1;
//					bstrInstance=_bstrInstance.Mid(iFirst, _bstrInstance.GetLength()-iFirst);
//					lstServers.insert( bstrComputer + "\\"+bstrInstance);
//				}
//			}
//		}
//	}
//	catch(CMNException* pEx)
//	{
//		if ( ! m_bUnattendedMode )
//		{
//			CString strErr = CMNException::GetAllErrorMessages(pEx);
//			pEx->Delete();
//		}
//	}
//}

//void CConfigApp::ListLocalReportServers(RS_NAMESPACES& namespaces_list)
//{
//	namespaces_list.clear();
//	bstr_t bstrComputer = "";
//	bstr_t bstrBaseReportServerNamespace = "root\\Microsoft\\SqlServer\\ReportServer";
//
//	list<bstr_t> namespacesList;
//
//	SearchBaseReportServerNamespace(bstrBaseReportServerNamespace, namespacesList);
//
//	for (list<bstr_t>::iterator itNamespace = namespacesList.begin(); itNamespace != namespacesList.end(); itNamespace++)
//	{
//		ReportingServiceNamespace rsns;
//		rsns.bstrNamespace = *itNamespace;
//		try
//		{
//			RGSWMIHelper wmi;
//			bstr_t bstrRSNamespace = *itNamespace;
//			wmi.connect(bstrComputer, bstrRSNamespace);
//			TWMITableList rReportServerInstance = wmi.ExecQuery( bstr_t("SELECT * FROM MSReportServer_Instance") );
//			for (TWMITableListIterator it = rReportServerInstance.begin(); it != rReportServerInstance.end(); ++it)
//			{
//				variant_t vVersion = wmi.GetPropertyValue(*it, _T("Version"));
//				int version = 9;
//				if (vVersion.vt == VT_BSTR)
//				{
//					version = atoi( (bstr_t) vVersion );
//				}
//				//
//				if ( version == 9 )
//				{
//					bstr_t bstrURL = wmi.GetPropertyStrValue(*it, _T("ReportServerUrl"));
//					rsns.lstServers.insert( bstrURL );
//				}
//				else
//				{
//					bstr_t bstrClassName		= wmi.GetPropertyValue(*it, "__CLASS");
//					bstr_t bstrObjectPath		= wmi.GetPropertyValue(*it, "__RELPATH");
//					TWMIPropertyMap in_params, out_params;
//					wmi.ExecWMIMethod( bstrClassName, bstrObjectPath, _T("GetReportServerUrls"), in_params, out_params);
//					//
//					CComSafeArray<BSTR> arrURLs;
//					arrURLs = out_params["URLs"].parray;
//					CComSafeArray<BSTR> arrApps;
//					arrApps = out_params["ApplicationName"].parray;
//					UINT nURLs = arrURLs.GetCount();
//					for (UINT i=0; i<nURLs; i++)
//					{
//						CString strDesc, strURL;
//						strDesc	= arrApps.GetAt(i);
//						strURL	= arrURLs.GetAt(i);
//						if ( strDesc == _T("ReportServerWebService") )
//						{
//							rsns.lstServers.insert( (const TCHAR*) strURL );
//						}
//					}
//				}
//			}
//		}
//		catch (CMNException* pEx)
//		{
//			CString strErr = CMNException::GetAllErrorMessages(pEx);
//			pEx->Delete();
//		}
//		catch(...)
//		{
//		}
//		//
//		namespaces_list.push_back( rsns );
//	}
//}

void CConfigApp::SearchBaseReportServerNamespace(bstr_t bstrBaseNamespace, list<bstr_t>& namespacesList)
{	// вызывается рекурсивно
	try
	{
		RGSWMIHelper wmi;
		TWMITableList rReportServerInstance;
		bstr_t bstrComputer = "";
		wmi.connect(bstrComputer, bstrBaseNamespace);
		try
		{
			rReportServerInstance = wmi.ExecQuery( bstr_t("SELECT * FROM MSReportServer_Instance") );
			if ( ! rReportServerInstance.empty() )
			{
				namespacesList.push_back( bstrBaseNamespace );
			}
		}
		catch(CMNException* pEx)
		{
			HRESULT hr = pEx->GetHRESULT();
			if ( hr == WBEM_E_INVALID_CLASS )
			{
				pEx->Delete();
			}
			else
			{
				pEx->Throw();
			}
		}
		TWMITableList rReportServerNamespaces = wmi.ExecQuery( bstr_t("SELECT * FROM __NAMESPACE") );		
		for (TWMITableListIterator it = rReportServerNamespaces.begin(); it != rReportServerNamespaces.end(); ++it)
		{
			bstr_t bstrName = wmi.GetPropertyValue(*it, _T("Name"));
			bstr_t bstrFoundNamespace = bstrBaseNamespace + "\\" + bstrName;
			SearchBaseReportServerNamespace(bstrFoundNamespace, namespacesList);
		}
	}
	catch (CMNException* pEx)
	{
		CString strErr;
		strErr.Format( _T("Error search Report Server namespaces: %s\r\n"),
			CMNException::GetAllErrorMessages(pEx));
		pEx->Delete();
	}
	catch(...)
	{
	}
}


ATL::CAtlString CConfigApp::GetSQLServerHostName() const 
{ 
	if(m_vecSQLServerName.size() == 2)
	{
		return *m_vecSQLServerName.begin(); 
	}
	return _T("");
}


ATL::CAtlString CConfigApp::GetSQLServerUserEnterName() const 
{ 
	if(m_vecSQLServerName.size() == 2)
	{
		return *(m_vecSQLServerName.begin() + 1); 
	}
	return _T("");
}


void CConfigApp::SetSQLServerName(const ATL::CAtlString& rin_strSQLServerHostName, const ATL::CAtlString& rin_strSQLServerIPName) 
{ 
	m_vecSQLServerName.clear();
	m_vecSQLServerName.push_back(rin_strSQLServerHostName);
	m_vecSQLServerName.push_back(rin_strSQLServerIPName);
} 


void CConfigApp::ShowHelp(DWORD dwDlgResID)
{
	//TWaitCursor wait;

	CString strHelpTopic = GetHelpPathForDlgResID(dwDlgResID);

	HtmlHelp((DWORD_PTR) strHelpTopic.GetString(), HH_DISPLAY_TOPIC);
}


CString CConfigApp::GetHelpPathForDlgResID(DWORD dwDlgResID)
{
	/*
	static map<DWORD, CString> mapHelpTopics;
	mapHelpTopics.clear();
	//CMNBEGIN_INIT_STATIC(mapHelpTopics)

		CString strPath;

		strPath = _T("/Setup%20Guide/ChangeConfigurationSamServer/ChangeConfigurationSamServer.htm");
		mapHelpTopics[IDD_OCRCFG_WELCOME_PAGE] = strPath;

		if(m_bInstallerMode) // install mode
		{
			strPath = _T("/Setup%20Guide/SetupServerAndSamMgr.htm#1");
			mapHelpTopics[IDD_OCRCFG_CREATEDB_PAGE] = strPath;

			strPath = _T("/Setup%20Guide/SetupServerAndSamMgr.htm#2");
			mapHelpTopics[IDD_OCRCFG_DEPLOYRPT_PAGE] = strPath;

			strPath = _T("/Setup%20Guide/SetupServerAndSamMgr.htm#3");
			mapHelpTopics[IDD_OCRCFG_SOFTLIBRARY_PAGE] = strPath;		
		}
		else if(m_bReportPackMode) // deploy reports
		{
			strPath = _T("/Setup%20Guide/ChangeConfigurationSamServer/PublicationAdditionalReports.htm");
			mapHelpTopics[IDD_OCRCFG_CREATEDB_PAGE] = strPath;

			strPath = _T("/Setup%20Guide/ChangeConfigurationSamServer/PublicationAdditionalReports.htm#1");
			mapHelpTopics[IDD_OCRCFG_DEPLOYRPT_PAGE] = strPath;

			strPath = _T("/Setup%20Guide/ChangeConfigurationSamServer/PublicationAdditionalReports.htm#1");
			mapHelpTopics[IDD_OCRCFG_SOFTLIBRARY_PAGE] = strPath;				
		}
		else
		{
			strPath = _T("/Setup%20Guide/ChangeConfigurationSamServer/CreateNewDB.htm");
			mapHelpTopics[IDD_OCRCFG_CREATEDB_PAGE] = strPath;

			strPath = _T("/Setup%20Guide/ChangeConfigurationSamServer/CreateNewDB.htm#1");
			mapHelpTopics[IDD_OCRCFG_DEPLOYRPT_PAGE] = strPath;

			if(m_bImportExportOnlyMode)
			{
				strPath = _T("/Setup%20Guide/ChangeConfigurationSamServer/ExportAndImportLibrary.htm");
			}
			else
			{
				strPath = _T("/Setup%20Guide/ChangeConfigurationSamServer/CreateNewDB.htm#2");
			}
			mapHelpTopics[IDD_OCRCFG_SOFTLIBRARY_PAGE] = strPath;
		}

		strPath = _T("/Setup%20Guide/ChangeConfigurationSamServer/ConnectToExistingDB.htm");
		mapHelpTopics[IDD_OCRCFG_SELECTSERVERS_PAGE] = strPath;

		strPath = _T("/Setup%20Guide/ChangeConfigurationSamServer/ConfiguringDCOM.htm");
		mapHelpTopics[IDD_OCRCFG_DCOM_PAGE] = strPath;

		strPath = _T("/Common/SystemOverview/PurposeAndMainFunctions.htm");
		mapHelpTopics[IDD_OCRCFG_LICENSE_PAGE] = strPath;
		mapHelpTopics[IDD_OCRCFG_FINISH_PAGE] = strPath;

	//CMNEND_INIT_STATIC(mapHelpTopics)
	*/
	CString strHelpTopic;
	/*
	map<DWORD, CString>::iterator it = mapHelpTopics.find(dwDlgResID);
	if(it != mapHelpTopics.end())
		strHelpTopic = it->second;
	else
		strHelpTopic = _T("/Common/SystemOverview/PurposeAndMainFunctions.htm");
	*/
	return strHelpTopic;
}


void CConfigApp::WinHelpInternal(DWORD_PTR dwData, UINT nCmd)
{
	DWORD nID = dwData - HID_BASE_RESOURCE;
	ShowHelp(nID);
}

#include "cmn/include/AuthorityGrantor.h"

BOOL SetPrivilege(
	HANDLE hToken,          // access token handle
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable or disable privilege
)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
		NULL,            // lookup privilege on local system
		lpszPrivilege,   // privilege to lookup 
		&luid))        // receives LUID of privilege
	{
		printf("LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		printf("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

	{
		printf("The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}

BOOL CConfigApp::GrantDebugPrivelegeToYourself()
{
	BOOL	bGranted = FALSE;
	BOOL	bOK;
	HANDLE	hToken = NULL;
	HANDLE	hCurrentProcess;
	hCurrentProcess = GetCurrentProcess(); // 1
	bOK = OpenProcessToken(hCurrentProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	if (bOK && hToken)
	{
		bGranted = SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
		CloseHandle(hToken);
	}	
	CloseHandle(hCurrentProcess);

	return bGranted;
}


