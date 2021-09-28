//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        DB
//  Cooperation: absent
//  Date:        2008-11-13
//--------------------------------------------------------------------------------
//  Description: General universal definitions
//--------------------------------------------------------------------------------

#pragma once

#if defined (_BUILD_DBDLL)                   // defined when we are building the dll
	#define DB_CLASS __declspec(dllexport)
#elif defined (_DBDLL)                       // defined when we are using the dll
	#define	DB_CLASS __declspec(dllimport)     
#else                                     
	#define DB_CLASS
#endif


#define DB_FUNC  DB_CLASS
#define DB_DATA  DB_CLASS  
#define DB_API   __stdcall


// auto-linking dynamic libraries
#if !defined(_BUILD_DBDLL) && !defined(_BUILD_DBLIB)
    #pragma comment(lib,"DB.lib")
#endif
