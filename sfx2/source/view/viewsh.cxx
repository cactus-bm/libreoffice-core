/*************************************************************************
 *
 *  $RCSfile: viewsh.cxx,v $
 *
 *  $Revision: 1.43 $
 *
 *  last change: $Author: kz $ $Date: 2004-08-31 12:38:10 $
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

#ifndef _PLUGIN_HXX //autogen
#include <so3/plugin.hxx>
#endif
#ifndef _APPLET_HXX //autogen
#include <so3/applet.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFX_WHITER_HXX //autogen
#include <svtools/whiter.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SOERR_HXX //autogen
#include <so3/soerr.hxx>
#endif
#ifndef _SFXINTITEM_HXX //autogen
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXECODE_HXX
#include <svtools/sfxecode.hxx>
#endif
#ifndef _EHDL_HXX
#include <svtools/ehdl.hxx>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_FRAME_XLAYOUTMANAGER_HPP_
#include <drafts/com/sun/star/frame/XLayoutManager.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#include <tools/urlobj.hxx>
#include <svtools/pathoptions.hxx>
#include <svtools/miscopt.hxx>
#ifndef INCLUDED_SVTOOLS_INTERNALOPTIONS_HXX
#include <svtools/internaloptions.hxx>
#endif

#include <svtools/javaoptions.hxx>
#include <basic/basmgr.hxx>
#include <basic/sbuno.hxx>
#include <framework/actiontriggerhelper.hxx>

#ifndef GCC
#pragma hdrstop
#endif

#include "viewsh.hxx"
#include "viewimp.hxx"

#include "sfxresid.hxx"
#include "request.hxx"
#include "templdlg.hxx"
#include "interno.hxx"
#include "printer.hxx"
#include "docfile.hxx"
#include "dispatch.hxx"
#include "arrdecl.hxx"
#include "ipenv.hxx"
#include "accmgr.hxx"
#include "intfrm.hxx"
#include "docfac.hxx"
#include "view.hrc"
#include "mnumgr.hxx"
#include "virtmenu.hxx"
#include "objuno.hxx"
#include "sfxlocal.hrc"
#include "frameobj.hxx"
#include "sfxbasecontroller.hxx"
#include "topfrm.hxx"
#include "mailmodel.hxx"
#include "event.hxx"
#include "appdata.hxx"
#include "fcontnr.hxx"

// #110897#
#ifndef _UNOTOOLS_PROCESSFACTORY_HXX
#include <comphelper/processfactory.hxx>
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::beans;

//=========================================================================
DBG_NAME(SfxViewShell);

#define SfxViewShell
#include "sfxslots.hxx"

//=========================================================================

SFX_IMPL_INTERFACE(SfxViewShell,SfxShell,SfxResId(0))
{
    SFX_CHILDWINDOW_REGISTRATION( SID_MAIL_CHILDWIN );
}

TYPEINIT2(SfxViewShell,SfxShell,SfxListener);

//--------------------------------------------------------------------

ErrCode SfxViewShell::DirectLoad( SfxFrame* pFrame, SfxMedium& rMedium )
{
    return ERRCODE_SFX_FORCEDOCLOAD;
}

//--------------------------------------------------------------------

void SfxViewShell::ExecMisc_Impl( SfxRequest &rReq )
{
    const USHORT nId = rReq.GetSlot();
    switch( nId )
    {
        case SID_STYLE_FAMILY :
        {
            SFX_REQUEST_ARG(rReq, pItem, SfxUInt16Item, nId, FALSE);
            if (pItem)
                pImp->nFamily = pItem->GetValue();
            break;
        }

        case SID_STYLE_CATALOG:
        {
            SfxTemplateCatalog aCatalog(
                SFX_APP()->GetTopWindow(), &GetViewFrame()->GetBindings());
            aCatalog.Execute();
            rReq.Ignore();
            break;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        case SID_MAIL_SENDDOCASPDF:
        case SID_MAIL_SENDDOC:
        {
            SfxObjectShell* pDoc = GetObjectShell();
            if ( pDoc && pDoc->QueryHiddenInformation(
                            WhenSaving, &GetViewFrame()->GetWindow() ) != RET_YES )
                break;

            if ( SvtInternalOptions().MailUIEnabled() )
            {
                GetViewFrame()->SetChildWindow( SID_MAIL_CHILDWIN, TRUE );
            }
            else
            {
                SfxMailModel_Impl   aModel( &GetViewFrame()->GetBindings() );

                SFX_REQUEST_ARG(rReq, pMailSubject, SfxStringItem, SID_MAIL_SUBJECT, FALSE );
                if ( pMailSubject )
                    aModel.SetSubject( pMailSubject->GetValue() );

                SFX_REQUEST_ARG(rReq, pMailRecipient, SfxStringItem, SID_MAIL_RECIPIENT, FALSE );
                if ( pMailRecipient )
                {
                    String aRecipient( pMailRecipient->GetValue() );
                    String aMailToStr( String::CreateFromAscii( "mailto:" ));

                    if ( aRecipient.Search( aMailToStr ) == 0 )
                        aRecipient = aRecipient.Erase( 0, aMailToStr.Len() );
                    aModel.AddAddress( aRecipient, SfxMailModel_Impl::ROLE_TO );
                }

                SfxMailModel_Impl::SendMailResult eResult = SfxMailModel_Impl::SEND_MAIL_ERROR;
                if ( nId == SID_MAIL_SENDDOCASPDF )
                    eResult = aModel.Send( SfxMailModel_Impl::TYPE_ASPDF );
                else
                    eResult = aModel.Send( SfxMailModel_Impl::TYPE_SELF );

                if ( eResult == SfxMailModel_Impl::SEND_MAIL_ERROR )
                {
                    InfoBox aBox( SFX_APP()->GetTopWindow(), SfxResId( MSG_ERROR_SEND_MAIL ));
                    aBox.Execute();
                    rReq.Ignore();
                }
                else
                    rReq.Done();
            }

            break;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        case SID_PLUGINS_ACTIVE:
        {
            SFX_REQUEST_ARG(rReq, pShowItem, SfxBoolItem, nId, FALSE);
            BOOL bActive = pShowItem ? pShowItem->GetValue() : !pImp->bPlugInsActive;
            // ggf. recorden
            if ( !rReq.IsAPI() )
                rReq.AppendItem( SfxBoolItem( nId, bActive ) );

            // Jetzt schon DONE aufrufen, da die Argumente evtl. einen Pool
            // benutzen, der demn"achst weg ist
            rReq.Done(TRUE);

            // ausfuehren
            if ( !pShowItem || bActive != pImp->bPlugInsActive )
            {
                SfxFrame *pFrame = GetFrame()->GetTopFrame();
                if ( pFrame != GetFrame()->GetFrame() )
                {
                    // FramesetDocument
                    SfxViewShell *pShell = pFrame->GetCurrentViewFrame()->GetViewShell();
                    if ( pShell->GetInterface()->GetSlot( nId ) )
                        pShell->ExecuteSlot( rReq );
                    break;
                }

                SfxFrameIterator aIter( *pFrame );
                while ( pFrame )
                {
                    if ( pFrame->GetCurrentViewFrame() )
                    {
                        SfxViewShell *pView = pFrame->GetCurrentViewFrame()->GetViewShell();
                        if ( pView )
                        {
                            pView->pImp->bPlugInsActive = bActive;
                            SfxInPlaceObject *pObj =
                                pFrame->GetCurrentDocument()->GetInPlaceObject();
                            if ( pObj )
                            {
                                Rectangle aVisArea = pObj->GetVisArea();
                                VisAreaChanged(aVisArea);
                            }
                        }
                    }

                    if ( !pFrame->GetParentFrame() )
                        pFrame = aIter.FirstFrame();
                    else
                        pFrame = aIter.NextFrame( *pFrame );
                }
            }

            break;
        }
    }
}

//--------------------------------------------------------------------

void SfxViewShell::GetState_Impl( SfxItemSet &rSet )
{
    DBG_CHKTHIS(SfxViewShell, 0);

    SfxWhichIter aIter( rSet );
    for ( USHORT nSID = aIter.FirstWhich(); nSID; nSID = aIter.NextWhich() )
    {
        switch ( nSID )
        {
            case SID_STYLE_CATALOG:
            {
                if ( !GetViewFrame()->KnowsChildWindow( SID_STYLE_DESIGNER ) )
                    rSet.DisableItem( nSID );
                break;
            }

            // Printer-Funktionen
            case SID_PRINTDOC:
            case SID_PRINTDOCDIRECT:
            case SID_SETUPPRINTER:
            {
                BOOL bEnabled = pImp->bCanPrint && !pImp->nPrinterLocks;
                if ( bEnabled )
                {
                    SfxPrinter *pPrinter = GetPrinter(FALSE);
                    bEnabled = !pPrinter || !pPrinter->IsPrinting();
                }
                if ( !bEnabled )
                {
                    rSet.DisableItem( SID_PRINTDOC );
                    rSet.DisableItem( SID_PRINTDOCDIRECT );
                    rSet.DisableItem( SID_SETUPPRINTER );
                }
                break;
            }

            // Mail-Funktionen
            case SID_MAIL_SENDDOCASPDF:
            case SID_MAIL_SENDDOC:
            {
                BOOL bEnable = !GetViewFrame()->HasChildWindow( SID_MAIL_CHILDWIN );
                if ( !bEnable )
                    rSet.DisableItem( nSID );
                else if ( nSID == SID_MAIL_SENDDOCASPDF )
                {
                    SfxObjectShellRef xDocShell = GetViewFrame()->GetObjectShell();
                    if ( xDocShell.Is() )
                    {
                        // Get PDF Filter
                        String aPDFExtension = String::CreateFromAscii( ".pdf" );
                        const SfxFilter* pFilter =
                            SfxFilterMatcher( String::CreateFromAscii( xDocShell->GetFactory().GetShortName()) ).GetFilter4Extension( aPDFExtension, SFX_FILTER_EXPORT );
                        if ( pFilter != NULL )
                            break;
                    }

                    rSet.DisableItem( nSID );
                }

                break;
            }

            // PlugIns running
            case SID_PLUGINS_ACTIVE:
            {
                rSet.Put( SfxBoolItem( SID_PLUGINS_ACTIVE, !pImp->bPlugInsActive) );
                break;
            }
/*
            // SelectionText
            case SID_SELECTION_TEXT:
            {
                rSet.Put( SfxStringItem( SID_SELECTION_TEXT, GetSelectionText() ) );
                break;
            }

            // SelectionTextExt
            case SID_SELECTION_TEXT_EXT:
            {
                rSet.Put( SfxStringItem( SID_SELECTION_TEXT_EXT, GetSelectionText(TRUE) ) );
                break;
            }
*/
            case SID_STYLE_FAMILY :
            {
                rSet.Put( SfxUInt16Item( SID_STYLE_FAMILY, pImp->nFamily ) );
                break;
            }
        }
    }
}

