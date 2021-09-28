//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Thread for background operations
//--------------------------------------------------------------------------

#pragma once


class CDCOMThread : public WZRThread
{
	CString	m_strGrantLogin;

public:
	CDCOMThread(HWND hParentWnd, const CString& strLogin);
	virtual ~CDCOMThread();

public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();

};


