//--------------------------------------------------------------------------------
//  Copyright: (C) 2009-2010 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit: ConfigXMLData.h
//  Cooperation:
//  Date:        2010-01-25
//--------------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------------

#pragma once

#include "CMN/include/Unknown.h"


struct SoftLibraryFileInfoC
{
	SoftLibraryFileInfoC(CString strFile, CString strPath): 
		m_strSoftLibraryFileName(strFile), m_strSoftLibraryFilePath(strPath){}

	CString m_strSoftLibraryFileName;
	CString m_strSoftLibraryFilePath;
};

// Предикат для std::set<> =================================================================================
struct SoftLibraryInfoStructCompare { 
    bool operator() (const SoftLibraryFileInfoC& a, const SoftLibraryFileInfoC& b) const {
		return a.m_strSoftLibraryFileName.CompareNoCase(b.m_strSoftLibraryFileName)<0;
    }
};



//
//		class COcrConfigData
//		used to store and load credentials and SQL server adresses
//



class COcrConfigData
{
public:
	// it may  be some_computer/some_instance or 192.168.86.1/some_instance
	CString		m_strSQLServerAddr;
	// it may  be some_computer or 192.168.86.1 address
	CString		m_strComputerAddr;
	// login as entered
	CString		m_strLogin;
	// password as entered
	CString		m_strPassword;
	// project folder, i.e. //server/CentralServer
	CString		m_strProjectFolder;
	// scans folder, i.e. //scans
	CString		m_strScansFolder;


public:
	COcrConfigData();
	~COcrConfigData();
	void LoadFromRegistry();
	void SaveToRegistry();
};


//
//		class CConfigXMLData
//		used to store information about the sql script pack
//


class CConfigXMLData
{
public:
	class ReportInfo: public CMNUnknown
	{
	public:
		typedef	ReportInfo			Class;
		typedef	CMNSmartPtr<Class>	Ptr;
		typedef	vector<Ptr>			Vector;

	public:
		CString m_strVisibleName;
		CString	m_strReportFolder;
		CString	m_strRdlFilePath;
		CString m_strReportType;
		CString m_strRdlFileHardDriveFullPath;
		CString m_strRdlFileDirPath;

	public:
		ReportInfo(CString const& strVisibleName, CString const& strReportFolder,
					CString const& strRdlFilePath, CString const& strReportType, 
					CString const& strRdlFileHardDrivePath /*= CString("")*/) :
			m_strVisibleName(strVisibleName),
			m_strReportFolder(strReportFolder),
			m_strRdlFilePath(strRdlFilePath),
			m_strReportType(strReportType),
			m_strRdlFileHardDriveFullPath(strRdlFileHardDrivePath)
		{
			if(m_strRdlFileHardDriveFullPath.IsEmpty())
			{
				return;
			}

			// Create all subfolders
			wchar_t drive[_MAX_DRIVE];
			wchar_t dir[_MAX_DIR];
			wchar_t fname[_MAX_FNAME];
			wchar_t ext[_MAX_EXT];
			::_wsplitpath_s(m_strRdlFileHardDriveFullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

			m_strRdlFileDirPath = CString(drive) + CString(dir);
		}
	};

protected:
	//CString	m_strXMLConfigPathToLoad;
	// container for all temporary files which was used for loading settings
	set<CString> c_setXMLConfigPath;
	CString	m_strXMLConfigPath;
	CString m_strXMLBody;
	std::set<CString> m_setXMLBody;

	bool	m_bIsCreateServer;
	bool	m_bIsRunSqlScripts;
	bool	m_bIsDeployReports;
	bool	m_bIsImportSoftLibrary;
	bool    m_bIsEnableClearConfig;

	// Server Data
	CString	m_strSqlServer;
	CString	m_strDBName;

	// Sql Scripts Data
	set<CString>	m_setScriptFilePaths; // first - index, second - file path of the script

	// Reports Data
	CString	m_strReportURL;
	CString	m_strWebFolder;
	ReportInfo::Vector	m_arrReportsInfo;

	// Soft library data
	set<SoftLibraryFileInfoC, SoftLibraryInfoStructCompare> m_setSoftLibraryFiles;

	// temp unpacked archive report pack's folder
	CString m_strReportPackArchiveFolder;

	void LoadReportPackArchive(CString const& strArchivePath);
	void LoadXML(CString const& strXMLConfigPath, bool bIsArchive = false);
	void LoadXmlSoftLibraryFiles(MSXML::IXMLDOMNodePtr spSoftLibraryNode, const wchar_t *strDir);


	//Сравнивает версию КБИ указанную в xml-файле конфигурации с текущей версией КБИ [m.fomin]
	bool CheckProductVersion(const bstr_t version, CString strXMLConfigPath); 

public:
	CConfigXMLData();
	~CConfigXMLData();

	void LoadConfig(CString const& strConfigPath);
	CString GetXMLBody() { return m_strXMLBody; }
	std::set<CString>* GetXMLset() { return &m_setXMLBody; }

	bool IsXMLConfigFileLoaded() { return !m_strXMLConfigPath.IsEmpty(); }
	CString GetLoadedXMLConfigFilePath() { return m_strXMLConfigPath; }
	void ClearConfig();

	void DisableCreateServer();
	void DisableRunSqlScripts();
	void DisableImportSoftLibrary();
	void SetDisableClearConfig(){ m_bIsEnableClearConfig = false;   }

	bool IsCreateServer()		{ return m_bIsCreateServer;			}
	bool IsRunSqlScripts()		{ return m_bIsRunSqlScripts;		}
	bool IsDeployReports()		{ return m_bIsDeployReports;		}
	bool IsImportSoftLibrary()	{ return m_bIsImportSoftLibrary;	}

	// Server Data
	CString GetSQLServer();
	void SetSQLServer(CString const& strSQLServer);
	CString GetDBName();
	void SetDBName(CString const& strDBName);

	// Sql Scripts Data
	set<CString>	const& GetScriptFilePaths();

	// Reports Data
	CString GetConfigFileDir();
	CString	GetReportServerURL();
	CString	GetWebFolder();
	CConfigXMLData::ReportInfo::Vector const& GetReportsInfo();

	//  Soft library data
	set<SoftLibraryFileInfoC, SoftLibraryInfoStructCompare> GetSoftLibraryFiles();

	// Report Pack
	CString GetReportPackArchiveFolder();
	void DeleteReportPackArchiveFolder();
};
