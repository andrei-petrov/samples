//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Ptr
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description:   MFC class with reference count, idea taken from CodeGuru
//--------------------------------------------------------------------------------

#pragma once

// compiler vendor files
// 3-d party libraries
// includes from other modules
// includes from this module
// its own header

#include "CMN/Include/SmartPtr.h"
#include "CMN/include/Unknown.h"

#include "T/include/T.h"

#define TOBJECT_SCHEMA  1


class T_CLASS TObject : virtual public CMNUnknown, public CObject
{
    DECLARE_SERIAL (TObject)
	DECLARE_SMARTPTR_SERIAL(TObject, T_CLASS)

public:
	typedef CMNSmartPtr<TObject>	Ptr;
    TObject() {}
    virtual ~TObject() {}

public:
    virtual bool operator== (TObject const& rObject) const;
    virtual void Serialize (CArchive& ar);

#if defined (_DEBUG) || defined (_AFXDLL)
public:
    // ?? virtual void        AssertValid () const;
    virtual void        Dump        (CDumpContext& dc) const;    
#endif
};
