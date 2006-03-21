/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SdUnoDrawView.cxx,v $
 *
 *  $Revision: 1.28 $
 *
 *  last change: $Author: obo $ $Date: 2006-03-21 17:34:06 $
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

#include "SdUnoDrawView.hxx"

#include "DrawController.hxx"
#ifndef SD_DRAW_DOC_SHELL_HXX
#include "DrawDocShell.hxx"
#endif
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
#include "drawdoc.hxx"
#include "unolayer.hxx"
#include "unomodel.hxx"
#include "unopage.hxx"
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif

#include <cppuhelper/proptypehlp.hxx>
#ifndef _SFXDISPATCH_HXX
#include <sfx2/dispatch.hxx>
#endif
#include <sfx2/viewfrm.hxx>
#include <svx/svdpagv.hxx>
#ifndef _SVX_UNOSHAPE_HXX
#include <svx/unoshape.hxx>
#endif
#include <svx/unoshcol.hxx>
#ifndef _SVX_ZOOMITEM_HXX
#include <svx/zoomitem.hxx>
#endif

#ifndef _COM_SUN_STAR_DRAWING_XLAYERMANAGER_HPP_
#include <com/sun/star/drawing/XLayerManager.hpp>
#endif
#ifndef _COM_SUN_STAR_VIEW_DOCUMENTZOOMTYPE_HPP_
#include <com/sun/star/view/DocumentZoomType.hpp>
#endif

#include <vector>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;


