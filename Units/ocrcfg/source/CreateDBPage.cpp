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
#include "cmn/include/Misc.h"

#include "v/include/Constants.h"

#include "db/include/Constants.h"
#include "db/include/Storage.h"
#include "db/include/SqlBrowser.h"

#include "scm/include/SchemaDef.h"

#include "wzr/include/Page.h"
#include "wzr/include/Sheet.h"
#include "wzr/include/SelectServersDlg.h"
#include "wzr/include/SQLSrvAddrParser.h"
#include "wzr/include/Thread.h"

#include "ocrcfg/include/misc.h"
#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/ConfigApp.h"
#include "ocrcfg/include/CreateDBThread.h"
#include "ocrcfg/include/CreateDBPage.h"
#include "ocrcfg/resource/resource.h"
//#include "samcfg/resource/samcfg.rc"

#pragma warning(disable: 4800)

const TCHAR cszSearchItemName[] = _T("<Поиск...>");

// CCreateDBPage dialog

//IMPLEMENT_DYNAMIC(CCreateDBPage, WZRPage)

CCreateDBPage::CCreateDBPage(COcrConfigData& rConfigData, int* pnModificationFlags)
	: WZRPage(IDD_OCRCFG_CREATEDB_PAGE, IDS_OCRCFG_CREATEDB_TITLE, IDS_OCRCFG_CREATEDB_SUBTITLE),
	m_rConfigData(rConfigData),	
	m_pnModificationFlags(pnModificationFlags),
	m_bError(true)
{    
	m_psp.dwFlags |= PSP_HASHELP;

	if(!m_pnModificationFlags)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_POINTER_TO_INT_IS_NULL);
		*pEx << _T("CCreateDBPage::CCreateDBPage");
		ThrowException(pEx);
	}

	m_vecDatabases.push_back(new CDeliceTechProcDBDesc(NULL));
	m_vecDatabases.push_back(new CRioIpsDBDesc(NULL));


	m_strDatabase	= _T("DELICE_TECH_PROCESS");
	m_strDatabase2	= _T("RIO_IPS");
}

CCreateDBPage::~CCreateDBPage()
{
	
}

BEGIN_MESSAGE_MAP(CCreateDBPage, WZRPage)
    //{{AFX_MSG_MAP(CCreateDBPage)    
	ON_MESSAGE		( PSM_QUERYSIBLINGS,						OnQuerySiblings)
    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_START,		OnScriptExcecutionStart  )
    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_STAGE,		OnScriptExcecutionStage  )
    ON_MESSAGE      ( WM_OCRCFG_SCRIPT_EXCECUTION_FINISH,		OnScriptExcecutionFinish )
    //ON_MESSAGE      ( WM_OCRCFG_CUSTOMSCRIPT_EXCECUTION_START,	OnCustomScriptExcecutionStart )
	ON_CBN_SELENDOK	( IDC_OCRCFG_CREATEDB_COMBO_SQLSERVER,		OnSQLComboChange )
	ON_EN_CHANGE    ( IDC_OCRCFG_CREATEDB_EDIT_DATABASE,		OnNamesChange )
	ON_CBN_EDITCHANGE(IDC_OCRCFG_CREATEDB_COMBO_SQLSERVER,		OnNamesChange )
	ON_BN_CLICKED	( IDC_WZR_CHECK_SKIP, OnSkipBtnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCreateDBPage::DoDataExchange(CDataExchange* pDX)
{
  	WZRPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CCreateDBPage)
	DDX_Control	(pDX, IDC_OCRCFG_CREATEDB_COMBO_SQLSERVER,	m_comboSqlServer);
	DDX_Control	(pDX, IDC_OCRCFG_CREATEDB_EDIT_DATABASE,	m_edtDatabase);
	DDX_Control	(pDX, IDC_OCRCFG_CREATEDB_EDIT_DATABASE_2,	m_edtDatabase2);
    DDX_Text	(pDX, IDC_OCRCFG_CREATEDB_COMBO_SQLSERVER,	m_strSqlServer);
    DDX_Text	(pDX, IDC_OCRCFG_CREATEDB_EDIT_DATABASE,	m_strDatabase);
	DDX_Text	(pDX, IDC_OCRCFG_CREATEDB_EDIT_DATABASE_2,	m_strDatabase2);
    //}}AFX_DATA_MAP
}


BOOL CCreateDBPage::OnInitDialog()
{
    WZRPage::OnInitDialog();
	return TRUE;
}


