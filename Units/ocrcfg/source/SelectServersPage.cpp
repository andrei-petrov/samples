#include "StdAfx.h"

#include "cmn/include/Log.h"
#include "cmn/include/McException.h"
#include "cmn/include/RegKey.h"
#include "cmn/include/Misc.h"

#include "scm/include/SchemaDef.h"

#include "db/include/Storage.h"
//#include "db/include/Constants.h"
#include "db/include/SqlBrowser.h"

#include "wzr\include\ReadNetworkSQLServerThread.h"
#include "wzr\include\ReadNetURLRptSrvThread.h"
#include "wzr\include\SelectServersDlg.h"
#include "wzr\include\ReadSQLSrvDataBaseThread.h"
#include "wzr\include\Constants.h"
#include "wzr\include\Sheet.h"


#include "..\include\SelectServersPage.h"
#include "..\include\Constants.h"
#include "..\include\ConfigApp.h"
#include "..\include\misc.h"
#include "..\include\ReadWebFolderThread.h"
#include "..\resource\resource.h"


const TCHAR cszSearchItemName[] = _T("<Поиск...>");


CSelectServersPage::CSelectServersPage( int* pnModificationFlags  )
	: WZRPage(IDD_OCRCFG_SELECTSERVERS_PAGE, IDS_OCRCFG_SELECTSERVERS_TITLE, IDS_OCRCFG_SELECTSERVERS_SUBTITLE), 
	//__SelSrvPageConditionEnum(SelSrvPageNone)
	m_enSelSrvPageDatabase(SelSrvPageNone),
	m_enSelSrvPageReportServer(SelSrvPageNone),
	m_enSelSrvPageWebFolder(SelSrvPageNone),
	m_pnModificationFlags(pnModificationFlags)
{
	m_bSaveRegistrySettings = false;
	m_hEventLock 			= CreateEvent(NULL, FALSE, TRUE, NULL); // no security, auto-reset, signaled, no-name
	m_hSQLSrvDBEventLock 	= CreateEvent(NULL, FALSE, TRUE, NULL); // no security, auto-reset, signaled, no-name
	m_hWebFolderventLock 	= CreateEvent(NULL, FALSE, TRUE, NULL); // no security, auto-reset, signaled, no-name

	if(!m_pnModificationFlags)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_POINTER_TO_INT_IS_NULL);
		*pEx << _T("CSelectServersPage::CSelectServersPage");
		ThrowException(pEx);
	}

	m_psp.dwFlags |= PSP_HASHELP;
}


CSelectServersPage::~CSelectServersPage(void)
{
	CloseHandle(m_hEventLock);
	CloseHandle(m_hSQLSrvDBEventLock);
	CloseHandle(m_hWebFolderventLock);
}


BEGIN_MESSAGE_MAP(CSelectServersPage, WZRPage)
	ON_MESSAGE(PSM_QUERYSIBLINGS, OnQuerySiblings)
	//ON_CBN_EDITCHANGE(IDC_OCRCFG_SELECTSERVERS_COMBO_SQLSERVER, OnSelectSQLServerComboChange)
	//ON_CBN_SELCHANGE(IDC_OCRCFG_SELECTSERVERS_COMBO_SQLSERVER, OnSelectSQLServerComboChange)
	ON_CBN_SELENDOK(IDC_OCRCFG_SELECTSERVERS_COMBO_SQLSERVER, OnSelectSQLServerComboChange)
	//ON_CBN_EDITCHANGE(IDC_OCRCFG_SELECTSERVERS_COMBO_REPORTSERVER, OnReportsServerComboChange)
	//ON_CBN_SELCHANGE(IDC_OCRCFG_SELECTSERVERS_COMBO_REPORTSERVER, OnReportsServerComboChange)
	ON_CBN_SELENDOK(IDC_OCRCFG_SELECTSERVERS_COMBO_REPORTSERVER, OnReportsServerComboChange)
	ON_MESSAGE( WM_TIMER, OnTimer )
	ON_MESSAGE(WM_WZR_READ_NETWORK_REPORT_SERVER_FINISH, OnReadNetworkReportServerFinish)
	ON_MESSAGE(WM_WZR_READ_SQLSRV_DATABASE_FINISH, OnReadSQLSrvDataBaseFinish)
	ON_MESSAGE(WM_OCRCFG_READ_WEB_FOLDER_FINISH, OnReadWebFolderFinish)
	ON_CBN_EDITCHANGE(IDC_OCRCFG_SELECTSERVERS_COMBO_SQLSERVER, OnSQLServerEditChange)
	ON_CBN_EDITCHANGE(IDC_OCRCFG_SELECTSERVERS_COMBO_REPORTSERVER, OnURLReportServerEditChange)

	ON_CBN_EDITCHANGE(IDC_OCRCFG_SELECTSERVERS_COMBO_DATABASE, OnNamesEdit)
	ON_CBN_EDITCHANGE(IDC_OCRCFG_SELECTSERVERS_COMBO_FOLDER, OnNamesEdit)
	ON_CBN_SELENDOK(IDC_OCRCFG_SELECTSERVERS_COMBO_DATABASE, OnNamesChange)
	ON_CBN_SELENDOK(IDC_OCRCFG_SELECTSERVERS_COMBO_FOLDER, OnNamesChange)
