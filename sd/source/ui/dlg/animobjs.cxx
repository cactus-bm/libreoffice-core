/*************************************************************************
 *
 *  $RCSfile: animobjs.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: obo $ $Date: 2004-01-20 10:41:10 $
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

#ifndef _COM_SUN_STAR_PRESENTATION_ANIMATIONEFFECT_HPP_
#include <com/sun/star/presentation/AnimationEffect.hpp>
#endif
#ifndef _COM_SUN_STAR_PRESENTATION_ANIMATIONSPEED_HPP_
#include <com/sun/star/presentation/AnimationSpeed.hpp>
#endif

#define _ANIMATION          //animation freischalten

#define _SV_BITMAPEX
#include <svx/xoutbmp.hxx>
#include <svx/xoutx.hxx>

#include <time.h>

#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SVDOGRAF_HXX //autogen
#include <svx/svdograf.hxx>
#endif
#ifndef _SVDOGRP_HXX //autogen
#include <svx/svdogrp.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFX_PROGRESS_HXX //autogen
#include <sfx2/progress.hxx>
#endif
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#include "anminfo.hxx"
#include "animobjs.hxx"
#include "animobjs.hrc"
#include "anmdef.hxx"
#include "app.hrc"
#include "strings.hrc"
#include "sdresid.hxx"
#ifndef SD_VIEW_HXX
#include "View.hxx"
#endif
#include "drawdoc.hxx"
#include "sdpage.hxx"
#include "res_bmp.hrc"
#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif

#include <string>
#include <algorithm>

using namespace ::com::sun::star;

namespace sd {

/*************************************************************************
|*  SdDisplay - Control
\************************************************************************/

SdDisplay::SdDisplay( Window* pWin, SdResId Id ) :
        Control( pWin, Id ),
        aScale( 1, 1 )
{
    SetMapMode( MAP_PIXEL );
    const StyleSettings& rStyles = Application::GetSettings().GetStyleSettings();
    SetBackground( Wallpaper( Color( rStyles.GetFieldColor() ) ) );
}

// -----------------------------------------------------------------------

SdDisplay::~SdDisplay()
{
}

// -----------------------------------------------------------------------

void SdDisplay::SetBitmapEx( BitmapEx* pBmpEx )
{
    if( pBmpEx )
    {
        aBitmapEx = *pBmpEx;
    }
    else
    {
        const StyleSettings& rStyles = Application::GetSettings().GetStyleSettings();
        const Color aFillColor = rStyles.GetFieldColor();
        aBitmapEx.Erase(aFillColor);
    }
}

// -----------------------------------------------------------------------

void SdDisplay::Paint( const Rectangle& rRect )
{
    Point aPt;
    Size aSize = GetOutputSize();
    Size aBmpSize = aBitmapEx.GetBitmap().GetSizePixel();
    aBmpSize.Width() = (long) ( (double) aBmpSize.Width() * (double) aScale );
    aBmpSize.Height() = (long) ( (double) aBmpSize.Height() * (double) aScale );

    if( aBmpSize.Width() < aSize.Width() )
        aPt.X() = ( aSize.Width() - aBmpSize.Width() ) / 2;
    if( aBmpSize.Height() < aSize.Height() )
        aPt.Y() = ( aSize.Height() - aBmpSize.Height() ) / 2;

    aBitmapEx.Draw( this, aPt, aBmpSize );
    //DrawBitmap( aPt, aBmpSize, *pBitmap );
}

// -----------------------------------------------------------------------

void SdDisplay::SetScale( const Fraction& rFrac )
{
    aScale = rFrac;
}

void SdDisplay::DataChanged( const DataChangedEvent& rDCEvt )
{
    Control::DataChanged( rDCEvt );

    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) && (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        const StyleSettings& rStyles = Application::GetSettings().GetStyleSettings();
        SetBackground( Wallpaper( Color( rStyles.GetFieldColor() ) ) );
        SetDrawMode( GetDisplayBackground().GetColor().IsDark()
            ? ViewShell::OUTPUT_DRAWMODE_CONTRAST
            : ViewShell::OUTPUT_DRAWMODE_COLOR );
    }
}

/*************************************************************************
|*  AnimationWindow - FloatingWindow
\************************************************************************/

