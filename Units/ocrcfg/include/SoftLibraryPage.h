//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Interface for thread for background operations
//--------------------------------------------------------------------------

#pragma once


// CSoftLibraryPage dialog

class CSoftLibraryPage : public WZRPage
{
//	DECLARE_DYNAMIC(CSoftLibraryPage)
    // Dialog Data
    //{{AFX_DATA(CSoftLibraryPage)  	

	//}}AFX_DATA    
    // runtime params    


	// members
protected:
    CString			m_strSqlServer;
	CString			m_strDatabase;
	CComboBox		m_comboSqlServer;
	CEdit			m_edtDatabase;
	int				m_iProcedure;
	
	set<CString>	m_setXmlFileNames;	//Список файлов для импорта Библиотека ПО в -> БД
	CString			m_strXmlFile;		//Строка для отображения в GUI состояния m_setXmlFileNames
	
	CButton			m_btnExport;

public:
	CSoftLibraryPage();
	virtual ~CSoftLibraryPage();

    virtual BOOL OnInitDialog();
    virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
    virtual LRESULT OnWizardBack();
    virtual LRESULT OnWizardNext();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    virtual WZRThread* ConstructThread() const;

	afx_msg LRESULT OnQuerySiblings(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSoftLibraryProcStart (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSoftLibraryFileImportStart (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSoftLibraryProcStage (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSoftLibraryProcFinish(WPARAM wParam, LPARAM lParam);
	afx_msg void OnXmlPathChange();
	afx_msg void OnBrowse();

	DECLARE_MESSAGE_MAP()

	void CheckWizardNextButton();
	
protected:
	//Заполняет m_setXmlFileNames и m_strXmlFile
	void GetSoftLibraryFilePaths();


};
