//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Misc/
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Misc
//--------------------------------------------------------------------------------

#pragma once

#include <afxconv.h>
#include <afxtempl.h>

#include "CMN/include/Guid.h"

#include "T/include/T.h"
#include "T/include/OleVariant.h"
#include "T/include/WinApp.h"

class TSettings;
//predicate for Case Independed comparation of 2 CString 
struct TStrCILess : public binary_function<LPCTSTR, LPCTSTR, bool>
{
    bool operator()(LPCTSTR psz1, LPCTSTR psz2) const
    {
        if (psz1 == NULL)
        {
            if (psz2 == NULL)
                return false;
            else
                return true; // NOTE: empty string less then any not-empty
        }
        else if (psz2 == NULL) // str1 is not epmty and str2 is empty - not-empty string is greater then empty one
            return false;
        return _tcsicmp(psz1, psz2) < 0;
    }
};

struct TStrCIEqual : public binary_function<LPCTSTR, LPCTSTR, bool>
{
    bool operator()(LPCTSTR psz1, LPCTSTR psz2) const
    {
        if (psz1 == NULL)
        {
            if (psz2 == NULL)
                return true; // NOTE: empty strings are equal
            else
                return false;
        }
        else if (psz2 == NULL) // str1 is not-epmty and str2 is empty
            return false;
        return _tcsicmp(psz1, psz2) == 0;
    }
};

T_FUNC bool T_API TIsUnicode(const void* pv, int nSize);


///////////////////////////////////////////////////////////////////////////////////////////////
//
//                                      Debug helpers
//


T_FUNC CString T_API TGetDebugName(const CString& strFileName); // return "FileNamed.ext" instead of "FileNamed.ext"

///////////////////////////////////////////////////////////////////////////////////////////////
//
//                                      Some arrays

typedef CArray  <HWND, HWND&>    THWndArray;
typedef CArray  <CView*, CView*> TViewArray;
typedef CArray  <CRect, CRect&>  TRectArray;

