/*************************************************************************
 *
 *  $RCSfile: docsh.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: nn $ $Date: 2000-10-20 09:14:03 $
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
#include "ui_pch.hxx"
#endif

#pragma hdrstop

#include "scitems.hxx"

#include <sdb/sdbcol.hxx>
#include <sdb/sdbconn.hxx>
#include <sdb/sdbtab.hxx>

#include <sfx2/sfx.hrc>
#include <sfx2/app.hxx>
#include <sfx2/topfrm.hxx>
#include <vcl/exchange.hxx>
#include <offmgr/app.hxx>
#include <svx/srchitem.hxx>
#include <svtools/zforlist.hxx>
#include <svtools/sfxecode.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/docfilt.hxx>
#include <sfx2/evntconf.hxx>
#include <svtools/stritem.hxx>
#include <vcl/msgbox.hxx>
#include <tools/stream.hxx>
#include <tools/string.hxx>
#include <tools/urlobj.hxx>
#include <vcl/virdev.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dinfdlg.hxx>
#include <svtools/ctrltool.hxx>
#include <so3/clsids.hxx>
#include <svtools/converter.hxx>
#include <offmgr/fltrcfg.hxx>
#include <svx/svxmsbas.hxx>
#include <vcl/waitobj.hxx>
#include <unotools/charclass.hxx>

#include <sot/formats.hxx>
#define SOT_FORMATSTR_ID_STARCALC_30 SOT_FORMATSTR_ID_STARCALC

#include <offmgr/sbaobj.hxx>

#ifndef SO2_DECL_SVSTORAGESTREAM_DEFINED
#define SO2_DECL_SVSTORAGESTREAM_DEFINED
SO2_DECL_REF(SvStorageStream)
#endif

// INCLUDE ---------------------------------------------------------------

#include "docoptio.hxx"     // fuer SbaSdbExport
#include "cell.hxx"         // fuer SbaSdbExport

#include "global.hxx"
#include "filter.hxx"
#include "scmod.hxx"
#include "dbdocfun.hxx"
#include "tabvwsh.hxx"
#include "docfunc.hxx"
#include "imoptdlg.hxx"
#include "impex.hxx"
#include "scresid.hxx"
#include "sc.hrc"
#include "globstr.hrc"
#include "printfun.hxx"
#include "tpstat.hxx"
#include "scerrors.hxx"
#include "brdcst.hxx"
#include "stlpool.hxx"
#include "autostyl.hxx"
#include "attrib.hxx"
#include "asciiopt.hxx"
#include "waitoff.hxx"
#include "docpool.hxx"      // LoadCompleted
#include "progress.hxx"
#include "pntlock.hxx"
#include "collect.hxx"
#include "docuno.hxx"
#include "appoptio.hxx"
#include "detdata.hxx"
#include "printfun.hxx"     // ScJobSetup
#include "dociter.hxx"
#include "cellform.hxx"
#include "chartlis.hxx"
#include "hints.hxx"
#include "xmlwrap.hxx"

#include "docsh.hxx"

// STATIC DATA -----------------------------------------------------------

//  Stream-Namen im Storage

const sal_Char __FAR_DATA ScDocShell::pStarCalcDoc[] = STRING_SCSTREAM;     // "StarCalcDocument"
const sal_Char __FAR_DATA ScDocShell::pStyleName[] = "SfxStyleSheets";

//  Filter-Namen (wie in sclib.cxx)

static const sal_Char __FAR_DATA pFilterSc50[]      = "StarCalc 5.0";
//static const sal_Char __FAR_DATA pFilterSc50Temp[]    = "StarCalc 5.0 Vorlage/Template";
static const sal_Char __FAR_DATA pFilterSc40[]      = "StarCalc 4.0";
//static const sal_Char __FAR_DATA pFilterSc40Temp[]    = "StarCalc 4.0 Vorlage/Template";
static const sal_Char __FAR_DATA pFilterSc30[]      = "StarCalc 3.0";
//static const sal_Char __FAR_DATA pFilterSc30Temp[]    = "StarCalc 3.0 Vorlage/Template";
static const sal_Char __FAR_DATA pFilterSc10[]      = "StarCalc 1.0";
static const sal_Char __FAR_DATA pFilterXML[]       = "StarOffice XML (Calc)";
static const sal_Char __FAR_DATA pFilterAscii[]     = "Text - txt - csv (StarCalc)";
static const sal_Char __FAR_DATA pFilterLotus[]     = "Lotus";
static const sal_Char __FAR_DATA pFilterExcel4[]    = "MS Excel 4.0";
static const sal_Char __FAR_DATA pFilterEx4Temp[]   = "MS Excel 4.0 Vorlage/Template";
static const sal_Char __FAR_DATA pFilterExcel5[]    = "MS Excel 5.0/95";
static const sal_Char __FAR_DATA pFilterEx5Temp[]   = "MS Excel 5.0/95 Vorlage/Template";
static const sal_Char __FAR_DATA pFilterExcel95[]   = "MS Excel 95";
static const sal_Char __FAR_DATA pFilterEx95Temp[]  = "MS Excel 95 Vorlage/Template";
static const sal_Char __FAR_DATA pFilterExcel97[]   = "MS Excel 97";
static const sal_Char __FAR_DATA pFilterEx97Temp[]  = "MS Excel 97 Vorlage/Template";
static const sal_Char __FAR_DATA pFilterDBase[]     = "dBase";
static const sal_Char __FAR_DATA pFilterDif[]       = "DIF";
static const sal_Char __FAR_DATA pFilterSylk[]      = "SYLK";
static const sal_Char __FAR_DATA pFilterHtml[]      = "HTML (StarCalc)";
static const sal_Char __FAR_DATA pFilterRtf[]       = "Rich Text Format (StarCalc)";

//----------------------------------------------------------------------

#define ScDocShell
#include "scslots.hxx"


SFX_IMPL_INTERFACE(ScDocShell,SfxObjectShell, ScResId(SCSTR_DOCSHELL))
{}

//  GlobalName der aktuellen Version:
SFX_IMPL_OBJECTFACTORY_DLL(ScDocShell, SFXOBJECTSHELL_STD_NORMAL,
                            SvGlobalName(SO3_SC_CLASSID), Sc)

TYPEINIT1( ScDocShell, SfxObjectShell );        // SfxInPlaceObject: kein Type-Info ?

//------------------------------------------------------------------

void __EXPORT ScDocShell::FillClass( SvGlobalName* pClassName,
                                        ULONG* pFormat,
                                        String* pAppName,
                                        String* pFullTypeName,
                                        String* pShortTypeName,
                                        long nFileFormat ) const
{
    SfxInPlaceObject::FillClass( pClassName, pFormat, pAppName,
                                pFullTypeName, pShortTypeName, nFileFormat );

    if ( nFileFormat == SOFFICE_FILEFORMAT_31 )
    {
        *pClassName     = SvGlobalName( SO3_SC_CLASSID_30 );
        *pFormat        = SOT_FORMATSTR_ID_STARCALC_30;
        *pAppName       = String( ScResId( SCSTR_30_APPLICATION ) );
        *pFullTypeName  = String( ScResId( SCSTR_30_LONG_DOCNAME ) );
        *pShortTypeName = String( ScResId( SCSTR_SHORT_SCDOC_NAME ) );
    }
    else if ( nFileFormat == SOFFICE_FILEFORMAT_40 )
    {
        *pClassName     = SvGlobalName( SO3_SC_CLASSID_40 );
        *pFormat        = SOT_FORMATSTR_ID_STARCALC_40;
        *pAppName       = String( ScResId( SCSTR_40_APPLICATION ) );
        *pFullTypeName  = String( ScResId( SCSTR_40_LONG_DOCNAME ) );
        *pShortTypeName = String( ScResId( SCSTR_SHORT_SCDOC_NAME ) );
    }
    else if ( nFileFormat == SOFFICE_FILEFORMAT_NOW )
    {
        *pFullTypeName  = String( ScResId( SCSTR_LONG_SCDOC_NAME ) );
        *pShortTypeName = String( ScResId( SCSTR_SHORT_SCDOC_NAME ) );
    }
    else
        DBG_ERROR("wat fuer ne Version?");
}

//------------------------------------------------------------------

void __EXPORT ScDocShell::FillRegInfo( SvEmbeddedRegistryInfo* pInfo )
{
    SfxInPlaceObject::FillRegInfo( pInfo );

    pInfo->aObjName             = String::CreateFromAscii(pStarCalcDoc);
    pInfo->nMajorVers           = 5;
    pInfo->nMinorVers           = 0;
    pInfo->aHumanShortTypeName  = String( ScResId( SCSTR_HUMAN_SCDOC_NAME ) );

    //! nIconResId nur, wenn Desktop?

    pInfo->nIconResId           = REG_ICON_RESID_CALC;

    //  nShell und aExt sind schon gefuellt
}

//------------------------------------------------------------------

void ScDocShell::DoEnterHandler()
{
    ScTabViewShell* pViewSh = ScTabViewShell::GetActiveViewShell();
    if (pViewSh)
        if (pViewSh->GetViewData()->GetDocShell() == this)
            SC_MOD()->InputEnterHandler();
}

//------------------------------------------------------------------

USHORT ScDocShell::GetSaveTab()
{
    USHORT nTab = 0;
    ScTabViewShell* pSh = GetBestViewShell();
    if (pSh)
    {
        const ScMarkData& rMark = pSh->GetViewData()->GetMarkData();
        for ( nTab = 0; nTab <= MAXTAB; nTab++ )    // erste markierte Tabelle
            if ( rMark.GetTableSelect( nTab ) )
                break;
    }
    return nTab;
}

//------------------------------------------------------------------

inline IsAsciiDigit( sal_Unicode c )
{
    return 0x31 <= c && c <= 0x39;
}

inline IsAsciiAlpha( sal_Unicode c )
{
    return (0x41 <= c && c <= 0x5a) || (0x61 <= c && c <= 0x7a);
}


ULONG ScDocShell::SbaSdbExport( BOOL& bHasMemo, const String& rParStr,
        const String& aFileName )
{

    ULONG nErr = eERR_OK;
    bHasMemo = FALSE;
    SbaObject* pSbaObject = ((OfficeApplication*)SFX_APP())->GetSbaObject();
    DBG_ASSERT( pSbaObject, "pSbaObject==NULL" );
    SdbConnectionRef xConnect = pSbaObject->OpenConnection( rParStr );

    if ( !xConnect.Is() || !xConnect->Status().IsSuccessful() )
        return SCERR_EXPORT_CONNECT;

    SdbTableRef xTable = xConnect->CreateTable( aFileName );  // Erzeugen der Tabellenstruktur
    if ( !xTable.Is() )
        return SCERR_EXPORT_CURSOR;

    SdbColumnsRef xTblCol = xTable->Columns();  // leere Struktur holen

    USHORT nFirstCol, nFirstRow, nLastCol, nLastRow, nCol, nRow;

    USHORT nTab = GetSaveTab();
    aDocument.GetDataStart( nTab, nFirstCol, nFirstRow );
    aDocument.GetCellArea( nTab, nLastCol, nLastRow );
    ScProgress aProgress( this, ScGlobal::GetRscString( STR_SAVE_DOC ),
        nLastRow - nFirstRow );
    SvNumberFormatter* pNumFmt = aDocument.GetFormatTable();
    String aFieldName, aString, aOutString;
    SdbDatabaseType eDbType;
    xub_StrLen nFieldLen;
    USHORT nPrecision;
    BOOL bTypeDefined, bPrecDefined;
    StrCollection aFieldNamesCollection;
    BOOL bHasFieldNames = TRUE;
    for ( nCol = nFirstCol; nCol <= nLastCol && bHasFieldNames; nCol++ )
    {   // nur Strings in erster Zeile => sind Feldnamen
        if ( !aDocument.HasStringData( nCol, nFirstRow, nTab ) )
            bHasFieldNames = FALSE;
    }
    USHORT nFirstDataRow = ( bHasFieldNames ? nFirstRow + 1 : nFirstRow );
    for ( nCol = nFirstCol; nCol <= nLastCol; nCol++ )
    {
        bTypeDefined = FALSE;
        bPrecDefined = FALSE;
        nFieldLen = 0;
        nPrecision = 0;
        // Feldname[,Type[,Width[,Prec]]]
        // Typ etc.: L; D; C[,W]; N[,W[,P]]
        if ( bHasFieldNames )
        {
            aDocument.GetString( nCol, nFirstRow, nTab, aString );
            aString.ToUpperAscii();
            xub_StrLen nToken = aString.GetTokenCount( ',' );
            if ( nToken > 1 )
            {
                aFieldName = aString.GetToken( 0, ',' );
                aString.EraseAllChars( ' ' );
                switch ( aString.GetToken( 1, ',' ).GetChar(0) )
                {
                    case 'L' :
                        eDbType = SDB_DBTYPE_BOOLEAN;
                        nFieldLen = 1;
                        bTypeDefined = TRUE;
                        bPrecDefined = TRUE;
                        break;
                    case 'D' :
                        eDbType = SDB_DBTYPE_DATE;
                        nFieldLen = 8;
                        bTypeDefined = TRUE;
                        bPrecDefined = TRUE;
                        break;
                    case 'M' :
                        eDbType = SDB_DBTYPE_LONGVARCHAR;
                        nFieldLen = 10;
                        bTypeDefined = TRUE;
                        bPrecDefined = TRUE;
                        bHasMemo = TRUE;
                        break;
                    case 'C' :
                        eDbType = SDB_DBTYPE_VARCHAR;
                        bTypeDefined = TRUE;
                        bPrecDefined = TRUE;
                        break;
                    case 'N' :
                        eDbType = SDB_DBTYPE_DECIMAL;
                        bTypeDefined = TRUE;
                        break;
                }
                if ( bTypeDefined && !nFieldLen && nToken > 2 )
                {
                    nFieldLen = aString.GetToken( 2, ',' ).ToInt32();
                    if ( !bPrecDefined && nToken > 3 )
                    {
                        String aTmp( aString.GetToken( 3, ',' ) );
                        if ( CharClass::isAsciiNumeric(aTmp) )
                        {
                            nPrecision = aTmp.ToInt32();
                            bPrecDefined = TRUE;
                        }
                    }
                }
            }
            else
                aFieldName = aString;

            // Feldnamen pruefen und ggbf. gueltigen Feldnamen erzeugen.
            // Erstes Zeichen muss Buchstabe sein,
            // weitere nur alphanumerisch und Unterstrich erlaubt,
            // "_DBASELOCK" ist reserviert (obsolet weil erstes Zeichen kein Buchstabe),
            // keine doppelten Namen.
            if ( !IsAsciiAlpha( aFieldName.GetChar(0) ) )
                aFieldName.Insert( 'N', 0 );
            String aTmpStr;
            sal_Unicode c;
            for ( const sal_Unicode* p = aFieldName.GetBuffer(); c = *p; p++ )
            {
                if ( IsAsciiAlpha( c ) || IsAsciiDigit( c ) || c == '_' )
                    aTmpStr += c;
                else
                    aTmpStr += '_';
            }
            aFieldName = aTmpStr;
            if ( aFieldName.Len() > 10 )
                aFieldName.Erase( 10 );
            StrData* pStrData = new StrData( aFieldName );
            if ( !aFieldNamesCollection.Insert( pStrData ) )
            {   // doppelter Feldname, numerisch erweitern
                USHORT nSub = 1;
                String aFixPart( aFieldName );
                do
                {
                    ++nSub;
                    String aVarPart = String::CreateFromInt32( nSub );
                    if ( aFixPart.Len() + aVarPart.Len() > 10 )
                        aFixPart.Erase( 10 - aVarPart.Len() );
                    aFieldName = aFixPart;
                    aFieldName += aVarPart;
                    pStrData->SetString( aFieldName );
                } while ( !aFieldNamesCollection.Insert( pStrData ) );
            }
        }
        else
        {
            aFieldName = 'N';
            aFieldName += String::CreateFromInt32(nCol+1);
        }

        if ( !bTypeDefined )
        {   // Feldtyp
            ScBaseCell* pCell;
            aDocument.GetCell( nCol, nFirstDataRow, nTab, pCell );
            if ( !pCell || pCell->HasStringData() )
                eDbType = SDB_DBTYPE_VARCHAR;
            else
            {
                ULONG nFormat;
                aDocument.GetNumberFormat( nCol, nFirstDataRow, nTab, nFormat );
                if ( pCell && pCell->GetCellType() == CELLTYPE_FORMULA
                  && ((nFormat % SV_COUNTRY_LANGUAGE_OFFSET) == 0) )
                {
                    nFormat = ScGlobal::GetStandardFormat(
                        ((ScFormulaCell*)pCell)->GetValue(), *pNumFmt, nFormat,
                        ((ScFormulaCell*)pCell)->GetFormatType() );
                }
                switch ( pNumFmt->GetType( nFormat ) )
                {
                    case NUMBERFORMAT_LOGICAL :
                        eDbType = SDB_DBTYPE_BOOLEAN;
                        nFieldLen = 1;
                        break;
                    case NUMBERFORMAT_DATE :
                        eDbType = SDB_DBTYPE_DATE;
                        nFieldLen = 8;
                        break;
                    case NUMBERFORMAT_TIME :
                    case NUMBERFORMAT_DATETIME :
                        eDbType = SDB_DBTYPE_VARCHAR;
                        break;
                    default:
                        eDbType = SDB_DBTYPE_DECIMAL;
                }
            }
        }
        BOOL bSdbLenAdjusted = FALSE;
        BOOL bSdbLenBad = FALSE;
        // Feldlaenge
        if ( eDbType == SDB_DBTYPE_VARCHAR && !nFieldLen )
        {   // maximale Feldbreite bestimmen
            nFieldLen = aDocument.GetMaxStringLen( nTab, nCol, nFirstDataRow,
                nLastRow );
            if ( nFieldLen == 0 )
                nFieldLen = 1;
        }
        else if ( eDbType == SDB_DBTYPE_DECIMAL )
        {   // maximale Feldbreite und Nachkommastellen bestimmen
            xub_StrLen nLen;
            USHORT nPrec;
            nLen = aDocument.GetMaxNumberStringLen( nPrec, nTab, nCol,
                nFirstDataRow, nLastRow );
            // dBaseIII Limit Nachkommastellen: 15
            if ( nPrecision > 15 )
                nPrecision = 15;
            if ( nPrec > 15 )
                nPrec = 15;
            if ( bPrecDefined && nPrecision != nPrec )
            {   // Laenge auf vorgegebene Nachkommastellen anpassen
                if ( nPrecision )
                    nLen += nPrecision - nPrec;
                else
                    nLen -= nPrec+1;            // auch den . mit raus
            }
            if ( nLen > nFieldLen )
                nFieldLen = nLen;
            if ( !bPrecDefined )
                nPrecision = nPrec;
            if ( nFieldLen == 0 )
                nFieldLen = 1;
            else if ( nFieldLen > 19 )
                nFieldLen = 19;     // dBaseIII Limit Feldlaenge numerisch: 19
            if ( nPrecision && nFieldLen < nPrecision + 2 )
                nFieldLen = nPrecision + 2;     // 0. muss mit reinpassen
            // 538 MUST: Sdb internal representation adds 2 to the field length!
            // To give the user what he wants we must substract it here.
             //! CAVEAT! There is no way to define a numeric field with a length
             //! of 1 and no decimals!
            if ( nFieldLen == 1 && nPrecision == 0 )
                bSdbLenBad = TRUE;
            nFieldLen = SvDbaseConverter::ConvertPrecisionToOdbc( nFieldLen, nPrecision );
            bSdbLenAdjusted = TRUE;
        }
        if ( nFieldLen > 254 )
        {
            if ( eDbType == SDB_DBTYPE_VARCHAR )
            {   // zu lang fuer normales Textfeld => Memofeld
                eDbType = SDB_DBTYPE_LONGVARCHAR;
                nFieldLen = 10;
                bHasMemo = TRUE;
            }
            else
                nFieldLen = 254;                    // dumm gelaufen..
        }
        xTblCol->AddColumn( xConnect->CreateColumn(
                                    aFieldName,     // feld name (max 10 Zeichen)
                                    eDbType,        // typ
                                    nFieldLen,      // feldlaenge
                                    nPrecision));   // Nachkommastellen
        // undo change to field length, reflect reality
        if ( bSdbLenAdjusted )
        {
            nFieldLen = SvDbaseConverter::ConvertPrecisionToDbase( nFieldLen, nPrecision );
            if ( bSdbLenBad && nFieldLen == 1 )
                nFieldLen = 2;      // THIS is reality
        }
        if ( bTypeDefined )
        {   // Angabe anpassen und ausgeben
            aOutString = aFieldName;
            switch ( eDbType )
            {
                case SDB_DBTYPE_BOOLEAN :
                    aOutString.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ",L" ));
                    break;
                case SDB_DBTYPE_DATE :
                    aOutString.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ",D" ));
                    break;
                case SDB_DBTYPE_LONGVARCHAR :
                    aOutString.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ",M" ));
                    break;
                case SDB_DBTYPE_VARCHAR :
                    aOutString.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ",C," ));
                    aOutString += String::CreateFromInt32( nFieldLen );
                    break;
                case SDB_DBTYPE_DECIMAL :
                    aOutString.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ",N," ));
                    aOutString += String::CreateFromInt32( nFieldLen );
                    aOutString += ',';
                    aOutString += String::CreateFromInt32( nPrecision );
                    break;
            }
            if ( !aOutString.EqualsIgnoreCaseAscii( aString ) )
            {
                aDocument.SetString( nCol, nFirstRow, nTab, aOutString );
                PostPaint( nCol, nFirstRow, nTab, nCol, nFirstRow, nTab, PAINT_GRID );
            }
        }
    }
    xTable->Update();

    if ( xTable->Status().IsError() )
        nErr = SCERR_EXPORT_CURSOR;
    else
    {
        SdbCursorRef xCur = xConnect->CreateCursor( SDB_KEYSET, SDB_APPENDONLY );
        SdbCursor* pCur = &xCur;
        DBG_ASSERT( pCur, "Cursor" );
        pCur->Open( aFileName, TRUE );

        if ( !pCur->Status().IsError() )
        {
            USHORT nIdx;
            double fVal;
            Date aDate;
            SvNumberFormatter& rFormatter = *aDocument.GetFormatTable();
            ScFieldEditEngine aEditEngine( aDocument.GetEditPool() );
            for ( nRow = nFirstDataRow; nRow <= nLastRow; nRow++ )
            {
                pCur->AppendRow();
                for ( nCol = nFirstCol, nIdx = 1; nCol <= nLastCol; nCol++, nIdx++ )
                {
                    ODbVariantRef xVar = (*pCur->GetRow())[nIdx];
                    const SdbColumn* pCol = xTblCol->Column( nIdx );
                    SdbDatabaseType eType = pCol->GetType();
                    switch ( eType )
                    {
                        case SDB_DBTYPE_LONGVARCHAR :
                        {
                            ScBaseCell* pCell;
                            aDocument.GetCell( nCol, nRow, nTab, pCell );
                            if ( pCell && pCell->GetCellType() != CELLTYPE_NOTE )
                            {
                                if ( pCell->GetCellType() == CELLTYPE_EDIT )
                                {   // #60761# Paragraphs erhalten
                                    aEditEngine.SetText( *((ScEditCell*)pCell)->GetData() );
                                    aString = aEditEngine.GetText( LINEEND_CRLF );
                                }
                                else
                                {
                                    ULONG nFormat;
                                    Color* pColor;
                                    aDocument.GetNumberFormat( nCol, nRow, nTab, nFormat );
                                    ScCellFormat::GetString( pCell, nFormat, aString, &pColor, rFormatter );
                                }
                                xVar->setString( aString );
                            }
                            else
                                xVar->setVoid();
                        }
                        break;
                        case SDB_DBTYPE_VARCHAR :
                            aDocument.GetString( nCol, nRow, nTab, aString );
                            xVar->setString( aString );
                            if ( nErr == eERR_OK && pCol->GetLength() < aString.Len() )
                                nErr = SCWARN_EXPORT_DATALOST;
                        break;
                        case SDB_DBTYPE_DATE :
                        {
                            aDocument.GetValue( nCol, nRow, nTab, fVal );
                            // #39274# zwischen 0 Wert und 0 kein Wert unterscheiden
                            BOOL bIsNull = (fVal == 0.0);
                            if ( bIsNull )
                                bIsNull = !aDocument.HasValueData( nCol, nRow, nTab );
                            if ( bIsNull )
                            {
                                xVar->setVoid();    // sonst Basic-NullDate
                                if ( nErr == eERR_OK &&
                                        aDocument.HasStringData( nCol, nRow, nTab ) )
                                    nErr = SCWARN_EXPORT_DATALOST;
                            }
                            else
                            {
                                // unser NullDate: 30.12.1899 / 1.1.1900 / 1.1.1904
                                aDate = *(pNumFmt->GetNullDate());
                                aDate += (long)fVal;
                                // Basic NullDate: 30.12.1899
                                //pVar->PutDate( (double)( aDate - Date(30,12,1899) ) );
                                xVar->setDate( aDate );
                            }
                        }
                        break;
                        case SDB_DBTYPE_BOOLEAN :
                        case SDB_DBTYPE_DECIMAL :
                        {
                            aDocument.GetValue( nCol, nRow, nTab, fVal );
                            if ( fVal == 0.0 && nErr == eERR_OK &&
                                    aDocument.HasStringData( nCol, nRow, nTab ) )
                                nErr = SCWARN_EXPORT_DATALOST;
                            xVar->setDouble( fVal );
                        }
                        break;
                        default:
                            DBG_ERROR( "ScDocShell::SbaSdbExport: unknown FieldType" );
                            if ( nErr == eERR_OK )
                                nErr = SCWARN_EXPORT_DATALOST;
                            aDocument.GetValue( nCol, nRow, nTab, fVal );
                            xVar->setDouble( fVal );
                    }
                }
                pCur->UpdateRow();
                if ( pCur->Status().IsError() )
                {   // #63824# aussagekraeftige Fehlermeldung
                    //! 2do: was ist bei Call aus Basic? generelles Problem..
                    String aMsg;
                    ScRange aRange( nFirstCol, nRow, nTab, nLastCol, nRow, nTab );
                    aRange.Format( aMsg, (SCA_VALID | SCA_TAB_3D), &aDocument );
                    aMsg += '\n';
                    const SdbStatus& rStat = pCur->Status();
                    aMsg += rStat.ErrorMessage();
                    aMsg.AppendAscii(RTL_CONSTASCII_STRINGPARAM( "\n\n" ));
                    aMsg += rStat.ErrorInfo();
                    // STR_MSSG_DOSUBTOTALS_0 == "StarCalc"
                    BOOL bOk = MessBox( GetDialogParent(),
                        WinBits( WB_OK_CANCEL | WB_DEF_OK ),
                        ScGlobal::GetRscString( STR_MSSG_DOSUBTOTALS_0 ),
                        aMsg ).Execute();
                    if ( bOk )
                    {
                        nErr = SCWARN_EXPORT_DATALOST;
                        // Wenigstens schreiben, was geht,
                        // SdbCursor UpdateRow schreibt bei Ueberlaeufen nichts.
                        // Leider laesst sich nicht feststellen, welche Spalte
                        // den Fehler ausgeloest hat.
                        //! 2do: Sdb sollte hier einen Spaltenwert liefern
                        USHORT nCol, nIdx;
                        for ( nCol = nFirstCol, nIdx = 1; nCol <= nLastCol; nCol++, nIdx++ )
                        {
                            switch ( xTblCol->Column( nIdx )->GetType() )
                            {
                                case SDB_DBTYPE_DECIMAL :
                                    //pCur->Variable( nIdx )->PutNull();
                                    (*pCur->GetRow())[nIdx]->setVoid();
                                break;
                            }
                        }
                        pCur->UpdateRow();
                        if ( pCur->Status().IsError() )
                        {   // Letzter Versuch: eine leere Zeile
                            for ( nCol = nFirstCol, nIdx = 1; nCol <= nLastCol; nCol++, nIdx++ )
                            {
                                //pCur->Variable( nIdx )->PutNull();
                                (*pCur->GetRow())[nIdx]->setVoid();
                            }
                            pCur->UpdateRow();
                            if ( pCur->Status().IsError() )
                            {   // da geht anscheinend gar nichts mehr
                                nErr = SCERR_EXPORT_DATA;
                                break;  // for ( nRow ... )
                            }
                        }
                    }
                    else
                    {
                        nErr = SCERR_EXPORT_DATA;
                        break;
                    }
                }
                if ( !aProgress.SetStateOnPercent( nRow - nFirstRow ) )
                {   // UserBreak
                    nErr = SCERR_EXPORT_DATA;
                    break;
                }
            }
        }
        else
            nErr = SCERR_EXPORT_CURSOR;
        pCur->Close();
    }

    return nErr;
}


BOOL ScDocShell::LoadCalc( SvStorage* pStor )       // StarCalc 3 oder 4 Datei
{
    SfxEventConfiguration* pConf = SFX_APP()->GetEventConfig();
    BOOL bWarningEnabled = pConf && pConf->IsWarningForced();
    if (bWarningEnabled)
        aDocument.SetMacroCallMode(SC_MACROCALL_ASK);   // ask before executing

    BOOL bRet = TRUE;

    SvStorageStreamRef aPoolStm = pStor->OpenStream( String::CreateFromAscii(pStyleName), STREAM_STD_READ );
    SvStorageStreamRef aDocStm  = pStor->OpenStream( String::CreateFromAscii(pStarCalcDoc), STREAM_STD_READ );
    ULONG nPoolErr = aPoolStm->GetError();
    ULONG nDocErr  = aDocStm->GetError();

    ScProgress* pProgress = NULL;
    if ( eShellMode == SFX_CREATE_MODE_STANDARD && !nDocErr )
    {
        ULONG nCurPos = aDocStm->Tell();
        ULONG nEndPos = aDocStm->Seek( STREAM_SEEK_TO_END );
        aDocStm->Seek( nCurPos );
        ULONG nRange = nEndPos - nCurPos;
        pProgress = new ScProgress( this, ScGlobal::GetRscString(STR_LOAD_DOC), nRange );
    }

    if ( nPoolErr == ERRCODE_IO_NOTEXISTS && nDocErr == ERRCODE_IO_NOTEXISTS )
    {
        //  leerer Storage = leeres Dokument -> wie InitNew

        aDocument.MakeTable(0);
        Size aSize( (long) ( STD_COL_WIDTH           * HMM_PER_TWIPS * OLE_STD_CELLS_X ),
                    (long) ( ScGlobal::nStdRowHeight * HMM_PER_TWIPS * OLE_STD_CELLS_Y ) );
        // hier muss auch der Start angepasst werden
        SetVisAreaOrSize( Rectangle( Point(), aSize ), TRUE );
        aDocument.GetStyleSheetPool()->CreateStandardStyles();
        aDocument.UpdStlShtPtrsFrmNms();
    }
    else if ( !nPoolErr && !nDocErr )
    {
        aPoolStm->SetVersion(pStor->GetVersion());
        aDocStm->SetVersion(pStor->GetVersion());

        aDocument.Clear();          // keine Referenzen auf Pool behalten!

        RemoveItem( SID_ATTR_CHAR_FONTLIST );
        RemoveItem( ITEMID_COLOR_TABLE );
        RemoveItem( ITEMID_GRADIENT_LIST );
        RemoveItem( ITEMID_HATCH_LIST );
        RemoveItem( ITEMID_BITMAP_LIST );
        RemoveItem( ITEMID_DASH_LIST );
        RemoveItem( ITEMID_LINEEND_LIST );

        aDocument.LoadPool( *aPoolStm, FALSE );     // FALSE: RefCounts nicht laden
        bRet = (aPoolStm->GetError() == 0);

        if ( !bRet )
        {
            pStor->SetError( aPoolStm->GetError() );
            DBG_ERROR( "Fehler im Pool-Stream" );
        }
        else if (eShellMode != SFX_CREATE_MODE_ORGANIZER)
        {
            //  ViewOptions are not completely stored (to avoid warnings),
            //  so must be initialized from global settings.
            //! This can be removed if a new file format loads all ViewOptions !!!

            aDocument.SetViewOptions( SC_MOD()->GetViewOptions() );

            ULONG nErrCode = aPoolStm->GetErrorCode();
            const ByteString aStrKey = pStor->GetKey();
            const BOOL   bKeySet = (aStrKey.Len() > 0);

            if ( bKeySet )
                aDocStm->SetKey( aStrKey );         // Passwort setzen

            bRet = aDocument.Load( *aDocStm, pProgress );

            if ( !bRet && bKeySet ) // Passwort falsch
            {
                SetError( ERRCODE_SFX_WRONGPASSWORD );
                pStor->SetError( ERRCODE_SFX_WRONGPASSWORD );
            }

            if ( aDocStm->GetError() )
            {
                //  Zeile-8192-Meldung braucht nur zu kommen, wenn die Zeilen
                //  beschraenkt sind, sonst ist "falsches Format" besser
#ifdef SC_LIMIT_ROWS
                if ( aDocument.HasLostData() )                  // zuviele Zeilen?
                    pStor->SetError( SCERR_IMPORT_8K_LIMIT );
                else
#endif
                    pStor->SetError( aDocStm->GetError() );
                bRet = FALSE;
                DBG_ERROR( "Fehler im Document-Stream" );
            }
            else
            {
                if ( !(nErrCode & ERRCODE_WARNING_MASK) )
                    nErrCode = aDocStm->GetErrorCode();
                if ( nErrCode & ERRCODE_WARNING_MASK )
                    SetError( nErrCode );
            }

            if (bRet)
            {
                aDocument.UpdateFontCharSet();

                UpdateLinks();              // verknuepfte Tabellen in Link-Manager
                RemoveUnknownObjects();     // unbekannte Ole-Objekte loeschen
            }
        }
        else            // SFX_CREATE_MODE_ORGANIZER
        {
            //  UpdateStdNames is called from ScDocument::Load, but is also needed
            //  if only the styles are loaded!
            ScStyleSheetPool* pStylePool = aDocument.GetStyleSheetPool();
            if (pStylePool)
                pStylePool->UpdateStdNames();   // correct style names for different languages
        }

        if (bRet)
            aDocument.GetPool()->MyLoadCompleted();
    }
    else
    {
        DBG_ERROR( "Stream-Fehler");
        bRet = FALSE;
    }

    if (!aDocument.HasTable(0))
        aDocument.MakeTable(0);

    if (eShellMode == SFX_CREATE_MODE_EMBEDDED)
    {
        USHORT nVisTab = aDocument.GetVisibleTab();
        BOOL bHasVis = aDocument.HasTable(nVisTab);
        if ( SfxInPlaceObject::GetVisArea().IsEmpty() || !bHasVis )
        {
            if (!bHasVis)
            {
                nVisTab = 0;
                aDocument.SetVisibleTab(nVisTab);
            }
            USHORT nStartCol,nStartRow;
            aDocument.GetDataStart( nVisTab, nStartCol, nStartRow );
            USHORT nEndCol,nEndRow;
            aDocument.GetPrintArea( nVisTab, nEndCol, nEndRow );
            if (nStartCol>nEndCol)
                nStartCol = nEndCol;
            if (nStartRow>nEndRow)
                nStartRow = nEndRow;
            // hier muss auch der Start angepasst werden
            SetVisAreaOrSize( aDocument.GetMMRect( nStartCol,nStartRow, nEndCol,nEndRow, nVisTab ),
                                TRUE );
        }
    }

    delete pProgress;

    if ( eShellMode == SFX_CREATE_MODE_STANDARD && bRet )
    {
        if ( bWarningEnabled )
        {
            //  If the document contains macro calls, ask now
            //  otherwise set to ALLOWED (for entering new formulas).
            //  (If document is loaded invisibly, ASK state is kept)

            if ( aDocument.HasMacroCallsAfterLoad() )
                aDocument.CheckMacroWarn();
            else
                aDocument.SetMacroCallMode(SC_MACROCALL_ALLOWED);
        }
    }

    return bRet;
}


BOOL ScDocShell::SaveCalc( SvStorage* pStor )           // Calc 3 oder 4 Datei
{
    BOOL bRet = TRUE;

    ScProgress* pProgress = NULL;
    if ( eShellMode == SFX_CREATE_MODE_STANDARD )
    {
        ULONG nRange = aDocument.GetWeightedCount() + 1;
        pProgress = new ScProgress( this, ScGlobal::GetRscString(STR_SAVE_DOC), nRange );
    }

    SvStorageStreamRef aPoolStm = pStor->OpenStream( String::CreateFromAscii(pStyleName) );
    if( !aPoolStm->GetError() )
    {
        aPoolStm->SetVersion(pStor->GetVersion());
        aPoolStm->SetSize(0);
        bRet = aDocument.SavePool( *aPoolStm );
    }
    else
    {
        DBG_ERROR( "Stream Error" );
        bRet = FALSE;
    }

    if ( bRet && eShellMode != SFX_CREATE_MODE_ORGANIZER )
    {
        SvStorageStreamRef aDocStm  = pStor->OpenStream( String::CreateFromAscii(pStarCalcDoc) );
        if( !aDocStm->GetError() )
        {
            aDocStm->SetVersion(pStor->GetVersion());
            aDocStm->SetKey(pStor->GetKey());               // Passwort setzen
            aDocStm->SetSize(0);
            bRet = aDocument.Save( *aDocStm, pProgress );
            DBG_ASSERT( bRet, "Error while saving" );

            if ( aDocument.HasLostData() )
            {
                //  Warnung, dass nicht alles gespeichert wurde

                if (!pStor->GetError())
                    pStor->SetError(SCWARN_EXPORT_MAXROW);
            }
            else if ( aDocStm->GetErrorCode() && !pStor->GetErrorCode() )
                pStor->SetError(aDocStm->GetErrorCode());
            else if ( OFF_APP()->GetFilterOptions()->IsLoadExcelBasicStorage() )
            {
                //  #75497# warning if MS VBA macros are lost
                //  GetSaveWarningOfMSVBAStorage checks if sub-storage with VBA macros is present
                //  (only possible when editing Excel documents -- storage is not copied to
                //  StarCalc files)
                ULONG nVBAWarn = SvxImportMSVBasic::GetSaveWarningOfMSVBAStorage( *this );
                if ( nVBAWarn && !pStor->GetErrorCode() )
                    pStor->SetError( nVBAWarn );
            }
        }
        else
        {
            DBG_ERROR( "Stream Error" );
            bRet = FALSE;
        }
    }

    delete pProgress;

    return bRet;
}


BOOL __EXPORT ScDocShell::Load( SvStorage* pStor )
{
    DBG_ASSERT( pStor, "Nanu... Load ohne Storage?" );
    //  wait cursor is handled with progress bar

    GetUndoManager()->Clear();

    BOOL bRet = SfxInPlaceObject::Load( pStor );
    if( bRet )
        bRet = LoadCalc( pStor );

    if (!bRet && !pStor->GetError())
        pStor->SetError( SVSTREAM_FILEFORMAT_ERROR );

    InitItems();
    CalcOutputFactor();

    // #73762# invalidate eventually temporary table areas
    if ( bRet )
        aDocument.InvalidateTableArea();

    bIsEmpty = FALSE;
    FinishedLoading( SFX_LOADED_MAINDOCUMENT | SFX_LOADED_IMAGES );
    return bRet;
}


void __EXPORT ScDocShell::SFX_NOTIFY( SfxBroadcaster& rBC, const TypeId& rBCType,
                         const SfxHint& rHint, const TypeId& rHintType )
{
    if (rHint.ISA(SfxSimpleHint))                               // ohne Parameter
    {
        ULONG nSlot = ((const SfxSimpleHint&)rHint).GetId();
        switch ( nSlot )
        {
            case SFX_HINT_TITLECHANGED:
                aDocument.SetName( SfxShell::GetName() );
                //  RegisterNewTargetNames gibts nicht mehr
                SFX_APP()->Broadcast(SfxSimpleHint( SC_HINT_DOCNAME_CHANGED )); // Navigator
                break;
        }
    }
    else if (rHint.ISA(SfxStyleSheetHint))                      // Vorlagen geaendert
        NotifyStyle((const SfxStyleSheetHint&) rHint);
    else if (rHint.ISA(ScAutoStyleHint))
    {
        //! direct call for AutoStyles

        //  this is called synchronously from ScInterpreter::ScStyle,
        //  modifying the document must be asynchronous
        //  (handled by AddInitial)

        ScAutoStyleHint& rStlHint = (ScAutoStyleHint&)rHint;
        ScRange aRange = rStlHint.GetRange();
        String aName1 = rStlHint.GetStyle1();
        String aName2 = rStlHint.GetStyle2();
        UINT32 nTimeout = rStlHint.GetTimeout();

        if (!pAutoStyleList)
            pAutoStyleList = new ScAutoStyleList(this);
        pAutoStyleList->AddInitial( aRange, aName1, nTimeout, aName2 );
    }
}

    // Inhalte fuer Organizer laden


BOOL __EXPORT ScDocShell::LoadFrom( SvStorage* pStor )
{
    DBG_ASSERT( pStor, "Nanu... LoadFrom ohne Storage?" );

    WaitObject aWait( GetDialogParent() );

    BOOL bRet = FALSE;
    SvStorageStreamRef aPoolStm = pStor->OpenStream( String::CreateFromAscii(pStyleName), STREAM_STD_READ );
    if ( !aPoolStm->GetError() )
    {
        aPoolStm->SetVersion(pStor->GetVersion());

        aDocument.Clear();                      // keine Referenzen auf Pool behalten!
        RemoveItem( SID_ATTR_CHAR_FONTLIST );
        RemoveItem( ITEMID_COLOR_TABLE );
        RemoveItem( ITEMID_GRADIENT_LIST );
        RemoveItem( ITEMID_HATCH_LIST );
        RemoveItem( ITEMID_BITMAP_LIST );
        RemoveItem( ITEMID_DASH_LIST );
        RemoveItem( ITEMID_LINEEND_LIST );

        aDocument.LoadPool( *aPoolStm, TRUE );      // TRUE: RefCounts aus Datei laden
        bRet = (aPoolStm->GetError() == 0);
        DBG_ASSERT( bRet, "Error in pool stream" );

        //  UpdateStdNames is called from ScDocument::Load, but is also needed
        //  if only the styles are loaded!
        ScStyleSheetPool* pStylePool = aDocument.GetStyleSheetPool();
        if (pStylePool)
            pStylePool->UpdateStdNames();   // correct style names for different languages

        //  Hier auf keinen Fall LoadCompleted, weil ohne Laden der Tabellen die RefCounts
        //  nicht hochgezaehlt wurden.
        //  Die Items wuerden dann geloescht, und beim Speichern wuerde Muell herauskommen.
        //  Darum die Ref-Counts aus der Datei laden (TRUE bei LoadPool).
        //  (Bug #37635#)

        InitItems();
        //  CalcOutputFactor interessiert hier nicht
    }

    SfxObjectShell::LoadFrom( pStor );

    return bRet;
}


BOOL __EXPORT ScDocShell::ConvertFrom( SfxMedium& rMedium )
{
    BOOL bRet = FALSE;              // FALSE heisst Benutzerabbruch !!
                                    // bei Fehler: Fehler am Stream setzen!!

    GetUndoManager()->Clear();

    // ob nach dem Import optimale Spaltenbreiten gesetzt werden sollen
    BOOL bSetColWidths = FALSE;
    BOOL bSetSimpleTextColWidths = FALSE;
    BOOL bSimpleColWidth[MAXCOL+1];
    memset( bSimpleColWidth, 1, (MAXCOL+1) * sizeof(BOOL) );
    ScRange aColWidthRange;
    // ob nach dem Import optimale Zeilenhoehen gesetzt werden sollen
    BOOL bSetRowHeights = FALSE;

    aConvFilterName.Erase(); //@ #BugId 54198

    //  Alle Filter brauchen die komplette Datei am Stueck (nicht asynchron),
    //  darum vorher per CreateFileStream dafuer sorgen, dass die komplette
    //  Datei uebertragen wird.
    rMedium.GetPhysicalName();  //! CreateFileStream direkt rufen, wenn verfuegbar

    const SfxFilter* pFilter = rMedium.GetFilter();
    if (pFilter)
    {
        String aFltName = pFilter->GetFilterName();

        aConvFilterName=aFltName; //@ #BugId 54198

        BOOL bCalc3 = ( aFltName.EqualsAscii(pFilterSc30) );
        BOOL bCalc4 = ( aFltName.EqualsAscii(pFilterSc40) );
        if (!bCalc3 && !bCalc4)
            aDocument.SetInsertingFromOtherDoc( TRUE );

        if (bCalc3 || bCalc4)                   // Calc3/4 - "Import"
        {
            //  wait cursor is handled with progress bar
            SvStorage* pStor = rMedium.GetStorage();
            if ( pStor )
            {
                bRet = SfxInPlaceObject::Load( pStor );
                if( bRet )
                    bRet = LoadCalc( pStor );
                if (!bRet && !pStor->GetError())
                    pStor->SetError( SVSTREAM_FILEFORMAT_ERROR );
            }
            else
                DBG_ERROR("Calc3/4: kein Storage");
        }
        else if (aFltName.EqualsAscii(pFilterXML))
        {
            // prevent unnecessary broadcasts and updates
            ScDocShellModificator aModificator( *this );

            aDocument.SetImportingXML( TRUE );
            SvStream* pInStream = rMedium.GetInStream();
            if (pInStream)
                pInStream->Seek( 0 );

            ScXMLImportWrapper aImport( aDocument, rMedium );
            bRet = aImport.Import();
            if ( bRet )
            {
                // don't prevent establishing of listeners anymore
                aDocument.SetInsertingFromOtherDoc( FALSE );
                aDocument.CompileXML();
            }
            aDocument.SetImportingXML( FALSE );

            bSetRowHeights = TRUE;  // row height update is disabled during XML import
        }
        else if (aFltName.EqualsAscii(pFilterSc10))
        {
            SvStream* pStream = rMedium.GetInStream();
            if (pStream)
            {
                FltError eError = ScImportStarCalc10( *pStream, &aDocument );
                if (eError != eERR_OK)
                {
                    if (!GetError())
                        SetError(eError);
                }
                else
                    bRet = TRUE;
            }
        }
        else if (aFltName.EqualsAscii(pFilterLotus))
        {
//          SvStream* pStream = rMedium.GetInStream();
//          if (pStream)
            {
                FltError eError;
                String sCharSet;
                String sItStr;
                SfxItemSet*  pSet = rMedium.GetItemSet();
                const SfxPoolItem* pItem;
                if ( pSet && SFX_ITEM_SET ==
                     pSet->GetItemState( SID_FILE_FILTEROPTIONS, TRUE, &pItem ) )
                {
                    sItStr = ((const SfxStringItem*)pItem)->GetValue();
                }

                if (sItStr.Len() == 0)
                {
                    String aTitle = ScGlobal::GetRscString( STR_IMPORT_LOTUS );
                    sCharSet = String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM( "IBMPC (437)" ));
                    ScImportOptions aOptions(59, 34, sCharSet);
                    ScImportOptionsDlg* pDlg =
                        new ScImportOptionsDlg( NULL, FALSE, &aOptions, &aTitle );
                    int nDlgRet;
                    {   // WarteCursor aus und wieder einschalten
                        ScWaitCursorOff aWait( GetDialogParent() );
                        nDlgRet = pDlg->Execute();
                    }
                    if ( RET_OK == nDlgRet )
                    {
                        pDlg->GetImportOptions( aOptions );
                        sCharSet = aOptions.aStrFont;
                        SfxItemSet* pItemSet = rMedium.GetItemSet();
                        if (pItemSet)
                            pItemSet->Put(SfxStringItem(SID_FILE_FILTEROPTIONS, sCharSet));
//                      eError = ScImportLotus123( *pStream, &aDocument,
//                                      GetCharsetValue(sCharSet));
                        ScColumn::bDoubleAlloc = TRUE;
                        eError = ScImportLotus123( rMedium, &aDocument,
                                        GetCharsetValue(sCharSet));
                        ScColumn::bDoubleAlloc = FALSE;
                        if (eError != eERR_OK)
                        {
                            if (!GetError())
                                SetError(eError);

                            if( ( eError & ERRCODE_WARNING_MASK ) == ERRCODE_WARNING_MASK )
                                bRet = TRUE;
                        }
                        else
                            bRet = TRUE;
                    }
                    else
                    {
                        // ConvertFrom lautlos abbrechen
                        SetError( ERRCODE_IO_ABORT );
                        bRet = FALSE;
                    }

                    delete pDlg;
                }
                else
                {
                    sCharSet  = sItStr;
//                  eError = ScImportLotus123( *pStream, &aDocument,
//                                      GetCharsetValue(sCharSet));
                    ScColumn::bDoubleAlloc = TRUE;
                    eError = ScImportLotus123( rMedium, &aDocument,
                                        GetCharsetValue(sCharSet));
                    ScColumn::bDoubleAlloc = FALSE;
                    if (eError != eERR_OK)
                    {
                        if (!GetError())
                            SetError(eError);

                        if( ( eError & ERRCODE_WARNING_MASK ) == ERRCODE_WARNING_MASK )
                            bRet = TRUE;
                    }
                    else
                        bRet = TRUE;
                }
//              rMedium.CloseInStream();
            }
        }
        else if ( aFltName.EqualsAscii(pFilterExcel4) || aFltName.EqualsAscii(pFilterExcel5) ||
                   aFltName.EqualsAscii(pFilterExcel95) || aFltName.EqualsAscii(pFilterExcel97) ||
                   aFltName.EqualsAscii(pFilterEx4Temp) || aFltName.EqualsAscii(pFilterEx5Temp) ||
                   aFltName.EqualsAscii(pFilterEx95Temp) || aFltName.EqualsAscii(pFilterEx97Temp) )
        {
            EXCIMPFORMAT eFormat = EIF_AUTO;
            if ( aFltName.EqualsAscii(pFilterExcel4) || aFltName.EqualsAscii(pFilterEx4Temp) )
                eFormat = EIF_BIFF_LE4;
            else if ( aFltName.EqualsAscii(pFilterExcel5) || aFltName.EqualsAscii(pFilterExcel95) ||
                      aFltName.EqualsAscii(pFilterEx5Temp) || aFltName.EqualsAscii(pFilterEx95Temp) )
                eFormat = EIF_BIFF5;
            else if ( aFltName.EqualsAscii(pFilterExcel97) || aFltName.EqualsAscii(pFilterEx97Temp) )
                eFormat = EIF_BIFF8;

            MakeDrawLayer();                //! im Filter
            ScColumn::bDoubleAlloc = TRUE;
            FltError eError = ScImportExcel( rMedium, &aDocument, eFormat );
            ScColumn::bDoubleAlloc = FALSE;
            aDocument.UpdateFontCharSet();
            if ( aDocument.IsChartListenerCollectionNeedsUpdate() )
                aDocument.UpdateChartListenerCollection();              //! fuer alle Importe?

            // #75299# all graphics objects must have names
            aDocument.EnsureGraphicNames();

            if (eError == SCWARN_IMPORT_RANGE_OVERFLOW)
            {
                if (!GetError())
                    SetError(eError);
                bRet = TRUE;
            }
            else if (eError != eERR_OK)
            {
                if (!GetError())
                    SetError(eError);
            }
            else
                bRet = TRUE;

            bSetRowHeights = TRUE;      //  #75357# optimal row heights must be updated
        }
        else if (aFltName.EqualsAscii(pFilterAscii))
        {
            SfxItemSet*  pSet = rMedium.GetItemSet();
            const SfxPoolItem* pItem;
            ScAsciiOptions aOptions;
            BOOL bOptInit = FALSE;

            if ( pSet && SFX_ITEM_SET ==
                 pSet->GetItemState( SID_FILE_FILTEROPTIONS, TRUE, &pItem ) )
            {
                aOptions.ReadFromString( ((const SfxStringItem*)pItem)->GetValue() );
                bOptInit = TRUE;
            }

            if ( !bOptInit )
            {
                String aExt = rMedium.GetURLObject().getExtension();
                String aPrivDatName = rMedium.GetURLObject().getName();
                sal_Unicode cAsciiDel;
                if (aExt.EqualsIgnoreCaseAscii("CSV"))
                    cAsciiDel = ',';
                else
                    cAsciiDel = '\t';

                SvStream* pInStream = rMedium.GetInStream();
                if (pInStream)
                {
                    ScImportAsciiDlg* pDlg = new ScImportAsciiDlg(
                                                    NULL,aPrivDatName,
                                                    pInStream, cAsciiDel );
                    int nDlgRet;
                    {   // WarteCursor aus und wieder einschalten
                        ScWaitCursorOff aWait( GetDialogParent() );
                        nDlgRet = pDlg->Execute();
                    }
                    if ( nDlgRet == RET_OK )
                    {
                        pDlg->GetOptions( aOptions );

                        SfxItemSet* pItemSet = rMedium.GetItemSet();
                        if (pItemSet)
                            pItemSet->Put(SfxStringItem(SID_FILE_FILTEROPTIONS, aOptions.WriteToString() ));

                        bOptInit = TRUE;
                    }
                    else
                    {
                        // ConvertFrom lautlos abbrechen
                        SetError( ERRCODE_IO_ABORT );
                        bRet = FALSE;
                    }

                    delete pDlg;
                }
                else
                    DBG_ERROR( "No Stream" );
            }

            if( bOptInit ) // Jetzt gesetzt ?
            {
                FltError eError = eERR_OK;
                BOOL bOverflow = FALSE;

                if( ! rMedium.IsStorage() )
                {
                    ScImportExport  aImpEx( &aDocument );
                    aImpEx.SetExtOptions( aOptions );

                    SvStream* pInStream = rMedium.GetInStream();
                    if (pInStream)
                    {
                        pInStream->SetStreamCharSet( aOptions.GetCharSet() );
                        pInStream->Seek( 0 );
                        bRet = aImpEx.ImportStream( *pInStream );
                        eError = bRet ? eERR_OK : SCERR_IMPORT_CONNECT;
                        aDocument.StartAllListeners();
                        aDocument.SetDirty();
                        bOverflow = aImpEx.IsOverflow();
                    }
                    else
                        DBG_ERROR( "No Stream" );
                }

                if (eError != eERR_OK)
                {
                    if (!GetError())
                        SetError(eError);
                }
                else if ( bOverflow )
                {
                    if (!GetError())
                        SetError(SCWARN_IMPORT_RANGE_OVERFLOW);
                }
                bSetColWidths = TRUE;
                bSetSimpleTextColWidths = TRUE;
            }
        }
        else if (aFltName.EqualsAscii(pFilterDBase))
        {
            String sCharSet;
            String sItStr;
            SfxItemSet*  pSet = rMedium.GetItemSet();
            const SfxPoolItem* pItem;
            if ( pSet && SFX_ITEM_SET ==
                 pSet->GetItemState( SID_FILE_FILTEROPTIONS, TRUE, &pItem ) )
            {
                sItStr = ((const SfxStringItem*)pItem)->GetValue();
            }
            if (sItStr.Len() == 0)
            {
                String aTitle = ScGlobal::GetRscString(STR_IMPORT_DBF);
                sCharSet = String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM( "IBMPC" ));
                ScImportOptions aOptions;
                aOptions.aStrFont = sCharSet;
                aOptions.eCharSet = GetCharsetValue( sCharSet );
                ScImportOptionsDlg* pDlg =
                    new ScImportOptionsDlg( NULL, FALSE, &aOptions, &aTitle );
                int nDlgRet;
                {   // WarteCursor aus und wieder einschalten
                    ScWaitCursorOff aWait( GetDialogParent() );
                    nDlgRet = pDlg->Execute();
                }
                if ( nDlgRet == RET_OK )
                {
                    pDlg->GetImportOptions( aOptions );
                    sCharSet = aOptions.aStrFont;
                    SfxItemSet* pItemSet = rMedium.GetItemSet();
                    if (pItemSet)
                        pItemSet->Put(SfxStringItem(SID_FILE_FILTEROPTIONS, sCharSet));
                    String aBaseName = rMedium.GetURLObject().getBase();
                    String aParString = String::CreateFromAscii(
                                           RTL_CONSTASCII_STRINGPARAM( "Typ=DBF;Dsn=" ));
                    aParString += rMedium.GetPhysicalName();    //  Sdb kann keine URLs !!!
                    aParString.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ";Charset=" ));
                    aParString += sCharSet;
                    aParString.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ";deleted;HDR;" ));
                    ULONG eError = SbaSdbImport( aBaseName, aParString, TRUE, bSimpleColWidth );
                    if (eError != eERR_OK)
                    {
                        if (!GetError())
                            SetError(eError);
                        bRet = ( eError == SCWARN_IMPORT_RANGE_OVERFLOW );
                    }
                    else
                        bRet = TRUE;
                }
                else
                {
                    // ConvertFrom lautlos abbrechen
                    SetError( ERRCODE_IO_ABORT );
                    bRet = FALSE;
                }

                delete pDlg;
            }
            else
            {
                sCharSet = sItStr;
                String aBaseName = rMedium.GetURLObject().getBase();
                String aParString = String::CreateFromAscii(
                                       RTL_CONSTASCII_STRINGPARAM( "Typ=DBF;Dsn=" ));
                aParString += rMedium.GetPhysicalName();    //  Sdb kann keine URLs !!!
                aParString.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ";Charset=" ));
                aParString += sCharSet;
                aParString.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ";deleted;HDR" ));
                ULONG eError = SbaSdbImport( aBaseName, aParString, TRUE, bSimpleColWidth );
                if (eError != eERR_OK)
                {
                    if (!GetError())
                        SetError(eError);
                    bRet = ( eError == SCWARN_IMPORT_RANGE_OVERFLOW );
                }
                else
                    bRet = TRUE;
            }
            aColWidthRange.aStart.SetRow( 1 );  // Spaltenheader nicht
            bSetColWidths = TRUE;
            bSetSimpleTextColWidths = TRUE;
            // Memo-Felder fuehren zu einem bSimpleColWidth[nCol]==FALSE
            for ( USHORT nCol=0; nCol <= MAXCOL && !bSetRowHeights; nCol++ )
            {
                if ( !bSimpleColWidth[nCol] )
                    bSetRowHeights = TRUE;
            }
        }
        else if (aFltName.EqualsAscii(pFilterDif))
        {
            SvStream* pStream = rMedium.GetInStream();
            if (pStream)
            {
                FltError eError;
                String sCharSet;
                String sItStr;
                SfxItemSet*  pSet = rMedium.GetItemSet();
                const SfxPoolItem* pItem;
                if ( pSet && SFX_ITEM_SET ==
                     pSet->GetItemState( SID_FILE_FILTEROPTIONS, TRUE, &pItem ) )
                {
                    sItStr = ((const SfxStringItem*)pItem)->GetValue();
                }

                if (sItStr.Len() == 0)
                {
                    String aTitle = ScGlobal::GetRscString( STR_IMPORT_DIF );
                    sCharSet = String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM( "ANSI" ));
                    ScImportOptions aOptions(59, 34, sCharSet);
                    ScImportOptionsDlg* pDlg =
                        new ScImportOptionsDlg( NULL, FALSE, &aOptions, &aTitle );
                    int nDlgRet;
                    {   // WarteCursor aus und wieder einschalten
                        ScWaitCursorOff aWait( GetDialogParent() );
                        nDlgRet = pDlg->Execute();
                    }
                    if ( RET_OK == nDlgRet )
                    {
                        pDlg->GetImportOptions( aOptions );
                        sCharSet = aOptions.aStrFont;
                        SfxItemSet* pItemSet = rMedium.GetItemSet();
                        if (pItemSet)
                            pItemSet->Put(SfxStringItem(SID_FILE_FILTEROPTIONS, sCharSet));
                        eError = ScImportDif( *pStream, &aDocument, ScAddress(0,0,0),
                                                GetCharsetValue(sCharSet));
                        if (eError != eERR_OK)
                        {
                            if (!GetError())
                                SetError(eError);

                            if( ( eError & ERRCODE_WARNING_MASK ) == ERRCODE_WARNING_MASK )
                                bRet = TRUE;
                        }
                        else
                            bRet = TRUE;
                    }
                    else
                    {
                        // ConvertFrom lautlos abbrechen
                        SetError( ERRCODE_IO_ABORT );
                        bRet = FALSE;
                    }

                    delete pDlg;
                }
                else
                {
                    sCharSet  = sItStr;
                    eError = ScImportDif( *pStream, &aDocument, ScAddress(0,0,0),
                                            GetCharsetValue(sCharSet));
                    if (eError != eERR_OK)
                    {
                        if (!GetError())
                            SetError(eError);

                        if( ( eError & ERRCODE_WARNING_MASK ) == ERRCODE_WARNING_MASK )
                            bRet = TRUE;
                    }
                    else
                        bRet = TRUE;
                }
                rMedium.CloseInStream();
            }
            bSetColWidths = TRUE;
            bSetSimpleTextColWidths = TRUE;
        }
        else if (aFltName.EqualsAscii(pFilterSylk))
        {
            FltError eError = SCERR_IMPORT_UNKNOWN;
            if( !rMedium.IsStorage() )
            {
                ScImportExport aImpEx( &aDocument );

                SvStream* pInStream = rMedium.GetInStream();
                if (pInStream)
                {
                    pInStream->Seek( 0 );
                    bRet = aImpEx.ImportStream( *pInStream, SOT_FORMATSTR_ID_SYLK );
                    eError = bRet ? eERR_OK : SCERR_IMPORT_UNKNOWN;
                    aDocument.StartAllListeners();
                    aDocument.SetDirty();
                }
                else
                    DBG_ERROR( "No Stream" );
            }

            if ( eError != eERR_OK && !GetError() )
                SetError(eError);
            bSetColWidths = TRUE;
            bSetSimpleTextColWidths = TRUE;
        }
        else if (aFltName.EqualsAscii(pFilterRtf))
        {
            FltError eError = SCERR_IMPORT_UNKNOWN;
            if( !rMedium.IsStorage() )
            {
                SvStream* pInStream = rMedium.GetInStream();
                if (pInStream)
                {
                    pInStream->Seek( 0 );
                    ScRange aRange;
                    eError = ScImportRTF( *pInStream, &aDocument, aRange );
                    if (eError != eERR_OK)
                    {
                        if (!GetError())
                            SetError(eError);

                        if( ( eError & ERRCODE_WARNING_MASK ) == ERRCODE_WARNING_MASK )
                            bRet = TRUE;
                    }
                    else
                        bRet = TRUE;
                    aDocument.StartAllListeners();
                    aDocument.SetDirty();
                    bSetColWidths = TRUE;
                    bSetRowHeights = TRUE;
                }
                else
                    DBG_ERROR( "No Stream" );
            }

            if ( eError != eERR_OK && !GetError() )
                SetError(eError);
        }
        else if (aFltName.EqualsAscii(pFilterHtml))
        {
            FltError eError = SCERR_IMPORT_UNKNOWN;
            if( !rMedium.IsStorage() )
            {
                SvStream* pInStream = rMedium.GetInStream();
                if (pInStream)
                {
                    pInStream->Seek( 0 );
                    ScRange aRange;
                    // HTML macht eigenes ColWidth/RowHeight
                    CalcOutputFactor();
                    eError = ScImportHTML( *pInStream, &aDocument, aRange,
                                            GetOutputFactor() );
                    if (eError != eERR_OK)
                    {
                        if (!GetError())
                            SetError(eError);

                        if( ( eError & ERRCODE_WARNING_MASK ) == ERRCODE_WARNING_MASK )
                            bRet = TRUE;
                    }
                    else
                        bRet = TRUE;
                    aDocument.StartAllListeners();
                    aDocument.SetDirty();
                }
                else
                    DBG_ERROR( "No Stream" );
            }

            if ( eError != eERR_OK && !GetError() )
                SetError(eError);
        }
        else
        {
            if (!GetError())
                SetError(SCERR_IMPORT_NI);
        }

        if (!bCalc3)
            aDocument.SetInsertingFromOtherDoc( FALSE );
    }
    else
        DBG_ERROR("Kein Filter bei ConvertFrom");

    InitItems();
    CalcOutputFactor();
    if ( bRet && (bSetColWidths || bSetRowHeights) )
    {   // Spaltenbreiten/Zeilenhoehen anpassen, Basis 100% Zoom
        Fraction aZoom( 1, 1 );
        double nPPTX = ScGlobal::nScreenPPTX * (double) aZoom
            / GetOutputFactor();    // Faktor ist Drucker zu Bildschirm
        double nPPTY = ScGlobal::nScreenPPTY * (double) aZoom;
        VirtualDevice aVirtDev;
        //  all sheets (for Excel import)
        USHORT nTabCount = aDocument.GetTableCount();
        for (USHORT nTab=0; nTab<nTabCount; nTab++)
        {
            USHORT nEndCol, nEndRow;
            aDocument.GetCellArea( nTab, nEndCol, nEndRow );
            aColWidthRange.aEnd.SetCol( nEndCol );
            aColWidthRange.aEnd.SetRow( nEndRow );
            ScMarkData aMark;
            aMark.SetMarkArea( aColWidthRange );
            aMark.MarkToMulti();
            // Reihenfolge erst Breite dann Hoehe ist wichtig (vergl. hund.rtf)
            if ( bSetColWidths )
            {
                for ( USHORT nCol=0; nCol <= nEndCol; nCol++ )
                {
                    USHORT nWidth = aDocument.GetOptimalColWidth(
                        nCol, nTab, &aVirtDev, nPPTX, nPPTY, aZoom, aZoom, FALSE, &aMark,
                        (bSetSimpleTextColWidths && bSimpleColWidth[nCol]) );
                    aDocument.SetColWidth( nCol, nTab,
                        nWidth + (USHORT)ScGlobal::nLastColWidthExtra );
                }
            }
            if ( bSetRowHeights )
            {
                //  nExtra must be 0
                aDocument.SetOptimalHeight( 0, nEndRow, nTab, 0, &aVirtDev,
                    nPPTX, nPPTY, aZoom, aZoom, FALSE );
            }
        }
    }
    FinishedLoading( SFX_LOADED_MAINDOCUMENT | SFX_LOADED_IMAGES );

    // #73762# invalidate eventually temporary table areas
    if ( bRet )
        aDocument.InvalidateTableArea();

    bIsEmpty = FALSE;

    return bRet;
}


void __EXPORT ScDocShell::HandsOff()
{
    SfxInPlaceObject::HandsOff();
}


BOOL __EXPORT ScDocShell::Save()
{
    //  DoEnterHandler hier nicht (wegen AutoSave), ist im ExecuteSave

    ScChartListenerCollection* pCharts = aDocument.GetChartListenerCollection();
    if (pCharts)
        pCharts->UpdateDirtyCharts();                   // Charts, die noch upgedated werden muessen
    if (pAutoStyleList)
        pAutoStyleList->ExecuteAllNow();                // Vorlagen-Timeouts jetzt ausfuehren
    if (eShellMode == SFX_CREATE_MODE_STANDARD)
        SvInPlaceObject::SetVisArea( Rectangle() );     // normal bearbeitet -> keine VisArea

    // #77577# save additionally XML in storage
    if ( GetCreateMode() != SFX_CREATE_MODE_EMBEDDED )
        AddXMLAsZipToTheStorage( *GetStorage() );

    //  wait cursor is handled with progress bar
    BOOL bRet = SfxInPlaceObject::Save();
    if( bRet )
        bRet = SaveCalc( GetStorage() );
    return bRet;
}


BOOL __EXPORT ScDocShell::SaveAs( SvStorage* pStor )
{
    //  DoEnterHandler hier nicht (wegen AutoSave), ist im ExecuteSave

    ScChartListenerCollection* pCharts = aDocument.GetChartListenerCollection();
    if (pCharts)
        pCharts->UpdateDirtyCharts();                   // Charts, die noch upgedated werden muessen
    if (pAutoStyleList)
        pAutoStyleList->ExecuteAllNow();                // Vorlagen-Timeouts jetzt ausfuehren
    if (eShellMode == SFX_CREATE_MODE_STANDARD)
        SvInPlaceObject::SetVisArea( Rectangle() );     // normal bearbeitet -> keine VisArea

    DBG_ASSERT( pStor, "Nanu... SaveAs ohne Storage?" );

    // #77577# save additionally XML in storage
    if ( GetCreateMode() != SFX_CREATE_MODE_EMBEDDED )
        AddXMLAsZipToTheStorage( *pStor );

    //  wait cursor is handled with progress bar
    BOOL bRet = SfxInPlaceObject::SaveAs( pStor );
    if( bRet )
        bRet = SaveCalc( pStor );

    return bRet;
}


BOOL __EXPORT ScDocShell::IsInformationLost()
{
/*
    const SfxFilter *pFilt = GetMedium()->GetFilter();
    BOOL bRet = pFilt && pFilt->IsAlienFormat() && bNoInformLost;
    if (bNoInformLost)                  // nur einmal!!
        bNoInformLost = FALSE;
    return bRet;
*/
    //!!! bei Gelegenheit ein korrekte eigene Behandlung einbauen

    return SfxObjectShell::IsInformationLost();
}


