//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Interface for thread for background operations
//--------------------------------------------------------------------------


#include "stdafx.h"

#include "cmn/include/Log.h"
#include "cmn/include/NetPath.h"
#include "cmn/include/McException.h"
#include "cmn/include/RegKey.h"
#include "cmn/include/Misc.h"
#include "cmn/include/AuthorityGrantor.h"

#include "v/include/Constants.h"

#include "db/include/Constants.h"
#include "db/include/Storage.h"
#include "db/include/SqlBrowser.h"

#include "scm/include/SchemaDef.h"

#include "wzr/include/SelectServersDlg.h"
#include "wzr/include/Page.h"
#include "wzr/include/Sheet.h"
#include "wzr/include/SQLSrvAddrParser.h"
#include "wzr/include/Thread.h"

#include "ocrcfg/include/misc.h"
#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/ConfigApp.h"
//#include "ocrcfg/include/RegServicesThread.h"
#include "ocrcfg/include/CredentialsPage.h"
#include "ocrcfg/resource/resource.h"
//#include "samcfg/resource/samcfg.rc"

#pragma warning(disable: 4800)

const TCHAR cszSearchItemName[] = _T("<Поиск...>");

// CCredentialsPage dialog

//IMPLEMENT_DYNAMIC(CCredentialsPage, WZRPage)

CCredentialsPage::CCredentialsPage(COcrConfigData&  rConfigData)
	: WZRPage(IDD_OCRCFG_CREDENTIALS_PAGE, IDS_OCRCFG_CREDENTIALS_TITLE, IDS_OCRCFG_CREDENTIALS_SUBTITLE),
	m_bError(true),
	m_rConfigData(rConfigData)	
{    
	m_psp.dwFlags |= PSP_HASHELP;
}

CCredentialsPage::~CCredentialsPage()
{
	
}

BEGIN_MESSAGE_MAP(CCredentialsPage, WZRPage)
    //{{AFX_MSG_MAP(CCredentialsPage)    
	ON_MESSAGE		( PSM_QUERYSIBLINGS,		OnQuerySiblings)
//    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_START, OnScriptExcecutionStart  )
//    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_STAGE, OnScriptExcecutionStage  )
//    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_FINISH, OnScriptExcecutionFinish )
//    ON_MESSAGE      ( WM_OCRCFG_CUSTOMSCRIPT_EXCECUTION_START, OnCustomScriptExcecutionStart )
//	ON_CBN_SELENDOK	( IDC_OCRCFG_CREATEDB_COMBO_SQLSERVER, OnSQLComboChange )
	ON_EN_CHANGE    ( IDC_OCRCFG_CREDENTIALS_EDIT_LOGIN, OnNamesChange )
	ON_EN_CHANGE	( IDC_OCRCFG_CREDENTIALS_EDIT_PASSWORD, OnNamesChange )
	ON_BN_CLICKED   ( IDC_OCRCFG_CREDENTIALS_BUTTON_SELECT, OnSelect )
//	ON_CBN_EDITCHANGE(IDC_OCRCFG_CREATEDB_COMBO_SQLSERVER, OnNamesChange )
	ON_BN_CLICKED	( IDC_WZR_CHECK_SKIP, OnSkipBtnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCredentialsPage::DoDataExchange(CDataExchange* pDX)
{
//  	WZRPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CCredentialsPage)
	//DDX_Control	(pDX, IDC_OCRCFG_CREATEDB_COMBO_SQLSERVER,	m_comboSqlServer);
	DDX_Control	(pDX, IDC_OCRCFG_CREDENTIALS_EDIT_LOGIN,		m_edtLogin);
	DDX_Control	(pDX, IDC_OCRCFG_CREDENTIALS_EDIT_PASSWORD,		m_edtPassword);
    //DDX_Text	(pDX, IDC_OCRCFG_CREATEDB_COMBO_SQLSERVER,	m_strSqlServer);
    DDX_Text	(pDX, IDC_OCRCFG_CREDENTIALS_EDIT_LOGIN,		m_strLogin);
	DDX_Text	(pDX, IDC_OCRCFG_CREDENTIALS_EDIT_PASSWORD,		m_strPassword);
    //}}AFX_DATA_MAP
}


BOOL CCredentialsPage::OnInitDialog()
{
    WZRPage::OnInitDialog();
	return TRUE;
}


BOOL CCredentialsPage::OnSetActive()
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if ( ! pApp ) 
		return FALSE;
	   
	m_rConfigData.LoadFromRegistry();		
	m_strLogin		= m_rConfigData.m_strLogin;
	m_strPassword	= m_rConfigData.m_strPassword;

	if (!pApp->m_bInstallerMode) // in installer mode GetCurrentName produce something like WORKGROUP\Computername$, that is unacceptable
	{
		if (m_strLogin.GetLength() == 0)
		{
			m_strLogin = GetCurrentUserName();
		}
	}
	UpdateData(FALSE);

	// in installer mode 'skip' checkbox is not visible
    if (pApp->m_bInstallerMode)
    {
        if (m_btnSkipPage.m_hWnd)
            m_btnSkipPage.ShowWindow(SW_HIDE);
    }

    if (pApp->m_bUnattendedMode)
	{
		::PostMessage(m_pSheet->GetSafeHwnd(), PSM_PRESSBUTTON, PSBTN_NEXT, 0); // and simulate next button press
	}

	BOOL bRes = WZRPage::OnSetActive();
	SetButtons();
	return bRes;    
}


