/*************************************************************************
 *
 *  $RCSfile: richtextmodel.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: kz $ $Date: 2005-01-21 16:51:55 $
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

#ifndef FORMS_SOURCE_RICHTEXT_RICHTEXTMODEL_CXX
#include "richtextmodel.hxx"
#endif

#ifndef FORMS_SOURCE_RICHTEXT_RICHTEXTENGINE_HXX
#include "richtextengine.hxx"
#endif

#ifndef FORMS_SOURCE_RICHTEXT_RICHTEXTUNOWRAPPER_HXX
#include "richtextunowrapper.hxx"
#endif

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_AWT_LINEENDFORMAT_HPP_
#include <com/sun/star/awt/LineEndFormat.hpp>
#endif
/** === end UNO includes === **/

#ifndef _CPPUHELPER_TYPEPROVIDER_HXX_
#include <cppuhelper/typeprovider.hxx>
#endif

#ifndef _TOOLKIT_AWT_VCLXDEVICE_HXX_
#include <toolkit/awt/vclxdevice.hxx>
#endif
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/helper/vclunohelper.hxx>
#endif

#ifndef _EDITSTAT_HXX
#include <svx/editstat.hxx>
#endif

#ifndef _SV_OUTDEV_HXX
#include <vcl/outdev.hxx>
#endif

//--------------------------------------------------------------------------
extern "C" void SAL_CALL createRegistryInfo_ORichTextModel()
{
    static ::frm::OMultiInstanceAutoRegistration< ::frm::ORichTextModel >   aRegisterModel;
}

//........................................................................
namespace frm
{
//........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::awt;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::io;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::form;
    using namespace ::com::sun::star::util;

    //====================================================================
    //= OModifyListenerContainer
    //====================================================================
    //--------------------------------------------------------------------
    OModifyListenerContainer::OModifyListenerContainer( ::osl::Mutex& _rMutex )
        :OModifyListenerContainer_Base( _rMutex )
    {
    }

    //--------------------------------------------------------------------
    bool OModifyListenerContainer::implNotify( const Reference< XModifyListener >& _rxListener, const EventObject& _rEvent ) SAL_THROW( ( Exception ) )
    {
        _rxListener->modified( _rEvent );
        return true;
    }

    //====================================================================
    //= ORichTextModel
    //====================================================================
    DBG_NAME( ORichTextModel )
    //--------------------------------------------------------------------
    ORichTextModel::ORichTextModel( const Reference< XMultiServiceFactory >& _rxFactory )
        :OControlModel       ( _rxFactory, ::rtl::OUString() )
        ,FontControlModel    ( true                          )
        ,m_pEngine           ( RichTextEngine::Create()      )
        ,m_aModifyListeners  ( m_aMutex                      )
        ,m_bSettingEngineText( false                         )
    {
        DBG_CTOR( ORichTextModel, NULL );
        m_nClassId = FormComponentType::TEXTFIELD;

        getPropertyDefaultByHandle( PROPERTY_ID_DEFAULTCONTROL          ) >>= m_sDefaultControl;
        getPropertyDefaultByHandle( PROPERTY_ID_BORDER                  ) >>= m_nBorder;
        getPropertyDefaultByHandle( PROPERTY_ID_ENABLED                 ) >>= m_bEnabled;
        getPropertyDefaultByHandle( PROPERTY_ID_HARDLINEBREAKS          ) >>= m_bHardLineBreaks;
        getPropertyDefaultByHandle( PROPERTY_ID_HSCROLL                 ) >>= m_bHScroll;
        getPropertyDefaultByHandle( PROPERTY_ID_VSCROLL                 ) >>= m_bVScroll;
        getPropertyDefaultByHandle( PROPERTY_ID_READONLY                ) >>= m_bReadonly;
        getPropertyDefaultByHandle( PROPERTY_ID_PRINTABLE               ) >>= m_bPrintable;
        getPropertyDefaultByHandle( PROPERTY_ID_ALIGN                   ) >>= m_aAlign;
        getPropertyDefaultByHandle( PROPERTY_ID_ECHO_CHAR               ) >>= m_nEchoChar;
        getPropertyDefaultByHandle( PROPERTY_ID_MAXTEXTLEN              ) >>= m_nMaxTextLength;
        getPropertyDefaultByHandle( PROPERTY_ID_MULTILINE               ) >>= m_bMultiLine;
        getPropertyDefaultByHandle( PROPERTY_ID_RICH_TEXT               ) >>= m_bReallyActAsRichText;
        getPropertyDefaultByHandle( PROPERTY_ID_HIDEINACTIVESELECTION   ) >>= m_bHideInactiveSelection;
        getPropertyDefaultByHandle( PROPERTY_ID_LINEEND_FORMAT          ) >>= m_nLineEndFormat;

        implInit();
    }

