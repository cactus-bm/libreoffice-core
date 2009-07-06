/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: viewprn.cxx,v $
 * $Revision: 1.36.84.1 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sfx2.hxx"

#include <com/sun/star/document/XDocumentProperties.hpp>
#include <com/sun/star/view/PrintableState.hpp>
#include "com/sun/star/view/XRenderable.hpp"

#include <svtools/itempool.hxx>
#include <vcl/msgbox.hxx>
#include <svtools/printdlg.hxx>
#include <svtools/prnsetup.hxx>
#include <svtools/flagitem.hxx>
#include <svtools/stritem.hxx>
#include <svtools/intitem.hxx>
#include <svtools/eitem.hxx>
#include <sfx2/app.hxx>
#include <svtools/useroptions.hxx>
#include <svtools/printwarningoptions.hxx>
#include <tools/datetime.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/objface.hxx>
#include <sfx2/viewsh.hxx>
#include "viewimp.hxx"
#include <sfx2/viewfrm.hxx>
#include <sfx2/prnmon.hxx>
#include "sfxresid.hxx"
#include <sfx2/request.hxx>
#include <sfx2/objsh.hxx>
#include "sfxtypes.hxx"
#include <sfx2/event.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/docfilt.hxx>

#include "toolkit/awt/vclxdevice.hxx"

#include "view.hrc"
#include "helpid.hrc"

using namespace com::sun::star;
using namespace com::sun::star::uno;

TYPEINIT1(SfxPrintingHint, SfxHint);

// -----------------------------------------------------------------------
class SfxPrinterController : public vcl::PrinterController, public SfxListener
{
    Any                                     maCompleteSelection;
    Any                                     maSelection;
    Reference< view::XRenderable >          mxRenderable;
    mutable Printer*                        mpLastPrinter;
    mutable Reference<awt::XDevice>         mxDevice;
    SfxViewShell*                           mpViewShell;
    SfxObjectShell*                         mpObjectShell;
    sal_Bool        m_bOrigStatus;
    sal_Bool        m_bNeedsChange;
    sal_Bool        m_bApi;
    util::DateTime  m_aLastPrinted;
    ::rtl::OUString m_aLastPrintedBy;

    Sequence< beans::PropertyValue > getMergedOptions() const;
    const Any& getSelectionObject() const;
public:
    SfxPrinterController( const Any& i_rComplete,
                          const Any& i_rSelection,
                          const Any& i_rViewProp,
                          const Reference< view::XRenderable >& i_xRender,
                          sal_Bool i_bApi, sal_Bool i_bDirect,
                          SfxViewShell* pView
                        );
    
    virtual ~SfxPrinterController();
    virtual void Notify( SfxBroadcaster&, const SfxHint& );
    
    virtual int  getPageCount() const;
    virtual Sequence< beans::PropertyValue > getPageParameters( int i_nPage ) const;
    virtual void printPage( int i_nPage ) const;
    virtual void jobStarted();
    virtual void jobFinished( com::sun::star::view::PrintableState );
};

SfxPrinterController::SfxPrinterController( const Any& i_rComplete,
                                            const Any& i_rSelection,
                                            const Any& i_rViewProp,
                                            const Reference< view::XRenderable >& i_xRender,
                                            sal_Bool i_bApi, sal_Bool i_bDirect,
                                            SfxViewShell* pView
                                          )
    : maCompleteSelection( i_rComplete )
    , maSelection( i_rSelection )
    , mxRenderable( i_xRender )
    , mpLastPrinter( NULL )
    , mpViewShell( pView )
    , mpObjectShell(0)
    , m_bOrigStatus( sal_False )
    , m_bNeedsChange( sal_False )
    , m_bApi(i_bApi)
{
    if ( mpViewShell )
    {
        StartListening( *mpViewShell );
        mpObjectShell = mpViewShell->GetObjectShell();
        StartListening( *mpObjectShell );
        m_bOrigStatus = mpObjectShell->IsEnableSetModified();

        // check configuration: shall update of printing information in DocInfo set the document to "modified"?
        if ( m_bOrigStatus && !SvtPrintWarningOptions().IsModifyDocumentOnPrintingAllowed() )
        {
            mpObjectShell->EnableSetModified( sal_False );
            m_bNeedsChange = sal_True;
        }

        // refresh document info
        uno::Reference<document::XDocumentProperties> xDocProps(mpObjectShell->getDocProperties());
        m_aLastPrintedBy = xDocProps->getPrintedBy();
        m_aLastPrinted = xDocProps->getPrintDate();

        xDocProps->setPrintedBy( mpObjectShell->IsUseUserData()
            ? ::rtl::OUString( SvtUserOptions().GetFullName() )
            : ::rtl::OUString() );
        ::DateTime now;

        xDocProps->setPrintDate( util::DateTime(
            now.Get100Sec(), now.GetSec(), now.GetMin(), now.GetHour(),
            now.GetDay(), now.GetMonth(), now.GetYear() ) );
    }

    // initialize extra ui options
    if( mxRenderable.is() )
    {
        Sequence< beans::PropertyValue > aRenderOptions( 1 );
        aRenderOptions[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ExtraPrintUIOptions" ) );
        Sequence< beans::PropertyValue > aRenderParms( mxRenderable->getRenderer( 0 , getSelectionObject(), aRenderOptions ) );
        int nProps = aRenderParms.getLength();
        for( int i = 0; i < nProps; i++ )
        {
            if( aRenderParms[i].Name.equalsAscii( "ExtraPrintUIOptions" ) )
            {
                Sequence< beans::PropertyValue > aUIProps;
                aRenderParms[i].Value >>= aUIProps;
                setUIOptions( aUIProps );
                break;
            }
        }
    }
    
    // set some job parameters
    setValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsApi" ) ), makeAny( i_bApi ) );
    setValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsDirect" ) ), makeAny( i_bDirect ) );
    setValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsPrinter" ) ), makeAny( sal_True ) );
    setValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "View" ) ), makeAny( i_rViewProp ) );
}

