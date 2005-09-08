/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dateitem.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 16:04:24 $
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

// include ---------------------------------------------------------------

#define _DATETIMEITEM_CXX
#include "dateitem.hxx"

#include <unotools/intlwrapper.hxx>
#include <comphelper/processfactory.hxx>


#include <tools/stream.hxx>
#include <tools/debug.hxx>
#include <tools/datetime.hxx>

#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef _COM_SUN_STAR_UTIL_DATETIME_HPP_
#include <com/sun/star/util/DateTime.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif


#include <vcl/svapp.hxx>

#include "svtdata.hxx"
#include "svtools.hrc"

// STATIC DATA -----------------------------------------------------------

DBG_NAME(SfxDateTimeItem);


// -----------------------------------------------------------------------

TYPEINIT1(SfxDateTimeItem, SfxPoolItem);

// -----------------------------------------------------------------------

SfxDateTimeItem::SfxDateTimeItem( USHORT nWhich ) :

    SfxPoolItem( nWhich )

{
    DBG_CTOR(SfxDateTimeItem, 0);
}

// -----------------------------------------------------------------------

SfxDateTimeItem::SfxDateTimeItem( USHORT nWhich, const DateTime& rDT ) :

    SfxPoolItem( nWhich ),

    aDateTime( rDT )

{
    DBG_CTOR(SfxDateTimeItem, 0);
}

// -----------------------------------------------------------------------

SfxDateTimeItem::SfxDateTimeItem( const SfxDateTimeItem& rItem ) :

    SfxPoolItem( rItem ),

    aDateTime( rItem.aDateTime )
{
    DBG_CTOR(SfxDateTimeItem, 0);
}

// -----------------------------------------------------------------------

int SfxDateTimeItem::operator==( const SfxPoolItem& rItem ) const
{
    DBG_CHKTHIS(SfxDateTimeItem, 0);
    DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal type" );
    return ( ( (SfxDateTimeItem&)rItem ).aDateTime == aDateTime );
}

// -----------------------------------------------------------------------

