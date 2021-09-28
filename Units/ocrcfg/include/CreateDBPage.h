//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Interface for thread for background operations
//--------------------------------------------------------------------------


#pragma once

#include "v/include/ImageList.h"
#include "ocrcfg/include/CreateDBData.h"


class CCreateDBThread;

// CCreateDBPage dialog

class CCreateDBPage : public WZRPage
{
	//DECLARE_DYNAMIC(CCreateDBPage)
    // Dialog Data
    //{{AFX_DATA(CCreateDBPage)  	

	//Текущие значения
    CString			m_strSqlServer;
	CString			m_strDatabase;
	CString			m_strDatabase2;

	//Предыдущие значения
	//CString			m_strPrevSqlServer;
	//CString			m_strPrevDatabase;
	
	// 
	// variable for sql server name
	// The name which user entered may be diffrent with name for connection to data base in two cases
	// 1. User used "." (dot) symbol for localhost server. The name "localhost" will be used to 
	//		connect to data base and for report server (and same name will be written to registry). 
	//		Because it can't access the report server with "." host name. Only with "localhost".
	// 2. User uses IP adress with port. Example "10.4.0.55:1444/SQLEXPRESS". It will be convert to
	//		"10.4.0.55/SQLEXPRESS,1444". Because sql accepts second variant.
	//CString			m_strSqlServerHostName; // the name is used for connection to data base
	//CString			m_strSqlServerСompName; // the name is used for connection to computer
	
	//CButton			m_btnSQLSearch;
	CComboBox		m_comboSqlServer;
	CEdit			m_edtDatabase;
	CEdit			m_edtDatabase2;
    //}}AFX_DATA    
    // runtime params    
    // constant data
	bool			m_bError;
	set<CAtlString> m_p_set_networkServerName;

	CDBDescription::Vector	m_vecDatabases;
	COcrConfigData&			m_rConfigData;

	int* m_pnModificationFlags; //В этом классе ожидается установка флага IsCreateDB. 
								//См. Constants.h раздел Configurator Modification Flags

public:
	CCreateDBPage(COcrConfigData& rConfigData, int* pnModificationFlags );
	virtual ~CCreateDBPage();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCreateDBPage)
    public:
    virtual BOOL OnInitDialog();
    virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual LRESULT OnWizardBack();
    virtual LRESULT OnWizardNext();
	void SetButtons();
    protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

protected:
    // Implementation
    virtual WZRThread* ConstructThread() const;

    LRESULT ProcessDB();    

    //bool    CheckInstalledComponents(bool& bAllComponentsInstalled);

    // Generated message map functions
    //{{AFX_MSG(CCreateDBPage)      
	afx_msg LRESULT OnQuerySiblings(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnScriptExcecutionStart (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScriptExcecutionStage	(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScriptExcecutionFinish(WPARAM wParam, LPARAM lParam);
    //afx_msg LRESULT OnCustomScriptExcecutionStart(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSQLComboChange();
	afx_msg void OnNamesChange();
	afx_msg void OnSkipBtnClick();
    //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
