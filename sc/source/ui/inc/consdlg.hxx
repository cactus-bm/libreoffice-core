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

#ifndef SC_CONSDLG_HXX
#define SC_CONSDLG_HXX

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif
#ifndef _MOREBTN_HXX //autogen
#include <vcl/morebtn.hxx>
#endif
#include "global.hxx"
#include "anyrefdg.hxx"

class ScViewData;
class ScDocument;
class ScRangeUtil;
class ScAreaData;


//============================================================================

class ScConsolidateDlg : public ScAnyRefDlg
{
public:
                    ScConsolidateDlg( SfxBindings* pB, SfxChildWindow* pCW, Window* pParent,
                                      const SfxItemSet& rArgSet );
                    ~ScConsolidateDlg();

    virtual void    SetReference( const ScRange& rRef, ScDocument* pDoc );

    virtual BOOL    IsRefInputMode() const { return TRUE; }
    virtual void    SetActive();

    virtual BOOL    Close();

protected:
    virtual void    Deactivate();

private:
    FixedText       aFtFunc;
    ListBox         aLbFunc;

    FixedText       aFtConsAreas;
    MultiListBox    aLbConsAreas;

    ListBox         aLbDataArea;
    FixedText       aFtDataArea;
    formula::RefEdit        aEdDataArea;
    formula::RefButton      aRbDataArea;

    ListBox         aLbDestArea;
    FixedText       aFtDestArea;
    formula::RefEdit        aEdDestArea;
    formula::RefButton      aRbDestArea;

    FixedLine       aFlConsBy;
    CheckBox        aBtnByRow;
    CheckBox        aBtnByCol;

    FixedLine       aFlSep;
    FixedLine       aFlOptions;
    CheckBox        aBtnRefs;

    OKButton        aBtnOk;
    CancelButton    aBtnCancel;
    HelpButton      aBtnHelp;
    PushButton      aBtnAdd;
    PushButton      aBtnRemove;
    MoreButton      aBtnMore;

    String          aStrUndefined;

    ScConsolidateParam  theConsData;
    ScViewData*         pViewData;
    ScDocument*         pDoc;
    ScRangeUtil*        pRangeUtil;
    ScAreaData*         pAreaData;
    USHORT              nAreaDataCount;
    USHORT              nWhichCons;

    formula::RefEdit*           pRefInputEdit;
    BOOL                bDlgLostFocus;

#ifdef _CONSDLG_CXX
private:
    void Init               ();
    void FillAreaLists      ();
    BOOL VerifyEdit         ( formula::RefEdit* pEd );

    DECL_LINK( OkHdl,        void* );
    DECL_LINK( ClickHdl,     PushButton* );
    DECL_LINK( GetFocusHdl, Control* );
    DECL_LINK( ModifyHdl,    formula::RefEdit* );
    DECL_LINK( SelectHdl,    ListBox* );

    ScSubTotalFunc  LbPosToFunc( USHORT nPos );
    USHORT          FuncToLbPos( ScSubTotalFunc eFunc );
#endif // _CONSDLG_CXX
};



#endif // SC_CONSDLG_HXX

