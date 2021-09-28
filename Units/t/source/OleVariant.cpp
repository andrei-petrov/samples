//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Misc
//  Date:        2008-11-17
//--------------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------------

#include "StdAfx.h"

// compiler vendor files
// 3-d party libraries
// includes from other modules
// includes from this module
#include "CMN/Include/ComException.h"
// its own header
#include "T/Include/OleVariant.h"

/////////////////////////////////////////////////////////////////////////////
// Variant Util functions

// Removes indirection from pvarg (only 1-st level).
// Change type of pvarg from VT_BYREF|VT_any to VT_any,
// Except VT_BYREF|VT_VARIANT, witch is changed to this (refernced) variant type.
HRESULT T_API TVariantRemoveInd (VARIANTARG*  pvarg)
{
    if (pvarg == NULL)
        return S_OK;    

    while ((V_VT (pvarg) & VT_TYPEMASK) == VT_VARIANT &&
        !( pvarg->vt & VT_ARRAY ) )                // while variant is VT_BYREF|VT_VARIANT
    {
        VARIANT*    pvar = V_VARIANTREF(pvarg);
        ::VariantCopy (pvarg, pvar);
        //memcpy (pvarg, pvar, sizeof(VARIANT));    // Copy entire variant from pvarVal to this
	    //V_VT (pvar) = VT_EMPTY;
		//::VariantClear (pvar); 
    }

    if (!V_ISBYREF (pvarg))
        return S_OK;
    
    // Removes 1st level of indirection
    if (V_ISARRAY (pvarg))    // If array of some value - copy SAFEARRAY structure from "pparray" to "parray"
        // $$ memcpy (&(V_ARRAY (pvarg)), V_ARRAYREF (pvarg), sizeof (SAFEARRAY));
        V_ARRAY (pvarg) = *(V_ARRAYREF (pvarg));
    else
    {
        switch (V_VT (pvarg))
        {
            case (VT_BYREF|VT_DECIMAL)  :
            {
                DECIMAL*    pVal = V_DECIMALREF(pvarg);
                memcpy (&(V_DECIMAL(pvarg)), pVal, sizeof(DECIMAL));
                delete pVal;
            }   break;            

            case (VT_BYREF|VT_I1)       :
            {
                char*    pVal = V_I1REF(pvarg);
                memcpy (&(V_I1(pvarg)), pVal, sizeof(char));
                delete pVal;
            }   break;

            case (VT_BYREF|VT_I2)       :
            {
                short*    pVal = V_I2REF(pvarg);
                memcpy (&(V_I2(pvarg)), pVal, sizeof(short));
                delete pVal;
            }   break;

            case (VT_BYREF|VT_I4)       :
            {
                long*    pVal = V_I4REF(pvarg);
                memcpy (&(V_I4(pvarg)), pVal, sizeof(long));
                delete pVal;
            }   break;

            case (VT_BYREF|VT_R4)       :
            {
                float*    pVal = V_R4REF(pvarg);
                memcpy (&(V_R4(pvarg)), pVal, sizeof(float));
                delete pVal;
            }   break;

            case (VT_BYREF|VT_R8)       :
            {
                double*    pVal = V_R8REF(pvarg);
                memcpy (&(V_R8(pvarg)), pVal, sizeof(double));
                delete pVal;
            }   break;

            case (VT_BYREF|VT_BOOL)     :
            {
                VARIANT_BOOL*    pVal = V_BOOLREF(pvarg);
                memcpy (&(V_BOOL(pvarg)), pVal, sizeof(VARIANT_BOOL));
                delete pVal;
            }   break;

            case (VT_BYREF|VT_ERROR)    :
            {
                SCODE*    pVal = V_ERRORREF(pvarg);
                memcpy (&(V_ERROR(pvarg)), pVal, sizeof(SCODE));
                delete pVal;
            }   break;

            case (VT_BYREF|VT_CY)       :
            {
                CY*    pVal = V_CYREF(pvarg);
                memcpy (&(V_CY(pvarg)), pVal, sizeof(CY));
                delete pVal;
            }   break;

            case (VT_BYREF|VT_DATE)     :
            {
                DATE*    pVal = V_DATEREF(pvarg);
                memcpy (&(V_DATE(pvarg)), pVal, sizeof(DATE));
                delete pVal;
            }   break;

            case (VT_BYREF|VT_BSTR)     :
            {
                BSTR*    pVal = V_BSTRREF(pvarg);
                memcpy (&(V_BSTR(pvarg)), pVal, sizeof(BSTR));                
            }   break;

            case (VT_BYREF|VT_UNKNOWN)  :
            {
                IUnknown FAR* FAR*  pVal = V_UNKNOWNREF(pvarg);
                memcpy (&(V_UNKNOWN(pvarg)), pVal, sizeof(IUnknown FAR*));                
            }   break;

            case (VT_BYREF|VT_DISPATCH) :          
            {
                IDispatch FAR* FAR*    pVal = V_DISPATCHREF(pvarg);
                memcpy (&(V_DISPATCH(pvarg)), pVal, sizeof(IDispatch FAR*));                
            }   break;            

            case (VT_BYREF|VT_UI1)      :
            {
                BYTE*    pVal = V_UI1REF(pvarg);
                memcpy (&(V_UI1(pvarg)), pVal, sizeof(BYTE));
                delete pVal;
            }   break;            

            case (VT_BYREF|VT_UI2)      :
            {
                unsigned short*    pVal = V_UI2REF(pvarg);
                memcpy (&(V_UI2(pvarg)), pVal, sizeof(unsigned short));
                delete pVal;
            }   break;

            case (VT_BYREF|VT_UI4)      :
            {
                unsigned long*    pVal = V_UI4REF(pvarg);
                memcpy (&(V_UI4(pvarg)), pVal, sizeof(unsigned long));
                delete pVal;
            }   break;

            case (VT_BYREF|VT_INT)      :
            {
                int*    pVal = V_INTREF(pvarg);
                memcpy (&(V_INT(pvarg)), pVal, sizeof(int));
                delete pVal;
            }   break;

            case (VT_BYREF|VT_UINT)     :
            {
                unsigned int*    pVal = V_UINTREF(pvarg);
                memcpy (&(V_UINT(pvarg)), pVal, sizeof(unsigned int));
                delete pVal;
            }   break;

            default:
                return HRESULT_FROM_WIN32 (ERROR_INVALID_PARAMETER);
        }
    }

    V_VT (pvarg) = V_VT(pvarg) & ~VT_BYREF;
    return S_OK;
}

