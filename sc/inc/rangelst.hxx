/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: rangelst.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: hr $ $Date: 2005-09-28 11:27:12 $
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

#ifndef SC_RANGELST_HXX
#define SC_RANGELST_HXX

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif

#ifndef SC_ADDRESS_HXX
#include "address.hxx"
#endif
#ifndef _SOLAR_H
#include <tools/solar.h>
#endif

class ScDocument;

typedef ScRange* ScRangePtr;
DECLARE_LIST( ScRangeListBase, ScRangePtr );
class ScRangeList : public ScRangeListBase, public SvRefBase
{
public:
                    ScRangeList() {}
                    ScRangeList( const ScRangeList& rList );
    virtual         ~ScRangeList();
    ScRangeList&    operator=(const ScRangeList& rList);
    void            RemoveAll();
    ScRangeList*    Clone() const;
    void            Append( const ScRange& rRange )
                    {
                        ScRangePtr pR = new ScRange( rRange );
                        Insert( pR, LIST_APPEND );
                    }
    USHORT          Parse( const String&, ScDocument* = NULL,
                            USHORT nMask = SCA_VALID );
    void            Format( String&, USHORT nFlags = 0, ScDocument* = NULL ) const;
    void            Join( const ScRange&, BOOL bIsInList = FALSE );
    BOOL            UpdateReference( UpdateRefMode, ScDocument*,
                                    const ScRange& rWhere,
                                    SCsCOL nDx, SCsROW nDy, SCsTAB nDz );
    ScRange*        Find( const ScAddress& ) const;
    ScRange*        Find( const ScRange& ) const;
    BOOL            Load( SvStream&, USHORT nVer );
    BOOL            Store( SvStream& ) const;
    BOOL            operator==( const ScRangeList& ) const;
    BOOL            Intersects( const ScRange& ) const;
    BOOL            In( const ScRange& ) const;
    ULONG           GetCellCount() const;
};
SV_DECL_IMPL_REF( ScRangeList );


// RangePairList: erster Range (aRange[0]) eigentlicher Range, zweiter
// Range (aRange[1]) Daten zu diesem Range, z.B. Rows eines ColName
DECLARE_LIST( ScRangePairListBase, ScRangePair* );
class ScRangePairList : public ScRangePairListBase, public SvRefBase
{
#if defined( ICC ) && defined( OS2 )
    friend static int _Optlink ICCQsortRPairCompare( const void*, const void*);
#endif
private:
    static int
#ifdef WNT
        __cdecl
#endif
                    QsortNameCompare( const void*, const void* );

public:
    virtual         ~ScRangePairList();
    ScRangePairList*    Clone() const;
    void            Append( const ScRangePair& rRangePair )
                    {
                        ScRangePair* pR = new ScRangePair( rRangePair );
                        Insert( pR, LIST_APPEND );
                    }
    void            Join( const ScRangePair&, BOOL bIsInList = FALSE );
    BOOL            UpdateReference( UpdateRefMode, ScDocument*,
                                    const ScRange& rWhere,
                                    SCsCOL nDx, SCsROW nDy, SCsTAB nDz );
    void            DeleteOnTab( SCTAB nTab );
    ScRangePair*    Find( const ScAddress& ) const;
    ScRangePair*    Find( const ScRange& ) const;
    BOOL            Load( SvStream&, USHORT nVer );
    BOOL            Store( SvStream& ) const;
    ScRangePair**   CreateNameSortedArray( ULONG& nCount, ScDocument* ) const;
    BOOL            operator==( const ScRangePairList& ) const;
};
SV_DECL_IMPL_REF( ScRangePairList );

#if defined( ICC ) && defined( SC_RANGELST_CXX ) && defined( OS2 )
    static int _Optlink ICCQsortRPairCompare( const void* a, const void* b)
                    { ScRangePairList::QsortNameCompare(a,b); }
#endif


#endif
