/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: embedhlp.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: kz $ $Date: 2005-11-03 12:02:48 $
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

#include "embedhlp.hxx"

#include "filter.hxx"
#include "svtools.hrc"
#include "svtdata.hxx"

#include <comphelper/embeddedobjectcontainer.hxx>
#include <comphelper/seqstream.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <unotools/ucbstreamhelper.hxx>
#include <unotools/streamwrap.hxx>

#ifndef _COM_SUN_STAR_UTIL_XMODIFYLISTENER_HPP_
#include <com/sun/star/util/XModifyListener.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XMODIFYiBLE_HPP_
#include <com/sun/star/util/XModifiable.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_EMBEDSTATES_HPP_
#include <com/sun/star/embed/EmbedStates.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XSTATECHANGELISTENER_HPP_
#include <com/sun/star/embed/XStateChangeListener.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XMODIFIABLE_HPP_
#include <com/sun/star/util/XModifiable.hpp>
#endif
#ifndef _COM_SUN_STAR_DATATRANSFER_XTRANSFERABLE_HPP_
#include <com/sun/star/datatransfer/XTransferable.hpp>
#endif

#ifndef _CPPUHELPER_IMPLBASE4_HXX_
#include <cppuhelper/implbase4.hxx>
#endif

#ifndef _SV_SVAPP_HXX
#include "vcl/svapp.hxx"
#endif

using namespace com::sun::star;

namespace svt
{

class EmbedEventListener_Impl : public ::cppu::WeakImplHelper4 < embed::XStateChangeListener,
                                                                 document::XEventListener,
                                                                 util::XModifyListener,
                                                                 util::XCloseListener >
{
public:
    EmbeddedObjectRef*          pObject;
    sal_Int32                   nState;

                                EmbedEventListener_Impl( EmbeddedObjectRef* p ) :
                                    pObject(p)
                                    , nState(-1)
                                {}

    static EmbedEventListener_Impl* Create( EmbeddedObjectRef* );

    virtual void SAL_CALL changingState( const lang::EventObject& aEvent, ::sal_Int32 nOldState, ::sal_Int32 nNewState )
                                    throw (embed::WrongStateException, uno::RuntimeException);
    virtual void SAL_CALL stateChanged( const lang::EventObject& aEvent, ::sal_Int32 nOldState, ::sal_Int32 nNewState )
                                    throw (uno::RuntimeException);
    virtual void SAL_CALL queryClosing( const lang::EventObject& Source, ::sal_Bool GetsOwnership )
                                    throw (util::CloseVetoException, uno::RuntimeException);
    virtual void SAL_CALL notifyClosing( const lang::EventObject& Source ) throw (uno::RuntimeException);
    virtual void SAL_CALL notifyEvent( const document::EventObject& aEvent ) throw( uno::RuntimeException );
    virtual void SAL_CALL disposing( const lang::EventObject& aEvent ) throw( uno::RuntimeException );
    virtual void SAL_CALL modified( const ::com::sun::star::lang::EventObject& aEvent ) throw (::com::sun::star::uno::RuntimeException);
};

EmbedEventListener_Impl* EmbedEventListener_Impl::Create( EmbeddedObjectRef* p )
{
    EmbedEventListener_Impl* xRet = new EmbedEventListener_Impl( p );
    xRet->acquire();

    if ( p->GetObject().is() )
    {
        p->GetObject()->addStateChangeListener( xRet );

        uno::Reference < util::XCloseable > xClose( p->GetObject(), uno::UNO_QUERY );
        DBG_ASSERT( xClose.is(), "Object does not support XCloseable!" );
        if ( xClose.is() )
            xClose->addCloseListener( xRet );

        uno::Reference < document::XEventBroadcaster > xBrd( p->GetObject(), uno::UNO_QUERY );
        if ( xBrd.is() )
            xBrd->addEventListener( xRet );

        xRet->nState = p->GetObject()->getCurrentState();
        if ( xRet->nState == embed::EmbedStates::RUNNING )
        {
            uno::Reference < util::XModifiable > xMod( p->GetObject()->getComponent(), uno::UNO_QUERY );
            if ( xMod.is() )
                // listen for changes in running state (update replacements in case of changes)
                xMod->addModifyListener( xRet );
        }
    }

    return xRet;
}

void SAL_CALL EmbedEventListener_Impl::changingState( const lang::EventObject& aEvent,
                                                    ::sal_Int32 nOldState,
                                                    ::sal_Int32 nNewState )
    throw ( embed::WrongStateException,
            uno::RuntimeException )
{
}

void SAL_CALL EmbedEventListener_Impl::stateChanged( const lang::EventObject& aEvent,
                                                    ::sal_Int32 nOldState,
                                                    ::sal_Int32 nNewState )
    throw ( uno::RuntimeException )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    nState = nNewState;
    if ( !pObject )
        return;