BOOL CCreateDBPage::OnSetActive()
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if (!pApp)
		return FALSE;

	//CMNRegKey keyRoot;
	//UINT uiTitleId=0, uiSubtitleId=0;

	try
	{
		//keyRoot.OpenEx(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot, KEY_READ);
		m_rConfigData.LoadFromRegistry();
		m_strSqlServer = m_rConfigData.m_strSQLServerAddr;

		switch ( pApp->m_enScenario )
		{		
		case CUseScenarioEnum::eusRemove:			
			
			m_comboSqlServer.ResetContent();
			// load values from registry
			//m_strSqlServer = keyRoot.QueryStringValue(cszSAMSqlServer, _T(""));
			//m_strDatabase  = keyRoot.QueryStringValue(cszSAMDatabase, _T(""));
			break;			
			
		case CUseScenarioEnum::eusCreateNew:
			CConfigXMLData& data = pApp->GetConfigXMLData();

			m_comboSqlServer.EnableWindow(TRUE);
			m_edtDatabase.EnableWindow(TRUE);
			
			set<CAtlString> setLocalServerNames;
			CString strValueFromReristry;

			/* --- this code is temporary excluded to fix OCRCONFIG-13
			if(!pApp->m_bInstallerMode)
			{
				// get sql server name from registry
				m_strSqlServer = keyRoot.QueryStringValue(cszSAMSqlServer, _T(""));					
			}			
			// load local sql servers from wmi
			CWaitCursor waitcur;
			CSqlAndReportingServersInfo serverInfo;
			serverInfo.ReadLocalSqlServerNames( setLocalServerNames );

			//Заполнить comboBox для выбора серверов ____________________
			m_comboSqlServer.ResetContent();
			if(!strValueFromReristry.IsEmpty())
			{
				// put sql name from registry to first string
				//m_comboSqlServer.AddString(strValueFromReristry);
				setLocalServerNames.insert(strValueFromReristry);
			}

			for (set<CAtlString>::iterator it = setLocalServerNames.begin(); it != setLocalServerNames.end(); ++it)
			{
				CString strServer = *it;
				m_comboSqlServer.AddString(strServer);
			}

			// assign value to m_strSqlServer. It is combo text.
			if(m_comboSqlServer.GetCount())
			{
				if(strValueFromReristry.IsEmpty())
				{
					m_comboSqlServer.SetCurSel(0);
					m_comboSqlServer.GetLBText(m_comboSqlServer.GetCurSel(), m_strSqlServer);					
				}
				else
				{
					m_strSqlServer = strValueFromReristry;
				}
			}
			else
			*/

			if (m_strSqlServer.GetLength() == 0)
			{
				// if nothing was found then we try to recieve the current computer name
				wchar_t lpszComputer[255];
				DWORD dComputer = sizeof(lpszComputer);

				if(GetComputerName(lpszComputer, &dComputer))
				{
					// create string
					m_strSqlServer = CString(lpszComputer); // +CString(_T("\\SQLExpress"));						
				}
				else
				{
					if(m_strSqlServer.IsEmpty()) // if we found nothing then get setings
						m_strSqlServer = data.GetSQLServer();
				}
			}
			m_comboSqlServer.AddString(m_strSqlServer);
			//m_comboSqlServer.AddString(cszSearchItemName);

			/*[m.fomin] если сервера не обнаружены, т.е. не к кому конектиться, то зачем что-то предлагать по умолчанию?
			if ( m_strSqlServer.IsEmpty() )
			{
				m_strSqlServer = cszDefSAMSqlServerValue;
			}*/
			//
			//m_strDatabase  = keyRoot.QueryStringValue(cszSAMDatabase,  cszDefSAMDatabaseValue);

			if ( data.IsCreateServer() )
			{
				//if(m_strSqlServer.IsEmpty()) // if we found nothing then get setings
					//m_strSqlServer = data.GetSQLServer();

				if(m_strDatabase.IsEmpty())
					m_strDatabase  = data.GetDBName();

				if (m_btnSkipPage.m_hWnd)
				{
					m_btnSkipPage.ShowWindow(SW_SHOWNORMAL);
				}
			}
			else
			{
				if (m_btnSkipPage.m_hWnd)
				{
					m_btnSkipPage.ShowWindow(SW_HIDE);
				}
			}			
			break;
		}
	}
	catch(CMNException* pEx)
	{	
	    CString strAllErrors = CMNException::GetAllErrorMessages(pEx, _T("\n"));
	    CMNLog(_T("[CCreateDBPage::OnSetActive]: %s"), strAllErrors);		
		pEx->Delete();
	}
	UpdateData(FALSE);

	//m_strPrevSqlServer = keyRoot.QueryStringValue(cszSAMSqlServer, _T(""));
	//m_strPrevDatabase  = keyRoot.QueryStringValue(cszSAMDatabase,  _T(""));


	if (pApp->m_enScenario == CUseScenarioEnum::eusRemove)
	{
		CString strTitle, strSubtitle;
		strTitle.LoadString(IDS_OCRCFG_DELETEDB_TITLE);
		strSubtitle.LoadString(IDS_OCRCFG_DELETEDB_SUBTITLE);
		PropSheet_SetHeaderTitle(m_pSheet->GetSafeHwnd(), m_pSheet->GetActiveIndex(), (const TCHAR*)strTitle);
		PropSheet_SetHeaderSubTitle(m_pSheet->GetSafeHwnd(), m_pSheet->GetActiveIndex(), (const TCHAR*)strSubtitle);

		m_comboSqlServer.EnableWindow(FALSE);
		m_edtDatabase.EnableWindow(FALSE);
		m_edtDatabase2.EnableWindow(FALSE);
	}

    // in installer mode 'skip' checkbox is not visible
    if (pApp->m_bInstallerMode || pApp->m_enScenario == CUseScenarioEnum::eusRemove)
    {
        if (m_btnSkipPage.m_hWnd)
            m_btnSkipPage.ShowWindow(SW_HIDE);
    }

    if (pApp->m_bUnattendedMode)
	{
		::PostMessage(m_pSheet->GetSafeHwnd(), PSM_PRESSBUTTON, PSBTN_NEXT, 0); // and simulate next button press
	}

	BOOL bResult = WZRPage::OnSetActive();
	SetButtons();
    return bResult;
}


