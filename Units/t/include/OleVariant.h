//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Misc
//  Date:        2008-11-17
//--------------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------------

#pragma once

// compiler vendor files
#include <memory>

// 3-d party libraries
// includes from other modules
// includes from this module
// its own header
#include "T/include/T.h"

class TOleVariant;

/////////////////////////////////////////////////////////////////////////////
// Variant Util functions

// Removes indirection from pvarg (only 1-st level).
// Change type of pvarg from VT_BYREF|VT_any to VT_any,
// Except VT_BYREF|VT_VARIANT, witch is changed to this (refernced) variant type until base type.
T_FUNC HRESULT T_API TVariantRemoveInd (VARIANTARG*  pvarg);

// The same, but works.
T_FUNC void T_API TVariantRemoveByRefs( LPCVARIANT pcvarSrc, TOleVariant& rvarDst );

/////////////////////////////////////////////////////////////////////////////
// class TOleVariant
// Extends COleVariant

class T_CLASS TOleVariant : public COleVariant
{
public:
	typedef	TOleVariant	Class;
	typedef COleVariant	Base;
	typedef shared_ptr<Class>	Ptr;

// Type definitions
public:
// Construction    
    TOleVariant ();
    TOleVariant (const VARIANT& varSrc);
    TOleVariant (LPCVARIANT pSrc);
    TOleVariant (const TOleVariant& varSrc);
    TOleVariant (const char* lpszSrc);
    TOleVariant (LPCTSTR lpszSrc, VARTYPE vtSrc); // used to set to ANSI string
    TOleVariant (CString const& strSrc);
    TOleVariant (BYTE nSrc);
    TOleVariant (short nSrc, VARTYPE vtSrc = VT_I2);
    TOleVariant (long lSrc, VARTYPE vtSrc = VT_I4);
    TOleVariant (const COleCurrency& curSrc);
    TOleVariant (float fltSrc);
    TOleVariant (double dblSrc);
    TOleVariant (const COleDateTime& timeSrc);
    TOleVariant (const CByteArray& arrSrc);
    TOleVariant (const CLongBinary& lbSrc);
    TOleVariant (LPCITEMIDLIST pidl);

    // Additional constructors
    TOleVariant (IDispatch* pSrc, bool fAddRef = true);          // Creates a VT_DISPATCH
    TOleVariant (IUnknown* pSrc, bool fAddRef = true);           // Creates a VT_UNKNOWN
    TOleVariant (int nSrc);
    TOleVariant (const wchar_t *pSrcStr);                       // Creates a VT_BSTR
    TOleVariant (DWORD dwSrc);
    explicit TOleVariant (CComBSTR const& bstr);
    TOleVariant (bool   b);

    ~TOleVariant();

// Members
protected:    

// Virtual functions

// Operations
public:    
    // Extractors
    // This list was get from _varinat_t declaration
    // Define a macro for Extractor Definition and Declaration
    #undef  EXTRACTOR_DEF
    #define EXTRACTOR_DEF(strOp) operator strOp () const;

    EXTRACTOR_DEF(short)
    EXTRACTOR_DEF(long)
    EXTRACTOR_DEF(float)
    EXTRACTOR_DEF(double)
    EXTRACTOR_DEF(CY)
    EXTRACTOR_DEF(CString)
    EXTRACTOR_DEF(LPTSTR)
    EXTRACTOR_DEF(IDispatch*)
    EXTRACTOR_DEF(bool)
    EXTRACTOR_DEF(IUnknown*)
    EXTRACTOR_DEF(DECIMAL)
    EXTRACTOR_DEF(BYTE)
    EXTRACTOR_DEF(int)
    EXTRACTOR_DEF(SYSTEMTIME)
    EXTRACTOR_DEF(LPVOID)
    EXTRACTOR_DEF(DWORD)

    // Assignment
    const TOleVariant& operator = (const VARIANT& varSrc);
	const TOleVariant& operator = (LPCVARIANT pSrc);
	const TOleVariant& operator = (const COleVariant& varSrc);
	const TOleVariant& operator = (const char* lpszSrc);
	const TOleVariant& operator = (const CString& strSrc);
	const TOleVariant& operator = (BYTE nSrc);
	const TOleVariant& operator = (short nSrc);
	const TOleVariant& operator = (long lSrc);
	const TOleVariant& operator = (const COleCurrency& curSrc);
	const TOleVariant& operator = (float fltSrc);
	const TOleVariant& operator = (double dblSrc);
	const TOleVariant& operator = (const COleDateTime& dateSrc);
	const TOleVariant& operator = (const CByteArray& arrSrc);
	const TOleVariant& operator = (const CLongBinary& lbSrc);

    // Additional assignments
    const TOleVariant& operator = (IDispatch* pSrc);
    const TOleVariant& operator = (IUnknown* pSrc);
    const TOleVariant& operator = (int  iSrc);
    const TOleVariant& operator = (const wchar_t* pSrc);		// Assign a VT_BSTR
    const TOleVariant& operator = (DWORD dwSrc);
	const TOleVariant& operator = (bool b);

    void Assign (LPVOID pData, VARTYPE vtData);   // Copy Data from pData. vtData specify Data's type.

// Implemenation
protected:    
};


typedef CArray<TOleVariant, TOleVariant&> TOleVariantArray;
