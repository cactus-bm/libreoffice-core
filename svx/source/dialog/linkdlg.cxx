/*************************************************************************
 *
 *  $RCSfile: linkdlg.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: pjunck $ $Date: 2004-10-27 15:33:27 $
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

#include <linkdlg.hxx>

#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif

#include <tools/urlobj.hxx>
#include <svtools/svmedit.hxx>
#include <svtools/filedlg.hxx>
#include <vcl/dialog.hxx>
#include <vcl/button.hxx>
#include <vcl/fixed.hxx>
#include <vcl/group.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/timer.hxx>
#include <svtools/svtabbx.hxx>

#include <svuidlg.hrc>
#include <sfx2/linkmgr.hxx>
#include <sfx2/linksrc.hxx>
#include <svtools/soerr.hxx>
#include <sfx2/lnkbase.hxx>
#include <sfx2/objsh.hxx>

#include "dialmgr.hxx"

#define _SVSTDARR_USHORTS
#include <svtools/svstdarr.hxx>

#define MAX_FILENAME    18
#define MAX_LINKNAME    18
#define MAX_TYPENAME    15
#define MAX_UPDATENAME  10

#define FILEOBJECT ( OBJECT_CLIENT_FILE & ~OBJECT_CLIENT_SO )

using namespace sfx2;

SV_DECL_IMPL_REF_LIST(SvBaseLink,SvBaseLink*)

// Achtung im Code wird dieses Array direkt (0, 1, ...) indiziert
static long nTabs[] =
    {   4, // Number of Tabs
        0, 77, 144, 209
    };


SvBaseLinksDlg::SvBaseLinksDlg( Window * pParent, SvLinkManager* pMgr, BOOL bHtml )
    : ModalDialog( pParent, ResId( MD_UPDATE_BASELINKS, DIALOG_MGR() ) )
    , pLinkMgr( NULL )
    , bHtmlMode(bHtml)
    , aTbLinks( this, ResId(TB_LINKS ) )
    , aFtFiles( this, ResId( FT_FILES ) ),
    aFtLinks( this, ResId( FT_LINKS ) ),
    aFtType( this, ResId( FT_TYPE ) ),
    aFtStatus( this, ResId( FT_STATUS ) ),
    aCancelButton1( this, ResId( 1 ) ),
    aHelpButton1( this, ResId( 1 ) ),
    aPbUpdateNow( this, ResId( PB_UPDATE_NOW ) ),
    aPbOpenSource( this, ResId( PB_OPEN_SOURCE ) ),
    aPbChangeSource( this, ResId( PB_CHANGE_SOURCE ) ),
    aPbBreakLink( this, ResId( PB_BREAK_LINK ) ),
    aFtFiles2( this, ResId( FT_FILES2 ) ),
    aFtSource2( this, ResId( FT_SOURCE2 ) ),
    aFtType2( this, ResId( FT_TYPE2 ) ),
    aFtUpdate( this, ResId( FT_UPDATE ) ),
    aRbAutomatic( this, ResId( RB_AUTOMATIC ) ),
    aRbManual( this, ResId( RB_MANUAL ) ),
    aFtFullFileName( this, ResId( FT_FULL_FILE_NAME ) ),
    aFtFullSourceName( this, ResId( FT_FULL_SOURCE_NAME ) ),
    aFtFullTypeName( this, ResId( FT_FULL_TYPE_NAME ) )
{
    aStrAutolink = String( ResId( STR_AUTOLINK ) );
    aStrManuallink = String( ResId( STR_MANUALLINK ) );
    aStrBrokenlink = String( ResId( STR_BROKENLINK ) );
    aStrGraphiclink = String( ResId( STR_GRAPHICLINK ) );
    aStrButtonclose = String( ResId( STR_BUTTONCLOSE ) );
    aStrCloselinkmsg = String( ResId( STR_CLOSELINKMSG ) );
    aStrCloselinkmsgMulti = String( ResId( STR_CLOSELINKMSG_MULTI ) );
    aStrWaitinglink = String( ResId( STR_WAITINGLINK ) );

    FreeResource();

    aTbLinks.SetHelpId(HID_LINKDLG_TABLB);
    aTbLinks.SetSelectionMode( MULTIPLE_SELECTION );
    aTbLinks.SetTabs( &nTabs[0], MAP_APPFONT );
    aTbLinks.Resize();  // OS: Hack fuer richtige Selektion

    //JP 24.02.99: UpdateTimer fuer DDE-/Grf-Links, auf die gewarted wird
    aUpdateTimer.SetTimeoutHdl( LINK( this, SvBaseLinksDlg, UpdateWaitingHdl ) );
    aUpdateTimer.SetTimeout( 1000 );

    OpenSource().Hide();

    Links().SetSelectHdl( LINK( this, SvBaseLinksDlg, LinksSelectHdl ) );
    Links().SetDoubleClickHdl( LINK( this, SvBaseLinksDlg, LinksDoubleClickHdl ) );
    Automatic().SetClickHdl( LINK( this, SvBaseLinksDlg, AutomaticClickHdl ) );
    Manual().SetClickHdl( LINK( this, SvBaseLinksDlg, ManualClickHdl ) );
    UpdateNow().SetClickHdl( LINK( this, SvBaseLinksDlg, UpdateNowClickHdl ) );
//  OpenSource().SetClickHdl( LINK( this, SvBaseLinksDlg, OpenSourceClickHdl ) );
    ChangeSource().SetClickHdl( LINK( this, SvBaseLinksDlg, ChangeSourceClickHdl ) );
    if(!bHtmlMode)
        BreakLink().SetClickHdl( LINK( this, SvBaseLinksDlg, BreakLinkClickHdl ) );
    else
        BreakLink().Hide();

    SetManager( pMgr );
}

SvBaseLinksDlg::~SvBaseLinksDlg()
{
}

/*************************************************************************
|*    SvBaseLinksDlg::Handler()
|*
|*    Beschreibung
|*    Ersterstellung    MM 14.06.94
|*    Letzte Aenderung  JP 30.05.95
*************************************************************************/
IMPL_LINK( SvBaseLinksDlg, LinksSelectHdl, SvTabListBox *, pSvTabListBox )
{
    USHORT nSelectionCount = pSvTabListBox ?
        (USHORT)pSvTabListBox->GetSelectionCount() : 0;
    if(nSelectionCount > 1)
    {
        //bei Mehrfachselektion ggf. alte Eintraege deselektieren
        SvLBoxEntry* pEntry = 0;
        SvBaseLink* pLink = 0;
        pEntry = pSvTabListBox->GetHdlEntry();
        pLink = (SvBaseLink*)pEntry->GetUserData();
        USHORT nObjectType = pLink->GetObjType();
        if((OBJECT_CLIENT_FILE & nObjectType) != OBJECT_CLIENT_FILE)
        {
            pSvTabListBox->SelectAll(FALSE);
            pSvTabListBox->Select(pEntry);
            nSelectionCount = 1;
        }
        else
        {
            for( USHORT i = 0; i < nSelectionCount; i++)
            {
                pEntry = i == 0 ? pSvTabListBox->FirstSelected() :
                                    pSvTabListBox->NextSelected(pEntry);
                DBG_ASSERT(pEntry, "Wo ist der Entry?")
                pLink = (SvBaseLink*)pEntry->GetUserData();
                DBG_ASSERT(pLink, "Wo ist der Link?")
                if( (OBJECT_CLIENT_FILE & pLink->GetObjType()) != OBJECT_CLIENT_FILE )
                    pSvTabListBox->Select( pEntry, FALSE );

            }
        }

        UpdateNow().Enable();

        Automatic().Disable();
        Manual().Check();
        Manual().Disable();
    }
    else
    {
        USHORT nPos;
        SvBaseLink* pLink = GetSelEntry( &nPos );
        if( !pLink )
            return 0;

        UpdateNow().Enable();

        String sType, sLink;
        String *pLinkNm = &sLink, *pFilter = 0;

        if( FILEOBJECT & pLink->GetObjType() )
        {
            Automatic().Disable();
            Manual().Check();
            Manual().Disable();
            if( OBJECT_CLIENT_GRF == pLink->GetObjType() )
                pLinkNm = 0, pFilter = &sLink;
        }
        else
        {
            Automatic().Enable();
            Manual().Enable();

            if( LINKUPDATE_ALWAYS == pLink->GetUpdateMode() )
                Automatic().Check();
            else
                Manual().Check();
        }

        String aFileName;
        pLinkMgr->GetDisplayNames( pLink, &sType, &aFileName, pLinkNm, pFilter );

        FileName().SetText( aFileName );
        SourceName().SetText( sLink );
        TypeName().SetText( sType );
    }
    return 0;
}

