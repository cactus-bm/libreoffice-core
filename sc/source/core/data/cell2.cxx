/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: cell2.cxx,v $
 *
 *  $Revision: 1.28 $
 *
 *  last change: $Author: ihi $ $Date: 2006-12-19 13:16:32 $
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



// INCLUDE ---------------------------------------------------------------

#ifndef _SV_MAPMOD_HXX //autogen
#include <vcl/mapmod.hxx>
#endif
#ifndef _EDITOBJ_HXX //autogen
#include <svx/editobj.hxx>
#endif
#ifndef _EDITSTAT_HXX //autogen
#include <svx/editstat.hxx>
#endif

#include "cell.hxx"
#include "compiler.hxx"
#include "errorcodes.hxx"
#include "document.hxx"
#include "rangenam.hxx"
#include "rechead.hxx"
#include "refupdat.hxx"
#include "scmatrix.hxx"
#include "editutil.hxx"
#include "chgtrack.hxx"
#include "indexmap.hxx"


// STATIC DATA -----------------------------------------------------------

#pragma code_seg("SCSTATICS")

#ifdef USE_MEMPOOL
const USHORT nMemPoolEditCell = (0x1000 - 64) / sizeof(ScNoteCell);
IMPL_FIXEDMEMPOOL_NEWDEL( ScEditCell, nMemPoolEditCell, nMemPoolEditCell )
#endif

#pragma code_seg()

// -----------------------------------------------------------------------

ScEditCell::ScEditCell( ScDocument* pDocP ) :
        ScBaseCell( CELLTYPE_EDIT ),
        pData( NULL ),
        pString( NULL ),
        pDoc( pDocP )
{
}

ScEditCell::ScEditCell( const EditTextObject* pObject, ScDocument* pDocP,
            const SfxItemPool* pFromPool )  :
        ScBaseCell( CELLTYPE_EDIT ),
        pString( NULL ),
        pDoc( pDocP )
{
    SetTextObject( pObject, pFromPool );
}

ScEditCell::ScEditCell( const ScEditCell& rEditCell, ScDocument* pDocP )  :
        ScBaseCell( rEditCell, pDocP),
        pString( NULL ),
        pDoc( pDocP )
{
    SetTextObject( rEditCell.pData, rEditCell.pDoc->GetEditPool() );
}

ScEditCell::ScEditCell( SvStream& rStream, USHORT nVer, ScDocument* pDocP ) :
        ScBaseCell( CELLTYPE_EDIT ),
        pString( NULL ),
        pDoc( pDocP )
{
    if( nVer >= SC_DATABYTES2 )
    {
        BYTE cData;
        rStream >> cData;
        if( cData & 0x0F )
            rStream.SeekRel( cData & 0x0F );
    }
    if ( nVer < SC_VERSION_EDITPOOL )
    {   // jedes hat seinen eigenen Pool, dem neuen zuordnen
        EditTextObject* pTmp = EditTextObject::Create( rStream );
        SetTextObject( pTmp, NULL );
        delete pTmp;
    }
    else
        pData = EditTextObject::Create( rStream, pDoc->GetEditPool() );
}

ScEditCell::ScEditCell( const String& rString, ScDocument* pDocP )  :
        ScBaseCell( CELLTYPE_EDIT ),
        pString( NULL ),
        pDoc( pDocP )
{
    DBG_ASSERT( rString.Search('\n') != STRING_NOTFOUND ||
                rString.Search(CHAR_CR) != STRING_NOTFOUND,
                "EditCell mit einfachem Text !?!?" );

    EditEngine& rEngine = pDoc->GetEditEngine();
    rEngine.SetText( rString );
    pData = rEngine.CreateTextObject();
}

ScEditCell::~ScEditCell()
{
    delete pData;
    delete pString;

#ifdef DBG_UTIL
    eCellType = CELLTYPE_DESTROYED;
#endif
}

ScBaseCell* ScEditCell::Clone( ScDocument* pNewDoc ) const
{
    return new ScEditCell( *this, pNewDoc );
}

void ScEditCell::SetData( const EditTextObject* pObject,
            const SfxItemPool* pFromPool )
{
    if ( pString )
    {
        delete pString;
        pString = NULL;
    }
    delete pData;
    SetTextObject( pObject, pFromPool );
}

void ScEditCell::GetData( const EditTextObject*& rpObject ) const
{
    rpObject = pData;
}

void ScEditCell::GetString( String& rString ) const
{
    if ( pString )
        rString = *pString;
    else if ( pData )
    {
        // auch Text von URL-Feldern, Doc-Engine ist eine ScFieldEditEngine
        EditEngine& rEngine = pDoc->GetEditEngine();
        rEngine.SetText( *pData );
        rString = ScEditUtil::GetSpaceDelimitedString(rEngine);     // space between paragraphs
        // kurze Strings fuer Formeln merken
        if ( rString.Len() < MAXSTRLEN )
            ((ScEditCell*)this)->pString = new String( rString );   //! non-const
    }
    else
        rString.Erase();
}

void ScEditCell::Save( SvStream& rStream ) const
{
    DBG_ASSERT(pData,"StoreTextObject(NULL)");
    rStream << (BYTE) 0x00;
    if ( rStream.GetVersion() < SOFFICE_FILEFORMAT_50 )
    {   // jedem seinen eigenen Pool
        ScEditEngineDefaulter aEngine( EditEngine::CreatePool(), TRUE );
        // #52396# richtige Metric schreiben
        aEngine.SetRefMapMode( MAP_100TH_MM );
        aEngine.SetText( *pData );
        EditTextObject* pTmp = aEngine.CreateTextObject();
        pTmp->Store( rStream );
        delete pTmp;
    }
    else
        pData->Store( rStream );
}

void ScEditCell::SetTextObject( const EditTextObject* pObject,
            const SfxItemPool* pFromPool )
{
    if ( pObject )
    {
        if ( pFromPool && pDoc->GetEditPool() == pFromPool )
            pData = pObject->Clone();
        else
        {   //! anderer Pool
            // Leider gibt es keinen anderen Weg, um den Pool umzuhaengen,
            // als das Object durch eine entsprechende Engine zu schleusen..
            EditEngine& rEngine = pDoc->GetEditEngine();
            if ( pObject->HasOnlineSpellErrors() )
            {
                ULONG nControl = rEngine.GetControlWord();
                const ULONG nSpellControl = EE_CNTRL_ONLINESPELLING | EE_CNTRL_ALLOWBIGOBJS;
                BOOL bNewControl = ( (nControl & nSpellControl) != nSpellControl );
                if ( bNewControl )
                    rEngine.SetControlWord( nControl | nSpellControl );
                rEngine.SetText( *pObject );
                pData = rEngine.CreateTextObject();
                if ( bNewControl )
                    rEngine.SetControlWord( nControl );
            }
            else
            {
                rEngine.SetText( *pObject );
                pData = rEngine.CreateTextObject();
            }
        }
    }
    else
        pData = NULL;
}

