/*************************************************************************
 *
 *  $RCSfile: dlgedobj.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: tbe $ $Date: 2001-03-03 16:20:36 $
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

#ifndef _BASCTL_DLGEDOBJ_HXX
#include "dlgedobj.hxx"
#endif

#ifndef _SVX_FMPROP_HXX
//#include "fmprop.hxx"
#endif

#ifndef _TOOLS_RESMGR_HXX
#include <tools/resmgr.hxx>
#endif

#ifndef _SVX_FMPROP_HRC
//#include "fmprop.hrc"
#endif

#ifndef _SVDIO_HXX //autogen
#include <svx/svdio.hxx>
#endif

#ifndef _SVX_FMTOOLS_HXX
//#include "fmtools.hxx"
#endif

#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif

#ifndef _STRING_HXX
//#include <tools/string.hxx>
#endif

#ifndef _SVX_FMRESIDS_HRC
//#include <svx/fmresids.hrc>
#endif

#ifndef _SVX_FMGLOB_HXX
#include <svx/fmglob.hxx>
#endif

#ifndef _BASCTL_DLGED_HXX
#include "dlged.hxx"
#endif

#ifndef _BASCTL_DLGEDMOD_HXX
#include "dlgedmod.hxx"
#endif

#ifndef _BASCTL_DLGEDPAGE_HXX
#include "dlgedpage.hxx"
#endif

#ifndef _BASCTL_DLGEDVIEW_HXX
#include "dlgedview.hxx"
#endif

#ifndef _BASCTL_DLGEDLIST_HXX
#include "dlgedlist.hxx"
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#include "vcsbxdef.hxx"

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::rtl;

//----------------------------------------------------------------------------

TYPEINIT1(DlgEdObj, SdrUnoObj);
DBG_NAME(DlgEdObj);

//----------------------------------------------------------------------------

DlgEdObj::DlgEdObj()
          :SdrUnoObj(String(), sal_False)
          ,bIsListening(sal_False)
{
    DBG_CTOR(DlgEdObj, NULL);
}

//----------------------------------------------------------------------------

DlgEdObj::DlgEdObj(const ::rtl::OUString& rModelName)
          :SdrUnoObj(rModelName, sal_False)
          ,bIsListening(sal_False)
{
    DBG_CTOR(DlgEdObj, NULL);
}

//----------------------------------------------------------------------------

DlgEdObj::DlgEdObj(const ::rtl::OUString& rModelName,
                   const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rxSFac)
          :SdrUnoObj(rModelName, rxSFac, sal_False)
          ,bIsListening(sal_False)
{
    DBG_CTOR(DlgEdObj, NULL);
}

//----------------------------------------------------------------------------

DlgEdObj::~DlgEdObj()
{
    DBG_DTOR(DlgEdObj, NULL);

    EndPropertyListening();
}

//----------------------------------------------------------------------------

void DlgEdObj::SetPage(SdrPage* _pNewPage)
{
    // now set the page
    SdrUnoObj::SetPage(_pNewPage);
}

//----------------------------------------------------------------------------

void DlgEdObj::SetRectFromProps()
{
    // DIRTY HACK!!!
    // I need the window borders from MT
    //
    // window borders in pixel
    sal_Int32 nLeftBorder_px = 3;
    sal_Int32 nRightBorder_px = 3;
    sal_Int32 nTopBorder_px = 22;
    sal_Int32 nBottomBorder_px = 3;

    if ( !ISA(DlgEdForm) )
    {
        // get property sets
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >  xPSet(GetUnoControlModel(), ::com::sun::star::uno::UNO_QUERY);
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >  xPSetForm(GetDlgEdForm()->GetUnoControlModel(), ::com::sun::star::uno::UNO_QUERY);

        if (xPSet.is() && xPSetForm.is())
        {
            // get control position and size from properties
            sal_Int32 nX, nY, nWidth, nHeight;
            xPSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PositionX" ) ) ) >>= nX;
            xPSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PositionY" ) ) ) >>= nY;
            xPSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Width" ) ) ) >>= nWidth;
            xPSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Height" ) ) ) >>= nHeight;

            // get form position and size from properties
            sal_Int32 nFormX, nFormY, nFormWidth, nFormHeight;
            xPSetForm->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PositionX" ) ) ) >>= nFormX;
            xPSetForm->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PositionY" ) ) ) >>= nFormY;
            xPSetForm->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Width" ) ) ) >>= nFormWidth;
            xPSetForm->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Height" ) ) ) >>= nFormHeight;

            Size aPos( nX, nY );
            Size aSize( nWidth, nHeight );
            Size aFormPos( nFormX, nFormY );
            Size aFormSize( nFormWidth, nFormHeight );

            // convert logic units to pixel
            OutputDevice* pDevice = Application::GetDefaultDevice();
            DBG_ASSERT( pDevice, "Missing Default Device!" );
            if( pDevice )
            {
                aPos = pDevice->LogicToPixel( aPos, MapMode(MAP_APPFONT) );
                aSize = pDevice->LogicToPixel( aSize, MapMode(MAP_APPFONT) );
                aFormPos = pDevice->LogicToPixel( aFormPos, MapMode(MAP_APPFONT) );
                aFormSize = pDevice->LogicToPixel( aFormSize, MapMode(MAP_APPFONT) );
            }

            // take window borders into account
            aPos.Width() += aFormPos.Width() + nLeftBorder_px;
            aPos.Height() += aFormPos.Height() + nTopBorder_px;

            // convert pixel to 100th_mm
            if( pDevice )
            {
                aPos = pDevice->PixelToLogic( aPos, MapMode(MAP_100TH_MM) );
                aSize = pDevice->PixelToLogic( aSize, MapMode(MAP_100TH_MM) );
            }

            Point aPoint;
            aPoint.X() = aPos.Width();
            aPoint.Y() = aPos.Height();

            // set rectangle position and size
            SetSnapRect( Rectangle( aPoint, aSize ) );
        }
    }
    else if ( ISA(DlgEdForm) )
    {
        // get property set
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >  xPSetForm(GetUnoControlModel(), ::com::sun::star::uno::UNO_QUERY);

        if (xPSetForm.is())
        {
            // get form position and size from properties
            sal_Int32 nFormX, nFormY, nFormWidth, nFormHeight;
            xPSetForm->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PositionX" ) ) ) >>= nFormX;
            xPSetForm->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PositionY" ) ) ) >>= nFormY;
            xPSetForm->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Width" ) ) ) >>= nFormWidth;
            xPSetForm->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Height" ) ) ) >>= nFormHeight;

            Size aFormPos( nFormX, nFormY );
            Size aFormSize( nFormWidth, nFormHeight );

            // convert logic units to pixel
            OutputDevice* pDevice = Application::GetDefaultDevice();
            DBG_ASSERT( pDevice, "Missing Default Device!" );
            if( pDevice )
            {
                aFormPos = pDevice->LogicToPixel( aFormPos, MapMode(MAP_APPFONT) );
                aFormSize = pDevice->LogicToPixel( aFormSize, MapMode(MAP_APPFONT) );
            }

            // take window borders into account
            aFormSize.Width() += nLeftBorder_px + nRightBorder_px;
            aFormSize.Height() += nTopBorder_px + nBottomBorder_px;

            // convert pixel to 100th_mm
            if( pDevice )
            {
                aFormPos = pDevice->PixelToLogic( aFormPos, MapMode(MAP_100TH_MM) );
                aFormSize = pDevice->PixelToLogic( aFormSize, MapMode(MAP_100TH_MM) );
            }

            Point aFormPoint;
            aFormPoint.X() = aFormPos.Width();
            aFormPoint.Y() = aFormPos.Height();

            // set rectangle position and size
            SetSnapRect( Rectangle( aFormPoint, aFormSize ) );
        }
    }

    /* old method
    //
    // get property set
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >  xPSet(GetUnoControlModel(), ::com::sun::star::uno::UNO_QUERY);
    if (xPSet.is())
    {
        // get position and size from properties
        sal_Int32 nX, nY, nWidth, nHeight;
        xPSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PositionX" ) ) ) >>= nX;
        xPSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PositionY" ) ) ) >>= nY;
        xPSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Width" ) ) ) >>= nWidth;
        xPSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Height" ) ) ) >>= nHeight;

        Size aPos( nX, nY );
        Size aSize( nWidth, nHeight );

        // convert logic units
        OutputDevice* pDevice = Application::GetDefaultDevice();
        DBG_ASSERT( pDevice, "Missing Default Device!" );
        if( pDevice )
        {
            aPos = pDevice->LogicToPixel( aPos, MapMode(MAP_APPFONT) );
            aPos = pDevice->PixelToLogic( aPos, MapMode(MAP_100TH_MM) );
            aSize = pDevice->LogicToPixel( aSize, MapMode(MAP_APPFONT) );
            aSize = pDevice->PixelToLogic( aSize, MapMode(MAP_100TH_MM) );
        }

        Point aPoint;
        aPoint.X() = aPos.Width();
        aPoint.Y() = aPos.Height();

        if ( !ISA(DlgEdForm) )
        {
            Point aFormPos = (GetDlgEdForm()->GetSnapRect()).TopLeft();
            aPoint += aFormPos;
        }

        // set rectangle position and size
        SetSnapRect( Rectangle( aPoint, aSize ) );
    }
    */
}

