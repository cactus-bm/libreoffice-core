/*************************************************************************
 *
 *  $RCSfile: ivctrl.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: gt $ $Date: 2002-05-29 11:52:59 $
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

#pragma hdrstop

#include "ivctrl.hxx"
#include "imivctl.hxx"

#ifndef _SV_BITMAPEX_HXX
#include <vcl/bitmapex.hxx>
#endif

/*****************************************************************************
|
| class : SvxIconChoiceCtrlEntry
|
\*****************************************************************************/

SvxIconChoiceCtrlEntry::SvxIconChoiceCtrlEntry( const String& rText, const Image& rImage, USHORT _nFlags )
{
    aText = rText;
    aImage = rImage;
    pUserData = NULL;

    nFlags = _nFlags;
    eTextMode = IcnShowTextShort;
    pblink = 0;
    pflink = 0;
}

SvxIconChoiceCtrlEntry::SvxIconChoiceCtrlEntry( USHORT _nFlags )
{
    pUserData = NULL;

    nFlags = _nFlags;
    eTextMode = IcnShowTextShort;
    pblink = 0;
    pflink = 0;
}

void SvxIconChoiceCtrlEntry::SetMoved( BOOL bMoved )
{
    if( bMoved )
        nFlags |= ICNVIEW_FLAG_POS_MOVED;
    else
        nFlags &= ~ICNVIEW_FLAG_POS_MOVED;
}

void SvxIconChoiceCtrlEntry::LockPos( BOOL bLock )
{
    if( bLock )
        nFlags |= ICNVIEW_FLAG_POS_LOCKED;
    else
        nFlags &= ~ICNVIEW_FLAG_POS_LOCKED;
}

/*sal_Unicode SvxIconChoiceCtrlEntry::GetMnemonicChar() const
{
    sal_Unicode cChar = 0;
    xub_StrLen nPos = aText.Search( '~' );
    if ( nPos != STRING_NOTFOUND && nPos < ( aText.Len() ) - 1 )
        cChar = aText.GetChar( nPos + 1 );
    return cChar;
}*/

// ----------------------------------------------------------------------------

SvxIconChoiceCtrlColumnInfo::SvxIconChoiceCtrlColumnInfo( const SvxIconChoiceCtrlColumnInfo& rInfo )
    : aColText( rInfo.aColText ), aColImage( rInfo.aColImage )
{
    nWidth = rInfo.nWidth;
    eAlignment = rInfo.eAlignment;
    nSubItem = rInfo.nSubItem;
}

/*****************************************************************************
|
| class : SvtIconChoiceCtrl
|
\*****************************************************************************/

SvtIconChoiceCtrl::SvtIconChoiceCtrl( Window* pParent, WinBits nWinStyle ) :

     // WB_CLIPCHILDREN an, da ScrollBars auf dem Fenster liegen!
    Control( pParent, nWinStyle | WB_CLIPCHILDREN ),

    _pCurKeyEvent   ( NULL ),
    _pImp           ( new SvxIconChoiceCtrl_Impl( this, nWinStyle ) ),
    _bAutoFontColor ( FALSE )

{
    SetLineColor();
    _pImp->SetGrid( Size( 100, 70 ) );
    _pImp->InitSettings();
    _pImp->SetPositionMode( IcnViewPositionModeAutoArrange );
}

SvtIconChoiceCtrl::SvtIconChoiceCtrl( Window* pParent, const ResId& rResId ) :

    Control( pParent, rResId ),

    _pCurKeyEvent   ( NULL ),
    _pImp           ( new SvxIconChoiceCtrl_Impl( this, WB_BORDER ) ),
    _bAutoFontColor ( FALSE )

{
    SetLineColor();
    _pImp->SetGrid( Size( 100, 70 ) );
    _pImp->InitSettings();
    _pImp->SetPositionMode( IcnViewPositionModeAutoArrange );
}

SvtIconChoiceCtrl::~SvtIconChoiceCtrl()
{
    delete _pImp;
}

SvxIconChoiceCtrlEntry* SvtIconChoiceCtrl::InsertEntry( ULONG nPos, const Point* pPos, USHORT nFlags  )
{
    SvxIconChoiceCtrlEntry* pEntry = new SvxIconChoiceCtrlEntry( nFlags );
    _pImp->InsertEntry( pEntry, nPos, pPos );
    return pEntry;
}

