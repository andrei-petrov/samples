//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Misc
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Misc
//--------------------------------------------------------------------------------

#include "stdafx.h"
#include <math.h>

#include <CMN/include/afximpl.h>  // Afx(Un)LockGlobals(CRIT_RUNTIMECLASSLIST);
#include "CMN/include/Log.h"
#include "CMN/include/PerformanceCounter.h"  
#include "T/include/Exception.h"
#include "T/include/ComException.h"
#include "T/include/Misc.h"

using namespace _com_util;

#define DF_UNICODE_MAXPATH 32767

namespace		// helpers
{
DATE GetLocalTimeBias()
{
	static
	const double g_dblMinutesToDays		= 1.0 / (60 * 24);	// coeff to make days from minutes

	LONG					lBias		= 0;
	TIME_ZONE_INFORMATION	tzi;
	DWORD					dwResult	= GetTimeZoneInformation(&tzi);

	// if TIME_ZONE_ID_INVALID or something other is returned - no bias
	switch (dwResult)
	{
		case TIME_ZONE_ID_STANDARD:							// daylight saving is switched OFF
			lBias = tzi.Bias + tzi.StandardBias;
			break;

		case TIME_ZONE_ID_DAYLIGHT:							// daylight saving is switched ON
			lBias = tzi.Bias + tzi.DaylightBias;
			break;

		case TIME_ZONE_ID_UNKNOWN:							// unknown, just use default bias
			lBias = tzi.Bias;
			break;

		case TIME_ZONE_ID_INVALID:							// some internal error
			ASSERT(false);
	}

	DATE		dateBias(lBias * g_dblMinutesToDays);
	return dateBias;

	// P.S. Possibly we should test (tzi.DaylightDate.wMonth != 0) && (tzi.DaylightBias != 0) for DST mode
}

}				// anonymous

//T_DATA TDateTime    g_operDate;
//T_DATA TWorkingMode g_nWorkingMode;


bool T_API TIsUnicode(const void* pv, int nSize)
{
	// signature may absent, but if it is present IS_TEXT_UNICODE_ASCII16 test can fail
	static const int	nMinimumOptions	= IS_TEXT_UNICODE_UNICODE_MASK & ~(IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_ASCII16);

	int		nOptions	= IS_TEXT_UNICODE_UNICODE_MASK;
	BOOL	bUnicode	=
		IsTextUnicode(pv, min(nSize, 1024), &nOptions) &&
		(nOptions & nMinimumOptions) == nMinimumOptions;
	return bUnicode != 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
//                                      Debug helpers
//

//T_DATA CDumpContext g_debugDump;


// returns "FileNamed.ext" instead of "FileName.ext"
CString T_API TGetDebugName(const CString& strFileName)
{
    TCHAR szDrive  [_MAX_DRIVE];
    TCHAR szDir    [_MAX_DIR];
    TCHAR szFname  [_MAX_FNAME];
    TCHAR szExt    [_MAX_EXT];
    TCHAR szPath   [_MAX_PATH];

    _tsplitpath(strFileName, szDrive, szDir, szFname, szExt);    
    int iStrLen = lstrlen(szFname);
    szFname[iStrLen] = 'd';
    szFname[iStrLen+1] = 0;    
    _tmakepath(szPath, szDrive, szDir, szFname, szExt);

    return CString(szPath);
}


///////////////////////////////////////////////////////////////////////////////////////////////
//
//                                      _int64 functions

#ifdef _AFXDLL
////////////////////////////////////////////////////////////////////////////
// CRuntimeClass search functions

CRuntimeClass* T_API TFindRuntimeClass(const CString& strClassName)
{	
    CRuntimeClass* pFoundClass = 0;
    // just walk through the simple list of registered classes
    AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
    AfxLockGlobals(CRIT_RUNTIMECLASSLIST);
    CRuntimeClass* pClass;
    for (pClass = pModuleState->m_classList; pClass != NULL;
         pClass = pClass->m_pNextClass)
    {
		// note that pClass->m_lpszClassName has a LPCSTR ANSI type 
        if ( _tcscmp( CA2CT(pClass->m_lpszClassName), strClassName) == 0 )
        {
            pFoundClass = pClass;
            break;
        }
    }
    AfxUnlockGlobals(CRIT_RUNTIMECLASSLIST);

    if (pFoundClass)
        return pFoundClass;

    // walk through the list of dynlink library registered classes
    AfxLockGlobals(CRIT_DYNLINKLIST);
    for (CDynLinkLibrary* pDLL = pModuleState->m_libraryList; pDLL != NULL;
         pDLL = pDLL->m_pNextDLL)
    {
        for (pClass = pDLL->m_classList; pClass != NULL;
            pClass = pClass->m_pNextClass)
        {
			// note that pClass->m_lpszClassName has a LPCSTR ANSI type 
            if ( _tcscmp(CA2CT(pClass->m_lpszClassName), strClassName) == 0)
            {
                pFoundClass = pClass;
                break;
            }
        }
        if (pFoundClass)
            break;
    }
    AfxUnlockGlobals(CRIT_DYNLINKLIST);
    return pFoundClass;
}
#endif

////////////////////////////////////////////////////////////////////////////
// CDocTemplate search functions

CDocTemplate* T_API TFindDocTemplate(LPCTSTR pszExt)
{
	CDocTemplate* pTempl = NULL;

	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp);
	if(!pApp)
		return NULL;

	if(!pApp->m_pDocManager)
		return NULL;

	POSITION pos = pApp->m_pDocManager->GetFirstDocTemplatePosition();
	while (pos)
	{
		pTempl = pApp->m_pDocManager->GetNextDocTemplate(pos);
		if(pTempl)
		{
			CString strDoc;
			pTempl->GetDocString(strDoc, CDocTemplate::filterExt);
			if (strDoc == pszExt)
				return pTempl;
		}
	}

	return 0;
}


// evident !


