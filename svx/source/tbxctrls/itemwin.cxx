/*************************************************************************
 *
 *  $RCSfile: itemwin.cxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: vg $ $Date: 2003-05-22 10:27:45 $
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
 *  WITHOUT WARRUNTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRUNTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
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

#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFX_OBJSH_HXX //autogen
#include <sfx2/objsh.hxx>
#endif
#ifndef _SFXVIEWSH_HXX
#include <sfx2/viewsh.hxx>
#endif
#ifndef _SFXMODULE_HXX
#include <sfx2/module.hxx>
#endif
#pragma hdrstop

#define _SVX_ITEMWIN_CXX

#include "dialogs.hrc"

#define ITEMID_COLOR_TABLE      SID_COLOR_TABLE
#define ITEMID_DASH_LIST        SID_DASH_LIST

#define DELAY_TIMEOUT           100

#include "xlnclit.hxx"
#include "xlnwtit.hxx"
#include "xlineit0.hxx"
#include "xlndsit.hxx"
#include "xtable.hxx"
#include "drawitem.hxx"
#include "dialmgr.hxx"
#include "dlgutil.hxx"
#include "itemwin.hxx"
#include "linectrl.hxx"
#ifndef INCLUDED_SVTOOLS_COLORCFG_HXX
#include <svtools/colorcfg.hxx>
#endif

#include "linectrl.hrc"

#define LOGICAL_EDIT_HEIGHT         12
//========================================================================
// SvxLineBox
//========================================================================

SvxLineBox::SvxLineBox( Window* pParent, SfxBindings& rBind, WinBits nBits ) :

    LineLB( pParent, nBits ),
    aLogicalSize(40,140),
    nCurPos     ( 0 ),
    rBindings   ( rBind ),
    bRelease    ( TRUE ),
    mpSh        ( NULL ),
    meBmpMode   ( GetDisplayBackground().GetColor().IsDark() ? BMP_COLOR_HIGHCONTRAST : BMP_COLOR_NORMAL )
{
    SetSizePixel( LogicToPixel( aLogicalSize, MAP_APPFONT ));
    Show();

    aDelayTimer.SetTimeout( DELAY_TIMEOUT );
    aDelayTimer.SetTimeoutHdl( LINK( this, SvxLineBox, DelayHdl_Impl ) );
    aDelayTimer.Start();
}

// -----------------------------------------------------------------------

SvxLineBox::~SvxLineBox()
{
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxLineBox, DelayHdl_Impl, Timer *, pTimer )
{
    if ( GetEntryCount() == 0 )
    {
        mpSh = SfxObjectShell::Current();
        FillControl();
    }
    return 0;
}

// -----------------------------------------------------------------------

void SvxLineBox::Select()
{
    // Call the parent's Select() member to trigger accessibility events.
    LineLB::Select();

    if ( !IsTravelSelect() )
    {
        XLineStyle eXLS;
        USHORT nPos = GetSelectEntryPos();
        SfxDispatcher* pDisp = rBindings.GetDispatcher();
        DBG_ASSERT( pDisp, "invalid Dispatcher" );

        switch ( nPos )
        {
            case 0:
                eXLS = XLINE_NONE;
                break;

            case 1:
                eXLS = XLINE_SOLID;
                break;

            default:
            {
                eXLS = XLINE_DASH;

                if ( nPos != LISTBOX_ENTRY_NOTFOUND &&
                     SfxObjectShell::Current()  &&
                     SfxObjectShell::Current()->GetItem( SID_DASH_LIST ) )
                {
                    // LineDashItem wird nur geschickt, wenn es auch einen Dash besitzt.
                    // Notify k"ummert sich darum!
                    SvxDashListItem aItem( *(const SvxDashListItem*)(
                        SfxObjectShell::Current()->GetItem( SID_DASH_LIST ) ) );
                    XLineDashItem aLineDashItem( GetSelectEntry(),
                        aItem.GetDashList()->Get( nPos - 2 )->GetDash() );
                    pDisp->Execute( SID_ATTR_LINE_DASH, SFX_CALLMODE_RECORD, &aLineDashItem, 0L );
                }
            }
            break;
        }

        XLineStyleItem aLineStyleItem( eXLS );
        pDisp->Execute( SID_ATTR_LINE_STYLE, SFX_CALLMODE_RECORD, &aLineStyleItem, 0L );
        ReleaseFocus_Impl();
    }
}

// -----------------------------------------------------------------------

long SvxLineBox::PreNotify( NotifyEvent& rNEvt )
{
    USHORT nType = rNEvt.GetType();

    switch(nType)
    {
        case EVENT_MOUSEBUTTONDOWN:
        case EVENT_GETFOCUS:
            nCurPos = GetSelectEntryPos();
        break;
        case EVENT_LOSEFOCUS:
            SelectEntryPos(nCurPos);
        break;
        case EVENT_KEYINPUT:
        {
            const KeyEvent* pKEvt = rNEvt.GetKeyEvent();
            if( pKEvt->GetKeyCode().GetCode() == KEY_TAB)
            {
                bRelease = FALSE;
                Select();
            }
        }
        break;
    }
    return LineLB::PreNotify( rNEvt );
}

// -----------------------------------------------------------------------

long SvxLineBox::Notify( NotifyEvent& rNEvt )
{
    long nHandled = LineLB::Notify( rNEvt );

    if ( rNEvt.GetType() == EVENT_KEYINPUT )
    {
        const KeyEvent* pKEvt = rNEvt.GetKeyEvent();

        switch ( pKEvt->GetKeyCode().GetCode() )
        {
            case KEY_RETURN:
                Select();
                nHandled = 1;
                break;

            case KEY_ESCAPE:
                SelectEntryPos( nCurPos );
                ReleaseFocus_Impl();
                nHandled = 1;
                break;
        }
    }
    return nHandled;
}

// -----------------------------------------------------------------------

void SvxLineBox::ReleaseFocus_Impl()
{
    if(!bRelease)
    {
        bRelease = TRUE;
        return;
    }
    Window* pShellWnd = SfxViewShell::Current()->GetWindow();

    if ( pShellWnd )
        pShellWnd->GrabFocus();
}
/* -----------------------------08.03.2002 15:39------------------------------

 ---------------------------------------------------------------------------*/
