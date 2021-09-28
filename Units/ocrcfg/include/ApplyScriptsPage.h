//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Interface for thread for background operations
//--------------------------------------------------------------------------


#pragma once

#include "v/include/ImageList.h"
//#include "ocrcfg/include/CreateDBData.h"


class CCreateDBThread;

// CApplyScriptsPage dialog

class CApplyScriptsPage : public WZRPage
{
	//DECLARE_DYNAMIC(CApplyScriptsPage)
    // Dialog Data	
    //{{AFX_DATA(CApplyScriptsPage)  	
	CListBox				m_lbScripts;
    //}}AFX_DATA    
	
    // runtime params    
	// constant data
	bool					m_bError;
	COcrConfigData&			m_rConfigData;
	//CConfigXMLData&			m_rXmlConfigData;

	int* m_pnModificationFlags; //� ���� ������ ��������� ��������� ����� IsCreateDB. 
								//��. Constants.h ������ Configurator Modification Flags

public:
	CApplyScriptsPage(COcrConfigData& rConfigData, int* pnModificationFlags );
	virtual ~CApplyScriptsPage();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CApplyScriptsPage)
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

    LRESULT ApplyScripts();        

    // Generated message map functions
    //{{AFX_MSG(CApplyScriptsPage)      
	afx_msg LRESULT OnQuerySiblings(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnScriptExcecutionStart (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScriptExcecutionStage	(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScriptExcecutionFinish(WPARAM wParam, LPARAM lParam);
    //afx_msg LRESULT OnCustomScriptExcecutionStart(WPARAM wParam, LPARAM lParam);
	
	afx_msg void OnNamesChange();
	afx_msg void OnSkipBtnClick();
    //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
