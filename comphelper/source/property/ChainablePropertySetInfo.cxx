/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <comphelper/ChainablePropertySetInfo.hxx>

using ::comphelper::PropertyInfo;
using ::comphelper::ChainablePropertySetInfo;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::Type;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::XInterface;
using ::com::sun::star::uno::RuntimeException;
using ::com::sun::star::beans::Property;
using ::com::sun::star::beans::XPropertySetInfo;
using ::com::sun::star::beans::UnknownPropertyException;

ChainablePropertySetInfo::ChainablePropertySetInfo( PropertyInfo const * pMap )
    throw()
{
    add ( pMap );
}

ChainablePropertySetInfo::~ChainablePropertySetInfo()
    throw()
{
}

void ChainablePropertySetInfo::add( PropertyInfo const * pMap, sal_Int32 nCount )
    throw()
{
    // nCount < 0   => add all
    // nCount == 0  => add nothing
    // nCount > 0   => add at most nCount entries
    if( maProperties.getLength() )
        maProperties.realloc( 0 );

    while( !pMap->maName.isEmpty() && ( ( nCount < 0) || ( nCount-- > 0 ) ) )
    {
#ifdef DBG_UTIL
        PropertyInfoHash::iterator aIter = maMap.find( pMap->maName );
        if( aIter != maMap.end() )
            OSL_FAIL( "Warning: PropertyInfo added twice, possible error!");
#endif
        maMap[pMap->maName] = pMap;
        ++pMap;
    }
}

void ChainablePropertySetInfo::remove( const OUString& aName )
    throw()
{
    maMap.erase ( aName );
    if ( maProperties.getLength() )
         maProperties.realloc( 0 );
}

Sequence< ::Property > SAL_CALL ChainablePropertySetInfo::getProperties()
    throw(::com::sun::star::uno::RuntimeException)
{
    sal_Int32 nSize = maMap.size();
    if( maProperties.getLength() != nSize )
    {
        maProperties.realloc ( nSize );
        Property* pProperties = maProperties.getArray();

        for (PropertyInfoHash::const_iterator aIter(maMap.begin()), aEnd(maMap.end()); aIter != aEnd; ++aIter, ++pProperties)
        {
            PropertyInfo const * pInfo = (*aIter).second;

            pProperties->Name = pInfo->maName;
            pProperties->Handle = pInfo->mnHandle;
            pProperties->Type = pInfo->maType;
            pProperties->Attributes = pInfo->mnAttributes;
        }
    }
    return maProperties;
}

Property SAL_CALL ChainablePropertySetInfo::getPropertyByName( const OUString& rName )
    throw(::UnknownPropertyException, ::com::sun::star::uno::RuntimeException)
{
    PropertyInfoHash::iterator aIter = maMap.find( rName );

    if ( maMap.end() == aIter )
        throw UnknownPropertyException( rName, *this );

    PropertyInfo const *pInfo = (*aIter).second;
    Property aProperty;
    aProperty.Name   = pInfo->maName;
    aProperty.Handle = pInfo->mnHandle;
    aProperty.Type = pInfo->maType;
    aProperty.Attributes = pInfo->mnAttributes;
    return aProperty;
}

sal_Bool SAL_CALL ChainablePropertySetInfo::hasPropertyByName( const OUString& rName )
    throw(::com::sun::star::uno::RuntimeException)
{
    return static_cast < sal_Bool > ( maMap.find ( rName ) != maMap.end() );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
