//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Interface for thread for background operations
//--------------------------------------------------------------------------


#include "stdafx.h"

#include "cmn/include/Log.h"
#include "cmn/include/McException.h"
#include "cmn/include/RegKey.h"

#include "db/include/Constants.h"
#include "db/include/Storage.h"
#include "db/include/SqlBrowser.h"

#include "scm/include/SchemaDef.h"

#include "v/include/Constants.h"

#include "wzr\include\SelectServersDlg.h"
#include "wzr\include\ReadNetURLRptSrvThread.h"
#include "wzr\include\SQLSrvAddrParser.h"
#include "wzr/include/Thread.h"
#include "wzr/include/Page.h"
#include "wzr/include/Sheet.h"
#include "wzr/include/Constants.h"

#include "ocrcfg/include/misc.h"
#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/ConfigApp.h"
#include "ocrcfg/include/DeployRptThread.h"
#include "ocrcfg/include/DeployRptPage.h"
#include "ocrcfg/resource/resource.h"

// CDeployRptPage dialog

//IMPLEMENT_DYNAMIC(CDeployRptPage, WZRPage)

const TCHAR cszSearchItemName[]		= _T("<Поиск...>");
const TCHAR cszSeparator[] 			= _T(".");

// cnostants for search map
const TCHAR csz2005Version[] 		= _T("9");
const TCHAR csz2008Version[] 		= _T("10");
const TCHAR csz2005Name[] 			= _T("reportserver$SQLExpress");
const TCHAR csz2008Name[] 			= _T("reportserver_SQLExpress");

// names of instanses for unnamed servers
const TCHAR cszNoNameInstance1[] = _T("MSSQLSERVER");
const TCHAR cszNoNameInstance2[] = _T("default");

CDeployRptPage::CDeployRptPage( int* pnModificationFlags )
: 
WZRPage(IDD_OCRCFG_DEPLOYRPT_PAGE, IDS_OCRCFG_DEPLOYRPT_TITLE, IDS_OCRCFG_DEPLOYRPT_SUBTITLE),
m_strReportsServerURL(_T("")),
m_strReportsWebFolder(_T("")),
m_pnModificationFlags(pnModificationFlags)
{       
	m_hEventLock = CreateEvent(NULL, FALSE, TRUE, NULL); // no security, auto-reset, signaled, no-name
	bUnattendedMode = false; // attended mode by default

	if(!m_pnModificationFlags)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_POINTER_TO_INT_IS_NULL);
		*pEx << _T("CDeployRptPage::CDeployRptPage");
		ThrowException(pEx);
	}

	m_psp.dwFlags |= PSP_HASHELP;

}

CDeployRptPage::~CDeployRptPage()
{
	CloseHandle(m_hEventLock);
}

BEGIN_MESSAGE_MAP(CDeployRptPage, WZRPage)
    //{{AFX_MSG_MAP(CDeployRptPage)    
	ON_MESSAGE( PSM_QUERYSIBLINGS,			OnQuerySiblings		)
    ON_MESSAGE( WM_OCRCFG_DEPLOYRPT_START,	OnDeployRptStart	)
    ON_MESSAGE( WM_OCRCFG_DEPLOYRPT_STAGE,	OnDeployRptStage	)
    ON_MESSAGE( WM_OCRCFG_DEPLOYRPT_FINISH,	OnDeployRptFinish	)
	ON_CBN_SELENDOK	( IDC_OCRCFG_DEPLOYRPT_COMBO_REPORTSERVER, OnReportServerComboChange )
	//ON_BN_CLICKED( IDC_OCRCFG_DEPLOYRPT_COMBO_SQLSERVER, OnSQLServerRefresh )
	ON_MESSAGE(WM_WZR_READ_NETWORK_REPORT_SERVER_FINISH, OnReadNetworkReportServerFinish)
	ON_MESSAGE( WM_TIMER, OnTimer )
	ON_EN_CHANGE    ( IDC_OCRCFG_DEPLOYRPT_EDIT_FOLDER, OnNamesChange )
	ON_CBN_EDITCHANGE(IDC_OCRCFG_DEPLOYRPT_COMBO_REPORTSERVER, OnNamesChange )
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDeployRptPage::DoDataExchange(CDataExchange* pDX)
{
  	WZRPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDeployRptPage)
	DDX_Control(pDX, IDC_OCRCFG_DEPLOYRPT_COMBO_REPORTSERVER, m_comboReportsServerURL);
	DDX_Control(pDX, IDC_OCRCFG_DEPLOYRPT_EDIT_FOLDER, m_edtReportsWebFolder);

	//DDX_Control(pDX, IDC_OCRCFG_DEPLOYRPT_ICON, m_ctlIcon);
	//DDX_Control(pDX, IDC_OCRCFG_DEPLOYRPT_STATIC_STATUS_TEXT,		m_wndStatus);   

	//IDC_OCRCFG_DEPLOYRPT_ICON
    DDX_Text  (pDX, IDC_OCRCFG_DEPLOYRPT_COMBO_REPORTSERVER, m_strReportsServerURL);
    DDX_Text  (pDX, IDC_OCRCFG_DEPLOYRPT_EDIT_FOLDER,		m_strReportsWebFolder);   

	//DDX_Text  (pDX, IDC_OCRCFG_DEPLOYRPT_STATIC_STATUS_TEXT,		m_strStatus);   
    //}}AFX_DATA_MAP
}


