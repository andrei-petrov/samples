//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Exceptions
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Com exceptions class (based on HRESULT and IErrorInfo 
//               provided by _com_error). Idea taken from CodeGuru and refactored
//--------------------------------------------------------------------------------

#include "stdafx.h"

#include "T/include/Constants.h"
#include "T/include/ComException.h"
#include "T/resource/resource.h"

/////////////////////////////////////////////////////////////////////////
//
//                      exception processing functions

void TSetComRaiseErrorHandler()
{
	::_set_com_error_handler(_t_raise_error);
}

void __stdcall _t_raise_error(const HRESULT hr, IErrorInfo* pIErrInfo)
{
	//_com_error  comerr(hr, pIErrinfo);
	//ThrowException(new TComException(comerr));

	if(!pIErrInfo)
		::GetErrorInfo(0, &pIErrInfo);

	_com_error  comerr(hr, pIErrInfo);
	ThrowException(new TComException(comerr));
}

/*
void __stdcall _t_issue_errorex (const HRESULT hr, IUnknown* pIUnknown, REFIID nIID)
{
	ThrowException(ConstructComException(hr, pIUnknown, nIID));
}

void __stdcall _t_issue_error (const HRESULT hr)
{
    _t_raise_error(hr, NULL);
}
*/

void T_API TCheckErrorEx (HRESULT hr, IUnknown* piUnk, REFIID rid, bool bIgnorePositive)
{
    UINT nType;
    if ( hr == S_OK )
        return;
    if ( FAILED(hr) )
        nType = MB_OK | /*MB_HELP | */MB_ICONSTOP;
    else if ( SUCCEEDED(hr) && bIgnorePositive == false ) // positive hr
        nType = MB_OK | /*MB_HELP | */MB_ICONEXCLAMATION;
    else
        return;

    try
    {
	    _com_issue_errorex(hr, piUnk, rid);
    }
	catch (TComException* e)
    {
        e->ModifyType(0xFFFFFFFF, nType);
        throw e;
	}
}

// Constructs an exception describing COM method call error.
// In contrast to TCheckErroEx an exception is returned as a result rather than
// be thrown.
// Use it to report an error withowt throwing an exception.
TComException* ConstructComException(HRESULT hr, IUnknown* pIUnknown, REFIID nIID)
{
	IErrorInfo *pIErrinfo = NULL;

	if (pIUnknown)
    {
        if (::GetErrorInfo(0, &pIErrinfo) == S_OK)
        {
            if (pIErrinfo) // thus we have it, let's ask server were we able to get it :)))))))))
            {
    		    ISupportErrorInfo* pIsei;
    		    if (SUCCEEDED(pIUnknown->QueryInterface(__uuidof(ISupportErrorInfo), reinterpret_cast<void**>(&pIsei))))
                {
    			    HRESULT hrSupports = pIsei->InterfaceSupportsErrorInfo(nIID);
    			    pIsei->Release();
                    if (hrSupports != S_OK)
                        pIErrinfo = NULL;
		        }
            }
        }
	}

	_com_error  comerr(hr, pIErrinfo);
	return new TComException(comerr);
}

/*
T_FUNC void T_API ThrowException(TComException* pException)
{
    pException->AddRef();
    TDumpException(pException);
    throw pException;
}
*/

/////////////////////////////////////////////////////////////////////////
//
//  TComException memthods
//

IMPLEMENT_DYNAMIC(TComException, TMcException)

TComException::TComException(_com_error& rComError, UINT nType, UINT nErrorCode):
	CMNException(nErrorCode),
	CMNMcException(rComError.Error(), nErrorCode),
	TMcException(rComError.Error(), nType, nErrorCode),
	CMNComException(rComError, nErrorCode)
{
    CString		        strMsg;

    if (ms_bExtendedReporting && FAILED(m_hr) && (nErrorCode == IDE_T_COM_ERROR))
        m_nErrorCode = IDE_T_COM_EXTENDED_ERROR;

//	m_nHelpContext		= rComError.HelpContext(); // com errors has no help contexts yet
	m_strSource			= (LPTSTR) rComError.Source();
    m_nIID              = rComError.GUID();
    if (m_strSource.GetLength() == 0)
        m_strSource     = _T("Unknown");
//	rComError.HelpFile(),

    // If there was an interface error we get a description, otherwise we get an ErrorMessage.
	m_strComErrorMsg	= (LPTSTR)rComError.Description();
	if (m_strComErrorMsg.IsEmpty())
		m_strComErrorMsg = rComError.ErrorMessage();

    // we do not need to have line feed
    int iLength = m_strComErrorMsg.GetLength();
	if (iLength)
	{
		if ( m_strComErrorMsg[iLength-1] == _T('\n'))
			m_strComErrorMsg.SetAt(iLength-1, NULL);
		if ( m_strComErrorMsg[iLength-2] == _T('\r'))
			m_strComErrorMsg.SetAt(iLength-2, NULL);
	}
    if (ms_bExtendedReporting && FAILED(m_hr))
    {
		TException::operator << (m_strSource);
    }
}

TComException::TComException(TComException const& rException):
	CMNException(rException),
	CMNMcException(rException),
    TMcException(rException),
	CMNComException(rException)
{
	m_strSource         = rException.m_strSource;
    m_nIID              = rException.m_nIID;
    m_strComErrorMsg    = rException.m_strComErrorMsg;
}

TComException::TComException(CMNComException const& rException):
	CMNException(rException),
	CMNMcException(rException),
    TMcException(rException),
	CMNComException(rException)
{
}

TComException::~TComException()
{
}

//                              virtual functions

void TComException::InternalThrow()
{
    throw this;
}

/*
void TComException::FormatMessage(LPCTSTR pcszFormat, LPTSTR lpszError, UINT nMaxError) const
{
    if (m_strComErrorMsg.GetLength())
    {
        TException::FormatMessage(pcszFormat, lpszError, nMaxError);
        UINT nLength = lstrlen(lpszError);
        lstrcpyn(lpszError + nLength, m_strComErrorMsg, nMaxError - nLength);
    }
    else // messages from our private .mc file
        TMcException::FormatMessage(pcszFormat, lpszError, nMaxError);
}
*/

CString TComException::GetKeyName()
{
    CString str;
    str.Format(_T("0x%08X"), m_nErrorCode);
    return str;
}

CMNException* TComException::Clone()
{
    return new TComException(*this);
}

#if defined (_DEBUG) || defined (_AFXDLL)

void TComException::AssertValid() const
{
    ASSERT(this != NULL);
}

void TComException::Dump(CDumpContext& dc) const
{
	LPOLESTR lpOleStr;
	StringFromIID(m_nIID, &lpOleStr);
    dc << "a " << GetRuntimeClass()->m_lpszClassName <<
    " at " << (void*)this   << 
    " m_hr = "              << m_hr <<
    " m_strSource = "       << m_strSource <<
    " m_nIID = "            << lpOleStr <<
    " m_strComErrorMsg = "  << m_strComErrorMsg;
    CoTaskMemFree(lpOleStr);			// Free lpOleStr.
}

#endif