//----------------------------------------------------------------------------

void DlgEdObj::SetPropsFromRect()
{
    // DIRTY HACK!!!
    // I need the window borders from MT
    //
    // window borders in pixel
    sal_Int32 nLeftBorder_px = 3;
    sal_Int32 nRightBorder_px = 3;
    sal_Int32 nTopBorder_px = 22;
    sal_Int32 nBottomBorder_px = 3;

    if ( !ISA(DlgEdForm) )
    {
        EndPropertyListening(sal_False);

        // get control property set
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >  xPSet(GetUnoControlModel(), ::com::sun::star::uno::UNO_QUERY);
        if (xPSet.is())
        {
            // get control position and size from rectangle
            Rectangle aRect = GetSnapRect();
            Point aPoint = aRect.TopLeft();
            Size aSize = aRect.GetSize();
            Size aPos;
            aPos.Width() = aPoint.X();
            aPos.Height() = aPoint.Y();

            // get form position and size from rectangle
            Rectangle aFormRect = GetDlgEdForm()->GetSnapRect();
            Point aFormPoint = aFormRect.TopLeft();
            Size aFormSize = aFormRect.GetSize();
            Size aFormPos;
            aFormPos.Width() = aFormPoint.X();
            aFormPos.Height() = aFormPoint.Y();

            // convert 100th_mm to pixel
            OutputDevice* pDevice = Application::GetDefaultDevice();
            DBG_ASSERT( pDevice, "Missing Default Device!" );
            if( pDevice )
            {
                aPos = pDevice->LogicToPixel( aPos, MapMode(MAP_100TH_MM) );
                aSize = pDevice->LogicToPixel( aSize, MapMode(MAP_100TH_MM) );
                aFormPos = pDevice->LogicToPixel( aFormPos, MapMode(MAP_100TH_MM) );
                aFormSize = pDevice->LogicToPixel( aFormSize, MapMode(MAP_100TH_MM) );
            }

            // take window borders into account
            aPos.Width() -= aFormPos.Width() + nLeftBorder_px;
            aPos.Height() -= aFormPos.Height() + nTopBorder_px;

            // convert pixel to logic units
            if( pDevice )
            {
                aPos = pDevice->PixelToLogic( aPos, MapMode(MAP_APPFONT) );
                aSize = pDevice->PixelToLogic( aSize, MapMode(MAP_APPFONT) );
            }

            // set properties
            Any aValue;
            aValue <<= aPos.Width();
            xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "PositionX" ) ), aValue );
            aValue <<= aPos.Height();
            xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "PositionY" ) ), aValue );
            aValue <<= aSize.Width();
            xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Width" ) ), aValue );
            aValue <<= aSize.Height();
            xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Height" ) ), aValue );
        }

        StartPropertyListening();
    }
    else if ( ISA(DlgEdForm) )
    {
        EndPropertyListening(sal_False);

        // get control property set
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >  xPSetForm(GetUnoControlModel(), ::com::sun::star::uno::UNO_QUERY);
        if (xPSetForm.is())
        {
            // get form position and size from rectangle
            Rectangle aFormRect = GetSnapRect();
            Point aFormPoint = aFormRect.TopLeft();
            Size aFormSize = aFormRect.GetSize();
            Size aFormPos;
            aFormPos.Width() = aFormPoint.X();
            aFormPos.Height() = aFormPoint.Y();

            // convert 100th_mm to pixel
            OutputDevice* pDevice = Application::GetDefaultDevice();
            DBG_ASSERT( pDevice, "Missing Default Device!" );
            if( pDevice )
            {
                aFormPos = pDevice->LogicToPixel( aFormPos, MapMode(MAP_100TH_MM) );
                aFormSize = pDevice->LogicToPixel( aFormSize, MapMode(MAP_100TH_MM) );
            }

            // take window borders into account
            aFormSize.Width() -= nLeftBorder_px + nRightBorder_px;
            aFormSize.Height() -= nTopBorder_px + nBottomBorder_px;

            // convert pixel to logic units
            if( pDevice )
            {
                aFormPos = pDevice->PixelToLogic( aFormPos, MapMode(MAP_APPFONT) );
                aFormSize = pDevice->PixelToLogic( aFormSize, MapMode(MAP_APPFONT) );
            }

            // set properties
            Any aValue;
            aValue <<= aFormPos.Width();
            xPSetForm->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "PositionX" ) ), aValue );
            aValue <<= aFormPos.Height();
            xPSetForm->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "PositionY" ) ), aValue );
            aValue <<= aFormSize.Width();
            xPSetForm->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Width" ) ), aValue );
            aValue <<= aFormSize.Height();
            xPSetForm->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Height" ) ), aValue );
        }

        StartPropertyListening();
    }

    /* old method
    //
    EndPropertyListening(sal_False);

    // get property set
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >  xPSet(GetUnoControlModel(), ::com::sun::star::uno::UNO_QUERY);
    if (xPSet.is())
    {
        // get position and size from rectangle
        Rectangle aRect = GetSnapRect();
        Point aPoint = aRect.TopLeft();
        Size aSize = aRect.GetSize();

        if ( !ISA(DlgEdForm) )
        {
            Point aFormPos = (GetDlgEdForm()->GetSnapRect()).TopLeft();
            aPoint -= aFormPos;
        }

        Size aPos;
        aPos.Width() = aPoint.X();
        aPos.Height() = aPoint.Y();

        // convert logic units
        OutputDevice* pDevice = Application::GetDefaultDevice();
        DBG_ASSERT( pDevice, "Missing Default Device!" );
        if( pDevice )
        {
            aPos = pDevice->LogicToPixel( aPos, MapMode(MAP_100TH_MM) );
            aPos = pDevice->PixelToLogic( aPos, MapMode(MAP_APPFONT) );
            aSize = pDevice->LogicToPixel( aSize, MapMode(MAP_100TH_MM) );
            aSize = pDevice->PixelToLogic( aSize, MapMode(MAP_APPFONT) );
        }

        // set properties
        Any aValue;
        aValue <<= aPos.Width();
        xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "PositionX" ) ), aValue );
        aValue <<= aPos.Height();
        xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "PositionY" ) ), aValue );
        aValue <<= aSize.Width();
        xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Width" ) ), aValue );
        aValue <<= aSize.Height();
        xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Height" ) ), aValue );
    }

    StartPropertyListening();
    */
}