    uno::Reference < util::XModifiable > xMod( pObject->GetObject()->getComponent(), uno::UNO_QUERY );
    if ( nNewState == embed::EmbedStates::RUNNING )
    {
        // TODO/LATER: container must be set before!
        // When is this event created? Who sets the new container when it changed?
        if( nOldState != embed::EmbedStates::LOADED )
            // get new replacement after deactivation
            pObject->UpdateReplacement();

        if ( xMod.is() && nOldState == embed::EmbedStates::LOADED )
            // listen for changes (update replacements in case of changes)
            xMod->addModifyListener( this );
    }
    else if ( nNewState == embed::EmbedStates::LOADED )
    {
        // in loaded state we can't listen
        if ( xMod.is() )
            xMod->removeModifyListener( this );
    }
}

void SAL_CALL EmbedEventListener_Impl::modified( const lang::EventObject& aEvent ) throw (uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( pObject )
    {
        if ( nState == embed::EmbedStates::RUNNING )
        {
            // updates only necessary in non-active states
            pObject->UpdateReplacement();
        }
        else if ( nState == embed::EmbedStates::UI_ACTIVE || nState == embed::EmbedStates::INPLACE_ACTIVE )
        {
            // in case the object is inplace or UI active the replacement image should be updated on demand
            pObject->UpdateReplacementOnDemand();
        }
    }

}

void SAL_CALL EmbedEventListener_Impl::notifyEvent( const document::EventObject& aEvent ) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );

#if 0
    if ( pObject && aEvent.EventName.equalsAscii("OnSaveDone") || aEvent.EventName.equalsAscii("OnSaveAsDone") )
    {
        // TODO/LATER: container must be set before!
        // When is this event created? Who sets the new container when it changed?
        pObject->UpdateReplacement();
    }
    else
#endif
    if ( pObject && aEvent.EventName.equalsAscii("OnVisAreaChanged") )
    {
        pObject->UpdateReplacement();
    }
}

void SAL_CALL EmbedEventListener_Impl::queryClosing( const lang::EventObject& Source, ::sal_Bool GetsOwnership )
        throw ( util::CloseVetoException, uno::RuntimeException)
{
    // An embedded object can be shared between several objects (f.e. for undo purposes)
    // the object will not be closed before the last "customer" is destroyed
    // Now the EmbeddedObjectRef helper class works like a "lock" on the object
    if ( pObject && pObject->IsLocked() && Source.Source == pObject->GetObject() )
        throw util::CloseVetoException();
}

void SAL_CALL EmbedEventListener_Impl::notifyClosing( const lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException)
{
    if ( pObject && Source.Source == pObject->GetObject() )
    {
        pObject->Clear();
        pObject = 0;
    }
}

void SAL_CALL EmbedEventListener_Impl::disposing( const lang::EventObject& aEvent ) throw( uno::RuntimeException )
{
    if ( pObject && aEvent.Source == pObject->GetObject() )
    {
        pObject->Clear();
        pObject = 0;
    }
}

struct EmbeddedObjectRef_Impl
{
    EmbedEventListener_Impl*                    xListener;
    ::rtl::OUString                             aPersistName;
    ::rtl::OUString                             aMediaType;
    comphelper::EmbeddedObjectContainer*        pContainer;
    Graphic*                                    pGraphic;
    sal_Int64                                   nViewAspect;
    BOOL                                        bIsLocked;
    sal_Bool                                    bNeedUpdate;
};

