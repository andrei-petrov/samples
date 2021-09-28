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

#include "scm/include/SchemaDef.h"

#include "wzr/include/Constants.h"
#include "wzr/include/Thread.h"
#include "wzr/include/Page.h"
#include "wzr/include/SQLSrvAddrParser.h"

#include "ocrcfg/include/misc.h"
#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/ConfigApp.h"
#include "ocrcfg/include/SoftLibraryThread.h"
#include "ocrcfg/include/SoftLibraryPage.h"
#include "ocrcfg/resource/resource.h"

// CSoftLibraryPage dialog

//IMPLEMENT_DYNAMIC(CSoftLibraryPage, WZRPage)

CSoftLibraryPage::CSoftLibraryPage()
	: WZRPage(IDD_OCRCFG_SOFTLIBRARY_PAGE, IDS_OCRCFG_SOFTLIBRARY_TITLE, IDS_OCRCFG_SOFTLIBRARY_SUBTITLE)
{
	m_iProcedure = 0;

	m_psp.dwFlags |= PSP_HASHELP;
}

CSoftLibraryPage::~CSoftLibraryPage()
{
}

BEGIN_MESSAGE_MAP(CSoftLibraryPage, WZRPage)
	//{{AFX_MSG_MAP(CSoftLibraryPage)    
	ON_MESSAGE( PSM_QUERYSIBLINGS,			OnQuerySiblings		)
	ON_MESSAGE( WM_OCRCFG_SOFTLIBRARYPROC_START,	OnSoftLibraryProcStart	)
	ON_MESSAGE( WM_OCRCFG_SOFTLIBRARYPROC_FILE_IMPORT_START, OnSoftLibraryFileImportStart	)
	ON_MESSAGE( WM_OCRCFG_SOFTLIBRARYPROC_STAGE,	OnSoftLibraryProcStage	)
	ON_MESSAGE( WM_OCRCFG_SOFTLIBRARYPROC_FINISH,	OnSoftLibraryProcFinish	)
	ON_EN_CHANGE(IDC_OCRCFG_SOFTLIBRARY_EDIT_XMLFILEPATH, OnXmlPathChange)
	ON_BN_CLICKED(IDC_OCRCFG_SOFTLIBRARY_BUTTON_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CSoftLibraryPage::DoDataExchange(CDataExchange* pDX)
{
	WZRPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSoftLibraryPage)
	DDX_Radio(pDX, IDC_OCRCFG_SOFTLIBRARY_RADIO_EXPORT, m_iProcedure);
	DDX_Control(pDX, IDC_OCRCFG_SOFTLIBRARY_COMBO_SQLSERVER, m_comboSqlServer);
	DDX_Control(pDX, IDC_OCRCFG_SOFTLIBRARY_EDIT_DATABASE, m_edtDatabase);
	DDX_Control(pDX, IDC_OCRCFG_SOFTLIBRARY_RADIO_EXPORT, m_btnExport);
	DDX_Text  (pDX, IDC_OCRCFG_SOFTLIBRARY_COMBO_SQLSERVER, m_strSqlServer);
	DDX_Text  (pDX, IDC_OCRCFG_SOFTLIBRARY_EDIT_DATABASE,  m_strDatabase);
	DDX_Text  (pDX, IDC_OCRCFG_SOFTLIBRARY_EDIT_XMLFILEPATH, m_strXmlFile);
	//}}AFX_DATA_MAP
}