void ScDocShell::AsciiSave( SvStream& rStream, sal_Unicode cDelim, sal_Unicode cStrDelim,
                            CharSet eCharSet)
{
    // could be UTF8 => ByteString, not sal_Char
    ByteString aDelim( cDelim, eCharSet );

    USHORT nStartCol = 0;
    USHORT nStartRow = 0;
    USHORT nTab = GetSaveTab();
    USHORT nEndCol, nEndRow;
    aDocument.GetCellArea( nTab, nEndCol, nEndRow );

    ScProgress aProgress( this, ScGlobal::GetRscString( STR_SAVE_DOC ), nEndRow );

    String aString;
    ByteString aEmptyLine;      // fuer WriteLine mit ConvertEndl

    ScTabViewShell* pViewSh = PTR_CAST(ScTabViewShell, SfxViewShell::Current());
    const ScViewOptions& rOpt = (pViewSh)
                                ? pViewSh->GetViewData()->GetOptions()
                                : aDocument.GetViewOptions();
    BOOL bShowFormulas = rOpt.GetOption( VOPT_FORMULAS );
    BOOL bTabProtect = aDocument.IsTabProtected( nTab );

    USHORT nCol;
    USHORT nRow;
    USHORT nNextCol = nStartCol;
    USHORT nNextRow = nStartRow;
    USHORT nEmptyCol, nEmptyRow;
    SvNumberFormatter& rFormatter = *aDocument.GetFormatTable();

    ScHorizontalCellIterator aIter( &aDocument, nTab, nStartCol, nStartRow,
        nEndCol, nEndRow );
    ScBaseCell* pCell;
    while ( pCell = aIter.GetNext( nCol, nRow ) )
    {
        BOOL bProgress = FALSE;     // nur bei Zeilenwechsel
        if ( nNextRow < nRow )
        {   // leere Zeilen oder/und leere Spalten bis zum Ende der Zeile
            bProgress = TRUE;
            if(cDelim!=0) //@ BugId 55355
                for ( nEmptyCol = nNextCol; nEmptyCol < nEndCol; nEmptyCol++ )
                {   // restliche Spalten der letzten Zeile
                    rStream.Write( aDelim.GetBuffer(), aDelim.Len() );
                }

            rStream.WriteLine( aEmptyLine );        // ConvertEndl
            nNextRow++;
            for ( nEmptyRow = nNextRow; nEmptyRow < nRow; nEmptyRow++ )
            {   // ganze leere Zeilen
                if(cDelim!=0) //@ BugId 55355
                    for ( nEmptyCol = nStartCol; nEmptyCol < nEndCol; nEmptyCol++ )
                    {
                        rStream.Write( aDelim.GetBuffer(), aDelim.Len() );
                    }
                rStream.WriteLine( aEmptyLine );        // ConvertEndl
            }
            if(cDelim!=0) //@ BugId 55355
                for ( nEmptyCol = nStartCol; nEmptyCol < nCol; nEmptyCol++ )
                {   // leere Spalten am Anfang der Zeile
                    rStream.Write( aDelim.GetBuffer(), aDelim.Len() );
                }
            nNextRow = nRow;
        }
        else if ( nNextCol < nCol )
        {   // leere Spalten in gleicher Zeile
            if(cDelim!=0) //@ BugId 55355
                for ( nEmptyCol = nNextCol; nEmptyCol < nCol; nEmptyCol++ )
                {   // dazwischen liegende Spalten
                    rStream.Write( aDelim.GetBuffer(), aDelim.Len() );
                }
        }
        if ( nCol == nEndCol )
        {
            bProgress = TRUE;
            nNextCol = nStartCol;
            nNextRow = nRow + 1;
        }
        else
            nNextCol = nCol + 1;

        CellType eType = pCell->GetCellType();
        if ( bTabProtect )
        {
            const ScProtectionAttr* pProtAttr =
                (const ScProtectionAttr*) aDocument.GetAttr(
                nCol, nRow, nTab, ATTR_PROTECTION );
            if ( pProtAttr->GetHideCell() ||
                        ( eType == CELLTYPE_FORMULA && bShowFormulas &&
                            pProtAttr->GetHideFormula() ) )
                eType = CELLTYPE_NONE;  // ausblenden
        }
        BOOL bString;
        switch ( eType )
        {
            case CELLTYPE_NOTE:
            case CELLTYPE_NONE:
                aString.Erase();
                bString = FALSE;
            break;
            case CELLTYPE_FORMULA :
            {
                USHORT nErrCode;
                if ( bShowFormulas )
                {
                    ((ScFormulaCell*)pCell)->GetFormula( aString );
                    bString = TRUE;
                }
                else if ( nErrCode = ((ScFormulaCell*)pCell)->GetErrCode() )
                {
                    aString = ScGlobal::GetErrorString( nErrCode );
                    bString = TRUE;
                }
                else if ( ((ScFormulaCell*)pCell)->IsValue() )
                {
                    ULONG nFormat;
                    aDocument.GetNumberFormat( nCol, nRow, nTab, nFormat );
                    ScCellFormat::GetInputString( pCell, nFormat, aString, rFormatter );
                    bString = FALSE;
                }
                else
                {
                    ((ScFormulaCell*)pCell)->GetString( aString );
                    bString = TRUE;
                }
            }
            break;
            case CELLTYPE_STRING :
                ((ScStringCell*)pCell)->GetString( aString );
                bString = TRUE;
            break;
            case CELLTYPE_EDIT :
                ((ScEditCell*)pCell)->GetString( aString );
                bString = TRUE;
            break;
            case CELLTYPE_VALUE :
            {
                ULONG nFormat;
                aDocument.GetNumberFormat( nCol, nRow, nTab, nFormat );
                ScCellFormat::GetInputString( pCell, nFormat, aString, rFormatter );
                bString = FALSE;
            }
            break;
            default:
                DBG_ERROR( "ScDocShell::AsciiSave: unknown CellType" );
                aString.Erase();
                bString = FALSE;
        }

        if ( bString )
        {
            if(cStrDelim!=0) //@ BugId 55355
            {
                xub_StrLen nPos = aString.Search( cStrDelim );
                while (nPos != STRING_NOTFOUND)
                {
                    aString.Insert(cStrDelim, nPos);
                    nPos = aString.Search(cStrDelim, nPos+2);
                }
            }
            if(cStrDelim!=0) //@ BugId 55355
            {
                aString.Insert( cStrDelim, 0 );
                aString += cStrDelim;
            }
            ByteString aByteStr( aString, eCharSet );
            rStream.Write( aByteStr.GetBuffer(), aByteStr.Len() );
        }
        else
        {
            ByteString aByteStr( aString, eCharSet );
            rStream.Write( aByteStr.GetBuffer(), aByteStr.Len() );
        }

        if( nCol < nEndCol )
        {
            if(cDelim!=0) //@ BugId 55355
                rStream.Write( aDelim.GetBuffer(), aDelim.Len() );
        }
        else
            rStream.WriteLine( aEmptyLine );        // ConvertEndl

        if ( bProgress )
            aProgress.SetStateOnPercent( nRow );
    }

    // was noch gewuenscht ist leer rausschreiben
    if ( nNextRow <= nEndRow )
    {
        if(cDelim!=0) //@ BugId 55355
            for ( nEmptyCol = nNextCol; nEmptyCol < nEndCol; nEmptyCol++ )
            {   // restliche Spalten der letzten Zeile
                rStream.Write( aDelim.GetBuffer(), aDelim.Len() );
            }
        rStream.WriteLine( aEmptyLine );        // ConvertEndl
        nNextRow++;
    }
    for ( nEmptyRow = nNextRow; nEmptyRow <= nEndRow; nEmptyRow++ )
    {   // ganze leere Zeilen
        if(cDelim!=0) //@ BugId 55355
            for ( nEmptyCol = nStartCol; nEmptyCol < nEndCol; nEmptyCol++ )
            {
                rStream.Write( aDelim.GetBuffer(), aDelim.Len() );
            }
        rStream.WriteLine( aEmptyLine );        // ConvertEndl
    }
}