void SvxLineBox::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        SetSizePixel(LogicToPixel(aLogicalSize, MAP_APPFONT));
        Size aDropSize( aLogicalSize.Width(), LOGICAL_EDIT_HEIGHT);
        SetDropDownSizePixel(LogicToPixel(aDropSize, MAP_APPFONT));
   }

    LineLB::DataChanged( rDCEvt );

    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        BmpColorMode eMode = GetDisplayBackground().GetColor().IsDark() ? BMP_COLOR_HIGHCONTRAST : BMP_COLOR_NORMAL;
        if( eMode != meBmpMode )
        {
            meBmpMode = eMode;
            FillControl();
        }
     }
}

void SvxLineBox::FillControl()
{
    Clear();

    InsertEntry( SVX_RESSTR(RID_SVXSTR_INVISIBLE) );

    Bitmap aBitmap ( SVX_RES ( RID_SVXCTRL_LINECTRL ) );

    Color aSourceColors[2];
    Color aDestColors[2];

    aSourceColors[0] = Color( COL_WHITE );
    aSourceColors[1] = Color( COL_BLACK );

    const StyleSettings& rStyles = Application::GetSettings().GetStyleSettings();
    aDestColors[0] = rStyles.GetFieldColor();
    aDestColors[1] = rStyles.GetFieldTextColor();

    aBitmap.Replace ( aSourceColors, aDestColors, 2 );
    Image aSolidLine ( aBitmap );
    InsertEntry( SVX_RESSTR(RID_SVXSTR_SOLID), aSolidLine );

    if( mpSh )
    {
        const SvxDashListItem* pItem = (const SvxDashListItem*)( mpSh->GetItem( SID_DASH_LIST ) );
        if ( pItem )
            Fill( pItem->GetDashList() );
    }

    rBindings.Invalidate( SID_ATTR_LINE_DASH );
}
//========================================================================
// SvxColorBox
//========================================================================

SvxColorBox::SvxColorBox( Window* pParent, USHORT nSID, SfxBindings& rBind, WinBits nBits ) :

    ColorLB( pParent, nBits ),
    aLogicalSize(45,80),
    nCurPos     ( 0 ),
    nId         ( nSID ),
    rBindings   ( rBind ),
    bRelease    ( TRUE )

