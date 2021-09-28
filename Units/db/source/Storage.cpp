//--------------------------------------------------------------------
// Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------
// Unit:        db
// Cooperation: absent
//--------------------------------------------------------------------
// Description: Database incapsulation
//--------------------------------------------------------------------

 
#include "stdafx.h"
#include "cmn/include/McException.h"
#include "cmn/include/Log.h"
#include "cmn/include/XMLUtilities.h"
#include "cmn/include/Misc.h"
#include "cmn/include/AuthorityGrantor.h"
#include "cmn/include/Sid.h"

#include "db/include/constants.h"
#include "db/include/Storage.h"
#include "db/include/ConnectionPool.h" 
#include "db/include/ConnectionHolder.h" 
#include "scm/include/SchemaDef.h" 

#pragma warning (disable: 4996)

DBScriptIDVector	DBStorage::m_vecScriptID;

// ============================================================================================================
DBStorageProcParameter::DBStorageProcParameter(bstr_t bstrName, DataTypeEnum enType, ParameterDirectionEnum enDirection, long lSize, variant_t* vtValue)
{
	m_bstrName=bstrName;
	m_enType=enType;
	m_enDirection=enDirection;
	m_vtValue=vtValue;
	m_lSize=lSize;
}

// ============================================================================================================
void DBStorage::FillScriptIDVector()
{
	if (m_vecScriptID.size() == 0)
	{
		m_vecScriptID.push_back(IDR_DB_CREATE_TABLES);
		m_vecScriptID.push_back(IDR_DB_INSERT_NEW_SOFT);
        m_vecScriptID.push_back(IDR_DB_INSERT_NEW_USBDEVICE);
        m_vecScriptID.push_back(IDR_DB_INSERT_NEW_INSPECTIONDATA);
        m_vecScriptID.push_back(IDR_DB_INSERT_NEW_HARDWARE);
        m_vecScriptID.push_back(IDR_DB_INSERT_NEW_FOUNDPRODUCTFILES);
        m_vecScriptID.push_back(IDR_DB_INSERT_NEW_FILESFOUND);
		m_vecScriptID.push_back(IDR_DB_SET_VERSION);
		m_vecScriptID.push_back(IDR_DB_SET_ROOTDN);
        m_vecScriptID.push_back(IDR_DB_SET_ROOTNAME);
		m_vecScriptID.push_back(IDR_DB_GET_REPORT_PARAMS);
		m_vecScriptID.push_back(IDR_DB_CREATE_PUBLISHER);
		m_vecScriptID.push_back(IDR_DB_CREATE_SOFTTYPE);
		m_vecScriptID.push_back(IDR_DB_CREATE_TASK);
		m_vecScriptID.push_back(IDR_DB_SEARCH_OBJECTS);
		m_vecScriptID.push_back(IDR_DB_GET_PROPERTIES);
		m_vecScriptID.push_back(IDR_DB_DELETE_OBJECT);
		m_vecScriptID.push_back(IDR_DB_SEARCH_CHILDREN);
		m_vecScriptID.push_back(IDR_DB_ADD_INITIAL_DATA);
		m_vecScriptID.push_back(IDR_DB_INSERT_FAILED_INSPECTION);
		m_vecScriptID.push_back(IDR_DB_ADD_INITIAL_OBJECTS);
		m_vecScriptID.push_back(IDR_DB_MOVE_OBJECT);
		m_vecScriptID.push_back(IDR_DB_GET_LANGUAGE);
        m_vecScriptID.push_back(IDR_DB_GET_OBJECTCOUNT);
		m_vecScriptID.push_back(IDR_DB_GET_FULL_VERSION);
		m_vecScriptID.push_back(IDR_DB_GET_VERSION);
		m_vecScriptID.push_back(IDR_DB_GET_ROOTDN);
		m_vecScriptID.push_back(IDR_DB_CREATE_COMP_TASK_LINK);
        m_vecScriptID.push_back(IDR_DB_GET_PASSPORTLIST);
		m_vecScriptID.push_back(IDR_DB_DELETE_OLD_ACTIONS);
		m_vecScriptID.push_back(IDR_DB_SET_USER_PICTURE);
        m_vecScriptID.push_back(IDR_DB_SP_GET_LANGUAGE);
        m_vecScriptID.push_back(IDR_DB_IS_COMPUTER_EXIST);
        m_vecScriptID.push_back(IDR_DB_IS_OBJECT_LOCKED);
        m_vecScriptID.push_back(IDR_DB_GET_REP_PARAM_VALUE);
        m_vecScriptID.push_back(IDR_DB_CREATE_PROHIBITION_RULE);
        m_vecScriptID.push_back(IDR_DB_CREATE_PASSPORT);
        m_vecScriptID.push_back(IDR_DB_CREATE_PASSPORT_LINK);
        m_vecScriptID.push_back(IDR_DB_CREATE_LICENSERULE_LINK);
        m_vecScriptID.push_back(IDR_DB_GET_SOFTFORCOMPUTER);
        m_vecScriptID.push_back(IDR_DB_GET_OSFORCOMPUTER);
        m_vecScriptID.push_back(IDR_DB_GET_HARDFORCOMPUTER);
        m_vecScriptID.push_back(IDR_DB_GET_INSPECTEDCOMPUTERS);
        m_vecScriptID.push_back(IDR_DB_GET_OSLISTFORCOMPUTER);
        m_vecScriptID.push_back(IDR_DB_GET_CORRECTEDUTCTIMETO);
        m_vecScriptID.push_back(IDR_DB_GET_HARDWARECHANGES);
        m_vecScriptID.push_back(IDR_DB_GET_ALLSOFTCHANGES);
        m_vecScriptID.push_back(IDR_DB_GET_ALLHARDCHANGES);
        m_vecScriptID.push_back(IDR_DB_GET_PRODUCTNAMEWITHOUTLANGUAGE);
        m_vecScriptID.push_back(IDR_DB_GET_PRODUCTNAMEGROUP);
        m_vecScriptID.push_back(IDR_DB_GET_PRODUCTNAMEFROMTEMPLATE);
        m_vecScriptID.push_back(IDR_DB_INSERT_NEW_REPORTS);
        m_vecScriptID.push_back(IDR_DB_GET_REPCOMPWITHSOFTPARAMS);
        m_vecScriptID.push_back(IDR_DB_GET_REPCOMPWITHHARDWAREPARAMS);
        m_vecScriptID.push_back(IDR_DB_CREATELICENSERULE);
        m_vecScriptID.push_back(IDR_DB_CREATEHARDWARELICENSERULE);
        m_vecScriptID.push_back(IDR_DB_SAVEPASSWORD);
        m_vecScriptID.push_back(IDR_DB_RETRIEVEPASSWORD);
        m_vecScriptID.push_back(IDR_DB_CREATE_HARDWARELICENSERULE_LINK);
        m_vecScriptID.push_back(IDR_DB_GET_PASSPORTNONQUALIFYCOMP);
        m_vecScriptID.push_back(IDR_DB_GET_DEVICELIST);
        m_vecScriptID.push_back(IDR_DB_GET_REPORTS);

        m_vecScriptID.push_back(IDR_DB_GET_WIDESPREADHARDWARE);
        m_vecScriptID.push_back(IDR_DB_GET_WIDESPREADSOFT);

        m_vecScriptID.push_back(IDR_DB_GET_CHARGUID);
        m_vecScriptID.push_back(IDR_DB_GET_SOFTPASSPORTCHECK);
        m_vecScriptID.push_back(IDR_DB_GET_HARDWAREPASSPORTCHECK);
        m_vecScriptID.push_back(IDR_DB_GET_EXTENDDEVICEDESC);
        m_vecScriptID.push_back(IDR_DB_GET_FREQUENCYFROMPROCESSORNAME);
        m_vecScriptID.push_back(IDR_DB_CREATE_CATEGORY);
        m_vecScriptID.push_back(IDR_DB_CREATE_LICENSE_IMAGE);
        m_vecScriptID.push_back(IDR_DB_CREATE_SYNONYM);
        m_vecScriptID.push_back(IDR_DB_CREATE_SUBDIVISION);
        //m_vecScriptID.push_back(IDR_DB_UPDATE_DBONNEWSYNONYMS);
        //m_vecScriptID.push_back(IDR_DB_GET_SOFTTYPEGUID);
        m_vecScriptID.push_back(IDR_DB_GET_SOFTLIST);
        m_vecScriptID.push_back(IDR_DB_CREATE_OBJECTRELATIONVIEWS);
        m_vecScriptID.push_back(IDR_DB_GET_LIBRARYXML);
        m_vecScriptID.push_back(IDR_DB_ADDLIBRARY);
        m_vecScriptID.push_back(IDR_DB_GET_DEFINITEINSPECTIONFILECHECK);
        m_vecScriptID.push_back(IDR_DB_GET_FILECHECK);
        m_vecScriptID.push_back(IDR_DB_GET_MAJORVERSIONFROMFULLVERSION);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWLASTCOMPUTERINSP);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWLASTSUCCESSCOMPUTERINSP);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWCOMPUTER);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWCOMPUTERPASSPORTS);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWHARDINSTALLATION);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWHARDWARELICENSERULE);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWINSTALLATION);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWFILEINSTALLATION);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWLICENSERULE);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWPASSPORTHARDWARELICENSERULES);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWPASSPORTLICENSERULES);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWPROHIBITIONRULE);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWS);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWSOFT);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWSOFTTYPE);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWSYNONYM);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWTASKCOMPUTERS);
        m_vecScriptID.push_back(IDR_DB_CREATE_LIBPRODUCTFILE);              
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWFOUNDPRODUCTFILE);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWLIBPRODUCTFILE);
        m_vecScriptID.push_back(IDR_DB_SET_CATEGORYPROPERTIES);
        m_vecScriptID.push_back(IDR_DB_SET_COMPUTERPROPERTIES);
        m_vecScriptID.push_back(IDR_DB_SET_HARDWARELICENSERULEPROPERTIES);
        m_vecScriptID.push_back(IDR_DB_SET_LICENSERULEPROPERTIES);
        m_vecScriptID.push_back(IDR_DB_SET_HARDWAREPROPERTIES);
        m_vecScriptID.push_back(IDR_DB_SET_OBJECTROOTPROPERTIES);
        m_vecScriptID.push_back(IDR_DB_SET_PASSPORTPROPERTIES);
        m_vecScriptID.push_back(IDR_DB_SET_PROHIBITIONRULEPROPERTIES);
        m_vecScriptID.push_back(IDR_DB_SET_PUBLISHERPROPERTIES);
        m_vecScriptID.push_back(IDR_DB_SET_SOFTPROPERTIES);
        m_vecScriptID.push_back(IDR_DB_SET_SOFTTYPEPROPERTIES);
        m_vecScriptID.push_back(IDR_DB_SET_SUBDIVISIONPROPERTIES);       
        m_vecScriptID.push_back(IDR_DB_SET_SYNONYMPROPERTIES);       
        m_vecScriptID.push_back(IDR_DB_SET_TASKPROPERTIES);
        m_vecScriptID.push_back(IDR_DB_SET_LIBPRODUCTFILEPROPERTIES);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWMAXFILESSEARCHCOMPUTERACTION);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWMAXPROGRAMSSEARCHCOMPUTERACTION);
        m_vecScriptID.push_back(IDR_DB_CREATE_VIEWMAXCOMPUTERACTION);
        //m_vecScriptID.push_back(IDR_DB_GET_COMPWITHSECURITY);
        //m_vecScriptID.push_back(IDR_DB_GET_COMPWITHDEFINITESECURITY);
        m_vecScriptID.push_back(IDR_DB_GET_SECURITYSOFTLIST);
        m_vecScriptID.push_back(IDR_DB_GET_DATAFROMOLDDB);
        m_vecScriptID.push_back(IDR_DB_GET_LIBRARYFROMOLDDB);
        m_vecScriptID.push_back(IDR_DB_GET_COMPUTERSFROMOLDDB);
        m_vecScriptID.push_back(IDR_DB_GET_TASKSFROMOLDDB);
        m_vecScriptID.push_back(IDR_DB_GET_RULESFROMOLDDB);
        m_vecScriptID.push_back(IDR_DB_GET_PASPORTSFROMOLDDB);
        m_vecScriptID.push_back(IDR_DB_GET_INSPECTIONSFROMOLDDB);
        m_vecScriptID.push_back(IDR_DB_CLEARDB);
        m_vecScriptID.push_back(IDR_DB_INSERT_CHIPSETLIBRARY);
        m_vecScriptID.push_back(IDR_DB_ADD_CHIPSETLIBRARY);
        
	}    
}

