/*************************************************************************
 *
 *  $RCSfile: waterfallwipe.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-11-26 19:10:40 $
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

#include "canvas/debug.hxx"
#include "waterfallwipe.hxx"
#include "transitiontools.hxx"
#include "basegfx/matrix/b2dhommatrix.hxx"
#include "basegfx/point/b2dpoint.hxx"
#include "basegfx/numeric/ftools.hxx"


namespace presentation {
namespace internal {

WaterfallWipe::WaterfallWipe( sal_Int32 nElements, bool flipOnYAxis )
    : m_flipOnYAxis( flipOnYAxis )
{
    const sal_Int32 sqrtElements = static_cast<sal_Int32>(
        sqrt( static_cast<double>(nElements) ) );
    const double elementEdge = (1.0 / sqrtElements);
    m_waterfall.append( ::basegfx::B2DPoint( 0.0, -1.0 ) );
    for ( sal_Int32 pos = sqrtElements; pos--; )
    {
        const sal_Int32 xPos = (sqrtElements - pos - 1);
        const double yPos = ::basegfx::pruneScaleValue( ((pos + 1) * elementEdge) - 1.0 );
        m_waterfall.append( ::basegfx::B2DPoint(
                                ::basegfx::pruneScaleValue( xPos * elementEdge ),
                                yPos ) );
        m_waterfall.append( ::basegfx::B2DPoint(
                                ::basegfx::pruneScaleValue( (xPos + 1) * elementEdge ),
                                yPos ) );
    }
    m_waterfall.append( ::basegfx::B2DPoint( 1.0, -1.0 ) );
    m_waterfall.setClosed(true);
}

::basegfx::B2DPolyPolygon WaterfallWipe::operator () ( double t )
{
    ::basegfx::B2DPolygon poly( m_waterfall );
    ::basegfx::B2DHomMatrix aTransform;
    aTransform.translate( 0.0, ::basegfx::pruneScaleValue( 2.0 * t ) );
    poly.transform( aTransform );
    poly.setB2DPoint( 0, ::basegfx::B2DPoint( 0.0, -1.0 ) );
    poly.setB2DPoint( poly.count()-1, ::basegfx::B2DPoint( 1.0, -1.0 ) );

    return m_flipOnYAxis ? flipOnYAxis( ::basegfx::B2DPolyPolygon(poly) )
                         : ::basegfx::B2DPolyPolygon(poly);
}

}
}