BOOL CSoftLibraryPage::OnInitDialog()
{
	WZRPage::OnInitDialog();

	return TRUE;
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOL CSoftLibraryPage::OnSetActive()
{    
	BOOL b = WZRPage::OnSetActive();

	CMNRegKey	keyRoot;
	try
	{
		keyRoot.OpenEx(HKEY_LOCAL_MACHINE, cszOCRRegistryRoot, KEY_READ);

		m_comboSqlServer.ResetContent();
		m_strSqlServer = keyRoot.QueryStringValue(cszSAMSqlServer, _T(""));
		m_strDatabase  = keyRoot.QueryStringValue(cszSAMDatabase, _T(""));

		m_comboSqlServer.EnableWindow(FALSE);
		m_edtDatabase.EnableWindow(FALSE);
	}
	catch(CMNException* pEx)
	{	
	    CString strAllErrors = CMNException::GetAllErrorMessages(pEx, _T("\n"));
	    CMNLog(_T("[CSoftLibraryPage::OnSetActive]: %s"), strAllErrors);		
		pEx->Delete();
	}

	if ( theApp.m_enScenario != eusExportImportSoftLibrary )
	{
		m_iProcedure = slpImport;
		GetSoftLibraryFilePaths();
	}

	UpdateData(FALSE);

	m_btnExport.EnableWindow( theApp.m_enScenario == eusExportImportSoftLibrary ? TRUE : FALSE );

	CheckWizardNextButton();

	if (theApp.m_bInstallerMode || (theApp.m_enScenario == eusExportImportSoftLibrary))
    {
        if (m_btnSkipPage.m_hWnd)
            m_btnSkipPage.ShowWindow(SW_HIDE);
    }

    if (theApp.m_bUnattendedMode)
	{
		::PostMessage(m_pSheet->GetSafeHwnd(), PSM_PRESSBUTTON, PSBTN_NEXT, 0); // and simulate next button press
	}

	return b;
}

// -----------------------------------------------------------------------------------------------------------------------------
void CSoftLibraryPage::GetSoftLibraryFilePaths()
{
	//Получить относительные пути к файлам Библиотеки ПО
	set<SoftLibraryFileInfoC, SoftLibraryInfoStructCompare> setRelativeFilePaths = 
		theApp.GetConfigXMLData().GetSoftLibraryFiles();

	m_strXmlFile.Empty();
	m_setXmlFileNames.clear();

	//Если файл один _______________________________________________________________________________________
	if(setRelativeFilePaths.size()==1)
	{
		//На экране отображаться будет полный путь
		m_strXmlFile = setRelativeFilePaths.begin()->m_strSoftLibraryFilePath + setRelativeFilePaths.begin()->m_strSoftLibraryFileName;
		//Сохранить полный путь в список
		m_setXmlFileNames.insert(m_strXmlFile);
	}
	else //Если файлов > 1 __________________________________________________________________________________
	{
			
		for(set<SoftLibraryFileInfoC, SoftLibraryInfoStructCompare>::iterator it=setRelativeFilePaths.begin(); it!=setRelativeFilePaths.end(); ++it)
		{
			CString strFullPath = it->m_strSoftLibraryFilePath + it->m_strSoftLibraryFileName;

			//Отображать на экране только имена файлов ____________
			if( ! m_strXmlFile.IsEmpty())
				m_strXmlFile+=_T(" ");
			//
			m_strXmlFile+=_T("\"");
			m_strXmlFile.Append(it->m_strSoftLibraryFileName);
			m_strXmlFile+=_T("\"");
			
			//Сохранить полный путь в список
			m_setXmlFileNames.insert(strFullPath);
		}
	}
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOL CSoftLibraryPage::OnKillActive()
{	
	return WZRPage::OnKillActive();
}

LRESULT CSoftLibraryPage::OnWizardBack()
{
	LRESULT id = -1;
	CConfigXMLData& data = theApp.GetConfigXMLData();
	if ( theApp.m_enScenario == eusExportImportSoftLibrary )
	{
		id = IDD_OCRCFG_WELCOME_PAGE;
	}
	else
	{
		if (data.IsDeployReports())
			id = IDD_OCRCFG_DEPLOYRPT_PAGE;
		else if (data.IsCreateServer() || data.IsRunSqlScripts())
			id = IDD_OCRCFG_CREATEDB_PAGE;
		else
			id = IDD_OCRCFG_WELCOME_PAGE;
	}
	return id;
}

LRESULT CSoftLibraryPage::OnWizardNext()
{	
	if (m_iSkipPage)
		return WZRPage::OnWizardNext();

	if ( UpdateData(TRUE) == TRUE ) // correct input
	{
		OnThreadStarted();
	}
	return -1; // stay at this page until install completes
}


// CSoftLibraryPage implementation

WZRThread* CSoftLibraryPage::ConstructThread() const
{
	CAtlString strSQLHostName;

	// get sql server name
	// 1. try to get it from application object
	strSQLHostName = theApp.GetSQLServerHostName();
	if(strSQLHostName.IsEmpty())
	{
		// 2. if here then we start from import/export page and we have to recive host name by ourself;
		CSQLSrvAddrParser SrvAddr;
		SrvAddr.SetStringAddressString(m_strSqlServer);
		strSQLHostName = SrvAddr.GetSQLAddressString();
	}

	return new CSoftLibraryThread(GetSafeHwnd(), strSQLHostName, m_strDatabase,
		(CSoftLibActionEnum)m_iProcedure, &m_setXmlFileNames);
}

// CSoftLibraryPage message handlers

LRESULT CSoftLibraryPage::OnQuerySiblings(WPARAM wParam, LPARAM lParam)
{
	if(wParam == WM_OCRCFG_QUERY_SOFTLIBRARY)
	{
		::memcpy((void*) lParam, (void*) m_strThreadResults.GetString(), m_strThreadResults.GetLength() * sizeof(TCHAR));
		return 1; // answer
	}
	return 0; // ignore
}

LRESULT CSoftLibraryPage::OnSoftLibraryProcStart(WPARAM wParam, LPARAM lParam)
{
	if ( m_iProcedure == slpExport )
	{
		m_ctlProgress.SetRange(0, 1);
		m_ctlProgress.SetStep(1);
		m_ctlProgress.SetPos(0);
		m_strStage.LoadString(IDS_OCRCFG_SOFTLIBRARY_EXPORT_PROCESS);
		ShowProgress();
	}
	else //slpImport
	{
		m_ctlProgress.SetRange(0, (short)lParam);
		m_ctlProgress.SetStep(1);
		m_ctlProgress.SetPos(0);
	}
	
	return 0;
}

LRESULT CSoftLibraryPage::OnSoftLibraryFileImportStart(WPARAM wParam, LPARAM lParam)
{
	m_strStage.LoadString(IDS_OCRCFG_SOFTLIBRARY_IMPORT_PROCESS);
	wchar_t szExeName[MAX_PATH];
	AfxGetFileName((LPCTSTR)lParam, szExeName, MAX_PATH);
	m_strStage.Append(szExeName);
	ShowProgress();
	return 0;
}

LRESULT CSoftLibraryPage::OnSoftLibraryProcStage(WPARAM wParam, LPARAM lParam)
{
	CString strFormat;
	DWORD   dwNewPos   = (DWORD)wParam;
	DWORD dwPos = (DWORD)m_ctlProgress.GetPos();
	if (dwPos < dwNewPos)
	{
		m_ctlProgress.StepIt();
		ShowProgress();
	}
	return 0;
}

LRESULT CSoftLibraryPage::OnSoftLibraryProcFinish (WPARAM wParam, LPARAM lParam)
{
	BOOL bError = (BOOL)wParam;

	CString strError;
	if ( m_pThread ) strError = m_pThread->GetErrorString();    
	OnThreadFinished();

	if ( bError )
	{
		if ( ! theApp.m_bUnattendedMode)
		{
			TException tex(IDE_OCRCFG_ERROR_STRING);
			tex << strError;
			tex.ReportError();
		}
	}
	else
	{
		//for (int i=0; i<m_pSheet->GetPageCount(); i++)
		//{
		//	CPropertyPage* pPage = m_pSheet->GetPage(i);
		//	if ( pPage )
		//	{
		//		if ((LRESULT)pPage->m_psp.pszTemplate == IDD_OCRCFG_FINISH_PAGE )
		//		{
		//			m_pSheet->SetActivePage( i );
		//			break;
		//		}
		//	}
		//}
        int iActivePage = m_pSheet->GetActiveIndex();
	    m_pSheet->SetActivePage(iActivePage + 1);     
	}
	return 0;
}

void CSoftLibraryPage::OnXmlPathChange()
{
	//Если ранее было указано несколько файлов импорта
	if(m_setXmlFileNames.size()>1)
	{
		//Предупредить пользователя
		//Редактирование строки позволяет указать путь только к одному файлу
		if(IDOK==AfxMessageBox(IDS_OCRCFG_EDIT_IMPORT_PATH, MB_OKCANCEL | MB_ICONHAND | MB_DEFBUTTON2))
		{
			//Очистить список файлов и поместить в него одно текущее значение пути к файлу
			m_setXmlFileNames.clear();
			UpdateData(TRUE);
			m_setXmlFileNames.insert(m_strXmlFile);
		}
		else 
		{
			//Восстановить предыдущее значение
			UpdateData(FALSE);
		}
	}
	else //если список файлов содержит только один путь
	{
		//Очистить список файлов и поместить в него одно текущее значение пути к файлу
		m_setXmlFileNames.clear();
		UpdateData(TRUE);
		m_setXmlFileNames.insert(m_strXmlFile);
	}

	CheckWizardNextButton();
}

void CSoftLibraryPage::CheckWizardNextButton()
{
	UpdateData(TRUE);
	UINT uiWizButtons = 0;
	if ( theApp.m_bInstallerMode || theApp.m_bUnattendedMode )
    {
		uiWizButtons = 0;
    }
	else
	{
		uiWizButtons = PSWIZB_BACK;
	}
	if ( ! m_strXmlFile.IsEmpty() )
	{
		uiWizButtons |= PSWIZB_NEXT;
	}
	m_pSheet->SetWizardButtons(uiWizButtons);
}

void CSoftLibraryPage::OnBrowse()
{
	UpdateData(TRUE);

	CString strFileExt, strFileFilter;
	strFileExt.LoadString(IDS_OCRCFG_XMLCONFIG_DEFEXT);
	strFileFilter.LoadString(IDS_OCRCFG_XMLCONFIG_FILTER);

	BOOL bOpenFlag = FALSE;
	DWORD dwFlags = 0;
	if ( m_iProcedure == slpImport )
	{
		bOpenFlag = TRUE;
		dwFlags = OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT ;
	}
	else
	{
		dwFlags = OFN_OVERWRITEPROMPT | OFN_DONTADDTORECENT | OFN_HIDEREADONLY;
		bOpenFlag = FALSE;
	}

	CFileDialog dlg( bOpenFlag,
		strFileExt, m_strXmlFile,
		dwFlags,
		strFileFilter, AfxGetMainWnd(), 0, FALSE);

	//Настроить буфер для выбора нескольких файлов _________________
	CString fileName;
	const int c_cMaxFiles = 100;
	const int c_cbBuffSize = (c_cMaxFiles * (MAX_PATH + 1)) + 1;
	dlg.GetOFN().lpstrFile = fileName.GetBuffer(c_cbBuffSize);
	dlg.GetOFN().nMaxFile = c_cbBuffSize;

	if ( dlg.DoModal() == IDOK )
	{
		//Получить список выбранных файлов __________________________________
		POSITION pos = dlg.GetStartPosition();
		//
		m_strXmlFile.Empty();
		m_setXmlFileNames.clear();
		//
		int nFileCount=0;
        while (pos != NULL)
        {
			CString str=dlg.GetPathName();
			CString strPathName=dlg.GetNextPathName(pos);

			m_setXmlFileNames.insert(strPathName);

			//Заполнить экранное имя файла __________________
			if(m_strXmlFile.GetLength()>0)
				m_strXmlFile+=_T(" ");
			//
			m_strXmlFile+=_T("\"");
			//
			wchar_t szExeName[MAX_PATH];
			AfxGetFileName(strPathName, szExeName, MAX_PATH);
			m_strXmlFile.Append(szExeName);
			//
			m_strXmlFile+=_T("\"");

			nFileCount++;
        }

		//Если был выбран один файл _________________________________________
		//сделать представление как в предыдущей весрии программы
		if( nFileCount==1 )
		{
			m_strXmlFile=dlg.GetPathName();
		}
		
		UpdateData(FALSE);
	}

	fileName.ReleaseBuffer ();

	CheckWizardNextButton();
}