void SfxPrinterController::Notify( SfxBroadcaster& , const SfxHint& rHint )
{ 
    if ( rHint.IsA(TYPE(SfxSimpleHint)) )
    {
        if ( ((SfxSimpleHint&)rHint).GetId() == SFX_HINT_DYING )
        {
            EndListening(*mpViewShell);
            EndListening(*mpObjectShell);
            mpViewShell = 0;
            mpObjectShell = 0;
        }
    }
}

SfxPrinterController::~SfxPrinterController()
{
}

const Any& SfxPrinterController::getSelectionObject() const
{
    sal_Int32 nChoice = 0;
    const beans::PropertyValue* pVal = getValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PrintContent" ) ) );
    if( pVal )
        pVal->Value >>= nChoice;
    return nChoice > 1 ? maSelection : maCompleteSelection;
}

Sequence< beans::PropertyValue > SfxPrinterController::getMergedOptions() const
{
    boost::shared_ptr<Printer> pPrinter( getPrinter() );
    if( pPrinter.get() != mpLastPrinter )
    {
        mpLastPrinter = pPrinter.get();
        VCLXDevice* pXDevice = new VCLXDevice();
        pXDevice->SetOutputDevice( mpLastPrinter );
        mxDevice = Reference< awt::XDevice >( pXDevice );
    }
    
    Sequence< beans::PropertyValue > aRenderOptions( 1 );
    aRenderOptions[ 0 ].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "RenderDevice" ) );
    aRenderOptions[ 0 ].Value <<= mxDevice;

    aRenderOptions = getJobProperties( aRenderOptions );
    return aRenderOptions;
}

int SfxPrinterController::getPageCount() const
{
    int nPages = 0;
    boost::shared_ptr<Printer> pPrinter( getPrinter() );
    if( mxRenderable.is() && pPrinter )
    {
        Sequence< beans::PropertyValue > aJobOptions( getMergedOptions() );
        nPages = mxRenderable->getRendererCount( getSelectionObject(), aJobOptions );
    }
    return nPages;
}

Sequence< beans::PropertyValue > SfxPrinterController::getPageParameters( int i_nPage ) const
{
    boost::shared_ptr<Printer> pPrinter( getPrinter() );
    Sequence< beans::PropertyValue > aResult;
    
    if( mxRenderable.is() && pPrinter )
    {
        Sequence< beans::PropertyValue > aJobOptions( getMergedOptions() );
        aResult = mxRenderable->getRenderer( i_nPage, getSelectionObject(), aJobOptions );
    }
    return aResult;
}

void SfxPrinterController::printPage( int i_nPage ) const
{
    boost::shared_ptr<Printer> pPrinter( getPrinter() );
    if( mxRenderable.is() && pPrinter )
    {
        Sequence< beans::PropertyValue > aJobOptions( getMergedOptions() );
        mxRenderable->render( i_nPage, getSelectionObject(), aJobOptions );
    }
}

void SfxPrinterController::jobStarted()
{
    if ( mpObjectShell )
    {
        // FIXME: how to get all print options incl. AdditionalOptions easily?
        uno::Sequence < beans::PropertyValue > aOpts;
        mpObjectShell->Broadcast( SfxPrintingHint( view::PrintableState_JOB_STARTED, aOpts ) );
    }
}

