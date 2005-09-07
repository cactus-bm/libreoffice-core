/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: prnmon.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 19:29:42 $
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

#ifndef _COM_SUN_STAR_VIEW_PRINTABLESTATE_HPP_
#include <com/sun/star/view/PrintableState.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_XCLOSEABLE_HPP_
#include <com/sun/star/util/XCloseable.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_XCLOSEBROADCASTER_HPP_
#include <com/sun/star/util/XCloseBroadcaster.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_XCLOSELISTENER_HPP_
#include <com/sun/star/util/XCloseListener.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_CLOSEVETOEXCEPTION_HPP_
#include <com/sun/star/util/CloseVetoException.hpp>
#endif

#ifndef _SV_FIXED_HXX
#include <vcl/fixed.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif

#ifndef   SVTOOLS_ASYNCLINK_HXX
#include <svtools/asynclink.hxx>
#endif

#include <svtools/printwarningoptions.hxx>

#ifndef GCC
#pragma hdrstop
#endif

#include "prnmon.hxx"
#include "viewsh.hxx"
#include "viewfrm.hxx"
#include "objsh.hxx"
#include "docfile.hxx"
#include "sfxtypes.hxx"
#include "progress.hxx"
#include "desrupt.hxx"
#include "bindings.hxx"
#include "sfxresid.hxx"
#include "event.hxx"

#include "view.hrc"

//------------------------------------------------------------------------

#define SFX_TITLE_MAXLEN_PRINTMONITOR   22

//------------------------------------------------------------------------

struct SfxPrintMonitor_Impl: public ModelessDialog
{

    SfxPrintMonitor_Impl( Window *pParent, SfxViewShell *rpViewShell );
    virtual BOOL Close();

    SfxViewShell*           pViewShell;
    FixedText               aDocName;
    FixedText               aPrinting;
    FixedText               aPrinter;
    FixedText               aPrintInfo;
    CancelButton            aCancel;
};

//-------------------------------------------------------------------------

struct SfxPrintProgress_Impl : public SfxListener
{
    SfxPrintMonitor_Impl*   pMonitor;
    SfxViewShell*           pViewShell;
    SfxPrinter*             pPrinter;
    SfxPrinter*             pOldPrinter;
    USHORT                  nLastPage;
    BOOL                    bRunning;
    BOOL                    bCancel;
    BOOL                    bDeleteOnEndPrint;
    BOOL                    bShow;
    BOOL                    bCallbacks;
    BOOL                    bOldEnablePrintFile;
    BOOL                    bOldFlag;
    BOOL                    bRestoreFlag;
    BOOL                    bAborted;
    svtools::AsynchronLink  aDeleteLink;
    Link                    aCancelHdl;

private:
    DECL_LINK( CancelHdl, Button * );
    DECL_STATIC_LINK( SfxPrintProgress_Impl, DeleteHdl, SfxPrintProgress * );

public:
    SfxPrintProgress_Impl( SfxViewShell* pTheViewShell, SfxPrinter* pThePrinter );
    ~SfxPrintProgress_Impl();

    void                    Delete( SfxPrintProgress* pAntiImpl ) { aDeleteLink.Call( pAntiImpl ); }
    SfxViewShell*           GetViewShell() const { return pViewShell; }
    BOOL                    SetPage( USHORT nPage, const String &rPage );
    virtual void            SFX_NOTIFY( SfxBroadcaster& rBC,
                                        const TypeId& rBCType,
                                        const SfxHint& rHint,
                                        const TypeId& rHintType );
};

void SfxPrintProgress_Impl::SFX_NOTIFY( SfxBroadcaster& rBC,
                            const TypeId& rBCType,
                            const SfxHint& rHint,
                            const TypeId& rHintType )
{
    SfxPrintingHint* pPrintHint = PTR_CAST( SfxPrintingHint, &rHint );
    if ( pPrintHint )
    {
        if ( pPrintHint->GetWhich() == -2 )
            CancelHdl(0);
    }
}

//------------------------------------------------------------------------

SfxPrintMonitor_Impl::SfxPrintMonitor_Impl( Window* pParent, SfxViewShell* rpViewShell ) :

    ModelessDialog( pParent, SfxResId( DLG_PRINTMONITOR ) ),

    aDocName    ( this, ResId( FT_DOCNAME ) ),
    aPrinting   ( this, ResId( FT_PRINTING ) ),
    aPrinter    ( this, ResId( FT_PRINTER ) ),
    aPrintInfo  ( this, ResId( FT_PRINTINFO ) ),
    aCancel     ( this, ResId( PB_CANCELPRNMON ) ),
    pViewShell  ( rpViewShell )