BOOL CDeployRptPage::OnInitDialog()
{
    WZRPage::OnInitDialog();

	COLORREF clrSeeThrough = RGB(255,   0, 255);
	if(!oImageList.Create(16, 16, ILC_MASK | ILC_COLOR8))
	{
		CMNLog(_T("[CDeployRptPage::OnInitDialog]: Error create image list."));		
		return FALSE;
	}
	if(!oImageList.LoadToolbar(IDB_OCRCFG_TOOLBAR16x16, clrSeeThrough))
	{
		CMNLog(_T("[CDeployRptPage::OnInitDialog]: Error load toolbar bitmap."));		
		return FALSE;
	}

	return TRUE;
}


BOOL CDeployRptPage::OnSetActive()
{    
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if ( ! pApp ) return FALSE;

	// variable tp indicate was thread started
	bool bIsThreadStarted = false;

	CMNRegKey	keyRoot;
	try
	{
		keyRoot.OpenEx(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot, KEY_READ);
		//m_strSQLServer			= keyRoot.QueryStringValue(cszSAMSqlServer, cszDefSAMSqlServerValue);
		m_strDbName				= keyRoot.QueryStringValue(cszSAMDatabase,  cszDefSAMDatabaseValue);
		m_comboReportsServerURL.ResetContent();

		switch ( pApp->m_enScenario )
		{
			case CUseScenarioEnum::eusApplyConfig:
			{
				m_strReportsServerURL	= keyRoot.QueryStringValue(cszSAMReportsServerURL, _T(""));
				m_strReportsWebFolder	= keyRoot.QueryStringValue(cszSAMReportsFolder,  _T(""));

				//m_strSQLServerHostName = pApp->GetSQLServerHostName();
				m_strSQLServerHostName = keyRoot.QueryStringValue(cszSAMSqlServer, _T(""));

				m_comboReportsServerURL.EnableWindow(FALSE);
				m_edtReportsWebFolder.EnableWindow(FALSE);
				break;
			}

			default:
			//case eusCreateNew:
				// create names
				m_strSQLServerHostName = keyRoot.QueryStringValue(cszSAMSqlServer, _T(""));

				if(!m_strSQLServerHostName.IsEmpty())
				{
					// start thread for report server search ONLY if sql server name is not empty
					m_setReportServerName.clear();
					//CWinThread* pThread = new CReadNetworkURLReportServer(GetSafeHwnd(), 
					//	m_setReportServerName, m_strSQLServerUserEnterName, m_hEventLock);
					CWinThread* pThread = new CReadNetworkURLReportServer(GetSafeHwnd(), 
						m_setReportServerName, m_strSQLServerHostName, m_hEventLock);
					if (!pThread->CreateThread())
					{
						ASSERT(!"Cannot create thread");
					}

					bIsThreadStarted = true;

					// Show icon and string message
					m_strStatus.LoadString(IDS_OCRCFG_RPT_SEARCH_PROCESS); // = CAtlString(_T("Статус: ищутся экземпляры репорт серверов. Подождите..."));

					m_ctlIcon.SetIcon( oImageList.ExtractIcon(IDI_OCRCFG_INFORMATION) ); 
					m_ctlIcon.ShowWindow(TRUE);

					m_comboReportsServerURL.EnableWindow(FALSE);
					m_edtReportsWebFolder.EnableWindow(TRUE);
					m_wndStatus.ShowWindow(TRUE);	

					// set timer
					StartTimer();

					m_mapNamespaces.clear();
				}

				CWaitCursor waitcur;

				// lock combo box 
				CSingleLock singleLock(&m_Mutex);
				if(singleLock.Lock())
				{
					m_comboReportsServerURL.ResetContent();
					m_comboReportsServerURL.AddString( cszSearchItemName );
					// share combobox
					singleLock.Unlock();
				}

				m_strReportsWebFolder	= keyRoot.QueryStringValue(cszSAMReportsFolder,    cszDefSAMReportsFolderValue);

				break;
		}

		m_strReportsDir	= keyRoot.QueryStringValue(cszSAMReportsDir, _T(""));

		CConfigXMLData& data = pApp->GetConfigXMLData();
		if(data.IsDeployReports())
		{
			m_strXMLBody			= data.GetXMLBody();
			m_setXMLBody = data.GetXMLset();
			//if(m_strSQLServer.IsEmpty())
				//m_strSQLServer = data.GetSQLServer();
			if(m_strDbName.IsEmpty())
				m_strDbName = data.GetDBName();
			if(m_strReportsWebFolder.IsEmpty())
				m_strReportsWebFolder = data.GetWebFolder();

			m_strReportsDir2 = data.GetConfigFileDir();
			m_arrReportsInfo = data.GetReportsInfo();
		}

		
	}
	catch(CMNException* pEx)
	{	
		CString strAllErrors = CMNException::GetAllErrorMessages(pEx, _T("\n"));
		CMNLog(_T("[CDeployRptPage::OnSetActive]: %s"), strAllErrors);		
		pEx->Delete();
	}

    UpdateData(FALSE);

	// We can't skip reports deploy
	if ( m_btnSkipPage.m_hWnd )
	{
		m_btnSkipPage.ShowWindow( SW_HIDE );
	}

    if (pApp->m_bUnattendedMode)
	{
		// wait until search will not be complete
		//::PostMessage(m_pSheet->GetSafeHwnd(), PSM_PRESSBUTTON, PSBTN_NEXT, 0); // and simulate next button press
		bUnattendedMode = true;
	}

	BOOL bReturn = WZRPage::OnSetActive();

	// disable next button
	if(bIsThreadStarted)
	{
		m_pSheet->SetWizardButtons(0);
	}
	else
	{
		m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	}
	UpdateData(FALSE);

	//Нужно для принятия решения о перезапуске samsvc
	//Предыдущие значения берем из реестра, т.к. именно их использует samsvc 
	m_strPrevReportsServerURL	= keyRoot.QueryStringValue(cszSAMReportsServerURL, _T(""));
	m_strPrevReportsWebFolder	= keyRoot.QueryStringValue(cszSAMReportsFolder,    _T(""));

	return bReturn;
}