//---------------------------------------------------------------------

void ScFormulaCell::GetEnglishFormula( String& rFormula, BOOL bCompileXML,
                                       ScAddress::Convention conv ) const
{
    rtl::OUStringBuffer rBuffer( rFormula );
    GetEnglishFormula( rBuffer, bCompileXML, conv );
    rFormula = rBuffer;
}


void ScFormulaCell::GetEnglishFormulaForPof( rtl::OUStringBuffer &rBuffer,
        const ScAddress &rPos, BOOL bCompileXML ) const
{
    ScTokenArray *pCompileCode = pCode;
    if (bCompileXML)
    {
        /* Scan pCode [ token array ! ] for missing args &
           re-write if present */
        if (pCode->NeedsPofRewrite())
            pCompileCode = pCode->RewriteMissingToPof();
    }

    ScCompiler aComp( pDocument, rPos, *pCompileCode );
    aComp.SetCompileEnglish( TRUE );
    aComp.SetCompileXML( bCompileXML );
    aComp.CreateStringFromTokenArray( rBuffer );

    if ( pCompileCode != pCode )
        delete pCompileCode;
}

void ScFormulaCell::GetEnglishFormula( rtl::OUStringBuffer& rBuffer, BOOL bCompileXML,
                                       ScAddress::Convention conv ) const
{
    //! mit GetFormula zusammenfassen !!!

    if( pCode->GetError() && !pCode->GetLen() )
    {
        rBuffer = rtl::OUStringBuffer( ScGlobal::GetErrorString( pCode->GetError()));
        return;
    }
    else if( cMatrixFlag == MM_REFERENCE )
    {
        // Referenz auf eine andere Zelle, die eine Matrixformel enthaelt
        pCode->Reset();
        ScToken* p = pCode->GetNextReferenceRPN();
        if( p )
        {
            ScBaseCell* pCell;
            SingleRefData& rRef = p->GetSingleRef();
            rRef.CalcAbsIfRel( aPos );
            if ( rRef.Valid() )
                pCell = pDocument->GetCell( ScAddress( rRef.nCol,
                    rRef.nRow, rRef.nTab ) );
            else
                pCell = NULL;
            if (pCell && pCell->GetCellType() == CELLTYPE_FORMULA)
            {
                ((ScFormulaCell*)pCell)->GetEnglishFormula( rBuffer, bCompileXML, conv);
                return;
            }
            else
                GetEnglishFormulaForPof( rBuffer, aPos, bCompileXML);
        }
        else
        {
            DBG_ERROR("ScFormulaCell::GetEnglishFormula: Keine Matrix");
        }
    }
    else
        GetEnglishFormulaForPof( rBuffer, aPos, bCompileXML);

    sal_Unicode ch('=');
    rBuffer.insert( 0, &ch, 1 );
    if( cMatrixFlag )
    {
        sal_Unicode ch2('{');
        rBuffer.insert( 0, &ch2, 1);
        rBuffer.append( sal_Unicode('}'));
    }
}

BOOL ScFormulaCell::IsValue()
{
    if (IsDirtyOrInTableOpDirty() && pDocument->GetAutoCalc())
        Interpret();
    return bIsValue;
}

double ScFormulaCell::GetValue()
{
    if (IsDirtyOrInTableOpDirty() && pDocument->GetAutoCalc())
        Interpret();
    if ( !pCode->GetError() || pCode->GetError() == errDoubleRef)
        return nErgValue;
    return 0.0;
}

double ScFormulaCell::GetValueAlways()
{
    // for goal seek: return result value even if error code is set

    if (IsDirtyOrInTableOpDirty() && pDocument->GetAutoCalc())
        Interpret();
    return nErgValue;
}

void ScFormulaCell::GetString( String& rString )
{
    if (IsDirtyOrInTableOpDirty() && pDocument->GetAutoCalc())
        Interpret();
    if ( !pCode->GetError() || pCode->GetError() == errDoubleRef)
        rString = aErgString;
    else
        rString.Erase();
}

const ScMatrix* ScFormulaCell::GetMatrix()
{
    if ( pDocument->GetAutoCalc() )
    {
        // war !bDirty gespeichert aber zugehoerige Matrixzelle bDirty?
        // => wir brauchen pMatrix
        if ( !pMatrix && cMatrixFlag == MM_FORMULA )
            bDirty = TRUE;
        if ( IsDirtyOrInTableOpDirty() )
            Interpret();
    }
    return pMatrix;
}

BOOL ScFormulaCell::GetMatrixOrigin( ScAddress& rPos ) const
{
    switch ( cMatrixFlag )
    {
        case MM_FORMULA :
            rPos = aPos;
            return TRUE;
        break;
        case MM_REFERENCE :
        {
            pCode->Reset();
            ScToken* t = pCode->GetNextReferenceRPN();
            if( t )
            {
                SingleRefData& rRef = t->GetSingleRef();
                rRef.CalcAbsIfRel( aPos );
                if ( rRef.Valid() )
                {
                    rPos.Set( rRef.nCol, rRef.nRow, rRef.nTab );
                    return TRUE;
                }
            }
        }
        break;
    }
    return FALSE;
}


/*
 Edge-Values:

   8
 4   16
   2

 innerhalb: 1
 ausserhalb: 0
 (reserviert: offen: 32)
 */

