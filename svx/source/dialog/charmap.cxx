/*************************************************************************
 *
 *  $RCSfile: charmap.cxx,v $
 *
 *  $Revision: 1.24 $
 *
 *  last change: $Author: hr $ $Date: 2002-02-25 14:49:33 $
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

// include ---------------------------------------------------------------

#include <stdio.h>

#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _SV_SOUND_HXX
#include <vcl/sound.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#ifndef _SV_FIXED_HXX
#include <vcl/fixed.hxx>
#endif
#ifndef _SV_LSTBOX_HXX
#include <vcl/lstbox.hxx>
#endif
#ifndef _SV_EDIT_HXX
#include <vcl/edit.hxx>
#endif
#pragma hdrstop

#include <rtl/textenc.h>
#include <ucsubset.hxx>

#include "dialogs.hrc"
#include "charmap.hrc"

#include "charmap.hxx"
#include "dialmgr.hxx"

// class SvxShowText =====================================================

class SvxShowText : public Control
{
public:
                    SvxShowText( Window* pParent,
                                 const ResId& rResId,
                                 BOOL bCenter = FALSE );
                    ~SvxShowText();

    void            SetFont( const Font& rFont );
    void            SetText( const String& rText );

protected:
    virtual void    Paint( const Rectangle& );

private:
    long            nY;
    BOOL            bCenter;

};

// class SvxCharMapData ==================================================

class SvxCharMapData
{
public:
                    SvxCharMapData( class SfxModalDialog* pDialog, BOOL bOne_ );

    void            SetCharFont( const Font& rFont );

private:
friend class SvxCharacterMap;
    SfxModalDialog* mpDialog;

    SvxShowCharSet  aShowSet;
//    Edit            aShowText;
    SvxShowText     aShowText;
    OKButton        aOKBtn;
    CancelButton    aCancelBtn;
    HelpButton      aHelpBtn;
    PushButton      aDeleteBtn;
    FixedText       aFontText;
    ListBox         aFontLB;
    FixedText       aSubsetText;
    ListBox         aSubsetLB;
    FixedText       aSymbolText;
    SvxShowText     aShowChar;
    FixedText       aCharCodeText;

    Font            aFont;
    BOOL            bOne;
    const SubsetMap* pSubsetMap;

    DECL_LINK( OKHdl, OKButton* );
    DECL_LINK( FontSelectHdl, ListBox* );
    DECL_LINK( SubsetSelectHdl, ListBox* );
    DECL_LINK( CharDoubleClickHdl, Control* pControl );
    DECL_LINK( CharSelectHdl, Control* pControl );
    DECL_LINK( CharHighlightHdl, Control* pControl );
    DECL_LINK( CharPreSelectHdl, Control* pControl );
    DECL_LINK( DeleteHdl, PushButton* pBtn );
};


// -----------------------------------------------------------------------

long SvxShowCharSet::nSelectedIndex = -1;// TODO: remove "static" at next incompatible build
static sal_Unicode cSelectedChar = ' '; // keeps selected character over app livetime

// -----------------------------------------------------------------------

static int UnicodeToMapIndex( const FontCharMap& rMap, sal_UCS4 cChar )
{
    int nIndex = 0;
    ULONG nRanges = rMap.GetRangeCount();
    for( ULONG i = 0; i < nRanges; ++i )
    {
        sal_UCS4 cFirst, cLast;
        rMap.GetRange( i, cFirst, cLast );
        if( cChar < cLast )
            if( cChar < cFirst )
                break;
            else
                return nIndex + (cChar - cFirst);
        nIndex += cLast - cFirst;
    }

    return -1;
}

// -----------------------------------------------------------------------

static sal_UCS4 MapIndexToUnicode( const FontCharMap& rMap, unsigned nIndex )
{
    ULONG nRanges = rMap.GetRangeCount();
    for( ULONG i = 0; i < nRanges; ++i )
    {
        sal_UCS4 cFirst, cLast;
        rMap.GetRange( i, cFirst, cLast );
        if( nIndex < cLast - cFirst )
            return (cFirst + nIndex);
        nIndex -= cLast - cFirst;
    }

    // we should never get here
    return rMap.GetFirstChar();
}

// class SvxShowCharSet ==================================================

#define SBWIDTH 16

SvxShowCharSet::SvxShowCharSet( Window* pParent, const ResId& rResId ) :
    Control( pParent, rResId ),
    aVscrollSB( this, WB_VERT)
{
    nSelectedIndex = -1;    // TODO: move into init list when it is no longer static

    aOrigSize = GetOutputSizePixel();
    aOrigPos = GetPosPixel();

    SetStyle( GetStyle() | WB_CLIPCHILDREN );
    aVscrollSB.SetScrollHdl( LINK( this, SvxShowCharSet, VscrollHdl ) );
    // other settings like aVscroll depend on selected font => see SetFont

    bDrag = FALSE;
    InitSettings( TRUE, TRUE );
}

// -----------------------------------------------------------------------

void SvxShowCharSet::GetFocus()
{
    Control::GetFocus();
    SelectIndex( nSelectedIndex, TRUE );
}

// -----------------------------------------------------------------------

void SvxShowCharSet::LoseFocus()
{
    Control::LoseFocus();
    SelectIndex( nSelectedIndex, FALSE );
}

// -----------------------------------------------------------------------

void SvxShowCharSet::StateChanged( StateChangedType nType )
{
    if ( nType == STATE_CHANGE_CONTROLFOREGROUND )
        InitSettings( TRUE, FALSE );
    else if ( nType == STATE_CHANGE_CONTROLBACKGROUND )
        InitSettings( FALSE, TRUE );

    Control::StateChanged( nType );
}

// -----------------------------------------------------------------------

void SvxShowCharSet::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( ( rDCEvt.GetType() == DATACHANGED_SETTINGS )
      && ( rDCEvt.GetFlags() & SETTINGS_STYLE ) )
        InitSettings( TRUE, TRUE );
    else
        Control::DataChanged( rDCEvt );
}

// -----------------------------------------------------------------------

void SvxShowCharSet::MouseButtonDown( const MouseEvent& rMEvt )
{
    if ( rMEvt.IsLeft() )
    {
        if ( rMEvt.GetClicks() == 1 )
        {
            GrabFocus();
            bDrag = TRUE;
            CaptureMouse();

            int nIndex = PixelToMapIndex( rMEvt.GetPosPixel() );
            SelectIndex( nIndex );
        }

        if ( !(rMEvt.GetClicks() % 2) )
            aDoubleClkHdl.Call( this );
    }
}

// -----------------------------------------------------------------------

void SvxShowCharSet::MouseButtonUp( const MouseEvent& rMEvt )
{
    if ( bDrag && rMEvt.IsLeft() )
    {
        // released mouse over character map
        if ( Rectangle(Point(), GetOutputSize()).IsInside(rMEvt.GetPosPixel()))
            aSelectHdl.Call( this );
        ReleaseMouse();
        bDrag = FALSE;
    }
}

// -----------------------------------------------------------------------

void SvxShowCharSet::MouseMove( const MouseEvent& rMEvt )
{
    if ( rMEvt.IsLeft() && bDrag )
    {
        Point aPos  = rMEvt.GetPosPixel();
        Size  aSize = GetSizePixel();

        if ( aPos.X() < 0 )
            aPos.X() = 0;
        else if ( aPos.X() > aSize.Width()-5 )
            aPos.X() = aSize.Width()-5;
        if ( aPos.Y() < 0 )
            aPos.Y() = 0;
        else if ( aPos.Y() > aSize.Height()-5 )
            aPos.Y() = aSize.Height()-5;

        int nIndex = PixelToMapIndex( aPos );
        SelectIndex( nIndex );
    }
}

// -----------------------------------------------------------------------

void SvxShowCharSet::Command( const CommandEvent& rCEvt )
{
    if( !HandleScrollCommand( rCEvt, 0, &aVscrollSB ) )
        Control::Command( rCEvt );
}

// -----------------------------------------------------------------------

inline int SvxShowCharSet::FirstInView( void ) const
{
    int nIndex = 0;
    if( aVscrollSB.IsVisible() )
        nIndex += aVscrollSB.GetThumbPos() * COLUMN_COUNT;
    return nIndex;
}

// -----------------------------------------------------------------------

inline int SvxShowCharSet::LastInView( void ) const
{
    ULONG nIndex = FirstInView();
    nIndex += ROW_COUNT * COLUMN_COUNT;
    return Min( nIndex, maFontCharMap.GetCharCount() ) - 1;
}

// -----------------------------------------------------------------------

inline Point SvxShowCharSet::MapIndexToPixel( int nIndex ) const
{
    const int nBase = FirstInView();
    int x = ((nIndex - nBase) % COLUMN_COUNT) * nX;
    int y = ((nIndex - nBase) / COLUMN_COUNT) * nY;
    return Point( x, y );
}

// -----------------------------------------------------------------------

int SvxShowCharSet::PixelToMapIndex( const Point point) const
{
    int nBase = FirstInView();
    return (nBase + (point.X()/nX) + (point.Y()/nY) * COLUMN_COUNT);
}

// -----------------------------------------------------------------------

void SvxShowCharSet::KeyInput( const KeyEvent& rKEvt )
{
    KeyCode aCode = rKEvt.GetKeyCode();

    if( aCode.GetModifier() )
    {
        Control::KeyInput( rKEvt );
        return;
    }

    int tmpSelected = nSelectedIndex;

    switch ( aCode.GetCode() )
    {
        case KEY_SPACE:
            aSelectHdl.Call( this );
            break;
        case KEY_LEFT:
            --tmpSelected;
            break;
        case KEY_RIGHT:
            ++tmpSelected;
            break;
        case KEY_UP:
            tmpSelected -= COLUMN_COUNT;
            break;
        case KEY_DOWN:
            tmpSelected += COLUMN_COUNT;
            break;
        case KEY_PAGEUP:
            tmpSelected -= ROW_COUNT * COLUMN_COUNT;
            break;
        case KEY_PAGEDOWN:
            tmpSelected += ROW_COUNT * COLUMN_COUNT;
            break;
        case KEY_HOME:
            tmpSelected = 0;
            break;
        case KEY_END:
            tmpSelected = maFontCharMap.GetCharCount() - 1;
            break;
        case KEY_TAB:   // some fonts have a character at these unicode control codes
        case KEY_ESCAPE:
        case KEY_RETURN:
            Control::KeyInput( rKEvt );
            tmpSelected = - 1;  // mark as invalid
            break;
        default:
            {
                sal_Unicode cChar = rKEvt.GetCharCode();
                sal_Unicode cNext = maFontCharMap.GetNextChar( cChar - 1 );
                tmpSelected = UnicodeToMapIndex( maFontCharMap, cNext );
                if( tmpSelected < 0 || (cChar != cNext) )
                {
                    Control::KeyInput( rKEvt );
                    tmpSelected = - 1;  // mark as invalid
                }
            }
    }

    if ( tmpSelected >= 0 )
    {
        SelectIndex( tmpSelected, TRUE );
        aPreSelectHdl.Call( this );
    }
}

// -----------------------------------------------------------------------

void SvxShowCharSet::Paint( const Rectangle& )
{
    DrawChars_Impl( FirstInView(), LastInView() );
}

// -----------------------------------------------------------------------

void SvxShowCharSet::DrawChars_Impl( int n1, int n2)
{
    if( n1 > LastInView() || n2 < FirstInView() )
        return;

    Size aOutputSize = GetOutputSizePixel();
    if( aVscrollSB.IsVisible())
        aOutputSize.setWidth( aOutputSize.Width() - SBWIDTH);

    int i;
    for ( i = 1; i < COLUMN_COUNT; ++i )
        DrawLine( Point( nX * i, 0 ), Point( nX * i, aOutputSize.Height() ) );
    for ( i = 1; i < ROW_COUNT; ++i )
        DrawLine( Point( 0, nY * i ), Point( aOutputSize.Width(), nY * i ) );

    for ( i = n1; i <= n2; ++i)
    {
        Point pix = MapIndexToPixel( i );
        int x = pix.X();
        int y = pix.Y();

        String aCharStr( MapIndexToUnicode( maFontCharMap, i ) );
        int tx = x + ( nX - GetTextWidth(aCharStr) ) / 2;
        int ty = y + ( nY - GetTextHeight() ) / 2;

        if ( i != nSelectedIndex /*&& HasFocus()*/ )
            DrawText( Point( tx, ty ), aCharStr );
        else
        {
            const StyleSettings& rStyleSettings =
                Application::GetSettings().GetStyleSettings();

            Color aLineCol = GetLineColor();
            Color aFillCol = GetFillColor();
            SetLineColor();
            if( HasFocus() )
            {
                SetFillColor( rStyleSettings.GetHighlightColor() );
                DrawRect( Rectangle( Point(x+1,y+1), Size(nX-1,nY-1) ) );

                Color aTextCol = GetTextColor();
                SetTextColor( rStyleSettings.GetHighlightTextColor() );
                DrawText( Point( tx, ty ), aCharStr );
                SetTextColor( aTextCol );
            }
            else
            {
                SetFillColor( rStyleSettings.GetFaceColor() );
                DrawRect( Rectangle( Point( x+1, y+1), Size( nX-1, nY-1) ) );
                SetLineColor( rStyleSettings.GetLightColor() );
                DrawLine( Point( x+1,y+1 ), Point( x+nX-1, y+1) );
                DrawLine( Point( x+1,y+1 ), Point( x+1, y+nY-1) );
                SetLineColor( rStyleSettings.GetShadowColor() );
                DrawLine( Point( x+1, y+nY-1), Point( x+nX-1, y+nY-1) );
                DrawLine( Point( x+nX-1, y+nY-1), Point( x+nX-1, y+1) );

                DrawText( Point( tx, ty ), aCharStr );
            }
            SetLineColor( aLineCol );
            SetFillColor( aFillCol );
        }
    }
}