//--------------------------------------------------------------------

void SfxViewShell::SetZoomFactor( const Fraction &rZoomX,
                                  const Fraction &rZoomY )
{
    DBG_ASSERT( GetWindow(), "no window" );
    MapMode aMap( GetWindow()->GetMapMode() );
    aMap.SetScaleX( rZoomX );
    aMap.SetScaleY( rZoomY );
    GetWindow()->SetMapMode( aMap );
}

//--------------------------------------------------------------------

SfxInPlaceClient* SfxViewShell::CreateIPClient
(
    WorkWindow*     pTop,
    WorkWindow*     pDoc,
    Window*         pDraw
)

/*  [Beschreibung]

    "Uber diese Factory-Methode erzeugt der SFx spezielle SfxShell-Instanzen,
    die als Stellvertreter f"ur InPlace-Objekte dienen. Solle  auf Callbacks
    dieser Stellvertreter reagiert werden, die dieses "uber eine eigene
    SfxInPlaceClient-Subklasse zu realisieren und diese Factory zu "uberladen.
*/

{
    HACK(kann raus)
    return new SfxInPlaceClient( this, pDraw );
}

//--------------------------------------------------------------------

void SfxViewShell::UIActivate
(
    SvInPlaceObject*    pObj    // das Objekt, welched deaktiv wurde
)

/*  [Beschreibung]

    Dieser Handler wird gerufen, wenn ein mit der Methode
    <SfxViewShell::DoVerb()> angesteuertes Objekt aktiviert
    wurde.

    Die Basisimplementation braucht nicht gerufen zu werden.


    [Querverweise]
    <SfxViewShell::UIDeactivate(SvInPlaceObject)>
*/

{
}

//--------------------------------------------------------------------

void SfxViewShell::UIDeactivate
(
    SvInPlaceObject*    pObj    // das Objekt, welched deaktiv wurde
)

/*  [Beschreibung]

    Dieser Handler wird gerufen, wenn ein mit der Methode
    <SfxViewShell::DoVerb()> aktiviertes Objekt deaktiviert
    wurde. Dies kann von der Server-Applikation oder durch die Client-
    Applikation hervorgerufen worden sein.

    Die Basisimplementation braucht nicht gerufen zu werden.


    [Querverweise]
    <SfxViewShell::UIDeactivate(SvInPlaceObject)>
*/

{
}

//--------------------------------------------------------------------

ErrCode SfxViewShell::DoVerb
(
    SfxInPlaceClient*   pIPClient,  // zu aktivierender <SfxInPlaceClient>
    long                nVerb       // auszuf"uhrendes Verb (default = 0)
)

/*  [Beschreibung]

    [Beispiel]

    ErrCode SwView::DoVerb( SwSoNode *pSoNode, long nVerb )
    {
        SwIPClientRef xIPCli = (SwIPClient*)
                pViewSh->FindIPClient( pSoNode->GetObject() );
        if ( !xIPCli.Is() )
            xIPCli = new SwIPClient( pViewSh, pEditWin, .... );
        pSoNode->GetObject()->DoConnect( xIPCli );
        xIPCli->GetEnv()->SetObjArea( Rectangle( ... ) );
        ...
        return DoVerb( xIPCli, nVerb );
    }
*/

{
    DBG_ASSERT( pIPClient->GetEmbedObj(), "DoVerb aber nicht connected?!" );

    // falls neu, in Liste eintragen
    // Passiert jetzt schon im Connect !
//  if ( LIST_ENTRY_NOTFOUND == aIPClientList.GetPos( pIPClient ) )
//      aIPClientList.Insert(pIPClient);

    // und ab gehts (kein SetModified rufen, das mach das Obj schon selbst)
    SfxErrorContext aEc( ERRCTX_SO_DOVERB, GetWindow(), RID_SO_ERRCTX );
    SvPersist* pPersist = GetObjectShell()->GetInPlaceObject();
    if ( !pPersist )
        pPersist = GetObjectShell();
    pPersist->StartActivation( pIPClient->GetEmbedObj() );
    ErrCode nErr = pIPClient->GetEmbedObj()->DoVerb( nVerb );
    if( nErr )
        ErrorHandler::HandleError( nErr );
    return nErr;
}

//--------------------------------------------------------------------

SfxInPlaceClient* SfxViewShell::FindIPClient
(
    SvEmbeddedObject*   pObj,           /*  <SfxInPlaceClient> f"ur dieses
                                            <SvEmbeddedObject> suchen */
    Window*             pObjParentWin   /*  SfxInPlaceClient, der in diesem
                                            Window dargestellt wird */
)   const

