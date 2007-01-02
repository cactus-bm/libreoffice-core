/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: formcomponenthandler.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: hr $ $Date: 2007-01-02 15:53:53 $
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
#include "precompiled_extensions.hxx"

#ifndef EXTENSIONS_SOURCE_PROPCTRLR_FORMCOMPONENTHANDLER_HXX
#include "formcomponenthandler.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_FORMMETADATA_HXX_
#include "formmetadata.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_USERCONTROL_HXX_
#include "usercontrol.hxx"
#endif
#ifndef EXTENSIONS_INC_EXTENSIO_HRC
#include "extensio.hrc"
#endif
#ifndef _EXTENSIONS_FORMSCTRLR_FORMSTRINGS_HXX_
#include "formstrings.hxx"
#endif
#ifndef _EXTENSIONS_FORMCTRLR_PROPRESID_HRC_
#include "formresid.hrc"
#endif
#ifndef EXTENSIONS_SOURCE_PROPCTRLR_CONTROLTYPE_HXX
#include "controltype.hxx"
#endif
#ifndef EXTENSIONS_SOURCE_PROPCTRLR_LISTSELECTIONDLG_HXX
#include "listselectiondlg.hxx"
#endif
#ifndef EXTENSIONS_SOURCE_PROPCTRLR_FORMLINKDIALOG_HXX
#include "formlinkdialog.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_FONTDIALOG_HXX_
#include "fontdialog.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_SELECTLABELDIALOG_HXX_
#include "selectlabeldialog.hxx"
#endif
#ifndef EXTENSIONS_SOURCE_PROPCTRLR_TABORDER_HXX
#include "taborder.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_PCRCOMMON_HXX_
#include "pcrcommon.hxx"
#endif
#ifndef EXTENSIONS_SOURCE_PROPCTRLR_HANDLERHELPER_HXX
#include "handlerhelper.hxx"
#endif

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_LANG_NULLPOINTEREXCEPTION_HPP_
#include <com/sun/star/lang/NullPointerException.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XCONTROLMODEL_HPP_
#include <com/sun/star/awt/XControlModel.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_FORMCOMPONENTTYPE_HPP_
#include <com/sun/star/form/FormComponentType.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XNUMBERFORMATSSUPPLIER_HPP_
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XFORM_HPP_
#include <com/sun/star/form/XForm.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCHILD_HPP_
#include <com/sun/star/container/XChild.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCONNECTION_HPP_
#include <com/sun/star/sdbc/XConnection.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_COMMANDTYPE_HPP_
#include <com/sun/star/sdb/CommandType.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XGRIDCOLUMNFACTORY_HPP_
#include <com/sun/star/form/XGridColumnFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_SQLCONTEXT_HPP_
#include <com/sun/star/sdb/SQLContext.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XTABLESSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XTablesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XQUERIESSUPPLIER_HPP_
#include <com/sun/star/sdb/XQueriesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_LISTSOURCETYPE_HPP_
#include <com/sun/star/form/ListSourceType.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XEXECUTABLEDIALOG_HPP_
#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XSINGLESELECTQUERYCOMPOSER_HPP_
#include <com/sun/star/sdb/XSingleSelectQueryComposer.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILEPICKERCONTROLACCESS_HPP_
#include <com/sun/star/ui/dialogs/XFilePickerControlAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_EXTENDEDFILEPICKERELEMENTIDS_HPP_
#include <com/sun/star/ui/dialogs/ExtendedFilePickerElementIds.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XTABCONTROLLERMODEL_HPP_
#include <com/sun/star/awt/XTabControllerModel.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_FORMSUBMITENCODING_HPP_
#include <com/sun/star/form/FormSubmitEncoding.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_VISUALEFFECT_HPP_
#include <com/sun/star/awt/VisualEffect.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_FORMBUTTONTYPE_HPP_
#include <com/sun/star/form/FormButtonType.hpp>
#endif
#ifndef _COM_SUN_STAR_INSPECTION_PROPERTYCONTROLTYPE_HPP_
#include <com/sun/star/inspection/PropertyControlType.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_MEASUREUNIT_HPP_
#include <com/sun/star/util/MeasureUnit.hpp>
#endif
#ifndef _COM_SUN_STAR_INSPECTION_XOBJECTINSPECTORUI_HPP_
#include <com/sun/star/inspection/XObjectInspectorUI.hpp>
#endif
#ifndef _COM_SUN_STAR_INSPECTION_PROPERTYLINEELEMENT_HPP_
#include <com/sun/star/inspection/PropertyLineElement.hpp>
#endif
#ifndef _COM_SUN_STAR_RESOURCE_XSTRINGRESOURCEMANAGER_HPP_
#include <com/sun/star/resource/XStringResourceManager.hpp>
#endif
/** === end UNO includes === **/

#ifndef _DBHELPER_DBEXCEPTION_HXX_
#include <connectivity/dbexception.hxx>
#endif
#ifndef _SV_WRKWIN_HXX
#include <vcl/wrkwin.hxx>
#endif
#ifndef _NUMUNO_HXX
#include <svtools/numuno.hxx>
#endif
#ifndef _UNOTOOLS_CONFIGNODE_HXX_
#include <unotools/confignode.hxx>
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef SVTOOLS_FILENOTATION_HXX_
#include <svtools/filenotation.hxx>
#endif
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/helper/vclunohelper.hxx>
#endif
#ifndef _VCL_STDTEXT_HXX
#include <vcl/stdtext.hxx>
#endif
#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif
#ifndef _NUMUNO_HXX
#include <svtools/numuno.hxx>
#endif
#ifndef _SFXAPP_HXX
#include <sfx2/app.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <svx/svxids.hrc>
#endif
#ifndef _SFXINTITEM_HXX
#include <svtools/intitem.hxx>
#endif
#define ITEMID_NUMBERINFO   SID_ATTR_NUMBERFORMAT_INFO
#ifndef _SVX_NUMINF_HXX
#include <svx/numinf.hxx>
#endif
#ifndef _SVX_DIALOG_HXX
#include <svx/svxdlg.hxx>
#endif
#ifndef _SVX_DIALOGS_HRC
#include <svx/dialogs.hrc>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _FILEDLGHELPER_HXX
#include <sfx2/filedlghelper.hxx>
#endif
#ifndef _BASEDLGS_HXX
#include <sfx2/basedlgs.hxx>
#endif
#ifndef _SFX_DOCFILT_HACK_HXX
#include <sfx2/docfilt.hxx>
#endif
#ifndef _SV_COLRDLG_HXX
#include <svtools/colrdlg.hxx>
#endif
#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_MODULEOPTIONS_HXX
#include <svtools/moduleoptions.hxx>
#endif
#ifndef _DBHELPER_DBCONVERSION_HXX_
#include <connectivity/dbconversion.hxx>
#endif

#include <limits>

//------------------------------------------------------------------------
extern "C" void SAL_CALL createRegistryInfo_FormComponentPropertyHandler()
{
    ::pcr::FormComponentPropertyHandler::registerImplementation();
}

//........................................................................
namespace pcr
{
//........................................................................

    using namespace ::com::sun::star;
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::frame;
    using namespace ::com::sun::star::script;
    using namespace ::com::sun::star::form;
    using namespace ::com::sun::star::util;
    using namespace ::com::sun::star::awt;
    using namespace ::com::sun::star::sdb;
    using namespace ::com::sun::star::sdbc;
    using namespace ::com::sun::star::sdbcx;
    using namespace ::com::sun::star::form;
    using namespace ::com::sun::star::container;
    using namespace ::com::sun::star::ui::dialogs;
    using namespace ::com::sun::star::inspection;
    using namespace ::dbtools;

    //====================================================================
    //= FormComponentPropertyHandler
    //====================================================================
    DBG_NAME( FormComponentPropertyHandler )
    //--------------------------------------------------------------------
    FormComponentPropertyHandler::FormComponentPropertyHandler( const Reference< XComponentContext >& _rxContext )
        :FormComponentPropertyHandler_Base( _rxContext )
        ,m_sDefaultValueString( String( PcrRes( RID_STR_STANDARD ) ) )
        ,m_eComponentClass( eUnknown )
        ,m_bComponentIsSubForm( false )
        ,m_bHaveListSource( false )
        ,m_bHaveCommand( false )
        ,m_nClassId( 0 )
    {
        DBG_CTOR( FormComponentPropertyHandler, NULL );
    }