//----------------------------------------------------------------------------

void SAL_CALL DlgEdObj::SetNameFromProp( const  ::com::sun::star::beans::PropertyChangeEvent& evt ) throw( ::com::sun::star::uno::RuntimeException)
{
    if ( !ISA(DlgEdForm) )
    {
        // get old name
        ::rtl::OUString aOldName;
        evt.OldValue >>= aOldName;

        // get new name
        ::rtl::OUString aNewName;
        evt.NewValue >>= aNewName;

        // remove the control by the old name and insert the control by the new name in the container
        uno::Reference< container::XNameAccess > xNameAcc((GetDlgEdForm()->GetUnoControlModel()), uno::UNO_QUERY);
        if ( xNameAcc.is() && xNameAcc->hasByName(aOldName) )
        {
            uno::Reference< container::XNameContainer > xCont(xNameAcc, uno::UNO_QUERY );
            if ( xCont.is() )
            {
                uno::Reference< awt::XControlModel > xCtrl(GetUnoControlModel(), uno::UNO_QUERY);
                uno::Any aAny;
                aAny <<= xCtrl;
                xCont->removeByName( aOldName );
                xCont->insertByName( aNewName , aAny );
            }
        }
    }
}

//----------------------------------------------------------------------------

String DlgEdObj::GetDefaultName()
{
    String aStr = GetUnoControlTypeName();

    sal_Unicode pDelims[] = { '.' , 0 };
    xub_StrLen nPos = aStr.SearchCharBackward( pDelims );
    if( nPos != STRING_NOTFOUND )
    {
        aStr.Erase( 0, ++nPos );
    }

    return aStr;
}