END_MESSAGE_MAP()


void CSelectServersPage::DoDataExchange(CDataExchange* pDX)
{
	WZRPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_OCRCFG_SELECTSERVERS_COMBO_SQLSERVER, 	  m_strSQLServer);
	DDX_Text(pDX, IDC_OCRCFG_SELECTSERVERS_COMBO_REPORTSERVER, 	  m_strReportsServerURL);
	DDX_Text(pDX, IDC_OCRCFG_SELECTSERVERS_COMBO_DATABASE, 		  m_strDatabase);
	DDX_Text(pDX, IDC_OCRCFG_SELECTSERVERS_COMBO_FOLDER, 		  m_strReportsFolder);
	DDX_Control(pDX, IDC_OCRCFG_SELECTSERVERS_COMBO_SQLSERVER, 	  m_comboSQLServer);    
	DDX_Control(pDX, IDC_OCRCFG_SELECTSERVERS_COMBO_DATABASE, 	  m_comboDatabase);    
	DDX_Control(pDX, IDC_OCRCFG_SELECTSERVERS_COMBO_REPORTSERVER, m_comboReportsServerURL);    
	DDX_Control(pDX, IDC_OCRCFG_SELECTSERVERS_COMBO_FOLDER, 	  m_comboReportsFolder); 
//	DDX_Control(pDX, IDC_OCRCFG_SELECTSERVERS_STATUS_TEXT,		  m_wndStatusText);   
}


BOOL CSelectServersPage::OnSetActive()
{    
	m_bFirstLanchl = true;
	m_bSaveRegistrySettings = false;
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	CMNRegKey	keyRoot;
	try
	{
		keyRoot.OpenEx(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot, KEY_READ);

		m_comboSQLServer.ResetContent();
		m_comboDatabase.ResetContent();
		m_comboReportsServerURL.ResetContent();
		m_comboReportsFolder.ResetContent();

		m_strSQLServer			= keyRoot.QueryStringValue(cszSAMSqlServer, _T(""));
		//m_strReportsServerURL	= keyRoot.QueryStringValue(cszSAMReportsServerURL, _T(""));
		//m_strDatabase			= keyRoot.QueryStringValue(cszSAMDatabase, _T(""));
		//m_strReportsFolder		= keyRoot.QueryStringValue(cszSAMReportsFolder, _T(""));

		//Нужно для принятия решения о перезапуске samsvc
		//Предыдущие значения берем из реестра, т.к. именно их использует samsvc 
		m_strPrevSQLServer 		  = keyRoot.QueryStringValue(cszSAMSqlServer, _T(""));
		m_strPrevReportsServerURL = keyRoot.QueryStringValue(cszSAMReportsServerURL, _T(""));
		m_strPrevDatabase 		  = keyRoot.QueryStringValue(cszSAMDatabase, _T(""));
		m_strPrevReportsFolder 	  = keyRoot.QueryStringValue(cszSAMReportsFolder, _T(""));

		m_strStatus = _T("");

		//if ( ! m_strDatabase.IsEmpty() )
		//{
		//	m_comboDatabase.AddString( m_strDatabase );
		//	m_comboDatabase.SetCurSel( 0 );
		//}

		//if ( ! m_strReportsFolder.IsEmpty() )
		//{
		//	m_comboReportsFolder.AddString( m_strReportsFolder );
		//	m_comboReportsFolder.SetCurSel( 0 );
		//}

		if ( pApp )
		{
			// list local sql servers
			CSqlAndReportingServersInfo serverInfo;
			set<CAtlString> localServerNames;
			serverInfo.ReadLocalSqlServerNames( localServerNames );
			//Заполнить comboBox для выбора серверов ____________________
			CSingleLock singleLock(&m_MutexSQLServerCombo);
			if(singleLock.Lock())
			{
				m_comboSQLServer.ResetContent();
				for (set<CAtlString>::iterator it=localServerNames.begin(); it!=localServerNames.end(); ++it)
				{
					CString strServer = *it;
					m_comboSQLServer.AddString(strServer);
				}			
				m_comboSQLServer.AddString(cszSearchItemName);

				singleLock.Unlock();
			}
		}
		
		m_comboReportsServerURL.AddString(cszSearchItemName);
		//UpdateData(FALSE);

		// StartSearch();

	}
	catch(CMNException* pEx)
	{	
		CString strAllErrors = CMNException::GetAllErrorMessages(pEx, _T("\n"));
		CMNLog(_T("[CSelectServersPage::OnSetActive]: %s"), strAllErrors);		
		pEx->Delete();
	}

	if ( m_btnSkipPage.m_hWnd )
	{
		m_btnSkipPage.ShowWindow( SW_HIDE );
	}

	m_wndStatus.ShowWindow(FALSE);	

	//return WZRPage::OnSetActive();
	BOOL bReturn = WZRPage::OnSetActive();

	// disable next button
	m_pSheet->SetWizardButtons(0);
	UpdateData(FALSE);

	// start threads for search 
	StartSearch();

	return bReturn;
}