BOOL CDeployRptPage::OnKillActive()
{	
	CMNRegKey keyRoot;
	try
	{
		keyRoot.OpenEx(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot, KEY_WRITE);		
		CMNCheckWIN32Error( keyRoot.SetStringValue(cszSAMReportsServerURL, m_strReportsServerURL) );	
		CMNCheckWIN32Error( keyRoot.SetStringValue(cszSAMReportsFolder, m_strReportsWebFolder) );	
		//CMNCheckWIN32Error( keyRoot.SetStringValue(cszSAMReportsDir, m_strReportsDir) );	
	}
	catch(CMNException* pEx)
	{	
		CString strAllErrors = CMNException::GetAllErrorMessages(pEx, _T("\n"));
		CMNLog(_T("[CDeployRptPage::OnKillActive]: %s"), strAllErrors);		
		pEx->Delete();
	}
	return WZRPage::OnKillActive();
}

LRESULT CDeployRptPage::OnWizardBack()
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	if(!pApp)
		return -1;

	CConfigXMLData& data = pApp->GetConfigXMLData();
	if(data.IsCreateServer() || data.IsRunSqlScripts())
		return IDD_OCRCFG_CREATEDB_PAGE;
	else
		return IDD_OCRCFG_WELCOME_PAGE;

	return -1;
}

