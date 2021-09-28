//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Thread for background operations
//--------------------------------------------------------------------------

#include "stdafx.h"

#include "resource.h"

#include "cmn/include/misc.h"
#include "cmn/include/Log.h"
#include "cmn/include/Exception.h"
#include "cmn/include/VersionInfo.h"

#include "db/include/Constants.h"
#include "db/include/Storage.h"

#include "scm/include/SchemaDef.h"

#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/ConfigApp.h"
#include "wzr/include/Thread.h"
#include "ocrcfg/include/SoftLibraryThread.h"
#include "ocrcfg/include/misc.h"

// CSoftLibraryThread

//IMPLEMENT_DYNCREATE(CSoftLibraryThread, CWinThread)

CSoftLibraryThread::CSoftLibraryThread(HWND hParentWnd,
								 LPCTSTR pszSqlServer, LPCTSTR pszDatabase,
								 CSoftLibActionEnum proc, const set<CString>*	setXmlFileNames)
    : WZRThread(hParentWnd)
{
	m_strSqlServer		= pszSqlServer;
	m_strDatabase		= pszDatabase;
	m_enSoftLibAction	= proc;
	m_setXmlFileNames	= setXmlFileNames;
}

CSoftLibraryThread::~CSoftLibraryThread()
{
}

BOOL CSoftLibraryThread::InitInstance()
{
	CoInitialize(NULL);
    CWinThread::InitInstance();

	BOOL	bError = FALSE;
	long i = 0;
	bstr_t bstrXML("");
	CString strImportedFiles;

	try
	{
		m_storage.Connect(cszDefSAMConnectionString, m_strSqlServer, m_strDatabase);
		switch ( m_enSoftLibAction )
		{
		case slpExport:
			::PostMessage(m_hParentWnd, WM_OCRCFG_SOFTLIBRARYPROC_START, NULL, NULL);
			{
				CFile file;
				CFileException ex;
				if ( file.Open((LPCTSTR)*m_setXmlFileNames->begin(), CFile::modeWrite | CFile::modeCreate | CFile::shareDenyWrite, &ex) )
				{
					bstr_t bstrXML = m_storage.GetSoftLibrary(1, "", "");
					CT2CA strXML_UTF8(bstrXML, CP_UTF8);
					CStringA strHead = "\xef\xbb\xbf<?xml version=\"1.0\" ?>\r\n";
					file.Write((const char*)strHead, strHead.GetLength());
					file.Write(strXML_UTF8, strlen(strXML_UTF8));
					file.Close();
				}
				else
				{
					CString strErr = CMNFormatString(ex.m_lOsError);
					CMNException* pE = new CMNException(IDE_FILEOPERATION_ERROR);
					*pE << strErr;
					*pE << *m_setXmlFileNames->begin();
					ThrowException(pE);
				}
			}
			m_strThreadResults.Format(IDS_OCRCFG_SOFTLIBRARY_EXPORT_RESULTS, *m_setXmlFileNames->begin());
			break;

		case slpImport:
				for(set<CString>::iterator it=m_setXmlFileNames->begin(); it!=m_setXmlFileNames->end(); ++it )
				{
					strImportedFiles+=_T("\r\n");
					strImportedFiles.Append(*it);
					CString strStatus;
					try
					{
						ImportLibraryXmlFileToDB(*it);
						
						strStatus.LoadString(IDS_OCRCFG_LIBRARY_IMPORT_FILE_SUCCESS);
						strImportedFiles+=_T(" - ")+strStatus;
					}
					catch(CMNException *pEx)
					{
						CString strMessage;
						strMessage.LoadString(IDE_OCRCFG_LIBRARY_IMPORT_FILE);
						strMessage+=*it;

						CString strException=CMNException::GetAllErrorMessages(pEx);

						strMessage+=_T("\r\n")+strException;

				        CMNLog(_T("[CSoftLibraryThread] Exception occurs. %s"), (LPCTSTR)strMessage);
						AfxMessageBox(strMessage, MB_OK | MB_ICONWARNING);

						strStatus.LoadString(IDS_OCRCFG_LIBRARY_IMPORT_FILE_FAILED);
						strImportedFiles+=_T(" - ")+strStatus;
					}
				}
				m_strThreadResults.Format(IDS_OCRCFG_SOFTLIBRARY_IMPORT_RESULTS, strImportedFiles);
			break;

		default:
			break;
		}
		m_storage.Disconnect();
		CMNLog(_T("[CCreateDBThread::InitInstance]: %s"), m_strThreadResults);
	}
	catch(CMNException* pEx)
	{	    
		m_storage.Disconnect();
		m_strError = CMNException::GetAllErrorMessages(pEx, _T("\n"));
		CMNLog(_T("[CSoftLibraryThread::InitInstance]: %s"), m_strError);
		bError = TRUE;
		pEx->Delete();
	}

	::PostMessage(m_hParentWnd, WM_OCRCFG_SOFTLIBRARYPROC_FINISH, bError, NULL);	
	
	return TRUE;
}


