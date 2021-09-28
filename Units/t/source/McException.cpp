//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Exceptions
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Exceptions class for HR codes with mc file as a message source,
//               idea taken from CodeGuru and refactored
//--------------------------------------------------------------------------------

#include "stdafx.h"
/*
#include <iads.h>
#include <adshlp.h>
#include <mapicode.h>
*/
#include "CMN/include/ComOle.h"

#include "T/include/Constants.h"
#include "T/include/SimpleException.h"
#include "T/include/McException.h"

#include "T/resource/resource.h"


void T_API TCheckError (HRESULT hr, bool bIgnorePositive, LPCSTR pszFile, int nLine)
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

	TMcException* pEx = new TMcException(hr, nType);
    ThrowException(pEx, pszFile, nLine);
}


void T_API TCheckIsError (BOOL bFuncResult, LPCSTR pszFile, int nLine)
{
	if (!bFuncResult)
	{
		TMcException* pExc = new TMcException(CMN::GetLastHRESULT());
		ThrowException(pExc, pszFile, nLine);
	}
}

#define STATUS_SUCCESS                  ((NTSTATUS)0x00000000L)

void T_API TCheckNtStatus (NTSTATUS ntStatus, LPCSTR pszFile, int nLine)
{
	if (ntStatus != STATUS_SUCCESS)
	{
		TMcException* pExc = new TMcException(CMN::E2HR(::LsaNtStatusToWinError(ntStatus)));
		ThrowException(pExc, pszFile, nLine);
	}
}

void T_API TCheckWIN32Error (long lWIN32Error, LPCSTR pszFile, int nLine)
{
	if (lWIN32Error != ERROR_SUCCESS)
	{
		TMcException* pExc = new TMcException(CMN::E2HR(lWIN32Error));
		ThrowException(pExc, pszFile, nLine);
	}
}
/*
void T_API TCheckADSError (HRESULT hr, LPCSTR lpszFile, int nLine)
{
    if (FAILED(hr))
	{	
		DWORD dwLastError = ERROR_SUCCESS;
		WCHAR szErrorBuf[MAX_PATH];
		WCHAR szNameBuf[MAX_PATH];
		//Get extended error value.
		HRESULT hr_return = S_OK;
        
        if (HRESULT_FACILITY(hr) == FACILITY_WIN32)
        {
		    hr_return = ::ADsGetLastError( &dwLastError,
			    						   szErrorBuf,
				    					   MAX_PATH-1,
					    				   szNameBuf,
						    			   MAX_PATH-1);
        
		    if ( SUCCEEDED(hr_return) && dwLastError != ERROR_SUCCESS)
            {
                TMcException* pExc;
                if (TException::ms_bExtendedReporting)
                {
                    pExc = new TMcException(CMN::E2HR(dwLastError), MB_OK | MB_HELP | MB_ICONERROR, IDE_T_LDAP_EXT_ERROR);
                    *pExc << szNameBuf;
                    *pExc << szErrorBuf;
                }
                else
                {
                    TTrace(traceAll, L"ADSI error: %s\n", szErrorBuf);
                    pExc = new TMcException(CMN::E2HR(dwLastError));
                }
                ThrowException(pExc, lpszFile, nLine);
            }
        }
        TCheckError(hr, false, lpszFile, nLine);
	}
}
*/

TMcException* T_API THasHRESULT(TException* pHiLevelEx, HRESULT hr, bool bFindAnyPositive, bool bFindAny)
{
    CMNException* pEx = pHiLevelEx;
    do
    {
        CMNMcException* pMcEx = dynamic_cast <CMNMcException*> (pEx);
		TMcException* pTMcEx = dynamic_cast<TMcException*>(pEx);
        if (pMcEx)
        {
            HRESULT hres = pMcEx->GetHRESULT();
            if (bFindAny && pTMcEx)
                return pTMcEx;

            if (bFindAnyPositive)
            {
                if (SUCCEEDED(hres))
                    return pTMcEx;
            }
            else
            {
                if (hres ==  hr) // we have found the particular HRESULT value
                    return pTMcEx;
            }
        }
    }
    while (pEx = (CMNException*) pEx->GetNext());

    return NULL;
}


CString TFormatString (HRESULT hr, va_list argList)
{
    TCHAR szBuf[4096];
    TMcException::FormatString(hr, szBuf, sizeof(szBuf), argList);
    return szBuf;
}

CString TFormatString (HRESULT hr, ...)
{
    va_list  argList;
    va_start(argList, hr);
    CString str = TFormatString(hr, argList);
    va_end(argList);
    return str;
}

T_FUNC void T_API ThrowException(TMcException* pException)
{
    throw pException;
}


/////////////////////////////////////////////////////////////////////////
//
//                          TMcException
//

//TArrayHModule TMcException::ms_arrHModules;

IMPLEMENT_DYNAMIC(TMcException, TException)


TMcException::TMcException(HRESULT hr, UINT nType, UINT nErrorCode) :
	CMNException(nErrorCode),
    TException(nErrorCode, nType, NULL, NULL),
	CMNMcException(hr, nErrorCode)
{
	if (ms_bExtendedReporting && FAILED(m_hr) && (nErrorCode == IDE_T_MC_ERROR))
		m_nErrorCode = IDE_T_MC_EXTENDED_ERROR;

	if (ms_bExtendedReporting && FAILED(m_hr))
	{
		TException::operator << (m_hr);
	}
}

