/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: kdevcllayer.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2006-06-02 12:26:23 $
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

#ifndef KDEVCLLAYER_HXX_
#include "kdevcllayer.hxx"
#endif

#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_PROPERTYINFO_HPP_
#include <com/sun/star/configuration/backend/PropertyInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XLAYERCONTENTDESCIBER_HPP_
#include <com/sun/star/configuration/backend/XLayerContentDescriber.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

#ifndef QGLOBAL_H
#include "qglobal.h"
#endif
#ifndef QACCESSIBLE_H
#include "qaccessible.h"
#endif

//==============================================================================

KDEVCLLayer::KDEVCLLayer(const uno::Reference<uno::XComponentContext>& xContext)
{
    //Create instance of LayerContentDescriber Service
    rtl::OUString const k_sLayerDescriberService(RTL_CONSTASCII_USTRINGPARAM(
        "com.sun.star.comp.configuration.backend.LayerDescriber"));

    typedef uno::Reference<backend::XLayerContentDescriber> LayerDescriber;
    uno::Reference< lang::XMultiComponentFactory > xServiceManager = xContext->getServiceManager();
    if( xServiceManager.is() )
    {
        m_xLayerContentDescriber = LayerDescriber::query(
            xServiceManager->createInstanceWithContext(k_sLayerDescriberService, xContext));
    }
    else
    {
        OSL_TRACE("Could not retrieve ServiceManager");
    }
}

//------------------------------------------------------------------------------

void SAL_CALL KDEVCLLayer::readData( const uno::Reference<backend::XLayerHandler>& xHandler)
    throw ( backend::MalformedDataException, lang::NullPointerException,
            lang::WrappedTargetException, uno::RuntimeException)
{
    if( ! m_xLayerContentDescriber.is() )
    {
        throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
            "Could not create com.sun.star.configuration.backend.LayerContentDescriber Service"
        ) ), static_cast < backend::XLayer * > (this) );
    }

    uno::Sequence<backend::PropertyInfo> aPropInfoList(1);

/*
   Commenting out, does not make much sense without an accessibility bridge
===========================================================================
#if defined(QT_ACCESSIBILITY_SUPPORT)
// Accessibility tools under Qt for UNIX are available starting with Qt 4.0
    int nVersionMajor = 0;
    const char *q = qVersion();     // "3.1.0" for example
    while ('0' <= *q && *q <= '9')
        nVersionMajor = nVersionMajor * 10 + *q++ - '0';
    sal_Bool ATToolSupport = (sal_Bool) (nVersionMajor >= 4);
#else
    sal_Bool ATToolSupport = sal_False;
#endif
===========================================================================
   End of commented out section
*/  sal_Bool ATToolSupport = sal_False;

    aPropInfoList[0].Name = rtl::OUString(
        RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.VCL/Settings/Accessibility/EnableATToolSupport") );
    aPropInfoList[0].Type = rtl::OUString(
        RTL_CONSTASCII_USTRINGPARAM( "string" ) );
    aPropInfoList[0].Protected = sal_False;
    aPropInfoList[0].Value = uno::makeAny( rtl::OUString::valueOf( ATToolSupport ) );

    m_xLayerContentDescriber->describeLayer(xHandler, aPropInfoList);
}

//------------------------------------------------------------------------------

rtl::OUString SAL_CALL KDEVCLLayer::getTimestamp(void)
    throw (uno::RuntimeException)
{
    // Return the value as timestamp to avoid regenerating the binary cache
    // on each office launch.

    ::rtl::OUString sTimeStamp(
                RTL_CONSTASCII_USTRINGPARAM( "FALSE" ) );

    return sTimeStamp;
}
