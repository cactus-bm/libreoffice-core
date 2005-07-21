/*************************************************************************
 *
 *  $RCSfile: LocaleNode.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: obo $ $Date: 2005-07-21 14:28:03 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <set>

#include <rtl/ustrbuf.hxx>

#include "LocaleNode.hxx"

#ifndef _COM_SUN_STAR_I18N_NUMBERFORMATINDEX_HPP_
#include <com/sun/star/i18n/NumberFormatIndex.hpp>
#endif

// NOTE: MUST match the Locale versionDTD attribute defined in data/locale.dtd
#define LOCALE_VERSION_DTD "2.0"

typedef ::std::set< ::rtl::OUString > NameSet;
typedef ::std::set< sal_Int16 > ValueSet;

namespace cssi = ::com::sun::star::i18n;

LocaleNode::LocaleNode (const OUString& name, const Reference< XAttributeList > & attr)
    : aName(name)
    , xAttribs(new Attr(attr))
    , parent(0)
    , children(0)
    , nChildren(0)
    , childArrSize(0)
    , nError(0)
{
}

int LocaleNode::getError() const
{
    int err = nError;
    for (sal_Int32 i=0;i<nChildren;i++)
        err += children[i]->getError();
    return err;
}

void LocaleNode::print () const {
    printf ("<");
    ::rtl::OUString str (aName);
    for(sal_Int32 i = 0; i < str.getLength(); i++)
        printf( "%c", str[i]);
    printf (">\n");
}

void LocaleNode::printR () const {
    print();
    for (sal_Int32 i=0;i<nChildren;i++)
        children[i]->printR();
    printf ("\t");
    print();
}

void LocaleNode::addChild ( LocaleNode * node) {
    if (childArrSize <= nChildren) {
        LocaleNode ** arrN = (LocaleNode **)malloc( sizeof (LocaleNode *)*(childArrSize+10) ) ;
        for (sal_Int32 i = 0; i<childArrSize ; i++)
            arrN[i] = children[i];
        if ( childArrSize > 0 )
            free(children);
        childArrSize += 10;
        children = arrN;
    }
    children[nChildren++] = node;
    node->setParent (this);
}

void LocaleNode::setParent ( LocaleNode * node) {
    parent = node;
}

const LocaleNode* LocaleNode::getRoot() const
{
    const LocaleNode* pRoot = 0;
    const LocaleNode* pParent = this;
    while (pParent = pParent->getParent())
        pRoot = pParent;
    return pRoot;
}

const LocaleNode * LocaleNode::findNode ( const sal_Char *name) const {
    if (aName.equalsAscii(name))
        return this;
    for (sal_Int32 i = 0; i< nChildren; i++)  {
        const LocaleNode *n=children[i]->findNode(name);
        if (n)
            return n;
        }
    return 0;
}

 LocaleNode::~LocaleNode() {
    for (sal_Int32 i=0; i<nChildren;i++)
        delete (children[i]);
}

LocaleNode* LocaleNode::createNode (const OUString& name, const Reference< XAttributeList > & attr)
{
    if (name.equalsAscii("LC_INFO"))
        return new LCInfoNode (name,attr);
    if (name.equalsAscii("LC_CTYPE"))
        return new LCCTYPENode (name,attr);
    if (name.equalsAscii("LC_FORMAT"))
        return new LCFormatNode (name,attr);
    if (name.equalsAscii("LC_CALENDAR"))
        return new LCCalendarNode (name,attr);
    if (name.equalsAscii("LC_CURRENCY"))
        return new LCCurrencyNode (name,attr);
    if (name.equalsAscii("LC_TRANSLITERATION"))
        return new LCTransliterationNode (name,attr);
    if (name.equalsAscii("LC_COLLATION"))
        return new LCCollationNode (name,attr);
    if (name.equalsAscii("LC_INDEX"))
        return new LCIndexNode (name,attr);
    if (name.equalsAscii("LC_SEARCH"))
        return new LCSearchNode (name,attr);
    if (name.equalsAscii("LC_MISC"))
        return new LCMiscNode (name,attr);
      if (name.equalsAscii("LC_NumberingLevel"))
                return new LCNumberingLevelNode (name, attr);
      if (name.equalsAscii("LC_OutLineNumberingLevel"))
                return new LCOutlineNumberingLevelNode (name, attr);

    return new LocaleNode(name,attr);
}


//   printf(" name: '%s'\n", p->getName().pData->buffer );
//   printf("value: '%s'\n", p->getValue().pData->buffer );

void print_OUString( const OUString& s )
{
    printf( "%s", OUStringToOString( s, RTL_TEXTENCODING_UTF8).getStr());
}

bool is_empty( const OUString& s )
{
     return (s.getLength()==0) || (s.getLength()==1 && s[0]=='\n');
}

void print_indent( int depth )
{
     for( int i=0; i<depth; i++ ) printf("    ");
}

void print_color( int color )
{
     printf("\033[%dm", color);
}

void print_node( const LocaleNode* p, int depth=0 )
{
     if( !p ) return;

     print_indent( depth );
     printf("<");
     print_color(36);
     print_OUString( p->getName()  );
     print_color(0);
     const Attr* q = p->getAttr();
     if( q )
     {
          for( sal_Int32 j=0; j<q->getLength(); j++ )
          {
               printf(" ");
               print_color(33);
               print_OUString( q->getTypeByIndex(j) );
               print_color(0);
               printf("=");
               print_color(31);
               printf("'");
               print_OUString( q->getValueByIndex(j) );
               printf("'");
               print_color(0);
          }
     }
     printf(">");
     printf("\n");
     if( !is_empty( p->getValue() ) )
     {
          print_indent( depth+1 );
          printf("value: ");
          print_color(31);
          printf("'");
          print_OUString( p->getValue() );
          printf("'");
          print_color(0);
          printf("\n");
     }
     for( sal_Int32 i=0; i<p->getNumberOfChildren(); i++ )
     {
          print_node( p->getChildAt(i), depth+1 );
     }
     print_indent( depth );
     printf("</");
     print_OUString( p->getName()  );
     printf(">");
     printf("\n");
}

void LocaleNode :: generateCode (const OFileWriter &of) const
{
    ::rtl::OUString aDTD = getAttr()->getValueByName("versionDTD");
    if (!aDTD.equalsAscii( LOCALE_VERSION_DTD))
    {
        ++nError;
        fprintf( stderr, "Error: Locale versionDTD is not %s, see comment in locale.dtd\n", LOCALE_VERSION_DTD);
    }
    for (sal_Int32 i=0; i<nChildren;i++)
        children[i]->generateCode (of);
//      print_node( this );
}


::rtl::OUString LocaleNode::writeParameterCheckLen( const OFileWriter &of,
        const char* pParameterName, const LocaleNode* pNode,
        sal_Int32 nMinLen, sal_Int32 nMaxLen ) const
{
    OUString aValue;
    if (pNode)
        aValue = pNode->getValue();
    else
    {
        ++nError;
        fprintf( stderr, "Error: node NULL pointer for parameter %s.\n",
                pParameterName);
    }
    // write empty data if error
    of.writeParameter( pParameterName, aValue);
    sal_Int32 nLen = aValue.getLength();
    if (nLen < nMinLen)
    {
        ++nError;
        fprintf( stderr, "Error: less than %d character%s (%d) in %s '%s'.\n",
                nMinLen, (nMinLen > 1 ? "s" : ""), nLen,
                (pNode ? OUStringToOString( pNode->getName(), RTL_TEXTENCODING_UTF8).getStr() : ""),
                OUStringToOString( aValue, RTL_TEXTENCODING_UTF8).getStr());
    }
    else if (nLen > nMaxLen && nMaxLen >= 0)
        fprintf( stderr,
                "Warning: more than %d character%s (%d) in %s %s not supported by application.\n",
                nMaxLen, (nMaxLen > 1 ? "s" : ""), nLen,
                (pNode ? OUStringToOString( pNode->getName(), RTL_TEXTENCODING_UTF8).getStr() : ""),
                OUStringToOString( aValue, RTL_TEXTENCODING_UTF8).getStr());
    return aValue;
}


::rtl::OUString LocaleNode::writeParameterCheckLen( const OFileWriter &of,
        const char* pNodeName, const char* pParameterName,
        sal_Int32 nMinLen, sal_Int32 nMaxLen ) const
{
    OUString aValue;
    const LocaleNode * pNode = findNode( pNodeName);
    if (pNode)
        aValue = writeParameterCheckLen( of, pParameterName, pNode, nMinLen, nMaxLen);
    else
    {
        ++nError;
        fprintf( stderr, "Error: node %s not found.\n", pNodeName);
        // write empty data if error
        of.writeParameter( pParameterName, aValue);
    }
    return aValue;
}

void LocaleNode::incError( const char* pStr ) const
{
    ++nError;
    fprintf( stderr, "Error: %s\n", pStr);
}

char* LocaleNode::prepareErrorFormat( const char* pFormat, const char* pDefaultConversion ) const
{
    static char buf[2048];
    strcpy( buf, "Error: ");
    strncat( buf, pFormat, 2000);
    char* p = buf;
    while (((p = strchr( p, '%')) != 0) && p[1] == '%')
        p += 2;
    if (!p)
        strcat( buf, pDefaultConversion);
    strcat( buf, "\n");
    return buf;
}

void LocaleNode::incErrorInt( const char* pStr, int nVal ) const
{
    ++nError;
    fprintf( stderr, prepareErrorFormat( pStr, ": %d"), nVal);
}

void LocaleNode::incErrorStr( const char* pStr, const ::rtl::OUString& rVal ) const
{
    ++nError;
    fprintf( stderr, prepareErrorFormat( pStr, ": %s"), OUStringToOString(
                rVal, RTL_TEXTENCODING_UTF8).getStr());
}

void LCInfoNode::generateCode (const OFileWriter &of) const
{

    const LocaleNode * languageNode = findNode("Language");
    const LocaleNode * countryNode = findNode("Country");
    const LocaleNode * variantNode = findNode("Variant");

    if (languageNode)
    {
        writeParameterCheckLen( of, "langID", languageNode->getChildAt(0), 2, -1);
        of.writeParameter("langDefaultName", languageNode->getChildAt(1)->getValue());
    }
    else
        incError( "No Language node.");
    if (countryNode)
    {
        of.writeParameter("countryID", countryNode->getChildAt(0)->getValue());
        of.writeParameter("countryDefaultName", countryNode->getChildAt(1)->getValue());
    }
    else
        incError( "No Country node.");
    if (variantNode)
    {
        of.writeParameter("Variant", variantNode->getValue());
        fprintf( stderr, "Warning: %s\n",
                "Variants are not supported by application.");
    }
    else
        of.writeParameter("Variant", ::rtl::OUString());
    of.writeAsciiString("\nstatic const sal_Unicode* LCInfoArray[] = {\n");
    of.writeAsciiString("\tlangID,\n");
    of.writeAsciiString("\tlangDefaultName,\n");
    of.writeAsciiString("\tcountryID,\n");
    of.writeAsciiString("\tcountryDefaultName,\n");
    of.writeAsciiString("\tVariant\n");
    of.writeAsciiString("};\n\n");
    of.writeFunction("getLCInfo_", "0", "LCInfoArray");
}

void LCCTYPENode::generateCode (const OFileWriter &of) const
{
    const LocaleNode * sepNode = 0;
    ::rtl::OUString useLocale =   getAttr() -> getValueByName("ref");
    if (useLocale.getLength() > 0) {
        of.writeRefFunction("getLocaleItem_", useLocale);
        return;
    }
    ::rtl::OUString str =   getAttr() -> getValueByName("unoid");
    of.writeAsciiString("\n\n");
    of.writeParameter("LC_CTYPE_Unoid", str);;

    OUString aDateSep =
        writeParameterCheckLen( of, "DateSeparator", "dateSeparator", 1, 1);
    OUString aThoSep =
        writeParameterCheckLen( of, "ThousandSeparator", "thousandSeparator", 1, 1);
    OUString aDecSep =
        writeParameterCheckLen( of, "DecimalSeparator", "decimalSeparator", 1, 1);
    OUString aTimeSep =
        writeParameterCheckLen( of, "TimeSeparator", "timeSeparator", 1, 1);
    OUString aTime100Sep =
        writeParameterCheckLen( of, "Time100SecSeparator", "time100SecSeparator", 1, 1);
    OUString aListSep =
        writeParameterCheckLen( of, "ListSeparator", "listSeparator", 1, 1);

    int nSavErr = nError;
    int nWarn = 0;
    if (aDateSep == aTimeSep)
        incError( "DateSeparator equals TimeSeparator.");
    if (aDecSep == aThoSep)
        incError( "DecimalSeparator equals ThousandSeparator.");
    if (aListSep == aDecSep)
        fprintf( stderr, "Warning: %s\n",
                "ListSeparator equals DecimalSeparator.");
    if (aListSep == aThoSep)
        fprintf( stderr, "Warning: %s\n",
                "ListSeparator equals ThousandSeparator.");
    if (aTimeSep == aTime100Sep)
        ++nWarn, fprintf( stderr, "Warning: %s\n",
                "Time100SecSeparator equals TimeSeparator, this is probably an error.");
    if (aDecSep != aTime100Sep)
        ++nWarn, fprintf( stderr, "Warning: %s\n",
                "DecimalSeparator is different from Time100SecSeparator, this may be correct or not.");
    if (aThoSep.equalsAscii( " "))
        ++nWarn, fprintf( stderr, "Warning: %s\n",
                "ThousandSeparator is an ' ' ordinary space, this should be a non-breaking space U+00A0 instead.");
    if (nSavErr != nError || nWarn)
        fprintf( stderr, "Warning: %s\n",
                "Don't forget to adapt FormatCode elements when changing separators.");

    writeParameterCheckLen( of, "QuotationStart", "quotationStart", 1, 1);
    writeParameterCheckLen( of, "QuotationEnd", "quotationEnd", 1, 1);
    writeParameterCheckLen( of, "DoubleQuotationStart", "doubleQuotationStart", 1, 1);
    writeParameterCheckLen( of, "DoubleQuotationEnd", "doubleQuotationEnd", 1, 1);
    writeParameterCheckLen( of, "TimeAM", "timeAM", 1, -1);
    writeParameterCheckLen( of, "TimePM", "timePM", 1, -1);
    sepNode = findNode("MeasurementSystem");
    of.writeParameter("measurementSystem", sepNode->getValue());
    sepNode = findNode("LongDateDayOfWeekSeparator");
    of.writeParameter("LongDateDayOfWeekSeparator", sepNode->getValue());
    sepNode = findNode("LongDateDaySeparator");
    of.writeParameter("LongDateDaySeparator", sepNode->getValue());
    sepNode = findNode("LongDateMonthSeparator");
    of.writeParameter("LongDateMonthSeparator", sepNode->getValue());
    sepNode = findNode("LongDateYearSeparator");
    of.writeParameter("LongDateYearSeparator", sepNode->getValue());

    of.writeAsciiString("\nstatic const sal_Unicode* LCType[] = {\n");
    of.writeAsciiString("\tLC_CTYPE_Unoid,\n");
    of.writeAsciiString("\tdateSeparator,\n");
    of.writeAsciiString("\tthousandSeparator,\n");
    of.writeAsciiString("\tdecimalSeparator,\n");
    of.writeAsciiString("\ttimeSeparator,\n");
    of.writeAsciiString("\ttime100SecSeparator,\n");
    of.writeAsciiString("\tlistSeparator,\n");
    of.writeAsciiString("\tquotationStart,\n");
    of.writeAsciiString("\tquotationEnd,\n");
    of.writeAsciiString("\tdoubleQuotationStart,\n");
    of.writeAsciiString("\tdoubleQuotationEnd,\n");
    of.writeAsciiString("\ttimeAM,\n");
    of.writeAsciiString("\ttimePM,\n");
    of.writeAsciiString("\tmeasurementSystem,\n");
    of.writeAsciiString("\tLongDateDayOfWeekSeparator,\n");
    of.writeAsciiString("\tLongDateDaySeparator,\n");
    of.writeAsciiString("\tLongDateMonthSeparator,\n");
    of.writeAsciiString("\tLongDateYearSeparator\n");
    of.writeAsciiString("};\n\n");
    of.writeFunction("getLocaleItem_", "0", "LCType");
}

void LCFormatNode::generateCode (const OFileWriter &of) const
{
    of.writeParameter("replaceFrom", getAttr() -> getValueByName("replaceFrom"));
    of.writeParameter("replaceTo", getAttr() -> getValueByName("replaceTo"));
    ::rtl::OUString useLocale =   getAttr() -> getValueByName("ref");
    if (useLocale.getLength() > 0) {
        of.writeRefFunction("getAllFormats_", useLocale, "replaceTo");
        return;
    }
    sal_Int32 formatCount = 0;
    sal_Int16 i;
    NameSet aMsgId;
    ValueSet aFormatIndex;
    bool bCtypeIsRef = false;

    for ( i = 0; i< getNumberOfChildren() ; i++,formatCount++) {
        LocaleNode * currNode = getChildAt (i);
        ::rtl::OUString str;
        //      currNode -> print();
        const Attr *  currNodeAttr = currNode->getAttr();
        //printf ("getLen() = %d\n", currNode->getAttr()->getLength());
        str = currNodeAttr -> getValueByName("msgid");
        if (!aMsgId.insert( str).second)
            incErrorStr( "Duplicated msgid=\"%s\" in FormatElement.", str);
        of.writeParameter("FormatKey", str, formatCount);
        str = currNodeAttr -> getValueByName("default");
        of.writeDefaultParameter("FormatElement", str, formatCount);
        str = currNodeAttr -> getValueByName("type");
        of.writeParameter("FormatType", str, formatCount);
        str = currNodeAttr -> getValueByName("usage");
        of.writeParameter("FormatUsage", str, formatCount);
        str = currNodeAttr -> getValueByName("formatindex");
        sal_Int16 formatindex = (sal_Int16)str.toInt32();
        if (!aFormatIndex.insert( formatindex).second)
            incErrorInt( "Duplicated formatindex=\"%d\" in FormatElement.", formatindex);
        of.writeIntParameter("Formatindex", formatCount, formatindex);

        const LocaleNode * n = currNode -> findNode("FormatCode");
        if (n)
        {
            of.writeParameter("FormatCode", n->getValue(), formatCount);
            // Check separator usage for some FormatCode elements.
            const LocaleNode* pCtype = 0;
            switch (formatindex)
            {
                case cssi::NumberFormatIndex::NUMBER_1000DEC2 : // #,##0.00
                case cssi::NumberFormatIndex::TIME_MMSS00 :     // MM:SS.00
                case cssi::NumberFormatIndex::TIME_HH_MMSS00 :  // [HH]:MM:SS.00
                    {
                        const LocaleNode* pRoot = getRoot();
                        if (!pRoot)
                            incError( "No root for FormatCode.");
                        else
                        {
                            pCtype = pRoot->findNode( "LC_CTYPE");
                            if (!pCtype)
                                incError( "No LC_CTYPE found for FormatCode.");
                            else
                            {
                                OUString aRef( pCtype->getAttr()->getValueByName("ref"));
                                if (aRef.getLength() > 0)
                                {
                                    if (!bCtypeIsRef)
                                        fprintf( stderr,
                                                "Warning: Can't check separators used in FormatCode due to LC_CTYPE ref=\"%s\".\n"
                                                "If these two locales use identical format codes, you should consider to use the ref= mechanism also for the LC_FORMAT element, together with replaceFrom= and replaceTo= for the currency.\n",
                                                OUStringToOString( aRef, RTL_TEXTENCODING_UTF8).getStr());
                                    bCtypeIsRef = true;
                                    pCtype = 0;
                                }
                            }
                        }
                    }
                    break;
            }
            if (pCtype)
            {
                int nSavErr = nError;
                OUString aCode( n->getValue());
                if (formatindex == cssi::NumberFormatIndex::NUMBER_1000DEC2)
                {
                    sal_Int32 nDec = -1;
                    sal_Int32 nGrp = -1;
                    const LocaleNode* pSep = pCtype->findNode( "DecimalSeparator");
                    if (!pSep)
                        incError( "No DecimalSeparator found for FormatCode.");
                    else
                    {
                        nDec = aCode.indexOf( pSep->getValue());
                        if (nDec < 0)
                            incErrorInt( "DecimalSeparator not present in FormatCode formatindex=\"%d\".",
                                    formatindex);
                    }
                    pSep = pCtype->findNode( "ThousandSeparator");
                    if (!pSep)
                        incError( "No ThousandSeparator found for FormatCode.");
                    else
                    {
                        nGrp = aCode.indexOf( pSep->getValue());
                        if (nGrp < 0)
                            incErrorInt( "ThousandSeparator not present in FormatCode formatindex=\"%d\".",
                                    formatindex);
                    }
                    if (nDec <= nGrp)
                        incErrorInt( "Ordering of ThousandSeparator and DecimalSeparator not correct in formatindex=\"%d\".",
                                formatindex);
                }
                if (formatindex == cssi::NumberFormatIndex::TIME_MMSS00 ||
                        formatindex == cssi::NumberFormatIndex::TIME_HH_MMSS00)
                {
                    sal_Int32 nTime = -1;
                    sal_Int32 n100s = -1;
                    const LocaleNode* pSep = pCtype->findNode( "TimeSeparator");
                    if (!pSep)
                        incError( "No TimeSeparator found for FormatCode.");
                    else
                    {
                        nTime = aCode.indexOf( pSep->getValue());
                        if (nTime < 0)
                            incErrorInt( "TimeSeparator not present in FormatCode formatindex=\"%d\".",
                                    formatindex);
                    }
                    pSep = pCtype->findNode( "Time100SecSeparator");
                    if (!pSep)
                        incError( "No Time100SecSeparator found for FormatCode.");
                    else
                    {
                        n100s = aCode.indexOf( pSep->getValue());
                        if (n100s < 0)
                            incErrorInt( "Time100SecSeparator not present in FormatCode formatindex=\"%d\".",
                                    formatindex);
                        OUStringBuffer a100s( pSep->getValue());
                        a100s.appendAscii( "00");
                        n100s = aCode.indexOf( a100s.makeStringAndClear());
                        if (n100s < 0)
                            incErrorInt( "Time100SecSeparator+00 not present in FormatCode formatindex=\"%d\".",
                                    formatindex);
                    }
                    if (n100s <= nTime)
                        incErrorInt( "Ordering of Time100SecSeparator and TimeSeparator not correct in formatindex=\"%d\".",
                                formatindex);
                }
                if (nSavErr != nError)
                    fprintf( stderr,
                            "Warning: formatindex=\"%d\",\"%d\",\"%d\" are the only FormatCode elements checked, there may be others that have errors.\n",
                            int(cssi::NumberFormatIndex::NUMBER_1000DEC2),
                            int(cssi::NumberFormatIndex::TIME_MMSS00),
                            int(cssi::NumberFormatIndex::TIME_HH_MMSS00));

            }
        }
        else
            incError( "No FormatCode in FormatElement.");
        n = currNode -> findNode("DefaultName");
        if (n)
            of.writeParameter("FormatDefaultName", n->getValue(), formatCount);
        else
            of.writeParameter("FormatDefaultName", ::rtl::OUString(), formatCount);

    }

    // 0..49 MUST be present
    ValueSet::const_iterator aIter( aFormatIndex.begin());
    for (sal_Int16 nNext = cssi::NumberFormatIndex::NUMBER_START;
            nNext < cssi::NumberFormatIndex::INDEX_TABLE_ENTRIES; ++nNext)
    {
        sal_Int16 nHere = ::std::min( ((aIter != aFormatIndex.end() ? *aIter :
                cssi::NumberFormatIndex::INDEX_TABLE_ENTRIES)),
                cssi::NumberFormatIndex::INDEX_TABLE_ENTRIES);
        ++aIter;
        for ( ; nNext < nHere; ++nNext)
        {
            switch (nNext)
            {
                case cssi::NumberFormatIndex::FRACTION_1 :
                case cssi::NumberFormatIndex::FRACTION_2 :
                case cssi::NumberFormatIndex::BOOLEAN :
                case cssi::NumberFormatIndex::TEXT :
                    // generated internally
                    break;
                default:
                    incErrorInt( "FormatElement formatindex=\"%d\" not present.", nNext);
            }
        }
    }

    of.writeAsciiString("\nstatic const sal_Int16 ");
    of.writeAsciiString("FormatElements");
    of.writeAsciiString("Count = ");
    of.writeInt(formatCount);
    of.writeAsciiString(";\n");
    of.writeAsciiString("static const sal_Unicode* ");
    of.writeAsciiString("FormatElements");
    of.writeAsciiString("Array[] = {\n");
    for(i = 0; i < formatCount; i++) {

        of.writeAsciiString("\t");
        of.writeAsciiString("FormatCode");
        of.writeInt(i);
        of.writeAsciiString(",\n");

        of.writeAsciiString("\t");
        of.writeAsciiString("FormatDefaultName");
        of.writeInt(i);
        of.writeAsciiString(",\n");

        of.writeAsciiString("\t");
        of.writeAsciiString("FormatKey");
        of.writeInt(i);
        of.writeAsciiString(",\n");

        of.writeAsciiString("\t");
        of.writeAsciiString("FormatType");
        of.writeInt(i);
        of.writeAsciiString(",\n");

        of.writeAsciiString("\t");
        of.writeAsciiString("FormatUsage");
        of.writeInt(i);
        of.writeAsciiString(",\n");

        of.writeAsciiString("\t");
        of.writeAsciiString("Formatindex");
        of.writeInt(i);
        of.writeAsciiString(",\n");


        of.writeAsciiString("\tdefaultFormatElement");
        of.writeInt(i);
        of.writeAsciiString(",\n");
    }
    of.writeAsciiString("};\n\n");

    of.writeFunction("getAllFormats_", "FormatElementsCount", "FormatElementsArray", "replaceFrom", "replaceTo");
}

void LCCollationNode::generateCode (const OFileWriter &of) const
{
    ::rtl::OUString useLocale =   getAttr() -> getValueByName("ref");
    if (useLocale.getLength() > 0) {
        of.writeRefFunction("getCollatorImplementation_", useLocale);
        of.writeRefFunction("getCollationOptions_", useLocale);
        return;
    }
    sal_Int16 nbOfCollations = 0;
    sal_Int16 nbOfCollationOptions = 0;
    sal_Int16 i;

    for ( i = 0; i < getNumberOfChildren(); i++ ) {
        LocaleNode * currNode = getChildAt (i);
        if( currNode->getName().compareToAscii("Collator") == 0 )
        {
            ::rtl::OUString str;
            str = currNode->getAttr() -> getValueByName("unoid");
            of.writeParameter("CollatorID", str, i);
            str = currNode -> getAttr() -> getValueByName("default");
            of.writeDefaultParameter("Collator", str, i);
            of.writeAsciiString("\n");

            nbOfCollations++;
        }
        if( currNode->getName().compareToAscii("CollationOptions") == 0 )
        {
            LocaleNode* pCollationOptions = currNode;
            nbOfCollationOptions = pCollationOptions->getNumberOfChildren();
            for( sal_Int32 i=0; i<nbOfCollationOptions; i++ )
            {
                of.writeParameter("collationOption", pCollationOptions->getChildAt( i )->getValue(), i );
            }

            of.writeAsciiString("static const sal_Int16 nbOfCollationOptions = ");
            of.writeInt( nbOfCollationOptions );
            of.writeAsciiString(";\n\n");
        }
    }
    of.writeAsciiString("static const sal_Int16 nbOfCollations = ");
    of.writeInt(nbOfCollations);
    of.writeAsciiString(";\n\n");

    of.writeAsciiString("\nstatic const sal_Unicode* LCCollatorArray[] = {\n");
    for(i = 0; i < nbOfCollations; i++) {
        of.writeAsciiString("\tCollatorID");
        of.writeInt(i);
        of.writeAsciiString(",\n");

        of.writeAsciiString("\tdefaultCollator");
        of.writeInt(i);
        of.writeAsciiString(",\n");
    }
    of.writeAsciiString("};\n\n");

    of.writeAsciiString("static const sal_Unicode* collationOptions[] = {");
    for( i=0; i<nbOfCollationOptions; i++ )
    {
        of.writeAsciiString( "collationOption" );
        of.writeInt( i );
        of.writeAsciiString( ", " );
    }
    of.writeAsciiString("NULL };\n");
    of.writeFunction("getCollatorImplementation_", "nbOfCollations", "LCCollatorArray");
    of.writeFunction("getCollationOptions_", "nbOfCollationOptions", "collationOptions");
}

void LCSearchNode::generateCode (const OFileWriter &of) const
{
    ::rtl::OUString useLocale =   getAttr() -> getValueByName("ref");
    if (useLocale.getLength() > 0) {
        of.writeRefFunction("getSearchOptions_", useLocale);
        return;
    }

    if( getNumberOfChildren() != 1 )
    {
        ++nError;
        fprintf( stderr, "Error: LC_SEARCH: more than 1 child: %d\n", getNumberOfChildren());
    }
    sal_Int32 i;
    LocaleNode* pSearchOptions = getChildAt( 0 );
    sal_Int32   nSearchOptions = pSearchOptions->getNumberOfChildren();
    for( i=0; i<nSearchOptions; i++ )
    {
        of.writeParameter("searchOption", pSearchOptions->getChildAt( i )->getValue(), i );
    }

    of.writeAsciiString("static const sal_Int16 nbOfSearchOptions = ");
    of.writeInt( nSearchOptions );
    of.writeAsciiString(";\n\n");

    of.writeAsciiString("static const sal_Unicode* searchOptions[] = {");
    for( i=0; i<nSearchOptions; i++ )
    {
        of.writeAsciiString( "searchOption" );
        of.writeInt( i );
        of.writeAsciiString( ", " );
    }
    of.writeAsciiString("NULL };\n");
    of.writeFunction("getSearchOptions_", "nbOfSearchOptions", "searchOptions");
}

void LCIndexNode::generateCode (const OFileWriter &of) const
{
    ::rtl::OUString useLocale =   getAttr() -> getValueByName("ref");
    if (useLocale.getLength() > 0) {
        of.writeRefFunction("getIndexAlgorithm_", useLocale);
        of.writeRefFunction("getUnicodeScripts_", useLocale);
        of.writeRefFunction("getFollowPageWords_", useLocale);
        return;
    }
    sal_Int16 nbOfIndexs = 0;
    sal_Int16 nbOfUnicodeScripts = 0;
    sal_Int16 nbOfPageWords = 0;
    sal_Int16 i;
    for (i = 0; i< getNumberOfChildren();i++) {
        LocaleNode * currNode = getChildAt (i);
        if( currNode->getName().compareToAscii("IndexKey") == 0 )
        {
            ::rtl::OUString str;
            str = currNode->getAttr() -> getValueByName("unoid");
            of.writeParameter("IndexID", str, nbOfIndexs);
            str = currNode->getValue();
            of.writeParameter("IndexKey", str, nbOfIndexs);
            str = currNode -> getAttr() -> getValueByName("default");
            of.writeDefaultParameter("Index", str, nbOfIndexs);
            str = currNode -> getAttr() -> getValueByName("phonetic");
            of.writeDefaultParameter("Phonetic", str, nbOfIndexs);
            of.writeAsciiString("\n");

            nbOfIndexs++;
        }
        if( currNode->getName().compareToAscii("UnicodeScript") == 0 )
        {
            of.writeParameter("unicodeScript", currNode->getValue(), nbOfUnicodeScripts );
            nbOfUnicodeScripts++;

        }
        if( currNode->getName().compareToAscii("FollowPageWord") == 0 )
        {
            of.writeParameter("followPageWord", currNode->getValue(), nbOfPageWords);
            nbOfPageWords++;
        }
    }
    of.writeAsciiString("static const sal_Int16 nbOfIndexs = ");
    of.writeInt(nbOfIndexs);
    of.writeAsciiString(";\n\n");

    of.writeAsciiString("\nstatic const sal_Unicode* IndexArray[] = {\n");
    for(i = 0; i < nbOfIndexs; i++) {
        of.writeAsciiString("\tIndexID");
        of.writeInt(i);
        of.writeAsciiString(",\n");

        of.writeAsciiString("\tIndexKey");
        of.writeInt(i);
        of.writeAsciiString(",\n");

        of.writeAsciiString("\tdefaultIndex");
        of.writeInt(i);
        of.writeAsciiString(",\n");

        of.writeAsciiString("\tdefaultPhonetic");
        of.writeInt(i);
        of.writeAsciiString(",\n");
    }
    of.writeAsciiString("};\n\n");

    of.writeAsciiString("static const sal_Int16 nbOfUnicodeScripts = ");
    of.writeInt( nbOfUnicodeScripts );
    of.writeAsciiString(";\n\n");

    of.writeAsciiString("static const sal_Unicode* UnicodeScriptArray[] = {");
    for( i=0; i<nbOfUnicodeScripts; i++ )
    {
        of.writeAsciiString( "unicodeScript" );
        of.writeInt( i );
        of.writeAsciiString( ", " );
    }
    of.writeAsciiString("NULL };\n");

    of.writeAsciiString("static const sal_Int16 nbOfPageWords = ");
    of.writeInt(nbOfPageWords);
    of.writeAsciiString(";\n\n");

    of.writeAsciiString("\nstatic const sal_Unicode* FollowPageWordArray[] = {\n");
    for(i = 0; i < nbOfPageWords; i++) {
        of.writeAsciiString("\tfollowPageWord");
        of.writeInt(i);
        of.writeAsciiString(",\n");
    }
    of.writeAsciiString("};\n\n");

    of.writeFunction("getIndexAlgorithm_", "nbOfIndexs", "IndexArray");
    of.writeFunction("getUnicodeScripts_", "nbOfUnicodeScripts", "UnicodeScriptArray");
    of.writeFunction("getFollowPageWords_", "nbOfPageWords", "FollowPageWordArray");
}

void LCCalendarNode::generateCode (const OFileWriter &of) const
{
    ::rtl::OUString useLocale =   getAttr() -> getValueByName("ref");
    if (useLocale.getLength() > 0) {
        of.writeRefFunction("getAllCalendars_", useLocale);
        return;
    }
    sal_Int16 nbOfCalendars = getNumberOfChildren();
    ::rtl::OUString str;
    sal_Int16 * nbOfDays = new sal_Int16[nbOfCalendars];
    sal_Int16 * nbOfMonths = new sal_Int16[nbOfCalendars];
    sal_Int16 * nbOfEras = new sal_Int16[nbOfCalendars];
    sal_Int16 j;
    sal_Int16 i;
    bool bHasGregorian = false;


    for ( i = 0; i < nbOfCalendars; i++) {
        LocaleNode * calNode = getChildAt (i);
        OUString calendarID = calNode -> getAttr() -> getValueByName("unoid");
        of.writeParameter( "calendarID", calendarID, i);
        bool bGregorian = calendarID.equalsAscii( "gregorian");
        if (!bHasGregorian)
            bHasGregorian = bGregorian;
        str = calNode -> getAttr() -> getValueByName("default");
        of.writeDefaultParameter("Calendar", str, i);

        // Generate Days of Week
        const sal_Char *elementTag;
        LocaleNode * daysNode = NULL;
        ::rtl::OUString ref_name = calNode->getChildAt(0)->getAttr()->getValueByName("ref");
        if (ref_name.getLength() > 0 && i > 0) {
            for (j = 0; j < i; j++) {
                str = getChildAt(j)->getAttr()->getValueByName("unoid");
                if (str.equals(ref_name))
                    daysNode = getChildAt(j)->getChildAt(0);
            }
        }
        if (ref_name.getLength() > 0 && daysNode == NULL) {
            of.writeParameter("dayRef", OUString::createFromAscii("ref"), i);
            of.writeParameter("dayRefName", ref_name, i);
            nbOfDays[i] = 0;
        } else {
            if (daysNode == NULL)
                daysNode = calNode -> getChildAt(0);
            nbOfDays[i] = daysNode->getNumberOfChildren();
            if (bGregorian && nbOfDays[i] != 7)
                incErrorInt( "A Gregorian calendar must have 7 days per week, this one has %d", nbOfDays[i]);
            elementTag = "day";
            for (j = 0; j < nbOfDays[i]; j++) {
                LocaleNode *currNode = daysNode -> getChildAt(j);
                OUString dayID( currNode->getChildAt(0)->getValue());
                of.writeParameter("dayID", dayID, i, j);
                if (j == 0 && bGregorian && !dayID.equalsAscii( "sun"))
                    incError( "First day of a week of a Gregorian calendar must be <DayID>sun</DayID>");
                of.writeParameter(elementTag, "DefaultAbbrvName",currNode->getChildAt(1)->getValue() ,i, j);
                of.writeParameter(elementTag, "DefaultFullName",currNode->getChildAt(2)->getValue() , i, j);
            }
        }

        // Generate Months of Year
        LocaleNode * monthsNode = NULL;
        ref_name = calNode->getChildAt(1)->getAttr()->getValueByName("ref");
        if (ref_name.getLength() > 0 && i > 0) {
            for (j = 0; j < i; j++) {
                str = getChildAt(j)->getAttr()->getValueByName("unoid");
                if (str.equals(ref_name))
                    monthsNode = getChildAt(j)->getChildAt(1);
            }
        }
        if (ref_name.getLength() > 0 && monthsNode == NULL) {
            of.writeParameter("monthRef", OUString::createFromAscii("ref"), i);
            of.writeParameter("monthRefName", ref_name, i);
            nbOfMonths[i] = 0;
        } else {
            if (monthsNode == NULL)
                monthsNode = calNode -> getChildAt(1);
            nbOfMonths[i] = monthsNode->getNumberOfChildren();
            if (bGregorian && nbOfMonths[i] != 12)
                incErrorInt( "A Gregorian calendar must have 12 months, this one has %d", nbOfMonths[i]);
            elementTag = "month";
            for (j = 0; j < nbOfMonths[i]; j++) {
                LocaleNode *currNode = monthsNode -> getChildAt(j);
                OUString monthID( currNode->getChildAt(0)->getValue());
                of.writeParameter("monthID", monthID, i, j);
                if (j == 0 && bGregorian && !monthID.equalsAscii( "jan"))
                    incError( "First month of a year of a Gregorian calendar must be <MonthID>jan</MonthID>");
                of.writeParameter(elementTag, "DefaultAbbrvName",currNode->getChildAt(1)->getValue() ,i, j);
                of.writeParameter(elementTag, "DefaultFullName",currNode->getChildAt(2)->getValue() , i, j);
            }
        }

        // Generate Era name
        LocaleNode * erasNode = NULL;
        ref_name =   calNode -> getChildAt(2) ->getAttr()->getValueByName("ref");
        if (ref_name.getLength() > 0 && i > 0) {
            for (j = 0; j < i; j++) {
                str = getChildAt(j)->getAttr()->getValueByName("unoid");
                if (str.equals(ref_name))
                    erasNode = getChildAt(j)->getChildAt(2);
            }
        }
        if (ref_name.getLength() > 0 && erasNode == NULL) {
            of.writeParameter("eraRef", OUString::createFromAscii("ref"), i);
            of.writeParameter("eraRefName", ref_name, i);
            nbOfEras[i] = 0;
        } else {
            if (erasNode == NULL)
                erasNode = calNode -> getChildAt(2);
            nbOfEras[i] = erasNode->getNumberOfChildren();
            if (bGregorian && nbOfEras[i] != 2)
                incErrorInt( "A Gregorian calendar must have 2 eras, this one has %d", nbOfEras[i]);
            elementTag = "era";
            for (j = 0; j < nbOfEras[i]; j++) {
                LocaleNode *currNode = erasNode -> getChildAt(j);
                OUString eraID( currNode->getChildAt(0)->getValue());
                of.writeParameter("eraID", eraID, i, j);
                if (j == 0 && bGregorian && !eraID.equalsAscii( "bc"))
                    incError( "First era of a Gregorian calendar must be <EraID>bc</EraID>");
                if (j == 1 && bGregorian && !eraID.equalsAscii( "ad"))
                    incError( "Second era of a Gregorian calendar must be <EraID>ad</EraID>");
                of.writeAsciiString("\n");
                of.writeParameter(elementTag, "DefaultAbbrvName",currNode->getChildAt(1)->getValue() ,i, j);
                of.writeParameter(elementTag, "DefaultFullName",currNode->getChildAt(2)->getValue() , i, j);
            }
        }
        of.writeParameter("startDayOfWeek", calNode ->getChildAt(3)-> getChildAt(0)->getValue(), i);
        str = calNode ->getChildAt(4)-> getValue();
        sal_Int16 nDays = str.toInt32();
        if (nDays < 1 || (0 < nbOfDays[i] && nbOfDays[i] < nDays))
            incErrorInt( "Bad value of MinimalDaysInFirstWeek: %d, must be 1 <= value <= days_in_week",  nDays);
        of.writeIntParameter("minimalDaysInFirstWeek", i, nDays);
    }
    if (!bHasGregorian)
        fprintf( stderr, "Warning: %s\n", "No Gregorian calendar defined, are you sure?");

    of.writeAsciiString("static const sal_Int16 calendarsCount = ");
    of.writeInt(nbOfCalendars);
    of.writeAsciiString(";\n\n");

    of.writeAsciiString("static const sal_Unicode nbOfDays[] = {");
    for(i = 0; i < nbOfCalendars - 1; i++) {
        of.writeInt(nbOfDays[i]);
        of.writeAsciiString(", ");
    };
    of.writeInt(nbOfDays[i]);
    of.writeAsciiString("};\n");

    of.writeAsciiString("static const sal_Unicode nbOfMonths[] = {");
    for(i = 0; i < nbOfCalendars - 1; i++) {
        of.writeInt(nbOfMonths[i]);
        of.writeAsciiString(", ");
    };
    of.writeInt(nbOfMonths[i]);
    of.writeAsciiString("};\n");

    of.writeAsciiString("static const sal_Unicode nbOfEras[] = {");
    for(i = 0; i < nbOfCalendars - 1; i++) {
        of.writeInt(nbOfEras[i]);
        of.writeAsciiString(", ");
    };
    of.writeInt(nbOfEras[i]);
    of.writeAsciiString("};\n");


    of.writeAsciiString("static const sal_Unicode* calendars[] = {\n");
    of.writeAsciiString("\tnbOfDays,\n");
    of.writeAsciiString("\tnbOfMonths,\n");
    of.writeAsciiString("\tnbOfEras,\n");
    for(i = 0; i < nbOfCalendars; i++) {
        of.writeAsciiString("\tcalendarID");
        of.writeInt(i);
        of.writeAsciiString(",\n");
        of.writeAsciiString("\tdefaultCalendar");
        of.writeInt(i);
        of.writeAsciiString(",\n");
        if (nbOfDays[i] == 0) {
            of.writeAsciiString("\tdayRef");
            of.writeInt(i); of.writeAsciiString(",\n");
            of.writeAsciiString("\tdayRefName");
            of.writeInt(i); of.writeAsciiString(",\n");
        } else {
            for(sal_Int16 j = 0; j < nbOfDays[i]; j++) {
                of.writeAsciiString("\tdayID");
                of.writeInt(i); of.writeInt(j); of.writeAsciiString(",\n");
                of.writeAsciiString("\tdayDefaultAbbrvName");
                of.writeInt(i); of.writeInt(j); of.writeAsciiString(",\n");
                of.writeAsciiString("\tdayDefaultFullName");of.writeInt(i); of.writeInt(j); of.writeAsciiString(",\n");
            }
        }
        if (nbOfMonths[i] == 0) {
            of.writeAsciiString("\tmonthRef");
            of.writeInt(i); of.writeAsciiString(",\n");
            of.writeAsciiString("\tmonthRefName");
            of.writeInt(i); of.writeAsciiString(",\n");
        } else {
            for(j = 0; j < nbOfMonths[i]; j++) {
                of.writeAsciiString("\tmonthID");of.writeInt(i);of.writeInt(j);of.writeAsciiString(",\n");
                of.writeAsciiString("\tmonthDefaultAbbrvName");of.writeInt(i);of.writeInt(j);of.writeAsciiString(",\n");
                of.writeAsciiString("\tmonthDefaultFullName");of.writeInt(i);of.writeInt(j);of.writeAsciiString(",\n");
            }
        }
        if (nbOfEras[i] == 0) {
            of.writeAsciiString("\teraRef");
            of.writeInt(i); of.writeAsciiString(",\n");
            of.writeAsciiString("\teraRefName");
            of.writeInt(i); of.writeAsciiString(",\n");
        } else {
            for(j = 0; j < nbOfEras[i]; j++) {
                of.writeAsciiString("\teraID"); of.writeInt(i); of.writeInt(j); of.writeAsciiString(",\n");
                of.writeAsciiString("\teraDefaultAbbrvName");of.writeInt(i);of.writeInt(j);of.writeAsciiString(",\n");
                of.writeAsciiString("\teraDefaultFullName");of.writeInt(i);of.writeInt(j);of.writeAsciiString(",\n");
            }
        }
        of.writeAsciiString("\tstartDayOfWeek");of.writeInt(i); of.writeAsciiString(",\n");
        of.writeAsciiString("\tminimalDaysInFirstWeek");of.writeInt(i); of.writeAsciiString(",\n");
    }

    of.writeAsciiString("};\n\n");
    of.writeFunction("getAllCalendars_", "calendarsCount", "calendars");

    delete []nbOfDays;
    delete []nbOfMonths;
    delete []nbOfEras;
}

void LCCurrencyNode :: generateCode (const OFileWriter &of) const
{
    ::rtl::OUString useLocale =   getAttr() -> getValueByName("ref");
    if (useLocale.getLength() > 0) {
        of.writeRefFunction("getAllCurrencies_", useLocale);
        return;
    }
    sal_Int16 nbOfCurrencies = 0;
    ::rtl::OUString str;
    sal_Int16 i;

    for ( i = 0; i < getNumberOfChildren(); i++,nbOfCurrencies++) {
        LocaleNode * calNode = getChildAt (i);
        str = calNode->getAttr() -> getValueByName("default");
        of.writeDefaultParameter("Currency", str, nbOfCurrencies);
        str = calNode->getAttr() -> getValueByName("usedInCompatibleFormatCodes");
        of.writeDefaultParameter("CurrencyUsedInCompatibleFormatCodes", str, nbOfCurrencies);
        str = calNode -> findNode ("CurrencyID") -> getValue();
        of.writeParameter("currencyID", str, nbOfCurrencies);
        str = calNode -> findNode ("CurrencySymbol") -> getValue();
        of.writeParameter("currencySymbol", str, nbOfCurrencies);
        str = calNode -> findNode ("BankSymbol") -> getValue();
        of.writeParameter("bankSymbol", str, nbOfCurrencies);
        str = calNode -> findNode ("CurrencyName") -> getValue();
        of.writeParameter("currencyName", str, nbOfCurrencies);
        str = calNode -> findNode ("DecimalPlaces") -> getValue();
        sal_Int16 nDecimalPlaces = (sal_Int16)str.toInt32();
        of.writeIntParameter("currencyDecimalPlaces", nbOfCurrencies, nDecimalPlaces);
        of.writeAsciiString("\n");
    };

    of.writeAsciiString("static const sal_Int16 currencyCount = ");
    of.writeInt(nbOfCurrencies);
    of.writeAsciiString(";\n\n");
    of.writeAsciiString("static const sal_Unicode* currencies[] = {\n");
    for(i = 0; i < nbOfCurrencies; i++) {
        of.writeAsciiString("\tcurrencyID");
        of.writeInt(i);
        of.writeAsciiString(",\n");
        of.writeAsciiString("\tcurrencySymbol");
        of.writeInt(i);
        of.writeAsciiString(",\n");
        of.writeAsciiString("\tbankSymbol");
        of.writeInt(i);
        of.writeAsciiString(",\n");
        of.writeAsciiString("\tcurrencyName");
        of.writeInt(i);
        of.writeAsciiString(",\n");
        of.writeAsciiString("\tdefaultCurrency");
        of.writeInt(i);
        of.writeAsciiString(",\n");
        of.writeAsciiString("\tdefaultCurrencyUsedInCompatibleFormatCodes");
        of.writeInt(i);
        of.writeAsciiString(",\n");
        of.writeAsciiString("\tcurrencyDecimalPlaces");
        of.writeInt(i);
        of.writeAsciiString(",\n");
    }
    of.writeAsciiString("};\n\n");
    of.writeFunction("getAllCurrencies_", "currencyCount", "currencies");
}

void LCTransliterationNode::generateCode (const OFileWriter &of) const
{
    ::rtl::OUString useLocale =   getAttr() -> getValueByName("ref");
    if (useLocale.getLength() > 0) {
        of.writeRefFunction("getTransliterations_", useLocale);
        return;
    }
    sal_Int16 nbOfModules = 0;
    ::rtl::OUString str;
    sal_Int16 i;

    for ( i = 0; i < getNumberOfChildren(); i++,nbOfModules++) {
        LocaleNode * calNode = getChildAt (i);
        str = calNode->getAttr() -> getValueByIndex(0);
        of.writeParameter("Transliteration", str, nbOfModules);
    }
    of.writeAsciiString("static const sal_Int16 nbOfTransliterations = ");
    of.writeInt(nbOfModules);
    of.writeAsciiString(";\n\n");

    of.writeAsciiString("\nstatic const sal_Unicode* LCTransliterationsArray[] = {\n");
    for( i = 0; i < nbOfModules; i++) {
        of.writeAsciiString("\tTransliteration");
        of.writeInt(i);
        of.writeAsciiString(",\n");
    }
    of.writeAsciiString("};\n\n");
    of.writeFunction("getTransliterations_", "nbOfTransliterations", "LCTransliterationsArray");
}

struct NameValuePair {
    const sal_Char *name;
    const sal_Char *value;
};
static NameValuePair ReserveWord[] = {
    { "trueWord", "true" },
    { "falseWord", "false" },
    { "quarter1Word", "1st quarter" },
    { "quarter2Word", "2nd quarter" },
    { "quarter3Word", "3rd quarter" },
    { "quarter4Word", "4th quarter" },
    { "aboveWord", "above" },
    { "belowWord", "below" },
    { "quarter1Abbreviation", "Q1" },
    { "quarter2Abbreviation", "Q2" },
    { "quarter3Abbreviation", "Q3" },
    { "quarter4Abbreviation", "Q4" }
};

void LCMiscNode::generateCode (const OFileWriter &of) const
{
    ::rtl::OUString useLocale =   getAttr() -> getValueByName("ref");
    if (useLocale.getLength() > 0) {
    of.writeRefFunction("getForbiddenCharacters_", useLocale);
    of.writeRefFunction("getReservedWords_", useLocale);
    return;
    }
    const LocaleNode * reserveNode = findNode("ReservedWords");
    if (!reserveNode)
        incError( "No ReservedWords element."); // should not happen if validated..
    const LocaleNode * forbidNode = findNode("ForbiddenCharacters");

    sal_Int16 nbOfWords = 0;
    ::rtl::OUString str;
    sal_Int16 i;

    for ( i = 0; i < sal_Int16(sizeof(ReserveWord)/sizeof(ReserveWord[0])); i++,nbOfWords++) {
        const LocaleNode * curNode = (reserveNode ? reserveNode->findNode(
                    ReserveWord[i].name) : 0);
        if (!curNode)
            fprintf( stderr,
                    "Warning: No %s in ReservedWords, using en_US default: \"%s\".\n",
                    ReserveWord[i].name, ReserveWord[i].value);
        str = curNode ? curNode -> getValue() : OUString::createFromAscii(ReserveWord[i].value);
        if (!str.getLength())
        {
            ++nError;
            fprintf( stderr, "Error: No content for ReservedWords %s.\n", ReserveWord[i].name);
        }
        of.writeParameter("ReservedWord", str, nbOfWords);
    }
    of.writeAsciiString("static const sal_Int16 nbOfReservedWords = ");
    of.writeInt(nbOfWords);
    of.writeAsciiString(";\n\n");
    of.writeAsciiString("\nstatic const sal_Unicode* LCReservedWordsArray[] = {\n");
    for( i = 0; i < nbOfWords; i++) {
        of.writeAsciiString("\tReservedWord");
        of.writeInt(i);
        of.writeAsciiString(",\n");
    }
    of.writeAsciiString("};\n\n");

    if (forbidNode)    {
         of.writeParameter( "forbiddenBegin", forbidNode -> getChildAt(0)->getValue());
         of.writeParameter( "forbiddenEnd", forbidNode -> getChildAt(1)->getValue());
    } else {
         of.writeParameter( "forbiddenBegin", ::rtl::OUString());
         of.writeParameter( "forbiddenEnd", ::rtl::OUString());
    }
    of.writeAsciiString("\nstatic const sal_Unicode* LCForbiddenCharactersArray[] = {\n");
    of.writeAsciiString("\tforbiddenBegin,\n");
    of.writeAsciiString("\tforbiddenEnd\n");
    of.writeAsciiString("};\n\n");
    of.writeFunction("getForbiddenCharacters_", "2", "LCForbiddenCharactersArray");
    of.writeFunction("getReservedWords_", "nbOfReservedWords", "LCReservedWordsArray");
}

void LCNumberingLevelNode::generateCode (const OFileWriter &of) const
{
     of.writeAsciiString("// ---> ContinuousNumbering\n");
    ::rtl::OUString useLocale =   getAttr() -> getValueByName("ref");
    if (useLocale.getLength() > 0) {
    of.writeRefFunction2("getContinuousNumberingLevels_", useLocale);
    return;
    }

     // hard code number of attributes per style.
     const int   nAttributes = 5;
     const char* attr[ nAttributes ] = { "Prefix", "NumType", "Suffix", "Transliteration", "NatNum" };

     // record each attribute of each style in a static C++ variable.
     // determine number of styles on the fly.
     sal_Int32 nStyles = getNumberOfChildren();
     sal_Int32 i;

     for( i = 0; i < nStyles; i++ )
     {
          const Attr* q = getChildAt( i )->getAttr();
          for( sal_Int32 j=0; j<nAttributes; j++ )
          {
               const char* name = attr[j];
               OUString   value = q->getValueByName( name );
               of.writeParameter("continuous", name, value, i );
          }
     }

     // record number of styles and attributes.
     of.writeAsciiString("static const sal_Int16 continuousNbOfStyles = ");
     of.writeInt( nStyles );
     of.writeAsciiString(";\n\n");
     of.writeAsciiString("static const sal_Int16 continuousNbOfAttributesPerStyle = ");
     of.writeInt( nAttributes );
     of.writeAsciiString(";\n\n");

     // generate code. (intermediate arrays)
     for( i=0; i<nStyles; i++ )
     {
          of.writeAsciiString("\nstatic const sal_Unicode* continuousStyle" );
          of.writeInt( i );
          of.writeAsciiString("[] = {\n");
          for( sal_Int32 j=0; j<nAttributes; j++)
          {
               of.writeAsciiString("\t");
               of.writeAsciiString( "continuous" );
               of.writeAsciiString( attr[j] );
               of.writeInt(i);
               of.writeAsciiString(",\n");
          }
          of.writeAsciiString("\t0\n};\n\n");
     }

     // generate code. (top-level array)
     of.writeAsciiString("\n");
     of.writeAsciiString("static const sal_Unicode** LCContinuousNumberingLevelsArray[] = {\n" );
     for( i=0; i<nStyles; i++ )
     {
          of.writeAsciiString( "\t" );
          of.writeAsciiString( "continuousStyle" );
          of.writeInt( i );
          of.writeAsciiString( ",\n");
     }
     of.writeAsciiString("\t0\n};\n\n");
     of.writeFunction2("getContinuousNumberingLevels_", "continuousNbOfStyles",
            "continuousNbOfAttributesPerStyle", "LCContinuousNumberingLevelsArray");
}


void LCOutlineNumberingLevelNode::generateCode (const OFileWriter &of) const
{
     of.writeAsciiString("// ---> OutlineNumbering\n");
    ::rtl::OUString useLocale =   getAttr() -> getValueByName("ref");
    if (useLocale.getLength() > 0) {
    of.writeRefFunction3("getOutlineNumberingLevels_", useLocale);
    return;
    }

     // hardcode number of attributes per level
     const int   nAttributes = 11;
     const char* attr[ nAttributes ] =
     {
          "Prefix",
          "NumType",
          "Suffix",
          "BulletChar",
          "BulletFontName",
          "ParentNumbering",
          "LeftMargin",
          "SymbolTextDistance",
          "FirstLineOffset",
          "Transliteration",
          "NatNum",
     };

     // record each attribute of each level of each style in a static C++ variable.
     // determine number of styles and number of levels per style on the fly.
     sal_Int32 nStyles = getNumberOfChildren();
     vector<sal_Int32> nLevels; // may be different for each style?
     for( sal_Int32 i = 0; i < nStyles; i++ )
     {
          LocaleNode* p = getChildAt( i );
          nLevels.push_back( p->getNumberOfChildren() );
          for( sal_Int32 j=0; j<nLevels.back(); j++ )
          {
               const Attr* q = p->getChildAt( j )->getAttr();
               for( sal_Int32 k=0; k<nAttributes; k++ )
               {
                    const char* name = attr[k];
                    OUString   value = q->getValueByName( name );
                    of.writeParameter("outline", name, value, i, j );
               }
          }
     }

     // verify that each style has the same number of levels.
     for( size_t i=0; i<nLevels.size(); i++ )
     {
          if( nLevels[0] != nLevels[i] )
          {
               incError( "Numbering levels don't match.");
          }
     }

     // record number of attributes, levels, and styles.
     of.writeAsciiString("static const sal_Int16 outlineNbOfStyles = ");
     of.writeInt( nStyles );
     of.writeAsciiString(";\n\n");
     of.writeAsciiString("static const sal_Int16 outlineNbOfLevelsPerStyle = ");
     of.writeInt( nLevels.back() );
     of.writeAsciiString(";\n\n");
     of.writeAsciiString("static const sal_Int16 outlineNbOfAttributesPerLevel = ");
     of.writeInt( nAttributes );
     of.writeAsciiString(";\n\n");

     // too complicated for now...
//     of.writeAsciiString("static const sal_Int16 nbOfOutlineNumberingLevels[] = { ");
//     for( sal_Int32 j=0; j<nStyles; j++ )
//     {
//          of.writeInt( nLevels[j] );
//          of.writeAsciiString(", ");
//     }
//     of.writeAsciiString("};\n\n");


     for( sal_Int32 i=0; i<nStyles; i++ )
     {
          for( sal_Int32 j=0; j<nLevels.back(); j++ )
          {
               of.writeAsciiString("static const sal_Unicode* outline");
               of.writeAsciiString("Style");
               of.writeInt( i );
               of.writeAsciiString("Level");
               of.writeInt( j );
               of.writeAsciiString("[] = { ");

               for( sal_Int32 k=0; k<nAttributes; k++ )
               {
                    of.writeAsciiString( "outline" );
                    of.writeAsciiString( attr[k] );
                    of.writeInt( i );
                    of.writeInt( j );
                    of.writeAsciiString(", ");
               }
               of.writeAsciiString("NULL };\n");
          }
     }

     of.writeAsciiString("\n");


     for( sal_Int32 i=0; i<nStyles; i++ )
     {
          of.writeAsciiString("static const sal_Unicode** outline");
          of.writeAsciiString( "Style" );
          of.writeInt( i );
          of.writeAsciiString("[] = { ");

          for( sal_Int32 j=0; j<nLevels.back(); j++ )
          {
               of.writeAsciiString("outlineStyle");
               of.writeInt( i );
               of.writeAsciiString("Level");
               of.writeInt( j );
               of.writeAsciiString(", ");
          }
          of.writeAsciiString("NULL };\n");
     }
     of.writeAsciiString("\n");

     of.writeAsciiString("static const sal_Unicode*** LCOutlineNumberingLevelsArray[] = {\n" );
     for( sal_Int32 i=0; i<nStyles; i++ )
     {
          of.writeAsciiString( "\t" );
          of.writeAsciiString( "outlineStyle" );
          of.writeInt( i );
          of.writeAsciiString(",\n");
     }
     of.writeAsciiString("\tNULL\n};\n\n");
     of.writeFunction3("getOutlineNumberingLevels_", "outlineNbOfStyles", "outlineNbOfLevelsPerStyle",
            "outlineNbOfAttributesPerLevel", "LCOutlineNumberingLevelsArray");
}

Attr::Attr (const Reference< XAttributeList > & attr) {
    sal_Int32 len = attr->getLength();
    name.realloc (len);
    value.realloc (len);
    for (sal_Int32 i =0; i< len;i++) {
        name[i] = attr->getNameByIndex(i);
        value[i] = attr -> getValueByIndex(i);
    }
}

const OUString& Attr::getValueByName (const sal_Char *str) const {
    static OUString empty;
    sal_Int32 len = name.getLength();
    for (sal_Int32 i = 0;i<len;i++)
        if (name[i].equalsAscii(str))
            return value[i];
    return empty;
}

sal_Int32 Attr::getLength() const{
    return name.getLength();
}

const OUString& Attr::getTypeByIndex (sal_Int32 idx) const {
    return name[idx];
}

const OUString& Attr::getValueByIndex (sal_Int32 idx) const
{
    return value[idx];
}
