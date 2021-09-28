//--------------------------------------------------------------------------------
//  Copyright: (C) 2009-2010 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit: ConfigXMLData.h
//  Cooperation:
//  Date:        2010-01-25
//--------------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------------

#include "StdAfx.h"

#include <wincrypt.h>
#include "CMN/include/Exception.h"
#include "CMN/include/McException.h"
#include "CMN/include/XMLUtilities.h"
#include "CMN/include/Misc.h"
#include "cmn/include/Log.h"
#include "CMN/include/RegKey.h"
#include "CMN/include/SimpleCodec.h"
#include "cmn/include/ZlibWrapper.h"

#include "scm/include/SchemaDef.h"

#include "ocrcfg/include/ConfigXMLData.h"
#include "ocrcfg/include/Constants.h"

#include "cmn/include/VersionInfo.h" //[m.fomin]

const TCHAR cszDeployInfoNode[] = _T("deploy-info");
const TCHAR cszProductVersion[] = _T("product_version"); //[m.fomin]
const TCHAR cszSqlServerNode[] = _T("sql-server");
const TCHAR cszServerName[] = _T("server_name");
const TCHAR cszDbName[] = _T("db_name");
const TCHAR cszSqlScriptsNode[] = _T("sql-scripts");
const TCHAR cszSqlScript[] = _T("script");
const TCHAR cszStepNumber[] = _T("step_number");
const TCHAR cszSqlScriptFile[] = _T("sql_script_file");
const TCHAR cszReportsNode[] = _T("reports");
const TCHAR cszServerURL[] = _T("server_url");
const TCHAR cszWebFolder[] = _T("web_folder");
const TCHAR cszVisibleName[] = _T("visible_name");
const TCHAR cszReportFolder[] = _T("report_folder");
const TCHAR cszRdlFile[] = _T("rdl_file");
const TCHAR cszParamList[] = _T("parameter_list");
const TCHAR cszReportType[] = _T("report_type");
const TCHAR cszSoftLibraryImportNode[] = _T("soft_library_import");
const TCHAR cszSoftLibraryFile[] = _T("soft_library_file");
const TCHAR cszName[] = _T("name");



//
//		class COcrConfigData
//		used to store and load credentials and SQL server adresses
//


COcrConfigData::COcrConfigData()
{


}

COcrConfigData::~COcrConfigData()
{


}


constexpr TCHAR szPasswordMask[] = _T("1C722AEC5E7A4c52BA01E2953D558CC1");

void COcrConfigData::LoadFromRegistry()
{
	CMNRegKey keyRoot;
	try
	{
		keyRoot.OpenEx(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot, KEY_READ);

		m_strLogin			= keyRoot.QueryStringValue(cszSAMLogin,			_T(""));
		//m_strPassword		= keyRoot.QueryStringValue(cszSAMPassword,		_T(""));
		m_strSQLServerAddr	= keyRoot.QueryStringValue(cszSAMSqlServer,		_T(""));
		m_strComputerAddr	= keyRoot.QueryStringValue(cszSAMSqlComputer,	_T(""));

		m_strProjectFolder	= keyRoot.QueryStringValue(cszOCR_ProjectFolder,	_T(""));
		m_strScansFolder	= keyRoot.QueryStringValue(cszOCR_ScansFolder,		_T(""));

		// decode secret mark
		ULONG	lResult;
		BYTE	pBytes[10000];
		ULONG	ulBytesCount = sizeof(pBytes);

		lResult = keyRoot.QueryBinaryValue(cszSAMPasswordBinary, &pBytes, &ulBytesCount);
		if ( (lResult == ERROR_SUCCESS) && (ulBytesCount != 0) )
		{
			//CAtlString strPassword;
			CMNSimpleCodec codec(szPasswordMask);
			m_strPassword = codec.Decode( (BYTE*)pBytes, ulBytesCount);
			//ASSERT(m_strPassword.Compare(strPassword) == 0);
		}
	}
	catch (CMNException* pEx)
	{
		CString strAllErrors = CMNException::GetAllErrorMessages(pEx, _T("\n"));
		CMNLog(_T("[COcrConfigData::LoadFromRegistry]: %s"), strAllErrors);
		pEx->Delete();
	}
}