void T_API TVariantRemoveByRefs( LPCVARIANT pcvarSrc, TOleVariant& rvarDst )
{   
    if( rvarDst.vt != VT_EMPTY )
        rvarDst.Clear();
    VARIANT varTmp = *pcvarSrc;
    if( pcvarSrc->vt & VT_ARRAY )
    {
        if( pcvarSrc->vt & VT_BYREF )
        {
            varTmp.parray = *varTmp.pparray;
            varTmp.vt &= ~VT_BYREF;
        }
        
        SAFEARRAY* psaTmp;
        CMNCheckError( ::SafeArrayCopy( varTmp.parray, &psaTmp ) );
        varTmp.parray = psaTmp;
        
        if ((varTmp.vt & VT_TYPEMASK) == VT_VARIANT)
        {
            // this is a VARIANT array... walk through it
            ::SafeArrayLock (varTmp.parray);
            try
            {
                // TODO: add multi-dimensional array support
                ASSERT( ::SafeArrayGetDim( varTmp.parray ) == 1 );

                LONG iLBound, iUBound;
                CMNCheckError( ::SafeArrayGetLBound( varTmp.parray, 1, &iLBound ) );
                CMNCheckError( ::SafeArrayGetUBound( varTmp.parray, 1, &iUBound ) );
                

                TOleVariant varTmp2;
                VARIANT* pvarTmp3;
                for( LONG i = iLBound; i <= iUBound; i++ )
                {
                    ::SafeArrayPtrOfIndex (varTmp.parray, &i, (void**)&pvarTmp3);
                    TVariantRemoveByRefs( pvarTmp3, varTmp2 );
                    ::VariantClear (pvarTmp3);
                    *pvarTmp3 = varTmp2.Detach();
                }
                ::SafeArrayUnlock (varTmp.parray);
            }
            catch( CException* pEx )
            {
                ::SafeArrayUnlock (varTmp.parray);
                ::SafeArrayDestroy (varTmp.parray);
                throw pEx;
            }
        }
        rvarDst.Attach (varTmp);
    }
    else
    {
        CMNCheckError( ::VariantCopyInd( ( LPVARIANT )rvarDst, &varTmp ) );
        if( rvarDst.vt & VT_ARRAY )
        {
            varTmp = rvarDst.Detach();
            try
            {
                TVariantRemoveByRefs( &varTmp, rvarDst );
            }
            catch( CException* pEx )
            {
                ::VariantClear( &varTmp );
                throw pEx;
            }
            ::VariantClear( &varTmp );
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
// class TOleVariant
// Extends COleVariant

TOleVariant::TOleVariant()
    :   COleVariant ()
{
}

TOleVariant::TOleVariant(const VARIANT& varSrc)
    :   COleVariant (varSrc)
{
}

TOleVariant::TOleVariant(LPCVARIANT pSrc)
    :   COleVariant (pSrc)
{
}

TOleVariant::TOleVariant(const TOleVariant& varSrc)
    :   COleVariant (varSrc)
{
}

TOleVariant::TOleVariant(const char* lpszSrc)
    :   COleVariant (CString(lpszSrc))
{
}

TOleVariant::TOleVariant(LPCTSTR lpszSrc, VARTYPE vtSrc)
    :   COleVariant (lpszSrc, vtSrc)
{
}

TOleVariant::TOleVariant(CString const& strSrc)
    :   COleVariant (strSrc)
{
}


TOleVariant::TOleVariant(BYTE nSrc)
    :   COleVariant (nSrc)
{
}

TOleVariant::TOleVariant(short nSrc, VARTYPE vtSrc)
    :   COleVariant (nSrc, vtSrc)
{
}

TOleVariant::TOleVariant(long lSrc, VARTYPE vtSrc)
    :   COleVariant (lSrc, vtSrc)
{
}

TOleVariant::TOleVariant(const COleCurrency& curSrc)
    :   COleVariant (curSrc)
{
}


TOleVariant::TOleVariant(float fltSrc)
    :   COleVariant (fltSrc)
{
}

TOleVariant::TOleVariant(double dblSrc)
    :   COleVariant (dblSrc)
{
}

TOleVariant::TOleVariant(const COleDateTime& timeSrc)
    :   COleVariant (timeSrc)
{
}


TOleVariant::TOleVariant(const CByteArray& arrSrc)
    :   COleVariant (arrSrc)
{
}

TOleVariant::TOleVariant(const CLongBinary& lbSrc)
    :   COleVariant (lbSrc)
{
}


TOleVariant::TOleVariant(LPCITEMIDLIST pidl)
    :   COleVariant (pidl)
{
}

/*
TOleVariant::TOleVariant(list <CString> const& rarrSrc)
{
    V_VT(this) = VT_ARRAY | VT_BSTR;

    SAFEARRAY *psa;
    SAFEARRAYBOUND rgsabound[1];
    rgsabound[0].lLbound = 0;
    rgsabound[0].cElements = rarrSrc.size();
    psa = SafeArrayCreate(VT_BSTR, 1, rgsabound);
    if(psa == NULL)
        AfxThrowMemoryException();

    long rgIndices[1];
    long i = 0;

    for (list<CString>::const_iterator iter = rarrSrc.begin(); iter != rarrSrc.end(); ++iter, i++)
    {
        rgIndices[0] = i;
        TCheckError(SafeArrayPutElement(psa, rgIndices, ::SysAllocString((LPCTSTR)(*iter))));
    }

    this->parray = psa;
}
*/

TOleVariant::TOleVariant(IDispatch* pSrc, bool bAddRef)
    :   COleVariant ()
{
    V_VT(this) = VT_DISPATCH;
    V_DISPATCH(this) = pSrc;

    // Need the AddRef() as VariantClear() calls Release(), unless fAddRef
    // false indicates we're taking ownership
    //
    if (bAddRef && pSrc)
        V_DISPATCH(this)->AddRef();
}

TOleVariant::TOleVariant(IUnknown* pSrc, bool bAddRef)
    :   COleVariant ()
{
    V_VT(this) = VT_UNKNOWN;
    V_UNKNOWN(this) = pSrc;

    // Need the AddRef() as VariantClear() calls Release(), unless fAddRef
    // false indicates we're taking ownership
    //
    if (bAddRef && pSrc)
        V_UNKNOWN(this)->AddRef();
}

TOleVariant::TOleVariant (int nSrc)
    :   COleVariant ()
{
    V_VT (this) = VT_I4;
    V_I4 (this) = nSrc;    
}

// Creates a VT_BSTR
//
TOleVariant::TOleVariant (const wchar_t *pSrcStr)
{
    V_VT (this) = VT_BSTR;
	V_BSTR (this) = ::SysAllocString (pSrcStr);

	if (V_BSTR (this) == NULL && pSrcStr != NULL)     
		AfxThrowMemoryException();
}


TOleVariant::TOleVariant (DWORD dwSrc)
    :   COleVariant ((long) dwSrc)
{
}

TOleVariant::TOleVariant (CComBSTR const& bstr)
{
    V_VT (this) = VT_BSTR;
    V_BSTR (this) = bstr.Copy();
}

TOleVariant::TOleVariant (bool b)
{
    V_VT (this) = VT_BOOL;
    V_BOOL (this) = b ? VARIANT_TRUE : VARIANT_FALSE;
}

TOleVariant::~TOleVariant()
{
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Extractors
// The definition of Extrators was get from _variant_t definition
// NOTE: Extractor will not change type of This variant
//////////////////////////////////////////////////////////////////////////////////////////

#undef  EXTRACTOR_DECL
#define EXTRACTOR_DECL(strOp, vtType) \
    TOleVariant::operator strOp () const \
    { \
        if (V_VT(this) == VT_##vtType) { \
            return V_##vtType(this); \
        } \
        COleVariant varDest(this); \
        varDest.ChangeType(VT_##vtType); \
        return V_##vtType(&varDest); \
    }


    EXTRACTOR_DECL(short, I2)
    EXTRACTOR_DECL(long, I4)
    EXTRACTOR_DECL(float, R4)
    EXTRACTOR_DECL(double, R8)
    EXTRACTOR_DECL(CY, CY)
    EXTRACTOR_DECL(CString, BSTR)    
    EXTRACTOR_DECL(DECIMAL, DECIMAL)
    EXTRACTOR_DECL(BYTE, UI1)
    EXTRACTOR_DECL(int, INT)    
    EXTRACTOR_DECL(IDispatch*, DISPATCH)
    EXTRACTOR_DECL(IUnknown*, UNKNOWN)    

TOleVariant::operator bool() const
{
    if (V_VT(this) == VT_BOOL) {
        return V_BOOL(this) ? true : false;
    }

    TOleVariant varDest (this);
    varDest.ChangeType(VT_BOOL);

    return V_BOOL(&varDest) ? true : false;
}

TOleVariant::operator SYSTEMTIME() const
{
    DATE        date = 0;
    SYSTEMTIME  sysTime;
    memset (&sysTime, NULL, sizeof (SYSTEMTIME));

    if (V_VT(this) == VT_DATE) 
        date = V_DATE (this);
    else
    {
        TOleVariant varDest (this);
        varDest.ChangeType(VT_DATE);
        date = V_DATE (&varDest);
    }
    if (VariantTimeToSystemTime (date, &sysTime) == FALSE)
        AfxCheckError (GetScode(OLE_E_CANTCONVERT));
        
    return sysTime;
}

TOleVariant::operator LPTSTR() const
{
    if (V_VT(this) == VT_BSTR)
        return bstrVal;
    else if (V_VT(this) == (VT_BSTR|VT_BYREF))
        return *pbstrVal;

    TOleVariant varDest (this);
    varDest.ChangeType(VT_BSTR);
    return varDest.bstrVal;
}


TOleVariant::operator LPVOID() const
{
    if (V_ISARRAY (this))
        if (V_ISBYREF (this))
            return (LPVOID) &(V_ARRAYREF (this));
        else
            return (LPVOID) &(V_ARRAY (this));

    switch (V_VT(this) & VT_TYPEMASK)
    {
        case VT_BOOL:
            if (V_ISBYREF (this))
                return (LPVOID) &(V_BOOLREF (this));
            else
                return (LPVOID) &(V_BOOL (this));
            
        case VT_I1:
            if (V_ISBYREF (this))
                return (LPVOID) &(V_UI1REF (this));
            else
                return (LPVOID) &(V_UI1 (this));
        
        case VT_I2:        
            if (V_ISBYREF (this))
                return (LPVOID) &(V_UI2REF (this));
            else
                return (LPVOID) &(V_UI2 (this));

        case VT_I4:
        if (V_ISBYREF (this))
                return (LPVOID) &(V_UI4REF (this));
            else
                return (LPVOID) &(V_UI4 (this));
        
        case VT_UI1:
            if (V_ISBYREF (this))
                return (LPVOID) &(V_UI1REF (this));
            else
                return (LPVOID) &(V_UI1 (this));

        case VT_UI2:
            if (V_ISBYREF (this))
                return (LPVOID) &(V_UI2REF (this));
            else
                return (LPVOID) &(V_UI2 (this));

        case VT_UI4:
            if (V_ISBYREF (this))
                return (LPVOID) &(V_UI4REF (this));
            else
                return (LPVOID) &(V_UI4 (this));

        case VT_R4:
            if (V_ISBYREF (this))
                return (LPVOID) &(V_R4REF (this));
            else
                return (LPVOID) &(V_R4 (this));

        case VT_R8:
            if (V_ISBYREF (this))
                return (LPVOID) &(V_R8REF (this));
            else
                return (LPVOID) &(V_R8 (this));

        case VT_CY:
            if (V_ISBYREF (this))
                return (LPVOID) &(V_CYREF (this));
            else
                return (LPVOID) &(V_CY (this));

        case VT_DATE:            
            if (V_ISBYREF (this))
                return (LPVOID) &(V_DATEREF (this));
            else
                return (LPVOID) &(V_DATE (this));

        case VT_BSTR:
            if (V_ISBYREF (this))
                return (LPVOID) &(V_BSTRREF (this));
            else
                // "VT_DISPATCH, VT_UNKNOWN, and VT_BSTR variant types are pointers and do not require another level of indirection." [MSDN, COleSafeArray::PutElement]
                return (LPVOID) (V_BSTR (this));

        case VT_ERROR:
            if (V_ISBYREF (this))
                return (LPVOID) &(V_ERRORREF (this));
            else
                return (LPVOID) &(V_ERROR (this));

        case VT_DISPATCH:
            if (V_ISBYREF (this))
                return (LPVOID) &(V_DISPATCHREF (this));
            else
                // "VT_DISPATCH, VT_UNKNOWN, and VT_BSTR variant types are pointers and do not require another level of indirection." [MSDN, COleSafeArray::PutElement]
                return (LPVOID) (V_DISPATCH (this));

        case VT_UNKNOWN:
            if (V_ISBYREF (this))
                return (LPVOID) &(V_UNKNOWNREF (this));
            else
                // "VT_DISPATCH, VT_UNKNOWN, and VT_BSTR variant types are pointers and do not require another level of indirection." [MSDN, COleSafeArray::PutElement]
                return (LPVOID) &(V_UNKNOWN (this));

        case VT_VARIANT:          
            if (V_ISBYREF (this))
                return (LPVOID) (V_VARIANTREF (this));
            else
                ASSERT (false); // illegal type
            break;

        default:
          ASSERT(FALSE);
          break;
    }
    return NULL;
}

TOleVariant::operator DWORD() const
{
	return (DWORD ) operator long();
}

/* $$
TOleVariant::operator LPVARIANT()
{
    return this;
}

TOleVariant::operator LPCVARIANT() const
{
    return this;
}
*/
///////////////////////////////////////////////////////////////////////////////
// Assignment
//
const TOleVariant& TOleVariant::operator = (const VARIANT& varSrc)
{
    COleVariant::operator = (varSrc);
    return *this;
}

const TOleVariant& TOleVariant::operator = (LPCVARIANT pSrc)
{
    COleVariant::operator = (pSrc);
    return *this;
}

const TOleVariant& TOleVariant::operator = (const COleVariant& varSrc)
{
    COleVariant::operator = (varSrc);
    return *this;
}

const TOleVariant& TOleVariant::operator = (const char* lpszSrc)
{
    COleVariant::operator = (lpszSrc);
    return *this;
}

const TOleVariant& TOleVariant::operator = (const CString& strSrc)
{
    COleVariant::operator = (strSrc);
    return *this;
}

const TOleVariant& TOleVariant::operator = (BYTE nSrc)
{
    COleVariant::operator = (nSrc);
    return *this;
}

const TOleVariant& TOleVariant::operator = (short nSrc)
{
    COleVariant::operator = (nSrc);
    return *this;
}

const TOleVariant& TOleVariant::operator = (long lSrc)
{
    COleVariant::operator = (lSrc);
    return *this;
}

const TOleVariant& TOleVariant::operator = (const COleCurrency& curSrc)
{
    COleVariant::operator = (curSrc);
    return *this;
}

const TOleVariant& TOleVariant::operator = (float fltSrc)
{
    COleVariant::operator = (fltSrc);
    return *this;
}

const TOleVariant& TOleVariant::operator = (double dblSrc)
{
    COleVariant::operator = (dblSrc);
    return *this;
}

const TOleVariant& TOleVariant::operator = (const COleDateTime& dateSrc)
{
    COleVariant::operator = (dateSrc);
    return *this;
}

const TOleVariant& TOleVariant::operator = (const CByteArray& arrSrc)
{
    COleVariant::operator = (arrSrc);
    return *this;
}

const TOleVariant& TOleVariant::operator = (const CLongBinary& lbSrc)
{
    COleVariant::operator = (lbSrc);
    return *this;
}

// Additional assignments
//
const TOleVariant& TOleVariant::operator = (IDispatch* pSrc)
{
    // Clear the VARIANT (This will Release() any previous occupant)
	//
	Clear();

	V_VT(this) = VT_DISPATCH;
	V_DISPATCH(this) = pSrc;

	// Need the AddRef() as VariantClear() calls Release()
	//
    if (pSrc)
	    V_DISPATCH(this)->AddRef();

	return *this;
}

const TOleVariant& TOleVariant::operator = (IUnknown* pSrc)
{
    // Clear VARIANT (This will Release() any previous occupant)
	//
	Clear();

	V_VT(this) = VT_UNKNOWN;
	V_UNKNOWN(this) = pSrc;

	// Need the AddRef() as VariantClear() calls Release()
	//
    if (pSrc)
	    V_UNKNOWN(this)->AddRef();

	return *this;
}

const TOleVariant& TOleVariant::operator = (int iSrc)
{    
	Clear();
	V_VT (this) = VT_I4;
	V_I4 (this) = iSrc;	
    return *this;
}

// Assign a VT_BSTR
//
const TOleVariant& TOleVariant::operator = (const wchar_t* pSrc)
{    
	Clear();

	V_VT(this) = VT_BSTR;

	if (pSrc == NULL)
		V_BSTR(this) = NULL;
	else 
    {
		V_BSTR(this) = ::SysAllocString(pSrc);
		if (V_BSTR(this) == NULL) 
			AfxThrowMemoryException();
	}

	return *this;
}


const TOleVariant& TOleVariant::operator = (DWORD dwSrc)
{
    COleVariant::operator = ((long) dwSrc);
    return *this;
}

const TOleVariant& TOleVariant::operator = (bool b)
{
	Clear();
	V_VT (this) = VT_BOOL;
	V_BOOL (this) = b ? VARIANT_TRUE : VARIANT_FALSE;
    return *this;
}

// Copy Data from pData. vtData specify Data's type.
//
void TOleVariant::Assign (LPVOID pData, VARTYPE vtData)
{
    switch(vtData & VT_TYPEMASK)
    {
        case VT_BOOL:            
            *this = *((VARIANT_BOOL*)pData);
            break;

        /* $$ case VT_I1:
            *this = *((char*)pData);
            break; 
        */
        

        case VT_I2:
        case VT_UI2:
            *this = *((short*)pData);
            break;

        case VT_I4:
        case VT_UI4:
            *this = *((long*)pData);
            break;

        case VT_I1:
        case VT_UI1:
            *this = *((BYTE*)pData);
            break;

        /* $$ case VT_UI2:
            *this = *((WORD*)pData);
            break;         

        case VT_UI4:
            *this = *((DWORD*)pData);
            break;
        */

        case VT_R4:
            *this = *((float*)pData);
            break;

        case VT_R8:
            *this = *((double*)pData);
            break;

        case VT_CY:
            V_VT (this) = VT_CY;
            V_CY (this) = *((CY*)pData);            
            break;

        case VT_DATE:            
            *this = *((DATE*)pData);
            break;

        case VT_BSTR:
        {   // "VT_DISPATCH, VT_UNKNOWN, and VT_BSTR variant types are pointers and do not require another level of indirection." [MSDN, COleSafeArray::PutElement]
            BSTR&   rBstr = *((BSTR*)pData);
            *this = rBstr;
        }   break;

        case VT_ERROR:
            *this = *((SCODE*)pData);
            break;

        case VT_DISPATCH:
        {   // "VT_DISPATCH, VT_UNKNOWN, and VT_BSTR variant types are pointers and do not require another level of indirection." [MSDN, COleSafeArray::PutElement]
            IDispatch FAR*&    rpiDispatch = *(IDispatch**)pData;
            *this = rpiDispatch;
        }   break;

        case VT_UNKNOWN:
        {   // "VT_DISPATCH, VT_UNKNOWN, and VT_BSTR variant types are pointers and do not require another level of indirection." [MSDN, COleSafeArray::PutElement]
            IUnknown FAR*&    rpiUnknown = *(IUnknown**)pData;
            *this = rpiUnknown;
        }   break;

        case VT_VARIANT:          
            *this = *((VARIANT*)pData);             
            break;

        default:
          ASSERT(FALSE);
          break;
    }
}
