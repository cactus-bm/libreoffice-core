/*************************************************************************
 *
 *  $RCSfile: unocontrolmodel.cxx,v $
 *
 *  $Revision: 1.36 $
 *
 *  last change: $Author: obo $ $Date: 2004-07-05 15:56:58 $
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

#ifndef _COM_SUN_STAR_BEANS_PROPERTYSTATE_HPP_
#include <com/sun/star/beans/PropertyState.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTDESCRIPTOR_HPP_
#include <com/sun/star/awt/FontDescriptor.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTWIDTH_HPP_
#include <com/sun/star/awt/FontWidth.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTWEIGHT_HPP_
#include <com/sun/star/awt/FontWeight.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTSLANT_HPP_
#include <com/sun/star/awt/FontSlant.hpp>
#endif

#ifndef _COM_SUN_STAR_IO_XMARKABLESTREAM_HPP_
#include <com/sun/star/io/XMarkableStream.hpp>
#endif

#ifndef _TOOLKIT_AWT_UNOCONTROLMODEL_HXX_
#include <toolkit/controls/unocontrolmodel.hxx>
#endif
#ifndef _TOOLKIT_HELPER_MACROS_HXX_
#include <toolkit/helper/macros.hxx>
#endif
#ifndef _CPPUHELPER_TYPEPROVIDER_HXX_
#include <cppuhelper/typeprovider.hxx>
#endif
#ifndef _RTL_MEMORY_H_
#include <rtl/memory.h>
#endif
#ifndef _RTL_UUID_H_
#include <rtl/uuid.h>
#endif

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _TOOLS_TABLE_HXX
#include <tools/table.hxx>
#endif
#ifndef _DATE_HXX
#include <tools/date.hxx>
#endif
#ifndef _TOOLS_TIME_HXX
#include <tools/time.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _TOOLKIT_HELPER_PROPERTY_HXX_
#include <toolkit/helper/property.hxx>
#endif
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/helper/vclunohelper.hxx>
#endif
#ifndef _TOOLKIT_HELPER_EMPTYFONTDESCRIPTOR_HXX_
#include <toolkit/helper/emptyfontdescriptor.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif
#ifndef _UNOTOOLS_LOCALEDATAWRAPPER_HXX
#include <unotools/localedatawrapper.hxx>
#endif
#ifndef _UTL_CONFIGMGR_HXX_
#include <unotools/configmgr.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COMPHELPER_SEQUENCE_HXX_
#include <comphelper/sequence.hxx>
#endif

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

#ifndef _UNO_DATA_H_
#include <uno/data.h>
#endif

#include <memory>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::i18n;

struct ImplControlProperty
{
private:
    sal_uInt16                  nId;
    ::com::sun::star::uno::Any  aValue;

public:
    ImplControlProperty( const ImplControlProperty& rProp ) : aValue( rProp.aValue )
    {
        nId = rProp.nId;
    }

    ImplControlProperty( sal_uInt16 nT )
    {
        nId = nT;
    }

    ImplControlProperty( sal_uInt16 nT, const ::com::sun::star::uno::Any& rValue ) : aValue( rValue )
    {
        nId = nT;
    }

    sal_uInt16                          GetId() const                                           { return nId; }
    const ::com::sun::star::uno::Any&   GetValue() const                                        { return aValue; }
    void                                SetValue( const ::com::sun::star::uno::Any& rValue )    { aValue = rValue; }
};

DECLARE_TABLE( ImplPropertyTable, ImplControlProperty* );

#define UNOCONTROL_STREAMVERSION    (short)2

static void lcl_ImplMergeFontProperty( ::com::sun::star::awt::FontDescriptor& rFD, sal_uInt16 nPropId, const ::com::sun::star::uno::Any& rValue )
{
    // some props are defined with other types than the matching FontDescriptor members have
    // (e.g. FontWidth, FontSlant)
    // 78474 - 09/19/2000 - FS
    float       nExtractFloat = 0;
    sal_Int16   nExtractShort = 0;

    switch ( nPropId )
    {
        case BASEPROPERTY_FONTDESCRIPTORPART_NAME:          rValue >>= rFD.Name;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_STYLENAME:     rValue >>= rFD.StyleName;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_FAMILY:        rValue >>= rFD.Family;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_CHARSET:       rValue >>= rFD.CharSet;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_HEIGHT:        rValue >>= nExtractFloat; rFD.Height = (sal_Int16)nExtractFloat;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_WEIGHT:        rValue >>= rFD.Weight;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_SLANT:         if ( rValue >>= nExtractShort )
                                                                rFD.Slant = (::com::sun::star::awt::FontSlant)nExtractShort;
                                                            else
                                                                rValue >>= rFD.Slant;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_UNDERLINE:     rValue >>= rFD.Underline;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_STRIKEOUT:     rValue >>= rFD.Strikeout;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_WIDTH:         rValue >>= rFD.Width;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_PITCH:         rValue >>= rFD.Pitch;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_CHARWIDTH:     rValue >>= rFD.CharacterWidth;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_ORIENTATION:   rValue >>= rFD.Orientation;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_KERNING:       rValue >>= rFD.Kerning;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_WORDLINEMODE:  rValue >>= rFD.WordLineMode;
                                                            break;
        case BASEPROPERTY_FONTDESCRIPTORPART_TYPE:          rValue >>= rFD.Type;
                                                            break;
        default:                                            DBG_ERROR( "FontProperty?!" );
    }
}

//  ----------------------------------------------------
//  class UnoControlModel
//  ----------------------------------------------------
UnoControlModel::UnoControlModel()
    : OPropertySetHelper( BrdcstHelper ), maDisposeListeners( *this )
{
    // Die Properties muessen vom Model in die Tabelle gestopft werden,
    // nur vorhandene Properties sind gueltige Properties, auch wenn VOID.
    mpData = new ImplPropertyTable;
}

UnoControlModel::UnoControlModel( const UnoControlModel& rModel )
    : OPropertySetHelper( BrdcstHelper ), maDisposeListeners( *this )
{
    mpData = new ImplPropertyTable;

    for ( sal_uInt32 n = rModel.mpData->Count(); n; )
    {
        ImplControlProperty* pProp = rModel.mpData->GetObject( --n );
        ImplControlProperty* pNew = new ImplControlProperty( *pProp );
        mpData->Insert( pNew->GetId(), pNew );
    }
}

UnoControlModel::~UnoControlModel()
{
    for ( sal_uInt32 n = mpData->Count(); n; )
        delete mpData->GetObject( --n );
    delete mpData;
}

UnoControlModel* UnoControlModel::Clone() const
{
    DBG_ERROR( "UnoControlModel::Clone() ?!" );
    return NULL;
}

::com::sun::star::uno::Sequence<sal_Int32> UnoControlModel::ImplGetPropertyIds() const
{
    sal_uInt32 nIDs = mpData->Count();
    ::com::sun::star::uno::Sequence<sal_Int32>  aIDs( nIDs );
    sal_Int32* pIDs = aIDs.getArray();
    for ( sal_uInt32 n = 0; n < nIDs; n++ )
        pIDs[n] = mpData->GetObjectKey( n );
    return aIDs;
}

sal_Bool UnoControlModel::ImplHasProperty( sal_uInt16 nPropId ) const
{
    if ( ( nPropId >= BASEPROPERTY_FONTDESCRIPTORPART_START ) && ( nPropId <= BASEPROPERTY_FONTDESCRIPTORPART_END ) )
        nPropId = BASEPROPERTY_FONTDESCRIPTOR;

    return mpData->Get( nPropId ) ? sal_True : sal_False;
}

void UnoControlModel::ImplPropertyChanged( sal_uInt16 nPropId )
{
}

::com::sun::star::uno::Any UnoControlModel::ImplGetDefaultValue( sal_uInt16 nPropId ) const
{
    ::com::sun::star::uno::Any aDefault;

    if ( ( nPropId == BASEPROPERTY_FONTDESCRIPTOR ) ||
         ( nPropId >= BASEPROPERTY_FONTDESCRIPTORPART_START ) && ( nPropId <= BASEPROPERTY_FONTDESCRIPTORPART_END ) )
    {
        EmptyFontDescriptor aFD;
        switch ( nPropId )
        {
            case BASEPROPERTY_FONTDESCRIPTOR:                   aDefault <<= aFD;                   break;
            case BASEPROPERTY_FONTDESCRIPTORPART_NAME:          aDefault <<= aFD.Name;              break;
            case BASEPROPERTY_FONTDESCRIPTORPART_STYLENAME:     aDefault <<= aFD.StyleName;         break;
            case BASEPROPERTY_FONTDESCRIPTORPART_FAMILY:        aDefault <<= aFD.Family;            break;
            case BASEPROPERTY_FONTDESCRIPTORPART_CHARSET:       aDefault <<= aFD.CharSet;           break;
            case BASEPROPERTY_FONTDESCRIPTORPART_HEIGHT:        aDefault <<= (float)aFD.Height;     break;
            case BASEPROPERTY_FONTDESCRIPTORPART_WEIGHT:        aDefault <<= aFD.Weight;            break;
            case BASEPROPERTY_FONTDESCRIPTORPART_SLANT:         aDefault <<= (sal_Int16)aFD.Slant;  break;
            case BASEPROPERTY_FONTDESCRIPTORPART_UNDERLINE:     aDefault <<= aFD.Underline;         break;
            case BASEPROPERTY_FONTDESCRIPTORPART_STRIKEOUT:     aDefault <<= aFD.Strikeout;         break;
            case BASEPROPERTY_FONTDESCRIPTORPART_WIDTH:         aDefault <<= aFD.Width;             break;
            case BASEPROPERTY_FONTDESCRIPTORPART_PITCH:         aDefault <<= aFD.Pitch;             break;
            case BASEPROPERTY_FONTDESCRIPTORPART_CHARWIDTH:     aDefault <<= aFD.CharacterWidth;    break;
            case BASEPROPERTY_FONTDESCRIPTORPART_ORIENTATION:   aDefault <<= aFD.Orientation;       break;
            case BASEPROPERTY_FONTDESCRIPTORPART_KERNING:       aDefault <<= aFD.Kerning;           break;
            case BASEPROPERTY_FONTDESCRIPTORPART_WORDLINEMODE:  aDefault <<= aFD.WordLineMode;      break;
            case BASEPROPERTY_FONTDESCRIPTORPART_TYPE:          aDefault <<= aFD.Type;              break;
            default: DBG_ERROR( "FontProperty?!" );
        }
    }
    else
    {
        switch ( nPropId )
        {
            case BASEPROPERTY_BORDERCOLOR:
            case BASEPROPERTY_SYMBOL_COLOR:
            case BASEPROPERTY_TABSTOP:
            case BASEPROPERTY_TEXTCOLOR:
            case BASEPROPERTY_TEXTLINECOLOR:
            case BASEPROPERTY_DATE:
            case BASEPROPERTY_DATESHOWCENTURY:
            case BASEPROPERTY_TIME:
            case BASEPROPERTY_VALUE_DOUBLE:
            case BASEPROPERTY_PROGRESSVALUE:
            case BASEPROPERTY_SCROLLVALUE:
            case BASEPROPERTY_VISIBLESIZE:
            case BASEPROPERTY_BACKGROUNDCOLOR:
            case BASEPROPERTY_FILLCOLOR:            break;  // Void

            case BASEPROPERTY_FONTRELIEF:
            case BASEPROPERTY_FONTEMPHASISMARK:
            case BASEPROPERTY_MAXTEXTLEN:
            case BASEPROPERTY_STATE:
            case BASEPROPERTY_EXTDATEFORMAT:
            case BASEPROPERTY_EXTTIMEFORMAT:
            case BASEPROPERTY_ECHOCHAR:             aDefault <<= (sal_Int16) 0; break;
            case BASEPROPERTY_BORDER:               aDefault <<= (sal_Int16) 1; break;
            case BASEPROPERTY_DECIMALACCURACY:      aDefault <<= (sal_Int16) 2; break;
            case BASEPROPERTY_LINECOUNT:            aDefault <<= (sal_Int16) 5; break;
            case BASEPROPERTY_ALIGN:                aDefault <<= (sal_Int16) PROPERTY_ALIGN_LEFT; break;
            case BASEPROPERTY_IMAGEALIGN:           aDefault <<= (sal_Int16) 1 /*ImageAlign::TOP*/; break;
            case BASEPROPERTY_IMAGEPOSITION:        aDefault <<= (sal_Int16) 12 /*ImagePosition::Centered*/; break;
            case BASEPROPERTY_PUSHBUTTONTYPE:       aDefault <<= (sal_Int16) 0 /*PushButtonType::STANDARD*/; break;

            case BASEPROPERTY_DATEMAX:              aDefault <<= (sal_Int32) Date( 31, 12, 2200 ).GetDate();    break;
            case BASEPROPERTY_DATEMIN:              aDefault <<= (sal_Int32) Date( 1, 1, 1900 ).GetDate();  break;
            case BASEPROPERTY_TIMEMAX:              aDefault <<= (sal_Int32) Time( 23, 59 ).GetTime();  break;
            case BASEPROPERTY_TIMEMIN:              aDefault <<= (sal_Int32) 0;     break;
            case BASEPROPERTY_VALUEMAX_DOUBLE:      aDefault <<= (double) 1000000;  break;
            case BASEPROPERTY_VALUEMIN_DOUBLE:      aDefault <<= (double) -1000000; break;
            case BASEPROPERTY_VALUESTEP_DOUBLE:     aDefault <<= (double ) 1;       break;
            case BASEPROPERTY_PROGRESSVALUE_MAX:    aDefault <<= (sal_Int32) 100;   break;
            case BASEPROPERTY_PROGRESSVALUE_MIN:    aDefault <<= (sal_Int32)   0;   break;
            case BASEPROPERTY_SCROLLVALUE_MAX:      aDefault <<= (sal_Int32) 100;   break;
            case BASEPROPERTY_SCROLLVALUE_MIN:      aDefault <<= (sal_Int32)   0;   break;
            case BASEPROPERTY_LINEINCREMENT:        aDefault <<= (sal_Int32)   1;   break;
            case BASEPROPERTY_BLOCKINCREMENT:       aDefault <<= (sal_Int32)  10;   break;
            case BASEPROPERTY_ORIENTATION:          aDefault <<= (sal_Int32)   0;   break;
            case BASEPROPERTY_SPINVALUE:            aDefault <<= (sal_Int32)   0;   break;
            case BASEPROPERTY_SPININCREMENT:        aDefault <<= (sal_Int32)   1;   break;
            case BASEPROPERTY_SPINVALUE_MIN:        aDefault <<= (sal_Int32)   0;   break;
            case BASEPROPERTY_SPINVALUE_MAX:        aDefault <<= (sal_Int32) 100;   break;
            case BASEPROPERTY_REPEAT_DELAY:         aDefault <<= (sal_Int32)  50;   break;    // 50 milliseconds
            case BASEPROPERTY_DEFAULTCONTROL:       aDefault <<= ((UnoControlModel*)this)->getServiceName();    break;


            case BASEPROPERTY_MOVEABLE:
            case BASEPROPERTY_CLOSEABLE:
            case BASEPROPERTY_SIZEABLE:
            case BASEPROPERTY_HSCROLL:
            case BASEPROPERTY_DEFAULTBUTTON:
            case BASEPROPERTY_MULTILINE:
            case BASEPROPERTY_MULTISELECTION:
            case BASEPROPERTY_TRISTATE:
            case BASEPROPERTY_DROPDOWN:
            case BASEPROPERTY_SPIN:
            case BASEPROPERTY_READONLY:
            case BASEPROPERTY_VSCROLL:
            case BASEPROPERTY_NUMSHOWTHOUSANDSEP:
            case BASEPROPERTY_STRICTFORMAT:
            case BASEPROPERTY_REPEAT:
            case BASEPROPERTY_HARDLINEBREAKS:       aDefault <<= (sal_Bool) sal_False; break;

            case BASEPROPERTY_HIDEINACTIVESELECTION:
            case BASEPROPERTY_ENFORCE_FORMAT:
            case BASEPROPERTY_AUTOCOMPLETE:
            case BASEPROPERTY_SCALEIMAGE:
            case BASEPROPERTY_ENABLED:
            case BASEPROPERTY_PRINTABLE:            aDefault <<= (sal_Bool) sal_True;   break;

            case BASEPROPERTY_HELPTEXT:
            case BASEPROPERTY_HELPURL:
            case BASEPROPERTY_IMAGEURL:
            case BASEPROPERTY_EDITMASK:
            case BASEPROPERTY_LITERALMASK:
            case BASEPROPERTY_LABEL:
            case BASEPROPERTY_TITLE:
            case BASEPROPERTY_TEXT:                 aDefault <<= ::rtl::OUString(); break;

            case BASEPROPERTY_STRINGITEMLIST:
            {
                ::com::sun::star::uno::Sequence< ::rtl::OUString> aStringSeq;
                aDefault <<= aStringSeq;

            }
            break;
            case BASEPROPERTY_SELECTEDITEMS:
            {
                ::com::sun::star::uno::Sequence<sal_Int16> aINT16Seq;
                aDefault <<= aINT16Seq;
            }
            break;
            case BASEPROPERTY_CURRENCYSYMBOL:
            {
                Any aDefaultCurrency = ::utl::ConfigManager::GetDirectConfigProperty(::utl::ConfigManager::DEFAULTCURRENCY);
                DBG_ASSERT( TypeClass_STRING == aDefaultCurrency.getValueTypeClass(), "UnoControlModel::ImplGetDefaultValue: invalid currency config value!" );

                ::rtl::OUString sDefaultCurrency;
                aDefaultCurrency >>= sDefaultCurrency;

                // extract the bank symbol
                sal_Int32 nSepPos = sDefaultCurrency.indexOf( '-' );
                ::rtl::OUString sBankSymbol;
                if ( nSepPos >= 0 )
                {
                    sBankSymbol = sDefaultCurrency.copy( 0, nSepPos );
                    sDefaultCurrency = sDefaultCurrency.copy( nSepPos + 1 );
                }

                // the remaming is the locale
                Locale aLocale;
                nSepPos = sDefaultCurrency.indexOf( '-' );
                if ( nSepPos >= 0 )
                {
                    aLocale.Language = sDefaultCurrency.copy( 0, nSepPos );
                    aLocale.Country = sDefaultCurrency.copy( nSepPos + 1 );
                }

                LocaleDataWrapper aLocaleInfo( ::comphelper::getProcessServiceFactory(), aLocale );
                if ( !sBankSymbol.getLength() )
                    sBankSymbol = aLocaleInfo.getCurrBankSymbol();

                // look for the currency entry (for this language) which has the given bank symbol
                Sequence< Currency > aAllCurrencies = aLocaleInfo.getAllCurrencies();
                const Currency* pAllCurrencies      =                       aAllCurrencies.getConstArray();
                const Currency* pAllCurrenciesEnd   =   pAllCurrencies  +   aAllCurrencies.getLength();

                ::rtl::OUString sCurrencySymbol = aLocaleInfo.getCurrSymbol();
                if ( !sBankSymbol.getLength() )
                {
                    DBG_ASSERT( pAllCurrencies != pAllCurrenciesEnd, "UnoControlModel::ImplGetDefaultValue: no currencies at all!" );
                    if ( pAllCurrencies != pAllCurrenciesEnd )
                    {
                        sBankSymbol = pAllCurrencies->BankSymbol;
                        sCurrencySymbol = pAllCurrencies->Symbol;
                    }
                }

                if ( sBankSymbol.getLength() )
                {
                    for ( ;pAllCurrencies != pAllCurrenciesEnd; ++pAllCurrencies )
                        if ( pAllCurrencies->BankSymbol == sBankSymbol )
                        {
                            sCurrencySymbol = pAllCurrencies->Symbol;
                            break;
                        }
                    DBG_ASSERT( pAllCurrencies != pAllCurrenciesEnd, "UnoControlModel::ImplGetDefaultValue: did not find the given bank symbol!" );
                }

                aDefault <<= sCurrencySymbol;
            }
            break;

            default:    DBG_ERROR( "ImplGetDefaultValue - unknown Property" );
        }
    }

    return aDefault;
}

