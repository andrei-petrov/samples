// CWelcomePage.cpp : implementation file
//

#include "stdafx.h"

#include "cmn/include/McException.h"
#include "cmn/include/RegKey.h"

#include "scm/include/SchemaDef.h"

#include "wzr/include/Page.h"

#include "ocrcfg/include/ConfigApp.h"
#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/WelcomePage.h"
#include "ocrcfg/include/misc.h"
#include "ocrcfg/resource/resource.h"


// CWelcomePage dialog

//IMPLEMENT_DYNAMIC(CWelcomePage, WZRPage)
CWelcomePage::CWelcomePage()
	: WZRPage(IDD_OCRCFG_WELCOME_PAGE, 0, 0),
	m_iUseScenario(0)
{
    m_psp.dwFlags |= PSP_HIDEHEADER; // no header, just watermark on left part of the page

  	m_dwWizardButtonsFlagNormal  = PSWIZB_NEXT;
    m_dwWizardButtonsFlagInstall = PSWIZB_NEXT;

	m_psp.dwFlags |= PSP_HASHELP;
}

CWelcomePage::~CWelcomePage()
{
}

BEGIN_MESSAGE_MAP(CWelcomePage, WZRPage)
	ON_BN_CLICKED(IDC_OCRCFG_WELCOME_RADIO_DEFAULTXML, OnUseConfigFileButton)
	ON_BN_CLICKED(IDC_OCRCFG_WELCOME_RADIO_SELECTSERVERS, OnUseConfigFileButton)
//	ON_BN_CLICKED(IDC_OCRCFG_WELCOME_RADIO_SOFT_LIBRARY, OnUseConfigFileButton)
//	ON_BN_CLICKED(IDC_OCRCFG_WELCOME_RADIO_DCOM, OnUseConfigFileButton)
	ON_BN_CLICKED(IDC_OCRCFG_WELCOME_RADIO_CONFIGXML, OnUseConfigFileButton)
	ON_EN_CHANGE(IDC_OCRCFG_WELCOME_EDIT_CONFIGFILEPATH, OnConfigPathChange)
	ON_BN_CLICKED(IDC_OCRCFG_WELCOME_BUTTON_BROWSE, OnBrowse)
END_MESSAGE_MAP()

void CWelcomePage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Radio  (pDX, IDC_OCRCFG_WELCOME_RADIO_DEFAULTXML, m_iUseScenario);
	DDX_Control(pDX, IDC_OCRCFG_WELCOME_EDIT_CONFIGFILEPATH, m_edtConfigFile);
	DDX_Control(pDX, IDC_OCRCFG_WELCOME_BUTTON_BROWSE, m_btnBrowse);
	if (pDX->m_bSaveAndValidate)
		theApp.m_enScenario = (CUseScenarioEnum)m_iUseScenario;
}

BOOL CWelcomePage::OnInitDialog()
{
    BOOL b = WZRPage::OnInitDialog();

	CWnd* pWndTitle = GetDlgItem (IDC_OCRCFG_WELCOME_STATIC_TITLE);
    if (pWndTitle != NULL)
    {        
        if (!m_fontTitle.CreateFont(0x12, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
                DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE, _T("Verdana")))
		{
			ASSERT(!"Cannot create font");
			return false;
		}

        pWndTitle->SetFont(&m_fontTitle, true);
    }

	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	if(!pApp)
		return -1;
	CConfigXMLData& data = pApp->GetConfigXMLData();
	m_strConfigFile = data.GetLoadedXMLConfigFilePath();

	UpdateData(FALSE);

	OnUseConfigFileButton();

    return b;
}

