/*************************************************************************
 *
 *  $RCSfile: arealink.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: nn $ $Date: 2002-10-25 16:42:50 $
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
#include "ui_pch.hxx"
#endif

#pragma hdrstop

// INCLUDE ---------------------------------------------------------

#include <sfx2/app.hxx>
#include <sfx2/docfile.hxx>
#include <svx/linkmgr.hxx>
#include <svtools/stritem.hxx>
#include <vcl/msgbox.hxx>

#include "arealink.hxx"

#include "tablink.hxx"
#include "document.hxx"
#include "docsh.hxx"
#include "rangenam.hxx"
#include "dbcolect.hxx"
#include "undoblk.hxx"
#include "globstr.hrc"
#include "markdata.hxx"
#include "hints.hxx"
#include "htmlimp.hxx"
#include "linkarea.hxx"         // dialog

#include "attrib.hxx"           // raus, wenn ResetAttrib am Dokument
#include "patattr.hxx"          // raus, wenn ResetAttrib am Dokument
#include "docpool.hxx"          // raus, wenn ResetAttrib am Dokument

TYPEINIT1(ScAreaLink,::so3::SvBaseLink);

//------------------------------------------------------------------------

ScAreaLink::ScAreaLink( SfxObjectShell* pShell, const String& rFile,
                        const String& rFilter, const String& rOpt,
                        const String& rArea, const ScRange& rDest,
                        ULONG nRefresh ) :
    ::so3::SvBaseLink(so3::LINKUPDATE_ONCALL,FORMAT_FILE),
    ScRefreshTimer  ( nRefresh ),
    pDocShell       ((ScDocShell*)pShell),
    aFileName       (rFile),
    aFilterName     (rFilter),
    aOptions        (rOpt),
    aSourceArea     (rArea),
    aDestArea       (rDest),
    bAddUndo        (TRUE),
    bInCreate       (FALSE),
    bDoInsert       (TRUE)
{
    DBG_ASSERT(pShell->ISA(ScDocShell), "ScAreaLink mit falscher ObjectShell");
    SetRefreshHandler( LINK( this, ScAreaLink, RefreshHdl ) );
    SetRefreshControl( pDocShell->GetDocument()->GetRefreshTimerControlAddress() );
}

__EXPORT ScAreaLink::~ScAreaLink()
{
    StopRefreshTimer();
}

BOOL __EXPORT ScAreaLink::Edit(Window* pParent)
{
    //  use own dialog instead of SvBaseLink::Edit...
    //  DefModalDialogParent setzen, weil evtl. aus der DocShell beim ConvertFrom
    //  ein Optionen-Dialog kommt...

    BOOL bRet = FALSE;
    ScLinkedAreaDlg* pDlg = new ScLinkedAreaDlg( pParent );
    pDlg->InitFromOldLink( aFileName, aFilterName, aOptions, aSourceArea, GetRefreshDelay() );
    if (pDlg->Execute() == RET_OK)
    {
        aOptions = pDlg->GetOptions();
        bRet = Refresh( pDlg->GetURL(), pDlg->GetFilter(), pDlg->GetSource(), pDlg->GetRefresh() );

        //  copy source data from members (set in Refresh) into link name for dialog
        String aLinkName;
        so3::MakeLnkName( aLinkName, NULL, aFileName, aSourceArea, &aFilterName );
        SetName( aLinkName );
    }
    delete pDlg;

    return bRet;
}

void __EXPORT ScAreaLink::DataChanged( const String&,
                                       const ::com::sun::star::uno::Any& )
{
    //  bei bInCreate nichts tun, damit Update gerufen werden kann, um den Status im
    //  LinkManager zu setzen, ohne die Daten im Dokument zu aendern

    if (bInCreate)
        return;

    SvxLinkManager* pLinkManager=pDocShell->GetDocument()->GetLinkManager();
    if (pLinkManager!=NULL)
    {
        String aFile;
        String aFilter;
        String aArea;
        pLinkManager->GetDisplayNames( this,0,&aFile,&aArea,&aFilter);

        //  the file dialog returns the filter name with the application prefix
        //  -> remove prefix
        ScDocumentLoader::RemoveAppPrefix( aFilter );

        // #81155# dialog doesn't set area, so keep old one
        if ( !aArea.Len() )
        {
            aArea = aSourceArea;

            // adjust in dialog:
            String aLinkName;
            so3::MakeLnkName( aLinkName, NULL, aFile, aArea, &aFilter );
            SetName( aLinkName );
        }

        Refresh( aFile, aFilter, aArea, GetRefreshDelay() );
    }
}

void __EXPORT ScAreaLink::Closed()
{
    // Verknuepfung loeschen: Undo

    ScDocument* pDoc = pDocShell->GetDocument();
    BOOL bUndo (pDoc->IsUndoEnabled());
    if (bAddUndo && bUndo)
    {
        pDocShell->GetUndoManager()->AddUndoAction( new ScUndoRemoveAreaLink( pDocShell,
                                                        aFileName, aFilterName, aOptions,
                                                        aSourceArea, aDestArea, GetRefreshDelay() ) );

        bAddUndo = FALSE;   // nur einmal
    }

    SvBaseLink::Closed();
}

void ScAreaLink::SetDestArea(const ScRange& rNew)
{
    aDestArea = rNew;           // fuer Undo
}

void ScAreaLink::SetSource(const String& rDoc, const String& rFlt, const String& rOpt,
                                const String& rArea)
{
    aFileName   = rDoc;
    aFilterName = rFlt;
    aOptions    = rOpt;
    aSourceArea = rArea;

    //  also update link name for dialog
    String aLinkName;
    so3::MakeLnkName( aLinkName, NULL, aFileName, aSourceArea, &aFilterName );
    SetName( aLinkName );
}

BOOL ScAreaLink::IsEqual( const String& rFile, const String& rFilter, const String& rOpt,
                            const String& rSource, const ScRange& rDest ) const
{
    return aFileName == rFile && aFilterName == rFilter && aOptions == rOpt &&
            aSourceArea == rSource && aDestArea.aStart == rDest.aStart;
}

// find a range with name >rAreaName< in >pSrcDoc<, return it in >rRange<
BOOL ScAreaLink::FindExtRange( ScRange& rRange, ScDocument* pSrcDoc, const String& rAreaName )
{
    BOOL bFound = FALSE;
    ScRangeName* pNames = pSrcDoc->GetRangeName();
    USHORT nPos;
    if (pNames)         // benannte Bereiche
    {
        if (pNames->SearchName( rAreaName, nPos ))
            if ( (*pNames)[nPos]->IsReference( rRange ) )
                bFound = TRUE;
    }
    if (!bFound)        // Datenbankbereiche
    {
        ScDBCollection* pDBColl = pSrcDoc->GetDBCollection();
        if (pDBColl)
            if (pDBColl->SearchName( rAreaName, nPos ))
            {
                USHORT nTab,nCol1,nRow1,nCol2,nRow2;
                (*pDBColl)[nPos]->GetArea(nTab,nCol1,nRow1,nCol2,nRow2);
                rRange = ScRange( nCol1,nRow1,nTab, nCol2,nRow2,nTab );
                bFound = TRUE;
            }
    }
    if (!bFound)        // direct reference (range or cell)
    {
        if ( rRange.ParseAny( rAreaName, pSrcDoc ) & SCA_VALID )
            bFound = TRUE;
    }
    return bFound;
}

//  ausfuehren:

BOOL ScAreaLink::Refresh( const String& rNewFile, const String& rNewFilter,
                            const String& rNewArea, ULONG nNewRefresh )
{
    //  Dokument laden - wie TabLink

    if (!rNewFile.Len() || !rNewFilter.Len())
        return FALSE;

    String aNewUrl( ScGlobal::GetAbsDocName( rNewFile, pDocShell ) );
    BOOL bNewUrlName = (aNewUrl != aFileName);

    const SfxFilter* pFilter = SFX_APP()->GetFilter( pDocShell->GetFactory(), rNewFilter );
    if (!pFilter)
        return FALSE;

    ScDocument* pDoc = pDocShell->GetDocument();

    BOOL bUndo (pDoc->IsUndoEnabled());
    pDoc->SetInLinkUpdate( TRUE );

    //  wenn neuer Filter ausgewaehlt wurde, Optionen vergessen
    if ( rNewFilter != aFilterName )
        aOptions.Erase();

    //  ItemSet immer anlegen, damit die DocShell die Optionen setzen kann
    SfxItemSet* pSet = new SfxAllItemSet( SFX_APP()->GetPool() );
    if ( aOptions.Len() )
        pSet->Put( SfxStringItem( SID_FILE_FILTEROPTIONS, aOptions ) );

    SfxMedium* pMed = new SfxMedium(aNewUrl, STREAM_STD_READ, FALSE, pFilter);

    ScDocShell* pSrcShell = new ScDocShell(SFX_CREATE_MODE_INTERNAL);
    SvEmbeddedObjectRef aRef = pSrcShell;
    pSrcShell->DoLoad(pMed);

    ScDocument* pSrcDoc = pSrcShell->GetDocument();

    // Optionen koennten gesetzt worden sein
    String aNewOpt = ScDocumentLoader::GetOptions(*pMed);
    if (!aNewOpt.Len())
        aNewOpt = aOptions;

    // correct source range name list for web query import
    String aTempArea;

    if( rNewFilter == ScDocShell::GetWebQueryFilterName() )
        aTempArea = ScHTMLImport::GetHTMLRangeNameList( pSrcDoc, rNewArea );
    else
        aTempArea = rNewArea;

    // find total size of source area
    USHORT nWidth = 0;
    USHORT nHeight = 0;
    xub_StrLen nTokenCnt = aTempArea.GetTokenCount( ';' );
    xub_StrLen nStringIx = 0;
    xub_StrLen nToken;

    for( nToken = 0; nToken < nTokenCnt; nToken++ )
    {
        String aToken( aTempArea.GetToken( 0, ';', nStringIx ) );
        ScRange aTokenRange;
        if( FindExtRange( aTokenRange, pSrcDoc, aToken ) )
        {
            // columns: find maximum
            nWidth = Max( nWidth, (USHORT)(aTokenRange.aEnd.Col() - aTokenRange.aStart.Col() + 1) );
            // rows: add row range + 1 empty row
            nHeight += aTokenRange.aEnd.Row() - aTokenRange.aStart.Row() + 2;
        }
    }
    // remove the last empty row
    if( nHeight )
        nHeight--;

    //  alte Daten loeschen / neue kopieren

    ScAddress aDestPos = aDestArea.aStart;
    USHORT nDestTab = aDestPos.Tab();
    ScRange aOldRange = aDestArea;
    ScRange aNewRange = aDestArea;          // alter Bereich, wenn Datei nicht gefunden o.ae.
    if (nWidth && nHeight)
    {
        aNewRange.aEnd.SetCol( aNewRange.aStart.Col() + nWidth - 1 );
        aNewRange.aEnd.SetRow( aNewRange.aStart.Row() + nHeight - 1 );
    }

    BOOL bCanDo = pDoc->CanFitBlock( aOldRange, aNewRange );    //! nach bDoInsert unterscheiden
    if (bCanDo)
    {
        ScDocShellModificator aModificator( *pDocShell );

        USHORT nStartX = aDestPos.Col();
        USHORT nStartY = aDestPos.Row();
        USHORT nOldEndX = aOldRange.aEnd.Col();
        USHORT nOldEndY = aOldRange.aEnd.Row();
        USHORT nNewEndX = aNewRange.aEnd.Col();
        USHORT nNewEndY = aNewRange.aEnd.Row();
        ScRange aMaxRange( aDestPos,
                    ScAddress(Max(nOldEndX,nNewEndX), Max(nOldEndY,nNewEndY), nDestTab) );

        //  Undo initialisieren

        ScDocument* pUndoDoc = NULL;
        ScDocument* pRedoDoc = NULL;
        if ( bAddUndo && bUndo )
        {
            pUndoDoc = new ScDocument( SCDOCMODE_UNDO );
            if ( bDoInsert )
            {
                if ( nNewEndX != nOldEndX || nNewEndY != nOldEndY )             // Bereich veraendert?
                {
                    pUndoDoc->InitUndo( pDoc, 0, pDoc->GetTableCount()-1 );
                    pDoc->CopyToDocument( 0,0,0,MAXCOL,MAXROW,MAXTAB,
                                            IDF_FORMULA, FALSE, pUndoDoc );     // alle Formeln
                }
                else
                    pUndoDoc->InitUndo( pDoc, nDestTab, nDestTab );             // nur Zieltabelle
                pDoc->CopyToDocument( aOldRange, IDF_ALL, FALSE, pUndoDoc );
            }
            else        // ohne Einfuegen
            {
                pUndoDoc->InitUndo( pDoc, nDestTab, nDestTab );             // nur Zieltabelle
                pDoc->CopyToDocument( aMaxRange, IDF_ALL, FALSE, pUndoDoc );
            }
        }

        //  Zellen einfuegen / loeschen
        //  DeleteAreaTab loescht auch MERGE_FLAG Attribute

        if (bDoInsert)
            pDoc->FitBlock( aOldRange, aNewRange );         // incl. loeschen
        else
            pDoc->DeleteAreaTab( aMaxRange, IDF_ALL );

        //  Daten kopieren

        if (nWidth && nHeight)
        {
            ScDocument aClipDoc( SCDOCMODE_CLIP );
            ScRange aNewTokenRange( aNewRange.aStart );
            nStringIx = 0;
            for( nToken = 0; nToken < nTokenCnt; nToken++ )
            {
                String aToken( aTempArea.GetToken( 0, ';', nStringIx ) );
                ScRange aTokenRange;
                if( FindExtRange( aTokenRange, pSrcDoc, aToken ) )
                {
                    USHORT nSrcTab = aTokenRange.aStart.Tab();
                    ScMarkData aSourceMark;
                    aSourceMark.SelectOneTable( nSrcTab );      // selektieren fuer CopyToClip
                    aSourceMark.SetMarkArea( aTokenRange );

                    pSrcDoc->CopyToClip( aTokenRange.aStart.Col(), aTokenRange.aStart.Row(),
                                         aTokenRange.aEnd.Col(), aTokenRange.aEnd.Row(),
                                         FALSE, &aClipDoc, FALSE, &aSourceMark );

                    if ( aClipDoc.HasAttrib( 0,0,nSrcTab, MAXCOL,MAXROW,nSrcTab,
                                            HASATTR_MERGED | HASATTR_OVERLAPPED ) )
                    {
                        //! ResetAttrib am Dokument !!!

                        ScPatternAttr aPattern( pSrcDoc->GetPool() );
                        aPattern.GetItemSet().Put( ScMergeAttr() );             // Defaults
                        aPattern.GetItemSet().Put( ScMergeFlagAttr() );
                        aClipDoc.ApplyPatternAreaTab( 0,0, MAXCOL,MAXROW, nSrcTab, aPattern );
                    }

                    aNewTokenRange.aEnd.SetCol( aNewTokenRange.aStart.Col() + (aTokenRange.aEnd.Col() - aTokenRange.aStart.Col()) );
                    aNewTokenRange.aEnd.SetRow( aNewTokenRange.aStart.Row() + (aTokenRange.aEnd.Row() - aTokenRange.aStart.Row()) );
                    ScMarkData aDestMark;
                    aDestMark.SelectOneTable( nDestTab );
                    aDestMark.SetMarkArea( aNewTokenRange );
                    pDoc->CopyFromClip( aNewTokenRange, aDestMark, IDF_ALL, NULL, &aClipDoc, FALSE );
                    aNewTokenRange.aStart.SetRow( aNewTokenRange.aEnd.Row() + 2 );
                }
            }
        }
        else
        {
            String aErr = ScGlobal::GetRscString(STR_LINKERROR);
            pDoc->SetString( aDestPos.Col(), aDestPos.Row(), aDestPos.Tab(), aErr );
        }

        //  Undo eintragen

        if ( bAddUndo && bUndo)
        {
            pRedoDoc = new ScDocument( SCDOCMODE_UNDO );
            pRedoDoc->InitUndo( pDoc, nDestTab, nDestTab );
            pDoc->CopyToDocument( aNewRange, IDF_ALL, FALSE, pRedoDoc );

            pDocShell->GetUndoManager()->AddUndoAction(
                new ScUndoUpdateAreaLink( pDocShell,
                                            aFileName, aFilterName, aOptions,
                                            aSourceArea, aOldRange, GetRefreshDelay(),
                                            aNewUrl, rNewFilter, aNewOpt,
                                            rNewArea, aNewRange, nNewRefresh,
                                            pUndoDoc, pRedoDoc, bDoInsert ) );
        }

        //  neue Einstellungen merken

        if ( bNewUrlName )
            aFileName = aNewUrl;
        if ( rNewFilter != aFilterName )
            aFilterName = rNewFilter;
        if ( rNewArea != aSourceArea )
            aSourceArea = rNewArea;
        if ( aNewOpt != aOptions )
            aOptions = aNewOpt;

        if ( aNewRange != aDestArea )
            aDestArea = aNewRange;

        if ( nNewRefresh != GetRefreshDelay() )
            SetRefreshDelay( nNewRefresh );

        USHORT nPaintEndX = Max( aOldRange.aEnd.Col(), aNewRange.aEnd.Col() );
        USHORT nPaintEndY = Max( aOldRange.aEnd.Row(), aNewRange.aEnd.Row() );

        if ( aOldRange.aEnd.Col() != aNewRange.aEnd.Col() )
            nPaintEndX = MAXCOL;
        if ( aOldRange.aEnd.Row() != aNewRange.aEnd.Row() )
            nPaintEndY = MAXROW;

        if ( !pDocShell->AdjustRowHeight( aDestPos.Row(), nPaintEndY, nDestTab ) )
            pDocShell->PostPaint( aDestPos.Col(),aDestPos.Row(),nDestTab,
                                    nPaintEndX,nPaintEndY,nDestTab, PAINT_GRID );
        aModificator.SetDocumentModified();
    }
    else
    {
        //  CanFitBlock FALSE -> Probleme mit zusammengefassten Zellen
        //                       oder Tabellengrenze erreicht!
        //! Zellschutz ???

        //! Link-Dialog muss Default-Parent setzen
        //  "kann keine Zeilen einfuegen"
        InfoBox aBox( Application::GetDefDialogParent(),
                        ScGlobal::GetRscString( STR_MSSG_DOSUBTOTALS_2 ) );
        aBox.Execute();
    }

    //  aufraeumen

    aRef->DoClose();

    pDoc->SetInLinkUpdate( FALSE );

    if (bCanDo)
    {
        //  notify Uno objects (for XRefreshListener)
        //! also notify Uno objects if file name was changed!
        ScLinkRefreshedHint aHint;
        aHint.SetAreaLink( aDestPos );
        pDoc->BroadcastUno( aHint );
    }

    return bCanDo;
}


IMPL_LINK( ScAreaLink, RefreshHdl, ScAreaLink*, pCaller )
{
    long nRes = Refresh( aFileName, aFilterName, aSourceArea,
        GetRefreshDelay() ) != 0;
    return nRes;
}

