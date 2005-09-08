/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: tabopdlg.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 22:14:47 $
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

#ifdef PCH
#include "ui_pch.hxx"
#endif

#pragma hdrstop

//----------------------------------------------------------------------------

#include "scitems.hxx"
#include <sfx2/dispatch.hxx>
#include <vcl/msgbox.hxx>

#include "uiitems.hxx"
#include "global.hxx"
#include "document.hxx"
#include "scresid.hxx"
#include "sc.hrc"
#include "reffact.hxx"
#include "tabopdlg.hrc"

#define _TABOPDLG_CXX
#include "tabopdlg.hxx"


//============================================================================
//  class ScTabOpDlg
//----------------------------------------------------------------------------

ScTabOpDlg::ScTabOpDlg( SfxBindings* pB, SfxChildWindow* pCW, Window* pParent,
                        ScDocument*         pDocument,
                        const ScRefAddress& rCursorPos )

    :   ScAnyRefDlg         ( pB, pCW, pParent, RID_SCDLG_TABOP ),
        //
        aFtFormulaRange     ( this, ScResId( FT_FORMULARANGE ) ),
        aEdFormulaRange     ( this, ScResId( ED_FORMULARANGE ) ),
        aRBFormulaRange     ( this, ScResId( RB_FORMULARANGE ), &aEdFormulaRange ),
        aFtRowCell          ( this, ScResId( FT_ROWCELL ) ),
        aEdRowCell          ( this, ScResId( ED_ROWCELL ) ),
        aRBRowCell          ( this, ScResId( RB_ROWCELL ), &aEdRowCell ),
        aFtColCell          ( this, ScResId( FT_COLCELL ) ),
        aEdColCell          ( this, ScResId( ED_COLCELL ) ),
        aRBColCell          ( this, ScResId( RB_COLCELL ), &aEdColCell ),
        aFlVariables        ( this, ScResId( FL_VARIABLES ) ),
        aBtnOk              ( this, ScResId( BTN_OK ) ),
        aBtnCancel          ( this, ScResId( BTN_CANCEL ) ),
        aBtnHelp            ( this, ScResId( BTN_HELP ) ),
        errMsgNoFormula     ( ScResId( STR_NOFORMULA ) ),
        errMsgNoColRow      ( ScResId( STR_NOCOLROW ) ),
        errMsgWrongFormula  ( ScResId( STR_WRONGFORMULA ) ),
        errMsgWrongRowCol   ( ScResId( STR_WRONGROWCOL ) ),
        errMsgNoColFormula  ( ScResId( STR_NOCOLFORMULA ) ),
        errMsgNoRowFormula  ( ScResId( STR_NOROWFORMULA ) ),
        //
        pDoc                ( pDocument ),
        theFormulaCell      ( rCursorPos ),
        nCurTab             ( theFormulaCell.Tab() ),
        pEdActive           ( NULL ),
        bDlgLostFocus       ( FALSE )
{
    Init();
    FreeResource();
}

//----------------------------------------------------------------------------

__EXPORT ScTabOpDlg::~ScTabOpDlg()
{
    Hide();
}

//----------------------------------------------------------------------------

void __EXPORT ScTabOpDlg::Init()
{
    aBtnOk.         SetClickHdl     ( LINK( this, ScTabOpDlg, BtnHdl ) );
    aBtnCancel.     SetClickHdl     ( LINK( this, ScTabOpDlg, BtnHdl ) );

    Link aLink = LINK( this, ScTabOpDlg, GetFocusHdl );
    aEdFormulaRange.SetGetFocusHdl( aLink );
    aRBFormulaRange.SetGetFocusHdl( aLink );
    aEdRowCell.     SetGetFocusHdl( aLink );
    aRBRowCell.     SetGetFocusHdl( aLink );
    aEdColCell.     SetGetFocusHdl( aLink );
    aRBColCell.     SetGetFocusHdl( aLink );

    aLink = LINK( this, ScTabOpDlg, LoseFocusHdl );
    aEdFormulaRange.SetLoseFocusHdl( aLink );
    aRBFormulaRange.SetLoseFocusHdl( aLink );
    aEdRowCell.     SetLoseFocusHdl( aLink );
    aRBRowCell.     SetLoseFocusHdl( aLink );
    aEdColCell.     SetLoseFocusHdl( aLink );
    aRBColCell.     SetLoseFocusHdl( aLink );

    aEdFormulaRange.GrabFocus();
    pEdActive = &aEdFormulaRange;

    //@BugID 54702 Enablen/Disablen nur noch in Basisklasse
    //SFX_APPWINDOW->Enable();
}

