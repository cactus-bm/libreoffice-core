/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: excobj.cxx,v $
 *
 *  $Revision: 1.40 $
 *
 *  last change: $Author: obo $ $Date: 2006-07-10 13:29:23 $
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

#ifndef _COM_SUN_STAR_EMBED_ASPECTS_HPP_
#include <com/sun/star/embed/Aspects.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XFORMCOMPONENT_HPP_
#include <com/sun/star/form/XFormComponent.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XCONTROLSHAPE_HPP_
#include <com/sun/star/drawing/XControlShape.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPES_HPP_
#include <com/sun/star/drawing/XShapes.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXCONTAINER_HPP_
#include <com/sun/star/container/XIndexContainer.hpp>
#endif

#include <toolkit/helper/vclunohelper.hxx>

#include <stdio.h>

#include "scitems.hxx"
#include <svx/eeitem.hxx>
#define ITEMID_FIELD EE_FEATURE_FIELD

#include <svx/colritem.hxx>
#include <svx/svddef.hxx>
#include <svx/svdoole2.hxx>
#include <svx/svdopath.hxx>
#include <svx/svdorect.hxx>
#include <svx/svdotext.hxx>
#include <svx/svdpage.hxx>
#include <svx/xcolit.hxx>
#include <svx/xfillit.hxx>
#include <svx/xfillit0.hxx>
#include <svx/xlineit.hxx>
#include <svx/xlineit0.hxx>
#include <svx/xsetit.hxx>
#include <svx/outlobj.hxx>
#include <svx/outliner.hxx>
#include <sfx2/objsh.hxx>
#include <sot/storage.hxx>
#include <sch/schdll.hxx>
#include <sch/memchrt.hxx>
#include <svtools/itemset.hxx>
#include <svtools/moduleoptions.hxx>
#include <sfx2/app.hxx>
#include <svx/editdata.hxx>
#include <svx/editeng.hxx>
#include <svx/editobj.hxx>
#include <svx/editstat.hxx>
#include <sot/clsids.hxx>
#ifndef _SVDOGRAF_HXX //autogen wg. SdrGrafObj
#include <svx/svdograf.hxx>
#endif

#include "document.hxx"
#include "drwlayer.hxx"
#include "chartarr.hxx"
#include "editutil.hxx"

#ifndef SC_XLESCHER_HXX
#include "xlescher.hxx"
#endif
#ifndef SC_XLTRACER_HXX
#include "xltracer.hxx"
#endif
#ifndef SC_XILINK_HXX
#include "xilink.hxx"
#endif

#include "imp_op.hxx"

using namespace com::sun::star;

// Object Flags

#define OBJF_SELECTED   0x0001          // ---
#define OBJF_AUTOSIZE   0x0002          // zellgebunden
#define OBJF_MOVE       0x0004          // ---
#define OBJF_LOCKED     0x0010          // ---
#define OBJF_GROUPED    0x0080          // ---
#define OBJF_HIDDEN     0x0100          // ---
#define OBJF_VISIBLE    0x0200          // ---
#define OBJF_PRINTABLE  0x0400          // ---


void ImportExcel::Obj()
{
    ScDocument& rDoc = GetDoc();
    SCTAB nScTab = GetCurrScTab();

    UINT32 nObj;
    UINT16 nType, nId, nFlags;
    UINT16 nMacroLen;
    XclEscherAnchor aAnchor( nScTab );

    aIn >> nObj >> nType >> nId >> nFlags >> aAnchor >> nMacroLen;
    bool bBiff5 = GetBiff() == EXC_BIFF5;
    aIn.Ignore( bBiff5 ? 6 : 2 );

    Rectangle aRect( aAnchor.GetRect( rDoc, MAP_100TH_MM ) );
    Point aTL( aRect.TopLeft() );
    Point aBR( aRect.BottomRight() );

    SfxItemSet aSet( rDoc.GetDrawLayer()->GetItemPool(), SDRATTR_START, SDRATTR_END );
    SdrObject* pObj = NULL;
    switch( nType )
    {
        case 0x01:  pObj = LineObj( aSet, aTL, aBR ); break;
        case 0x02:  pObj = RectObj( aSet, aTL, aBR ); break;
        case 0x05:  pObj = BeginChartObj( aSet, aTL, aBR ); break;
        default: pExcRoot->pIR->GetTracer().TraceUnsupportedObjects(); break;
    }

    if( pObj )
    {
        pObj->ClearMergedItem();
        pObj->SetMergedItemSetAndBroadcast(aSet);

        pObj->NbcSetLogicRect( aRect );
        pObj->SetLayer( SC_LAYER_FRONT );
        rDoc.GetDrawLayer()->GetPage( static_cast<sal_uInt16>(nScTab) )->InsertObject( pObj );
        if( bBiff5 && aIn.GetRecLeft() )
        {
            sal_uInt8 nNameLen;
            aIn >> nNameLen;
            aIn.Ignore( nNameLen + nMacroLen );
        }
    }
}