/*
// function extracts keyword value from DSN
CString TGetKeywordValue(TString strDSN, LPCTSTR pszKeyword)
{
    CString str = "";
    
    if (strDSN.GetLength() == 0)
        return str;

    // set the last semicolon
    if (strDSN[strDSN.GetLength()-1] != ';')
        strDSN += ';';
    
    int iKeywordPos = strDSN.Find(pszKeyword);
    if (iKeywordPos == -1) 
        return str;
    int iSemicolonPos = strDSN.Search(iKeywordPos, _T(";"));
    if (iSemicolonPos == -1) 
        return str;

    // retrieve Keyword value
    int iValuePos = iKeywordPos + _tcslen(pszKeyword) + 1;
    int iLength = iSemicolonPos-iValuePos;
    if (iLength == 0)
        return str;
    str = strDSN.Extract(iValuePos, iLength);
    return str;
}
*/

//------------------------------------------------------------------------------------------
// Name : TLoadToolTip
// Description : loads command description defined in .rc (IDS=nID) , extracts tooltip text 
// after separator and optionaly adds ellipsis
// Parameters: 
//		[in] uint  - command ID
//		[in] bool  - Wheather to add ellipsis ?
//      [in] UINT  - tooltip number (Ex: ID_SOME_COMMAND "Statusbar text\nTooltip1\nTooltip2" ) 
//      [in] TCHAR - tooltip separator (Ex: '\n', '\t') 
// Return value: 
//		CString type - tooltip loaded, or main text when tooltip number == 0
//------------------------------------------------------------------------------------------


CString T_API TLoadToolTip(UINT nID, BOOL bAddDots, UINT nToolTip, TCHAR chSeparator)
{
    CString strSrc, strDst;
    VERIFY(strSrc.LoadString(nID));

    AfxExtractSubString(strDst, strSrc, nToolTip, chSeparator); // TCHAR('\n') by default
    if (bAddDots == TRUE)
        strDst += _T("...");
    return strDst;
}



T_FUNC CPoint T_API TScreenToWindow(CWnd* pWnd, POINT& ptScreen)
{
    CPoint ptClient, ptWindow, ptTopLeftClient(0,0);
    CRect  rcWindow; // window rectangles    
    ptClient = ptScreen;
    pWnd->ScreenToClient(&ptClient);
    pWnd->ClientToScreen(&ptTopLeftClient);
    pWnd->GetWindowRect (&rcWindow);
    CSize size = ptTopLeftClient - rcWindow.TopLeft(); // gap between windowrect and clientrect
    ptWindow = ptClient + size;
    return ptWindow;
}

T_FUNC CPoint T_API TWindowToScreen(CWnd* pWnd, POINT& ptWindow)
{
    ASSERT(0);
    return ptWindow;
}



static LPCTSTR cszWindowFlags  = _T("flags"  );
static LPCTSTR cszWindowState  = _T("state"  );
static LPCTSTR cszWindowLeft   = _T("left"   );
static LPCTSTR cszWindowTop    = _T("top"    );
static LPCTSTR cszWindowRight  = _T("right"  );
static LPCTSTR cszWindowBottom = _T("bottom" );
static LPCTSTR cszWindowMinX   = _T("min_x"  );    
static LPCTSTR cszWindowMinY   = _T("min_y"  );    
static LPCTSTR cszWindowMaxX   = _T("max_x"  );    
static LPCTSTR cszWindowMaxY   = _T("max_y"  );    

 
void T_API TLoadWindowPlacement(CWnd* pWnd, LPCTSTR pszSection, UINT nCmdShow, bool bRestoreOnlySize)
{
    WINDOWPLACEMENT pl;
    CWinApp* pApp = AfxGetApp();

    // load parameters    
    pl.flags                        = pApp->GetProfileInt(pszSection, cszWindowFlags,      0);
    pl.showCmd                      = pApp->GetProfileInt(pszSection, cszWindowState,    SW_NORMAL);
    pl.rcNormalPosition.left        = pApp->GetProfileInt(pszSection, cszWindowLeft,      20);
    pl.rcNormalPosition.top         = pApp->GetProfileInt(pszSection, cszWindowTop,       20);
    pl.rcNormalPosition.right       = pApp->GetProfileInt(pszSection, cszWindowRight,    620);
    pl.rcNormalPosition.bottom      = pApp->GetProfileInt(pszSection, cszWindowBottom,   460);   

    pl.ptMinPosition.x              = pApp->GetProfileInt(pszSection, cszWindowMinX,      -1);   
    pl.ptMinPosition.y              = pApp->GetProfileInt(pszSection, cszWindowMinY,      -1);   
    pl.ptMaxPosition.x              = pApp->GetProfileInt(pszSection, cszWindowMaxX,      -1);   
    pl.ptMaxPosition.y              = pApp->GetProfileInt(pszSection, cszWindowMaxY,      -1);   

	// this values are experimentally detected
	//    pl.ptMinPosition = CPoint( 1,-1);
	//    pl.ptMaxPosition = CPoint(-1,-1);

	int nMaxX = GetSystemMetrics( SM_CXFULLSCREEN );
	int nMaxY = GetSystemMetrics( SM_CYFULLSCREEN );
	if( nMaxX < pl.rcNormalPosition.left || 
		nMaxY < pl.rcNormalPosition.top  ||
		nMaxX < ( pl.rcNormalPosition.right - pl.rcNormalPosition.left )  ||
		nMaxY < ( pl.rcNormalPosition.bottom - pl.rcNormalPosition.top ) ||
		pl.rcNormalPosition.left < 0 ||
		pl.rcNormalPosition.top < 0	)
	{
		return;
	}

	if(bRestoreOnlySize)
	{
		if( (pl.rcNormalPosition.right - pl.rcNormalPosition.left) > 0 && (pl.rcNormalPosition.bottom - pl.rcNormalPosition.top > 0))
			pWnd->SetWindowPos(&CWnd::wndNoTopMost,0,0,pl.rcNormalPosition.right - pl.rcNormalPosition.left,pl.rcNormalPosition.bottom - pl.rcNormalPosition.top, SWP_NOZORDER | SWP_NOMOVE );
	}
	else
		pWnd->SetWindowPlacement(&pl);
}

     
void T_API TSaveWindowPlacement(CWnd* pWnd, LPCTSTR pszSection)
{
    WINDOWPLACEMENT pl;
    CWinApp* pApp = AfxGetApp();

    pWnd->GetWindowPlacement(&pl);

    // save parameters
    pApp->WriteProfileInt(pszSection, cszWindowFlags,   pl.flags);
    pApp->WriteProfileInt(pszSection, cszWindowState,   pl.showCmd);
    pApp->WriteProfileInt(pszSection, cszWindowLeft,    pl.rcNormalPosition.left  );
    pApp->WriteProfileInt(pszSection, cszWindowTop,     pl.rcNormalPosition.top   );
    pApp->WriteProfileInt(pszSection, cszWindowRight,   pl.rcNormalPosition.right );
    pApp->WriteProfileInt(pszSection, cszWindowBottom,  pl.rcNormalPosition.bottom);  

    pApp->WriteProfileInt(pszSection, cszWindowMinX,    pl.ptMinPosition.x);  
    pApp->WriteProfileInt(pszSection, cszWindowMinY,    pl.ptMinPosition.y);  
    pApp->WriteProfileInt(pszSection, cszWindowMaxX,    pl.ptMaxPosition.x);  
    pApp->WriteProfileInt(pszSection, cszWindowMaxY,    pl.ptMaxPosition.y);  
}

