/*************************************************************************
 *
 *  $RCSfile: txtparae.cxx,v $
 *
 *  $Revision: 1.68 $
 *
 *  last change: $Author: mib $ $Date: 2001-03-21 10:01:02 $
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

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _SVSTDARR_LONGS_DECL
#define _SVSTDARR_LONGS
#include <svtools/svstdarr.hxx>
#endif
#ifndef _SVARRAY_HXX
#include <svtools/svarray.hxx>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#include <vector>


#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XENUMERATIONACCESS_HPP_
#include <com/sun/star/container/XEnumerationAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XENUMERATION_HPP_
#include <com/sun/star/container/XEnumeration.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XINDEXREPLACE_HPP_
#include <com/sun/star/container/XIndexReplace.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSTATE_HPP_
#include <com/sun/star/beans/XPropertyState.hpp>
#endif

#ifndef _COM_SUN_STAR_TEXT_XTEXTDOCUMENT_HPP_
#include <com/sun/star/text/XTextDocument.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXT_HPP_
#include <com/sun/star/text/XText.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTCONTENT_HPP_
#include <com/sun/star/text/XTextContent.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTRANGE_HPP_
#include <com/sun/star/text/XTextRange.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTFIELD_HPP_
#include <com/sun/star/text/XTextField.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XFOOTNOTE_HPP_
#include <com/sun/star/text/XFootnote.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMED_HPP_
#include <com/sun/star/container/XNamed.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCONTENTENUMERATIONACCESS_HPP_
#include <com/sun/star/container/XContentEnumerationAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTFRAME_HPP_
#include <com/sun/star/text/XTextFrame.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMED_HPP_
#include <com/sun/star/container/XNamed.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_SIZETYPE_HPP_
#include <com/sun/star/text/SizeType.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_HORIORIENTATION_HPP_
#include <com/sun/star/text/HoriOrientation.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_VERTORIENTATION_HPP_
#include <com/sun/star/text/VertOrientation.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_TEXTCONTENTANCHORTYPE_HPP
#include <com/sun/star/text/TextContentAnchorType.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTFRAMESSUPPLIER_HPP_
#include <com/sun/star/text/XTextFramesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTGRAPHICOBJECTSSUPPLIER_HPP_
#include <com/sun/star/text/XTextGraphicObjectsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTEMBEDDEDOBJECTSSUPPLIER_HPP_
#include <com/sun/star/text/XTextEmbeddedObjectsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGESUPPLIER_HPP_
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XEMBEDDEDOBJECTSUPPLIER_HPP_
#include <com/sun/star/document/XEmbeddedObjectSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XEVENTSSUPPLIER_HPP_
#include <com/sun/star/document/XEventsSupplier.hpp>
#endif

#ifndef _COM_SUN_STAR_TEXT_XTEXTSECTION_HPP_
#include <com/sun/star/text/XTextSection.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_SECTIONFILELINK_HPP_
#include <com/sun/star/text/SectionFileLink.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPE_HPP_
#include <com/sun/star/drawing/XShape.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_DATETIME_HPP_
#include <com/sun/star/util/DateTime.hpp>
#endif

#ifndef _XMLOFF_XMLKYWD_HXX
#include "xmlkywd.hxx"
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_XMLASTPLP_HXX
#include "xmlaustp.hxx"
#endif
#ifndef _XMLOFF_FAMILIES_HXX_
#include "families.hxx"
#endif
#ifndef _XMLOFF_TXTEXPPR_HXX
#include "txtexppr.hxx"
#endif
#ifndef _XMLOFF_XMLNUMFE_HXX
#include "xmlnumfe.hxx"
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif
#ifndef _XMLOFF_XMLANCHORTYPEPROPHDL_HXX
#include "XMLAnchorTypePropHdl.hxx"
#endif
#ifndef _XEXPTRANSFORM_HXX
#include "xexptran.hxx"
#endif
#ifndef _XMLOFF_PROGRESSBARHELPER_HXX
#include "ProgressBarHelper.hxx"
#endif

#ifndef _XMLOFF_NMSPMAP_HXX
#include "nmspmap.hxx"
#endif
#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif
#ifndef _XMLOFF_TXTFLDE_HXX
#include "txtflde.hxx"
#endif
#ifndef _XMLOFF_TXTPRMAP_HXX
#include "txtprmap.hxx"
#endif
#ifndef _XMLOFF_XMLTEXTNUMRULEINFO_HXX
#include "XMLTextNumRuleInfo.hxx"
#endif
#ifndef _XMLOFF_XMLTEXTLISTAUTOSTYLEPOOL_HXX
#include "XMLTextListAutoStylePool.hxx"
#endif
#ifndef _XMLOFF_TXTPARAE_HXX
#include "txtparae.hxx"
#endif
#ifndef _XMLOFF_XMLSECTIONEXPORT_HXX_
#include "XMLSectionExport.hxx"
#endif
#ifndef _XMLOFF_XMLINDEXMARKEXPORT_HXX_
#include "XMLIndexMarkExport.hxx"
#endif
#ifndef _XMLOFF_XMLEVENTEXPORT_HXX
#include "XMLEventExport.hxx"
#endif
#ifndef _XMLOFF_XMLREDLINEEXPORT_HXX
#include "XMLRedlineExport.hxx"
#endif

using namespace ::rtl;
using namespace ::std;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::drawing;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::frame;

typedef OUString *OUStringPtr;
SV_DECL_PTRARR_DEL( OUStrings_Impl, OUStringPtr, 20, 10 )
SV_IMPL_PTRARR( OUStrings_Impl, OUStringPtr )

SV_DECL_PTRARR_SORT_DEL( OUStringsSort_Impl, OUStringPtr, 20, 10 )
SV_IMPL_OP_PTRARR_SORT( OUStringsSort_Impl, OUStringPtr )

void XMLTextParagraphExport::Add( sal_uInt16 nFamily,
                                  const Reference < XPropertySet > & rPropSet,
                                  const XMLPropertyState** ppAddStates)
{
    sal_Bool bCache = sal_False;
    UniReference < SvXMLExportPropertyMapper > xPropMapper;
    switch( nFamily )
    {
    case XML_STYLE_FAMILY_TEXT_PARAGRAPH:
        xPropMapper = GetParaPropMapper();
//      bCache = sal_True;
        break;
    case XML_STYLE_FAMILY_TEXT_TEXT:
        xPropMapper = GetTextPropMapper();
        break;
    case XML_STYLE_FAMILY_TEXT_FRAME:
        xPropMapper = GetAutoFramePropMapper();
        break;
    case XML_STYLE_FAMILY_TEXT_SECTION:
        xPropMapper = GetSectionPropMapper();
        break;
    case XML_STYLE_FAMILY_TEXT_RUBY:
        xPropMapper = GetRubyPropMapper();
        break;
    }
    DBG_ASSERT( xPropMapper.is(), "There is the property mapper?" );

    vector< XMLPropertyState > xPropStates =
            xPropMapper->Filter( rPropSet );
    if( ppAddStates )
    {
        while( *ppAddStates )
        {
            xPropStates.push_back( **ppAddStates );
            ppAddStates++;
        }
    }

    if( xPropStates.size() > 0L || bCache )
    {
        Reference< XPropertySetInfo > xPropSetInfo =
            rPropSet->getPropertySetInfo();
        OUString sParent, sCondParent;
        Any aAny;
        switch( nFamily )
        {
        case XML_STYLE_FAMILY_TEXT_PARAGRAPH:
            if( xPropSetInfo->hasPropertyByName( sParaStyleName ) )
            {
                aAny = rPropSet->getPropertyValue( sParaStyleName );
                aAny >>= sParent;
            }
            if( xPropSetInfo->hasPropertyByName( sParaConditionalStyleName ) )
            {
                aAny = rPropSet->getPropertyValue( sParaConditionalStyleName );
                aAny >>= sCondParent;
            }
            if( xPropSetInfo->hasPropertyByName( sNumberingRules ) )
            {
                aAny = rPropSet->getPropertyValue( sNumberingRules );
                Reference < XIndexReplace > xNumRule;
                aAny >>= xNumRule;
                if( xNumRule.is() )
                {
                    Reference < XNamed > xNamed( xNumRule, UNO_QUERY );
                    OUString sName;
                    if( xNamed.is() )
                        sName = xNamed->getName();
                    if( !sName.getLength() || !pListAutoPool->HasName( sName ) )
                    {
                        // HasName returns false if the num rule is a style,
                        // because all style names have been registered while
                        // all automatic rules not.
                        pListAutoPool->Add( xNumRule );
                    }
                }
            }
            break;
        case XML_STYLE_FAMILY_TEXT_TEXT:
            if( xPropSetInfo->hasPropertyByName( sCharStyleName ) )
            {
                aAny = rPropSet->getPropertyValue( sCharStyleName );
                aAny >>= sParent;
            }
            break;
        case XML_STYLE_FAMILY_TEXT_FRAME:
            if( xPropSetInfo->hasPropertyByName( sFrameStyleName ) )
            {
                aAny = rPropSet->getPropertyValue( sFrameStyleName );
                aAny >>= sParent;
            }
            break;
        case XML_STYLE_FAMILY_TEXT_SECTION:
        case XML_STYLE_FAMILY_TEXT_RUBY:
            ; // section styles have no parents
            break;
        }
        if( xPropStates.size() > 0 )
        {
            if( bCache )
            {
                GetAutoStylePool().AddAndCache( nFamily, sParent, xPropStates );
                if( sCondParent.getLength() && sParent != sCondParent )
                    GetAutoStylePool().AddAndCache( nFamily, sCondParent, xPropStates );
            }
            else
            {
                GetAutoStylePool().Add( nFamily, sParent, xPropStates );
                if( sCondParent.getLength() && sParent != sCondParent )
                    GetAutoStylePool().Add( nFamily, sCondParent, xPropStates );
            }
        }
        else if( bCache )
        {
            GetAutoStylePool().AddAndCache( nFamily, sParent );
            if( sCondParent.getLength() && sParent != sCondParent )
                GetAutoStylePool().AddAndCache( nFamily, sCondParent );
        }

    }
}

OUString XMLTextParagraphExport::Find(
        sal_uInt16 nFamily,
           const Reference < XPropertySet > & rPropSet,
        const OUString& rParent,
        const XMLPropertyState** ppAddStates) const
{
    sal_Bool bCache = sal_False;
    OUString sName( rParent );
    UniReference < SvXMLExportPropertyMapper > xPropMapper;
    switch( nFamily )
    {
    case XML_STYLE_FAMILY_TEXT_PARAGRAPH:
        xPropMapper = GetParaPropMapper();
//      bCache = sal_True;
        break;
    case XML_STYLE_FAMILY_TEXT_TEXT:
        xPropMapper = GetTextPropMapper();
        break;
    case XML_STYLE_FAMILY_TEXT_FRAME:
        xPropMapper = GetAutoFramePropMapper();
        break;
    case XML_STYLE_FAMILY_TEXT_SECTION:
        xPropMapper = GetSectionPropMapper();
        break;
    case XML_STYLE_FAMILY_TEXT_RUBY:
        xPropMapper = GetRubyPropMapper();
        break;
    }
    OUString sCachedName;
    if( bCache )
    {
        sCachedName = GetAutoStylePool().FindAndRemoveCached( nFamily );
#ifdef PRODUCT
        if( sCachedName.getLength() )
            return sCachedName;
#endif
    }
    DBG_ASSERT( xPropMapper.is(), "There is the property mapper?" );
    vector< XMLPropertyState > xPropStates =
            xPropMapper->Filter( rPropSet );
    if( ppAddStates )
    {
        while( *ppAddStates )
        {
            xPropStates.push_back( **ppAddStates );
            ppAddStates++;
        }
    }

    if( xPropStates.size() > 0L )
        sName = GetAutoStylePool().Find( nFamily, sName, xPropStates );
    DBG_ASSERT( !bCache || !sCachedName.getLength() || sCachedName == sName,
                "cache error" );

    return sName;
}

OUString XMLTextParagraphExport::FindTextStyle(
           const Reference < XPropertySet > & rPropSet ) const
{
    Reference< XPropertySetInfo > xPropSetInfo =
        rPropSet->getPropertySetInfo();

    OUString sStyle;
    if( xPropSetInfo->hasPropertyByName( sCharStyleName ) )
    {
        Any aAny = rPropSet->getPropertyValue( sCharStyleName );
        aAny >>= sStyle;
    }

    return Find( XML_STYLE_FAMILY_TEXT_TEXT, rPropSet, sStyle );
}


void XMLTextParagraphExport::exportListChange(
        const XMLTextNumRuleInfo& rPrevInfo,
        const XMLTextNumRuleInfo& rNextInfo )
{
    // end a list
    if( rPrevInfo.GetLevel() > 0 &&
        ( !rNextInfo.HasSameNumRules( rPrevInfo ) ||
          rNextInfo.GetLevel() < rPrevInfo.GetLevel() ||
             rNextInfo.IsRestart()  ) )
    {
        sal_Int16 nPrevLevel = rPrevInfo.GetLevel();
        sal_Int16 nNextLevel =
            ( !rNextInfo.HasSameNumRules( rPrevInfo ) ||
              rNextInfo.IsRestart() ) ?  0 : rNextInfo.GetLevel();

        DBG_ASSERT( pListElements &&
                pListElements->Count() >= 2*(nNextLevel-nPrevLevel),
                "SwXMLExport::ExportListChange: list elements missing" );

        for( sal_Int16 i=nPrevLevel; i > nNextLevel; i-- )
        {
            for( sal_uInt16 j=0; j<2; j++ )
            {
                OUString *pElem = (*pListElements)[pListElements->Count()-1];
                pListElements->Remove( pListElements->Count()-1 );

                GetExport().GetDocHandler()->ignorableWhitespace(
                        GetExport().sWS );
                GetExport().GetDocHandler()->endElement( *pElem );

                delete pElem;
            }
        }
    }

    // start a new list
    if( rNextInfo.GetLevel() > 0 &&
        ( !rPrevInfo.HasSameNumRules( rNextInfo ) ||
          rPrevInfo.GetLevel() < rNextInfo.GetLevel() ||
             rNextInfo.IsRestart() ) )
    {
        sal_Int16 nPrevLevel =
            ( !rNextInfo.HasSameNumRules( rPrevInfo ) ||
              rNextInfo.IsRestart() ) ? 0 : rPrevInfo.GetLevel();
        sal_Int16 nNextLevel = rNextInfo.GetLevel();

        // Find out whether this is the first application of the list or not.
        // For named lists, we use the internal name. For unnamed lists, we
        // use the generated name. This works well, because there are either
        // unnamed or either named lists only.
        sal_Bool bListExported = sal_False;
        OUString sName;
        if( rNextInfo.IsNamed() )
            sName = rNextInfo.GetName();
        else
            sName = pListAutoPool->Find( rNextInfo.GetNumRules() );
        DBG_ASSERT( sName.getLength(), "list without a name" );
        if( sName.getLength() )
        {
            bListExported = pExportedLists &&
                             pExportedLists->Seek_Entry( (OUString *)&sName );
            if( !bListExported )
            {
                if( !pExportedLists )
                    pExportedLists = new OUStringsSort_Impl;
                pExportedLists->Insert( new OUString(sName) );
            }
        }
        sal_Bool bContinue = !rNextInfo.IsRestart() && bListExported &&
                             !rPrevInfo.HasSameNumRules( rNextInfo );

        for( sal_Int16 i=nPrevLevel; i < nNextLevel; i++)
        {
            // <text:ordered-list> or <text:unordered-list>
            GetExport().CheckAttrList();
            if( 0 == i )
            {
                // For named list, the name might be the name of an automatic
                // rule, so we have to take a look into the style pool.
                // For unnamed lists, we have done this already.
                if( rNextInfo.IsNamed() )
                {
                    OUString sTmp( pListAutoPool->Find(
                                            rNextInfo.GetNumRules() ) );
                    if( sTmp.getLength() )
                        sName = sTmp;
                }
                GetExport().AddAttribute( XML_NAMESPACE_TEXT, sXML_style_name,
                                          sName );
            }
            if( bContinue && rNextInfo.IsOrdered() )
                GetExport().AddAttributeASCII( XML_NAMESPACE_TEXT,
                                               sXML_continue_numbering,
                                                  sXML_true );

            const sal_Char *pLName =
                    rNextInfo.IsOrdered() ? sXML_ordered_list
                                          : sXML_unordered_list;

            OUString *pElem = new OUString(
                    GetExport().GetNamespaceMap().GetQNameByKey(
                                        XML_NAMESPACE_TEXT,
                                        OUString::createFromAscii(pLName) ) );
            GetExport().GetDocHandler()->ignorableWhitespace(
                    GetExport().sWS );
            GetExport().GetDocHandler()->startElement(
                    *pElem, GetExport().GetXAttrList() );
            GetExport().ClearAttrList();

            if( !pListElements )
                pListElements = new OUStrings_Impl;
            pListElements->Insert( pElem, pListElements->Count() );

            // <text:list-header> or <text:list-item>
            GetExport().CheckAttrList();
            if( rNextInfo.HasStartValue() )
            {
                OUStringBuffer aBuffer;
                aBuffer.append( (sal_Int32)rNextInfo.GetStartValue() );
                GetExport().AddAttribute( XML_NAMESPACE_TEXT, sXML_start_value,
                              aBuffer.makeStringAndClear() );
            }
            pLName = (rNextInfo.IsNumbered() || i+1 < nNextLevel)
                        ? sXML_list_item
                        : sXML_list_header;
            pElem = new OUString(  GetExport().GetNamespaceMap().GetQNameByKey(
                                        XML_NAMESPACE_TEXT,
                                        OUString::createFromAscii(pLName) ) );
            GetExport().GetDocHandler()->ignorableWhitespace( GetExport().sWS );
            GetExport().GetDocHandler()->startElement(
                    *pElem, GetExport().GetXAttrList() );
            GetExport().ClearAttrList();

            pListElements->Insert( pElem, pListElements->Count() );
        }
    }

    if( rNextInfo.GetLevel() > 0 && rNextInfo.IsNumbered() &&
        rPrevInfo.HasSameNumRules( rNextInfo ) &&
        rPrevInfo.GetLevel() >= rNextInfo.GetLevel() &&
        !rNextInfo.IsRestart() )
    {
        // </text:list-item> or </text:list-header>
        DBG_ASSERT( pListElements && pListElements->Count() >= 2,
                "SwXMLExport::ExportListChange: list elements missing" );

        OUString *pElem = (*pListElements)[pListElements->Count()-1];
        GetExport().GetDocHandler()->ignorableWhitespace( GetExport().sWS );
        GetExport().GetDocHandler()->endElement( *pElem );

        pListElements->Remove( pListElements->Count()-1 );
        delete pElem;

        // <text:list-item>
        GetExport().CheckAttrList();
        pElem = new OUString( GetExport().GetNamespaceMap().GetQNameByKey(
                                XML_NAMESPACE_TEXT,
                                OUString::createFromAscii(sXML_list_item) ) );
        GetExport().GetDocHandler()->ignorableWhitespace( GetExport().sWS );
        GetExport().GetDocHandler()->startElement( *pElem,
                GetExport().GetXAttrList() );
        GetExport().ClearAttrList();

        pListElements->Insert( pElem, pListElements->Count() );
    }
}


XMLTextParagraphExport::XMLTextParagraphExport(
        SvXMLExport& rExp,
        SvXMLAutoStylePoolP & rASP,
        sal_Int32 nProg ) :
    XMLStyleExport( rExp, OUString(), &rASP ),
//  rExport( rExp ),
    rAutoStylePool( rASP ),
    pListElements( 0 ),
    pExportedLists( 0 ),
    pListAutoPool( new XMLTextListAutoStylePool( this->GetExport() ) ),
    pFieldExport( 0 ),
    pPageTextFrameIdxs( 0 ),
    pPageGraphicIdxs( 0 ),
    pPageEmbeddedIdxs( 0 ),
    pPageShapeIdxs( 0 ),
    pFrameTextFrameIdxs( 0 ),
    pFrameGraphicIdxs( 0 ),
    pFrameEmbeddedIdxs( 0 ),
    pSectionExport( NULL ),
    pIndexMarkExport( NULL ),
    pRedlineExport( NULL ),
    pFrameShapeIdxs( 0 ),
    bBlock( sal_False ),
    bOpenRuby( sal_False ),
    sParagraphService(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.Paragraph")),
    sTableService(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextTable")),
    sTextFieldService(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextField")),
    sTextFrameService(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextFrame")),
    sTextGraphicService(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextGraphicObject")),
    sTextEmbeddedService(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextEmbeddedObject")),
    sTextEndnoteService(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.Endnote")),
    sTextContentService(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextContent")),
    sShapeService(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.drawing.Shape")),
    sParaStyleName(RTL_CONSTASCII_USTRINGPARAM("ParaStyleName")),
    sParaConditionalStyleName(RTL_CONSTASCII_USTRINGPARAM("ParaConditionalStyleName")),
    sParaChapterNumberingLevel(RTL_CONSTASCII_USTRINGPARAM("ParaChapterNumberingLevel")),
    sCharStyleName(RTL_CONSTASCII_USTRINGPARAM("CharStyleName")),
    sFrameStyleName(RTL_CONSTASCII_USTRINGPARAM("FrameStyleName")),
    sTextField(RTL_CONSTASCII_USTRINGPARAM("TextField")),
    sText(RTL_CONSTASCII_USTRINGPARAM("Text")),
    sFrame(RTL_CONSTASCII_USTRINGPARAM("Frame")),
    sCategory(RTL_CONSTASCII_USTRINGPARAM("Category")),
    sNumberingRules(RTL_CONSTASCII_USTRINGPARAM("NumberingRules")),
    sTextPortionType(RTL_CONSTASCII_USTRINGPARAM("TextPortionType")),
    sFootnote(RTL_CONSTASCII_USTRINGPARAM("Footnote")),
    sBookmark(RTL_CONSTASCII_USTRINGPARAM("Bookmark")),
    sReferenceMark(RTL_CONSTASCII_USTRINGPARAM("ReferenceMark")),
    sIsCollapsed(RTL_CONSTASCII_USTRINGPARAM("IsCollapsed")),
    sIsStart(RTL_CONSTASCII_USTRINGPARAM("IsStart")),
    sReferenceId(RTL_CONSTASCII_USTRINGPARAM("ReferenceId")),
    sNumberingType(RTL_CONSTASCII_USTRINGPARAM("NumberingType")),
    sPageStyleName(RTL_CONSTASCII_USTRINGPARAM("PageStyleName")),
    sPageDescName(RTL_CONSTASCII_USTRINGPARAM("PageDescName")),
    sPrefix(RTL_CONSTASCII_USTRINGPARAM("Prefix")),
    sStartAt(RTL_CONSTASCII_USTRINGPARAM("StartAt")),
    sSuffix(RTL_CONSTASCII_USTRINGPARAM("Suffix")),
    sPositionEndOfDoc(RTL_CONSTASCII_USTRINGPARAM("PositionEndOfDoc")),
    sFootnoteCounting(RTL_CONSTASCII_USTRINGPARAM("FootnoteCounting")),
    sEndNotice(RTL_CONSTASCII_USTRINGPARAM("EndNotice")),
    sBeginNotice(RTL_CONSTASCII_USTRINGPARAM("BeginNotice")),
    sFrameWidthAbsolute(RTL_CONSTASCII_USTRINGPARAM("FrameWidthAbsolute")),
    sFrameWidthPercent(RTL_CONSTASCII_USTRINGPARAM("FrameWidthPercent")),
    sFrameHeightAbsolute(RTL_CONSTASCII_USTRINGPARAM("FrameHeightAbsolute")),
    sFrameHeightPercent(RTL_CONSTASCII_USTRINGPARAM("FrameHeightPercent")),
    sWidth(RTL_CONSTASCII_USTRINGPARAM("Width")),
    sRelativeWidth(RTL_CONSTASCII_USTRINGPARAM("RelativeWidth")),
    sHeight(RTL_CONSTASCII_USTRINGPARAM("Height")),
    sRelativeHeight(RTL_CONSTASCII_USTRINGPARAM("RelativeHeight")),
    sSizeType(RTL_CONSTASCII_USTRINGPARAM("SizeType")),
    sIsSyncWidthToHeight(RTL_CONSTASCII_USTRINGPARAM("IsSyncWidthToHeight")),
    sIsSyncHeightToWidth(RTL_CONSTASCII_USTRINGPARAM("IsSyncHeightToWidth")),
    sHoriOrient(RTL_CONSTASCII_USTRINGPARAM("HoriOrient")),
    sHoriOrientPosition(RTL_CONSTASCII_USTRINGPARAM("HoriOrientPosition")),
    sVertOrient(RTL_CONSTASCII_USTRINGPARAM("VertOrient")),
    sVertOrientPosition(RTL_CONSTASCII_USTRINGPARAM("VertOrientPosition")),
    sChainNextName(RTL_CONSTASCII_USTRINGPARAM("ChainNextName")),
    sAnchorType(RTL_CONSTASCII_USTRINGPARAM("AnchorType")),
    sAnchorPageNo(RTL_CONSTASCII_USTRINGPARAM("AnchorPageNo")),
    sGraphicURL(RTL_CONSTASCII_USTRINGPARAM("GraphicURL")),
    sGraphicFilter(RTL_CONSTASCII_USTRINGPARAM("GraphicFilter")),
    sGraphicRotation(RTL_CONSTASCII_USTRINGPARAM("GraphicRotation")),
    sAlternativeText(RTL_CONSTASCII_USTRINGPARAM("AlternativeText")),
    sHyperLinkURL(RTL_CONSTASCII_USTRINGPARAM("HyperLinkURL")),
    sHyperLinkName(RTL_CONSTASCII_USTRINGPARAM("HyperLinkName")),
    sHyperLinkTarget(RTL_CONSTASCII_USTRINGPARAM("HyperLinkTarget")),
    sUnvisitedCharStyleName(RTL_CONSTASCII_USTRINGPARAM("UnvisitedCharStyleName")),
    sVisitedCharStyleName(RTL_CONSTASCII_USTRINGPARAM("VisitedCharStyleName")),
    sTextSection(RTL_CONSTASCII_USTRINGPARAM("TextSection")),
    sDocumentIndex(RTL_CONSTASCII_USTRINGPARAM("DocumentIndex")),
    sDocumentIndexMark(RTL_CONSTASCII_USTRINGPARAM("DocumentIndexMark")),
    sActualSize(RTL_CONSTASCII_USTRINGPARAM("ActualSize")),
    sContourPolyPolygon(RTL_CONSTASCII_USTRINGPARAM("ContourPolyPolygon")),
    sAnchorCharStyleName(RTL_CONSTASCII_USTRINGPARAM("AnchorCharStyleName")),
    sServerMap(RTL_CONSTASCII_USTRINGPARAM("ServerMap")),
    sRedline(RTL_CONSTASCII_USTRINGPARAM("Redline")),
    sRuby(RTL_CONSTASCII_USTRINGPARAM("Ruby")),
    sRubyText(RTL_CONSTASCII_USTRINGPARAM("RubyText")),
    sRubyAdjust(RTL_CONSTASCII_USTRINGPARAM("RubyAdjust")),
    sRubyCharStyleName(RTL_CONSTASCII_USTRINGPARAM("RubyCharStyleName")),
    nProgress( nProg )
{
    UniReference < XMLPropertySetMapper > xPropMapper =
        new XMLTextPropertySetMapper( TEXT_PROP_MAP_PARA );
    xParaPropMapper = new XMLTextExportPropertySetMapper( xPropMapper,
                                                             GetExport() );

    OUString sFamily( RTL_CONSTASCII_USTRINGPARAM(sXML_paragraph) );
    OUString sPrefix( 'P' );
    rAutoStylePool.AddFamily( XML_STYLE_FAMILY_TEXT_PARAGRAPH, sFamily,
                              xParaPropMapper, sPrefix );

    xPropMapper = new XMLTextPropertySetMapper( TEXT_PROP_MAP_TEXT );
    xTextPropMapper = new XMLTextExportPropertySetMapper( xPropMapper,
                                                             GetExport() );
    sFamily = OUString( RTL_CONSTASCII_USTRINGPARAM(sXML_text) );
    sPrefix = OUString( 'T' );
    rAutoStylePool.AddFamily( XML_STYLE_FAMILY_TEXT_TEXT, sFamily,
                              xTextPropMapper, sPrefix );

    xPropMapper = new XMLTextPropertySetMapper( TEXT_PROP_MAP_AUTO_FRAME );
    xAutoFramePropMapper = new XMLTextExportPropertySetMapper( xPropMapper,
                                                                  GetExport() );
    sFamily = OUString( RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_SD_GRAPHICS_NAME) );
    sPrefix = OUString( RTL_CONSTASCII_USTRINGPARAM( "fr" ) );
    rAutoStylePool.AddFamily( XML_STYLE_FAMILY_TEXT_FRAME, sFamily,
                              xAutoFramePropMapper, sPrefix );

    xPropMapper = new XMLTextPropertySetMapper( TEXT_PROP_MAP_SECTION );
    xSectionPropMapper = new XMLTextExportPropertySetMapper( xPropMapper,
                                                             GetExport() );
    sFamily = OUString( RTL_CONSTASCII_USTRINGPARAM( sXML_section ) );
    sPrefix = OUString( RTL_CONSTASCII_USTRINGPARAM( "Sect" ) );
    rAutoStylePool.AddFamily( XML_STYLE_FAMILY_TEXT_SECTION, sFamily,
                              xSectionPropMapper, sPrefix );

    xPropMapper = new XMLTextPropertySetMapper( TEXT_PROP_MAP_RUBY );
    xRubyPropMapper = new SvXMLExportPropertyMapper( xPropMapper );
    sFamily = OUString( RTL_CONSTASCII_USTRINGPARAM( sXML_ruby ) );
    sPrefix = OUString( RTL_CONSTASCII_USTRINGPARAM( "Ru" ) );
    rAutoStylePool.AddFamily( XML_STYLE_FAMILY_TEXT_RUBY, sFamily,
                              xRubyPropMapper, sPrefix );

    xPropMapper = new XMLTextPropertySetMapper( TEXT_PROP_MAP_FRAME );
    xFramePropMapper = new XMLTextExportPropertySetMapper( xPropMapper,
                                                              GetExport() );
    pSectionExport = new XMLSectionExport( rExp, *this );
    pIndexMarkExport = new XMLIndexMarkExport( rExp, *this );
    pRedlineExport = IsBlockMode() ? NULL : new XMLRedlineExport( rExp );

    // The text field helper needs a pre-constructed XMLPropertyState
    // to export the combined characters field. We construct that
    // here, because we need the text property mapper to do it.

    // construct Any value, then find index
    Any aAny;
    sal_Bool bTmp = sal_True;
    aAny.setValue(&bTmp, ::getBooleanCppuType());
    sal_Int32 nIndex = xTextPropMapper->getPropertySetMapper()->FindEntryIndex(
                                "", XML_NAMESPACE_STYLE, sXML_text_combine);
    pFieldExport = new XMLTextFieldExport( rExp, new XMLPropertyState( nIndex, aAny ) );
}

XMLTextParagraphExport::~XMLTextParagraphExport()
{
    delete pRedlineExport;
    delete pIndexMarkExport;
    delete pSectionExport;
    delete pFieldExport;
    delete pListElements;
    delete pExportedLists;
    delete pListAutoPool;
    delete pPageTextFrameIdxs;
    delete pPageGraphicIdxs;
    delete pPageEmbeddedIdxs;
    delete pPageShapeIdxs;
    delete pFrameTextFrameIdxs;
    delete pFrameGraphicIdxs;
    delete pFrameEmbeddedIdxs;
    delete pFrameShapeIdxs;
}

SvXMLExportPropertyMapper *XMLTextParagraphExport::CreateShapeExtPropMapper(
        SvXMLExport& rExport )
{
    UniReference < XMLPropertySetMapper > xPropMapper =
        new XMLTextPropertySetMapper( TEXT_PROP_MAP_SHAPE );
    return new XMLTextExportPropertySetMapper( xPropMapper, rExport );
}

SvXMLExportPropertyMapper *XMLTextParagraphExport::CreateCharExtPropMapper(
        SvXMLExport& rExport)
{
    XMLPropertySetMapper *pPropMapper =
        new XMLTextPropertySetMapper( TEXT_PROP_MAP_TEXT );
    return new XMLTextExportPropertySetMapper( pPropMapper, rExport );
}

SvXMLExportPropertyMapper *XMLTextParagraphExport::CreateParaExtPropMapper(
        SvXMLExport& rExport)
{
    XMLPropertySetMapper *pPropMapper =
        new XMLTextPropertySetMapper( TEXT_PROP_MAP_PARA );
    return new XMLTextExportPropertySetMapper( pPropMapper, rExport );
}

void XMLTextParagraphExport::collectFrames()
{
    Reference < XTextFramesSupplier > xTFS( GetExport().GetModel(), UNO_QUERY );
    if( xTFS.is() )
    {
        xTextFrames = Reference < XIndexAccess >( xTFS->getTextFrames(),
                                                  UNO_QUERY );
        sal_Int32 nCount =  xTextFrames->getCount();
        for( sal_Int32 i = 0; i < nCount; i++ )
        {
            Any aAny = xTextFrames->getByIndex( i );
            Reference < XTextFrame > xTxtFrame;
            aAny >>= xTxtFrame;
            Reference < XPropertySet > xPropSet( xTxtFrame, UNO_QUERY );

            aAny = xPropSet->getPropertyValue( sAnchorType );
            TextContentAnchorType eAnchor;
            aAny >>= eAnchor;

            switch( eAnchor )
            {
            case TextContentAnchorType_AT_PAGE:
                if( !pPageTextFrameIdxs )
                    pPageTextFrameIdxs = new SvLongs;
                pPageTextFrameIdxs->Insert( i, pPageTextFrameIdxs->Count() );
                break;
            case TextContentAnchorType_AT_FRAME:
                if( !pFrameTextFrameIdxs )
                    pFrameTextFrameIdxs = new SvLongs;
                pFrameTextFrameIdxs->Insert( i, pFrameTextFrameIdxs->Count() );
                break;
            }
        }
    }

    Reference < XTextGraphicObjectsSupplier > xTGOS( GetExport().GetModel(),
                                                    UNO_QUERY );
    if( xTGOS.is() )
    {
        xGraphics = Reference < XIndexAccess >( xTGOS->getGraphicObjects(),
                                                  UNO_QUERY );
        sal_Int32 nCount =  xGraphics->getCount();
        for( sal_Int32 i = 0; i < nCount; i++ )
        {
            Any aAny = xGraphics->getByIndex( i );
            Reference < XTextContent > xTxtCntnt;
            aAny >>= xTxtCntnt;
            Reference < XPropertySet > xPropSet( xTxtCntnt, UNO_QUERY );

            aAny = xPropSet->getPropertyValue( sAnchorType );
            TextContentAnchorType eAnchor;
            aAny >>= eAnchor;

            switch( eAnchor )
            {
            case TextContentAnchorType_AT_PAGE:
                if( !pPageGraphicIdxs )
                    pPageGraphicIdxs = new SvLongs;
                pPageGraphicIdxs->Insert( i, pPageGraphicIdxs->Count() );
                break;
            case TextContentAnchorType_AT_FRAME:
                if( !pFrameGraphicIdxs )
                    pFrameGraphicIdxs = new SvLongs;
                pFrameGraphicIdxs->Insert( i, pFrameGraphicIdxs->Count() );
                break;
            }
        }
    }

    Reference < XTextEmbeddedObjectsSupplier > xTEOS( GetExport().GetModel(),
                                                    UNO_QUERY );
    if( xTEOS.is() )
    {
        xEmbeddeds = Reference < XIndexAccess >( xTEOS->getEmbeddedObjects(),
                                                  UNO_QUERY );
        sal_Int32 nCount =  xEmbeddeds->getCount();
        for( sal_Int32 i = 0; i < nCount; i++ )
        {
            Any aAny = xEmbeddeds->getByIndex( i );
            Reference < XTextContent > xTxtCntnt;
            aAny >>= xTxtCntnt;
            Reference < XPropertySet > xPropSet( xTxtCntnt, UNO_QUERY );

            aAny = xPropSet->getPropertyValue( sAnchorType );
            TextContentAnchorType eAnchor;
            aAny >>= eAnchor;

            switch( eAnchor )
            {
            case TextContentAnchorType_AT_PAGE:
                if( !pPageEmbeddedIdxs )
                    pPageEmbeddedIdxs = new SvLongs;
                pPageEmbeddedIdxs->Insert( i, pPageEmbeddedIdxs->Count() );
                break;
            case TextContentAnchorType_AT_FRAME:
                if( !pFrameEmbeddedIdxs )
                    pFrameEmbeddedIdxs = new SvLongs;
                pFrameEmbeddedIdxs->Insert( i, pFrameEmbeddedIdxs->Count() );
                break;
            }
        }
    }

    Reference < XDrawPageSupplier > xDPS( GetExport().GetModel(),
                                                    UNO_QUERY );
    if( xDPS.is() )
    {
        xShapes = Reference < XIndexAccess >( xDPS->getDrawPage(),
                                                  UNO_QUERY );
        sal_Int32 nCount =  xShapes->getCount();
        for( sal_Int32 i = 0; i < nCount; i++ )
        {
            Any aAny = xShapes->getByIndex( i );
            Reference < XShape > xShape;
            aAny >>= xShape;
            if( !xShape.is() )
                continue;

            Reference < XPropertySet > xPropSet( xShape, UNO_QUERY );

            aAny = xPropSet->getPropertyValue( sAnchorType );
            TextContentAnchorType eAnchor;
            aAny >>= eAnchor;

            if( TextContentAnchorType_AT_PAGE != eAnchor &&
                TextContentAnchorType_AT_FRAME != eAnchor )
                continue;

            Reference<XServiceInfo> xServiceInfo( xShape,
                                                  UNO_QUERY );
            if( xServiceInfo->supportsService( sTextFrameService ) ||
                  xServiceInfo->supportsService( sTextGraphicService ) ||
                xServiceInfo->supportsService( sTextEmbeddedService ) )
                continue;

            if( TextContentAnchorType_AT_PAGE == eAnchor )
            {
                if( !pPageShapeIdxs )
                    pPageShapeIdxs = new SvLongs;
                pPageShapeIdxs->Insert( i, pPageShapeIdxs->Count() );
            }
            else
            {
                if( !pFrameShapeIdxs )
                    pFrameShapeIdxs = new SvLongs;
                pFrameShapeIdxs->Insert( i, pFrameShapeIdxs->Count() );
            }
        }
    }
}

void XMLTextParagraphExport::exportPageFrames( sal_Bool bAutoStyles,
                                               sal_Bool bProgress )
{
    if( pPageTextFrameIdxs )
    {
        for( sal_uInt16 i = 0; i < pPageTextFrameIdxs->Count(); i++ )
        {
            Any aAny = xTextFrames->getByIndex( (*pPageTextFrameIdxs)[i] );
            Reference < XTextFrame > xTxtFrame;
            aAny >>= xTxtFrame;
            Reference < XTextContent > xTxtCntnt( xTxtFrame, UNO_QUERY );
            exportTextFrame( xTxtCntnt, bAutoStyles, bProgress );
        }
    }
    if( pPageGraphicIdxs )
    {
        for( sal_uInt16 i = 0; i < pPageGraphicIdxs->Count(); i++ )
        {
            Any aAny = xGraphics->getByIndex( (*pPageGraphicIdxs)[i] );
            Reference < XTextContent > xTxtCntnt;
            aAny >>= xTxtCntnt;
            exportTextGraphic( xTxtCntnt, bAutoStyles );
        }
    }
    if( pPageEmbeddedIdxs )
    {
        for( sal_uInt16 i = 0; i < pPageEmbeddedIdxs->Count(); i++ )
        {
            Any aAny = xEmbeddeds->getByIndex( (*pPageEmbeddedIdxs)[i] );
            Reference < XTextContent > xTxtCntnt;
            aAny >>= xTxtCntnt;
            exportTextEmbedded( xTxtCntnt, bAutoStyles );
        }
    }
    if( pPageShapeIdxs )
    {
        for( sal_uInt16 i = 0; i < pPageShapeIdxs->Count(); i++ )
        {
            Any aAny = xShapes->getByIndex( (*pPageShapeIdxs)[i] );
            Reference < XShape > xShape;
            aAny >>= xShape;
            Reference < XTextContent > xTxtCntnt( xShape, UNO_QUERY );
            exportShape( xTxtCntnt, bAutoStyles );
        }
    }
}

sal_Bool lcl_txtpara_isFrameAnchor(
        const Reference < XTextContent > rTxtCntnt,
        const Reference < XTextFrame >& rParentTxtFrame )
{
    Reference < XPropertySet > xPropSet( rTxtCntnt, UNO_QUERY );
    Any aAny = xPropSet->getPropertyValue( OUString(RTL_CONSTASCII_USTRINGPARAM("AnchorFrame") ) );
    Reference < XTextFrame > xAnchorTxtFrame;
    aAny >>= xAnchorTxtFrame;

    return xAnchorTxtFrame == rParentTxtFrame;
}

void XMLTextParagraphExport::exportFrameFrames(
        sal_Bool bAutoStyles,
        sal_Bool bProgress,
        const Reference < XTextFrame > *pParentTxtFrame )
{
    if( pFrameTextFrameIdxs )
    {
        sal_uInt16 i = 0;
        while( i < pFrameTextFrameIdxs->Count() )
        {
            Any aAny = xTextFrames->getByIndex( (*pFrameTextFrameIdxs)[i] );
            Reference < XTextFrame > xTxtFrame;
            aAny >>= xTxtFrame;
            Reference < XTextContent > xTxtCntnt( xTxtFrame, UNO_QUERY );
            if( bAutoStyles ||
                lcl_txtpara_isFrameAnchor( xTxtCntnt, *pParentTxtFrame ) )
            {
                if( !bAutoStyles )
                    pFrameTextFrameIdxs->Remove( i );
                sal_uInt16 nOldCount = pFrameTextFrameIdxs->Count();
                exportTextFrame( xTxtCntnt, bAutoStyles, bProgress );
                if( bAutoStyles )
                    i++;
                else if( pFrameTextFrameIdxs->Count() != nOldCount )
                    i = 0;
            }
            else
                i++;
        }
    }
    if( pFrameGraphicIdxs )
    {
        sal_uInt16 i = 0;
        while( i < pFrameGraphicIdxs->Count() )
        {
            Any aAny = xGraphics->getByIndex( (*pFrameGraphicIdxs)[i] );
            Reference < XTextContent > xTxtCntnt;
            aAny >>= xTxtCntnt;
            if( bAutoStyles ||
                lcl_txtpara_isFrameAnchor( xTxtCntnt, *pParentTxtFrame ) )
            {
                if( !bAutoStyles )
                    pFrameGraphicIdxs->Remove( i );
                sal_uInt16 nOldCount = pFrameGraphicIdxs->Count();
                exportTextGraphic( xTxtCntnt, bAutoStyles );
                if( bAutoStyles )
                    i++;
                else if( pFrameGraphicIdxs->Count() != nOldCount )
                    i = 0;
            }
            else
                i++;
        }
    }
    if( pFrameEmbeddedIdxs )
    {
        sal_uInt16 i = 0;
        while( i < pFrameEmbeddedIdxs->Count() )
        {
            Any aAny = xEmbeddeds->getByIndex( (*pFrameEmbeddedIdxs)[i] );
            Reference < XTextContent > xTxtCntnt;
            aAny >>= xTxtCntnt;
            if( bAutoStyles ||
                lcl_txtpara_isFrameAnchor( xTxtCntnt, *pParentTxtFrame ) )
            {
                if( !bAutoStyles )
                    pFrameEmbeddedIdxs->Remove( i );
                sal_uInt16 nOldCount = pFrameEmbeddedIdxs->Count();
                exportTextEmbedded( xTxtCntnt, bAutoStyles );
                if( bAutoStyles )
                    i++;
                else if( pFrameEmbeddedIdxs->Count() != nOldCount )
                    i = 0;
            }
            else
                i++;
        }
    }
    if( pFrameShapeIdxs )
    {
        sal_uInt16 i = 0;
        while( i < pFrameShapeIdxs->Count() )
        {
            Any aAny = xShapes->getByIndex( (*pFrameShapeIdxs)[i] );
            Reference < XShape > xShape;
            aAny >>= xShape;
            Reference < XTextContent > xTxtCntnt( xShape, UNO_QUERY );
            if( bAutoStyles ||
                lcl_txtpara_isFrameAnchor( xTxtCntnt, *pParentTxtFrame ) )
            {
                if( !bAutoStyles )
                    pFrameShapeIdxs->Remove( i );
                sal_uInt16 nOldCount = pFrameShapeIdxs->Count();
                exportShape( xTxtCntnt, bAutoStyles );
                if( bAutoStyles )
                    i++;
                else if( pFrameShapeIdxs->Count() != nOldCount )
                    i = 0;
            }
            else
                i++;
        }
    }
}

void XMLTextParagraphExport::exportText(
        const Reference < XText > & rText,
        sal_Bool bAutoStyles,
        sal_Bool bProgress,
        sal_Bool bExportParagraph )
{
    if( bAutoStyles )
        GetExport().GetShapeExport(); // make sure the graphics styles family
                                      // is added
    Reference < XEnumerationAccess > xEA( rText, UNO_QUERY );
    Reference < XEnumeration > xParaEnum = xEA->createEnumeration();
    Reference < XPropertySet > xPropertySet( rText, UNO_QUERY );
    Reference < XTextSection > xBaseSection;
    if (xPropertySet.is())
    {
        if (xPropertySet->getPropertySetInfo()->
            hasPropertyByName( sTextSection ))
        {
            Any aAny = xPropertySet->getPropertyValue(sTextSection);
            aAny >>= xBaseSection;
        }
    }
    exportTextContentEnumeration( xParaEnum, bAutoStyles, xBaseSection,
                                  bProgress, bExportParagraph   );
}

void XMLTextParagraphExport::exportTextContentEnumeration(
        const Reference < XEnumeration > & rContEnum,
        sal_Bool bAutoStyles,
        const Reference < XTextSection > & rBaseSection,
        sal_Bool bProgress,
        sal_Bool bExportParagraph )
{
    XMLTextNumRuleInfo aPrevNumInfo;
    XMLTextNumRuleInfo aNextNumInfo;

    sal_Bool bHasContent sal_False;
    Reference<XTextSection> xCurrentTextSection = rBaseSection;

    sal_Bool bHoldElement = sal_False;
    Reference < XTextContent > xTxtCntnt;
    while( bHoldElement || rContEnum->hasMoreElements() )
    {
        if (bHoldElement)
        {
            bHoldElement = sal_False;
        }
        else
        {
            Any aAny = rContEnum->nextElement();
            aAny >>= xTxtCntnt;
        }

        Reference<XServiceInfo> xServiceInfo( xTxtCntnt, UNO_QUERY );
        if( xServiceInfo->supportsService( sParagraphService ) )
        {
            if( !bAutoStyles )
            {
                aNextNumInfo.Set( xTxtCntnt );
            }

            exportListAndSectionChange( xCurrentTextSection, xTxtCntnt,
                                        aPrevNumInfo, aNextNumInfo,
                                        bAutoStyles );

            // if we found a mute section: skip all section content
            if (pSectionExport->IsMuteSection(xCurrentTextSection))
            {
                while (rContEnum->hasMoreElements() &&
                       pSectionExport->IsInSection( xCurrentTextSection,
                                                    xTxtCntnt, sal_True ))
                {
                    Any aAny = rContEnum->nextElement();
                    aAny >>= xTxtCntnt;
                }
                // the first non-mute element still needs to be processed
                bHoldElement =
                    ! pSectionExport->IsInSection( xCurrentTextSection,
                                                   xTxtCntnt, sal_False );
            }
            else
                exportParagraph( xTxtCntnt, bAutoStyles, bProgress,
                                 bExportParagraph );
            bHasContent = sal_True;
        }
        else if( xServiceInfo->supportsService( sTableService ) )
        {
            if( !bAutoStyles )
            {
                aNextNumInfo.Reset();
            }

            exportListAndSectionChange( xCurrentTextSection, xTxtCntnt,
                                        aPrevNumInfo, aNextNumInfo,
                                        bAutoStyles );

            // export start + end redlines (for wholly redlined tables)
            if ((! bAutoStyles) && (NULL != pRedlineExport))
                pRedlineExport->ExportStartOrEndRedline(xTxtCntnt, sal_True);

            exportTable( xTxtCntnt, bAutoStyles, bProgress  );

            if ((! bAutoStyles) && (NULL != pRedlineExport))
                pRedlineExport->ExportStartOrEndRedline(xTxtCntnt, sal_False);

            bHasContent = sal_True;
        }
        else if( xServiceInfo->supportsService( sTextFrameService ) )
        {
            exportTextFrame( xTxtCntnt, bAutoStyles, bProgress );
        }
        else if( xServiceInfo->supportsService( sTextGraphicService ) )
        {
            exportTextGraphic( xTxtCntnt, bAutoStyles );
        }
        else if( xServiceInfo->supportsService( sTextEmbeddedService ) )
        {
            exportTextEmbedded( xTxtCntnt, bAutoStyles );
        }
        else if( xServiceInfo->supportsService( sShapeService ) )
        {
            exportShape( xTxtCntnt, bAutoStyles );
        }
        else
        {
            DBG_ASSERT( !xTxtCntnt.is(), "unknown text content" );
        }

        if( !bAutoStyles )
        {
            aPrevNumInfo = aNextNumInfo;
        }
    }

    if( bHasContent && !bAutoStyles )
    {
        aNextNumInfo.Reset();

        // close open lists and sections; no new styles
        exportListAndSectionChange( xCurrentTextSection, rBaseSection,
                                    aPrevNumInfo, aNextNumInfo,
                                    bAutoStyles );
    }
}

void XMLTextParagraphExport::exportParagraph(
        const Reference < XTextContent > & rTextContent,
        sal_Bool bAutoStyles, sal_Bool bProgress, sal_Bool bExportParagraph )
{
    sal_Int8 nOutlineLevel = -1;

    if( bProgress )
    {
        ProgressBarHelper *pProgress = GetExport().GetProgressBarHelper();
            pProgress->SetValue( ++nProgress );
    }

    Reference < XPropertySet > xPropSet( rTextContent, UNO_QUERY );
    Reference< XPropertySetInfo > xPropSetInfo =
        xPropSet->getPropertySetInfo();
    Any aAny;

    if( bExportParagraph )
    {
        if( bAutoStyles )
        {
            Add( XML_STYLE_FAMILY_TEXT_PARAGRAPH, xPropSet );
        }
        else
        {
            OUString sStyle;
            if( xPropSetInfo->hasPropertyByName( sParaStyleName ) )
            {
                aAny = xPropSet->getPropertyValue( sParaStyleName );
                aAny >>= sStyle;
            }

            OUString sAutoStyle( sStyle );
            sAutoStyle = Find( XML_STYLE_FAMILY_TEXT_PARAGRAPH, xPropSet, sStyle );
            if( sAutoStyle.getLength() )
                GetExport().AddAttribute( XML_NAMESPACE_TEXT, sXML_style_name,
                                          sAutoStyle );

            if( xPropSetInfo->hasPropertyByName( sParaConditionalStyleName ) )
            {
                OUString sCondStyle;
                aAny = xPropSet->getPropertyValue( sParaConditionalStyleName );
                aAny >>= sCondStyle;
                if( sCondStyle != sStyle )
                {
                    sCondStyle = Find( XML_STYLE_FAMILY_TEXT_PARAGRAPH, xPropSet,
                                          sCondStyle );
                    if( sCondStyle.getLength() )
                        GetExport().AddAttribute( XML_NAMESPACE_TEXT,
                                                  sXML_cond_style_name,
                                                  sCondStyle );
                }
            }

            if( xPropSetInfo->hasPropertyByName( sParaChapterNumberingLevel ) )
            {
                aAny = xPropSet->getPropertyValue( sParaChapterNumberingLevel );
                aAny >>= nOutlineLevel;
                if( -1 != nOutlineLevel )
                {
                    OUStringBuffer sTmp;
                    sTmp.append( (sal_Int32)nOutlineLevel+1L );
                    GetExport().AddAttribute( XML_NAMESPACE_TEXT, sXML_level,
                                  sTmp.makeStringAndClear() );
                }
            }
        }
    }

    Reference < XEnumerationAccess > xEA( rTextContent, UNO_QUERY );
    Reference < XEnumeration > xTextEnum = xEA->createEnumeration();

    Reference < XEnumeration> xContentEnum;
    Reference < XContentEnumerationAccess > xCEA( rTextContent, UNO_QUERY );
    if( xCEA.is() )
        xContentEnum = xCEA->createContentEnumeration( sTextContentService );

    Reference < XTextSection > xSection;
    if (xPropSetInfo->hasPropertyByName(sTextSection))
    {
        aAny = xPropSet->getPropertyValue(sTextSection);
        aAny >>= xSection;
    }

    if( bAutoStyles )
    {
        if( xContentEnum.is() )
            exportTextContentEnumeration( xContentEnum, bAutoStyles, xSection,
                                          bProgress );
        exportTextRangeEnumeration( xTextEnum, bAutoStyles, bProgress );
    }
    else
    {
        const sal_Char *pElem =
            -1 == nOutlineLevel ? sXML_p : sXML_h;
        SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_TEXT, pElem,
                                  sal_True, sal_False );
        if( xContentEnum.is() )
            exportTextContentEnumeration( xContentEnum, bAutoStyles, xSection,
                                          bProgress );
        exportTextRangeEnumeration( xTextEnum, bAutoStyles, bProgress );
    }
}

// bookmarks, reference marks (and TOC marks) are the same except for the
// element names. We use the same method for export and it an array with
// the proper element names
static const sal_Char* lcl_XmlReferenceElements[] = {
    sXML_reference_mark, sXML_reference_mark_start, sXML_reference_mark_end };
static const sal_Char* lcl_XmlBookmarkElements[] = {
    sXML_bookmark, sXML_bookmark_start, sXML_bookmark_end };


void XMLTextParagraphExport::exportTextRangeEnumeration(
        const Reference < XEnumeration > & rTextEnum,
        sal_Bool bAutoStyles, sal_Bool bProgress )
{
    sal_Bool bPrevCharIsSpace = sal_True;

    while( rTextEnum->hasMoreElements() )
    {
        Any aAny = rTextEnum->nextElement();
        Reference < XTextRange > xTxtRange;
        aAny >>= xTxtRange;

        Reference<XPropertySet> xPropSet(xTxtRange, UNO_QUERY);
        Reference<XPropertySetInfo> xPropInfo =
            xPropSet->getPropertySetInfo();

        if (xPropInfo->hasPropertyByName(sTextPortionType))
        {
            Any aAny;
            aAny = xPropSet->getPropertyValue(sTextPortionType);
            OUString sType;
            aAny >>= sType;

            if( sType.equals(sText))
            {
                exportTextRange( xTxtRange, bAutoStyles,
                                 bPrevCharIsSpace );
            }
            else if( sType.equals(sTextField))
            {
                exportTextField( xTxtRange, bAutoStyles );
                bPrevCharIsSpace = sal_False;
            }
            else if( sType.equals( sFrame ) )
            {
                Reference < XEnumeration> xContentEnum;
                Reference < XContentEnumerationAccess > xCEA( xTxtRange,
                                                              UNO_QUERY );
                if( xCEA.is() )
                    xContentEnum = xCEA->createContentEnumeration(
                                                    sTextContentService );
                // frames are never in sections
                Reference<XTextSection> xSection;
                if( xContentEnum.is() )
                    exportTextContentEnumeration( xContentEnum, bAutoStyles,
                                                  xSection, bProgress );
                bPrevCharIsSpace = sal_False;
            }
            else if (sType.equals(sFootnote))
            {
                exportTextFootnote(xPropSet,
                                   xTxtRange->getString(),
                                   bAutoStyles, bProgress );
                bPrevCharIsSpace = sal_False;
            }
            else if (sType.equals(sBookmark))
            {
                exportTextMark(xPropSet,
                               sBookmark,
                               lcl_XmlBookmarkElements,
                               bAutoStyles);
            }
            else if (sType.equals(sReferenceMark))
            {
                exportTextMark(xPropSet,
                               sReferenceMark,
                               lcl_XmlReferenceElements,
                               bAutoStyles);
            }
            else if (sType.equals(sDocumentIndexMark))
            {
                pIndexMarkExport->ExportIndexMark(xPropSet, bAutoStyles);
            }
            else if (sType.equals(sRedline))
            {
                if (NULL != pRedlineExport)
                    pRedlineExport->ExportChange(xPropSet, bAutoStyles);
            }
            else if (sType.equals(sRuby))
            {
                exportRuby(xPropSet, bAutoStyles);
            }
            else
                DBG_ERROR("unknown text portion type");
        }
        else
        {
            Reference<XServiceInfo> xServiceInfo( xTxtRange, UNO_QUERY );
            if( xServiceInfo->supportsService( sTextFieldService ) )
            {
                exportTextField( xTxtRange, bAutoStyles );
                bPrevCharIsSpace = sal_False;
            }
            else
            {
                // no TextPortionType property -> non-Writer app -> text
                exportTextRange( xTxtRange, bAutoStyles, bPrevCharIsSpace );
            }
        }
    }

    DBG_ASSERT( !bOpenRuby, "Red Alert: Ruby still open!" );
}

void XMLTextParagraphExport::exportTable(
        const Reference < XTextContent > & rTextContent,
        sal_Bool bAutoStyles, sal_Bool bProgress )
{
}

void XMLTextParagraphExport::exportTextField(
        const Reference < XTextRange > & rTextRange,
        sal_Bool bAutoStyles )
{
    Reference < XPropertySet > xPropSet( rTextRange, UNO_QUERY );
    // non-Writer apps need not support Property TextField, so test first
    if (xPropSet->getPropertySetInfo()->hasPropertyByName( sTextField ))
    {
        Any aAny = xPropSet->getPropertyValue( sTextField );

        Reference < XTextField > xTxtFld;
        aAny >>= xTxtFld;
        DBG_ASSERT( xTxtFld.is(), "text field missing" );
        if( xTxtFld.is() )
        {
            if( bAutoStyles )
            {
                pFieldExport->ExportFieldAutoStyle( xTxtFld );
            }
            else
            {
                pFieldExport->ExportField( xTxtFld );
            }
        }
        else
        {
            // write only characters
            GetExport().GetDocHandler()->characters(rTextRange->getString());
        }

    }
}

void XMLTextParagraphExport::exportTextMark(
    const Reference<XPropertySet> & rPropSet,
    const OUString sProperty,
    const sal_Char* pElements[],
    sal_Bool bAutoStyles)
{
    // mib said: "Hau wech!"
    //
    // (Originally, I'd export a span element in case the (book|reference)mark
    //  was formatted. This actually makes a difference in case some pervert
    //  sets a point reference mark in the document and, say, formats it bold.
    //  This basically meaningless formatting will now been thrown away
    //  (aka cleaned up), since mib said: ...                   dvo

     if (!bAutoStyles)
    {
        // get reference (as text content)
        Any aAny;
        aAny = rPropSet->getPropertyValue(sProperty);
        Reference<XTextContent> xRefMark;
        aAny >>= xRefMark;

        // name element
        Reference<XNamed> xName(xRefMark, UNO_QUERY);
        GetExport().AddAttribute(XML_NAMESPACE_TEXT, sXML_name,
                                 xName->getName());

        // start, end, or point-reference?
        sal_Int8 nElement;
        aAny = rPropSet->getPropertyValue(sIsCollapsed);
        if( *(sal_Bool *)aAny.getValue() )
        {
            nElement = 0;
        }
        else
        {
            aAny = rPropSet->getPropertyValue(sIsStart);
            nElement = *(sal_Bool *)aAny.getValue() ? 1 : 2;
        }

        // export element
        DBG_ASSERT(pElements != NULL, "illegal element array");
        DBG_ASSERT(nElement >= 0, "illegal element number");
        DBG_ASSERT(nElement <= 2, "illegal element number");
        SvXMLElementExport aElem(GetExport(),
                                 XML_NAMESPACE_TEXT, pElements[nElement],
                                 sal_False, sal_False);
    }
    // else: no styles. (see above)
}


sal_Int32 XMLTextParagraphExport::addTextFrameAttributes(
    const Reference < XPropertySet >& rPropSet,
    sal_Bool bShape )
{
    sal_Int32 nShapeFeatures = SEF_DEFAULT;

    // draw:name
    Reference < XNamed > xNamed( rPropSet, UNO_QUERY );
    if( xNamed.is() )
    {
        OUString sName( xNamed->getName() );
        if( sName.getLength() )
            GetExport().AddAttribute( XML_NAMESPACE_DRAW, sXML_name,
                                      xNamed->getName() );
    }

    Any aAny;
    OUStringBuffer sValue;

    // text:anchor-type
    aAny = rPropSet->getPropertyValue( sAnchorType );
    TextContentAnchorType eAnchor = TextContentAnchorType_AT_PARAGRAPH;
    aAny >>= eAnchor;
    {
        XMLAnchorTypePropHdl aAnchorTypeHdl;
        OUString sTmp;
        aAnchorTypeHdl.exportXML( sTmp, aAny,
                                  GetExport().GetMM100UnitConverter() );
        GetExport().AddAttribute( XML_NAMESPACE_TEXT, sXML_anchor_type, sTmp );
    }

    // text:anchor-page-number
    if( TextContentAnchorType_AT_PAGE == eAnchor )
    {
        aAny = rPropSet->getPropertyValue( sAnchorPageNo );
        sal_Int16 nPage;
        aAny >>= nPage;
        GetExport().GetMM100UnitConverter().convertNumber( sValue,
                                                           (sal_Int32)nPage );
        GetExport().AddAttribute( XML_NAMESPACE_TEXT, sXML_anchor_page_number,
                                  sValue.makeStringAndClear() );
    }

    if( !bShape )
    {
        // svg:x
        sal_Int16 nHoriOrient =  HoriOrientation::NONE;
        aAny = rPropSet->getPropertyValue( sHoriOrient );
        aAny >>= nHoriOrient;
        if( HoriOrientation::NONE == nHoriOrient )
        {
            sal_Int32 nPos = 0;
            Any aAny = rPropSet->getPropertyValue( sHoriOrientPosition );
            aAny >>= nPos;
            GetExport().GetMM100UnitConverter().convertMeasure( sValue, nPos );
            GetExport().AddAttribute( XML_NAMESPACE_SVG, sXML_x,
                                      sValue.makeStringAndClear() );
        }
    }
    else if( TextContentAnchorType_AS_CHARACTER == eAnchor )
        nShapeFeatures = (nShapeFeatures & ~SEF_EXPORT_X);

    if( !bShape || TextContentAnchorType_AS_CHARACTER == eAnchor  )
    {
        // svg:y
        sal_Int16 nVertOrient =  VertOrientation::NONE;
        aAny = rPropSet->getPropertyValue( sVertOrient );
        aAny >>= nVertOrient;
        if( VertOrientation::NONE == nVertOrient )
        {
            sal_Int32 nPos = 0;
            Any aAny = rPropSet->getPropertyValue( sVertOrientPosition );
            aAny >>= nPos;
            GetExport().GetMM100UnitConverter().convertMeasure( sValue, nPos );
            GetExport().AddAttribute( XML_NAMESPACE_SVG, sXML_y,
                                      sValue.makeStringAndClear() );
        }
        if( bShape )
            nShapeFeatures = (nShapeFeatures & ~SEF_EXPORT_Y);
    }


    Reference< XPropertySetInfo > xPropSetInfo = rPropSet->getPropertySetInfo();

    // svg:width
    if( xPropSetInfo->hasPropertyByName( sWidth ) )
    {
        sal_Int32 nWidth =  0;
        aAny = rPropSet->getPropertyValue( sWidth );
        aAny >>= nWidth;
        GetExport().GetMM100UnitConverter().convertMeasure( sValue, nWidth );
        GetExport().AddAttribute( XML_NAMESPACE_SVG, sXML_width,
                                  sValue.makeStringAndClear() );
    }
    sal_Bool bSyncWidth = sal_False;
    if( xPropSetInfo->hasPropertyByName( sIsSyncWidthToHeight ) )
    {
        aAny = rPropSet->getPropertyValue( sIsSyncWidthToHeight );
        bSyncWidth = *(sal_Bool *)aAny.getValue();
        if( bSyncWidth )
            GetExport().AddAttributeASCII( XML_NAMESPACE_STYLE, sXML_rel_width,
                                             sXML_scale );
    }
    if( !bSyncWidth && xPropSetInfo->hasPropertyByName( sRelativeWidth ) )
    {
        sal_Int8 nRelWidth =  0;
        aAny = rPropSet->getPropertyValue( sRelativeWidth );
        aAny >>= nRelWidth;
        DBG_ASSERT( nRelWidth >= 0 && nRelWidth <= 100,
                    "Got illegal relative width from API" );
        if( nRelWidth > 0 )
        {
            GetExport().GetMM100UnitConverter().convertPercent( sValue,
                                                                nRelWidth );
            GetExport().AddAttribute( XML_NAMESPACE_STYLE, sXML_rel_width,
                                      sValue.makeStringAndClear() );
        }
    }

    // svg:height, fo:min-height or style:rel-height
    sal_Int16 nSizeType = SizeType::FIX;
    if( xPropSetInfo->hasPropertyByName( sSizeType ) )
    {
        aAny = rPropSet->getPropertyValue( sSizeType );
        aAny >>= nSizeType;
    }
    sal_Bool bSyncHeight = sal_False;
    if( xPropSetInfo->hasPropertyByName( sIsSyncHeightToWidth ) )
    {
        aAny = rPropSet->getPropertyValue( sIsSyncHeightToWidth );
        bSyncHeight = *(sal_Bool *)aAny.getValue();
    }
    sal_Int8 nRelHeight =  0;
    if( !bSyncHeight && xPropSetInfo->hasPropertyByName( sRelativeHeight ) )
    {
        aAny = rPropSet->getPropertyValue( sRelativeHeight );
        aAny >>= nRelHeight;
    }
    if( xPropSetInfo->hasPropertyByName( sHeight ) )
    {
        sal_Int32 nHeight =  0;
        aAny = rPropSet->getPropertyValue( sHeight );
        aAny >>= nHeight;
        GetExport().GetMM100UnitConverter().convertMeasure( sValue,
                                                            nHeight );
        if( SizeType::MIN == nSizeType && 0==nRelHeight && !bSyncHeight )
            GetExport().AddAttribute( XML_NAMESPACE_FO, sXML_min_height,
                                      sValue.makeStringAndClear() );
        else
            GetExport().AddAttribute( XML_NAMESPACE_SVG, sXML_height,
                                      sValue.makeStringAndClear() );
    }
    if( bSyncHeight )
    {
        GetExport().AddAttributeASCII( XML_NAMESPACE_STYLE, sXML_rel_height,
                SizeType::MIN == nSizeType ? sXML_scale_min : sXML_scale );

    }
    else if( nRelHeight > 0 )
    {
        GetExport().GetMM100UnitConverter().convertPercent( sValue,
                                                            nRelHeight );
        if( SizeType::MIN == nSizeType )
            GetExport().AddAttribute( XML_NAMESPACE_FO, sXML_min_height,
                                      sValue.makeStringAndClear() );
        else
            GetExport().AddAttribute( XML_NAMESPACE_STYLE, sXML_rel_height,
                                      sValue.makeStringAndClear() );
    }

    OUString sZOrder( RTL_CONSTASCII_USTRINGPARAM( "ZOrder" ) );
    if( xPropSetInfo->hasPropertyByName( sZOrder ) )
    {
        sal_Int32 nZIndex;
        aAny = rPropSet->getPropertyValue( sZOrder );
        aAny >>= nZIndex;
        if( -1 != nZIndex )
        {
            GetExport().GetMM100UnitConverter().convertNumber( sValue,
                                                                nZIndex );
            GetExport().AddAttribute( XML_NAMESPACE_DRAW, sXML_zindex,
                                      sValue.makeStringAndClear() );
        }
    }

    return nShapeFeatures;
}

void XMLTextParagraphExport::_exportTextFrame(
        const Reference < XPropertySet > & rPropSet,
        const Reference < XPropertySetInfo > & rPropSetInfo,
        sal_Bool bProgress )
{
    Reference < XTextFrame > xTxtFrame( rPropSet, UNO_QUERY );
    Reference < XText > xTxt = xTxtFrame->getText();

    OUString sStyle;
    Any aAny;
    if( rPropSetInfo->hasPropertyByName( sFrameStyleName ) )
    {
        aAny = rPropSet->getPropertyValue( sFrameStyleName );
        aAny >>= sStyle;
    }

    OUString sAutoStyle( sStyle );
    sAutoStyle = Find( XML_STYLE_FAMILY_TEXT_FRAME, rPropSet, sStyle );
    if( sAutoStyle.getLength() )
        GetExport().AddAttribute( XML_NAMESPACE_DRAW, sXML_style_name,
                                  sAutoStyle );
    addTextFrameAttributes( rPropSet, sal_False );

    // draw:chain-next-name
    if( rPropSetInfo->hasPropertyByName( sChainNextName ) )
    {
        OUString sNext;
        aAny = rPropSet->getPropertyValue( sChainNextName );
        if( (aAny >>= sNext) && sNext.getLength() > 0 )
            GetExport().AddAttribute( XML_NAMESPACE_DRAW,
                                      sXML_chain_next_name,
                                      sNext );
    }

    SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_DRAW,
                              sXML_text_box, sal_False, sal_True );

    // frame bound frames
    exportFramesBoundToFrame( xTxtFrame, bProgress );

    // script:events
    Reference<XEventsSupplier> xEventsSupp( xTxtFrame, UNO_QUERY );
    GetExport().GetEventExport().Export(xEventsSupp);

    exportText( xTxt, sal_False, bProgress, sal_True );
}

void XMLTextParagraphExport::exportTextFrame(
        const Reference < XTextContent > & rTxtCntnt,
        sal_Bool bAutoStyles,
        sal_Bool bProgress )
{
    Reference < XPropertySet > xPropSet( rTxtCntnt, UNO_QUERY );

    if( bAutoStyles )
    {
        Reference < XTextFrame > xTxtFrame( rTxtCntnt, UNO_QUERY );
        Reference < XText > xTxt = xTxtFrame->getText();
        Add( XML_STYLE_FAMILY_TEXT_FRAME, xPropSet );
        exportText( xTxt, bAutoStyles, bProgress, sal_True );
    }
    else
    {
        Reference< XPropertySetInfo > xPropSetInfo =
            xPropSet->getPropertySetInfo();
        Reference< XPropertyState > xPropState( xPropSet, UNO_QUERY );
        if( addHyperlinkAttributes( xPropSet, xPropState, xPropSetInfo ) )
        {
            SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_DRAW,
                                      sXML_a, sal_False, sal_False );
            _exportTextFrame( xPropSet, xPropSetInfo, bProgress );
        }
        else
        {
            _exportTextFrame( xPropSet, xPropSetInfo, bProgress );
        }
    }
}

void XMLTextParagraphExport::exportContour(
        const Reference < XPropertySet > & rPropSet,
        const Reference < XPropertySetInfo > & rPropSetInfo )
{
    if( !rPropSetInfo->hasPropertyByName( sContourPolyPolygon ) )
        return;

    Any aAny = rPropSet->getPropertyValue( sContourPolyPolygon );

    PointSequenceSequence aSourcePolyPolygon;
    aAny >>= aSourcePolyPolygon;

    if( !aSourcePolyPolygon.getLength() )
        return;

    awt::Point aPoint( 0, 0 );
    awt::Size aSize;
    aAny = rPropSet->getPropertyValue( sActualSize );
    aAny >>= aSize;

    // svg: width
    OUStringBuffer aStringBuffer( 10 );
    GetExport().GetMM100UnitConverter().convertMeasure(aStringBuffer, aSize.Width);
    GetExport().AddAttribute( XML_NAMESPACE_SVG, sXML_width,
                              aStringBuffer.makeStringAndClear() );

    // svg: height
    GetExport().GetMM100UnitConverter().convertMeasure(aStringBuffer, aSize.Height);
    GetExport().AddAttribute( XML_NAMESPACE_SVG, sXML_height,
                              aStringBuffer.makeStringAndClear() );

    // svg:viewbox
    SdXMLImExViewBox aViewBox(0, 0, aSize.Width, aSize.Height);
    GetExport().AddAttribute(XML_NAMESPACE_SVG, sXML_viewBox,
                aViewBox.GetExportString(GetExport().GetMM100UnitConverter()));

    sal_Int32 nOuterCnt( aSourcePolyPolygon.getLength() );

    const sal_Char *pElem = 0;
    if( 1L == nOuterCnt )
    {
        // simple polygon shape, can be written as svg:points sequence
        /*const*/ PointSequence* pSequence =
                            (PointSequence*)aSourcePolyPolygon.getConstArray();

        SdXMLImExPointsElement aPoints( pSequence, aViewBox, aPoint,
                                aSize, GetExport().GetMM100UnitConverter() );

        // write point array
        GetExport().AddAttribute( XML_NAMESPACE_DRAW, sXML_points,
                                      aPoints.GetExportString());
        pElem = sXML_contour_polygon;
    }
    else
    {
        // polypolygon, needs to be written as a svg:path sequence
        /*const*/ PointSequence* pOuterSequence =
                        (PointSequence*)aSourcePolyPolygon.getConstArray();
        if(pOuterSequence)
        {
            // prepare svx:d element export
            SdXMLImExSvgDElement aSvgDElement( aViewBox );

            for(sal_Int32 a(0L); a < nOuterCnt; a++)
            {
                /*const*/ PointSequence* pSequence = pOuterSequence++;
                if(pSequence)
                {
                    aSvgDElement.AddPolygon(pSequence, 0L, aPoint,
                        aSize, GetExport().GetMM100UnitConverter(),
                        sal_True );
                }
            }

            // write point array
            GetExport().AddAttribute( XML_NAMESPACE_SVG, sXML_d,
                                      aSvgDElement.GetExportString());
            pElem = sXML_contour_path;
        }
    }

    // write object now
    SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_DRAW, pElem,
                              sal_True, sal_True );
}

