/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: propacc.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: obo $ $Date: 2006-10-12 14:25:24 $
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
#include "precompiled_basic.hxx"

#include "propacc.hxx"

#include <tools/urlobj.hxx>
#include <tools/errcode.hxx>
#include <svtools/svarray.hxx>
#include <sbstar.hxx>
#include <sbunoobj.hxx>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace cppu;
using namespace rtl;


//========================================================================

// Deklaration Konvertierung von Sbx nach Uno mit bekannter Zielklasse
Any sbxToUnoValue( SbxVariable* pVar, const Type& rType, Property* pUnoProperty = NULL );

//========================================================================

#ifdef WNT
#define CDECL _cdecl
#endif
#if defined(UNX)
#define CDECL
#endif

int CDECL SbCompare_PropertyValues_Impl( const void *arg1, const void *arg2 )
{
   return ((PropertyValue*)arg1)->Name.compareTo( ((PropertyValue*)arg2)->Name );
}

extern "C" int CDECL SbCompare_UString_PropertyValue_Impl( const void *arg1, const void *arg2 )
{
    const OUString *pArg1 = (OUString*) arg1;
    const PropertyValue **pArg2 = (const PropertyValue**) arg2;
    return pArg1->compareTo( (*pArg2)->Name );
}

int CDECL SbCompare_Properties_Impl( const void *arg1, const void *arg2 )
{
   return ((Property*)arg1)->Name.compareTo( ((Property*)arg2)->Name );
}

extern "C" int CDECL SbCompare_UString_Property_Impl( const void *arg1, const void *arg2 )
{
    const OUString *pArg1 = (OUString*) arg1;
    const Property *pArg2 = (Property*) arg2;
    return pArg1->compareTo( pArg2->Name );
}

//----------------------------------------------------------------------------

SbPropertyValues::SbPropertyValues()
{
}

//----------------------------------------------------------------------------

SbPropertyValues::~SbPropertyValues()
{
    _xInfo = Reference< XPropertySetInfo >();

    for ( USHORT n = 0; n < _aPropVals.Count(); ++n )
        delete _aPropVals.GetObject( n );
}

//----------------------------------------------------------------------------

Reference< XPropertySetInfo > SbPropertyValues::getPropertySetInfo(void) throw( RuntimeException )
{
    // create on demand?
    if ( !_xInfo.is() )
    {
        SbPropertySetInfo *pInfo = new SbPropertySetInfo( _aPropVals );
        ((SbPropertyValues*)this)->_xInfo = (XPropertySetInfo*)pInfo;
    }
    return _xInfo;
}

//-------------------------------------------------------------------------

INT32 SbPropertyValues::GetIndex_Impl( const OUString &rPropName ) const
{
    PropertyValue **ppPV;
    ppPV = (PropertyValue **)
            bsearch( &rPropName, _aPropVals.GetData(), _aPropVals.Count(),
                      sizeof( PropertyValue* ),
                      SbCompare_UString_PropertyValue_Impl );
    return ppPV ? ( (ppPV-_aPropVals.GetData()) / sizeof(ppPV) ) : USHRT_MAX;
}

//----------------------------------------------------------------------------

void SbPropertyValues::setPropertyValue(
                    const OUString& aPropertyName,
                    const Any& aValue)
                    throw (::com::sun::star::beans::UnknownPropertyException,
                    ::com::sun::star::beans::PropertyVetoException,
                    ::com::sun::star::lang::IllegalArgumentException,
                    ::com::sun::star::lang::WrappedTargetException,
                    ::com::sun::star::uno::RuntimeException)
{
    INT32 nIndex = GetIndex_Impl( aPropertyName );
    PropertyValue *pPropVal = _aPropVals.GetObject(
        sal::static_int_cast< USHORT >(nIndex));
    pPropVal->Value = aValue;
}

//----------------------------------------------------------------------------

Any SbPropertyValues::getPropertyValue(
                    const OUString& aPropertyName)
                    throw(::com::sun::star::beans::UnknownPropertyException,
                    ::com::sun::star::lang::WrappedTargetException,
                    ::com::sun::star::uno::RuntimeException)
{
    INT32 nIndex = GetIndex_Impl( aPropertyName );
    if ( nIndex != USHRT_MAX )
        return _aPropVals.GetObject(
            sal::static_int_cast< USHORT >(nIndex))->Value;
    return Any();
}

