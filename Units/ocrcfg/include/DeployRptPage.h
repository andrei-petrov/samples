//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Interface for thread for background operations
//--------------------------------------------------------------------------


#pragma once

#include "v/include/ImageList.h"

#include "ocrcfg/include/ConfigXMLData.h"


//const TCHAR cszSQLRoot[]			= _T("SOFTWARE\\Microsoft\\Microsoft SQL Server\\MSSQL.2\\Setup");
//const TCHAR	cszSRSVirtualFolder[]	= _T("RSVirtualRootServer");


class CDeployRptThread;

// CDeployRptPage dialog

class CDeployRptPage : public WZRPage
{
	//DECLARE_DYNAMIC(CDeployRptPage)
    // Dialog Data
    //{{AFX_DATA(CDeployRptPage)  	
	CString			m_strXMLBody;
	//CString			m_strSQLServer;
	CString			m_strDbName;
	
	//Текущие значения
	CString			m_strReportsServerURL;
	CString			m_strReportsWebFolder;
	//Предыдущие значения
	CString			m_strPrevReportsServerURL;
	CString			m_strPrevReportsWebFolder;

	CString			m_strReportsDir;
	CString			m_strReportsDir2;
	CConfigXMLData::ReportInfo::Vector m_arrReportsInfo;
	CComboBox		m_comboReportsServerURL;
	CEdit			m_edtReportsWebFolder;
	std::set<CString> *m_setXMLBody;
	CMutex			m_Mutex;
    // безобразно написанные члены-класса, дублирующие то, что есть в классе-предке
	HANDLE m_hEventLock;
	set<CAtlString> m_setReportServerName;
	//CAtlString m_strSQLServerUserEnterName; // user can write IP adress as host name
	CAtlString m_strSQLServerHostName;
	//CString m_strStatusText;
	//CStatic m_stcIcon;
	UINT_PTR m_nCallbackTimer;
	//CWnd m_wndStatusText;
	bool bUnattendedMode;
	VImageList oImageList;

	//}}AFX_DATA    
    // runtime params    
    // constant data

	// показывает namespace в WMI для URL сервера отчетов
	std::map<CString, CString>	m_mapNamespaces;

	/*bool* m_pbIsModifiedReportServerName;
	bool* m_pbIsModifiedReportsWebCatalog;  */

	int* m_pnModificationFlags; //В этом классе ожидается установка флагов IsModifiedReportServerName, 
								//IsModifiedReportsWebCatalog, IsInstallReports. 
								//См. Constants.h раздел Configurator Modification Flags
		

public:
	CDeployRptPage( int* pnModificationFlags );
	virtual ~CDeployRptPage();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDeployRptPage)
    public:
    virtual BOOL OnInitDialog();
    virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
    virtual LRESULT OnWizardBack();
    virtual LRESULT OnWizardNext();
    protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

protected:
    // Implementation
    virtual WZRThread* ConstructThread() const;

    LRESULT DeployReports();    

    bool    CheckInstalledComponents(bool& bAllComponentsInstalled);

    // Generated message map functions
    //{{AFX_MSG(CDeployRptPage)      
	afx_msg LRESULT OnQuerySiblings(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDeployRptStart (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeployRptStage (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeployRptFinish(WPARAM wParam, LPARAM lParam);
	afx_msg void OnReportServerComboChange();
	LRESULT OnReadNetworkReportServerFinish(WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	void StartTimer();
	void StopTimer();
	afx_msg void OnNamesChange();
	void SetButtons();
    //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
