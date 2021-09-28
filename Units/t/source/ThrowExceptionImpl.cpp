//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2010 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Exceptions
//  Date:        2010-09-22
//--------------------------------------------------------------------------------
//  Description: This file must be include in one .cpp (ComRaiseError.cpp)
//               file of each .dll or .exe project
//--------------------------------------------------------------------------------

#ifdef CUSTOM_THROW_EXCEPTION

#include "CMN/include/Exception.h"
#include "CMN/include/ComException.h"
#include "CMN/include/McException.h"
#include "CMN/include/SeException.h"

#include "T/include/Exception.h"
#include "T/include/ComException.h"
#include "T/include/McException.h"
#include "T/include/SimpleException.h"


__declspec (noreturn) void _stdcall ThrowException(CMNException* pException, LPCSTR pszFile, int nLine)
{
	ASSERT(pException);

	TException* pEx = dynamic_cast<TException*>(pException);
	if(!pEx)
	{
		CMNComException* pComEx = dynamic_cast<CMNComException*>(pException);
		if(pComEx)
			pEx = new TComException(*pComEx);
		else
		{
			CMNMcException* pMcEx = dynamic_cast<CMNMcException*>(pException);
			if(pMcEx)
				pEx = new TMcException(*pMcEx);
			else
				pEx = new TException(*pException);
		}

		delete pException;
	}

	TDumpException(pEx, pszFile, nLine);
	pEx->InternalThrow();
}

void _stdcall DeleteException(CMNException* pException)
{
	ASSERT(pException);
	if(!pException)
		return ;

	pException->Delete();
}

#endif
