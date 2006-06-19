/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: gridcolumnproptranslator.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 18:19:21 $
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

#ifndef XMLOFF_SOURCE_FORMS_GRIDCOLUMNPROPTRANSLATOR_HXX
#include "gridcolumnproptranslator.hxx"
#endif

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSETINFO_HPP_
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_TEXTALIGN_HPP_
#include <com/sun/star/awt/TextAlign.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_PARAGRAPHADJUST_HPP_
#include <com/sun/star/style/ParagraphAdjust.hpp>
#endif
/** === end UNO includes === **/

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif

#include <algorithm>

//........................................................................
namespace xmloff
{
//........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::awt;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::style;

    namespace
    {
        //----------------------------------------------------------------
        ::rtl::OUString getParaAlignProperty()
        {
            return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ParaAdjust" ) );
        }

        //----------------------------------------------------------------
        ::rtl::OUString getAlignProperty()
        {
            return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Align" ) );
        }

        //----------------------------------------------------------------
        sal_Int32 findStringElement( const Sequence< ::rtl::OUString >& _rNames, const ::rtl::OUString& _rName )
        {
            const ::rtl::OUString* pStart = _rNames.getConstArray();
            const ::rtl::OUString* pEnd = _rNames.getConstArray() + _rNames.getLength();
            const ::rtl::OUString* pPos = ::std::find( pStart, pEnd, _rName );
            if ( pPos != pEnd )
                return pPos - pStart;
            return -1;
        }

        //----------------------------------------------------------------
        struct AlignmentTranslationEntry
        {
            ParagraphAdjust nParagraphValue;
            sal_Int16       nControlValue;
        }
        AlignmentTranslations[] =
        {
            // note that order matters:
            // valueAlignToParaAdjust and valueParaAdjustToAlign search this map from the _beginning_
            // and use the first matching entry
            { ParagraphAdjust_LEFT,             TextAlign::LEFT     },
            { ParagraphAdjust_CENTER,           TextAlign::CENTER   },
            { ParagraphAdjust_RIGHT,            TextAlign::RIGHT    },
            { ParagraphAdjust_BLOCK,            TextAlign::RIGHT    },
            { ParagraphAdjust_STRETCH,          TextAlign::LEFT     },
            { ParagraphAdjust_MAKE_FIXED_SIZE,  TextAlign::LEFT     },
            { ParagraphAdjust_MAKE_FIXED_SIZE,  -1 }
        };

        //----------------------------------------------------------------
        void valueAlignToParaAdjust(Any& rValue)
        {
            sal_Int16 nValue;
            rValue >>= nValue;
            const AlignmentTranslationEntry* pTranslation = AlignmentTranslations;
            while (-1 != pTranslation->nControlValue)
            {
                if ( nValue == pTranslation->nControlValue )
                {
                    rValue <<= pTranslation->nParagraphValue;
                    return;
                }
                ++pTranslation;
            }
            OSL_ENSURE( sal_False, "valueAlignToParaAdjust: unreachable!" );
        }

        //----------------------------------------------------------------
        void valueParaAdjustToAlign(Any& rValue)
        {
            sal_Int32 nValue;
            rValue >>= nValue;
            const AlignmentTranslationEntry* pTranslation = AlignmentTranslations;
            while ( ParagraphAdjust_MAKE_FIXED_SIZE != pTranslation->nParagraphValue)
            {
                if ( nValue == pTranslation->nParagraphValue)
                {
                    rValue <<= pTranslation->nControlValue;
                    return;
                }
                ++pTranslation;
            }
            OSL_ENSURE( sal_False, "valueParaAdjustToAlign: unreachable!" );
        }

        //====================================================================
        //= OMergedPropertySetInfo
        //====================================================================
        typedef ::cppu::WeakImplHelper1 <   XPropertySetInfo
                                        >   OMergedPropertySetInfo_Base;
        class OMergedPropertySetInfo : public OMergedPropertySetInfo_Base
        {
        private:
            Reference< XPropertySetInfo >   m_xMasterInfo;

        public:
            OMergedPropertySetInfo( const Reference< XPropertySetInfo >& _rxMasterInfo );

        protected:
            virtual ~OMergedPropertySetInfo();

