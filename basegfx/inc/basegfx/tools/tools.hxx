/*************************************************************************
 *
 *  $RCSfile: tools.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2005-01-13 18:00:02 $
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

#ifndef _BGFX_TOOLS_TOOLS_HXX
#define _BGFX_TOOLS_TOOLS_HXX

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

namespace basegfx
{
    class B2DPoint;
    class B2DRange;

    namespace tools
    {
        /** Liang-Barsky 2D line clipping algorithm

            This function clips a line given by two points against the
            given rectangle. The resulting line is returned in the
            given points.

            @param io_rStart
            Start point of the line. On return, contains the clipped
            start point.

            @param io_rEnd
            End point of the line. On return, contains the clipped
            end point.

            @param rClipRect
            The rectangle to clip against

            @return true, when at least part of the line is visible
            after the clip, false otherwise
         */
        bool liangBarskyClip2D( ::basegfx::B2DPoint&        io_rStart,
                                ::basegfx::B2DPoint&        io_rEnd,
                                const ::basegfx::B2DRange&  rClipRect );

        /** Expand given parallelogram, such that it extends beyond
            bound rect in a given direction.

            This method is useful when e.g. generating one-dimensional
            gradients, such as linear or axial gradients: those
            gradients vary only in one direction, the other has
            constant color. Most of the time, those gradients extends
            infinitely in the direction with the constant color, but
            practically, one always has a limiting bound rect into
            which the gradient is painted. The method at hand now
            extends a given parallelogram (e.g. the transformed
            bounding box of a gradient) virtually into infinity to the
            top and to the bottom (i.e. normal to the line io_rLeftTop
            io_rRightTop), such that the given rectangle is guaranteed
            to be covered in that direction.

            @attention There might be some peculiarities with this
            method, that might limit its usage to the described
            gradients. One of them is the fact that when determining
            how far the parallelogram has to be extended to the top or
            the bottom, the upper and lower border are assumed to be
            infinite lines.

            @param io_rLeftTop
            Left, top edge of the parallelogramm. Note that this need
            not be the left, top edge geometrically, it's just used
            when determining the extension direction. Thus, it's
            perfectly legal to affine-transform a rectangle, and given
            the transformed point here. On method return, this
            parameter will contain the adapted output.

            @param io_rLeftBottom
            Left, bottom edge of the parallelogramm. Note that this need
            not be the left, bottom edge geometrically, it's just used
            when determining the extension direction. Thus, it's
            perfectly legal to affine-transform a rectangle, and given
            the transformed point here. On method return, this
            parameter will contain the adapted output.

            @param io_rRightTop
            Right, top edge of the parallelogramm. Note that this need
            not be the right, top edge geometrically, it's just used
            when determining the extension direction. Thus, it's
            perfectly legal to affine-transform a rectangle, and given
            the transformed point here. On method return, this
            parameter will contain the adapted output.

            @param io_rRightBottom
            Right, bottom edge of the parallelogramm. Note that this need
            not be the right, bottom edge geometrically, it's just used
            when determining the extension direction. Thus, it's
            perfectly legal to affine-transform a rectangle, and given
            the transformed point here. On method return, this
            parameter will contain the adapted output.

            @param rFitTarget
            The rectangle to fit the parallelogram into.
         */
        void infiniteLineFromParallelogram( ::basegfx::B2DPoint&        io_rLeftTop,
                                            ::basegfx::B2DPoint&        io_rLeftBottom,
                                            ::basegfx::B2DPoint&        io_rRightTop,
                                            ::basegfx::B2DPoint&        io_rRightBottom,
                                            const ::basegfx::B2DRange&  rFitTarget  );

    }
}

#endif /* _BGFX_TOOLS_TOOLS_HXX */
