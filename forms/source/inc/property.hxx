/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: property.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: obo $ $Date: 2007-03-09 13:35:48 $
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

#ifndef _FRM_PROPERTY_HXX_
#define _FRM_PROPERTY_HXX_

#ifndef _COM_SUN_STAR_UNO_XAGGREGATION_HPP_
#include  <com/sun/star/uno/XAggregation.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSTATE_HPP_
#include <com/sun/star/beans/XPropertyState.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif

#ifndef _CPPUHELPER_PROPSHLP_HXX
#include <cppuhelper/propshlp.hxx>
#endif
#ifndef _CPPUHELPER_PROPTYPEHLP_HXX
#include <cppuhelper/proptypehlp.hxx>
#endif

#ifndef _COMPHELPER_PROPERTY_HXX_
#include <comphelper/property.hxx>
#endif
#ifndef _COMPHELPER_PROPERTY_AGGREGATION_HXX_
#include <comphelper/propagg.hxx>
#endif
#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif

using namespace comphelper;

//=========================================================================
//= property helper classes
//=========================================================================

//... namespace frm .......................................................
namespace frm
{
//.........................................................................

//==================================================================
//= assigment property handle <-> property name
//= used by the PropertySetAggregationHelper
//==================================================================

class PropertyInfoService
{
    //..................................................................
    struct PropertyAssignment
    {
        ::rtl::OUString     sName;
        sal_Int32           nHandle;

        PropertyAssignment() { nHandle = -1; }
        PropertyAssignment(const PropertyAssignment& _rSource)
            :sName(_rSource.sName), nHandle(_rSource.nHandle) { }
        PropertyAssignment(const ::rtl::OUString& _rName, sal_Int32 _nHandle)
            :sName(_rName), nHandle(_nHandle) { }

    };

    DECLARE_STL_VECTOR(PropertyAssignment, PropertyMap);
    static PropertyMap      s_AllKnownProperties;

    //..................................................................
    // comparing two PropertyAssignment's
public:
    typedef PropertyAssignment PUBLIC_SOLARIS_COMPILER_HACK;
        // did not get the following compiled under with SUNPRO 5 without this
        // public typedef
private:
    friend struct PropertyAssignmentNameCompareLess;
    typedef binary_function< PUBLIC_SOLARIS_COMPILER_HACK, PUBLIC_SOLARIS_COMPILER_HACK, sal_Bool > PropertyAssignmentNameCompareLess_Base;
    struct PropertyAssignmentNameCompareLess : public PropertyAssignmentNameCompareLess_Base
    {
        inline sal_Bool operator() (const PUBLIC_SOLARIS_COMPILER_HACK& _rL, const PUBLIC_SOLARIS_COMPILER_HACK& _rR) const
        {
            return (_rL.sName.compareTo(_rR.sName) < 0);
        }
    };

public:
    PropertyInfoService() { }

public:
    static sal_Int32            getPropertyId(const ::rtl::OUString& _rName);
    static ::rtl::OUString      getPropertyName(sal_Int32 _nHandle);

private:
    static void initialize();
};

//..................................................................
// a class implementing the comphelper::IPropertyInfoService
class ConcretInfoService : public ::comphelper::IPropertyInfoService
{
public:
    virtual sal_Int32 getPreferedPropertyId(const ::rtl::OUString& _rName);
};

//------------------------------------------------------------------------------
#define DECL_PROP_IMPL(varname, type) \
    *pProperties++ = com::sun::star::beans::Property(PROPERTY_##varname, PROPERTY_ID_##varname, ::getCppuType(reinterpret_cast< type* >(NULL)),

//------------------------------------------------------------------------------
#define DECL_BOOL_PROP_IMPL(varname) \
    *pProperties++ = com::sun::star::beans::Property(PROPERTY_##varname, PROPERTY_ID_##varname, ::getBooleanCppuType(),

//------------------------------------------------------------------------------
#define DECL_IFACE_PROP_IMPL(varname, type) \
    *pProperties++ = com::sun::star::beans::Property(PROPERTY_##varname, PROPERTY_ID_##varname, ::getCppuType(reinterpret_cast< com::sun::star::uno::Reference< type >* >(NULL)),

//------------------------------------------------------------------------------
#define BEGIN_DESCRIBE_PROPERTIES( count, baseclass )   \
    baseclass::describeFixedProperties( _rProps ); \
    sal_Int32 nOldCount = _rProps.getLength(); \
    _rProps.realloc( nOldCount + ( count ) );   \
    ::com::sun::star::beans::Property* pProperties = _rProps.getArray() + nOldCount;       \

//------------------------------------------------------------------------------
#define BEGIN_DESCRIBE_BASE_PROPERTIES( count ) \
    _rProps.realloc( count );   \
    ::com::sun::star::beans::Property* pProperties = _rProps.getArray();       \

//------------------------------------------------------------------------------
#define BEGIN_DESCRIBE_AGGREGATION_PROPERTIES( count, aggregate )   \
    _rProps.realloc( count );   \
    ::com::sun::star::beans::Property* pProperties = _rProps.getArray();       \
    \
    if (aggregate.is()) \
        _rAggregateProps = aggregate->getPropertySetInfo()->getProperties();    \

// ===
//------------------------------------------------------------------------------
#define DECL_PROP0(varname, type)   \
    DECL_PROP_IMPL(varname, type) 0)

//------------------------------------------------------------------------------
#define DECL_PROP1(varname, type, attrib1)  \
        DECL_PROP_IMPL(varname, type) com::sun::star::beans::PropertyAttribute::attrib1)

//------------------------------------------------------------------------------
#define DECL_PROP2(varname, type, attrib1, attrib2) \
        DECL_PROP_IMPL(varname, type) com::sun::star::beans::PropertyAttribute::attrib1 | com::sun::star::beans::PropertyAttribute::attrib2)

//------------------------------------------------------------------------------
#define DECL_PROP3(varname, type, attrib1, attrib2, attrib3)    \
        DECL_PROP_IMPL(varname, type) com::sun::star::beans::PropertyAttribute::attrib1 | com::sun::star::beans::PropertyAttribute::attrib2 | com::sun::star::beans::PropertyAttribute::attrib3)

