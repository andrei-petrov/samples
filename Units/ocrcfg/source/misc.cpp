//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        
//--------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------

// misc.h : include file for helper functions

#include "stdafx.h"

#include "CMN/include/Misc.h"
#include "ocrcfg/include/Constants.h"
#include "ocrcfg/include/misc.h"
#include "ocrcfg/include/ConfigApp.h"

const TCHAR cszReportingServiceASPApplicationName[] = _T("/ReportService2005.asmx");


CString SamCfgGetFileFullPath(LPCTSTR lpszRelativeFilePath)
{
	CString strFullFilePath = lpszRelativeFilePath;

	// file path
	if(PathIsRelative(lpszRelativeFilePath))
	{
		TCHAR chCurrentDir[_MAX_PATH];
		::memset(chCurrentDir, 0, sizeof(TCHAR) * _MAX_PATH);

		if(GetCurrentDirectory(_MAX_PATH, chCurrentDir) != 0)
			strFullFilePath = CMNGetFullFilePathFromRelativeDir(chCurrentDir, lpszRelativeFilePath);

		CFile file;
		CFileException ex;
		if(FALSE == file.Open(strFullFilePath, CFile::modeRead | CFile::shareDenyWrite, &ex))
		{
			TCHAR chCurrentDir[_MAX_PATH];
			::memset(chCurrentDir, 0, sizeof(TCHAR) * _MAX_PATH);

			HINSTANCE hInst = AfxGetInstanceHandle();
			CString strDirName;
			if(::GetModuleFileName(hInst, chCurrentDir, _MAX_PATH))
			{
				// get module working directory
				wchar_t wchDrive[_MAX_DRIVE];
				::memset(wchDrive, 0, sizeof(wchDrive));
				wchar_t wchDir[_MAX_PATH];
				::memset(wchDir, 0, sizeof(wchDir));
				wchar_t wchFileName[_MAX_PATH];
				::memset(wchFileName, 0, sizeof(wchFileName));
				wchar_t wchExt[_MAX_PATH];
				::memset(wchExt, 0, sizeof(wchExt));
				_wsplitpath_s(chCurrentDir, wchDrive, _MAX_DRIVE, wchDir, _MAX_PATH, wchFileName, _MAX_PATH, wchExt, _MAX_PATH);

				strDirName.Format(_T("%s%s"), wchDrive, wchDir);
			}

			strFullFilePath = _T("");
			strFullFilePath = CMNGetFullFilePathFromRelativeDir(strDirName, lpszRelativeFilePath);
		}
	}

	return strFullFilePath;
}