void XMLTextParagraphExport::_exportTextGraphic(
        const Reference < XPropertySet > & rPropSet,
        const Reference < XPropertySetInfo > & rPropSetInfo )
{
    OUString sStyle;
    Any aAny;
    if( rPropSetInfo->hasPropertyByName( sFrameStyleName ) )
    {
        aAny = rPropSet->getPropertyValue( sFrameStyleName );
        aAny >>= sStyle;
    }

    OUString sAutoStyle( sStyle );
    sAutoStyle = Find( XML_STYLE_FAMILY_TEXT_FRAME, rPropSet, sStyle );
    if( sAutoStyle.getLength() )
        GetExport().AddAttribute( XML_NAMESPACE_DRAW, sXML_style_name,
                                  sAutoStyle );
    addTextFrameAttributes( rPropSet, sal_False );

    // xlink:href
    OUString sURL;
    aAny = rPropSet->getPropertyValue( sGraphicURL );
    aAny >>= sURL;
    sURL = GetExport().AddEmbeddedGraphicObject( sURL );
    setTextEmbeddedGraphicURL( rPropSet, sURL );

    // If there still is no url, then teh graphic is empty
    if( sURL.getLength() )
    {
        GetExport().AddAttribute(XML_NAMESPACE_XLINK, sXML_href, sURL );
        GetExport().AddAttributeASCII( XML_NAMESPACE_XLINK, sXML_type,
                                       sXML_simple );
        GetExport().AddAttributeASCII( XML_NAMESPACE_XLINK, sXML_show,
                                       sXML_embed );
        GetExport().AddAttributeASCII( XML_NAMESPACE_XLINK, sXML_actuate,
                                       sXML_onLoad );
    }

    // draw:filter-name
    OUString sGrfFilter;
    aAny = rPropSet->getPropertyValue( sGraphicFilter );
    aAny >>= sGrfFilter;
    if( sGrfFilter.getLength() )
        GetExport().AddAttribute( XML_NAMESPACE_DRAW, sXML_filter_name,
                                  sGrfFilter );

    // svg:transform
    aAny = rPropSet->getPropertyValue( sGraphicRotation );
    sal_Int16 nVal;
    aAny >>= nVal;
    if( nVal != 0 )
    {
        OUStringBuffer sRet( sizeof(sXML_rotate)+4 );
        sRet.appendAscii(sXML_rotate);
        sRet.append( (sal_Unicode)'(' );
        GetExport().GetMM100UnitConverter().convertNumber( sRet, (sal_Int32)nVal );
        sRet.append( (sal_Unicode)')' );
        GetExport().AddAttribute( XML_NAMESPACE_SVG, sXML_transform,
                                  sRet.makeStringAndClear() );
    }

    SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_DRAW,
                              sXML_image, sal_False, sal_True );

    // script:events
    Reference<XEventsSupplier> xEventsSupp( rPropSet, UNO_QUERY );
    GetExport().GetEventExport().Export(xEventsSupp);

    // svg:desc
    exportAlternativeText( rPropSet, rPropSetInfo );
    /*
    OUString sAltText;
    aAny = rPropSet->getPropertyValue( sAlternativeText );
    aAny >>= sAltText;
    if( sAltText.getLength() )
    {
        SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_SVG,
                                  sXML_desc, sal_True, sal_False );
        GetExport().GetDocHandler()->characters( sAltText );
    }
    */

    // draw:contour
    exportContour( rPropSet, rPropSetInfo );
}

