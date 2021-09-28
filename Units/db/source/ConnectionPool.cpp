//--------------------------------------------------------------------
// Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------
// Unit:        db
// Cooperation: absent
//--------------------------------------------------------------------
// Description: Pool of database connections
//--------------------------------------------------------------------

#include "stdafx.h"
#include "cmn/include/Exception.h"
#include "db/include/Constants.h"
#include "db/include/ConnectionPool.h"

DBConnectionPool*  DBConnectionPool::s_pConnectionPool = NULL;
int DBConnectionPool::s_nConnectionCount = 0;

// Wait for free connection for 30 seconds
#define LOCK_TIMEOUT 30*1000

DBConnectionPool::DBConnectionPool(void)
{
    CMNLog(_T("[DBConnectionPool::DBConnectionPool] Start."));
    m_pConnectionListLock = new CSAMLock(); 

    // Create a auto-reset event object. 
    m_hFreeConnectionEvent = ::CreateEvent( 
        NULL,                       // default security attributes
        FALSE,                       // auto-reset event
        FALSE,                      // initial state is non-signaled
        _T("FreeConnectionEvent")   // object name
        ); 
    CMNLog(_T("[DBConnectionPool::DBConnectionPool] Stopped."));
}

DBConnectionPool* DBConnectionPool::Create() 
{
    _ASSERT( s_pConnectionPool == NULL );
    s_pConnectionPool = new DBConnectionPool();
    return s_pConnectionPool;
}

DBConnectionPool* DBConnectionPool::GetInstance()
{
    // check that connection Pool is created and initilized
    _ASSERT( s_pConnectionPool != NULL );
    _ASSERT( s_nConnectionCount != 0);
    return s_pConnectionPool;
}

DBConnectionPool::~DBConnectionPool(void)
{

    // CMNLog(_T("[DBConnectionPool::~DBConnectionPool] Before clearing connection list: Size of free connection map is %u"), m_FreeConnections.size());
    // close connections and clear connection list
    BEGIN_LOCK(m_pConnectionListLock) 
        // Assure that all connections were returned to connection list (released)
        _ASSERT( s_nConnectionCount == m_FreeConnections.size());

        _ConnectionPtr		piConnection;
        while (!m_FreeConnections.empty())
        {
            piConnection = m_FreeConnections.back( );
            piConnection->Close(); 
            m_FreeConnections.pop_back();    
        }   
	END_LOCK()
    
    // CMNLog(_T("[DBConnectionPool::~DBConnectionPool] After clearing connection list: Size of free connection map is %u"), m_FreeConnections.size());
    ::CloseHandle(m_hFreeConnectionEvent);
    delete m_pConnectionListLock; 
    s_pConnectionPool = NULL;
}

void DBConnectionPool::InitConnectionPool(LPCTSTR pszConnectionString, LPCTSTR pszServer, LPCTSTR pszDatabase, const int nConnectionCount)
{
    //to prevent calling init twice
    _ASSERT(m_FreeConnections.size() == 0);

    CString strConnect;
	strConnect.Format(pszConnectionString, pszServer, pszDatabase);

    _ConnectionPtr		piConnection;
    
    BEGIN_LOCK(m_pConnectionListLock) 
        for(int i = 0; i < nConnectionCount; i++)
        {
            piConnection.CreateInstance(__uuidof(Connection));   
            piConnection->Open( (bstr_t)strConnect, L"", L"", 0);
            m_FreeConnections.push_back(piConnection);
        }

        s_nConnectionCount = nConnectionCount;
//        CMNLog(_T("[DBConnectionPool::InitConnectionPool] Size of free connection map is %u"), m_FreeConnections.size());
        
        // Set m_hFreeConnectionEvent to signaled state
        ::SetEvent(m_hFreeConnectionEvent); 
	END_LOCK()
}

_ConnectionPtr DBConnectionPool::AcquireConnection()
{
    //CMNLog( _T("[DBConnectionPool::AcquireConnection] Start."));
    _ConnectionPtr piAcquiredConnection = NULL;

    // wait for free connection if we don't have any
    DWORD dwRes = ::WaitForSingleObject(m_hFreeConnectionEvent, LOCK_TIMEOUT /*INFINITE*/); 
    if(dwRes == WAIT_TIMEOUT)
    {
        CMNLog( _T("[DBConnectionPool::AcquireConnection] Waiting for free connection timeout occurs."));
        CMNException* pEx = new CMNException(IDE_DB_GETCONNECTION_TIMEOUT);
        ThrowException(pEx);
    }

	BEGIN_LOCK(m_pConnectionListLock) 
        // remove connection from list of free connections 
        piAcquiredConnection = m_FreeConnections.back( );
        m_FreeConnections.pop_back();    
        
        //CMNLog(_T("[DBConnectionPool::AcquireConnection] Size of free connection map is %u"), m_FreeConnections.size());
        if (!m_FreeConnections.empty())
        {
            // There is still free connections so Set event to signaled state
            ::SetEvent(m_hFreeConnectionEvent);
        }
	END_LOCK()
    //CMNLog( _T("[DBConnectionPool::AcquireConnection] Stopped."));
    return piAcquiredConnection;
}

void DBConnectionPool::ReleaseConnection(_ConnectionPtr piReleasedConnection)
{
    //CMNLog( _T("[DBConnectionPool::ReleaseConnection] Started."));
    BEGIN_LOCK(m_pConnectionListLock)
        m_FreeConnections.push_back(piReleasedConnection);

        //CMNLog(_T("[DBConnectionPool::ReleaseConnection] Size of free connection map is %u"), m_FreeConnections.size());
        // Set event to signaled state if we got free connection 
        if (m_FreeConnections.size() == 1)
        {
            ::SetEvent(m_hFreeConnectionEvent);
        }   
    END_LOCK()
    //CMNLog( _T("[DBConnectionPool::ReleaseConnection] Stopped."));
}

