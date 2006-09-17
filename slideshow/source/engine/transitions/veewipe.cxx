/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: veewipe.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 08:43:10 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_slideshow.hxx"

#include "canvas/debug.hxx"
#include "veewipe.hxx"
#include "basegfx/matrix/b2dhommatrix.hxx"
#include <basegfx/point/b2dpoint.hxx>


namespace presentation {
namespace internal {

::basegfx::B2DPolyPolygon VeeWipe::operator () ( double t )
{
    ::basegfx::B2DPolygon poly;
    poly.append( ::basegfx::B2DPoint( 0.0, -1.0 ) );
    const double d = ::basegfx::pruneScaleValue( 2.0 * t );
    poly.append( ::basegfx::B2DPoint( 0.0, d - 1.0 ) );
    poly.append( ::basegfx::B2DPoint( 0.5, d ) );
    poly.append( ::basegfx::B2DPoint( 1.0, d - 1.0 ) );
    poly.append( ::basegfx::B2DPoint( 1.0, -1.0 ) );
    poly.setClosed(true);
    return ::basegfx::B2DPolyPolygon( poly );
}

}
}
