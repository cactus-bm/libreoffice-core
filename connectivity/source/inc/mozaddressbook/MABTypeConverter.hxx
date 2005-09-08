/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: MABTypeConverter.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 07:32:11 $
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


#ifndef _CONNECTIVITY_MAB_TYPECONVERTER_HXX_
#define _CONNECTIVITY_MAB_TYPECONVERTER_HXX_

#ifndef _CONNECTIVITY_FILE_TYPECONVERTER_HXX_
#include "file/FTable.hxx"
#endif

#include <string>

namespace connectivity
{
    namespace mozaddressbook
    {
        class OMozabTypeConverter
        {
        public:
            static void  ouStringToNsString(const ::rtl::OUString&, nsString&);
            static void  nsStringToOUString(const nsString&, ::rtl::OUString&);
            static void  prUnicharToOUString(const PRUnichar*, ::rtl::OUString&);
            // Use free() for the following 3 calls.
            static char *ouStringToCCharStringAscii(const ::rtl::OUString&);
            static char *nsStringToCCharStringAscii(const nsString&);
            static char *ouStringToCCharStringUtf8(const ::rtl::OUString&);
            // Convert to stl-string.
            static ::std::string ouStringToStlString(const ::rtl::OUString&);
            static ::std::string nsStringToStlString(const nsString&);
        };
    }
}

#endif // _CONNECTIVITY_MAB_TYPECONVERTER_HXX_

