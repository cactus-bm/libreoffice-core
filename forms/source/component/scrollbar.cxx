/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: scrollbar.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 22:52:11 $
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
#ifndef FORMS_SOURCE_COMPONENT_SCROLLBAR_HXX
#include "scrollbar.hxx"
#endif

#ifndef _COMPHELPER_STREAMSECTION_HXX_
#include <comphelper/streamsection.hxx>
#endif
#ifndef _COMPHELPER_BASIC_IO_HXX_
#include <comphelper/basicio.hxx>
#endif

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif

//--------------------------------------------------------------------------
extern "C" void SAL_CALL createRegistryInfo_OScrollBarModel()
{
    static ::frm::OMultiInstanceAutoRegistration< ::frm::OScrollBarModel >   aRegisterModel;
}

//........................................................................
namespace frm
{
//........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::form;
    using namespace ::com::sun::star::awt;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::util;
    using namespace ::com::sun::star::io;
    using namespace ::com::sun::star::form::binding;

    //====================================================================
    //= helper
    //====================================================================
    //--------------------------------------------------------------------
    Any translateExternalDoubleToControlIntValue(
        const Reference< XValueBinding >& _rxBinding, const Reference< XPropertySet >& _rxProperties,
        const ::rtl::OUString& _rMinValueName, const ::rtl::OUString& _rMaxValueName )
    {
        OSL_PRECOND( _rxBinding.is(), "translateExternalDoubleToControlIntValue: no external binding!" );
        OSL_ENSURE( _rxProperties.is(), "translateExternalDoubleToControlIntValue: no aggregate!?" );

        sal_Int32 nControlValue( 0 );
        if ( _rxBinding.is() )
        {
            Any aExternalValue = _rxBinding->getValue( ::getCppuType( static_cast< double* >( NULL ) ) );
            double nExternalValue = 0;
            if ( aExternalValue >>= nExternalValue )
            {
                if ( ::rtl::math::isInf( nExternalValue ) )
                {
                    // set the minimum or maximum of the scroll values
                    ::rtl::OUString sLimitPropertyName = ::rtl::math::isSignBitSet( nExternalValue )
                        ? _rMinValueName : _rMaxValueName;
                    if ( _rxProperties.is() )
                        _rxProperties->getPropertyValue( sLimitPropertyName ) >>= nControlValue;
                }
                else
                {
                    nControlValue = (sal_Int32)::rtl::math::round( nExternalValue );
                }
            }
            else
            {
                if ( _rxProperties.is() )
                    _rxProperties->getPropertyValue( _rMinValueName ) >>= nControlValue;
            }
        }

        return makeAny( nControlValue );
    }

    //--------------------------------------------------------------------
    Any translateControlIntToExternalDoubleValue( const Any& _rControlIntValue )
    {
        Any aExternalDoubleValue;
        sal_Int32 nScrollValue = 0;
        if ( _rControlIntValue >>= nScrollValue )
            aExternalDoubleValue <<= (double)nScrollValue;
        else
        {
            OSL_ENSURE( sal_False, "translateControlIntToExternalDoubleValue: no integer scroll value!" );
            // aExternalDoubleValue is void here, which is okay for this purpose ...
        }

        return aExternalDoubleValue;
    }

    //====================================================================
    //= OScrollBarModel
    //====================================================================
    //--------------------------------------------------------------------
    DBG_NAME( OScrollBarModel )
    //--------------------------------------------------------------------
    OScrollBarModel::OScrollBarModel( const Reference<XMultiServiceFactory>& _rxFactory )
        :OBoundControlModel( _rxFactory, VCL_CONTROLMODEL_SCROLLBAR, VCL_CONTROL_SCROLLBAR, sal_True, sal_True, sal_False )
        ,m_nDefaultScrollValue( 0 )
    {
        DBG_CTOR( OScrollBarModel, NULL );

        m_nClassId = FormComponentType::SCROLLBAR;
        initValueProperty( PROPERTY_SCROLL_VALUE, PROPERTY_ID_SCROLL_VALUE );
    }

