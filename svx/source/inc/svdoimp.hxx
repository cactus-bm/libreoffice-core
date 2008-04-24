/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: svdoimp.hxx,v $
 * $Revision: 1.14 $
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

#ifndef _SVX_SVDOIMP_HXX
#define _SVX_SVDOIMP_HXX

#include <memory>
#include <vector>
#include <cppuhelper/weakref.hxx>

#ifndef _MAPMOD_HXX //autogen
#include <vcl/mapmod.hxx>
#endif
#include <svtools/lstner.hxx>
#include <vcl/timer.hxx>
#include <svx/svdsob.hxx>
#include <svx/svdtypes.hxx> // fuer SdrLayerID
#include <svx/svdglue.hxx> // Klebepunkte
#include <svx/xdash.hxx>
#include <svx/xpoly.hxx>

//#ifndef _POLY3D_HXX
//#include "poly3d.hxx"
//#endif
#include <svx/xenum.hxx>
#include <basegfx/vector/b2dvector.hxx>
#include <svx/rectenum.hxx>
#include <basegfx/polygon/b2dpolypolygon.hxx>

class SdrObject;
class XOutputDevice;
class XFillAttrSetItem;
class XLineAttrSetItem;
class SfxItemSet;
class Bitmap;


///////////////////////////////////////////////////////////////////////////////

// #100127# Bracket filled shapes with a comment, if recording a Mtf
class ImpGraphicFill
{
public:
    ImpGraphicFill( const SdrObject& rObj, const XOutputDevice& rXOut, const SfxItemSet& rFillItemSet, bool bIsShadow=false );
    ImpGraphicFill( const XOutputDevice& rXOut, const SfxItemSet& rSet,basegfx::B2DPolyPolygon& aGeometry, const SfxItemSet& rFillItemSet, bool bIsShadow );
    ~ImpGraphicFill();

private:
    void prepare( const XOutputDevice&  rXOut, const SfxItemSet& rSet, basegfx::B2DPolyPolygon& aGeometry, const SfxItemSet& rFillItemSet, bool bIsShadow );

    const XOutputDevice&    mrXOut;
    bool                    mbCommentWritten;
};

///////////////////////////////////////////////////////////////////////////////

// #104609# Extracted from XOutputDevice::ImpCalcBmpFillStartValues

/** Calc offset and size for bitmap fill

    This method calculates the size and the offset from the left, top
    position of a shape in logical coordinates

    @param rStartOffset
    The offset from the left, top position of the output rectangle is returned

    @param rBmpOutputSize
    The output size of the bitmap is returned herein

    @param rOutputRect
    Specifies the output rectangle into which the bitmap should be tiled into

    @param rOutputMapMode
    Specifies the logical coordinate system the output rectangle is in

    @param rFillBitmap
    Specifies the bitmap to fill with

    @param rBmpSize
    The desired destination bitmap size. If null, size is taken from the bitmap

    @param rBmpPerCent
    Percentage of bitmap size, relative to the output rectangle

    @param rBmpOffPerCent
    Offset for bitmap tiling, in percentage relative to bitmap output size

    @param bBmpLogSize
    True when using the preferred bitmap size, False when using the percentage value

    @param bBmpTile
    True for tiling. False only paints one instance of the bitmap

    @param bBmpStretch
    True if bitmap should be stretched to output rect dimension

    @param eBmpRectPoint
    Position of the start point relative to the bitmap

 */
void ImpCalcBmpFillSizes( Size&            rStartOffset,
                          Size&            rBmpOutputSize,
                          const Rectangle& rOutputRect,
                          const MapMode&   rOutputMapMode,
                          const Bitmap&    rFillBitmap,
                          const Size&      rBmpSize,
                          const Size&      rBmpPerCent,
                          const Size&      rBmpOffPerCent,
                          BOOL             bBmpLogSize,
                          BOOL             bBmpTile,
                          BOOL             bBmpStretch,
                          RECT_POINT       eBmpRectPoint );


///////////////////////////////////////////////////////////////////////////////

class ImpLineStyleParameterPack
{
    XLineJoint                      meLineJoint;
    basegfx::B2DPolyPolygon     maStartPolyPolygon;
    basegfx::B2DPolyPolygon     maEndPolyPolygon;

    sal_Int32                       mnLineWidth;
    sal_Int32                       mnStartWidth;
    sal_Int32                       mnEndWidth;

    ::std::vector<double>           maDotDashArray;
    double                          mfFullDotDashLen;
    double                          mfDegreeStepWidth;

