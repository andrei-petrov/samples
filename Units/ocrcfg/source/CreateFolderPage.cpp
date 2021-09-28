//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Interface for thread for background operations
//--------------------------------------------------------------------------


#include "stdafx.h"

#include <shobjidl.h> 

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

#include "ocrcfg/include/CreateFolderPage.h"
#include "ocrcfg/resource/resource.h"
//#include "samcfg/resource/samcfg.rc"

#pragma warning(disable: 4800)

const TCHAR cszSearchItemName[] = _T("<Поиск...>");

// CCreateFolderPage dialog

//IMPLEMENT_DYNAMIC(CCreateFolderPage, WZRPage)

CCreateFolderPage::CCreateFolderPage(COcrConfigData&  rConfigData)
	: WZRPage(IDD_OCRCFG_CREATEFOLDER_PAGE, IDS_OCRCFG_CREATEFOLDER_TITLE, IDS_OCRCFG_CREATEFOLDER_SUBTITLE),
	m_bError(true),
	m_rConfigData(rConfigData)	
{    
	m_psp.dwFlags |= PSP_HASHELP;

	m_strProjectFolder	= _T("C:\\Server");
	m_strScansFolder	= _T("C:\\Scans");
}

CCreateFolderPage::~CCreateFolderPage()
{
	
}

BEGIN_MESSAGE_MAP(CCreateFolderPage, WZRPage)
    //{{AFX_MSG_MAP(CCreateFolderPage)    
	ON_MESSAGE		( PSM_QUERYSIBLINGS,			OnQuerySiblings)
//    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_START, OnScriptExcecutionStart  )
//    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_STAGE, OnScriptExcecutionStage  )
//    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_FINISH, OnScriptExcecutionFinish )
//    ON_MESSAGE      ( WM_OCRCFG_CUSTOMSCRIPT_EXCECUTION_START, OnCustomScriptExcecutionStart )
	ON_EN_CHANGE	( IDC_OCRCFG_CREATEFOLDER_EDIT_PROJECT_FOLDER,		OnNamesChange)
	ON_BN_CLICKED	( IDC_OCRCFG_CREATEFOLDER_BUTTON_BROWSE_PROJECT,	OnBrowseProject )
	ON_BN_CLICKED	( IDC_OCRCFG_CREATEFOLDER_BUTTON_BROWSE_SCANS,		OnBrowseScans)

	ON_BN_CLICKED	( IDC_WZR_CHECK_SKIP, OnSkipBtnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCreateFolderPage::DoDataExchange(CDataExchange* pDX)
{
//  	WZRPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CCreateFolderPage)
	//DDX_Control	(pDX, IDC_OCRCFG_CREATEDB_COMBO_SQLSERVER,	m_comboSqlServer);
	DDX_Control	(pDX, IDC_OCRCFG_CREATEFOLDER_EDIT_PROJECT_FOLDER,		m_edtProjectFolder);
    DDX_Text	(pDX, IDC_OCRCFG_CREATEFOLDER_EDIT_PROJECT_FOLDER,		m_strProjectFolder);

	DDX_Control	(pDX, IDC_OCRCFG_CREATEFOLDER_EDIT_SCANS_FOLDER,		m_edtScansFolder);
	DDX_Text	(pDX, IDC_OCRCFG_CREATEFOLDER_EDIT_SCANS_FOLDER,		m_strScansFolder);
    //}}AFX_DATA_MAP
}


BOOL CCreateFolderPage::OnInitDialog()
{
    WZRPage::OnInitDialog();
	return TRUE;
}


BOOL CCreateFolderPage::OnSetActive()
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if ( ! pApp ) 
		return FALSE;

	m_rConfigData.LoadFromRegistry();

	if ( m_rConfigData.m_strProjectFolder.GetLength() )
		m_strProjectFolder	= m_rConfigData.m_strProjectFolder;	

	if (m_rConfigData.m_strScansFolder.GetLength())
		m_strScansFolder	= m_rConfigData.m_strScansFolder;
		
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


BOOL CCreateFolderPage::OnKillActive()
{	
	if (!m_bError)
	{
		m_rConfigData.m_strProjectFolder	= m_strProjectFolder;
		m_rConfigData.m_strScansFolder		= m_strScansFolder;
		m_rConfigData.SaveToRegistry();
	}
	return WZRPage::OnKillActive();
}

LRESULT CCreateFolderPage::OnWizardBack()
{
	return IDD_OCRCFG_CREDENTIALS_PAGE;
}

LRESULT CCreateFolderPage::OnWizardNext()
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
			return IDD_OCRCFG_CREATEDB_PAGE;
		}
		
		HRESULT hr; 
		hr = CMNCreateFullDirectory(m_strProjectFolder);
		CMNCheckError(hr);
		ShareFolder(m_strProjectFolder, L"server", L"RIO.OCR project share");
		CreateProjectSubFolders();

		hr = CMNCreateFullDirectory(m_strScansFolder);
		CMNCheckError(hr);
		ShareFolder(m_strScansFolder, L"scans", L"RIO.OCR images share");

		CString str;
		str.FormatMessage(IDS_OCRCFG_PROJECT_FOLDER_CHOOSEN, m_strProjectFolder);
		m_strThreadResults += str;
		m_strThreadResults += _T(" ");
		str.FormatMessage(IDS_OCRCFG_SCANS_FOLDER_CHOOSEN, m_strScansFolder);
		m_strThreadResults += str;
		
		// store credentials
		m_bError = false;
		return IDD_OCRCFG_CREATEDB_PAGE; // stay at this page until logon completes
	}
	catch(TException* pTEx)
	{
		CAtlString strErrMessage = TException::GetAllErrorMessages(pTEx);
		CMNLog(_T("[CCreateFolderPage::OnWizardNext]: %s"), strErrMessage);
		pTEx->ReportError();
		pTEx->Delete();
		return -1;
	}
}


