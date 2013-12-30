/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <vcl/msgbox.hxx>
#include <sfx2/app.hxx>

#include "reffact.hxx"
#include "document.hxx"
#include "scresid.hxx"
#include "globstr.hrc"
#include "rangenam.hxx"
#include "simpref.hxx"
#include "scmod.hxx"

//============================================================================

#define ABS_SREF          SCA_VALID \
                        | SCA_COL_ABSOLUTE | SCA_ROW_ABSOLUTE | SCA_TAB_ABSOLUTE
#define ABS_DREF          ABS_SREF \
                        | SCA_COL2_ABSOLUTE | SCA_ROW2_ABSOLUTE | SCA_TAB2_ABSOLUTE
#define ABS_DREF3D      ABS_DREF | SCA_TAB_3D

ScSimpleRefDlg::ScSimpleRefDlg(SfxBindings* pB, SfxChildWindow* pCW, Window* pParent,
    ScViewData* ptrViewData)
    : ScAnyRefDlg(pB, pCW, pParent, "SimpleRefDialog", "modules/scalc/ui/simplerefdialog.ui")
,

        pDoc            ( ptrViewData->GetDocument() ),
        bAutoReOpen     ( sal_True ),
        bCloseOnButtonUp( false ),
        bSingleCell     ( false ),
        bMultiSelection ( false )
{
    get(m_pFtAssign, "area");
    get(m_pEdAssign, "assign");
    m_pEdAssign->SetReferences(this, m_pFtAssign);
    get(m_pRbAssign, "assignref");
    m_pRbAssign->SetReferences(this, m_pEdAssign);

    get(m_pBtnOk, "ok");
    get(m_pBtnCancel, "cancel");

    //  damit die Strings in der Resource bei den FixedTexten bleiben koennen:
    Init();
    SetDispatcherLock( sal_True ); // Modal-Modus einschalten
}

//----------------------------------------------------------------------------
ScSimpleRefDlg::~ScSimpleRefDlg()
{
    SetDispatcherLock( false ); // Modal-Modus einschalten
}

//----------------------------------------------------------------------------
void ScSimpleRefDlg::FillInfo(SfxChildWinInfo& rWinInfo) const
{
    ScAnyRefDlg::FillInfo(rWinInfo);
    rWinInfo.bVisible=bAutoReOpen;
}

//----------------------------------------------------------------------------
void ScSimpleRefDlg::SetRefString(const OUString &rStr)
{
    m_pEdAssign->SetText(rStr);
}

//----------------------------------------------------------------------------
void ScSimpleRefDlg::Init()
{
    m_pBtnOk->SetClickHdl      ( LINK( this, ScSimpleRefDlg, OkBtnHdl ) );
    m_pBtnCancel->SetClickHdl  ( LINK( this, ScSimpleRefDlg, CancelBtnHdl ) );
    bCloseFlag=false;
}

//----------------------------------------------------------------------------
// Uebergabe eines mit der Maus selektierten Tabellenbereiches, der dann als
//  neue Selektion im Referenz-Fenster angezeigt wird.
void ScSimpleRefDlg::SetReference( const ScRange& rRef, ScDocument* pDocP )
{
    if ( m_pEdAssign->IsEnabled() )
    {
        if ( rRef.aStart != rRef.aEnd )
            RefInputStart(m_pEdAssign);

        theCurArea = rRef;
        OUString aRefStr;
        if ( bSingleCell )
        {
            ScAddress aAdr = rRef.aStart;
            aRefStr = aAdr.Format(SCA_ABS_3D, pDocP, pDocP->GetAddressConvention());
        }
        else
            aRefStr = theCurArea.Format(ABS_DREF3D, pDocP, pDocP->GetAddressConvention());

        if ( bMultiSelection )
        {
            OUString aVal = m_pEdAssign->GetText();
            Selection aSel = m_pEdAssign->GetSelection();
            aSel.Justify();
            aVal = aVal.replaceAt( aSel.Min(), aSel.Len(), aRefStr );
            Selection aNewSel( aSel.Min(), aSel.Min()+aRefStr.getLength() );
            m_pEdAssign->SetRefString( aVal );
            m_pEdAssign->SetSelection( aNewSel );
        }
        else
            m_pEdAssign->SetRefString( aRefStr );

        aChangeHdl.Call( &aRefStr );
    }
}


//----------------------------------------------------------------------------
sal_Bool ScSimpleRefDlg::Close()
{
    CancelBtnHdl(m_pBtnCancel);
    return sal_True;
}

//------------------------------------------------------------------------
void ScSimpleRefDlg::SetActive()
{
    m_pEdAssign->GrabFocus();

    //  kein NameModifyHdl, weil sonst Bereiche nicht geaendert werden koennen
    //  (nach dem Aufziehen der Referenz wuerde der alte Inhalt wieder angezeigt)
    //  (der ausgewaehlte DB-Name hat sich auch nicht veraendert)

    RefInputDone();
}
//------------------------------------------------------------------------
sal_Bool ScSimpleRefDlg::IsRefInputMode() const
{
    return sal_True;
}

OUString ScSimpleRefDlg::GetRefString() const
{
    return m_pEdAssign->GetText();
}

void ScSimpleRefDlg::SetCloseHdl( const Link& rLink )
{
    aCloseHdl=rLink;
}

void ScSimpleRefDlg::SetUnoLinks( const Link& rDone, const Link& rAbort,
                                    const Link& rChange )
{
    aDoneHdl    = rDone;
    aAbortedHdl = rAbort;
    aChangeHdl  = rChange;
}

void ScSimpleRefDlg::SetFlags( sal_Bool bSetCloseOnButtonUp, sal_Bool bSetSingleCell, sal_Bool bSetMultiSelection )
{
    bCloseOnButtonUp = bSetCloseOnButtonUp;
    bSingleCell = bSetSingleCell;
    bMultiSelection = bSetMultiSelection;
}

void ScSimpleRefDlg::StartRefInput()
{
    if ( bMultiSelection )
    {
        // initially select the whole string, so it gets replaced by default
        m_pEdAssign->SetSelection( Selection( 0, m_pEdAssign->GetText().getLength() ) );
    }

    m_pRbAssign->DoRef();
    bCloseFlag=sal_True;
}

void ScSimpleRefDlg::RefInputDone( sal_Bool bForced)
{
    ScAnyRefDlg::RefInputDone(bForced);
    if ( (bForced || bCloseOnButtonUp) && bCloseFlag )
        OkBtnHdl(m_pBtnOk);
}
//------------------------------------------------------------------------
// Handler:
// ========
IMPL_LINK_NOARG(ScSimpleRefDlg, OkBtnHdl)
{
    bAutoReOpen=false;
    OUString aResult=m_pEdAssign->GetText();
    aCloseHdl.Call(&aResult);
    Link aUnoLink = aDoneHdl;       // stack var because this is deleted in DoClose
    DoClose( ScSimpleRefDlgWrapper::GetChildWindowId() );
    aUnoLink.Call( &aResult );
    return 0;
}

//------------------------------------------------------------------------
IMPL_LINK_NOARG(ScSimpleRefDlg, CancelBtnHdl)
{
    bAutoReOpen=false;
    OUString aResult=m_pEdAssign->GetText();
    aCloseHdl.Call(NULL);
    Link aUnoLink = aAbortedHdl;    // stack var because this is deleted in DoClose
    DoClose( ScSimpleRefDlgWrapper::GetChildWindowId() );
    aUnoLink.Call( &aResult );
    return 0;
}



//------------------------------------------------------------------------

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
