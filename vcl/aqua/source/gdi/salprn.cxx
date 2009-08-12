/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: salprn.cxx,v $
 * $Revision: 1.16.56.2 $
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
#include "precompiled_vcl.hxx"

#include "salinst.h"
#include "salprn.h"
#include "aquaprintview.h"
#include "salgdi.h"
#include "saldata.hxx"
#include "vcl/jobset.h"
#include "vcl/salptype.hxx"
#include "vcl/print.hxx"
#include "vcl/unohelp.hxx"

#include <boost/bind.hpp>

#include "com/sun/star/lang/XMultiServiceFactory.hpp"
#include "com/sun/star/container/XNameAccess.hpp"
#include "com/sun/star/beans/PropertyValue.hpp"
#include "com/sun/star/awt/Size.hpp"

#include <algorithm>

using namespace rtl;
using namespace vcl;
using namespace com::sun::star;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;

// =======================================================================

AquaSalInfoPrinter::AquaSalInfoPrinter( const SalPrinterQueueInfo& i_rQueue ) :
    mpGraphics( 0 ),
    mbGraphics( false ),
    mbJob( false ),
    mpPrinter( nil ),
    mpPrintInfo( nil ),
    mePageOrientation( ORIENTATION_PORTRAIT ),
    mnStartPageOffsetX( 0 ),
    mnStartPageOffsetY( 0 ),
    mnCurPageRangeStart( 0 ),
    mnCurPageRangeCount( 0 )
{
    NSString* pStr = CreateNSString( i_rQueue.maPrinterName );
    mpPrinter = [NSPrinter printerWithName: pStr];
    [pStr release];

    NSPrintInfo* pShared = [NSPrintInfo sharedPrintInfo];
    if( pShared )
    {
        mpPrintInfo = [pShared copy];
        [mpPrintInfo setPrinter: mpPrinter];
        mePageOrientation = ([mpPrintInfo orientation] == NSLandscapeOrientation) ? ORIENTATION_LANDSCAPE : ORIENTATION_PORTRAIT;
        [mpPrintInfo setOrientation: NSPortraitOrientation];
    }

    mpGraphics = new AquaSalGraphics();

    const int nWidth = 100, nHeight = 100;
    maContextMemory.reset( reinterpret_cast<sal_uInt8*>( rtl_allocateMemory( nWidth * 4 * nHeight ) ),
                           boost::bind( rtl_freeMemory, _1 ) );

    if( maContextMemory )
    {
        mrContext = CGBitmapContextCreate( maContextMemory.get(), nWidth, nHeight, 8, nWidth * 4, GetSalData()->mxRGBSpace, kCGImageAlphaNoneSkipFirst );
        if( mrContext )
            SetupPrinterGraphics( mrContext );
    }
}

// -----------------------------------------------------------------------

AquaSalInfoPrinter::~AquaSalInfoPrinter()
{
    delete mpGraphics;
    if( mpPrintInfo )
        [mpPrintInfo release];
    #if 0
    // FIXME: verify that NSPrintInfo releases the printer
    // else we have a leak here
    if( mpPrinter )
        [mpPrinter release];
    #endif
    if( mrContext )
        CFRelease( mrContext );
}

// -----------------------------------------------------------------------

void AquaSalInfoPrinter::SetupPrinterGraphics( CGContextRef i_rContext ) const
{
    if( mpGraphics )
    {
        if( mpPrintInfo )
        {
            // FIXME: get printer resolution
            long nDPIX = 720, nDPIY = 720;
            NSSize aPaperSize = [mpPrintInfo paperSize];

            NSRect aImageRect = [mpPrintInfo imageablePageBounds];
            if( mePageOrientation == ORIENTATION_PORTRAIT )
            {
                // move mirrored CTM back into paper
                double dX = 0, dY = aPaperSize.height;
                // move CTM to reflect imageable area
                dX += aImageRect.origin.x;
                dY -= aPaperSize.height - aImageRect.size.height - aImageRect.origin.y;
                CGContextTranslateCTM( i_rContext, dX + mnStartPageOffsetX, dY - mnStartPageOffsetY );
                // scale to be top/down and reflect our "virtual" DPI
                CGContextScaleCTM( i_rContext, 0.1, -0.1 );
            }
            else
            {
                // move CTM to reflect imageable area
                double dX = aImageRect.origin.x, dY = aPaperSize.height - aImageRect.size.height - aImageRect.origin.y;
                CGContextTranslateCTM( i_rContext, -dX, -dY );
                // turn by 90 degree
                CGContextRotateCTM( i_rContext, M_PI/2 );
                // move turned CTM back into paper
                dX = aPaperSize.height;
                dY = -aPaperSize.width;
                CGContextTranslateCTM( i_rContext, dX + mnStartPageOffsetY, dY - mnStartPageOffsetX );
                // scale to be top/down and reflect our "virtual" DPI
                CGContextScaleCTM( i_rContext, -0.1, 0.1 );
            }
            mpGraphics->SetPrinterGraphics( i_rContext, nDPIX, nDPIY, 1.0 );
        }
        else
            DBG_ERROR( "no print info in SetupPrinterGraphics" );
    }
}