/*  [Beschreibung]

    Sucht nach einem exisitierenden SfxInPlaceClient f"ur das angegebene
    Objekt/Window-Paar.

    Wird keins gefunden, wird 0 zur"uckgegeben.


    [Querverweise]

    <SfxViewShell::DoVerb(SfxInPlaceClient*,long)>
*/

{
    SvInPlaceClientMemberList *pClients = GetIPClientList_Impl(FALSE);
    if ( !pClients )
        return 0;

    if( !pObjParentWin )
        pObjParentWin = GetWindow();
    for (USHORT n=0; n < pClients->Count(); n++)
    {
        SfxInPlaceClient *pIPClient = (SfxInPlaceClient*) pClients->GetObject(n);
        if ( pIPClient->GetEmbedObj() == pObj &&
             pIPClient->GetEnv()->GetEditWin() == pObjParentWin )
            return pIPClient;
    }

    return 0;
}

//--------------------------------------------------------------------

SfxInPlaceClient* SfxViewShell::GetIPClient() const
{
    return GetUIActiveClient();
}

//--------------------------------------------------------------------

SfxInPlaceClient* SfxViewShell::GetUIActiveClient() const
{
    SvInPlaceClientMemberList *pClients = GetIPClientList_Impl(FALSE);
    if ( !pClients )
        return 0;

    SvInPlaceClient *pIPClient=0;
    SvInPlaceClientRef aIPClient;

    for (USHORT n=0; n < pClients->Count(); n++)
    {
        aIPClient = pClients->GetObject(n);
        if( aIPClient.Is() )
        {
            if (aIPClient->GetProtocol().IsUIActive())
            {
                pIPClient = aIPClient;
                break;
            }
        }
    }

    return (SfxInPlaceClient*) pIPClient;
}

//--------------------------------------------------------------------

void SfxViewShell::Activate( BOOL bMDI )
{
    DBG_CHKTHIS(SfxViewShell, 0);
    if ( bMDI )
    {
        SfxObjectShell *pSh = GetViewFrame()->GetObjectShell();
        if ( pSh->GetModel().is() )
            pSh->GetModel()->setCurrentController( GetViewFrame()->GetFrame()->GetController() );
        if ( pSh && pSh->GetMedium() && pSh->GetMedium()->GetName().Len() )
            INetURLObject::SetBaseURL( pSh->GetBaseURL() );
        else
        {
            INetURLObject aObject( SvtPathOptions().GetWorkPath() );
            aObject.setFinalSlash();
            INetURLObject::SetBaseURL( aObject.GetMainURL( INetURLObject::NO_DECODE ) );
        }

        SfxObjectShell::SetWorkingDocument( pSh );
    }
}

//--------------------------------------------------------------------

void SfxViewShell::Deactivate(BOOL bMDI)
{
    DBG_CHKTHIS(SfxViewShell, 0);
    if ( bMDI )
    {
        SfxObjectShell *pSh = GetViewFrame()->GetObjectShell();
        if ( pSh == SfxObjectShell::GetWorkingDocument() )
            SfxObjectShell::SetWorkingDocument( 0 );
    }
}

//--------------------------------------------------------------------

void SfxViewShell::AdjustPosSizePixel
(
    const Point&    rToolOffset,// linke obere Ecke der Tools im Frame-Window
    const Size&     rSize       // gesamte zur Verf"ugung stehende Gr"o\se
)

{
    DBG_CHKTHIS(SfxViewShell, 0);
}

//--------------------------------------------------------------------

void SfxViewShell::Move()

/*  [Beschreibung]

    Diese virtuelle Methode wird gerufen, wenn das Fenster, in dem die
    SfxViewShell dargestellt wird eine StarView-Move() Nachricht erh"alt.

    Die Basisimplementierung braucht nicht gerufen zu werden.


    [Anmerkung]

    Diese Methode kann dazu verwendet werden, eine Selektion abzubrechen,
    um durch das Moven des Fensters erzeugte Maus-Bewegungen anzufangen.

    Zur Zeit funktioniert die Benachrichtigung nicht In-Place.
*/

{
}

//--------------------------------------------------------------------

void SfxViewShell::OuterResizePixel
(
    const Point&    rToolOffset,// linke obere Ecke der Tools im Frame-Window
    const Size&     rSize       // gesamte zur Verf"ugung stehende Gr"o\se
)

/*  [Beschreibung]

    Diese Methode mu� �berladen werden, um auf "Anderungen der Gr��e
    der View zu reagieren. Dabei definieren wir die View als das Edit-Window
    zuz"uglich der um das Edit-Window angeordnenten Tools (z.B. Lineale).

    Das Edit-Window darf weder in Gr"o\se noch Position ver"andert werden.

    Die Vis-Area der SfxObjectShell, dessen Skalierung und Position
    d"urfen hier ver"andert werden. Der Hauptanwendungsfall ist dabei,
    das Ver"andern der Gr"o\se der Vis-Area.

    "Andert sich durch die neue Berechnung der Border, so mu\s dieser
    mit <SfxViewShell::SetBorderPixel(const SvBorder&)> gesetzt werden.
    Erst nach Aufruf von 'SetBorderPixel' ist das Positionieren von
    Tools erlaubt.


    [Beispiel]

    void AppViewSh::OuterViewResizePixel( const Point &rOfs, const Size &rSz )
    {
        // Tool-Positionen und Gr"o\sen von au\sen berechnen, NICHT setzen!
        // (wegen folgender Border-Berechnung)
        Point aHLinPos...; Size aHLinSz...;
        ...

        // Border f"ur Tools passend zu rSize berechnen und setzen
        SvBorder aBorder...
        SetBorderPixel( aBorder ); // ab jetzt sind Positionierungen erlaubt

        // Tools anordnen
        pHLin->SetPosSizePixel( aHLinPos, aHLinSz );
        ...
    }


    [Querverweise]

    <SfxViewShell::InnerResizePixel(const Point&,const Size& rSize)>
*/

{
    DBG_CHKTHIS(SfxViewShell, 0);
    SetBorderPixel( SvBorder() );
}

//--------------------------------------------------------------------

void SfxViewShell::InnerResizePixel
(
    const Point&    rToolOffset,// linke obere Ecke der Tools im Frame-Window
    const Size&     rSize       // dem Edit-Win zur Verf"ugung stehende Gr"o\se
)

/*  [Beschreibung]

    Diese Methode mu� �berladen werden, um auf "Anderungen der Gr��e
    des Edit-Windows zu reagieren.

    Das Edit-Window darf weder in Gr"o\se noch Position ver"andert werden.
    Weder die Vis-Area der SfxObjectShell noch dessen Skalierung oder
    Position d"urfen ver"andert werden.

    "Andert sich durch die neue Berechnung der Border, so mu\s dieser
    mit <SfxViewShell::SetBorderPixel(const SvBorder&)> gesetzt werden.
    Erst nach Aufruf von 'SetBorderPixel' ist das Positionieren von
    Tools erlaubt.


    [Beispiel]

    void AppViewSh::InnerViewResizePixel( const Point &rOfs, const Size &rSz )
    {
        // Tool-Positionen und Gr"o\sen von innen berechnen, NICHT setzen!
        // (wegen folgender Border-Berechnung)
        Point aHLinPos...; Size aHLinSz...;
        ...

        // Border f"ur Tools passend zu rSz berechnen und setzen
        SvBorder aBorder...
        SetBorderPixel( aBorder ); // ab jetzt sind Positionierungen erlaubt

        // Tools anordnen
        pHLin->SetPosSizePixel( aHLinPos, aHLinSz );
        ...
    }


    [Querverweise]

    <SfxViewShell::OuterResizePixel(const Point&,const Size& rSize)>
*/

{
    DBG_CHKTHIS(SfxViewShell, 0);
    SetBorderPixel( SvBorder() );
}

//--------------------------------------------------------------------

