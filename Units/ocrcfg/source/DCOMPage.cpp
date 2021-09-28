// CWelcomePage.cpp : implementation file
//

#include "stdafx.h"
#include <atlsecurity.h>

#include "cmn/include/McException.h"

#include "ocrcfg/include/ConfigApp.h"
#include "ocrcfg/include/Constants.h"
#include "wzr/include/Thread.h"
#include "ocrcfg/include/DCOMThread.h"
#include "wzr/include/Page.h"
#include "wzr/include/Sheet.h"
#include "ocrcfg/include/DCOMPage.h"
#include "ocrcfg/include/misc.h"
#include "ocrcfg/resource/resource.h"


// CDCOMPage dialog

//IMPLEMENT_DYNAMIC(CDCOMPage, WZRPage)

CDCOMPage::CDCOMPage()
	: WZRPage(IDD_OCRCFG_DCOM_PAGE, IDS_OCRCFG_DCOM_TITLE, IDS_OCRCFG_DCOM_SUBTITLE)
{
	m_psp.dwFlags |= PSP_HASHELP;
}

CDCOMPage::~CDCOMPage()
{
}

BEGIN_MESSAGE_MAP(CDCOMPage, WZRPage)
	//{{AFX_MSG_MAP(CDCOMPage)    
	ON_MESSAGE( PSM_QUERYSIBLINGS,			OnQuerySiblings		)
	ON_MESSAGE( WM_OCRCFG_DCOMCONFIGURE_START,	OnDCOMCfgStart	)
	ON_MESSAGE( WM_OCRCFG_DCOMCONFIGURE_STAGE,	OnDCOMCfgStage	)
	ON_MESSAGE( WM_OCRCFG_DCOMCONFIGURE_FINISH,	OnDCOMCfgFinish	)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDCOMPage::DoDataExchange(CDataExchange* pDX)
{
	WZRPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDCOMPage)
	DDX_Text(pDX, IDC_OCRCFG_DCOM_EDIT_LOGIN, m_strDCOMLogin);
	//}}AFX_DATA_MAP
}


BOOL CDCOMPage::OnInitDialog()
{
	WZRPage::OnInitDialog();

	return TRUE;
}


BOOL CDCOMPage::OnSetActive()
{    
	BOOL b = WZRPage::OnSetActive();

	if ( m_btnSkipPage.m_hWnd )
	{
		m_btnSkipPage.ShowWindow( SW_HIDE );
	}

	return b;
}

BOOL CDCOMPage::OnKillActive()
{	
	return WZRPage::OnKillActive();
}

LRESULT CDCOMPage::OnWizardBack()
{
	return IDD_OCRCFG_WELCOME_PAGE;
}

LRESULT CDCOMPage::OnWizardNext()
{	
	if (m_iSkipPage)
		return WZRPage::OnWizardNext();

	if ( UpdateData(TRUE) == TRUE ) // correct input
	{
		if ( m_strDCOMLogin.IsEmpty() )
		{
			CString strMsg;
			strMsg.LoadString( IDE_DCOM_LOGIN_EMPTY );
			AfxMessageBox(strMsg, MB_OK|MB_ICONERROR);
			CWnd* pLoginWnd = GetDlgItem(IDC_OCRCFG_DCOM_EDIT_LOGIN);
			if ( pLoginWnd ) pLoginWnd->SetFocus();
		}
		else
		{
			CSid loginSID;
			bool bLogin = loginSID.LoadAccount( m_strDCOMLogin );
			if ( bLogin )
			{
				OnThreadStarted();
			}
			else
			{
				CString strMsg;
				strMsg.LoadString( IDE_DCOM_LOGIN_NOT_EXISTS );
				AfxMessageBox(strMsg, MB_OK|MB_ICONERROR);
				CWnd* pLoginWnd = GetDlgItem(IDC_OCRCFG_DCOM_EDIT_LOGIN);
				if ( pLoginWnd ) pLoginWnd->SetFocus();
			}
		}
	}
	return -1; // stay at this page until install completes
}


// CDCOMPage implementation

WZRThread* CDCOMPage::ConstructThread() const
{
	return new CDCOMThread(GetSafeHwnd(), m_strDCOMLogin);
}

// CDCOMPage message handlers

LRESULT CDCOMPage::OnQuerySiblings(WPARAM wParam, LPARAM lParam)
{
	/*
	if (wParam == WM_OCRCFG_QUERY_DCOM)
	{
		::memcpy((void*) lParam, (void*) m_strThreadResults.GetString(), m_strThreadResults.GetLength() * sizeof(TCHAR));
		return 1; // answer
	}
	*/
	return 0; // ignore
}

LRESULT CDCOMPage::OnDCOMCfgStart(WPARAM wParam, LPARAM lParam)
{
	m_ctlProgress.SetRange(0, (short)lParam);
	m_ctlProgress.SetStep(1);
	m_ctlProgress.SetPos(0);
	m_strStage.LoadString(IDS_OCRCFG_DCOM_COMMON_PROCESS);
	ShowProgress();
	return 0;
}

LRESULT CDCOMPage::OnDCOMCfgStage(WPARAM wParam, LPARAM lParam)
{
	DWORD   dwNewPos   = (DWORD)wParam;
	DWORD dwPos = (DWORD)m_ctlProgress.GetPos();
	if (dwPos < dwNewPos)
	{
		m_ctlProgress.StepIt();
		if ( lParam != 0 )
		{
			DWORD dwMsgID = lParam;
			m_strStage.LoadString(dwMsgID);
		}
		ShowProgress();
	}
	return 0;
}

LRESULT CDCOMPage::OnDCOMCfgFinish(WPARAM wParam, LPARAM lParam)
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
		for (int i=0; i<m_pSheet->GetPageCount(); i++)
		{
			CPropertyPage* pPage = m_pSheet->GetPage(i);
			if ( pPage )
			{
				if ((LRESULT)pPage->m_psp.pszTemplate == IDD_OCRCFG_FINISH_PAGE )
				{
					m_pSheet->SetActivePage( i );
					break;
				}
			}
		}
	//	int iActivePage = m_pSheet->GetActiveIndex();
	//	m_pSheet->SetActivePage(iActivePage + 1);     
	}
	return 0;
}

