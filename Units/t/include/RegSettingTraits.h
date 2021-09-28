//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Settings
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
// Description:  Program settings storage. Get settings from registry.
//--------------------------------------------------------------------------------

#pragma once

#include "T/include/SettingTraits.h"
#include "T/Include/T.h"


class T_CLASS TRegSettingTraits: public TSettingTraits
{
public:
	typedef TRegSettingTraits	Class;
	typedef CMNSmartPtr<Class>		Ptr;

public:
    virtual int		GetInt       (LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
    virtual BOOL	WriteInt     (LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
    virtual CString	GetString    (LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL);
    virtual BOOL	WriteString  (LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
	virtual vector<BYTE> GetBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry);
    virtual BOOL    WriteBinary  (LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData,  UINT nBytes);
};