void COcrConfigData::SaveToRegistry()
{
	CMNRegKey keyRoot;
	try
	{
		keyRoot.OpenEx(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot, KEY_WRITE);

		CMNCheckWIN32Error(keyRoot.SetStringValue(cszSAMLogin,			m_strLogin));
		//CMNCheckWIN32Error(keyRoot.SetStringValue(cszSAMPassword,		m_strPassword));
		CMNCheckWIN32Error(keyRoot.SetStringValue(cszSAMSqlServer,		m_strSQLServerAddr));
		CMNCheckWIN32Error(keyRoot.SetStringValue(cszSAMSqlComputer,	m_strComputerAddr));

		CMNCheckWIN32Error(keyRoot.SetStringValue(cszOCR_ProjectFolder, m_strProjectFolder));
		CMNCheckWIN32Error(keyRoot.SetStringValue(cszOCR_ScansFolder,	m_strScansFolder));
		
		// encode computer name
		CMNSimpleCodec	codec(szPasswordMask);
		LONG			lResult;
		BYTE*			pBytes = NULL;
		ULONG			ulBytesCount = 0;

		ulBytesCount = codec.Encode((LPCTSTR)m_strPassword, &pBytes);
		if (ulBytesCount != 0)
		{
			lResult = keyRoot.SetBinaryValue(cszSAMPasswordBinary, pBytes, ulBytesCount);
			delete pBytes;
			CMNCheckWIN32Error(lResult);
		}
	}
	catch (CMNException* pEx)
	{
		CString strAllErrors = CMNException::GetAllErrorMessages(pEx, _T("\n"));
		CMNLog(_T("[COcrConfigData::SaveToRegistry]: %s"), strAllErrors);
		pEx->Delete();
	}
}




//
//		class CConfigXMLData
//		used to store information about the sql script pack
//



CConfigXMLData::CConfigXMLData():
	m_bIsCreateServer(false),
	m_bIsRunSqlScripts(false),
	m_bIsDeployReports(false),
	m_bIsImportSoftLibrary(false),
	m_bIsEnableClearConfig(true)
{
}

CConfigXMLData::~CConfigXMLData()
{
	DeleteReportPackArchiveFolder();
}

void CConfigXMLData::LoadConfig(CString const& strConfigPath)
{
	CString strExt = PathFindExtension(strConfigPath);
	if ( strExt.CompareNoCase( _T(".zip")) == 0 )
	{
		LoadReportPackArchive( strConfigPath );
	}
	else
	{
		LoadXML( strConfigPath );
	}
}

void CConfigXMLData::ClearConfig()
{
	m_strXMLConfigPath = _T("");
	m_strXMLBody = _T("");
	m_bIsCreateServer = false;
	m_bIsRunSqlScripts = false;
	m_bIsDeployReports = false;

	m_strSqlServer = _T("");
	m_strDBName = _T("");

	m_setScriptFilePaths.clear();

	m_strReportURL = _T("");
	m_strWebFolder = _T("");
	m_arrReportsInfo.clear();
}

void CConfigXMLData::LoadReportPackArchive(CString const& strArchivePath)
{
	DWORD dwRes = 0;
	TCHAR szTempPath[MAX_PATH] = {0};

	dwRes = GetTempPath(MAX_PATH, szTempPath);
	if (0 == dwRes) CMNCheckWIN32Error( GetLastError() );
	
	m_strReportPackArchiveFolder = szTempPath;
	m_strReportPackArchiveFolder += PathFindFileName(strArchivePath);
	
	CMNCheckError( CMNCreateFullDirectory(m_strReportPackArchiveFolder) );

	wchar_t strDrive[MAX_PATH], strDir[MAX_PATH], strFname[MAX_PATH], strExt[MAX_PATH];
	_wsplitpath(strArchivePath, strDrive, strDir, strFname, strExt);

	try
	{
		ExtractArchive(strArchivePath, m_strReportPackArchiveFolder);

		TCHAR szConfigFile[MAX_PATH] = {0};
		_stprintf(szConfigFile, _T("%s\\%s.xml"), (const TCHAR*)m_strReportPackArchiveFolder, 
			(const TCHAR*) strFname);

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = FindFirstFile(szConfigFile, &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			_stprintf(szConfigFile,
				_T("%s\\%s"),
				(const TCHAR*)m_strReportPackArchiveFolder,
				FindFileData.cFileName);
			FindClose(hFind);
		}

		LoadXML(szConfigFile, true);
	}
	catch(CMNException* pEx)
	{
		DeleteReportPackArchiveFolder();
		ThrowException(pEx);
	}
}

