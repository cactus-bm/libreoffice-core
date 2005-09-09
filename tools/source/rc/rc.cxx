/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: rc.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 14:30:33 $
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

#define _TOOLS_RC_CXX

#include <string.h>

#ifndef _DATE_HXX
#include <date.hxx>
#endif
#ifndef _TIME_HXX
#include <time.hxx>
#endif
#ifndef _INTN_HXX
#include <intn.hxx>
#endif

#ifndef _TOOLS_RC_HXX
#include <rc.hxx>
#endif
#ifndef _TOOLS_RCID_H
#include <rcid.h>
#endif

// =======================================================================

Resource::Resource( const ResId& rResId )
{
    GetRes( rResId.SetRT( RSC_RESOURCE ) );
}

// -----------------------------------------------------------------------

void Resource::GetRes( const ResId& rResId )
{
    if ( rResId.GetResMgr() )
        rResId.GetResMgr()->GetResource( rResId, this );
    else
        GetResManager()->GetResource( rResId, this );

    IncrementRes( sizeof( RSHEADER_TYPE ) );
}

// -----------------------------------------------------------------------

// =======================================================================

Time::Time( const ResId& rResId )
{
    nTime = 0;
    rResId.SetRT( RSC_TIME );
    ResMgr* pResMgr = NULL;

    ResMgr::GetResourceSkipHeader( rResId, &pResMgr );

    ULONG nObjMask = (USHORT)pResMgr->ReadLong();

    if ( 0x01 & nObjMask )
        SetHour( (USHORT)pResMgr->ReadShort() );
    if ( 0x02 & nObjMask )
        SetMin( (USHORT)pResMgr->ReadShort() );
    if ( 0x04 & nObjMask )
        SetSec( (USHORT)pResMgr->ReadShort() );
    if ( 0x08 & nObjMask )
        Set100Sec( (USHORT)pResMgr->ReadShort() );
}

// =======================================================================

Date::Date( const ResId& rResId )
{
    rResId.SetRT( RSC_DATE );
    ResMgr* pResMgr = NULL;

    ResMgr::GetResourceSkipHeader( rResId, &pResMgr );

    ULONG nObjMask = (USHORT)pResMgr->ReadLong();

    if ( 0x01 & nObjMask )
        SetYear( (USHORT)pResMgr->ReadShort() );
    if ( 0x02 & nObjMask )
        SetMonth( (USHORT)pResMgr->ReadShort() );
    if ( 0x04 & nObjMask )
        SetDay( (USHORT)pResMgr->ReadShort() );
}

// =======================================================================

International::International( const ResId& rResId )
{
    rResId.SetRT( RSC_INTERNATIONAL );
    ResMgr* pResMgr = NULL;

    ResMgr::GetResourceSkipHeader( rResId, &pResMgr );

    ULONG nObjMask = (USHORT)pResMgr->ReadLong();

    LanguageType eLangType = LANGUAGE_SYSTEM;
    LanguageType eFormatType = LANGUAGE_SYSTEM;

    if ( 0x0001 & nObjMask )
    {
            eLangType = (LanguageType)pResMgr->ReadLong();
            eFormatType = eLangType;
    }
    if ( 0x0002 & nObjMask )
        eFormatType = (LanguageType)pResMgr->ReadLong();
    Init( eLangType, eFormatType );

    if ( 0x0004 & nObjMask )
        SetDateFormat( (DateFormat)pResMgr->ReadLong() );
    if ( 0x0008 & nObjMask )
            SetDateDayLeadingZero( (BOOL)(USHORT)pResMgr->ReadShort() );
    if ( 0x0010 & nObjMask )
        SetDateMonthLeadingZero( (BOOL)(USHORT)pResMgr->ReadShort() );
    if ( 0x0020 & nObjMask )
        SetDateCentury( (BOOL)(USHORT)pResMgr->ReadShort() );
    if ( 0x0040 & nObjMask )
        SetLongDateFormat( (DateFormat)pResMgr->ReadLong() );
    if ( 0x0080 & nObjMask )
            SetLongDateDayOfWeekFormat( (DayOfWeekFormat)pResMgr->ReadLong() );
    if ( 0x0100 & nObjMask )
        SetLongDateDayOfWeekSep( pResMgr->ReadString() );
    if ( 0x0200 & nObjMask )
        SetLongDateDayLeadingZero( (BOOL)(USHORT)pResMgr->ReadShort() );
    if ( 0x0400 & nObjMask )
        SetLongDateDaySep( pResMgr->ReadString() );
    if ( 0x0800 & nObjMask )
        SetLongDateMonthFormat( (MonthFormat)pResMgr->ReadLong() );
    if ( 0x1000 & nObjMask )
        SetLongDateMonthSep( pResMgr->ReadString() );
    if ( 0x2000 & nObjMask )
        SetLongDateCentury( (BOOL)(USHORT)pResMgr->ReadShort() );
    if ( 0x4000 & nObjMask )
        SetLongDateYearSep( pResMgr->ReadString() );
    if ( 0x8000 & nObjMask )
        SetTimeFormat( (TimeFormat)pResMgr->ReadLong() );

    // Zweite Maske holen
    nObjMask = pResMgr->ReadLong();
    if ( 0x0001 & nObjMask )
        SetTimeLeadingZero( (BOOL)(USHORT)pResMgr->ReadShort() );
    if ( 0x0002 & nObjMask )
        SetTimeAM( pResMgr->ReadString() );
    if ( 0x0004 & nObjMask )
        SetTimePM( pResMgr->ReadString() );
    if ( 0x0008 & nObjMask )
        SetNumLeadingZero( (BOOL)(USHORT)pResMgr->ReadShort() );
    if ( 0x0010 & nObjMask )
        SetNumDigits( (USHORT)pResMgr->ReadShort() );
    if ( 0x0020 & nObjMask )
        SetCurrPositiveFormat( (USHORT)pResMgr->ReadShort() );
    if ( 0x0040 & nObjMask )
        SetCurrNegativeFormat( (USHORT)pResMgr->ReadShort() );
    if ( 0x0080 & nObjMask )
        SetCurrDigits( (USHORT)pResMgr->ReadShort() );
    if ( 0x0100 & nObjMask )
        SetNumTrailingZeros( (BOOL)(USHORT)pResMgr->ReadShort() );
    if ( 0x0200 & nObjMask )
        SetMeasurementSystem( (MeasurementSystem)(USHORT)pResMgr->ReadShort() );
}
