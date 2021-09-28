//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Interface for thread for background operations
//--------------------------------------------------------------------------


#pragma once

#include "v/include/ImageList.h"
#include "ocrcfg/include/RegJobsData.h"


class CRegJobsThread;

// CRegJobsPage dialog

class CRegJobsPage : public WZRPage
{
	//DECLARE_DYNAMIC(CRegJobsPage)
    // Dialog Data
    //{{AFX_DATA(CRegJobsPage)  	
	CCheckListBox				m_lbJobs;
    //}}AFX_DATA    
    // runtime params    
    // constant data

	bool						m_bError;
	CDeliceOcrJobDesc::Vector	m_vecJobs;	
	COcrConfigData&				m_rConfigData;

	int* m_pnModificationFlags; //В этом классе ожидается установка флага IsCreateDB. 
								//См. Constants.h раздел Configurator Modification Flags

public:
	CRegJobsPage(COcrConfigData& rConfigData, int* pnModificationFlags );
	virtual ~CRegJobsPage();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CRegJobsPage)
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

    LRESULT ProcessJobs();    

    // Generated message map functions
    //{{AFX_MSG(CRegJobsPage)      
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
