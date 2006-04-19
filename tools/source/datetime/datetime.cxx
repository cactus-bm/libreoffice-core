/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: datetime.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: hr $ $Date: 2006-04-19 14:02:20 $
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

#include <datetime.hxx>
#include <rtl/math.hxx>

/*************************************************************************
|*
|*    DateTime::IsBetween()
|*
|*    Beschreibung      DATETIME.SDW
|*    Ersterstellung    TH 18.05.92
|*    Letzte Aenderung  TH 18.05.92
|*
*************************************************************************/

BOOL DateTime::IsBetween( const DateTime& rFrom,
                          const DateTime& rTo ) const
{
    if ( (*this >= rFrom) && (*this <= rTo) )
        return TRUE;
    else
        return FALSE;
}

/*************************************************************************
|*
|*    DateTime::operator >()
|*
|*    Beschreibung      DATETIME.SDW
|*    Ersterstellung    TH 18.05.92
|*    Letzte Aenderung  TH 18.05.92
|*
*************************************************************************/

BOOL DateTime::operator >( const DateTime& rDateTime ) const
{
    if ( (Date::operator>( rDateTime )) ||
         (Date::operator==( rDateTime ) && Time::operator>( rDateTime )) )
        return TRUE;
    else
        return FALSE;
}

/*************************************************************************
|*
|*    DateTime::operator <()
|*
|*    Beschreibung      DATETIME.SDW
|*    Ersterstellung    TH 18.05.92
|*    Letzte Aenderung  TH 18.05.92
|*
*************************************************************************/

BOOL DateTime::operator <( const DateTime& rDateTime ) const
{
    if ( (Date::operator<( rDateTime )) ||
         (Date::operator==( rDateTime ) && Time::operator<( rDateTime )) )
        return TRUE;
    else
        return FALSE;
}

/*************************************************************************
|*
|*    DateTime::operator >=()
|*
|*    Beschreibung      DATETIME.SDW
|*    Ersterstellung    TH 18.05.92
|*    Letzte Aenderung  TH 18.05.92
|*
*************************************************************************/

BOOL DateTime::operator >=( const DateTime& rDateTime ) const
{
    if ( (Date::operator>( rDateTime )) ||
         (Date::operator==( rDateTime ) && Time::operator>=( rDateTime )) )
        return TRUE;
    else
        return FALSE;
}

/*************************************************************************
|*
|*    DateTime::operator <=()
|*
|*    Beschreibung      DATETIME.SDW
|*    Ersterstellung    TH 18.05.92
|*    Letzte Aenderung  TH 18.05.92
|*
*************************************************************************/

BOOL DateTime::operator <=( const DateTime& rDateTime ) const
{
    if ( (Date::operator<( rDateTime )) ||
         (Date::operator==( rDateTime ) && Time::operator<=( rDateTime )) )
        return TRUE;
    else
        return FALSE;
}

/*************************************************************************
|*
|*    DateTime::GetSecFromDateTime()
|*
|*    Beschreibung      DATETIME.SDW
|*    Ersterstellung    TH 02.10.96
|*    Letzte Aenderung  TH 02.10.96
|*
*************************************************************************/

ULONG DateTime::GetSecFromDateTime( const Date& rDate ) const
{
    if ( Date::operator<( rDate ) )
        return 0;
    else
    {
        ULONG nSec = *this- (DateTime) rDate;
        nSec *= 24UL*60*60;
        long nHour = GetHour();
        long nMin  = GetMin();
        nSec += (nHour*3600)+(nMin*60)+GetSec();
        return nSec;
    }
}

/*************************************************************************
|*
|*    DateTime::GetSecFromDateTime()
|*
|*    Beschreibung      DATETIME.SDW
|*    Ersterstellung    TH 02.10.96
|*    Letzte Aenderung  TH 02.10.96
|*
*************************************************************************/

void DateTime::MakeDateTimeFromSec( const Date& rDate, ULONG nSec )
{
    long nDays = nSec / (24UL*60*60);
    ((Date*)this)->operator=( rDate );
    nSec -= nDays * (24UL*60*60);
    USHORT nMin = nSec / 60;
    nSec -= nMin * 60;
    ((Time*)this)->operator=( Time( 0, nMin, (USHORT)nSec ) );
    operator+=( nDays );
}