BOOL __EXPORT ScDocShell::ConvertTo( SfxMedium &rMed )
{
    DoEnterHandler();                                   // nicht abgeschlossene Zelle beenden
    if (pAutoStyleList)
        pAutoStyleList->ExecuteAllNow();                // Vorlagen-Timeouts jetzt ausfuehren
    if (eShellMode == SFX_CREATE_MODE_STANDARD)
        SvInPlaceObject::SetVisArea( Rectangle() );     // normal bearbeitet -> keine VisArea


    DBG_ASSERT( rMed.GetFilter(), "Filter == 0" );

    BOOL bRet = FALSE;
    String aFltName = rMed.GetFilter()->GetFilterName();

/*
    if (aFltName.EqualsAscii(pFilterLotus))
    {
        SvStream* pStream = rMed.GetOutStream();
        if (pStream)
        {
            FltError eError = ScExportLotus123( *pStream, &aDocument, ExpWK1,
                                                CHARSET_IBMPC_437 );
            bRet = eError == eERR_OK;
        }
    }
    else
*/
    if ( aFltName.EqualsAscii(pFilterSc30) || aFltName.EqualsAscii(pFilterSc40) )
    {
        //  Calc3/4 - "Export"
        //! wird nicht gerufen (geht ueber Save/SaveAs) - wird das noch gebraucht ???????

        DBG_ERROR("ConvertTo fuer altes Calc gibt's noch...");
        SvStorage* pStor = rMed.GetStorage();
        if ( pStor )
        {
            //  wait cursor is handled with progress bar
            bRet = SfxInPlaceObject::SaveAs( pStor );
            if( bRet )
                bRet = SaveCalc( pStor );

            String aShortTypeName = String( ScResId(SCSTR_SHORT_SCDOC_NAME) );  // "Dokument"
            if ( aFltName.EqualsAscii(pFilterSc30) )
            {
                //  Ole-Geraffel auf 3.0-Werte setzen:
                SvGlobalName aName( SO3_SC_CLASSID_30 );
                UINT32 nClip = SOT_FORMATSTR_ID_STARCALC_30;
                pStor->SetClass( aName, nClip, aShortTypeName );
            }
            else
            {
                //  Ole-Geraffel auf 4.0-Werte setzen:
                SvGlobalName aName( SO3_SC_CLASSID_40 );
                UINT32 nClip = SOT_FORMATSTR_ID_STARCALC_40;
                pStor->SetClass( aName, nClip, aShortTypeName );
            }
        }
    }
    else if (aFltName.EqualsAscii(pFilterXML))
    {
        ScXMLImportWrapper aImport( aDocument, rMed );
        bRet = aImport.Export();
    }
    else if (aFltName.EqualsAscii(pFilterExcel5) || aFltName.EqualsAscii(pFilterExcel95) ||
             aFltName.EqualsAscii(pFilterExcel97) || aFltName.EqualsAscii(pFilterEx5Temp) ||
             aFltName.EqualsAscii(pFilterEx95Temp) || aFltName.EqualsAscii(pFilterEx97Temp))
    {
        WaitObject aWait( GetDialogParent() );
        BOOL bFake97 = ( aFltName.EqualsAscii(pFilterExcel97) || aFltName.EqualsAscii(pFilterEx97Temp) );
        FltError eError = ScExportExcel5( rMed, &aDocument, bFake97, RTL_TEXTENCODING_MS_1252 );

        if (eError && !GetError())
            SetError(eError);

        if( ( eError & ERRCODE_WARNING_MASK ) == ERRCODE_WARNING_MASK )
            bRet = TRUE;
        else
            bRet = eError == eERR_OK;
    }
    else if (aFltName.EqualsAscii(pFilterAscii))
    {
        SvStream* pStream = rMed.GetOutStream();
        if (pStream)
        {
            String sCharSet;
            sal_Unicode cAsciiDel;
            sal_Unicode cStrDel;
            String sItStr;
            SfxItemSet*  pSet = rMed.GetItemSet();

            BOOL bShowDlg=FALSE;
            BOOL bCancel=FALSE;

            const SfxPoolItem* pDlgItem;
            if (pSet&& SFX_ITEM_SET ==
                 pSet->GetItemState( SID_USE_FILTEROPTIONS, TRUE, &pDlgItem ) )
            {
                bShowDlg=((const SfxBoolItem*)pDlgItem)->GetValue();
            }

            const SfxPoolItem* pItem;
            if ( pSet && SFX_ITEM_SET ==
                 pSet->GetItemState( SID_FILE_FILTEROPTIONS, TRUE, &pItem ) )
            {
                sItStr = ((const SfxStringItem*)pItem)->GetValue();
            }
            if (sItStr.Len() == 0 || bShowDlg)
            {
                if (aDocument.GetTableCount() > 1)
                    if (!rMed.GetError())
                        rMed.SetError(SCWARN_EXPORT_ASCII);
                sCharSet = String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM( "SYSTEM" ));
                cStrDel = '"';
                String aExt = rMed.GetName().Copy(rMed.GetName().Len()-3, 3);
                if (aExt.EqualsIgnoreCaseAscii("CSV"))
                    cAsciiDel = ',';
                else
                    cAsciiDel = '\t';
                ScImportOptions aOptions((USHORT)cAsciiDel, (USHORT)cStrDel, sCharSet);
                String aTitle = ScGlobal::GetRscString( STR_EXPORT_ASCII );
                ScImportOptionsDlg* pDlg =
                    new ScImportOptionsDlg( NULL, TRUE, &aOptions, &aTitle );
                if ( pDlg->Execute() == RET_OK )
                {
                    WaitObject aWait( GetDialogParent() );
                    pDlg->GetImportOptions( aOptions );
                    cAsciiDel = (sal_Unicode) aOptions.nFieldSepCode;
                    cStrDel   = (sal_Unicode) aOptions.nTextSepCode;
                    sCharSet  = aOptions.aStrFont;
                    sItStr    = aOptions.BuildString();
                    SfxItemSet* pItemSet = rMed.GetItemSet();
                    if (pItemSet)
                        pItemSet->Put(SfxStringItem(SID_FILE_FILTEROPTIONS, sItStr));
                    AsciiSave( *pStream, cAsciiDel, cStrDel,
                                GetCharsetValue(sCharSet));
                }
                else
                {
                    bCancel = TRUE;
                    SetError( ERRCODE_IO_ABORT );
                }
                delete pDlg;
            }
            else
            {
                WaitObject aWait( GetDialogParent() );
                ScImportOptions aOptions( sItStr );
                cAsciiDel = (sal_Unicode)aOptions.nFieldSepCode;
                cStrDel   = (sal_Unicode)aOptions.nTextSepCode;
                sCharSet  = aOptions.aStrFont;
                AsciiSave( *pStream, cAsciiDel, cStrDel, GetCharsetValue(sCharSet));
            }
            bRet = !bCancel;
        }
    }
    else if (aFltName.EqualsAscii(pFilterDBase))
    {
        BOOL bCancel = FALSE;
        String sCharSet;
        SfxItemSet* pSet = rMed.GetItemSet();
        const SfxPoolItem* pItem;
        if ( pSet && SFX_ITEM_SET ==
             pSet->GetItemState( SID_FILE_FILTEROPTIONS, TRUE, &pItem ) )
        {
            sCharSet = ((const SfxStringItem*)pItem)->GetValue();

            if ( pSet->GetItemState( SID_USE_FILTEROPTIONS, TRUE, &pItem ) == SFX_ITEM_SET &&
                 ((const SfxBoolItem*)pItem)->GetValue() )
            {
                sCharSet.Erase();       // "edit options" - forget old
            }
        }
        if (sCharSet.Len() == 0)
        {
            sCharSet = String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM( "IBMPC" ));
            ScImportOptions aOptions(59, 34, sCharSet);
            String aTitle = ScGlobal::GetRscString( STR_EXPORT_DBF );
            ScImportOptionsDlg* pDlg =
                new ScImportOptionsDlg( NULL, FALSE, &aOptions, &aTitle );
            if ( pDlg->Execute() == RET_OK )
            {
                pDlg->GetImportOptions( aOptions );
                sCharSet = aOptions.aStrFont;
                SfxItemSet* pItemSet = rMed.GetItemSet();
                if (pItemSet)
                    pItemSet->Put(SfxStringItem(SID_FILE_FILTEROPTIONS, sCharSet));
            }
            else
                bCancel = TRUE;
            delete pDlg;
        }

        if ( bCancel )
        {
            SetError( ERRCODE_IO_ABORT );
        }
        else
        {
            WaitObject aWait( GetDialogParent() );
            String aParString = String::CreateFromAscii(
                                   RTL_CONSTASCII_STRINGPARAM( "Typ=DBF;Dsn=" ));
            DirEntry aTmpFile( rMed.GetPhysicalName() );
            // Sdb kann keine URLs !!!
            // nur Pfad fuer OpenPrivateConnection!
            aParString += aTmpFile.GetPath().GetFull();
            // die Extension braucht's auch noch..
            aParString.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ";Ext=" ));
            aParString += aTmpFile.GetExtension();
            // Dateiname mit vollem Pfad ist laenger als 8 Zeichen...
            aParString.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ";NOFILENAMELIMIT=1" ));
            aParString.AppendAscii(RTL_CONSTASCII_STRINGPARAM( ";Charset=" ));
            aParString += sCharSet;