//----------------------------------------------------------------------------

BOOL __EXPORT ScTabOpDlg::Close()
{
    return DoClose( ScTabOpDlgWrapper::GetChildWindowId() );
}

//----------------------------------------------------------------------------

void ScTabOpDlg::SetActive()
{
    if ( bDlgLostFocus )
    {
        bDlgLostFocus = FALSE;
        if( pEdActive )
            pEdActive->GrabFocus();
    }
    else
        GrabFocus();

    RefInputDone();
}

//----------------------------------------------------------------------------

void ScTabOpDlg::SetReference( const ScRange& rRef, ScDocument* pDoc )
{
    if ( pEdActive )
    {
        if ( rRef.aStart != rRef.aEnd )
            RefInputStart(pEdActive);

        String      aStr;
        USHORT      nFmt = ( rRef.aStart.Tab() == nCurTab )
                                ? SCR_ABS
                                : SCR_ABS_3D;

        if ( pEdActive == &aEdFormulaRange )
        {
            theFormulaCell.Set( rRef.aStart, false, false, false);
            theFormulaEnd.Set( rRef.aEnd, false, false, false);
            rRef.Format( aStr, nFmt, pDoc );
        }
        else if ( pEdActive == &aEdRowCell )
        {
            theRowCell.Set( rRef.aStart, false, false, false);
            rRef.aStart.Format( aStr, nFmt, pDoc );
        }
        else if ( pEdActive == &aEdColCell )
        {
            theColCell.Set( rRef.aStart, false, false, false);
            rRef.aStart.Format( aStr, nFmt, pDoc );
        }

        pEdActive->SetRefString( aStr );
    }
}

//----------------------------------------------------------------------------

void ScTabOpDlg::RaiseError( ScTabOpErr eError )
{
    const String* pMsg = &errMsgNoFormula;
    Edit*         pEd  = &aEdFormulaRange;

    switch ( eError )
    {
        case TABOPERR_NOFORMULA:
            pMsg = &errMsgNoFormula;
            pEd  = &aEdFormulaRange;
            break;

        case TABOPERR_NOCOLROW:
            pMsg = &errMsgNoColRow;
            pEd  = &aEdRowCell;
            break;

        case TABOPERR_WRONGFORMULA:
            pMsg = &errMsgWrongFormula;
            pEd  = &aEdFormulaRange;
            break;

        case TABOPERR_WRONGROW:
            pMsg = &errMsgWrongRowCol;
            pEd  = &aEdRowCell;
            break;

        case TABOPERR_NOCOLFORMULA:
            pMsg = &errMsgNoColFormula;
            pEd  = &aEdFormulaRange;
            break;

        case TABOPERR_WRONGCOL:
            pMsg = &errMsgWrongRowCol;
            pEd  = &aEdColCell;
            break;

        case TABOPERR_NOROWFORMULA:
            pMsg = &errMsgNoRowFormula;
            pEd  = &aEdFormulaRange;
            break;
    }

    ErrorBox( this, WinBits( WB_OK_CANCEL | WB_DEF_OK), *pMsg ).Execute();
    pEd->GrabFocus();
}

//----------------------------------------------------------------------------

BOOL lcl_Parse( const String& rString, ScDocument* pDoc, SCTAB nCurTab,
                ScRefAddress& rStart, ScRefAddress& rEnd )
{
    BOOL bRet = FALSE;
    if ( rString.Search(':') != STRING_NOTFOUND )
        bRet = ConvertDoubleRef( pDoc, rString, nCurTab, rStart, rEnd );
    else
    {
        bRet = ConvertSingleRef( pDoc, rString, nCurTab, rStart );
        rEnd = rStart;
    }
    return bRet;
}