AnimationWindow::AnimationWindow( SfxBindings* pInBindings,
                SfxChildWindow *pCW, Window* pParent, const SdResId& rSdResId ) :
        SfxDockingWindow    ( pInBindings, pCW, pParent, rSdResId ),
        aCtlDisplay         ( this, SdResId( CTL_DISPLAY ) ),
        aBtnFirst           ( this, SdResId( BTN_FIRST ) ),
        aBtnReverse         ( this, SdResId( BTN_REVERSE ) ),
        aBtnStop            ( this, SdResId( BTN_STOP ) ),
        aBtnPlay            ( this, SdResId( BTN_PLAY ) ),
        aBtnLast            ( this, SdResId( BTN_LAST ) ),
        aNumFldBitmap       ( this, SdResId( NUM_FLD_BITMAP ) ),
        aTimeField          ( this, SdResId( TIME_FIELD ) ),
        aLbLoopCount        ( this, SdResId( LB_LOOP_COUNT ) ),
        aBtnGetOneObject    ( this, SdResId( BTN_GET_ONE_OBJECT ) ),
        aBtnGetAllObjects   ( this, SdResId( BTN_GET_ALL_OBJECTS ) ),
        aBtnRemoveBitmap    ( this, SdResId( BTN_REMOVE_BITMAP ) ),
        aBtnRemoveAll       ( this, SdResId( BTN_REMOVE_ALL ) ),
        aFtCount            ( this, SdResId( FT_COUNT ) ),
        aFiCount            ( this, SdResId( FI_COUNT ) ),
        aGrpBitmap          ( this, SdResId( GRP_BITMAP ) ),
        aRbtGroup           ( this, SdResId( RBT_GROUP ) ),
        aRbtBitmap          ( this, SdResId( RBT_BITMAP ) ),
        aFtAdjustment       ( this, SdResId( FT_ADJUSTMENT ) ),
        aLbAdjustment       ( this, SdResId( LB_ADJUSTMENT ) ),
        aBtnCreateGroup     ( this, SdResId( BTN_CREATE_GROUP ) ),
        aGrpAnimation       ( this, SdResId( GRP_ANIMATION_GROUP ) ),
        pBindings           ( pInBindings ),
        pWin                ( pParent ),
        pBitmapEx           ( NULL ),
        bMovie              ( FALSE ),
        bAllObjects         ( FALSE )
{
    FreeResource();

    aBtnGetOneObject.SetModeImage( Image( SdResId( IMG_GET1OBJECT_H ) ), BMP_COLOR_HIGHCONTRAST );
    aBtnGetAllObjects.SetModeImage( Image( SdResId( IMG_GETALLOBJECT_H ) ), BMP_COLOR_HIGHCONTRAST );
    aBtnRemoveBitmap.SetModeImage( Image( SdResId( IMG_REMOVEBMP_H ) ), BMP_COLOR_HIGHCONTRAST );
    aBtnRemoveAll.SetModeImage( Image( SdResId( IMG_REMOVEALLBMP_H ) ), BMP_COLOR_HIGHCONTRAST );

    // neues Dokument mit Seite erzeugen
    pMyDoc = new SdDrawDocument(DOCUMENT_TYPE_IMPRESS, NULL);
    SdPage* pPage = (SdPage*) pMyDoc->AllocPage(FALSE);
    pMyDoc->InsertPage(pPage);

    pControllerItem = new AnimationControllerItem( SID_ANIMATOR_STATE, this, pBindings );

    // Solange noch nicht in der Resource
    aTimeField.SetFormat( TIMEF_SEC_CS );

    aBtnFirst.SetClickHdl( LINK( this, AnimationWindow, ClickFirstHdl ) );
    aBtnReverse.SetClickHdl( LINK( this, AnimationWindow, ClickPlayHdl ) );
    aBtnStop.SetClickHdl( LINK( this, AnimationWindow, ClickStopHdl ) );
    aBtnPlay.SetClickHdl( LINK( this, AnimationWindow, ClickPlayHdl ) );
    aBtnLast.SetClickHdl( LINK( this, AnimationWindow, ClickLastHdl ) );

    aBtnGetOneObject.SetClickHdl( LINK( this, AnimationWindow, ClickGetObjectHdl ) );
    aBtnGetAllObjects.SetClickHdl( LINK( this, AnimationWindow, ClickGetObjectHdl ) );
    aBtnRemoveBitmap.SetClickHdl( LINK( this, AnimationWindow, ClickRemoveBitmapHdl ) );
    aBtnRemoveAll.SetClickHdl( LINK( this, AnimationWindow, ClickRemoveBitmapHdl ) );

    aRbtGroup.SetClickHdl( LINK( this, AnimationWindow, ClickRbtHdl ) );
    aRbtBitmap.SetClickHdl( LINK( this, AnimationWindow, ClickRbtHdl ) );
    aBtnCreateGroup.SetClickHdl( LINK( this, AnimationWindow, ClickCreateGroupHdl ) );
    aNumFldBitmap.SetModifyHdl( LINK( this, AnimationWindow, ModifyBitmapHdl ) );
    aTimeField.SetModifyHdl( LINK( this, AnimationWindow, ModifyTimeHdl ) );

    // disable 3D border
    aCtlDisplay.SetBorderStyle(WINDOW_BORDER_MONO);
    aDisplaySize = aCtlDisplay.GetOutputSize();

    aSize = GetOutputSizePixel();
    SetMinOutputSizePixel( aSize );

    ResetAttrs();

    // der Animator ist leer; es kann keine Animationsgruppe erstellt werden
    aBtnCreateGroup.Disable();
}

// -----------------------------------------------------------------------

AnimationWindow::~AnimationWindow()
{
    ULONG i, nCount;

    delete pControllerItem;

    // Bitmapliste bereinigen
    for( i = 0, nCount = aBmpExList.Count(); i < nCount; i++ )
        delete static_cast< BitmapEx* >( aBmpExList.GetObject( i ) );
    aBmpExList.Clear();

    // Timeliste bereinigen
    for( i = 0, nCount = aTimeList.Count(); i < nCount; i++ )
        delete static_cast< Time* >( aTimeList.GetObject( i ) );
    aTimeList.Clear();

    // die Clones loeschen
    delete pMyDoc;
}

// -----------------------------------------------------------------------

IMPL_LINK( AnimationWindow, ClickFirstHdl, void *, EMPTYARG )
{
    aBmpExList.First();
    pBitmapEx = static_cast< BitmapEx* >( aBmpExList.GetCurObject() );
    UpdateControl( aBmpExList.GetCurPos() );

    return( 0L );
}

// -----------------------------------------------------------------------

IMPL_LINK( AnimationWindow, ClickStopHdl, void *, EMPTYARG )
{
    bMovie = FALSE;
    return( 0L );
}

// -----------------------------------------------------------------------

IMPL_LINK( AnimationWindow, ClickPlayHdl, void *, p )
{
    bMovie = TRUE;
    BOOL bDisableCtrls = FALSE;
    ULONG nCount = aBmpExList.Count();
    BOOL bReverse = p == &aBtnReverse;

    // Kann spaeter schwer ermittelt werden
    BOOL bRbtGroupEnabled = aRbtGroup.IsEnabled();
    BOOL bBtnGetAllObjectsEnabled = aBtnGetAllObjects.IsEnabled();
    BOOL bBtnGetOneObjectEnabled = aBtnGetOneObject.IsEnabled();

    // Gesamtzeit ermitteln
    Time aTime( 0 );
    long nFullTime;
    if( aRbtBitmap.IsChecked() )
    {
        for( ULONG i = 0; i < nCount; i++ )
            aTime += *static_cast< Time* >( aTimeList.GetObject( i ) );
        nFullTime  = aTime.GetMSFromTime();
    }
    else
    {
        nFullTime = nCount * 100;
        aTime.MakeTimeFromMS( nFullTime );
    }

    // StatusBarManager ab 1 Sekunde
    SfxProgress* pProgress = NULL;
    if( nFullTime >= 1000 )
    {
        bDisableCtrls = TRUE;
        aBtnStop.Enable();
        String aStr( RTL_CONSTASCII_USTRINGPARAM( "Animator:" ) ); // Hier sollte man sich noch etwas gescheites ausdenken!
        pProgress = new SfxProgress( NULL, aStr, nFullTime );
    }

    ULONG nTmpTime = 0;
    long i = 0;
    BOOL bCount = i < (long) nCount;
    if( bReverse )
    {
        i = nCount - 1;
        bCount = i >= 0;
    }
    while( bCount && bMovie )
    {
        // Um Konsistenz zwischen Liste und Anzeige zu erwirken
        aBmpExList.Seek( i );
        pBitmapEx = static_cast< BitmapEx* >(  aBmpExList.GetCurObject() );

        UpdateControl( i, bDisableCtrls );

        if( aRbtBitmap.IsChecked() )
        {
            Time* pTime = static_cast< Time* >( aTimeList.GetObject( i ) );
            DBG_ASSERT( pTime, "Keine Zeit gefunden!" );

            aTimeField.SetTime( *pTime );
            ULONG nTime = pTime->GetMSFromTime();

            WaitInEffect( nTime, nTmpTime, pProgress );
            nTmpTime += nTime;
        }
        else
        {
            WaitInEffect( 100, nTmpTime, pProgress );
            nTmpTime += 100;
        }
        if( bReverse )
        {
            i--;
            bCount = i >= 0;
        }
        else
        {
            i++;
            bCount = i < (long) nCount;
        }
    }

    // Um die Controls wieder zu enablen
    bMovie = FALSE;
    UpdateControl( i - 1 );

    if( pProgress )
    {
        delete pProgress;
        aBtnStop.Disable();
    }

    aRbtGroup.Enable( bRbtGroupEnabled );
    aBtnGetAllObjects.Enable( bBtnGetAllObjectsEnabled );
    aBtnGetOneObject.Enable( bBtnGetOneObjectEnabled );

    return( 0L );
}

