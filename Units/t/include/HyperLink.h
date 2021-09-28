//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: MFCext
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: HiperLink logic 
//--------------------------------------------------------------------------------


#pragma once

#include "T/include/T.h"

class T_CLASS THyperlink : public CString 
{
	friend unsigned __stdcall OpenURL(void* pParam);

public:
	THyperlink(const CString& strLink = CString());
	~THyperlink();
	const THyperlink& operator=(const CString& str);
	
	HINSTANCE			Navigate();
	void				NavigateAsync();
protected:
	static HINSTANCE	DoNavigate(LPCTSTR pszURL);	
};