BOOL CCredentialsPage::OnKillActive()
{	
	if(!m_bError)
	{
		m_rConfigData.m_strLogin	= m_strLogin;
		m_rConfigData.m_strPassword = m_strPassword;
		m_rConfigData.SaveToRegistry();
	}	
	return WZRPage::OnKillActive();
}

LRESULT CCredentialsPage::OnWizardBack()
{
	return IDD_OCRCFG_WELCOME_PAGE;
}

LRESULT CCredentialsPage::OnWizardNext()
{
	try
	{
		if ( UpdateData(TRUE) == FALSE )
		{
			if ( ! m_iSkipPage )
			{
				ThrowException(new TException(IDE_OCRCFG_INCORRECT_INPUT));
			}
		}

		if ( m_iSkipPage )
		{
			m_bError = false;			
			//OnKillActive();
			return IDD_OCRCFG_CREATEFOLDER_PAGE;
		}
		
		GrantLogOnAsServiceRight();
		LogonUser();

		// store credentials
		m_rConfigData.m_strLogin	= m_strLogin;
		m_rConfigData.m_strPassword = m_strPassword;
		m_bError = false;
		return IDD_OCRCFG_CREATEFOLDER_PAGE; // stay at this page until logon completes
	}
	catch(TException* pTEx)
	{
		CAtlString strErrMessage = TException::GetAllErrorMessages(pTEx);
		CMNLog(_T("[CCredentialsPage::OnWizardNext]: %s"), strErrMessage);
		pTEx->ReportError();
		pTEx->Delete();
		return -1;  // stay on this page
	}
}


void CCredentialsPage::SetButtons()
{
	if ( ! theApp.m_bUnattendedMode)
	{
		UpdateData();

		// check is sql server name and DB name fields is empty
		if(m_strLogin.IsEmpty() || m_strPassword.IsEmpty())
		{
			// disable Next button
			m_pSheet->SetWizardButtons(PSWIZB_BACK);
		}
		else
		{
			// enable next button
			m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		}
	}
}



HRESULT CCredentialsPage::GrantLogOnAsServiceRight()
{
	HRESULT hr = S_OK;
	PSID pSid = NULL;

	try
	{
		CMNAuthorityGrantor authorityGrantor;
		NTSTATUS status = authorityGrantor.OpenPolicy(NULL, POLICY_CREATE_ACCOUNT | POLICY_LOOKUP_NAMES);
		CMNCheckNtStatus(status);

		BOOL bResult = authorityGrantor.GetAccountSid(NULL, m_strLogin, &pSid);
		CMNCheckLastError(bResult);

		status = authorityGrantor.SetPrivilegeOnAccount(pSid, _T("SeServiceLogonRight"), TRUE);
		CMNCheckNtStatus(status);

		status = authorityGrantor.ClosePolicy();
		CMNCheckNtStatus(status);
		if (pSid)
			::LocalFree(pSid);

		CString str;
		str.FormatMessage(IDS_OCRCFG_LOGON_AS_SERVICE_PERMISSION_GRANTED, m_strLogin);
		m_strThreadResults += str;
		//m_strThreadResults += _T(" ");
	}
	catch (CMNException* pEx)
	{
		if (pSid)
			::LocalFree(pSid);
		TException* pHiLevelEx = new TException(IDE_OCRCFG_LOGON_AS_SERVICE_PERMISSION_GRANT_ERROR);
		*pHiLevelEx << (LPCTSTR)m_strLogin;
		*pHiLevelEx += *pEx;
		ThrowException(pHiLevelEx);
	}	
	return hr;
}


