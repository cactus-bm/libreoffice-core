/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fuconcs.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-12-14 17:08:52 $
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

#ifndef SD_FU_CONSTRUCT_CUSTOMSHAPE_HXX
#define SD_FU_CONSTRUCT_CUSTOMSHAPE_HXX

#ifndef SD_FU_CONSTRUCT_HXX
#include "fuconstr.hxx"
#endif

class KeyEvent;
class SdDrawDocument;
class SdrObject;
class SdxRequest;
class SfxItemSet;

namespace sd {

/*************************************************************************
|*
|* Rechteck zeichnen
|*
\************************************************************************/

class FuConstructCustomShape
    : public FuConstruct
{
public:
    TYPEINFO();

    static FunctionReference Create( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* pView, SdDrawDocument* pDoc, SfxRequest& rReq, bool bPermanent );
    virtual void DoExecute( SfxRequest& rReq );

    // Mouse- & Key-Events
    virtual BOOL KeyInput(const KeyEvent& rKEvt);
    virtual BOOL MouseMove(const MouseEvent& rMEvt);
    virtual BOOL MouseButtonUp(const MouseEvent& rMEvt);
    virtual BOOL MouseButtonDown(const MouseEvent& rMEvt);

    virtual void Activate();           // Function aktivieren

    void SetAttributes( SdrObject* pObj );

    // #97016#
    virtual SdrObject* CreateDefaultObject(const sal_uInt16 nID, const Rectangle& rRectangle);

    // #i33136#
    virtual bool doConstructOrthogonal() const;

protected:
    FuConstructCustomShape (
        ViewShell* pViewSh,
        ::sd::Window* pWin,
        ::sd::View* pView,
        SdDrawDocument* pDoc,
        SfxRequest& rReq);

private:
    rtl::OUString aCustomShape;
};

} // end of namespace sd

#endif