LRESULT CDeployRptPage::OnWizardNext()
{	
	if (m_iSkipPage)
		return WZRPage::OnWizardNext();

	if ( UpdateData(TRUE) == TRUE ) // correct input
	{
		if( m_strReportsServerURL.CompareNoCase( m_strPrevReportsServerURL ) != 0 )
			*m_pnModificationFlags |= IsModifiedReportServerName;

		if( m_strReportsWebFolder.CompareNoCase( m_strPrevReportsWebFolder ) != 0 )
			*m_pnModificationFlags |= IsModifiedReportsWebCatalog;

		DeployReports();
	}
	return -1; // stay at this page until install completes
}


// CDeployRptPage implementation

WZRThread* CDeployRptPage::ConstructThread() const
{
	CString strReportsDir, strReportsDir2;
	if ( theApp.m_enScenario == CUseScenarioEnum::eusApplyConfig )
	{
		strReportsDir = m_strReportsDir2;
		strReportsDir2 = m_strReportsDir.GetLength() ? m_strReportsDir : CString(SCMGetDefSAMReportsDirValue());
	}
	else
	{
		strReportsDir = m_strReportsDir.GetLength() ? m_strReportsDir : CString(SCMGetDefSAMReportsDirValue());
		strReportsDir2 = m_strReportsDir2;
	}
	CString strReportServerUrlUC = m_strReportsServerURL;
	strReportServerUrlUC.MakeUpper();
	CString strReportServerNamespace;
	std::map<CString, CString>::const_iterator it = m_mapNamespaces.find(strReportServerUrlUC);
	if ( it != m_mapNamespaces.end() ) strReportServerNamespace = it->second;
    return new CDeployRptThread(GetSafeHwnd(), *m_setXMLBody, m_strSQLServerHostName, m_strDbName, 
		m_strReportsServerURL, strReportServerNamespace, m_strReportsWebFolder, 
		strReportsDir, strReportsDir2, m_arrReportsInfo, m_pnModificationFlags );
}

LRESULT CDeployRptPage::DeployReports()
{    
	//DBStorage::FillScriptIDVector();
	m_ctlProgress.SetRange( (short)0, (short)7 );
    m_ctlProgress.SetStep(1);
	m_ctlProgress.SetPos(0);
	m_strStage.FormatMessage(IDS_OCRCFG_RPT_WAIT);
    //m_nCurrTmplNumber = 0;
    OnThreadStarted();
    return 0;
}

// checks for two templates and one SVE to be installed and fills out the edit control with missed components names

bool CDeployRptPage::CheckInstalledComponents(bool& bAllComponentsInstalled)
{
    // check for installed components

	bAllComponentsInstalled = false;

    return false;
}

// CDeployRptPage message handlers

LRESULT CDeployRptPage::OnQuerySiblings(WPARAM wParam, LPARAM lParam)
{
    if(wParam == WM_OCRCFG_QUERY_DEPLOYRPT)
    {
        ::memcpy((void*) lParam, (void*) m_strThreadResults.GetString(), m_strThreadResults.GetLength() * sizeof(TCHAR));
		return 1; // answer
    }
    return 0; // ignore
}

LRESULT CDeployRptPage::OnDeployRptStart (WPARAM wParam, LPARAM lParam)
{
    DWORD   dwMaxPos = (DWORD)lParam;
    m_ctlProgress.SetRange( (short)0, (short)dwMaxPos );
	m_strStage.FormatMessage(IDS_OCRCFG_RPT_DEPLOY_PROCESS, m_strReportsWebFolder);

	// determine is width of string correct
	TEXTMETRIC tm;
	if(this->GetDC()->GetOutputTextMetrics(&tm))
	{
		RECT rectl;
		m_wndStage.GetWindowRect(&rectl);
		int iWinWidth = rectl.right - rectl.left + (tm.tmAveCharWidth * 7);
		int iLength = m_strStage.GetLength();
		int temp = iLength * tm.tmAveCharWidth;
		if((temp) > iWinWidth)
		{
			iWinWidth /= tm.tmAveCharWidth;
			m_strStage = m_strStage.Mid(0, iWinWidth) + _T("...");
		}
	}

	UpdateData(FALSE);
	return 0;
}

