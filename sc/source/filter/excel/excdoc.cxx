/*************************************************************************
 *
 *  $RCSfile: excdoc.cxx,v $
 *
 *  $Revision: 1.38 $
 *
 *  last change: $Author: dr $ $Date: 2002-11-21 12:15:59 $
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

//------------------------------------------------------------------------

#include <math.h>

#include "scitems.hxx"

#include <svx/svdobj.hxx>
#include <svx/svditer.hxx>
#include <svx/svdpage.hxx>
#include <svx/lrspitem.hxx>
#include <svx/ulspitem.hxx>
#include <svtools/intitem.hxx>
#include <svtools/zformat.hxx>
#include <so3/svstor.hxx>
#include <sfx2/objsh.hxx>
#include <tools/urlobj.hxx>
#include <rtl/ustring>

#include "drwlayer.hxx"

#include "cell.hxx"
#include "dociter.hxx"
#include "document.hxx"
#include "rangenam.hxx"
#include "dbcolect.hxx"
#include "global.hxx"
#include "globstr.hrc"
#include "progress.hxx"
#include "conditio.hxx"
#include "dpobject.hxx"
#include "attrib.hxx"
#include "scextopt.hxx"
#include "stlsheet.hxx"
#include "stlpool.hxx"
#include "olinetab.hxx"
#include "unonames.hxx"
#include "convuno.hxx"
#include "patattr.hxx"

#include "excdoc.hxx"
#include "excupn.hxx"
#include "namebuff.hxx"

#include "xcl97dum.hxx"
#include "xcl97rec.hxx"
#include "xcl97esc.hxx"

#ifndef SC_XELINK_HXX
#include "xelink.hxx"
#endif
#ifndef SC_XESTYLE_HXX
#include "xestyle.hxx"
#endif
#ifndef SC_XECONTENT_HXX
#include "xecontent.hxx"
#endif

#ifndef SC_XCLEXPPIVOTTABLES_HXX
#include "XclExpPivotTables.hxx"
#endif
#ifndef SC_XCLEXPCHANGETRACK_HXX
#include "XclExpChangeTrack.hxx"
#endif


NameBuffer*     ExcDocument::pTabNames = NULL;



static String lcl_GetVbaTabName( UINT16 n )
{
    String  aRet( RTL_CONSTASCII_USTRINGPARAM( "__VBA__" ) );
    aRet += String::CreateFromInt32( n );
    return aRet;
}




ExcRecordListRefs::~ExcRecordListRefs()
{
}




DefRowXFs::DefRowXFs( void )
{
    nLastList = 0;
    nLastRow = 0;
}


DefRowXFs::~DefRowXFs()
{
}


//void DefRowXFs::ChangeXF( ExcRow& rRow )
BOOL DefRowXFs::ChangeXF( UINT16 nRowNum, UINT16& rXF )
{
    UINT32  nCnt;
    UINT16  nR, nXF;

    nCnt = ScfUInt32List::Count();
    for( UINT32 n = ( nRowNum > nLastRow )? nLastList : 0 ; n < nCnt ; n++ )
    {
        Get( ScfUInt32List::GetValue( n ), nR, nXF );
        if( nRowNum == nR )
        {
            rXF = nXF;

            nLastList = n;
            nLastRow = nR;

            return TRUE;
        }
    }
    return FALSE;
}




ExcRowBlock* ExcTable::pRowBlock = NULL;

ExcTable::ExcTable( RootData* pRD ) :
    ExcRoot( pRD ),
    nScTab( 0 ),
    nExcTab( EXC_TABBUF_INVALID ),
    pDefRowXFs( NULL )
{   }


ExcTable::ExcTable( RootData* pRD, UINT16 nScTable ) :
    ExcRoot( pRD ),
    nScTab( nScTable ),
    nExcTab( pRD->pER->GetTabIdBuffer().GetXclTab( nScTable ) ),
    pDefRowXFs( NULL )
{   }


ExcTable::~ExcTable()
{
    Clear();
}


void ExcTable::Clear( void )
{
    if( pDefRowXFs )
    {
        delete pDefRowXFs;
        pDefRowXFs = NULL;
    }
}


void ExcTable::AddRow( ExcRow* pRow )
{
    DBG_ASSERT( pRowBlock, "ExcTable::AddRow() - missing RowBlock!" );
    ExcRowBlock* pNewRowBlock = pRowBlock->Append( pRow );
    if( pNewRowBlock )
    {
        pRowBlock = pNewRowBlock;
        Add( pRowBlock );
    }
}


void ExcTable::AddUsedRow( ExcRow*& rpRow )
{
    if( rpRow->IsDefault() )
        delete rpRow;
    else
        AddRow( rpRow );
}


void ExcTable::SetDefRowXF( UINT16 nXF, UINT16 n )
{
    if( !pDefRowXFs )
        pDefRowXFs = new DefRowXFs;

    pDefRowXFs->Add( n, nXF );
}


void ExcTable::FillAsHeader( ExcRecordListRefs& rBSRecList )
{
    RootData&           rR              = *pExcRoot;
    ScDocument&         rDoc            = *rR.pDoc;
    XclExpTabIdBuffer&  rTabBuffer      = rR.pER->GetTabIdBuffer();

    if ( rR.eDateiTyp < Biff8 )
        Add( new ExcBofW );
    else
        Add( new ExcBofW8 );

    UINT16  nC;
    String  aTmpString;
    UINT16  nScTabCount     = rTabBuffer.GetScTabCount();
    UINT16  nExcTabCount    = rTabBuffer.GetXclTabCount();
    UINT16  nCodenames      = rTabBuffer.GetCodenameCount();

    ExcNameList*    pNameList   = rR.pNameList  = new ExcNameList( rR );
    UsedFormList*   pFormRecs   = rR.pFormRecs  = new UsedFormList( rR );
    UsedAttrList*   pXFRecs     = rR.pXFRecs    = new UsedAttrList( &rR, *pFormRecs );

    rR.pObjRecs = NULL;             // per sheet
    rR.pNoteRecs = NULL;            // per sheet

    pFormRecs->SetBaseIndex( 164 ); // siehe auch ValueFormBuffer::nNewFormats
    pXFRecs->SetBaseIndex( 21 );

    if( rR.eDateiTyp < Biff8 )
        Add( new ExcDummy_00 );
    else
    {
        // first create style XFs
        SfxStyleSheetIterator   aStyleIter( rDoc.GetStyleSheetPool(), SFX_STYLE_FAMILY_PARA );
        SfxStyleSheetBase*      pStyle = aStyleIter.First();
        ScPatternAttr*          pPatt;

        while( pStyle )
        {
            if( pStyle->IsUserDefined() )
            {
                pPatt = new ScPatternAttr( &pStyle->GetItemSet() );
                pXFRecs->Find( pPatt, TRUE );
            }
            pStyle = aStyleIter.Next();
        }

        Add( new ExcDummy8_00a );
        rR.pTabId = new XclExpChTrTabId( Max( nExcTabCount, nCodenames ) );
        Add( rR.pTabId );
        if( rR.bWriteVBAStorage )
        {
            Add( new XclObproj );
            const String*   p = rR.pExtDocOpt->GetCodename();
            if( p )
                Add( new XclCodename( *p ) );
        }
        Add( new ExcDummy8_00b );
    }

    // erst Namen- und Tabellen-Eintraege aufbauen
    DBG_ASSERT( rDoc.GetRangeName(), "-ExcTable::Header(): Keine Namen? Kann nicht angehen!" );

    String          aName;

    for( nC = 0 ; nC < nScTabCount ; nC++ )
        if( rTabBuffer.IsExportTable( nC ) )
        {
            rDoc.GetName( nC, aTmpString );
            *ExcDocument::pTabNames << aTmpString;
        }

    if ( rR.eDateiTyp < Biff8 )
    {
        // Externcount & Externsheet
        ExcExterncount* pExtCnt = new ExcExterncount( &rR, FALSE );

        Add( pExtCnt );

        ExcExternsheetList*     pExcExtShtList = new ExcExternsheetList;

        DBG_ASSERT( !rR.pExtSheetCntAndRecs, "*ExcTable::Header(): pExtSheetCntAndRecs already exist!" );

        rR.pExtSheetCntAndRecs = new ExcExternDup( *pExtCnt, *pExcExtShtList );

        Add( pExcExtShtList );

        for( nC = 0 ; nC < nScTabCount ; nC++ )
            if( rTabBuffer.IsExportTable( nC ) )
                pExcExtShtList->Add( new ExcExternsheet( &rR, nC ) );

        // Names
        Add( pNameList );

        Add( new ExcDummy_040 );
        Add( new Exc1904( rDoc ) );
        Add( new ExcDummy_041 );
        // Font
        Add( new XclExpRefRecord( rR.pER->GetFontBuffer() ) );
        // Format
        Add( new ExcDummy_Fm );
        Add( pFormRecs );
        // XF + Style
        Add( new ExcDummy_XF );
        Add( pXFRecs );
        // Style
        Add( new ExcDummy_Style );
        // Colors
        Add( new XclExpRefRecord( rR.pER->GetPalette() ) );

        // Bundlesheet
        ExcBundlesheetBase* pBS;
        for( nC = 0 ; nC < nScTabCount ; nC++ )
            if( rTabBuffer.IsExportTable( nC ) )
            {
                pBS = new ExcBundlesheet( rR, nC );
                Add( pBS );
                rBSRecList.Append( pBS );
            }
    }
    else
    {
        Add( new ExcDummy8_040 );
        Add( new ExcWindow18( rR ) );
        Add( new Exc1904( rDoc ) );
        Add( new ExcDummy8_041 );
        // Font
        Add( new XclExpRefRecord( rR.pER->GetFontBuffer() ) );
        // Format
        Add( new ExcDummy8_Fm );
        Add( pFormRecs );
        // XF + Style
        Add( new ExcDummy8_XF );
        Add( pXFRecs );
        // Style
        Add( new ExcDummy8_Style );

        // Pivot Cache
        ScDPCollection*     pDPColl = rDoc.GetDPCollection();
        if( pDPColl )
        {
            XclPivotCacheList* pPCList = new XclPivotCacheList( &rR, *pDPColl );
            rR.pPivotCacheList = pPCList;

            for( const XclPivotCache* pCache = pPCList->First(); pCache; pCache = pPCList->Next() )
            {
                Add( new XclSxIdStm( *pCache ) );
                Add( new XclSxVs( *pCache ) );
                Add( new XclDConRef( pCache->GetRange(), pCache->GetWorkbook() ) );
            }
        }

        // Colors
        Add( new XclExpRefRecord( rR.pER->GetPalette() ) );

        // Change tracking
        if( rDoc.GetChangeTrack() )
        {
            rR.pUserBViewList = new XclExpUserBViewList( *rDoc.GetChangeTrack() );
            Add( rR.pUserBViewList );
        }

        // Natural Language Formulas Flag
        Add( new ExcDummy8_UsesElfs );

        // Bundlesheet
        ExcBundlesheetBase* pBS;
        for( nC = 0 ; nC < nScTabCount ; nC++ )
            if( rTabBuffer.IsExportTable( nC ) )
            {
                pBS = new ExcBundlesheet8( rR, nC );
                Add( pBS );
                rBSRecList.Append( pBS );
            }

        for( UINT16 nAdd = 0; nC < nCodenames ; nC++, nAdd++ )
        {
            aTmpString = lcl_GetVbaTabName( nAdd );
            pBS = new ExcBundlesheet8( aTmpString );
            Add( pBS );
            rBSRecList.Append( pBS );
        }

        // COUNTRY always Germany
        Add( new ExcDummy8_Country );
        // SUPBOOKs, XCTs, CRNs, EXTERNSHEET
        Add( new XclExpRefRecord( rR.pER->GetLinkManager() ) );
        // NAMEs
        Add( pNameList );

        // MSODRAWINGGROUP per-document data
        Add( new XclMsodrawinggroup( rR, ESCHER_DggContainer ) );
        // SST, EXTSST
        Add( new XclExpRefRecord( rR.pER->GetSst() ) );
    }

    Add( new ExcEof );
}


void ExcTable::FillAsTable( void )
{
    RootData&           rR          = *pExcRoot;
    ScDocument&         rDoc        = *rR.pDoc;
    XclExpTabIdBuffer&  rTabBuffer  = rR.pER->GetTabIdBuffer();

    if( nScTab >= rTabBuffer.GetScTabCount() )
    {
        CodenameList*       pL = rR.pExtDocOpt->GetCodenames();
        if( pL )
        {
            const String*   p = pL->Next();
            if( p )
                NullTab( p );
        }

        return;
    }

    UINT16                  nLastCol, nLastRow,         // in Tabelle
                            nFirstCol, nFirstRow;
    UINT16                  nPrevRow = 0;
    UINT16                  nColMin;                    // fuer aktuelle Zeile
                                                        //  Row-Records
    const UINT16            nDefXF = 0x0F;
    UINT16                  nCol = 0;
    UINT16                  nRow = 0;

    UINT16                  nMaxFlagCol = rDoc.GetLastFlaggedCol( nScTab );
    UINT16                  nMaxFlagRow = rDoc.GetLastFlaggedRow( nScTab );

    ExcCell*                pAktExcCell;
    SvNumberFormatter&      rFormatter = *rR.pDoc->GetFormatTable();
    const BiffTyp           eDateiTyp = rR.eDateiTyp;

    SfxStyleSheet*          pStSh = ( SfxStyleSheet* ) rDoc.GetStyleSheetPool()->Find(
                                        rDoc.GetPageStyle( nScTab ), SFX_STYLE_FAMILY_PAGE );
    rR.pStyleSheet = pStSh;

    SfxItemSet*             pStyleSheetItemSet = pStSh? &pStSh->GetItemSet() : NULL;
    rR.pStyleSheetItemSet = pStyleSheetItemSet;

    XclExpHyperlinkList*    pHlinks = new XclExpHyperlinkList;
    XclExpTableOpManager    aTableOpList;
    XclExpTableOp*          pTableOpRec = NULL;

    ExcArrays               aArrayFormList;
    ExcArray*               pLastArray = NULL;

    ExcArrays               aShrdFmlaList;
    ExcShrdFmla*            pShrdFmla = NULL;

    XclExpDval*             pRecDval = NULL;        // data validation

    DBG_ASSERT( (nScTab >= 0L) && (nScTab <= MAXTAB), "-ExcTable::Table(): nScTab - no ordinary table!" );
    DBG_ASSERT( (nExcTab >= 0L) && (nExcTab <= MAXTAB), "-ExcTable::Table(): nExcTab - no ordinary table!" );

    rDoc.GetTableArea( nScTab, nLastCol, nLastRow );

    if( nLastRow > rR.nRowMax )     // max. Zeilenzahl ueberschritten?
    {
        nLastRow = rR.nRowMax;
        rR.bCellCut = TRUE;
    }

    // find outline range
    ScOutlineTable*         pOLTable    = rDoc.GetOutlineTable( nScTab );
    ScOutlineArray*         pOLColArray = NULL;
    ScOutlineArray*         pOLRowArray = NULL;
    if( pOLTable )
    {
        UINT16              nStart, nEnd;
        UINT16              nMaxOLCol = 0;
        UINT16              nMaxOLRow = 0;

        pOLColArray = pOLTable->GetColArray();
        if( pOLColArray )
        {
            pOLColArray->GetRange( nStart, nEnd );
            nMaxOLCol = nEnd + 1;
        }
        nMaxFlagCol = Max( nMaxFlagCol, nMaxOLCol );

        pOLRowArray = pOLTable->GetRowArray();
        if( pOLRowArray )
        {
            pOLRowArray->GetRange( nStart, nEnd );
            nMaxOLRow = nEnd + 1;
        }
        nMaxFlagRow = Max( nMaxFlagRow, nMaxOLRow );
    }
    nMaxFlagCol = Min( nMaxFlagCol, (UINT16) MAXCOL );
    nMaxFlagRow = Min( Min( nMaxFlagRow, (UINT16) MAXROW ), rR.nRowMax );

    ExcEOutline aExcOLCol( pOLColArray );
    ExcEOutline aExcOLRow( pOLRowArray );

    DBG_ASSERT( !rR.pCellMerging, "ExcTable::FillAsTable - old merging list found" );
    rR.pCellMerging = new XclExpCellMerging;


//    ScUsedAreaIterator      aIterator( &rDoc, nScTab, 0, 0, nLastCol, nLastRow );
    ScUsedAreaIterator      aIterator( &rDoc, nScTab, 0, 0, MAXCOL, nLastRow );
    const ScBaseCell*       pAktScCell;
    const ScPatternAttr*    pPatt;
    ExcBlankMulblank*       pLastBlank = NULL;
    ExcRKMulRK*             pLastRKMulRK = NULL;
    BOOL                    bIter;

    // jetz schon, um erste Zeile zu bekommen
    bIter = aIterator.GetNext();
    if( bIter )
    {
        nCol = aIterator.GetStartCol();
        nRow = aIterator.GetRow();

        if( nRow > rR.nRowMax )     // max. Zeilenzahl ueberschritten?
        {
            rR.bCellCut = TRUE;
            NullTab();
            return;
        }
        pAktScCell = aIterator.GetCell();
        pPatt = aIterator.GetPattern();
    }

    // Header und Default-Recs
    if( eDateiTyp < Biff8 )
    {
        Add( new ExcBof );
        Add( new ExcDummy_02a );
        Add( new ExcPrintheaders( pStyleSheetItemSet ) );
        Add( new ExcPrintGridlines( pStyleSheetItemSet ) );
        Add( new ExcDummy_02b );
        // GUTS (count & size of outline icons)
        Add( new ExcEGuts( pOLColArray, pOLRowArray ) );
        Add( new ExcDummy_02c );
        Add( new XclExpWsbool( rR ) );
        Add( new XclExpPagebreaks( rR, nScTab, xlPBHorizontal ) );
        Add( new XclExpPagebreaks( rR, nScTab, xlPBVertical ) );
    }
    else
    {
        Add( new ExcBof8 );
        Add( new XclCalccount( rDoc ) );
        Add( new XclRefmode() );
        Add( new XclIteration( rDoc ) );
        Add( new XclDelta( rDoc ) );
        Add( new ExcPrintheaders( pStyleSheetItemSet ) );
        Add( new ExcPrintGridlines( pStyleSheetItemSet ) );
        Add( new ExcDummy8_02 );
        // GUTS (count & size of outline icons)
        Add( new ExcEGuts( pOLColArray, pOLRowArray ) );
        Add( new XclExpWsbool( rR ) );
        Add( new XclExpPagebreaks8( rR, nScTab, xlPBHorizontal ) );
        Add( new XclExpPagebreaks8( rR, nScTab, xlPBVertical ) );
    }

    Add( new XclExpHeader( rR ) );
    Add( new XclExpFooter( rR ) );
    Add( new ExcHcenter( pStyleSheetItemSet ) );
    Add( new ExcVcenter( pStyleSheetItemSet ) );

    // margins
    const SvxLRSpaceItem&   rLRSpaceItem = ( const SvxLRSpaceItem& ) pStyleSheetItemSet->Get( ATTR_LRSPACE );
    Add( new XclExpMargin( rLRSpaceItem.GetLeft(), xlLeftMargin ) );
    Add( new XclExpMargin( rLRSpaceItem.GetRight(), xlRightMargin ) );
        const SvxULSpaceItem&   rULSpaceItem = ( const SvxULSpaceItem& ) pStyleSheetItemSet->Get( ATTR_ULSPACE );
    Add( new XclExpMargin( rULSpaceItem.GetUpper(), xlTopMargin ) );
    Add( new XclExpMargin( rULSpaceItem.GetLower(), xlBottomMargin ) );

    Add( new ExcSetup( &rR ) );

    if( eDateiTyp >= Biff8 )
    {
        Add( new XclExpBitmap( *rR.pER ) );

        if( rDoc.IsTabProtected( nScTab ) )
            Add( new XclProtection() );
    }

    if ( eDateiTyp < Biff8 && rR.pExtSheetCntAndRecs )
        Add( new ExcExternDup( *rR.pExtSheetCntAndRecs ) );

    // Defcolwidth (Breite aus Excel-Dokument)
    ExcDefcolwidth*             pExcDefColWidth = new ExcDefcolwidth( 0x000a );
    Add( pExcDefColWidth );

    // COLINFO records
    ExcColinfo* pLastColInfo = new ExcColinfo( 0, nScTab, nDefXF, rR, aExcOLCol );
    ExcColinfo* pNewColInfo;

    Add( pLastColInfo );
    for( UINT16 iCol = 1; iCol <= MAXCOL; iCol++ )
    {
        pNewColInfo = new ExcColinfo( iCol, nScTab, nDefXF, rR, aExcOLCol );
        pLastColInfo->Expand( pNewColInfo );
        if( pNewColInfo )
        {
            pLastColInfo = pNewColInfo;
            Add( pLastColInfo );
        }
    }

    // Dimensions
    ExcDimensions*          pDimensions = new ExcDimensions( rR.eDateiTyp );
    Add( pDimensions );

    if ( rR.eDateiTyp >= Biff8 )
    {
        // Scenarios
        Add( new ExcEScenarioManager( rDoc, nScTab ) );
        // list holding OBJ records and creating MSODRAWING per-sheet data
        rR.pObjRecs = new XclObjList( rR );
        // AutoFilter
        Add( new ExcAutoFilterRecs( rR, nScTab ) );
        // list of NOTE records
        rR.pNoteRecs = new XclNoteList;
    }

    // NOTE
    const ScPostIt*         pNote = NULL;

    // rows & cols
    nFirstRow = nRow;
    nColMin = nFirstCol = nCol;
    pRowBlock = new ExcRowBlock;
    Add( pRowBlock );

    // at least one ROW rec
    if( !bIter )
        AddRow( new ExcRow( 0, nScTab, 0, 0, nDefXF, rDoc, aExcOLRow, *this ) );

    while( bIter )
    {
        nCol = aIterator.GetStartCol();     // nur bei erstem Durchlauf doppelt!
        pAktScCell = aIterator.GetCell();
        pPatt = aIterator.GetPattern();

        pAktExcCell = NULL;
        pTableOpRec = NULL;

        // add ROW recs from empty rows
        while( nPrevRow < nRow )
        {
            ExcRow* pRow = new ExcRow( nPrevRow, nScTab, 0, 0, nDefXF, rDoc, aExcOLRow, *this );
            AddUsedRow( pRow );
            nPrevRow++;
        }

        ScAddress   aScPos( nCol, nRow, nScTab );
        rR.sAddNoteText.Erase();

        if( pAktScCell )
        {// nicht-leere Zelle
            pLastBlank = NULL;
            pNote = pAktScCell->GetNotePtr();

            switch( pAktScCell->GetCellType() )
            {
                case CELLTYPE_NONE:
                    pLastRKMulRK = NULL;
                    break;
                case CELLTYPE_VALUE:
                {
                    double  fVal = ( ( ScValueCell * ) pAktScCell )->GetValue();
                    INT32   nRKValue;
                    if ( pPatt && (fVal == 0.0 || fVal == 1.0) &&
                            rFormatter.GetType(
                            ((const SfxUInt32Item&)pPatt->GetItem(
                            ATTR_VALUE_FORMAT )).GetValue() ) == NUMBERFORMAT_LOGICAL )
                    {
                        pLastRKMulRK = NULL;
                        pAktExcCell = new ExcBoolerr( aScPos, pPatt, rR, UINT8(fVal), FALSE );
                    }
                    else if( XclTools::GetRKFromDouble( nRKValue, fVal ) )
                    {
                        if( pLastRKMulRK )
                        {
                            ExcRKMulRK* pNewRK = pLastRKMulRK->Extend( aScPos, pPatt, rR, nRKValue );
                            if( pNewRK )
                                pLastRKMulRK = pNewRK;

                            pAktExcCell = pNewRK;
                        }
                        else
                            pAktExcCell = pLastRKMulRK = new ExcRKMulRK( aScPos, pPatt, rR, nRKValue );
                    }
                    else
                    {
                        pAktExcCell = new ExcNumber( aScPos, pPatt, rR, fVal );
                        pLastRKMulRK = NULL;
                    }
                }
                break;
                case CELLTYPE_STRING:
                {
                    pLastRKMulRK = NULL;
                    String  aTemp;

                    ( ( ScStringCell* ) pAktScCell )->GetString( aTemp );

                    if ( rR.eDateiTyp < Biff8 )
                        pAktExcCell = new ExcLabel( aScPos, pPatt, rR, aTemp );
                    else
                        pAktExcCell = new ExcLabelSst( aScPos, pPatt, rR, aTemp );
                }
                break;
                case CELLTYPE_FORMULA:
                {
                    pLastRKMulRK = NULL;
                    ScFormulaCell*      pFormCell = ( ScFormulaCell * ) pAktScCell;
                    ULONG   nCellNumForm = ( pPatt ?
                        (( const SfxUInt32Item& ) pPatt->GetItem(
                        ATTR_VALUE_FORMAT )).GetValue() : 0 );
                    ULONG   nAltNumForm;
                    BOOL    bForceAltNumForm;
                    if( ( nCellNumForm % SV_COUNTRY_LANGUAGE_OFFSET ) == 0 )
                    {
                        // #73420# Xcl doesn't know boolean number formats,
                        // we write "TRUE";"TRUE";"FALSE" or "WAHR";"WAHR";"FALSCH"
                        // or any other language dependent key words instead.
                        // Don't do it for automatic formula formats,
                        // because Xcl gets them right.
                        if( pFormCell->GetFormatType() == NUMBERFORMAT_LOGICAL )
                            nAltNumForm = NUMBERFORMAT_ENTRY_NOT_FOUND;
                        else
                            nAltNumForm = pFormCell->GetStandardFormat(
                                rFormatter, nCellNumForm );
                        bForceAltNumForm = FALSE;
                    }
                    else
                    {
                        // #73420# If number format set is boolean and
                        // automatic format is boolean don't write that ugly
                        // special format.
                        if( pFormCell->GetFormatType() == NUMBERFORMAT_LOGICAL
                                && rFormatter.GetType( nCellNumForm ) == NUMBERFORMAT_LOGICAL )
                        {
                            nAltNumForm = 0;
                            bForceAltNumForm = TRUE;
                        }
                        else
                        {
                            nAltNumForm = NUMBERFORMAT_ENTRY_NOT_FOUND;
                            bForceAltNumForm = FALSE;
                        }

                    }
                    ExcFormula* pFmlaCell = new ExcFormula(
                        aScPos, pPatt, rR, nAltNumForm, bForceAltNumForm, *pFormCell->GetCode(),
                        &pLastArray, ( ScMatrixMode ) pFormCell->GetMatrixFlag(), &pShrdFmla, &aShrdFmlaList );
                    pAktExcCell = pFmlaCell;
                    pTableOpRec = aTableOpList.InsertCell( pFormCell->GetCode(), *pFmlaCell );
                }
                break;
                case CELLTYPE_EDIT:
                {
                    pLastRKMulRK = NULL;
                    if( rR.eDateiTyp < Biff8 )
                        pAktExcCell = new ExcRString( aScPos, pPatt, rR, *((ScEditCell*) pAktScCell) );
                    else
                        pAktExcCell = new ExcLabelSst( aScPos, pPatt, rR, *((ScEditCell*) pAktScCell) );

                    XclExpHyperlink*& rpHlink = rR.pLastHlink;
                    if( rpHlink )
                    {
                        rpHlink->SetPosition( aScPos );
                        pHlinks->Append( rpHlink );
                        rpHlink = NULL;
                    }
                }
                break;
                case CELLTYPE_NOTE:
                {
                    pAktExcCell = NULL;
                    pLastRKMulRK = NULL;
                    DBG_ASSERT( pNote, "-ExcTable::Table(): Note-Cell ohne Note!" );
                }
                break;
#ifdef DBG_UTIL
                case CELLTYPE_DESTROYED:
                    pAktExcCell = NULL;
                    pLastRKMulRK = NULL;
                    break;
#endif
                default:
                    DBG_ERROR( "*ExcTable::Table(): Unbekannter Zelltyp" );
                    pAktExcCell = NULL;
                    pLastRKMulRK = NULL;
            }
        }
        else
        {// leere Zelle mit Attributierung
            pNote = NULL;

            UINT16  nColCnt = aIterator.GetEndCol() - aIterator.GetStartCol() + 1;

            if( pLastBlank && pLastBlank->GetLastCol() + 1 == aIterator.GetStartCol() )
            {
                pLastBlank->Add( aScPos, pPatt, rR, nColCnt, *this );

                pAktScCell = NULL;  // kein NEUER Record!
            }
            else
            {
                pLastBlank = new ExcBlankMulblank( aScPos, pPatt, rR, nColCnt, *this );
                pAktExcCell = pLastBlank;
            }
        }

        if( pAktExcCell )
        {
            Add( pAktExcCell );

            if( pLastArray )
            {
                if( aArrayFormList.Insert( pLastArray ) )
                    Add( pLastArray );  // really new
                else
                    delete pLastArray;  // allready added

                pLastArray = NULL;
            }

            if( pShrdFmla )
            {
                aShrdFmlaList.Append( pShrdFmla );
                Add( pShrdFmla );
                pShrdFmla = NULL;
            }
        }

        if( pTableOpRec )
            Add( pTableOpRec );

        // notes
        String sNoteText;
        String sNoteAuthor;
        if( pNote )
        {
            sNoteText = pNote->GetText();
            sNoteAuthor = pNote->GetAuthor();
        }
        if( rR.sAddNoteText.Len() )
        {
            if( sNoteText.Len() )
                (sNoteText += (sal_Unicode) 0x0A) += (sal_Unicode) 0x0A;
            sNoteText += rR.sAddNoteText;
        }
        if( sNoteText.Len() || sNoteAuthor.Len() )
        {
            if ( rR.eDateiTyp < Biff8 )
                Add( new ExcNote( aScPos, sNoteText, rR ) );
            else
                rR.pNoteRecs->Add( new XclNote( rR, aScPos, sNoteText, sNoteAuthor ) );
        }

        if( pPatt && (rR.eDateiTyp >= Biff8) )
        {
            // merged cells
            ScMergeAttr& rItem = (ScMergeAttr&) pPatt->GetItem( ATTR_MERGE );
            if( rItem.IsMerged() )
            {
                UINT16 nXF = (pAktExcCell ? pAktExcCell->GetXF() :
                            (pLastBlank ? pLastBlank->GetXF() :
                            (pLastRKMulRK ? pLastRKMulRK->GetXF() : 0)));
                for( UINT16 iCol = aIterator.GetStartCol(); iCol <= aIterator.GetEndCol(); iCol++ )
                    rR.pCellMerging->Append( iCol, rItem.GetColMerge(), nRow, rItem.GetRowMerge(), nXF );
            }

            // data validation
            const SfxPoolItem* pItem;
            if( pPatt->GetItemSet().GetItemState( ATTR_VALIDDATA, FALSE, &pItem ) == SFX_ITEM_SET )
            {
                sal_uInt32 nHandle = ((const SfxUInt32Item*)pItem)->GetValue();
                if( !pRecDval )
                    pRecDval = new XclExpDval( *rR.pER );
                ScRange aRange( aScPos );
                aRange.aEnd.SetCol( aIterator.GetEndCol() );
                pRecDval->InsertCellRange( aRange, nHandle );
            }
        }

        bIter = aIterator.GetNext();

        // new row number
        if( bIter )
        {
            nRow = aIterator.GetRow();
            if( nRow > rR.nRowMax )     // Excel row limit
                bIter = FALSE;
        }

        // new row -> add previous ROW rec
        if( !bIter || (nPrevRow < nRow) )
        {
            AddRow( new ExcRow( nPrevRow, nScTab, nColMin, nCol, nDefXF, rDoc, aExcOLRow, *this ) );

            nPrevRow++;
            nColMin = aIterator.GetStartCol();
            nFirstCol = Min( nFirstCol, nColMin );
            pLastBlank = NULL;
            pLastRKMulRK = NULL;
        }
    }

    // remaining rows with attributes
    while( nRow < nMaxFlagRow )
    {
        nRow++;
        ExcRow* pRow = new ExcRow( nRow, nScTab, 0, 0, nDefXF, rDoc, aExcOLRow, *this );
        AddUsedRow( pRow );
    }

    // insert merged cells
    Add( rR.pCellMerging );
    rR.pCellMerging = NULL;
    // label ranges
    if( rR.eDateiTyp >= Biff8 )
        Add( new XclExpLabelranges( *rR.pER ) );
    // insert data validation
    if( pRecDval )
        Add( pRecDval );
    pRecDval = NULL;
    // update dimensions
    pDimensions->SetLimits( nFirstCol, nFirstRow, nLastCol, nLastRow );
    // update formula cells for multiple operations
    aTableOpList.UpdateCells();

    if( rR.eDateiTyp < Biff8 )
    {
        Add( new ExcWindow2( nExcTab ) );
        Add( new ExcSelection( 0, 0, 3 ) );
    }
    else
    {
        ScDrawLayer* pDrawLayer = rDoc.GetDrawLayer();
        if( pDrawLayer )
        {
            SdrPage* pPage = pDrawLayer->GetPage( nScTab );
            if( pPage )
                rR.pEscher->GetEx()->AddSdrPage( *pPage );
        }
        //! close Escher group shape and ESCHER_DgContainer
        //! opened by XclObjList ctor MSODRAWING
        rR.pObjRecs->EndSheet();
        // all MSODRAWING and OBJ stuff of this sheet goes here
        Add( rR.pObjRecs );
        // NOTE records
        Add( rR.pNoteRecs );

        // pivot tables
        ScDPCollection*     pDPColl = rDoc.GetDPCollection();
        XclPivotCacheList*  pPCList = rR.pPivotCacheList;
        if( pDPColl && pPCList )
        {
            for( USHORT nObjCnt = 0; nObjCnt < pDPColl->GetCount(); nObjCnt++ )
            {
                ScDPObject*             pDPObject   = (*pDPColl)[ nObjCnt ];
                const XclPivotCache*    pCache      = pPCList->Get( nObjCnt );

                if( pDPObject && pCache )
                {
                    const ScRange& rRange = pDPObject->GetOutRange();
                    if( rRange.aStart.Tab() == nScTab )
                        Add( new XclPivotTableRecs( *pCache, nObjCnt ) );
                }
            }
        }

        // WINDOW2
        Add( new ExcWindow28( *rR.pER, nScTab ) );
    }

    if( rR.eDateiTyp >= Biff8 )
    {
        // web queries
        Add( new XclExpWebQueryBuffer( *rR.pER ) );

        // conditional formats
        const ScConditionalFormatList*  pCondFormList = rDoc.GetCondFormList();
        if( pCondFormList )
        {
            UINT32                      nCondCnt = pCondFormList->Count();
            ScConditionalFormat* const* ppCondForm = pCondFormList->GetData();
            ScRangeList*                pRangeList = NULL;

            while( nCondCnt )
            {
                if( *ppCondForm )
                {
                    const ScConditionalFormat&  rCF = **ppCondForm;

                    if( pRangeList )
                        pRangeList->Clear();
                    else
                        pRangeList = new ScRangeList;

                    rDoc.FindConditionalFormat( rCF.GetKey(), *pRangeList );

                    if( pRangeList->Count() )
                    {
                        Add( new XclCondFormat( rCF, pRangeList, rR ) );
                        pRangeList = NULL;
                    }
                }

                ppCondForm++;
                nCondCnt--;
            }

            if( pRangeList )
                delete pRangeList;
        }

        if( rR.bWriteVBAStorage )
        {
            CodenameList*       pL = rR.pExtDocOpt->GetCodenames();
            if( pL )
            {
                const String* p = nExcTab ? pL->Next() : pL->First();
                if( p )
                    Add( new XclCodename( *p ) );
            }
        }
    }

    rR.pStyleSheet = NULL;
    rR.pStyleSheetItemSet = NULL;

    Add( pHlinks );

    // change tracking
    if( rR.pUserBViewList )
    {
        for( const XclExpUserBView* pBView = rR.pUserBViewList->First(); pBView; pBView = rR.pUserBViewList->Next() )
        {
            Add( new XclExpUsersViewBegin( pBView->GetGUID(), nExcTab ) );
            Add( new XclExpUsersViewEnd );
        }
    }

    // EOF
    Add( new ExcEof );
}


void ExcTable::NullTab( const String* pCodename )
{
    DBG_ASSERT( (nScTab >= 0L) && (nScTab <= MAXTAB), "-ExcTable::Table(): nScTab - no ordinary table!" );
    DBG_ASSERT( (nExcTab >= 0L) && (nExcTab <= MAXTAB), "-ExcTable::Table(): nExcTab - no ordinary table!" );

    RootData&       rR = *pExcRoot;

    if ( rR.eDateiTyp < Biff8 )
    {
        Add( new ExcBof );
        Add( new ExcWindow2( nExcTab ) );
    }
    else
    {
        Add( new ExcBof8 );

        if( pCodename )
            Add( new XclCodename( *pCodename ) );
        else
        {
            // create at least the MSODRAWING per-sheet data
            rR.pObjRecs = new XclObjList( rR );
            // all drawing obects
            ScDrawLayer*    pDrawLayer = pExcRoot->pDoc->GetDrawLayer();
            if ( pDrawLayer )
            {
                SdrPage*    pPage = pDrawLayer->GetPage( nScTab );
                if ( pPage )
                    rR.pEscher->GetEx()->AddSdrPage( *pPage );
            }
            //! close Escher group shape and ESCHER_DgContainer
            //! opened by XclObjList ctor MSODRAWING
            rR.pObjRecs->EndSheet();
            Add( rR.pObjRecs );
        }
        // WINDOW2

        Add( new ExcWindow28( *rR.pER, nScTab ) );
    }
    Add( new ExcEof );
}


BOOL ExcTable::ModifyToDefaultRowXF( UINT16 nRowNum, UINT16& rXF )
{
    if( pDefRowXFs )
        return pDefRowXFs->ChangeXF( nRowNum, rXF );
    return FALSE;
}


void ExcTable::Write( XclExpStream& rStr )
{
    aRecList.Save( rStr );
}




ExcDocument::ExcDocument( const XclExpRoot& rRoot ) :
    XclExpRoot( rRoot ),
    aHeader( rRoot.mpRD ),
    pExpChangeTrack( NULL )
{
    pTabNames = new NameBuffer( 0, 16 );

    pPrgrsBar = new ScProgress(
        NULL, ScGlobal::GetRscString(STR_SAVE_DOC),
        ( UINT32 ) GetDoc().GetCellCount() * 2 );
    ExcCell::SetPrgrsBar( *pPrgrsBar );
}


ExcDocument::~ExcDocument()
{
    maTableList.Clear();    //! for the following assertion
    DBG_ASSERT( ExcCell::_nRefCount == 0, "*ExcDocument::~ExcDocument(): Ein'n hab'n wir noch!" );

    delete pTabNames;
#ifdef DBG_UTIL
    pTabNames = NULL;
#endif

    delete pPrgrsBar;
    ExcCell::ClearPrgrsBar();

    delete pExpChangeTrack;
}


void ExcDocument::ReadDoc( void )
{
    aHeader.FillAsHeader( aBundleSheetRecList );

    sal_uInt16 nScTabCount = GetTabIdBuffer().GetMaxScTabCount();
    while( GetScTab() < nScTabCount )
    {
        if( GetTabIdBuffer().IsExportTable( GetScTab() ) )
        {
            ExcTable* pTab = new ExcTable( mpRD, GetScTab() );
            maTableList.Append( pTab );
            pTab->FillAsTable();
        }
        IncScTab();
    }

    if ( GetBiff() >= xlBiff8 )
    {
        // complete temporary Escher stream
        mpRD->pEscher->GetEx()->EndDocument();

        // change tracking
        if ( GetDoc().GetChangeTrack() )
            pExpChangeTrack = new XclExpChangeTrack( mpRD );
    }
}


void ExcDocument::Write( SvStream& rSvStrm )
{
    if( !maTableList.Empty() )
    {
        if ( GetBiff() >= xlBiff8 )
            mpRD->pEscher->GetStrm().Seek(0);   // ready for take off

        GetPalette().ReduceColors();

        XclExpStream        aXclStrm( rSvStrm, *this );
        ExcTable*           pTab = maTableList.First();
        ExcBundlesheetBase* pAktBS = ( ExcBundlesheetBase* ) aBundleSheetRecList.First();

        aHeader.Write( aXclStrm );

        while( pTab )
        {
            DBG_ASSERT( pAktBS, "-ExcDocument::Write(): BundleSheetRecs und Tabs passen nicht zusammen!" );
            pAktBS->SetStreamPos( aXclStrm.GetStreamPos() );
            pTab->Write( aXclStrm );
            pTab = maTableList.Next();
            pAktBS = ( ExcBundlesheetBase* ) aBundleSheetRecList.Next();
        }

        DBG_ASSERT( !pAktBS, "+ExcDocument::Write(): mehr BundleSheetRecs als Tabs!" );

        // BundleSheetRecs anpassen
        pAktBS = ( ExcBundlesheetBase* ) aBundleSheetRecList.First();
        while( pAktBS )
        {
            pAktBS->UpdateStreamPos( aXclStrm );
            pAktBS = ( ExcBundlesheetBase* ) aBundleSheetRecList.Next();
        }

    }
    if( mpRD->pPivotCacheList )
        mpRD->pPivotCacheList->Write();
    if( pExpChangeTrack )
        pExpChangeTrack->Write();
}