// -----------------------------------------------------------------------

SalGraphics* AquaSalInfoPrinter::GetGraphics()
{
    SalGraphics* pGraphics = mbGraphics ? NULL : mpGraphics;
    mbGraphics = true;
    return pGraphics;
}

// -----------------------------------------------------------------------

void AquaSalInfoPrinter::ReleaseGraphics( SalGraphics* )
{
    mbGraphics = false;
}

// -----------------------------------------------------------------------

BOOL AquaSalInfoPrinter::Setup( SalFrame* i_pFrame, ImplJobSetup* i_pSetupData )
{
    return FALSE;
}

// -----------------------------------------------------------------------

static struct PaperSizeEntry
{
    double fWidth;
    double fHeight;
    Paper        nPaper;
} aPaperSizes[] =
{
    { 842, 1191, PAPER_A3 },
    { 595,  842, PAPER_A4 },
    { 420,  595, PAPER_A5 },
    { 612,  792, PAPER_LETTER },
    { 612, 1008, PAPER_LEGAL },
    { 728, 1032, PAPER_B4_JIS },
    { 516,  729, PAPER_B5_JIS },
    { 792, 1224, PAPER_TABLOID }
};

static bool getPaperSize( double& o_fWidth, double& o_fHeight, const Paper i_ePaper )
{
    for(unsigned int i = 0; i < sizeof(aPaperSizes)/sizeof(aPaperSizes[0]); i++ )
    {
        if( aPaperSizes[i].nPaper == i_ePaper )
        {
            o_fWidth = aPaperSizes[i].fWidth;
            o_fHeight = aPaperSizes[i].fHeight;
            return true;
        }
    }
    return false;
}

static Paper recognizePaper( double i_fWidth, double i_fHeight )
{
    Paper aPaper = PAPER_USER;
    sal_uInt64 nPaperDesc = 1000000*sal_uInt64(i_fWidth) + sal_uInt64(i_fHeight);
    switch( nPaperDesc )
    {
    case 842001191: aPaper = PAPER_A3;      break;
    case 595000842: aPaper = PAPER_A4;      break;
    case 420000595: aPaper = PAPER_A5;      break;
    case 612000792: aPaper = PAPER_LETTER;  break;
    case 728001032: aPaper = PAPER_B4_JIS;  break;
    case 516000729: aPaper = PAPER_B5_JIS;  break;
    case 612001008: aPaper = PAPER_LEGAL;   break;
    case 792001224: aPaper = PAPER_TABLOID; break;
    default:
        aPaper = PAPER_USER;
        break;
    }

    if( aPaper == PAPER_USER )
    {
        // search with fuzz factor
        for( unsigned int i = 0; i < sizeof(aPaperSizes)/sizeof(aPaperSizes[0]); i++ )
        {
            double w = (i_fWidth > aPaperSizes[i].fWidth) ? i_fWidth - aPaperSizes[i].fWidth : aPaperSizes[i].fWidth - i_fWidth;
            double h = (i_fHeight > aPaperSizes[i].fHeight) ? i_fHeight - aPaperSizes[i].fHeight : aPaperSizes[i].fHeight - i_fHeight;
            if( w < 3 && h < 3 )
            {
                aPaper = aPaperSizes[i].nPaper;
                break;
            }
        }
    }

    return aPaper;
}

