/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: deflt3d.hxx,v $
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

#ifndef _E3D_DEFLT3D_HXX
#define _E3D_DEFLT3D_HXX

#include <goodies/matril3d.hxx>
#include <basegfx/point/b3dpoint.hxx>
#include <basegfx/vector/b3dvector.hxx>
#include "svx/svxdllapi.h"

/*************************************************************************
|*
|* Klasse zum verwalten der 3D-Default Attribute
|*
\************************************************************************/

class SVX_DLLPUBLIC E3dDefaultAttributes
{
private:
    // Compound-Objekt
    Color               aDefaultAmbientColor;
    B3dMaterial         aDefaultBackMaterial;
    BOOL                bDefaultCreateNormals                   ;
    BOOL                bDefaultCreateTexture                   ;
    BOOL                bDefaultUseDifferentBackMaterial        ;

    // Cube-Objekt
    basegfx::B3DPoint               aDefaultCubePos;
    basegfx::B3DVector          aDefaultCubeSize;
    UINT16              nDefaultCubeSideFlags;
    BOOL                bDefaultCubePosIsCenter                 ;

    // Sphere-Objekt
    basegfx::B3DPoint               aDefaultSphereCenter;
    basegfx::B3DVector          aDefaultSphereSize;

    // Lathe-Objekt
    long                nDefaultLatheEndAngle;
    BOOL                bDefaultLatheSmoothed                   ;
    BOOL                bDefaultLatheSmoothFrontBack            ;
    BOOL                bDefaultLatheCharacterMode              ;
    BOOL                bDefaultLatheCloseFront;
    BOOL                bDefaultLatheCloseBack;

    // Extrude-Objekt
    BOOL                bDefaultExtrudeSmoothed                 ;
    BOOL                bDefaultExtrudeSmoothFrontBack          ;
    BOOL                bDefaultExtrudeCharacterMode            ;
    BOOL                bDefaultExtrudeCloseFront;
    BOOL                bDefaultExtrudeCloseBack;

    // Scene-Objekt
    BOOL                bDefaultDither                          ;

public:
    // Konstruktor
    E3dDefaultAttributes();

    // Defaults zuruecksetzen
    void Reset();

    // Getter/Setter fuer Default-Werte aller 3D-Objekte
    // Compound-Objekt
    const Color& GetDefaultAmbientColor() { return aDefaultAmbientColor; }
    void SetDefaultAmbientColor(const Color& rNew) { aDefaultAmbientColor = rNew; }

    const B3dMaterial& GetDefaultBackMaterial() { return aDefaultBackMaterial; }
    void SetDefaultBackMaterial(const B3dMaterial& rNew) { aDefaultBackMaterial = rNew; }
    const BOOL GetDefaultCreateNormals() { return bDefaultCreateNormals; }
    void SetDefaultCreateNormals(const BOOL bNew) { bDefaultCreateNormals = bNew; }
    const BOOL GetDefaultCreateTexture() { return bDefaultCreateTexture; }
    void SetDefaultCreateTexture(const BOOL bNew) { bDefaultCreateTexture = bNew; }
    const BOOL GetDefaultUseDifferentBackMaterial() { return bDefaultUseDifferentBackMaterial; }
    void SetDefaultUseDifferentBackMaterial(const BOOL bNew) { bDefaultUseDifferentBackMaterial = bNew; }

    // Cube-Objekt
    const basegfx::B3DPoint& GetDefaultCubePos() { return aDefaultCubePos; }
    void SetDefaultCubePos(const basegfx::B3DPoint& rNew) { aDefaultCubePos = rNew; }
    const basegfx::B3DVector& GetDefaultCubeSize() { return aDefaultCubeSize; }
    void SetDefaultCubeSize(const basegfx::B3DVector& rNew) { aDefaultCubeSize = rNew; }
    const UINT16 GetDefaultCubeSideFlags() { return nDefaultCubeSideFlags; }
    void SetDefaultCubeSideFlags(const UINT16 nNew) { nDefaultCubeSideFlags = nNew; }
    const BOOL GetDefaultCubePosIsCenter() { return bDefaultCubePosIsCenter; }
    void SetDefaultCubePosIsCenter(const BOOL bNew) { bDefaultCubePosIsCenter = bNew; }

    // Sphere-Objekt
    const basegfx::B3DPoint& GetDefaultSphereCenter() { return aDefaultSphereCenter; }
    void SetDefaultSphereCenter(const basegfx::B3DPoint& rNew) { aDefaultSphereCenter = rNew; }
    const basegfx::B3DVector& GetDefaultSphereSize() { return aDefaultSphereSize; }
    void SetDefaultSphereSize(const basegfx::B3DPoint& rNew) { aDefaultSphereSize = rNew; }

    // Lathe-Objekt
    const long GetDefaultLatheEndAngle() { return nDefaultLatheEndAngle; }
    void SetDefaultLatheEndAngle(const long nNew) { nDefaultLatheEndAngle = nNew; }
    const BOOL GetDefaultLatheSmoothed() { return bDefaultLatheSmoothed; }
    void SetDefaultLatheSmoothed(const BOOL bNew) { bDefaultLatheSmoothed = bNew; }
    const BOOL GetDefaultLatheSmoothFrontBack() { return bDefaultLatheSmoothFrontBack; }
    void SetDefaultLatheSmoothFrontBack(const BOOL bNew) { bDefaultLatheSmoothFrontBack = bNew; }
    const BOOL GetDefaultLatheCharacterMode() { return bDefaultLatheCharacterMode; }
    void SetDefaultLatheCharacterMode(const BOOL bNew) { bDefaultLatheCharacterMode = bNew; }
    const BOOL GetDefaultLatheCloseFront() { return bDefaultLatheCloseFront; }
    void SetDefaultLatheCloseFront(const BOOL bNew) { bDefaultLatheCloseFront = bNew; }
    const BOOL GetDefaultLatheCloseBack() { return bDefaultLatheCloseBack; }
    void SetDefaultLatheCloseBack(const BOOL bNew) { bDefaultLatheCloseBack = bNew; }

    // Extrude-Objekt
    const BOOL GetDefaultExtrudeSmoothed() { return bDefaultExtrudeSmoothed; }
    void SetDefaultExtrudeSmoothed(const BOOL bNew) { bDefaultExtrudeSmoothed = bNew; }
    const BOOL GetDefaultExtrudeSmoothFrontBack() { return bDefaultExtrudeSmoothFrontBack; }
    void SetDefaultExtrudeSmoothFrontBack(const BOOL bNew) { bDefaultExtrudeSmoothFrontBack = bNew; }
    const BOOL GetDefaultExtrudeCharacterMode() { return bDefaultExtrudeCharacterMode; }
    void SetDefaultExtrudeCharacterMode(const BOOL bNew) { bDefaultExtrudeCharacterMode = bNew; }
    const BOOL GetDefaultExtrudeCloseFront() { return bDefaultExtrudeCloseFront; }
    void SetDefaultExtrudeCloseFront(const BOOL bNew) { bDefaultExtrudeCloseFront = bNew; }
    const BOOL GetDefaultExtrudeCloseBack() { return bDefaultExtrudeCloseBack; }
    void SetDefaultExtrudeCloseBack(const BOOL bNew) { bDefaultExtrudeCloseBack = bNew; }

    // Scene-Objekt
    const BOOL GetDefaultDither() { return bDefaultDither; }
    void SetDefaultDither(const BOOL bNew) { bDefaultDither = bNew; }
};

#endif          // _E3D_DEFLT3D_HXX