// HACK damit Sba geoffnetes TempFile ueberschreiben kann
            rMed.CloseOutStream();
            BOOL bHasMemo = FALSE;
            ULONG eError = SbaSdbExport( bHasMemo, aParString, aTmpFile.GetBase() );
            if ( eError != eERR_OK && (eError & ERRCODE_WARNING_MASK) )
            {
                if ( !rMed.GetError() )
                    rMed.SetError( eError );
                eError = eERR_OK;
            }
            else if ( aDocument.GetTableCount() > 1 && !rMed.GetError() )
                rMed.SetError( SCWARN_EXPORT_ASCII );
            if ( bHasMemo )
                aTmpFile.SetExtension( String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("dbt")) );
            if ( eError != eERR_OK )
            {
                if (!GetError())
                    SetError(eError);
                if ( bHasMemo && aTmpFile.Exists() )
                    aTmpFile.Kill();
            }
            else
            {
                bRet = TRUE;
                if ( bHasMemo )
                {
                    SfxStringItem* pNameItem =
                        (SfxStringItem*) rMed.GetItemSet()->GetItem( SID_FILE_NAME );
                    DirEntry aDbtFile( pNameItem->GetValue() );
                    aDbtFile.SetExtension( String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("dbt")) );
                    if ( aDbtFile.Exists() && aDbtFile.Kill() )
                        bRet = FALSE;
                    if ( bRet && aTmpFile.MoveTo( aDbtFile ) )
                        bRet = FALSE;
                    if ( !bRet )
                    {
                        if ( aTmpFile.Exists() )
                            aTmpFile.Kill();
                        if ( !GetError() )
                            SetError( SCERR_EXPORT_DATA );
                    }
                }
            }
        }
    }
    else if (aFltName.EqualsAscii(pFilterDif))
    {
        SvStream* pStream = rMed.GetOutStream();
        if (pStream)
        {
            BOOL bCancel = FALSE;
            String sCharSet;
            String sItStr;
            SfxItemSet*  pSet = rMed.GetItemSet();
            const SfxPoolItem* pItem;
            if ( pSet && SFX_ITEM_SET ==
                 pSet->GetItemState( SID_FILE_FILTEROPTIONS, TRUE, &pItem ) )
            {
                sItStr = ((const SfxStringItem*)pItem)->GetValue();
            }
            if (sItStr.Len() == 0)
            {
                if (aDocument.GetTableCount() > 1)
                    if (!rMed.GetError())
                        rMed.SetError(SCWARN_EXPORT_ASCII);

                sCharSet = String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM( "ANSI" ));
                ScImportOptions aOptions(59, 34, sCharSet);
                String aTitle = ScGlobal::GetRscString( STR_EXPORT_DIF );
                ScImportOptionsDlg* pDlg =
                    new ScImportOptionsDlg( NULL, FALSE, &aOptions, &aTitle );
                if ( pDlg->Execute() == RET_OK )
                {
                    WaitObject aWait( GetDialogParent() );
                    pDlg->GetImportOptions( aOptions );
                    sCharSet  = aOptions.aStrFont;
                    SfxItemSet* pItemSet = rMed.GetItemSet();
                    if (pItemSet)
                        pItemSet->Put(SfxStringItem(SID_FILE_FILTEROPTIONS, sCharSet));
                    USHORT nTab = GetSaveTab();
                    ScExportDif( *pStream, &aDocument, ScAddress(0,0,nTab), GetCharsetValue(sCharSet) );
                }
                else
                {
                    bCancel = TRUE;
                    SetError( ERRCODE_IO_ABORT );
                }
                delete pDlg;
            }
            else
            {
                WaitObject aWait( GetDialogParent() );
                sCharSet  = sItStr;
                ScExportDif( *pStream, &aDocument, ScAddress(0,0,0), GetCharsetValue(sCharSet) );
            }
            bRet = !bCancel;
        }
    }
    else if (aFltName.EqualsAscii(pFilterSylk))
    {
        SvStream* pStream = rMed.GetOutStream();
        if ( pStream )
        {
            WaitObject aWait( GetDialogParent() );

            //  Formeln mit exportieren?

            ScTabViewShell* pViewSh = PTR_CAST(ScTabViewShell, SfxViewShell::Current());
            const ScViewOptions& rOpt = (pViewSh)
                                        ? pViewSh->GetViewData()->GetOptions()
                                        : aDocument.GetViewOptions();
            BOOL bShowFormulas = rOpt.GetOption( VOPT_FORMULAS );

            USHORT nEndCol, nEndRow;
            aDocument.GetCellArea( 0, nEndCol, nEndRow );
            ScRange aRange( 0,0,0, nEndCol,nEndRow,0 );

            ScImportExport aImExport( &aDocument, aRange );
            aImExport.SetFormulas(bShowFormulas);
            bRet = aImExport.ExportStream( *pStream, SOT_FORMATSTR_ID_SYLK );
        }
    }
    else if (aFltName.EqualsAscii(pFilterHtml))
    {
        SvStream* pStream = rMed.GetOutStream();
        if ( pStream )
        {
            WaitObject aWait( GetDialogParent() );
            ScImportExport aImExport( &aDocument );
            aImExport.SetStreamPath( rMed.GetName() );
            bRet = aImExport.ExportStream( *pStream, SOT_FORMATSTR_ID_HTML );
        }
    }
    else
    {
        if (GetError())
            SetError(SCERR_IMPORT_NI);
    }
    return bRet;
}


