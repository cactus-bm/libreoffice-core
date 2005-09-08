/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: impop.cxx,v $
 *
 *  $Revision: 1.75 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 18:58:10 $
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
#ifdef PCH
#include "filt_pch.hxx"
#endif

#pragma hdrstop

#include "paramisc.hxx"
#ifndef _IMP_OP_HXX
#include "imp_op.hxx"
#endif

#ifndef INCLUDED_SVX_COUNTRYID_HXX
#include <svx/countryid.hxx>
#endif

#include "scitems.hxx"
#include <svx/eeitem.hxx>
#define ITEMID_FIELD EE_FEATURE_FIELD

#include <svx/editdata.hxx>
#include <svx/editeng.hxx>
#include <svx/editobj.hxx>
#include <svx/editstat.hxx>
#include <svx/flditem.hxx>
#include <svx/pageitem.hxx>
#include <svx/colritem.hxx>
#include <sfx2/printer.hxx>
#include <svtools/zforlist.hxx>

#include <sfx2/objsh.hxx>
#include "docuno.hxx"

#if defined( WNT ) || defined( WIN )
#include <math.h>
#else
#include <stdlib.h>
#endif

#include "cell.hxx"
#include "document.hxx"
#include "rangenam.hxx"
#include "compiler.hxx"
#include "patattr.hxx"
#include "attrib.hxx"
#include "globstr.hrc"
#include "global.hxx"
#include "markdata.hxx"
#include "olinetab.hxx"
#include "stlsheet.hxx"
#include "stlpool.hxx"
#include "compiler.hxx"
#include "viewopti.hxx"
#include "docoptio.hxx"
#include "scextopt.hxx"
#include "editutil.hxx"
#include "filtopt.hxx"
#include "scerrors.hxx"
#include "unonames.hxx"

#ifndef SC_FAPIHELPER_HXX
#include "fapihelper.hxx"
#endif

#ifndef SC_XLTOOLS_HXX
#include "xltools.hxx"
#endif
#ifndef SC_XLTABLE_HXX
#include "xltable.hxx"
#endif
#ifndef SC_XLVIEW_HXX
#include "xlview.hxx"
#endif
#ifndef SC_XLTRACER_HXX
#include "xltracer.hxx"
#endif
#ifndef SC_XIHELPER_HXX
#include "xihelper.hxx"
#endif
#ifndef SC_XIPAGE_HXX
#include "xipage.hxx"
#endif
#ifndef SC_XILINK_HXX
#include "xilink.hxx"
#endif
#ifndef SC_XIESCHER_HXX
#include "xiescher.hxx"
#endif

#include "excimp8.hxx"
#include "excform.hxx"

using namespace ::com::sun::star;


const double ImportExcel::fExcToTwips =
    ( double ) TWIPS_PER_CHAR / 256.0;


ImportTyp::ImportTyp( ScDocument* pDoc, CharSet eQ )
{
    eQuellChar = eQ;
    pD = pDoc;
}


ImportTyp::~ImportTyp()
{
}


FltError ImportTyp::Read()
{
    return eERR_INTERN;
}


ImportExcel::ImportExcel( XclImpRootData& rImpData ):
    ImportTyp( &rImpData.mrDoc, rImpData.meCharSet ),
    XclImpRoot( rImpData ),
    maStrm( rImpData.mrBookStrm, GetRoot() ),
    aIn( maStrm )
{
    pChart = pUsedChartFirst = pUsedChartLast = NULL;

    nBdshtTab = 0;
    nIxfeIndex = 0;     // zur Sicherheit auf 0

    // Root-Daten fuellen - nach new's ohne Root als Parameter
    pExcRoot = &GetOldRoot();
    pExcRoot->pIR = this;   // ExcRoot -> XclImpRoot
    pExcRoot->eDateiTyp = BiffX;
    pExcRoot->pExtSheetBuff = new ExtSheetBuffer( pExcRoot );   //&aExtSheetBuff;
    pExcRoot->pTabNameBuff = new NameBuffer( pExcRoot );        //&aTabNameBuff;
    pExcRoot->pShrfmlaBuff = new ShrfmlaBuffer( pExcRoot );     //&aShrfrmlaBuff;
    pExcRoot->pExtNameBuff = new ExtNameBuff ( pExcRoot );

    pExtNameBuff = new NameBuffer( pExcRoot );          //#94039# prevent empty rootdata
    pExtNameBuff->SetBase( 1 );

    pOutlineListBuffer = new XclImpOutlineListBuffer( );

    // ab Biff8
    pFormConv = pExcRoot->pFmlaConverter = new ExcelToSc( aIn );

    bTabTruncated = FALSE;

    // Excel-Dokument per Default auf 31.12.1899, entspricht Excel-Einstellungen mit 1.1.1900
    ScDocOptions aOpt = pD->GetDocOptions();
    aOpt.SetDate( 30, 12, 1899 );
    pD->SetDocOptions( aOpt );
    pD->GetFormatTable()->ChangeNullDate( 30, 12, 1899 );

    ScDocOptions aDocOpt( pD->GetDocOptions() );
    aDocOpt.SetIgnoreCase( TRUE );              // always in Excel
    aDocOpt.SetFormulaRegexEnabled( FALSE );    // regular expressions? what's that?
    aDocOpt.SetLookUpColRowNames( FALSE );      // default: no natural language refs
    pD->SetDocOptions( aDocOpt );
}


ImportExcel::~ImportExcel( void )
{
    GetDoc().SetSrcCharSet( GetCharSet() );

    delete pExtNameBuff;

    delete pOutlineListBuffer;

    delete pFormConv;
}


void ImportExcel::Dimensions( void )
{
    XclRange aXclUsedArea( ScAddress::UNINITIALIZED );
    maStrm >> aXclUsedArea;
    if( (aXclUsedArea.GetColCount() > 1) && (aXclUsedArea.GetRowCount() > 1) )
    {
        // Excel stores first unused row/column index
        --aXclUsedArea.maLast.mnCol;
        --aXclUsedArea.maLast.mnRow;
        // create the Calc range
        SCTAB nScTab = GetCurrScTab();
        ScRange& rScUsedArea = GetExtDocOptions().GetOrCreateTabSettings( nScTab ).maUsedArea;
        GetAddressConverter().ConvertRange( rScUsedArea, aXclUsedArea, nScTab, nScTab, false );
        // if any error occurs in ConvertRange(), rScUsedArea keeps untouched
    }
}