    //--------------------------------------------------------------------
    OScrollBarModel::OScrollBarModel( const OScrollBarModel* _pOriginal, const Reference< XMultiServiceFactory >& _rxFactory )
        :OBoundControlModel( _pOriginal, _rxFactory )
    {
        DBG_CTOR( OScrollBarModel, NULL );
        m_nDefaultScrollValue = _pOriginal->m_nDefaultScrollValue;
    }

    //--------------------------------------------------------------------
    OScrollBarModel::~OScrollBarModel( )
    {
        DBG_DTOR( OScrollBarModel, NULL );
    }

    //--------------------------------------------------------------------
    IMPLEMENT_SERVICE_REGISTRATION_2( OScrollBarModel, OControlModel, FRM_SUN_COMPONENT_SCROLLBAR, BINDABLE_INTEGER_VALUE_RANGE )
        // note that we're passing OControlModel as "base class". This is because
        // OBoundControlModel, our real base class, claims to support the DataAwareControlModel
        // service, which isn't really true for us. We only derive from this class
        // to benefit from the functionality for binding to spreadsheet cells

    //------------------------------------------------------------------------------
    IMPLEMENT_DEFAULT_CLONING( OScrollBarModel )

    //------------------------------------------------------------------------------
    void SAL_CALL OScrollBarModel::disposing()
    {
        OBoundControlModel::disposing();
    }

    //--------------------------------------------------------------------
    Reference< XPropertySetInfo > SAL_CALL OScrollBarModel::getPropertySetInfo() throw(RuntimeException)
    {
        return Reference< XPropertySetInfo >( createPropertySetInfo( getInfoHelper() ) );
    }

    //--------------------------------------------------------------------
    cppu::IPropertyArrayHelper& OScrollBarModel::getInfoHelper()
    {
        return *const_cast< OScrollBarModel* >( this )->getArrayHelper();
    }

    //--------------------------------------------------------------------
    void OScrollBarModel::fillProperties( Sequence< Property >& _rProps, Sequence< Property >& _rAggregateProps ) const
    {
        BEGIN_DESCRIBE_PROPERTIES( 3, OControlModel )
            DECL_PROP1( DEFAULT_SCROLL_VALUE, sal_Int32,       BOUND );
            DECL_PROP1( TABINDEX,             sal_Int16,       BOUND );
            DECL_PROP2( CONTROLSOURCEPROPERTY,::rtl::OUString, READONLY, TRANSIENT );
        END_DESCRIBE_PROPERTIES();
    }

    //------------------------------------------------------------------------------
    void OScrollBarModel::getFastPropertyValue( Any& _rValue, sal_Int32 _nHandle ) const
    {
        switch ( _nHandle )
        {
            case PROPERTY_ID_DEFAULT_SCROLL_VALUE:
                _rValue <<= m_nDefaultScrollValue;
                break;

            default:
                OBoundControlModel::getFastPropertyValue( _rValue, _nHandle );
        }
    }

    //------------------------------------------------------------------------------
    void OScrollBarModel::setFastPropertyValue_NoBroadcast( sal_Int32 _nHandle, const Any& _rValue ) throw ( Exception )
    {
        switch ( _nHandle )
        {
            case PROPERTY_ID_DEFAULT_SCROLL_VALUE:
                OSL_VERIFY( _rValue >>= m_nDefaultScrollValue );
                resetNoBroadcast();
                break;

            default:
                OBoundControlModel::setFastPropertyValue_NoBroadcast( _nHandle, _rValue );
        }
    }

    //------------------------------------------------------------------------------
    sal_Bool OScrollBarModel::convertFastPropertyValue(
                Any& _rConvertedValue, Any& _rOldValue, sal_Int32 _nHandle, const Any& _rValue )
                throw ( IllegalArgumentException )
    {
        sal_Bool bModified( sal_False );
        switch ( _nHandle )
        {
            case PROPERTY_ID_DEFAULT_SCROLL_VALUE:
                bModified = tryPropertyValue( _rConvertedValue, _rOldValue, _rValue, m_nDefaultScrollValue );
                break;

            default:
                bModified = OBoundControlModel::convertFastPropertyValue( _rConvertedValue, _rOldValue, _nHandle, _rValue );
                break;
        }
        return bModified;
    }