// -----------------------------------------------------------------------

void SvxShowCharSet::InitSettings( BOOL bForeground, BOOL bBackground )
{
    const StyleSettings& rStyleSettings = Application::GetSettings().GetStyleSettings();

    if ( bForeground )
    {
        Color aTextColor = rStyleSettings.GetWindowTextColor();

        if ( IsControlForeground() )
            aTextColor = GetControlForeground();
        SetTextColor( aTextColor );
    }

    if ( bBackground )
    {
        if ( IsControlBackground() )
            SetBackground( GetControlBackground() );
        else
            SetBackground( rStyleSettings.GetWindowColor() );
    }

    Invalidate();
}

// -----------------------------------------------------------------------

sal_Unicode SvxShowCharSet::GetSelectCharacter() const
{
    if( nSelectedIndex >= 0 )
        cSelectedChar = MapIndexToUnicode( maFontCharMap, nSelectedIndex );
    return cSelectedChar;
}

// -----------------------------------------------------------------------

void SvxShowCharSet::SetFont( const Font& rFont )
{
    // save last selected unicode
    if( nSelectedIndex >= 0 )
        cSelectedChar = MapIndexToUnicode( maFontCharMap, nSelectedIndex );

    Font aFont = rFont;
    aFont.SetWeight( WEIGHT_LIGHT );
    int nFontHeight = (aOrigSize.Height() - 5) * 2 / (3 * ROW_COUNT);
    aFont.SetSize( PixelToLogic( Size( 0, nFontHeight ) ) );
    aFont.SetTransparent( TRUE );
    Control::SetFont( aFont );
    GetFontCharMap( maFontCharMap );

    // hide scrollbar when there is nothing to scroll
    BOOL bNeedVscroll = (maFontCharMap.GetCharCount() > ROW_COUNT*COLUMN_COUNT);

    nX = (aOrigSize.Width() - (bNeedVscroll ? SBWIDTH : 0)) / COLUMN_COUNT;
    nY = aOrigSize.Height() / ROW_COUNT;

    if( bNeedVscroll)
    {
        aVscrollSB.SetPosSizePixel( nX * COLUMN_COUNT, 0, SBWIDTH, nY * ROW_COUNT );
        aVscrollSB.SetRangeMin( 0 );
        int nLastRow = (maFontCharMap.GetCharCount() - 1 + COLUMN_COUNT) / COLUMN_COUNT;
        aVscrollSB.SetRangeMax( nLastRow );
        aVscrollSB.SetPageSize( ROW_COUNT-1 );
        aVscrollSB.SetVisibleSize( ROW_COUNT );
    }

    // restore last selected unicode
    int nMapIndex = UnicodeToMapIndex( maFontCharMap, cSelectedChar );
    SelectIndex( nMapIndex );

    // rearrange CharSet element in sync with nX- and nY-multiples
    Size aNewSize( nX * COLUMN_COUNT + (bNeedVscroll ? SBWIDTH : 0), nY * ROW_COUNT );
    Point aNewPos = aOrigPos + Point( (aOrigSize.Width() - aNewSize.Width()) / 2, 0 );
    SetPosPixel( aNewPos );
    SetOutputSizePixel( aNewSize );

    aVscrollSB.Show( bNeedVscroll );
    Invalidate();
}