//----------------------------------------------------------------------------
// Handler:

IMPL_LINK( ScTabOpDlg, BtnHdl, PushButton*, pBtn )
{
    if ( pBtn == &aBtnOk )
    {
        BYTE nMode = 3;
        USHORT nError = 0;

        // Zu ueberpruefen:
        // 1. enthalten die Strings korrekte Tabellenkoordinaten/def.Namen?
        // 2. IstFormelRang Zeile bei leerer Zeile bzw. Spalte bei leerer Spalte
        //    bzw. Einfachreferenz bei beidem?
        // 3. Ist mindestens Zeile oder Spalte und Formel voll?

        if (aEdFormulaRange.GetText().Len() == 0)
            nError = TABOPERR_NOFORMULA;
        else if (aEdRowCell.GetText().Len() == 0 &&
                 aEdColCell.GetText().Len() == 0)
            nError = TABOPERR_NOCOLROW;
        else if ( !lcl_Parse( aEdFormulaRange.GetText(), pDoc, nCurTab,
                                theFormulaCell, theFormulaEnd ) )
            nError = TABOPERR_WRONGFORMULA;
        else
        {
            if (aEdRowCell.GetText().Len() > 0)
            {
                if (!ConvertSingleRef( pDoc, aEdRowCell.GetText(), nCurTab, theRowCell ))
                    nError = TABOPERR_WRONGROW;
                else
                {
                    if (aEdColCell.GetText().Len() == 0 &&
                        theFormulaCell.Col() != theFormulaEnd.Col())
                        nError = TABOPERR_NOCOLFORMULA;
                    else
                        nMode = 1;
                }
            }
            if (aEdColCell.GetText().Len() > 0)
            {
                if (!ConvertSingleRef( pDoc, aEdColCell.GetText(), nCurTab,
                                       theColCell ))
                    nError = TABOPERR_WRONGCOL;
                else
                {
                    if (nMode == 1)                         // beides
                    {
                        nMode = 2;
                        ConvertSingleRef( pDoc, aEdFormulaRange.GetText(), nCurTab,
                                          theFormulaCell );
                    }
                    else if (theFormulaCell.Row() != theFormulaEnd.Row())
                        nError = TABOPERR_NOROWFORMULA;
                    else
                        nMode = 0;
                }
            }
        }

        if (nError)
            RaiseError( (ScTabOpErr) nError );
        else
        {
            ScTabOpParam aOutParam( theFormulaCell,
                                    theFormulaEnd,
                                    theRowCell,
                                    theColCell,
                                    nMode );
            ScTabOpItem  aOutItem( SID_TABOP, &aOutParam );

            SetDispatcherLock( FALSE );
            SwitchToDocument();
            GetBindings().GetDispatcher()->Execute( SID_TABOP,
                                      SFX_CALLMODE_SLOT | SFX_CALLMODE_RECORD,
                                      &aOutItem, 0L, 0L );
            Close();
        }
    }
    else if ( pBtn == &aBtnCancel )
        Close();

    return 0;
}

//----------------------------------------------------------------------------

IMPL_LINK( ScTabOpDlg, GetFocusHdl, Control*, pCtrl )
{
    if( (pCtrl == (Control*)&aEdFormulaRange) || (pCtrl == (Control*)&aRBFormulaRange) )
        pEdActive = &aEdFormulaRange;
    else if( (pCtrl == (Control*)&aEdRowCell) || (pCtrl == (Control*)&aRBRowCell) )
        pEdActive = &aEdRowCell;
    else if( (pCtrl == (Control*)&aEdColCell) || (pCtrl == (Control*)&aRBColCell) )
        pEdActive = &aEdColCell;
    else
        pEdActive = NULL;

    if( pEdActive )
        pEdActive->SetSelection( Selection( 0, SELECTION_MAX ) );

    return 0;
}

//----------------------------------------------------------------------------

IMPL_LINK( ScTabOpDlg, LoseFocusHdl, Control*, pCtrl )
{
    bDlgLostFocus = !IsActive();
    return 0;
}