void SfxPrinterController::jobFinished( com::sun::star::view::PrintableState nState )
{
    if ( mpObjectShell )
    {
        mpObjectShell->Broadcast( SfxPrintingHint( nState ) );
        switch ( nState )
        {
            case view::PrintableState_JOB_FAILED :
            {
                // "real" problem (not simply printing cancelled by user)
                String aMsg( SfxResId( STR_NOSTARTPRINTER ) );
                if ( !m_bApi )
                    ErrorBox( mpViewShell->GetWindow(), WB_OK | WB_DEF_OK,  aMsg ).Execute();
                // intentionally no break
            }
            case view::PrintableState_JOB_ABORTED :
            {
                // printing not succesful, reset DocInfo
                uno::Reference<document::XDocumentProperties> xDocProps(mpObjectShell->getDocProperties());
                xDocProps->setPrintedBy(m_aLastPrintedBy);
                xDocProps->setPrintDate(m_aLastPrinted);
                break;
            }

            case view::PrintableState_JOB_SPOOLED :
            case view::PrintableState_JOB_COMPLETED :
            {
                SfxBindings& rBind = mpViewShell->GetViewFrame()->GetBindings();
                rBind.Invalidate( SID_PRINTDOC );
                rBind.Invalidate( SID_PRINTDOCDIRECT );
                rBind.Invalidate( SID_SETUPPRINTER );
                break;
            }

            default:
                break;
        }

        if ( m_bNeedsChange )
            mpObjectShell->EnableSetModified( m_bOrigStatus );
    }
}

// -----------------------------------------------------------------------

void DisableRanges( PrintDialog& rDlg, SfxPrinter* pPrinter )

/*      [Beschreibung]

    Mit dieser Funktion werden die nicht verf"ugbaren Ranges
    vom Printer zum PrintDialog geforwarded.
*/

{
    if ( !pPrinter )
        return;

    if ( !pPrinter->IsRangeEnabled( PRINTDIALOG_ALL ) )
        rDlg.DisableRange( PRINTDIALOG_ALL );
    if ( !pPrinter->IsRangeEnabled( PRINTDIALOG_SELECTION ) )
        rDlg.DisableRange( PRINTDIALOG_SELECTION );
    if ( !pPrinter->IsRangeEnabled( PRINTDIALOG_FROMTO ) )
        rDlg.DisableRange( PRINTDIALOG_FROMTO );
    if ( !pPrinter->IsRangeEnabled( PRINTDIALOG_RANGE ) )
        rDlg.DisableRange( PRINTDIALOG_RANGE );
}

//====================================================================

class SfxDialogExecutor_Impl

/*  [Beschreibung]

    Eine Instanz dieser Klasse wird f"ur die Laufzeit des Printer-Dialogs
    erzeugt, um im dessen Click-Handler f"ur die Zus"atze den per
    virtueller Methode von der abgeleiteten SfxViewShell erzeugten
    Print-Options-Dialog zu erzeugen und die dort eingestellten Optionen
    als SfxItemSet zu zwischenzuspeichern.
*/

{
private:
    SfxViewShell*           _pViewSh;
    PrintDialog*            _pPrintParent;
    PrinterSetupDialog*     _pSetupParent;
    SfxItemSet*             _pOptions;
    sal_Bool                _bModified;
    sal_Bool                _bHelpDisabled;

    DECL_LINK( Execute, void * );

public:
            SfxDialogExecutor_Impl( SfxViewShell* pViewSh, PrintDialog* pParent );
            SfxDialogExecutor_Impl( SfxViewShell* pViewSh, PrinterSetupDialog* pParent );
            ~SfxDialogExecutor_Impl() { delete _pOptions; }

    Link                GetLink() const { return LINK( this, SfxDialogExecutor_Impl, Execute); }
    const SfxItemSet*   GetOptions() const { return _pOptions; }
    void                DisableHelp() { _bHelpDisabled = sal_True; }
};

//--------------------------------------------------------------------

SfxDialogExecutor_Impl::SfxDialogExecutor_Impl( SfxViewShell* pViewSh, PrintDialog* pParent ) :

    _pViewSh        ( pViewSh ),
    _pPrintParent   ( pParent ),
    _pSetupParent   ( NULL ),
    _pOptions       ( NULL ),
    _bModified      ( sal_False ),
    _bHelpDisabled  ( sal_False )

{
}

SfxDialogExecutor_Impl::SfxDialogExecutor_Impl( SfxViewShell* pViewSh, PrinterSetupDialog* pParent ) :

    _pViewSh        ( pViewSh ),
    _pPrintParent   ( NULL ),
    _pSetupParent   ( pParent ),
    _pOptions       ( NULL ),
    _bModified      ( sal_False ),
    _bHelpDisabled  ( sal_False )

{
}

//--------------------------------------------------------------------

