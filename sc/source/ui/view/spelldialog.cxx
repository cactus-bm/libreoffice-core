/*************************************************************************
 *
 *  $RCSfile: spelldialog.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-09-17 13:54:50 $
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

#ifndef SC_SPELLDIALOG_HXX
#include "spelldialog.hxx"
#endif

#include <sfx2/app.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <svx/svxids.hrc>
#include <svx/editstat.hxx>
#include <svx/editview.hxx>
#include <svx/unolingu.hxx>

#ifndef SC_SELECTIONSTATE_HXX
#include "selectionstate.hxx"
#endif

#include "spelleng.hxx"
#include "tabvwsh.hxx"
#include "docsh.hxx"
#include "scmod.hxx"
#include "editable.hxx"
#include "undoblk.hxx"

// ============================================================================

SFX_IMPL_CHILDWINDOW( ScSpellDialogChildWindow, SID_SPELL_DIALOG )

ScSpellDialogChildWindow::ScSpellDialogChildWindow( Window* pParent, USHORT nId,
        SfxBindings* pBindings, SfxChildWinInfo* pInfo ) :
    ::svx::SpellDialogChildWindow( pParent, nId, pBindings, pInfo ),
    mpViewShell( 0 ),
    mpViewData( 0 ),
    mpDocShell( 0 ),
    mpDoc( 0 ),
    mbNeedNextObj( false ),
    mbOldIdleDisabled( false )
{
    Init();
}

ScSpellDialogChildWindow::~ScSpellDialogChildWindow()
{
    Reset();
}

SfxChildWinInfo ScSpellDialogChildWindow::GetInfo() const
{
    return ::svx::SpellDialogChildWindow::GetInfo();
}

void ScSpellDialogChildWindow::InvalidateSpellDialog()
{
    ::svx::SpellDialogChildWindow::InvalidateSpellDialog();
}

// protected ------------------------------------------------------------------

::svx::SpellPortions ScSpellDialogChildWindow::GetNextWrongSentence()
{
    ::svx::SpellPortions aPortions;
    if( mxEngine.get() && mpViewData )
    {
        if( EditView* pEditView = mpViewData->GetSpellingView() )
        {
            // edit engine handles cell iteration internally
            do
            {
                if( mbNeedNextObj )
                    mxEngine->SpellNextDocument();
                mbNeedNextObj = !mxEngine->IsFinished() && !mxEngine->SpellSentence( *pEditView, aPortions );
            }
            while( mbNeedNextObj );
        }

        // finished? - close the spelling dialog
        if( mxEngine->IsFinished() )
            GetBindings().GetDispatcher()->Execute( SID_SPELL_DIALOG, SFX_CALLMODE_ASYNCHRON );
    }
    return aPortions;
}

void ScSpellDialogChildWindow::ApplyChangedSentence( const ::svx::SpellPortions& rChanged )
{
    if( mxEngine.get() && mpViewData )
        if( EditView* pEditView = mpViewData->GetSpellingView() )
            mxEngine->ApplyChangedSentence( *pEditView, rChanged );
}

void ScSpellDialogChildWindow::GetFocus()
{
    if( IsSelectionChanged() )
    {
        Reset();
        InvalidateSpellDialog();
        Init();
    }
}

void ScSpellDialogChildWindow::LoseFocus()
{
}

// private --------------------------------------------------------------------

void ScSpellDialogChildWindow::Reset()
{
    if( mpViewShell && (mpViewShell == PTR_CAST( ScTabViewShell, SfxViewShell::Current() )) )
    {
        if( mxEngine.get() && mxEngine->IsAnyModified() )
        {
            const ScAddress& rCursor = mxOldSel->GetCellCursor();
            SCTAB nTab = rCursor.Tab();
            SCCOL nOldCol = rCursor.Col();
            SCROW nOldRow = rCursor.Row();
            SCCOL nNewCol = mpViewData->GetCurX();
            SCROW nNewRow = mpViewData->GetCurY();
            mpDocShell->GetUndoManager()->AddUndoAction( new ScUndoConversion(
                mpDocShell, mpViewData->GetMarkData(),
                nOldCol, nOldRow, nTab, mxUndoDoc.release(),
                nNewCol, nNewRow, nTab, mxRedoDoc.release(),
                SC_CONVERSION_SPELLCHECK ) );
            mpDoc->SetDirty();
            mpDocShell->SetDocumentModified();
        }

        mpViewData->SetSpellingView( 0 );
        mpViewShell->KillEditView( TRUE );
        mpDocShell->PostPaintGridAll();
        mpViewShell->UpdateInputHandler();
        mpDoc->DisableIdle( mbOldIdleDisabled );
    }
    mxEngine.reset();
    mxUndoDoc.reset();
    mxRedoDoc.reset();
    mxOldSel.reset();
    mpViewShell = 0;
    mpViewData = 0;
    mpDocShell = 0;
    mpDoc = 0;
    mbNeedNextObj = false;
    mbOldIdleDisabled = false;
}

void ScSpellDialogChildWindow::Init()
{
    if( mpViewShell )
        return;
    if( (mpViewShell = PTR_CAST( ScTabViewShell, SfxViewShell::Current() )) == 0 )
        return;

    mpViewData = mpViewShell->GetViewData();

    // exit edit mode - TODO support spelling in edit mode
    if( mpViewData->HasEditView( mpViewData->GetActivePart() ) )
        SC_MOD()->InputEnterHandler();

    mxOldSel.reset( new ScSelectionState( *mpViewData ) );

    mpDocShell = mpViewData->GetDocShell();
    mpDoc = mpDocShell->GetDocument();

    const ScAddress& rCursor = mxOldSel->GetCellCursor();
    SCCOL nCol = rCursor.Col();
    SCROW nRow = rCursor.Row();
    SCTAB nTab = rCursor.Tab();

    ScMarkData& rMarkData = mpViewData->GetMarkData();
    rMarkData.MarkToMulti();

    switch( mxOldSel->GetSelectionType() )
    {
        case SC_SELECTTYPE_NONE:
        case SC_SELECTTYPE_SHEET:
        {
            // test if there is something editable
            ScEditableTester aTester( mpDoc, rMarkData );
            if( !aTester.IsEditable() )
            {
                mpViewShell->ErrorMessage( aTester.GetMessageId() );
                return;
            }
        }
        break;

        // edit mode exited, see TODO above
//        case SC_SELECTTYPE_EDITCELL:
//        break;

        default:
            DBG_ERRORFILE( "ScSpellDialogChildWindow::Init - unknown selection type" );
    }

    mbOldIdleDisabled = mpDoc->IsIdleDisabled();
    mpDoc->DisableIdle( TRUE );   // #42726# stop online spelling

    // *** create Undo/Redo documents *** -------------------------------------

    mxUndoDoc.reset( new ScDocument( SCDOCMODE_UNDO ) );
    mxUndoDoc->InitUndo( mpDoc, nTab, nTab );
    mxRedoDoc.reset( new ScDocument( SCDOCMODE_UNDO ) );
    mxRedoDoc->InitUndo( mpDoc, nTab, nTab );

    if ( rMarkData.GetSelectCount() > 1 )
    {
        SCTAB nTabCount = mpDoc->GetTableCount();
        for( SCTAB nOtherTab = 0; nOtherTab < nTabCount; ++nOtherTab )
        {
            if( rMarkData.GetTableSelect( nOtherTab ) && (nOtherTab != nTab) )
            {
                mxUndoDoc->AddUndoTab( nOtherTab, nOtherTab );
                mxRedoDoc->AddUndoTab( nOtherTab, nOtherTab );
            }
        }
    }

    // *** create and init the edit engine *** --------------------------------

    mxEngine.reset( new ScSpellingEngine(
        mpDoc->GetEnginePool(), *mpViewData, mxUndoDoc.get(), mxRedoDoc.get(), LinguMgr::GetSpellChecker() ) );
    mxEngine->SetRefDevice( mpViewData->GetActiveWin() );

    mpViewShell->MakeEditView( mxEngine.get(), nCol, nRow );
    EditView* pEditView = mpViewData->GetEditView( mpViewData->GetActivePart() );
    mpViewData->SetSpellingView( pEditView );
    Rectangle aRect( Point( 0, 0 ), Point( 0, 0 ) );
    pEditView->SetOutputArea( aRect );
    mxEngine->SetControlWord( EE_CNTRL_USECHARATTRIBS );
    mxEngine->EnableUndo( FALSE );
    mxEngine->SetPaperSize( aRect.GetSize() );
    mxEngine->SetText( EMPTY_STRING );
    mxEngine->ClearModifyFlag();

    mbNeedNextObj = true;
}

bool ScSpellDialogChildWindow::IsSelectionChanged()
{
    if( !mxOldSel.get() || !mpViewShell || (mpViewShell != PTR_CAST( ScTabViewShell, SfxViewShell::Current() )) )
        return true;

    if( EditView* pEditView = mpViewData->GetSpellingView() )
        if( pEditView->GetEditEngine() != mxEngine.get() )
            return true;

    ScSelectionState aNewSel( *mpViewData );
    return mxOldSel->GetSheetSelection() != aNewSel.GetSheetSelection();
}

// ============================================================================

