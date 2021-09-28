//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: Settings
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
// Description:  Program settings storage, taken from RSDN and refactored
//--------------------------------------------------------------------------------


#pragma once

#include "CMN/Include/SmartPtr.h"

#include "T/include/SettingTraits.h"
#include "T/Include/T.h"
//#include "T/Include/Buf.h"
#include "T/Include/Object.h"


typedef CMap<CString, LPCTSTR, int, int&>   TMapStringToInt;
typedef CMap<CString, LPCTSTR, vector<BYTE>, vector<BYTE>&> TMapStringToBuf;

///////////////////////////////////////////////////////////////////////////////////////
//
//                           TMapStringToSmartPtr
//

class TMapStringToSmartPtr : public CMap<CString, LPCTSTR, TObject::Ptr, TObject::Ptr&>
{
public:
	TMapStringToSmartPtr(){};
	virtual ~TMapStringToSmartPtr(){};
	void Serialize(CArchive&);
};

///////////////////////////////////////////////////////////////////////////////////////
//
//                                TSettings
//

class T_CLASS TSettings: public TObject
{
    DECLARE_SERIAL (TSettings)

    //friend class TSettingTraits;

public:
    typedef TSettings			Class;
    typedef CMNSmartPtr<Class>	Ptr;

protected:
	TSettingTraits::Ptr	m_piSettingProcessing;
    // maps for settings
    TMapStringToInt         m_mapStringToInt;
    CMapStringToString      m_mapStringToString;
    TMapStringToBuf         m_mapStringToBuf;
	TMapStringToSmartPtr    m_mapStringToOb;

    bool					m_bDirty;

public:
    TSettings();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(TSettings)
    //virtual void Serialize (CArchive& ar);
    //}}AFX_VIRTUAL

	void			Save();
    void            InitNew();
	void			SetSettingProcessing(TSettingTraits* pSettingProcessing);

    // Settings values with p processing.
    int				GetInt       (LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
    BOOL			WriteInt     (LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
    CString			GetString    (LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL);
    BOOL			WriteString  (LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
    vector<BYTE>	GetBinary    (LPCTSTR lpszSection, LPCTSTR lpszEntry);
    BOOL			WriteBinary  (LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData,  UINT nBytes);
	TObject::Ptr	GetObject    (LPCTSTR lpszSection, LPCTSTR lpszEntry);
    BOOL			WriteObject  (LPCTSTR lpszSection, LPCTSTR lpszEntry, TObject::Ptr spObject);

protected: // Raw settings values
	/*
    BOOL			_WriteInt     (HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, int nValue);
    BOOL			_WriteString  (HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, LPCTSTR lpszValue);
    BOOL			_WriteBinary  (HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, LPBYTE pData,  UINT nBytes);
    BOOL			_WriteObject  (HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, TObject::Ptr spObject);

	// return true and fill Value parameter if value persist else just return false
	bool _FindInt(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, int& rnValue);
	bool _FindString(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, CString& rValue);
	bool _FindBinary(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, vector<BYTE>& rbufValue);
	bool _FindObject(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingsStorageModeEnum enMode, TObject::Ptr& rspValue);
	*/

public:
    void            SetDirty(bool bDirty) { m_bDirty = bDirty; }
    bool            IsDirty() { return m_bDirty; }
    //static void                     SetStorageMode(TSettingsStorageModeEnum enMode) { ms_enMode = enMode; }
    //static TSettingsStorageModeEnum GetStorageMode() { return ms_enMode; }
    //static void     SetDefaultSettingProcessing(TSettingTraits* pSettingProcessing);
    //static void     AddSettingProcessing(LPCTSTR lpszSection, LPCTSTR lpszEntry, TSettingTraits* pSettingProcessing);

//  Implementation
protected:
	/*
    TSettingTraits* FindSettingProcessing(LPCTSTR lpszSection, LPCTSTR lpszEntry);
    CString GetAppropriateSection(LPCTSTR lpszRelativeSection, TSettingsStorageModeEnum enMode);
    CString GetValueName(LPCTSTR lpszSection, LPCTSTR lpszEntry);
    static TRegKeyProcessing::Vector* GetSettingProcessings();
	*/
};

/*
#define T_SET_DEFAULT_SETTING_PROCESSING(theClass, settingProcessing) \
    static class TSetDefaultSettingProcessing_##theClass \
    { \
    public: \
        TSetDefaultSettingProcessing_##theClass() \
        { \
            TSettings::SetDefaultSettingProcessing(settingProcessing); \
        } \
    } defaultSettingProcessing_##theClass;

#define T_BEGIN_SETTING_PROCESSING_MAP(theClass) \
    namespace AddSettingProcessingNamespace_##theClass \
    { \
        static class TAddSettingProcessing_##theClass \
        { \
        public: \
            TAddSettingProcessing_##theClass() \
            {

#define T_SETTING_PROCESSING_ENTRY(strSection, strEntry, pProcessing) \
        TSettings::AddSettingProcessing(strSection, strEntry, pProcessing);

#define T_END_SETTING_PROCESSING_MAP() \
            }; \
        } settingProcessings; \
    }
*/
