//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Exceptions
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Message dialog with detailed error reporting, skip checkbox 
//               and help capabilities, idea taken from CodeGuru and refactored
//--------------------------------------------------------------------------------

#pragma once
#include "T/include/Constants.h"
#include "T/include/Misc.h"
#include "T/include/T.h"

const  int ciMaxButtons = 6;
extern T_DATA LPCTSTR cszPermanentMessagesKeyName;

typedef CArray <CStatic*, CStatic*&> TArrayLPStatic;
struct  THelpInfo;

/////////////////////////////////////////////////////////////////////////
//
//  TMessageBox
//

T_FUNC UINT T_API TCheckMessageBox(LPCTSTR szMessage,UINT nType, 
								UINT nIDHelp=0, LPCTSTR szCheckbox=NULL, 
								PBOOL pbCheckState=NULL);


class T_CLASS TMessageBox : public CDialog
{
protected:
// Dialog Data
	enum { IDD = IDD_T_MESSAGE_BOX };
	CStatic			m_icon;
	CButton			m_buttons[ciMaxButtons];
	CButton			m_checkbox;
	BOOL			m_bCheckState;
	TSettings*		m_pSettings;

// General data
	UINT			m_nButtonType;
	UINT			m_nIconType;
	UINT			m_nDefButton;
	BOOL			m_bHelp;				// help button present
	BOOL			m_bCopyToClipboard;		// copy to clipboard present
	BOOL			m_bCheckPresent;		// checkbox present
	BOOL			m_bRunErrorReporter;
	CString			m_strCheckText;
	CStringArray	m_arrMessages;
	CDWordArray		m_arrHelpIDs;
	CString			m_strSectionName;
	CString			m_strKeyName;
	int				m_bSavedValue;
	int				m_nType;
	int				m_nMessageID;

// displayed information, runtime data
//	BOOL			m_bArrCreated;
	TArrayLPStatic	m_arrStaticControls;
//	THelpInfo*		m_arrHelpInfos;
	DWORD			m_arrButtonIDs[ciMaxButtons];
	DWORD			m_arrButtonNames[ciMaxButtons];
	UINT			m_nButtons;

// geometrical information
	CSize			m_sizeWnd;
	TRectArray		m_arrStaticRects;		// rectangles of static controls
	TRectArray		m_arrButtonRects;		// rectangles of buttons
	CRect			m_rcIcon;    
	CRect			m_rcCheck;

// etalones for controls aranging
	int				m_nBorderX;
	int				m_nBorderY;
	int				m_nContolsIntervalX;
	int				m_nContolsIntervalY;

	int				m_nRelatedContolsIntervalX;
	int				m_nRelatedContolsIntervalY;

	int				m_nButtonsHeight;
	int				m_nButtonsWidth;
//  font management
	CFont			m_font;
//  silence management
	static bool		ms_bSilent; // do not show message box in the screen


public:
	TMessageBox( const CStringArray& arrMessages, const CDWordArray& arrHelpIDs, UINT nType,
					const CString& strCheckBoxText = CString(), BOOL bCheckInitialState = FALSE , 
					const CString& strSectionName = CString(), int nMessageID = NULL/*LPCTSTR szKeyName = NULL*/, TSettings* pSettings = NULL);
	~TMessageBox();

// Overrides
public:
	virtual INT_PTR DoModal();
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL PreTranslateMessage( MSG* pMsg );

//  Operations
public:
	void EndDialog(int nResult);
	BOOL GetCheckState()	{ return m_bCheckState; };
	static void SetSilentMode(bool bSilent) { ms_bSilent = bSilent; }

// Implementation
protected:
// separating long strings by '\n'
	void  SeparateString(CString& strSubj, UINT nMaxLength);
// caclulations control positions
	void  CalcEtalones(void);
	CSize CalcStaticSize(LPCTSTR szText);
	void  CalcControlPositions();
// setting controls on their positions
	void  SetupFont();
	void  SetButtonInfo();
	void  SetupButton(CButton& rButton, UINT nCommand, UINT nIDText, CRect rect);
	void  SetupButtons();
	void  SetupTitleAndIcon();
	void  SetupTextControls();
	void  SetupAllControls();
//    bool  FindIDInSetting(CString& rstrSettings, LPCTSTR szKeyName);

// message map
protected:
	// messages
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	// commands
	afx_msg void OnButton(UINT nID);
	afx_msg void OnEditCopy();
	afx_msg void OnHelp();
	afx_msg void OnRunReporter();

	DECLARE_MESSAGE_MAP()
};