IMPL_LINK_INLINE_START( SvBaseLinksDlg, LinksDoubleClickHdl, SvTabListBox *, pSvTabListBox )
{
    ChangeSourceClickHdl( 0 );
    return 0;
}
IMPL_LINK_INLINE_END( SvBaseLinksDlg, LinksDoubleClickHdl, SvTabListBox *, pSvTabListBox )

IMPL_LINK_INLINE_START( SvBaseLinksDlg, AutomaticClickHdl, RadioButton *, pRadioButton )
{
    USHORT nPos;
    SvBaseLink* pLink = GetSelEntry( &nPos );
    if( pLink && !( FILEOBJECT & pLink->GetObjType() ) &&
        LINKUPDATE_ALWAYS != pLink->GetUpdateMode() )
        SetType( *pLink, nPos, LINKUPDATE_ALWAYS );
    return 0;
}
IMPL_LINK_INLINE_END( SvBaseLinksDlg, AutomaticClickHdl, RadioButton *, pRadioButton )

IMPL_LINK_INLINE_START( SvBaseLinksDlg, ManualClickHdl, RadioButton *, pRadioButton )
{
    USHORT nPos;
    SvBaseLink* pLink = GetSelEntry( &nPos );
    if( pLink && !( FILEOBJECT & pLink->GetObjType() ) &&
        LINKUPDATE_ONCALL != pLink->GetUpdateMode())
        SetType( *pLink, nPos, LINKUPDATE_ONCALL );
    return 0;
}
IMPL_LINK_INLINE_END( SvBaseLinksDlg, ManualClickHdl, RadioButton *, pRadioButton )

