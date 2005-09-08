/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: propertycontainerhelper.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 02:58:04 $
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

#ifndef COMPHELPER_PROPERTYCONTAINERHELPER_HXX
#include <comphelper/propertycontainerhelper.hxx>
#endif
#ifndef _COMPHELPER_PROPERTY_HXX_
#include <comphelper/property.hxx>
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _UNO_DATA_H_
#include <uno/data.h>
#endif
#ifndef _COM_SUN_STAR_UNO_GENFUNC_H_
#include <com/sun/star/uno/genfunc.h>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#include <algorithm>

//.........................................................................
namespace comphelper
{
//.........................................................................

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;

//==========================================================================
//= OPropertyContainerHelper
//==========================================================================
//--------------------------------------------------------------------------
OPropertyContainerHelper::OPropertyContainerHelper()
    :m_bAlreadyAccessed(sal_False)
{
}

// -------------------------------------------------------------------------
OPropertyContainerHelper::~OPropertyContainerHelper()
{
}

//--------------------------------------------------------------------------
void OPropertyContainerHelper::registerProperty(const ::rtl::OUString& _rName, sal_Int32 _nHandle,
        sal_Int32 _nAttributes, void* _pPointerToMember, const Type& _rMemberType)
{
    OSL_ENSURE(!m_bAlreadyAccessed, "OPropertyContainerHelper::registerProperty: invalid call, the property sequence was already accessed !");
    OSL_ENSURE((_nAttributes & PropertyAttribute::MAYBEVOID) == 0,
        "OPropertyContainerHelper::registerProperty: don't use this for properties which may be void ! There is a method called \"registerMayBeVoidProperty\" for this !");
    OSL_ENSURE(!_rMemberType.equals(::getCppuType(static_cast< Any* >(NULL))),
        "OPropertyContainerHelper::registerProperty: don't give my the type of an uno::Any ! Really can't handle this !");
    OSL_ENSURE(_pPointerToMember,
        "OPropertyContainerHelper::registerProperty: you gave me nonsense : the pointer must be non-NULL");

    PropertyDescription aNewProp;
    aNewProp.sName = _rName;
    aNewProp.nHandle = _nHandle;
    aNewProp.nAttributes = _nAttributes;
    aNewProp.eLocated = PropertyDescription::ltDerivedClassRealType;
    aNewProp.aLocation.pDerivedClassMember = _pPointerToMember;
    aNewProp.aType = _rMemberType;

    implPushBackProperty(aNewProp);
}

//--------------------------------------------------------------------------
void OPropertyContainerHelper::registerMayBeVoidProperty(const ::rtl::OUString& _rName, sal_Int32 _nHandle, sal_Int32 _nAttributes,
        Any* _pPointerToMember, const Type& _rExpectedType)
{
    OSL_ENSURE(!m_bAlreadyAccessed, "OPropertyContainerHelper::registerMayBeVoidProperty: invalid call, the property sequence was already accessed !");
    OSL_ENSURE((_nAttributes & PropertyAttribute::MAYBEVOID) != 0,
        "OPropertyContainerHelper::registerMayBeVoidProperty: why calling this when the attributes say nothing about may-be-void ?");
    OSL_ENSURE(!_rExpectedType.equals(::getCppuType(static_cast< Any* >(NULL))),
        "OPropertyContainerHelper::registerMayBeVoidProperty: don't give my the type of an uno::Any ! Really can't handle this !");
    OSL_ENSURE(_pPointerToMember,
        "OPropertyContainerHelper::registerMayBeVoidProperty: you gave me nonsense : the pointer must be non-NULL");

    _nAttributes |= PropertyAttribute::MAYBEVOID;

    PropertyDescription aNewProp;
    aNewProp.sName = _rName;
    aNewProp.nHandle = _nHandle;
    aNewProp.nAttributes = _nAttributes;
    aNewProp.eLocated = PropertyDescription::ltDerivedClassAnyType;
    aNewProp.aLocation.pDerivedClassMember = _pPointerToMember;
    aNewProp.aType = _rExpectedType;

    implPushBackProperty(aNewProp);
}


//--------------------------------------------------------------------------
void OPropertyContainerHelper::registerPropertyNoMember(const ::rtl::OUString& _rName, sal_Int32 _nHandle, sal_Int32 _nAttributes,
        const Type& _rType, void* _pInitialValue)
{
    OSL_ENSURE(!m_bAlreadyAccessed, "OPropertyContainerHelper::registerMayBeVoidProperty : invalid call, the property sequence was already accessed !");
    OSL_ENSURE(!_rType.equals(::getCppuType(static_cast< Any* >(NULL))),
        "OPropertyContainerHelper::registerPropertyNoMember : don't give my the type of an uno::Any ! Really can't handle this !");
    OSL_ENSURE(_pInitialValue || ((_nAttributes & PropertyAttribute::MAYBEVOID) != 0),
        "OPropertyContainerHelper::registerPropertyNoMember : you should not ommit the initial value if the property can't be void ! This will definitivly crash later !");

    PropertyDescription aNewProp;
    aNewProp.sName = _rName;
    aNewProp.nHandle = _nHandle;
    aNewProp.nAttributes = _nAttributes;
    aNewProp.eLocated = PropertyDescription::ltHoldMyself;
    aNewProp.aLocation.nOwnClassVectorIndex = m_aHoldProperties.size();
    if (_pInitialValue)
        m_aHoldProperties.push_back(Any(_pInitialValue, _rType));
    else
        m_aHoldProperties.push_back(Any());
    aNewProp.aType = _rType;

    implPushBackProperty(aNewProp);
}

//--------------------------------------------------------------------------
sal_Bool OPropertyContainerHelper::isRegisteredProperty( sal_Int32 _nHandle ) const
{
    return const_cast< OPropertyContainerHelper* >( this )->searchHandle( _nHandle ) != m_aProperties.end();
}

//--------------------------------------------------------------------------
namespace
{
    struct ComparePropertyWithHandle
    {
        bool operator()( const OPropertyContainerHelper::PropertyDescription& _rLHS, sal_Int32 _nRHS ) const
        {
            return _rLHS.nHandle < _nRHS;
        }
        bool operator()( sal_Int32 _nLHS, const OPropertyContainerHelper::PropertyDescription& _rRHS ) const
        {
            return _nLHS < _rRHS.nHandle;
        }
    };
}

//--------------------------------------------------------------------------
void OPropertyContainerHelper::implPushBackProperty(const PropertyDescription& _rProp)
{
#ifdef DBG_UTIL
    for (   PropertiesIterator checkConflicts = m_aProperties.begin();
            checkConflicts != m_aProperties.end();
            ++checkConflicts
        )
    {
        OSL_ENSURE(checkConflicts->sName != _rProp.sName, "OPropertyContainerHelper::implPushBackProperty: name already exists!");
        OSL_ENSURE(checkConflicts->nHandle != _rProp.nHandle, "OPropertyContainerHelper::implPushBackProperty: name already exists!");
    }
#endif

    // need one more element
/*  sal_Int32 nOldLen = m_aProperties.size();
    m_aProperties.resize(nOldLen + 1);
    PropertiesIterator aIter = m_aProperties.begin() + nOldLen - 1;

    // search the corect position, shifting the elements to the tail if needed
    sal_Int32 nPos = nOldLen;
    while ( nPos )
    {
        if ( _rProp.nHandle > aIter->nHandle )
            break;

        *(aIter+1) = *aIter;
        --aIter;
        --nPos;
    }

    m_aProperties[nPos] = _rProp;
*/

    PropertiesIterator pos = ::std::lower_bound( m_aProperties.begin(), m_aProperties.end(), _rProp.nHandle, ComparePropertyWithHandle() );
//    if ( pos == m_aProperties.end() )
//        pos = m_aProperties.begin();

    m_aProperties.insert( pos, _rProp );
}

//--------------------------------------------------------------------------
sal_Bool OPropertyContainerHelper::convertFastPropertyValue(
    Any& _rConvertedValue, Any& _rOldValue, sal_Int32 _nHandle, const Any& _rValue ) SAL_THROW( (IllegalArgumentException) )
{
#ifdef DBG_UTIL
    m_bAlreadyAccessed = sal_True;
#endif
    sal_Bool bModified = sal_False;

    // get the property somebody is asking for
    PropertiesIterator aPos = searchHandle(_nHandle);
    if (aPos == m_aProperties.end())
    {
        OSL_ASSERT("OPropertyContainerHelper::convertFastPropertyValue: unknown handle !");
        // should not happen if the derived class has built a correct property set info helper to be used by
        // our base class OPropertySetHelper
        return bModified;
    }

    switch (aPos->eLocated)
    {
        // similar handling for the two cases where the value is stored in an any
        case PropertyDescription::ltHoldMyself:
        case PropertyDescription::ltDerivedClassAnyType:
        {
            sal_Bool bMayBeVoid = ((aPos->nAttributes & PropertyAttribute::MAYBEVOID) != 0);


            // non modifiable version of the value-to-be-set
            Any aNewRequestedValue( _rValue );

            // normalization
            // (#102329# - 2002-08-14 - fs@openoffice.org)
            // (#i29490# - 2004-06-16 - fs@openoffice.org)
            if ( !aNewRequestedValue.getValueType().equals( aPos->aType ) )
            {   // the actually given value is not of the same type as the one required
                Any aProperlyTyped( NULL, aPos->aType.getTypeLibType() );

                if (    uno_type_assignData(
                            const_cast< void* >( aProperlyTyped.getValue() ), aProperlyTyped.getValueType().getTypeLibType(),
                            const_cast< void* >( aNewRequestedValue.getValue() ), aNewRequestedValue.getValueType().getTypeLibType(),
                            cpp_queryInterface, cpp_acquire, cpp_release
                        )
                    )
                {
                    // we were able to query the given XInterface-derivee for the interface
                    // which is required for this property
                    aNewRequestedValue = aProperlyTyped;
                }
            }

            // argument check
            if  (   !   (   (bMayBeVoid && !aNewRequestedValue.hasValue())          // void is allowed if the attribute says so
                        ||  (aNewRequestedValue.getValueType().equals(aPos->aType)) // else the types have to be equal
                        )
                )
            {
                ::rtl::OUStringBuffer sMessage;
                sMessage.appendAscii( "invalid value type for property \"" );
                sMessage.append( aPos->sName );
                sMessage.appendAscii( "\"\n" );
                sMessage.appendAscii( "expected: " );
                sMessage.append( aPos->aType.getTypeName() );
                sMessage.appendAscii( "\nfound   : " );
                sMessage.append( _rValue.getValueType().getTypeName() );
                sMessage.appendAscii( "\n" );

                throw IllegalArgumentException(
                    sMessage.makeStringAndClear(),
                    NULL,
                    4
                );
            }

            Any* pPropContainer = NULL;
                // the pointer to the any which holds the property value, no matter if located in the derived clas
                // or in out vector

            if (PropertyDescription::ltHoldMyself == aPos->eLocated)
            {
                OSL_ENSURE(aPos->aLocation.nOwnClassVectorIndex < (sal_Int32)m_aHoldProperties.size(),
                    "OPropertyContainerHelper::convertFastPropertyValue: invalid position !");
                PropertyContainerIterator aIter = m_aHoldProperties.begin() + aPos->aLocation.nOwnClassVectorIndex;
                pPropContainer = &(*aIter);
            }
            else
                pPropContainer = reinterpret_cast<Any*>(aPos->aLocation.pDerivedClassMember);

            // check if the new value differs from the current one
            if (!pPropContainer->hasValue() || !aNewRequestedValue.hasValue())
                bModified = pPropContainer->hasValue() != aNewRequestedValue.hasValue();
            else
                bModified = !uno_type_equalData(
                                const_cast< void* >( pPropContainer->getValue() ), aPos->aType.getTypeLibType(),
                                const_cast< void* >( aNewRequestedValue.getValue() ), aPos->aType.getTypeLibType(),
                                cpp_queryInterface, cpp_release
                            );

            if (bModified)
            {
                _rOldValue = *pPropContainer;
                _rConvertedValue = aNewRequestedValue;
            }
        }
        break;
        case PropertyDescription::ltDerivedClassRealType:
            // let the UNO runtime library do any possible conversion
            // this may include a change of the type - for instance, if a LONG is required,
            // but a short is given, then this is valid, as it can be converted without any potential
            // data loss

            Any aProperlyTyped;
            const Any* pNewValue = &_rValue;

            if (!_rValue.getValueType().equals(aPos->aType))
            {
                sal_Bool bConverted = sal_False;

                // a temporary any of the correct (required) type
                aProperlyTyped = Any( NULL, aPos->aType.getTypeLibType() );
                    // (need this as we do not want to overwrite the derived class member here)

                if (    uno_type_assignData(
                            const_cast<void*>(aProperlyTyped.getValue()), aProperlyTyped.getValueType().getTypeLibType(),
                            const_cast<void*>(_rValue.getValue()), _rValue.getValueType().getTypeLibType(),
                            cpp_queryInterface, cpp_acquire, cpp_release
                        )
                    )
                {
                    // could query for the requested interface
                    bConverted = sal_True;
                    pNewValue = &aProperlyTyped;
                }

                if ( !bConverted )
                    // TODO: error message
                    throw IllegalArgumentException();
            }

            // from here on, we should have the proper type
            OSL_ENSURE( pNewValue->getValueType() == aPos->aType,
                "OPropertyContainerHelper::convertFastPropertyValue: conversion failed!" );
            bModified = !uno_type_equalData(
                            aPos->aLocation.pDerivedClassMember, aPos->aType.getTypeLibType(),
                            const_cast<void*>(pNewValue->getValue()), aPos->aType.getTypeLibType(),
                            cpp_queryInterface, cpp_release
                        );

            if (bModified)
            {
                _rOldValue.setValue(aPos->aLocation.pDerivedClassMember, aPos->aType);
                _rConvertedValue = *pNewValue;
            }
            break;
    }

    return bModified;
}

//--------------------------------------------------------------------------
void OPropertyContainerHelper::setFastPropertyValue(sal_Int32 _nHandle, const Any& _rValue) SAL_THROW( (Exception) )
{
#ifdef DBG_UTIL
    m_bAlreadyAccessed = sal_True;
#endif
    // get the property somebody is asking for
    PropertiesIterator aPos = searchHandle(_nHandle);
    if (aPos == m_aProperties.end())
    {
        OSL_ASSERT("OPropertyContainerHelper::setFastPropertyValue: unknown handle !");
        // should not happen if the derived class has built a correct property set info helper to be used by
        // our base class OPropertySetHelper
        return;
    }

    switch (aPos->eLocated)
    {
        case PropertyDescription::ltHoldMyself:
            m_aHoldProperties[aPos->aLocation.nOwnClassVectorIndex] = _rValue;
            break;

        case PropertyDescription::ltDerivedClassAnyType:
            *reinterpret_cast< Any* >(aPos->aLocation.pDerivedClassMember) = _rValue;
            break;

        case PropertyDescription::ltDerivedClassRealType:
#if OSL_DEBUG_LEVEL > 0
            sal_Bool bSuccess =
#endif
            // copy the data from the to-be-set value
            uno_type_assignData(
                aPos->aLocation.pDerivedClassMember,        aPos->aType.getTypeLibType(),
                const_cast< void* >( _rValue.getValue() ),  _rValue.getValueType().getTypeLibType(),
                cpp_queryInterface,
                cpp_acquire, cpp_release );

            OSL_ENSURE( bSuccess,
                "OPropertyContainerHelper::setFastPropertyValue: ooops .... the value could not be assigned!");

            break;
    }
}

//--------------------------------------------------------------------------
void OPropertyContainerHelper::getFastPropertyValue(Any& _rValue, sal_Int32 _nHandle) const
{
#ifdef DBG_UTIL
    const_cast<OPropertyContainerHelper*>(this)->m_bAlreadyAccessed = sal_True;
        // (are we allowed to use the mutable keyword? Means do all our compilers support this meanwhile?)
#endif

    // get the property somebody is asking for
    PropertiesIterator aPos = const_cast<OPropertyContainerHelper*>(this)->searchHandle(_nHandle);
    if (aPos == m_aProperties.end())
    {
        OSL_ASSERT("OPropertyContainerHelper::getFastPropertyValue: unknown handle !");
        // should not happen if the derived class has built a correct property set info helper to be used by
        // our base class OPropertySetHelper
        return;
    }

    switch (aPos->eLocated)
    {
        case PropertyDescription::ltHoldMyself:
            OSL_ENSURE(aPos->aLocation.nOwnClassVectorIndex < (sal_Int32)m_aHoldProperties.size(),
                "OPropertyContainerHelper::convertFastPropertyValue: invalid position !");
            _rValue = m_aHoldProperties[aPos->aLocation.nOwnClassVectorIndex];
            break;
        case PropertyDescription::ltDerivedClassAnyType:
            _rValue = *reinterpret_cast<Any*>(aPos->aLocation.pDerivedClassMember);
            break;
        case PropertyDescription::ltDerivedClassRealType:
            _rValue.setValue(aPos->aLocation.pDerivedClassMember, aPos->aType);
            break;
    }
}

//--------------------------------------------------------------------------
OPropertyContainerHelper::PropertiesIterator OPropertyContainerHelper::searchHandle(sal_Int32 _nHandle)
{
    // search a lower bound
    PropertiesIterator aLowerBound = ::std::lower_bound(
        m_aProperties.begin(),
        m_aProperties.end(),
        _nHandle,
        PropertyDescriptionHandleCompare());

    // check for identity
    if ((aLowerBound != m_aProperties.end()) && aLowerBound->nHandle != _nHandle)
        aLowerBound = m_aProperties.end();

    return aLowerBound;
}

//--------------------------------------------------------------------------
void OPropertyContainerHelper::modifyAttributes(sal_Int32 _nHandle, sal_Int32 _nAddAttrib, sal_Int32 _nRemoveAttrib)
{
    OSL_ENSURE(!m_bAlreadyAccessed, "OPropertyContainerHelper::modifyAttributes: invalid call, the property sequence was already accessed !");

    // get the property somebody is asking for
    PropertiesIterator aPos = searchHandle(_nHandle);
    if (aPos == m_aProperties.end())
    {
        OSL_ASSERT("OPropertyContainerHelper::modifyAttributes: invalid handle !");
        // should not happen if the derived class has built a correct property set info helper to be used by
        // our base class OPropertySetHelper
        return;
    }
    aPos->nHandle |= _nAddAttrib;
    aPos->nHandle &= ~_nRemoveAttrib;
}

//--------------------------------------------------------------------------
void OPropertyContainerHelper::describeProperties(Sequence< Property >& _rProps) const
{
    Sequence< Property > aOwnProps(m_aProperties.size());
    Property* pOwnProps = aOwnProps.getArray();

    for (   ConstPropertiesIterator aLoop = m_aProperties.begin();
            aLoop != m_aProperties.end();
            ++aLoop, ++pOwnProps
        )
    {
        pOwnProps->Name = aLoop->sName;
        pOwnProps->Handle = aLoop->nHandle;
        pOwnProps->Attributes = (sal_Int16)aLoop->nAttributes;
        pOwnProps->Type = aLoop->aType;
    }

    // as our property vector is sorted by handles, not by name, we have to sort aOwnProps
    ::std::sort(aOwnProps.getArray(), aOwnProps.getArray() + aOwnProps.getLength(), PropertyCompareByName());

    // unfortunally the STL merge function does not allow the output range to overlap one of the input ranges,
    // so we need an extra sequence
    Sequence< Property > aOutput;
    aOutput.realloc(_rProps.getLength() + aOwnProps.getLength());
    // do the merge
    ::std::merge(   _rProps.getConstArray(), _rProps.getConstArray() + _rProps.getLength(),         // input 1
                    aOwnProps.getConstArray(), aOwnProps.getConstArray() + aOwnProps.getLength(),   // input 2
                    aOutput.getArray(),                                                             // output
                    PropertyCompareByName()                                                         // compare operator
                );

    // copy the output
    _rProps = aOutput;
}

//.........................................................................
}   // namespace comphelper
//.........................................................................


