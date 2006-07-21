/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: filldlg.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: kz $ $Date: 2006-07-21 14:05:23 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sc.hxx"

#undef SC_DLLIMPLEMENTATION



//----------------------------------------------------------------------------

#include <svtools/zforlist.hxx>
#include <vcl/msgbox.hxx>
#include <limits.h>

#include "scresid.hxx"
#include "document.hxx"
#include "miscdlgs.hrc"

#define _FILLDLG_CXX
#include "filldlg.hxx"
#undef _FILLDLG_CXX



//============================================================================
// class ScFillSeriesDlg

//----------------------------------------------------------------------------

ScFillSeriesDlg::ScFillSeriesDlg( Window*       pParent,
                                  ScDocument&   rDocument,
                                  FillDir       eFillDir,
                                  FillCmd       eFillCmd,
                                  FillDateCmd   eFillDateCmd,
                                  String        aStartStr,
                                  double        fStep,
                                  double        fMax,
                                  USHORT        nPossDir )

    :   ModalDialog     ( pParent, ScResId( RID_SCDLG_FILLSERIES ) ),

        aFlDirection    ( this, ScResId( FL_DIRECTION ) ),
        aBtnLeft        ( this, ScResId( BTN_LEFT ) ),
        aBtnRight       ( this, ScResId( BTN_RIGHT ) ),
        aBtnUp          ( this, ScResId( BTN_TOP ) ),
        aBtnDown        ( this, ScResId( BTN_BOTTOM ) ),
        aFlSep1         ( this, ScResId( FL_SEP1 ) ),
        aFlType         ( this, ScResId( FL_TYPE ) ),
        aBtnArithmetic  ( this, ScResId( BTN_ARITHMETIC ) ),
        aBtnGeometric   ( this, ScResId( BTN_GEOMETRIC ) ),
        aBtnDate        ( this, ScResId( BTN_DATE ) ),
        aBtnAutoFill    ( this, ScResId( BTN_AUTOFILL ) ),
        aFlSep2         ( this, ScResId( FL_SEP2 ) ),
        aFlTimeUnit     ( this, ScResId( FL_TIME_UNIT ) ),
        aBtnDay         ( this, ScResId( BTN_DAY ) ),
        aBtnDayOfWeek   ( this, ScResId( BTN_DAY_OF_WEEK ) ),
        aBtnMonth       ( this, ScResId( BTN_MONTH ) ),
        aBtnYear        ( this, ScResId( BTN_YEAR ) ),

        aFtStartVal     ( this, ScResId( FT_START_VALUE ) ),
        aEdStartVal     ( this, ScResId( ED_START_VALUES ) ),
        aFtEndVal       ( this, ScResId( FT_END_VALUE ) ),
        aEdEndVal       ( this, ScResId( ED_END_VALUES ) ),
        aFtIncrement    ( this, ScResId( FT_INCREMENT ) ),
        aEdIncrement    ( this, ScResId( ED_INCREMENT ) ),

        aBtnOk          ( this, ScResId( BTN_OK ) ),
        aBtnCancel      ( this, ScResId( BTN_CANCEL ) ),
        aBtnHelp        ( this, ScResId( BTN_HELP ) ),
        errMsgInvalidVal( ScResId( STR_VALERR ) ),
        rDoc            ( rDocument ),
        theFillDir      ( eFillDir ),
        theFillCmd      ( eFillCmd ),
        theFillDateCmd  ( eFillDateCmd ),
        aStartStrVal    ( aStartStr),
        fIncrement      ( fStep ),
        fEndVal         ( fMax )
{
    Init( nPossDir );
    FreeResource();
}


//----------------------------------------------------------------------------

__EXPORT ScFillSeriesDlg::~ScFillSeriesDlg()
{
}

//----------------------------------------------------------------------------

void ScFillSeriesDlg::SetEdStartValEnabled(BOOL bFlag)
{
    bStartValFlag=bFlag;
    if(bFlag)
    {
       aFtStartVal.Enable();
       aEdStartVal.Enable();
    }
    else
    {
       aFtStartVal.Disable();
       aEdStartVal.Disable();
    }
}

