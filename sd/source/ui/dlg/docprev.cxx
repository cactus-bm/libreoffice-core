/*************************************************************************
 *
 *  $RCSfile: docprev.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: rt $ $Date: 2005-03-30 10:29:49 $
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

#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGE_HPP_
#include <com/sun/star/drawing/XDrawPage.hpp>
#endif
#ifndef _COM_SUN_STAR_ANIMATIONS_XANIMATIONNODE_HPP_
#include <com/sun/star/animations/XAnimationNode.hpp>
#endif

#ifndef _SD_SLIDESHOW_HXX
#include "slideshow.hxx"
#endif

#ifndef _SFX_OBJSH_HXX // SfxObjectShell
#include <sfx2/objsh.hxx>
#endif

#ifndef _SV_GDIMTF_HXX // GDIMetaFile
#include <vcl/gdimtf.hxx>
#endif

#ifndef _SV_VIRDEV_HXX // class VirtualDevice
#include <vcl/virdev.hxx>
#endif

#ifndef _COM_SUN_STAR_PRESENTATION_FADEEFFECT_HPP_
#include <com/sun/star/presentation/FadeEffect.hpp>
#endif

#ifndef _SD_FADEDEF_H // enum FadeSpeed
#include <fadedef.h>
#endif

#ifndef _SV_CTRL_HXX // class Control
#include <vcl/ctrl.hxx>
#endif

#ifndef _SVDOUTL_HXX
#include <svx/svdoutl.hxx>
#endif
#ifndef _SVDPAGV_HXX
#include <svx/svdpagv.hxx>
#endif
#ifndef _SVDORECT_HXX
#include <svx/svdorect.hxx>
#endif

#include "docprev.hxx"
#include "drawdoc.hxx"
#include "DrawDocShell.hxx"
#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#ifndef SD_SHOW_VIEW_HXX
#include "showview.hxx"
#endif
#ifndef SD_DRAW_VIEW_HXX
#include "drawview.hxx"
#endif
#include "sdpage.hxx"

#include <memory>


using ::com::sun::star::drawing::XDrawPage;
using ::com::sun::star::animations::XAnimationNode;

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

const int SdDocPreviewWin::FRAME = 4;

void SdDocPreviewWin::SetObjectShell( SfxObjectShell* pObj, sal_uInt16 nShowPage )
{
    mpObj = pObj;
    mnShowPage = nShowPage;
    delete mpSlideShow;
    mpSlideShow = 0;

    updateViewSettings();
}

SdDocPreviewWin::SdDocPreviewWin( Window* pParent, const ResId& rResId )
: Control(pParent, rResId), pMetaFile( 0 ), bInEffect(FALSE), mpObj(NULL), mnShowPage(0)
{
    mpSlideShow = 0;
    SetBorderStyle( WINDOW_BORDER_MONO );
    svtools::ColorConfig aColorConfig;
    Wallpaper aEmpty;
    SetBackground( aEmpty );
}

SdDocPreviewWin::SdDocPreviewWin( Window* pParent )
: Control(pParent, 0 ), pMetaFile( 0 ), bInEffect(FALSE), mpObj(NULL), mnShowPage(0)
{
    mpSlideShow = 0;
    SetBorderStyle( WINDOW_BORDER_MONO );
    svtools::ColorConfig aColorConfig;
    Wallpaper aEmpty;
    SetBackground( aEmpty );
    Resize();
    Show();
}

SdDocPreviewWin::~SdDocPreviewWin()
{
    delete mpSlideShow;
    delete pMetaFile;
}

void SdDocPreviewWin::Resize()
{
    Invalidate();
    if( mpSlideShow )
        mpSlideShow->resize( GetSizePixel() );
}

void SdDocPreviewWin::SetGDIFile( GDIMetaFile* pFile )
{
    delete pMetaFile;
    pMetaFile = pFile;
    Invalidate();
}

void SdDocPreviewWin::CalcSizeAndPos( GDIMetaFile* pFile, Size& rSize, Point& rPoint )
{
    Size aTmpSize = pFile ? pFile->GetPrefSize() : Size(1,1 );
    long nWidth = rSize.Width() - 2*FRAME;
    long nHeight = rSize.Height() - 2*FRAME;
    if( nWidth < 0 ) nWidth = 0;
    if( nHeight < 0 ) nHeight = 0;

    double dRatio=((double)aTmpSize.Width())/aTmpSize.Height();
    double dRatioPreV=((double) nWidth ) / nHeight;

    if (dRatio>dRatioPreV)
    {
        rSize=Size(nWidth, (USHORT)(nWidth/dRatio));
        rPoint=Point( 0, (USHORT)((nHeight-rSize.Height())/2));
    }
    else
    {
        rSize=Size((USHORT)(nHeight*dRatio), nHeight);
        rPoint=Point((USHORT)((nWidth-rSize.Width())/2),0);
    }
}

void SdDocPreviewWin::ImpPaint( GDIMetaFile* pFile, OutputDevice* pVDev )
{
    Point aPoint;
    Size aSize = pVDev->GetOutputSize();
    Point bPoint(aSize.Width()-2*FRAME, aSize.Height()-2*FRAME );
    CalcSizeAndPos( pFile, aSize, aPoint );
    bPoint -= aPoint;
    aPoint += Point( FRAME, FRAME );

    svtools::ColorConfig aColorConfig;

    pVDev->SetLineColor();
    pVDev->SetFillColor( Color( aColorConfig.GetColorValue( svtools::APPBACKGROUND ).nColor ) );
    pVDev->DrawRect(Rectangle( Point(0,0 ), pVDev->GetOutputSize()));
    if( pFile )
    {
        pVDev->SetFillColor( maDocumentColor );
        pVDev->DrawRect(Rectangle(aPoint, aSize));
        pFile->WindStart();
        pFile->Play( pVDev, aPoint, aSize  );
    }
}

void SdDocPreviewWin::Paint( const Rectangle& rRect )
{
    if(( mpSlideShow == 0) || (mpSlideShow->isTerminated() ) )
    {
        SvtAccessibilityOptions aAccOptions;
        bool bUseContrast = aAccOptions.GetIsForPagePreviews() && Application::GetSettings().GetStyleSettings().GetHighContrastMode();
        SetDrawMode( bUseContrast
            ? ::sd::ViewShell::OUTPUT_DRAWMODE_CONTRAST
            : ::sd::ViewShell::OUTPUT_DRAWMODE_COLOR );

        ImpPaint( pMetaFile, (VirtualDevice*)this );

    }
}

void SdDocPreviewWin::startPreview()
{
    if( mpSlideShow )
    {
        delete mpSlideShow;
        mpSlideShow = 0;
    }

    ::sd::DrawDocShell* pDocShell = dynamic_cast< ::sd::DrawDocShell * >( mpObj );
    if( mpObj )
    {
        SdDrawDocument* pDoc = pDocShell->GetDoc();

        if( pDoc )
        {
            SdPage* pPage = pDoc->GetSdPage( mnShowPage, PK_STANDARD );

            if( pPage && (pPage->getTransitionType() != 0) )
            {
                std::auto_ptr<sd::Slideshow> pSlideShow(
                    new sd::Slideshow( 0, 0, pDoc ) );

                Reference< XDrawPage > xDrawPage( pPage->getUnoPage(), UNO_QUERY );
                Reference< XAnimationNode > xAnimationNode;

                if (pSlideShow->startPreview( xDrawPage, xAnimationNode, this ))
                    mpSlideShow = pSlideShow.release();
            }
        }
    }
}

long SdDocPreviewWin::Notify( NotifyEvent& rNEvt )
{
    if ( rNEvt.GetType() == EVENT_MOUSEBUTTONDOWN )
    {
        const MouseEvent* pMEvt = rNEvt.GetMouseEvent();
        if ( pMEvt->IsLeft() )
        {
            if( rNEvt.GetWindow() == this )
            {
                if(aClickHdl.IsSet())
                    aClickHdl.Call(this);
            }
        }
    }

    return Control::Notify( rNEvt );
}


void SdDocPreviewWin::updateViewSettings()
{
    ::sd::DrawDocShell* pDocShell = PTR_CAST(::sd::DrawDocShell,mpObj);
    SdDrawDocument* pDoc = pDocShell?pDocShell->GetDoc():NULL;

    SvtAccessibilityOptions aAccOptions;
    bool bUseWhiteColor = !aAccOptions.GetIsForPagePreviews() && GetSettings().GetStyleSettings().GetHighContrastMode();
    if( bUseWhiteColor )
    {
        maDocumentColor = Color( COL_WHITE );
    }
    else
    {
        svtools::ColorConfig aColorConfig;
        maDocumentColor = Color( aColorConfig.GetColorValue( svtools::DOCCOLOR ).nColor );
    }

    GDIMetaFile* pMtf = NULL;

    if(pDoc)
    {
        SdrOutliner& rOutl = pDoc->GetDrawOutliner();
        Color aOldBackgroundColor = rOutl.GetBackgroundColor();
        rOutl.SetBackgroundColor( maDocumentColor );

        pMtf = new GDIMetaFile;
        SdPage * pPage = pDoc->GetSdPage( mnShowPage, PK_STANDARD );

        VirtualDevice       aVDev;

        const Fraction      aFrac( pDoc->GetScaleFraction() );
        const MapMode       aMap( pDoc->GetScaleUnit(), Point(), aFrac, aFrac );

        aVDev.SetMapMode( aMap );

        // #109058# Disable output, as we only want to record a metafile
        aVDev.EnableOutput( FALSE );

        pMtf->Record( &aVDev );

        ::sd::DrawView* pView = new ::sd::DrawView(pDocShell, this, NULL);


        const Size aSize( pPage->GetSize() );

        pView->SetBordVisible( FALSE );
        pView->SetPageVisible( FALSE );
        pView->ShowPage( pPage, Point() );

        const Point aNewOrg( pPage->GetLftBorder(), pPage->GetUppBorder() );
        const Size aNewSize( aSize.Width() - pPage->GetLftBorder() - pPage->GetRgtBorder(),
                              aSize.Height() - pPage->GetUppBorder() - pPage->GetLwrBorder() );
        const Rectangle aClipRect( aNewOrg, aNewSize );
        MapMode         aVMap( aMap );

        SdrPageView* pPageView  = pView->GetPageView( pPage );

        aVDev.Push();
        aVMap.SetOrigin( Point( -aNewOrg.X(), -aNewOrg.Y() ) );
        aVDev.SetRelativeMapMode( aVMap );
        aVDev.IntersectClipRegion( aClipRect );

        // Use new StandardCheckVisisbilityRedirector
        StandardCheckVisisbilityRedirector aRedirector;

        for (USHORT i=0; i<pView->GetPageViewCount(); i++)
        {
            SdrPageView* pPV=pView->GetPageViewPvNum(i);
            pPV->CompleteRedraw(&aVDev, Region(Rectangle(Point(), aNewSize)), 0, &aRedirector);
        }

        aVDev.Pop();

        pMtf->Stop();
        pMtf->WindStart();
        pMtf->SetPrefMapMode( aMap );
        pMtf->SetPrefSize( aNewSize );

        rOutl.SetBackgroundColor( aOldBackgroundColor );

        delete pView;
    }

    delete pMetaFile;
    pMetaFile = pMtf;

    Invalidate();
}

void SdDocPreviewWin::SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType, const SfxHint& rHint, const TypeId& rHintType)
{
    if( rHint.ISA( SfxSimpleHint ) && ( (SfxSimpleHint&) rHint ).GetId() == SFX_HINT_COLORS_CHANGED )
    {
        updateViewSettings();
    }
}
void SdDocPreviewWin::DataChanged( const DataChangedEvent& rDCEvt )
{
    Control::DataChanged( rDCEvt );

    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) && (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        updateViewSettings();
    }
}