//----------------------------------------------------------------------------

void SbPropertyValues::addPropertyChangeListener(
                    const OUString& aPropertyName,
                    const Reference< XPropertyChangeListener >& )
                    throw ()
{
    (void)aPropertyName;
}

//----------------------------------------------------------------------------

void SbPropertyValues::removePropertyChangeListener(
                    const OUString& aPropertyName,
                    const Reference< XPropertyChangeListener >& )
                    throw ()
{
    (void)aPropertyName;
}

//----------------------------------------------------------------------------

void SbPropertyValues::addVetoableChangeListener(
                    const OUString& aPropertyName,
                    const Reference< XVetoableChangeListener >& )
                    throw()
{
    (void)aPropertyName;
}

//----------------------------------------------------------------------------

void SbPropertyValues::removeVetoableChangeListener(
                    const OUString& aPropertyName,
                    const Reference< XVetoableChangeListener >& )
                    throw()
{
    (void)aPropertyName;
}

//----------------------------------------------------------------------------

Sequence< PropertyValue > SbPropertyValues::getPropertyValues(void) throw (::com::sun::star::uno::RuntimeException)
{
    Sequence<PropertyValue> aRet( _aPropVals.Count());
    for ( USHORT n = 0; n < _aPropVals.Count(); ++n )
        aRet.getArray()[n] = *_aPropVals.GetObject(n);
    return aRet;
}

//----------------------------------------------------------------------------

void SbPropertyValues::setPropertyValues(const Sequence< PropertyValue >& rPropertyValues )
                     throw (::com::sun::star::beans::UnknownPropertyException,
                     ::com::sun::star::beans::PropertyVetoException,
                     ::com::sun::star::lang::IllegalArgumentException,
                     ::com::sun::star::lang::WrappedTargetException,
                     ::com::sun::star::uno::RuntimeException)
{
    if ( _aPropVals.Count() )
        throw PropertyExistException();

    const PropertyValue *pPropVals = rPropertyValues.getConstArray();
    for ( sal_Int16 n = 0; n < rPropertyValues.getLength(); ++n )
    {
        PropertyValue *pPropVal = new PropertyValue(pPropVals[n]);
        _aPropVals.Insert( pPropVal, n );
    }
}

//============================================================================
//PropertySetInfoImpl

PropertySetInfoImpl::PropertySetInfoImpl()
{
}

INT32 PropertySetInfoImpl::GetIndex_Impl( const OUString &rPropName ) const
{
    Property *pP;
    pP = (Property*)
            bsearch( &rPropName, _aProps.getConstArray(), _aProps.getLength(),
                      sizeof( Property ),
                      SbCompare_UString_Property_Impl );
    return pP ? sal::static_int_cast<INT32>( (pP-_aProps.getConstArray()) / sizeof(pP) ) : -1;
}

Sequence< Property > PropertySetInfoImpl::getProperties(void) throw()
{
    return _aProps;
}

Property PropertySetInfoImpl::getPropertyByName(const OUString& Name) throw( RuntimeException )
{
    sal_Int32 nIndex = GetIndex_Impl( Name );
    if( USHRT_MAX != nIndex )
        return _aProps.getConstArray()[ nIndex ];
    return Property();
}

sal_Bool PropertySetInfoImpl::hasPropertyByName(const OUString& Name) throw( RuntimeException )
{
    sal_Int32 nIndex = GetIndex_Impl( Name );
    return USHRT_MAX != nIndex;
}


//============================================================================

SbPropertySetInfo::SbPropertySetInfo()
{
}

//----------------------------------------------------------------------------

SbPropertySetInfo::SbPropertySetInfo( const SbPropertyValueArr_Impl &rPropVals )
{
    aImpl._aProps.realloc( rPropVals.Count() );
    for ( USHORT n = 0; n < rPropVals.Count(); ++n )
    {
        Property &rProp = aImpl._aProps.getArray()[n];
        const PropertyValue &rPropVal = *rPropVals.GetObject(n);
        rProp.Name = rPropVal.Name;
        rProp.Handle = rPropVal.Handle;
        rProp.Type = getCppuVoidType();
        rProp.Attributes = 0;
    }
}