USHORT ScFormulaCell::GetMatrixEdge( ScAddress& rOrgPos )
{
    switch ( cMatrixFlag )
    {
        case MM_FORMULA :
        case MM_REFERENCE :
        {
            static SCCOL nC;
            static SCROW nR;
            ScAddress aOrg;
            if ( !GetMatrixOrigin( aOrg ) )
                return 0;               // dumm gelaufen..
            if ( aOrg != rOrgPos )
            {   // erstes Mal oder andere Matrix als letztes Mal
                rOrgPos = aOrg;
                ScFormulaCell* pFCell;
                if ( cMatrixFlag == MM_REFERENCE )
                    pFCell = (ScFormulaCell*) pDocument->GetCell( aOrg );
                else
                    pFCell = this;      // this MM_FORMULA
                // this gibt's nur einmal, kein Vergleich auf pFCell==this
                if ( pFCell && pFCell->GetCellType() == CELLTYPE_FORMULA
                  && pFCell->cMatrixFlag == MM_FORMULA )
                {
                    pFCell->GetMatColsRows( nC, nR );
                    if ( nC == 0 || nR == 0 )
                    {   // aus altem Dokument geladen, neu erzeugen
                        nC = 1;
                        nR = 1;
                        ScAddress aTmpOrg;
                        ScBaseCell* pCell;
                        ScAddress aAdr( aOrg );
                        aAdr.IncCol();
                        BOOL bCont = TRUE;
                        do
                        {
                            pCell = pDocument->GetCell( aAdr );
                            if ( pCell && pCell->GetCellType() == CELLTYPE_FORMULA
                              && ((ScFormulaCell*)pCell)->cMatrixFlag == MM_REFERENCE
                              && GetMatrixOrigin( aTmpOrg ) && aTmpOrg == aOrg )
                            {
                                nC++;
                                aAdr.IncCol();
                            }
                            else
                                bCont = FALSE;
                        } while ( bCont );
                        aAdr = aOrg;
                        aAdr.IncRow();
                        bCont = TRUE;
                        do
                        {
                            pCell = pDocument->GetCell( aAdr );
                            if ( pCell && pCell->GetCellType() == CELLTYPE_FORMULA
                              && ((ScFormulaCell*)pCell)->cMatrixFlag == MM_REFERENCE
                              && GetMatrixOrigin( aTmpOrg ) && aTmpOrg == aOrg )
                            {
                                nR++;
                                aAdr.IncRow();
                            }
                            else
                                bCont = FALSE;
                        } while ( bCont );
                        pFCell->SetMatColsRows( nC, nR );
                    }
                }
                else
                {
#ifndef PRODUCT
                    String aTmp;
                    ByteString aMsg( "broken Matrix, no MatFormula at origin, Pos: " );
                    aPos.Format( aTmp, SCA_VALID_COL | SCA_VALID_ROW, pDocument );
                    aMsg += ByteString( aTmp, RTL_TEXTENCODING_ASCII_US );
                    aMsg += ", MatOrg: ";
                    aOrg.Format( aTmp, SCA_VALID_COL | SCA_VALID_ROW, pDocument );
                    aMsg += ByteString( aTmp, RTL_TEXTENCODING_ASCII_US );
                    DBG_ERRORFILE( aMsg.GetBuffer() );
#endif
                    return 0;           // bad luck ...
                }
            }
            // here we are, healthy and clean, somewhere in between
            SCsCOL dC = aPos.Col() - aOrg.Col();
            SCsROW dR = aPos.Row() - aOrg.Row();
            USHORT nEdges = 0;
            if ( dC >= 0 && dR >= 0 && dC < nC && dR < nR )
            {
                if ( dC == 0 )
                    nEdges |= 4;            // linke Kante
                if ( dC+1 == nC )
                    nEdges |= 16;           // rechte Kante
                if ( dR == 0 )
                    nEdges |= 8;            // obere Kante
                if ( dR+1 == nR )
                    nEdges |= 2;            // untere Kante
                if ( !nEdges )
                    nEdges = 1;             // mittendrin
            }
#ifndef PRODUCT
            else
            {
                String aTmp;
                ByteString aMsg( "broken Matrix, Pos: " );
                aPos.Format( aTmp, SCA_VALID_COL | SCA_VALID_ROW, pDocument );
                aMsg += ByteString( aTmp, RTL_TEXTENCODING_ASCII_US );
                aMsg += ", MatOrg: ";
                aOrg.Format( aTmp, SCA_VALID_COL | SCA_VALID_ROW, pDocument );
                aMsg += ByteString( aTmp, RTL_TEXTENCODING_ASCII_US );
                aMsg += ", MatCols: ";
                aMsg += ByteString::CreateFromInt32( nC );
                aMsg += ", MatRows: ";
                aMsg += ByteString::CreateFromInt32( nR );
                aMsg += ", DiffCols: ";
                aMsg += ByteString::CreateFromInt32( dC );
                aMsg += ", DiffRows: ";
                aMsg += ByteString::CreateFromInt32( dR );
                DBG_ERRORFILE( aMsg.GetBuffer() );
            }
#endif
            return nEdges;
            break;
        }
        default:
            return 0;
    }
}

USHORT ScFormulaCell::GetErrCode()
{
    if (IsDirtyOrInTableOpDirty() && pDocument->GetAutoCalc())
        Interpret();
    return pCode->GetError();
}

BOOL ScFormulaCell::HasOneReference( ScRange& r ) const
{
    pCode->Reset();
    ScToken* p = pCode->GetNextReferenceRPN();
    if( p && !pCode->GetNextReferenceRPN() )        // nur eine!
    {
        p->CalcAbsIfRel( aPos );
        SingleDoubleRefProvider aProv( *p );
        r.aStart.Set( aProv.Ref1.nCol,
                      aProv.Ref1.nRow,
                      aProv.Ref1.nTab );
        r.aEnd.Set( aProv.Ref2.nCol,
                    aProv.Ref2.nRow,
                    aProv.Ref2.nTab );
        return TRUE;
    }
    else
        return FALSE;
}

BOOL ScFormulaCell::HasRelNameReference() const
{
    pCode->Reset();
    for( ScToken* t = pCode->GetNextReferenceRPN(); t;
                  t = pCode->GetNextReferenceRPN() )
    {
        if ( t->GetSingleRef().IsRelName() ||
                (t->GetType() == svDoubleRef &&
                t->GetDoubleRef().Ref2.IsRelName()) )
            return TRUE;
    }
    return FALSE;
}

BOOL ScFormulaCell::HasDBArea() const
{
    pCode->Reset();
    return (pCode->GetNextDBArea() != NULL);
}

BOOL ScFormulaCell::HasColRowName() const
{
    pCode->Reset();
    return (pCode->GetNextColRowName() != NULL);
}

