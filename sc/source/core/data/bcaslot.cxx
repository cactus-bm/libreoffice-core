/*************************************************************************
 *
 *  $RCSfile: bcaslot.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: er $ $Date: 2002-11-27 21:19:51 $
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

// System - Includes -----------------------------------------------------

#ifdef PCH
#include "core_pch.hxx"
#endif

#pragma hdrstop

#include <sfx2/objsh.hxx>
#include <svtools/lstner.hxx>

// INCLUDE ---------------------------------------------------------------

#include "document.hxx"
#include "brdcst.hxx"
#include "bcaslot.hxx"
#include "scerrors.hxx"
#include "docoptio.hxx"
#include "refupdat.hxx"
#include "table.hxx"

// Anzahl der Slots je Dimension
// muessen ganzzahlige Teiler von MAXCOL+1 bzw. MAXROW+1 sein
#define BCA_SLOTS_COL 16
#define BCA_SLOTS_ROW 256
#define BCA_SLOT_COLS ((MAXCOL+1) / BCA_SLOTS_COL)
#define BCA_SLOT_ROWS ((MAXROW+1) / BCA_SLOTS_ROW)
// vielfaches?
#if (BCA_SLOT_COLS * BCA_SLOTS_COL) != (MAXCOL+1)
#error bad BCA_SLOTS_COL value!
#endif
#if (BCA_SLOT_ROWS * BCA_SLOTS_ROW) != (MAXROW+1)
#error bad BCA_SLOTS_ROW value!
#endif
// Groesse des Slot-Arrays
#define BCA_SLOTS (BCA_SLOTS_COL * BCA_SLOTS_ROW)
#if BCA_SLOTS > 16350
#error BCA_SLOTS DOOMed!
#endif

DECLARE_LIST( ScBroadcastAreaList, ScBroadcastArea* );

// STATIC DATA -----------------------------------------------------------

#ifdef erDEBUG
ULONG erCountBCAInserts = 0;
ULONG erCountBCAFinds = 0;
#endif

SV_IMPL_OP_PTRARR_SORT( ScBroadcastAreas, ScBroadcastAreaPtr );
TYPEINIT1( ScHint, SfxSimpleHint );
TYPEINIT1( ScAreaChangedHint, SfxHint );


ScBroadcastAreaSlot::ScBroadcastAreaSlot( ScDocument* pDocument,
        ScBroadcastAreaSlotMachine* pBASMa ) :
    pDoc( pDocument ),
    pBASM( pBASMa )
{
    pBroadcastAreaTbl = new ScBroadcastAreas( BCA_INITGROWSIZE, BCA_INITGROWSIZE );
    pTmpSeekBroadcastArea = new ScBroadcastArea( ScRange() );
}


ScBroadcastAreaSlot::~ScBroadcastAreaSlot()
{
    USHORT nPos = pBroadcastAreaTbl->Count();
    if ( nPos )
    {
        ScBroadcastArea** ppArea =
            ((ScBroadcastArea**) pBroadcastAreaTbl->GetData()) + nPos - 1;
        for ( ; nPos-- >0; ppArea-- )
        {
            if ( !(*ppArea)->DecRef() )
                delete *ppArea;
        }
    }
    delete pBroadcastAreaTbl;
    delete pTmpSeekBroadcastArea;
}


// nur hier werden neue BroadcastAreas angelegt, wodurch keine doppelten entstehen.
// Ist rpArea != NULL werden keine Listener gestartet sondern nur die Area
// eingetragen und der RefCount erhoeht
void ScBroadcastAreaSlot::StartListeningArea( const ScRange& rRange,
        SfxListener* pListener, ScBroadcastArea*& rpArea
    )
{
    DBG_ASSERT(pListener, "StartListeningArea: pListener Null");
    if ( pDoc->GetHardRecalcState() )
        return;
    if ( (long)( (pBroadcastAreaTbl->Count() + 1 + BCA_INITGROWSIZE)
            * sizeof(ScBroadcastArea*) ) >= USHRT_MAX
        )
    {
        if ( !pDoc->GetHardRecalcState() )
        {
            pDoc->SetHardRecalcState( 1 );

            SfxObjectShell* pShell = pDoc->GetDocumentShell();
            DBG_ASSERT( pShell, "Missing DocShell :-/" );

            if ( pShell )
                pShell->SetError( SCWARN_CORE_HARD_RECALC );

            pDoc->SetAutoCalc( FALSE );
            pDoc->SetHardRecalcState( 2 );
        }
        return;
    }
    if ( !rpArea )
    {
        rpArea = new ScBroadcastArea( rRange );
        // meistens existiert die Area noch nicht, der Versuch sofort zu inserten
        // erspart in diesen Faellen ein doppeltes Seek_Entry
        if ( pBroadcastAreaTbl->Insert( rpArea ) )
            rpArea->IncRef();
        else
        {
            delete rpArea;
            rpArea = GetBroadcastArea( rRange );
        }
        pListener->StartListening( *rpArea, TRUE );
    }
    else
    {
        if ( pBroadcastAreaTbl->Insert( rpArea ) )
            rpArea->IncRef();
    }
}


// Ist rpArea != NULL werden keine Listener gestopt sondern nur die Area
// ausgetragen und der RefCount vermindert
void ScBroadcastAreaSlot::EndListeningArea( const ScRange& rRange,
        SfxListener* pListener, ScBroadcastArea*& rpArea
    )
{
    DBG_ASSERT(pListener, "EndListeningArea: pListener Null");
    if ( !rpArea )
    {
        USHORT nPos;
        if ( (nPos = FindBroadcastArea( rRange )) == USHRT_MAX )
            return;
        rpArea = (*pBroadcastAreaTbl)[ nPos ];
        pListener->EndListening( *rpArea );
        if ( !rpArea->HasListeners() )
        {   // wenn keiner mehr zuhoert ist die Area ueberfluessig
            pBroadcastAreaTbl->Remove( nPos );
            if ( !rpArea->DecRef() )
            {
                delete rpArea;
                rpArea = NULL;
            }
        }
    }
    else
    {
        if ( !rpArea->HasListeners() )
        {
            USHORT nPos;
            if ( (nPos = FindBroadcastArea( rRange )) == USHRT_MAX )
                return;
            pBroadcastAreaTbl->Remove( nPos );
            if ( !rpArea->DecRef() )
            {
                delete rpArea;
                rpArea = NULL;
            }
        }
    }
}


USHORT ScBroadcastAreaSlot::FindBroadcastArea( const ScRange& rRange ) const
{
    USHORT nPos;
    pTmpSeekBroadcastArea->UpdateRange( rRange );
    if ( pBroadcastAreaTbl->Seek_Entry( pTmpSeekBroadcastArea, &nPos ) )
        return nPos;
    return USHRT_MAX;
}


ScBroadcastArea* ScBroadcastAreaSlot::GetBroadcastArea(
        const ScRange& rRange ) const
{
    USHORT nPos;
    if ( (nPos = FindBroadcastArea( rRange )) != USHRT_MAX )
        return (*pBroadcastAreaTbl)[ nPos ];
    return 0;
}


BOOL ScBroadcastAreaSlot::AreaBroadcast( const ScHint& rHint) const
{
    USHORT nCount = pBroadcastAreaTbl->Count();
    if ( nCount == 0 )
        return FALSE;
    const ScBroadcastArea** ppArea =
        (const ScBroadcastArea**) pBroadcastAreaTbl->GetData();
    BOOL bIsBroadcasted = FALSE;
    // leider laesst sich nicht nach dem erstmoeglichen suchen
    USHORT nPos = 0;
    // den letztmoeglichen suchen, Seek_Entry liefert naechst groesseren
    // oder freie Position wenn nicht gefunden
    USHORT nPosEnd;
    const ScAddress& rAddress = rHint.GetAddress();
    pTmpSeekBroadcastArea->UpdateRange( ScRange( rAddress,
        ScAddress( MAXCOL, MAXROW, MAXTAB ) ) );
    if ( !pBroadcastAreaTbl->Seek_Entry( pTmpSeekBroadcastArea, &nPosEnd )
      && nPosEnd > 0 )
        --nPosEnd;
    for ( ; nPos <= nPosEnd; ++nPos, ppArea++ )
    {
        if ( ((ScBroadcastArea*)*ppArea)->In( rAddress ) )
        {
            ((ScBroadcastArea*)*ppArea)->Broadcast( rHint );
            bIsBroadcasted = TRUE;
        }
    }
    return bIsBroadcasted;
}


BOOL ScBroadcastAreaSlot::AreaBroadcastInRange( const ScRange& rRange,
        const ScHint& rHint) const
{
    USHORT nCount = pBroadcastAreaTbl->Count();
    if ( nCount == 0 )
        return FALSE;
    const ScBroadcastArea** ppArea =
        (const ScBroadcastArea**) pBroadcastAreaTbl->GetData();
    BOOL bIsBroadcasted = FALSE;
    // unfortunately we can't search for the first matching entry
    USHORT nPos = 0;
    // search the last matching entry, Seek_Entry returns the next being
    // greater, or a free position if not found
    USHORT nPosEnd;
    pTmpSeekBroadcastArea->UpdateRange( rRange );
    if ( !pBroadcastAreaTbl->Seek_Entry( pTmpSeekBroadcastArea, &nPosEnd ) &&
            nPosEnd > 0 )
        --nPosEnd;
    for ( ; nPos <= nPosEnd; ++nPos, ppArea++ )
    {
        if ( ((ScBroadcastArea*)*ppArea)->Intersects( rRange ) )
        {
            ((ScBroadcastArea*)*ppArea)->Broadcast( rHint );
            bIsBroadcasted = TRUE;
        }
    }
    return bIsBroadcasted;
}


//  DelBroadcastAreasInRange wird unter Windows (16 Bit) kaputtoptimiert

#ifdef WIN
#pragma optimize("",off)
#endif

void ScBroadcastAreaSlot::DelBroadcastAreasInRange( const ScRange& rRange )
{
    ScBroadcastArea* pArea;
    ScAddress aStart( rRange.aStart );
    USHORT nPos = pBroadcastAreaTbl->Count();
    const ScBroadcastArea** ppArea =
        (const ScBroadcastArea**) pBroadcastAreaTbl->GetData() + nPos - 1;
    for ( ; nPos-- >0; ppArea-- )
    {   // rueckwaerts wg. Pointer-Aufrueckerei im Array
        pArea = (ScBroadcastArea*)*ppArea;
        if ( pArea->aStart < aStart )
            return;     // davor nur noch niedrigere
            // gesuchte muessen komplett innerhalb von rRange liegen
        if ( rRange.In( pArea->aStart ) && rRange.In( pArea->aEnd ) )
        {
            pBroadcastAreaTbl->Remove( nPos );
            ppArea = (const ScBroadcastArea**) pBroadcastAreaTbl->GetData()
                + nPos;
            if ( !pArea->DecRef() )
                delete pArea;
        }
    }
}

#ifdef WIN
#pragma optimize("",on)
#endif

void ScBroadcastAreaSlot::UpdateRemove( UpdateRefMode eUpdateRefMode,
        const ScRange& rRange, short nDx, short nDy, short nDz
    )
{
    USHORT nPos = pBroadcastAreaTbl->Count();
    if ( nPos )
    {
        USHORT nCol1, nRow1, nTab1, nCol2, nRow2, nTab2;
        USHORT theCol1, theRow1, theTab1, theCol2, theRow2, theTab2;
        nCol1 = rRange.aStart.Col();
        nRow1 = rRange.aStart.Row();
        nTab1 = rRange.aStart.Tab();
        nCol2 = rRange.aEnd.Col();
        nRow2 = rRange.aEnd.Row();
        nTab2 = rRange.aEnd.Tab();
        ScAddress aAdr;
        const ScBroadcastArea** ppArea =
            ((const ScBroadcastArea**) pBroadcastAreaTbl->GetData()) + nPos - 1;
        for ( ; nPos-- >0; ppArea-- )
        {   // rueckwaerts wg. Pointer-Aufrueckerei im Array
            ScBroadcastArea* pArea = (ScBroadcastArea*) *ppArea;
            if ( pArea->IsInUpdateChain() )
            {
                pBroadcastAreaTbl->Remove( nPos );
                // Remove kann pData veraendern
                ppArea = (const ScBroadcastArea**)
                    pBroadcastAreaTbl->GetData() + nPos;
                pArea->DecRef();
            }
            else
            {
                aAdr = pArea->GetStart();
                theCol1 = aAdr.Col();
                theRow1 = aAdr.Row();
                theTab1 = aAdr.Tab();
                aAdr = pArea->GetEnd();
                theCol2 = aAdr.Col();
                theRow2 = aAdr.Row();
                theTab2 = aAdr.Tab();
                if ( ScRefUpdate::Update( pDoc, eUpdateRefMode,
                        nCol1,nRow1,nTab1, nCol2,nRow2,nTab2, nDx,nDy,nDz,
                        theCol1,theRow1,theTab1, theCol2,theRow2,theTab2 )
                    )
                {
                    pBroadcastAreaTbl->Remove( nPos );
                    // Remove kann pData veraendern
                    ppArea = (const ScBroadcastArea**)
                        pBroadcastAreaTbl->GetData() + nPos;
                    pArea->DecRef();
                    pArea->SetInUpdateChain( TRUE );
                    ScBroadcastArea* pUC = pBASM->GetEOUpdateChain();
                    if ( pUC )
                        pUC->SetUpdateChainNext( pArea );
                    else    // kein Ende kein Anfang
                        pBASM->SetUpdateChain( pArea );
                    pBASM->SetEOUpdateChain( pArea );
                }
            }
        }
    }
}


void ScBroadcastAreaSlot::UpdateInsert( ScBroadcastArea* pArea )
{
    if ( pBroadcastAreaTbl->Insert( pArea ) )
        pArea->IncRef();
}


// --- ScBroadcastAreaSlotMachine -------------------------------------

ScBroadcastAreaSlotMachine::ScBroadcastAreaSlotMachine(
        ScDocument* pDocument ) :
    pDoc( pDocument ),
    pBCAlwaysList( new ScBroadcastAreaList ),
    pUpdateChain( NULL ),
    pEOUpdateChain( NULL )
{
    ppSlots = new ScBroadcastAreaSlot* [ BCA_SLOTS ];
    memset( ppSlots, 0 , sizeof( ScBroadcastAreaSlot* ) * BCA_SLOTS );
}


ScBroadcastAreaSlotMachine::~ScBroadcastAreaSlotMachine()
{

    ScBroadcastAreaSlot** pp = ppSlots;
    for ( USHORT j=0; j < BCA_SLOTS; ++j, ++pp )
    {
        if ( *pp )
            delete *pp;
    }
    delete ppSlots;

    for ( ScBroadcastArea* pBCA = pBCAlwaysList->First(); pBCA; pBCA = pBCAlwaysList->Next() )
    {
        delete pBCA;
    }
    delete pBCAlwaysList;
}


inline USHORT ScBroadcastAreaSlotMachine::ComputeSlotOffset(
        const ScAddress& rAddress ) const
{
    USHORT nRow = rAddress.Row();
    USHORT nCol = rAddress.Col();
    if ( nRow > MAXROW || nCol > MAXCOL )
    {
        DBG_ASSERT( FALSE, "Row/Col ungueltig!" );
        return 0;
    }
    else
        return
            nRow / BCA_SLOT_ROWS +
            nCol / BCA_SLOT_COLS * BCA_SLOTS_ROW;
}


void ScBroadcastAreaSlotMachine::ComputeAreaPoints( const ScRange& rRange,
        USHORT& rStart, USHORT& rEnd, USHORT& rRowBreak
    ) const
{
    rStart = ComputeSlotOffset( rRange.aStart );
    rEnd = ComputeSlotOffset( rRange.aEnd );
    // Anzahl Zeilen-Slots pro Spalte minus eins
    rRowBreak = ComputeSlotOffset(
        ScAddress( rRange.aStart.Col(), rRange.aEnd.Row(), 0 ) ) - rStart;
}


void ScBroadcastAreaSlotMachine::StartListeningArea( const ScRange& rRange,
        SfxListener* pListener
    )
{
    if ( rRange == BCA_LISTEN_ALWAYS  )
    {
        ScBroadcastArea* pBCA;
        if ( !pBCAlwaysList->Count() )
        {
            pBCA = new ScBroadcastArea( rRange );
            pListener->StartListening( *pBCA, FALSE );  // kein PreventDupes
            pBCAlwaysList->Insert( pBCA, LIST_APPEND );
            return ;
        }
        ScBroadcastArea* pLast;
        for ( pBCA = pBCAlwaysList->First(); pBCA; pBCA = pBCAlwaysList->Next() )
        {
            if ( pListener->IsListening( *pBCA ) )
                return ;        // keine Dupes
            pLast = pBCA;
        }
        pBCA = pLast;
        //! ListenerArrays don't shrink!
        if ( pBCA->GetListenerCount() > ((USHRT_MAX / 2) / sizeof(SfxBroadcaster*)) )
        {   // Arrays nicht zu gross werden lassen
            pBCA = new ScBroadcastArea( rRange );
            pBCAlwaysList->Insert( pBCA, LIST_APPEND );
        }
        pListener->StartListening( *pBCA, FALSE );  // kein PreventDupes
    }
    else
    {
        USHORT nStart, nEnd, nRowBreak;
        ComputeAreaPoints( rRange, nStart, nEnd, nRowBreak );
        USHORT nOff = nStart;
        USHORT nBreak = nOff + nRowBreak;
        ScBroadcastAreaSlot** pp = ppSlots + nOff;
        ScBroadcastArea* pArea = NULL;
        while ( nOff <= nEnd )
        {
            if ( !*pp )
                *pp = new ScBroadcastAreaSlot( pDoc, this );
            // der erste erzeugt ggbf. die BroadcastArea
            (*pp)->StartListeningArea( rRange, pListener, pArea );
            if ( nOff < nBreak )
            {
                ++nOff;
                ++pp;
            }
            else
            {
                nStart += BCA_SLOTS_ROW;
                nOff = nStart;
                pp = ppSlots + nOff;
                nBreak = nOff + nRowBreak;
            }
        }
    }
}


void ScBroadcastAreaSlotMachine::EndListeningArea( const ScRange& rRange,
        SfxListener* pListener
    )
{
    if ( rRange == BCA_LISTEN_ALWAYS  )
    {
        if ( pBCAlwaysList->Count() )
        {
            for ( ScBroadcastArea* pBCA = pBCAlwaysList->First(); pBCA; pBCA = pBCAlwaysList->Next() )
            {
                // EndListening liefert FALSE wenn !IsListening, keine Dupes
                if ( pListener->EndListening( *pBCA, FALSE ) )
                {
                    if ( !pBCA->HasListeners() )
                    {
                        pBCAlwaysList->Remove();
                        delete pBCA;
                    }
                    return ;
                }
            }
        }
    }
    else
    {
        USHORT nStart, nEnd, nRowBreak;
        ComputeAreaPoints( rRange, nStart, nEnd, nRowBreak );
        USHORT nOff = nStart;
        USHORT nBreak = nOff + nRowBreak;
        ScBroadcastAreaSlot** pp = ppSlots + nOff;
        ScBroadcastArea* pArea = NULL;
        while ( nOff <= nEnd )
        {
            if ( *pp )
                (*pp)->EndListeningArea( rRange, pListener, pArea );
            if ( nOff < nBreak )
            {
                ++nOff;
                ++pp;
            }
            else
            {
                nStart += BCA_SLOTS_ROW;
                nOff = nStart;
                pp = ppSlots + nOff;
                nBreak = nOff + nRowBreak;
            }
        }
    }
}


BOOL ScBroadcastAreaSlotMachine::AreaBroadcast( const ScHint& rHint ) const
{
    const ScAddress& rAddress = rHint.GetAddress();
    if ( rAddress == BCA_BRDCST_ALWAYS )
    {
        if ( pBCAlwaysList->Count() )
        {
            for ( ScBroadcastArea* pBCA = pBCAlwaysList->First(); pBCA; pBCA = pBCAlwaysList->Next() )
            {
                pBCA->Broadcast( rHint );
            }
            return TRUE;
        }
        else
            return FALSE;
    }
    else
    {
        ScBroadcastAreaSlot* pSlot = ppSlots[ ComputeSlotOffset( rAddress ) ];
        if ( pSlot )
            return pSlot->AreaBroadcast( rHint );
        else
            return FALSE;
    }
}


BOOL ScBroadcastAreaSlotMachine::AreaBroadcastInRange( const ScRange& rRange,
        const ScHint& rHint ) const
{
    BOOL bBroadcasted = FALSE;
    USHORT nStart, nEnd, nRowBreak;
    ComputeAreaPoints( rRange, nStart, nEnd, nRowBreak );
    USHORT nOff = nStart;
    USHORT nBreak = nOff + nRowBreak;
    ScBroadcastAreaSlot** pp = ppSlots + nOff;
    while ( nOff <= nEnd )
    {
        if ( *pp )
            bBroadcasted |= (*pp)->AreaBroadcastInRange( rRange, rHint );
        if ( nOff < nBreak )
        {
            ++nOff;
            ++pp;
        }
        else
        {
            nStart += BCA_SLOTS_ROW;
            nOff = nStart;
            pp = ppSlots + nOff;
            nBreak = nOff + nRowBreak;
        }
    }
    return bBroadcasted;
}


void ScBroadcastAreaSlotMachine::DelBroadcastAreasInRange(
        const ScRange& rRange
    )
{
    USHORT nStart, nEnd, nRowBreak;
    ComputeAreaPoints( rRange, nStart, nEnd, nRowBreak );
    USHORT nOff = nStart;
    USHORT nBreak = nOff + nRowBreak;
    ScBroadcastAreaSlot** pp = ppSlots + nOff;
    while ( nOff <= nEnd )
    {
        if ( *pp )
            (*pp)->DelBroadcastAreasInRange( rRange );
        if ( nOff < nBreak )
        {
            ++nOff;
            ++pp;
        }
        else
        {
            nStart += BCA_SLOTS_ROW;
            nOff = nStart;
            pp = ppSlots + nOff;
            nBreak = nOff + nRowBreak;
        }
    }
}


// alle Betroffenen austragen, verketten, Range anpassen, neu eintragen
void ScBroadcastAreaSlotMachine::UpdateBroadcastAreas(
        UpdateRefMode eUpdateRefMode,
        const ScRange& rRange, short nDx, short nDy, short nDz
    )
{
    USHORT nStart, nEnd, nRowBreak;
    // Betroffene austragen und verketten
    ComputeAreaPoints( rRange, nStart, nEnd, nRowBreak );
    USHORT nOff = nStart;
    USHORT nBreak = nOff + nRowBreak;
    ScBroadcastAreaSlot** pp = ppSlots + nOff;
    while ( nOff <= nEnd )
    {
        if ( *pp )
            (*pp)->UpdateRemove( eUpdateRefMode, rRange, nDx, nDy, nDz );
        if ( nOff < nBreak )
        {
            ++nOff;
            ++pp;
        }
        else
        {
            nStart += BCA_SLOTS_ROW;
            nOff = nStart;
            pp = ppSlots + nOff;
            nBreak = nOff + nRowBreak;
        }
    }
    // Verkettung abarbeiten
    USHORT nCol1, nRow1, nTab1, nCol2, nRow2, nTab2;
    USHORT theCol1, theRow1, theTab1, theCol2, theRow2, theTab2;
    nCol1 = rRange.aStart.Col();
    nRow1 = rRange.aStart.Row();
    nTab1 = rRange.aStart.Tab();
    nCol2 = rRange.aEnd.Col();
    nRow2 = rRange.aEnd.Row();
    nTab2 = rRange.aEnd.Tab();
    while ( pUpdateChain )
    {
        ScAddress aAdr;
        ScRange aRange;
        ScBroadcastArea* pArea = pUpdateChain;
        pUpdateChain = pArea->GetUpdateChainNext();

        // Range anpassen
        aAdr = pArea->GetStart();
        theCol1 = aAdr.Col();
        theRow1 = aAdr.Row();
        theTab1 = aAdr.Tab();
        aAdr = pArea->GetEnd();
        theCol2 = aAdr.Col();
        theRow2 = aAdr.Row();
        theTab2 = aAdr.Tab();
        if ( ScRefUpdate::Update( pDoc, eUpdateRefMode,
                nCol1,nRow1,nTab1, nCol2,nRow2,nTab2, nDx,nDy,nDz,
                theCol1,theRow1,theTab1, theCol2,theRow2,theTab2 )
            )
        {
            aRange = ScRange( ScAddress( theCol1,theRow1,theTab1 ),
                                ScAddress( theCol2,theRow2,theTab2 ) );
            pArea->UpdateRange( aRange );
            pArea->Broadcast( ScAreaChangedHint( aRange ) );    // fuer DDE
        }

        // in die Slots eintragen
        ComputeAreaPoints( aRange, nStart, nEnd, nRowBreak );
        nOff = nStart;
        nBreak = nOff + nRowBreak;
        pp = ppSlots + nOff;
        while ( nOff <= nEnd )
        {
            if ( *pp )
                (*pp)->UpdateInsert( pArea );
            if ( nOff < nBreak )
            {
                ++nOff;
                ++pp;
            }
            else
            {
                nStart += BCA_SLOTS_ROW;
                nOff = nStart;
                pp = ppSlots + nOff;
                nBreak = nOff + nRowBreak;
            }
        }

        // Verkettung loesen
        pArea->SetUpdateChainNext( NULL );
        pArea->SetInUpdateChain( FALSE );
    }
    pEOUpdateChain = NULL;
}

