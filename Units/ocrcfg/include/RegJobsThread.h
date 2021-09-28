//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:  Configurator      
//--------------------------------------------------------------------------
//  Description: Thread for background operations
//--------------------------------------------------------------------------

#pragma once

#include "cmn/include/Unknown.h"
#include "db/include/Storage.h"
#include "ocrcfg/include/RegJobsData.h"


//typedef vector<CString> VZStringVector;


class CRegJobsThread : public WZRThread
{
//	DECLARE_DYNCREATE(CRegJobsThread)
protected:
	DBStorage					m_storage;
		
	CDeliceOcrJobDesc::Vector	m_vecJobs;
	COcrConfigData&				m_rConfigData;
	//CConfigXMLData&			m_config;

	int* m_pnModificationFlags; //В этом классе ожидается установка флага IsCreateDB. 
								//См. Constants.h раздел Configurator Modification Flags

	
public:
	CRegJobsThread( HWND hParentWnd, const CDeliceOcrJobDesc::Vector rvecJobs, COcrConfigData& rConfigData, CConfigXMLData& config, int* pnModificationFlags );
	virtual ~CRegJobsThread();

public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();

    bool			AreAnyJobExist();
//	static int		GetJobsCount() { return m_setRecognizableJobs.size(); }

protected:    
	int		CreateJobs();	
	int		RemoveJobs();
	CString CheckOurJobs(); // это наши jobs?	
};


