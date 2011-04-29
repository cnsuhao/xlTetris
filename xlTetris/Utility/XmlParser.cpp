//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   XmlParser.cpp
//    Author:      Streamlet
//    Create Time: 2009-09-15
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------


#include "XmlParser.h"

class XmlStringTrans
{
public:
    static XmlString XmlEncodeProperty(const XmlString &str);
    static XmlString XmlEncodeInnerXml(const XmlString &str);
    static XmlString XmlDecode(const XmlString &str);
};

XmlString XmlStringTrans::XmlEncodeProperty(const XmlString &str)
{
    XmlString strRet = str;

    strRet = strRet.Replace(L"&", L"&amp;");
    strRet = strRet.Replace(L"\"", L"&quot;");

    return strRet;
}

XmlString XmlStringTrans::XmlEncodeInnerXml(const XmlString &str)
{
    XmlString strRet = str;

    strRet = strRet.Replace(L"&", L"&amp;");
    strRet = strRet.Replace(L"<", L"&lt;");
    strRet = strRet.Replace(L">", L"&gt;");

    return strRet;
}

XmlString XmlStringTrans::XmlDecode(const XmlString &str)
{
    XmlString strRet = str;

    strRet = strRet.Replace(L"&lt;", L"<");
    strRet = strRet.Replace(L"&gt;", L">");
    strRet = strRet.Replace(L"&quot;", L"\"");
    strRet = strRet.Replace(L"&apos;", L"\'");
    strRet = strRet.Replace(L"&amp;", L"&");

    return strRet;
}

//////////////////////////////////////////////////////////////////////////
// XmlNode

XmlNode::XmlNode()
{
    Clear();
}

XmlNode::~XmlNode()
{
    Clear();
}

void XmlNode::Clear()
{
    m_ntType = XML_NODE;
    m_strTagName.Clear();
    m_strValue.Clear();
    m_mapProperties.Clear();
    m_listSubNodes.Clear();
}

XmlString XmlNode::GetXmlString(const XmlString &strLineEnd /*= L"\r\n"*/,
                                const XmlString &strIndent /*= L"\t"*/,
                                size_t nCount /*= 0*/)
{
    XmlString strRet;
    XmlString strIndents;

    for (size_t i = 0; i < nCount; ++i)
    {
        strIndents += strIndent;
    }

    if (m_ntType == XML_VALUE)
    {
        return strIndents + XmlStringTrans::XmlEncodeInnerXml(m_strValue);
    }
    else if (m_ntType == XML_CDATA)
    {
        return strIndents + L"<![CDATA[" + m_strValue + L"]]>";
    }

    strRet += strIndents;
    strRet += L"<";
    strRet += m_strTagName;

    for (XmlPropertyMap::Iterator i = m_mapProperties.Begin(); i != m_mapProperties.End(); ++i)
    {
        strRet += L" ";
        strRet += i->Key;
        strRet += L"=\"";
        strRet += XmlStringTrans::XmlEncodeProperty(i->Value);
        strRet += L"\"";
    }

    if (m_listSubNodes.Empty())
    {
        strRet += L" />";
        strRet += strLineEnd;

        return strRet;
    }

    strRet += L">";

    if (m_listSubNodes.Size() > 1 || (*m_listSubNodes.Begin())->m_ntType == XML_NODE)
    {
        strRet += strLineEnd;
        strRet += GetInnerXml(strLineEnd, strIndent, nCount + 1);
        strRet += strIndents;
    }
    else
    {
        strRet += GetInnerXml(strLineEnd, strIndent, 0);
    }

    strRet += L"</";
    strRet += m_strTagName;
    strRet += L">";
    strRet += strLineEnd;

    return strRet;
}

XmlNode::NODE_TYPE XmlNode::GetType()
{
    return m_ntType;
}

void XmlNode::SetType(NODE_TYPE type)
{
    m_ntType = type;

    if (m_ntType != XML_NODE)
    {
        m_strTagName.Clear();
        m_listSubNodes.Clear();
        m_mapProperties.Clear();
    }
    else
    {
        m_strValue.Clear();
    }
}


