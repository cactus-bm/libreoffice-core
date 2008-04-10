/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: lathe3d.hxx,v $
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

#ifndef _E3D_LATHE3D_HXX
#define _E3D_LATHE3D_HXX

#include <svx/obj3d.hxx>

//#ifndef _E3D_POLY3D_HXX
//#include <svx/poly3d.hxx>
//#endif
#include "svx/svxdllapi.h"

/*************************************************************************
|*
|* 3D-Rotationsobjekt aus uebergebenem 2D-Polygon erzeugen
|*
|* Das aPolyPoly3D wird in nHSegments-Schritten um die Achse rotiert.
|* nVSegments gibt die Anzahl der Linien von aPolyPoly3D an und stellt damit
|* quasi eine vertikale Segmentierung dar.
|*
\************************************************************************/

class SVX_DLLPUBLIC E3dLatheObj : public E3dCompoundObject
{
    virtual sdr::properties::BaseProperties* CreateObjectSpecificProperties();

    // Partcodes fuer Wireframe-Generierung: Standard oder Deckelflaeche
    enum { LATHE_PART_STD = 1, LATHE_PART_COVER = 2 };
    basegfx::B2DPolyPolygon maPolyPoly2D;

    // #78972#
    basegfx::B3DPolyPolygon maLinePolyPolygon;

 protected:
    void SetDefaultAttributes(E3dDefaultAttributes& rDefault);

    basegfx::B2DPolyPolygon CreateLathePolyPoly(const basegfx::B2DPolyPolygon& rPolyPoly2D, sal_uInt32 nVSegs);
    basegfx::B2DPolygon CreateLathePoly(const basegfx::B2DPolygon& rPoly2D, sal_uInt32 nVSegs);

 public:
    TYPEINFO();
    E3dLatheObj(E3dDefaultAttributes& rDefault, const basegfx::B2DPolyPolygon rPoly2D);
    E3dLatheObj();

    // HorizontalSegments:
    sal_uInt32 GetHorizontalSegments() const
        { return ((const Svx3DHorizontalSegmentsItem&)GetObjectItemSet().Get(SDRATTR_3DOBJ_HORZ_SEGS)).GetValue(); }

    // VerticalSegments:
    sal_uInt32 GetVerticalSegments() const
        { return ((const Svx3DVerticalSegmentsItem&)GetObjectItemSet().Get(SDRATTR_3DOBJ_VERT_SEGS)).GetValue(); }

    // PercentDiagonal: 0..100, before 0.0..0.5
    sal_uInt16 GetPercentDiagonal() const
        { return ((const Svx3DPercentDiagonalItem&)GetObjectItemSet().Get(SDRATTR_3DOBJ_PERCENT_DIAGONAL)).GetValue(); }

    // BackScale: 0..100, before 0.0..1.0
    sal_uInt16 GetBackScale() const
        { return ((const Svx3DBackscaleItem&)GetObjectItemSet().Get(SDRATTR_3DOBJ_BACKSCALE)).GetValue(); }

    // EndAngle: 0..10000
    sal_uInt32 GetEndAngle() const
        { return ((const Svx3DEndAngleItem&)GetObjectItemSet().Get(SDRATTR_3DOBJ_END_ANGLE)).GetValue(); }

    // #107245# GetSmoothNormals() for bLatheSmoothed
    sal_Bool GetSmoothNormals() const
        { return ((const Svx3DSmoothNormalsItem&)GetObjectItemSet().Get(SDRATTR_3DOBJ_SMOOTH_NORMALS)).GetValue(); }

    // #107245# GetSmoothLids() for bLatheSmoothFrontBack
    sal_Bool GetSmoothLids() const
        { return ((const Svx3DSmoothLidsItem&)GetObjectItemSet().Get(SDRATTR_3DOBJ_SMOOTH_LIDS)).GetValue(); }

    // #107245# GetCharacterMode() for bLatheCharacterMode
    sal_Bool GetCharacterMode() const
        { return ((const Svx3DCharacterModeItem&)GetObjectItemSet().Get(SDRATTR_3DOBJ_CHARACTER_MODE)).GetValue(); }

    // #107245# GetCloseFront() for bLatheCloseFront
    sal_Bool GetCloseFront() const
        { return ((const Svx3DCloseFrontItem&)GetObjectItemSet().Get(SDRATTR_3DOBJ_CLOSE_FRONT)).GetValue(); }

    // #107245# GetCloseBack() for bLatheCloseBack
    sal_Bool GetCloseBack() const
        { return ((const Svx3DCloseBackItem&)GetObjectItemSet().Get(SDRATTR_3DOBJ_CLOSE_BACK)).GetValue(); }

    virtual UINT16 GetObjIdentifier() const;
    void    ReSegment(sal_uInt32 nHSegs, sal_uInt32 nVSegs);

    virtual void operator=(const SdrObject&);

    virtual SdrObject* DoConvertToPolyObj(BOOL bBezier) const;

    // Geometrieerzeugung
    virtual void CreateGeometry();

    // Give out simple line geometry
    virtual basegfx::B3DPolyPolygon Get3DLineGeometry() const;

    // TakeObjName...() ist fuer die Anzeige in der UI, z.B. "3 Rahmen selektiert".
    virtual void TakeObjNameSingul(String& rName) const;
    virtual void TakeObjNamePlural(String& rName) const;

    // Lokale Parameter setzen/lesen mit Geometrieneuerzeugung
    void SetPolyPoly2D(const basegfx::B2DPolyPolygon& rNew);
    const basegfx::B2DPolyPolygon& GetPolyPoly2D() { return maPolyPoly2D; }

    // Aufbrechen
    virtual BOOL IsBreakObjPossible();
    virtual SdrAttrObj* GetBreakObj();
};

#endif          // _E3D_LATHE3D_HXX

