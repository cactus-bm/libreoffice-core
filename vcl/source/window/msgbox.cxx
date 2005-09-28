/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: msgbox.cxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: hr $ $Date: 2005-09-28 14:51:46 $
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

#ifndef _SV_SVIDS_HRC
#include <svids.hrc>
#endif
#ifndef _SV_SVDATA_HXX
#include <svdata.hxx>
#endif
#ifndef _SV_METRIC_HXX
#include <metric.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <svapp.hxx>
#endif
#ifndef _SV_WRKWIN_HXX
#include <wrkwin.hxx>
#endif
#ifndef _SV_FIXED_HXX
#include <fixed.hxx>
#endif
#ifndef _SV_SOUND_HXX
#include <sound.hxx>
#endif
#ifndef _SV_BRDWIN_HXX
#include <brdwin.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <msgbox.hxx>
#endif
#ifndef _SV_BUTTON_HXX
#include <button.hxx>
#endif
#ifndef _SV_RC_H
#include <tools/rc.h>
#endif
#ifndef _SV_MNEMONIC_HXX
#include <mnemonic.hxx>
#endif
#ifndef _SV_WINDOW_H
#include <window.h>
#endif



// =======================================================================

static void ImplInitMsgBoxImageList()
{
    ImplSVData* pSVData = ImplGetSVData();
    if ( !pSVData->maWinData.mpMsgBoxImgList )
    {
        BitmapEx aBmpEx;
        ResMgr* pResMgr = ImplGetResMgr();
        if( pResMgr )
        {
            aBmpEx = BitmapEx( ResId( SV_RESID_BITMAP_MSGBOX, ImplGetResMgr() ) );

            if( !aBmpEx.IsTransparent() )
                aBmpEx = BitmapEx( aBmpEx.GetBitmap(), Color( 0xC0, 0xC0, 0xC0 ) );
        }

        pSVData->maWinData.mpMsgBoxImgList = new ImageList( aBmpEx, 4 );
    }
}

// =======================================================================

void MessBox::ImplInitData()
{
    mpFixedText         = NULL;
    mpFixedImage        = NULL;
    mnSoundType         = 0;
    mbHelpBtn           = FALSE;
    mbSound             = TRUE;
    mpCheckBox          = NULL;
    mbCheck             = FALSE;
}

// -----------------------------------------------------------------------

