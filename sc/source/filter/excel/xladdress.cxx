/*************************************************************************
 *
 *  $RCSfile: xladdress.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2005-02-21 13:34:22 $
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

#ifndef SC_XLADDRESS_HXX
#include "xladdress.hxx"
#endif

#ifndef SC_XLTRACER_HXX
#include "xltracer.hxx"
#endif
#ifndef SC_XISTREAM_HXX
#include "xistream.hxx"
#endif
#ifndef SC_XESTREAM_HXX
#include "xestream.hxx"
#endif

// ============================================================================

void XclAddress::Read( XclImpStream& rStrm, bool bCol16Bit )
{
    rStrm >> mnRow;
    if( bCol16Bit )
        rStrm >> mnCol;
    else
        mnCol = rStrm.ReaduInt8();
}

void XclAddress::Write( XclExpStream& rStrm, bool bCol16Bit ) const
{
    rStrm << mnRow;
    if( bCol16Bit )
        rStrm << mnCol;
    else
        rStrm << static_cast< sal_uInt8 >( mnCol );
}

// ----------------------------------------------------------------------------

bool XclRange::Contains( const XclAddress& rPos ) const
{
    return  (maFirst.mnCol <= rPos.mnCol) && (rPos.mnCol <= maLast.mnCol) &&
            (maFirst.mnRow <= rPos.mnRow) && (rPos.mnRow <= maLast.mnRow);
}

void XclRange::Read( XclImpStream& rStrm, bool bCol16Bit )
{
    rStrm >> maFirst.mnRow >> maLast.mnRow;
    if( bCol16Bit )
        rStrm >> maFirst.mnCol >> maLast.mnCol;
    else
    {
        maFirst.mnCol = rStrm.ReaduInt8();
        maLast.mnCol = rStrm.ReaduInt8();
    }
}

void XclRange::Write( XclExpStream& rStrm, bool bCol16Bit ) const
{
    rStrm << maFirst.mnRow << maLast.mnRow;
    if( bCol16Bit )
        rStrm << maFirst.mnCol << maLast.mnCol;
    else
        rStrm << static_cast< sal_uInt8 >( maFirst.mnCol ) << static_cast< sal_uInt8 >( maLast.mnCol );
}

// ----------------------------------------------------------------------------

XclRange XclRangeList::GetEnclosingRange() const
{
    XclRange aXclRange( ScAddress::UNINITIALIZED );
    if( !empty() )
    {
        const_iterator aIt = begin(), aEnd = end();
        aXclRange = *aIt;
        for( ++aIt; aIt != aEnd; ++aIt )
        {
            aXclRange.maFirst.mnCol = ::std::min( aXclRange.maFirst.mnCol, aIt->maFirst.mnCol );
            aXclRange.maFirst.mnRow = ::std::min( aXclRange.maFirst.mnRow, aIt->maFirst.mnRow );
            aXclRange.maLast.mnCol = ::std::max( aXclRange.maLast.mnCol, aIt->maLast.mnCol );
            aXclRange.maLast.mnRow = ::std::max( aXclRange.maLast.mnRow, aIt->maLast.mnRow );
        }
    }
    return aXclRange;
}

void XclRangeList::Read( XclImpStream& rStrm, bool bCol16Bit )
{
    sal_uInt16 nCount;
    rStrm >> nCount;
    size_t nOldSize = size();
    resize( nOldSize + nCount );
    for( iterator aIt = begin() + nOldSize; rStrm.IsValid() && (nCount > 0); --nCount, ++aIt )
        aIt->Read( rStrm, bCol16Bit );
}

void XclRangeList::Write( XclExpStream& rStrm, bool bCol16Bit ) const
{
    WriteSubList( rStrm, 0, size(), bCol16Bit );
}

void XclRangeList::WriteSubList( XclExpStream& rStrm, size_t nBegin, size_t nCount, bool bCol16Bit ) const
{
    DBG_ASSERT( nBegin <= size(), "XclRangeList::WriteSubList - invalid start position" );
    size_t nEnd = ::std::min< size_t >( nBegin + nCount, size() );
    sal_uInt16 nXclCount = ulimit_cast< sal_uInt16 >( nEnd - nBegin );
    rStrm << nXclCount;
    rStrm.SetSliceSize( bCol16Bit ? 8 : 6 );
    for( const_iterator aIt = begin() + nBegin, aEnd = begin() + nEnd; aIt != aEnd; ++aIt )
        aIt->Write( rStrm, bCol16Bit );
}

// ============================================================================

XclAddressConverterBase::XclAddressConverterBase( XclTracer& rTracer, const ScAddress& rMaxPos ) :
    mrTracer( rTracer ),
    maMaxPos( rMaxPos ),
    mnMaxCol( static_cast< sal_uInt16 >( rMaxPos.Col() ) ),
    mnMaxRow( static_cast< sal_uInt16 >( rMaxPos.Row() ) ),
    mbColTrunc( false ),
    mbRowTrunc( false ),
    mbTabTrunc( false )
{
    DBG_ASSERT( static_cast< size_t >( rMaxPos.Col() ) <= SAL_MAX_UINT16, "XclAddressConverterBase::XclAddressConverterBase - invalid max column" );
    DBG_ASSERT( static_cast< size_t >( rMaxPos.Row() ) <= SAL_MAX_UINT16, "XclAddressConverterBase::XclAddressConverterBase - invalid max row" );
}

XclAddressConverterBase::~XclAddressConverterBase()
{
}

bool XclAddressConverterBase::CheckScTab( SCTAB nScTab, bool bWarn )
{
    bool bValid = (0 <= nScTab) && (nScTab <= maMaxPos.Tab());
    if( !bValid && bWarn )
    {
        mbTabTrunc |= (nScTab > maMaxPos.Tab());  // do not warn for deleted refs
        mrTracer.TraceInvalidTab( nScTab, maMaxPos.Tab() );
    }
    return bValid;
}

// ============================================================================