//----------------------------------------------------------------------------

void ScFillSeriesDlg::Init( USHORT nPossDir )
{
    aBtnOk.SetClickHdl         ( LINK( this, ScFillSeriesDlg, OKHdl ) );
    aBtnArithmetic.SetClickHdl ( LINK( this, ScFillSeriesDlg, DisableHdl ) );
    aBtnGeometric.SetClickHdl  ( LINK( this, ScFillSeriesDlg, DisableHdl ) );
    aBtnDate.SetClickHdl       ( LINK( this, ScFillSeriesDlg, DisableHdl ) );
    aBtnAutoFill.SetClickHdl   ( LINK( this, ScFillSeriesDlg, DisableHdl ) );

    if( nPossDir == FDS_OPT_NONE )
    {
        aBtnLeft.Disable();
        aBtnRight.Disable();
        aBtnDown.Disable();
        aBtnUp.Disable();
    }

    if( nPossDir == FDS_OPT_HORZ )
    {
        aBtnDown.Disable();
        aBtnUp.Disable();
    }

    if( nPossDir == FDS_OPT_VERT )
    {
        aBtnLeft.Disable();
        aBtnRight.Disable();
    }

    switch ( theFillDir )
    {
        case FILL_TO_LEFT:      aBtnLeft.Check();   break;
        case FILL_TO_RIGHT:     aBtnRight.Check();  break;
        case FILL_TO_BOTTOM:    aBtnDown.Check();   break;
        case FILL_TO_TOP:       aBtnUp.Check();     break;
        default:
        break;
    }

    switch ( theFillCmd )
    {
        case FILL_LINEAR:
            aBtnArithmetic.Check();
            DisableHdl( &aBtnArithmetic );
            break;
        case FILL_GROWTH:
            aBtnGeometric.Check();
            DisableHdl( &aBtnGeometric );
            break;
        case FILL_DATE:
            aBtnDate.Check();
            DisableHdl( &aBtnDate );
            break;
        case FILL_AUTO:
            aBtnAutoFill.Check();
            DisableHdl( &aBtnAutoFill );
            break;
        default:
        break;
    }

    switch ( theFillDateCmd )
    {
        case FILL_DAY:          aBtnDay.Check();        break;
        case FILL_WEEKDAY:      aBtnDayOfWeek.Check();  break;
        case FILL_MONTH:        aBtnMonth.Check();      break;
        case FILL_YEAR:         aBtnYear.Check();       break;
        default:
        break;
    }

    fStartVal = MAXDOUBLE;
    /*
    String aStartTxt;
    if ( fStartVal != MAXDOUBLE )
        rDoc.GetFormatTable()->GetInputLineString( fStartVal, 0, aStartTxt );
    aEdStartVal.SetText( aStartTxt );
    */

    aEdStartVal.SetText( aStartStrVal);

    String aIncrTxt;
    rDoc.GetFormatTable()->GetInputLineString( fIncrement, 0, aIncrTxt );
    aEdIncrement.SetText( aIncrTxt );

    String aEndTxt;
    if ( fEndVal != MAXDOUBLE )
        rDoc.GetFormatTable()->GetInputLineString( fEndVal, 0, aEndTxt );
    aEdEndVal.SetText( aEndTxt );

    bStartValFlag=FALSE;

    aFlSep1.SetStyle( aFlSep1.GetStyle() | WB_VERT );
    aFlSep2.SetStyle( aFlSep2.GetStyle() | WB_VERT );
}


//----------------------------------------------------------------------------

BOOL __EXPORT ScFillSeriesDlg::CheckStartVal()
{
    BOOL bValOk = FALSE;
    sal_uInt32 nKey = 0;
    String aStr( aEdStartVal.GetText() );

    if ( aStr.Len() == 0 || aBtnAutoFill.IsChecked())
    {
        fStartVal = MAXDOUBLE;
        bValOk = TRUE;
    }
    else
        bValOk = rDoc.GetFormatTable()->IsNumberFormat( aStr, nKey, fStartVal );

    return bValOk;
}