XmlString XmlNode::GetTagName()
{
    return m_strTagName;
}

bool XmlNode::SetTagName(const XmlString &strTagName)
{
    if (strTagName.Empty())
    {
        return false;
    }

    m_strTagName = strTagName;

    return true;
}

XmlString XmlNode::GetValue()
{
    if (m_ntType == XML_NODE)
    {
        return L"";
    }

    return m_strValue;
}

bool XmlNode::SetValue(const XmlString &strValue)
{
    if (m_ntType == XML_NODE)
    {
        return false;
    }

    m_strValue = strValue;

    return true;
}

XmlString XmlNode::GetInnerXml(const XmlString &strLineEnd /*= L"\r\n"*/,
                               const XmlString &strIndent /*= L"\t"*/,
                               size_t nCount /*= 0*/)
{
    XmlString strIndents;

    for (size_t i = 0; i < nCount; ++i)
    {
        strIndents += strIndent;
    }

    if (m_ntType != XML_NODE)
    {
        return L"";
    }

    XmlString strRet;

    for (XmlNodeList::Iterator i = m_listSubNodes.Begin(); i != m_listSubNodes.End(); ++i)
    {
        strRet += (*i)->GetXmlString(strLineEnd, strIndent, nCount);
    }

    return strRet;
}

bool XmlNode::SetInnerXml(const XmlString &strInnerXml)
{
    XmlInstList xmlInstList;

    if(!XmlParser::ParseXml(strInnerXml, &xmlInstList, &m_listSubNodes))
    {
        return false;
    }

    return true;
}


XmlPropertyMap &XmlNode::Properties()
{
    return m_mapProperties;
}

XmlNodeList &XmlNode::SubNodes()
{
    return m_listSubNodes;
}

//////////////////////////////////////////////////////////////////////////
// XmlInst

XmlInst::XmlInst()
{
    Clear();
}

XmlInst::~XmlInst()
{
    Clear();
}

void XmlInst::Clear()
{
    m_strTagName.Clear();
    m_mapProperties.Clear();
}

XmlString XmlInst::GetXmlString(const XmlString &strLineEnd /*= L"\r\n"*/)
{
    XmlString strRet;

    strRet += L"<?";
    strRet += m_strTagName;

    for (XmlPropertyMap::Iterator i = m_mapProperties.Begin(); i != m_mapProperties.End(); ++i)
    {
        strRet += L" ";
        strRet += i->Key;
        strRet += L"=\"";
        strRet += XmlStringTrans::XmlEncodeProperty(i->Value);
        strRet += L"\"";
    }

    strRet += L"?>";
    strRet += strLineEnd;

    return strRet;
}

XmlString XmlInst::GetTagName()
{
    return m_strTagName;
}

bool XmlInst::SetTagName(const XmlString &strTagName)
{
    if (strTagName != L"xml")
    {
        return false;
    }

    m_strTagName = strTagName;

    return true;

}

XmlPropertyMap &XmlInst::Properties()
{
    return m_mapProperties;
}


//////////////////////////////////////////////////////////////////////////
// XmlParserEx

/*
    <XmlDoc>        ->    <InstList><NodeList> | <NodeList>
    <InstList>        ->    <Inst><Inst>...<Inst>
    <Inst>            ->    <InstStart><NodeName><PropertyList><InstEnd>
    <NodeList>        ->    <Node><Node>...<Node>
    <Node>            ->    <NodeValue> |
                        <CDATA>    |
                        <NodeStartStart><NodeName><PropertyList><NodeSingleEnd> |
                        <NodeStartStart><NodeName><PropertyList><NodeEnd><NodeList><NodeEndStart><NodeName><NodeEnd>
    <PropertyList>    ->    <Property><Property>...<Property>
    <Property>        ->    <PropertyName><Equal><Quote><PropertyValue><Quote>

    <InstStart>        ->    "<?"
    <InstEnd>        ->    "?>"
    <NodeStartStart>->    "<"
    <NodeEnd>        ->    ">"
    <NodeSingleEnd>    ->    "/>"
    <NodeEndStart>    ->    "</"
    <NodeEnd>        ->    ">"
    <Equal>            ->    "="
    <Quote>            ->    "\""
    <NodeName>        ->    [A-Za-z0-9]+
    <PropertyName>    ->    [A-Za-z0-9]+
    <PropertyValue>    ->    [^"]*
    <NodeValue>        ->    [^<>]*
    <CDATA>            ->    <![CDATA[...]]>
*/

