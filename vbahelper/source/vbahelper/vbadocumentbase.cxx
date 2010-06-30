/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#include <vbahelper/vbadocumentbase.hxx>
#include <vbahelper/helperdecl.hxx>
#include <comphelper/unwrapargs.hxx>

#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/util/XProtectable.hpp>
#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/document/XEmbeddedScripts.hpp> //Michael E. Bohn
#include <com/sun/star/beans/XPropertySet.hpp>
#include <ooo/vba/XVBADocService.hpp>

#include <tools/urlobj.hxx>
#include <osl/file.hxx>

using namespace ::com::sun::star;
using namespace ::ooo::vba;

VbaDocumentBase::VbaDocumentBase( const uno::Reference< ov::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext) :VbaDocumentBase_BASE( xParent, xContext ), mxModel(NULL)
{
}

VbaDocumentBase::VbaDocumentBase( const uno::Reference< ov::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, uno::Reference< frame::XModel > xModel ) : VbaDocumentBase_BASE( xParent, xContext ),  mxModel( xModel )
{
}

VbaDocumentBase::VbaDocumentBase( uno::Sequence< uno::Any> const & args,
    uno::Reference< uno::XComponentContext> const & xContext ) : VbaDocumentBase_BASE( getXSomethingFromArgs< XHelperInterface >( args, 0 ), xContext ),  mxModel( getXSomethingFromArgs< frame::XModel >( args, 1 ) )
{
}

::rtl::OUString
VbaDocumentBase::getName() throw (uno::RuntimeException)
{
    rtl::OUString sName = getModel()->getURL();
    if ( sName.getLength() )
    {

        INetURLObject aURL( getModel()->getURL() );
        ::osl::File::getSystemPathFromFileURL( aURL.GetLastName(), sName );
    }
    else
    {
        const static rtl::OUString sTitle( RTL_CONSTASCII_USTRINGPARAM("Title" ) );
        // process "UntitledX - $(PRODUCTNAME)"
        uno::Reference< frame::XFrame > xFrame( getModel()->getCurrentController()->getFrame(), uno::UNO_QUERY_THROW );
        uno::Reference< beans::XPropertySet > xProps( xFrame, uno::UNO_QUERY_THROW );
        xProps->getPropertyValue(sTitle ) >>= sName;
        sal_Int32 pos = 0;
        sName = sName.getToken(0,' ',pos);
    }
    return sName;
}
::rtl::OUString
VbaDocumentBase::getPath() throw (uno::RuntimeException)
{
       INetURLObject aURL( getModel()->getURL() );
    rtl::OUString sURL( aURL.GetMainURL( INetURLObject::DECODE_TO_IURI ) );
    sURL = sURL.copy( 0, sURL.getLength() - aURL.GetLastName().getLength() - 1 );
        rtl::OUString sPath;
    ::osl::File::getSystemPathFromFileURL( sURL, sPath );
    return sPath;
}

::rtl::OUString
VbaDocumentBase::getFullName() throw (uno::RuntimeException)
{
        rtl::OUString sPath;
    ::osl::File::getSystemPathFromFileURL( getModel()->getURL(), sPath );
    return sPath;
}

void
VbaDocumentBase::Close( const uno::Any &rSaveArg, const uno::Any &rFileArg,
                      const uno::Any &rRouteArg ) throw (uno::RuntimeException)
{
    sal_Bool bSaveChanges = sal_False;
    rtl::OUString aFileName;
    sal_Bool bRouteWorkbook = sal_True;

    rSaveArg >>= bSaveChanges;
    sal_Bool bFileName =  ( rFileArg >>= aFileName );
    rRouteArg >>= bRouteWorkbook;
    uno::Reference< frame::XStorable > xStorable( getModel(), uno::UNO_QUERY_THROW );
    uno::Reference< util::XModifiable > xModifiable( getModel(), uno::UNO_QUERY_THROW );

    if( bSaveChanges )
    {
        if( xStorable->isReadonly() )
        {
            throw uno::RuntimeException(::rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM( "Unable to save to a read only file ") ),
                            uno::Reference< XInterface >() );
        }
        if( bFileName )
            xStorable->storeAsURL( aFileName, uno::Sequence< beans::PropertyValue >(0) );
        else
            xStorable->store();
    }
    else
        xModifiable->setModified( false );

    uno::Reference< util::XCloseable > xCloseable( getModel(), uno::UNO_QUERY );

    if( xCloseable.is() )
        // use close(boolean DeliverOwnership)

        // The boolean parameter DeliverOwnership tells objects vetoing the close process that they may
        // assume ownership if they object the closure by throwing a CloseVetoException
        // Here we give up ownership. To be on the safe side, catch possible veto exception anyway.
        xCloseable->close(sal_True);
    // If close is not supported by this model - try to dispose it.
    // But if the model disagree with a reset request for the modify state
    // we shouldn't do so. Otherwhise some strange things can happen.
    else
    {
        uno::Reference< lang::XComponent > xDisposable ( getModel(), uno::UNO_QUERY );
        if ( xDisposable.is() )
            xDisposable->dispose();
    }
}

