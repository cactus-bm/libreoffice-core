/*************************************************************************
 *
 *  $RCSfile: prnsave.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2003-12-01 09:51:01 $
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

#ifdef PCH
#include "core_pch.hxx"
#endif

#pragma hdrstop

// INCLUDE ---------------------------------------------------------------

#include <tools/debug.hxx>

#include "prnsave.hxx"
#include "global.hxx"

// STATIC DATA -----------------------------------------------------------

//------------------------------------------------------------------

//
//      Daten pro Tabelle
//

ScPrintSaverTab::ScPrintSaverTab() :
    mpRepeatCol(NULL),
    mpRepeatRow(NULL),
    mbEntireSheet(FALSE)
{
}

ScPrintSaverTab::~ScPrintSaverTab()
{
    delete mpRepeatCol;
    delete mpRepeatRow;
}

void ScPrintSaverTab::SetAreas( const ScRangeVec& rRanges, BOOL bEntireSheet )
{
    maPrintRanges = rRanges;
    mbEntireSheet = bEntireSheet;
}

void ScPrintSaverTab::SetRepeat( const ScRange* pCol, const ScRange* pRow )
{
    delete mpRepeatCol;
    mpRepeatCol = pCol ? new ScRange(*pCol) : NULL;
    delete mpRepeatRow;
    mpRepeatRow = pRow ? new ScRange(*pRow) : NULL;
}

inline BOOL PtrEqual( const ScRange* p1, const ScRange* p2 )
{
    return ( !p1 && !p2 ) || ( p1 && p2 && *p1 == *p2 );
}

BOOL ScPrintSaverTab::operator==( const ScPrintSaverTab& rCmp ) const
{
    return
        PtrEqual( mpRepeatCol, rCmp.mpRepeatCol ) &&
        PtrEqual( mpRepeatRow, rCmp.mpRepeatRow ) &&
        (mbEntireSheet == rCmp.mbEntireSheet) &&
        (maPrintRanges == rCmp.maPrintRanges);
}

//
//      Daten fuer das ganze Dokument
//

ScPrintRangeSaver::ScPrintRangeSaver( USHORT nCount ) :
    nTabCount( nCount )
{
    if (nCount)
        pData = new ScPrintSaverTab[nCount];
    else
        pData = NULL;
}

ScPrintRangeSaver::~ScPrintRangeSaver()
{
    delete[] pData;
}

ScPrintSaverTab& ScPrintRangeSaver::GetTabData(USHORT nTab)
{
    DBG_ASSERT(nTab<nTabCount,"ScPrintRangeSaver Tab zu gross");
    return pData[nTab];
}

const ScPrintSaverTab& ScPrintRangeSaver::GetTabData(USHORT nTab) const
{
    DBG_ASSERT(nTab<nTabCount,"ScPrintRangeSaver Tab zu gross");
    return pData[nTab];
}

BOOL ScPrintRangeSaver::operator==( const ScPrintRangeSaver& rCmp ) const
{
    BOOL bEqual = ( nTabCount == rCmp.nTabCount );
    if (bEqual)
        for (USHORT i=0; i<nTabCount; i++)
            if (!(pData[i]==rCmp.pData[i]))
            {
                bEqual = FALSE;
                break;
            }
    return bEqual;
}