{
    SetSizePixel( LogicToPixel( aLogicalSize , MAP_APPFONT));
    Show();

    SfxObjectShell* pSh = SfxObjectShell::Current();

    if ( pSh )
    {
        const SvxColorTableItem* pItem =
            (const SvxColorTableItem*)( pSh->GetItem( SID_COLOR_TABLE ) );
        if(pItem)
            Fill( pItem->GetColorTable() );
    }
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxColorBox, DelayHdl_Impl, Timer *, pTimer )
{
    SfxObjectShell* pSh = SfxObjectShell::Current();

    if ( pSh )
    {
        const SvxColorTableItem* pItem = (const SvxColorTableItem*)( pSh->GetItem( SID_COLOR_TABLE ) );
        if ( pItem )
            Fill( pItem->GetColorTable() );
        rBindings.Invalidate( nId );
    }
    return 0;
}

// -----------------------------------------------------------------------

SvxColorBox::~SvxColorBox()
{
}

// -----------------------------------------------------------------------

void SvxColorBox::Update( const XLineColorItem* pItem )
{
    if ( pItem )
        SelectEntry( pItem->GetValue() );
    else
        SetNoSelection();
}

// -----------------------------------------------------------------------

void SvxColorBox::Select()
{
    // OJ: base class call needed here because otherwise no event is send for accessibility
    ColorLB::Select();
    if ( !IsTravelSelect() )
    {
        XLineColorItem aLineColorItem( GetSelectEntry(), GetSelectEntryColor() );
        rBindings.GetDispatcher()->Execute( nId, SFX_CALLMODE_RECORD, &aLineColorItem, 0L );
        nCurPos = GetSelectEntryPos();
        ReleaseFocus_Impl();
    }
}

// -----------------------------------------------------------------------

long SvxColorBox::PreNotify( NotifyEvent& rNEvt )
{
    USHORT nType = rNEvt.GetType();

    switch(nType)
    {
        case  EVENT_MOUSEBUTTONDOWN:
        case EVENT_GETFOCUS:
            nCurPos = GetSelectEntryPos();
        break;
        case EVENT_LOSEFOCUS:
            SelectEntryPos(nCurPos);
        break;
        case EVENT_KEYINPUT:
        {
            const KeyEvent* pKEvt = rNEvt.GetKeyEvent();

            if( pKEvt->GetKeyCode().GetCode() == KEY_TAB)
            {
                bRelease = FALSE;
                Select();
            }
        }
    }

    return ColorLB::PreNotify( rNEvt );
}

// -----------------------------------------------------------------------

long SvxColorBox::Notify( NotifyEvent& rNEvt )
{
    long nHandled = ColorLB::Notify( rNEvt );

    if ( rNEvt.GetType() == EVENT_KEYINPUT )
    {
        const KeyEvent* pKEvt = rNEvt.GetKeyEvent();

        switch ( pKEvt->GetKeyCode().GetCode() )
        {
            case KEY_RETURN:
                Select();
                nHandled = 1;
                break;

            case KEY_ESCAPE:
                SelectEntryPos( nCurPos );
                ReleaseFocus_Impl();
                nHandled = 1;
                break;
        }
    }
    return nHandled;
}
/* -----------------------------08.03.2002 15:35------------------------------

 ---------------------------------------------------------------------------*/
void SvxColorBox::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        SetSizePixel(LogicToPixel(aLogicalSize, MAP_APPFONT));
        Size aDropSize( aLogicalSize.Width(), LOGICAL_EDIT_HEIGHT);
        SetDropDownSizePixel(LogicToPixel(aDropSize, MAP_APPFONT));
    }

    ColorLB::DataChanged( rDCEvt );
}
// -----------------------------------------------------------------------

void SvxColorBox::ReleaseFocus_Impl()
{
    if(!bRelease)
    {
        bRelease = TRUE;
        return;
    }
    Window* pShellWnd = SfxViewShell::Current()->GetWindow();

    if ( pShellWnd )
        pShellWnd->GrabFocus();
}

//========================================================================
// SvxMetricField
//========================================================================

SvxMetricField::SvxMetricField( Window* pParent, SfxBindings& rBind, WinBits nBits ) :

    MetricField( pParent, nBits ),
    aCurTxt     ( String() ),
    rBindings   ( rBind )

{
    Size aSize = Size(GetTextWidth( String::CreateFromAscii("99,99mm") ),GetTextHeight());
    aSize.Width() += 20;
#ifndef MAC
    aSize.Height() += 6;
#else
    aSize.Height() += 5;
#endif
    SetSizePixel( aSize );
    aLogicalSize = PixelToLogic(aSize, MAP_APPFONT);
    SetUnit( FUNIT_MM );
    SetDecimalDigits( 2 );
    SetMax( 5000 );
    SetMin( 0 );
    SetLast( 5000 );
    SetFirst( 0 );

    eDlgUnit = GetModuleFieldUnit( NULL );
    SetFieldUnit( *this, eDlgUnit, FALSE );
    Show();
}

