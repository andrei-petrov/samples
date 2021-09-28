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
//--------------------------------------------------------------------
#pragma once
#include "cmn/include/lockMap.h"
#include "db/include/db.h"

//This macro creates connectionHolder which acquires connection from connection pool.
#define BEGIN_USE_CONNECTION(connectionName) \
    {\
        DBConnectionHolder connectionName; 

// This macro is a counterpart to BEGIN_USE_CONNECTION macro.
// It deletes connection holder, release previously acquired connection and returns it to connection pool.
#define END_USE_CONNECTION \
    }

class DB_CLASS DBConnectionHolder
{
private:
    _ConnectionPtr m_piConnection;
public:
    DBConnectionHolder( void);
    virtual ~DBConnectionHolder(void);
    operator _Connection*() const; 
    _ConnectionPtr operator->() const;
    long GetState();
};