{
    FreeResource();
}

//------------------------------------------------------------------------

IMPL_STATIC_LINK( SfxPrintProgress_Impl, DeleteHdl, SfxPrintProgress*, pAntiImpl )
{
    delete pAntiImpl;
    return 0;
}

void actualizePrintCancelState(CancelButton& rButton, const SfxObjectShell* pShell)
{
    sal_Bool bEnableCancelButton = sal_True;
    if (pShell)
        bEnableCancelButton = pShell->Stamp_GetPrintCancelState();

    if (!bEnableCancelButton)
        rButton.Disable();
    else
        rButton.Enable();
}

//------------------------------------------------------------------------

SfxPrintProgress_Impl::SfxPrintProgress_Impl( SfxViewShell* pTheViewShell,
                                              SfxPrinter* pThePrinter ) :

    pViewShell          ( pTheViewShell ),
    pPrinter            ( pThePrinter ),
    pOldPrinter         ( NULL ),
    nLastPage           ( 0 ),
    bRunning            ( TRUE ),
    bCancel             ( FALSE ),
    bDeleteOnEndPrint   ( FALSE ),
    bCallbacks          ( FALSE ),
    bOldEnablePrintFile ( FALSE ),
    bOldFlag            ( TRUE ),
    bRestoreFlag        ( FALSE ),
    bAborted            ( FALSE ),
    aDeleteLink         ( STATIC_LINK( this, SfxPrintProgress_Impl, DeleteHdl ) )

{
    Window* pParent =
        pTheViewShell->GetWindow()->IsReallyVisible() ? pTheViewShell->GetWindow() : NULL;
    pMonitor = new SfxPrintMonitor_Impl( pParent, pViewShell );
    pMonitor->aDocName.SetText(
        pViewShell->GetViewFrame()->GetObjectShell()->GetTitle( SFX_TITLE_MAXLEN_PRINTMONITOR ) );
    pMonitor->aPrinter.SetText( pViewShell->GetPrinter()->GetName() );

    // Stampit enable/dsiable cancel button
    actualizePrintCancelState(pMonitor->aCancel, pViewShell->GetObjectShell());

    pMonitor->aCancel.SetClickHdl( LINK( this, SfxPrintProgress_Impl, CancelHdl ) );

    StartListening( *pViewShell->GetObjectShell() );
}

//------------------------------------------------------------------------

SfxPrintProgress_Impl::~SfxPrintProgress_Impl()
{
}

//------------------------------------------------------------------------

BOOL SfxPrintProgress_Impl::SetPage( USHORT nPage, const String &rPage )
{
    // wurde der Druckauftrag abgebrochen?
    if ( bCancel || !pMonitor )
        return FALSE;

    // Stampit enable/dsiable cancel button
    actualizePrintCancelState(pMonitor->aCancel, pViewShell->GetObjectShell());

    nLastPage = nPage;
    String aStrPrintInfo = String( SfxResId( STR_PAGE ) );
    if ( !rPage.Len() )
        aStrPrintInfo += String::CreateFromInt32( nLastPage );
    else
        aStrPrintInfo += rPage;
    pMonitor->aPrintInfo.SetText( aStrPrintInfo );
    pMonitor->Update();
    return TRUE;
}

//------------------------------------------------------------------------

IMPL_LINK( SfxPrintProgress_Impl, CancelHdl, Button *, pButton )
{
    if ( pMonitor )
        pMonitor->Hide();

    pViewShell->GetObjectShell()->Broadcast( SfxPrintingHint( com::sun::star::view::PrintableState_JOB_ABORTED, NULL, NULL ) );
    pViewShell->GetPrinter()->AbortJob();
    bCancel = TRUE;

    if ( aCancelHdl.IsSet() )
        aCancelHdl.Call( this );

    bAborted = TRUE;
    return 0;
}

BOOL SfxPrintMonitor_Impl::Close()
{
    BOOL bAgree = pViewShell ? pViewShell->GetObjectShell()->Stamp_GetPrintCancelState() : TRUE;
    if (!bAgree)
        return FALSE;
    else
        return ModelessDialog::Close();
}

