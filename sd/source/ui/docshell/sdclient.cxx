/*************************************************************************
 *
 *  $RCSfile: sdclient.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2004-01-20 10:54:37 $
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

#include "Client.hxx"

#ifndef _IPOBJ_HXX //autogen
#include <so3/ipobj.hxx>
#endif

#ifndef _SVDOOLE2_HXX //autogen
#include <svx/svdoole2.hxx>
#endif
#ifndef _SVDOGRAF_HXX //autogen
#include <svx/svdograf.hxx>
#endif
#ifndef _CLIENT_HXX //autogen
#include <so3/client.hxx>
#endif
#ifndef _IPENV_HXX //autogen
#include <so3/ipenv.hxx>
#endif
#ifndef _SVDPAGV_HXX
#include <svx/svdpagv.hxx>
#endif

#pragma hdrstop

#include "misc.hxx"

#ifdef STARIMAGE_AVAILABLE
#ifndef _SIMDLL_HXX
#include <sim2/simdll.hxx>
#endif
#endif

#include "strings.hrc"

#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
#ifndef SD_VIEW_HXX
#include "View.hxx"
#endif
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif
#include "sdresid.hxx"

namespace sd {

/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/

Client::Client(SdrOle2Obj* pObj, ViewShell* pViewShell, ::Window* pWindow) :
    SfxInPlaceClient(pViewShell->GetViewShell(), pWindow),
    pViewShell(pViewShell),
    pSdrOle2Obj(pObj),
    pSdrGrafObj(NULL),
    pOutlinerParaObj (NULL)
{
}

/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/

Client::~Client()
{
}


/*************************************************************************
|*
|* Wenn IP-aktiv, dann kommt diese Anforderung um Vergroesserung des
|* sichtbaren Ausschnitts des Objektes
|*
\************************************************************************/

void Client::RequestObjAreaPixel(const Rectangle& rRect)
{
    ::Window* pWin = pViewShell->GetWindow();
    Rectangle aObjRect( pWin->PixelToLogic( rRect.TopLeft() ),
                        pWin->PixelToLogic( rRect.GetSize() ) );

    ::sd::View* pView = pViewShell->GetView();
    Rectangle aWorkArea( pView->GetWorkArea() );

    if (!aWorkArea.IsInside(aObjRect))
    {
        // Position korrigieren
        Point aPos = aObjRect.TopLeft();
        Size  aSize = aObjRect.GetSize();
        Point aWorkAreaTL = aWorkArea.TopLeft();
        Point aWorkAreaBR = aWorkArea.BottomRight();

        aPos.X() = Max(aPos.X(), aWorkAreaTL.X());
        aPos.X() = Min(aPos.X(), aWorkAreaBR.X()-aSize.Width());
        aPos.Y() = Max(aPos.Y(), aWorkAreaTL.Y());
        aPos.Y() = Min(aPos.Y(), aWorkAreaBR.Y()-aSize.Height());

        aObjRect.SetPos(aPos);

        SfxInPlaceClient::RequestObjAreaPixel(pViewShell->GetWindow()->
                                              LogicToPixel(aObjRect) );
    }
    else
    {
        SfxInPlaceClient::RequestObjAreaPixel(rRect);
    }

    const SdrMarkList& rMarkList = pView->GetMarkList();

    if (rMarkList.GetMarkCount() == 1)
    {
        SdrMark* pMark = rMarkList.GetMark(0);
        SdrObject* pObj = pMark->GetObj();

        Rectangle aOldRect( pObj->GetLogicRect() );

        if ( aObjRect != aOldRect )
        {
            // Rundungsfehler vermeiden - nur, wenn mindestens 1 Pixel Unterschied
            // (getrennt fuer Position und Groesse)
            Size aOnePixel = pWin->PixelToLogic( Size(1, 1) );
            Size aLogicSize = aObjRect.GetSize();
            Rectangle aNewRect = aOldRect;
            Size aNewSize = aNewRect.GetSize();

            if ( Abs( aObjRect.Left() - aOldRect.Left() ) >= aOnePixel.Width() )
                aNewRect.SetPos( Point( aObjRect.Left(), aNewRect.Top() ) );
            if ( Abs( aObjRect.Top() - aOldRect.Top() ) >= aOnePixel.Height() )
                aNewRect.SetPos( Point( aNewRect.Left(), aObjRect.Top() ) );

            if ( Abs( aLogicSize.Width() - aNewSize.Width() ) >= aOnePixel.Width() )
                aNewSize.Width() = aLogicSize.Width();
            if ( Abs( aLogicSize.Height() - aNewSize.Height() ) >= aOnePixel.Height() )
                aNewSize.Height() = aLogicSize.Height();

            aNewRect.SetSize( aNewSize );

            if ( aNewRect != aOldRect )     // veraendert nur, wenn mindestens 1 Pixel
                pObj->SetLogicRect( aNewRect );
        }
    }
}



