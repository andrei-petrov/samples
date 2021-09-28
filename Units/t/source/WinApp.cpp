//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: MFCext
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Enhanced CWinApp
//--------------------------------------------------------------------------------

#include "stdafx.h"

#include "T/include/WinApp.h"


LPCTSTR pcszLanguageSuffix = _T("LanguageSuffix");

//
//			TAppFusionInit
//
//
//TAppFusionInit::TAppFusionInit(HMODULE hMod, int id):
//	m_hActCtx(INVALID_HANDLE_VALUE),
//	m_hModule(hMod),
//	m_resourceID(id)
//{
//	InitializeFromModuleID(hMod,id);
//}
//
//TAppFusionInit::~TAppFusionInit()
//{
//	FusionUninitialize();
//}
//
//void TAppFusionInit::InitializeFromModuleID(HMODULE hMod, int id)
//{
//	TCHAR szPath[MAX_PATH];
//	if(0 == GetModuleFileName(hMod, szPath, sizeof(szPath) / sizeof(TCHAR)))
//		return;
//
//	ACTCTX act = {0};
//	act.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID;
//	act.lpResourceName = MAKEINTRESOURCE(id);
//
//	if(INVALID_HANDLE_VALUE == m_hActCtx)
//	{
//		act.cbSize = sizeof(act);
//		act.lpSource = szPath;
//
//		#ifdef UNICODE
//			m_hActCtx = CreateActCtxW(&act);
//		#else
//			m_hActCtx = CreateActCtxA(&act);
//		#endif // !UNICODE
//	}
//}
//
//void TAppFusionInit::FusionUninitialize()
//{
//	if(INVALID_HANDLE_VALUE != m_hActCtx)
//	{
//		ReleaseActCtx(m_hActCtx);
//		m_hActCtx = INVALID_HANDLE_VALUE;
//	}
//}
//
//HANDLE TAppFusionInit::GetThemeContextHandle()
//{
//	return m_hActCtx;
//}


//
//  TInterceptionWnd class (should be TInterceptionWnd)
//

BEGIN_MESSAGE_MAP(TInterceptionWnd, CWnd)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL TInterceptionWnd::OnHelpInfo(HELPINFO* pHelpInfo)
{
	TWinAppBase* pAppBase = dynamic_cast<TWinAppBase*>(m_pApp);
	ASSERT(pAppBase);

	// call virtual callback in TWinApp
	if(pAppBase && pAppBase->OnHelpInfo(pHelpInfo))
		return TRUE;

	// not required, default processing
	if(m_hParent && IsWindow(m_hParent))
	{
		if(0 != ::SendMessage(m_hParent, WM_HELP, 0, (LPARAM)pHelpInfo))
			return TRUE;
	}

	return CWnd::OnHelpInfo(pHelpInfo);
}

//
//			TThemeContextActivator
//
//
//TThemeContextActivator::TThemeContextActivator(HANDLE hActCtx):
//	m_ulActivationCookie(0)
//{                
//	try
//	{
//		ActivateActCtx(hActCtx, &m_ulActivationCookie);  
//	}
//	catch(...)
//	{
//		ASSERT(FALSE);
//	}
//}
//
//TThemeContextActivator::~TThemeContextActivator()
//{
//	try
//	{
//		if(m_ulActivationCookie != 0)
//			DeactivateActCtx(0, m_ulActivationCookie);
//	}
//	catch(...)
//	{
//		ASSERT(FALSE);
//	}
//}


T_FUNC TWinApp* T_API TGetApp()
{
    TWinApp* pApp = dynamic_cast <TWinApp*> (AfxGetApp());
    return pApp;
}
