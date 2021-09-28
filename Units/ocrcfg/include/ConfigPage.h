//--------------------------------------------------------------------------------
//  Copyright: (C) 2000-2005 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit: SamCfg
//  Cooperation: absent
//  Author:      Petrov Andrey
//  Date:        2008-04-01
//--------------------------------------------------------------------------------
//  Description: Base class
//--------------------------------------------------------------------------------

#pragma once


#include "wzr/include/Page.h"
typedef WZRPage CConfigPage;


class WZRThread;

// CConfigPage 
/*
class CConfigPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CConfigPage)

// Dialog Data
    //{{AFX_DATA(CConfigPage)         
    CString         m_strStage;			// инсталлирую то-то
    CProgressCtrl   m_ctlProgress;		
    CString         m_strStatus;		// такой-то компонент не инсталлирован
    CButton         m_btnSkipPage;		// пропустить эту страницу
    int             m_iSkipPage;
    CStatic         m_ctlIcon;			// маленькая икнока
	CWnd			m_wndStage;
	CWnd			m_wndStatus;		
	CWnd			m_wndToContinue;	

    //}}AFX_DATA
    bool            m_bBkgrndThreadActive;
    UINT            m_nIconID;
    CPropertySheet* m_pSheet;   // parent sheet
    CConfigThread*  m_pThread;
	CString			m_strThreadResults;

    DWORD           m_dwWizardButtonsFlagNormal;
    DWORD           m_dwWizardButtonsFlagInstall;

public:
	CConfigPage(UINT nIDTemplate, UINT nIDHeaderTitle = 0, UINT nIDHeaderSubTitle = 0);
	virtual ~CConfigPage();
	
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CConfigPage)
    public:
    virtual BOOL OnInitDialog();
    virtual BOOL OnQueryCancel();
	virtual BOOL OnSetActive();
    protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

protected:
    // Implementation
    virtual void EnableWindows() {}; 
    virtual void ShowData() {};      

    virtual CConfigThread* ConstructThread() const {return NULL;}

  	DWORD GetWizardButtonsFlags();
    void OnThreadStarted();
    void OnThreadFinished();

	void SetSkip(BOOL bSkip);
	void ShowProgress();

    // Generated message map functions
    //{{AFX_MSG(CConfigPage)  
    afx_msg void OnCheckSkip();
    afx_msg void OnUpdateCheckSkip( CCmdUI* pCmdUI );
    afx_msg void OnUpdateAllControls( CCmdUI* pCmdUI );
    //}}AFX_MSG

//	BOOL m_bLocalExpress;
//	BOOL m_bLocalUpgrade;
//	DWORD m_dwWizardButtonsFlag;

	DECLARE_MESSAGE_MAP()
};
*/