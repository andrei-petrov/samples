//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: MFCext
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Extended COM message filter 
//--------------------------------------------------------------------------------


#pragma once

#include "T/include/T.h"


struct T_CLASS TMsgRange
{
	UINT m_nFirst;
	UINT m_nLast;
};


class T_CLASS TOleMessageFilter : public COleMessageFilter
{
public:
//    CCriticalSection    m_sect;
//    bool    m_bOnMessagePending;
    TMsgRange*          m_pRange;
    int                 m_nCount; // number of TMsgRange structures in m_pRange array
    static TMsgRange    ms_msgsDefBeingProcessed[];
    bool                m_bRestorePrevious;
    LPMESSAGEFILTER     m_lpPrevMessageFilter;

public:
    TOleMessageFilter();
    TOleMessageFilter(TMsgRange* pRange, int nCount);
    virtual ~TOleMessageFilter();
//  direct override, non-virtual functions with the same names exists in base class
    BOOL Register();
    void Revoke();   
//  COleMessageFilter virtuals
    virtual BOOL OnMessagePending(const MSG* pMsg);
    virtual int  OnNotRespondingDialog(HTASK htaskBusy);
//  simple functions
    void RestorePrevious(bool bRestore) { m_bRestorePrevious = bRestore; }
};



inline T_FUNC TOleMessageFilter* TGetMessageFilter()
{
    COleMessageFilter* pFilter = AfxOleGetMessageFilter();
    if (pFilter)
        return dynamic_cast <TOleMessageFilter*> (pFilter);
    else
        return NULL;
}
