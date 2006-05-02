/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: clview.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2006-05-02 15:07:27 $
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

#ifndef _SFXAPP_HXX
#include <sfx2/app.hxx>
#endif

#pragma hdrstop

#ifndef SD_CLIENT_VIEW_HXX
#include "ClientView.hxx"
#endif
#ifndef SD_DRAW_VIEW_HXX
#include "drawview.hxx"
#endif
#include "sdpage.hxx"

class DrawDocShell;

namespace sd {

class DrawViewShell;


/*************************************************************************
|*
|* Ctor: Die ClientView wird fuer DrawDocShell::Draw() verwendet
|*
\************************************************************************/

ClientView::ClientView(
    DrawDocShell* pDocSh,
    OutputDevice* pOutDev,
    DrawViewShell* pShell)
    : DrawView (pDocSh, pOutDev, pShell)
{
}


/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/

ClientView::~ClientView()
{
}

/*************************************************************************
|*
|* Wenn die View kein Invalidate() an den Fenstern durchfuehren soll, muss
|* man diese Methode ueberladen und entsprechend anders reagieren.
|*
\************************************************************************/

void ClientView::InvalidateOneWin(::Window& rWin)
{
    Region aRegion;
    CompleteRedraw(&rWin, aRegion);
}

/*************************************************************************
|*
|* Wenn die View kein Invalidate() an den Fenstern durchfuehren soll, muss
|* man diese Methode ueberladen und entsprechend anders reagieren.
|*
\************************************************************************/

void ClientView::InvalidateOneWin(::Window& rWin, const Rectangle& rRect)
{
    CompleteRedraw(&rWin, rRect);
}


/*************************************************************************
|*
|*
|*
\************************************************************************/

void ClientView::CompleteRedraw(OutputDevice* pOutDev, const Region& rReg)
{
    DrawView::CompleteRedraw(pOutDev, rReg);
}


} // end of namespace sd
