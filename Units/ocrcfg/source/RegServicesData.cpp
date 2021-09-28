//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:  Configurator
//--------------------------------------------------------------------------
//  Description: Data for registering serivces
//--------------------------------------------------------------------------

#include "stdafx.h"

#include "scm/include/SchemaDef.h"
#include "ocrcfg/include/RegServicesData.h"


#pragma warning(disable: 4800)


CDeliceOcrModuleDesc::CDeliceOcrModuleDesc(	LPCTSTR pszName, LPCTSTR pszDisplay, LPCTSTR pszDesc, LPCTSTR pszSubfolder, LPCTSTR pszModuleFileName, 
											LPCTSTR pszDefDatabase, LPCTSTR pszConnectSection, LPCTSTR pszDebugSection)
{
	m_strName				= pszName;
	m_strDisplayName		= pszDisplay;
	m_strDescription		= pszDesc;
	m_strSubfolder			= pszSubfolder;
	m_strModuleFileName		= pszModuleFileName;
	m_strINIFileName		= _T("settings.ini");	
	m_strDefDatabase		= pszDefDatabase;	
	m_strConnectSection		= pszConnectSection;
	m_strDebugSection		= pszDebugSection;

	m_bCheck				= true;
}


CString	CDeliceOcrModuleDesc::GetFullModuleFileName()
{	
	return CString();
}

CString	CDeliceOcrModuleDesc::GetFullIniFileName()
{	
	return CString();
}

CString CDeliceOcrModuleDesc::ConstructConnectionString(LPCTSTR pszSQLServerAddr)
{
	CString str;
	str.Format(cszDefSAMConnectionString, pszSQLServerAddr, m_strDefDatabase);
	return str;
}


void CDeliceOcrModuleDesc::WriteConnectionString(LPCTSTR pszSQLServerAddr)
{
	// connection string
	CString strFullIniFileName	= GetFullIniFileName();
	CString strConnectionString = ConstructConnectionString(pszSQLServerAddr);
	CString	strSection			= m_strConnectSection; // [Connection] for all and [Settings] for DeliceControlPanel
	CString	strKey				= _T("ConnectionString");

	DWORD dw;
	TCHAR szPrevConnectionString[MAX_PATH * 4]; // переменная - куда читаем	
	dw = ::GetPrivateProfileString(strSection, strKey, _T(""), szPrevConnectionString, MAX_PATH * 4, strFullIniFileName);
	if (dw)
	{
		//CMNLog(_T("[CRegServicesThread::CreateServices]: prev connection string: %s"), szPrevConnectionString);
	}
	dw = ::WritePrivateProfileString(strSection, strKey, strConnectionString, strFullIniFileName);
	if (dw)
	{
		CMNLog(_T("[CRegServicesThread::CreateServices]: New connection string is: %s"), (LPCTSTR)strConnectionString);
	}
	else
		CMNCheckWIN32Error(GetLastError());
}



void CDeliceOcrModuleDesc::WriteDebugFlag(bool bEnable)
{
	// connection string
	CString strFullIniFileName = GetFullIniFileName();	
	CString	strSection	= m_strDebugSection; // [Connection] for all and [Settings] for DeliceControlPanel
	CString	strKey		= _T("Debug");
	CString	strValue;

	strValue.Format(_T("%i"), (int)bEnable);

	DWORD dw;
	TCHAR szPrevDebugValue[MAX_PATH * 4]; // переменная - куда читаем	
	dw = ::GetPrivateProfileString(strSection, _T("Debug"), _T(""), szPrevDebugValue, MAX_PATH * 4, strFullIniFileName);
	if (dw)
	{
		//CMNLog(_T("[CRegServicesThread::CreateServices]: prev debug value: %s"), szPrevDebugValue);
	}
	dw = ::WritePrivateProfileString(strSection, _T("Debug"), strValue, strFullIniFileName);
	if (dw)
	{
		CMNLog(_T("[CRegServicesThread::CreateServices]: New debug value is: %s"), (LPCTSTR)strValue);
	}
	else
		CMNCheckWIN32Error(GetLastError());
}


// Service

CDeliceOcrServiceDesc::CDeliceOcrServiceDesc(LPCTSTR pszName, LPCTSTR pszDisplay, LPCTSTR pszDesc, LPCTSTR pszSubfolder, 
											LPCTSTR pszModuleFileName, LPCTSTR pszDefDatabase, LPCTSTR pszConnectSection, LPCTSTR pszDebugSection)
	: CDeliceOcrModuleDesc(pszName, pszDisplay, pszDesc, pszSubfolder, pszModuleFileName, pszDefDatabase, pszConnectSection, pszDebugSection)
{
	m_bIsService = true;	
}

CString	CDeliceOcrServiceDesc::GetFullModuleFileName()
{
	CString strServicesDir = SCMGetDefOCR_ServicesDirValue();
	CString strFullSvcFileName = strServicesDir + m_strSubfolder + _T("\\") + m_strModuleFileName;
	return strFullSvcFileName;
}

CString	CDeliceOcrServiceDesc::GetFullIniFileName()
{
	CString strServicesDir = SCMGetDefOCR_ServicesDirValue();
	CString strFullIniFileName = strServicesDir + m_strSubfolder + _T("\\") + m_strINIFileName;
	return strFullIniFileName;
}

// Application


CDeliceOcrAppDesc::CDeliceOcrAppDesc(LPCTSTR pszName, LPCTSTR pszDisplay, LPCTSTR pszDesc, LPCTSTR pszSubfolder, 
									 LPCTSTR pszModuleFileName, LPCTSTR pszDefDatabase, LPCTSTR pszConnectSection, LPCTSTR pszDebugSection)
	: CDeliceOcrModuleDesc(pszName, pszDisplay, pszDesc, pszSubfolder, pszModuleFileName, pszDefDatabase, pszConnectSection, pszDebugSection)
{
	m_bIsService = false;
}

CString	CDeliceOcrAppDesc::GetFullModuleFileName()
{
	CString strServicesDir = SCMGetDefOCR_AppsDirValue();
	CString strFullSvcFileName = strServicesDir + m_strSubfolder + _T("\\") + m_strModuleFileName;
	return strFullSvcFileName;
}

CString	CDeliceOcrAppDesc::GetFullIniFileName()
{
	CString strServicesDir = SCMGetDefOCR_AppsDirValue();
	CString strFullIniFileName = strServicesDir + m_strSubfolder + _T("\\") + m_strINIFileName;
	return strFullIniFileName;
}