void XMLTextParagraphExport::exportTextGraphic(
        const Reference < XTextContent > & rTextContent,
        sal_Bool bAutoStyles )
{
    Reference < XPropertySet > xPropSet( rTextContent, UNO_QUERY );

    if( bAutoStyles )
    {
        Add( XML_STYLE_FAMILY_TEXT_FRAME, xPropSet );
    }
    else
    {
        Reference< XPropertySetInfo > xPropSetInfo =
            xPropSet->getPropertySetInfo();
        Reference< XPropertyState > xPropState( xPropSet, UNO_QUERY );
        if( addHyperlinkAttributes( xPropSet, xPropState, xPropSetInfo ) )
        {
            SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_DRAW,
                                      sXML_a, sal_False, sal_False );
            _exportTextGraphic( xPropSet, xPropSetInfo );
        }
        else
        {
            _exportTextGraphic( xPropSet, xPropSetInfo );
        }
    }
}

void XMLTextParagraphExport::exportShape(
        const Reference < XTextContent > & rTxtCntnt,
        sal_Bool bAutoStyles )
{
    Reference < XShape > xShape( rTxtCntnt, UNO_QUERY );

    if( bAutoStyles )
    {
        GetExport().GetShapeExport()->collectShapeAutoStyles( xShape );
    }
    else
    {
        Reference < XPropertySet > xPropSet( rTxtCntnt, UNO_QUERY );
        sal_Int32 nFeatures = addTextFrameAttributes( xPropSet, sal_True );
        GetExport().GetShapeExport()->exportShape( xShape, nFeatures );
    }
}

