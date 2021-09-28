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



class CApplyScriptsThread : public WZRThread
{
//	DECLARE_DYNCREATE(CApplyScriptsThread)
protected:
	DBStorage				m_storage;
	//const CDBDescription::Vector&	m_vecDatabases;

	COcrConfigData&			m_rConfigData;
	CConfigXMLData&			m_rXmlConfigData;

	int* m_pnModificationFlags; //В этом классе ожидается установка флага IsCreateDB. 
								//См. Constants.h раздел Configurator Modification Flags

	
public:
	CApplyScriptsThread(HWND hParentWnd, /*const CDBDescription::Vector& rvecDatabases,*/ COcrConfigData& rConfigData, 
						CConfigXMLData& rXmlConfigData, int* pnModificationFlags );
	virtual ~CApplyScriptsThread();

public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();

    //bool IsDBExist();

protected:

	int		RunSqlScripts();
	void	SetupInitialValues();
    //BOOL	CreateDB(BOOL bDeleteFirst);	
	//CString GetServiceLogin(CString strServiceName);
	//CString GetCurrentUserName();

	//BOOL	GrantDBOwnerRole(CString& strError);
	//void	CheckIsOurDB(); // это наша база?
	// be sure that we have SQL Server 2017 with FullTextSerach Services installed and Database Mail configured and running
	//void	CheckSQLServerCompatibility(); 
	// be sure that we have four Integration Servcies (win32 servcie) installed and running
	//tuple<int, int> CheckIntegrationServices();
};


