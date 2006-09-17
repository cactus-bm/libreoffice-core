/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: impanmvw.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 12:02:38 $
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
#include "precompiled_vcl.hxx"

#include "impanmvw.hxx"
#include "virdev.hxx"
#include "window.hxx"
#include "salbtype.hxx"

// ----------------
// - ImplAnimView -
// ----------------

ImplAnimView::ImplAnimView( Animation* pParent, OutputDevice* pOut,
                            const Point& rPt, const Size& rSz,
                            ULONG nExtraData,
                            OutputDevice* pFirstFrameOutDev ) :
        mpParent        ( pParent ),
        mpOut           ( pFirstFrameOutDev ? pFirstFrameOutDev : pOut ),
        mnExtraData     ( nExtraData ),
        maPt            ( rPt ),
        maSz            ( rSz ),
        maSzPix         ( mpOut->LogicToPixel( maSz ) ),
        maClip          ( mpOut->GetClipRegion() ),
        mpBackground    ( new VirtualDevice ),
        mpRestore       ( new VirtualDevice ),
        meLastDisposal  ( DISPOSE_BACK ),
        mbPause         ( FALSE ),
        mbMarked        ( FALSE ),
        mbHMirr         ( maSz.Width() < 0L ),
        mbVMirr         ( maSz.Height() < 0L )
{
    mpParent->ImplIncAnimCount();

    // mirrored horizontically?
    if( mbHMirr )
    {
        maDispPt.X() = maPt.X() + maSz.Width() + 1L;
        maDispSz.Width() = -maSz.Width();
        maSzPix.Width() = -maSzPix.Width();
    }
    else
    {
        maDispPt.X() = maPt.X();
        maDispSz.Width() = maSz.Width();
    }

    // mirrored vertically?
    if( mbVMirr )
    {
        maDispPt.Y() = maPt.Y() + maSz.Height() + 1L;
        maDispSz.Height() = -maSz.Height();
        maSzPix.Height() = -maSzPix.Height();
    }
    else
    {
        maDispPt.Y() = maPt.Y();
        maDispSz.Height() = maSz.Height();
    }

    // save background
    mpBackground->SetOutputSizePixel( maSzPix );

    if( mpOut->GetOutDevType() == OUTDEV_WINDOW )
    {
        MapMode aTempMap( mpOut->GetMapMode() );
        aTempMap.SetOrigin( Point() );
        mpBackground->SetMapMode( aTempMap );
        ( (Window*) mpOut )->SaveBackground( maDispPt, maDispSz, Point(), *mpBackground );
        mpBackground->SetMapMode( MapMode() );
    }
    else
        mpBackground->DrawOutDev( Point(), maSzPix, maDispPt, maDispSz, *mpOut );

    // initial drawing to actual position
    ImplDrawToPos( mpParent->ImplGetCurPos() );

    // if first frame OutputDevice is set, update variables now for real OutputDevice
    if( pFirstFrameOutDev )
        maClip = ( mpOut = pOut )->GetClipRegion();
}

// ------------------------------------------------------------------------

ImplAnimView::~ImplAnimView()
{
    delete mpBackground;
    delete mpRestore;

    mpParent->ImplDecAnimCount();
}

// ------------------------------------------------------------------------

BOOL ImplAnimView::ImplMatches( OutputDevice* pOut, long nExtraData ) const
{
    BOOL bRet = FALSE;

    if( nExtraData )
    {
        if( ( mnExtraData == nExtraData ) && ( !pOut || ( pOut == mpOut ) ) )
            bRet = TRUE;
    }
    else if( !pOut || ( pOut == mpOut ) )
        bRet = TRUE;

    return bRet;
}

// ------------------------------------------------------------------------