// -----------------------------------------------------------------------

void SvxShowCharSet::SelectIndex( int nNewIndex, BOOL bFocus )
{
    if( nNewIndex < 0 )
    {
        // need to scroll see closest unicode
        sal_Unicode cPrev = maFontCharMap.GetPrevChar( cSelectedChar );
        int nMapIndex = UnicodeToMapIndex( maFontCharMap, cPrev );
        int nNewPos = nMapIndex / COLUMN_COUNT;
        aVscrollSB.SetThumbPos( nNewPos );
        nSelectedIndex = bFocus ? nMapIndex+1 : -1;
        Invalidate();
        Update();
    }
    else if( nNewIndex < FirstInView() )
    {
        // need to scroll up to see selected item
        int nOldPos = aVscrollSB.GetThumbPos();
        int nDelta = (FirstInView() - nNewIndex + COLUMN_COUNT-1) / COLUMN_COUNT;
        aVscrollSB.SetThumbPos( nOldPos - nDelta );
        nSelectedIndex = nNewIndex;
        Invalidate();
        if( nDelta )
            Update();
    }
    else if( nNewIndex > LastInView() )
    {
        // need to scroll down to see selected item
        int nOldPos = aVscrollSB.GetThumbPos();
        int nDelta = (nNewIndex - LastInView() + COLUMN_COUNT) / COLUMN_COUNT;
        aVscrollSB.SetThumbPos( nOldPos + nDelta );
        if( nNewIndex < maFontCharMap.GetCharCount() )
        {
            nSelectedIndex = nNewIndex;
            Invalidate();
        }
        if( nOldPos != aVscrollSB.GetThumbPos() )
        {
            Invalidate();
            Update();
        }
    }
    else
    {
        // remove highlighted view
        Color aLineCol = GetLineColor();
        Color aFillCol = GetFillColor();
        SetLineColor();
        SetFillColor( GetBackground().GetColor() );

        Point aOldPixel = MapIndexToPixel( nSelectedIndex );
        aOldPixel.Move( +1, +1);
        DrawRect( Rectangle( aOldPixel, Size( nX-1, nY-1 ) ) );
        SetLineColor( aLineCol );
        SetFillColor( aFillCol );

        int nOldIndex = nSelectedIndex;
        nSelectedIndex = nNewIndex;
        DrawChars_Impl( nOldIndex, nOldIndex );
        DrawChars_Impl( nNewIndex, nNewIndex );
    }

    if( nSelectedIndex >= 0 )
        cSelectedChar = MapIndexToUnicode( maFontCharMap, nSelectedIndex );

    aHighHdl.Call( this );
}

