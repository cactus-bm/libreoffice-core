/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: documentsignaturehelper.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 17:21:45 $
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

#include <xmlsecurity/documentsignaturehelper.hxx>

#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/DisposedException.hpp>

#ifndef _COM_SUN_STAR_EMBED_XSTORAGE_HPP_
#include <com/sun/star/embed/XStorage.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif

#include <tools/debug.hxx>

using namespace ::com::sun::star;


void ImplFillElementList( std::vector< rtl::OUString >& rList, const uno::Reference < embed::XStorage >& rxStore, const ::rtl::OUString rRootStorageName, bool bRecursive )
{
    ::rtl::OUString aMetaInfName( RTL_CONSTASCII_USTRINGPARAM( "META-INF" ) );
    ::rtl::OUString aSep( RTL_CONSTASCII_USTRINGPARAM( "/" ) );

    uno::Reference < container::XNameAccess > xElements( rxStore, uno::UNO_QUERY );
    uno::Sequence< ::rtl::OUString > aElements = xElements->getElementNames();
    sal_Int32 nElements = aElements.getLength();
    const ::rtl::OUString* pNames = aElements.getConstArray();
    for ( sal_Int32 n = 0; n < nElements; n++ )
    {
        if ( pNames[n] != aMetaInfName )
        {
            if ( rxStore->isStreamElement( pNames[n] ) )
            {
                ::rtl::OUString aFullName( rRootStorageName + pNames[n] );
                rList.push_back( aFullName );
            }
            else if ( bRecursive && rxStore->isStorageElement( pNames[n] ) )
            {
                uno::Reference < embed::XStorage > xSubStore = rxStore->openStorageElement( pNames[n], embed::ElementModes::READ );
                rtl::OUString aFullRootName( rRootStorageName + pNames[n] + aSep );
                ImplFillElementList( rList, xSubStore, aFullRootName, bRecursive );
            }
        }
    }
}

std::vector< rtl::OUString > DocumentSignatureHelper::CreateElementList( const uno::Reference < embed::XStorage >& rxStore, const ::rtl::OUString rRootStorageName, DocumentSignatureMode eMode )
{
    std::vector< rtl::OUString > aElements;
    ::rtl::OUString aSep( RTL_CONSTASCII_USTRINGPARAM( "/" ) );


    switch ( eMode )
    {
        case SignatureModeDocumentContent:
        {
            // 1) Main content
            ImplFillElementList( aElements, rxStore, ::rtl::OUString(), false );

            // 2) Pictures...
            rtl::OUString aSubStorageName( rtl::OUString::createFromAscii( "Pictures" ) );
            try
            {
                uno::Reference < embed::XStorage > xSubStore = rxStore->openStorageElement( aSubStorageName, embed::ElementModes::READ );
                ImplFillElementList( aElements, xSubStore, aSubStorageName+aSep, true );
            }
            catch( com::sun::star::io::IOException& )
            {
                ; // Doesn't have to exist...
            }
            // 3) OLE....
            aSubStorageName = rtl::OUString::createFromAscii( "ObjectReplacements" );
            try
            {
                uno::Reference < embed::XStorage > xSubStore = rxStore->openStorageElement( aSubStorageName, embed::ElementModes::READ );
                ImplFillElementList( aElements, xSubStore, aSubStorageName+aSep, true );
                xSubStore.clear();

                // Object folders...
                rtl::OUString aMatchStr( rtl::OUString::createFromAscii( "Object " ) );
                uno::Reference < container::XNameAccess > xElements( rxStore, uno::UNO_QUERY );
                uno::Sequence< ::rtl::OUString > aElementNames = xElements->getElementNames();
                sal_Int32 nElements = aElementNames.getLength();
                const ::rtl::OUString* pNames = aElementNames.getConstArray();
                for ( sal_Int32 n = 0; n < nElements; n++ )
                {
                    if ( ( pNames[n].match( aMatchStr ) ) && rxStore->isStorageElement( pNames[n] ) )
                    {
                        uno::Reference < embed::XStorage > xSubStore = rxStore->openStorageElement( pNames[n], embed::ElementModes::READ );
                        ImplFillElementList( aElements, xSubStore, pNames[n]+aSep, true );
                    }
                }
            }
            catch( com::sun::star::io::IOException& )
            {
                ; // Doesn't have to exist...
            }
        }
        break;
        case SignatureModeMacros:
        {
            // 1) Macros
            rtl::OUString aSubStorageName( rtl::OUString::createFromAscii( "Basic" ) );
            try
            {
                uno::Reference < embed::XStorage > xSubStore = rxStore->openStorageElement( aSubStorageName, embed::ElementModes::READ );
                ImplFillElementList( aElements, xSubStore, aSubStorageName+aSep, true );
            }
            catch( com::sun::star::io::IOException& )
            {
                ; // Doesn't have to exist...
            }

            // 2) Dialogs
            aSubStorageName = rtl::OUString::createFromAscii( "Dialogs") ;
            try
            {
                uno::Reference < embed::XStorage > xSubStore = rxStore->openStorageElement( aSubStorageName, embed::ElementModes::READ );
                ImplFillElementList( aElements, xSubStore, aSubStorageName+aSep, true );
            }
            catch( com::sun::star::io::IOException& )
            {
                ; // Doesn't have to exist...
            }
            // 3) Scripts
            aSubStorageName = rtl::OUString::createFromAscii( "Scripts") ;
            try
            {
                uno::Reference < embed::XStorage > xSubStore = rxStore->openStorageElement( aSubStorageName, embed::ElementModes::READ );
                ImplFillElementList( aElements, xSubStore, aSubStorageName+aSep, true );
            }
            catch( com::sun::star::io::IOException& )
            {
                ; // Doesn't have to exist...
            }
        }
        break;
        case SignatureModePackage:
        {
            // Everything except META-INF
            ImplFillElementList( aElements, rxStore, ::rtl::OUString(), true );
        }
        break;
    }

    return aElements;
}

