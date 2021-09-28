//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2012 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:		 Sammigrator
//  Cooperation: 
//  Date:        2012-02-14
//--------------------------------------------------------------------------------------------------
//  Description:  Обнаружение локальных и сетевых Sql серверов
//--------------------------------------------------------------------------------------------------
#pragma once

#include "WinNT.h"

//Класс для запроса информации о серверах ==============================================================
class CSqlAndReportingServersInfo
{
	wchar_t m_strAgentName[MAX_PATH];
public:
	//Конструктор, деструктор 
	CSqlAndReportingServersInfo();
	~CSqlAndReportingServersInfo();

	//В set<> выбран CAtlString т.к. класс будет использован в MFC и ATL проектах.
	//CString имеет разную реализацию в этих библиотеках.

	//Получить список Sql-серверов на локальной машине
	void ReadLocalSqlServerNames( std::set<CAtlString>& result );
	
	//Получить список полных имен Sql-серверов в локальной сети
	//полное имя="имя хоста/имя экземпляра"
	void ReadNetworkSqlServerNames(set<CAtlString>& result);
	void ReadNetworkSqlServerNamesAndVersion(set<CAtlString>& result);
	
	//Получить список БД сервера
	void ReadDbNames( set<CAtlString>& result, CAtlString strSqlServerName );

	// Get list of available URL by default for excact host
	void ReadHostReportServerURL(CAtlString strSqlServerName, set<CAtlString>& r_out_setReportServerURL);

	//Получить список имен SQL-серверов по имени хоста
	void ReadSqlInstanceNamesByHostName( set<CAtlString>& result, CAtlString cstrHostName ); // attention!! throw exception
	void ReadSqlInstanceNamesAndVerByHostName( set<CAtlString>& result, CAtlString cstrHostName ); // attention!! throw exception
	// recive sql instance name and versionnumber by host name
	bool ReadSqlInformationByHostName(std::map<CAtlString, CAtlString>& rin_result, CAtlString cstrHostName ); // attention!! throw exception

	// get host name by it's IP
	CAtlString GetHostNameByIP(CAtlString& rin_strIP);

	bool IsIP(const CAtlString& rin_strName) const;

private:
	//Получить список имен хостов на которых размещены экземпляры Sql-серверов
	void ReadSqlHostNames( set<CAtlString>& result ); 

	//Извлекает имена экземпляров Sql-сервреров из ответа полученного от службы SqlBrowser
	void ExtractSqlInstanceName( std::set<CAtlString>& result, const char* chSqlBrowserResponse, int iResponseLength );
	void ExtractSqlInstanceNameAndVersion( std::set<CAtlString>& result, const char* chSqlBrowserResponse, int iResponseLength );
	void ExtractSqlInstanceName( CAtlString& routstrHostName, std::set<CAtlString>& routResult, const char* chSqlBrowserResponse, int iResponseLength);
	// extrsct instace name and version number
	void ExtractSqlInformName( std::map<CAtlString, CAtlString>& rin_result, const char* chSqlBrowserResponse, int iResponseLength );
	void ExtractSqlInstanceNameAndVersion( CAtlString& routstrHostName, std::set<CAtlString>& routResult, const char* chSqlBrowserResponse, int iResponseLength);

	//Получить список имен SQL-серверов по IP хоста, используя службу SQL Browser
	void ReadSqlInstanceNamesWithSqlBrowserService( set<CAtlString>& result, const char* chIP );
	void ReadSqlInstanceNamesAndVerWithSqlBrowserService(set<CAtlString>& result, const char*  pchIP);
	void ReadSqlInstanceNamesWithSqlBrowserService( set<CAtlString>& routsetResult ); //Используя широковещательный запрос
	void ReadSqlInstanceNamesAndVersionWithSqlBrowserService( set<CAtlString>& routsetResult ); //Используя широковещательный запрос
	void ReadSqlInstanceWithWMI( set<CAtlString>& result );
	// get sql instance name and versionnumber with sql browser service
	void ReadSqlInformationWithSqlBrowserService( std::map<CAtlString, CAtlString>& rin_result, const char* chIP );

	void GetHttpPage(CAtlString strServerName, CAtlString strWebFolser, CAtlString& strBuf);

};