class XmlParserEx
{
public:
    XmlParserEx();
    ~XmlParserEx();

public:
    void ClearState();

private:
    enum XML_TOKEN
    {
        TOKEN_BOF,
        TOKEN_EOF,
        TOKEN_BLANK,
        TOKEN_INST_START,
        TOKEN_INST_END,
        TOKEN_NODE_START_START,
        TOKEN_NODE_SINGLE_END,
        TOKEN_NODE_END,
        TOKEN_NODE_END_START,
        TOKEN_EQUAL,
        TOKEN_QUOTE,
        TOKEN_NODE_NAME,
        TOKEN_PROPERTY_NAME,
        TOKEN_PROPERTY_VALUE,
        TOKEN_NODE_VALUE,
        TOKEN_CDATA_START,
        TOKEN_CDATA_END,
        TOKEN_UNKNOWN,
        TOKEN_MAX
    };

public:
    bool Parse(const XmlString &strXml, XmlInstList *pXmlInstList, XmlNodeList *pXmlNodeList);

private:
    XmlChar GetTransChar();
private:
    bool IsBlank(XmlChar ch);
    int CommentLength(int nPos);
    int BlankLength(int nPos);
    int BlankAndCommentLength(int nPos);
    void IgnoreBlankAndComment();
    void LookAhead();
    bool Match(XML_TOKEN token);
    bool MatchName(XmlString &strName);
    bool MatchNodeValue(XmlString &strNodeValue);
    bool MatchPropertyValue(XmlString &strPropertyValue);
    bool MatchCdata(XmlString &strCdata);

private:
    bool InstList(XmlInstList &xmlInstList);
    bool Inst(XmlInstPtr &xmlInstPtr);
    bool NodeList(XmlNodeList &xmlNodeList);
    bool Node(XmlNodePtr &xmlNodePtr);
    bool PropertyList(XmlPropertyMap &xmlPropertyMap);
    bool Property(XmlProperty &xmlProperty);

private:
    XmlString m_strXml;
    XML_TOKEN m_tLookAhead;
    int m_nPos;
};

XmlParserEx::XmlParserEx()
{
    ClearState();
}

XmlParserEx::~XmlParserEx()
{
    ClearState();
}

void XmlParserEx::ClearState()
{
    m_strXml.Clear();
    m_tLookAhead = TOKEN_BOF;
    m_nPos = 0;
}

bool XmlParserEx::Parse(const XmlString &strXml, XmlInstList *pXmlInstList, XmlNodeList *pXmlNodeList)
{
    ClearState();

    m_strXml = strXml;

    LookAhead();

    XmlInstList xmlInstList;
    
    if (pXmlInstList == nullptr)
    {
        pXmlInstList = &xmlInstList;
    }

    if (!InstList(*pXmlInstList) || (m_tLookAhead != TOKEN_EOF && m_tLookAhead != TOKEN_NODE_START_START))
    {
        pXmlInstList->Clear();
    }

    if (pXmlNodeList != nullptr)
    {
        if (!NodeList(*pXmlNodeList) || m_tLookAhead != TOKEN_EOF)
        {
            pXmlNodeList->Clear();

            return false;
        }
    }
    
    return true;
}

