/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sfx2.hxx"

// include ---------------------------------------------------------------
#include <aboutbmpnames.hxx>

#include <vcl/svapp.hxx>
#include <vcl/msgbox.hxx>

#include <tools/stream.hxx>
#include <tools/urlobj.hxx>
#include <rtl/bootstrap.hxx>
#include <unotools/configmgr.hxx>
#include <unotools/bootstrap.hxx>
#include <com/sun/star/uno/Any.h>
#include <unotools/configmgr.hxx>
#include <vcl/svapp.hxx>
#include <vcl/graph.hxx>
#include <svtools/filter.hxx>

#include <sfx2/sfxuno.hxx>
#include "about.hxx"
#include "sfxresid.hxx"
#include <sfx2/sfxdefs.hxx>
#include <sfx2/app.hxx>

#include "dialog.hrc"

// defines ---------------------------------------------------------------

#define SCROLL_OFFSET   1
#define SPACE_OFFSET    5
#define SCROLL_TIMER    30

#define WELCOME_URL     DEFINE_CONST_UNICODE( "http://www.openoffice.org/welcome/credits.html" )

/** loads the application logo as used in the about dialog and impress slideshow pause screen */
Image SfxApplication::GetApplicationLogo()
{
    BitmapEx aBitmap;
    Application::LoadBrandBitmap ("about", aBitmap);
    return Image( aBitmap );
}