// -----------------------------------------------------------------------

IMPL_LINK( AnimationWindow, ClickLastHdl, void *, EMPTYARG )
{
    aBmpExList.Last();
    pBitmapEx = static_cast< BitmapEx* >(  aBmpExList.GetCurObject() );
    UpdateControl( aBmpExList.GetCurPos() );

    return( 0L );
}

// -----------------------------------------------------------------------

IMPL_LINK( AnimationWindow, ClickRbtHdl, void *, p )
{
    if( !pBitmapEx || p == &aRbtGroup || aRbtGroup.IsChecked() )
    {
        aTimeField.SetText( String() );
        aTimeField.Enable( FALSE );
        aLbLoopCount.Enable( FALSE );
    }
    else if( p == &aRbtBitmap || aRbtBitmap.IsChecked() )
    {
        ULONG n = aNumFldBitmap.GetValue() - 1;
        if( n >= 0 )
        {
            Time* pTime = static_cast< Time* >( aTimeList.GetObject( n ) );
            aTimeField.SetTime( *pTime );
        }
        aTimeField.Enable();
        aLbLoopCount.Enable();
    }

    return( 0L );
}

// -----------------------------------------------------------------------

IMPL_LINK( AnimationWindow, ClickGetObjectHdl, void *, pBtn )
{
    bAllObjects = pBtn == &aBtnGetAllObjects;

    // Code jetzt in AddObj()
    SfxBoolItem aItem( SID_ANIMATOR_ADD, TRUE );

    GetBindings().GetDispatcher()->Execute(
        SID_ANIMATOR_ADD, SFX_CALLMODE_SLOT | SFX_CALLMODE_RECORD, &aItem, 0L );
    return( 0L );
}

// -----------------------------------------------------------------------

IMPL_LINK( AnimationWindow, ClickRemoveBitmapHdl, void *, pBtn )
{
    SdPage*     pPage = pMyDoc->GetSdPage(0, PK_STANDARD);
    SdrObject*  pObject;

    if( pBtn == &aBtnRemoveBitmap )
    {
        ULONG nPos = aBmpExList.GetCurPos();
        pBitmapEx = static_cast< BitmapEx* >( aBmpExList.GetCurObject() );
        if( pBitmapEx )
        {
            delete pBitmapEx;
            aBmpExList.Remove();
            pBitmapEx = static_cast< BitmapEx* >( aBmpExList.GetCurObject() );
        }
        Time* pTime = static_cast< Time* >( aTimeList.GetObject( nPos ) );
        if( pTime )
        {
            delete pTime;
            aTimeList.Remove( nPos );
        }

        pObject = pPage->GetObj( nPos );
        // Durch Uebernahme der AnimatedGIFs muessen nicht unbedingt
        // Objekte vorhanden sein.
        if( pObject )
        {
            pObject = pPage->RemoveObject(nPos);
            DBG_ASSERT(pObject, "Clone beim Loeschen nicht gefunden");
            delete pObject;
            pPage->RecalcObjOrdNums();
        }

    }
    else // Alles loeschen
    {
        WarningBox aWarnBox( this, WB_YES_NO, String( SdResId( STR_ASK_DELETE_ALL_PICTURES ) ) );
        short nReturn = aWarnBox.Execute();

        if( nReturn == RET_YES )
        {
            // Bitmapliste bereinigen
            long nCount = aBmpExList.Count();
            long i;

            for( i = nCount - 1; i >= 0; i-- )
            {
                pBitmapEx = static_cast< BitmapEx* >( aBmpExList.GetObject( i ) );
                delete pBitmapEx;

                pObject = pPage->GetObj( i );
                if( pObject )
                {
                    pObject = pPage->RemoveObject( i );
                    DBG_ASSERT(pObject, "Clone beim Loeschen nicht gefunden");
                    delete pObject;
                    //pPage->RecalcObjOrdNums();
                }

            }
            aBmpExList.Clear();

            // Timeliste bereinigen
            nCount = aTimeList.Count();
            for( i = 0; i < nCount; i++ )
            {
                delete static_cast< Time* >( aTimeList.GetObject( i ) );
            }
            aTimeList.Clear();
        }
    }

    // kann noch eine Animationsgruppe erstellt werden?
    if (aBmpExList.Count() == 0)
    {
        aBtnCreateGroup.Disable();
        // Falls vorher durch Uebernahme von AnimatedGIFs disabled:
        //aRbtBitmap.Enable();
        aRbtGroup.Enable();
    }

    // Zoom fuer DisplayWin berechnen und setzen
    Fraction aFrac( GetScale() );
    aCtlDisplay.SetScale( aFrac );

    UpdateControl( aBmpExList.GetCurPos() );

    return( 0L );
}

// -----------------------------------------------------------------------

IMPL_LINK( AnimationWindow, ClickCreateGroupHdl, void *, EMPTYARG )
{
    // Code jetzt in CreatePresObj()
    SfxBoolItem aItem( SID_ANIMATOR_CREATE, TRUE );

    GetBindings().GetDispatcher()->Execute(
        SID_ANIMATOR_CREATE, SFX_CALLMODE_SLOT | SFX_CALLMODE_RECORD, &aItem, 0L );
    return( 0L );
}

