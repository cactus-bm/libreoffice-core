/*************************************************************************
 *
 *  $RCSfile: fmdocumentclassification.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2004-09-09 10:21:41 $
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

#ifndef SVX_SOURCE_FORM_FMDOCUMENTCLASSIFICATION_HXX
#include "fmdocumentclassification.hxx"
#endif

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_CONTAINER_XCHILD_HPP_
#include <com/sun/star/container/XChild.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCONNECTION_HPP_
#include <com/sun/star/sdbc/XConnection.hpp>
#endif
/** === end UNO includes === **/

#ifndef SVX_DBTOOLSCLIENT_HXX
#include "dbtoolsclient.hxx"
#endif

//........................................................................
namespace svxform
{
//........................................................................

    namespace
    {
        using ::com::sun::star::uno::Reference;
        using ::com::sun::star::uno::XInterface;
        using ::com::sun::star::container::XChild;
        using ::com::sun::star::frame::XModel;
        using ::com::sun::star::uno::UNO_QUERY;

        //....................................................................
        template< class TYPE >
        Reference< TYPE > getTypedModelNode( const Reference< XInterface >& _rxModelNode )
        {
            Reference< TYPE > xTypedNode( _rxModelNode, UNO_QUERY );
            if ( xTypedNode.is() )
                return xTypedNode;
            else
            {
                Reference< XChild > xChild( _rxModelNode, UNO_QUERY );
                if ( xChild.is() )
                    return getTypedModelNode< TYPE >( xChild->getParent() );
                else
                    return NULL;
            }
        }

        //....................................................................
        Reference< XModel > getDocument( const Reference< XInterface >& _rxModelNode )
        {
            return getTypedModelNode< XModel >( _rxModelNode );
        }
    }

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::frame;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::sdbc;

    //====================================================================
    //= DocumentClassification
    //====================================================================
    //--------------------------------------------------------------------
    DocumentType DocumentClassification::classifyDocument( const Reference< XModel >& _rxDocumentModel ) SAL_THROW(())
    {
        DocumentType eType( eUnknownDocumentType );

        OSL_ENSURE( _rxDocumentModel.is(), "DocumentClassification::classifyDocument: invalid document!" );
        if ( !_rxDocumentModel.is() )
            return eType;

        try
        {
            Reference< XConnection > xContextConnection = OStaticDataAccessTools().getComponentContextConnection( _rxDocumentModel );
            if ( xContextConnection.is() )
                eType = eDatabaseForm;
            else
            {
                Reference< XServiceInfo > xSI( _rxDocumentModel, UNO_QUERY_THROW );
                if ( xSI->supportsService( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.text.WebDocument" ) ) ) )
                    eType = eWebDocument;
                else if ( xSI->supportsService( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.text.TextDocument" ) ) ) )
                    eType = eTextDocument;
                else if ( xSI->supportsService( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sheet.SpreadsheetDocument" ) ) ) )
                    eType = eSpreadsheetDocument;
                else if ( xSI->supportsService( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.DrawingDocument" ) ) ) )
                {
                    if ( xSI->supportsService( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.presentation.PresentationDocument" ) ) ) )
                        eType = ePresentationDocument;
                    else
                        eType = eDrawingDocument;
                }
            }
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "DocumentClassification::classifyDocument: caught an exception!" );
        }

        return eType;
    }

    //--------------------------------------------------------------------
    DocumentType DocumentClassification::classifyHostDocument( const Reference< XInterface >& _rxFormComponent ) SAL_THROW(())
    {
        DocumentType eType( eUnknownDocumentType );

        try
        {
            Reference< XModel > xDocument( getDocument( _rxFormComponent.get() ) );
            eType = classifyDocument( xDocument );
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "DocumentClassification::classifyHostDocument: caught an exception!" );
        }

        return eType;
    }

//........................................................................
} // namespace svxform
//........................................................................