void ImplAnimView::ImplGetPosSize( const AnimationBitmap& rAnm, Point& rPosPix, Size& rSizePix )
{
    const Size& rAnmSize = mpParent->GetDisplaySizePixel();
    Point       aPt2( rAnm.aPosPix.X() + rAnm.aSizePix.Width() - 1L,
                      rAnm.aPosPix.Y() + rAnm.aSizePix.Height() - 1L );
    double      fFactX, fFactY;

    // calculate x scaling
    if( rAnmSize.Width() > 1L )
        fFactX = (double) ( maSzPix.Width() - 1L ) / ( rAnmSize.Width() - 1L );
    else
        fFactX = 1.0;

    // calculate y scaling
    if( rAnmSize.Height() > 1L )
        fFactY = (double) ( maSzPix.Height() - 1L ) / ( rAnmSize.Height() - 1L );
    else
        fFactY = 1.0;

    rPosPix.X() = FRound( rAnm.aPosPix.X() * fFactX );
    rPosPix.Y() = FRound( rAnm.aPosPix.Y() * fFactY );

    aPt2.X() = FRound( aPt2.X() * fFactX );
    aPt2.Y() = FRound( aPt2.Y() * fFactY );

    rSizePix.Width() = aPt2.X() - rPosPix.X() + 1L;
    rSizePix.Height() = aPt2.Y() - rPosPix.Y() + 1L;

    // mirrored horizontically?
    if( mbHMirr )
        rPosPix.X() = maSzPix.Width() - 1L - aPt2.X();

    // mirrored vertically?
    if( mbVMirr )
        rPosPix.Y() = maSzPix.Height() - 1L - aPt2.Y();
}

// ------------------------------------------------------------------------

void ImplAnimView::ImplDrawToPos( ULONG nPos )
{
    VirtualDevice   aVDev;
    Region*         pOldClip = !maClip.IsNull() ? new Region( mpOut->GetClipRegion() ) : NULL;

    aVDev.SetOutputSizePixel( maSzPix, FALSE );
    nPos = Min( nPos, (ULONG) mpParent->Count() - 1UL );

    for( ULONG i = 0UL; i <= nPos; i++ )
        ImplDraw( i, &aVDev );

    if( pOldClip )
        mpOut->SetClipRegion( maClip );

    mpOut->DrawOutDev( maDispPt, maDispSz, Point(), maSzPix, aVDev );

    if( pOldClip )
    {
        mpOut->SetClipRegion( *pOldClip );
        delete pOldClip;
    }
}

// ------------------------------------------------------------------------

void ImplAnimView::ImplDraw( ULONG nPos )
{
    ImplDraw( nPos, NULL );
}

// ------------------------------------------------------------------------