DBStorage::DBStorage()
{
    m_pWriteLock = new CSAMLock();
	FillScriptIDVector();
}
 
DBStorage::~DBStorage()
{
	if (ConnectionState() == 1)
		Disconnect();

    if (m_pWriteLock)
        delete m_pWriteLock;
}

void DBStorage::Connect(LPCTSTR pszConnectionString, LPCTSTR pszServer, LPCTSTR pszDatabase)
{
    CSAMScopeLock scopeLock( m_pWriteLock ); //sdf
	CString strConnect;
	strConnect.Format(pszConnectionString, pszServer, pszDatabase);
    CMNLog(_T("[DBStorage::Connect] %s"), strConnect); 

	try
    {
        HRESULT hr = m_piWriteConnection.CreateInstance(__uuidof(Connection));
        CMNCheckError(hr);
		hr = m_piWriteConnection->Open( (bstr_t)strConnect, L"", L"", ConnectOptionEnum::adConnectUnspecified);

        CMNCheckError(hr);
        DBConnectionPool* pConnectionPool = DBConnectionPool::Create();
        pConnectionPool->InitConnectionPool(pszConnectionString, pszServer, pszDatabase, 3);
    }
    catch (CMNException* pEx)
    {	    
		CMNException* pHiLevelEx = new CMNException(IDE_DB_CANNOT_CONNECT);
		*pHiLevelEx << pszServer;
		*pHiLevelEx << pszDatabase;
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);	
    }	
}

long DBStorage::ConnectionState()
{
//  CSAMScopeLock scopeLock( m_pWriteLock );    //sdf
    long lState = -1;
	if (m_piWriteConnection) 
	{
		lState = m_piWriteConnection->GetState();    
	}	
    CMNLog(_T("[DBStorage::ConnectionState]: connection state =  %u"), lState); 
    return lState;
}

bstr_t DBStorage::GetDefaultDatabase()
{
    bstr_t bstrDatabase;
    BEGIN_USE_CONNECTION(connectionFromPool) 
        bstrDatabase = connectionFromPool->GetDefaultDatabase();
    END_USE_CONNECTION
    return bstrDatabase;
}

void DBStorage::Disconnect()
{
    CMNLog(_T("[DBStorage::Disconnect]")); 
    CSAMScopeLock scopeLock( m_pWriteLock );
	if (m_piWriteConnection)    
	{
		m_piWriteConnection->Close();
	}	

    DBConnectionPool* pConnectionPool = DBConnectionPool::GetInstance();
    delete pConnectionPool;
}

// returns version for  SQL Server 2000 and up. 
// 0  - version is too old, 
// 8  - 2000  SQL Server 2000, 
// 9  - 2005  SQL Server 2005, 
// 10 - 2008 SQL Server 2008/2008R2 
// 11 - SQL Server 2012, 
// 12 - SQL Server 2014, 
// 13 - SQL Server 2016, 
// 14 - SQL Server 2017, 
// 15 - SQL Server 2019 


void DBStorage::LoadServerOptions()
{
    //int nVersion = 0;
	bstr_t bstrVersionDetails;
	bstr_t bstrQuery;

	try
    {
		_CommandPtr piCmd;
        _RecordsetPtr piRecords;

		bstr_t bstrQuery = _T("SELECT CAST(SERVERPROPERTY('productversion') AS VARCHAR), \
                          'SQL Server ' + \
                          CAST(SERVERPROPERTY('productversion') AS VARCHAR) + ' - ' + \
                          CAST(SERVERPROPERTY('productlevel') AS VARCHAR) + ' (' + \
                          CAST(SERVERPROPERTY('edition') AS VARCHAR) + ')'");


        BEGIN_USE_CONNECTION(connectionFromPool)
	        piCmd.CreateInstance(__uuidof(Command));
	        piCmd->CommandText = bstrQuery;
	        piCmd->CommandType = adCmdText; 

            piCmd->ActiveConnection = connectionFromPool; 
			piCmd->CommandTimeout = 0;						 //	no timeout
	        piRecords = piCmd->Execute(NULL, NULL, adCmdText);
        END_USE_CONNECTION

        FieldsPtr	piFields = piRecords->GetFields();
        FieldPtr	piField0 = piFields->GetItem(CComVariant(0));
        
        CString strVersion((LPCTSTR)(bstr_t)piField0->GetValue());
        int nStart = 0;
        strVersion = strVersion.Tokenize(_T("."), nStart);
        m_nVersion = _ttoi(strVersion);
		FieldPtr	piField1 = piFields->GetItem(CComVariant(1));
        bstrVersionDetails = (bstr_t)piField1->GetValue();
		

		bstrQuery = _T("SELECT	SERVERPROPERTY('InstanceDefaultDataPath'), SERVERPROPERTY('InstanceDefaultLogPath'), SERVERPROPERTY('Collation'), \
								SERVERPROPERTY('EngineEdition'), SERVERPROPERTY('ProductMajorVersion'), SERVERPROPERTY('ProductMinorVersion'), \
								SERVERPROPERTY('IsIntegratedSecurityOnly') ");
		BEGIN_USE_CONNECTION(connectionFromPool)
			piCmd.CreateInstance(__uuidof(Command));
			piCmd->CommandText = bstrQuery;
			piCmd->CommandType = adCmdText;
			piCmd->ActiveConnection = connectionFromPool;
			piCmd->CommandTimeout = 0;						 //	no timeout
			piRecords = piCmd->Execute(NULL, NULL, adCmdText);
		END_USE_CONNECTION

		piFields = piRecords->GetFields();
		FieldPtr	piFieldDataPath				= piFields->GetItem(CComVariant(0));
		FieldPtr	piFieldLogPath				= piFields->GetItem(CComVariant(1));
		FieldPtr	piFieldCollation			= piFields->GetItem(CComVariant(2));
		FieldPtr	piFieldEngineEdition		= piFields->GetItem(CComVariant(3));
		FieldPtr	piFieldMajorVersion			= piFields->GetItem(CComVariant(4));
		FieldPtr	piFieldMinorVersion			= piFields->GetItem(CComVariant(5));
		FieldPtr	piFieldIntegratedSecOnly    = piFields->GetItem(CComVariant(6));
		
		m_strDefDataPath	= piFieldDataPath->GetValue();
		m_strDefLogPath		= piFieldLogPath->GetValue();
		m_strCollation		= piFieldCollation->GetValue();

		m_nEngineEdition	= piFieldEngineEdition->GetValue();
		m_nMajorVersion		= piFieldMajorVersion->GetValue();
		m_nMinorVersion		= piFieldMinorVersion->GetValue();
		m_nIntegratedSecOnly= piFieldIntegratedSecOnly->GetValue();

	}
    catch (CMNException* pEx)
    {	
        CString strErrorMessages = CMNException::GetAllErrorMessages(pEx);
        CMNLog(_T("[DBStorage::LoafServerOptions] \r\n%s"), (LPCTSTR)strErrorMessages);        
	}    
}


void DBStorage::LoadServerCapabilities()
{
	bstr_t bstrQuery;
	try
	{
		_CommandPtr piCmd;
		_RecordsetPtr piRecords;
				
		bstrQuery = _T("SELECT SERVERPROPERTY('IsFullTextInstalled')");
		BEGIN_USE_CONNECTION(connectionFromPool)
			piCmd.CreateInstance(__uuidof(Command));
			piCmd->CommandText = bstrQuery;
			piCmd->CommandType = adCmdText;
			piCmd->ActiveConnection = connectionFromPool;
			piCmd->CommandTimeout = 0;						 //	no timeout
			piRecords = piCmd->Execute(NULL, NULL, adCmdText);
		END_USE_CONNECTION

		FieldsPtr	piFields = piRecords->GetFields();
		FieldPtr	piField0 = piFields->GetItem(CComVariant(0));

		m_nFullTextStatus	 = piField0->GetValue();
		piRecords.Release();
		
		
		//bstrQuery = _T("sp_configure 'show advanced options', 1; RECONFIGURE; sp_configure;");

		BEGIN_USE_CONNECTION(connectionFromPool)
			piCmd.CreateInstance(__uuidof(Command));

			bstrQuery = _T("sp_configure 'show advanced options', 1; ");
			piCmd->CommandText = bstrQuery;
			piCmd->CommandType = adCmdText;
			piCmd->ActiveConnection = connectionFromPool;
			piCmd->CommandTimeout = 0;						 //	no timeout
			piRecords = piCmd->Execute(NULL, NULL, adCmdText);
		
			bstrQuery = _T("reconfigure"); 
			piCmd->CommandText = bstrQuery;
			piRecords = piCmd->Execute(NULL, NULL, adCmdText);

			bstrQuery = _T("sp_configure");
			piCmd->CommandText = bstrQuery;
			piRecords = piCmd->Execute(NULL, NULL, adCmdText);		
		END_USE_CONNECTION

		// iterate trought result rows
		HRESULT bNextRow;
		do
		{
			FieldsPtr	piFields = piRecords->GetFields();
			FieldPtr	piField0 = piFields->GetItem(CComVariant(0));
			FieldPtr	piFieldConfigured;
			FieldPtr	piFieldRun;
			CString		strName = piField0->GetValue();			
			if (strName.CompareNoCase(L"Database Mail XPs") == 0)
			{
				piFieldConfigured = piFields->GetItem(CComVariant(3)); // config_value
				piFieldRun        = piFields->GetItem(CComVariant(4)); // run_value
				
				m_nDatabaseMailStatus = (piFieldConfigured->GetValue() && piFieldRun->GetValue());
			}			
			bNextRow = piRecords->MoveNext();
		}		
		while (bNextRow == S_OK && piRecords->GetadoEOF() == VARIANT_FALSE);
	}

	catch (CMNException* pEx)
	{
		CString strErrorMessages = CMNException::GetAllErrorMessages(pEx);
		CMNLog(_T("[DBStorage::LoadServerCapabilities] \r\n%s"), (LPCTSTR)strErrorMessages);		
	}
	//return std::make_tuple(nFullTextInstalled, nDatabaseMailInstalled);
}


void DBStorage::CreateDatabase(LPCTSTR pszDatabaseName)
{
    CString str;

    //str.Format(_T("if db_id('%s') is not null  begin alter database %s set single_user with rollback immediate; drop database %s; end"), pszDatabaseName, pszDatabaseName, pszDatabaseName);
	//ExecuteCommand( str );
	
	str.Format(_T("create database [%s] collate Cyrillic_General_CI_AS"), pszDatabaseName);
	ExecuteCommand(str);
	
	str.Format(_T("use [%s]"), pszDatabaseName);
	ExecuteCommand(str);

    str.Format(_T("ALTER DATABASE [%s] SET RECOVERY SIMPLE "), pszDatabaseName);
	ExecuteCommand(str);

    str.Format(_T("ALTER DATABASE [%s] MODIFY FILE  (NAME = [%s], FILEGROWTH = 10%%) "), pszDatabaseName, pszDatabaseName);
	ExecuteCommand(str);

    //printf("Data Base create successfully\n");
}

void DBStorage::DeleteDatabase(LPCTSTR pszDatabaseName)
{
    CString str;

    str.Format(_T("if db_id('%s') is not null  begin alter database [%s] set single_user with rollback immediate; drop database [%s]; end"), pszDatabaseName, pszDatabaseName, pszDatabaseName);
	ExecuteCommand( str );

}