// Test credentials entered
LRESULT CCredentialsPage::LogonUser()
{
	CAtlString strDomain;
	CAtlString strUsername;

	CMNSplitExtendedName(m_strLogin, strDomain, strUsername);
	HANDLE hToken;
	BOOL bResult;
		
	bResult = ::LogonUser(strUsername, strDomain, m_strPassword, LOGON32_LOGON_SERVICE, LOGON32_PROVIDER_DEFAULT, &hToken);
	CMNCheckLastError(bResult);
	//CloseHandle(hToken);

	CMNLog(_T("[CCredentialsPage::LogonUser]: impersonalted as '%s'"), m_strLogin);
	
	/*
	try
	{
		HANDLE hToken;
		BOOL bResult = ::LogonUser(strUsername, strDomain, m_strPassword, LOGON32_LOGON_SERVICE, LOGON32_PROVIDER_DEFAULT, &hToken);		
		CMNCheckLastError(bResult);
	}
	catch (CMNException* pEx)
	{
		TException* pHiLevelEx = new TException(IDE_OCRCFG_LOGON_AS_SERVICE_PERMISSION_ABSENT);
		*pHiLevelEx << (LPCTSTR)m_strLogin;		
		*pHiLevelEx += *pEx;
		ThrowException(pHiLevelEx);
	}
	*/
    return 0;
}


CString CCredentialsPage::GetCurrentUserName()
{
	LPTSTR	szCurrentUserLogin = NULL;
	ULONG	nSize = 0;
	CString strCurrentUserNname;

	if (::GetUserNameEx(NameSamCompatible, szCurrentUserLogin, &nSize) == FALSE)
	{
		DWORD	dwErr = ::GetLastError();
		if (dwErr == ERROR_MORE_DATA)
		{
			szCurrentUserLogin = new TCHAR[nSize + 1];
			memset(szCurrentUserLogin, 0, (nSize + 1) * sizeof(TCHAR));
			if (::GetUserNameEx(NameSamCompatible, szCurrentUserLogin, &nSize) == TRUE)
			{
				strCurrentUserNname = szCurrentUserLogin;
			}
			delete[] szCurrentUserLogin;
		}
	}
	return strCurrentUserNname;
}

// checks for two templates and one SVE to be installed and fills out the edit control with missed components names


// CCredentialsPage message handlers


LRESULT CCredentialsPage::OnQuerySiblings(WPARAM wParam, LPARAM lParam)
{
    if (wParam == WM_OCRCFG_QUERY_CREDENTIALS)
    {
        ::memcpy((void*) lParam, (void*) m_strThreadResults.GetString(), m_strThreadResults.GetLength() * sizeof(TCHAR));
		return 1; // answer
    }	
    return 0; // ignore
}

/*
LRESULT CCredentialsPage::OnScriptExcecutionStart (WPARAM wParam, LPARAM lParam)
{
	m_strStage.FormatMessage(IDS_OCRCFG_SVC_CREATE_PROCESS);
	UpdateData(FALSE);
	return 0;
}

LRESULT CCredentialsPage::OnScriptExcecutionStage (WPARAM wParam, LPARAM lParam)
{
	CString strFormat;
	DWORD dwPercent = (DWORD) wParam;	
    DWORD iPos = (DWORD)m_ctlProgress.GetPos();
	TRACE(_T("[CCredentialsPage::OnScriptExcecutionStage] %i, %i\n"), dwPercent, iPos);
    if (iPos < dwPercent)
    {
        m_ctlProgress.StepIt();
		TRACE(_T("[CCredentialsPage::OnScriptExcecutionStage] StepIt()\n"));
		ShowProgress();
    }
    return 0;
}

LRESULT CCredentialsPage::OnScriptExcecutionFinish (WPARAM wParam, LPARAM lParam)
{
	m_bError = (BOOL) wParam;

	CString strError = m_pThread->GetErrorString();
	OnThreadFinished();

	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	if(!pApp)
		return -1;

	if(m_bError)
	{
		theApp.m_bInstallerMode = FALSE;

		if ( ! theApp.m_bUnattendedMode)
		{
			m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

			TException tex(IDE_OCRCFG_ERROR_STRING);
			tex << strError;
			tex.ReportError();

			return -1;
		}
		else
		{
			// record to log was made in CreateDBThread class
			//::PostMessage(m_pSheet->GetSafeHwnd(), WM_QUIT, 0, 0);
			::PostMessage(m_pSheet->GetSafeHwnd(), PSM_PRESSBUTTON, PSBTN_FINISH, 0);
			//return 0;
		}
	}

	CConfigXMLData& data = pApp->GetConfigXMLData();

	int iActivePage = m_pSheet->GetActiveIndex();
	if (data.IsDeployReports())
		m_pSheet->SetActivePage(iActivePage + 1);
	else if (data.IsImportSoftLibrary())
		m_pSheet->SetActivePage(iActivePage + 2);
	else
		m_pSheet->SetActivePage(iActivePage + 3);

	return 0;
}

LRESULT CCredentialsPage::OnCustomScriptExcecutionStart(WPARAM wParam, LPARAM lParam)
{
	m_strStage.LoadString(IDS_OCRCFG_CUSTOMSCRIPT_EXECUTION);
	UpdateData(FALSE);

	return 0;
}
*/
void CCredentialsPage::OnNamesChange()
{
	SetButtons();
}

