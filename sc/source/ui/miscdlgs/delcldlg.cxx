/*************************************************************************
 *
 *  $RCSfile: delcldlg.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2004-08-23 09:35:59 $
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

#undef SC_DLLIMPLEMENTATION

#ifdef PCH
#include "ui_pch.hxx"
#endif

#pragma hdrstop

//------------------------------------------------------------------

#include "delcldlg.hxx"
#include "scresid.hxx"
#include "miscdlgs.hrc"


static BYTE nDelItemChecked=0;

//==================================================================

ScDeleteCellDlg::ScDeleteCellDlg( Window* pParent, BOOL bDisallowCellMove ) :
    ModalDialog     ( pParent, ScResId( RID_SCDLG_DELCELL ) ),
    //
    aBtnOk          ( this, ScResId( BTN_OK ) ),
    aBtnCancel      ( this, ScResId( BTN_CANCEL ) ),
    aBtnHelp        ( this, ScResId( BTN_HELP ) ),
    aBtnCellsUp     ( this, ScResId( BTN_CELLSUP ) ),
    aBtnCellsLeft   ( this, ScResId( BTN_CELLSLEFT ) ),
    aBtnDelRows     ( this, ScResId( BTN_DELROWS ) ),
    aBtnDelCols     ( this, ScResId( BTN_DELCOLS ) ),
    aFlFrame        ( this, ScResId( FL_FRAME ) )
{

    if (bDisallowCellMove)
    {
        aBtnCellsUp.Disable();
        aBtnCellsLeft.Disable();

        switch(nDelItemChecked)
        {
            case 2: aBtnDelRows.Check();break;
            case 3: aBtnDelCols.Check();break;
            default:aBtnDelRows.Check();break;
        }
    }
    else
    {
        switch(nDelItemChecked)
        {
            case 0: aBtnCellsUp.Check();break;
            case 1: aBtnCellsLeft.Check();break;
            case 2: aBtnDelRows.Check();break;
            case 3: aBtnDelCols.Check();break;
        }
    }

    FreeResource();
}

//------------------------------------------------------------------------

DelCellCmd ScDeleteCellDlg::GetDelCellCmd() const
{
    DelCellCmd nReturn = DEL_NONE;

    if ( aBtnCellsUp.IsChecked()   )
    {
        nDelItemChecked=0;
        nReturn = DEL_CELLSUP;
    }
    else if ( aBtnCellsLeft.IsChecked() )
    {
        nDelItemChecked=1;
        nReturn = DEL_CELLSLEFT;
    }
    else if ( aBtnDelRows.IsChecked()   )
    {
        nDelItemChecked=2;
        nReturn = DEL_DELROWS;
    }
    else if ( aBtnDelCols.IsChecked()   )
    {
        nDelItemChecked=3;
        nReturn = DEL_DELCOLS;
    }

    return nReturn;
}

__EXPORT ScDeleteCellDlg::~ScDeleteCellDlg()
{
}



