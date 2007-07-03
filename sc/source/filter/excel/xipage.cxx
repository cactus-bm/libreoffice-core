/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xipage.cxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-03 15:51:05 $
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

#ifndef SC_XIPAGE_HXX
#include "xipage.hxx"
#endif

#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif
#ifndef _SV_GRAPH_HXX
#include <vcl/graph.hxx>
#endif
#ifndef _SV_BMPACC_HXX
#include <vcl/bmpacc.hxx>
#endif

#ifndef SC_ITEMS_HXX
#include "scitems.hxx"
#endif
#ifndef _SFXENUMITEM_HXX
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXINTITEM_HXX
#include <svtools/intitem.hxx>
#endif
#ifndef _SVX_PAGEITEM_HXX
#include <svx/pageitem.hxx>
#endif
#ifndef _SVX_SIZEITEM_HXX
#include <svx/sizeitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX
#include <svx/ulspitem.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX
#include <svx/brshitem.hxx>
#endif

#ifndef SC_DOCUMENT_HXX
#include "document.hxx"
#endif
#ifndef SC_STLSHEET_HXX
#include "stlsheet.hxx"
#endif
#ifndef SC_SCATTR_HXX
#include "attrib.hxx"
#endif

#ifndef SC_XISTREAM_HXX
#include "xistream.hxx"
#endif
#ifndef SC_XIHELPER_HXX
#include "xihelper.hxx"
#endif

// Page settings ==============================================================

XclImpPageSettings::XclImpPageSettings( const XclImpRoot& rRoot ) :
    XclImpRoot( rRoot )
{
    Initialize();
}

void XclImpPageSettings::Initialize()
{
    maData.SetDefaults();
    mbValidPaper = false;
}

void XclImpPageSettings::ReadSetup( XclImpStream& rStrm )
{
    DBG_ASSERT_BIFF( GetBiff() >= EXC_BIFF4 );
    if( GetBiff() < EXC_BIFF4 )
        return;

    // BIFF4 - BIFF8
    sal_uInt16 nFlags;
    rStrm   >> maData.mnPaperSize >> maData.mnScaling >> maData.mnStartPage
            >> maData.mnFitToWidth >> maData.mnFitToHeight >> nFlags;

    mbValidPaper = maData.mbValid = !::get_flag( nFlags, EXC_SETUP_INVALID );
    maData.mbPrintInRows = ::get_flag( nFlags, EXC_SETUP_INROWS );
    maData.mbPortrait = ::get_flag( nFlags, EXC_SETUP_PORTRAIT );
    maData.mbBlackWhite = ::get_flag( nFlags, EXC_SETUP_BLACKWHITE );
    maData.mbManualStart = true;

    // new in BIFF5 - BIFF8
    if( GetBiff() >= EXC_BIFF5 )
    {
        rStrm   >> maData.mnHorPrintRes >> maData.mnVerPrintRes
                >> maData.mfHeaderMargin >> maData.mfFooterMargin >> maData.mnCopies;

        maData.mbDraftQuality = ::get_flag( nFlags, EXC_SETUP_DRAFT );
        maData.mbPrintNotes = ::get_flag( nFlags, EXC_SETUP_PRINTNOTES );
        maData.mbManualStart = ::get_flag( nFlags, EXC_SETUP_STARTPAGE );
    }
}

void XclImpPageSettings::ReadMargin( XclImpStream& rStrm )
{
    switch( rStrm.GetRecId() )
    {
        case EXC_ID_LEFTMARGIN:     rStrm >> maData.mfLeftMargin;   break;
        case EXC_ID_RIGHTMARGIN:    rStrm >> maData.mfRightMargin;  break;
        case EXC_ID_TOPMARGIN:      rStrm >> maData.mfTopMargin;    break;
        case EXC_ID_BOTTOMMARGIN:   rStrm >> maData.mfBottomMargin; break;
        default:    DBG_ERRORFILE( "XclImpPageSettings::ReadMargin - unknown record" );
    }
}