XmlChar XmlParserEx::GetTransChar()
{
    if (m_nPos + 4 < m_strXml.Length() &&
        m_strXml[m_nPos + 1] == L'a' && m_strXml[m_nPos + 2] == L'm' && m_strXml[m_nPos + 3] == L'p' && m_strXml[m_nPos + 4] == L';')
    {
        m_nPos += 5;

        return L'&';
    }
    else if (m_nPos + 3 < m_strXml.Length() &&
        m_strXml[m_nPos + 1] == L'l' && m_strXml[m_nPos + 2] == L't' && m_strXml[m_nPos + 3] == L';')
    {
        m_nPos += 4;

        return L'<';
    }
    else if (m_nPos + 3 < m_strXml.Length() &&
        m_strXml[m_nPos + 1] == L'g' && m_strXml[m_nPos + 2] == L't' && m_strXml[m_nPos + 3] == L';')
    {
        m_nPos += 4;

        return L'>';
    }
    else if (m_nPos + 5 < m_strXml.Length() &&
        m_strXml[m_nPos + 1] == L'q' && m_strXml[m_nPos + 2] == L'u' && m_strXml[m_nPos + 3] == L'o' && m_strXml[m_nPos + 4] == L't' && m_strXml[m_nPos + 5] == L';')
    {
        m_nPos += 6;

        return L'\"';
    }
    else if (m_nPos + 5 < m_strXml.Length() &&
        m_strXml[m_nPos + 1] == L'a' && m_strXml[m_nPos + 2] == L'p' && m_strXml[m_nPos + 3] == L'o' && m_strXml[m_nPos + 4] == L's' && m_strXml[m_nPos + 5] == L';')
    {
        m_nPos += 6;

        return L'\'';
    }

    return L'\0';
}

bool XmlParserEx::IsBlank(XmlChar ch)
{
    switch (ch)
    {
    case L' ':
    case L'\t':
    case L'\r':
    case L'\n':
        break;
    default:
        return false;
    }

    return true;
}

int XmlParserEx::CommentLength(int nPos)
{
    int nRet = 0;

    if (nPos + nRet + 3 < m_strXml.Length() &&
        m_strXml[nPos + nRet] == L'<' && m_strXml[nPos + nRet + 1] == L'!' && m_strXml[nPos + nRet + 2] == L'-' && m_strXml[nPos + nRet + 3] == L'-')
    {
        while (nPos + nRet + 2 < m_strXml.Length() &&
                !(m_strXml[nPos + nRet] == L'-' && m_strXml[nPos + nRet + 1] == L'-' && m_strXml[nPos + nRet + 2] == L'>'))
        {
            ++nRet;
        }

        nRet += 3;

    }

    return nRet;
}

int XmlParserEx::BlankLength(int nPos)
{
    int nRet = 0;

    while (nPos + nRet < m_strXml.Length() && IsBlank(m_strXml[nPos + nRet]))
    {
        ++nRet;
    }

    return nRet;
}

int XmlParserEx::BlankAndCommentLength(int nPos)
{
    int nRet = BlankLength(nPos);
    int nCommentLen = CommentLength(nPos + nRet);

    while (nCommentLen > 0)
    {
        nRet += nCommentLen;
        nRet += BlankLength(nPos + nRet);
        nCommentLen = CommentLength(nPos + nRet);
    }

    return nRet;
}
void XmlParserEx::IgnoreBlankAndComment()
{
    m_nPos += BlankAndCommentLength(m_nPos);
}

