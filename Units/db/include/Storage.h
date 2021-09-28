//--------------------------------------------------------------------
// Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------
// Unit:        db
// Cooperation: absent
//--------------------------------------------------------------------
// Description: Database incapsulation
//--------------------------------------------------------------------

#pragma once
#include "cmn/include/lockMap.h"
#include "db/include/db.h"
 
typedef	vector<UINT>	DBScriptIDVector;

// Параметр хранимой процедуры ===========================================================================
//Создан унификации способа передачи набора параметров в вызов хранимой процедуры.
//P.S. В отличии от этой структуры, параметр ADO не имеет своего конструктора, и для его создания используется объект Command, 
//который еще не доступен за перделами методов вызова ХП. 
struct DB_CLASS DBStorageProcParameter
{
	bstr_t m_bstrName;
	DataTypeEnum m_enType;
	ParameterDirectionEnum m_enDirection;
	long	m_lSize;
	variant_t* m_vtValue;

	//Конструктор
	DBStorageProcParameter(bstr_t bstrName, DataTypeEnum enType, ParameterDirectionEnum enDirection,	long lSize, variant_t* vtValue);
};

// ===================================================================================================
class DB_CLASS DBStorage
{
protected:
    CSAMLock*			m_pWriteLock;
	_ConnectionPtr		m_piWriteConnection;

	int					m_nVersion;
	CString				m_strDefDataPath;
	CString				m_strDefLogPath;
	CString				m_strCollation;
	int					m_nEngineEdition;
	int					m_nMajorVersion;
	int					m_nMinorVersion;
	int					m_nIntegratedSecOnly;
	
	   
	// C++ 11 initializers
	int					m_nFullTextStatus		= -1;	// -1 not detected, 0 - not installed, 1 -installed
	int					m_nDatabaseMailStatus	= -1;	// -1 not detected, 0 - not installed, 1 -installed
		
public:
	static DBScriptIDVector	m_vecScriptID;
	static void FillScriptIDVector();

	int			GetServerVersion()		{ return m_nVersion; }
	CString		GetDefDataPath()		{ return m_strDefDataPath; }
	CString		GetDefLogPath()			{ return m_strDefLogPath;  }
	CString		GetCollation()			{ return m_strCollation; }

	int			GetEngineEdition()		{ return m_nEngineEdition; }
	int			GetMajorVersion()		{ return m_nMajorVersion; }
	int			GetMinorVersion()		{ return m_nMinorVersion; }
	int			GetIntegratedSecOnly()	{ return m_nIntegratedSecOnly; }
	int			GetFullTextStatus()		{ return m_nFullTextStatus; }
	int			GetDatabaseMailStatus() { return m_nDatabaseMailStatus; }

	DBStorage();
    virtual ~DBStorage();
	void Connect(LPCTSTR pszConnectionString, LPCTSTR pszServer, LPCTSTR pszDatabase);
    
    // returns version for  SQL Server 2000 and up. 
    // 0  - version is too old, 8  - 2000  SQL Server 2000, 9  - 2005  SQL Server 2005, 10 - 2008 SQL Server 2008 
	// 11 - SQL Server 2012, 12 - SQL Server 2014, 13 - SQL Server 2016, 14 - SQL Server 2017, 15 - SQL Server 2019 

	void	LoadServerOptions();    
	void	LoadServerCapabilities();
	
    bstr_t	GetDefaultDatabase();
    long	ConnectionState();
	void	Disconnect();
 
	void	CreateDatabase(LPCTSTR pszDatabaseName);
    void	DeleteDatabase(LPCTSTR pszDatabaseName);
	int		IsDatabaseExist(LPCTSTR pszDatabaseName);

	set<CAtlString> ListObjects(LPCTSTR pszCommand);
	set<CAtlString> ListDatabases();
	set<CAtlString> ListStoredProc();
	set<CAtlString> ListTables();
	set<CAtlString> ListJobs();


	/* specific to IM, and should be removed */
	void ImportXMLFile(const bstr_t& bstrObjectGUID, const bstr_t& bstrFullName, const bool bWithAgent);
    //nInspectionType parameter: 0 - WithoutAgent, 1 - With Agent, 2 - manual import
    void ImportXMLBody(const bstr_t& bstrObjectGUID, const bstr_t& bstrXMLBody, const short sInspectionType);
	void AddFailedInspection(const bstr_t& bstrObjectGUID, const bstr_t& bstrErrorMessage, const short sInspectionType );
	void CreateObject(const bstr_t& bstrType, const bstr_t& bstrXMLBody);
    
    void CreateLicenseImage(const bstr_t& bstrGUID,       const bstr_t& bstrName, 
                            const bstr_t& bstrParentGuid, const bstr_t& bstrLicenseType, 
                            const bstr_t& bstrDesription, const CComVariant& varBinaryData);