BOOL __EXPORT ScDocShell::SaveCompleted( SvStorage * pStor )
{
    return SfxInPlaceObject::SaveCompleted( pStor );
}


BOOL __EXPORT ScDocShell::DoSaveCompleted( SfxMedium * pNewStor )
{
    BOOL bRet = SfxObjectShell::DoSaveCompleted( pNewStor );

    //  SC_HINT_DOC_SAVED fuer Wechsel ReadOnly -> Read/Write
    Broadcast( SfxSimpleHint( SC_HINT_DOC_SAVED ) );
    return bRet;
}


USHORT __EXPORT ScDocShell::PrepareClose( BOOL bUI, BOOL bForBrowsing )
{
    if(SC_MOD()->GetCurRefDlgId()>0)
    {
        SfxViewFrame* pFrame = SfxViewFrame::GetFirst( this, TYPE(SfxTopViewFrame) );
        if( pFrame )
        {
            SfxViewShell* p = pFrame->GetViewShell();
            ScTabViewShell* pViewSh = PTR_CAST(ScTabViewShell,p);
            if(pViewSh!=NULL)
            {
                Window *pWin=pViewSh->GetWindow();
                if(pWin!=NULL) pWin->GrabFocus();
            }
        }

        return FALSE;
    }
    if ( aDocument.IsInLinkUpdate() || aDocument.IsInInterpreter() )
    {
        ErrorMessage(STR_CLOSE_ERROR_LINK);
        return FALSE;
    }

    DoEnterHandler();

    USHORT nRet = SfxObjectShell::PrepareClose( bUI, bForBrowsing );
    if (nRet == TRUE)                       // TRUE = schliessen
        aDocument.DisableIdle(TRUE);        // nicht mehr drin rumpfuschen !!!

    return nRet;
}

