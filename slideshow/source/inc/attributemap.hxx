/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: attributemap.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2007-07-17 15:03:35 $
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

#ifndef INCLUDED_SLIDESHOW_ATTRIBUTEMAP_HXX
#define INCLUDED_SLIDESHOW_ATTRIBUTEMAP_HXX

#include <rtl/ustring.hxx>

/* Definition of AttributeMap helper function */

namespace slideshow
{
    namespace internal
    {
        /** Type of to-be-animated attribute.

            This enum describes the type of an animated
            attribute.
        */
        enum AttributeType
        {
            ATTRIBUTE_INVALID,
            ATTRIBUTE_CHAR_COLOR,
            ATTRIBUTE_CHAR_FONT_NAME,
            ATTRIBUTE_CHAR_HEIGHT,
            ATTRIBUTE_CHAR_POSTURE,
            ATTRIBUTE_CHAR_ROTATION,
            ATTRIBUTE_CHAR_UNDERLINE,
            ATTRIBUTE_CHAR_WEIGHT,
            ATTRIBUTE_COLOR,
            ATTRIBUTE_DIMCOLOR,
            ATTRIBUTE_FILL_COLOR,
            ATTRIBUTE_FILL_STYLE,
            ATTRIBUTE_HEIGHT,
            ATTRIBUTE_LINE_COLOR,
            ATTRIBUTE_LINE_STYLE,
            ATTRIBUTE_OPACITY,
            ATTRIBUTE_ROTATE,
            ATTRIBUTE_SKEW_X,
            ATTRIBUTE_SKEW_Y,
            ATTRIBUTE_VISIBILITY,
            ATTRIBUTE_WIDTH,
            ATTRIBUTE_POS_X,
            ATTRIBUTE_POS_Y
        };

        /** Map attribute name to AttributeType enum

            @returns ATTRIBUTE_INVALID, if name was not found in the
            mapping table.
         */
        AttributeType mapAttributeName( const ::rtl::OUString& rAttrName );
    }
}

#endif /* INCLUDED_SLIDESHOW_ATTRIBUTEMAP_HXX */