void XmlParserEx::LookAhead()
{
    int nCurPos = m_nPos + BlankAndCommentLength(m_nPos);

    if (nCurPos >= m_strXml.Length())
    {
        m_tLookAhead = TOKEN_EOF;

        return;
    }

    // Len-1 Tokens

    if (m_strXml[nCurPos] == L'<' && (nCurPos + 1 >= m_strXml.Length() || (m_strXml[nCurPos + 1] != L'/' && m_strXml[nCurPos + 1] != L'!' && m_strXml[nCurPos + 1] != L'?')))
    {
        m_tLookAhead = TOKEN_NODE_START_START;

        return;
    }

    if (m_strXml[nCurPos] == L'=')
    {
        m_tLookAhead = TOKEN_EQUAL;

        return;
    }

    if (m_strXml[nCurPos] == L'>')
    {
        m_tLookAhead = TOKEN_NODE_END;

        return;
    }

    if (m_strXml[nCurPos] == L'\"')
    {
        m_tLookAhead = TOKEN_QUOTE;

        return;
    }

    // Len-2 Tokens

    if (nCurPos + 1 < m_strXml.Length())
    {
        if (m_strXml[nCurPos] == L'?' && m_strXml[nCurPos + 1] == L'>')
        {
            m_tLookAhead = TOKEN_INST_END;

            return;
        }

        if (m_strXml[nCurPos] == L'/' && m_strXml[nCurPos + 1] == L'>')
        {
            m_tLookAhead = TOKEN_NODE_SINGLE_END;

            return;
        }

        if (m_strXml[nCurPos] == L'<' && m_strXml[nCurPos + 1] == L'?')
        {
            m_tLookAhead = TOKEN_INST_START;

            return;
        }

        if (m_strXml[nCurPos] == L'<' && m_strXml[nCurPos + 1] == L'/')
        {
            m_tLookAhead = TOKEN_NODE_END_START;

            return;
        }
    }

    // Len-3 Tokens

    if (nCurPos + 2 < m_strXml.Length())
    {
        if (m_strXml[nCurPos] == L']' && m_strXml[nCurPos + 1] == L']' && m_strXml[nCurPos + 2] == L'>')
        {
            m_tLookAhead = TOKEN_CDATA_END;

            return;
        }

    }

    // Len-9 Tokens

    if (nCurPos + 8 < m_strXml.Length())
    {
        if (m_strXml[nCurPos] == L'<' && m_strXml[nCurPos + 1] == L'!' && m_strXml[nCurPos + 2] == L'[' &&    m_strXml[nCurPos + 3] == L'C' &&
            m_strXml[nCurPos + 4] == L'D' && m_strXml[nCurPos + 5] == L'A' && m_strXml[nCurPos + 6] == L'T' && m_strXml[nCurPos + 7] == L'A' &&
            m_strXml[nCurPos + 8] == L'[')
        {
            m_tLookAhead = TOKEN_CDATA_START;

            return;
        }

    }

    m_tLookAhead = TOKEN_UNKNOWN;
}

bool XmlParserEx::Match(XML_TOKEN token)
{
    IgnoreBlankAndComment();

    switch (token)
    {
    case TOKEN_INST_START:
        if (m_tLookAhead != TOKEN_INST_START)
        {
            return false;
        }

        m_nPos += 2;

        break;

    case TOKEN_INST_END:
        if (m_tLookAhead != TOKEN_INST_END)
        {
            return false;
        }

        m_nPos += 2;

        break;

    case TOKEN_NODE_START_START:
        if (m_tLookAhead != TOKEN_NODE_START_START)
        {
            return false;
        }

        m_nPos += 1;

        break;

    case TOKEN_NODE_SINGLE_END:
        if (m_tLookAhead != TOKEN_NODE_SINGLE_END)
        {
            return false;
        }

        m_nPos += 2;

        break;

    case TOKEN_NODE_END:
        if (m_tLookAhead != TOKEN_NODE_END)
        {
            return false;
        }

        m_nPos += 1;

        break;

    case TOKEN_NODE_END_START:
        if (m_tLookAhead != TOKEN_NODE_END_START)
        {
            return false;
        }

        m_nPos += 2;

        break;

    case TOKEN_EQUAL:
        if (m_tLookAhead != TOKEN_EQUAL)
        {
            return false;
        }

        m_nPos += 1;

        break;

    case TOKEN_QUOTE:
        if (m_tLookAhead != TOKEN_QUOTE)
        {
            return false;
        }

        m_nPos += 1;

        break;

    case TOKEN_CDATA_START:
        if (m_tLookAhead != TOKEN_CDATA_START)
        {
            return false;
        }

        m_nPos += 9;

        break;

    case TOKEN_CDATA_END:
        if (m_tLookAhead != TOKEN_CDATA_END)
        {
            return false;
        }

        m_nPos += 3;

        break;

    default:
        return false;
    }

    LookAhead();

    return true;
}