void CConfigXMLData::LoadXML(CString const& strXMLConfigPath, bool bIsArchive)
{
	wchar_t strDrive[MAX_PATH], strDir[MAX_PATH], strFname[MAX_PATH], strExt[MAX_PATH];
	_wsplitpath(strXMLConfigPath, strDrive, strDir, strFname, strExt);

	if (m_bIsEnableClearConfig)
	{ 
		ClearConfig();
	}
	else
	{
		m_bIsEnableClearConfig = true; // Auto enable clear config. 
	}

	try
	{
		if (strXMLConfigPath.IsEmpty())
		{
			ASSERT(false);
			return ;
		}

		bstr_t bstrXML = XMLUtilities::ReadXMLFile(bstr_t(strXMLConfigPath));
		MSXML::IXMLDOMNodePtr spNode = XMLUtilities::GetDocumentNode(bstrXML);
		if ( spNode == NULL )
		{
			ASSERT(false);
			return ;
		}

		CComBSTR bstrNodeName;
		spNode->get_nodeName(&bstrNodeName);
		CString strNodeName(bstrNodeName);
		if(strNodeName.CompareNoCase(cszDeployInfoNode) != 0)
		{
			CString strErrorMsgFormat, strErrorMsg;
			strErrorMsgFormat.LoadString(IDE_OPENING_CONFIGXML_ERROR);
			strErrorMsg.Format(strErrorMsgFormat, strXMLConfigPath);
			AfxMessageBox(strErrorMsg, MB_OK | MB_ICONEXCLAMATION);

			return ;
		}

		// get product_version [m.fomin]
		bstr_t bstrRPProductVersion;
		XMLUtilities::GetAttribute(spNode, bstr_t(cszProductVersion), bstrRPProductVersion);
		
		//Сравнить версию КБИ указанную в xml-файле конфигурации с текущей версией КБИ [m.fomin]
		if(!CheckProductVersion((BSTR)bstrRPProductVersion, strXMLConfigPath))
		{
			return;
		}

		m_strXMLBody = (LPCTSTR) bstrXML;
		m_setXMLBody.insert(m_strXMLBody);

		// get sql-server node
		MSXML::IXMLDOMNodePtr spSqlServer = spNode->selectSingleNode(bstr_t(cszSqlServerNode));
		if(spSqlServer)
		{
			CComBSTR bstrCurrXML;
			HRESULT hr = spSqlServer->get_xml(&bstrCurrXML);

			bstr_t bstrSqlServer, bstrDbName;
			XMLUtilities::GetAttribute(spSqlServer, bstr_t(cszServerName), bstrSqlServer);
			XMLUtilities::GetAttribute(spSqlServer, bstr_t(cszDbName), bstrDbName);

			m_strSqlServer = (wchar_t* ) bstrSqlServer;
			m_strDBName = (wchar_t* ) bstrDbName;

			m_strSqlServer.Trim();
			m_strDBName.Trim();

			if(!m_strSqlServer.IsEmpty() || !m_strDBName.IsEmpty())
				m_bIsCreateServer = true;
		}

		// get sql-scripts node
		MSXML::IXMLDOMNodePtr spSqlScripts = spNode->selectSingleNode(bstr_t(cszSqlScriptsNode));
		if(spSqlScripts)
		{
			// enum script files
			MSXML::IXMLDOMNodeListPtr spSqlScriptsList = spSqlScripts->GetchildNodes();
			if(spSqlScriptsList)
			{
				long lLength = spSqlScriptsList->Getlength();
				for(long lIndex = 0; lIndex < lLength; lIndex++)
				{
					MSXML::IXMLDOMNodePtr spSqlScript = spSqlScriptsList->Getitem(lIndex);
					ASSERT(spSqlScript);
					if(!spSqlScript)
						continue ;

					CComBSTR bstrCurrXML;
					HRESULT hr = spSqlScript->get_xml(&bstrCurrXML);

					bstr_t bstrStepNumber, bstrSqlScriptFile;
					XMLUtilities::GetAttribute(spSqlScript, bstr_t(cszSqlScriptFile), bstrSqlScriptFile);

					CString strSqlScriptFile = (wchar_t* ) bstrSqlScriptFile;
					strSqlScriptFile.Trim();

					m_setScriptFilePaths.insert(CString(strDrive) + CString(strDir) + CString(strSqlScriptFile));

					m_bIsRunSqlScripts = true;
				}
			}
		}

		// get reports node
		MSXML::IXMLDOMNodePtr spReports = spNode->selectSingleNode(bstr_t(cszReportsNode));
		if(spReports)
		{
			bstr_t bstrServerURL, bstrWebFolder;
			XMLUtilities::GetAttribute(spReports, bstr_t(cszServerURL), bstrServerURL);
			XMLUtilities::GetAttribute(spReports, bstr_t(cszWebFolder), bstrWebFolder);

			// enum report files
			MSXML::IXMLDOMNodeListPtr spReportsList = spReports->GetchildNodes();
			if(spReportsList)
			{
				long lLength = spReportsList->Getlength();
				if(lLength > 0)
				{
					m_strReportURL = (wchar_t* ) bstrServerURL;
					m_strWebFolder = (wchar_t* ) bstrWebFolder;
				}

				for(long lIndex = 0; lIndex < lLength; lIndex++)
				{
					MSXML::IXMLDOMNodePtr spReport = spReportsList->Getitem(lIndex);
					ASSERT(spReport);
					if(!spReport)
						continue ;

					bstr_t bstrVisibleName, bstrReportFolder, bstrRdlFile, bstrReportType;
					XMLUtilities::GetAttribute(spReport, bstr_t(cszVisibleName), bstrVisibleName);
					XMLUtilities::GetAttribute(spReport, bstr_t(cszReportFolder), bstrReportFolder);
					XMLUtilities::GetAttribute(spReport, bstr_t(cszRdlFile), bstrRdlFile);
					XMLUtilities::GetAttribute(spReport, bstr_t(cszReportType), bstrReportType);

					CString strVisibleName, strReportFolder, strRdlFile, strReportType;
					strVisibleName = (TCHAR*) bstrVisibleName;
					strReportFolder = (TCHAR*) bstrReportFolder;
					strRdlFile = (TCHAR*) bstrRdlFile;
					strReportType = (TCHAR*) bstrReportType;

					strVisibleName.Trim();
					strReportFolder.Trim();
					strRdlFile.Trim();

					if(!strVisibleName.IsEmpty() && !strReportFolder.IsEmpty() && !strRdlFile.IsEmpty())
					{
						m_arrReportsInfo.push_back(
								new ReportInfo(strVisibleName, strReportFolder, strRdlFile, strReportType, strXMLConfigPath)
								);
						m_bIsDeployReports = true;
					}
				}
			}
		}

		// import soft library
		LoadXmlSoftLibraryFiles(spNode, CString(strDrive) + CString(strDir));
	}
	catch(CMNException* pEx)
	{
		CString strErrorMsg = CMNException::GetAllErrorMessages(pEx);
		CMNLog(_T("[CConfigXMLData] Exception occurs. %s"), (LPCTSTR)strErrorMsg);
		AfxMessageBox(strErrorMsg, MB_OK | MB_ICONEXCLAMATION);
		pEx->Delete();

		return ;
	}

	m_strXMLConfigPath = strXMLConfigPath;

	if(bIsArchive)
	{
		// get path to the report folder
		c_setXMLConfigPath.insert(CString(strDrive) + CString(strDir));
	}
}