void UnoControlModel::ImplRegisterProperty( sal_uInt16 nPropId, const ::com::sun::star::uno::Any& rDefault )
{
    ImplControlProperty* pProp = new ImplControlProperty( nPropId, rDefault );
    mpData->Insert( nPropId, pProp );
}

void UnoControlModel::ImplRegisterProperty( sal_uInt16 nPropId )
{
    ImplRegisterProperty( nPropId, ImplGetDefaultValue( nPropId ) );

    if ( nPropId == BASEPROPERTY_FONTDESCRIPTOR )
    {
        // some properties are not included in the FontDescriptor, but everytime
        // when we have a FontDescriptor we want to have these properties too.
        // => Easier to register the here, istead everywhere where I register the FontDescriptor...

        ImplRegisterProperty( BASEPROPERTY_TEXTCOLOR );
        ImplRegisterProperty( BASEPROPERTY_TEXTLINECOLOR );
        ImplRegisterProperty( BASEPROPERTY_FONTRELIEF );
        ImplRegisterProperty( BASEPROPERTY_FONTEMPHASISMARK );
    }
}

// ::com::sun::star::uno::XInterface
::com::sun::star::uno::Any UnoControlModel::queryAggregation( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException)
{
    ::com::sun::star::uno::Any aRet = ::cppu::queryInterface( rType,
                                        SAL_STATIC_CAST( ::com::sun::star::awt::XControlModel*, this ),
                                        SAL_STATIC_CAST( ::com::sun::star::io::XPersistObject*, this ),
                                        SAL_STATIC_CAST( ::com::sun::star::lang::XComponent*, this ),
                                        SAL_STATIC_CAST( ::com::sun::star::lang::XServiceInfo*, this ),
                                        SAL_STATIC_CAST( ::com::sun::star::util::XCloneable*, this ),
                                        SAL_STATIC_CAST( ::com::sun::star::beans::XPropertyState*, this ),
                                        SAL_STATIC_CAST( ::com::sun::star::beans::XMultiPropertySet*, this ),
                                        SAL_STATIC_CAST( ::com::sun::star::beans::XFastPropertySet*, this ),
                                        SAL_STATIC_CAST( ::com::sun::star::beans::XPropertySet*, this ),
                                        SAL_STATIC_CAST( ::com::sun::star::lang::XTypeProvider*, this ),
                                        SAL_STATIC_CAST( ::com::sun::star::lang::XUnoTunnel*, this ) );
    return (aRet.hasValue() ? aRet : OWeakAggObject::queryAggregation( rType ));
}

