/*************************************************************************
 *
 *  $RCSfile: xmlaustp.cxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: dvo $ $Date: 2001-10-25 20:57:03 $
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
#ifndef _XMLOFF_XMLASTPL_IMPL_HXX
#include "impastpl.hxx"
#endif
#ifndef _XMLOFF_XMLASTPLP_HXX
#include "xmlaustp.hxx"
#endif
#ifndef _XMLOFF_FAMILIES_HXX_
#include "families.hxx"
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_XMLTOKEN_HXX
#include "xmltoken.hxx"
#endif
#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif

#ifndef _XMLOFF_PAGEMASTERSTYLEMAP_HXX
#include "PageMasterStyleMap.hxx"
#endif
#ifndef _XMLOFF_PAGEMASTEREXPORTPROPMAPPER_HXX
#include "PageMasterExportPropMapper.hxx"
#endif
#ifndef _XMLBACKGROUNDIMAGEEXPORT_HXX
#include "XMLBackgroundImageExport.hxx"
#endif


using namespace ::std;
using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::xmloff::token;


void SvXMLAutoStylePoolP::exportStyleAttributes(
#if SUPD < 650
        SvXMLAttributeList& rAttrList,
#endif
        sal_Int32 nFamily,
        const vector< XMLPropertyState >& rProperties,
        const SvXMLExportPropertyMapper& rPropExp
#if SUPD < 650
        , const SvXMLUnitConverter& rUnitConverter,
        const SvXMLNamespaceMap& rNamespaceMap
#endif
        ) const
{
    if (XML_STYLE_FAMILY_SD_GRAPHICS_ID == nFamily)
    {   // it's a graphics style
        UniReference< XMLPropertySetMapper > aPropertyMapper = rPropExp.getPropertySetMapper();
        DBG_ASSERT(aPropertyMapper.is(), "SvXMLAutoStylePoolP::exportStyleAttributes: invalid property set mapper!");

#ifdef DBG_UTIL
        sal_Bool bFoundControlShapeDataStyle = sal_False;
#endif
        for (   vector< XMLPropertyState >::const_iterator pProp = rProperties.begin();
                pProp != rProperties.end();
                ++pProp
            )
        {
            if (pProp->mnIndex > -1)
            {   // it's a valid property
                if (CTF_SD_CONTROL_SHAPE_DATA_STYLE == aPropertyMapper->GetEntryContextId(pProp->mnIndex))
                {   // it's the control shape data style property

#ifdef DBG_UTIL
                    if (bFoundControlShapeDataStyle)
                    {
                        DBG_ERROR("SvXMLAutoStylePoolP::exportStyleAttributes: found two properties with the ControlShapeDataStyle context id!");
                        // already added the attribute for the first occurence
                        break;
                    }
#endif
                    // obtain the data style name
                    ::rtl::OUString sControlDataStyleName;
                    pProp->maValue >>= sControlDataStyleName;
                    DBG_ASSERT(sControlDataStyleName.getLength(), "SvXMLAutoStylePoolP::exportStyleAttributes: invalid property value for the data style name!");

                    // add the attribute
                    GetExport().AddAttribute(
                        aPropertyMapper->GetEntryNameSpace(pProp->mnIndex),
                        aPropertyMapper->GetEntryXMLName(pProp->mnIndex),
                        sControlDataStyleName );

#ifdef DBG_UTIL
                    // in a non-pro version, check if there is another property with the special context id we're handling here
                    bFoundControlShapeDataStyle = sal_True;
                    continue;
#else
                    // in a pro version, just leave the loop
                    break;
#endif
                }
            }
        }
    }

    if( nFamily == XML_STYLE_FAMILY_PAGE_MASTER )
    {
        for( vector< XMLPropertyState >::const_iterator pProp = rProperties.begin(); pProp != rProperties.end(); pProp++ )
        {
            if (pProp->mnIndex > -1)
            {
                UniReference< XMLPropertySetMapper > aPropMapper = rPropExp.getPropertySetMapper();
                sal_Int32 nIndex = pProp->mnIndex;
                sal_Int16 nContextID = aPropMapper->GetEntryContextId( nIndex );
                switch( nContextID )
                {
                    case CTF_PM_PAGEUSAGE:
                    {
                        OUString sValue;
                        const XMLPropertyHandler* pPropHdl = aPropMapper->GetPropertyHandler( nIndex );
                        if( pPropHdl &&
                            pPropHdl->exportXML( sValue, pProp->maValue,
                                                 GetExport().GetMM100UnitConverter() ) &&
                            ( ! IsXMLToken( sValue, XML_ALL ) ) )
                        {
                            GetExport().AddAttribute( aPropMapper->GetEntryNameSpace( nIndex ), aPropMapper->GetEntryXMLName( nIndex ), sValue );
                        }
                    }
                    break;
                }
            }
        }
    }
}

void SvXMLAutoStylePoolP::exportStyleContent(
#if SUPD < 650
        const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > & rHandler,
#endif
        sal_Int32 nFamily,
        const vector< XMLPropertyState >& rProperties,
        const SvXMLExportPropertyMapper& rPropExp
#if SUPD < 650
        , const SvXMLUnitConverter& rUnitConverter,
        const SvXMLNamespaceMap& rNamespaceMap
#endif
        ) const
{
    if( nFamily == XML_STYLE_FAMILY_PAGE_MASTER )
    {
        OUString sWS( GetXMLToken(XML_WS) );

        sal_Int32       nHeaderStartIndex(-1);
        sal_Int32       nHeaderEndIndex(-1);
        sal_Int32       nFooterStartIndex(-1);
        sal_Int32       nFooterEndIndex(-1);
        sal_Bool        bHeaderStartIndex(sal_False);
        sal_Bool        bHeaderEndIndex(sal_False);
        sal_Bool        bFooterStartIndex(sal_False);
        sal_Bool        bFooterEndIndex(sal_False);

        UniReference< XMLPropertySetMapper > aPropMapper = rPropExp.getPropertySetMapper();

        sal_Int32 nIndex(0);
        while(nIndex < aPropMapper->GetEntryCount())
        {
            switch( aPropMapper->GetEntryContextId( nIndex ) & CTF_PM_FLAGMASK )
            {
                case CTF_PM_HEADERFLAG:
                {
                    if (!bHeaderStartIndex)
                    {
                        nHeaderStartIndex = nIndex;
                        bHeaderStartIndex = sal_True;
                    }
                    if (bFooterStartIndex && !bFooterEndIndex)
                    {
                        nFooterEndIndex = nIndex;
                        bFooterEndIndex = sal_True;
                    }
                }
                break;
                case CTF_PM_FOOTERFLAG:
                {
                    if (!bFooterStartIndex)
                    {
                        nFooterStartIndex = nIndex;
                        bFooterStartIndex = sal_True;
                    }
                    if (bHeaderStartIndex && !bHeaderEndIndex)
                    {
                        nHeaderEndIndex = nIndex;
                        bHeaderEndIndex = sal_True;
                    }
                }
                break;
            }
            nIndex++;
        }
        if (!bHeaderEndIndex)
            nHeaderEndIndex = nIndex;
        if (!bFooterEndIndex)
            nFooterEndIndex = nIndex;

        // export header style element
        {
            SvXMLElementExport aElem(
                GetExport(), XML_NAMESPACE_STYLE, XML_HEADER_STYLE,
                sal_True, sal_True );

            rPropExp.exportXML(
                GetExport(), rProperties,
                nHeaderStartIndex, nHeaderEndIndex, XML_EXPORT_FLAG_IGN_WS);
        }

        // export footer style
        {
            SvXMLElementExport aElem(
                GetExport(), XML_NAMESPACE_STYLE, XML_FOOTER_STYLE,
                sal_True, sal_True );

            rPropExp.exportXML(
                GetExport(), rProperties,
                nFooterStartIndex, nFooterEndIndex, XML_EXPORT_FLAG_IGN_WS);
        }
    }
}

#if SUPD < 650
SvXMLAutoStylePoolP::SvXMLAutoStylePoolP()
{
    DBG_ERROR("This constuctor is obsoleted and should not be used!");
    pImpl = NULL;
}
#endif

SvXMLAutoStylePoolP::SvXMLAutoStylePoolP( SvXMLExport& rExport )
{
    pImpl = new SvXMLAutoStylePoolP_Impl( rExport );
}

SvXMLAutoStylePoolP::~SvXMLAutoStylePoolP()
{
    delete pImpl;
}

SvXMLExport& SvXMLAutoStylePoolP::GetExport() const
{
    return pImpl->GetExport();
}

// TODO: romove this
void SvXMLAutoStylePoolP::AddFamily(
        sal_Int32 nFamily,
        const OUString& rStrName,
        SvXMLExportPropertyMapper* pMapper,
        OUString aStrPrefix,
        sal_Bool bAsFamily )
{
    UniReference <SvXMLExportPropertyMapper> xTmp = pMapper;
    AddFamily( nFamily, rStrName, xTmp, aStrPrefix, bAsFamily );
}

void SvXMLAutoStylePoolP::AddFamily(
        sal_Int32 nFamily,
        const OUString& rStrName,
        const UniReference < SvXMLExportPropertyMapper > & rMapper,
        const OUString& rStrPrefix,
        sal_Bool bAsFamily )
{
    pImpl->AddFamily( nFamily, rStrName, rMapper, rStrPrefix, bAsFamily );
}

void SvXMLAutoStylePoolP::RegisterName( sal_Int32 nFamily,
                                         const OUString& rName )
{
    pImpl->RegisterName( nFamily, rName );
}

OUString SvXMLAutoStylePoolP::Add( sal_Int32 nFamily,
                                  const vector< XMLPropertyState >& rProperties )
{
    OUString sEmpty;
    OUString sName;
    pImpl->Add(sName, nFamily, sEmpty, rProperties );
    return sName;
}

OUString SvXMLAutoStylePoolP::Add( sal_Int32 nFamily,
                                  const OUString& rParent,
                                  const vector< XMLPropertyState >& rProperties )
{
    OUString sName;
    pImpl->Add(sName, nFamily, rParent, rProperties );
    return sName;
}

sal_Bool SvXMLAutoStylePoolP::Add(OUString& rName, sal_Int32 nFamily, const ::std::vector< XMLPropertyState >& rProperties )
{
    OUString sEmpty;
    return pImpl->Add(rName, nFamily, sEmpty, rProperties);
}

sal_Bool SvXMLAutoStylePoolP::Add(OUString& rName, sal_Int32 nFamily, const OUString& rParent, const ::std::vector< XMLPropertyState >& rProperties )
{
    return pImpl->Add(rName, nFamily, rParent, rProperties);
}

OUString SvXMLAutoStylePoolP::AddAndCache( sal_Int32 nFamily,
                                  const vector< XMLPropertyState >& rProperties )
{
    OUString sEmpty;
    OUString sName;
    pImpl->Add(sName, nFamily, sEmpty, rProperties, sal_True );
    return sName;
}

OUString SvXMLAutoStylePoolP::AddAndCache( sal_Int32 nFamily,
                                  const OUString& rParent,
                                  const vector< XMLPropertyState >& rProperties )
{
    OUString sName;
    pImpl->Add(sName, nFamily, rParent, rProperties, sal_True );
    return sName;
}

OUString SvXMLAutoStylePoolP::AddAndCache( sal_Int32 nFamily,
                                  const OUString& rParent )
{
    return pImpl->AddToCache( nFamily, rParent );
}

OUString SvXMLAutoStylePoolP::Find( sal_Int32 nFamily,
                                   const vector< XMLPropertyState >& rProperties ) const
{
    OUString sEmpty;
    return pImpl->Find( nFamily, sEmpty, rProperties );
}

OUString SvXMLAutoStylePoolP::Find( sal_Int32 nFamily,
                                   const OUString& rParent,
                                   const vector< XMLPropertyState >& rProperties ) const
{
    return pImpl->Find( nFamily, rParent, rProperties );
}

OUString SvXMLAutoStylePoolP::FindAndRemoveCached( sal_Int32 nFamily ) const
{
    return pImpl->FindAndRemoveCached( nFamily );
}


void SvXMLAutoStylePoolP::exportXML( sal_Int32 nFamily

#if SUPD < 650
    , const uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > & rHandler,
    const SvXMLUnitConverter& rUnitConverter,
    const SvXMLNamespaceMap& rNamespaceMap
#endif
    ) const
{
    pImpl->exportXML( nFamily,
#if SUPD < 650
                      GetExport().GetDocHandler(),
                      GetExport().GetMM100UnitConverter(),
                      GetExport().GetNamespaceMap(),
#endif
                      this);
}

void SvXMLAutoStylePoolP::ClearEntries()
{
    pImpl->ClearEntries();
}
