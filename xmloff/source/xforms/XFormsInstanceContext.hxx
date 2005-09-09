/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: XFormsInstanceContext.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 16:03:23 $
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

#ifndef _XMLOFF_XFORMSINSTANCECONTEXT_HXX
#define _XMLOFF_XFORMSINSTANCECONTEXT_HXX


//
// include for parent class and members
//

#ifndef _XMLOFF_TOKENCONTEXT_HXX
#include "TokenContext.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif


//
// forward declarations
//

namespace com { namespace sun { namespace star {
    namespace xml { namespace sax { class XAttributeList; } }
    namespace xml { namespace dom { class XDocument; } }
    namespace beans { class XPropertySet; }
    namespace xforms { class XModel; }
} } }
namespace rtl { class OUString; }
class SvXMLImport;
class SvXMLImportContext;


/** import the xforms:instance element */
class XFormsInstanceContext : public TokenContext
{
    com::sun::star::uno::Reference<com::sun::star::xforms::XModel> mxModel;
    com::sun::star::uno::Reference<com::sun::star::xml::dom::XDocument> mxInstance;
    rtl::OUString msId;
    rtl::OUString msURL;

public:

    XFormsInstanceContext( SvXMLImport& rImport,
                           USHORT nPrfx,
                           const ::rtl::OUString& rLName,
                           com::sun::star::uno::Reference<com::sun::star::beans::XPropertySet> xModel );
    virtual ~XFormsInstanceContext();


    //
    // implement SvXMLImportContext & TokenContext methods:
    // We override CreateChildContext, because we want to read
    // arbitrary DOM elements. For the attributes, we use the
    // TokenContext mechanism.
    //

    virtual SvXMLImportContext* CreateChildContext(
        USHORT nPrefix,
        const rtl::OUString& rLocalName,
        const com::sun::star::uno::Reference<com::sun::star::xml::sax::XAttributeList >& xAttrList );

    virtual void EndElement();

protected:

    virtual void HandleAttribute(
        sal_uInt16 nToken,
        const rtl::OUString& rValue );

    virtual SvXMLImportContext* HandleChild(
        sal_uInt16 nToken,
        sal_uInt16 nNamespace,
        const rtl::OUString& rLocalName,
        const com::sun::star::uno::Reference<com::sun::star::xml::sax::XAttributeList>& xAttrList );
};

#endif
