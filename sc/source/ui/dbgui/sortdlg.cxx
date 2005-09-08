/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sortdlg.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 20:42:24 $
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

#undef SC_DLLIMPLEMENTATION

#ifdef PCH
#include "ui_pch.hxx"
#endif

#pragma hdrstop

#include "tpsort.hxx"
#include "sortdlg.hxx"
#include "scresid.hxx"
#include "sortdlg.hrc"

#pragma hdrstop

//==================================================================

ScSortDlg::ScSortDlg( Window*           pParent,
                      const SfxItemSet* pArgSet ) :
        SfxTabDialog( pParent,
                      ScResId( RID_SCDLG_SORT ),
                      pArgSet ),
        bIsHeaders  ( FALSE ),
        bIsByRows   ( FALSE )

{
    AddTabPage( TP_FIELDS,  ScTabPageSortFields::Create,  0 );
    AddTabPage( TP_OPTIONS, ScTabPageSortOptions::Create, 0 );
    FreeResource();
}

__EXPORT ScSortDlg::~ScSortDlg()
{
}

