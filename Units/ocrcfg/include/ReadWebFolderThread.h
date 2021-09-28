#pragma once

class CReadWebFolder : public CWinThread
{
	set<CAtlString>*	m_p_setWebFolder;
	CAtlString			m_strReportServerName; 
	HWND				m_hParentWnd;
	HANDLE				m_hEventLock;

public:
	CReadWebFolder(HWND hParentWnd, set<CAtlString>& rout_setReportServerURL, 
		CAtlString& rin_strSQLServerName, HANDLE hEventLock);
	~CReadWebFolder();
	virtual BOOL InitInstance();
};