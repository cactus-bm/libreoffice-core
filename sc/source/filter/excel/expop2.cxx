/*************************************************************************
 *
 *  $RCSfile: expop2.cxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: hr $ $Date: 2003-03-26 18:04:33 $
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
#include "filt_pch.hxx"
#endif

#pragma hdrstop

//------------------------------------------------------------------------

#include <offmgr/fltrcfg.hxx>
#include <offmgr/app.hxx>

#include <sfx2/objsh.hxx>
#include <sfx2/docinf.hxx>
#include <svx/svxmsbas.hxx>

#include "scerrors.hxx"
#include "scextopt.hxx"

#include "root.hxx"
#include "exp_op.hxx"
#include "excdoc.hxx"

#include "xcl97esc.hxx"

#include "document.hxx"
#include "rangenam.hxx"
#include "filtopt.hxx"

#ifndef SC_XLTOOLS_HXX
#include "xltools.hxx"
#endif
#ifndef SC_XELINK_HXX
#include "xelink.hxx"
#endif

extern const sal_Char*  pVBAStorageName;
extern const sal_Char*  pVBASubStorageName;




ExportBiff5::ExportBiff5( SvStorage& rRootStorage, SvStream& aStream, ScDocument* pDoc, const String& rBasePath, CharSet eDest, bool bRelUrl ):
    XclExpRootData( xlBiff5, *pDoc, rBasePath, eDest, bRelUrl ),
    ExportTyp( aStream, pDoc, eDest ),
    XclExpRoot( static_cast< XclExpRootData& >( *this ) )
    // Excel immer Windoofs, Quelle (SC) immer System
{
    DBG_ASSERT( pDoc, "-ExportBiff5::ExportBiff5(): No Null-Document!" );

    // nur Teil der Root-Daten gebraucht
    pExcRoot = mpRD;
    pExcRoot->pDoc = pDoc;
    pExcRoot->pER = this;   // ExcRoot -> XclExpRoot
    pExcRoot->pScNameList = new ScRangeName;
    pExcRoot->bCellCut = FALSE;
    pExcRoot->eHauptDateiTyp = Biff5;
    pExcRoot->eDateiTyp = Biff5;
    pExcRoot->nColMax = XCL_COLMAX;
    pExcRoot->nRowMax = EXC5_ANZROW;
    pExcRoot->pRootStorage = &rRootStorage;

    pExcRoot->pCharset = &eZielChar;

    pExcRoot->pExtDocOpt = new ScExtDocOptions;
    if( pDoc->GetExtDocOptions() )
        *pExcRoot->pExtDocOpt = *pDoc->GetExtDocOptions();

    // options from configuration

    ScFilterOptions aFilterOpt;
    pExcRoot->bBreakSharedFormula = aFilterOpt.GetBreakShared();

    pExcRoot->fRowScale = aFilterOpt.GetExcelRowScale();
    if( pExcRoot->fRowScale <= 0.0 )
        pExcRoot->fRowScale = 1.0;

    pExcDoc = new ExcDocument( *this );
}


ExportBiff5::~ExportBiff5()
{
    delete pExcDoc;
}


FltError ExportBiff5::Write()
{
    FltError                eRet = eERR_OK;
    OfaFilterOptions*       pFiltOpt = NULL;
    SvStorage*              pRootStorage = pExcRoot->pRootStorage;

    if( pExcRoot->eHauptDateiTyp >= Biff8 )
    {
        pFiltOpt = OFF_APP()->GetFilterOptions();

        pExcRoot->bWriteVBAStorage = pFiltOpt && pFiltOpt->IsLoadExcelBasicStorage();
    }

    SfxObjectShell&         rDocShell = *pExcRoot->pDoc->GetDocumentShell();

    if( pExcRoot->bWriteVBAStorage )
    {
        SfxObjectShell&     rDocShell = *pExcRoot->pDoc->GetDocumentShell();

        DBG_ASSERT( pExcRoot->pRootStorage, "-ImportExcel8::ImportExcel8(): no storage, no cookies!" );

        SvxImportMSVBasic   aBasicImport(   rDocShell,
                                            *pRootStorage,
                                            pFiltOpt->IsLoadExcelBasicCode(),
                                            pFiltOpt->IsLoadExcelBasicStorage() );

        ULONG       nErr = aBasicImport.SaveOrDelMSVBAStorage( TRUE, _STRING( pVBAStorageName ) );

        if( nErr != ERRCODE_NONE )
            rDocShell.SetError( nErr );
    }

    // VBA-storage written?
    pExcRoot->bWriteVBAStorage = pRootStorage->IsContained( _STRING( pVBAStorageName ) );

    pExcDoc->ReadDoc();         // ScDoc -> ExcDoc
    pExcDoc->Write( aOut );     // wechstreamen

    SfxDocumentInfo&    rInfo = rDocShell.GetDocInfo();
    rInfo.SavePropertySet( pExcRoot->pRootStorage );

    if( pExcRoot->bCellCut || IsTruncated() )
        return SCWARN_EXPORT_MAXROW;
    else
        return eERR_OK;
}



ExportBiff8::ExportBiff8( SvStorage& rRootStorage, SvStream& aStream, ScDocument* pDoc, const String& rBasePath, CharSet eZ, bool bRelUrl ) :
    ExportBiff5( rRootStorage, aStream, pDoc, rBasePath, eZ, bRelUrl )
{
    SetBiff( xlBiff8 );
    pExcRoot->eHauptDateiTyp = Biff8;
    pExcRoot->eDateiTyp = Biff8;
    pExcRoot->nRowMax = XCL8_ROWMAX;
    pExcRoot->pEscher = new XclEscher( pDoc->GetTableCount(), *pExcRoot );
}


ExportBiff8::~ExportBiff8()
{
    delete pExcRoot->pEscher;
    pExcRoot->pEscher = NULL;
}