/*
void T_API TLoadWindowPlacement(TSettings* pSettings, CWnd* pWnd, LPCTSTR pszSection, UINT nCmdShow, bool bRestoreOnlySize)
{
    WINDOWPLACEMENT pl; 
    // load parameters    
    pl.flags                        = pSettings->GetInt(pszSection, cszWindowFlags,      0);
    pl.showCmd                      = pSettings->GetInt(pszSection, cszWindowState,    SW_NORMAL);
    pl.rcNormalPosition.left        = pSettings->GetInt(pszSection, cszWindowLeft,      20);
    pl.rcNormalPosition.top         = pSettings->GetInt(pszSection, cszWindowTop,       20);
    pl.rcNormalPosition.right       = pSettings->GetInt(pszSection, cszWindowRight,    620);
    pl.rcNormalPosition.bottom      = pSettings->GetInt(pszSection, cszWindowBottom,   460);   

    pl.ptMinPosition.x              = pSettings->GetInt(pszSection, cszWindowMinX,      -1);   
    pl.ptMinPosition.y              = pSettings->GetInt(pszSection, cszWindowMinY,      -1);   
    pl.ptMaxPosition.x              = pSettings->GetInt(pszSection, cszWindowMaxX,      -1);   
    pl.ptMaxPosition.y              = pSettings->GetInt(pszSection, cszWindowMaxY,      -1);   

	// this values are experimentally detected
	//    pl.ptMinPosition = CPoint( 1,-1);
	//    pl.ptMaxPosition = CPoint(-1,-1);

	int nMaxX = GetSystemMetrics( SM_CXFULLSCREEN );
	int nMaxY = GetSystemMetrics( SM_CYFULLSCREEN );
	if( nMaxX < pl.rcNormalPosition.left || 
		nMaxY < pl.rcNormalPosition.top  ||
		nMaxX < ( pl.rcNormalPosition.right - pl.rcNormalPosition.left )  ||
		nMaxY < ( pl.rcNormalPosition.bottom - pl.rcNormalPosition.top ) ||
		pl.rcNormalPosition.left < 0 ||
		pl.rcNormalPosition.top < 0	) 
		return;

	if(bRestoreOnlySize)
	{
		if( (pl.rcNormalPosition.right - pl.rcNormalPosition.left) > 0 && (pl.rcNormalPosition.bottom - pl.rcNormalPosition.top > 0))
		pWnd->SetWindowPos(&CWnd::wndNoTopMost,0,0,pl.rcNormalPosition.right - pl.rcNormalPosition.left,pl.rcNormalPosition.bottom - pl.rcNormalPosition.top, SWP_NOZORDER | SWP_NOMOVE );
	}
	else
	{
		pWnd->SetWindowPlacement(&pl);
	}
}
void T_API TSaveWindowPlacement(TSettings* pSettings, CWnd* pWnd, LPCTSTR pszSection)
{
    WINDOWPLACEMENT pl;
    
    pWnd->GetWindowPlacement(&pl);    
    // save parameters
    pSettings->WriteInt(pszSection, cszWindowFlags,   pl.flags);
    pSettings->WriteInt(pszSection, cszWindowState,   pl.showCmd);
    pSettings->WriteInt(pszSection, cszWindowLeft,    pl.rcNormalPosition.left  );
    pSettings->WriteInt(pszSection, cszWindowTop,     pl.rcNormalPosition.top   );
    pSettings->WriteInt(pszSection, cszWindowRight,   pl.rcNormalPosition.right );
    pSettings->WriteInt(pszSection, cszWindowBottom,  pl.rcNormalPosition.bottom);  

    pSettings->WriteInt(pszSection, cszWindowMinX,    pl.ptMinPosition.x);  
    pSettings->WriteInt(pszSection, cszWindowMinY,    pl.ptMinPosition.y);  
    pSettings->WriteInt(pszSection, cszWindowMaxX,    pl.ptMaxPosition.x);  
    pSettings->WriteInt(pszSection, cszWindowMaxY,    pl.ptMaxPosition.y);  
}
*/

