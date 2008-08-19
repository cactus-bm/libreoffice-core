/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: viewcontactofe3dpolygon.cxx,v $
 *
 * $Revision: 1.2 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svx.hxx"

#include <svx/sdr/contact/viewcontactofe3dpolygon.hxx>
#include <svx/polygn3d.hxx>
#include <drawinglayer/primitive3d/sdrpolypolygonprimitive3d.hxx>
#include <svx/sdr/primitive2d/sdrattributecreator.hxx>
#include <svx/sdr/primitive3d/sdrattributecreator3d.hxx>
#include <basegfx/polygon/b3dpolygon.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace contact
    {
        ViewContactOfE3dPolygon::ViewContactOfE3dPolygon(E3dPolygonObj& rPolygon)
        :   ViewContactOfE3d(rPolygon)
        {
        }

        ViewContactOfE3dPolygon::~ViewContactOfE3dPolygon()
        {
        }

        drawinglayer::primitive3d::Primitive3DSequence ViewContactOfE3dPolygon::createViewIndependentPrimitive3DSequence() const
        {
            drawinglayer::primitive3d::Primitive3DSequence xRetval;
            const SfxItemSet& rItemSet = GetE3dPolygonObj().GetMergedItemSet();
            const bool bSuppressFill(GetE3dPolygonObj().GetLineOnly());
            drawinglayer::attribute::SdrLineFillShadowAttribute* pAttribute = drawinglayer::primitive2d::createNewSdrLineFillShadowAttribute(rItemSet, bSuppressFill);

            // for 3D Objects, always create a primitive even when not visible. This is necessary ATM
            // since e.g. chart geometries rely on the occupied space of non-visible objects
            if(!pAttribute)
            {
                pAttribute = new drawinglayer::attribute::SdrLineFillShadowAttribute(
                    impCreateFallbackLineAttribute(basegfx::BColor(0.0, 1.0, 0.0)),
                    0, 0, 0, 0);
            }

            // get extrude geometry
            basegfx::B3DPolyPolygon aPolyPolygon3D(GetE3dPolygonObj().GetPolyPolygon3D());
            const basegfx::B3DPolyPolygon aPolyNormals3D(GetE3dPolygonObj().GetPolyNormals3D());
            const basegfx::B2DPolyPolygon aPolyTexture2D(GetE3dPolygonObj().GetPolyTexture2D());
            const bool bNormals(aPolyNormals3D.count());
            const bool bTexture(aPolyTexture2D.count());

            if(bNormals || bTexture)
            {
                for(sal_uInt32 a(0L); a < aPolyPolygon3D.count(); a++)
                {
                    basegfx::B3DPolygon aCandidate3D(aPolyPolygon3D.getB3DPolygon(a));
                    basegfx::B3DPolygon aNormals3D;
                    basegfx::B2DPolygon aTexture2D;

                    if(bNormals)
                    {
                        aNormals3D = aPolyNormals3D.getB3DPolygon(a);
                    }

                    if(bTexture)
                    {
                        aTexture2D = aPolyTexture2D.getB2DPolygon(a);
                    }

                    for(sal_uInt32 b(0L); b < aCandidate3D.count(); b++)
                    {
                        if(bNormals)
                        {
                            aCandidate3D.setNormal(b, aNormals3D.getB3DPoint(b));
                        }

                        if(bTexture)
                        {
                            aCandidate3D.setTextureCoordinate(b, aTexture2D.getB2DPoint(b));
                        }
                    }

                    aPolyPolygon3D.setB3DPolygon(a, aCandidate3D);
                }
            }

            // add object to world transformation
            basegfx::B3DHomMatrix aWorldTransform(GetE3dPolygonObj().GetTransform());

            // get 3D Object Attributes
            drawinglayer::attribute::Sdr3DObjectAttribute* pSdr3DObjectAttribute = drawinglayer::primitive2d::createNewSdr3DObjectAttribute(rItemSet);

            // calculate texture size
            basegfx::B2DVector aTextureSize(1.0, 1.0);

            if(bTexture)
            {
                const basegfx::B2DRange aTxRange(basegfx::tools::getRange(aPolyTexture2D));
                aTextureSize.setX(aTxRange.getWidth());
                aTextureSize.setY(aTxRange.getHeight());
            }

            // create primitive and add
            const drawinglayer::primitive3d::Primitive3DReference xReference(new drawinglayer::primitive3d::SdrPolyPolygonPrimitive3D(
                aPolyPolygon3D, aWorldTransform, aTextureSize, *pAttribute, *pSdr3DObjectAttribute));
            xRetval = drawinglayer::primitive3d::Primitive3DSequence(&xReference, 1);

            // delete 3D Object Attributes
            delete pSdr3DObjectAttribute;
            delete pAttribute;

            return xRetval;
        }
    } // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////
// eof