IMPL_LINK( SfxDialogExecutor_Impl, Execute, void *, EMPTYARG )
{
    // Options lokal merken
    if ( !_pOptions )
    {
        DBG_ASSERT( _pPrintParent || _pSetupParent, "no dialog parent" );
        if( _pPrintParent )
            _pOptions = ( (SfxPrinter*)_pPrintParent->GetPrinter() )->GetOptions().Clone();
        else if( _pSetupParent )
            _pOptions = ( (SfxPrinter*)_pSetupParent->GetPrinter() )->GetOptions().Clone();
    }

    if ( _pOptions && _pPrintParent && _pPrintParent->IsSheetRangeAvailable() )
    {
        SfxItemState eState = _pOptions->GetItemState( SID_PRINT_SELECTEDSHEET );
        if ( eState != SFX_ITEM_UNKNOWN )
        {
            PrintSheetRange eRange = _pPrintParent->GetCheckedSheetRange();
            BOOL bValue = ( PRINTSHEETS_ALL != eRange );
            _pOptions->Put( SfxBoolItem( SID_PRINT_SELECTEDSHEET, bValue ) );
        }
    }

    // Dialog ausf"uhren
    SfxPrintOptionsDialog* pDlg = new SfxPrintOptionsDialog( _pPrintParent ? static_cast<Window*>(_pPrintParent)
                                                                           : static_cast<Window*>(_pSetupParent),
                                                             _pViewSh, _pOptions );
    if ( _bHelpDisabled )
        pDlg->DisableHelp();
    if ( pDlg->Execute() == RET_OK )
    {
        delete _pOptions;
        _pOptions = pDlg->GetOptions().Clone();

        if ( _pOptions && _pPrintParent && _pPrintParent->IsSheetRangeAvailable() )
        {
            const SfxPoolItem* pItem;
            if ( SFX_ITEM_SET == _pOptions->GetItemState( SID_PRINT_SELECTEDSHEET, FALSE , &pItem ) )
            {
                _pPrintParent->CheckSheetRange( ( (const SfxBoolItem*)pItem )->GetValue()
                    ? PRINTSHEETS_SELECTED_SHEETS : PRINTSHEETS_ALL );
            }
        }
    }
    delete pDlg;

    return 0;
}

//-------------------------------------------------------------------------

BOOL UseStandardPrinter_Impl( Window* /*pParent*/, SfxPrinter* pDocPrinter )
{
    // Optionen abfragen, ob gewarnt werden soll (Doc uebersteuert App)
    BOOL bWarn = FALSE;
    const SfxItemSet *pDocOptions = &pDocPrinter->GetOptions();
    if ( pDocOptions )
    {
        USHORT nWhich = pDocOptions->GetPool()->GetWhich(SID_PRINTER_NOTFOUND_WARN);
        const SfxBoolItem* pBoolItem = NULL;
        pDocPrinter->GetOptions().GetItemState( nWhich, FALSE, (const SfxPoolItem**) &pBoolItem );
        if ( pBoolItem )
            bWarn = pBoolItem->GetValue();
    }
/*
    // ggf. den User fragen
    if ( bWarn )
    {
        // Geht nicht mehr ohne OrigJobSetup!
        String aTmp( SfxResId( STR_PRINTER_NOTAVAIL ) );
        QueryBox aBox( pParent, WB_OK_CANCEL | WB_DEF_OK, aTmp );
        return RET_OK == aBox.Execute();
    }
*/
    // nicht gewarnt => einfach so den StandardDrucker nehmen
    return TRUE;
}
//-------------------------------------------------------------------------

SfxPrinter* SfxViewShell::SetPrinter_Impl( SfxPrinter *pNewPrinter )

/*  Interne Methode zum Setzen der Unterschiede von 'pNewPrinter' zum
    aktuellen Printer. pNewPrinter wird entweder "ubernommen oder gel"oscht.
*/

