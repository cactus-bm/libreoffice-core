/*************************************************************************
 *
 *  $RCSfile: persistence.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: kz $ $Date: 2004-10-04 19:49:49 $
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

#include <commonembobj.hxx>

#ifndef _COM_SUN_STAR_DOCUMENT_XSTORAGEBASEDDOCUMENT_HPP_
#include <com/sun/star/document/XStorageBasedDocument.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_EMBEDSTATES_HPP_
#include <com/sun/star/embed/EmbedStates.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_EMBEDVERBS_HPP_
#include <com/sun/star/embed/EmbedVerbs.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_ENTRYINITMODES_HPP_
#include <com/sun/star/embed/EntryInitModes.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XSTORAGE_HPP_
#include <com/sun/star/embed/XStorage.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_EMBEDUPDATEMODES_HPP_
#include <com/sun/star/embed/EmbedUpdateModes.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTORABLE_HPP_
#include <com/sun/star/frame/XStorable.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XLOADABLE_HPP_
#include <com/sun/star/frame/XLoadable.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCOMPONENTLOADER_HPP_
#include <com/sun/star/frame/XComponentLoader.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_XMODIFIABLE_HPP_
#include <com/sun/star/util/XModifiable.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCEESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_XCLOSEABLE_HPP_
#include <com/sun/star/util/XCloseable.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#include <comphelper/fileformat.h>
#include <comphelper/storagehelper.hxx>

#include "convert.hxx"

using namespace ::com::sun::star;


//------------------------------------------------------
uno::Sequence< beans::PropertyValue > GetValuableArgs_Impl( const uno::Sequence< beans::PropertyValue >& aMedDescr,
                                                            sal_Bool bCanUseDocumentBaseURL )
{
    uno::Sequence< beans::PropertyValue > aResult;
    sal_Int32 nResLen = 0;

    for ( sal_Int32 nInd = 0; nInd < aMedDescr.getLength(); nInd++ )
    {
        if ( aMedDescr[nInd].Name.equalsAscii( "ComponentData" )
          || aMedDescr[nInd].Name.equalsAscii( "DocumentTitle" )
          || aMedDescr[nInd].Name.equalsAscii( "InteractionHandler" )
          || aMedDescr[nInd].Name.equalsAscii( "JumpMark" )
          // || aMedDescr[nInd].Name.equalsAscii( "Password" ) makes no sence for embedded objects
          || aMedDescr[nInd].Name.equalsAscii( "Preview" )
          || aMedDescr[nInd].Name.equalsAscii( "ReadOnly" )
          || aMedDescr[nInd].Name.equalsAscii( "StartPresentation" )
          || aMedDescr[nInd].Name.equalsAscii( "RepairPackage" )
          || aMedDescr[nInd].Name.equalsAscii( "StatusIndicator" )
          || aMedDescr[nInd].Name.equalsAscii( "ViewData" )
          || aMedDescr[nInd].Name.equalsAscii( "ViewId" )
          || aMedDescr[nInd].Name.equalsAscii( "MacroExecutionMode" )
          || aMedDescr[nInd].Name.equalsAscii( "UpdateDocMode" )
          || aMedDescr[nInd].Name.equalsAscii( "DocumentBaseURL" ) && bCanUseDocumentBaseURL )
        {
            aResult.realloc( ++nResLen );
            aResult[nResLen-1] = aMedDescr[nInd];
        }
    }

    return aResult;
}

//------------------------------------------------------
uno::Sequence< beans::PropertyValue > addAsTemplate( const uno::Sequence< beans::PropertyValue >& aOrig )
{
    sal_Bool bAsTemplateSet = sal_False;
    sal_Int32 nLength = aOrig.getLength();
    uno::Sequence< beans::PropertyValue > aResult( nLength );

    for ( sal_Int32 nInd = 0; nInd < nLength; nInd++ )
    {
        aResult[nInd].Name = aOrig[nInd].Name;
        if ( aResult[nInd].Name.equalsAscii( "AsTemplate" ) )
        {
            aResult[nInd].Value <<= sal_True;
            bAsTemplateSet = sal_True;
        }
        else
            aResult[nInd].Value = aOrig[nInd].Value;
    }

    if ( !bAsTemplateSet )
    {
        aResult.realloc( nLength + 1 );
        aResult[nLength].Name = ::rtl::OUString::createFromAscii( "AsTemplate" );
        aResult[nLength].Value <<= sal_True;
    }

    return aResult;
}

//------------------------------------------------------
uno::Reference< io::XInputStream > createTempInpStreamFromStor(
                                                            const uno::Reference< embed::XStorage >& xStorage,
                                                            const uno::Reference< lang::XMultiServiceFactory >& xFactory )
{
    OSL_ENSURE( xStorage.is(), "The storage can not be empty!" );

    uno::Reference< io::XInputStream > xResult;

    const ::rtl::OUString aServiceName ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.io.TempFile" ) );
    uno::Reference < io::XStream > xTempStream = uno::Reference < io::XStream > (
                                                            xFactory->createInstance ( aServiceName ),
                                                            uno::UNO_QUERY );
    if ( xTempStream.is() )
    {
        uno::Reference < lang::XSingleServiceFactory > xStorageFactory(
                    xFactory->createInstance ( ::rtl::OUString::createFromAscii( "com.sun.star.embed.StorageFactory" ) ),
                    uno::UNO_QUERY );

        uno::Sequence< uno::Any > aArgs( 2 );
        aArgs[0] <<= xTempStream;
        aArgs[1] <<= embed::ElementModes::READWRITE;
        uno::Reference< embed::XStorage > xTempStorage( xStorageFactory->createInstanceWithArguments( aArgs ),
                                                        uno::UNO_QUERY );
        if ( !xTempStorage.is() )
            throw uno::RuntimeException(); // TODO:

        try
        {
            xStorage->copyToStorage( xTempStorage );
        } catch( uno::Exception& e )
        {
            throw embed::StorageWrappedTargetException(
                        ::rtl::OUString::createFromAscii( "Can't copy storage!" ),
                        uno::Reference< uno::XInterface >(),
                        uno::makeAny( e ) );
        }

        try {
            uno::Reference< lang::XComponent > xComponent( xTempStorage, uno::UNO_QUERY );
            OSL_ENSURE( xComponent.is(), "Wrong storage implementation!" );
            if ( xComponent.is() )
                xComponent->dispose();
        }
        catch ( uno::Exception& )
        {
        }

        try {
            uno::Reference< io::XOutputStream > xTempOut = xTempStream->getOutputStream();
            if ( xTempOut.is() )
                xTempOut->closeOutput();
        }
        catch ( uno::Exception& )
        {
        }

        xResult = xTempStream->getInputStream();
    }

    return xResult;

}

//------------------------------------------------------
static void SetDocToEmbedded( const uno::Reference< frame::XModel > xDocument )
{
    if ( xDocument.is() )
    {
        uno::Sequence< beans::PropertyValue > aSeq( 1 );
        aSeq[0].Name = ::rtl::OUString::createFromAscii( "SetEmbedded" );
        aSeq[0].Value <<= sal_True;
        xDocument->attachResource( ::rtl::OUString(), aSeq );
    }
}

//------------------------------------------------------
void OCommonEmbeddedObject::SwitchOwnPersistence( const uno::Reference< embed::XStorage >& xNewParentStorage,
                                                  const uno::Reference< embed::XStorage >& xNewObjectStorage,
                                                  const ::rtl::OUString& aNewName )
{
    if ( xNewParentStorage == m_xParentStorage && aNewName.equals( m_aEntryName ) )
    {
        OSL_ENSURE( xNewObjectStorage == m_xObjectStorage, "The storage must be the same!\n" );
        return;
    }

    uno::Reference< lang::XComponent > xComponent( m_xObjectStorage, uno::UNO_QUERY );
    OSL_ENSURE( !m_xObjectStorage.is() || xComponent.is(), "Wrong storage implementation!" );

    m_xObjectStorage = xNewObjectStorage;
    m_xParentStorage = xNewParentStorage;
    m_aEntryName = aNewName;

#ifdef USE_STORAGEBASED_DOCUMENT
    uno::Reference< document::XStorageBasedDocument > xDoc( m_pDocHolder->GetComponent(), uno::UNO_QUERY );
    if ( xDoc.is() )
        xDoc->switchToStorage( m_xObjectStorage );
#endif

    try {
        if ( xComponent.is() )
            xComponent->dispose();
    }
    catch ( uno::Exception& )
    {
    }
}

//------------------------------------------------------
void OCommonEmbeddedObject::SwitchOwnPersistence( const uno::Reference< embed::XStorage >& xNewParentStorage,
                                                  const ::rtl::OUString& aNewName )
{
    if ( xNewParentStorage == m_xParentStorage && aNewName.equals( m_aEntryName ) )
        return;

    sal_Int32 nStorageMode = m_bReadOnly ? embed::ElementModes::READ : embed::ElementModes::READWRITE;

    uno::Reference< embed::XStorage > xNewOwnStorage = xNewParentStorage->openStorageElement( aNewName, nStorageMode );
    OSL_ENSURE( xNewOwnStorage.is(), "The method can not return empty reference!" );

    SwitchOwnPersistence( xNewParentStorage, xNewOwnStorage, aNewName );
}

//------------------------------------------------------
uno::Reference< util::XCloseable > OCommonEmbeddedObject::InitNewDocument_Impl()
{
    uno::Reference< util::XCloseable > xDocument( m_xFactory->createInstance( GetDocumentServiceName() ),
                                                uno::UNO_QUERY );

    uno::Reference< frame::XModel > xModel( xDocument, uno::UNO_QUERY );
    uno::Reference< frame::XLoadable > xLoadable( xModel, uno::UNO_QUERY );
    if ( !xLoadable.is() )
        throw uno::RuntimeException();

    try
    {
        // set the document mode to embedded
        SetDocToEmbedded( xModel );

        // init document as a new
        xLoadable->initNew();
        xModel->attachResource( xModel->getURL(),m_aDocMediaDescriptor);
    }
    catch( uno::Exception& )
    {
        uno::Reference< util::XCloseable > xCloseable( xDocument, uno::UNO_QUERY );
        if ( xCloseable.is() )
        {
            try
            {
                xCloseable->close( sal_True );
            }
            catch( uno::Exception& )
            {
            }
        }

        throw; // TODO
    }

    return xDocument;
}

//------------------------------------------------------
uno::Reference< util::XCloseable > OCommonEmbeddedObject::LoadLink_Impl()
{
    uno::Reference< util::XCloseable > xDocument( m_xFactory->createInstance( GetDocumentServiceName() ),
                                                uno::UNO_QUERY );

    uno::Reference< frame::XLoadable > xLoadable( xDocument, uno::UNO_QUERY );
    if ( !xLoadable.is() )
        throw uno::RuntimeException();

    uno::Sequence< beans::PropertyValue > aArgs( 2 );
    aArgs[0].Name = ::rtl::OUString::createFromAscii( "URL" );
    aArgs[0].Value <<= m_aLinkURL;
    aArgs[1].Name = ::rtl::OUString::createFromAscii( "FilterName" );
    aArgs[1].Value <<= m_aLinkFilterName;

    try
    {
        // the document is not really an embedded one, it is a link
        SetDocToEmbedded( uno::Reference < frame::XModel >( xDocument, uno::UNO_QUERY ) );

        // load the document
        xLoadable->load( aArgs );
    }
    catch( uno::Exception& )
    {
        uno::Reference< util::XCloseable > xCloseable( xDocument, uno::UNO_QUERY );
        if ( xCloseable.is() )
        {
            try
            {
                xCloseable->close( sal_True );
            }
            catch( uno::Exception& )
            {
            }
        }

        throw; // TODO
    }

    return xDocument;

}

//------------------------------------------------------
uno::Reference< util::XCloseable > OCommonEmbeddedObject::LoadDocumentFromStorage_Impl(
                                                            const uno::Reference< embed::XStorage >& xStorage )
{
    OSL_ENSURE( xStorage.is(), "The storage can not be empty!" );

    uno::Reference< util::XCloseable >  xDocument( m_xFactory->createInstance( GetDocumentServiceName() ), uno::UNO_QUERY );
    uno::Reference< frame::XLoadable > xLoadable( xDocument, uno::UNO_QUERY );
    uno::Reference< document::XStorageBasedDocument > xDoc
#ifdef USE_STORAGEBASED_DOCUMENT
            ( xDocument, uno::UNO_QUERY )
#endif
            ;
    if ( !xDoc.is() && !xLoadable.is() )
        throw uno::RuntimeException();

    ::rtl::OUString aFilterName;
    // TODO/LATER: the filter will be provided from outside, factory will set it in object props
    try {
        aFilterName = GetDefaultFilterFromServName( GetDocumentServiceName(),
                                                    ::comphelper::OStorageHelper::GetXStorageFormat( xStorage ) );
    } catch( uno::Exception& )
    {}

    OSL_ENSURE( aFilterName.getLength(), "Wrong document service name!" );
    if ( !aFilterName.getLength() )
        throw io::IOException();

    sal_Int32 nLen = xDoc.is() ? 4 : 6;
    uno::Sequence< beans::PropertyValue > aArgs( nLen );

    aArgs[0].Name = ::rtl::OUString::createFromAscii( "DocumentBaseURL" );
    aArgs[0].Value <<= GetBaseURL_Impl();
    aArgs[1].Name = ::rtl::OUString::createFromAscii( "HierarchicalDocumentName" );
    aArgs[1].Value <<= m_aEntryName;
    aArgs[2].Name = ::rtl::OUString::createFromAscii( "ReadOnly" );
    aArgs[2].Value <<= m_bReadOnly;
    aArgs[3].Name = ::rtl::OUString::createFromAscii( "FilterName" );
    aArgs[3].Value <<= aFilterName;
    if ( !xDoc.is() )
    {
        uno::Reference< io::XInputStream > xTempInpStream = createTempInpStreamFromStor( xStorage, m_xFactory );
        if ( !xTempInpStream.is() )
            throw uno::RuntimeException();

        ::rtl::OUString aTempFileURL;
        try
        {
            // no need to let the file stay after the stream is removed since the embedded document
            // can not be stored directly
            uno::Reference< beans::XPropertySet > xTempStreamProps( xTempInpStream, uno::UNO_QUERY_THROW );
            xTempStreamProps->getPropertyValue( ::rtl::OUString::createFromAscii( "Uri" ) ) >>= aTempFileURL;
        }
        catch( uno::Exception& )
        {
        }

        OSL_ENSURE( aTempFileURL.getLength(), "Coudn't retrieve temporary file URL!\n" );

        aArgs[4].Name = ::rtl::OUString::createFromAscii( "URL" );
        aArgs[4].Value <<= aTempFileURL; // ::rtl::OUString::createFromAscii( "private:stream" );
        aArgs[5].Name = ::rtl::OUString::createFromAscii( "InputStream" );
        aArgs[5].Value <<= xTempInpStream;
    }

    // aArgs[4].Name = ::rtl::OUString::createFromAscii( "AsTemplate" );
    // aArgs[4].Value <<= sal_True;

    for ( sal_Int32 nInd = 0; nInd < m_aDocMediaDescriptor.getLength(); nInd++ )
    {
        aArgs.realloc( nInd + nLen );
        aArgs[nInd+nLen-1].Name = m_aDocMediaDescriptor[nInd].Name;
        aArgs[nInd+nLen-1].Value = m_aDocMediaDescriptor[nInd].Value;
    }

    try
    {
        // set the document mode to embedded
        SetDocToEmbedded( uno::Reference < frame::XModel >( xDocument, uno::UNO_QUERY ) );
        if ( xDoc.is() )
            xDoc->loadFromStorage( xStorage, aArgs );
        else
            xLoadable->load( aArgs );
    }
    catch( uno::Exception& )
    {
        uno::Reference< util::XCloseable > xCloseable( xDocument, uno::UNO_QUERY );
        if ( xCloseable.is() )
        {
            try
            {
                xCloseable->close( sal_True );
            }
            catch( uno::Exception& )
            {
            }
        }

        throw; // TODO
    }

    return xDocument;
}

//------------------------------------------------------
uno::Reference< io::XInputStream > OCommonEmbeddedObject::StoreDocumentToTempStream_Impl(
                                                                            sal_Int32 nStorageFormat,
                                                                            const ::rtl::OUString& aBaseURL,
                                                                            const ::rtl::OUString& aHierarchName )
{
    uno::Reference < io::XOutputStream > xTempOut(
                m_xFactory->createInstance ( ::rtl::OUString::createFromAscii( "com.sun.star.io.TempFile" ) ),
                uno::UNO_QUERY );
    uno::Reference< io::XInputStream > aResult( xTempOut, uno::UNO_QUERY );

    if ( !xTempOut.is() || !aResult.is() )
        throw uno::RuntimeException(); // TODO:

    uno::Reference< frame::XStorable > xStorable( m_pDocHolder->GetComponent(), uno::UNO_QUERY );
    if( !xStorable.is() )
        throw uno::RuntimeException(); // TODO:

    ::rtl::OUString aFilterName;
    try {
        // TODO/LATER: the filter must be provided from outside in future
        aFilterName = GetDefaultFilterFromServName( GetDocumentServiceName(),
                                                    nStorageFormat );
    }
    catch( uno::Exception& )
    {
    }

    OSL_ENSURE( aFilterName.getLength(), "Wrong document service name!" );
    if ( !aFilterName.getLength() )
        throw io::IOException(); // TODO:

    uno::Sequence< beans::PropertyValue > aArgs( 4 );
    aArgs[0].Name = ::rtl::OUString::createFromAscii( "FilterName" );
    aArgs[0].Value <<= aFilterName;
    aArgs[1].Name = ::rtl::OUString::createFromAscii( "OutputStream" );
    aArgs[1].Value <<= xTempOut;
    aArgs[2].Name = ::rtl::OUString::createFromAscii( "DocumentBaseURL" );
    aArgs[2].Value <<= aBaseURL;
    aArgs[3].Name = ::rtl::OUString::createFromAscii( "HierarchicalDocumentName" );
    aArgs[3].Value <<= aHierarchName;

    xStorable->storeToURL( ::rtl::OUString::createFromAscii( "private:stream" ), aArgs );
    try
    {
        xTempOut->closeOutput();
    }
    catch( uno::Exception& )
    {
        OSL_ENSURE( sal_False, "Looks like stream was closed already" );
    }

    return aResult;
}

//------------------------------------------------------
void OCommonEmbeddedObject::SaveObject_Impl()
{
    if ( m_xClientSite.is() )
    {
        try {
            m_xClientSite->saveObject();
        }
        catch( uno::Exception& )
        {
            OSL_ENSURE( sal_False, "The object was not stored!\n" );
        }
    }
}

//------------------------------------------------------
::rtl::OUString OCommonEmbeddedObject::GetBaseURL_Impl()
{
    ::rtl::OUString aBaseURL;
    sal_Int32 nInd = 0;

    if ( m_xClientSite.is() )
    {
        try
        {
            uno::Reference< frame::XModel > xParentModel( m_xClientSite->getComponent(), uno::UNO_QUERY_THROW );
            uno::Sequence< beans::PropertyValue > aModelProps = xParentModel->getArgs();
            for ( nInd = 0; nInd < aModelProps.getLength(); nInd++ )
                if ( aModelProps[nInd].Name.equals(
                                                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DocumentBaseURL" ) ) ) )
                {
                    aModelProps[nInd].Value >>= aBaseURL;
                    break;
                }


        }
        catch( uno::Exception& )
        {}
    }

    if ( !aBaseURL.getLength() )
    {
        for ( nInd = 0; nInd < m_aDocMediaDescriptor.getLength(); nInd++ )
            if ( m_aDocMediaDescriptor[nInd].Name.equals(
                                                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DocumentBaseURL" ) ) ) )
            {
                m_aDocMediaDescriptor[nInd].Value >>= aBaseURL;
                break;
            }
    }

    if ( !aBaseURL.getLength() )
        aBaseURL = m_aDefaultParentBaseURL;

    return aBaseURL;
}

//------------------------------------------------------
::rtl::OUString OCommonEmbeddedObject::GetBaseURLFrom_Impl(
                    const uno::Sequence< beans::PropertyValue >& lArguments,
                    const uno::Sequence< beans::PropertyValue >& lObjArgs )
{
    ::rtl::OUString aBaseURL;
    sal_Int32 nInd = 0;

    for ( nInd = 0; nInd < lArguments.getLength(); nInd++ )
        if ( lArguments[nInd].Name.equals( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DocumentBaseURL" ) ) ) )
        {
            lArguments[nInd].Value >>= aBaseURL;
            break;
        }

    if ( !aBaseURL.getLength() )
    {
        for ( nInd = 0; nInd < lObjArgs.getLength(); nInd++ )
            if ( lObjArgs[nInd].Name.equals( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DefaultParentBaseURL" ) ) ) )
            {
                lObjArgs[nInd].Value >>= aBaseURL;
                break;
            }
    }

    return aBaseURL;
}


//------------------------------------------------------
void OCommonEmbeddedObject::StoreDocToStorage_Impl( const uno::Reference< embed::XStorage >& xStorage,
                                                    sal_Int32 nStorageFormat,
                                                    const ::rtl::OUString& aBaseURL,
                                                    const ::rtl::OUString& aHierarchName )
{
    OSL_ENSURE( xStorage.is(), "No storage is provided for storing!" );

    if ( !xStorage.is() )
        throw uno::RuntimeException(); // TODO:

#ifdef USE_STORAGEBASED_DOCUMENT
    uno::Reference< document::XStorageBasedDocument > xDoc( m_pDocHolder->GetComponent(), uno::UNO_QUERY );
    if ( xDoc.is() )
    {
        ::rtl::OUString aFilterName;
        try {
            // TODO/LATER: the filter must be provided from outside in future
            aFilterName = GetDefaultFilterFromServName( GetDocumentServiceName(),
                                                        nStorageFormat );
        }
        catch( uno::Exception& )
        {
        }

        OSL_ENSURE( aFilterName.getLength(), "Wrong document service name!" );
        if ( !aFilterName.getLength() )
            throw io::IOException(); // TODO:

        uno::Sequence< beans::PropertyValue > aArgs( 3 );
        aArgs[0].Name = ::rtl::OUString::createFromAscii( "FilterName" );
        aArgs[0].Value <<= aFilterName;
        aArgs[2].Name = ::rtl::OUString::createFromAscii( "DocumentBaseURL" );
        aArgs[2].Value <<= aBaseURL;
        aArgs[1].Name = ::rtl::OUString::createFromAscii( "HierarchicalDocumentName" );
        aArgs[1].Value <<= aHierarchName;

        xDoc->storeToStorage( xStorage, aArgs );
    }
    else
#endif
    {
        // store document to temporary stream based on temporary file
        uno::Reference < io::XInputStream > xTempIn = StoreDocumentToTempStream_Impl( nStorageFormat, aBaseURL, aHierarchName );
        OSL_ENSURE( xTempIn.is(), "The stream reference can not be empty!\n" );

        // open storage based on document temporary file for reading
        uno::Reference < lang::XSingleServiceFactory > xStorageFactory(
                    m_xFactory->createInstance ( ::rtl::OUString::createFromAscii( "com.sun.star.embed.StorageFactory" ) ),
                    uno::UNO_QUERY );

        uno::Sequence< uno::Any > aArgs(1);
        aArgs[0] <<= xTempIn;
        uno::Reference< embed::XStorage > xTempStorage( xStorageFactory->createInstanceWithArguments( aArgs ),
                                                            uno::UNO_QUERY );
        if ( !xTempStorage.is() )
            throw uno::RuntimeException(); // TODO:

        // object storage must be commited automatically
        xTempStorage->copyToStorage( xStorage );
    }
}

//------------------------------------------------------
uno::Reference< util::XCloseable > OCommonEmbeddedObject::CreateDocFromMediaDescr_Impl(
                                        const uno::Sequence< beans::PropertyValue >& aMedDescr )
{
    uno::Reference< util::XCloseable > xDocument( m_xFactory->createInstance( GetDocumentServiceName() ),
                                                uno::UNO_QUERY );

    uno::Reference< frame::XLoadable > xLoadable( xDocument, uno::UNO_QUERY );
    if ( !xLoadable.is() )
        throw uno::RuntimeException();

    try
    {
        // set the document mode to embedded
        SetDocToEmbedded( uno::Reference < frame::XModel >( xDocument, uno::UNO_QUERY ) );

        xLoadable->load( addAsTemplate( aMedDescr ) );
    }
    catch( uno::Exception& )
    {
        uno::Reference< util::XCloseable > xCloseable( xDocument, uno::UNO_QUERY );
        if ( xCloseable.is() )
        {
            try
            {
                xCloseable->close( sal_True );
            }
            catch( uno::Exception& )
            {
            }
        }

        throw; // TODO
    }

    return xDocument;
}

//------------------------------------------------------
uno::Reference< util::XCloseable > OCommonEmbeddedObject::CreateTempDocFromLink_Impl()
{
    uno::Reference< util::XCloseable > xResult;

    OSL_ENSURE( m_bIsLink, "The object is not a linked one!\n" );

    uno::Sequence< beans::PropertyValue > aTempMediaDescr;

    sal_Int32 nStorageFormat = SOFFICE_FILEFORMAT_CURRENT;
    try {
        nStorageFormat = ::comphelper::OStorageHelper::GetXStorageFormat( m_xParentStorage );
    }
    catch ( uno::Exception& )
    {
        OSL_ENSURE( sal_False, "Can not retrieve storage media type!\n" );
    }


    if ( m_pDocHolder->GetComponent().is() )
    {
        aTempMediaDescr.realloc( 3 );

        // TODO/LATER: may be private:stream should be used as target URL
        ::rtl::OUString aTempFileURL;
        uno::Reference< io::XInputStream > xTempStream = StoreDocumentToTempStream_Impl( SOFFICE_FILEFORMAT_CURRENT,
                                                                                         ::rtl::OUString(),
                                                                                         ::rtl::OUString() );
        try
        {
            // no need to let the file stay after the stream is removed since the embedded document
            // can not be stored directly
            uno::Reference< beans::XPropertySet > xTempStreamProps( xTempStream, uno::UNO_QUERY_THROW );
            xTempStreamProps->getPropertyValue( ::rtl::OUString::createFromAscii( "Uri" ) ) >>= aTempFileURL;
        }
        catch( uno::Exception& )
        {
        }

        OSL_ENSURE( aTempFileURL.getLength(), "Coudn't retrieve temporary file URL!\n" );

        aTempMediaDescr[0].Name = ::rtl::OUString::createFromAscii( "URL" );
        aTempMediaDescr[0].Value <<= aTempFileURL;
        aTempMediaDescr[1].Name = ::rtl::OUString::createFromAscii( "InputStream" );
        aTempMediaDescr[1].Value <<= xTempStream;
        aTempMediaDescr[2].Name = ::rtl::OUString::createFromAscii( "FilterName" );
        // TODO/LATER: the filter must be provided from outside in future
        aTempMediaDescr[2].Value <<= GetDefaultFilterFromServName(
                                                    GetDocumentServiceName(),
                                                    nStorageFormat );
        aTempMediaDescr[3].Name = ::rtl::OUString::createFromAscii( "AsTemplate" );
        aTempMediaDescr[3].Value <<= sal_True;
    }
    else
    {
        aTempMediaDescr.realloc( 2 );
        aTempMediaDescr[0].Name = ::rtl::OUString::createFromAscii( "URL" );
        aTempMediaDescr[0].Value <<= m_aLinkURL;
        aTempMediaDescr[1].Name = ::rtl::OUString::createFromAscii( "FilterName" );
        aTempMediaDescr[1].Value <<= m_aLinkFilterName;
        // aTempMediaDescr[2].Name = ::rtl::OUString::createFromAscii( "AsTemplate" );
        // aTempMediaDescr[2].Value <<= sal_True;
    }

    xResult = CreateDocFromMediaDescr_Impl( aTempMediaDescr );

    return xResult;
}

//------------------------------------------------------
void SAL_CALL OCommonEmbeddedObject::setPersistentEntry(
                    const uno::Reference< embed::XStorage >& xStorage,
                    const ::rtl::OUString& sEntName,
                    sal_Int32 nEntryConnectionMode,
                    const uno::Sequence< beans::PropertyValue >& lArguments,
                    const uno::Sequence< beans::PropertyValue >& lObjArgs )
        throw ( lang::IllegalArgumentException,
                embed::WrongStateException,
                io::IOException,
                uno::Exception,
                uno::RuntimeException )
{
    // the type of the object must be already set
    // a kind of typedetection should be done in the factory

    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    if ( !xStorage.is() )
        throw lang::IllegalArgumentException( ::rtl::OUString::createFromAscii( "No parent storage is provided!\n" ),
                                            uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ),
                                            1 );

    if ( !sEntName.getLength() )
        throw lang::IllegalArgumentException( ::rtl::OUString::createFromAscii( "Empty element name is provided!\n" ),
                                            uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ),
                                            2 );

    // May be LOADED should be forbidden here ???
    if ( ( m_nObjectState != -1 || nEntryConnectionMode == embed::EntryInitModes::NO_INIT )
      && ( m_nObjectState == -1 || nEntryConnectionMode != embed::EntryInitModes::NO_INIT ) )
    {
        // if the object is not loaded
        // it can not get persistant representation without initialization

        // if the object is loaded
        // it can switch persistant representation only without initialization

        throw embed::WrongStateException(
                    ::rtl::OUString::createFromAscii( "Can't change persistant representation of activated object!\n" ),
                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );
    }

    if ( m_bWaitSaveCompleted )
    {
        if ( nEntryConnectionMode == embed::EntryInitModes::NO_INIT )
            saveCompleted( ( m_xParentStorage != xStorage || !m_aEntryName.equals( sEntName ) ) );
        else
            throw embed::WrongStateException(
                        ::rtl::OUString::createFromAscii( "The object waits for saveCompleted() call!\n" ),
                        uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );
    }

    OSL_ENSURE( !m_bIsLink, "This method implementation must not be used for links!\n" );
    if ( m_bIsLink )
        return;

    uno::Reference< container::XNameAccess > xNameAccess( xStorage, uno::UNO_QUERY );
    if ( !xNameAccess.is() )
        throw uno::RuntimeException(); //TODO

    // detect entry existence
    sal_Bool bElExists = xNameAccess->hasByName( sEntName );

    m_aDocMediaDescriptor = GetValuableArgs_Impl( lArguments,
                                                  nEntryConnectionMode != embed::EntryInitModes::MEDIA_DESCRIPTOR_INIT );

    m_bReadOnly = sal_False;
    for ( sal_Int32 nInd = 0; nInd < lArguments.getLength(); nInd++ )
        if ( lArguments[nInd].Name.equalsAscii( "ReadOnly" ) )
            lArguments[nInd].Value >>= m_bReadOnly;

    // TODO: use lObjArgs for StoreVisualReplacement
    for ( sal_Int32 nObjInd = 0; nObjInd < lObjArgs.getLength(); nObjInd++ )
        if ( lObjArgs[nObjInd].Name.equalsAscii( "OutplaceDispatchInterceptor" ) )
        {
            uno::Reference< frame::XDispatchProviderInterceptor > xDispatchInterceptor;
            if ( lObjArgs[nObjInd].Value >>= xDispatchInterceptor )
                m_pDocHolder->SetOutplaceDispatchInterceptor( xDispatchInterceptor );
        }
        else if ( lObjArgs[nObjInd].Name.equalsAscii( "DefaultParentBaseURL" ) )
        {
            lObjArgs[nObjInd].Value >>= m_aDefaultParentBaseURL;
        }

    sal_Int32 nStorageMode = m_bReadOnly ? embed::ElementModes::READ : embed::ElementModes::READWRITE;

    SwitchOwnPersistence( xStorage, sEntName );

    if ( nEntryConnectionMode == embed::EntryInitModes::DEFAULT_INIT )
    {
        if ( bElExists )
        {
            // the initialization from existing storage allows to leave object in loaded state
            m_nObjectState = embed::EmbedStates::LOADED;
        }
        else
        {
            m_pDocHolder->SetComponent( InitNewDocument_Impl(), m_bReadOnly );
            if ( !m_pDocHolder->GetComponent().is() )
                throw io::IOException(); // TODO: can not create document

            m_nObjectState = embed::EmbedStates::RUNNING;
        }
    }
    else
    {
        if ( ( nStorageMode & embed::ElementModes::READWRITE ) != embed::ElementModes::READWRITE )
            throw io::IOException();

        if ( nEntryConnectionMode == embed::EntryInitModes::NO_INIT )
        {
            // the document just already changed its storage to store to
            // the links to OOo documents for now ignore this call
            // TODO: OOo links will have persistence so it will be switched here
        }
        else if ( nEntryConnectionMode == embed::EntryInitModes::TRUNCATE_INIT )
        {
            // TODO:
            m_pDocHolder->SetComponent( InitNewDocument_Impl(), m_bReadOnly );

            if ( !m_pDocHolder->GetComponent().is() )
                throw io::IOException(); // TODO: can not create document

            m_nObjectState = embed::EmbedStates::RUNNING;
        }
        else if ( nEntryConnectionMode == embed::EntryInitModes::MEDIA_DESCRIPTOR_INIT )
        {
            m_pDocHolder->SetComponent( CreateDocFromMediaDescr_Impl( lArguments ), m_bReadOnly );
            m_nObjectState = embed::EmbedStates::RUNNING;
        }
        //else if ( nEntryConnectionMode == embed::EntryInitModes::TRANSFERABLE_INIT )
        //{
            //TODO:
        //}
        else
            throw lang::IllegalArgumentException( ::rtl::OUString::createFromAscii( "Wrong connection mode is provided!\n" ),
                                        uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ),
                                        3 );
    }
}

//------------------------------------------------------
void SAL_CALL OCommonEmbeddedObject::storeToEntry( const uno::Reference< embed::XStorage >& xStorage,
                            const ::rtl::OUString& sEntName,
                            const uno::Sequence< beans::PropertyValue >& lArguments,
                            const uno::Sequence< beans::PropertyValue >& lObjArgs )
        throw ( lang::IllegalArgumentException,
                embed::WrongStateException,
                io::IOException,
                uno::Exception,
                uno::RuntimeException )
{
    // TODO: use lObjArgs

    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    if ( m_nObjectState == -1 )
    {
        // the object is still not loaded
        throw embed::WrongStateException( ::rtl::OUString::createFromAscii( "Can't store object without persistence!\n" ),
                                        uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );
    }

    if ( m_bWaitSaveCompleted )
        throw embed::WrongStateException(
                    ::rtl::OUString::createFromAscii( "The object waits for saveCompleted() call!\n" ),
                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );

    OSL_ENSURE( !m_bIsLink, "This method implementation must not be used for links!\n" );
    if ( m_bIsLink )
        return;

    OSL_ENSURE( m_xParentStorage.is() && m_xObjectStorage.is(), "The object has no valid persistence!\n" );

    // TODO/LATER: Storing to different format can be done only in running state,
    //           copiing is not legal for documents with relative links.
    if ( m_nObjectState == embed::EmbedStates::LOADED )
        m_xParentStorage->copyElementTo( m_aEntryName, xStorage, sEntName );
    else
    {
        uno::Reference< embed::XStorage > xSubStorage =
                    xStorage->openStorageElement( sEntName, embed::ElementModes::READWRITE );

        if ( !xSubStorage.is() )
            throw uno::RuntimeException(); //TODO

        sal_Int32 nStorageFormat = SOFFICE_FILEFORMAT_CURRENT;
        try {
            nStorageFormat = ::comphelper::OStorageHelper::GetXStorageFormat( xStorage );
        }
        catch ( uno::Exception& )
        {
            OSL_ENSURE( sal_False, "Can not retrieve storage media type!\n" );
        }

        // TODO/LATER: support hierarchical name for embedded objects in embedded objects
        StoreDocToStorage_Impl( xSubStorage, nStorageFormat, GetBaseURLFrom_Impl( lArguments, lObjArgs ), sEntName );
    }

    // TODO: should the listener notification be done?
}

//------------------------------------------------------
void SAL_CALL OCommonEmbeddedObject::storeAsEntry( const uno::Reference< embed::XStorage >& xStorage,
                            const ::rtl::OUString& sEntName,
                            const uno::Sequence< beans::PropertyValue >& lArguments,
                            const uno::Sequence< beans::PropertyValue >& lObjArgs )
        throw ( lang::IllegalArgumentException,
                embed::WrongStateException,
                io::IOException,
                uno::Exception,
                uno::RuntimeException )
{
    // TODO: use lObjArgs

    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    if ( m_nObjectState == -1 )
    {
        // the object is still not loaded
        throw embed::WrongStateException( ::rtl::OUString::createFromAscii( "Can't store object without persistence!\n" ),
                                        uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );
    }

    if ( m_bWaitSaveCompleted )
        throw embed::WrongStateException(
                    ::rtl::OUString::createFromAscii( "The object waits for saveCompleted() call!\n" ),
                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );

    OSL_ENSURE( !m_bIsLink, "This method implementation must not be used for links!\n" );
    if ( m_bIsLink )
        return;

    OSL_ENSURE( m_xParentStorage.is() && m_xObjectStorage.is(), "The object has no valid persistence!\n" );

    sal_Int32 nStorageFormat = SOFFICE_FILEFORMAT_CURRENT;
    try {
        nStorageFormat = ::comphelper::OStorageHelper::GetXStorageFormat( xStorage );
    }
    catch ( uno::Exception& )
    {
        OSL_ENSURE( sal_False, "Can not retrieve storage media type!\n" );
    }

    PostEvent_Impl( ::rtl::OUString::createFromAscii( "OnSaveAs" ) );

    // TODO/MAV: in case storage type is changed storing can be done only in running state
    if ( m_nObjectState == embed::EmbedStates::LOADED )
        m_xParentStorage->copyElementTo( m_aEntryName, xStorage, sEntName );

    uno::Reference< embed::XStorage > xSubStorage =
                xStorage->openStorageElement( sEntName, embed::ElementModes::READWRITE );

    if ( !xSubStorage.is() )
        throw uno::RuntimeException(); //TODO

    if ( m_nObjectState != embed::EmbedStates::LOADED )
    {
        // TODO/LATER: support hierarchical name for embedded objects in embedded objects
        StoreDocToStorage_Impl( xSubStorage, nStorageFormat, GetBaseURLFrom_Impl( lArguments, lObjArgs ), sEntName );
    }

    m_bWaitSaveCompleted = sal_True;
    m_xNewObjectStorage = xSubStorage;
    m_xNewParentStorage = xStorage;
    m_aNewEntryName = sEntName;
    m_aNewDocMediaDescriptor = GetValuableArgs_Impl( lArguments, sal_True );

    // TODO: register listeners for storages above, in case thay are disposed
    //       an exception will be thrown on saveCompleted( true )

    // TODO: should the listener notification be done here or in saveCompleted?
}

//------------------------------------------------------
void SAL_CALL OCommonEmbeddedObject::saveCompleted( sal_Bool bUseNew )
        throw ( embed::WrongStateException,
                uno::Exception,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    if ( m_nObjectState == -1 )
    {
        // the object is still not loaded
        throw embed::WrongStateException( ::rtl::OUString::createFromAscii( "Can't store object without persistence!\n" ),
                                        uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );
    }

    OSL_ENSURE( !m_bIsLink, "This method implementation must not be used for links!\n" );
    if ( m_bIsLink )
        return;

    OSL_ENSURE( m_bWaitSaveCompleted, "Unexpected saveCompleted() call!\n" );
    if ( !m_bWaitSaveCompleted )
        throw io::IOException(); // TODO: illegal call

    OSL_ENSURE( m_xNewObjectStorage.is() && m_xNewParentStorage.is() , "Internal object information is broken!\n" );
    if ( !m_xNewObjectStorage.is() || !m_xNewParentStorage.is() )
        throw uno::RuntimeException(); // TODO: broken internal information

    if ( bUseNew )
    {
        SwitchOwnPersistence( m_xNewParentStorage, m_xNewObjectStorage, m_aNewEntryName );
        m_aDocMediaDescriptor = m_aNewDocMediaDescriptor;

        uno::Reference< util::XModifiable > xModif( m_pDocHolder->GetComponent(), uno::UNO_QUERY );
        if ( xModif.is() )
            xModif->setModified( sal_False );

        PostEvent_Impl( ::rtl::OUString::createFromAscii( "OnSaveAsDone" ) );
    }
    else
    {
        try {
            uno::Reference< lang::XComponent > xComponent( m_xNewObjectStorage, uno::UNO_QUERY );
            OSL_ENSURE( xComponent.is(), "Wrong storage implementation!" );
            if ( xComponent.is() )
                xComponent->dispose();
        }
        catch ( uno::Exception& )
        {
        }
    }

    m_xNewObjectStorage = uno::Reference< embed::XStorage >();
    m_xNewParentStorage = uno::Reference< embed::XStorage >();
    m_aNewEntryName = ::rtl::OUString();
    m_aNewDocMediaDescriptor.realloc( 0 );
    m_bWaitSaveCompleted = sal_False;

    if ( bUseNew )
    {
        // TODO: notify listeners

        if ( m_nUpdateMode == embed::EmbedUpdateModes::ALWAYS_UPDATE )
        {
            // TODO: update visual representation
        }
    }
}

//------------------------------------------------------
sal_Bool SAL_CALL OCommonEmbeddedObject::hasEntry()
        throw ( embed::WrongStateException,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    if ( m_bWaitSaveCompleted )
        throw embed::WrongStateException(
                    ::rtl::OUString::createFromAscii( "The object waits for saveCompleted() call!\n" ),
                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );

    if ( m_xObjectStorage.is() )
        return sal_True;

    return sal_False;
}

//------------------------------------------------------
::rtl::OUString SAL_CALL OCommonEmbeddedObject::getEntryName()
        throw ( embed::WrongStateException,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    if ( m_nObjectState == -1 )
    {
        // the object is still not loaded
        throw embed::WrongStateException( ::rtl::OUString::createFromAscii( "The object persistence is not initialized!\n" ),
                                        uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );
    }

    if ( m_bWaitSaveCompleted )
        throw embed::WrongStateException(
                    ::rtl::OUString::createFromAscii( "The object waits for saveCompleted() call!\n" ),
                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );

    return m_aEntryName;
}

//------------------------------------------------------
void SAL_CALL OCommonEmbeddedObject::storeOwn()
        throw ( embed::WrongStateException,
                io::IOException,
                uno::Exception,
                uno::RuntimeException )
{
    // during switching from Activated to Running and from Running to Loaded states the object will
    // ask container to store the object, the container has to make decision
    // to do so or not

    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    if ( m_nObjectState == -1 )
    {
        // the object is still not loaded
        throw embed::WrongStateException( ::rtl::OUString::createFromAscii( "Can't store object without persistence!\n" ),
                                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );
    }

    if ( m_bWaitSaveCompleted )
        throw embed::WrongStateException(
                    ::rtl::OUString::createFromAscii( "The object waits for saveCompleted() call!\n" ),
                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );

    if ( m_bReadOnly )
        throw io::IOException(); // TODO: access denied

    // nothing to do, if the object is in loaded state
    if ( m_nObjectState == embed::EmbedStates::LOADED )
        return;

    PostEvent_Impl( ::rtl::OUString::createFromAscii( "OnSave" ) );

    OSL_ENSURE( m_pDocHolder->GetComponent().is(), "If an object is activated or in running state it must have a document!\n" );
    if ( !m_pDocHolder->GetComponent().is() )
        throw uno::RuntimeException();

    if ( m_bIsLink )
    {
        // TODO: just store the document to it's location
        uno::Reference< frame::XStorable > xStorable( m_pDocHolder->GetComponent(), uno::UNO_QUERY );
        if ( !xStorable.is() )
            throw uno::RuntimeException(); // TODO

        xStorable->store();
    }
    else
    {
        OSL_ENSURE( m_xParentStorage.is() && m_xObjectStorage.is(), "The object has no valid persistence!\n" );

        if ( !m_xObjectStorage.is() )
            throw io::IOException(); //TODO: access denied

        sal_Int32 nStorageFormat = SOFFICE_FILEFORMAT_CURRENT;
        try {
            nStorageFormat = ::comphelper::OStorageHelper::GetXStorageFormat( m_xParentStorage );
        }
        catch ( uno::Exception& )
        {
            OSL_ENSURE( sal_False, "Can not retrieve storage media type!\n" );
        }

        StoreDocToStorage_Impl( m_xObjectStorage, nStorageFormat, GetBaseURL_Impl(), m_aEntryName );
    }

    uno::Reference< util::XModifiable > xModif( m_pDocHolder->GetComponent(), uno::UNO_QUERY );
    if ( xModif.is() )
        xModif->setModified( sal_False );

    PostEvent_Impl( ::rtl::OUString::createFromAscii( "OnSaveDone" ) );
}

//------------------------------------------------------
sal_Bool SAL_CALL OCommonEmbeddedObject::isReadonly()
        throw ( embed::WrongStateException,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    if ( m_nObjectState == -1 )
    {
        // the object is still not loaded
        throw embed::WrongStateException( ::rtl::OUString::createFromAscii( "The object persistence is not initialized!\n" ),
                                        uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );
    }

    if ( m_bWaitSaveCompleted )
        throw embed::WrongStateException(
                    ::rtl::OUString::createFromAscii( "The object waits for saveCompleted() call!\n" ),
                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );

    return m_bReadOnly;
}

//------------------------------------------------------
void SAL_CALL OCommonEmbeddedObject::reload(
                const uno::Sequence< beans::PropertyValue >& lArguments,
                const uno::Sequence< beans::PropertyValue >& lObjArgs )
        throw ( lang::IllegalArgumentException,
                embed::WrongStateException,
                io::IOException,
                uno::Exception,
                uno::RuntimeException )
{
    // TODO: use lObjArgs
    // for now this method is used only to switch readonly state

    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    if ( m_nObjectState == -1 )
    {
        // the object is still not loaded
        throw embed::WrongStateException( ::rtl::OUString::createFromAscii( "The object persistence is not initialized!\n" ),
                                        uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );
    }

    if ( m_nObjectState != embed::EmbedStates::LOADED )
    {
        // the object is still not loaded
        throw embed::WrongStateException(
                                ::rtl::OUString::createFromAscii( "The object must be in loaded state to be reloaded!\n" ),
                                uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );
    }

    if ( m_bWaitSaveCompleted )
        throw embed::WrongStateException(
                    ::rtl::OUString::createFromAscii( "The object waits for saveCompleted() call!\n" ),
                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );

    m_aDocMediaDescriptor = GetValuableArgs_Impl( lArguments, sal_True );

    // TODO: use lObjArgs for StoreVisualReplacement
    for ( sal_Int32 nObjInd = 0; nObjInd < lObjArgs.getLength(); nObjInd++ )
        if ( lObjArgs[nObjInd].Name.equalsAscii( "OutplaceDispatchInterceptor" ) )
        {
            uno::Reference< frame::XDispatchProviderInterceptor > xDispatchInterceptor;
            if ( lObjArgs[nObjInd].Value >>= xDispatchInterceptor )
                m_pDocHolder->SetOutplaceDispatchInterceptor( xDispatchInterceptor );

            break;
        }

    // TODO:
    // when document allows reloading through API the object can be reloaded not only in loaded state

    sal_Bool bOldReadOnlyValue = m_bReadOnly;

    m_bReadOnly = sal_False;
    for ( sal_Int32 nInd = 0; nInd < lArguments.getLength(); nInd++ )
        if ( lArguments[nInd].Name.equalsAscii( "ReadOnly" ) )
            lArguments[nInd].Value >>= m_bReadOnly;

    if ( bOldReadOnlyValue != m_bReadOnly && !m_bIsLink )
    {
        // close own storage
        try {
            uno::Reference< lang::XComponent > xComponent( m_xObjectStorage, uno::UNO_QUERY );
            OSL_ENSURE( !m_xObjectStorage.is() || xComponent.is(), "Wrong storage implementation!" );
            if ( xComponent.is() )
                xComponent->dispose();
        }
        catch ( uno::Exception& )
        {
        }

        sal_Int32 nStorageMode = m_bReadOnly ? embed::ElementModes::READ : embed::ElementModes::READWRITE;
        m_xObjectStorage = m_xParentStorage->openStorageElement( m_aEntryName, nStorageMode );
    }
}

//------------------------------------------------------
void SAL_CALL OCommonEmbeddedObject::breakLink( const uno::Reference< embed::XStorage >& xStorage,
                                                const ::rtl::OUString& sEntName )
        throw ( lang::IllegalArgumentException,
                embed::WrongStateException,
                io::IOException,
                uno::Exception,
                uno::RuntimeException )
{
    if ( !m_bIsLink )
    {
        // it must be a linked initialized object
        throw embed::WrongStateException(
                    ::rtl::OUString::createFromAscii( "The object is not a valid linked object!\n" ),
                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );
    }
    else
    {
        // the current implementation of OOo links does not implement this method since it does not implement
        // all the set of interfaces required for OOo embedded object ( XEmbedPersist is not supported ).
        throw io::IOException(); // TODO:
    }

#if 0
    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    if ( !xStorage.is() )
        throw lang::IllegalArgumentException( ::rtl::OUString::createFromAscii( "No parent storage is provided!\n" ),
                                            uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ),
                                            1 );

    if ( !sEntName.getLength() )
        throw lang::IllegalArgumentException( ::rtl::OUString::createFromAscii( "Empty element name is provided!\n" ),
                                            uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ),
                                            2 );

    if ( !m_bIsLink || m_nObjectState == -1 )
    {
        // it must be a linked initialized object
        throw embed::WrongStateException(
                    ::rtl::OUString::createFromAscii( "The object is not a valid linked object!\n" ),
                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );
    }

    if ( m_bWaitSaveCompleted )
        throw embed::WrongStateException(
                    ::rtl::OUString::createFromAscii( "The object waits for saveCompleted() call!\n" ),
                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );

    uno::Reference< container::XNameAccess > xNameAccess( xStorage, uno::UNO_QUERY );
    if ( !xNameAccess.is() )
        throw uno::RuntimeException(); //TODO

    // detect entry existence
    sal_Bool bElExists = xNameAccess->hasByName( sEntName );

    m_bReadOnly = sal_False;
    sal_Int32 nStorageMode = embed::ElementModes::READWRITE;

    if ( m_xParentStorage != xStorage || !m_aEntryName.equals( sEntName ) )
        SwitchOwnPersistence( xStorage, sEntName );

    // for linked object it means that it becomes embedded object
    // the document must switch it's persistence also

    // TODO/LATER: handle the case when temp doc can not be created
    // the document is a new embedded object so it must be marked as modified
    uno::Reference< util::XCloseable > xDocument = CreateTempDocFromLink_Impl();
    uno::Reference< util::XModifiable > xModif( m_pDocHolder->GetComponent(), uno::UNO_QUERY );
    if ( !xModif.is() )
        throw uno::RuntimeException();
    try
    {
        xModif->setModified( sal_True );
    }
    catch( uno::Exception& )
    {}

    m_pDocHolder->SetComponent( xDocument, m_bReadOnly );
    OSL_ENSURE( m_pDocHolder->GetComponent().is(), "If document cant be created, an exception must be thrown!\n" );

    if ( m_nObjectState == embed::EmbedStates::LOADED )
        m_nObjectState = embed::EmbedStates::RUNNING;
    else if ( m_nObjectState == embed::EmbedStates::ACTIVE )
        m_pDocHolder->Show();

    m_bIsLink = sal_False;
    m_aLinkFilterName = ::rtl::OUString();
    m_aLinkURL = ::rtl::OUString();
#endif
}

//------------------------------------------------------
sal_Bool SAL_CALL  OCommonEmbeddedObject::isLink()
        throw ( embed::WrongStateException,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    if ( m_bWaitSaveCompleted )
        throw embed::WrongStateException(
                    ::rtl::OUString::createFromAscii( "The object waits for saveCompleted() call!\n" ),
                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );

    return m_bIsLink;
}

//------------------------------------------------------
::rtl::OUString SAL_CALL OCommonEmbeddedObject::getLinkURL()
        throw ( embed::WrongStateException,
                uno::Exception,
                uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    if ( m_bWaitSaveCompleted )
        throw embed::WrongStateException(
                    ::rtl::OUString::createFromAscii( "The object waits for saveCompleted() call!\n" ),
                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );

    if ( !m_bIsLink )
        throw embed::WrongStateException(
                    ::rtl::OUString::createFromAscii( "The object is not a link object!\n" ),
                    uno::Reference< uno::XInterface >( reinterpret_cast< ::cppu::OWeakObject* >(this) ) );

    return m_aLinkURL;
}