    //------------------------------------------------------------------
    ORichTextModel::ORichTextModel( const ORichTextModel* _pOriginal, const Reference< XMultiServiceFactory >& _rxFactory )
        :OControlModel       ( _pOriginal, _rxFactory, sal_False )
        ,FontControlModel    ( _pOriginal                        )
        ,m_pEngine           ( NULL                              )
        ,m_aModifyListeners  ( m_aMutex                          )
        ,m_bSettingEngineText( false                             )
    {
        DBG_CTOR( ORichTextModel, NULL );

        m_aTabStop               = _pOriginal->m_aTabStop;
        m_aBackgroundColor       = _pOriginal->m_aBackgroundColor;
        m_aBorderColor           = _pOriginal->m_aBorderColor;
        m_sDefaultControl        = _pOriginal->m_sDefaultControl;
        m_sHelpText              = _pOriginal->m_sHelpText;
        m_sHelpURL               = _pOriginal->m_sHelpURL;
        m_nBorder                = _pOriginal->m_nBorder;
        m_bEnabled               = _pOriginal->m_bEnabled;
        m_bHardLineBreaks        = _pOriginal->m_bHardLineBreaks;
        m_bHScroll               = _pOriginal->m_bHScroll;
        m_bVScroll               = _pOriginal->m_bVScroll;
        m_bReadonly              = _pOriginal->m_bReadonly;
        m_bPrintable             = _pOriginal->m_bPrintable;
        m_bReallyActAsRichText   = _pOriginal->m_bReallyActAsRichText;
        m_bHideInactiveSelection = _pOriginal->m_bHideInactiveSelection;
        m_nLineEndFormat         = _pOriginal->m_nLineEndFormat;

        m_aAlign               = _pOriginal->m_aAlign;
        m_nEchoChar            = _pOriginal->m_nEchoChar;
        m_nMaxTextLength       = _pOriginal->m_nMaxTextLength;
        m_bMultiLine           = _pOriginal->m_bMultiLine;

        m_pEngine              = _pOriginal->m_pEngine->Clone();
        m_sLastKnownEngineText = m_pEngine->GetText();

        implInit();
    }

    //------------------------------------------------------------------
    void ORichTextModel::implInit()
    {
        OSL_ENSURE( m_pEngine, "ORichTextModel::implInit: where's the engine?" );
        if ( m_pEngine )
        {
            m_pEngine->SetModifyHdl( LINK( this, ORichTextModel, OnEngineContentModified ) );

            ULONG nEngineControlWord = m_pEngine->GetControlWord();
            nEngineControlWord = nEngineControlWord & ~EE_CNTRL_AUTOPAGESIZE;
            m_pEngine->SetControlWord( nEngineControlWord );

            VCLXDevice* pUnoRefDevice = new VCLXDevice;
            pUnoRefDevice->SetOutputDevice( m_pEngine->GetRefDevice() );
            m_xReferenceDevice = pUnoRefDevice;
        }

        implDoAggregation();
        implRegisterProperties();
    }

    //------------------------------------------------------------------
    void ORichTextModel::implDoAggregation()
    {
        increment( m_refCount );

        {
            m_xAggregate = new ORichTextUnoWrapper( *m_pEngine, this );
            setAggregation( m_xAggregate );
            doSetDelegator();
        }

        decrement( m_refCount );
    }

