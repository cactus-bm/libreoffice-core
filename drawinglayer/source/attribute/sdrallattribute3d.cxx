/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sdrallattribute3d.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: aw $ $Date: 2006-08-09 16:47:33 $
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

#ifndef _DRAWINGLAYER_ATTRIBUTE_SDRALLATTRIBUTE3D_HXX
#include <drawinglayer/attribute/sdrallattribute3d.hxx>
#endif

#ifndef _DRAWINGLAYER_ATTRIBUTE_SDRATTRIBUTE_HXX
#include <drawinglayer/attribute/sdrattribute.hxx>
#endif

#ifndef _DRAWINGLAYER_ATTRIBUTE_FILLATTRIBUTE_HXX
#include <drawinglayer/attribute/fillattribute.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////
// pointer compare define
#define pointerOrContentEqual(p, q) ((p == q) || (p && q && *p == *q))

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace attribute
    {
        sdrLineFillShadowAttribute::sdrLineFillShadowAttribute(
            sdrLineAttribute* pLine,
            sdrFillAttribute* pFill,
            sdrLineStartEndAttribute* pLineStartEnd,
            sdrShadowAttribute* pShadow,
            fillGradientAttribute* pFillFloatTransGradient)
        :   mpShadow(pShadow),
            mpLine(pLine),
            mpLineStartEnd(pLineStartEnd),
            mpFill(pFill),
            mpFillFloatTransGradient(pFillFloatTransGradient)
        {
        }

        sdrLineFillShadowAttribute::sdrLineFillShadowAttribute(const sdrLineFillShadowAttribute& rCandidate)
        :   mpShadow(0L),
            mpLine(0L),
            mpLineStartEnd(0L),
            mpFill(0L),
            mpFillFloatTransGradient(0L)
        {
            if(!(*this == rCandidate))
            {
                *this = rCandidate;
            }
        }

        sdrLineFillShadowAttribute::~sdrLineFillShadowAttribute()
        {
            delete mpShadow;
            delete mpLine;
            delete mpLineStartEnd;
            delete mpFill;
            delete mpFillFloatTransGradient;
        }

        sdrLineFillShadowAttribute& sdrLineFillShadowAttribute::operator=(const sdrLineFillShadowAttribute& rCandidate)
        {
            // handle mpShadow
            {
                // delete local mpShadow if necessary
                if(mpShadow && ((!rCandidate.mpShadow) || (!(*mpShadow == *rCandidate.mpShadow))))
                {
                    delete mpShadow;
                    mpShadow = 0L;
                }

                // copy mpShadow if necessary
                if(!mpShadow && rCandidate.mpShadow)
                {
                    mpShadow = new sdrShadowAttribute(*rCandidate.mpShadow);
                }
            }

            // handle mpLine
            {
                // delete local mpLine if necessary
                if(mpLine && ((!rCandidate.mpLine) || (!(*mpLine == *rCandidate.mpLine))))
                {
                    delete mpLine;
                    mpLine = 0L;
                }

                // copy mpLine if necessary
                if(!mpLine && rCandidate.mpLine)
                {
                    mpLine = new sdrLineAttribute(*rCandidate.mpLine);
                }
            }

            // handle mpLineStartEnd
            {
                // delete local mpLineStartEnd if necessary
                if(mpLineStartEnd && ((!rCandidate.mpLineStartEnd) || (!(*mpLineStartEnd == *rCandidate.mpLineStartEnd))))
                {
                    delete mpLineStartEnd;
                    mpLineStartEnd = 0L;
                }

                // copy mpLineStartEnd if necessary
                if(!mpLineStartEnd && rCandidate.mpLineStartEnd)
                {
                    mpLineStartEnd = new sdrLineStartEndAttribute(*rCandidate.mpLineStartEnd);
                }
            }

            // handle mpFill
            {
                // delete local mpFill if necessary
                if(mpFill && ((!rCandidate.mpFill) || (!(*mpFill == *rCandidate.mpFill))))
                {
                    delete mpFill;
                    mpFill = 0L;
                }

                // copy mpFill if necessary
                if(!mpFill && rCandidate.mpFill)
                {
                    mpFill = new sdrFillAttribute(*rCandidate.mpFill);
                }
            }

            // handle mpFillFloatTransGradient
            {
                // delete local mpFillFloatTransGradient if necessary
                if(mpFillFloatTransGradient && ((!rCandidate.mpFillFloatTransGradient) || (!(*mpFillFloatTransGradient == *rCandidate.mpFillFloatTransGradient))))
                {
                    delete mpFillFloatTransGradient;
                    mpFillFloatTransGradient = 0L;
                }

                // copy mpFillFloatTransGradient if necessary
                if(!mpFillFloatTransGradient && rCandidate.mpFillFloatTransGradient)
                {
                    mpFillFloatTransGradient = new fillGradientAttribute(*rCandidate.mpFillFloatTransGradient);
                }
            }

            return *this;
        }

        bool sdrLineFillShadowAttribute::operator==(const sdrLineFillShadowAttribute& rCandidate) const
        {
            // handle mpShadow
            if(!pointerOrContentEqual(mpShadow, rCandidate.mpShadow))
                return false;

            // handle mpLine
            if(!pointerOrContentEqual(mpLine, rCandidate.mpLine))
                return false;

            // handle mpLineStartEnd
            if(!pointerOrContentEqual(mpLineStartEnd, rCandidate.mpLineStartEnd))
                return false;

            // handle mpFill
            if(!pointerOrContentEqual(mpFill, rCandidate.mpFill))
                return false;

            // handle mpFillFloatTransGradient
            if(!pointerOrContentEqual(mpFillFloatTransGradient, rCandidate.mpFillFloatTransGradient))
                return false;

            return true;
        }
    } // end of namespace attribute
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////
// eof