// -----------------------------------------------------------------------

SvxMetricField::~SvxMetricField()
{
}

// -----------------------------------------------------------------------

void SvxMetricField::Update( const XLineWidthItem* pItem )
{
    if ( pItem )
    {
        if ( pItem->GetValue() != GetCoreValue( *this, ePoolUnit ) )
            SetMetricValue( *this, pItem->GetValue(), ePoolUnit );
    }
    else
        SetText( String() );
}

// -----------------------------------------------------------------------

void SvxMetricField::Modify()
{
    MetricField::Modify();
    long nTmp = GetCoreValue( *this, ePoolUnit );
    XLineWidthItem aLineWidthItem( nTmp );
    rBindings.GetDispatcher()->Execute( SID_ATTR_LINE_WIDTH, SFX_CALLMODE_RECORD, &aLineWidthItem, 0L );
}

// -----------------------------------------------------------------------

void SvxMetricField::ReleaseFocus_Impl()
{
    Window* pShellWnd = SfxViewShell::Current()->GetWindow();
    if ( pShellWnd )
        pShellWnd->GrabFocus();
}

// -----------------------------------------------------------------------

void SvxMetricField::Down()
{
    long nValue = GetValue();
    nValue -= GetSpinSize();

    // Um unter OS/2 einen Sprung auf Max zu verhindern
    if ( nValue >= GetMin() )
        MetricField::Down();
}

// -----------------------------------------------------------------------

void SvxMetricField::Up()
{
    MetricField::Up();
}

// -----------------------------------------------------------------------

void SvxMetricField::SetCoreUnit( SfxMapUnit eUnit )
{
    ePoolUnit = eUnit;
}

// -----------------------------------------------------------------------

void SvxMetricField::RefreshDlgUnit()
{
    FieldUnit eTmpUnit = GetModuleFieldUnit( NULL );

    if ( eDlgUnit != eTmpUnit )
    {
        eDlgUnit = eTmpUnit;
        SetFieldUnit( *this, eDlgUnit, FALSE );
    }
}

// -----------------------------------------------------------------------

long SvxMetricField::PreNotify( NotifyEvent& rNEvt )
{
    USHORT nType = rNEvt.GetType();

    if ( EVENT_MOUSEBUTTONDOWN == nType || EVENT_GETFOCUS == nType )
        aCurTxt = GetText();

    return MetricField::PreNotify( rNEvt );
}

// -----------------------------------------------------------------------

long SvxMetricField::Notify( NotifyEvent& rNEvt )
{
    long nHandled = MetricField::Notify( rNEvt );

    if ( rNEvt.GetType() == EVENT_KEYINPUT )
    {
        const KeyEvent* pKEvt = rNEvt.GetKeyEvent();
        const KeyCode& rKey = pKEvt->GetKeyCode();
        SfxViewShell* pSh = SfxViewShell::Current();

        if ( rKey.GetModifier() && rKey.GetGroup() != KEYGROUP_CURSOR && pSh )
            pSh->KeyInput( *pKEvt );
        else
        {
            FASTBOOL bHandled = FALSE;

            switch ( rKey.GetCode() )
            {
                case KEY_RETURN:
                    Reformat();
                    bHandled = TRUE;
                    break;

                case KEY_ESCAPE:
                    SetText( aCurTxt );
                    bHandled = TRUE;
                    break;
            }

            if ( bHandled )
            {
                nHandled = 1;
                Modify();
                ReleaseFocus_Impl();
            }
        }
    }
    return nHandled;
}
/* -----------------------------08.03.2002 15:32------------------------------

 ---------------------------------------------------------------------------*/
void SvxMetricField::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        SetSizePixel(LogicToPixel(aLogicalSize, MAP_APPFONT));
    }

    MetricField::DataChanged( rDCEvt );
}

//========================================================================
// SvxFillTypeBox
//========================================================================

SvxFillTypeBox::SvxFillTypeBox( Window* pParent, WinBits nBits ) :

    FillTypeLB( pParent, nBits | WB_TABSTOP ),

    nCurPos ( 0 ),
    bSelect ( FALSE ),
    bRelease(TRUE)