AboutDialog::AboutDialog( Window* pParent, const ResId& rId, const String& rVerStr ) :

    SfxModalDialog  ( pParent,  rId ),

    aOKButton       ( this,     ResId( ABOUT_BTN_OK, *rId.GetResMgr() ) ),
    aVersionText    ( this,     ResId( ABOUT_FTXT_VERSION, *rId.GetResMgr() ) ),
    aCopyrightText  ( this,     ResId( ABOUT_FTXT_COPYRIGHT, *rId.GetResMgr() ) ),
    // FIXME: What is the purpose of the aBuildData when it is not connected to any widget?
    aBuildData      ( this ),
    aDeveloperAry   (           ResId( ABOUT_STR_DEVELOPER_ARY, *rId.GetResMgr() ) ),
    aDevVersionStr  ( rVerStr ),
    aAccelStr       (           ResId( ABOUT_STR_ACCEL, *rId.GetResMgr() ) ),
    aVersionTextStr(          ResId( ABOUT_STR_VERSION, *rId.GetResMgr() ) ),
    aCopyrightTextStr(          ResId( ABOUT_STR_COPYRIGHT, *rId.GetResMgr() ) ),
    aTimer          (),
    nOff            ( 0 ),
    m_nDeltaWidth   ( 0 ),
    m_nPendingScrolls( 0 ),
    bNormal         ( TRUE )
{
    rtl::OUString sProduct;
    utl::ConfigManager::GetDirectConfigProperty(utl::ConfigManager::PRODUCTNAME) >>= sProduct;

    // load image from module path
    aAppLogo = SfxApplication::GetApplicationLogo();

    // Transparenter Font
    Font aFont = GetFont();
    aFont.SetTransparent( TRUE );
    SetFont( aFont );

    // if necessary more info
    String sVersion = aVersionTextStr;
    sVersion.SearchAndReplaceAscii( "$(VER)", Application::GetDisplayName() );
    sVersion += '\n';
    sVersion += rVerStr;
#ifdef BUILD_VER_STRING
    String aBuildString( DEFINE_CONST_UNICODE( BUILD_VER_STRING ) );
    sVersion += '\n';
    sVersion += aBuildString;
#endif
    aVersionText.SetText( sVersion );

    // Initialisierung fuer Aufruf Entwickler
    if ( aAccelStr.Len() && ByteString(U2S(aAccelStr)).IsAlphaAscii() )
    {
        Accelerator *pAccel = 0, *pPrevAccel = 0, *pFirstAccel = 0;
        aAccelStr.ToUpperAscii();

        for ( USHORT i = 0; i < aAccelStr.Len(); ++i )
        {
            pPrevAccel = pAccel;
            pAccel = new Accelerator;
            aAccelList.Insert( pAccel, LIST_APPEND );
            USHORT nKey = aAccelStr.GetChar(i) - 'A' + KEY_A;
            pAccel->InsertItem( 1, KeyCode( nKey, KEY_MOD1 ) );
            if ( i > 0 )
                pPrevAccel->SetAccel( 1, pAccel );
            if ( i == 0 )
                pFirstAccel = pAccel;
        }
        pAccel->SetSelectHdl( LINK( this, AboutDialog, AccelSelectHdl ) );
        GetpApp()->InsertAccel( pFirstAccel );
    }

    // set for background and text the correct system color
    const StyleSettings& rSettings = GetSettings().GetStyleSettings();
    Color aWhiteCol( rSettings.GetWindowColor() );
    Wallpaper aWall( aWhiteCol );
    SetBackground( aWall );
    Font aNewFont( aCopyrightText.GetFont() );
    aNewFont.SetTransparent( TRUE );

    aVersionText.SetFont( aNewFont );
    aCopyrightText.SetFont( aNewFont );

    aVersionText.SetBackground();
    aCopyrightText.SetBackground();

    Color aTextColor( rSettings.GetWindowTextColor() );
    aVersionText.SetControlForeground( aTextColor );
    aCopyrightText.SetControlForeground( aTextColor );
    aBuildData.SetBackground( aWall );

    Font aSmallFont = rSettings.GetInfoFont();
    Size aSmaller = aNewFont.GetSize();
    aSmaller.Width() = (long) (aSmaller.Width() * 0.75);
    aSmaller.Height() = (long) (aSmaller.Height() * 0.75);
    aNewFont.SetSize( aSmaller );
    aBuildData.SetFont( aNewFont );
    aBuildData.SetBackground( aWall );
    // FIXME: What is the purpose of the build data?
    // they are not showed even when set, so???
    String aBuildDataString;
    aBuildData.SetText( aBuildDataString );
    aBuildData.Show();

    aCopyrightText.SetText( aCopyrightTextStr );

    // determine size and position of the dialog & elements
    Size aAppLogoSiz = aAppLogo.GetSizePixel();
    Size aOutSiz     = GetOutputSizePixel();
    aOutSiz.Width()  = aAppLogoSiz.Width();

    // analyze size of the aVersionText widget
    // character size
    Size a6Size      = aVersionText.LogicToPixel( Size( 6, 6 ), MAP_APPFONT );
    // preferred Version widget size
    Size aVTSize = aVersionText.CalcMinimumSize();
    long nY          = aAppLogoSiz.Height() + ( a6Size.Height() * 2 );
    long nDlgMargin  = a6Size.Width() * 3 ;
    long nCtrlMargin = aVTSize.Height() + ( a6Size.Height() * 2 );
    long nTextWidth  = aOutSiz.Width() - nDlgMargin;

    // finally set the aVersionText widget position and size
    Size aVTCopySize = aVTSize;
    Point aVTCopyPnt;
    aVTCopySize.Width()  = nTextWidth;
    aVTCopyPnt.X() = ( aOutSiz.Width() - aVTCopySize.Width() ) / 2;
    aVTCopyPnt.Y() = nY;
    aVersionText.SetPosSizePixel( aVTCopyPnt, aVTCopySize );

    nY += nCtrlMargin;

    // OK-Button-Position (at the bottom and centered)
    Size aOKSiz = aOKButton.GetSizePixel();
    Point aOKPnt = aOKButton.GetPosPixel();

    // Multiline edit with Copyright-Text
    Point aCopyPnt = aCopyrightText.GetPosPixel();
    Size aCopySize = aCopyrightText.GetSizePixel();
    aCopySize.Width()  = nTextWidth;
    aCopySize.Height() = aOutSiz.Height() - nY - ( aOKSiz.Height() * 2 ) - nCtrlMargin;

    aCopyPnt.X() = ( aOutSiz.Width() - aCopySize.Width() ) / 2;
    aCopyPnt.Y() = nY;
    aCopyrightText.SetPosSizePixel( aCopyPnt, aCopySize );

    nY += aCopySize.Height() + nCtrlMargin;
    aOKPnt.X() = ( aOutSiz.Width() - aOKSiz.Width() ) / 2;
    aOKPnt.Y() = nY;
    aOKButton.SetPosPixel( aOKPnt );

    // Change the width of the dialog
    SetOutputSizePixel( aOutSiz );

    FreeResource();

    // explizite Help-Id
    SetHelpId( SID_ABOUT );
}