    //--------------------------------------------------------------------
    FormComponentPropertyHandler::~FormComponentPropertyHandler()
    {
        DBG_DTOR( FormComponentPropertyHandler, NULL );
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL FormComponentPropertyHandler::getImplementationName_static(  ) throw (RuntimeException)
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.extensions.FormComponentPropertyHandler" ) );
    }

    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL FormComponentPropertyHandler::getSupportedServiceNames_static(  ) throw (RuntimeException)
    {
        Sequence< ::rtl::OUString > aSupported( 1 );
        aSupported[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.form.inspection.FormComponentPropertyHandler" ) );
        return aSupported;
    }

    //============================================
    // TODO: -> export from toolkit
    struct LanguageDependentProp
    {
        const char* pPropName;
        sal_Int32   nPropNameLength;
    };

    static LanguageDependentProp aLanguageDependentProp[] =
    {
        { "Text",            4 },
        { "Label",           5 },
        { "Title",           5 },
        { "HelpText",        8 },
        { "CurrencySymbol", 14 },
        { 0, 0                 }
    };

    namespace
    {
        bool lcl_isLanguageDependentProperty( ::rtl::OUString aName )
        {
            bool bRet = false;

            LanguageDependentProp* pLangDepProp = aLanguageDependentProp;
            while( pLangDepProp->pPropName != 0 )
            {
                if( aName.equalsAsciiL( pLangDepProp->pPropName, pLangDepProp->nPropNameLength ))
                {
                    bRet = true;
                    break;
                }
                pLangDepProp++;
            }
            return bRet;
        }

        Reference< resource::XStringResourceResolver > lcl_getStringResourceResolverForProperty
            ( Reference< XPropertySet > _xComponent, const ::rtl::OUString& _rPropertyName,
              const Any& _rPropertyValue )
        {
            static ::rtl::OUString aResourceResolverPropName
                = ::rtl::OUString::createFromAscii( "ResourceResolver" );

            Reference< resource::XStringResourceResolver > xStringResourceResolver;
            try
            {
                Any aResourceAny( _xComponent->getPropertyValue( aResourceResolverPropName ) );
                aResourceAny >>= xStringResourceResolver;
            }
            catch(UnknownPropertyException&)
            {}

            Reference< resource::XStringResourceResolver > xRet;
            if( xStringResourceResolver.is() &&
                xStringResourceResolver->getLocales().getLength() > 0 &&
                _rPropertyValue.getValueType().getTypeClass() == TypeClass_STRING &&
                lcl_isLanguageDependentProperty( _rPropertyName ) )
            {
                xRet = xStringResourceResolver;
            }

            return xRet;
        }
    }

    //--------------------------------------------------------------------
    Any FormComponentPropertyHandler::impl_getPropertyValue_throw( const ::rtl::OUString& _rPropertyName ) const
    {
        PropertyId nPropId( impl_getPropertyId_throw( _rPropertyName ) );

        Any aPropertyValue( m_xComponent->getPropertyValue( _rPropertyName ) );

        Reference< resource::XStringResourceResolver > xStringResourceResolver
            = lcl_getStringResourceResolverForProperty( m_xComponent, _rPropertyName, aPropertyValue );
        if( xStringResourceResolver.is() )
        {
            ::rtl::OUString aPropStr;
            aPropertyValue >>= aPropStr;
            if( aPropStr.getLength() > 1 )
            {
                ::rtl::OUString aPureIdStr = aPropStr.copy( 1 );
                if( xStringResourceResolver->hasEntryForId( aPureIdStr ) )
                {
                    ::rtl::OUString aResourceStr = xStringResourceResolver->resolveString( aPureIdStr );
                    aPropertyValue <<= aResourceStr;
                }
            }
        }
        else
            impl_normalizePropertyValue_nothrow( aPropertyValue, nPropId );

        return aPropertyValue;
    }

    //--------------------------------------------------------------------
    Any SAL_CALL FormComponentPropertyHandler::getPropertyValue( const ::rtl::OUString& _rPropertyName ) throw (UnknownPropertyException, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        return impl_getPropertyValue_throw( _rPropertyName );
    }

    //--------------------------------------------------------------------
    void SAL_CALL FormComponentPropertyHandler::setPropertyValue( const ::rtl::OUString& _rPropertyName, const Any& _rValue ) throw (UnknownPropertyException, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        PropertyId nPropId( impl_getPropertyId_throw( _rPropertyName ) );

        if ( PROPERTY_ID_FONT_NAME == nPropId )
        {
            // special handling, the value is a faked value we generated ourself in impl_executeFontDialog_nothrow
            Sequence< NamedValue > aFontPropertyValues;
            OSL_VERIFY( _rValue >>= aFontPropertyValues );
            const NamedValue* fontPropertyValue = aFontPropertyValues.getConstArray();
            const NamedValue* fontPropertyValueEnd = fontPropertyValue + aFontPropertyValues.getLength();
            for ( ; fontPropertyValue != fontPropertyValueEnd; ++fontPropertyValue )
                m_xComponent->setPropertyValue( fontPropertyValue->Name, fontPropertyValue->Value );
        }
        else
        {
            Any aValue = _rValue;

            Reference< resource::XStringResourceResolver > xStringResourceResolver
                = lcl_getStringResourceResolverForProperty( m_xComponent, _rPropertyName, _rValue );
            if( xStringResourceResolver.is() )
            {
                Reference< resource::XStringResourceManager >
                    xStringResourceManager( xStringResourceResolver, UNO_QUERY );
                if( xStringResourceManager.is() )
                {
                    Any aPropertyValue( m_xComponent->getPropertyValue( _rPropertyName ) );
                    ::rtl::OUString aPropStr;
                    aPropertyValue >>= aPropStr;
                    if( aPropStr.getLength() > 1 )
                    {
                        ::rtl::OUString aPureIdStr = aPropStr.copy( 1 );
                        ::rtl::OUString aValueStr;
                        _rValue >>= aValueStr;
                        xStringResourceManager->setString( aPureIdStr, aValueStr );
                        aValue = aPropertyValue;    // set value to force modified
                    }
                }
            }

            m_xComponent->setPropertyValue( _rPropertyName, aValue );
        }
    }

    //--------------------------------------------------------------------
    Any SAL_CALL FormComponentPropertyHandler::convertToPropertyValue( const ::rtl::OUString& _rPropertyName, const Any& _rControlValue ) throw (UnknownPropertyException, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        PropertyId nPropId( impl_getPropertyId_throw( _rPropertyName ) );
        Property aProperty( impl_getPropertyFromId_throw( nPropId ) );

        Any aPropertyValue( _rControlValue );
        if ( !aPropertyValue.hasValue() )
        {
            if ( ( aProperty.Attributes & PropertyAttribute::MAYBEVOID ) == 0 )
                // default construct an instance of the proper type
                aPropertyValue = Any( NULL, aProperty.Type );
            // nothing to do
            return aPropertyValue;
        }

        /// care for the special "default" string, translate it to VOID
        if ( m_aPropertiesWithDefListEntry.find( _rPropertyName ) != m_aPropertiesWithDefListEntry.end() )
        {
            // it's a control with a string list
            ::rtl::OUString sStringValue;
            if ( _rControlValue >>= sStringValue )
            {   // note that ColorListBoxes might transfer values either as string or as css.util.Color,
                // so this check here is important
                if ( sStringValue == m_sDefaultValueString )
                    return Any();
            }
        }

        switch ( nPropId )
        {
        case PROPERTY_ID_DATASOURCE:
        {
            ::rtl::OUString sControlValue;
            OSL_VERIFY( _rControlValue >>= sControlValue );

            if ( sControlValue.getLength() )
            {
                Reference< XNameAccess > xDatabaseContext;
                m_aContext.createComponent( (::rtl::OUString)SERVICE_DATABASE_CONTEXT, xDatabaseContext );
                if ( !xDatabaseContext.is() || !xDatabaseContext->hasByName( sControlValue ) )
                {
                    ::svt::OFileNotation aTransformer(sControlValue);
                    aPropertyValue <<= ::rtl::OUString( aTransformer.get( ::svt::OFileNotation::N_URL ) );
                }
            }
        }
        break;  // case PROPERTY_ID_DATASOURCE

        case PROPERTY_ID_SHOW_POSITION:
        case PROPERTY_ID_SHOW_NAVIGATION:
        case PROPERTY_ID_SHOW_RECORDACTIONS:
        case PROPERTY_ID_SHOW_FILTERSORT:
        {
            ::rtl::OUString sControlValue;
            OSL_VERIFY( _rControlValue >>= sControlValue );

            String sShowHide = String( PcrRes( RID_STR_SHOW_HIDE ) );
            aPropertyValue <<= (sal_Bool)( sShowHide.GetToken(1) == String( sControlValue ) );
        }
        break;

        case PROPERTY_ID_TARGET_URL:
        case PROPERTY_ID_IMAGE_URL:
        {
            ::rtl::OUString sControlValue;
            OSL_VERIFY( _rControlValue >>= sControlValue );

            INetURLObject aDocURL( impl_getDocumentURL_nothrow() );
            aPropertyValue <<= (::rtl::OUString)URIHelper::SmartRel2Abs( aDocURL, sControlValue, Link(), false, true, INetURLObject::WAS_ENCODED, INetURLObject::DECODE_TO_IURI );
        }
        break;

        case PROPERTY_ID_DATEMIN:
        case PROPERTY_ID_DATEMAX:
        case PROPERTY_ID_DEFAULT_DATE:
        case PROPERTY_ID_DATE:
        {
            util::Date aDate;
            OSL_VERIFY( _rControlValue >>= aDate );
            aPropertyValue <<= (sal_Int32)DBTypeConversion::toINT32( aDate );
        }
        break;

        case PROPERTY_ID_TIMEMIN:
        case PROPERTY_ID_TIMEMAX:
        case PROPERTY_ID_DEFAULT_TIME:
        case PROPERTY_ID_TIME:
        {
            util::Time aTime;
            OSL_VERIFY( _rControlValue >>= aTime );
            aPropertyValue <<= (sal_Int32)DBTypeConversion::toINT32( aTime );
        }
        break;

        default:
            aPropertyValue = FormComponentPropertyHandler_Base::convertToPropertyValue( _rPropertyName, _rControlValue );
            break;  // default

        }   // switch ( nPropId )

        return aPropertyValue;
    }

    //--------------------------------------------------------------------
    Any SAL_CALL FormComponentPropertyHandler::convertToControlValue( const ::rtl::OUString& _rPropertyName, const Any& _rPropertyValue, const Type& _rControlValueType ) throw (UnknownPropertyException, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        sal_Int32 nPropId = m_pInfoService->getPropertyId( _rPropertyName );
        DBG_ASSERT( nPropId != -1, "FormComponentPropertyHandler::convertToPropertyValue: not one of my properties!!" );

        Property aProperty( impl_getPropertyFromId_throw( nPropId ) );

        Any aControlValue( _rPropertyValue );
        if ( !aControlValue.hasValue() )
        {
            // if the property is represented with a list box or color list box, we need to
            // translate this into the string "Default"
            if ( m_aPropertiesWithDefListEntry.find( _rPropertyName ) != m_aPropertiesWithDefListEntry.end() )
                aControlValue <<= m_sDefaultValueString;

            return aControlValue;
        }

        switch ( nPropId )
        {
        //////////////////////////////////////////////////////////////
        case PROPERTY_ID_SHOW_POSITION:
        case PROPERTY_ID_SHOW_NAVIGATION:
        case PROPERTY_ID_SHOW_RECORDACTIONS:
        case PROPERTY_ID_SHOW_FILTERSORT:
        {
            String aEntries = PcrRes( RID_STR_SHOW_HIDE );
            ::rtl::OUString sControlValue(
                ::comphelper::getBOOL( _rPropertyValue ) ? aEntries.GetToken( 1 ) : aEntries.GetToken( 0 ) );
            aControlValue <<= sControlValue;
        }
        break;

        //////////////////////////////////////////////////////////////
        case PROPERTY_ID_DATASOURCE:
        {
            OSL_ENSURE( _rControlValueType.getTypeClass() == TypeClass_STRING,
                "FormComponentPropertyHandler::convertToControlValue: wrong ControlValueType!" );

            ::rtl::OUString sDataSource;
            _rPropertyValue >>= sDataSource;
            if ( sDataSource.getLength() )
            {
                ::svt::OFileNotation aTransformer( sDataSource );
                sDataSource = aTransformer.get( ::svt::OFileNotation::N_SYSTEM );
            }
            aControlValue <<= sDataSource;
        }
        break;

        //////////////////////////////////////////////////////////////
        case PROPERTY_ID_CONTROLLABEL:
        {
            ::rtl::OUString sControlValue;

            Reference< XPropertySet >  xSet;
            _rPropertyValue >>= xSet;
            Reference< XPropertySetInfo > xPSI;
            if ( xSet.is() )
                xPSI = xSet->getPropertySetInfo();
            if ( xPSI.is() && xPSI->hasPropertyByName( PROPERTY_LABEL ) )
            {
                ::rtl::OUStringBuffer aValue;
                aValue.append( (sal_Unicode)'<' );
                ::rtl::OUString sLabel;
                OSL_VERIFY( xSet->getPropertyValue( PROPERTY_LABEL ) >>= sLabel );
                aValue.append( sLabel );
                aValue.append( (sal_Unicode)'>' );
                sControlValue = aValue.makeStringAndClear();
            }

            aControlValue <<= sControlValue;
        }
        break;

        //////////////////////////////////////////////////////////////
        case PROPERTY_ID_DATEMIN:
        case PROPERTY_ID_DATEMAX:
        case PROPERTY_ID_DEFAULT_DATE:
        case PROPERTY_ID_DATE:
        {
            sal_Int32 nDate = 0;
            OSL_VERIFY( _rPropertyValue >>= nDate );
            aControlValue <<= DBTypeConversion::toDate( nDate );
        }
        break;

        case PROPERTY_ID_TIMEMIN:
        case PROPERTY_ID_TIMEMAX:
        case PROPERTY_ID_DEFAULT_TIME:
        case PROPERTY_ID_TIME:
        {
            sal_Int32 nTime = 0;
            OSL_VERIFY( _rPropertyValue >>= nTime );
            aControlValue <<= DBTypeConversion::toTime( nTime );
        }
        break;

        default:
            aControlValue = FormComponentPropertyHandler_Base::convertToControlValue( _rPropertyName, _rPropertyValue, _rControlValueType );
            break;

        }   // switch ( nPropId )

        return aControlValue;
    }

    //--------------------------------------------------------------------
    PropertyState SAL_CALL FormComponentPropertyHandler::getPropertyState( const ::rtl::OUString& _rPropertyName ) throw (UnknownPropertyException, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( m_xPropertyState.is() )
            return m_xPropertyState->getPropertyState( _rPropertyName );
        return PropertyState_DIRECT_VALUE;
    }

    //--------------------------------------------------------------------
    void SAL_CALL FormComponentPropertyHandler::addPropertyChangeListener( const Reference< XPropertyChangeListener >& _rxListener ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        FormComponentPropertyHandler_Base::addPropertyChangeListener( _rxListener );
        if ( m_xComponent.is() )
            m_xComponent->addPropertyChangeListener( ::rtl::OUString(), _rxListener );
    }

    //--------------------------------------------------------------------
    void SAL_CALL FormComponentPropertyHandler::removePropertyChangeListener( const Reference< XPropertyChangeListener >& _rxListener ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( m_xComponent.is() )
            m_xComponent->removePropertyChangeListener( ::rtl::OUString(), _rxListener );
        FormComponentPropertyHandler_Base::removePropertyChangeListener( _rxListener );
    }

    //--------------------------------------------------------------------
    void FormComponentPropertyHandler::onNewComponent()
    {
        FormComponentPropertyHandler_Base::onNewComponent();
        if ( !m_xComponentPropertyInfo.is() && m_xComponent.is() )
            throw NullPointerException();

        m_xPropertyState = Reference< XPropertyState >( m_xComponent, UNO_QUERY_THROW );
        m_eComponentClass = eUnknown;
        m_bComponentIsSubForm = m_bHaveListSource = m_bHaveCommand = false;
        m_nClassId = 0;

        impl_initComponentMetaData_throw();
    }

    //--------------------------------------------------------------------
    Sequence< Property > SAL_CALL FormComponentPropertyHandler::doDescribeSupportedProperties() const
    {
        if ( !m_xComponentPropertyInfo.is() )
            return Sequence< Property >();

        ::std::vector< Property > aProperties;

        Sequence< Property > aAllProperties( m_xComponentPropertyInfo->getProperties() );
        aProperties.reserve( aAllProperties.getLength() );

        // filter the properties
        PropertyId nPropId( 0 );
        ::rtl::OUString sDisplayName;

        Property* pProperty = aAllProperties.getArray();
        Property* pPropertiesEnd = pProperty + aAllProperties.getLength();
        for ( ; pProperty != pPropertiesEnd; ++pProperty )
        {
            nPropId = m_pInfoService->getPropertyId( pProperty->Name );
            if ( nPropId == -1 )
                continue;
            pProperty->Handle = nPropId;

            sDisplayName = m_pInfoService->getPropertyTranslation( nPropId );
            if ( !sDisplayName.getLength() )
                continue;

            sal_uInt32  nPropertyUIFlags = m_pInfoService->getPropertyUIFlags( nPropId );
            bool bIsVisibleForForms   = ( nPropertyUIFlags & PROP_FLAG_FORM_VISIBLE   ) != 0;
            bool bIsVisibleForDialogs = ( nPropertyUIFlags & PROP_FLAG_DIALOG_VISIBLE ) != 0;

            // depending on whether we're working for a form or a UNO dialog, some
            // properties are not displayed
            if  (  ( m_eComponentClass == eFormControl   && !bIsVisibleForForms )
                || ( m_eComponentClass == eDialogControl && !bIsVisibleForDialogs )
                )
                continue;

            // some generic sanity checks
            if ( impl_shouldExcludeProperty_nothrow( *pProperty ) )
                continue;

            switch ( nPropId )
            {
            case PROPERTY_ID_BORDER:
            case PROPERTY_ID_TABSTOP:
                // BORDER and TABSTOP are normalized (see impl_normalizePropertyValue_nothrow)
                // to not allow VOID values
                pProperty->Attributes &= ~( PropertyAttribute::MAYBEVOID );
                break;

            case PROPERTY_ID_LISTSOURCE:
                // no cursor source if no Base is installed. #124939#
                // This fix is not intendend to appear on the main trunk. If you find it there,
                // please tell me! frank.schoenheit@sun.com
                if ( SvtModuleOptions().IsModuleInstalled( SvtModuleOptions::E_SDATABASE ) )
                    const_cast< FormComponentPropertyHandler* >( this )->m_bHaveListSource = true;
                break;

            case PROPERTY_ID_COMMAND:
                // no cursor source if no Base is installed. #124939#
                // This fix is not intendend to appear on the main trunk. If you find it there,
                // please tell me! frank.schoenheit@sun.com
                if ( SvtModuleOptions().IsModuleInstalled( SvtModuleOptions::E_SDATABASE ) )
                    const_cast< FormComponentPropertyHandler* >( this )->m_bHaveCommand = true;
                break;

            }   // switch ( nPropId )

            aProperties.push_back( *pProperty );
        }

        if ( aProperties.empty() )
            return Sequence< Property >();
        return Sequence< Property >( &(*aProperties.begin()), aProperties.size() );
    }

    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL FormComponentPropertyHandler::getSupersededProperties( ) throw (RuntimeException)
    {
        return Sequence< ::rtl::OUString >( );
    }

    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL FormComponentPropertyHandler::getActuatingProperties( ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        ::std::vector< ::rtl::OUString > aInterestingProperties;
        aInterestingProperties.push_back( PROPERTY_DATASOURCE );
        aInterestingProperties.push_back( PROPERTY_COMMAND );
        aInterestingProperties.push_back( PROPERTY_COMMANDTYPE );
        aInterestingProperties.push_back( PROPERTY_LISTSOURCE );
        aInterestingProperties.push_back( PROPERTY_LISTSOURCETYPE );
        aInterestingProperties.push_back( PROPERTY_SUBMIT_ENCODING );
        aInterestingProperties.push_back( PROPERTY_REPEAT );
        aInterestingProperties.push_back( PROPERTY_TABSTOP );
        aInterestingProperties.push_back( PROPERTY_BORDER );
        aInterestingProperties.push_back( PROPERTY_CONTROLSOURCE );
        aInterestingProperties.push_back( PROPERTY_DROPDOWN );
        aInterestingProperties.push_back( PROPERTY_IMAGE_URL );
        aInterestingProperties.push_back( PROPERTY_TARGET_URL );
        aInterestingProperties.push_back( PROPERTY_STRINGITEMLIST );
        aInterestingProperties.push_back( PROPERTY_BUTTONTYPE );
        aInterestingProperties.push_back( PROPERTY_ESCAPE_PROCESSING );
        aInterestingProperties.push_back( PROPERTY_TRISTATE );
        aInterestingProperties.push_back( PROPERTY_DECIMAL_ACCURACY );
        aInterestingProperties.push_back( PROPERTY_SHOWTHOUSANDSEP );
        aInterestingProperties.push_back( PROPERTY_FORMATKEY );
        return Sequence< ::rtl::OUString >( &(*aInterestingProperties.begin()), aInterestingProperties.size() );
    }

    //--------------------------------------------------------------------
    LineDescriptor SAL_CALL FormComponentPropertyHandler::describePropertyLine( const ::rtl::OUString& _rPropertyName,
        const Reference< XPropertyControlFactory >& _rxControlFactory )
        throw (UnknownPropertyException, NullPointerException, RuntimeException)
    {
        if ( !_rxControlFactory.is() )
            throw NullPointerException();

        ::osl::MutexGuard aGuard( m_aMutex );
        PropertyId nPropId( impl_getPropertyId_throw( _rPropertyName ) );
        Property aProperty( impl_getPropertyFromId_throw( nPropId ) );

        //////////////////////////////////////////////////////////////////////
        // for the MultiLine property, we have different UI translations depending on the control
        // type
        if ( nPropId == PROPERTY_ID_MULTILINE )
        {
            if (  ( m_nClassId == FormComponentType::FIXEDTEXT )
               || ( m_nClassId == FormComponentType::COMMANDBUTTON )
               || ( m_nClassId == FormComponentType::RADIOBUTTON )
               || ( m_nClassId == FormComponentType::CHECKBOX )
               )
                nPropId = PROPERTY_ID_WORDBREAK;
        }

        String sDisplayName = m_pInfoService->getPropertyTranslation( nPropId );
        if ( !sDisplayName.Len() )
        {
            DBG_ERROR( "FormComponentPropertyHandler::describePropertyLine: did getSupportedProperties not work properly?" );
            throw UnknownPropertyException();
        }

        //////////////////////////////////////////////////////////////////////

        LineDescriptor aDescriptor;
        aDescriptor.HelpURL = HelpIdUrl::getHelpURL( m_pInfoService->getPropertyHelpId( nPropId ) );
        aDescriptor.DisplayName = sDisplayName;

        // for the moment, assume a text field
        sal_Int16 nControlType = PropertyControlType::TextField;
        sal_Bool bReadOnly = sal_False;
        aDescriptor.Control.clear();

        //////////////////////////////////////////////////////////////////////

        bool bNeedDefaultStringIfVoidAllowed = false;

        TypeClass eType = aProperty.Type.getTypeClass();

        switch ( nPropId )
        {
        case PROPERTY_ID_DEFAULT_SELECT_SEQ:
        case PROPERTY_ID_SELECTEDITEMS:
            aDescriptor.PrimaryButtonId = UID_PROP_DLG_SELECTION;
            break;

        case PROPERTY_ID_FILTER:
            aDescriptor.PrimaryButtonId = UID_PROP_DLG_FILTER;
            break;

        case PROPERTY_ID_SORT:
            aDescriptor.PrimaryButtonId = UID_PROP_DLG_ORDER;
            break;

        case PROPERTY_ID_MASTERFIELDS:
        case PROPERTY_ID_DETAILFIELDS:
            nControlType = PropertyControlType::StringListField;
            aDescriptor.PrimaryButtonId = UID_PROP_DLG_FORMLINKFIELDS;
            break;

        case PROPERTY_ID_COMMAND:
            aDescriptor.PrimaryButtonId = UID_PROP_DLG_SQLCOMMAND;
            break;

        case PROPERTY_ID_TABINDEX:
        {
            Reference< XControlContainer > xControlContext( impl_getContextControlContainer_nothrow() );
            if ( xControlContext.is() )
                aDescriptor.PrimaryButtonId = UID_PROP_DLG_TABINDEX;
            nControlType = PropertyControlType::NumericField;
        };
        break;

        case PROPERTY_ID_FONT_NAME:
            bReadOnly = sal_True;
            aDescriptor.PrimaryButtonId = UID_PROP_DLG_FONT_TYPE;
            break;

        case PROPERTY_ID_TARGET_URL:
        case PROPERTY_ID_IMAGE_URL:
        {
            aDescriptor.Control = new OFileUrlControl( impl_getDefaultDialogParent_nothrow(), WB_TABSTOP | WB_BORDER );

            aDescriptor.PrimaryButtonId = ( PROPERTY_ID_TARGET_URL == nPropId )
                ? UID_PROP_DLG_ATTR_TARGET_URL : UID_PROP_DLG_IMAGE_URL;
        }
        break;

        case PROPERTY_ID_ECHO_CHAR:
            nControlType = PropertyControlType::CharacterField;
            break;

        case PROPERTY_ID_BACKGROUNDCOLOR:
        case PROPERTY_ID_FILLCOLOR:
        case PROPERTY_ID_SYMBOLCOLOR:
        case PROPERTY_ID_BORDERCOLOR:
            nControlType = PropertyControlType::ColorListBox;

            switch( nPropId )
            {
            case PROPERTY_ID_BACKGROUNDCOLOR:
                aDescriptor.PrimaryButtonId = UID_PROP_DLG_BACKGROUNDCOLOR; break;
            case PROPERTY_ID_FILLCOLOR:
                aDescriptor.PrimaryButtonId = UID_PROP_DLG_FILLCOLOR; break;
            case PROPERTY_ID_SYMBOLCOLOR:
                aDescriptor.PrimaryButtonId = UID_PROP_DLG_SYMBOLCOLOR; break;
            case PROPERTY_ID_BORDERCOLOR:
                aDescriptor.PrimaryButtonId = UID_PROP_DLG_BORDERCOLOR; break;
            }
            break;

        case PROPERTY_ID_LABEL:
            nControlType = PropertyControlType::MultiLineTextField;
            break;

        case PROPERTY_ID_DEFAULT_TEXT:
        {
            if (FormComponentType::FILECONTROL == m_nClassId)
                nControlType = PropertyControlType::TextField;
            else
                nControlType = PropertyControlType::MultiLineTextField;
        }
        break;

        case PROPERTY_ID_TEXT:
            if ( impl_componentHasProperty_throw( PROPERTY_MULTILINE ) )
                nControlType = PropertyControlType::MultiLineTextField;
            break;

        case PROPERTY_ID_CONTROLLABEL:
            bReadOnly = sal_True;
            aDescriptor.PrimaryButtonId = UID_PROP_DLG_CONTROLLABEL;
            break;

        case PROPERTY_ID_FORMATKEY:
        case PROPERTY_ID_EFFECTIVE_MIN:
        case PROPERTY_ID_EFFECTIVE_MAX:
        case PROPERTY_ID_EFFECTIVE_DEFAULT:
        case PROPERTY_ID_EFFECTIVE_VALUE:
        {
            // and the supplier is really available
            Reference< XNumberFormatsSupplier >  xSupplier;
            m_xComponent->getPropertyValue( PROPERTY_FORMATSSUPPLIER ) >>= xSupplier;
            if (xSupplier.is())
            {
                Reference< XUnoTunnel > xTunnel(xSupplier,UNO_QUERY);
                DBG_ASSERT(xTunnel.is(), "FormComponentPropertyHandler::describePropertyLine : xTunnel is invalid!");
                SvNumberFormatsSupplierObj* pSupplier = (SvNumberFormatsSupplierObj*)xTunnel->getSomething(SvNumberFormatsSupplierObj::getUnoTunnelId());

                if (pSupplier != NULL)
                {
                    sal_Bool bIsFormatKey = (PROPERTY_ID_FORMATKEY == nPropId);

                    bReadOnly = bIsFormatKey;

                    if ( bIsFormatKey )
                    {
                        OFormatSampleControl* pControl = new OFormatSampleControl( impl_getDefaultDialogParent_nothrow(), WB_READONLY | WB_TABSTOP | WB_BORDER );
                        aDescriptor.Control = pControl;
                        pControl->SetFormatSupplier( pSupplier );

                        aDescriptor.PrimaryButtonId = UID_PROP_DLG_NUMBER_FORMAT;
                    }
                    else
                    {
                        OFormattedNumericControl* pControl = new OFormattedNumericControl( impl_getDefaultDialogParent_nothrow(), WB_TABSTOP | WB_BORDER );
                        aDescriptor.Control = pControl;

                        FormatDescription aDesc;
                        aDesc.pSupplier = pSupplier;
                        Any aFormatKeyValue = m_xComponent->getPropertyValue(PROPERTY_FORMATKEY);
                        if ( !( aFormatKeyValue >>= aDesc.nKey ) )
                            aDesc.nKey = 0;

                        pControl->SetFormatDescription( aDesc );
                    }
                }
            }
        }
        break;

        case PROPERTY_ID_DATEMIN:
        case PROPERTY_ID_DATEMAX:
        case PROPERTY_ID_DEFAULT_DATE:
        case PROPERTY_ID_DATE:
            nControlType = PropertyControlType::DateField;
            break;

        case PROPERTY_ID_TIMEMIN:
        case PROPERTY_ID_TIMEMAX:
        case PROPERTY_ID_DEFAULT_TIME:
        case PROPERTY_ID_TIME:
            nControlType = PropertyControlType::TimeField;
            break;

        case PROPERTY_ID_VALUEMIN:
        case PROPERTY_ID_VALUEMAX:
        case PROPERTY_ID_DEFAULT_VALUE:
        case PROPERTY_ID_VALUE:
            {
                OFormattedNumericControl* pControl = new OFormattedNumericControl( impl_getDefaultDialogParent_nothrow(), WB_TABSTOP | WB_BORDER | WB_SPIN | WB_REPEAT );
                aDescriptor.Control = pControl;

                // we don't set a formatter so the control uses a default (which uses the application
                // language and a default numeric format)
                // but we set the decimal digits
                pControl->SetDecimalDigits(
                    ::comphelper::getINT16( m_xComponent->getPropertyValue( PROPERTY_DECIMAL_ACCURACY ) )
                );

                // and the thousands separator
                pControl->SetThousandsSep(
                    ::comphelper::getBOOL( m_xComponent->getPropertyValue(PROPERTY_SHOWTHOUSANDSEP) )
                );

                // and the default value for the property
                try
                {
                    if (m_xPropertyState.is() && ((PROPERTY_ID_VALUEMIN == nPropId) || (PROPERTY_ID_VALUEMAX == nPropId)))
                    {
                        double nDefault = 0;
                        if ( m_xPropertyState->getPropertyDefault( aProperty.Name ) >>= nDefault )
                            pControl->SetDefaultValue( nDefault );
                    }
                }
                catch (Exception&)
                {
                    // just ignore it
                }

                // and allow empty values only for the default value and the value
                pControl->EnableEmptyField( ( PROPERTY_ID_DEFAULT_VALUE == nPropId )
                                        ||  ( PROPERTY_ID_VALUE == nPropId ) );
            }
            break;

        default:
            if ( TypeClass_BYTE <= eType && eType <= TypeClass_DOUBLE )
            {
                sal_Int16 nDigits = 0;
                sal_Int16 nValueUnit = -1;
                sal_Int16 nDisplayUnit = -1;
                if ( m_eComponentClass == eFormControl )
                {
                    if  ( nPropId == PROPERTY_ID_HEIGHT )
                        nDigits = 1;

                    if  (  ( nPropId == PROPERTY_ID_WIDTH )
                        || ( nPropId == PROPERTY_ID_ROWHEIGHT )
                        )
                    {
                        nValueUnit = MeasureUnit::MM_10TH;
                        nDisplayUnit = impl_getDocumentMeasurementUnit_throw();
                        nDigits = 2;
                    }
                }

                Optional< double > aValueNotPresent( sal_False, 0 );
                aDescriptor.Control = PropertyHandlerHelper::createNumericControl(
                    _rxControlFactory, nDigits, aValueNotPresent, aValueNotPresent, sal_False );

                Reference< XNumericControl > xNumericControl( aDescriptor.Control, UNO_QUERY_THROW );
                if ( nValueUnit != -1 )
                    xNumericControl->setValueUnit( nValueUnit );
                if ( nDisplayUnit != -1 )
                    xNumericControl->setDisplayUnit( nDisplayUnit );
            }
            break;
        }

        //////////////////////////////////////////////////////////////////////
        if ( eType == TypeClass_SEQUENCE )
            nControlType = PropertyControlType::StringListField;

        //////////////////////////////////////////////////////////////////////
        // boolean values
        if ( eType == TypeClass_BOOLEAN )
        {
            String aEntries;
            if  (   ( nPropId == PROPERTY_ID_SHOW_POSITION )
                ||  ( nPropId == PROPERTY_ID_SHOW_NAVIGATION )
                ||  ( nPropId == PROPERTY_ID_SHOW_RECORDACTIONS )
                ||  ( nPropId == PROPERTY_ID_SHOW_FILTERSORT )
                )
                aEntries = String( PcrRes( RID_STR_SHOW_HIDE ) );
            else
                aEntries = String( PcrRes( RID_STR_BOOL ) );

            ::std::vector< ::rtl::OUString > aListEntries;
            for ( xub_StrLen i=0; i<2; ++i )
                aListEntries.push_back( aEntries.GetToken(i) );
            aDescriptor.Control = PropertyHandlerHelper::createListBoxControl( _rxControlFactory, aListEntries, sal_False, sal_False );
            bNeedDefaultStringIfVoidAllowed = true;
        }

        //////////////////////////////////////////////////////////////////////
        // enum properties
        sal_uInt32 nPropertyUIFlags = m_pInfoService->getPropertyUIFlags( nPropId );
        bool bIsEnumProperty = ( nPropertyUIFlags & PROP_FLAG_ENUM ) != 0;
        if ( bIsEnumProperty || ( PROPERTY_ID_TARGET_FRAME == nPropId ) )
        {
            ::std::vector< ::rtl::OUString > aEnumValues = m_pInfoService->getPropertyEnumRepresentations( nPropId );
            ::std::vector< ::rtl::OUString >::const_iterator pStart = aEnumValues.begin();
            ::std::vector< ::rtl::OUString >::const_iterator pEnd = aEnumValues.end();

            // for a checkbox: if "ambiguous" is not allowed, remove this from the sequence
            if  (   ( PROPERTY_ID_DEFAULTCHECKED == nPropId )
                ||  ( PROPERTY_ID_STATE == nPropId )
                )
                if ( impl_componentHasProperty_throw( PROPERTY_TRISTATE ) )
                {
                    if ( !::comphelper::getBOOL( m_xComponent->getPropertyValue( PROPERTY_TRISTATE ) ) )
                    {   // remove the last sequence element
                        if ( pEnd > pStart )
                            --pEnd;
                    }
                }
                else
                    --pEnd;

            if ( PROPERTY_ID_LISTSOURCETYPE == nPropId )
                if ( FormComponentType::COMBOBOX == m_nClassId )
                    // remove the first sequence element -> value list not possible for combo boxes
                    ++pStart;

            // copy the sequence
            ::std::vector< ::rtl::OUString > aListEntries( pEnd - pStart );
            ::std::copy( pStart, pEnd, aListEntries.begin() );

            // create the control
            if ( PROPERTY_ID_TARGET_FRAME == nPropId )
                aDescriptor.Control = PropertyHandlerHelper::createComboBoxControl( _rxControlFactory, aListEntries, sal_False, sal_False );
            else
            {
                aDescriptor.Control = PropertyHandlerHelper::createListBoxControl( _rxControlFactory, aListEntries, sal_False, sal_False );
                bNeedDefaultStringIfVoidAllowed = true;
            }
        }

        //////////////////////////////////////////////////////////////////////
        switch( nPropId )
        {
            case PROPERTY_ID_REPEAT_DELAY:
            {
                OTimeDurationControl* pControl = new OTimeDurationControl( impl_getDefaultDialogParent_nothrow(), WB_BORDER | WB_TABSTOP );
                aDescriptor.Control = pControl;

                pControl->setMinValue( Optional< double >( sal_True, 0 ) );
                pControl->setMaxValue( Optional< double >( sal_True, ::std::numeric_limits< double >::max() ) );
            }
            break;

            case PROPERTY_ID_TABINDEX:
            case PROPERTY_ID_BOUNDCOLUMN:
            case PROPERTY_ID_VISIBLESIZE:
            case PROPERTY_ID_MAXTEXTLEN:
            case PROPERTY_ID_LINEINCREMENT:
            case PROPERTY_ID_BLOCKINCREMENT:
            case PROPERTY_ID_SPININCREMENT:
            {
                Optional< double > aMinValue( sal_True, 0 );
                Optional< double > aMaxValue( sal_True, 0x7FFFFFFF );

                if ( nPropId == PROPERTY_ID_MAXTEXTLEN )
                    aMinValue.Value = -1;
                else if ( ( nPropId == PROPERTY_ID_BOUNDCOLUMN ) || ( nPropId == PROPERTY_ID_VISIBLESIZE ) )
                    aMinValue.Value = 1;
                else
                    aMinValue.Value = 0;

                aDescriptor.Control = PropertyHandlerHelper::createNumericControl(
                    _rxControlFactory, 0, aMinValue, aMaxValue, sal_False );
            }
            break;

            case PROPERTY_ID_DECIMAL_ACCURACY:
            {
                Optional< double > aMinValue( sal_True, 0 );
                Optional< double > aMaxValue( sal_True, 20 );

                aDescriptor.Control = PropertyHandlerHelper::createNumericControl(
                    _rxControlFactory, 0, aMinValue, aMaxValue, sal_False );
            }
            break;

            //////////////////////////////////////////////////////////////////////
            // DataSource
            case PROPERTY_ID_DATASOURCE:
            {
                aDescriptor.PrimaryButtonId = UID_PROP_DLG_ATTR_DATASOURCE;

                ::std::vector< ::rtl::OUString > aListEntries;

                Reference< XNameAccess > xDatabaseContext;
                m_aContext.createComponent( (rtl::OUString)SERVICE_DATABASE_CONTEXT, xDatabaseContext );
                if (xDatabaseContext.is())
                {
                    Sequence< ::rtl::OUString > aDatasources = xDatabaseContext->getElementNames();
                    aListEntries.resize( aDatasources.getLength() );
                    ::std::copy( aDatasources.getConstArray(), aDatasources.getConstArray() + aDatasources.getLength(),
                        aListEntries.begin() );
                }
                aDescriptor.Control = PropertyHandlerHelper::createComboBoxControl(
                    _rxControlFactory, aListEntries, sal_False, sal_True );
            }
            break;

            case PROPERTY_ID_CONTROLSOURCE:
            {
                ::std::vector< ::rtl::OUString > aFieldNames;
                impl_initFieldList_nothrow( aFieldNames );
                aDescriptor.Control = PropertyHandlerHelper::createComboBoxControl(
                    _rxControlFactory, aFieldNames, sal_False, sal_False );
            }
            break;

            case PROPERTY_ID_COMMAND:
                impl_describeCursorSource_nothrow( aDescriptor, _rxControlFactory );
                break;

            case PROPERTY_ID_LISTSOURCE:
                impl_describeListSourceUI_throw( aDescriptor, _rxControlFactory );
                break;
        }

        if ( !aDescriptor.Control.is() )
            aDescriptor.Control = _rxControlFactory->createPropertyControl( nControlType, bReadOnly );

        if ( ( aProperty.Attributes & PropertyAttribute::MAYBEVOID ) != 0 )
        {
            // insert the string "Default" string, if necessary
            if ( bNeedDefaultStringIfVoidAllowed || ( nControlType == PropertyControlType::ColorListBox ) )
            {
                Reference< XStringListControl > xStringList( aDescriptor.Control, UNO_QUERY_THROW );
                xStringList->prependListEntry( m_sDefaultValueString );
                m_aPropertiesWithDefListEntry.insert( _rPropertyName );
            }
        }

        if ( aDescriptor.PrimaryButtonId )
            aDescriptor.HasPrimaryButton = sal_True;
        if ( aDescriptor.SecondaryButtonId )
            aDescriptor.HasSecondaryButton = sal_True;

        bool bIsDataProperty = ( nPropertyUIFlags & PROP_FLAG_DATA_PROPERTY ) != 0;
        aDescriptor.Category = ::rtl::OUString::createFromAscii( bIsDataProperty ? "Data" : "General" );
        return aDescriptor;
    }

    //--------------------------------------------------------------------
    InteractiveSelectionResult SAL_CALL FormComponentPropertyHandler::onInteractivePropertySelection( const ::rtl::OUString& _rPropertyName, sal_Bool /*_bPrimary*/, Any& _rData, const Reference< XObjectInspectorUI >& _rxInspectorUI ) throw (UnknownPropertyException, NullPointerException, RuntimeException)
    {
        if ( !_rxInspectorUI.is() )
            throw NullPointerException();

        ::osl::ClearableMutexGuard aGuard( m_aMutex );
        PropertyId nPropId( impl_getPropertyId_throw( _rPropertyName ) );

        InteractiveSelectionResult eResult = InteractiveSelectionResult_Cancelled;
        switch ( nPropId )
        {
        case PROPERTY_ID_DEFAULT_SELECT_SEQ:
        case PROPERTY_ID_SELECTEDITEMS:
            if ( impl_dialogListSelection_nothrow( _rPropertyName, aGuard ) )
                eResult = InteractiveSelectionResult_Success;
            break;

        case PROPERTY_ID_FILTER:
        case PROPERTY_ID_SORT:
        {
            ::rtl::OUString sClause;
            if ( impl_dialogFilterOrSort_nothrow( PROPERTY_ID_FILTER == nPropId, sClause, aGuard ) )
            {
                _rData <<= sClause;
                eResult = InteractiveSelectionResult_ObtainedValue;
            }
        }
        break;

        case PROPERTY_ID_MASTERFIELDS:
        case PROPERTY_ID_DETAILFIELDS:
            if ( impl_dialogLinkedFormFields_nothrow( aGuard ) )
                eResult = InteractiveSelectionResult_Success;
            break;

        case PROPERTY_ID_FORMATKEY:
            if ( impl_dialogFormatting_nothrow( _rData, aGuard ) )
                eResult = InteractiveSelectionResult_ObtainedValue;
            break;

        case PROPERTY_ID_IMAGE_URL:
            if ( impl_browseForImage_nothrow( _rData, aGuard ) )
                eResult = InteractiveSelectionResult_ObtainedValue;
            break;

        case PROPERTY_ID_TARGET_URL:
            if ( impl_browseForTargetURL_nothrow( _rData, aGuard ) )
                eResult = InteractiveSelectionResult_ObtainedValue;
            break;

        case PROPERTY_ID_FONT_NAME:
            if ( impl_executeFontDialog_nothrow( _rData, aGuard ) )
                eResult = InteractiveSelectionResult_ObtainedValue;
            break;

        case PROPERTY_ID_DATASOURCE:
            if ( impl_browseForDatabaseDocument_throw( _rData, aGuard ) )
                eResult = InteractiveSelectionResult_ObtainedValue;
            break;

        case PROPERTY_ID_BACKGROUNDCOLOR:
        case PROPERTY_ID_FILLCOLOR:
        case PROPERTY_ID_SYMBOLCOLOR:
        case PROPERTY_ID_BORDERCOLOR:
            if ( impl_dialogColorChooser_throw( nPropId, _rData, aGuard ) )
                eResult = InteractiveSelectionResult_ObtainedValue;
            break;

        case PROPERTY_ID_CONTROLLABEL:
            if ( impl_dialogChooseLabelControl_nothrow( _rData, aGuard ) )
                eResult = InteractiveSelectionResult_ObtainedValue;
            break;

        case PROPERTY_ID_TABINDEX:
            if ( impl_dialogChangeTabOrder_nothrow( aGuard ) )
                eResult = InteractiveSelectionResult_Success;
            break;

        case PROPERTY_ID_COMMAND:
            if ( impl_doDesignSQLCommand_nothrow( _rxInspectorUI ) )
                eResult = InteractiveSelectionResult_Pending;
            break;
        default:
            DBG_ERROR( "FormComponentPropertyHandler::onInteractivePropertySelection: request for a property which does not have dedicated UI!" );
            break;
        }
        return eResult;
    }

    //--------------------------------------------------------------------
    namespace
    {
        void lcl_rebuildAndResetCommand( const Reference< XObjectInspectorUI >& _rxInspectorUI, const Reference< XPropertyHandler >& _rxHandler )
        {
            OSL_PRECOND( _rxInspectorUI.is(), "lcl_rebuildAndResetCommand: invalid BrowserUI!" );
            OSL_PRECOND( _rxHandler.is(), "lcl_rebuildAndResetCommand: invalid handler!" );
            _rxInspectorUI->rebuildPropertyUI( PROPERTY_COMMAND );
            _rxHandler->setPropertyValue( PROPERTY_COMMAND, makeAny( ::rtl::OUString() ) );
        }
    }

    //--------------------------------------------------------------------
    void SAL_CALL FormComponentPropertyHandler::actuatingPropertyChanged( const ::rtl::OUString& _rActuatingPropertyName, const Any& _rNewValue, const Any& /*_rOldValue*/, const Reference< XObjectInspectorUI >& _rxInspectorUI, sal_Bool _bFirstTimeInit ) throw (NullPointerException, RuntimeException)
    {
        if ( !_rxInspectorUI.is() )
            throw NullPointerException();

        ::osl::MutexGuard aGuard( m_aMutex );
        PropertyId nActuatingPropId( impl_getPropertyId_throw( _rActuatingPropertyName ) );

        ::std::vector< PropertyId > aDependentProperties;

        switch ( nActuatingPropId )
        {
        // ----- EscapeProcessing -----
        case PROPERTY_ID_ESCAPE_PROCESSING:
            aDependentProperties.push_back( PROPERTY_ID_FILTER );
            aDependentProperties.push_back( PROPERTY_ID_SORT );
            aDependentProperties.push_back( PROPERTY_ID_COMMAND );
            break;  // case PROPERTY_ID_ESCAPE_PROCESSING

        // ----- CommandType -----
        case PROPERTY_ID_COMMANDTYPE:
            // available commands (tables or queries) might have changed
            if ( !_bFirstTimeInit && m_bHaveCommand )
                lcl_rebuildAndResetCommand( _rxInspectorUI, this );
            aDependentProperties.push_back( PROPERTY_ID_COMMAND );
            break;  // case PROPERTY_ID_COMMANDTYPE

        // ----- DataSourceName -----
        case PROPERTY_ID_DATASOURCE:
            // reset the connection, now that we have a new data source
            impl_clearRowsetConnection_nothrow();

            // available list source values (tables or queries) might have changed
            if ( !_bFirstTimeInit && m_bHaveListSource )
                _rxInspectorUI->rebuildPropertyUI( PROPERTY_LISTSOURCE );

            // available commands (tables or queries) might have changed
            if ( !_bFirstTimeInit && m_bHaveCommand )
                lcl_rebuildAndResetCommand( _rxInspectorUI, this );

            // Command also depends on DataSource
            aDependentProperties.push_back( PROPERTY_ID_COMMAND );
            // NO break!

        // ----- Command -----
        case PROPERTY_ID_COMMAND:
            aDependentProperties.push_back( PROPERTY_ID_FILTER );
            aDependentProperties.push_back( PROPERTY_ID_SORT );
            if ( m_bComponentIsSubForm )
                aDependentProperties.push_back( PROPERTY_ID_DETAILFIELDS );
            break;

        // ----- ListSourceType -----
        case PROPERTY_ID_LISTSOURCETYPE:
            if ( !_bFirstTimeInit && m_bHaveListSource )
                // available list source values (tables or queries) might have changed
                _rxInspectorUI->rebuildPropertyUI( PROPERTY_LISTSOURCE );
            aDependentProperties.push_back( PROPERTY_ID_BOUNDCOLUMN );
            aDependentProperties.push_back( PROPERTY_ID_STRINGITEMLIST );
            // NO break!

        // ----- StringItemList -----
        case PROPERTY_ID_STRINGITEMLIST:
            aDependentProperties.push_back( PROPERTY_ID_SELECTEDITEMS );
            aDependentProperties.push_back( PROPERTY_ID_DEFAULT_SELECT_SEQ );
            break;

        // ----- ListSource -----
        case PROPERTY_ID_LISTSOURCE:
            aDependentProperties.push_back( PROPERTY_ID_STRINGITEMLIST );
            break;

        // ----- DataField -----
        case PROPERTY_ID_CONTROLSOURCE:
        {
            ::rtl::OUString sControlSource;
            _rNewValue >>= sControlSource;
            if ( impl_componentHasProperty_throw( PROPERTY_FILTERPROPOSAL ) )
                _rxInspectorUI->enablePropertyUI( PROPERTY_FILTERPROPOSAL, sControlSource.getLength() > 0 );
            if ( impl_componentHasProperty_throw( PROPERTY_EMPTY_IS_NULL ) )
                _rxInspectorUI->enablePropertyUI( PROPERTY_EMPTY_IS_NULL, sControlSource.getLength() > 0 );

            aDependentProperties.push_back( PROPERTY_ID_BOUNDCOLUMN );
            aDependentProperties.push_back( PROPERTY_ID_SCALEIMAGE );
        }
        break;

        // ----- SubmitEncoding -----
        case PROPERTY_ID_SUBMIT_ENCODING:
        {
            FormSubmitEncoding eEncoding = FormSubmitEncoding_URL;
            OSL_VERIFY( _rNewValue >>= eEncoding );
            _rxInspectorUI->enablePropertyUI( PROPERTY_SUBMIT_METHOD, eEncoding == FormSubmitEncoding_URL );
        }
        break;

        // ----- Repeat -----
        case PROPERTY_ID_REPEAT:
        {
            sal_Bool bIsRepeating = sal_False;
            OSL_VERIFY( _rNewValue >>= bIsRepeating );
            _rxInspectorUI->enablePropertyUI( PROPERTY_REPEAT_DELAY, bIsRepeating );
        }
        break;

        // ----- TabStop -----
        case PROPERTY_ID_TABSTOP:
        {
            if ( !impl_componentHasProperty_throw( PROPERTY_TABINDEX ) )
                break;
            sal_Bool bHasTabStop = sal_False;
            _rNewValue >>= bHasTabStop;
            _rxInspectorUI->enablePropertyUI( PROPERTY_TABINDEX, bHasTabStop );
        }
        break;

        // ----- Border -----
        case PROPERTY_ID_BORDER:
        {
            sal_Int16 nBordeType = VisualEffect::NONE;
            OSL_VERIFY( _rNewValue >>= nBordeType );
            _rxInspectorUI->enablePropertyUI( PROPERTY_BORDERCOLOR, nBordeType == VisualEffect::FLAT );
        }
        break;

        // ----- DropDown -----
        case PROPERTY_ID_DROPDOWN:
        {
            if ( impl_isSupportedProperty_nothrow( PROPERTY_ID_LINECOUNT ) )
            {
                sal_Bool bDropDown = sal_True;
                _rNewValue >>= bDropDown;
                _rxInspectorUI->enablePropertyUI( PROPERTY_LINECOUNT, bDropDown );
            }
        }
        break;

        // ----- ImageURL -----
        case PROPERTY_ID_IMAGE_URL:
        {
            if ( impl_isSupportedProperty_nothrow( PROPERTY_ID_IMAGEPOSITION ) )
            {
                ::rtl::OUString sImageURL;
                OSL_VERIFY( _rNewValue >>= sImageURL );
                _rxInspectorUI->enablePropertyUI( PROPERTY_IMAGEPOSITION, sImageURL.getLength() != 0 );
            }

            aDependentProperties.push_back( PROPERTY_ID_SCALEIMAGE );
        }
        break;

        // ----- ButtonType -----
        case PROPERTY_ID_BUTTONTYPE:
        {
            FormButtonType eButtonType( FormButtonType_PUSH );
            OSL_VERIFY( _rNewValue >>= eButtonType );
            _rxInspectorUI->enablePropertyUI( PROPERTY_TARGET_URL, FormButtonType_URL == eButtonType );
        }
        // NO break!

        // ----- TargetURL -----
        case PROPERTY_ID_TARGET_URL:
            aDependentProperties.push_back( PROPERTY_ID_TARGET_FRAME );
            break;  // case PROPERTY_ID_TARGET_URL

        // ----- TriState -----
        case PROPERTY_ID_TRISTATE:
            if ( !_bFirstTimeInit )
                _rxInspectorUI->rebuildPropertyUI( m_eComponentClass == eFormControl ? PROPERTY_DEFAULTCHECKED : PROPERTY_STATE );
            break;  // case PROPERTY_ID_TRISTATE

        // ----- DecimalAccuracy -----
        case PROPERTY_ID_DECIMAL_ACCURACY:
        // ----- ShowThousandsSeparator -----
        case PROPERTY_ID_SHOWTHOUSANDSEP:
        {
            sal_Bool bAccuracy = (PROPERTY_ID_DECIMAL_ACCURACY == nActuatingPropId);
            sal_uInt16  nNewDigits = 0;
            sal_Bool    bUseSep = sal_False;
            if ( bAccuracy )
                OSL_VERIFY( _rNewValue >>= nNewDigits );
            else
                OSL_VERIFY( _rNewValue >>= bUseSep );

            // propagate the changes to the min/max/default fields
            Any aCurrentProp;
            ::rtl::OUString aAffectedProps[] = { PROPERTY_VALUE, PROPERTY_DEFAULT_VALUE, PROPERTY_VALUEMIN, PROPERTY_VALUEMAX };
            for (sal_uInt16 i=0; i<sizeof(aAffectedProps)/sizeof(aAffectedProps[0]); ++i)
            {
                Reference< XPropertyControl > xControl;
                try
                {
                    xControl = _rxInspectorUI->getPropertyControl( aAffectedProps[i] );
                }
                catch( const UnknownPropertyException& e ) { (void)e; }
                if ( xControl.is() )
                {
                    OFormattedNumericControl* pControl = dynamic_cast< OFormattedNumericControl* >( xControl.get() );
                    DBG_ASSERT( pControl, "FormComponentPropertyHandler::actuatingPropertyChanged: invalid control!" );
                    if ( pControl )
                        if ( bAccuracy )
                            pControl->SetDecimalDigits( nNewDigits );
                        else
                            pControl->SetThousandsSep( bUseSep );
                }
            }
        }
        break;

        // ----- FormatKey -----
        case PROPERTY_ID_FORMATKEY:
        {
            FormatDescription aNewDesc;

            Reference< XNumberFormatsSupplier >  xSupplier;
            OSL_VERIFY( m_xComponent->getPropertyValue( PROPERTY_FORMATSSUPPLIER ) >>= xSupplier );

            Reference< XUnoTunnel > xTunnel( xSupplier, UNO_QUERY );
            DBG_ASSERT(xTunnel.is(), "FormComponentPropertyHandler::actuatingPropertyChanged: xTunnel is invalid!");
            SvNumberFormatsSupplierObj* pSupplier = (SvNumberFormatsSupplierObj*)xTunnel->getSomething(SvNumberFormatsSupplierObj::getUnoTunnelId());
                // the same again

            aNewDesc.pSupplier = pSupplier;
            if ( !( _rNewValue >>= aNewDesc.nKey ) )
                aNewDesc.nKey = 0;

            // give each control which has to know this an own copy of the description
            ::rtl::OUString aFormattedPropertyControls[] = {
                PROPERTY_EFFECTIVE_MIN, PROPERTY_EFFECTIVE_MAX, PROPERTY_EFFECTIVE_DEFAULT, PROPERTY_EFFECTIVE_VALUE
            };
            for ( sal_Int16 i=0; i<sizeof(aFormattedPropertyControls)/sizeof(aFormattedPropertyControls[0]); ++i )
            {
                Reference< XPropertyControl > xControl;
                try
                {
                    xControl = _rxInspectorUI->getPropertyControl( aFormattedPropertyControls[i] );
                }
                catch( const UnknownPropertyException& e ) { (void)e; }
                if ( xControl.is() )
                {
                    OFormattedNumericControl* pControl = dynamic_cast< OFormattedNumericControl* >( xControl.get() );
                    DBG_ASSERT( pControl, "FormComponentPropertyHandler::actuatingPropertyChanged: invalid control!" );
                    if ( pControl )
                        pControl->SetFormatDescription( aNewDesc );
                }
            }
        }
        break;

        default:
            DBG_ERROR( "FormComponentPropertyHandler::actuatingPropertyChanged: did not register for this property!" );
            break;

        }   // switch ( nActuatingPropId )

        for ( ::std::vector< PropertyId >::const_iterator loopAffected = aDependentProperties.begin();
              loopAffected != aDependentProperties.end();
              ++loopAffected
            )
        {
            if ( impl_isSupportedProperty_nothrow( *loopAffected ) )
                impl_updateDependentProperty_nothrow( *loopAffected, _rxInspectorUI );
        }
    }

    //------------------------------------------------------------------------
    void FormComponentPropertyHandler::impl_updateDependentProperty_nothrow( PropertyId _nPropId, const Reference< XObjectInspectorUI >& _rxInspectorUI ) const
    {
        try
        {
            switch ( _nPropId )
            {
            // ----- StringItemList -----
            case PROPERTY_ID_STRINGITEMLIST:
            {
                ListSourceType eLSType = ListSourceType_VALUELIST;
                OSL_VERIFY( impl_getPropertyValue_throw( PROPERTY_LISTSOURCETYPE ) >>= eLSType );

                ::rtl::OUString sListSource;
                {
                    Sequence< ::rtl::OUString > aListSource;
                    Any aListSourceValue( impl_getPropertyValue_throw( PROPERTY_LISTSOURCE ) );
                    if ( aListSourceValue >>= aListSource )
                    {
                        if ( aListSource.getLength() )
                            sListSource = aListSource[0];
                    }
                    else
                        OSL_VERIFY( aListSourceValue >>= sListSource );
                }

                sal_Bool bIsEnabled =   (  ( eLSType == ListSourceType_VALUELIST )
                                        || ( sListSource.getLength() == 0 )
                                        );
                _rxInspectorUI->enablePropertyUI( PROPERTY_STRINGITEMLIST, bIsEnabled );
            }
            break;  // case PROPERTY_ID_STRINGITEMLIST

            // ----- BoundColumn -----
            case PROPERTY_ID_BOUNDCOLUMN:
            {
                ::rtl::OUString sControlSource;
                OSL_VERIFY( impl_getPropertyValue_throw( PROPERTY_CONTROLSOURCE ) >>= sControlSource );

                ListSourceType eLSType = ListSourceType_VALUELIST;
                OSL_VERIFY( impl_getPropertyValue_throw( PROPERTY_LISTSOURCETYPE ) >>= eLSType );

                _rxInspectorUI->enablePropertyUI( PROPERTY_BOUNDCOLUMN,
                        ( sControlSource.getLength() > 0 )
                    &&  ( eLSType != ListSourceType_TABLEFIELDS )
                    &&  ( eLSType != ListSourceType_VALUELIST )
                );
            }
            break;  // case PROPERTY_ID_BOUNDCOLUMN

            // ----- ScaleImage -----
            case PROPERTY_ID_SCALEIMAGE:
            {
                ::rtl::OUString sControlSource;
                if ( impl_isSupportedProperty_nothrow( PROPERTY_ID_CONTROLSOURCE ) )
                    impl_getPropertyValue_throw( PROPERTY_CONTROLSOURCE ) >>= sControlSource;

                ::rtl::OUString sImageURL;
                impl_getPropertyValue_throw( PROPERTY_IMAGE_URL ) >>= sImageURL;

                _rxInspectorUI->enablePropertyUI( PROPERTY_SCALEIMAGE,
                    ( sControlSource.getLength() != 0 ) || ( sImageURL.getLength() != 0 )
                );
            }
            break;  // case PROPERTY_ID_SCALEIMAGE

            // ----- SelectedItems -----
            case PROPERTY_ID_SELECTEDITEMS:
            // ----- DefaultSelection -----
            case PROPERTY_ID_DEFAULT_SELECT_SEQ:
            {
                Sequence< ::rtl::OUString > aEntries;
                impl_getPropertyValue_throw( PROPERTY_STRINGITEMLIST ) >>= aEntries;
                bool isEnabled = aEntries.getLength() != 0;

                if ( ( m_nClassId == FormComponentType::LISTBOX ) && ( m_eComponentClass == eFormControl ) )
                {
                    ListSourceType eLSType = ListSourceType_VALUELIST;
                    impl_getPropertyValue_throw( PROPERTY_LISTSOURCETYPE ) >>= eLSType;
                    isEnabled &= ( eLSType == ListSourceType_VALUELIST );
                }
                _rxInspectorUI->enablePropertyUIElements( impl_getPropertyNameFromId_nothrow( _nPropId ),
                    PropertyLineElement::PrimaryButton, isEnabled );
            }
            break;  // case PROPERTY_ID_DEFAULT_SELECT_SEQ

            // ----- TargetFrame ------
            case PROPERTY_ID_TARGET_FRAME:
            {
                ::rtl::OUString sTargetURL;
                impl_getPropertyValue_throw( PROPERTY_TARGET_URL ) >>= sTargetURL;
                FormButtonType eButtonType( FormButtonType_URL );
                if ( 0 != m_nClassId )
                {
                    OSL_VERIFY( impl_getPropertyValue_throw( PROPERTY_BUTTONTYPE ) >>= eButtonType );
                }
                // if m_nClassId is 0, then we're inspecting a form. In this case, eButtonType is always
                // FormButtonType_URL here
                _rxInspectorUI->enablePropertyUI( PROPERTY_TARGET_FRAME,
                    ( eButtonType == FormButtonType_URL ) && ( sTargetURL.getLength() > 0 )
                );
            }
            break;

            // ----- Order ------
            case PROPERTY_ID_SORT:
            // ----- Filter ------
            case PROPERTY_ID_FILTER:
            {
                Reference< XConnection > xConnection;
                bool bAllowEmptyDS = ::dbtools::isEmbeddedInDatabase( m_xComponent, xConnection );

                // if there's no escape processing, we cannot enter any values for this property
                sal_Bool  bDoEscapeProcessing( sal_False );
                impl_getPropertyValue_throw( PROPERTY_ESCAPE_PROCESSING ) >>= bDoEscapeProcessing;
                _rxInspectorUI->enablePropertyUI(
                    impl_getPropertyNameFromId_nothrow( _nPropId ),
                    bDoEscapeProcessing
                );

                // also care for the browse button - enabled if we have escape processing, and a valid
                // data source signature
                _rxInspectorUI->enablePropertyUIElements(
                    impl_getPropertyNameFromId_nothrow( _nPropId ),
                    PropertyLineElement::PrimaryButton,
                        impl_hasValidDataSourceSignature_nothrow( Reference< XForm >( m_xComponent, UNO_QUERY ), bAllowEmptyDS )
                    &&  bDoEscapeProcessing
                );
            }
            break;  // case PROPERTY_ID_FILTER:

            // ----- Command -----
            case PROPERTY_ID_COMMAND:
            {
                sal_Int32       nCommandType( CommandType::COMMAND );
                sal_Bool        bEscapeProcessing( sal_False );
                ::rtl::OUString sDataSource;
                OSL_VERIFY( impl_getPropertyValue_throw( PROPERTY_COMMANDTYPE ) >>= nCommandType );
                OSL_VERIFY( impl_getPropertyValue_throw( PROPERTY_ESCAPE_PROCESSING ) >>= bEscapeProcessing );
                OSL_VERIFY( impl_getPropertyValue_throw( PROPERTY_DATASOURCE ) >>= sDataSource );

                bool doEnable = ( nCommandType == CommandType::COMMAND )
                            &&  ( bEscapeProcessing )
                            &&  (  m_xRowSetConnection.is()
                                || impl_isValidDataSourceName_nothrow( sDataSource )
                                );

                _rxInspectorUI->enablePropertyUIElements(
                    PROPERTY_COMMAND,
                    PropertyLineElement::PrimaryButton,
                    doEnable
                );
            }
            break;  // case PROPERTY_ID_COMMAND

            // ----- DetailFields -----
            case PROPERTY_ID_DETAILFIELDS:
            {
                Reference< XConnection > xConnection;
                bool bAllowEmptyDS = ::dbtools::isEmbeddedInDatabase( m_xComponent, xConnection );

                // both our current form, and it's parent form, need to have a valid
                // data source signature
                bool doEnableMasterDetailFields =
                        impl_hasValidDataSourceSignature_nothrow( Reference< XForm >( m_xComponent,    UNO_QUERY ), bAllowEmptyDS )
                    &&  impl_hasValidDataSourceSignature_nothrow( Reference< XForm >( m_xObjectParent, UNO_QUERY ), bAllowEmptyDS );

                // in opposite to the other properties, here in real *two* properties are
                // affected
                _rxInspectorUI->enablePropertyUIElements( PROPERTY_DETAILFIELDS, PropertyLineElement::PrimaryButton, doEnableMasterDetailFields );
                _rxInspectorUI->enablePropertyUIElements( PROPERTY_MASTERFIELDS, PropertyLineElement::PrimaryButton, doEnableMasterDetailFields );
            }
            break;

            default:
                OSL_ENSURE( false, "FormComponentPropertyHandler::impl_updateDependentProperty_nothrow: unexpected property to update!" );
                break;

            }   // switch
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "FormComponentPropertyHandler::impl_updateDependentProperty_nothrow: caught an exception!" );
        }
    }

