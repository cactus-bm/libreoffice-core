/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: MasterPropertySetInfo.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 17:17:15 $
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
#include "precompiled_comphelper.hxx"
#ifndef _COMPHELPER_CHAINABLEPROPERTYSETINFO_HXX_
#include <comphelper/MasterPropertySetInfo.hxx>
#endif
#ifndef _COMPHELPER_TYPEGENERATION_HXX_
#include <comphelper/TypeGeneration.hxx>
#endif

using ::rtl::OUString;
using ::comphelper::PropertyInfo;
using ::comphelper::GenerateCppuType;
using ::comphelper::MasterPropertySetInfo;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::Type;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::XInterface;
using ::com::sun::star::uno::RuntimeException;
using ::com::sun::star::beans::Property;
using ::com::sun::star::beans::XPropertySetInfo;
using ::com::sun::star::beans::UnknownPropertyException;

MasterPropertySetInfo::MasterPropertySetInfo()
    throw()
{
}

MasterPropertySetInfo::MasterPropertySetInfo( PropertyInfo* pMap )
    throw()
{
    add ( pMap );
}

MasterPropertySetInfo::~MasterPropertySetInfo()
    throw()
{
    PropertyDataHash::iterator aEnd = maMap.end(), aIter = maMap.begin();
    while (aIter != aEnd )
    {
        delete (*aIter).second;
        aIter++;
    }
}

void MasterPropertySetInfo::add( PropertyInfo* pMap, sal_Int32 nCount, sal_uInt8 nMapId )
    throw()
{
    // nCount < 0   => add all
    // nCount == 0  => add nothing
    // nCount > 0   => add at most nCount entries
    if( maProperties.getLength() )
        maProperties.realloc( 0 );

    for ( ; pMap->mpName && ( ( nCount < 0 ) || ( nCount > 0 ) ); --nCount, ++pMap )
    {
        OUString aName( pMap->mpName, pMap->mnNameLen, RTL_TEXTENCODING_ASCII_US );

#ifndef PRODUCT
        PropertyDataHash::iterator aIter = maMap.find( aName );
        if( aIter != maMap.end() )
            OSL_ENSURE( sal_False, "Warning: PropertyInfo added twice, possible error!");
#endif
        maMap[aName] = new PropertyData ( nMapId, pMap );
    }
}

void MasterPropertySetInfo::add( PropertyInfoHash &rHash, sal_uInt8 nMapId )
    throw()
{
    if( maProperties.getLength() )
        maProperties.realloc( 0 );
    PropertyInfoHash::iterator aIter = rHash.begin(), aEnd = rHash.end();

    while ( aIter != aEnd )
    {
#ifndef PRODUCT
        PropertyDataHash::iterator aDebugIter = maMap.find( (*aIter).first );
        if( aDebugIter != maMap.end() )
            OSL_ENSURE( sal_False, "Warning: PropertyInfo added twice, possible error!");
#endif
        maMap[(*aIter).first] = new PropertyData ( nMapId, (*aIter).second );
        aIter++;
    }
}

void MasterPropertySetInfo::remove( const rtl::OUString& aName )
    throw()
{
    maMap.erase ( aName );
    if ( maProperties.getLength() )
         maProperties.realloc( 0 );
}

Sequence< ::Property > SAL_CALL MasterPropertySetInfo::getProperties()
    throw(::com::sun::star::uno::RuntimeException)
{
    sal_Int32 nSize = maMap.size();
    if( maProperties.getLength() != nSize )
    {
        maProperties.realloc ( nSize );
        Property* pProperties = maProperties.getArray();

        PropertyDataHash::iterator aIter = maMap.begin();
        const PropertyDataHash::iterator aEnd = maMap.end();
        for ( ; aIter != aEnd; ++aIter, ++pProperties)
        {
            PropertyInfo* pInfo = (*aIter).second->mpInfo;

            pProperties->Name = OUString( pInfo->mpName, pInfo->mnNameLen, RTL_TEXTENCODING_ASCII_US );
            pProperties->Handle = pInfo->mnHandle;
            const Type* pType;
            GenerateCppuType ( pInfo->meCppuType, pType);
            pProperties->Type = *pType;
            pProperties->Attributes = pInfo->mnAttributes;
        }
    }
    return maProperties;
}

Property SAL_CALL MasterPropertySetInfo::getPropertyByName( const ::rtl::OUString& rName )
    throw(::UnknownPropertyException, ::com::sun::star::uno::RuntimeException)
{
    PropertyDataHash::iterator aIter = maMap.find( rName );

    if ( maMap.end() == aIter )
        throw UnknownPropertyException();

    PropertyInfo *pInfo = (*aIter).second->mpInfo;
    Property aProperty;
    aProperty.Name   = OUString( pInfo->mpName, pInfo->mnNameLen, RTL_TEXTENCODING_ASCII_US );
    aProperty.Handle = pInfo->mnHandle;
    const Type* pType;
    GenerateCppuType ( pInfo->meCppuType, pType );
    aProperty.Type = *pType;

    aProperty.Attributes = pInfo->mnAttributes;
    return aProperty;
}

sal_Bool SAL_CALL MasterPropertySetInfo::hasPropertyByName( const ::rtl::OUString& rName )
    throw(::com::sun::star::uno::RuntimeException)
{
    return static_cast < sal_Bool > ( maMap.find ( rName ) != maMap.end() );
}