static LPCTSTR cszHeight         = _T("Height"        );
static LPCTSTR cszWidth          = _T("Width"         );
static LPCTSTR cszEscapement     = _T("Escapement"    );
static LPCTSTR cszOrientation    = _T("Orientation"   );
static LPCTSTR cszWeight         = _T("Weight"        );
static LPCTSTR cszItalic         = _T("Italic"        );
static LPCTSTR cszUnderline      = _T("Underline"     );
static LPCTSTR cszStrikeOut      = _T("StrikeOut"     );
static LPCTSTR cszCharSet        = _T("CharSet"       );
static LPCTSTR cszOutPrecision   = _T("OutPrecision"  );
static LPCTSTR cszClipPrecision  = _T("ClipPrecision" );
static LPCTSTR cszQuality        = _T("Quality"       );
static LPCTSTR cszPitchAndFamily = _T("PitchAndFamily");
static LPCTSTR cszFaceName       = _T("FaceName"      );


void T_API TLoadFontInfo(LOGFONT& logfont, LPCTSTR pszSection)
{    
    TWinApp* pApp = TGetApp();

    logfont.lfHeight                = pApp->GetProfileInt(pszSection, cszHeight,         0);
    logfont.lfWidth                 = pApp->GetProfileInt(pszSection, cszWidth,          0);
    logfont.lfEscapement            = pApp->GetProfileInt(pszSection, cszEscapement,     0);
    logfont.lfOrientation           = pApp->GetProfileInt(pszSection, cszOrientation,    0);
    logfont.lfWeight                = pApp->GetProfileInt(pszSection, cszWeight,         0);   
    logfont.lfItalic                = pApp->GetProfileInt(pszSection, cszItalic,         0);   
    logfont.lfUnderline             = pApp->GetProfileInt(pszSection, cszUnderline,      0);   
    logfont.lfStrikeOut             = pApp->GetProfileInt(pszSection, cszStrikeOut,      0);   
    logfont.lfCharSet               = pApp->GetProfileInt(pszSection, cszCharSet,        0);   
    logfont.lfOutPrecision          = pApp->GetProfileInt(pszSection, cszOutPrecision,   0);   
    logfont.lfClipPrecision         = pApp->GetProfileInt(pszSection, cszClipPrecision,  0);   
    logfont.lfQuality               = pApp->GetProfileInt(pszSection, cszQuality,        0);   
    logfont.lfPitchAndFamily        = pApp->GetProfileInt(pszSection, cszPitchAndFamily, 0);   
    lstrcpyn(logfont.lfFaceName, pApp->GetProfileString(pszSection, cszFaceName), LF_FACESIZE);   
}


void T_API TSaveFontInfo(LOGFONT& logfont, LPCTSTR pszSection)
{
    CWinApp* pApp = AfxGetApp();

    pApp->WriteProfileInt   (pszSection, cszHeight,         logfont.lfHeight);
    pApp->WriteProfileInt   (pszSection, cszWidth,          logfont.lfWidth );
    pApp->WriteProfileInt   (pszSection, cszEscapement,     logfont.lfEscapement );
    pApp->WriteProfileInt   (pszSection, cszOrientation,    logfont.lfOrientation);
    pApp->WriteProfileInt   (pszSection, cszWeight,         logfont.lfWeight);   
    pApp->WriteProfileInt   (pszSection, cszItalic,         logfont.lfItalic);   
    pApp->WriteProfileInt   (pszSection, cszUnderline,      logfont.lfUnderline);   
    pApp->WriteProfileInt   (pszSection, cszStrikeOut,      logfont.lfStrikeOut);   
    pApp->WriteProfileInt   (pszSection, cszCharSet,        logfont.lfCharSet);   
    pApp->WriteProfileInt   (pszSection, cszOutPrecision,   logfont.lfOutPrecision);   
    pApp->WriteProfileInt   (pszSection, cszClipPrecision,  logfont.lfClipPrecision);   
    pApp->WriteProfileInt   (pszSection, cszQuality,        logfont.lfQuality);   
    pApp->WriteProfileInt   (pszSection, cszPitchAndFamily, logfont.lfPitchAndFamily);   
    pApp->WriteProfileString(pszSection, cszFaceName, logfont.lfFaceName);   
}


int T_API TAddStringToComboBox(CComboBox& rComboBox, const CString& rStr, int iPos,
                               bool bReinsertDuplicated, bool bAddOnlyIfNew) 
{
    int nIndex = -1;
    if ( !rStr.IsEmpty() )
    {
		nIndex = rComboBox.FindStringExact (-1, rStr);

		if (nIndex >= 0) // already exist
        {
            if (bAddOnlyIfNew)
                return nIndex;

            if (bReinsertDuplicated)             
                rComboBox.DeleteString(nIndex);
        }
        nIndex = rComboBox.InsertString(iPos, rStr);
    }
    return nIndex;
}


//------------------------------------------------------------------------------------------
// Name : LoadComboBoxStrings, SaveComboBoxStrings
// Description : loads, saves combo box strings 
// Parameters: 
//		[in, out] CComboBox& - reference to the combo box
//      [in]      CString&   - regitry section like \\Dialogs\\SelectDelServer\\ 
//      [in]      LPCTSTR    - key name prefix (i.e. User for UserXXX, Group for GroupXXX)
// Return value: 
//		absent
//------------------------------------------------------------------------------------------


void T_API TLoadComboBoxStrings(CComboBox& rComboBox, const CString& strSection, const CString& strKeyName )
{
    CWinApp*    pApp = AfxGetApp();	
	CString     lstrKeyName;
    CString     strFormat = strKeyName;
    strFormat += _T("%d");
    
    CString str;
	for(int i=0;;i++)
    {
		lstrKeyName.Format (strFormat, i);
		str = pApp->GetProfileString(strSection, lstrKeyName, NULL);
        if ( !str.IsEmpty() )
            TAddStringToComboBox(rComboBox, str, -1);
        else
            break;        
	}
}


void T_API TSaveComboBoxStrings(CComboBox& rComboBox, const CString& strSection, const CString& strKeyName )
{
    CWinApp*    pApp = AfxGetApp();	
	CString     lstrKeyName;
    CString     strFormat = strKeyName;
    strFormat += _T("%d");
    
    CString str;
	for (int i = 0; i < rComboBox.GetCount(); i++) 
    {
		lstrKeyName.Format (strFormat, i);
        rComboBox.GetLBText(i, str);
		pApp->WriteProfileString(strSection, lstrKeyName, str);
	}
}


