//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: MFCext
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: HiperLink logic
//--------------------------------------------------------------------------------

#include "StdAfx.h"
#include "T/include/HyperLink.h"



THyperlink::THyperlink(const CString& strLink) 
	: CString(strLink) 
{ 
}

THyperlink::~THyperlink()
{ 
}


const THyperlink& THyperlink::operator=(const CString& str) 
{
	CString::operator=(str);
	return *this;
}



HINSTANCE THyperlink::Navigate()
{
	return DoNavigate( (LPCTSTR)*this );
}


HINSTANCE THyperlink::DoNavigate(LPCTSTR pszURL)
{
    HINSTANCE   hInst;
    CString str = pszURL;
    if (str.Find(TCHAR('@')) != -1) // '@' sign present    
	{
		str = _T("mailto:") + str;		
	}
    else
	{
		if (str.Find(_T("http://")) == -1) // http:// prefix absent
			str = _T("http://") + str;        
	}
	hInst = ShellExecute(0, _T("open"), str, 0, 0, SW_SHOWNORMAL);
    return hInst;
}


unsigned __stdcall OpenURL(void* pParam)
{
	LPCTSTR pszURL = (LPCTSTR)pParam; 		
	THyperlink::DoNavigate(pszURL);
    return 0;
}



void THyperlink::NavigateAsync()
{    
    HANDLE  hThread;
    unsigned threadID;    
    hThread = (HANDLE)_beginthreadex(NULL, 0, &OpenURL, (void*)(LPCTSTR)*this, 0, &threadID);
    CloseHandle(hThread);
}


