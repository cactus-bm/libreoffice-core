/*************************************************************************
 *
 *  $RCSfile: xml_import.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: dbo $ $Date: 2001-04-04 14:35:07 $
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
#ifndef _XMLSCRIPT_XML_IMPORT_HXX_
#define _XMLSCRIPT_XML_IMPORT_HXX_

#ifndef _COM_SUN_STAR_XML_XIMPORTER_HPP_
#include <com/sun/star/xml/XImporter.hpp>
#endif
#ifndef _COM_SUN_STAR_XML_SAX_XDOCUMENTHANDLER_HPP_
#include <com/sun/star/xml/sax/XExtendedDocumentHandler.hpp>
#endif


namespace xmlscript
{

/*##################################################################################################

    IMPORTING

##################################################################################################*/

//==================================================================================================
struct NameSpaceUid
{
    /** URI defining XML namespace
    */
    ::rtl::OUString     sURI;
    /** Identifier given for URI (given back in createRootContext(), createChildContext() callbacks
    */
    sal_Int32           nUid;

    inline NameSpaceUid( ::rtl::OUString const & sURI_, sal_Int32 nUid_ ) SAL_THROW( () )
        : sURI( sURI_ )
        , nUid( nUid_ )
        {}
};

/** Creates a document handler to be used for SAX1 parser that can handle namespaces.
    Give a list of NameSpaceUid structs defining namespace mappings to integers (performance).
    Implementing the XImporter interface, you will get a createRootContext() for the root
    element of your XML document and subsequent createChildContext() callbacks for each
    sub element.
    Namespaces of tags are identified by their integer value.

    @param pNamespaceUids
           array of namespace mappings
    @param nNameSpaceUids
           number of element in namespace mappings array
    @param nUnknownNamespaceUid
           namespace id given for unrecognized namespace prefix
           (one that is not given via pNamespaceUids)
    @param xImporter
           initial import object being called for root context
    @param bSingleThreadedUse
           flag whether context management is synchronized.
    @return
            document handler for parser
*/
::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler >
SAL_CALL createDocumentHandler(
    NameSpaceUid const * pNamespaceUids, sal_Int32 nNameSpaceUids,
    sal_Int32 nUnknownNamespaceUid,
    ::com::sun::star::uno::Reference< ::com::sun::star::xml::XImporter > const & xImporter,
    bool bSingleThreadedUse = true )
    SAL_THROW( () );

}

#endif
