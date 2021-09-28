//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2012 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:		 Sammigrator
//  Cooperation: 
//  Date:        2012-02-14
//--------------------------------------------------------------------------------------------------
//  Description:  Обнаружение локальных и сетевых Sql серверов
//--------------------------------------------------------------------------------------------------
#include "stdafx.h"

#include "db/include/SqlBrowser.h"
#include "scm/include/SchemaDef.h"

#include "db/include/Constants.h"

#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"wininet")

#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include <lm.h>
#include <wininet.h>
#include <stdlib.h>


#include "cmn/include/Log.h"
#include "cmn/include/Misc.h"
#include "cmn/include/Exception.h"
#include "cmn/include/McException.h"

#include "db/include/Storage.h"
#include "rgs/include/wmihelper.h"

#define CLNT_BCAST_EX  	  2
#define CLNT_UCAST_EX  	  3
#define DEFAULT_BUFLEN 	  2048
#define SQL_BROWSER_PORT  1434
#define BROADCAST_ADDRESS 0xffffffff

#define MSG_WAITALL 0x8

const TCHAR cszInstanceName[] = _T("InstanceName");
const TCHAR cszServerName[]   = _T("ServerName");
const TCHAR cszVersion[]   = _T("Version");
const TCHAR cszSeparator[] 	  = _T(";");
const TCHAR cszLocalIP[] 	  = _T("127.0.0.1");
const TCHAR cszSpaseAndBracet[] = _T("   (");

#define mystrlen(a) _tcslen(a) / sizeof(TCHAR)

