/*************************************************************************
 *
 *  $RCSfile: polypolyaction.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: thb $ $Date: 2004-03-18 10:41:06 $
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

#ifndef _CPPCANVAS_POLYPOLYACTION_HXX
#define _CPPCANVAS_POLYPOLYACTION_HXX

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_RENDERING_RENDERSTATE_HPP__
#include <drafts/com/sun/star/rendering/RenderState.hpp>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_RENDERING_XPOLYPOLYGON2D_HPP__
#include <drafts/com/sun/star/rendering/XPolyPolygon2D.hpp>
#endif

#include "action.hxx"
#include <cppcanvas/canvas.hxx>

class PolyPolygon;
class Color;

/* Definition of internal::PolyPolyAction class */

namespace cppcanvas
{
    namespace internal
    {
        struct OutDevState;

        class PolyPolyAction : public Action
        {
        public:
            enum Mode
            {
                /// regardless of the state, only stroke polygon (if line color is set, that is)
                strokeOnly
            };

            PolyPolyAction( const ::PolyPolygon&,
                            const CanvasSharedPtr&,
                            const OutDevState&  );
            PolyPolyAction( const ::PolyPolygon&,
                            const CanvasSharedPtr&,
                            const OutDevState&,
                            Mode                );
            /// For transparent painting of the given polygon (normally, we take the colors always opaque)
            PolyPolyAction( const ::PolyPolygon&,
                            const CanvasSharedPtr&,
                            const OutDevState&,
                            int                 nTransparency );
            virtual ~PolyPolyAction();

            virtual bool render() const;

        private:
            // default: disabled copy/assignment
            PolyPolyAction(const PolyPolyAction&);
            PolyPolyAction& operator = ( const PolyPolyAction& );

            ::com::sun::star::uno::Reference<
                ::drafts::com::sun::star::rendering::XPolyPolygon2D >   mxPolyPoly;
            CanvasSharedPtr                                             mpCanvas;
            ::drafts::com::sun::star::rendering::RenderState            maState;

            ::com::sun::star::uno::Sequence< double >                   maFillColor;
            ::com::sun::star::uno::Sequence< double >                   maStrokeColor;
            bool                                                        mbFill;
            bool                                                        mbStroke;
        };
    }
}

#endif /* _CPPCANVAS_POLYPOLYACTION_HXX */