// ::com::sun::star::lang::XUnoTunnel
IMPL_XUNOTUNNEL( UnoControlModel )

// ::com::sun::star::lang::XTypeProvider
IMPL_XTYPEPROVIDER_START( UnoControlModel )
    getCppuType( ( ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel>* ) NULL ),
    getCppuType( ( ::com::sun::star::uno::Reference< ::com::sun::star::io::XPersistObject>* ) NULL ),
    getCppuType( ( ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent>* ) NULL ),
    getCppuType( ( ::com::sun::star::uno::Reference< ::com::sun::star::lang::XServiceInfo>* ) NULL ),
    getCppuType( ( ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloneable>* ) NULL ),
    getCppuType( ( ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyState>* ) NULL ),
    getCppuType( ( ::com::sun::star::uno::Reference< ::com::sun::star::beans::XMultiPropertySet>* ) NULL ),
    getCppuType( ( ::com::sun::star::uno::Reference< ::com::sun::star::beans::XFastPropertySet>* ) NULL ),
    getCppuType( ( ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>* ) NULL )
IMPL_XTYPEPROVIDER_END


uno::Reference< util::XCloneable > UnoControlModel::createClone() throw(::com::sun::star::uno::RuntimeException)
{
    UnoControlModel* pClone = Clone();
    uno::Reference< util::XCloneable > xClone( (::cppu::OWeakObject*) pClone, uno::UNO_QUERY );
    return xClone;
}

