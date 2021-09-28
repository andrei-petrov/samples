//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Misc
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: context help structure and macroses, taken from MSJ
//--------------------------------------------------------------------------------

#pragma once

#include "T/include/T.h"


#define MAKEHELPID(a, b) ((DWORD)(0x80000000 | ((DWORD)(a) << 16) | ((DWORD)(b)) ))

struct T_CLASS THelpInfo
{
	DWORD CtrlID;
	DWORD HelpID;    
	THelpInfo(DWORD x, DWORD y);    
};


