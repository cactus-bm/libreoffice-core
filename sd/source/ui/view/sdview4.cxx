/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sdview4.cxx,v $
 *
 *  $Revision: 1.30 $
 *
 *  last change: $Author: ihi $ $Date: 2006-11-14 14:46:22 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sd.hxx"

#include "View.hxx"

#ifndef _UNOTOOLS_LOCALFILEHELPER_HXX
#include <unotools/localfilehelper.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SFX_DOCFILT_HACK_HXX //autogen
#include <sfx2/docfilt.hxx>
#endif
#ifndef _SFX_FCONTNR_HXX //autogen
#include <sfx2/fcontnr.hxx>
#endif
#ifndef _SFXDOCFILE_HXX //autogen
#include <sfx2/docfile.hxx>
#endif
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SV_SOUND_HXX //autogen
#include <vcl/sound.hxx>
#endif
#ifndef _URLBMK_HXX //autogen
#include <svtools/urlbmk.hxx>
#endif
#ifndef _SVDPAGV_HXX //autogen
#include <svx/svdpagv.hxx>
#endif
#ifndef _SVX_FILLITEM_HXX //autogen
#include <svx/xfillit.hxx>
#endif
#ifndef _SVDUNDO_HXX //autogen
#include <svx/svdundo.hxx>
#endif
#ifndef _XOUTBMP_HXX //autogen
#include <svx/xoutbmp.hxx>
#endif
#ifndef _SVDOGRAF_HXX
#include <svx/svdograf.hxx>
#endif
#ifndef _SVDOMEDIA_HXX
#include <svx/svdomedia.hxx>
#endif
#ifndef _SVDOOLE2_HXX
#include <svx/svdoole2.hxx>
#endif
#ifndef _IMPGRF_HXX
#include <svx/impgrf.hxx>
#endif
#include <sot/storage.hxx>
#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif
#ifndef _AVMEDIA_MEDIAWINDOW_HXX //autogen
#include <avmedia/mediawindow.hxx>
#endif
#ifndef _EHDL_HXX
#include <svtools/ehdl.hxx>
#endif
#ifndef _SFXECODE_HXX
#include <svtools/sfxecode.hxx>
#endif

#include "app.hrc"
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif
#include "DrawDocShell.hxx"
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
#ifndef SD_FU_INSERT_FILE_HXX
#include "fuinsfil.hxx"
#endif
#include "drawdoc.hxx"
#include "sdresid.hxx"
#include "strings.hrc"
#include "imapinfo.hxx"
#include "sdpage.hxx"
#ifndef SD_SLIDE_VIEW_HXX
#include "SlideView.hxx"
#endif
#include "undo/undoobjects.hxx"

#include <comphelper/processfactory.hxx>

#ifndef _COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XEMBEDPERSIST_HPP_
#include <com/sun/star/embed/XEmbedPersist.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_ASPECTS_HPP_
#include <com/sun/star/embed/Aspects.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_NOVISUALAREASIZEEXCEPTION_HPP_
#include <com/sun/star/embed/NoVisualAreaSizeException.hpp>
#endif

#ifndef _SOERR_HXX
#include <svtools/soerr.hxx>
#endif

#include <sfx2/ipclient.hxx>

using namespace com::sun::star;