void ImportExcel::Window1()
{
    sal_uInt16 nFlags, nTabBarSize;
    maStrm.Ignore( 8 );
    maStrm >> nFlags;
    maStrm.Ignore( 6 );
    maStrm >> nTabBarSize;

    ScViewOptions aViewOpt( GetDoc().GetViewOptions() );
    aViewOpt.SetOption( VOPT_HSCROLL,       ::get_flag( nFlags, EXC_WIN1_HOR_SCROLLBAR ) );
    aViewOpt.SetOption( VOPT_VSCROLL,       ::get_flag( nFlags, EXC_WIN1_VER_SCROLLBAR ) );
    aViewOpt.SetOption( VOPT_TABCONTROLS,   ::get_flag( nFlags, EXC_WIN1_TABBAR ) );
    GetDoc().SetViewOptions( aViewOpt );

    if( nTabBarSize <= 1000 )
        GetExtDocOptions().GetDocSettings().mfTabBarWidth = static_cast< double >( nTabBarSize ) / 1000.0;
}


void ImportExcel::Blank25( void )
{
    UINT16 nRow, nCol, nXF;

    aIn >> nRow >> nCol;

    if( GetBiff() == EXC_BIFF2 )
    {
        sal_uInt8 nXFData;
        aIn >> nXFData;
        nXF = nXFData & 0x3F;
        if( nXF == 63 ) nXF = nIxfeIndex;
    }
    else
        aIn >> nXF;

    SCCOL nScCol = static_cast< SCCOL >( nCol );
    SCROW nScRow = static_cast< SCROW >( nRow );

    if( (nScRow <= MAXROW) && (nScCol <= MAXCOL) )
    {
        pColRowBuff->Used( nScCol, nScRow );
        GetXFRangeBuffer().SetBlankXF( nScCol, nScRow, nXF );
    }
    else
    {
        bTabTruncated = TRUE;
        GetTracer().TraceInvalidRow(GetCurrScTab(), nRow, MAXROW);
    }

    pLastFormCell = NULL;
}


void ImportExcel::Integer( void )
{
    UINT16 nRow, nCol, nInt;
    sal_uInt8 nXFData;

    aIn >> nRow >> nCol >> nXFData;
    aIn.Ignore( 2 );
    aIn >> nInt;

    SCCOL nScCol = static_cast< SCCOL >( nCol );
    SCROW nScRow = static_cast< SCROW >( nRow );

    if( (nScRow <= MAXROW) && (nScCol <= MAXCOL) )
    {
        sal_uInt16 nXF = nXFData & 0x3F;
        if( nXF == 63 ) nXF = nIxfeIndex;
        GetXFRangeBuffer().SetXF( nScCol, nScRow, nXF );

        ScBaseCell* pCell = new ScValueCell( nInt );
        GetDoc().PutCell( nScCol, nScRow, GetCurrScTab(), pCell );

        pColRowBuff->Used( nScCol, nScRow );
    }
    else
    {
        bTabTruncated = TRUE;
        GetTracer().TraceInvalidRow(GetCurrScTab(), nRow, MAXROW);
    }

    pLastFormCell = NULL;
}


void ImportExcel::Number25( void )
{
    UINT16 nRow, nCol, nXF;
    double fValue;

    aIn >> nRow >> nCol;

    if( GetBiff() == EXC_BIFF2 )
    {
        sal_uInt8 nXFData;
        aIn >> nXFData;
        aIn.Ignore( 2 );
        nXF = nXFData & 0x3F;
        if( nXF == 63 ) nXF = nIxfeIndex;
    }
    else
        aIn >> nXF;

    aIn >> fValue;

    SCCOL nScCol = static_cast< SCCOL >( nCol );
    SCROW nScRow = static_cast< SCROW >( nRow );

    if( (nScRow <= MAXROW) && (nScCol <= MAXCOL) )
    {
        GetXFRangeBuffer().SetXF( nScCol, nScRow, nXF );

        ScBaseCell* pCell = new ScValueCell( fValue );
        GetDoc().PutCell( nScCol, nScRow, GetCurrScTab(), pCell );

        pColRowBuff->Used( nScCol, nScRow );
    }
    else
    {
        bTabTruncated = TRUE;
        GetTracer().TraceInvalidRow(GetCurrScTab(), nRow, MAXROW);
    }

    pLastFormCell = NULL;
}


void ImportExcel::Boolerr25( void )
{
    UINT16  nRow, nCol, nXF;
    BYTE    bErrOrVal, nError;

    if( GetBiff() == EXC_BIFF2 )
    {// nur fuer BIFF2
        BYTE nAttr0, nAttr1, nAttr2;

        aIn >> nRow >> nCol >> nAttr0 >> nAttr1 >> nAttr2;

        nXF = nAttr0 & 0x3F;
        if( nXF == 63 )
            // IXFE-record stand davor
            nXF = nIxfeIndex;
    }
    else
        aIn >> nRow >> nCol >> nXF;

    SCCOL nScCol = static_cast< SCCOL >( nCol );
    SCROW nScRow = static_cast< SCROW >( nRow );

    if( (nScRow <= MAXROW) && (nScCol <= MAXCOL) )
    {
        double              fVal;
        const ScTokenArray  *pErgebnis;

        aIn >> nError >> bErrOrVal;

        // Simulation ueber Formelzelle!
        pErgebnis = ErrorToFormula( bErrOrVal, nError, fVal );
        ScAddress aPos( nScCol, nScRow, GetCurrScTab());
        ScFormulaCell* pCell = new ScFormulaCell( pD, aPos, pErgebnis );
        pCell->SetDouble( fVal );
        GetDoc().PutCell( aPos, pCell );

        pColRowBuff->Used( nScCol, nScRow );

        if( bErrOrVal )     // !=0 -> Error
            GetXFRangeBuffer().SetXF( nScCol, nScRow, nXF );
        else                // ==0 -> Boolean
            GetXFRangeBuffer().SetBoolXF( nScCol, nScRow, nXF );
    }
    else
    {
        bTabTruncated = TRUE;
        GetTracer().TraceInvalidRow(GetCurrScTab(), nRow, MAXROW);
    }

    pLastFormCell = NULL;
}


void ImportExcel::RecString( void )
{
    if( pLastFormCell )
    {
        pLastFormCell->SetString( aIn.ReadByteString( GetBiff() != Biff2 ) );

        pLastFormCell = NULL;
    }
}


void ImportExcel::Row25( void )
{
    UINT16  nRow, nRowHeight;

    aIn >> nRow;
    aIn.Ignore( 4 );    // Mic und Mac ueberspringen

    if( nRow <= MAXROW )
    {
        aIn >> nRowHeight;  // direkt in Twips angegeben
        aIn.Ignore( 2 );

        if( GetBiff() == EXC_BIFF2 )
        {// -------------------- BIFF2
            pColRowBuff->SetHeight( nRow, nRowHeight );
        }
        else
        {// -------------------- BIFF5
            UINT16  nGrbit;

            aIn.Ignore( 2 );    // reserved
            aIn >> nGrbit;

            BYTE nLevel = 0;
            ::extract_value( nLevel, nGrbit, 0, 3 );
            pRowOutlineBuff->SetLevel( nRow, nLevel,
                ::get_flag( nGrbit, EXC_ROW_COLLAPSED ), ::get_flag( nGrbit, EXC_ROW_HIDDEN ) );

            pColRowBuff->SetRowSettings( nRow, nRowHeight, nGrbit );
        }
    }
}


