/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: contwnd.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 16:56:31 $
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

#ifndef _CONTWND_HXX
#define _CONTWND_HXX

#ifndef _TL_POLY_HXX
#include <tools/poly.hxx>
#endif

#ifndef _GRAPHCTL_HXX
#include <svx/graphctl.hxx>
#endif

/*************************************************************************
|*
|*
|*
\************************************************************************/

class ContourWindow : public GraphCtrl
{
    PolyPolygon         aPolyPoly;
    Color               aPipetteColor;
    Rectangle           aWorkRect;
    Link                aPipetteLink;
    Link                aPipetteClickLink;
    Link                aWorkplaceClickLink;
    BOOL                bPipetteMode;
    BOOL                bWorkplaceMode;
    BOOL                bClickValid;

protected:

    virtual void        MouseButtonDown(const MouseEvent& rMEvt);
    virtual void        MouseMove(const MouseEvent& rMEvt);
    virtual void        MouseButtonUp(const MouseEvent& rMEvt);
    virtual void        SdrObjCreated( const SdrObject& rObj );
    virtual void        InitSdrModel();
    virtual void        Paint( const Rectangle& rRect );

    void                CreatePolyPolygon();

public:

                        ContourWindow( Window* pParent, const ResId& rResId );
                        ~ContourWindow();

    void                SetPolyPolygon( const PolyPolygon& rPolyPoly );
    const PolyPolygon&  GetPolyPolygon();

    void                SetPipetteMode( const BOOL bPipette ) { bPipetteMode = bPipette; }
    BOOL                IsPipetteMode() const { return bPipetteMode; }
    const Color&        GetPipetteColor() const { return aPipetteColor; }

    BOOL                IsClickValid() const { return bClickValid; }
    BOOL                IsContourChanged() const;

    void                SetWorkplaceMode( const BOOL bWorkplace ) { bWorkplaceMode = bWorkplace; }
    BOOL                IsWorkplaceMode() const { return bWorkplaceMode; }
    const Rectangle&    GetWorkRect() const { return aWorkRect; }

    void                SetPipetteHdl( const Link& rLink ) { aPipetteLink = rLink; }
    void                SetPipetteClickHdl( const Link& rLink ) { aPipetteClickLink = rLink; }

    void                SetWorkplaceClickHdl( const Link& rLink ) { aWorkplaceClickLink = rLink; }
};


#endif

