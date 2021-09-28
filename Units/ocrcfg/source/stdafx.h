//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit: samcfg
//--------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.

#define _WIN32_WINNT	0x0501
#define  WINVER         0x0501
#define _WIN32_WINDOWS 0x0500 // Windows Me or later
#define _WIN32_IE 0x0600	  // IE 6.0 or later.

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>     
#include <afxcmn.h>   
#include <afxext.h>   
#include <afxcoll.h>   
#include <afxtempl.h> 
#include <afxdisp.h>  
#include <afxdtctl.h>
#include <afxdlgs.h>
#include <afxmt.h>
#include <afxole.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlstr.h>
#include <mshtml.h>
#include <mshtmhst.h>


#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxdlgs.h>

// ATL
#include <atlbase.h>

// STL
#include <map>
#include <set>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>

#include <Winsvc.h>

#define SECURITY_WIN32
#include <Security.h>

using namespace ATL;
using namespace std;

#import <msxml.tlb> 
using namespace MSXML;
//#import <msxml2.tlb> 
//using namespace MSXML2;

#import <msado20.tlb> rename_namespace("USEADO") rename ("EOF","adoEOF")
using namespace USEADO;

/*
#ifdef _DEBUG
#import "../bin/Win32/Debug/RptMan.tlb" rename_namespace("RPTMAN")
#else
#import "../bin/Win32/Release/RptMan.tlb" rename_namespace("RPTMAN")
#endif

using namespace RPTMAN;
*/

class CConfigApp;
extern CConfigApp theApp;

#define CUSTOM_THROW_EXCEPTION
#include "CMN/include/ThrowException.h"

#include "T/include/ComException.h"
#include "T/include/Misc.h"