IMPL_LINK( SvBaseLinksDlg, UpdateNowClickHdl, PushButton *, pPushButton )
{
    SvTabListBox& rListBox = Links();
    USHORT nSelCnt = (USHORT)rListBox.GetSelectionCount();
    if( 255 < nSelCnt )
        nSelCnt = 255;
    SvPtrarr aLnkArr( (BYTE)nSelCnt );
    SvUShorts aPosArr( (BYTE)nSelCnt );

    USHORT nFirstPos = USHRT_MAX;

    SvLBoxEntry* pE = rListBox.FirstSelected();
    while( pE )
    {
        USHORT nFndPos = (USHORT)rListBox.GetModel()->GetAbsPos( pE );
        if( LISTBOX_ENTRY_NOTFOUND != nFndPos )
        {
            aLnkArr.Insert( pE->GetUserData(), aLnkArr.Count() );
            aPosArr.Insert( nFndPos, aPosArr.Count() );
        }
        pE = rListBox.NextSelected( pE );
    }

    if( aLnkArr.Count() )
    {
        for( USHORT n = 0; n < aLnkArr.Count(); ++n )
        {
            SvBaseLinkRef xLink = (SvBaseLink*)aLnkArr[ n ];

            // suche erstmal im Array nach dem Eintrag
            USHORT nFndPos = USHRT_MAX;
            for( USHORT i = 0; i < pLinkMgr->GetLinks().Count(); ++i )
                if( &xLink == *pLinkMgr->GetLinks()[ i ] )
                {
                    xLink->SetUseCache( FALSE );
                    SetType( *xLink, aPosArr[ n ], xLink->GetUpdateMode() );
                    xLink->SetUseCache( TRUE );
                    break;
                }
        }

        // falls jemand der Meinung ist, seine Links auszutauschen (SD)
        SvLinkManager* pNewMgr = pLinkMgr;
        pLinkMgr = 0;
        SetManager( pNewMgr );


        if( 0 == (pE = rListBox.GetEntry( aPosArr[ 0 ] )) ||
            pE->GetUserData() != aLnkArr[ 0 ] )
        {
            // suche mal den Link
            pE = rListBox.First();
            while( pE )
            {
                if( pE->GetUserData() == aLnkArr[ 0 ] )
                    break;
                pE = rListBox.Next( pE );
            }

            if( !pE )
                pE = rListBox.FirstSelected();
        }

        if( pE )
        {
            SvLBoxEntry* pSelEntry = rListBox.FirstSelected();
            if( pE != pSelEntry )
                rListBox.Select( pSelEntry, FALSE );
            rListBox.Select( pE );
            rListBox.MakeVisible( pE );
        }
    }
    return 0;
}