int DBStorage::IsDatabaseExist(LPCTSTR pszDatabaseName)
{
	int		iRetVal = 0;
    CString strCommand;
	strCommand.Format(_T("select count(db_id('%s'))"), pszDatabaseName);

	try
    {
		_CommandPtr piCmd;
        _RecordsetPtr piRec;

        BEGIN_USE_CONNECTION(connectionFromPool) 
	        piCmd.CreateInstance(__uuidof(Command));
	        piCmd->CommandText = (bstr_t)strCommand;
	        piCmd->CommandType = adCmdText; 

            piCmd->ActiveConnection = connectionFromPool; 

			piCmd->CommandTimeout = 0;						 //	no timeout
	        piRec = piCmd->Execute(NULL, NULL, adCmdText);
        END_USE_CONNECTION

		if(piRec)
        {
			FieldsPtr piFields;
            CComVariant vType;
            while(piRec->adoEOF == VARIANT_FALSE)
            {
                piFields = piRec->GetFields();
                vType = piFields->GetItem(CComVariant(0))->Value;
				iRetVal = (vType.intVal);	
                piRec->MoveNext();
            }                     
		 }
	}
    catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_COMMAND_FAILED);
		*pHiLevelEx << (LPCTSTR)strCommand;
		*pHiLevelEx << strCommand.GetLength();
		*pHiLevelEx	+= *pEx;

		ThrowException(pHiLevelEx);
	}
	return iRetVal;
}


set<CAtlString> DBStorage::ListObjects(LPCTSTR pszCommand)
{	
	CString strCommand = pszCommand;
	set<CAtlString>	setResult;

	try
	{
		_CommandPtr		piCmd;
		_RecordsetPtr	piRecords;
		BEGIN_USE_CONNECTION(connectionFromPool)
			piCmd.CreateInstance(__uuidof(Command));
			piCmd->CommandText		= (const TCHAR*)strCommand;
			piCmd->CommandType		= adCmdText;
			piCmd->ActiveConnection = connectionFromPool;
			piCmd->CommandTimeout	= 0;						 //	no timeout
			piRecords = piCmd->Execute(NULL, NULL, adCmdText);
		END_USE_CONNECTION

		if (piRecords)
		{
			piRecords->MoveFirst();
			while (!piRecords->adoEOF)
			{
				FieldPtr piField = piRecords->Fields->Item[_T("name")];
				CAtlString strDbName(piField->GetValue());
				setResult.insert(strDbName);
				piRecords->MoveNext();
			}
		}
	}
	catch (CMNException* pEx)
	{
		CMNException* pHiLevelEx = new CMNException(IDE_DB_COMMAND_FAILED);
		*pHiLevelEx << (LPCTSTR)strCommand;
		*pHiLevelEx << strCommand.GetLength();
		*pHiLevelEx += *pEx;
		ThrowException(pHiLevelEx);
	}
	return setResult;
}


set<CAtlString> DBStorage::ListDatabases()
{    
	return ListObjects(_T("SELECT name from sys.databases WHERE name NOT IN ('master', 'model', 'msdb', 'tempdb')"));    
}

set<CAtlString> DBStorage::ListStoredProc()
{
	return ListObjects(_T("SELECT  name FROM sys.objects WHERE type='P' AND name NOT LIKE 'sys.%'"));
}

set<CAtlString> DBStorage::ListTables()
{
	return ListObjects(_T("SELECT name FROM sys.tables"));
}

// you should connent to msdb before calling this methos
set<CAtlString> DBStorage::ListJobs()
{
	return ListObjects(_T("exec sp_help_job"));
}


void DBStorage::ImportXMLFile(const bstr_t& bstrObjectGUID, const bstr_t& bstrFullName, const bool bWithAgent)
{
	HRESULT hr = S_OK;
    try
    {
        bstr_t bstrXMLBody = XMLUtilities::ReadXMLFile(bstrFullName);
		ImportXMLBody(bstrObjectGUID, bstrXMLBody, bWithAgent ? 1 : 0);

		CMNLog(_T("[DBStorage::ImportXMLFile]: '%s'"), (LPCTSTR)bstrFullName); 
	}
    catch (CMNException* pEx)
    {	
        CString strExceptionError = CMNException::GetAllErrorMessages(pEx);
        CMNLog(_T("[DBStorage::ImportXMLFile] Exception occurs. %s"), (LPCTSTR)strExceptionError);
		CMNException* pHiLevelEx = new CMNException(IDE_DB_XML_IMPORT_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
}

//sInspectionType: 0 - WithoutAgent, 1 - With Agent, 2 - import xml data from network folder
void DBStorage::ImportXMLBody(const bstr_t& bstrObjectGUID, const bstr_t& bstrXMLBody, const short sInspectionType )
{
    CSAMScopeLock scopeLock( m_pWriteLock );
	ParametersPtr piParams;
	_CommandPtr piCmd;

	CMNLog(_T("[DBStorage::ImportXMLBody]: Started for Object with '%s' GUID"), (LPCTSTR)bstrObjectGUID); 
	DWORD dwStart = ::GetTickCount();
	m_piWriteConnection->BeginTrans();

	try
	{
		piCmd.CreateInstance(__uuidof(Command));
		piCmd->CommandText = "dbo.InsertInspectionData";
		piCmd->CommandType = adCmdStoredProc;  

		piParams = piCmd->Parameters;

		piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
		piParams->Append( piCmd->CreateParameter("@ObjectGUID", adLongVarWChar, adParamInput, bstrObjectGUID.length() + 1, (BSTR)bstrObjectGUID ));
		piParams->Append( piCmd->CreateParameter("@XMLSoftList", adLongVarWChar, adParamInput, bstrXMLBody.length() + 1, (variant_t)bstrXMLBody) );
		piParams->Append( piCmd->CreateParameter("@InspectionType", adSmallInt, adParamInput, sizeof(SHORT), (variant_t)sInspectionType) );

	    piCmd->ActiveConnection = m_piWriteConnection;
		piCmd->CommandTimeout = 0;
	    piCmd->Execute(NULL, NULL, adCmdStoredProc);
	}
	catch(CMNException* pEx)
	{
		CString strExceptionError = CMNException::GetAllErrorMessages(pEx);
		CMNLog(_T("[DBStorage::ImportXMLBody]: Exception occurs: 's'. Rollback transaction..."), (LPCTSTR)strExceptionError); 
		m_piWriteConnection->RollbackTrans();
		pEx->Throw();
	}

	m_piWriteConnection->CommitTrans();
	_ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];

	CMNLog(_T("[DBStorage::ImportXMLBody]: result = '%i', time %d ms"),
		piPtr->Value.iVal,
		::GetTickCount() - dwStart);
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}		
}

//sInspectionType: 0 - WithoutAgent, 1 - With Agent, 2 - import xml data from network folder
void DBStorage::AddFailedInspection(const bstr_t& bstrObjectGUID, const bstr_t& bstrErrorMessage, const short sInspectionType )
{
    CSAMScopeLock scopeLock( m_pWriteLock );
    try
    {
		CMNLog(_T("[DBStorage::AddFailedInspection]: '%s'"), (LPCTSTR)bstrErrorMessage); 
		_CommandPtr piCmd;
        ParametersPtr piParams;

        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.InsertFailedInspection";
        piCmd->CommandType = adCmdStoredProc;  

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue",	 adSmallInt,	 adParamReturnValue, sizeof(SHORT), NULL) );
	    piParams->Append( piCmd->CreateParameter("@ObjectGUID",		 adLongVarWChar, adParamInput,	bstrObjectGUID.length() + 1, (BSTR)bstrObjectGUID ));
	    piParams->Append( piCmd->CreateParameter("@InspectionError",   adLongVarWChar, adParamInput,	bstrErrorMessage.length() + 1, (BSTR)bstrErrorMessage ));
	
	    piParams->Append( piCmd->CreateParameter("@InspectionType", adSmallInt, adParamInput, sizeof(SHORT), (variant_t)sInspectionType) );

        piCmd->ActiveConnection = m_piWriteConnection;
		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);

	    _ParameterPtr piPtr;
		piPtr = piParams->Item["@ReturnValue"];
	    
		CMNLog(_T("[DBStorage::AddFailedInspection]: result = '%i'"), piPtr->Value.iVal); 
		if (piPtr->Value.iVal != 0)
		{
			CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
			*pEx << (LPCTSTR)piCmd->CommandText;
			*pEx << piPtr->Value.iVal;
			ThrowException(pEx);
		}		
    }
    catch (CMNException* pEx)
    {
		CMNException* pHiLevelEx = new CMNException(IDE_DB_FAILINSPECTINFO_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
}

_RecordsetPtr DBStorage::SearchObject(const bstr_t& bsObjectGuid, const bool bSubTree, const bstr_t& bsAttrNames, const bstr_t& bsFilter)
{
	HRESULT hr = S_OK;

	// create recordset
	_RecordsetPtr piRowsPtr = NULL;
	piRowsPtr.CreateInstance(__uuidof(Recordset));
	piRowsPtr->CursorLocation = adUseClient;  
	piRowsPtr->CursorType = adOpenStatic;  

    //create Command
	_CommandPtr piCmd;
    ParametersPtr piParams;

	BEGIN_USE_CONNECTION(connectionFromPool)
		piCmd.CreateInstance(__uuidof(Command));
		piCmd->CommandText = "dbo.SearchObjects";
		piCmd->CommandType = adCmdStoredProc;  
		piCmd->ActiveConnection = connectionFromPool;

		piParams = piCmd->Parameters;
		piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
		piParams->Append( piCmd->CreateParameter("@ObjectGUID", adLongVarWChar, adParamInput, bsObjectGuid.length() + 1, (BSTR)bsObjectGuid ));
		piParams->Append( piCmd->CreateParameter("@SubTree", adBoolean, adParamInput,  sizeof(BOOL), bSubTree ));
		piParams->Append( piCmd->CreateParameter("@AttrNames", adLongVarWChar, adParamInput, bsAttrNames.length() + 1, (BSTR)bsAttrNames ));
		piParams->Append( piCmd->CreateParameter("@Filter", adLongVarWChar, adParamInput, bsFilter.length() + 1, (BSTR)bsFilter ));

		piCmd->CommandTimeout = 0;						 //	no timeout
		piRowsPtr->Open( _variant_t((IDispatch *)piCmd, true), 
								vtMissing, 
								adOpenForwardOnly, 
								adLockReadOnly, 
								-1 );
	END_USE_CONNECTION

	variant_t vtNull; 
	vtNull.vt = VT_NULL;
	piRowsPtr->put_ActiveConnection(vtNull);

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];

	//CMNLog(_T("[DBStorage::SearchObjects] Object GUID: '%s'. Вернуть аттрибуты '%s' с фильтром '%s'"), (LPCTSTR)bsObjectGuid, (LPCTSTR)bsAttrNames, (LPCTSTR)bsFilter ); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	

	if(!piRowsPtr)
		return NULL;

	if(piRowsPtr->State == adStateClosed)
		return NULL;

	return piRowsPtr;
}

_RecordsetPtr DBStorage::SearchWithProperties(const bstr_t& bsStartGUID, const bstr_t& bsChildType, const bstr_t& bsAttrNames, const bstr_t &bsFilter)
{
	HRESULT hr = S_OK;

	// create recordset
	_RecordsetPtr piRowsPtr = NULL;
	piRowsPtr.CreateInstance(__uuidof(Recordset));
	piRowsPtr->CursorLocation = adUseClient;  
	piRowsPtr->CursorType = adOpenStatic; 

	//create Command
	_CommandPtr piCmd;
    ParametersPtr piParams;
    
	BEGIN_USE_CONNECTION(connectionFromPool)
		piCmd.CreateInstance(__uuidof(Command));
		piCmd->CommandText = "dbo.SearchChildren";
		piCmd->CommandType = adCmdStoredProc;  
		piCmd->ActiveConnection = connectionFromPool;

		piParams = piCmd->Parameters;
		piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
		piParams->Append( piCmd->CreateParameter("@ObjectGUID", adLongVarWChar, adParamInput, bsStartGUID.length() + 1, (BSTR)bsStartGUID ));
		piParams->Append( piCmd->CreateParameter("@ObjectType", adLongVarWChar, adParamInput, bsChildType.length() + 1, (BSTR)bsChildType ));
		piParams->Append( piCmd->CreateParameter("@AttrNames", adLongVarWChar, adParamInput, bsAttrNames.length() + 1, (BSTR)bsAttrNames ));
		piParams->Append( piCmd->CreateParameter("@Filter", adLongVarWChar, adParamInput, bsFilter.length() + 1, (BSTR)bsFilter ));

		piCmd->CommandTimeout = 0;						 //	no timeout
		piRowsPtr->Open( _variant_t((IDispatch *)piCmd, true), 
								vtMissing, 
								adOpenForwardOnly, 
								adLockReadOnly, 
								-1 );
	END_USE_CONNECTION

	variant_t vtNull; 
	vtNull.vt = VT_NULL;
	piRowsPtr->put_ActiveConnection(vtNull);
    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];
    
	CMNLog(_T("[DBStorage::SearchWithProperties] Object GUID: '%s'. Вернуть аттрибуты '%s' для объектов типа '%s' с фильтром '%s'"), (LPCTSTR)bsStartGUID, (LPCTSTR)bsAttrNames, (LPCTSTR)bsChildType, (LPCTSTR)bsFilter); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	

	if(!piRowsPtr)
		return NULL;

	if(piRowsPtr->State == adStateClosed)
		return NULL;

	return piRowsPtr;
}

