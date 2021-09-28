//--------------------------------------------------------------------
// Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------
// Unit:        db
// Cooperation: absent
//--------------------------------------------------------------------
// Description: Helper class to get connections from Connection Pool
//              Must be used via macros 
//              Example: 
//
//              BEGIN_USE_CONNECTION(connectionHolder)
//                  _ConnectionPtr piConnection = connectionHolder.GetConnection();
//              END_USE_CONNECTION 
//---------------------------------------------------------------------------------------

#include "stdafx.h"
#include "db/include/ConnectionHolder.h"
#include "db/include/ConnectionPool.h"

DBConnectionHolder::DBConnectionHolder()
{
    DBConnectionPool* pConnectionPool =  DBConnectionPool::GetInstance();    
    _ASSERT( pConnectionPool != NULL );
    if (pConnectionPool)
        m_piConnection = pConnectionPool->AcquireConnection();
    else 
        m_piConnection = NULL;
}

DBConnectionHolder::~DBConnectionHolder()
{
    DBConnectionPool* pConnectionPool =  DBConnectionPool::GetInstance();    
    _ASSERT( pConnectionPool != NULL );
    if (pConnectionPool)
        pConnectionPool->ReleaseConnection(m_piConnection);
}

DBConnectionHolder::operator _Connection*() const
{
    return m_piConnection;
} 

_ConnectionPtr DBConnectionHolder::operator->() const
{
    return m_piConnection;
}

long DBConnectionHolder::GetState()
{
    _ASSERT( m_piConnection != NULL );
    return m_piConnection->GetState();
}