void MessBox::ImplInitButtons()
{
    WinBits nStyle = GetStyle();
    USHORT  nOKFlags = BUTTONDIALOG_OKBUTTON;
    USHORT  nCancelFlags = BUTTONDIALOG_CANCELBUTTON;
    USHORT  nRetryFlags = 0;
    USHORT  nYesFlags = 0;
    USHORT  nNoFlags = 0;

    if ( nStyle & WB_OK_CANCEL )
    {
        if ( nStyle & WB_DEF_CANCEL )
            nCancelFlags |= BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON;
        else // WB_DEF_OK
            nOKFlags |= BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON;

        AddButton( BUTTON_OK, BUTTONID_OK, nOKFlags );
        AddButton( BUTTON_CANCEL, BUTTONID_CANCEL, nCancelFlags );
    }
    else if ( nStyle & WB_YES_NO )
    {
        if ( nStyle & WB_DEF_YES )
            nYesFlags |= BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON;
        else // WB_DEF_NO
            nNoFlags |= BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON;
        nNoFlags |= BUTTONDIALOG_CANCELBUTTON;

        AddButton( BUTTON_YES, BUTTONID_YES, nYesFlags );
        AddButton( BUTTON_NO, BUTTONID_NO, nNoFlags );
    }
    else if ( nStyle & WB_YES_NO_CANCEL )
    {
        if ( nStyle & WB_DEF_YES )
            nYesFlags |= BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON;
        else if ( nStyle & WB_DEF_NO )
            nNoFlags |= BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON;
        else
            nCancelFlags |= BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON;

        AddButton( BUTTON_YES, BUTTONID_YES, nYesFlags );
        AddButton( BUTTON_NO, BUTTONID_NO, nNoFlags );
        AddButton( BUTTON_CANCEL, BUTTONID_CANCEL, nCancelFlags );
    }
    else if ( nStyle & WB_RETRY_CANCEL )
    {
        if ( nStyle & WB_DEF_CANCEL )
            nCancelFlags |= BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON;
        else // WB_DEF_RETRY
            nRetryFlags |= BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON;

        AddButton( BUTTON_RETRY, BUTTONID_RETRY, nRetryFlags );
        AddButton( BUTTON_CANCEL, BUTTONID_CANCEL, nCancelFlags );
    }
    else if ( nStyle & WB_ABORT_RETRY_IGNORE )
    {
        USHORT nAbortFlags = 0;
        USHORT nRetryFlags = 0;
        USHORT nIgnoreFlags = 0;

        if ( nStyle & WB_DEF_CANCEL )
            nAbortFlags |= BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON;
        else if ( nStyle & WB_DEF_RETRY )
            nRetryFlags |= BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON;
        else if ( nStyle & WB_DEF_IGNORE )
            nIgnoreFlags |= BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON;

        AddButton( BUTTON_ABORT, BUTTONID_CANCEL, nAbortFlags );
        AddButton( BUTTON_RETRY, BUTTONID_RETRY, nRetryFlags );
        AddButton( BUTTON_IGNORE, BUTTONID_IGNORE, nIgnoreFlags );
    }
    else if ( nStyle & WB_OK )
    {
        nOKFlags |= BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON;

        AddButton( BUTTON_OK, BUTTONID_OK, nOKFlags );
    }
}

// -----------------------------------------------------------------------

MessBox::MessBox( WindowType nType ) :
    ButtonDialog( WINDOW_MESSBOX )
{
    ImplInitData();
}

// -----------------------------------------------------------------------

MessBox::MessBox( Window* pParent, WinBits nStyle,
                  const XubString& rTitle, const XubString& rMessage ) :
    ButtonDialog( WINDOW_MESSBOX ),
    maMessText( rMessage )
{
    ImplInitData();
    ImplInit( pParent, nStyle | WB_MOVEABLE | WB_HORZ | WB_CENTER );
    ImplInitButtons();

    if ( rTitle.Len() )
        SetText( rTitle );
}

// -----------------------------------------------------------------------

MessBox::MessBox( Window* pParent, const ResId& rResId ) :
    ButtonDialog( WINDOW_MESSBOX )
{
    ImplInitData();

    GetRes( rResId.SetRT( RSC_MESSBOX ) );
    USHORT nHiButtons   = ReadShortRes();
    USHORT nLoButtons   = ReadShortRes();
    USHORT nHiDefButton = ReadShortRes();
    USHORT nLoDefButton = ReadShortRes();
    USHORT nHiHelpId    = ReadShortRes();
    USHORT nLoHelpId    = ReadShortRes();
    /* USHORT bSysModal = */ ReadShortRes();
    SetHelpId( ((ULONG)nHiHelpId << 16) + nLoHelpId );
    WinBits nBits = (((ULONG)nHiButtons << 16) + nLoButtons) |
                    (((ULONG)nHiDefButton << 16) + nLoDefButton);
    ImplInit( pParent, nBits | WB_MOVEABLE | WB_HORZ | WB_CENTER );

    ImplLoadRes( rResId );
    ImplInitButtons();
}

// -----------------------------------------------------------------------

void MessBox::ImplLoadRes( const ResId& rResId )
{
    SetText( ReadStringRes() );
    SetMessText( ReadStringRes() );
    SetHelpText( ReadStringRes() );
}

// -----------------------------------------------------------------------

MessBox::~MessBox()
{
    if ( mpFixedText )
        delete mpFixedText;
    if ( mpFixedImage )
        delete mpFixedImage;
    if ( mpCheckBox )
        delete mpCheckBox;
}