BOOL CCreateDBPage::OnKillActive()
{
	if(!m_bError)
	{
		m_rConfigData.SaveToRegistry();
		/*
		CMNRegKey keyRoot;
		try
		{
			keyRoot.OpenEx(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot, KEY_WRITE);		
			
			CMNCheckWIN32Error( keyRoot.SetStringValue(cszSAMSqlServer,		m_rConfigData.m_strSQLServerAddr));
			CMNCheckWIN32Error( keyRoot.SetStringValue(cszSAMSqlComputer,	m_rConfigData.m_strComputerAddr));
			CMNCheckWIN32Error( keyRoot.SetStringValue(cszSAMDatabase,		m_strDatabase) );	
			
		}
		catch(CMNException* pEx)
		{	
			CString strAllErrors = CMNException::GetAllErrorMessages(pEx, _T("\n"));
			CMNLog(_T("[CCreateDBPage::OnKillActive]: %s"), strAllErrors);		
			pEx->Delete();
		}
		*/
	}

	return WZRPage::OnKillActive();
}

LRESULT CCreateDBPage::OnWizardBack()
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());

	switch (pApp->m_enScenario)
	{
	case CUseScenarioEnum::eusCreateNew:
		return IDD_OCRCFG_CREATEFOLDER_PAGE;		
	case CUseScenarioEnum::eusRemove:
		return IDD_OCRCFG_REGJOBS_PAGE;		
	}
	return -1;
}

LRESULT CCreateDBPage::OnWizardNext()
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

		CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
		if ( ! pApp)
		{
			ThrowException(new TException(IDE_OCRCFG_ERROR_RECIEVE_POINTER));
		}
		CConfigXMLData& data = pApp->GetConfigXMLData();

		CSQLSrvAddrParser parser;
		parser.ParseInputSrvAddr(m_strSqlServer);
		//m_strSqlServerHostName = parser.GetSQLAddressString();
		
		m_rConfigData.m_strSQLServerAddr = parser.GetSQLAddressString();		// it may  be some_computer/some_instance or 192.168.86.1/some_instance
		m_rConfigData.m_strComputerAddr  = parser.GetComputerAddressString();	// it will be some_computer or 192.168.86.1

		pApp->SetSQLServerName(m_rConfigData.m_strSQLServerAddr, m_strSqlServer);

		if ( data.IsCreateServer() )
		{
			data.SetSQLServer(m_rConfigData.m_strSQLServerAddr);
			//data.SetDBName(m_strDatabase);
		}

		if ( m_iSkipPage )
		{
			m_bError = false;

			//Вернуть предыдущие значения
			//if( m_strSqlServer.CompareNoCase( m_strPrevSqlServer ) != 0 
			//	|| m_strDatabase.CompareNoCase( m_strPrevDatabase ) != 0 )
			//{
			//	CAtlString strMessage;
			//	strMessage.LoadString(IDS_OCRCFG_DB_SKIP_STEP);
			//	CMNLog(_T("[CCreateDBPage::OnWizardNext]: %s"), strMessage);
			//	AfxMessageBox(strMessage);
			//	m_strSqlServer = m_strPrevSqlServer;
			//	m_strDatabase  = m_strPrevDatabase;
			//	UpdateData(FALSE);
			//	return -1;
			//}
			
			//OnKillActive();

			return IDD_OCRCFG_APPLYSCRIPTS_PAGE;
		}
		
		ProcessDB();
		//m_bError = false;
		return -1; // stay at this page until install completes
	}
	catch(TException* pTEx)
	{
		CAtlString strErrMessage = TException::GetAllErrorMessages(pTEx);
		CMNLog(_T("[CCreateDBPage::OnWizardNext]: %s"), strErrMessage);
		pTEx->ReportError();
		pTEx->Delete();
		return -1;
	}
}