BOOL AquaSalInfoPrinter::SetPrinterData( ImplJobSetup* io_pSetupData )
{
    // FIXME: implement driver data
    if( io_pSetupData && io_pSetupData->mpDriverData )
        return SetData( ~0, io_pSetupData );


    BOOL bSuccess = TRUE;

    // set system type
    io_pSetupData->mnSystem = JOBSETUP_SYSTEM_MAC;

    // get paper format
    if( mpPrintInfo )
    {
        NSSize aPaperSize = [mpPrintInfo paperSize];
        double width = aPaperSize.width, height = aPaperSize.height;
        // set paper
        io_pSetupData->mePaperFormat = recognizePaper( width, height );
        if( io_pSetupData->mePaperFormat == PAPER_USER )
        {
            io_pSetupData->mnPaperWidth = PtTo10Mu( width );
            io_pSetupData->mnPaperHeight = PtTo10Mu( height );
        }
        else
        {
            io_pSetupData->mnPaperWidth = 0;
            io_pSetupData->mnPaperHeight = 0;
        }

        // set orientation
        io_pSetupData->meOrientation = mePageOrientation;

        io_pSetupData->mnPaperBin = 0;
        io_pSetupData->mpDriverData = reinterpret_cast<BYTE*>(rtl_allocateMemory( 4 ));
        io_pSetupData->mnDriverDataLen = 4;
    }
    else
        bSuccess = FALSE;


    return bSuccess;
}

// -----------------------------------------------------------------------

void AquaSalInfoPrinter::setPaperSize( long i_nWidth, long i_nHeight, Orientation i_eSetOrientation )
{

    Orientation ePaperOrientation = ORIENTATION_PORTRAIT;
    const PaperInfo* pPaper = matchPaper( i_nWidth, i_nHeight, ePaperOrientation );

    if( pPaper )
    {
        NSString* pPaperName = [CreateNSString( rtl::OStringToOUString(PaperInfo::toPSName(pPaper->getPaper()), RTL_TEXTENCODING_ASCII_US) ) autorelease];
        [mpPrintInfo setPaperName: pPaperName];
    }
    else if( i_nWidth > 0 && i_nHeight > 0 )
    {
        NSSize aPaperSize = { TenMuToPt(i_nWidth), TenMuToPt(i_nHeight) };
        [mpPrintInfo setPaperSize: aPaperSize];
    }
    // this seems counterintuitive
    mePageOrientation = i_eSetOrientation;
}

// -----------------------------------------------------------------------

BOOL AquaSalInfoPrinter::SetData( ULONG i_nFlags, ImplJobSetup* io_pSetupData )
{
    if( ! io_pSetupData || io_pSetupData->mnSystem != JOBSETUP_SYSTEM_MAC )
        return FALSE;


    if( mpPrintInfo )
    {
        if( (i_nFlags & SAL_JOBSET_ORIENTATION) != 0 )
            mePageOrientation = io_pSetupData->meOrientation;

        if( (i_nFlags & SAL_JOBSET_PAPERSIZE) !=  0)
        {
            // set paper format
            long width = 0, height = 0;
            if( io_pSetupData->mePaperFormat == PAPER_USER )
            {
                width = io_pSetupData->mnPaperWidth;
                height = io_pSetupData->mnPaperHeight;
            }
            else
            {
                double w = 595,  h = 842;
                getPaperSize( w, h, io_pSetupData->mePaperFormat );
                width = static_cast<long>(PtTo10Mu( w ));
                height = static_cast<long>(PtTo10Mu( h ));
            }

            setPaperSize( width, height, mePageOrientation );
        }
    }

    return mpPrintInfo != nil;
}

// -----------------------------------------------------------------------

ULONG AquaSalInfoPrinter::GetPaperBinCount( const ImplJobSetup* i_pSetupData )
{
    return 0;
}

// -----------------------------------------------------------------------

XubString AquaSalInfoPrinter::GetPaperBinName( const ImplJobSetup* i_pSetupData, ULONG i_nPaperBin )
{
    return XubString();
}

// -----------------------------------------------------------------------