void CCreateFolderPage::SetButtons()
{
	//if ( ! theApp.m_bUnattendedMode)
	{
		UpdateData();

		// check is sql server name and DB name fields is empty
		if(m_strProjectFolder.IsEmpty() || m_strProjectFolder.IsEmpty())
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



HRESULT CCreateFolderPage::ShareFolder(LPCTSTR pszFolder, LPCTSTR pszNetworkName, LPCTSTR pszRemark)
{
	HRESULT hr = S_OK;
	
	SHARE_INFO_2 info		= { 0 };
	info.shi2_netname		= (LPWSTR)pszNetworkName; // L"server";
	info.shi2_type			= STYPE_DISKTREE;
	info.shi2_remark		= (LPWSTR)pszRemark; // L"RIO.OCR project share";
	info.shi2_permissions	= ACCESS_ALL;
	info.shi2_max_uses		= -1;
	info.shi2_path			= (LPWSTR)pszFolder; // m_strProjectFolder; // L"C:\\local\\path";	
	info.shi2_passwd		= NULL;

	NET_API_STATUS			dwRes;
	
	try
	{
		dwRes = NetShareDel(NULL, (LPWSTR)pszNetworkName, 0); // delete if exist
		if (dwRes == ERROR_SUCCESS)
			CMNLog(_T("[CCreateFolderPage::ShareFolder]: Old share %s deleted"), pszNetworkName);
		dwRes = NetShareAdd(NULL, 2, (BYTE*)&info, NULL);
		if (dwRes != ERROR_SUCCESS && dwRes != NERR_DuplicateShare)
			CMNCheckWIN32Error(dwRes);
		CMNLog(_T("[CCreateFolderPage::ShareFolder]: New share %s created"), pszNetworkName);
		/*
		ERROR_ACCESS_DENIED
		ERROR_INVALID_LEVEL
		ERROR_INVALID_NAME
		ERROR_INVALID_PARAMETER				
		NERR_DuplicateShare
		NERR_RedirectedPath
		NERR_UnknownDevDir
		*/
	}
	catch (CMNException* pEx)
	{
		TException* pHiLevelEx = new TException(IDE_OCRCFG_FOLDER_SHARE_FAILED);
		*pHiLevelEx << (LPCTSTR)pszFolder; // m_strProjectFolder;
		*pHiLevelEx += *pEx;
		ThrowException(pHiLevelEx);		
	}
	return hr;
}


// Test credentials entered
LRESULT CCreateFolderPage::CreateProjectSubFolders()
{
	HRESULT hr = S_OK;

	CString strCentralServer	= m_strProjectFolder + _T("\\CentralServer");
	CString strCustomPages		= m_strProjectFolder + _T("\\CustomPages");
	CString strHocr				= m_strProjectFolder + _T("\\Hocr");
	CString strLogs				= m_strProjectFolder + _T("\\Logs");
	CString strPdf				= m_strProjectFolder + _T("\\Pdf");
	CString strProjectSource	= m_strProjectFolder + _T("\\ProjectSource");
	CString strScaled			= m_strProjectFolder + _T("\\Scaled");
	CString strSmallFiles		= m_strProjectFolder + _T("\\SmallFiles");
	CString strTemplates		= m_strProjectFolder + _T("\\Templates");
	CString strText				= m_strProjectFolder + _T("\\Text");
	CString strThumbnails		= m_strProjectFolder + _T("\\Thumbnails");

	try
	{
		hr = CMNCreateFullDirectory(strCentralServer);
		CMNCheckError(hr);
		hr = CMNCreateFullDirectory(strCustomPages);
		CMNCheckError(hr);
		hr = CMNCreateFullDirectory(strHocr);
		CMNCheckError(hr);
		hr = CMNCreateFullDirectory(strLogs);
		CMNCheckError(hr);
		hr = CMNCreateFullDirectory(strPdf);
		CMNCheckError(hr);
		hr = CMNCreateFullDirectory(strProjectSource);
		CMNCheckError(hr);
		hr = CMNCreateFullDirectory(strScaled);
		CMNCheckError(hr);
		hr = CMNCreateFullDirectory(strSmallFiles);
		CMNCheckError(hr);
		hr = CMNCreateFullDirectory(strTemplates);
		CMNCheckError(hr);
		hr = CMNCreateFullDirectory(strText);
		CMNCheckError(hr);
		hr = CMNCreateFullDirectory(strThumbnails);
		CMNCheckError(hr);
	}
	catch (CMNException* pEx)
	{
		TException* pHiLevelEx = new TException(IDE_OCRCFG_SUBFOLDERS_CREATION_FAILED);
		*pHiLevelEx << (LPCTSTR)m_strProjectFolder;
		*pHiLevelEx += *pEx;
		ThrowException(pHiLevelEx);
	}	   	
    return 0;
}

// checks for two templates and one SVE to be installed and fills out the edit control with missed components names


// CCreateFolderPage message handlers


LRESULT CCreateFolderPage::OnQuerySiblings(WPARAM wParam, LPARAM lParam)
{
    if (wParam == WM_OCRCFG_QUERY_CREATEFOLDER)
    {
        ::memcpy((void*) lParam, (void*) m_strThreadResults.GetString(), m_strThreadResults.GetLength() * sizeof(TCHAR));
		return 1; // answer
    }	
    return 0; // ignore
}



BOOL CCreateFolderPage::OpenDirectory(CString& strSelectedDirectory, LPCTSTR pszTitle)
{
	BROWSEINFO   bi;
	ZeroMemory	(&bi, sizeof(bi));
	TCHAR		szDisplayName[MAX_PATH];
	szDisplayName[0] = ' ';

	//TCHAR	szPath[MAX_PATH];
	//lstrcpy(szDisk, _tgetenv(_T("SystemDrive")) );
	//BOOL bRes = ::SHGetSpecialFolderPath(NULL, szPath, CSIDL_DRIVES, FALSE); // My Computer, This PC

	PIDLIST_ABSOLUTE pidlRoot = NULL; // = ::SHSimpleIDListFromPath(szPath);	
	HRESULT hr = SHGetFolderLocation(0, CSIDL_DRIVES, 0, 0, &pidlRoot);
	
	bi.hwndOwner		= NULL;
	bi.pidlRoot			= pidlRoot;
	bi.pszDisplayName	= szDisplayName;
	bi.lpszTitle		= pszTitle; // _T("Выберите папку для проекта сканирования. Создайте её при необходимости.");
	bi.ulFlags			= BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_RETURNFSANCESTORS | BIF_NEWDIALOGSTYLE; // | BIF_USENEWUI;
	bi.lParam			= NULL;
	bi.iImage			= 0;

	LPITEMIDLIST	pidl = ::SHBrowseForFolder(&bi);
	TCHAR			szPathName[256];
	BOOL			bRet = FALSE;
	if (NULL != pidl)
	{
		bRet = ::SHGetPathFromIDList(pidl, szPathName);
		if (bRet)
		{
			strSelectedDirectory = &szPathName[0];
		}
	}

	ILFree(pidl);
	ILFree(pidlRoot);
	return bRet;
}


void CCreateFolderPage::OnNamesChange()
{
	SetButtons();
}


void CCreateFolderPage::OnBrowseProject()
{	
	CAtlString strDomain;
	CAtlString strUsername;

	CMNSplitExtendedName(m_rConfigData.m_strLogin, strDomain, strUsername);
	HANDLE hToken;
	BOOL bResult;

	bResult = ::LogonUser(strUsername, strDomain, m_rConfigData.m_strPassword, LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &hToken);
	if (bResult)
	{
		bResult = ::ImpersonateLoggedOnUser(hToken);
		CMNLog(_T("[CCreateFolderPage::LogonUser]: impersonalted as '%s'"), m_rConfigData.m_strLogin);
	}

	
	BOOL bFolderChoosen = OpenDirectory(m_strProjectFolder, _T("Выберите папку для проекта распознавания. Создайте её при необходимости."));
	if (bFolderChoosen)
	{		
		UpdateData(FALSE);
		OnNamesChange();
	}
	if (bResult)
		::RevertToSelf();
	//CMNCheckLastError(bResult);

	/*
	int nResult;
	CFolderPickerDialog dlg; // (LPCTSTR lpszFolder = NULL, DWORD dwFlags = 0, CWnd* pParentWnd = NULL, DWORD dwSize = 0);
	nResult = dlg.DoModal();
	if (nResult == IDOK)
	{
		m_strProjectFolder = dlg.GetPathName();
		UpdateData(FALSE);
		OnNamesChange();
	}
	*/
}


void CCreateFolderPage::OnBrowseScans()
{
	CAtlString strDomain;
	CAtlString strUsername;

	CMNSplitExtendedName(m_rConfigData.m_strLogin, strDomain, strUsername);
	HANDLE hToken;
	BOOL bResult;

	bResult = ::LogonUser(strUsername, strDomain, m_rConfigData.m_strPassword, LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &hToken);
	if (bResult)
	{
		bResult = ::ImpersonateLoggedOnUser(hToken);
		CMNLog(_T("[CCreateFolderPage::LogonUser]: impersonalted as '%s'"), m_rConfigData.m_strLogin);
	}
	BOOL bFolderChoosen = OpenDirectory(m_strScansFolder, _T("Выберите папку с отсканированными документами. Создайте её при необходимости."));
	if (bFolderChoosen)
	{
		UpdateData(FALSE);
		OnNamesChange();
	}
	if (bResult)
		::RevertToSelf();
}



void CCreateFolderPage::OnSkipBtnClick()
{
	UpdateData();

	if ( m_iSkipPage )
	{
	}
	else
	{
		// enable next button
		if(!m_strProjectFolder.IsEmpty() && !m_strProjectFolder.IsEmpty())
		{
			m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		}
		else
		{
			CAtlString strMessage;
			strMessage.LoadString(IDS_OCRCFG_DB_EMPTY_FIELD);
			CMNLog(_T("[CCreateFolderPage::OnWizardNext]: %s"), strMessage);
			AfxMessageBox(strMessage);
		}
	}

	UpdateData(FALSE);
}