void CCreateDBPage::SetButtons()
{
	if ( ! theApp.m_bUnattendedMode)
	{
		UpdateData();

		// check is sql server name and DB name fields is empty
		if(m_strSqlServer.IsEmpty() || m_strDatabase.IsEmpty())
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

WZRThread* CCreateDBPage::ConstructThread() const
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if ( ! pApp ) 
		return NULL;
	CConfigXMLData& data = pApp->GetConfigXMLData();

	return new CCreateDBThread( GetSafeHwnd(), m_strSqlServer, m_rConfigData.m_strSQLServerAddr, m_vecDatabases, m_rConfigData, data, m_pnModificationFlags );
}

LRESULT CCreateDBPage::ProcessDB()
{
	CConfigApp* pApp = dynamic_cast<CConfigApp*>(TGetApp());
	ASSERT(pApp);
	if ( !pApp )  
		return 0;
	CConfigXMLData& data = pApp->GetConfigXMLData();
	//set<CString> setScripts = data.GetScriptFilePaths();
	/*
	DBStorage::FillScriptIDVector();
	int nScripts = 0;
	if ( data.IsCreateServer() )
	{		
		nScripts += DBStorage::m_vecScriptID.size();
	}
	if ( data.IsRunSqlScripts() )
	{
		nScripts += data.GetScriptFilePaths().size();
	}
	m_ctlProgress.SetRange((short) 0, (short) ( nScripts > 0 ? (nScripts-1) : 0 ));
	*/
	m_ctlProgress.SetRange( (short) 0, (short) m_vecDatabases.size() );  // restore two databases	
    m_ctlProgress.SetStep(1);
	m_ctlProgress.SetPos(0);
    OnThreadStarted();

    return 0;
}

// checks for two templates and one SVE to be installed and fills out the edit control with missed components names
/*
bool CCreateDBPage::CheckInstalledComponents(bool& bAllPossibleComponentsInstalled)
{
    // check for installed components

	bAllPossibleComponentsInstalled = false;

	if (m_strSqlServer.GetLength() && m_strDatabase.GetLength())
	{	
		try		
		{
			DBStorage storage;
			storage.Connect(cszDefSAMConnectionString, m_strSqlServer, _T("master"));
			bAllPossibleComponentsInstalled = (bool)storage.IsDatabaseExist(m_strDatabase);		
			storage.Disconnect();		
		}
		catch(CMNException* pEx)
		{
            CString strAllErrors = CMNException::GetAllErrorMessages(pEx, _T("\n"));
		    CMNLog(_T("[CCreateDBPage::CheckInstalledComponents]: %s"), strAllErrors);		
			pEx->Delete();
			return false;
		}
	}
    return true;
}
*/
// CCreateDBPage message handlers


LRESULT CCreateDBPage::OnQuerySiblings(WPARAM wParam, LPARAM lParam)
{
    if (wParam == WM_OCRCFG_QUERY_CREATEDB)
    {
        ::memcpy((void*) lParam, (void*) m_strThreadResults.GetString(), m_strThreadResults.GetLength() * sizeof(TCHAR));
		return 1; // answer
    }	
    return 0; // ignore
}

LRESULT CCreateDBPage::OnScriptExcecutionStart (WPARAM wParam, LPARAM lParam)
{
	switch (theApp.m_enScenario)
	{
	case CUseScenarioEnum::eusCreateNew:
		m_strStage.FormatMessage(IDS_OCRCFG_DB_CREATE_PROCESS);
		break;
	case CUseScenarioEnum::eusRemove:
		m_strStage.FormatMessage(IDS_OCRCFG_DB_REMOVE_PROCESS);
		break;
	}

	UpdateData(FALSE);
	return 0;
}

LRESULT CCreateDBPage::OnScriptExcecutionStage (WPARAM wParam, LPARAM lParam)
{
	CString strFormat;
	DWORD dwPercent = (DWORD) wParam;	
    DWORD iPos = (DWORD)m_ctlProgress.GetPos();
	TRACE(_T("[CCreateDBPage::OnScriptExcecutionStage] %i, %i\n"), dwPercent, iPos);
    if (iPos < dwPercent)
    {
        m_ctlProgress.StepIt();
		TRACE(_T("[CCreateDBPage::OnScriptExcecutionStage] StepIt()\n"));
		ShowProgress();
    }
    return 0;
}

LRESULT CCreateDBPage::OnScriptExcecutionFinish (WPARAM wParam, LPARAM lParam)
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

	switch (pApp->m_enScenario)
	{
	case CUseScenarioEnum::eusCreateNew:
		m_pSheet->SetActivePage((UINT)IDD_OCRCFG_APPLYSCRIPTS_PAGE);	// RegJobs
		break;
	case CUseScenarioEnum::eusRemove:
		m_pSheet->SetActivePage((UINT)IDD_OCRCFG_FINISH_PAGE);			// DB removed, nothing more to do
		break;
	}

	return 0;
}