LRESULT CDeployRptPage::OnDeployRptStage (WPARAM wParam, LPARAM lParam)
{
	CString strFormat;
	DWORD   dwNewPos   = (DWORD)wParam;
//	TRACE(_T("[CCreateRptPage::OnDeployRptStage] %i\n"), dwPercent );
    DWORD dwPos = (DWORD)m_ctlProgress.GetPos();
    if (dwPos < dwNewPos)
    {
        m_ctlProgress.StepIt();
		ShowProgress();
    }
    return 0;
}

LRESULT CDeployRptPage::OnDeployRptFinish (WPARAM wParam, LPARAM lParam)
{
    BOOL bError = (BOOL)wParam;
	CString strError = m_pThread->GetErrorString();    
    OnThreadFinished();

	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	if (!pApp)
		return 0;

	CConfigXMLData& data = pApp->GetConfigXMLData();
	//data.DeleteReportPackArchiveFolder();

    if (bError)
	{
		if (theApp.m_bUnattendedMode)
		{
			::PostMessage(m_pSheet->GetSafeHwnd(), PSM_PRESSBUTTON, PSBTN_FINISH, 0);
		}
        else
        {
			TException tex(IDE_OCRCFG_ERROR_STRING);
			tex << strError;
			tex.ReportError();

			m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
        }
	}
	else
	{
        int iActivePage = m_pSheet->GetActiveIndex();
		if (data.IsImportSoftLibrary())
			m_pSheet->SetActivePage(iActivePage + 1);
		else
			m_pSheet->SetActivePage(iActivePage + 2);
	}
    return 0;
}


void CDeployRptPage::OnReportServerComboChange()
{
	UpdateData();

	int iLasVal = 0;
	iLasVal = m_comboReportsServerURL.GetCurSel() - 1;

	// check is choosen string is cszSearchItemName
	CString strBuf;
	m_comboReportsServerURL.GetLBText(m_comboReportsServerURL.GetCurSel(), strBuf);
	if(strBuf == CString(cszSearchItemName))
	{
		// open dialog
		CSelectServerDlg SelectServerDlg(SSDReportServer, m_strSQLServerHostName, 
			m_strSQLServerHostName);
		INT_PTR nRet = -1;
		nRet = SelectServerDlg.DoModal();

		switch ( nRet )
		{
			case -1: 
			{
				TException tex(IDE_OCRCFG_ERROR_CREATE_SEARCH_DIALOG);
				tex.ReportError();
				break;
			}
			//case IDABORT:
			//{
			//	// Do something
			//	break;
			//}
			case IDOK:
			{
				// transmit data to parent dialog
				// Get currenl list 
				set<CAtlString> setComboboxServerNames;
				for (int i = 0; i < m_comboReportsServerURL.GetCount(); i++)
				{
					CString strTemp;
					m_comboReportsServerURL.GetLBText(i, strTemp);
					if(strTemp == cszSearchItemName)
					{
						continue;
					}
					setComboboxServerNames.insert(CAtlString(strTemp));
				}
				// create list with unique items
				CString strTemp = SelectServerDlg.GetChoosenText();
				set<CAtlString,InsensitiveCompare>lstServers; //нечувствительный к регистру набор
				lstServers.insert(setComboboxServerNames.begin(), setComboboxServerNames.end());
				lstServers.insert(strTemp);
				// clear combobox
				m_comboReportsServerURL.ResetContent();
				// fill in combobox with new items
				for (set<CAtlString,InsensitiveCompare>::iterator it=lstServers.begin(); it!=lstServers.end(); ++it)
				{
					CString strServer = *it;
					m_comboReportsServerURL.AddString(strServer);
				}
				m_comboReportsServerURL.AddString(cszSearchItemName);

				// set cur sel element which was returned from dialog
				iLasVal = m_comboReportsServerURL.FindStringExact(0, strTemp);
				if(CB_ERR != iLasVal)
				{
					m_comboReportsServerURL.SetCurSel(iLasVal);
					m_comboReportsServerURL.GetLBText(m_comboReportsServerURL.GetCurSel(), strBuf);
				}

				break;
			}
			//case IDCANCEL:
			//{
			//	// Do something
			//	break;
			//}
			default:
			{
				m_comboReportsServerURL.SetCurSel(iLasVal);
				m_comboReportsServerURL.GetLBText(m_comboReportsServerURL.GetCurSel(), strBuf);
				break;
			}
		};
	}

	m_strReportsServerURL = strBuf;

	UpdateData(FALSE);

	SetButtons();

}