BOOL CWelcomePage::OnSetActive()
{
	BOOL b = WZRPage::OnSetActive();

//	theApp.m_bImportExportOnlyMode = false;
//	theApp.m_bConfigReportPackMode = false;

	if ( theApp.m_bReportPackMode )
	{
		GetDlgItem(IDC_OCRCFG_WELCOME_RADIO_DEFAULTXML)->EnableWindow( FALSE );
		GetDlgItem(IDC_OCRCFG_WELCOME_RADIO_SELECTSERVERS)->EnableWindow( FALSE );
		//GetDlgItem(IDC_OCRCFG_WELCOME_RADIO_SOFT_LIBRARY)->EnableWindow( FALSE );
		//GetDlgItem(IDC_OCRCFG_WELCOME_RADIO_DCOM)->EnableWindow( FALSE );
		m_iUseScenario = (int) CUseScenarioEnum::eusApplyConfig;
		UpdateData(FALSE);
		OnUseConfigFileButton();
	}

	//check is there are some thing in registry
	CMNRegKey	keyRoot;
	keyRoot.OpenEx(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot, KEY_READ);

	CAtlString strSQLServer			= keyRoot.QueryStringValue(cszSAMSqlServer, _T(""));
	CAtlString strReportsServerURL	= keyRoot.QueryStringValue(cszSAMReportsServerURL, _T(""));
	CAtlString strDatabase			= keyRoot.QueryStringValue(cszSAMDatabase, _T(""));
	CAtlString strReportsFolder		= keyRoot.QueryStringValue(cszSAMReportsFolder, _T(""));

	if(strSQLServer.IsEmpty() || strDatabase.IsEmpty() || theApp.m_bReportPackMode)
	{
//		GetDlgItem(IDC_OCRCFG_WELCOME_RADIO_SOFT_LIBRARY)->EnableWindow( FALSE );
	}
	else
	{
//		GetDlgItem(IDC_OCRCFG_WELCOME_RADIO_SOFT_LIBRARY)->EnableWindow( TRUE );
	}

	if(strSQLServer.IsEmpty() || strDatabase.IsEmpty() || strReportsServerURL.IsEmpty() || strReportsFolder.IsEmpty())
	{
		GetDlgItem(IDC_OCRCFG_WELCOME_RADIO_CONFIGXML)->EnableWindow( FALSE );
	}
	else
	{
		GetDlgItem(IDC_OCRCFG_WELCOME_RADIO_CONFIGXML)->EnableWindow( FALSE );
	}

	UpdateData(FALSE);

	if ( ! (theApp.m_bInstallerMode || theApp.m_bUnattendedMode) )
	{
		theApp.GetConfigXMLData().ClearConfig();
	}
	return b;
}

LRESULT CWelcomePage::OnWizardNext()
{
	UpdateData(TRUE);	

	if (theApp.m_enScenario == CUseScenarioEnum::eusRemove)
	{
		return IDD_OCRCFG_REGSVC_PAGE;
	}
	/*
	if ( eusExportImportSoftLibrary == m_iUseScenario )
	{
		theApp.m_bImportExportOnlyMode = true;
		return IDD_OCRCFG_SOFTLIBRARY_PAGE;
	}

	if ( eusConfigureDCOM == m_iUseScenario )
	{
		return IDD_OCRCFG_DCOM_PAGE;
	}
	*/

	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	if(!pApp)
		return -1;

	CConfigXMLData& data = pApp->GetConfigXMLData();
	CString strConfigFullFilePath = data.GetLoadedXMLConfigFilePath();
	if(strConfigFullFilePath.IsEmpty())
		strConfigFullFilePath = SamCfgGetFileFullPath(g_lpszDefaultConfigFileName);

	// Load data from reportpack if user have set reportpack file name 
	// report pack deploying in file DeployRptPage.cpp "BOOL CDeployRptPage::OnSetActive()" function
	if (theApp.m_enScenario == CUseScenarioEnum::eusApplyConfig)
	{
//		theApp.m_bConfigReportPackMode = true;

		for(std::set<CString>::iterator it = m_setConfigFile.begin(); it != m_setConfigFile.end(); ++it)
		{
			strConfigFullFilePath = *it;
			data.SetDisableClearConfig();
			try
			{
				data.LoadConfig(strConfigFullFilePath);
				if(!data.IsXMLConfigFileLoaded())
					return -1;
			}
			catch(CMNException* pEx)
			{
				CString strErrorMsg = pEx->GetErrorMessage();
				AfxMessageBox(strErrorMsg, MB_OK | MB_ICONEXCLAMATION);
				pEx->Delete();

				return -1;
			}
		}

	}
	else
	{
		try
		{
			data.LoadConfig(strConfigFullFilePath);
			if(!data.IsXMLConfigFileLoaded())
				return -1;
		}
		catch(CMNException* pEx)
		{
			CString strErrorMsg = pEx->GetErrorMessage();
			AfxMessageBox(strErrorMsg, MB_OK | MB_ICONEXCLAMATION);
			pEx->Delete();

			return -1;
		}
	}

	if (theApp.m_enScenario == CUseScenarioEnum::eusCreateNew)
		return IDD_OCRCFG_CREDENTIALS_PAGE;
	/*
	if (data.IsCreateServer() || data.IsRunSqlScripts())
		return IDD_OCRCFG_CREDENTIALS_PAGE;
	else if (data.IsDeployReports())
		return IDD_OCRCFG_DEPLOYRPT_PAGE;
	else if (data.IsImportSoftLibrary())
		return IDD_OCRCFG_SOFTLIBRARY_PAGE;
	*/
	return IDD_OCRCFG_FINISH_PAGE;
}

