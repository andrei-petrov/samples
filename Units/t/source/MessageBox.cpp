//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Exceptions
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Message dialog with detailed error reporting, skip checkbox 
//               and help capabilities
//--------------------------------------------------------------------------------

#include "stdafx.h"
#include <afxpriv.h>

#include "CMN/include/Misc.h"

//#include "T/Include/DisabledMessagesData.h"
#include "T/include/Constants.h"
#include "T/include/HelpInfo.h"
#include "T/include/Exception.h"
#include "T/include/Misc.h"
#include "T/include/WinApp.h"
#include "T/include/MessageBox.h"
#include "T/include/WaitCursor.h"

#define APSTUDIO_INVOKED
#include "T/resource/resource.h"		// I want to access _APS_NEXT_CONTROL_VALUE
#undef  APSTUDIO_INVOKED

#define BORDER_OFFSET			7

#define CONTROLS_INTERVAL_X		7	// there is only one X interval (between buttons)
#define CONTROLS_INTERVAL_Y		7

#define RELATED_CONTROLS_INTERVAL_X		4	// there is only one X interval (between buttons)
#define RELATED_CONTROLS_INTERVAL_Y		4

#define BUTTON_HEIGHT			14
#define BUTTON_WIDTH			60

#define MAX_STRING_LENGTH		60


//CFont TMessageBox::ms_font;
bool  TMessageBox::ms_bSilent = false;

/////////////////////////////////////////////////////////////////////////
//
//                          class TMessageBox
//

TMessageBox::TMessageBox(const CStringArray& arrMessages, const CDWordArray& arrHelpIDs, UINT nType,
							const CString& strCheckBoxText, BOOL bCheckInitialState,
							const CString& strSectionName, /*LPCTSTR szKeyName,*/  int nMessageID, TSettings* pSettings ) :
	CDialog(IDD, NULL ),
	m_bSavedValue(0)
{
	ASSERT ( arrMessages.GetSize() == arrHelpIDs.GetSize() );
	ASSERT ( arrMessages.GetSize() != 0 );
	m_pSettings = pSettings;

	m_arrMessages.Copy(arrMessages);
	m_arrHelpIDs.Copy (arrHelpIDs );
	m_nType = nType;

	m_nButtonType = nType & MB_TYPEMASK;	// 0x0000000FL

	//  The message box cannot have both MB_SAVECHECKSTATE style OK, Cancel Buttons only
	if (nType & MB_SAVECHECKSTATE)
		ASSERT(m_nButtonType == MB_OK || m_nButtonType == MB_OKCANCEL || m_nButtonType == MB_YESNO);

	m_nIconType = nType & MB_ICONMASK;     // 0x000000F0L
	m_nDefButton = (nType & MB_DEFMASK) >> 8; // 0x00000F00L
	// runtime parameters
	if(!strCheckBoxText.IsEmpty())
		m_strCheckText = strCheckBoxText;

	m_bCheckState = bCheckInitialState;
	m_bCheckPresent = nType & MB_CHECK;
	m_bHelp = nType & MB_HELP;
	m_bCopyToClipboard = nType & MB_COPYTOCLIPBOARD;
	m_bRunErrorReporter = nType & MB_RUNREPORTER;

	if (!strSectionName.IsEmpty() && nMessageID && m_pSettings != NULL)
	{
		m_nMessageID = nMessageID;
		if(nType & MB_SAVECHECKSTATE)
			m_strSectionName = cszPermanentMessagesSection;
		if(nType & MB_SAVECHECKSTATETMP)
			m_strSectionName = cszTemporaryMessagesSection;
		/*
		if (m_strSectionName.GetLength() && (nType & (MB_SAVECHECKSTATE | MB_SAVECHECKSTATETMP))) // if no check box show message anyway
		{
			TDisabledMessagesData::Ptr spDisabledMessages = m_pSettings->GetObject(m_strSectionName, cszMessagesDisabledKey);
			if (spDisabledMessages == NULL)
				spDisabledMessages = new TDisabledMessagesData; //create default data
					m_bSavedValue = spDisabledMessages->IsMessageDisabled( nMessageID );
		}
		*/
	}
}