// -----------------------------------------------------------------------

IMPL_LINK( AnimationWindow, ModifyBitmapHdl, void *, EMPTYARG )
{
    ULONG nBmp = aNumFldBitmap.GetValue();

    if( nBmp > aBmpExList.Count() )
        nBmp = aBmpExList.Count();

    pBitmapEx = static_cast< BitmapEx* >( aBmpExList.GetObject( nBmp - 1 ) );

    // Positionieren in der Liste
    aBmpExList.Seek( nBmp - 1 );

    UpdateControl( nBmp - 1 );

    return( 0L );
}

// -----------------------------------------------------------------------

IMPL_LINK( AnimationWindow, ModifyTimeHdl, void *, EMPTYARG )
{
    ULONG nPos = aNumFldBitmap.GetValue() - 1;

    Time* pTime = static_cast< Time* >( aTimeList.GetObject( nPos ) );
    DBG_ASSERT( pTime, "Zeit nicht gefunden!" )

    *pTime = aTimeField.GetTime();

    return( 0L );
}

// -----------------------------------------------------------------------

void AnimationWindow::UpdateControl( ULONG nListPos, BOOL bDisableCtrls )
{
    String aString;

    if( pBitmapEx )
    {
        BitmapEx aBmp( *pBitmapEx );

        SdPage* pPage = pMyDoc->GetSdPage(0, PK_STANDARD);
        SdrObject* pObject = (SdrObject*) pPage->GetObj( (ULONG) nListPos );
        if( pObject )
        {
            SdrPaintInfoRec aPaintInfoRec;
            VirtualDevice   aVD;
            Rectangle       aObjRect( pObject->GetCurrentBoundRect() );
            Size            aObjSize( aObjRect.GetSize() );
            Point           aOrigin( Point( -aObjRect.Left(), -aObjRect.Top() ) );
            MapMode         aMap( aVD.GetMapMode() );
            aMap.SetMapUnit( MAP_100TH_MM );
            aMap.SetOrigin( aOrigin );
            aVD.SetMapMode( aMap );
            aVD.SetOutputSize( aObjSize );
            const StyleSettings& rStyles = Application::GetSettings().GetStyleSettings();
            aVD.SetBackground( Wallpaper( rStyles.GetFieldColor() ) );
            //AdjustVDev( &aVD );
            ExtOutputDevice aOut( &aVD );
            aVD.SetDrawMode( GetDisplayBackground().GetColor().IsDark()
                ? ViewShell::OUTPUT_DRAWMODE_CONTRAST
                : ViewShell::OUTPUT_DRAWMODE_COLOR );
            aVD.Erase();
            pObject->SingleObjectPainter( aOut, aPaintInfoRec ); // #110094#-17
            aBmp = BitmapEx( aVD.GetBitmap( aObjRect.TopLeft(), aObjSize ) );
        }


        aCtlDisplay.SetBitmapEx( &aBmp );
    }
    else
    {
        aCtlDisplay.SetBitmapEx( pBitmapEx );
    }
    aCtlDisplay.Invalidate();
    aCtlDisplay.Update();

    aFiCount.SetText( UniString::CreateFromInt32( aBmpExList.Count() ) );

    if( pBitmapEx && !bMovie )
    {
        aNumFldBitmap.SetValue( nListPos + 1 );

        // Wenn mind. 1 Objekt in der Liste ist
        aBtnFirst.Enable();
        aBtnReverse.Enable();
        aBtnPlay.Enable();
        aBtnLast.Enable();
        aNumFldBitmap.Enable();
        aTimeField.Enable();
        aLbLoopCount.Enable();
        aBtnRemoveBitmap.Enable();
        aBtnRemoveAll.Enable();
    }
    else
    {
        //aFiCount.SetText( String( SdResId( STR_NULL ) ) );

        // Wenn kein Objekt in der Liste ist
        aBtnFirst.Enable( FALSE );
        aBtnReverse.Enable( FALSE );
        aBtnPlay.Enable( FALSE );
        aBtnLast.Enable( FALSE );
        aNumFldBitmap.Enable( FALSE );
        aTimeField.Enable( FALSE );
        aLbLoopCount.Enable( FALSE );
        aBtnRemoveBitmap.Enable( FALSE );
        aBtnRemoveAll.Enable( FALSE );

        //aFtAdjustment.Enable();
        //aLbAdjustment.Enable();
    }

    if( bMovie && bDisableCtrls )
    {
        aBtnGetOneObject.Enable( FALSE );
        aBtnGetAllObjects.Enable( FALSE );
        aRbtGroup.Enable( FALSE );
        aRbtBitmap.Enable( FALSE );
        aBtnCreateGroup.Enable( FALSE );
        aFtAdjustment.Enable( FALSE );
        aLbAdjustment.Enable( FALSE );
    }
    else
    {
        // 'Gruppenobjekt' nur dann enablen, wenn es kein Animated GIF ist
        if (aBmpExList.Count() == 0)
            aRbtGroup.Enable();

        aRbtBitmap.Enable();
        aBtnCreateGroup.Enable(aBmpExList.Count() != 0);
        aFtAdjustment.Enable( TRUE );
        aLbAdjustment.Enable( TRUE );
    }

    ClickRbtHdl( NULL );
}

// -----------------------------------------------------------------------

void AnimationWindow::ResetAttrs()
{
    aRbtGroup.Check();
    aLbAdjustment.SelectEntryPos( BA_CENTER );
    // LoopCount
    aLbLoopCount.SelectEntryPos( aLbLoopCount.GetEntryCount() - 1);

    UpdateControl( 0 );
}

// -----------------------------------------------------------------------

void AnimationWindow::WaitInEffect( ULONG nMilliSeconds ) const
{
    ULONG nEnd = Time::GetSystemTicks() + nMilliSeconds;
    ULONG nCurrent = Time::GetSystemTicks();
    while (nCurrent < nEnd)
    {
        nCurrent = Time::GetSystemTicks();
    }
}

// -----------------------------------------------------------------------

void AnimationWindow::WaitInEffect( ULONG nMilliSeconds, ULONG nTime,
                                    SfxProgress* pProgress ) const
{
    clock_t aEnd = Time::GetSystemTicks() + nMilliSeconds;
    clock_t aCurrent = Time::GetSystemTicks();
    while (aCurrent < aEnd)
    {
        aCurrent = Time::GetSystemTicks();

        if( pProgress )
            pProgress->SetState( nTime + nMilliSeconds + aCurrent - aEnd );

        if( !bMovie )
            return;
    }
}

