//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: MFCext
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Extended COM message filter 
//--------------------------------------------------------------------------------


#include "stdafx.h"

#include "CMN/include/AfxImpl.h"
#include "T/include/WinApp.h"
#include "T/include/OleMessageFilter.h"


TMsgRange TOleMessageFilter::ms_msgsDefBeingProcessed[] =
{
	{WM_PAINT,      WM_PAINT},    // 0x000F
    {WM_SETCURSOR,  WM_SETCURSOR},// 0x0020
	{WM_TIMER,      WM_TIMER},    // 0x0113 // Got it from tree-view. Seems nothing wrong to process.
    {WM_MOUSEMOVE,  WM_MOUSEMOVE} // 0x0200
	//{WM_QUIT,		WM_QUIT},
    //{WM_MOUSEFIRST,  WM_MOUSELAST} // 0x0200
};


TOleMessageFilter::TOleMessageFilter() :
    COleMessageFilter()
{
    m_pRange = ms_msgsDefBeingProcessed;
    m_nCount = _countof(ms_msgsDefBeingProcessed);
    m_bRestorePrevious    = false;
    m_lpPrevMessageFilter = NULL;
//    DWORD   dwThreadId = GetCurrentThreadId();
//    TRACE(_T("[TOleMessageFilter::TOleMessageFilter] this = 0x%0X, threadId = 0x%0X\n"), this, dwThreadId );
}


TOleMessageFilter::TOleMessageFilter(TMsgRange* pRange, int nCount) :
    COleMessageFilter()
{
    m_pRange = pRange;
    m_nCount = nCount;
    m_bRestorePrevious    = false;
    m_lpPrevMessageFilter = NULL;
//    DWORD   dwThreadId = GetCurrentThreadId();
//    TRACE(_T("[TOleMessageFilter::TOleMessageFilter] this = 0x%0X, threadId = 0x%0X\n"), this, dwThreadId );
}


TOleMessageFilter::~TOleMessageFilter()
{
	ASSERT_VALID(this);
	Revoke();
    if (m_lpPrevMessageFilter)
    {
        m_lpPrevMessageFilter->Release();
        m_lpPrevMessageFilter = NULL;
    }
//    DWORD   dwThreadId = GetCurrentThreadId();
//    TRACE(_T("[TOleMessageFilter::~TOleMessageFilter] this = 0x%0X, threadId = 0x%0X\n"), this, dwThreadId );
}


BOOL TOleMessageFilter::Register()
{
	ASSERT_VALID(this);
	ASSERT(!m_bRegistered); // calling Register twice?

    if (m_lpPrevMessageFilter)
    {
        m_lpPrevMessageFilter->Release();
        m_lpPrevMessageFilter = NULL;
    }
    LPMESSAGEFILTER* ppPrevMessageFilter;
    if (m_bRestorePrevious)
        ppPrevMessageFilter = &m_lpPrevMessageFilter;
    else
        ppPrevMessageFilter = NULL;

	if (::CoRegisterMessageFilter(&m_xMessageFilter, ppPrevMessageFilter) == S_OK)
	{
		m_bRegistered = TRUE;
		return TRUE;
	}
	return FALSE;
}


void TOleMessageFilter::Revoke()
{
	ASSERT_VALID(this);

	if (m_bRegistered)
	{
        ::CoRegisterMessageFilter(m_bRestorePrevious ? m_lpPrevMessageFilter : NULL, NULL);
		m_bRegistered = FALSE;
	}
}



//#define TRACE_FILTER

BOOL TOleMessageFilter::OnMessagePending(const MSG* pMsg)
{
    // By default we rely on OLE's default message handling for every message
    // except WM_PAINT messages.  WM_PAINT messages should not generate
    // out-going calls.
//    AFX_MANAGE_STATE(AfxGetStaticModuleState());    

    BOOL bEatMessage = FALSE;    
    MSG msg;
//    TRACE(_T("[TOleMessageFilter::OnMessagePending] \n"));

/*
    //HCURSOR hCursor = ::GetCursor(); //afxData.hcurWait;   
    if (::GetCursor() != afxData.hcurWait)
    {            
        TGetApp()->DoWaitCursor(0); // restore;
        TRACE(_T("[TWinApp::CheckRestoreWaitCursor] wait cursor restored, count = %i \n"), TGetApp()->GetWaitCursorCount()); 
        return TRUE; // stop further processing
    }
*/
	for (int i = 0; i < m_nCount; i++) 
	{
		UINT nMsgFirst = m_pRange[i].m_nFirst;
		UINT nMsgLast =  m_pRange[i].m_nLast;
		while (::PeekMessage(&msg, NULL, nMsgFirst, nMsgLast, PM_REMOVE | PM_NOYIELD)) // wait cursor can sometimes disappear during this call
		{
			bEatMessage = TRUE;
			LRESULT lres = DispatchMessage(&msg);            
            //TTRACE(traceMisc, _T("[TOleMessageFilter::OnMessagePending] message 0x%0x dispatched in thread 0x%0x with result code = %i \n"), msg.message, GetCurrentThreadId(), lres);
			//TRACE(_T("[TOleMessageFilter::OnMessagePending] message 0x%0x dispatched in thread 0x%0x with result code = %i \n"), msg.message, GetCurrentThreadId(), lres);
		}
	}

	// this code works in snap-in (not checked in dialogs)
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp);
	if(pApp)
	{
		TWinAppBase* pAppBase = dynamic_cast<TWinAppBase*>(AfxGetApp());
		ASSERT(pAppBase);
		//TRACE(_T("[TWinApp::CheckRestoreWaitCursor] wait cursor restored, count = %i \n"), pApp->GetWaitCursorCount()); 
		if (pAppBase && pAppBase->GetWaitCursorCount() && ::GetCursor() != afxData.hcurWait)
		{            
			pApp->DoWaitCursor(0); // restore;
			//TRACE(_T("[TWinApp::CheckRestoreWaitCursor] wait cursor restored, count = %i \n"), pApp->GetWaitCursorCount()); 
			return TRUE; // stop further processing
		}
	}

	/*
    if (afxData.hcurWait == hCursor);
        SetCursor(hCursor); // restore
    */
    return bEatMessage;
}


int TOleMessageFilter::OnNotRespondingDialog(HTASK htaskBusy)
{ 
    // this leads to the queue cleaning from WM_MOUSEFIRST to WM_MOUSELAST and
    // from WM_KEYFIRST to WM_KEYLAST in COleMessageFilter::XMessageFilter::MessagePending
    return IDOK; 
}