TMessageBox::~TMessageBox()
{
	for(int i=0; i < m_arrStaticControls.GetSize(); i++)
		delete m_arrStaticControls.GetAt(i);
}

BEGIN_MESSAGE_MAP(TMessageBox, CDialog)
	ON_WM_SETCURSOR()
	ON_WM_HELPINFO()
	ON_COMMAND_RANGE(IDOK, IDCLOSE, OnButton)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_T_RUNREPORTER, OnRunReporter)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////
//  TMessageBox message virtuals

INT_PTR TMessageBox::DoModal()
{
	if(ms_bSilent)
		return -1; // failed to create

	if(m_bSavedValue)
		return m_bSavedValue;

	TWinAppBase* pApp = dynamic_cast<TWinAppBase*>(AfxGetApp());
	ASSERT(pApp);

	// disable windows for modal dialog
//    if (pApp)
//	    pApp->EnableModeless(FALSE);
	CWnd* pSafeOwner = CWnd::GetSafeOwner(NULL, NULL);
	BOOL bEnabled = FALSE;
	if (pSafeOwner)
		bEnabled = pSafeOwner->IsWindowEnabled();

	DWORD dwOldContext = 0;
	if(pApp)
	{
		dwOldContext = *pApp->GetPromptContext();
		*pApp->GetPromptContext() = m_arrHelpIDs[0];
	}
	else
		TRACE(_T("[TMessageBox::DoModal] running without TWinApp object created. Help contexts will not work\n"));

	INT_PTR iCode = CDialog::DoModal();
	if(pApp)
		*pApp->GetPromptContext() = dwOldContext;
	if(pSafeOwner && !bEnabled)	// we should restore WS_DISABLED style of an owner if it was set before
		pSafeOwner->EnableWindow(FALSE);

//	if (pApp)
//		pApp->EnableModeless(TRUE);

	return iCode;
}

BOOL TMessageBox::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_bHelp)
		ModifyStyleEx(0,WS_EX_CONTEXTHELP);
	else
		ModifyStyleEx(WS_EX_CONTEXTHELP,0);

	CalcEtalones();
	SetupFont();

	for(int i = 0; i < ciMaxButtons; i++)
		m_buttons[i].ShowWindow(SW_HIDE);

	m_checkbox.ShowWindow(SW_HIDE);

	CalcControlPositions();
	SetupAllControls();

	// here calculates the difference between window rect size and client window rect size
	CRect rcClientRect;
	CRect rcWindowRect;
	GetClientRect( rcClientRect );
	GetWindowRect( rcWindowRect );

	// calculates the difference
	int iDeltaX = rcWindowRect.Width() - rcClientRect.Width();
	int iDeltaY = rcWindowRect.Height() - rcClientRect.Height();

	// sets new window size according to the difference
	rcWindowRect.bottom = rcWindowRect.top + iDeltaY + m_sizeWnd.cy;
	rcWindowRect.right = rcWindowRect.left + iDeltaX + m_sizeWnd.cx;
	MoveWindow( rcWindowRect );

	if(m_nButtonType != MB_OK && m_nButtonType != MB_OKCANCEL 
		&& m_nButtonType != MB_YESNOCANCEL && m_nButtonType != MB_RETRYCANCEL)
	{
		CMenu* pSysMenu = GetSystemMenu(FALSE);
		ASSERT(pSysMenu);
		if(pSysMenu)
			pSysMenu->EnableMenuItem (SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
	}
	// calculate small height

	return FALSE;//TRUE;
}


void TMessageBox::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MESSAGEBOX_ICON, m_icon);

	for(int i = 0; i < ciMaxButtons; i++)
		DDX_Control(pDX, IDC_MESSAGEBOX_BUTTON0 + i, m_buttons[i]);

	DDX_Control(pDX, IDC_MESSAGEBOX_CHECK, m_checkbox);
	DDX_Check(pDX, IDC_MESSAGEBOX_CHECK, m_bCheckState);
}

