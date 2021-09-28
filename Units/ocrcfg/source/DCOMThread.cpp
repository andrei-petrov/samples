//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Thread for background operations
//--------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "cmn/include/misc.h"
#include "cmn/include/Log.h"
#include "cmn/include/Exception.h"
#include "CMN/include/PolicyConfigure.h"
#include "CMN/include/FirewallConfigure.h"
#include "rgs/include/ServiceController.h"
#include "scm/include/SchemaDef.h"
#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/ConfigApp.h"
#include "wzr/include/Thread.h"
#include "ocrcfg/include/DCOMThread.h"
#include "ocrcfg/include/misc.h"

#include "SCDCOMPerm/dcomperm/dcomperm.h"
using namespace dcomperm;


CDCOMThread::CDCOMThread(HWND hParentWnd, const CString& strLogin)
    : WZRThread(hParentWnd)
{
	m_strGrantLogin = strLogin;
}

CDCOMThread::~CDCOMThread()
{
}

BOOL CDCOMThread::InitInstance()
{
	CoInitialize(NULL);
    CWinThread::InitInstance();

	BOOL	bError = FALSE;
	const DWORD dwSteps = 3;
	DWORD dwStep = 1;

	::PostMessage(m_hParentWnd, WM_OCRCFG_DCOMCONFIGURE_START, NULL, dwSteps);

	bool bOk = false;
	try
	{
		// Общие настройки
		//CMNDComPermissions dcomPerm;
		//dcomPerm.SetServerDComPerm(strServerAppId);
		TCHAR szServerAppId[] = _T("{C95C1600-05A6-4E14-AFD5-9B0D9A01D814}");	// from samsvc/include/Module.h
		SetServerDComPerm(szServerAppId, (TCHAR*)(const TCHAR*)m_strGrantLogin);
		dwStep++;

		// настройки политики безопасности
		::PostMessage(m_hParentWnd, WM_OCRCFG_DCOMCONFIGURE_STAGE, dwStep, IDS_OCRCFG_DCOM_POLICY_PROCESS);
		CMNConfigureLocalPolicySecurity();
		if ( IsGlobalPolicySet() )
		{
			CString strWarning;
			strWarning.LoadString(IDS_OCRCFG_DCOM_GLOBAL_POLICY_WARNING);
			CMNLog(_T("DCOM configure warning: %s"), (LPCTSTR)strWarning);
			if ( ! theApp.m_bUnattendedMode )
			{
				AfxMessageBox(strWarning, MB_OK|MB_ICONWARNING);
			}
		}
		dwStep++;

		// настройки файрвола
		::PostMessage(m_hParentWnd, WM_OCRCFG_DCOMCONFIGURE_STAGE, dwStep, IDS_OCRCFG_DCOM_FIREWALL_PROCESS);
		CMNFirewallConfigure fwCfg;
        fwCfg.Initialize();
		if (fwCfg.IsFirewallStarted())
        {

		    bOk = fwCfg.SetRemoteAdmin(true);
		    if ( ! bOk )
		    {
			    CMNException* pEx = new CMNException(IDS_OCRCFG_DCOM_REMOTE_ADMIN_ERROR);
			    pEx->Throw();
		    }
    		
		    const int nPort = 135;
		    bOk = fwCfg.OpenTCPPort(nPort, true);
		    if ( ! bOk )
		    {
			    CMNException* pEx = new CMNException(IDS_OCRCFG_DCOM_OPEN_PORT_ERROR);
			    *pEx << nPort;
			    pEx->Throw();
		    }
    		
		    CString strName, strPath;
		    bOk = false;
		    RGSServiceController controller((bstr_t)L"", (bstr_t)cszSAMSerivceName); 
		    LPQUERY_SERVICE_CONFIG	lpServiceConfig;
		    controller.OpenSCMgr();
            controller.Open();
		    if ( controller.QueryServiceConfig(&lpServiceConfig) == TRUE )
		    {
			    if (lpServiceConfig->lpDisplayName != NULL) strName = lpServiceConfig->lpDisplayName;
			    if (lpServiceConfig->lpBinaryPathName != NULL) strPath = lpServiceConfig->lpBinaryPathName;
			    const TCHAR szQuota[] = _T("\"");
			    strPath.TrimLeft(szQuota);
			    strPath.TrimRight(szQuota);
			    if ((!strName.IsEmpty()) && (!strPath.IsEmpty()))
			    {
				    bOk = fwCfg.EnableApplication(strName, strPath, true);
			    }
			    controller.FreeConfigData( lpServiceConfig );
		    }
		    if ( ! bOk )
		    {
			    CMNException* pEx = new CMNException(IDS_OCRCFG_DCOM_FIREWALL_EXCEPTION_APP_ERROR);
			    *pEx << strName;
			    pEx->Throw();
		    }
        }
		dwStep++;        
		::PostMessage(m_hParentWnd, WM_OCRCFG_DCOMCONFIGURE_STAGE, dwStep, 0);
		m_strThreadResults.LoadString(IDS_OCRCFG_DCOM_CONFIGURE_RESULTS);
		CMNLog(_T("[CDCOMThread::InitInstance]: %s"), m_strThreadResults);
	}
	catch(CMNException* pEx)
	{
		m_strError = CMNException::GetAllErrorMessages(pEx, _T("\n"));
        CMNLog(_T("[CDCOMThread::InitInstance]: %s"), m_strError);
		bError = TRUE;
		pEx->Delete();
	}

	::PostMessage(m_hParentWnd, WM_OCRCFG_DCOMCONFIGURE_FINISH, bError, NULL);	

	return TRUE;
}


int CDCOMThread::ExitInstance()
{
	CoUninitialize();
	return CWinThread::ExitInstance();
}
