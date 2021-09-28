//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: MFCext
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Enhanced CWinApp, made to open some protected memnbers
//--------------------------------------------------------------------------------

#pragma once

#include "CMN/include/AfxImpl.h"

#include "T/include/Settings.h"
#include "T/include/T.h"

#pragma warning(disable : 4996) // 'AfxLoadLangResourceDLL': AfxLoadLangResourceDLL(LPCTSTR pszFormat) has been deprecated, 

extern LPCTSTR pcszLanguageSuffix;

//
//			TAppFusionInit
//
//
//class T_CLASS TAppFusionInit
//{
//protected:
//	HANDLE			m_hActCtx;
//	HMODULE			m_hModule;
//	int				m_resourceID;
//
//public:
//	TAppFusionInit(HMODULE hMod, int id);
//	virtual ~TAppFusionInit();
//
//private:
//	// The following is deliberately not implemented.
//	TAppFusionInit(const TAppFusionInit&); 
//	// The following is deliberately not implemented.
//	void operator=(const TAppFusionInit&);
//
//private:
//	void InitializeFromModuleID(HMODULE hMod, int id);
//	void FusionUninitialize();
//
//public:
//	HANDLE GetThemeContextHandle();
//};

//
//			TThemeContextActivator
//
//
//class T_CLASS TThemeContextActivator
//{
//private:
//     ULONG_PTR m_ulActivationCookie;
//
//public:
//    TThemeContextActivator(HANDLE hActCtx);
//    virtual ~TThemeContextActivator();
//};
//
//
//		TInterceptionWnd
//

class T_CLASS TWinAppBase
{
public:
	virtual void		DoBackgroundCursor(int nCode) = 0;

    virtual BOOL		CheckRestoreWaitCursor() = 0;
    virtual DWORD*		GetPromptContext() = 0;
    virtual int			GetWaitCursorCount() = 0;
    virtual int			GetBackgroundCursorCount() = 0;
    virtual HCURSOR		GetWaitCursorRestore() = 0;
    virtual HCURSOR		GetBackgroundCursorRestore() = 0;
	virtual LOGFONT&	GetLogFont() = 0;
	virtual TSettings::Ptr GetSettings() = 0;


    virtual BOOL		OnHelpInfo(HELPINFO* pHelpInfo) = 0;
};

class TInterceptionWnd: public CWnd
{
private:
	CWinApp*	m_pApp;
	HWND		m_hParent;

public:
	TInterceptionWnd(HWND hParent, CWinApp* pApp): m_hParent(hParent), m_pApp(pApp) {}
	
	BOOL Create()
	{
		if(!CWnd::CreateEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP, 0, 0, 0, 0, m_hParent, NULL, NULL))
			return FALSE;

		ShowWindow(SW_SHOW);
		return TRUE;
	}

	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);

	DECLARE_MESSAGE_MAP()
};

//
//			TWinApp
//

template<class BaseT>
class TWinAppT: public BaseT, public TWinAppBase
{
protected:
    LOGFONT			m_logfont;
	BOOL			m_bHelpInfoTrick;
//    TAppFusionInit* m_pFusionInit;
	TSettings::Ptr	m_spSettings;

	int				m_nBackgroundCursorCount;
	HCURSOR			m_hBackgroundCursor;

	typedef TWinAppT<BaseT>	WinApp;

public:
	static WinApp* GetApp() { return dynamic_cast<WinApp*>(AfxGetApp()); }

public:
    TWinAppT(LPCTSTR lpszAppName = NULL);

public:
	virtual int         DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);
    virtual HINSTANCE   LoadAppLangResourceDLL();
	virtual BOOL		InitInstance();
    virtual int			ExitInstance();
	virtual void		DoBackgroundCursor(int nCode);
	virtual void		DoWaitCursor(int nCode); // 0 => restore, 1=> begin, -1=> end

    // TWinApp virtual functions
    virtual BOOL OnHelpInfo(HELPINFO* pHelpInfo) { return FALSE; }

    // simple functions
    void        SetRegistryPath();
	void		SetRigistryProfileName(LPCTSTR lpszProfileName);
    BOOL        CheckRestoreWaitCursor();
    virtual DWORD* GetPromptContext()     { return &m_dwPromptContext; } // opening this member
    int         GetWaitCursorCount()   { return m_nWaitCursorCount; }
    int         GetBackgroundCursorCount()   { return m_nBackgroundCursorCount; }
    HCURSOR     GetWaitCursorRestore() { return m_hcurWaitCursorRestore; }
    HCURSOR     GetBackgroundCursorRestore() { return m_hBackgroundCursor; }
    virtual LOGFONT& GetLogFont()           { return m_logfont; }
	TSettings::Ptr GetSettings() { return m_spSettings; }
	//void EnableWorkingInBackgroundCursor(bool bEnable) { m_bWorkingInBackgroundCursor = bEnable; }

	// MMC don't allow to process WM_HELP from AfxMessageBox(),
	// so we made some trick. To enable it call EnableHelpInfoTrick and
	// override OnHelpInfo() function in you TWinApp-based snap-in.
	void EnableHelpInfoTrick(BOOL bEnable) { m_bHelpInfoTrick = bEnable; }	
	virtual UINT GetManifestID() const{ return 1000; }