// -------------------------------------------------------------------------------------------------------------------------
void CConfigXMLData::LoadXmlSoftLibraryFiles(MSXML::IXMLDOMNodePtr spSoftLibraryNode, const wchar_t *strDir)
{
	//Выбрать узел содержащий список файлов
	MSXML::IXMLDOMNodePtr piSoftLibrary = spSoftLibraryNode->selectSingleNode(bstr_t(cszSoftLibraryImportNode));
	if ( piSoftLibrary )
	{
		//Выбрать дочерние узлы содержащие файлы
		MSXML::IXMLDOMNodeListPtr spSoftLibraryFiles = piSoftLibrary->GetchildNodes();
		if(spSoftLibraryFiles)
		{
			long lLength = spSoftLibraryFiles->Getlength();

			//Пройтись по файлам 
			for(long lIndex = 0; lIndex < lLength; lIndex++)
			{
				//Получить имя файла _____________________________________________________________________
				MSXML::IXMLDOMNodePtr spSoftLibaryFile = spSoftLibraryFiles->Getitem(lIndex);
				bstr_t bstrLibraryFile("");
				bool b = XMLUtilities::GetAttribute(spSoftLibaryFile, bstr_t(cszName), bstrLibraryFile);
				if ( b && bstrLibraryFile.length() )
				{
					//Сохранить имя файла в список ___________
					CString strValue((TCHAR*)bstrLibraryFile);
					m_setSoftLibraryFiles.insert(SoftLibraryFileInfoC(strValue, strDir));

					m_bIsImportSoftLibrary = true;
				}

			}
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------------
bool CConfigXMLData::CheckProductVersion(const bstr_t bstrVersion, CString strXMLConfigPath)
{
	if(bstrVersion.length()>(UINT)0)//Если версия КБИ в файле указана
	{
		CMNVersionInfo vi;
		BOOL bVersionLoad = vi.LoadVersionInfo( ::GetModuleHandle(NULL) );
		WORD wMajorHi;
		WORD wMajorLo;
		CString strRPProductVersion((BSTR)bstrVersion);
		int iRes = ::_stscanf_s(strRPProductVersion, _T("%hd.%hd"), &wMajorHi, &wMajorLo);
		if(wMajorHi==vi.GetProductMajorHi() && wMajorLo==vi.GetProductMajorLo())
		{
			return true;
		}
		else
		{
			CString msg1, msg2, msg3;
			msg1.LoadString(IDE_OCRCFG_XML_CONFIG_PRODUCT_VERSION_ERROR);
			msg2.LoadString(IDS_OCRCFG_XML_CONFIG_PRODUCT_VERSION_MSG1);
			msg3.LoadString(IDS_OCRCFG_XML_CONFIG_PRODUCT_VERSION_MSG2);
			CString strCurProductVersion(vi.GetProductVersion());
				
			AfxMessageBox(msg1+strXMLConfigPath+msg2+strRPProductVersion+msg3+strCurProductVersion, MB_OK | MB_ICONEXCLAMATION);
			return false;
		}
	}
	else //Если версия КБИ в файле не указана
	{
		//Делается допущения, что версию забыли указать, но данные при этом совместимы с текущей вресией КБИ.
		//Выводится предупреждение о возможной несовместимости
		CString msg1, msg2;
		msg1.LoadString(IDE_OCRCFG_XML_CONFIG_PRODUCT_VERSION_UNKNOW_MSG1);
		msg2.LoadString(IDE_OCRCFG_XML_CONFIG_PRODUCT_VERSION_UNKNOW_MSG2);

		AfxMessageBox(msg1+strXMLConfigPath+msg2, MB_OK | MB_ICONINFORMATION);
		return true;
	}
  
}
void CConfigXMLData::DisableCreateServer()
{
	m_bIsCreateServer = false;
}

void CConfigXMLData::DisableRunSqlScripts()
{
	m_bIsRunSqlScripts = false;
}

void CConfigXMLData::DisableImportSoftLibrary()
{
	m_bIsImportSoftLibrary = false;
}

// Server Data
CString CConfigXMLData::GetSQLServer()
{
	return m_strSqlServer;
}

void CConfigXMLData::SetSQLServer(CString const& strSQLServer)
{
	m_strSqlServer = strSQLServer;
}

CString CConfigXMLData::GetDBName()
{
	return m_strDBName;
}

void CConfigXMLData::SetDBName(CString const& strDBName)
{
	m_strDBName = strDBName;
}

// Sql Scripts Data
set<CString>	const& CConfigXMLData::GetScriptFilePaths()
{
	return m_setScriptFilePaths;
}

// Reports Data
CString CConfigXMLData::GetConfigFileDir()
{
	if(m_strXMLConfigPath.IsEmpty())
		return _T("");

	// Create all subfolders
	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[_MAX_DIR];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];
	::_wsplitpath_s(m_strXMLConfigPath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	CString strDirPath = drive;
	strDirPath += dir;

	return strDirPath;
}

CString	CConfigXMLData::GetReportServerURL()
{
	return m_strReportURL;
}

CString	CConfigXMLData::GetWebFolder()
{
	return m_strWebFolder;
}

CConfigXMLData::ReportInfo::Vector const& CConfigXMLData::GetReportsInfo()
{
	return m_arrReportsInfo;
}

set<SoftLibraryFileInfoC, SoftLibraryInfoStructCompare> CConfigXMLData::GetSoftLibraryFiles()
{
	return m_setSoftLibraryFiles;
}

CString CConfigXMLData::GetReportPackArchiveFolder()
{
	return m_strReportPackArchiveFolder;
}

void CConfigXMLData::DeleteReportPackArchiveFolder()
{
	try
	{
		for(set<CString>::iterator it = c_setXMLConfigPath.begin(); it != c_setXMLConfigPath.end(); it++)
		{
			if ( (*it).GetLength() )
			{
				CMNCheckError( CMNDeleteFullDirectory( *it ) );
			}
		}
	}
	catch(CMNException* pEx)
	{
		CString strAllErrors = CMNException::GetAllErrorMessages(pEx, _T("\n"));
		CMNLog(_T("[CConfigXMLData::DeleteReportPackArchiveFolder]: %s"), strAllErrors);
		pEx->Delete();
	}
	m_strReportPackArchiveFolder = _T("");
}