void ImportExcel::SetLineStyle( SfxItemSet& rSet, short nColor, short nStyle, short nWidth )
{
    if( nColor >= 0 )
        rSet.Put( XLineColorItem( String(), GetPalette().GetColor( nColor ) ) );
    if( nStyle >= 0 )
    {
        XLineStyle eStyle = ( nStyle == 1 || nStyle == 2 )
                          ? XLINE_DASH : XLINE_SOLID;
        rSet.Put( XLineStyleItem( eStyle ) );
    }
    if( nWidth >= 0 )
        rSet.Put( XLineWidthItem( nWidth * 40 ) );
}


void ImportExcel::SetFillStyle( SfxItemSet& rSet, short nBg, short nFg, short nStyle )
{
    if( nBg >= 0 )
        rSet.Put( XFillColorItem( String(), GetPalette().GetColor( nFg ) ) );
    if( nStyle >= 0 )
        // Entweder mit Farbe oder leer, keine Patterns!
        rSet.Put( XFillStyleItem( nStyle ? XFILL_SOLID : XFILL_NONE ) );
}


SdrObject* ImportExcel::LineObj( SfxItemSet& rSet, const Point& rUL, const Point& rLR )
{
    BYTE        nLc, nStyle, nWeight, nAuto;
    UINT16      nEndStyles;
    BYTE        nQuadrant;
    aIn >> nLc >> nStyle >> nWeight >> nAuto >> nEndStyles >> nQuadrant;
    aIn.Ignore( 1 );

    Point       aPointArr[2] = { rUL, rLR };
    SdrPathObj* pObj = new SdrPathObj( OBJ_LINE, XPolyPolygon( XPolygon( Polygon( 2, aPointArr ) ) ) );
    SetLineStyle( rSet, nLc, nStyle, nWeight );
    return pObj;
}


SdrObject* ImportExcel::RectObj( SfxItemSet& rSet, const Point& rUL, const Point& rLR )
{
    BYTE        nBg, nFg, nPat, fAuto1, nLc, nLStyle, nLWeight, fAuto2;
    UINT16      nFRS;
    aIn >> nBg >> nFg >> nPat >> fAuto1 >> nLc >> nLStyle
        >> nLWeight >> fAuto2 >> nFRS;

    SdrRectObj* pObj = new SdrRectObj( Rectangle( rUL, rLR ) );
    SetLineStyle( rSet, nLc, nLStyle, nLWeight );
    SetFillStyle( rSet, nBg, nFg, nPat );
    return pObj;
}


SdrObject* ImportExcel::BeginChartObj( SfxItemSet&, const Point& rUL, const Point& rLR )
{
    BYTE        nBg, nFg, nPat, fAuto1, nLc, nLStyle, nLWeight, fAuto2;
    UINT16      nFRS;
    aIn >> nBg >> nFg >> nPat >> fAuto1 >> nLc >> nLStyle
        >> nLWeight >> fAuto2 >> nFRS;
    aIn.Ignore( 18 );

    ExcelChartData* pData = new ExcelChartData( pD, rUL, rLR, GetCurrScTab() );
    SetLineStyle( *pData->pAttrs, nLc, nLStyle, nLWeight );
    SetFillStyle( *pData->pAttrs, nBg, nFg, nPat );
    pData->pNext = pChart;
    pChart = pData;
    pExcRoot->pIR->GetTracer().TraceUnsupportedObjects();
    return NULL;
}


void ImportExcel::EndChartObj()
{
    DBG_ASSERT( pChart, "Kein Chart definiert!" );

    ExcelChartData*         pData = pChart;
    if( !pData )
        {
            pExcRoot->pIR->GetTracer().TraceUnsupportedObjects();
        return;
        }

    pChart = pData->pNext;

    if( pUsedChartFirst )
        pUsedChartLast->pNext = pData;
    else
        pUsedChartFirst = pData;

    pUsedChartLast = pData;

    pData->pNext = NULL;
}


