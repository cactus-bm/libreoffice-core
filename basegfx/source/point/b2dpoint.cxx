/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: b2dpoint.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: kz $ $Date: 2005-11-02 13:56:49 $
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

#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif

#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif

#ifndef _BGFX_NUMERIC_FTOOLS_HXX
#include <basegfx/numeric/ftools.hxx>
#endif

namespace basegfx
{
    B2DPoint& B2DPoint::operator=( const ::basegfx::B2DTuple& rPoint )
    {
        mfX = rPoint.getX();
        mfY = rPoint.getY();
        return *this;
    }

    B2DPoint& B2DPoint::operator*=( const ::basegfx::B2DHomMatrix& rMat )
    {
        double fTempX(
            rMat.get(0, 0) * mfX +
            rMat.get(0, 1) * mfY +
            rMat.get(0, 2));
        double fTempY(
            rMat.get(1, 0) * mfX +
            rMat.get(1, 1) * mfY +
            rMat.get(1, 2));

        if(!rMat.isLastLineDefault())
        {
            const double fOne(1.0);
            const double fTempM(
                rMat.get(2, 0) * mfX +
                rMat.get(2, 1) * mfY +
                rMat.get(2, 2));

            if(!fTools::equalZero(fTempM) && !fTools::equal(fOne, fTempM))
            {
                fTempX /= fTempM;
                fTempY /= fTempM;
            }
        }

        mfX = fTempX;
        mfY = fTempY;

        return *this;
    }

    B2DPoint operator*( const ::basegfx::B2DHomMatrix& rMat, const B2DPoint& rPoint )
    {
        B2DPoint aRes( rPoint );
        return aRes *= rMat;
    }
} // end of namespace basegfx

// eof