SignatureStreamHelper DocumentSignatureHelper::OpenSignatureStream( const uno::Reference < embed::XStorage >& rxStore, sal_Int32 nOpenMode, DocumentSignatureMode eDocSigMode )
{
    sal_Int32 nSubStorageOpenMode = embed::ElementModes::READ;
    if ( nOpenMode & embed::ElementModes::WRITE )
        nSubStorageOpenMode = embed::ElementModes::WRITE;

    SignatureStreamHelper aHelper;

    try
    {
        ::rtl::OUString aSIGStoreName( RTL_CONSTASCII_USTRINGPARAM( "META-INF" ) );
        aHelper.xSignatureStorage = rxStore->openStorageElement( aSIGStoreName, nSubStorageOpenMode );
        if ( aHelper.xSignatureStorage.is() )
        {
            ::rtl::OUString aSIGStreamName;
            if ( eDocSigMode == SignatureModeDocumentContent )
                aSIGStreamName = DocumentSignatureHelper::GetDocumentContentSignatureDefaultStreamName();
            else if ( eDocSigMode == SignatureModeMacros )
                aSIGStreamName = DocumentSignatureHelper::GetScriptingContentSignatureDefaultStreamName();
            else
                aSIGStreamName = DocumentSignatureHelper::GetPackageSignatureDefaultStreamName();

            aHelper.xSignatureStream = aHelper.xSignatureStorage->openStreamElement( aSIGStreamName, nOpenMode );
        }
    }
    catch( com::sun::star::io::IOException& )
    {
        // Doesn't have to exist...
        DBG_ASSERT( nOpenMode == embed::ElementModes::READ, "Error creating signature stream..." );
    }

    return aHelper;
}

::rtl::OUString DocumentSignatureHelper::GetDocumentContentSignatureDefaultStreamName()
{
    return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "documentsignatures.xml" ) );
}

::rtl::OUString DocumentSignatureHelper::GetScriptingContentSignatureDefaultStreamName()
{
    return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "macrosignatures.xml" ) );
}

::rtl::OUString DocumentSignatureHelper::GetPackageSignatureDefaultStreamName()
{
    return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "packagesignatures.xml" ) );
}