// -----------------------------------------------------------------------

void MessBox::ImplPosControls()
{
    if ( GetHelpId() )
    {
        if ( !mbHelpBtn )
        {
            AddButton( BUTTON_HELP, BUTTONID_HELP, BUTTONDIALOG_HELPBUTTON, 3 );
            mbHelpBtn = TRUE;
        }
    }
    else
    {
        if ( mbHelpBtn )
        {
            RemoveButton( BUTTONID_HELP );
            mbHelpBtn = FALSE;
        }
    }

    XubString       aMessText( maMessText );
    TextRectInfo    aTextInfo;
    Rectangle       aRect( 0, 0, 30000, 30000 );
    Rectangle       aFormatRect;
    Point           aTextPos( IMPL_DIALOG_OFFSET, IMPL_DIALOG_OFFSET+IMPL_MSGBOX_OFFSET_EXTRA_Y );
    Size            aImageSize;
    Size            aPageSize;
    Size            aFixedSize;
    long            nTitleWidth;
    long            nButtonSize = ImplGetButtonSize();
    long            nMaxWidth = GetDesktopRectPixel().GetWidth()-8;
    long            nMaxLineWidth;
    long            nWidth;
    WinBits         nWinStyle = WB_LEFT | WB_WORDBREAK | WB_NOLABEL | WB_INFO;
    USHORT          nTextStyle = TEXT_DRAW_MULTILINE | TEXT_DRAW_TOP | TEXT_DRAW_LEFT;

    if ( mpFixedText )
        delete mpFixedText;
    if ( mpFixedImage )
    {
        delete mpFixedImage;
        mpFixedImage = NULL;
    }
    if ( mpCheckBox )
    {
        mbCheck = mpCheckBox->IsChecked();
        delete mpCheckBox;
        mpCheckBox = NULL;
    }


    // Message-Text um Tabs bereinigen
    XubString   aTabStr( RTL_CONSTASCII_USTRINGPARAM( "    " ) );
    USHORT      nIndex = 0;
    while ( nIndex != STRING_NOTFOUND )
        nIndex = aMessText.SearchAndReplace( '\t', aTabStr, nIndex );

    // Wenn Fenster zu schmall, machen wir Dialog auch breiter
    if ( mpWindowImpl->mbFrame )
        nMaxWidth = 630;
    else if ( nMaxWidth < 120 )
        nMaxWidth = 120;

    nMaxWidth -= mpWindowImpl->mnLeftBorder+mpWindowImpl->mnRightBorder+4;

    // MessageBox sollte min. so breit sein, das auch Title sichtbar ist
    // Extra-Width for Closer, because Closer is set after this call
    nTitleWidth = CalcTitleWidth();
    nTitleWidth += mpWindowImpl->mnTopBorder;

    nMaxWidth -= (IMPL_DIALOG_OFFSET*2)+(IMPL_MSGBOX_OFFSET_EXTRA_X*2);

    // Wenn wir ein Image haben, dann deren Groesse ermitteln und das
    // entsprechende Control anlegen und positionieren
    aImageSize = maImage.GetSizePixel();
    if ( aImageSize.Width() )
    {
        aImageSize.Width()  += 4;
        aImageSize.Height() += 4;
        aTextPos.X() += aImageSize.Width()+IMPL_SEP_MSGBOX_IMAGE;
        mpFixedImage = new FixedImage( this );
        mpFixedImage->SetPosSizePixel( Point( IMPL_DIALOG_OFFSET-2+IMPL_MSGBOX_OFFSET_EXTRA_X,
                                              IMPL_DIALOG_OFFSET-2+IMPL_MSGBOX_OFFSET_EXTRA_Y ),
                                       aImageSize );
        mpFixedImage->SetImage( maImage );
        // forward the HC image
        if( !!maImageHC )
            mpFixedImage->SetModeImage( maImageHC, BMP_COLOR_HIGHCONTRAST );
        mpFixedImage->Show();
        nMaxWidth -= aImageSize.Width()+IMPL_SEP_MSGBOX_IMAGE;
    }
    else
        aTextPos.X() += IMPL_MSGBOX_OFFSET_EXTRA_X;

    // Maximale Zeilenlaenge ohne Wordbreak ermitteln
    aFormatRect = GetTextRect( aRect, aMessText, nTextStyle, &aTextInfo );
    nMaxLineWidth = aFormatRect.GetWidth();
    nTextStyle |= TEXT_DRAW_WORDBREAK;

    // Breite fuer Textformatierung ermitteln
    if ( nMaxLineWidth > 450 )
        nWidth = 450;
    else if ( nMaxLineWidth > 300 )
        nWidth = nMaxLineWidth+5;
    else
        nWidth = 300;
    if ( nButtonSize > nWidth )
        nWidth = nButtonSize-(aTextPos.X()-IMPL_DIALOG_OFFSET);
    if ( nWidth > nMaxWidth )
        nWidth = nMaxWidth;

    aRect.Right() = nWidth;
    aFormatRect = GetTextRect( aRect, aMessText, nTextStyle, &aTextInfo );
    if ( aTextInfo.GetMaxLineWidth() > nWidth )
    {
        nWidth = aTextInfo.GetMaxLineWidth()+8;
        aRect.Right() = nWidth;
        aFormatRect = GetTextRect( aRect, aMessText, nTextStyle, &aTextInfo );
    }

    // Style fuer FixedText ermitteln
    aPageSize.Width()   = aImageSize.Width();
    aFixedSize.Width()  = aTextInfo.GetMaxLineWidth()+1;
    aFixedSize.Height() = aFormatRect.GetHeight();
    if ( aFixedSize.Height() < aImageSize.Height() )
    {
        nWinStyle |= WB_VCENTER;
        aPageSize.Height()  = aImageSize.Height();
        aFixedSize.Height() = aImageSize.Height();
    }
    else
    {
        nWinStyle |= WB_TOP;
        aPageSize.Height()  = aFixedSize.Height();
    }
    if ( aImageSize.Width() )
        aPageSize.Width() += IMPL_SEP_MSGBOX_IMAGE;
    aPageSize.Width()  += (IMPL_DIALOG_OFFSET*2)+(IMPL_MSGBOX_OFFSET_EXTRA_X*2);
    aPageSize.Width()  += aFixedSize.Width()+1;
    aPageSize.Height() += (IMPL_DIALOG_OFFSET*2)+(IMPL_MSGBOX_OFFSET_EXTRA_Y*2);

    if ( aPageSize.Width() < IMPL_MINSIZE_MSGBOX_WIDTH )
        aPageSize.Width() = IMPL_MINSIZE_MSGBOX_WIDTH;
    if ( aPageSize.Width() < nTitleWidth )
        aPageSize.Width() = nTitleWidth;

    if ( maCheckBoxText.Len() )
    {
        Size aMinCheckboxSize ( aFixedSize );
        if ( aPageSize.Width() < IMPL_MINSIZE_MSGBOX_WIDTH+80 )
        {
            aPageSize.Width() = IMPL_MINSIZE_MSGBOX_WIDTH+80;
            aMinCheckboxSize.Width() += 80;
        }

        // #104492# auto mnemonics for CJK strings may increase the length, so measure the
        // checkbox length including a temporary mnemonic, the correct auto mnemonic will be
        // generated later in the dialog (see init_show)

        String aMnemonicString( maCheckBoxText );
        if( GetSettings().GetStyleSettings().GetAutoMnemonic() )
        {
            if( aMnemonicString == GetNonMnemonicString( maCheckBoxText ) )
            {
                // no mnemonic found -> create one
                MnemonicGenerator aMnemonicGenerator;
                aMnemonicGenerator.CreateMnemonic( aMnemonicString );
            }
        }

        mpCheckBox = new CheckBox( this );
        mpCheckBox->Check( mbCheck );
        mpCheckBox->SetText( aMnemonicString );
        mpCheckBox->SetStyle( mpCheckBox->GetStyle() | WB_WORDBREAK );
        mpCheckBox->SetHelpId( GetHelpId() );   // DR: Check box and dialog have same HID

        // align checkbox with message text
        Size aSize = mpCheckBox->CalcMinimumSize( aMinCheckboxSize.Width() );

        // now set the original non-mnemonic string
        mpCheckBox->SetText( maCheckBoxText );

        Point aPos( aTextPos );
        aPos.Y() += aFixedSize.Height() + (IMPL_DIALOG_OFFSET)+(IMPL_MSGBOX_OFFSET_EXTRA_Y*2);

        // increase messagebox
        aPageSize.Height() += aSize.Height() + (IMPL_DIALOG_OFFSET*2)+(IMPL_MSGBOX_OFFSET_EXTRA_Y*2);

        mpCheckBox->SetPosSizePixel( aPos, aSize );
        mpCheckBox->Show();
    }

    mpFixedText = new FixedText( this, nWinStyle );
    mpFixedText->SetPosSizePixel( aTextPos, aFixedSize );
    mpFixedText->SetText( aMessText );
    mpFixedText->Show();
    SetPageSizePixel( aPageSize );
}

