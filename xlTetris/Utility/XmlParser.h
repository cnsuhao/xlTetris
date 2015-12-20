//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   XmlParser.h
//    Author:      Streamlet
//    Create Time: 2009-09-15
//    Description: 
//
//    Version history:
//        Reviewd on 2010-10-26
//
//
//------------------------------------------------------------------------------

#ifndef __XMLPARSER_H_81AA27B5_A5C3_472C_9013_298686A3DC99_INCLUDED__
#define __XMLPARSER_H_81AA27B5_A5C3_472C_9013_298686A3DC99_INCLUDED__


#include <xl/Common/Containers/xlArray.h>
#include <xl/Common/Containers/xlList.h>
#include <xl/Common/Containers/xlPair.h>
#include <xl/Common/Containers/xlMap.h>
#include <xl/Common/String/xlString.h>
#include <xl/Common/Memory/xlSmartPtr.h>

typedef xl::Char XmlChar;
typedef xl::String XmlString;
class XmlNode;
class XmlInst;
typedef xl::SharedPtr<XmlNode> XmlNodePtr;
typedef xl::SharedPtr<XmlInst> XmlInstPtr;
typedef xl::List<XmlNodePtr> XmlNodeList;
typedef xl::List<XmlInstPtr> XmlInstList;
typedef xl::Pair<XmlString, XmlString> XmlProperty;
typedef xl::Map<XmlString, XmlString> XmlPropertyMap;


//////////////////////////////////////////////////////////////////////////
// XmlNode

class XmlNode
{
public:
    XmlNode();
    ~XmlNode();

public:
    void Clear();

public:
    XmlString GetXmlString(const XmlString &strLineEnd = L"\r\n", const XmlString &strIndent = L"\t", size_t nCount = 0);

public:
    enum NODE_TYPE
    {
        XML_NODE,
        XML_VALUE,
        XML_CDATA
    };

    NODE_TYPE GetType();
    void SetType(NODE_TYPE type);

public:
    XmlString GetTagName();
    bool SetTagName(const XmlString &strTagName);
    XmlString GetValue();
    bool SetValue(const XmlString &strValue);
    XmlString GetInnerXml(const XmlString &strLineEnd = L"\r\n", const XmlString &strIndent = L"\t", size_t nCount = 0);
    bool SetInnerXml(const XmlString &strInnerXml);

public:
    XmlPropertyMap &Properties();
    XmlNodeList &SubNodes();

private:
    NODE_TYPE m_ntType;
    XmlString m_strTagName;
    XmlString m_strValue;
    XmlPropertyMap m_mapProperties;
    XmlNodeList m_listSubNodes;
};

//////////////////////////////////////////////////////////////////////////
// XmlInst

#define XML_INST_XML            L"xml"
#define XML_INST_VERSION        L"version"
#define XML_INST_VERSION_1_0    L"1.0"
#define XML_INST_ENCODING       L"encoding"
#define XML_INST_ENCODING_UTF8  L"utf-8"

class XmlInst
{
public:
    XmlInst();
    ~XmlInst();

public:
    void Clear();

public:
    XmlString GetXmlString(const XmlString &strLineEnd = L"\r\n");

public:
    XmlString GetTagName();
    bool SetTagName(const XmlString &strTagName);

public:
    XmlPropertyMap &Properties();

private:
    XmlString m_strTagName;
    XmlPropertyMap m_mapProperties;
};

//////////////////////////////////////////////////////////////////////////
// XmlParser

class XmlParser
{
public:
    static bool ParseXml(const XmlString &strXml, XmlInstList *pXmlInstList, XmlNodeList *pXmlNodeList);
    static bool ParseXmlFromFile(XmlNode &xml);
    static bool WriteXmlToFile(const XmlNode &xml);
};


#endif // #ifndef __XMLPARSER_H_81AA27B5_A5C3_472C_9013_298686A3DC99_INCLUDED__
