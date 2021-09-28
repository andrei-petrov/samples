//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Thread for background operations
//--------------------------------------------------------------------------

#pragma once

#include "db/include/Storage.h"


class CDeployRptThread: public WZRThread
{
//	DECLARE_DYNCREATE(CDeployRptThread)
    //friend class CDeployRptPage;
protected:
	DBStorage			m_storage;
	CString				m_strXMLBody;
	CString				m_strSQLServer;
	CString				m_strDbName;
	CString				m_strReportsServerURL; // user can write the name throught IP
	//CString				m_strReportsServerURLUserEnter; // user can write the name throught IP
	CString				m_strReportServerWMINamespace;
	CString				m_strReportsFolder;
	CString				m_strReportsDir;
	CString				m_strReportsDir2;
	CConfigXMLData::ReportInfo::Vector m_arrReportsInfo;

	CString				m_strReportsList;
	DWORD				m_dwReports;

	std::set<CString> m_setXMLBody;

	int* m_pnModificationFlags; //В этом классе ожидается установка флагов IsInstallReports. 
								//См. Constants.h раздел Configurator Modification Flags

public:
	CDeployRptThread(HWND hParentWnd, std::set<CString> m_setXMLBody, LPCTSTR pszSQLServer, LPCTSTR pszDbName,
		LPCTSTR pszReportsServerURL, LPCTSTR pszReportServerWMINamespace, 
		LPCTSTR pszReportsFolder, LPCTSTR pszReportsDir, LPCTSTR pszReportsDir2, 
		CConfigXMLData::ReportInfo::Vector arrReportsInfo, int* pnModificationFlags);
	virtual ~CDeployRptThread();

public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
	//static bool IsTemplateInstalled(LPCTSTR szTemplateName);
	CString CheckReportsExists();

protected:
    // TEMPLATES
//	BOOL m_bReinstall;
    void DeployReports();
	void DeleteReports();
	//void DeleteReport(IRptManPtr piMan, CString const& strReportName);
	void ResetReportServerSecurityLevel();

	//IRptManPtr GetRptMan();

//	DECLARE_MESSAGE_MAP()
};