// ::com::sun::star::lang::XComponent
void UnoControlModel::dispose(  ) throw(::com::sun::star::uno::RuntimeException)
{
    ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

    ::com::sun::star::lang::EventObject aEvt;
    aEvt.Source = (::com::sun::star::uno::XAggregation*)(::cppu::OWeakAggObject*)this;
    maDisposeListeners.disposeAndClear( aEvt );

    // let the property set helper notify our property listeners
    OPropertySetHelper::disposing();
}

void UnoControlModel::addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& rxListener ) throw(::com::sun::star::uno::RuntimeException)
{
    ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

    maDisposeListeners.addInterface( rxListener );
}

void UnoControlModel::removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& rxListener ) throw(::com::sun::star::uno::RuntimeException)
{
    ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

    maDisposeListeners.removeInterface( rxListener );
}


// ::com::sun::star::beans::XPropertyState
::com::sun::star::beans::PropertyState UnoControlModel::getPropertyState( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException)
{
    ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

    sal_uInt16 nPropId = GetPropertyId( PropertyName );

    ::com::sun::star::uno::Any aValue = getPropertyValue( PropertyName );
    ::com::sun::star::uno::Any aDefault = ImplGetDefaultValue( nPropId );

    return CompareProperties( aValue, aDefault ) ? ::com::sun::star::beans::PropertyState_DEFAULT_VALUE : ::com::sun::star::beans::PropertyState_DIRECT_VALUE;
}

::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > UnoControlModel::getPropertyStates( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& PropertyNames ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException)
{
    ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

    sal_uInt32 nNames = PropertyNames.getLength();
    const ::rtl::OUString* pNames = PropertyNames.getConstArray();

    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > aStates( nNames );
    ::com::sun::star::beans::PropertyState* pStates = aStates.getArray();

    for ( sal_uInt32 n = 0; n < nNames; n++ )
        pStates[n] = getPropertyState( pNames[n] );

    return aStates;
}

void UnoControlModel::setPropertyToDefault( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException)
{
    ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

    setPropertyValue( PropertyName, ImplGetDefaultValue( GetPropertyId( PropertyName ) ) );
}

::com::sun::star::uno::Any UnoControlModel::getPropertyDefault( const ::rtl::OUString& rPropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

    return ImplGetDefaultValue( GetPropertyId( rPropertyName ) );
}


// ::com::sun::star::io::XPersistObjec
::rtl::OUString UnoControlModel::getServiceName(  ) throw(::com::sun::star::uno::RuntimeException)
{
    ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

    DBG_ERROR( "ServiceName von UnoControlModel ?!" );
    return ::rtl::OUString();
}