void ScFormulaCell::UpdateReference(UpdateRefMode eUpdateRefMode,
                                    const ScRange& r,
                                    SCsCOL nDx, SCsROW nDy, SCsTAB nDz,
                                    ScDocument* pUndoDoc )
{
    SCCOL nCol1 = r.aStart.Col();
    SCROW nRow1 = r.aStart.Row();
    SCTAB nTab1 = r.aStart.Tab();
    SCCOL nCol2 = r.aEnd.Col();
    SCROW nRow2 = r.aEnd.Row();
    SCTAB nTab2 = r.aEnd.Tab();
    SCCOL nCol = aPos.Col();
    SCROW nRow = aPos.Row();
    SCTAB nTab = aPos.Tab();
    ScAddress aUndoPos( aPos );         // position for undo cell in pUndoDoc
    ScAddress aOldPos( aPos );
//  BOOL bPosChanged = FALSE;           // ob diese Zelle bewegt wurde
    BOOL bIsInsert = FALSE;
    if (eUpdateRefMode == URM_INSDEL)
    {
        bIsInsert = (nDx >= 0 && nDy >= 0 && nDz >= 0);
        if ( nDx && nRow >= nRow1 && nRow <= nRow2 &&
            nTab >= nTab1 && nTab <= nTab2 )
        {
            if (nCol >= nCol1)
            {
                nCol += nDx;
                if ((SCsCOL) nCol < 0)
                    nCol = 0;
                else if ( nCol > MAXCOL )
                    nCol = MAXCOL;
                aPos.SetCol( nCol );
//              bPosChanged = TRUE;
            }
        }
        if ( nDy && nCol >= nCol1 && nCol <= nCol2 &&
            nTab >= nTab1 && nTab <= nTab2 )
        {
            if (nRow >= nRow1)
            {
                nRow += nDy;
                if ((SCsROW) nRow < 0)
                    nRow = 0;
                else if ( nRow > MAXROW )
                    nRow = MAXROW;
                aPos.SetRow( nRow );
//              bPosChanged = TRUE;
            }
        }
        if ( nDz && nCol >= nCol1 && nCol <= nCol2 &&
            nRow >= nRow1 && nRow <= nRow2 )
        {
            if (nTab >= nTab1)
            {
                SCTAB nMaxTab = pDocument->GetTableCount() - 1;
                nTab += nDz;
                if ((SCsTAB) nTab < 0)
                    nTab = 0;
                else if ( nTab > nMaxTab )
                    nTab = nMaxTab;
                aPos.SetTab( nTab );
//              bPosChanged = TRUE;
            }
        }
    }
    else if ( r.In( aPos ) )
    {
        aOldPos.Set( nCol - nDx, nRow - nDy, nTab - nDz );
//      bPosChanged = TRUE;
    }

    BOOL bHasRefs = FALSE;
    BOOL bHasColRowNames = FALSE;
    BOOL bOnRefMove = FALSE;
    if ( !pDocument->IsClipOrUndo() )
    {
        pCode->Reset();
        bHasRefs = (pCode->GetNextReferenceRPN() != NULL);
        if ( !bHasRefs || eUpdateRefMode == URM_COPY )
        {
            pCode->Reset();
            bHasColRowNames = (pCode->GetNextColRowName() != NULL);
            bHasRefs = bHasRefs || bHasColRowNames;
        }
        bOnRefMove = pCode->IsRecalcModeOnRefMove();
    }
    if( bHasRefs || bOnRefMove )
    {
        ScTokenArray* pOld = pUndoDoc ? pCode->Clone() : NULL;
        BOOL bValChanged;
        ScRangeData* pRangeData;
        BOOL bRangeModified;            // any range, not only shared formula
        BOOL bRefSizeChanged;
        if ( bHasRefs )
        {
            ScCompiler aComp(pDocument, aPos, *pCode);
            pRangeData = aComp.UpdateReference(eUpdateRefMode, aOldPos, r,
                                             nDx, nDy, nDz,
                                             bValChanged, bRefSizeChanged);
            bRangeModified = aComp.HasModifiedRange();
        }
        else
        {
            bValChanged = FALSE;
            pRangeData = NULL;
            bRangeModified = FALSE;
            bRefSizeChanged = FALSE;
        }
        if ( bOnRefMove )
            bOnRefMove = (bValChanged || (aPos != aOldPos));
            // Cell may reference itself, e.g. ocColumn, ocRow without parameter

        BOOL bColRowNameCompile, bHasRelName, bNewListening, bInDeleteUndo;
        if ( bHasRefs )
        {
            // Upon Insert ColRowNames have to be recompiled in case the
            // insertion occurs right in front of the range.
            bColRowNameCompile =
                (eUpdateRefMode == URM_INSDEL && (nDx > 0 || nDy > 0));
            if ( bColRowNameCompile )
            {
                bColRowNameCompile = FALSE;
                ScToken* t;
                ScRangePairList* pColList = pDocument->GetColNameRanges();
                ScRangePairList* pRowList = pDocument->GetRowNameRanges();
                pCode->Reset();
                while ( !bColRowNameCompile && (t = pCode->GetNextColRowName()) )
                {
                    SingleRefData& rRef = t->GetSingleRef();
                    if ( nDy > 0 && rRef.IsColRel() )
                    {   // ColName
                        rRef.CalcAbsIfRel( aPos );
                        ScAddress aAdr( rRef.nCol, rRef.nRow, rRef.nTab );
                        ScRangePair* pR = pColList->Find( aAdr );
                        if ( pR )
                        {   // definiert
                            if ( pR->GetRange(1).aStart.Row() == nRow1 )
                                bColRowNameCompile = TRUE;
                        }
                        else
                        {   // on the fly
                            if ( rRef.nRow + 1 == nRow1 )
                                bColRowNameCompile = TRUE;
                        }
                    }
                    if ( nDx > 0 && rRef.IsRowRel() )
                    {   // RowName
                        rRef.CalcAbsIfRel( aPos );
                        ScAddress aAdr( rRef.nCol, rRef.nRow, rRef.nTab );
                        ScRangePair* pR = pRowList->Find( aAdr );
                        if ( pR )
                        {   // definiert
                            if ( pR->GetRange(1).aStart.Col() == nCol1 )
                                bColRowNameCompile = TRUE;
                        }
                        else
                        {   // on the fly
                            if ( rRef.nCol + 1 == nCol1 )
                                bColRowNameCompile = TRUE;
                        }
                    }
                }
            }
            else if ( eUpdateRefMode == URM_MOVE )
            {   // bei Move/D&D neu kompilieren wenn ColRowName verschoben wurde
                // oder diese Zelle auf einen zeigt und verschoben wurde
                bColRowNameCompile = bCompile;      // evtl. aus Copy-ctor
                if ( !bColRowNameCompile )
                {
                    BOOL bMoved = (aPos != aOldPos);
                    pCode->Reset();
                    ScToken* t = pCode->GetNextColRowName();
                    if ( t && bMoved )
                        bColRowNameCompile = TRUE;
                    while ( t && !bColRowNameCompile )
                    {
                        SingleRefData& rRef = t->GetSingleRef();
                        rRef.CalcAbsIfRel( aPos );
                        if ( rRef.Valid() )
                        {
                            ScAddress aAdr( rRef.nCol, rRef.nRow, rRef.nTab );
                            if ( r.In( aAdr ) )
                                bColRowNameCompile = TRUE;
                        }
                        t = pCode->GetNextColRowName();
                    }
                }
            }
            else if ( eUpdateRefMode == URM_COPY && bHasColRowNames && bValChanged )
            {
                bColRowNameCompile = TRUE;
            }
            ScChangeTrack* pChangeTrack = pDocument->GetChangeTrack();
            if ( pChangeTrack && pChangeTrack->IsInDeleteUndo() )
                bInDeleteUndo = TRUE;
            else
                bInDeleteUndo = FALSE;
            // RelNameRefs are always moved
            bHasRelName = HasRelNameReference();
            // Reference changed and new listening needed?
            // Except in Insert/Delete without specialties.
            bNewListening = (bRangeModified || pRangeData || bColRowNameCompile
                    || (bValChanged && (eUpdateRefMode != URM_INSDEL ||
                            bInDeleteUndo || bRefSizeChanged)) ||
                    (bHasRelName && eUpdateRefMode != URM_COPY))
                // #i36299# Don't duplicate action during cut&paste / drag&drop
                // on a cell in the range moved, start/end listeners is done
                // via ScDocument::DeleteArea() and ScDocument::CopyFromClip().
                && !(eUpdateRefMode == URM_MOVE &&
                        pDocument->IsInsertingFromOtherDoc() && r.In(aPos));
            if ( bNewListening )
                EndListeningTo( pDocument, pOld, aOldPos );
        }
        else
        {
            bColRowNameCompile = bHasRelName = bNewListening = bInDeleteUndo =
                FALSE;
        }

        BOOL bNeedDirty;
        // NeedDirty bei Aenderungen ausser Copy und Move/Insert ohne RelNames
        if ( bRangeModified || pRangeData || bColRowNameCompile ||
                (bValChanged && eUpdateRefMode != URM_COPY &&
                 (eUpdateRefMode != URM_MOVE || bHasRelName) &&
                 (!bIsInsert || bHasRelName || bInDeleteUndo ||
                  bRefSizeChanged)) || bOnRefMove)
            bNeedDirty = TRUE;
        else
            bNeedDirty = FALSE;
        if (pUndoDoc && (bValChanged || pRangeData || bOnRefMove))
        {
            //  Copy the cell to aUndoPos, which is its current position in the document,
            //  so this works when UpdateReference is called before moving the cells
            //  (InsertCells/DeleteCells - aPos is changed above) as well as when UpdateReference
            //  is called after moving the cells (MoveBlock/PasteFromClip - aOldPos is changed).

            ScFormulaCell* pFCell = new ScFormulaCell( pUndoDoc, aUndoPos, pOld, cMatrixFlag );
            pFCell->nErgValue = MINDOUBLE;      // to recognize it as changed later (Cut/Paste!)
            pUndoDoc->PutCell( aUndoPos, pFCell );
        }
        bValChanged = FALSE;
        if ( pRangeData )
        {   // Replace shared formula with own formula
            pDocument->RemoveFromFormulaTree( this );   // update formula count
            delete pCode;
            pCode = pRangeData->GetCode()->Clone();
            ScCompiler aComp2(pDocument, aPos, *pCode);
            aComp2.UpdateSharedFormulaReference( eUpdateRefMode, aOldPos, r,
                nDx, nDy, nDz );
            bValChanged = TRUE;
            bNeedDirty = TRUE;
        }
        if ( ( bCompile = (bCompile || bValChanged || bRangeModified || bColRowNameCompile) ) != 0 )
        {
            CompileTokenArray( bNewListening ); // kein Listening
            bNeedDirty = TRUE;
        }
        if ( !bInDeleteUndo )
        {   // In ChangeTrack Delete-Reject listeners are established in
            // InsertCol/InsertRow
            if ( bNewListening )
            {
                if ( eUpdateRefMode == URM_INSDEL )
                {
                    // Inserts/Deletes re-establish listeners after all
                    // UpdateReference calls.
                    // All replaced shared formula listeners have to be
                    // established after an Insert or Delete. Do nothing here.
                    SetNeedsListening( TRUE);
                }
                else
                    StartListeningTo( pDocument );
            }
        }
        if ( bNeedDirty && (!(eUpdateRefMode == URM_INSDEL && bHasRelName) || pRangeData) )
        {   // Referenzen abgeschnitten, ungueltig o.ae.?
            BOOL bOldAutoCalc = pDocument->GetAutoCalc();
            // kein Interpret in SubMinimalRecalc wegen evtl. falscher Referenzen
            pDocument->SetAutoCalc( FALSE );
            SetDirty();
            pDocument->SetAutoCalc( bOldAutoCalc );
        }

        delete pOld;
    }
}

