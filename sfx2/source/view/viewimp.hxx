/*************************************************************************
 *
 *  $RCSfile: viewimp.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: mba $ $Date: 2002-10-24 12:23:07 $
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

#ifndef _VIEWIMP_HXX
#define _VIEWIMP_HXX

// include ---------------------------------------------------------------

#ifndef __SBX_SBXOBJ_HXX
#include <svtools/sbxobj.hxx>
#endif
#include "viewsh.hxx"

#include <osl/mutex.hxx>
#ifndef _CPPUHELPER_INTERFACECONTAINER_HXX_
#include <cppuhelper/interfacecontainer.hxx>
#endif

// forward ---------------------------------------------------------------

class SfxFrameSetDescriptor;
class SfxMenuBarManager;
class SfxAcceleratorManager;
class SfxOfficeDispatch;
class SfxBaseController;

struct ModelessDialog_Impl
{
    Dialog* pDialog;
    USHORT  nSlotId;
};

typedef ModelessDialog_Impl* ModelessDialogPtr_Impl;
SV_DECL_PTRARR( ModelessDialogPtrArr_Impl, ModelessDialogPtr_Impl, 4, 4 );

typedef SfxShell* SfxShellPtr_Impl;
SV_DECL_PTRARR( SfxShellArr_Impl, SfxShellPtr_Impl, 4, 4 );

typedef SfxOfficeDispatch* SfxOffDispPtr_Impl;
SV_DECL_PTRARR( SfxOffDispArr_Impl, SfxOffDispPtr_Impl, 4, 4 );

// struct SfxViewShell_Impl ----------------------------------------------

struct SfxViewShell_Impl
{
    ::osl::Mutex               aMutex;
    ::cppu::OInterfaceContainerHelper aInterceptorContainer;
    BOOL                        bControllerSet;
    SfxShellArr_Impl            aArr;
    ModelessDialogPtrArr_Impl   aDialogArr;
    SbxObjectRef                xSelectionObj;
    SvBorder                    aBorder;
    Size                        aOptimalSize;
    SfxFrameSetDescriptor*      pSetDescr;
    Size                        aMargin;
    USHORT                      nPrinterLocks;
    BOOL                        bUseObjectSize;
    BOOL                        bCanPrint;
    BOOL                        bHasPrintOptions;
    BOOL                        bPlugInsActive;
    BOOL                        bIsShowView;
    BOOL                        bFrameSetImpl;
    BOOL                        bOwnsMenu;
    BOOL                        bGotOwnerShip;
    SfxScrollingMode            eScroll;
    ResId*                      pMenuBarResId;
    SfxMenuBarManager*          pMenu;
    ResId*                      pAccelResId;
    SfxAcceleratorManager*      pAccel;
    USHORT                      nFamily;
    SfxBaseController*          pController;

                                SfxViewShell_Impl()
                                  : aInterceptorContainer( aMutex )
                                {}
};


#endif

