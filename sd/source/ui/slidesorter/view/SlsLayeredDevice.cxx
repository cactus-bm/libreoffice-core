/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: SlsViewCacheContext.hxx,v $
 *
 * $Revision: 1.3 $
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

#include "precompiled_sd.hxx"

#include "SlsLayeredDevice.hxx"

#include <vcl/window.hxx>
#include <vcl/virdev.hxx>

#include <boost/bind.hpp>
#include <boost/function.hpp>


namespace sd { namespace slidesorter { namespace view {

namespace {
static const sal_Int32 gnMaximumLayerCount = 8;

class LayerInvalidator : public ILayerInvalidator
{
public:
    LayerInvalidator (
        const ::boost::shared_ptr<LayeredDevice>& rpLayeredDevice,
        const SharedSdWindow& rpTargetWindow,
        const int nLayer)
        : mpLayeredDevice(rpLayeredDevice),
          mpTargetWindow(rpTargetWindow),
          mnLayer(nLayer)
    {
    }

    virtual void Invalidate (const Rectangle& rInvalidationBox)
    {
        mpLayeredDevice->Invalidate(rInvalidationBox, mnLayer);
        mpTargetWindow->Invalidate(rInvalidationBox);
    }

private:
    const ::boost::shared_ptr<LayeredDevice> mpLayeredDevice;
    SharedSdWindow mpTargetWindow;
    const int mnLayer;
};

void DeviceCopy (
    OutputDevice& rTargetDevice,
    OutputDevice& rSourceDevice,
    const Rectangle& rBox)
{
    rTargetDevice.DrawOutDev(
        rBox.TopLeft(),
        rBox.GetSize(),
        rBox.TopLeft(),
        rBox.GetSize(),
        rSourceDevice);
}


void ForAllRectangles (const Region& rRegion, ::boost::function<void(const Rectangle&)> aFunction)
{
    OSL_ASSERT(aFunction);

    if (rRegion.GetRectCount() <= 1)
    {
        aFunction(rRegion.GetBoundRect());
    }
    else
    {
        Region aMutableRegionCopy (rRegion);
        RegionHandle aHandle(aMutableRegionCopy.BeginEnumRects());
        Rectangle aBox;
        while (aMutableRegionCopy.GetNextEnumRect(aHandle, aBox))
            aFunction(aBox);
        aMutableRegionCopy.EndEnumRects(aHandle);
    }
}

class Layer : private ::boost::noncopyable
{
public:
    Layer (void);
    ~Layer (void);

    void Initialize (const SharedSdWindow& rpTargetWindow);
    void InvalidateRectangle (const Rectangle& rInvalidationBox);
    void InvalidateRegion (const Region& rInvalidationRegion);
    void Validate (const MapMode& rMapMode);
    void Repaint (
        OutputDevice& rTargetDevice,
        const Rectangle& rRepaintRectangle);
    void Resize (const Size& rSize);
    void AddPainter (const SharedILayerPainter& rpPainter);
    void RemovePainter (const SharedILayerPainter& rpPainter);
    bool HasPainter (void) const;
    void Dispose (void);

private:
    ::boost::shared_ptr<VirtualDevice> mpLayerDevice;
    ::std::vector<SharedILayerPainter> maPainters;
    Region maInvalidationRegion;

