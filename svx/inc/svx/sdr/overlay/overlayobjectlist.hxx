/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: overlayobjectlist.hxx,v $
 * $Revision: 1.3 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef _SDR_OVERLAY_OVERLAYOBJECTLIST_HXX
#define _SDR_OVERLAY_OVERLAYOBJECTLIST_HXX

#include <svx/sdr/overlay/overlayobject.hxx>
#include <tools/gen.hxx>
#include "svx/svxdllapi.h"

//////////////////////////////////////////////////////////////////////////////

#define DEFAULT_VALUE_FOR_HITTEST_PIXEL         (2L)

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace overlay
    {
        class SVX_DLLPUBLIC OverlayObjectList
        {
        protected:
            // the vector of OverlayObjects
            OverlayObjectVector                     maVector;

            // default value for HiTestPixel
            sal_uInt32 getDefaultValueForHitTestPixel() const { return 4L; }

        public:
            OverlayObjectList() {}
            ~OverlayObjectList();

            // clear list, this includes deletion of all contained objects
            void clear();

            // append/remove objects
            void append(OverlayObject& rOverlayObject) { maVector.push_back(&rOverlayObject); }
            void remove(OverlayObject& rOverlayObject);

            // access to objects
            sal_uInt32 count() const { return maVector.size(); }
            OverlayObject& getOverlayObject(sal_uInt32 nIndex) const { return *(maVector[nIndex]); }

            // Hittest with logical coordinates
            sal_Bool isHit(const basegfx::B2DPoint& rPos, double fTol = 0.0) const;

            // Hittest with pixel coordinates and pixel tolerance
            sal_Bool isHitPixel(const Point& rPos, sal_uInt32 nTol = DEFAULT_VALUE_FOR_HITTEST_PIXEL) const;

            // calculate BaseRange of all included OverlayObjects and return
            basegfx::B2DRange getBaseRange() const;

            // transform all members coordinates
            void transform(const basegfx::B2DHomMatrix& rMatrix);
        };
    } // end of namespace overlay
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

#endif //_SDR_OVERLAY_OVERLAYOBJECTLIST_HXX

// eof