_RecordsetPtr DBStorage::GetProperties(const bstr_t& bsGuid, const bstr_t& bsObjectType, const bstr_t& bsAttrNames)
{
	HRESULT hr = S_OK;

	// create recordset
	_RecordsetPtr piRowsPtr = NULL;
	piRowsPtr.CreateInstance(__uuidof(Recordset));
	piRowsPtr->CursorLocation = adUseClient;  
	piRowsPtr->CursorType = adOpenStatic;

    //create Command
	_CommandPtr piCmd;
    ParametersPtr piParams;

    BEGIN_USE_CONNECTION(connectionFromPool) 
        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.GetProperties";
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = connectionFromPool; 

        CComBSTR bstrGUID2 = (BSTR) bsGuid;
        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
	    piParams->Append( piCmd->CreateParameter("@AttrNames", adLongVarWChar, adParamInput, bsAttrNames.length() + 1, (BSTR)bsAttrNames ));
	    piParams->Append( piCmd->CreateParameter("@ObjectGUID", adLongVarWChar, adParamInput, bsGuid.length() + 1, (BSTR)bsGuid ));
        piParams->Append( piCmd->CreateParameter("@ObjectType", adLongVarWChar, adParamInput, bsObjectType.length() + 1, (BSTR)bsObjectType ));

		piCmd->CommandTimeout = 0;						 //	no timeout
	    piRowsPtr->Open( _variant_t((IDispatch *)piCmd, true), 
								vtMissing, 
								adOpenForwardOnly, 
								adLockReadOnly, 
								-1 );

    END_USE_CONNECTION  

	variant_t vtNull; 
	vtNull.vt = VT_NULL;
	piRowsPtr->put_ActiveConnection(vtNull);

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];

    CMNLog(_T("[DBStorage::GetProperties] Object GUID: '%s', Attributes: '%s'"), (LPCTSTR)bsGuid, (LPCTSTR)bsAttrNames); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	

	if(!piRowsPtr)
		return NULL;

	if(piRowsPtr->State == adStateClosed)
		return NULL;

	return piRowsPtr;
}

void DBStorage::CreateObject(const bstr_t& bstrType, const bstr_t& bstrXMLBody)
{
    CMNLog(_T("[DBStorage::CreateObject] Started, type '%s'"), (const TCHAR*)bstrType); 
    CSAMScopeLock scopeLock( m_pWriteLock );    
	HRESULT hr = S_OK;

    _CommandPtr piCmd;
    ParametersPtr piParams;

    piCmd.CreateInstance(__uuidof(Command));
    piCmd->CommandText = "dbo.Create" + bstrType;
    piCmd->CommandType = adCmdStoredProc;  

    piParams = piCmd->Parameters;
    piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
    piParams->Append( piCmd->CreateParameter("@XMLObject", adLongVarWChar, adParamInput, bstrXMLBody.length() + 1, (variant_t)bstrXMLBody) );

    piCmd->ActiveConnection = m_piWriteConnection;
	piCmd->CommandTimeout = 0;						 //	no timeout
    piCmd->Execute(NULL, NULL, adCmdStoredProc);

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];
    
	CMNLog(_T("[DBStorage::CreateObject]: result = '%i'"), piPtr->Value.iVal); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}		
     CMNLog(_T("[DBStorage::CreateObject] Finished")); 
}

void DBStorage::CreateLicenseImage( const bstr_t& bstrGUID,       const bstr_t& bstrName, 
                                    const bstr_t& bstrParentGuid, const bstr_t& bstrLicenseType, 
                                    const bstr_t& bstrDesription, const CComVariant& varBinaryData)
{
    CMNLog(_T("[DBStorage::CreateLicenseImage] Started")); 
    CSAMScopeLock scopeLock( m_pWriteLock );    
    try
    {
        _CommandPtr piCmd;
        ParametersPtr piParams;

        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.CreateLicenseImage";
        piCmd->CommandType = adCmdStoredProc;  

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );

        piParams->Append( piCmd->CreateParameter("@ObjectGUID", adLongVarWChar, adParamInput, bstrGUID.length() + 1, (variant_t)bstrGUID) );
        piParams->Append( piCmd->CreateParameter("@ParentGUID", adLongVarWChar, adParamInput, bstrParentGuid.length() + 1, (variant_t)bstrParentGuid) );
        piParams->Append( piCmd->CreateParameter("@Name", adLongVarWChar, adParamInput, bstrName.length() + 1, (variant_t)bstrName) );
        piParams->Append( piCmd->CreateParameter("@LicenseType", adLongVarWChar, adParamInput, bstrLicenseType.length() + 1, (variant_t)bstrLicenseType) );
        piParams->Append( piCmd->CreateParameter("@Description", adLongVarWChar, adParamInput, bstrDesription.length() + 1, (variant_t)bstrDesription) );
        piParams->Append( piCmd->CreateParameter("@BinaryData", adLongVarBinary, adParamInput, varBinaryData.GetSize(), varBinaryData ));

        piCmd->ActiveConnection = m_piWriteConnection;
		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);

        _ParameterPtr piPtr;
	    piPtr = piParams->Item["@ReturnValue"];

	    CMNLog(_T("[DBStorage::CreateLicenseImage]: result = '%i'"), piPtr->Value.iVal); 
	    if (piPtr->Value.iVal != 0)
	    {
		    CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		    *pEx << (LPCTSTR)piCmd->CommandText;
		    *pEx << piPtr->Value.iVal;
		    ThrowException(pEx);
	    }		
    }
    catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_CREATELICENSEIMAGE_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
    CMNLog(_T("[DBStorage::CreateLicenseImage] Finished")); 
}

void DBStorage::DeleteObject(const bstr_t& bsGuid, const bstr_t& bsObjectType)
{
    CSAMScopeLock scopeLock( m_pWriteLock );    
	_CommandPtr piCmd;
    ParametersPtr piParams;

    piCmd.CreateInstance(__uuidof(Command));
    piCmd->CommandText = "dbo.DeleteObject";
    piCmd->CommandType = adCmdStoredProc;  

    piParams = piCmd->Parameters;
    piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
    piParams->Append( piCmd->CreateParameter("@ObjectGUID", adLongVarWChar, adParamInput, bsGuid.length() + 1, (BSTR)bsGuid ));
    piParams->Append( piCmd->CreateParameter("@ObjectType", adLongVarWChar, adParamInput, bsObjectType.length() + 1, (BSTR)bsObjectType ));

    piCmd->ActiveConnection = m_piWriteConnection; 
	piCmd->CommandTimeout = 0;						 //	no timeout
    piCmd->Execute(NULL, NULL, adCmdStoredProc);

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];
    
	CMNLog(_T("[DBStorage::DeleteObject]: result = '%i'"), piPtr->Value.iVal); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}		
}

void DBStorage::DeleteOldActions(const bstr_t& bstrActionsListXML)
{
    CSAMScopeLock scopeLock( m_pWriteLock );    
	_CommandPtr piCmd;
    ParametersPtr piParams;

	CMNLog(_T("[DBStorage::DeleteOldActions]: Begin delete old actions"));
//	m_piWriteConnection->BeginTrans();

	try
	{
		piCmd.CreateInstance(__uuidof(Command));
		piCmd->CommandText = "dbo.DeleteOldActions";
		piCmd->CommandType = adCmdStoredProc;  

		piParams = piCmd->Parameters;

		piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
		piParams->Append( piCmd->CreateParameter("@@XMLObject", adLongVarWChar, adParamInput, bstrActionsListXML.length()+1, bstrActionsListXML));

		piCmd->ActiveConnection = m_piWriteConnection; 
		piCmd->CommandTimeout = 0;						 //	no timeout
		piCmd->Execute(NULL, NULL, adCmdStoredProc);
	}
	catch(CMNException* pEx)
	{
		CString strExceptionError = CMNException::GetAllErrorMessages(pEx);
		CMNLog(_T("[DBStorage::DeleteOldActions]: Exception occurs: '%s'. Rollback transaction..."), (LPCTSTR)strExceptionError); 
//		m_piWriteConnection->RollbackTrans();
		pEx->Throw();
	}

//	m_piWriteConnection->CommitTrans();
    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];
    
	CMNLog(_T("[DBStorage::DeleteOldActions]: result = '%i'"), piPtr->Value.iVal); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}

	ExecuteCommand( _T("CHECKPOINT") );
}

void DBStorage::MoveObject(const bstr_t& bsGuid, const bstr_t& bsNewParentGuid)
{
    CSAMScopeLock scopeLock( m_pWriteLock );    
	_CommandPtr piCmd;
    ParametersPtr piParams;

    piCmd.CreateInstance(__uuidof(Command));
    piCmd->CommandText = "dbo.MoveObject";
    piCmd->CommandType = adCmdStoredProc;  
    
    piParams = piCmd->Parameters;
    piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
    piParams->Append( piCmd->CreateParameter("@ObjectGUID", adLongVarWChar, adParamInput, bsGuid.length() + 1, (BSTR)bsGuid ));
    piParams->Append( piCmd->CreateParameter("@NewParentGuid", adLongVarWChar, adParamInput, bsNewParentGuid.length() + 1, (BSTR)bsNewParentGuid ));

    piCmd->ActiveConnection = m_piWriteConnection; 
	piCmd->CommandTimeout = 0;						 //	no timeout
    piCmd->Execute(NULL, NULL, adCmdStoredProc);

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];
    
	CMNLog(_T("[DBStorage::MoveObject]: result = '%i'"), piPtr->Value.iVal); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}		
}

void DBStorage::PutProperties(const bstr_t& bsObjectGuid, const bstr_t& bsObjectType, const bstr_t& bsXMLBody )
{
    CSAMScopeLock scopeLock( m_pWriteLock );    
	HRESULT hr = S_OK;
	_CommandPtr piCmd;
    ParametersPtr piParams;

    piCmd.CreateInstance(__uuidof(Command));
    piCmd->CommandText = "dbo.Set" + bsObjectType  + "Properties";
    piCmd->CommandType = adCmdStoredProc;  

    piParams = piCmd->Parameters;
    piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
    piParams->Append( piCmd->CreateParameter("@ObjectGUID", adLongVarWChar, adParamInput, bsObjectGuid.length() + 1, (variant_t)bsObjectGuid) );
    piParams->Append( piCmd->CreateParameter("@XMLObject", adLongVarWChar, adParamInput, bsXMLBody.length() + 1, (variant_t)bsXMLBody) );

    piCmd->ActiveConnection = m_piWriteConnection; 
	piCmd->CommandTimeout = 0;						 //	no timeout
    piCmd->Execute(NULL, NULL, adCmdStoredProc);

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];
    
	//CMNLog(_T("[DBStorage::PutProperties]: result = '%i'"), piPtr->Value.iVal); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}		
}

