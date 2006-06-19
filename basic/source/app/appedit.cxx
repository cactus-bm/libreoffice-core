/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: appedit.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 17:33:39 $
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

#ifndef _CONFIG_HXX
#include <tools/config.hxx>
#endif
#ifndef _CTRLTOOL_HXX
#include <svtools/ctrltool.hxx>
#endif
#ifndef _TEXTVIEW_HXX //autogen
#include <svtools/textview.hxx>
#endif
#ifndef _TEXTENG_HXX //autogen
#include <svtools/texteng.hxx>
#endif
#ifndef _UNDO_HXX
#include <svtools/undo.hxx>
#endif

#ifndef _BASIC_TTRESHLP_HXX
#include "ttstrhlp.hxx"
#endif

#include "basic.hrc"
#include "appedit.hxx"
#include "brkpnts.hxx"

TYPEINIT1(AppEdit,AppWin);
AppEdit::AppEdit( BasicFrame* pParent )
: AppWin( pParent )
, pVScroll( NULL )
, pHScroll( NULL )
, nCurTextWidth(5)
{
    String aEmpty;
    // evtl. den Untitled-String laden:

    pDataEdit = new TextEdit( this, WB_LEFT );
    LoadIniFile();
    // Icon definieren:
//  pIcon = new Icon( ResId( RID_WORKICON ) );
//  if( pIcon ) SetIcon( *pIcon );

    pDataEdit->SetText( aEmpty );

    pDataEdit->Show();

    pVScroll = new ScrollBar( this, WB_VSCROLL|WB_DRAG );
    pVScroll->Show();
    pVScroll->SetScrollHdl( LINK( this, AppEdit, Scroll ) );
    pHScroll = new ScrollBar( this, WB_HSCROLL|WB_DRAG );
    pHScroll->Show();
    pHScroll->SetScrollHdl( LINK( this, AppEdit, Scroll ) );

    InitScrollBars();
}

AppEdit::~AppEdit()
{
    DataEdit *pHold = pDataEdit;
    pDataEdit = NULL;       // Erst abklemmen, dann l�schen
    delete pHold;
    delete pHScroll;
    delete pVScroll;
}

void AppEdit::LoadIniFile()
{
    TextView *pTextView = ((TextEdit*)pDataEdit)->aEdit.pTextView;
    BOOL bWasModified = pTextView->GetTextEngine()->IsModified();
    pTextView->GetTextEngine()->SetModified( FALSE );

    FontList aFontList( pFrame );   // Just some Window is needed
    Config aConf(Config::GetConfigName( Config::GetDefDirectory(), CUniString("testtool") ));
    aConf.SetGroup("Misc");
    String aFontName = String( aConf.ReadKey( "ScriptFontName", "Courier" ), RTL_TEXTENCODING_UTF8 );
    String aFontStyle = String( aConf.ReadKey( "ScriptFontStyle", "normal" ), RTL_TEXTENCODING_UTF8 );
    String aFontSize = String( aConf.ReadKey( "ScriptFontSize", "12" ), RTL_TEXTENCODING_UTF8 );
    Font aFont = aFontList.Get( aFontName, aFontStyle );
//    ULONG nFontSize = aFontSize.GetValue( FUNIT_POINT );
    ULONG nFontSize = aFontSize.ToInt32();
//    aFont.SetSize( Size( nFontSize, nFontSize ) );
    aFont.SetHeight( nFontSize );

#if OSL_DEBUG_LEVEL > 1
    {
        Font aFont( OutputDevice::GetDefaultFont( DEFAULTFONT_FIXED, Application::GetSettings().GetUILanguage(), 0, pFrame ));
    }
#endif
    aFont.SetTransparent( FALSE );
//    aFont.SetAlign( ALIGN_BOTTOM );
//    aFont.SetHeight( aFont.GetHeight()+2 );
    pDataEdit->SetFont( aFont );

    if ( ((TextEdit*)pDataEdit)->GetBreakpointWindow() )
    {
        ((TextEdit*)pDataEdit)->GetBreakpointWindow()->SetFont( aFont );
        ((TextEdit*)pDataEdit)->GetBreakpointWindow()->Invalidate();
    }

    pTextView->GetTextEngine()->SetModified( bWasModified );    // Eventuell wieder setzen
}

void AppEdit::Command( const CommandEvent& rCEvt )
{
    switch( rCEvt.GetCommand() ) {
        case COMMAND_WHEEL:
            {
                HandleScrollCommand( rCEvt, pHScroll, pVScroll );
            }
            break;
        default:
            AppWin::Command( rCEvt );
    }
}


IMPL_LINK( AppEdit, Scroll, ScrollBar*, pScroll )
{
    (void) pScroll; /* avoid warning about unused parameter */
    if ( !pHScroll || !pVScroll )
        return 0;

    TextView *pTextView = ((TextEdit*)pDataEdit)->aEdit.pTextView;
    pTextView->SetStartDocPos( Point( pHScroll->GetThumbPos(), pVScroll->GetThumbPos() ) );
    pTextView->Invalidate();

    if ( ((TextEdit*)pDataEdit)->GetBreakpointWindow() )
        ((TextEdit*)pDataEdit)->GetBreakpointWindow()->Scroll( 0, ((TextEdit*)pDataEdit)->GetBreakpointWindow()->GetCurYOffset() - pTextView->GetStartDocPos().Y() );

    return 0L;
}