//----------------------------------------------------------------------------

BOOL __EXPORT ScFillSeriesDlg::CheckIncrementVal()
{
    sal_uInt32 nKey = 0;
    String aStr( aEdIncrement.GetText() );

    return rDoc.GetFormatTable()->IsNumberFormat( aStr, nKey, fIncrement );
}


//----------------------------------------------------------------------------

BOOL __EXPORT ScFillSeriesDlg::CheckEndVal()
{
    BOOL    bValOk = FALSE;
    sal_uInt32 nKey = 0;
    String  aStr( aEdEndVal.GetText() );

    if ( aStr.Len() == 0 )
    {
        fEndVal = (fIncrement < 0) ? -MAXDOUBLE : MAXDOUBLE;
        bValOk  = TRUE;
    }
    else
        bValOk = rDoc.GetFormatTable()->IsNumberFormat( aStr, nKey, fEndVal );

    return bValOk;
}


//----------------------------------------------------------------------------
// Handler:
//----------------------------------------------------------------------------

IMPL_LINK( ScFillSeriesDlg, DisableHdl, Button *, pBtn )
{
    if ( pBtn == &aBtnDate )
    {
        aBtnDay.Enable();
        aBtnDayOfWeek.Enable();
        aBtnMonth.Enable();
        aBtnYear.Enable();
        aFlTimeUnit.Enable();
    }
    else
    {
        aBtnDay.Disable();
        aBtnDayOfWeek.Disable();
        aBtnMonth.Disable();
        aBtnYear.Disable();
        aFlTimeUnit.Disable();
    }

    if ( pBtn != &aBtnAutoFill )
    {
        aFtIncrement.Enable();
        aEdIncrement.Enable();
        aFtEndVal.Enable();
        aEdEndVal.Enable();
    }
    else
    {
        aFtIncrement.Disable();
        aEdIncrement.Disable();
        aFtEndVal.Disable();
        aEdEndVal.Disable();
    }
    return 0;
}


//----------------------------------------------------------------------------

IMPL_LINK( ScFillSeriesDlg, OKHdl, void *, EMPTYARG )
{
    if ( aBtnLeft.IsChecked() )             theFillDir = FILL_TO_LEFT;
    else if ( aBtnRight.IsChecked() )       theFillDir = FILL_TO_RIGHT;
    else if ( aBtnDown.IsChecked() )        theFillDir = FILL_TO_BOTTOM;
    else if ( aBtnUp.IsChecked() )          theFillDir = FILL_TO_TOP;

    if ( aBtnArithmetic.IsChecked() )       theFillCmd = FILL_LINEAR;
    else if ( aBtnGeometric.IsChecked() )   theFillCmd = FILL_GROWTH;
    else if ( aBtnDate.IsChecked() )        theFillCmd = FILL_DATE;
    else if ( aBtnAutoFill.IsChecked() )    theFillCmd = FILL_AUTO;

    if ( aBtnDay.IsChecked() )              theFillDateCmd = FILL_DAY;
    else if ( aBtnDayOfWeek.IsChecked() )   theFillDateCmd = FILL_WEEKDAY;
    else if ( aBtnMonth.IsChecked() )       theFillDateCmd = FILL_MONTH;
    else if ( aBtnYear.IsChecked() )        theFillDateCmd = FILL_YEAR;

    BOOL  bAllOk = TRUE;
    Edit* pEdWrong;
    if ( !CheckStartVal() )
    {
        bAllOk = FALSE;
        pEdWrong = &aEdStartVal;
    }
    else if ( !CheckIncrementVal() )
    {
        bAllOk = FALSE;
        pEdWrong = &aEdIncrement;
    }
    else if ( !CheckEndVal() )
    {
        bAllOk = FALSE;
        pEdWrong = &aEdEndVal;
    }
    if ( bAllOk )
        EndDialog( RET_OK );
    else
    {
        ErrorBox( this,
                  WinBits( WB_OK | WB_DEF_OK ),
                  errMsgInvalidVal
                ).Execute();
        pEdWrong->GrabFocus();
    }

    return 0;
}




