/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: vclhelpergradient.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: aw $ $Date: 2007-03-06 12:34:17 $
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

#ifndef INCLUDED_DRAWINGLAYER_PROCESSOR2D_VCLHELPERGRADIENT_HXX
#define INCLUDED_DRAWINGLAYER_PROCESSOR2D_VCLHELPERGRADIENT_HXX

#ifndef INCLUDED_DRAWINGLAYER_ATTRIBUTE_FILLATTRIBUTE_HXX
#include <drawinglayer/attribute/fillattribute.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////
// predefines

class OutputDevice;
namespace basegfx {
    class B2DPolyPolygon;
    class BColor;
}

//////////////////////////////////////////////////////////////////////////////
// support methods for vcl direct gradient renderering

namespace drawinglayer
{
    void impDrawGradientToOutDev(
        OutputDevice& rOutDev,
        const basegfx::B2DPolyPolygon& rTargetForm,
        attribute::GradientStyle eGradientStyle,
        sal_uInt32 nSteps,
        const basegfx::BColor& rStart,
        const basegfx::BColor& rEnd,
        double fBorder, double fAngle, double fOffsetX, double fOffsetY, bool bSimple);
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif // INCLUDED_DRAWINGLAYER_PROCESSOR2D_VCLHELPERGRADIENT_HXX

// eof
