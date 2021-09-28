//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: Thread for background operations
//--------------------------------------------------------------------------

#include "stdafx.h"

#include <shlwapi.h>
#include <atlsecurity.h>

#include "cmn/include/Log.h"
#include "cmn/include/McException.h"
#include "cmn/include/RegKey.h"
#include "cmn/include/VersionInfo.h"
#include "CMN/include/Misc.h"
#include "CMN/include/IEconfigure.h"

#include "db/include/Constants.h"
#include "db/include/Storage.h"

#include "scm/include/SchemaDef.h"

#include "rgs/include/ServiceController.h"
#include "rgs/include/wmihelper.h"

#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/ConfigApp.h"
#include "wzr/include/Thread.h"
#include "ocrcfg/include/DeployRptThread.h"
#include "ocrcfg/include/misc.h"


#pragma warning(disable: 4800)

// CDeployRptThread

//IMPLEMENT_DYNCREATE(CDeployRptThread, CWinThread)

CDeployRptThread::CDeployRptThread(HWND hParentWnd, std::set<CString> m_setXMLBody, LPCTSTR pszSQLServer, LPCTSTR pszDbName,
		LPCTSTR pszReportsServerURL, LPCTSTR pszReportServerWMINamespace, 
		LPCTSTR pszReportsFolder, LPCTSTR pszReportsDir, LPCTSTR pszReportsDir2, 
		CConfigXMLData::ReportInfo::Vector arrReportsInfo, int* pnModificationFlags)
: 
WZRThread(hParentWnd),
m_dwReports(0),
m_setXMLBody(m_setXMLBody),
m_strSQLServer(pszSQLServer),
m_strDbName(pszDbName),
m_strReportsServerURL(pszReportsServerURL),
//m_strReportsServerURLUserEnter(pszReportsServerURLUserEnter),
m_strReportServerWMINamespace(pszReportServerWMINamespace),
m_strReportsFolder(pszReportsFolder),
m_strReportsDir(pszReportsDir),
m_strReportsDir2(pszReportsDir2),
m_arrReportsInfo(arrReportsInfo),
m_pnModificationFlags(pnModificationFlags)
{
	if(!m_pnModificationFlags)
	{
		CMNException* pEx = new CMNException(IDE_OCRCFG_POINTER_TO_INT_IS_NULL);
		*pEx << _T("CDeployRptThread::CDeployRptThread");
		ThrowException(pEx);
	}

}

CDeployRptThread::~CDeployRptThread()
{
}

BOOL CDeployRptThread::InitInstance()
{
	CoInitialize(NULL);

    CWinThread::InitInstance();

	BOOL bError = FALSE;
	int nRes    = IDNO;
	BOOL bExist = FALSE;
	BOOL bDeployed = FALSE;

	::PostMessage(m_hParentWnd, WM_OCRCFG_DEPLOYRPT_START, NULL, m_arrReportsInfo.size());
	try
	{
		CString strExistingReportsList = CheckReportsExists();
		if ( strExistingReportsList.GetLength() > 0 )
		{
			bExist = TRUE;
            if(theApp.m_bUnattendedMode)
                nRes = IDYES;
            else
            {
			    CString strQuestion;
			    strQuestion.FormatMessage(IDS_OCRCFG_RPT_DEPLOY_QUESTION,
					m_strReportsFolder, m_strReportsServerURL, strExistingReportsList);
			    nRes = AfxMessageBox(strQuestion, MB_ICONQUESTION | MB_YESNO);
            }
		}

		if(nRes == IDYES)
		{
			DeleteReports();
			DeployReports();
			bDeployed = TRUE;
		}

		if(!bExist)
		{
			DeployReports();
			bDeployed = TRUE;
		}

		CString strUrl = m_strReportsServerURL;
		strUrl.Replace(_T('\\'), _T('/'));
		CMNSetTrustedSite(strUrl);

		if(bDeployed == TRUE)
		{
			CString strFormat;
			strFormat.LoadString(IDS_OCRCFG_RPT_RESULTS);
			m_strThreadResults.Format(strFormat, m_strReportsFolder, m_strReportsServerURL, 
				m_strReportsList, m_dwReports);
			CMNLog(_T("[CDeployRptThread::InitInstance]: %s"), m_strThreadResults);
			*m_pnModificationFlags |= IsInstallReports;
		}
	}
	catch(CMNException* pEx)
	{
		m_strError = CMNException::GetAllErrorMessages(pEx, _T("\n"));
		CMNLog(_T("[CDeployRptThread::InitInstance]: %s"), m_strError);
		bError = TRUE;
		pEx->Delete();
	}

	::PostMessage(m_hParentWnd, WM_OCRCFG_DEPLOYRPT_FINISH, bError, NULL);

	return TRUE;
}