#include <ComDef.h>
#include <ObjSel.h>

_COM_SMARTPTR_TYPEDEF(IDsObjectPicker, IID_IDsObjectPicker);

void CCredentialsPage::OnSelect()
{
	IDsObjectPickerPtr ptrObjPick(CLSID_DsObjectPicker);
	// semi-smart pointer to object
	IDataObjectPtr pDataObject;         // result data object
	DSOP_INIT_INFO InitInfo;            // Init Info
	DSOP_SCOPE_INIT_INFO aScopeInit[1]; // Scope Init Info
	HRESULT hr;                         // standard hresult

	// init the DSOP_SCOPE_INIT_INFO
	ZeroMemory(aScopeInit, sizeof(aScopeInit));
	aScopeInit[0].cbSize = sizeof(DSOP_SCOPE_INIT_INFO);

	// all the relevant settings are assigned directly from the dialogs
	aScopeInit[0].flType =	DSOP_SCOPE_TYPE_UPLEVEL_JOINED_DOMAIN |
							DSOP_SCOPE_TYPE_DOWNLEVEL_JOINED_DOMAIN |
							DSOP_SCOPE_TYPE_GLOBAL_CATALOG |
							DSOP_SCOPE_TYPE_ENTERPRISE_DOMAIN |
							DSOP_SCOPE_TYPE_EXTERNAL_UPLEVEL_DOMAIN |
							DSOP_SCOPE_TYPE_EXTERNAL_DOWNLEVEL_DOMAIN |
							DSOP_SCOPE_TYPE_TARGET_COMPUTER |
							DSOP_SCOPE_TYPE_USER_ENTERED_UPLEVEL_SCOPE |
							DSOP_SCOPE_TYPE_USER_ENTERED_DOWNLEVEL_SCOPE;

	aScopeInit[0].flScope = DSOP_SCOPE_FLAG_WANT_PROVIDER_WINNT | DSOP_SCOPE_FLAG_DEFAULT_FILTER_USERS |
							DSOP_SCOPE_FLAG_DEFAULT_FILTER_GROUPS | DSOP_SCOPE_FLAG_STARTING_SCOPE;

	aScopeInit[0].FilterFlags.Uplevel.flBothModes = DSOP_FILTER_USERS; // | DSOP_FILTER_GLOBAL_GROUPS_DL;
	aScopeInit[0].FilterFlags.Uplevel.flMixedModeOnly	= 0x0;		
	aScopeInit[0].FilterFlags.Uplevel.flNativeModeOnly	= 0x0;		
	aScopeInit[0].FilterFlags.flDownlevel = DSOP_DOWNLEVEL_FILTER_USERS; // | DSOP_DOWNLEVEL_FILTER_GLOBAL_GROUPS;
		

	// init the struct
	ZeroMemory(&InitInfo, sizeof(DSOP_INIT_INFO));
	InitInfo.cbSize = sizeof(DSOP_INIT_INFO); 
	InitInfo.pwzTargetComputer = NULL;
	InitInfo.cDsScopeInfos = sizeof(aScopeInit) / sizeof(DSOP_SCOPE_INIT_INFO);
	InitInfo.aDsScopeInfos = aScopeInit;

	// pick up the optional settings
	InitInfo.flOptions = 0;

	// bail out if we could not do anything
	if (ptrObjPick == NULL)
	{
		CMNLog(_T("[CCredentialsPage::OnSelect]: Could not create the required COM object in objsel.dll. Are you running Win2K or XP ?") );
		return;
	}

	// make the call to tell the system what kind of dialog it should
	// display
	hr = ptrObjPick->Initialize(&InitInfo);
	if (!SUCCEEDED(hr))
	{
		CMNLog(_T("[CCredentialsPage::OnSelect]: Something went wrong trying in the call to Initialze(), bailing out..."));
		return;
	}

	// make the call to show the dialog that we want
	hr = ptrObjPick->InvokeDialog(GetSafeHwnd(),
		(IDataObject**)&pDataObject);
	if (!SUCCEEDED(hr) || hr == S_FALSE)
	{
		CMNLog(_T("[CCredentialsPage::OnSelect]: InvokeDialog returned with a failure, or just canceled"));
		return;
	}

	// decode the results from the dialog
	CString strADPath;
	hr = ProcessResults(pDataObject, strADPath);
	if (!SUCCEEDED(hr))
	{
		CMNLog(_T("[CCredentialsPage::OnSelect]: Problem processing the ObjectPicker results, bailing out..."));
		return;
	}
	CMNLog(_T("[CCredentialsPage::OnSelect]: user selected '%s'"), strADPath);

	if (strADPath.Find(_T("WinNT://"), 0) != -1) // local path like WinNT://WORKGROUP/COMPUTER/username
	{
		int iBeforeUserNameSlash = strADPath.ReverseFind( TCHAR('/'));
		CString strUserName		 = strADPath.Right(strADPath.GetLength() - iBeforeUserNameSlash - 1);
		strADPath.TrimRight (strUserName);
		strADPath.TrimRight (TCHAR('/'));

		int iBeforeComputerSlash = strADPath.ReverseFind(TCHAR('/'));
		CString strComputerName  = strADPath.Right(strADPath.GetLength() - iBeforeComputerSlash - 1);

		m_strLogin =  strComputerName;
		m_strLogin += _T("\\");
		m_strLogin += strUserName;
	}
	UpdateData(FALSE);
	SetButtons();
}



