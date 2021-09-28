#pragma once

#include "wzr/include/Thread.h"
typedef WZRThread CConfigThread;

/*
class CConfigThread : public CWinThread
{
//	DECLARE_DYNCREATE(CCreateDBThread)
protected:
    HWND        m_hParentWnd;    
    CString     m_strError;
	CString		m_strThreadResults;

public:    
	CConfigThread(HWND hParentWnd);           // protected constructor used by dynamic creation
	virtual ~CConfigThread();

public:
    
    CString GetErrorString() { return m_strError; }
	CString GetThreadResults() { return m_strThreadResults; }

protected:
//	DECLARE_MESSAGE_MAP()
};
*/