// -----------------------------------------------------------------------

Fraction AnimationWindow::GetScale()
{
    Fraction aFrac;
    ULONG nPos = aBmpExList.GetCurPos();
    ULONG nCount = aBmpExList.Count();
    if( nCount > 0 )
    {
        aBmpSize.Width() = 0;
        aBmpSize.Height() = 0;
        for( ULONG i = 0; i < nCount; i++ )
        {
            pBitmapEx = static_cast< BitmapEx* >( aBmpExList.GetObject( i ) );
            Size aSize( pBitmapEx->GetBitmap().GetSizePixel() );
            aBmpSize.Width() = Max( aBmpSize.Width(), aSize.Width() );
            aBmpSize.Height() = Max( aBmpSize.Height(), aSize.Height() );
        }

        aBmpSize.Width() += 10;
        aBmpSize.Height() += 10;

        aFrac = Fraction( std::min( (double)aDisplaySize.Width() / (double)aBmpSize.Width(),
                             (double)aDisplaySize.Height() / (double)aBmpSize.Height() ) );
    }
    // Liste wieder auf alten Stand bringen
    pBitmapEx = static_cast< BitmapEx* >( aBmpExList.GetObject( nPos ) );
    return( aFrac );
}

// -----------------------------------------------------------------------

void AnimationWindow::Resize()
{
    //if( !IsZoomedIn() )
    if ( !IsFloatingMode() ||
         !GetFloatingWindow()->IsRollUp() )
    {
        Size aWinSize( GetOutputSizePixel() ); // vorher rSize im Resizing()

        Size aDiffSize;
        aDiffSize.Width() = aWinSize.Width() - aSize.Width();
        aDiffSize.Height() = aWinSize.Height() - aSize.Height();

        // Umgroessern des Display-Controls
        aDisplaySize.Width() += aDiffSize.Width();
        aDisplaySize.Height() += aDiffSize.Height();
        aCtlDisplay.SetOutputSizePixel( aDisplaySize );

        Point aPt;
        // aPt.X() = aDiffSize.Width() / 2;
        aPt.Y() = aDiffSize.Height();

        // Verschieben der anderen Controls
        aBtnFirst.Hide();
        aBtnReverse.Hide();
        aBtnStop.Hide();
        aBtnPlay.Hide();
        aBtnLast.Hide();
        aTimeField.Hide();
        aLbLoopCount.Hide();
        aNumFldBitmap.Hide();
        aFtCount.Hide();
        aFiCount.Hide();
        aBtnGetOneObject.Hide();
        aBtnGetAllObjects.Hide();
        aBtnRemoveBitmap.Hide();
        aBtnRemoveAll.Hide();
        aGrpBitmap.Hide();
        aRbtGroup.Hide();
        aRbtBitmap.Hide();
        aFtAdjustment.Hide();
        aLbAdjustment.Hide();
        aBtnCreateGroup.Hide();
        aGrpAnimation.Hide();


        aBtnFirst.SetPosPixel( aBtnFirst.GetPosPixel() + aPt );
        aBtnReverse.SetPosPixel( aBtnReverse.GetPosPixel() + aPt );
        aBtnStop.SetPosPixel( aBtnStop.GetPosPixel() + aPt );
        aBtnPlay.SetPosPixel( aBtnPlay.GetPosPixel() + aPt );
        aBtnLast.SetPosPixel( aBtnLast.GetPosPixel() + aPt );
        aNumFldBitmap.SetPosPixel( aNumFldBitmap.GetPosPixel() + aPt );
        aTimeField.SetPosPixel( aTimeField.GetPosPixel() + aPt );
        aLbLoopCount.SetPosPixel( aLbLoopCount.GetPosPixel() + aPt );
        aFtCount.SetPosPixel( aFtCount.GetPosPixel() + aPt );
        aFiCount.SetPosPixel( aFiCount.GetPosPixel() + aPt );
        aRbtGroup.SetPosPixel( aRbtGroup.GetPosPixel() + aPt );
        aRbtBitmap.SetPosPixel( aRbtBitmap.GetPosPixel() + aPt );
        aFtAdjustment.SetPosPixel( aFtAdjustment.GetPosPixel() + aPt );
        aLbAdjustment.SetPosPixel( aLbAdjustment.GetPosPixel() + aPt );
        aBtnGetOneObject.SetPosPixel( aBtnGetOneObject.GetPosPixel() + aPt );
        aBtnGetAllObjects.SetPosPixel( aBtnGetAllObjects.GetPosPixel() + aPt );
        aBtnRemoveBitmap.SetPosPixel( aBtnRemoveBitmap.GetPosPixel() + aPt );
        aBtnRemoveAll.SetPosPixel( aBtnRemoveAll.GetPosPixel() + aPt );
        aBtnCreateGroup.SetPosPixel( aBtnCreateGroup.GetPosPixel() + aPt );
        aGrpBitmap.SetPosPixel( aGrpBitmap.GetPosPixel() + aPt );
        aGrpAnimation.SetPosPixel( aGrpAnimation.GetPosPixel() + aPt );

        // Zoom fuer DisplayWin berechnen und setzen
        Fraction aFrac( GetScale() );
        aCtlDisplay.SetScale( aFrac );

        aBtnFirst.Show();
        aBtnReverse.Show();
        aBtnStop.Show();
        aBtnPlay.Show();
        aBtnLast.Show();
        aNumFldBitmap.Show();
        aTimeField.Show();
        aLbLoopCount.Show();
        aFtCount.Show();
        aFiCount.Show();
        aFtAdjustment.Show();
        aLbAdjustment.Show();
        aBtnGetOneObject.Show();
        aBtnGetAllObjects.Show();
        aBtnRemoveBitmap.Show();
        aBtnRemoveAll.Show();
        aGrpBitmap.Show();
        aRbtGroup.Show();
        aRbtBitmap.Show();
        aFtAdjustment.Show();
        aLbAdjustment.Show();
        aBtnCreateGroup.Show();
        aGrpAnimation.Show();

        aSize = aWinSize;

        //aFltWinSize = GetSizePixel();
    }
    SfxDockingWindow::Resize();
}

// -----------------------------------------------------------------------