template <class T>
class TArray : public CArray <T*, T*>
{
public:
    TArray  ()    {}
    TArray  (const TArray<T>& arr)   { *this = arr; }
    TArray& operator = (const TArray<T>& arr) { Copy(arr); return *this; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
//
//                                      _int64 functions

#ifndef _GXSTYLES_H_
//T_FUNC CArchive& AFXAPI operator << (CArchive& ar, const CMNGuid&   guidVal);
T_FUNC CArchive& AFXAPI operator << (CArchive& ar, list<CString>& lstVal);
//T_FUNC CArchive& AFXAPI operator >> (CArchive& ar, CMNGuid&   guidVal);
T_FUNC CArchive& AFXAPI operator >> (CArchive& ar, list<CString>& lstVal);
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
//
//

#ifdef _AFXDLL
T_FUNC CRuntimeClass* T_API TFindRuntimeClass(const CString& strClassName);  // CRuntimeClass search function
#endif

T_FUNC CDocTemplate*  T_API TFindDocTemplate(LPCTSTR pszExt);   // CDocTemplate search function

//T_FUNC CString        TGetKeywordValue(TString strDSN, LPCTSTR pszKeyword); // function extracts keyword value from ODBC DSN
T_FUNC CString        T_API TLoadToolTip(UINT nID, BOOL bAddDots, UINT nToolTip = 1, 
                                         TCHAR chSeparator = TCHAR('\n')); // load tooltips after separeted by \n 

T_FUNC CPoint T_API TScreenToWindow(CWnd* pWnd, POINT& ptScreen);
T_FUNC CPoint T_API TWindowToScreen(CWnd* pWnd, POINT& ptWindow);

//  windows and controls save/restore functions
T_FUNC void T_API TLoadWindowPlacement(CWnd* pWnd, LPCTSTR pszSection, UINT nCmdShow, bool bRestoreOnlySize = true);
T_FUNC void T_API TSaveWindowPlacement(CWnd* pWnd, LPCTSTR pszSection);
/*
//  windows and controls save/restore functions via TSettings
T_FUNC void T_API TLoadWindowPlacement(TSettings* pSettings, CWnd* pWnd, LPCTSTR pszSection, UINT nCmdShow, bool bRestoreOnlySize = true);
T_FUNC void T_API TSaveWindowPlacement(TSettings* pSettings, CWnd* pWnd, LPCTSTR pszSection);
*/
T_FUNC void T_API TLoadFontInfo(LOGFONT& logfont, LPCTSTR pszSection);
T_FUNC void T_API TSaveFontInfo(LOGFONT& logfont, LPCTSTR pszSection);

T_FUNC int  T_API TAddStringToComboBox(CComboBox& rComboBox, const CString& rStr, int iPos,
                                       bool bReinsertDuplicated = false, bool bAddOnlyIfNew = false);
T_FUNC void T_API TLoadComboBoxStrings(CComboBox& rComboBox, const CString& strSection, const CString& strKeyName);
T_FUNC void T_API TSaveComboBoxStrings(CComboBox& rComboBox, const CString& strSection, const CString& strKeyName);

//  doc/view helpers

T_FUNC void T_API TGetHwndOfAllViews  (THWndArray& rarrHWNDs, TViewArray& rarrViews);    // enumerate all views in active document

//  connection points
T_FUNC void T_API TConnectionAdvise   (IUnknown* piUnk, REFIID iid, IDispatch* piEventSink,                                  IConnectionPointPtr& pConnectionPoint,  DWORD* pdwCookie);
T_FUNC void T_API TConnectionUnadvise (IConnectionPointPtr pConnectionPoint, const DWORD dwCookie);// 
T_FUNC BOOL T_API TCompareTypeInfos   (ITypeInfo *pTI1, ITypeInfo *pTI2);

T_FUNC void T_API TSplitStrToArray  (CString str, CStringArray& arrStrings, TCHAR chDelim);
//
T_FUNC FARPROC T_API TGetProcAddress  (HINSTANCE hInst, LPCSTR pszProcName);

// dataconvert
T_FUNC CString T_API TConvertBytesToString(BYTE const * pbyteData, int nLength, bool bWithSlash = false);
T_FUNC int T_API TConvertStringToBytes(LPCTSTR szData, PBYTE pbyteBuf, int nBufLen);

/* [Move to CMN module]
// time
T_FUNC CString T_API TGetCurrentTime  (); // returns current time formatted as "%Y %m %d %H:%M:%S.%ms"
// very - very useful functions for local time processing
T_FUNC DATE T_API TUTCDateToLocalDate(DATE dateUTC);
T_FUNC DATE T_API TLocalDateToUTCDate(DATE dateLocal);
T_FUNC COleDateTime T_API TValueFromLDAPFilterToUTCDate (LPCTSTR szIn);
T_FUNC SYSTEMTIME T_API TUTCDateToLocalDate(const SYSTEMTIME& systemtimeUTC);
T_FUNC SYSTEMTIME T_API TLocalDateToUTCDate(const SYSTEMTIME& systemtimeLocal);
//T_FUNC DATE T_API TConvertGMT         (DATE val, bool bToGMT);
*/

T_FUNC void T_API		TDeleteTempMap();
T_FUNC LANGID T_API		TGetUserLangID();
T_FUNC CString T_API	TGetUserLangRDN();

T_FUNC CString T_API	TLoadScriptFromFile(LPCTSTR pszSqlFileName);

T_FUNC void	T_API		TLoadUnicodeTextFromResource (HMODULE hModule, LPCTSTR pcszName, LPCTSTR pcszType, CStringW& rstrDest, bool bCheckForConversion = false);
T_FUNC CStringW T_API	TLoadUnicodeTextFromResource (UINT nID, LPCTSTR pcszType, bool bCheckForConversion = false);

T_FUNC void    T_API	TLoadAnsiTextFromResource (HMODULE hModule, LPCTSTR pcszName, LPCTSTR pcszType, CStringA& rstrDest);
T_FUNC CStringA T_API	TLoadAnsiTextFromResource (UINT nID, LPCTSTR pcszType);

// Functions to bring multiline strings to convenient format
T_FUNC void T_API		TMakeText0D0A(wstring* pstrDest, const wchar_t* pcwszBegin, const wchar_t* pcwszEnd);
T_FUNC void T_API		TMakeText0A(wstring* pstrDest, const wchar_t* pcwszBegin, const wchar_t* pcwszEnd);

// To make GetModuleFileName safe and atomic
T_FUNC CString T_API	TGetModuleFileName(HMODULE hmod);
T_FUNC bool T_API		TSplitFullFileName(CString const& strFullFileName, CString& strDirectory, CString& strFileName);

T_FUNC BOOL T_API       TIs64WowPresent(); //return TRUE if running under Wow64

//run msc file by mmc depend on 64 bit version of or not
T_FUNC BOOL T_API       TRunMSCFile(CString const& rstrMscFile, BOOL bUseSystemPathToMsc, CString const& rstrParametrs, CString const& rstrWorkDir);

// Environmant utils
T_FUNC CString T_API	TExpandEnvironmentStrings(const CString& strSource);

// CMNGuid utils
T_FUNC bool T_API		TFromVariant(TOleVariant const& rcSource, CMNGuid& guidDest);
T_FUNC TOleVariant T_API TToVariant(CMNGuid const& guidSource, VARTYPE enDesiredType);


// No case sense find predicates
class T_CLASS TFindStringNoCaseSens
{
protected:
	CString m_strValue;

public:
	TFindStringNoCaseSens(CString const& strValue)
	{
		m_strValue = strValue;
	}

	bool operator() (CString const& strValue) const
	{
		return m_strValue.CompareNoCase(strValue) == 0 ? true: false;
	}
};

template<class MapValueType>
class TFindMapKeyNoCaseSens
{
protected:
	CString m_strKey;

public:
	TFindMapKeyNoCaseSens(CString const& strKey):
	  m_strKey(strKey)
	{
	}

	bool operator()(MapValueType const& value) const
	{
		return m_strKey.CompareNoCase(value.first) == 0 ? true: false;
	}
};
