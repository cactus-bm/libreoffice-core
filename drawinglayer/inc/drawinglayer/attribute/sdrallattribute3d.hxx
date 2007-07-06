/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sdrallattribute3d.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: aw $ $Date: 2007-07-06 13:38:51 $
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

#ifndef INCLUDED_DRAWINGLAYER_ATTRIBUTE_SDRALLATTRIBUTE3D_HXX
#define INCLUDED_DRAWINGLAYER_ATTRIBUTE_SDRALLATTRIBUTE3D_HXX

//////////////////////////////////////////////////////////////////////////////
// predefines

namespace drawinglayer { namespace attribute {
    class SdrLineAttribute;
    class SdrFillAttribute;
    class SdrLineStartEndAttribute;
    class SdrShadowAttribute;
    class FillGradientAttribute;
}}

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace attribute
    {
        class SdrLineFillShadowAttribute
        {
            // shadow, line, lineStartEnd, fill, transGradient and compound3D attributes
            SdrShadowAttribute*                 mpShadow;                   // shadow attributes (if used)
            SdrLineAttribute*                   mpLine;                     // line attributes (if used)
            SdrLineStartEndAttribute*           mpLineStartEnd;             // line start end (if used)
            SdrFillAttribute*                   mpFill;                     // fill attributes (if used)
            FillGradientAttribute*              mpFillFloatTransGradient;   // fill float transparence gradient (if used)

        public:
            SdrLineFillShadowAttribute(
                SdrLineAttribute* pLine = 0L,
                SdrFillAttribute* pFill = 0L,
                SdrLineStartEndAttribute* pLineStartEnd = 0L,
                SdrShadowAttribute* pShadow = 0L,
                FillGradientAttribute* pFillFloatTransGradient = 0L);
            ~SdrLineFillShadowAttribute();

            // copy constructor and assigment operator
            SdrLineFillShadowAttribute(const SdrLineFillShadowAttribute& rCandidate);
            SdrLineFillShadowAttribute& operator=(const SdrLineFillShadowAttribute& rCandidate);

            // compare operator
            bool operator==(const SdrLineFillShadowAttribute& rCandidate) const;

            // bool access
            bool isVisible() const { return (mpFill || mpLine); }

            // data access
            const SdrShadowAttribute* getShadow() const { return mpShadow; }
            const SdrLineAttribute* getLine() const { return mpLine; }
            const SdrLineStartEndAttribute* getLineStartEnd() const { return mpLineStartEnd; }
            const SdrFillAttribute* getFill() const { return mpFill; }
            const FillGradientAttribute* getFillFloatTransGradient() const { return mpFillFloatTransGradient; }
        };
    } // end of namespace overlay
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif // INCLUDED_DRAWINGLAYER_ATTRIBUTE_SDRALLATTRIBUTE3D_HXX

// eof