void XMLTextParagraphExport::_collectTextEmbeddedAutoStyles(
        const Reference < XPropertySet > & rPropSet )
{
    Add( XML_STYLE_FAMILY_TEXT_FRAME, rPropSet );
}

void XMLTextParagraphExport::_exportTextEmbedded(
        const Reference < XPropertySet > & rPropSet,
        const Reference < XPropertySetInfo > & rPropSetInfo )
{
    OUString sStyle;
    Any aAny;

    if( rPropSetInfo->hasPropertyByName( sFrameStyleName ) )
    {
        aAny = rPropSet->getPropertyValue( sFrameStyleName );
        aAny >>= sStyle;
    }

    OUString sAutoStyle( sStyle );
    sAutoStyle = Find( XML_STYLE_FAMILY_TEXT_FRAME, rPropSet, sStyle );
    if( sAutoStyle.getLength() )
        GetExport().AddAttribute( XML_NAMESPACE_DRAW, sXML_style_name,
                                  sAutoStyle );
    addTextFrameAttributes( rPropSet, sal_False );


    // xlink:href
    OUString sURL;
    sal_Bool bExtern;
    getTextEmbeddedObjectProperties( rPropSet, sURL, bExtern );

    sURL = GetExport().AddEmbeddedObject( sURL );

    GetExport().AddAttribute(XML_NAMESPACE_XLINK, sXML_href, sURL );
    GetExport().AddAttributeASCII( XML_NAMESPACE_XLINK, sXML_type,
                                   sXML_simple );
    GetExport().AddAttributeASCII( XML_NAMESPACE_XLINK, sXML_show,
                                   sXML_embed );
    GetExport().AddAttributeASCII( XML_NAMESPACE_XLINK, sXML_actuate,
                                   sXML_onLoad );

    const sal_Char *pElem = bExtern ? sXML_object_ole : sXML_object;
    SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_DRAW,
                              pElem, sal_False, sal_True );

    exportEvents( rPropSet );
    exportAlternativeText( rPropSet, rPropSetInfo );
    // draw:contour
    exportContour( rPropSet, rPropSetInfo );

}