void XclImpPageSettings::ReadCenter( XclImpStream& rStrm )
{
    DBG_ASSERT_BIFF( GetBiff() >= EXC_BIFF3 );  // read it anyway
    bool bCenter = (rStrm.ReaduInt16() != 0);
    switch( rStrm.GetRecId() )
    {
        case EXC_ID_HCENTER:    maData.mbHorCenter = bCenter;   break;
        case EXC_ID_VCENTER:    maData.mbVerCenter = bCenter;   break;
        default:    DBG_ERRORFILE( "XclImpPageSettings::ReadCenter - unknown record" );
    }
}

void XclImpPageSettings::ReadHeaderFooter( XclImpStream& rStrm )
{
    String aString;
    if( rStrm.GetRecLeft() )
        aString = (GetBiff() <= EXC_BIFF5) ? rStrm.ReadByteString( false ) : rStrm.ReadUniString();

    switch( rStrm.GetRecId() )
    {
        case EXC_ID_HEADER:     maData.maHeader = aString;  break;
        case EXC_ID_FOOTER:     maData.maFooter = aString;  break;
        default:    DBG_ERRORFILE( "XclImpPageSettings::ReadHeaderFooter - unknown record" );
    }
}

void XclImpPageSettings::ReadPageBreaks( XclImpStream& rStrm )
{
    ScfUInt16Vec* pVec = 0;
    switch( rStrm.GetRecId() )
    {
        case EXC_ID_HORPAGEBREAKS:  pVec = &maData.maHorPageBreaks;     break;
        case EXC_ID_VERPAGEBREAKS:  pVec = &maData.maVerPageBreaks;     break;
        default:    DBG_ERRORFILE( "XclImpPageSettings::ReadPageBreaks - unknown record" );
    }

    if( pVec )
    {
        bool bIgnore = GetBiff() == EXC_BIFF8;  // ignore start/end columns or rows in BIFF8

        sal_uInt16 nCount, nBreak;
        rStrm >> nCount;
        pVec->clear();
        pVec->reserve( nCount );

        while( nCount-- )
        {
            rStrm >> nBreak;
            if( nBreak )
                pVec->push_back( nBreak );
            if( bIgnore )
                rStrm.Ignore( 4 );
        }
    }
}

void XclImpPageSettings::ReadPrintHeaders( XclImpStream& rStrm )
{
    maData.mbPrintHeadings = (rStrm.ReaduInt16() != 0);
}

void XclImpPageSettings::ReadPrintGridLines( XclImpStream& rStrm )
{
    maData.mbPrintGrid = (rStrm.ReaduInt16() != 0);
}

void XclImpPageSettings::ReadBitmap( XclImpStream& rStrm )
{
    sal_uInt32 nID;
    sal_uInt16 nWidth, nHeight, nPlanes, nDepth;

    rStrm >> nID;
    rStrm.Ignore( 8 );
    rStrm >> nWidth >> nHeight >> nPlanes >> nDepth;

    DBG_ASSERT( nID == EXC_BITMAP_UNKNOWNID, "XclImpPageSettings::ReadBitmap - wrong ID" );
    DBG_ASSERT( nDepth == 24, "XclImpPageSettings::ReadBitmap - wrong depth" );
    DBG_ASSERT( nPlanes == 1, "XclImpPageSettings::ReadBitmap - wrong plane count" );
    if( rStrm.IsValid() && (nID == EXC_BITMAP_UNKNOWNID) && (nDepth == 24) && (nPlanes == 1) )
    {
        sal_Size nPadding = nWidth % 4;
        if( rStrm.GetRecLeft() == (nWidth * 3 + nPadding) * nHeight )
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
                maData.mxBrushItem.reset( new SvxBrushItem( Graphic( aBmp ), GPOS_TILED, ATTR_BACKGROUND ) );
            }
        }
        else
            DBG_ERRORFILE( "XclImpPageSettings::ReadBitmap - record size invalid" );
    }
}