BOOL CSelectServersPage::OnKillActive()
{
	if ( m_bSaveRegistrySettings )
	{
		CMNRegKey keyRoot;
		try
		{
			keyRoot.OpenEx(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot, KEY_WRITE);


			CMNCheckWIN32Error( keyRoot.SetStringValue(cszSAMSqlServer, m_strSQLServer) );
			CMNCheckWIN32Error( keyRoot.SetStringValue(cszSAMDatabase, m_strDatabase) );
			CMNCheckWIN32Error( keyRoot.SetStringValue(cszSAMReportsServerURL, m_strReportsServerURL) );
			CMNCheckWIN32Error( keyRoot.SetStringValue(cszSAMReportsFolder, m_strReportsFolder) );

			CString strDatabaseResults;
			strDatabaseResults.FormatMessage(IDS_OCRCFG_DB_USE, m_strDatabase, m_strSQLServer);

			CString strReportResults;
			strReportResults.Format(IDS_OCRCFG_RPT_USE, m_strReportsFolder, m_strReportsServerURL);

			m_strThreadResults = strDatabaseResults + _T("\r\n\r\n") + strReportResults;
			CMNLog( _T("[CSelectServersPage::OnKillActive]: %s"), m_strThreadResults );
		}
		catch(TException* pTEx)
		{	
			CString strAllErrors = TException::GetAllErrorMessages(pTEx, _T("\n"));
			CMNLog(_T("[CSelectServersPage::OnKillActive]: %s"), strAllErrors);		
			if ( ! theApp.m_bUnattendedMode )
			{
				pTEx->ReportError();
			}
			pTEx->Delete();
		}

	}
	return WZRPage::OnKillActive();
}


LRESULT CSelectServersPage::OnWizardBack()
{
	return IDD_OCRCFG_WELCOME_PAGE;
}


LRESULT CSelectServersPage::OnWizardNext()
{
	UpdateData(TRUE);

	if ( m_strDatabase.GetLength() == 0 )
	{
		CString strError;
		strError.LoadString(IDE_EMPTY_DATABASE_NAME);
		CMNLog(_T("[CSelectServersPage::OnWizardNext]: %s"), strError);
		if ( ! theApp.m_bUnattendedMode )
		{
			MessageBox(strError, 0, MB_OK | MB_ICONERROR);
			return -1;
		}
	}

	if ( m_strReportsFolder.GetLength() == 0 )
	{
		CString strError;
		strError.LoadString(IDE_EMPTY_REPORTS_FOLDER_NAME);
		CMNLog(_T("[CSelectServersPage::OnWizardNext]: %s"), strError);
		if ( ! theApp.m_bUnattendedMode )
		{
			MessageBox(strError, 0, MB_OK | MB_ICONERROR);
			return -1;
		}
	}

	if( m_strSQLServer.CompareNoCase( m_strPrevSQLServer ) != 0 )
		*m_pnModificationFlags |= IsModifiedSqlServerName;

	if( m_strDatabase.CompareNoCase(m_strPrevDatabase) != 0 )
		*m_pnModificationFlags |= IsModifiedDBName;

	if( m_strReportsServerURL.CompareNoCase(m_strPrevReportsServerURL) != 0 )
		*m_pnModificationFlags |= IsModifiedReportServerName;

	if( m_strReportsFolder.CompareNoCase(m_strPrevReportsFolder) != 0 )
		*m_pnModificationFlags |= IsModifiedReportsWebCatalog;

	m_bSaveRegistrySettings = true;
	return IDD_OCRCFG_FINISH_PAGE;
}


LRESULT CSelectServersPage::OnQuerySiblings(WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_OCRCFG_QUERY_SELECTSERVERS)
	{
        ::memcpy((void*) lParam, (void*) m_strThreadResults.GetString(), m_strThreadResults.GetLength() * sizeof(TCHAR));
		return 1; // answer
	}
    return 0; // ignore
}