void ScFormulaCell::UpdateInsertTab(SCTAB nTable)
{
    BOOL bPosChanged = ( aPos.Tab() >= nTable ? TRUE : FALSE );
    pCode->Reset();
    if( pCode->GetNextReferenceRPN() && !pDocument->IsClipOrUndo() )
    {
        EndListeningTo( pDocument );
        // IncTab _nach_ EndListeningTo und _vor_ Compiler UpdateInsertTab !
        if ( bPosChanged )
            aPos.IncTab();
        ScRangeData* pRangeData;
        ScCompiler aComp(pDocument, aPos, *pCode);
        pRangeData = aComp.UpdateInsertTab( nTable, FALSE );
        if (pRangeData)                     // Shared Formula gegen echte Formel
        {                                   // austauschen
            BOOL bChanged;
            pDocument->RemoveFromFormulaTree( this );   // update formula count
            delete pCode;
            pCode = new ScTokenArray( *pRangeData->GetCode() );
            ScCompiler aComp2(pDocument, aPos, *pCode);
            aComp2.MoveRelWrap();
            aComp2.UpdateInsertTab( nTable, FALSE );
            // If the shared formula contained a named range/formula containing
            // an absolute reference to a sheet, those have to be readjusted.
            aComp2.UpdateDeleteTab( nTable, FALSE, TRUE, bChanged );
            bCompile = TRUE;
        }
        // kein StartListeningTo weil pTab[nTab] noch nicht existiert!
    }
    else if ( bPosChanged )
        aPos.IncTab();
}

BOOL ScFormulaCell::UpdateDeleteTab(SCTAB nTable, BOOL bIsMove)
{
    BOOL bChanged = FALSE;
    BOOL bPosChanged = ( aPos.Tab() > nTable ? TRUE : FALSE );
    pCode->Reset();
    if( pCode->GetNextReferenceRPN() && !pDocument->IsClipOrUndo() )
    {
        EndListeningTo( pDocument );
        // IncTab _nach_ EndListeningTo und _vor_ Compiler UpdateDeleteTab !
        if ( bPosChanged )
            aPos.IncTab(-1);
        ScRangeData* pRangeData;
        ScCompiler aComp(pDocument, aPos, *pCode);
        pRangeData = aComp.UpdateDeleteTab(nTable, bIsMove, FALSE, bChanged);
        if (pRangeData)                     // Shared Formula gegen echte Formel
        {                                   // austauschen
            pDocument->RemoveFromFormulaTree( this );   // update formula count
            delete pCode;
            pCode = pRangeData->GetCode()->Clone();
            ScCompiler aComp2(pDocument, aPos, *pCode);
            aComp2.CompileTokenArray();
            aComp2.MoveRelWrap();
            aComp2.UpdateDeleteTab( nTable, FALSE, FALSE, bChanged );
            // If the shared formula contained a named range/formula containing
            // an absolute reference to a sheet, those have to be readjusted.
            aComp2.UpdateInsertTab( nTable,TRUE );
            // bChanged kann beim letzten UpdateDeleteTab zurueckgesetzt worden sein
            bChanged = TRUE;
            bCompile = TRUE;
        }
        // kein StartListeningTo weil pTab[nTab] noch nicht korrekt!
    }
    else if ( bPosChanged )
        aPos.IncTab(-1);

    return bChanged;
}