BOOL TMessageBox::PreTranslateMessage(MSG* pMsg)
{
	if((pMsg->message == WM_KEYDOWN ) && (GetAsyncKeyState(VK_CONTROL) & 0x8000) && 
			(pMsg->wParam == TCHAR('C')))
	{
		switch(pMsg->wParam)
		{
			case TCHAR('C'):
				OnEditCopy();
				break;
		}

		return TRUE; // message translated
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void TMessageBox::EndDialog(int nResult)
{
	UpdateData();
	/*
	if (m_nMessageID && GetCheckState() && (nResult==IDOK || nResult==IDYES || nResult==IDNO || nResult==IDIGNORE))
	{
		if ( m_strKeyName && m_strSectionName && m_bCheckPresent && m_pSettings != NULL)
		{   
			TDisabledMessagesData::Ptr spDisabledMessages = m_pSettings->GetObject(m_strSectionName, cszMessagesDisabledKey);
			if (spDisabledMessages == NULL)
				spDisabledMessages = new TDisabledMessagesData; //create default data	    
			spDisabledMessages->DisableMessage(m_nMessageID, nResult);
			m_pSettings->WriteObject(m_strSectionName, cszMessagesDisabledKey, (TObject::Ptr)spDisabledMessages);            
		}
	}
	*/
	CDialog::EndDialog(nResult);
}

//                              Implementation

void TMessageBox::SeparateString(CString& strSubj, UINT nMaxLength)
{
	CString strNew;
	int nPos;
	do
	{
		CString strCurrent;
		nPos = strSubj.Find(_T('\n'));
		if( nPos < 0 ) // \n not found
		{
			nPos = strSubj.GetLength();
			strCurrent = strSubj;
			strSubj.Empty();
		}
		else
		{
			strCurrent = strSubj.Left(nPos);
			if (nPos > 0) // not a leading \n
			{
				if (strCurrent[nPos-1] == _T('\r')) // \r present before \n
					strCurrent.SetAt(nPos-1, _T(' ')); // delete it!
			}
			strSubj = strSubj.Right(strSubj.GetLength() - nPos - 1);
		}
		if((UINT)strCurrent.GetLength() > nMaxLength - 2)	// leave two positions for \r\n after leaving cycle
		{
			do
			{
				CString strCandidate = strCurrent.Left(nMaxLength-1); // leave space for \r
				int nSpacePos = strCandidate.ReverseFind(_T(' ')); // this space will be user by \n
				if(nSpacePos > 0)
				{
					strNew += strCurrent.Left(nSpacePos) + _T("\r\n");
					strCurrent = strCurrent.Right(strCurrent.GetLength() - nSpacePos - 1); // excluding one leading ' '
				}
				else
				{
					strNew += strCurrent.Left(nMaxLength - 2) + _T("\r\n"); // leave space for \r\n
					strCurrent = strCurrent.Right(strCurrent.GetLength() - (nMaxLength - 2));
				}
			}
			while((UINT)strCurrent.GetLength() > nMaxLength-2); // leave two positions for \r\n after leaving cycle
		}
		strNew += strCurrent + _T("\r\n");
	}
	while( strSubj.GetLength() > 0 );
	strSubj = strNew;
}

void TMessageBox::CalcEtalones(void)
{
	RECT rect;

	rect.top = 0;
	rect.bottom = BUTTON_HEIGHT;
	rect.left = 0;
	rect.right = BUTTON_WIDTH;
	MapDialogRect(&rect);
	m_nButtonsWidth = rect.right;
	m_nButtonsHeight = rect.bottom;

	rect.bottom = BORDER_OFFSET;
	rect.right = BORDER_OFFSET;
	MapDialogRect(&rect);
	m_nBorderX = rect.right;
	m_nBorderY = rect.bottom;

	rect.bottom = CONTROLS_INTERVAL_Y;
	rect.right = CONTROLS_INTERVAL_X;
	MapDialogRect(&rect);
	m_nContolsIntervalX = rect.right;
	m_nContolsIntervalY = rect.bottom;

	rect.bottom = RELATED_CONTROLS_INTERVAL_Y;
	rect.right = RELATED_CONTROLS_INTERVAL_X;
	MapDialogRect(&rect);
	m_nRelatedContolsIntervalX = rect.right;
	m_nRelatedContolsIntervalY = rect.bottom;
}

// modified by Vladimir Lapshin to fix incorrect text string length bug, 14.02.2002
CSize TMessageBox::CalcStaticSize(LPCTSTR szText)
{
	CString strText = szText;
	int nX = 0, nY = 0;
	CDC* pDC = GetDC();

	pDC->SelectObject(&m_font);

	CSize size;
	for(int nPos; (nPos = strText.Find(_T('\n'))) >= 0; nY += size.cy)
	{
		CString strSub = strText.Left( nPos );
		if(strSub.IsEmpty() && !strText.IsEmpty())
			strSub = _T("\n"); // does not matter

		strText = strText.Right(strText.GetLength() - nPos - 1);

		size = pDC->GetOutputTextExtent(strSub);

		if(nX < size.cx)
			nX = size.cx;
	}

	size = pDC->GetOutputTextExtent(strText);

	if(nX < size.cx)
		nX = size.cx;

	nY += size.cy;

	ReleaseDC(pDC);

	size.cx = nX;
	size.cy = nY;

	return size;
}


void TMessageBox::CalcControlPositions()
{
	CSize sizeIcon;
	CSize sizeWnd;
	sizeIcon.cx = GetSystemMetrics(SM_CXICON);
	sizeIcon.cy = GetSystemMetrics(SM_CYICON);
	CRect rcTextPos;
	rcTextPos.left = m_nBorderX + sizeIcon.cx + m_nContolsIntervalX;
	rcTextPos.top = m_nBorderY;
	rcTextPos.bottom = m_nBorderY;
	rcTextPos.right = rcTextPos.left;
	m_arrStaticRects.RemoveAll();
	for(int i=0; i<m_arrMessages.GetSize(); i++)
	{
		CString strBuffer = m_arrMessages.GetAt(i);
		SeparateString(strBuffer, MAX_STRING_LENGTH);
		m_arrMessages.SetAt(i,strBuffer);
		CSize sizeText = CalcStaticSize(strBuffer);
		CRect rcText;
		rcText.left   = rcTextPos.left;
		rcText.top    = rcTextPos.bottom;
		rcText.bottom = rcText.top + sizeText.cy;
		rcText.right  = rcTextPos.left + sizeText.cx;

		m_arrStaticRects.Add(rcText);

		rcTextPos.bottom = rcText.bottom+m_nRelatedContolsIntervalY;
		if(rcTextPos.right < rcText.right)
			rcTextPos.right = rcText.right;
	}

	sizeWnd.cx = rcTextPos.right + m_nBorderX;

	if(rcTextPos.Height() < sizeIcon.cy + m_nRelatedContolsIntervalY)
		rcTextPos.bottom = m_nBorderY + sizeIcon.cy + m_nRelatedContolsIntervalY;

	m_rcIcon.left = m_nBorderX;
	m_rcIcon.top = (rcTextPos.Height() - sizeIcon.cy - m_nRelatedContolsIntervalY) / 2 + m_nBorderY;
	m_rcIcon.right = m_rcIcon.left + sizeIcon.cx;
	m_rcIcon.bottom = m_rcIcon.top + sizeIcon.cy;
	CPoint buttonPos;
	buttonPos.y = rcTextPos.bottom + m_nRelatedContolsIntervalY;

	if(m_bCheckPresent)
	{
		CRect checkRect;
		m_checkbox.GetWindowRect(checkRect);
		CSize sizeCheck = CalcStaticSize(m_strCheckText);
		checkRect.right = checkRect.left + checkRect.Height()*2 + sizeCheck.cx;
		buttonPos.y += checkRect.Height() + m_nRelatedContolsIntervalY * 2;

		if(checkRect.Width() + m_nBorderX * 2 > sizeWnd.cx)
			sizeWnd.cx = checkRect.Width() + m_nBorderX * 3;

		m_rcCheck.top  = rcTextPos.bottom + m_nRelatedContolsIntervalY;
		m_rcCheck.left = rcTextPos.left; //m_nBorderX;
		m_rcCheck.bottom = rcTextPos.bottom + checkRect.Height() + m_nRelatedContolsIntervalY;
		m_rcCheck.right=checkRect.Width() + rcTextPos.left;
		if (sizeWnd.cx < m_rcCheck.right + m_nBorderX)
			sizeWnd.cx = m_rcCheck.right + m_nBorderX;
	}
	SetButtonInfo();
	if (sizeWnd.cx < (int)m_nButtons * m_nButtonsWidth + ((int) m_nButtons - 1) * m_nRelatedContolsIntervalX + m_nBorderX * 2)
		sizeWnd.cx = m_nButtons * m_nButtonsWidth + (m_nButtons - 1)*m_nRelatedContolsIntervalX + m_nBorderX * 2;
	buttonPos.x = (sizeWnd.cx - m_nButtons * m_nButtonsWidth - (m_nButtons - 1) * m_nRelatedContolsIntervalX - m_nBorderX * 2) / 2
		+ m_nBorderX;

	for(int i=0; i<(int)m_nButtons; i++)
	{
		CRect buttonRect;
		buttonRect.left = buttonPos.x;
		buttonRect.top = buttonPos.y;
		buttonRect.right = buttonPos.x+m_nButtonsWidth;
		buttonRect.bottom = buttonPos.y+m_nButtonsHeight;
		m_arrButtonRects.Add(buttonRect);
		buttonPos.x += m_nButtonsWidth+m_nRelatedContolsIntervalX;
	}

	m_sizeWnd.cx = sizeWnd.cx;
	m_sizeWnd.cy = buttonPos.y + m_nButtonsHeight + m_nBorderY;
}

void TMessageBox::SetupFont()
{
	TWinAppBase* pApp = dynamic_cast<TWinAppBase*>(AfxGetApp());
	ASSERT(pApp);
	if( pApp )
	{
		LOGFONT& lf = pApp->GetLogFont();
		if ( lstrlen(lf.lfFaceName) != 0 ) // if we have a global font setting we should use it
		{
			VERIFY( m_font.CreateFontIndirect(&lf) );
		}
	}

	if( m_font.GetSafeHandle() == 0 ) // global font for entire application was not set up
	{
		CFont* pFont = GetFont(); // get dialog font
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		VERIFY( m_font.CreateFontIndirect(&lf) );
	}
}

void TMessageBox::SetButtonInfo()
{
	UINT nButton=0;
	if(m_nButtonType == MB_OK || m_nButtonType == MB_OKCANCEL)
	{
		m_arrButtonNames[nButton] = IDS_T_OK;
		m_arrButtonIDs[nButton++] = IDOK;
	}

	if(m_nButtonType == MB_RETRYCANCEL) 
	{
		m_arrButtonNames[nButton] = IDS_T_RETRY;
		m_arrButtonIDs[nButton++] = IDRETRY;
	}

	if(m_nButtonType == MB_OKCANCEL || m_nButtonType == MB_RETRYCANCEL) 
	{
		m_arrButtonNames[nButton] = IDS_T_CANCEL;
		m_arrButtonIDs[nButton++] = IDCANCEL;
	}

	if(m_nButtonType == MB_YESNO || m_nButtonType == MB_YESNOCANCEL)
	{
		m_arrButtonNames[nButton] = IDS_T_YES;
		m_arrButtonIDs[nButton++] = IDYES;

		m_arrButtonNames[nButton] = IDS_T_NO;
		m_arrButtonIDs[nButton++] = IDNO;
	}

	if(m_nButtonType == MB_YESNOCANCEL)
	{
		m_arrButtonNames[nButton] = IDS_T_CANCEL;
		m_arrButtonIDs[nButton++] = IDCANCEL;
	}

	if(m_nButtonType == MB_ABORTRETRYIGNORE)
	{
		m_arrButtonNames[nButton] = IDS_T_ABORT;
		m_arrButtonIDs[nButton++] = IDABORT;

		m_arrButtonNames[nButton] = IDS_T_RETRY;
		m_arrButtonIDs[nButton++] = IDRETRY;

		m_arrButtonNames[nButton] = IDS_T_IGNORE;
		m_arrButtonIDs[nButton++] = IDIGNORE;
	}

	if(m_bHelp)
	{
		m_arrButtonNames[nButton] = IDS_T_HELP;
		m_arrButtonIDs[nButton++] = ID_HELP;
	}

	if(m_bRunErrorReporter)
	{
		m_arrButtonNames[nButton] = IDS_T_RUNREPORTER;
		m_arrButtonIDs[nButton++] = ID_T_RUNREPORTER;
	}

	m_nButtons = nButton;
}


void TMessageBox::SetupAllControls()
{
	// Setting up static text
	for(int i=0; i<m_arrMessages.GetSize(); i++)
	{
		CStatic* pNewStatic = new CStatic();
		BOOL bRes = pNewStatic->Create(m_arrMessages.GetAt(i), SS_LEFTNOWORDWRAP | SS_NOPREFIX, 
										m_arrStaticRects.GetAt(i), this, _APS_NEXT_CONTROL_VALUE + i);
		pNewStatic->SetWindowContextHelpId(m_arrHelpIDs[i]);
		if(bRes)
		{
			pNewStatic->SetFont( &m_font );
			pNewStatic->ShowWindow(SW_SHOW);
			m_arrStaticControls.Add(pNewStatic);
		}
		else
			delete pNewStatic;
	}

	// Setting up buttons
	for(int i=0; i < (int)m_nButtons; i++)
	{
		CString strButtonName;
		m_buttons[i].MoveWindow(m_arrButtonRects.GetAt(i));
		m_buttons[i].SetDlgCtrlID(m_arrButtonIDs[i]);
		strButtonName.LoadString(m_arrButtonNames[i]);
		m_buttons[i].SetWindowText(strButtonName);

		if(i == (int)m_nDefButton) 
		{ 
			m_buttons[i].SetFocus(); 
			m_buttons[i].SetButtonStyle(BS_DEFPUSHBUTTON);
		}
		else 
			m_buttons[i].SetButtonStyle(BS_PUSHBUTTON);

		m_buttons[i].ShowWindow(SW_SHOW);
	}

	if(m_bCheckPresent)
	{
		m_checkbox.SetWindowText(m_strCheckText);
		m_checkbox.MoveWindow(m_rcCheck);
		m_checkbox.ShowWindow(SW_SHOW);
	}

	SetupTitleAndIcon();
}


void TMessageBox::SetupTitleAndIcon()
{
	HICON hIcon = NULL;
	CString strTitle;
	CString strAppName;
	LPCTSTR strIcon = NULL;
	UINT nTitleID = 0;
	switch(m_nIconType)
	{
		case MB_ICONHAND:
			strIcon = IDI_HAND;
			nTitleID = IDS_T_ERROR;
			break;
		case MB_ICONQUESTION:
			strIcon = IDI_QUESTION;
			nTitleID = IDS_T_QUESTION;
			break;
		case MB_ICONEXCLAMATION:
			strIcon = IDI_EXCLAMATION;
			nTitleID = IDS_T_WARNING;
			break;
		case MB_ICONASTERISK:
			strIcon = IDI_ASTERISK;
			nTitleID = IDS_T_INFORMATION;
			break;
		default:
			ASSERT(0);
	}

	// load information
	hIcon = ::LoadIcon(NULL, strIcon); // ex CWinApp->LoadStandartIcon()
	strTitle.LoadString(nTitleID);
	CWinApp* pWinApp = AfxGetApp();
	if (pWinApp)
		strAppName = pWinApp->m_pszAppName;
	else
		strAppName.LoadString(AFX_IDS_APP_TITLE);

	if(strAppName.GetLength() == 0)
		strAppName += strTitle; // if strAppName is empty nothing catastrophic will happen

	// update controls
	SetWindowText(strAppName);
	m_icon.SetIcon(hIcon);
	m_icon.MoveWindow(m_rcIcon);
}

/*
bool TMessageBox::FindIDInSetting(CString& rstrSettings, LPCTSTR szKeyName)
{
	bool bIsSet = false;
	int iNextDelimiter = 0;
	int nLenght = rstrSettings.GetLength();
	CString strCurrent;
	do
	{
		int iCurDelimiter = rstrSettings.Find(_T(","), iNextDelimiter);
		if (iCurDelimiter != -1 || (iNextDelimiter + 1) < nLenght )
		{
			strCurrent = rstrSettings.Mid(iNextDelimiter, iCurDelimiter != -1?(iCurDelimiter - iNextDelimiter):(nLenght - iNextDelimiter + 1) );
			if (strCurrent.GetLength() > 1 && strCurrent.Find(szKeyName) != -1)
				bIsSet = true;
		}
		else
			break;
		if ( iCurDelimiter == -1)
			break;            
		iNextDelimiter = iCurDelimiter + 1;
	}
	while(iNextDelimiter != -1);
	return bIsSet;
}

*/

/*
MB_ABORTRETRYIGNORE 
MB_OK                       +
MB_OKCANCEL                 +
MB_RETRYCANCEL              +
MB_YESNO                    +
MB_YESNOCANCEL              +

#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L

#define MB_ICONHAND                 0x00000010L
#define MB_ICONQUESTION             0x00000020L
#define MB_ICONEXCLAMATION          0x00000030L
#define MB_ICONASTERISK             0x00000040L

#define MB_USERICON                 0x00000080L
#define MB_ICONWARNING              MB_ICONEXCLAMATION
#define MB_ICONERROR                MB_ICONHAND

#define MB_ICONINFORMATION          MB_ICONASTERISK
#define MB_ICONSTOP                 MB_ICONHAND

#define MB_DEFBUTTON1               0x00000000L
#define MB_DEFBUTTON2               0x00000100L
#define MB_DEFBUTTON3               0x00000200L
#define MB_DEFBUTTON4               0x00000300L

#define MB_APPLMODAL                0x00000000L
#define MB_SYSTEMMODAL              0x00001000L
#define MB_TASKMODAL                0x00002000L
#define MB_HELP                     0x00004000L // Help Button

#define MB_NOFOCUS                  0x00008000L
#define MB_SETFOREGROUND            0x00010000L
#define MB_DEFAULT_DESKTOP_ONLY     0x00020000L

#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7
#define IDCLOSE             8
#define IDHELP              9
*/

////////////////////////////////////////////////////////////////////////
//  TMessageBox message handlers
//

BOOL TMessageBox::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

BOOL TMessageBox::OnHelpInfo(HELPINFO* pHelpInfo)
{
	SHORT state = GetAsyncKeyState(VK_F1);
	DWORD_PTR dwContext = pHelpInfo->dwContextId;
	UINT nCmd = HELP_WM_HELP;

	if(pHelpInfo->iContextType == HELPINFO_WINDOW && pHelpInfo->dwContextId != 0)
	{
		dwContext = pHelpInfo->dwContextId;
		nCmd = HELP_WM_HELP;
		TRACE3("WinHelp: pszHelpFile = '%s', dwData: $%lx, fuCommand: %d.\n",
			AfxGetApp()->m_pszHelpFilePath, dwContext, nCmd);

		::WinHelp((HWND)pHelpInfo->hItemHandle,
					AfxGetApp()->m_pszHelpFilePath,
					nCmd,
					// HELP_WM_HELP flag brings up pop-up help and expects an array
					// of DWORD pairs of the control ID and the context help ID
					(DWORD_PTR) &THelpInfo(pHelpInfo->iCtrlId, (DWORD) dwContext));
		return TRUE;
	}
	else
	{
/*
		if (state & 0x8000)
			return CWnd::OnHelpInfo(pHelpInfo);	 // standart behaviour
*/
		if(state & 0x8000) // I want popup instead of normal window
		{
			dwContext = m_arrHelpIDs[0];
			nCmd = HELP_CONTEXTPOPUP;

			TRACE3("WinHelp: pszHelpFile = '%s', dwData: $%lx, fuCommand: %d.\n",
					AfxGetApp()->m_pszHelpFilePath, dwContext, nCmd);

			::WinHelp((HWND)pHelpInfo->hItemHandle,
						AfxGetApp()->m_pszHelpFilePath,
						nCmd,
						dwContext);
		}
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////
//  TMessageBox command handlers
//

void TMessageBox::OnButton(UINT nID)
{
	if(nID == IDCANCEL)
	{
		if(!(m_nButtonType == MB_OK || m_nButtonType == MB_OKCANCEL || m_nButtonType == MB_YESNOCANCEL || m_nButtonType == MB_RETRYCANCEL))
			return; // do nothing
	}

	EndDialog(nID);
}

void TMessageBox::OnEditCopy()
{
	if(!OpenClipboard())
		return;

	EmptyClipboard();

	CString strError;
	CString strWindowText;
	for (int i=0; i < m_arrMessages.GetSize(); i++)
	{
		//CStatic* pStatic = m_arrStaticControls.GetAt(i);
		//pStatic->GetWindowText(strWindowText);
		strError += m_arrMessages.GetAt(i);
	}

	int iLength = strError.GetLength();
	// Allocate a global memory object for the text.
	HGLOBAL hglbCopy = ::GlobalAlloc(GMEM_MOVEABLE, (iLength + 1) * sizeof(TCHAR));
	if (hglbCopy == NULL)
	{
		CloseClipboard();
		return;
	}

	// Lock the handle and copy the text to the buffer.
	LPTSTR lpstrCopy = (LPTSTR) ::GlobalLock(hglbCopy);
	memcpy(lpstrCopy, strError, (iLength + 1) * sizeof(TCHAR));
	lpstrCopy[iLength] = (TCHAR) 0;		// null character
	::GlobalUnlock(hglbCopy);

	// Place the handle on the clipboard.
	HANDLE hHandle = SetClipboardData(CF_UNICODETEXT, hglbCopy);
	CloseClipboard();

	return;
}


void TMessageBox::OnHelp()
{
	DWORD dwContext = m_arrHelpIDs[0];
	UINT nCmd = HELP_CONTEXTPOPUP;
	TRACE3("WinHelp: pszHelpFile = '%s', dwData: $%lx, fuCommand: %d.\n",
		AfxGetApp()->m_pszHelpFilePath, dwContext, nCmd);

	::WinHelp ( (HWND)m_hWnd,
		AfxGetApp()->m_pszHelpFilePath,
		nCmd,
		dwContext);
}

void TMessageBox::OnRunReporter()
{
	TWaitCursor wait;
    /*
	TCHAR chCurrentDir[_MAX_PATH];
	::memset(chCurrentDir, 0, sizeof(TCHAR) * _MAX_PATH);
    */
	CString strErrorReporterPath = CMNGetFullCurrentPath(AfxGetInstanceHandle(), _T("samerr.exe"));
	if(!strErrorReporterPath.IsEmpty())
	{
		SHELLEXECUTEINFO execInfo;
		::memset(&execInfo, 0, sizeof(SHELLEXECUTEINFO));
		execInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		execInfo.lpFile = strErrorReporterPath;
		execInfo.lpVerb = _T("open");
		execInfo.nShow = SW_SHOWNORMAL;
		BOOL bRes = ::ShellExecuteEx(&execInfo);
	}
}
