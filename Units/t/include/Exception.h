//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Exceptions
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: Basic exceptions class with extended functionality, idea taken 
//               from CodeGuru and refactored
//--------------------------------------------------------------------------------

#pragma once

#include "CMN/include/Exception.h"

#include "T/include/T.h"
#include "T/include/Constants.h"
//#include "T/include/Buf.h"
//#include "T/Include/StartStopSyncronizer.h"

#define MB_CHECK				0x00200000L       // show checkbox (used in TMessageBox)
// the following flags are not used directly in TMessageBox
#define MB_SAVECHECKSTATE		0x00400000L       // save checkbox state 
#define MB_SAVECHECKSTATETMP	0x00800000L       // save checkbox state in temporary list  
#define MB_COPYTOCLIPBOARD		0x01000000L       // copy to clipboard
#define MB_RUNREPORTER			0x02000000L       // run the error reporter app

class TException;

extern T_DATA LPCTSTR cszPermanentMessagesSection;
extern T_DATA LPCTSTR cszTemporaryMessagesSection;
extern T_DATA LPCTSTR cszMessagesDisabledKey;

//T_FUNC __declspec (noreturn) void T_API   ThrowException(CException* pException, LPCSTR pszFile = NULL, int nLine = 0); // useful for any kind of exception
T_FUNC void         T_API   TDumpException(CException* pException, LPCSTR pszFile = NULL, int nLine = 0); // useful for any kind of exception
T_FUNC TException*  T_API   THasErrorCode (TException* pHiLevelEx, UINT nCode);
//T_FUNC void TClearTemporaryMessages(void);

/////////////////////////////////////////////////////////////////////////
//
//  TException
//

class T_CLASS TException: public CException, virtual public CMNException
{
    DECLARE_DYNAMIC(TException)

    //friend  T_FUNC void T_API ThrowException(CException* pException, LPCSTR pszFile, int nLine);
	friend __declspec (noreturn) void _stdcall ThrowException(CMNException* pException, LPCSTR pszFile, int nLine);

protected:
    //[by CMNException] UINT            m_nErrorCode;        // error code is a resource id in string table
    UINT			m_nType;
    UINT			m_nHelpContext;
    LPARAM			m_lParam;

    //[by CMNException] vector<BYTE>	m_bufArgList;
    //[by CMNException] CStringList		m_lstStrings;        // list of exception string arguments
    //[by CMNException] mutable CString	m_strRetrievedErrorText;

	//[by CMNException] CException*     m_pNext; // we do not know what is the actual type

public:
    //[by CMNException] static BOOL	ms_bExtendedReporting;
    //[by CMNException] static BOOL	ms_bUseWindowsMessageBox; // you reject help contexts, checkbox and other TMessageBox features
	//[by CMNException] static BOOL	ms_bAllowWin32Formatting;

//  Operations
public:
//  Construction/Desctruction
    TException();
    TException(TException const& rException);
    TException(CMNException const& rException);
    TException(UINT nErrorCode, UINT nType = MB_OK | /*MB_HELP | */MB_ICONERROR, LPARAM lParam = 0, UINT nHelpContext = 0);
    virtual ~TException();

//    void  Init(UINT nErrorCode, UINT nType = MB_OK | MB_HELP | MB_ICONERROR, LPARAM lParam = 0, UINT nHelpContext = 0);

//  CException overrides
    virtual BOOL    GetErrorMessage( LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL );
    virtual int     ReportError( UINT nType = MB_OK | MB_RUNREPORTER | MB_ICONERROR, UINT nMessageID = 0 );
//  own virtual functions
    //[by CMNException] virtual CString GetErrorMessage();
    //[by CMNException] virtual CString GetPreviousErrorMessages( LPCTSTR pszDelimiter = _T("\r\n") );   // return summary error text of all previous errors (from latest to the earlyest)
    virtual CMNException* Clone();
    virtual HRESULT GetHRESULT() const;

	virtual void	Delete();

protected:
    virtual void    InternalThrow();    // should be overriden in every descendant !!!
    virtual void    FormatMessage (LPCTSTR pcszFormat, LPTSTR pszError, UINT nMaxError) const;
    virtual CString GetKeyName    () const;

public:
//  simple functions
    //[by CMNException] UINT            GetErrorCode()      const { return m_nErrorCode; }
    UINT            GetType()           const { return m_nType;      }
    UINT            GetHelpContext()    const { return m_nHelpContext; }
    LPARAM          GetLParam()         const { return m_lParam;     }
    //[by CMNException] CException*     GetNext()           const { return m_pNext;      }
    void            ModifyType  (UINT nRemove, UINT nADD);

	TException&     operator << (const int iArg);
    TException&     operator << (LPCTSTR pszArg);
    TException&     operator += (const CMNException& rException);
    TException&     operator += (const TException& rException);
    //TException&     operator += (const CException& rException);
    void            Insert(TException* pEx);

    static void     T_API ClearTemporaryMessages();
    static CAtlString T_API GetErrorMessage(CException* pEx);
    static CString  T_API GetAllErrorMessages(CException* pEx, LPCTSTR pszDelimiter = _T("\r\n") );
    // [to restore] static TException*  T_API CloneException(CException* pEx); // will never return CException !!!
    static HRESULT  T_API GetHRESULT(CException* pEx);
//	const CStringList& GetListOfArguments() const {return m_lstStrings;}

//  Implementation
protected:
    CString         GetMessagePrefix() const;
    void            GetMessagesAndHelpIDs(CStringArray& arrMessages, CDWordArray& arrHelpIDs);
    INT_PTR         DisplayMessage(const CStringArray& arrMessages, const CDWordArray& arrHelpIDs, UINT nType,
                                   LPCTSTR pszCheckText, LPCTSTR pszKeyName, PBOOL pbCheckState);

//  Diagnostics
#if defined(_DEBUG) || defined(_AFXDLL)
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
};


