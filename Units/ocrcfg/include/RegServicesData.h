//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:  Configurator      
//--------------------------------------------------------------------------
//  Description: Data for registering serivces
//--------------------------------------------------------------------------

#pragma once

#include "cmn/include/Unknown.h"
#include "db/include/Storage.h"



class CDeliceOcrModuleDesc : public CMNUnknown
{
public:
	typedef CMNSmartPtr<CDeliceOcrModuleDesc>	Ptr;
	typedef vector<Ptr>							Vector;
	
	CString			m_strName;
	CString			m_strDisplayName;
	CString			m_strDescription;
	CString			m_strSubfolder;			// for example DeliceCodesService2 or TechServices (relative from "C:\Program Files (x86)\DeliceInfo\DeliceOCR\Services\")
	CString			m_strModuleFileName;    // service executable file name
	CString			m_strINIFileName;		// setting.ini by default
	CString			m_strDefDatabase;		// DELICE_TECH_PROCESS by default
	CString			m_strConnectSection;	// section for connection string
	CString			m_strDebugSection;		// section for Debug flag [settings] by default

	bool			m_bCheck;
	bool			m_bIsService;

	CDeliceOcrModuleDesc(LPCTSTR pszName, LPCTSTR pszDisplayName, LPCTSTR pszDesc, LPCTSTR pszSubfolder, 
		                 LPCTSTR pszModuleFileName, LPCTSTR pszDefDatabase, LPCTSTR pszConnectSection, LPCTSTR pszDebugSection);
	
	virtual CString		GetFullModuleFileName();
	virtual CString		GetFullIniFileName();
	virtual CString		ConstructConnectionString(LPCTSTR pszSQLServerAddr);
	virtual void		WriteConnectionString(LPCTSTR pszSQLServerAddr);
	virtual void		WriteDebugFlag(bool bEnable);
};



class CDeliceOcrServiceDesc : public CDeliceOcrModuleDesc
{
public:
	CDeliceOcrServiceDesc(LPCTSTR pszName, LPCTSTR pszDisplayName, LPCTSTR pszDesc, LPCTSTR pszSubfolder, LPCTSTR pszServiceFileName, 
						  LPCTSTR pszDefDatabase = _T("DELICE_TECH_PROCESS"), LPCTSTR pszConnectSection = _T("Connection"), LPCTSTR pszDebugSection = _T("Settings"));

	virtual CString		GetFullModuleFileName();	
	virtual CString		GetFullIniFileName();
};


class CDeliceOcrAppDesc : public CDeliceOcrModuleDesc
{
public:
	CDeliceOcrAppDesc ( LPCTSTR pszName, LPCTSTR pszDisplayName, LPCTSTR pszDesc,
					    LPCTSTR pszSubfolder, LPCTSTR pszServiceFileName, LPCTSTR pszDefDatabase, LPCTSTR pszConnectSection, LPCTSTR pszDebugSection);

	virtual CString		GetFullModuleFileName();
	virtual CString		GetFullIniFileName();
};


