/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: XMLStarBasicExportHandler.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 15:21:43 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_xmloff.hxx"

#ifndef _XMLOFF_XMLSTARBASICEXPORTHANDLER_HXX
#include "XMLStarBasicExportHandler.hxx"
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _XMLOFF_XMLEXP_HXX
#include <xmloff/xmlexp.hxx>
#endif

#ifndef _XMLOFF_XMLTOKEN_HXX
#include <xmloff/xmltoken.hxx>
#endif

#ifndef _XMLOFF_NMSPMAP_HXX
#include <xmloff/nmspmap.hxx>
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif


using namespace ::com::sun::star::uno;
using namespace ::xmloff::token;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::com::sun::star::beans::PropertyValue;


XMLStarBasicExportHandler::XMLStarBasicExportHandler() :
    sStarBasic(RTL_CONSTASCII_USTRINGPARAM("StarBasic")),
    sLibrary(RTL_CONSTASCII_USTRINGPARAM("Library")),
    sMacroName(RTL_CONSTASCII_USTRINGPARAM("MacroName")),
    sStarOffice(RTL_CONSTASCII_USTRINGPARAM("StarOffice")),
    sApplication(RTL_CONSTASCII_USTRINGPARAM("application"))
{
}

XMLStarBasicExportHandler::~XMLStarBasicExportHandler()
{
}

void XMLStarBasicExportHandler::Export(
    SvXMLExport& rExport,
    const OUString& rEventQName,
    Sequence<PropertyValue> & rValues,
    sal_Bool bUseWhitespace)
{
    rExport.AddAttribute(XML_NAMESPACE_SCRIPT, XML_LANGUAGE,
                         rExport.GetNamespaceMap().GetQNameByKey(
                             XML_NAMESPACE_OOO, sStarBasic ) );
    rExport.AddAttribute(XML_NAMESPACE_SCRIPT, XML_EVENT_NAME, rEventQName);

    OUString sLocation, sName;
    sal_Int32 nCount = rValues.getLength();
    for(sal_Int32 i = 0; i < nCount; i++)
    {
        if (sLibrary.equals(rValues[i].Name))
        {
            OUString sTmp;
            rValues[i].Value >>= sTmp;
            sLocation = GetXMLToken(
                (sTmp.equalsIgnoreAsciiCase(sApplication) ||
                 sTmp.equalsIgnoreAsciiCase(sStarOffice) ) ? XML_APPLICATION
                                                           : XML_DOCUMENT );
        }
        else if (sMacroName.equals(rValues[i].Name))
        {
            rValues[i].Value >>= sName;
        }
        // else: disregard
    }

    if( sLocation.getLength() )
    {
        OUStringBuffer sTmp( sLocation.getLength() + sName.getLength() + 1 );
        sTmp = sLocation;
        sTmp.append( sal_Unicode( ':' ) );
        sTmp.append( sName );
        rExport.AddAttribute(XML_NAMESPACE_SCRIPT, XML_MACRO_NAME,
                            sTmp.makeStringAndClear());
    }
    else
    {
        rExport.AddAttribute(XML_NAMESPACE_SCRIPT, XML_MACRO_NAME, sName );
    }

    SvXMLElementExport aEventElemt(rExport, XML_NAMESPACE_SCRIPT,
                                   XML_EVENT_LISTENER,
                                   bUseWhitespace, sal_False);
}
