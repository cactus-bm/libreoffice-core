/*************************************************************************
 *
 *  $RCSfile: colrowst.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: dr $ $Date: 2001-05-10 17:26:23 $
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

#ifndef _COLROWST_HXX
#define _COLROWST_HXX

#ifndef _TOOLS_DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif

#ifndef _SCEXTOPT_HXX
#include "scextopt.hxx"
#endif
#ifndef _ROOT_HXX
#include "root.hxx"
#endif
#ifndef _FLTTOOLS_HXX
#include "flttools.hxx"
#endif


// ----- forwards --------------------------------------------------------

class XF_Buffer;
class XclImpStream;

// ----------------------------------------------- class ColRowSettings --

#define ROWFLAG_USED        0x01
#define ROWFLAG_DEFAULT     0x02
#define ROWFLAG_HIDDEN      0x04
#define ROWFLAG_MAN         0x08


class ColRowSettings : public ExcRoot
{
private:
    // ACHTUNG: Col-/Row-Angaben in TWIPS
    friend ScExtDocOptions;

    UINT16List*         pHorizPb;   // horizontale Seitenumbrueche
    UINT16List*         pVertPb;    // vertikale ~

    ScExtTabOptions*    pExtTabOpt;
    INT32*              pWidth;
    BOOL*               pColHidden;
    UINT16*             pHeight;
    INT8*               pRowFlags;

    UINT16              nMaxRow;
    UINT16              nDefWidth;
    UINT16              nDefHeight;
    BOOL                bDirty;     // noch nicht rausgehauen?
    BOOL                bSetByStandard;     // StandardWidth hat Vorrang vor DefColWidth!

    void                _SetRowSettings( const UINT16 nRow, const UINT16 nExcelHeight, const UINT16 nGrbit );
public:
                        ColRowSettings( void );
                        ~ColRowSettings();
    void                Reset( void );

    void                SetDefaults( UINT16 nWidth, UINT16 nHeight );
    inline void         SetDefWidth( const UINT16 nNew, const BOOL bStandardWidth = FALSE );
    inline void         SetDefHeight( const UINT16 nNew );

    inline void         Used( const UINT16 nCol, const UINT16 nRow );

    inline void         HideCol( const UINT16 nCol );
    void                HideColRange( UINT16 nColFirst, UINT16 nColLast );
    void                SetWidthRange( UINT16 nF, UINT16 nL, UINT16 nNew );
    void                SetDefaultXF( UINT16 nColFirst, UINT16 nColLast, UINT16 nXF );
    inline void         SetWidth( const UINT16 nCol, const INT32 nNew );

    inline void         SetHeight( const UINT16 nRow, const UINT16 nNew );
    inline void         HideRow( const UINT16 nRow );
    inline void         SetRowSettings( const UINT16 nRow, const UINT16 nExcelHeight, const UINT16 nGrbit );
                                    // Auswertung/Umrechung von nExcelHeight und Auswertung nGrbit

    inline ScExtTabOptions& GetExtTabOpt()
                            { if( !pExtTabOpt ) pExtTabOpt = new ScExtTabOptions; return *pExtTabOpt; }
    inline UINT16       GetActivePane() const
                            { return pExtTabOpt ? pExtTabOpt->nActPane : 3; }


    void                ReadSplit( XclImpStream& rIn );
    void                SetFrozen( const BOOL bFrozen );
    inline void         SetTabSelected( const BOOL bSelected )
                            { GetExtTabOpt().bSelected = bSelected; }
    inline void         SetSelection( const ScRange& rSel );
                            { GetExtTabOpt().SetSelection( rSel ); }
    inline void         SetDimension( const ScRange& rDim )
                            { GetExtTabOpt().SetDimension( rDim ); }

    void                SetHorizPagebreak( const UINT16 nRow );
    void                SetVertPagebreak( const UINT16 nCol );

    void                Apply( const UINT16 nTab );
};




inline void ColRowSettings::SetDefWidth( const UINT16 n, const BOOL b )
{
    if( b )
    {
        nDefWidth = n;
        bSetByStandard = TRUE;
    }
    else if( !bSetByStandard )
        nDefWidth = n;
}


inline void ColRowSettings::SetDefHeight( const UINT16 n )
{
    nDefHeight = n;
}


inline void ColRowSettings::SetWidth( const UINT16 nCol, const INT32 nNew )
{
    if( nCol <= MAXCOL )
        pWidth[ nCol ] = nNew;
}


inline void ColRowSettings::SetHeight( const UINT16 nRow, const UINT16 n )
{
    if( nRow <= MAXROW )
    {
        pHeight[ nRow ] = ( UINT16 ) ( ( double ) ( n & 0x7FFF ) * pExcRoot->fRowScale );

        INT8    nFlags = pRowFlags[ nRow ];
        nFlags |= ROWFLAG_USED;

        if( n & 0x8000 )
            nFlags|= ROWFLAG_DEFAULT;

        if( nRow > nMaxRow )
            nMaxRow = nRow;

        pRowFlags[ nRow ] = nFlags;
    }
}


inline void ColRowSettings::HideCol( const UINT16 nCol )
{
    if( nCol <= MAXCOL )
        pColHidden[ nCol ] = TRUE;
}


inline void ColRowSettings::HideRow( const UINT16 nRow )
{
    if( nRow <= MAXROW )
    {
        pRowFlags[ nRow ] |= ( ROWFLAG_HIDDEN | ROWFLAG_USED );

        if( nRow > nMaxRow )
            nMaxRow = nRow;
    }
}


inline void ColRowSettings::Used( const UINT16 nCol, const UINT16 nRow )
{
    if( nCol <= MAXCOL && nRow <= MAXROW )
    {
        pRowFlags[ nRow ] |= ROWFLAG_USED;

        if( nRow > nMaxRow )
            nMaxRow = nRow;
    }
}


inline void ColRowSettings::SetRowSettings( const UINT16 nRow, const UINT16 nExcelHeight, const UINT16 nGrbit )
{
    if( nRow <= MAXROW )
        _SetRowSettings( nRow, nExcelHeight, nGrbit );
}




// ---------------------------------------------------- class FltColumn --

struct FltColumnItem;

class FltColumn : public ExcRoot
{
private:
    friend FltColumnItem;
    UINT16*                 pData;      // Daten-Array fuer XF-Indizes
    UINT16                  nLastRow;
    UINT16                  nCol;       // Column-Nummer
    const static UINT16     nDefCleared;
#ifdef DEBUG
    static UINT16           nSizeFixedArray;
#else
    const static UINT16     nSizeFixedArray;
#endif
    FltColumnItem*          pFirst;
    FltColumnItem*          pLast;
    FltColumnItem*          pAct;

    void                    DeleteList( void );
    inline FltColumnItem*   First( void );
    inline FltColumnItem*   Next( void );
    inline FltColumnItem*   Last( void );
    inline FltColumnItem*   Prev( void );

    inline void             Append( FltColumnItem* );
    inline void             InsertBefore( FltColumnItem* pActual, FltColumnItem* pPreAct );
    void                    InsertIn( FltColumnItem* pActual, FltColumnItem* pInAct );
    BOOL                    Merge( FltColumnItem* pActual, UINT16 nRow );   // return = TRUE if row could be merged
    BOOL                    Insert( FltColumnItem* pActual, UINT16 nRow, UINT16 nXF );
                                                                            // return = TRUE if row could be inserted
    void                    _SetXF( UINT16 nRow, UINT16 nNewXF );
public:
                            FltColumn( RootData* pRD, UINT16 nNewCol );     // fuer den Ersten
    virtual                 ~FltColumn();

    inline void             SetXF( UINT16 nRow, UINT16 nNewXF );
    void                    Reset( void );
    void                    Apply( const UINT16 nTab ) const;
};


inline void FltColumn::SetXF( UINT16 nRow, UINT16 nNewXF )
{
    DBG_ASSERT( nRow <= MAXROW, "-FltColumn::SetXF(): Row > MAXROW!" );

    if( nRow > nLastRow )
        nLastRow = nRow;

    if( nRow < nSizeFixedArray )
        pData[ nRow ] = nNewXF;
    else
        _SetXF( nRow, nNewXF );
}


// --------------------------------------------------- class FltTabelle --

struct FltCellMerge
{
    FltCellMerge*           pNext;
    UINT16                  nFirstCol;
    UINT16                  nFirstRow;
    UINT16                  nLastCol;
    UINT16                  nLastRow;

    inline                  FltCellMerge( UINT16 nCol, UINT16 nRow ) :
                                pNext( NULL ),
                                nFirstCol( nCol ), nFirstRow( nRow ),
                                nLastCol( nCol ), nLastRow( nRow ) {}
    inline                  FltCellMerge( UINT16 nCol1, UINT16 nRow1, UINT16 nCol2, UINT16 nRow2 ) :
                                pNext( NULL ),
                                nFirstCol( nCol1 ), nFirstRow( nRow1 ),
                                nLastCol( nCol2 ), nLastRow( nRow2 ) {}
};

class FltTabelle : public ExcRoot
{
private:
    FltColumn**             pData;  // Array mit Column-Daten
    UINT16                  nSize;
    UINT16                  nLastCol;

    FltCellMerge*           pFirst;
    FltCellMerge*           pLast;
    FltCellMerge*           pCurr;

    inline FltCellMerge*    First() { return (pCurr = pFirst); }
    inline FltCellMerge*    Next()  { return (pCurr = (pCurr ? pCurr->pNext : NULL)); }
    inline FltCellMerge*    Last()  { return (pCurr = pLast); }
    void                    Append( FltCellMerge* pNew );
    void                    DeleteList();

    inline void             AppendMerge( UINT16 nCol, UINT16 nRow );
    void                    SetBorderLine( const FltCellMerge& rMerge, USHORT nTab, USHORT nLine );

public:
                            FltTabelle( RootData* pRD );
                            ~FltTabelle();

    void                    SetXF( UINT16 nCol, UINT16 nRow, UINT16 nNewXF, const BOOL bBlank = FALSE );
    inline void             AppendMerge( UINT16 nCol1, UINT16 nRow1, UINT16 nCol2, UINT16 nRow2 );

    void                    Reset( void );
    void                    Apply( const UINT16 nTab );
};

inline void FltTabelle::AppendMerge( UINT16 nCol, UINT16 nRow )
{
    Append( new FltCellMerge( nCol, nRow ) );
}

inline void FltTabelle::AppendMerge( UINT16 nCol1, UINT16 nRow1, UINT16 nCol2, UINT16 nRow2 )
{
    Append( new FltCellMerge( nCol1, nRow1, nCol2, nRow2 ) );
}

// -----------------------------------------------------------------------

#endif