// -----------------------------------------------------------------------

void SvxShowCharSet::SelectCharacter( sal_Unicode cNew, BOOL bFocus )
{
    // get next available char of current font
    sal_Unicode cNext = maFontCharMap.GetNextChar( cNew - 1 );

    int nMapIndex = UnicodeToMapIndex( maFontCharMap, cNext );
    SelectIndex( nMapIndex, bFocus );
    if( !bFocus )
    {
        // move selected item to top row if not in focus
        aVscrollSB.SetThumbPos( nMapIndex / COLUMN_COUNT );
        Invalidate();
    }
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxShowCharSet, VscrollHdl, ScrollBar *, EMPTYARG )
{
    if( nSelectedIndex < FirstInView() )
        SelectIndex( FirstInView() + (nSelectedIndex % COLUMN_COUNT) );
    else if( nSelectedIndex > LastInView() )
        SelectIndex( (LastInView() - COLUMN_COUNT + 1) + (nSelectedIndex % COLUMN_COUNT) );

    Invalidate();
    return 0;
}

// -----------------------------------------------------------------------

SvxShowCharSet::~SvxShowCharSet()
{}

// class SvxShowText =====================================================

SvxShowText::SvxShowText( Window* pParent, const ResId& rResId, BOOL _bCenter )
:   Control( pParent, rResId ),
    bCenter( _bCenter)
{}

// -----------------------------------------------------------------------

void SvxShowText::Paint( const Rectangle& )
{
    if ( bCenter )
    {
        String aText = GetText();
        DrawText( Point( (GetOutputSizePixel().Width() - GetTextWidth(aText))/2, nY),
            aText );
    }
    else
        DrawText( Point( 2, nY ), GetText() );
}

// -----------------------------------------------------------------------

void SvxShowText::SetFont( const Font& rFont )
{
    long nWinHeight = GetOutputSizePixel().Height();
    Font aFont = rFont;
    aFont.SetWeight( WEIGHT_NORMAL );
    aFont.SetSize( PixelToLogic( Size( 0, nWinHeight-6 ) ) );
    aFont.SetTransparent( TRUE );
    Control::SetFont( aFont );
    nY = ( nWinHeight - GetTextHeight() ) / 2;

    Invalidate();
}

// -----------------------------------------------------------------------

void SvxShowText::SetText( const String& rText )
{
    Control::SetText( rText );
    Invalidate();
}

// -----------------------------------------------------------------------

SvxShowText::~SvxShowText()
{}

// class SvxCharacterMap =================================================

SvxCharacterMap::SvxCharacterMap( Window* pParent, BOOL bOne ) :
    SfxModalDialog( pParent, SVX_RES( RID_SVXDLG_CHARMAP ) ),
    mpCharMapData( new SvxCharMapData( this, bOne ) )
{
    FreeResource();
}

// -----------------------------------------------------------------------

SvxCharacterMap::~SvxCharacterMap()
{
    delete mpCharMapData;
}

// -----------------------------------------------------------------------

const Font& SvxCharacterMap::GetCharFont() const
{
    return mpCharMapData->aFont;
}

// -----------------------------------------------------------------------

void SvxCharacterMap::SetChar( sal_Unicode c )
{
    mpCharMapData->aShowSet.SelectCharacter( c );
}

// -----------------------------------------------------------------------

sal_Unicode SvxCharacterMap::GetChar() const
{
    return mpCharMapData->aShowSet.GetSelectCharacter();
}

// -----------------------------------------------------------------------

String SvxCharacterMap::GetCharacters() const
{
    return mpCharMapData->aShowText.GetText();
}

// =======================================================================

