//--------------------------------------------------------------------------------
//  Copyright: (C) 2021-2022 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:  Configurator      
//--------------------------------------------------------------------------
//  Description: Database description data
//--------------------------------------------------------------------------

#include "stdafx.h"

#include "ocrcfg/include/CreateDBData.h"



#pragma warning(disable: 4800)

//
//	CDBDescription class
//


CDBDescription::CDBDescription()
{
}


CString	CDBDescription::GetDatabaseData()
{
	return m_strDatabase + _T("_data");
}


CString CDBDescription::GetDatabaseLog()
{
	return m_strDatabase + _T("_log");
}


CString CDBDescription::GetDatabaseDataFile()
{
	return m_strDatabase + _T(".mdf");
}


CString	CDBDescription::GetDatabaseLogFile()
{
	return m_strDatabase + _T(".ldf");
}




CDeliceTechProcDBDesc::CDeliceTechProcDBDesc(LPCTSTR pszDatabase)
	: CDBDescription()
{
	if (pszDatabase)
		m_strDatabase				= pszDatabase;
	else
		m_strDatabase				= _T("DELICE_TECH_PROCESS");
	m_strDatabaseBackupFileName		= _T("DELICE_TECH_PROCESS_etalon_2_20210831135808823.bak"); // _T("delice_tech_process_etalon_20200907.bak");

	m_setRecognizableTables.insert(_T("DT_CODES_LIST"));
	m_setRecognizableTables.insert(_T("DT_CODES_LOG"));
	m_setRecognizableTables.insert(_T("DT_CODES_SETTINGS"));
	m_setRecognizableTables.insert(_T("DT_CODES_STATUSES"));
	m_setRecognizableTables.insert(_T("DT_CODES_TASKS"));
		
	m_setRecognizableProcedures.insert(_T("copyProjectsStats"));
	m_setRecognizableProcedures.insert(_T("copyTaskStructure"));
	m_setRecognizableProcedures.insert(_T("deletePart"));
	m_setRecognizableProcedures.insert(_T("deleteParts"));
	m_setRecognizableProcedures.insert(_T("indexingProjectsStats"));
}




CRioIpsDBDesc::CRioIpsDBDesc(LPCTSTR pszDatabase)
	: CDBDescription()
{
	if (pszDatabase)
		m_strDatabase				= pszDatabase;
	else
		m_strDatabase				= _T("RIO_IPS");
	m_strDatabaseBackupFileName		= _T("RIO_IPS_etalon_2_20210831135814370.bak"); // _T("RIO_IPS_etalon_20200907.bak");

	m_setRecognizableTables.insert(_T("DT_DOCUMENTS"));
	m_setRecognizableTables.insert(_T("DT_DOCUMENTS_HEAD"));
	m_setRecognizableTables.insert(_T("DT_DOCUMENTS_LIST"));
	m_setRecognizableTables.insert(_T("DT_DOCUMENTS_TABLES"));
	m_setRecognizableTables.insert(_T("DT_DOCUMENTS_XML"));

	m_setRecognizableProcedures.insert(_T("checkDBObjects"));
	m_setRecognizableProcedures.insert(_T("checkDocumentsTypesAccess"));
	m_setRecognizableProcedures.insert(_T("checkFiltersEditAccess"));
	m_setRecognizableProcedures.insert(_T("checkFoldersManagerAccess"));
	m_setRecognizableProcedures.insert(_T("checkIndexingAccess"));
}

CString	CRioIpsDBDesc::GetDatabaseData()
{
	return _T("RIO");
}

CString	CRioIpsDBDesc::GetDatabaseLog()
{
	return _T("RIO_log");
}

CString	CRioIpsDBDesc::GetDatabaseDataFile()
{
	return _T("RIO.mdf");
}

CString	CRioIpsDBDesc::GetDatabaseLogFile()
{
	return _T("RIO_log.ldf");
}




