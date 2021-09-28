//--------------------------------------------------------------------
// Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------
// Unit:        db
// Cooperation: absent
//--------------------------------------------------------------------
// Description: Exporting inspection data from database
//--------------------------------------------------------------------

#pragma once
#include "db/include/storage.h"

#define ALL_DB_XML_ROOT_NODE_NAME		_T("IM_EXPORT")

class DB_CLASS DBExporter
{
private:
    DBStorage* m_pStorage;
public:
    // Note: pStorage should be connected
    DBExporter(DBStorage* pStorage) : m_pStorage(pStorage){};
    virtual ~DBExporter(void){};

	bool ExportDatabase(bool bExportToOneFile, LPCTSTR pszPath = _T(""));

private:

	MSXML::IXMLDOMElementPtr GetXMLComputerInfo(FieldsPtr piFields, MSXML::IXMLDOMDocumentPtr piDoc);

	void exportSoft(MSXML::IXMLDOMDocumentPtr piDoc,
					MSXML::IXMLDOMElementPtr piRoot,
					const _bstr_t& bstrComputerId);
				
	void exportOS(MSXML::IXMLDOMDocumentPtr piDoc,
					MSXML::IXMLDOMElementPtr piRoot,
					const _bstr_t& bstrComputerId);

    void exportOSList(MSXML::IXMLDOMDocumentPtr piDoc,
					MSXML::IXMLDOMElementPtr piRoot,
					const _bstr_t& bstrComputerId);

	void exportHardware(MSXML::IXMLDOMDocumentPtr piDoc,
					MSXML::IXMLDOMElementPtr piRoot,
					const _bstr_t& bstrComputerId);

	void saveRecordsetToXml(_RecordsetPtr piRs, LPCTSTR pszRecordName,
						MSXML::IXMLDOMDocumentPtr piDoc, MSXML::IXMLDOMElementPtr piRoot);

    bstr_t formatFullPath(LPCTSTR pszPath, LPCTSTR pszFileName);
};