{
    // aktuellen Printer holen
    SfxPrinter *pDocPrinter = GetPrinter();

    // Printer-Options auswerten
    FASTBOOL bOriToDoc = FALSE;
    FASTBOOL bSizeToDoc = FALSE;
    if ( &pDocPrinter->GetOptions() )
    {
        USHORT nWhich = GetPool().GetWhich(SID_PRINTER_CHANGESTODOC);
        const SfxFlagItem *pFlagItem = 0;
        pDocPrinter->GetOptions().GetItemState( nWhich, FALSE, (const SfxPoolItem**) &pFlagItem );
        bOriToDoc = pFlagItem ? (pFlagItem->GetValue() & SFX_PRINTER_CHG_ORIENTATION) : FALSE;
        bSizeToDoc = pFlagItem ? (pFlagItem->GetValue() & SFX_PRINTER_CHG_SIZE) : FALSE;
    }

    // vorheriges Format und Size feststellen
    Orientation eOldOri = pDocPrinter->GetOrientation();
    Size aOldPgSz = pDocPrinter->GetPaperSizePixel();

    // neues Format und Size feststellen
    Orientation eNewOri = pNewPrinter->GetOrientation();
    Size aNewPgSz = pNewPrinter->GetPaperSizePixel();

    // "Anderungen am Seitenformat feststellen
    BOOL bOriChg = (eOldOri != eNewOri) && bOriToDoc;
    BOOL bPgSzChg = ( aOldPgSz.Height() !=
            ( bOriChg ? aNewPgSz.Width() : aNewPgSz.Height() ) ||
            aOldPgSz.Width() !=
            ( bOriChg ? aNewPgSz.Height() : aNewPgSz.Width() ) ) &&
            bSizeToDoc;

    // Message und Flags f"ur Seitenformat-"Anderung zusammenstellen
    String aMsg;
    USHORT nNewOpt=0;
    if( bOriChg && bPgSzChg )
    {
        aMsg = String(SfxResId(STR_PRINT_NEWORISIZE));
        nNewOpt = SFX_PRINTER_CHG_ORIENTATION | SFX_PRINTER_CHG_SIZE;
    }
    else if (bOriChg )
    {
        aMsg = String(SfxResId(STR_PRINT_NEWORI));
        nNewOpt = SFX_PRINTER_CHG_ORIENTATION;
    }
    else if (bPgSzChg)
    {
        aMsg = String(SfxResId(STR_PRINT_NEWSIZE));
        nNewOpt = SFX_PRINTER_CHG_SIZE;
    }

    // in dieser Variable sammeln, was sich so ge"aendert hat
    USHORT nChangedFlags = 0;

    // ggf. Nachfrage, ob Seitenformat vom Drucker "ubernommen werden soll
    if ( ( bOriChg  || bPgSzChg ) &&
        RET_YES == QueryBox(0, WB_YES_NO | WB_DEF_OK, aMsg).Execute() )
    // Flags mit "Anderungen f"ur <SetPrinter(SfxPrinter*)> mitpflegen
    nChangedFlags |= nNewOpt;

    // fuer den MAC sein "temporary of class String" im naechsten if()
    String aTempPrtName = pNewPrinter->GetName();
    String aDocPrtName = pDocPrinter->GetName();

    // Wurde der Drucker gewechselt oder von Default auf Specific
    // oder umgekehrt geaendert?
    if ( (aTempPrtName != aDocPrtName) || (pDocPrinter->IsDefPrinter() != pNewPrinter->IsDefPrinter()) )
    {
        // neuen Printer "ubernehmen
        // pNewPrinter->SetOrigJobSetup( pNewPrinter->GetJobSetup() );
        nChangedFlags |= SFX_PRINTER_PRINTER|SFX_PRINTER_JOBSETUP;
        pDocPrinter = pNewPrinter;
    }
    else
    {
        // Extra-Optionen vergleichen
        if ( ! (pNewPrinter->GetOptions() == pDocPrinter->GetOptions()) )
        {
            // Options haben sich geaendert
            pDocPrinter->SetOptions( pNewPrinter->GetOptions() );
            nChangedFlags |= SFX_PRINTER_OPTIONS;
        }

        // JobSetups vergleichen
        JobSetup aNewJobSetup = pNewPrinter->GetJobSetup();
        JobSetup aOldJobSetup = pDocPrinter->GetJobSetup();
        if ( aNewJobSetup != aOldJobSetup )
        {
            // JobSetup hat sich geaendert (=> App mu\s neu formatieren)
            // pDocPrinter->SetOrigJobSetup( aNewJobSetup );
            nChangedFlags |= SFX_PRINTER_JOBSETUP;
        }

        // alten, ver"anderten Printer behalten
        pDocPrinter->SetPrinterProps( pNewPrinter );
        delete pNewPrinter;
    }

    if ( 0 != nChangedFlags )
        // SetPrinter will delete the old printer if it changes
        SetPrinter( pDocPrinter, nChangedFlags );
    return pDocPrinter;
}

//-------------------------------------------------------------------------
// Unter WIN32 tritt leider das Problem auf, dass nichts gedruckt
// wird, wenn SID_PRINTDOCDIRECT auflaueft; bisher bekannte,
// einzige Abhilfe ist in diesem Fall das Abschalten der Optimierungen
// (KA 17.12.95)
#ifdef _MSC_VER
#pragma optimize ( "", off )
#endif

