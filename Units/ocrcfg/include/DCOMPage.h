#pragma once


// CDCOMPage dialog

class CDCOMPage : public WZRPage
{
	//DECLARE_DYNAMIC(CDCOMPage)
    // Dialog Data
    //{{AFX_DATA(CDCOMPage)  	

	//}}AFX_DATA    
    // runtime params    


public:
	CDCOMPage();
	virtual ~CDCOMPage();

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
    afx_msg LRESULT OnDCOMCfgStart (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDCOMCfgStage (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDCOMCfgFinish(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

// members
	CString	m_strDCOMLogin;
};
