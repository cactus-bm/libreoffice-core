/*************************************************************************
 *
 *  $RCSfile: fudraw.hxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 16:48:38 $
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

#ifndef _SD_FUDRAW_HXX
#define _SD_FUDRAW_HXX

#ifndef _SV_POINTR_HXX //autogen
#include <vcl/pointr.hxx>
#endif

#ifndef _SD_FUPOOR_HXX
#include "fupoor.hxx"
#endif

struct SdrViewEvent;
class SdrObject;


/*************************************************************************
|*
|* Basisklasse fuer alle Drawmodul-spezifischen Funktionen
|*
\************************************************************************/

class FuDraw : public FuPoor
{
 protected:
    Pointer aNewPointer;
    Pointer aOldPointer;
    BOOL    bMBDown;
    BOOL    bDragHelpLine;
    USHORT  nHelpLine;
    BOOL    bPermanent;

 public:
    TYPEINFO();

    FuDraw(SdViewShell* pViewSh, SdWindow* pWin, SdView* pView,
            SdDrawDocument* pDoc, SfxRequest& rReq);
    virtual ~FuDraw();

    virtual BOOL KeyInput(const KeyEvent& rKEvt);
    virtual BOOL MouseMove(const MouseEvent& rMEvt);
    virtual BOOL MouseButtonUp(const MouseEvent& rMEvt);
    virtual BOOL MouseButtonDown(const MouseEvent& rMEvt);
    virtual BOOL Command(const CommandEvent& rCEvt);
    virtual BOOL RequestHelp(const HelpEvent& rHEvt);

    virtual void ScrollStart();
    virtual void ScrollEnd();

    virtual void Activate();
    virtual void Deactivate();

    virtual void ForcePointer(const MouseEvent* pMEvt = NULL);

    virtual void DoubleClick(const MouseEvent& rMEvt);

    BOOL    SetPointer(SdrObject* pObj, const Point& rPos);
    BOOL    SetHelpText(SdrObject* pObj, const Point& rPos, const SdrViewEvent& rVEvt);

    void    SetPermanent(BOOL bSet) { bPermanent = bSet; }
};



#endif      // _SD_FUDRAW_HXX
