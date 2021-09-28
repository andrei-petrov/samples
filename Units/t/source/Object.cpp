//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Ptr
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------------

#include "StdAfx.h"

#include "T/Include/Object.h"

IMPLEMENT_SERIAL (TObject, CObject, TOBJECT_SCHEMA)
IMPLEMENT_SMARTPTR_SERIAL(TObject)

//-------------------------------------------------------------------------------------------
//                            FUNCTIONS
//-------------------------------------------------------------------------------------------

bool TObject::operator== (TObject const& rObject) const
{
    ASSERT(false);
    return false;
}

void TObject::Serialize (CArchive& ar)
{   
}


#if defined (_DEBUG) || defined (_AFXDLL)

void TObject::Dump (CDumpContext& dc) const
{    
    dc << "A " << GetRuntimeClass()->m_lpszClassName << " at " << (void*)this;
    dc << "m_nCnt = "   << m_nCnt;
}

#endif