void CSelectServersPage::OnSelectSQLServerComboChange()
{
	UpdateData();

	int iLasVal = 0;
	iLasVal = m_comboSQLServer.GetCurSel() - 1;

	// check is choosen string is cszSearchItemName
	CString strBuf;
	m_comboSQLServer.GetLBText(m_comboSQLServer.GetCurSel(), strBuf);
	if(strBuf == CString(cszSearchItemName))
	{
		// open dialog
		CSelectServerDlg SelectServerDlg;
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
				for (int i = 0; i < m_comboSQLServer.GetCount(); i++)
				{
					CString strTemp;
					m_comboSQLServer.GetLBText(i, strTemp);
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
				m_comboSQLServer.ResetContent();
				// fill in combobox with new items
				for (set<CAtlString,InsensitiveCompare>::iterator it=lstServers.begin(); it!=lstServers.end(); ++it)
				{
					CString strServer = *it;
					m_comboSQLServer.AddString(strServer);
				}
				m_comboSQLServer.AddString(cszSearchItemName);

				// set cur sel element which was returned from dialog
				iLasVal = m_comboSQLServer.FindStringExact(0, strTemp);
				if(CB_ERR != iLasVal)
				{
					m_comboSQLServer.SetCurSel(iLasVal);
				}
				// update text
				m_comboSQLServer.GetLBText(iLasVal, m_strSQLServer);

				// start search for choosen sql server
				StartSearch();

				break;
			}
			//case IDCANCEL:
			//{
			//	// Do something
			//	break;
			//}
			default:
			{
				m_comboSQLServer.SetCurSel(iLasVal);
				// update text
				m_comboSQLServer.GetLBText(iLasVal, m_strSQLServer);
				break;
			}
		};
	}
	else
	{	
		// start search for choosen sql server
		m_strSQLServer = strBuf;
		StartSearch();
	}

	UpdateData(FALSE);

	// not need because we call StartSearch, it controls buttons
	//SetButtons();
}


void CSelectServersPage::OnReportsServerComboChange()
{
	UpdateData();

	int iLasVal = 0;
	iLasVal = m_comboReportsServerURL.GetCurSel() - 1;

	try
	{
		// check is choosen string is cszSearchItemName
		CString strBuf;
		m_comboReportsServerURL.GetLBText(m_comboReportsServerURL.GetCurSel(), strBuf);
		if(strBuf == CString(cszSearchItemName))
		{
			// get selectes line from m_comboSQLServer
			CString strBufServerName;
			m_comboSQLServer.GetWindowTextW(strBufServerName);

			// open dialog
			CSelectServerDlg SelectServerDlg(SSDReportServer, strBufServerName, strBufServerName);
			INT_PTR nRet = -1;
			nRet = SelectServerDlg.DoModal();

			switch ( nRet )
			{
				case -1: 
				{
					TException tex(IDE_OCRCFG_ERROR_CREATE_RPT_SEARCH_DIALOG);
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
					}

					// start thread for web folder
					m_setWebFolder.clear();
					CWinThread* pThread = new CReadWebFolder(GetSafeHwnd(),
						m_setWebFolder, CAtlString(strTemp), m_hWebFolderventLock);
					if (!pThread->CreateThread())
					{
						ASSERT(!"Cannot create thread");
					}
					SetStatus(SelSrvPageWebFolderSearchStart);

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
					break;
				}
			};

		
		}
		else
		{	
			//m_comboReportsFolder.ResetContent();

			// start thread for web folder
			m_setWebFolder.clear();
			CWinThread* pThread = new CReadWebFolder(GetSafeHwnd(),
				m_setWebFolder, CAtlString(strBuf), m_hWebFolderventLock);
			if (!pThread->CreateThread())
			{
				ASSERT(!"Cannot create thread");
			}
			SetStatus(SelSrvPageWebFolderSearchStart);

		}
	}
	catch(TException* pTEx)
	{
		CAtlString strErrMessage = TException::GetAllErrorMessages(pTEx);
		CMNLog(_T("[CSelectServersPage::OnReportsServerComboChange]: %s"), strErrMessage);
		pTEx->ReportError();
		pTEx->Delete();

		m_comboReportsServerURL.SetCurSel(iLasVal);
	}

	UpdateData(FALSE);
}


void CSelectServersPage::OnSQLServerEditChange()
{
	UpdateData();

	//m_strSQLServer;
	m_strReportsServerURL = _T("");
	m_strDatabase = _T("");
	m_strReportsFolder = _T("");

	//m_comboSQLServer;
	m_comboDatabase.ResetContent();
	m_comboReportsServerURL.ResetContent();
	m_comboReportsFolder.ResetContent();

	if(!m_strSQLServer.IsEmpty())
	{
		m_comboReportsServerURL.AddString(cszSearchItemName);
	}

	UpdateData(FALSE);

	SetButtons();
}


