//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Misc
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: context help structure and macroses, taken from MSJ
//--------------------------------------------------------------------------------

#include "stdafx.h"
#include "T/include/HelpInfo.h"


THelpInfo::THelpInfo(DWORD x, DWORD y) 
{
    CtrlID = (USHORT)x;
    HelpID = y;
}