SvxIconChoiceCtrlEntry* SvtIconChoiceCtrl::InsertEntry( const String& rText, const Image& rImage, ULONG nPos, const Point* pPos, USHORT nFlags  )
{
    SvxIconChoiceCtrlEntry* pEntry = new SvxIconChoiceCtrlEntry( rText, rImage, nFlags);

    _pImp->InsertEntry( pEntry, nPos, pPos );

    return pEntry;
}

BOOL SvtIconChoiceCtrl::EditedEntry( SvxIconChoiceCtrlEntry*, const XubString& rNewText, BOOL bCancelled )
{
    return TRUE;
}
BOOL SvtIconChoiceCtrl::EditingEntry( SvxIconChoiceCtrlEntry* pEntry )
{
    return TRUE;
}
void SvtIconChoiceCtrl::DrawEntryImage( SvxIconChoiceCtrlEntry* pEntry, const Point& rPos, OutputDevice& rDev )
{
    rDev.DrawImage ( rPos, pEntry->GetImage() );
}
String SvtIconChoiceCtrl::GetEntryText( SvxIconChoiceCtrlEntry* pEntry, BOOL bInplaceEdit )
{
    return pEntry->GetText();
}
BOOL SvtIconChoiceCtrl::HasBackground() const
{
    return FALSE;
}
BOOL SvtIconChoiceCtrl::HasFont() const
{
    return FALSE;
}
BOOL SvtIconChoiceCtrl::HasFontTextColor() const
{
    return FALSE;
}
BOOL SvtIconChoiceCtrl::HasFontFillColor() const
{
    return FALSE;
}

void SvtIconChoiceCtrl::Paint( const Rectangle& rRect )
{
    _pImp->Paint( rRect );
}

void SvtIconChoiceCtrl::MouseButtonDown( const MouseEvent& rMEvt )
{
    if( !_pImp->MouseButtonDown( rMEvt ) )
        Control::MouseButtonDown( rMEvt );
}

void SvtIconChoiceCtrl::MouseButtonUp( const MouseEvent& rMEvt )
{
    if( !_pImp->MouseButtonUp( rMEvt ) )
        Control::MouseButtonUp( rMEvt );
}

void SvtIconChoiceCtrl::MouseMove( const MouseEvent& rMEvt )
{
    if( !_pImp->MouseMove( rMEvt ) )
        Control::MouseMove( rMEvt );
}
void SvtIconChoiceCtrl::ArrangeIcons()
{
    if ( GetStyle() & WB_ALIGN_TOP )
    {
        Size aFullSize;
        Rectangle aEntryRect;

        for ( ULONG i = 0; i < GetEntryCount(); i++ )
        {
            SvxIconChoiceCtrlEntry* pEntry = GetEntry ( i );
            aEntryRect = _pImp->GetEntryBoundRect ( pEntry );

            aFullSize.setWidth ( aFullSize.getWidth()+aEntryRect.GetWidth() );
        }

        _pImp->Arrange ( FALSE, aFullSize.getWidth() );
    }
    else if ( GetStyle() & WB_ALIGN_LEFT )
    {
        Size aFullSize;
        Rectangle aEntryRect;

        for ( ULONG i = 0; i < GetEntryCount(); i++ )
        {
            SvxIconChoiceCtrlEntry* pEntry = GetEntry ( i );
            aEntryRect = _pImp->GetEntryBoundRect ( pEntry );

            aFullSize.setHeight ( aFullSize.getHeight()+aEntryRect.GetHeight() );
        }

        _pImp->Arrange ( FALSE, 0, aFullSize.getHeight() );
    }
    else
    {
        _pImp->Arrange();
    }
    _pImp->Arrange( FALSE, 0, 1000 );
}
void SvtIconChoiceCtrl::Resize()
{
    _pImp->Resize();
    Control::Resize();
}

Point SvtIconChoiceCtrl::GetLogicPos( const Point& rPosPixel ) const
{
    Point aPos( rPosPixel );
    aPos -= GetMapMode().GetOrigin();
    return aPos;
}

Point SvtIconChoiceCtrl::GetPixelPos( const Point& rPosLogic ) const
{
    Point aPos( rPosLogic );
    aPos += GetMapMode().GetOrigin();
    return aPos;
}

void SvtIconChoiceCtrl::DocumentRectChanged()
{
    _aDocRectChangedHdl.Call( this );
}

void SvtIconChoiceCtrl::VisibleRectChanged()
{
    _aVisRectChangedHdl.Call( this );
}

