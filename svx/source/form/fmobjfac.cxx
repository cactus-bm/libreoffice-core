/*************************************************************************
 *
 *  $RCSfile: fmobjfac.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: oj $ $Date: 2002-10-31 13:47:23 $
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
#pragma hdrstop

#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif

#ifndef _SVDOBJ_HXX
#include "svdobj.hxx"
#endif

#ifndef _SVX_FMTOOLS_HXX
#include "fmtools.hxx"
#endif

#ifndef _SVX_FMSERVS_HXX
#include "fmservs.hxx"
#endif

#ifndef _FM_FMOBJFAC_HXX
#include "fmobjfac.hxx"
#endif

#ifndef _FM_FMGLOB_HXX
#include "fmglob.hxx"
#endif

#ifndef _FM_FMOBJ_HXX
#include "fmobj.hxx"
#endif

#ifndef _SVX_FMSHIMP_HXX
#include "fmshimp.hxx"
#endif

#ifndef _FM_FMSHELL_HXX
#include "fmshell.hxx"
#endif

#ifndef _SVX_SVXIDS_HRC
#include "svxids.hrc"
#endif

#ifndef _SVX_TBXFORM_HXX
#include "tbxform.hxx"
#endif

#ifndef _TOOLS_RESID_HXX //autogen
#include <tools/resid.hxx>
#endif

#ifndef _SVX_FMRESIDS_HRC
#include "fmresids.hrc"
#endif

#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif

#ifndef _SVX_DIALMGR_HXX
#include "dialmgr.hxx"
#endif

#ifndef _SVX_FMSERVS_HXX
#include "fmservs.hxx"
#endif

#ifndef _SVX_TABWIN_HXX
#include "tabwin.hxx"
#endif

#ifndef _SVX_FMEXPL_HXX
#include "fmexpl.hxx"
#endif

#ifndef _SVX_FILTNAV_HXX
#include "filtnav.hxx"
#endif

#ifndef _SVX_FMPROP_HRC
#include "fmprop.hrc"
#endif
#ifndef SVX_FMPROPBRW_HXX
#include "fmPropBrw.hxx"
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::svxform;

/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/
FmFormObjFactory::FmFormObjFactory()
{
    SdrObjFactory::InsertMakeObjectHdl(LINK(this, FmFormObjFactory, MakeObject));

    //////////////////////////////////////////////////////////////////////
    // Konfigurations-::com::sun::star::frame::Controller und NavigationBar registrieren
    SvxFmTbxCtlConfig::RegisterControl( SID_FM_CONFIG );
    SvxFmTbxCtlAbsRec::RegisterControl( SID_FM_RECORD_ABSOLUTE );
    SvxFmTbxCtlRecText::RegisterControl( SID_FM_RECORD_TEXT );
    SvxFmTbxCtlRecFromText::RegisterControl( SID_FM_RECORD_FROM_TEXT );
    SvxFmTbxCtlRecTotal::RegisterControl( SID_FM_RECORD_TOTAL );
    SvxFmTbxPrevRec::RegisterControl( SID_FM_RECORD_PREV );
    SvxFmTbxNextRec::RegisterControl( SID_FM_RECORD_NEXT );
    ControlConversionMenuController::RegisterControl(SID_FM_CHANGECONTROLTYPE);

    // Registrieung von globalen fenstern
    FmFieldWinMgr::RegisterChildWindow();
    FmPropBrwMgr::RegisterChildWindow();
    NavigatorFrameManager::RegisterChildWindow();
    FmFilterNavigatorWinMgr::RegisterChildWindow();

    //////////////////////////////////////////////////////////////////////
    // Interface fuer die Formshell registrieren
    FmFormShell::RegisterInterface(0);

    ImplSmartRegisterUnoServices();

}


/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/
FmFormObjFactory::~FmFormObjFactory()
{
}


/*************************************************************************
|*
|* ::com::sun::star::form::Form-Objekte erzeugen
|*
\************************************************************************/
namespace
{
    void    lcl_initProperty( FmFormObj* _pObject, const ::rtl::OUString& _rPropName, const Any& _rValue )
    {
        try
        {
            Reference< XPropertySet >  xModelSet( _pObject->GetUnoControlModel(), UNO_QUERY );
            if ( xModelSet.is() )
                xModelSet->setPropertyValue( _rPropName, _rValue );
        }
        catch( const Exception& )
        {
            DBG_ERROR( "lcl_initProperty: caught an exception!" );
        }
    }
}

IMPL_LINK(FmFormObjFactory, MakeObject, SdrObjFactory*, pObjFactory)
{
    if (pObjFactory->nInventor == FmFormInventor)
    {
        switch (pObjFactory->nIdentifier)
        {
            case OBJ_FM_CONTROL:    // allgemeines Object
            {
                pObjFactory->pNewObj = new FmFormObj(pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_EDIT:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_EDIT,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_BUTTON:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_COMMANDBUTTON,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_FIXEDTEXT:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_FIXEDTEXT,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_LISTBOX:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_LISTBOX,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_CHECKBOX:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_CHECKBOX,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_RADIOBUTTON:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_RADIOBUTTON,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_GROUPBOX:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_GROUPBOX,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_COMBOBOX:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_COMBOBOX,pObjFactory->nIdentifier);
                lcl_initProperty( static_cast< FmFormObj* >( pObjFactory->pNewObj ), FM_PROP_DROPDOWN, makeAny( sal_True ) );

            }   break;
            case OBJ_FM_GRID:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_GRID,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_IMAGEBUTTON:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_IMAGEBUTTON,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_FILECONTROL:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_FILECONTROL,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_DATEFIELD:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_DATEFIELD,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_TIMEFIELD:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_TIMEFIELD,pObjFactory->nIdentifier);
                lcl_initProperty( static_cast< FmFormObj* >( pObjFactory->pNewObj ), FM_PROP_TIMEMAX, makeAny( (sal_Int32)( Time( 23, 59, 59, 99 ).GetTime() ) ) );
            }   break;
            case OBJ_FM_NUMERICFIELD:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_NUMERICFIELD,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_CURRENCYFIELD:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_CURRENCYFIELD,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_PATTERNFIELD:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_PATTERNFIELD,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_HIDDEN:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_HIDDEN,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_IMAGECONTROL:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_IMAGECONTROL,pObjFactory->nIdentifier);
            }   break;
            case OBJ_FM_FORMATTEDFIELD:
            {
                pObjFactory->pNewObj = new FmFormObj(FM_COMPONENT_FORMATTEDFIELD,pObjFactory->nIdentifier);
            }   break;
            default:
                return 0;
        }
    }

    return 0;
}



