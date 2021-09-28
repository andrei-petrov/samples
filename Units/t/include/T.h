//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: absent
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Import and Export declarations
//  All classes (and class ideas) in this module are taken from MSJ, CodeGuru,
//  Code Project, RSDN, DR GUI and other internet resources
//--------------------------------------------------------------------------------

#pragma once

#if defined ( _BUILD_TDLL )     // defined when we are building the dll
	#define T_CLASS __declspec(dllexport)
#elif defined ( _TDLL )         // defined when we are using the dll
	#define T_CLASS __declspec(dllimport)
#else                           // static libraries  
    #define T_CLASS           
#endif


#define T_FUNC  T_CLASS
#define T_DATA  T_CLASS


#define T_API   __stdcall

#if defined(_TDLL) || defined(_BUILD_TDLL)
    T_FUNC void T_API TInitModule();
#endif