int DBStorage::GetDatabaseVersion()
{
	HRESULT hr = S_OK;
	_CommandPtr piCmd;
    ParametersPtr piParams;

	BEGIN_USE_CONNECTION(connectionFromPool)
        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.GetVersion";
        piCmd->CommandType = adCmdStoredProc;  
		piCmd->ActiveConnection = connectionFromPool; 

        piParams = piCmd->Parameters;

        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
        piParams->Append( piCmd->CreateParameter("@Version", adSmallInt, adParamOutput, sizeof(SHORT) ));

		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);
	END_USE_CONNECTION
    
    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];
	CMNLog(_T("[DBStorage::GetDatabaseVersion]: result = '%i'"), piPtr->Value.iVal); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	
    return (int)piCmd->Parameters->Item["@Version"]->Value;
}

void DBStorage::SetDatabaseVersion(int nVersion)
{
    CSAMScopeLock scopeLock( m_pWriteLock );   
	HRESULT hr = S_OK;
    try
    {
		_CommandPtr piCmd;
        ParametersPtr piParams;

        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.SetVersion";
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = m_piWriteConnection;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
	    piParams->Append( piCmd->CreateParameter("@Version", adSmallInt, adParamInput, sizeof(SHORT), nVersion) );

		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);

        _ParameterPtr piPtr;
		piPtr = piParams->Item["@ReturnValue"];
		CMNLog(_T("[DBStorage::SetDatabaseVersion]: result = '%i'"), piPtr->Value.iVal); 
		if (piPtr->Value.iVal != 0)
		{
			CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
			*pEx << (LPCTSTR)piCmd->CommandText;
			*pEx << piPtr->Value.iVal;
			ThrowException(pEx);
		}	
    }
	catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_SETDBVERSION_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
}

void DBStorage::SetRootName(const bstr_t& bstrRootName)
{
    CSAMScopeLock scopeLock( m_pWriteLock );   
	HRESULT hr = S_OK;
    try
    {
		_CommandPtr piCmd;
        ParametersPtr piParams;

        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.SetRootName";
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = m_piWriteConnection;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
        piParams->Append( piCmd->CreateParameter("@RootName", adLongVarWChar, adParamInput, bstrRootName.length() + 1, (BSTR)bstrRootName ));

		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);

        _ParameterPtr piPtr;
		piPtr = piParams->Item["@ReturnValue"];
		CMNLog(_T("[DBStorage::SetRootName]: result = '%i'"), piPtr->Value.iVal); 
		if (piPtr->Value.iVal != 0)
		{
			CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
			*pEx << (LPCTSTR)piCmd->CommandText;
			*pEx << piPtr->Value.iVal;
			ThrowException(pEx);
		}	
    }
	catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_SETROOTNAME_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
}


bool DBStorage::IsComputerExist(const bstr_t& bstrComputerName)
{
	HRESULT hr = S_OK;
	_CommandPtr piCmd;
    ParametersPtr piParams;

    BEGIN_USE_CONNECTION(connectionFromPool)
        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.IsComputerExist";
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = connectionFromPool; 

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
        piParams->Append( piCmd->CreateParameter("@ComputerName", adLongVarWChar, adParamInput, bstrComputerName.length() + 1, (BSTR)bstrComputerName ));
        piParams->Append( piCmd->CreateParameter("@IsExist", adBoolean, adParamOutput, sizeof(BOOL) ));

		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);
    END_USE_CONNECTION

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];
	CMNLog(_T("[DBStorage::IsComputerExist]: result = '%i'"), piPtr->Value.iVal); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	
    return ((BOOL)piCmd->Parameters->Item["@IsExist"]->Value != FALSE);
}

bool DBStorage::IsObjectLocked(LPCTSTR szObjectGuid, LPCTSTR szRunningTasks)
{
	HRESULT hr = S_OK;
    bstr_t bstrObjectGuid = szObjectGuid;
    bstr_t bstrGuidList = szRunningTasks;

	_CommandPtr piCmd;
    ParametersPtr piParams;

    BEGIN_USE_CONNECTION(connectionFromPool)
        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.IsObjectLocked";
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = connectionFromPool;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
        piParams->Append( piCmd->CreateParameter("@ObjectGuid", adLongVarWChar, adParamInput, bstrObjectGuid.length() + 1, (BSTR)bstrObjectGuid ));
        piParams->Append( piCmd->CreateParameter("@XMLObject", adLongVarWChar, adParamInput, bstrGuidList.length() + 1, (variant_t)bstrGuidList) );
        piParams->Append( piCmd->CreateParameter("@IsLocked", adBoolean, adParamOutput, sizeof(BOOL) ));

		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);
    END_USE_CONNECTION

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];
	CMNLog(_T("[DBStorage::IsObjectLocked]: result = '%i'"), piPtr->Value.iVal); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	
    return ((BOOL)piCmd->Parameters->Item["@IsLocked"]->Value != FALSE);
}

void DBStorage::CreateComputerLink(const bstr_t& bstrTaskGuid, const bstr_t& bstrComputerGuid)
{
    CSAMScopeLock scopeLock( m_pWriteLock );    
	HRESULT hr = S_OK;
	_CommandPtr piCmd;
    ParametersPtr piParams;

    piCmd.CreateInstance(__uuidof(Command));
    piCmd->CommandText = "dbo.CreateCompTaskLink";
    piCmd->CommandType = adCmdStoredProc;  

    piParams = piCmd->Parameters;
    piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
    piParams->Append( piCmd->CreateParameter("@ComputerGUID", adLongVarWChar, adParamInput, bstrComputerGuid.length() + 1, (BSTR)bstrComputerGuid ));
    piParams->Append( piCmd->CreateParameter("@TaskGUID", adLongVarWChar, adParamInput, bstrTaskGuid.length() + 1, (BSTR)bstrTaskGuid ));

    piCmd->ActiveConnection = m_piWriteConnection; 
	piCmd->CommandTimeout = 0;						 //	no timeout
    piCmd->Execute(NULL, NULL, adCmdStoredProc);

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];
    
	CMNLog(_T("[DBStorage::CreateComputerLink]: result = '%i'"), piPtr->Value.iVal); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}		
}

long DBStorage::GetCurrentCompCount()
{
	try
    {
		_CommandPtr piCmd;
        _RecordsetPtr piRecords;

        BEGIN_USE_CONNECTION(connectionFromPool)
	        piCmd.CreateInstance(__uuidof(Command));
	        piCmd->CommandText = bstr_t("SELECT count(*) as compCount FROM Computer");
	        piCmd->CommandType = adCmdText; 

            piCmd->ActiveConnection = connectionFromPool; 
			piCmd->CommandTimeout = 0;						 //	no timeout
	        piRecords = piCmd->Execute(NULL, NULL, adCmdText);
        END_USE_CONNECTION

        FieldsPtr piFields = piRecords->GetFields();
        FieldPtr piField = piFields->GetItem(CComVariant(0));
        long lCompCount = (long)piField->GetValue();
        return lCompCount;
	}
    catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_GETCOMPUTERSCOUNT_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
}

// -------------------------------------------------------------------------------------------------------------------
long DBStorage::GetSuccessInspectedCompCount()
{
	try
    {
		_CommandPtr piCmd;
        _RecordsetPtr piRecords;

        BEGIN_USE_CONNECTION(connectionFromPool)
	        piCmd.CreateInstance(__uuidof(Command));
	        piCmd->CommandText = bstr_t("SELECT count(*) as compCount FROM ViewMaxComputerAction");
	        piCmd->CommandType = adCmdText; 

            piCmd->ActiveConnection = connectionFromPool; 
			piCmd->CommandTimeout = 0;						 //	no timeout
	        piRecords = piCmd->Execute(NULL, NULL, adCmdText);
        END_USE_CONNECTION

        FieldsPtr piFields = piRecords->GetFields();
        FieldPtr piField = piFields->GetItem(CComVariant(0));
        long lCompCount = (long)piField->GetValue();
        return lCompCount;
	}
    catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_GETCOMPUTERSCOUNT_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
}


int DBStorage::GetObjectCount(LPCTSTR szParentGuid, LPCTSTR szObjectType, const VARIANT_BOOL vbRecursive)
{
	HRESULT hr = S_OK;
    bstr_t bstrParentGuid = szParentGuid;
    bstr_t bstrObjectType = szObjectType;

	_CommandPtr piCmd;
    ParametersPtr piParams;

    BEGIN_USE_CONNECTION(connectionFromPool) 

        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.GetObjectCount";
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = connectionFromPool; 

        piParams = piCmd->Parameters;

        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
	    piParams->Append( piCmd->CreateParameter("@ParentGuid", adLongVarWChar, adParamInput, bstrParentGuid.length() + 1, (BSTR)bstrParentGuid ));
        piParams->Append( piCmd->CreateParameter("@ObjectType", adLongVarWChar, adParamInput, bstrObjectType.length() + 1, (BSTR)bstrObjectType ));
        piParams->Append( piCmd->CreateParameter("@RecursiveSearch", adBoolean, adParamInput,  sizeof(BOOL), vbRecursive ));
        piParams->Append( piCmd->CreateParameter("@ObjectCount", adInteger, adParamOutput, sizeof(INT) ));

		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);
    
    END_USE_CONNECTION  

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];
//	CMNLog(_T("[DBStorage::GetObjectCount]: result = '%i'"), piPtr->Value.iVal); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	
    return (int)piCmd->Parameters->Item["@ObjectCount"]->Value;
}

DATE DBStorage::GetCreationDate(LPCTSTR szDatabase)
{
	try
    {
        bstr_t bstrQuery = bstr_t("SELECT create_date from master.sys.databases where name = '") + bstr_t(szDatabase) + bstr_t("'");
		_CommandPtr piCmd;
        _RecordsetPtr piRecords;

        BEGIN_USE_CONNECTION(connectionFromPool)
	        piCmd.CreateInstance(__uuidof(Command));
	        piCmd->CommandText = bstrQuery; 

	        piCmd->CommandType = adCmdText; 

            piCmd->ActiveConnection = connectionFromPool;
			piCmd->CommandTimeout = 0;						 //	no timeout
	        piRecords = piCmd->Execute(NULL, NULL, adCmdText);
        END_USE_CONNECTION

        FieldsPtr piFields = piRecords->GetFields();
        FieldPtr piField = piFields->GetItem(CComVariant(0));
        DATE dtCreationDate = (DATE)piField->GetValue();
        return dtCreationDate;
	}
    catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_GETCREATIONDATE_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
}

_RecordsetPtr DBStorage::GetComputerSoft(const bstr_t& bstrComputerGuid)
{
	HRESULT hr = S_OK;

    // create recordset
	_RecordsetPtr piRowsPtr = NULL;
	piRowsPtr.CreateInstance(__uuidof(Recordset));
	piRowsPtr->CursorLocation = adUseClient;  
	piRowsPtr->CursorType = adOpenStatic;  

    //create Command
	_CommandPtr piCmd;
    ParametersPtr piParams;

    BEGIN_USE_CONNECTION(connectionFromPool)
        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.GetComputerSoft"; 
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = connectionFromPool;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
	    piParams->Append( piCmd->CreateParameter("@ComputerGUID", adLongVarWChar, adParamInput, bstrComputerGuid.length() + 1, (BSTR)bstrComputerGuid ));

		piCmd->CommandTimeout = 0;						 //	no timeout
	    piRowsPtr->Open( _variant_t((IDispatch *)piCmd, true), 
								vtMissing, 
								adOpenForwardOnly, 
								adLockReadOnly, 
								-1 );
    END_USE_CONNECTION

	variant_t vtNull; 
	vtNull.vt = VT_NULL;
	piRowsPtr->put_ActiveConnection(vtNull);

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];

	CMNLog(_T("[DBStorage::GetComputerSoft] Вернуть ПО для компьютера '%s'"), (LPCTSTR)bstrComputerGuid); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	

	if(!piRowsPtr)
		return NULL;

	if(piRowsPtr->State == adStateClosed)
		return NULL;

	return piRowsPtr;
}