//	HANDLE GetFusionInitHandle();
};

//
//      TWinApp class
//

template<class BaseT>
TWinAppT<BaseT>::TWinAppT(LPCTSTR lpszAppName):
	m_bHelpInfoTrick(FALSE),
//	m_pFusionInit(NULL),
	m_hBackgroundCursor(0),
	m_nBackgroundCursorCount(0)
{
	m_spSettings = new TSettings;
}


//////////////////////////////////////////////////
// This overriden fuction allow (if enabled)
// to process WM_HELPINFO message from
// message box created with AfxMessageBox() 
//////////////////////////////////////////////////
template<class BaseT>
int TWinAppT<BaseT>::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
	if(!m_bHelpInfoTrick)
		// trick is disabled, default processing
		return BaseT::DoMessageBox(lpszPrompt, nType, nIDPrompt);

	// Make app's window modal
	EnableModeless(FALSE);
	//HWND hParent = AfxGetMainWnd()->GetSafeHwnd();
	HWND hWndTop = NULL;
	HWND hParent = CWnd::GetSafeOwner_(NULL, &hWndTop);
	BOOL bReenableParent = FALSE;

	if(hParent && hParent != ::GetDesktopWindow() && ::IsWindowEnabled(hParent))
	{
		EnableWindow(hParent, FALSE);
		bReenableParent = TRUE;
	}

	// Creating "intermediate parent" window between
	// app window and message box.
	TInterceptionWnd IntWnd(hParent, this);
	BOOL bIntercepted = IntWnd.Create();

	IntWnd.CenterWindow();
	int nResult = CWinApp::DoMessageBox(lpszPrompt, nType, nIDPrompt);

	// re-enable app window
	if (bReenableParent)
		::EnableWindow(hParent, TRUE);

	// destroy the intercepter window
	if(bIntercepted)
		IntWnd.DestroyWindow();

	EnableModeless(TRUE);

	return nResult;
}

template<class BaseT>
HINSTANCE TWinAppT<BaseT>::LoadAppLangResourceDLL()
{
//  code taken from CWinApp::LoadAppLangResourceDLL()
	TCHAR szFormat[MAX_PATH];
	LPTSTR pszExtension;

	int ret = ::GetModuleFileName(m_hInstance, szFormat, MAX_PATH);
	if(ret == 0 || ret == MAX_PATH)
	{
		ASSERT(FALSE);
		return NULL;
	}
	pszExtension = ::PathFindExtension(szFormat);
	
	
	TCHAR szExt[] = _T("%s.dll");
	if((pszExtension - szFormat + _countof(szExt)) > MAX_PATH)
	{
		ASSERT(FALSE);
		return NULL;
	}
	
	lstrcpy(pszExtension, szExt);
//  end of code 

    CString strSuffix = GetProfileString(_T(""), pcszLanguageSuffix);
    if ( !strSuffix.IsEmpty() )
    {        
//  code taken from _AfxLoadLangDLL(LPCTSTR pszFormat, LCID lcid)
      	TCHAR szLangDLL[_MAX_PATH+14];
        //TCHAR szLangCode[4];
	    HINSTANCE hInstance;

        //_tcsncpy ( szLangCode, strSuffix, sizeof(szLangCode)/sizeof(szLangCode[0])-1);

  	    int ret = _sntprintf(szLangDLL, _countof(szLangDLL), szFormat, strSuffix /*szLangCode*/);
	    if(ret == -1 || ret >= _countof(szLangDLL))
	    {
		    ASSERT(FALSE);
		    return NULL;
	    }	
	    hInstance = ::LoadLibrary(szLangDLL);
        if (hInstance)
   	        return hInstance;
//  end of code
    }

  	return AfxLoadLangResourceDLL(szFormat);
}

template<class BaseT>
BOOL TWinAppT<BaseT>::InitInstance()
{
	if(FALSE == BaseT::InitInstance())
		return FALSE;

	m_hBackgroundCursor = ::LoadCursor(NULL, IDC_APPSTARTING);

	return TRUE;
}

template<class BaseT>
int TWinAppT<BaseT>::ExitInstance()
{ 
	//if(m_pFusionInit)
	//	delete m_pFusionInit;

	return BaseT::ExitInstance();
}