    //------------------------------------------------------------------------
    void SAL_CALL FormComponentPropertyHandler::disposing()
    {
        FormComponentPropertyHandler_Base::disposing();
        if ( m_xCommandDesigner.is() && m_xCommandDesigner->isActive() )
            m_xCommandDesigner->dispose();
    }

    //------------------------------------------------------------------------
    sal_Bool SAL_CALL FormComponentPropertyHandler::suspend( sal_Bool _bSuspend ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( _bSuspend )
            if ( m_xCommandDesigner.is() && m_xCommandDesigner->isActive() )
                return m_xCommandDesigner->suspend();
        return sal_True;
    }

    //------------------------------------------------------------------------
    void FormComponentPropertyHandler::impl_initComponentMetaData_throw()
    {
        try
        {
            //////////////////////////////////////////////////////////////////
            // component class
            m_eComponentClass = eUnknown;

            if  (   impl_componentHasProperty_throw( PROPERTY_WIDTH )
                &&  impl_componentHasProperty_throw( PROPERTY_HEIGHT )
                &&  impl_componentHasProperty_throw( PROPERTY_POSITIONX )
                &&  impl_componentHasProperty_throw( PROPERTY_POSITIONY )
                )
            {
                m_eComponentClass = eDialogControl;
            }
            else
            {
                m_eComponentClass = eFormControl;
            }

            //////////////////////////////////////////////////////////////////
            // (database) sub form?
            Reference< XForm > xAsForm( m_xComponent, UNO_QUERY );
            if ( xAsForm.is() )
            {
                Reference< XChild > xFormAsChild( xAsForm, UNO_QUERY );
                Reference< XForm > xFormsParent;
                if ( xFormAsChild.is() )
                    xFormsParent = xFormsParent.query( xFormAsChild->getParent() );
                m_bComponentIsSubForm = xFormsParent.is();
            }

            //////////////////////////////////////////////////////////////////
            // ClassId
            Reference< XChild > xCompAsChild( m_xComponent, UNO_QUERY );
            if ( xCompAsChild.is() )
                m_xObjectParent = xCompAsChild->getParent();

            //////////////////////////////////////////////////////////////////
            // ClassId
            impl_classifyControlModel_throw();
        }
        catch( const RuntimeException& )
        {
            throw;
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "FormComponentPropertyHandler::impl_initComponentMetaData_throw: caught an exception!" );
        }
    }

    //------------------------------------------------------------------------
    void FormComponentPropertyHandler::impl_classifyControlModel_throw( )
    {
        if ( impl_componentHasProperty_throw( PROPERTY_CLASSID ) )
        {
            OSL_VERIFY( m_xComponent->getPropertyValue( PROPERTY_CLASSID ) >>= m_nClassId );
        }
        else if ( eDialogControl == m_eComponentClass )
        {
            Reference< XControlModel > xControlModel( m_xComponent, UNO_QUERY );
            Reference< XServiceInfo > xServiceInfo( xControlModel, UNO_QUERY );
            if ( xServiceInfo.is() )
            {
                // it's a control model, and can tell about it's supported services
                m_nClassId = FormComponentType::CONTROL;

                const sal_Char* aControlModelServiceNames[] =
                {
                    "UnoControlButtonModel",
                    "UnoControlCheckBoxModel",
                    "UnoControlComboBoxModel",
                    "UnoControlCurrencyFieldModel",
                    "UnoControlDateFieldModel",
                    "UnoControlEditModel",
                    "UnoControlFileControlModel",
                    "UnoControlFixedTextModel",
                    "UnoControlGroupBoxModel",
                    "UnoControlImageControlModel",
                    "UnoControlListBoxModel",
                    "UnoControlNumericFieldModel",
                    "UnoControlPatternFieldModel",
                    "UnoControlRadioButtonModel",
                    "UnoControlScrollBarModel",
                    "UnoControlSpinButtonModel",
                    "UnoControlTimeFieldModel",

                    "UnoControlFixedLineModel",
                    "UnoControlFormattedFieldModel",
                    "UnoControlProgressBarModel"
                };
                const sal_Int16 nClassIDs[] =
                {
                    FormComponentType::COMMANDBUTTON,
                    FormComponentType::CHECKBOX,
                    FormComponentType::COMBOBOX,
                    FormComponentType::CURRENCYFIELD,
                    FormComponentType::DATEFIELD,
                    FormComponentType::TEXTFIELD,
                    FormComponentType::FILECONTROL,
                    FormComponentType::FIXEDTEXT,
                    FormComponentType::GROUPBOX,
                    FormComponentType::IMAGECONTROL,
                    FormComponentType::LISTBOX,
                    FormComponentType::NUMERICFIELD,
                    FormComponentType::PATTERNFIELD,
                    FormComponentType::RADIOBUTTON,
                    FormComponentType::SCROLLBAR,
                    FormComponentType::SPINBUTTON,
                    FormComponentType::TIMEFIELD,

                    ControlType::FIXEDLINE,
                    ControlType::FORMATTEDFIELD,
                    ControlType::PROGRESSBAR
                };

                sal_Int32 nKnownControlTypes = sizeof( aControlModelServiceNames ) / sizeof( aControlModelServiceNames[ 0 ] );
                OSL_ENSURE( nKnownControlTypes == sizeof( nClassIDs ) / sizeof( nClassIDs[ 0 ] ),
                    "FormComponentPropertyHandler::impl_classifyControlModel_throw: inconsistence" );

                for ( sal_Int32 i = 0; i < nKnownControlTypes; ++i )
                {
                    ::rtl::OUString sServiceName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt." ) );
                    sServiceName += ::rtl::OUString::createFromAscii( aControlModelServiceNames[ i ] );

                    if ( xServiceInfo->supportsService( sServiceName ) )
                    {
                        m_nClassId = nClassIDs[ i ];
                        break;
                    }
                }
            }
        }
    }

    //------------------------------------------------------------------------
    void FormComponentPropertyHandler::impl_normalizePropertyValue_nothrow( Any& _rValue, PropertyId _nPropId ) const
    {
        switch ( _nPropId )
        {
        case PROPERTY_ID_TABSTOP:
            if ( !_rValue.hasValue() )
            {
                switch ( m_nClassId )
                {
                case FormComponentType::COMMANDBUTTON:
                case FormComponentType::RADIOBUTTON:
                case FormComponentType::CHECKBOX:
                case FormComponentType::TEXTFIELD:
                case FormComponentType::LISTBOX:
                case FormComponentType::COMBOBOX:
                case FormComponentType::FILECONTROL:
                case FormComponentType::DATEFIELD:
                case FormComponentType::TIMEFIELD:
                case FormComponentType::NUMERICFIELD:
                case ControlType::FORMATTEDFIELD:
                case FormComponentType::CURRENCYFIELD:
                case FormComponentType::PATTERNFIELD:
                    _rValue = makeAny( (sal_Bool)sal_True );
                    break;
                default:
                    _rValue = makeAny( (sal_Bool)sal_False );
                    break;
                }
            }
            break;
        }
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_shouldExcludeProperty_nothrow( const Property& _rProperty ) const
    {
        OSL_ENSURE( _rProperty.Handle == m_pInfoService->getPropertyId( _rProperty.Name ),
            "FormComponentPropertyHandler::impl_shouldExcludeProperty_nothrow: insonsistency in the property!" );

        if ( _rProperty.Handle == PROPERTY_ID_CONTROLLABEL )
            // prevent that this is caught below
            return false;

        if  (   ( _rProperty.Type.getTypeClass() == TypeClass_INTERFACE )
            ||  ( _rProperty.Type.getTypeClass() == TypeClass_ARRAY )
            ||  ( _rProperty.Type.getTypeClass() == TypeClass_UNKNOWN )
            )
            return true;

        if ( ( _rProperty.Attributes & PropertyAttribute::TRANSIENT ) && ( m_eComponentClass != eDialogControl ) )
            // strange enough, dialog controls declare a lot of their properties as transient
            return true;

        if ( _rProperty.Attributes & PropertyAttribute::READONLY )
            return true;

        switch ( _rProperty.Handle )
        {
        case PROPERTY_ID_MASTERFIELDS:
        case PROPERTY_ID_DETAILFIELDS:
            if ( !m_bComponentIsSubForm )
                // no master and detail fields for forms which are no sub forms
                return true;
            break;

        case PROPERTY_ID_DATASOURCE:
        {
            // don't show DataSource if the component is part of an embedded form document
            Reference< XConnection > xConn;
            if ( isEmbeddedInDatabase( m_xComponent, xConn ) )
                return true;
        }
        break;

        case PROPERTY_ID_TEXT:
            // don't show the "Text" property of formatted fields
            if ( ControlType::FORMATTEDFIELD == m_nClassId )
                return true;
            break;

        case PROPERTY_ID_FORMATKEY:
        case PROPERTY_ID_EFFECTIVE_MIN:
        case PROPERTY_ID_EFFECTIVE_MAX:
        case PROPERTY_ID_EFFECTIVE_DEFAULT:
        case PROPERTY_ID_EFFECTIVE_VALUE:
            // only if the set has a formats supplier, too
            if  ( !impl_componentHasProperty_throw( PROPERTY_FORMATSSUPPLIER ) )
                return true;
            // (form) date and time fields also have a formats supplier, but the format itself
            // is reflected in another property
            if  (   ( FormComponentType::DATEFIELD == m_nClassId )
                ||  ( FormComponentType::TIMEFIELD == m_nClassId )
                )
                return true;
            break;
        }

        sal_uInt32 nPropertyUIFlags = m_pInfoService->getPropertyUIFlags( _rProperty.Handle );

        // don't show experimental properties unless allowed to do so
        if ( ( nPropertyUIFlags & PROP_FLAG_EXPERIMENTAL ) != 0 )
        {
            if ( true ) // TODO
                return true;
        }

        // no data properties if no Base is installed. #124939#
        // This fix is not intendend to appear on the main trunk. If you find it there,
        // please tell me! frank.schoenheit@sun.com
        if ( ( nPropertyUIFlags & PROP_FLAG_DATA_PROPERTY ) != 0 )
            if ( !SvtModuleOptions().IsModuleInstalled( SvtModuleOptions::E_SDATABASE ) )
                return true;

        return false;
    }

    //------------------------------------------------------------------------
    sal_Int16 FormComponentPropertyHandler::impl_getDocumentMeasurementUnit_throw() const
    {
        FieldUnit eUnit = FUNIT_NONE;

        Reference< XServiceInfo > xDocumentSI( impl_getContextDocument_nothrow(), UNO_QUERY );
        OSL_ENSURE( xDocumentSI.is(), "FormComponentPropertyHandler::impl_getDocumentMeasurementUnit_throw: No context document - where do I live?" );
        if ( xDocumentSI.is() )
        {
            // determine the application type we live in
            ::rtl::OUString sConfigurationLocation;
            ::rtl::OUString sConfigurationProperty;
            if ( xDocumentSI->supportsService( SERVICE_WEB_DOCUMENT ) )
            {   // writer
                sConfigurationLocation = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/org.openoffice.Office.WriterWeb/Layout/Other" ) );
                sConfigurationProperty = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MeasureUnit" ) );
            }
            else if ( xDocumentSI->supportsService( SERVICE_TEXT_DOCUMENT ) )
            {   // writer
                sConfigurationLocation = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/org.openoffice.Office.Writer/Layout/Other" ) );
                sConfigurationProperty = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MeasureUnit" ) );
            }
            else if ( xDocumentSI->supportsService( SERVICE_SPREADSHEET_DOCUMENT ) )
            {   // calc
                sConfigurationLocation = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/org.openoffice.Office.Calc/Layout/Other/MeasureUnit" ) );
                sConfigurationProperty = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Metric" ) );
            }
            else if ( xDocumentSI->supportsService( SERVICE_DRAWING_DOCUMENT ) )
            {
                sConfigurationLocation = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/org.openoffice.Office.Draw/Layout/Other/MeasureUnit" ) );
                sConfigurationProperty = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Metric" ) );
            }
            else if ( xDocumentSI->supportsService( SERVICE_PRESENTATION_DOCUMENT ) )
            {
                sConfigurationLocation = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/org.openoffice.Office.Impress/Layout/Other/MeasureUnit" ) );
                sConfigurationProperty = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Metric" ) );
            }

            // read the measurement unit from the configuration
            if ( sConfigurationLocation.getLength() && sConfigurationProperty.getLength() )
            {
                ::utl::OConfigurationTreeRoot aConfigTree( ::utl::OConfigurationTreeRoot::createWithServiceFactory(
                    m_aContext.getLegacyServiceFactory(), sConfigurationLocation, -1, ::utl::OConfigurationTreeRoot::CM_READONLY ) );
                sal_Int32 nUnitAsInt = (sal_Int32)FUNIT_NONE;
                aConfigTree.getNodeValue( sConfigurationProperty ) >>= nUnitAsInt;

                // if this denotes a valid (and accepted) unit, then use it
                if  ( ( nUnitAsInt > FUNIT_NONE ) && ( nUnitAsInt <= FUNIT_100TH_MM ) )
                    eUnit = static_cast< FieldUnit >( nUnitAsInt );
            }
        }

        if ( FUNIT_NONE == eUnit )
        {
            MeasurementSystem eSystem = SvtSysLocale().GetLocaleData().getMeasurementSystemEnum();
            eUnit = MEASURE_METRIC == eSystem ? FUNIT_CM : FUNIT_INCH;
        }

        return MeasurementUnitConversion::convertToMeasurementUnit( eUnit, 1 );
    }

    //------------------------------------------------------------------------
    Reference< XRowSet > FormComponentPropertyHandler::impl_getRowSet_throw( ) const
    {
        Reference< XRowSet > xRowSet( m_xComponent, UNO_QUERY );
        if ( !xRowSet.is() )
        {
            xRowSet = Reference< XRowSet >( m_xObjectParent, UNO_QUERY );
            if ( !xRowSet.is() )
            {
                // are we inspecting a grid column?
                if (Reference< XGridColumnFactory >( m_xObjectParent, UNO_QUERY) .is())
                {   // yes
                    Reference< XChild > xParentAsChild( m_xObjectParent, UNO_QUERY );
                    if ( xParentAsChild.is() )
                        xRowSet = Reference< XRowSet >( xParentAsChild->getParent(), UNO_QUERY );
                }
            }
        }
        DBG_ASSERT( xRowSet.is(), "FormComponentPropertyHandler::impl_getRowSet_throw: could not obtain the rowset for the introspectee!" );
        return xRowSet;
    }

    //------------------------------------------------------------------------
    Reference< XRowSet > FormComponentPropertyHandler::impl_getRowSet_nothrow( ) const
    {
        Reference< XRowSet > xReturn;
        try
        {
            xReturn = impl_getRowSet_throw();
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "FormComponentPropertyHandler::impl_getRowSet_nothrow: caught an exception!" );
        }
        return xReturn;
    }

    //------------------------------------------------------------------------
    void FormComponentPropertyHandler::impl_initFieldList_nothrow( ::std::vector< ::rtl::OUString >& _rFieldNames ) const
    {
        clearContainer( _rFieldNames );
        try
        {
            WaitCursor aWaitCursor( impl_getDefaultDialogParent_nothrow() );

            Reference< XPreparedStatement >  xStatement;

            // get the form of the control we're inspecting
            Reference< XPropertySet > xFormSet( impl_getRowSet_throw(), UNO_QUERY );
            if ( !xFormSet.is() )
                return;

            ::rtl::OUString sObjectName;
            OSL_VERIFY( xFormSet->getPropertyValue( PROPERTY_COMMAND ) >>= sObjectName );
            // when there is no command we don't need to ask for columns
            if ( sObjectName.getLength() && impl_ensureRowsetConnection_nothrow() )
            {
                ::rtl::OUString aDatabaseName;
                OSL_VERIFY( xFormSet->getPropertyValue( PROPERTY_DATASOURCE ) >>= aDatabaseName );
                sal_Int32 nObjectType = CommandType::COMMAND;
                OSL_VERIFY( xFormSet->getPropertyValue( PROPERTY_COMMANDTYPE ) >>= nObjectType );

                Sequence< ::rtl::OUString > aFields( ::dbtools::getFieldNamesByCommandDescriptor( m_xRowSetConnection, nObjectType, sObjectName ) );

                const ::rtl::OUString* pFields = aFields.getConstArray();
                for ( sal_Int32 i = 0; i < aFields.getLength(); ++i, ++pFields )
                    _rFieldNames.push_back( *pFields );
            }
        }
        catch (Exception&)
        {
            DBG_ERROR( "FormComponentPropertyHandler::impl_initFieldList_nothrow: caught an exception!" )
        }
    }

    //------------------------------------------------------------------------
    void FormComponentPropertyHandler::impl_clearRowsetConnection_nothrow()
    {
        m_xRowSetConnection.clear();
    }

    //------------------------------------------------------------------------
    void FormComponentPropertyHandler::impl_displaySQLError_nothrow( const ::dbtools::SQLExceptionInfo& _rErrorDescriptor ) const
    {
        ::dbtools::showError( _rErrorDescriptor, VCLUnoHelper::GetInterface( impl_getDefaultDialogParent_nothrow() ), m_aContext.getLegacyServiceFactory() );
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_ensureRowsetConnection_nothrow() const
    {
        if ( m_xRowSetConnection.is() )
            return true;

        Reference< XRowSet > xRowSet( impl_getRowSet_throw() );
        Reference< XPropertySet > xRowSetProps( xRowSet, UNO_QUERY );

        // connect the row set - this is delegated to elsewhere - while observing errors
        SQLExceptionInfo aError;
        try
        {
            if ( xRowSetProps.is() )
            {
                WaitCursor aWaitCursor( impl_getDefaultDialogParent_nothrow() );
                m_xRowSetConnection = ::dbtools::ensureRowSetConnection( xRowSet, m_aContext.getLegacyServiceFactory(), false );
            }
        }
        catch (SQLContext& e) { aError = e; }
        catch (SQLWarning& e) { aError = e; }
        catch (SQLException& e) { aError = e; }
        catch (WrappedTargetException& e ) { aError = SQLExceptionInfo( e.TargetException ); }
        catch (Exception&) { }

        // report errors, if necessary
        if ( aError.isValid() )
        {
            ::rtl::OUString sDataSourceName;
            try
            {
                xRowSetProps->getPropertyValue( PROPERTY_DATASOURCE ) >>= sDataSourceName;
            }
            catch( const Exception& )
            {
                DBG_ERROR( "FormComponentPropertyHandler::impl_ensureRowsetConnection_nothrow: caught an exception during error handling!" );
            }
            // additional info about what happended
            String sInfo( PcrRes( RID_STR_UNABLETOCONNECT ) );
            INetURLObject aParser( sDataSourceName );
            if ( aParser.GetProtocol() != INET_PROT_NOT_VALID )
                sDataSourceName = aParser.getBase( INetURLObject::LAST_SEGMENT, true, INetURLObject::DECODE_WITH_CHARSET );
            sInfo.SearchAndReplaceAllAscii( "$name$", sDataSourceName );

            SQLContext aContext;
            aContext.Message = sInfo;
            aContext.NextException = aError.get();
            impl_displaySQLError_nothrow( aContext );
        }

        return m_xRowSetConnection.is();
    }

    //------------------------------------------------------------------------
    void FormComponentPropertyHandler::impl_describeCursorSource_nothrow( LineDescriptor& _out_rProperty, const Reference< XPropertyControlFactory >& _rxControlFactory ) const
    {
        try
        {
            WaitCursor aWaitCursor( impl_getDefaultDialogParent_nothrow() );

            ////////////////////////////////////////////////////////////
            // Setzen der UI-Daten
            _out_rProperty.DisplayName = m_pInfoService->getPropertyTranslation( PROPERTY_ID_COMMAND );

            _out_rProperty.HelpURL = HelpIdUrl::getHelpURL( m_pInfoService->getPropertyHelpId( PROPERTY_ID_COMMAND ) );
            _out_rProperty.PrimaryButtonId = UID_PROP_DLG_SQLCOMMAND;

            ////////////////////////////////////////////////////////////
            sal_Int32 nCommandType = CommandType::COMMAND;
            impl_getPropertyValue_throw( PROPERTY_COMMANDTYPE ) >>= nCommandType;

            switch ( nCommandType )
            {
            case CommandType::TABLE:
            case CommandType::QUERY:
            {
                ::std::vector< ::rtl::OUString > aNames;
                if ( impl_ensureRowsetConnection_nothrow() )
                {
                    if ( nCommandType == CommandType::TABLE )
                        impl_fillTableNames_throw( aNames );
                    else
                        impl_fillQueryNames_throw( aNames );
                }
                _out_rProperty.Control = PropertyHandlerHelper::createComboBoxControl( _rxControlFactory, aNames, sal_False, sal_True );
            }
            break;

            default:
                _out_rProperty.Control = _rxControlFactory->createPropertyControl( PropertyControlType::MultiLineTextField, sal_False );
                break;
            }
        }
        catch (Exception&)
        {
            DBG_ERROR("FormComponentPropertyHandler::impl_describeCursorSource_nothrow: caught an exception !")
        }
    }

    //------------------------------------------------------------------------
    void FormComponentPropertyHandler::impl_fillTableNames_throw( ::std::vector< ::rtl::OUString >& _out_rNames ) const
    {
        OSL_PRECOND( m_xRowSetConnection.is(), "FormComponentPropertyHandler::impl_fillTableNames_throw: need a connection!" );
        _out_rNames.resize( 0 );

        Reference< XTablesSupplier > xSupplyTables( m_xRowSetConnection, UNO_QUERY );
        Reference< XNameAccess > xTableNames;
        if ( xSupplyTables.is() )
            xTableNames = xSupplyTables->getTables();
        DBG_ASSERT( xTableNames.is(), "FormComponentPropertyHandler::impl_fillTableNames_throw: no way to obtain the tables of the connection!" );
        if ( !xTableNames.is() )
            return;

        Sequence< ::rtl::OUString> aTableNames = xTableNames->getElementNames();
        sal_uInt32 nCount = aTableNames.getLength();
        const ::rtl::OUString* pTableNames = aTableNames.getConstArray();

        for ( sal_uInt32 i=0; i<nCount; ++i ,++pTableNames )
            _out_rNames.push_back( *pTableNames );
    }

    //------------------------------------------------------------------------
    void FormComponentPropertyHandler::impl_fillQueryNames_throw( ::std::vector< ::rtl::OUString >& _out_rNames ) const
    {
        OSL_PRECOND( m_xRowSetConnection.is(), "FormComponentPropertyHandler::impl_fillQueryNames_throw: need a connection!" );
        _out_rNames.resize( 0 );

        Reference< XQueriesSupplier > xSupplyQueries( m_xRowSetConnection, UNO_QUERY );
        Reference< XNameAccess > xQueryNames;
        if ( xSupplyQueries.is() )
            xQueryNames = xSupplyQueries->getQueries();
        DBG_ASSERT( xQueryNames.is(), "FormComponentPropertyHandler::impl_fillQueryNames_throw: no way to obtain the queries of the connection!" );
        if ( !xQueryNames.is() )
            return;

        Sequence< ::rtl::OUString> aQueryNames = xQueryNames->getElementNames();
        sal_uInt32 nCount = aQueryNames.getLength();
        const ::rtl::OUString* pQueryNames = aQueryNames.getConstArray();

        for ( sal_uInt32 i=0; i<nCount; i++, ++pQueryNames )
            _out_rNames.push_back( *pQueryNames );
    }

    //------------------------------------------------------------------------
    void FormComponentPropertyHandler::impl_describeListSourceUI_throw( LineDescriptor& _out_rDescriptor, const Reference< XPropertyControlFactory >& _rxControlFactory ) const
    {
        OSL_PRECOND( m_xComponent.is(), "FormComponentPropertyHandler::impl_describeListSourceUI_throw: no component!" );

        ////////////////////////////////////////////////////////////
        // Auslesen des ListSourceTypes
        Any aListSourceType( m_xComponent->getPropertyValue( PROPERTY_LISTSOURCETYPE ) );

        sal_Int32 nListSourceType = ListSourceType_VALUELIST;
        ::cppu::enum2int( nListSourceType, aListSourceType );

        _out_rDescriptor.DisplayName = m_pInfoService->getPropertyTranslation( PROPERTY_ID_LISTSOURCE );
        _out_rDescriptor.HelpURL = HelpIdUrl::getHelpURL( m_pInfoService->getPropertyHelpId( PROPERTY_ID_LISTSOURCE ) );

        ////////////////////////////////////////////////////////////
        // Enums setzen
        switch( nListSourceType )
        {
        case ListSourceType_VALUELIST:
            _out_rDescriptor.Control = _rxControlFactory->createPropertyControl( PropertyControlType::StringListField, sal_False );
            break;

        case ListSourceType_TABLEFIELDS:
        case ListSourceType_TABLE:
        case ListSourceType_QUERY:
        {
            ::std::vector< ::rtl::OUString > aListEntries;
            if ( impl_ensureRowsetConnection_nothrow() )
            {
                if ( nListSourceType == ListSourceType_QUERY )
                    impl_fillQueryNames_throw( aListEntries );
                else
                    impl_fillTableNames_throw( aListEntries );
            }
            _out_rDescriptor.Control = PropertyHandlerHelper::createComboBoxControl( _rxControlFactory, aListEntries, sal_False, sal_False );
        }
        break;
        }
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_dialogListSelection_nothrow( const ::rtl::OUString& _rProperty, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const
    {
        OSL_PRECOND( m_pInfoService.get(), "FormComponentPropertyHandler::impl_dialogListSelection_nothrow: no property meta data!" );

        String sPropertyUIName( m_pInfoService->getPropertyTranslation( m_pInfoService->getPropertyId( _rProperty ) ) );
        ListSelectionDialog aDialog( impl_getDefaultDialogParent_nothrow(), m_xComponent, _rProperty, sPropertyUIName );
        _rClearBeforeDialog.clear();
        return ( RET_OK == aDialog.Execute() );
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_dialogFilterOrSort_nothrow( bool _bFilter, ::rtl::OUString& _out_rSelectedClause, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const
    {
        OSL_PRECOND( Reference< XRowSet >( m_xComponent, UNO_QUERY ).is(),
            "FormComponentPropertyHandler::impl_dialogFilterOrSort_nothrow: to be called for forms only!" );

        _out_rSelectedClause = ::rtl::OUString();
        bool bSuccess = false;
        SQLExceptionInfo aErrorInfo;
        try
        {
            if ( !impl_ensureRowsetConnection_nothrow() )
                return false;

            // get a composer for the statement which the form is currently based on
            Reference< XSingleSelectQueryComposer > xComposer( ::dbtools::getCurrentSettingsComposer( m_xComponent, m_aContext.getLegacyServiceFactory() ) );
            OSL_ENSURE( xComposer.is(), "FormComponentPropertyHandler::impl_dialogFilterOrSort_nothrow: could not obtain a composer!" );
            if ( !xComposer.is() )
                return false;

            ::rtl::OUString sPropertyUIName( m_pInfoService->getPropertyTranslation( _bFilter ? PROPERTY_ID_FILTER : PROPERTY_ID_SORT ) );

            const sal_Char* pAsciiServiceName = _bFilter ? "com.sun.star.sdb.FilterDialog" : "com.sun.star.sdb.OrderDialog";

            // create the dialog
            Reference< XExecutableDialog > xDialog;
            if ( !m_aContext.createComponent( pAsciiServiceName, xDialog ) )
            {
                ShowServiceNotAvailableError( impl_getDefaultDialogParent_nothrow(), ::rtl::OUString::createFromAscii( pAsciiServiceName ), sal_True );
                return false;
            }

            // initialize the dialog
            Reference< XPropertySet > xDialogProps( xDialog, UNO_QUERY_THROW );
            xDialogProps->setPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "QueryComposer" ) ), makeAny( xComposer ) );
            xDialogProps->setPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "RowSet" ) ),        makeAny( m_xComponent ) );
            xDialogProps->setPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ParentWindow" ) ),  makeAny( VCLUnoHelper::GetInterface( impl_getDefaultDialogParent_nothrow() ) ) );
            xDialogProps->setPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Title" ) ),         makeAny( sPropertyUIName ) );

            _rClearBeforeDialog.clear();
            bSuccess = ( xDialog->execute() != 0 );
            if ( bSuccess )
                _out_rSelectedClause = _bFilter ? xComposer->getFilter() : xComposer->getOrder();
        }
        catch (SQLContext& e) { aErrorInfo = e; }
        catch (SQLWarning& e) { aErrorInfo = e; }
        catch (SQLException& e) { aErrorInfo = e; }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "FormComponentPropertyHandler::impl_dialogFilterOrSort_nothrow: caught an exception!" );
        }

        if ( aErrorInfo.isValid() )
            impl_displaySQLError_nothrow( aErrorInfo );

        return bSuccess;
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_dialogLinkedFormFields_nothrow( ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const
    {
        Reference< XForm > xDetailForm( m_xComponent, UNO_QUERY );
        Reference< XForm > xMasterForm( m_xObjectParent, UNO_QUERY );
        OSL_PRECOND( xDetailForm.is() && xMasterForm.is(), "FormComponentPropertyHandler::impl_dialogLinkedFormFields_nothrow: no forms!" );
        if ( !xDetailForm.is() || !xMasterForm.is() )
            return false;

        FormLinkDialog aDialog( impl_getDefaultDialogParent_nothrow(), xDetailForm, xMasterForm, m_aContext.getLegacyServiceFactory() );
        _rClearBeforeDialog.clear();
        return ( RET_OK == aDialog.Execute() );
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_dialogFormatting_nothrow( Any& _out_rNewValue, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const
    {
        bool bChanged = false;
        try
        {
            // create the itemset for the dialog
            SfxItemSet aCoreSet(SFX_APP()->GetPool(),
                SID_ATTR_NUMBERFORMAT_VALUE, SID_ATTR_NUMBERFORMAT_VALUE,
                SID_ATTR_NUMBERFORMAT_INFO, SID_ATTR_NUMBERFORMAT_INFO,
                0);     // ripped this somewhere ... don't understand it :(

            // get the number formats supplier
            Reference< XNumberFormatsSupplier >  xSupplier;
            m_xComponent->getPropertyValue( PROPERTY_FORMATSSUPPLIER ) >>= xSupplier;

            DBG_ASSERT(xSupplier.is(), "FormComponentPropertyHandler::impl_dialogFormatting_nothrow: invalid call !" );
            Reference< XUnoTunnel > xTunnel( xSupplier, UNO_QUERY_THROW );
            SvNumberFormatsSupplierObj* pSupplier =
                reinterpret_cast< SvNumberFormatsSupplierObj* >( xTunnel->getSomething( SvNumberFormatsSupplierObj::getUnoTunnelId() ) );
            DBG_ASSERT( pSupplier != NULL, "FormComponentPropertyHandler::impl_dialogFormatting_nothrow: invalid call !" );

            sal_Int32 nFormatKey = 0;
            impl_getPropertyValue_throw( PROPERTY_FORMATKEY ) >>= nFormatKey;
            aCoreSet.Put( SfxUInt32Item( SID_ATTR_NUMBERFORMAT_VALUE, nFormatKey ) );

            SvNumberFormatter* pFormatter = pSupplier->GetNumberFormatter();
            double dPreviewVal = 1234.56789;
            SvxNumberInfoItem aFormatter( pFormatter, dPreviewVal, SID_ATTR_NUMBERFORMAT_INFO );
            aCoreSet.Put( aFormatter );

            // a tab dialog with a single page
            ::std::auto_ptr< SfxSingleTabDialog > pDialog( new SfxSingleTabDialog( impl_getDefaultDialogParent_nothrow(), aCoreSet, 0 ) );
            SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
            DBG_ASSERT( pFact, "CreateFactory fail!" );
            ::CreateTabPage fnCreatePage = pFact->GetTabPageCreatorFunc( RID_SVXPAGE_NUMBERFORMAT );
            if ( !fnCreatePage )
                throw RuntimeException();   // caught below

            SfxTabPage* pPage = (*fnCreatePage)( pDialog.get(), aCoreSet );
            pDialog->SetTabPage( pPage );

            _rClearBeforeDialog.clear();
            if ( RET_OK == pDialog->Execute() )
            {
                const SfxItemSet* pResult = pDialog->GetOutputItemSet();

                const SfxPoolItem* pItem = pResult->GetItem( SID_ATTR_NUMBERFORMAT_INFO );
                const SvxNumberInfoItem* pInfoItem = dynamic_cast< const SvxNumberInfoItem* >( pItem );
                if (pInfoItem && pInfoItem->GetDelCount())
                {
                    const sal_uInt32* pDeletedKeys = pInfoItem->GetDelArray();

                    for (sal_uInt16 i=0; i< pInfoItem->GetDelCount(); ++i, ++pDeletedKeys)
                        pFormatter->DeleteEntry(*pDeletedKeys);
                }

                pItem = NULL;
                if ( SFX_ITEM_SET == pResult->GetItemState( SID_ATTR_NUMBERFORMAT_VALUE, sal_False, &pItem ) )
                {
                    _out_rNewValue <<= (sal_Int32)( static_cast< const SfxUInt32Item* >( pItem )->GetValue() );
                    bChanged = true;
                }
            }
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "FormComponentPropertyHandler::impl_dialogFormatting_nothrow: : caught an exception!" );
        }
        return bChanged;
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_browseForImage_nothrow( Any& _out_rNewValue, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const
    {
        ::rtl::OUString aStrTrans = m_pInfoService->getPropertyTranslation( PROPERTY_ID_IMAGE_URL );

        ::sfx2::FileDialogHelper aFileDlg(SFXWB_GRAPHIC);

        aFileDlg.SetTitle(aStrTrans);

        Reference< XFilePickerControlAccess > xController(aFileDlg.GetFilePicker(), UNO_QUERY);
        DBG_ASSERT(xController.is(), "FormComponentPropertyHandler::impl_browseForImage_nothrow: missing the controller interface on the file picker!");
        if (xController.is())
        {
            // do a preview by default
            xController->setValue(ExtendedFilePickerElementIds::CHECKBOX_PREVIEW, 0, ::cppu::bool2any(sal_True));

            // "as link" is checked, but disabled
            xController->setValue(ExtendedFilePickerElementIds::CHECKBOX_LINK, 0, ::cppu::bool2any(sal_True));
            xController->enableControl(ExtendedFilePickerElementIds::CHECKBOX_LINK, sal_False);
        }

        ::rtl::OUString sCurValue;
        OSL_VERIFY( impl_getPropertyValue_throw( PROPERTY_IMAGE_URL ) >>= sCurValue );
        if ( sCurValue.getLength() != 0 )
        {
            aFileDlg.SetDisplayDirectory( sCurValue );
            // TODO: need to set the display directory _and_ the default name
        }

        _rClearBeforeDialog.clear();
        bool bSuccess = ( 0 == aFileDlg.Execute() );
        if ( bSuccess )
            _out_rNewValue <<= (::rtl::OUString)aFileDlg.GetPath();
        return bSuccess;
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_browseForTargetURL_nothrow( Any& _out_rNewValue, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const
    {
        ::sfx2::FileDialogHelper aFileDlg( WB_3DLOOK );

        ::rtl::OUString sURL;
        OSL_VERIFY( impl_getPropertyValue_throw( PROPERTY_TARGET_URL ) >>= sURL );
        INetURLObject aParser( sURL );
        if ( INET_PROT_FILE == aParser.GetProtocol() )
            // set the initial directory only for file-URLs. Everything else
            // is considered to be potentially expensive
            // 106126 - 2002/12/10 - fs@openoffice.org
            aFileDlg.SetDisplayDirectory( sURL );

        _rClearBeforeDialog.clear();
        bool bSuccess = ( 0 == aFileDlg.Execute() );
        if ( bSuccess )
            _out_rNewValue <<= (::rtl::OUString)aFileDlg.GetPath();
        return bSuccess;
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_executeFontDialog_nothrow( Any& _out_rNewValue, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const
    {
        bool bSuccess = false;

        // create an item set for use with the dialog
        SfxItemSet* pSet = NULL;
        SfxItemPool* pPool = NULL;
        SfxPoolItem** pDefaults = NULL;
        ControlCharacterDialog::createItemSet(pSet, pPool, pDefaults);
        ControlCharacterDialog::translatePropertiesToItems(m_xComponent, pSet);

        {   // do this in an own block. The dialog needs to be destroyed before we call
            // destroyItemSet
            ControlCharacterDialog aDlg( impl_getDefaultDialogParent_nothrow(), *pSet );
            _rClearBeforeDialog.clear();
            if ( RET_OK == aDlg.Execute() )
            {
                const SfxItemSet* pOut = aDlg.GetOutputItemSet();
                if ( pOut )
                {
                    Sequence< NamedValue > aFontPropertyValues;
                    ControlCharacterDialog::translateItemsToProperties( *pOut, aFontPropertyValues );
                    _out_rNewValue <<= aFontPropertyValues;
                    bSuccess = true;
                }
            }
        }

        ControlCharacterDialog::destroyItemSet(pSet, pPool, pDefaults);
        return bSuccess;
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_browseForDatabaseDocument_throw( Any& _out_rNewValue, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const
    {
        ::sfx2::FileDialogHelper aFileDlg(WB_3DLOOK);

        ::rtl::OUString sDataSource;
        OSL_VERIFY( impl_getPropertyValue_throw( PROPERTY_DATASOURCE ) >>= sDataSource );
        INetURLObject aParser( sDataSource );
        if ( INET_PROT_FILE == aParser.GetProtocol() )
            // set the initial directory only for file-URLs. Everything else
            // is considered to be potentially expensive
            // 106126 - 2002/12/10 - fs@openoffice.org
            aFileDlg.SetDisplayDirectory( sDataSource );

        const String s_sDatabaseType = String::CreateFromAscii("StarOffice XML (Base)");
        const SfxFilter* pFilter = SfxFilter::GetFilterByName( s_sDatabaseType);
        OSL_ENSURE(pFilter,"Filter: StarOffice XML (Base) could not be found!");
        if ( pFilter )
        {
            aFileDlg.AddFilter(pFilter->GetFilterName(),pFilter->GetDefaultExtension());
        }

        _rClearBeforeDialog.clear();
        bool bSuccess = ( 0 == aFileDlg.Execute() );
        if ( bSuccess )
            _out_rNewValue <<= (::rtl::OUString)aFileDlg.GetPath();
        return bSuccess;
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_dialogColorChooser_throw( sal_Int32 _nColorPropertyId, Any& _out_rNewValue, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const
    {
        sal_Int32 nColor = 0;
        OSL_VERIFY( impl_getPropertyValue_throw( impl_getPropertyNameFromId_nothrow( _nColorPropertyId ) ) >>= nColor );
        ::Color aColor( nColor );
        SvColorDialog aColorDlg( impl_getDefaultDialogParent_nothrow() );
        aColorDlg.SetColor( aColor );

        _rClearBeforeDialog.clear();
        if ( !aColorDlg.Execute() )
            return false;

        aColor = aColorDlg.GetColor();
        nColor = aColor.GetColor();
        _out_rNewValue <<= (sal_Int32)nColor;
        return true;
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_dialogChooseLabelControl_nothrow( Any& _out_rNewValue, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const
    {
        OSelectLabelDialog dlgSelectLabel( impl_getDefaultDialogParent_nothrow(), m_xComponent );
        _rClearBeforeDialog.clear();
        bool bSuccess = ( RET_OK == dlgSelectLabel.Execute() );
        if ( bSuccess )
            _out_rNewValue <<= dlgSelectLabel.GetSelected();
        return bSuccess;
    }

    //------------------------------------------------------------------------
    Reference< XControlContainer > FormComponentPropertyHandler::impl_getContextControlContainer_nothrow() const
    {
        Reference< XControlContainer > xControlContext(
            m_aContext.getContextValueByAsciiName( "ControlContext" ),
            UNO_QUERY );
        return xControlContext;
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_dialogChangeTabOrder_nothrow( ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const
    {
        OSL_PRECOND( impl_getContextControlContainer_nothrow().is(), "FormComponentPropertyHandler::impl_dialogChangeTabOrder_nothrow: invalid control context!" );

        Reference< XTabControllerModel > xTabControllerModel( impl_getRowSet_nothrow(), UNO_QUERY );
        TabOrderDialog aDialog(
            impl_getDefaultDialogParent_nothrow(),
            xTabControllerModel,
            impl_getContextControlContainer_nothrow(),
            m_aContext.getLegacyServiceFactory()
        );
        _rClearBeforeDialog.clear();
        return ( RET_OK == aDialog.Execute() );
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_doDesignSQLCommand_nothrow( const Reference< XObjectInspectorUI >& _rxInspectorUI )
    {
        try
        {
            if ( m_xCommandDesigner.is() )
            {
                if ( m_xCommandDesigner->isActive() )
                {
                    m_xCommandDesigner->raise();
                    return true;
                }
                m_xCommandDesigner->dispose();
                m_xCommandDesigner.set( NULL );
            }

            if ( !impl_ensureRowsetConnection_nothrow() )
                return false;

            Reference< XPropertySet > xRowSetProperties( impl_getRowSet_throw(), UNO_QUERY_THROW );
            m_xCommandDesigner.set( new SQLCommandDesigner( m_aContext.getUNOContext(), xRowSetProperties, m_xRowSetConnection, LINK( this, FormComponentPropertyHandler, OnDesignerClosed ) ) );

            DBG_ASSERT( _rxInspectorUI.is(), "FormComponentPropertyHandler::OnDesignerClosed: no access to the property browser ui!" );
            if ( m_xCommandDesigner->isActive() && _rxInspectorUI.is() )
            {
                m_xBrowserUI = _rxInspectorUI;
                // disable everything which has to do with the form's data source
                m_xBrowserUI->enablePropertyUIElements( PROPERTY_DATASOURCE, PropertyLineElement::All, sal_False );
                m_xBrowserUI->enablePropertyUIElements( PROPERTY_COMMANDTYPE, PropertyLineElement::All, sal_False );
                m_xBrowserUI->enablePropertyUIElements( PROPERTY_COMMAND, PropertyLineElement::All, sal_False );
                m_xBrowserUI->enablePropertyUIElements( PROPERTY_ESCAPE_PROCESSING, PropertyLineElement::All, sal_False );
                // but enable the browse button for Command - so it can be used to raise the query designer
                m_xBrowserUI->enablePropertyUIElements( PROPERTY_COMMAND, PropertyLineElement::PrimaryButton, sal_True );
            }
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "FormComponentPropertyHandler::impl_doDesignSQLCommand_nothrow: caught an exception!" );
        }
        return m_xCommandDesigner.is();
    }

    //------------------------------------------------------------------------
    IMPL_LINK( FormComponentPropertyHandler, OnDesignerClosed, void*, /*NOTINTERESTEDIN*/ )
    {
        OSL_ENSURE( m_xBrowserUI.is(), "FormComponentPropertyHandler::OnDesignerClosed: no access to the property browser ui!" );
        if ( m_xBrowserUI.is() )
        {
            m_xBrowserUI->enablePropertyUI( PROPERTY_DATASOURCE, sal_True );
            m_xBrowserUI->enablePropertyUI( PROPERTY_COMMANDTYPE, sal_True );
            m_xBrowserUI->enablePropertyUI( PROPERTY_COMMAND, sal_True );
            m_xBrowserUI->enablePropertyUI( PROPERTY_ESCAPE_PROCESSING, sal_True );

            m_xBrowserUI->enablePropertyUIElements( PROPERTY_DATASOURCE, PropertyLineElement::All, sal_True );
            m_xBrowserUI->enablePropertyUIElements( PROPERTY_COMMANDTYPE, PropertyLineElement::All, sal_True );
            m_xBrowserUI->enablePropertyUIElements( PROPERTY_COMMAND, PropertyLineElement::All, sal_True );
            m_xBrowserUI->enablePropertyUIElements( PROPERTY_ESCAPE_PROCESSING, PropertyLineElement::All, sal_True );
        }

        return 0L;
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_hasValidDataSourceSignature_nothrow( const Reference< XForm >& _rxForm, bool _bAllowEmptyDataSourceName ) SAL_THROW(())
    {
        bool bHas = false;
        try
        {
            Reference< XPropertySet > xFormProperties( _rxForm, UNO_QUERY_THROW );

            ::rtl::OUString sPropertyValue;
            // first, we need the name of an existent data source
            xFormProperties->getPropertyValue( PROPERTY_DATASOURCE ) >>= sPropertyValue;
            bHas = ( sPropertyValue.getLength() != 0 ) || _bAllowEmptyDataSourceName;

            // then, the command should not be empty
            if ( bHas )
            {
                xFormProperties->getPropertyValue( PROPERTY_COMMAND ) >>= sPropertyValue;
                bHas = ( sPropertyValue.getLength() != 0 );
            }
        }
        catch( const Exception& )
        {
            OSL_ENSURE( false, "FormComponentPropertyHandler::impl_hasValidDataSourceSignature_nothrow: caught an exception!" );
        }
        return bHas;
    }

    //------------------------------------------------------------------------
    bool FormComponentPropertyHandler::impl_isValidDataSourceName_nothrow( const ::rtl::OUString& _rDSName ) const
    {
        bool bHas = false;
        try
        {
            Reference< XNameAccess > xDataSourceNameAccess;
            if ( m_aContext.createComponent( (rtl::OUString)SERVICE_DATABASE_CONTEXT, xDataSourceNameAccess ) )
                bHas = xDataSourceNameAccess->hasByName( _rDSName );
        }
        catch ( const Exception& )
        {
            OSL_ENSURE( sal_False, "FormComponentPropertyHandler::impl_isValidDataSourceName_nothrow: caught an exception!" );
        }
        return bHas;
    }

    //------------------------------------------------------------------------
    ::rtl::OUString FormComponentPropertyHandler::impl_getDocumentURL_nothrow() const
    {
        ::rtl::OUString sURL;
        try
        {
            Reference< XModel > xDocument( impl_getContextDocument_nothrow() );
            if ( xDocument.is() )
                sURL = xDocument->getURL();
        }
        catch( const Exception& e )
        {
        #if OSL_DEBUG_LEVEL > 0
            ::rtl::OString sMessage( "FormComponentPropertyHandler::: caught an exception!\n" );
            sMessage += "message:\n";
            sMessage += ::rtl::OString( e.Message.getStr(), e.Message.getLength(), osl_getThreadTextEncoding() );
            OSL_ENSURE( false, sMessage );
        #else
            e; // make compiler happy
        #endif
        }
        return sURL;
    }

//........................................................................
} // namespace pcr
//........................................................................

