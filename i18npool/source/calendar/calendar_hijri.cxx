/*************************************************************************
 *
 *  $RCSfile: calendar_hijri.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: bustamam $ $Date: 2002-08-15 03:09:37 $
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

#include <math.h>

#include "calendar_hijri.hxx"

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::i18n;
using namespace ::rtl;

#define ERROR RuntimeException()

static UErrorCode status; // status is shared in all calls to Calendar, it has to be reset for each call.

// radians per degree (pi/180)
const double Calendar_hijri::RadPerDeg      = 0.01745329251994329577;

// Synodic Period (mean time between 2 successive new moon: 29d, 12 hr, 44min, 3sec
const double Calendar_hijri::SynPeriod      = 29.53058868;
const double Calendar_hijri::SynMonth       = 365.25/29.53058868;   // Solar days in a year/SynPeriod

// Julian day on Jan 1, 1900
const double Calendar_hijri::jd1900     = 2415020.75933;

// Reference point: March 26, 2001 == 1422 Hijri == 1252 Synodial month from 1900
const sal_Int32 Calendar_hijri::SynRef      = 1252;
const sal_Int32 Calendar_hijri::GregRef     = 1422;

// Local time specific to Saudi Arabia
const double Calendar_hijri::SA_TimeZone    = 3.0;

const double Calendar_hijri::EveningPeriod  = 6.0;

const sal_Int32 Calendar_hijri::LeapYear[] = {
    2, 5, 7, 10, 13, 16, 18, 21, 24, 26, 29
};

Calendar_hijri::Calendar_hijri()
{
    cCalendar = "com.sun.star.i18n.Calendar_hijri";
}

#define FIELDS  ((1 << CalendarFieldIndex::ERA) | (1 << CalendarFieldIndex::YEAR) | (1 << CalendarFieldIndex::MONTH) | (1 << CalendarFieldIndex::DAY_OF_MONTH))

// map field value from hijri calendar to gregorian calendar
void SAL_CALL Calendar_hijri::mapToGregorian() throw(RuntimeException)
{
    if (fieldSet & FIELDS) {
        sal_Int32 day = (sal_Int32)fieldSetValue[CalendarFieldIndex::DAY_OF_MONTH];
        sal_Int32 month = (sal_Int32)fieldSetValue[CalendarFieldIndex::MONTH] + 1;
        sal_Int32 year = (sal_Int32)fieldSetValue[CalendarFieldIndex::YEAR];
        if (fieldSetValue[CalendarFieldIndex::ERA] == 0)
        year = 1 - year;

        ToGregorian(&day, &month, &year);

        fieldSetValue[CalendarFieldIndex::ERA] = year <= 0 ? 0 : 1;
        fieldSetValue[CalendarFieldIndex::MONTH] = (sal_Int32) (month - 1);
        fieldSetValue[CalendarFieldIndex::DAY_OF_MONTH] = (sal_Int16) day;
        fieldSetValue[CalendarFieldIndex::YEAR] = (sal_Int16) year;
        fieldSet |= FIELDS;
    }
}

// map field value from gregorian calendar to hijri calendar
void SAL_CALL Calendar_hijri::mapFromGregorian() throw(RuntimeException)
{
    sal_Int32 month, day, year;

    day = (sal_Int32)fieldValue[CalendarFieldIndex::DAY_OF_MONTH];
    month = (sal_Int32)fieldValue[CalendarFieldIndex::MONTH] + 1;
    year = (sal_Int32)fieldValue[CalendarFieldIndex::YEAR];
    if (fieldValue[CalendarFieldIndex::ERA] == 0)
        year = 1 - year;

    // Get Hijri date
    getHijri(&day, &month, &year);

    fieldValue[CalendarFieldIndex::DAY_OF_MONTH] = (sal_Int16)day;
    fieldValue[CalendarFieldIndex::MONTH] = month - 1;
    fieldValue[CalendarFieldIndex::YEAR] = (sal_Int16)year;
    fieldValue[CalendarFieldIndex::ERA] = (sal_Int16) year < 1 ? 0 : 1;
}

//
// This function returns the Julian date/time of the Nth new moon since
// January 1900.  The synodic month is passed as parameter.
//
// Adapted from "Astronomical  Formulae for Calculators" by
// Jean Meeus, Third Edition, Willmann-Bell, 1985.
//
double
Calendar_hijri::NewMoon(sal_Int32 n)
{
    double jd, t, t2, t3, k, ma, sa, tf, xtra;
    k = n;
    t = k/1236.85;  // Time in Julian centuries from 1900 January 0.5
    t2 = t * t;
    t3 = t2 * t;

    // Mean time of phase
    jd =  jd1900
        + SynPeriod * k
        - 0.0001178 * t2
        - 0.000000155 * t3
        + 0.00033 * sin(RadPerDeg * (166.56 + 132.87 * t - 0.009173 * t2));

    // Sun's mean anomaly in radian
    sa =  RadPerDeg * (359.2242
                + 29.10535608 * k
                - 0.0000333 * t2
                - 0.00000347 * t3);

    // Moon's mean anomaly
    ma =  RadPerDeg * (306.0253
                + 385.81691806 * k
                + 0.0107306 * t2
                + 0.00001236 * t3);

    // Moon's argument of latitude
    tf = RadPerDeg * 2.0 * (21.2964
                + 390.67050646 * k
                - 0.0016528 * t2
                - 0.00000239 * t3);

    // should reduce to interval between 0 to 1.0 before calculating further
    // Corrections for New Moon
    xtra = (0.1734 - 0.000393 * t) * sin(sa)
        + 0.0021 * sin(sa * 2)
        - 0.4068 * sin(ma)
        + 0.0161 * sin(2 * ma)
        - 0.0004 * sin(3 * ma)
        + 0.0104 * sin(tf)
        - 0.0051 * sin(sa + ma)
        - 0.0074 * sin(sa - ma)
        + 0.0004 * sin(tf + sa)
        - 0.0004 * sin(tf - sa)
        - 0.0006 * sin(tf + ma)
        + 0.0010 * sin(tf - ma)
        + 0.0005 * sin(sa + 2 * ma);

    // convert from Ephemeris Time (ET) to (approximate) Universal Time (UT)
    jd += xtra - (0.41 + 1.2053 * t + 0.4992 * t2)/1440;

    return (jd);
}

// Get Hijri Date
void
Calendar_hijri::getHijri(sal_Int32 *day, sal_Int32 *month, sal_Int32 *year)
{
    double prevday;
    double dayfraction;
    sal_Int32 syndiff;
    sal_Int32 newsyn;
    double newjd;
    double julday;
    sal_Int32 synmonth;

    // Get Julian Day from Gregorian
    julday = getJulianDay(*day, *month, *year);

    // obtain approx. of how many Synodic months since the beginning of the year 1900
    synmonth = (sal_Int32)(0.5 + (julday - jd1900)/SynPeriod);

    newsyn = synmonth;
    prevday = (sal_Int32)julday - 0.5;

    do {
        newjd = NewMoon(newsyn);
        dayfraction = (newjd - (sal_Int32)newjd);   // Get fraction of day

        // If new moon appears during the first half of the day, take the next day
        if (dayfraction <= 0.5) {
            newjd += 1.0;
        }

        // Convert to localtime (assuming Mecca)
        dayfraction = (dayfraction - 0.5) * 24.0 + SA_TimeZone;

        // if new moon starts after second half of the day
        if (dayfraction > EveningPeriod) {
            newjd += 1.0;
        }

        // Decrement syndonic months
        newsyn--;
    } while (newjd > prevday);
    newsyn++;

    // difference from reference point
    syndiff = newsyn - SynRef;

    // Round up the day
    *day = (sal_Int32)(((sal_Int32)julday) - newjd + 0.5);
    *month =  (syndiff % 12) + 1;

    // currently not supported
    //dayOfYear = (sal_Int32)(month * SynPeriod + day);
    *year = GregRef + (sal_Int32)(syndiff / 12);

    // If month negative, consider it previous year
    if (syndiff != 0 && *month <= 0) {
        *month += 12;
        *year--;
    }

    // If Before Hijri subtract 1
    if (*year <= 0) *year--;
}

void
Calendar_hijri::ToGregorian(sal_Int32 *day, sal_Int32 *month, sal_Int32 *year)
{
    sal_Int32 nmonth;
    double dayfraction;
    double jday;
    sal_Int32 dayint;

    if ( *year < 0 ) *year++;

    // Number of month from reference point
    nmonth = (*month + 1) + *year * 12 - (GregRef * 12 + 1);

    // Add Synodic Reference point
    nmonth += SynRef;

    // Get Julian days add time too
    jday = NewMoon(nmonth) + *day;

    // Convert the julian day to Gregorian
    jday = jday - 1721119;
    *year = (4 * jday - 1)/146097;
    jday = 4 * jday - 1 - 146097 * *year;
    *day = jday/4;
    jday = (4 * *day + 3) /1461;
    *day = 4 * *day + 3 - 1461 * jday;
    *day = (*day + 4) /4;
    *month = (5 * *day - 3) / 153;
    *day = 5 * *day - 3 - 153 * *month;
    *day = (*day + 5) / 5;
    *year = 100 * *year + jday;
    if ( *month < 10 ) {
    *month = *month + 3;
    } else {
    *month = *month - 9;
    *year = *year + 1;
    }
}

double
Calendar_hijri::getJulianDay(sal_Int32 day, sal_Int32 month, sal_Int32 year)
{
    double jy, jm;

    if( year == 0 ) {
    return -1.0;
    }

    if( year == 1582 && month == 10 && day > 4 && day < 15 ) {
    return -1.0;
    }

    if( month > 2 ) {
    jy = year;
    jm = month + 1;
    } else {
    jy = year - 1;
    jm = month + 13;
    }

    sal_Int32 intgr = (sal_Int32)((sal_Int32)(365.25 * jy) + (sal_Int32)(30.6001 * jm) + day + 1720995 );

    //check for switch to Gregorian calendar
    double gregcal = 15 + 31 * ( 10 + 12 * 1582 );

    if( day + 31 * (month + 12 * year) >= gregcal ) {
    double ja;
    ja = (sal_Int32)(0.01 * jy);
    intgr += 2 - ja + (sal_Int32)(0.25 * ja);
    }

    return (double) intgr;
}