BOOL AnimationWindow::Close()
{
    SfxBoolItem aItem( SID_ANIMATION_OBJECTS, FALSE );

    GetBindings().GetDispatcher()->Execute(
        SID_ANIMATION_OBJECTS, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD, &aItem, 0L );

    SfxDockingWindow::Close();

    return( TRUE );
}

// -----------------------------------------------------------------------

void AnimationWindow::FillInfo( SfxChildWinInfo& rInfo ) const
{
    SfxDockingWindow::FillInfo( rInfo ) ;
}

// -----------------------------------------------------------------------

void AnimationWindow::AddObj (::sd::View& rView )
{
    // Texteingabemodus beenden, damit Bitmap mit
    // Objekt identisch ist.
    if( rView.IsTextEdit() )
        rView.EndTextEdit();

    // Objekt(e) clonen und den/die Clone(s) in die Liste stellen
    const SdrMarkList& rMarkList   = rView.GetMarkList();
    ULONG              nMarkCount  = rMarkList.GetMarkCount();
    SdPage*            pPage       = pMyDoc->GetSdPage(0, PK_STANDARD);
    ULONG              nCloneCount = pPage->GetObjCount();

    if (nMarkCount > 0)
    {
        // Wenn es sich um EIN Animationsobjekt oder ein Gruppenobjekt
        // handelt, das 'einzeln uebernommen' wurde,
        // werden die einzelnen Objekte eingefuegt
        BOOL bAnimObj = FALSE;
        if( nMarkCount == 1 )
        {
            SdrMark*            pMark = rMarkList.GetMark(0);
            SdrObject*          pObject = pMark->GetObj();
            SdAnimationInfo*    pAnimInfo = rView.GetDoc()->GetAnimationInfo( pObject );
            UINT32              nInv = pObject->GetObjInventor();
            UINT16              nId = pObject->GetObjIdentifier();

            // Animated Bitmap (GIF)
            if( nInv == SdrInventor && nId == OBJ_GRAF && ( (SdrGrafObj*) pObject )->IsAnimated() )
            {
                const SdrGrafObj*   pGrafObj = (SdrGrafObj*) pObject;
                Graphic             aGraphic( pGrafObj->GetTransformedGraphic() );
                USHORT              nCount = 0;

                if( aGraphic.IsAnimated() )
                    nCount = aGraphic.GetAnimation().Count();

                if( nCount > 0 )
                {
                    const Animation aAnimation( aGraphic.GetAnimation() );

                    for( USHORT i = 0; i < nCount; i++ )
                    {
                        const AnimationBitmap& rAnimBmp = aAnimation.Get( i );

                        pBitmapEx = new BitmapEx( rAnimBmp.aBmpEx );
                        aBmpExList.Insert( pBitmapEx, aBmpExList.GetCurPos() + 1 );

                        // LoopCount
                        if( i == 0 )
                        {
                            long nLoopCount = aAnimation.GetLoopCount();

                            if( !nLoopCount ) // unendlich
                                aLbLoopCount.SelectEntryPos( aLbLoopCount.GetEntryCount() - 1);
                            else
                                aLbLoopCount.SelectEntry( UniString::CreateFromInt32( nLoopCount ) );
                        }

                        // Time
                        long nTime = rAnimBmp.nWait;
                        Time* pTime = new Time( 0, 0, nTime / 100, nTime % 100 );
                        aTimeList.Insert( pTime, aBmpExList.GetCurPos() + 1 );

                        // Weiterschalten der BitmapListe
                        aBmpExList.Next();
                    }
                    // Nachdem ein animated GIF uebernommen wurde, kann auch nur ein solches erstellt werden
                    aRbtBitmap.Check();
                    aRbtGroup.Enable( FALSE );
                    bAnimObj = TRUE;
                }
            }
            else if( bAllObjects || ( pAnimInfo && pAnimInfo->bIsMovie ) )
            {
                // Mehrere Objekte
                SdrObjList* pObjList = ((SdrObjGroup*)pObject)->GetSubList();

                for( USHORT nObject = 0; nObject < pObjList->GetObjCount(); nObject++ )
                {
                    SdrObject* pSnapShot = (SdrObject*) pObjList->GetObj( (ULONG) nObject );

                    pBitmapEx = new BitmapEx( SdrExchangeView::GetObjGraphic( pSnapShot->GetModel(), pSnapShot ).GetBitmapEx() );
                    aBmpExList.Insert( pBitmapEx, aBmpExList.GetCurPos() + 1 );

                    // Time
                    Time* pTime = new Time( aTimeField.GetTime() );
                    aTimeList.Insert( pTime, aBmpExList.GetCurPos() + 1 );

                    // Clone
                    pPage->InsertObject( pSnapShot->Clone(), aBmpExList.GetCurPos() + 1 );

                    // Weiterschalten der BitmapListe
                    aBmpExList.Next();
                }
                bAnimObj = TRUE;
            }
        }
        // Auch ein einzelnes animiertes Objekt
        if( !bAnimObj && !( bAllObjects && nMarkCount > 1 ) )
        {
            pBitmapEx = new BitmapEx( rView.GetAllMarkedGraphic().GetBitmapEx() );
            aBmpExList.Insert( pBitmapEx, aBmpExList.GetCurPos() + 1 );

            // Time
            Time* pTime = new Time( aTimeField.GetTime() );
            aTimeList.Insert( pTime, aBmpExList.GetCurPos() + 1 );

        }

        // ein einzelnes Objekt
        if( nMarkCount == 1 && !bAnimObj )
        {
            SdrMark*    pMark   = rMarkList.GetMark(0);
            SdrObject*  pObject = pMark->GetObj();
            SdrObject*  pClone  = pObject->Clone();
            pPage->InsertObject(pClone, aBmpExList.GetCurPos() + 1);
        }
        // mehrere Objekte: die Clones zu einer Gruppe zusammenfassen
        else if (nMarkCount > 1)
        {
            // Objekte einzeln uebernehmen
            if( bAllObjects )
            {
                for( ULONG nObject= 0; nObject < nMarkCount; nObject++ )
                {
                    // Clone
                    SdrObject* pObject = rMarkList.GetMark( nObject )->GetObj();

                    pBitmapEx = new BitmapEx( SdrExchangeView::GetObjGraphic( pObject->GetModel(), pObject ).GetBitmapEx() );
                    aBmpExList.Insert( pBitmapEx, aBmpExList.GetCurPos() + 1 );

                    // Time
                    Time* pTime = new Time( aTimeField.GetTime() );
                    aTimeList.Insert( pTime, aBmpExList.GetCurPos() + 1 );

                    pPage->InsertObject( pObject->Clone(), aBmpExList.GetCurPos() + 1 );

                    aBmpExList.Next();
                }
                bAnimObj = TRUE; // damit nicht nochmal weitergeschaltet wird
            }
            else
            {
                SdrObjGroup* pCloneGroup = new SdrObjGroup;
                SdrObjList*  pObjList    = pCloneGroup->GetSubList();

                for (ULONG nObject= 0; nObject < nMarkCount; nObject++)
                    pObjList->InsertObject(rMarkList.GetMark(nObject)->GetObj()->Clone(), LIST_APPEND);

                pPage->InsertObject(pCloneGroup, aBmpExList.GetCurPos() + 1);
            }
        }

        if( !bAnimObj )
            aBmpExList.Next();

        // wenn vorher nichts im Animator war und jetzt was da ist, kann eine
        // Animationsgruppe erstellt werden
        if (nCloneCount == 0 && aBmpExList.Count() > 0)
            aBtnCreateGroup.Enable();

        // Zoom fuer DisplayWin berechnen und setzen
        Fraction aFrac( GetScale() );
        aCtlDisplay.SetScale( aFrac );

        UpdateControl( aBmpExList.GetCurPos() );
    }
}