/*************************************************************************
|*
|*
|*
\************************************************************************/

void Client::ViewChanged(USHORT nAspect)
{
    // Eventuell neues MetaFile holen
    SfxInPlaceClient::ViewChanged(nAspect);

    if (pViewShell->GetActiveWindow())
    {
        ::sd::View* pView = pViewShell->GetView();

        if (pView)
        {
            SvClientData* pData = GetEnv();

            if( pData )
            {
                SvEmbeddedObject*   pObj = GetEmbedObj();
                MapMode             aMap100( MAP_100TH_MM );
                Rectangle           aVisArea( OutputDevice::LogicToLogic( pObj->GetVisArea(), pObj->GetMapUnit(), aMap100 ) );
                Rectangle           aLogicRect( pSdrOle2Obj->GetLogicRect() );
                Size                aScaledSize( static_cast< long >( pData->GetScaleWidth() * Fraction( aVisArea.GetWidth() ) ),
                                                 static_cast< long >( pData->GetScaleHeight() * Fraction( aVisArea.GetHeight() ) ) );

                if( Application::GetDefaultDevice()->LogicToPixel( aScaledSize, aMap100 ) !=
                    Application::GetDefaultDevice()->LogicToPixel( aLogicRect.GetSize(), aMap100 ) )
                {
                    const sal_Bool bOldLock = pView->GetModel()->isLocked();

                    pView->GetModel()->setLock( sal_True );
                    pSdrOle2Obj->SetLogicRect( Rectangle( aLogicRect.TopLeft(), aScaledSize ) );
                    pView->GetModel()->setLock( bOldLock );

                    pSdrOle2Obj->BroadcastObjectChange();
                }
            }
        }
    }
}


/*************************************************************************
|*
|* InPlace-Objekt aktivieren / deaktivieren
|*
\************************************************************************/

void Client::UIActivate(BOOL bActivate)
{
    SfxInPlaceClient::UIActivate(bActivate);

    if (!bActivate)
    {
#ifdef STARIMAGE_AVAILABLE
        if (pSdrGrafObj && pViewShell->GetActiveWindow())
        {
            // Das Ole2Obj (Image) wird gegen das GrafObj ausgetauscht
            pSdrGrafObj->SetLogicRect(pSdrOle2Obj->GetLogicRect());
            SvInPlaceObjectRef aIPObj = pSdrOle2Obj->GetObjRef();
            pSdrGrafObj->SetGraphic ( SimDLL::GetGraphic( aIPObj ) );
            SdView* pView = pViewShell->GetView();
            SdrPageView* pPV = pView->GetPageViewPvNum(0);
            SdrPage* pPg = pPV->GetPage();
            delete pPg->RemoveObject( pSdrOle2Obj->GetOrdNum() );
            pSdrGrafObj = NULL;
        }
#endif
    }
}

/*************************************************************************
|*
|* Daten fuer eine ggf. spaeter zu erzeugende View
|*
\************************************************************************/

void Client::MakeViewData()
{
    SfxInPlaceClient::MakeViewData();

    SvClientData* pCD = GetClientData();

    if (pCD)
    {
        SvEmbeddedObject* pObj = GetEmbedObj();
        Rectangle aObjVisArea = OutputDevice::LogicToLogic(
                                    pObj->GetVisArea(), pObj->GetMapUnit(),
                                    MAP_100TH_MM );
        Size aVisSize = aObjVisArea.GetSize();
        Fraction aFractX = pCD->GetScaleWidth();
        Fraction aFractY = pCD->GetScaleHeight();
        aFractX *= aVisSize.Width();
        aFractY *= aVisSize.Height();
        pCD->SetSizeScale(aFractX, aFractY);

        Rectangle aObjArea = pSdrOle2Obj->GetLogicRect();
        pCD->SetObjArea(aObjArea);
    }
}

/*************************************************************************
|*
|* Objekt in den sichtbaren Breich scrollen
|*
\************************************************************************/

void Client::MakeVisible()
{
    SfxInPlaceClient::MakeVisible();

    if (pViewShell->ISA(DrawViewShell))
    {
        static_cast<DrawViewShell*>(pViewShell)->MakeVisible(
            pSdrOle2Obj->GetLogicRect(),
            *pViewShell->GetActiveWindow());
    }
}


} // end of namespace sd