static bool getUseNativeDialog()
{
    bool bNative = true;
    try
    {
        // get service provider
        Reference< XMultiServiceFactory > xSMgr( unohelper::GetMultiServiceFactory() );
        // create configuration hierachical access name
        if( xSMgr.is() )
        {
            try
            {
                Reference< XMultiServiceFactory > xConfigProvider(
                    Reference< XMultiServiceFactory >(
                        xSMgr->createInstance( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                                        "com.sun.star.configuration.ConfigurationProvider" ))),
                        UNO_QUERY )
                    );
                if( xConfigProvider.is() )
                {
                    Sequence< Any > aArgs(1);
                    PropertyValue aVal;
                    aVal.Name = OUString( RTL_CONSTASCII_USTRINGPARAM( "nodepath" ) );
                    aVal.Value <<= OUString( RTL_CONSTASCII_USTRINGPARAM( "/org.openoffice.Office.Common/Misc" ) );
                    aArgs.getArray()[0] <<= aVal;
                    Reference< XNameAccess > xConfigAccess(
                        Reference< XNameAccess >(
                            xConfigProvider->createInstanceWithArguments( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                                                "com.sun.star.configuration.ConfigurationAccess" )),
                                                                            aArgs ),
                            UNO_QUERY )
                        );
                    if( xConfigAccess.is() )
                    {
                        try
                        {
                            sal_Bool bValue = sal_False;
                            Any aAny = xConfigAccess->getByName( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "UseSystemPrintDialog" ) ) );
                            if( aAny >>= bValue )
                                bNative = bValue;
                        }
                        catch( NoSuchElementException& )
                        {
                        }
                        catch( WrappedTargetException& )
                        {
                        }
                    }
                }
            }
            catch( Exception& )
            {
            }
        }
    }
    catch( WrappedTargetException& )
    {
    }

    return bNative;
}

ULONG AquaSalInfoPrinter::GetCapabilities( const ImplJobSetup* i_pSetupData, USHORT i_nType )
{
    switch( i_nType )
    {
        case PRINTER_CAPABILITIES_SUPPORTDIALOG:
            return 0;
        case PRINTER_CAPABILITIES_COPIES:
            return 0xffff;
        case PRINTER_CAPABILITIES_COLLATECOPIES:
            return 0;
        case PRINTER_CAPABILITIES_SETORIENTATION:
            return 1;
        case PRINTER_CAPABILITIES_SETDUPLEX:
            return 0;
        case PRINTER_CAPABILITIES_SETPAPERBIN:
            return 0;
        case PRINTER_CAPABILITIES_SETPAPERSIZE:
            return 1;
        case PRINTER_CAPABILITIES_SETPAPER:
            return 1;
        case PRINTER_CAPABILITIES_EXTERNALDIALOG:
            return getUseNativeDialog() ? 1 : 0;
        case PRINTER_CAPABILITIES_PDF:
            return 1;
        default: break;
    };
    return 0;
}

// -----------------------------------------------------------------------

void AquaSalInfoPrinter::GetPageInfo( const ImplJobSetup*,
                                  long& o_rOutWidth, long& o_rOutHeight,
                                  long& o_rPageOffX, long& o_rPageOffY,
                                  long& o_rPageWidth, long& o_rPageHeight )
{
    if( mpPrintInfo )
    {
        long nDPIX = 72, nDPIY = 72;
        mpGraphics->GetResolution( nDPIX, nDPIY );
        const double fXScaling = static_cast<double>(nDPIX)/72.0,
                     fYScaling = static_cast<double>(nDPIY)/72.0;

        NSSize aPaperSize = [mpPrintInfo paperSize];
        o_rPageWidth  = static_cast<long>( double(aPaperSize.width) * fXScaling );
        o_rPageHeight = static_cast<long>( double(aPaperSize.height) * fYScaling );

        NSRect aImageRect = [mpPrintInfo imageablePageBounds];
        o_rPageOffX   = static_cast<long>( aImageRect.origin.x * fXScaling );
        o_rPageOffY   = static_cast<long>( (aPaperSize.height - aImageRect.size.height - aImageRect.origin.y) * fYScaling );
        o_rOutWidth   = static_cast<long>( aImageRect.size.width * fXScaling );
        o_rOutHeight  = static_cast<long>( aImageRect.size.height * fYScaling );

        if( mePageOrientation == ORIENTATION_LANDSCAPE )
        {
            std::swap( o_rOutWidth, o_rOutHeight );
            std::swap( o_rPageWidth, o_rPageHeight );
            std::swap( o_rPageOffX, o_rPageOffY );
        }
    }
}