// --------------------------------------------------------------------------------------------------------------
int CSoftLibraryThread::ExitInstance()
{
	CoUninitialize();
	return CWinThread::ExitInstance();
}

// --------------------------------------------------------------------------------------------------------------
void CSoftLibraryThread::ImportLibraryXmlFileToDB(CString strXmlFileName)
{
	int nStage = 1;
	bstr_t bstrXML("");
	MSXML::IXMLDOMDocumentPtr piDoc;
	HRESULT hr = piDoc.CreateInstance(__uuidof(MSXML::DOMDocument));
	CMNCheckError(hr);

	VARIANT_BOOL bLoad = piDoc->load( (variant_t) strXmlFileName);		
	if (bLoad == VARIANT_FALSE)
	{
		CMNException* pEx = new CMNException( IDE_CMN_XML_LOAD_FAILED );
		*pEx << strXmlFileName;
		ThrowException(pEx);
	}

	MSXML::IXMLDOMElementPtr piLibRoot = piDoc->documentElement;
	if ( ! piLibRoot )
	{
		CMNException* pEx = new CMNException( IDE_CMN_XML_LOAD_FAILED );
		*pEx << strXmlFileName;
		ThrowException(pEx);
	}

	const bstr_t bstrLibNodeName = piLibRoot->nodeName;

	const bstr_t bstrLibRoot = cszSoftLibraryRootTag;
	if ( bstrLibNodeName != bstrLibRoot )
	{
		CMNException* pEx = new CMNException( IDE_LOAD_LIBRARY_FORMAT_ERROR );
		ThrowException(pEx);
	}

	const bstr_t bstrLibPublisher = cszObjectType_Publisher;
	MSXML::IXMLDOMNodeListPtr piPublisherList = piLibRoot->selectNodes(bstrLibPublisher);
	if ( ! piPublisherList )
	{
		CMNException* pEx = new CMNException( IDE_CMN_XML_LOAD_FAILED );
		*pEx << strXmlFileName;
		ThrowException(pEx);
	}

	const long nPublishers = piPublisherList->length;

	if ( nPublishers <= 0)
	{
		CMNException* pEx = new CMNException( IDE_LOAD_LIBRARY_EMPTY_PUBLISHERS_LIST );
		ThrowException(pEx);
	}

	const DWORD dwMaxSteps = 100;
	DWORD dwStep = 0;
	if ( nPublishers < dwMaxSteps )
	{
		dwStep = 1;
		::PostMessage(m_hParentWnd, WM_OCRCFG_SOFTLIBRARYPROC_START, NULL, nPublishers);
	}
	else
	{
		dwStep = nPublishers / dwMaxSteps;
		if ((nPublishers % dwMaxSteps) != 0 ) dwStep++;
		::PostMessage(m_hParentWnd, WM_OCRCFG_SOFTLIBRARYPROC_START, NULL, dwMaxSteps);
	}

	MSXML::IXMLDOMElementPtr piCurrentRoot = piDoc->createElement(bstrLibNodeName);
	if ( ! piCurrentRoot )
	{
		CMNException* pEx = new CMNException( IDE_CREATE_XML_NODE_ERROR );
		ThrowException(pEx);
	}

	::SendMessage(m_hParentWnd, WM_OCRCFG_SOFTLIBRARYPROC_FILE_IMPORT_START, NULL, (LPARAM)(LPCTSTR)strXmlFileName);

	for (long i=1; i<=nPublishers; i++)
	{
		MSXML::IXMLDOMElementPtr piPublisher = piPublisherList->item[i-1];
		if ( ! piPublisher ) continue;
		piCurrentRoot->appendChild( piPublisher->cloneNode(VARIANT_TRUE) );
		long testlen = piPublisherList->length;
		DWORD dwModule = i % dwStep;
		if ((dwModule == 0) || (i==nPublishers))
		{
			// сохранить документ
			bstrXML = piCurrentRoot->xml;
			m_storage.InsertSoftLibrary( bstrXML );
			// пересоздать новый документ
			bstrXML = "";
			piCurrentRoot = piDoc->createElement(bstrLibNodeName);
			if ( ! piCurrentRoot )
			{
				CMNException* pEx = new CMNException( IDE_CREATE_XML_NODE_ERROR );
				ThrowException(pEx);
			}
			// сдвинуть прогресс
			::PostMessage(m_hParentWnd, WM_OCRCFG_SOFTLIBRARYPROC_STAGE, nStage, NULL);
			nStage++;
		}
	}
	
}