void ScDocShell::PrepareReload()
{
    SfxObjectShell::PrepareReload();    // tut nichts?

    //  Das Disconnect von DDE-Links kann Reschedule ausloesen.
    //  Wenn die DDE-Links erst im Dokument-dtor geloescht werden, kann beim Reload
    //  aus diesem Reschedule das DDE-Link-Update fuer das neue Dokument ausgeloest
    //  werden. Dabei verklemmt sicht dann irgendwas.
    //  -> Beim Reload die DDE-Links des alten Dokuments vorher disconnecten

    aDocument.DisconnectDdeLinks();
}


String ScDocShell::GetOwnFilterName()           // static
{
    return String::CreateFromAscii(pFilterSc50);
}

BOOL ScDocShell::HasAutomaticTableName( const String& rFilter )     // static
{
    //  TRUE for those filters that keep the default table name
    //  (which is language specific)

    return rFilter.EqualsAscii( pFilterAscii )
        || rFilter.EqualsAscii( pFilterLotus )
        || rFilter.EqualsAscii( pFilterExcel4 )
        || rFilter.EqualsAscii( pFilterEx4Temp )
        || rFilter.EqualsAscii( pFilterDBase )
        || rFilter.EqualsAscii( pFilterDif )
        || rFilter.EqualsAscii( pFilterSylk )
        || rFilter.EqualsAscii( pFilterHtml )
        || rFilter.EqualsAscii( pFilterRtf );
}