template<class BaseT>
void TWinAppT<BaseT>::DoWaitCursor(int nCode)
{
	// 0 => restore, 1=> begin, -1=> end
	ENSURE_ARG(nCode == 0 || nCode == 1 || nCode == -1);
	ENSURE(afxData.hcurWait != NULL);
	AfxLockGlobals(CRIT_WAITCURSOR);
	m_nWaitCursorCount += nCode;
	if(m_nWaitCursorCount > 0)
	{
		HCURSOR hcurPrev = 0;
		//if(m_bWorkingInBackgroundCursor)
		//	hcurPrev = ::SetCursor(m_hBackgroundCursor);
		//else
		hcurPrev = ::SetCursor(afxData.hcurWait);
		if(nCode > 0 && m_nWaitCursorCount == 1)
			m_hcurWaitCursorRestore = hcurPrev;
	}
	else
	{
		// turn everything off
		m_nWaitCursorCount = 0;     // prevent underflow
		//::SetCursor(m_hcurWaitCursorRestore);
		DoBackgroundCursor(0); // restore working in background cursor
	}
	AfxUnlockGlobals(CRIT_WAITCURSOR);
}

template<class BaseT>
void TWinAppT<BaseT>::DoBackgroundCursor(int nCode)
{
	// 0 => restore, 1=> begin, -1=> end
	ENSURE_ARG(nCode == 0 || nCode == 1 || nCode == -1);
	ENSURE(m_hBackgroundCursor != NULL);
	AfxLockGlobals(CRIT_WAITCURSOR);
	//TRACE(_T("[TWinAppT<BaseT>::DoBackgroundCursor]: nCode = %d\n"), nCode);
	m_nBackgroundCursorCount += nCode;
	if(m_nBackgroundCursorCount > 0)
	{
		HCURSOR hcurPrev = 0;
		hcurPrev = ::SetCursor(m_hBackgroundCursor);

		if(nCode > 0 && !m_hcurWaitCursorRestore)
			m_hcurWaitCursorRestore = hcurPrev;
	}
	else
	{
		// turn everything off
		m_nBackgroundCursorCount = 0;     // prevent underflow
		//ASSERT(m_hcurWaitCursorRestore);
		if(m_hcurWaitCursorRestore)
			::SetCursor(m_hcurWaitCursorRestore);
		m_hcurWaitCursorRestore = 0;
	}
	AfxUnlockGlobals(CRIT_WAITCURSOR);
}

template<class BaseT>
void TWinAppT<BaseT>::SetRegistryPath()
{
	/*
    //  Set registry path
    TVersionInfo versionInfo;
    versionInfo.GetFileVersionInfo(m_hInstance); // get self file version info
    SetRegistryKey(AFX_IDS_COMPANY_NAME);   // "CURRENT_USER\Softwate\AFX_IDS_COMPANY_NAME\AFX_IDS_APP_TITLE"
    // correct m_pszProfileName variable, strip program suffixes
    ::free( (void*)m_pszProfileName );
    CString str;
    str.Format (_T("%s\\%u.%u"), m_pszAppName, versionInfo.MajorHi(), versionInfo.MajorLo());
    m_pszProfileName = _tcsdup(str);
	*/
}

template<class BaseT>
void TWinAppT<BaseT>::SetRigistryProfileName(LPCTSTR lpszProfileName)
{
	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup(lpszProfileName);
}

template<class BaseT>
BOOL TWinAppT<BaseT>::CheckRestoreWaitCursor()
{
	if(GetWaitCursorCount() > 0) 
	{
		if(::GetCursor() != afxData.hcurWait)
		{            
			GetApp()->DoWaitCursor(0); // restore;
			TRACE(_T("[TWinAppT<BaseT>::CheckRestoreWaitCursor] wait cursor restored, count = %i \n"), GetApp()->GetWaitCursorCount()); 
			return TRUE; // stop further processing
		}
		return TRUE; // do not touch cursor until it has a hourglass form!
	}
	else if(GetBackgroundCursorCount() > 0)
	{
		if(::GetCursor() != m_hBackgroundCursor)
		{            
			GetApp()->DoBackgroundCursor(0); // restore;
			TRACE(_T("[TWinAppT<BaseT>::CheckRestoreWaitCursor] wait WorkingInBackgroundCursor restored, count = %i \n"), GetApp()->GetBackgroundCursorCount()); 
			return TRUE; // stop further processing
		}
		return TRUE; // do not touch cursor until it has a hourglass form!
	}

	return FALSE; 
}

//template<class BaseT>
//HANDLE TWinAppT<BaseT>::GetFusionInitHandle()
//{
//	if(m_pFusionInit == NULL)
//	{
//		m_pFusionInit = new TAppFusionInit(m_hInstance, static_cast<int>(GetManifestID()));
//		ASSERT(m_pFusionInit);
//	}
//
//	if(m_pFusionInit)
//		return m_pFusionInit->GetThemeContextHandle();
//
//	return NULL;
//}

typedef TWinAppT<CWinApp>	TWinApp;

T_FUNC TWinApp* T_API TGetApp();