//----------------------------------------------------------------------------

String DlgEdObj::GetUniqueName()
{
    String sUniqueName;
    uno::Reference< container::XNameAccess > xNameAcc((GetDlgEdForm()->GetUnoControlModel()), uno::UNO_QUERY);

    if ( xNameAcc.is() )
    {
        sal_Int32 n = 0;
        String sDefaultName = GetDefaultName();

        do
        {
            sUniqueName = sDefaultName + String::CreateFromInt32(++n);
        }   while (xNameAcc->hasByName(sUniqueName));
    }

    return sUniqueName;
}

//----------------------------------------------------------------------------

sal_uInt32 DlgEdObj::GetObjInventor()   const
{
    /*
    if( GetModel() && ((FmFormModel*)GetModel())->IsStreamingOldVersion() )
        return SdrInventor;
    return FmFormInventor;
    */
    return VCSbxInventor;
}

//----------------------------------------------------------------------------

sal_uInt16 DlgEdObj::GetObjIdentifier() const
{
    /*
    if( GetModel() && ((FmFormModel*)GetModel())->IsStreamingOldVersion() )
        return OBJ_RECT;
    return OBJ_FM_CONTROL;
    */
    return OBJ_FM_CONTROL;  // change this
}

//----------------------------------------------------------------------------

