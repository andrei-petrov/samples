//--------------------------------------------------------------------
// Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------
// Unit:        db
// Cooperation: absent
//--------------------------------------------------------------------
// Description: Exporting inspection data from database
//--------------------------------------------------------------------

#include "stdafx.h"

#include "rgs/include/schemadef.h"
#include "scm/include/SchemaDef.h"
#include "cmn/include/McException.h"
#include "cmn/include/Misc.h"

#include "db/include/Constants.h"
#include "db/include/DBExporter.h"


bool DBExporter::ExportDatabase(bool bExportToOneFile, LPCTSTR pszPath)

{
	HRESULT hr = S_OK;
    try
    {
	    _RecordsetPtr piComputersRecordset = m_pStorage->GetInspectedComputers();

        long nComputers = piComputersRecordset->GetRecordCount();
	    if ( nComputers <= 0 )
	    {
            CMNLog(_T("[DBExporter::ExportDatabase]: компьютеры не найдены в базе данных.")); 
            return false;
	    }

	    piComputersRecordset->MoveFirst();

	    if ( bExportToOneFile )
	    {
		    MSXML::IXMLDOMDocumentPtr piDoc;
		    hr = piDoc.CreateInstance("MSXML.DOMDocument");
		    CMNCheckError (hr);

		    MSXML::IXMLDOMElementPtr piRootNode = piDoc->createElement( ALL_DB_XML_ROOT_NODE_NAME );

		    for (long i = 0; i < nComputers; i++)
		    {
			    MSXML::IXMLDOMElementPtr
				    piCompNode = GetXMLComputerInfo( piComputersRecordset->Fields, piDoc );

			    piRootNode->appendChild( piCompNode );

			    piComputersRecordset->MoveNext();
		    }

		    piDoc->appendChild( piRootNode );

            bstr_t bstrDatabaseName = m_pStorage->GetDefaultDatabase();
		    bstr_t bstrFilename = bstrDatabaseName + _T("Database.xml"); 
            bstr_t bstrFullPath = formatFullPath(pszPath, (LPCTSTR)bstrFilename);

		    hr = piDoc->save( bstrFullPath );
		    CMNCheckError (hr);
	    }
	    else
	    {
		    for (long i = 0; i < nComputers; i++)
		    {
			    MSXML::IXMLDOMDocumentPtr piDoc;
			    hr = piDoc.CreateInstance("MSXML.DOMDocument");
			    CMNCheckError (hr);

			    MSXML::IXMLDOMElementPtr
				    piRootNode = GetXMLComputerInfo( piComputersRecordset->Fields, piDoc );

			    piDoc->appendChild( piRootNode );

			    bstr_t bstrFilename = piComputersRecordset->Fields->Item[ cszAttrib_Name ]->Value;
                bstrFilename += "_.xml"; 
                bstr_t bstrFullPath = formatFullPath(pszPath, (LPCTSTR)bstrFilename);

			    hr = piDoc->save( bstrFullPath );
			    CMNCheckError (hr);

			    piComputersRecordset->MoveNext();
		    }
	    }
    }
    catch (CMNException* pEx)
    {	
		CMNException* pHiLevelEx = new CMNException(IDE_DB_EXPORTDATABASE_FAILED);
		*pHiLevelEx	+= *pEx;
		ThrowException(pHiLevelEx);
	}
    return true;
}

MSXML::IXMLDOMElementPtr
	DBExporter::GetXMLComputerInfo(FieldsPtr piFields, MSXML::IXMLDOMDocumentPtr piDoc)
{
    _ASSERT( piFields != NULL );
    _ASSERT( piDoc != NULL );

	MSXML::IXMLDOMElementPtr piRootNode				= piDoc->createElement( cszRGSResearchInfo );

	MSXML::IXMLDOMAttributePtr piAttrComputerName	= piDoc->createAttribute( cszRGSComputerName );
	MSXML::IXMLDOMAttributePtr piAttrDomainName		= piDoc->createAttribute( cszRGSDomainName );
    MSXML::IXMLDOMAttributePtr piAttrDescription	= piDoc->createAttribute( cszRGSDescription );
	MSXML::IXMLDOMAttributePtr piAttrDate			= piDoc->createAttribute( cszRGSDate );

	piAttrComputerName->value = piFields->Item[ cszAttrib_Name ]->Value; 
    piAttrDescription->value = piFields->Item[ cszAttrib_Description ]->Value; 

	COleDateTime dt( piFields->Item[ cszRGSDate ]->Value ); 
	piAttrDate->value = (LPCTSTR)dt.Format( DATE_TIME_COMMON_FORMAT );

	piRootNode->attributes->setNamedItem( piAttrComputerName );
	piRootNode->attributes->setNamedItem( piAttrDomainName );
    piRootNode->attributes->setNamedItem( piAttrDescription );
	piRootNode->attributes->setNamedItem( piAttrDate );

	_bstr_t bstrCompId = piFields->Item[ cszAttrib_ObjectGUID ]->Value; 

    CMNLog(_T("[DBExporter::GetXMLComputerInfo]: экспорт данных для компьютера '%s' "), (LPCTSTR)bstrCompId); 

	exportOS( piDoc, piRootNode, bstrCompId );
	exportSoft( piDoc, piRootNode, bstrCompId );
	exportHardware( piDoc, piRootNode, bstrCompId );
    exportOSList( piDoc, piRootNode, bstrCompId );

	return piRootNode;
}

