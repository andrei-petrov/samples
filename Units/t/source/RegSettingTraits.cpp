//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Settings
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
// Description:  Program settings storage.
//--------------------------------------------------------------------------------

#include "StdAfx.h"

#include "T/include/RegSettingTraits.h"


int TRegSettingTraits::GetInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
{
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp);
	if(!pApp)
		return nDefault;
	else
		return pApp->GetProfileInt(lpszSection, lpszEntry, nDefault);
}

BOOL TRegSettingTraits::WriteInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
{
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp);
	if(!pApp)
		return FALSE;
	else
		return pApp->WriteProfileInt(lpszSection, lpszEntry, nValue);
}

CString	TRegSettingTraits::GetString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
{
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp);
	if(!pApp)
		return _T("");
	else
		return pApp->GetProfileString(lpszSection, lpszEntry, lpszDefault);
}

BOOL TRegSettingTraits::WriteString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
{
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp);
	if(!pApp)
		return FALSE;
	else
		return pApp->WriteProfileString(lpszSection, lpszEntry, lpszValue);
}

vector<BYTE> TRegSettingTraits::GetBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry)
{
	vector<BYTE> arr;

	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp);
	if(!pApp)
		return arr;
	else
	{
		BYTE* pData = NULL;
		UINT uiBytes = 0;
		BOOL bRes = pApp->GetProfileBinary(lpszSection, lpszEntry, &pData, &uiBytes);
		if(FALSE == bRes)
			return arr;

		if(!uiBytes)
			return arr;

		arr.resize(uiBytes);
		::memcpy(&(arr[0]), pData, uiBytes);
		delete pData;
	}

	return arr;
}

BOOL TRegSettingTraits::WriteBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes)
{
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp);
	if(!pApp)
		return FALSE;

	return pApp->WriteProfileBinary(lpszSection, lpszEntry, pData, nBytes);
}