ExcelChartData::ExcelChartData( ScDocument* p, const Point& rUL, const Point& rLR, const SCTAB nBT ) :
    aRect( rUL, rLR ),
    nRow1( SCROW_MAX ), nCol1( SCCOL_MAX ), nTab1( SCTAB_MAX ), nRow2( 0 ), nCol2( 0 ), nTab2( 0 ), pNext( NULL ),
    nBaseTab( nBT ), nObjNum( 0xFFFFFFFF )
{
    pAttrs = new SfxItemSet( p->GetDrawLayer()->GetItemPool(), SDRATTR_START, SDRATTR_END );
}


ExcelChartData::~ExcelChartData()
{
    delete pAttrs;
}


void ImportExcel::ChartSelection( void )
{
    UINT8       nId, nRef;
    UINT16      nFlags, nFmt, nSize;
    aIn >> nId >> nRef >> nFlags >> nFmt >> nSize;
    // Nur Links To Worksheet auswerten
    if( nRef == 2 && nSize )
    {
        // Der AI-Record enthaelt eine 3D Cell Reference (Opcode 0x3A)
        // oder eine 3D Area Reference (Opcode 0x3B) (oder auch nicht...)
        UINT8       nOp;
        UINT16      nLink;
        UINT16      nTab1, nTab2, nRow1, nRow2;
        UINT16      nCol1, nCol2;
        BOOL        bValues = FALSE;

        aIn >> nOp;

        if( GetBiff() == EXC_BIFF5 )
        {
            aIn >> nLink;

            if( nLink == 0xFFFF )
            {
                aIn.Ignore( 8 );
                switch( nOp )
                {
                    case 0x3A:
                    {
                        UINT8   nC;
                        aIn >> nTab1 >> nTab2 >> nRow1 >> nC;
                        nRow2 = nRow1;
                        nCol2 = nCol1 = nC;
                        bValues = TRUE;
                    }
                        break;
                    case 0x3B:
                    {
                        UINT8   nC1, nC2;
                        aIn >> nTab1 >> nTab2 >> nRow1 >> nRow2 >> nC1 >> nC2;
                        nCol1 = nC1;
                        nCol2 = nC2;
                        bValues = TRUE;
                    }
                        break;
                }

                nRow1 &= 0x3FFF;
                nRow2 &= 0x3FFF;
            }
        }
        else                                                    // Biff8 (?)
        {
            UINT16              nIxti;

            DBG_ASSERT( GetBiff() == EXC_BIFF8,
                "*ImportExcel::ChartSelection(): Den kenn' ich nicht!" );

            if( nOp == 0x3A || nOp == 0x3B )
            {
                if( nOp == 0x3A )
                {
                    aIn >> nIxti >> nRow1 >> nCol1;
                    nCol1 &= 0x3FFF;
                    nCol2 = nCol1;
                    nRow2 = nRow1;
                }
                else
                {
                    aIn >> nIxti >> nRow1 >> nRow2 >> nCol1 >> nCol2;
                    nCol1 &= 0x3FFF;
                    nCol2 &= 0x3FFF;
                }

                SCTAB nScTab1 = 0;
                SCTAB nScTab2 = 0;
                if( pExcRoot->pIR->GetLinkManager().GetScTabRange( nScTab1, nScTab2, nIxti ) )
                    bValues = TRUE;
                nTab1 = static_cast<UINT16>(nScTab1);
                nTab2 = static_cast<UINT16>(nScTab2);
            }
        }   // Ende Biff8

        if( bValues )
        {
            DBG_ASSERT( pChart, "Keine Chartdaten!" );
            if( pChart )
            {
                if( pChart->nRow1 > static_cast<SCROW>(nRow1) )
                    pChart->nRow1 = static_cast<SCROW>(nRow1);
                if( pChart->nCol1 > static_cast<SCCOL>(nCol1) )
                    pChart->nCol1 = static_cast<SCCOL>(nCol1);
                if( pChart->nTab1 > static_cast<SCTAB>(nTab1) )
                    pChart->nTab1 = static_cast<SCTAB>(nTab1);
                if( pChart->nRow2 < static_cast<SCROW>(nRow2) )
                    pChart->nRow2 = static_cast<SCROW>(nRow2);
                if( pChart->nCol2 < static_cast<SCCOL>(nCol2) )
                    pChart->nCol2 = static_cast<SCCOL>(nCol2);
                if( pChart->nTab2 > static_cast<SCTAB>(nTab2) )
                    pChart->nTab2 = static_cast<SCTAB>(nTab2);
            }
        }
    }
}