void ImportExcel::Bof2( void )
{
    sal_uInt16 nSubType;
    maStrm.DisableDecryption();
    maStrm.Ignore( 2 );
    maStrm >> nSubType;

    if( nSubType == 0x0010 )        // Worksheet?
        pExcRoot->eDateiTyp = Biff2;
    else if( nSubType == 0x0020 )   // Chart?
        pExcRoot->eDateiTyp = Biff2C;
    else if( nSubType == 0x0040 )   // Macro?
        pExcRoot->eDateiTyp = Biff2M;
    else
        pExcRoot->eDateiTyp = BiffX;
}


void ImportExcel::Eof( void )
{
    //  POST: darf nur nach einer GUELTIGEN Tabelle gerufen werden!
    EndSheet();
    IncCurrScTab();
}


BOOL ImportExcel::Password( void )
{
    // POST: return = TRUE, wenn Password <> 0
    UINT16 nPasswd;

    aIn >> nPasswd;

    return nPasswd != 0x0000;
}


void ImportExcel::Externsheet( void )
{
    String aUrl, aTabName;
    bool bSameWorkBook;
    String aEncodedUrl( aIn.ReadByteString( false ) );
    XclImpUrlHelper::DecodeUrl( aUrl, aTabName, bSameWorkBook, *pExcRoot->pIR, aEncodedUrl );
    ScfTools::ConvertToScSheetName( aTabName );
    pExcRoot->pExtSheetBuff->Add( aUrl, aTabName, bSameWorkBook );
}


void ImportExcel::Note( void )
{
    UINT16  nCol, nRow;
    aIn >> nRow >> nCol;

    if( nRow <= MAXROW && nCol <= MAXCOL )
        pD->SetNote( static_cast<SCCOL>(nCol), static_cast<SCROW>(nRow),
                GetCurrScTab(), ScPostIt( aIn.ReadByteString( TRUE ), pD ) );
    else
    {
        bTabTruncated = TRUE;
            GetTracer().TraceInvalidRow(GetCurrScTab(), nRow, MAXROW);
    }

    pLastFormCell = NULL;
}


void ImportExcel::Columndefault( void )
{// Default Cell Attributes
    UINT16  nColMic, nColMac;
    BYTE    nOpt0;

    aIn >> nColMic >> nColMac;

    DBG_ASSERT( aIn.GetRecLeft() == (ULONG)(nColMac - nColMic) * 3 + 2,
                "ImportExcel::Columndefault - wrong record size" );

    nColMac--;

    if( nColMac > MAXCOL )
        nColMac = static_cast<UINT16>(MAXCOL);

    for( UINT16 nCol = nColMic ; nCol <= nColMac ; nCol++ )
    {
        aIn >> nOpt0;
        aIn.Ignore( 2 );    // nur 0. Attribut-Byte benutzt

        if( nOpt0 & 0x80 )  // Col hidden?
            pColRowBuff->HideCol( nCol );
    }
}


void ImportExcel::Array25( void )
{
    UINT16      nFirstRow, nLastRow, nFormLen;
    BYTE        nFirstCol, nLastCol;

    aIn >> nFirstRow >> nLastRow >> nFirstCol >> nLastCol;

    if( GetBiff() == EXC_BIFF2 )
    {//                     BIFF2
        aIn.Ignore( 1 );
        nFormLen = aIn.ReaduInt8();
    }
    else
    {//                     BIFF5
        aIn.Ignore( 6 );
        aIn >> nFormLen;
    }

    if( nLastRow <= MAXROW && nLastCol <= MAXCOL )
    {
        // jetzt steht Lesemarke auf Formel, Laenge in nFormLen
        const ScTokenArray* pErgebnis;

        pFormConv->Reset( ScAddress( static_cast<SCCOL>(nFirstCol),
                    static_cast<SCROW>(nFirstRow), GetCurrScTab() ) );
        pFormConv->Convert( pErgebnis, nFormLen );

        DBG_ASSERT( pErgebnis, "*ImportExcel::Array25(): ScTokenArray ist NULL!" );

        ScMarkData          aMarkData;
        aMarkData.SelectOneTable( GetCurrScTab() );
        pD->InsertMatrixFormula( static_cast<SCCOL>(nFirstCol),
                static_cast<SCROW>(nFirstRow), static_cast<SCCOL>(nLastCol),
                static_cast<SCROW>(nLastRow), aMarkData, EMPTY_STRING,
                pErgebnis );
    }
}


void ImportExcel::Rec1904( void )
{
    UINT16  n1904;

    aIn >> n1904;

    if( n1904 )
    {// 1904 date system
        ScDocOptions aOpt = pD->GetDocOptions();
        aOpt.SetDate( 1, 1, 1904 );
        pD->SetDocOptions( aOpt );
        pD->GetFormatTable()->ChangeNullDate( 1, 1, 1904 );
    }
}


void ImportExcel::Externname25( void )
{
    UINT32      nRes;
    UINT16      nOpt;
    UINT16      nLenExpr = 0;

    aIn >> nOpt >> nRes;

    String aName( aIn.ReadByteString( FALSE ) );

    if( ( nOpt & 0x0001 ) || ( ( nOpt & 0xFFFE ) == 0x0000 ) )
    {// external name
        ScfTools::ConvertToScDefinedName( aName );
        pExcRoot->pExtNameBuff->AddName( aName );
    }
    else if( nOpt & 0x0010 )
    {// ole link
        pExcRoot->pExtNameBuff->AddOLE( aName, nRes );      // nRes is storage ID
    }
    else
    {// dde link
        pExcRoot->pExtNameBuff->AddDDE( aName );
    }
}


void ImportExcel::Colwidth( void )
{// Column Width
    BYTE    nColFirst, nColLast;
    UINT16  nColWidth;

    aIn >> nColFirst >> nColLast >> nColWidth;

    if( nColLast > MAXCOL )
        nColLast = static_cast<UINT16>(MAXCOL);

    USHORT nScWidth = XclTools::GetScColumnWidth( nColWidth, GetCharWidth() );
    pColRowBuff->SetWidthRange( nColFirst, nColLast, nScWidth );
}


void ImportExcel::Defrowheight2( void )
{
    UINT16  nDef;
    aIn >> nDef;
    nDef &=0x7FFF;
    pColRowBuff->SetDefHeight( nDef );
}


void ImportExcel::Protect( void )
{
    if( aIn.ReaduInt16() )
    {
        uno::Sequence<sal_Int8> aEmptyPass;
        GetDoc().SetTabProtection( GetCurrScTab(), TRUE, aEmptyPass );
    }
}

