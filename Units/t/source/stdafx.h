//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: absent
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------------

#pragma once

// Module is targeted for NT2000 as "minimum system required"
#define _WIN32_WINNT	0x0501
#define WINVER			0x0501

#define NOMINMAX                            // disable min() and max() macros
#define VC_EXTRALEAN                        // Exclude rarely-used stuff from Windows headers
#define _DEFINE_DEPRECATED_HASH_CLASSES 0   // disable old hashmap classes

#define _CRT_SECURE_NO_WARNINGS             // warning C4996: '_wsplitpath': This function or variable may be unsafe. 
#define _CRT_NON_CONFORMING_SWPRINTFS       // warning C4996: '_swprintf': swprintf has been changed to conform with the ISO C standard,

#pragma warning(disable : 4018) // '<' : signed/unsigned mismatch

#include <afx.h>
#include <afxwin.h>
#include <afxext.h>
#include <afxcoll.h>
#include <afxdisp.h>
#include <afxconv.h>
#include <afxole.h>
#include <afxmt.h>
#include <afxtempl.h>
#include <afxpriv.h>
#include <afxext.h>         // расширения MFC
#include <comdef.h>

// ATL
#include <atlbase.h>

// STL
#include <algorithm>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <iterator>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>
#include <list>
#include <memory>		// for shared_ptr

using namespace std;


#include <wincrypt.h>
#include <winerror.h>

#include "CMN/include/ThrowException.h"

//#define new DEBUG_NEW
//  uncomment only when looking for memory leaks