// enumerate all views in active document
void T_API TGetHwndOfAllViews (THWndArray& rarrHWNDs, TViewArray& rarrViews)
{   // NOTE: for MDI application - alter way: Get App, enumerate DocTemplates.
    // .. for each DocTemplate enumerate documents, for each document enumerate views.
    rarrHWNDs.RemoveAll();
    // 1. Get frame window
    CFrameWnd*  pFrameWnd = dynamic_cast <CFrameWnd*> (AfxGetMainWnd());
    if (!pFrameWnd)
    {
        ASSERT (pFrameWnd);
        return;
    }
    // 2. Get active document in it
    CDocument*  pDoc = pFrameWnd->GetActiveDocument( );
    if (!pDoc)
    {
        ASSERT (pDoc);
        return;
    }
    // 3. Enumerate all views in this document.
    POSITION pos = pDoc->GetFirstViewPosition();
    while (pos != NULL)
    {
        CView* pView = pDoc->GetNextView(pos);
        HWND    hWnd = pView->GetSafeHwnd();
        rarrHWNDs.Add (hWnd);
        rarrViews.Add(pView);
    }
}


//AfxConnectionAdvise
void T_API TConnectionAdvise(IUnknown* piUnk, REFIID iid, IDispatch* piEventSink, 
                             IConnectionPointPtr& pConnectionPoint,  DWORD* pdwCookie)
{
	ASSERT(piUnk && piEventSink && pdwCookie);
	if(!piUnk ||
		!piEventSink ||
		!pdwCookie)
	{
		return ;
	}

    IConnectionPointContainerPtr  pContainer = NULL;
    *pdwCookie = 0;

    // establish connection with connection point
    HRESULT hr = S_OK;
    hr = piUnk->QueryInterface(__uuidof(IConnectionPointContainer), (void**)&pContainer);
    TCheckErrorEx ( hr, piUnk, iid );    
    hr = pContainer->FindConnectionPoint(iid, &pConnectionPoint);
    TCheckErrorEx ( hr, pContainer, __uuidof(IConnectionPointContainer) );
    hr = pConnectionPoint->Advise(piEventSink, pdwCookie);
    TCheckErrorEx ( hr, pConnectionPoint, __uuidof(IConnectionPoint) );
}

//AfxConnectionUnadvise
void T_API TConnectionUnadvise(IConnectionPointPtr pConnectionPoint, const DWORD dwCookie)
{    
    if (dwCookie)
	{
		TCheckErrorEx( pConnectionPoint->Unadvise(dwCookie), pConnectionPoint, __uuidof(IConnectionPoint) );		
	}
}

/*
BOOL T_API TCompareTypeInfos(ITypeInfo *pTI1, ITypeInfo *pTI2) 
{
	HRESULT     hr;
	UINT        cFuncs;
	BOOL        bOK = TRUE;    
	TYPEATTR    *pTA1 = NULL, *pTA2 = NULL;
	FUNCDESC    *pFuncDesc1 = NULL, *pFuncDesc2 = NULL;

    try
    {
	    hr = pTI1->GetTypeAttr(&pTA1);
        TCheckErrorEx(hr, pTI1, __uuidof(ITypeInfo));
	    hr = pTI2->GetTypeAttr(&pTA2);
        TCheckErrorEx(hr, pTI2, __uuidof(ITypeInfo));	

	    // Compare typeinfo's
	    if ((pTA1->typekind     != pTA2->typekind    ) ||
		    (pTA1->cFuncs       >  pTA2->cFuncs      ) ||
		    (pTA1->wMajorVerNum != pTA2->wMajorVerNum) ||
		    (pTA1->wMinorVerNum != pTA2->wMinorVerNum)) 
        {
		    bOK = FALSE;
	    }
	    cFuncs = pTA1->cFuncs;

	    pTI1->ReleaseTypeAttr(pTA1);
	    pTI2->ReleaseTypeAttr(pTA2);
        pTA1 = pTA2 = NULL;

	    if (bOK) 
        {
		    // Compare function by function		
		    for (UINT i=0; i < cFuncs; i++) 
            {
			    hr = pTI1->GetFuncDesc(i, &pFuncDesc1);
                TCheckErrorEx(hr, pTI1, __uuidof(ITypeInfo));			

			    hr = pTI2->GetFuncDesc(i, &pFuncDesc2);
                TCheckErrorEx(hr, pTI2, __uuidof(ITypeInfo));			

			    if ((pFuncDesc1->memid    != pFuncDesc2->memid   ) ||
				    (pFuncDesc1->invkind  != pFuncDesc2->invkind ) ||
				    (pFuncDesc1->callconv != pFuncDesc2->callconv) ||
				    (pFuncDesc1->cParams  != pFuncDesc2->cParams )) 
                {
				    bOK = FALSE;
			    }
			    pTI1->ReleaseFuncDesc(pFuncDesc1);
			    pTI2->ReleaseFuncDesc(pFuncDesc2);
			    pFuncDesc1 = pFuncDesc2 = NULL;			
			    if (!bOK) 
                    break;
		    }
	    }	    
    }
    catch(TException* pEx)
    {
        if (pTA1) pTI1->ReleaseTypeAttr(pTA1);
        if (pTA2) pTI2->ReleaseTypeAttr(pTA2);    
		if (pFuncDesc1) pTI1->ReleaseFuncDesc(pFuncDesc1);
        if (pFuncDesc2) pTI2->ReleaseFuncDesc(pFuncDesc2);
        bOK = FALSE;
        pEx->Delete();
    }
    return bOK;
}
*/



