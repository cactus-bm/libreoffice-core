/*************************************************************************
 *
 *  $RCSfile: colrowst.cxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: rt $ $Date: 2003-04-08 16:22:12 $
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

#ifdef PCH
#include "filt_pch.hxx"
#endif


#pragma hdrstop

#include "colrowst.hxx"

#include <string.h>

#include "document.hxx"
#include "root.hxx"

#ifndef SC_FTOOLS_HXX
#include "ftools.hxx"
#endif
#ifndef SC_XISTREAM_HXX
#include "xistream.hxx"
#endif
#ifndef SC_XISTYLE_HXX
#include "xistyle.hxx"
#endif


ColRowSettings::ColRowSettings( RootData& rRootData ) :
    ExcRoot( &rRootData )
{
    pHorizPb = pVertPb = NULL;

    nDefWidth = nDefHeight = 0;

    pWidth = new INT32 [ MAXCOL + 1 ];
    pColHidden = new BOOL [ MAXCOL + 1 ];

    pHeight = new UINT16 [ MAXROW + 1 ];
    pRowFlags = new INT8[ MAXROW + 1 ];

    pExtTabOpt = NULL;

    Reset();
}


ColRowSettings::~ColRowSettings()
{
    if( pHorizPb )
        delete pHorizPb;

    if( pVertPb )
        delete pVertPb;

    delete[] pRowFlags;
    delete[] pHeight;
    delete[] pColHidden;
    delete[] pWidth;

    if( pExtTabOpt )
        delete pExtTabOpt;
}


void ColRowSettings::Reset( void )
{
    UINT16  nC;
    for( nC = 0 ; nC <= MAXCOL ; nC++ )
    {
        pColHidden[ nC ] = FALSE;
        pWidth[ nC ] = -1;
    }

    memset( pRowFlags, 0x00, sizeof( INT8 ) * ( MAXROW + 1 ) );

    if( pExtTabOpt )
    {
        delete pExtTabOpt;
        pExtTabOpt = NULL;
    }

    if( pHorizPb )
    {
        delete pHorizPb;
        pHorizPb = NULL;
    }

    if( pVertPb )
    {
        delete pVertPb;
        pVertPb = NULL;
    }

    bDirty = TRUE;
    nMaxRow = -1;

    bSetByStandard = FALSE;
}


void ColRowSettings::Apply( const UINT16 nAktTab )
{
    if( !bDirty )
        return;

    INT32                   nC;
    UINT16                  nStart = 0;
    UINT16                  nWidth;
    UINT16                  nLastWidth = ( pWidth[ 0 ] >= 0 )? ( UINT16 ) pWidth[ 0 ] : nDefWidth;
    ScDocument&             rD = *pExcRoot->pDoc;
    BOOL                    bExtraHide;

    rD.IncSizeRecalcLevel( nAktTab );

    // Column-Bemachung
    for( nC = 0 ; nC <= MAXCOL ; nC++ )
    {
        if( pWidth[ nC ] >= 0 )
            // eingestellte Width
            nWidth = ( UINT16 ) pWidth[ nC ];
        else
            // Default-Width
            nWidth = nDefWidth;

        bExtraHide = ( nWidth == 0 );

        if( !bExtraHide )
            rD.SetColWidth( UINT16( nC ), nAktTab, nWidth );

        if( pColHidden[ nC ] || bExtraHide )
            // Column versteckt
            rD.SetColFlags( UINT16( nC ), nAktTab, rD.GetColFlags( UINT16( nC ), nAktTab ) | CR_HIDDEN );
    }

    // Row-Bemachung

    INT8                    nFlags;
    nStart = 0;
    UINT16                  nHeight;

    UINT16                  nLastHeight;
    nFlags = pRowFlags[ 0 ];
    if( nFlags & ROWFLAG_USED )
    {
        if( nFlags & ROWFLAG_DEFAULT )
            nLastHeight = nDefHeight;
        else
        {
            nLastHeight = pHeight[ 0 ];
            if( !nLastHeight )
                nLastHeight = nDefHeight;
        }
    }
    else
        nLastHeight = nDefHeight;

    for( nC = 0 ; nC <= nMaxRow ; nC++ )
    {
        nFlags = pRowFlags[ nC ];

        if( nFlags & ROWFLAG_USED )
        {
            if( nFlags & ROWFLAG_DEFAULT )
                nHeight = nDefHeight;
            else
            {
                nHeight = pHeight[ nC ];
                if( !nHeight )
                    nHeight = nDefHeight;
            }

            if( nFlags & ( ROWFLAG_HIDDEN | ROWFLAG_MAN ) )
            {
                BYTE        nSCFlags = rD.GetRowFlags( UINT16( nC ), nAktTab );

                if( nFlags & ROWFLAG_HIDDEN )
                    nSCFlags |= CR_HIDDEN;

                if( nFlags & ROWFLAG_MAN )
                    nSCFlags |= CR_MANUALSIZE;

                rD.SetRowFlags( UINT16( nC ), nAktTab, nSCFlags );
            }
        }
        else
            nHeight = nDefHeight;

        if( !nHeight )
            rD.SetRowFlags( UINT16( nC ), nAktTab, rD.GetRowFlags( UINT16( nC ), nAktTab ) | CR_HIDDEN );

        if( nLastHeight != nHeight )
        {
            DBG_ASSERT( nC > 0, "ColRowSettings::Apply(): Algorithmus-Fehler!" );

            if( nLastHeight )
                rD.SetRowHeightRange( nStart, nC - 1, nAktTab, nLastHeight );

            nStart = UINT16( nC );
            nLastHeight = nHeight;
        }
    }

    if( nLastHeight && nMaxRow >= 0 )
        rD.SetRowHeightRange( nStart, UINT16( nMaxRow ), nAktTab, nLastHeight );

    if( pExtTabOpt )
        pExcRoot->pExtDocOpt->Add( *this );

    if( pHorizPb )
        {
        UINT16  n = pHorizPb->First();

        while( n && n <= MAXROW )
        {
            rD.SetRowFlags( n, nAktTab, rD.GetRowFlags( n, nAktTab ) | CR_MANUALBREAK );
            n = pHorizPb->Next();
        }
    }

    if( pVertPb )
    {
        UINT16  n = pVertPb->First();

        while( n && n <= MAXCOL )
        {
            rD.SetColFlags( n, nAktTab, rD.GetColFlags( n, nAktTab ) | CR_MANUALBREAK );
            n = pVertPb->Next();
        }
    }

    bDirty = FALSE; // jetzt stimmt Tabelle im ScDocument

    rD.DecSizeRecalcLevel( nAktTab );
}


void ColRowSettings::HideColRange( UINT16 nColFirst, UINT16 nColLast )
{
    DBG_ASSERT( nColFirst <= nColLast, "+ColRowSettings::HideColRange(): First > Last?!" );
    DBG_ASSERT( nColLast <= MAXCOL, "+ColRowSettings::HideColRange(): ungueltige Column" );

    if( nColLast > MAXCOL )
        nColLast = MAXCOL;

    BOOL*   pHidden;
    BOOL*   pFinish;
    pHidden = &pColHidden[ nColFirst ];
    pFinish = &pColHidden[ nColLast ];
    while( pHidden <= pFinish )
        *( pHidden++ ) = TRUE;
}


void ColRowSettings::SetWidthRange( UINT16 nColFirst, UINT16 nColLast, UINT16 nNew )
{
    DBG_ASSERT( nColFirst <= nColLast, "+ColRowSettings::SetColWidthRange(): First > Last?!" );
    DBG_ASSERT( nColLast <= MAXCOL, "+ColRowSettings::SetColWidthRange(): ungueltige Column" );

    if( nColLast > MAXCOL )
        nColLast = MAXCOL;

    INT32*  pWidthCount;
    INT32*  pFinish;
    pWidthCount = &pWidth[ nColFirst ];
    pFinish = &pWidth[ nColLast ];

    while( pWidthCount <= pFinish )
        *( pWidthCount++ ) = nNew;
}


void ColRowSettings::SetDefaultXF( UINT16 nColFirst, UINT16 nColLast, UINT16 nXF )
{
    DBG_ASSERT( nColFirst <= nColLast, "+ColRowSettings::SetDefaultXF(): First > Last?!" );
    DBG_ASSERT( nColLast <= MAXCOL, "+ColRowSettings::SetDefaultXF(): ungueltige Column" );

    if( nColLast > MAXCOL )
        nColLast = MAXCOL;

    const XclImpRoot& rRoot = *pExcRoot->pIR;
    rRoot.GetXFBuffer().ApplyPattern( nColFirst, 0, nColLast, MAXROW, rRoot.GetScTab(), nXF );
}


void ColRowSettings::SetDefaults( UINT16 nWidth, UINT16 nHeight )
{
    nDefWidth = nWidth;
    nDefHeight = nHeight;
}


void ColRowSettings::_SetRowSettings( const UINT16 nRow, const UINT16 nExcelHeight, const UINT16 nGrbit )
{
    pHeight[ nRow ] = ( UINT16 ) ( ( double ) ( nExcelHeight & 0x7FFF ) * pExcRoot->fRowScale );

    INT8    nFlags = ROWFLAG_USED;

    if( nExcelHeight & 0x8000 )
        nFlags |= ROWFLAG_DEFAULT;

    if( nGrbit & EXC_ROW_UNSYNCED )
        nFlags |= ROWFLAG_MAN;

    if( nGrbit &  EXC_ROW_ZEROHEIGHT )
        nFlags |= ROWFLAG_HIDDEN;

    pRowFlags[ nRow ] = nFlags;

    if( nRow > nMaxRow )
        nMaxRow = nRow;
}


void ColRowSettings::ReadSplit( XclImpStream& rIn )
{
    GetExtTabOpt();

    rIn >> pExtTabOpt->nSplitX >> pExtTabOpt->nSplitY >> pExtTabOpt->nTopSplitRow >> pExtTabOpt->nLeftSplitCol;

    if( (pExcRoot->eHauptDateiTyp == Biff5) || (pExcRoot->eHauptDateiTyp == Biff8) )
        rIn >> pExtTabOpt->nActPane;
    else
        pExtTabOpt->nActPane = rIn.ReaduInt8();

    pExtTabOpt->nTabNum = pExcRoot->pIR->GetScTab();
}


void ColRowSettings::SetVisCorner( UINT16 nCol, UINT16 nRow )
{
    GetExtTabOpt().nLeftCol = nCol;
    GetExtTabOpt().nTopRow = nRow;
}



void ColRowSettings::SetFrozen( const BOOL bFrozen )
{
    GetExtTabOpt().nTabNum = pExcRoot->pIR->GetScTab();
    GetExtTabOpt().bFrozen = bFrozen;
}



void ColRowSettings::SetHorizPagebreak( const UINT16 n )
{
    DBG_ASSERT( n, "+ColRowSettings::SetHorizPagebreak(): 0 ist hier nicht zulaessig!" );

    if( !pHorizPb )
        pHorizPb = new ScfUInt16List;

    pHorizPb->Append( n );
}


void ColRowSettings::SetVertPagebreak( const UINT16 n )
{
    DBG_ASSERT( n, "+ColRowSettings::SetVertPagebreak(): 0 ist hier nicht zulaessig!" );

    if( !pVertPb )
        pVertPb = new ScfUInt16List;

    pVertPb->Append( n );
}






void ScExtTabOptions::SetSelection( const ScRange& r )
{
    if( r.aStart.Row() <= MAXROW && r.aEnd.Row() <= MAXROW )
    {
        bValidSel = TRUE;
        aLastSel = r;
    }
    else
        bValidSel = FALSE;
}


void ScExtTabOptions::SetDimension( const ScRange& r )
{
    if( r.aStart.Row() <= MAXROW && r.aEnd.Row() <= MAXROW )
    {
        bValidDim = TRUE;
        aDim = r;
    }
    else
        bValidDim = FALSE;
}




CodenameList::CodenameList( const CodenameList& r )
{
    UINT32          n = 0;
    const String*   p = ( const String*) r.GetObject( n );

    while( p )
    {
        Append( *p );
        n++;
        p = ( const String*) r.GetObject( n );
    }
}


CodenameList::~CodenameList()
{
    String*         p = ( String* ) List::First();

    while( p )
    {
        delete p;
        p = ( String* ) List::Next();
    }
}




ScExtDocOptions::ScExtDocOptions( void )
{
    pGridCol = NULL;
    nActTab = nSelTabs = nCurCol = nCurRow = 0;
    pOleSize = NULL;
    nLinkCnt = 0;       // -> 'Root'-Dokument
    nZoom = 100;

    ppExtTabOpts = new ScExtTabOptions *[ MAXTAB + 1 ];
    for( UINT16 nCnt = 0 ; nCnt <= MAXTAB ; nCnt++ )
        ppExtTabOpts[ nCnt ] = NULL;

    pCodenameWB = NULL;
    pCodenames = NULL;
    bChanged = TRUE;

    SetWinProtection(false);
}


ScExtDocOptions::~ScExtDocOptions()
{
    if( pGridCol )
        delete pGridCol;
    if( pOleSize )
        delete pOleSize;

    for( UINT16 nCnt = 0 ; nCnt <= MAXTAB ; nCnt++ )
    {
        if( ppExtTabOpts[ nCnt ] )
            delete ppExtTabOpts[ nCnt ];
    }

    delete[] ppExtTabOpts;

    if( pCodenameWB )
        delete pCodenameWB;
    if( pCodenames )
        delete pCodenames;
}


ScExtDocOptions& ScExtDocOptions::operator =( const ScExtDocOptions& rCpy )
{
    nLinkCnt = rCpy.nLinkCnt;
    nActTab = rCpy.nActTab;
    nSelTabs = rCpy.nSelTabs;
    nCurCol = rCpy.nCurCol;
    nCurRow = rCpy.nCurRow;

    if( pGridCol )
    {
        if( rCpy.pGridCol )
            *pGridCol = *rCpy.pGridCol;
        else
        {
            delete pGridCol;
            pGridCol = NULL;
        }
    }
    else if( rCpy.pGridCol )
        pGridCol = new Color( *rCpy.pGridCol );

    nZoom = rCpy.nZoom;
    nCurCol = rCpy.nCurCol;
    nCurRow = rCpy.nCurRow;

    for( UINT16 nCnt = 0 ; nCnt <= MAXTAB ; nCnt++ )
    {
        const ScExtTabOptions*  pT = rCpy.ppExtTabOpts[ nCnt ];
        if( pT )
        {
            if( ppExtTabOpts[ nCnt ] )
                *ppExtTabOpts[ nCnt ] = *pT;
            else
                ppExtTabOpts[ nCnt ] = new ScExtTabOptions( *pT );
        }
        else
        {
            if( ppExtTabOpts[ nCnt ] )
                delete ppExtTabOpts[ nCnt ];

            ppExtTabOpts[ nCnt ] = NULL;
        }
    }

    if( rCpy.pCodenameWB )
        pCodenameWB = new String( *rCpy.pCodenameWB );
    if( rCpy.pCodenames )
        pCodenames = new CodenameList( *rCpy.pCodenames );

    bChanged = rCpy.bChanged;

    return *this;
}


void ScExtDocOptions::SetExtTabOptions( UINT16 nTabNum, ScExtTabOptions* pTabOpt )
{
    if( ppExtTabOpts[ nTabNum ] )
        delete ppExtTabOpts[ nTabNum ];
    ppExtTabOpts[ nTabNum ] = pTabOpt;
}


void ScExtDocOptions::SetGridCol( const Color& rColor )
{
    if( pGridCol )
        pGridCol->SetColor( rColor.GetColor() );
    else
        pGridCol = new Color( rColor );
}


void ScExtDocOptions::SetActTab( UINT16 nTab )
{
    nActTab = ( nTab <= MAXTAB )? nTab : MAXTAB;
}


void ScExtDocOptions::SetOleSize( USHORT nFirstCol, USHORT nFirstRow, USHORT nLastCol, USHORT nLastRow )
{
    if( pOleSize )
    {
        pOleSize->aStart.Set( nFirstCol, nFirstRow, 0 );
        pOleSize->aEnd.Set( nLastCol, nLastRow, 0 );
    }
    else
        pOleSize = new ScRange( nFirstCol, nFirstRow, 0, nLastCol, nLastRow, 0 );
}


void ScExtDocOptions::SetCursor( UINT16 nCol, UINT16 nRow )
{
    nCurCol = ( nCol <= MAXCOL )? nCol : MAXCOL;
    nCurRow = ( nRow <= MAXROW )? nRow : MAXROW;
}


void ScExtDocOptions::SetZoom( UINT16 nZaehler, UINT16 nNenner )
{
    nZoom = 100 * nZaehler / nNenner;
    if( nZoom < 20 )
        nZoom = 20;
    else if( nZoom > 400 )
        nZoom = 400;
}


void ScExtDocOptions::Add( const ColRowSettings& rCRS )
{
    const UINT16 nTab = rCRS.pExcRoot->pIR->GetScTab();

    if( nTab <= MAXTAB )
    {
        if( rCRS.pExtTabOpt )
        {
            if( ppExtTabOpts[ nTab ] )
                *ppExtTabOpts[ nTab ] = *rCRS.pExtTabOpt;
            else
                ppExtTabOpts[ nTab ] = new ScExtTabOptions( *rCRS.pExtTabOpt );
        }
    }
    else
    {
        DBG_WARNING( "ScExtDocOptions::Add(): Aetsch... falsch bedient!" );
    }
}


void ScExtDocOptions::SetCodename( const String& r )
{
    if( pCodenameWB )
        *pCodenameWB = r;
    else
        pCodenameWB = new String( r );
}


void ScExtDocOptions::AddCodename( const String& r )
{
    if( !pCodenames )
        pCodenames = new CodenameList;

    pCodenames->Append( r );
}