void EmbeddedObjectRef::Construct_Impl()
{
    mpImp = new EmbeddedObjectRef_Impl;
    mpImp->pContainer = 0;
    mpImp->pGraphic = 0;
    mpImp->nViewAspect = embed::Aspects::MSOLE_CONTENT;
    mpImp->bIsLocked = FALSE;
    mpImp->bNeedUpdate = sal_False;
}

EmbeddedObjectRef::EmbeddedObjectRef()
{
    Construct_Impl();
}

EmbeddedObjectRef::EmbeddedObjectRef( const NS_UNO::Reference < NS_EMBED::XEmbeddedObject >& xObj, sal_Int64 nAspect )
{
    Construct_Impl();
    mpImp->nViewAspect = nAspect;
    mxObj = xObj;
    mpImp->xListener = EmbedEventListener_Impl::Create( this );
}

EmbeddedObjectRef::EmbeddedObjectRef( const EmbeddedObjectRef& rObj )
{
    mpImp = new EmbeddedObjectRef_Impl;
    mpImp->pContainer = rObj.mpImp->pContainer;
    mpImp->nViewAspect = rObj.mpImp->nViewAspect;
    mpImp->bIsLocked = rObj.mpImp->bIsLocked;
    mxObj = rObj.mxObj;
    mpImp->xListener = EmbedEventListener_Impl::Create( this );
    mpImp->aPersistName = rObj.mpImp->aPersistName;
    mpImp->aMediaType = rObj.mpImp->aMediaType;
    mpImp->bNeedUpdate = rObj.mpImp->bNeedUpdate;

    if ( rObj.mpImp->pGraphic && !rObj.mpImp->bNeedUpdate )
        mpImp->pGraphic = new Graphic( *rObj.mpImp->pGraphic );
    else
        mpImp->pGraphic = 0;
}

EmbeddedObjectRef::~EmbeddedObjectRef()
{
    delete mpImp->pGraphic;
    Clear();
}
/*
EmbeddedObjectRef& EmbeddedObjectRef::operator = ( const EmbeddedObjectRef& rObj )
{
    DBG_ASSERT( !mxObj.is(), "Never assign an already assigned object!" );

    delete mpImp->pGraphic;
    Clear();

    mpImp->nViewAspect = rObj.mpImp->nViewAspect;
    mpImp->bIsLocked = rObj.mpImp->bIsLocked;
    mxObj = rObj.mxObj;
    mpImp->xListener = EmbedEventListener_Impl::Create( this );
    mpImp->pContainer = rObj.mpImp->pContainer;
    mpImp->aPersistName = rObj.mpImp->aPersistName;
    mpImp->aMediaType = rObj.mpImp->aMediaType;
    mpImp->bNeedUpdate = rObj.mpImp->bNeedUpdate;

    if ( rObj.mpImp->pGraphic && !rObj.mpImp->bNeedUpdate )
        mpImp->pGraphic = new Graphic( *rObj.mpImp->pGraphic );
    else
        mpImp->pGraphic = 0;
    return *this;
}
*/
void EmbeddedObjectRef::Assign( const NS_UNO::Reference < NS_EMBED::XEmbeddedObject >& xObj, sal_Int64 nAspect )
{
    DBG_ASSERT( !mxObj.is(), "Never assign an already assigned object!" );

    Clear();
    mpImp->nViewAspect = nAspect;
    mxObj = xObj;
    mpImp->xListener = EmbedEventListener_Impl::Create( this );
}

void EmbeddedObjectRef::Clear()
{
    if ( mxObj.is() && mpImp->xListener )
    {
        mxObj->removeStateChangeListener( mpImp->xListener );

        uno::Reference < util::XCloseable > xClose( mxObj, uno::UNO_QUERY );
        if ( xClose.is() )
            xClose->removeCloseListener( mpImp->xListener );

        uno::Reference < document::XEventBroadcaster > xBrd( mxObj, uno::UNO_QUERY );
        if ( xBrd.is() )
            xBrd->removeEventListener( mpImp->xListener );

        if ( mpImp->bIsLocked )
        {
            if ( xClose.is() )
            {
                try
                {
                    mxObj->changeState( embed::EmbedStates::LOADED );
                    xClose->close( sal_True );
                }
                catch ( util::CloseVetoException& )
                {
                    // there's still someone who needs the object!
                }
            }
        }

        if ( mpImp->xListener )
        {
            mpImp->xListener->pObject = 0;
            mpImp->xListener->release();
            mpImp->xListener = 0;
        }

        mxObj = 0;
        mpImp->bNeedUpdate = sal_False;
    }
}