void SvtIconChoiceCtrl::GetFocus()
{
    _pImp->GetFocus();
    Control::GetFocus();
}

void SvtIconChoiceCtrl::LoseFocus()
{
    _pImp->LoseFocus();
    Control::LoseFocus();
}

void SvtIconChoiceCtrl::SetUpdateMode( BOOL bUpdate )
{
    Control::SetUpdateMode( bUpdate );
    _pImp->SetUpdateMode( bUpdate );
}
void SvtIconChoiceCtrl::SetFont( const Font& rFont )
{
    if( rFont != GetFont() )
    {
        Control::SetFont( rFont );
        _pImp->FontModified();
    }
}

void SvtIconChoiceCtrl::SetPointFont( const Font& rFont )
{
    if( rFont != GetPointFont() )
    {
        Control::SetPointFont( rFont );
        _pImp->FontModified();
    }
}
SvxIconChoiceCtrlEntry* SvtIconChoiceCtrl::GetEntry( const Point& rPixPos, BOOL bHit ) const
{
    Point aPos( rPixPos );
    aPos -= GetMapMode().GetOrigin();
    return ((SvtIconChoiceCtrl*)this)->_pImp->GetEntry( aPos, bHit );
}

void SvtIconChoiceCtrl::SetStyle( WinBits nWinStyle )
{
    _pImp->SetStyle( nWinStyle );
}

WinBits SvtIconChoiceCtrl::GetStyle() const
{
    return _pImp->GetStyle();
}
void SvtIconChoiceCtrl::Command( const CommandEvent& rCEvt )
{
    _pImp->Command( rCEvt );
}

void SvtIconChoiceCtrl::SetEntryTextMode( SvxIconChoiceCtrlTextMode eMode, SvxIconChoiceCtrlEntry* pEntry )
{
    _pImp->SetEntryTextMode( eMode, pEntry );
}

SvxIconChoiceCtrlTextMode SvtIconChoiceCtrl::GetEntryTextMode( const SvxIconChoiceCtrlEntry* pEntry ) const
{
    return _pImp->GetEntryTextModeSmart( pEntry );
}

SvxIconChoiceCtrlEntry* SvtIconChoiceCtrl::GetNextEntry( const Point& rPixPos, SvxIconChoiceCtrlEntry* pCurEntry, BOOL  ) const
{
    Point aPos( rPixPos );
    aPos -= GetMapMode().GetOrigin();
    return ((SvtIconChoiceCtrl*)this)->_pImp->GetNextEntry( aPos, pCurEntry );
}

SvxIconChoiceCtrlEntry* SvtIconChoiceCtrl::GetPrevEntry( const Point& rPixPos, SvxIconChoiceCtrlEntry* pCurEntry, BOOL  ) const
{
    Point aPos( rPixPos );
    aPos -= GetMapMode().GetOrigin();
    return ((SvtIconChoiceCtrl*)this)->_pImp->GetPrevEntry( aPos, pCurEntry );
}
ULONG SvtIconChoiceCtrl::GetEntryCount() const
{
    return _pImp->GetEntryCount();
}

SvxIconChoiceCtrlEntry* SvtIconChoiceCtrl::GetEntry( ULONG nPos ) const
{
    return _pImp->GetEntry( nPos );
}

void SvtIconChoiceCtrl::CreateAutoMnemonics( void )
{
    _pImp->CreateAutoMnemonics();
}

void SvtIconChoiceCtrl::RemoveEntry( SvxIconChoiceCtrlEntry* pEntry )
{
    _pImp->RemoveEntry( pEntry );
}

SvxIconChoiceCtrlEntry* SvtIconChoiceCtrl::GetSelectedEntry( ULONG& rPos ) const
{
    return _pImp->GetFirstSelectedEntry( rPos );
}

void SvtIconChoiceCtrl::ClickIcon()
{
    /*
    ULONG nPos;
    SvxIconChoiceCtrlEntry* pSelectedEntry = GetSelectedEntry ( nPos );
    */
    _aClickIconHdl.Call( this );
}
BOOL SvtIconChoiceCtrl::IsEntryEditing() const
{
    return _pImp->IsEntryEditing();
}

BOOL SvtIconChoiceCtrl::SetChoiceWithCursor ( BOOL bDo )
{
    return _pImp->SetChoiceWithCursor (bDo);
}