_RecordsetPtr DBStorage::GetComputerOS(const bstr_t& bstrComputerGuid)
{
	HRESULT hr = S_OK;

	// create recordset
	_RecordsetPtr piRowsPtr = NULL;
	piRowsPtr.CreateInstance(__uuidof(Recordset));
	piRowsPtr->CursorLocation = adUseClient;  
	piRowsPtr->CursorType = adOpenStatic;  

    //create Command
	_CommandPtr piCmd;
    ParametersPtr piParams;

    BEGIN_USE_CONNECTION(connectionFromPool)
        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.GetComputerOS"; 
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = connectionFromPool;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
	    piParams->Append( piCmd->CreateParameter("@ComputerGUID", adLongVarWChar, adParamInput, bstrComputerGuid.length() + 1, (BSTR)bstrComputerGuid ));

		piCmd->CommandTimeout = 0;						 //	no timeout
	    piRowsPtr->Open( _variant_t((IDispatch *)piCmd, true), 
								vtMissing, 
								adOpenForwardOnly, 
								adLockReadOnly, 
								-1 );
    END_USE_CONNECTION

	variant_t vtNull; 
	vtNull.vt = VT_NULL;
	piRowsPtr->put_ActiveConnection(vtNull);

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];

	CMNLog(_T("[DBStorage::GetComputerOS] Вернуть ОС для компьютера '%s'"), (LPCTSTR)bstrComputerGuid); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	

	if(!piRowsPtr)
		return NULL;

	if(piRowsPtr->State == adStateClosed)
		return NULL;

	return piRowsPtr;
}

_RecordsetPtr DBStorage::GetComputerOSList(const bstr_t& bstrComputerGuid)
{
	HRESULT hr = S_OK;

	// create recordset
	_RecordsetPtr piRowsPtr = NULL;
	piRowsPtr.CreateInstance(__uuidof(Recordset));
	piRowsPtr->CursorLocation = adUseClient;  
	piRowsPtr->CursorType = adOpenStatic;  

    //create Command
	_CommandPtr piCmd;
    ParametersPtr piParams;

    BEGIN_USE_CONNECTION(connectionFromPool)
        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.GetComputerOSList"; 
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = connectionFromPool;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
	    piParams->Append( piCmd->CreateParameter("@ComputerGUID", adLongVarWChar, adParamInput, bstrComputerGuid.length() + 1, (BSTR)bstrComputerGuid ));

		piCmd->CommandTimeout = 0;						 //	no timeout
	    piRowsPtr->Open( _variant_t((IDispatch *)piCmd, true), 
								vtMissing, 
								adOpenForwardOnly, 
								adLockReadOnly, 
								-1 );
    END_USE_CONNECTION

	variant_t vtNull; 
	vtNull.vt = VT_NULL;
	piRowsPtr->put_ActiveConnection(vtNull);

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];

	CMNLog(_T("[DBStorage::GetComputerOSList] Вернуть список ОС для компьютера '%s'"), (LPCTSTR)bstrComputerGuid); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	

	if(!piRowsPtr)
		return NULL;

	if(piRowsPtr->State == adStateClosed)
		return NULL;

	return piRowsPtr;
}

_RecordsetPtr DBStorage::GetComputerHard(const bstr_t& bstrComputerGuid)
{
    HRESULT hr = S_OK;

	// create recordset
	_RecordsetPtr piRowsPtr = NULL;
	piRowsPtr.CreateInstance(__uuidof(Recordset));
	piRowsPtr->CursorLocation = adUseClient;  
	piRowsPtr->CursorType = adOpenStatic;  

    //create Command
	_CommandPtr piCmd;
    ParametersPtr piParams;



    BEGIN_USE_CONNECTION(connectionFromPool)
        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.GetComputerHard"; 
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = connectionFromPool;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
	    piParams->Append( piCmd->CreateParameter("@ComputerGUID", adLongVarWChar, adParamInput, bstrComputerGuid.length() + 1, (BSTR)bstrComputerGuid ));

		piCmd->CommandTimeout = 0;						 //	no timeout
	    piRowsPtr->Open( _variant_t((IDispatch *)piCmd, true), 
								vtMissing, 
								adOpenForwardOnly, 
								adLockReadOnly, 
								-1 );
    END_USE_CONNECTION

	variant_t vtNull; 
	vtNull.vt = VT_NULL;
	piRowsPtr->put_ActiveConnection(vtNull);

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];

	CMNLog(_T("[DBStorage::GetComputerHard] Вернуть оборудование для компьютера '%s'"), (LPCTSTR)bstrComputerGuid); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	

	if(!piRowsPtr)
		return NULL;

	if(piRowsPtr->State == adStateClosed)
		return NULL;

	return piRowsPtr;
}

_RecordsetPtr DBStorage::GetInspectedComputers()
{
    HRESULT hr = S_OK;

	// create recordset
	_RecordsetPtr piRowsPtr = NULL;
	piRowsPtr.CreateInstance(__uuidof(Recordset));
	piRowsPtr->CursorLocation = adUseClient;  
	piRowsPtr->CursorType = adOpenStatic;  

    //create Command
	_CommandPtr piCmd;
    ParametersPtr piParams;

    BEGIN_USE_CONNECTION(connectionFromPool)
        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.GetInspectedComputers"; 
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = connectionFromPool;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );

		piCmd->CommandTimeout = 0;						 //	no timeout
	    piRowsPtr->Open( _variant_t((IDispatch *)piCmd, true), 
								vtMissing, 
								adOpenForwardOnly, 
								adLockReadOnly, 
								-1 );
    END_USE_CONNECTION

	variant_t vtNull; 
	vtNull.vt = VT_NULL;
	piRowsPtr->put_ActiveConnection(vtNull);

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];

	CMNLog(_T("[DBStorage::GetInspectedComputers] Вернуть проинспектированные компьютеры")); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	

	if(!piRowsPtr)
		return NULL;

	if(piRowsPtr->State == adStateClosed)
		return NULL;

	return piRowsPtr;
}

_RecordsetPtr DBStorage::GetReports()
{
    HRESULT hr = S_OK;

	// create recordset
	_RecordsetPtr piRowsPtr = NULL;
	piRowsPtr.CreateInstance(__uuidof(Recordset));
	piRowsPtr->CursorLocation = adUseClient;  
	piRowsPtr->CursorType = adOpenStatic;  

    //create Command
	_CommandPtr piCmd;
    ParametersPtr piParams;

    BEGIN_USE_CONNECTION(connectionFromPool)
        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.GetReports"; 
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = connectionFromPool;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );

		piCmd->CommandTimeout = 0;						 //	no timeout
	    piRowsPtr->Open( _variant_t((IDispatch *)piCmd, true), 
								vtMissing, 
								adOpenForwardOnly, 
								adLockReadOnly, 
								-1 );
    END_USE_CONNECTION

	variant_t vtNull; 
	vtNull.vt = VT_NULL;
	piRowsPtr->put_ActiveConnection(vtNull);

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];

	CMNLog(_T("[DBStorage::GetReports] Вернуть список отчетов")); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	

	if(!piRowsPtr)
		return NULL;

	if(piRowsPtr->State == adStateClosed)
		return NULL;

	return piRowsPtr;
}

bstr_t DBStorage::GetSoftLibrary(const short sLibType, const bstr_t& bstrInitChars, const bstr_t& bstrEndChars)
{
	// create recordset
	_RecordsetPtr piRowsPtr = NULL;
	piRowsPtr.CreateInstance(__uuidof(Recordset));
	piRowsPtr->CursorLocation = adUseClient;  
	piRowsPtr->CursorType = adOpenStatic;  

	//create Command
	_CommandPtr piCmd;
	ParametersPtr piParams;

	BEGIN_USE_CONNECTION(connectionFromPool)
		piCmd.CreateInstance(__uuidof(Command));
		piCmd->CommandText = "dbo.GetLibraryXML"; 
		piCmd->CommandType = adCmdStoredProc;  
		piCmd->ActiveConnection = connectionFromPool;

		piParams = piCmd->Parameters;
		piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
	    piParams->Append( piCmd->CreateParameter("@LibType", adSmallInt, adParamInput, sizeof(SHORT), sLibType));
	    piParams->Append( piCmd->CreateParameter("@InitChars", adLongVarWChar, adParamInput, bstrInitChars.length() + 1, bstrInitChars));
	    piParams->Append( piCmd->CreateParameter("@EndChars", adLongVarWChar, adParamInput,	bstrEndChars.length() + 1, bstrEndChars));

		piCmd->CommandTimeout = 0;						 //	no timeout
		piRowsPtr->Open( _variant_t((IDispatch *)piCmd, true), 
						vtMissing, 
						adOpenForwardOnly, 
						adLockReadOnly, 
						-1 );
	END_USE_CONNECTION

	variant_t vtNull; 
	vtNull.vt = VT_NULL;
	piRowsPtr->put_ActiveConnection(vtNull);

	_ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];

	CMNLog(_T("[DBStorage::GetSoftLibrary] Вернуть библиотеку ПО")); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}

	if ( ! piRowsPtr )
		return "";

	if ( piRowsPtr->State == adStateClosed )
		return "";

	HRESULT hr = piRowsPtr->MoveFirst();
	CMNCheckError(hr);

	variant_t vVal = piRowsPtr->Fields->Item[(long)0]->Value;
	
	bstr_t bstrXML = "";
	if ( vVal.vt == VT_BSTR )
		bstrXML = vVal;
	return bstrXML;
}

void DBStorage::InsertSoftLibrary(const bstr_t& bstrLibraryXML)
{
    CSAMScopeLock scopeLock( m_pWriteLock );
	ParametersPtr piParams;
	_CommandPtr piCmd;

	CMNLog(_T("[DBStorage::InsertSoftLibrary]: Starting insertion soft library"));
	m_piWriteConnection->BeginTrans();

	try
	{
		piCmd.CreateInstance(__uuidof(Command));
		piCmd->CommandText = "dbo.AddLibrary";
		piCmd->CommandType = adCmdStoredProc;  

		piParams = piCmd->Parameters;

		piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
		piParams->Append( piCmd->CreateParameter("@XMLObject", adLongVarWChar, adParamInput, bstrLibraryXML.length() + 1, bstrLibraryXML ));

	    piCmd->ActiveConnection = m_piWriteConnection;
		piCmd->CommandTimeout = 0;	// no timeout

		piCmd->Execute(NULL, NULL, adCmdStoredProc);
	}
	catch(CMNException* pEx)
	{
		CString strExceptionError = CMNException::GetAllErrorMessages(pEx);
		CMNLog(_T("[DBStorage::InsertSoftLibrary]: Exception occurs: 's'. Rollback transaction..."), (LPCTSTR)strExceptionError); 
		m_piWriteConnection->RollbackTrans();
		pEx->Throw();
	}

	m_piWriteConnection->CommitTrans();
	_ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];

	CMNLog(_T("[DBStorage::InsertSoftLibrary]: result = '%i'"), piPtr->Value.iVal); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}		
}

bstr_t DBStorage::RetrievePassword(const bstr_t& bstrTaskGuid, const bstr_t& bstrPassPhrase)
{
    bstr_t bstrPassword = "";
	_CommandPtr piCmd;
    ParametersPtr piParams;

    BEGIN_USE_CONNECTION(connectionFromPool);
        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.RetrievePassword";
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = connectionFromPool;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
        piParams->Append( piCmd->CreateParameter("@TaskGUID", adLongVarWChar, adParamInput, bstrTaskGuid.length() + 1, (BSTR)bstrTaskGuid ));
        piParams->Append( piCmd->CreateParameter("@PassPhrase", adLongVarWChar, adParamInput, bstrPassPhrase.length() + 1, (BSTR)bstrPassPhrase ));
        piParams->Append( piCmd->CreateParameter("@Password", adVarWChar, adParamOutput, 501 ));

		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);
    END_USE_CONNECTION

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];
	CMNLog(_T("[DBStorage::RetrievePassword]: result = '%i'"), piPtr->Value.iVal); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	

    variant_t varPassword = piCmd->Parameters->Item["@Password"]->Value;
    if (varPassword.vt != VT_NULL)
        bstrPassword = varPassword;

    return bstrPassword;
}