    void ValidateRectangle (const Rectangle& rBox);
};
typedef ::boost::shared_ptr<Layer> SharedLayer;


} // end of anonymous namespace


class LayeredDevice::LayerContainer : public ::std::vector<SharedLayer>
{
public:
    LayerContainer (void) { OSL_TRACE("created layer container at %x", this); }
    ~LayerContainer (void) { OSL_TRACE("destroyed layer container at %x", this); }
};




//===== LayeredDevice =========================================================

LayeredDevice::LayeredDevice (const SharedSdWindow& rpTargetWindow)
    : mpTargetWindow(rpTargetWindow),
      mpLayers(new LayerContainer()),
      mpBackBuffer(new VirtualDevice(*mpTargetWindow)),
      maSavedMapMode(rpTargetWindow->GetMapMode())
{
    OSL_TRACE("layered device created at %x", this);
    mpBackBuffer->SetOutputSizePixel(mpTargetWindow->GetSizePixel());
}




LayeredDevice::~LayeredDevice (void)
{
    OSL_TRACE("layered device destroyed at %x", this);
}




void LayeredDevice::Invalidate (
    const Rectangle& rInvalidationArea,
    const sal_Int32 nLayer)
{
    if (nLayer<0 || sal_uInt32(nLayer)>=mpLayers->size())
    {
        OSL_ASSERT(nLayer>=0 && nLayer<mpLayers->size());
        return;
    }

    (*mpLayers)[nLayer]->InvalidateRectangle(rInvalidationArea);
}




void LayeredDevice::InvalidateAllLayers (const Rectangle& rInvalidationArea)
{
    for (sal_uInt32 nLayer=0; nLayer<mpLayers->size(); ++nLayer)
        (*mpLayers)[nLayer]->InvalidateRectangle(rInvalidationArea);
}




void LayeredDevice::InvalidateAllLayers (const Region& rInvalidationRegion)
{
    for (sal_uInt32 nLayer=0; nLayer<mpLayers->size(); ++nLayer)
        (*mpLayers)[nLayer]->InvalidateRegion(rInvalidationRegion);
}




void LayeredDevice::RegisterPainter (
    const SharedILayerPainter& rpPainter,
    const sal_Int32 nLayer)
{
    OSL_TRACE("layered device registering painter at %x", this);
    OSL_ASSERT(mpLayers);
    if ( ! rpPainter)
    {
        OSL_ASSERT(rpPainter);
        return;
    }
    if (nLayer<0 || nLayer>=gnMaximumLayerCount)
    {
        OSL_ASSERT(nLayer>=0 && nLayer<gnMaximumLayerCount);
        return;
    }

    // Provide the layers.
    if (sal_uInt32(nLayer) >= mpLayers->size())
    {
        const sal_Int32 nOldLayerCount (mpLayers->size());
        mpLayers->resize(nLayer+1);

        for (size_t nIndex=nOldLayerCount; nIndex<mpLayers->size(); ++nIndex)
            (*mpLayers)[nIndex].reset(new Layer());
    }

    (*mpLayers)[nLayer]->AddPainter(rpPainter);
    if (nLayer == 0)
        (*mpLayers)[nLayer]->Initialize(mpTargetWindow);

    rpPainter->SetLayerInvalidator(
        SharedILayerInvalidator(new LayerInvalidator(shared_from_this(),mpTargetWindow,nLayer)));
}




void LayeredDevice::RemovePainter (
    const SharedILayerPainter& rpPainter,
    const sal_Int32 nLayer)
{
    if ( ! rpPainter)
    {
        OSL_ASSERT(rpPainter);
        return;
    }
    if (nLayer<0 || sal_uInt32(nLayer)>=mpLayers->size())
    {
        OSL_ASSERT(nLayer>=0 && nLayer<mpLayers->size());
        return;
    }

    rpPainter->SetLayerInvalidator(SharedILayerInvalidator());

    (*mpLayers)[nLayer]->RemovePainter(rpPainter);

    // Remove top most layers that do not contain any painters.
    while ( ! mpLayers->empty() && ! mpLayers->back()->HasPainter())
        mpLayers->erase(mpLayers->end()-1);
}




bool LayeredDevice::HasPainter (const sal_Int32 nLayer)
{
    return nLayer>=0
        && sal_uInt32(nLayer)<mpLayers->size()
        && (*mpLayers)[nLayer]->HasPainter();
}




void LayeredDevice::Repaint (const Region& rRepaintRegion)
{
    HandleMapModeChange();

    // Validate the contents of all layers (that have their own devices.)
    ::std::for_each(
        mpLayers->begin(),
        mpLayers->end(),
        ::boost::bind(&Layer::Validate, _1, mpTargetWindow->GetMapMode()));

    ForAllRectangles(rRepaintRegion, ::boost::bind(&LayeredDevice::RepaintRectangle, this, _1));
}




void LayeredDevice::RepaintRectangle (const Rectangle& rRepaintRectangle)
{
    if (mpLayers->size() == 0)
        return;
    else if (mpLayers->size() == 1)
    {
        // Just copy the main layer into the target device.
        (*mpLayers)[0]->Repaint(*mpTargetWindow, rRepaintRectangle);
    }
    else
    {
        // Paint all layers first into the back buffer (to avoid flickering
        // due to synchronous paints) and then copy that into the target
        // device.
        mpBackBuffer->SetMapMode(mpTargetWindow->GetMapMode());
        ::std::for_each(
            mpLayers->begin(),
            mpLayers->end(),
            ::boost::bind(&Layer::Repaint, _1, ::boost::ref(*mpBackBuffer), rRepaintRectangle));

        DeviceCopy(*mpTargetWindow, *mpBackBuffer, rRepaintRectangle);
    }
}




void LayeredDevice::Resize (void)
{
    const Size aSize (mpTargetWindow->GetSizePixel());
    mpBackBuffer->SetOutputSizePixel(aSize);
    ::std::for_each(mpLayers->begin(), mpLayers->end(), ::boost::bind(&Layer::Resize, _1, aSize));
}




void LayeredDevice::Dispose (void)
{
    ::std::for_each(mpLayers->begin(), mpLayers->end(), ::boost::bind(&Layer::Dispose, _1));
    mpLayers->clear();
}




void LayeredDevice::HandleMapModeChange (void)
{
    const MapMode& rMapMode (mpTargetWindow->GetMapMode());
    if (maSavedMapMode == rMapMode)
        return;

    const Rectangle aLogicWindowBox (
        mpTargetWindow->PixelToLogic(Rectangle(Point(0,0), mpTargetWindow->GetSizePixel())));
    if (maSavedMapMode.GetScaleX() != rMapMode.GetScaleX()
        || maSavedMapMode.GetScaleY() != rMapMode.GetScaleY()
        || maSavedMapMode.GetMapUnit() != rMapMode.GetMapUnit())
    {
        // When the scale has changed then we have to paint everything.
        ::std::for_each(
            mpLayers->begin(),
            mpLayers->end(),
            ::boost::bind(&Layer::InvalidateRectangle, _1, ::boost::cref(aLogicWindowBox)));
    }
    else if (maSavedMapMode.GetOrigin() != rMapMode.GetOrigin())
    {
        // Window has been scrolled.  Adapt contents of backbuffers and
        // layer devices.
        const Point aDelta (rMapMode.GetOrigin() - maSavedMapMode.GetOrigin());
        mpBackBuffer->CopyArea(
            aLogicWindowBox.TopLeft(),
            mpTargetWindow->PixelToLogic(Point(0,0), maSavedMapMode),
            aLogicWindowBox.GetSize());
        InvalidateAllLayers(aLogicWindowBox);

        /*
        const Rectangle aWindowBox (Point(0,0), mpTargetWindow->GetSizePixel());
        if (aDelta.X < 0)
            Invalidate(
                mpTargetWindow->PixelToLogic(
                    aWindowBox.TopRight(),
                    Point(mpTargetWindow->GetSizePixel().Right()X+aDelta.X,0)),
                mpTargetWindow->PixelToLogic(
                    Point(mpTargetWindow->GetSizePixel().X+aDelta.X,0)),
        */
    }
    else
    {
        // Can this happen?  Lets trigger a warning when it does.
        OSL_ASSERT(false);
    }

    maSavedMapMode = rMapMode;
}




//===== Layer =================================================================

Layer::Layer (void)
    : mpLayerDevice(),
      maPainters(),
      maInvalidationRegion()
{
}




Layer::~Layer (void)
{
}




void Layer::Initialize (const SharedSdWindow& rpTargetWindow)
{
    if ( ! mpLayerDevice)
    {
        mpLayerDevice.reset(new VirtualDevice(*rpTargetWindow));
        mpLayerDevice->SetOutputSizePixel(rpTargetWindow->GetSizePixel());
    }
}




void Layer::InvalidateRectangle (const Rectangle& rInvalidationBox)
{
    maInvalidationRegion.Union(rInvalidationBox);
}




void Layer::InvalidateRegion (const Region& rInvalidationRegion)
{
    maInvalidationRegion.Union(rInvalidationRegion);
}




void Layer::Validate (const MapMode& rMapMode)
{
    if (mpLayerDevice && ! maInvalidationRegion.IsEmpty())
    {
        mpLayerDevice->SetMapMode(rMapMode);

        ForAllRectangles(
            maInvalidationRegion,
            ::boost::bind(&Layer::ValidateRectangle, this, _1));
    }
    // else nothing to do now.  The painting is done in Repaint() directly
    // into the back buffer.

    maInvalidationRegion.SetEmpty();
}




void Layer::ValidateRectangle (const Rectangle& rBox)
{
    if ( ! mpLayerDevice)
        return;

    const Region aSavedClipRegion (mpLayerDevice->GetClipRegion());
    mpLayerDevice->SetClipRegion(Region(rBox));

    for (::std::vector<SharedILayerPainter>::const_iterator
             iPainter(maPainters.begin()),
             iEnd(maPainters.end());
         iPainter!=iEnd;
         ++iPainter)
    {
        (*iPainter)->Paint(*mpLayerDevice, rBox);
    }

    mpLayerDevice->SetClipRegion(aSavedClipRegion);
}




void Layer::Repaint (
    OutputDevice& rTargetDevice,
    const Rectangle& rRepaintRectangle)
{
    if (mpLayerDevice)
    {
        DeviceCopy(rTargetDevice, *mpLayerDevice, rRepaintRectangle);
    }
    else
    {
        ::std::for_each(
            maPainters.begin(),
            maPainters.end(),
            ::boost::bind(&ILayerPainter::Paint,
                _1,
                ::boost::ref(rTargetDevice),
                rRepaintRectangle));
    }
}




void Layer::Resize (const Size& rSize)
{
    if (mpLayerDevice)
    {
        mpLayerDevice->SetOutputSizePixel(rSize);
        maInvalidationRegion.Union(Rectangle(Point(0,0), rSize));
    }
}




void Layer::AddPainter (const SharedILayerPainter& rpPainter)
{
    OSL_ASSERT(::std::find(maPainters.begin(), maPainters.end(), rpPainter) == maPainters.end());

    maPainters.push_back(rpPainter);
}




void Layer::RemovePainter (const SharedILayerPainter& rpPainter)
{
    const ::std::vector<SharedILayerPainter>::iterator iPainter (
        ::std::find(maPainters.begin(), maPainters.end(), rpPainter));
    if (iPainter != maPainters.end())
    {
        maPainters.erase(iPainter);
    }
    else
    {
        DBG_ASSERT(false,"LayeredDevice::RemovePainter called for painter that is not registered");
    }
}




bool Layer::HasPainter (void) const
{
    return !maPainters.empty();
}




void Layer::Dispose (void)
{
    maPainters.clear();
}


} } } // end of namespace ::sd::slidesorter::view