void ImportExcel::DocProtect( void )
{
    if( aIn.ReaduInt16() )
    {
        uno::Sequence<sal_Int8> aEmptyPass;
        GetDoc().SetDocProtection( TRUE, aEmptyPass );
    }
}


void ImportExcel::Codepage( void )
{
    rtl_TextEncoding eEnc = XclTools::GetTextEncoding( maStrm.ReaduInt16() );
    if( eEnc != RTL_TEXTENCODING_DONTKNOW )
        SetCharSet( eEnc );
}


void ImportExcel::Ixfe( void )
{
    aIn >> nIxfeIndex;
}


void ImportExcel::DefColWidth( void )
{
    // stored as entire characters -> convert to 1/256 of characters (as in COLINFO)
    double fDefWidth = 256.0 * maStrm.ReaduInt16();

    // #i3006# additional space for default width - Excel adds space depending on font size
    long nFontHt = GetFontBuffer().GetAppFontData().mnHeight;
    fDefWidth += XclTools::GetXclDefColWidthCorrection( nFontHt );

    USHORT nScWidth = XclTools::GetScColumnWidth( limit_cast< sal_uInt16 >( fDefWidth ), GetCharWidth() );
    pColRowBuff->SetDefWidth( nScWidth );
}


void ImportExcel::Builtinfmtcnt( void )
{
}


void ImportExcel::Colinfo( void )
{// Column Formatting Information
    UINT16  nColFirst, nColLast, nColWidth, nXF;
    UINT16  nOpt;

    aIn >> nColFirst >> nColLast >> nColWidth >> nXF >> nOpt;

    if( nColFirst > MAXCOL )
        return;

    if( nColLast > MAXCOL )
        nColLast = static_cast<UINT16>(MAXCOL);

    bool bHidden = ::get_flag( nOpt, EXC_COLINFO_HIDDEN );
    bool bCollapsed = ::get_flag( nOpt, EXC_COLINFO_COLLAPSED );
    sal_uInt8 nLevel;
    ::extract_value( nLevel, nOpt, 8, 3 );
    pColOutlineBuff->SetLevelRange( nColFirst, nColLast, nLevel, bCollapsed, bHidden );

    if( bHidden )
        pColRowBuff->HideColRange( nColFirst, nColLast );

    USHORT nScWidth = XclTools::GetScColumnWidth( nColWidth, GetCharWidth() );
    pColRowBuff->SetWidthRange( nColFirst, nColLast, nScWidth );
    pColRowBuff->SetDefaultXF( nColFirst, nColLast, nXF );
}


void ImportExcel::Rk( void )
{// Cell Value, RK Number
    UINT16              nRow, nCol, nXF;
    INT32               nRkNum;

    aIn >> nRow >> nCol >> nXF >> nRkNum;

    SCCOL nScCol = static_cast< SCCOL >( nCol );
    SCROW nScRow = static_cast< SCROW >( nRow );

    if( (nScRow <= MAXROW) && (nScCol <= MAXCOL) )
    {
        GetXFRangeBuffer().SetXF( nScCol, nScRow, nXF );

        ScBaseCell* pCell = new ScValueCell( XclTools::GetDoubleFromRK( nRkNum ) );
        GetDoc().PutCell( nScCol, nScRow, GetCurrScTab(), pCell );

        pColRowBuff->Used( nScCol, nScRow );
    }
    else
    {
        bTabTruncated = TRUE;
        GetTracer().TraceInvalidRow(GetCurrScTab(), nRow, MAXROW);
    }

    pLastFormCell = NULL;
}


void ImportExcel::Wsbool( void )
{
    UINT16 nFlags;
    aIn >> nFlags;

    pRowOutlineBuff->SetButtonMode( ::get_flag( nFlags, EXC_WSBOOL_ROWBELOW ) );
    pColOutlineBuff->SetButtonMode( ::get_flag( nFlags, EXC_WSBOOL_COLBELOW ) );

    GetPageSettings().SetFitToPages( ::get_flag( nFlags, EXC_WSBOOL_FITTOPAGE ) );
}


void ImportExcel::Boundsheet( void )
{
    UINT16      nGrbit;

    if( GetBiff() == EXC_BIFF5 )
    {
        aIn.Ignore( 4 );
        aIn >> nGrbit;
    }
    else
        nGrbit = 0x0000;

    String aName( aIn.ReadByteString( FALSE ) );
    ScfTools::ConvertToScSheetName( aName );

    *pExcRoot->pTabNameBuff << aName;

    const SCTAB nTab = static_cast<SCTAB>(nBdshtTab);
    if( nTab > 0 )
    {
        DBG_ASSERT( !pD->HasTable( nTab ), "*ImportExcel::Boundsheet(): Tabelle schon vorhanden!" );

        pD->MakeTable( nTab );
    }

    if( ( nGrbit & 0x0001 ) || ( nGrbit & 0x0002 ) )
        pD->SetVisible( nTab, FALSE );

    pD->RenameTab( nTab, aName );
    nBdshtTab++;
}


void ImportExcel::Country( void )
{
    sal_uInt16 nUICountry, nDocCountry;
    maStrm >> nUICountry >> nDocCountry;

    // Store system language in XclRoot
    LanguageType eLanguage = ::svx::ConvertCountryToLanguage( static_cast< ::svx::CountryId >( nDocCountry ) );
    if( eLanguage != LANGUAGE_DONTKNOW )
        SetDocLanguage( eLanguage );

    // Set Excel UI language in add-in name translator
    eLanguage = ::svx::ConvertCountryToLanguage( static_cast< ::svx::CountryId >( nUICountry ) );
    if( eLanguage != LANGUAGE_DONTKNOW )
        SetUILanguage( eLanguage );
}


void ImportExcel::ReadUsesElfs()
{
    if( maStrm.ReaduInt16() != 0 )
    {
        ScDocOptions aDocOpt = GetDoc().GetDocOptions();
        aDocOpt.SetLookUpColRowNames( TRUE );
        GetDoc().SetDocOptions( aDocOpt );
    }
}


void ImportExcel::Hideobj( void )
{
    UINT16      nHide;
    ScVObjMode  eOle, eChart, eDraw;

    aIn >> nHide;

    ScViewOptions aOpts( pD->GetViewOptions() );

    switch( nHide )
    {
        case 1:                         // Placeholders
            eOle   = VOBJ_MODE_SHOW;    // in Excel 97 werden nur Charts als Platzhalter angezeigt
            eChart = VOBJ_MODE_DUMMY;
            eDraw  = VOBJ_MODE_SHOW;
            break;
        case 2:                         // Hide all
            eOle   = VOBJ_MODE_HIDE;
            eChart = VOBJ_MODE_HIDE;
            eDraw  = VOBJ_MODE_HIDE;
            break;
        default:                        // Show all
            eOle   = VOBJ_MODE_SHOW;
            eChart = VOBJ_MODE_SHOW;
            eDraw  = VOBJ_MODE_SHOW;
            break;
    }

    aOpts.SetObjMode( VOBJ_TYPE_OLE,   eOle );
    aOpts.SetObjMode( VOBJ_TYPE_CHART, eChart );
    aOpts.SetObjMode( VOBJ_TYPE_DRAW,  eDraw );

    pD->SetViewOptions( aOpts );
}