bool XmlParserEx::MatchName(XmlString &strName)
{
    IgnoreBlankAndComment();

    strName.Clear();

    while (m_nPos < m_strXml.Length() && !IsBlank(m_strXml[m_nPos]))
    {
        if (m_strXml[m_nPos] == L'<' ||
            m_strXml[m_nPos] == L'>' ||
            m_strXml[m_nPos] == L'/' ||
            m_strXml[m_nPos] == L'=' ||
            m_strXml[m_nPos] == L'\"' ||
            m_strXml[m_nPos] == L'?' ||
            IsBlank(m_strXml[m_nPos]))
        {
            break;
        }
        else
        {
            strName.AppendBack(m_strXml[m_nPos++], 1);
        }
    }

    LookAhead();

    return !strName.Empty();
}

bool XmlParserEx::MatchNodeValue(XmlString &strNodeValue)
{
    m_nPos += BlankLength(m_nPos);

    strNodeValue.Clear();

    while (m_nPos < m_strXml.Length())
    {
        if (m_strXml[m_nPos] == L'<')
        {
            size_t nCommentLen = CommentLength(m_nPos);

            if (nCommentLen == 0)
            {
                break;
            }

            m_nPos += (int)nCommentLen;
        }
        else if (m_strXml[m_nPos] == L'&')
        {
            XmlChar chTrans = GetTransChar();

            if (chTrans == L'\0')
            {
                return false;
            }

            strNodeValue.AppendBack(chTrans, 1);
        }
        else
        {
            strNodeValue.AppendBack(m_strXml[m_nPos++], 1);
        }
    }

    int i = strNodeValue.Length();

    while (!strNodeValue.Empty())
    {
        if (!IsBlank(strNodeValue[i - 1]))
        {
            break;
        }

        --i;
    }

    if (i != strNodeValue.Length())
    {
        strNodeValue = strNodeValue.Left(i);
    }

    LookAhead();

    return true;
}

bool XmlParserEx::MatchPropertyValue(XmlString &strPropertyValue)
{
    strPropertyValue.Clear();

    while (m_nPos < m_strXml.Length())
    {
        if (m_strXml[m_nPos] == L'\"')
        {
            break;
        }
        else if (m_strXml[m_nPos] == L'&')
        {
            XmlChar chTrans = GetTransChar();

            if (chTrans == L'\0')
            {
                return false;
            }

            strPropertyValue.AppendBack(chTrans, 1);
        }
        else
        {
            strPropertyValue.AppendBack(m_strXml[m_nPos++], 1);
        }
    }

    LookAhead();

    return true;
}

bool XmlParserEx::MatchCdata(XmlString &strCdata)
{
    strCdata.Clear();

    while (m_nPos < m_strXml.Length())
    {
        if (m_nPos + 2 < m_strXml.Length() && m_strXml[m_nPos] == L']' && m_strXml[m_nPos + 1] == L']' &&  m_strXml[m_nPos + 2] == L'>')
        {
            break;
        }
        else
        {
            strCdata.AppendBack(m_strXml[m_nPos++], 1);
        }
    }

    LookAhead();

    return true;
}

bool XmlParserEx::InstList(XmlInstList &xmlInstList)
{
/*
    <InstList>    ->    <Inst><Inst>...<Inst>
*/
    xmlInstList.Clear();

    XmlInstPtr xmlInstPtr;

    while (Inst(xmlInstPtr))
    {
        xmlInstList.PushBack(xmlInstPtr);
    }

    return true;
}

bool XmlParserEx::Inst(XmlInstPtr &xmlInstPtr)
{
/*
    <Inst>        ->    <InstStart><NodeName><PropertyList><InstEnd>
*/
    XmlInstPtr pInst = new XmlInst;

    if (!Match(TOKEN_INST_START))
    {
        return false;
    }

    XmlString strNodeName;

    if (!MatchName(strNodeName))
    {
        return false;
    }

    pInst->SetTagName(strNodeName);

    if (!PropertyList(pInst->Properties()))
    {
        return false;
    }

    if (!Match(TOKEN_INST_END))
    {
        return false;
    }

    xmlInstPtr = pInst;

    return true;
}