SvxCharMapData::SvxCharMapData( SfxModalDialog* pDialog, BOOL bOne_ )
:   mpDialog( pDialog ),
    aShowSet        ( pDialog, ResId( CT_SHOWSET ) ),
    aFontText       ( pDialog, ResId( FT_FONT ) ),
    aFontLB         ( pDialog, ResId( LB_FONT ) ),
    aSubsetText     ( pDialog, ResId( FT_SUBSET ) ),
    aSubsetLB       ( pDialog, ResId( LB_SUBSET ) ),
    aSymbolText     ( pDialog, ResId( FT_SYMBOLE ) ),
    aShowText       ( pDialog, ResId( CT_SHOWTEXT ) ),
    aShowChar       ( pDialog, ResId( CT_SHOWCHAR ), TRUE ),
    aCharCodeText   ( pDialog, ResId( FT_CHARCODE ) ),
    aOKBtn          ( pDialog, ResId( BTN_CHAR_OK ) ),
    aCancelBtn      ( pDialog, ResId( BTN_CHAR_CANCEL ) ),
    aHelpBtn        ( pDialog, ResId( BTN_CHAR_HELP ) ),
    aDeleteBtn      ( pDialog, ResId( BTN_DELETE ) ),
    pSubsetMap( NULL ),
    bOne( bOne_ )
{
    aFont = pDialog->GetFont();
    aFont.SetTransparent( TRUE );
    aFont.SetFamily( FAMILY_DONTKNOW );
    aFont.SetPitch( PITCH_DONTKNOW );
    aFont.SetCharSet( RTL_TEXTENCODING_DONTKNOW );

    if ( bOne )
    {
        Size aDlgSize = pDialog->GetSizePixel();
        pDialog->SetSizePixel( Size( aDlgSize.Width(),
            aDlgSize.Height()-aShowText.GetSizePixel().Height() ) );
        aSymbolText.Hide();
        aShowText.Hide();
        aDeleteBtn.Hide();
    }

    String aDefStr( aFont.GetName() );
    String aLastName;
    xub_StrLen i;
    xub_StrLen nCount = mpDialog->GetDevFontCount();
    for ( i = 0; i < nCount; i++ )
    {
        String aFontName( mpDialog->GetDevFont( i ).GetName() );
        if ( aFontName != aLastName )
        {
            aLastName = aFontName;
            USHORT nPos = aFontLB.InsertEntry( aFontName );
            aFontLB.SetEntryData( nPos, (void*)(ULONG)i );
        }
    }
    // the font may not be in the list =>
    // try to find a font name token in list and select found font,
    // else select topmost entry
    FASTBOOL bFound = (aFontLB.GetEntryPos( aDefStr ) == LISTBOX_ENTRY_NOTFOUND );
    if( !bFound )
    {
        for ( i = 0; i < aDefStr.GetTokenCount(); ++i )
        {
            String aToken = aDefStr.GetToken(i);
            if ( aFontLB.GetEntryPos( aToken ) != LISTBOX_ENTRY_NOTFOUND )
            {
                aDefStr = aToken;
                bFound = TRUE;
                break;
            }
        }
    }

    if ( bFound )
        aFontLB.SelectEntry( aDefStr );
    else if ( aFontLB.GetEntryCount() )
        aFontLB.SelectEntryPos(0);
    FontSelectHdl( &aFontLB );

    aOKBtn.SetClickHdl( LINK( this, SvxCharMapData, OKHdl ) );
    aFontLB.SetSelectHdl( LINK( this, SvxCharMapData, FontSelectHdl ) );
    aSubsetLB.SetSelectHdl( LINK( this, SvxCharMapData, SubsetSelectHdl ) );
    aShowSet.SetDoubleClickHdl( LINK( this, SvxCharMapData, CharDoubleClickHdl ) );
    aShowSet.SetSelectHdl( LINK( this, SvxCharMapData, CharSelectHdl ) );
    aShowSet.SetHighlightHdl( LINK( this, SvxCharMapData, CharHighlightHdl ) );
    aShowSet.SetPreSelectHdl( LINK( this, SvxCharMapData, CharPreSelectHdl ) );
    aDeleteBtn.SetClickHdl( LINK( this, SvxCharMapData, DeleteHdl ) );

    aOKBtn.Disable();

    // left align aShowText field
    int nLeftEdge = aSymbolText.GetPosPixel().X();
    nLeftEdge += aSymbolText.GetTextWidth( aSymbolText.GetText() );
    Size aNewSize = aShowText.GetOutputSizePixel();
    aShowText.SetPosPixel( Point( nLeftEdge+4, aShowText.GetPosPixel().Y() ) );
}

// -----------------------------------------------------------------------

void SvxCharacterMap::DisableFontSelection()
{
    mpCharMapData->aFontText.Disable();
    mpCharMapData->aFontLB.Disable();
}

// -----------------------------------------------------------------------

void SvxCharMapData::SetCharFont( const Font& rFont )
{
    // first get the underlying info in order to get font names
    // like "Times New Roman;Times" resolved
    Font aTmp( mpDialog->GetFontMetric( rFont ) );

    if ( aFontLB.GetEntryPos( aTmp.GetName() ) == LISTBOX_ENTRY_NOTFOUND )
        return;

    aFontLB.SelectEntry( aTmp.GetName() );
    aFont = aTmp;
    FontSelectHdl( &aFontLB );

    // for compatibility reasons
    mpDialog->ModalDialog::SetFont( aFont );
}

// -----------------------------------------------------------------------

void SvxCharacterMap::SetCharFont( const Font& rFont )
{
    mpCharMapData->SetCharFont( rFont );
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxCharMapData, OKHdl, OKButton *, EMPTYARG )
{
    String aStr = aShowText.GetText();

    if ( !aStr.Len() )
    {
        if ( aShowSet.GetSelectCharacter() > 0 )
            aStr = aShowSet.GetSelectCharacter();
        aShowText.SetText( aStr );
    }
    mpDialog->EndDialog( TRUE );
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxCharMapData, FontSelectHdl, ListBox *, EMPTYARG )
{
    USHORT nPos = aFontLB.GetSelectEntryPos(),
        nFont = (USHORT)(ULONG)aFontLB.GetEntryData( nPos );
    aFont = mpDialog->GetDevFont( nFont );

    // notify children using this font
    aShowSet.SetFont( aFont );
    aShowChar.SetFont( aFont );
    aShowText.SetFont( aFont );

    // right align some fields to aShowSet
    int nRightEdge = aShowSet.GetPosPixel().X() + aShowSet.GetOutputSizePixel().Width();
    Size aNewSize = aSubsetLB.GetOutputSizePixel();
    aNewSize.setWidth( nRightEdge - aSubsetLB.GetPosPixel().X() );
    aSubsetLB.SetOutputSizePixel( aNewSize );

    // setup unicode subset listbar with font specific subsets,
    // hide unicode subset listbar for symbol fonts
    // TODO: get info from the Font once it provides it
    if( pSubsetMap)
        delete pSubsetMap;
    pSubsetMap = NULL;

    BOOL bNeedSubset = (aFont.GetCharSet() != RTL_TEXTENCODING_SYMBOL);
    if( bNeedSubset )
    {
        FontCharMap aFontCharMap;
        aShowSet.GetFontCharMap( aFontCharMap );
        pSubsetMap = new SubsetMap( &aFontCharMap );

        // update subset listbox for new font's unicode subsets
        aSubsetLB.Clear();
        const Subset* s = 0;
        // TODO: is it worth to improve the stupid linear search?
        for( int i = 0; (s = pSubsetMap->GetSubsetByIndex( i)) != 0; ++i )
        {
            USHORT nPos = aSubsetLB.InsertEntry( s->GetName());
            aSubsetLB.SetEntryData( nPos, (void*)s );
            // subset must live at least as long as the selected font !!!
            if( i == 0)
                aSubsetLB.SelectEntryPos( nPos);
        }
        if( aSubsetLB.GetEntryCount() <= 1)
            bNeedSubset = FALSE;
    }

    aSubsetText.Show( bNeedSubset);
    aSubsetLB.Show( bNeedSubset);

    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxCharMapData, SubsetSelectHdl, ListBox *, EMPTYARG )
{
    USHORT nPos = aSubsetLB.GetSelectEntryPos();
    const Subset* pSubset = reinterpret_cast<const Subset*> (aSubsetLB.GetEntryData(nPos));
    if( pSubset )
    {
        sal_Unicode cFirst = pSubset->GetRangeMin();
        aShowSet.SelectCharacter( cFirst );
    }
    aSubsetLB.SelectEntryPos( nPos );
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxCharMapData, CharDoubleClickHdl, Control *, EMPTYARG )
{
    mpDialog->EndDialog( TRUE );
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxCharMapData, CharSelectHdl, Control *, EMPTYARG )
{
    if ( !bOne )
    {
        String aText = aShowText.GetText();

        if ( aText.Len() == CHARMAP_MAXLEN )
            Sound::Beep( SOUND_WARNING );
        else
        {
            if ( aShowSet.GetSelectCharacter() > 0 )
                aText += aShowSet.GetSelectCharacter();
            aShowText.SetText( aText );
        }

    }
    aOKBtn.Enable();
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxCharMapData, CharHighlightHdl, Control *, EMPTYARG )
{
    String aTemp;
    sal_Unicode c = aShowSet.GetSelectCharacter();
    sal_Bool bSelect = ( c > 0 );
    if ( bSelect )
    {
        aTemp = c;
        const Subset* pSubset = NULL;
        if( pSubsetMap )
            pSubset = pSubsetMap->GetSubsetByUnicode( c );
        if( pSubset )
            aSubsetLB.SelectEntry( pSubset->GetName() );
        else
            aSubsetLB.SetNoSelection();
    }
    aShowChar.SetText( aTemp );
    aShowChar.Update();
    if ( bSelect )
    {
        // no sprintf or hex-formatter around :-(
        char buf[16] = "0x0000";
        sal_Unicode c_Shifted = c;
        for( int i = 0; i < 4; ++i )
        {
            char h = c_Shifted & 0x0F;
            buf[5-i] = (h > 9) ? (h - 10 + 'A') : (h + '0');
            c_Shifted >>= 4;
        }
        if( c < 256 )
            sprintf( buf+6, " (%d)", c );
        aTemp = String::CreateFromAscii( buf );
    }
    aCharCodeText.SetText( aTemp );
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxCharMapData, CharPreSelectHdl, Control *, EMPTYARG )
{
    // adjust subset selection
    if( pSubsetMap )
    {
        sal_Unicode cChar = aShowSet.GetSelectCharacter();
        const Subset* pSubset = pSubsetMap->GetSubsetByUnicode( cChar );
        if( pSubset )
            aSubsetLB.SelectEntry( pSubset->GetName() );
    }

    aOKBtn.Enable();
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxCharMapData, DeleteHdl, PushButton *, EMPTYARG )
{
    aShowText.SetText( String() );
    aOKBtn.Disable();
    return 0;
}

