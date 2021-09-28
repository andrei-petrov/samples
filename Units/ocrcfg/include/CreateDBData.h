//--------------------------------------------------------------------------------
//  Copyright: (C) 2021-2022 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:  Configurator      
//--------------------------------------------------------------------------
//  Description: Database description data
//--------------------------------------------------------------------------

#pragma once

#include "cmn/include/Unknown.h"
//#include "db/include/Storage.h"


//typedef vector<CString> VZStringVector;


// Described particular DB that program will work with
class CDBDescription : public CMNUnknown
{
public:
	typedef CMNSmartPtr<CDBDescription>	Ptr;
	typedef vector<Ptr>					Vector;

	set<CAtlString>	m_setRecognizableTables;
	set<CAtlString>	m_setRecognizableProcedures;

	CString			m_strDatabase;					// sql server database name
	CString			m_strDatabaseBackupFileName;	// restore from

public:
	CDBDescription();

	virtual CString			GetDatabaseData();
	virtual CString			GetDatabaseLog();

	virtual CString			GetDatabaseDataFile();
	virtual CString			GetDatabaseLogFile();
};


class CDeliceTechProcDBDesc : public CDBDescription
{
public:
	CDeliceTechProcDBDesc(LPCTSTR pszDatabase);

};


class CRioIpsDBDesc : public CDBDescription
{
public:
	CRioIpsDBDesc(LPCTSTR pszDatabase);

	virtual CString			GetDatabaseData();
	virtual CString			GetDatabaseLog();

	virtual CString			GetDatabaseDataFile();
	virtual CString			GetDatabaseLogFile();
};



