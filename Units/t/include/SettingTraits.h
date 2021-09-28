//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Settings
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
// Description:  Program settings storage provider, taken from RSDN and refactored
//--------------------------------------------------------------------------------


#pragma once


//#include "T/include/Buf.h"
#include "T/include/Object.h"
#include "T/Include/T.h"


class TSettings;

enum TSettingsStorageModeEnum {ssmRegistry = 0, ssmStream = 1};

//
//  TSettingTraits
//

class T_CLASS TSettingTraits: public TObject
{
public:
	typedef TSettingTraits	Class;
	typedef CMNSmartPtr<Class>	Ptr;

public:
    virtual int		GetInt       (LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault) = 0;
    virtual BOOL	WriteInt     (LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue) = 0;
    virtual CString	GetString    (LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL) = 0;
    virtual BOOL	WriteString  (LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue) = 0;
	virtual vector<BYTE> GetBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry) = 0;
    virtual BOOL    WriteBinary  (LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData,  UINT nBytes) = 0;
};
