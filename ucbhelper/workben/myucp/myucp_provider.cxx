/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: myucp_provider.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 17:25:52 $
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
#include "precompiled_ucbhelper.hxx"

/**************************************************************************
                                TODO
 **************************************************************************

 *************************************************************************/

#ifndef _VOS_DIAGNOSE_HXX_
#include <vos/diagnose.hxx>
#endif
#ifndef _UCBHELPER_CONTENTIDENTIFIER_HXX
#include <ucbhelper/contentidentifier.hxx>
#endif

// @@@ Adjust multi-include-protection-ifdef and header file name.
#ifndef _MYUCP_PROVIDER_HXX
#include "myucp_provider.hxx"
#endif
// @@@ Adjust multi-include-protection-ifdef and header file name.
#ifndef _MYUCP_CONTENT_HXX
#include "myucp_content.hxx"
#endif

using namespace com::sun;
using namespace com::sun::star;

// @@@ Adjust namespace name.
using namespace myucp;

//=========================================================================
//=========================================================================
//
// ContentProvider Implementation.
//
//=========================================================================
//=========================================================================

ContentProvider::ContentProvider(
                const uno::Reference< lang::XMultiServiceFactory >& rSMgr )
: ::ucb::ContentProviderImplHelper( rSMgr )
{
}

//=========================================================================
// virtual
ContentProvider::~ContentProvider()
{
}

//=========================================================================
//
// XInterface methods.
//
//=========================================================================

// @@@ Add own interfaces.
XINTERFACE_IMPL_3( ContentProvider,
                   lang::XTypeProvider,
                   lang::XServiceInfo,
                   star::ucb::XContentProvider );

//=========================================================================
//
// XTypeProvider methods.
//
//=========================================================================

// @@@ Add own interfaces.
XTYPEPROVIDER_IMPL_3( ContentProvider,
                      lang::XTypeProvider,
                      lang::XServiceInfo,
                      star::ucb::XContentProvider );

//=========================================================================
//
// XServiceInfo methods.
//
//=========================================================================

// @@@ Adjust implementation name. Keep the prefix "com.sun.star.comp."!
// @@@ Adjust service name.
XSERVICEINFO_IMPL_1( ContentProvider,
                     rtl::OUString::createFromAscii(
                            "com.sun.star.comp.myucp.ContentProvider" ),
                     rtl::OUString::createFromAscii(
                             MYUCP_CONTENT_PROVIDER_SERVICE_NAME ) );

//=========================================================================
//
// Service factory implementation.
//
//=========================================================================

ONE_INSTANCE_SERVICE_FACTORY_IMPL( ContentProvider );

//=========================================================================
//
// XContentProvider methods.
//
//=========================================================================

// virtual
uno::Reference< star::ucb::XContent > SAL_CALL ContentProvider::queryContent(
        const uno::Reference< star::ucb::XContentIdentifier >& Identifier )
    throw( star::ucb::IllegalIdentifierException, uno::RuntimeException )
{
    // Check URL scheme...

    rtl::OUString aScheme( rtl::OUString::createFromAscii( MYUCP_URL_SCHEME ) );
    if ( !Identifier->getContentProviderScheme().equalsIgnoreAsciiCase( aScheme ) )
        throw star::ucb::IllegalIdentifierException();

    // @@@ Further id checks may go here...
#if 0
    if ( id-check-failes )
        throw star::ucb::IllegalIdentifierException();
#endif

    // @@@ Id normalization may go here...
#if 0
    // Normalize URL and create new Id.
    rtl::OUString aCanonicURL = xxxxx( Identifier->getContentIdentifier() );
    uno::Reference< star::ucb::XContentIdentifier > xCanonicId
        = new ::ucb::ContentIdentifier( m_xSMgr, aCanonicURL );
#else
    uno::Reference< star::ucb::XContentIdentifier > xCanonicId = Identifier;
#endif

    vos::OGuard aGuard( m_aMutex );

    // Check, if a content with given id already exists...
    uno::Reference< star::ucb::XContent > xContent
        = queryExistingContent( xCanonicId ).getBodyPtr();
    if ( xContent.is() )
        return xContent;

    // @@@ Decision, which content implementation to instanciate may be
    //     made here ( in case you have different content classes ).

    // Create a new content. Note that the content will insert itself
    // into providers content list by calling addContent(...) from it's ctor.

    xContent = new Content( m_xSMgr, this, xCanonicId );

    if ( !xContent->getIdentifier().is() )
        throw star::ucb::IllegalIdentifierException();

    return xContent;
}