void UnoControlModel::write( const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream >& OutStream ) throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException)
{
    ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

    ::com::sun::star::uno::Reference< ::com::sun::star::io::XMarkableStream > xMark( OutStream, ::com::sun::star::uno::UNO_QUERY );
    DBG_ASSERT( xMark.is(), "write: no ::com::sun::star::io::XMarkableStream!" );

    OutStream->writeShort( UNOCONTROL_STREAMVERSION );

    ImplPropertyTable aProps;
    sal_uInt32 n;
    for ( n = mpData->Count(); n; )
    {
        ImplControlProperty* pProp = mpData->GetObject( --n );
        if ( ( ( GetPropertyAttribs( pProp->GetId() ) & ::com::sun::star::beans::PropertyAttribute::TRANSIENT ) == 0 )
            && ( getPropertyState( GetPropertyName( pProp->GetId() ) ) != ::com::sun::star::beans::PropertyState_DEFAULT_VALUE ) )
        {
            aProps.Insert( pProp->GetId(), pProp );
        }
    }

    sal_uInt32 nProps = aProps.Count();

    // FontProperty wegen fehlender Unterscheidung zwischen 5.0 / 5.1
    // immer im alten Format mitspeichern.
    OutStream->writeLong( (long) aProps.IsKeyValid( BASEPROPERTY_FONTDESCRIPTOR ) ? ( nProps + 3 ) : nProps );
    for ( n = 0; n < nProps; n++ )
    {
        sal_Int32 nPropDataBeginMark = xMark->createMark();
        OutStream->writeLong( 0L ); // DataLen

        ImplControlProperty* pProp = aProps.GetObject( n );
        OutStream->writeShort( pProp->GetId() );

        sal_Bool bVoid = pProp->GetValue().getValueType().getTypeClass() == ::com::sun::star::uno::TypeClass_VOID;

        OutStream->writeBoolean( bVoid );

        if ( !bVoid )
        {
            const ::com::sun::star::uno::Any& rValue = pProp->GetValue();
            const ::com::sun::star::uno::Type& rType = rValue.getValueType();

            if ( rType == ::getBooleanCppuType() )
            {
                sal_Bool b;
                rValue >>= b;
                OutStream->writeBoolean( b );
            }
            else if ( rType == ::getCppuType((const ::rtl::OUString*)0) )
            {
                // #HACK 52041# Pfade relativ speichern
                ::rtl::OUString aUString;
                rValue >>= aUString;
                if ( pProp->GetId() == BASEPROPERTY_IMAGEURL )
                    aUString= INetURLObject::AbsToRel( aUString );
                OutStream->writeUTF( aUString );
            }
            else if ( rType == ::getCppuType((const sal_uInt16*)0) )
            {
                sal_uInt16 n;
                rValue >>= n;
                OutStream->writeShort( n );
            }
            else if ( rType == ::getCppuType((const sal_Int16*)0) )
            {
                sal_Int16 n;
                rValue >>= n;
                OutStream->writeShort( n );
            }
            else if ( rType == ::getCppuType((const sal_uInt32*)0) )
            {
                sal_uInt32 n;
                rValue >>= n;
                OutStream->writeLong( n );
            }
            else if ( rType == ::getCppuType((const sal_Int32*)0) )
            {
                sal_Int32 n;
                rValue >>= n;
                OutStream->writeLong( n );
            }
            else if ( rType == ::getCppuType((const double*)0) )
            {
                double n;
                rValue >>= n;
                OutStream->writeDouble( n );
            }
            else if ( rType == ::getCppuType((const ::com::sun::star::awt::FontDescriptor*)0) )
            {
                ::com::sun::star::awt::FontDescriptor aFD;
                rValue >>= aFD;
                OutStream->writeUTF( aFD.Name );
                OutStream->writeShort( aFD.Height );
                OutStream->writeShort( aFD.Width );
                OutStream->writeUTF( aFD.StyleName );
                OutStream->writeShort( aFD.Family );
                OutStream->writeShort( aFD.CharSet );
                OutStream->writeShort( aFD.Pitch );
                OutStream->writeDouble( aFD.CharacterWidth );
                OutStream->writeDouble( aFD.Weight );
                OutStream->writeShort( aFD.Slant );
                OutStream->writeShort( aFD.Underline );
                OutStream->writeShort( aFD.Strikeout );
                OutStream->writeDouble( aFD.Orientation );
                OutStream->writeBoolean( aFD.Kerning );
                OutStream->writeBoolean( aFD.WordLineMode );
                OutStream->writeShort( aFD.Type );
            }
            else if ( rType == ::getCppuType((const ::com::sun::star::uno::Sequence< ::rtl::OUString>*)0 ) )
            {
                ::com::sun::star::uno::Sequence< ::rtl::OUString> aSeq;
                rValue >>= aSeq;
                long nEntries = aSeq.getLength();
                OutStream->writeLong( nEntries );
                for ( long n = 0; n < nEntries; n++ )
                    OutStream->writeUTF( aSeq.getConstArray()[n] );
            }
            else if ( rType == ::getCppuType((const ::com::sun::star::uno::Sequence<sal_uInt16>*)0 ) )
            {
                ::com::sun::star::uno::Sequence<sal_uInt16> aSeq;
                rValue >>= aSeq;
                long nEntries = aSeq.getLength();
                OutStream->writeLong( nEntries );
                for ( long n = 0; n < nEntries; n++ )
                    OutStream->writeShort( aSeq.getConstArray()[n] );
            }
            else if ( rType == ::getCppuType((const ::com::sun::star::uno::Sequence<sal_Int16>*)0 ) )
            {
                ::com::sun::star::uno::Sequence<sal_Int16> aSeq;
                rValue >>= aSeq;
                long nEntries = aSeq.getLength();
                OutStream->writeLong( nEntries );
                for ( long n = 0; n < nEntries; n++ )
                    OutStream->writeShort( aSeq.getConstArray()[n] );
            }
            else
            {
                DBG_ERROR( "write: unknown Property!" );
            }
        }

        sal_Int32 nPropDataLen = xMark->offsetToMark( nPropDataBeginMark );
        xMark->jumpToMark( nPropDataBeginMark );
        OutStream->writeLong( nPropDataLen );
        xMark->jumpToFurthest();
        xMark->deleteMark(nPropDataBeginMark);
    }

    ImplControlProperty* pProp = aProps.Get( BASEPROPERTY_FONTDESCRIPTOR );
    if ( pProp )
    {
        // Solange wir keinen 5.0-Export haben, muss das alte
        // Format mit rausgeschrieben werden...
        ::com::sun::star::awt::FontDescriptor aFD;
        pProp->GetValue() >>= aFD;

        for ( sal_uInt16 n = BASEPROPERTY_FONT_TYPE; n <= BASEPROPERTY_FONT_ATTRIBS; n++ )
        {
            sal_Int32 nPropDataBeginMark = xMark->createMark();
            OutStream->writeLong( 0L ); // DataLen
            OutStream->writeShort( n ); // PropId
            OutStream->writeBoolean( sal_False );   // Void

            if ( n == BASEPROPERTY_FONT_TYPE )
            {
                OutStream->writeUTF( aFD.Name );
                OutStream->writeUTF( aFD.StyleName );
                OutStream->writeShort( aFD.Family );
                OutStream->writeShort( aFD.CharSet );
                OutStream->writeShort( aFD.Pitch );
            }
            else if ( n == BASEPROPERTY_FONT_SIZE )
            {
                OutStream->writeLong( aFD.Width );
                OutStream->writeLong( aFD.Height );
                OutStream->writeShort( VCLUnoHelper::ConvertFontWidth( aFD.CharacterWidth ) );
            }
            else if ( n == BASEPROPERTY_FONT_ATTRIBS )
            {
                OutStream->writeShort( VCLUnoHelper::ConvertFontWeight( aFD.Weight ) );
                OutStream->writeShort( aFD.Slant );
                OutStream->writeShort( aFD.Underline );
                OutStream->writeShort( aFD.Strikeout );
                OutStream->writeShort( (short)(aFD.Orientation * 10) );
                OutStream->writeBoolean( aFD.Kerning );
                OutStream->writeBoolean( aFD.WordLineMode );
            }
            else
            {
                DBG_ERROR( "Property?!" );
            }

            sal_Int32 nPropDataLen = xMark->offsetToMark( nPropDataBeginMark );
            xMark->jumpToMark( nPropDataBeginMark );
            OutStream->writeLong( nPropDataLen );
            xMark->jumpToFurthest();
            xMark->deleteMark(nPropDataBeginMark);
        }
    }
}