static Size getPageSize( vcl::PrinterController& i_rController, sal_Int32 i_nPage )
{
    Size aPageSize;
    Sequence< PropertyValue > aPageParms( i_rController.getPageParameters( i_nPage ) );
    for( sal_Int32 nProperty = 0, nPropertyCount = aPageParms.getLength(); nProperty < nPropertyCount; ++nProperty )
    {
        if( aPageParms[ nProperty ].Name.equalsAscii( "PageSize" ) )
        {
            awt::Size aSize;
            aPageParms[ nProperty].Value >>= aSize;
            aPageSize.Width() = aSize.Width;
            aPageSize.Height() = aSize.Height;
            break;
        }
    }
    return aPageSize;
}

BOOL AquaSalInfoPrinter::StartJob( const String* i_pFileName,
                                   const String& i_rJobName,
                                   const String& i_rAppName,
                                   ImplJobSetup* i_pSetupData,
                                   vcl::PrinterController& i_rController
                                   )
{
    if( mbJob )
        return FALSE;

    BOOL bSuccess = FALSE;
    bool bWasAborted = false;
    AquaSalInstance* pInst = GetSalData()->mpFirstInstance;
    PrintAccessoryViewState aAccViewState;
    sal_Int32 nAllPages = 0;

    aAccViewState.bNeedRestart = true;

    // reset IsLastPage
    i_rController.setLastPage( sal_False );

    // update job data
    if( i_pSetupData )
        SetData( ~0, i_pSetupData );

    // do we want a progress panel ?
    sal_Bool bShowProgressPanel = sal_True;
    beans::PropertyValue* pMonitor = i_rController.getValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MonitorVisible" ) ) );
    if( pMonitor )
        pMonitor->Value >>= bShowProgressPanel;
    if( ! i_rController.isShowDialogs() )
        bShowProgressPanel = sal_False;

    // FIXME: jobStarted() should be done after the print dialog has ended (if there is one)
    // how do I know when that might be ?
    i_rController.jobStarted();
    do
    {
        if( aAccViewState.bNeedRestart )
        {
            mnCurPageRangeStart = 0;
            mnCurPageRangeCount = 0;
            nAllPages = i_rController.getFilteredPageCount();
        }

        aAccViewState.bNeedRestart = false;

        Size aCurSize( 21000, 29700 );
        if( nAllPages > 0 )
        {
            mnCurPageRangeCount = 1;
            aCurSize = getPageSize( i_rController, mnCurPageRangeStart );
            Size aNextSize( aCurSize );

            // print pages up to a different size
            while( mnCurPageRangeCount + mnCurPageRangeStart < nAllPages )
            {
                aNextSize = getPageSize( i_rController, mnCurPageRangeStart + mnCurPageRangeCount );
                if( aCurSize == aNextSize // same page size
                    ||
                    (aCurSize.Width() == aNextSize.Height() && aCurSize.Height() == aNextSize.Width()) // same size, but different orientation
                    )
                {
                    mnCurPageRangeCount++;
                }
                else
                    break;
            }
        }
        else
            mnCurPageRangeCount = 0;

        // now for the current run
        mnStartPageOffsetX = mnStartPageOffsetY = 0;
        // setup the paper size and orientation
        setPaperSize( aCurSize.Width(), aCurSize.Height(), ORIENTATION_PORTRAIT );

        // create view
        NSView* pPrintView = [[AquaPrintView alloc] initWithController: &i_rController withInfoPrinter: this];

        NSMutableDictionary* pPrintDict = [mpPrintInfo dictionary];

        // set filename
        if( i_pFileName )
        {
            [mpPrintInfo setJobDisposition: NSPrintSaveJob];
            NSString* pPath = CreateNSString( *i_pFileName );
            [pPrintDict setObject: pPath forKey: NSPrintSavePath];
            [pPath release];
        }

        [pPrintDict setObject: [[NSNumber numberWithInt: (int)i_rController.getPrinter()->GetCopyCount()] autorelease] forKey: NSPrintCopies];
        [pPrintDict setObject: [[NSNumber numberWithBool: YES] autorelease] forKey: NSPrintDetailedErrorReporting];
        [pPrintDict setObject: [[NSNumber numberWithInt: 1] autorelease] forKey: NSPrintFirstPage];
        // #i103253# weird: for some reason, autoreleasing the value below like the others above
        // leads do a double free malloc error. Why this value should behave differently from all the others
        // is a mystery.
        [pPrintDict setObject: [NSNumber numberWithInt: mnCurPageRangeCount] forKey: NSPrintLastPage];


        // create print operation
        NSPrintOperation* pPrintOperation = [NSPrintOperation printOperationWithView: pPrintView printInfo: mpPrintInfo];

        if( pPrintOperation )
        {
            NSObject* pReleaseAfterUse = nil;
            bool bShowPanel = (! i_rController.isDirectPrint() && getUseNativeDialog() && i_rController.isShowDialogs() );
            [pPrintOperation setShowsPrintPanel: bShowPanel ? YES : NO ];
            [pPrintOperation setShowsProgressPanel: bShowProgressPanel ? YES : NO];

            // set job title (since MacOSX 10.5)
            if( [pPrintOperation respondsToSelector: @selector(setJobTitle:)] )
                [pPrintOperation performSelector: @selector(setJobTitle:) withObject: [CreateNSString( i_rJobName ) autorelease]];

            if( bShowPanel && mnCurPageRangeStart == 0 ) // only the first range of pages gets the accesory view
                pReleaseAfterUse = [AquaPrintAccessoryView setupPrinterPanel: pPrintOperation withController: &i_rController withState: &aAccViewState];

            bSuccess = TRUE;
            mbJob = true;
            pInst->startedPrintJob();
            [pPrintOperation runOperation];
            pInst->endedPrintJob();
            bWasAborted = [[[pPrintOperation printInfo] jobDisposition] compare: NSPrintCancelJob] == NSOrderedSame;
            mbJob = false;
            if( pReleaseAfterUse )
                [pReleaseAfterUse release];
        }

        mnCurPageRangeStart += mnCurPageRangeCount;
        mnCurPageRangeCount = 1;
    } while( aAccViewState.bNeedRestart || mnCurPageRangeStart + mnCurPageRangeCount < nAllPages );

    // inform applictation that it can release its data
    // this is awkward, but the XRenderable interface has no method for this,
    // so we need to call XRenderadble::render one last time with IsLastPage = TRUE
    if( nAllPages > 0 )
    {
        i_rController.setLastPage( sal_True );
        GDIMetaFile aPageFile;
        if( mrContext )
            SetupPrinterGraphics( mrContext );
        i_rController.getFilteredPageFile( 0, aPageFile );
    }

    i_rController.setJobState( bWasAborted
                             ? view::PrintableState_JOB_ABORTED
                             : view::PrintableState_JOB_SPOOLED );

    mnCurPageRangeStart = mnCurPageRangeCount = 0;

    return bSuccess;
}

