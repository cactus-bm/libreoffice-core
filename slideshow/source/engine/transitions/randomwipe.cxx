/*************************************************************************
 *
 *  $RCSfile: randomwipe.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-11-26 19:07:54 $
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
#include "randomwipe.hxx"
#include "basegfx/matrix/b2dhommatrix.hxx"
#include "basegfx/numeric/ftools.hxx"
#include <stdlib.h>


namespace presentation {
namespace internal {

RandomWipe::RandomWipe( sal_Int32 nElements, bool randomBars )
    : m_positions( new ::basegfx::B2DPoint[ nElements ] ),
      m_nElements( nElements ),
      m_rect( createUnitRect() )
{
    ::basegfx::B2DHomMatrix aTransform;
    if (randomBars)
    {
        double edge = (1.0 / nElements);
        for ( sal_Int32 pos = nElements; pos--; )
            m_positions[ pos ].setY( ::basegfx::pruneScaleValue( pos * edge ) );
        aTransform.scale( 1.0, ::basegfx::pruneScaleValue(edge) );
    }
    else // dissolve effect
    {
        sal_Int32 sqrtElements = static_cast<sal_Int32>(
            sqrt( static_cast<double>(nElements) ) );
        double edge = (1.0 / sqrtElements);
        for ( sal_Int32 pos = nElements; pos--; ) {
            m_positions[ pos ] = ::basegfx::B2DPoint(
                ::basegfx::pruneScaleValue( (pos % sqrtElements) * edge ),
                ::basegfx::pruneScaleValue( (pos / sqrtElements) * edge ) );
        }
        const double pedge = ::basegfx::pruneScaleValue(edge);
        aTransform.scale( pedge, pedge );
    }
    m_rect.transform( aTransform );

    // mix up:
    for ( sal_Int32 i = (nElements / 2); i--; )
    {
        sal_Int32 pos1 = (rand() * nElements / RAND_MAX);
        sal_Int32 pos2 = (rand() * nElements / RAND_MAX);
        ::basegfx::B2DPoint point( m_positions[ pos1 ] );
        m_positions[ pos1 ] = m_positions[ pos2 ];
        m_positions[ pos2 ] = point;
    }
}

::basegfx::B2DPolyPolygon RandomWipe::operator () ( double t )
{
    ::basegfx::B2DPolyPolygon res;
    for ( sal_Int32 pos = static_cast<sal_Int32>(t * m_nElements); pos--; )
    {
        ::basegfx::B2DHomMatrix aTransform;
        ::basegfx::B2DPoint const & point = m_positions[ pos ];
        aTransform.translate( point.getX(), point.getY() );
        ::basegfx::B2DPolygon poly( m_rect );
        poly.transform( aTransform );
        res.append( poly );
    }
    return res;
}

}
}