/*
IMPL_LINK_INLINE_START( SvBaseLinksDlg, OpenSourceClickHdl, PushButton *, pPushButton )
{
    DBG_ASSERT( !this, "Open noch nicht impl." )
    return 0;
}
IMPL_LINK_INLINE_END( SvBaseLinksDlg, OpenSourceClickHdl, PushButton *, pPushButton )
*/

IMPL_LINK( SvBaseLinksDlg, ChangeSourceClickHdl, PushButton *, pPushButton )
{
    USHORT nSelectionCount = (USHORT)Links().GetSelectionCount();
    if(nSelectionCount > 1)
    {
        PathDialog aPathDlg( this );
        String sType, sFile, sLinkName;
        String  sFilter;
        SvLBoxEntry* pEntry = Links().FirstSelected();
        SvBaseLink* pLink = (SvBaseLink*)pEntry->GetUserData();
        pLinkMgr->GetDisplayNames( pLink, &sType, &sFile, 0, 0 );
        INetURLObject aUrl(sFile);
        if(aUrl.GetProtocol() == INET_PROT_FILE)
        {
            String sOldPath(aUrl.PathToFileName());
            USHORT nLen = aUrl.GetName().Len();
            sOldPath.Erase(sOldPath.Len() - nLen, nLen);
            aPathDlg.SetPath(sOldPath);
        }
        if(aPathDlg.Execute() == RET_OK)
        {
            String aPath = aPathDlg.GetPath();

            for( USHORT i = 0; i < nSelectionCount; i++)
            {
                pEntry = i==0 ?
                        Links().FirstSelected() :
                            Links().NextSelected( pEntry );
                DBG_ASSERT(pEntry,"Wo ist der Entry")
                pLink = (SvBaseLink*)pEntry->GetUserData();
                DBG_ASSERT(pLink,"Wo ist der Link")
                pLinkMgr->GetDisplayNames( pLink, &sType, &sFile, &sLinkName, &sFilter );
                INetURLObject aUrl(sFile);
                INetURLObject aUrl2(aPath, INET_PROT_FILE);
                aUrl2.insertName( aUrl.getName() );
                String sNewLinkName;
                MakeLnkName( sNewLinkName, 0 ,
                        aUrl2.GetMainURL(INetURLObject::DECODE_TO_IURI), sLinkName, &sFilter);
                pLink->SetLinkSourceName( sNewLinkName );
                pLink->Update();
            }
            if( pLinkMgr->GetPersist() )
                pLinkMgr->GetPersist()->SetModified();
            SvLinkManager* pNewMgr = pLinkMgr;
            pLinkMgr = 0;
            SetManager( pNewMgr );
        }
    }
    else
    {
        USHORT nPos;
        SvBaseLink* pLink = GetSelEntry( &nPos );
        if( pLink && (pLink->GetLinkSourceName().Len() != 0) && pLink->Edit( this ) )
        {
            // JP 09.01.98:
            // StarImpress/Draw tauschen die LinkObjecte selbst aus!
            // also suche den Link im Manager, wenn der nicht mehr existiert,
            // dann setze fuelle die Liste komplett neu. Ansonsten braucht
            // nur der editierte Linkt aktualisiert werden.
            BOOL bLinkFnd = FALSE;
            for( USHORT n = pLinkMgr->GetLinks().Count(); n;  )
                if( pLink == &(*pLinkMgr->GetLinks()[ --n ]) )
                {
                    bLinkFnd = TRUE;
                    break;
                }

            if( bLinkFnd )
            {
                Links().SetUpdateMode(FALSE);
                Links().GetModel()->Remove( Links().GetEntry( nPos ) );
                SvLBoxEntry* pToUnselect = Links().FirstSelected();
                InsertEntry( *pLink, nPos, sal_True );
                if(pToUnselect)
                    Links().Select(pToUnselect, FALSE);
                Links().SetUpdateMode(TRUE);
            }
            else
            {
                SvLinkManager* pNewMgr = pLinkMgr;
                pLinkMgr = 0;
                SetManager( pNewMgr );
            }
            if( pLinkMgr->GetPersist() )
                pLinkMgr->GetPersist()->SetModified();
        }
    }
    return 0;
}