// Split a str into array of strings, delimited by chDelim. Ex: 'aaa\bbb', chDelim = '\\' => {'bbb', 'aaa'}
// NOTE: sequence of tokens will be reversed in array
void T_API TSplitStrToArray  (CString str, CStringArray& arrStrings, TCHAR chDelim)
{
    ASSERT (chDelim != '\0');   // CString functions does all work untill '\0' is found
    if (chDelim == '\0')
        return;
    
    while (str.GetLength() > 0)
    {
        CString     strToken    = str.SpanExcluding (CString(chDelim));
        int         nStrLen     = str.GetLength();
        int         nTokenLen   = strToken.GetLength();

        ASSERT (nTokenLen);
        if (nTokenLen == 0)
            return;

        arrStrings.Add (strToken);  // Add to end of archive - sequence of tokens will be reversed

        if ((nStrLen - nTokenLen) <= 1)
            str.Empty();    // No tokens left in string (may be one delimiter at last symbol)
        else
            str = str.Right (nStrLen - nTokenLen - 1);    // Trim string by givven token and delimiter after it
    }
}



FARPROC T_API TGetProcAddress(HINSTANCE hInst, LPCSTR pszProcName)
{
    FARPROC pnfProc = GetProcAddress(hInst, pszProcName);
    if (!pnfProc)
    {
/*      TCHAR szFileName[MAX_PATH];
        GetModuleFileName(hInst, szFileName, sizeof(szFileName));
*/      
        
        TException* pEx = new TException(IDE_T_ENTRYPOINT_NOT_FOUND);
        *pEx << CA2CT(pszProcName);
        *pEx << TGetModuleFileName( hInst );
        ThrowException(pEx);
    }
    return pnfProc;
}

CString T_API TConvertBytesToString(BYTE const * pbyteData, int nLength, bool bWithSlash)
{
	CString strResult;
	
	wchar_t* pcwszFormat = (bWithSlash ? _T("\\%02X") : _T("%02X"));
	int n = (bWithSlash ? 3 : 2);

	TCHAR* pchPos = strResult.GetBuffer(nLength * n);
	for (int i = 0; i < nLength; ++i, pchPos += n)
		_stprintf(pchPos, pcwszFormat, (ULONG) pbyteData[i]);

	strResult.ReleaseBuffer();
	return strResult;
}

T_FUNC int T_API TConvertStringToBytes(LPCTSTR szIn, PBYTE pbyteBuf, int nBufLen)
{
/*
	int nStrLen = _tcslen(szIn);
	int nBytes = nStrLen / 2;
	if ( nBytes > nBufLen || nStrLen % 2)
	{
		ASSERT(FALSE); 
		return 0;
	}
	for (int i = 0; i < nBytes; i++)
	{
		TCHAR chTemp = szIn[2];
		((LPTSTR)szIn)[2] = 0;
		LPTSTR szEnd;
		pbyteBuf[i] = static_cast<BYTE>(_tcstoul(szIn, &szEnd, 16));
		((LPTSTR)szIn)[2] = chTemp;
		szIn += 2;
	}
*/

    int nBytes = static_cast<int>(ceil ((double) _tcslen(szIn) / 2));

    if (nBytes > nBufLen)
	{
		ASSERT(FALSE); 
		return 0;
	}

    TCHAR szTmp [3];
    szTmp[2] = 0;
    for (int i = 0; i < nBytes; i++)
    {
        _tcsncpy (szTmp, szIn + i*2, 2);
        pbyteBuf[i] = static_cast<BYTE>(_tcstoul(szTmp, NULL, 16));
    }

	return nBytes;
}

void T_API TDeleteTempMap()
{
    AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();
    if (pState->m_nTempMapLock == 0)
    {
        // free temp maps, OLE DLLs, etc.
        AfxLockTempMaps();
        AfxUnlockTempMaps();
    }
}

LANGID T_API TGetUserLangID()
{
    HMODULE hKernel = ::GetModuleHandle( _T("kernel32.dll"));
    if( hKernel != 0 )
    {
        LANGID ( WINAPI* GetLangID)(void)  = 
            (LANGID( WINAPI* )(void))(::GetProcAddress( hKernel, "GetUserDefaultUILanguage" ));
        if( GetLangID != 0 )
            return GetLangID();
    }
    return ::GetUserDefaultLangID();
}

CString T_API TGetUserLangRDN()
{
    TCHAR buff[12];
    _ultot( TGetUserLangID(), buff, 16 );
    return CString(buff);
}

CString T_API TLoadScriptFromFile(LPCTSTR pszSqlFileName)
{
	CString strScript;

	CFile fileScript;
	CFileException ex;
	if (!fileScript.Open(pszSqlFileName, CFile::modeRead | CFile::shareDenyWrite, &ex))
	{
		CString strErrorMsg;
		strErrorMsg.Format(IDE_T_SQLSCRIPTFILE_OPEN_ERROR, pszSqlFileName);
		AfxMessageBox(strErrorMsg, MB_OK | MB_ICONINFORMATION);
		return strScript;
	}

	UINT	uiFileSize = (UINT)fileScript.GetLength();
	auto_ptr<BYTE> spBuf(new BYTE[uiFileSize + 2]);

	//memset(pbBuf, 0, uiFileSize + 2);
	UINT	uiRead = fileScript.Read(spBuf.get(), uiFileSize);
	fileScript.Close();
	if (uiRead != uiFileSize)
	{
		CString strErrorMsg;
		strErrorMsg.Format(IDE_T_SQLSCRIPTFILE_OPEN_ERROR, pszSqlFileName);
		AfxMessageBox(strErrorMsg, MB_OK | MB_ICONINFORMATION);
		return strScript;
	}

	if ((*spBuf == 0xFF) && ((*spBuf + 1) == 0xFE)) // Unicode
	{
		CMNLog(_T("[LoadScriptFromFile]: %s, %i, '%s'"), pszSqlFileName, uiFileSize, _T("unicode"));
		LPCWSTR psz = (LPCWSTR)spBuf.get();
		strScript = CString(psz + 1, uiFileSize / sizeof(WCHAR) - 1); // pass first OxFF 0xFE
	}
	else // ANSI
	{
		CMNLog(_T("[LoadScriptFromFile]: %s, %i, '%s'"), pszSqlFileName, uiFileSize, _T("ANSI"));
		LPCSTR psz = (LPCSTR)spBuf.get();
		strScript = CString(psz, uiFileSize);
	}

	return strScript;
}


