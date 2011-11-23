/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

// System - Includes ---------------------------------------------------------



// INCLUDE -------------------------------------------------------------------

#include "global.hxx"
#include "reffact.hxx"
#include "document.hxx"
#include "docfunc.hxx"
#include "scresid.hxx"
#include "globstr.hrc"
#include "namedlg.hrc"
#include "namedlg.hxx"
#include "viewdata.hxx"

#include "sfx2/app.hxx"

#include <vcl/msgbox.hxx>

#include <map>
#include <memory>

using ::std::auto_ptr;

// defines -------------------------------------------------------------------

#define ABS_SREF          SCA_VALID \
    | SCA_COL_ABSOLUTE | SCA_ROW_ABSOLUTE | SCA_TAB_ABSOLUTE
#define ABS_DREF          ABS_SREF \
    | SCA_COL2_ABSOLUTE | SCA_ROW2_ABSOLUTE | SCA_TAB2_ABSOLUTE
#define ABS_SREF3D      ABS_SREF | SCA_TAB_3D
#define ABS_DREF3D      ABS_DREF | SCA_TAB_3D

//ScRangeManagerTable

#define ITEMID_NAME 1
#define ITEMID_RANGE 2
#define ITEMID_SCOPE 3

namespace {

ScRangeName* GetRangeName(const rtl::OUString& rTableName, ScDocument* pDoc)
{
    ScRangeName* pRangeName;
    SCTAB nTab;
    if ( pDoc->GetTable(rTableName, nTab))
    {
        pRangeName = pDoc->GetRangeName(nTab);
    }
    else
    {
        pRangeName = pDoc->GetRangeName();
    }
    return pRangeName;
}

}

String createEntryString(const ScRangeNameLine& rLine)
{
    String aRet(rLine.aName);
    aRet += '\t';
    aRet += String(rLine.aExpression);
    aRet += '\t';
    aRet += String(rLine.aScope);
    return aRet;
}

ScRangeManagerTable::ScRangeManagerTable( Window* pWindow, ScRangeName* pGlobalRangeName, std::map<rtl::OUString, ScRangeName*> aTabRangeNames ):
    SvTabListBox( pWindow, WB_SORT | WB_HSCROLL | WB_CLIPCHILDREN | WB_TABSTOP ),
    maHeaderBar( pWindow, WB_BUTTONSTYLE | WB_BOTTOMBORDER ),
    mpGlobalRangeName( pGlobalRangeName ),
    maTabRangeNames( aTabRangeNames ),
    maGlobalString( ResId::toString(ScResId(STR_GLOBAL_SCOPE)))
{
    Size aBoxSize( pWindow->GetOutputSizePixel() );

    maHeaderBar.SetPosSizePixel( Point(0, 0), Size( aBoxSize.Width(), 16 ) );

    String aNameStr(ScResId(STR_HEADER_NAME));
    String aRangeStr(ScResId(STR_HEADER_RANGE));
    String aScopeStr(ScResId(STR_HEADER_SCOPE));

    long nTabSize = aBoxSize.Width()/3;
    maHeaderBar.InsertItem( ITEMID_NAME, aNameStr, nTabSize, HIB_LEFT| HIB_VCENTER );
    maHeaderBar.InsertItem( ITEMID_RANGE, aRangeStr, nTabSize, HIB_LEFT| HIB_VCENTER );
    maHeaderBar.InsertItem( ITEMID_SCOPE, aScopeStr, nTabSize, HIB_LEFT| HIB_VCENTER );

    static long nTabs[] = {3, 0, nTabSize, 2*nTabSize };
    Size aHeadSize( maHeaderBar.GetSizePixel() );

    //pParent->SetFocusControl( this );
    SetPosSizePixel( Point( 0, aHeadSize.Height() ), Size( aBoxSize.Width(), aBoxSize.Height() - aHeadSize.Height() ) );
    SetTabs( &nTabs[0], MAP_PIXEL );

    Show();
    maHeaderBar.Show();
}

void ScRangeManagerTable::addEntry(const ScRangeNameLine& rLine)
{
    SvLBoxEntry* pEntry = InsertEntryToColumn( createEntryString(rLine), LIST_APPEND, 0xffff);
    SetCurEntry(pEntry);
}

