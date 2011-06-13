//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   Language.cpp
//    Author:      Streamlet
//    Create Time: 2011-03-08
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------


#include <xl/String/xlEncoding.h>
#include <Loki/ScopeGuard.h>
#include "Utility/XmlParser.h"
#include "Language.h"
#include <tchar.h>

Language::Language() :
    m_pCurrentLanguage(nullptr)
{

}

Language::~Language()
{

}

bool Language::Initialize(LPCTSTR lpszXmlFile)
{
    HANDLE hFile = CreateFile(lpszXmlFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    LOKI_ON_BLOCK_EXIT(CloseHandle, hFile);

    LARGE_INTEGER li = {};

    if (!GetFileSizeEx(hFile, &li))
    {
        return false;
    }

    xl::CharA *pBuffer = new xl::CharA[li.LowPart];
    DWORD dwRead = 0;
    bool bSucceeded = (ReadFile(hFile, pBuffer, li.LowPart, &dwRead, NULL) && dwRead == li.LowPart);
    xl::String strXml;
    xl::Encoding::Utf8ToUtf16(pBuffer, &strXml);
    delete[] pBuffer;

    if (!bSucceeded)
    {
        return false;
    }

    if (!InnerInit(strXml))
    {
        return false;
    }
    
    return true;
}

bool Language::Initialize(LPCTSTR lpszResType, UINT nResID)
{
    HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(nResID), lpszResType);

    if (hRes == NULL)
    {
        return false;
    }

    DWORD cbSize = SizeofResource(NULL, hRes);
    BYTE *lpbyRes = (BYTE *)LoadResource(NULL, hRes);

    if (lpbyRes == NULL)
    {
        return false;
    }

    xl::CharA *pBuffer = new xl::CharA[cbSize + 1];
    memcpy(pBuffer, lpbyRes, cbSize);
    pBuffer[cbSize] = '\0';
    xl::String strXml;
    xl::Encoding::Utf8ToUtf16(pBuffer, &strXml);
    delete[] pBuffer;

    if (!InnerInit(strXml))
    {
        return false;
    }

    return true;
}

bool Language::InnerInit(const xl::String &strXml)
{
    XmlInstList xmlInsts;
    XmlNodeList xmlNodes;

    if (!XmlParser::ParseXml(strXml, &xmlInsts, &xmlNodes))
    {
        return false;
    }

    if (xmlNodes.Empty())
    {
        return false;
    }

    XmlNodePtr pLanguagesNode = *xmlNodes.Begin();

    if (pLanguagesNode->GetTagName() != _T("Languages"))
    {
        return false;
    }

    for (auto &itLanguage = pLanguagesNode->SubNodes().Begin(); itLanguage != pLanguagesNode->SubNodes().End(); ++itLanguage)
    {
        XmlNodePtr pNodeLanguage = *itLanguage;

        if (pNodeLanguage->GetTagName() != _T("Language"))
        {
            continue;
        }

        xl::String strLangName = pNodeLanguage->Properties()[_T("DisplayName")];
        xl::String strLangCode = pNodeLanguage->Properties()[_T("Name")];

        LanguageData &language = m_languages[strLangCode];
        language.stLanguageInfo.strDisplayName = strLangName;
        language.stLanguageInfo.strLanguageCode = strLangCode;

        for (auto &itString = pNodeLanguage->SubNodes().Begin(); itString != pNodeLanguage->SubNodes().End(); ++itString)
        {
            XmlNodePtr pNodeString = *itString;

            if (pNodeString->GetTagName() != _T("String"))
            {
                continue;
            }

            if (pNodeString->SubNodes().Empty())
            {
                continue;
            }

            XmlNodePtr pSubNode = *pNodeString->SubNodes().Begin();

            if (pSubNode->GetType() != XmlNode::XML_CDATA && pSubNode->GetType() != XmlNode::XML_VALUE)
            {
                continue;
            }

            XmlString strID = pNodeString->Properties()[_T("ID")];

            if (strID.Empty())
            {
                continue;
            }

            XmlString strValue = pSubNode->GetValue();

            language.mapStringTable.Insert(strID, strValue);
        }
    }

    return true;
}

void Language::EnumLanguages(xl::Array<LanguageInfo> *pLanguages)
{
    if (pLanguages == nullptr)
    {
        return;
    }

    for (auto it = m_languages.Begin(); it != m_languages.End(); ++it)
    {
        pLanguages->PushBack(it->Value.stLanguageInfo);
    }
}

bool Language::SetCurrentLanguage(const xl::String &strLanguageCode)
{
    auto it = m_languages.Find(strLanguageCode);

    if (it == m_languages.End())
    {
        return false;
    }

    m_pCurrentLanguage = &it->Value;

    return true;
}

bool Language::SetCurrentLanguage(LANGID wLangCode)
{
    TCHAR szLangName[64] = {};

    if (!GetLocaleInfo(wLangCode, LOCALE_SISO639LANGNAME, szLangName, ARRAYSIZE(szLangName)))
    {
        return false;
    }

    TCHAR szRegionName[64] = {};

    if (!GetLocaleInfo(wLangCode, LOCALE_SISO3166CTRYNAME, szRegionName, ARRAYSIZE(szRegionName)))
    {
        return false;
    }

    xl::String strLanguageCode = szLangName;
    strLanguageCode += _T("-");
    strLanguageCode += szRegionName;

    auto it = m_languages.Find(strLanguageCode);

    if (it == m_languages.End())
    {
        it = m_languages.Find(szLangName);

        if (it == m_languages.End())
        {
            return false;
        }
    }

    m_pCurrentLanguage = &it->Value;

    return true;
}

xl::String Language::GetString(const xl::String &strStringID)
{
    if (m_pCurrentLanguage == nullptr)
    {
        return _T("");
    }

    return m_pCurrentLanguage->mapStringTable[strStringID];
}
