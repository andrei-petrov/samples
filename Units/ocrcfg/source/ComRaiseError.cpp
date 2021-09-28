//--------------------------------------------------------------------------------
//  Copyright (c) by Andrey Petrov, 2008-2009
//  All rights reserved. 
//--------------------------------------------------------------------------------
//  Unit:        vussvc
//  Cooperation: Exceptions
//  Date:        2008-11-15
//--------------------------------------------------------------------------------
//  Description: _com_error catcher
//--------------------------------------------------------------------------------

#include "stdafx.h"
//#include "vussvc/include/log.h"
#include "T/Include/ComException.h"


//
//	Here we override ATL exception throw
//
/*
void WINAPI AtlThrowImpl( HRESULT hr )
{
	ATLTRACE(atlTraceException, 0, _T("AtlThrow: hr = 0x%x\n"), hr );
	_cmn_issue_error(hr);
}
*/
//#include "T/source/ComRaiseError.cpp"
#include "T/source/ThrowExceptionImpl.cpp"
