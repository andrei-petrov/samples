//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: MFCext
//  Date:        2009-11-26
//--------------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------------

#include "StdAfx.h"

#include "T/include/WinApp.h"
#include "T/include/WaitCursor.h"


TWaitCursor::TWaitCursor(TWAIT_CURSOR enWaitType):
	m_enWaitType(enWaitType)
{
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp);
	if(pApp)
	{
		TWinAppBase* pAppBase = dynamic_cast<TWinAppBase*>(pApp);
		ASSERT(pAppBase);
		if(m_enWaitType == WAIT)
			pApp->BeginWaitCursor();
		else if(pAppBase && m_enWaitType == BACKGROUND)
			pAppBase->DoBackgroundCursor(1);
	}
}

TWaitCursor::~TWaitCursor()
{
	AFX_BEGIN_DESTRUCTOR

	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp);
	if(pApp)
	{
		TWinAppBase* pAppBase = dynamic_cast<TWinAppBase*>(pApp);
		ASSERT(pAppBase);
		if(m_enWaitType == WAIT)
			pApp->EndWaitCursor();
		else if(m_enWaitType == BACKGROUND)
			pAppBase->DoBackgroundCursor(-1);
	}

	AFX_END_DESTRUCTOR
}

void TWaitCursor::Restore()
{
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp);
	if(pApp)
		pApp->RestoreWaitCursor();
}
