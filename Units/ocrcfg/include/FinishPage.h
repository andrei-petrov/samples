//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------

#pragma once

#include "ocrcfg\include\CommandLineInfo.h"

// CFinishPage dialog

class CFinishPage : public WZRPage
{
	//DECLARE_DYNAMIC(CFinishPage)
protected:
    // Dialog Data
    //{{AFX_DATA(CFinishPage)  
	CButton			m_btnUninstall;
    int             m_iUninstall;    
    CString         m_strInfo;
    //}}AFX_DATA
	CWnd*	m_pWndTitle;
    CFont   m_fontTitle;        // bold font for welcome page title

	/*bool* m_pbIsCreateDB;					 //База создана новая или пересоздан
	bool* m_pbIsModifiedSqlServerName;		 //Имя сервера изменено
	bool* m_pbIsModifiedDBName;				 //Имя базы изменено
	bool* m_pbIsModifiedReportServerName;	 //Адрес сервера отчетов изменен
	bool* m_pbIsModifiedReportsWebCatalog;	 //Имя каталога с отчетами изменено*/

	int* m_pnModificationFlags; ////Возможные значения см. в Constans.h в разделе Configurator Modification Flags

public:
	CFinishPage( int* pnModificationFlags /*bool* pbIsCreateDB, bool* pbIsModifiedSqlServerName, bool* pbIsModifiedDBName, bool* pbIsModifiedReportServerName, bool* pbIsModifiedReportsWebCatalog*/ );
	virtual ~CFinishPage();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CFinishPage)
    protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    public:
    virtual BOOL OnInitDialog();
    virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	
    //}}AFX_VIRTUAL

protected:

	void Uninstall();
	void RestartServer();
	void RefreshClient();

    // Implementation
//	DECLARE_MESSAGE_MAP()
};