int SfxDateTimeItem::Compare( const SfxPoolItem& rItem ) const
{
    DBG_CHKTHIS(SfxDateTimeItem, 0);
    DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal type" );

    // da X.Compare( Y ) am String einem Compare( Y, X ) entspricht,
    // vergleichen wir hier Y mit X
    if ( ( (const SfxDateTimeItem&)rItem ).aDateTime < aDateTime )
        return -1;
    else if ( ( (const SfxDateTimeItem&)rItem ).aDateTime == aDateTime )
        return 0;
    else
        return 1;
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxDateTimeItem::Create( SvStream& rStream, USHORT ) const
{
    DBG_CHKTHIS(SfxDateTimeItem, 0);
    ULONG nDate = 0;
    long nTime = 0;
    rStream >> nDate;
    rStream >> nTime;
    DateTime aDT(nDate, nTime);
    return new SfxDateTimeItem( Which(), aDT );
}

// -----------------------------------------------------------------------

SvStream& SfxDateTimeItem::Store( SvStream& rStream, USHORT ) const
{
    DBG_CHKTHIS(SfxDateTimeItem, 0);
    rStream << aDateTime.GetDate();
    rStream << aDateTime.GetTime();
    return rStream;
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxDateTimeItem::Clone( SfxItemPool* ) const
{
    DBG_CHKTHIS(SfxDateTimeItem, 0);
    return new SfxDateTimeItem( *this );
}

// -----------------------------------------------------------------------

SfxItemPresentation SfxDateTimeItem::GetPresentation
(
    SfxItemPresentation     ePresentation,
    SfxMapUnit              eCoreMetric,
    SfxMapUnit              ePresentationMetric,
    XubString&              rText,
    const IntlWrapper *   pIntlWrapper
)   const
{
    DBG_CHKTHIS(SfxDateTimeItem, 0);
    if (aDateTime.IsValid())
        if (pIntlWrapper)
        {
            rText = pIntlWrapper->getLocaleData()->getDate(aDateTime);
            rText.AppendAscii(RTL_CONSTASCII_STRINGPARAM(", "));
            rText += pIntlWrapper->getLocaleData()->getTime(aDateTime);
        }
        else
        {
            DBG_WARNING("SfxDateTimeItem::GetPresentation():"
                         " Using default en_US IntlWrapper");
            const IntlWrapper aIntlWrapper(
                ::comphelper::getProcessServiceFactory(), LANGUAGE_ENGLISH_US );
            rText = aIntlWrapper.getLocaleData()->getDate(aDateTime);
            rText.AppendAscii(RTL_CONSTASCII_STRINGPARAM(", "));
            rText += aIntlWrapper.getLocaleData()->getTime(aDateTime);
        }
    else
        rText.Erase();
    return SFX_ITEM_PRESENTATION_NAMELESS;
}

//----------------------------------------------------------------------------
// virtual
BOOL SfxDateTimeItem::PutValue( const com::sun::star::uno::Any& rVal,
                                   BYTE nMemberId )
{
    sal_Bool bConvert = 0!=(nMemberId&CONVERT_TWIPS);
    nMemberId &= ~CONVERT_TWIPS;
    com::sun::star::util::DateTime aValue;
    if ( rVal >>= aValue )
    {
        aDateTime = DateTime( Date( aValue.Day,
                                      aValue.Month,
                                      aValue.Year ),
                              Time( aValue.Hours,
                                      aValue.Minutes,
                                      aValue.Seconds,
                                      aValue.HundredthSeconds ) );
        return TRUE;
    }

    DBG_ERROR( "SfxDateTimeItem::PutValue - Wrong type!" );
    return FALSE;
}

//----------------------------------------------------------------------------
// virtual
BOOL SfxDateTimeItem::QueryValue( com::sun::star::uno::Any& rVal,
                                   BYTE nMemberId ) const
{
    sal_Bool bConvert = 0!=(nMemberId&CONVERT_TWIPS);
    nMemberId &= ~CONVERT_TWIPS;
    com::sun::star::util::DateTime aValue( aDateTime.Get100Sec(),
                                           aDateTime.GetSec(),
                                              aDateTime.GetMin(),
                                           aDateTime.GetHour(),
                                           aDateTime.GetDay(),
                                           aDateTime.GetMonth(),
                                           aDateTime.GetYear() );
    rVal <<= aValue;
    return TRUE;
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

TYPEINIT1(SfxColumnDateTimeItem, SfxDateTimeItem);


SfxColumnDateTimeItem::SfxColumnDateTimeItem( USHORT nWhich ) :
    SfxDateTimeItem( nWhich )
{}

SfxColumnDateTimeItem::SfxColumnDateTimeItem( USHORT nWhich, const DateTime& rDT ) :
    SfxDateTimeItem( nWhich, rDT )
{}

SfxColumnDateTimeItem::SfxColumnDateTimeItem( const SfxDateTimeItem& rCpy ) :
    SfxDateTimeItem( rCpy )
{}

SfxPoolItem* SfxColumnDateTimeItem::Clone( SfxItemPool* ) const
{
    return new SfxColumnDateTimeItem( *this );
}

SfxItemPresentation SfxColumnDateTimeItem::GetPresentation
(
    SfxItemPresentation     ePresentation,
    SfxMapUnit              eCoreMetric,
    SfxMapUnit              ePresentationMetric,
    XubString&              rText,
    const IntlWrapper *   pIntlWrapper
)   const
{
    DBG_ASSERT(pIntlWrapper,
               "SfxColumnDateTimeItem::GetPresentation():"
                " Using default en_US IntlWrapper");

    ::com::sun::star::lang::Locale aLocale;
    if (GetDateTime() == DateTime(Date(1, 2, 3), Time(3, 2, 1)))
        rText = String(SvtResId(STR_COLUM_DT_AUTO,
                                pIntlWrapper ?
                                 pIntlWrapper->getLocale() :
                                 aLocale));
    else if (pIntlWrapper)
    {
        rText = pIntlWrapper->getLocaleData()->getDate(GetDateTime());
        rText.AppendAscii(RTL_CONSTASCII_STRINGPARAM(", "));
        rText += pIntlWrapper->getLocaleData()->getTime(GetDateTime());
    }
    else
    {
        const IntlWrapper aIntlWrapper(
            ::comphelper::getProcessServiceFactory(), LANGUAGE_ENGLISH_US );
        rText = aIntlWrapper.getLocaleData()->getDate(GetDateTime());
        rText.AppendAscii(RTL_CONSTASCII_STRINGPARAM(", "));
        rText += aIntlWrapper.getLocaleData()->getTime(GetDateTime());
    }
    return SFX_ITEM_PRESENTATION_NAMELESS;
}



