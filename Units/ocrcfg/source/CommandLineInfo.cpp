//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2010 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:		 SamCfg
//  Cooperation: 
//  Date:        2010-01-26
//--------------------------------------------------------------------------------
//  Description: Application
//--------------------------------------------------------------------------------

#include "StdAfx.h"

#include "ocrcfg/include/CommandLineInfo.h"


SamCfgCommandLineInfo::SamCfgCommandLineInfo():
	m_bInstall(false),
	m_bUnattended(false),
	m_bDeploy(false),
	m_bReportPack(false),
	m_bDeployReports(false),
	m_bQueryParamList(false)
{
}

void SamCfgCommandLineInfo::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	if(bFlag)
	{
		TCHAR* pszBufParam = const_cast<TCHAR*>(pszParam);
		CString strParam(pszBufParam);
		if(strParam.CompareNoCase(_T("install")) == 0)
		{
			m_bInstall = true;
			m_bDeploy = false;
			m_bReportPack = false;
		}
		else if(strParam.CompareNoCase(_T("unattended")) == 0)
		{
			m_bUnattended = true;
			m_bDeploy = false;
			m_bReportPack = false;
		}
		else if (strParam.CompareNoCase(_T("reportpack")) == 0)
		{
			m_bReportPack = true;
			m_bDeploy = false;
			m_bUnattended = false;
		}
		else if(strParam.CompareNoCase(_T("deploy")) == 0)
		{
			m_bDeploy = true;
		}
		else if(strParam.CompareNoCase(_T("DeployReports")) == 0)
		{
			m_bDeployReports = true;
		}
		else if(strParam.CompareNoCase(_T("help")) == 0)
		{
			m_bQueryParamList = true;
		}
		else if(strParam.CompareNoCase(_T("?")) == 0)
		{
			m_bQueryParamList = true;
		}
	}
	else if(m_bDeploy)
	{
		m_strConfigFilePath = pszParam; // use specified .xml file with scenario
		m_bDeploy = false;
	}


	CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
}