/*************************************************************************
|*
|*    DateTime::operator +=()
|*
|*    Beschreibung      DATETIME.SDW
|*    Ersterstellung    TH 02.10.96
|*    Letzte Aenderung  TH 02.10.96
|*
*************************************************************************/

DateTime& DateTime::operator +=( const Time& rTime )
{
    Time aTime = *this;
    aTime += rTime;
    USHORT nHours = aTime.GetHour();
    if ( aTime.GetTime() > 0 )
    {
        while ( nHours >= 24 )
        {
            Date::operator++();
            nHours -= 24;
        }
        aTime.SetHour( nHours );
    }
    else if ( aTime.GetTime() != 0 )
    {
        while ( nHours >= 24 )
        {
            Date::operator--();
            nHours -= 24;
        }
        Date::operator--();
        aTime = Time( 24, 0, 0 )+aTime;
    }
    Time::operator=( aTime );

    return *this;
}

/*************************************************************************
|*
|*    DateTime::operator -=()
|*
|*    Beschreibung      DATETIME.SDW
|*    Ersterstellung    TH 02.10.96
|*    Letzte Aenderung  TH 02.10.96
|*
*************************************************************************/

DateTime& DateTime::operator -=( const Time& rTime )
{
    Time aTime = *this;
    aTime -= rTime;
    USHORT nHours = aTime.GetHour();
    if ( aTime.GetTime() > 0 )
    {
        while ( nHours >= 24 )
        {
            Date::operator++();
            nHours -= 24;
        }
        aTime.SetHour( nHours );
    }
    else if ( aTime.GetTime() != 0 )
    {
        while ( nHours >= 24 )
        {
            Date::operator--();
            nHours -= 24;
        }
        Date::operator--();
        aTime = Time( 24, 0, 0 )+aTime;
    }
    Time::operator=( aTime );

    return *this;
}

/*************************************************************************
|*
|*    DateTime::operator+()
|*
|*    Beschreibung      DATETIME.SDW
|*    Ersterstellung    TH 02.10.96
|*    Letzte Aenderung  TH 02.10.96
|*
*************************************************************************/

DateTime operator +( const DateTime& rDateTime, long nDays )
{
    DateTime aDateTime( rDateTime );
    aDateTime += nDays;
    return aDateTime;
}

/*************************************************************************
|*
|*    DateTime::operator-()
|*
|*    Beschreibung      DATETIME.SDW
|*    Ersterstellung    TH 02.10.96
|*    Letzte Aenderung  TH 02.10.96
|*
*************************************************************************/

DateTime operator -( const DateTime& rDateTime, long nDays )
{
    DateTime aDateTime( rDateTime );
    aDateTime -= nDays;
    return aDateTime;
}

/*************************************************************************
|*
|*    DateTime::operator+()
|*
|*    Beschreibung      DATETIME.SDW
|*    Ersterstellung    TH 02.10.96
|*    Letzte Aenderung  TH 02.10.96
|*
*************************************************************************/

DateTime operator +( const DateTime& rDateTime, const Time& rTime )
{
    DateTime aDateTime( rDateTime );
    aDateTime += rTime;
    return aDateTime;
}

/*************************************************************************
|*
|*    DateTime::operator-()
|*
|*    Beschreibung      DATETIME.SDW
|*    Ersterstellung    TH 02.10.96
|*    Letzte Aenderung  TH 02.10.96
|*
*************************************************************************/

DateTime operator -( const DateTime& rDateTime, const Time& rTime )
{
    DateTime aDateTime( rDateTime );
    aDateTime -= rTime;
    return aDateTime;
}

/*************************************************************************
|*
|*    DateTime::operator +=( double )
|*
*************************************************************************/