void SfxViewShell::ExecPrint( const uno::Sequence < beans::PropertyValue >& rProps, sal_Bool bIsAPI, sal_Bool bIsDirect )
{
    // get the current selection; our controller should know it
    Reference< frame::XController > xController( GetController() );
    Reference< view::XSelectionSupplier > xSupplier( xController, UNO_QUERY );
    
    Any aSelection;
    if( xSupplier.is() )
        aSelection = xSupplier->getSelection();
    else
        aSelection <<= GetObjectShell()->GetModel();
    Any aComplete( makeAny( GetObjectShell()->GetModel() ) );
    Any aViewProp( makeAny( xController ) );
    
    boost::shared_ptr<vcl::PrinterController> pController( new SfxPrinterController( aComplete,
                                                                               aSelection,
                                                                               aViewProp,
                                                                               GetRenderable(),
                                                                               bIsAPI,
                                                                               bIsDirect,
                                                                               this
                                                                               ) );
    for (sal_Int32 nProp=0; nProp<rProps.getLength(); nProp++)
        pController->setValue( rProps[nProp].Name, rProps[nProp].Value );

    SfxObjectShell *pObjShell = GetObjectShell();
    pController->setValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "JobName" ) ),
                        makeAny( rtl::OUString( pObjShell->GetTitle(0) ) ) );

    // FIXME: job setup
    JobSetup aJobSetup = GetJobSetup();
    if( bIsDirect )
        aJobSetup.SetValue( String( RTL_CONSTASCII_USTRINGPARAM( "IsQuickJob" ) ),
                            String( RTL_CONSTASCII_USTRINGPARAM( "true" ) ) );

    Printer::PrintJob( pController, aJobSetup );
}

