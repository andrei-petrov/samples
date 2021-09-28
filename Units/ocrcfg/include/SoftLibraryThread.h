//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Thread for background operations
//--------------------------------------------------------------------------

#pragma once

#include "db/include/Storage.h"


class CSoftLibraryThread : public WZRThread
{
//	DECLARE_DYNCREATE(CSoftLibraryThread)
protected:
	DBStorage			m_storage;
	CString				m_strSqlServer;
	CString				m_strDatabase;
	CSoftLibActionEnum	m_enSoftLibAction;
	const set<CString>*		m_setXmlFileNames;

public:
	CSoftLibraryThread(HWND hParentWnd, LPCTSTR pszSqlServer, LPCTSTR pszDatabase,
		CSoftLibActionEnum proc, const set<CString>* setXmlFileNames);
	virtual ~CSoftLibraryThread();

public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();

    bool IsDBExist();

protected:
	void ImportLibraryXmlFileToDB(CString strXmlFileName);

};


