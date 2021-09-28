//--------------------------------------------------------------------
// Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------
// Unit:        db
// Cooperation: absent
//--------------------------------------------------------------------
// Description: Pool of database connections
//--------------------------------------------------------------------
#pragma once
#include "cmn/include/lockMap.h"
#include "db/include/db.h"

class DBConnectionPool
{
    friend class DBConnectionHolder;
private:
    static DBConnectionPool* s_pConnectionPool;
    static int s_nConnectionCount;
    CSAMLock* m_pConnectionListLock; 
    
    HANDLE m_hFreeConnectionEvent; 

    typedef list<CAdapt<_ConnectionPtr>> TConnection;
    TConnection m_FreeConnections;

private:
    // make ctor private - create connection pool singleton
    DBConnectionPool(void);
public:
    static DBConnectionPool* Create();
    static DBConnectionPool* GetInstance();

    virtual ~DBConnectionPool(void); 
    void InitConnectionPool(LPCTSTR pszConnectionString, LPCTSTR pszServer, LPCTSTR pszDatabase, const int nConnectionCount);

private:
    _ConnectionPtr AcquireConnection();
    void ReleaseConnection(_ConnectionPtr);
};
