/*************************************************************************
 *
 *  $RCSfile: pointaction.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-03-30 08:31:20 $
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

#ifndef _CPPCANVAS_POINTACTION_HXX
#define _CPPCANVAS_POINTACTION_HXX

#include <action.hxx>
#include <cppcanvas/canvas.hxx>

class Point;
class Color;


/* Definition of internal::PointActionFactory class */

namespace cppcanvas
{
    namespace internal
    {
        struct OutDevState;

        /** Creates encapsulated converters between GDIMetaFile and
            XCanvas. The Canvas argument is deliberately placed at the
            constructor, to force reconstruction of this object for a
            new canvas. This considerably eases internal state
            handling, since a lot of the internal state (e.g. fonts,
            text layout) is Canvas-dependent.
         */
        class PointActionFactory
        {
        public:
            /// Point in current color
            static ActionSharedPtr createPointAction( const ::Point&,
                                                      const CanvasSharedPtr&,
                                                      const OutDevState& );

            /// Point in given color
            static ActionSharedPtr createPointAction( const ::Point&,
                                                      const CanvasSharedPtr&,
                                                      const OutDevState&,
                                                      const ::Color&        );

        private:
            // static factory, disable big four
            PointActionFactory();
            ~PointActionFactory();
            PointActionFactory(const PointActionFactory&);
            PointActionFactory& operator=( const PointActionFactory& );
        };
    }
}

#endif /* _CPPCANVAS_POINTACTION_HXX */