//==================================================================

#define __SCDOCSHELL_INIT \
        aDocument       ( SCDOCMODE_DOCUMENT, this ), \
        pUndoManager    ( NULL ), \
        pFontList       ( NULL ), \
        bHeaderOn       ( TRUE ), \
        bFooterOn       ( TRUE ), \
        pDocHelper      ( NULL ), \
        pAutoStyleList  ( NULL ), \
        pOldJobSetup    ( NULL ), \
        pPaintLockData  ( NULL ), \
        nPrtToScreenFactor( 1.0 ), \
        bIsEmpty        ( TRUE ), \
        bIsInUndo       ( FALSE ), \
        bDocumentModifiedPending( FALSE )

//------------------------------------------------------------------

ScDocShell::ScDocShell( const ScDocShell& rShell )
    :   SfxObjectShell( rShell.GetCreateMode() ),
        aDdeTextFmt(String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("TEXT"))),
        bNoInformLost( TRUE ),
        __SCDOCSHELL_INIT
{
    SetPool( &SC_MOD()->GetPool() );

    SetShell(this);
    eShellMode = rShell.eShellMode;
    bIsInplace = rShell.bIsInplace;

    pDocFunc = new ScDocFunc(*this);

    //  SetBaseModel needs exception handling
    ScModelObj::CreateAndSet( this );

    StartListening(*this);
    SfxStyleSheetPool* pStlPool = aDocument.GetStyleSheetPool();
    if (pStlPool)
        StartListening(*pStlPool);

    GetPageOnFromPageStyleSet( NULL, 0, bHeaderOn, bFooterOn );
    SetHelpId( HID_SCSHELL_DOCSH );

    //  InitItems und CalcOutputFactor werden jetzt nach bei Load/ConvertFrom/InitNew gerufen
}