void ImportExcel::Bundleheader( void )
{
}


void ImportExcel::Standardwidth( void )
{
    USHORT nScWidth = XclTools::GetScColumnWidth( maStrm.ReaduInt16(), GetCharWidth() );
    pColRowBuff->SetDefWidth( nScWidth, TRUE );
}


void ImportExcel::Shrfmla( void )
{
    UINT16              nFirstRow, nLastRow, nLenExpr;
    BYTE                nFirstCol, nLastCol;

    aIn >> nFirstRow >> nLastRow >> nFirstCol >> nLastCol;
    aIn.Ignore( 2 );
    aIn >> nLenExpr;

    // jetzt steht Lesemarke an der Formel

    const ScTokenArray* pErgebnis;

    pFormConv->Reset();
    pFormConv->Convert( pErgebnis, nLenExpr, FT_SharedFormula );


    DBG_ASSERT( pErgebnis, "+ImportExcel::Shrfmla(): ScTokenArray ist NULL!" );

    pExcRoot->pShrfmlaBuff->Store( ScRange( static_cast<SCCOL>(nFirstCol),
                static_cast<SCROW>(nFirstRow), GetCurrScTab(),
                static_cast<SCCOL>(nLastCol), static_cast<SCROW>(nLastRow),
                GetCurrScTab()), *pErgebnis );

    pLastFormCell = NULL;
}


void ImportExcel::Mulrk( void )
{
    UINT16  nRow, nColFirst, nXF;
    INT32   nRkNum;

    aIn >> nRow >> nColFirst;

    SCCOL nFirstScCol = static_cast< SCCOL >( nColFirst );
    SCROW nScRow = static_cast< SCROW >( nRow );

    if( nScRow <= MAXROW )
    {
        for( SCCOL nScCol = nFirstScCol ; aIn.GetRecLeft() > 2 ; ++nScCol )
        {
            aIn >> nXF >> nRkNum;

            if( nScCol <= MAXCOL )
            {
                GetXFRangeBuffer().SetXF( nScCol, nScRow, nXF );

                ScBaseCell* pCell = new ScValueCell( XclTools::GetDoubleFromRK( nRkNum ) );
                GetDoc().PutCell( nScCol, nScRow, GetCurrScTab(), pCell );

                pColRowBuff->Used( nScCol, nScRow );
            }
        }
        DBG_ASSERT( aIn.GetRecLeft() == 2, "+ImportExcel::Mulrk(): Was'n das?!!!" );
    }
    else
    {
        bTabTruncated = TRUE;
        GetTracer().TraceInvalidRow(GetCurrScTab(), nRow, MAXROW);
    }

    pLastFormCell = NULL;
}


void ImportExcel::Mulblank( void )
{
    UINT16  nRow, nColFirst, nXF;

    aIn >> nRow >> nColFirst;

    SCCOL nFirstScCol = static_cast< SCCOL >( nColFirst );
    SCROW nScRow = static_cast< SCROW >( nRow );

    if( nScRow <= MAXROW )
    {
        for( SCCOL nScCol = nFirstScCol; aIn.GetRecLeft() > 2; ++nScCol )
        {
            aIn >> nXF;

            if( nScCol <= MAXCOL )
            {
                pColRowBuff->Used( nScCol, nScRow );
                GetXFRangeBuffer().SetBlankXF( nScCol, nScRow, nXF );
            }
        }
        aIn >> nRow;    // nRow zum Testen von letzter Col missbraucht
    }
    else
    {
        bTabTruncated = TRUE;
        GetTracer().TraceInvalidRow(GetCurrScTab(), nRow, MAXROW);
    }

    pLastFormCell = NULL;
}


void ImportExcel::Rstring( void )
{
    UINT16 nRow, nCol, nXF;
    aIn >> nRow >> nCol >> nXF;

    SCCOL nScCol = static_cast< SCCOL >( nCol );
    SCROW nScRow = static_cast< SCROW >( nRow );

    if( (nScRow <= MAXROW) && (nScCol <= MAXCOL) )
    {
        GetXFRangeBuffer().SetXF( nScCol, nScRow, nXF );

        // unformatted Unicode string with separate formatting information
        XclImpString aString( maStrm );
        if( !aString.IsRich() )
            aString.ReadFormats( maStrm );

        ScBaseCell* pCell = XclImpStringHelper::CreateCell( *this, aString, nXF );
        if( pCell )
            GetDoc().PutCell( nScCol, nScRow, GetCurrScTab(), pCell );

        pColRowBuff->Used( nScCol, nScRow );
    }
    else
    {
        bTabTruncated = TRUE;
        GetTracer().TraceInvalidRow(GetCurrScTab(), nRow, MAXROW);
    }

    pLastFormCell = NULL;
}


void ImportExcel::Olesize( void )
{
    XclRange aXclOleSize( ScAddress::UNINITIALIZED );
    maStrm.Ignore( 2 );
    aXclOleSize.Read( maStrm, false );

    SCTAB nScTab = GetCurrScTab();
    ScRange& rOleSize = GetExtDocOptions().GetDocSettings().maOleSize;
    GetAddressConverter().ConvertRange( rOleSize, aXclOleSize, nScTab, nScTab, false );
}


void ImportExcel::Blank34( void )
{
    UINT16  nRow, nCol, nXF;

    aIn >> nRow >> nCol >> nXF;

    SCCOL nScCol = static_cast< SCCOL >( nCol );
    SCROW nScRow = static_cast< SCROW >( nRow );

    if( (nScRow <= MAXROW) && (nScCol <= MAXCOL) )
    {
        pColRowBuff->Used( nScCol, nScRow );
        GetXFRangeBuffer().SetBlankXF( nScCol, nScRow, nXF );
    }
    else
    {
        bTabTruncated = TRUE;
        GetTracer().TraceInvalidRow(GetCurrScTab(), nRow, MAXROW);
    }

    pLastFormCell = NULL;
}


