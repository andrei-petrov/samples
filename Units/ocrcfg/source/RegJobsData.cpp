//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:  Configurator
//--------------------------------------------------------------------------
//  Description: Data for registering serivces
//--------------------------------------------------------------------------

#include "stdafx.h"

#include "ocrcfg/include/RegJobsData.h"


#pragma warning(disable: 4800)


CDeliceOcrJobDesc::CDeliceOcrJobDesc(LPCTSTR pszName, LPCTSTR pszDisplay)
{
	m_strName			= pszName;
	m_strDisplayName	= pszDisplay;
	m_bCheck			= true;
}