void DlgEdObj::clonedFrom(const DlgEdObj* _pSource)
{
}

//----------------------------------------------------------------------------

SdrObject* DlgEdObj::Clone() const
{
    SdrObject* pReturn = SdrUnoObj::Clone();

    DlgEdObj* pFormObject = PTR_CAST(DlgEdObj, pReturn);
    DBG_ASSERT(pFormObject != NULL, "DlgEdObj::Clone : invalid clone !");
    if (pFormObject)
        pFormObject->clonedFrom(this);

    return pReturn;
}

//----------------------------------------------------------------------------

SdrObject* DlgEdObj::Clone(SdrPage* _pPage, SdrModel* _pModel) const
{
    SdrObject* pReturn = SdrUnoObj::Clone(_pPage, _pModel);
    if (!pReturn)
        return pReturn;

    return pReturn;
}

//----------------------------------------------------------------------------

void DlgEdObj::operator= (const SdrObject& rObj)
{
    SdrUnoObj::operator= (rObj);

}

//----------------------------------------------------------------------------

void DlgEdObj::WriteData(SvStream& rOut) const
{
    /*
    FmFormModel* pModel = (FmFormModel*)GetModel();
    if( pModel && pModel->IsStreamingOldVersion() )
    {
        SdrLayerID nOld = GetLayer();
        ((FmFormObj*)this)->NbcSetLayer( pModel->GetControlExportLayerId( *this ) );
        SdrUnoObj::WriteData( rOut );
        ((FmFormObj*)this)->NbcSetLayer( nOld );
        return;
    }
    */
    SdrUnoObj::WriteData(rOut);
}

