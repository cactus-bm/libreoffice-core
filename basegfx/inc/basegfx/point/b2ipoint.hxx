/*************************************************************************
 *
 *  $RCSfile: b2ipoint.hxx,v $
 *
 *  $Revision: 1.3 $
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

#ifndef _BGFX_POINT_B2IPOINT_HXX
#define _BGFX_POINT_B2IPOINT_HXX

#ifndef _BGFX_TUPLE_B2ITUPLE_HXX
#include <basegfx/tuple/b2ituple.hxx>
#endif

namespace basegfx
{
    // predeclaration
    class B2DHomMatrix;

    /** Base Point class with two sal_Int32 values

        This class derives all operators and common handling for
        a 2D data class from B2ITuple. All necessary extensions
        which are special for points will be added here.

        @see B2ITuple
    */
    class B2IPoint : public ::basegfx::B2ITuple
    {
    public:
        /** Create a 2D Point

            The point is initialized to (0, 0)
        */
        B2IPoint()
        :   B2ITuple()
        {}

        /** Create a 2D Point

            @param nX
            This parameter is used to initialize the X-coordinate
            of the 2D Point.

            @param nY
            This parameter is used to initialize the Y-coordinate
            of the 2D Point.
        */
        B2IPoint(sal_Int32 nX, sal_Int32 nY)
        :   B2ITuple(nX, nY)
        {}

        /** Create a copy of a 2D Point

            @param rPoint
            The 2D Point which will be copied.
        */
        B2IPoint(const B2IPoint& rPoint)
        :   B2ITuple(rPoint)
        {}

        /** constructor with tuple to allow copy-constructing
            from B2ITuple-based classes
        */
        B2IPoint(const ::basegfx::B2ITuple& rTuple)
        :   B2ITuple(rTuple)
        {}

        ~B2IPoint()
        {}

        /** *=operator to allow usage from B2IPoint, too
        */
        B2IPoint& operator*=( const B2IPoint& rPnt )
        {
            mnX *= rPnt.mnX;
            mnY *= rPnt.mnY;
            return *this;
        }

        /** *=operator to allow usage from B2IPoint, too
        */
        B2IPoint& operator*=(sal_Int32 t)
        {
            mnX *= t;
            mnY *= t;
            return *this;
        }

        /** assignment operator to allow assigning the results
            of B2ITuple calculations
        */
        B2IPoint& operator=( const ::basegfx::B2ITuple& rPoint );

        /** Transform point by given transformation matrix.

            The translational components of the matrix are, in
            contrast to B2DVector, applied.
        */
        B2IPoint& operator*=( const ::basegfx::B2DHomMatrix& rMat );

        static const B2IPoint& getEmptyPoint()
        {
            return (const B2IPoint&) ::basegfx::B2ITuple::getEmptyTuple();
        }
    };
} // end of namespace basegfx

#endif /* _BGFX_POINT_B2IPOINT_HXX */