void ImportExcel::Number34( void )
{
    UINT16  nRow, nCol, nXF;
    double  fValue;

    aIn >> nRow >> nCol >> nXF >> fValue;

    SCCOL nScCol = static_cast< SCCOL >( nCol );
    SCROW nScRow = static_cast< SCROW >( nRow );

    if( (nScRow <= MAXROW) && (nScCol <= MAXCOL) )
    {
        GetXFRangeBuffer().SetXF( nScCol, nScRow, nXF );

        ScBaseCell* pCell = new ScValueCell( fValue );
        GetDoc().PutCell( nScCol, nScRow, GetCurrScTab(), pCell );

        pColRowBuff->Used( nScCol, nScRow );
    }
    else
    {
        bTabTruncated = TRUE;
        GetTracer().TraceInvalidRow(GetCurrScTab(), nRow, MAXROW);
    }

    pLastFormCell = NULL;
}


void ImportExcel::Label( void )
{
    sal_uInt16 nRow, nCol, nXFIndex;
    maStrm >> nRow >> nCol;

    if( GetBiff() == EXC_BIFF2 )
    {
        sal_uInt8 nAttr0, nAttr1, nAttr2;
        maStrm >> nAttr0 >> nAttr1 >> nAttr2;

        nXFIndex = nAttr0 & 0x3F;
        if( nXFIndex == 63 )
            nXFIndex = nIxfeIndex;
    }
    else
        maStrm >> nXFIndex;

    SCCOL nScCol = static_cast< SCCOL >( nCol );
    SCROW nScRow = static_cast< SCROW >( nRow );

    if( (nScRow <= MAXROW) && (nScCol <= MAXCOL) )
    {
        GetXFRangeBuffer().SetXF( nScCol, nScRow, nXFIndex );
        pColRowBuff->Used( nScCol, nScRow );

        XclStrFlags nFlags = (GetBiff() == EXC_BIFF2) ? EXC_STR_8BITLENGTH : EXC_STR_DEFAULT;
        XclImpString aString( maStrm, nFlags );
        if( ScBaseCell* pCell = XclImpStringHelper::CreateCell( GetRoot(), aString, nXFIndex ) )
            GetDoc().PutCell( nScCol, nScRow, GetCurrScTab(), pCell );
    }
    else
    {
        bTabTruncated = TRUE;
        GetTracer().TraceInvalidRow(GetCurrScTab(), nRow, MAXROW);
    }

    pLastFormCell = NULL;
}


void ImportExcel::Boolerr34( void )
{
    UINT16                  nRow, nCol, nXF;
    BYTE                    bErrOrVal, nError;

    aIn >> nRow >> nCol >> nXF;

    SCCOL nScCol = static_cast< SCCOL >( nCol );
    SCROW nScRow = static_cast< SCROW >( nRow );

    if( (nScRow <= MAXROW) && (nScCol <= MAXCOL) )
    {
        double              fVal;
        const ScTokenArray* pErgebnis;

        aIn >> nError >> bErrOrVal;

        // Simulation ueber Formelzelle!
        pErgebnis = ErrorToFormula( bErrOrVal, nError, fVal );

        ScAddress aPos( nScCol, nScRow, GetCurrScTab() );

        ScFormulaCell*      pZelle = new ScFormulaCell( pD, aPos, pErgebnis );

        pZelle->SetDouble( fVal );

        GetDoc().PutCell( aPos, pZelle );
        pColRowBuff->Used( nScCol, nScRow );

        if( bErrOrVal )     // !=0 -> Error
            GetXFRangeBuffer().SetXF( nScCol, nScRow, nXF );
        else                // ==0 -> Boolean
            GetXFRangeBuffer().SetBoolXF( nScCol, nScRow, nXF );
    }
    else
    {
        bTabTruncated = TRUE;
        GetTracer().TraceInvalidRow(GetCurrScTab(), nRow, MAXROW);
    }

    pLastFormCell = NULL;
}


void ImportExcel::Row34( void )
{
    UINT16  nRow, nRowHeight, nGrbit, nXF;

    aIn >> nRow;
    aIn.Ignore( 4 );    // Mic und Mac ueberspringen

    SCROW nScRow = static_cast< SCROW >( nRow );

    if( nScRow <= MAXROW )
    {
        aIn >> nRowHeight;  // direkt in Twips angegeben
        aIn.Ignore( 4 );

        aIn >> nGrbit >> nXF;

        BYTE nLevel = 0;
        ::extract_value( nLevel, nGrbit, 0, 3 );
        pRowOutlineBuff->SetLevel( nScRow, nLevel,
            ::get_flag( nGrbit, EXC_ROW_COLLAPSED ), ::get_flag( nGrbit, EXC_ROW_HIDDEN ) );

        pColRowBuff->SetRowSettings( nScRow, nRowHeight, nGrbit );

        if( nGrbit & EXC_ROW_USEDEFXF )
            GetXFRangeBuffer().SetRowDefXF( nScRow, nXF & EXC_ROW_XFMASK );
    }
}


void ImportExcel::Bof3( void )
{
    sal_uInt16 nSubType;
    maStrm.DisableDecryption();
    maStrm.Ignore( 2 );
    maStrm >> nSubType;

    DBG_ASSERT( nSubType != 0x0100, "*ImportExcel::Bof3(): Biff3 als Workbook?!" );
    if( nSubType == 0x0010 )        // Sheet?
        pExcRoot->eDateiTyp = Biff3;
    else if( nSubType == 0x0100 )   // Book?
        pExcRoot->eDateiTyp = Biff3W;
    else if( nSubType == 0x0020 )   // Chart?
        pExcRoot->eDateiTyp = Biff3C;
    else if( nSubType == 0x0040 )   // Macro?
        pExcRoot->eDateiTyp = Biff3M;
    else
        pExcRoot->eDateiTyp = BiffX;
}


void ImportExcel::Array34( void )
{
    UINT16                  nFirstRow, nLastRow, nFormLen;
    BYTE                    nFirstCol, nLastCol;

    aIn >> nFirstRow >> nLastRow >> nFirstCol >> nLastCol;
    aIn.Ignore( (GetBiff() >= EXC_BIFF5) ? 6 : 2 );
    aIn >> nFormLen;

    if( nLastRow <= MAXROW && nLastCol <= MAXCOL )
    {
        // jetzt steht Lesemarke auf Formel, Laenge in nFormLen
        const ScTokenArray* pErgebnis;

        pFormConv->Reset( ScAddress( static_cast<SCCOL>(nFirstCol),
                    static_cast<SCROW>(nFirstRow), GetCurrScTab() ) );
        pFormConv->Convert( pErgebnis, nFormLen );

        DBG_ASSERT( pErgebnis, "+ImportExcel::Array34(): ScTokenArray ist NULL!" );

        ScMarkData          aMarkData;
        aMarkData.SelectOneTable( GetCurrScTab() );
        pD->InsertMatrixFormula( static_cast<SCCOL>(nFirstCol),
                static_cast<SCROW>(nFirstRow), static_cast<SCCOL>(nLastCol),
                static_cast<SCROW>(nLastRow), aMarkData, EMPTY_STRING,
                pErgebnis);
    }

    pLastFormCell = NULL;
}


void ImportExcel::Externname34( void )
{
}