    //------------------------------------------------------------------
    void ORichTextModel::implRegisterProperties()
    {
        REGISTER_PROP_2( DEFAULTCONTROL,        m_sDefaultControl,          BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( HELPTEXT,              m_sHelpText,                BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( HELPURL,               m_sHelpURL,                 BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( ENABLED,               m_bEnabled,                 BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( BORDER,                m_nBorder,                  BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( HARDLINEBREAKS,        m_bHardLineBreaks,          BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( HSCROLL,               m_bHScroll,                 BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( VSCROLL,               m_bVScroll,                 BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( READONLY,              m_bReadonly,                BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( PRINTABLE,             m_bPrintable,               BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( REFERENCE_DEVICE,      m_xReferenceDevice,         BOUND, TRANSIENT    );
        REGISTER_PROP_2( RICH_TEXT,             m_bReallyActAsRichText,     BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( HIDEINACTIVESELECTION, m_bHideInactiveSelection,   BOUND, MAYBEDEFAULT );

        REGISTER_VOID_PROP_2( TABSTOP,          m_aTabStop,         sal_Bool,  BOUND, MAYBEDEFAULT );
        REGISTER_VOID_PROP_2( BACKGROUNDCOLOR,  m_aBackgroundColor, sal_Int32, BOUND, MAYBEDEFAULT );
        REGISTER_VOID_PROP_2( BORDERCOLOR,      m_aBorderColor,     sal_Int32, BOUND, MAYBEDEFAULT );

        // properties which exist only for compatibility with the css.swt.UnoControlEditModel,
        // since we intend to replace the default implementation for this service
        REGISTER_PROP_2( ECHO_CHAR,         m_nEchoChar,            BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( MAXTEXTLEN,        m_nMaxTextLength,       BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( MULTILINE,         m_bMultiLine,           BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( TEXT,              m_sLastKnownEngineText, BOUND, MAYBEDEFAULT );
        REGISTER_PROP_2( LINEEND_FORMAT,    m_nLineEndFormat,       BOUND, MAYBEDEFAULT );

        REGISTER_VOID_PROP_2( ALIGN,        m_aAlign,           sal_Int16, BOUND, MAYBEDEFAULT );
    }

    //--------------------------------------------------------------------
    ORichTextModel::~ORichTextModel( )
    {
        if ( !OComponentHelper::rBHelper.bDisposed )
        {
            acquire();
            dispose();
        }

        DBG_DTOR( ORichTextModel, NULL );
    }

    //------------------------------------------------------------------
    Any SAL_CALL ORichTextModel::queryAggregation( const Type& _rType ) throw ( RuntimeException )
    {
        Any aReturn = ORichTextModel_BASE::queryInterface( _rType );

        if ( !aReturn.hasValue() )
            aReturn = OControlModel::queryAggregation( _rType );

        return aReturn;
    }

    //------------------------------------------------------------------
    IMPLEMENT_FORWARD_XTYPEPROVIDER2( ORichTextModel, OControlModel, ORichTextModel_BASE )

    //--------------------------------------------------------------------
    IMPLEMENT_SERVICE_REGISTRATION_7( ORichTextModel, OControlModel,
        FRM_SUN_COMPONENT_RICHTEXTCONTROL,
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.text.TextRange" ) ),
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.style.CharacterProperties" ) ),
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.style.ParagraphProperties" ) ),
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.style.CharacterPropertiesAsian" ) ),
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.style.CharacterPropertiesComplex" ) ),
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.style.ParagraphPropertiesAsian" ) )
    )

    //------------------------------------------------------------------------------
    IMPLEMENT_DEFAULT_CLONING( ORichTextModel )

    //------------------------------------------------------------------------------
    void SAL_CALL ORichTextModel::disposing()
    {
        m_aModifyListeners.disposing( EventObject( *this ) );
        OControlModel::disposing();
    }

    //--------------------------------------------------------------------
    Reference< XPropertySetInfo > SAL_CALL ORichTextModel::getPropertySetInfo() throw ( RuntimeException )
    {
        return createPropertySetInfo( getInfoHelper() );
    }

    //--------------------------------------------------------------------
    ::cppu::IPropertyArrayHelper& SAL_CALL ORichTextModel::getInfoHelper()
    {
        return *const_cast< ORichTextModel* >( this )->getArrayHelper();
    }

    //------------------------------------------------------------------------------
    namespace
    {
        void lcl_removeProperty( Sequence< Property >& _rSeq, const ::rtl::OUString& _rPropertyName )
        {
            Property* pLoop = _rSeq.getArray();
            Property* pEnd = _rSeq.getArray() + _rSeq.getLength();
            while ( pLoop != pEnd )
            {
                if ( pLoop->Name == _rPropertyName )
                {
                    ::std::copy( pLoop + 1, pEnd, pLoop );
                    _rSeq.realloc( _rSeq.getLength() - 1 );
                    break;
                }
                ++pLoop;
            }
        }
    }
    //------------------------------------------------------------------------------
    void ORichTextModel::fillProperties(
            Sequence< Property >& _rProps,
            Sequence< Property >& _rAggregateProps ) const
    {
        BEGIN_DESCRIBE_PROPERTIES( 1, OControlModel )
            DECL_PROP2( TABINDEX,       sal_Int16,  BOUND,    MAYBEDEFAULT );
        END_DESCRIBE_PROPERTIES();

        // properties which the OPropertyContainerHelper is responsible for
        Sequence< Property > aContainedProperties;
        describeProperties( aContainedProperties );

        // properties which the FontControlModel is responsible for
        Sequence< Property > aFontProperties;
        describeFontRelatedProperties( aFontProperties );

        // our aggregate (the SvxUnoText) declares a FontDescriptor property, as does
        // our FormControlFont base class. We remove it from the base class' sequence
        // here, and later on care for both instances being in sync
        lcl_removeProperty( _rAggregateProps, PROPERTY_FONT );

        _rProps = concatSequences(
            aContainedProperties,
            aFontProperties,
            _rProps
        );
    }

    //--------------------------------------------------------------------
    void SAL_CALL ORichTextModel::getFastPropertyValue( Any& _rValue, sal_Int32 _nHandle ) const
    {
        if ( isRegisteredProperty( _nHandle ) )
        {
            OPropertyContainerHelper::getFastPropertyValue( _rValue, _nHandle );
        }
        else if ( isFontRelatedProperty( _nHandle ) )
        {
            FontControlModel::getFastPropertyValue( _rValue, _nHandle );
        }
        else
        {
            OControlModel::getFastPropertyValue( _rValue, _nHandle );
        }
    }

    //--------------------------------------------------------------------
    sal_Bool SAL_CALL ORichTextModel::convertFastPropertyValue( Any& _rConvertedValue, Any& _rOldValue, sal_Int32 _nHandle, const Any& _rValue ) throw( IllegalArgumentException )
    {
        sal_Bool bModified = sal_False;

        if ( isRegisteredProperty( _nHandle ) )
        {
            bModified = OPropertyContainerHelper::convertFastPropertyValue( _rConvertedValue, _rOldValue, _nHandle, _rValue );
        }
        else if ( isFontRelatedProperty( _nHandle ) )
        {
            bModified = FontControlModel::convertFastPropertyValue( _rConvertedValue, _rOldValue, _nHandle, _rValue );
        }
        else
        {
            bModified = OControlModel::convertFastPropertyValue( _rConvertedValue, _rOldValue, _nHandle, _rValue );
        }

        return bModified;
    }

    //--------------------------------------------------------------------
    void SAL_CALL ORichTextModel::setFastPropertyValue_NoBroadcast( sal_Int32 _nHandle, const Any& _rValue ) throw ( Exception)
    {
        if ( isRegisteredProperty( _nHandle ) )
        {
            OPropertyContainerHelper::setFastPropertyValue( _nHandle, _rValue );

            if ( PROPERTY_ID_REFERENCE_DEVICE == _nHandle )
            {
#if OSL_DEBUG_LEVEL > 0
                MapMode aOldMapMode = m_pEngine->GetRefDevice()->GetMapMode();
#endif

                OutputDevice* pRefDevice = VCLUnoHelper::GetOutputDevice( m_xReferenceDevice );
                OSL_ENSURE( pRefDevice, "ORichTextModel::setFastPropertyValue_NoBroadcast: empty reference device?" );
                m_pEngine->SetRefDevice( pRefDevice );

#if OSL_DEBUG_LEVEL > 0
                MapMode aNewMapMode = m_pEngine->GetRefDevice()->GetMapMode();
                OSL_ENSURE( aNewMapMode.GetMapUnit() == aOldMapMode.GetMapUnit(),
                    "ORichTextModel::setFastPropertyValue_NoBroadcast: You should not tamper with the MapUnit of the ref device!" );
                // if this assertion here is triggered, then we would need to adjust all
                // items in all text portions in all paragraphs in the attributes of the EditEngine,
                // as long as they are MapUnit-dependent. This holds at least for the FontSize.
#endif
            }
            else if ( PROPERTY_ID_TEXT == _nHandle )
            {
                implSetEngineText( m_sLastKnownEngineText );
            }
        }
        else if ( isFontRelatedProperty( _nHandle ) )
        {
            FontDescriptor aOldFont( getFont() );

            FontControlModel::setFastPropertyValue_NoBroadcast( _nHandle, _rValue );

            if ( isFontAggregateProperty( _nHandle ) )
                firePropertyChange( PROPERTY_ID_FONT, makeAny( getFont() ), makeAny( aOldFont ) );
        }
        else
        {
            OControlModel::setFastPropertyValue_NoBroadcast( _nHandle, _rValue );
        }
    }

    //--------------------------------------------------------------------
    Any ORichTextModel::getPropertyDefaultByHandle( sal_Int32 _nHandle ) const
    {
        Any aDefault;

        switch ( _nHandle )
        {
        case PROPERTY_ID_LINEEND_FORMAT:
            aDefault <<= (sal_Int16)LineEndFormat::LINE_FEED;
            break;

        case PROPERTY_ID_ECHO_CHAR:
        case PROPERTY_ID_ALIGN:
        case PROPERTY_ID_MAXTEXTLEN:
            aDefault <<= (sal_Int16)0;
            break;

        case PROPERTY_ID_TABSTOP:
        case PROPERTY_ID_BACKGROUNDCOLOR:
        case PROPERTY_ID_BORDERCOLOR:
            /* void */
            break;

        case PROPERTY_ID_ENABLED:
        case PROPERTY_ID_PRINTABLE:
        case PROPERTY_ID_HIDEINACTIVESELECTION:
            aDefault <<= (sal_Bool)sal_True;
            break;

        case PROPERTY_ID_HARDLINEBREAKS:
        case PROPERTY_ID_HSCROLL:
        case PROPERTY_ID_VSCROLL:
        case PROPERTY_ID_READONLY:
        case PROPERTY_ID_MULTILINE:
        case PROPERTY_ID_RICH_TEXT:
            aDefault <<= (sal_Bool)sal_False;
            break;

        case PROPERTY_ID_DEFAULTCONTROL:
            aDefault <<= (::rtl::OUString)FRM_SUN_CONTROL_RICHTEXTCONTROL;
            break;

        case PROPERTY_ID_HELPTEXT:
        case PROPERTY_ID_HELPURL:
        case PROPERTY_ID_TEXT:
            aDefault <<= ::rtl::OUString();
            break;

        case PROPERTY_ID_BORDER:
            aDefault <<= (sal_Int16)1;
            break;

        default:
            if ( isFontRelatedProperty( _nHandle ) )
                aDefault = FontControlModel::getPropertyDefaultByHandle( _nHandle );
            else
                aDefault = OControlModel::getPropertyDefaultByHandle( _nHandle );
        }

        return aDefault;
    }

    //--------------------------------------------------------------------
    void ORichTextModel::implSetEngineText( const ::rtl::OUString& _rText )
    {
        if ( m_pEngine )
        {
            m_bSettingEngineText = true;
            m_pEngine->SetText( _rText );
            m_bSettingEngineText = false;
        }
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL ORichTextModel::getServiceName() throw ( RuntimeException)
    {
        return FRM_SUN_COMPONENT_RICHTEXTCONTROL;
    }

    //--------------------------------------------------------------------
    void SAL_CALL ORichTextModel::write(const Reference< XObjectOutputStream >& _rxOutStream) throw ( IOException, RuntimeException)
    {
        OControlModel::write( _rxOutStream );
        // TODO: place your code here
    }

    //--------------------------------------------------------------------
    void SAL_CALL ORichTextModel::read(const Reference< XObjectInputStream >& _rxInStream) throw ( IOException, RuntimeException)
    {
        OControlModel::read( _rxInStream );
        // TODO: place your code here
    }

    //--------------------------------------------------------------------
    RichTextEngine* ORichTextModel::getEditEngine( const Reference< XControlModel >& _rxModel )
    {
        RichTextEngine* pEngine = NULL;

        Reference< XUnoTunnel > xTunnel( _rxModel, UNO_QUERY );
        OSL_ENSURE( xTunnel.is(), "ORichTextModel::getEditEngine: invalid model!" );
        if ( xTunnel.is() )
        {
            try
            {
                pEngine = reinterpret_cast< RichTextEngine* >( xTunnel->getSomething( getEditEngineTunnelId() ) );
            }
            catch( const Exception& )
            {
                OSL_ENSURE( sal_False, "ORichTextModel::getEditEngine: caught an exception!" );
            }
        }
        return pEngine;
    }

    //--------------------------------------------------------------------
    Sequence< sal_Int8 > ORichTextModel::getEditEngineTunnelId()
    {
        static ::cppu::OImplementationId * pId = 0;
        if (! pId)
        {
            ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
            if (! pId)
            {
                static ::cppu::OImplementationId aId;
                pId = &aId;
            }
        }
        return pId->getImplementationId();
    }

    //--------------------------------------------------------------------
    IMPL_LINK( ORichTextModel, OnEngineContentModified, void*, _pNotInterestedIn )
    {
        if ( !m_bSettingEngineText )
        {
            EventObject aModifyEvent( *this );
            m_aModifyListeners.notify( aModifyEvent );

            potentialTextChange();
                // is this a good idea? It may become expensive in case of larger texts,
                // and this method here is called for every single changed character ...
                // On the other hand, the API *requires* us to notify changes in the "Text"
                // property immediately ...
        }

        return 0L;
    }

    //--------------------------------------------------------------------
    sal_Int64 SAL_CALL ORichTextModel::getSomething( const Sequence< sal_Int8 >& _rId ) throw (RuntimeException)
    {
        Sequence< sal_Int8 > aEditEngineAccessId( getEditEngineTunnelId() );
        if  (   ( _rId.getLength() == aEditEngineAccessId.getLength() )
            &&  ( 0 == rtl_compareMemory( aEditEngineAccessId.getConstArray(),  _rId.getConstArray(), _rId.getLength() ) )
            )
            return reinterpret_cast< sal_Int64 >( m_pEngine );

        Reference< XUnoTunnel > xAggTunnel;
        if ( query_aggregation( m_xAggregate, xAggTunnel ) )
            return xAggTunnel->getSomething( _rId );

        return 0;
    }

    //--------------------------------------------------------------------
    void SAL_CALL ORichTextModel::addModifyListener( const Reference< XModifyListener >& _rxListener ) throw (RuntimeException)
    {
        m_aModifyListeners.addListener( _rxListener );
    }

    //--------------------------------------------------------------------
    void SAL_CALL ORichTextModel::removeModifyListener( const Reference< XModifyListener >& _rxListener ) throw (RuntimeException)
    {
        m_aModifyListeners.removeListener( _rxListener );
    }

    //--------------------------------------------------------------------
    void ORichTextModel::potentialTextChange( )
    {
        ::rtl::OUString sCurrentEngineText;
        if ( m_pEngine )
            sCurrentEngineText = m_pEngine->GetText();

        if ( sCurrentEngineText != m_sLastKnownEngineText )
        {
            sal_Int32 nHandle = PROPERTY_ID_TEXT;
            Any aOldValue; aOldValue <<= m_sLastKnownEngineText;
            Any aNewValue; aNewValue <<= sCurrentEngineText;
            fire( &nHandle, &aNewValue, &aOldValue, 1, sal_False );

            m_sLastKnownEngineText = sCurrentEngineText;
        }
    }

//........................................................................
} // namespace frm
//........................................................................
