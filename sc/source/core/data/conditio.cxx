/*************************************************************************
 *
 *  $RCSfile: conditio.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: obo $ $Date: 2004-06-04 10:21:07 $
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
#include "core_pch.hxx"
#endif

#pragma hdrstop

//------------------------------------------------------------------

#include "scitems.hxx"
#include <sfx2/objsh.hxx>
#include <svtools/itemset.hxx>
#include <svtools/zforlist.hxx>
#include <rtl/math.hxx>

#ifndef _UNOTOOLS_COLLATORWRAPPER_HXX
#include <unotools/collatorwrapper.hxx>
#endif

#include "conditio.hxx"
#include "cell.hxx"
#include "document.hxx"
#include "hints.hxx"
#include "compiler.hxx"
#include "rechead.hxx"
#include "rangelst.hxx"
#include "stlpool.hxx"
#include "rangenam.hxx"

//------------------------------------------------------------------------

SV_IMPL_OP_PTRARR_SORT( ScConditionalFormats_Impl, ScConditionalFormatPtr );

//------------------------------------------------------------------------

BOOL lcl_HasRelRef( ScDocument* pDoc, ScTokenArray* pFormula, USHORT nRecursion = 0 )
{
    if (pFormula)
    {
        pFormula->Reset();
        ScToken* t;
        for( t = pFormula->GetNextReferenceOrName(); t; t = pFormula->GetNextReferenceOrName() )
        {
            switch( t->GetType() )
            {
                case svDoubleRef:
                {
                    SingleRefData& rRef2 = t->GetDoubleRef().Ref2;
                    if ( rRef2.IsColRel() || rRef2.IsRowRel() || rRef2.IsTabRel() )
                        return TRUE;
                }
                // fall through

                case svSingleRef:
                {
                    SingleRefData& rRef1 = t->GetSingleRef();
                    if ( rRef1.IsColRel() || rRef1.IsRowRel() || rRef1.IsTabRel() )
                        return TRUE;
                }
                break;

                case svIndex:
                {
                    if( t->GetOpCode() == ocName )      // DB areas always absolute
                        if( ScRangeData* pRangeData = pDoc->GetRangeName()->FindIndex( t->GetIndex() ) )
                            if( (nRecursion < 42) && lcl_HasRelRef( pDoc, pRangeData->GetCode(), nRecursion + 1 ) )
                                return TRUE;
                }
                break;
            }
        }
    }
    return FALSE;
}

ScConditionEntry::ScConditionEntry( const ScConditionEntry& r ) :
    eOp(r.eOp),
    nOptions(r.nOptions),
    nVal1(r.nVal1),
    nVal2(r.nVal2),
    aStrVal1(r.aStrVal1),
    aStrVal2(r.aStrVal2),
    bIsStr1(r.bIsStr1),
    bIsStr2(r.bIsStr2),
    bRelRef1(r.bRelRef1),
    bRelRef2(r.bRelRef2),
    pFormula1(NULL),
    pFormula2(NULL),
    pFCell1(NULL),
    pFCell2(NULL),
    pDoc(r.pDoc),
    aSrcPos(r.aSrcPos),
    bFirstRun(TRUE)
{
    //  ScTokenArray copy ctor erzeugt flache Kopie

    if (r.pFormula1)
        pFormula1 = new ScTokenArray( *r.pFormula1 );
    if (r.pFormula2)
        pFormula2 = new ScTokenArray( *r.pFormula2 );

    //  Formelzellen werden erst bei IsValid angelegt
}

ScConditionEntry::ScConditionEntry( ScDocument* pDocument, const ScConditionEntry& r ) :
    eOp(r.eOp),
    nOptions(r.nOptions),
    nVal1(r.nVal1),
    nVal2(r.nVal2),
    aStrVal1(r.aStrVal1),
    aStrVal2(r.aStrVal2),
    bIsStr1(r.bIsStr1),
    bIsStr2(r.bIsStr2),
    bRelRef1(r.bRelRef1),
    bRelRef2(r.bRelRef2),
    pFormula1(NULL),
    pFormula2(NULL),
    pFCell1(NULL),
    pFCell2(NULL),
    pDoc(pDocument),
    aSrcPos(r.aSrcPos),
    bFirstRun(TRUE)
{
    // echte Kopie der Formeln (fuer Ref-Undo)

    if (r.pFormula1)
        pFormula1 = r.pFormula1->Clone();
    if (r.pFormula2)
        pFormula2 = r.pFormula2->Clone();

    //  Formelzellen werden erst bei IsValid angelegt
    //! im Clipboard nicht - dann vorher interpretieren !!!
}

ScConditionEntry::ScConditionEntry( ScConditionMode eOper,
                                const String& rExpr1, const String& rExpr2,
                                ScDocument* pDocument, const ScAddress& rPos,
                                BOOL bCompileEnglish, BOOL bCompileXML ) :
    eOp(eOper),
    nOptions(0),    // spaeter...
    nVal1(0.0),
    nVal2(0.0),
    bIsStr1(FALSE),
    bIsStr2(FALSE),
    bRelRef1(FALSE),
    bRelRef2(FALSE),
    pFormula1(NULL),
    pFormula2(NULL),
    pFCell1(NULL),
    pFCell2(NULL),
    pDoc(pDocument),
    aSrcPos(rPos),
    bFirstRun(TRUE)
{
    Compile( rExpr1, rExpr2, bCompileEnglish, bCompileXML, FALSE );

    //  Formelzellen werden erst bei IsValid angelegt
}

ScConditionEntry::ScConditionEntry( ScConditionMode eOper,
                                const ScTokenArray* pArr1, const ScTokenArray* pArr2,
                                ScDocument* pDocument, const ScAddress& rPos ) :
    eOp(eOper),
    nOptions(0),    // spaeter...
    nVal1(0.0),
    nVal2(0.0),
    bIsStr1(FALSE),
    bIsStr2(FALSE),
    bRelRef1(FALSE),
    bRelRef2(FALSE),
    pFormula1(NULL),
    pFormula2(NULL),
    pFCell1(NULL),
    pFCell2(NULL),
    pDoc(pDocument),
    aSrcPos(rPos),
    bFirstRun(TRUE)
{
    if ( pArr1 )
    {
        pFormula1 = new ScTokenArray( *pArr1 );
        if ( pFormula1->GetLen() == 1 )
        {
            // einzelne (konstante Zahl) ?
            ScToken* pToken = pFormula1->First();
            if ( pToken->GetOpCode() == ocPush )
            {
                if ( pToken->GetType() == svDouble )
                {
                    nVal1 = pToken->GetDouble();
                    DELETEZ(pFormula1);             // nicht als Formel merken
                }
                else if ( pToken->GetType() == svString )
                {
                    bIsStr1 = TRUE;
                    aStrVal1 = pToken->GetString();
                    DELETEZ(pFormula1);             // nicht als Formel merken
                }
            }
        }
        bRelRef1 = lcl_HasRelRef( pDoc, pFormula1 );
    }
    if ( pArr2 )
    {
        pFormula2 = new ScTokenArray( *pArr2 );
        if ( pFormula2->GetLen() == 1 )
        {
            // einzelne (konstante Zahl) ?
            ScToken* pToken = pFormula2->First();
            if ( pToken->GetOpCode() == ocPush )
            {
                if ( pToken->GetType() == svDouble )
                {
                    nVal2 = pToken->GetDouble();
                    DELETEZ(pFormula2);             // nicht als Formel merken
                }
                else if ( pToken->GetType() == svString )
                {
                    bIsStr2 = TRUE;
                    aStrVal2 = pToken->GetString();
                    DELETEZ(pFormula2);             // nicht als Formel merken
                }
            }
        }
        bRelRef2 = lcl_HasRelRef( pDoc, pFormula2 );
    }

    //  formula cells are created at IsValid
}

ScConditionEntry::~ScConditionEntry()
{
    delete pFCell1;
    delete pFCell2;

    delete pFormula1;
    delete pFormula2;
}

ScConditionEntry::ScConditionEntry( SvStream& rStream, ScMultipleReadHeader& rHdr,
                                    ScDocument* pDocument ) :
    nVal1(0.0),
    nVal2(0.0),
    bIsStr1(FALSE),
    bIsStr2(FALSE),
    bRelRef1(FALSE),
    bRelRef2(FALSE),
    pFormula1(NULL),
    pFormula2(NULL),
    pFCell1(NULL),
    pFCell2(NULL),
    pDoc(pDocument),
    bFirstRun(TRUE)
{
#if SC_ROWLIMIT_STREAM_ACCESS
#error address types changed!
    USHORT nVer = (USHORT) pDoc->GetSrcVersion();

    rHdr.StartEntry();

    BYTE nOpByte;
    rStream >> nOpByte;
    eOp = (ScConditionMode) nOpByte;

    rStream >> nOptions;

    ScAddress aPos;
    BYTE nTypeByte;
    rStream >> nTypeByte;
    ScConditionValType eType = (ScConditionValType) nTypeByte;
    if ( eType == SC_VAL_FORMULA )
    {
        rStream >> aPos;
        pFormula1 = new ScTokenArray;
        pFormula1->Load( rStream, nVer, aPos );
        bRelRef1 = lcl_HasRelRef( pDoc, pFormula1 );
    }
    else if ( eType == SC_VAL_VALUE )
        rStream >> nVal1;
    else
    {
        bIsStr1 = TRUE;
        rStream.ReadByteString( aStrVal1, rStream.GetStreamCharSet() );
    }

    if ( eOp == SC_COND_BETWEEN || eOp == SC_COND_NOTBETWEEN )
    {
        rStream >> nTypeByte;
        eType = (ScConditionValType) nTypeByte;
        if ( eType == SC_VAL_FORMULA )
        {
            rStream >> aPos;
            pFormula2 = new ScTokenArray;
            pFormula2->Load( rStream, nVer, aPos );
            bRelRef2 = lcl_HasRelRef( pDoc, pFormula2 );
        }
        else if ( eType == SC_VAL_VALUE )
            rStream >> nVal2;
        else
        {
            bIsStr2 = TRUE;
            rStream.ReadByteString( aStrVal2, rStream.GetStreamCharSet() );
        }
    }

    rHdr.EndEntry();

    aSrcPos = aPos;

    //  Formelzellen werden erst bei IsValid angelegt
#endif // SC_ROWLIMIT_STREAM_ACCESS
}

void ScConditionEntry::StoreCondition(SvStream& rStream, ScMultipleWriteHeader& rHdr) const
{
#if SC_ROWLIMIT_STREAM_ACCESS
#error address types changed!
    rHdr.StartEntry();

    //  1) Byte fuer die Operation
    //  2) USHORT fuer Optionen
    //  3) Byte, ob Wert, String oder Formel folgt
    //  4) double, String oder TokenArray
    //  5) je nach Operation 3 und 4 nochmal
    //  vor jedem TokenArray noch die Position als ScAddress

    rStream << (BYTE) eOp;
    rStream << nOptions;

    ScConditionValType eType =
            pFormula1 ? SC_VAL_FORMULA : ( bIsStr1 ? SC_VAL_STRING : SC_VAL_VALUE );
    rStream << (BYTE) eType;
    if ( eType == SC_VAL_FORMULA )
    {
        rStream << aSrcPos;
        pFormula1->Store( rStream, aSrcPos );
    }
    else if ( eType == SC_VAL_VALUE )
        rStream << nVal1;
    else
        rStream.WriteByteString( aStrVal1, rStream.GetStreamCharSet() );

    if ( eOp == SC_COND_BETWEEN || eOp == SC_COND_NOTBETWEEN )
    {
        eType = pFormula2 ? SC_VAL_FORMULA : ( bIsStr2 ? SC_VAL_STRING : SC_VAL_VALUE );
        rStream << (BYTE) eType;
        if ( eType == SC_VAL_FORMULA )
        {
            rStream << aSrcPos;
            pFormula2->Store( rStream, aSrcPos );
        }
        else if ( eType == SC_VAL_VALUE )
            rStream << nVal2;
        else
            rStream.WriteByteString( aStrVal2, rStream.GetStreamCharSet() );
    }

    rHdr.EndEntry();
#endif // SC_ROWLIMIT_STREAM_ACCESS
}

void ScConditionEntry::Compile( const String& rExpr1, const String& rExpr2,
                                BOOL bEnglish, BOOL bCompileXML, BOOL bTextToReal )
{
    if ( rExpr1.Len() || rExpr2.Len() )
    {
        ScCompiler aComp( pDoc, aSrcPos );
        aComp.SetCompileEnglish( bEnglish );
        aComp.SetCompileXML( bCompileXML );

        if ( rExpr1.Len() )
        {
            if ( pDoc->IsImportingXML() && !bTextToReal )
            {
                //  temporary formula string as string tokens
                //! merge with lcl_ScDocFunc_CreateTokenArrayXML
                pFormula1 = new ScTokenArray;
                pFormula1->AddString( rExpr1 );
                // bRelRef1 is set when the formula is compiled again (CompileXML)
            }
            else
            {
                pFormula1 = aComp.CompileString( rExpr1 );
                if ( pFormula1->GetLen() == 1 )
                {
                    // einzelne (konstante Zahl) ?
                    ScToken* pToken = pFormula1->First();
                    if ( pToken->GetOpCode() == ocPush )
                    {
                        if ( pToken->GetType() == svDouble )
                        {
                            nVal1 = pToken->GetDouble();
                            DELETEZ(pFormula1);             // nicht als Formel merken
                        }
                        else if ( pToken->GetType() == svString )
                        {
                            bIsStr1 = TRUE;
                            aStrVal1 = pToken->GetString();
                            DELETEZ(pFormula1);             // nicht als Formel merken
                        }
                    }
                }
                bRelRef1 = lcl_HasRelRef( pDoc, pFormula1 );
            }
        }

        if ( rExpr2.Len() )
        {
            if ( pDoc->IsImportingXML() && !bTextToReal )
            {
                //  temporary formula string as string tokens
                //! merge with lcl_ScDocFunc_CreateTokenArrayXML
                pFormula2 = new ScTokenArray;
                pFormula2->AddString( rExpr2 );
                // bRelRef2 is set when the formula is compiled again (CompileXML)
            }
            else
            {
                pFormula2 = aComp.CompileString( rExpr2 );
                if ( pFormula2->GetLen() == 1 )
                {
                    // einzelne (konstante Zahl) ?
                    ScToken* pToken = pFormula2->First();
                    if ( pToken->GetOpCode() == ocPush )
                    {
                        if ( pToken->GetType() == svDouble )
                        {
                            nVal2 = pToken->GetDouble();
                            DELETEZ(pFormula2);             // nicht als Formel merken
                        }
                        else if ( pToken->GetType() == svString )
                        {
                            bIsStr2 = TRUE;
                            aStrVal2 = pToken->GetString();
                            DELETEZ(pFormula2);             // nicht als Formel merken
                        }
                    }
                }
                bRelRef2 = lcl_HasRelRef( pDoc, pFormula2 );
            }
        }
    }
}

void ScConditionEntry::MakeCells( const ScAddress& rPos )           // Formelzellen anlegen
{
    if ( !pDoc->IsClipOrUndo() )            // nie im Clipboard rechnen!
    {
        if ( pFormula1 && !pFCell1 && !bRelRef1 )
        {
            pFCell1 = new ScFormulaCell( pDoc, rPos, pFormula1 );
            pFCell1->StartListeningTo( pDoc );
        }

        if ( pFormula2 && !pFCell2 && !bRelRef2 )
        {
            pFCell2 = new ScFormulaCell( pDoc, rPos, pFormula2 );
            pFCell2->StartListeningTo( pDoc );
        }
    }
}

void ScConditionEntry::SetIgnoreBlank(BOOL bSet)
{
    //  Das Bit SC_COND_NOBLANKS wird gesetzt, wenn Blanks nicht ignoriert werden
    //  (nur bei Gueltigkeit)

    if (bSet)
        nOptions &= ~SC_COND_NOBLANKS;
    else
        nOptions |= SC_COND_NOBLANKS;
}

void ScConditionEntry::CompileAll()
{
    //  Formelzellen loeschen, dann wird beim naechsten IsValid neu kompiliert

    DELETEZ(pFCell1);
    DELETEZ(pFCell2);
}

void ScConditionEntry::CompileXML()
{
    //  Convert the text tokens that were created during XML import into real tokens:
    //  The stored string tokens contain english function names, but no XML-style references

    Compile( GetExpression(aSrcPos, 0, 0, TRUE, FALSE, TRUE),
             GetExpression(aSrcPos, 1, 0, TRUE, FALSE, TRUE),
             TRUE, FALSE, TRUE );
}

void lcl_CondUpdateInsertTab( ScTokenArray& rCode, SCTAB nInsTab, SCTAB nPosTab, BOOL& rChanged )
{
    //  Insert table: only update absolute table references.
    //  (Similar to ScCompiler::UpdateInsertTab with bIsName=TRUE, result is the same as for named ranges)
    //  For deleting, ScCompiler::UpdateDeleteTab is used because of the handling of invalid references.

    rCode.Reset();
    ScToken* p = rCode.GetNextReference();
    while( p )
    {
        SingleRefData& rRef1 = p->GetSingleRef();
        if ( !rRef1.IsTabRel() && nInsTab <= rRef1.nTab )
        {
            rRef1.nTab += 1;
            rRef1.nRelTab = rRef1.nTab - nPosTab;
            rChanged = TRUE;
        }
        if( p->GetType() == svDoubleRef )
        {
            SingleRefData& rRef2 = p->GetDoubleRef().Ref2;
            if ( !rRef2.IsTabRel() && nInsTab <= rRef2.nTab )
            {
                rRef2.nTab += 1;
                rRef2.nRelTab = rRef2.nTab - nPosTab;
                rChanged = TRUE;
            }
        }
        p = rCode.GetNextReference();
    }
}

void ScConditionEntry::UpdateReference( UpdateRefMode eUpdateRefMode,
                                const ScRange& rRange, SCsCOL nDx, SCsROW nDy, SCsTAB nDz )
{
    BOOL bInsertTab = ( eUpdateRefMode == URM_INSDEL && nDz == 1 );
    BOOL bDeleteTab = ( eUpdateRefMode == URM_INSDEL && nDz == -1 );

    BOOL bChanged1 = FALSE;
    BOOL bChanged2 = FALSE;

    if (pFormula1)
    {
        if ( bInsertTab )
            lcl_CondUpdateInsertTab( *pFormula1, rRange.aStart.Tab(), aSrcPos.Tab(), bChanged1 );
        else
        {
            ScCompiler aComp( pDoc, aSrcPos, *pFormula1 );
            if ( bDeleteTab )
                aComp.UpdateDeleteTab( rRange.aStart.Tab(), FALSE, TRUE, bChanged1 );
            else
                aComp.UpdateNameReference( eUpdateRefMode, rRange, nDx, nDy, nDz, bChanged1 );
        }

        if (bChanged1)
            DELETEZ(pFCell1);       // is created again in IsValid
    }
    if (pFormula2)
    {
        if ( bInsertTab )
            lcl_CondUpdateInsertTab( *pFormula2, rRange.aStart.Tab(), aSrcPos.Tab(), bChanged2 );
        else
        {
            ScCompiler aComp( pDoc, aSrcPos, *pFormula2 );
            if ( bDeleteTab )
                aComp.UpdateDeleteTab( rRange.aStart.Tab(), FALSE, TRUE, bChanged2 );
            else
                aComp.UpdateNameReference( eUpdateRefMode, rRange, nDx, nDy, nDz, bChanged2 );
        }

        if (bChanged2)
            DELETEZ(pFCell2);       // is created again in IsValid
    }
}

void ScConditionEntry::UpdateMoveTab( SCTAB nOldPos, SCTAB nNewPos )
{
    if (pFormula1)
    {
        ScCompiler aComp( pDoc, aSrcPos, *pFormula1 );
        aComp.UpdateMoveTab(nOldPos, nNewPos, TRUE );
        DELETEZ(pFCell1);
    }
    if (pFormula2)
    {
        ScCompiler aComp( pDoc, aSrcPos, *pFormula2 );
        aComp.UpdateMoveTab(nOldPos, nNewPos, TRUE );
        DELETEZ(pFCell2);
    }
}

//! als Vergleichsoperator ans TokenArray ???

BOOL lcl_IsEqual( const ScTokenArray* pArr1, const ScTokenArray* pArr2 )
{
    //  verglichen wird nur das nicht-UPN Array

    if ( pArr1 && pArr2 )
    {
        USHORT nLen = pArr1->GetLen();
        if ( pArr2->GetLen() != nLen )
            return FALSE;

        ScToken** ppToken1 = pArr1->GetArray();
        ScToken** ppToken2 = pArr2->GetArray();
        for (USHORT i=0; i<nLen; i++)
        {
            if ( ppToken1[i] != ppToken2[i] &&
                 !(*ppToken1[i] == *ppToken2[i]) )
                return FALSE;                       // Unterschied
        }
        return TRUE;                    // alle Eintraege gleich
    }
    else
        return !pArr1 && !pArr2;        // beide 0 -> gleich
}

int ScConditionEntry::operator== ( const ScConditionEntry& r ) const
{
    BOOL bEq = (eOp == r.eOp && nOptions == r.nOptions &&
                lcl_IsEqual( pFormula1, r.pFormula1 ) &&
                lcl_IsEqual( pFormula2, r.pFormula2 ));
    if (bEq)
    {
        //  bei Formeln auch die Referenz-Tabelle vergleichen
        if ( ( pFormula1 || pFormula2 ) && aSrcPos != r.aSrcPos )
            bEq = FALSE;

        //  wenn keine Formeln, Werte vergleichen
        if ( !pFormula1 && ( nVal1 != r.nVal1 || aStrVal1 != r.aStrVal1 || bIsStr1 != r.bIsStr1 ) )
            bEq = FALSE;
        if ( !pFormula2 && ( nVal2 != r.nVal2 || aStrVal2 != r.aStrVal2 || bIsStr2 != r.bIsStr2 ) )
            bEq = FALSE;
    }

    return bEq;
}

void ScConditionEntry::Interpret( const ScAddress& rPos )
{
    //  Formelzellen anlegen
    //  dabei koennen neue Broadcaster (Note-Zellen) ins Dokument eingefuegt werden !!!!

    if ( ( pFormula1 && !pFCell1 ) || ( pFormula2 && !pFCell2 ) )
        MakeCells( rPos );

    //  Formeln auswerten

    BOOL bDirty = FALSE;        //! 1 und 2 getrennt ???

    ScFormulaCell* pTemp1 = NULL;
    ScFormulaCell* pEff1 = pFCell1;
    if ( bRelRef1 )
    {
        pTemp1 = new ScFormulaCell( pDoc, rPos, pFormula1 );    // ohne Listening
        pEff1 = pTemp1;
    }
    if ( pEff1 )
    {
        if (!pEff1->IsRunning())        // keine 522 erzeugen
        {
            //! Changed statt Dirty abfragen !!!
            if (pEff1->GetDirty() && !bRelRef1)
                bDirty = TRUE;
            if (pEff1->IsValue())
            {
                bIsStr1 = FALSE;
                nVal1 = pEff1->GetValue();
                aStrVal1.Erase();
            }
            else
            {
                bIsStr1 = TRUE;
                pEff1->GetString( aStrVal1 );
                nVal1 = 0.0;
            }
        }
    }
    delete pTemp1;

    ScFormulaCell* pTemp2 = NULL;
    ScFormulaCell* pEff2 = pFCell2; //@ 1!=2
    if ( bRelRef2 )
    {
        pTemp2 = new ScFormulaCell( pDoc, rPos, pFormula2 );    // ohne Listening
        pEff2 = pTemp2;
    }
    if ( pEff2 )
    {
        if (!pEff2->IsRunning())        // keine 522 erzeugen
        {
            if (pEff2->GetDirty() && !bRelRef2)
                bDirty = TRUE;
            if (pEff2->IsValue())
            {
                bIsStr2 = FALSE;
                nVal2 = pEff2->GetValue();
                aStrVal2.Erase();
            }
            else
            {
                bIsStr2 = TRUE;
                pEff2->GetString( aStrVal2 );
                nVal2 = 0.0;
            }
        }
    }
    delete pTemp2;

    //  wenn IsRunning, bleiben die letzten Werte erhalten

    if (bDirty && !bFirstRun)
    {
        //  bei bedingten Formaten neu painten

        DataChanged( NULL );    // alles
    }

    bFirstRun = FALSE;
}

BOOL ScConditionEntry::IsValid( double nArg ) const
{
    //  Interpret muss schon gerufen sein

    if ( bIsStr1 )
    {
        // wenn auf String getestet wird, bei Zahlen immer FALSE, ausser bei "ungleich"

        return ( eOp == SC_COND_NOTEQUAL );
    }

    if ( eOp == SC_COND_BETWEEN || eOp == SC_COND_NOTBETWEEN )
        if ( bIsStr2 )
            return FALSE;

    double nComp1 = nVal1;      // Kopie, damit vertauscht werden kann
    double nComp2 = nVal2;

    if ( eOp == SC_COND_BETWEEN || eOp == SC_COND_NOTBETWEEN )
        if ( nComp1 > nComp2 )
        {
            //  richtige Reihenfolge fuer Wertebereich
            double nTemp = nComp1; nComp1 = nComp2; nComp2 = nTemp;
        }

    //  Alle Grenzfaelle muessen per ::rtl::math::approxEqual getestet werden!

    BOOL bValid = FALSE;
    switch (eOp)
    {
        case SC_COND_NONE:
            break;                  // immer FALSE;
        case SC_COND_EQUAL:
            bValid = ::rtl::math::approxEqual( nArg, nComp1 );
            break;
        case SC_COND_NOTEQUAL:
            bValid = !::rtl::math::approxEqual( nArg, nComp1 );
            break;
        case SC_COND_GREATER:
            bValid = ( nArg > nComp1 ) && !::rtl::math::approxEqual( nArg, nComp1 );
            break;
        case SC_COND_EQGREATER:
            bValid = ( nArg >= nComp1 ) || ::rtl::math::approxEqual( nArg, nComp1 );
            break;
        case SC_COND_LESS:
            bValid = ( nArg < nComp1 ) && !::rtl::math::approxEqual( nArg, nComp1 );
            break;
        case SC_COND_EQLESS:
            bValid = ( nArg <= nComp1 ) || ::rtl::math::approxEqual( nArg, nComp1 );
            break;
        case SC_COND_BETWEEN:
            bValid = ( nArg >= nComp1 && nArg <= nComp2 ) ||
                     ::rtl::math::approxEqual( nArg, nComp1 ) || ::rtl::math::approxEqual( nArg, nComp2 );
            break;
        case SC_COND_NOTBETWEEN:
            bValid = ( nArg < nComp1 || nArg > nComp2 ) &&
                     !::rtl::math::approxEqual( nArg, nComp1 ) && !::rtl::math::approxEqual( nArg, nComp2 );
            break;
        case SC_COND_DIRECT:
            bValid = !::rtl::math::approxEqual( nComp1, 0.0 );
            break;
        default:
            DBG_ERROR("unbekannte Operation bei ScConditionEntry");
            break;
    }
    return bValid;
}

BOOL ScConditionEntry::IsValidStr( const String& rArg ) const
{
    //  Interpret muss schon gerufen sein

    if ( eOp == SC_COND_DIRECT )                // Formel ist unabhaengig vom Inhalt
        return !::rtl::math::approxEqual( nVal1, 0.0 );

    //  Wenn Bedingung Zahl enthaelt, immer FALSE, ausser bei "ungleich"

    if ( !bIsStr1 )
        return ( eOp == SC_COND_NOTEQUAL );
    if ( eOp == SC_COND_BETWEEN || eOp == SC_COND_NOTBETWEEN )
        if ( !bIsStr2 )
            return FALSE;

    String aUpVal1( aStrVal1 );     //! als Member? (dann auch in Interpret setzen)
    String aUpVal2( aStrVal2 );

    if ( eOp == SC_COND_BETWEEN || eOp == SC_COND_NOTBETWEEN )
        if ( ScGlobal::pCollator->compareString( aUpVal1, aUpVal2 )
                == COMPARE_GREATER )
        {
            //  richtige Reihenfolge fuer Wertebereich
            String aTemp( aUpVal1 ); aUpVal1 = aUpVal2; aUpVal2 = aTemp;
        }

    BOOL bValid;
    switch ( eOp )
    {
        case SC_COND_EQUAL:
            bValid = (ScGlobal::pCollator->compareString(
                rArg, aUpVal1 ) == COMPARE_EQUAL);
        break;
        case SC_COND_NOTEQUAL:
            bValid = (ScGlobal::pCollator->compareString(
                rArg, aUpVal1 ) != COMPARE_EQUAL);
        break;
        default:
        {
            sal_Int32 nCompare = ScGlobal::pCollator->compareString(
                rArg, aUpVal1 );
            switch ( eOp )
            {
                case SC_COND_GREATER:
                    bValid = ( nCompare == COMPARE_GREATER );
                    break;
                case SC_COND_EQGREATER:
                    bValid = ( nCompare == COMPARE_EQUAL || nCompare == COMPARE_GREATER );
                    break;
                case SC_COND_LESS:
                    bValid = ( nCompare == COMPARE_LESS );
                    break;
                case SC_COND_EQLESS:
                    bValid = ( nCompare == COMPARE_EQUAL || nCompare == COMPARE_LESS );
                    break;
                case SC_COND_BETWEEN:
                case SC_COND_NOTBETWEEN:
                    //  Test auf NOTBETWEEN:
                    bValid = ( nCompare == COMPARE_LESS ||
                        ScGlobal::pCollator->compareString( rArg,
                        aUpVal2 ) == COMPARE_GREATER );
                    if ( eOp == SC_COND_BETWEEN )
                        bValid = !bValid;
                    break;
                //  SC_COND_DIRECT schon oben abgefragt
                default:
                    DBG_ERROR("unbekannte Operation bei ScConditionEntry");
                    bValid = FALSE;
                    break;
            }
        }
    }
    return bValid;
}

BOOL ScConditionEntry::IsCellValid( ScBaseCell* pCell, const ScAddress& rPos ) const
{
    ((ScConditionEntry*)this)->Interpret(rPos);         // Formeln auswerten

    double nArg = 0.0;
    String aArgStr;
    BOOL bVal = TRUE;

    if ( pCell )
    {
        CellType eType = pCell->GetCellType();
        switch (eType)
        {
            case CELLTYPE_VALUE:
                nArg = ((ScValueCell*)pCell)->GetValue();
                break;
            case CELLTYPE_FORMULA:
                {
                    ScFormulaCell* pFCell = (ScFormulaCell*)pCell;
                    bVal = pFCell->IsValue();
                    if (bVal)
                        nArg = pFCell->GetValue();
                    else
                        pFCell->GetString(aArgStr);
                }
                break;
            case CELLTYPE_STRING:
            case CELLTYPE_EDIT:
                bVal = FALSE;
                if ( eType == CELLTYPE_STRING )
                    ((ScStringCell*)pCell)->GetString(aArgStr);
                else
                    ((ScEditCell*)pCell)->GetString(aArgStr);
                break;

            default:
                pCell = NULL;           // Note-Zellen wie leere
                break;
        }
    }

    if (!pCell)
        if (bIsStr1)
            bVal = FALSE;               // leere Zellen je nach Bedingung

    if (bVal)
        return IsValid( nArg );
    else
        return IsValidStr( aArgStr );
}

String ScConditionEntry::GetExpression( const ScAddress& rCursor, USHORT nIndex,
                                        ULONG nNumFmt, BOOL bEnglish,
                                        BOOL bCompileXML, BOOL bTextToReal ) const
{
    String aRet;

    if ( bEnglish && nNumFmt == 0 )
        nNumFmt = pDoc->GetFormatTable()->GetStandardIndex( LANGUAGE_ENGLISH_US );

    if ( nIndex==0 )
    {
        if ( pFormula1 )
        {
            ScCompiler aComp(pDoc, rCursor, *pFormula1);
            aComp.SetCompileEnglish( bEnglish );
            aComp.SetCompileXML( bCompileXML );
            aComp.SetImportXML( bTextToReal );          // set only from CompileXML method
            aComp.CreateStringFromTokenArray( aRet );
        }
        else if (bIsStr1)
        {
            aRet = '"';
            aRet += aStrVal1;
            aRet += '"';
        }
        else
            pDoc->GetFormatTable()->GetInputLineString(nVal1, nNumFmt, aRet);
    }
    else if ( nIndex==1 )
    {
        if ( pFormula2 )
        {
            ScCompiler aComp(pDoc, rCursor, *pFormula2);
            aComp.SetCompileEnglish( bEnglish );
            aComp.SetCompileXML( bCompileXML );
            aComp.SetImportXML( bTextToReal );          // set only from CompileXML method
            aComp.CreateStringFromTokenArray( aRet );
        }
        else if (bIsStr2)
        {
            aRet = '"';
            aRet += aStrVal2;
            aRet += '"';
        }
        else
            pDoc->GetFormatTable()->GetInputLineString(nVal2, nNumFmt, aRet);
    }
    else
        DBG_ERROR("GetExpression: falscher Index");

    return aRet;
}

ScTokenArray* ScConditionEntry::CreateTokenArry( USHORT nIndex ) const
{
    ScTokenArray* pRet;
    ScAddress aAddr;

    if ( nIndex==0 )
    {
        if ( pFormula1 )
            pRet = new ScTokenArray( *pFormula1 );
        else
        {
            pRet = new ScTokenArray();
            if (bIsStr1)
                pRet->AddString( aStrVal1.GetBuffer() );
            else
                pRet->AddDouble( nVal1 );
        }
    }
    else if ( nIndex==1 )
    {
        if ( pFormula2 )
            pRet = new ScTokenArray( *pFormula2 );
        else
        {
            pRet = new ScTokenArray();
            if (bIsStr2)
                pRet->AddString( aStrVal2.GetBuffer() );
            else
                pRet->AddDouble( nVal2 );
        }
    }
    else
        DBG_ERROR("GetExpression: falscher Index");

    return pRet;
}

void ScConditionEntry::SourceChanged( const ScAddress& rChanged )
{
    for (USHORT nPass = 0; nPass < 2; nPass++)
    {
        ScTokenArray* pFormula = nPass ? pFormula2 : pFormula1;
        if (pFormula)
        {
            pFormula->Reset();
            ScToken* t;
            for( t = pFormula->GetNextReference(); t; t = pFormula->GetNextReference() )
            {
                SingleDoubleRefProvider aProv( *t );
                if ( aProv.Ref1.IsColRel() || aProv.Ref1.IsRowRel() || aProv.Ref1.IsTabRel() ||
                     aProv.Ref2.IsColRel() || aProv.Ref2.IsRowRel() || aProv.Ref2.IsTabRel() )
                {
                    //  absolut muss getroffen sein, relativ bestimmt Bereich

                    BOOL bHit = TRUE;
                    SCsCOL nCol1;
                    SCsROW nRow1;
                    SCsTAB nTab1;
                    SCsCOL nCol2;
                    SCsROW nRow2;
                    SCsTAB nTab2;

                    if ( aProv.Ref1.IsColRel() )
                        nCol2 = rChanged.Col() - aProv.Ref1.nRelCol;
                    else
                    {
                        bHit &= ( rChanged.Col() >= aProv.Ref1.nCol );
                        nCol2 = MAXCOL;
                    }
                    if ( aProv.Ref1.IsRowRel() )
                        nRow2 = rChanged.Row() - aProv.Ref1.nRelRow;
                    else
                    {
                        bHit &= ( rChanged.Row() >= aProv.Ref1.nRow );
                        nRow2 = MAXROW;
                    }
                    if ( aProv.Ref1.IsTabRel() )
                        nTab2 = rChanged.Tab() - aProv.Ref1.nRelTab;
                    else
                    {
                        bHit &= ( rChanged.Tab() >= aProv.Ref1.nTab );
                        nTab2 = MAXTAB;
                    }

                    if ( aProv.Ref2.IsColRel() )
                        nCol1 = rChanged.Col() - aProv.Ref2.nRelCol;
                    else
                    {
                        bHit &= ( rChanged.Col() <= aProv.Ref2.nCol );
                        nCol1 = 0;
                    }
                    if ( aProv.Ref2.IsRowRel() )
                        nRow1 = rChanged.Row() - aProv.Ref2.nRelRow;
                    else
                    {
                        bHit &= ( rChanged.Row() <= aProv.Ref2.nRow );
                        nRow1 = 0;
                    }
                    if ( aProv.Ref2.IsTabRel() )
                        nTab1 = rChanged.Tab() - aProv.Ref2.nRelTab;
                    else
                    {
                        bHit &= ( rChanged.Tab() <= aProv.Ref2.nTab );
                        nTab1 = 0;
                    }

                    if ( bHit )
                    {
                        //! begrenzen

                        ScRange aPaint( nCol1,nRow1,nTab1, nCol2,nRow2,nTab2 );

                        //  kein Paint, wenn es nur die Zelle selber ist
                        if ( aPaint.aStart != rChanged || aPaint.aEnd != rChanged )
                            DataChanged( &aPaint );
                    }
                }
            }
        }
    }
}

void ScConditionEntry::DataChanged( const ScRange* pModified ) const
{
    // nix
}

//------------------------------------------------------------------------

ScCondFormatEntry::ScCondFormatEntry( ScConditionMode eOper,
                                        const String& rExpr1, const String& rExpr2,
                                        ScDocument* pDocument, const ScAddress& rPos,
                                        const String& rStyle,
                                        BOOL bCompileEnglish, BOOL bCompileXML ) :
    ScConditionEntry( eOper, rExpr1, rExpr2, pDocument, rPos, bCompileEnglish, bCompileXML ),
    aStyleName( rStyle ),
    pParent( NULL )
{
}

ScCondFormatEntry::ScCondFormatEntry( ScConditionMode eOper,
                                        const ScTokenArray* pArr1, const ScTokenArray* pArr2,
                                        ScDocument* pDocument, const ScAddress& rPos,
                                        const String& rStyle ) :
    ScConditionEntry( eOper, pArr1, pArr2, pDocument, rPos ),
    aStyleName( rStyle ),
    pParent( NULL )
{
}

ScCondFormatEntry::ScCondFormatEntry( const ScCondFormatEntry& r ) :
    ScConditionEntry( r ),
    aStyleName( r.aStyleName ),
    pParent( NULL )
{
}

ScCondFormatEntry::ScCondFormatEntry( ScDocument* pDocument, const ScCondFormatEntry& r ) :
    ScConditionEntry( pDocument, r ),
    aStyleName( r.aStyleName ),
    pParent( NULL )
{
}

ScCondFormatEntry::ScCondFormatEntry( SvStream& rStream, ScMultipleReadHeader& rHdr,
                                        ScDocument* pDocument ) :
    ScConditionEntry( rStream, rHdr, pDocument ),
    pParent( NULL )
{
    //  im Datei-Header sind getrennte Eintraege fuer ScConditionEntry und ScCondFormatEntry

    rHdr.StartEntry();
    rStream.ReadByteString( aStyleName, rStream.GetStreamCharSet() );
    rHdr.EndEntry();
}

void ScCondFormatEntry::Store(SvStream& rStream, ScMultipleWriteHeader& rHdr) const
{
    //  im Datei-Header sind getrennte Eintraege fuer ScConditionEntry und ScCondFormatEntry

    StoreCondition( rStream, rHdr );

    rHdr.StartEntry();
    rStream.WriteByteString( aStyleName, rStream.GetStreamCharSet() );
    rHdr.EndEntry();
}


int ScCondFormatEntry::operator== ( const ScCondFormatEntry& r ) const
{
    return ScConditionEntry::operator==( r ) &&
            aStyleName == r.aStyleName;

    //  Range wird nicht verglichen
}

ScCondFormatEntry::~ScCondFormatEntry()
{
}

void ScCondFormatEntry::DataChanged( const ScRange* pModified ) const
{
    if ( pParent )
        pParent->DoRepaint( pModified );
}

//------------------------------------------------------------------------

ScConditionalFormat::ScConditionalFormat(ULONG nNewKey, ScDocument* pDocument) :
    pDoc( pDocument ),
    pAreas( NULL ),
    nKey( nNewKey ),
    ppEntries( NULL ),
    nEntryCount( 0 )
{
}

ScConditionalFormat::ScConditionalFormat(const ScConditionalFormat& r) :
    pDoc( r.pDoc ),
    pAreas( NULL ),
    nKey( r.nKey ),
    ppEntries( NULL ),
    nEntryCount( r.nEntryCount )
{
    if (nEntryCount)
    {
        ppEntries = new ScCondFormatEntry*[nEntryCount];
        for (USHORT i=0; i<nEntryCount; i++)
        {
            ppEntries[i] = new ScCondFormatEntry(*r.ppEntries[i]);
            ppEntries[i]->SetParent(this);
        }
    }
}

ScConditionalFormat* ScConditionalFormat::Clone(ScDocument* pNewDoc) const
{
    // echte Kopie der Formeln (fuer Ref-Undo / zwischen Dokumenten)

    if (!pNewDoc)
        pNewDoc = pDoc;

    ScConditionalFormat* pNew = new ScConditionalFormat(nKey, pNewDoc);
    DBG_ASSERT(!pNew->ppEntries, "wo kommen die Eintraege her?");

    if (nEntryCount)
    {
        pNew->ppEntries = new ScCondFormatEntry*[nEntryCount];
        for (USHORT i=0; i<nEntryCount; i++)
        {
            pNew->ppEntries[i] = new ScCondFormatEntry( pNewDoc, *ppEntries[i] );
            pNew->ppEntries[i]->SetParent(pNew);
        }
        pNew->nEntryCount = nEntryCount;
    }

    return pNew;
}

ScConditionalFormat::ScConditionalFormat(SvStream& rStream, ScMultipleReadHeader& rHdr,
                                            ScDocument* pDocument) :
    pDoc( pDocument ),
    pAreas( NULL ),
    ppEntries( NULL ),
    nEntryCount( 0 )
{
    //  ein Eintrag im Header fuer die ScConditionalFormat-Daten,
    //  je zwei Eintraege fuer jede Bedingung (ScConditionEntry und ScCondFormatEntry)

    rHdr.StartEntry();

    rStream >> nKey;
    rStream >> nEntryCount;

    rHdr.EndEntry();

        //  Eintraege laden

    if (nEntryCount)
    {
        ppEntries = new ScCondFormatEntry*[nEntryCount];
        for (USHORT i=0; i<nEntryCount; i++)
        {
            ppEntries[i] = new ScCondFormatEntry(rStream, rHdr, pDocument);
            ppEntries[i]->SetParent(this);
        }
    }
}

void ScConditionalFormat::Store(SvStream& rStream, ScMultipleWriteHeader& rHdr) const
{
    //  ein Eintrag im Header fuer die ScConditionalFormat-Daten,
    //  je zwei Eintraege fuer jede Bedingung (ScConditionEntry und ScCondFormatEntry)

    rHdr.StartEntry();

    rStream << nKey;
    rStream << nEntryCount;

    rHdr.EndEntry();

        //  Eintraege speichern

    for (USHORT i=0; i<nEntryCount; i++)
        ppEntries[i]->Store(rStream, rHdr);
}

BOOL ScConditionalFormat::EqualEntries( const ScConditionalFormat& r ) const
{
    if ( nEntryCount != r.nEntryCount )
        return FALSE;

    //! auf gleiche Eintraege in anderer Reihenfolge testen ???

    for (USHORT i=0; i<nEntryCount; i++)
        if ( ! (*ppEntries[i] == *r.ppEntries[i]) )
            return FALSE;

    return TRUE;
}

void ScConditionalFormat::AddEntry( const ScCondFormatEntry& rNew )
{
    ScCondFormatEntry** ppNew = new ScCondFormatEntry*[nEntryCount+1];
    for (USHORT i=0; i<nEntryCount; i++)
        ppNew[i] = ppEntries[i];
    ppNew[nEntryCount] = new ScCondFormatEntry(rNew);
    ppNew[nEntryCount]->SetParent(this);
    ++nEntryCount;
    delete[] ppEntries;
    ppEntries = ppNew;
}

ScConditionalFormat::~ScConditionalFormat()
{
    for (USHORT i=0; i<nEntryCount; i++)
        delete ppEntries[i];
    delete[] ppEntries;

    delete pAreas;
}

const ScCondFormatEntry* ScConditionalFormat::GetEntry( USHORT nPos ) const
{
    if ( nPos < nEntryCount )
        return ppEntries[nPos];
    else
        return NULL;
}

const String& ScConditionalFormat::GetCellStyle( ScBaseCell* pCell, const ScAddress& rPos ) const
{
    for (USHORT i=0; i<nEntryCount; i++)
        if ( ppEntries[i]->IsCellValid( pCell, rPos ) )
            return ppEntries[i]->GetStyle();

    return EMPTY_STRING;
}

void lcl_Extend( ScRange& rRange, ScDocument* pDoc, BOOL bLines )
{
    SCTAB nTab = rRange.aStart.Tab();
    DBG_ASSERT(rRange.aEnd.Tab() == nTab, "lcl_Extend - mehrere Tabellen?");

    SCCOL nStartCol = rRange.aStart.Col();
    SCROW nStartRow = rRange.aStart.Row();
    SCCOL nEndCol = rRange.aEnd.Col();
    SCROW nEndRow = rRange.aEnd.Row();

    BOOL bEx = pDoc->ExtendMerge( nStartCol, nStartRow, nEndCol, nEndRow, nTab );

    if (bLines)
    {
        if (nStartCol > 0)    --nStartCol;
        if (nStartRow > 0)    --nStartRow;
        if (nEndCol < MAXCOL) ++nEndCol;
        if (nEndRow < MAXROW) ++nEndRow;
    }

    if ( bEx || bLines )
    {
        rRange.aStart.Set( nStartCol, nStartRow, nTab );
        rRange.aEnd.Set( nEndCol, nEndRow, nTab );
    }
}

BOOL lcl_CutRange( ScRange& rRange, const ScRange& rOther )
{
    rRange.Justify();
    ScRange aCmpRange = rOther;
    aCmpRange.Justify();

    if ( rRange.aStart.Col() <= aCmpRange.aEnd.Col() &&
         rRange.aEnd.Col() >= aCmpRange.aStart.Col() &&
         rRange.aStart.Row() <= aCmpRange.aEnd.Row() &&
         rRange.aEnd.Row() >= aCmpRange.aStart.Row() &&
         rRange.aStart.Tab() <= aCmpRange.aEnd.Tab() &&
         rRange.aEnd.Tab() >= aCmpRange.aStart.Tab() )
    {
        if ( rRange.aStart.Col() < aCmpRange.aStart.Col() )
            rRange.aStart.SetCol( aCmpRange.aStart.Col() );
        if ( rRange.aStart.Row() < aCmpRange.aStart.Row() )
            rRange.aStart.SetRow( aCmpRange.aStart.Row() );
        if ( rRange.aStart.Tab() < aCmpRange.aStart.Tab() )
            rRange.aStart.SetTab( aCmpRange.aStart.Tab() );
        if ( rRange.aEnd.Col() > aCmpRange.aEnd.Col() )
            rRange.aEnd.SetCol( aCmpRange.aEnd.Col() );
        if ( rRange.aEnd.Row() > aCmpRange.aEnd.Row() )
            rRange.aEnd.SetRow( aCmpRange.aEnd.Row() );
        if ( rRange.aEnd.Tab() > aCmpRange.aEnd.Tab() )
            rRange.aEnd.SetTab( aCmpRange.aEnd.Tab() );

        return TRUE;
    }

    return FALSE;       // ausserhalb
}

void ScConditionalFormat::DoRepaint( const ScRange* pModified )
{
    USHORT i;
    SfxObjectShell* pSh = pDoc->GetDocumentShell();
    if (pSh)
    {
        //  Rahmen/Schatten enthalten?
        //  (alle Bedingungen testen)
        BOOL bExtend = FALSE;
        BOOL bRotate = FALSE;
        for (i=0; i<nEntryCount; i++)
        {
            String aStyle = ppEntries[i]->GetStyle();
            if (aStyle.Len())
            {
                SfxStyleSheetBase* pStyleSheet =
                    pDoc->GetStyleSheetPool()->Find( aStyle, SFX_STYLE_FAMILY_PARA );
                if ( pStyleSheet )
                {
                    const SfxItemSet& rSet = pStyleSheet->GetItemSet();
                    if (rSet.GetItemState( ATTR_BORDER, TRUE ) == SFX_ITEM_SET ||
                        rSet.GetItemState( ATTR_SHADOW, TRUE ) == SFX_ITEM_SET)
                    {
                        bExtend = TRUE;
                    }
                    if (rSet.GetItemState( ATTR_ROTATE_VALUE, TRUE ) == SFX_ITEM_SET ||
                        rSet.GetItemState( ATTR_ROTATE_MODE, TRUE ) == SFX_ITEM_SET)
                    {
                        bRotate = TRUE;
                    }
                }
            }
        }

        if (!pAreas)        //  RangeList ggf. holen
        {
            pAreas = new ScRangeList;
            pDoc->FindConditionalFormat( nKey, *pAreas );
        }
        USHORT nCount = (USHORT) pAreas->Count();
        for (i=0; i<nCount; i++)
        {
            ScRange aRange = *pAreas->GetObject(i);
            BOOL bDo = TRUE;
            if ( pModified )
            {
                if ( !lcl_CutRange( aRange, *pModified ) )
                    bDo = FALSE;
            }
            if (bDo)
            {
                lcl_Extend( aRange, pDoc, bExtend );        // zusammengefasste und bExtend
                if ( bRotate )
                {
                    aRange.aStart.SetCol(0);
                    aRange.aEnd.SetCol(MAXCOL);     // gedreht: ganze Zeilen
                }

                // gedreht -> ganze Zeilen
                if ( aRange.aStart.Col() != 0 || aRange.aEnd.Col() != MAXCOL )
                {
                    if ( pDoc->HasAttrib( 0,aRange.aStart.Row(),aRange.aStart.Tab(),
                                            MAXCOL,aRange.aEnd.Row(),aRange.aEnd.Tab(),
                                            HASATTR_ROTATE ) )
                    {
                        aRange.aStart.SetCol(0);
                        aRange.aEnd.SetCol(MAXCOL);
                    }
                }

                pSh->Broadcast( ScPaintHint( aRange, PAINT_GRID ) );
            }
        }
    }
}

void ScConditionalFormat::InvalidateArea()
{
    delete pAreas;
    pAreas = NULL;
}

void ScConditionalFormat::CompileAll()
{
    for (USHORT i=0; i<nEntryCount; i++)
        ppEntries[i]->CompileAll();
}

void ScConditionalFormat::CompileXML()
{
    for (USHORT i=0; i<nEntryCount; i++)
        ppEntries[i]->CompileXML();
}

void ScConditionalFormat::UpdateReference( UpdateRefMode eUpdateRefMode,
                                const ScRange& rRange, SCsCOL nDx, SCsROW nDy, SCsTAB nDz )
{
    for (USHORT i=0; i<nEntryCount; i++)
        ppEntries[i]->UpdateReference(eUpdateRefMode, rRange, nDx, nDy, nDz);

    delete pAreas;      // aus dem AttrArray kommt beim Einfuegen/Loeschen kein Aufruf
    pAreas = NULL;
}

void ScConditionalFormat::UpdateMoveTab( SCTAB nOldPos, SCTAB nNewPos )
{
    for (USHORT i=0; i<nEntryCount; i++)
        ppEntries[i]->UpdateMoveTab( nOldPos, nNewPos );

    delete pAreas;      // aus dem AttrArray kommt beim Einfuegen/Loeschen kein Aufruf
    pAreas = NULL;
}

void ScConditionalFormat::SourceChanged( const ScAddress& rAddr )
{
    for (USHORT i=0; i<nEntryCount; i++)
        ppEntries[i]->SourceChanged( rAddr );
}

//------------------------------------------------------------------------

ScConditionalFormatList::ScConditionalFormatList(const ScConditionalFormatList& rList)
{
    //  fuer Ref-Undo - echte Kopie mit neuen Tokens!

    USHORT nCount = rList.Count();

    for (USHORT i=0; i<nCount; i++)
        InsertNew( rList[i]->Clone() );

    //!     sortierte Eintraege aus rList schneller einfuegen ???
}

ScConditionalFormatList::ScConditionalFormatList(ScDocument* pNewDoc,
                                                const ScConditionalFormatList& rList)
{
    //  fuer neues Dokument - echte Kopie mit neuen Tokens!

    USHORT nCount = rList.Count();

    for (USHORT i=0; i<nCount; i++)
        InsertNew( rList[i]->Clone(pNewDoc) );

    //!     sortierte Eintraege aus rList schneller einfuegen ???
}

BOOL ScConditionalFormatList::operator==( const ScConditionalFormatList& r ) const
{
    // fuer Ref-Undo - interne Variablen werden nicht verglichen

    USHORT nCount = Count();
    BOOL bEqual = ( nCount == r.Count() );
    for (USHORT i=0; i<nCount && bEqual; i++)           // Eintraege sind sortiert
        if ( !(*this)[i]->EqualEntries(*r[i]) )         // Eintraege unterschiedlich ?
            bEqual = FALSE;

    return bEqual;
}

void ScConditionalFormatList::Load( SvStream& rStream, ScDocument* pDocument )
{
    ScMultipleReadHeader aHdr( rStream );

    USHORT nNewCount;
    rStream >> nNewCount;

    for (USHORT i=0; i<nNewCount; i++)
    {
        ScConditionalFormat* pNew = new ScConditionalFormat( rStream, aHdr, pDocument );
        InsertNew( pNew );
    }
}

void ScConditionalFormatList::Store( SvStream& rStream ) const
{
    USHORT i;
    ScMultipleWriteHeader aHdr( rStream );

    USHORT nCount = Count();
    USHORT nUsed = 0;
    for (i=0; i<nCount; i++)
        if ((*this)[i]->IsUsed())
            ++nUsed;

    rStream << nUsed;       // Anzahl der gespeicherten

    for (i=0; i<nCount; i++)
    {
        const ScConditionalFormat* pForm = (*this)[i];
        if (pForm->IsUsed())
            pForm->Store( rStream, aHdr );
    }
}

ScConditionalFormat* ScConditionalFormatList::GetFormat( ULONG nKey )
{
    //! binaer suchen

    USHORT nCount = Count();
    for (USHORT i=0; i<nCount; i++)
        if ((*this)[i]->GetKey() == nKey)
            return (*this)[i];

    DBG_ERROR("ScConditionalFormatList: Eintrag nicht gefunden");
    return NULL;
}

void ScConditionalFormatList::ResetUsed()
{
    USHORT nCount = Count();
    for (USHORT i=0; i<nCount; i++)
        (*this)[i]->SetUsed(FALSE);
}

void ScConditionalFormatList::CompileAll()
{
    USHORT nCount = Count();
    for (USHORT i=0; i<nCount; i++)
        (*this)[i]->CompileAll();
}

void ScConditionalFormatList::CompileXML()
{
    USHORT nCount = Count();
    for (USHORT i=0; i<nCount; i++)
        (*this)[i]->CompileXML();
}

void ScConditionalFormatList::UpdateReference( UpdateRefMode eUpdateRefMode,
                                const ScRange& rRange, SCsCOL nDx, SCsROW nDy, SCsTAB nDz )
{
    USHORT nCount = Count();
    for (USHORT i=0; i<nCount; i++)
        (*this)[i]->UpdateReference( eUpdateRefMode, rRange, nDx, nDy, nDz );
}

void ScConditionalFormatList::UpdateMoveTab( SCTAB nOldPos, SCTAB nNewPos )
{
    USHORT nCount = Count();
    for (USHORT i=0; i<nCount; i++)
        (*this)[i]->UpdateMoveTab( nOldPos, nNewPos );
}

void ScConditionalFormatList::SourceChanged( const ScAddress& rAddr )
{
    USHORT nCount = Count();
    for (USHORT i=0; i<nCount; i++)
        (*this)[i]->SourceChanged( rAddr );
}



