/*************************************************************************
 *
 *  $RCSfile: xicontent.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: dr $ $Date: 2002-12-04 14:18:17 $
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

// ============================================================================

#ifndef SC_XICONTENT_HXX
#include "xicontent.hxx"
#endif

#ifndef _SV_GRAPH_HXX
#include <vcl/graph.hxx>
#endif
#ifndef _SV_BMPACC_HXX
#include <vcl/bmpacc.hxx>
#endif
#ifndef _SFX_OBJSH_HXX
#include <sfx2/objsh.hxx>
#endif
#ifndef _SFXDOCFILE_HXX
#include <sfx2/docfile.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _MyEDITENG_HXX
#include <svx/editeng.hxx>
#endif
#ifndef _EDITOBJ_HXX
#include <svx/editobj.hxx>
#endif
#ifndef _SVXLINKMGR_HXX
#include <svx/linkmgr.hxx>
#endif
#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif

#ifndef SC_ITEMS_HXX
#include "scitems.hxx"
#endif
#ifndef _EEITEM_HXX
#include <svx/eeitem.hxx>
#endif
#define ITEMID_FIELD EE_FEATURE_FIELD
#ifndef _SFXINTITEM_HXX
#include <svtools/intitem.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX
#include <svx/brshitem.hxx>
#endif
#ifndef _SVX_FLDITEM_HXX
#include <svx/flditem.hxx>
#endif

#ifndef SC_DOCUMENT_HXX
#include "document.hxx"
#endif
#ifndef SC_EDITUTIL_HXX
#include "editutil.hxx"
#endif
#ifndef SC_CELL_HXX
#include "cell.hxx"
#endif
#ifndef SC_VALIDAT_HXX
#include "validat.hxx"
#endif
#ifndef SC_SCPATATR_HXX
#include "patattr.hxx"
#endif
#ifndef SC_SCDOCPOL_HXX
#include "docpool.hxx"
#endif
#ifndef SC_RANGENAM_HXX
#include "rangenam.hxx"
#endif
#ifndef SC_AREALINK_HXX
#include "arealink.hxx"
#endif

#ifndef SC_XISTREAM_HXX
#include "xistream.hxx"
#endif


#include "excform.hxx"


// Shared string table ========================================================

XclImpSst::XclImpSst( const XclImpRoot& rRoot ) :
    XclImpRoot( rRoot ),
    maErrorString( String( RTL_CONSTASCII_USTRINGPARAM( "*** ERROR IN SST ***" ) ) )
{
}

void XclImpSst::ReadSst( XclImpStream& rStrm )
{
    sal_uInt32 nListCount;
    rStrm.Ignore( 4 );
    rStrm >> nListCount;
    while( nListCount && rStrm.IsValid() )
    {
        maStringList.Append( new XclImpString( rStrm ) );
        --nListCount;
    }
}

ScBaseCell* XclImpSst::CreateCell( sal_uInt32 nSstIndex, sal_uInt32 nXFIndex ) const
{
    const XclImpString* pString = GetString( nSstIndex );
    if( !pString )
        pString = &maErrorString;
    return XclImpStringHelper::CreateCell( *this, *pString, nXFIndex );
}


// Background bitmap ==========================================================

void XclImpBitmap::ReadBitmap( XclImpStream& rStrm )
{
    const XclImpRoot& rRoot = rStrm.GetRoot();
    DBG_ASSERT( rRoot.mpRD->pStyleSheetItemSet, "XclImpBitmap::ReadBitmap - no itemset" );
    SfxItemSet& rItemSet = *rRoot.mpRD->pStyleSheetItemSet;

    sal_uInt32 nID;
    sal_uInt16 nWidth, nHeight, nPlanes, nDepth;

    rStrm >> nID;
    rStrm.Ignore( 8 );
    rStrm >> nWidth >> nHeight >> nPlanes >> nDepth;

    DBG_ASSERT( nID == EXC_BITMAP_UNKNOWNID, "XclImpBitmap::ReadBitmap - wrong ID" );
    DBG_ASSERT( nDepth == 24, "XclImpBitmap::ReadBitmap - wrong depth" );
    DBG_ASSERT( nPlanes == 1, "XclImpBitmap::ReadBitmap - wrong plane count" );
    if( rStrm.IsValid() && (nID == EXC_BITMAP_UNKNOWNID) && (nDepth == 24) && (nPlanes == 1) )
    {
        sal_uInt32 nPadding = nWidth % 4;
        if( rStrm.GetRecLeft() == (nWidth * 3UL + nPadding) * nHeight )
        {
            sal_Int32 nVclWidth = nWidth;
            sal_Int32 nVclHeight = nHeight;
            Bitmap aBmp( Size( nVclWidth, nVclHeight ), nDepth );
            BitmapWriteAccess* pAccess = aBmp.AcquireWriteAccess();
            if( pAccess )
            {
                sal_uInt8 nBlue, nGreen, nRed;
                for( sal_Int32 nY = nVclHeight - 1; nY >= 0; --nY )
                {
                    for( sal_Int32 nX = 0; nX < nVclWidth; ++nX )
                    {
                        rStrm >> nBlue >> nGreen >> nRed;
                        pAccess->SetPixel( nY, nX, BitmapColor( nRed, nGreen, nBlue ) );
                    }
                    rStrm.Ignore( nPadding );
                }

                aBmp.ReleaseAccess( pAccess );
                rItemSet.Put( SvxBrushItem( Graphic( aBmp ), GPOS_TILED ) );
            }
        }
        else
            DBG_ERRORFILE( "XclImpBitmap::ReadBitmap - record size invalid" );
    }
}


// Hyperlinks =================================================================

void XclImpHyperlink::AppendString32( String& rString, XclImpStream& rStrm, sal_uInt32 nChars, bool b16Bit )
{
    sal_uInt16 nReadChars = static_cast< sal_uInt16 >( ::std::min( nChars, 0xFFFFUL ) );
    rStrm.AppendRawUniString( rString, nReadChars, b16Bit );
    // ignore remaining chars
    sal_uInt32 nIgnore = nChars - nReadChars;
    if( b16Bit )
        nIgnore *= 2;
    rStrm.Ignore( nIgnore );
}

void XclImpHyperlink::AppendString32( String& rString, XclImpStream& rStrm, bool b16Bit )
{
    AppendString32( rString, rStrm, rStrm.ReaduInt32(), b16Bit );
}

void XclImpHyperlink::IgnoreString32( XclImpStream& rStrm, bool b16Bit )
{
    sal_uInt32 nChars;
    rStrm >> nChars;
    if( b16Bit )
        nChars *= 2;
    rStrm.Ignore( nChars );
}

void XclImpHyperlink::GetAbsPath( String& rPath, sal_uInt16 nLevel, SfxObjectShell* pDocShell )
{
    String aTmpStr;
    while( nLevel )
    {
        aTmpStr.AppendAscii( "../" );
        --nLevel;
    }
    aTmpStr += rPath;

    if( pDocShell )
    {
        bool bWasAbs = false;
        rPath = pDocShell->GetMedium()->GetURLObject().smartRel2Abs( aTmpStr, bWasAbs ).GetMainURL( INetURLObject::NO_DECODE );
        // full path as stored in SvxURLField must be encoded
    }
    else
        rPath = aTmpStr;
}

void XclImpHyperlink::InsertUrl( const XclImpRoot& rRoot, const String& rURL, sal_uInt16 nCol, sal_uInt16 nRow )
{
    ScDocument& rDoc = rRoot.GetDoc();
    sal_uInt16 nTab = rRoot.GetScTab();
    ScAddress aPos( nCol, nRow, nTab );
    CellType eCellType = rDoc.GetCellType( aPos );

    // hyperlinks in value/formula cells not supported
    if( (eCellType != CELLTYPE_FORMULA) && (eCellType != CELLTYPE_VALUE) )
    {
        String aOrigText;
        rDoc.GetString( nCol, nRow, nTab, aOrigText );
        if( !aOrigText.Len() )
            aOrigText = rURL;

        EditEngine& rEE = rRoot.GetEditEngine();
        rEE.SetText( EMPTY_STRING );

        SvxURLField aUrlField( rURL, aOrigText, SVXURLFORMAT_APPDEFAULT );
        rEE.QuickInsertField( SvxFieldItem( aUrlField ), ESelection( 0xFFFF, 0xFFFF ) );
        ::std::auto_ptr< EditTextObject > pTextObj( rEE.CreateTextObject() );

        ScEditCell* pCell = new ScEditCell( pTextObj.get(), &rDoc, rEE.GetEditTextObjectPool() );
        rDoc.PutCell( aPos, pCell );
    }
}

void XclImpHyperlink::ReadHlink( XclImpStream& rStrm )
{
    const XclImpRoot& rRoot = rStrm.GetRoot();
    DBG_ASSERT_BIFF( rRoot.GetBiff() == xlBiff8 );

    ScDocument& rDoc = rRoot.GetDoc();
    SfxObjectShell* pDocShell = rRoot.GetDocShell();

    sal_uInt16 nRow1, nRow2, nCol1, nCol2;
    sal_uInt32 nFlags;
    XclGuid aGuid;

    rStrm >> nRow1 >> nRow2 >> nCol1 >> nCol2 >> aGuid;
    rStrm.Ignore( 4 );
    rStrm >> nFlags;

    DBG_ASSERT( aGuid == XclTools::maGuidStdLink, "XclImpHyperlink::ReadHlink - unknown header GUID" );

    sal_uInt16 nLevel = 0;                  // counter for level to climb down in path
    ::std::auto_ptr< String > pLongName;    // link / file name
    ::std::auto_ptr< String > pShortName;   // 8.3-representation of file name
    ::std::auto_ptr< String > pTextMark;    // text mark

    // description (ignore)
    if( ::get_flag( nFlags, EXC_HLINK_DESCR ) )
        IgnoreString32( rStrm, true );
    // target frame (ignore) !! DESCR/FRAME - is this the right order? (never seen them together)
    if( ::get_flag( nFlags, EXC_HLINK_FRAME ) )
        IgnoreString32( rStrm, true );

    // UNC path
    if( ::get_flag( nFlags, EXC_HLINK_UNC ) )
    {
        pLongName.reset( new String );
        AppendString32( *pLongName, rStrm, true );
        GetAbsPath( *pLongName, 0, pDocShell );
    }
    // file link or URL
    else if( ::get_flag( nFlags, EXC_HLINK_BODY ) )
    {
        rStrm >> aGuid;

        if( aGuid == XclTools::maGuidFileMoniker )
        {
            rStrm >> nLevel;
            pShortName.reset( new String );
            AppendString32( *pShortName, rStrm, false );
            rStrm.Ignore( 24 );

            sal_uInt32 nStrLen;
            rStrm >> nStrLen;
            if( nStrLen )
            {
                rStrm >> nStrLen;
                nStrLen /= 2;       // it's byte count here...
                rStrm.Ignore( 2 );
                pLongName.reset( new String );
                AppendString32( *pLongName, rStrm, nStrLen, true );
                GetAbsPath( *pLongName, nLevel, pDocShell );
            }
            else
                GetAbsPath( *pShortName, nLevel, pDocShell );
        }
        else if( aGuid == XclTools::maGuidUrlMoniker )
        {
            sal_uInt32 nStrLen;
            rStrm >> nStrLen;
            nStrLen /= 2;       // it's byte count here...
            pLongName.reset( new String );
            AppendString32( *pLongName, rStrm, nStrLen, true );
            if( !::get_flag( nFlags, EXC_HLINK_ABS ) )
                GetAbsPath( *pLongName, 0, pDocShell );
        }
        else
            DBG_ERRORFILE( "XclImpHyperlink::ReadHlink - unknown content GUID" );
    }

    // text mark
    if( ::get_flag( nFlags, EXC_HLINK_MARK ) )
    {
        pTextMark.reset( new String );
        AppendString32( *pTextMark, rStrm, true );
    }

    DBG_ASSERT( !rStrm.GetRecLeft(), "XclImpHyperlink::ReadHlink - record size mismatch" );

    if( !pLongName.get() && pShortName.get() )
        pLongName = pShortName;
    else if( !pLongName.get() && pTextMark.get() )
        pLongName.reset( new String );

    if( pLongName.get() )
    {
        if( pTextMark.get() )
        {
            if( !pLongName->Len() )
                pTextMark->SearchAndReplaceAll( '!', '.' );
            *pLongName += '#';
            *pLongName += *pTextMark;
        }

        sal_uInt16 nTab = rRoot.GetScTab();
        ScRange aRange( nCol1, nRow1, nTab, nCol2, nRow2, nTab );
        if( rRoot.CheckCellRange( aRange ) )
        {
            aRange.GetVars( nCol1, nRow1, nTab, nCol2, nRow2, nTab );
            for( sal_uInt16 nCol = nCol1; nCol <= nCol2 ; ++nCol )
                for( sal_uInt16 nRow = nRow1; nRow <= nRow2; ++nRow )
                    InsertUrl( rRoot, *pLongName, nCol, nRow );
        }
    }
}


// Label ranges ===============================================================

void XclImpLabelranges::ReadLabelranges( XclImpStream& rStrm )
{
    const XclImpRoot& rRoot = rStrm.GetRoot();
    DBG_ASSERT_BIFF( rRoot.GetBiff() == xlBiff8 );

    ScDocument& rDoc = rRoot.GetDoc();
    sal_uInt16 nTab = rRoot.GetScTab();
    ScRangeList aRanges;
    ScRangePairListRef xLabelRangesRef;
    const ScRange* pRange;

    // row label ranges
    xLabelRangesRef = rDoc.GetRowNameRangesRef();

    rStrm >> aRanges;
    rRoot.CheckCellRangeList( aRanges );
    for( pRange = aRanges.First(); pRange; pRange = aRanges.Next() )
    {
        ScRange aDataRange( *pRange );
        if( aDataRange.aEnd.Col() < MAXCOL )
        {
            aDataRange.aStart.SetCol( aDataRange.aEnd.Col() + 1 );
            aDataRange.aEnd.SetCol( MAXCOL );
        }
        else if( aDataRange.aStart.Col() > 0 )
        {
            aDataRange.aEnd.SetCol( aDataRange.aStart.Col() - 1 );
            aDataRange.aStart.SetCol( 0 );
        }
        xLabelRangesRef->Append( ScRangePair( *pRange, aDataRange ) );
    }

    // column label ranges
    xLabelRangesRef = rDoc.GetColNameRangesRef();

    aRanges.RemoveAll();
    rStrm >> aRanges;
    rRoot.CheckCellRangeList( aRanges );
    for( pRange = aRanges.First(); pRange; pRange = aRanges.Next() )
    {
        ScRange aDataRange( *pRange );
        if( aDataRange.aEnd.Row() < MAXROW )
        {
            aDataRange.aStart.SetRow( aDataRange.aEnd.Row() + 1 );
            aDataRange.aEnd.SetRow( MAXROW );
        }
        else if( aDataRange.aStart.Row() > 0 )
        {
            aDataRange.aEnd.SetRow( aDataRange.aStart.Row() - 1 );
            aDataRange.aStart.SetRow( 0 );
        }
        xLabelRangesRef->Append( ScRangePair( *pRange, aDataRange ) );
    }
}


// Data Validation ============================================================

void XclImpValidation::ReadDval( XclImpStream& rStrm, XclImpRoot& rRoot )
{
    DBG_ASSERT_BIFF( rRoot.GetBiff() == xlBiff8 );

    sal_uInt32 nObjId;
    rStrm.Ignore( 10 );
    rStrm >> nObjId;
    if( nObjId != EXC_DVAL_NOOBJ )
        rRoot.SetIgnoreObject( nObjId );
}

void XclImpValidation::ReadDv( XclImpStream& rStrm, ExcelToSc& rFmlaConv )
{
    const XclImpRoot& rRoot = rStrm.GetRoot();
    DBG_ASSERT_BIFF( rRoot.GetBiff() == xlBiff8 );

    ScDocument& rDoc = rRoot.GetDoc();
    sal_uInt16 nTab = rRoot.GetScTab();

    // flags
    sal_uInt32 nFlags;
    rStrm >> nFlags;

    // messages
    String aPromptTitle, aErrorTitle, aPromptMessage, aErrorMessage;
    rStrm.AppendUniString( aPromptTitle );
    rStrm.AppendUniString( aErrorTitle );
    rStrm.AppendUniString( aPromptMessage );
    rStrm.AppendUniString( aErrorMessage );

    // formula(s)
    if( rStrm.GetRecLeft() > 8 )
    {
        sal_uInt16 nLen;

        // first formula
        const ScTokenArray* pFmla1 = NULL;
        rStrm >> nLen;
        rStrm.Ignore( 2 );
        if( nLen )
        {
            rFmlaConv.Reset();
            rFmlaConv.Convert( pFmla1, nLen, FT_RangeName );
        }
        // formula converter owns pFmla1 -> create a copy of the token array
        ::std::auto_ptr< const ScTokenArray > pFmla1AutoPtr( pFmla1 ? pFmla1->Clone() : NULL );
        pFmla1 = pFmla1AutoPtr.get();

        // second formula
        const ScTokenArray* pFmla2 = NULL;
        rStrm >> nLen;
        rStrm.Ignore( 2 );
        if( nLen )
        {
            rFmlaConv.Reset();
            rFmlaConv.Convert( pFmla2, nLen, FT_RangeName );
        }
        // we do not own pFmla2

        // read all cell ranges
        ScRangeList aRanges;
        rStrm >> aRanges;
        rRoot.CheckCellRangeList( aRanges );

        if( aRanges.Count() )
        {
            bool bIsValid = true;   // valid settings in flags field

            ScValidationMode eValMode;
            switch( nFlags & EXC_DV_MODE_MASK )
            {
                case EXC_DV_MODE_ANY:       eValMode = SC_VALID_ANY;        break;
                case EXC_DV_MODE_WHOLE:     eValMode = SC_VALID_WHOLE;      break;
                case EXC_DV_MODE_DECIMAL:   eValMode = SC_VALID_DECIMAL;    break;
                case EXC_DV_MODE_LIST:      eValMode = SC_VALID_LIST;       break;
                case EXC_DV_MODE_DATE:      eValMode = SC_VALID_DATE;       break;
                case EXC_DV_MODE_TIME:      eValMode = SC_VALID_TIME;       break;
                case EXC_DV_MODE_TEXTLEN:   eValMode = SC_VALID_TEXTLEN;    break;
                case EXC_DV_MODE_CUSTOM:    eValMode = SC_VALID_CUSTOM;     break;
                default:                    bIsValid = false;
            }

            ScConditionMode eCondMode;
            switch( nFlags & EXC_DV_COND_MASK )
            {
                case EXC_DV_COND_BETWEEN:   eCondMode = SC_COND_BETWEEN;    break;
                case EXC_DV_COND_NOTBETWEEN:eCondMode = SC_COND_NOTBETWEEN; break;
                case EXC_DV_COND_EQUAL:     eCondMode = SC_COND_EQUAL;      break;
                case EXC_DV_COND_NOTEQUAL:  eCondMode = SC_COND_NOTEQUAL;   break;
                case EXC_DV_COND_GREATER:   eCondMode = SC_COND_GREATER;    break;
                case EXC_DV_COND_LESS:      eCondMode = SC_COND_LESS;       break;
                case EXC_DV_COND_EQGREATER: eCondMode = SC_COND_EQGREATER;  break;
                case EXC_DV_COND_EQLESS:    eCondMode = SC_COND_EQLESS;     break;
                default:                    bIsValid = false;
            }

            // first range for base address for relative references
            const ScRange* pRange = aRanges.GetObject( 0 );
            if( bIsValid && pRange )
            {
                ScValidationData aValidData( eValMode, eCondMode, pFmla1, pFmla2, &rDoc, pRange->aStart );

                aValidData.SetIgnoreBlank( ::get_flag( nFlags, EXC_DV_IGNOREBLANK ) );

                // *** prompt box ***
                if( aPromptTitle.Len() || aPromptMessage.Len() )
                {
                    // set any text stored in the record
                    aValidData.SetInput( aPromptTitle, aPromptMessage );
                    if( !::get_flag( nFlags, EXC_DV_SHOWPROMPT ) )
                        aValidData.ResetInput();
                }

                // *** error box ***
                ScValidErrorStyle eErrStyle = SC_VALERR_STOP;
                switch( nFlags & EXC_DV_ERROR_MASK )
                {
                    case EXC_DV_ERROR_WARNING:  eErrStyle = SC_VALERR_WARNING;  break;
                    case EXC_DV_ERROR_INFO:     eErrStyle = SC_VALERR_INFO;     break;
                }
                // set texts and error style
                aValidData.SetError( aErrorTitle, aErrorMessage, eErrStyle );
                if( !::get_flag( nFlags, EXC_DV_SHOWERROR ) )
                    aValidData.ResetError();

                // set the handle ID
                sal_uInt32 nHandle = rDoc.AddValidationEntry( aValidData );
                ScPatternAttr aPattern( rDoc.GetPool() );
                aPattern.GetItemSet().Put( SfxUInt32Item( ATTR_VALIDDATA, nHandle ) );

                // apply all ranges
                for( pRange = aRanges.First(); pRange; pRange = aRanges.Next() )
                    rDoc.ApplyPatternAreaTab( pRange->aStart.Col(), pRange->aStart.Row(),
                        pRange->aEnd.Col(), pRange->aEnd.Row(), nTab, aPattern );
            }
        }
    }
}


// Web queries ================================================================

XclImpWebQuery::XclImpWebQuery( const ScRange& rDestRange ) :
    maDestRange( rDestRange ),
    meMode( xlWQUnknown ),
    mnRefresh( 0 )
{
}

void XclImpWebQuery::ReadParamqry( XclImpStream& rStrm )
{
    if( ::get_flag( rStrm.ReaduInt16(), EXC_PQRY_TABLES ) )
    {
        meMode = xlWQAllTables;
        maTables = ScfTools::GetHTMLTablesName();
    }
    else
    {
        meMode = xlWQDocument;
        maTables = ScfTools::GetHTMLDocName();
    }
}

void XclImpWebQuery::ReadSxstring( XclImpStream& rStrm )
{
    maURL.Erase();
    rStrm.AppendUniString( maURL );
}

void XclImpWebQuery::ReadWqsettings( XclImpStream& rStrm )
{
    sal_uInt16 nFlags;
    rStrm.Ignore( 10 );
    rStrm >> nFlags;
    rStrm.Ignore( 10 );
    rStrm >> mnRefresh;

    if( ::get_flag( nFlags, EXC_WQSETT_SPECTABLES ) && (meMode == xlWQAllTables) )
        meMode = xlWQSpecTables;
}

void XclImpWebQuery::ReadWqtables( XclImpStream& rStrm )
{
    if( meMode == xlWQSpecTables )
    {
        String aTables;
        rStrm.Ignore( 4 );
        rStrm.AppendUniString( aTables );

        const sal_Unicode cSep = ';';
        aTables.SearchAndReplaceAll( ',', cSep );
        String aQuotedPairs( RTL_CONSTASCII_USTRINGPARAM( "\"\"" ) );
        xub_StrLen nTokenCnt = aTables.GetQuotedTokenCount( aQuotedPairs, cSep );
        maTables.Erase();
        xub_StrLen nStringIx = 0;
        for( xub_StrLen nToken = 0; nToken < nTokenCnt; ++nToken )
        {
            String aToken( aTables.GetQuotedToken( 0, aQuotedPairs, cSep, nStringIx ) );
            sal_Int32 nTabNum = CharClass::isAsciiNumeric( aToken ) ? aToken.ToInt32() : 0;
            if( nTabNum > 0 )
                ScfTools::AddToken( maTables, ScfTools::GetNameFromHTMLIndex( static_cast< sal_uInt32 >( nTabNum ) ), cSep );
            else
            {
                ScfTools::EraseQuotes( aToken );
                if( aToken.Len() )
                    ScfTools::AddToken( maTables, ScfTools::GetNameFromHTMLName( aToken ), cSep );
            }
        }
    }
}

void XclImpWebQuery::Apply( ScDocument& rDoc, const String& rFilterName )
{
    if( maURL.Len() && (meMode != xlWQUnknown) && rDoc.GetDocumentShell() )
    {
        ScAreaLink* pLink = new ScAreaLink( rDoc.GetDocumentShell(),
            maURL, rFilterName, EMPTY_STRING, maTables, maDestRange, mnRefresh * 60UL );
        rDoc.GetLinkManager()->InsertFileLink( *pLink, OBJECT_CLIENT_FILE,
            maURL, &rFilterName, &maTables );
    }
}


// ----------------------------------------------------------------------------

void XclImpWebQueryBuffer::ReadQsi( XclImpStream& rStrm )
{
    if( GetBiff() == xlBiff8 )
    {
        rStrm.Ignore( 10 );
        String aName;
        rStrm.AppendUniString( aName );

        ScRangeName& rRangeNames = GetNamedRanges();
        sal_uInt16 nIndex;
        if( rRangeNames.SearchName( aName, nIndex ) )
        {
            const ScRangeData* pRangeData = rRangeNames[ nIndex ];
            ScRange aRange;
            if( pRangeData && pRangeData->IsReference( aRange ) )
                maWQList.Append( new XclImpWebQuery( aRange ) );
        }
    }
    else
        DBG_ERROR_BIFF();
}

void XclImpWebQueryBuffer::ReadParamqry( XclImpStream& rStrm )
{
    XclImpWebQuery* pQuery = maWQList.Last();
    if( pQuery )
        pQuery->ReadParamqry( rStrm );
}

void XclImpWebQueryBuffer::ReadSxstring( XclImpStream& rStrm )
{
    XclImpWebQuery* pQuery = maWQList.Last();
    if( pQuery )
        pQuery->ReadSxstring( rStrm );
}

void XclImpWebQueryBuffer::ReadWqsettings( XclImpStream& rStrm )
{
    XclImpWebQuery* pQuery = maWQList.Last();
    if( pQuery )
        pQuery->ReadWqsettings( rStrm );
}

void XclImpWebQueryBuffer::ReadWqtables( XclImpStream& rStrm )
{
    XclImpWebQuery* pQuery = maWQList.Last();
    if( pQuery )
        pQuery->ReadWqtables( rStrm );
}

void XclImpWebQueryBuffer::Apply()
{
    ScDocument& rDoc = GetDoc();
    String aFilterName( RTL_CONSTASCII_USTRINGPARAM( EXC_WEBQRY_FILTER ) );
    for( XclImpWebQuery* pQuery = maWQList.First(); pQuery; pQuery = maWQList.Next() )
        pQuery->Apply( rDoc, aFilterName );
}


// ============================================================================

