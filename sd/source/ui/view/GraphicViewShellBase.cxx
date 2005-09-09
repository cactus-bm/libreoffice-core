/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: GraphicViewShellBase.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:59:09 $
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

#include "GraphicViewShellBase.hxx"

#include "ViewTabBar.hxx"
#ifndef SD_RESID_HXX
#include "sdresid.hxx"
#endif
#ifndef SD_GRAPHIC_DOC_SHELL_HXX
#include "GraphicDocShell.hxx"
#endif
#include "strings.hrc"
#include "app.hrc"

#ifndef _SFXREQUEST_HXX
#include <sfx2/request.hxx>
#endif

namespace sd {

class DrawDocShell;

TYPEINIT1(GraphicViewShellBase, ViewShellBase);

// We have to expand the SFX_IMPL_VIEWFACTORY macro to call LateInit() after a
// new GraphicViewShellBase object has been constructed.

/*
SFX_IMPL_VIEWFACTORY(GraphicViewShellBase, SdResId(STR_DEFAULTVIEW))
{
    SFX_VIEW_REGISTRATION(GraphicDocShell);
}
*/
SfxViewFactory* GraphicViewShellBase::pFactory;
SfxViewShell* __EXPORT GraphicViewShellBase::CreateInstance (
    SfxViewFrame *pFrame, SfxViewShell *pOldView)
{
    GraphicViewShellBase* pBase = new GraphicViewShellBase(pFrame, pOldView);
    pBase->LateInit();
    return pBase;
}
void GraphicViewShellBase::RegisterFactory( USHORT nPrio )
{
    pFactory = new SfxViewFactory(
        &CreateInstance,&InitFactory,nPrio,SdResId(STR_DEFAULTVIEW));
    InitFactory();
}
void GraphicViewShellBase::InitFactory()
{
    SFX_VIEW_REGISTRATION(GraphicDocShell);
}








GraphicViewShellBase::GraphicViewShellBase (
    SfxViewFrame* pFrame,
    SfxViewShell* pOldShell)
    : ViewShellBase (pFrame, pOldShell, ViewShell::ST_DRAW)
{
}




void GraphicViewShellBase::LateInit (void)
{
    ViewShellBase::LateInit();
    // Turn the ViewTabBar off again.  It is not needed for Draw.
    mpViewTabBar.reset();
}




GraphicViewShellBase::~GraphicViewShellBase (void)
{
}




void GraphicViewShellBase::Execute (SfxRequest& rRequest)
{
    USHORT nSlotId = rRequest.GetSlot();

    switch (nSlotId)
    {
        case SID_TASK_PANE:
            // Since there is no tool panel in Draw we ignore this call.
            rRequest.Cancel();
            break;

        default:
            // The remaining requests are forwarded to out base class.
            ViewShellBase::Execute (rRequest);
            break;
    }

}


} // end of namespace sd

