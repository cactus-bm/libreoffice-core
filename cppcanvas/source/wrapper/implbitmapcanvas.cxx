/*************************************************************************
 *
 *  $RCSfile: implbitmapcanvas.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: thb $ $Date: 2004-03-18 10:41:09 $
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

#ifndef _DRAFTS_COM_SUN_STAR_RENDERING_XCANVAS_HPP__
#include <drafts/com/sun/star/rendering/XCanvas.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_RENDERING_XBITMAPCANVAS_HPP__
#include <drafts/com/sun/star/rendering/XBitmapCanvas.hpp>
#endif

#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif
#ifndef _BGFX_TOOLS_CANVASTOOLS_HXX
#include <basegfx/tools/canvastools.hxx>
#endif

#include "implbitmapcanvas.hxx"


using namespace ::drafts::com::sun::star;
using namespace ::com::sun::star;

namespace cppcanvas
{
    namespace internal
    {
        ImplBitmapCanvas::ImplBitmapCanvas( const uno::Reference< rendering::XBitmapCanvas >& rCanvas ) :
            ImplCanvas( uno::Reference< rendering::XCanvas >(rCanvas,
                                                             uno::UNO_QUERY) ),
            mxBitmapCanvas( rCanvas ),
            mxBitmap( rCanvas,
                      uno::UNO_QUERY )
        {
            OSL_ENSURE( mxBitmapCanvas.is(), "ImplBitmapCanvas::ImplBitmapCanvas(): Invalid canvas" );
            OSL_ENSURE( mxBitmap.is(), "ImplBitmapCanvas::ImplBitmapCanvas(): Invalid bitmap" );
        }

        ImplBitmapCanvas::~ImplBitmapCanvas()
        {
        }

        ::basegfx::B2ISize ImplBitmapCanvas::getSize() const
        {
            OSL_ENSURE( mxBitmap.is(), "ImplBitmapCanvas::getSize(): Invalid canvas" );
            return ::basegfx::unotools::b2ISizeFromIntegerSize2D( mxBitmap->getSize() );
        }

        BitmapCanvasSharedPtr ImplBitmapCanvas::cloneBitmapCanvas() const
        {
            return BitmapCanvasSharedPtr( new ImplBitmapCanvas( *this ) );
        }
    }
}
