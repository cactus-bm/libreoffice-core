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
#include "precompiled_sc.hxx"



//------------------------------------------------------------------

#define _TPHFEDIT_CXX
#include "scitems.hxx"
#include <editeng/eeitem.hxx>


#include <editeng/editobj.hxx>
#include <editeng/editstat.hxx>
#include <editeng/editview.hxx>
#include <editeng/flditem.hxx>
#include <editeng/adjitem.hxx>
#include <sfx2/basedlgs.hxx>
#include <sfx2/objsh.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/svapp.hxx>

#include "tphfedit.hxx"
#include "editutil.hxx"
#include "global.hxx"
#include "attrib.hxx"
#include "patattr.hxx"
#include "scresid.hxx"
#include "sc.hrc"
#include "globstr.hrc"
#include "tabvwsh.hxx"
#include "prevwsh.hxx"
#include "hfedtdlg.hrc"
#include "AccessibleEditObject.hxx"

#include "scabstdlg.hxx"


// STATIC DATA -----------------------------------------------------------
static ScEditWindow* pActiveEdWnd = NULL;

ScEditWindow* GetScEditWindow ()
{
    return pActiveEdWnd;
}

//========================================================================

void lcl_GetFieldData( ScHeaderFieldData& rData )
{
    SfxViewShell* pShell = SfxViewShell::Current();
    if (pShell)
    {
        if (pShell->ISA(ScTabViewShell))
            ((ScTabViewShell*)pShell)->FillFieldData(rData);
        else if (pShell->ISA(ScPreviewShell))
            ((ScPreviewShell*)pShell)->FillFieldData(rData);
    }
}

//========================================================================
// class ScEditWindow
//========================================================================

ScEditWindow::ScEditWindow( Window* pParent, const ResId& rResId, ScEditWindowLocation eLoc )
    :   Control( pParent, rResId ),
    eLocation(eLoc),
    pAcc(NULL)
{
    EnableRTL(FALSE);

    const StyleSettings& rStyleSettings = Application::GetSettings().GetStyleSettings();
    Color aBgColor = rStyleSettings.GetWindowColor();

    SetMapMode( MAP_TWIP );
    SetPointer( POINTER_TEXT );
    SetBackground( aBgColor );

    Size aSize( GetOutputSize() );
    aSize.Height() *= 4;

    pEdEngine = new ScHeaderEditEngine( EditEngine::CreatePool(), TRUE );
    pEdEngine->SetPaperSize( aSize );
    pEdEngine->SetRefDevice( this );

    ScHeaderFieldData aData;
    lcl_GetFieldData( aData );

        //  Feldbefehle:
    pEdEngine->SetData( aData );
    pEdEngine->SetControlWord( pEdEngine->GetControlWord() | EE_CNTRL_MARKFIELDS );
    mbRTL = ScGlobal::IsSystemRTL();
    if (mbRTL)
        pEdEngine->SetDefaultHorizontalTextDirection(EE_HTEXTDIR_R2L);

    pEdView = new EditView( pEdEngine, this );
    pEdView->SetOutputArea( Rectangle( Point(0,0), GetOutputSize() ) );

    pEdView->SetBackgroundColor( aBgColor );
    pEdEngine->InsertView( pEdView );
}

// -----------------------------------------------------------------------

__EXPORT ScEditWindow::~ScEditWindow()
{
    // delete Accessible object before deleting EditEngine and EditView
    if (pAcc)
    {
        ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > xTemp = xAcc;
        if (xTemp.is())
            pAcc->dispose();
    }
    delete pEdEngine;
    delete pEdView;
}

// -----------------------------------------------------------------------

void ScEditWindow::SetNumType(SvxNumType eNumType)
{
    pEdEngine->SetNumType(eNumType);
    pEdEngine->UpdateFields();
}

// -----------------------------------------------------------------------

EditTextObject* __EXPORT ScEditWindow::CreateTextObject()
{
    //  wegen #38841# die Absatzattribute zuruecksetzen
    //  (GetAttribs beim Format-Dialog-Aufruf gibt immer gesetzte Items zurueck)

    const SfxItemSet& rEmpty = pEdEngine->GetEmptyItemSet();
    USHORT nParCnt = pEdEngine->GetParagraphCount();
    for (USHORT i=0; i<nParCnt; i++)
        pEdEngine->SetParaAttribs( i, rEmpty );

    return pEdEngine->CreateTextObject();
}

// -----------------------------------------------------------------------

