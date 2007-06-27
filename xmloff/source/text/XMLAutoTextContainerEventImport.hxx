/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: XMLAutoTextContainerEventImport.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 15:50:59 $
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

#ifndef _XMLOFF_XMLAUTOTEXTCONTAINEREVENTIMPORT_HXX
#define _XMLOFF_XMLAUTOTEXTCONTAINEREVENTIMPORT_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include <xmloff/xmlictxt.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif


namespace rtl { class OUString; }
namespace com { namespace sun { namespace star {
    namespace container { class XNameReplace; }
    namespace xml { namespace sax { class XAttributeList; } }
} } }


/**
 * Import the text:auto-text-container element.
 * This only instantiates text:auto-text-group contexta.
 */
class XMLAutoTextContainerEventImport : public SvXMLImportContext
{
    /// the parent auto text container
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::container::XNameReplace> & rEvents;

public:

    TYPEINFO();

    XMLAutoTextContainerEventImport(
        SvXMLImport& rImport,
        USHORT nPrfx,
        const ::rtl::OUString& rLName,
        const ::com::sun::star::uno::Reference<
                ::com::sun::star::container::XNameReplace > & rEvents );

    ~XMLAutoTextContainerEventImport();


protected:

    virtual SvXMLImportContext *CreateChildContext(
        USHORT nPrefix,
        const ::rtl::OUString& rLocalName,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XAttributeList> & xAttrList );
};

#endif