void AppEdit::InitScrollBars()
{
    if ( !pHScroll || !pVScroll )
        return;

    TextView *pTextView = ((TextEdit*)pDataEdit)->aEdit.pTextView;
// Kopiert und angepasst.
    SetScrollBarRanges();

    Size aOutSz( pTextView->GetWindow()->GetOutputSizePixel() );
    pVScroll->SetVisibleSize( aOutSz.Height() );
    pVScroll->SetPageSize( aOutSz.Height() * 8 / 10 );
    pVScroll->SetLineSize( GetTextHeight() +2 );    // +2 is empirical. don't know why
    pVScroll->SetThumbPos( pTextView->GetStartDocPos().Y() );
    pVScroll->Show();

    pHScroll->SetVisibleSize( aOutSz.Width() );
    pHScroll->SetPageSize( aOutSz.Width() * 8 / 10 );
    pHScroll->SetLineSize( GetTextWidth( CUniString("x") ) );
    pHScroll->SetThumbPos( pTextView->GetStartDocPos().X() );
    pHScroll->Show();
}

void AppEdit::SetScrollBarRanges()
{
    // Extra-Methode, nicht InitScrollBars, da auch fuer EditEngine-Events.

    if ( !pHScroll || !pVScroll )
        return;

    pHScroll->SetRange( Range( 0, nCurTextWidth ) );

    pVScroll->SetRange( Range( 0, ((TextEdit*)pDataEdit)->aEdit.pTextEngine->GetTextHeight() ) );
}



USHORT AppEdit::GetLineNr(){ return pDataEdit->GetLineNr(); }

FileType AppEdit::GetFileType()
{
    return FT_BASIC_SOURCE;
}

// Set up the menu
long AppEdit::InitMenu( Menu* pMenu )
{
    AppWin::InitMenu (pMenu );

    USHORT UndoCount = ((TextEdit*)pDataEdit)->aEdit.pTextEngine->GetUndoManager().GetUndoActionCount();
    USHORT RedoCount = ((TextEdit*)pDataEdit)->aEdit.pTextEngine->GetUndoManager().GetRedoActionCount();

    pMenu->EnableItem( RID_EDITUNDO,    UndoCount > 0 );
    pMenu->EnableItem( RID_EDITREDO,    RedoCount > 0 );

    return TRUE;
}

long AppEdit::DeInitMenu( Menu* pMenu )
{
    AppWin::DeInitMenu (pMenu );

    pMenu->EnableItem( RID_EDITUNDO );
    pMenu->EnableItem( RID_EDITREDO );

    return TRUE;
}

// Sourcecode-Datei laden

void AppEdit::Resize()
{
    if( !pDataEdit )
        return;

    Point rHStart,rVStart;
    Size rHSize,rVSize;
    Size rNewSize( GetOutputSizePixel() );

    if ( pHScroll )
    {
        rHSize = pHScroll->GetSizePixel();
        ULONG nHieght = rHSize.Height();
        rNewSize.Height() -= nHieght;
        rHStart.Y() = rNewSize.Height();
    }

    if ( pVScroll )
    {
        rVSize = pVScroll->GetSizePixel();
        ULONG nWidth = rVSize.Width();
        rNewSize.Width() -= nWidth;
        rVStart.X() = rNewSize.Width();
    }

    rHSize.Width() = rNewSize.Width();
    rVSize.Height() = rNewSize.Height();

    if ( pHScroll )
    {
        pHScroll->SetPosPixel( rHStart );
        pHScroll->SetSizePixel( rHSize );
    }

    if ( pVScroll )
    {
        pVScroll->SetPosPixel( rVStart );
        pVScroll->SetSizePixel( rVSize );
    }
    pDataEdit->SetPosPixel( Point() );
    pDataEdit->SetSizePixel( rNewSize );


    TextView *pTextView = ((TextEdit*)pDataEdit)->aEdit.pTextView;
// Kopiert und adaptiert
    long nVisY = pTextView->GetStartDocPos().Y();
    pTextView->ShowCursor();
    Size aOutSz( pTextView->GetWindow()->GetOutputSizePixel() );
    long nMaxVisAreaStart = pTextView->GetTextEngine()->GetTextHeight() - aOutSz.Height();
    if ( nMaxVisAreaStart < 0 )
        nMaxVisAreaStart = 0;
    if ( pTextView->GetStartDocPos().Y() > nMaxVisAreaStart )
    {
        Point aStartDocPos( pTextView->GetStartDocPos() );
        aStartDocPos.Y() = nMaxVisAreaStart;
        pTextView->SetStartDocPos( aStartDocPos );
        pTextView->ShowCursor();
//              pModulWindow->GetBreakPointWindow().GetCurYOffset() = aStartDocPos.Y();
    }
    InitScrollBars();
    if ( nVisY != pTextView->GetStartDocPos().Y() )
        pTextView->GetWindow()->Invalidate();

}

void AppEdit::PostLoad()
{
}

// mit neuem Namen speichern

void AppEdit::PostSaveAs()
{
}

void AppEdit::Highlight( USHORT nLine, USHORT nCol1, USHORT nCol2 )
{
    ((TextEdit*)pDataEdit)->Highlight( nLine, nCol1, nCol2 );
    ToTop();
}

