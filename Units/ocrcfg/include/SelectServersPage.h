#pragma once
#include "wzr/include/Page.h"

class CSelectServersPage :	public WZRPage
{
	enum SelSrvPageConditionEnum
	{
		SelSrvPageNone = 1, 
		SelSrvPageYES,
		SelSrvPageNO, 
		SelSrvPageSearch
	} m_enSelSrvPageDatabase, m_enSelSrvPageReportServer, m_enSelSrvPageWebFolder;

	enum SelSrvPageActionEnum
	{
		SelSrvPageDBAndRptSrvSearchStart = 1,
		SelSrvPageWebFolderSearchStart,
		SelSrvPageDataBaseSearchFinishFail,
		SelSrvPageDataBaseSearchFinishSuccess,
		SelSrvPageReportServerSearchFinishFail,
		SelSrvPageReportServerSearchFinishSuccess,
		SelSrvPageWebFolderSearchFinishFail,
		SelSrvPageWebFolderSearchFinishSuccess,	
	} m_enSelSrvPageAction;

	// Explanation
	CAtlString m_strExplanations;

	//Текущий выбор пользователя
	CString			m_strSQLServer;
	CString			m_strReportsServerURL;
	CString			m_strDatabase;
	CString			m_strReportsFolder;

	//Предыдущий выбор пользователя
	CString			m_strPrevSQLServer;
	CString			m_strPrevReportsServerURL;
	CString			m_strPrevDatabase;
	CString			m_strPrevReportsFolder;

	CComboBox		m_comboSQLServer;
	CComboBox		m_comboDatabase;
	CComboBox		m_comboReportsServerURL;
	CComboBox		m_comboReportsFolder;

	bool			m_bSaveRegistrySettings;
//	CWnd m_wndStatusText;
	UINT_PTR m_nCallbackTimer;
	CMutex			m_MutexSQLServerCombo;
	HANDLE m_hEventLock;
	set<CAtlString> m_setReportServerName;
	CString m_strSqlServerName;
	HANDLE m_hSQLSrvDBEventLock;
	set<CAtlString> m_setSQLSrvDataBase;
	HANDLE m_hWebFolderventLock;
	set<CAtlString> m_setWebFolder;
	bool m_bFirstLanchl; // indicator of first lanch

	int* m_pnModificationFlags; //В этом классе ожидается установка флагов IsModifiedSqlServerName, IsModifiedDBName, 
								//IsModifiedReportServerName, IsModifiedReportsWebCatalog. 
								//См. Constants.h раздел Configurator Modification Flags

public:
	CSelectServersPage( int* pnModificationFlags );
	virtual ~CSelectServersPage(void);

	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnQuerySiblings(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelectSQLServerComboChange();
	afx_msg void OnReportsServerComboChange();
	afx_msg void OnSQLServerEditChange();
	afx_msg void OnURLReportServerEditChange();
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	void StartTimer();
	void StopTimer();
	LRESULT OnReadNetworkReportServerFinish(WPARAM wParam, LPARAM lParam);
	void StartSearch();
	LRESULT OnReadSQLSrvDataBaseFinish(WPARAM wParam, LPARAM lParam);
	LRESULT OnReadWebFolderFinish(WPARAM wParam, LPARAM lParam);
	void SetStatus(SelSrvPageActionEnum Cond, CAtlString* strExplanations = NULL);
	void DisableControls();
	void EnableControls();
	void SetButtons();
	afx_msg void OnNamesChange();
	afx_msg void OnNamesEdit();

	DECLARE_MESSAGE_MAP()
};