LRESULT CDeployRptPage::OnReadNetworkReportServerFinish(WPARAM wParam, LPARAM lParam)
{
	CMNLog(_T("[CDeployRptPage::OnReadNetworkReportServerFinish]: start functions for treat report service search finish"));
	// indicator: something was found.
	bool bWasFound = m_setReportServerName.size() != 0; 
	CMNLog(_T("[CDeployRptPage::OnReadNetworkReportServerFinish]: size of recived set is %i"), 
		m_setReportServerName.size());

	// stop timer
	StopTimer();
	m_comboReportsServerURL.EnableWindow(TRUE);

	// parce SQL server name
	CAtlString strInstanceName, strSQLHostName;
	int iPos = m_strSQLServerHostName.Find(_T("\\"));
	if( -1 != iPos)
	{
		strInstanceName = m_strSQLServerHostName.Mid(iPos + 1);
		strSQLHostName = m_strSQLServerHostName.Mid(0, iPos);

		// check strInstanceName for the case using IP address with port
		// the sql server name can be like this 10.4.0.55\SQLSERVERNAME,1444
		iPos = strInstanceName.Find(_T(","));
		if( -1 != iPos)
		{
			strInstanceName = strInstanceName.Mid(0, iPos);
		}
	}
	else
	{
		strSQLHostName = m_strSQLServerHostName;
	}

	set<CAtlString,InsensitiveCompare> lstServers; //нечувствительный к регистру набор
	if(bWasFound)
	{
		lstServers.insert(m_setReportServerName.begin(), m_setReportServerName.end());
        VERIFY( m_strStatus.LoadString(IDS_OCRCFG_RPT_SEARCH_COMPLETED) ); // = _T("Статус: поиск завершен.");
		m_ctlIcon.ShowWindow(FALSE);
	}
	else
	{
		CMNLog(_T("[CDeployRptPage::OnReadNetworkReportServerFinish]: start create default URL"));
		// add default name if nothing was found
		CAtlString strVersion, strDefaultName, strHostName;
		std::map<CAtlString, CAtlString> mapResult;
		std::map<CAtlString, CAtlString>::iterator it;
		CSqlAndReportingServersInfo serverInfo;

		if(!m_strSQLServerHostName.IsEmpty())
		{
			int iPos = m_strSQLServerHostName.Find(_T("\\"));
			if(-1 != iPos)
			{
				strHostName = m_strSQLServerHostName.Mid(0, iPos);
			}
			else
			{
				// case of unnamed server
				strHostName = m_strSQLServerHostName;
				strInstanceName = cszNoNameInstance1;
			}
		}
		else
		{
			CMNLog(_T("[CDeployRptPage::OnReadNetworkReportServerFinish]: Error sql server host name is empty"));
		}

		VERIFY( m_strStatus.LoadString(IDS_OCRCFG_RPT_SEARCH_FAILED_WAIT) );
		m_ctlIcon.SetIcon( oImageList.ExtractIcon(IDI_OCRCFG_WARNING) );
		UpdateData(FALSE);

		BeginWaitCursor();
		CMNLog(_T("[CDeployRptPage::OnReadNetworkReportServerFinish]: read sql instances on \"%s\""), strHostName);
		if(serverInfo.ReadSqlInformationByHostName(mapResult, strHostName))// accept only host name
		{	
			EndWaitCursor();
			CMNLog(_T("[CDeployRptPage::OnReadNetworkReportServerFinish]: recived map size is %i"), mapResult.size());
			it = mapResult.find(strInstanceName);
			if(it != mapResult.end())
			{
				std::map<CAtlString, CAtlString> mapCompare;
				mapCompare.insert(std::pair<CAtlString, CAtlString>(csz2005Version, csz2005Name));
				mapCompare.insert(std::pair<CAtlString, CAtlString>(csz2008Version, csz2008Name));

				int iPos = it->second.Find(cszSeparator);
				strVersion = it->second.Mid(0, iPos);

				it = mapCompare.find(strVersion);
				if(it != mapCompare.end())
				{
					strDefaultName = mapCompare[strVersion];
				}
			}
		}
		else
		{
			EndWaitCursor();

			CMNLog(_T("[CDeployRptPage::OnReadNetworkReportServerFinish]: Error get information about %s server"), 
				m_strSQLServerHostName);		
		}
		// get sql server version and assign default name
		CAtlString strDefaultURL = CAtlString("http://") + strSQLHostName + CAtlString("/") + strDefaultName;
		lstServers.insert(strDefaultURL);

		CMNLog(_T("[CDeployRptPage::OnReadNetworkReportServerFinish]: default URL is %s"), strDefaultURL);

        VERIFY( m_strStatus.LoadString(IDS_OCRCFG_RPT_SEARCH_FAILED) ); // = _T("Статус: ни одного экземпляра было найденно, введите название вручную.");
	}
	//UpdateData(FALSE);
	// unlock shared resource
	SetEvent(m_hEventLock);

	// fill in combo box
	// Get currenl list 
	// lock combobox
	CSingleLock singleLock(&m_Mutex);
	if(singleLock.Lock())
	{
		for (int i = 0; i < m_comboReportsServerURL.GetCount(); i++)
		{
			CString strTemp;
			m_comboReportsServerURL.GetLBText(i, strTemp);
			if(strTemp == cszSearchItemName)
			{
				continue;
			}
			lstServers.insert(CAtlString(strTemp));
		}

		// clear combobox
		m_comboReportsServerURL.ResetContent();
		// fill in combobox with new items
		for (set<CAtlString,InsensitiveCompare>::iterator it=lstServers.begin(); it!=lstServers.end(); ++it)
		{
			CString strServer = *it;
			m_comboReportsServerURL.AddString(strServer);
		}
		// combo box is NOT empty here, if nothing was found it contains default names
		m_comboReportsServerURL.AddString(cszSearchItemName);
		m_comboReportsServerURL.SetCurSel(0);
		m_comboReportsServerURL.GetLBText(0, m_strReportsServerURL);
		// share combobox
		singleLock.Unlock();
	}

	if(bUnattendedMode)
	{
		if(!bWasFound)
		{ // if no one instance of report server was found then we should write log
			CMNLog(_T("[CDeployRptPage::OnReadNetworkReportServerFinish]: %s"), 
				_T("instance of report server was not found"));		
		}

		::PostMessage(m_pSheet->GetSafeHwnd(), PSM_PRESSBUTTON, PSBTN_NEXT, 0); // and simulate next button press

		return 0;
	}

	// enable back\next buttons
	m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	UpdateData(FALSE);

	return 0;
}

LRESULT CDeployRptPage::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if(m_wndStatus.IsWindowVisible())
	{
		m_wndStatus.ShowWindow(FALSE);
	}
	else
	{
		m_wndStatus.ShowWindow(TRUE);	
	}

	return 0;
}

void CDeployRptPage::StartTimer()
{
	m_nCallbackTimer = SetTimer(1, 500, NULL);

}

void CDeployRptPage::StopTimer()
{
	KillTimer(m_nCallbackTimer); 
	m_wndStatus.ShowWindow(TRUE);	
}


void CDeployRptPage::OnNamesChange()
{
	SetButtons();
}


void CDeployRptPage::SetButtons()
{
	if ( ! theApp.m_bUnattendedMode)
	{
		UpdateData();

		// check is sql server name and DB name fields is empty
		if(m_strReportsServerURL.IsEmpty() || m_strReportsWebFolder.IsEmpty())
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