    // bitfield
    unsigned                        mbStartCentered : 1;
    unsigned                        mbEndCentered : 1;
    unsigned                        mbForceNoArrowsLeft : 1;
    unsigned                        mbForceNoArrowsRight : 1;
    unsigned                        mbForceHair : 1;

    // flag for LineStyle. True is XLINE_SOLID, false is XLINE_DASH
    unsigned                        mbLineStyleSolid : 1;

public:
    ImpLineStyleParameterPack(const SfxItemSet& rSet, bool bForceHair);
    ~ImpLineStyleParameterPack();

    sal_Int32 GetLineWidth() const { return mnLineWidth; }
    sal_Int32 GetDisplayLineWidth() const { return mbForceHair ? 0L : mnLineWidth; }
    bool IsLineStyleSolid() const { return mbLineStyleSolid; }

    sal_Int32 GetStartWidth() const { return mnStartWidth; }
    sal_Int32 GetEndWidth() const { return mnEndWidth; }

    const basegfx::B2DPolyPolygon& GetStartPolyPolygon() const { return maStartPolyPolygon; }
    const basegfx::B2DPolyPolygon& GetEndPolyPolygon() const { return maEndPolyPolygon; }

    double GetDegreeStepWidth() const { return mfDegreeStepWidth; }

    XLineJoint GetLineJoint() const { return meLineJoint; }
    double GetLinejointMiterMinimumAngle() const { return 15.0; }

    double GetFullDotDashLen() const { return mfFullDotDashLen; }
    const ::std::vector< double >& GetDotDash() const { return maDotDashArray; }

    bool IsStartCentered() const { return mbStartCentered; }
    bool IsEndCentered() const { return mbEndCentered; }

    bool IsStartActive() const;
    bool IsEndActive() const;

    void ForceNoArrowsLeft(bool bNew) { mbForceNoArrowsLeft = bNew; }
    void ForceNoArrowsRight(bool bNew) { mbForceNoArrowsRight = bNew; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ImpLineGeometryCreator
{
    const ImpLineStyleParameterPack&        mrLineAttr;
    basegfx::B2DPolyPolygon&                maAreaPolyPolygon;
    basegfx::B2DPolyPolygon&                maLinePolyPolygon;

    // bitfield
    unsigned                                mbLineDraft : 1;

    // private support functions
    // help functions for line geometry creation
    void ImpCreateLineGeometry(
        const basegfx::B2DPolygon& rSourcePoly);

public:
    ImpLineGeometryCreator(
        const ImpLineStyleParameterPack& rAttr,
        basegfx::B2DPolyPolygon& rPoPo,
        basegfx::B2DPolyPolygon& rPoLi)
    :   mrLineAttr(rAttr),
        maAreaPolyPolygon(rPoPo),
        maLinePolyPolygon(rPoLi),
        mbLineDraft(false)
    {
    }

    void AddPolygon(const basegfx::B2DPolygon& rPoly) { ImpCreateLineGeometry(rPoly); }
    const basegfx::B2DPolyPolygon& GetAreaPolyPolygon() const { return maAreaPolyPolygon; }
    const basegfx::B2DPolyPolygon& GetLinePolyPolygon() const { return maLinePolyPolygon; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class SdrLineGeometry
{
    basegfx::B2DPolyPolygon             maAreaPolyPolygon;
    basegfx::B2DPolyPolygon             maLinePolyPolygon;
    ImpLineStyleParameterPack               maLineAttr;

    // bitfield
    unsigned                                mbForceOnePixel : 1;
    unsigned                                mbForceTwoPixel : 1;

public:
    SdrLineGeometry(
        const basegfx::B2DPolyPolygon& rAreaPolyPolygon,
        const basegfx::B2DPolyPolygon& rLinePolyPolygon,
        const ImpLineStyleParameterPack& rLineAttr,
        bool bForceOnePixel,
        bool bForceTwoPixel)
    :   maAreaPolyPolygon(rAreaPolyPolygon),
        maLinePolyPolygon(rLinePolyPolygon),
        maLineAttr(rLineAttr),
        mbForceOnePixel(bForceOnePixel),
        mbForceTwoPixel(bForceTwoPixel)
    {}

    const basegfx::B2DPolyPolygon& GetAreaPolyPolygon() { return maAreaPolyPolygon; }
    const basegfx::B2DPolyPolygon& GetLinePolyPolygon() { return maLinePolyPolygon; }
    const ImpLineStyleParameterPack& GetLineAttr() { return maLineAttr; }
    bool DoForceOnePixel() const { return mbForceOnePixel; }
    bool DoForceTwoPixel() const { return mbForceTwoPixel; }
};

#endif // _SVX_SVDOIMP_HXX

// eof
