//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2010 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:		 SamCfg
//  Cooperation: 
//  Date:        2010-01-26
//--------------------------------------------------------------------------------
//  Description: Application
//--------------------------------------------------------------------------------

#pragma once


class SamCfgCommandLineInfo: public CCommandLineInfo
{
protected:
	bool	m_bInstall;         // run from MSI installer
	bool	m_bUnattended;      // run in unattended more
	bool	m_bDeploy;          // use .xml file specified
	bool	m_bReportPack;      // install report pack and nothing else
	bool	m_bDeployReports;   // deploy standart reports only, and nothing else (run from sammgr.exe main page)
	bool	m_bQueryParamList;  // show the message box which contains the list of possible launch parameters and flags
	CString	m_strConfigFilePath;

public:
	SamCfgCommandLineInfo();

public:
	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

	bool IsInstall()	{ return m_bInstall;	}
	bool IsUnattended()	{ return m_bUnattended;	}
	bool IsReportPack()	{ return m_bReportPack;	}
	bool IsDeployReports() { return m_bDeployReports; }
	bool IsQueryParamList() { return m_bQueryParamList; }
	CString GetConfigFilePath() { return m_strConfigFilePath; }
};