void ImportExcel::Defrowheight345( void )
{
    UINT16  nDef, nOpt;

    aIn >> nOpt >> nDef;

    if( nOpt & 0x0002 )
        pColRowBuff->SetDefHeight( 0 );
    else
        pColRowBuff->SetDefHeight( nDef );
}


void ImportExcel::TableOp( void )
{
    UINT16 nFirstRow, nLastRow;
    UINT8 nFirstCol, nLastCol;
    UINT16 nGrbit;
    UINT16 nInpRow, nInpCol, nInpRow2, nInpCol2;

    aIn >> nFirstRow >> nLastRow >> nFirstCol >> nLastCol >> nGrbit
        >> nInpRow >> nInpCol >> nInpRow2 >> nInpCol2;

    if( (nLastRow <= MAXROW) && (nLastCol <= MAXCOL) )
    {
        if( nFirstCol && nFirstRow )
        {
            ScTabOpParam aTabOpParam;
            aTabOpParam.nMode = (nGrbit & EXC_TABLEOP_BOTH) ? 2 : ((nGrbit & EXC_TABLEOP_ROW) ? 1 : 0 );
            USHORT nCol = nFirstCol - 1;
            USHORT nRow = nFirstRow - 1;
            SCTAB nTab = GetCurrScTab();
            switch( aTabOpParam.nMode )
            {
                case 0:     // COL
                    aTabOpParam.aRefFormulaCell.Set(
                            static_cast<SCCOL>(nFirstCol),
                            static_cast<SCROW>(nFirstRow - 1), nTab, FALSE,
                            FALSE, FALSE );
                    aTabOpParam.aRefFormulaEnd.Set(
                            static_cast<SCCOL>(nLastCol),
                            static_cast<SCROW>(nFirstRow - 1), nTab, FALSE,
                            FALSE, FALSE );
                    aTabOpParam.aRefColCell.Set( static_cast<SCCOL>(nInpCol),
                            static_cast<SCROW>(nInpRow), nTab, FALSE, FALSE,
                            FALSE );
                    nRow++;
                break;
                case 1:     // ROW
                    aTabOpParam.aRefFormulaCell.Set(
                            static_cast<SCCOL>(nFirstCol - 1),
                            static_cast<SCROW>(nFirstRow), nTab, FALSE, FALSE,
                            FALSE );
                    aTabOpParam.aRefFormulaEnd.Set(
                            static_cast<SCCOL>(nFirstCol - 1),
                            static_cast<SCROW>(nLastRow), nTab, FALSE, FALSE,
                            FALSE );
                    aTabOpParam.aRefRowCell.Set( static_cast<SCCOL>(nInpCol),
                            static_cast<SCROW>(nInpRow), nTab, FALSE, FALSE,
                            FALSE );
                    nCol++;
                break;
                case 2:     // TWO-INPUT
                    aTabOpParam.aRefFormulaCell.Set(
                            static_cast<SCCOL>(nFirstCol - 1),
                            static_cast<SCROW>(nFirstRow - 1), nTab, FALSE,
                            FALSE, FALSE );
                    aTabOpParam.aRefRowCell.Set( static_cast<SCCOL>(nInpCol),
                            static_cast<SCROW>(nInpRow), nTab, FALSE, FALSE,
                            FALSE );
                    aTabOpParam.aRefColCell.Set( static_cast<SCCOL>(nInpCol2),
                            static_cast<SCROW>(nInpRow2), nTab, FALSE, FALSE,
                            FALSE );
                break;
            }

            ScMarkData aMarkData;
            aMarkData.SelectOneTable( nTab );
            pD->InsertTableOp( aTabOpParam, static_cast<SCCOL>(nCol),
                    static_cast<SCROW>(nRow), static_cast<SCCOL>(nLastCol),
                    static_cast<SCROW>(nLastRow), aMarkData );
        }
    }
    else
        {
        bTabTruncated = TRUE;
            GetTracer().TraceInvalidRow(GetCurrScTab(), nLastRow, MAXROW);
        }

    pLastFormCell = NULL;
}


void ImportExcel::Bof4( void )
{
    sal_uInt16 nSubType;
    maStrm.DisableDecryption();
    maStrm.Ignore( 2 );
    maStrm >> nSubType;

    if( nSubType == 0x0010 )        // Sheet?
        pExcRoot->eDateiTyp = Biff4;
    else if( nSubType == 0x0100 )   // Book?
        pExcRoot->eDateiTyp = Biff4W;
    else if( nSubType == 0x0020 )   // Chart?
        pExcRoot->eDateiTyp = Biff4C;
    else if( nSubType == 0x0040 )   // Macro?
        pExcRoot->eDateiTyp = Biff4M;
    else
        pExcRoot->eDateiTyp = BiffX;
}


void ImportExcel::Bof5( void )
{
    //POST: eDateiTyp = Typ der zu lesenden Datei
    UINT16      nSubType, nVers;
    BiffTyp     eDatei;

    maStrm.DisableDecryption();
    maStrm >> nVers >> nSubType;

    switch( nSubType )
    {
        case 0x0005:    eDatei = Biff5W;    break;  // workbook globals
        case 0x0006:    eDatei = Biff5V;    break;  // VB module
        case 0x0010:    eDatei = Biff5;     break;  // worksheet
        case 0x0020:    eDatei = Biff5C;    break;  // chart
        case 0x0040:    eDatei = Biff5M4;   break;  // macro sheet
        default:
            pExcRoot->eDateiTyp = BiffX;
            return;
    }

    if( nVers == 0x0600 && (GetBiff() == EXC_BIFF8) )
        eDatei = ( BiffTyp ) ( eDatei - Biff5 + Biff8 );

    pExcRoot->eDateiTyp = eDatei;
}

void ImportExcel::EndSheet( void )
{
    pColRowBuff->Apply( GetCurrScTab() );

    pExcRoot->pExtSheetBuff->Reset();

    if( GetBiff() <= EXC_BIFF5 )
        pExcRoot->pExtNameBuff->Reset();

    FinalizeTable();
}


void ImportExcel::NeueTabelle( void )
{
    SCTAB nTab = GetCurrScTab();
    if( nTab > 0 && !pD->HasTable( nTab ) )
        pD->MakeTable( nTab );

    InitializeTable( nTab );

    pOutlineListBuffer->Append(new XclImpOutlineDataBuffer(*pExcRoot, nTab ));          //#94039# prevent empty rootdata

    pColRowBuff = pOutlineListBuffer->Last()->GetColRowBuff();
    pColOutlineBuff = pOutlineListBuffer->Last()->GetColOutline();
    pRowOutlineBuff = pOutlineListBuffer->Last()->GetRowOutline();
}


const ScTokenArray* ImportExcel::ErrorToFormula( BYTE bErrOrVal, BYTE nError, double& rVal )
{
    return pFormConv->GetBoolErr( XclTools::ErrorToEnum( rVal, bErrOrVal, nError ) );
}