// class SubsetMap =======================================================
// TODO: should be moved into Font Attributes stuff
// we let it mature here though because it is currently the only use

static const Subset** ppAllSubsets = NULL;
static int nAllSubSetCount = 0;

SubsetMap::SubsetMap( const FontCharMap* pFontCharMap )
:   Resource( ResId( SVX_RES(RID_SUBSETMAP) ) )
{
    InitList();
    ApplyCharMap( pFontCharMap );
    FreeResource();
}

SubsetMap::~SubsetMap()
{
    delete[] maSubsets;
}

int SubsetMap::GetSubsetCount() const
{
    return mnSubsets;
}

const Subset* SubsetMap::GetSubsetByIndex( int nIndex ) const
{
    if( (nIndex >= 0) && (nIndex < mnSubsets))
        return maSubsets[ nIndex ];
    return NULL;
}

const Subset* SubsetMap::GetSubsetByUnicode( sal_Unicode cChar ) const
{
    const Subset* s = NULL;
    // TODO: is it worth to avoid a linear search?
    for( int i = 0; (s = GetSubsetByIndex( i)) != 0; ++i)
        if( (cChar >= s->GetRangeMin()) && (cChar <= s->GetRangeMax()))
            break;
    return s;
}

inline Subset::Subset( sal_Unicode _min, sal_Unicode _max, int resId)
:   mnRangeMin(_min), mnRangeMax(_max), maRangeName( ResId(resId) )
{}

