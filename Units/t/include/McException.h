//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Exceptions
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Exceptions class for HR codes with mc file as a message source
//               idea taken from CodeGuru and refacored
//--------------------------------------------------------------------------------

#pragma once

#include <Ntsecapi.h>

#include "CMN/include/McException.h"

#include "T/include/Constants.h"
#include "T/include/T.h"
#include "T/include/Exception.h"

/////////////////////////////////////////////////////////////////////////
//
//  TMcException - GetErrorMessage() is overridden, because message text resides in *.mc file, 
//	not in *.rc string table

class TMcException;

typedef CArray <HMODULE, HMODULE&> TArrayHModule;

T_FUNC void T_API   TCheckError     (HRESULT hr, bool bIgnorePositive = true, LPCSTR pszFile = 0, int nLine = 0);
T_FUNC void T_API   TCheckIsError   (BOOL bFuncResult,  LPCSTR pszFile = 0, int nLine = 0);
T_FUNC void T_API   TCheckNtStatus  (NTSTATUS ntStatus, LPCSTR pszFile = 0, int nLine = 0);
T_FUNC void T_API   TCheckWIN32Error(long lWIN32Error,  LPCSTR pszFile = 0, int nLine = 0);
//T_FUNC void T_API   TCheckADSError  (HRESULT hr, LPCSTR pszFile = 0, int nLine = 0);

T_FUNC TMcException* T_API THasHRESULT (TException* pHiLevelEx, HRESULT hr, bool bFindAnyPositive = false, bool bFindAny = false);

T_FUNC CString __cdecl TFormatString (HRESULT hr, va_list pBuf);
T_FUNC CString __cdecl TFormatString (HRESULT hr, ...);


class T_CLASS TMcException : public TException, virtual public CMNMcException
{
    DECLARE_DYNAMIC(TMcException)

    friend T_FUNC CString __cdecl TFormatString (HRESULT hr, va_list pBuf);
//    friend T_FUNC CString __cdecl TFormatString (HRESULT hr, ...);

protected:
    // [by CMNMcException] static TArrayHModule    ms_arrHModules;
    // [by CMNMcException] HRESULT                 m_hr;

public:
    TMcException(TMcException const& rException);
    TMcException(CMNMcException const& rException);
	TMcException(HRESULT hr, UINT nType = MB_OK | /*MB_HELP | */MB_ICONERROR, UINT nErrorCode = IDE_T_MC_ERROR);
    virtual ~TMcException() {}

	virtual BOOL    GetErrorMessage( LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL ) { return TException::GetErrorMessage(lpszError, nMaxError, pnHelpContext); }
	virtual HRESULT GetHRESULT() const { return CMNMcException::GetHRESULT(); }

	virtual void	Delete() { TException::Delete(); }

//  TException overrides
protected:
    virtual void    InternalThrow();
	virtual void    FormatPrefix (LPTSTR pszError, UINT nMaxError) const { CMNMcException::FormatPrefix(pszError, nMaxError); }
	virtual void    FormatMessage(LPCTSTR pcszFormat, LPTSTR lpszError, UINT nMaxError) const { CMNMcException::FormatMessage(pcszFormat, lpszError, nMaxError); }
    virtual CMNException* Clone();

//  operations
public:
    TMcException&   operator <<  (LPCTSTR pszArg);

	// Searches for last TMcException in the chain of exceptions.
	// Returns HRESULT of an exception or S_OK is exception is not found.
	// [by CMNMcException] static  HRESULT FindMcHR(TException *pEx);
    // [by CMNMcException] static  HMODULE T_API AddModule(LPCTSTR pcszModuleName);
    // [by CMNMcException] static  void    ReleaseAllModules();

protected:
    static  void    FormatString (HRESULT hr, LPTSTR lpszError, UINT nMaxError, va_list pBuf);
    TMcException&   operator <<  (const int iArg); // only for internal use
};