void CSelectServersPage::OnURLReportServerEditChange()
{
	UpdateData();

	m_strReportsFolder = _T("");
	m_comboReportsFolder.ResetContent();

	UpdateData(FALSE);

	SetButtons();
}


LRESULT CSelectServersPage::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if(m_wndStatus.IsWindowVisible())
	{
		m_wndStatus.ShowWindow(FALSE);
	}
	else
	{
		m_wndStatus.ShowWindow(TRUE);	
	}

	UpdateData(FALSE);

	return 0;
}


void CSelectServersPage::StartTimer()
{
	m_nCallbackTimer = SetTimer(1, 500, NULL);

}


void CSelectServersPage::StopTimer()
{
	KillTimer(m_nCallbackTimer); 
	m_wndStatus.ShowWindow(TRUE);	
	UpdateData(FALSE);
}


LRESULT CSelectServersPage::OnReadNetworkReportServerFinish(WPARAM wParam, LPARAM lParam)
{
	CString strFirsServer;

	// parce SQL server name
	CAtlString strInstanceName, strSQLHostName;
	int iPos = m_strSqlServerName.Find(_T("\\"));
	if( -1 != iPos)
	{
		strInstanceName = m_strSqlServerName.Mid(iPos + 1);
		strSQLHostName = m_strSqlServerName.Mid(0, iPos);
	}
	else
	{
		strSQLHostName = m_strSqlServerName;
	}

	set<CAtlString,InsensitiveCompare>lstServers; //нечувствительный к регистру набор
	if(m_setReportServerName.size() == 0)
	{
		// search fail: nothing was found
		lstServers.insert(CAtlString("http://") + CAtlString(strSQLHostName) + CAtlString("/"));
		SetStatus(SelSrvPageReportServerSearchFinishFail);
		strFirsServer = *lstServers.begin();
		m_strReportsFolder = _T("");
	}
	else
	{
		//search successfully
		lstServers.insert(m_setReportServerName.begin(), m_setReportServerName.end());
		strFirsServer = *m_setReportServerName.begin();

		// Start web folder search for the first server
		// if servers was not found don't do anything
		m_setWebFolder.clear();
		CWinThread* pThread = new CReadWebFolder(GetSafeHwnd(),
			m_setWebFolder, CAtlString(strFirsServer), m_hWebFolderventLock);
		if (!pThread->CreateThread())
		{
			ASSERT(!"Cannot create thread");
		}
		SetStatus(SelSrvPageReportServerSearchFinishSuccess);
		
	}
	// unlock shared resource
	SetEvent(m_hEventLock);

	// fill in combo box
	// lock combobox
	CSingleLock singleLock(&m_MutexSQLServerCombo);
	if(singleLock.Lock())
	{
		// get contained items
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
			m_comboReportsServerURL.AddString(*it);
		}
		m_comboReportsServerURL.AddString(cszSearchItemName);

		// find m_setReportServerName.begin() and set it as current sell;
		//m_comboReportsServerURL.SetCurSel(0);
		int iLasVal = m_comboReportsServerURL.FindStringExact(-1, strFirsServer);
		if(CB_ERR != iLasVal)
		{
			m_comboReportsServerURL.SetCurSel(iLasVal);
			m_comboReportsServerURL.GetLBText(iLasVal, m_strReportsServerURL);
		}

		// unlock combobox
		singleLock.Unlock();
	}

	UpdateData(FALSE);

	SetButtons();

	return 0;
}


void CSelectServersPage::StartSearch()
{
	// resrt m_comboDatabase content
	m_comboDatabase.ResetContent();

	// start report search thread
	//m_comboSQLServer.GetWindowTextW(m_strSqlServerName); // desn't work
	// another variant
	int iCurSel = m_comboSQLServer.GetCurSel();
	if(iCurSel != -1)
	{
		m_comboSQLServer.GetLBText(iCurSel, m_strSqlServerName);
	}
	else
	{
		m_comboSQLServer.GetWindowTextW(m_strSqlServerName);
	}

	// start report server url search thread
	m_setReportServerName.clear();
    if (!m_strSqlServerName.IsEmpty())
    {
		// set status
		SetStatus(SelSrvPageDBAndRptSrvSearchStart);

	    CWinThread* pThread = new CReadNetworkURLReportServer(GetSafeHwnd(), 
		    m_setReportServerName, CAtlString(m_strSqlServerName), m_hEventLock);
	    if (!pThread->CreateThread())
	    {
		    ASSERT(!"Cannot create thread");
	    }
    
	    // start data base search thrad
	    m_setSQLSrvDataBase.clear();
	    CWinThread* pThread2 = new CReadSQLSrvDataBase(GetSafeHwnd(), m_setSQLSrvDataBase,
		    m_hSQLSrvDBEventLock, m_strSqlServerName);
	    if (!pThread2->CreateThread())
	    {

		    ASSERT(!"Cannot create thread");
	    }
    }
	else
	{
		EnableControls();
	}

	UpdateData(FALSE);
}