void SfxViewShell::InvalidateBorder()
{
    DBG_CHKTHIS(SfxViewShell, 0);
    DBG_ASSERT( GetViewFrame(), "SfxViewShell without SfxViewFrame" );

    GetViewFrame()->InvalidateBorderImpl( this );
}

//--------------------------------------------------------------------

void SfxViewShell::SetBorderPixel( const SvBorder &rBorder )
{
    DBG_CHKTHIS(SfxViewShell, 0);
    DBG_ASSERT( GetViewFrame(), "SfxViewShell without SfxViewFrame" );

    GetViewFrame()->SetBorderPixelImpl( this, rBorder );
}

//--------------------------------------------------------------------

const SvBorder& SfxViewShell::GetBorderPixel() const
{
    DBG_CHKTHIS(SfxViewShell, 0);
    DBG_ASSERT( GetViewFrame(), "SfxViewShell without SfxViewFrame" );

    return GetViewFrame()->GetBorderPixelImpl( this );
}

//--------------------------------------------------------------------

void SfxViewShell::SetWindow
(
    Window*     pViewPort   // Pointer auf das Datenfenster bzw. 0 im Destruktor
)

/*  [Beschreibung]

    Mit dieser Methode wird der SfxViewShell das Datenfenster mitgeteilt.
    Dieses wird f"ur den In-Place-Container und f"ur das korrekte
    Wiederherstellen des Focus ben"otigt.

    Selbst In-Place-aktiv ist das Umsetzen des ViewPort-Windows verboten.
*/

{
    if( pWindow == pViewPort )
        return;

    // ggf. vorhandene IP-Clients disconnecten
    SvInPlaceClientMemberList *pClients = GetIPClientList_Impl(FALSE);
    if ( pClients )
    {
        SvInPlaceClientRef aIPClient;
        USHORT nCount = (USHORT)pClients->Count();
        for (USHORT n=0; n<nCount; n++)
        {
            // Da beim DisConnect der Client immer aus der Liste entfernt wird,
            // immer das 0-te Element holen
            aIPClient = pClients->GetObject(0);
            if( aIPClient.Is() )
            {
                if (aIPClient->GetIPObj())
                {
                    aIPClient->DoDisconnect();
                    aIPClient.Clear();
                }
            }
        }
    }

    // View-Port austauschen
    BOOL bHadFocus = pWindow ? pWindow->HasChildPathFocus( TRUE ) : FALSE;
    pWindow = pViewPort;

    if( pWindow )
    {
        // Disable automatic GUI mirroring (right-to-left) for document windows
        pWindow->EnableRTL( FALSE );
    }

    if ( bHadFocus && pWindow )
        SFX_APP()->GrabFocus( pWindow );
}

//--------------------------------------------------------------------

Size SfxViewShell::GetOptimalSizePixel() const
{
    DBG_ERROR( "Useless call!" );
    return Size();
}

//------------------------------------------------------------------------

SfxViewShell::SfxViewShell
(
    SfxViewFrame*   pViewFrame,     /*  <SfxViewFrame>, in dem diese View
                                        dargestellt wird */
    USHORT          nFlags          /*  siehe <SfxViewShell-Flags> */
)

:   SfxShell(this)
    ,pImp( new SfxViewShell_Impl )
#if !SFX_VIEWSH_INCLUDES_CLIENTSH_HXX
    ,pIPClientList( 0 )
#endif
    ,pFrame(pViewFrame)
    ,pSubShell(0)
    ,pWindow(0)
    ,bMaximizeFirst( 0 != (nFlags & SFX_VIEW_MAXIMIZE_FIRST) )
    ,bOptimizeEach(0 != (nFlags & SFX_VIEW_OPTIMIZE_EACH))
    ,bNoNewWindow( 0 != (nFlags & SFX_VIEW_NO_NEWWINDOW) )
{
    DBG_CTOR(SfxViewShell, 0);

    pImp->pController = 0;
    pImp->bIsShowView =
        !(SFX_VIEW_NO_SHOW == (nFlags & SFX_VIEW_NO_SHOW));
    pImp->bUseObjectSize =
        SFX_CREATE_MODE_EMBEDDED==pFrame->GetObjectShell()->GetCreateMode() &&
        SFX_VIEW_OBJECTSIZE_EMBEDDED == (nFlags & SFX_VIEW_OBJECTSIZE_EMBEDDED);
    pImp->bCanPrint = SFX_VIEW_CAN_PRINT == (nFlags & SFX_VIEW_CAN_PRINT);
    pImp->bFrameSetImpl = nFlags & SFX_VIEW_IMPLEMENTED_AS_FRAMESET;
    pImp->bHasPrintOptions =
        SFX_VIEW_HAS_PRINTOPTIONS == (nFlags & SFX_VIEW_HAS_PRINTOPTIONS);
    pImp->bPlugInsActive = TRUE;
    pImp->bGotOwnerShip = FALSE;
    if ( pFrame->GetParentViewFrame() )
        pImp->bPlugInsActive = pFrame->GetParentViewFrame()->GetViewShell()->pImp->bPlugInsActive;
    pImp->eScroll = SCROLLING_DEFAULT;
    pImp->pSetDescr = NULL;
    pImp->nPrinterLocks = 0;
    pImp->pMenuBarResId = 0;
    pImp->pAccelResId = 0;
    pImp->pAccel = 0;
    pImp->pMenu = 0;
    pImp->bControllerSet = FALSE;
    pImp->bOwnsMenu = TRUE;
    pImp->nFamily = 0xFFFF;                 // undefined, default set by TemplateDialog
    SetMargin( pFrame->GetMargin_Impl() );

    SetPool( &pViewFrame->GetObjectShell()->GetPool() );
    StartListening(*SFX_APP());

    // in Liste eintragen
    const SfxViewShell *pThis = this; // wegen der kranken Array-Syntax
    SfxViewShellArr_Impl &rViewArr = SFX_APP()->GetViewShells_Impl();
    rViewArr.Insert(pThis, rViewArr.Count() );
}

//--------------------------------------------------------------------

SfxViewShell::~SfxViewShell()
{
    DBG_DTOR(SfxViewShell, 0);

    // aus Liste austragen
    const SfxViewShell *pThis = this;
    SfxViewShellArr_Impl &rViewArr = SFX_APP()->GetViewShells_Impl();
    rViewArr.Remove( rViewArr.GetPos(pThis) );
//  if ( GetViewFrame()->GetFrame()->GetFrameSet_Impl() == pImp->pSetDescr )
//      GetViewFrame()->GetFrame()->SetFrameSet_Impl( NULL );
//  delete pImp->pSetDescr;

/*
    if ( pImp->pMenu && pImp->bOwnsMenu )
    {
        SfxTopViewFrame* pTopView = PTR_CAST( SfxTopViewFrame, GetViewFrame()->GetTopViewFrame() );
        SfxTopFrame *pTop = pTopView ? pTopView->GetTopFrame_Impl() : NULL;
        if ( pTop )
        {
            Menu* pMenu = pImp->pMenu->GetMenu()->GetSVMenu();
            if ( pMenu == pTop->GetMenuBar_Impl() )
                pTop->SetMenuBar_Impl( 0 );
        }

        delete pImp->pMenu;
    }
*/
    if ( pImp->pController )
    {
        pImp->pController->ReleaseShell_Impl();
        pImp->pController->release();
    }

    delete pImp->pMenuBarResId;
    delete pImp;

#if !SFX_VIEWSH_INCLUDES_CLIENTSH_HXX
    delete pIPClientList;
#endif
}

//--------------------------------------------------------------------

USHORT SfxViewShell::PrepareClose
(
    BOOL    bUI,     // TRUE: Dialoge etc. erlaubt, FALSE: silent-mode
    BOOL bForBrowsing
)
{
    SfxPrinter *pPrinter = GetPrinter();
    if ( pPrinter && pPrinter->IsPrinting() )
    {
        if ( bUI )
        {
            InfoBox aInfoBox( &GetViewFrame()->GetWindow(), SfxResId( MSG_CANT_CLOSE ) );
            aInfoBox.Execute();
        }

        return FALSE;
    }

    if( GetViewFrame()->IsInModalMode() )
        return FALSE;

    return TRUE;
}