//----------------------------------------------------------------------------

void DlgEdObj::ReadData(const SdrObjIOHeader& rHead, SvStream& rIn)
{
    SdrUnoObj::ReadData(rHead,rIn);
}

//----------------------------------------------------------------------------

void DlgEdObj::NbcMove( const Size& rSize )
{
    SdrUnoObj::NbcMove( rSize );
    SetPropsFromRect();
}

//----------------------------------------------------------------------------

void DlgEdObj::NbcResize(const Point& rRef, const Fraction& xFract, const Fraction& yFract)
{
    SdrUnoObj::NbcResize( rRef, xFract, yFract );
    SetPropsFromRect();
}

//----------------------------------------------------------------------------

FASTBOOL DlgEdObj::EndCreate(SdrDragStat& rStat, SdrCreateCmd eCmd)
{
    sal_Bool bResult = SdrUnoObj::EndCreate(rStat, eCmd);

    // set parent form
    pDlgEdForm = ((DlgEdPage*)GetPage())->GetDlgEd()->GetDlgEdForm();

    // get unique name
    ::rtl::OUString aOUniqueName( GetUniqueName() );

    // set name property
    uno::Reference< beans::XPropertySet > xPSet( GetUnoControlModel(), uno::UNO_QUERY );
    uno::Any aUniqueName;
    aUniqueName <<= aOUniqueName;
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Name" ) ), aUniqueName );

    // set geometry properties
    SetPropsFromRect();

    // set labels
    String sDefaultName = GetDefaultName();
    if (sDefaultName.EqualsAscii("Button") ||
        sDefaultName.EqualsAscii("RadioButton") ||
        sDefaultName.EqualsAscii("CheckBox") ||
        sDefaultName.EqualsAscii("GroupBox") ||
        sDefaultName.EqualsAscii("FixedText") )
    {
        xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Label" ) ), aUniqueName );
    }

    // insert control model in dialog model
    uno::Reference< container::XNameContainer > xC((GetDlgEdForm()->GetUnoControlModel()), uno::UNO_QUERY);
    uno::Reference< awt::XControlModel > xCtrl(GetUnoControlModel(), uno::UNO_QUERY);
    uno::Any aAny;
    aAny <<= xCtrl;
    xC->insertByName( aOUniqueName , aAny );

    return bResult;
}

//----------------------------------------------------------------------------

IMPL_LINK(DlgEdObj, OnCreate, void*, EMPTYTAG)
{
    /*
    nEvent = 0;
    if (pTempView)
        pTempView->ObjectCreated(this);
    */
    return 0;
}

//----------------------------------------------------------------------------