void SubsetMap::InitList()
{
    if( !ppAllSubsets )
    {
        ppAllSubsets = new const Subset*[ RID_SUBSET_COUNT];
        const Subset** ppSubSet = ppAllSubsets;

        // TODO: eventually merge or split unicode subranges
        //       a "native writer" should decide for his subsets
        *(ppSubSet++) = new Subset( 0x0020, 0x007A, RID_SUBSETSTR_BASIC_LATIN);
        *(ppSubSet++) = new Subset( 0x00A0, 0x00FF, RID_SUBSETSTR_LATIN_1);
        *(ppSubSet++) = new Subset( 0x0100, 0x017F, RID_SUBSETSTR_LATIN_EXTENDED_A);
        *(ppSubSet++) = new Subset( 0x0180, 0x024F, RID_SUBSETSTR_LATIN_EXTENDED_B);
        *(ppSubSet++) = new Subset( 0x0250, 0x02AF, RID_SUBSETSTR_IPA_EXTENSIONS);
        *(ppSubSet++) = new Subset( 0x02B0, 0x02FF, RID_SUBSETSTR_SPACING_MODIFIERS);
        *(ppSubSet++) = new Subset( 0x0300, 0x036F, RID_SUBSETSTR_COMB_DIACRITICAL);
        *(ppSubSet++) = new Subset( 0x0370, 0x03FF, RID_SUBSETSTR_BASIC_GREEK);
    //  *(ppSubSet++) = new Subset( 0x03D0, 0x03F3, RID_SUBSETSTR_GREEK_SYMS_COPTIC);
        *(ppSubSet++) = new Subset( 0x0400, 0x04FF, RID_SUBSETSTR_CYRILLIC);
        *(ppSubSet++) = new Subset( 0x0530, 0x058F, RID_SUBSETSTR_ARMENIAN);
        *(ppSubSet++) = new Subset( 0x0590, 0x05FF, RID_SUBSETSTR_BASIC_HEBREW);
    //  *(ppSubSet++) = new Subset( 0x0591, 0x05C4, RID_SUBSETSTR_HEBREW_EXTENDED);
        *(ppSubSet++) = new Subset( 0x0600, 0x06FF, RID_SUBSETSTR_BASIC_ARABIC);
    //  *(ppSubSet++) = new Subset( 0x0660, 0x06FF, RID_SUBSETSTR_ARABIC_EXTENDED);
        *(ppSubSet++) = new Subset( 0x0700, 0x074F, RID_SUBSETSTR_SYRIAC);
        *(ppSubSet++) = new Subset( 0x0780, 0x07BF, RID_SUBSETSTR_THAANA);
        *(ppSubSet++) = new Subset( 0x0900, 0x097F, RID_SUBSETSTR_DEVANAGARI);
        *(ppSubSet++) = new Subset( 0x0980, 0x09FF, RID_SUBSETSTR_BENGALI);
        *(ppSubSet++) = new Subset( 0x0A00, 0x0A7F, RID_SUBSETSTR_GURMUKHI);
        *(ppSubSet++) = new Subset( 0x0A80, 0x0AFF, RID_SUBSETSTR_GUJARATI);
        *(ppSubSet++) = new Subset( 0x0B00, 0x0B7F, RID_SUBSETSTR_ORIYA);
        *(ppSubSet++) = new Subset( 0x0B80, 0x0BFF, RID_SUBSETSTR_TAMIL);
        *(ppSubSet++) = new Subset( 0x0C00, 0x0C7F, RID_SUBSETSTR_TELUGU);
        *(ppSubSet++) = new Subset( 0x0C80, 0x0CFF, RID_SUBSETSTR_KANNADA);
        *(ppSubSet++) = new Subset( 0x0D00, 0x0D7F, RID_SUBSETSTR_MALAYALAM);
        *(ppSubSet++) = new Subset( 0x0D80, 0x0DFF, RID_SUBSETSTR_SINHALA);
        *(ppSubSet++) = new Subset( 0x0E00, 0x0E7F, RID_SUBSETSTR_THAI);
        *(ppSubSet++) = new Subset( 0x0E80, 0x0EFF, RID_SUBSETSTR_LAO);
        *(ppSubSet++) = new Subset( 0x0F00, 0x0FBF, RID_SUBSETSTR_TIBETAN);
        *(ppSubSet++) = new Subset( 0x1000, 0x109F, RID_SUBSETSTR_MYANMAR);
        *(ppSubSet++) = new Subset( 0x10A0, 0x10FF, RID_SUBSETSTR_BASIC_GEORGIAN);
    //  *(ppSubSet++) = new Subset( 0x10A0, 0x10C5, RID_SUBSETSTR_GEORGIAN_EXTENDED);

        *(ppSubSet++) = new Subset( 0x1100, 0x11FF, RID_SUBSETSTR_HANGUL_JAMO);
        *(ppSubSet++) = new Subset( 0x1200, 0x137F, RID_SUBSETSTR_ETHIOPIC);
        *(ppSubSet++) = new Subset( 0x13A0, 0x13FF, RID_SUBSETSTR_CHEROKEE);
        *(ppSubSet++) = new Subset( 0x1400, 0x167F, RID_SUBSETSTR_CANADIAN_ABORIGINAL);
        *(ppSubSet++) = new Subset( 0x1680, 0x169F, RID_SUBSETSTR_OGHAM);
        *(ppSubSet++) = new Subset( 0x16A0, 0x16F0, RID_SUBSETSTR_RUNIC);

        *(ppSubSet++) = new Subset( 0x1780, 0x17FF, RID_SUBSETSTR_KHMER);
        *(ppSubSet++) = new Subset( 0x1800, 0x18AF, RID_SUBSETSTR_MONGOLIAN);
        *(ppSubSet++) = new Subset( 0x1E00, 0x1EFF, RID_SUBSETSTR_LATIN_EXTENDED_ADDS);
        *(ppSubSet++) = new Subset( 0x1F00, 0x1FFF, RID_SUBSETSTR_GREEK_EXTENDED);

        *(ppSubSet++) = new Subset( 0x2000, 0x206F, RID_SUBSETSTR_GENERAL_PUNCTUATION);
        *(ppSubSet++) = new Subset( 0x2070, 0x209F, RID_SUBSETSTR_SUB_SUPER_SCRIPTS);
        *(ppSubSet++) = new Subset( 0x20A0, 0x20CF, RID_SUBSETSTR_CURRENCY_SYMBOLS);
        *(ppSubSet++) = new Subset( 0x20D0, 0x20FF, RID_SUBSETSTR_COMB_DIACRITIC_SYMS);
        *(ppSubSet++) = new Subset( 0x2100, 0x214F, RID_SUBSETSTR_LETTERLIKE_SYMBOLS);
        *(ppSubSet++) = new Subset( 0x2150, 0x218F, RID_SUBSETSTR_NUMBER_FORMS);
        *(ppSubSet++) = new Subset( 0x2190, 0x21FF, RID_SUBSETSTR_ARROWS);
        *(ppSubSet++) = new Subset( 0x2200, 0x22FF, RID_SUBSETSTR_MATH_OPERATORS);
        *(ppSubSet++) = new Subset( 0x2300, 0x23FF, RID_SUBSETSTR_MISC_TECHNICAL);
        *(ppSubSet++) = new Subset( 0x2400, 0x243F, RID_SUBSETSTR_CONTROL_PICTURES);
        *(ppSubSet++) = new Subset( 0x2440, 0x245F, RID_SUBSETSTR_OPTICAL_CHAR_REC);
        *(ppSubSet++) = new Subset( 0x2460, 0x24FF, RID_SUBSETSTR_ENCLOSED_ALPHANUM);
        *(ppSubSet++) = new Subset( 0x2500, 0x257F, RID_SUBSETSTR_BOX_DRAWING);
        *(ppSubSet++) = new Subset( 0x2580, 0x259F, RID_SUBSETSTR_BLOCK_ELEMENTS);
        *(ppSubSet++) = new Subset( 0x25A0, 0x25FF, RID_SUBSETSTR_GEOMETRIC_SHAPES);
        *(ppSubSet++) = new Subset( 0x2600, 0x26FF, RID_SUBSETSTR_MISC_DINGBATS);
        *(ppSubSet++) = new Subset( 0x2700, 0x27BF, RID_SUBSETSTR_DINGBATS);

        *(ppSubSet++) = new Subset( 0x3000, 0x303F, RID_SUBSETSTR_CJK_SYMS_PUNCTUATION);
        *(ppSubSet++) = new Subset( 0x3040, 0x309F, RID_SUBSETSTR_HIRAGANA);
        *(ppSubSet++) = new Subset( 0x30A0, 0x30FF, RID_SUBSETSTR_KATAKANA);
        *(ppSubSet++) = new Subset( 0x3100, 0x312F, RID_SUBSETSTR_BOPOMOFO);
        *(ppSubSet++) = new Subset( 0x3130, 0x318F, RID_SUBSETSTR_HANGUL_COMPAT_JAMO);
        *(ppSubSet++) = new Subset( 0x3190, 0x31FF, RID_SUBSETSTR_CJK_MISC);
        *(ppSubSet++) = new Subset( 0x3200, 0x32FF, RID_SUBSETSTR_ENCLOSED_CJK_LETTERS);
        *(ppSubSet++) = new Subset( 0x3300, 0x33FF, RID_SUBSETSTR_CJK_COMPATIBILITY);

        *(ppSubSet++) = new Subset( 0x3400, 0x4DFF, RID_SUBSETSTR_CJK_EXT_A_UNIFIED_IDGRAPH);
        *(ppSubSet++) = new Subset( 0x4E00, 0x9FA5, RID_SUBSETSTR_CJK_UNIFIED_IDGRAPH);
        *(ppSubSet++) = new Subset( 0xA000, 0xA4CF, RID_SUBSETSTR_YI);
        *(ppSubSet++) = new Subset( 0xAC00, 0xB097, RID_SUBSETSTR_HANGUL_GA);
        *(ppSubSet++) = new Subset( 0xB098, 0xB2E3, RID_SUBSETSTR_HANGUL_NA);
        *(ppSubSet++) = new Subset( 0xB2E4, 0xB77B, RID_SUBSETSTR_HANGUL_DA);
        *(ppSubSet++) = new Subset( 0xB77C, 0xB9C7, RID_SUBSETSTR_HANGUL_RA);
        *(ppSubSet++) = new Subset( 0xB9C8, 0xBC13, RID_SUBSETSTR_HANGUL_MA);
        *(ppSubSet++) = new Subset( 0xBC14, 0xC0AB, RID_SUBSETSTR_HANGUL_BA);
        *(ppSubSet++) = new Subset( 0xC0AC, 0xC543, RID_SUBSETSTR_HANGUL_SA);
        *(ppSubSet++) = new Subset( 0xC544, 0xC78F, RID_SUBSETSTR_HANGUL_AH);
        *(ppSubSet++) = new Subset( 0xC790, 0xCC27, RID_SUBSETSTR_HANGUL_JA);
        *(ppSubSet++) = new Subset( 0xCC28, 0xCE73, RID_SUBSETSTR_HANGUL_CHA);
        *(ppSubSet++) = new Subset( 0xCE74, 0xD0BF, RID_SUBSETSTR_HANGUL_KA);
        *(ppSubSet++) = new Subset( 0xD0C0, 0xD30B, RID_SUBSETSTR_HANGUL_TA);
        *(ppSubSet++) = new Subset( 0xD30C, 0xD557, RID_SUBSETSTR_HANGUL_PA);
        *(ppSubSet++) = new Subset( 0xD558, 0xD7A3, RID_SUBSETSTR_HANGUL_HA);
    //  *(ppSubSet++) = new Subset( 0xAC00, 0xD7A3, RID_SUBSETSTR_HANGUL);

    //  *(ppSubSet++) = new Subset( 0xD800, 0xDFFF, RID_SUBSETSTR_SURROGATE);
        *(ppSubSet++) = new Subset( 0xE000, 0xF8FF, RID_SUBSETSTR_PRIVATE_USE_AREA);
        *(ppSubSet++) = new Subset( 0xF900, 0xFAFF, RID_SUBSETSTR_CJK_COMPAT_IDGRAPHS);
        *(ppSubSet++) = new Subset( 0xFB00, 0xFB4F, RID_SUBSETSTR_ALPHA_PRESENTATION);
        *(ppSubSet++) = new Subset( 0xFB50, 0xFDFF, RID_SUBSETSTR_ARABIC_PRESENT_A);
        *(ppSubSet++) = new Subset( 0xFE20, 0xFE2F, RID_SUBSETSTR_COMBINING_HALF_MARKS);
        *(ppSubSet++) = new Subset( 0xFE30, 0xFE4F, RID_SUBSETSTR_CJK_COMPAT_FORMS);
        *(ppSubSet++) = new Subset( 0xFE50, 0xFE6F, RID_SUBSETSTR_SMALL_FORM_VARIANTS);
        *(ppSubSet++) = new Subset( 0xFE70, 0xFEFF, RID_SUBSETSTR_ARABIC_PRESENT_B);
        *(ppSubSet++) = new Subset( 0xFF00, 0xFFEF, RID_SUBSETSTR_HALFW_FULLW_FORMS);
        *(ppSubSet++) = new Subset( 0xFFF0, 0xFFFF, RID_SUBSETSTR_SPECIALS);

        nAllSubSetCount = ppSubSet - ppAllSubsets;

        DBG_ASSERT( (nAllSubSetCount < RID_SUBSET_COUNT), "RID_SUBSET_COUNT too small");
        DBG_ASSERT( (2*nAllSubSetCount > RID_SUBSET_COUNT), "RID_SUBSET_COUNT way to big");
    }

    mnSubsets = nAllSubSetCount;
    maSubsets = new const Subset*[ mnSubsets ];
    for( int i = 0; i < mnSubsets; ++i )
        maSubsets[i] = ppAllSubsets[i];
}

void SubsetMap::ApplyCharMap( const FontCharMap* pFontCharMap )
{
    if( !pFontCharMap )
        return;

    int nSrc = 0;
    int nDst = 0;
    ULONG nIndex = 0;
    const ULONG nRanges = pFontCharMap->GetRangeCount();
    while( nSrc < mnSubsets )
    {
        sal_UCS4 cFirst, cLast;
        if( nIndex < nRanges )
            pFontCharMap->GetRange( nIndex, cFirst, cLast );
        else
            cFirst = cLast = ~0;

        const Subset* pS = maSubsets[ nSrc ];
        if( cLast <= pS->GetRangeMin() )
            if( nIndex < nRanges )
                { ++nIndex; continue; }

        if( cFirst > pS->GetRangeMax() )
            maSubsets[ nSrc ] = NULL;
        else if( nSrc != nDst )
        {
            maSubsets[ nDst++ ] = maSubsets[ nSrc ];
            maSubsets[ nSrc ] = NULL;
        } else
            ++nDst;

        ++nSrc;
    }

    mnSubsets = nDst;
}