void XMLTextParagraphExport::exportEvents( const Reference < XPropertySet > & rPropSet )
{
    // script:events
    Reference<XEventsSupplier> xEventsSupp( rPropSet, UNO_QUERY );
    GetExport().GetEventExport().Export(xEventsSupp);
}
void XMLTextParagraphExport::exportAlternativeText(
        const Reference < XPropertySet > & rPropSet,
        const Reference < XPropertySetInfo > & rPropSetInfo )
{
    // svg:desc
    if( rPropSetInfo->hasPropertyByName( sAlternativeText  ) )
    {
        OUString sAltText;
        Any aAny = rPropSet->getPropertyValue( sAlternativeText );
        aAny >>= sAltText;
        if( sAltText.getLength() )
        {
            SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_SVG,
                                      sXML_desc, sal_True, sal_False );
            GetExport().GetDocHandler()->characters( sAltText );
        }
    }
}
void XMLTextParagraphExport::exportTextEmbedded(
        const Reference < XTextContent > & rTextContent,
        sal_Bool bAutoStyles )
{
    Reference < XPropertySet > xPropSet( rTextContent, UNO_QUERY );

    if( bAutoStyles )
    {
        _collectTextEmbeddedAutoStyles( xPropSet );
    }
    else
    {
        Reference< XPropertySetInfo > xPropSetInfo =
            xPropSet->getPropertySetInfo();
        Reference< XPropertyState > xPropState( xPropSet, UNO_QUERY );
        if( addHyperlinkAttributes( xPropSet, xPropState, xPropSetInfo ) )
        {
            SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_DRAW,
                                      sXML_a, sal_False, sal_False );
            _exportTextEmbedded( xPropSet, xPropSetInfo );
        }
        else
        {
            _exportTextEmbedded( xPropSet, xPropSetInfo );
        }
    }
}