DateTime& DateTime::operator +=( double fTimeInDays )
{
    double fInt, fFrac;
    if ( fTimeInDays < 0.0 )
    {
        fInt = ::rtl::math::approxCeil( fTimeInDays );
        fFrac = fInt <= fTimeInDays ? 0.0 : fTimeInDays - fInt;
    }
    else
    {
        fInt = ::rtl::math::approxFloor( fTimeInDays );
        fFrac = fInt >= fTimeInDays ? 0.0 : fTimeInDays - fInt;
    }
    Date::operator+=( long(fInt) );     // full days
    if ( fFrac )
    {
        Time aTime(0);  // default ctor calls system time, we don't need that
        fFrac *= 24UL * 60 * 60 * 1000;     // time expressed in milliseconds
        aTime.MakeTimeFromMS( long(fFrac) );    // method handles negative ms
        operator+=( aTime );
    }
    return *this;
}

/*************************************************************************
|*
|*    DateTime::operator +( double )
|*
*************************************************************************/

DateTime operator +( const DateTime& rDateTime, double fTimeInDays )
{
    DateTime aDateTime( rDateTime );
    aDateTime += fTimeInDays;
    return aDateTime;
}

/*************************************************************************
|*
|*    DateTime::operator -()
|*
*************************************************************************/

double operator -( const DateTime& rDateTime1, const DateTime& rDateTime2 )
{
    long nDays = (const Date&) rDateTime1 - (const Date&) rDateTime2;
    long nTime = rDateTime1.GetMSFromTime() - rDateTime2.GetMSFromTime();
    if ( nTime )
    {
        double fTime = double(nTime);
        fTime /= 24UL * 60 * 60 * 1000; // convert from milliseconds to fraction
        if ( nDays < 0 && fTime > 0.0 )
            fTime = 1.0 - fTime;
        return double(nDays) + fTime;
    }
    return double(nDays);
}

void DateTime::GetWin32FileDateTime( sal_uInt32 & rLower, sal_uInt32 & rUpper )
{
    const sal_Int64 a100nPerSecond = SAL_CONST_INT64( 10000000 );
    const sal_Int64 a100nPerDay = a100nPerSecond * sal_Int64( 60 * 60 * 24 );

    sal_Int64 nYears = GetYear() - 1601;
    sal_Int64 nDays =
        nYears * 365 +
        nYears / 4 - nYears / 100 + nYears / 400 +
        GetDayOfYear() - 1;

    sal_Int64 aTime =
        a100nPerDay * nDays +
        a100nPerSecond * (
                sal_Int64( GetSec() ) +
                60 * sal_Int64( GetMin() ) +
                60 * 60 * sal_Int64( GetHour() ) );

    rLower = sal_uInt32( aTime % SAL_CONST_UINT64( 0x100000000 ) );
    rUpper = sal_uInt32( aTime / SAL_CONST_UINT64( 0x100000000 ) );
}

DateTime DateTime::CreateFromWin32FileDateTime( const sal_uInt32 & rLower, const sal_uInt32 & rUpper )
{
    const sal_Int64 a100nPerSecond = SAL_CONST_INT64( 10000000 );
    const sal_Int64 a100nPerDay = a100nPerSecond * sal_Int64( 60 * 60 * 24 );

    sal_Int64 aTime = sal_Int64(
            sal_uInt64( rUpper ) * SAL_CONST_UINT64( 0x100000000 ) +
            sal_uInt64( rLower ) );

    sal_Int64 nDays = aTime / a100nPerDay;
    sal_Int64 nYears =
        ( nDays -
          ( nDays / ( 4 * 365 ) ) +
          ( nDays / ( 100 * 365 ) ) -
          ( nDays / ( 400 * 365 ) ) ) / 365;
    nDays -= nYears * 365 + nYears / 4 - nYears / 100 + nYears / 400;

    USHORT nMonths = 0;
    for( long nDaysCount = nDays; nDaysCount >= 0; )
    {
        nDays = nDaysCount;
        nMonths ++;
        nDaysCount -= Date( 1, nMonths, 1601 + nYears ).GetDaysInMonth();
    }

    Date _aDate( (USHORT)( nDays + 1 ), nMonths, nYears + 1601 );
    Time _aTime( ULONG( ( aTime / ( a100nPerSecond * 60 * 60 ) ) % sal_Int64( 24 ) ),
            ULONG( ( aTime / ( a100nPerSecond * 60 ) ) % sal_Int64( 60 ) ),
            ULONG( ( aTime / ( a100nPerSecond ) ) % sal_Int64( 60 ) ) );

    return DateTime( _aDate, _aTime );
}
