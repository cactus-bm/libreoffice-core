/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: RelativePositionHelper.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-22 18:21:43 $
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

#ifndef _CHART2_RELATIVEPOSITIONHELPER_HXX
#define _CHART2_RELATIVEPOSITIONHELPER_HXX

#ifndef _COM_SUN_STAR_CHART2_RELATIVEPOSITION_HPP_
#include <com/sun/star/chart2/RelativePosition.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_RELATIVESIZE_HPP_
#include <com/sun/star/chart2/RelativeSize.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_ALIGNMENT_HPP_
#include <com/sun/star/drawing/Alignment.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_POINT_HPP_
#include <com/sun/star/awt/Point.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_SIZE_HPP_
#include <com/sun/star/awt/Size.hpp>
#endif

//.............................................................................
namespace chart
{
//.............................................................................

//-----------------------------------------------------------------------------
/**
*/

class RelativePositionHelper
{
public:
    /** returns the upper left corner of an object that has size aObjectSize and
        where the point indicated by aAnchor has coordinates indicated by aPoint
        ( e.g. if aAnchor equals BOTTOM_LEFT, aPoint describes the
        coordinates of the bottom left corner of an object with size aObjectSize )
     */
    static ::com::sun::star::awt::Point
        getUpperLeftCornerOfAnchoredObject(
            ::com::sun::star::awt::Point aPoint,
            ::com::sun::star::awt::Size aObjectSize,
            ::com::sun::star::drawing::Alignment aAnchor );

    /** returns the center of an object that has size aObjectSize and
        where the point indicated by aAnchor has coordinates indicated by aPoint
        ( e.g. if aAnchor equals BOTTOM_LEFT, aPoint describes the
        coordinates of the bottom left corner of an object with size aObjectSize )
     */
    static ::com::sun::star::awt::Point
        getCenterOfAnchoredObject(
            ::com::sun::star::awt::Point aPoint,
            ::com::sun::star::awt::Size aUnrotatedObjectSize,
            ::com::sun::star::drawing::Alignment aAnchor,
            double fAnglePi );

    /** Returns a relative position that is the same point after the anchor has
        been changed to the given one.  The passed object size is taken into
        account for shifting the position.
     */
    static ::com::sun::star::chart2::RelativePosition
        getReanchoredPosition(
            const ::com::sun::star::chart2::RelativePosition & rPosition,
            const ::com::sun::star::chart2::RelativeSize & rObjectSize,
            ::com::sun::star::drawing::Alignment aNewAnchor );

    /** grows a relative size about the given amount and shifts the given
        position such that the resize is relative to the former rectangle's
        center.

        @param bCheck If </TRUE>, the resize is only done, if after
               transformation, the position and size are within the bounds [0,1].

        @return </TRUE>, if changes were applied.

        <p>That means, if the position's alignment is center, the position will
        not change at all.</p>
     */
    static bool centerGrow(
        ::com::sun::star::chart2::RelativePosition & rInOutPosition,
        ::com::sun::star::chart2::RelativeSize & rInOutSize,
        double fAmountX, double fAmountY,
        bool bCheck = true );

    /** shifts a relative position about the given amount

        @param bCheck If </TRUE>, the shift is only done, if after
               transformation, the object represented by the position
               rInOutPosition and its size rObjectSize the position and size are
               within the bounds [0,1].

        @return </TRUE>, if changes were applied.
     */
    static bool moveObject(
        ::com::sun::star::chart2::RelativePosition & rInOutPosition,
        const ::com::sun::star::chart2::RelativeSize & rObjectSize,
        double fAmountX, double fAmountY,
        bool bCheck = true );
};

//.............................................................................
} //namespace chart
//.............................................................................
#endif