void XMLTextParagraphExport::getTextEmbeddedObjectProperties(
    const Reference < XPropertySet >& rPropSet,
    OUString& rStreamName, sal_Bool& r ) const
{
}

void XMLTextParagraphExport::setTextEmbeddedGraphicURL(
    const Reference < XPropertySet >& rPropSet,
    OUString& rStreamName ) const
{
}

void XMLTextParagraphExport::_exportTextRange(
        const Reference < XTextRange > & rTextRange,
        const Reference < XPropertySet > & rPropSet,
        sal_Bool& rPrevCharIsSpace )
{
    OUString sStyle = FindTextStyle( rPropSet );
    OUString sText = rTextRange->getString();
    if( sStyle.getLength() )
    {
        GetExport().AddAttribute( XML_NAMESPACE_TEXT, sXML_style_name,
                                  sStyle );
        SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_TEXT,
                                  sXML_span, sal_False, sal_False );
        exportText( sText, rPrevCharIsSpace );
    }
    else
    {
        exportText( sText, rPrevCharIsSpace );
    }
}

sal_Bool XMLTextParagraphExport::addHyperlinkAttributes(
        const Reference < XPropertySet > & rPropSet,
        const Reference < XPropertyState > & rPropState,
        const Reference < XPropertySetInfo > & rPropSetInfo )
{
    sal_Bool bExport = sal_False;
    Any aAny;
    OUString sHRef, sName, sTargetFrame, sUStyleName, sVStyleName;
    sal_Bool bServerMap = sal_False;

    if( rPropSetInfo->hasPropertyByName( sHyperLinkURL ) &&
        ( !rPropState.is() || PropertyState_DIRECT_VALUE ==
                    rPropState->getPropertyState( sHyperLinkURL ) ) )
    {
        aAny= rPropSet->getPropertyValue( sHyperLinkURL );
        aAny >>= sHRef;

        if( sHRef.getLength() > 0 )
            bExport = sal_True;
    }

    if( rPropSetInfo->hasPropertyByName( sHyperLinkName ) &&
        ( !rPropState.is() || PropertyState_DIRECT_VALUE ==
                    rPropState->getPropertyState( sHyperLinkName ) ) )
    {
        aAny = rPropSet->getPropertyValue( sHyperLinkName );
        aAny >>= sName;
        if( sName.getLength() > 0 )
            bExport = sal_True;
    }

    if( rPropSetInfo->hasPropertyByName( sHyperLinkTarget ) &&
        ( !rPropState.is() || PropertyState_DIRECT_VALUE ==
                    rPropState->getPropertyState( sHyperLinkTarget ) ) )
    {
        aAny = rPropSet->getPropertyValue( sHyperLinkTarget );
        aAny >>= sTargetFrame;
        if( sTargetFrame.getLength() )
            bExport = sal_True;
    }

    if( rPropSetInfo->hasPropertyByName( sServerMap ) &&
        ( !rPropState.is() || PropertyState_DIRECT_VALUE ==
                    rPropState->getPropertyState( sServerMap ) ) )
    {
        aAny = rPropSet->getPropertyValue( sServerMap );
        bServerMap = *(sal_Bool *)aAny.getValue();
        if( bServerMap  )
            bExport = sal_True;
    }

    if( rPropSetInfo->hasPropertyByName( sUnvisitedCharStyleName ) &&
        ( !rPropState.is() || PropertyState_DIRECT_VALUE ==
            rPropState->getPropertyState( sUnvisitedCharStyleName ) ) )
    {
        aAny = rPropSet->getPropertyValue( sUnvisitedCharStyleName );
        aAny >>= sUStyleName;
        if( sUStyleName.getLength() )
            bExport = sal_True;
    }

    if( rPropSetInfo->hasPropertyByName( sVisitedCharStyleName ) &&
        ( !rPropState.is() || PropertyState_DIRECT_VALUE ==
            rPropState->getPropertyState( sVisitedCharStyleName ) ) )
    {
        aAny = rPropSet->getPropertyValue( sVisitedCharStyleName );
        aAny >>= sVStyleName;
        if( sVStyleName.getLength() )
            bExport = sal_True;
    }

    if( bExport )
    {
        GetExport().AddAttributeASCII( XML_NAMESPACE_XLINK, sXML_type,
                                       sXML_simple );
        GetExport().AddAttribute( XML_NAMESPACE_XLINK, sXML_href, sHRef );

        if( sName.getLength() > 0 )
            GetExport().AddAttribute( XML_NAMESPACE_OFFICE, sXML_name, sName );

        if( sTargetFrame.getLength() )
        {
            GetExport().AddAttribute( XML_NAMESPACE_OFFICE,
                                      sXML_target_frame_name, sTargetFrame );
            const sal_Char *pStr =
                sTargetFrame.equalsAsciiL( "_blank", sizeof("_blank")-1 )
                    ? sXML_new : sXML_replace;
            GetExport().AddAttributeASCII( XML_NAMESPACE_XLINK,
                                      sXML_show, pStr );
        }

        if( bServerMap  )
            GetExport().AddAttributeASCII( XML_NAMESPACE_OFFICE,
                                           sXML_server_map,
                                             sXML_true );

        if( sUStyleName.getLength() )
            GetExport().AddAttribute( XML_NAMESPACE_TEXT,
                                      sXML_style_name, sUStyleName );

        if( sVStyleName.getLength() )
            GetExport().AddAttribute( XML_NAMESPACE_TEXT,
                                      sXML_visited_style_name, sVStyleName );
    }

    return bExport;
}