void ScEditWindow::SetFont( const ScPatternAttr& rPattern )
{
    SfxItemSet* pSet = new SfxItemSet( pEdEngine->GetEmptyItemSet() );
    rPattern.FillEditItemSet( pSet );
    //  FillEditItemSet adjusts font height to 1/100th mm,
    //  but for header/footer twips is needed, as in the PatternAttr:
    pSet->Put( rPattern.GetItem(ATTR_FONT_HEIGHT), EE_CHAR_FONTHEIGHT );
    pSet->Put( rPattern.GetItem(ATTR_CJK_FONT_HEIGHT), EE_CHAR_FONTHEIGHT_CJK );
    pSet->Put( rPattern.GetItem(ATTR_CTL_FONT_HEIGHT), EE_CHAR_FONTHEIGHT_CTL );
    if (mbRTL)
        pSet->Put( SvxAdjustItem( SVX_ADJUST_RIGHT, EE_PARA_JUST ) );
    pEdEngine->SetDefaults( pSet );
}

// -----------------------------------------------------------------------

void ScEditWindow::SetText( const EditTextObject& rTextObject )
{
    pEdEngine->SetText( rTextObject );
}

// -----------------------------------------------------------------------

void ScEditWindow::InsertField( const SvxFieldItem& rFld )
{
    pEdView->InsertField( rFld );
}

// -----------------------------------------------------------------------

void ScEditWindow::SetCharAttriutes()
{
    SfxObjectShell* pDocSh  = SfxObjectShell::Current();

    SfxViewShell*       pViewSh = SfxViewShell::Current();

    ScTabViewShell* pTabViewSh = PTR_CAST(ScTabViewShell, SfxViewShell::Current());


    DBG_ASSERT( pDocSh,  "Current DocShell not found" );
    DBG_ASSERT( pViewSh, "Current ViewShell not found" );

    if ( pDocSh && pViewSh )
    {
        if(pTabViewSh!=NULL) pTabViewSh->SetInFormatDialog(TRUE);

        SfxItemSet aSet( pEdView->GetAttribs() );

        ScAbstractDialogFactory* pFact = ScAbstractDialogFactory::Create();
        DBG_ASSERT(pFact, "ScAbstractFactory create fail!");

        SfxAbstractTabDialog* pDlg = pFact->CreateScCharDlg(  GetParent(),  &aSet,
                                                            pDocSh,RID_SCDLG_CHAR );
        DBG_ASSERT(pDlg, "Dialog create fail!");
        pDlg->SetText( ScGlobal::GetRscString( STR_TEXTATTRS ) );
        if ( pDlg->Execute() == RET_OK )
        {
            aSet.ClearItem();
            aSet.Put( *pDlg->GetOutputItemSet() );
            pEdView->SetAttribs( aSet );
        }

        if(pTabViewSh!=NULL) pTabViewSh->SetInFormatDialog(FALSE);
        delete pDlg;
    }
}

// -----------------------------------------------------------------------

void __EXPORT ScEditWindow::Paint( const Rectangle& rRec )
{
    const StyleSettings& rStyleSettings = Application::GetSettings().GetStyleSettings();
    Color aBgColor = rStyleSettings.GetWindowColor();

    pEdView->SetBackgroundColor( aBgColor );

    SetBackground( aBgColor );

    Control::Paint( rRec );

    pEdView->Paint( rRec );
}

// -----------------------------------------------------------------------

void __EXPORT ScEditWindow::MouseMove( const MouseEvent& rMEvt )
{
    pEdView->MouseMove( rMEvt );
}

// -----------------------------------------------------------------------

void __EXPORT ScEditWindow::MouseButtonDown( const MouseEvent& rMEvt )
{
    if ( !HasFocus() )
        GrabFocus();

    pEdView->MouseButtonDown( rMEvt );
}

// -----------------------------------------------------------------------

void __EXPORT ScEditWindow::MouseButtonUp( const MouseEvent& rMEvt )
{
    pEdView->MouseButtonUp( rMEvt );
}

// -----------------------------------------------------------------------

void __EXPORT ScEditWindow::KeyInput( const KeyEvent& rKEvt )
{
    USHORT nKey =  rKEvt.GetKeyCode().GetModifier()
                 + rKEvt.GetKeyCode().GetCode();

    if ( nKey == KEY_TAB || nKey == KEY_TAB + KEY_SHIFT )
    {
        Control::KeyInput( rKEvt );
    }
    else if ( !pEdView->PostKeyEvent( rKEvt ) )
    {
        Control::KeyInput( rKEvt );
    }
}