int CDeployRptThread::ExitInstance()
{
	CoUninitialize();
	return CWinThread::ExitInstance();
}


//BEGIN_MESSAGE_MAP(CDeployRptThread, CWinThread)
//END_MESSAGE_MAP()

// Implementation

// TEMPLATES

//
//  Функция определяет, были ли ранее опубликованы некоторые из используемых в программе отчетов
//

CString CDeployRptThread::CheckReportsExists()
{
	/*
	IRptManPtr piMan = GetRptMan();
	if(!piMan)
		return _T("");

	HRESULT hr = S_OK;
	
	CString strExist;
    try
    {
	    CString strFullUrl = m_strReportsServerURL + cszReportingServiceASPApplicationName;
		piMan->SetUrl((LPCTSTR)strFullUrl);
		_bstr_t bstrReportList = piMan->GetReportList((bstr_t)m_strReportsFolder);
        CString strRepList = (const TCHAR*) bstrReportList;

		for (CConfigXMLData::ReportInfo::Vector::iterator it = m_arrReportsInfo.begin(); it != m_arrReportsInfo.end(); ++it)
        {
			const CString& strNewReportName = (*it)->m_strReportType;
			const CString& strNewReportVisibleName = (*it)->m_strVisibleName;
			if ( strRepList.Find( strNewReportName ) != -1 )
            {
				if ( strExist.GetLength() > 0 )
				{
					strExist += _T("; ");
				}
				strExist += strNewReportVisibleName;
            }
        }

		return strExist;
	}
    catch(CMNException* pEx)
    {   
		CString strError = CMNException::GetAllErrorMessages(pEx, _T("\n"));
        CMNLog(_T("[CDeployRptThread::CheckReportsExists]: %s"), strError);
		pEx->Delete();
    }
	*/
	return _T("");
}

void CDeployRptThread::DeployReports()
{
	/*
	IRptManPtr piMan = GetRptMan();
	if(!piMan)
		return ;

	ResetReportServerSecurityLevel();

	HRESULT		hr = S_OK;
	BOOL		bError = FALSE;

	m_strReportsList = _T("");
	m_dwReports = 0;

	CString strFullUrl = m_strReportsServerURL + cszReportingServiceASPApplicationName;
	piMan->SetUrl((LPCTSTR)strFullUrl);

	int iTotalSteps = m_arrReportsInfo.size();

	//  1-й параметр - название папки на RS для публикации, второй и третий - источник данных
	piMan->DeployDataSource((bstr_t) m_strReportsFolder, (bstr_t) m_strSQLServer, (bstr_t) m_strDbName);
	::PostMessage(m_hParentWnd, WM_OCRCFG_DEPLOYRPT_STAGE, 0, iTotalSteps);

    int iCurrentStep = 1;
	for(CConfigXMLData::ReportInfo::Vector::iterator it = m_arrReportsInfo.begin(); it != m_arrReportsInfo.end(); ++it, iCurrentStep++)
	{
		CConfigXMLData::ReportInfo* pInfo = *it;

		//[d.titov]
		// we check some pathes: 
		// 1. pInfo->m_strRdlFileDirPath - the path from where config file was loaded
		// 2. m_strReportsDir - path from reg
		// 3. m_strReportsDir2 - 
		CString strReportDir;
		CString strReportFullPath = CMNGetFullFilePathFromRelativeDir(pInfo->m_strRdlFileDirPath, pInfo->m_strRdlFilePath);
		CFile file;
		CFileException ex;
		if(FALSE != file.Open(strReportFullPath, CFile::modeRead | CFile::shareDenyWrite, &ex))
		{
			strReportDir = pInfo->m_strRdlFileDirPath;
			file.Close();
		}
		else
		{
			CString strAlternateReportPath = CMNGetFullFilePathFromRelativeDir(m_strReportsDir, pInfo->m_strRdlFilePath);
			if(FALSE != file.Open(strAlternateReportPath, CFile::modeRead | CFile::shareDenyWrite, &ex))
			{
				strReportDir = m_strReportsDir;
				file.Close();
			}
			else
			{
				CString strAlternateReportPath = CMNGetFullFilePathFromRelativeDir(m_strReportsDir2, pInfo->m_strRdlFilePath);
				if(FALSE != file.Open(strAlternateReportPath, CFile::modeRead | CFile::shareDenyWrite, &ex))
				{
					strReportDir = m_strReportsDir2;
					file.Close();
				}
				else
				{
					// [titov.d] 
					// the part of code below breaks the process of reporting of files if any of listed files
					// was not found. bug 1547.
					// I am not sure it should work in new way. The new way code below cuurrent block
					// Reason: if it can't find some files user should press "ok" for each file instead of immediately eliminate the cause
					//CString strErr = CMNFormatString(ex.m_lOsError);
					//CMNException* pE = new CMNException(IDE_FILEOPERATION_ERROR);
					//*pE << strErr;
					//*pE << strReportFullPath;
					//ThrowException(pE);

					TException tex(IDE_OCRCFG_RPT_FILE_NOT_FOUND);
					tex << strReportFullPath;
					tex.ReportError();
					
					continue;
				}
			}
		}

		if(!strReportDir.IsEmpty())
		{
			piMan->DeployReport((LPCTSTR) pInfo->m_strVisibleName,
				(LPCTSTR) strReportDir,
				(LPCTSTR) pInfo->m_strRdlFilePath,
				(LPCTSTR) pInfo->m_strReportFolder);
		}

		m_strReportsList += _T("- ") + pInfo->m_strVisibleName + _T("\r\n");
		m_dwReports++;

		::PostMessage(m_hParentWnd, WM_OCRCFG_DEPLOYRPT_STAGE, iCurrentStep, iTotalSteps );
	}

	// treat XML body for each report pack
	for(std::set<CString>::iterator it_XMLBody = m_setXMLBody.begin(); it_XMLBody != m_setXMLBody.end(); ++it_XMLBody)
	{
		m_strXMLBody = *it_XMLBody;
		if ( ! m_strXMLBody.IsEmpty() )
		{
			m_storage.Connect(cszDefSAMConnectionString, m_strSQLServer, m_strDbName);
			m_storage.RegisterNewReports(m_strXMLBody);
			m_storage.Disconnect();
			iCurrentStep++;
		}
	}
	*/
}