void EmbeddedObjectRef::AssignToContainer( comphelper::EmbeddedObjectContainer* pContainer, const ::rtl::OUString& rPersistName )
{
    mpImp->pContainer = pContainer;
    mpImp->aPersistName = rPersistName;

    if ( mpImp->pGraphic && !mpImp->bNeedUpdate && pContainer )
        SetGraphicToContainer( *mpImp->pGraphic, *pContainer, mpImp->aPersistName, ::rtl::OUString() );
}

comphelper::EmbeddedObjectContainer* EmbeddedObjectRef::GetContainer() const
{
    return mpImp->pContainer;
}

::rtl::OUString EmbeddedObjectRef::GetPersistName() const
{
    return mpImp->aPersistName;
}

MapUnit EmbeddedObjectRef::GetMapUnit() const
{
    return VCLUnoHelper::UnoEmbed2VCLMapUnit( mxObj->getMapUnit( mpImp->nViewAspect ) );
}

sal_Int64 EmbeddedObjectRef::GetViewAspect() const
{
    return mpImp->nViewAspect;
}

void EmbeddedObjectRef::Lock( BOOL bLock )
{
    mpImp->bIsLocked = bLock;
}

BOOL EmbeddedObjectRef::IsLocked() const
{
    return mpImp->bIsLocked;
}

void EmbeddedObjectRef::GetReplacement( BOOL bUpdate )
{
    if ( bUpdate )
    {
        DELETEZ( mpImp->pGraphic );
        mpImp->aMediaType = ::rtl::OUString();
        mpImp->pGraphic = new Graphic;
    }
    else if ( !mpImp->pGraphic )
        mpImp->pGraphic = new Graphic;
    else
    {
        DBG_ERROR("No update, but replacement exists already!");
        return;
    }

    SvStream* pGraphicStream = GetGraphicStream( bUpdate );
    if ( pGraphicStream )
    {
        GraphicFilter* pGF = GraphicFilter::GetGraphicFilter();
        pGF->ImportGraphic( *mpImp->pGraphic, String(), *pGraphicStream, GRFILTER_FORMAT_DONTKNOW );
        delete pGraphicStream;
    }
}

Graphic* EmbeddedObjectRef::GetGraphic( ::rtl::OUString* pMediaType ) const
{
    if ( mpImp->bNeedUpdate )
        // bNeedUpdate will be set to false while retrieving new replacement
        const_cast < EmbeddedObjectRef* >(this)->GetReplacement( sal_True );
    else if ( !mpImp->pGraphic )
        const_cast < EmbeddedObjectRef* >(this)->GetReplacement( FALSE );

    if ( mpImp->pGraphic && pMediaType )
        *pMediaType = mpImp->aMediaType;
    return mpImp->pGraphic;
}

void EmbeddedObjectRef::SetGraphic( const Graphic& rGraphic, const ::rtl::OUString& rMediaType )
{
    if ( mpImp->pGraphic )
        delete mpImp->pGraphic;
    mpImp->pGraphic = new Graphic( rGraphic );
    mpImp->aMediaType = rMediaType;

    if ( mpImp->pContainer )
        SetGraphicToContainer( rGraphic, *mpImp->pContainer, mpImp->aPersistName, rMediaType );

    mpImp->bNeedUpdate = sal_False;
}

