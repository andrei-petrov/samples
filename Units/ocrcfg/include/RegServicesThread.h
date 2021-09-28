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
#include "ocrcfg/include/RegServicesData.h"



class CRegServicesThread : public WZRThread
{
//	DECLARE_DYNCREATE(CRegServicesThread)
protected:
//	CString					m_strComputerName;	
//	CString					m_strServiceLogin;
//	CString					m_strServicePassword;
	
	CDeliceOcrModuleDesc::Vector	m_vecModules;

	COcrConfigData&					m_rConfigData;
	//CConfigXMLData&			m_config;

	int* m_pnModificationFlags; //В этом классе ожидается установка флага IsCreateDB. 
								//См. Constants.h раздел Configurator Modification Flags

	
public:
	CRegServicesThread( HWND hParentWnd, CDeliceOcrModuleDesc::Vector	rvecModules, COcrConfigData& rConfigData, CConfigXMLData& config, int* pnModificationFlags );
	virtual ~CRegServicesThread();

public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();

    
protected:    	
	bool			AreAnyServicesRegistered();
	CString			CheckOurServices(); // это наши сервисы?	
	int				CreateServices();
	int				RemoveServices();

	void			WriteConnectionStringAndFlags(CDeliceOcrModuleDesc::Ptr spDeliceModule);
};


