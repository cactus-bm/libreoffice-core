/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: appreg.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: rt $ $Date: 2006-05-02 16:17:28 $
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

#include <vcl/toolbox.hxx>

#ifndef GCC
#pragma hdrstop
#endif

#include "app.hxx"
#include "appdata.hxx"
#include "arrdecl.hxx"
#include "sfxhelp.hxx"
#include "templdlg.hxx"
#include "objmnctl.hxx"
#include "inettbc.hxx"
#include "stbitem.hxx"
#include "navigat.hxx"
#include "module.hxx"
#include "topfrm.hxx"
#include "partwnd.hxx"
#include "sfxsids.hrc"
#include "recfloat.hxx"
#include "objsh.hxx"
#include "viewsh.hxx"
#include "objface.hxx"

//===================================================================

void SfxApplication::Registrations_Impl()
{
    // Interfaces
    SfxApplication::RegisterInterface();
    SfxModule::RegisterInterface();
    SfxViewFrame::RegisterInterface();
    SfxTopViewFrame::RegisterInterface();
    SfxObjectShell::RegisterInterface();
    SfxViewShell::RegisterInterface();

    // ChildWindows
    SfxRecordingFloatWrapper_Impl::RegisterChildWindow();
    SfxNavigatorWrapper::RegisterChildWindow();
    SfxPartChildWnd_Impl::RegisterChildWindow();
    SfxTemplateDialogWrapper::RegisterChildWindow(TRUE);

    // Controller
    SfxToolBoxControl::RegisterControl(SID_REPEAT);
    SfxURLToolBoxControl_Impl::RegisterControl(SID_OPENURL);
    SfxCancelToolBoxControl_Impl::RegisterControl(SID_BROWSE_STOP);
    SfxAppToolBoxControl_Impl::RegisterControl( SID_NEWDOCDIRECT );
    SfxAppToolBoxControl_Impl::RegisterControl( SID_AUTOPILOTMENU );
};

//--------------------------------------------------------------------

void SfxApplication::RegisterToolBoxControl_Impl( SfxModule *pMod, SfxTbxCtrlFactory *pFact )
{
    if ( pMod )
    {
        pMod->RegisterToolBoxControl( pFact );
        return;
    }

#ifdef DBG_UTIL
    for ( USHORT n=0; n<pAppData_Impl->pTbxCtrlFac->Count(); n++ )
    {
        SfxTbxCtrlFactory *pF = (*pAppData_Impl->pTbxCtrlFac)[n];
        if ( pF->nTypeId && pF->nTypeId == pFact->nTypeId &&
            (pF->nSlotId == pFact->nSlotId || pF->nSlotId == 0) )
        {
            DBG_WARNING("TbxController-Registrierung ist nicht eindeutig!");
        }
    }
#endif

    pAppData_Impl->pTbxCtrlFac->C40_INSERT( SfxTbxCtrlFactory, pFact, pAppData_Impl->pTbxCtrlFac->Count() );
}

//--------------------------------------------------------------------

void SfxApplication::RegisterStatusBarControl_Impl( SfxModule *pMod, SfxStbCtrlFactory *pFact )
{
    if ( pMod )
    {
        pMod->RegisterStatusBarControl( pFact );
        return;
    }

#ifdef DBG_UTIL
    for ( USHORT n=0; n<pAppData_Impl->pStbCtrlFac->Count(); n++ )
    {
        SfxStbCtrlFactory *pF = (*pAppData_Impl->pStbCtrlFac)[n];
        if ( pF->nTypeId && pF->nTypeId == pFact->nTypeId &&
            (pF->nSlotId == pFact->nSlotId || pF->nSlotId == 0) )
        {
            DBG_WARNING("StbController-Registrierung ist nicht eindeutig!");
        }
    }
#endif

    pAppData_Impl->pStbCtrlFac->C40_INSERT( SfxStbCtrlFactory, pFact, pAppData_Impl->pStbCtrlFac->Count() );
}

//--------------------------------------------------------------------

void SfxApplication::RegisterMenuControl_Impl( SfxModule *pMod, SfxMenuCtrlFactory *pFact )
{
    if ( pMod )
    {
        pMod->RegisterMenuControl( pFact );
        return;
    }

#ifdef DBG_UTIL
    for ( USHORT n=0; n<pAppData_Impl->pMenuCtrlFac->Count(); n++ )
    {
        SfxMenuCtrlFactory *pF = (*pAppData_Impl->pMenuCtrlFac)[n];
        if ( pF->nTypeId && pF->nTypeId == pFact->nTypeId &&
            (pF->nSlotId == pFact->nSlotId || pF->nSlotId == 0) )
        {
            DBG_WARNING("MenuController-Registrierung ist nicht eindeutig!");
        }
    }
#endif

    pAppData_Impl->pMenuCtrlFac->C40_INSERT( SfxMenuCtrlFactory, pFact, pAppData_Impl->pMenuCtrlFac->Count() );
}