void DBStorage::SavePassword(const bstr_t& bstrTaskGuid, const bstr_t& bstrPassPhrase, const bstr_t& bstrPassword )
{
    CSAMScopeLock scopeLock( m_pWriteLock );   
	HRESULT hr = S_OK;
    try
    {
		_CommandPtr piCmd;
        ParametersPtr piParams;

        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.SavePassword";
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = m_piWriteConnection;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
	    piParams->Append( piCmd->CreateParameter("@TaskGUID", adLongVarWChar, adParamInput, bstrTaskGuid.length() + 1, (BSTR)bstrTaskGuid) );
        piParams->Append( piCmd->CreateParameter("@PassPhrase", adLongVarWChar, adParamInput, bstrPassPhrase.length() + 1, (BSTR)bstrPassPhrase) );
        piParams->Append( piCmd->CreateParameter("@Password", adLongVarWChar, adParamInput, bstrPassword.length() + 1, (BSTR)bstrPassword) );

		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);

        _ParameterPtr piPtr;
		piPtr = piParams->Item["@ReturnValue"];
		CMNLog(_T("[DBStorage::SavePassword]: result = '%i'"), piPtr->Value.iVal); 
		if (piPtr->Value.iVal != 0)
		{
			CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
			*pEx << (LPCTSTR)piCmd->CommandText;
			*pEx << piPtr->Value.iVal;
			ThrowException(pEx);
		}	
    }
	catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_SETDBVERSION_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
}

void DBStorage::ApplySynonyms( )
{
    CSAMScopeLock scopeLock( m_pWriteLock );   
	HRESULT hr = S_OK;

    try
    {
		_CommandPtr piCmd;
        ParametersPtr piParams;

        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.UpdateDBOnNewSynonyms";
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = m_piWriteConnection;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );

		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);

        _ParameterPtr piPtr;
		piPtr = piParams->Item["@ReturnValue"];
		CMNLog(_T("[DBStorage::ApplySynonyms]: result = '%i'"), piPtr->Value.iVal); 
		if (piPtr->Value.iVal != 0)
		{
			CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
			*pEx << (LPCTSTR)piCmd->CommandText;
			*pEx << piPtr->Value.iVal;
			ThrowException(pEx);
		}	
    }
	catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_APPLYSYNONYMS_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
}

_RecordsetPtr DBStorage::ExecuteCommand(LPCTSTR pszCommand)
{	
    CSAMScopeLock scopeLock( m_pWriteLock );    
	_RecordsetPtr piRec;
	try
    {
		_CommandPtr piCmd;
        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = (bstr_t)pszCommand;
        piCmd->CommandType = adCmdText; 

        piCmd->ActiveConnection = m_piWriteConnection;
		piCmd->CommandTimeout = 0;						 //	no timeout
        piRec = piCmd->Execute(NULL, NULL, adCmdText);		
	}
    catch (CMNException* pEx)
    {	
		CString str (pszCommand);		
		int iLength = str.GetLength();
		/*
		if (iLength > 100)
		{
			CString strShort (_T("\n"));
			strShort += str.Left(50);		
			strShort += _T("\n<...>\n");
			strShort += str.Right(50);
			str = strShort;
		}
		*/
		CMNException* pHiLevelEx = new CMNException(IDE_DB_COMMAND_FAILED);
		*pHiLevelEx << (LPCTSTR)str;
		*pHiLevelEx << iLength;
		*pHiLevelEx	+= *pEx;

		ThrowException(pHiLevelEx);
	}
	return piRec;
}

void DBStorage::ExecuteProcedure(LPCTSTR pszCommand)
{
    CSAMScopeLock scopeLock( m_pWriteLock );    
	try
    {
        _CommandPtr piCmd;

        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = pszCommand;
        piCmd->CommandType = adCmdStoredProc;  

        ParametersPtr piParams = piCmd->Parameters;

        piCmd->ActiveConnection = m_piWriteConnection;
		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);  
    }
    catch (CMNException* pEx)
    {
		CMNException* pHiLevelEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pHiLevelEx << pszCommand;
		*pHiLevelEx << pEx->GetErrorCode();
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}    
}

void DBStorage::RegisterNewReports(LPCTSTR lpszConfigXML)
{
	CSAMScopeLock scopeLock(m_pWriteLock);

	CMNLog(_T("[DBStorage::RegisterNewReports]"));

	bstr_t bstrXMLBody = lpszConfigXML;
	if(!bstrXMLBody.length())
		return ;

	_CommandPtr piCmd;
	ParametersPtr piParams;

	piCmd.CreateInstance(__uuidof(Command));
	piCmd->CommandText = "dbo.InsertNewReports";
	piCmd->CommandType = adCmdStoredProc;

	piParams = piCmd->Parameters;

	piParams->Append(piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL));
	piParams->Append(piCmd->CreateParameter("@XMLReportList", adLongVarWChar, adParamInput, bstrXMLBody.length() + 1, (variant_t) bstrXMLBody));

	piCmd->ActiveConnection = m_piWriteConnection;
	piCmd->CommandTimeout = 0;						 //	no timeout
	piCmd->Execute(NULL, NULL, adCmdStoredProc);

	_ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];

	CMNLog(_T("[DBStorage::RegisterNewReports]: result = '%i'"), piPtr->Value.iVal);

	if(piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}
}

void DBStorage::GetReportParams(const bstr_t& bstrReportName, bstr_t& bstrDefaultVersion,  bstr_t& bstrParamList, bool& bReportTitle, bstr_t& bstrReportFriendlyName, bstr_t& bstrReportSubFolder)
{
    bstrParamList = "";
    bstrDefaultVersion = "";
    bstrReportFriendlyName = "";
    bstrReportSubFolder = "";
    bReportTitle = true;

	_CommandPtr piCmd;
    ParametersPtr piParams;

    BEGIN_USE_CONNECTION(connectionFromPool);
        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.GetReportParams";
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = connectionFromPool;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
        piParams->Append( piCmd->CreateParameter("@ReportName", adLongVarWChar, adParamInput, bstrReportName.length() + 1, (BSTR)bstrReportName ));
        piParams->Append( piCmd->CreateParameter("@ParamList", adVarWChar, adParamOutput, 501 ));
        piParams->Append( piCmd->CreateParameter("@DefaultVersion", adVarWChar, adParamOutput, 501 ));
        piParams->Append( piCmd->CreateParameter("@IsReportTitle", adBoolean, adParamOutput, sizeof(BOOL) ));
        piParams->Append( piCmd->CreateParameter("@ReportFriendlyName", adVarWChar, adParamOutput, 501 ));
        piParams->Append( piCmd->CreateParameter("@ReportSubFolder", adVarWChar, adParamOutput, 501 ));

		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);
    END_USE_CONNECTION

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];
	CMNLog(_T("[DBStorage::GetReportParams]: result = '%i'"), piPtr->Value.iVal); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	

    variant_t varDefaultVersion = piCmd->Parameters->Item["@DefaultVersion"]->Value;
    if (varDefaultVersion.vt != VT_NULL)
        bstrDefaultVersion = varDefaultVersion;

    variant_t varParamList = piCmd->Parameters->Item["@ParamList"]->Value;
    if (varParamList.vt != VT_NULL)
        bstrParamList = varParamList;

    variant_t varReportTitle = piCmd->Parameters->Item["@IsReportTitle"]->Value;
    if (varReportTitle.vt != VT_NULL)
        bReportTitle = varReportTitle;

    variant_t varReportFriendlyName = piCmd->Parameters->Item["@ReportFriendlyName"]->Value;
    if (varReportFriendlyName.vt != VT_NULL)
        bstrReportFriendlyName = varReportFriendlyName;

    variant_t varReportSubFolder = piCmd->Parameters->Item["@ReportSubFolder"]->Value;
    if (varReportSubFolder.vt != VT_NULL)
        bstrReportSubFolder = varReportSubFolder;
}

_RecordsetPtr  DBStorage::GetRepCompWithSoftParams(const bstr_t& bstrObjectGUIDList, const bstr_t& bstrSubdivisionName)
{
    try
    {

      _CommandPtr piCmd;
      ParametersPtr piParams;
      _RecordsetPtr piRowsPtr;

      BEGIN_USE_CONNECTION(connectionFromPool);
      piCmd.CreateInstance(__uuidof(Command));
      piCmd->CommandText = "dbo.GetRepComputersWithSoftParams";
      piCmd->CommandType = adCmdStoredProc;  
      piCmd->ActiveConnection = connectionFromPool;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
        piParams->Append( piCmd->CreateParameter("@ObjectGUIDList", adLongVarWChar, adParamInput, bstrObjectGUIDList.length() + 1, (BSTR)bstrObjectGUIDList ));
        piParams->Append( piCmd->CreateParameter("@SubdivisionName", adLongVarWChar, adParamInput, bstrSubdivisionName.length() + 1, (BSTR)bstrSubdivisionName ));

		piCmd->CommandTimeout = 0;						 //	no timeout
        piRowsPtr = piCmd->Execute(NULL, NULL, adCmdStoredProc);

    END_USE_CONNECTION

        return piRowsPtr;
    }
	catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_GETREPPARAMVALUE_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}	
}

void DBStorage::GetLanguage(const bstr_t& bstrLocalization, bstr_t& bstrLanguage )
{
    bstrLanguage = "";

	_CommandPtr piCmd;
    ParametersPtr piParams;

    BEGIN_USE_CONNECTION(connectionFromPool)
        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.SPGetLanguage";
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = connectionFromPool; 

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
        piParams->Append( piCmd->CreateParameter("@Localization", adVarWChar, adParamInput, bstrLocalization.length() + 1, (BSTR)bstrLocalization ));
        piParams->Append( piCmd->CreateParameter("@Language", adVarWChar, adParamOutput, 501 ));

		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);
    END_USE_CONNECTION

    _ParameterPtr piPtr;
	piPtr = piParams->Item["@ReturnValue"];
	CMNLog(_T("[DBStorage::GetLanguage]: result = '%i'"), piPtr->Value.iVal); 
	if (piPtr->Value.iVal != 0)
	{
		CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pEx << (LPCTSTR)piCmd->CommandText;
		*pEx << piPtr->Value.iVal;
		ThrowException(pEx);
	}	

    variant_t varLanguage = piCmd->Parameters->Item["@Language"]->Value;
    if (varLanguage.vt != VT_NULL)
        bstrLanguage = varLanguage;

}

void DBStorage::InsertPicture(const CComVariant& varData, VARIANT_BOOL vbUserPicture)
{
    CSAMScopeLock scopeLock( m_pWriteLock );    
    try
    {
		_CommandPtr piCmd;
        ParametersPtr piParams;

        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.SetUserPicture";
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = m_piWriteConnection;
        
        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
        piParams->Append( piCmd->CreateParameter("@Image", adLongVarBinary, adParamInput, varData.GetSize(), varData ));
        piParams->Append( piCmd->CreateParameter("@IsUserPicture", adBoolean, adParamInput, sizeof(BOOL), vbUserPicture ));

		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);

	    _ParameterPtr piPtr;
		piPtr = piParams->Item["@ReturnValue"];
		CMNLog(_T("[DBStorage::InsertPicture]: result = '%i'"), piPtr->Value.iVal); 
		if (piPtr->Value.iVal != 0)
		{
			CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
			*pEx << (LPCTSTR)piCmd->CommandText;
			*pEx << piPtr->Value.iVal;
			ThrowException(pEx);
		}	
    }
	catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_INSERTPICTURE_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
}

void DBStorage::InsertPicture(const UINT nID, VARIANT_BOOL vbUserPicture)
{
    CSAMScopeLock scopeLock( m_pWriteLock );    
    try
    {
        CComVariant varData;
        CMNLoadBinaryFromResources(nID, varData);

		_CommandPtr piCmd;
        ParametersPtr piParams;

        piCmd.CreateInstance(__uuidof(Command));
        piCmd->CommandText = "dbo.SetUserPicture";
        piCmd->CommandType = adCmdStoredProc;  
        piCmd->ActiveConnection = m_piWriteConnection;

        piParams = piCmd->Parameters;
        piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
        piParams->Append( piCmd->CreateParameter("@Image", adLongVarBinary, adParamInput, varData.GetSize(), varData ));
        piParams->Append( piCmd->CreateParameter("@IsUserPicture", adBoolean, adParamInput, sizeof(BOOL), vbUserPicture ));

		piCmd->CommandTimeout = 0;						 //	no timeout
        piCmd->Execute(NULL, NULL, adCmdStoredProc);

	    _ParameterPtr piPtr;
		piPtr = piParams->Item["@ReturnValue"];
		CMNLog(_T("[DBStorage::InsertPicture]: result = '%i'"), piPtr->Value.iVal); 
		if (piPtr->Value.iVal != 0)
		{
			CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
			*pEx << (LPCTSTR)piCmd->CommandText;
			*pEx << piPtr->Value.iVal;
			ThrowException(pEx);
		}	
    }
	catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_INSERTPICTURE_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
}