void ScFormulaCell::UpdateMoveTab( SCTAB nOldPos, SCTAB nNewPos, SCTAB nTabNo )
{
    pCode->Reset();
    if( pCode->GetNextReferenceRPN() && !pDocument->IsClipOrUndo() )
    {
        EndListeningTo( pDocument );
        // SetTab _nach_ EndListeningTo und _vor_ Compiler UpdateMoveTab !
        aPos.SetTab( nTabNo );
        ScRangeData* pRangeData;
        ScCompiler aComp(pDocument, aPos, *pCode);
        pRangeData = aComp.UpdateMoveTab( nOldPos, nNewPos, FALSE );
        if (pRangeData)                     // Shared Formula gegen echte Formel
        {                                   // austauschen
            pDocument->RemoveFromFormulaTree( this );   // update formula count
            delete pCode;
            pCode = pRangeData->GetCode()->Clone();
            ScCompiler aComp2(pDocument, aPos, *pCode);
            aComp2.CompileTokenArray();
            aComp2.MoveRelWrap();
            aComp2.UpdateMoveTab( nOldPos, nNewPos, TRUE );
            bCompile = TRUE;
        }
        // kein StartListeningTo weil pTab[nTab] noch nicht korrekt!
    }
    else
        aPos.SetTab( nTabNo );
}

void ScFormulaCell::UpdateInsertTabAbs(SCTAB nTable)
{
    if( !pDocument->IsClipOrUndo() )
    {
        pCode->Reset();
        ScToken* p = pCode->GetNextReferenceRPN();
        while( p )
        {
            SingleRefData& rRef1 = p->GetSingleRef();
            if( !rRef1.IsTabRel() && (SCsTAB) nTable <= rRef1.nTab )
                rRef1.nTab++;
            if( p->GetType() == svDoubleRef )
            {
                SingleRefData& rRef2 = p->GetDoubleRef().Ref2;
                if( !rRef2.IsTabRel() && (SCsTAB) nTable <= rRef2.nTab )
                    rRef2.nTab++;
            }
            p = pCode->GetNextReferenceRPN();
        }
    }
}

BOOL ScFormulaCell::TestTabRefAbs(SCTAB nTable)
{
    BOOL bRet = FALSE;
    if( !pDocument->IsClipOrUndo() )
    {
        pCode->Reset();
        ScToken* p = pCode->GetNextReferenceRPN();
        while( p )
        {
            SingleRefData& rRef1 = p->GetSingleRef();
            if( !rRef1.IsTabRel() )
            {
                if( (SCsTAB) nTable != rRef1.nTab )
                    bRet = TRUE;
                else if (nTable != aPos.Tab())
                    rRef1.nTab = aPos.Tab();
            }
            if( p->GetType() == svDoubleRef )
            {
                SingleRefData& rRef2 = p->GetDoubleRef().Ref2;
                if( !rRef2.IsTabRel() )
                {
                    if( (SCsTAB) nTable != rRef2.nTab )
                        bRet = TRUE;
                    else if (nTable != aPos.Tab())
                        rRef2.nTab = aPos.Tab();
                }
            }
            p = pCode->GetNextReferenceRPN();
        }
    }
    return bRet;
}

void ScFormulaCell::UpdateCompile( BOOL bForceIfNameInUse )
{
    if ( bForceIfNameInUse && !bCompile )
        bCompile = pCode->HasNameOrColRowName();
    if ( bCompile )
        pCode->SetError( 0 );   // damit auch wirklich kompiliert wird
    CompileTokenArray();
}

//  Referenzen transponieren - wird nur in Clipboard-Dokumenten aufgerufen

void ScFormulaCell::TransposeReference()
{
    BOOL bFound = FALSE;
    pCode->Reset();
    for( ScToken* t = pCode->GetNextReference(); t;
                  t = pCode->GetNextReference() )
    {
        SingleRefData& rRef1 = t->GetSingleRef();
        if ( rRef1.IsColRel() && rRef1.IsRowRel() )
        {
            BOOL bDouble = (t->GetType() == svDoubleRef);
            SingleRefData& rRef2 = (bDouble ? t->GetDoubleRef().Ref2 : rRef1);
            if ( !bDouble || (rRef2.IsColRel() && rRef2.IsRowRel()) )
            {
                INT16 nTemp;

                nTemp = rRef1.nRelCol;
                rRef1.nRelCol = static_cast<SCCOL>(rRef1.nRelRow);
                rRef1.nRelRow = static_cast<SCROW>(nTemp);

                if ( bDouble )
                {
                    nTemp = rRef2.nRelCol;
                    rRef2.nRelCol = static_cast<SCCOL>(rRef2.nRelRow);
                    rRef2.nRelRow = static_cast<SCROW>(nTemp);
                }

                bFound = TRUE;
            }
        }
    }

    if (bFound)
        bCompile = TRUE;
}

