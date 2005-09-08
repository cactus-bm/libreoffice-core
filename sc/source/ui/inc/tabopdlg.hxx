/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: tabopdlg.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 21:55:25 $
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

#ifndef SC_TABOPDLG_HXX
#define SC_TABOPDLG_HXX

#ifndef _SV_FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _SV_GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif

#ifndef SC_ADDRESS_HXX
#include "address.hxx"
#endif

#ifndef SC_ANYREFDG_HXX
#include "anyrefdg.hxx"
#endif


//------------------------------------------------------------------------

enum ScTabOpErr
{
    TABOPERR_NOFORMULA = 1,
    TABOPERR_NOCOLROW,
    TABOPERR_WRONGFORMULA,
    TABOPERR_WRONGROW,
    TABOPERR_NOCOLFORMULA,
    TABOPERR_WRONGCOL,
    TABOPERR_NOROWFORMULA
};

//========================================================================

class ScTabOpDlg : public ScAnyRefDlg
{
public:
                    ScTabOpDlg( SfxBindings* pB, SfxChildWindow* pCW, Window* pParent,
                                ScDocument*     pDocument,
                                const ScRefAddress& rCursorPos );
                    ~ScTabOpDlg();

    virtual void    SetReference( const ScRange& rRef, ScDocument* pDoc );
    virtual BOOL    IsRefInputMode() const { return TRUE; }
    virtual void    SetActive();

    virtual BOOL    Close();

private:
    FixedLine       aFlVariables;
    FixedText       aFtFormulaRange;
    ScRefEdit       aEdFormulaRange;
    ScRefButton     aRBFormulaRange;

    FixedText       aFtRowCell;
    ScRefEdit       aEdRowCell;
    ScRefButton     aRBRowCell;

    FixedText       aFtColCell;
    ScRefEdit       aEdColCell;
    ScRefButton     aRBColCell;

    OKButton        aBtnOk;
    CancelButton    aBtnCancel;
    HelpButton      aBtnHelp;

    ScRefAddress    theFormulaCell;
    ScRefAddress    theFormulaEnd;
    ScRefAddress    theRowCell;
    ScRefAddress    theColCell;

    ScDocument*     pDoc;
    const SCTAB     nCurTab;
    ScRefEdit*      pEdActive;
    BOOL            bDlgLostFocus;
    const String    errMsgNoFormula;
    const String    errMsgNoColRow;
    const String    errMsgWrongFormula;
    const String    errMsgWrongRowCol;
    const String    errMsgNoColFormula;
    const String    errMsgNoRowFormula;

#ifdef _TABOPDLG_CXX
    void    Init();
    void    RaiseError( ScTabOpErr eError );

    DECL_LINK( BtnHdl, PushButton* );
    DECL_LINK( GetFocusHdl, Control* );
    DECL_LINK( LoseFocusHdl, Control* );
#endif  // _TABOPDLG_CXX
};

#endif // SC_TABOPDLG_HXX



