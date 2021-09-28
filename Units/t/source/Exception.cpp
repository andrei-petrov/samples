//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Exceptions
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Basic exceptions class with extended functionality, 
//               Idea taken from CodeGuru and refactored
//--------------------------------------------------------------------------------

#include "stdafx.h"

#include "T/include/MessageBox.h"
#include "T/include/SimpleException.h"
#include "T/include/WinApp.h"
#include "T/include/Exception.h"

#include "T/resource/resource.h"

const int ciMaxMessageLength = 4096; // in symbols, not in bytes

LPCTSTR cszPermanentMessagesSection = _T("Messages\\Permanent");
LPCTSTR cszTemporaryMessagesSection = _T("Messages\\Temporary");
LPCTSTR cszMessagesDisabledKey =      _T("DisabledMessages");

/*
void T_API ThrowException(CException* pException, LPCSTR pszFile, int nLine)
{   
    TDumpException(pException, pszFile, nLine);
    TException* pEx = dynamic_cast<TException*>(pException);
    if(pEx)
        pEx->Throw();
    else
        throw pException;
}
*/

void T_API TDumpException (CException* pException, LPCSTR pszFile, int nLine)
{
    CRuntimeClass* pClass = pException->GetRuntimeClass();
    if (pszFile == NULL && nLine == 0)
		TRACE(_T("%s: %s \n"), (LPCTSTR)CA2CT(pClass->m_lpszClassName), (LPCTSTR) TException::GetErrorMessage(pException));
        //TTrace (traceAll, _T("%s: %s \n"), (LPCTSTR)CA2CT(pClass->m_lpszClassName), (LPCTSTR) TException::GetErrorMessage(pException));
    else
		TRACE(_T("%s: %s, file = %s line = %i \n"), (LPCTSTR)CA2CT(pClass->m_lpszClassName), (LPCTSTR) TException::GetErrorMessage(pException), (LPCTSTR)CA2CT(pszFile), nLine);
        //TTrace (traceAll, _T("%s: %s, file = %s line = %i \n"), (LPCTSTR)CA2CT(pClass->m_lpszClassName), (LPCTSTR) TException::GetErrorMessage(pException), (LPCTSTR)CA2CT(pszFile), nLine);
}

TException* T_API THasErrorCode(TException* pHighLevelEx, UINT nCode)
{
    CMNException* pEx = static_cast<CMNException*>(pHighLevelEx);

    do
    {
		TException* pException = dynamic_cast<TException*>(pEx);
        if (pException && pEx->GetErrorCode() ==  nCode) // we have found the particular nCode value
            return pException;
    }
    while (pEx = pEx->GetNext());

    return NULL;
}


/////////////////////////////////////////////////////////////////////////
//
//  TException class
//

IMPLEMENT_DYNAMIC(TException, CException)

//[by CMNException] BOOL    TException::ms_bExtendedReporting = FALSE;
//[by CMNException] BOOL    TException::ms_bUseWindowsMessageBox = TRUE;
//[by CMNException] BOOL    TException::ms_bAllowWin32Formatting = TRUE;

TException::TException()
{
    m_bAutoDelete       = TRUE;
    m_nType             = MB_OK | /*MB_HELP | */MB_ICONERROR;
    m_nHelpContext      = 0;
    m_lParam            = 0;
}

TException::TException(TException const& rException):
	CMNException(rException)
{
    m_bAutoDelete           = TRUE;
    m_nType                 = rException.m_nType;
    m_lParam                = rException.m_lParam;
    m_nHelpContext          = rException.m_nHelpContext;    
}

TException::TException(CMNException const& rException):
	CMNException(rException)
{
    m_bAutoDelete           = TRUE;
    m_nType                 = MB_OK | /*MB_HELP | */MB_ICONERROR;
    m_lParam                = 0;
    m_nHelpContext          = 0;
}

TException::TException(UINT nErrorCode, UINT nType, LPARAM lParam, UINT nHelpContext):
	CMNException(nErrorCode)
{
    m_bAutoDelete       = TRUE;
    m_nType             = nType | MB_COPYTOCLIPBOARD;
    m_lParam            = lParam;
    m_nHelpContext      = nHelpContext;

    if (m_nHelpContext  == 0) // if no explicit context given nErrorCode becomes a context
        m_nHelpContext  = nErrorCode + HID_BASE_PROMPT;

    if (ms_bExtendedReporting)
        *this << (int)nErrorCode; // we should push this argument first !
}