void CDeployRptThread::DeleteReports()
{
	/*
	IRptManPtr piMan = GetRptMan();
	if(!piMan)
		return ;

	HRESULT hr = S_OK;

	CString strFullUrl = m_strReportsServerURL + cszReportingServiceASPApplicationName;
	piMan->SetUrl((LPCTSTR)strFullUrl);

	for(CConfigXMLData::ReportInfo::Vector::iterator it = m_arrReportsInfo.begin(); it != m_arrReportsInfo.end(); ++it)
		DeleteReport(piMan, (*it)->m_strVisibleName);
		*/
}

/*
void CDeployRptThread::DeleteReport(IRptManPtr piMan, CString const& strReportName)
{
	if(piMan)
	{
       	piMan->DeleteRepIfExist((bstr_t ) m_strReportsFolder, (bstr_t) strReportName);
	}
}
*/
/*
IRptManPtr CDeployRptThread::GetRptMan()
{
	return theApp.GetRptMan();
}
*/

void CDeployRptThread::ResetReportServerSecurityLevel()
{
	CString strMachine;
	strMachine = m_strSQLServer.Left( m_strSQLServer.Find(_T("\\")) );
	if ( strMachine == _T(".") )
	{
		strMachine = _T("");
	}

	bstr_t bstrNamespace = m_strReportServerWMINamespace;

	if ( bstrNamespace.length() == 0 )
	{
		if ( theApp.m_bOSType )
		{
			bstrNamespace = "root\\Microsoft\\SqlServer\\ReportServer\\RS_SQLEXPRESS\\v10";
		}
		else
		{
			bstrNamespace = "root\\Microsoft\\SqlServer\\ReportServer\\v9";
		}
	}

	bstrNamespace += "\\Admin";

	try
	{
		RGSWMIHelper wmi;

		bstr_t bstrComputer = (const TCHAR*) strMachine;
		wmi.connectMultipleTry( bstrComputer, bstrNamespace, 2);

		TWMITableList& rWmiTableList = wmi.ExecQuery( bstr_t("SELECT * FROM MSReportServer_ConfigurationSetting WHERE InstanceName=\"SQLEXPRESS\"") );

		if ( ! rWmiTableList.empty() )
		{
			variant_t vCurrentLevel =  wmi.GetPropertyValue(*rWmiTableList.begin(), "SecureConnectionLevel");

			long current_level = (long) vCurrentLevel;

			if ( current_level )
			{
				bstr_t bstrClassName	= wmi.GetPropertyValue(*rWmiTableList.begin(), "__CLASS");
				bstr_t bstrObjectPath	= wmi.GetPropertyValue(*rWmiTableList.begin(), "__RELPATH");

				TWMIPropertyMap in_params, out_params;
				in_params["Level"] = (long) 0;
				wmi.ExecWMIMethod( bstrClassName, bstrObjectPath, _T("SetSecureConnectionLevel"), in_params, out_params);
			}
		}

	}
	catch (CMNException* pEx)
	{
		CString strErr = CMNException::GetAllErrorMessages(pEx);
		pEx->Delete();
	}
	catch (...)
	{
	}
}
