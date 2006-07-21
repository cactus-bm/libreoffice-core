/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sortparam.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: kz $ $Date: 2006-07-21 11:08:18 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sc.hxx"



#include "sortparam.hxx"
#include "global.hxx"
#include "address.hxx"

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif


//------------------------------------------------------------------------

ScSortParam::ScSortParam()
{
    Clear();
}

//------------------------------------------------------------------------

ScSortParam::ScSortParam( const ScSortParam& r ) :
        nCol1(r.nCol1),nRow1(r.nRow1),nCol2(r.nCol2),nRow2(r.nRow2),
        bHasHeader(r.bHasHeader),bCaseSens(r.bCaseSens),
        bByRow(r.bByRow),bUserDef(r.bUserDef),nUserIndex(r.nUserIndex),bIncludePattern(r.bIncludePattern),
        bInplace(r.bInplace),
        nDestTab(r.nDestTab),nDestCol(r.nDestCol),nDestRow(r.nDestRow),
        aCollatorLocale( r.aCollatorLocale ), aCollatorAlgorithm( r.aCollatorAlgorithm )
{
    for (USHORT i=0; i<MAXSORT; i++)
    {
        bDoSort[i]    = r.bDoSort[i];
        nField[i]     = r.nField[i];
        bAscending[i] = r.bAscending[i];
    }
}

//------------------------------------------------------------------------

void ScSortParam::Clear()
{
    nCol1=nCol2=nDestCol = 0;
    nRow1=nRow2=nDestRow = 0;
    nDestTab = 0;
    nUserIndex = 0;
    bHasHeader=bCaseSens=bUserDef = FALSE;
    bByRow=bIncludePattern=bInplace = TRUE;
    aCollatorLocale = ::com::sun::star::lang::Locale();
    aCollatorAlgorithm.Erase();

    for (USHORT i=0; i<MAXSORT; i++)
    {
        bDoSort[i]    = FALSE;
        nField[i]     = 0;
        bAscending[i] = TRUE;
    }
}

//------------------------------------------------------------------------

ScSortParam& ScSortParam::operator=( const ScSortParam& r )
{
    nCol1           = r.nCol1;
    nRow1           = r.nRow1;
    nCol2           = r.nCol2;
    nRow2           = r.nRow2;
    bHasHeader      = r.bHasHeader;
    bCaseSens       = r.bCaseSens;
    bByRow          = r.bByRow;
    bUserDef        = r.bUserDef;
    nUserIndex      = r.nUserIndex;
    bIncludePattern = r.bIncludePattern;
    bInplace        = r.bInplace;
    nDestTab        = r.nDestTab;
    nDestCol        = r.nDestCol;
    nDestRow        = r.nDestRow;
    aCollatorLocale         = r.aCollatorLocale;
    aCollatorAlgorithm      = r.aCollatorAlgorithm;

    for (USHORT i=0; i<MAXSORT; i++)
    {
        bDoSort[i]    = r.bDoSort[i];
        nField[i]     = r.nField[i];
        bAscending[i] = r.bAscending[i];
    }

    return *this;
}

//------------------------------------------------------------------------

BOOL ScSortParam::operator==( const ScSortParam& rOther ) const
{
    BOOL bEqual = FALSE;
    // Anzahl der Sorts gleich?
    USHORT nLast      = 0;
    USHORT nOtherLast = 0;
    while ( bDoSort[nLast++] && nLast < MAXSORT );
    while ( rOther.bDoSort[nOtherLast++] && nOtherLast < MAXSORT );
    nLast--;
    nOtherLast--;
    if (   (nLast           == nOtherLast)
        && (nCol1           == rOther.nCol1)
        && (nRow1           == rOther.nRow1)
        && (nCol2           == rOther.nCol2)
        && (nRow2           == rOther.nRow2)
        && (bHasHeader      == rOther.bHasHeader)
        && (bByRow          == rOther.bByRow)
        && (bCaseSens       == rOther.bCaseSens)
        && (bUserDef        == rOther.bUserDef)
        && (nUserIndex      == rOther.nUserIndex)
        && (bIncludePattern == rOther.bIncludePattern)
        && (bInplace        == rOther.bInplace)
        && (nDestTab        == rOther.nDestTab)
        && (nDestCol        == rOther.nDestCol)
        && (nDestRow        == rOther.nDestRow)
        && (aCollatorLocale.Language    == rOther.aCollatorLocale.Language)
        && (aCollatorLocale.Country     == rOther.aCollatorLocale.Country)
        && (aCollatorLocale.Variant     == rOther.aCollatorLocale.Variant)
        && (aCollatorAlgorithm          == rOther.aCollatorAlgorithm)
        )
    {
        bEqual = TRUE;
        for ( USHORT i=0; i<=nLast && bEqual; i++ )
        {
            bEqual = (nField[i] == rOther.nField[i]) && (bAscending[i]  == rOther.bAscending[i]);
        }
    }
    return bEqual;
}