// ============================================================================================================
CSqlAndReportingServersInfo::CSqlAndReportingServersInfo()
{
	//Инициировать библиотеку WSA __________________
	WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    int iResult = WSAStartup(wVersionRequested, &wsaData);
	if(iResult!=0)
	{
		WSACleanup();
		throw(new CMNException(IDE_DB_WSAINIT_FAILED));
	}

	// get application name, it is used in GetHttpPage function
    wchar_t szPath[MAX_PATH];
    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
    {
		TCHAR strDefault[] = _T("my_app");
		wcscpy_s(m_strAgentName, strDefault);
    }
	else
	{
		wchar_t strDrive[MAX_PATH];
		wchar_t strDir[MAX_PATH];
		wchar_t strExt[MAX_PATH];
		_wsplitpath(szPath, strDrive, strDir, m_strAgentName, strExt);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CSqlAndReportingServersInfo::~CSqlAndReportingServersInfo()
{
	WSACleanup();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ReadLocalSqlServerNames(std::set<CAtlString>& result)
{
	USES_CONVERSION;
	std::set<CAtlString> instanceNames;

	ReadSqlInstanceNamesWithSqlBrowserService( instanceNames, W2A(cszLocalIP) );
	ReadSqlInstanceWithWMI( instanceNames );
	
	if(0 == instanceNames.size())
	{
		//throw(new CMNException(IDE_DB_GETLOCALSQL_FAILED)); //альтернативный способ реагировать на отсутствие локальных SQL-серверов
		return;
	}

	//Получить имя компьютера _________________________________
	char cszHostName[128]="";
	int iHostNameResult=gethostname(cszHostName,sizeof(cszHostName));
		
	if(SOCKET_ERROR==iHostNameResult)
	{
		throw(new CMNException(IDE_DB_GETHOSTNAME_FAILED));
	}

	//Подставить имя компьютера _______________________________
	for(std::set<CAtlString>::iterator it=instanceNames.begin(); it!=instanceNames.end(); it++)
	{
		CAtlString strInstanceName(cszHostName);
		strInstanceName=strInstanceName+"\\"+*it;
		result.insert(strInstanceName);
	}

}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ReadHostReportServerURL(CAtlString strSqlServerName, set<CAtlString>& r_out_setReportServerURL)
{
	CMNLog(_T("[CSqlAndReportingServersInfo::ReadHostReportServerURL]: start search reporting services for %s"), strSqlServerName);

	try
	{
		// parce SQL server name
		CAtlString strInstanceName, strSQLHostName;
		int iPos = strSqlServerName.Find(_T("\\"));
		if( -1 != iPos)
		{
			strInstanceName = strSqlServerName.Mid(iPos + 1);
			strSQLHostName = strSqlServerName.Mid(0, iPos);

			// check strInstanceName for the case using IP address with port
			// the sql server name can be like this 10.4.0.55\SQLSERVERNAME,1444
			iPos = strInstanceName.Find(_T(","));
			if( -1 != iPos)
			{
				strInstanceName = strInstanceName.Mid(0, iPos);
			}
		}
		else
		{
			strSQLHostName = strSqlServerName;
		}
		if(strSQLHostName == _T("."))
		{
			strSQLHostName = _T("localhost");
		}

		CMNLog(_T("[CSqlAndReportingServersInfo::ReadHostReportServerURL]: parsing finish, sql host name is \"%s\",")
			_T("sql instance name is \"%s\""), strSQLHostName, strInstanceName);

		// Connect to server before check url
		SOCKET		s;
		sockaddr_in	adr;
		IN_ADDR addr;
		bool s_ok=CMNResolveHostByName(bstr_t(strSQLHostName), addr);
		if(!s_ok)
		{
			CMNLog(_T("[CSqlAndReportingServersInfo::ReadHostReportServerURL]: Error. CMNResolveHostByName returned false."));
			return;
		}
		char* pChar = inet_ntoa(addr); //для pchLocalIP delete не вызывать. см. msdn

		// create socket
		if (INVALID_SOCKET == (s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP) ) )
		{
			throw(new CMNException(IDE_DB_SOCK_CREATE_ERROR));
		}

		adr.sin_family				= AF_INET;
		adr.sin_addr.s_addr = inet_addr(pChar);
		adr.sin_port				= htons (80);

		// Устанавливаем соединение с сервером
		if (SOCKET_ERROR == connect (s, (sockaddr* )&adr,  sizeof (adr) ) )
		{
			ThrowException(new CMNException(IDE_DB_SOCK_CONNECT_ERROR));
		}

		if (SOCKET_ERROR == closesocket (s) )	// закрываем соединенеие. *
		{
			ThrowException(new CMNException(IDE_DB_SOCK_CLOSE_ERROR));
		}


		// Start to enum web folsers
		// source of information about names http://msdn.microsoft.com/ru-ru/library/bb677364%28v=sql.100%29.aspx
		std::set<CAtlString> setWebFolderName;
		std::set<CAtlString> setUrl;

		setWebFolderName.insert(L"reportserver"); // SQL version: 2005, 2008, 2012
		setWebFolderName.insert(L"reportserver_SQLExpress"); // SQL version: 2008, 2012
		setWebFolderName.insert(L"reportserver$SQLExpress"); // SQL version: 2005

		if(!strInstanceName.IsEmpty())
		{
			// construct name for report serber webfolder
			setUrl.insert("reportserver_" + strInstanceName); // SQL version: 2008, 2012
			setUrl.insert("reportserver$" + strInstanceName); // SQL version: 2005
		}
		// add defutl names
		setUrl.insert(setWebFolderName.begin(), setWebFolderName.end());


		for( std::set<CAtlString>::iterator it_folder = setUrl.begin(); it_folder != setUrl.end(); it_folder++)
		{
			CAtlString strFinalBuffer;

			GetHttpPage(strSQLHostName, *it_folder, strFinalBuffer);

			if (-1 != strFinalBuffer.Find(CAtlString("Reporting Services")))
			{// We find it!!
				CAtlString strUrl = "http://" + strSQLHostName + "/" + *it_folder; // servername + foldername
				r_out_setReportServerURL.insert(strUrl);
				CMNLog(_T("[CSqlAndReportingServersInfo::ReadHostReportServerURL]: server was found, server name is \"%s\""),
					strUrl);
			}

		}
	}
	catch(CMNException* pEx)
	{
		CAtlString strErrMessage = CMNException::GetAllErrorMessages(pEx);
		CMNLog(_T("[CSqlAndReportingServersInfo::ReadHostReportServerURL]: %s"), strErrMessage);
		pEx->Delete();
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ReadSqlHostNames( std::set<CAtlString>& result )
{
	CMNLog(_T("[CSqlAndReportingServersInfo::ReadSqlHostNames] Start"));
	LPSERVER_INFO_100 pBuf = NULL;
    DWORD dwLevel = 100;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwTotalCount = 0;
    DWORD dwServerType = SV_TYPE_SQLSERVER;        
    DWORD dwResumeHandle = 0;
    NET_API_STATUS nStatus;
    LPWSTR pszServerName = NULL;
    LPWSTR pszDomainName = NULL;

	//Запросить имена Sql-хостов _________________________________
	nStatus = NetServerEnum(pszServerName, dwLevel, (LPBYTE *) & pBuf, dwPrefMaxLen, &dwEntriesRead,
																&dwTotalEntries, dwServerType, pszDomainName, &dwResumeHandle);

	if(nStatus!=NERR_Success)
	{
		throw(new CMNException(IDE_DB_SERVERENUM_FAILED));
	}

	//Сформировать результат ___________________________________
	for(int i=0;i<dwEntriesRead;i++)
	{
		result.insert(pBuf->sv100_name);
		pBuf++;
	}

	CMNLog(_T("[CSqlAndReportingServersInfo::ReadSqlHostNames] Complete"));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ExtractSqlInstanceName( std::set<CAtlString>& result, const char* chSqlBrowserResponse, int iResponseLength)
{
	//Преобразовать chSqlBrowserResponse в CAtlString отрезав первые три символа 
	//префикс(1байт)+длина ответа(2байта)
	CAtlString cstrResponse(chSqlBrowserResponse+3, iResponseLength-3);

	int iFirst=cstrResponse.Find(cszInstanceName);
	while(iFirst!=-1)
	{
		iFirst+=_tcslen(cszInstanceName)+1;
		int iEnd=cstrResponse.Find(cszSeparator, iFirst+1);
		CAtlString intanceName=cstrResponse.Mid(iFirst, iEnd-iFirst);
		if(intanceName.GetLength()>0)
		{
			result.insert(intanceName);
		}

		iFirst=cstrResponse.Find(cszInstanceName, iEnd);
	}

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ExtractSqlInstanceNameAndVersion( std::set<CAtlString>& result, const char* chSqlBrowserResponse, int iResponseLength)
{
	//Преобразовать chSqlBrowserResponse в CAtlString отрезав первые три символа 
	//префикс(1байт)+длина ответа(2байта)
	CAtlString cstrResponse(chSqlBrowserResponse+3, iResponseLength-3);

	int iFirst=cstrResponse.Find(cszInstanceName);
	while(iFirst!=-1)
	{
		//cszVersion
		// read instance name
		iFirst += _tcslen(cszInstanceName)+1;
		int iEnd = cstrResponse.Find(cszSeparator, iFirst+1);
		CAtlString strName = cstrResponse.Mid(iFirst, iEnd-iFirst);

		// read version
		iFirst=cstrResponse.Find(cszVersion, iEnd);
		iFirst += _tcslen(cszVersion)+1;
		iEnd = cstrResponse.Find(cszSeparator, iFirst+1);
		CAtlString strVersion = cstrResponse.Mid(iFirst, iEnd-iFirst);;

		// store
		if(strName.GetLength() > 0 && strVersion.GetLength() > 0)
		{
			result.insert(strName + cszSpaseAndBracet + strVersion + _T(")"));
		}
		else
		{
			strVersion = _T(" "); // space is nesessary for futher parsing
			result.insert(strName);
		}

		iFirst=cstrResponse.Find(cszInstanceName, iEnd);
	}

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ExtractSqlInstanceName( CAtlString& routstrHostName, std::set<CAtlString>& routResult, const char* chSqlBrowserResponse, int iResponseLength)
{
	//Преобразовать chSqlBrowserResponse в CAtlString отрезав первые три символа 
	//префикс(1байт)+длина ответа(2байта)
	CAtlString cstrResponse(chSqlBrowserResponse+3, iResponseLength-3);
	
	//Получить имя хоста
	int iFirst = cstrResponse.Find(cszServerName);
	iFirst+=_tcslen(cszServerName)+1;
	int iEnd=cstrResponse.Find(cszSeparator, iFirst+1);
	routstrHostName = cstrResponse.Mid(iFirst, iEnd-iFirst);

	//Получить имена экземпляров
	iFirst=cstrResponse.Find(cszInstanceName);
	while(iFirst!=-1)
	{
		iFirst += _tcslen( cszInstanceName ) + 1;
		int iEnd = cstrResponse.Find( cszSeparator, iFirst + 1 );
		CAtlString strIntanceName = cstrResponse.Mid( iFirst, iEnd - iFirst );
		if( strIntanceName.GetLength() > 0 )
		{
			CAtlString strFullName = routstrHostName + "\\" + strIntanceName;
			routResult.insert(strFullName);
		}

		iFirst=cstrResponse.Find(cszInstanceName, iEnd);
	}

}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ExtractSqlInformName( std::map<CAtlString, CAtlString>& rin_result, 
	const char* chSqlBrowserResponse, int iResponseLength )
{
	//Преобразовать chSqlBrowserResponse в CAtlString отрезав первые три символа 
	//префикс(1байт)+длина ответа(2байта)
	CAtlString cstrResponse(chSqlBrowserResponse+3, iResponseLength-3);

	int iFirst=cstrResponse.Find(cszInstanceName);
	while(iFirst!=-1)
	{
		//cszVersion
		// read instance name
		iFirst += _tcslen(cszInstanceName)+1;
		int iEnd = cstrResponse.Find(cszSeparator, iFirst+1);
		CAtlString strName = cstrResponse.Mid(iFirst, iEnd-iFirst);

		// read version
		iFirst=cstrResponse.Find(cszVersion, iEnd);
		iFirst += _tcslen(cszVersion)+1;
		iEnd = cstrResponse.Find(cszSeparator, iFirst+1);
		CAtlString strVersion = cstrResponse.Mid(iFirst, iEnd-iFirst);;

		// store
		if(strName.GetLength() > 0 && strVersion.GetLength() > 0)
		{
			rin_result.insert(std::pair<CAtlString, CAtlString>(strName, strVersion));
		}

		iFirst=cstrResponse.Find(cszInstanceName, iEnd);
	}

}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ExtractSqlInstanceNameAndVersion( CAtlString& routstrHostName, 
	std::set<CAtlString>& routResult, const char* chSqlBrowserResponse, int iResponseLength)
{
	//Преобразовать chSqlBrowserResponse в CAtlString отрезав первые три символа 
	//префикс(1байт)+длина ответа(2байта)
	CAtlString cstrResponse(chSqlBrowserResponse+3, iResponseLength-3);
	
	//Получить имя хоста
	int iFirst = cstrResponse.Find(cszServerName);
	iFirst+=_tcslen(cszServerName)+1;
	int iEnd=cstrResponse.Find(cszSeparator, iFirst+1);
	routstrHostName = cstrResponse.Mid(iFirst, iEnd-iFirst);

	//Получить имена экземпляров
	iFirst=cstrResponse.Find(cszInstanceName);
	while(iFirst!=-1)
	{
		//cszVersion
		// read instance name
		iFirst += _tcslen(cszInstanceName)+1;
		int iEnd = cstrResponse.Find(cszSeparator, iFirst+1);
		CAtlString strName = cstrResponse.Mid(iFirst, iEnd-iFirst);

		// read version
		iFirst=cstrResponse.Find(cszVersion, iEnd);
		iFirst += _tcslen(cszVersion)+1;
		iEnd = cstrResponse.Find(cszSeparator, iFirst+1);
		CAtlString strVersion = cstrResponse.Mid(iFirst, iEnd-iFirst);;

		// store
		if(strName.GetLength() > 0 && strVersion.GetLength() > 0)
		{
			routResult.insert(routstrHostName + _T("\\") + strName + cszSpaseAndBracet + strVersion + _T(")"));
		}
		else
		{
			routResult.insert(routstrHostName + _T("\\") + strName);
		}

		iFirst=cstrResponse.Find(cszInstanceName, iEnd);
	}
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ReadSqlInstanceNamesWithSqlBrowserService(set<CAtlString>& routsetResult )
{
	CMNLog(_T("[CSqlAndReportingServersInfo::ReadSqlInstanceNamesWithSqlBrowserService] Start"));

	try{

		//Подключиться к SqlBrowser _____________________________________________________________________

		// Создаем сокет для работы с broadcast
		SOCKET sockbrcast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(sockbrcast == INVALID_SOCKET) 
		{
			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}
  
		// Структура для отправки broadcast
		struct sockaddr_in addrbrcast_snd;
		memset(&addrbrcast_snd, 0, sizeof(addrbrcast_snd));
		addrbrcast_snd.sin_family 	   = AF_INET;
		addrbrcast_snd.sin_port 	   = htons(SQL_BROWSER_PORT);
		addrbrcast_snd.sin_addr.s_addr = htonl(BROADCAST_ADDRESS);
	
		// Разрешаем broadcast на сокете
		const char chAccess = 1;
		if(setsockopt(sockbrcast, SOL_SOCKET, SO_BROADCAST, &chAccess, sizeof(chAccess)) <0 )
		{
			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}
	
		// Посылаем broadcast
		char chMessage = CLNT_BCAST_EX;
		if(sendto(sockbrcast, &chMessage, sizeof(chMessage) , 0, (SOCKADDR*) &addrbrcast_snd, sizeof(addrbrcast_snd)) < 0) 
		{
  			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}
	
		//Перевод сокета в асинхронный режим (неблокирующий)
		ULONG ulMode=1;
		ioctlsocket(sockbrcast, FIONBIO, &ulMode); 
	
		// Создаем множество прослушивания
		fd_set readset;
		FD_ZERO(&readset);
		FD_SET(sockbrcast, &readset);
	
		// Таймаут
		timeval timeout;
		timeout.tv_sec 	= 0;
		timeout.tv_usec = 500000;
	
		sockaddr_in SenderAddr;
		int nSenderAddrSize = sizeof(SenderAddr);
	
		char recvbuf[DEFAULT_BUFLEN];
		int recvbuflen = DEFAULT_BUFLEN;
	
		std::set<sockaddr_in> addrs;
	
		// Создаем список серверов servers
		while(select(sockbrcast, &readset, NULL, &readset, &timeout) > 0) 
		{
		  int iReciveCount = recvfrom(sockbrcast, recvbuf, recvbuflen, 0, (SOCKADDR *) &SenderAddr, &nSenderAddrSize);
		  CAtlString strHostName;
		  set<CAtlString> setInstanceNames;
		  ExtractSqlInstanceName(strHostName, setInstanceNames, recvbuf,iReciveCount);

		  //Удалить имя хоста если есть
		  set<CAtlString>::const_iterator it = routsetResult.find( strHostName );
		  if( it != routsetResult.end() )
		  {
			  routsetResult.erase(it);
		  }

		  //Добавить имена экземпляров
		  routsetResult.insert( setInstanceNames.begin(), setInstanceNames.end() );
		}
	
		CMNLog(_T("[CSqlAndReportingServersInfo::ReadSqlInstanceNamesWithSqlBrowserService] Complete"));

	}
	catch(CMNException* pEx)
	{
		CAtlString strErrMessage = CMNException::GetAllErrorMessages(pEx);
		CMNLog(_T("[CSqlAndReportingServersInfo::ReadSqlInstanceNamesWithSqlBrowserService]: %s"), strErrMessage);
		delete pEx;
	}
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ReadSqlInstanceNamesAndVersionWithSqlBrowserService( set<CAtlString>& routsetResult )
{
	CMNLog(_T("[CSqlAndReportingServersInfo::ReadSqlInstanceNamesWithSqlBrowserService] Start"));

	try{

		//Подключиться к SqlBrowser _____________________________________________________________________

		// Создаем сокет для работы с broadcast
		SOCKET sockbrcast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(sockbrcast == INVALID_SOCKET) 
		{
			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}
  
		// Структура для отправки broadcast
		struct sockaddr_in addrbrcast_snd;
		memset(&addrbrcast_snd, 0, sizeof(addrbrcast_snd));
		addrbrcast_snd.sin_family 	   = AF_INET;
		addrbrcast_snd.sin_port 	   = htons(SQL_BROWSER_PORT);
		addrbrcast_snd.sin_addr.s_addr = htonl(BROADCAST_ADDRESS);
	
		// Разрешаем broadcast на сокете
		const char chAccess = 1;
		if(setsockopt(sockbrcast, SOL_SOCKET, SO_BROADCAST, &chAccess, sizeof(chAccess)) <0 )
		{
			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}
	
		// Посылаем broadcast
		char chMessage = CLNT_BCAST_EX;
		if(sendto(sockbrcast, &chMessage, sizeof(chMessage) , 0, (SOCKADDR*) &addrbrcast_snd, sizeof(addrbrcast_snd)) < 0) 
		{
  			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}
	
		//Перевод сокета в асинхронный режим (неблокирующий)
		ULONG ulMode=1;
		ioctlsocket(sockbrcast, FIONBIO, &ulMode); 
	
		// Создаем множество прослушивания
		fd_set readset;
		FD_ZERO(&readset);
		FD_SET(sockbrcast, &readset);
	
		// Таймаут
		timeval timeout;
		timeout.tv_sec 	= 0;
		timeout.tv_usec = 500000;
	
		sockaddr_in SenderAddr;
		int nSenderAddrSize = sizeof(SenderAddr);
	
		char recvbuf[DEFAULT_BUFLEN];
		int recvbuflen = DEFAULT_BUFLEN;
	
		std::set<sockaddr_in> addrs;
	
		// Создаем список серверов servers
		while(select(sockbrcast, &readset, NULL, &readset, &timeout) > 0) 
		{
		  int iReciveCount = recvfrom(sockbrcast, recvbuf, recvbuflen, 0, (SOCKADDR *) &SenderAddr, &nSenderAddrSize);
		  CAtlString strHostName;
		  set<CAtlString> setInstanceNames;
		  ExtractSqlInstanceNameAndVersion(strHostName, setInstanceNames, recvbuf,iReciveCount);

		  //Удалить имя хоста если есть
		  set<CAtlString>::const_iterator it = routsetResult.find( strHostName );
		  if( it != routsetResult.end() )
		  {
			  routsetResult.erase(it);
		  }

		  //Добавить имена экземпляров
		  routsetResult.insert( setInstanceNames.begin(), setInstanceNames.end() );
		}
	
		CMNLog(_T("[CSqlAndReportingServersInfo::ReadSqlInstanceNamesWithSqlBrowserService] Complete"));

	}
	catch(CMNException* pEx)
	{
		CAtlString strErrMessage = CMNException::GetAllErrorMessages(pEx);
		CMNLog(_T("[CSqlAndReportingServersInfo::ReadSqlInstanceNamesWithSqlBrowserService]: %s"), strErrMessage);
		delete pEx;
	}
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ReadSqlInstanceNamesWithSqlBrowserService(set<CAtlString>& result, const char*  pchIP)
{
	try{
		//Подключиться к SqlBrowser _____________________________________________________________________

		//Создать сокет _________________________________
		SOCKET sock = INVALID_SOCKET;
		int iFamily = AF_INET;
		int iType = SOCK_DGRAM;
		int iProtocol = IPPROTO_UDP;
	
		sock = socket(iFamily, iType, iProtocol);

		if(sock == INVALID_SOCKET)
		{
			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}
	
		//Установить соединение _________________________________________________
		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(pchIP);
		clientService.sin_port = htons(SQL_BROWSER_PORT);

		int iConnectResult = connect(sock, (SOCKADDR *) & clientService, sizeof (clientService));

		if (iConnectResult == SOCKET_ERROR) 
		{
			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}

		//Послать запрос ________________________________________________________
		char chRequest(CLNT_UCAST_EX);
		int  iRequestResult = send( sock, &chRequest, sizeof(chRequest), 0 );

		if (iRequestResult == SOCKET_ERROR) {
			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}

		//Получить ответ _________________________________________________________________________
			char recvbuf[DEFAULT_BUFLEN];
			int recvbuflen = DEFAULT_BUFLEN;
	
			//Ожидание приема __________________________________________	
				u_long iMode=1;
				int iUnblockResult=ioctlsocket(sock, FIONBIO, &iMode); //Перевод сокета в асинхронный режим
		
				timeval time_out; // Таймаут
				time_out.tv_sec = 0; 
				time_out.tv_usec = 500000; //Таймаут 0.5 секунды.
	
				fd_set read_s;  //Задаем множество сокетов 
				FD_ZERO (&read_s); // Обнуляем множество
				FD_SET (sock, &read_s); // Заносим в него сокет 

				//Ждем
				if (SOCKET_ERROR == (select (0, &read_s, 0, 0, &time_out) ) )
				{
					throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
				}

			int iReciveCount = recv(sock, recvbuf, recvbuflen, 0);
	 
			if(iReciveCount == SOCKET_ERROR)
			{
				CMNException *pEx=new CMNException(IDE_DB_SQLBROWSER_FAILED);
				throw(pEx);
			}

		ExtractSqlInstanceName(result, recvbuf,iReciveCount);
	

		//Закрыть соединение ____________________________________________________
		int iCloseResult = closesocket(sock);
		if (iCloseResult == SOCKET_ERROR) 
		{
			CMNCheckWIN32Error( WSAGetLastError() );
		}
	}
	catch(CMNException* pEx)
	{
		CAtlString strErrMessage = CMNException::GetAllErrorMessages(pEx);
		CMNLog(_T("[CSqlAndReportingServersInfo::ReadSqlInstanceNamesWithSqlBrowserService]: %s"), strErrMessage);
		delete pEx;
	}

}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ReadSqlInstanceNamesAndVerWithSqlBrowserService(set<CAtlString>& result, const char*  pchIP)
{
	try{
		//Подключиться к SqlBrowser _____________________________________________________________________

		//Создать сокет _________________________________
		SOCKET sock = INVALID_SOCKET;
		int iFamily = AF_INET;
		int iType = SOCK_DGRAM;
		int iProtocol = IPPROTO_UDP;
	
		sock = socket(iFamily, iType, iProtocol);

		if(sock == INVALID_SOCKET)
		{
			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}
	
		//Установить соединение _________________________________________________
		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(pchIP);
		clientService.sin_port = htons(SQL_BROWSER_PORT);

		int iConnectResult = connect(sock, (SOCKADDR *) & clientService, sizeof (clientService));

		if (iConnectResult == SOCKET_ERROR) 
		{
			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}

		//Послать запрос ________________________________________________________
		char chRequest(CLNT_UCAST_EX);
		int  iRequestResult = send( sock, &chRequest, sizeof(chRequest), 0 );

		if (iRequestResult == SOCKET_ERROR) {
			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}

		//Получить ответ _________________________________________________________________________
			char recvbuf[DEFAULT_BUFLEN];
			int recvbuflen = DEFAULT_BUFLEN;
	
			//Ожидание приема __________________________________________	
				u_long iMode=1;
				int iUnblockResult=ioctlsocket(sock, FIONBIO, &iMode); //Перевод сокета в асинхронный режим
		
				timeval time_out; // Таймаут
				time_out.tv_sec = 0; 
				time_out.tv_usec = 500000; //Таймаут 0.5 секунды.
	
				fd_set read_s;  //Задаем множество сокетов 
				FD_ZERO (&read_s); // Обнуляем множество
				FD_SET (sock, &read_s); // Заносим в него сокет 

				//Ждем
				if (SOCKET_ERROR == (select (0, &read_s, 0, 0, &time_out) ) )
				{
					throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
				}

			int iReciveCount = recv(sock, recvbuf, recvbuflen, 0);
	 
			if(iReciveCount == SOCKET_ERROR)
			{
				CMNException *pEx=new CMNException(IDE_DB_SQLBROWSER_FAILED);
				throw(pEx);
			}

		ExtractSqlInstanceNameAndVersion(result, recvbuf,iReciveCount);
	

		//Закрыть соединение ____________________________________________________
		int iCloseResult = closesocket(sock);
		if (iCloseResult == SOCKET_ERROR) 
		{
			CMNCheckWIN32Error( WSAGetLastError() );
		}
	}
	catch(CMNException* pEx)
	{
		CAtlString strErrMessage = CMNException::GetAllErrorMessages(pEx);
		CMNLog(_T("[CSqlAndReportingServersInfo::ReadSqlInstanceNamesWithSqlBrowserService]: %s"), strErrMessage);
		delete pEx;
	}

}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ReadSqlInstanceWithWMI( set<CAtlString>& result )
{
	try
	{
		RGSWMIHelper wmi;
		const CString strDefaultInstance = _T("MSSQLSERVER");

		//Получить имя локального компьютера
		bstr_t bstrComputer = "";
		char cszHostName[128]="";
		int iHostNameResult=gethostname(cszHostName,sizeof(cszHostName));
		if(iHostNameResult==S_OK)
		{
			bstrComputer=bstr_t(cszHostName);
		}

		
		bstr_t bstrBaseSQLNamespace = "root\\Microsoft\\SqlServer";
		wmi.connect("\\\\"+bstrComputer, bstrBaseSQLNamespace);
		TWMITableList rWmiTableList = wmi.ExecQuery( bstr_t("SELECT * FROM __NAMESPACE where NAME like \"ComputerManagement%\"") );
		list<bstr_t> namespacesList;
		for (TWMITableListIterator it = rWmiTableList.begin(); it != rWmiTableList.end(); ++it)
		{
			bstr_t bstrNsName = wmi.GetPropertyStrValue(*it, _T("Name"));
			if ( bstrNsName.length() > 0 )
			{
				namespacesList.push_back( bstrNsName );
			}
		}
		for (list<bstr_t>::iterator it = namespacesList.begin(); it != namespacesList.end(); ++it)
		{
			bstr_t bstrName = *it;
			//
			wmi.connect("\\\\"+bstrComputer, bstrBaseSQLNamespace + bstr_t("\\") + bstrName);
			//
			TWMITableList rServerSettings = wmi.ExecQuery( bstr_t("SELECT * FROM SqlService WHERE SqlServiceType=1") );
			for(TWMITableListIterator itSettings = rServerSettings.begin(); itSettings != rServerSettings.end(); ++itSettings)
			{
				CAtlString strInstance( (TCHAR*)wmi.GetPropertyStrValue(*itSettings, _T("ServiceName") ) );
				if ( strInstance.GetLength() > 0 )
				{
					int iFirst= strInstance.Find(_T("$"))+1;
					strInstance = strInstance.Mid( iFirst, strInstance.GetLength()-iFirst );
					result.insert( strInstance );
				}
			}
		}
	}
	catch(CMNException* pEx)
	{
		CString strErr = CMNException::GetAllErrorMessages(pEx);
		CMNLog( _T("[CSqlAndReportingServersInfo::ReadSqlInstanceWithWMI]: %s"), strErr );
		pEx->Delete();
	}
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ReadSqlInformationWithSqlBrowserService(std::map<CAtlString, CAtlString>& rin_result, 
	const char* pchIP )
{
	try{
		//Подключиться к SqlBrowser _____________________________________________________________________

		//Создать сокет _________________________________
		SOCKET sock = INVALID_SOCKET;
		int iFamily = AF_INET;
		int iType = SOCK_DGRAM;
		int iProtocol = IPPROTO_UDP;
	
		sock = socket(iFamily, iType, iProtocol);

		if(sock == INVALID_SOCKET)
		{
			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}
	
		//Установить соединение _________________________________________________
		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(pchIP);
		clientService.sin_port = htons(SQL_BROWSER_PORT);

		int iConnectResult = connect(sock, (SOCKADDR *) & clientService, sizeof (clientService));

		if (iConnectResult == SOCKET_ERROR) 
		{
			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}

		//Послать запрос ________________________________________________________
		char chRequest(CLNT_UCAST_EX);
		int  iRequestResult = send( sock, &chRequest, sizeof(chRequest), 0 );

		if (iRequestResult == SOCKET_ERROR) {
			throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
		}

		//Получить ответ _________________________________________________________________________
			char recvbuf[DEFAULT_BUFLEN];
			int recvbuflen = DEFAULT_BUFLEN;
	
			//Ожидание приема __________________________________________	
				u_long iMode=1;
				int iUnblockResult=ioctlsocket(sock, FIONBIO, &iMode); //Перевод сокета в асинхронный режим
		
				timeval time_out; // Таймаут
				time_out.tv_sec = 0; 
				time_out.tv_usec = 500000; //Таймаут 0.5 секунды.
	
				fd_set read_s;  //Задаем множество сокетов 
				FD_ZERO (&read_s); // Обнуляем множество
				FD_SET (sock, &read_s); // Заносим в него сокет 

				//Ждем
				if (SOCKET_ERROR == (select (0, &read_s, 0, 0, &time_out) ) )
				{
					throw(new CMNException(IDE_DB_SQLBROWSER_FAILED));
				}

			int iReciveCount = recv(sock, recvbuf, recvbuflen, 0);
	 
			if(iReciveCount == SOCKET_ERROR)
			{
				CMNException *pEx=new CMNException(IDE_DB_SQLBROWSER_FAILED);
				throw(pEx);
			}

		ExtractSqlInformName(rin_result, recvbuf, iReciveCount);
	

		//Закрыть соединение ____________________________________________________
		int iCloseResult = closesocket(sock);
		if (iCloseResult == SOCKET_ERROR) 
		{
			CMNCheckWIN32Error( WSAGetLastError() );
		}
	}
	catch(CMNException* pEx)
	{
		CAtlString strErrMessage = CMNException::GetAllErrorMessages(pEx);
		CMNLog(_T("[CSqlAndReportingServersInfo::ReadSqlInstanceNamesWithSqlBrowserService]: %s"), strErrMessage);
		delete pEx;
	}

}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ReadSqlInstanceNamesByHostName( set<CAtlString>& result, CAtlString cstrHostName )
{
	//Получить IP по имени хоста
	in_addr addr;
	bool s_ok=CMNResolveHostByName(bstr_t(cstrHostName), addr);
	
	if(!s_ok){
		throw(new CMNException(IDE_DB_GETIPBYHOSTNAME_FAILED));
	}

	char* pChar= inet_ntoa (addr); //для pchLocalIP delete не вызывать. см. msdn
	
	char pchLocalIP[15];
	strcpy(&pchLocalIP[0],pChar);

	ReadSqlInstanceNamesWithSqlBrowserService(result, &pchLocalIP[0]);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ReadSqlInstanceNamesAndVerByHostName( set<CAtlString>& result, CAtlString cstrHostName )
{
	//Получить IP по имени хоста
	in_addr addr;
	bool s_ok=CMNResolveHostByName(bstr_t(cstrHostName), addr);
	
	if(!s_ok){
		throw(new CMNException(IDE_DB_GETIPBYHOSTNAME_FAILED));
	}

	char* pChar= inet_ntoa (addr); //для pchLocalIP delete не вызывать. см. msdn
	
	char pchLocalIP[15];
	strcpy(&pchLocalIP[0],pChar);

	ReadSqlInstanceNamesAndVerWithSqlBrowserService(result, &pchLocalIP[0]);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool CSqlAndReportingServersInfo::ReadSqlInformationByHostName(std::map<CAtlString, CAtlString>& rin_result, CAtlString cstrHostName )
{
	//Получить IP по имени хоста
	in_addr addr;
	bool s_ok=CMNResolveHostByName(bstr_t(cstrHostName), addr);
	
	if(!s_ok){
		CMNLog(_T("[CSqlAndReportingServersInfo::ReadSqlInformationByHostName]: %s"), IDE_DB_GETIPBYHOSTNAME_FAILED);
		return false;
		//throw(new CMNException(IDE_DB_GETIPBYHOSTNAME_FAILED));
	}

	char* pChar= inet_ntoa (addr); //для pchLocalIP delete не вызывать. см. msdn
	
	char pchLocalIP[15];
	strcpy(&pchLocalIP[0],pChar);

	ReadSqlInformationWithSqlBrowserService(rin_result, &pchLocalIP[0]);

	return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CAtlString CSqlAndReportingServersInfo::GetHostNameByIP(CAtlString& rin_strIP)
{
	vector<CAtlString> vecTemp;
	char host_addr[256]; host_addr[0] = 0;
	struct hostent *remoteHost;
	struct in_addr addr = { 0 };
	if(rin_strIP.IsEmpty())
	{
		throw(new CMNException(IDE_DB_GETIPBYHOSTNAME_FAILED));
	}
	size_t intTemp(0);
	wcstombs_s(&intTemp, host_addr, rin_strIP, (size_t)256);
	addr.s_addr = inet_addr(host_addr);
	remoteHost = gethostbyaddr((char *) &addr, 4, AF_INET);
	if(remoteHost == NULL)
	{
		throw(new CMNException(IDE_DB_GETIPBYHOSTNAME_FAILED));
	}
	vecTemp = CMNSplitString(CAtlString(remoteHost->h_name), _T(".")); // host name example: srv-hyperv.infosec.ru
	if(vecTemp.empty())
	{
		throw(new CMNException(IDE_DB_GETIPBYHOSTNAME_FAILED));
	}

	return *(vecTemp.begin());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool CSqlAndReportingServersInfo::IsIP(const CAtlString& rin_strName) const
{
	// is there are three "." sign?
	CAtlString strTemp(rin_strName);
	int iTemp(0);
	for(int i = 0; i < 3; i++)
	{
		iTemp = strTemp.Find(_T("."));
		if(iTemp != -1)
		{
			strTemp = strTemp.Right(strTemp.GetLength() - iTemp - 1);
		}
		else
		{
			return false;
		}
	}

	return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Следующая версия с широковещательным запросом
void CSqlAndReportingServersInfo::ReadNetworkSqlServerNames(set<CAtlString>& result)
{
	CMNLog( _T("[CSqlAndReportingServersInfo::ReadNetworkSqlServerNames] Start") );
	ReadSqlHostNames( result );
	CMNLog( _T("[CSqlAndReportingServersInfo::ReadNetworkSqlServerNames] HostNamesCount = %u"), result.size() );
	ReadSqlInstanceNamesWithSqlBrowserService( result );
	CMNLog( _T("[CSqlAndReportingServersInfo::ReadNetworkSqlServerNames] InstanceNamesCount = %u"), result.size() );
	CMNLog( _T("[CSqlAndReportingServersInfo::ReadNetworkSqlServerNames] Complete"));
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ReadNetworkSqlServerNamesAndVersion(set<CAtlString>& result)
{
	CMNLog( _T("[CSqlAndReportingServersInfo::ReadNetworkSqlServerNamesAndVersion] Start") );
	ReadSqlHostNames( result );
	CMNLog( _T("[CSqlAndReportingServersInfo::ReadNetworkSqlServerNamesAndVersion] HostNamesCount = %u"), result.size() );
	ReadSqlInstanceNamesAndVersionWithSqlBrowserService( result );
	CMNLog( _T("[CSqlAndReportingServersInfo::ReadNetworkSqlServerNamesAndVersion] InstanceNamesCount = %u"), result.size() );
	CMNLog( _T("[CSqlAndReportingServersInfo::ReadNetworkSqlServerNamesAndVersion] Complete"));
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Прежняя версия с последовательным опросом серверов
/*void CSqlAndReportingServersInfo::ReadNetworkSqlServerNames(set<CAtlString>& result)
{
	std::set<CAtlString> setHostNames;
	ReadSqlHostNames( setHostNames );

	for(std::set<CAtlString>::iterator itHostNames=setHostNames.begin();itHostNames!=setHostNames.end();itHostNames++)
	{
		CString cstrErrMsg;
		try
		{
			std::set<CAtlString> instanceNames;
			ReadSqlInstanceNamesByHostName(instanceNames, (const CAtlString) *itHostNames);
			for(std::set<CAtlString>::iterator itInstanceNames=instanceNames.begin(); itInstanceNames!=instanceNames.end(); itInstanceNames++)
			{
				CAtlString strFullName=*itHostNames+"\\"+*itInstanceNames;
				result.insert(strFullName);
			}
			
		}
		catch(CMNException *ex)
		{
			switch(ex->GetErrorCode())
			{
				case IDE_DB_SQLBROWSER_FAILED:
					cstrErrMsg.LoadString(IDE_DB_SQLBROWSER_FAILED);
					CMNLog(_T("[CSqlAndReportingServersInfo::ReadLocalNetworkSqlServerNames]: %s, %s"), *itHostNames, cstrErrMsg);
				break;

				case IDE_DB_GETIPBYHOSTNAME_FAILED:
					cstrErrMsg.LoadString(IDE_DB_GETIPBYHOSTNAME_FAILED);
					CMNLog(_T("[CSqlAndReportingServersInfo::ReadLocalNetworkSqlServerNames]: %s %s"), cstrErrMsg, *itHostNames);
				break;

				default:
					throw(ex);
				break;
			}
		}
	}
}*/

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::ReadDbNames(set<CAtlString>& result, CAtlString strSqlServerName)
{
	DBStorage storage;

	//Подулючиться к серверу
	storage.Connect(cszDefSAMConnectionString, strSqlServerName, _T("master"));

	//Получить список баз данных
	result = storage.ListDatabases();
	/*
	//Сформировать результат ___________________________
	if(piDatabaseList)
	{
		piDatabaseList->MoveFirst();

		while(!piDatabaseList->adoEOF)
		{
			FieldPtr piField = piDatabaseList->Fields->Item[_T("name")];
			CAtlString strDbName(piField->GetValue());
			result.insert(strDbName);
			piDatabaseList->MoveNext();
		}
	}
	*/
	storage.Disconnect();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CSqlAndReportingServersInfo::GetHttpPage(CAtlString strServerName, CAtlString strWebFolser, CAtlString& strBuf)
{	
	// change web folder name
	strWebFolser = "/" + strWebFolser;

    // initialize WinInet
    HINTERNET hInternet = ::InternetOpen( m_strAgentName, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

    if (hInternet != NULL) 
	{
		// set time out
		DWORD dwTimeOut = 60 * 1000; // In milliseconds
		InternetSetOption( hInternet, INTERNET_OPTION_CONNECT_TIMEOUT,
		&dwTimeOut, sizeof( dwTimeOut ) );
		InternetSetOption( hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT,
		&dwTimeOut, sizeof( dwTimeOut ) );
		InternetSetOption( hInternet, INTERNET_OPTION_SEND_TIMEOUT,
		&dwTimeOut, sizeof( dwTimeOut ) );
		DWORD dwRetries = 5;
		InternetSetOption( hInternet, INTERNET_OPTION_CONNECT_RETRIES,
		&dwRetries, sizeof( dwRetries ) );

        // open HTTP session
        HINTERNET hConnect = ::InternetConnect(hInternet, strServerName, INTERNET_DEFAULT_HTTP_PORT,
                NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1u);

        if (hConnect != NULL) 
		{
            // open request
            HINTERNET hRequest = ::HttpOpenRequest(hConnect, _T("GET"), strWebFolser, NULL,
                    NULL, 0, INTERNET_FLAG_KEEP_CONNECTION, 1);

            if (hRequest != NULL) {
                // send request
                BOOL bSend = ::HttpSendRequest(hRequest, NULL,0, NULL,0);
                if (bSend) 
				{
					// read data
					char  szData[1024];
					DWORD dwBytesRead;
					do
					{
						BOOL bRead = ::InternetReadFile(hRequest, szData, sizeof(szData) - 1,
								&dwBytesRead);

						// break if error occured
						if (bRead != FALSE  ||  dwBytesRead != 0)
						{
							//save  data
							szData[dwBytesRead] = 0;
							strBuf += szData;
						}
						else
						{
							break;
						}
					}
					while(dwBytesRead > 0);
                }
                // close request
                ::InternetCloseHandle(hRequest);
            }
            // close session
            ::InternetCloseHandle(hConnect);
        }
        // close WinInet
        ::InternetCloseHandle(hInternet);
    }
}