LRESULT CSelectServersPage::OnReadSQLSrvDataBaseFinish(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);

	m_comboDatabase.ResetContent();

	if(m_setSQLSrvDataBase.size() == 0)
	{
		SetStatus(SelSrvPageDataBaseSearchFinishFail);
		m_strDatabase = _T("");
	}
	else
	{
		// fill in m_comboDatabase with m_setSQLSrvDataBase items
		for (set<CAtlString>::iterator it = m_setSQLSrvDataBase.begin(); it != m_setSQLSrvDataBase.end(); ++it)
		{
			m_comboDatabase.AddString(*it);
		}
		m_comboDatabase.SetCurSel(0);
		m_comboDatabase.GetLBText(0, m_strDatabase);
		SetStatus(SelSrvPageDataBaseSearchFinishSuccess);
	}

	// unlock shared resource
	SetEvent(m_hSQLSrvDBEventLock);

	UpdateData(FALSE);

	return 0;
}


LRESULT CSelectServersPage::OnReadWebFolderFinish(WPARAM wParam, LPARAM lParam)
{
	// get text message
	CAtlString* pStr = (CAtlString*)wParam;

	m_comboReportsFolder.ResetContent();

	if(m_setWebFolder.size() == 0)
	{
		SetStatus(SelSrvPageWebFolderSearchFinishFail, pStr);
		m_strReportsFolder = _T("");
	}
	else
	{
		// fill in m_comboDatabase with m_setSQLSrvDataBase items
		for (set<CAtlString>::iterator it = m_setWebFolder.begin(); it != m_setWebFolder.end(); ++it)
		{
			m_comboReportsFolder.AddString(*it);
		}
		m_comboReportsFolder.SetCurSel(0);
		m_comboReportsFolder.GetLBText(0, m_strReportsFolder);
		SetStatus(SelSrvPageWebFolderSearchFinishSuccess);
	}

	// unlock shared resource
	SetEvent(m_hWebFolderventLock);

	UpdateData(FALSE);

	// memory free
	if(pStr != NULL)
	{
		delete pStr;
	}

	return 0;
}


