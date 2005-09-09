/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: tustring.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 14:38:54 $
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

#include <string.h>

#ifndef _OSL_INTERLCK_H
#include <osl/interlck.h>
#endif
#ifndef _RTL_ALLOC_H
#include <rtl/alloc.h>
#endif
#ifndef _RTL_MEMORY_H
#include <rtl/memory.h>
#endif
#ifndef _RTL_TENCINFO_H
#include <rtl/tencinfo.h>
#endif
#ifndef INCLUDED_RTL_INSTANCE_HXX
#include <rtl/instance.hxx>
#endif

#define private public
#include <string.hxx>
#undef private
#include <impstrg.hxx>

#include <debug.hxx>

// =======================================================================

DBG_NAME( UniString );
DBG_NAMEEX( ByteString );

// -----------------------------------------------------------------------

#define STRCODE         sal_Unicode
#define STRING          UniString
#define STRINGDATA      UniStringData
#define DBGCHECKSTRING  DbgCheckUniString

// -----------------------------------------------------------------------

#include <strimp.cxx>
#include <strucvt.cxx>
#include <strascii.cxx>

UniString::UniString(char c): mpData(ImplAllocData(1)) { mpData->maStr[0] = c; }

// -----------------------------------------------------------------------

UniString UniString::CreateFromInt32( sal_Int32 n, sal_Int16 nRadix )
{
    sal_Unicode aBuf[RTL_USTR_MAX_VALUEOFINT32];
    return UniString( aBuf, rtl_ustr_valueOfInt32( aBuf, n, nRadix ) );
}

// -----------------------------------------------------------------------

UniString UniString::CreateFromInt64( sal_Int64 n, sal_Int16 nRadix )
{
    sal_Unicode aBuf[RTL_USTR_MAX_VALUEOFINT64];
    return UniString( aBuf, rtl_ustr_valueOfInt64( aBuf, n, nRadix ) );
}

// -----------------------------------------------------------------------

UniString UniString::CreateFromFloat( float f )
{
    sal_Unicode aBuf[RTL_USTR_MAX_VALUEOFFLOAT];
    return UniString( aBuf, rtl_ustr_valueOfFloat( aBuf, f ) );
}

// -----------------------------------------------------------------------

UniString UniString::CreateFromDouble( double d )
{
    sal_Unicode aBuf[RTL_USTR_MAX_VALUEOFDOUBLE];
    return UniString( aBuf, rtl_ustr_valueOfDouble( aBuf, d ) );
}

// -----------------------------------------------------------------------

namespace { struct Empty : public rtl::Static< const UniString, Empty> {}; }
const UniString& UniString::EmptyString()
{
    return Empty::get();
}

// -----------------------------------------------------------------------

sal_Int32 UniString::ToInt32() const
{
    DBG_CHKTHIS( UniString, DbgCheckUniString );

    return rtl_ustr_toInt32( mpData->maStr, 10 );
}

// -----------------------------------------------------------------------

sal_Int64 UniString::ToInt64() const
{
    DBG_CHKTHIS( UniString, DbgCheckUniString );

    return rtl_ustr_toInt64( mpData->maStr, 10 );
}

// -----------------------------------------------------------------------

float UniString::ToFloat() const
{
    DBG_CHKTHIS( UniString, DbgCheckUniString );

    return rtl_ustr_toFloat( mpData->maStr );
}

// -----------------------------------------------------------------------

double UniString::ToDouble() const
{
    DBG_CHKTHIS( UniString, DbgCheckUniString );

    return rtl_ustr_toDouble( mpData->maStr );
}

