/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: PreviewRenderer.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 05:12:52 $
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

#ifndef SD_PREVIEW_RENDERER_HXX
#define SD_PREVIEW_RENDERER_HXX

#include "drawview.hxx"
#include <vcl/image.hxx>
#include <vcl/virdev.hxx>
#include <svtools/listener.hxx>
#include <memory>


class OutputDevice;
class SdPage;
class VirtualDevice;

namespace sd {

class DrawDocShell;
class DrawView;


class PreviewRenderer
    : public SfxListener
{
public:
    /** Create a new preview renderer that takes some of its inital values
        from the given output device.
        @param pTemplate
            May be NULL.
    */
    PreviewRenderer (OutputDevice* pTemplate = NULL);
    ~PreviewRenderer (void);

    /** Render a page with the given pixel size.
        Use this version when only the width of the preview is known to the
        caller.  The height is then calculated according to the aspect
        ration of the given page.
        @param pPage
            The page to render.
        @param nWidth
            The width of the preview in device coordinates.
        @param sSubstitutionText
            When the actual preview can not be created for some reason, then
            this text is painted in an empty rectangle of the requested size
            instead.
    */
    Image RenderPage (
        const SdPage* pPage,
        int nWidth,
        const String& sSubstitutionText);

    /** Render a page with the given pixel size.
        @param pPage
            The page to render.
        @param aPreviewPixelSize
            The size in device coordinates of the preview.
        @param sSubstitutionText
            When the actual preview can not be created for some reason, then
            this text is painted in an empty rectangle of the requested size
            instead.
    */
    Image RenderPage (
        const SdPage* pPage,
        Size aPreviewPixelSize,
        const String& sSubstitutionText);

    /** Scale the given bitmap by keeping its aspect ratio to the desired
        width.  Add a frame to it afterwards.
    */
    Image ScaleBitmap (
        const BitmapEx& rBitmap,
        int nWidth);

protected:
    virtual void SFX_NOTIFY(SfxBroadcaster& rBC,
        const TypeId& rBCType,
        const SfxHint& rHint,
        const TypeId& rHintType);

private:
    ::std::auto_ptr<VirtualDevice> mpPreviewDevice;
    ::std::auto_ptr<DrawView> mpView;
    DrawDocShell* mpDocShellOfView;
    int mnWidthOfView;
    Color maFrameColor;
    static const int snSubstitutioinTextSize;
    // Width of the frame that is painted arround the preview.
    static const int snFrameWidth;

    bool Initialize (
        const SdPage* pPage,
        const Size& rPixelSize);
    void Cleanup (void);
    void PaintPage (const SdPage* pPage);
    void PaintSubstitutionText (const String& rSubstitutionText);
    void PaintFrame (void);

    /** Set up the map mode so that the given page is renderer into a bitmap
        with the specified width.
     */
    void SetupOutputSize (const SdPage* pPage, const Size& rPixelSize);

    /** When mpView is empty then create a new view and initialize it.
        Otherwise just initialize it.
    */
    void ProvideView (DrawDocShell* pDocShell);
};

} // end of namespace ::sd

#endif
