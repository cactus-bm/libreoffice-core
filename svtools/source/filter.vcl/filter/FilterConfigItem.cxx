/*************************************************************************
 *
 *  $RCSfile: FilterConfigItem.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: sj $ $Date: 2001-04-25 09:17:53 $
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

#ifndef _FILTER_CONFIG_ITEM_HXX_
#include "FilterConfigItem.hxx"
#endif

#ifndef SVX_LIGHT
#include <tools/debug.hxx>
#ifndef _UTL_CONFIGMGR_HXX_
#include <unotools/configmgr.hxx>
#endif
#ifndef _UNOTOOLS_PROCESSFACTORY_HXX_
#include <unotools/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XCHANGESBATCH_HPP_
#include <com/sun/star/util/XChangesBatch.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSETINFO_HPP_
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XHIERARCHICALNAMEACCESS_HPP_
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#endif

using namespace ::rtl;
using namespace ::utl                       ;   // getProcessServiceFactory
using namespace ::com::sun::star::lang      ;   // XMultiServiceFactory
using namespace ::com::sun::star::beans     ;   // PropertyValue
using namespace ::com::sun::star::uno       ;   // Reference
using namespace ::com::sun::star::util      ;   // XChangesBatch
using namespace ::com::sun::star::awt       ;   // Size
using namespace ::com::sun::star::container ;   //

static sal_Bool ImpIsTreeAvailable( Reference< XMultiServiceFactory >& rXCfgProv, const String& rTree )
{
    sal_Bool    bAvailable = rTree.Len() != 0;
    if ( bAvailable )
    {
        xub_StrLen  nTokenCount = rTree.GetTokenCount( (sal_Unicode)'/' );
        xub_StrLen  i = 0;

        if ( rTree.GetChar( 0 ) == (sal_Unicode)'/' )
            i++;

        Any aAny;
        aAny <<= (OUString)rTree.GetToken( i++, (sal_Unicode)'/' );

        // creation arguments: nodepath
        PropertyValue aPathArgument;
        aPathArgument.Name = OUString( RTL_CONSTASCII_USTRINGPARAM( "nodepath" ) );
        aPathArgument.Value = aAny;

        Sequence< Any > aArguments( 1 );
        aArguments[ 0 ] <<= aPathArgument;

        Reference< XInterface > xReadAccess;
        try
        {
            xReadAccess = rXCfgProv->createInstanceWithArguments(
                OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.configuration.ConfigurationAccess" ) ),
                    aArguments );
        }
        catch ( ::com::sun::star::uno::Exception& )
        {
            bAvailable = sal_False;
        }
        if ( xReadAccess.is() )
        {
            for ( ; bAvailable && ( i < nTokenCount ); i++ )
            {
                Reference< XHierarchicalNameAccess > xHierarchicalNameAccess
                    ( xReadAccess, UNO_QUERY );

                if ( !xHierarchicalNameAccess.is() )
                    bAvailable = sal_False;
                else
                {
                    String aNode( rTree.GetToken( i, (sal_Unicode)'/' ) );
                    if ( !xHierarchicalNameAccess->hasByHierarchicalName( aNode ) )
                        bAvailable = sal_False;
                    else
                    {
                        Any aAny( xHierarchicalNameAccess->getByHierarchicalName( aNode ) );
                        try
                        {
                            aAny >>= xReadAccess;
                        }
                        catch ( ::com::sun::star::uno::Exception& )
                        {
                            bAvailable = sal_False;
                        }
                    }
                }
            }
        }
    }
    return bAvailable;
}

FilterConfigItem::FilterConfigItem( const OUString& rSubTree ) :
    bModified   ( sal_False )
{
    OUString sTree( ConfigManager::GetConfigBaseURL() );
    sTree += rSubTree;
    Reference< XMultiServiceFactory > xSMGR = getProcessServiceFactory();   // get global uno service manager

    Reference< XMultiServiceFactory > xCfgProv(
        xSMGR->createInstance( OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.configuration.ConfigurationProvider" ) ) ),
            UNO_QUERY );

    if ( xCfgProv.is() )
    {
        if ( ImpIsTreeAvailable( xCfgProv, String( sTree ) ) )
        {
            Any aAny;
            // creation arguments: nodepath
            PropertyValue aPathArgument;
            aAny <<= sTree;
            aPathArgument.Name = OUString( RTL_CONSTASCII_USTRINGPARAM( "nodepath" ) );
            aPathArgument.Value = aAny;

            // creation arguments: commit mode
            PropertyValue aModeArgument;
            sal_Bool bAsyncron = sal_True;
            aAny <<= bAsyncron;
            aModeArgument.Name = OUString( RTL_CONSTASCII_USTRINGPARAM( "lazywrite" ) );
            aModeArgument.Value = aAny;

            Sequence< Any > aArguments( 2 );
            aArguments[ 0 ] <<= aPathArgument;
            aArguments[ 1 ] <<= aModeArgument;

            try
            {
                xUpdatableView = xCfgProv->createInstanceWithArguments(
                    OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.configuration.ConfigurationUpdateAccess" ) ),
                        aArguments );
                if ( xUpdatableView.is() )
                    xPropSet = Reference< XPropertySet >( xUpdatableView, UNO_QUERY );
            }
            catch ( ::com::sun::star::uno::Exception& )
            {
                DBG_ERROR( "FilterConfigItem::FilterConfigItem - Could not access configuration Key" );
            }
        }
    }
};

FilterConfigItem::~FilterConfigItem()
{
    if ( xUpdatableView.is() )
    {
        if ( xPropSet.is() && bModified )
        {
            Reference< XChangesBatch > xUpdateControl( xUpdatableView, UNO_QUERY );
            if ( xUpdateControl.is() )
            {
                try
                {
                    xUpdateControl->commitChanges();
                }
                catch ( ::com::sun::star::uno::Exception& )
                {
                    DBG_ERROR( "FilterConfigItem::FilterConfigItem - Could not update configuration data" );
                }
            }
        }
    }
}

sal_Bool FilterConfigItem::ImplGetPropertyValue( Any& rAny, const Reference< XPropertySet >& rXPropSet, const OUString& rString, sal_Bool bTestPropertyAvailability )
{
    sal_Bool bRetValue = sal_True;

    if ( rXPropSet.is() )
    {
        if ( bTestPropertyAvailability )
        {
            bRetValue = sal_False;
            try
            {
                Reference< XPropertySetInfo >
                    aXPropSetInfo( rXPropSet->getPropertySetInfo() );
                if ( aXPropSetInfo.is() )
                    bRetValue = aXPropSetInfo->hasPropertyByName( rString );
            }
            catch( ::com::sun::star::uno::Exception& )
            {
                //
            }
        }
        if ( bRetValue )
        {
            try
            {
                rAny = rXPropSet->getPropertyValue( rString );
                if ( !rAny.hasValue() )
                    bRetValue = sal_False;
            }
            catch( ::com::sun::star::uno::Exception& )
            {
                bRetValue = sal_False;
            }
        }
    }
    else
        bRetValue = sal_False;
    return bRetValue;
}

sal_Bool FilterConfigItem::ReadBool( const OUString& rKey, sal_Bool bDefault )
{
    sal_Bool bRetValue = bDefault;
    Any aAny;

    if ( ImplGetPropertyValue( aAny, xPropSet, rKey, sal_True ) )
        aAny >>= bRetValue;
    return bRetValue;
}

sal_Int32 FilterConfigItem::ReadInt32( const OUString& rKey, sal_Int32 nDefault )
{
    sal_Int32 nRetValue = nDefault;
    Any aAny;

    if ( ImplGetPropertyValue( aAny, xPropSet, rKey, sal_True ) )
        aAny >>= nRetValue;
    return nRetValue;
}


Size FilterConfigItem::ReadSize( const OUString& rKey, const Size& rDefault )
{
    Size aRetValue( rDefault );
    Any aAny;

    if ( ImplGetPropertyValue( aAny, xPropSet, rKey, sal_True ) )
    {
        try
        {
            Reference< XPropertySet > aXPropSet;
            if ( aAny >>= aXPropSet )
            {
                if ( ImplGetPropertyValue( aAny, aXPropSet, OUString( RTL_CONSTASCII_USTRINGPARAM( "Width" ) ), sal_True ) )
                    aAny >>= aRetValue.Width;
                if ( ImplGetPropertyValue( aAny, aXPropSet, OUString( RTL_CONSTASCII_USTRINGPARAM( "Height" ) ), sal_True ) )
                    aAny >>= aRetValue.Height;
            }
        }
        catch ( ::com::sun::star::uno::Exception& )
        {
            DBG_ERROR( "FilterConfigItem::ReadSize - could not read PropertyValue" );
        }
    }
    return aRetValue;
}

void FilterConfigItem::WriteBool( const OUString& rKey, sal_Bool bNewValue )
{
    if ( xPropSet.is() )
    {
        Any aAny;

        if ( ImplGetPropertyValue( aAny, xPropSet, rKey, sal_True ) )
        {
            sal_Bool bOldValue;
            if ( aAny >>= bOldValue )
            {
                if ( bOldValue != bNewValue )
                {
                    aAny <<= bNewValue;
                    try
                    {
                        xPropSet->setPropertyValue( rKey, aAny );
                        bModified = sal_True;
                    }
                    catch ( ::com::sun::star::uno::Exception& )
                    {
                        DBG_ERROR( "FilterConfigItem::WriteBool - could not set PropertyValue" );
                    }
                }
            }
        }
    }
}

void FilterConfigItem::WriteInt32( const OUString& rKey, sal_Int32 nNewValue )
{
    if ( xPropSet.is() )
    {
        Any aAny;

        if ( ImplGetPropertyValue( aAny, xPropSet, rKey, sal_True ) )
        {
            sal_Int32 nOldValue;
            if ( aAny >>= nOldValue )
            {
                if ( nOldValue != nNewValue )
                {
                    aAny <<= nNewValue;
                    try
                    {
                        xPropSet->setPropertyValue( rKey, aAny );
                        bModified = sal_True;
                    }
                    catch ( ::com::sun::star::uno::Exception& )
                    {
                        DBG_ERROR( "FilterConfigItem::WriteInt32 - could not set PropertyValue" );
                    }
                }
            }
        }
    }
}

void FilterConfigItem::WriteSize( const OUString& rKey, const Size& rNewValue )
{
    if ( xPropSet.is() )
    {
        Any aAny;
        sal_Int32 nOldWidth = rNewValue.Width;
        sal_Int32 nOldHeight = rNewValue.Height;

        if ( ImplGetPropertyValue( aAny, xPropSet, rKey, sal_True ) )
        {
            try
            {
                Reference< XPropertySet > aXPropSet;
                if ( aAny >>= aXPropSet )
                {
                    if ( ImplGetPropertyValue( aAny, aXPropSet, OUString( RTL_CONSTASCII_USTRINGPARAM( "Width" ) ), sal_True ) )
                        aAny >>= nOldWidth;
                    if ( ImplGetPropertyValue( aAny, aXPropSet, OUString( RTL_CONSTASCII_USTRINGPARAM( "Height" ) ), sal_True ) )
                        aAny >>= nOldHeight;
                }
                if ( ( nOldWidth != rNewValue.Width ) || ( nOldHeight != rNewValue.Height ) )
                {
                    aAny <<= rNewValue.Width;
                    aXPropSet->setPropertyValue( OUString( RTL_CONSTASCII_USTRINGPARAM( "Width" ) ), aAny );
                    aAny <<= rNewValue.Height;
                    aXPropSet->setPropertyValue( OUString( RTL_CONSTASCII_USTRINGPARAM( "Height" ) ), aAny );
                    bModified = sal_True;
                }
            }
            catch ( ::com::sun::star::uno::Exception& )
            {
                DBG_ERROR( "FilterConfigItem::WriteSize - could not read PropertyValue" );
            }
        }
    }
}

#else

FilterConfigItem::FilterConfigItem( const ::rtl::OUString& rSubTree )
{
};

#endif // SVX_LIGHT


// ------------------------------------------------------------------------