void ImplAnimView::ImplDraw( ULONG nPos, VirtualDevice* pVDev )
{
    Rectangle aOutRect( mpOut->PixelToLogic( Point() ), mpOut->GetOutputSize() );

    // check, if output lies out of display
    if( aOutRect.Intersection( Rectangle( maDispPt, maDispSz ) ).IsEmpty() )
        ImplSetMarked( TRUE );
    else if( !mbPause )
    {
        VirtualDevice*          pDev;
        Point                   aPosPix;
        Point                   aBmpPosPix;
        Size                    aSizePix;
        Size                    aBmpSizePix;
        const ULONG             nLastPos = mpParent->Count() - 1;
        const AnimationBitmap&  rAnm = mpParent->Get( (USHORT) ( mnActPos = Min( nPos, nLastPos ) ) );

        ImplGetPosSize( rAnm, aPosPix, aSizePix );

        // mirrored horizontically?
        if( mbHMirr )
        {
            aBmpPosPix.X() = aPosPix.X() + aSizePix.Width() - 1L;
            aBmpSizePix.Width() = -aSizePix.Width();
        }
        else
        {
            aBmpPosPix.X() = aPosPix.X();
            aBmpSizePix.Width() = aSizePix.Width();
        }

        // mirrored vertically?
        if( mbVMirr )
        {
            aBmpPosPix.Y() = aPosPix.Y() + aSizePix.Height() - 1L;
            aBmpSizePix.Height() = -aSizePix.Height();
        }
        else
        {
            aBmpPosPix.Y() = aPosPix.Y();
            aBmpSizePix.Height() = aSizePix.Height();
        }

        // get output device
        if( !pVDev )
        {
            pDev = new VirtualDevice;
            pDev->SetOutputSizePixel( maSzPix, FALSE );
            pDev->DrawOutDev( Point(), maSzPix, maDispPt, maDispSz, *mpOut );
        }
        else
            pDev = pVDev;

        // restore background after each run
        if( !nPos )
        {
            meLastDisposal = DISPOSE_BACK;
            maRestPt = Point();
            maRestSz = maSzPix;
        }

        // restore
        if( ( DISPOSE_NOT != meLastDisposal ) && maRestSz.Width() && maRestSz.Height() )
        {
            if( DISPOSE_BACK == meLastDisposal )
                pDev->DrawOutDev( maRestPt, maRestSz, maRestPt, maRestSz, *mpBackground );
            else
                pDev->DrawOutDev( maRestPt, maRestSz, Point(), maRestSz, *mpRestore );
        }

        meLastDisposal = rAnm.eDisposal;
        maRestPt = aPosPix;
        maRestSz = aSizePix;

        // Was muessen wir beim naechsten Mal restaurieren ?
        // ==> ggf. in eine Bitmap stecken, ansonsten SaveBitmap
        // aus Speichergruenden loeschen
        if( ( meLastDisposal == DISPOSE_BACK ) || ( meLastDisposal == DISPOSE_NOT ) )
            mpRestore->SetOutputSizePixel( Size( 1, 1 ), FALSE );
        else
        {
            mpRestore->SetOutputSizePixel( maRestSz, FALSE );
            mpRestore->DrawOutDev( Point(), maRestSz, aPosPix, aSizePix, *pDev );
        }

        pDev->DrawBitmapEx( aBmpPosPix, aBmpSizePix, rAnm.aBmpEx );

        if( !pVDev )
        {
            Region* pOldClip = !maClip.IsNull() ? new Region( mpOut->GetClipRegion() ) : NULL;

            if( pOldClip )
                mpOut->SetClipRegion( maClip );

            mpOut->DrawOutDev( maDispPt, maDispSz, Point(), maSzPix, *pDev );

            if( pOldClip )
            {
                mpOut->SetClipRegion( *pOldClip );
                delete pOldClip;
            }

            delete pDev;

            if( mpOut->GetOutDevType() == OUTDEV_WINDOW )
                ( (Window*) mpOut )->Sync();
        }
    }
}

// ------------------------------------------------------------------------

void ImplAnimView::ImplRepaint()
{
    const BOOL bOldPause = mbPause;

    if( mpOut->GetOutDevType() == OUTDEV_WINDOW )
    {
        MapMode aTempMap( mpOut->GetMapMode() );
        aTempMap.SetOrigin( Point() );
        mpBackground->SetMapMode( aTempMap );
        ( (Window*) mpOut )->SaveBackground( maDispPt, maDispSz, Point(), *mpBackground );
        mpBackground->SetMapMode( MapMode() );
    }
    else
        mpBackground->DrawOutDev( Point(), maSzPix, maDispPt, maDispSz, *mpOut );

    mbPause = FALSE;
    ImplDrawToPos( mnActPos );
    mbPause = bOldPause;
}

// ------------------------------------------------------------------------

AInfo* ImplAnimView::ImplCreateAInfo() const
{
    AInfo* pAInfo = new AInfo;

    pAInfo->aStartOrg = maPt;
    pAInfo->aStartSize = maSz;
    pAInfo->pOutDev = mpOut;
    pAInfo->pViewData = (void*) this;
    pAInfo->nExtraData = mnExtraData;
    pAInfo->bPause = mbPause;

    return pAInfo;
}