void SAL_CALL DlgEdObj::_propertyChange( const  ::com::sun::star::beans::PropertyChangeEvent& evt ) throw( ::com::sun::star::uno::RuntimeException)
{
    if (isListening())
    {
        // set rectangle, if geometry information in the model changed
        if ( evt.PropertyName == ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Width")) ||
             evt.PropertyName == ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Height")) ||
             evt.PropertyName == ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PositionX")) ||
             evt.PropertyName == ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PositionY")) )
        {
            SetRectFromProps();
        }
        else if ( evt.PropertyName == ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Name")) )
        {
            SetNameFromProp(evt);
        }
    }
}

//----------------------------------------------------------------------------

void DlgEdObj::StartPropertyListening()
{
    DBG_ASSERT(!isListening(), "DlgEdObj::StartPropertyListening: already listening!");

    if (!isListening())
    {
        Reference< XPropertySet > xControlModel(GetUnoControlModel(), UNO_QUERY);

        if (!m_xListener.is() && xControlModel.is())
        {
            // create listener
            m_xListener = static_cast< ::com::sun::star::beans::XPropertyChangeListener*>( new DlgEdListenerImpl( (DlgEdObj*)this ) );

            // register listener to properties
            xControlModel->addPropertyChangeListener(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Width")), m_xListener);
            xControlModel->addPropertyChangeListener(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Height")), m_xListener);
            xControlModel->addPropertyChangeListener(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PositionX")), m_xListener);
            xControlModel->addPropertyChangeListener(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PositionY")), m_xListener);
            xControlModel->addPropertyChangeListener(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Name")), m_xListener);
        }

        bIsListening = sal_True;
    }
}

//----------------------------------------------------------------------------

void DlgEdObj::EndPropertyListening(sal_Bool bRemoveListener)
{
    DBG_ASSERT(isListening(), "DlgEdObj::EndPropertyListening: not listening currently!");

    if (isListening())
    {
        bIsListening = sal_False;

        if (bRemoveListener)
        {
            Reference< XPropertySet > xControlModel(GetUnoControlModel(), UNO_QUERY);
            if (m_xListener.is() && xControlModel.is())
            {
                // remove listener
                xControlModel->removePropertyChangeListener(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Width")), m_xListener);
                xControlModel->removePropertyChangeListener(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Height")), m_xListener);
                xControlModel->removePropertyChangeListener(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PositionX")), m_xListener);
                xControlModel->removePropertyChangeListener(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PositionY")), m_xListener);
                xControlModel->removePropertyChangeListener(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Name")), m_xListener);
            }

            m_xListener.clear();
        }
    }
}

//----------------------------------------------------------------------------

TYPEINIT1(DlgEdForm, DlgEdObj);
DBG_NAME(DlgEdForm);

//----------------------------------------------------------------------------

DlgEdForm::DlgEdForm(const ::rtl::OUString& rModelName)
          :DlgEdObj(rModelName)
{
    DBG_CTOR(DlgEdForm, NULL);
}

//----------------------------------------------------------------------------

DlgEdForm::DlgEdForm(const ::rtl::OUString& rModelName,
                    const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rxSFac)
          :DlgEdObj(rModelName, rxSFac)
{
    DBG_CTOR(DlgEdForm, NULL);
}

//----------------------------------------------------------------------------

DlgEdForm::DlgEdForm()
          :DlgEdObj()
{
    DBG_CTOR(DlgEdForm, NULL);
}

//----------------------------------------------------------------------------

DlgEdForm::~DlgEdForm()
{
    DBG_DTOR(DlgEdForm, NULL);
}

//----------------------------------------------------------------------------

SdrObject* DlgEdForm::CheckHit( const Point& rPnt, USHORT nTol,
    const SetOfByte*  pSet ) const
{
    Rectangle aROuter = aOutRect;
    aROuter.Left()   -= nTol;
    aROuter.Right()  += nTol;
    aROuter.Top()    -= nTol;
    aROuter.Bottom() += nTol;

    Rectangle aRInner = aOutRect;
    if( (aRInner.GetSize().Height() > (long)nTol*2) &&
        (aRInner.GetSize().Width()  > (long)nTol*2)    )
    {
        aRInner.Left()   += nTol;
        aRInner.Right()  -= nTol;
        aRInner.Top()    += nTol;
        aRInner.Bottom() -= nTol;
    }

    if( aROuter.IsInside( rPnt ) && !aRInner.IsInside( rPnt ) )
        return (SdrObject*)this;
    else
        return 0;
}

//----------------------------------------------------------------------------

FASTBOOL DlgEdForm::EndCreate(SdrDragStat& rStat, SdrCreateCmd eCmd)
{
    sal_Bool bResult = SdrUnoObj::EndCreate(rStat, eCmd);

    SetPropsFromRect();

    return bResult;
}

//----------------------------------------------------------------------------


