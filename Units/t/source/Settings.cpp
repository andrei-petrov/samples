//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Settings
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
// Description:  Program settings storage
//--------------------------------------------------------------------------------

#include "stdafx.h"

#include "T/include/RegSettingTraits.h"
//#include "T/include/RegKey.h"
#include "T/Include/Constants.h"
#include "T/Include/Settings.h"
#include "T/Include/Exception.h"


///////////////////////////////////////////////////////////////////////////////////////
//
//                           TMapStringToSmartPtr
//

void TMapStringToSmartPtr::Serialize(CArchive& ar)
{
	//CMap by default make bitwise read/write.
	//So we must use logic like CMapStringToOb::Serialize(CArchive& ar)
	ASSERT_VALID(this);

	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar.WriteCount(m_nCount);
		if (m_nCount == 0)
			return;  // nothing more to do

		ASSERT(m_pHashTable != NULL);
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			CAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
			  pAssoc = static_cast<CAssoc*>(PGetNextAssoc (pAssoc)))
			{
				ar << pAssoc->key;
				ar << pAssoc->value;
			}
		}
	}
	else
	{
		DWORD_PTR nNewCount = ar.ReadCount();
		CString newKey;
		//CObject* newValue;
		TObject::Ptr newValue;
		while (nNewCount--)
		{
			ar >> newKey;
			ar >> newValue;
			SetAt(newKey, newValue);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//
//                                TSettings
//


const int cnCurrentSettingsVersion = 1; // current version, should be incremented

IMPLEMENT_SERIAL(TSettings, TObject, VERSIONABLE_SCHEMA | cnCurrentSettingsVersion)

/*
TSettingsStorageModeEnum  TSettings::ms_enMode = ssmStream;
TSettingTraits* TSettings::m_pDefaultSettingProcessing = NULL;
*/

TSettings::TSettings()
{
//    ms_enMode = ssmStream; // can't touch this! (ssmStream by default)
    InitNew();
}

//  MFC overridables

/*
void TSettings::Serialize (CArchive &ar)
{
	ar.SerializeClass( RUNTIME_CLASS(TSettings) );
//    CObject::Serialize (ar);    -- does nothing

	if (ar.IsStoring ())
	{
        m_mapStringToInt.Serialize(ar);
        m_mapStringToString.Serialize(ar);
        m_mapStringToBuf.Serialize(ar);
        m_mapStringToOb.Serialize(ar);
	}
	else // version schema is very important here
	{
        UINT nSchema = ar.GetObjectSchema();
        switch(nSchema)
        {
            case 1: // there were only three maps in that version
            {
                m_mapStringToInt.Serialize(ar);
                m_mapStringToString.Serialize(ar);
//                m_mapStringToBuf.Serialize(ar);
                m_mapStringToOb.Serialize(ar);
                break;
            }
            case cnCurrentSettingsVersion:
            {   
                m_mapStringToInt.Serialize(ar);
                m_mapStringToString.Serialize(ar);
                m_mapStringToBuf.Serialize(ar);
                m_mapStringToOb.Serialize(ar);
                break;           
            }
            case -1 : break; // schema is unknown, do nothing
            default : // error
            {            
                TException* pEx = new TException(IDE_T_UNSUPPORTED_SCHEMA, MB_OK | MB_HELP | MB_ICONERROR, (LPARAM)this);
				*pEx << CA2T(GetRuntimeClass()->m_lpszClassName);
                *pEx << nSchema;                
                ThrowException(pEx);            
            }
        }
	}
}
*/

void TSettings::Save()
{
}

void TSettings::InitNew()
{
	SetSettingProcessing(new TRegSettingTraits());
}

void TSettings::SetSettingProcessing(TSettingTraits* pSettingProcessing)
{
	ASSERT(pSettingProcessing);
	if(!pSettingProcessing)
		return ;

	m_piSettingProcessing = pSettingProcessing;

    m_mapStringToInt.RemoveAll();
    m_mapStringToString.RemoveAll();
	m_mapStringToBuf.RemoveAll();
    m_mapStringToOb.RemoveAll();

    m_bDirty = false;
}

int	TSettings::GetInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
{
	return m_piSettingProcessing->GetInt(lpszSection, lpszEntry, nDefault);
}

BOOL TSettings::WriteInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
{
	return m_piSettingProcessing->WriteInt(lpszSection, lpszEntry, nValue);
}

CString TSettings::GetString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
{
	return m_piSettingProcessing->GetString(lpszSection, lpszEntry, lpszDefault);
}

BOOL TSettings::WriteString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
{
	return m_piSettingProcessing->WriteString(lpszSection, lpszEntry, lpszValue);
}

vector<BYTE> TSettings::GetBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry)
{
	return m_piSettingProcessing->GetBinary(lpszSection, lpszEntry);
}

BOOL TSettings::WriteBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData,  UINT nBytes)
{
	return m_piSettingProcessing->WriteBinary(lpszSection, lpszEntry, pData,  nBytes);
}

