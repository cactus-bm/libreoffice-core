/*************************************************************************
 *
 *  $RCSfile: imexp.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: dbo $ $Date: 2001-08-07 10:55:47 $
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

#include <stdio.h>
#include <rtl/memory.h>

#include <xmlscript/xmldlg_imexp.hxx>
#include <xmlscript/xml_helper.hxx>

#include <cppuhelper/servicefactory.hxx>
#include <cppuhelper/bootstrap.hxx>
#include <cppuhelper/implbase2.hxx>

#include <comphelper/processfactory.hxx>
#include <comphelper/regpathhelper.hxx>

#include <tools/debug.hxx>
#include <vcl/svapp.hxx>
#include <svtools/unoiface.hxx> // InitExtToolkit

#include <com/sun/star/io/XActiveDataSource.hpp>

#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

#include <com/sun/star/registry/XSimpleRegistry.hpp>
#include <com/sun/star/registry/XImplementationRegistration.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include <com/sun/star/awt/XToolkit.hpp>
#include <com/sun/star/awt/XControlModel.hpp>

#include <com/sun/star/container/XNameContainer.hpp>


using namespace ::rtl;
using namespace ::cppu;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;



Reference< lang::XMultiServiceFactory > createApplicationServiceManager()
{
    Reference< XComponentContext > xContext;

    try
    {
        ::rtl::OUString localRegistry = OUString::createFromAscii( "xmlscript.rdb" ); //"::comphelper::getPathToUserRegistry();
        ::rtl::OUString systemRegistry = ::comphelper::getPathToSystemRegistry();

        Reference< registry::XSimpleRegistry > xLocalRegistry( ::cppu::createSimpleRegistry() );
        Reference< registry::XSimpleRegistry > xSystemRegistry( ::cppu::createSimpleRegistry() );
        if ( xLocalRegistry.is() && (localRegistry.getLength() > 0) )
        {
            try
            {
                xLocalRegistry->open( localRegistry, sal_False, sal_True);
            }
            catch ( registry::InvalidRegistryException& )
            {
            }

            if ( !xLocalRegistry->isValid() )
                xLocalRegistry->open(localRegistry, sal_True, sal_True);
        }

        if ( xSystemRegistry.is() && (systemRegistry.getLength() > 0) )
            xSystemRegistry->open( systemRegistry, sal_True, sal_False);

        if ( (xLocalRegistry.is() && xLocalRegistry->isValid()) &&
             (xSystemRegistry.is() && xSystemRegistry->isValid()) )
        {
            Reference < registry::XSimpleRegistry > xReg( ::cppu::createNestedRegistry() );
            Sequence< Any > seqAnys(2);
            seqAnys[0] <<= xLocalRegistry ;
            seqAnys[1] <<= xSystemRegistry ;
            Reference< lang::XInitialization > xInit( xReg, UNO_QUERY );
            xInit->initialize( seqAnys );

            xContext = ::cppu::bootstrap_InitialComponentContext( xReg );
        }
        else
        {
            throw Exception(
                OUString( RTL_CONSTASCII_USTRINGPARAM("no registry!") ),
                Reference< XInterface >() );
        }

        Reference< XInterface > x( xContext->getServiceManager()->createInstanceWithContext(
            OUString::createFromAscii( "com.sun.star.xml.sax.Parser" ), xContext ) );
        if (! x.is()) // register only once
        {
            Reference < registry::XImplementationRegistration > xReg(
                xContext->getServiceManager()->createInstanceWithContext(
                    OUString::createFromAscii( "com.sun.star.registry.ImplementationRegistration" ), xContext ), UNO_QUERY );

#ifdef SAL_W32
            OUString aDllName = OUString::createFromAscii( "sax.dll" );
#else
            OUString aDllName = OUString::createFromAscii( "libsax.so" );
#endif
            xReg->registerImplementation(
                OUString::createFromAscii( "com.sun.star.loader.SharedLibrary" ),
                aDllName, Reference< registry::XSimpleRegistry > () );
#ifdef SAL_W32
            aDllName = OUString::createFromAscii( "tk" );
            aDllName += OUString::valueOf( (sal_Int32)SUPD );
            aDllName += OUString::createFromAscii( "mi.dll" );
#else
            aDllName = OUString::createFromAscii( "libtk" );
            aDllName += OUString::valueOf( (sal_Int32)SUPD );
            aDllName += OUString::createFromAscii( ".so" );
#endif
            xReg->registerImplementation(
                OUString::createFromAscii( "com.sun.star.loader.SharedLibrary" ),
                aDllName, Reference< registry::XSimpleRegistry > () );
#ifdef SAL_W32
            aDllName = OUString::createFromAscii( "svt" );
            aDllName += OUString::valueOf( (sal_Int32)SUPD );
            aDllName += OUString::createFromAscii( "mi.dll" );
#else
            aDllName = OUString::createFromAscii( "libsvt" );
            aDllName += OUString::valueOf( (sal_Int32)SUPD );
            aDllName += OUString::createFromAscii( ".so" );
#endif
            xReg->registerImplementation(
                OUString::createFromAscii( "com.sun.star.loader.SharedLibrary" ),
                aDllName, Reference< registry::XSimpleRegistry > () );
#ifdef SAL_W32
            aDllName = OUString::createFromAscii( "i18n" );
            aDllName += OUString::valueOf( (sal_Int32)SUPD );
            aDllName += OUString::createFromAscii( "mi.dll" );
#else
            aDllName = OUString::createFromAscii( "libi18n" );
            aDllName += OUString::valueOf( (sal_Int32)SUPD );
            aDllName += OUString::createFromAscii( ".so" );
#endif
            xReg->registerImplementation(
                OUString::createFromAscii( "com.sun.star.loader.SharedLibrary" ),
                aDllName, Reference< registry::XSimpleRegistry > () );
        }
    }

    catch( Exception& rExc )
    {
        OString aStr( OUStringToOString( rExc.Message, RTL_TEXTENCODING_ASCII_US ) );
        OSL_ENSURE( 0, aStr.getStr() );
    }

    return Reference< lang::XMultiServiceFactory >( xContext->getServiceManager(), UNO_QUERY );
}


// -----------------------------------------------------------------------

Reference< container::XNameContainer > importFile(
    char const * fname )
{
    // create the input stream
    FILE *f = ::fopen( fname, "rb" );
    if (f)
    {
        ::fseek( f, 0 ,SEEK_END );
        int nLength = ::ftell( f );
        ::fseek( f, 0, SEEK_SET );

        ByteSequence bytes( nLength );
        ::fread( bytes.getArray(), nLength, 1, f );
        ::fclose( f );

        Reference< lang::XMultiServiceFactory > xSMgr( ::comphelper::getProcessServiceFactory() );
        Reference< container::XNameContainer > xModel( xSMgr->createInstance(
            OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.UnoControlDialogModel" ) ) ), UNO_QUERY );
        ::xmlscript::importDialogModel( ::xmlscript::createInputStream( bytes ), xModel );

        return xModel;
    }
    else
    {
        throw Exception( OUString( RTL_CONSTASCII_USTRINGPARAM("### Cannot read file!") ),
                         Reference< XInterface >() );
    }
}

void exportToFile(
    char const * fname,
    Reference< container::XNameContainer > const & xModel )
{
    Reference< io::XInputStreamProvider > xProvider( ::xmlscript::exportDialogModel( xModel ) );
    Reference< io::XInputStream > xStream( xProvider->createInputStream() );

    Sequence< sal_Int8 > bytes;
    sal_Int32 nRead = xStream->readBytes( bytes, xStream->available() );
    for (;;)
    {
        Sequence< sal_Int8 > readBytes;
        nRead = xStream->readBytes( readBytes, 1024 );
        if (! nRead)
            break;
        OSL_ASSERT( readBytes.getLength() >= nRead );

        sal_Int32 nPos = bytes.getLength();
        bytes.realloc( nPos + nRead );
        ::rtl_copyMemory( bytes.getArray() + nPos, readBytes.getConstArray(), (sal_uInt32)nRead );
    }

    FILE * f = ::fopen( fname, "w" );
    ::fwrite( bytes.getConstArray(), 1, bytes.getLength(), f );
    ::fflush( f );
    ::fclose( f );
}



class MyApp : public Application
{
public:
    void Main();
};

MyApp aMyApp;

// -----------------------------------------------------------------------

void MyApp::Main()
{
    if (GetCommandLineParamCount() < 1)
    {
        OSL_ENSURE( 0, "usage: imexp inputfile [outputfile]\n" );
        return;
    }

    Reference< lang::XMultiServiceFactory >  xMSF = createApplicationServiceManager();

    try
    {
        ::comphelper::setProcessServiceFactory( xMSF );

        Reference< awt::XToolkit> xToolkit( xMSF->createInstance(
            OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.ExtToolkit" ) ) ), UNO_QUERY );

        // import dialogs
        OString aParam1( OUStringToOString( OUString( GetCommandLineParam( 0 ) ), RTL_TEXTENCODING_ASCII_US ) );
        Reference< container::XNameContainer > xModel( importFile( aParam1.getStr() ) );
        OSL_ASSERT( xModel.is() );

        Reference< awt::XControl > xDlg( xMSF->createInstance(
            OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.UnoControlDialog" ) ) ), UNO_QUERY );
        xDlg->setModel( Reference< awt::XControlModel >::query( xModel ) );
        xDlg->createPeer( xToolkit, 0 );
        Reference< awt::XDialog > xD( xDlg, UNO_QUERY );
        xD->execute();

        if (GetCommandLineParamCount() == 2)
        {
            // write modified dialogs
            OString aParam2( OUStringToOString( OUString( GetCommandLineParam( 1 ) ), RTL_TEXTENCODING_ASCII_US ) );
            exportToFile( aParam2.getStr(), xModel );
        }
    }
    catch (xml::sax::SAXException & rExc)
    {
        OString aStr( OUStringToOString( rExc.Message, RTL_TEXTENCODING_ASCII_US ) );
        uno::Exception exc;
        if (rExc.WrappedException >>= exc)
        {
            aStr += OString( " >>> " );
            aStr += OUStringToOString( exc.Message, RTL_TEXTENCODING_ASCII_US );
        }
        OSL_ENSURE( 0, aStr.getStr() );
    }
    catch (uno::Exception & rExc)
    {
        OString aStr( OUStringToOString( rExc.Message, RTL_TEXTENCODING_ASCII_US ) );
        OSL_ENSURE( 0, aStr.getStr() );
    }

    // dispose component context
    Reference< beans::XPropertySet > xProps( xMSF, UNO_QUERY );
    if (xProps.is())
    {
        try
        {
            Reference< lang::XComponent > xComp;
            if (xProps->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("DefaultContext") ) ) >>= xComp)
            {
                xComp->dispose();
            }
        }
        catch (beans::UnknownPropertyException &)
        {
        }
    }
}

