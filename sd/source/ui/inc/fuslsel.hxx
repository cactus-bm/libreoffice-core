/*************************************************************************
 *
 *  $RCSfile: fuslsel.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: ka $ $Date: 2001-09-24 13:20:01 $
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

#ifndef _SD_FUSLSEL_HXX
#define _SD_FUSLSEL_HXX

#ifndef _SD_FUSLID_HXX
#include "fuslid.hxx"
#endif

class SdSlideViewShell;
class SdWindow;
class SdSlideView;
class SdDrawDocument;
class Sound;

struct FSS_IsShowingEffectInfo
{
    BOOL bIsShowingEffect;          // TRUE while we show a fade effect one the slide view
    BOOL bDisposed;                 // TRUE if the FuSlideSelection was deleted during fade effect
};

class FuSlideSelection : public FuSlide
{
private:

    BOOL                        bSubstShown;
    BOOL                        bPageHit;
    List                        aSubstList;       // Liste mit Ertsatzdarstellungen
    Point                       aDragPos;             // hier wird die Seite angefasst
    BOOL                        bDragSelection;
    Point                       aDragSelRectAnchor;  // fester Punkt des Selektionsrechtecks
    Rectangle                   aDragSelRect;
    Point                       aPosOfInsertMarker;
    Sound*                      pSound;
    FSS_IsShowingEffectInfo*    pIsShowingEffectInfo;

    void                        DrawInsertMarker(BOOL bShow);
    Point                       CalcPosOfInsertMarker(const Point& rPoint);

    USHORT                      GetTargetPage(const Point& rPoint) const;
    BOOL                        MovePages(USHORT nTargetPage) const;

    void                        CreateSubst();
    void                        DeleteSubst();
    void                        DrawSubst() const;
    void                        ChangeSubstPos(const Point& rVector);
    void                        DrawDragSelectionRect() const;
    void                        ShowEffect(USHORT nPageNo);

                                DECL_LINK( DragSlideHdl, Timer* );

public:

                                TYPEINFO();

                                FuSlideSelection( SdSlideViewShell* pViewSh, SdWindow* pWin,
                                                  SdSlideView* pView, SdDrawDocument* pDoc,
                                                  SfxRequest& rReq );

    virtual                     ~FuSlideSelection();
                                                   // Mouse- & Key-Events
    virtual BOOL                KeyInput(const KeyEvent& rKEvt);
    virtual BOOL                MouseMove(const MouseEvent& rMEvt);
    virtual BOOL                MouseButtonUp(const MouseEvent& rMEvt);
    virtual BOOL                MouseButtonDown(const MouseEvent& rMEvt);
    virtual void                Paint(const Rectangle& rRect, SdWindow* pWin);

    virtual void                Activate();        // Function aktivieren
    virtual void                Deactivate();          // Function deaktivieren

    virtual void                ScrollStart();
    virtual void                ScrollEnd();

    BOOL                        IsShowingEffect() const { return pIsShowingEffectInfo && pIsShowingEffectInfo->bIsShowingEffect; }
};



#endif      // _SD_FUSLSEL_HXX