void ScRangeManagerTable::GetCurrentLine(ScRangeNameLine& rLine)
{
    SvLBoxEntry* pCurrentEntry = GetCurEntry();
    rLine.aName = GetEntryText( pCurrentEntry, 0);
    rLine.aExpression = GetEntryText(pCurrentEntry, 1);
    rLine.aScope = GetEntryText(pCurrentEntry, 2);
}

void ScRangeManagerTable::UpdateEntries()
{
    Clear();
    for (ScRangeName::iterator itr = mpGlobalRangeName->begin();
            itr != mpGlobalRangeName->end(); ++itr)
    {
        if (!itr->HasType(RT_DATABASE) && !itr->HasType(RT_SHARED))
        {
            ScRangeNameLine aLine;
            aLine.aName = itr->GetName();
            aLine.aScope = maGlobalString;
            itr->GetSymbol(aLine.aExpression);
            addEntry(aLine);
        }
    }
    for (std::map<rtl::OUString, ScRangeName*>::iterator itr = maTabRangeNames.begin();
            itr != maTabRangeNames.end(); ++itr)
    {
        ScRangeName* pLocalRangeName = itr->second;
        ScRangeNameLine aLine;
        aLine.aScope = itr->first;
        for (ScRangeName::iterator it = pLocalRangeName->begin();
                it != pLocalRangeName->end(); ++it)
        {
            if (!it->HasType(RT_DATABASE) && !it->HasType(RT_SHARED))
            {
                aLine.aName = it->GetName();
                it->GetSymbol(aLine.aExpression);
                addEntry(aLine);
            }
        }
    }
}

//logic

#define ERRORBOX(s) ErrorBox(this,WinBits(WB_OK|WB_DEF_OK),s).Execute();

ScNameDlg::ScNameDlg( SfxBindings* pB, SfxChildWindow* pCW, Window* pParent,
        ScViewData*       ptrViewData,
        const ScAddress&  aCursorPos )

:   ScAnyRefDlg ( pB, pCW, pParent, RID_SCDLG_NAMES ),
    //
    maFtScope( this, ScResId(FT_SCOPE) ),
    maFtRange( this, ScResId(FT_RANGE) ),
    maFtName( this,  ScResId(FT_NAME) ),
    maLbScope( this, ScResId(LB_SCOPE) ),
    //
    maEdAssign       ( this, this, ScResId( ED_ASSIGN ) ),
    maEdName         ( this, ScResId( ED_NAME2 ) ),
    aRbAssign       ( this, ScResId( RB_ASSIGN ), &maEdAssign, this ),
    //
    maFlDiv          ( this, ScResId( FL_DIV ) ),
    maBtnPrintArea   ( this, ScResId( BTN_PRINTAREA ) ),
    maBtnColHeader   ( this, ScResId( BTN_COLHEADER ) ),
    maBtnCriteria    ( this, ScResId( BTN_CRITERIA ) ),
    maBtnRowHeader   ( this, ScResId( BTN_ROWHEADER ) ),
    //
    maNameMgrCtrl   ( this, ScResId( CTRL_MANAGENAMES ) ),
    //
    maBtnHelp        ( this, ScResId( BTN_HELP ) ),
    maBtnAdd         ( this, ScResId( BTN_ADD ) ),
    maBtnDelete      ( this, ScResId( BTN_DELETE ) ),
    maBtnSelect      ( this, ScResId( BTN_SELECT ) ),
    maBtnOk          ( this, ScResId( BTN_CLOSE ) ),
    maBtnCancel      ( this, ScResId( BTN_CANCEL ) ),
    maBtnMore        ( this, ScResId( BTN_MORE ) ),
    //
    mErrMsgInvalidSym( ScResId( STR_INVALIDSYMBOL ) ),
    maErrMsgModifiedFailed( ResId::toString(ScResId( STR_MODIFYFAILED ) ) ),
    maGlobalNameStr( ResId::toString(ScResId(STR_GLOBAL_SCOPE)) ),
    //
    mpViewData       ( ptrViewData ),
    mpDoc            ( ptrViewData->GetDocument() ),
    maCursorPos      ( aCursorPos )
{
    Init();
    FreeResource();
}

