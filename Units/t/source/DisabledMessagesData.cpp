//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Exceptions
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Disabled Messages helper structure
//--------------------------------------------------------------------------------

#include "stdafx.h"

// Headers from T module
#include "T/Include/Misc.h"
#include "T/Include/DisabledMessagesData.h"

///////////////////////////////////////////////////////////////////////////////////////////
//
//                              TDisabledMessagesData
//


IMPLEMENT_SERIAL (TDisabledMessagesData, TObject ,VERSIONABLE_SCHEMA)
IMPLEMENT_SMARTPTR_SERIAL(TDisabledMessagesData)    

TDisabledMessagesData::TDisabledMessagesData()
{
  	//{{AFX_DATA_INIT(EDMCredentialsData)
  	//}}AFX_DATA_INIT
}

TDisabledMessagesData::~TDisabledMessagesData()
{
}

// Overrides    
void TDisabledMessagesData::Serialize (CArchive& ar)
{
	ar.SerializeClass( RUNTIME_CLASS(TDisabledMessagesData) ); 
    m_mapDisabledMessages.Serialize(ar);
}   

int  TDisabledMessagesData::IsMessageDisabled(UINT nMessageID)
{
    int nResultCode = 0;
    if (m_mapDisabledMessages.Lookup(nMessageID, nResultCode) == 0)
        return 0; //Message not disabled
    else 
        return nResultCode;
}

bool TDisabledMessagesData::EnableMessage(UINT nMessageID)
{
    int nResultCode = 0;
    if (m_mapDisabledMessages.Lookup(nMessageID, nResultCode) == 0)
        return false;
    else
        m_mapDisabledMessages.RemoveKey(nMessageID);
    return true;
}

bool TDisabledMessagesData::DisableMessage(UINT nMessageID, int nCode)
{
    int nResultCode = 0;
    if (m_mapDisabledMessages.Lookup(nMessageID, nResultCode) == 0)
        m_mapDisabledMessages.SetAt(nMessageID, nCode);
    else
        return false;
    return true;
}

bool TDisabledMessagesData::operator== (TObject const& rObject) const
{
    TDisabledMessagesData* pObj = dynamic_cast<TDisabledMessagesData*>(&const_cast<TObject&>(rObject));
    ASSERT(pObj);
    if (pObj)
    {    
        if ( pObj->m_mapDisabledMessages.GetCount() != m_mapDisabledMessages.GetCount() )
            return false;
        else
        {
            POSITION pos;
            int nKey   = 0;
            int nValue = 0;
            int nValue2 = 0;
            for( pos = m_mapDisabledMessages.GetStartPosition(); pos != NULL; )
            {
                m_mapDisabledMessages.GetNextAssoc( pos, nKey, nValue );
                BOOL bRes = pObj->m_mapDisabledMessages.Lookup( nKey, nValue2 );
                if (bRes == false || nValue != nValue2)
                    return false;
            }
            return true;
        }
    }    
    return false;
}