bool XmlParserEx::NodeList(XmlNodeList &xmlNodeList)
{
/*
    <NodeList>    ->    <Node><Node>...<Node>
*/
    xmlNodeList.Clear();

    XmlNodePtr xmlNodePtr;

    int nPos = m_nPos;
    int nCount = 0;

    while (Node(xmlNodePtr))
    {
        xmlNodeList.PushBack(xmlNodePtr);

        if (m_nPos == nPos && nCount++ > 0)
        {
            return false;
        }
    }

    return true;
}

bool XmlParserEx::Node(XmlNodePtr &xmlNodePtr)
{
/*
    <Node>    ->    <NodeValue> |
                    <Cdata> |
                    <NodeStartStart><NodeName><PropertyList><NodeSingleEnd> |
                    <NodeStartStart><NodeName><PropertyList><NodEnd><NodeList><NodeEndStart><NodeName><NodeEnd>
*/
    XmlNodePtr pNode = new XmlNode;

    if (!Match(TOKEN_NODE_START_START) && m_tLookAhead != TOKEN_NODE_END_START && m_tLookAhead != TOKEN_EOF)
    {
        XmlString strNodeValue;

        if (Match(TOKEN_CDATA_START))
        {
            if (!MatchCdata(strNodeValue))
            {
                return false;
            }

            if (!Match(TOKEN_CDATA_END))
            {
                return false;
            }

            pNode->SetType(XmlNode::XML_CDATA);
        }
        else
        {
            if (!MatchNodeValue(strNodeValue))
            {
                return false;
            }

            pNode->SetType(XmlNode::XML_VALUE);

        }

        pNode->SetValue(strNodeValue);
        xmlNodePtr = pNode;

        return true;
    }

    XmlString strNodeName;

    if (!MatchName(strNodeName))
    {
        return false;
    }

    pNode->SetTagName(strNodeName);

    if (!PropertyList(pNode->Properties()))
    {
        return false;
    }

    do 
    {
        if (Match(TOKEN_NODE_SINGLE_END))
        {
            break;
        }

        if (Match(TOKEN_NODE_END))
        {
            if (m_tLookAhead != TOKEN_NODE_END_START)
            {
                NodeList(pNode->SubNodes());
            }
            
            if (!Match(TOKEN_NODE_END_START))
            {
                return false;
            }

            XmlString strNodeNameEnd;

            if (!MatchName(strNodeNameEnd) || strNodeNameEnd != strNodeName)
            {
                return false;
            }

            if (!Match(TOKEN_NODE_END))
            {
                return false;
            }

            break;
        }

        return false;

    } while (false);

    xmlNodePtr = pNode;

    return true;
}

bool XmlParserEx::PropertyList(XmlPropertyMap &xmlPropertyMap)
{
/*
    <PropertyList>    ->    <Property><Property>...<Property>
*/
    xmlPropertyMap.Clear();

    XmlProperty xmlProperty;

    while (Property(xmlProperty))
    {
        xmlPropertyMap.Insert(xmlProperty);
    }

    return true;
}

bool XmlParserEx::Property(XmlProperty &xmlProperty)
{
/*
    <Property>    ->    <PropertyName><Equal><Quote><PropertyValue><Quote>
*/
    XmlString strPropertyName;

    if (!MatchName(strPropertyName))
    {
        return false;
    }

    if (!Match(TOKEN_EQUAL))
    {
        return false;
    }

    if (!Match(TOKEN_QUOTE))
    {
        return false;
    }

    XmlString strPropertyValue;

    if (!MatchPropertyValue(strPropertyValue))
    {
        return false;
    }

    if (!Match(TOKEN_QUOTE))
    {
        return false;
    }

    xmlProperty.Key = strPropertyName;
    xmlProperty.Value = strPropertyValue;

    return true;
}

//////////////////////////////////////////////////////////////////////////
// XmlParser


bool XmlParser::ParseXml(const XmlString &strXml, XmlInstList *pXmlInstList, XmlNodeList *pXmlNodeList)
{
    XmlParserEx parser;
    return parser.Parse(strXml, pXmlInstList, pXmlNodeList);
}

bool XmlParser::ParseXmlFromFile(XmlNode &xml)
{
    return true;
}

bool XmlParser::WriteXmlToFile(const XmlNode &xml)
{
    return true;
}