//--------------------------------------------------------------------

SfxViewShell* SfxViewShell::Current()
{
    SfxViewFrame *pCurrent = SfxViewFrame::Current();
    return pCurrent ? pCurrent->GetViewShell() : NULL;
}

//--------------------------------------------------------------------

SdrView* SfxViewShell::GetDrawView() const

/*  [Beschreibung]

    Diese virtuelle Methode mu\s von den Subklassen "uberladen werden, wenn
    der Property-Editor zur Verf"ugung stehen soll.

    Die Default-Implementierung liefert immer 0.
*/

{
    return 0;
}

//--------------------------------------------------------------------

String SfxViewShell::GetSelectionText
(
    BOOL bCompleteWords     /*  FALSE (default)
                                Nur der tats"achlich selektierte Text wird
                                zur"uckgegeben.

                                TRUE
                                Der selektierte Text wird soweit erweitert,
                                da\s nur ganze W"orter zur"uckgegeben werden.
                                Als Worttrenner gelten White-Spaces und die
                                Satzzeichen �.,;� sowie einfache und doppelte
                                Anf"uhrungszeichen.
                            */
)

/*  [Beschreibung]

    Diese Methode kann von Anwendungsprogrammierer "uberladen werden,
    um einen Text zur"uckzuliefern, der in der aktuellen Selektion
    steht. Dieser wird z.B. beim Versenden (email) verwendet.

    Mit �bCompleteWords == TRUE� ger"ufen, reicht z.B. auch der Cursor,
    der in einer URL steht, um die gesamte URL zu liefern.
*/

{
    return String();
}

//--------------------------------------------------------------------

BOOL SfxViewShell::HasSelection( BOOL ) const

/*  [Beschreibung]

    Mit dieser virtuellen Methode kann z.B. ein Dialog abfragen, ob in der
    aktuellen View etwas selektiert ist. Wenn der Parameter <BOOL> TRUE ist,
    wird abgefragt, ob Text selektiert ist.
*/

{
    return FALSE;
}

//--------------------------------------------------------------------

void SfxViewShell::SetSubShell( SfxShell *pShell )

/*  [Beschreibung]

    Mit dieser Methode kann eine Selektions- oder Cursor-Shell angemeldet
    werden, die automatisch unmittelbar nach der SfxViewShell auf den
    SfxDispatcher gepusht wird, und automatisch umittelbar vor ihr
    gepoppt wird.

    Ist die SfxViewShell-Instanz bereits gepusht, dann wird pShell
    sofort ebenfalls gepusht. Wird mit SetSubShell eine andere SfxShell
    Instanz angemeldet, als vorher angemeldet war, wird die zuvor angemeldete
    ggf. automatisch gepoppt. Mit pShell==0 kann daher die aktuelle
    Sub-Shell abgemeldet werden.
*/

{
    // ist diese ViewShell "uberhaupt aktiv?
    SfxDispatcher *pDisp = pFrame->GetDispatcher();
    if ( pDisp->IsActive(*this) )
    {
        // Dispatcher updaten
        if ( pSubShell )
            pDisp->Pop(*pSubShell);
        if ( pShell )
            pDisp->Push(*pShell);
        pDisp->Flush();
    }

    pSubShell = pShell;
}

void SfxViewShell::AddSubShell( SfxShell& rShell )
{
    pImp->aArr.Insert( &rShell, pImp->aArr.Count() );
    SfxDispatcher *pDisp = pFrame->GetDispatcher();
    if ( pDisp->IsActive(*this) )
    {
        pDisp->Push(rShell);
        pDisp->Flush();
    }
}

void SfxViewShell::RemoveSubShell( SfxShell* pShell )
{
    SfxDispatcher *pDisp = pFrame->GetDispatcher();
    if ( !pShell )
    {
        USHORT nCount = pImp->aArr.Count();
        if ( pDisp->IsActive(*this) )
        {
            for ( USHORT n=nCount; n>0; n-- )
                pDisp->Pop( *pImp->aArr[n-1] );
            pDisp->Flush();
        }

        pImp->aArr.Remove(0, nCount);
    }
    else
    {
        USHORT nPos = pImp->aArr.GetPos( pShell );
        if ( nPos != 0xFFFF )
        {
            pImp->aArr.Remove( nPos );
            if ( pDisp->IsActive(*this) )
            {
                pDisp->RemoveShell_Impl( *pShell );
                pDisp->Flush();
            }
        }
    }
}

SfxShell* SfxViewShell::GetSubShell( USHORT nNo )
{
    USHORT nCount = pImp->aArr.Count();
    if ( nNo<nCount )
        return pImp->aArr[nCount-nNo-1];
    return NULL;
}

void SfxViewShell::PushSubShells_Impl( BOOL bPush )
{
    USHORT nCount = pImp->aArr.Count();
    SfxDispatcher *pDisp = pFrame->GetDispatcher();
    if ( bPush )
    {
        for ( USHORT n=0; n<nCount; n++ )
            pDisp->Push( *pImp->aArr[n] );

//      HACK(evtl. PushSubShells fuer SW virtuell machen oder im SW umbauen)
//      Notify( *this, SfxSimpleHint( SFX_HINT_RESERVED4 ) );
    }
    else if ( nCount )
    {
        pDisp->Pop( *pImp->aArr[0], SFX_SHELL_POP_UNTIL );

//      HACK(evtl. PushSubShells fuer SW virtuell machen oder im SW umbauen)
//      Notify( *this, SfxSimpleHint( SFX_HINT_RESERVED3 ) );
    }

    pDisp->Flush();
}

//--------------------------------------------------------------------

void SfxViewShell::WriteUserData( String &, BOOL bBrowse )
{
}

//--------------------------------------------------------------------

void SfxViewShell::ReadUserData(const String &, BOOL bBrowse )
{
}

void SfxViewShell::ReadUserDataSequence ( const ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >&, sal_Bool bBrowse )
{
}
void SfxViewShell::WriteUserDataSequence ( ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >&, sal_Bool bBrowse )
{
}


//--------------------------------------------------------------------
// returns the first shell of spec. type viewing the specified doc.

SfxViewShell* SfxViewShell::GetFirst
(
    const TypeId*   pType,
    BOOL            bOnlyVisible
)
{
    SfxViewShellArr_Impl &rShells = SFX_APP()->GetViewShells_Impl();

    // search for a SfxViewShell of the specified type
    for ( USHORT nPos = 0; nPos < rShells.Count(); ++nPos )
    {
        SfxViewShell *pShell = rShells.GetObject(nPos);
        if ( !pType || pShell->IsA(*pType) )
            return pShell;
    }

    return 0;
}

//--------------------------------------------------------------------
// returns the next shell of spec. type viewing the specified doc.

SfxViewShell* SfxViewShell::GetNext
(
    const SfxViewShell& rPrev,
    const TypeId*       pType,
    BOOL                bOnlyVisible
)
{
    SfxViewShellArr_Impl &rShells = SFX_APP()->GetViewShells_Impl();

    // refind the specified predecessor
    USHORT nPos;
    for ( nPos = 0; nPos < rShells.Count(); ++nPos )
        if ( rShells.GetObject(nPos) == &rPrev )
            break;

    // search for a Frame of the specified type
    for ( ++nPos; nPos < rShells.Count(); ++nPos )
    {
        SfxViewShell *pShell = rShells.GetObject(nPos);
        if ( !pType || pShell->IsA(*pType) )
            return pShell;
    }
    return 0;
}

//--------------------------------------------------------------------