ScNameDlg::~ScNameDlg()
{
    delete mpRangeManagerTable;
}

void ScNameDlg::Init()
{
    String  aAreaStr;
    ScRange aRange;

    OSL_ENSURE( mpViewData && mpDoc, "ViewData oder Document nicht gefunden!" );

    //init UI
    std::map<rtl::OUString,ScRangeName*> aTabRangeNameMap;
    mpDoc->GetTabRangeNameMap(aTabRangeNameMap);
    mpRangeManagerTable = new ScRangeManagerTable(&maNameMgrCtrl, mpDoc->GetRangeName(), aTabRangeNameMap);
    mpRangeManagerTable->SetSelectHdl( LINK( this, ScNameDlg, SelectionChangedHdl_Impl ) );
    mpRangeManagerTable->SetDeselectHdl( LINK( this, ScNameDlg, SelectionChangedHdl_Impl ) );

    maBtnOk.SetClickHdl  ( LINK( this, ScNameDlg, OkBtnHdl ) );
    maBtnCancel.SetClickHdl  ( LINK( this, ScNameDlg, CancelBtnHdl ) );
    maBtnAdd.SetClickHdl     ( LINK( this, ScNameDlg, AddBtnHdl ) );
    maEdAssign.SetGetFocusHdl( LINK( this, ScNameDlg, AssignGetFocusHdl ) );
    maEdAssign.SetModifyHdl  ( LINK( this, ScNameDlg, EdModifyHdl ) );
    maEdName.SetModifyHdl ( LINK( this, ScNameDlg, EdModifyHdl ) );
    maLbScope.SetSelectHdl( LINK(this, ScNameDlg, ScopeChangedHdl) );
    maBtnDelete.SetClickHdl ( LINK( this, ScNameDlg, RemoveBtnHdl ) );
    maBtnMore.SetClickHdl   ( LINK( this, ScNameDlg, MoreBtnHdl ) );

    maBtnCriteria .Hide();
    maBtnPrintArea.Hide();
    maBtnColHeader.Hide();
    maBtnRowHeader.Hide();

    maBtnMore.AddWindow( &maBtnCriteria );
    maBtnMore.AddWindow( &maBtnPrintArea );
    maBtnMore.AddWindow( &maBtnColHeader );
    maBtnMore.AddWindow( &maBtnRowHeader );

    // Initialize scope list.
    maLbScope.InsertEntry(maGlobalNameStr);
    maLbScope.SelectEntryPos(0);
    SCTAB n = mpDoc->GetTableCount();
    for (SCTAB i = 0; i < n; ++i)
    {
        rtl::OUString aTabName;
        mpDoc->GetName(i, aTabName);
        maLbScope.InsertEntry(aTabName);
    }

    UpdateNames();

    mpViewData->GetSimpleArea( aRange );
    aRange.Format( aAreaStr, ABS_DREF3D, mpDoc,
            ScAddress::Details(mpDoc->GetAddressConvention(), 0, 0) );

    maCurSel = Selection( 0, SELECTION_MAX );
    maEdAssign.GrabFocus();
    maEdAssign.SetText( aAreaStr );
    maEdAssign.SetSelection( maCurSel );

    EdModifyHdl( 0 );

}

sal_Bool ScNameDlg::IsRefInputMode() const
{
    return maEdAssign.IsEnabled();
}

void ScNameDlg::RefInputDone( sal_Bool bForced)
{
    ScAnyRefDlg::RefInputDone(bForced);
    EdModifyHdl(&maEdAssign);
}

void ScNameDlg::SetReference( const ScRange& rRef, ScDocument* pDocP )
{
    if ( maEdAssign.IsEnabled() )
    {
        if ( rRef.aStart != rRef.aEnd )
            RefInputStart(&maEdAssign);
        String aRefStr;
        rRef.Format( aRefStr, ABS_DREF3D, pDocP,
                ScAddress::Details(pDocP->GetAddressConvention(), 0, 0) );
        maEdAssign.SetRefString( aRefStr );
    }
}