void ScFormulaCell::UpdateTranspose( const ScRange& rSource, const ScAddress& rDest,
                                        ScDocument* pUndoDoc )
{
    EndListeningTo( pDocument );

    ScAddress aOldPos = aPos;
    BOOL bPosChanged = FALSE;           // ob diese Zelle bewegt wurde

    ScRange aDestRange( rDest, ScAddress(
                static_cast<SCCOL>(rDest.Col() + rSource.aEnd.Row() - rSource.aStart.Row()),
                static_cast<SCROW>(rDest.Row() + rSource.aEnd.Col() - rSource.aStart.Col()),
                rDest.Tab() + rSource.aEnd.Tab() - rSource.aStart.Tab() ) );
    if ( aDestRange.In( aOldPos ) )
    {
        //  Position zurueckrechnen
        SCsCOL nRelPosX = aOldPos.Col();
        SCsROW nRelPosY = aOldPos.Row();
        SCsTAB nRelPosZ = aOldPos.Tab();
        ScRefUpdate::DoTranspose( nRelPosX, nRelPosY, nRelPosZ, pDocument, aDestRange, rSource.aStart );
        aOldPos.Set( nRelPosX, nRelPosY, nRelPosZ );
        bPosChanged = TRUE;
    }

    ScTokenArray* pOld = pUndoDoc ? pCode->Clone() : NULL;
    BOOL bChanged = FALSE;
    ScToken* t;

    ScRangeData* pShared = NULL;
    pCode->Reset();
    for( t = pCode->GetNextReferenceOrName(); t; t = pCode->GetNextReferenceOrName() )
    {
        if( t->GetOpCode() == ocName )
        {
            ScRangeData* pName = pDocument->GetRangeName()->FindIndex( t->GetIndex() );
            if (pName)
            {
                if (pName->IsModified())
                    bChanged = TRUE;
                if (pName->HasType(RT_SHAREDMOD))
                    pShared = pName;
            }
        }
        else if( t->GetType() != svIndex )
        {
            t->CalcAbsIfRel( aOldPos );
            BOOL bMod;
            {   // own scope for SingleDoubleRefModifier dtor if SingleRef
                SingleDoubleRefModifier aMod( *t );
                ComplRefData& rRef = aMod.Ref();
                bMod = (ScRefUpdate::UpdateTranspose( pDocument, rSource,
                    rDest, rRef ) != UR_NOTHING || bPosChanged);
            }
            if ( bMod )
            {
                t->CalcRelFromAbs( aPos );
                bChanged = TRUE;
            }
        }
    }

    if (pShared)            // Shared Formula gegen echte Formel austauschen
    {
        pDocument->RemoveFromFormulaTree( this );   // update formula count
        delete pCode;
        pCode = new ScTokenArray( *pShared->GetCode() );
        bChanged = TRUE;
        pCode->Reset();
        for( t = pCode->GetNextReference(); t; t = pCode->GetNextReference() )
        {
            if( t->GetType() != svIndex )
            {
                t->CalcAbsIfRel( aOldPos );
                BOOL bMod;
                {   // own scope for SingleDoubleRefModifier dtor if SingleRef
                    SingleDoubleRefModifier aMod( *t );
                    ComplRefData& rRef = aMod.Ref();
                    bMod = (ScRefUpdate::UpdateTranspose( pDocument, rSource,
                        rDest, rRef ) != UR_NOTHING || bPosChanged);
                }
                if ( bMod )
                    t->CalcRelFromAbs( aPos );
            }
        }
    }

    if (bChanged)
    {
        if (pUndoDoc)
        {
            ScFormulaCell* pFCell = new ScFormulaCell( pUndoDoc, aPos, pOld, cMatrixFlag );
            pFCell->nErgValue = MINDOUBLE;      // damit spaeter changed (Cut/Paste!)
            pUndoDoc->PutCell( aPos.Col(), aPos.Row(), aPos.Tab(), pFCell );
        }

        bCompile = TRUE;
        CompileTokenArray();                // ruft auch StartListeningTo
        SetDirty();
    }
    else
        StartListeningTo( pDocument );      // Listener wie vorher

    delete pOld;
}

void ScFormulaCell::UpdateGrow( const ScRange& rArea, SCCOL nGrowX, SCROW nGrowY )
{
    EndListeningTo( pDocument );

    BOOL bChanged = FALSE;
    ScToken* t;
    ScRangeData* pShared = NULL;

    pCode->Reset();
    for( t = pCode->GetNextReferenceOrName(); t; t = pCode->GetNextReferenceOrName() )
    {
        if( t->GetOpCode() == ocName )
        {
            ScRangeData* pName = pDocument->GetRangeName()->FindIndex( t->GetIndex() );
            if (pName)
            {
                if (pName->IsModified())
                    bChanged = TRUE;
                if (pName->HasType(RT_SHAREDMOD))
                    pShared = pName;
            }
        }
        else if( t->GetType() != svIndex )
        {
            t->CalcAbsIfRel( aPos );
            BOOL bMod;
            {   // own scope for SingleDoubleRefModifier dtor if SingleRef
                SingleDoubleRefModifier aMod( *t );
                ComplRefData& rRef = aMod.Ref();
                bMod = (ScRefUpdate::UpdateGrow( rArea,nGrowX,nGrowY,
                    rRef ) != UR_NOTHING);
            }
            if ( bMod )
            {
                t->CalcRelFromAbs( aPos );
                bChanged = TRUE;
            }
        }
    }

    if (pShared)            // Shared Formula gegen echte Formel austauschen
    {
        pDocument->RemoveFromFormulaTree( this );   // update formula count
        delete pCode;
        pCode = new ScTokenArray( *pShared->GetCode() );
        bChanged = TRUE;
        pCode->Reset();
        for( t = pCode->GetNextReference(); t; t = pCode->GetNextReference() )
        {
            if( t->GetType() != svIndex )
            {
                t->CalcAbsIfRel( aPos );
                BOOL bMod;
                {   // own scope for SingleDoubleRefModifier dtor if SingleRef
                    SingleDoubleRefModifier aMod( *t );
                    ComplRefData& rRef = aMod.Ref();
                    bMod = (ScRefUpdate::UpdateGrow( rArea,nGrowX,nGrowY,
                        rRef ) != UR_NOTHING);
                }
                if ( bMod )
                    t->CalcRelFromAbs( aPos );
            }
        }
    }

    if (bChanged)
    {
        bCompile = TRUE;
        CompileTokenArray();                // ruft auch StartListeningTo
        SetDirty();
    }
    else
        StartListeningTo( pDocument );      // Listener wie vorher
}