TException::~TException()
{
	/* [moved to CMNException::~CMNException]
    if (m_pNext)
        m_pNext->Delete(); // USE ->Delete() instead of operator delete
	*/
}

/*
void TException::Init(UINT nErrorCode, UINT nType, LPARAM lParam, UINT nHelpContext)
{
    m_bAutoDelete       = TRUE;
    m_nErrorCode        = nErrorCode;
    m_nType             = nType;
    m_lParam            = lParam;
    m_nHelpContext      = nHelpContext;
    if (m_nHelpContext  == 0) // if no explicit context given nErrorCode becomes a context
        m_nHelpContext  = nErrorCode + HID_BASE_PROMPT;
    m_pNext             = NULL;
    if (ms_bExtendedReporting)
        *this << (int)nErrorCode; // we should push this argument first !
}
*/
/////////////////////////////////////////////////////////////////////////////
//
//                      Virtual functions


//                    CException overrides

BOOL TException::GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext)
{
    if (m_strRetrievedErrorText.GetLength() || !m_nErrorCode) // error message text already retrieved
        lstrcpyn(lpszError, (LPCTSTR)m_strRetrievedErrorText, nMaxError);
    else
    {
        CString strFormat, strTemplate;
        strTemplate = TLoadToolTip(m_nErrorCode, FALSE, 0, TCHAR('\t')); // load zero tooltip  - the string itself
        strTemplate.TrimLeft();
        strFormat = GetMessagePrefix() + strTemplate; // concatenating prefix and message
        FormatMessage(strFormat, lpszError, nMaxError);

        m_strRetrievedErrorText = CString(lpszError, nMaxError); // store for future use
    }

    if(pnHelpContext)
        *pnHelpContext = m_nHelpContext;

    return TRUE;
}

int TException::ReportError( UINT nType, UINT nMessageID ) // ignore any supplied parameters
{
    if (nType)
        m_nType = nType;

    CStringArray arrMessages;
    CDWordArray  arrHelpIDs;

    GetMessagesAndHelpIDs(arrMessages, arrHelpIDs);
	m_nType |= MB_RUNREPORTER;
    return DisplayMessage(arrMessages, arrHelpIDs, m_nType, NULL, GetKeyName(), NULL); // always suggest checked checkbox ?
}

/*
CString TException::GetErrorMessage()
{
	return GetErrorMessage(this);
}

CString TException::GetPreviousErrorMessages (LPCTSTR pszDelimiter)
{
    CString strText;
    TCHAR szMessage[ciMaxMessageLength];
    
    // Start not from current Exc, but from the next
    CException* pException = m_pNext;
    
    while (pException)
    {
        szMessage[0] = TCHAR('\0');
        // Get error message of the current Exc
        pException->GetErrorMessage (szMessage, sizeof(szMessage)/sizeof(TCHAR));
        strText += szMessage;

        // Check if current Exc is TExcpetion (not CException that has no pNext member)
        TException* pTEx = dynamic_cast<TException*> (pException);
        if (pTEx && pTEx->m_pNext)
        {   // Current Exc is not the last one ..
            strText += pszDelimiter;    // .. add delimiter between current and next Exc
            pException = pTEx->m_pNext;  // .. move to next Exc in list
        }
        else
            pException = NULL;    // current Exc was the last one
    }

    return strText;
}
*/

CMNException* TException::Clone()
{
    return new TException(*this);
}

HRESULT TException::GetHRESULT() const
{
    UINT nIconType = m_nType & MB_ICONMASK;     // 0x000000F0L
    int iSev = 0;
    if (nIconType == MB_ICONERROR)
        iSev = 1;
    return MAKE_HRESULT(iSev, FACILITY_ITF, GetErrorCode() );
}


//                          protected virtual

void TException::InternalThrow()
{
    throw this;
}

