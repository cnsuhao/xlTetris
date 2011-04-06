//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   Language.h
//    Author:      Streamlet
//    Create Time: 2011-03-08
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------

#ifndef __LANGUAGE_H_C0C5FFD4_36C4_4368_83C2_173E8C908073_INCLUDED__
#define __LANGUAGE_H_C0C5FFD4_36C4_4368_83C2_173E8C908073_INCLUDED__


#include <Windows.h>
#include <xl/Containers/xlArray.h>
#include <xl/Containers/xlMap.h>
#include <xl/String/xlString.h>

class Language
{
private:
    typedef xl::Map<xl::String, xl::String> StringTable;

    struct LanguageInfo
    {
        xl::String  strDisplayName;
        xl::String  strLanguageCode;
    };

    struct LanguageData
    {
        LanguageInfo stLanguageInfo;
        StringTable  mapStringTable;
    };

    typedef xl::Map<xl::String, LanguageData> MultiLanguages;

public:
    Language();
    ~Language();

private:
    MultiLanguages m_languages;
    LanguageData *m_pCurrentLanguage;

public:
    bool Initialize(LPCTSTR lpszXmlFile);
    bool Initialize(LPCTSTR lpszResType, UINT nResID);

private:
    bool InnerInit(const xl::String &strXml);

public:
    void EnumLanguages(xl::Array<LanguageInfo> *pLanguages);
    bool SetCurrentLanguage(const xl::String &strLanguageCode);
    bool SetCurrentLanguage(LANGID wLangCode);
    xl::String GetString(const xl::String &strStringID);
};

__declspec(selectany) Language _Language;


#endif // #ifndef __LANGUAGE_H_C0C5FFD4_36C4_4368_83C2_173E8C908073_INCLUDED__
