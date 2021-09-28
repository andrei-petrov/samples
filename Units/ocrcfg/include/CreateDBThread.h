//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:  Configurator      
//--------------------------------------------------------------------------
//  Description: Thread for background database creation 
//--------------------------------------------------------------------------

#pragma once

#include "cmn/include/Unknown.h"
#include "db/include/Storage.h"
#include "ocrcfg/include/CreateDBData.h"



class CCreateDBThread : public WZRThread
{
//	DECLARE_DYNCREATE(CCreateDBThread)
protected:
	DBStorage				m_storage;
	const CDBDescription::Vector&	m_vecDatabases;

	COcrConfigData&			m_rConfigData;
	CConfigXMLData&			m_config;

	int* m_pnModificationFlags; //В этом классе ожидается установка флага IsCreateDB. 
								//См. Constants.h раздел Configurator Modification Flags

	
public:
	CCreateDBThread(HWND hParentWnd, LPCTSTR pszSqlServerUserEnterName, 
		LPCTSTR pszSqlServerHostName, const CDBDescription::Vector& rvecDatabases, COcrConfigData& rConfigData, CConfigXMLData& config, int* pnModificationFlags );
	virtual ~CCreateDBThread();

public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();

    bool IsDBExist();

protected:
    BOOL	CreateDB(BOOL bDeleteFirst);
	BOOL	RemoveDB();
	int		RunSqlScripts();
	CString GetServiceLogin(CString strServiceName);
	CString GetCurrentUserName();

	BOOL	GrantDBOwnerRole(CString& strError);
	void	CheckIsOurDB(); // это наша база?
	// be sure that we have SQL Server 2017 with FullTextSerach Services installed and Database Mail configured and running
	void	CheckSQLServerCompatibility(); 
	// be sure that we have four Integration Servcies (win32 servcie) installed and running
	tuple<int, int> CheckIntegrationServices();
	tuple<int, int> StartSQLAgent();
};