// -----------------------------------------------------------------------

BOOL AquaSalInfoPrinter::EndJob()
{
    mnStartPageOffsetX = mnStartPageOffsetY = 0;
    mbJob = false;
    return TRUE;
}

// -----------------------------------------------------------------------

BOOL AquaSalInfoPrinter::AbortJob()
{
    mbJob = false;

    // FIXME: implementation
    return FALSE;
}

// -----------------------------------------------------------------------

SalGraphics* AquaSalInfoPrinter::StartPage( ImplJobSetup* i_pSetupData, BOOL i_bNewJobData )
{
    if( i_bNewJobData && i_pSetupData )
        SetPrinterData( i_pSetupData );

    CGContextRef rContext = reinterpret_cast<CGContextRef>([[NSGraphicsContext currentContext] graphicsPort]);

    SetupPrinterGraphics( rContext );

    return mpGraphics;
}

// -----------------------------------------------------------------------

BOOL AquaSalInfoPrinter::EndPage()
{
    return TRUE;
}

// -----------------------------------------------------------------------

ULONG AquaSalInfoPrinter::GetErrorCode() const
{
    return 0;
}

// =======================================================================

AquaSalPrinter::AquaSalPrinter( AquaSalInfoPrinter* i_pInfoPrinter ) :
    mpInfoPrinter( i_pInfoPrinter )
{
}