SvStream* EmbeddedObjectRef::GetGraphicStream( BOOL bUpdate ) const
{
    DBG_ASSERT( bUpdate || mpImp->pContainer, "Can't retrieve current graphic!" );
    uno::Reference < io::XInputStream > xStream;
    if ( mpImp->pContainer && !bUpdate )
    {
        // try to get graphic stream from container storage
        xStream = mpImp->pContainer->GetGraphicStream( mxObj, &mpImp->aMediaType );
        if ( xStream.is() )
        {
            const sal_Int32 nConstBufferSize = 32000;
            SvStream *pStream = new SvMemoryStream( 32000, 32000 );
            sal_Int32 nRead=0;
            uno::Sequence < sal_Int8 > aSequence ( nConstBufferSize );
            do
            {
                nRead = xStream->readBytes ( aSequence, nConstBufferSize );
                pStream->Write( aSequence.getConstArray(), nRead );
            }
            while ( nRead == nConstBufferSize );
            pStream->Seek(0);
            return pStream;
        }
    }

    if ( !xStream.is() )
    {
        // update wanted or no stream in container storage available
        xStream = GetGraphicReplacementStream( mpImp->nViewAspect, mxObj, &mpImp->aMediaType );

        if ( xStream.is() )
        {
            if ( mpImp->pContainer )
                mpImp->pContainer->InsertGraphicStream( xStream, mpImp->aPersistName, mpImp->aMediaType );

            SvStream* pResult = ::utl::UcbStreamHelper::CreateStream( xStream );
            if ( pResult && bUpdate )
                mpImp->bNeedUpdate = sal_False;

            return pResult;
        }
    }

    return NULL;
}

void EmbeddedObjectRef::DrawPaintReplacement( const Rectangle &rRect, const String &rText, OutputDevice *pOut )
{
    MapMode aMM( MAP_APPFONT );
    Size aAppFontSz = pOut->LogicToLogic( Size( 0, 8 ), &aMM, NULL );
    Font aFnt( String::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM( "Helvetica" ) ), aAppFontSz );
    aFnt.SetTransparent( TRUE );
    aFnt.SetColor( Color( COL_LIGHTRED ) );
    aFnt.SetWeight( WEIGHT_BOLD );
    aFnt.SetFamily( FAMILY_SWISS );

    pOut->Push();
    pOut->SetBackground();
    pOut->SetFont( aFnt );

    Point aPt;
    // Nun den Text so skalieren, dass er in das Rect passt.
    // Wir fangen mit der Defaultsize an und gehen 1-AppFont runter
    for( USHORT i = 8; i > 2; i-- )
    {
        aPt.X() = (rRect.GetWidth()  - pOut->GetTextWidth( rText )) / 2;
        aPt.Y() = (rRect.GetHeight() - pOut->GetTextHeight()) / 2;

        BOOL bTiny = FALSE;
        if( aPt.X() < 0 ) bTiny = TRUE, aPt.X() = 0;
        if( aPt.Y() < 0 ) bTiny = TRUE, aPt.Y() = 0;
        if( bTiny )
        {
            // heruntergehen bei kleinen Bildern
            aFnt.SetSize( Size( 0, aAppFontSz.Height() * i / 8 ) );
            pOut->SetFont( aFnt );
        }
        else
            break;
    }

    Bitmap aBmp( SvtResId( BMP_PLUGIN ) );
    long nHeight = rRect.GetHeight() - pOut->GetTextHeight();
    long nWidth = rRect.GetWidth();
    if( nHeight > 0 )
    {
        aPt.Y() = nHeight;
        Point   aP = rRect.TopLeft();
        Size    aBmpSize = aBmp.GetSizePixel();
        // Bitmap einpassen
        if( nHeight * 10 / nWidth
          > aBmpSize.Height() * 10 / aBmpSize.Width() )
        {
            // nach der Breite ausrichten
            // Proportion beibehalten
            long nH = nWidth * aBmpSize.Height() / aBmpSize.Width();
            // zentrieren
            aP.Y() += (nHeight - nH) / 2;
            nHeight = nH;
        }
        else
        {
            // nach der H"ohe ausrichten
            // Proportion beibehalten
            long nW = nHeight * aBmpSize.Width() / aBmpSize.Height();
            // zentrieren
            aP.X() += (nWidth - nW) / 2;
            nWidth = nW;
        }

        pOut->DrawBitmap( aP, Size( nWidth, nHeight ), aBmp );
    }

    pOut->IntersectClipRegion( rRect );
    aPt += rRect.TopLeft();
    pOut->DrawText( aPt, rText );
    pOut->Pop();
}