/*
LRESULT CCreateDBPage::OnCustomScriptExcecutionStart(WPARAM wParam, LPARAM lParam)
{
	m_strStage.LoadString(IDS_OCRCFG_CUSTOMSCRIPT_EXECUTION);
	UpdateData(FALSE);

	return 0;
}
*/

void CCreateDBPage::OnSQLComboChange()
{
	UpdateData();

	int iLasVal = 0;
	iLasVal = m_comboSqlServer.GetCurSel() - 1;

	// check is choosen string is cszSearchItemName
	CString strBuf;
	m_comboSqlServer.GetLBText(m_comboSqlServer.GetCurSel(), strBuf);
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
				for (int i = 0; i < m_comboSqlServer.GetCount(); i++)
				{
					CString strTemp;
					m_comboSqlServer.GetLBText(i, strTemp);
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
				m_comboSqlServer.ResetContent();
				// fill in combobox with new items
				for (set<CAtlString,InsensitiveCompare>::iterator it=lstServers.begin(); it!=lstServers.end(); ++it)
				{
					CString strServer = *it;
					m_comboSqlServer.AddString(strServer);
				}
				m_comboSqlServer.AddString(cszSearchItemName);

				// set cur sel element which was returned from dialog
				iLasVal = m_comboSqlServer.FindStringExact(0, strTemp);
				if(CB_ERR != iLasVal)
				{
					m_comboSqlServer.SetCurSel(iLasVal);
					m_comboSqlServer.GetLBText(m_comboSqlServer.GetCurSel(), strBuf);
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
				m_comboSqlServer.SetCurSel(iLasVal);
				m_comboSqlServer.GetLBText(m_comboSqlServer.GetCurSel(), strBuf);
				break;
			}
		};
	}

	m_strSqlServer = strBuf;

	UpdateData(FALSE);

	SetButtons();
}

void CCreateDBPage::OnNamesChange()
{
	SetButtons();
}

void CCreateDBPage::OnSkipBtnClick()
{
	UpdateData();

	if ( m_iSkipPage )
	{

		// Дизэйбл кнопки next и отображение сообщения
		/*
		if( m_strSqlServer.CompareNoCase( m_strPrevSqlServer ) != 0 || m_strDatabase.CompareNoCase( m_strPrevDatabase ) != 0)
		{
			// disable Next button
			m_pSheet->SetWizardButtons(PSWIZB_BACK);
			UpdateData(FALSE);

			CAtlString strMessage;
			strMessage.LoadString(IDS_OCRCFG_DB_SKIP_STEP);
			CMNLog(_T("[CCreateDBPage::OnWizardNext]: %s"), strMessage);
			AfxMessageBox(strMessage);
		}
		else*/
		{
			// enable next button
			if(!m_strSqlServer.IsEmpty() && !m_strDatabase.IsEmpty())
			{
				m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
			}
			else
			{
				CAtlString strMessage;
				strMessage.LoadString(IDS_OCRCFG_DB_EMPTY_FIELD);
				CMNLog(_T("[CCreateDBPage::OnWizardNext]: %s"), strMessage);
				AfxMessageBox(strMessage);
			}
		}
	}
	else
	{
		// enable next button
		if(!m_strSqlServer.IsEmpty() && !m_strDatabase.IsEmpty())
		{
			m_pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		}
		else
		{
			CAtlString strMessage;
			strMessage.LoadString(IDS_OCRCFG_DB_EMPTY_FIELD);
			CMNLog(_T("[CCreateDBPage::OnWizardNext]: %s"), strMessage);
			AfxMessageBox(strMessage);
		}
	}

	UpdateData(FALSE);
}