void ImportExcel::AdjustRowHeight()
{
    // #93255# speed up chart import: import all sheets without charts, then
    // update row heights (here), last load all charts -> do not any longer
    // update inside of ScDocShell::ConvertFrom() (causes update of existing
    // charts during each and every change of row height)
    ScModelObj* pDocObj = GetDocModelObj();
    if( pDocObj )
    {
        for( SCTAB nTab = 0; nTab < GetDoc().GetTableCount(); ++nTab )
            pDocObj->AdjustRowHeight( 0, MAXROW, nTab );
    }
}


void ImportExcel::PostDocLoad( void )
{
    /*  Set automatic page numbering in Default page style (default is "page number = 1").
        Otherwise hidden tables (i.e. for scenarios) which have Default page style will
        break automatic page numbering. */
    if( SfxStyleSheetBase* pStyleSheet = GetStyleSheetPool().Find( ScGlobal::GetRscString( STR_STYLENAME_STANDARD ), SFX_STYLE_FAMILY_PAGE ) )
        pStyleSheet->GetItemSet().Put( SfxUInt16Item( ATTR_PAGE_FIRSTPAGENO, 0 ) );

    // Apply any Outlines for each sheet
    for(XclImpOutlineDataBuffer* pBuffer = pOutlineListBuffer->First(); pBuffer; pBuffer = pOutlineListBuffer->Next() )
        pBuffer->Apply(pD);


    // visible area if embedded OLE
    if( ScModelObj* pDocObj = GetDocModelObj() )
    {
        if( SfxObjectShell* pEmbObj = pDocObj->GetEmbeddedObject() )
        {
            // visible area if embedded
            const ScExtDocSettings& rDocSett = GetExtDocOptions().GetDocSettings();
            SCTAB nDisplScTab = rDocSett.mnDisplTab;

            // first try if there was an OLESIZE record
            ScRange aScOleSize = rDocSett.maOleSize;

            /*  #i44077# If a new OLE object is inserted from file, there
                is no OLESIZE record in the Excel file. Calculate used area
                from file contents (used cells and drawing objects). */
            if( !aScOleSize.IsValid() )
            {
                // used area of displayed sheet (cell contents)
                if( const ScExtTabSettings* pTabSett = GetExtDocOptions().GetTabSettings( nDisplScTab ) )
                    aScOleSize = pTabSett->maUsedArea;
                // add all valid drawing objects (object manager only available in BIFF8)
                if( GetBiff() == EXC_BIFF8 )
                    GetObjectManager().ExtendUsedArea( aScOleSize, nDisplScTab );
            }

            // valid size found - set it at the document
            if( aScOleSize.IsValid() )
            {
                pEmbObj->SetVisArea( GetDoc().GetMMRect(
                    aScOleSize.aStart.Col(), aScOleSize.aStart.Row(),
                    aScOleSize.aEnd.Col(), aScOleSize.aEnd.Row(), nDisplScTab ) );
                GetDoc().SetVisibleTab( nDisplScTab );
            }
        }

        // #111099# open forms in alive mode (has no effect, if no controls in document)
        pDocObj->setPropertyValue( CREATE_OUSTRING( SC_UNO_APPLYFMDES ), ::comphelper::makeBoolAny( sal_False ) );
    }

    // enables extended options to be set to the view after import
    GetExtDocOptions().SetChanged( true );

    // root data owns the extended document options -> create a new object
    GetDoc().SetExtDocOptions( new ScExtDocOptions( GetExtDocOptions() ) );

    EndAllChartObjects();

    GetDoc().UpdateAllCharts();

    const SCTAB     nLast = pD->GetTableCount();
    const ScRange*      p;

    if( pExcRoot->pPrintRanges->HasRanges() )
    {
        for( SCTAB n = 0 ; n < nLast ; n++ )
        {
            p = pExcRoot->pPrintRanges->First( static_cast<UINT16>(n) );
            if( p )
            {
                DBG_ASSERT( pExcRoot->pPrintRanges->GetActList(),
                            "-ImportExcel::PostDocLoad(): Imaginaere Tabelle gefunden!" );

                pD->ClearPrintRanges( n );
                while( p )
                {
                    pD->AddPrintRange( n, *p );
                    p = pExcRoot->pPrintRanges->Next();
                }
            }
            else
            {
                // #i4063# no print ranges -> print entire sheet
                pD->SetPrintEntireSheet( n );
            }
        }
        GetTracer().TracePrintRange();
    }

    if( pExcRoot->pPrintTitles->HasRanges() )
    {
        for( SCTAB n = 0 ; n < nLast ; n++ )
        {
            p = pExcRoot->pPrintTitles->First( static_cast<UINT16>(n) );
            if( p )
            {
                DBG_ASSERT( pExcRoot->pPrintTitles->GetActList(),
                    "-ImportExcel::PostDocLoad(): Imaginaere Tabelle gefunden!" );

                BOOL    bRowVirgin = TRUE;
                BOOL    bColVirgin = TRUE;

                while( p )
                {
                    if( p->aStart.Col() == 0 && p->aEnd.Col() == MAXCOL && bRowVirgin )
                    {
                        pD->SetRepeatRowRange( n, p );
                        bRowVirgin = FALSE;
                    }

                    if( p->aStart.Row() == 0 && p->aEnd.Row() == MAXROW && bColVirgin )
                    {
                        pD->SetRepeatColRange( n, p );
                        bColVirgin = FALSE;
                    }

                    p = pExcRoot->pPrintTitles->Next();
                }
            }
        }
    }
}

XclImpOutlineDataBuffer::XclImpOutlineDataBuffer(RootData& rRootData, SCTAB nTabNo) :
    nTab (nTabNo),
    pColOutlineBuff( new XclImpOutlineBuffer (MAXCOLCOUNT) ),
    pRowOutlineBuff( new XclImpOutlineBuffer (MAXROWCOUNT) ),
    pColRowBuff( new XclImpColRowSettings( rRootData ) )
{
    pColRowBuff->SetDefWidth( STD_COL_WIDTH );
    pColRowBuff->SetDefHeight( ( UINT16 ) STD_ROW_HEIGHT );
}

XclImpOutlineDataBuffer::~XclImpOutlineDataBuffer()
{
}

void XclImpOutlineDataBuffer::Apply(ScDocument* pD)
{
    pColOutlineBuff->SetOutlineArray( pD->GetOutlineTable( nTab, TRUE )->GetColArray() );
    pColOutlineBuff->MakeScOutline();

    pRowOutlineBuff->SetOutlineArray( pD->GetOutlineTable( nTab, TRUE )->GetRowArray() );
    pRowOutlineBuff->MakeScOutline();

    pColRowBuff->SetHiddenFlags(nTab);
}
