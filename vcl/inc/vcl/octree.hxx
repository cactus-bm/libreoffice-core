/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#ifndef _SV_OCTREE_HXX
#define _SV_OCTREE_HXX

#include <vcl/salbtype.hxx>
#include <vcl/dllapi.h>

// -----------
// - Defines -
// -----------

#define OCTREE_BITS     5
#define OCTREE_BITS_1   10

// --------------
// - OctreeNode -
// --------------

typedef struct OctreeNode
{
    sal_uIntPtr     nCount;
    sal_uIntPtr     nRed;
    sal_uIntPtr     nGreen;
    sal_uIntPtr     nBlue;
    OctreeNode* pChild[ 8 ];
    OctreeNode* pNext;
    OctreeNode* pNextInCache;
    sal_uInt16      nPalIndex;
    sal_Bool        bLeaf;
} NODE;

typedef NODE*       PNODE;
typedef PNODE*      PPNODE;

// ----------
// - Octree -
// ----------

class ImpNodeCache;
class BitmapReadAccess;

class VCL_DLLPUBLIC Octree
{
private:

    BitmapPalette               aPal;
    sal_uIntPtr                     nMax;
    sal_uIntPtr                     nLeafCount;
    sal_uIntPtr                     nLevel;
    PNODE                       pTree;
    PNODE                       pReduce[ OCTREE_BITS + 1 ];
    BitmapColor*                pColor;
    ImpNodeCache*               pNodeCache;
    const BitmapReadAccess*     pAcc;
    sal_uInt16                      nPalIndex;

                                Octree() {};

    void                        CreatePalette( PNODE pNode );
    void                        GetPalIndex( PNODE pNode );

    SAL_DLLPRIVATE void         ImplCreateOctree();
    SAL_DLLPRIVATE void         ImplDeleteOctree( PPNODE ppNode );
    SAL_DLLPRIVATE void         ImplAdd( PPNODE ppNode );
    SAL_DLLPRIVATE void         ImplReduce();

public:

                                Octree( const BitmapReadAccess& rReadAcc, sal_uIntPtr nColors );
                                Octree( sal_uIntPtr nColors );
                                ~Octree();

    void                        AddColor( const BitmapColor& rColor );

    inline const BitmapPalette& GetPalette();
    inline sal_uInt16               GetBestPaletteIndex( const BitmapColor& rColor );
};

// ------------------------------------------------------------------------

inline const BitmapPalette& Octree::GetPalette()
{
    aPal.SetEntryCount( (sal_uInt16) nLeafCount );
    nPalIndex = 0;
    CreatePalette( pTree );
    return aPal;
}

// ------------------------------------------------------------------------

inline sal_uInt16 Octree::GetBestPaletteIndex( const BitmapColor& rColor )
{
    pColor = &(BitmapColor&) rColor;
    nPalIndex = 65535;
    nLevel = 0L;
    GetPalIndex( pTree );
    return nPalIndex;
}

// -------------------
// - InverseColorMap -
// -------------------

class VCL_DLLPUBLIC InverseColorMap
{
private:

    sal_uInt8*              pBuffer;
    sal_uInt8*              pMap;
    const sal_uIntPtr           nBits;

//#if 0 // _SOLAR__PRIVATE

    SAL_DLLPRIVATE void ImplCreateBuffers( const sal_uIntPtr nMax );

//#endif // __PRIVATE

public:

                        InverseColorMap( const BitmapPalette& rPal );
                        ~InverseColorMap();

    inline sal_uInt16       GetBestPaletteIndex( const BitmapColor& rColor );
};

// ------------------------------------------------------------------------

inline sal_uInt16 InverseColorMap::GetBestPaletteIndex( const BitmapColor& rColor )
{
    return pMap[ ( ( (sal_uIntPtr) rColor.GetRed() >> nBits ) << OCTREE_BITS_1 ) |
                 ( ( (sal_uIntPtr) rColor.GetGreen() >> nBits ) << OCTREE_BITS ) |
                 ( (sal_uIntPtr) rColor.GetBlue() >> nBits ) ];
}

#endif // _SV_OCTREE_HXX
