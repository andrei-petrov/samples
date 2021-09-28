//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Exceptions
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Exceptions class as an error text storage, 
//               taken from RSDN and refactored
//--------------------------------------------------------------------------------

#pragma once

#include "T/include/Exception.h"

/////////////////////////////////////////////////////////////////////////
//
//  TException
//

class T_CLASS TSimpleException : public TException
{
    DECLARE_DYNAMIC(TSimpleException)
//  Attributes
//    typedef CMNSmartPtr<TSimpleException>   Ptr;
protected :    
    CString         m_strText;                               

//  Operations
public :
//  Construction/Desctruction    
    TSimpleException(const char* pcszText);
    TSimpleException(const wchar_t* pcszText);
    TSimpleException(TSimpleException const& rException);

//  CException overrides
//    virtual BOOL    GetErrorMessage( LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL );    
protected:    
//  TException overrides
    virtual void    InternalThrow();
    virtual CMNException* Clone();

//  operations
    TException&     operator << (const int iArg);
    TException&     operator << (LPCTSTR pszArg);

//    virtual void    FormatMessage (LPCTSTR pcszFormat, LPTSTR pszError, UINT nMaxError) const;
};



//T_FUNC void  T_API   ThrowException(TSimpleException* pException); 