TObject::Ptr TSettings::GetObject(LPCTSTR lpszSection, LPCTSTR lpszEntry)
{
	TObject::Ptr spObject;
    if(m_piSettingProcessing)
	{
		vector<BYTE> bufTmp = m_piSettingProcessing->GetBinary(lpszSection, lpszEntry);
		DWORD dwSize = bufTmp.size();
		if(dwSize)
		{
			CMemFile file;
			file.Attach((BYTE*)&(bufTmp[0]), dwSize);
			try
			{
				CArchive ar(&file, CArchive::load);
				ar >> spObject;
			}
			catch(CException* pEx)
			{
				pEx->ReportError();
				pEx->Delete();
			}
			file.Detach();
		}
	}

	return spObject;
}

BOOL TSettings::WriteObject(LPCTSTR lpszSection, LPCTSTR lpszEntry, TObject::Ptr spObject)
{
	BOOL bRes = TRUE;
    if(m_piSettingProcessing)
	{
		CMemFile file;
		try
		{
			CArchive ar(&file, CArchive::store);
			ar << spObject;
		}
		catch(CException* pEx)
		{
			pEx->ReportError();
			pEx->Delete();

			return FALSE;
		}

		int nBytes = static_cast<int>(file.GetLength());
		BYTE* pData  = file.Detach();
		bRes = m_piSettingProcessing->WriteBinary(lpszSection, lpszEntry, pData, nBytes);
		delete (pData); // free
	}

	return bRes;
}

