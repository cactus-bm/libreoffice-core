/*************************************************************************
 *
 *  $RCSfile: storagehelper.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2004-10-04 21:07:02 $
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

#ifndef _COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XENCRYPTIONPROTECTEDSOURCE_HPP_
#include <com/sun/star/embed/XEncryptionProtectedSource.hpp>
#endif

#ifndef _COM_SUN_STAR_UCB_XSIMPLEFILEACCESS_HPP_
#include <com/sun/star/ucb/XSimpleFileAccess.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#include <comphelper/fileformat.h>
#include <comphelper/storagehelper.hxx>
#include <comphelper/processfactory.hxx>

using namespace ::com::sun::star;

namespace comphelper {

// ----------------------------------------------------------------------
uno::Reference< lang::XSingleServiceFactory > OStorageHelper::GetStorageFactory(
                            const uno::Reference< lang::XMultiServiceFactory >& xSF )
        throw ( uno::Exception )
{
    uno::Reference< lang::XMultiServiceFactory > xFactory = xSF.is() ? xSF : ::comphelper::getProcessServiceFactory();
    if ( !xFactory.is() )
        throw uno::RuntimeException();

    uno::Reference < lang::XSingleServiceFactory > xStorageFactory(
                    xFactory->createInstance ( ::rtl::OUString::createFromAscii( "com.sun.star.embed.StorageFactory" ) ),
                    uno::UNO_QUERY );

    if ( !xStorageFactory.is() )
        throw uno::RuntimeException();

    return xStorageFactory;
}

// ----------------------------------------------------------------------
uno::Reference< embed::XStorage > OStorageHelper::GetTemporaryStorage(
            const uno::Reference< lang::XMultiServiceFactory >& xFactory )
    throw ( uno::Exception )
{
    uno::Reference< embed::XStorage > xTempStorage( GetStorageFactory( xFactory )->createInstance(),
                                                    uno::UNO_QUERY );
    if ( !xTempStorage.is() )
        throw uno::RuntimeException();

    return xTempStorage;
}

// ----------------------------------------------------------------------
uno::Reference< embed::XStorage > OStorageHelper::GetStorageFromURL(
            const ::rtl::OUString& aURL,
            sal_Int32 nStorageMode,
            const uno::Reference< lang::XMultiServiceFactory >& xFactory )
    throw ( uno::Exception )
{
    uno::Sequence< uno::Any > aArgs( 2 );
    aArgs[0] <<= aURL;
    aArgs[1] <<= nStorageMode;

    uno::Reference< embed::XStorage > xTempStorage( GetStorageFactory( xFactory )->createInstanceWithArguments( aArgs ),
                                                    uno::UNO_QUERY );
    if ( !xTempStorage.is() )
        throw uno::RuntimeException();

    return xTempStorage;
}

// ----------------------------------------------------------------------
uno::Reference< embed::XStorage > OStorageHelper::GetStorageFromInputStream(
            const uno::Reference < io::XInputStream >& xStream,
            const uno::Reference< lang::XMultiServiceFactory >& xFactory )
        throw ( uno::Exception )
{
    uno::Sequence< uno::Any > aArgs( 2 );
    aArgs[0] <<= xStream;
    aArgs[1] <<= embed::ElementModes::READ;

    uno::Reference< embed::XStorage > xTempStorage( GetStorageFactory( xFactory )->createInstanceWithArguments( aArgs ),
                                                    uno::UNO_QUERY );
    if ( !xTempStorage.is() )
        throw uno::RuntimeException();

    return xTempStorage;
}

// ----------------------------------------------------------------------
uno::Reference< embed::XStorage > OStorageHelper::GetStorageFromStream(
            const uno::Reference < io::XStream >& xStream,
            sal_Int32 nStorageMode,
            const uno::Reference< lang::XMultiServiceFactory >& xFactory )
        throw ( uno::Exception )
{
    uno::Sequence< uno::Any > aArgs( 2 );
    aArgs[0] <<= xStream;
    aArgs[1] <<= nStorageMode;

    uno::Reference< embed::XStorage > xTempStorage( GetStorageFactory( xFactory )->createInstanceWithArguments( aArgs ),
                                                    uno::UNO_QUERY );
    if ( !xTempStorage.is() )
        throw uno::RuntimeException();

    return xTempStorage;
}

// ----------------------------------------------------------------------
void OStorageHelper::CopyInputToOutput(
            const uno::Reference< io::XInputStream >& xInput,
            const uno::Reference< io::XOutputStream >& xOutput )
    throw ( uno::Exception )
{
    static const sal_Int32 nConstBufferSize = 32000;

    sal_Int32 nRead;
    uno::Sequence < sal_Int8 > aSequence ( nConstBufferSize );

    do
    {
        nRead = xInput->readBytes ( aSequence, nConstBufferSize );
        if ( nRead < nConstBufferSize )
        {
            uno::Sequence < sal_Int8 > aTempBuf ( aSequence.getConstArray(), nRead );
            xOutput->writeBytes ( aTempBuf );
        }
        else
            xOutput->writeBytes ( aSequence );
    }
    while ( nRead == nConstBufferSize );
}

// ----------------------------------------------------------------------
uno::Reference< io::XInputStream > OStorageHelper::GetInputStreamFromURL(
            const ::rtl::OUString& aURL,
            const uno::Reference< lang::XMultiServiceFactory >& xSF )
    throw ( uno::Exception )
{
    uno::Reference< lang::XMultiServiceFactory > xFactory = xSF.is() ? xSF : ::comphelper::getProcessServiceFactory();
    if ( !xFactory.is() )
        throw uno::RuntimeException();

    uno::Reference < ::com::sun::star::ucb::XSimpleFileAccess > xTempAccess(
            xFactory->createInstance ( ::rtl::OUString::createFromAscii( "com.sun.star.ucb.SimpleFileAccess" ) ),
            uno::UNO_QUERY );

    if ( !xTempAccess.is() )
        throw uno::RuntimeException();

    uno::Reference< io::XInputStream > xInputStream = xTempAccess->openFileRead( aURL );
    if ( !xInputStream.is() )
        throw uno::RuntimeException();

    return xInputStream;
}

// ----------------------------------------------------------------------
void OStorageHelper::SetCommonStoragePassword(
            const uno::Reference< embed::XStorage >& xStorage,
            const ::rtl::OUString& aPass )
    throw ( uno::Exception )
{
    uno::Reference< embed::XEncryptionProtectedSource > xEncrSet( xStorage, uno::UNO_QUERY );
    if ( !xEncrSet.is() )
        throw io::IOException(); // TODO

    xEncrSet->setEncryptionPassword( aPass );
}

// ----------------------------------------------------------------------
sal_Int32 OStorageHelper::GetXStorageFormat(
            const uno::Reference< embed::XStorage >& xStorage )
        throw ( uno::Exception )
{
    uno::Reference< beans::XPropertySet > xStorProps( xStorage, uno::UNO_QUERY_THROW );

    ::rtl::OUString aMediaType;
    xStorProps->getPropertyValue( ::rtl::OUString::createFromAscii( "MediaType" ) ) >>= aMediaType;

    sal_Int32 nResult = 0;

    // TODO/LATER: the filter configuration could be used to detect it later, or batter a special service
    if ( aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/vnd.sun.xml.writer" ) )
      || aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/vnd.sun.xml.writer.web" ) )
      || aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/vnd.sun.xml.writer.global" ) )
      || aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/vnd.sun.xml.draw" ) )
      || aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/vnd.sun.xml.impress" ) )
      || aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/vnd.sun.xml.calc" ) )
      || aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/vnd.sun.xml.chart" ) )
      || aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/vnd.sun.xml.math" ) ) )
    {
        nResult = SOFFICE_FILEFORMAT_60;
    }
    else if ( aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/x-vnd.oasis.openoffice.text" ) )
      || aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/x-vnd.oasis.openoffice.text-web" ) )
      || aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/x-vnd.oasis.openoffice.text-global" ) )
      || aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/x-vnd.oasis.openoffice.drawing" ) )
      || aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/x-vnd.oasis.openoffice.presentation" ) )
      || aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/x-vnd.oasis.openoffice.spreadsheet" ) )
      || aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/x-vnd.oasis.openoffice.chart" ) )
      || aMediaType.equalsIgnoreAsciiCase( ::rtl::OUString::createFromAscii( "application/x-vnd.oasis.openoffice.math" ) ) )
    {
        nResult = SOFFICE_FILEFORMAT_8;
    }
    else
        throw uno::Exception();

    return nResult;
}

}