void UnoControlModel::read( const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream >& InStream ) throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException)
{
    ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

    ::com::sun::star::uno::Reference< ::com::sun::star::io::XMarkableStream > xMark( InStream, ::com::sun::star::uno::UNO_QUERY );
    DBG_ASSERT( xMark.is(), "read: no ::com::sun::star::io::XMarkableStream!" );

    short nVersion = InStream->readShort();
    sal_uInt32 nProps = (sal_uInt32)InStream->readLong();
    ::com::sun::star::uno::Sequence< ::rtl::OUString> aProps( nProps );
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any> aValues( nProps );
    sal_Bool bInvalidEntries = sal_False;

    // Dummerweise kein Mark fuer den gesamten Block, es koennen also
    // nur Properties geaendert werden, es koennen aber nicht spaeter mal Daten
    // fuer das Model hinter den Properties geschrieben werden.

    // Fuer den Import der alten ::com::sun::star::awt::FontDescriptor-Teile
    ::com::sun::star::awt::FontDescriptor* pFD = NULL;

    sal_uInt32 n;
    for ( n = 0; n < nProps; n++ )
    {
        sal_Int32 nPropDataBeginMark = xMark->createMark();
        sal_Int32 nPropDataLen = InStream->readLong();

        sal_uInt16 nPropId = (sal_uInt16)InStream->readShort();

        ::com::sun::star::uno::Any aValue;
        sal_Bool bIsVoid = InStream->readBoolean();
        if ( !bIsVoid )
        {
            const ::com::sun::star::uno::Type* pType = mpData->Get( nPropId ) ? GetPropertyType( nPropId ) : NULL;
            if ( pType )
            {
                if ( *pType == ::getBooleanCppuType() )
                {
                    sal_Bool b = InStream->readBoolean();
                    aValue <<= b;
                }
                else if ( *pType == ::getCppuType((const ::rtl::OUString*)0) )
                {
                    ::rtl::OUString aUTF = InStream->readUTF();
                    // #HACK 52041# Pfade relativ speichern
                    if ( nPropId == BASEPROPERTY_IMAGEURL )
                        aUTF = INetURLObject::RelToAbs( aUTF );
                    aValue <<= aUTF;
                }
                else if ( *pType == ::getCppuType((const sal_uInt16*)0) )
                {
                    sal_uInt16 n = InStream->readShort();
                    aValue <<= n;
                }
                else if ( *pType == ::getCppuType((const sal_Int16*)0) )
                {
                    sal_Int16 n = InStream->readShort();
                    aValue <<= n;
                }
                else if ( *pType == ::getCppuType((const sal_uInt32*)0) )
                {
                    sal_uInt32 n = InStream->readLong();
                    aValue <<= n;
                }
                else if ( *pType == ::getCppuType((const sal_Int32*)0) )
                {
                    sal_Int32 n = InStream->readLong();
                    aValue <<= n;
                }
                else if ( *pType == ::getCppuType((const double*)0) )
                {
                    double n = InStream->readDouble();
                    aValue <<= n;
                }
                else if ( *pType == ::getCppuType((const ::com::sun::star::awt::FontDescriptor*)0) )
                {
                    ::com::sun::star::awt::FontDescriptor aFD;
                    aFD.Name = InStream->readUTF();
                    aFD.Height = InStream->readShort();
                    aFD.Width = InStream->readShort();
                    aFD.StyleName = InStream->readUTF();
                    aFD.Family = InStream->readShort();
                    aFD.CharSet = InStream->readShort();
                    aFD.Pitch = InStream->readShort();
                    aFD.CharacterWidth = (float)InStream->readDouble();
                    aFD.Weight = (float)InStream->readDouble();
                    aFD.Slant =  (::com::sun::star::awt::FontSlant)InStream->readShort();
                    aFD.Underline = InStream->readShort();
                    aFD.Strikeout = InStream->readShort();
                    aFD.Orientation = (float)InStream->readDouble();
                    aFD.Kerning = InStream->readBoolean();
                    aFD.WordLineMode = InStream->readBoolean();
                    aFD.Type = InStream->readShort();
                    aValue <<= aFD;
                }
                else if ( *pType == ::getCppuType((const ::com::sun::star::uno::Sequence< ::rtl::OUString>*)0 ) )
                {
                    long nEntries = InStream->readLong();
                    ::com::sun::star::uno::Sequence< ::rtl::OUString> aSeq( nEntries );
                    for ( long n = 0; n < nEntries; n++ )
                        aSeq.getArray()[n] = InStream->readUTF();
                    aValue <<= aSeq;

                }
                else if ( *pType == ::getCppuType((const ::com::sun::star::uno::Sequence<sal_uInt16>*)0 ) )

                {
                    long nEntries = InStream->readLong();
                    ::com::sun::star::uno::Sequence<sal_uInt16> aSeq( nEntries );
                    for ( long n = 0; n < nEntries; n++ )
                        aSeq.getArray()[n] = (sal_uInt16)InStream->readShort();
                    aValue <<= aSeq;
                }
                else if ( *pType == ::getCppuType((const ::com::sun::star::uno::Sequence<sal_Int16>*)0 ) )
                {
                    long nEntries = InStream->readLong();
                    ::com::sun::star::uno::Sequence<sal_Int16> aSeq( nEntries );
                    for ( long n = 0; n < nEntries; n++ )
                        aSeq.getArray()[n] = (sal_Int16)InStream->readShort();
                    aValue <<= aSeq;
                }
            }
            else
            {
                // Altes Geraffel aus 5.0
                if ( nPropId == BASEPROPERTY_FONT_TYPE )
                {
                    // Sonst ist es nur die redundante Info fuer alte Versionen
                    // Daten werden durch MarkableStream geskippt.
                    if ( nVersion < 2 )
                    {
                        if ( !pFD )
                        {
                            pFD = new ::com::sun::star::awt::FontDescriptor;
                            ImplControlProperty* pProp = mpData->Get( BASEPROPERTY_FONTDESCRIPTOR );
                            if ( pProp ) // wegen den Defaults...
                            {
                                pFD = new ::com::sun::star::awt::FontDescriptor;
                                pProp->GetValue() >>= *pFD;
                            }
                        }
                        pFD->Name = InStream->readUTF();
                        pFD->StyleName = InStream->readUTF();
                        pFD->Family = InStream->readShort();
                        pFD->CharSet = InStream->readShort();
                        pFD->Pitch = InStream->readShort();
                    }
                }
                else if ( nPropId == BASEPROPERTY_FONT_SIZE )
                {
                    if ( nVersion < 2 )
                    {
                        if ( !pFD )
                        {
                            pFD = new ::com::sun::star::awt::FontDescriptor;
                            ImplControlProperty* pProp = mpData->Get( BASEPROPERTY_FONTDESCRIPTOR );
                            if ( pProp ) // wegen den Defaults...
                                pProp->GetValue() >>= *pFD;
                        }
                        pFD->Width = (sal_Int16)InStream->readLong();
                        pFD->Height = (sal_Int16)InStream->readLong();
                        InStream->readShort();  // ::com::sun::star::awt::FontWidth ignorieren - wurde mal falsch geschrieben und wird nicht gebraucht.
                        pFD->CharacterWidth = ::com::sun::star::awt::FontWidth::DONTKNOW;
                    }
                }
                else if ( nPropId == BASEPROPERTY_FONT_ATTRIBS )
                {
                    if ( nVersion < 2 )
                    {
                         if ( !pFD )
                        {
                            pFD = new ::com::sun::star::awt::FontDescriptor;
                            ImplControlProperty* pProp = mpData->Get( BASEPROPERTY_FONTDESCRIPTOR );
                            if ( pProp ) // wegen den Defaults...
                                pProp->GetValue() >>= *pFD;
                        }
                        pFD->Weight = VCLUnoHelper::ConvertFontWeight( (FontWeight) InStream->readShort() );
                        pFD->Slant =  (::com::sun::star::awt::FontSlant)InStream->readShort();
                        pFD->Underline = InStream->readShort();
                        pFD->Strikeout = InStream->readShort();
                        pFD->Orientation = ( (float)(double)InStream->readShort() ) / 10;
                        pFD->Kerning = InStream->readBoolean();
                        pFD->WordLineMode = InStream->readBoolean();
                    }
                }
                else
                {
                    DBG_ERROR( "read: unknown Property!" );
                }
            }
        }
        else // bVoid
        {
            if ( nPropId == BASEPROPERTY_FONTDESCRIPTOR )
            {
                EmptyFontDescriptor aFD;
                aValue <<= aFD;
            }
        }

        if ( mpData->Get( nPropId ) )
        {
            aProps.getArray()[n] = GetPropertyName( nPropId );
            aValues.getArray()[n] = aValue;
        }
        else
        {
            bInvalidEntries = sal_True;
        }

        // Falls bereits mehr drinsteht als diese Version kennt:
        xMark->jumpToMark( nPropDataBeginMark );
        InStream->skipBytes( nPropDataLen );
        xMark->deleteMark(nPropDataBeginMark);
    }
    if ( bInvalidEntries )
    {
        for ( n = 0; n < (sal_uInt32)aProps.getLength(); n++ )
        {
            if ( !aProps.getConstArray()[n].getLength() )
            {
                ::comphelper::removeElementAt( aProps, n );
                ::comphelper::removeElementAt( aValues, n );
                n--;
            }
        }
    }

    setPropertyValues( aProps, aValues );

    if ( pFD )
    {
        ::com::sun::star::uno::Any aValue;
        aValue <<= *pFD;
        setPropertyValue( GetPropertyName( BASEPROPERTY_FONTDESCRIPTOR ), aValue );
        delete pFD;
    }
}