	void DeleteObject(const bstr_t& bsGuid, const bstr_t& bsObjectType);
    void DeleteOldActions(const bstr_t& bstrActionsListXML);

    void InsertPicture(const CComVariant& varData, VARIANT_BOOL vbUserPicture = VARIANT_FALSE);
    void InsertPicture(const UINT nID, VARIANT_BOOL vbUserPicture = VARIANT_FALSE);
    void InsertDefaultPicture();
    CComVariant GetPicture();

	void MoveObject(const bstr_t& bsGuid, const bstr_t& bsNewParentGuid);

	_RecordsetPtr SearchObject(const bstr_t& bsObjectGuid, const bool bSubTree, const bstr_t& bsAttrNames, const bstr_t& bsFilter);
	_RecordsetPtr SearchWithProperties(const bstr_t& bsStartGUID, const bstr_t& bsChildType, const bstr_t& bsAttrNames, const bstr_t &bsFilter);

	_RecordsetPtr GetProperties(const bstr_t& bsGuid, const bstr_t& bsObjectType, const bstr_t& bsAttrNames);
	void PutProperties(const bstr_t& bsObjectGuid, const bstr_t& bsObjectType, const bstr_t& bsXMLBody );

    void GetReportParams(const bstr_t& bstrReportName, bstr_t& bstrDefaultVersion,  bstr_t& bstrParamList, bool& bReportTitle, bstr_t& bstrReportFriendlyName, bstr_t& bstrReportSubFolder);
    _RecordsetPtr GetRepCompWithSoftParams(const bstr_t& bstrObjectGUIDList, const bstr_t& bstrSubdivisionName);
    _RecordsetPtr GetReportParamsValues(const bstr_t& bsObjectGUID, const bstr_t& bsReportName);
    void GetLanguage(const bstr_t& bstrLocalization, bstr_t& bstrLanguage );
    int  GetDatabaseVersion();
    void SetDatabaseVersion(int nVersion);
    void SetRootName(const bstr_t& bstrRootName);
    
	bool IsComputerExist(const bstr_t& bstrComputerName);

    // Parameters:
    // szObjectGuid - guid of object to check
    // szRunningTasks - comma separeted list of running task's guids
    bool IsObjectLocked(LPCTSTR szObjectGuid, LPCTSTR szRunningTasks);

    void CreateComputerLink(const bstr_t& bstrTaskGuid, const bstr_t& bstrComputerGuid);

    long GetCurrentCompCount();
	long GetSuccessInspectedCompCount();

    int GetObjectCount(LPCTSTR szParentGuid, LPCTSTR szObjectType, const VARIANT_BOOL vbRecursive = VARIANT_FALSE);
    DATE GetCreationDate(LPCTSTR szDatabase);
	
	void CreateDBOwner(LPCTSTR	pszDatabaseName, LPCTSTR	pszLogin );
	void GrantReadPermissionsForGuest();

    _RecordsetPtr GetComputerSoft(const bstr_t& bstrComputerGuid);
    _RecordsetPtr GetComputerOS(const bstr_t& bstrComputerGuid);
    _RecordsetPtr GetComputerOSList(const bstr_t& bstrComputerGuid);
    _RecordsetPtr GetComputerHard(const bstr_t& bstrComputerGuid);
    _RecordsetPtr GetInspectedComputers();
    _RecordsetPtr GetReports();
	
	//Миграция баз данных

	// LibType: 1 - All soft, both initial and added, 0 - Only added soft
	// @InitChars, @EndChars - for ProductName
	bstr_t GetSoftLibrary(const short sLibType, const bstr_t& bstrInitChars, const bstr_t& bstrEndChars);
	void InsertSoftLibrary(const bstr_t& bstrLibraryXML);

    bstr_t RetrievePassword(const bstr_t& bstrTaskGuid, const bstr_t& bstrPassPhrase);
    void SavePassword(const bstr_t& bstrTaskGuid, const bstr_t& bstrPassPhrase, const bstr_t& bstrPassword );

    void ApplySynonyms();
	/* specific to IM, and should be removed */

	void   ExecuteProcedure(const bstr_t& bstrProcName, set<DBStorageProcParameter*>& pParameters);

	_RecordsetPtr	ExecuteCommand  (LPCTSTR pszCommand);
	void			ExecuteProcedure(LPCTSTR pszCommand);
	void			AddUser(LPCTSTR pszUserName, LPCTSTR pszLoginNT, bstr_t& bstrResult);

	void			RegisterNewReports(LPCTSTR lpszConfigXML);
};


// ===================================================================================================
class DB_CLASS DBPropertyHelper
{
private:
    MSXML::IXMLDOMElementPtr m_piElement;
public:
	DBPropertyHelper();
    virtual ~DBPropertyHelper();

    static bstr_t GetPropertyBSTR(CComVariant varPropertyValue);
    void Append(const bstr_t& bstrPropertyName, const bstr_t& bstrPropertyValue);
    bstr_t GetXML();
};