/*
BOOL TSettings::_WriteInt(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, int nValue)
{
    BOOL bRes = TRUE;
    CString strSection = GetAppropriateSection(lpszSection, enMode);
    if(ssmRegistry == enMode)
    {
        TRegKey key;
        while(true)
        {
            if(key.Open(hKey, strSection, KEY_WRITE) != ERROR_SUCCESS)
                break ;

            if(key.SetDWORDValue(lpszEntry, nValue) != ERROR_SUCCESS)
                break ;

            break ;
        }
    }
    else if(ssmStream == enMode)
    {
        int nOldValue = 0;
        CString strKey = GetValueName(strSection, lpszEntry);
        if(m_mapStringToInt.Lookup(strKey, nOldValue) != 0)
        {
            if(nOldValue == nValue)
                return TRUE;
        }

        m_mapStringToInt.SetAt(strKey, nValue);
        m_bDirty = true;
    }

    return bRes;
}


BOOL TSettings::_WriteString(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, LPCTSTR lpszValue)
{
    BOOL bRes = TRUE;
    CString strSection = GetAppropriateSection(lpszSection, enMode);
    if(ssmRegistry == enMode)
    {
        TRegKey key;
        while(true)
        {
            if(key.Open(hKey, strSection, KEY_WRITE) != ERROR_SUCCESS)
                break ;

            if(key.SetStringValue(lpszEntry, lpszValue) != ERROR_SUCCESS)
                break ;

            break ;
        }
    }
    else if(ssmStream == enMode)
    {
        CString strKey = GetValueName(strSection, lpszEntry);
        CString strOldValue;
        if(m_mapStringToString.Lookup(strKey, strOldValue))
        {
            if(strOldValue.Compare(lpszValue) == 0)
                return TRUE;
        }            
        m_mapStringToString.SetAt(strKey, lpszValue);
        m_bDirty = true;
    }

    return bRes;
}


BOOL TSettings::_WriteBinary(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, LPBYTE pData, UINT nBytes)
{
    BOOL bRes = TRUE;
    CString strSection = GetAppropriateSection(lpszSection, enMode);
    if(ssmRegistry == enMode)
    {
        TRegKey key;
        while(true)
        {
            if(key.Open(hKey, strSection, KEY_WRITE) != ERROR_SUCCESS)
                break ;

            if(key.SetBinValue(lpszEntry, pData, nBytes) != ERROR_SUCCESS)
                break ;

            break ;
        }
    }
    else if(ssmStream == enMode)
    {
        CString strKey = GetValueName(strSection, lpszEntry);
        vector<BYTE> buf(nBytes);
		::memcpy(&(buf[0]), pData, nBytes);
        vector<BYTE> bufOld;
        if(m_mapStringToBuf.Lookup(strKey, bufOld) != 0)
        {
            if(bufOld.size() == nBytes && bufOld == buf)
                return TRUE;
        }
        m_mapStringToBuf.SetAt(strKey, buf);
        m_bDirty = true;
        return TRUE;
    }

    return FALSE;
}


BOOL TSettings::_WriteObject(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, TObject::Ptr spObject)
{
    if(!spObject)
    {
        ASSERT(FALSE);
        return FALSE;
    }

    BOOL bRes = FALSE;
    if(ssmRegistry == enMode)
    {
        CMemFile file;        
        try
        {
            CArchive ar(&file, CArchive::store);
            ar << spObject;            
        }
        catch(CException* pEx)
        {
            pEx->ReportError(); // todo: add new ex, remove ReportError()
            pEx->Delete();
        }

        int   nBytes = static_cast<int>(file.GetLength());
        BYTE* pData  = file.Detach();
        bRes = _WriteBinary(hKey, lpszSection, lpszEntry, enMode, pData, nBytes);
        delete (pData); // free
    }
    if(enMode == ssmStream)
    {
        CString strSection = GetAppropriateSection(lpszSection, enMode);
        TObject::Ptr spOldObject;
        CString strKey = GetValueName(strSection, lpszEntry);
        if(m_mapStringToOb.Lookup(strKey, spOldObject) != 0)
        {
            if(spOldObject)
                if(dynamic_cast<TObject&>(*spOldObject) == dynamic_cast<TObject&>(*spObject))
                    return TRUE;
        }
        m_mapStringToOb.SetAt(strKey, spObject);
        m_bDirty = true;
        bRes = TRUE;
    }

    return bRes;
}
bool TSettings::_FindInt(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, int& rnValue)
{
	bool bRes = false;
    CString strSection = GetAppropriateSection(lpszSection, enMode);

    if(ssmRegistry == enMode)
    {
        TRegKey key;
        while(true)
        {
            if(key.Open(hKey, strSection, KEY_READ) != ERROR_SUCCESS)
                break ;

            DWORD dwResult = 0;
            if(key.QueryDWORDValue(lpszEntry, dwResult) != ERROR_SUCCESS)
                break ;

            rnValue = (int ) dwResult;
			bRes = true;
            break ;
        }
    }
    else if(ssmStream == enMode)
    {
        CString strKey = GetValueName(lpszSection, lpszEntry);
        if(m_mapStringToInt.Lookup(strKey, rnValue))
		{
            bRes = true;
		}
    }
	return bRes;
}

bool TSettings::_FindString(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, CString& rValue)
{
	bool bRes = false;
	CString strRetValue;
    CString strSection = GetAppropriateSection(lpszSection, enMode);

    if(ssmRegistry == enMode)
    {
        TRegKey key;
        while(true)
        {
            if(key.Open(hKey, strSection, KEY_READ) != ERROR_SUCCESS)
                break ;

            if(key.QueryStringValue(lpszEntry, strRetValue) != ERROR_SUCCESS)
                break ;

			rValue = strRetValue;
			bRes = true;
            break ;
        }
    }
    else if(ssmStream == enMode)
    {
        CString strKey = GetValueName(strSection, lpszEntry);
        if(m_mapStringToString.Lookup(strKey, rValue))
		{
			bRes = true;
		}
    }

    return bRes;
}

bool TSettings::_FindBinary(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, vector<BYTE>& rbufValue)
{
	bool bRes = false;
    CString strSection = GetAppropriateSection(lpszSection, enMode);

    if(ssmRegistry == enMode)
    {
        TRegKey key;
        while(true)
        {
            if(key.Open(hKey, strSection, KEY_READ) != ERROR_SUCCESS)
                break ;

            DWORD dwLen = 0;
            if(key.QueryBinValueLength(lpszEntry, dwLen) != ERROR_SUCCESS)
                break ;

            if(!dwLen)
                break ;

			rbufValue.resize(dwLen);
            //*ppData = new BYTE[*pBytes];

            if(key.QueryBinValue(lpszEntry, (LPBYTE)&(rbufValue[0]), &dwLen) != ERROR_SUCCESS)
			{
				rbufValue.clear();
                break ;
			}

            bRes = true;

            break ;
        }
    }
    if(enMode == ssmStream)
    {
        CString strKey = GetValueName(strSection, lpszEntry);
        if(m_mapStringToBuf.Lookup(strKey, rbufValue))
        {
			bRes = true;
            //if(pBytes && ppData)
            //{
            //    *pBytes = bufResult.GetSize();
            //    *ppData = (BYTE*)bufResult.GetBuf();
            //    if(*pBytes)
            //    {
            //        *ppData = new BYTE[*pBytes];
            //        memcpy(*ppData, bufResult.GetBuf(), *pBytes);
            //    }
            //}
        }
    }

    return bRes;
}

bool TSettings::_FindObject(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, TObject::Ptr& rspValue)
{
	bool bRes = false;
    if(ssmRegistry == enMode)
    {
		vector<BYTE> bufTmp;

		int nSize = 0;
		if(_FindBinary(hKey, lpszSection, lpszEntry, enMode, bufTmp) && (nSize = bufTmp.size()))
		{       
            CMemFile file;
            file.Attach((BYTE*)&(bufTmp[0]), nSize);
            try
            {
                CArchive ar(&file, CArchive::load);
                ar >> rspValue;
				bRes = true;
            }
            catch(CException* pEx)
            {
                pEx->ReportError(); // todo: add new ex, remove ReportError()
                pEx->Delete();
            }
			file.Detach();
        }
    }
    else if(enMode == ssmStream)
    {
        CString strSection = GetAppropriateSection(lpszSection, enMode);
        CString strKey = GetValueName(strSection, lpszEntry);
        if(m_mapStringToOb.Lookup(strKey, rspValue))
		{
            bRes = true;
		}
    }

    return bRes;
}

void TSettings::SetDefaultSettingProcessing(TSettingTraits* pSettingProcessing)
{
    if(m_pDefaultSettingProcessing)
    {
        ASSERT(false);
        return ;
    }

    m_pDefaultSettingProcessing = pSettingProcessing;
}

void TSettings::AddSettingProcessing(LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingTraits* pSettingProcessing)
{
    if(!pSettingProcessing)
    {
        ASSERT(false);
        return ;
    }

    TRegKeyProcessing::Vector* pProcessings = GetSettingProcessings();
    if(!pProcessings)
    {
        ASSERT(false);
        return ;
    }

    TRegKeyProcessing::Vector::iterator viFindProcessing = find_if(pProcessings->begin(), pProcessings->end(),
        TRegKeyProcessing::FindByKey(lpszSection, lpszEntry));

    if(viFindProcessing != pProcessings->end())
    {
        ASSERT(false);
        return ;
    }

    pProcessings->push_back(TRegKeyProcessing(lpszSection, lpszEntry, pSettingProcessing));
}


///////////////////////////////////////////////////////////////////////////
//
//                      Implementation
//

TSettingTraits* TSettings::FindSettingProcessing(LPCTSTR lpszSection, LPCTSTR lpszEntry)
{
    TRegKeyProcessing::Vector* pProcessings = GetSettingProcessings();
    if(!pProcessings)
    {
        ASSERT(false);
        return NULL;
    }

    TRegKeyProcessing::Vector::iterator viFindProcessing = find_if(pProcessings->begin(), pProcessings->end(),
        TRegKeyProcessing::FindByKey(lpszSection, lpszEntry));

    if(viFindProcessing == pProcessings->end())
        return m_pDefaultSettingProcessing;

    return (*viFindProcessing).m_pProcessing;
}

CString TSettings::GetAppropriateSection(LPCTSTR lpszRelativeSection, TSettingsStorageModeEnum enMode)
{
    CWinApp* pApp = AfxGetApp();
    if(!pApp)
    {
        ASSERT(false);
        return _T("");
    }

    CString strSection = _T("Software");

    if(ssmRegistry == enMode)
    {
        if(pApp->m_pszRegistryKey && *pApp->m_pszRegistryKey)//wcslen(pApp->m_pszRegistryKey))
        {
            strSection += _T("\\");
            strSection +=  pApp->m_pszRegistryKey;
        }

        if(pApp->m_pszProfileName && *pApp->m_pszProfileName)//wcslen(pApp->m_pszProfileName))
        {
            strSection += _T("\\");
            strSection += pApp->m_pszProfileName;
        }

        if(lpszRelativeSection && *lpszRelativeSection)//wcslen(lpszRelativeSection))
        {
            strSection += _T("\\");
            strSection += lpszRelativeSection;
        }
    }
    else if(ssmStream == enMode)
        strSection = lpszRelativeSection;

    return strSection;
}

CString TSettings::GetValueName(LPCTSTR lpszSection, LPCTSTR lpszEntry)
{
    CString strKey = lpszSection;
    strKey += _T("\\");
    strKey += lpszEntry;
    return strKey;
}

TRegKeyProcessing::Vector* TSettings::GetSettingProcessings()
{
    static TRegKeyProcessing::Vector s_vecProcessings;
    return &s_vecProcessings;
}
*/