//------------------------------------------------------------------

ScDocShell::ScDocShell( SfxObjectCreateMode eMode )
    :   SfxObjectShell( eMode ),
        aDdeTextFmt(String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("TEXT"))),
        bNoInformLost( TRUE ),
        __SCDOCSHELL_INIT
{
    SetPool( &SC_MOD()->GetPool() );

    SetShell(this);
    eShellMode = eMode;
    bIsInplace = (eShellMode == SFX_CREATE_MODE_EMBEDDED);
    //  wird zurueckgesetzt, wenn nicht inplace

    pDocFunc = new ScDocFunc(*this);

    //  SetBaseModel needs exception handling
    ScModelObj::CreateAndSet( this );

    StartListening(*this);
    SfxStyleSheetPool* pStlPool = aDocument.GetStyleSheetPool();
    if (pStlPool)
        StartListening(*pStlPool);
    SetHelpId( HID_SCSHELL_DOCSH );

    //  InitItems und CalcOutputFactor werden jetzt nach bei Load/ConvertFrom/InitNew gerufen
}

//------------------------------------------------------------------

__EXPORT ScDocShell::~ScDocShell()
{
    ResetDrawObjectShell(); // #55570# falls der Drawing-Layer noch versucht, darauf zuzugreifen

    SfxStyleSheetPool* pStlPool = aDocument.GetStyleSheetPool();
    if (pStlPool)
        EndListening(*pStlPool);
    EndListening(*this);

    delete pAutoStyleList;

    SfxApplication *pSfxApp = SFX_APP();
    if ( pSfxApp->GetDdeService() )             // DDE vor Dokument loeschen
        pSfxApp->RemoveDdeTopic( this );

    delete pDocFunc;
    delete pUndoManager;
    delete pFontList;

    delete pPaintLockData;

    delete pOldJobSetup;        // gesetzt nur bei Fehler in StartJob()
}

//------------------------------------------------------------------

SfxUndoManager* __EXPORT ScDocShell::GetUndoManager()
{
    if (!pUndoManager)
        pUndoManager = new SfxUndoManager;
    return pUndoManager;
}

void ScDocShell::SetModified( BOOL bModified )
{
    SfxInPlaceObject::SetModified( bModified );
    Broadcast( SfxSimpleHint( SFX_HINT_DOCCHANGED ) );
}


void ScDocShell::SetDocumentModified( BOOL bIsModified /* = TRUE */ )
{
    //  BroadcastUno muss auch mit pPaintLockData sofort passieren
    //! auch bei SetDrawModified, wenn Drawing angebunden ist
    //! dann eigener Hint???

    if (bIsModified)
        aDocument.BroadcastUno( SfxSimpleHint( SFX_HINT_DATACHANGED ) );

    if ( pPaintLockData && bIsModified )
    {
        pPaintLockData->SetModified();          // spaeter...
        return;
    }

    SetDrawModified( bIsModified );

    if ( bIsModified )
    {
        if ( aDocument.IsAutoCalcShellDisabled() )
            SetDocumentModifiedPending( TRUE );
        else
        {
            SetDocumentModifiedPending( FALSE );
            aDocument.InvalidateTableArea();
            aDocument.Broadcast( SC_HINT_DATACHANGED, BCA_BRDCST_ALWAYS, NULL );
            if ( aDocument.IsForcedFormulaPending() && aDocument.GetAutoCalc() )
                aDocument.CalcFormulaTree( TRUE );
            PostDataChanged();

            //  automatisches Detektiv-Update

            if ( aDocument.IsDetectiveDirty() )
            {
                aDocument.SetDetectiveDirty(FALSE);     // bei jedem Modified

                ScDetOpList* pList = aDocument.GetDetOpList();
                if ( pList && pList->Count() && !IsInUndo() &&
                        SC_MOD()->GetAppOptions().GetDetectiveAuto() )
                {
                    GetDocFunc().DetectiveRefresh(TRUE);    // TRUE = automatisch erzeugt
                }
            }
        }
    }
}

//  SetDrawModified - ohne Formel-Update
//  (Drawing muss auch beim normalen SetDocumentModified upgedated werden,
//   z.B. bei Tabelle loeschen etc.)

void ScDocShell::SetDrawModified( BOOL bIsModified /* = TRUE */ )
{
    BOOL bUpdate = ( bIsModified != IsModified() );

    SetModified( bIsModified );

    if (bUpdate)
    {
        SfxBindings* pBindings = GetViewBindings();
        if (pBindings)
        {
            pBindings->Invalidate( SID_SAVEDOC );
            pBindings->Invalidate( SID_DOC_MODIFIED );
        }
    }

    if (bIsModified)
    {
        if ( aDocument.IsChartListenerCollectionNeedsUpdate() )
        {
            aDocument.UpdateChartListenerCollection();
            SFX_APP()->Broadcast(SfxSimpleHint( SC_HINT_DRAW_CHANGED ));    // Navigator
        }
        SC_MOD()->AnythingChanged();
    }
}

void ScDocShell::SetInUndo(BOOL bSet)
{
    bIsInUndo = bSet;
}


void ScDocShell::GetDocStat( ScDocStat& rDocStat )
{
    SfxPrinter* pPrinter = GetPrinter();

    aDocument.GetDocStat( rDocStat );
    rDocStat.nPageCount = 0;

    if ( pPrinter )
        for ( USHORT i=0; i<rDocStat.nTableCount; i++ )
            rDocStat.nPageCount +=
                (USHORT) ScPrintFunc( this, pPrinter, i ).GetTotalPages();
}


SfxDocumentInfoDialog* __EXPORT ScDocShell::CreateDocumentInfoDialog(
                                         Window *pParent, const SfxItemSet &rSet )
{
    SfxDocumentInfoDialog* pDlg   = new SfxDocumentInfoDialog( pParent, rSet );
    ScDocShell*            pDocSh = PTR_CAST(ScDocShell,SfxObjectShell::Current());

    //nur mit Statistik, wenn dieses Doc auch angezeigt wird, nicht
    //aus dem Doc-Manager

    if( pDocSh == this )
        pDlg->AddTabPage( 42,
                          ScGlobal::GetRscString( STR_DOC_STAT ),
                          ScDocStatPage::Create,
                          NULL );

    return pDlg;
}

Window* ScDocShell::GetDialogParent()
{
    ScTabViewShell* pViewSh = ScTabViewShell::GetActiveViewShell();
    if ( pViewSh )
        return pViewSh->GetDialogParent();
    else
        return Application::GetDefDialogParent();
}


// --- ScDocShellModificator ------------------------------------------

ScDocShellModificator::ScDocShellModificator( ScDocShell& rDS ) :
        rDocShell( rDS )
{
    ScDocument* pDoc = rDocShell.GetDocument();
    bAutoCalcShellDisabled = pDoc->IsAutoCalcShellDisabled();
    bIdleDisabled = pDoc->IsIdleDisabled();
    pDoc->SetAutoCalcShellDisabled( TRUE );
    pDoc->DisableIdle( TRUE );
}


ScDocShellModificator::~ScDocShellModificator()
{
    ScDocument* pDoc = rDocShell.GetDocument();
    pDoc->SetAutoCalcShellDisabled( bAutoCalcShellDisabled );
    if ( !bAutoCalcShellDisabled && rDocShell.IsDocumentModifiedPending() )
        rDocShell.SetDocumentModified();    // der letzte macht das Licht aus
    pDoc->DisableIdle( bIdleDisabled );
}


void ScDocShellModificator::SetDocumentModified()
{
    ScDocument* pDoc = rDocShell.GetDocument();
    if ( !pDoc->IsImportingXML() )
    {
        // AutoCalcShellDisabled temporaer restaurieren
        BOOL bDisabled = pDoc->IsAutoCalcShellDisabled();
        pDoc->SetAutoCalcShellDisabled( bAutoCalcShellDisabled );
        rDocShell.SetDocumentModified();
        pDoc->SetAutoCalcShellDisabled( bDisabled );
    }
    else
    {
        // uno broadcast is necessary for api to work
        // -> must also be done during xml import
        pDoc->BroadcastUno( SfxSimpleHint( SFX_HINT_DATACHANGED ) );
    }
}