HRESULT CCredentialsPage::ProcessResults(IDataObjectPtr& ptrDataObj, CString& strADPath)
{
	HRESULT					hr;					// standard hresult
	STGMEDIUM				stm;				// the storage medium
	FORMATETC				fe;					// formatetc specifier
	PDS_SELECTION_LIST		pDsSelList;			// pointer to our results
	ULONG					i;					// loop iterator
	//LV_ITEM				    lvitem;				// the item

	// Init
	pDsSelList = NULL;

	// Sanity check
	if (ptrDataObj == NULL)
		return E_INVALIDARG;

	// Get the global memory block that contain the user's selections.
	fe.cfFormat = RegisterClipboardFormat(CFSTR_DSOP_DS_SELECTION_LIST);
	fe.ptd = NULL;
	fe.dwAspect = DVASPECT_CONTENT;
	fe.lindex = -1;
	fe.tymed = TYMED_HGLOBAL;

	// grab the data object
	hr = ptrDataObj->GetData(&fe, &stm);
	if (!SUCCEEDED(hr))
		return hr;

	// Retrieve a pointer to DS_SELECTION_LIST structure.
	pDsSelList = (PDS_SELECTION_LIST)GlobalLock(stm.hGlobal);
	if (pDsSelList != NULL)
	{
		// Loop through DS_SELECTION array of selected objects.
		for (i = 0; i < pDsSelList->cItems; i++)
		{
			/*
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = i;
			lvitem.iSubItem = 0;
			lvitem.pszText = _T("");
			lvitem.iImage = NULL;
			m_list.InsertItem(&lvitem); // insert new item
			m_list.SetItemText(i, 0, pDsSelList->aDsSelection[i].pwzName);
			m_list.SetItemText(i, 1, pDsSelList->aDsSelection[i].pwzClass);
			m_list.SetItemText(i, 2, pDsSelList->aDsSelection[i].pwzADsPath);
			m_list.SetItemText(i, 3, pDsSelList->aDsSelection[i].pwzUPN);
			*/
			strADPath = pDsSelList->aDsSelection[i].pwzADsPath;
			break;
		}
		GlobalUnlock(stm.hGlobal);
	}
	else
	{
		hr = E_POINTER;
	}

	ReleaseStgMedium(&stm);

	return hr;
}





void CCredentialsPage::OnSkipBtnClick()
{
	UpdateData();

	if ( m_iSkipPage )
	{
	}
	else
	{
		// enable next button
		if(!m_strLogin.IsEmpty() && !m_strPassword.IsEmpty())
		{
			m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		}
		else
		{
			CAtlString strMessage;
			strMessage.LoadString(IDS_OCRCFG_DB_EMPTY_FIELD);
			CMNLog(_T("[CCredentialsPage::OnWizardNext]: %s"), strMessage);
			AfxMessageBox(strMessage);
		}
	}

	UpdateData(FALSE);
}

