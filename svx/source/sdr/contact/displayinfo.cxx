/*************************************************************************
 *
 *  $RCSfile: displayinfo.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: kz $ $Date: 2004-02-26 17:46:20 $
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

#ifndef _SDR_CONTACT_DISPLAYINFO_HXX
#include <svx/sdr/contact/displayinfo.hxx>
#endif

#ifndef _SV_OUTDEV_HXX
#include <vcl/outdev.hxx>
#endif

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

#ifndef _SVDOBJ_HXX
#include <svdobj.hxx>
#endif

#ifndef _SV_GDIMTF_HXX
#include <vcl/gdimtf.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace contact
    {
        // This uses Application::AnyInput() and may change mbContinuePaint
        // to interrupt the paint
        void DisplayInfo::CheckContinuePaint()
        {
            // #111111#
            // INPUT_PAINT and INPUT_TIMER removed again since this leads to
            // problems under Linux and Solaris when painting slow objects
            // (e.g. bitmaps)

            // #114335#
            // INPUT_OTHER removed too, leads to problems with added controls
            // from the form layer.
            if(Application::AnyInput(INPUT_KEYBOARD))
            {
                mbContinuePaint = sal_False;
            }
        }

        DisplayInfo::DisplayInfo(const SdrPageView* pPageView)
        :   mpPageView(pPageView),
            // init layer info with all bits set to draw everything on default
            maProcessLayers(sal_True),
            mpOutputDevice(0L),
            mpExtOutputDevice(0L),
            mpPaintInfoRec(0L),
            mpRootVOC(0L),
            mbControlLayerPainting(sal_False),
            mbPagePainting(sal_True),
            mbGhostedDrawModeActive(sal_False),
            mbBufferingAllowed(sal_True),
            mbContinuePaint(sal_True),
            mbMasterPagePainting(sal_False),
            mbPreRenderingAllowed(sal_False)
        {
        }

        DisplayInfo::~DisplayInfo()
        {
        }

        // Access to LayerInfos (which layers to proccess)
        void DisplayInfo::SetProcessLayers(const SetOfByte& rSet)
        {
            maProcessLayers = rSet;
        }

        const SetOfByte& DisplayInfo::GetProcessLayers() const
        {
            return maProcessLayers;
        }

        // access to ExtendedOutputDevice
        void DisplayInfo::SetExtendedOutputDevice(ExtOutputDevice* pExtOut)
        {
            if(mpExtOutputDevice != pExtOut)
            {
                mpExtOutputDevice = pExtOut;
            }
        }

        ExtOutputDevice* DisplayInfo::GetExtendedOutputDevice() const
        {
            return mpExtOutputDevice;
        }

        // access to PaintInfoRec
        void DisplayInfo::SetPaintInfoRec(SdrPaintInfoRec* pInfoRec)
        {
            if(mpPaintInfoRec != pInfoRec)
            {
                mpPaintInfoRec = pInfoRec;
            }
        }

        SdrPaintInfoRec* DisplayInfo::GetPaintInfoRec() const
        {
            return mpPaintInfoRec;
        }

        // access to OutputDevice
        void DisplayInfo::SetOutputDevice(OutputDevice* pOutDev)
        {
            if(mpOutputDevice != pOutDev)
            {
                mpOutputDevice = pOutDev;
            }
        }

        OutputDevice* DisplayInfo::GetOutputDevice() const
        {
            return mpOutputDevice;
        }

        // access to RedrawArea
        void DisplayInfo::SetRedrawArea(const Region& rRegion)
        {
            maRedrawArea = rRegion;
        }

        const Region& DisplayInfo::GetRedrawArea() const
        {
            return maRedrawArea;
        }

        // Is OutDev a printer?
        sal_Bool DisplayInfo::OutputToPrinter() const
        {
            if(mpOutputDevice && OUTDEV_PRINTER == mpOutputDevice->GetOutDevType())
            {
                return sal_True;
            }

            return sal_False;
        }

        // Is OutDev a window?
        sal_Bool DisplayInfo::OutputToWindow() const
        {
            if(mpOutputDevice && OUTDEV_WINDOW == mpOutputDevice->GetOutDevType())
            {
                return sal_True;
            }

            return sal_False;
        }

        // Is OutDev a VirtualDevice?
        sal_Bool DisplayInfo::OutputToVirtualDevice() const
        {
            if(mpOutputDevice && OUTDEV_VIRDEV == mpOutputDevice->GetOutDevType())
            {
                return sal_True;
            }

            return sal_False;
        }

        // Is OutDev a recording MetaFile?
        sal_Bool DisplayInfo::OutputToRecordingMetaFile() const
        {
            if(mpOutputDevice)
            {
                GDIMetaFile* pMetaFile = mpOutputDevice->GetConnectMetaFile();

                if(pMetaFile)
                {
                    sal_Bool bRecording = pMetaFile->IsRecord() && !pMetaFile->IsPause();
                    return bRecording;
                }
            }

            return sal_False;
        }

        void DisplayInfo::SetControlLayerPainting(sal_Bool bDoPaint)
        {
            if(mbControlLayerPainting != bDoPaint)
            {
                mbControlLayerPainting = bDoPaint;
            }
        }

        sal_Bool DisplayInfo::GetControlLayerPainting() const
        {
            return mbControlLayerPainting;
        }

        void DisplayInfo::SetPagePainting(sal_Bool bDoPaint)
        {
            if(mbPagePainting != bDoPaint)
            {
                mbPagePainting = bDoPaint;
            }
        }

        sal_Bool DisplayInfo::GetPagePainting() const
        {
            return mbPagePainting;
        }

        // Access to svtools::ColorConfig
        const svtools::ColorConfig& DisplayInfo::GetColorConfig() const
        {
            return maColorConfig;
        }

        void DisplayInfo::SaveOriginalDrawMode()
        {
            if(mpOutputDevice)
            {
                mnOriginalDrawMode = mpOutputDevice->GetDrawMode();
            }
        }

        sal_uInt32 DisplayInfo::GetOriginalDrawMode() const
        {
            return mnOriginalDrawMode;
        }

        sal_uInt32 DisplayInfo::GetCurrentDrawMode() const
        {
            if(mpOutputDevice)
            {
                return mpOutputDevice->GetDrawMode();
            }

            // fallback to OriginalDrawMode
            return GetOriginalDrawMode();
        }

        void DisplayInfo::RestoreOriginalDrawMode()
        {
            if(mpOutputDevice)
            {
                mpOutputDevice->SetDrawMode(mnOriginalDrawMode);
                mbGhostedDrawModeActive = sal_False;
            }
        }

        void DisplayInfo::SetGhostedDrawMode()
        {
            if(mpOutputDevice)
            {
                mpOutputDevice->SetDrawMode(mnOriginalDrawMode | (
                DRAWMODE_GHOSTEDLINE|DRAWMODE_GHOSTEDFILL|DRAWMODE_GHOSTEDTEXT|
                DRAWMODE_GHOSTEDBITMAP|DRAWMODE_GHOSTEDGRADIENT));
                mbGhostedDrawModeActive = sal_True;
            }
        }

        sal_Bool DisplayInfo::IsGhostedDrawModeActive() const
        {
            return mbGhostedDrawModeActive;
        }

        // access to buffering allowed flag
        void DisplayInfo::SetBufferingAllowed(sal_Bool bNew)
        {
            if(mbBufferingAllowed != bNew)
            {
                mbBufferingAllowed = bNew;
            }
        }

        sal_Bool DisplayInfo::IsBufferingAllowed() const
        {
            return mbBufferingAllowed;
        }

        // Check if painting should be continued. If not, return from paint
        // as soon as possible.
        sal_Bool DisplayInfo::DoContinuePaint()
        {
            if(mbContinuePaint
                && mpOutputDevice
                && OUTDEV_WINDOW == mpOutputDevice->GetOutDevType())
            {
                CheckContinuePaint();
            }

            return mbContinuePaint;
        }

        sal_Bool DisplayInfo::GetMasterPagePainting() const
        {
            return mbMasterPagePainting;
        }

        void DisplayInfo::SetMasterPagePainting(sal_Bool bNew)
        {
            if(mbMasterPagePainting != bNew)
            {
                mbMasterPagePainting = bNew;
            }
        }

        // access to PreRendering flag
        sal_Bool DisplayInfo::IsPreRenderingAllowed() const
        {
            return mbPreRenderingAllowed;
        }

        void DisplayInfo::SetPreRenderingAllowed(sal_Bool bNew)
        {
            if(mbPreRenderingAllowed != bNew)
            {
                mbPreRenderingAllowed = bNew;
            }
        }

        // Infos about draft painting. These may get bitfield members later.
        sal_Bool DisplayInfo::IsDraftText() const
        {
            return (0 != (mpPaintInfoRec->nPaintMode & SDRPAINTMODE_DRAFTTEXT));
        }
        sal_Bool DisplayInfo::IsDraftGraphic() const
        {
            return (0 != (mpPaintInfoRec->nPaintMode & SDRPAINTMODE_DRAFTGRAF));
        }
        sal_Bool DisplayInfo::IsDraftLine() const
        {
            return (0 != (mpPaintInfoRec->nPaintMode & SDRPAINTMODE_DRAFTLINE));
        }
        sal_Bool DisplayInfo::IsDraftFill() const
        {
            return (0 != (mpPaintInfoRec->nPaintMode & SDRPAINTMODE_DRAFTFILL));
        }
        sal_Bool DisplayInfo::IsHideDraftGraphic() const
        {
            return (0 != (mpPaintInfoRec->nPaintMode & SDRPAINTMODE_HIDEDRAFTGRAF));
        }
    } // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////
// eof