// -----------------------------------------------------------------------

void MessBox::StateChanged( StateChangedType nType )
{
    if ( nType == STATE_CHANGE_INITSHOW )
    {
        ImplPosControls();
        if ( mbSound && mnSoundType )
            Sound::Beep( (SoundType)(mnSoundType-1), this );
    }
    ButtonDialog::StateChanged( nType );
}

// -----------------------------------------------------------------------

BOOL MessBox::GetCheckBoxState() const
{
    return mpCheckBox ? mpCheckBox->IsChecked() : mbCheck;
}

// -----------------------------------------------------------------------

void MessBox::SetCheckBoxState( BOOL bCheck )
{
    if( mpCheckBox ) mpCheckBox->Check( bCheck );
    mbCheck = bCheck;
}

// -----------------------------------------------------------------------

void MessBox::SetDefaultCheckBoxText()
{
    ResMgr* pResMgr = ImplGetResMgr();
    if( pResMgr )
        maCheckBoxText = XubString( ResId( SV_STDTEXT_DONTHINTAGAIN, pResMgr ) );
}

// -----------------------------------------------------------------------

BOOL MessBox::SetModeImage( const Image& rImage, BmpColorMode eMode )
{
    if( eMode == BMP_COLOR_NORMAL )
        SetImage( rImage );
    else if( eMode == BMP_COLOR_HIGHCONTRAST )
        maImageHC = rImage;
    else
        return FALSE;
    return TRUE;
}