//--------------------------------------------------------------------

SfxPrintProgress::SfxPrintProgress( SfxViewShell* pViewSh, FASTBOOL bShow )
:   SfxProgress( pViewSh->GetViewFrame()->GetObjectShell(),
                 String(SfxResId(STR_PRINTING)), 1, FALSE ),
    pImp( new SfxPrintProgress_Impl( pViewSh, pViewSh->GetPrinter() ) )
{
    // Callback fuer Fehler und EndPrint setzen
    pImp->pPrinter->SetEndPrintHdl(
                LINK( this, SfxPrintProgress, EndPrintNotify ));
    pImp->pPrinter->SetErrorHdl(
                LINK( this, SfxPrintProgress, PrintErrorNotify ));
    pImp->bCallbacks = TRUE;

    //pImp->pViewShell->GetViewFrame()->GetFrame()->Lock_Impl(TRUE);
    pImp->bShow = bShow;
    Lock();
    if ( !SvtPrintWarningOptions().IsModifyDocumentOnPrintingAllowed() )
    {
        pImp->bRestoreFlag = TRUE;
        pImp->bOldFlag = pViewSh->GetObjectShell()->IsEnableSetModified();
        if ( pImp->bOldFlag )
            pViewSh->GetObjectShell()->EnableSetModified( FALSE );
    }
}

//--------------------------------------------------------------------

SfxPrintProgress::~SfxPrintProgress()
{
    // k"onnte auch schon weg sein (in EndPrintNotify)
    DELETEZ(pImp->pMonitor);

    // ggf. Callbacks entfermen
    if ( pImp->bCallbacks )
    {
        pImp->pPrinter->SetEndPrintHdl( Link() );
        pImp->pPrinter->SetErrorHdl( Link() );
        pImp->bCallbacks = FALSE;
    }

    // ggf. vorherigen Drucker wieder einsetzen
    if ( pImp->pOldPrinter )
        pImp->pViewShell->SetPrinter( pImp->pOldPrinter, SFX_PRINTER_PRINTER );
    else
        // ggf. vorherigen Print-To-File-Status zuruecksetzen
        pImp->pViewShell->GetPrinter()->EnablePrintFile( pImp->bOldEnablePrintFile );

    // EndPrint-Notification an Frame
    //pImp->pViewShell->GetViewFrame()->GetFrame()->Lock_Impl(FALSE);
    pImp->EndListening( *(pImp->pViewShell->GetObjectShell()) );

    // the following call might destroy the view or even the document
    pImp->pViewShell->CheckOwnerShip_Impl();
    delete pImp;
}

//--------------------------------------------------------------------

BOOL SfxPrintProgress::SetState( ULONG nVal, ULONG nNewRange )
{
#ifndef MAC
    // auf dem MAC kommt einer vom Betriebssystem
    if ( pImp->bShow )
    {
        pImp->bShow = FALSE;
        pImp->pMonitor->Show();
        pImp->pMonitor->Update();
    }
#endif

    return pImp->SetPage( (USHORT)nVal, GetStateText_Impl() ) &&
           SfxProgress::SetState( nVal, nNewRange );
}

//--------------------------------------------------------------------

void SfxPrintProgress::SetText( const String& rText )
{
    if ( pImp->pMonitor )
    {
        pImp->pMonitor->SetText( rText );
        pImp->pMonitor->Update();
    }
    SfxProgress::SetText( rText );
}

//------------------------------------------------------------------------

IMPL_LINK_INLINE_START( SfxPrintProgress, PrintErrorNotify, void *, pvoid )
{
    if ( pImp->pMonitor )
        pImp->pMonitor->Hide();
    // AbortJob calls EndPrint hdl, so do not delete pImp just now
    BOOL bWasDeleteOnEndPrint = pImp->bDeleteOnEndPrint;
    pImp->bDeleteOnEndPrint = FALSE;
    pImp->pPrinter->AbortJob();
    InfoBox( pImp->GetViewShell()->GetWindow(),
             String( SfxResId(STR_ERROR_PRINT) ) ).Execute();
    if ( pImp->bRestoreFlag && pImp->pViewShell->GetObjectShell()->IsEnableSetModified() != pImp->bOldFlag )
        pImp->pViewShell->GetObjectShell()->EnableSetModified( pImp->bOldFlag );
    pImp->GetViewShell()->GetObjectShell()->Broadcast( SfxPrintingHint( com::sun::star::view::PrintableState_JOB_FAILED, NULL, NULL ) );
    // now we can clean up like normally EndPrint hdl does
    if( bWasDeleteOnEndPrint )
    {
        DELETEZ(pImp->pMonitor);
        delete this;
    }
    return 0;
}
IMPL_LINK_INLINE_END( SfxPrintProgress, PrintErrorNotify, void *, pvoid )