void SfxViewShell::SFX_NOTIFY( SfxBroadcaster& rBC,
                            const TypeId& rBCType,
                            const SfxHint& rHint,
                            const TypeId& rHintType )
{
    if ( rHint.IsA(TYPE(SfxEventHint)) )
    {
        switch ( ((SfxEventHint&)rHint).GetEventId() )
        {
            case SFX_EVENT_LOADFINISHED:
            {
                if ( GetController().is() )
                {
                    SfxItemSet* pSet = GetObjectShell()->GetMedium()->GetItemSet();
                    SFX_ITEMSET_ARG( pSet, pItem, SfxUnoAnyItem, SID_VIEW_DATA, sal_False );
                    if ( pItem )
                        pImp->pController->restoreViewData( pItem->GetValue() );
                    pSet->ClearItem( SID_VIEW_DATA );
                }
                break;
            }
        }
    }
}

//--------------------------------------------------------------------

FASTBOOL SfxViewShell::KeyInput( const KeyEvent &rKeyEvent )

/*  [Beschreibung]

    Diese Methode f"uhrt das KeyEvent 'rKeyEvent' "uber die an dieser
    SfxViewShell direkt oder indirekt (z.B. via Applikation) konfigurierten
    Tasten (Accelerator) aus.


    [R"uckgabewert]

    FASTBOOL                TRUE
                            die Taste ist konfiguriert, der betreffende
                            Handler wurde gerufen

                            FALSE
                            die Taste ist nicht konfiguriert, es konnte
                            also kein Handler gerufen werden


    [Querverweise]
    <SfxApplication::KeyInput(const KeyEvent&)>
*/
{
    SfxAcceleratorManager* pAccMgr = GetAccMgr_Impl();
    BOOL bRet = ( pAccMgr && pAccMgr->Call( rKeyEvent, GetViewFrame()->GetBindings(), FALSE ) );
    if ( bRet )
        return bRet;
    else
        bRet = SFX_APP()->GetAppAccel_Impl()->Call( rKeyEvent, GetViewFrame()->GetBindings(), FALSE );
    if ( !bRet && rKeyEvent.GetKeyCode().GetCode() == KEY_ESCAPE )
    {
        SfxTopViewFrame *pTop = PTR_CAST( SfxTopViewFrame, GetViewFrame()->GetTopViewFrame() );
        if ( pTop )
        {
            WorkWindow* pWork = (WorkWindow*) pTop->GetTopFrame_Impl()->GetTopWindow_Impl();
            if ( pWork && pWork->IsFullScreenMode() )
            {
                GetViewFrame()->GetDispatcher()->Execute( SID_WIN_FULLSCREEN, SFX_CALLMODE_SLOT );
            }
        }
    }

    return bRet;
}

FASTBOOL SfxViewShell::GlobalKeyInput_Impl( const KeyEvent &rKeyEvent )
{
    SfxAcceleratorManager* pAccMgr = GetAccMgr_Impl();
    BOOL bRet = ( pAccMgr && pAccMgr->Call( rKeyEvent, GetViewFrame()->GetBindings(), TRUE ) );
    if ( !bRet )
        bRet = SFX_APP()->GetAppAccel_Impl()->Call( rKeyEvent, GetViewFrame()->GetBindings(), TRUE );
    return bRet;
}

//--------------------------------------------------------------------

void SfxViewShell::ShowCursor( FASTBOOL bOn )

/*  [Beschreibung]

    Diese Methode mu\s von Subklassen "uberladen werden, damit vom SFx
    aus der Cursor ein- und ausgeschaltet werden kann. Dies geschieht
    z.B. bei laufendem <SfxProgress>.
*/

{
}

//--------------------------------------------------------------------

void SfxViewShell::GotFocus() const

/*  [Beschreibung]

    Diese Methode mu\s vom Applikationsentwickler gerufen werden, wenn
    das Edit-Window den Focus erhalten hat. Der SFx hat so z.B. die
    M"oglichkeit, den Accelerator einzuschalten.


    [Anmerkung]

    <StarView> liefert leider keine M"oglichkeit, solche Events
    'von der Seite' einzuh"angen.
*/

{
}

//--------------------------------------------------------------------

ErrCode SfxViewShell::DoVerb(long nVerb)

/*  [Beschreibung]

    Virtuelle Methode, um am selektierten Objekt ein Verb auszuf"uhren.
    Da dieses Objekt nur den abgeleiteten Klassen bekannt ist, mu� DoVerb
    dort "uberlschrieben werden.

*/

{
    return ERRCODE_SO_NOVERBS;
}

//--------------------------------------------------------------------

void SfxViewShell::DisconnectClients_Impl(SvInPlaceClient *pIP)

/*  [Beschreibung]

    Diese Methode dient dazu, bei UI-Aktivierung eins IPClients alle anderen
    noch bestehenden Verbindungen zu trennen.

*/

{

    // SO2 stellt sicher, da\s nur ein Object gleichzeitig UI-aktiv ist.
    // Aus Speicherplatzgr"unden werden aber alle Objekte, die nicht aktiv
    // sind oder sein m"ussen, disconnected.

    SvInPlaceClientMemberList *pClients = GetIPClientList_Impl(FALSE);
    if ( !pClients )
        return;

    SvInPlaceClientRef aIPClient;
    for ( USHORT n=0; n < pClients->Count(); n++ )
    {
        aIPClient = pClients->GetObject(n);
        if( aIPClient.Is() && aIPClient != pIP && aIPClient->GetIPObj() &&
                !(aIPClient->GetIPObj()->GetMiscStatus() & SVOBJ_MISCSTATUS_ACTIVATEWHENVISIBLE) )
            aIPClient->GetProtocol().Reset2Connect();
    }
}

//--------------------------------------------------------------------

void SfxViewShell::DisconnectAllClients()
{
    SvInPlaceClientMemberList *pClients = GetIPClientList_Impl(FALSE);
    if ( !pClients )
        return;

    SvInPlaceClientRef aIPClient;
    for ( USHORT n=0; n<pClients->Count(); )
    {
        aIPClient = pClients->GetObject(n);
        if( aIPClient.Is() && aIPClient->GetIPObj() )
            aIPClient->DoDisconnect();
        else
            n++;
    }
}

//--------------------------------------------------------------------

BOOL SfxViewShell::UseObjectSize() const
{
    return pImp->bUseObjectSize;
}

//--------------------------------------------------------------------

void SfxViewShell::QueryObjAreaPixel( Rectangle& ) const
{
}

//--------------------------------------------------------------------

void SfxViewShell::AdjustVisArea(const Rectangle& rRect)
{
    DBG_ASSERT (pFrame, "Kein Frame?");

    SfxInPlaceObject *pObj = pFrame->GetObjectShell()->GetInPlaceObject();
    if ( !pObj )
        return;

    if ( UseObjectSize() )
    {
        Point aPos = rRect.TopLeft();
        Size aSize = pObj->GetVisArea().GetSize();
        pObj->SetVisArea( Rectangle(aPos, aSize) );
    }
    else
        pObj->SetVisArea( rRect );
}

//--------------------------------------------------------------------

void SfxViewShell::VisAreaChanged(const Rectangle& rVisArea)
{
    SvInPlaceClientMemberList *pClients = GetIPClientList_Impl(FALSE);
    if ( !pClients )
        return;

    SvInPlaceClientRef aIPClient;
    for (USHORT n=0; n < pClients->Count(); n++)
    {
        // Die Liste enth"alt alle connected clients
        aIPClient = pClients->GetObject(n);
        if( aIPClient.Is() )
            CheckIPClient_Impl( aIPClient, rVisArea );
    }
}


//--------------------------------------------------------------------