// -----------------------------------------------------------------------

void ScEditWindow::Command( const CommandEvent& rCEvt )
{
    pEdView->Command( rCEvt );
}

// -----------------------------------------------------------------------

void __EXPORT ScEditWindow::GetFocus()
{
    pActiveEdWnd = this;

    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > xTemp = xAcc;
    if (xTemp.is() && pAcc)
    {
        pAcc->GotFocus();
    }
    else
        pAcc = NULL;
}

void __EXPORT ScEditWindow::LoseFocus()
{
    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > xTemp = xAcc;
    if (xTemp.is() && pAcc)
    {
        pAcc->LostFocus();
    }
    else
        pAcc = NULL;
}

// -----------------------------------------------------------------------

::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > ScEditWindow::CreateAccessible()
{
    String sName;
    String sDescription(GetHelpText());
    switch (eLocation)
    {
    case Left:
        {
            sName = String(ScResId(STR_ACC_LEFTAREA_NAME));
//            sDescription = String(ScResId(STR_ACC_LEFTAREA_DESCR));
        }
        break;
    case Center:
        {
            sName = String(ScResId(STR_ACC_CENTERAREA_NAME));
//            sDescription = String(ScResId(STR_ACC_CENTERAREA_DESCR));
        }
        break;
    case Right:
        {
            sName = String(ScResId(STR_ACC_RIGHTAREA_NAME));
//            sDescription = String(ScResId(STR_ACC_RIGHTAREA_DESCR));
        }
        break;
    }
    pAcc = new ScAccessibleEditObject(GetAccessibleParentWindow()->GetAccessible(), pEdView, this,
        rtl::OUString(sName), rtl::OUString(sDescription), EditControl);
    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > xAccessible = pAcc;
    xAcc = xAccessible;
    return pAcc;
}

/*
class ScExtIButton : public ImageButton
{
private:

    Timer           aTimer;
    ScPopupMenu*    pPopupMenu;

    DECL_LINK( TimerHdl, Timer*);

    void            DrawArrow();

protected:

    virtual void    MouseButtonDown( const MouseEvent& rMEvt );
    virtual void    MouseButtonUp( const MouseEvent& rMEvt);

    virtual void    StartPopup();

public:

    ScExtIButton(Window* pParent, const ResId& rResId );

    void            SetPopupMenu(ScPopupMenu* pPopUp);

    USHORT          GetSelected();

    void            SetMenuHdl( const Link& rLink ) { aFxLink = rLink; }
    const Link&     GetMenuHdl() const { return aFxLink; }

}
*/
ScExtIButton::ScExtIButton(Window* pParent, const ResId& rResId )
:   ImageButton(pParent,rResId),
    pPopupMenu(NULL)
{
    nSelected=0;
    aTimer.SetTimeout(600);
    SetDropDown( TRUE);

//  DrawArrow();
}

void ScExtIButton::SetPopupMenu(ScPopupMenu* pPopUp)
{
    pPopupMenu=pPopUp;
}

USHORT ScExtIButton::GetSelected()
{
    return nSelected;
}

void ScExtIButton::MouseButtonDown( const MouseEvent& rMEvt )
{
    if(!aTimer.IsActive())
    {
        aTimer.Start();
        aTimer.SetTimeoutHdl(LINK( this, ScExtIButton, TimerHdl));
    }

    ImageButton::MouseButtonDown(rMEvt );
}
void ScExtIButton::MouseButtonUp( const MouseEvent& rMEvt)
{
    aTimer.Stop();
    aTimer.SetTimeoutHdl(Link());
    ImageButton::MouseButtonUp(rMEvt );
}

void ScExtIButton::Click()
{
    aTimer.Stop();
    aTimer.SetTimeoutHdl(Link());
    ImageButton::Click();
}

void ScExtIButton::StartPopup()
{
    nSelected=0;

    if(pPopupMenu!=NULL)
    {
        SetPressed( TRUE );
        EndSelection();
        Point aPoint(0,0);
        aPoint.Y()=GetOutputSizePixel().Height();

        nSelected=pPopupMenu->Execute( this, aPoint );

        if(nSelected)
        {
            aMLink.Call(this);
        }
        SetPressed( FALSE);
    }
}

long ScExtIButton::PreNotify( NotifyEvent& rNEvt )
{
    USHORT nSwitch=rNEvt.GetType();
    if(nSwitch==EVENT_MOUSEBUTTONUP)
    {
        MouseButtonUp(*rNEvt.GetMouseEvent());
    }

    return ImageButton::PreNotify(rNEvt );
}