void SfxViewShell::ExecPrint_Impl( SfxRequest &rReq )
{
    // USHORT                  nCopies=1;
    USHORT                  nDialogRet = RET_CANCEL;
    // BOOL                    bCollate=FALSE;
    SfxPrinter*             pPrinter = 0;
    PrintDialog*            pPrintDlg = 0;
    SfxDialogExecutor_Impl* pExecutor = 0;
    bool                    bSilent = false;
    BOOL bIsAPI = rReq.GetArgs() && rReq.GetArgs()->Count();
    if ( bIsAPI )
    {
        SFX_REQUEST_ARG(rReq, pSilentItem, SfxBoolItem, SID_SILENT, FALSE);
        bSilent = pSilentItem && pSilentItem->GetValue();
    }

    //FIXME: how to transport "bPrintOnHelp"?

    // no help button in dialogs if called from the help window
    // (pressing help button would exchange the current page inside the help document that is going to be printed!)
    String aHelpFilterName( DEFINE_CONST_UNICODE("writer_web_HTML_help") );
    SfxMedium* pMedium = GetViewFrame()->GetObjectShell()->GetMedium();
    const SfxFilter* pFilter = pMedium ? pMedium->GetFilter() : NULL;
    sal_Bool bPrintOnHelp = ( pFilter && pFilter->GetFilterName() == aHelpFilterName );

    const USHORT nId = rReq.GetSlot();
    switch( nId )
    {
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case SID_PRINTDOC:
        case SID_PRINTDOCDIRECT:
        {
            SfxObjectShell* pDoc = GetObjectShell();
            bool bDetectHidden = ( !bSilent && pDoc );
            if ( bDetectHidden && pDoc->QueryHiddenInformation( WhenPrinting, NULL ) != RET_YES )
                break;

            uno::Sequence < beans::PropertyValue > aProps;
            if ( bIsAPI )
            {
                // supported properties:
                // String PrinterName 
                // String FileName
                // Int16 From 
                // Int16 To 
                // In16 Copies
                // String RangeText
                // bool Selection 
                // bool Asynchron
                // bool Collate 
                // bool Silent
                TransformItems( nId, *rReq.GetArgs(), aProps, GetInterface()->GetSlot(nId) );
                for ( sal_Int32 nProp=0; nProp<aProps.getLength(); nProp++ )
                {
                    if ( aProps[nProp].Name.equalsAscii("Copies") )
                        aProps[nProp]. Name = rtl::OUString::createFromAscii("CopyCount");
                    else if ( aProps[nProp].Name.equalsAscii("RangeText") )
                        aProps[nProp]. Name = rtl::OUString::createFromAscii("Pages");
                    if ( aProps[nProp].Name.equalsAscii("Asynchron") )
                    {
                        aProps[nProp]. Name = rtl::OUString::createFromAscii("Wait");
                        sal_Bool bAsynchron;
                        aProps[nProp].Value >>= bAsynchron;
                        aProps[nProp].Value <<= (sal_Bool) (!bAsynchron);
                    }
                    if ( aProps[nProp].Name.equalsAscii("Silent") )
                    {
                        aProps[nProp]. Name = rtl::OUString::createFromAscii("MonitorVisible");
                        sal_Bool bSilent;
                        aProps[nProp].Value >>= bSilent;
                        aProps[nProp].Value <<= (sal_Bool) (!bSilent);
                    }
                }
            }

            ExecPrint( aProps, bIsAPI, (nId == SID_PRINTDOCDIRECT) );

            // FIXME: Recording
            rReq.Done();
            break;
        }

        case SID_SETUPPRINTER :
        case SID_PRINTER_NAME : // only for recorded macros
        {
            // get printer and printer settings from the document
            SfxPrinter *pDocPrinter = GetPrinter(TRUE);

            // look for printer in parameters
            SFX_REQUEST_ARG( rReq, pPrinterItem, SfxStringItem, SID_PRINTER_NAME, FALSE );
            if ( pPrinterItem )
            {
                // use PrinterName parameter to create a printer
                pPrinter = new SfxPrinter( pDocPrinter->GetOptions().Clone(), ((const SfxStringItem*) pPrinterItem)->GetValue() );

                // if printer is unknown, it can't be used - now printer from document will be used
                if ( !pPrinter->IsOriginal() )
                    DELETEZ(pPrinter);
            }

            if ( SID_PRINTER_NAME == nId )
            {
                // just set a recorded printer name
                if ( pPrinter )
                    SetPrinter( pPrinter, SFX_PRINTER_PRINTER  );
                break;
            }

            // no PrinterName parameter in ItemSet or the PrinterName points to an unknown printer
            if ( !pPrinter )
                // use default printer from document
                pPrinter = pDocPrinter;

            if( !pPrinter || !pPrinter->IsValid() )
            {
                // no valid printer either in ItemSet or at the document
                if ( bSilent )
                {
                    rReq.SetReturnValue(SfxBoolItem(0,FALSE));
                    break;
                }
                else
                    ErrorBox( NULL, WB_OK | WB_DEF_OK, String( SfxResId( STR_NODEFPRINTER ) ) ).Execute();
            }

            if ( !pPrinter->IsOriginal() && rReq.GetArgs() && !UseStandardPrinter_Impl( NULL, pPrinter ) )
            {
                // printer is not available, but standard printer should not be used
                rReq.SetReturnValue(SfxBoolItem(0,FALSE));
                break;
            }

            // FIXME: printer isn't used for printing anymore!
            if( pPrinter->IsPrinting() )
            {
                // if printer is busy, abort printing
                if ( !bSilent )
                    InfoBox( NULL, String( SfxResId( STR_ERROR_PRINTER_BUSY ) ) ).Execute();
                rReq.SetReturnValue(SfxBoolItem(0,FALSE));
                break;
            }

            // if no arguments are given, retrieve them from a dialog
            if ( !bIsAPI )
            {
                // PrinterDialog needs a temporary printer
                SfxPrinter* pDlgPrinter = pPrinter->Clone();
                nDialogRet = 0;
                
                // execute PrinterSetupDialog
                PrinterSetupDialog* pPrintSetupDlg = new PrinterSetupDialog( GetWindow() );

                if ( pImp->bHasPrintOptions )
                {
                    // additional controls for dialog
                    pExecutor = new SfxDialogExecutor_Impl( this, pPrintSetupDlg );
                    if ( bPrintOnHelp )
                        pExecutor->DisableHelp();
                    pPrintSetupDlg->SetOptionsHdl( pExecutor->GetLink() );
                }

                pPrintSetupDlg->SetPrinter( pDlgPrinter );
                nDialogRet = pPrintSetupDlg->Execute();

                if ( pExecutor && pExecutor->GetOptions() )
                {
                    if ( nDialogRet == RET_OK )
                        // remark: have to be recorded if possible!
                        pDlgPrinter->SetOptions( *pExecutor->GetOptions() );
                    else
                    {
                        pPrinter->SetOptions( *pExecutor->GetOptions() );
                        SetPrinter( pPrinter, SFX_PRINTER_OPTIONS );
                    }
                }

                DELETEZ( pPrintSetupDlg );

                // no recording of PrinterSetup except printer name (is printer dependent)
                rReq.Ignore();

                if ( nDialogRet == RET_OK )
                {
                    if ( pPrinter->GetName() != pDlgPrinter->GetName() )
                    {
                        // user has changed the printer -> macro recording
                        SfxRequest aReq( GetViewFrame(), SID_PRINTER_NAME );
                        aReq.AppendItem( SfxStringItem( SID_PRINTER_NAME, pDlgPrinter->GetName() ) );
                        aReq.Done();
                    }

                    // take the changes made in the dialog
                    pPrinter = SetPrinter_Impl( pDlgPrinter );

                    // forget new printer, it was taken over (as pPrinter) or deleted
                    pDlgPrinter = NULL;

                    /* Now lets reset the Dialog printer, since its freed */
                    if (pPrintDlg)
                        pPrintDlg->SetPrinter (pPrinter);
                }
                else
                {
                    // PrinterDialog is used to transfer information on printing,
                    // so it will only be deleted here if dialog was cancelled
                    DELETEZ( pDlgPrinter );
                    DELETEZ( pPrintDlg );
                    rReq.Ignore();
                    if ( SID_PRINTDOC == nId )
                        rReq.SetReturnValue(SfxBoolItem(0,FALSE));
                }
            }
        }
       
        break;       
    }
}