void XclImpPageSettings::SetPaperSize( sal_uInt16 nXclPaperSize, bool bPortrait )
{
    maData.mnPaperSize = nXclPaperSize;
    maData.mbPortrait = bPortrait;
    mbValidPaper = true;
}

// ----------------------------------------------------------------------------

namespace {

void lclPutMarginItem( SfxItemSet& rItemSet, sal_uInt16 nRecId, double fMarginInch )
{
    sal_uInt16 nMarginTwips = XclTools::GetTwipsFromInch( fMarginInch );
    switch( nRecId )
    {
        case EXC_ID_TOPMARGIN:
        case EXC_ID_BOTTOMMARGIN:
        {
            SvxULSpaceItem aItem( GETITEM( rItemSet, SvxULSpaceItem, ATTR_ULSPACE ) );
            if( nRecId == EXC_ID_TOPMARGIN )
                aItem.SetUpperValue( nMarginTwips );
            else
                aItem.SetLowerValue( nMarginTwips );
            rItemSet.Put( aItem );
        }
        break;
        case EXC_ID_LEFTMARGIN:
        case EXC_ID_RIGHTMARGIN:
        {
            SvxLRSpaceItem aItem( GETITEM( rItemSet, SvxLRSpaceItem, ATTR_LRSPACE ) );
            if( nRecId == EXC_ID_LEFTMARGIN )
                aItem.SetLeftValue( nMarginTwips );
            else
                aItem.SetRightValue( nMarginTwips );
            rItemSet.Put( aItem );
        }
        break;
        default:
            DBG_ERRORFILE( "XclImpPageSettings::SetMarginItem - unknown record id" );
    }
}

} // namespace