#define TTRY try {
// to escape conflict with old MFC 'TRY' macros

#define THROW0(nMessageID) \
	{ \
		TException* pHiLevelEx = new TException(##nMessageID); \
        ::ThrowException(pHiLevelEx, __FILE__, __LINE__); \
	}

#define THROW1(nMessageID, arg1) \
	{ \
		TException* pHiLevelEx = new TException(##nMessageID); \
		(*pHiLevelEx) << ##arg1;\
		::ThrowException(pHiLevelEx, __FILE__, __LINE__); \
	}

#define THROW2(nMessageID, arg1, arg2) \
	{ \
		TException* pHiLevelEx = new TException(##nMessageID); \
		(*pHiLevelEx) << ##arg1;\
		(*pHiLevelEx) << ##arg2;\
		::ThrowException(pHiLevelEx, __FILE__, __LINE__); \
	}

#define THROW3(nMessageID, arg1, arg2, arg3) \
	{ \
		TException* pHiLevelEx = new TException(##nMessageID); \
		(*pHiLevelEx) << ##arg1;\
		(*pHiLevelEx) << ##arg2;\
		(*pHiLevelEx) << ##arg3;\
		::ThrowException(pHiLevelEx, __FILE__, __LINE__); \
	}

#define THROW4(nMessageID, arg1, arg2, arg3, arg4) \
	{ \
		TException* pHiLevelEx = new TException(##nMessageID); \
		(*pHiLevelEx) << ##arg1;\
		(*pHiLevelEx) << ##arg2;\
		(*pHiLevelEx) << ##arg3;\
		(*pHiLevelEx) << ##arg4;\
		::ThrowException(pHiLevelEx, __FILE__, __LINE__); \
	}

#define THROW5(nMessageID, arg1, arg2, arg3, arg4, arg5) \
	{ \
		TException* pHiLevelEx = new TException(##nMessageID); \
		(*pHiLevelEx) << ##arg1;\
		(*pHiLevelEx) << ##arg2;\
		(*pHiLevelEx) << ##arg3;\
		(*pHiLevelEx) << ##arg4;\
		(*pHiLevelEx) << ##arg5;\
		::ThrowException(pHiLevelEx, __FILE__, __LINE__); \
	}

#define THROW6(nMessageID, arg1, arg2, arg3, arg4, arg5, arg6) \
	{ \
		TException* pHiLevelEx = new TException(##nMessageID); \
		(*pHiLevelEx) << ##arg1;\
		(*pHiLevelEx) << ##arg2;\
		(*pHiLevelEx) << ##arg3;\
		(*pHiLevelEx) << ##arg4;\
		(*pHiLevelEx) << ##arg5;\
        (*pHiLevelEx) << ##arg6;\
		::ThrowException(pHiLevelEx, __FILE__, __LINE__); \
	}


#define CATCH_AND_THROW0(nMessageID) \
	} \
	catch (TException *pEx) { \
		TException* pHiLevelEx = new TException(##nMessageID); \
		(*pHiLevelEx) += (*pEx); \
		::ThrowException(pHiLevelEx, __FILE__, __LINE__); \
	}

#define CATCH_AND_THROW1(nMessageID, arg1) \
	} \
	catch (TException *pEx) { \
		TException* pHiLevelEx = new TException(##nMessageID); \
		(*pHiLevelEx) << ##arg1; \
		(*pHiLevelEx) += (*pEx); \
		::ThrowException(pHiLevelEx, __FILE__, __LINE__); \
	}

#define CATCH_AND_THROW2(nMessageID, arg1, arg2) \
	} \
	catch (TException *pEx) { \
		TException* pHiLevelEx = new TException(##nMessageID); \
		(*pHiLevelEx) << ##arg1;\
		(*pHiLevelEx) << ##arg2;\
		(*pHiLevelEx) += (*pEx); \
		::ThrowException(pHiLevelEx, __FILE__, __LINE__); \
	}

#define CATCH_AND_THROW3(nMessageID, arg1, arg2, arg3) \
	} \
	catch (TException *pEx) { \
		TException* pHiLevelEx = new TException(##nMessageID); \
		(*pHiLevelEx) << ##arg1;\
		(*pHiLevelEx) << ##arg2;\
		(*pHiLevelEx) << ##arg3;\
		(*pHiLevelEx) += (*pEx); \
		::ThrowException(pHiLevelEx, __FILE__, __LINE__); \
	}

#define CATCH_AND_THROW4(nMessageID, arg1, arg2, arg3, arg4) \
	} \
	catch (TException *pEx) { \
		TException* pHiLevelEx = new TException(##nMessageID); \
		(*pHiLevelEx) << ##arg1;\
		(*pHiLevelEx) << ##arg2;\
		(*pHiLevelEx) << ##arg3;\
        (*pHiLevelEx) << ##arg4;\
		(*pHiLevelEx) += (*pEx); \
		::ThrowException(pHiLevelEx, __FILE__, __LINE__); \
	}


#define CATCH_AND_SUPRESS_ALL(strMethodName)\
	}\
	catch (TException *pEx)\
	{\
        if (!T_g_StartStopSyncronizer.GetIsStopping())\
        {\
            TTrace(traceAll, _T("%s: %s\n"), strMethodName, pEx->GetErrorMessage());\
        }\
        pEx->Delete();\
    } \
	catch (CException* pEx)\
	{ \
        if (!T_g_StartStopSyncronizer.GetIsStopping())\
        {\
		    TTrace(traceAll, _T("%s: CException occured \n"), strMethodName);\
        }\
        pEx->Delete();\
	}