sal_Bool ScNameDlg::Close()
{
    return DoClose( ScNameDlgWrapper::GetChildWindowId() );
}

void ScNameDlg::CancelPushed()
{
    DoClose( ScNameDlgWrapper::GetChildWindowId() );
}

void ScNameDlg::SetActive()
{
    maEdAssign.GrabFocus();
    RefInputDone();
}

void ScNameDlg::UpdateChecks(ScRangeData* pData)
{
    maBtnCriteria .Check( pData->HasType( RT_CRITERIA ) );
    maBtnPrintArea.Check( pData->HasType( RT_PRINTAREA ) );
    maBtnColHeader.Check( pData->HasType( RT_COLHEADER ) );
    maBtnRowHeader.Check( pData->HasType( RT_ROWHEADER ) );
}

//updates the table and the buttons
void ScNameDlg::UpdateNames()
{
    mpRangeManagerTable->UpdateEntries();

    ScRangeNameLine aLine;
    mpRangeManagerTable->GetCurrentLine(aLine);
    if (aLine.aName.getLength())
    {
        maBtnDelete.Enable();
    }
    else
    {
        maBtnDelete.Disable();
    }
}

void ScNameDlg::CalcCurTableAssign( String& aAssign, ScRangeData* pRangeData )
{
    if ( pRangeData )
    {
        rtl::OUStringBuffer sBuffer;
        pRangeData->UpdateSymbol( sBuffer, maCursorPos );
        aAssign = sBuffer.makeStringAndClear();
    }
    else
    {
        aAssign.Erase();
    }
}

void ScNameDlg::ShowOptions(const ScRangeNameLine& rLine)
{
    ScRangeName* pRangeName = GetRangeName(rLine.aScope, mpDoc);
    ScRangeData* pData = pRangeName->findByName(rLine.aName);
    if (pData)
    {
        UpdateChecks(pData);
    }
}


bool ScNameDlg::AddPushed()
{
    if (!mpDoc)
        return false;

    rtl::OUString aName = maEdName.GetText();
    aName = aName.trim();
    if (!aName.getLength())
        return false;

    if (!ScRangeData::IsNameValid( aName, mpDoc ))
    {
        ERRORBOX( ScGlobal::GetRscString(STR_INVALIDNAME));
        return false;
    }

    rtl::OUString aScope = maLbScope.GetSelectEntry();
    rtl::OUString aExpr = maEdAssign.GetText();
    ScRangeName* pRangeName = GetRangeName(aScope, mpDoc);

    RangeType nType = RT_NAME |
         (maBtnRowHeader.IsChecked() ? RT_ROWHEADER : RangeType(0))
        |(maBtnColHeader.IsChecked() ? RT_COLHEADER : RangeType(0))
        |(maBtnPrintArea.IsChecked() ? RT_PRINTAREA : RangeType(0))
        |(maBtnCriteria.IsChecked()  ? RT_CRITERIA  : RangeType(0));

    ScRangeData* pNewEntry = new ScRangeData( mpDoc, aName, aExpr,
                                            maCursorPos, nType);
    if ( 0 == pNewEntry->GetErrCode() )
    {
        if (!pRangeName->insert( pNewEntry))
        {
            pNewEntry = NULL;
        }
        else
        {
            maEdName.SetText(EMPTY_STRING);
            maBtnAdd.Disable();
            UpdateNames();
            SFX_APP()->Broadcast( SfxSimpleHint( SC_HINT_AREAS_CHANGED ) );
        }
    }
    else
    {
        delete pNewEntry;
        ERRORBOX( mErrMsgInvalidSym );
    }
    return true;
}

