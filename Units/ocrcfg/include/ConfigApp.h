//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------

#pragma once

#include "T/include/WinApp.h"

#include "wzr/include/App.h"
#include "ocrcfg/include/ConfigXMLData.h"

struct ReportingServiceNamespace
{
	bstr_t bstrNamespace;
	std::set<bstr_t> lstServers;
};

typedef std::list<ReportingServiceNamespace> RS_NAMESPACES;


enum class CUseScenarioEnum
{
	eusCreateNew = 0,				// default behaviour, create new database, publish reports, import default library
	eusRemove    = 1,			// 1

//	eusExportImportSoftLibrary,	// 2
//	eusConfigureDCOM,			// 3

	eusApplyConfig = 2				// 2
};

enum CSoftLibActionEnum
{
	slpExport,	// 0
	slpImport	// 1
};


extern LPCTSTR g_lpszDefaultConfigFileName;


class CConfigApp : public WZRApp
{
    typedef	    WZRApp Base;

//    CComGITPtr<IRptMan> m_spRptManGIT;
	CConfigXMLData	m_data;
	vector<ATL::CAtlString> m_vecSQLServerName; // User can write IP adress as the host name

public:
	BOOL				m_bReportPackMode;
	CString				m_strLogDir; // log directory
    CString				m_strInspectionsDir; // inspections directory
	bool				m_bOSType;   // false for 32 bit XP and 32 bit Windows 2003
    CUseScenarioEnum	m_enScenario;
	/*
	static BOOL m_bConfigInstallerMode;
	static BOOL m_bConfigReportPackMode;
	static BOOL m_bImportExportOnlyMode;
	static BOOL m_bDefaultMode;
	*/
	CConfigApp();

// Overrides
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CWinApp)
	public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance(); // return app exit code
    //}}AFX_VIRTUAL
    HRESULT     InitLog();
    HRESULT     InitInspections();

	CConfigXMLData& GetConfigXMLData() { return m_data; }

	//IRptManPtr GetRptMan();

	//[d.titov] DO NOT REMOVE comments below
	//void ListLocalSqlServers(std::set<bstr_t>& lstServers);
	//void ListLocalReportServers(RS_NAMESPACES& namespaces_list);

	void SearchBaseReportServerNamespace(bstr_t bstrBaseNamespace, list<bstr_t>& namespacesList);

	ATL::CAtlString GetSQLServerHostName() const;
	ATL::CAtlString GetSQLServerUserEnterName() const;
	void SetSQLServerName(const ATL::CAtlString& rin_strSQLServerHostName, const ATL::CAtlString& rin_strSQLServerIPName);
	void ShowHelp(DWORD dwDlgResID);
	CString GetHelpPathForDlgResID(DWORD dwDlgResID);

// Implementation
	DECLARE_MESSAGE_MAP()
protected:
	// helpers
	LONG	OpenRootKey(void); // open or create root key 
	LONG	CloseRootKey(void);

	LONG	SetDefaultKeyValues(void); // write to registry all default values for IM
	LONG	SetDefStringValue(_bstr_t	bsValueName, _bstr_t	bsValue);

	LONG	QueryLogDirValue(_bstr_t&	bsLogDir);

	virtual void WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
		
	BOOL	GrantDebugPrivelegeToYourself();
};
