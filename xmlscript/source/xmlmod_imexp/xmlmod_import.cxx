/*************************************************************************
 *
 *  $RCSfile: xmlmod_import.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: dbo $ $Date: 2001-05-04 09:14:57 $
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
#include "imp_share.hxx"

#include <osl/diagnose.h>

#include <rtl/ustrbuf.hxx>

#include <xmlscript/xml_import.hxx>
#include <comphelper/processfactory.hxx>


namespace xmlscript
{

//##################################################################################################

//__________________________________________________________________________________________________
Reference< xml::XImportContext > ModuleElement::getParent()
    throw (RuntimeException)
{
    return static_cast< xml::XImportContext * >( _pParent );
}
//__________________________________________________________________________________________________
OUString ModuleElement::getLocalName()
    throw (RuntimeException)
{
    return _aLocalName;
}
//__________________________________________________________________________________________________
sal_Int32 ModuleElement::getUid()
    throw (RuntimeException)
{
    return XMLNS_DIALOGS_UID;
}
//__________________________________________________________________________________________________
Reference< xml::sax2::XExtendedAttributes > ModuleElement::getAttributes()
    throw (RuntimeException)
{
    return _xAttributes;
}
//__________________________________________________________________________________________________
void ModuleElement::ignorableWhitespace(
    OUString const & rWhitespaces )
    throw (xml::sax::SAXException, RuntimeException)
{
    // not used
}
//__________________________________________________________________________________________________
void ModuleElement::characters( OUString const & rChars )
    throw (xml::sax::SAXException, RuntimeException)
{
    _pImport->mrModuleDesc.aCode += rChars;
}
//__________________________________________________________________________________________________
void ModuleElement::endElement()
    throw (xml::sax::SAXException, RuntimeException)
{
}
//__________________________________________________________________________________________________
Reference< xml::XImportContext > ModuleElement::createChildContext(
    sal_Int32 nUid, OUString const & rLocalName,
    Reference< xml::sax2::XExtendedAttributes > const & xAttributes )
    throw (xml::sax::SAXException, RuntimeException)
{
    throw xml::sax::SAXException(
        OUString( RTL_CONSTASCII_USTRINGPARAM("unexpected element!") ),
        Reference< XInterface >(), Any() );
}

//__________________________________________________________________________________________________
ModuleElement::ModuleElement(
    OUString const & rLocalName,
    Reference< xml::sax2::XExtendedAttributes > const & xAttributes,
    ModuleElement * pParent, ModuleImport * pImport )
    SAL_THROW( () )
    : _pImport( pImport )
    , _pParent( pParent )
    , _aLocalName( rLocalName )
    , _xAttributes( xAttributes )
{
    _pImport->acquire();

    if (_pParent)
    {
        _pParent->acquire();
    }
}
//__________________________________________________________________________________________________
ModuleElement::~ModuleElement()
    SAL_THROW( () )
{
    _pImport->release();

    if (_pParent)
    {
        _pParent->release();
    }

#ifdef DEBUG
    OString aStr( OUStringToOString( _aLocalName, RTL_TEXTENCODING_ASCII_US ) );
    OSL_TRACE( "ModuleElement::~ModuleElement(): %s\n", aStr.getStr() );
#endif
}

//##################################################################################################

// XImporter
//__________________________________________________________________________________________________
void ModuleImport::startDocument()
    throw (xml::sax::SAXException, RuntimeException)
{
    // ignored
}
//__________________________________________________________________________________________________
void ModuleImport::endDocument()
    throw (xml::sax::SAXException, RuntimeException)
{
    // ignored
}
//__________________________________________________________________________________________________
void ModuleImport::processingInstruction(
    OUString const & rTarget, OUString const & rData )
    throw (xml::sax::SAXException, RuntimeException)
{
    // ignored for now: xxx todo
}
//__________________________________________________________________________________________________
void ModuleImport::setDocumentLocator(
    Reference< xml::sax::XLocator > const & xLocator )
    throw (xml::sax::SAXException, RuntimeException)
{
    // ignored for now: xxx todo
}
//__________________________________________________________________________________________________
Reference< xml::XImportContext > ModuleImport::createRootContext(
    sal_Int32 nUid, OUString const & rLocalName,
    Reference< xml::sax2::XExtendedAttributes > const & xAttributes )
    throw (xml::sax::SAXException, RuntimeException)
{
    if (XMLNS_SCRIPT_UID != nUid)
    {
        throw xml::sax::SAXException(
            OUString( RTL_CONSTASCII_USTRINGPARAM("illegal namespace!") ),
            Reference< XInterface >(), Any() );
    }
    // window
    else if (rLocalName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("module") ))
    {
        mrModuleDesc.aName = xAttributes->getValueByUidName(
            XMLNS_LIBRARY_UID, OUString( RTL_CONSTASCII_USTRINGPARAM("name") ) );
        mrModuleDesc.aLanguage = xAttributes->getValueByUidName(
            XMLNS_XLINK_UID, OUString( RTL_CONSTASCII_USTRINGPARAM("language") ) );

        return new ModuleElement( rLocalName, xAttributes, 0, this );
    }
    else
    {
        throw xml::sax::SAXException(
            OUString( RTL_CONSTASCII_USTRINGPARAM("illegal root element (expected module) given: ") ) +
            rLocalName, Reference< XInterface >(), Any() );
    }
}
//__________________________________________________________________________________________________
ModuleImport::~ModuleImport()
    SAL_THROW( () )
{
#ifdef DEBUG
    OSL_TRACE( "ModuleImport::~ModuleImport().\n" );
#endif
}

//##################################################################################################

SAL_DLLEXPORT Reference< xml::sax::XDocumentHandler >
SAL_CALL importScriptModule( ModuleDescriptor& rMod )
    SAL_THROW( (Exception) )
{
    NameSpaceUid arNamespaceUids[] = {
        NameSpaceUid( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_SCRIPT_URI) ), XMLNS_SCRIPT_UID ),
    };

    return ::xmlscript::createDocumentHandler(
        arNamespaceUids, sizeof(arNamespaceUids) / sizeof(NameSpaceUid),
        -1 /* unknown namespace id */,
        static_cast< xml::XImporter * >( new ModuleImport( rMod ) ) );
}

};


