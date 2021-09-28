//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Interface for thread for background operations
//--------------------------------------------------------------------------


#pragma once

#include "v/include/ImageList.h"



class COcrConfigData;

// CCredentialsPage dialog

class CCredentialsPage : public WZRPage
{
	//DECLARE_DYNAMIC(CCredentialsPage)
    // Dialog Data
    //{{AFX_DATA(CCredentialsPage)  	

	//Текущие значения
	
	CString				m_strLogin;
	CString				m_strPassword;

	//Предыдущие значения
	
	CEdit				m_edtLogin;
	CEdit				m_edtPassword;
	bool				m_bError;
	//set<CAtlString> m_p_set_networkServerName;
    //}}AFX_DATA    
    // runtime params    
    // constant data
	COcrConfigData&		m_rConfigData;


	//bool* m_pbIsCreateDB; //База создана новая или пересоздана под старым именем
						 //Передается в конструктор потока создания базы

	//int* m_pnModificationFlags; //В этом классе ожидается установка флага IsCreateDB. 
								//См. Constants.h раздел Configurator Modification Flags

public:
	CCredentialsPage(COcrConfigData& rConfigData);
	virtual ~CCredentialsPage();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCredentialsPage)
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
    
	HRESULT GrantLogOnAsServiceRight();
    LRESULT LogonUser();    
	CString GetCurrentUserName();
	   

    // Generated message map functions
    //{{AFX_MSG(CCredentialsPage)      
	
	afx_msg LRESULT OnQuerySiblings(WPARAM wParam, LPARAM lParam);
	/*
    afx_msg LRESULT OnScriptExcecutionStart (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScriptExcecutionStage	(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScriptExcecutionFinish(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCustomScriptExcecutionStart(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSQLComboChange();
	*/
	afx_msg void OnNamesChange();
	afx_msg void OnSelect();
	HRESULT ProcessResults(IDataObjectPtr& ptrDataObj, CString& strADPath);
	afx_msg void OnSkipBtnClick();
    //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
