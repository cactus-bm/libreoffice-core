/*************************************************************************
 *
 *  $RCSfile: b3dpoint.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: thb $ $Date: 2004-02-16 17:03:05 $
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

#ifndef _BGFX_POINT_B3DPOINT_HXX
#define _BGFX_POINT_B3DPOINT_HXX

#ifndef _BGFX_TUPLE_B3DTUPLE_HXX
#include <basegfx/tuple/b3dtuple.hxx>
#endif

namespace basegfx
{
    // predeclaration
    class B3DHomMatrix;

    /** Base Point class with three double values

        This class derives all operators and common handling for
        a 3D data class from B3DTuple. All necessary extensions
        which are special for points will be added here.

        @see B3DTuple
    */
    class B3DPoint : public ::basegfx::B3DTuple
    {
    public:
        /** Create a 3D Point

            The point is initialized to (0.0, 0.0, 0.0)
        */
        B3DPoint()
        :   B3DTuple()
        {}

        /** Create a 3D Point

            @param fX
            This parameter is used to initialize the X-coordinate
            of the 3D Point.

            @param fY
            This parameter is used to initialize the Y-coordinate
            of the 3D Point.

            @param fZ
            This parameter is used to initialize the Z-coordinate
            of the 3D Point.
        */
        B3DPoint(double fX, double fY, double fZ)
        :   B3DTuple(fX, fY, fZ)
        {}

        /** Create a copy of a 3D Point

            @param rVec
            The 3D Point which will be copied.
        */
        B3DPoint(const B3DPoint& rVec)
        :   B3DTuple(rVec)
        {}

        /** constructor with tuple to allow copy-constructing
            from B3DTuple-based classes
        */
        B3DPoint(const ::basegfx::B3DTuple& rTuple)
        :   B3DTuple(rTuple)
        {}

        ~B3DPoint()
        {}

        /** *=operator to allow usage from B3DPoint, too
        */
        B3DPoint& operator*=( const B3DPoint& rPnt )
        {
            mfX *= rPnt.mfX;
            mfY *= rPnt.mfY;
            mfZ *= rPnt.mfZ;
            return *this;
        }

        /** *=operator to allow usage from B3DPoint, too
        */
        B3DPoint& operator*=(double t)
        {
            mfX *= t;
            mfY *= t;
            mfZ *= t;
            return *this;
        }

        /** assignment operator to allow assigning the results
            of B3DTuple calculations
        */
        B3DPoint& operator=( const ::basegfx::B3DTuple& rVec )
        {
            mfX = rVec.getX();
            mfY = rVec.getY();
            mfZ = rVec.getZ();
            return *this;
        }

        /** Transform point by given transformation matrix.

            The translational components of the matrix are, in
            contrast to B3DVector, applied.
        */
        B3DPoint& operator*=( const ::basegfx::B3DHomMatrix& rMat );

        static const B3DPoint& getEmptyPoint()
        {
            return (const B3DPoint&) ::basegfx::B3DTuple::getEmptyTuple();
        }
    };
} // end of namespace basegfx

#endif /* _BGFX_POINT_B3DPOINT_HXX */