BOOL lcl_IsRangeNameInUse(USHORT nIndex, ScTokenArray* pCode, ScRangeName* pNames)
{
    for (ScToken* p = pCode->First(); p; p = pCode->Next())
    {
        if (p->GetOpCode() == ocName)
        {
            if (p->GetIndex() == nIndex)
                return TRUE;
            else
            {
                //  RangeData kann Null sein in bestimmten Excel-Dateien (#31168#)
                ScRangeData* pSubName = pNames->FindIndex(p->GetIndex());
                if (pSubName && lcl_IsRangeNameInUse(nIndex,
                                    pSubName->GetCode(), pNames))
                    return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL ScFormulaCell::IsRangeNameInUse(USHORT nIndex) const
{
    return lcl_IsRangeNameInUse( nIndex, pCode, pDocument->GetRangeName() );
}

void ScFormulaCell::ReplaceRangeNamesInUse( const ScIndexMap& rMap )
{
    for( ScToken* p = pCode->First(); p; p = pCode->Next() )
    {
        if( p->GetOpCode() == ocName )
        {
            USHORT nIndex = p->GetIndex();
            USHORT nNewIndex = rMap.Find( nIndex );
            if ( nIndex != nNewIndex )
            {
                p->SetIndex( nNewIndex );
                bCompile = TRUE;
            }
        }
    }
    if( bCompile )
        CompileTokenArray();
}

void ScFormulaCell::CompileDBFormula()
{
    for( ScToken* p = pCode->First(); p; p = pCode->Next() )
    {
        if ( p->GetOpCode() == ocDBArea
            || (p->GetOpCode() == ocName && p->GetIndex() >= SC_START_INDEX_DB_COLL) )
        {
            bCompile = TRUE;
            CompileTokenArray();
            SetDirty();
            break;
        }
    }
}

void ScFormulaCell::CompileDBFormula( BOOL bCreateFormulaString )
{
    // zwei Phasen, muessen (!) nacheinander aufgerufen werden:
    // 1. FormelString mit alten Namen erzeugen
    // 2. FormelString mit neuen Namen kompilieren
    if ( bCreateFormulaString )
    {
        BOOL bRecompile = FALSE;
        pCode->Reset();
        for ( ScToken* p = pCode->First(); p && !bRecompile; p = pCode->Next() )
        {
            switch ( p->GetOpCode() )
            {
                case ocBad:             // DB-Bereich evtl. zugefuegt
                case ocColRowName:      // #36762# falls Namensgleichheit
                case ocDBArea:          // DB-Bereich
                    bRecompile = TRUE;
                break;
                case ocName:
                    if ( p->GetIndex() >= SC_START_INDEX_DB_COLL )
                        bRecompile = TRUE;  // DB-Bereich
                break;
                default:
                    ; // nothing
            }
        }
        if ( bRecompile )
        {
            String aFormula;
            GetFormula( aFormula, ScAddress::CONV_OOO);
            if ( GetMatrixFlag() != MM_NONE && aFormula.Len() )
            {
                if ( aFormula.GetChar( aFormula.Len()-1 ) == '}' )
                    aFormula.Erase( aFormula.Len()-1 , 1 );
                if ( aFormula.GetChar(0) == '{' )
                    aFormula.Erase( 0, 1 );
            }
            EndListeningTo( pDocument );
            pDocument->RemoveFromFormulaTree( this );
            pCode->Clear();
            aErgString = aFormula;
            nErgConv = ScAddress::CONV_OOO;
        }
    }
    else if ( !pCode->GetLen() && aErgString.Len() )
    {
        Compile( aErgString );
        aErgString.Erase();
        SetDirty();
    }
}

void ScFormulaCell::CompileNameFormula( BOOL bCreateFormulaString )
{
    // zwei Phasen, muessen (!) nacheinander aufgerufen werden:
    // 1. FormelString mit alten RangeNames erzeugen
    // 2. FormelString mit neuen RangeNames kompilieren
    if ( bCreateFormulaString )
    {
        BOOL bRecompile = FALSE;
        pCode->Reset();
        for ( ScToken* p = pCode->First(); p && !bRecompile; p = pCode->Next() )
        {
            switch ( p->GetOpCode() )
            {
                case ocBad:             // RangeName evtl. zugefuegt
                case ocColRowName:      // #36762# falls Namensgleichheit
                    bRecompile = TRUE;
                break;
                default:
                    if ( p->GetType() == svIndex )
                        bRecompile = TRUE;  // RangeName
            }
        }
        if ( bRecompile )
        {
            String aFormula;
            GetFormula( aFormula, ScAddress::CONV_OOO);
            if ( GetMatrixFlag() != MM_NONE && aFormula.Len() )
            {
                if ( aFormula.GetChar( aFormula.Len()-1 ) == '}' )
                    aFormula.Erase( aFormula.Len()-1 , 1 );
                if ( aFormula.GetChar(0) == '{' )
                    aFormula.Erase( 0, 1 );
            }
            EndListeningTo( pDocument );
            pDocument->RemoveFromFormulaTree( this );
            pCode->Clear();
            aErgString = aFormula;
            nErgConv = ScAddress::CONV_OOO;
        }
    }
    else if ( !pCode->GetLen() && aErgString.Len() )
    {
        Compile( aErgString );
        aErgString.Erase();
        SetDirty();
    }
}

void ScFormulaCell::CompileColRowNameFormula()
{
    pCode->Reset();
    for ( ScToken* p = pCode->First(); p; p = pCode->Next() )
    {
        if ( p->GetOpCode() == ocColRowName )
        {
            bCompile = TRUE;
            CompileTokenArray();
            SetDirty();
            break;
        }
    }
}

ScValueCell::ScValueCell( SvStream& rStream, USHORT nVer ) :
    ScBaseCell( CELLTYPE_VALUE )
{
    if( nVer >= SC_DATABYTES2 )
    {
        BYTE cData;
        rStream >> cData;
        if( cData & 0x0F )
            rStream.SeekRel( cData & 0x0F );
    }
    rStream >> aValue;
}

void ScValueCell::Save( SvStream& rStream ) const
{
    rStream << (BYTE) 0x00 << aValue;
}

ScStringCell::ScStringCell( SvStream& rStream, USHORT nVer ) :
    ScBaseCell( CELLTYPE_STRING )
{
    if( nVer >= SC_DATABYTES2 )
    {
        BYTE cData;
        rStream >> cData;
        if( cData & 0x0F )
            rStream.SeekRel( cData & 0x0F );
    }
    rStream.ReadByteString( aString, rStream.GetStreamCharSet() );
}

void ScStringCell::Save( SvStream& rStream, FontToSubsFontConverter hConv ) const
{
    rStream << (BYTE) 0x00;
    if ( !hConv )
        rStream.WriteByteString( aString, rStream.GetStreamCharSet() );
    else
    {
        String aTmp( aString );
        sal_Unicode* p = aTmp.GetBufferAccess();
        sal_Unicode const * const pStop = p + aTmp.Len();
        for ( ; p < pStop; ++p )
        {
            *p = ConvertFontToSubsFontChar( hConv, *p );
        }
        aTmp.ReleaseBufferAccess();
        rStream.WriteByteString( aTmp, rStream.GetStreamCharSet() );
    }
}

void ScStringCell::ConvertFont( FontToSubsFontConverter hConv )
{
    if ( hConv )
    {
        sal_Unicode* p = aString.GetBufferAccess();
        sal_Unicode const * const pStop = p + aString.Len();
        for ( ; p < pStop; ++p )
        {
            *p = ConvertFontToSubsFontChar( hConv, *p );
        }
        aString.ReleaseBufferAccess();
    }
}

ScNoteCell::ScNoteCell( SvStream& rStream, USHORT nVer ) :
    ScBaseCell( CELLTYPE_NOTE )
{
    if( nVer >= SC_DATABYTES2 )
    {
        BYTE cData;
        rStream >> cData;
        if( cData & 0x0F )
            rStream.SeekRel( cData & 0x0F );
    }
}

void ScNoteCell::Save( SvStream& rStream ) const
{
    rStream << (BYTE) 0x00;
}