// -----------------------------------------------------------------------

AquaSalPrinter::~AquaSalPrinter()
{
}

// -----------------------------------------------------------------------

BOOL AquaSalPrinter::StartJob( const String* i_pFileName,
                               const String& i_rJobName,
                               const String& i_rAppName,
                               ImplJobSetup* i_pSetupData,
                               vcl::PrinterController& i_rController )
{
    return mpInfoPrinter->StartJob( i_pFileName, i_rJobName, i_rAppName, i_pSetupData, i_rController );
}

// -----------------------------------------------------------------------

BOOL AquaSalPrinter::StartJob( const XubString* i_pFileName,
                               const XubString& i_rJobName,
                               const XubString& i_rAppName,
                               ULONG i_nCopies,
                               bool i_bCollate,
                               bool i_bDirect,
                               ImplJobSetup* i_pSetupData )
{
    DBG_ERROR( "should never be called" );
    return FALSE;
}

// -----------------------------------------------------------------------

BOOL AquaSalPrinter::EndJob()
{
    return mpInfoPrinter->EndJob();
}

// -----------------------------------------------------------------------

BOOL AquaSalPrinter::AbortJob()
{
    return mpInfoPrinter->AbortJob();
}

// -----------------------------------------------------------------------

SalGraphics* AquaSalPrinter::StartPage( ImplJobSetup* i_pSetupData, BOOL i_bNewJobData )
{
    return mpInfoPrinter->StartPage( i_pSetupData, i_bNewJobData );
}

// -----------------------------------------------------------------------

BOOL AquaSalPrinter::EndPage()
{
    return mpInfoPrinter->EndPage();
}

// -----------------------------------------------------------------------

ULONG AquaSalPrinter::GetErrorCode()
{
    return mpInfoPrinter->GetErrorCode();
}

void AquaSalInfoPrinter::InitPaperFormats( const ImplJobSetup* i_pSetupData )
{
    m_aPaperFormats.clear();
    m_bPapersInit = true;

    if( mpPrinter )
    {
        if( [mpPrinter statusForTable: @"PPD"] == NSPrinterTableOK )
        {
            NSArray* pPaperNames = [mpPrinter stringListForKey: @"PageSize" inTable: @"PPD"];
            if( pPaperNames )
            {
                unsigned int nPapers = [pPaperNames count];
                for( unsigned int i = 0; i < nPapers; i++ )
                {
                    NSString* pPaper = [pPaperNames objectAtIndex: i];
                    NSSize aPaperSize = [mpPrinter pageSizeForPaper: pPaper];
                    if( aPaperSize.width > 0 && aPaperSize.height > 0 )
                    {
                        PaperInfo aInfo( PtTo10Mu( aPaperSize.width ),
                                         PtTo10Mu( aPaperSize.height ) );
                        m_aPaperFormats.push_back( aInfo );
                    }
                }
            }
        }
    }
}

const PaperInfo* AquaSalInfoPrinter::matchPaper( long i_nWidth, long i_nHeight, Orientation& o_rOrientation ) const
{
    if( ! m_bPapersInit )
        const_cast<AquaSalInfoPrinter*>(this)->InitPaperFormats( NULL );

    const PaperInfo* pMatch = NULL;
    o_rOrientation = ORIENTATION_PORTRAIT;
    for( int n = 0; n < 2 ; n++ )
    {
        for( size_t i = 0; i < m_aPaperFormats.size(); i++ )
        {
            if( abs( m_aPaperFormats[i].getWidth() - i_nWidth ) < 50 &&
                abs( m_aPaperFormats[i].getHeight() - i_nHeight ) < 50 )
            {
                pMatch = &m_aPaperFormats[i];
                return pMatch;
            }
        }
        o_rOrientation = ORIENTATION_LANDSCAPE;
        std::swap( i_nWidth, i_nHeight );
    }
    return pMatch;
}

int AquaSalInfoPrinter::GetLandscapeAngle( const ImplJobSetup* i_pSetupData )
{
    return 900;
}


