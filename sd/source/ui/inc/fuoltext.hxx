/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fuoltext.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-12-14 17:14:02 $
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

#ifndef SD_FU_OUTLINE_TEXT_HXX
#define SD_FU_OUTLINE_TEXT_HXX

#ifndef SD_FU_OUTLINE_HXX
#include "fuoutl.hxx"
#endif

class SdDrawDocument;
class SfxRequest;

namespace sd {

class View;
class ViewShell;
class Window;

/*************************************************************************
|*
|* Textfunktionen im Outlinermodus
|*
\************************************************************************/
class FuOutlineText
    : public FuOutline
{
public:
    TYPEINFO();

    static FunctionReference Create( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* pView, SdDrawDocument* pDoc, SfxRequest& rReq );

    virtual BOOL KeyInput(const KeyEvent& rKEvt);
    virtual BOOL MouseMove(const MouseEvent& rMEvt);
    virtual BOOL MouseButtonUp(const MouseEvent& rMEvt);
    virtual BOOL MouseButtonDown(const MouseEvent& rMEvt);

    virtual void DoCut();
    virtual void DoCopy();
    virtual void DoPaste();

    virtual void Activate();           // Function aktivieren
    virtual void Deactivate();         // Function deaktivieren

    /** Call this method when the text in the outliner (may) has changed.
        It will invalidate some slots of the view frame and update the
        preview in the slide sorter.
    */
    void UpdateForKeyPress (const KeyEvent& rEvent);

protected:
    FuOutlineText (
        ViewShell* pViewShell,
        ::sd::Window* pWin,
        ::sd::View* pView,
        SdDrawDocument* pDoc,
        SfxRequest& rReq);

};

} // end of namespace sd

#endif
