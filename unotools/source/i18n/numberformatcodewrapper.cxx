/*************************************************************************
 *
 *  $RCSfile: numberformatcodewrapper.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: er $ $Date: 2000-10-13 19:52:27 $
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

#include <unotools/numberformatcodewrapper.hxx>

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _UNOTOOLS_COMPONENTFACTORY_HXX_
#include <unotools/componentfactory.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_XINTERFACE_HPP_
#include <com/sun/star/uno/XInterface.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#pragma hdrstop


#define LOCALEDATA_LIBRARYNAME "int"
#define LOCALEDATA_SERVICENAME "com.sun.star.lang.NumberFormatCode"

using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;


NumberFormatCodeWrapper::NumberFormatCodeWrapper(
            const Reference< XMultiServiceFactory > & xSF,
            const Locale& rLocale
            )
        :
        xSMgr( xSF )
{
    setLocale( rLocale );
    if ( xSMgr.is() )
    {
        try
        {
            xNFC = Reference< XNumberFormatCode > ( xSMgr->createInstance(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( LOCALEDATA_SERVICENAME ) ) ),
                uno::UNO_QUERY );
        }
        catch ( Exception& e )
        {
            DBG_ERRORFILE( "NumberFormatCodeWrapper ctor: Exception caught!" );
        }
    }
    else
    {   // try to get an instance somehow
        DBG_ERRORFILE( "NumberFormatCodeWrapper: no service manager, trying own" );
        try
        {
            Reference< XInterface > xI = ::utl::getComponentInstance(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( LLCF_LIBNAME( LOCALEDATA_LIBRARYNAME ) ) ),
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( LOCALEDATA_SERVICENAME ) ) );
            if ( xI.is() )
            {
                Any x = xI->queryInterface( ::getCppuType((const Reference< XNumberFormatCode >*)0) );
                x >>= xNFC;
            }
        }
        catch ( Exception& e )
        {
            DBG_ERRORFILE( "getComponentInstance: Exception caught!" );
        }
    }
}


NumberFormatCodeWrapper::~NumberFormatCodeWrapper()
{
}


void NumberFormatCodeWrapper::setLocale( const ::com::sun::star::lang::Locale& rLocale )
{
    aLocale = rLocale;
}


::com::sun::star::lang::NumberFormatCode
NumberFormatCodeWrapper::getDefault( sal_Int16 formatType, sal_Int16 formatUsage ) const
{
    try
    {
        if ( xNFC.is() )
            return xNFC->getDefault( formatType, formatUsage, aLocale );
    }
    catch ( Exception& e )
    {
        DBG_ERRORFILE( "getDefault: Exception caught!" );
    }
    return ::com::sun::star::lang::NumberFormatCode();
}


::com::sun::star::lang::NumberFormatCode
NumberFormatCodeWrapper::getFormatCode( sal_Int32 formatIndex ) const
{
    try
    {
        if ( xNFC.is() )
            return xNFC->getFormatCode( formatIndex, aLocale );
    }
    catch ( Exception& e )
    {
        DBG_ERRORFILE( "getFormatCode: Exception caught!" );
    }
    return ::com::sun::star::lang::NumberFormatCode();
}


::com::sun::star::uno::Sequence< ::com::sun::star::lang::NumberFormatCode >
NumberFormatCodeWrapper::getAllFormatCode( sal_Int16 formatUsage ) const
{
    try
    {
        if ( xNFC.is() )
            return xNFC->getAllFormatCode( formatUsage, aLocale );
    }
    catch ( Exception& e )
    {
        DBG_ERRORFILE( "getAllFormatCode: Exception caught!" );
    }
    return ::com::sun::star::uno::Sequence< ::com::sun::star::lang::NumberFormatCode > (0);
}
