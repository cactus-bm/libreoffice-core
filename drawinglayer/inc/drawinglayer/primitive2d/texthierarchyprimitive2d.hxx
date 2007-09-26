/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: texthierarchyprimitive2d.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: aw $ $Date: 2007-09-26 11:36:28 $
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

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_TEXTHIERARCHYPRIMITIVE2D_HXX
#define INCLUDED_DRAWINGLAYER_PRIMITIVE2D_TEXTHIERARCHYPRIMITIVE2D_HXX

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_GROUPPRIMITIVE2D_HXX
#include <drawinglayer/primitive2d/groupprimitive2d.hxx>
#endif

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        // text format hierarchy helper class. It decomposes to it's
        // content, so all direct renderers may ignore it. If You need
        // to know more about line hierarchies You may react on it and
        // also need to take care that the source of data uses it.
        class TextHierarchyLinePrimitive2D : public GroupPrimitive2D
        {
        private:
        public:
            TextHierarchyLinePrimitive2D(const Primitive2DSequence& rChildren);

            // provide unique ID
            DeclPrimitrive2DIDBlock()
        };
    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        // see TextHierarchyLinePrimitive2D comment
        class TextHierarchyParagraphPrimitive2D : public GroupPrimitive2D
        {
        private:
        public:
            TextHierarchyParagraphPrimitive2D(const Primitive2DSequence& rChildren);

            // provide unique ID
            DeclPrimitrive2DIDBlock()
        };
    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        // see TextHierarchyLinePrimitive2D comment
        class TextHierarchyBlockPrimitive2D : public GroupPrimitive2D
        {
        private:
        public:
            TextHierarchyBlockPrimitive2D(const Primitive2DSequence& rChildren);

            // provide unique ID
            DeclPrimitrive2DIDBlock()
        };
    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        // type enum
        enum FieldType
        {
            FIELD_TYPE_COMMON,          // unspecified. If more info is needed for a FieldType,
                                        // create a new type and it's handling
            FIELD_TYPE_PAGE,            // uses "FIELD_SEQ_BEGIN;PageField" -> special handling
            FIELD_TYPE_URL              // uses URL as string -> special handling
        };

        // see TextHierarchyLinePrimitive2D comment.
        // Also: This type uses a type enum to transport the encapsulated field
        // type. Also added is a String which is type-dependent. E.g. for URL
        // fields, it contains the URL.
        class TextHierarchyFieldPrimitive2D : public GroupPrimitive2D
        {
        private:
            FieldType                               meType;
            rtl::OUString                           maString;

        public:
            TextHierarchyFieldPrimitive2D(
                const Primitive2DSequence& rChildren,
                const FieldType& rFieldType,
                const rtl::OUString& rString);

            // get data
            FieldType getType() const { return meType; }
            const rtl::OUString& getString() const { return maString; }

            // compare operator
            virtual bool operator==(const BasePrimitive2D& rPrimitive) const;

            // provide unique ID
            DeclPrimitrive2DIDBlock()
        };
    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_DRAWINGLAYER_PRIMITIVE2D_TEXTHIERARCHYPRIMITIVE2D_HXX

//////////////////////////////////////////////////////////////////////////////
// eof