void DBStorage::InsertDefaultPicture()
{
    InsertPicture(IDR_DB_DEFAULT_PICTURE);
}

CComVariant DBStorage::GetPicture()
{
    CComVariant resultPicture;
    bstr_t bstrCommand("SELECT UserPicture FROM ObjectRoot");
	try
    {
        _RecordsetPtr piRec;
        BEGIN_USE_CONNECTION(connectionFromPool)
	        _CommandPtr piCmd;
            piCmd.CreateInstance(__uuidof(Command));
            piCmd->CommandText = bstrCommand;
            piCmd->CommandType = adCmdText; 

            piCmd->ActiveConnection = connectionFromPool;
			piCmd->CommandTimeout = 0;						 //	no timeout
            piRec = piCmd->Execute(NULL, NULL, adCmdText);
        END_USE_CONNECTION 

        if(!piRec)
    	    return resultPicture;

	    if(!piRec->GetRecordCount())
    	    return resultPicture;

        piRec->MoveFirst();
	    if(piRec->GetadoEOF() == VARIANT_FALSE)
	    {

		    FieldsPtr spFields = piRec->GetFields();
		    if(!spFields)
            {
			    return resultPicture;
            }

		    long lFieldsCount = spFields->GetCount();
		    if(!lFieldsCount)
            {
    			return resultPicture;
            }

            // Get each task guid from recordset
		    FieldPtr spField = spFields->GetItem(CComVariant(cszAttrib_UserPicture));
		    resultPicture = spField->GetValue();

        }
	}
    catch (CMNException* pEx)
    {	
		int iLength = bstrCommand.length();
		CMNException* pHiLevelEx = new CMNException(IDE_DB_COMMAND_FAILED);
		*pHiLevelEx << (LPCTSTR)bstrCommand;
		*pHiLevelEx << iLength;
		*pHiLevelEx	+= *pEx;

		ThrowException(pHiLevelEx);
	}
    return resultPicture;
}

void	DBStorage::CreateDBOwner(LPCTSTR	pszDatabaseName, LPCTSTR	pszLogin )
{
    CSAMScopeLock scopeLock( m_pWriteLock );    
    CString		strCommand;
	CString		strUser(pszLogin);
	int			nSlash = 0;
	
	try{	
		nSlash = strUser.ReverseFind('\\');
		strUser = strUser.Mid( nSlash + 1 );

		// look for login
		{		
			_CommandPtr piCmd;

		    piCmd.CreateInstance(__uuidof(Command));

            // Get sid from login account
            PSID pSid = NULL;
            BOOL bGetSid = CMNAuthorityGrantor::GetAccountSid(NULL, pszLogin, &pSid);
			CMNCheckLastError(bGetSid);
            
            // Convert binary sid to string 
            CString  strTemp;
            CString  strSID = _T("0x");
            for (int i=0; i < ::GetLengthSid(pSid); i++)
            {
                strTemp.Format(_T("%0.2X"), ((BYTE*)pSid)[i]);
                strSID += strTemp;
            }
            strCommand.Format(_T("select * from syslogins where sid=cast(%s as varbinary)"), (LPCTSTR)strSID);

		    piCmd->CommandText = (bstr_t)strCommand;
		    piCmd->CommandType = adCmdText; 

            piCmd->ActiveConnection = m_piWriteConnection;
		    _RecordsetPtr piRec = piCmd->Execute(NULL, NULL, adCmdText);
		    if ( piRec->adoEOF == VARIANT_TRUE )// not found
		    {
			    strCommand.Format(_T("CREATE LOGIN [%s] FROM WINDOWS"), pszLogin);
			    ExecuteCommand(strCommand);
		    }
		}

		strCommand.Format(_T("USE [%s]"), pszDatabaseName);
		ExecuteCommand(strCommand);

		strCommand.Format(_T("CREATE USER [%s] FOR LOGIN [%s]"), strUser, pszLogin);
		ExecuteCommand(strCommand);

		strCommand.Format(_T("EXEC sp_addrolemember 'db_owner', '%s'"), strUser ); 
		ExecuteCommand(strCommand);
	}
	catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_ADDLOGIN_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}

}

void DBStorage::GrantReadPermissionsForGuest()
{
	try
	{
		ExecuteCommand( _T("GRANT CONNECT TO guest") );
		ExecuteCommand( _T("GRANT EXECUTE TO guest") );
		ExecuteCommand( _T("GRANT SELECT TO guest") );
	}
	catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_GRANT_GUEST_PERMISSIONS_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
}

_RecordsetPtr DBStorage::GetReportParamsValues(const bstr_t& bsObjectGUID, const bstr_t& bsReportName)
{
    try
    {
        _CommandPtr piCmd;
        ParametersPtr piParams;
        _RecordsetPtr piRowsPtr;
        BEGIN_USE_CONNECTION(connectionFromPool);
            piCmd.CreateInstance(__uuidof(Command));
            piCmd->CommandText = "dbo.GetReportParamsValues";
            piCmd->CommandType = adCmdStoredProc;  
            piCmd->ActiveConnection = connectionFromPool;

            piParams = piCmd->Parameters;
            piParams->Append( piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT), NULL) );
            piParams->Append( piCmd->CreateParameter("@ObjectGUID", adLongVarWChar, adParamInput, bsObjectGUID.length() + 1, (BSTR)bsObjectGUID ));
            piParams->Append( piCmd->CreateParameter("@ReportName", adLongVarWChar, adParamInput, bsReportName.length() + 1, (BSTR)bsReportName ));

			piCmd->CommandTimeout = 0;						 //	no timeout
            piRowsPtr = piCmd->Execute(NULL, NULL, adCmdStoredProc);
        END_USE_CONNECTION

        return piRowsPtr;
    }
	catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_GETREPPARAMVALUE_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------
void DBStorage::ExecuteProcedure(const bstr_t& bstrProcName, set<DBStorageProcParameter*>& pParameters)
{
	try
    {
        _CommandPtr piCmd;
        ParametersPtr piParams;
        _RecordsetPtr piRowsPtr;
        BEGIN_USE_CONNECTION(connectionFromPool);
            piCmd.CreateInstance(__uuidof(Command));
            piCmd->CommandText = bstrProcName;
            piCmd->CommandType = adCmdStoredProc;  
            piCmd->ActiveConnection = connectionFromPool;

            piParams=piCmd->Parameters;
			for(set<DBStorageProcParameter*>::iterator it=pParameters.begin(); it!=pParameters.end(); ++it)
			{
				piParams->Append( piCmd->CreateParameter((*it)->m_bstrName, (*it)->m_enType, (*it)->m_enDirection, (*it)->m_lSize, (*it)->m_vtValue) );
			}

			piCmd->CommandTimeout = 0;						 //	no timeout
            piRowsPtr = piCmd->Execute(NULL, NULL, adCmdStoredProc);
        END_USE_CONNECTION

    }
	catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
		*pHiLevelEx << (LPCTSTR)bstrProcName;
		*pHiLevelEx << pEx->GetErrorCode();
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
}


void DBStorage::AddUser(LPCTSTR pszUserName, LPCTSTR pszLoginNT, bstr_t& bstrResult)
{
/*
		declare @i int
		declare @UserName varchar(128) = 'Тест  '
		declare @LoginNT varchar(128) = 'HOST\NT_TEST2'
		declare @result nvarchar(4000)
		exec @i = ADD_USERNAME @UserName, @LoginNT, @result out
		select @i as I, @result as RES
*/
	bstr_t bstrUserName = pszUserName;
	bstr_t bstrLoginNT  = pszLoginNT;

	CSAMScopeLock scopeLock(m_pWriteLock);
	HRESULT hr = S_OK;
	
	try
	{		
		ParametersPtr piParams;
		_CommandPtr piCmd;
		piCmd.CreateInstance(__uuidof(Command));
		piCmd->CommandText = "dbo.ADD_USER";
		piCmd->CommandType = adCmdStoredProc;
		piCmd->ActiveConnection = m_piWriteConnection;

		piParams = piCmd->Parameters;
		piParams->Append(piCmd->CreateParameter("@ReturnValue", adSmallInt, adParamReturnValue, sizeof(SHORT),				NULL				));
		piParams->Append(piCmd->CreateParameter("@UserName",	adLongVarWChar,	adParamInput,		bstrUserName.length() + 1,  (BSTR)bstrUserName	));
		piParams->Append(piCmd->CreateParameter("@LoginNT",		adLongVarWChar,	adParamInput,		bstrLoginNT.length()  + 1,  (BSTR)bstrLoginNT	));
		piParams->Append(piCmd->CreateParameter("@result",		adVarWChar,		adParamOutput,		4001,						(BSTR)bstrResult	));

		piCmd->CommandTimeout = 0;						 //	no timeout
		piCmd->Execute(NULL, NULL, adCmdStoredProc);

		bstrResult = piCmd->Parameters->Item["@result"]->Value;

		_ParameterPtr piPtr;
		piPtr = piParams->Item["@ReturnValue"];
		CMNLog(_T("[DBStorage::AddUser]: result = '%i'"), piPtr->Value.iVal);
		if (piPtr->Value.iVal != 1)
		{
			CMNException* pEx = new CMNException(IDE_DB_STORED_PROCEDURE_FAILED);
			*pEx << (LPCTSTR)piCmd->CommandText;
			*pEx << piPtr->Value.iVal;
			ThrowException(pEx);
		}
	}
	catch (CMNException* pEx)
	{
		CMNException* pHiLevelEx = new CMNException(IDE_DB_ADD_USER_FAILED);
		*pHiLevelEx << pszUserName;
		*pHiLevelEx << pszLoginNT;
		*pHiLevelEx += *pEx;
		ThrowException(pHiLevelEx);
	}	
}





// ============================================================================================================
DBPropertyHelper::DBPropertyHelper()
{
    //Create XMLDocument and Object node in it.
	MSXML::IXMLDOMDocumentPtr piXmlDocument; 
	MSXML::IXMLDOMNodePtr piBodyNode; 

	piXmlDocument.CreateInstance(__uuidof(MSXML::DOMDocument));	
	piBodyNode = piXmlDocument->createNode( CComVariant(MSXML::NODE_ELEMENT),
											_T("Object"), _T(""));

	piBodyNode->QueryInterface(&m_piElement);
}

DBPropertyHelper::~DBPropertyHelper()
{

}

bstr_t DBPropertyHelper::GetPropertyBSTR(CComVariant varPropertyValue)
{
    bstr_t bstrValue;
    switch ( varPropertyValue.vt )
    {
    case VT_DATE:
        {   // if it's a DateTime then format it correctly
            COleDateTime dateTimeValue = (COleDateTime)varPropertyValue;
            bstrValue = dateTimeValue.Format( DATE_TIME_COMMON_FORMAT );
            break;
        }
    case VT_BOOL:
        {
            VARIANT_BOOL vbValue = varPropertyValue.boolVal;
            bstrValue = (vbValue != VARIANT_FALSE) ? _T("1") : _T("0") ;
            break;
        }
    default:
        {
            // else just return string representation of a value
            varPropertyValue.ChangeType(VT_BSTR);
            bstrValue = varPropertyValue.bstrVal;
        }
    }
    return bstrValue;
}

void DBPropertyHelper::Append(const bstr_t& bstrPropertyName, const bstr_t& bstrPropertyValue)
{
    m_piElement->setAttribute(bstrPropertyName, bstrPropertyValue);
}

bstr_t DBPropertyHelper::GetXML()
{
    BSTR bsXMLString;
	m_piElement->get_xml(&bsXMLString);
    
    bstr_t bstrXMLString;
    bstrXMLString.Attach(bsXMLString);
    return bstrXMLString;
}



