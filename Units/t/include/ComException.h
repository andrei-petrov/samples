//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Exceptions
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Com exceptions class based on HRESULT, interface and its IDD
//				 Idea taken from CodeGurus and refactored
//--------------------------------------------------------------------------------

#pragma once

#include <comdef.h>

#include "CMN/include/ComException.h"

#include "T/include/T.h"
#include "T/include/Constants.h"
#include "T/include/McException.h"


/////////////////////////////////////////////////////////////////////////
//
//  TComException
//

class TComException;

T_FUNC void TSetComRaiseErrorHandler();

T_FUNC void __stdcall  _t_raise_error	(const HRESULT hr, IErrorInfo* pIErrInfo);
//T_FUNC void __stdcall  _t_issue_errorex (const HRESULT hr, IUnknown* pIUnknown, REFIID nIID);
//T_FUNC void __stdcall  _t_issue_error   (const HRESULT hr);

// sorry, here I can't provide __FILE__ and __LINE__ arguments
T_FUNC void T_API TCheckErrorEx (HRESULT hr, IUnknown* piUnk, REFIID rid, bool bIgnorePositive = true); 

// Constructs an exception describing COM method call error.
// In contrast to TCheckErroEx an exception is returned as a result rather than
// being thrown.
// Use it to report an error withowt throwing an exception.
TComException* ConstructComException(HRESULT hr, IUnknown* pIUnknown, REFIID nIID);

class T_CLASS TComException : public TMcException, public CMNComException
{
    DECLARE_DYNAMIC(TComException)

protected:
	// [by CMNComException] CString			m_strSource;
    // [by CMNComException] GUID            m_nIID;
    // [by CMNComException] CString 		m_strComErrorMsg;

public:
    TComException(_com_error& rComError, UINT nType = MB_OK | MB_ICONERROR, UINT nErrorCode = IDE_T_COM_ERROR);
    TComException(TComException const& rException);
    TComException(CMNComException const& rException);
    virtual ~TComException();

	virtual BOOL    GetErrorMessage( LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL ) { return TMcException::GetErrorMessage(lpszError, nMaxError, pnHelpContext); }
	virtual HRESULT GetHRESULT() const { return CMNMcException::GetHRESULT(); }

	virtual void	Delete() { TException::Delete(); }

//  TException overrides
protected:
    virtual void InternalThrow();
    virtual void FormatPrefix (LPTSTR pszError, UINT nMaxError) const { CMNComException::FormatPrefix(pszError, nMaxError); }
	virtual void FormatMessage(LPCTSTR pcszFormat, LPTSTR lpszError, UINT nMaxError) const { CMNComException::FormatMessage(pcszFormat, lpszError, nMaxError); }
    virtual CString GetKeyName();
    virtual CMNException* Clone();

public:
//  operations
	// [by CMNComException] LPCTSTR	GetSource() {return m_strSource; }
	_declspec(property(get = GetSource)) LPCTSTR Source;

//  Implementation
protected:
//  Diagnistics
#if defined(_DEBUG) || defined(_AFXDLL)
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
};