//------------------------------------------------------------------------------
#define DECL_PROP4(varname, type, attrib1, attrib2, attrib3, attrib4)   \
        DECL_PROP_IMPL(varname, type) com::sun::star::beans::PropertyAttribute::attrib1 | com::sun::star::beans::PropertyAttribute::attrib2 | com::sun::star::beans::PropertyAttribute::attrib3 | com::sun::star::beans::PropertyAttribute::attrib4)

// === some property types require special handling
// === such as interfaces
//------------------------------------------------------------------------------
#define DECL_IFACE_PROP0(varname, type) \
    DECL_IFACE_PROP_IMPL(varname, type) 0)

//------------------------------------------------------------------------------
#define DECL_IFACE_PROP1(varname, type, attrib1)    \
    DECL_IFACE_PROP_IMPL(varname, type) starbeans::PropertyAttribute::attrib1)

//------------------------------------------------------------------------------
#define DECL_IFACE_PROP2(varname, type, attrib1, attrib2)   \
        DECL_IFACE_PROP_IMPL(varname, type) com::sun::star::beans::PropertyAttribute::attrib1 | com::sun::star::beans::PropertyAttribute::attrib2)

//------------------------------------------------------------------------------
#define DECL_IFACE_PROP3(varname, type, attrib1, attrib2, attrib3)  \
    DECL_IFACE_PROP_IMPL(varname, type) starbeans::PropertyAttribute::attrib1 | starbeans::PropertyAttribute::attrib2 | starbeans::PropertyAttribute::attrib3)

//------------------------------------------------------------------------------
#define DECL_IFACE_PROP4(varname, type, attrib1, attrib2, attrib3, attrib4) \
    DECL_IFACE_PROP_IMPL(varname, type) starbeans::PropertyAttribute::attrib1 | starbeans::PropertyAttribute::attrib2 | starbeans::PropertyAttribute::attrib3 | PropertyAttribute::attrib4)

// === or Boolean properties
//------------------------------------------------------------------------------
#define DECL_BOOL_PROP0(varname)    \
    DECL_BOOL_PROP_IMPL(varname) 0)

//------------------------------------------------------------------------------
#define DECL_BOOL_PROP1(varname, attrib1)   \
        DECL_BOOL_PROP_IMPL(varname) com::sun::star::beans::PropertyAttribute::attrib1)

//------------------------------------------------------------------------------
#define DECL_BOOL_PROP2(varname, attrib1, attrib2)  \
        DECL_BOOL_PROP_IMPL(varname) com::sun::star::beans::PropertyAttribute::attrib1 | com::sun::star::beans::PropertyAttribute::attrib2)

//------------------------------------------------------------------------------
#define DECL_BOOL_PROP3( varname, attrib1, attrib2, attrib3 )   \
        DECL_BOOL_PROP_IMPL(varname) com::sun::star::beans::PropertyAttribute::attrib1 | com::sun::star::beans::PropertyAttribute::attrib2 | com::sun::star::beans::PropertyAttribute::attrib3 )

// ===
//------------------------------------------------------------------------------
#define END_DESCRIBE_PROPERTIES()   \
    DBG_ASSERT( pProperties == _rProps.getArray() + _rProps.getLength(), "<...>::describeFixedProperties/getInfoHelper: forgot to adjust the count ?"); \

//==============================================================================
//------------------------------------------------------------------------------
#define REGISTER_PROP_1( prop, member, attrib1 ) \
    registerProperty( PROPERTY_##prop, PROPERTY_ID_##prop, PropertyAttribute::attrib1, \
        &member, ::getCppuType( &member ) );

#define REGISTER_PROP_2( prop, member, attrib1, attrib2 ) \
    registerProperty( PROPERTY_##prop, PROPERTY_ID_##prop, PropertyAttribute::attrib1 | PropertyAttribute::attrib2, \
        &member, ::getCppuType( &member ) );

//------------------------------------------------------------------------------
#define REGISTER_VOID_PROP_1( prop, memberAny, type, attrib1 ) \
    registerMayBeVoidProperty( PROPERTY_##prop, PROPERTY_ID_##prop, PropertyAttribute::MAYBEVOID | PropertyAttribute::attrib1, \
        &memberAny, ::getCppuType( static_cast< type* >( NULL ) ) );

#define REGISTER_VOID_PROP_2( prop, memberAny, type, attrib1, attrib2 ) \
    registerMayBeVoidProperty( PROPERTY_##prop, PROPERTY_ID_##prop, PropertyAttribute::MAYBEVOID | PropertyAttribute::attrib1 | PropertyAttribute::attrib2, \
        &memberAny, ::getCppuType( static_cast< type* >( NULL ) ) );

//.........................................................................
}
//... namespace frm .......................................................

#endif // _FRM_PROPERTY_HXX_