IMPL_LINK( SvBaseLinksDlg, BreakLinkClickHdl, PushButton *, pPushButton )
{
    BOOL bModified = FALSE;
    if(Links().GetSelectionCount() <= 1)
    {
        USHORT nPos;
        SvBaseLinkRef xLink = GetSelEntry( &nPos );
        if( !xLink.Is() )
            return 0;

        QueryBox aBox( this, WB_YES_NO | WB_DEF_YES, Closelinkmsg() );

        if( RET_YES == aBox.Execute() )
        {
            Links().GetModel()->Remove( Links().GetEntry( nPos ) );

            // falls Object noch vorhanden, dann das schliessen
            BOOL bNewLnkMgr = OBJECT_CLIENT_FILE == xLink->GetObjType();

            // dem Link sagen, das er aufgeloest wird!
            xLink->Closed();

            // falls einer vergessen hat sich auszutragen
            if( xLink.Is() )
                pLinkMgr->Remove( &xLink );

            if( bNewLnkMgr )
            {
                SvLinkManager* pNewMgr = pLinkMgr;
                pLinkMgr = 0;
                SetManager( pNewMgr );

                SvLBoxEntry* pEntry = Links().GetEntry( nPos ? --nPos : 0 );
                if( pEntry )
                    Links().SetCurEntry( pEntry );
            }
            bModified = TRUE;
        }
    }
    else
    {
        QueryBox aBox( this, WB_YES_NO | WB_DEF_YES, CloselinkmsgMulti() );

        if( RET_YES == aBox.Execute() )
        {

            SvBaseLinkMemberList aLinkList;
            SvLBoxEntry* pEntry = Links().FirstSelected();
            while ( pEntry )
            {
                void * pUD = pEntry->GetUserData();
                if( pUD )
                    aLinkList.Append( (SvBaseLink*)pUD );
                pEntry = Links().NextSelected(pEntry);
            }
            Links().RemoveSelection();
            for( ULONG i = 0; i < aLinkList.Count(); i++ )
            {
                SvBaseLinkRef xLink = aLinkList.GetObject( i );
                // dem Link sagen, das er aufgeloest wird!
                xLink->Closed();

                // falls einer vergessen hat sich auszutragen
                pLinkMgr->Remove( &xLink );
                bModified = TRUE;
            }
            //Danach alle selektierten Eintraege entfernen
        }
    }
    if(bModified)
    {
        if( !Links().GetEntryCount() )
        {
            // Der letzte macht das Licht aus
            Automatic().Disable();
            Manual().Disable();
            UpdateNow().Disable();
//            OpenSource().Disable();
            ChangeSource().Disable();
            BreakLink().Disable();

            String aEmpty;
            SourceName().SetText( aEmpty );
            TypeName().SetText( aEmpty );
        }
        if( pLinkMgr->GetPersist() )
            pLinkMgr->GetPersist()->SetModified();
    }
    return 0;
}

IMPL_LINK( SvBaseLinksDlg, UpdateWaitingHdl, Timer*, pTimer )
{
//    for( SvLBoxEntry* pBox = Links().First(); pBox;
//          pBox = Links().Next( pBox ))

    Links().SetUpdateMode(FALSE);
    for( ULONG nPos = Links().GetEntryCount(); nPos; )
    {
        SvLBoxEntry* pBox = Links().GetEntry( --nPos );
        SvBaseLinkRef xLink( (SvBaseLink*)pBox->GetUserData() );
        if( xLink.Is() )
        {
            String sCur( ImplGetStateStr( *xLink ) ),
                    sOld( Links().GetEntryText( pBox, 3 ) );
            if( sCur != sOld )
                Links().SetEntryText( sCur, pBox, 3 );
        }
    }
    Links().SetUpdateMode(TRUE);
    return 0;
}

String SvBaseLinksDlg::ImplGetStateStr( const SvBaseLink& rLnk )
{
    String sRet;
    if( !rLnk.GetObj() )
        sRet = Brokenlink();
    else if( rLnk.GetObj()->IsPending() )
    {
        sRet = Waitinglink();
        StartUpdateTimer();
    }
    else if( LINKUPDATE_ALWAYS == rLnk.GetUpdateMode() )
        sRet = Autolink();
    else
        sRet = Manuallink();

    return sRet;
}

