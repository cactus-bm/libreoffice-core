/*************************************************************************
 *
 *  $RCSfile: txtstyli.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: dvo $ $Date: 2001-01-31 10:18:35 $
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

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_XSTYLE_HPP_
#include <com/sun/star/style/XStyle.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_PARAGRAPHSTYLECATEGORY_HPP_
#include <com/sun/star/style/ParagraphStyleCategory.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XEVENTSSUPPLIER_HPP
#include <com/sun/star/document/XEventsSupplier.hpp>
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_XMLKYWD_HXX
#include "xmlkywd.hxx"
#endif
#ifndef _XMLOFF_FAMILIES_HXX
#include "families.hxx"
#endif
#ifndef _XMLOFF_XMLTEXTPROPERTYSETCONTEXT_HXX
#include "XMLTextPropertySetContext.hxx"
#endif
#ifndef _XMLOFF_XMLTKMAP_HXX
#include "xmltkmap.hxx"
#endif
#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif

#ifndef _XMLOFF_TXTPRMAP_HXX
#include "txtprmap.hxx"
#endif
#ifndef _XMLOFF_TXTSTYLI_HXX
#include "txtstyli.hxx"
#endif
#ifndef _XMLOFF_ATTRLIST_HXX
#include "attrlist.hxx"
#endif

#ifndef _XMLOFF_XMLEVENTSIMPORTCONTEXT_HXX
#include "XMLEventsImportContext.hxx"
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;
using namespace ::com::sun::star::style;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
//using namespace ::com::sun::star::text;

static __FAR_DATA SvXMLEnumMapEntry aCategoryMap[] =
{
    { sXML_text, ParagraphStyleCategory::TEXT },
    { sXML_chapter, ParagraphStyleCategory::CHAPTER },
    { sXML_list, ParagraphStyleCategory::LIST },
    { sXML_index, ParagraphStyleCategory::INDEX },
    { sXML_extra, ParagraphStyleCategory::EXTRA },
    { sXML_html, ParagraphStyleCategory::HTML },
    { 0,          0 }
};

void XMLTextStyleContext::SetAttribute( sal_uInt16 nPrefixKey,
                                        const OUString& rLocalName,
                                        const OUString& rValue )
{
    if( XML_NAMESPACE_STYLE == nPrefixKey )
    {
        // TODO: use a map here
        if( rLocalName.compareToAscii( sXML_auto_update ) == 0 )
        {
            if( rValue.compareToAscii( sXML_true ) == 0 )
                bAutoUpdate = sal_True;
        }
        else if( rLocalName.compareToAscii( sXML_list_style_name ) == 0 )
        {
            sListStyleName = rValue;
        }
        else if( rLocalName.compareToAscii( sXML_master_page_name ) == 0 )
        {
            sMasterPageName = rValue;
            bHasMasterPageName = sal_True;
        }
        else if( rLocalName.compareToAscii( sXML_class ) == 0 )
        {
            sCategoryVal = rValue;
        }
        else
        {
            XMLPropStyleContext::SetAttribute( nPrefixKey, rLocalName, rValue );
        }
    }
    else
    {
        XMLPropStyleContext::SetAttribute( nPrefixKey, rLocalName, rValue );
    }
}

TYPEINIT1( XMLTextStyleContext, XMLPropStyleContext );

XMLTextStyleContext::XMLTextStyleContext( SvXMLImport& rImport,
        sal_uInt16 nPrfx, const OUString& rLName,
        const Reference< XAttributeList > & xAttrList,
        SvXMLStylesContext& rStyles, sal_uInt16 nFamily,
        sal_Bool bDefaultStyle ) :
    XMLPropStyleContext( rImport, nPrfx, rLName, xAttrList, rStyles,
                         nFamily, bDefaultStyle ),
    bAutoUpdate( sal_False ),
    bHasMasterPageName( sal_False ),
    bHasCombinedCharactersLetter( sal_False ),
    pEventContext( NULL ),
    sIsAutoUpdate( RTL_CONSTASCII_USTRINGPARAM( "IsAutoUpdate" ) ),
    sCategory( RTL_CONSTASCII_USTRINGPARAM( "Category" ) ),
    sNumberingStyleName( RTL_CONSTASCII_USTRINGPARAM( "NumberingStyleName" ) ),
    sPageDescName( RTL_CONSTASCII_USTRINGPARAM( "PageDescName" ) ),
    sDropCapCharStyleName( RTL_CONSTASCII_USTRINGPARAM( "DropCapCharStyleName" ) )
{
}

XMLTextStyleContext::~XMLTextStyleContext()
{
}

SvXMLImportContext *XMLTextStyleContext::CreateChildContext(
        sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const Reference< XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = 0;

    if( XML_NAMESPACE_STYLE == nPrefix &&
        rLocalName.compareToAscii( sXML_properties ) == 0 )
    {
        UniReference < SvXMLImportPropertyMapper > xImpPrMap =
            GetStyles()->GetImportPropertyMapper( GetFamily() );
        if( xImpPrMap.is() )
            pContext = new XMLTextPropertySetContext( GetImport(), nPrefix,
                                                    rLocalName, xAttrList,
                                                    GetProperties(),
                                                    xImpPrMap,
                                                    sDropCapTextStyleName );
    }
    else if ( (XML_NAMESPACE_SCRIPT == nPrefix) &&
              rLocalName.equalsAsciiL( sXML_events, sizeof(sXML_events)-1 ) )
    {
        // create and remember events import context
        // (for delayed processing of events)
        pEventContext = new XMLEventsImportContext( GetImport(), nPrefix,
                                                   rLocalName);
        pEventContext->AddRef();
        pContext = pEventContext;
    }

    if( !pContext )
        pContext = XMLPropStyleContext::CreateChildContext( nPrefix, rLocalName,
                                                          xAttrList );

    return pContext;
}

void XMLTextStyleContext::CreateAndInsert( sal_Bool bOverwrite )
{
    XMLPropStyleContext::CreateAndInsert( bOverwrite );
    Reference < XStyle > xStyle = GetStyle();
    if( !xStyle.is() || !(bOverwrite || IsNew()) )
        return;

    Reference < XPropertySet > xPropSet( xStyle, UNO_QUERY );
    Reference< XPropertySetInfo > xPropSetInfo =
                xPropSet->getPropertySetInfo();
    if( xPropSetInfo->hasPropertyByName( sIsAutoUpdate ) )
    {
        Any aAny;
        sal_Bool bTmp = bAutoUpdate;
        aAny.setValue( &bTmp, ::getBooleanCppuType() );
        xPropSet->setPropertyValue( sIsAutoUpdate, aAny );
    }

    sal_uInt16 nCategory = ParagraphStyleCategory::TEXT;
    if(  XML_STYLE_FAMILY_TEXT_PARAGRAPH == GetFamily() &&
         sCategoryVal.getLength() && xStyle->isUserDefined() &&
         xPropSetInfo->hasPropertyByName( sCategory ) &&
          SvXMLUnitConverter::convertEnum( nCategory, sCategoryVal, aCategoryMap ) )
    {
        Any aAny;
        aAny <<= (sal_Int16)nCategory;
        xPropSet->setPropertyValue( sCategory, aAny );
    }

    // tell the style about it's events (if applicable)
    if (NULL != pEventContext)
    {
        // set event suppplier and release reference to context
        Reference<document::XEventsSupplier> xEventsSupplier(xStyle,UNO_QUERY);
        pEventContext->SetEvents(xEventsSupplier);
        pEventContext->ReleaseRef();
    }
}

void XMLTextStyleContext::Finish( sal_Bool bOverwrite )
{
    XMLPropStyleContext::Finish( bOverwrite );

    Reference < XStyle > xStyle = GetStyle();
    if( !(sListStyleName.getLength() || sDropCapTextStyleName.getLength() ||
        bHasMasterPageName) ||
        !xStyle.is() || !(bOverwrite || IsNew()) )
        return;

    Reference < XPropertySet > xPropSet( xStyle, UNO_QUERY );
    Reference< XPropertySetInfo > xPropSetInfo =
                xPropSet->getPropertySetInfo();

    if( sListStyleName.getLength() )
    {
        // The families cointaner must exist
        const Reference < XNameContainer >& rNumStyles =
            GetImport().GetTextImport()->GetNumberingStyles();
        if( rNumStyles.is() && rNumStyles->hasByName( sListStyleName ) &&
            xPropSetInfo->hasPropertyByName( sNumberingStyleName ) )
        {
            Any aAny;
            aAny <<= sListStyleName;
            xPropSet->setPropertyValue( sNumberingStyleName, aAny );
        }
    }

    if( sDropCapTextStyleName.getLength() )
    {
        // The families cointaner must exist
        const Reference < XNameContainer >& rTextStyles =
            GetImport().GetTextImport()->GetTextStyles();
        if( rTextStyles.is() &&
            rTextStyles->hasByName( sDropCapTextStyleName ) &&
            xPropSetInfo->hasPropertyByName( sDropCapCharStyleName ) )
        {
            Any aAny;
            aAny <<= sDropCapTextStyleName;
            xPropSet->setPropertyValue( sDropCapCharStyleName, aAny );
        }
    }

    if( bHasMasterPageName )
    {
        // The families cointaner must exist
        const Reference < XNameContainer >& rPageStyles =
            GetImport().GetTextImport()->GetPageStyles();
        if( ( !sMasterPageName.getLength() ||
              (rPageStyles.is() &&
               rPageStyles->hasByName( sMasterPageName )) ) &&
            xPropSetInfo->hasPropertyByName( sPageDescName ) )
        {
            Any aAny;
            aAny <<= sMasterPageName;
            xPropSet->setPropertyValue( sPageDescName, aAny );
        }
    }
}

void XMLTextStyleContext::FillPropertySet(
    const Reference<XPropertySet > & rPropSet )
{
    // imitate the FillPropertySet of the super class, so we get a chance to
    // catch the combined characters attribute

    // imitate XMLPropStyleContext::FillPropertySet(...)
    UniReference < SvXMLImportPropertyMapper > xImpPrMap =
        ((SvXMLStylesContext *)GetStyles())->GetImportPropertyMapper(GetFamily());
    DBG_ASSERT( xImpPrMap.is(), "Where is the import prop mapper?" );
    if( xImpPrMap.is() )
    {

        // imitate SvXMLImportPropertyMapper::FillPropertySet(...)

        sal_Bool bSet = sal_False;
        Reference< XPropertySetInfo > xInfo = rPropSet->getPropertySetInfo();

        sal_Int32 nCount = GetProperties().size();
        for( sal_Int32 i=0; i < nCount; i++ )
        {
            const XMLPropertyState& rProp = GetProperties()[i];
            sal_Int32 nIdx = rProp.mnIndex;
            if( -1 == nIdx )
                continue;

            UniReference<XMLPropertySetMapper> rPropMapper =
                xImpPrMap->getPropertySetMapper();


            // catch combined characters
            if (XML_NAMESPACE_STYLE == rPropMapper->GetEntryNameSpace(nIdx) &&
                rPropMapper->GetEntryXMLName(nIdx).equalsAsciiL(
                    sXML_text_combine, sizeof(sXML_text_combine)-1) &&
                (rPropMapper->GetEntryAPIName(nIdx).getLength() == 0) )
            {
                Any aAny = rProp.maValue;
                sal_Bool bVal = *(sal_Bool*)aAny.getValue();
                bHasCombinedCharactersLetter = bVal;
            }

            // now proceed normally
            const OUString& rPropName = rPropMapper->GetEntryAPIName( nIdx );
            if( xInfo->hasPropertyByName( rPropName ) )
            {
                try
                {
                    rPropSet->setPropertyValue( rPropName, rProp.maValue );
                    bSet = sal_True;
                }
                catch(...)
                {
                }
            }
        }
    }
}