// -----------------------------------------------------------------------

AboutDialog::~AboutDialog()
{
    // L"oschen des Entwickleraufrufs
    if ( aAccelList.Count() )
    {
        GetpApp()->RemoveAccel( aAccelList.First() );
        Accelerator* pAccel = aAccelList.Last();

        while ( pAccel )
        {
            delete pAccel;
            pAccel = aAccelList.Prev();
        }
    }
}

// -----------------------------------------------------------------------

IMPL_LINK( AboutDialog, TimerHdl, Timer *, pTimer )
{
    (void)pTimer; //unused
    ++m_nPendingScrolls;
    Invalidate( INVALIDATE_NOERASE | INVALIDATE_NOCHILDREN );
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( AboutDialog, AccelSelectHdl, Accelerator *, pAccelerator )
{
#ifdef YURI_DARIO
    aCopyrightText.SetHelpText( DEFINE_CONST_UNICODE("Conoscere qualcuno ovunque egli sia, con cui comprendersi nonostante le distanze\n"
                      "e le differenze, puo' trasformare la terra in un giardino. baci Valeria") );
#endif

    (void)pAccelerator; //unused
    // init Timer
    aTimer.SetTimeoutHdl( LINK( this, AboutDialog, TimerHdl ) );

    // init scroll mode
    nOff = GetOutputSizePixel().Height();
    MapMode aMapMode( MAP_PIXEL );
    SetMapMode( aMapMode );
    bNormal = FALSE;

    // start scroll Timer
    aTimer.SetTimeout( SCROLL_TIMER );
    aTimer.Start();
    return 0;
}

// -----------------------------------------------------------------------

BOOL AboutDialog::Close()
{
    // stop Timer and finish the dialog
    aTimer.Stop();
    EndDialog( RET_OK );
    return( FALSE );
}

// -----------------------------------------------------------------------

void AboutDialog::Paint( const Rectangle& rRect )
{
    SetClipRegion( rRect );

    if ( bNormal ) // not in scroll mode
    {
        Point aPos( m_nDeltaWidth / 2, 0 );
        DrawImage( aPos, aAppLogo );
        return;
    }

    // scroll the content
    const int nDeltaY = -SCROLL_OFFSET * m_nPendingScrolls;
    if( !nDeltaY )
        return;
    nOff += nDeltaY;
    Scroll( 0, nDeltaY, SCROLL_NOERASE );
    m_nPendingScrolls = 0;

    // draw the credits text
    const Font aOrigFont = GetFont();
    const int nFullWidth = GetOutputSizePixel().Width();

    int nY = nOff;
    const int nDevCnt = static_cast<int>( aDeveloperAry.Count() );
    for( int i = 0; i < nDevCnt; ++i )
    {
        if( nY >= rRect.Bottom() )
            break;

        int nPos2 = nY + GetTextHeight() + 3;
        if( nPos2 >= rRect.Top() + nDeltaY )
        {
            const String aStr = aDeveloperAry.GetString(i);
            const long nVal = aDeveloperAry.GetValue(i);

            if ( nVal )
            {
                // emphasize the headers
                Font aFont = aOrigFont;
                aFont.SetWeight( (FontWeight)nVal );
                SetFont( aFont );
                nPos2 = nY + GetTextHeight() + 3;
            }

            // clear text background
            Rectangle aEraseRect( Point(0,nY), Size( nFullWidth, nPos2-nY));
            Erase( aEraseRect );

            // draw centered text
            const long nTextWidth = GetTextWidth( aStr );
            long nX = (nFullWidth - 5 - nTextWidth) / 2;
            if( nX < 0 )
                nX = SPACE_OFFSET;
            const Point aPnt( nX, nY );
            DrawText( aPnt, aStr );

            // restore the font if needed
            if( nVal )
                SetFont( aOrigFont );
        }
        nY = nPos2;
    }

    // close dialog if the whole text has been scrolled
    if ( nY <= 0 )
    {
        bNormal = TRUE;
        Close();
    }
}