// -----------------------------------------------------------------------

const Image& MessBox::GetModeImage( BmpColorMode eMode ) const
{
    if( eMode == BMP_COLOR_HIGHCONTRAST )
        return maImageHC;
    else
        return maImage;
}

// -----------------------------------------------------------------------

void InfoBox::ImplInitData()
{
    // Default Text is the display title from the application
    if ( !GetText().Len() )
        SetText( Application::GetDisplayName() );

    SetImage( InfoBox::GetStandardImage() );
    mnSoundType = ((USHORT)SOUND_INFO)+1;
}

// -----------------------------------------------------------------------

InfoBox::InfoBox( Window* pParent, const XubString& rMessage ) :
    MessBox( pParent, WB_OK | WB_DEF_OK, ImplGetSVEmptyStr(), rMessage )
{
    ImplInitData();
}

// -----------------------------------------------------------------------

InfoBox::InfoBox( Window* pParent, const ResId & rResId ) :
    MessBox( pParent, rResId.SetRT( RSC_INFOBOX ) )
{
    ImplInitData();
}

// -----------------------------------------------------------------------

Image InfoBox::GetStandardImage()
{
    ImplInitMsgBoxImageList();
    return ImplGetSVData()->maWinData.mpMsgBoxImgList->GetImage( 4 );
}