// -----------------------------------------------------------------------

void AnimationWindow::CreateAnimObj (::sd::View& rView )
{
    ::Window* pOutWin = static_cast< ::Window*>(rView.GetWin(0));
    DBG_ASSERT( pOutWin, "Window ist nicht vorhanden!" );

    // die Fentermitte ermitteln
    const MapMode       aMap100( MAP_100TH_MM );
    Size                aMaxSizeLog;
    Size                aMaxSizePix;
    Size                aTemp( pOutWin->GetOutputSizePixel() );
    const Point         aWindowCenter( pOutWin->PixelToLogic( Point( aTemp.Width() >> 1, aTemp.Height() >> 1 ) ) );
    const OutputDevice* pDefDev = Application::GetDefaultDevice();
    const ULONG         nCount = aBmpExList.Count();
    BitmapAdjustment    eBA = (BitmapAdjustment) aLbAdjustment.GetSelectEntryPos();
    ULONG               i;

    // Groesste Bitmap ermitteln
    for( i = 0; i < nCount; i++ )
    {
        const BitmapEx& rBmpEx = *static_cast< BitmapEx* >( aBmpExList.GetObject( i ) );
        const Graphic   aGraphic( rBmpEx );
        Size            aTmpSizeLog;
        const Size      aTmpSizePix( rBmpEx.GetSizePixel() );

        if ( aGraphic.GetPrefMapMode().GetMapUnit() == MAP_PIXEL )
            aTmpSizeLog = pDefDev->PixelToLogic( aGraphic.GetPrefSize(), aMap100 );
        else
            aTmpSizeLog = pDefDev->LogicToLogic( aGraphic.GetPrefSize(), aGraphic.GetPrefMapMode(), aMap100 );

        aMaxSizeLog.Width() = Max( aMaxSizeLog.Width(), aTmpSizeLog.Width() );
        aMaxSizeLog.Height() = Max( aMaxSizeLog.Height(), aTmpSizeLog.Height() );

        aMaxSizePix.Width() = Max( aMaxSizePix.Width(), aTmpSizePix.Width() );
        aMaxSizePix.Height() = Max( aMaxSizePix.Height(), aTmpSizePix.Height() );
    }

    SdrPageView* pPV = rView.GetPageViewPvNum( 0 );

    if( aRbtBitmap.IsChecked() )
    {
        // Bitmapgruppe erzeugen (Animated GIF)
        Animation   aAnimation;
        Point       aPt;

        for( i = 0; i < nCount; i++ )
        {
            Time* pTime = static_cast< Time* >( aTimeList.GetObject( i ) );
            long  nTime = pTime->Get100Sec();
            nTime += pTime->GetSec() * 100;

            pBitmapEx = static_cast< BitmapEx* >( aBmpExList.GetObject( i ) );

            // Offset fuer die gewuenschte Ausrichtung bestimmen
            const Size aBmpSize( pBitmapEx->GetSizePixel() );

            switch( eBA )
            {
                case BA_LEFT_UP:
                break;

                case BA_LEFT:
                    aPt.Y() = (aMaxSizePix.Height() - aBmpSize.Height()) >> 1;
                break;

                case BA_LEFT_DOWN:
                    aPt.Y() = aMaxSizePix.Height() - aBmpSize.Height();
                break;

                case BA_UP:
                    aPt.X() = (aMaxSizePix.Width() - aBmpSize.Width()) >> 1;
                break;

                case BA_CENTER:
                    aPt.X()  = (aMaxSizePix.Width() - aBmpSize.Width()) >> 1;
                    aPt.Y() = (aMaxSizePix.Height() - aBmpSize.Height()) >> 1;
                break;

                case BA_DOWN:
                    aPt.X()  = (aMaxSizePix.Width() - aBmpSize.Width()) >> 1;
                    aPt.Y() = aMaxSizePix.Height() - aBmpSize.Height();
                break;

                case BA_RIGHT_UP:
                    aPt.X() = aMaxSizePix.Width() - aBmpSize.Width();
                break;

                case BA_RIGHT:
                    aPt.X()  = aMaxSizePix.Width() - aBmpSize.Width();
                    aPt.Y() = (aMaxSizePix.Height() - aBmpSize.Height()) >> 1;
                break;

                case BA_RIGHT_DOWN:
                    aPt.X()  = aMaxSizePix.Width() - aBmpSize.Width();
                    aPt.Y() = aMaxSizePix.Height() - aBmpSize.Height();
                break;

            }

            // LoopCount (Anzahl der Durchlaeufe) ermitteln
            AnimationBitmap aAnimBmp;
            long            nLoopCount = 0L;
            USHORT          nPos = aLbLoopCount.GetSelectEntryPos();

            if( nPos != LISTBOX_ENTRY_NOTFOUND && nPos != aLbLoopCount.GetEntryCount() - 1 ) // unendlich
                nLoopCount = (long) aLbLoopCount.GetSelectEntry().ToInt32();

            aAnimBmp.aBmpEx = *pBitmapEx;
            aAnimBmp.aPosPix = aPt;
            aAnimBmp.aSizePix = aBmpSize;
            aAnimBmp.nWait = nTime;
            aAnimBmp.eDisposal = DISPOSE_BACK;
            aAnimBmp.bUserInput = FALSE;

            aAnimation.Insert( aAnimBmp );
            aAnimation.SetDisplaySizePixel( aMaxSizePix );
            aAnimation.SetLoopCount( nLoopCount );
        }

        SdrGrafObj* pGrafObj = new SdrGrafObj( Graphic( aAnimation ) );
        const Point aOrg( aWindowCenter.X() - ( aMaxSizeLog.Width() >> 1 ), aWindowCenter.Y() - ( aMaxSizeLog.Height() >> 1 ) );

        pGrafObj->SetLogicRect( Rectangle( aOrg, aMaxSizeLog ) );
        rView.InsertObject( pGrafObj, *pPV, SDRINSERT_SETDEFLAYER);
    }
    else
    {
        // Offset fuer die gewuenschte Ausrichtung bestimmen
        Size aOffset;
        SdrObject * pClone = NULL;
        SdPage* pPage = pMyDoc->GetSdPage(0, PK_STANDARD);

        for(i = 0; i < nCount; i++)
        {
            pClone = pPage->GetObj(i);
            Rectangle aRect( pClone->GetSnapRect() );

            switch( eBA )
            {
                case BA_LEFT_UP:
                break;

                case BA_LEFT:
                    aOffset.Height() = (aMaxSizeLog.Height() - aRect.GetHeight()) / 2;
                break;

                case BA_LEFT_DOWN:
                    aOffset.Height() = aMaxSizeLog.Height() - aRect.GetHeight();
                break;

                case BA_UP:
                    aOffset.Width() = (aMaxSizeLog.Width() - aRect.GetWidth()) / 2;
                break;

                case BA_CENTER:
                    aOffset.Width()  = (aMaxSizeLog.Width() - aRect.GetWidth()) / 2;
                    aOffset.Height() = (aMaxSizeLog.Height() - aRect.GetHeight()) / 2;
                break;

                case BA_DOWN:
                    aOffset.Width()  = (aMaxSizeLog.Width() - aRect.GetWidth()) / 2;
                    aOffset.Height() = aMaxSizeLog.Height() - aRect.GetHeight();
                break;

                case BA_RIGHT_UP:
                    aOffset.Width() = aMaxSizeLog.Width() - aRect.GetWidth();
                break;

                case BA_RIGHT:
                    aOffset.Width()  = aMaxSizeLog.Width() - aRect.GetWidth();
                    aOffset.Height() = (aMaxSizeLog.Height() - aRect.GetHeight()) / 2;
                break;

                case BA_RIGHT_DOWN:
                    aOffset.Width()  = aMaxSizeLog.Width() - aRect.GetWidth();
                    aOffset.Height() = aMaxSizeLog.Height() - aRect.GetHeight();
                break;

            }
            //aRect.SetPos(aWindowCenter + Point(aOffset.Width(), aOffset.Height()));
            //pClone->SetSnapRect( aRect );
            // SetSnapRect ist fuer Ellipsen leider nicht implementiert !!!
            Point aMovePt( aWindowCenter + Point( aOffset.Width(), aOffset.Height() ) - aRect.TopLeft() );
            Size aMoveSize( aMovePt.X(), aMovePt.Y() );
            pClone->NbcMove( aMoveSize );
        }

        // Animationsgruppe erzeugen
        SdrObjGroup* pGroup   = new SdrObjGroup;
        SdrObjList*  pObjList = pGroup->GetSubList();

        for (i = 0; i < nCount; i++)
        {
            // der Clone verbleibt im Animator; in die Gruppe kommt ein Clone
            // des Clones
            pClone = pPage->GetObj(i);
            SdrObject* pCloneOfClone = pClone->Clone();
            //SdrObject* pCloneOfClone = pPage->GetObj(i)->Clone();
            pObjList->InsertObject(pCloneOfClone, LIST_APPEND);
        }

        // bis jetzt liegt die linke obere Ecke der Gruppe in der Fenstermitte;
        // jetzt noch um die Haelfte der Groesse nach oben und links korrigieren
        aTemp = aMaxSizeLog;
        aTemp.Height() = - aTemp.Height() / 2;
        aTemp.Width()  = - aTemp.Width() / 2;
        pGroup->NbcMove(aTemp);

        // Animationsinformation erzeugen
        SdDrawDocument* pDoc = (SdDrawDocument*)rView.GetModel();
        SdAnimationInfo* pInfo = new SdAnimationInfo(pDoc);
        // pInfo->aStart = Point();
        pInfo->eEffect = presentation::AnimationEffect_NONE;
        pInfo->eSpeed = presentation::AnimationSpeed_MEDIUM;
        pInfo->bActive = TRUE;
        pInfo->bIsMovie = TRUE;
        pInfo->aBlueScreen = COL_WHITE;
        pGroup->InsertUserData( pInfo );

        rView.InsertObject( pGroup, *pPV, SDRINSERT_SETDEFLAYER);
    }

    ClickFirstHdl( this );
}

