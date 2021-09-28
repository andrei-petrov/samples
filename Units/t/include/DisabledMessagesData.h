//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Exceptions
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Disabled Messages helper structure, taken from RSDN and refactored
//--------------------------------------------------------------------------------

#pragma once

#include "T/Include/Object.h"
#include "T/Include/Settings.h"
#include "T/include/Constants.h"
#include "resource.h"

#define DECLSTUB

class T_CLASS  TDisabledMessagesData : public TObject
{
	DECLARE_SERIAL (TDisabledMessagesData)
	DECLARE_SMARTPTR_SERIAL(TDisabledMessagesData, DECLSTUB) 

  	typedef TDisabledMessagesData            Class;
    typedef CMNSmartPtr <Class>                   Ptr;    
    typedef CMap<int, int, int, int> CMapIntToInt;

public:
    //{{AFX_DATA(TDisabledMessagesData)
    CMapIntToInt m_mapDisabledMessages;
    //}}AFX_DATA        
public:
    TDisabledMessagesData();
    ~TDisabledMessagesData();

//virtual 
    virtual bool operator== (TObject const& rObject) const;
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(TDisabledMessagesData)
    virtual void Serialize (CArchive& ar);
    //}}AFX_VIRTUAL    
    int  IsMessageDisabled(UINT nMessageID);
    bool EnableMessage(UINT nMessageID);
    bool DisableMessage(UINT nMessageID, int nCode = IDYES);
};
