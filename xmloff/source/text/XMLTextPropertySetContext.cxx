/*************************************************************************
 *
 *  $RCSfile: XMLTextPropertySetContext.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: dvo $ $Date: 2001-02-16 16:39:27 $
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

#ifndef _XMLOFF_XMLTEXTPROPERTYSETCONTEXT_HXX
#include "XMLTextPropertySetContext.hxx"
#endif
#ifndef _XMLTEXTCOLUMNSCONTEXT_HXX
#include "XMLTextColumnsContext.hxx"
#endif
#ifndef _XMLBACKGROUNDIMAGECONTEXT_HXX
#include "XMLBackgroundImageContext.hxx"
#endif
#ifndef _XMLOFF_XMLSECTIONFOOTNOTECONFIGIMPORT_HXX
#include "XMLSectionFootnoteConfigImport.hxx"
#endif

#ifndef _XMLOFF_TXTPRMAP_HXX
#include "txtprmap.hxx"
#endif
#ifndef _XMLOFF_XMLTABI_HXX
#include "xmltabi.hxx"
#endif
#ifndef _XMLOFF_TXTDROPI_HXX
#include "txtdropi.hxx"
#endif

using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star;

XMLTextPropertySetContext::XMLTextPropertySetContext(
                 SvXMLImport& rImport, sal_uInt16 nPrfx,
                 const OUString& rLName,
                 const Reference< xml::sax::XAttributeList > & xAttrList,
                 ::std::vector< XMLPropertyState > &rProps,
                 const UniReference < SvXMLImportPropertyMapper > &rMap,
                 OUString& rDCTextStyleName ) :
    SvXMLPropertySetContext( rImport, nPrfx, rLName, xAttrList, rProps, rMap ),
    rDropCapTextStyleName( rDCTextStyleName )
{
}

XMLTextPropertySetContext::~XMLTextPropertySetContext()
{
}

SvXMLImportContext *XMLTextPropertySetContext::CreateChildContext(
                   sal_uInt16 nPrefix,
                   const OUString& rLocalName,
                   const Reference< xml::sax::XAttributeList > & xAttrList,
                   ::std::vector< XMLPropertyState > &rProperties,
                   const XMLPropertyState& rProp )
{
    SvXMLImportContext *pContext = 0;

    switch( xMapper->getPropertySetMapper()
                    ->GetEntryContextId( rProp.mnIndex ) )
    {
    case CTF_TABSTOP:
        pContext = new SvxXMLTabStopImportContext( GetImport(), nPrefix,
                                                   rLocalName, rProp,
                                                   rProperties );
        break;
    case CTF_TEXTCOLUMNS:
        pContext = new XMLTextColumnsContext( GetImport(), nPrefix,
                                                   rLocalName, xAttrList, rProp,
                                                   rProperties );
        break;
    case CTF_DROPCAPFORMAT:
        {
            DBG_ASSERT( rProp.mnIndex >= 2 &&
                        CTF_DROPCAPWHOLEWORD  == xMapper->getPropertySetMapper()
                            ->GetEntryContextId( rProp.mnIndex-2 ),
                        "invalid property map!");
            XMLTextDropCapImportContext *pDCContext =
                new XMLTextDropCapImportContext( GetImport(), nPrefix,
                                                        rLocalName, xAttrList,
                                                        rProp,
                                                        rProp.mnIndex-2,
                                                        rProperties );
            rDropCapTextStyleName = pDCContext->GetStyleName();
        pContext = pDCContext;
        }
        break;

    case CTF_BACKGROUND_URL:
        DBG_ASSERT( rProp.mnIndex >= 2 &&
                    CTF_BACKGROUND_POS  == xMapper->getPropertySetMapper()
                        ->GetEntryContextId( rProp.mnIndex-2 ) &&
                    CTF_BACKGROUND_FILTER  == xMapper->getPropertySetMapper()
                        ->GetEntryContextId( rProp.mnIndex-1 ),
                    "invalid property map!");
        pContext =
            new XMLBackgroundImageContext( GetImport(), nPrefix,
                                           rLocalName, xAttrList,
                                           rProp,
                                           rProp.mnIndex-2,
                                           rProp.mnIndex-1,
                                           rProperties );
        break;
    case CTF_SECTION_FOOTNOTE_END:
        DBG_ASSERT( rProp.mnIndex >= 7, "invalid property map!");
        DBG_ASSERT( CTF_SECTION_FOOTNOTE_NUM_OWN ==
                    xMapper->getPropertySetMapper()->GetEntryContextId(
                        rProp.mnIndex-6 ), "invalid property map!");
        DBG_ASSERT( CTF_SECTION_FOOTNOTE_NUM_RESTART ==
                    xMapper->getPropertySetMapper()->GetEntryContextId(
                        rProp.mnIndex-5 ), "invalid property map!");
        DBG_ASSERT( CTF_SECTION_FOOTNOTE_NUM_RESTART_AT ==
                    xMapper->getPropertySetMapper()->GetEntryContextId(
                        rProp.mnIndex-4 ), "invalid property map!");
        DBG_ASSERT( CTF_SECTION_FOOTNOTE_NUM_TYPE ==
                    xMapper->getPropertySetMapper()->GetEntryContextId(
                        rProp.mnIndex-3 ), "invalid property map!");
        DBG_ASSERT( CTF_SECTION_FOOTNOTE_NUM_PREFIX ==
                    xMapper->getPropertySetMapper()->GetEntryContextId(
                        rProp.mnIndex-2 ), "invalid property map!");
        DBG_ASSERT( CTF_SECTION_FOOTNOTE_NUM_SUFFIX ==
                    xMapper->getPropertySetMapper()->GetEntryContextId(
                        rProp.mnIndex-1 ), "invalid property map!");
        DBG_ASSERT( CTF_SECTION_FOOTNOTE_END ==
                    xMapper->getPropertySetMapper()->GetEntryContextId(
                        rProp.mnIndex ), "invalid property map!");
        pContext =
            new XMLSectionFootnoteConfigImport(
                GetImport(), nPrefix, rLocalName, rProperties, rProp.mnIndex);
        break;
    case CTF_SECTION_ENDNOTE_END:
        DBG_ASSERT( rProp.mnIndex >= 7, "invalid property map!");
        DBG_ASSERT( CTF_SECTION_ENDNOTE_NUM_OWN ==
                    xMapper->getPropertySetMapper()->GetEntryContextId(
                        rProp.mnIndex-6 ), "invalid property map!");
        DBG_ASSERT( CTF_SECTION_ENDNOTE_NUM_RESTART ==
                    xMapper->getPropertySetMapper()->GetEntryContextId(
                        rProp.mnIndex-5 ), "invalid property map!");
        DBG_ASSERT( CTF_SECTION_ENDNOTE_NUM_RESTART_AT ==
                    xMapper->getPropertySetMapper()->GetEntryContextId(
                        rProp.mnIndex-4 ), "invalid property map!");
        DBG_ASSERT( CTF_SECTION_ENDNOTE_NUM_TYPE ==
                    xMapper->getPropertySetMapper()->GetEntryContextId(
                        rProp.mnIndex-3 ), "invalid property map!");
        DBG_ASSERT( CTF_SECTION_ENDNOTE_NUM_PREFIX ==
                    xMapper->getPropertySetMapper()->GetEntryContextId(
                        rProp.mnIndex-2 ), "invalid property map!");
        DBG_ASSERT( CTF_SECTION_ENDNOTE_NUM_SUFFIX ==
                    xMapper->getPropertySetMapper()->GetEntryContextId(
                        rProp.mnIndex-1 ), "invalid property map!");
        DBG_ASSERT( CTF_SECTION_ENDNOTE_END ==
                    xMapper->getPropertySetMapper()->GetEntryContextId(
                        rProp.mnIndex ), "invalid property map!");
        pContext =
            new XMLSectionFootnoteConfigImport(
                GetImport(), nPrefix, rLocalName, rProperties, rProp.mnIndex);
        break;
    }

    if( !pContext )
        pContext = SvXMLPropertySetContext::CreateChildContext( nPrefix, rLocalName,
                                                            xAttrList,
                                                            rProperties, rProp );

    return pContext;
}