void TException::FormatMessage(LPCTSTR pcszFormat, LPTSTR pszError, UINT nMaxError) const
{
	_ASSERT (pcszFormat && pszError && nMaxError);
	if (nMaxError == 0)
		return;
	if (pcszFormat == NULL || pszError == NULL)
		return;

	// If ms_bAllowWin32Formatting is not set to TRUE, the old %s formatting is used.
	// Otherwise, a message is formatted either as %1 or %s depending on the pcszFormat value.

	BOOL bUseWin32Formatting = false;

	if (ms_bAllowWin32Formatting)
	{
		bool bPercent = false;	// true if the '%' symbol precedes the current one
		for (const TCHAR* pch = pcszFormat; *pch != _T('\0'); pch++)
		{
			if (*pch == _T('%'))
				bPercent = !bPercent;	// skip "%%" sequence
			else if (_istdigit(*pch) && bPercent)
			{
				bUseWin32Formatting = TRUE;
				break;
			}
			else
				bPercent = false;
		}
	}

	va_list argList = reinterpret_cast<va_list>(m_bufArgList.GetData());
	if (!bUseWin32Formatting)
	{
		CString str;
		str.FormatV(pcszFormat, argList); // much more safe that vsprintf
		lstrcpyn(pszError, str, nMaxError);
		return;
	}

	if (nMaxError == 0)
		return;

	pszError[0] = _T('\0');

	LPTSTR pszTemp = NULL;
	DWORD dwResult = ::FormatMessage(
		FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ALLOCATE_BUFFER,
		pcszFormat, 0, 0,
		reinterpret_cast<LPTSTR>(&pszTemp), 0,
		&argList);

	if (dwResult != 0)
	{
		lstrcpyn(pszError, pszTemp, nMaxError);
		LocalFree(pszTemp);
	}
}

CString TException::GetKeyName () const
{
    CString str;
    str.Format(_T("%08i"), m_nErrorCode);
    return str;
}

//

void TException::ModifyType(UINT nRemove, UINT nADD)
{
    m_nType &= ~nRemove;
    m_nType |= nADD;
}

TException& TException::operator << (const int iArg)
{
	CMNException::operator << (iArg);
    return *this;
}

TException& TException::operator << (LPCTSTR pszArg)
{
	CMNException::operator << (pszArg);
    return *this;
}

TException& TException::operator += (const CMNException& rException)
{
	CMNException::operator +=(rException);
	return *this;
}

TException& TException::operator += (const TException& rException)
{
	CMNException::operator +=(rException);
	return *this;
}

/*
TException& TException::operator += (const CException& rException)
{    
    //TException* pCurrent = this;      
    //while(pCurrent->m_pNext)
    //{
    //    TException* pNext = dynamic_cast <TException*>(pCurrent->m_pNext);
    //    if (pNext == NULL)
    //    {
    //        // Last exception is CException so will be converted to TSimpleException and deleted.
    //        ASSERT(FALSE);

    //        pNext = new TSimpleException( GetErrorMessage(pCurrent->m_pNext) );
    //        pCurrent->m_pNext->Delete();
    //        pCurrent->m_pNext = pNext;
    //    }
    //    pCurrent = pNext;
    //}
    //pCurrent->m_pNext = const_cast<CException*>(&rException);

	CMNException const* pCMNEx = dynamic_cast<CMNException const*>(&rException);
	if(pCMNEx)
		CMNException::operator += (*pCMNEx);
	else
	{
		TException* pTEx = new TException(rException);
		CMNException::operator += (*pTEx);
	}

    return *this;
}
*/

void TException::Insert(TException* pEx)
{
	/*
    CException* pNext = m_pNext;
    *this += *pEx;
    *pEx += *pNext;
	*/
	CMNException::Insert(pEx);
}


//                          static functions

void TException::ClearTemporaryMessages()
{
    CWinApp*    pApp = AfxGetApp();
    CRegKey key;
    key.Attach ( pApp->GetAppRegistryKey() ); // GetSectionKey();
    key.DeleteSubKey(cszTemporaryMessagesSection);
    key.Close();
}


CAtlString TException::GetErrorMessage(CException* pEx)
{
    ASSERT_VALID(pEx);
	TCHAR szBuf[ciMaxMessageLength] = _T("\0");
    if (pEx)
	    pEx->GetErrorMessage(szBuf, sizeof(szBuf)/sizeof(TCHAR)-1, NULL);
	return CAtlString(szBuf);
}