void ScNameDlg::RemovePushed()
{
    ScRangeNameLine aLine;
    mpRangeManagerTable->GetCurrentLine(aLine);
    rtl::OUString aName = aLine.aName;
    rtl::OUString aScope = aLine.aScope;
    rtl::OUString aExpr = aLine.aExpression;
    ScRangeName* pRangeName = GetRangeName(aScope, mpDoc);

    ScRangeData* pData = pRangeName->findByName(aName);
    if (pData)
    {
        sal_Int32 nIndex = 0;
        rtl::OUString aStrDelMsg = ScGlobal::GetRscString( STR_QUERY_DELENTRY );
        rtl::OUStringBuffer aMsg = aStrDelMsg.getToken( 0, '#', nIndex );
        aMsg.append(aName);
        aMsg.append(aStrDelMsg.getToken( 1, '#', nIndex));

        if (RET_YES ==
                QueryBox( this, WinBits( WB_YES_NO | WB_DEF_YES ), aMsg.makeStringAndClear() ).Execute() )
        {
            pRangeName->erase(*pData);
            UpdateNames();
            SFX_APP()->Broadcast( SfxSimpleHint( SC_HINT_AREAS_CHANGED ) );
        }
    }
}

void ScNameDlg::NameModified()
{
    rtl::OUString aName = maEdName.GetText();
    aName = aName.trim();
    rtl::OUString aExpr = maEdAssign.GetText();
    rtl::OUString aScope = maLbScope.GetSelectEntry();

    if (!aName.getLength() || !ScRangeData::IsNameValid(aName, mpDoc))
    {
        maBtnAdd.Disable();
    }
    else
    {
        ScRangeName* pRangeName = GetRangeName(aScope, mpDoc);
        if (pRangeName->findByName(aName))
        {
            maBtnAdd.Disable();
        }
        else
        {
            maBtnAdd.Enable();
        }
        ScRangeNameLine aLine;
        mpRangeManagerTable->GetCurrentLine(aLine);
    }
}

void ScNameDlg::SelectionChanged()
{
    ScRangeNameLine aLine;
    mpRangeManagerTable->GetCurrentLine(aLine);
    maEdAssign.SetText(aLine.aExpression);
    maEdName.SetText(aLine.aName);
    maLbScope.SelectEntry(aLine.aScope);
    ShowOptions(aLine);
    maBtnDelete.Enable();
}

void ScNameDlg::ScopeChanged()
{
    NameModified();
}

namespace {

void MoveWindow( Window& rButton, long nPixel)
{
    Point aPoint = rButton.GetPosPixel();
    aPoint.Y() += nPixel;
    rButton.SetPosPixel(aPoint);
}

}

void ScNameDlg::MorePushed()
{
    //depending on the state of the button, move all elements below up/down
    long nPixel = 85;
    if (!maBtnMore.GetState())
    {
        nPixel *= -1;
    }
    MoveWindow(maBtnAdd, nPixel);
    MoveWindow(maBtnDelete, nPixel);
    MoveWindow(maBtnHelp, nPixel);
    MoveWindow(maBtnOk, nPixel);
    MoveWindow(maBtnCancel, nPixel);
    MoveWindow(maBtnSelect, nPixel);
    MoveWindow(maFlDiv, nPixel);
}

IMPL_LINK( ScNameDlg, OkBtnHdl, void *, EMPTYARG )
{
    Close();
    return 0;
}

IMPL_LINK( ScNameDlg, CancelBtnHdl, void *, EMPTYARG )
{
    CancelPushed();
    return 0;
}

IMPL_LINK( ScNameDlg, AddBtnHdl, void *, EMPTYARG )
{
    return AddPushed();
}

IMPL_LINK( ScNameDlg, RemoveBtnHdl, void *, EMPTYARG )
{
    RemovePushed();
    return 0;
}

IMPL_LINK( ScNameDlg, EdModifyHdl, void *, EMPTYARG )
{
    NameModified();
    return 0;
}

IMPL_LINK( ScNameDlg, AssignGetFocusHdl, void *, EMPTYARG )
{
    EdModifyHdl( &maEdAssign );
    return 0;
}

IMPL_LINK( ScNameDlg, SelectionChangedHdl_Impl, void *, EMPTYARG )
{
    SelectionChanged();
    return 0;
}

IMPL_LINK( ScNameDlg, ScopeChangedHdl, void*, EMPTYARG )
{
    ScopeChanged();
    return 0;
}

IMPL_LINK( ScNameDlg, MoreBtnHdl, void*, EMPTYARG )
{
    MorePushed();
    return 0;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
