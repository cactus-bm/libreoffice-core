/*************************************************************************
 *
 *  $RCSfile: iso8601_converter.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: hr $ $Date: 2004-09-08 14:35:55 $
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

#ifndef ISO8601_CONVERTER_HXX_INCLUDED
#include "internal/iso8601_converter.hxx"
#endif

#ifndef UTILITIES_HXX_INCLUDED
#include "internal/utilities.hxx"
#endif

#include <sstream>
#include <iomanip>

//-----------------------------------
/* Converts ISO 8601 conform date/time
   represenation to the representation
   conforming to the current locale
*/
std::wstring iso8601_date_to_local_date(const std::wstring& isoDate )
{
    const std::wstring CONST_SPACE(L" ");
    ::std::wstring ws8601DateTime(isoDate);

    if ( ws8601DateTime.length() == 19 )
    {
        //fill in the SYSTEMTIME structure;
        std::string asDateTime = WStringToString( ws8601DateTime );
        SYSTEMTIME DateTime;
        DateTime.wYear         = ( unsigned short )strtol( asDateTime.substr( 0, 4 ).c_str(), NULL, 10 );
        DateTime.wMonth        = ( unsigned short )strtol( asDateTime.substr( 5, 2 ).c_str(), NULL, 10 );
        DateTime.wDayOfWeek    =  0;
        DateTime.wDay          = ( unsigned short )strtol( asDateTime.substr( 8, 2 ).c_str(), NULL, 10 );
        DateTime.wHour         = ( unsigned short )strtol( asDateTime.substr( 11,2 ).c_str(), NULL, 10 );
        DateTime.wMinute       = ( unsigned short )strtol( asDateTime.substr( 14,2 ).c_str(), NULL, 10 );
        DateTime.wSecond       = ( unsigned short )strtol( asDateTime.substr( 17,2 ).c_str(), NULL, 10 );
        DateTime.wMilliseconds =  0;

        //get Date info from structure
        WCHAR DateBuffer[ MAX_PATH ];
        int DateSize = GetDateFormatW(
            LOCALE_SYSTEM_DEFAULT,
            0,
            &DateTime,
            NULL,
            DateBuffer,
            MAX_PATH );

        if ( DateSize )
            ws8601DateTime.assign(DateBuffer);
        else
            ws8601DateTime = StringToWString( asDateTime );

        //get Time info from structure
        WCHAR TimeBuffer[ MAX_PATH ];

        int TimeSize =  GetTimeFormatW(
            LOCALE_SYSTEM_DEFAULT,
            0,
            &DateTime,
            NULL,
            TimeBuffer,
            MAX_PATH );

        if ( TimeSize )
        {
            ws8601DateTime.append(L" ");
            ws8601DateTime.append(TimeBuffer);
        }
        else
            ws8601DateTime = StringToWString( asDateTime );
    }

    return ws8601DateTime;
}

//------------------------------------
/* Converts ISO 8601 conform duration
   representation to the representation
   conforming to the current locale

   Expect format PTnHnMnS according to
   ISO 8601 where n is abitrary number
   of digits
*/

std::wstring iso8601_duration_to_local_duration(const std::wstring& iso8601duration)
{
    std::wstring days;
    std::wstring hours;
    std::wstring minutes;
    std::wstring seconds;

    std::wstring::const_iterator iter     = iso8601duration.begin();
    std::wstring::const_iterator iter_end = iso8601duration.end();

    std::wstring num;

    for (/**/; iter != iter_end; ++iter)
    {
        if (isdigit(*iter))
        {
            num += *iter;
        }
        else
        {
            if (*iter == L'D' || *iter == L'd')
                days = num;
            else if (*iter == L'H' || *iter == L'h')
                hours = num;
            else if (*iter == L'M' || *iter == L'm')
                minutes = num;
            else if (*iter == L'S' || *iter == L's')
                seconds = num;

            num.clear();
        }
    }

    if (days.length() > 0)
    {
        int h = ((_wtoi(days.c_str()) * 24) + _wtoi(hours.c_str()));
        wchar_t buff[10];
        _itow(h, buff, 10);
        hours = buff;
    }

    std::wostringstream oss;

    oss << std::setw(2) << std::setfill('0') << hours   << L":" <<
           std::setw(2) << std::setfill('0') << minutes << L":" <<
           std::setw(2) << std::setfill('0') << seconds;

    return oss.str();
}