void T_API TLoadUnicodeTextFromResource (HMODULE hModule, LPCTSTR pcszName, LPCTSTR pcszType, CStringW& rstrDest, bool bCheckForConversion)
{
    HRSRC hRsrc = ::FindResource (hModule, pcszName, pcszType);
    TCheckIsError (hRsrc != NULL);

    HGLOBAL hgRes = ::LoadResource (hModule, hRsrc);
    TCheckIsError (hgRes != NULL);

    DWORD dwResSize = ::SizeofResource (hModule, hRsrc);
    TCheckIsError (dwResSize != 0);

    LPVOID pvRes = ::LockResource(hgRes);
    TCheckIsError (pvRes != NULL);

	if (bCheckForConversion && !TIsUnicode(pvRes, dwResSize))
	{
		// convert it to unicode (there cannot be result longer than dwResSize wide chars)
		int		nRes		=
			MultiByteToWideChar(CP_ACP, 0UL, (const char*)pvRes, dwResSize, rstrDest.GetBufferSetLength(dwResSize), dwResSize);
		TCheckIsError(nRes != NULL);
		rstrDest.ReleaseBuffer(nRes);
	}
	else
	{
		// old part without test or if the test failed
		wchar_t* pwcRes = (wchar_t*)(pvRes);
		if (dwResSize >= sizeof (wchar_t) &&
			(pwcRes[0] == 0xfffe || pwcRes[0] == 0xfeff))
		{
			// Skip UNICODE byte-order mark, if one is present.
			// Byte-order marks are written by some text editors, but the XML Parser fails if such mark is present
			++pwcRes;
			dwResSize -= sizeof(wchar_t);
		}

		size_t iLen = dwResSize / sizeof(wchar_t);

		wmemcpy (rstrDest.GetBufferSetLength (iLen), pwcRes, iLen);
		rstrDest.ReleaseBuffer (iLen);
	}
}

CStringW T_API TLoadUnicodeTextFromResource (UINT nID, LPCTSTR pcszType, bool bCheckForConversion)
{
    CStringW strRetVal;
    TLoadUnicodeTextFromResource (::AfxFindResourceHandle (MAKEINTRESOURCE(nID), pcszType),
        MAKEINTRESOURCE(nID), pcszType, strRetVal, bCheckForConversion);
    return strRetVal;
}

void T_API TLoadAnsiTextFromResource (HMODULE hModule, LPCTSTR pcszName, LPCTSTR pcszType, CStringA& rstrDest)
{
    HRSRC hRsrc = ::FindResource (hModule, pcszName, pcszType);
    TCheckIsError (hRsrc != NULL);

    HGLOBAL hgRes = ::LoadResource (hModule, hRsrc);
    TCheckIsError (hgRes != NULL);

    DWORD dwResSize = ::SizeofResource (hModule, hRsrc);
    TCheckIsError (dwResSize != 0);

    LPVOID pvRes = ::LockResource(hgRes);
    TCheckIsError (pvRes != NULL);

    memcpy (rstrDest.GetBufferSetLength (dwResSize), pvRes, dwResSize);
    rstrDest.ReleaseBuffer (dwResSize);
}

CStringA T_API TLoadAnsiTextFromResource (UINT nID, LPCTSTR pcszType)
{
	CStringA strRetVal;
	TLoadAnsiTextFromResource (::AfxFindResourceHandle (MAKEINTRESOURCE(nID), pcszType),
        MAKEINTRESOURCE(nID), pcszType, strRetVal);
    return strRetVal;
}

// Functions to bring multiline strings to convenient format
void T_API TMakeText0D0A(wstring* pstrDest, const wchar_t* pcwszBegin, const wchar_t* pcwszEnd)
{
	for (const wchar_t* pcwch = pcwszBegin; pcwch != pcwszEnd; ++pcwch)
	{
		const wchar_t*	pcwchNext;
		switch (*pcwch)
		{
			case 0x000A:
				// cannot go first - just add ending symbols
				pstrDest->push_back(0x000D);
				pstrDest->push_back(0x000A);
				break;
			case 0x000D:
				// check whether 0x000A is present to and if it is
				// move to next position
				pcwchNext	= pcwch + 1;
				if (pcwchNext != pcwszEnd && *pcwchNext == 0x000A)
				{
					++pcwch;
				}
				pstrDest->push_back(0x000D);
				pstrDest->push_back(0x000A);
				break;
			default:
				// any other goes to dest directly
				pstrDest->push_back(*pcwch);
		}
	}
}

void T_API TMakeText0A(wstring* pstrDest, const wchar_t* pcwszBegin, const wchar_t* pcwszEnd)
{
	for (const wchar_t* pcwch = pcwszBegin; pcwch != pcwszEnd; ++pcwch)
	{
		const wchar_t*	pcwchNext;
		switch (*pcwch)
		{
			case 0x000A:
				// cannot go first - just add ending symbols
				pstrDest->push_back(0x000A);
				break;
			case 0x000D:
				// check whether 0x000A is present to and if it is
				// move to next position
				pcwchNext	= pcwch + 1;
				if (pcwchNext != pcwszEnd && *pcwchNext == 0x000A)
				{
					++pcwch;
				}
				pstrDest->push_back(0x000A);
				break;
			default:
				// any other goes to dest directly
				pstrDest->push_back(*pcwch);
		}
	}
}