TMcException::TMcException(CMNMcException const& rException):
	CMNException(rException),
	TException(rException),
	CMNMcException(rException)
{
}

TMcException::TMcException(TMcException const& rException) :
	CMNException(rException),
    TException(rException),
	CMNMcException(rException)
{
    m_hr = rException.m_hr;
}

//                              virtual fucntions

void TMcException::InternalThrow()
{
    throw this;
}

/*
void TMcException::FormatMessage(LPCTSTR pcszFormat, LPTSTR lpszError, UINT nMaxError) const
{
    TException::FormatMessage(pcszFormat, lpszError, nMaxError);
    UINT nLength = lstrlen(lpszError);

    va_list argList = NULL;
	if(!m_bufArgList.empty())
		argList = (va_list) &(m_bufArgList[0]); // language and processor specific

    if (ms_bExtendedReporting)
        argList += sizeof(int);    // we have first hidden integer argument (m_nErrorCode)
    if (ms_bExtendedReporting && FAILED(m_hr))
        argList += sizeof(int);    // we have second hidden integer argument (m_hr)

    FormatString(m_hr, lpszError + nLength, nMaxError - nLength, argList);
}
*/

CMNException* TMcException::Clone()
{
    return new TMcException(*this);
}


//                             simple functions

TMcException& TMcException::operator << (LPCTSTR pcszArg)
{
	TException::operator << (pcszArg);
    return *this;
}
/*
HMODULE T_API TMcException::AddModule(LPCTSTR pcszModuleName)
{
    HMODULE m_hModule = ::LoadLibraryEx( pcszModuleName, NULL, LOAD_LIBRARY_AS_DATAFILE );
    if (m_hModule)
        ms_arrHModules.Add(m_hModule);
    return m_hModule;
}

void TMcException::ReleaseAllModules()
{
    for(int i=0; i<ms_arrHModules.GetSize(); i++)
        ::FreeLibrary(ms_arrHModules[i]);
    ms_arrHModules.SetSize(0);
}
*/

/////////////////////////////////////////////////////////////////////////
//
//                      Implementation
//

void TMcException::FormatString(HRESULT hr, LPTSTR lpszError, UINT nMaxError, va_list argList)
{
	DWORD dwCode = 0; //HRESULT_CODE(hr);
	BOOL bOk = FALSE;

	if ((HRESULT_FACILITY(hr) == FACILITY_WIN32))    
		dwCode = HRESULT_CODE(hr); // convert to legacy error code  	

    HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(1), RT_MESSAGETABLE);
	if (!bOk) // try from our module    
    {
        bOk = ::FormatMessage(FORMAT_MESSAGE_FROM_HMODULE,
			                hInst, // current hmodule
			                hr,    // argument (error code)
			                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			                lpszError,
			                nMaxError,
                            (char**)&argList);
    }
	if (!bOk && dwCode) // try from system modules    
        bOk = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                            NULL,
                            dwCode, // try old-style code
                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                            lpszError,
                            nMaxError,
                            NULL );	
	if (!bOk) 
        bOk = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			                NULL,
			                hr,  // try hr
			                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			                lpszError,
			                nMaxError,
			                NULL );
	if (!bOk) 
    {
        for(int i = 0; i < ms_vecHModules.size() && !bOk; i++)
        {               
            bOk = ::FormatMessage(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
		                        ms_vecHModules[i],  // current hmodule
			                    hr,
			                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			                    lpszError,
		                        nMaxError,
			                    NULL );
            if (!bOk && dwCode)
            {
                bOk = ::FormatMessage(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
		                            ms_vecHModules[i],  // current hmodule
			                        dwCode,
			                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			                        lpszError,
		                            nMaxError,
			                        NULL );
            }
        }
	}
	if (!bOk)         
    {
        UINT nID = ms_bExtendedReporting ? IDS_T_MC_EXTENDED_UNKNOWN_ERROR : IDS_T_MC_UNKNOWN_ERROR;
        CString strFormat;
        strFormat.LoadString(nID);
        _sntprintf(lpszError, nMaxError, (LPCTSTR)strFormat, hr);
//        AfxLoadString(IDE_T_UNKNOWN_ERROR, lpszError, nMaxError);
    }
}

TMcException& TMcException::operator << (const int iArg)
{
	TException::operator << (iArg);
    return *this;
}

/*
// Searches for last TMcException in the chain of exceptions.
// Returns HRESULT of an exception or S_OK is exception is not found.
HRESULT TMcException::FindMcHR(TException *pEx)
{
	HRESULT hr = S_OK;
	TException* pHiLevelEx = pEx;
	do
	{
		TMcException* pMcEx = dynamic_cast <TMcException*> (pHiLevelEx);
		if (pMcEx)
			hr = pMcEx->GetHRESULT();
	}
	while (pHiLevelEx && (pHiLevelEx = (TException*)pHiLevelEx->GetNext()));

	return hr;
}
*/

