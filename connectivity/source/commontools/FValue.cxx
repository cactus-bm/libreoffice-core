/*************************************************************************
 *
 *  $RCSfile: FValue.cxx,v $
 *
 *  $Revision: 1.20 $
 *
 *  last change: $Author: fs $ $Date: 2002-09-06 12:11:35 $
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

#include <stdio.h>

#ifndef _CONNECTIVITY_FILE_VALUE_HXX_
#include "connectivity/FValue.hxx"
#endif
#ifndef _CONNECTIVITY_COMMONTOOLS_HXX_
#include "connectivity/CommonTools.hxx"
#endif
#ifndef _DBHELPER_DBCONVERSION_HXX_
#include <connectivity/dbconversion.hxx>
#endif
#ifndef _COM_SUN_STAR_IO_XINPUTSTREAM_HPP_
#include <com/sun/star/io/XInputStream.hpp>
#endif

using namespace connectivity;
using namespace dbtools;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::io;

namespace {
    static sal_Bool isStorageCompatible(sal_Int32 _eType1, sal_Int32 _eType2)
    {
        sal_Bool bIsCompatible = sal_True;

        if (_eType1 != _eType2)
        {
            switch (_eType1)
            {
                case DataType::CHAR:
                case DataType::VARCHAR:
                case DataType::DECIMAL:
                case DataType::NUMERIC:
                case DataType::LONGVARCHAR:
                    bIsCompatible = (DataType::CHAR         == _eType2)
                                ||  (DataType::VARCHAR      == _eType2)
                                ||  (DataType::DECIMAL      == _eType2)
                                ||  (DataType::NUMERIC      == _eType2)
                                ||  (DataType::LONGVARCHAR  == _eType2);
                    break;

                case DataType::DOUBLE:
                case DataType::REAL:
                    bIsCompatible = (DataType::DOUBLE   == _eType2)
                                ||  (DataType::REAL     == _eType2);
                    break;

                case DataType::BINARY:
                case DataType::VARBINARY:
                case DataType::LONGVARBINARY:
                    bIsCompatible = (DataType::BINARY           == _eType2)
                                ||  (DataType::VARBINARY        == _eType2)
                                ||  (DataType::LONGVARBINARY    == _eType2);
                    break;

                case DataType::INTEGER:
                    bIsCompatible = (DataType::SMALLINT == _eType2)
                                ||  (DataType::TINYINT  == _eType2)
                                ||  (DataType::BIT      == _eType2);
                    break;
                case DataType::SMALLINT:
                    bIsCompatible = (DataType::TINYINT  == _eType2)
                                ||  (DataType::BIT      == _eType2);
                    break;
                case DataType::TINYINT:
                    bIsCompatible = (DataType::BIT      == _eType2);
                    break;

                default:
                    bIsCompatible = sal_False;
            }
        }
        return bIsCompatible;
    }
}

// -----------------------------------------------------------------------------
#ifdef DBG_UTIL

#include <vector>
#include <rtl/string.h>

namespace tracing
{
    struct AllocationType
    {
        const sal_Char* pName;
        sal_Int32       nAllocatedUnits;

        AllocationType( ) : pName( NULL ), nAllocatedUnits( 0 ) { }
    };

    // �����������������������������������������������������������������������������
    class AllocationTracer
    {
    public:
        typedef ::std::vector< AllocationType > AllocationState;
        static AllocationState                  s_aAllocated;
        static ::osl::Mutex                     s_aMutex;

    public:
        static void registerUnit( const sal_Char* _pName );
        static void revokeUnit( const sal_Char* _pName );

    private:
        static AllocationState::iterator    getLocation( const sal_Char* _pName );
    };

    // �����������������������������������������������������������������������������
    AllocationTracer::AllocationState::iterator AllocationTracer::getLocation( const sal_Char* _pName )
    {
        AllocationState::iterator aLookFor = s_aAllocated.begin();
        for (   ;
                aLookFor != s_aAllocated.end();
                ++aLookFor
            )
        {
            if ( 0 == rtl_str_compare( aLookFor->pName, _pName ) )
                // found
                return aLookFor;
        }
        // not found
        s_aAllocated.push_back( AllocationType() );
        aLookFor = s_aAllocated.end(); --aLookFor;
        aLookFor->pName = _pName;   // note that this assumes that _pName is a constant string ....
        return aLookFor;
    }

    // �����������������������������������������������������������������������������
    AllocationTracer::AllocationState           AllocationTracer::s_aAllocated;
    ::osl::Mutex                                AllocationTracer::s_aMutex;

    // �����������������������������������������������������������������������������
    void AllocationTracer::registerUnit( const sal_Char* _pName )
    {
        ::osl::MutexGuard aGuard( s_aMutex );

        AllocationState::iterator aPos = getLocation( _pName );
        ++aPos->nAllocatedUnits;
    }

    // �����������������������������������������������������������������������������
    void AllocationTracer::revokeUnit( const sal_Char* _pName )
    {
        ::osl::MutexGuard aGuard( s_aMutex );

        AllocationState::iterator aPos = getLocation( _pName );
        --aPos->nAllocatedUnits;
    }

#define TRACE_ALLOC( type ) tracing::AllocationTracer::registerUnit( #type );
#define TRACE_FREE( type )  tracing::AllocationTracer::revokeUnit( #type );
}
#else
#define TRACE_ALLOC( type )
#define TRACE_FREE( type )
#endif

// -----------------------------------------------------------------------------
void ORowSetValue::setTypeKind(sal_Int32 _eType)
{
    if (!m_bNull)
        if (!isStorageCompatible(_eType, m_eTypeKind))
        {
            switch(_eType)
            {
                case DataType::VARCHAR:
                case DataType::CHAR:
                case DataType::DECIMAL:
                case DataType::NUMERIC:
                case DataType::LONGVARCHAR:
                    (*this) = getString();
                    break;
                case DataType::BIGINT:
                    (*this) = getLong();
                    break;

                case DataType::FLOAT:
                    (*this) = getFloat();
                    break;
                case DataType::DOUBLE:
                case DataType::REAL:
                    (*this) = getDouble();
                    break;
                case DataType::TINYINT:
                    (*this) = getInt8();
                    break;
                case DataType::SMALLINT:
                    (*this) = getInt16();
                    break;
                case DataType::INTEGER:
                    (*this) = getInt32();
                    break;
                case DataType::BIT:
                    (*this) = getBool();
                    break;
                case DataType::DATE:
                    (*this) = getDate();
                    break;
                case DataType::TIME:
                    (*this) = getTime();
                    break;
                case DataType::TIMESTAMP:
                    (*this) = getDateTime();
                    break;
                case DataType::BINARY:
                case DataType::VARBINARY:
                case DataType::LONGVARBINARY:
                    (*this) = getSequence();
                    break;
                default:
                    OSL_ENSURE(0,"ORowSetValue:operator==(): UNSPUPPORTED TYPE!");
            }
        }

    m_eTypeKind = _eType;
}

// -----------------------------------------------------------------------------
void ORowSetValue::free()
{
    if(!m_bNull)
    {
        switch(m_eTypeKind)
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::DECIMAL:
            case DataType::NUMERIC:
            case DataType::LONGVARCHAR:
                OSL_ENSURE(m_aValue.m_pString,"String pointer is null!");
                rtl_uString_release(m_aValue.m_pString);
                m_aValue.m_pString = NULL;
                break;
            case DataType::BIGINT:
                delete (sal_Int64*)m_aValue.m_pValue;
                TRACE_FREE( sal_Int64 )
                m_aValue.m_pValue = NULL;
                break;
            case DataType::FLOAT:
                delete (float*)m_aValue.m_pValue;
                TRACE_FREE( float )
                m_aValue.m_pValue = NULL;
                break;
            case DataType::DOUBLE:
            case DataType::REAL:
                delete (double*)m_aValue.m_pValue;
                TRACE_FREE( double )
                m_aValue.m_pValue = NULL;
                break;
            case DataType::DATE:
                delete (::com::sun::star::util::Date*)m_aValue.m_pValue;
                TRACE_FREE( Date )
                m_aValue.m_pValue = NULL;
                break;
            case DataType::TIME:
                delete (::com::sun::star::util::Time*)m_aValue.m_pValue;
                TRACE_FREE( Time )
                m_aValue.m_pValue = NULL;
                break;
            case DataType::TIMESTAMP:
                delete (::com::sun::star::util::DateTime*)m_aValue.m_pValue;
                TRACE_FREE( DateTime )
                m_aValue.m_pValue = NULL;
                break;
            case DataType::BINARY:
            case DataType::VARBINARY:
            case DataType::LONGVARBINARY:
                delete (Sequence<sal_Int8>*)m_aValue.m_pValue;
                TRACE_FREE( Sequence_sal_Int8 )
                m_aValue.m_pValue = NULL;
                break;
            case DataType::OBJECT:
                delete (Any*)m_aValue.m_pValue;
                TRACE_FREE( Any )
                m_aValue.m_pValue = NULL;
                break;

        }
        m_bNull = sal_True;
    }
}
// -----------------------------------------------------------------------------
ORowSetValue& ORowSetValue::operator=(const ORowSetValue& _rRH)
{
    if(&_rRH == this)
        return *this;

    if(m_eTypeKind != _rRH.m_eTypeKind || _rRH.m_bNull)
        free();

    m_bBound    = _rRH.m_bBound;
    m_eTypeKind = _rRH.m_eTypeKind;

    if(m_bNull && !_rRH.m_bNull)
    {
        switch(_rRH.m_eTypeKind)
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::DECIMAL:
            case DataType::NUMERIC:
            case DataType::LONGVARCHAR:
                rtl_uString_acquire(_rRH.m_aValue.m_pString);
                m_aValue.m_pString = _rRH.m_aValue.m_pString;
                break;
            case DataType::BIGINT:
                m_aValue.m_pValue   = new sal_Int64(*(sal_Int64*)_rRH.m_aValue.m_pValue);
                TRACE_ALLOC( sal_Int64 )
                break;
            case DataType::FLOAT:
                m_aValue.m_pValue   = new float(*(float*)_rRH.m_aValue.m_pValue);
                TRACE_ALLOC( float )
                break;
            case DataType::DOUBLE:
            case DataType::REAL:
                m_aValue.m_pValue   = new double(*(double*)_rRH.m_aValue.m_pValue);
                TRACE_ALLOC( double )
                break;
            case DataType::DATE:
                m_aValue.m_pValue   = new Date(*(Date*)_rRH.m_aValue.m_pValue);
                TRACE_ALLOC( Date )
                break;
            case DataType::TIME:
                m_aValue.m_pValue   = new Time(*(Time*)_rRH.m_aValue.m_pValue);
                TRACE_ALLOC( Time )
                break;
            case DataType::TIMESTAMP:
                m_aValue.m_pValue   = new DateTime(*(DateTime*)_rRH.m_aValue.m_pValue);
                TRACE_ALLOC( DateTime )
                break;
            case DataType::BINARY:
            case DataType::VARBINARY:
            case DataType::LONGVARBINARY:
                m_aValue.m_pValue   = new Sequence<sal_Int8>(*(Sequence<sal_Int8>*)_rRH.m_aValue.m_pValue);
                TRACE_ALLOC( Sequence_sal_Int8 )
                break;
            case DataType::BIT:
                m_aValue.m_bBool    = _rRH.m_aValue.m_bBool;
                break;
            case DataType::TINYINT:
                m_aValue.m_nInt8    = _rRH.m_aValue.m_nInt8;
                break;
            case DataType::SMALLINT:
                m_aValue.m_nInt16   = _rRH.m_aValue.m_nInt16;
                break;
            case DataType::INTEGER:
                m_aValue.m_nInt32   = _rRH.m_aValue.m_nInt32;
                break;
            default:
                m_aValue.m_pValue   = new Any(*(Any*)_rRH.m_aValue.m_pValue);
                TRACE_ALLOC( Any )
        }
    }
    else if(!_rRH.m_bNull)
    {
        switch(_rRH.m_eTypeKind)
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::DECIMAL:
            case DataType::NUMERIC:
            case DataType::LONGVARCHAR:
                (*this) = ::rtl::OUString(_rRH.m_aValue.m_pString);
                break;
            case DataType::BIGINT:
                (*this) = *(sal_Int64*)_rRH.m_aValue.m_pValue;
                break;
            case DataType::FLOAT:
                (*this) = *(float*)_rRH.m_aValue.m_pValue;
                break;
            case DataType::DOUBLE:
            case DataType::REAL:
                (*this) = *(double*)_rRH.m_aValue.m_pValue;
                break;
            case DataType::DATE:
                (*this) = *(Date*)_rRH.m_aValue.m_pValue;
                break;
            case DataType::TIME:
                (*this) = *(Time*)_rRH.m_aValue.m_pValue;
                break;
            case DataType::TIMESTAMP:
                (*this) = *(DateTime*)_rRH.m_aValue.m_pValue;
                break;
            case DataType::BINARY:
            case DataType::VARBINARY:
            case DataType::LONGVARBINARY:
                (*this) = *(Sequence<sal_Int8>*)_rRH.m_aValue.m_pValue;
                break;
            case DataType::BIT:
                m_aValue.m_bBool    = _rRH.m_aValue.m_bBool;
                break;
            case DataType::TINYINT:
                m_aValue.m_nInt8    = _rRH.m_aValue.m_nInt8;
                break;
            case DataType::SMALLINT:
                m_aValue.m_nInt16   = _rRH.m_aValue.m_nInt16;
                break;
            case DataType::INTEGER:
                m_aValue.m_nInt32   = _rRH.m_aValue.m_nInt32;
                break;
            default:
                (*(Any*)m_aValue.m_pValue)  = (*(Any*)_rRH.m_aValue.m_pValue);
        }
    }

    m_bNull     = _rRH.m_bNull;
    // OJ: BUGID: 96277
    m_eTypeKind = _rRH.m_eTypeKind;

    return *this;
}
// -------------------------------------------------------------------------

ORowSetValue& ORowSetValue::operator=(const Date& _rRH)
{
    if(m_eTypeKind != DataType::DATE)
        free();

    if(m_bNull)
    {
        m_aValue.m_pValue = new Date(_rRH);
        TRACE_ALLOC( Date )
        m_eTypeKind = DataType::DATE;
        m_bNull = sal_False;
    }
    else
        *(Date*)m_aValue.m_pValue = _rRH;

    return *this;
}
// -------------------------------------------------------------------------
ORowSetValue& ORowSetValue::operator=(const Time& _rRH)
{
    if(m_eTypeKind != DataType::TIME)
        free();

    if(m_bNull)
    {
        m_aValue.m_pValue = new Time(_rRH);
        TRACE_ALLOC( Time )
        m_eTypeKind = DataType::TIME;
        m_bNull = sal_False;
    }
    else
        *(Time*)m_aValue.m_pValue = _rRH;

    return *this;
}
// -------------------------------------------------------------------------
ORowSetValue& ORowSetValue::operator=(const DateTime& _rRH)
{
    if(m_eTypeKind != DataType::TIMESTAMP)
        free();
    if(m_bNull)
    {
        m_aValue.m_pValue = new DateTime(_rRH);
        TRACE_ALLOC( DateTime )
        m_eTypeKind = DataType::TIMESTAMP;
        m_bNull = sal_False;
    }
    else
        *(DateTime*)m_aValue.m_pValue = _rRH;

    return *this;
}
// -------------------------------------------------------------------------

ORowSetValue& ORowSetValue::operator=(const ::rtl::OUString& _rRH)
{
    if(m_eTypeKind != DataType::VARCHAR || m_aValue.m_pString != _rRH.pData)
    {
        free();
        m_bNull = sal_False;

        m_aValue.m_pString = _rRH.pData;
        rtl_uString_acquire(m_aValue.m_pString);
        m_eTypeKind = DataType::VARCHAR;
    }

    return *this;
}
// -------------------------------------------------------------------------

ORowSetValue& ORowSetValue::operator=(const double& _rRH)
{
    if(m_eTypeKind != DataType::DOUBLE)
        free();

    if(m_bNull)
    {
        m_aValue.m_pValue = new double(_rRH);
        TRACE_ALLOC( double )
        m_eTypeKind = DataType::DOUBLE;
        m_bNull = sal_False;
    }
    else
        *(double*)m_aValue.m_pValue = _rRH;

    return *this;
}
// -----------------------------------------------------------------------------
ORowSetValue& ORowSetValue::operator=(const float& _rRH)
{
    if(m_eTypeKind != DataType::FLOAT)
        free();

    if(m_bNull)
    {
        m_aValue.m_pValue = new float(_rRH);
        TRACE_ALLOC( float )
        m_eTypeKind = DataType::FLOAT;
        m_bNull = sal_False;
    }
    else
        *(float*)m_aValue.m_pValue = _rRH;

    return *this;
}
// -------------------------------------------------------------------------

ORowSetValue& ORowSetValue::operator=(const sal_Int8& _rRH)
{
    if(m_eTypeKind != DataType::TINYINT)
        free();

    m_aValue.m_nInt8 = _rRH;
    m_eTypeKind = DataType::TINYINT;
    m_bNull = sal_False;
    return *this;
}
// -------------------------------------------------------------------------

ORowSetValue& ORowSetValue::operator=(const sal_Int16& _rRH)
{
    if(m_eTypeKind != DataType::SMALLINT)
        free();

    m_aValue.m_nInt16 = _rRH;
    m_eTypeKind = DataType::SMALLINT;
    m_bNull = sal_False;

    return *this;
}
// -------------------------------------------------------------------------

ORowSetValue& ORowSetValue::operator=(const sal_Int32& _rRH)
{
    if(m_eTypeKind != DataType::INTEGER)
        free();

    m_aValue.m_nInt32 = _rRH;
    m_eTypeKind = DataType::INTEGER;
    m_bNull = sal_False;

    return *this;
}
// -------------------------------------------------------------------------

ORowSetValue& ORowSetValue::operator=(const sal_Bool _rRH)
{
    if(m_eTypeKind != DataType::BIT)
        free();

    m_aValue.m_bBool = _rRH;
    m_eTypeKind = DataType::BIT;
    m_bNull = sal_False;

    return *this;
}
// -------------------------------------------------------------------------
ORowSetValue& ORowSetValue::operator=(const sal_Int64& _rRH)
{
    if (DataType::BIGINT != m_eTypeKind)
        free();

    if(m_bNull)
    {
        m_aValue.m_pValue = new sal_Int64(_rRH);
        TRACE_ALLOC( sal_Int64 )
    }
    else
        *static_cast<sal_Int64*>(m_aValue.m_pValue) = _rRH;

    m_eTypeKind = DataType::BIGINT;
    m_bNull = sal_False;

    return *this;
}
// -------------------------------------------------------------------------
ORowSetValue& ORowSetValue::operator=(const Sequence<sal_Int8>& _rRH)
{
    if (!isStorageCompatible(DataType::LONGVARBINARY,m_eTypeKind))
        free();

    if (m_bNull)
    {
        m_aValue.m_pValue = new Sequence<sal_Int8>(_rRH);
        TRACE_ALLOC( Sequence_sal_Int8 )
    }
    else
        *static_cast< Sequence< sal_Int8 >* >(m_aValue.m_pValue) = _rRH;

    m_eTypeKind = DataType::LONGVARBINARY;
    m_bNull = sal_False;

    return *this;
}
// -------------------------------------------------------------------------
ORowSetValue& ORowSetValue::operator=(const Any& _rAny)
{
    if (DataType::OBJECT != m_eTypeKind && !m_bNull)
        free();

    if(m_bNull)
    {
        m_aValue.m_pValue = new Any(_rAny);
        TRACE_ALLOC( Any )
    }
    else
        *static_cast<Any*>(m_aValue.m_pValue) = _rAny;

    m_eTypeKind = DataType::OBJECT;
    m_bNull = sal_False;

    return *this;
}
// -------------------------------------------------------------------------

sal_Bool operator==(const Date& _rLH,const Date& _rRH)
{
    return _rLH.Day == _rRH.Day && _rLH.Month == _rRH.Month && _rLH.Year == _rRH.Year;
}
// -------------------------------------------------------------------------

sal_Bool operator==(const Time& _rLH,const Time& _rRH)
{
    return _rLH.Minutes == _rRH.Minutes && _rLH.Hours == _rRH.Hours && _rLH.Seconds == _rRH.Seconds && _rLH.HundredthSeconds == _rRH.HundredthSeconds;
}
// -------------------------------------------------------------------------

sal_Bool operator==(const DateTime& _rLH,const DateTime& _rRH)
{
    return _rLH.Day == _rRH.Day && _rLH.Month == _rRH.Month && _rLH.Year == _rRH.Year &&
        _rLH.Minutes == _rRH.Minutes && _rLH.Hours == _rRH.Hours && _rLH.Seconds == _rRH.Seconds && _rLH.HundredthSeconds == _rRH.HundredthSeconds;
}
// -------------------------------------------------------------------------

ORowSetValue::operator==(const ORowSetValue& _rRH) const
{
    if(m_eTypeKind != _rRH.m_eTypeKind)
        return sal_False;
    if(m_bNull != _rRH.isNull())
        return sal_False;
    if(m_bNull && _rRH.isNull())
        return sal_True;

    sal_Bool bRet = sal_False;
    OSL_ENSURE(!m_bNull,"SHould not be null!");
    switch(m_eTypeKind)
    {
        case DataType::VARCHAR:
        case DataType::CHAR:
        case DataType::DECIMAL:
        case DataType::NUMERIC:
        case DataType::LONGVARCHAR:
        {
            ::rtl::OUString aVal1(m_aValue.m_pString);
            ::rtl::OUString aVal2(_rRH.m_aValue.m_pString);
            bRet = aVal1 == aVal2;
            break;
        }
        case DataType::BIGINT:
            bRet = *(sal_Int64*)m_aValue.m_pValue == *(sal_Int64*)_rRH.m_aValue.m_pValue;
            break;
        case DataType::FLOAT:
            bRet = *(float*)m_aValue.m_pValue == *(float*)_rRH.m_aValue.m_pValue;
            break;
        case DataType::DOUBLE:
        case DataType::REAL:
            bRet = *(double*)m_aValue.m_pValue == *(double*)_rRH.m_aValue.m_pValue;
            break;
        case DataType::TINYINT:
            bRet = m_aValue.m_nInt8 == _rRH.m_aValue.m_nInt8;
            break;
        case DataType::SMALLINT:
            bRet = m_aValue.m_nInt16 == _rRH.m_aValue.m_nInt16;
            break;
        case DataType::INTEGER:
            bRet = m_aValue.m_nInt32 == _rRH.m_aValue.m_nInt32;
            break;
        case DataType::BIT:
            bRet = m_aValue.m_bBool == _rRH.m_aValue.m_bBool;
            break;
        case DataType::DATE:
            bRet = *(Date*)m_aValue.m_pValue == *(Date*)_rRH.m_aValue.m_pValue;
            break;
        case DataType::TIME:
            bRet = *(Time*)m_aValue.m_pValue == *(Time*)_rRH.m_aValue.m_pValue;
            break;
        case DataType::TIMESTAMP:
            bRet = *(DateTime*)m_aValue.m_pValue == *(DateTime*)_rRH.m_aValue.m_pValue;
            break;
        case DataType::BINARY:
        case DataType::VARBINARY:
        case DataType::LONGVARBINARY:
            bRet = sal_False;
            break;
        default:
            OSL_ENSURE(0,"ORowSetValue::operator==(): UNSPUPPORTED TYPE!");
    }
    return bRet;
}
// -------------------------------------------------------------------------
Any ORowSetValue::makeAny() const
{
    Any rValue;
    if(isBound() && !isNull())
    {
        switch(getTypeKind())
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::DECIMAL:
            case DataType::NUMERIC:
            case DataType::LONGVARCHAR:
                OSL_ENSURE(m_aValue.m_pString,"Value is null!");
                rValue <<= (::rtl::OUString)m_aValue.m_pString;
                break;
            case DataType::BIGINT:
                OSL_ENSURE(m_aValue.m_pValue,"Value is null!");
                rValue <<= *(sal_Int64*)m_aValue.m_pValue;
                break;
            case DataType::FLOAT:
                OSL_ENSURE(m_aValue.m_pValue,"Value is null!");
                rValue <<= *(float*)m_aValue.m_pValue;
                break;
            case DataType::DOUBLE:
            case DataType::REAL:
                OSL_ENSURE(m_aValue.m_pValue,"Value is null!");
                rValue <<= *(double*)m_aValue.m_pValue;
                break;
            case DataType::DATE:
                OSL_ENSURE(m_aValue.m_pValue,"Value is null!");
                rValue <<= *(Date*)m_aValue.m_pValue;
                break;
            case DataType::TIME:
                OSL_ENSURE(m_aValue.m_pValue,"Value is null!");
                rValue <<= *(Time*)m_aValue.m_pValue;
                break;
            case DataType::TIMESTAMP:
                OSL_ENSURE(m_aValue.m_pValue,"Value is null!");
                rValue <<= *(DateTime*)m_aValue.m_pValue;
                break;
            case DataType::BINARY:
            case DataType::VARBINARY:
            case DataType::LONGVARBINARY:
                OSL_ENSURE(m_aValue.m_pValue,"Value is null!");
                rValue <<= *(Sequence<sal_Int8>*)m_aValue.m_pValue;
                break;
            case DataType::OBJECT:
                rValue = getAny();
                break;
            case DataType::BIT:
                rValue.setValue( &m_aValue.m_bBool, ::getCppuBooleanType() );
                break;
            case DataType::TINYINT:
                rValue <<= m_aValue.m_nInt8;
                break;
            case DataType::SMALLINT:
                rValue <<= m_aValue.m_nInt16;
                break;
            case DataType::INTEGER:
                rValue <<= m_aValue.m_nInt32;
                break;
            default:
                OSL_ENSURE(0,"ORowSetValue::makeAny(): UNSPUPPORTED TYPE!");
        }
    }
    return rValue;
}
// -------------------------------------------------------------------------
::rtl::OUString ORowSetValue::getString( ) const
{
    ::rtl::OUString aRet;
    if(!m_bNull)
    {
        switch(getTypeKind())
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::DECIMAL:
            case DataType::NUMERIC:
            case DataType::LONGVARCHAR:
                aRet = m_aValue.m_pString;
                break;
            case DataType::BIGINT:
                aRet = ::rtl::OUString::valueOf((sal_Int64)*this);
                break;
            case DataType::FLOAT:
                aRet = ::rtl::OUString::valueOf((float)*this);
                break;
            case DataType::DOUBLE:
            case DataType::REAL:
                aRet = ::rtl::OUString::valueOf((double)*this);
                break;
            case DataType::DATE:
                aRet = connectivity::toDateString(*this);
                break;
            case DataType::TIME:
                aRet = connectivity::toTimeString(*this);
                break;
            case DataType::TIMESTAMP:
                aRet = connectivity::toDateTimeString(*this);
                break;
            case DataType::BINARY:
            case DataType::VARBINARY:
            case DataType::LONGVARBINARY:
                {
                    aRet = ::rtl::OUString::createFromAscii("0x");
                    Sequence<sal_Int8> aSeq(getSequence());
                    const sal_Int8* pBegin  = aSeq.getConstArray();
                    const sal_Int8* pEnd    = pBegin + aSeq.getLength();
                    for(;pBegin != pEnd;++pBegin)
                        aRet += ::rtl::OUString::valueOf((sal_Int32)*pBegin,16);
                }
                break;
            case DataType::BIT:
                aRet = ::rtl::OUString::valueOf((sal_Int32)(sal_Bool)*this);
                break;
            case DataType::TINYINT:
                aRet = ::rtl::OUString::valueOf((sal_Int32)(sal_Int8)*this);
                break;
            case DataType::SMALLINT:
                aRet = ::rtl::OUString::valueOf((sal_Int32)(sal_Int16)*this);
                break;
            case DataType::INTEGER:
                aRet = ::rtl::OUString::valueOf((sal_Int32)*this);
                break;
        }
    }
    return aRet;
}
// -------------------------------------------------------------------------
sal_Bool ORowSetValue::getBool()    const
{
    sal_Bool bRet = sal_False;
    if(!m_bNull)
    {
        switch(getTypeKind())
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::DECIMAL:
            case DataType::NUMERIC:
            case DataType::LONGVARCHAR:
                bRet = ::rtl::OUString(m_aValue.m_pString).toInt32() != 0;
                break;
            case DataType::BIGINT:
                bRet = *(sal_Int64*)m_aValue.m_pValue != 0.0;
                break;
            case DataType::FLOAT:
                bRet = *(float*)m_aValue.m_pValue != 0.0;
                break;
            case DataType::DOUBLE:
            case DataType::REAL:
                bRet = *(double*)m_aValue.m_pValue != 0.0;
                break;
            case DataType::DATE:
            case DataType::TIME:
            case DataType::TIMESTAMP:
            case DataType::BINARY:
            case DataType::VARBINARY:
            case DataType::LONGVARBINARY:
                OSL_ASSERT(!"getBool() for this type is not allowed!");
                break;
            case DataType::BIT:
                bRet = m_aValue.m_bBool;
                break;
            case DataType::TINYINT:
                bRet = m_aValue.m_nInt8  != 0;
                break;
            case DataType::SMALLINT:
                bRet = m_aValue.m_nInt16 != 0;
                break;
            case DataType::INTEGER:
                bRet = m_aValue.m_nInt32 != 0;
                break;
        }
    }
    return bRet;
}
// -------------------------------------------------------------------------
sal_Int8 ORowSetValue::getInt8()    const
{


    sal_Int8 nRet = 0;
    if(!m_bNull)
    {
        switch(getTypeKind())
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::DECIMAL:
            case DataType::NUMERIC:
            case DataType::LONGVARCHAR:
                nRet = sal_Int8(::rtl::OUString(m_aValue.m_pString).toInt32());
                break;
            case DataType::BIGINT:
                nRet = sal_Int8(*(sal_Int64*)m_aValue.m_pValue);
                break;
            case DataType::FLOAT:
                nRet = sal_Int8(*(float*)m_aValue.m_pValue);
                break;
            case DataType::DOUBLE:
            case DataType::REAL:
                nRet = sal_Int8(*(double*)m_aValue.m_pValue);
                break;
            case DataType::DATE:
            case DataType::TIME:
            case DataType::TIMESTAMP:
            case DataType::BINARY:
            case DataType::VARBINARY:
            case DataType::LONGVARBINARY:
                OSL_ASSERT(!"getInt8() for this type is not allowed!");
                break;
            case DataType::BIT:
                nRet = m_aValue.m_bBool;
                break;
            case DataType::TINYINT:
                nRet = m_aValue.m_nInt8;
                break;
            case DataType::SMALLINT:
                nRet = sal_Int8(m_aValue.m_nInt16);
                break;
            case DataType::INTEGER:
                nRet = sal_Int8(m_aValue.m_nInt32);
                break;
        }
    }
    return nRet;
}
// -------------------------------------------------------------------------
sal_Int16 ORowSetValue::getInt16()  const
{


    sal_Int16 nRet = 0;
    if(!m_bNull)
    {
        switch(getTypeKind())
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::DECIMAL:
            case DataType::NUMERIC:
            case DataType::LONGVARCHAR:
                nRet = sal_Int16(::rtl::OUString(m_aValue.m_pString).toInt32());
                break;
            case DataType::BIGINT:
                nRet = sal_Int16(*(sal_Int64*)m_aValue.m_pValue);
                break;
            case DataType::FLOAT:
                nRet = sal_Int16(*(float*)m_aValue.m_pValue);
                break;
            case DataType::DOUBLE:
            case DataType::REAL:
                nRet = sal_Int16(*(double*)m_aValue.m_pValue);
                break;
            case DataType::DATE:
            case DataType::TIME:
            case DataType::TIMESTAMP:
            case DataType::BINARY:
            case DataType::VARBINARY:
            case DataType::LONGVARBINARY:
                OSL_ASSERT(!"getInt16() for this type is not allowed!");
                break;
            case DataType::BIT:
                nRet = m_aValue.m_bBool;
                break;
            case DataType::TINYINT:
                nRet = m_aValue.m_nInt8;
                break;
            case DataType::SMALLINT:
                nRet = m_aValue.m_nInt16;
                break;
            case DataType::INTEGER:
                nRet = (sal_Int16)m_aValue.m_nInt32;
                break;
        }
    }
    return nRet;
}
// -------------------------------------------------------------------------
sal_Int32 ORowSetValue::getInt32()  const
{
    sal_Int32 nRet = 0;
    if(!m_bNull)
    {
        switch(getTypeKind())
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::DECIMAL:
            case DataType::NUMERIC:
            case DataType::LONGVARCHAR:
                nRet = ::rtl::OUString(m_aValue.m_pString).toInt32();
                break;
            case DataType::BIGINT:
                nRet = sal_Int32(*(sal_Int64*)m_aValue.m_pValue);
                break;
            case DataType::FLOAT:
                nRet = sal_Int32(*(float*)m_aValue.m_pValue);
                break;
            case DataType::DOUBLE:
            case DataType::REAL:
                nRet = sal_Int32(*(double*)m_aValue.m_pValue);
                break;
            case DataType::DATE:
                nRet = dbtools::DBTypeConversion::toDays(*(::com::sun::star::util::Date*)m_aValue.m_pValue);
                break;
            case DataType::TIME:
            case DataType::TIMESTAMP:
            case DataType::BINARY:
            case DataType::VARBINARY:
            case DataType::LONGVARBINARY:
                OSL_ASSERT(!"getInt32() for this type is not allowed!");
                break;
            case DataType::BIT:
                nRet = m_aValue.m_bBool;
                break;
            case DataType::TINYINT:
                nRet = m_aValue.m_nInt8;
                break;
            case DataType::SMALLINT:
                nRet = m_aValue.m_nInt16;
                break;
            case DataType::INTEGER:
                nRet = m_aValue.m_nInt32;
                break;
        }
    }
    return nRet;
}
// -------------------------------------------------------------------------
sal_Int64 ORowSetValue::getLong()   const
{
    sal_Int64 nRet = 0;
    if(!m_bNull)
    {
        switch(getTypeKind())
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::DECIMAL:
            case DataType::NUMERIC:
            case DataType::LONGVARCHAR:
                nRet = ::rtl::OUString(m_aValue.m_pString).toInt64();
                break;
            case DataType::BIGINT:
                nRet = *(sal_Int64*)m_aValue.m_pValue;
                break;
            case DataType::FLOAT:
                nRet = sal_Int64(*(float*)m_aValue.m_pValue);
                break;
            case DataType::DOUBLE:
            case DataType::REAL:
                nRet = sal_Int64(*(double*)m_aValue.m_pValue);
                break;
            case DataType::DATE:
                nRet = dbtools::DBTypeConversion::toDays(*(::com::sun::star::util::Date*)m_aValue.m_pValue);
                break;
            case DataType::TIME:
            case DataType::TIMESTAMP:
            case DataType::BINARY:
            case DataType::VARBINARY:
            case DataType::LONGVARBINARY:
                OSL_ASSERT(!"getInt32() for this type is not allowed!");
                break;
            case DataType::BIT:
                nRet = m_aValue.m_bBool;
                break;
            case DataType::TINYINT:
                nRet = m_aValue.m_nInt8;
                break;
            case DataType::SMALLINT:
                nRet = m_aValue.m_nInt16;
                break;
            case DataType::INTEGER:
                nRet = m_aValue.m_nInt32;
                break;
        }
    }
    return nRet;
}
// -------------------------------------------------------------------------
float ORowSetValue::getFloat()  const
{
    float nRet = 0;
    if(!m_bNull)
    {
        switch(getTypeKind())
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::DECIMAL:
            case DataType::NUMERIC:
            case DataType::LONGVARCHAR:
                nRet = ::rtl::OUString(m_aValue.m_pString).toFloat();
                break;
            case DataType::BIGINT:
                nRet = float(*(sal_Int64*)m_aValue.m_pValue);
                break;
            case DataType::FLOAT:
                nRet = *(float*)m_aValue.m_pValue;
                break;
            case DataType::DOUBLE:
            case DataType::REAL:
                nRet = (float)*(double*)m_aValue.m_pValue;
                break;
            case DataType::DATE:
                nRet = (float)dbtools::DBTypeConversion::toDouble(*(::com::sun::star::util::Date*)m_aValue.m_pValue);
                break;
            case DataType::TIME:
                nRet = (float)dbtools::DBTypeConversion::toDouble(*(::com::sun::star::util::Time*)m_aValue.m_pValue);
                break;
            case DataType::TIMESTAMP:
                nRet = (float)dbtools::DBTypeConversion::toDouble(*(::com::sun::star::util::DateTime*)m_aValue.m_pValue);
                break;
            case DataType::BINARY:
            case DataType::VARBINARY:
            case DataType::LONGVARBINARY:
                OSL_ASSERT(!"getDouble() for this type is not allowed!");
                break;
            case DataType::BIT:
                nRet = m_aValue.m_bBool;
                break;
            case DataType::TINYINT:
                nRet = m_aValue.m_nInt8;
                break;
            case DataType::SMALLINT:
                nRet = m_aValue.m_nInt16;
                break;
            case DataType::INTEGER:
                nRet = (float)m_aValue.m_nInt32;
                break;
        }
    }
    return nRet;
}
// -------------------------------------------------------------------------
double ORowSetValue::getDouble()    const
{


    double nRet = 0;
    if(!m_bNull)
    {
        switch(getTypeKind())
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::DECIMAL:
            case DataType::NUMERIC:
            case DataType::LONGVARCHAR:
                nRet = ::rtl::OUString(m_aValue.m_pString).toDouble();
                break;
            case DataType::BIGINT:
                nRet = double(*(sal_Int64*)m_aValue.m_pValue);
                break;
            case DataType::FLOAT:
                nRet = *(float*)m_aValue.m_pValue;
                break;
            case DataType::DOUBLE:
            case DataType::REAL:
                nRet = *(double*)m_aValue.m_pValue;
                break;
            case DataType::DATE:
                nRet = dbtools::DBTypeConversion::toDouble(*(::com::sun::star::util::Date*)m_aValue.m_pValue);
                break;
            case DataType::TIME:
                nRet = dbtools::DBTypeConversion::toDouble(*(::com::sun::star::util::Time*)m_aValue.m_pValue);
                break;
            case DataType::TIMESTAMP:
                nRet = dbtools::DBTypeConversion::toDouble(*(::com::sun::star::util::DateTime*)m_aValue.m_pValue);
                break;
            case DataType::BINARY:
            case DataType::VARBINARY:
            case DataType::LONGVARBINARY:
                OSL_ASSERT(!"getDouble() for this type is not allowed!");
                break;
            case DataType::BIT:
                nRet = m_aValue.m_bBool;
                break;
            case DataType::TINYINT:
                nRet = m_aValue.m_nInt8;
                break;
            case DataType::SMALLINT:
                nRet = m_aValue.m_nInt16;
                break;
            case DataType::INTEGER:
                nRet = m_aValue.m_nInt32;
                break;
        }
    }
    return nRet;
}
// -------------------------------------------------------------------------
void ORowSetValue::setFromDouble(const double& _rVal,sal_Int32 _nDatatype)
{
    free();

    m_bNull = sal_False;
    switch(_nDatatype)
    {
        case DataType::CHAR:
        case DataType::VARCHAR:
        case DataType::DECIMAL:
        case DataType::NUMERIC:
        case DataType::LONGVARCHAR:
            {
                ::rtl::OUString aVal = ::rtl::OUString::valueOf(_rVal);
                m_aValue.m_pString = aVal.pData;
                rtl_uString_acquire(m_aValue.m_pString);
            }
            break;
        case DataType::BIGINT:
            m_aValue.m_pValue = new sal_Int64((sal_Int64)_rVal);
            TRACE_ALLOC( sal_Int64 )
            break;
        case DataType::FLOAT:
            m_aValue.m_pValue = new float((float)_rVal);
            TRACE_ALLOC( float )
            break;
        case DataType::DOUBLE:
        case DataType::REAL:
            m_aValue.m_pValue = new double(_rVal);
            TRACE_ALLOC( double )
            break;
        case DataType::DATE:
            m_aValue.m_pValue = new Date(dbtools::DBTypeConversion::toDate(_rVal));
            TRACE_ALLOC( Date )
            break;
        case DataType::TIME:
            m_aValue.m_pValue = new Time(dbtools::DBTypeConversion::toTime(_rVal));
            TRACE_ALLOC( Time )
            break;
        case DataType::TIMESTAMP:
            m_aValue.m_pValue = new DateTime(dbtools::DBTypeConversion::toDateTime(_rVal));
            TRACE_ALLOC( DateTime )
            break;
        case DataType::BINARY:
        case DataType::VARBINARY:
        case DataType::LONGVARBINARY:
            OSL_ASSERT(!"setFromDouble() for this type is not allowed!");
            break;
        case DataType::BIT:
            m_aValue.m_bBool = _rVal != 0.0;
            break;
        case DataType::TINYINT:
            m_aValue.m_nInt8 = sal_Int8(_rVal);
            break;
        case DataType::SMALLINT:
            m_aValue.m_nInt16 = sal_Int16(_rVal);
            break;
        case DataType::INTEGER:
            m_aValue.m_nInt32 = sal_Int32(_rVal);
            break;
    }
    m_eTypeKind = _nDatatype;
}
// -----------------------------------------------------------------------------
Sequence<sal_Int8>  ORowSetValue::getSequence() const
{
    Sequence<sal_Int8> aSeq;
    if (!m_bNull)
    {
        switch(m_eTypeKind)
        {
            case DataType::OBJECT:
            case DataType::CLOB:
            case DataType::BLOB:
            {
                Reference<XInputStream> xStream;
                Any aValue = getAny();
                if(aValue.hasValue())
                {
                    aValue >>= xStream;
                    if(xStream.is())
                        xStream->readBytes(aSeq,xStream->available());
                }
            }
            break;
            case DataType::VARCHAR:
            case DataType::LONGVARCHAR:
                {
                    ::rtl::OUString sVal(m_aValue.m_pString);
                    aSeq = Sequence<sal_Int8>(reinterpret_cast<const sal_Int8*>(sVal.getStr()),sizeof(sal_Unicode)*sVal.getLength());
                }
                break;
            case DataType::BINARY:
            case DataType::VARBINARY:
            case DataType::LONGVARBINARY:
                aSeq = *static_cast< Sequence<sal_Int8>*>(m_aValue.m_pValue);
                break;
            default:
                ;
        }
    }
    return aSeq;

}
// -----------------------------------------------------------------------------
::com::sun::star::util::Date ORowSetValue::getDate()        const
{
    ::com::sun::star::util::Date aValue;
    if(!m_bNull)
    {
        switch(m_eTypeKind)
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::LONGVARCHAR:
                aValue = DBTypeConversion::toDate(getString());
                break;
            case DataType::DECIMAL:
            case DataType::NUMERIC:
                aValue = DBTypeConversion::toDate((double)*this);
                break;
            case DataType::FLOAT:
            case DataType::DOUBLE:
            case DataType::REAL:
                aValue = DBTypeConversion::toDate((double)*this);
                break;

            case DataType::DATE:
                aValue = *static_cast< ::com::sun::star::util::Date*>(m_aValue.m_pValue);
                break;
            case DataType::TIMESTAMP:
                {
                    ::com::sun::star::util::DateTime* pDateTime = static_cast< ::com::sun::star::util::DateTime*>(m_aValue.m_pValue);
                    aValue.Day      = pDateTime->Day;
                    aValue.Month    = pDateTime->Month;
                    aValue.Year     = pDateTime->Year;
                }
                break;
        }
    }
    return aValue;
}
// -----------------------------------------------------------------------------
::com::sun::star::util::Time ORowSetValue::getTime()        const
{
    ::com::sun::star::util::Time aValue;
    if(!m_bNull)
    {
        switch(m_eTypeKind)
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::LONGVARCHAR:
                aValue = DBTypeConversion::toTime(getString());
                break;
            case DataType::DECIMAL:
            case DataType::NUMERIC:
                aValue = DBTypeConversion::toTime((double)*this);
                break;
            case DataType::FLOAT:
            case DataType::DOUBLE:
            case DataType::REAL:
                aValue = DBTypeConversion::toTime((double)*this);
                break;
            case DataType::TIMESTAMP:
                {
                    ::com::sun::star::util::DateTime* pDateTime = static_cast< ::com::sun::star::util::DateTime*>(m_aValue.m_pValue);
                    aValue.HundredthSeconds = pDateTime->HundredthSeconds;
                    aValue.Seconds          = pDateTime->Seconds;
                    aValue.Minutes          = pDateTime->Minutes;
                    aValue.Hours            = pDateTime->Hours;
                }
                break;
            case DataType::TIME:
                aValue = *static_cast< ::com::sun::star::util::Time*>(m_aValue.m_pValue);
        }
    }
    return aValue;
}
// -----------------------------------------------------------------------------
::com::sun::star::util::DateTime ORowSetValue::getDateTime()    const
{
    ::com::sun::star::util::DateTime aValue;
    if(!m_bNull)
    {
        switch(m_eTypeKind)
        {
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::LONGVARCHAR:
                aValue = DBTypeConversion::toDateTime(getString());
                break;
            case DataType::DECIMAL:
            case DataType::NUMERIC:
                aValue = DBTypeConversion::toDateTime((double)*this);
                break;
            case DataType::FLOAT:
            case DataType::DOUBLE:
            case DataType::REAL:
                aValue = DBTypeConversion::toDateTime((double)*this);
                break;
            case DataType::DATE:
                {
                    ::com::sun::star::util::Date* pDate = static_cast< ::com::sun::star::util::Date*>(m_aValue.m_pValue);
                    aValue.Day      = pDate->Day;
                    aValue.Month    = pDate->Month;
                    aValue.Year     = pDate->Year;
                }
                break;
            case DataType::TIME:
                {
                    ::com::sun::star::util::Time* pTime = static_cast< ::com::sun::star::util::Time*>(m_aValue.m_pValue);
                    aValue.HundredthSeconds = pTime->HundredthSeconds;
                    aValue.Seconds          = pTime->Seconds;
                    aValue.Minutes          = pTime->Minutes;
                    aValue.Hours            = pTime->Hours;
                }
                break;
            case DataType::TIMESTAMP:
                aValue = *static_cast< ::com::sun::star::util::DateTime*>(m_aValue.m_pValue);
                break;
        }
    }
    return aValue;
}
// -----------------------------------------------------------------------------



