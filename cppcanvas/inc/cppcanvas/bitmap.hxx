/*************************************************************************
 *
 *  $RCSfile: bitmap.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: vg $ $Date: 2005-03-10 13:21:18 $
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

#ifndef _CPPCANVAS_BITMAP_HXX
#define _CPPCANVAS_BITMAP_HXX

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif

#ifndef BOOST_SHARED_PTR_HPP_INCLUDED
#include <boost/shared_ptr.hpp>
#endif

#ifndef _CPPCANVAS_CANVASGRAPHIC_HXX
#include <cppcanvas/canvasgraphic.hxx>
#endif
#ifndef _CPPCANVAS_BITMAPCANVAS_HXX
#include <cppcanvas/bitmapcanvas.hxx>
#endif

namespace com { namespace sun { namespace star { namespace rendering
{
    class  XBitmap;
} } } }


/* Definition of Bitmap interface */

namespace cppcanvas
{

    /** This interface defines a Bitmap canvas object

        Consider this object part of the view, and not of the model
        data, as this bitmap can only be painted on its parent canvas
     */
    class Bitmap : public virtual CanvasGraphic
    {
    public:
        /** Render to parent canvas, with global alpha.

            This method renders the content to the parent canvas,
            i.e. the canvas this object was constructed for.

            @param nAlphaModulation
            Global alpha value, with which each pixel alpha value gets
            multiplied. For a normal, opaque bitmap, this will make
            the bitmap appear transparent with the given alpha value
            (value must be in the range [0,1]).

            @return whether the rendering finished successfully.
         */
        virtual bool                        drawAlphaModulated( double nAlphaModulation ) const = 0;

        virtual BitmapCanvasSharedPtr       getBitmapCanvas() const = 0;

        virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::rendering::XBitmap >  getUNOBitmap() const = 0;
    };

    typedef ::boost::shared_ptr< ::cppcanvas::Bitmap > BitmapSharedPtr;
}

#endif /* _CPPCANVAS_BITMAP_HXX */