//----------------------------------------------------------------------------

SbPropertySetInfo::~SbPropertySetInfo()
{
}

//-------------------------------------------------------------------------

Sequence< Property > SbPropertySetInfo::getProperties(void) throw( RuntimeException )
{
    return aImpl.getProperties();
}

Property SbPropertySetInfo::getPropertyByName(const OUString& Name)
    throw( RuntimeException )
{
    return aImpl.getPropertyByName( Name );
}

BOOL SbPropertySetInfo::hasPropertyByName(const OUString& Name)
    throw( RuntimeException )
{
    return aImpl.hasPropertyByName( Name );
}


//----------------------------------------------------------------------------

SbPropertyContainer::SbPropertyContainer()
{
}

//----------------------------------------------------------------------------

SbPropertyContainer::~SbPropertyContainer()
{
}

//----------------------------------------------------------------------------
void SbPropertyContainer::addProperty(const OUString& Name,
                                      INT16 Attributes,
                                      const Any& DefaultValue)
    throw(  PropertyExistException, IllegalTypeException,
            IllegalArgumentException, RuntimeException )
{
    (void)Name;
    (void)Attributes;
    (void)DefaultValue;
}

//----------------------------------------------------------------------------
void SbPropertyContainer::removeProperty(const OUString& Name)
    throw( UnknownPropertyException, RuntimeException )
{
    (void)Name;
}

//----------------------------------------------------------------------------
// XPropertySetInfo
Sequence< Property > SbPropertyContainer::getProperties(void) throw ()
{
    return aImpl.getProperties();
}

Property SbPropertyContainer::getPropertyByName(const OUString& Name)
    throw( RuntimeException )
{
    return aImpl.getPropertyByName( Name );
}

BOOL SbPropertyContainer::hasPropertyByName(const OUString& Name)
    throw( RuntimeException )
{
    return aImpl.hasPropertyByName( Name );
}

//----------------------------------------------------------------------------

Sequence< PropertyValue > SbPropertyContainer::getPropertyValues(void)
{
    return Sequence<PropertyValue>();
}

//----------------------------------------------------------------------------

void SbPropertyContainer::setPropertyValues(const Sequence< PropertyValue >& PropertyValues_)
{
    (void)PropertyValues_;
}

//----------------------------------------------------------------------------

void RTL_Impl_CreatePropertySet( StarBASIC* pBasic, SbxArray& rPar, BOOL bWrite )
{
    (void)pBasic;
    (void)bWrite;

    // Wir brauchen mindestens 1 Parameter
    if ( rPar.Count() < 2 )
    {
        StarBASIC::Error( SbERR_BAD_ARGUMENT );
        return;
    }

    // Klassen-Name der struct holen
    String aServiceName( RTL_CONSTASCII_USTRINGPARAM("stardiv.uno.beans.PropertySet") );

#if 0
    // Service suchen und instanzieren
    Reference< XMultiServiceFactory > xServiceManager = getProcessServiceFactory();
    Reference< XInterface > xInterface;
    if( xProv.is() )
        xInterface = xProv->newInstance();
#else
    Reference< XInterface > xInterface = (OWeakObject*) new SbPropertyValues();
#endif

    SbxVariableRef refVar = rPar.Get(0);
    if( xInterface.is() )
    {
        // PropertyValues setzen
        Any aArgAsAny = sbxToUnoValue( rPar.Get(1),
                getCppuType( (Sequence<PropertyValue>*)0 ) );
        Sequence<PropertyValue> *pArg =
                (Sequence<PropertyValue>*) aArgAsAny.getValue();
        Reference< XPropertyAccess > xPropAcc = Reference< XPropertyAccess >::query( xInterface );
        xPropAcc->setPropertyValues( *pArg );

        // SbUnoObject daraus basteln und zurueckliefern
        Any aAny;
        aAny <<= xInterface;
        SbUnoObjectRef xUnoObj = new SbUnoObject( aServiceName, aAny );
        if( xUnoObj->getUnoAny().getValueType().getTypeClass() != TypeClass_VOID )
        {
            // Objekt zurueckliefern
            refVar->PutObject( (SbUnoObject*)xUnoObj );
            return;
        }
    }

    // Objekt konnte nicht erzeugt werden
    refVar->PutObject( NULL );
}