void SvBaseLinksDlg::SetManager( SvLinkManager* pNewMgr )
{
    if( pLinkMgr == pNewMgr )
        return;

    if( pNewMgr )
        // Update muss vor Clear gestoppt werden
        Links().SetUpdateMode( FALSE );

    Links().Clear();
    pLinkMgr = pNewMgr;

    if( pLinkMgr )
    {
        SvBaseLinks& rLnks = (SvBaseLinks&)pLinkMgr->GetLinks();
        for( USHORT n = 0; n < rLnks.Count(); ++n )
        {
            SvBaseLinkRef* pLinkRef = rLnks[ n ];
            if( !pLinkRef->Is() )
            {
                rLnks.Remove( n, 1 );
                --n;
                continue;
            }
            if( (*pLinkRef)->IsVisible() )
                InsertEntry( **pLinkRef );
        }

        if( rLnks.Count() )
        {
            SvLBoxEntry* pEntry = Links().GetEntry( 0 );
            Links().SetCurEntry( pEntry );
            Links().Select( pEntry );
            LinksSelectHdl( 0 );
        }
        Links().SetUpdateMode( TRUE );
        Links().Invalidate();
    }
}


void SvBaseLinksDlg::InsertEntry( const SvBaseLink& rLink, USHORT nPos, sal_Bool bSelect )
{
    String aEntry, sFileNm, sLinkNm, sTypeNm, sFilter;
    const SvLinkSource* pObj = rLink.GetObj();

    pLinkMgr->GetDisplayNames( (SvBaseLink*)&rLink, &sTypeNm, &sFileNm, &sLinkNm, &sFilter );

    // GetTab(0) gibt die Position der von der TabListBox automatisch eingefuegten
    // Bitmap. Die Breite der ersten Textspalte ergibt sich deshalb aus Tab(2)-Tab(1)
    long nWidthPixel = Links().GetLogicTab( 2 ) - Links().GetLogicTab( 1 );
    nWidthPixel -= SV_TAB_BORDER;
    XubString aTxt = Links().GetEllipsisString( sFileNm, nWidthPixel, TEXT_DRAW_PATHELLIPSIS );
    INetURLObject aPath( sFileNm, INET_PROT_FILE );
    String aFileName = aPath.getName();
    if( aFileName.Len() > aTxt.Len() )
        aTxt = aFileName;
    else if( aTxt.Search( aFileName, aTxt.Len() - aFileName.Len() ) == STRING_NOTFOUND )
        // filename not in string
        aTxt = aFileName;

    aEntry = aTxt;
    aEntry += '\t';
    if( OBJECT_CLIENT_GRF == rLink.GetObjType() )
        aEntry += sFilter;
    else
        aEntry += sLinkNm;
    aEntry += '\t';
    aEntry += sTypeNm;
    aEntry += '\t';
    aEntry += ImplGetStateStr( rLink );

    SvLBoxEntry * pE = Links().InsertEntry( aEntry, nPos );
    pE->SetUserData( (void*)&rLink );
    if(bSelect)
        Links().Select(pE);
}

SvBaseLink* SvBaseLinksDlg::GetSelEntry( USHORT* pPos )
{
    SvLBoxEntry* pE = Links().FirstSelected();
    USHORT nPos;
    if( pE && LISTBOX_ENTRY_NOTFOUND !=
        ( nPos = (USHORT)Links().GetModel()->GetAbsPos( pE ) ) )
    {
        DBG_ASSERT( pE, "wo kommt der leere Eintrag her?" );

        if( pPos )
            *pPos = nPos;
        return (SvBaseLink*)pE->GetUserData();
    }
    return 0;
}

void SvBaseLinksDlg::SetType( SvBaseLink& rLink,
                                    USHORT nSelPos,
                                    USHORT nType )
{
    rLink.SetUpdateMode( nType );
    rLink.Update();
    SvLBoxEntry* pBox = Links().GetEntry( nSelPos );
    Links().SetEntryText( ImplGetStateStr( rLink ), pBox, 3 );
    if( pLinkMgr->GetPersist() )
        pLinkMgr->GetPersist()->SetModified();
}

void SvBaseLinksDlg::SetActLink( SvBaseLink * pLink )
{
    if( pLinkMgr )
    {
        const SvBaseLinks& rLnks = pLinkMgr->GetLinks();
        USHORT nSelect = 0;
        for( USHORT n = 0; n < rLnks.Count(); ++n )
        {
            SvBaseLinkRef* pLinkRef = rLnks[ n ];
            // #109573# only visible links have been inserted into the TreeListBox,
            // invisible ones have to be skipped here
            if( (*pLinkRef)->IsVisible() )
            {
                if( pLink == *pLinkRef )
                {
                    Links().Select( Links().GetEntry( nSelect ) );
                    LinksSelectHdl( 0 );
                    return ;
                }
                nSelect++;
            }
        }
    }
}

