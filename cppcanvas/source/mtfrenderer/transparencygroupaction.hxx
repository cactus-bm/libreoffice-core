/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: transparencygroupaction.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2007-01-22 11:52:27 $
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

#ifndef _CPPCANVAS_TRANSPARENCYGROUPACTION_HXX
#define _CPPCANVAS_TRANSPARENCYGROUPACTION_HXX

#include <cppcanvas/canvas.hxx>
#include <cppcanvas/renderer.hxx>
#include <action.hxx>

#include <memory> // auto_ptr

namespace basegfx {
    class B2DPoint;
    class B2DVector;
}

class GDIMetaFile;
class Gradient;


/* Definition of internal::TransparencyGroupActionFactory class */

namespace cppcanvas
{
    namespace internal
    {
        struct OutDevState;

        typedef ::std::auto_ptr< GDIMetaFile >  MtfAutoPtr;
        typedef ::std::auto_ptr< Gradient >     GradientAutoPtr;

        /** Transparency group action.

            This action groups a bunch of other actions, to be
            rendered with the given transparency setting against the
            background.

            Creates encapsulated converters between GDIMetaFile and
            XCanvas. The Canvas argument is deliberately placed at the
            constructor, to force reconstruction of this object for a
            new canvas. This considerably eases internal state
            handling, since a lot of the internal state (e.g. fonts,
            text layout) is Canvas-dependent.
         */
        class TransparencyGroupActionFactory
        {
        public:
            /** Create new transparency group action.

                @param rGroupMtf
                Metafile that groups all actions to be rendered
                transparent

                @param rParms
                Render parameters

                @param rDstPoint
                Left, top edge of destination, in current state
                coordinate system

                @param rDstSize
                Size of the transparency group object, in current
                state coordinate system.

                @param nAlpha
                Alpha value, must be in the range [0,1]
             */
            static ActionSharedPtr createTransparencyGroupAction( MtfAutoPtr&                   rGroupMtf,
                                                                  const Renderer::Parameters&   rParms,
                                                                  const ::basegfx::B2DPoint&    rDstPoint,
                                                                  const ::basegfx::B2DVector&   rDstSize,
                                                                  double                        nAlpha,
                                                                  const CanvasSharedPtr&        rCanvas,
                                                                  const OutDevState&            rState );

            /** Create new transparency group action.

                @param rGroupMtf
                Metafile that groups all actions to be rendered
                transparent.

                @param rAlphaGradient
                VCL gradient, to be rendered into the action's alpha
                channel.

                @param rParms
                Render parameters

                @param rDstPoint
                Left, top edge of destination, in current state
                coordinate system

                @param rDstSize
                Size of the transparency group object, in current
                state coordinate system.
             */
            static ActionSharedPtr createTransparencyGroupAction( MtfAutoPtr&                   rGroupMtf,
                                                                  GradientAutoPtr&              rAlphaGradient,
                                                                  const Renderer::Parameters&   rParms,
                                                                  const ::basegfx::B2DPoint&    rDstPoint,
                                                                  const ::basegfx::B2DVector&   rDstSize,
                                                                  const CanvasSharedPtr&        rCanvas,
                                                                  const OutDevState&            rState );

        private:
            // static factory, disable big four
            TransparencyGroupActionFactory();
            ~TransparencyGroupActionFactory();
            TransparencyGroupActionFactory(const TransparencyGroupActionFactory&);
            TransparencyGroupActionFactory& operator=( const TransparencyGroupActionFactory& );
        };
    }
}

#endif /*_CPPCANVAS_TRANSPARENCYGROUPACTION_HXX */