void SvtIconChoiceCtrl::KeyInput( const KeyEvent& rKEvt )
{
    BOOL bKeyUsed = DoKeyInput( rKEvt );
    if ( !bKeyUsed )
    {
        _pCurKeyEvent = (KeyEvent*)&rKEvt;
        Control::KeyInput( rKEvt );
        _pCurKeyEvent = NULL;
    }
}
BOOL SvtIconChoiceCtrl::DoKeyInput( const KeyEvent& rKEvt )
{
    // unter OS/2 bekommen wir auch beim Editieren Key-Up/Down
    if( IsEntryEditing() )
        return TRUE;
    _pCurKeyEvent = (KeyEvent*)&rKEvt;
    BOOL bHandled = _pImp->KeyInput( rKEvt );
    _pCurKeyEvent = NULL;
    return bHandled;
}
ULONG SvtIconChoiceCtrl::GetEntryListPos( SvxIconChoiceCtrlEntry* pEntry ) const
{
    return _pImp->GetEntryListPos( pEntry );
}
SvxIconChoiceCtrlEntry* SvtIconChoiceCtrl::GetCursor( ) const
{
    return _pImp->GetCurEntry( );
}
void SvtIconChoiceCtrl::SetCursor( SvxIconChoiceCtrlEntry* pEntry )
{
    _pImp->SetCursor( pEntry );
}
void SvtIconChoiceCtrl::InvalidateEntry( SvxIconChoiceCtrlEntry* pEntry )
{
    _pImp->InvalidateEntry( pEntry );
}
void SvtIconChoiceCtrl::Clear()
{
    _pImp->Clear();
}
void SvtIconChoiceCtrl::StateChanged( StateChangedType nType )
{
    Control::StateChanged( nType );
}


void SvtIconChoiceCtrl::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( ((rDCEvt.GetType() == DATACHANGED_SETTINGS) ||
         (rDCEvt.GetType() == DATACHANGED_FONTSUBSTITUTION) ||
         (rDCEvt.GetType() == DATACHANGED_FONTS) ) &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        _pImp->InitSettings();
        Invalidate(INVALIDATE_NOCHILDREN);
    }
    else
        Control::DataChanged( rDCEvt );
}

void SvtIconChoiceCtrl::SetBackground( const Wallpaper& rPaper )
{
    if( rPaper != GetBackground() )
    {
        const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();
        Wallpaper aEmpty;
        if( rPaper == aEmpty )
            Control::SetBackground( rStyleSettings.GetWindowColor() );
        else
        {
            Wallpaper aBackground( rPaper );
            // HACK, da Hintergrund sonst transparent sein koennte
            if( !aBackground.IsBitmap() )
                aBackground.SetStyle( WALLPAPER_TILE );

            WallpaperStyle eStyle = aBackground.GetStyle();
            Color aBack( aBackground.GetColor());
            Color aTrans( COL_TRANSPARENT );
            if( aBack == aTrans  && (
                (!aBackground.IsBitmap() ||
                    aBackground.GetBitmap().IsTransparent() ||
                    (eStyle != WALLPAPER_TILE && eStyle != WALLPAPER_SCALE))))
            {
                aBackground.SetColor( rStyleSettings.GetWindowColor() );
            }
            if( aBackground.IsScrollable() )
            {
                Rectangle aRect;
                aRect.SetSize( Size(32765, 32765) );
                aBackground.SetRect( aRect );
            }
            else
            {
                Rectangle aRect( _pImp->GetOutputRect() );
                aBackground.SetRect( aRect );
            }
            Control::SetBackground( aBackground );
        }

        // bei hart attributierter Textfarbe keine 'Automatik', die eine
        // lesbare Textfarbe einstellt.
        Font aFont( GetFont() );
        aFont.SetColor( rStyleSettings.GetWindowTextColor() );
        SetFont( aFont );

        Invalidate(INVALIDATE_NOCHILDREN);
    }
}

void SvtIconChoiceCtrl::Flush()
{
    _pImp->Flush();
}

void SvtIconChoiceCtrl::RequestHelp( const HelpEvent& rHEvt )
{
    if ( !_pImp->RequestHelp( rHEvt ) )
        Control::RequestHelp( rHEvt );
}

void SvtIconChoiceCtrl::SetSelectionMode( SelectionMode eMode )
{
    _pImp->SetSelectionMode( eMode );
}

BOOL SvtIconChoiceCtrl::HandleShortCutKey( const KeyEvent& r )
{
    return _pImp->HandleShortCutKey( r );
}