//------------------------------------------------------------------------

IMPL_LINK( SfxPrintProgress, EndPrintNotify, void *, pvoid )
{
    if ( pImp->pMonitor )
        pImp->pMonitor->Hide();

    SfxViewShell* pViewShell = pImp->pViewShell;

    // Slots enablen
    pViewShell->Invalidate( SID_PRINTDOC );
    pViewShell->Invalidate( SID_PRINTDOCDIRECT );
    pViewShell->Invalidate( SID_SETUPPRINTER );

    // . . . falls der Printer im System umgestellt wurde, hier Aenderung
    // nachziehen.
    //! if( pMDI->IsPrinterChanged() ) pMDI->Changed( 0L );

    // Callbacks rausnehmen
    pImp->pPrinter->SetEndPrintHdl( Link() );
    pImp->pPrinter->SetErrorHdl( Link() );
    pImp->bCallbacks = FALSE;

    // ggf. alten Printer wieder einsetzen
    if ( pImp->pOldPrinter )
    {
        // Fix #59613#: niemals den aktuellen Printer synchron abschiessen !
        // Da sowieso immer bDeleteOnEndPrint gesetzt wird, wird der der Drucker im
        // dtor vom Printprogress ( dann aber asynchron !! ) zur"uckgesetzt.
/*
        pImp->pViewShell->SetPrinter( pImp->pOldPrinter, SFX_PRINTER_PRINTER );
        pImp->pOldPrinter = 0;
        pImp->pPrinter = 0;
 */
    }
    else
        // ggf. vorherigen Print-To-File-Status zuruecksetzen
        pViewShell->GetPrinter()->EnablePrintFile( pImp->bOldEnablePrintFile );

    // it is possible that after printing the document or view is deleted (because the VieShell got the ownership)
    // so first clean up
    if ( pImp->bRestoreFlag && pViewShell->GetObjectShell()->IsEnableSetModified() != pImp->bOldFlag )
        pViewShell->GetObjectShell()->EnableSetModified( TRUE );

    pViewShell->GetObjectShell()->Broadcast( SfxPrintingHint( com::sun::star::view::PrintableState_JOB_COMPLETED, NULL, NULL ) );
    if ( pImp->bDeleteOnEndPrint )
    {
        DELETEZ(pImp->pMonitor);
        delete this;
    }
    else
    {
        DBG_ASSERT( !pImp->pOldPrinter, "Unable to restore printer!" );
        pImp->bRunning = FALSE;
    }

    return 0;
}

//------------------------------------------------------------------------

void SfxPrintProgress::DeleteOnEndPrint()
{
    UnLock(); // jetzt schon, wg. Drucken im Thread
#ifndef WIN
    // da das Drucken im 'Thread' unter Windows zu undefiniert ist bleibt der
    // Print-Monitor dort stehen, auf den anderen Plattformen kann man dann
    // weiterarbeiten, also kommt das Teil weg
    DELETEZ( pImp->pMonitor );
#endif

    pImp->bDeleteOnEndPrint = TRUE;
    if ( !pImp->bRunning )
        delete this;
}

//------------------------------------------------------------------------

void SfxPrintProgress::RestoreOnEndPrint( SfxPrinter *pOldPrinter,
                                          BOOL bOldEnablePrintFile )
{
    pImp->pOldPrinter = pOldPrinter;
    pImp->bOldEnablePrintFile = bOldEnablePrintFile;
}

//------------------------------------------------------------------------

void SfxPrintProgress::RestoreOnEndPrint( SfxPrinter *pOldPrinter )
{
    RestoreOnEndPrint( pOldPrinter, FALSE );
}

//------------------------------------------------------------------------

void SfxPrintProgress::SetCancelHdl( const Link& aCancelHdl )
{
    pImp->aCancelHdl = aCancelHdl;
}

BOOL SfxPrintProgress::IsAborted() const
{
    return pImp->bAborted;
}