CString T_API TGetModuleFileName(HMODULE hmod)
{

    TCHAR* szBuff    = static_cast<TCHAR*>(_alloca( DF_UNICODE_MAXPATH + 1 ));
           szBuff[0] = TEXT('\0');
    DWORD  dwResult  = GetModuleFileName(hmod, szBuff, DF_UNICODE_MAXPATH + 1);

    if( dwResult == 0 )
        return CString();

    ASSERT( dwResult <= DF_UNICODE_MAXPATH );

    return szBuff;

/*	static const int	nMinBufLen		= MAX_PATH;			// start from this
	static const int	nMaxBufLen		= MAX_PATH * 50;	// don't try greater

	for (int nBufLen = nMinBufLen; nBufLen <= nMaxBufLen; nBufLen *= 2)
	{
		CString		strResult;
		DWORD		dwResult	= GetModuleFileName(hmod, strResult.GetBufferSetLength(nBufLen - 1), nBufLen);

		// zero means error here
		if (dwResult == 0)
			break;

		// nBufLen means that there is not enough space in the buffer
		if (dwResult < nBufLen)
			return strResult;
	}

	// some error or exceeded maxinum allowed size
	return _T("");*/
}

bool T_API TSplitFullFileName(CString const& strFullFileName, CString& strDirectory, CString& strFileName)
{
	TCHAR szDrive  [_MAX_DRIVE];
	TCHAR szDir    [_MAX_DIR];
	TCHAR szFname  [_MAX_FNAME];
	TCHAR szExt    [_MAX_EXT];
	TCHAR szPath   [_MAX_PATH];

	_tsplitpath(strFullFileName, szDrive, szDir, szFname, szExt);    
	_tmakepath(szPath, szDrive, szDir, NULL, NULL);

	strDirectory = szPath;
	strFileName = szFname;
	if(!strFileName.IsEmpty())
		strFileName += szExt;

	return true;
}

BOOL T_API TIs64WowPresent()
{
    typedef BOOL (__stdcall *PIsWow64Process)(HANDLE hProcess, PBOOL Wow64Process);

    BOOL b64Bit = FALSE;
    HMODULE hModule = ::LoadLibrary(L"kernel32.dll");
    if (NULL != hModule)
    {         
         PIsWow64Process pFunc = (PIsWow64Process)::GetProcAddress(hModule, "IsWow64Process");
         if (NULL != pFunc)
         {            
            BOOL bR = (pFunc)(GetCurrentProcess(), &b64Bit);
            if (FALSE == bR)
                b64Bit = FALSE;
         }         
        ::FreeLibrary(hModule);
    }
    return b64Bit;
}


BOOL T_API TRunMSCFile(CString const& rstrMscFile, BOOL bUseSystemPathToMsc, CString const& rstrParametrs, CString const& rstrWorkDir)
{
    wchar_t pwszDir[MAX_PATH];    
    if(!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, 0, pwszDir)))
        return FALSE;

    CString strWorkDir = rstrWorkDir;
    if (strWorkDir.GetLength() == 0)
        strWorkDir = (LPCTSTR)pwszDir;

    CString strMscFile = rstrMscFile;
    if (TRUE == bUseSystemPathToMsc)
        strMscFile = CString(pwszDir) + L"\\" + strMscFile;
    
    CString strIs32(L" ");
    if (TRUE == TIs64WowPresent())
        strIs32 = L" -32 ";

    CString strParamLine;
    strParamLine.Format(L"%s\\mmc.exe%s\"%s\" %s", pwszDir, (LPCTSTR)strIs32, (LPCTSTR)strMscFile, (LPCTSTR)rstrParametrs);   

    STARTUPINFO si;    
    PROCESS_INFORMATION ProcessInformation;   
    ZeroMemory( &si, sizeof(si) );    
    ZeroMemory( &ProcessInformation, sizeof(ProcessInformation) );
    si.cb = sizeof(si);

    BOOL bRet = ::CreateProcess(NULL, (LPTSTR)(LPCTSTR)strParamLine, NULL, NULL, FALSE, 0, NULL, (LPCTSTR)strWorkDir, &si, &ProcessInformation);
    if (!bRet)
        return FALSE;

    ::CloseHandle(ProcessInformation.hThread);
    ::CloseHandle(ProcessInformation.hProcess);
   
    return TRUE;
}

CString T_API TExpandEnvironmentStrings(const CString& strSource)
{
	DWORD	dwBufSize	= 1024;
	CString	strDest;
	DWORD	dwRealLength;
	while ((dwRealLength = ExpandEnvironmentStrings(strSource, strDest.GetBufferSetLength(dwBufSize), dwBufSize)) > dwBufSize)
	{
		dwBufSize	*= 2;
	}

	// check against 0 here, it means an error:
	strDest.ReleaseBuffer(dwRealLength == 0 ? 0 : static_cast<int>(dwRealLength) - 1);
	return strDest;
}

bool T_API TFromVariant(TOleVariant const& rcSource, CMNGuid& guidDest)
{
	switch( rcSource.vt )
	{
		case VT_EMPTY:
		{
			guidDest.SetEmpty();
			return true;
		}
		case VT_ARRAY | VT_UI1:
		{
			memcpy( guidDest.GetPointer(), (static_cast<SAFEARRAY*>(rcSource.parray))->pvData, sizeof(GUID) );
			return true;
		}
		case VT_BSTR:
		{
            return guidDest.FromString (rcSource.bstrVal);
		}
		default:
		{
			_ASSERT(false); // cannot convert source to GUID: unhandled data type
			return false;
		}
	}

	guidDest.SetEmpty();
	return false;
}

TOleVariant T_API TToVariant(CMNGuid const& guidSource, VARTYPE enDesiredType)
{
	TOleVariant vValue;

	vValue.vt = enDesiredType;

	switch ( enDesiredType )
	{
		case VT_EMPTY:
			break;

		case VT_BSTR:
			vValue.bstrVal = ::SysAllocString(guidSource.ConvertToString());
			break;

		default:
			_ASSERT(false); // cannot convert source to GUID: unhandled data type
			return false;
	}

	return vValue;
}
