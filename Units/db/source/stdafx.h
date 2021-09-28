//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        DB
//  Cooperation: absent
//  Date:        2008-11-14
//--------------------------------------------------------------------------------
//  Description: StdAfx file
//--------------------------------------------------------------------------------

#pragma once

#define WINVER          0x0502      // Windows 2003 or later.
#define _WIN32_WINNT    0x0502      // Windows 2003 or later.
#define _WIN32_WINDOWS  0x0500      // Windows Me or later
#define _WIN32_IE       0x0600      // IE 6.0 or later.
#define _WIN32_DCOM                 // for CoQueryProxyBlanket for example

#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE 
#endif
#endif

#define NOMINMAX                          // disable min() and max() macros
#define _DEFINE_DEPRECATED_HASH_CLASSES 0 // disable old hashmap classes
#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers

#define _CRT_SECURE_NO_WARNINGS // warning C4996: 'strncpy': This function or variable may be unsafe. 
#define _CRT_NON_CONFORMING_SWPRINTFS // warning C4996: '_swprintf': swprintf has been changed to conform with the ISO C standard

#pragma warning(disable: 4251) // like in MFC
#pragma warning(disable: 4018) // '<' : signed/unsigned mismatch
#pragma warning(disable: 4995) // deprecated

#include <math.h>
#include <tchar.h>
#include <wtypes.h>
#include <windows.h>
#include <winbase.h>
#include <comdef.h>
#include <comutil.h>

#include <atlbase.h>
#include <atlconv.h>
#include <atlstr.h>
#include <atlsimpcoll.h>
#include <atlfile.h>
#include <ATLComTime.h>

#include <vector>
#include <list>
#include <set>
#include <map>
#include <memory>       // for shared_ptr
#include <strsafe.h>

using namespace ATL;
using namespace std;

#import <msxml.tlb> 
using namespace MSXML;
//#import <msxml2.tlb> 
//using namespace MSXML2;

#import <msado20.tlb> /*"c:\program files\common files\system\ado\msado15.dll"*/ rename_namespace("USEADO") rename ("EOF","adoEOF")
using namespace USEADO;