            // XPropertySetInfo
            virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property > SAL_CALL getProperties(  ) throw (::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::beans::Property SAL_CALL getPropertyByName( const ::rtl::OUString& aName ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
            virtual ::sal_Bool SAL_CALL hasPropertyByName( const ::rtl::OUString& Name ) throw (::com::sun::star::uno::RuntimeException);
        };

        //----------------------------------------------------------------
        OMergedPropertySetInfo::OMergedPropertySetInfo( const Reference< XPropertySetInfo >& _rxMasterInfo )
            :m_xMasterInfo( _rxMasterInfo )
        {
            OSL_ENSURE( m_xMasterInfo.is(), "OMergedPropertySetInfo::OMergedPropertySetInfo: hmm?" );
        }

        //----------------------------------------------------------------
        OMergedPropertySetInfo::~OMergedPropertySetInfo()
        {
        }

        //----------------------------------------------------------------
        Sequence< Property > SAL_CALL OMergedPropertySetInfo::getProperties(  ) throw (RuntimeException)
        {
            // add a "ParaAdjust" property to the master properties
            Sequence< Property > aProperties;
            if ( m_xMasterInfo.is() )
                aProperties = m_xMasterInfo->getProperties();

            sal_Int32 nOldLength = aProperties.getLength();
            aProperties.realloc( nOldLength + 1 );
            aProperties[ nOldLength ] = getPropertyByName( getParaAlignProperty() );

            return aProperties;
        }

        //----------------------------------------------------------------
        Property SAL_CALL OMergedPropertySetInfo::getPropertyByName( const ::rtl::OUString& aName ) throw (UnknownPropertyException, RuntimeException)
        {
            if ( aName == getParaAlignProperty() )
                return Property( getParaAlignProperty(), -1,
                    ::getCppuType( static_cast< const ParagraphAdjust* >( NULL ) ), 0 );

            if ( !m_xMasterInfo.is() )
                return Property();

            return m_xMasterInfo->getPropertyByName( aName );
        }

        //----------------------------------------------------------------
        ::sal_Bool SAL_CALL OMergedPropertySetInfo::hasPropertyByName( const ::rtl::OUString& Name ) throw (RuntimeException)
        {
            if ( Name == getParaAlignProperty() )
                return sal_True;

            if ( !m_xMasterInfo.is() )
                return sal_False;

            return m_xMasterInfo->hasPropertyByName( Name );
        }
    }


    //====================================================================
    //= OGridColumnPropertyTranslator
    //====================================================================
    //--------------------------------------------------------------------
    OGridColumnPropertyTranslator::OGridColumnPropertyTranslator( const Reference< XMultiPropertySet >& _rxGridColumn )
        :m_xGridColumn( _rxGridColumn )
    {
        OSL_ENSURE( m_xGridColumn.is(), "OGridColumnPropertyTranslator: invalid grid column!" );
    }

    //--------------------------------------------------------------------
    OGridColumnPropertyTranslator::~OGridColumnPropertyTranslator()
    {
    }

    //--------------------------------------------------------------------
    Reference< XPropertySetInfo > SAL_CALL OGridColumnPropertyTranslator::getPropertySetInfo(  ) throw (RuntimeException)
    {
        Reference< XPropertySetInfo > xColumnPropInfo;
        if ( m_xGridColumn.is() )
            xColumnPropInfo = m_xGridColumn->getPropertySetInfo();
        return new OMergedPropertySetInfo( xColumnPropInfo );
    }

    //--------------------------------------------------------------------
    void SAL_CALL OGridColumnPropertyTranslator::setPropertyValue( const ::rtl::OUString& aPropertyName, const Any& aValue ) throw (UnknownPropertyException, PropertyVetoException, IllegalArgumentException, WrappedTargetException, RuntimeException)
    {
        Sequence< ::rtl::OUString > aNames( &aPropertyName, 1 );
        Sequence< Any >             aValues( &aValue, 1 );
        setPropertyValues( aNames, aValues );
    }

    //--------------------------------------------------------------------
    Any SAL_CALL OGridColumnPropertyTranslator::getPropertyValue( const ::rtl::OUString& PropertyName ) throw (UnknownPropertyException, WrappedTargetException, RuntimeException)
    {
        Sequence< ::rtl::OUString > aNames( &PropertyName, 1 );
        Sequence< Any > aValues = getPropertyValues( aNames );
        OSL_ENSURE( aValues.getLength() == 1, "OGridColumnPropertyTranslator::getPropertyValue: nonsense!" );
        if ( aValues.getLength() == 1 )
            return aValues[0];
        return Any();
    }

    //--------------------------------------------------------------------
    void SAL_CALL OGridColumnPropertyTranslator::addPropertyChangeListener( const ::rtl::OUString&, const Reference< XPropertyChangeListener >& ) throw (UnknownPropertyException, WrappedTargetException, RuntimeException)
    {
        OSL_ENSURE( sal_False, "OGridColumnPropertyTranslator::addPropertyChangeListener: not implemented - this should not be needed!" );
    }

