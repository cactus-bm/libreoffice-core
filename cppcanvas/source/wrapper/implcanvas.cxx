/*************************************************************************
 *
 *  $RCSfile: implcanvas.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: thb $ $Date: 2004-03-18 10:41:10 $
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

#include "implcanvas.hxx"

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif
#ifndef _BGFX_POLYGON_B2DPOLYPOLYGON_HXX
#include <basegfx/polygon/b2dpolypolygon.hxx>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_RENDERING_XCANVAS_HPP_
#include <drafts/com/sun/star/rendering/XCanvas.hpp>
#endif

#include <canvas/canvastools.hxx>

#include <cppcanvas/polypolygon.hxx>
#include "implfont.hxx"
#include "implcolor.hxx"


using namespace ::drafts::com::sun::star;
using namespace ::com::sun::star;

namespace cppcanvas
{
    namespace internal
    {

        ImplCanvas::ImplCanvas( const uno::Reference< rendering::XCanvas >& xCanvas ) :
            maViewState(),
            mpClipPolyPolygon(),
            mxCanvas( xCanvas )
        {
            OSL_ENSURE( mxCanvas.is(), "Canvas::Canvas() invalid XCanvas" );

            ::canvas::tools::initViewState( maViewState );
        }

        ImplCanvas::~ImplCanvas()
        {
        }

        void ImplCanvas::setTransformation( const ::basegfx::B2DHomMatrix& rMatrix )
        {
            ::canvas::tools::setViewStateTransform( maViewState, rMatrix );
        }

        ::basegfx::B2DHomMatrix ImplCanvas::getTransformation() const
        {
            ::basegfx::B2DHomMatrix aMatrix;
            return ::canvas::tools::getViewStateTransform( aMatrix,
                                                           maViewState );
        }

        void ImplCanvas::setClip( const PolyPolygonSharedPtr& rClipPoly )
        {
            mpClipPolyPolygon = rClipPoly;
            maViewState.Clip = rClipPoly.get() != NULL ? rClipPoly->getUNOPolyPolygon() : NULL;
        }

        PolyPolygonSharedPtr ImplCanvas::getClip() const
        {
            return mpClipPolyPolygon;
        }

        FontSharedPtr ImplCanvas::createFont( const ::rtl::OUString& rFontName, const double& rCellSize ) const
        {
            return FontSharedPtr( new ImplFont( getUNOCanvas(), rFontName, rCellSize ) );
        }

        ColorSharedPtr ImplCanvas::createColor() const
        {
            return ColorSharedPtr( new ImplColor( getUNOCanvas()->getDevice() ) );
        }

        CanvasSharedPtr ImplCanvas::cloneCanvas() const
        {
            return CanvasSharedPtr( new ImplCanvas( *this ) );
        }

        uno::Reference< rendering::XCanvas > ImplCanvas::getUNOCanvas() const
        {
            OSL_ENSURE( mxCanvas.is(), "ImplCanvas::getUNOCanvas(): Invalid XCanvas" );

            return mxCanvas;
        }

        rendering::ViewState ImplCanvas::getViewState() const
        {
            return maViewState;
        }

    }
}