void ImportExcel::ChartSeriesText()
{
    UINT16  nId;
    aIn >> nId;
    if( pChart )
        pChart->aLastLabel = aIn.ReadByteString( false );
}


void ImportExcel::ChartObjectLink()
{
    UINT16 nType, nVar1, nVar2;
    aIn >> nType >> nVar1 >> nVar2;
    if( pChart && pChart->aLastLabel.Len() )
    {
        switch( nType )
        {
            case 1: pChart->aTitle = pChart->aLastLabel; break;
            case 2: pChart->aYTitle = pChart->aLastLabel; break;
            case 3: pChart->aXTitle = pChart->aLastLabel; break;
            case 7: pChart->aZTitle = pChart->aLastLabel; break;
        }
        pChart->aLastLabel.Erase();
    }
}


#ifdef WNT
#pragma optimize("",off)
#endif

void ImportExcel::EndAllChartObjects( void )
{
    SfxObjectShell* pSh = pD->GetDocumentShell();
    if( !pSh ) return;

    ExcelChartData*             p = pUsedChartFirst;
    ExcelChartData*             pDelete;

    while( p )
    {
        if( p->nRow1 <= p->nRow2 && p->nCol1 <= p->nCol2 )
        {
            //  wenn Chart nicht installiert ist, darf nicht auf SCH_MOD zugegriffen werden!
            //! Warnung am Storage setzen?
            if ( SvtModuleOptions().IsChart() )
            {
                sal_Int64 nAspect = embed::Aspects::MSOLE_CONTENT;
                ::rtl::OUString aName;
                uno::Reference < embed::XEmbeddedObject > xObj = pSh->GetEmbeddedObjectContainer().CreateEmbeddedObject( SvGlobalName( SO3_SCH_CLASSID ).GetByteSequence(), aName );

                awt::Size aSz = xObj->getVisualAreaSize( nAspect );
                Size aSize( aSz.Width, aSz.Height );
                if( aSize.Height() == 0 || aSize.Width() == 0 )
                {
                    MapUnit aUnit = VCLUnoHelper::UnoEmbed2VCLMapUnit( xObj->getMapUnit( nAspect ) );
                    aSize.Width() = 5000;
                    aSize.Height() = 5000;
                    aSize = Window::LogicToLogic
                                ( aSize, MapMode( MAP_100TH_MM ), MapMode( aUnit ) );
                    aSz.Width = aSize.Width();
                    aSz.Height = aSize.Height();
                    xObj->setVisualAreaSize( nAspect, aSz );
                }

                pD->LimitChartArea( p->nTab1, p->nCol1, p->nRow1, p->nCol2, p->nRow2 );

                SdrOle2Obj* pSdrObj = new SdrOle2Obj( svt::EmbeddedObjectRef( xObj, nAspect ), aName, p->aRect );

                pSdrObj->ClearMergedItem();
                pSdrObj->SetMergedItemSetAndBroadcast(*p->pAttrs);

                pSdrObj->SetLayer( SC_LAYER_FRONT );

                pD->GetDrawLayer()->GetPage( static_cast<sal_uInt16>(p->nBaseTab) )->InsertObject( pSdrObj );

                pSdrObj->NbcSetLogicRect( p->aRect );

                ScChartArray    aChartObj
                                ( pD, p->nTab1, p->nCol1, p->nRow1, p->nCol2, p->nRow2, aName );

                SchMemChart*    pMemChart = aChartObj.CreateMemChart();
                SchDLL::Update( xObj, pMemChart );
                pSdrObj->GetNewReplacement();
                delete pMemChart;
            }
        }

        pDelete = p;
        p = p->pNext;

        delete pDelete;
    }

    pUsedChartFirst = pUsedChartLast = NULL;

    p = pChart;
    while( p )
    {
        pDelete = p;
        p = p->pNext;
        delete pDelete;
    }

    GetDoc().UpdateAllCharts();
}



#ifdef WNT
#pragma optimize("",on)
#endif