// ::com::sun::star::lang::XServiceInfo
::rtl::OUString UnoControlModel::getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException)
{
    DBG_ERROR( "This method should be overloaded!" );
    return ::rtl::OUString();

}

sal_Bool UnoControlModel::supportsService( const ::rtl::OUString& rServiceName ) throw(::com::sun::star::uno::RuntimeException)
{
    ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

    ::com::sun::star::uno::Sequence< ::rtl::OUString > aSNL = getSupportedServiceNames();
    const ::rtl::OUString * pArray = aSNL.getConstArray();
    for( sal_Int32 i = 0; i < aSNL.getLength(); i++ )
        if( pArray[i] == rServiceName )
            return sal_True;
    return sal_False;
}

::com::sun::star::uno::Sequence< ::rtl::OUString > UnoControlModel::getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException)
{
    ::rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.UnoControlModel" ) );
    return Sequence< ::rtl::OUString >( &sName, 1 );
}

// ::cppu::OPropertySetHelper
::cppu::IPropertyArrayHelper& UnoControlModel::getInfoHelper()
{
    DBG_ERROR( "UnoControlModel::getInfoHelper() not possible!" );
    return *(::cppu::IPropertyArrayHelper*) NULL;
}

// ------------------------------------------------------------------
template <class TYPE>
sal_Bool convertType(Any& _rConvertedValue, const Any& _rNewValueTest, const TYPE* /* _pTypeDisambiguation */)
{
    TYPE tValue;
    if (_rNewValueTest >>= tValue)
    {
        _rConvertedValue <<= tValue;
        return sal_True;
    }
}

// ..................................................................
sal_Bool UnoControlModel::convertFastPropertyValue( Any & rConvertedValue, Any & rOldValue, sal_Int32 nPropId, const Any& rValue ) throw (IllegalArgumentException)
{
    ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

    sal_Bool bVoid = rValue.getValueType().getTypeClass() == ::com::sun::star::uno::TypeClass_VOID;
    if ( bVoid )
    {
        rConvertedValue.clear();
    }
    else
    {
        const ::com::sun::star::uno::Type* pDestType = GetPropertyType( (sal_uInt16)nPropId );
        if ( pDestType->getTypeClass() == TypeClass_ANY )
        {
            rConvertedValue = rValue;
        }
        else
        {
//          rConvertedValue.setValue( rValue.getValue(), *pDestType );
                // VERY BAD. No check if is made if the value type of rValue and pDestType are compatible in any way.
                // This will crash as soon as somebody tries to set a property value (rValue) which's type
                // is far enough from the destination type which it is 'c-style cast' to with the line above.


            if ( pDestType->equals( rValue.getValueType() ) )
            {
                rConvertedValue = rValue;
            }
            else
            {
                BOOL bConverted = FALSE;
                // 13.03.2001 - 84923 - frank.schoenheit@germany.sun.com

                switch (pDestType->getTypeClass())
                {
                    case TypeClass_DOUBLE:
                    {
                        // try as double
                        double nAsDouble = 0;
                        if ( bConverted = ( rValue >>= nAsDouble ) )
                            rConvertedValue <<= nAsDouble;
                        else
                        {   // try as integer - 96136 - 2002-10-08 - fs@openoffice.org
                            sal_Int32 nAsInteger = 0;
                            if ( bConverted = ( rValue >>= nAsInteger ) )
                                rConvertedValue <<= (double)nAsInteger;
                        }
                    }
                    break;
                    case TypeClass_SHORT:
                    {
                        sal_Int16 n;
                        if ( bConverted = ( rValue >>= n ) )
                            rConvertedValue <<= n;
                    }
                    break;
                    case TypeClass_UNSIGNED_SHORT:
                    {
                        sal_uInt16 n;
                        if ( bConverted = ( rValue >>= n ) )
                            rConvertedValue <<= n;
                    }
                    break;
                    case TypeClass_LONG:
                    {
                        sal_Int32 n;
                        if ( bConverted = ( rValue >>= n ) )
                            rConvertedValue <<= n;
                    }
                    break;
                    case TypeClass_UNSIGNED_LONG:
                    {
                        sal_uInt32 n;
                        if ( bConverted = ( rValue >>= n ) )
                            rConvertedValue <<= n;
                    }
                    break;
                    case TypeClass_INTERFACE:
                    {
                        if ( rValue.getValueType().getTypeClass() == TypeClass_INTERFACE )
                        {
                            Reference< XInterface > xPure;
                            if ( ( rValue >>= xPure ) && xPure.is() )
                            {
                                rConvertedValue = xPure->queryInterface( *pDestType );
                                bConverted = rConvertedValue.hasValue();
                            }
                        }
                    }
                    break;
                    // TODO: perhaps we should allow us some more tolerance for enum types, too ....
                }

                if (!bConverted)
                {
                    throw ::com::sun::star::lang::IllegalArgumentException(
                                ::rtl::OUString::createFromAscii("Unable to convert the given value for the property ")
                            +=  GetPropertyName((sal_uInt16)nPropId),
                        static_cast< ::com::sun::star::beans::XPropertySet* >(this),
                        1);
                }
            }
        }
    }

    // the current value
    getFastPropertyValue( rOldValue, nPropId );
    return !CompareProperties( rConvertedValue, rOldValue );
}