// Optimierungen wieder einschalten
#ifdef _MSC_VER
#pragma optimize ( "", on )
#endif

//--------------------------------------------------------------------

PrintDialog* SfxViewShell::CreatePrintDialog( Window* /*pParent*/ )

/*  [Beschreibung]

    Diese Methode kann "uberladen werden, um einen speziellen PrintDialog
    zu erzeugen. Dies ist z.B. notwendig wenn spezielle <StarView> Features
    wie drucken von Seitenbereichen.
*/

{
    #if 0
    PrintDialog *pDlg = new PrintDialog( pParent, false );
    pDlg->SetFirstPage( 1 );
    pDlg->SetLastPage( 9999 );
    pDlg->EnableCollate();
    return pDlg;
    #else
    return NULL;
    #endif
}

//--------------------------------------------------------------------

void SfxViewShell::PreparePrint( PrintDialog * )
{
}

//--------------------------------------------------------------------


ErrCode SfxViewShell::DoPrint( SfxPrinter* /*pPrinter*/,
                               PrintDialog* /*pPrintDlg*/,
                               BOOL /*bSilent*/, BOOL /*bIsAPI*/ )
{
    #if 0
    // Printer-Dialogbox waehrend des Ausdrucks mu\s schon vor
    // StartJob erzeugt werden, da SV bei einem Quit-Event h"angt
    SfxPrintProgress *pProgress = new SfxPrintProgress( this, !bSilent );
    SfxPrinter *pDocPrinter = GetPrinter(TRUE);
    if ( !pPrinter )
        pPrinter = pDocPrinter;
    else if ( pDocPrinter != pPrinter )
    {
        pProgress->RestoreOnEndPrint( pDocPrinter->Clone() );
        SetPrinter( pPrinter, SFX_PRINTER_PRINTER );
    }
    pProgress->SetWaitMode(FALSE);

    // Drucker starten
    PreparePrint( pPrintDlg );
    SfxObjectShell *pObjShell = GetViewFrame()->GetObjectShell();
    if ( pPrinter->StartJob(pObjShell->GetTitle(0)) )
    {
        // Drucken
        Print( *pProgress, bIsAPI, pPrintDlg );
        pProgress->Stop();
        pProgress->DeleteOnEndPrint();
        pPrinter->EndJob();
    }
    else
    {
        // Printer konnte nicht gestartet werden
        delete pProgress;
    }

    return pPrinter->GetError();
    #else
    DBG_ERROR( "DoPrint called, dead code !" );
    return ERRCODE_IO_NOTSUPPORTED;
    #endif
}

//--------------------------------------------------------------------

BOOL SfxViewShell::IsPrinterLocked() const
{
    return pImp->nPrinterLocks > 0;
}

//--------------------------------------------------------------------

void SfxViewShell::LockPrinter( BOOL bLock)
{
    BOOL bChanged = FALSE;
    if ( bLock )
        bChanged = 1 == ++pImp->nPrinterLocks;
    else
        bChanged = 0 == --pImp->nPrinterLocks;

    if ( bChanged )
    {
        Invalidate( SID_PRINTDOC );
        Invalidate( SID_PRINTDOCDIRECT );
        Invalidate( SID_SETUPPRINTER );
    }
}

//--------------------------------------------------------------------

USHORT SfxViewShell::Print( SfxProgress& /*rProgress*/, BOOL /*bIsAPI*/, PrintDialog* /*pDlg*/ )
{
    return 0;
}

//--------------------------------------------------------------------

SfxPrinter* SfxViewShell::GetPrinter( BOOL /*bCreate*/ )
{
    return 0;
}

//--------------------------------------------------------------------

USHORT SfxViewShell::SetPrinter( SfxPrinter* /*pNewPrinter*/, USHORT /*nDiffFlags*/, bool )
{
    return 0;
}

//--------------------------------------------------------------------

SfxTabPage* SfxViewShell::CreatePrintOptionsPage
(
    Window*             /*pParent*/,
    const SfxItemSet&   /*rOptions*/
)
{
    return 0;
}

JobSetup SfxViewShell::GetJobSetup() const
{
    return JobSetup();
}