IMPL_LINK( ScExtIButton, TimerHdl, Timer*, EMPTYARG )
{
    StartPopup();
    return 0;
}

/*
static void ImplDrawToolArrow( ToolBox* pBox, long nX, long nY, BOOL bBlack,
                               BOOL bLeft = FALSE, BOOL bTop = FALSE )
{
    Color           aOldFillColor = pBox->GetFillColor();
    WindowAlign     eAlign = pBox->meAlign;
    if ( bLeft )
        eAlign = WINDOWALIGN_RIGHT;
    else if ( bTop )
        eAlign = WINDOWALIGN_BOTTOM;

    switch ( eAlign )
    {
        case WINDOWALIGN_LEFT:
            if ( bBlack )
                pBox->SetFillColor( Color( COL_BLACK ) );
            pBox->DrawRect( Rectangle( nX+0, nY+0, nX+0, nY+6 ) );
            pBox->DrawRect( Rectangle( nX+1, nY+1, nX+1, nY+5 ) );
            pBox->DrawRect( Rectangle( nX+2, nY+2, nX+2, nY+4 ) );
            pBox->DrawRect( Rectangle( nX+3, nY+3, nX+3, nY+3 ) );
            if ( bBlack )
            {
                pBox->SetFillColor( aOldFillColor );
                pBox->DrawRect( Rectangle( nX+1, nY+2, nX+1, nY+4 ) );
                pBox->DrawRect( Rectangle( nX+2, nY+3, nX+2, nY+3 ) );
            }
            break;
        case WINDOWALIGN_TOP:
            if ( bBlack )
                pBox->SetFillColor( Color( COL_BLACK ) );
            pBox->DrawRect( Rectangle( nX+0, nY+0, nX+6, nY+0 ) );
            pBox->DrawRect( Rectangle( nX+1, nY+1, nX+5, nY+1 ) );
            pBox->DrawRect( Rectangle( nX+2, nY+2, nX+4, nY+2 ) );
            pBox->DrawRect( Rectangle( nX+3, nY+3, nX+3, nY+3 ) );
            if ( bBlack )
            {
                pBox->SetFillColor( aOldFillColor );
                pBox->DrawRect( Rectangle( nX+2, nY+1, nX+4, nY+1 ) );
                pBox->DrawRect( Rectangle( nX+3, nY+2, nX+3, nY+2 ) );
            }
            break;
        case WINDOWALIGN_RIGHT:
            if ( bBlack )
                pBox->SetFillColor( Color( COL_BLACK ) );
            pBox->DrawRect( Rectangle( nX+3, nY+0, nX+3, nY+6 ) );
            pBox->DrawRect( Rectangle( nX+2, nY+1, nX+2, nY+5 ) );
            pBox->DrawRect( Rectangle( nX+1, nY+2, nX+1, nY+4 ) );
            pBox->DrawRect( Rectangle( nX+0, nY+3, nX+0, nY+3 ) );
            if ( bBlack )
            {
                pBox->SetFillColor( aOldFillColor );
                pBox->DrawRect( Rectangle( nX+2, nY+2, nX+2, nY+4 ) );
                pBox->DrawRect( Rectangle( nX+1, nY+3, nX+1, nY+3 ) );
            }
            break;
        case WINDOWALIGN_BOTTOM:
            if ( bBlack )
                pBox->SetFillColor( Color( COL_BLACK ) );
            pBox->DrawRect( Rectangle( nX+0, nY+3, nX+6, nY+3 ) );
            pBox->DrawRect( Rectangle( nX+1, nY+2, nX+5, nY+2 ) );
            pBox->DrawRect( Rectangle( nX+2, nY+1, nX+4, nY+1 ) );
            pBox->DrawRect( Rectangle( nX+3, nY+0, nX+3, nY+0 ) );
            if ( bBlack )
            {
                pBox->SetFillColor( aOldFillColor );
                pBox->DrawRect( Rectangle( nX+2, nY+2, nX+4, nY+2 ) );
                pBox->DrawRect( Rectangle( nX+3, nY+1, nX+3, nY+1 ) );
            }
            break;
    }
}
Down
    - Timer starten

Click
    - Timer abbrechen

Timer
    if ( ??? )
    {
    - SetPressed( TRUE );
    - EndSelection();
    - Menu anzeigen
    - SetPressed( FALSE );
    }


*/