void UnoControlModel::setFastPropertyValue_NoBroadcast( sal_Int32 nPropId, const ::com::sun::star::uno::Any& rValue ) throw (::com::sun::star::uno::Exception)
{
    ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

    // Fehlt: Die gefakten Einzelproperties des FontDescriptors...

    ImplControlProperty* pProp = mpData->Get( nPropId );
    if ( pProp )
    {
        sal_Bool bVoid = rValue.getValueType().getTypeClass() == ::com::sun::star::uno::TypeClass_VOID;

        DBG_ASSERT( !bVoid || ( GetPropertyAttribs( (sal_uInt16)nPropId ) & ::com::sun::star::beans::PropertyAttribute::MAYBEVOID ), "Property darf nicht VOID sein!" );
        ImplPropertyChanged( (sal_uInt16)nPropId );
        pProp->SetValue( rValue );
    }
    else
    {
        // exception...
        DBG_ERROR( "SetPropertyValues: Invalid Property!" );
    }
}

void UnoControlModel::getFastPropertyValue( ::com::sun::star::uno::Any& rValue, sal_Int32 nPropId ) const
{
    ::osl::Guard< ::osl::Mutex > aGuard( ((UnoControlModel*)this)->GetMutex() );

    ImplControlProperty* pProp = mpData->Get( nPropId );

    if ( pProp )
        rValue = pProp->GetValue();
    else if ( ( nPropId >= BASEPROPERTY_FONTDESCRIPTORPART_START ) && ( nPropId <= BASEPROPERTY_FONTDESCRIPTORPART_END ) )
    {
        pProp = mpData->Get( BASEPROPERTY_FONTDESCRIPTOR );
        ::com::sun::star::awt::FontDescriptor aFD;
        pProp->GetValue() >>= aFD;
        switch ( nPropId )
        {
            case BASEPROPERTY_FONTDESCRIPTORPART_NAME:          rValue <<= aFD.Name;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_STYLENAME:     rValue <<= aFD.StyleName;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_FAMILY:        rValue <<= aFD.Family;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_CHARSET:       rValue <<= aFD.CharSet;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_HEIGHT:        rValue <<= (float)aFD.Height;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_WEIGHT:        rValue <<= aFD.Weight;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_SLANT:         rValue <<= (sal_Int16)aFD.Slant;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_UNDERLINE:     rValue <<= aFD.Underline;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_STRIKEOUT:     rValue <<= aFD.Strikeout;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_WIDTH:         rValue <<= aFD.Width;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_PITCH:         rValue <<= aFD.Pitch;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_CHARWIDTH:     rValue <<= aFD.CharacterWidth;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_ORIENTATION:   rValue <<= aFD.Orientation;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_KERNING:       rValue <<= aFD.Kerning;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_WORDLINEMODE:  rValue <<= aFD.WordLineMode;
                                                                break;
            case BASEPROPERTY_FONTDESCRIPTORPART_TYPE:          rValue <<= aFD.Type;
                                                                break;
            default: DBG_ERROR( "FontProperty?!" );
        }
    }
    else
    {
        DBG_ERROR( "getFastPropertyValue - invalid Property!" );
    }
}

// ::com::sun::star::beans::XPropertySet
void UnoControlModel::setPropertyValue( const ::rtl::OUString& rPropertyName, const ::com::sun::star::uno::Any& rValue ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

    sal_Int32 nPropId = (sal_Int32) GetPropertyId( rPropertyName );
    DBG_ASSERT( nPropId, "Invalid ID in UnoControlModel::setPropertyValue" );
    setFastPropertyValue( nPropId, rValue );
}

// ::com::sun::star::beans::XFastPropertySet
void UnoControlModel::setFastPropertyValue( sal_Int32 nPropId, const ::com::sun::star::uno::Any& rValue ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    if ( ( nPropId >= BASEPROPERTY_FONTDESCRIPTORPART_START ) && ( nPropId <= BASEPROPERTY_FONTDESCRIPTORPART_END ) )
    {
        ::osl::Guard< ::osl::Mutex > aGuard( GetMutex() );

        ImplControlProperty* pProp = mpData->Get( BASEPROPERTY_FONTDESCRIPTOR );
        ::com::sun::star::awt::FontDescriptor aFD;
        pProp->GetValue() >>= aFD;
        lcl_ImplMergeFontProperty( aFD, (sal_uInt16)nPropId, rValue );

        ::com::sun::star::uno::Any aFDValue;
        aFDValue <<= aFD;
        nPropId = BASEPROPERTY_FONTDESCRIPTOR;
        setFastPropertyValues( 1, &nPropId, &aFDValue, 1 );
       }
    else
        setFastPropertyValues( 1, &nPropId, &rValue, 1 );
}

// ::com::sun::star::beans::XMultiPropertySet
::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > UnoControlModel::getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException)
{
    DBG_ERROR( "UnoControlModel::getPropertySetInfo() not possible!" );
    return ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >();
}

void UnoControlModel::setPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& rPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& Values ) throw(::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    ::osl::ClearableMutexGuard aGuard( GetMutex() );

    sal_Int32 nProps = rPropertyNames.getLength();

//  sal_Int32* pHandles = new sal_Int32[nProps];
        // don't do this - it leaks in case of an exception
    Sequence< sal_Int32 > aHandles( nProps );
    sal_Int32* pHandles = aHandles.getArray();

    // may need to change the order in the sequence, for this we need a non-const value sequence
    // 15.05.2002 - 99314 - fs@openoffice.org
    uno::Sequence< uno::Any > aValues( Values );
    uno::Any* pValues = aValues.getArray();

    sal_Int32 nValidHandles = getInfoHelper().fillHandles( pHandles, rPropertyNames );

    if ( nValidHandles )
    {
        // if somebody sets properties which are single aspects of a font descriptor,
        // remove them, and build a font descriptor instead
        ::std::auto_ptr< awt::FontDescriptor > pFD;
        for ( sal_uInt16 n = 0; n < nProps; ++n )
        {
            if ( ( pHandles[n] >= BASEPROPERTY_FONTDESCRIPTORPART_START ) && ( pHandles[n] <= BASEPROPERTY_FONTDESCRIPTORPART_END ) )
            {
                if ( !pFD.get() )
                {
                    ImplControlProperty* pProp = mpData->Get( BASEPROPERTY_FONTDESCRIPTOR );
                    pFD.reset( new awt::FontDescriptor );
                    pProp->GetValue() >>= *pFD;
                }
                lcl_ImplMergeFontProperty( *pFD, (sal_uInt16)pHandles[n], pValues[n] );
                pHandles[n] = -1;
                nValidHandles--;
            }
        }

        if ( nValidHandles )
        {
            ImplNormalizePropertySequence( nProps, pHandles, pValues, &nValidHandles );
            aGuard.clear();
                // clear our guard before calling into setFastPropertyValues - this method
                // will implicitly call property listeners, and this should not happen with
                // our mutex locked
                // #i23451# - 2004-03-18 - fs@openoffice.org
             setFastPropertyValues( nProps, pHandles, pValues, nValidHandles );
        }
        else
            aGuard.clear();
            // same as a few lines above

        // FD-Propertie nicht in das Array mergen, weil sortiert...
        if ( pFD.get() )
        {
            ::com::sun::star::uno::Any aValue;
            aValue <<= *pFD;
            sal_Int32 nHandle = BASEPROPERTY_FONTDESCRIPTOR;
            setFastPropertyValues( 1, &nHandle, &aValue, 1 );
        }
    }
}



void UnoControlModel::ImplNormalizePropertySequence( const sal_Int32 _nCount, sal_Int32* _pHandles,
    uno::Any* _pValues, sal_Int32* _pValidHandles ) const SAL_THROW(())
{
    // nothing to do here
}