namespace sd {

SdUnoDrawView::SdUnoDrawView(
    DrawController& rController,
    DrawViewShell& rViewShell,
    View& rView) throw()
    :   DrawSubController(),
        mrController(rController),
        mrDrawViewShell(rViewShell),
        mrView(rView)
{
}




SdUnoDrawView::~SdUnoDrawView() throw()
{
}




sal_Bool SdUnoDrawView::getMasterPageMode(void) const throw()
{
    return (mrDrawViewShell.GetEditMode() == EM_MASTERPAGE);
}




void SdUnoDrawView::setMasterPageMode (sal_Bool bMasterPageMode) throw()
{
    if ((mrDrawViewShell.GetEditMode() == EM_MASTERPAGE) != bMasterPageMode)
    {
        mrDrawViewShell.ChangeEditMode (
            bMasterPageMode ? EM_MASTERPAGE : EM_PAGE,
            mrDrawViewShell.IsLayerModeActive());
    }
}




sal_Bool SdUnoDrawView::getLayerMode(void) const throw()
{
    return mrDrawViewShell.IsLayerModeActive();
}




void SdUnoDrawView::setLayerMode (sal_Bool bLayerMode) throw()
{
    if (mrDrawViewShell.IsLayerModeActive() != (bLayerMode==sal_True))
    {
        mrDrawViewShell.ChangeEditMode (
            mrDrawViewShell.GetEditMode(),
            bLayerMode);
    }
}




Reference<drawing::XLayer> SdUnoDrawView::getActiveLayer (void) throw ()
{
    Reference<drawing::XLayer> xCurrentLayer;

    do
    {
        // Retrieve the layer manager from the model.
        SdXImpressDocument* pModel = GetModel();
        if (pModel == NULL)
            break;

        SdDrawDocument* pSdModel = pModel->GetDoc();
        if (pSdModel == NULL)
            break;

        // From the model get the current SdrLayer object via the layer admin.
        SdrLayerAdmin& rLayerAdmin = pSdModel->GetLayerAdmin ();
        SdrLayer* pLayer = rLayerAdmin.GetLayer (mrView.GetActiveLayer(), TRUE);
        if (pLayer == NULL)
            break;

        // Get the corresponding XLayer object from the implementation
        // object of the layer manager.
        Reference<drawing::XLayerManager> xManager (pModel->getLayerManager(), uno::UNO_QUERY);
        SdLayerManager* pManager = SdLayerManager::getImplementation (xManager);
        if (pManager != NULL)
            xCurrentLayer = pManager->GetLayer (pLayer);
    }
    while (false);

    return xCurrentLayer;
}




void SdUnoDrawView::setActiveLayer (const Reference<drawing::XLayer>& rxLayer) throw ()
{
    do
    {
        // Get the SdrLayer object corresponding to the given reference.
        if ( ! rxLayer.is())
            break;

        SdLayer* pLayer = SdLayer::getImplementation (rxLayer);
        if (pLayer == NULL)
            break;

        SdrLayer* pSdrLayer = pLayer->GetSdrLayer();
        if (pSdrLayer == NULL)
            break;

        // Set the new active layer and make the change visible.
        mrView.SetActiveLayer (pSdrLayer->GetName());
        mrDrawViewShell.ResetActualLayer ();
    }
    while (false);
}




// XSelectionSupplier


sal_Bool SAL_CALL SdUnoDrawView::select( const Any& aSelection )
    throw(lang::IllegalArgumentException, RuntimeException)
{
    bool bOk = true;

    ::std::vector<SdrObject*> aObjects;

    SdrPage* pSdrPage = NULL;

    Reference< drawing::XShape > xShape;
    aSelection >>= xShape;

    if(xShape.is())
    {
        SvxShape* pShape = SvxShape::getImplementation( xShape );
        if( pShape && (pShape->GetSdrObject() != NULL) )
        {
            SdrObject* pObj = pShape->GetSdrObject();
            pSdrPage = pObj->GetPage();
            aObjects.push_back( pObj );
        }
        else
        {
            bOk = false;
        }
    }
    else
    {
        Reference< drawing::XShapes > xShapes;
        aSelection >>= xShapes;
        if( xShapes.is() )
        {
            const sal_uInt32 nCount = xShapes->getCount();
            for( sal_uInt32 i = 0; i < nCount; i++ )
            {
                xShapes->getByIndex(i) >>= xShape;
                if( xShape.is() )
                {
                    SvxShape* pShape = SvxShape::getImplementation(xShape);
                    if( (pShape == NULL) || (pShape->GetSdrObject() == NULL) )
                    {
                        bOk = false;
                        break;
                    }

                    SdrObject* pObj = pShape->GetSdrObject();

                    if( pSdrPage == NULL )
                    {
                        pSdrPage = pObj->GetPage();
                    }
                    else if( pSdrPage != pObj->GetPage() )
                    {
                        bOk = false;
                        break;
                    }

                    aObjects.push_back( pObj );
                }
            }
        }
    }

    if( bOk )
    {
        if( pSdrPage )
        {
            setMasterPageMode( pSdrPage->IsMasterPage() );
            mrDrawViewShell.SwitchPage( (pSdrPage->GetPageNum() - 1) >> 1 );
            mrDrawViewShell.WriteFrameViewData();
        }

        SdrPageView *pPV = mrView.GetPageViewPvNum(0);

        if(pPV)
        {
            // first deselect all
            mrView.UnmarkAllObj( pPV );

            ::std::vector<SdrObject*>::iterator aIter( aObjects.begin() );
            const ::std::vector<SdrObject*>::iterator aEnd( aObjects.end() );
            while( aIter != aEnd )
            {
                SdrObject* pObj = (*aIter++);
                mrView.MarkObj( pObj, pPV );
            }
        }
        else
        {
            bOk = false;
        }
    }

    return bOk;
}

//----------------------------------------------------------------------

Any SAL_CALL SdUnoDrawView::getSelection()
    throw(RuntimeException)
{
    Any aAny;

    if( mrView.IsTextEdit() )
        mrView.getTextSelection( aAny );


    if( !aAny.hasValue() )
    {
        const SdrMarkList& rMarkList = mrView.GetMarkedObjectList();
        sal_uInt32 nCount = rMarkList.GetMarkCount();
        if( nCount )
        {
            Reference< drawing::XShapes > xShapes( SvxShapeCollection_NewInstance(), UNO_QUERY );
            for( sal_uInt32 nNum = 0; nNum < nCount; nNum++)
            {
                SdrMark *pMark = rMarkList.GetMark(nNum);
                if(pMark==NULL)
                    continue;

                SdrObject *pObj = pMark->GetObj();
                if(pObj==NULL || pObj->GetPage() == NULL)
                    continue;

                Reference< drawing::XDrawPage > xPage( pObj->GetPage()->getUnoPage(), UNO_QUERY);

                if(!xPage.is())
                    continue;

                SvxDrawPage* pDrawPage = SvxDrawPage::getImplementation( xPage );

                if(pDrawPage==NULL)
                    continue;

                Reference< drawing::XShape > xShape( pObj->getUnoShape(), UNO_QUERY );

                if(xShape.is())
                    xShapes->add(xShape);
            }
            aAny <<= xShapes;
        }
    }

    return aAny;
}


//----------------------------------------------------------------------
//------ XPropertySet & OPropertySetHelper -----------------------------
//----------------------------------------------------------------------


// Return sal_True, value change
sal_Bool SdUnoDrawView::convertFastPropertyValue
(
    Any & rConvertedValue,
    Any & rOldValue,
    sal_Int32 nHandle,
    const Any& rValue
) throw ( com::sun::star::lang::IllegalArgumentException)
{
    sal_Bool bResult = sal_False;

    switch( nHandle )
    {
        case DrawController::PROPERTY_CURRENTPAGE:
            {
                Reference< drawing::XDrawPage > xOldPage( getCurrentPage() );
                Reference< drawing::XDrawPage > xNewPage;
                ::cppu::convertPropertyValue( xNewPage, rValue );
                if( xOldPage != xNewPage )
                {
                    rConvertedValue <<= xNewPage;
                    rOldValue <<= xOldPage;
                    bResult = sal_True;
                }
            }
            break;

        case DrawController::PROPERTY_MASTERPAGEMODE:
            {
                sal_Bool bOldValue = getMasterPageMode();
                sal_Bool b;
                ::cppu::convertPropertyValue( b , rValue );
                if( b != bOldValue )
                {

                    rConvertedValue.setValue( &b , ::getCppuBooleanType()  );
                    rOldValue.setValue( & bOldValue , ::getCppuBooleanType() );
                    bResult = sal_True;
                }
            }
            break;

        case DrawController::PROPERTY_LAYERMODE:
            {
                sal_Bool bOldValue = getLayerMode();
                sal_Bool b;
                ::cppu::convertPropertyValue( b , rValue );
                if( b != bOldValue )
                {
                    rConvertedValue.setValue( &b , ::getCppuBooleanType()  );
                    rOldValue.setValue( & bOldValue , ::getCppuBooleanType() );
                    bResult = sal_True;
                }
            }
            break;

        case DrawController::PROPERTY_ACTIVE_LAYER:
            {
                Reference<drawing::XLayer> xOldLayer (getActiveLayer());
                Reference<drawing::XLayer> xNewLayer;
                ::cppu::convertPropertyValue (xNewLayer, rValue);
                if (xOldLayer != xNewLayer)
                {
                    rConvertedValue <<= xNewLayer;
                    rOldValue <<= xOldLayer;
                    bResult = sal_True;
                }
            }
            break;

        case DrawController::PROPERTY_ZOOMVALUE:
            {
                sal_Int16 nOldZoom = GetZoom();
                sal_Int16 nNewZoom;
                ::cppu::convertPropertyValue( nNewZoom, rValue );
                if( nNewZoom != nOldZoom )
                {
                    rConvertedValue <<= nNewZoom;
                    rOldValue <<= nOldZoom;
                    bResult = sal_True;
                }
            }
            break;

        case DrawController::PROPERTY_ZOOMTYPE:
            {
                sal_Int16 nOldType = com::sun::star::view::DocumentZoomType::BY_VALUE;
                sal_Int16 nNewType;
                ::cppu::convertPropertyValue( nNewType, rValue );
                if( nNewType != nOldType )
                {
                    rConvertedValue <<= nNewType;
                    rOldValue <<= nOldType;
                    bResult = sal_True;
                }
            }
            break;

        case DrawController::PROPERTY_VIEWOFFSET:
            {
                awt::Point aOld( GetViewOffset() );
                awt::Point aNew;
                ::cppu::convertPropertyValue( aNew, rValue );
                if( (aOld.X != aNew.X) && (aOld.Y != aNew.Y) )
                {
                    rConvertedValue <<= aNew;
                    rOldValue <<= aOld;
                    bResult = sal_True;
                }
            }
            break;

        default:
            bResult = sal_False;
            break;
    }

    return bResult;
}

//----------------------------------------------------------------------

/**
 * only set the value.
 */
void SdUnoDrawView::setFastPropertyValue_NoBroadcast
(
    sal_Int32 nHandle,
    const Any& rValue
) throw ( com::sun::star::uno::Exception)
{
    switch( nHandle )
    {
        case DrawController::PROPERTY_CURRENTPAGE:
            {
                Reference< drawing::XDrawPage > xPage;
                rValue >>= xPage;
                setCurrentPage( xPage );
            }
            break;

        case DrawController::PROPERTY_MASTERPAGEMODE:
            {
                sal_Bool bValue;
                rValue >>= bValue;
                setMasterPageMode( bValue );
            }
            break;

        case DrawController::PROPERTY_LAYERMODE:
            {
                sal_Bool bValue;
                rValue >>= bValue;
                setLayerMode( bValue );
            }

        case DrawController::PROPERTY_ACTIVE_LAYER:
            {
                Reference<drawing::XLayer> xLayer;
                rValue >>= xLayer;
                setActiveLayer (xLayer);
            }
            break;
        case DrawController::PROPERTY_ZOOMVALUE:
            {
                sal_Int16 nZoom;
                rValue >>= nZoom;
                SetZoom( nZoom );
            }
            break;
        case DrawController::PROPERTY_ZOOMTYPE:
            {
                sal_Int16 nType;
                rValue >>= nType;
                SetZoomType( nType );
            }
            break;
        case DrawController::PROPERTY_VIEWOFFSET:
            {
                awt::Point aOffset;
                rValue >>= aOffset;
                SetViewOffset( aOffset );
            }
            break;

        default:
            break;
    }
}

//----------------------------------------------------------------------

void SdUnoDrawView::getFastPropertyValue( Any & rRet, sal_Int32 nHandle ) const
{
    switch( nHandle )
    {
        case DrawController::PROPERTY_CURRENTPAGE:
            rRet <<= (const_cast<SdUnoDrawView*>(this))->getCurrentPage();
            break;

        case DrawController::PROPERTY_MASTERPAGEMODE:
            rRet <<= getMasterPageMode();
            break;

        case DrawController::PROPERTY_LAYERMODE:
            rRet <<= getLayerMode();
            break;

        case DrawController::PROPERTY_ACTIVE_LAYER:
            rRet <<= (const_cast<SdUnoDrawView*>(this))->getActiveLayer();
            break;

        case DrawController::PROPERTY_ZOOMVALUE:
            rRet <<= GetZoom();
            break;
        case DrawController::PROPERTY_ZOOMTYPE:
            rRet <<= (sal_Int16)com::sun::star::view::DocumentZoomType::BY_VALUE;
            break;
        case DrawController::PROPERTY_VIEWOFFSET:
            rRet <<= GetViewOffset();
            break;

        default:
            break;
    }
}




// XDrawView


void SAL_CALL SdUnoDrawView::setCurrentPage (
    const Reference< drawing::XDrawPage >& xPage )
    throw(RuntimeException)
{
    SvxDrawPage* pDrawPage = SvxDrawPage::getImplementation( xPage );
    SdrPage *pSdrPage = pDrawPage ? pDrawPage->GetSdrPage() : NULL;

    if(pSdrPage)
    {
        // End editing of text.  Otherwise the edited text object would
        // still be visible on the new page.
        mrDrawViewShell.GetView()->EndTextEdit();

        setMasterPageMode( pSdrPage->IsMasterPage() );
        mrDrawViewShell.SwitchPage( (pSdrPage->GetPageNum() - 1) >> 1 );
        mrDrawViewShell.WriteFrameViewData();
    }
}

//----------------------------------------------------------------------

Reference< drawing::XDrawPage > SAL_CALL SdUnoDrawView::getCurrentPage()
    throw(RuntimeException)
{
    Reference< drawing::XDrawPage >  xPage;

    SdXImpressDocument* pModel = GetModel();
    SdrPageView *pPV = mrView.GetPageViewPvNum(0);
    SdrPage* pPage = pPV ? pPV->GetPage() : NULL;

    if(pPage)
        xPage = Reference< drawing::XDrawPage >::query( pPage->getUnoPage() );

    return xPage;
}


sal_Int16 SdUnoDrawView::GetZoom(void) const
{
    if (mrDrawViewShell.GetActiveWindow() )
    {
        return (sal_Int16)mrDrawViewShell.GetActiveWindow()->GetZoom();
    }
    else
    {
        return 0;
    }
}

void SdUnoDrawView::SetZoom( sal_Int16 nZoom )
{
    SvxZoomItem aZoomItem( SVX_ZOOM_PERCENT, nZoom );

    SfxViewFrame* pViewFrame = mrDrawViewShell.GetViewFrame();
    if( pViewFrame )
    {
        SfxDispatcher* pDispatcher = pViewFrame->GetDispatcher();
        if( pDispatcher )
        {
            pDispatcher->Execute(SID_ATTR_ZOOM,SFX_CALLMODE_SYNCHRON,&aZoomItem, 0L);
        }
    }
}


void SdUnoDrawView::SetViewOffset(const awt::Point& rWinPos )
{
    Point aWinPos( rWinPos.X, rWinPos.Y );
    aWinPos += mrDrawViewShell.GetViewOrigin();
    mrDrawViewShell.SetWinViewPos( aWinPos, true );
}

awt::Point SdUnoDrawView::GetViewOffset() const
{
    Point aRet;

    aRet = mrDrawViewShell.GetWinViewPos();
    aRet -= mrDrawViewShell.GetViewOrigin();

    return awt::Point( aRet.X(), aRet.Y() );
}

void SdUnoDrawView::SetZoomType ( sal_Int16 nType )
{
    SfxViewFrame* pViewFrame = mrDrawViewShell.GetViewFrame();
    if( pViewFrame )
    {
        SfxDispatcher* pDispatcher = pViewFrame->GetDispatcher();
        if( pDispatcher )
        {
            SvxZoomType eZoomType;
            switch( nType )
            {
                case com::sun::star::view::DocumentZoomType::OPTIMAL:
                    eZoomType = SVX_ZOOM_OPTIMAL;
                    break;

                case com::sun::star::view::DocumentZoomType::PAGE_WIDTH:
                case com::sun::star::view::DocumentZoomType::PAGE_WIDTH_EXACT:
                    eZoomType = SVX_ZOOM_PAGEWIDTH;
                    break;

                case com::sun::star::view::DocumentZoomType::ENTIRE_PAGE:
                    eZoomType = SVX_ZOOM_WHOLEPAGE;
                    break;

                default:
                    return;
            }
            SvxZoomItem aZoomItem( eZoomType );
            pDispatcher->Execute(SID_ATTR_ZOOM,SFX_CALLMODE_SYNCHRON,&aZoomItem, 0L);
        }
    }
}




SdXImpressDocument* SdUnoDrawView::GetModel (void) const throw()
{
    if (mrView.GetDocSh()!=NULL)
    {
        Reference<frame::XModel> xModel (mrView.GetDocSh()->GetModel());
        return SdXImpressDocument::getImplementation(xModel);
    }
    else
        return NULL;
}


} // end of namespace sd
