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

// CCreateFolderPage dialog

class CCreateFolderPage : public WZRPage
{
	//DECLARE_DYNAMIC(CCreateFolderPage)
    // Dialog Data
    //{{AFX_DATA(CCreateFolderPage)  	

	//������� ��������	
	CString			m_strProjectFolder;
	CString			m_strScansFolder;

	//���������� ��������	
	CEdit			m_edtProjectFolder;
	CEdit			m_edtScansFolder;
	bool			m_bError;
	//set<CAtlString> m_p_set_networkServerName;
    //}}AFX_DATA    
    // runtime params    
    // constant data
	COcrConfigData&	m_rConfigData;


	//bool* m_pbIsCreateDB; //���� ������� ����� ��� ����������� ��� ������ ������
						 //���������� � ����������� ������ �������� ����

	//int* m_pnModificationFlags; //� ���� ������ ��������� ��������� ����� IsCreateDB. 
								//��. Constants.h ������ Configurator Modification Flags

public:
	CCreateFolderPage(COcrConfigData& rConfigData);
	virtual ~CCreateFolderPage();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCreateFolderPage)
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
    
	BOOL	OpenDirectory(CString& strSelectedDirectory, LPCTSTR pszTitle = NULL);
	HRESULT ShareFolder(LPCTSTR pszFolder, LPCTSTR pszNetworkName, LPCTSTR pszRemark);
    LRESULT CreateProjectSubFolders();    
	   

    // Generated message map functions
    //{{AFX_MSG(CCreateFolderPage)    
	afx_msg LRESULT OnQuerySiblings(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBrowseProject();
	afx_msg void OnBrowseScans();
	afx_msg void OnNamesChange();
	afx_msg void OnSkipBtnClick();
    //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