// -----------------------------------------------------------------------

void WarningBox::ImplInitData()
{
    // Default Text is the display title from the application
    if ( !GetText().Len() )
        SetText( Application::GetDisplayName() );

    SetImage( WarningBox::GetStandardImage() );
    mnSoundType = ((USHORT)SOUND_WARNING)+1;
}

// -----------------------------------------------------------------------

WarningBox::WarningBox( Window* pParent, WinBits nStyle,
                        const XubString& rMessage ) :
    MessBox( pParent, nStyle, ImplGetSVEmptyStr(), rMessage )
{
    ImplInitData();
}

// -----------------------------------------------------------------------

WarningBox::WarningBox( Window* pParent, const ResId& rResId ) :
    MessBox( pParent, rResId.SetRT( RSC_WARNINGBOX ) )
{
    ImplInitData();
}

// -----------------------------------------------------------------------

void WarningBox::SetDefaultCheckBoxText()
{
    ResMgr* pResMgr = ImplGetResMgr();
    if( pResMgr )
        maCheckBoxText = XubString( ResId( SV_STDTEXT_DONTWARNAGAIN, pResMgr ) );
}

// -----------------------------------------------------------------------

Image WarningBox::GetStandardImage()
{
    ImplInitMsgBoxImageList();
    return ImplGetSVData()->maWinData.mpMsgBoxImgList->GetImage( 3 );
}

// -----------------------------------------------------------------------

void ErrorBox::ImplInitData()
{
    // Default Text is the display title from the application
    if ( !GetText().Len() )
        SetText( Application::GetDisplayName() );

    SetImage( ErrorBox::GetStandardImage() );
    mnSoundType = ((USHORT)SOUND_ERROR)+1;
}

// -----------------------------------------------------------------------

ErrorBox::ErrorBox( Window* pParent, WinBits nStyle,
                    const XubString& rMessage ) :
    MessBox( pParent, nStyle, ImplGetSVEmptyStr(), rMessage )
{
    ImplInitData();
}

// -----------------------------------------------------------------------

ErrorBox::ErrorBox( Window* pParent, const ResId& rResId ) :
    MessBox( pParent, rResId.SetRT( RSC_ERRORBOX ) )
{
    ImplInitData();
}

// -----------------------------------------------------------------------

Image ErrorBox::GetStandardImage()
{
    ImplInitMsgBoxImageList();
    return ImplGetSVData()->maWinData.mpMsgBoxImgList->GetImage( 1 );
}

// -----------------------------------------------------------------------

void QueryBox::ImplInitData()
{
    // Default Text is the display title from the application
    if ( !GetText().Len() )
        SetText( Application::GetDisplayName() );

    SetImage( QueryBox::GetStandardImage() );
    mnSoundType = ((USHORT)SOUND_QUERY)+1;
}

// -----------------------------------------------------------------------

QueryBox::QueryBox( Window* pParent, WinBits nStyle, const XubString& rMessage ) :
    MessBox( pParent, nStyle, ImplGetSVEmptyStr(), rMessage )
{
    ImplInitData();
}

// -----------------------------------------------------------------------

QueryBox::QueryBox( Window* pParent, const ResId& rResId ) :
    MessBox( pParent, rResId.SetRT( RSC_QUERYBOX ) )
{
    ImplInitData();
}

// -----------------------------------------------------------------------

void QueryBox::SetDefaultCheckBoxText()
{
    ResMgr* pResMgr = ImplGetResMgr();
    if( pResMgr )
        maCheckBoxText = XubString( ResId( SV_STDTEXT_DONTASKAGAIN, pResMgr ) );
}

// -----------------------------------------------------------------------

Image QueryBox::GetStandardImage()
{
    ImplInitMsgBoxImageList();
    return ImplGetSVData()->maWinData.mpMsgBoxImgList->GetImage( 2 );
}