void CSelectServersPage::SetStatus(SelSrvPageActionEnum enCond, CAtlString* strExplanations)
{
	if(strExplanations != NULL)
	{
		m_strExplanations += *strExplanations;
	}
	// SelSrvPageReportServerSearchFinishSuccess = Start Web folder Search
	switch(enCond)
	{
		//Start search case
		case SelSrvPageDBAndRptSrvSearchStart:
		{
			// set condition
			m_enSelSrvPageDatabase = SelSrvPageSearch;
			m_enSelSrvPageReportServer = SelSrvPageSearch;
			// write status
            m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_SEARCHING_REPORTSERVERS_DB);
			//SetWindowTextW(_T("Статус: ищутся экземпляры репорт серверов и БД SQL. Подождите..."));
			// disable controls
			DisableControls();
			return;
		}
		case SelSrvPageWebFolderSearchStart:
		{
			m_enSelSrvPageDatabase = SelSrvPageNone;
			m_enSelSrvPageReportServer = SelSrvPageNone;

			// set condition
			m_enSelSrvPageWebFolder = SelSrvPageSearch;
			// write status
            m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_SEARCHING_WEBFOLDERS);
			//SetWindowTextW(_T("Статус: ищутся веб папки репорт серверов. Подождите..."));
			// disable controls
			DisableControls();
			return;
		}

		//Finish search cases:
		// DataBase
		case SelSrvPageDataBaseSearchFinishFail:
		{
			// change condition
			m_enSelSrvPageDatabase = SelSrvPageNO;

			switch(m_enSelSrvPageReportServer)
			{
				//case SelSrvPageNone:
				//{
				//	break;
				//}
				case SelSrvPageYES:
				{
					// !! If we are here search for web folder was already started 
					switch(m_enSelSrvPageWebFolder)
					{
						//case SelSrvPageNone:
						//{
						//	break;
						//}
						case SelSrvPageYES:
						{
                            m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_ENTER_DBNAME);                            
							//SetWindowTextW(_T("Статус: поиск завершен, введите имя БД вручную."));
							EnableControls();
							return;
						}
						case SelSrvPageNO:
						{
                            m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_ENTER_DBNAME_WEBFOLDER);
							//m_wndStatus.SetWindowText(str); //SetWindowTextW(_T("Статус: поиск завершен, введите имена БД и веб папки вручную."));
							EnableControls();
							return;
						}
						case SelSrvPageSearch:
						{
                            m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_SEARCHING_WEBFOLDERS);
							//SetWindowTextW(_T("Статус: ищутся веб папки. Подождите..."));
							return;
						}
						default:
							return;
					}
					return;
				}
				case SelSrvPageNO:
				{
					// !! If we are here search for web folder condition is NO
                    m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_ENTER_NAMES);
					//SetWindowTextW(_T("Статус: поиск закончен, введите имена вручную."));
					EnableControls();
					return;
				}
				case SelSrvPageSearch:
				{
                    m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_SEARCHING_REPORTSERVERS);
					//_T("Статус: ищутся экземпялры репорт серверов. Подождите..."));
					return;
				}
				default:
					return;
			}
		}
		case SelSrvPageDataBaseSearchFinishSuccess:
		{
			// change condition
			m_enSelSrvPageDatabase = SelSrvPageYES;

			switch(m_enSelSrvPageReportServer)
			{
				//case SelSrvPageNone:
				//{
				//	return;
				//}
				case SelSrvPageYES:
				{
					// !! If we are here search for web folder was already started 
					switch(m_enSelSrvPageWebFolder)
					{
						//case SelSrvPageNone:
						//{
						//	return;
						//}
						case SelSrvPageYES:
						{
                            m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_SEARCH_COMPLETED);
							//SetWindowTextW(_T("Статус: поиск завершен."));
							EnableControls();
							return;
						}
						case SelSrvPageNO:
						{
                            m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_ENTER_WEBFOLDER);
							//SetWindowTextW(_T("Статус: поиск завершен, введите имя веб папки вручную."));
							EnableControls();
							return;
						}
						case SelSrvPageSearch:
						{
                            m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_SEARCHING_WEBFOLDERS); 
							//SetWindowTextW(_T("Статус: ищутся веб папки. Подождите..."));
							return;
						}
						default:
							return;
					}
					return;
				}
				case SelSrvPageNO:
				{
					// !! If we are here search for web folder condition is NO
                    m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_ENTER_REPORTSERVER);
					//SetWindowTextW(_T("Статус: поиск закончен, введите URL репорт сервера вручную."));
					EnableControls();
					return;
				}
				case SelSrvPageSearch:
				{
                    m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_SEARCHING_REPORTSERVERS);
					//_T("Статус: ищутся экземпялры репорт серверов. Подождите..."));
					return;
				}
				default:
					return;
			}
		}


		// Report server
		case SelSrvPageReportServerSearchFinishFail:
		{
			// change condition
			m_enSelSrvPageReportServer = SelSrvPageNO;
			// if we are here the thread for web folders search will NOT BE STARTED
			m_enSelSrvPageWebFolder = SelSrvPageNO;

			switch(m_enSelSrvPageDatabase)
			{
				case SelSrvPageYES:
				{
                    m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_ENTER_REPORTSERVER_WEBFOLDER);
					//SetWindowTextW(_T("Статус: поиск закончен, введите имена репорт сервера и веб папки вручную."));
					EnableControls();
					return;
				}
				case SelSrvPageNO:
				{
                    m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_ENTER_NAMES);
					//SetWindowTextW(_T("Статус: поиск закончен, введите имена вручную."));
					EnableControls();
					return;
				}
				case SelSrvPageSearch:
				{
                    m_strStatus.LoadStringW(IDS_OCRCFG_SELECTSERVERS_SEARCHING_DB);
					//_T("Статус: ищутся SQL БД. Подождите..."));
					return;
				}
				default:
					return;
			}
		}
		case SelSrvPageReportServerSearchFinishSuccess:
		{
			// change condition
			m_enSelSrvPageReportServer = SelSrvPageYES;
			// if we are here the thread for web folders search was started
			m_enSelSrvPageWebFolder = SelSrvPageSearch;

			switch(m_enSelSrvPageDatabase)
			{
				case SelSrvPageYES:
				case SelSrvPageNO:
				{
                    m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_SEARCHING_WEBFOLDERS);
					//_T("Статус: ищутся веб папки. Подождите..."));
					return;
				}
				case SelSrvPageSearch:
				{
                    m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_SEARCHING_DB_WEBFOLDERS);
					//_T("Статус: ищутся SQL БД и веб папки. Подождите..."));
					return;
				}
				default:
					return;
			}
		}

		// web folder
		case SelSrvPageWebFolderSearchFinishFail:
		{
			// change condition
			m_enSelSrvPageWebFolder = SelSrvPageNO;

			if(m_enSelSrvPageReportServer == SelSrvPageNone)
			{
                m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_ENTER_WEBFOLDER);
				//W(_T("Статус: поиск завершен, введите имя веб папки вручную."));
				EnableControls();
			}
			else
			{
				switch(m_enSelSrvPageDatabase)
				{
					//case SelSrvPageNone:
					//{
					//	return;
					//}
					case SelSrvPageYES:
					{
                        m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_ENTER_WEBFOLDER);
				        //SetWindowTextW(_T("Статус: поиск завершен, введите имя веб папки вручную."));
						EnableControls();
						return;
					}
					case SelSrvPageNO:
					{
                        m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_ENTER_DBNAME_WEBFOLDER);
				        //SetWindowTextW(_T("Статус: поиск завершен, введите имена БД и веб папки вручную."));
						EnableControls();
						return;
					}
					case SelSrvPageSearch:
					{
                        m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_SEARCHING_DB);
						//_T("Статус: ищутся SQL БД. Подождите..."));
						return;
					}
					default:
						return;
				}				
			}

			return;

		}
		case SelSrvPageWebFolderSearchFinishSuccess:
		{
			// change condition
			m_enSelSrvPageWebFolder = SelSrvPageYES;

			if(m_enSelSrvPageReportServer == SelSrvPageNone)
			{
                m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_SEARCH_COMPLETED);
		        //SetWindowTextW(_T("Статус: поиск завершен."));
				EnableControls();
			}
			else
			{
				switch(m_enSelSrvPageDatabase)
				{
					//case SelSrvPageNone:
					//{
					//	return;
					//}
					case SelSrvPageYES:
					{
                        m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_SEARCH_COMPLETED);
		                // SetWindowTextW(_T("Статус: поиск завершен."));
						EnableControls();
						return;
					}
					case SelSrvPageNO:
					{
                        m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_ENTER_DBNAME);
				        //SetWindowTextW(_T("Статус: поиск завершен, введите имя БД вручную."));
						EnableControls();
						return;
					}
					case SelSrvPageSearch:
					{
                        m_strStatus.LoadString(IDS_OCRCFG_SELECTSERVERS_SEARCHING_DB);
						//_T("Статус: ищутся SQL БД. Подождите..."));
						return;
					}
					default:
						return;
				}					
			}
			return;
		}
	}
}