void XMLTextParagraphExport::exportTextRange(
        const Reference < XTextRange > & rTextRange,
        sal_Bool bAutoStyles,
        sal_Bool& rPrevCharIsSpace )
{
    Reference < XPropertySet > xPropSet( rTextRange, UNO_QUERY );
    if( bAutoStyles )
    {
        Add( XML_STYLE_FAMILY_TEXT_TEXT, xPropSet );
    }
    else
    {
        Reference< XPropertyState > xPropState( xPropSet, UNO_QUERY );
        Reference < XPropertySetInfo > xPropSetInfo =
            xPropSet->getPropertySetInfo();
        if( xPropState.is() &&
            xPropSetInfo->hasPropertyByName( sHyperLinkURL ) &&
            PropertyState_DIRECT_VALUE ==
                                xPropState->getPropertyState( sHyperLinkURL ) )
        {
            addHyperlinkAttributes( xPropSet, xPropState, xPropSetInfo );
            SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_TEXT,
                                      sXML_a, sal_False, sal_False );

            // export events (if supported)
            OUString sHyperLinkEvents(RTL_CONSTASCII_USTRINGPARAM(
                "HyperLinkEvents"));
            if (xPropSetInfo->hasPropertyByName(sHyperLinkEvents))
            {
                Any aAny = xPropSet->getPropertyValue(sHyperLinkEvents);
                Reference<XNameReplace> xName;
                aAny >>= xName;
                GetExport().GetEventExport().Export(xName, sal_False);
            }

            _exportTextRange( rTextRange, xPropSet, rPrevCharIsSpace );
        }
        else
        {
            _exportTextRange( rTextRange, xPropSet, rPrevCharIsSpace );
        }
    }
}

