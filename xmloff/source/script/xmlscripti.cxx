/*************************************************************************
 *
 *  $RCSfile: xmlscripti.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: mib $ $Date: 2001-06-27 07:32:06 $
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

#include <tools/debug.hxx>
#include <tools/isolang.hxx>
#include <tools/time.hxx>

#include "xmlscripti.hxx"
#include "xmlnmspe.hxx"
#include "xmlkywd.hxx"
#include "xmlimp.hxx"
#include "nmspmap.hxx"
#include "XMLEventsImportContext.hxx"

#include <com/sun/star/script/XStarBasicAccess.hpp>
#include <com/sun/star/script/XStarBasicModuleInfo.hpp>
#include <com/sun/star/script/XStarBasicDialogInfo.hpp>
#include <com/sun/star/script/XStarBasicLibraryInfo.hpp>
#include <com/sun/star/document/XEventsSupplier.hpp>

using namespace rtl;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::frame;
using namespace com::sun::star::script;
using namespace com::sun::star::document;
using namespace com::sun::star::xml::sax;


//-------------------------------------------------------------------------

class XMLScriptElementContext;

class XMLScriptModuleContext : public SvXMLImportContext
{
private:
    XMLScriptElementContext&    mrParent;
    OUString                    msSource;
    Reference<XStarBasicAccess> mxBasicAccess;

    OUString                    msLibName;
    OUString                    msModuleName;
    OUString                    msLanguage;

public:
    XMLScriptModuleContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
                            const OUString& rLName, const OUString& aLibName,
                            const Reference<XAttributeList>& xAttrList,
                            XMLScriptElementContext& rParentContext,
                            Reference<XStarBasicAccess> xBasicAccess );

    virtual ~XMLScriptModuleContext();

    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                                 const OUString& rLName,
                                 const Reference<XAttributeList>& xAttrList );
    virtual void EndElement();
    virtual void Characters( const rtl::OUString& rChars );
};

//-------------------------------------------------------------------------

class XMLScriptElementContext : public SvXMLImportContext
{
private:
    XMLScriptContext&           mrParent;
    OUString                    msLName;
    OUString                    msContent;
    Reference<XStarBasicAccess> mxBasicAccess;

    OUString                    msLibName;

public:
    XMLScriptElementContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
                            const OUString& rLName,
                            const Reference<XAttributeList>& xAttrList,
                            XMLScriptContext& rParentContext,
                            Reference<XStarBasicAccess> xBasicAccess );

    virtual ~XMLScriptElementContext();

    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                                 const OUString& rLName,
                                 const Reference<XAttributeList>& xAttrList );
    virtual void EndElement();
    virtual void Characters( const rtl::OUString& rChars );
};


//-------------------------------------------------------------------------

XMLScriptElementContext::XMLScriptElementContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
                                    const OUString& rLName,
                                    const Reference<XAttributeList>& xAttrList,
                                    XMLScriptContext& rParentContext,
                                    Reference<XStarBasicAccess> xBasicAccess )
    : SvXMLImportContext( rImport, nPrfx, rLName )
    , msLName( rLName )
    , mrParent( rParentContext )
    , mxBasicAccess( xBasicAccess )
{
    mrParent.AddRef();

    OUString sPassword;
    OUString sExternalSourceURL;
    OUString sLinkTargetURL;

    sal_Bool bEmbedded = sal_False;
    sal_Bool bLinked = sal_False;
    if( msLName.equalsAsciiL( sXML_library_embedded, sizeof(sXML_library_embedded)-1 ) )
        bEmbedded = sal_True;
    else if( msLName.equalsAsciiL( sXML_library_linked, sizeof(sXML_library_linked)-1 ) )
        bLinked = sal_True;

    if( bEmbedded || bLinked )
    {
        sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
        for( sal_Int16 i = 0 ; i < nAttrCount ; i++ )
        {
            OUString sFullAttrName = xAttrList->getNameByIndex( i );
            OUString sAttrName;
            sal_Int16 nAttrPrefix =
                GetImport().GetNamespaceMap().GetKeyByAttrName( sFullAttrName,
                                                                &sAttrName );

            if( (XML_NAMESPACE_SCRIPT == nAttrPrefix) &&
                sAttrName.equalsAsciiL( sXML_name, sizeof(sXML_name)-1 ) )
            {
                msLibName = xAttrList->getValueByIndex( i );
            }
            else if( (XML_NAMESPACE_SCRIPT == nAttrPrefix) &&
                     sAttrName.equalsAsciiL( sXML_password,
                                             sizeof(sXML_password)-1 ) )
            {
                sPassword = xAttrList->getValueByIndex( i );
            }
            else if( (XML_NAMESPACE_XLINK == nAttrPrefix) && bLinked &&
                     sAttrName.equalsAsciiL( sXML_href, sizeof(sXML_href)-1 ) )
            {
                sLinkTargetURL = GetImport().GetAbsoluteReference(xAttrList->getValueByIndex( i ));
            }
            //else if( msLName.equalsAsciiL( sXML_external_source_url, sizeof(sXML_external_source_url)-1 ) )
            //{
                //sLinkTargetURL = xAttrList->getValueByIndex( i );
            //}
        }
    }

    if( msLibName.getLength() )
        mxBasicAccess->createLibrary( msLibName, sPassword, sExternalSourceURL, sLinkTargetURL );
}

XMLScriptElementContext::~XMLScriptElementContext()
{
    mrParent.ReleaseRef();
}


SvXMLImportContext* XMLScriptElementContext::CreateChildContext( sal_uInt16 nPrefix,
                                     const OUString& rLName,
                                     const Reference<XAttributeList>& xAttrList )
{
    SvXMLImportContext* pContext = NULL;

    if ( XML_NAMESPACE_SCRIPT == nPrefix)
    {
        if( msLName.equalsAsciiL( sXML_library_embedded, sizeof(sXML_library_embedded)-1 ) )
        {
            if( rLName.equalsAsciiL( sXML_module, sizeof(sXML_module)-1 ) )
            {
                pContext = new XMLScriptModuleContext( GetImport(), nPrefix,
                    rLName, msLibName, xAttrList, *this, mxBasicAccess );
            }
            //else if( rLName.equalsAsciiL( sXML_dialog, sizeof(sXML_dilaog)-1 ) )
            //{
                //pContext = new XMLScriptDialogContext( GetImport(),
                    //nPrefix, rLName, xAttrList, *this, mxBasicAccess );
            //}
        }
    }
    // else: unknown namespace: ignore

    if ( !pContext )
    {
        //  default context to ignore unknown elements
        pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );
    }
    return pContext;
}

void XMLScriptElementContext::EndElement()
{
}

void XMLScriptElementContext::Characters( const rtl::OUString& rChars )
{
    msContent += rChars;
}

//-------------------------------------------------------------------------

XMLScriptModuleContext::XMLScriptModuleContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
                                    const OUString& rLName, const OUString& aLibName,
                                    const Reference<XAttributeList>& xAttrList,
                                    XMLScriptElementContext& rParentContext,
                                    Reference<XStarBasicAccess> xBasicAccess )
    : SvXMLImportContext( rImport, nPrfx, rLName )
    , msLibName( aLibName )
    , mrParent( rParentContext )
    , mxBasicAccess( xBasicAccess )
{
    mrParent.AddRef();

    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    for( sal_Int16 i = 0 ; i < nAttrCount ; i++ )
    {
        OUString sFullAttrName = xAttrList->getNameByIndex( i );
        OUString sAttrName;
        sal_Int16 nAttrPrefix =
            GetImport().GetNamespaceMap().GetKeyByAttrName( sFullAttrName,
                                                            &sAttrName );

        if( (XML_NAMESPACE_SCRIPT == nAttrPrefix) &&
            sAttrName.equalsAsciiL( sXML_name, sizeof(sXML_name)-1 ) )
        {
            msModuleName = xAttrList->getValueByIndex( i );
        }
        else if( (XML_NAMESPACE_SCRIPT == nAttrPrefix) &&
                 sAttrName.equalsAsciiL( sXML_language,
                                         sizeof(sXML_language)-1 ) )
        {
            msLanguage = xAttrList->getValueByIndex( i );
        }
    }
}

XMLScriptModuleContext::~XMLScriptModuleContext()
{
    mrParent.ReleaseRef();
}

SvXMLImportContext* XMLScriptModuleContext::CreateChildContext( sal_uInt16 nPrefix,
                                     const OUString& rLName,
                                     const Reference<XAttributeList>& xAttrList )
{
    SvXMLImportContext* pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );
    return pContext;
}

void XMLScriptModuleContext::EndElement()
{
    mxBasicAccess->addModule( msLibName, msModuleName, msLanguage, msSource );
}

void XMLScriptModuleContext::Characters( const rtl::OUString& rChars )
{
    msSource += rChars;
}


//-------------------------------------------------------------------------
//
//  context for <office:script> element
//

XMLScriptContext::XMLScriptContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
                                    const OUString& rLName,
                                    const Reference<XModel>& rDocModel )
    : SvXMLImportContext( rImport, nPrfx, rLName )
{
    // Get Basic data
    mxBasicAccess = Reference< XStarBasicAccess >( rDocModel, UNO_QUERY );
    DBG_ASSERT( mxBasicAccess.is(), "no StarBasicAccess" );
}

XMLScriptContext::~XMLScriptContext()
{
}

SvXMLImportContext* XMLScriptContext::CreateChildContext( sal_uInt16 nPrefix,
                                    const OUString& rLName,
                                    const Reference<XAttributeList>& xAttrList )
{
    SvXMLImportContext* pContext = NULL;

    if (XML_NAMESPACE_SCRIPT == nPrefix)
    {
        if( rLName.equalsAsciiL( sXML_library_embedded,
                                 sizeof(sXML_library_embedded)-1 ) ||
            rLName.equalsAsciiL( sXML_library_linked,
                                 sizeof(sXML_library_linked)-1 )    )
        {
            pContext = new XMLScriptElementContext(
                GetImport(), nPrefix, rLName, xAttrList, *this, mxBasicAccess);
        }
    }
    else if (XML_NAMESPACE_OFFICE == nPrefix)
    {
        if ( rLName.equalsAsciiL(sXML_events, sizeof(sXML_events)-1) )
        {
            Reference<XEventsSupplier> xSupplier(GetImport().GetModel(),
                                                 UNO_QUERY);
            pContext = new XMLEventsImportContext(GetImport(), nPrefix, rLName,
                                                  xSupplier);
        }
    }

    if (NULL == pContext)
        pContext = SvXMLImportContext::CreateChildContext(nPrefix, rLName,
                                                          xAttrList);

    return pContext;
}

void XMLScriptContext::EndElement()
{
}

