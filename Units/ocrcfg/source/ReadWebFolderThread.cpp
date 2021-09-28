#include "StdAfx.h"

#include "..\include\ReadWebFolderThread.h"
#include "..\include\Constants.h"
#include "..\include\ConfigApp.h"

#include "cmn/include/Log.h"


const TCHAR cszReportingServiceASPApplicationName[] = _T("/ReportService2005.asmx");
const TCHAR cszSearchString[] = _T("are insufficient for performing this operation");


CReadWebFolder::CReadWebFolder(HWND hParentWnd, set<CAtlString>& rout_setWebFolder, 
	CAtlString& rin_strReportServerName, HANDLE hEventLock)
:
m_hParentWnd(hParentWnd),
m_p_setWebFolder(&rout_setWebFolder),
m_strReportServerName(rin_strReportServerName),
m_hEventLock(hEventLock)
{
	m_strReportServerName += cszReportingServiceASPApplicationName;
}


CReadWebFolder::~CReadWebFolder()
{

}


BOOL CReadWebFolder::InitInstance()
{
	CAtlString* pStr = NULL;
	/*
	try
	{
		// start to transmit data to window
		if(IsWindow(m_hParentWnd) != 0)
		{
			// Attempt to lock the shared resource
			DWORD dwRetValue = ::WaitForSingleObject(m_hEventLock, 4000); // Wait 4 s...
			switch (dwRetValue) 
			{
				case  WAIT_OBJECT_0:
				{

					IRptManPtr piRptMan = theApp.GetRptMan();
					if ( piRptMan )
					{
						piRptMan->SetUrl((LPCTSTR)m_strReportServerName);
						bstr_t bstrList = piRptMan->GetFoldersList("/");

						if ( bstrList.length() > 0 )
						{
							CString strList = (const TCHAR*) bstrList;
							int iPos = 0;
							CString strFolder = strList.Tokenize(_T(";"), iPos);
							while ( strFolder.GetLength() > 0 )
							{
								// fill in the set
								m_p_setWebFolder->insert(strFolder);
								strFolder = strList.Tokenize(_T(";"), iPos);
							}
						}
					}
					else
					{
						CAtlString strTemp;
						strTemp.LoadStringW(IDS_OCRCFG_SELECTSERVERS_GETRPTMAN_FAIL);
						CMNLog(_T("[CReadWebFolder::InitInstance]: %s"), strTemp);
					}

					break;
				}
				default:
					break;
			}
		}
	}
	catch(CMNException* pEx)
	{
		CString strAllErrors = CMNException::GetAllErrorMessages(pEx, _T("\n"));
		CMNLog(_T("[CReadWebFolder::InitInstance]: %s"), strAllErrors);

		// check error text 
		int iPos = strAllErrors.Find(cszSearchString);
		if(-1 != iPos)
		{
			pStr = new CAtlString;
			pStr->LoadStringW(IDS_OCRCFG_SELECTSERVERS_WEBFOLDER_ACCESS_DENIED);
		}
		pEx->Delete();

	}

	if(IsWindow(m_hParentWnd) != 0)
	{
		// send message to parent window
		::PostMessage(m_hParentWnd, WM_OCRCFG_READ_WEB_FOLDER_FINISH, (WPARAM)pStr, 0);
	}
	*/
	PostQuitMessage(0);

	return TRUE;
}