void EmbeddedObjectRef::DrawShading( const Rectangle &rRect, OutputDevice *pOut )
{
    GDIMetaFile * pMtf = pOut->GetConnectMetaFile();
    if( pMtf && pMtf->IsRecord() )
        return;

    pOut->Push();
    pOut->SetLineColor( Color( COL_BLACK ) );

    Size aPixSize = pOut->LogicToPixel( rRect.GetSize() );
    aPixSize.Width() -= 1;
    aPixSize.Height() -= 1;
    Point aPixViewPos = pOut->LogicToPixel( rRect.TopLeft() );
    INT32 nMax = aPixSize.Width() + aPixSize.Height();
    for( INT32 i = 5; i < nMax; i += 5 )
    {
        Point a1( aPixViewPos ), a2( aPixViewPos );
        if( i > aPixSize.Width() )
            a1 += Point( aPixSize.Width(), i - aPixSize.Width() );
        else
            a1 += Point( i, 0 );
        if( i > aPixSize.Height() )
            a2 += Point( i - aPixSize.Height(), aPixSize.Height() );
        else
            a2 += Point( 0, i );

        pOut->DrawLine( pOut->PixelToLogic( a1 ), pOut->PixelToLogic( a2 ) );
    }

    pOut->Pop();

}

BOOL EmbeddedObjectRef::TryRunningState()
{
    return TryRunningState( mxObj );
}

BOOL EmbeddedObjectRef::TryRunningState( const uno::Reference < embed::XEmbeddedObject >& xEmbObj )
{
    try
    {
        if ( xEmbObj->getCurrentState() == embed::EmbedStates::LOADED )
            xEmbObj->changeState( embed::EmbedStates::RUNNING );
    }
    catch ( uno::Exception& )
    {
        return FALSE;
    }

    return TRUE;
}

void EmbeddedObjectRef::SetGraphicToContainer( const Graphic& rGraphic,
                                                comphelper::EmbeddedObjectContainer& aContainer,
                                                const ::rtl::OUString& aName,
                                                const ::rtl::OUString& aMediaType )
{
    SvMemoryStream aStream;
    aStream.SetVersion( SOFFICE_FILEFORMAT_CURRENT );
    if ( rGraphic.ExportNative( aStream ) )
    {
        aStream.Seek( 0 );

           uno::Reference < io::XInputStream > xStream = new ::utl::OSeekableInputStreamWrapper( aStream );
           aContainer.InsertGraphicStream( xStream, aName, aMediaType );
    }
    else
        OSL_ENSURE( sal_False, "Export of graphic is failed!\n" );
}

sal_Bool EmbeddedObjectRef::ObjectIsModified( const uno::Reference< embed::XEmbeddedObject >& xObj )
    throw( uno::Exception )
{
    sal_Bool bResult = sal_False;

    sal_Int32 nState = xObj->getCurrentState();
    if ( nState != embed::EmbedStates::LOADED && nState != embed::EmbedStates::RUNNING )
    {
        // the object is active so if the model is modified the replacement
        // should be retrieved from the object
        uno::Reference< util::XModifiable > xModifiable( xObj->getComponent(), uno::UNO_QUERY );
        if ( xModifiable.is() )
            bResult = xModifiable->isModified();
    }

    return bResult;
}

uno::Reference< io::XInputStream > EmbeddedObjectRef::GetGraphicReplacementStream(
                                                                sal_Int64 nViewAspect,
                                                                const uno::Reference< embed::XEmbeddedObject >& xObj,
                                                                ::rtl::OUString* pMediaType )
    throw()
{
    uno::Reference< io::XInputStream > xInStream;
    if ( xObj.is() )
    {
        try
        {
            // retrieving of the visual representation can switch object to running state
            embed::VisualRepresentation aRep = xObj->getPreferredVisualRepresentation( nViewAspect );
            if ( pMediaType )
                *pMediaType = aRep.Flavor.MimeType;

            uno::Sequence < sal_Int8 > aSeq;
            aRep.Data >>= aSeq;
            xInStream = new ::comphelper::SequenceInputStream( aSeq );
        }
        catch ( uno::Exception& )
        {
        }
    }

    return xInStream;
}

void EmbeddedObjectRef::UpdateReplacementOnDemand()
{
    DELETEZ( mpImp->pGraphic );
    mpImp->bNeedUpdate = sal_True;
}

};
