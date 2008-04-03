/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: PaneHider.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: kz $ $Date: 2008-04-03 14:09:56 $
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
#include "PaneHider.hxx"

#include "ViewShell.hxx"
#include "ViewShellBase.hxx"
#include "slideshow.hxx"
#include "slideshowimpl.hxx"
#include "framework/FrameworkHelper.hxx"
#include "framework/ConfigurationController.hxx"

#include <com/sun/star/drawing/framework/XControllerManager.hpp>
#include <com/sun/star/drawing/framework/XConfigurationController.hpp>
#include <com/sun/star/drawing/framework/XConfiguration.hpp>
#include <com/sun/star/lang/DisposedException.hpp>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::drawing::framework;
using ::sd::framework::FrameworkHelper;
using ::com::sun::star::lang::DisposedException;

namespace sd {

PaneHider::PaneHider (const ViewShell& rViewShell, SlideshowImpl* pSlideShow)
    : mrViewShell(rViewShell),
      mbWindowVisibilitySaved(false),
      mbOriginalLeftPaneWindowVisibility(false),
      mbOriginalRightPaneWindowVisibility(false)
{
     // Hide the left and right pane windows when a slideshow exists and is
    // not full screen.
    if (pSlideShow!=NULL && !pSlideShow->isFullScreen()) try
    {
        Reference<XControllerManager> xControllerManager (
            mrViewShell.GetViewShellBase().GetController(), UNO_QUERY_THROW);
        mxConfigurationController = xControllerManager->getConfigurationController();
        if (mxConfigurationController.is())
        {
            // Get and save the current configuration.
            mxConfiguration = mxConfigurationController->getRequestedConfiguration();
            if (mxConfiguration.is())
            {
                // Iterate over the resources and deactivate the panes.
                Sequence<Reference<XResourceId> > aResources (
                    mxConfiguration->getResources(
                        NULL,
                        framework::FrameworkHelper::msPaneURLPrefix,
                        AnchorBindingMode_DIRECT));
                for (sal_Int32 nIndex=0; nIndex<aResources.getLength(); ++nIndex)
                {
                    Reference<XResourceId> xPaneId (aResources[nIndex]);
                    if ( ! xPaneId->getResourceURL().equals(FrameworkHelper::msCenterPaneURL))
                    {
                        mxConfigurationController->requestResourceDeactivation(xPaneId);
                    }
                }
            }
        }
        FrameworkHelper::Instance(mrViewShell.GetViewShellBase())->WaitForUpdate();
    }
    catch (RuntimeException&)
    {
        DBG_ASSERT(false, "caught exception in PaneHider constructor");
    }
}




PaneHider::~PaneHider (void)
{
    if (mxConfiguration.is() && mxConfigurationController.is())
    {
        try
        {
            mxConfigurationController->restoreConfiguration(mxConfiguration);
        }
        catch (DisposedException&)
        {
            // When the configuration controller is already disposed then
            // there is no point in restoring the configuration.
        }
    }
}


} // end of namespace sd