void AnimationWindow::DataChanged( const DataChangedEvent& rDCEvt )
{
    SfxDockingWindow::DataChanged( rDCEvt );

    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) && (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        UpdateControl( aBmpExList.GetCurPos() );
    }
}

/*************************************************************************
|*
|* ControllerItem fuer Animator
|*
\************************************************************************/

AnimationControllerItem::AnimationControllerItem(
    USHORT nId,
    AnimationWindow* pAnimWin,
    SfxBindings*    pBindings)
    : SfxControllerItem( nId, *pBindings ),
      pAnimationWin( pAnimWin )
{
}

// -----------------------------------------------------------------------

void AnimationControllerItem::StateChanged( USHORT nSId,
                        SfxItemState eState, const SfxPoolItem* pItem )
{
    if( eState >= SFX_ITEM_AVAILABLE && nSId == SID_ANIMATOR_STATE )
    {
        const SfxUInt16Item* pStateItem = PTR_CAST( SfxUInt16Item, pItem );
        DBG_ASSERT( pStateItem, "SfxUInt16Item erwartet");
        UINT16 nState = pStateItem->GetValue();

        pAnimationWin->aBtnGetOneObject.Enable( nState & 1 );
        pAnimationWin->aBtnGetAllObjects.Enable( nState & 2 );
    }
}


} // end of namespace sd
