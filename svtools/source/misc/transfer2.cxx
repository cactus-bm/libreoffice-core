/*************************************************************************
 *
 *  $RCSfile: transfer2.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: rt $ $Date: 2005-01-11 13:12:26 $
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

#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _UNTOOLS_UCBSTREAMHELPER_HXX
#include <unotools/ucbstreamhelper.hxx>
#endif
#ifndef _SOT_EXCHANGE_HXX
#include <sot/exchange.hxx>
#endif
#ifndef _SOT_STORAGE_HXX
#include <sot/storage.hxx>
#endif
#ifndef _SV_BITMAP_HXX
#include <vcl/bitmap.hxx>
#endif
#ifndef _SV_GDIMTF_HXX
#include <vcl/gdimtf.hxx>
#endif
#ifndef _SV_GRAPH_HXX
#include <vcl/graph.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SV_WINDOW_HXX
#include <vcl/window.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_DATATRANSFER_DND_DROPTARGETDRAGCONTEXT_HPP_
#include <com/sun/star/datatransfer/dnd/XDropTargetDragContext.hpp>
#endif

#include "urlbmk.hxx"
#include "inetimg.hxx"
#include "imap.hxx"
#include "transfer.hxx"

// --------------
// - Namespaces -
// --------------

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::datatransfer;
using namespace ::com::sun::star::datatransfer::clipboard;
using namespace ::com::sun::star::datatransfer::dnd;

// -----------------------------------------
// - DragSourceHelper::DragGestureListener -
// -----------------------------------------

DragSourceHelper::DragGestureListener::DragGestureListener( DragSourceHelper& rDragSourceHelper ) :
    mrParent( rDragSourceHelper )
{
}

// -----------------------------------------------------------------------------

DragSourceHelper::DragGestureListener::~DragGestureListener()
{
}

// -----------------------------------------------------------------------------

void SAL_CALL DragSourceHelper::DragGestureListener::disposing( const EventObject& Source ) throw( RuntimeException )
{
}

// -----------------------------------------------------------------------------

void SAL_CALL DragSourceHelper::DragGestureListener::dragGestureRecognized( const DragGestureEvent& rDGE ) throw( RuntimeException )
{
    const ::vos::OGuard aGuard( Application::GetSolarMutex() );

    const Point aPtPixel( rDGE.DragOriginX, rDGE.DragOriginY );
    mrParent.StartDrag( rDGE.DragAction, aPtPixel );
}

// --------------------
// - DragSourceHelper -
// --------------------

DragSourceHelper::DragSourceHelper( Window* pWindow ) :
    mxDragGestureRecognizer( pWindow->GetDragGestureRecognizer() )
{
    if( mxDragGestureRecognizer.is() )
    {
        mxDragGestureListener = new DragSourceHelper::DragGestureListener( *this );
        mxDragGestureRecognizer->addDragGestureListener( mxDragGestureListener );
    }
}

// -----------------------------------------------------------------------------

DragSourceHelper::~DragSourceHelper()
{
    if( mxDragGestureRecognizer.is()  )
        mxDragGestureRecognizer->removeDragGestureListener( mxDragGestureListener );
}

// -----------------------------------------------------------------------------

void DragSourceHelper::StartDrag( sal_Int8 nAction, const Point& rPosPixel )
{
}

// ----------------------------------------
// - DropTargetHelper::DropTargetListener -
// ----------------------------------------

DropTargetHelper::DropTargetListener::DropTargetListener( DropTargetHelper& rDropTargetHelper ) :
    mrParent( rDropTargetHelper ),
    mpLastDragOverEvent( NULL )
{
}

// -----------------------------------------------------------------------------

DropTargetHelper::DropTargetListener::~DropTargetListener()
{
    delete mpLastDragOverEvent;
}

// -----------------------------------------------------------------------------

void SAL_CALL DropTargetHelper::DropTargetListener::disposing( const EventObject& rSource ) throw( RuntimeException )
{
}

// -----------------------------------------------------------------------------

void SAL_CALL DropTargetHelper::DropTargetListener::drop( const DropTargetDropEvent& rDTDE ) throw( RuntimeException )
{
    const ::vos::OGuard aGuard( Application::GetSolarMutex() );

    try
    {
        AcceptDropEvent  aAcceptEvent;
        ExecuteDropEvent aExecuteEvt( rDTDE.DropAction & ~DNDConstants::ACTION_DEFAULT, Point( rDTDE.LocationX, rDTDE.LocationY ), rDTDE );
        sal_Int8         nRet = DNDConstants::ACTION_NONE;

        aExecuteEvt.mbDefault = ( ( rDTDE.DropAction & DNDConstants::ACTION_DEFAULT ) != 0 );

        // in case of a default action, call ::AcceptDrop first and use the returned
        // accepted action as the execute action in the call to ::ExecuteDrop
        aAcceptEvent.mnAction = aExecuteEvt.mnAction;
        aAcceptEvent.maPosPixel = aExecuteEvt.maPosPixel;
        (DropTargetEvent&)( aAcceptEvent.maDragEvent ) = (DropTargetEvent&) rDTDE;
        ( (DropTargetDragEvent&)( aAcceptEvent.maDragEvent ) ).DropAction = rDTDE.DropAction;
        ( (DropTargetDragEvent&)( aAcceptEvent.maDragEvent ) ).LocationX = rDTDE.LocationX;
        ( (DropTargetDragEvent&)( aAcceptEvent.maDragEvent ) ).LocationY = rDTDE.LocationY;
        ( (DropTargetDragEvent&)( aAcceptEvent.maDragEvent ) ).SourceActions = rDTDE.SourceActions;
        aAcceptEvent.mbLeaving = sal_False;
        aAcceptEvent.mbDefault = aExecuteEvt.mbDefault;

        nRet = mrParent.AcceptDrop( aAcceptEvent );

        if( DNDConstants::ACTION_NONE != nRet )
        {
            rDTDE.Context->acceptDrop( nRet );

            if( aExecuteEvt.mbDefault )
                aExecuteEvt.mnAction = nRet;

            nRet = mrParent.ExecuteDrop( aExecuteEvt );
        }

        rDTDE.Context->dropComplete( DNDConstants::ACTION_NONE != nRet );

        if( mpLastDragOverEvent )
        {
            delete mpLastDragOverEvent;
            mpLastDragOverEvent = NULL;
        }
    }
    catch( const ::com::sun::star::uno::Exception& )
    {
    }
}

// -----------------------------------------------------------------------------

void SAL_CALL DropTargetHelper::DropTargetListener::dragEnter( const DropTargetDragEnterEvent& rDTDEE ) throw( RuntimeException )
{
    const ::vos::OGuard aGuard( Application::GetSolarMutex() );

    try
    {
        mrParent.ImplBeginDrag( rDTDEE.SupportedDataFlavors );
    }
    catch( const ::com::sun::star::uno::Exception& )
    {
    }

    dragOver( rDTDEE );
}

// -----------------------------------------------------------------------------

void SAL_CALL DropTargetHelper::DropTargetListener::dragOver( const DropTargetDragEvent& rDTDE ) throw( RuntimeException )
{
    const ::vos::OGuard aGuard( Application::GetSolarMutex() );

    try
    {
        if( mpLastDragOverEvent )
            delete mpLastDragOverEvent;

        mpLastDragOverEvent = new AcceptDropEvent( rDTDE.DropAction & ~DNDConstants::ACTION_DEFAULT, Point( rDTDE.LocationX, rDTDE.LocationY ), rDTDE );
        mpLastDragOverEvent->mbDefault = ( ( rDTDE.DropAction & DNDConstants::ACTION_DEFAULT ) != 0 );

        const sal_Int8 nRet = mrParent.AcceptDrop( *mpLastDragOverEvent );

        if( DNDConstants::ACTION_NONE == nRet )
            rDTDE.Context->rejectDrag();
        else
            rDTDE.Context->acceptDrag( nRet );
    }
    catch( const ::com::sun::star::uno::Exception& )
    {
    }
}

// -----------------------------------------------------------------------------

void SAL_CALL DropTargetHelper::DropTargetListener::dragExit( const DropTargetEvent& dte ) throw( RuntimeException )
{
    const ::vos::OGuard aGuard( Application::GetSolarMutex() );

    try
    {
        if( mpLastDragOverEvent )
        {
            mpLastDragOverEvent->mbLeaving = sal_True;
            mrParent.AcceptDrop( *mpLastDragOverEvent );
            delete mpLastDragOverEvent;
            mpLastDragOverEvent = NULL;
        }

        mrParent.ImplEndDrag();
    }
    catch( const ::com::sun::star::uno::Exception& )
    {
    }
}


// -----------------------------------------------------------------------------

void SAL_CALL DropTargetHelper::DropTargetListener::dropActionChanged( const DropTargetDragEvent& dtde ) throw( RuntimeException )
{
}

// --------------------
// - DropTargetHelper -
// --------------------

DropTargetHelper::DropTargetHelper( Window* pWindow ) :
    mxDropTarget( pWindow->GetDropTarget() ),
    mpFormats( new DataFlavorExVector )
{
    ImplConstruct();
}

// -----------------------------------------------------------------------------

DropTargetHelper::DropTargetHelper( const Reference< XDropTarget >& rxDropTarget ) :
    mxDropTarget( rxDropTarget ),
    mpFormats( new DataFlavorExVector )
{
    ImplConstruct();
}

// -----------------------------------------------------------------------------

DropTargetHelper::~DropTargetHelper()
{
    if( mxDropTarget.is() )
        mxDropTarget->removeDropTargetListener( mxDropTargetListener );

    delete mpFormats;
}

// -----------------------------------------------------------------------------

void DropTargetHelper::ImplConstruct()
{
    if( mxDropTarget.is() )
    {
        mxDropTargetListener = new DropTargetHelper::DropTargetListener( *this );
        mxDropTarget->addDropTargetListener( mxDropTargetListener );
        mxDropTarget->setActive( sal_True );
    }
}

// -----------------------------------------------------------------------------

void DropTargetHelper::ImplBeginDrag( const Sequence< DataFlavor >& rSupportedDataFlavors )
{
    DataFlavorEx        aFlavorEx;
    const DataFlavor*   pFlavor = rSupportedDataFlavors.getConstArray();

    mpFormats->clear();
    TransferableDataHelper::FillDataFlavorExVector( rSupportedDataFlavors, *mpFormats );
}

// -----------------------------------------------------------------------------

void DropTargetHelper::ImplEndDrag()
{
    mpFormats->clear();
}

// -----------------------------------------------------------------------------

sal_Int8 DropTargetHelper::AcceptDrop( const AcceptDropEvent& rEvt )
{
    return( DNDConstants::ACTION_NONE );
}

// -----------------------------------------------------------------------------

sal_Int8 DropTargetHelper::ExecuteDrop( const ExecuteDropEvent& rEvt )
{
    return( DNDConstants::ACTION_NONE );
}

// -----------------------------------------------------------------------------

sal_Bool DropTargetHelper::IsDropFormatSupported( SotFormatStringId nFormat )
{
    DataFlavorExVector::iterator    aIter( mpFormats->begin() ), aEnd( mpFormats->end() );
    sal_Bool                        bRet = sal_False;

    while( aIter != aEnd )
    {
        if( nFormat == (*aIter++).mnSotId )
        {
            bRet = sal_True;
            aIter = aEnd;
        }
    }

    return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool DropTargetHelper::IsDropFormatSupported( const DataFlavor& rFlavor )
{
    DataFlavorExVector::iterator    aIter( mpFormats->begin() ), aEnd( mpFormats->end() );
    sal_Bool                        bRet = sal_False;

    while( aIter != aEnd )
    {
        if( TransferableDataHelper::IsEqual( rFlavor, *aIter++ ) )
        {
            bRet = sal_True;
            aIter = aEnd;
        }
    }

    return bRet;
}

// -----------------------------------------------------------------------------
// TransferDataContainer
// -----------------------------------------------------------------------------

struct TDataCntnrEntry_Impl
{
    ::com::sun::star::uno::Any aAny;
    SotFormatStringId nId;
};

// -----------------------------------------------------------------------------

typedef ::std::list< TDataCntnrEntry_Impl > TDataCntnrEntryList;

// -----------------------------------------------------------------------------

struct TransferDataContainer_Impl
{
    TDataCntnrEntryList aFmtList;
    Link aFinshedLnk;
    INetBookmark* pBookmk;
    Graphic* pGrf;

    TransferDataContainer_Impl()
        : pBookmk( 0 ), pGrf( 0 )
    {
    }

    ~TransferDataContainer_Impl()
    {
        delete pBookmk;
        delete pGrf;
    }
};

// -----------------------------------------------------------------------------

TransferDataContainer::TransferDataContainer()
    : pImpl( new TransferDataContainer_Impl )
{
}

// -----------------------------------------------------------------------------

TransferDataContainer::~TransferDataContainer()
{
    delete pImpl;
}

// -----------------------------------------------------------------------------

void TransferDataContainer::AddSupportedFormats()
{
}

// -----------------------------------------------------------------------------

sal_Bool TransferDataContainer::GetData( const
            ::com::sun::star::datatransfer::DataFlavor& rFlavor )
{
    TDataCntnrEntryList::iterator   aIter( pImpl->aFmtList.begin() ),
                                    aEnd( pImpl->aFmtList.end() );
    sal_Bool bFnd = sal_False;
    ULONG nFmtId = SotExchange::GetFormat( rFlavor );

    // test first the list
    for( ; aIter != aEnd; ++aIter )
    {
        TDataCntnrEntry_Impl& rEntry = (TDataCntnrEntry_Impl&)*aIter;
        if( nFmtId == rEntry.nId )
        {
            bFnd = SetAny( rEntry.aAny, rFlavor );
            break;
        }
    }

    // test second the bookmark pointer
    if( !bFnd )
        switch( nFmtId )
        {
         case SOT_FORMAT_STRING:
         case SOT_FORMATSTR_ID_SOLK:
         case SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK:
         case SOT_FORMATSTR_ID_FILECONTENT:
         case SOT_FORMATSTR_ID_FILEGRPDESCRIPTOR:
         case SOT_FORMATSTR_ID_UNIFORMRESOURCELOCATOR:
            if( pImpl->pBookmk )
                bFnd = SetINetBookmark( *pImpl->pBookmk, rFlavor );
            break;

        case SOT_FORMATSTR_ID_SVXB:
        case SOT_FORMAT_BITMAP:
        case SOT_FORMAT_GDIMETAFILE:
            if( pImpl->pGrf )
                bFnd = SetGraphic( *pImpl->pGrf, rFlavor );
            break;
        }

    return bFnd;
}

// -----------------------------------------------------------------------------

void TransferDataContainer::ClearData()
{
    delete pImpl;
    pImpl = new TransferDataContainer_Impl;
    ClearFormats();
}

// -----------------------------------------------------------------------------

void TransferDataContainer::CopyINetBookmark( const INetBookmark& rBkmk )
{
    if( !pImpl->pBookmk )
        pImpl->pBookmk = new INetBookmark( rBkmk );
    else
        *pImpl->pBookmk = rBkmk;

     AddFormat( SOT_FORMAT_STRING );
     AddFormat( SOT_FORMATSTR_ID_SOLK );
     AddFormat( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK );
     AddFormat( SOT_FORMATSTR_ID_FILECONTENT );
     AddFormat( SOT_FORMATSTR_ID_FILEGRPDESCRIPTOR );
     AddFormat( SOT_FORMATSTR_ID_UNIFORMRESOURCELOCATOR );
}

// -----------------------------------------------------------------------------

void TransferDataContainer::CopyAnyData( ULONG nFormatId,
                                        const sal_Char* pData, ULONG nLen )
{
    if( nLen )
    {
        TDataCntnrEntry_Impl aEntry;
        aEntry.nId = nFormatId;

        Sequence< sal_Int8 > aSeq( nLen  );
        memcpy( aSeq.getArray(), pData, nLen );
        aEntry.aAny <<= aSeq;
        pImpl->aFmtList.push_back( aEntry );
         AddFormat( nFormatId );
    }
}

// -----------------------------------------------------------------------------

void TransferDataContainer::CopyByteString( ULONG nFormatId,
                                            const ByteString& rStr )
{
    CopyAnyData( nFormatId, rStr.GetBuffer(), rStr.Len() );
}

// -----------------------------------------------------------------------------

void TransferDataContainer::CopyINetImage( const INetImage& rINtImg )
{
    SvMemoryStream aMemStm( 1024, 1024 );
    aMemStm.SetVersion( SOFFICE_FILEFORMAT_50 );
    rINtImg.Write( aMemStm, SOT_FORMATSTR_ID_INET_IMAGE );
    CopyAnyData( SOT_FORMATSTR_ID_INET_IMAGE, (sal_Char*)aMemStm.GetData(),
                    aMemStm.Seek( STREAM_SEEK_TO_END ) );
}

// -----------------------------------------------------------------------------

void TransferDataContainer::CopyImageMap( const ImageMap& rImgMap )
{
    SvMemoryStream aMemStm( 8192, 8192 );
    aMemStm.SetVersion( SOFFICE_FILEFORMAT_50 );
    rImgMap.Write( aMemStm, String() );
    CopyAnyData( SOT_FORMATSTR_ID_SVIM, (sal_Char*)aMemStm.GetData(),
                    aMemStm.Seek( STREAM_SEEK_TO_END ) );
}

// -----------------------------------------------------------------------------

void TransferDataContainer::CopyGraphic( const Graphic& rGrf )
{
    USHORT nType = rGrf.GetType();
    if( GRAPHIC_NONE != nType )
    {
        if( !pImpl->pGrf )
            pImpl->pGrf = new Graphic( rGrf );
        else
            *pImpl->pGrf = rGrf;

        AddFormat( SOT_FORMATSTR_ID_SVXB );
        if( GRAPHIC_BITMAP == nType )
            AddFormat( SOT_FORMAT_BITMAP );
        else if( GRAPHIC_GDIMETAFILE == nType )
            AddFormat( SOT_FORMAT_GDIMETAFILE );
    }
}

// -----------------------------------------------------------------------------

void TransferDataContainer::CopyString( USHORT nFmt, const String& rStr )
{
    if( rStr.Len() )
    {
        TDataCntnrEntry_Impl aEntry;
        aEntry.nId = nFmt;
        rtl::OUString aStr( rStr );
        aEntry.aAny <<= aStr;
        pImpl->aFmtList.push_back( aEntry );
         AddFormat( aEntry.nId );
    }
}

// -----------------------------------------------------------------------------

void TransferDataContainer::CopyString( const String& rStr )
{
    CopyString( SOT_FORMAT_STRING, rStr );
}

// -----------------------------------------------------------------------------

void TransferDataContainer::CopyAny( USHORT nFmt,
                                    const ::com::sun::star::uno::Any& rAny )
{
    TDataCntnrEntry_Impl aEntry;
    aEntry.nId = nFmt;
    aEntry.aAny = rAny;
    pImpl->aFmtList.push_back( aEntry );
    AddFormat( aEntry.nId );
}

// -----------------------------------------------------------------------------

sal_Bool TransferDataContainer::HasAnyData() const
{
    return pImpl->aFmtList.begin() != pImpl->aFmtList.end() ||
            0 != pImpl->pBookmk;
}

// -----------------------------------------------------------------------------

void TransferDataContainer::StartDrag(
        Window* pWindow, sal_Int8 nDragSourceActions,
        const Link& rLnk, sal_Int32 nDragPointer, sal_Int32 nDragImage )
{
    pImpl->aFinshedLnk = rLnk;
    TransferableHelper::StartDrag( pWindow, nDragSourceActions,
                                    nDragPointer, nDragImage );
}

// -----------------------------------------------------------------------------

void TransferDataContainer::DragFinished( sal_Int8 nDropAction )
{
    if( pImpl->aFinshedLnk.IsSet() )
        pImpl->aFinshedLnk.Call( &nDropAction );
}