namespace sd {

#ifdef WNT
#pragma optimize ( "", off )
#endif

/*************************************************************************
|*
|* Graphik einfuegen
|* Wird ein leeres Graphikobjekt uebergeben, so wird dieses gefuellt.
|* Andernfalls wird ein an der gegebenen Position vorhandenes Objekt
|* gefuellt. Ist an der Position kein Objekt vorhanden, so wird ein neues
|* Objekt erzeugt und ein Pointer auf dieses Objekt zurueckgegeben.
|*
\************************************************************************/

SdrGrafObj* View::InsertGraphic( const Graphic& rGraphic, sal_Int8& rAction,
                                   const Point& rPos, SdrObject* pObj, ImageMap* pImageMap )
{
    SdrEndTextEdit();
    nAction = rAction;

    // Liegt ein Objekt an der Position rPos?
    SdrGrafObj*     pNewGrafObj = NULL;
    SdrPageView*    pPV = GetSdrPageView();
    SdrObject*      pPickObj = pObj;

    if(pPV && this->ISA(SlideView))
    {
        if(!pPV->GetPageRect().IsInside(rPos))
            pPV = 0L;
    }

    if( !pPickObj && pPV )
    {
        SdrPageView* pPageView = pPV;
        PickObj(rPos, pPickObj, pPageView);
    }

    if( nAction == DND_ACTION_LINK && pPickObj && pPV )
    {
        if( pPickObj->ISA( SdrGrafObj ) )
        {
            // Das Objekt wird mit der Bitmap gefuellt
            pNewGrafObj = (SdrGrafObj*) pPickObj->Clone();
            pNewGrafObj->SetGraphic(rGraphic);

            if ( pNewGrafObj->IsEmptyPresObj() )
            {
                Rectangle aRect( pNewGrafObj->GetLogicRect() );
                pNewGrafObj->AdjustToMaxRect( aRect, FALSE );
                pNewGrafObj->SetOutlinerParaObject(NULL);
                pNewGrafObj->SetEmptyPresObj(FALSE);
            }

            BegUndo(String(SdResId(STR_UNDO_DRAGDROP)));

            SdPage* pPage = (SdPage*) pPickObj->GetPage();

            if (pPage && pPage->GetPresObjKind(pPickObj) == PRESOBJ_GRAPHIC)
            {
                // Neues PresObj in die Liste eintragen
                pNewGrafObj->SetUserCall(pPickObj->GetUserCall());
                AddUndo( new sd::UndoObjectPresentationKind( *pPickObj ) );
                AddUndo( new sd::UndoObjectPresentationKind( *pNewGrafObj ) );
                pPage->RemovePresObj(pPickObj);
                pPage->InsertPresObj(pNewGrafObj, PRESOBJ_GRAPHIC);
            }

            if (pImageMap)
                pNewGrafObj->InsertUserData(new SdIMapInfo(*pImageMap));

            ReplaceObjectAtView(pPickObj, *pPV, pNewGrafObj); // maybe ReplaceObjectAtView
            EndUndo();
        }
        else if (pPickObj->IsClosedObj() && !pPickObj->ISA(SdrOle2Obj))
        {
            /******************************************************************
            * Das Objekt wird mit der Graphik gefuellt
            ******************************************************************/
            BegUndo(String(SdResId(STR_UNDO_DRAGDROP)));
            AddUndo(GetModel()->GetSdrUndoFactory().CreateUndoAttrObject(*pPickObj));
            EndUndo();

            XOBitmap aXOBitmap( rGraphic.GetBitmap() );
            SfxItemSet aSet(pDocSh->GetPool(), XATTR_FILLSTYLE, XATTR_FILLBITMAP);
            aSet.Put(XFillStyleItem(XFILL_BITMAP));
            aSet.Put(XFillBitmapItem(&pDocSh->GetPool(), aXOBitmap));
            pPickObj->SetMergedItemSetAndBroadcast(aSet);
        }
    }
    else if ( pPV )
    {
        // create  new object
        Size aSize;

        if ( rGraphic.GetPrefMapMode().GetMapUnit() == MAP_PIXEL )
        {
            ::OutputDevice* pOutDev = 0;
            if( pViewSh )
                pOutDev = pViewSh->GetActiveWindow();

            if( !pOutDev )
                pOutDev = Application::GetDefaultDevice();

            if( pOutDev )
                aSize = pOutDev->PixelToLogic( rGraphic.GetPrefSize(), MAP_100TH_MM );
        }
        else
        {
            aSize = OutputDevice::LogicToLogic( rGraphic.GetPrefSize(),
                                                rGraphic.GetPrefMapMode(),
                                                MapMode( MAP_100TH_MM ) );
        }

        pNewGrafObj = new SdrGrafObj( rGraphic, Rectangle( rPos, aSize ) );
        SdrPage* pPage = pPV->GetPage();
        Size aPageSize( pPage->GetSize() );
        aPageSize.Width()  -= pPage->GetLftBorder() + pPage->GetRgtBorder();
        aPageSize.Height() -= pPage->GetUppBorder() + pPage->GetLwrBorder();
        pNewGrafObj->AdjustToMaxRect( Rectangle( Point(), aPageSize ), TRUE );
//      pNewGrafObj->AdjustToMaxRect( Rectangle( pPV->GetOffset(), aPageSize ), TRUE );

        ULONG   nOptions = SDRINSERT_SETDEFLAYER;
        BOOL    bIsPresTarget = FALSE;

        if ((pViewSh
                && pViewSh->GetViewShell()!=NULL
                && pViewSh->GetViewShell()->GetIPClient()
                && pViewSh->GetViewShell()->GetIPClient()->IsObjectInPlaceActive())
            || this->ISA(SlideView))
            nOptions |= SDRINSERT_DONTMARK;

        if( ( nAction & DND_ACTION_MOVE ) && pPickObj && (pPickObj->IsEmptyPresObj() || pPickObj->GetUserCall()) )
        {
            SdPage* pPage = (SdPage*) pPickObj->GetPage();

            if ( pPage && pPage->IsMasterPage() )
                bIsPresTarget = pPage->IsPresObj(pPickObj);
        }

        if( ( nAction & DND_ACTION_MOVE ) && pPickObj && !bIsPresTarget )
        {
            // replace object
            if (pImageMap)
                pNewGrafObj->InsertUserData(new SdIMapInfo(*pImageMap));

            Rectangle aPickObjRect(pPickObj->GetCurrentBoundRect());
            Size aPickObjSize(aPickObjRect.GetSize());
            Rectangle aObjRect(pNewGrafObj->GetCurrentBoundRect());
            Size aObjSize(aObjRect.GetSize());

            Fraction aScaleWidth(aPickObjSize.Width(), aObjSize.Width());
            Fraction aScaleHeight(aPickObjSize.Height(), aObjSize.Height());
            pNewGrafObj->NbcResize(aObjRect.TopLeft(), aScaleWidth, aScaleHeight);

            Point aVec = aPickObjRect.TopLeft() - aObjRect.TopLeft();
            pNewGrafObj->NbcMove(Size(aVec.X(), aVec.Y()));

            BegUndo(String(SdResId(STR_UNDO_DRAGDROP)));
            pNewGrafObj->NbcSetLayer(pPickObj->GetLayer());
            SdrPage* pPage = pPV->GetPage();
            pPage->InsertObject(pNewGrafObj);
            AddUndo(pDoc->GetSdrUndoFactory().CreateUndoNewObject(*pNewGrafObj));
            AddUndo(pDoc->GetSdrUndoFactory().CreateUndoDeleteObject(*pPickObj));
            pPage->RemoveObject(pPickObj->GetOrdNum());
            EndUndo();
            nAction = DND_ACTION_COPY;
        }
        else
        {
            InsertObjectAtView(pNewGrafObj, *pPV, nOptions);

            if( pImageMap )
                pNewGrafObj->InsertUserData(new SdIMapInfo(*pImageMap));
        }
    }

    rAction = nAction;

    return pNewGrafObj;
}

// -----------------------------------------------------------------------------

SdrMediaObj* View::InsertMediaURL( const rtl::OUString& rMediaURL, sal_Int8& rAction,
                                   const Point& rPos, const Size& rSize )
{
    SdrEndTextEdit();
    nAction = rAction;

    SdrMediaObj*    pNewMediaObj = NULL;
    SdrPageView*    pPV = GetSdrPageView();
    SdrObject*      pPickObj = NULL;

    if(pPV && this->ISA( SlideView ))
    {
        if(!pPV->GetPageRect().IsInside(rPos))
            pPV = 0L;
    }

    if( !pPickObj && pPV )
    {
        SdrPageView* pPageView = pPV;
        PickObj(rPos, pPickObj, pPageView);
    }

    if( nAction == DND_ACTION_LINK && pPickObj && pPV && pPickObj->ISA( SdrMediaObj ) )
    {
        pNewMediaObj = static_cast< SdrMediaObj* >( pPickObj->Clone() );
        pNewMediaObj->setURL( rMediaURL );

        BegUndo(String(SdResId(STR_UNDO_DRAGDROP)));
        ReplaceObjectAtView(pPickObj, *pPV, pNewMediaObj);
        EndUndo();
    }
    else if( pPV )
    {
        pNewMediaObj = new SdrMediaObj( Rectangle( rPos, rSize ) );

        if( pPV && InsertObjectAtView( pNewMediaObj, *pPV, SDRINSERT_SETDEFLAYER ) )
            pNewMediaObj->setURL( rMediaURL );
    }

    rAction = nAction;

    return pNewMediaObj;
}

/*************************************************************************
|*
|* Timer-Handler fuer InsertFile beim Drop()
|*
\************************************************************************/

IMPL_LINK( View, DropInsertFileHdl, Timer*, pTimer )
{
    DBG_ASSERT( pViewSh, "sd::View::DropInsertFileHdl(), I need a view shell to work!" );
    if( !pViewSh )
        return 0;

    SfxErrorContext aEc( ERRCTX_ERROR, pViewSh->GetActiveWindow(), RID_SO_ERRCTX );
    ErrCode nError = 0;

    ::std::vector< String >::const_iterator aIter( aDropFileVector.begin() );

    while( (aIter != aDropFileVector.end()) && !nError )
    {
        String          aCurrentDropFile( *aIter );
        INetURLObject   aURL( aCurrentDropFile );
        BOOL            bOK = FALSE;

        if( aURL.GetProtocol() == INET_PROT_NOT_VALID )
        {
            String aURLStr;
            ::utl::LocalFileHelper::ConvertPhysicalNameToURL( aCurrentDropFile, aURLStr );
            aURL = INetURLObject( aURLStr );
        }

        GraphicFilter*  pGraphicFilter = GetGrfFilter();
        Graphic         aGraphic;

        aCurrentDropFile = aURL.GetMainURL( INetURLObject::NO_DECODE );

        if( !::avmedia::MediaWindow::isMediaURL( aCurrentDropFile ) )
        {
            if( !pGraphicFilter->ImportGraphic( aGraphic, aURL ) )
            {
                sal_Int8    nTempAction = ( aIter == aDropFileVector.begin() ) ? nAction : 0;
                SdrGrafObj* pGrafObj = InsertGraphic( aGraphic, nTempAction, aDropPos, NULL, NULL );

                if( pGrafObj )
                    pGrafObj->SetGraphicLink( aCurrentDropFile, String() );

                // return action from first inserted graphic
                if( aIter == aDropFileVector.begin() )
                    nAction = nTempAction;

                bOK = TRUE;
            }
            if( !bOK )
            {
                const SfxFilter*        pFoundFilter = NULL;
                SfxMedium               aSfxMedium( aCurrentDropFile, STREAM_READ | STREAM_SHARE_DENYNONE, FALSE );
                ErrCode                 nErr = SFX_APP()->GetFilterMatcher().GuessFilter(  aSfxMedium, &pFoundFilter, SFX_FILTER_IMPORT, SFX_FILTER_NOTINSTALLED | SFX_FILTER_EXECUTABLE );

                if( pFoundFilter && !nErr )
                {
                    ::std::vector< String > aFilterVector;
                    const String            aFilterName( pFoundFilter->GetFilterName() );
                    String                  aLowerAsciiFileName( aCurrentDropFile );
                    aLowerAsciiFileName.ToLowerAscii();

                    FuInsertFile::GetSupportedFilterVector( aFilterVector );

                    if( ( ::std::find( aFilterVector.begin(), aFilterVector.end(), pFoundFilter->GetMimeType() ) != aFilterVector.end() ) ||
                        aFilterName.SearchAscii( "Text" ) != STRING_NOTFOUND ||
                        aFilterName.SearchAscii( "Rich" ) != STRING_NOTFOUND ||
                        aFilterName.SearchAscii( "RTF" ) != STRING_NOTFOUND ||
                        aFilterName.SearchAscii( "HTML" ) != STRING_NOTFOUND ||
                        aLowerAsciiFileName.SearchAscii(".sdd") != STRING_NOTFOUND ||
                        aLowerAsciiFileName.SearchAscii(".sda") != STRING_NOTFOUND ||
                        aLowerAsciiFileName.SearchAscii(".sxd") != STRING_NOTFOUND ||
                        aLowerAsciiFileName.SearchAscii(".sxi") != STRING_NOTFOUND ||
                        aLowerAsciiFileName.SearchAscii(".std") != STRING_NOTFOUND ||
                        aLowerAsciiFileName.SearchAscii(".sti") != STRING_NOTFOUND )
                    {
                        ::sd::Window* pWin = pViewSh->GetActiveWindow();
                        SfxRequest      aReq(SID_INSERTFILE, 0, pDoc->GetItemPool());
                        SfxStringItem   aItem1( ID_VAL_DUMMY0, aCurrentDropFile ), aItem2( ID_VAL_DUMMY1, pFoundFilter->GetFilterName() );

                        aReq.AppendItem( aItem1 );
                        aReq.AppendItem( aItem2 );
                        FuInsertFile::Create( pViewSh, pWin, this, pDoc, aReq );
                        bOK = TRUE;
                    }
                }
            }
        }

        if( !bOK )
        {
            Size aPrefSize;

            if( ::avmedia::MediaWindow::isMediaURL( aCurrentDropFile ) &&
                ::avmedia::MediaWindow::isMediaURL( aCurrentDropFile, true, &aPrefSize ) )
            {
                if( aPrefSize.Width() && aPrefSize.Height() )
                {
                    ::sd::Window* pWin = pViewSh->GetActiveWindow();

                    if( pWin )
                        aPrefSize = pWin->PixelToLogic( aPrefSize, MAP_100TH_MM );
                    else
                        aPrefSize = Application::GetDefaultDevice()->PixelToLogic( aPrefSize, MAP_100TH_MM );
                }
                else
                    aPrefSize  = Size( 5000, 5000 );

                InsertMediaURL( aCurrentDropFile, nAction, aDropPos, aPrefSize ) ;
            }
            else if( nAction & DND_ACTION_LINK )
                static_cast< DrawViewShell* >( pViewSh )->InsertURLButton( aCurrentDropFile, aCurrentDropFile, String(), &aDropPos );
            else
            {
                if( pViewSh )
                {
                    try
                    {
                        //TODO/MBA: testing
                        ::rtl::OUString aName;
                        uno::Sequence < beans::PropertyValue > aMedium(1);
                        aMedium[0].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "URL" ) );
                        aMedium[0].Value <<= ::rtl::OUString( aCurrentDropFile );

                        uno::Reference < embed::XEmbeddedObject > xObj = pDocSh->GetEmbeddedObjectContainer().
                                InsertEmbeddedObject( aMedium, aName );

                        uno::Reference < embed::XEmbedPersist > xPersist( xObj, uno::UNO_QUERY );
                        if ( xPersist.is())
                        {
                            // TODO/LEAN: VisualArea access can switch the object to running state
                            sal_Int64 nAspect = embed::Aspects::MSOLE_CONTENT;

                            xPersist->storeOwn();

                            awt::Size aSz;
                            try
                            {
                                aSz = xObj->getVisualAreaSize( nAspect );
                            }
                            catch( embed::NoVisualAreaSizeException& )
                            {
                                // the default size will be set later
                            }

                            Size        aSize( aSz.Width, aSz.Height );
                            Rectangle   aRect;

                            if (!aSize.Width() || !aSize.Height())
                            {
                                aSize.Width()   = 1410;
                                aSize.Height()  = 1000;
                            }

                            aRect = Rectangle( aDropPos, aSize );

                            SdrOle2Obj* pOleObj = new SdrOle2Obj( svt::EmbeddedObjectRef( xObj, nAspect ), aName, aRect );
                            ULONG       nOptions = SDRINSERT_SETDEFLAYER;

                            if (pViewSh != NULL)
                            {
                                OSL_ASSERT (pViewSh->GetViewShell()!=NULL);
                                SfxInPlaceClient* pIpClient =
                                    pViewSh->GetViewShell()->GetIPClient();
                                if (pIpClient!=NULL && pIpClient->IsObjectInPlaceActive())
                                    nOptions |= SDRINSERT_DONTMARK;
                            }

                            InsertObjectAtView( pOleObj, *GetSdrPageView(), nOptions );
                            pOleObj->SetLogicRect( aRect );
                            aSz.Width = aRect.GetWidth();
                            aSz.Height = aRect.GetHeight();
                            xObj->setVisualAreaSize( nAspect,aSz );
                        }
                    }
                    catch( uno::Exception& )
                    {
                        nError = ERRCODE_IO_GENERAL;
                        // TODO/LATER: better error handling
                    }
                }
            }
        }

