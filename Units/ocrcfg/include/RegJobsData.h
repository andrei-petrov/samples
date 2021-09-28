//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:  Configurator      
//--------------------------------------------------------------------------
//  Description: Data for registering serivces
//--------------------------------------------------------------------------

#pragma once

#include "cmn/include/Unknown.h"
#include "db/include/Storage.h"


class CDeliceOcrJobDesc : public CMNUnknown
{
public:
	typedef CMNSmartPtr<CDeliceOcrJobDesc>		Ptr;
	typedef vector<Ptr>							Vector;

	CString			m_strName;
	CString			m_strDisplayName;
	bool			m_bCheck;

	CDeliceOcrJobDesc(LPCTSTR pszName, LPCTSTR pszDisplayName);
};