void XclImpPageSettings::Finalize()
{
    ScDocument& rDoc = GetDoc();
    SCTAB nScTab = GetCurrScTab();

    // *** create page style sheet ***

    String aStyleName( RTL_CONSTASCII_USTRINGPARAM( "PageStyle_" ) );
    String aTableName;
    if( GetDoc().GetName( nScTab, aTableName ) )
        aStyleName.Append( aTableName );
    else
        aStyleName.Append( String::CreateFromInt32( nScTab + 1 ) );

    ScStyleSheet& rStyleSheet = ScfTools::MakePageStyleSheet( GetStyleSheetPool(), aStyleName, false );
    SfxItemSet& rItemSet = rStyleSheet.GetItemSet();

    // *** page settings ***

    ScfTools::PutItem( rItemSet, SfxBoolItem( ATTR_PAGE_TOPDOWN,    !maData.mbPrintInRows ),    true );
    ScfTools::PutItem( rItemSet, SfxBoolItem( ATTR_PAGE_HORCENTER,  maData.mbHorCenter ),       true );
    ScfTools::PutItem( rItemSet, SfxBoolItem( ATTR_PAGE_VERCENTER,  maData.mbVerCenter ),       true );
    ScfTools::PutItem( rItemSet, SfxBoolItem( ATTR_PAGE_HEADERS,    maData.mbPrintHeadings ),   true );
    ScfTools::PutItem( rItemSet, SfxBoolItem( ATTR_PAGE_GRID,       maData.mbPrintGrid ),       true );
    ScfTools::PutItem( rItemSet, SfxBoolItem( ATTR_PAGE_NOTES,      maData.mbPrintNotes ),      true );

    sal_uInt16 nStartPage = maData.mbManualStart ? maData.mnStartPage : 0;
    ScfTools::PutItem( rItemSet, SfxUInt16Item( ATTR_PAGE_FIRSTPAGENO, nStartPage ), true );

    if( maData.mxBrushItem.get() )
        rItemSet.Put( *maData.mxBrushItem );

    if( mbValidPaper )
    {
        SvxPageItem aPageItem( GETITEM( rItemSet, SvxPageItem, ATTR_PAGE ) );
        aPageItem.SetLandscape( !maData.mbPortrait );
        rItemSet.Put( aPageItem );
        ScfTools::PutItem( rItemSet, SvxSizeItem( ATTR_PAGE_SIZE, maData.GetScPaperSize() ), true );
    }

    if( maData.mbFitToPages )
        rItemSet.Put( ScPageScaleToItem( maData.mnFitToWidth, maData.mnFitToHeight ) );
    else if( maData.mbValid )
        rItemSet.Put( SfxUInt16Item( ATTR_PAGE_SCALE, maData.mnScaling ) );

    // *** margin preparations ***

    double fLeftMargin   = maData.mfLeftMargin;
    double fRightMargin  = maData.mfRightMargin;
    double fTopMargin    = maData.mfTopMargin;
    double fBottomMargin = maData.mfBottomMargin;
    // distances between header/footer and page area
    double fHeaderHeight = 0.0;
    double fHeaderDist = 0.0;
    double fFooterHeight = 0.0;
    double fFooterDist = 0.0;
    // in Calc, "header/footer left/right margin" is X distance between header/footer and page margin
    double fHdrLeftMargin  = maData.mfHdrLeftMargin  - maData.mfLeftMargin;
    double fHdrRightMargin = maData.mfHdrRightMargin - maData.mfRightMargin;
    double fFtrLeftMargin  = maData.mfFtrLeftMargin  - maData.mfLeftMargin;
    double fFtrRightMargin = maData.mfFtrRightMargin - maData.mfRightMargin;

    // *** header and footer ***

    XclImpHFConverter aHFConv( GetRoot() );

    // header
    bool bHasHeader = (maData.maHeader.Len() != 0);
    SvxSetItem aHdrSetItem( GETITEM( rItemSet, SvxSetItem, ATTR_PAGE_HEADERSET ) );
    SfxItemSet& rHdrItemSet = aHdrSetItem.GetItemSet();
    rHdrItemSet.Put( SfxBoolItem( ATTR_PAGE_ON, bHasHeader ) );
    if( bHasHeader )
    {
        aHFConv.ParseString( maData.maHeader );
        aHFConv.FillToItemSet( rItemSet, ATTR_PAGE_HEADERLEFT );
        aHFConv.FillToItemSet( rItemSet, ATTR_PAGE_HEADERRIGHT );
        // #i23296# In Calc, "top margin" is distance to header
        fTopMargin = maData.mfHeaderMargin;
        // Calc uses distance between header and sheet data area
        fHeaderHeight = XclTools::GetInchFromTwips( aHFConv.GetTotalHeight() );
        fHeaderDist = maData.mfTopMargin - maData.mfHeaderMargin - fHeaderHeight;
    }
    if( fHeaderDist < 0.0 )
    {
        /*  #i23296# Header overlays sheet data:
            -> set fixed header height to get correct sheet data position. */
        ScfTools::PutItem( rHdrItemSet, SfxBoolItem( ATTR_PAGE_DYNAMIC, false ), true );
        // shrink header height
        long nHdrHeight = XclTools::GetTwipsFromInch( fHeaderHeight + fHeaderDist );
        ScfTools::PutItem( rHdrItemSet, SvxSizeItem( ATTR_PAGE_SIZE, Size( 0, nHdrHeight ) ), true );
        lclPutMarginItem( rHdrItemSet, EXC_ID_BOTTOMMARGIN, 0.0 );
    }
    else
    {
        // use dynamic header height
        ScfTools::PutItem( rHdrItemSet, SfxBoolItem( ATTR_PAGE_DYNAMIC, true ), true );
        lclPutMarginItem( rHdrItemSet, EXC_ID_BOTTOMMARGIN, fHeaderDist );
    }
    lclPutMarginItem( rHdrItemSet, EXC_ID_LEFTMARGIN,   fHdrLeftMargin );
    lclPutMarginItem( rHdrItemSet, EXC_ID_RIGHTMARGIN,  fHdrRightMargin );
    rItemSet.Put( aHdrSetItem );

    // footer
    bool bHasFooter = (maData.maFooter.Len() != 0);
    SvxSetItem aFtrSetItem( GETITEM( rItemSet, SvxSetItem, ATTR_PAGE_FOOTERSET ) );
    SfxItemSet& rFtrItemSet = aFtrSetItem.GetItemSet();
    rFtrItemSet.Put( SfxBoolItem( ATTR_PAGE_ON, bHasFooter ) );
    if( bHasFooter )
    {
        aHFConv.ParseString( maData.maFooter );
        aHFConv.FillToItemSet( rItemSet, ATTR_PAGE_FOOTERLEFT );
        aHFConv.FillToItemSet( rItemSet, ATTR_PAGE_FOOTERRIGHT );
        // #i23296# In Calc, "bottom margin" is distance to footer
        fBottomMargin = maData.mfFooterMargin;
        // Calc uses distance between footer and sheet data area
        fFooterHeight = XclTools::GetInchFromTwips( aHFConv.GetTotalHeight() );
        fFooterDist = maData.mfBottomMargin - maData.mfFooterMargin - fFooterHeight;
    }
    if( fFooterDist < 0.0 )
    {
        /*  #i23296# Footer overlays sheet data:
            -> set fixed footer height to get correct sheet data end position. */
        ScfTools::PutItem( rFtrItemSet, SfxBoolItem( ATTR_PAGE_DYNAMIC, false ), true );
        // shrink footer height
        long nFtrHeight = XclTools::GetTwipsFromInch( fFooterHeight + fFooterDist );
        ScfTools::PutItem( rFtrItemSet, SvxSizeItem( ATTR_PAGE_SIZE, Size( 0, nFtrHeight ) ), true );
        lclPutMarginItem( rFtrItemSet, EXC_ID_TOPMARGIN, 0.0 );
    }
    else
    {
        // use dynamic footer height
        ScfTools::PutItem( rFtrItemSet, SfxBoolItem( ATTR_PAGE_DYNAMIC, true ), true );
        lclPutMarginItem( rFtrItemSet, EXC_ID_TOPMARGIN, fFooterDist );
    }
    lclPutMarginItem( rFtrItemSet, EXC_ID_LEFTMARGIN,   fFtrLeftMargin );
    lclPutMarginItem( rFtrItemSet, EXC_ID_RIGHTMARGIN,  fFtrRightMargin );
    rItemSet.Put( aFtrSetItem );

    // *** set final margins ***

    lclPutMarginItem( rItemSet, EXC_ID_LEFTMARGIN,   fLeftMargin );
    lclPutMarginItem( rItemSet, EXC_ID_RIGHTMARGIN,  fRightMargin );
    lclPutMarginItem( rItemSet, EXC_ID_TOPMARGIN,    fTopMargin );
    lclPutMarginItem( rItemSet, EXC_ID_BOTTOMMARGIN, fBottomMargin );

    // *** put style sheet into document ***

    rDoc.SetPageStyle( nScTab, rStyleSheet.GetName() );

    // *** page breaks ***

    ScfUInt16Vec::const_iterator aIt, aEnd;

    for( aIt = maData.maHorPageBreaks.begin(), aEnd = maData.maHorPageBreaks.end(); aIt != aEnd; ++aIt )
    {
        SCROW nScRow = static_cast< SCROW >( *aIt );
        if( nScRow <= MAXROW )
            rDoc.SetRowFlags( nScRow, nScTab, rDoc.GetRowFlags( nScRow, nScTab ) | CR_MANUALBREAK );
    }

    for( aIt = maData.maVerPageBreaks.begin(), aEnd = maData.maVerPageBreaks.end(); aIt != aEnd; ++aIt )
    {
        SCCOL nScCol = static_cast< SCCOL >( *aIt );
        if( nScCol <= MAXCOL )
            rDoc.SetColFlags( nScCol, nScTab, rDoc.GetColFlags( nScCol, nScTab ) | CR_MANUALBREAK );
    }
}

// ============================================================================