{
    SetSizePixel( LogicToPixel( Size(40, 40 ),MAP_APPFONT ));
    Fill();
    SelectEntryPos( XFILL_SOLID );
    Show();
}

// -----------------------------------------------------------------------

SvxFillTypeBox::~SvxFillTypeBox()
{
}

// -----------------------------------------------------------------------

long SvxFillTypeBox::PreNotify( NotifyEvent& rNEvt )
{
    USHORT nType = rNEvt.GetType();

    if ( EVENT_MOUSEBUTTONDOWN == nType || EVENT_GETFOCUS == nType )
        nCurPos = GetSelectEntryPos();
    else if ( EVENT_LOSEFOCUS == nType
        && Application::GetFocusWindow()
        && !IsWindowOrChild( Application::GetFocusWindow(), TRUE ) )
    {
        if ( !bSelect )
            SelectEntryPos( nCurPos );
        else
            bSelect = FALSE;
    }

    return FillTypeLB::PreNotify( rNEvt );
}

// -----------------------------------------------------------------------

long SvxFillTypeBox::Notify( NotifyEvent& rNEvt )
{
    long nHandled = FillTypeLB::Notify( rNEvt );

    if ( rNEvt.GetType() == EVENT_KEYINPUT )
    {
        const KeyEvent* pKEvt = rNEvt.GetKeyEvent();
        switch ( pKEvt->GetKeyCode().GetCode() )
        {
            case KEY_RETURN:
                nHandled = 1;
                ( (Link&)GetSelectHdl() ).Call( this );
            break;
            case KEY_TAB:
                bRelease = FALSE;
                ( (Link&)GetSelectHdl() ).Call( this );
                bRelease = TRUE;
                break;

            case KEY_ESCAPE:
                SelectEntryPos( nCurPos );
                ReleaseFocus_Impl();
                nHandled = 1;
                break;
        }
    }
    return nHandled;
}

// -----------------------------------------------------------------------

void SvxFillTypeBox::ReleaseFocus_Impl()
{
    Window* pShellWnd = SfxViewShell::Current()->GetWindow();

    if ( pShellWnd )
        pShellWnd->GrabFocus();
}

//========================================================================
// SvxFillAttrBox
//========================================================================

SvxFillAttrBox::SvxFillAttrBox( Window* pParent, WinBits nBits ) :

    FillAttrLB( pParent, nBits | WB_TABSTOP ),

    nCurPos( 0 ),
    bRelease( TRUE )

{
    SetPosPixel( Point( 90, 0 ) );
    SetSizePixel( LogicToPixel( Size(50, 80 ), MAP_APPFONT ));
    Show();
}

// -----------------------------------------------------------------------

SvxFillAttrBox::~SvxFillAttrBox()
{
}

// -----------------------------------------------------------------------

long SvxFillAttrBox::PreNotify( NotifyEvent& rNEvt )
{
    USHORT nType = rNEvt.GetType();

    if ( EVENT_MOUSEBUTTONDOWN == nType || EVENT_GETFOCUS == nType )
        nCurPos = GetSelectEntryPos();

    return FillAttrLB::PreNotify( rNEvt );
}

// -----------------------------------------------------------------------

long SvxFillAttrBox::Notify( NotifyEvent& rNEvt )
{
    long nHandled = FillAttrLB::Notify( rNEvt );

    if ( rNEvt.GetType() == EVENT_KEYINPUT )
    {
        const KeyEvent* pKEvt = rNEvt.GetKeyEvent();

        switch ( pKEvt->GetKeyCode().GetCode() )
        {
            case KEY_RETURN:
                ( (Link&)GetSelectHdl() ).Call( this );
                nHandled = 1;
            break;
            case KEY_TAB:
                bRelease = FALSE;
                GetSelectHdl().Call( this );
                bRelease = TRUE;
            break;
            case KEY_ESCAPE:
                SelectEntryPos( nCurPos );
                ReleaseFocus_Impl();
                nHandled = 1;
                break;
        }
    }
    return nHandled;
}

// -----------------------------------------------------------------------

void SvxFillAttrBox::Select()
{
    FillAttrLB::Select();
}

// -----------------------------------------------------------------------

void SvxFillAttrBox::ReleaseFocus_Impl()
{
    Window* pShellWnd = SfxViewShell::Current()->GetWindow();

    if ( pShellWnd )
        pShellWnd->GrabFocus();
}