CString TException::GetAllErrorMessages(CException* pEx, LPCTSTR pszDelimiter)
{
    ASSERT_VALID(pEx);
    CMNException* pCMNEx = dynamic_cast<CMNException*>(pEx);
    if (pCMNEx)
    {
        return pCMNEx->GetErrorMessage() +
			pszDelimiter +
			pCMNEx->GetPreviousErrorMessages( pszDelimiter );
    }
    else if (pEx)
    {
        return TException::GetErrorMessage(pEx);
    }
    return CString();
}
/* [to restore]
TException* TException::CloneException(CException* pEx)
{
    ASSERT_VALID(pEx);
    if (pEx == NULL)
        return NULL;

    TException* pTEx = dynamic_cast<TException*> (pEx);
    if (pTEx)
    {
        TException* pNewTEx = dynamic_cast<TException*>(pTEx->Clone());
        if ( pTEx->GetNext() )
        {
            CException* pNewNext = CloneException( dynamic_cast<CException*>(pTEx->GetNext()) );
			if(pNewNext)
				*pNewTEx += *pNewNext;
        }
        return pNewTEx;
    }
    else // CException could not be copied
    {
        TSimpleException* pSimpleEx = new TSimpleException( GetErrorMessage(pEx) );
        return pSimpleEx;
    }
}
*/
//--------------------------------------------------------------------------------------------
HRESULT TException::GetHRESULT(CException* pEx)
{
    TException* pExcT = dynamic_cast<TException*> (pEx);
    if (pExcT)
        return pExcT->GetHRESULT();
    else
        return E_FAIL;
}

void TException::Delete()
{
	CException::Delete();
}

//--------------------------------------------------------------------------------------------


//                                          implementation


CString TException::GetMessagePrefix() const
{
    CString str;
    if (ms_bExtendedReporting)
        str.LoadString(IDS_T_EXCEPTION_MSGPREFIX); // "Message %i:"    
    return str;
}

void TException::GetMessagesAndHelpIDs(CStringArray& arrMessages, CDWordArray& arrHelpIDs)
{
    TCHAR   szMessage[ciMaxMessageLength];
    
    CException* pException = this;      

    while (pException)
    {        
        UINT    nHelpContext = 0;

        ASSERT_VALID(pException);
        pException->GetErrorMessage(szMessage, sizeof(szMessage)/sizeof(TCHAR)-1, &nHelpContext);
        arrMessages.Add(szMessage);
        arrHelpIDs.Add(nHelpContext);
        TException* pTEx = dynamic_cast <TException*>(pException);
        if (pTEx) 
            pException = dynamic_cast<CException*>(pTEx->m_pNext);
        else
            pException = 0; 
    } 
}


INT_PTR TException::DisplayMessage(const CStringArray& arrMessages, const CDWordArray& arrHelpIDs, UINT nType, 
                               LPCTSTR pszCheckText, LPCTSTR pszKeyName, PBOOL pbCheckState)
{
    INT_PTR nResult;

    ASSERT( arrMessages.GetSize() != 0 );
    ASSERT( arrMessages.GetSize() == arrHelpIDs.GetSize() );

    CString strMessagesSection; // Select section    

    if(nType & MB_SAVECHECKSTATE) 
        strMessagesSection = cszPermanentMessagesSection;
    if(nType & MB_SAVECHECKSTATETMP) 
        strMessagesSection = cszTemporaryMessagesSection;
   	
    CString strCheckText;
    if( nType & MB_SAVECHECKSTATE )     strCheckText.LoadString(IDS_T_DO_NOT_SHOW);
    if( nType & MB_SAVECHECKSTATETMP )  strCheckText.LoadString(IDS_T_SUPPRESS);    

    BOOL bCheckState = FALSE;
    if (pbCheckState)
        bCheckState = *pbCheckState;

    if (!ms_bUseWindowsMessageBox)
    {
        TMessageBox msgbox( arrMessages, 
                            arrHelpIDs, 
                            nType, 
                            strCheckText, 
                            bCheckState, 
                            strMessagesSection, 
                            m_nErrorCode); // always suggest checked checkbox
        nResult = msgbox.DoModal();
    }
    else
    {
        CString strText;
        for (int i=0; i<arrMessages.GetSize(); i++)
            strText += arrMessages[i];

        nResult = AfxMessageBox(strText, nType, arrHelpIDs[0]);
    }
    return nResult;
}


/////////////////////////////////////////////////////////////////

#if defined (_DEBUG) || defined (_AFXDLL)

void TException::AssertValid() const
{
    ASSERT(this != NULL);    
}

void TException::Dump(CDumpContext& dc) const
{
    dc << "a " << GetRuntimeClass()->m_lpszClassName <<
    " at " << (void*)this   << 
    " m_nErrorCode = "      << m_nErrorCode << 
    " m_nType = "           << m_nType << 
    " m_nHelpContext = "    << m_nHelpContext << 
    " m_pNext = "           << m_pNext;    
}

#endif