void XMLTextParagraphExport::exportText( const OUString& rText,
                                           sal_Bool& rPrevCharIsSpace )
{
    sal_Int32 nExpStartPos = 0L;
    sal_Int32 nEndPos = rText.getLength();
    sal_Int32 nSpaceChars = 0;
    for( sal_Int32 nPos = 0; nPos < nEndPos; nPos++ )
    {
        sal_Unicode cChar = rText[nPos];
        sal_Bool bExpCharAsText = sal_True;
        sal_Bool bExpCharAsElement = sal_False;
        sal_Bool bCurrCharIsSpace = sal_False;
        switch( cChar )
        {
        case 0x0009:    // Tab
        case 0x000A:    // LF
            // These characters are exported as text.
            bExpCharAsElement = sal_True;
            bExpCharAsText = sal_False;
            break;
        case 0x0020:    // Blank
            if( rPrevCharIsSpace )
            {
                // If the previous character is a space character,
                // too, export a special space element.
                bExpCharAsText = sal_False;
            }
            bCurrCharIsSpace = sal_True;
            break;
        }

        // If the current character is not exported as text
           // the text that has not been exported by now has to be exported now.
        if( nPos > nExpStartPos && !bExpCharAsText )
        {
            DBG_ASSERT( 0==nSpaceChars, "pending spaces" );
            OUString sExp( rText.copy( nExpStartPos, nPos - nExpStartPos ) );
            GetExport().GetDocHandler()->characters( sExp );
            nExpStartPos = nPos;
        }

        // If there are spaces left that have not been exported and the
        // current chracter is not a space , the pending spaces have to be
        // exported now.
        if( nSpaceChars > 0 && !bCurrCharIsSpace )
        {
            DBG_ASSERT( nExpStartPos == nPos, " pending characters" );

            if( nSpaceChars > 1 )
            {
                OUStringBuffer sTmp;
                sTmp.append( (sal_Int32)nSpaceChars );
                GetExport().AddAttribute( XML_NAMESPACE_TEXT, sXML_c,
                              sTmp.makeStringAndClear() );
            }

            SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_TEXT,
                                      sXML_s, sal_False, sal_False );

            nSpaceChars = 0;
        }

        // If the current character has to be exported as a special
        // element, the elemnt will be exported now.
        if( bExpCharAsElement )
        {
            switch( cChar )
            {
            case 0x0009:    // Tab
                {
                    SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_TEXT,
                                              sXML_tab_stop, sal_False,
                                              sal_False );
                }
                break;
            case 0x000A:    // LF
                {
                    SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_TEXT,
                                              sXML_line_break, sal_False,
                                              sal_False );
                }
                break;
            }
        }

        // If the current character is a space, and the previous one
        // is a space, too, the number of pending spaces is incremented
        // only.
        if( bCurrCharIsSpace && rPrevCharIsSpace )
            nSpaceChars++;
        rPrevCharIsSpace = bCurrCharIsSpace;

        // If the currect character is not exported as text, the start
        // position for text is the position behind the current position.
        if( !bExpCharAsText )
        {
            DBG_ASSERT( nExpStartPos == nPos, "wrong export start pos" );
            nExpStartPos = nPos+1;
        }
    }

    if( nExpStartPos < nEndPos )
    {
        DBG_ASSERT( 0==nSpaceChars, " pending spaces " );
        OUString sExp( rText.copy( nExpStartPos, nEndPos - nExpStartPos ) );
        GetExport().GetDocHandler()->characters( sExp );
    }

    // If there are some spaces left, they have to be exported now.
    if( nSpaceChars > 0 )
    {
        if( nSpaceChars > 1 )
        {
            OUStringBuffer sTmp;
            sTmp.append( (sal_Int32)nSpaceChars );
            GetExport().AddAttribute( XML_NAMESPACE_TEXT, sXML_c,
                          sTmp.makeStringAndClear() );
        }

        SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_TEXT, sXML_s,
                                  sal_False, sal_False );
    }
}

void XMLTextParagraphExport::exportTextDeclarations()
{
    pFieldExport->ExportFieldDeclarations();
}

void XMLTextParagraphExport::exportTextDeclarations(
    const Reference<XText> & rText )
{
    pFieldExport->ExportFieldDeclarations(rText);
}

void XMLTextParagraphExport::exportUsedDeclarations( sal_Bool bOnlyUsed )
{
    pFieldExport->SetExportOnlyUsedFieldDeclarations( bOnlyUsed );
}


void XMLTextParagraphExport::exportTrackedChanges(sal_Bool bAutoStyles)
{
    if (NULL != pRedlineExport)
        pRedlineExport->ExportChangesList( bAutoStyles );
}

void XMLTextParagraphExport::exportTrackedChanges(
    const Reference<XText> & rText,
    sal_Bool bAutoStyle)
{
    if (NULL != pRedlineExport)
        pRedlineExport->ExportChangesList(rText, bAutoStyle);
}

void XMLTextParagraphExport::recordTrackedChangesForXText(
    const Reference<XText> & rText )
{
    if (NULL != pRedlineExport)
        pRedlineExport->SetCurrentXText(rText);
}

void XMLTextParagraphExport::recordTrackedChangesNoXText()
{
    if (NULL != pRedlineExport)
        pRedlineExport->SetCurrentXText();
}


void XMLTextParagraphExport::exportTextAutoStyles()
{
    GetAutoStylePool().exportXML( XML_STYLE_FAMILY_TEXT_PARAGRAPH,
                                   GetExport().GetDocHandler(),
                                   GetExport().GetMM100UnitConverter(),
                                   GetExport().GetNamespaceMap() );

    GetAutoStylePool().exportXML( XML_STYLE_FAMILY_TEXT_TEXT,
                                   GetExport().GetDocHandler(),
                                   GetExport().GetMM100UnitConverter(),
                                   GetExport().GetNamespaceMap() );

    GetAutoStylePool().exportXML( XML_STYLE_FAMILY_TEXT_FRAME,
                                   GetExport().GetDocHandler(),
                                   GetExport().GetMM100UnitConverter(),
                                   GetExport().GetNamespaceMap() );

    GetAutoStylePool().exportXML( XML_STYLE_FAMILY_TEXT_SECTION,
                                  GetExport().GetDocHandler(),
                                  GetExport().GetMM100UnitConverter(),
                                  GetExport().GetNamespaceMap() );

    GetAutoStylePool().exportXML( XML_STYLE_FAMILY_TEXT_RUBY,
                                  GetExport().GetDocHandler(),
                                  GetExport().GetMM100UnitConverter(),
                                  GetExport().GetNamespaceMap() );
    pListAutoPool->exportXML();
}

void XMLTextParagraphExport::exportRuby(
    const Reference<XPropertySet> & rPropSet,
    sal_Bool bAutoStyles )
{
    // early out: a collapsed ruby makes no sense
    Any aAny = rPropSet->getPropertyValue(sIsCollapsed);
    if (*(sal_Bool*)aAny.getValue())
        return;

    // start value ?
    aAny = rPropSet->getPropertyValue(sIsStart);
    sal_Bool bStart = (*(sal_Bool*)aAny.getValue());

    if (bAutoStyles)
    {
        // ruby auto styles
        if (bStart)
            Add( XML_STYLE_FAMILY_TEXT_RUBY, rPropSet );
    }
    else
    {
        // prepare element names
        OUString sRuby(RTL_CONSTASCII_USTRINGPARAM(sXML_ruby));
        OUString sTextRuby(GetExport().GetNamespaceMap().
                           GetQNameByKey(XML_NAMESPACE_TEXT, sRuby));
        OUString sRubyBase(RTL_CONSTASCII_USTRINGPARAM(sXML_ruby_base));
        OUString sTextRubyBase(GetExport().GetNamespaceMap().
                           GetQNameByKey(XML_NAMESPACE_TEXT, sRubyBase));

        if (bStart)
        {
            // ruby start

            // we can only start a ruby if none is open
            DBG_ASSERT(! bOpenRuby, "Can't open a ruby inside of ruby!");
            if( bOpenRuby )
                return;

            // save ruby text + ruby char style
            aAny = rPropSet->getPropertyValue(sRubyText);
            aAny >>= sOpenRubyText;
            aAny = rPropSet->getPropertyValue(sRubyCharStyleName);
            aAny >>= sOpenRubyCharStyle;

            // ruby style
            GetExport().CheckAttrList();
            OUString sEmpty;
            OUString sStyleName = Find( XML_STYLE_FAMILY_TEXT_RUBY, rPropSet,
                                        sEmpty );
            DBG_ASSERT(sStyleName.getLength() > 0, "I can't find the style!");
            GetExport().AddAttribute(XML_NAMESPACE_TEXT,
                                     sXML_style_name, sStyleName);

            // export <text:ruby> and <text:ruby-base> start elements
            GetExport().GetDocHandler()->startElement(
                sTextRuby, GetExport().GetXAttrList());
            GetExport().ClearAttrList();
            GetExport().GetDocHandler()->startElement(
                sTextRubyBase, GetExport().GetXAttrList());

            bOpenRuby = sal_True;
        }
        else
        {
            // ruby end

            // check for an open ruby
            DBG_ASSERT(bOpenRuby, "Can't close a ruby if none is open!");
            if( !bOpenRuby )
                return;

            // close <text:ruby-base>
            GetExport().GetDocHandler()->endElement(sTextRubyBase);

            // write the ruby text (with char style)
            {
                if (sOpenRubyCharStyle.getLength() > 0)
                    GetExport().AddAttribute(
                        XML_NAMESPACE_TEXT, sXML_style_name,
                        sOpenRubyCharStyle);

                SvXMLElementExport aRuby(
                    GetExport(), XML_NAMESPACE_TEXT, sXML_ruby_text,
                    sal_False, sal_False);

                GetExport().GetDocHandler()->characters(sOpenRubyText);
            }

            // and finally, close the ruby
            GetExport().GetDocHandler()->endElement(sTextRuby);
            bOpenRuby = sal_False;
        }
    }
}