        ++aIter;
    }

    if( nError )
        ErrorHandler::HandleError( nError );

    return nError;
}

/*************************************************************************
|*
|* Timer-Handler fuer Errorhandling beim Drop()
|*
\************************************************************************/

IMPL_LINK( View, DropErrorHdl, Timer*, pTimer )
{
    InfoBox( pViewSh ? pViewSh->GetActiveWindow() : 0, String(SdResId(STR_ACTION_NOTPOSSIBLE) ) ).Execute();
    return 0;
}

#ifdef WNT
#pragma optimize ( "", on )
#endif

/*************************************************************************
|*
|* Redraw sperren oder erlauben
|*
\************************************************************************/

void View::LockRedraw(BOOL bLock)
{
    if (bLock)
    {
        nLockRedrawSmph++;
        DBG_ASSERT(nLockRedrawSmph, "Ueberlauf im LockRedraw");
    }
    else
    {
        DBG_ASSERT(nLockRedrawSmph, "Unterlauf im LockRedraw");
        nLockRedrawSmph--;

        // alle gespeicherten Redraws ausfuehren
        if (!nLockRedrawSmph)
        {
            while (pLockedRedraws && pLockedRedraws->Count())
            {
                SdViewRedrawRec* pRec = (SdViewRedrawRec*)pLockedRedraws->First();
                OutputDevice* pCurrentOut = pRec->pOut;
                Rectangle aBoundRect(pRec->aRect);
                pLockedRedraws->Remove(pRec);
                delete pRec;

                pRec = (SdViewRedrawRec*)pLockedRedraws->First();
                while (pRec)
                {
                    if (pRec->pOut == pCurrentOut)
                    {
                        aBoundRect.Union(pRec->aRect);
                        pLockedRedraws->Remove(pRec);
                        delete pRec;
                        pRec = (SdViewRedrawRec*)pLockedRedraws->GetCurObject();
                    }
                    else
                    {
                        pRec = (SdViewRedrawRec*)pLockedRedraws->Next();
                    }
                }

                CompleteRedraw(pCurrentOut, Region(aBoundRect));
            }
            delete pLockedRedraws;
            pLockedRedraws = NULL;
        }
    }
}




bool View::IsRedrawLocked (void) const
{
    return nLockRedrawSmph>0;
}

/*************************************************************************
|*
|* StyleSheet aus der Sleketion besorgen
|*
\************************************************************************/

SfxStyleSheet* View::GetStyleSheet() const
{
    return SdrView::GetStyleSheet();
}

} // end of namespace sd