void CWelcomePage::CheckWizardNextButton()
{
	UpdateData(TRUE);

	//check is there are some thing in registry
	CMNRegKey	keyRoot;
	keyRoot.OpenEx(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot, KEY_READ);

	CAtlString strSQLServer			= keyRoot.QueryStringValue(cszSAMSqlServer, _T(""));
	CAtlString strReportsServerURL	= keyRoot.QueryStringValue(cszSAMReportsServerURL, _T(""));
	CAtlString strDatabase			= keyRoot.QueryStringValue(cszSAMDatabase, _T(""));
	CAtlString strReportsFolder		= keyRoot.QueryStringValue(cszSAMReportsFolder, _T(""));

	UINT uiWizButtons = 0;
	if (theApp.m_enScenario == CUseScenarioEnum::eusApplyConfig)
	{
		if(!m_strConfigFile.IsEmpty() && !strSQLServer.IsEmpty() && !strDatabase.IsEmpty() && !strReportsServerURL.IsEmpty() && !strReportsFolder.IsEmpty())
			uiWizButtons |= PSWIZB_NEXT;
	}
	else
		uiWizButtons |= PSWIZB_NEXT;

	CPropertySheet* pParent = static_cast<CPropertySheet*>(GetParent());
	if(pParent )
	{
		pParent->SetWizardButtons(uiWizButtons);
	}
}

void CWelcomePage::OnUseConfigFileButton()
{
	UpdateData(TRUE);

	if (theApp.m_enScenario == CUseScenarioEnum::eusApplyConfig)
	{
		m_btnBrowse.EnableWindow(TRUE);
		m_edtConfigFile.EnableWindow(TRUE);
		m_edtConfigFile.SetWindowText(m_strConfigFile);
	}
	else
	{
		m_btnBrowse.EnableWindow(FALSE);
		m_edtConfigFile.EnableWindow(FALSE);
		m_edtConfigFile.GetWindowText(m_strConfigFile);
		m_edtConfigFile.SetWindowText(_T(""));
	}

	CheckWizardNextButton();
}

void CWelcomePage::OnConfigPathChange()
{
	CheckWizardNextButton();
}

void CWelcomePage::OnBrowse()
{
	CString strFileExt, strFileFilter;
	strFileExt.LoadString(IDS_OCRCFG_XMLCONFIG_DEFEXT);
	strFileFilter.LoadString(IDS_OCRCFG_REPORTPACK_FILTER);

	CFileDialog dlg(TRUE, strFileExt, NULL,
		OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT | OFN_HIDEREADONLY, strFileFilter, AfxGetMainWnd(), 0, FALSE);

	const int iMaxFiles = 1000;
	dlg.GetOFN().nMaxFile = iMaxFiles;
	dlg.GetOFN().lpstrFile = new wchar_t[iMaxFiles * (_MAX_PATH + 1) + 1];
	dlg.GetOFN().lpstrFile[0] = 0;

	int itemp = lstrlen(dlg.GetOFN().lpstrFile);

	if(IDOK == dlg.DoModal())
	{
		m_setConfigFile.clear();
		POSITION pos = dlg.GetStartPosition();
		if(pos != NULL)
		{ // some files was choosed
			// change edit text
			m_strConfigFile = dlg.GetPathName();
			m_edtConfigFile.SetWindowText(m_strConfigFile);

			// fill in the set of report file names	
			while(pos != NULL)
			{
				m_setConfigFile.insert(dlg.GetNextPathName(pos));
			}
		}
	}

	delete[] dlg.GetOFN().lpstrFile;

	CheckWizardNextButton();
}
