/*************************************************************************
 *
 *  $RCSfile: polypolyaction.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: thb $ $Date: 2004-03-18 10:41:06 $
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

#include "polypolyaction.hxx"
#include "outdevstate.hxx"

#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif
#ifndef _VCL_CANVASTOOLS_HXX
#include <vcl/canvastools.hxx>
#endif

#ifndef _BGFX_TOOLS_CANVASTOOLS_HXX
#include <basegfx/tools/canvastools.hxx>
#endif
#ifndef _CANVAS_CANVASTOOLS_HXX
#include <canvas/canvastools.hxx>
#endif

#include "mtftools.hxx"


using namespace ::com::sun::star;
using namespace ::drafts::com::sun::star;

namespace cppcanvas
{
    namespace internal
    {
        PolyPolyAction::PolyPolyAction( const ::PolyPolygon&    rPolyPoly,
                                        const CanvasSharedPtr&  rCanvas,
                                        const OutDevState&      rState ) :
            mxPolyPoly( ::vcl::unotools::xPolyPolygonFromPolyPolygon( rCanvas->getUNOCanvas()->getDevice(),
                                                                      rPolyPoly ) ),
            mpCanvas( rCanvas ),
            maState(),
            maFillColor(),
            maStrokeColor(),
            mbFill( rState.isFillColorSet ),
            mbStroke( rState.isLineColorSet )
        {
            tools::initRenderState(maState,rState);

            if( mbFill )
                maFillColor = rState.fillColor;

            if( mbStroke )
                maStrokeColor = rState.lineColor;
        }

        PolyPolyAction::PolyPolyAction( const ::PolyPolygon&    rPolyPoly,
                                        const CanvasSharedPtr&  rCanvas,
                                        const OutDevState&      rState,
                                        Mode                    mode        ) :
            mxPolyPoly( ::vcl::unotools::xPolyPolygonFromPolyPolygon( rCanvas->getUNOCanvas()->getDevice(),
                                                                      rPolyPoly ) ),
            mpCanvas( rCanvas ),
            maState(),
            maFillColor(),
            maStrokeColor(),
            mbFill( false ),
            mbStroke( rState.isLineColorSet )
        {
            tools::initRenderState(maState,rState);

            if( mbStroke )
                maStrokeColor = rState.lineColor;
        }

        PolyPolyAction::PolyPolyAction( const ::PolyPolygon&    rPolyPoly,
                                        const CanvasSharedPtr&  rCanvas,
                                        const OutDevState&      rState,
                                        int                     nTransparency ) :
            mxPolyPoly( ::vcl::unotools::xPolyPolygonFromPolyPolygon( rCanvas->getUNOCanvas()->getDevice(),
                                                                      rPolyPoly ) ),
            mpCanvas( rCanvas ),
            maState(),
            maFillColor(),
            maStrokeColor(),
            mbFill( rState.isFillColorSet ),
            mbStroke( rState.isLineColorSet )
        {
            tools::initRenderState(maState,rState);

            if( mbFill )
            {
                maFillColor = rState.fillColor;

                // TODO: Color management
                // adapt fill color transparency
                maFillColor[3] = 1.0 - nTransparency / 100.0;
            }

            if( mbStroke )
            {
                maStrokeColor = rState.lineColor;

                // TODO: Color management
                // adapt fill color transparency
                maStrokeColor[3] = 1.0 - nTransparency / 100.0;
            }
        }

        PolyPolyAction::~PolyPolyAction()
        {
        }

        bool PolyPolyAction::render() const
        {
            if( mbFill )
            {
                // don't want to have full maState mutable, thus
                // selectively casting away const here.
                const_cast<PolyPolyAction*>(this)->maState.DeviceColor = maFillColor;

                // TODO: implement caching
                mpCanvas->getUNOCanvas()->fillPolyPolygon( mxPolyPoly,
                                                           mpCanvas->getViewState(),
                                                           maState );
            }

            if( mbStroke )
            {
                // don't want to have full maState mutable, thus
                // selectively casting away const here.
                const_cast<PolyPolyAction*>(this)->maState.DeviceColor = maStrokeColor;

                // TODO: implement caching
                mpCanvas->getUNOCanvas()->drawPolyPolygon( mxPolyPoly,
                                                           mpCanvas->getViewState(),
                                                           maState );
            }

            return true;
        }

    }
}
