#pragma once


// CWelcomePage dialog

class CWelcomePage : public WZRPage
{
	//DECLARE_DYNAMIC(CWelcomePage)

protected:
    CFont   m_fontTitle;        // bold font for welcome page title
	int		m_iUseScenario;
	CEdit	m_edtConfigFile;
	CButton m_btnBrowse;

	CString m_strConfigFile; // string for storing reportpack file name
	// the type of container is std::set because it will exclude repeat file name
	std::set<CString> m_setConfigFile; // container for storing strings contains reportpack file names

public:
	CWelcomePage();
	virtual ~CWelcomePage();

    // Overrides
public:
    virtual BOOL OnInitDialog();
    virtual BOOL OnSetActive();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT OnWizardNext();

protected:
	void CheckWizardNextButton();

	afx_msg void OnUseConfigFileButton();
	afx_msg void OnConfigPathChange();
	afx_msg void OnBrowse();

	DECLARE_MESSAGE_MAP()
};