    //--------------------------------------------------------------------
    void SAL_CALL OGridColumnPropertyTranslator::removePropertyChangeListener( const ::rtl::OUString&, const Reference< XPropertyChangeListener >& ) throw (UnknownPropertyException, WrappedTargetException, RuntimeException)
    {
        OSL_ENSURE( sal_False, "OGridColumnPropertyTranslator::removePropertyChangeListener: not implemented - this should not be needed!" );
    }

    //--------------------------------------------------------------------
    void SAL_CALL OGridColumnPropertyTranslator::addVetoableChangeListener( const ::rtl::OUString&, const Reference< XVetoableChangeListener >& ) throw (UnknownPropertyException, WrappedTargetException, RuntimeException)
    {
        OSL_ENSURE( sal_False, "OGridColumnPropertyTranslator::addVetoableChangeListener: not implemented - this should not be needed!" );
    }

    //--------------------------------------------------------------------
    void SAL_CALL OGridColumnPropertyTranslator::removeVetoableChangeListener( const ::rtl::OUString&, const Reference< XVetoableChangeListener >& ) throw (UnknownPropertyException, WrappedTargetException, RuntimeException)
    {
        OSL_ENSURE( sal_False, "OGridColumnPropertyTranslator::removeVetoableChangeListener: not implemented - this should not be needed!" );
    }

    //--------------------------------------------------------------------
    void SAL_CALL OGridColumnPropertyTranslator::setPropertyValues( const Sequence< ::rtl::OUString >& aPropertyNames, const Sequence< Any >& aValues ) throw (PropertyVetoException, IllegalArgumentException, WrappedTargetException, RuntimeException)
    {
        if ( !m_xGridColumn.is() )
            return;

        // if there's ever the need for more than one property being translated, then we should
        // certainly have a more clever implementation than this ...

        Sequence< ::rtl::OUString > aTranslatedNames( aPropertyNames );
        Sequence< Any >             aTranslatedValues( aValues );

        sal_Int32 nParaAlignPos = findStringElement( aTranslatedNames, getParaAlignProperty() );
        if ( nParaAlignPos != -1 )
        {
            aTranslatedNames[ nParaAlignPos ] = getAlignProperty();
            valueParaAdjustToAlign( aTranslatedValues[ nParaAlignPos ] );
        }

        m_xGridColumn->setPropertyValues( aTranslatedNames, aTranslatedValues );
    }

    //--------------------------------------------------------------------
    Sequence< Any > SAL_CALL OGridColumnPropertyTranslator::getPropertyValues( const Sequence< ::rtl::OUString >& aPropertyNames ) throw (RuntimeException)
    {
        Sequence< Any > aValues( aPropertyNames.getLength() );
        if ( !m_xGridColumn.is() )
            return aValues;

        Sequence< ::rtl::OUString > aTranslatedNames( aPropertyNames );
        sal_Int32 nAlignPos = findStringElement( aTranslatedNames, getParaAlignProperty() );
        if ( nAlignPos != -1 )
            aTranslatedNames[ nAlignPos ] = getAlignProperty();

        aValues = m_xGridColumn->getPropertyValues( aPropertyNames );
        if ( nAlignPos != -1 )
            valueAlignToParaAdjust( aValues[ nAlignPos ] );

        return aValues;
    }

    //--------------------------------------------------------------------
    void SAL_CALL OGridColumnPropertyTranslator::addPropertiesChangeListener( const Sequence< ::rtl::OUString >&, const Reference< XPropertiesChangeListener >& ) throw (RuntimeException)
    {
        OSL_ENSURE( sal_False, "OGridColumnPropertyTranslator::addPropertiesChangeListener: not implemented - this should not be needed!" );
    }

    //--------------------------------------------------------------------
    void SAL_CALL OGridColumnPropertyTranslator::removePropertiesChangeListener( const Reference< XPropertiesChangeListener >& ) throw (RuntimeException)
    {
        OSL_ENSURE( sal_False, "OGridColumnPropertyTranslator::removePropertiesChangeListener: not implemented - this should not be needed!" );
    }

    //--------------------------------------------------------------------
    void SAL_CALL OGridColumnPropertyTranslator::firePropertiesChangeEvent( const Sequence< ::rtl::OUString >&, const Reference< XPropertiesChangeListener >& ) throw (RuntimeException)
    {
        OSL_ENSURE( sal_False, "OGridColumnPropertyTranslator::firePropertiesChangeEvent: not implemented - this should not be needed!" );
    }

//........................................................................
} // namespace xmloff
//........................................................................