//------------------------------------------------------------------------

ScSortParam::ScSortParam( const ScSubTotalParam& rSub, const ScSortParam& rOld ) :
        nCol1(rSub.nCol1),nRow1(rSub.nRow1),nCol2(rSub.nCol2),nRow2(rSub.nRow2),
        bHasHeader(TRUE),bCaseSens(rSub.bCaseSens),
        bByRow(TRUE),bUserDef(rSub.bUserDef),nUserIndex(rSub.nUserIndex),bIncludePattern(rSub.bIncludePattern),
        bInplace(TRUE),
        nDestTab(0),nDestCol(0),nDestRow(0),
        aCollatorLocale( rOld.aCollatorLocale ), aCollatorAlgorithm( rOld.aCollatorAlgorithm )
{
    USHORT nNewCount = 0;
    USHORT i;

    //  zuerst die Gruppen aus den Teilergebnissen
    if (rSub.bDoSort)
        for (i=0; i<MAXSUBTOTAL; i++)
            if (rSub.bGroupActive[i])
            {
                if (nNewCount < MAXSORT)
                {
                    bDoSort[nNewCount]    = TRUE;
                    nField[nNewCount]     = rSub.nField[i];
                    bAscending[nNewCount] = rSub.bAscending;
                    ++nNewCount;
                }
            }

    //  dann dahinter die alten Einstellungen
    for (i=0; i<MAXSORT; i++)
        if (rOld.bDoSort[i])
        {
            SCCOLROW nThisField = rOld.nField[i];
            BOOL bDouble = FALSE;
            for (USHORT j=0; j<nNewCount; j++)
                if ( nField[j] == nThisField )
                    bDouble = TRUE;
            if (!bDouble)               // ein Feld nicht zweimal eintragen
            {
                if (nNewCount < MAXSORT)
                {
                    bDoSort[nNewCount]    = TRUE;
                    nField[nNewCount]     = nThisField;
                    bAscending[nNewCount] = rOld.bAscending[i];
                    ++nNewCount;
                }
            }
        }

    for (i=nNewCount; i<MAXSORT; i++)       // Rest loeschen
    {
        bDoSort[i]    = FALSE;
        nField[i]     = 0;
        bAscending[i] = TRUE;
    }
}

//------------------------------------------------------------------------

ScSortParam::ScSortParam( const ScQueryParam& rParam, SCCOL nCol ) :
        nCol1(nCol),nRow1(rParam.nRow1),nRow2(rParam.nRow2),nCol2(nCol),
        bHasHeader(rParam.bHasHeader),bCaseSens(rParam.bCaseSens),
//! TODO: what about Locale and Algorithm?
        bByRow(TRUE),bUserDef(FALSE),nUserIndex(0),bIncludePattern(FALSE),
        bInplace(TRUE),
        nDestTab(0),nDestCol(0),nDestRow(0)
{
    bDoSort[0] = TRUE;
    nField[0] = nCol;
    bAscending[0] = TRUE;
    for (USHORT i=1; i<MAXSORT; i++)
    {
        bDoSort[i]    = FALSE;
        nField[i]     = 0;
        bAscending[i] = TRUE;
    }
}

//------------------------------------------------------------------------

void ScSortParam::MoveToDest()
{
    if (!bInplace)
    {
        SCsCOL nDifX = ((SCsCOL) nDestCol) - ((SCsCOL) nCol1);
        SCsROW nDifY = ((SCsROW) nDestRow) - ((SCsROW) nRow1);

        nCol1 += nDifX;
        nRow1 += nDifY;
        nCol2 += nDifX;
        nRow2 += nDifY;
        for (USHORT i=0; i<MAXSORT; i++)
            if (bByRow)
                nField[i] += nDifX;
            else
                nField[i] += nDifY;

        bInplace = TRUE;
    }
    else
        DBG_ERROR("MoveToDest, bInplace == TRUE");
}