void CSelectServersPage::DisableControls()
{
	m_comboSQLServer.EnableWindow(FALSE);
	m_comboDatabase.EnableWindow(FALSE);
	m_comboReportsServerURL.EnableWindow(FALSE);
	m_comboReportsFolder.EnableWindow(FALSE);

	// start timer
	StartTimer();

	m_strExplanations.Empty();

	m_pSheet->SetWizardButtons(0);
	UpdateData(FALSE);
}


void CSelectServersPage::EnableControls()
{
	m_comboSQLServer.EnableWindow(TRUE);
	m_comboDatabase.EnableWindow(TRUE);
	m_comboReportsServerURL.EnableWindow(TRUE);
	m_comboReportsFolder.EnableWindow(TRUE);

	// stop timer
	StopTimer();

	// if lanch first time then show current settings
	if(m_bFirstLanchl)
	{
		CMNRegKey	keyRoot;
		keyRoot.OpenEx(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot, KEY_READ);

		m_strReportsServerURL	= keyRoot.QueryStringValue(cszSAMReportsServerURL, _T(""));
		m_strDatabase			= keyRoot.QueryStringValue(cszSAMDatabase, _T(""));
		m_strReportsFolder		= keyRoot.QueryStringValue(cszSAMReportsFolder, _T(""));

		//UpdateData(FALSE);

		m_bFirstLanchl = false;
	}

	m_strStatus += CAtlString(_T(" ")) + m_strExplanations;

	m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	UpdateData(FALSE);
}


void CSelectServersPage::SetButtons()
{
	UpdateData();

	// check is sql server name and DB name fields is empty
	if(m_strSQLServer.IsEmpty() || m_strReportsServerURL.IsEmpty() || m_strDatabase.IsEmpty() || m_strReportsFolder.IsEmpty() )
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


void CSelectServersPage::OnNamesChange()
{
	UpdateData();
	
	int iCurSel = m_comboDatabase.GetCurSel();
	if(iCurSel != -1)
	{
		m_comboDatabase.GetLBText(iCurSel, m_strDatabase);
	}
	else
	{
		m_strDatabase.Empty();
	}

	iCurSel = m_comboReportsFolder.GetCurSel();
	if(iCurSel != -1)
	{
		m_comboReportsFolder.GetLBText(iCurSel, m_strReportsFolder);
	}
	else
	{
		m_strReportsFolder.Empty();
	}

	UpdateData(FALSE);

	SetButtons();
}


void CSelectServersPage::OnNamesEdit()
{
	SetButtons();
}