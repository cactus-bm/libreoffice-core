/*************************************************************************
 *
 *  $RCSfile: mediacontrol.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: ka $ $Date: 2004-08-23 09:04:42 $
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

#include "mediacontrol.hxx"
#include "mediacontrol.hrc"
#include "mediamisc.hxx"
#include "mediawindow.hxx"
#include "mediaplayer.hxx"
#include <vcl/svapp.hxx>
#include <vcl/lstbox.hxx>
#include <sfx2/viewfrm.hxx>
#include <cmath>
#include <algorithm>

#define AVMEDIA_TIMEOUT             100
#define AVMEDIA_TIME_RANGE          2048
#define AVMEDIA_DB_RANGE            -40
#define AVMEDIA_LINEINCREMENT       1.0
#define AVMEDIA_PAGEINCREMENT       10.0

#define AVMEDIA_TOOLBOXITEM_PLAY    0x0001
#define AVMEDIA_TOOLBOXITEM_PLAYFFW 0x0002
#define AVMEDIA_TOOLBOXITEM_PAUSE   0x0004
#define AVMEDIA_TOOLBOXITEM_STOP    0x0008
#define AVMEDIA_TOOLBOXITEM_MUTE    0x0010
#define AVMEDIA_TOOLBOXITEM_LOOP    0x0011
#define AVMEDIA_TOOLBOXITEM_ZOOM    0x0012
#define AVMEDIA_TOOLBOXITEM_OPEN    0x0014
#define AVMEDIA_TOOLBOXITEM_INSERT  0x0018

#define AVMEDIA_ZOOMLEVEL_50        0
#define AVMEDIA_ZOOMLEVEL_100       1
#define AVMEDIA_ZOOMLEVEL_200       2
#define AVMEDIA_ZOOMLEVEL_FIT       3
#define AVMEDIA_ZOOMLEVEL_SCALED    4
#define AVMEDIA_ZOOMLEVEL_INVALID   65535

namespace avmedia
{

// ----------------
// - MediaControl -
// ---------------

MediaControl::MediaControl( Window* pParent, MediaControlStyle eControlStyle ) :
    Control( pParent ),
    maImageList( AVMEDIA_RESID( AVMEDIA_IMGLST ) ),
    maItem( 0, AVMEDIA_SETMASK_ALL ),
    maTimeSlider( this, WB_HORZ | WB_DRAG | WB_3DLOOK | WB_SLIDERSET ),
    maPlayToolBox( this, WB_3DLOOK ),
    maMuteToolBox( this, WB_3DLOOK ),
    maVolumeSlider( this, WB_HORZ | WB_DRAG | WB_SLIDERSET ),
    maZoomToolBox( this, WB_3DLOOK ),
    mpZoomListBox( new ListBox( &maZoomToolBox, WB_BORDER | WB_DROPDOWN | WB_AUTOHSCROLL | WB_3DLOOK ) ),
    maTimeEdit( this, WB_CENTER | WB_READONLY | WB_BORDER | WB_3DLOOK | WB_READONLY ),
    meControlStyle( eControlStyle )
{
    const String aTimeText( RTL_CONSTASCII_USTRINGPARAM( " 00:00:00/00:00:00 " ) );

    SetBackground();

    if( MEDIACONTROLSTYLE_SINGLELINE != meControlStyle )
    {
        maPlayToolBox.InsertItem( AVMEDIA_TOOLBOXITEM_OPEN, implGetImage( AVMEDIA_IMG_OPEN ), String( AVMEDIA_RESID( AVMEDIA_STR_OPEN ) ) );
        maPlayToolBox.InsertItem( AVMEDIA_TOOLBOXITEM_INSERT, implGetImage( AVMEDIA_IMG_INSERT ), String( AVMEDIA_RESID( AVMEDIA_STR_INSERT ) ) );
        maPlayToolBox.InsertSeparator();
    }
    else
    {
        maTimeSlider.SetBackground();
        maPlayToolBox.SetBackground();
        maMuteToolBox.SetBackground();
        maVolumeSlider.SetBackground();
        maZoomToolBox.SetBackground();
        mpZoomListBox->SetBackground();
        //maTimeEdit.SetBackground();
    }

    maPlayToolBox.InsertItem( AVMEDIA_TOOLBOXITEM_PLAY, implGetImage( AVMEDIA_IMG_PLAY ), String( AVMEDIA_RESID( AVMEDIA_STR_PLAY ) ), TIB_CHECKABLE );
    maPlayToolBox.InsertItem( AVMEDIA_TOOLBOXITEM_PAUSE, implGetImage( AVMEDIA_IMG_PAUSE ), String( AVMEDIA_RESID( AVMEDIA_STR_PAUSE ) ), TIB_CHECKABLE );
    maPlayToolBox.InsertItem( AVMEDIA_TOOLBOXITEM_STOP, implGetImage( AVMEDIA_IMG_STOP ), String( AVMEDIA_RESID( AVMEDIA_STR_STOP ) ), TIB_CHECKABLE );
    maPlayToolBox.InsertSeparator();
    maPlayToolBox.InsertItem( AVMEDIA_TOOLBOXITEM_LOOP, implGetImage( AVMEDIA_IMG_ENDLESS ), String( AVMEDIA_RESID( AVMEDIA_STR_ENDLESS ) ) );

    if( MEDIACONTROLSTYLE_SINGLELINE == meControlStyle )
        maPlayToolBox.InsertSeparator();

    maPlayToolBox.SetSelectHdl( LINK( this, MediaControl, implSelectHdl ) );
    maPlayToolBox.SetSizePixel( maPlayToolBox.CalcWindowSizePixel() );
    maPlayToolBox.Show();
    maMinSize = maPlayToolBox.GetSizePixel();

    maTimeSlider.SetSlideHdl( LINK( this, MediaControl, implTimeHdl ) );
    maTimeSlider.SetEndSlideHdl( LINK( this, MediaControl, implTimeEndHdl ) );
    maTimeSlider.SetRange( Range( 0, AVMEDIA_TIME_RANGE ) );
    maTimeSlider.SetUpdateMode( true );
    maTimeSlider.SetSizePixel( Size( 128, maPlayToolBox.GetSizePixel().Height() ) );
    maTimeSlider.Show();
    maMinSize.Width() += maTimeSlider.GetSizePixel().Width();

    maTimeEdit.SetText( aTimeText );
    maTimeEdit.SetUpdateMode( true );
    maTimeEdit.SetSizePixel( Size( maTimeEdit.GetTextWidth( aTimeText ) + 8, maPlayToolBox.GetSizePixel().Height() ) );
    maTimeEdit.SetControlBackground( Application::GetSettings().GetStyleSettings().GetWindowColor() );
    maTimeEdit.Disable();
    maTimeEdit.Show();
    maMinSize.Width() += maTimeEdit.GetSizePixel().Width();

    if( MEDIACONTROLSTYLE_SINGLELINE == meControlStyle )
        maMuteToolBox.InsertSeparator();

    maMuteToolBox.InsertItem( AVMEDIA_TOOLBOXITEM_MUTE, implGetImage( AVMEDIA_IMG_MUTE ), String( AVMEDIA_RESID( AVMEDIA_STR_MUTE ) ) );
    maMuteToolBox.SetSelectHdl( LINK( this, MediaControl, implSelectHdl ) );
    maMuteToolBox.SetSizePixel( maMuteToolBox.CalcWindowSizePixel() );
    maMuteToolBox.Show();
    maMinSize.Width() += maMuteToolBox.GetSizePixel().Width();

    maVolumeSlider.SetSlideHdl( LINK( this, MediaControl, implVolumeHdl ) );
    maVolumeSlider.SetEndSlideHdl( LINK( this, MediaControl, implVolumeEndHdl ) );
    maVolumeSlider.SetRange( Range( AVMEDIA_DB_RANGE, 0 ) );
    maTimeSlider.SetLineSize( labs( AVMEDIA_DB_RANGE / 33 ) );
    maTimeSlider.SetPageSize( labs( AVMEDIA_DB_RANGE / 10 ) );
    maVolumeSlider.SetUpdateMode( true );
    maVolumeSlider.SetSizePixel( Size( 48, maPlayToolBox.GetSizePixel().Height() ) );
    maVolumeSlider.Show();
    maMinSize.Width() += maVolumeSlider.GetSizePixel().Width();

    mpZoomListBox->SetSizePixel( Size( maTimeEdit.GetSizePixel().Width(), 260 ) );
    mpZoomListBox->InsertEntry( String( AVMEDIA_RESID( AVMEDIA_STR_ZOOM_50 ) ), AVMEDIA_ZOOMLEVEL_50 );
    mpZoomListBox->InsertEntry( String( AVMEDIA_RESID( AVMEDIA_STR_ZOOM_100 ) ), AVMEDIA_ZOOMLEVEL_100 );
    mpZoomListBox->InsertEntry( String( AVMEDIA_RESID( AVMEDIA_STR_ZOOM_200 ) ), AVMEDIA_ZOOMLEVEL_200 );
    mpZoomListBox->InsertEntry( String( AVMEDIA_RESID( AVMEDIA_STR_ZOOM_FIT ) ), AVMEDIA_ZOOMLEVEL_FIT );
    // mpZoomListBox->InsertEntry( String( AVMEDIA_RESID( AVMEDIA_STR_ZOOM_SCALED ) ), AVMEDIA_ZOOMLEVEL_SCALED );
    mpZoomListBox->SetSelectHdl( LINK( this, MediaControl, implZoomSelectHdl ) );

    maZoomToolBox.InsertItem( AVMEDIA_TOOLBOXITEM_ZOOM, String( AVMEDIA_RESID( AVMEDIA_STR_ZOOM ) ) );
    maZoomToolBox.SetItemWindow( AVMEDIA_TOOLBOXITEM_ZOOM, mpZoomListBox );
    maZoomToolBox.SetSelectHdl( LINK( this, MediaControl, implSelectHdl ) );
    maZoomToolBox.SetSizePixel( maZoomToolBox.CalcWindowSizePixel() );
    maZoomToolBox.Show();
    maMinSize.Width() += maZoomToolBox.GetSizePixel().Width();

    if( MEDIACONTROLSTYLE_MULTILINE == meControlStyle )
    {
        maMinSize.Width() = 256;
        maMinSize.Height() = ( maMinSize.Height() << 1 ) + AVMEDIA_CONTROLOFFSET;
    }

    maTimer.SetTimeout( AVMEDIA_TIMEOUT );
    maTimer.SetTimeoutHdl( LINK( this, MediaControl, implTimeoutHdl ) );
    maTimer.Start();
}

// ------------------------------------------------------------------------------

MediaControl::~MediaControl()
{
    maZoomToolBox.SetItemWindow( AVMEDIA_TOOLBOXITEM_ZOOM, NULL );
    delete mpZoomListBox;
}

// ------------------------------------------------------------------------------

const Size& MediaControl::getMinSizePixel() const
{
    return maMinSize;
}

// ------------------------------------------------------------------------------

void MediaControl::Resize()
{
    Point           aPos( 0, 0 );
    const sal_Int32 nPlayToolBoxWidth = maPlayToolBox.GetSizePixel().Width();
    const sal_Int32 nMuteToolBoxWidth = maMuteToolBox.GetSizePixel().Width();
    const sal_Int32 nVolumeSliderWidth = maVolumeSlider.GetSizePixel().Width();
    const sal_Int32 nZoomToolBoxWidth = maZoomToolBox.GetSizePixel().Width();
    const sal_Int32 nTimeEditWidth = maTimeEdit.GetSizePixel().Width();
    const sal_Int32 nTimeSliderHeight = maTimeSlider.GetSizePixel().Height();

    if( MEDIACONTROLSTYLE_SINGLELINE == meControlStyle )
    {
        const sal_Int32 nTimeSliderWidth = GetSizePixel().Width() - ( AVMEDIA_CONTROLOFFSET * 3 ) -
                                           nPlayToolBoxWidth - nMuteToolBoxWidth - nVolumeSliderWidth - nTimeEditWidth - nZoomToolBoxWidth;

        maPlayToolBox.SetPosSizePixel( aPos, maPlayToolBox.GetSizePixel() );

        aPos.X() += nPlayToolBoxWidth;
        maTimeSlider.SetPosSizePixel( aPos, Size( nTimeSliderWidth, nTimeSliderHeight ) );

        aPos.X() += nTimeSliderWidth + AVMEDIA_CONTROLOFFSET;
        maTimeEdit.SetPosSizePixel( aPos, maTimeEdit.GetSizePixel() );

        aPos.X() += nTimeEditWidth + AVMEDIA_CONTROLOFFSET;
        maMuteToolBox.SetPosSizePixel( aPos, maMuteToolBox.GetSizePixel() );

        aPos.X() += nMuteToolBoxWidth;
        maVolumeSlider.SetPosSizePixel( aPos, maVolumeSlider.GetSizePixel() );

        aPos.X() += nVolumeSliderWidth + AVMEDIA_CONTROLOFFSET;
        maZoomToolBox.SetPosSizePixel( aPos, maZoomToolBox.GetSizePixel() );
    }
    else
    {
        const sal_Int32 nTimeSliderWidth = GetSizePixel().Width() - AVMEDIA_CONTROLOFFSET - nTimeEditWidth;

        maTimeSlider.SetPosSizePixel( aPos, Size( nTimeSliderWidth, nTimeSliderHeight ) );

        aPos.X() += nTimeSliderWidth + AVMEDIA_CONTROLOFFSET;
        maTimeEdit.SetPosSizePixel( aPos, maTimeEdit.GetSizePixel() );

        aPos.X() = 0;
        aPos.Y() += nTimeSliderHeight + AVMEDIA_CONTROLOFFSET;
        maPlayToolBox.SetPosSizePixel( aPos, maPlayToolBox.GetSizePixel() );

        aPos.X() = GetSizePixel().Width() - nVolumeSliderWidth - nMuteToolBoxWidth - nZoomToolBoxWidth - AVMEDIA_CONTROLOFFSET;
        maMuteToolBox.SetPosSizePixel( aPos, maMuteToolBox.GetSizePixel() );

        aPos.X() += nMuteToolBoxWidth;
        maVolumeSlider.SetPosSizePixel( aPos, maVolumeSlider.GetSizePixel() );

        aPos.X() = GetSizePixel().Width() - nZoomToolBoxWidth;
        maZoomToolBox.SetPosSizePixel( aPos, maZoomToolBox.GetSizePixel() );
    }
}

// ------------------------------------------------------------------------------

void MediaControl::setState( const MediaItem& rItem )
{
    maItem.merge( rItem );

    implUpdateToolboxes();
    implUpdateTimeSlider();
    implUpdateVolumeSlider();
    implUpdateTimeField( maItem.getTime() );
}

// ------------------------------------------------------------------------------

void MediaControl::getState( MediaItem& rItem ) const
{
    rItem.merge( maItem );
}

// ------------------------------------------------------------------------------

void MediaControl::update()
{
}

// ------------------------------------------------------------------------------

void MediaControl::execute( const MediaItem& rItem )
{
}

// ------------------------------------------------------------------------------

void MediaControl::implUpdateToolboxes()
{
    const bool bValidURL = ( maItem.getURL().getLength() > 0 );

    maPlayToolBox.EnableItem( AVMEDIA_TOOLBOXITEM_INSERT, bValidURL );
    maPlayToolBox.EnableItem( AVMEDIA_TOOLBOXITEM_PLAY, bValidURL );
    maPlayToolBox.EnableItem( AVMEDIA_TOOLBOXITEM_PLAYFFW, bValidURL );
    maPlayToolBox.EnableItem( AVMEDIA_TOOLBOXITEM_PAUSE, bValidURL );
    maPlayToolBox.EnableItem( AVMEDIA_TOOLBOXITEM_STOP, bValidURL );
    maPlayToolBox.EnableItem( AVMEDIA_TOOLBOXITEM_LOOP, bValidURL );
    maMuteToolBox.EnableItem( AVMEDIA_TOOLBOXITEM_MUTE, bValidURL );

    if( !bValidURL )
        mpZoomListBox->Disable();
    else
    {
        if( MEDIASTATE_PLAY == maItem.getState() || MEDIASTATE_PLAYFFW == maItem.getState() )
        {
            maPlayToolBox.CheckItem( AVMEDIA_TOOLBOXITEM_PLAY, true );
            maPlayToolBox.CheckItem( AVMEDIA_TOOLBOXITEM_PLAYFFW, MEDIASTATE_PLAYFFW == maItem.getState() );
            maPlayToolBox.CheckItem( AVMEDIA_TOOLBOXITEM_PAUSE, false );
            maPlayToolBox.CheckItem( AVMEDIA_TOOLBOXITEM_STOP, false );
        }
        else if( maItem.getTime() > 0.0 && ( maItem.getTime() < maItem.getDuration() ) )
        {
            maPlayToolBox.CheckItem( AVMEDIA_TOOLBOXITEM_PLAY, false );
            maPlayToolBox.CheckItem( AVMEDIA_TOOLBOXITEM_PLAYFFW, false );
            maPlayToolBox.CheckItem( AVMEDIA_TOOLBOXITEM_PAUSE, true );
            maPlayToolBox.CheckItem( AVMEDIA_TOOLBOXITEM_STOP, false );
        }
        else
        {
            maPlayToolBox.CheckItem( AVMEDIA_TOOLBOXITEM_PLAY, false );
            maPlayToolBox.CheckItem( AVMEDIA_TOOLBOXITEM_PLAYFFW, false );
            maPlayToolBox.CheckItem( AVMEDIA_TOOLBOXITEM_PAUSE, false );
            maPlayToolBox.CheckItem( AVMEDIA_TOOLBOXITEM_STOP, true );
        }

        maPlayToolBox.CheckItem( AVMEDIA_TOOLBOXITEM_LOOP, maItem.isLoop() );
        maMuteToolBox.CheckItem( AVMEDIA_TOOLBOXITEM_MUTE, maItem.isMute() );

        if( !mpZoomListBox->IsTravelSelect() && !mpZoomListBox->IsInDropDown() )
        {
            USHORT nSelectEntryPos ;

            switch( maItem.getZoom() )
            {
                case( ::com::sun::star::media::ZoomLevel_ZOOM_1_TO_2 ): nSelectEntryPos = AVMEDIA_ZOOMLEVEL_50; break;
                case( ::com::sun::star::media::ZoomLevel_ORIGINAL ): nSelectEntryPos = AVMEDIA_ZOOMLEVEL_100; break;
                case( ::com::sun::star::media::ZoomLevel_ZOOM_2_TO_1 ): nSelectEntryPos = AVMEDIA_ZOOMLEVEL_200; break;
                case( ::com::sun::star::media::ZoomLevel_FIT_TO_WINDOW_FIXED_ASPECT ): nSelectEntryPos = AVMEDIA_ZOOMLEVEL_FIT; break;
                case( ::com::sun::star::media::ZoomLevel_FIT_TO_WINDOW ): nSelectEntryPos = AVMEDIA_ZOOMLEVEL_SCALED; break;

                default: nSelectEntryPos = AVMEDIA_ZOOMLEVEL_INVALID; break;
            }

            if( nSelectEntryPos != AVMEDIA_ZOOMLEVEL_INVALID )
            {
                mpZoomListBox->Enable();
                mpZoomListBox->SelectEntryPos( nSelectEntryPos );
            }
            else
                mpZoomListBox->Disable();
        }
    }
}

// ------------------------------------------------------------------------------

void MediaControl::implUpdateTimeSlider()
{
    if( maItem.getURL().getLength() == 0 )
        maTimeSlider.Disable();
    else
    {
        const double fDuration = maItem.getDuration();

        maTimeSlider.Enable();

        if( fDuration > 0.0 )
        {
            const double fTime = ::std::min( maItem.getTime(), fDuration );

            if( !maTimeSlider.GetLineSize() )
                maTimeSlider.SetLineSize( static_cast< sal_uInt32 >( AVMEDIA_TIME_RANGE * AVMEDIA_LINEINCREMENT / fDuration ) );

            if( !maTimeSlider.GetPageSize() )
                maTimeSlider.SetPageSize( static_cast< sal_uInt32 >( AVMEDIA_TIME_RANGE * AVMEDIA_PAGEINCREMENT / fDuration ) );

            maTimeSlider.SetThumbPos( static_cast< sal_Int32 >( fTime / fDuration * AVMEDIA_TIME_RANGE ) );
        }
    }
}

// ------------------------------------------------------------------------------

void MediaControl::implUpdateVolumeSlider()
{
    if( maItem.getURL().getLength() == 0 )
        maVolumeSlider.Disable();
    else
    {
        const sal_Int32 nVolumeDB = maItem.getVolumeDB();

        maVolumeSlider.Enable();
        maVolumeSlider.SetThumbPos( ::std::min( ::std::max( nVolumeDB, static_cast< sal_Int32 >( AVMEDIA_DB_RANGE ) ),
                                                static_cast< sal_Int32 >( 0 ) ) );
    }
}

// ------------------------------------------------------------------------------

void MediaControl::implUpdateTimeField( double fCurTime )
{
    maTimeEdit.Disable();

    if( maItem.getURL().getLength() > 0 )
    {
        const International aInternational;
        String              aTimeString;

        aTimeString += aInternational.GetDuration( Time( 0, 0, static_cast< sal_uInt32 >( floor( fCurTime ) ) ) );
        aTimeString.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " / " ));
        aTimeString += aInternational.GetDuration( Time( 0, 0, static_cast< sal_uInt32 >( floor( maItem.getDuration() ) ) ) );
        maTimeEdit.SetText( aTimeString );
    }
}

// ------------------------------------------------------------------------------

Image MediaControl::implGetImage( sal_Int32 nImageId ) const
{
    return maImageList.GetImage( static_cast< USHORT >( nImageId ) );
}

// ------------------------------------------------------------------------------

IMPL_LINK( MediaControl, implTimeHdl, Slider*, p )
{
    maTimer.Stop();
    implUpdateTimeField( p->GetThumbPos() * maItem.getDuration() / AVMEDIA_TIME_RANGE );

    return 0;
}

// ------------------------------------------------------------------------------

IMPL_LINK( MediaControl, implTimeEndHdl, Slider*, p )
{
    MediaItem aExecItem;

    aExecItem.setTime( p->GetThumbPos() * maItem.getDuration() / AVMEDIA_TIME_RANGE );
    execute( aExecItem );
    update();
    maTimer.Start();

    return 0;
}

// ------------------------------------------------------------------------------

IMPL_LINK( MediaControl, implVolumeHdl, Slider*, p )
{
    MediaItem aExecItem;

    aExecItem.setVolumeDB( p->GetThumbPos() );
    execute( aExecItem );
    update();

    return 0;
}

// ------------------------------------------------------------------------------

IMPL_LINK( MediaControl, implVolumeEndHdl, Slider*, p )
{
    return 0;
}

// ------------------------------------------------------------------------------

IMPL_LINK( MediaControl, implSelectHdl, ToolBox*, p )
{
    if( p )
    {
        MediaItem aExecItem;

        switch( p->GetCurItemId() )
        {
            case( AVMEDIA_TOOLBOXITEM_OPEN ):
            {
                ::rtl::OUString aURL;

                 if( ::avmedia::MediaWindow::executeMediaURLDialog( GetParent(), aURL, false ) )
                 {
                     if( !::avmedia::MediaWindow::isMediaURL( aURL, true ) )
                        ::avmedia::MediaWindow::executeFormatErrorBox( this );
                    else
                    {
                        aExecItem.setURL( aURL );
                        aExecItem.setState( MEDIASTATE_PLAY );
                    }
                }
            }
            break;

            case( AVMEDIA_TOOLBOXITEM_INSERT ):
            {
                MediaFloater* pFloater = AVMEDIA_MEDIAWINDOW();

                if( pFloater )
                    pFloater->dispatchCurrentURL();
            }
            break;

            case( AVMEDIA_TOOLBOXITEM_PLAY ):
            case( AVMEDIA_TOOLBOXITEM_PLAYFFW ):
            {
                aExecItem.setState( ( AVMEDIA_TOOLBOXITEM_PLAYFFW == p->GetCurItemId() ) ? MEDIASTATE_PLAYFFW : MEDIASTATE_PLAY );

                if( maItem.getTime() == maItem.getDuration() )
                    aExecItem.setTime( 0.0 );
                else
                    aExecItem.setTime( maItem.getTime() );
            }
            break;

            case( AVMEDIA_TOOLBOXITEM_PAUSE ):
            {
                aExecItem.setState( MEDIASTATE_PAUSE );
            }
            break;

            case( AVMEDIA_TOOLBOXITEM_STOP ):
            {
                aExecItem.setState( MEDIASTATE_STOP );
                aExecItem.setTime( 0.0 );
            }
            break;

            case( AVMEDIA_TOOLBOXITEM_MUTE ):
            {
                aExecItem.setMute( !maMuteToolBox.IsItemChecked( AVMEDIA_TOOLBOXITEM_MUTE ) );
            }
            break;

            case( AVMEDIA_TOOLBOXITEM_LOOP ):
            {
                aExecItem.setLoop( !maPlayToolBox.IsItemChecked( AVMEDIA_TOOLBOXITEM_LOOP ) );
            }
            break;

            default:
            break;
        }

        if( aExecItem.getMaskSet() != AVMEDIA_SETMASK_NONE )
            execute( aExecItem );
    }

    update();
    p->Invalidate();

    return 0;
}

// ------------------------------------------------------------------------------

IMPL_LINK( MediaControl, implZoomSelectHdl, ListBox*, p )
{
    if( p )
    {
        MediaItem aExecItem;
        ::com::sun::star::media::ZoomLevel eLevel;

        switch( p->GetSelectEntryPos() )
        {
            case( AVMEDIA_ZOOMLEVEL_50 ): eLevel = ::com::sun::star::media::ZoomLevel_ZOOM_1_TO_2; break;
            case( AVMEDIA_ZOOMLEVEL_100 ): eLevel = ::com::sun::star::media::ZoomLevel_ORIGINAL; break;
            case( AVMEDIA_ZOOMLEVEL_200 ): eLevel = ::com::sun::star::media::ZoomLevel_ZOOM_2_TO_1; break;
            case( AVMEDIA_ZOOMLEVEL_FIT ): eLevel = ::com::sun::star::media::ZoomLevel_FIT_TO_WINDOW_FIXED_ASPECT; break;
            case( AVMEDIA_ZOOMLEVEL_SCALED ): eLevel = ::com::sun::star::media::ZoomLevel_FIT_TO_WINDOW; break;

            default: eLevel = ::com::sun::star::media::ZoomLevel_NOT_AVAILABLE; break;
        }

        aExecItem.setZoom( eLevel );
        execute( aExecItem );
        update();
    }

    return 0;
}

// ------------------------------------------------------------------------------

IMPL_LINK( MediaControl, implTimeoutHdl, Timer*, p )
{
    update();
    maTimer.Start();

    return 0;
}

}