    //--------------------------------------------------------------------
    Any OScrollBarModel::getPropertyDefaultByHandle( sal_Int32 _nHandle ) const
    {
        Any aReturn;

        switch ( _nHandle )
        {
        case PROPERTY_ID_DEFAULT_SCROLL_VALUE:
            aReturn <<= (sal_Int32)0;
            break;

        default:
            aReturn = OBoundControlModel::getPropertyDefaultByHandle( _nHandle );
            break;
        }

        return aReturn;
    }

    //------------------------------------------------------------------------------
    Any OScrollBarModel::translateDbColumnToControlValue( )
    {
        OSL_ENSURE( sal_False, "OScrollBarModel::commitControlValueToDbColumn: never to be called (we're not bound)!" );
        return Any();
    }

    //------------------------------------------------------------------------------
    sal_Bool OScrollBarModel::commitControlValueToDbColumn( bool _bPostReset )
    {
        OSL_ENSURE( sal_False, "OScrollBarModel::commitControlValueToDbColumn: never to be called (we're not bound)!" );
        return sal_True;
    }

    //------------------------------------------------------------------------------
    Any OScrollBarModel::getDefaultForReset() const
    {
        return makeAny( (sal_Int32)m_nDefaultScrollValue );
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL OScrollBarModel::getServiceName() throw( RuntimeException )
    {
        return FRM_SUN_COMPONENT_SCROLLBAR;
    }

    //--------------------------------------------------------------------
    void SAL_CALL OScrollBarModel::write( const Reference< XObjectOutputStream >& _rxOutStream )
        throw( IOException, RuntimeException )
    {
        OBoundControlModel::write( _rxOutStream );
        ::osl::MutexGuard aGuard( m_aMutex );

        OStreamSection aSection( Reference< XDataOutputStream >( _rxOutStream, UNO_QUERY ) );

        // version
        _rxOutStream->writeShort( 0x0001 );

        // properties
        _rxOutStream << m_nDefaultScrollValue;
        writeHelpTextCompatibly( _rxOutStream );
    }

    //--------------------------------------------------------------------
    void SAL_CALL OScrollBarModel::read( const Reference< XObjectInputStream>& _rxInStream ) throw( IOException, RuntimeException )
    {
        OBoundControlModel::read( _rxInStream );
        ::osl::MutexGuard aGuard( m_aMutex );

        // version
        {
            OStreamSection aSection( Reference< XDataInputStream >( _rxInStream, UNO_QUERY ) );

            sal_uInt16 nVersion = _rxInStream->readShort();
            if ( nVersion == 0x0001 )
            {
                _rxInStream >> m_nDefaultScrollValue;
                readHelpTextCompatibly( _rxInStream );
            }
            else
                defaultCommonProperties();

            // here, everything in the stream section which is left will be skipped
        }
    }

    //--------------------------------------------------------------------
    Any OScrollBarModel::translateExternalValueToControlValue( ) const
    {
        return translateExternalDoubleToControlIntValue( getExternalValueBinding(), m_xAggregateSet,
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ScrollValueMin" ) ),
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ScrollValueMax" ) ) );
    }

    //--------------------------------------------------------------------
    Any OScrollBarModel::translateControlValueToExternalValue( ) const
    {
        // by definition, the base class simply obtains the property value
        return translateControlIntToExternalDoubleValue( OBoundControlModel::translateControlValueToExternalValue() );
    }

    //--------------------------------------------------------------------
    sal_Bool OScrollBarModel::approveValueBinding( const Reference< XValueBinding >& _rxBinding )
    {
        OSL_PRECOND( _rxBinding.is(), "OScrollBarModel::approveValueBinding: invalid binding!" );

        // only strings are accepted for simplicity
        return  _rxBinding.is()
            &&  _rxBinding->supportsType( ::getCppuType( static_cast< double* >( NULL ) ) );
    }

//........................................................................
}   // namespace frm
//........................................................................