void
VbaDocumentBase::Protect( const uno::Any &aPassword ) throw (uno::RuntimeException)
{
    rtl::OUString rPassword;
    uno::Reference< util::XProtectable > xProt( getModel(), uno::UNO_QUERY_THROW );
    SC_VBA_FIXME(("Workbook::Protect stub"));
    if(  aPassword >>= rPassword )
        xProt->protect( rPassword );
    else
        xProt->protect( rtl::OUString() );
}

void
VbaDocumentBase::Unprotect( const uno::Any &aPassword ) throw (uno::RuntimeException)
{
    rtl::OUString rPassword;
    uno::Reference< util::XProtectable > xProt( getModel(), uno::UNO_QUERY_THROW );
    if( !xProt->isProtected() )
        throw uno::RuntimeException(::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM( "File is already unprotected" ) ),
            uno::Reference< XInterface >() );
    else
    {
        if( aPassword >>= rPassword )
            xProt->unprotect( rPassword );
        else
            xProt->unprotect( rtl::OUString() );
    }
}

void
VbaDocumentBase::setSaved( sal_Bool bSave ) throw (uno::RuntimeException)
{
    uno::Reference< util::XModifiable > xModifiable( getModel(), uno::UNO_QUERY_THROW );
    xModifiable->setModified( !bSave );
}

sal_Bool
VbaDocumentBase::getSaved() throw (uno::RuntimeException)
{
    uno::Reference< util::XModifiable > xModifiable( getModel(), uno::UNO_QUERY_THROW );
    return !xModifiable->isModified();
}

void
VbaDocumentBase::Save() throw (uno::RuntimeException)
{
    rtl::OUString url = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(".uno:Save"));
    uno::Reference< frame::XModel > xModel = getModel();
    dispatchRequests(xModel,url);
}

void
VbaDocumentBase::Activate() throw (uno::RuntimeException)
{
    uno::Reference< frame::XFrame > xFrame( getModel()->getCurrentController()->getFrame(), uno::UNO_QUERY_THROW );
    xFrame->activate();
}

// ---- Michael E.Bohn  Start-----

uno::Any SAL_CALL
VbaDocumentBase::getVBProject() throw (uno::RuntimeException)

{
    uno::Any aAny;
    uno::Reference< ::lang::XMultiComponentFactory > xServiceManager = mxContext->getServiceManager();
    try
      {
        uno::Reference < ::uno::XInterface > xInterface =  xServiceManager->createInstanceWithContext( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "ooo.vba.VBADocService" )),mxContext);
        uno::Reference < ::ooo::vba::XVBADocService > xVBADocService (xInterface, ::uno::UNO_QUERY_THROW );
        if (xVBADocService.is()){
            uno::Reference< frame::XModel > xModel( getModel(), uno::UNO_QUERY_THROW );
            uno::Reference< document::XEmbeddedScripts > xEnbeddedScripts ( xModel, uno::UNO_QUERY_THROW );
            uno::Reference< script::XStorageBasedLibraryContainer >  xMacroStorageBasedLibraryContainer =  xEnbeddedScripts->getBasicLibraries();
            uno::Reference< script::XStorageBasedLibraryContainer >  xDialogStorageBasedLibraryContainer = xEnbeddedScripts->getDialogLibraries();
            uno::Reference< script::XLibraryContainer > xMacroLibraryContainer ( xMacroStorageBasedLibraryContainer, uno::UNO_QUERY_THROW );
            uno::Reference< script::XLibraryContainer > xDialogLibraryContainer( xDialogStorageBasedLibraryContainer, uno::UNO_QUERY_THROW );

            return xVBADocService->getVBProject( this, mxContext, xModel, xMacroLibraryContainer, xDialogLibraryContainer );
          }

        }catch(uno::Exception* e)
        {
        }
    return aAny;

}


// ---- Michael E.Bohn  End -----


rtl::OUString&
VbaDocumentBase::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("VbaDocumentBase") );
    return sImplName;
}

uno::Sequence< rtl::OUString >
VbaDocumentBase::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.VbaDocumentBase" ) );
    }
    return aServiceNames;
}