void SfxViewShell::CheckIPClient_Impl( SvInPlaceClient *pIPClient,
                                       const Rectangle& rVisArea )
{
    if ( GetObjectShell()->IsInClose() )
        return;

    BOOL bApplets = SvtJavaOptions().IsExecuteApplets();
    BOOL bActive = pIPClient->IsInPlaceActive();
    BOOL bPlugIn = SvtMiscOptions().IsPluginsEnabled();

    SvAppletObjectRef aAppRef = pIPClient->GetIPObj();
    SvPlugInObjectRef aPlugRef = pIPClient->GetIPObj();
    SfxFrameObjectRef aFrameRef = pIPClient->GetIPObj();

    if ( !pImp->bPlugInsActive && ( aPlugRef.Is() || aAppRef.Is() || aFrameRef.Is() ) )
    {
        if ( bActive )
            pIPClient->GetProtocol().Reset2Open();
        return;
    }

    BOOL bAlwaysActive = ( pIPClient->GetIPObj()->GetMiscStatus() == SVOBJ_MISCSTATUS_ALWAYSACTIVATE );

    if ( bAlwaysActive || rVisArea.IsOver( pIPClient->GetClientData()->GetObjArea()) )
    {
        // Der Client ist connected und sichtbar.
        if ( bActive )
        {
            // Wenn er aktiv ist, mu\s er benachrichtigt werden.
            pIPClient->GetEnv()->OutDevScaleChanged();

            if ( aPlugRef.Is() )
            {
                // Plugins sind nur sichtbar, wenn das entsprechende
                // flag auch angeschaltet ist
                if ( !bPlugIn )
                {
                    pIPClient->GetProtocol().Reset2Open();
                }
                else if ( aPlugRef->GetPlugInMode() == PLUGIN_FULL )
                {
                    SetBorderPixel(SvBorder());
                    Window *pWin = GetWindow();
                    Size aSize = pWin->GetOutputSizePixel();
                    aSize = pWin->PixelToLogic(aSize);
                    Rectangle aRect = Rectangle ( pWin->PixelToLogic(Point()), aSize);
                    pIPClient->GetClientData()->SetObjArea(aRect);
                }
            }
            else if ( aAppRef.Is() && !bApplets )
            {
                // Java-Applets sind nicht aktiv, wenn Java in den
                // Options ausgeschaltet wurde
                pIPClient->GetProtocol().Reset2Open();
            }
        }
        else
        {
            // Ist er es nicht, aber da\s Object soll aktiv sein,
            // wenn es sichtbar ist, mu\s das Object aktiviert werden;
            // bei Plugins und Applets sind noch die Flags auszuwerten
            if ( pIPClient->GetIPObj()->GetMiscStatus() &
                        SVOBJ_MISCSTATUS_ACTIVATEWHENVISIBLE )
            {
                BOOL bActivate = TRUE;
                if ( aPlugRef.Is() )
                    bActivate = bPlugIn;
                else if ( aAppRef.Is() )
                    bActivate = bApplets;
                if ( bActivate )
                    pIPClient->GetIPObj()->DoVerb(0);
            }
        }
    }
    else if ( bActive )
    {
        // Wenn er aktiv ist, mu\s er benachrichtigt werden.
        pIPClient->GetEnv()->OutDevScaleChanged();

        // Ein nicht sichtbarer client soll disconnected werden, wenn
        // er (nur) aktiv ist, wenn er sichtbar ist.
        // Wenn er dann wieder sichtbar wird, mu\s die Applikation
        // ihn connecten, damit er in der client list erscheint!

        // Auf Wunsch von MB erst mal entfernt !!
//              if ( pIPClient->GetIPObj()->GetMiscStatus() &
//                  SVOBJ_MISCSTATUS_ACTIVATEWHENVISIBLE )
//                pIPClient->DoDisconnect();
    }
}

//--------------------------------------------------------------------

BOOL SfxViewShell::PlugInsActive() const
{
    return pImp->bPlugInsActive;
}

//--------------------------------------------------------------------

void SfxViewShell::DiscardClients_Impl()

/*  [Beschreibung]

    Diese Methode dient dazu, vor dem Schlie\sen eines Dokuments das
    Speichern der Objekte zu verhindern, wenn der Benutzer Schlie\en ohne
    Speichern gew"ahlt hatte.
*/

{
    SvInPlaceClientMemberList *pClients = GetIPClientList_Impl(FALSE);
    if ( !pClients )
        return;

    SvInPlaceClientRef aIPClient;
    for (USHORT n=0; n < pClients->Count(); n++)
    {
        aIPClient = pClients->GetObject(n);
        if( aIPClient.Is() )
        {
            if (aIPClient->GetIPObj())
            {
                aIPClient->GetIPObj()->SetAutoSave(FALSE);
                aIPClient->DoDisconnect();
                aIPClient.Clear();
            }
        }
    }
}

//--------------------------------------------------------------------

SfxScrollingMode SfxViewShell::GetScrollingMode() const
{
    return pImp->eScroll;
}

//--------------------------------------------------------------------

void SfxViewShell::SetScrollingMode( SfxScrollingMode eMode )
{
    pImp->eScroll = eMode;
}

//--------------------------------------------------------------------

SfxFrameSetDescriptor* SfxViewShell::GetFrameSet_Impl() const
{
    return pImp->pSetDescr;
}

//--------------------------------------------------------------------

void SfxViewShell::SetFrameSet_Impl( SfxFrameSetDescriptor* pD )
{
//  delete pImp->pSetDescr;
    pImp->pSetDescr = pD;
}

//--------------------------------------------------------------------

SfxObjectShell* SfxViewShell::GetObjectShell()
{
    return GetViewFrame()->GetObjectShell();
}

//--------------------------------------------------------------------

const Size& SfxViewShell::GetMargin() const
{
    return pImp->aMargin;
}

//--------------------------------------------------------------------

void SfxViewShell::SetMargin( const Size& rSize )
{
    // Der default-Margin wurde "geeicht" mit www.apple.com !!
    Size aMargin = rSize;
    if ( aMargin.Width() == -1 )
        aMargin.Width() = DEFAULT_MARGIN_WIDTH;
    if ( aMargin.Height() == -1 )
        aMargin.Height() = DEFAULT_MARGIN_HEIGHT;

    if ( aMargin != pImp->aMargin )
    {
        pImp->aMargin = aMargin;
        MarginChanged();
    }
}

//--------------------------------------------------------------------

void SfxViewShell::MarginChanged()
{
}

//--------------------------------------------------------------------

BOOL SfxViewShell::IsShowView_Impl() const
{
    return pImp->bIsShowView;
}

//--------------------------------------------------------------------

SfxFrame* SfxViewShell::GetSmartSelf( SfxFrame* pSelf, SfxMedium& rMedium )
{
    return pSelf;
}

//--------------------------------------------------------------------

BOOL SfxViewShell::IsImplementedAsFrameset_Impl( ) const
{
    return pImp->bFrameSetImpl;
}

//------------------------------------------------------------------------

void SfxViewShell::JumpToMark( const String& rMark )
{
    SfxStringItem aMarkItem( SID_JUMPTOMARK, rMark );
    GetViewFrame()->GetDispatcher()->Execute(
            SID_JUMPTOMARK,
            SFX_CALLMODE_SYNCHRON|SFX_CALLMODE_RECORD,
            &aMarkItem, 0L );
}

//------------------------------------------------------------------------

#if !SFX_VIEWSH_INCLUDES_CLIENTSH_HXX

void SfxViewShell::NewIPClient_Impl( SfxInPlaceClient *pIPClient )
{
    SvInPlaceClientMemberList *pClients = GetIPClientList_Impl(TRUE);
    pClients->Insert(pIPClient);
}

void SfxViewShell::IPClientGone_Impl( SfxInPlaceClient *pIPClient )
{
    SvInPlaceClientMemberList *pClients = GetIPClientList_Impl(TRUE);
    pClients->Remove(pIPClient);
}

