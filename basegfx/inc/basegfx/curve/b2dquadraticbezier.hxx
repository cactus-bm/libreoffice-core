/*************************************************************************
 *
 *  $RCSfile: b2dquadraticbezier.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: thb $ $Date: 2004-01-16 10:33:49 $
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

#ifndef _BGFX_CURVE_B2DQUADRATICBEZIER_HXX
#define _BGFX_CURVE_B2DQUADRATICBEZIER_HXX

#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////

namespace basegfx
{
    class B2DQuadraticBezier
    {
        ::basegfx::B2DPoint                         maStartPoint;
        ::basegfx::B2DPoint                         maEndPoint;
        ::basegfx::B2DPoint                         maControlPoint;

    public:
        B2DQuadraticBezier();
        B2DQuadraticBezier(const B2DQuadraticBezier& rBezier);
        B2DQuadraticBezier(const ::basegfx::B2DPoint& rStart, const ::basegfx::B2DPoint& rEnd);
        B2DQuadraticBezier(const ::basegfx::B2DPoint& rStart,
            const ::basegfx::B2DPoint& rControlPoint, const ::basegfx::B2DPoint& rEnd);
        ~B2DQuadraticBezier();

        // assignment operator
        B2DQuadraticBezier& operator=(const B2DQuadraticBezier& rBezier);

        // compare operators
        bool operator==(const B2DQuadraticBezier& rBezier) const;
        bool operator!=(const B2DQuadraticBezier& rBezier) const;

        // test if control point is placed on the edge
        bool isBezier() const;

        // data interface
        ::basegfx::B2DPoint getStartPoint() const { return maStartPoint; }
        void setStartPoint(const ::basegfx::B2DPoint& rValue) { maStartPoint = rValue; }

        ::basegfx::B2DPoint getEndPoint() const { return maEndPoint; }
        void setEndPoint(const ::basegfx::B2DPoint& rValue) { maEndPoint = rValue; }

        ::basegfx::B2DPoint getControlPoint() const { return maControlPoint; }
        void setControlPoint(const ::basegfx::B2DPoint& rValue) { maControlPoint = rValue; }
    };
} // end of namespace basegfx

#endif //   _BGFX_CURVE_B2DQUADRATICBEZIER_HXX