void DBExporter::exportSoft(MSXML::IXMLDOMDocumentPtr piDoc,
				MSXML::IXMLDOMElementPtr piRoot,
				const _bstr_t& bstrComputerId)
{
    _ASSERT( piDoc != NULL );
    _ASSERT( piRoot != NULL );

	_RecordsetPtr piRsSoft = m_pStorage->GetComputerSoft( bstrComputerId );

	MSXML::IXMLDOMElementPtr piSoftNode = piDoc->createElement( cszRGSSoft );
	piRoot->appendChild( piSoftNode );

	saveRecordsetToXml( piRsSoft, cszRGSInstallInfo, piDoc, piSoftNode );
}


void DBExporter::exportOS(MSXML::IXMLDOMDocumentPtr piDoc,
				MSXML::IXMLDOMElementPtr piRoot,
				const _bstr_t& bstrComputerId)
{
    _ASSERT( piDoc != NULL );
    _ASSERT( piRoot != NULL );

	_RecordsetPtr piRsOS = m_pStorage->GetComputerOS( bstrComputerId );

	MSXML::IXMLDOMElementPtr piOSNode = piDoc->createElement( cszRGSOS );
	piRoot->appendChild( piOSNode );

	saveRecordsetToXml( piRsOS, cszRGSSystem, piDoc, piOSNode );
}

void DBExporter::exportOSList(MSXML::IXMLDOMDocumentPtr piDoc,
				MSXML::IXMLDOMElementPtr piRoot,
				const _bstr_t& bstrComputerId)
{
    _ASSERT( piDoc != NULL );
    _ASSERT( piRoot != NULL );

	_RecordsetPtr piRsOSList = m_pStorage->GetComputerOSList( bstrComputerId );

	MSXML::IXMLDOMElementPtr piOSListNode = piDoc->createElement( cszRGSOSList );
	piRoot->appendChild( piOSListNode );

	saveRecordsetToXml( piRsOSList, cszRGSOS, piDoc, piOSListNode );
}

void DBExporter::exportHardware(MSXML::IXMLDOMDocumentPtr piDoc,
				MSXML::IXMLDOMElementPtr piRoot,
				const _bstr_t& bstrComputerId)
{
    _ASSERT( piDoc != NULL );
    _ASSERT( piRoot != NULL );

	_RecordsetPtr piRsHard = m_pStorage->GetComputerHard( bstrComputerId );

	MSXML::IXMLDOMElementPtr piHardwareNode = piDoc->createElement( cszRGSHardware );
	piRoot->appendChild( piHardwareNode );

	saveRecordsetToXml( piRsHard, cszRGSDeviceInfo, piDoc, piHardwareNode );
}

void DBExporter::saveRecordsetToXml(_RecordsetPtr piRs,
										LPCTSTR pszRecordName,
										MSXML::IXMLDOMDocumentPtr piDoc,
										MSXML::IXMLDOMElementPtr piRoot)
{
    _ASSERT( piRs != NULL );
    _ASSERT( piDoc != NULL );
    _ASSERT( piRoot != NULL );

	long nRecords = piRs->RecordCount;
	if ( !nRecords )
	{
        CMNLog(_T("[DBExporter::saveRecordsetToXml]: для узла '%s' записей нет "), pszRecordName); 
		return;
	}

	piRs->MoveFirst();
	for( long i = 0; i < nRecords; i++ )
	{
		MSXML::IXMLDOMElementPtr piNode = piDoc->createElement( pszRecordName );

        long nFields = piRs->Fields->GetCount();
		for( long j=0; j < nFields; j++ )
		{
			FieldPtr piField = piRs->Fields->Item[j];
			MSXML::IXMLDOMAttributePtr piAttr = piDoc->createAttribute( piField->Name );
            piAttr->value = piField->GetValue();
			piNode->attributes->setNamedItem( piAttr );
		}

		piRoot->appendChild( piNode );
		piRs->MoveNext();
	}
}

bstr_t DBExporter::formatFullPath(LPCTSTR pszPath, LPCTSTR pszFileName)
{
    CString strPath = pszPath;
    CString strFileName = pszFileName;
   
    strPath.Trim();
    strPath.Replace(L'/', L'\\');

    int nLength = strPath.GetLength();
    if( (nLength > 0) && strPath[strPath.GetLength() - 1] != '\\' )
	    strPath += L'\\';

    strPath += strFileName;
    return bstr_t((LPCTSTR) strPath);
}