SvInPlaceClientMemberList* SfxViewShell::GetIPClientList_Impl( BOOL bCreate ) const
{
    if ( !pIPClientList && bCreate )
        ( (SfxViewShell*) this )->pIPClientList = new SvInPlaceClientMemberList;
    return pIPClientList;
}

#endif

void SfxViewShell::ReleaseMenuBar_Impl()
{
    pImp->bOwnsMenu = FALSE;
}

SfxMenuBarManager* SfxViewShell::GetMenuBar_Impl( BOOL bPlugin )
{
    // get the accelerators
    GetAccMgr_Impl();

    Reference < XPropertySet > xPropSet( GetViewFrame()->GetFrame()->GetFrameInterface(), UNO_QUERY );
    if ( xPropSet.is() )
    {
        Reference< drafts::com::sun::star::frame::XLayoutManager > xLayoutManager;

        if ( xPropSet.is() )
        {
            Any aValue = xPropSet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "LayoutManager" )));
            aValue >>= xLayoutManager;
        }

        if ( xLayoutManager.is() )
        {
            rtl::OUString aMenuBarURL( RTL_CONSTASCII_USTRINGPARAM( "private:resource/menubar/menubar" ));
            Reference< drafts::com::sun::star::ui::XUIElement > xMenuBarElement( xLayoutManager->getElement( aMenuBarURL ));
            if ( !xMenuBarElement.is() )
                GetObjectShell()->CreateMenuBarManager_Impl( GetViewFrame() );
        }
    }

    return NULL;
}

SfxAcceleratorManager* SfxViewShell::GetAccMgr_Impl()
{
    // all views of a document share the accelerators
#if SUPD>633
    return GetObjectShell()->GetAccMgr_Impl();
#else
    return GetObjectShell()->GetFactory().GetAccMgr_Impl();
#endif
}

void SfxViewShell::SetController( SfxBaseController* pController )
{
    pImp->pController = pController;
    pImp->pController->acquire();
    pImp->bControllerSet = TRUE;
}

Reference < XController > SfxViewShell::GetController()
{
    if ( !pImp->pController )
        new SfxBaseController( this );
    return pImp->pController;
}

void SfxViewShell::AddContextMenuInterceptor_Impl( const REFERENCE< XCONTEXTMENUINTERCEPTOR >& xInterceptor )
{
    pImp->aInterceptorContainer.addInterface( xInterceptor );
}

void SfxViewShell::RemoveContextMenuInterceptor_Impl( const REFERENCE< XCONTEXTMENUINTERCEPTOR >& xInterceptor )
{
    pImp->aInterceptorContainer.removeInterface( xInterceptor );
}

::cppu::OInterfaceContainerHelper& SfxViewShell::GetContextMenuInterceptors() const
{
    return pImp->aInterceptorContainer;
}

void Change( Menu* pMenu, SfxViewShell* pView )
{
    SfxDispatcher *pDisp = pView->GetViewFrame()->GetDispatcher();
    USHORT nCount = pMenu->GetItemCount();
    for ( USHORT nPos=0; nPos<nCount; ++nPos )
    {
        USHORT nId = pMenu->GetItemId(nPos);
        String aCmd = pMenu->GetItemCommand(nId);
        PopupMenu* pPopup = pMenu->GetPopupMenu(nId);
        if ( nId < 5000 )
        {
            if ( aCmd.CompareToAscii(".uno:", 5) == 0 )
            {
                SfxShell *pShell=0;
                USHORT nIdx;
                for (nIdx=0; (pShell=pDisp->GetShell(nIdx)); nIdx++)
                {
                    const SfxInterface *pIFace = pShell->GetInterface();
                    const SfxSlot* pSlot = pIFace->GetSlot( aCmd );
                    if ( pSlot )
                    {
                        pMenu->InsertItem( pSlot->GetSlotId(), pMenu->GetItemText( nId ), pMenu->GetItemBits( nId ), nPos );
                        pMenu->RemoveItem( nPos+1 );
                        break;
                    }
                }
            }
        }
        if ( pPopup )
        {
            Change( pPopup, pView );
        }
    }
}


BOOL SfxViewShell::TryContextMenuInterception( Menu& rIn, Menu*& rpOut, ::com::sun::star::ui::ContextMenuExecuteEvent aEvent )
{
    rpOut = NULL;
    BOOL bModified = FALSE;

    // create container from menu
    // #110897#
    // aEvent.ActionTriggerContainer = ::framework::ActionTriggerHelper::CreateActionTriggerContainerFromMenu( &rIn );
    aEvent.ActionTriggerContainer = ::framework::ActionTriggerHelper::CreateActionTriggerContainerFromMenu(
        ::comphelper::getProcessServiceFactory(), &rIn );

    // get selection from controller
    aEvent.Selection = ::com::sun::star::uno::Reference < ::com::sun::star::view::XSelectionSupplier > ( GetController(), ::com::sun::star::uno::UNO_QUERY );

    // call interceptors
    ::cppu::OInterfaceIteratorHelper aIt( pImp->aInterceptorContainer );
    while( aIt.hasMoreElements() )
    {
        try
        {
            ::com::sun::star::ui::ContextMenuInterceptorAction eAction =
                ((::com::sun::star::ui::XContextMenuInterceptor*)aIt.next())->notifyContextMenuExecute( aEvent );
            switch ( eAction )
            {
                case ::com::sun::star::ui::ContextMenuInterceptorAction_CANCELLED :
                    // interceptor does not want execution
                    return FALSE;
                    break;
                case ::com::sun::star::ui::ContextMenuInterceptorAction_EXECUTE_MODIFIED :
                    // interceptor wants his modified menu to be executed
                    bModified = TRUE;
                    break;
                case ::com::sun::star::ui::ContextMenuInterceptorAction_CONTINUE_MODIFIED :
                    // interceptor has modified menu, but allows for calling other interceptors
                    bModified = TRUE;
                    continue;
                    break;
                case ::com::sun::star::ui::ContextMenuInterceptorAction_IGNORED :
                    // interceptor is indifferent
                    continue;
                    break;
                default:
                    DBG_ERROR("Wrong return value of ContextMenuInterceptor!");
                    continue;
                    break;
            }
        }
        catch( ::com::sun::star::uno::RuntimeException& )
        {
            aIt.remove();
        }

        break;
    }

    if ( bModified )
    {
        // container was modified, create a new window out of it
        rpOut = new PopupMenu;
        ::framework::ActionTriggerHelper::CreateMenuFromActionTriggerContainer( rpOut, aEvent.ActionTriggerContainer );

        Change( rpOut, this );
    }

    return TRUE;
}

void SfxViewShell::TakeOwnerShip_Impl()
{
    // currently there is only one reason to take OwnerShip: a hidden frame is printed
    // so the ViewShell will check this on EndPrint (->prnmon.cxx)
    pImp->bGotOwnerShip = TRUE;
}

BOOL SfxViewShell::GotOwnerShip_Impl()
{
    return pImp->bGotOwnerShip;
}

long SfxViewShell::HandleNotifyEvent_Impl( NotifyEvent& rEvent )
{
    if ( pImp->pController )
        return pImp->pController->HandleEvent_Impl( rEvent );
    return 0;
}

BOOL SfxViewShell::HasKeyListeners_Impl()
{
    return pImp->pController ? pImp->pController->HasKeyListeners_Impl() : FALSE;
}

BOOL SfxViewShell::HasMouseClickListeners_Impl()
{
    return pImp->pController ? pImp->pController->HasMouseClickListeners_Impl() : FALSE;
}

void SfxViewShell::SetAdditionalPrintOptions( const com::sun::star::uno::Sequence < com::sun::star::beans::PropertyValue >& rOpts )
{
    pImp->aPrintOpts = rOpts;
     GetObjectShell()->Broadcast( SfxPrintingHint( -3, NULL, NULL, rOpts ) );
}
