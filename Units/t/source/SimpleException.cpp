//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Exceptions
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Exceptions class as an error text storage 
//               Idea taken from CodeGuru and refactored
//--------------------------------------------------------------------------------

#include "stdafx.h"
#include "T/include/SimpleException.h"

/////////////////////////////////////////////////////////////////////////
//
//  TSimpleException class
//

IMPLEMENT_DYNAMIC(TSimpleException, TException)

TSimpleException::TSimpleException(const char* pcszText):
	TException()
{
	m_strRetrievedErrorText	= pcszText;
}

TSimpleException::TSimpleException(const wchar_t* pcszText):
	TException()
{
	m_strRetrievedErrorText	= pcszText;
}

TSimpleException::TSimpleException(TSimpleException const& rException): 
    TException()
{
    m_strRetrievedErrorText = rException.m_strRetrievedErrorText;    
}


//                        virtual functions
/*
BOOL TSimpleException::GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext)
{
    lstrcpyn(lpszError, m_strText, nMaxError);    
    return TRUE;
}
*/

//  TExceptino overrides

void TSimpleException::InternalThrow()
{
    throw this;
}

CMNException* TSimpleException::Clone()
{
    return new TSimpleException(*this);
}


// operations

TException& TSimpleException::operator << (const int iArg)
{
    ASSERT(0);
    return *this;
}

TException& TSimpleException::operator << (LPCTSTR pszArg)
{
    ASSERT(0);
    return *this;
}

/*
void T_API ThrowException(TSimpleException* pException)
{     

    pException->AddRef();
    TDumpException(pException);
    throw pException;
}
*/