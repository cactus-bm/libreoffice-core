/*************************************************************************
 *
 *  $RCSfile: msashape.cxx,v $
 *
 *  $Revision: 1.30 $
 *
 *  last change: $Author: rt $ $Date: 2003-04-08 15:27:10 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _MSASHAPE_HXX
#include <msashape.hxx>
#endif
#ifndef _SVDOCIRC_HXX
#include <svdocirc.hxx>
#endif
#ifndef _SVDOGRP_HXX
#include <svdogrp.hxx>
#endif
#ifndef _SVDOPATH_HXX
#include <svdopath.hxx>
#endif
#ifndef _SVDPAGE_HXX
#include <svdpage.hxx>
#endif
#ifndef _SVX_XFLCLIT_HXX
#include <xflclit.hxx>
#endif
#ifndef _SDASAITM_HXX
#include <sdasaitm.hxx>
#endif
#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif
#ifndef _SVDMODEL_HXX
#include <svdmodel.hxx>
#endif
#ifndef _RTL_CRC_H_
#include <rtl/crc.h>
#endif
#ifndef _SVX_XLNSTIT_HXX
#include <svx/xlnstit.hxx>
#endif
#ifndef _SVX_XLNEDIT_HXX
#include <svx/xlnedit.hxx>
#endif
#ifndef _SVX_XLNSTWIT_HXX
#include <svx/xlnstwit.hxx>
#endif
#ifndef _SVX_XLNEDWIT_HXX
#include <svx/xlnedwit.hxx>
#endif
#ifndef _SVX_XLNSTCIT_HXX
#include <svx/xlnstcit.hxx>
#endif
#ifndef _SVX_XLNEDCIT_HXX
#include <svx/xlnedcit.hxx>
#endif

struct SvxMSDffVertPair
{
    sal_Int32   nValA;
    sal_Int32   nValB;
};
struct SvxMSDffCalculationData
{
    sal_uInt16  nFlags;
    sal_Int16   nVal[ 3 ];
};
struct SvxMSDffTextRectangles
{
    SvxMSDffVertPair    nPairA;
    SvxMSDffVertPair    nPairB;
};

struct mso_AutoShape
{
    SvxMSDffVertPair*           pVertices;
    sal_uInt32                  nVertices;
    sal_uInt16*                 pElements;
    sal_uInt32                  nElements;
    SvxMSDffCalculationData*    pCalculation;
    sal_uInt32                  nCalculation;
    sal_Int32*                  pDefData;
    SvxMSDffTextRectangles*     pTextRect;
    sal_uInt32                  nTextRect;
    sal_Int32                   nCoordWidth;
    sal_Int32                   nCoordHeight;
    sal_Int32                   nXRef;
    sal_Int32                   nYRef;
    SvxMSDffVertPair*           pGluePoints;
    sal_uInt32                  nGluePoints;
};

#define GEOMETRY_USED_LEFT              1
#define GEOMETRY_USED_TOP               2
#define GEOMETRY_USED_RIGHT             4
#define GEOMETRY_USED_BOTTOM            8

/*
static const SvxMSDffVertPair Vert[] =
{
    { 0, 0, }
};
static const sal_uInt16 Segm[] =
{
    0x4000,
    0x8000
};
static const SvxMSDffCalculationData Calc[] =
{
    { 0x0000, 0, 0, 0 }
};
static const sal_Int32 Default[] =
{
    0
};
static const SvxMSDffTextRect TextRect[] =
{
    { { 0, 0 }, { 0, 0 } }
};
static const sal_Int32 BoundRect[] =
{
    0, 0, 21600, 21600
};
static const mso_AutoShape mso =
{
    (SvxMSDffVertPair*)mso_sptVert, sizeof( mso_sptVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptSegm, sizeof( mso_sptSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptCalc, sizeof( mso_sptCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault,
    (SvxMSDffTextRectangles*)mso_sptTextRect, sizeof( mso_sptTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptVert, sizeof( mso_sptVert ) / sizeof( SvxMSDffVertPair )
};
*/

#define MSO_I | (sal_Int32)0x80000000

static const sal_Int32 mso_sptDefault0[] =
{
    1, 0
};
static const sal_Int32 mso_sptDefault1400[] =
{
    1, 1400
};
static const sal_Int32 mso_sptDefault1800[] =
{
    1, 1800
};
static const sal_Int32 mso_sptDefault2500[] =
{
    1, 2500
};
static const sal_Int32 mso_sptDefault2700[] =
{
    1, 2700
};
static const sal_Int32 mso_sptDefault3600[] =
{
    1, 3600
};
static const sal_Int32 mso_sptDefault3700[] =
{
    1, 3700
};
static const sal_Int32 mso_sptDefault5400[] =
{
    1, 5400
};
static const sal_Int32 mso_sptDefault8100[] =
{
    1, 5400
};
static const sal_Int32 mso_sptDefault10800[] =
{
    1, 10800
};
static const sal_Int32 mso_sptDefault16200and5400[] =
{
    2, 16200, 5400
};

static const sal_Int32 mso_sptArcDefault[] =
{
    2, 270 << 16, 0
};
static const SvxMSDffVertPair mso_sptStandardGluePoints[] =
{
    { 10800, 0 }, { 0, 10800 }, { 10800, 21600 }, { 21600, 10800 }
};
static const mso_AutoShape msoArc =
{
    NULL, 0,
    NULL, 0,
    NULL, 0,
    (sal_Int32*)mso_sptArcDefault,
    NULL, 0,
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const mso_AutoShape msoRectangle =
{
    NULL, 0,
    NULL, 0,
    NULL, 0,
    NULL,
    NULL, 0,
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffCalculationData mso_sptRoundRectangleCalc[] =  // adjustment1 : 0 - 10800
{
    { 0x2001, DFF_Prop_adjustValue, 1, 3 },
    { 0xa000, DFF_Prop_geoRight, 0, 0x400},
    { 0xa000, DFF_Prop_geoBottom, 0, 0x400},
    { 0x6000, DFF_Prop_geoLeft, 0x400, 0 },
    { 0x6000, DFF_Prop_geoTop, 0x400, 0 }
};
static const SvxMSDffTextRectangles mso_sptRoundRectangleTextRect[] =
{
    { { 3 MSO_I, 4 MSO_I }, { 1 MSO_I, 2 MSO_I } }
};
static const mso_AutoShape msoRoundRectangle =
{
    NULL, 0,
    NULL, 0,
    (SvxMSDffCalculationData*)mso_sptRoundRectangleCalc, sizeof( mso_sptRoundRectangleCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault3600,
    (SvxMSDffTextRectangles*)mso_sptRoundRectangleTextRect, sizeof( mso_sptRoundRectangleTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptRightTriangleVert[] =
{
    { 0, 0 }, { 21600, 21600 }, { 0, 21600 }, { 0, 0 }
};
static const SvxMSDffTextRectangles mso_sptRightTriangleTextRect[] =
{
    { { 1900, 12700 }, { 12700, 19700 } }
};
static const SvxMSDffVertPair mso_sptRightTriangleGluePoints[] =
{
    { 10800, 0 }, { 5400, 10800 }, { 0, 21600 }, { 10800, 21600 }, { 21600, 21600 }, { 16200, 10800 }
};
static const mso_AutoShape msoRightTriangle =
{
    (SvxMSDffVertPair*)mso_sptRightTriangleVert, sizeof( mso_sptRightTriangleVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptRightTriangleTextRect, sizeof( mso_sptRightTriangleTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptRightTriangleGluePoints, sizeof( mso_sptRightTriangleGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffTextRectangles mso_sptEllipseTextRect[] =
{
    { { 3200, 3200 }, { 18400, 18400 } }
};
static const SvxMSDffVertPair mso_sptEllipseGluePoints[] =
{
    { 10800, 0 }, { 3160, 3160 }, { 0, 10800 }, { 3160, 18440 }, { 10800, 21600 }, { 18440, 18440 }, { 21600, 10800 }, { 18440, 3160 }
};
static const mso_AutoShape msoEllipse =
{
    NULL, 0,
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptEllipseTextRect, sizeof( mso_sptEllipseTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptEllipseGluePoints, sizeof( mso_sptEllipseGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptParallelogramVert[] =  // adjustment1 : 0 - 21600
{
    { 0 MSO_I, 0 }, { 21600, 0 }, { 1 MSO_I, 21600 }, { 0, 21600 }
};
static const sal_uInt16 mso_sptParallelogramSegm[] =
{
    0x4000, 0x0003, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptParallelogramCalc[] =
{
    { 0x4000, 0, DFF_Prop_adjustValue, 0 },
    { 0x8000, 0, 21600, DFF_Prop_adjustValue },
    { 0x2001, DFF_Prop_adjustValue, 10, 24 },
    { 0x2000, 0x0402, 1750, 0 },
    { 0x8000, 21600, 0, 0x0403 },
    { 0x2001, 0x400, 1, 2 },
    { 0x4000, 10800, 0x405, 0 },
    { 0x2000, 0x400, 0, 10800 },
    { 0x6006, 0x407, 0x40d, 0 },
    { 0x8000, 10800, 0, 0x405 },
    { 0x6006, 0x407, 0x40c, 21600 },
    { 0x8000, 21600, 0, 0x405 },
    { 0x8001, 21600, 10800, 0x400 },
    { 0x8000, 21600, 0, 0x40c }
};
static const SvxMSDffTextRectangles mso_sptParallelogramTextRect[] =
{
    { { 3 MSO_I, 3 MSO_I }, { 4 MSO_I, 4 MSO_I } }
};
static const SvxMSDffVertPair mso_sptParallelogramGluePoints[] =
{
    { 6 MSO_I, 0 }, { 10800, 8 MSO_I }, { 11 MSO_I, 10800 }, { 9 MSO_I, 21600 }, { 10800, 10 MSO_I }, { 5 MSO_I, 10800 }
};
static const mso_AutoShape msoParallelogram =
{
    (SvxMSDffVertPair*)mso_sptParallelogramVert, sizeof( mso_sptParallelogramVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptParallelogramSegm, sizeof( mso_sptParallelogramSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptParallelogramCalc, sizeof( mso_sptParallelogramCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault5400,
    (SvxMSDffTextRectangles*)mso_sptParallelogramTextRect, sizeof( mso_sptParallelogramTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptParallelogramGluePoints, sizeof( mso_sptParallelogramGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptDiamondVert[] =
{
    { 10800, 0 }, { 21600, 10800 }, { 10800, 21600 }, { 0, 10800 }, { 10800, 0 }
};
static const SvxMSDffTextRectangles mso_sptDiamondTextRect[] =
{
    { { 5400, 5400 }, { 16200, 16200 } }
};
static const mso_AutoShape msoDiamond =
{
    (SvxMSDffVertPair*)mso_sptDiamondVert, sizeof( mso_sptDiamondVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptDiamondTextRect, sizeof( mso_sptDiamondTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptTrapezoidVert[] =      // adjustment1 : 0 - 10800
{
    { 0, 0 }, { 21600, 0 }, {0 MSO_I, 21600 }, { 1 MSO_I, 21600 }
};
static const sal_uInt16 mso_sptTrapezoidSegm[] =
{
    0x4000, 0x0003, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptTrapezoidCalc[] =
{
    { 0x8000, 21600, 0, DFF_Prop_adjustValue },
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x2001, DFF_Prop_adjustValue, 10, 18 },
    { 0x2000, 0x0402, 1750, 0 },
    { 0x8000, 21600, 0, 0x403 },
    { 0x2001, DFF_Prop_adjustValue, 1, 2 },
    { 0x8000, 21600, 0, 0x405 }
};
static const SvxMSDffTextRectangles mso_sptTrapezoidTextRect[] =
{
    { { 3 MSO_I, 3 MSO_I }, { 4 MSO_I, 4 MSO_I } }
};
static const SvxMSDffVertPair mso_sptTrapezoidGluePoints[] =
{
    { 6 MSO_I, 10800 }, { 10800, 21600 }, { 5 MSO_I, 10800 }, { 10800, 0 }
};
static const mso_AutoShape msoTrapezoid =
{
    (SvxMSDffVertPair*)mso_sptTrapezoidVert, sizeof( mso_sptTrapezoidVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptTrapezoidSegm, sizeof( mso_sptTrapezoidSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptTrapezoidCalc, sizeof( mso_sptTrapezoidCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault5400,
    (SvxMSDffTextRectangles*)mso_sptTrapezoidTextRect, sizeof( mso_sptTrapezoidTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptTrapezoidGluePoints, sizeof( mso_sptTrapezoidGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptOctagonVert[] =        // adjustment1 : 0 - 10800
{
    { 0 MSO_I, 0 }, { 2 MSO_I, 0 }, { 21600, 1 MSO_I }, { 21600, 3 MSO_I },
    { 2 MSO_I, 21600 }, { 0 MSO_I, 21600 }, { 0, 3 MSO_I }, { 0, 1 MSO_I }
};
static const sal_uInt16 mso_sptOctagonSegm[] =
{
    0x4000, 0x0007, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptOctagonCalc[] =
{
    { 0x6000, DFF_Prop_geoLeft, DFF_Prop_adjustValue, 0 },
    { 0x6000, DFF_Prop_geoTop, DFF_Prop_adjustValue, 0 },
    { 0xa000, DFF_Prop_geoRight, 0, DFF_Prop_adjustValue },
    { 0xa000, DFF_Prop_geoBottom, 0, DFF_Prop_adjustValue },
    { 0x2001, DFF_Prop_adjustValue, 1, 2 },
    { 0x6000, DFF_Prop_geoLeft, 0x404, 0 },
    { 0x6000, DFF_Prop_geoTop, 0x404, 0 },
    { 0xa000, DFF_Prop_geoRight, 0, 0x404 },
    { 0xa000, DFF_Prop_geoBottom, 0, 0x404 }
};
static const sal_Int32 mso_sptOctagonDefault[] =
{
    1, 5000
};
static const SvxMSDffTextRectangles mso_sptOctagonTextRect[] =
{
    { { 5 MSO_I, 6 MSO_I }, { 7 MSO_I, 8 MSO_I } }
};
static const mso_AutoShape msoOctagon =
{
    (SvxMSDffVertPair*)mso_sptOctagonVert, sizeof( mso_sptOctagonVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptOctagonSegm, sizeof( mso_sptOctagonSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptOctagonCalc, sizeof( mso_sptOctagonCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptOctagonDefault,
    (SvxMSDffTextRectangles*)mso_sptOctagonTextRect, sizeof( mso_sptOctagonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptIsocelesTriangleVert[] =   // adjustment1 : 0 - 21600
{
    { 0 MSO_I, 0 }, { 21600, 21600 }, { 0, 21600 }
};
static const sal_uInt16 mso_sptIsocelesTriangleSegm[] =
{
    0x4000, 0x0002, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptIsocelesTriangleCalc[] =
{
    { 0x4000, 0, DFF_Prop_adjustValue, 0 },
    { 0x2001, DFF_Prop_adjustValue, 1, 2 },
    { 0x2000, 0x401, 10800, 0 },
    { 0x2001, DFF_Prop_adjustValue, 2, 3 },
    { 0x2000, 0x403, 7200, 0 },
    { 0x8000, 21600, 0, 0x400 },
    { 0x2001, 0x405, 1, 2 },
    { 0x8000, 21600, 0, 0x406 }
};
static const SvxMSDffTextRectangles mso_sptIsocelesTriangleTextRect[] =
{
    { { 1 MSO_I, 10800 }, { 2 MSO_I, 18000 } },
    { { 3 MSO_I, 7200 }, { 4 MSO_I, 21600   } }
};
static const SvxMSDffVertPair mso_sptIsocelesTriangleGluePoints[] =
{
    { 10800, 0 }, { 1 MSO_I, 10800 }, { 0, 21600 }, { 10800, 21600 }, { 21600, 21600 }, { 7 MSO_I, 10800 }
};
static const mso_AutoShape msoIsocelesTriangle =
{
    (SvxMSDffVertPair*)mso_sptIsocelesTriangleVert, sizeof( mso_sptIsocelesTriangleVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptIsocelesTriangleSegm, sizeof( mso_sptIsocelesTriangleSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptIsocelesTriangleCalc, sizeof( mso_sptIsocelesTriangleCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault10800,
    (SvxMSDffTextRectangles*)mso_sptIsocelesTriangleTextRect, sizeof( mso_sptIsocelesTriangleTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptIsocelesTriangleGluePoints, sizeof( mso_sptIsocelesTriangleGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptHexagonVert[] =                // adjustment1 : 0 - 10800
{
    { 0 MSO_I, 0 }, { 1 MSO_I, 0 }, { 21600, 10800 }, { 1 MSO_I, 21600 },
    { 0 MSO_I, 21600 }, { 0, 10800 }
};
static const sal_uInt16 mso_sptHexagonSegm[] =
{
    0x4000, 0x0005, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptHexagonCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x8000, 21600, 0, DFF_Prop_adjustValue },
    { 0x2001, DFF_Prop_adjustValue, 100, 234 },
    { 0x2000, 0x402, 1700, 0 },
    { 0x8000, 21600, 0, 0x403 }
};
static const SvxMSDffTextRectangles mso_sptHexagonTextRect[] =
{
    { { 3 MSO_I, 3 MSO_I }, { 4 MSO_I, 4 MSO_I } }
};
static const mso_AutoShape msoHexagon =
{
    (SvxMSDffVertPair*)mso_sptHexagonVert, sizeof( mso_sptHexagonVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptHexagonSegm, sizeof( mso_sptHexagonSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptHexagonCalc, sizeof( mso_sptHexagonCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault5400,
    (SvxMSDffTextRectangles*)mso_sptHexagonTextRect, sizeof( mso_sptHexagonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptPentagonVert[] =
{
    { 10800, 0 }, { 0, 8260 }, { 4230, 21600 }, { 17370, 21600 },
    { 21600, 8260 }, { 10800, 0 }
};
static const SvxMSDffTextRectangles mso_sptPentagonTextRect[] =
{
    { { 4230, 5080 }, { 17370, 21600 } }
};
static const SvxMSDffVertPair mso_sptPentagonGluePoints[] =
{
    { 10800, 0 }, { 0, 8260 }, { 4230, 21600 }, { 10800, 21600 },
    { 17370, 21600 }, { 21600, 8260 }
};
static const mso_AutoShape msoPentagon =
{
    (SvxMSDffVertPair*)mso_sptPentagonVert, sizeof( mso_sptPentagonVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptPentagonTextRect, sizeof( mso_sptPentagonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptPentagonGluePoints, sizeof( mso_sptPentagonGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptPlusVert[] =               // adjustment1 : 0 - 10800
{
    { 1 MSO_I, 0 }, { 2 MSO_I, 0 }, { 2 MSO_I, 1 MSO_I }, { 21600, 1 MSO_I },
    { 21600, 2 MSO_I }, { 2 MSO_I, 2 MSO_I }, { 2 MSO_I, 21600 }, { 1 MSO_I, 21600 },
    { 1 MSO_I, 2 MSO_I }, { 0, 2 MSO_I }, { 0, 1 MSO_I }, { 1 MSO_I, 1 MSO_I }, { 1 MSO_I, 0 }
};
static const SvxMSDffCalculationData mso_sptPlusCalc[] =
{
    { 0x2001, DFF_Prop_adjustValue, 10799, 10800 },
    { 0x2000, 0x400, 0, 0 },
    { 0x8000, 21600, 0, 0x400 }
};
static const SvxMSDffTextRectangles mso_sptPlusTextRect[] =
{
    { { 1 MSO_I, 1 MSO_I }, { 2 MSO_I, 2 MSO_I } }
};
static const mso_AutoShape msoPlus =
{
    (SvxMSDffVertPair*)mso_sptPlusVert, sizeof( mso_sptPlusVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    (SvxMSDffCalculationData*)mso_sptPlusCalc, sizeof( mso_sptPlusCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault5400,
    (SvxMSDffTextRectangles*)mso_sptPlusTextRect, sizeof( mso_sptPlusTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    10800, 10800,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptCanVert[] =        // adjustment1 : 0 - 10800
{
    { 44, 0 },
    { 20, 0 }, { 0, 2 MSO_I }, { 0, 0 MSO_I },          // ccp
    { 0, 3 MSO_I },                                     // p
    { 0, 4 MSO_I }, { 20, 10800 }, { 44, 10800 },       // ccp
    { 68, 10800 }, { 88, 4 MSO_I }, { 88, 3 MSO_I },    // ccp
    { 88, 0 MSO_I },                                    // p
    { 88, 2 MSO_I }, { 68, 0 }, { 44, 0 },              // ccp
    { 44, 0 },                                          // p
    { 20, 0 }, { 0, 2 MSO_I }, { 0, 0 MSO_I },          // ccp
    { 0, 5 MSO_I }, { 20, 6 MSO_I }, { 44, 6 MSO_I },   // ccp
    { 68, 6 MSO_I },{ 88, 5 MSO_I }, { 88, 0 MSO_I },   // ccp
    { 88, 2 MSO_I },{ 68, 0 }, { 44, 0 }                // ccp
};
static const sal_uInt16 mso_sptCanSegm[] =
{
    0x4000, 0x2001, 0x0001, 0x2002, 0x0001, 0x2001, 0x6001, 0x8000,
    0x4000, 0x2004, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptCanCalc[] =
{
    { 0x2001, DFF_Prop_adjustValue, 1, 4 },     // 1/4
    { 0x2001, 0x0400, 6, 11 },
    { 0xa000,   0x0400, 0, 0x0401 },
    { 0x8000, 10800, 0, 0x0400 },
    { 0x6000, 0x0403, 0x0401, 0 },
    { 0x6000, 0x0400, 0x0401, 0 },
    { 0x2001, DFF_Prop_adjustValue, 1, 2 }
};
static const SvxMSDffTextRectangles mso_sptCanTextRect[] =
{
    { { 0, 6 MSO_I }, { 88, 3 MSO_I } }
};
static const SvxMSDffVertPair mso_sptCanGluePoints[] =
{
    { 44, 6 MSO_I }, { 44, 0 }, { 0, 5400 }, { 44, 10800 }, { 88, 5400 }
};
static const sal_Int32 mso_sptCanBoundRect[] =
{
    0, 0, 88, 10800
};
static const mso_AutoShape msoCan =
{
    (SvxMSDffVertPair*)mso_sptCanVert, sizeof( mso_sptCanVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptCanSegm, sizeof( mso_sptCanSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptCanCalc, sizeof( mso_sptCanCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault5400,
    (SvxMSDffTextRectangles*)mso_sptCanTextRect, sizeof( mso_sptCanTextRect ) / sizeof( SvxMSDffTextRectangles ),
    88, 10800,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptCanGluePoints, sizeof( mso_sptCanGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptArrowVert[] =  // adjustment1: x 0 - 21600
{                                                   // adjustment2: y 0 - 10800
    { 0, 0 MSO_I }, { 1 MSO_I, 0 MSO_I }, { 1 MSO_I, 0 }, { 21600, 10800 },
    { 1 MSO_I, 21600 }, { 1 MSO_I, 2 MSO_I }, { 0, 2 MSO_I }
};
static const sal_uInt16 mso_sptArrowSegm[] =
{
    0x4000, 0x0006, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptArrowCalc[] =
{
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x8000, 21600, 0, DFF_Prop_adjust2Value },
    { 0x8000, 21600, 0, 0x401 },
    { 0x6001, 0x403, 0x400, 10800 },
    { 0x6000, 0x401, 0x404, 0 },
    { 0x6001, 0x401, 0x400, 10800 },
    { 0xa000, 0x401, 0, 0x406 }
};
static const SvxMSDffTextRectangles mso_sptArrowTextRect[] =
{
    { { 0, 0 MSO_I }, { 5 MSO_I, 2 MSO_I } }
};
static const mso_AutoShape msoArrow =
{
    (SvxMSDffVertPair*)mso_sptArrowVert, sizeof( mso_sptArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptArrowSegm, sizeof( mso_sptArrowSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptArrowCalc, sizeof( mso_sptArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault16200and5400,
    (SvxMSDffTextRectangles*)mso_sptArrowTextRect, sizeof( mso_sptArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptLeftArrowVert[] =  // adjustment1: x 0 - 21600
{                                                       // adjustment2: y 0 - 10800
    { 21600, 0 MSO_I }, { 1 MSO_I, 0 MSO_I }, { 1 MSO_I, 0 }, { 0, 10800 },
    { 1 MSO_I, 21600 }, { 1 MSO_I, 2 MSO_I }, { 21600, 2 MSO_I }
};
static const sal_uInt16 mso_sptLeftArrowSegm[] =
{
    0x4000, 0x0006, 0x6001, 0x8000
};
static const sal_Int32 mso_sptLeftArrowDefault[] =
{
    2, 5400, 5400
};
static const SvxMSDffTextRectangles mso_sptLeftArrowTextRect[] =
{
    { { 7 MSO_I, 0 MSO_I }, { 21600, 2 MSO_I } }
};
static const mso_AutoShape msoLeftArrow =
{
    (SvxMSDffVertPair*)mso_sptLeftArrowVert, sizeof( mso_sptLeftArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptLeftArrowSegm, sizeof( mso_sptLeftArrowSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptArrowCalc, sizeof( mso_sptArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptLeftArrowDefault,
    (SvxMSDffTextRectangles*)mso_sptLeftArrowTextRect, sizeof( mso_sptLeftArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptDownArrowVert[] =  // adjustment1: x 0 - 21600
{                                                       // adjustment2: y 0 - 10800
    { 0 MSO_I, 0 }, { 0 MSO_I, 1 MSO_I }, { 0, 1 MSO_I }, { 10800, 21600 },
    { 21600, 1 MSO_I }, { 2 MSO_I, 1 MSO_I }, { 2 MSO_I, 0 }
};
static const sal_uInt16 mso_sptDownArrowSegm[] =
{
    0x4000, 0x0006, 0x6001, 0x8000
};
static const SvxMSDffTextRectangles mso_sptDownArrowTextRect[] =
{
    { { 0 MSO_I, 0 }, { 2 MSO_I, 5 MSO_I } }
};
static const mso_AutoShape msoDownArrow =
{
    (SvxMSDffVertPair*)mso_sptDownArrowVert, sizeof( mso_sptDownArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptDownArrowSegm, sizeof( mso_sptDownArrowSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptArrowCalc, sizeof( mso_sptArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault16200and5400,
    (SvxMSDffTextRectangles*)mso_sptDownArrowTextRect, sizeof( mso_sptDownArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptUpArrowVert[] =    // adjustment1: x 0 - 21600
{                                                       // adjustment2: y 0 - 10800
    { 0 MSO_I, 21600 }, { 0 MSO_I, 1 MSO_I }, { 0, 1 MSO_I }, { 10800, 0 },
    { 21600, 1 MSO_I }, { 2 MSO_I, 1 MSO_I }, { 2 MSO_I, 21600 }
};
static const sal_uInt16 mso_sptUpArrowSegm[] =
{
    0x4000, 0x0006, 0x6001, 0x8000
};
static const sal_Int32 mso_sptUpArrowDefault[] =
{
    2, 5400, 5400
};
static const SvxMSDffTextRectangles mso_sptUpArrowTextRect[] =
{
    { { 0 MSO_I, 7 MSO_I }, { 2 MSO_I, 21600 } }
};
static const mso_AutoShape msoUpArrow =
{
    (SvxMSDffVertPair*)mso_sptUpArrowVert, sizeof( mso_sptUpArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptUpArrowSegm, sizeof( mso_sptUpArrowSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptArrowCalc, sizeof( mso_sptArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptUpArrowDefault,
    (SvxMSDffTextRectangles*)mso_sptUpArrowTextRect, sizeof( mso_sptUpArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptLeftRightArrowVert[] = // adjustment1: x 0 - 10800
{                                                           // adjustment2: y 0 - 10800
    { 0, 10800 }, { 0 MSO_I, 0 }, { 0 MSO_I, 1 MSO_I }, { 2 MSO_I, 1 MSO_I },
    { 2 MSO_I, 0 }, { 21600, 10800 }, { 2 MSO_I, 21600 }, { 2 MSO_I, 3 MSO_I },
    { 0 MSO_I, 3 MSO_I }, { 0 MSO_I, 21600 }
};
static const sal_uInt16 mso_sptLeftRightArrowSegm[] =
{
    0x4000, 0x0009, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptDoubleArrowCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
    { 0x8000, 21600, 0, DFF_Prop_adjustValue },
    { 0x8000, 21600, 0, DFF_Prop_adjust2Value },
    { 0x8000, 10800, 0, DFF_Prop_adjust2Value },
    { 0x6001, DFF_Prop_adjustValue, 0x404, 10800 },
    { 0x8000, 21600, 0, 0x405 },
    { 0x8000, 10800, 0, DFF_Prop_adjustValue },
    { 0x6001, DFF_Prop_adjust2Value, 0x407, 10800 },
    { 0x8000, 21600, 0, 0x408 }
};
static const sal_Int32 mso_sptLeftRightArrowDefault[] =
{
    2, 4300, 5400
};
static const SvxMSDffTextRectangles mso_sptLeftRightArrowTextRect[] =
{
    { { 5 MSO_I, 1 MSO_I }, { 6 MSO_I, 3 MSO_I } }
};
static const mso_AutoShape msoLeftRightArrow =
{
    (SvxMSDffVertPair*)mso_sptLeftRightArrowVert, sizeof( mso_sptLeftRightArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptLeftRightArrowSegm, sizeof( mso_sptLeftRightArrowSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptDoubleArrowCalc, sizeof( mso_sptDoubleArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptLeftRightArrowDefault,
    (SvxMSDffTextRectangles*)mso_sptLeftRightArrowTextRect, sizeof( mso_sptLeftRightArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptUpDownArrowVert[] =    // adjustment1: x 0 - 10800
{                                                           // adjustment2: y 0 - 10800
    { 0, 1 MSO_I }, { 10800, 0 }, { 21600, 1 MSO_I }, { 2 MSO_I, 1 MSO_I },
    { 2 MSO_I, 3 MSO_I }, { 21600, 3 MSO_I }, { 10800, 21600 }, { 0, 3 MSO_I },
    { 0 MSO_I, 3 MSO_I }, { 0 MSO_I, 1 MSO_I }
};
static const sal_uInt16 mso_sptUpDownArrowSegm[] =
{
    0x4000, 0x0009, 0x6001, 0x8000
};
static const sal_Int32 mso_sptUpDownArrowDefault[] =
{
    2, 5400, 4300
};
static const SvxMSDffTextRectangles mso_sptUpDownArrowTextRect[] =
{
    { { 0 MSO_I, 8 MSO_I }, { 2 MSO_I, 9 MSO_I } }
};
static const mso_AutoShape msoUpDownArrow =
{
    (SvxMSDffVertPair*)mso_sptUpDownArrowVert, sizeof( mso_sptUpDownArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptUpDownArrowSegm, sizeof( mso_sptUpDownArrowSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptDoubleArrowCalc, sizeof( mso_sptDoubleArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptUpDownArrowDefault,
    (SvxMSDffTextRectangles*)mso_sptUpDownArrowTextRect, sizeof( mso_sptUpDownArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptQuadArrowVert[] =  // adjustment1: x 0 - 10800, adjustment2: x 0 - 10800
{                                                       // adjustment3: y 0 - 10800
    { 0, 10800 }, { 0 MSO_I, 1 MSO_I }, { 0 MSO_I, 2 MSO_I }, { 2 MSO_I, 2 MSO_I },
    { 2 MSO_I, 0 MSO_I }, { 1 MSO_I, 0 MSO_I }, { 10800, 0 }, { 3 MSO_I, 0 MSO_I },
    { 4 MSO_I, 0 MSO_I }, { 4 MSO_I, 2 MSO_I }, { 5 MSO_I, 2 MSO_I }, { 5 MSO_I, 1 MSO_I },
    { 21600, 10800 }, { 5 MSO_I, 3 MSO_I }, { 5 MSO_I, 4 MSO_I }, { 4 MSO_I, 4 MSO_I },
    { 4 MSO_I, 5 MSO_I }, { 3 MSO_I, 5 MSO_I }, { 10800, 21600 }, { 1 MSO_I, 5 MSO_I },
    { 2 MSO_I, 5 MSO_I }, { 2 MSO_I, 4 MSO_I }, { 0 MSO_I, 4 MSO_I }, { 0 MSO_I, 3 MSO_I }
};
static const sal_uInt16 mso_sptQuadArrowSegm[] =
{
    0x4000, 0x0017, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptQuadArrowCalc[] =
{
    { 0x2000, DFF_Prop_adjust3Value, 0, 0 },
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
    { 0x8000, 21600, 0, DFF_Prop_adjustValue },
    { 0x8000, 21600, 0, DFF_Prop_adjust2Value },
    { 0x8000, 21600, 0, DFF_Prop_adjust3Value }
};
static const sal_Int32 mso_sptQuadArrowDefault[] =
{
    3, 6500, 8600, 4300
};
static const SvxMSDffTextRectangles mso_sptQuadArrowTextRect[] =    // todo
{
    { { 0, 0 }, { 21600, 21600 } }
};
static const mso_AutoShape msoQuadArrow =
{
    (SvxMSDffVertPair*)mso_sptQuadArrowVert, sizeof( mso_sptQuadArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptQuadArrowSegm, sizeof( mso_sptQuadArrowSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptQuadArrowCalc, sizeof( mso_sptQuadArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptQuadArrowDefault,
    (SvxMSDffTextRectangles*)mso_sptQuadArrowTextRect, sizeof( mso_sptQuadArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptLeftRightUpArrowVert[] =   // adjustment1: x 0 - 10800, adjustment2: x 0 - 10800
{                                                               // adjustment3: y 0 - 21600
    { 10800, 0 }, { 3 MSO_I, 2 MSO_I }, { 4 MSO_I, 2 MSO_I }, { 4 MSO_I, 1 MSO_I },
    { 5 MSO_I, 1 MSO_I }, { 5 MSO_I, 0 MSO_I }, { 21600, 10800 }, { 5 MSO_I, 3 MSO_I },
    { 5 MSO_I, 4 MSO_I }, { 2 MSO_I, 4 MSO_I }, { 2 MSO_I, 3 MSO_I }, { 0, 10800 },
    { 2 MSO_I, 0 MSO_I }, { 2 MSO_I, 1 MSO_I }, { 1 MSO_I, 1 MSO_I }, { 1 MSO_I, 2 MSO_I },
    { 0 MSO_I, 2 MSO_I }
};
static const sal_uInt16 mso_sptLeftRightUpArrowSegm[] =
{
    0x4000, 0x0010, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptLeftRightUpArrowCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },             // 0
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },            // 1
    { 0x6001, 0x0403, DFF_Prop_adjust3Value, 21600 },   // 2
    { 0x8000, 21600, 0, DFF_Prop_adjustValue },         // 3
    { 0x8000, 21600, 0, DFF_Prop_adjust2Value },        // 4
    { 0x8000, 21600, 0, 0x0402 }                        // 5
};
static const sal_Int32 mso_sptLeftRightUpArrowDefault[] =
{
    3, 6500, 8600, 6200
};
static const SvxMSDffTextRectangles mso_sptLeftRightUpArrowTextRect[] =     // todo
{
    { { 0, 0 }, { 21600, 21600 } }
};
static const mso_AutoShape msoLeftRightUpArrow =
{
    (SvxMSDffVertPair*)mso_sptLeftRightUpArrowVert, sizeof( mso_sptLeftRightUpArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptLeftRightUpArrowSegm, sizeof( mso_sptLeftRightUpArrowSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptLeftRightUpArrowCalc, sizeof( mso_sptLeftRightUpArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptLeftRightUpArrowDefault,
    (SvxMSDffTextRectangles*)mso_sptLeftRightUpArrowTextRect, sizeof( mso_sptLeftRightUpArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptBentArrowVert[] =  // adjustment1 : x 12427 - 21600
{                                                       // adjustment2 : y 0 - 6079
    { 0, 21600 }, { 0, 12160 }, { 12427, 1 MSO_I }, { 0 MSO_I, 1 MSO_I },
    { 0 MSO_I, 0 }, { 21600, 6079 }, { 0 MSO_I, 12158 }, { 0 MSO_I, 2 MSO_I },
    { 12427, 2 MSO_I }, { 4 MSO_I, 12160 }, { 4 MSO_I, 21600 }
};
static const sal_uInt16 mso_sptBentArrowSegm[] =
{
    0x4000, 0x0001, 0xa801, 0x0006, 0xa701, 0x0001, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptBentArrowCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
    { 0x8000,   12158, 0, DFF_Prop_adjust2Value },
    { 0x8000, 6079, 0, DFF_Prop_adjust2Value },
    { 0x2001,   0x0403, 2, 1 }
};
static const sal_Int32 mso_sptBentArrowDefault[] =
{
    2, 15100, 2900
};
static const SvxMSDffTextRectangles mso_sptBentArrowTextRect[] =    // todo
{
    { { 0, 0 }, { 21600, 21600 } }
};
static const mso_AutoShape msoBentArrow =
{
    (SvxMSDffVertPair*)mso_sptBentArrowVert, sizeof( mso_sptBentArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptBentArrowSegm, sizeof( mso_sptBentArrowSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptBentArrowCalc, sizeof( mso_sptBentArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptBentArrowDefault,
    (SvxMSDffTextRectangles*)mso_sptBentArrowTextRect, sizeof( mso_sptBentArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptUturnArrowVert[] =
{
    { 0, 21600 }, { 0, 8550 },                                          // pp
    { 0, 3540 }, { 4370, 0 }, { 9270, 0 },                              // ccp
    { 13890, 0 }, { 18570, 3230 }, { 18600, 8300 },                     // ccp
    { 21600, 8300 }, { 15680, 14260 }, { 9700, 8300 }, { 12500, 8300 }, // pppp
    { 12320, 6380 }, { 10870, 5850 }, { 9320, 5850 },                   // ccp
    { 7770, 5850 }, { 6040, 6410 }, { 6110, 8520 },                     // ccp
    { 6110, 21600 }
};
static const sal_uInt16 mso_sptUturnArrowSegm[] =
{
    0x4000, 0x0001, 0x2002, 0x0004, 0x2002, 0x0001, 0x6000, 0x8000
};
static const SvxMSDffTextRectangles mso_sptUturnArrowTextRect[] =
{
    { { 0, 8280 }, { 6110, 21600 } }
};
static const mso_AutoShape msoUturnArrow =
{
    (SvxMSDffVertPair*)mso_sptUturnArrowVert, sizeof( mso_sptUturnArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptUturnArrowSegm, sizeof( mso_sptUturnArrowSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptUturnArrowTextRect, sizeof( mso_sptUturnArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptLeftUpArrowVert[] =    // adjustment1 : x 0 - 21600, adjustment2 : x 0 - 21600
{                                                           // adjustment3 : y 0 - 21600
    { 0, 5 MSO_I }, { 2 MSO_I, 0 MSO_I }, { 2 MSO_I, 7 MSO_I }, { 7 MSO_I, 7 MSO_I },
    { 7 MSO_I, 2 MSO_I }, { 0 MSO_I, 2 MSO_I }, { 5 MSO_I, 0 }, { 21600, 2 MSO_I },
    { 1 MSO_I, 2 MSO_I }, { 1 MSO_I, 1 MSO_I }, { 2 MSO_I, 1 MSO_I }, { 2 MSO_I, 21600 }
};
static const sal_uInt16 mso_sptLeftUpArrowSegm[] =
{
    0x4000, 0x000b, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptLeftUpArrowCalc[] =
{
    { 0x2000,   DFF_Prop_adjustValue, 0, 0 },       // 0
    { 0x2000,   DFF_Prop_adjust2Value, 0, 0 },      // 1
    { 0x2000, DFF_Prop_adjust3Value, 0, 0 },        // 2
    { 0x8000,   21600, 0, DFF_Prop_adjustValue },   // 3
    { 0x2001,   0x0403, 1, 2 },                     // 4
    { 0x6000, DFF_Prop_adjustValue, 0x0404, 0 },    // 5
    { 0x8000,   21600, 0, DFF_Prop_adjust2Value },  // 6
    { 0x6000, DFF_Prop_adjustValue, 0x0406, 0 }     // 7
};
static const sal_Int32 mso_sptLeftUpArrowDefault[] =
{
    3, 9340, 18500, 6200
};
static const SvxMSDffTextRectangles mso_sptLeftUpArrowTextRect[] =
{
    { { 2 MSO_I, 7 MSO_I }, { 1 MSO_I, 1 MSO_I } },
    { { 7 MSO_I, 2 MSO_I }, { 1 MSO_I, 1 MSO_I } }
};
static const mso_AutoShape msoLeftUpArrow =
{
    (SvxMSDffVertPair*)mso_sptLeftUpArrowVert, sizeof( mso_sptLeftUpArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptLeftUpArrowSegm, sizeof( mso_sptLeftUpArrowSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptLeftUpArrowCalc, sizeof( mso_sptLeftUpArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptLeftUpArrowDefault,
    (SvxMSDffTextRectangles*)mso_sptLeftUpArrowTextRect, sizeof( mso_sptLeftUpArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptBentUpArrowVert[] =    // adjustment1 : x 0 - 21600, adjustment2 : x 0 - 21600
{                                                           // adjustment3 : y 0 - 21600
    { 0, 8 MSO_I }, { 7 MSO_I, 8 MSO_I }, { 7 MSO_I, 2 MSO_I }, { 0 MSO_I, 2 MSO_I },
    { 5 MSO_I, 0 }, { 21600, 2 MSO_I }, { 1 MSO_I, 2 MSO_I }, { 1 MSO_I, 21600 },
    { 0, 21600 }
};
static const sal_uInt16 mso_sptBentUpArrowSegm[] =
{
    0x4000, 0x0008, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptBentUpArrowCalc[] =
{
    { 0x2000,   DFF_Prop_adjustValue, 0, 0 },       // 0
    { 0x2000,   DFF_Prop_adjust2Value, 0, 0 },      // 1
    { 0x2000, DFF_Prop_adjust3Value, 0, 0 },        // 2
    { 0x8000,   21600, 0, DFF_Prop_adjustValue },   // 3
    { 0x2001,   0x0403, 1, 2 },                     // 4
    { 0x6000, DFF_Prop_adjustValue, 0x0404, 0 },    // 5
    { 0x8000,   21600, 0, DFF_Prop_adjust2Value },  // 6
    { 0x6000, DFF_Prop_adjustValue, 0x0406, 0 },    // 7
    { 0x6000, 0x0407, 0x0406, 0 }                   // 8
};
static const sal_Int32 mso_sptBentUpArrowDefault[] =
{
    3, 9340, 18500, 7200
};
static const SvxMSDffTextRectangles mso_sptBentUpArrowTextRect[] =
{
    { { 2 MSO_I, 7 MSO_I }, { 1 MSO_I, 1 MSO_I } },
    { { 7 MSO_I, 2 MSO_I }, { 1 MSO_I, 1 MSO_I } }
};
static const mso_AutoShape msoBentUpArrow =
{
    (SvxMSDffVertPair*)mso_sptBentUpArrowVert, sizeof( mso_sptBentUpArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptBentUpArrowSegm, sizeof( mso_sptBentUpArrowSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptBentUpArrowCalc, sizeof( mso_sptBentUpArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptBentUpArrowDefault,
    (SvxMSDffTextRectangles*)mso_sptBentUpArrowTextRect, sizeof( mso_sptBentUpArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptCurvedArrowVert[] =    // adjustment1 : y 10800 - 21600, adjustment2 : y 16424 - 21600
{                                                           // adjustment3 : x 0 - 21600
    { 21600, 0 },
    { 9675, 0 }, { 0, 10 MSO_I }, { 0, 9 MSO_I },                                       // ccp
    { 0, 11 MSO_I },
    { 0, 14 MSO_I }, { 15 MSO_I, 1 MSO_I }, { 2 MSO_I, 1 MSO_I },                       // ccp
    { 2 MSO_I, 21600 }, { 21600, 7 MSO_I }, { 2 MSO_I, 0 MSO_I }, { 2 MSO_I, 16 MSO_I },// pppp
    { 2 MSO_I, 16 MSO_I }, { 80, 8 MSO_I }, { 80, 8 MSO_I },                            // ccp
    { 80, 8 MSO_I }, { 21600, 5 MSO_I }, { 21600, 0 }                                   // ccp
};
static const sal_uInt16 mso_sptCurvedArrowSegm[] =
{
    0x4000, 0x2001, 0x0001, 0x2001, 0x0004, 0x2002, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptCurvedArrowCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },                     // 0
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },                    // 1
    { 0x2000, DFF_Prop_adjust3Value, 0, 0 },                    // 2
    { 0x8000, 21600, 0, DFF_Prop_adjust2Value },                // 3
    { 0xa000, DFF_Prop_adjust2Value, 0, DFF_Prop_adjustValue }, // 4
    { 0xa000, 0x0404, 0, 0x0403 },                              // 5
    { 0x2001, 0x0405, 1, 2 },                                   // 6
    { 0xa000, DFF_Prop_adjust2Value, 0, 0x0406 },               // 7
    { 0x2001, DFF_Prop_adjust2Value, 1, 2 },                    // 8
    { 0xa000, 0x0408, 0, 0x0406 },                              // 9
    { 0x2001, 0x0409, 10000, 22326 },                           // 10
    { 0x6000, 0x0409, 0x0405, 0 },                              // 11
    { 0xa000, DFF_Prop_adjust2Value, 0, 0x040b },               // 12
    { 0x2001, 0x040c, 10000, 23148 },                           // 13
    { 0x6000, 0x040d, 0x040b, 0 },                              // 14
    { 0x2001, DFF_Prop_adjust3Value, 10000, 25467 },            // 15
    { 0x6000, DFF_Prop_adjustValue, 0x0403, 0 }                 // 16
};
static const sal_Int32 mso_sptCurvedArrowDefault[] =
{
    3, 13000, 19400, 14400
};
static const SvxMSDffTextRectangles mso_sptCurvedArrowTextRect[] =  // todo
{
    { { 0, 0 }, { 21600, 21600 } }
};
static const mso_AutoShape msoCurvedArrow =
{
    (SvxMSDffVertPair*)mso_sptCurvedArrowVert, sizeof( mso_sptCurvedArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptCurvedArrowSegm, sizeof( mso_sptCurvedArrowSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptCurvedArrowCalc, sizeof( mso_sptCurvedArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptCurvedArrowDefault,
    (SvxMSDffTextRectangles*)mso_sptCurvedArrowTextRect, sizeof( mso_sptCurvedArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptStripedRightArrowVert[] =  // adjustment1 : x 3375 - 21600
{                                                               // adjustment2 : y 0 - 10800
    { 3375, 0 MSO_I }, { 1 MSO_I, 0 MSO_I }, { 1 MSO_I, 0 }, { 21600, 10800 },
    { 1 MSO_I, 21600 }, { 1 MSO_I, 2 MSO_I }, { 3375, 2 MSO_I }, { 0, 0 MSO_I },
    { 675, 0 MSO_I }, { 675, 2 MSO_I }, { 0, 2 MSO_I }, { 1350, 0 MSO_I },
    { 2700, 0 MSO_I }, { 2700, 2 MSO_I }, { 1350, 2 MSO_I }
};
static const sal_uInt16 mso_sptStripedRightArrowSegm[] =
{
    0x4000, 0x0006, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptStripedRightArrowCalc[] =
{
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x8000, 21600, 0, DFF_Prop_adjust2Value },
    { 0x8000, 21600, 0, 0x401 },
    { 0x6001, 0x403, 0x400, 10800 },
    { 0x6000, 0x401, 0x404, 0 }
};
static const SvxMSDffTextRectangles mso_sptStripedRightArrowTextRect[] =
{
    { { 3375, 0 MSO_I }, { 5 MSO_I, 2 MSO_I } }
};
static const mso_AutoShape msoStripedRightArrow =
{
    (SvxMSDffVertPair*)mso_sptStripedRightArrowVert, sizeof( mso_sptStripedRightArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptStripedRightArrowSegm, sizeof( mso_sptStripedRightArrowSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptStripedRightArrowCalc, sizeof( mso_sptStripedRightArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault16200and5400,
    (SvxMSDffTextRectangles*)mso_sptStripedRightArrowTextRect, sizeof( mso_sptStripedRightArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptNotchedRightArrowVert[] =  // adjustment1 : x 0 - 21600    (default 16200)
{                                                               // adjustment2 : y 0 - 10800    (default 5400)
    { 0, 1 MSO_I }, { 0 MSO_I, 1 MSO_I }, { 0 MSO_I, 0 }, { 21600, 10800 },
    { 0 MSO_I, 21600 }, { 0 MSO_I, 2 MSO_I }, { 0, 2 MSO_I }, { 5 MSO_I, 10800 }, { 0, 1 MSO_I }
};
static const SvxMSDffCalculationData mso_sptNotchedRightArrowCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
    { 0x8000, 21600, 0, DFF_Prop_adjust2Value },
    { 0x8000, 21600, 0, DFF_Prop_adjustValue },
    { 0x8000, 10800, 0, DFF_Prop_adjust2Value },
    { 0x6001, 0x403, 0x404, 10800 }
};
static const SvxMSDffTextRectangles mso_sptNotchedRightArrowTextRect[] =    // todo
{
    { { 0, 0 }, { 21600, 21600 } }
};
static const mso_AutoShape msoNotchedRightArrow =
{
    (SvxMSDffVertPair*)mso_sptNotchedRightArrowVert, sizeof( mso_sptNotchedRightArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)NULL, 0,
    (SvxMSDffCalculationData*)mso_sptNotchedRightArrowCalc, sizeof( mso_sptNotchedRightArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault16200and5400,
    (SvxMSDffTextRectangles*)mso_sptNotchedRightArrowTextRect, sizeof( mso_sptNotchedRightArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptHomePlateVert[] =  // adjustment1 : x 0 - 21600
{
    { 0, 0 }, { 0 MSO_I, 0 }, { 21600, 10800 }, { 0 MSO_I, 21600 },
    { 0, 21600 }
};
static const sal_uInt16 mso_sptHomePlateSegm[] =
{
    0x4000, 0x0004, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptHomePlateCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 }
};
static const sal_Int32 mso_sptHomePlateDefault[] =
{
    1, 16200
};
static const SvxMSDffTextRectangles mso_sptHomePlateTextRect[] =    // todo
{
    { { 0, 0 }, { 21600, 21600 } }
};
static const mso_AutoShape msoHomePlate =
{
    (SvxMSDffVertPair*)mso_sptHomePlateVert, sizeof( mso_sptHomePlateVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptHomePlateSegm, sizeof( mso_sptHomePlateSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptHomePlateCalc, sizeof( mso_sptHomePlateCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptHomePlateDefault,
    (SvxMSDffTextRectangles*)mso_sptHomePlateTextRect, sizeof( mso_sptHomePlateTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptChevronVert[] =    // adjustment1 : x 0 - 21600
{
    { 0, 0 }, { 0 MSO_I, 0 }, { 21600, 10800 }, { 0 MSO_I, 21600 },
    { 0, 21600 }, { 1 MSO_I, 10800 }
};
static const sal_uInt16 mso_sptChevronSegm[] =
{
    0x4000, 0x0005, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptChevronCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x8000, 21600, 0, 0x0400 }
};
static const sal_Int32 mso_sptChevronDefault[] =
{
    1, 16200
};
static const SvxMSDffTextRectangles mso_sptChevronTextRect[] =  // todo
{
    { { 0, 0 }, { 21600, 21600 } }
};
static const mso_AutoShape msoChevron =
{
    (SvxMSDffVertPair*)mso_sptChevronVert, sizeof( mso_sptChevronVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptChevronSegm, sizeof( mso_sptChevronSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptChevronCalc, sizeof( mso_sptChevronCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptChevronDefault,
    (SvxMSDffTextRectangles*)mso_sptChevronTextRect, sizeof( mso_sptChevronTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptRightArrowCalloutVert[] =  // adjustment1 : x 0 - 21000
{                                                               // adjustment2 : y 0 - 10800
    { 0, 0 }, { 0 MSO_I, 0 }, { 0 MSO_I, 3 MSO_I }, { 2 MSO_I, 3 MSO_I },
    { 2 MSO_I, 1 MSO_I }, { 21600, 10800 }, { 2 MSO_I, 4 MSO_I }, { 2 MSO_I, 5 MSO_I },
    { 0 MSO_I, 5 MSO_I }, { 0 MSO_I, 21600 }, { 0, 21600 }
};
static const sal_uInt16 mso_sptRightArrowCalloutSegm[] =
{
    0x4000, 0x000a, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptRightArrowCalloutCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
    { 0x2000, DFF_Prop_adjust3Value, 0, 0 },
    { 0x2000, DFF_Prop_adjust4Value, 0, 0 },
    { 0x8000, 21600, 0, 0x0401 },
    { 0x8000, 21600, 0, 0x0403 }
};
static const sal_Int32 mso_sptRightArrowCalloutDefault[] =
{
    4, 14400, 5400, 18000, 8100
};
static const SvxMSDffTextRectangles mso_sptRightArrowCalloutTextRect[] =
{
    { { 0, 0 }, { 0 MSO_I, 21600 } }
};
static const mso_AutoShape msoRightArrowCallout =
{
    (SvxMSDffVertPair*)mso_sptRightArrowCalloutVert, sizeof( mso_sptRightArrowCalloutVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptRightArrowCalloutSegm, sizeof( mso_sptRightArrowCalloutSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptRightArrowCalloutCalc, sizeof( mso_sptRightArrowCalloutCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptRightArrowCalloutDefault,
    (SvxMSDffTextRectangles*)mso_sptRightArrowCalloutTextRect, sizeof( mso_sptRightArrowCalloutTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptLeftArrowCalloutVert[] =   // adjustment1 : x 0 - 21600, adjustment2 : y 0 - 10800
{                                                               // adjustment3 : x 0 - 21600, adjustment4 : y 0 - 10800
    { 0 MSO_I, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0 MSO_I, 21600 },
    { 0 MSO_I, 5 MSO_I }, { 2 MSO_I, 5 MSO_I }, { 2 MSO_I, 4 MSO_I }, { 0, 10800 },
    { 2 MSO_I, 1 MSO_I }, { 2 MSO_I, 3 MSO_I }, { 0 MSO_I, 3 MSO_I }
};
static const sal_uInt16 mso_sptLeftArrowCalloutSegm[] =
{
    0x4000, 0x000a, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptLeftArrowCalloutCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
    { 0x2000, DFF_Prop_adjust3Value, 0, 0 },
    { 0x2000, DFF_Prop_adjust4Value, 0, 0 },
    { 0x8000, 21600, 0, 0x0401 },
    { 0x8000, 21600, 0, 0x0403 }
};
static const sal_Int32 mso_sptLeftArrowCalloutDefault[] =
{
    4, 7200, 5400, 3600, 8100
};
static const SvxMSDffTextRectangles mso_sptLeftArrowCalloutTextRect[] =
{
    { { 0 MSO_I, 0 }, { 21600, 21600 } }
};
static const mso_AutoShape msoLeftArrowCallout =
{
    (SvxMSDffVertPair*)mso_sptLeftArrowCalloutVert, sizeof( mso_sptLeftArrowCalloutVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptLeftArrowCalloutSegm, sizeof( mso_sptLeftArrowCalloutSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptLeftArrowCalloutCalc, sizeof( mso_sptLeftArrowCalloutCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptLeftArrowCalloutDefault,
    (SvxMSDffTextRectangles*)mso_sptLeftArrowCalloutTextRect, sizeof( mso_sptLeftArrowCalloutTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptUpArrowCalloutVert[] =
{
    { 21600, 0 MSO_I }, { 21600, 21600 }, { 0, 21600 }, { 0, 0 MSO_I },
    { 3 MSO_I, 0 MSO_I }, { 3 MSO_I, 2 MSO_I }, { 1 MSO_I, 2 MSO_I }, { 10800, 0 },
    { 4 MSO_I, 2 MSO_I }, { 5 MSO_I, 2 MSO_I }, { 5 MSO_I, 0 MSO_I }
};
static const sal_uInt16 mso_sptUpArrowCalloutSegm[] =
{
    0x4000, 0x000a, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptUpArrowCalloutCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
    { 0x2000, DFF_Prop_adjust3Value, 0, 0 },
    { 0x2000, DFF_Prop_adjust4Value, 0, 0 },
    { 0x8000, 21600, 0, 0x0401 },
    { 0x8000, 21600, 0, 0x0403 }
};
static const sal_Int32 mso_sptUpArrowCalloutDefault[] =
{
    4, 7200, 5400, 3600, 8100
};
static const SvxMSDffTextRectangles mso_sptUpArrowCalloutTextRect[] =
{
    { { 0, 0 MSO_I }, { 21600, 21600 } }
};
static const mso_AutoShape msoUpArrowCallout =
{
    (SvxMSDffVertPair*)mso_sptUpArrowCalloutVert, sizeof( mso_sptUpArrowCalloutVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptUpArrowCalloutSegm, sizeof( mso_sptUpArrowCalloutSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptUpArrowCalloutCalc, sizeof( mso_sptUpArrowCalloutCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptUpArrowCalloutDefault,
    (SvxMSDffTextRectangles*)mso_sptUpArrowCalloutTextRect, sizeof( mso_sptUpArrowCalloutTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptDownArrowCalloutVert[] =
{
    { 0, 0 MSO_I }, { 0, 0 }, { 21600, 0 }, { 21600, 0 MSO_I },
    { 5 MSO_I, 0 MSO_I }, { 5 MSO_I, 2 MSO_I }, { 4 MSO_I, 2 MSO_I }, { 10800, 21600 },
    { 1 MSO_I, 2 MSO_I }, { 3 MSO_I, 2 MSO_I }, { 3 MSO_I, 0 MSO_I }
};
static const sal_uInt16 mso_sptDownArrowCalloutSegm[] =
{
    0x4000, 0x000a, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptDownArrowCalloutCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
    { 0x2000, DFF_Prop_adjust3Value, 0, 0 },
    { 0x2000, DFF_Prop_adjust4Value, 0, 0 },
    { 0x8000, 21600, 0, 0x0401 },
    { 0x8000, 21600, 0, 0x0403 }
};
static const sal_Int32 mso_sptDownArrowCalloutDefault[] =
{
    4, 14400, 5400, 18000, 8100
};
static const SvxMSDffTextRectangles mso_sptDownArrowCalloutTextRect[] =
{
    { { 0, 0 }, { 21600, 0 MSO_I } }
};
static const mso_AutoShape msoDownArrowCallout =
{
    (SvxMSDffVertPair*)mso_sptDownArrowCalloutVert, sizeof( mso_sptDownArrowCalloutVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptDownArrowCalloutSegm, sizeof( mso_sptDownArrowCalloutSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptDownArrowCalloutCalc, sizeof( mso_sptDownArrowCalloutCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDownArrowCalloutDefault,
    (SvxMSDffTextRectangles*)mso_sptDownArrowCalloutTextRect, sizeof( mso_sptDownArrowCalloutTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptLeftRightArrowCalloutVert[] =
{
    { 0 MSO_I, 0 }, { 4 MSO_I, 0 }, { 4 MSO_I, 3 MSO_I }, { 6 MSO_I, 3 MSO_I },
    { 6 MSO_I, 1 MSO_I }, { 21600, 10800 }, { 6 MSO_I, 5 MSO_I }, { 6 MSO_I, 7 MSO_I },
    { 4 MSO_I, 7 MSO_I }, { 4 MSO_I, 21600 }, { 0 MSO_I, 21600 }, { 0 MSO_I, 7 MSO_I },
    { 2 MSO_I, 7 MSO_I }, { 2 MSO_I, 5 MSO_I }, { 0, 10800 }, { 2 MSO_I, 1 MSO_I },
    { 2 MSO_I, 3 MSO_I }, { 0 MSO_I, 3 MSO_I }
};
static const sal_uInt16 mso_sptLeftRightArrowCalloutSegm[] =
{
    0x4000, 0x0011, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptLeftRightArrowCalloutCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
    { 0x2000, DFF_Prop_adjust3Value, 0, 0 },
    { 0x2000, DFF_Prop_adjust4Value, 0, 0 },
    { 0x8000, 21600, 0, 0x0400 },
    { 0x8000, 21600, 0, 0x0401 },
    { 0x8000, 21600, 0, 0x0402 },
    { 0x8000, 21600, 0, 0x0403 }
};
static const sal_Int32 mso_sptLeftRightArrowCalloutDefault[] =
{
    4, 5400, 5500, 2700, 8100
};
static const SvxMSDffTextRectangles mso_sptLeftRightArrowCalloutTextRect[] =
{
    { { 0 MSO_I, 0 }, { 4 MSO_I, 21600 } }
};
static const mso_AutoShape msoLeftRightArrowCallout =
{
    (SvxMSDffVertPair*)mso_sptLeftRightArrowCalloutVert, sizeof( mso_sptLeftRightArrowCalloutVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptLeftRightArrowCalloutSegm, sizeof( mso_sptLeftRightArrowCalloutSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptLeftRightArrowCalloutCalc, sizeof( mso_sptLeftRightArrowCalloutCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptLeftRightArrowCalloutDefault,
    (SvxMSDffTextRectangles*)mso_sptLeftRightArrowCalloutTextRect, sizeof( mso_sptLeftRightArrowCalloutTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptUpDownArrowCalloutVert[] =
{
    { 0, 0 MSO_I }, { 0, 4 MSO_I }, { 3 MSO_I, 4 MSO_I }, { 3 MSO_I, 6 MSO_I },
    { 1 MSO_I, 6 MSO_I }, { 10800, 21600 }, { 5 MSO_I, 6 MSO_I }, { 7 MSO_I, 6 MSO_I },
    { 7 MSO_I, 4 MSO_I }, { 21600, 4 MSO_I }, { 21600, 0 MSO_I }, { 7 MSO_I, 0 MSO_I },
    { 7 MSO_I, 2 MSO_I }, { 5 MSO_I, 2 MSO_I }, { 10800, 0 }, { 1 MSO_I, 2 MSO_I },
    { 3 MSO_I, 2 MSO_I }, { 3 MSO_I, 0 MSO_I }
};
static const sal_uInt16 mso_sptUpDownArrowCalloutSegm[] =
{
    0x4000, 0x0011, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptUpDownArrowCalloutCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
    { 0x2000, DFF_Prop_adjust3Value, 0, 0 },
    { 0x2000, DFF_Prop_adjust4Value, 0, 0 },
    { 0x8000, 21600, 0, 0x0400 },
    { 0x8000, 21600, 0, 0x0401 },
    { 0x8000, 21600, 0, 0x0402 },
    { 0x8000, 21600, 0, 0x0403 }
};
static const sal_Int32 mso_sptUpDownArrowCalloutDefault[] =
{
    4, 5400, 5500, 2700, 8100
};
static const SvxMSDffTextRectangles mso_sptUpDownArrowCalloutTextRect[] =
{
    { { 0, 0 MSO_I }, { 21600, 4 MSO_I } }
};
static const mso_AutoShape msoUpDownArrowCallout =
{
    (SvxMSDffVertPair*)mso_sptUpDownArrowCalloutVert, sizeof( mso_sptUpDownArrowCalloutVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptUpDownArrowCalloutSegm, sizeof( mso_sptUpDownArrowCalloutSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptUpDownArrowCalloutCalc, sizeof( mso_sptUpDownArrowCalloutCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptUpDownArrowCalloutDefault,
    (SvxMSDffTextRectangles*)mso_sptUpDownArrowCalloutTextRect, sizeof( mso_sptUpDownArrowCalloutTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptQuadArrowCalloutVert[] =
{
    { 0 MSO_I, 0 MSO_I }, { 3 MSO_I, 0 MSO_I }, { 3 MSO_I, 2 MSO_I }, { 1 MSO_I, 2 MSO_I },
    { 10800, 0 }, { 5 MSO_I, 2 MSO_I }, { 7 MSO_I, 2 MSO_I }, { 7 MSO_I, 0 MSO_I },
    { 4 MSO_I, 0 MSO_I }, { 4 MSO_I, 3 MSO_I }, { 6 MSO_I, 3 MSO_I }, { 6 MSO_I, 1 MSO_I },
    { 21600, 10800 }, { 6 MSO_I, 5 MSO_I }, { 6 MSO_I, 7 MSO_I }, { 4 MSO_I, 7 MSO_I },
    { 4 MSO_I, 4 MSO_I }, { 7 MSO_I, 4 MSO_I }, { 7 MSO_I, 6 MSO_I }, { 5 MSO_I, 6 MSO_I },
    { 10800, 21600 }, { 1 MSO_I, 6 MSO_I }, { 3 MSO_I, 6 MSO_I }, { 3 MSO_I, 4 MSO_I },
    { 0 MSO_I, 4 MSO_I }, { 0 MSO_I, 7 MSO_I }, { 2 MSO_I, 7 MSO_I }, { 2 MSO_I, 5 MSO_I },
    { 0, 10800 }, { 2 MSO_I, 1 MSO_I }, { 2 MSO_I, 3 MSO_I }, { 0 MSO_I, 3 MSO_I }
};
static const sal_uInt16 mso_sptQuadArrowCalloutSegm[] =
{
    0x4000, 0x001f, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptQuadArrowCalloutCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
    { 0x2000, DFF_Prop_adjust3Value, 0, 0 },
    { 0x2000, DFF_Prop_adjust4Value, 0, 0 },
    { 0x8000, 21600, 0, 0x0400 },
    { 0x8000, 21600, 0, 0x0401 },
    { 0x8000, 21600, 0, 0x0402 },
    { 0x8000, 21600, 0, 0x0403 }
};
static const sal_Int32 mso_sptQuadArrowCalloutDefault[] =
{
    4, 5400, 8100, 2700, 9400
};
static const SvxMSDffTextRectangles mso_sptQuadArrowCalloutTextRect[] =
{
    { { 0 MSO_I, 0 MSO_I }, { 4 MSO_I, 4 MSO_I } }
};
static const mso_AutoShape msoQuadArrowCallout =
{
    (SvxMSDffVertPair*)mso_sptQuadArrowCalloutVert, sizeof( mso_sptQuadArrowCalloutVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptQuadArrowCalloutSegm, sizeof( mso_sptQuadArrowCalloutSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptQuadArrowCalloutCalc, sizeof( mso_sptQuadArrowCalloutCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptQuadArrowCalloutDefault,
    (SvxMSDffTextRectangles*)mso_sptQuadArrowCalloutTextRect, sizeof( mso_sptQuadArrowCalloutTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptCircularArrowVert[] =
{
    { 3 MSO_I, 3 MSO_I }, { 20 MSO_I, 20 MSO_I }, { 19 MSO_I, 18 MSO_I }, { 17 MSO_I, 16 MSO_I },
    { 0, 0 }, { 21600, 21600 }, { 9 MSO_I, 8 MSO_I }, { 11 MSO_I, 10 MSO_I },
    { 24 MSO_I, 23 MSO_I }, { 40 MSO_I, 39 MSO_I }, { 29 MSO_I, 28 MSO_I }
};
static const sal_uInt16 mso_sptCircularArrowSegm[] =
{
    0xa404, 0xa504, 0x0003, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptCircularArrowCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },         // 0
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },        // 1
    { 0x2000, DFF_Prop_adjust3Value, 0, 0 },        // 2
    { 0x8000, 10800, 0,DFF_Prop_adjust3Value },     // 3
    { 0x4009, 10800, DFF_Prop_adjustValue, 0 },     // 4
    { 0x400a, 10800, DFF_Prop_adjustValue, 0 },     // 5
    { 0x4009, 10800, DFF_Prop_adjust2Value, 0 },    // 6
    { 0x400a, 10800, DFF_Prop_adjust2Value, 0 },    // 7
    { 0x2000, 0x0404, 10800, 0 },                   // 8
    { 0x2000, 0x0405, 10800, 0 },                   // 9
    { 0x2000, 0x0406, 10800, 0 },                   // 10
    { 0x2000, 0x0407, 10800, 0 },                   // 11
    { 0x6009, 0x0403, DFF_Prop_adjustValue, 0 },    // 12
    { 0x600a, 0x0403, DFF_Prop_adjustValue, 0 },    // 13
    { 0x6009, 0x0403, DFF_Prop_adjust2Value, 0 },   // 14
    { 0x600a, 0x0403, DFF_Prop_adjust2Value, 0 },   // 15
    { 0x2000, 0x040c, 10800, 0 },                   // 16
    { 0x2000, 0x040d, 10800, 0 },                   // 17
    { 0x2000, 0x040e, 10800, 0 },                   // 18
    { 0x2000, 0x040f, 10800, 0 },                   // 19
    { 0x8000, 21600, 0, 0x0403 },                   // 20
    { 0x4009, 12600, DFF_Prop_adjust2Value, 0 },    // 21
    { 0x400a, 12600, DFF_Prop_adjust2Value, 0 },    // 22
    { 0x2000, 0x0415, 10800, 0 },                   // 23
    { 0x2000, 0x0416, 10800, 0 },                   // 24
    { 0x2000, DFF_Prop_adjust3Value, 0, 1800 },     // 25
    { 0x6009, 0x0419, DFF_Prop_adjust2Value, 0 },   // 26
    { 0x600a, 0x0419, DFF_Prop_adjust2Value, 0 },   // 27
    { 0x2000, 0x041a, 10800, 0 },                   // 28
    { 0x2000, 0x041b, 10800, 0 },                   // 29
    { 0x2001, 0x0403, 1, 2 },                       // 30
    { 0x8000, 10800, 0, 0x041e },                   // 31
    { 0x4001, 35,   0x0403, 10800 },                // 32
    { 0x2000, 0x0420, 10, 0 },                      // 33
    { 0x2001, 0x0421, 256, 1 },                     // 34
    { 0x2001, 0x0422, 256, 1 },                     // 35
    { 0x6000, 0x0423, DFF_Prop_adjust2Value, 0 },   // 36
    { 0x6009, 0x041f, 0x0424, 0 },                  // 37
    { 0x600a, 0x041f, 0x0424, 0 },                  // 38
    { 0x2000, 0x0425, 10800, 0 },                   // 39
    { 0x2000, 0x0426, 10800, 0 }                    // 40
};
static const sal_Int32 mso_sptCircularArrowDefault[] =
{
    3, 180 << 16, 0, 5500
};
static const SvxMSDffTextRectangles mso_sptCircularArrowTextRect[] =    // todo
{
    { { 0, 0 }, { 21600, 21600 } }
};
static const mso_AutoShape msoCircularArrow =
{
    (SvxMSDffVertPair*)mso_sptCircularArrowVert, sizeof( mso_sptCircularArrowVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptCircularArrowSegm, sizeof( mso_sptCircularArrowSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptCircularArrowCalc, sizeof( mso_sptCircularArrowCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptCircularArrowDefault,
    (SvxMSDffTextRectangles*)mso_sptCircularArrowTextRect, sizeof( mso_sptCircularArrowTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptCubeVert[] =
{
    { 0, 21600 }, { 0, 1 MSO_I }, { 2 MSO_I, 0 }, { 21600, 0 },
    { 21600, 3 MSO_I }, { 4 MSO_I, 21600 }, { 0, 1 MSO_I }, { 2 MSO_I, 0 },
    { 21600, 0 }, { 4 MSO_I, 1 MSO_I }, { 4 MSO_I, 21600 }, { 4 MSO_I, 1 MSO_I },
    { 21600, 0 }, { 21600, 3 MSO_I }
};
static const sal_uInt16 mso_sptCubeSegm[] =
{
    0x4000, 0x0005, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptCubeCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x6000, DFF_Prop_geoTop, 0x400, 0 },
    { 0x6000, DFF_Prop_geoLeft, 0x400, 0 },
    { 0xa000, DFF_Prop_geoBottom, 0, 0x400 },
    { 0xa000, DFF_Prop_geoRight, 0, 0x400 },
    { 0x8000,  21600, 0, 0x402 },   // 5
    { 0x2001, 0x405, 1, 2 },        // 6
    { 0x6000, 0x402, 0x406, 0 },    // 7
    { 0x2001, 0x404, 1, 2 }         // 8

};
static const SvxMSDffTextRectangles mso_sptCubeTextRect[] =
{
    { { 0, 1 MSO_I }, { 4 MSO_I, 21600 } }
};
static const mso_AutoShape msoCube =
{
    (SvxMSDffVertPair*)mso_sptCubeVert, sizeof( mso_sptCubeVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptCubeSegm, sizeof( mso_sptCubeSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptCubeCalc, sizeof( mso_sptCubeCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault5400,
    (SvxMSDffTextRectangles*)mso_sptCubeTextRect, sizeof( mso_sptCubeTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    10800, 10800,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptBevelVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },
    { 0, 0 }, { 21600, 0 }, { 1 MSO_I, 0 MSO_I }, { 0 MSO_I, 0 MSO_I },
    { 21600, 0 }, { 21600, 21600 }, { 1 MSO_I, 1 MSO_I }, { 1 MSO_I, 0 MSO_I },
    { 21600, 21600 }, { 0, 21600 }, { 0 MSO_I, 1 MSO_I }, { 1 MSO_I, 1 MSO_I },
    { 0, 21600 }, { 0, 0 }, { 0 MSO_I, 0 MSO_I }, { 0 MSO_I, 1 MSO_I }
};
static const sal_uInt16 mso_sptBevelSegm[] =
{
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptBevelCalc[] =
{
    { 0x2001, DFF_Prop_adjustValue, 21599, 21600 },
    { 0x8000, 21600, 0, 0x400 }
};
static const SvxMSDffTextRectangles mso_sptBevelTextRect[] =
{
    { { 0 MSO_I, 0 MSO_I }, { 1 MSO_I, 1 MSO_I } }
};
static const mso_AutoShape msoBevel =
{
    (SvxMSDffVertPair*)mso_sptBevelVert, sizeof( mso_sptBevelVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptBevelSegm, sizeof( mso_sptBevelSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptBevelCalc, sizeof( mso_sptBevelCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault2700,
    (SvxMSDffTextRectangles*)mso_sptBevelTextRect, sizeof( mso_sptBevelTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    10800, 10800,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptFoldedCornerVert[] =   // adjustment1 : x 10800 - 21600
{
    { 0, 0 }, { 21600, 0 }, { 21600, 0 MSO_I }, { 0 MSO_I, 21600 },
    { 0, 21600 }, { 0 MSO_I, 21600 }, { 3 MSO_I, 0 MSO_I }, { 8 MSO_I, 9 MSO_I },
    { 10 MSO_I, 11 MSO_I }, { 21600, 0 MSO_I }
};
static const sal_uInt16 mso_sptFoldedCornerSegm[] =
{
    0x4000, 0x0004, 0x6001, 0x8000,
    0x4000, 0x0001, 0x2001, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptFoldedCornerCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x8000, 21600, 0, 0x400 },
    { 0x2001, 0x0401, 8000, 10800 },
    { 0x8000, 21600, 0, 0x0402 },
    { 0x2001, 0x0401, 1, 2 },
    { 0x2001, 0x0401, 1, 4 },
    { 0x2001, 0x0401, 1, 7 },
    { 0x2001, 0x0401, 1, 16 },
    { 0x6000, 0x0403, 0x405, 0 },
    { 0x6000, 0x0400, 0x406, 0 },
    { 0x8000, 21600, 0, 0x404 },
    { 0x6000, 0x400, 0x407, 0 }
};
static const sal_Int32 mso_sptFoldedCornerDefault[] =
{
    1, 18900
};
static const SvxMSDffTextRectangles mso_sptFoldedCornerTextRect[] =
{
    { { 0, 0 }, { 21600, 11 MSO_I } }
};
static const mso_AutoShape msoFoldedCorner =
{
    (SvxMSDffVertPair*)mso_sptFoldedCornerVert, sizeof( mso_sptFoldedCornerVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFoldedCornerSegm, sizeof( mso_sptFoldedCornerSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptFoldedCornerCalc, sizeof( mso_sptFoldedCornerCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptFoldedCornerDefault,
    (SvxMSDffTextRectangles*)mso_sptFoldedCornerTextRect, sizeof( mso_sptFoldedCornerTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptActionButtonBlankVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },
    { 0, 0 }, { 21600, 0 }, { 1 MSO_I, 0 MSO_I }, { 0 MSO_I, 0 MSO_I },
    { 21600, 0 }, { 21600, 21600 }, { 1 MSO_I, 1 MSO_I }, { 1 MSO_I, 0 MSO_I },
    { 21600, 21600 }, { 0, 21600 }, { 0 MSO_I, 1 MSO_I }, { 1 MSO_I, 1 MSO_I },
    { 0, 21600 }, { 0, 0 }, { 0 MSO_I, 0 MSO_I }, { 0 MSO_I, 1 MSO_I }
};
static const sal_uInt16 mso_sptActionButtonBlankSegm[] =
{
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptActionButtonBlankCalc[] =
{
    { 0x2001, DFF_Prop_adjustValue, 21599, 21600 },
    { 0x8000, 21600, 0, 0x400 }
};
static const SvxMSDffTextRectangles mso_sptActionButtonBlankTextRect[] =
{
    { { 0 MSO_I, 0 MSO_I }, { 1 MSO_I, 1 MSO_I } }
};
static const mso_AutoShape msoActionButtonBlank =
{
    (SvxMSDffVertPair*)mso_sptActionButtonBlankVert, sizeof( mso_sptActionButtonBlankVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptActionButtonBlankSegm, sizeof( mso_sptActionButtonBlankSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptActionButtonBlankCalc, sizeof( mso_sptActionButtonBlankCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1400,
    (SvxMSDffTextRectangles*)mso_sptActionButtonBlankTextRect, sizeof( mso_sptActionButtonBlankTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    10800, 10800,
    NULL, 0
};

static const SvxMSDffTextRectangles mso_sptActionButtonTextRect[] =
{
    { { 1 MSO_I, 2 MSO_I }, { 3 MSO_I, 4 MSO_I } }
};
static const SvxMSDffVertPair mso_sptActionButtonHomeVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },
    { 0, 0 }, { 21600, 0 }, { 3 MSO_I, 2 MSO_I }, { 1 MSO_I, 2 MSO_I },
    { 21600, 0 }, { 21600, 21600 }, { 3 MSO_I, 4 MSO_I }, { 3 MSO_I, 2 MSO_I },
    { 21600, 21600 }, { 0, 21600 }, { 1 MSO_I, 4 MSO_I }, { 3 MSO_I, 4 MSO_I },
    { 0, 21600 }, { 0, 0 }, { 1 MSO_I, 2 MSO_I }, { 1 MSO_I, 4 MSO_I },

    { 10800, 0xa MSO_I }, { 0xc MSO_I, 0xe MSO_I }, { 0xc MSO_I, 0x10 MSO_I }, { 0x12 MSO_I, 0x10 MSO_I },
    { 0x12 MSO_I, 0x14 MSO_I }, { 0x16 MSO_I, 10800 }, { 0x18 MSO_I, 10800 }, { 0x18 MSO_I, 0x1a MSO_I },
    { 0x1c MSO_I, 0x1a MSO_I }, { 0x1c MSO_I, 10800 }, { 0x1e MSO_I, 10800 },

    { 0xc MSO_I, 0xe MSO_I }, { 0xc MSO_I, 0x10 MSO_I }, { 0x12 MSO_I, 0x10 MSO_I },{ 0x12 MSO_I, 0x14 MSO_I },

    { 0x20 MSO_I, 0x24 MSO_I }, { 0x22 MSO_I, 0x24 MSO_I }, { 0x22 MSO_I, 0x1a MSO_I }, { 0x18 MSO_I, 0x1a MSO_I },
    { 0x18 MSO_I, 10800 }, { 0x1c MSO_I, 10800 }, { 0x1c MSO_I, 0x1a MSO_I }, { 0x20 MSO_I, 0x1a MSO_I }

};
static const sal_uInt16 mso_sptActionButtonHomeSegm[] =
{
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x000a, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0007, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptActionButtonHomeCalc[] =    // adj value 0 - 5400
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x6000, DFF_Prop_geoLeft, DFF_Prop_adjustValue, 0 },
    { 0x6000, DFF_Prop_geoTop, DFF_Prop_adjustValue, 0 },
    { 0xa000, DFF_Prop_geoRight, 0, DFF_Prop_adjustValue },
    { 0xa000, DFF_Prop_geoBottom, 0, DFF_Prop_adjustValue },
    { 0x8000, 10800, 0, DFF_Prop_adjustValue },
    { 0x2001, 0x0405, 1, 10800 },                           // scaling   6
    { 0x6000, DFF_Prop_geoRight, DFF_Prop_geoLeft, 10800 }, // lr center 7
    { 0x6000, DFF_Prop_geoBottom, DFF_Prop_geoTop, 10800 }, // ul center 8

    { 0x4001, -8000, 0x0406, 1 },   // 9
    { 0x6000, 0x0409, 0x0408, 0 },  // a
    { 0x4001, 2960, 0x0406, 1 },    // b
    { 0x6000, 0x040b, 0x0407, 0 },  // c
    { 0x4001, -5000, 0x0406, 1 },   // d
    { 0x6000, 0x040d, 0x0408, 0 },  // e
    { 0x4001, -7000, 0x0406, 1 },   // f
    { 0x6000, 0x040f, 0x0408, 0 },  // 10
    { 0x4001, 5000, 0x0406, 1 },    // 11
    { 0x6000, 0x0411, 0x0407, 0 },  // 12
    { 0x4001, -2960, 0x0406, 1 },   // 13
    { 0x6000, 0x0413, 0x0408, 0 },  // 14
    { 0x4001, 8000, 0x0406, 1 },    // 15
    { 0x6000, 0x0415,0x0407, 0 },   // 16
    { 0x4001, 6100, 0x0406, 1 },    // 17
    { 0x6000, 0x0417,0x0407, 0 },   // 18
    { 0x4001, 8260, 0x0406, 1 },    // 19
    { 0x6000, 0x0419, 0x0408, 0 },  // 1a
    { 0x4001, -6100, 0x0406, 1 },   // 1b
    { 0x6000, 0x041b, 0x0407, 0 },  // 1c
    { 0x4001, -8000, 0x0406, 1 },   // 1d
    { 0x6000, 0x041d, 0x0407, 0 },  // 1e
    { 0x4001, -1060, 0x0406, 1 },   // 1f
    { 0x6000, 0x041f, 0x0407, 0 },  // 20
    { 0x4001, 1060, 0x0406, 1 },    // 21
    { 0x6000, 0x0421, 0x0407, 0 },  // 22
    { 0x4001, 4020, 0x0406, 1 },    // 23
    { 0x6000, 0x0423, 0x0408, 0 }   // 24

};
static const mso_AutoShape msoActionButtonHome =
{
    (SvxMSDffVertPair*)mso_sptActionButtonHomeVert, sizeof( mso_sptActionButtonHomeVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptActionButtonHomeSegm, sizeof( mso_sptActionButtonHomeSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptActionButtonHomeCalc, sizeof( mso_sptActionButtonHomeCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1400,
    (SvxMSDffTextRectangles*)mso_sptActionButtonTextRect, sizeof( mso_sptActionButtonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptActionButtonHelpVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },
    { 0, 0 }, { 21600, 0 }, { 3 MSO_I, 2 MSO_I }, { 1 MSO_I, 2 MSO_I },
    { 21600, 0 }, { 21600, 21600 }, { 3 MSO_I, 4 MSO_I }, { 3 MSO_I, 2 MSO_I },
    { 21600, 21600 }, { 0, 21600 }, { 1 MSO_I, 4 MSO_I }, { 3 MSO_I, 4 MSO_I },
    { 0, 21600 }, { 0, 0 }, { 1 MSO_I, 2 MSO_I }, { 1 MSO_I,4 MSO_I },

    { 0xa MSO_I, 0xc MSO_I }, { 0xe MSO_I, 0x10 MSO_I },

    { 0x12 MSO_I, 0x14 MSO_I }, { 0x12 MSO_I, 0x16 MSO_I },                             // pp
    { 0x12 MSO_I, 0x18 MSO_I }, { 0x1a MSO_I, 10800 }, { 0x1c MSO_I, 10800 },           // ccp
    { 0x1e MSO_I, 10800 }, { 0x20 MSO_I, 0x22 MSO_I }, { 0x20 MSO_I, 0x24 MSO_I },      // ccp
    { 0x20 MSO_I, 0x26 MSO_I }, { 0x28 MSO_I, 0x2a MSO_I }, { 10800, 0x2a MSO_I },      // ccp
    { 0x2c MSO_I, 0x2a MSO_I }, { 0x2e MSO_I, 0x26 MSO_I }, { 0x2e MSO_I, 0x24 MSO_I }, // ccp
    { 0x30 MSO_I, 0x24 MSO_I }, { 0x30 MSO_I, 0x32 MSO_I }, { 0x34 MSO_I, 0x36 MSO_I }, // ccp
    { 10800, 0x36 MSO_I },                                                              // p
    { 0x12 MSO_I, 0x36 MSO_I }, { 0x1c MSO_I, 0x32 MSO_I }, { 0x1c MSO_I, 0x24 MSO_I }, // ccp
    { 0x1c MSO_I, 0x38 MSO_I }, { 0x3a MSO_I, 0x3c MSO_I }, { 0x12 MSO_I, 0x3c MSO_I }, // ccp
    { 10800, 0x3c MSO_I }, { 0x34 MSO_I, 10800 }, { 0x34 MSO_I, 0x16 MSO_I },           // ccp
    { 0x34 MSO_I, 0x14 MSO_I }
};
static const sal_uInt16 mso_sptActionButtonHelpSegm[] =
{
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0xa302, 0x6000, 0x8000,
    0x4000, 0x0001, 0x2004, 0x0001, 0x2004, 0x0001, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptActionButtonHelpCalc[] =    // adj value 0 - 5400
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x6000, DFF_Prop_geoLeft, DFF_Prop_adjustValue, 0 },
    { 0x6000, DFF_Prop_geoTop, DFF_Prop_adjustValue, 0 },
    { 0xa000, DFF_Prop_geoRight, 0, DFF_Prop_adjustValue },
    { 0xa000, DFF_Prop_geoBottom, 0, DFF_Prop_adjustValue },
    { 0x8000, 10800, 0, DFF_Prop_adjustValue },
    { 0x2001, 0x0405, 1, 10800 },                           // scaling   6
    { 0x6000, DFF_Prop_geoRight, DFF_Prop_geoLeft, 10800 }, // lr center 7
    { 0x6000, DFF_Prop_geoBottom, DFF_Prop_geoTop, 10800 }, // ul center 8

    { 0x4001, -1690, 0x0406, 1 },   // 9
    { 0x6000, 0x0409, 0x0407, 0 },  // a
    { 0x4001, 4600, 0x0406, 1 },    // b
    { 0x6000, 0x040b, 0x0408, 0 },  // c
    { 0x4001, 1690, 0x0406, 1 },    // d
    { 0x6000, 0x040d, 0x0407, 0 },  // e
    { 0x4001, 7980, 0x0406, 1 },    // f
    { 0x6000, 0x040f, 0x0408, 0 },  // 10
    { 0x4001, 1270, 0x0406, 1 },    // 11
    { 0x6000, 0x0411, 0x0407, 0 },  // 12
    { 0x4001, 4000, 0x0406, 1 },    // 13
    { 0x6000, 0x0413, 0x0408, 0 },  // 14
    { 0x4001, 1750, 0x0406, 1 },    // 15
    { 0x6000, 0x0415, 0x0408, 0 },  // 16
    { 0x4001, 800, 0x0406, 1 },     // 17
    { 0x6000, 0x0417, 0x0408, 0 },  // 18
    { 0x4001, 1650, 0x0406, 1 },    // 19
    { 0x6000, 0x0419, 0x0407, 0 },  // 1a
    { 0x4001, 2340, 0x0406, 1 },    // 1b
    { 0x6000, 0x041b, 0x0407, 0 },  // 1c
    { 0x4001, 3640, 0x0406, 1 },    // 1d
    { 0x6000, 0x041d, 0x0407, 0 },  // 1e
    { 0x4001, 4670, 0x0406, 1 },    // 1f
    { 0x6000, 0x041f, 0x0407, 0 },  // 20
    { 0x4001, -1570, 0x0406, 1 },   // 21
    { 0x6000, 0x0421, 0x0408, 0 },  // 22
    { 0x4001, -3390, 0x0406, 1 },   // 23
    { 0x6000, 0x0423, 0x0408, 0 },  // 24
    { 0x4001, -6050, 0x0406, 1 },   // 25
    { 0x6000, 0x0425, 0x0408, 0 },  // 26
    { 0x4001, 2540, 0x0406, 1 },    // 27
    { 0x6000, 0x0427, 0x0407, 0 },  // 28
    { 0x4001, -8050, 0x0406, 1 },   // 29
    { 0x6000, 0x0429, 0x0408, 0 },  // 2a
    { 0x4001, -2540, 0x0406, 1 },   // 2b
    { 0x6000, 0x042b, 0x0407, 0 },  // 2c
    { 0x4001, -4460, 0x0406, 1 },   // 2d
    { 0x6000, 0x042d, 0x0407, 0 },  // 2e
    { 0x4001, -2330, 0x0406, 1 },   // 2f
    { 0x6000, 0x042f, 0x0407, 0 },  // 30
    { 0x4001, -4700, 0x0406, 1 },   // 31
    { 0x6000, 0x0431, 0x0408, 0 },  // 32
    { 0x4001, -1270, 0x0406, 1 },   // 33
    { 0x6000, 0x0433, 0x0407, 0 },  // 34
    { 0x4001, -5720, 0x0406, 1 },   // 35
    { 0x6000, 0x0435, 0x0408, 0 },  // 36
    { 0x4001, -2540, 0x0406, 1 },   // 37
    { 0x6000, 0x0437, 0x0408, 0 },  // 38
    { 0x4001, 1800, 0x0406, 1 },    // 39
    { 0x6000, 0x0439, 0x0407, 0 },  // 3a
    { 0x4001, -1700, 0x0406, 1 },   // 3b
    { 0x6000, 0x043b, 0x0408, 0 }   // 3c
};
static const mso_AutoShape msoActionButtonHelp =
{
    (SvxMSDffVertPair*)mso_sptActionButtonHelpVert, sizeof( mso_sptActionButtonHelpVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptActionButtonHelpSegm, sizeof( mso_sptActionButtonHelpSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptActionButtonHelpCalc, sizeof( mso_sptActionButtonHelpCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1400,
    (SvxMSDffTextRectangles*)mso_sptActionButtonTextRect, sizeof( mso_sptActionButtonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptActionButtonInformationVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },
    { 0, 0 }, { 21600, 0 }, { 3 MSO_I, 2 MSO_I }, { 1 MSO_I, 2 MSO_I },
    { 21600, 0 }, { 21600, 21600 }, { 3 MSO_I, 4 MSO_I }, { 3 MSO_I, 2 MSO_I },
    { 21600, 21600 }, { 0, 21600 }, { 1 MSO_I, 4 MSO_I }, { 3 MSO_I, 4 MSO_I },
    { 0, 21600 }, { 0, 0 }, { 1 MSO_I, 2 MSO_I }, { 1 MSO_I, 4 MSO_I },

    { 0xa MSO_I, 0xc MSO_I }, { 0xe MSO_I, 0x10 MSO_I },

    { 0x12 MSO_I, 0x14 MSO_I }, { 0x16 MSO_I, 0x18 MSO_I },

    { 0x1a MSO_I, 0x1c MSO_I }, { 0x1e MSO_I, 0x1c MSO_I }, { 0x1e MSO_I, 0x20 MSO_I }, { 0x22 MSO_I, 0x20 MSO_I },
    { 0x22 MSO_I, 0x24 MSO_I }, { 0x1a MSO_I, 0x24 MSO_I }, { 0x1a MSO_I, 0x20 MSO_I }, { 0x26 MSO_I, 0x20 MSO_I },
    { 0x26 MSO_I, 0x28 MSO_I }, { 0x1a MSO_I, 0x28 MSO_I }
};
static const sal_uInt16 mso_sptActionButtonInformationSegm[] =
{
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0xa302, 0x6000, 0x8000,
    0xa302, 0x6000, 0x8000,
    0x4000, 0x0009, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptActionButtonInformationCalc[] = // adj value 0 - 5400
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x6000, DFF_Prop_geoLeft, DFF_Prop_adjustValue, 0 },
    { 0x6000, DFF_Prop_geoTop, DFF_Prop_adjustValue, 0 },
    { 0xa000, DFF_Prop_geoRight, 0, DFF_Prop_adjustValue },
    { 0xa000, DFF_Prop_geoBottom, 0, DFF_Prop_adjustValue },
    { 0x8000, 10800, 0, DFF_Prop_adjustValue },
    { 0x2001, 0x0405, 1, 10800 },                           // scaling   6
    { 0x6000, DFF_Prop_geoRight, DFF_Prop_geoLeft, 10800 }, // lr center 7
    { 0x6000, DFF_Prop_geoBottom, DFF_Prop_geoTop, 10800 }, // ul center 8

    { 0x4001, -8050, 0x0406, 1 },   // 9
    { 0x6000, 0x0409, 0x0407, 0 },  // a
    { 0x4001, -8050, 0x0406, 1 },   // b
    { 0x6000, 0x040b, 0x0408, 0 },  // c
    { 0x4001, 8050, 0x0406, 1 },    // d
    { 0x6000, 0x040d, 0x0407, 0 },  // e
    { 0x4001, 8050, 0x0406, 1 },    // f
    { 0x6000, 0x040f, 0x0408, 0 },  // 10

    { 0x4001, -2060, 0x0406, 1 },   // 11
    { 0x6000, 0x0411, 0x0407, 0 },  // 12
    { 0x4001, -7620, 0x0406, 1 },   // 13
    { 0x6000, 0x0413, 0x0408, 0 },  // 14
    { 0x4001, 2060, 0x0406, 1 },    // 15
    { 0x6000, 0x0415, 0x0407, 0 },  // 16
    { 0x4001, -3500, 0x0406, 1 },   // 17
    { 0x6000, 0x0417, 0x0408, 0 },  // 18

    { 0x4001, -2960, 0x0406, 1 },   // 19
    { 0x6000, 0x0419, 0x0407, 0 },  // 1a
    { 0x4001, -2960, 0x0406, 1 },   // 1b
    { 0x6000, 0x041b, 0x0408, 0 },  // 1c
    { 0x4001, 1480, 0x0406, 1 },    // 1d
    { 0x6000, 0x041d, 0x0407, 0 },  // 1e
    { 0x4001, 5080, 0x0406, 1 },    // 1f
    { 0x6000, 0x041f, 0x0408, 0 },  // 20
    { 0x4001, 2960, 0x0406, 1 },    // 21
    { 0x6000, 0x0421, 0x0407, 0 },  // 22
    { 0x4001, 6140, 0x0406, 1 },    // 23
    { 0x6000, 0x0423, 0x0408, 0 },  // 24
    { 0x4001, -1480, 0x0406, 1 },   // 25
    { 0x6000, 0x0425, 0x0407, 0 },  // 26
    { 0x4001, -1920, 0x0406, 1 },   // 27
    { 0x6000, 0x0427, 0x0408, 0 }   // 28
};
static const mso_AutoShape msoActionButtonInformation =
{
    (SvxMSDffVertPair*)mso_sptActionButtonInformationVert, sizeof( mso_sptActionButtonInformationVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptActionButtonInformationSegm, sizeof( mso_sptActionButtonInformationSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptActionButtonInformationCalc, sizeof( mso_sptActionButtonInformationCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1400,
    (SvxMSDffTextRectangles*)mso_sptActionButtonTextRect, sizeof( mso_sptActionButtonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptActionButtonBackPreviousVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },
    { 0, 0 }, { 21600, 0 }, { 3 MSO_I, 2 MSO_I }, { 1 MSO_I, 2 MSO_I },
    { 21600, 0 }, { 21600, 21600 }, { 3 MSO_I, 4 MSO_I }, { 3 MSO_I, 2 MSO_I },
    { 21600, 21600 }, { 0, 21600 }, { 1 MSO_I, 4 MSO_I }, { 3 MSO_I,4 MSO_I },
    { 0, 21600 }, { 0, 0 }, { 1 MSO_I, 2 MSO_I }, { 1 MSO_I, 4 MSO_I },

    { 0xa MSO_I, 10800 }, { 0xe MSO_I, 0xc MSO_I }, { 0xe MSO_I, 0x10 MSO_I }
};
static const sal_uInt16 mso_sptActionButtonForwardBackSegm[] =
{
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0002, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptActionButtonForwardBackCalc[] = // adj value 0 - 5400
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x6000, DFF_Prop_geoLeft, DFF_Prop_adjustValue, 0 },
    { 0x6000, DFF_Prop_geoTop, DFF_Prop_adjustValue, 0 },
    { 0xa000, DFF_Prop_geoRight, 0, DFF_Prop_adjustValue },
    { 0xa000, DFF_Prop_geoBottom, 0, DFF_Prop_adjustValue },
    { 0x8000, 10800, 0, DFF_Prop_adjustValue },
    { 0x2001, 0x0405, 1, 10800 },                           // scaling   6
    { 0x6000, DFF_Prop_geoRight, DFF_Prop_geoLeft, 10800 }, // lr center 7
    { 0x6000, DFF_Prop_geoBottom, DFF_Prop_geoTop, 10800 }, // ul center 8

    { 0x4001, -8050, 0x0406, 1 },   // 9
    { 0x6000, 0x0409, 0x0407, 0 },  // a
    { 0x4001, -8050, 0x0406, 1 },   // b
    { 0x6000, 0x040b, 0x0408, 0 },  // c
    { 0x4001, 8050, 0x0406, 1 },    // d
    { 0x6000, 0x040d, 0x0407, 0 },  // e
    { 0x4001, 8050, 0x0406, 1 },    // f
    { 0x6000, 0x040f, 0x0408, 0 }   // 10
};
static const mso_AutoShape msoActionButtonBackPrevious =
{
    (SvxMSDffVertPair*)mso_sptActionButtonBackPreviousVert, sizeof( mso_sptActionButtonBackPreviousVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptActionButtonForwardBackSegm, sizeof( mso_sptActionButtonForwardBackSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptActionButtonForwardBackCalc, sizeof( mso_sptActionButtonForwardBackCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1400,
    (SvxMSDffTextRectangles*)mso_sptActionButtonTextRect, sizeof( mso_sptActionButtonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptActionButtonForwardNextVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },
    { 0, 0 }, { 21600, 0 }, { 3 MSO_I, 2 MSO_I }, { 1 MSO_I, 2 MSO_I },
    { 21600, 0 }, { 21600, 21600 }, { 3 MSO_I, 4 MSO_I }, { 3 MSO_I, 2 MSO_I },
    { 21600, 21600 }, { 0, 21600 }, { 1 MSO_I, 4 MSO_I }, { 3 MSO_I, 4 MSO_I },
    { 0, 21600 }, { 0, 0 }, { 1 MSO_I, 2 MSO_I }, { 1 MSO_I, 4 MSO_I },

    { 0xa MSO_I, 0xc MSO_I }, { 0xe MSO_I, 10800 }, { 0xa MSO_I, 0x10 MSO_I }
};
static const mso_AutoShape msoActionButtonForwardNext =
{
    (SvxMSDffVertPair*)mso_sptActionButtonForwardNextVert, sizeof( mso_sptActionButtonForwardNextVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptActionButtonForwardBackSegm, sizeof( mso_sptActionButtonForwardBackSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptActionButtonForwardBackCalc, sizeof( mso_sptActionButtonForwardBackCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1400,
    (SvxMSDffTextRectangles*)mso_sptActionButtonTextRect, sizeof( mso_sptActionButtonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptActionButtonBeginningVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },
    { 0, 0 }, { 21600, 0 }, { 3 MSO_I, 2 MSO_I }, { 1 MSO_I, 2 MSO_I },
    { 21600, 0 }, { 21600, 21600 }, { 3 MSO_I, 4 MSO_I }, { 3 MSO_I, 2 MSO_I },
    { 21600, 21600 }, { 0, 21600 }, { 1 MSO_I, 4 MSO_I }, { 3 MSO_I, 4 MSO_I },
    { 0, 21600 }, { 0, 0 }, { 1 MSO_I, 2 MSO_I }, { 1 MSO_I, 4 MSO_I },

    { 0xa MSO_I, 10800 }, { 0xe MSO_I, 0xc MSO_I }, { 0xe MSO_I, 0x10 MSO_I }, { 0x12 MSO_I, 0xc MSO_I },
    { 0x14 MSO_I, 0xc MSO_I }, { 0x14 MSO_I, 0x10 MSO_I }, { 0x12 MSO_I, 0x10 MSO_I }
};
static const sal_uInt16 mso_sptActionButtonBeginningEndSegm[] =
{
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,

    0x4000, 0x0002, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptActionButtonBeginningEndCalc[] =    // adj value 0 - 5400
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x6000, DFF_Prop_geoLeft, DFF_Prop_adjustValue, 0 },
    { 0x6000, DFF_Prop_geoTop, DFF_Prop_adjustValue, 0 },
    { 0xa000, DFF_Prop_geoRight, 0, DFF_Prop_adjustValue },
    { 0xa000, DFF_Prop_geoBottom, 0, DFF_Prop_adjustValue },
    { 0x8000, 10800, 0, DFF_Prop_adjustValue },
    { 0x2001, 0x0405, 1, 10800 },                           // scaling   6
    { 0x6000, DFF_Prop_geoRight, DFF_Prop_geoLeft, 10800 }, // lr center 7
    { 0x6000, DFF_Prop_geoBottom, DFF_Prop_geoTop, 10800 }, // ul center 8

    { 0x4001, -4020, 0x0406, 1 },   // 9
    { 0x6000, 0x0409, 0x0407, 0 },  // a
    { 0x4001, -8050, 0x0406, 1 },   // b
    { 0x6000, 0x040b, 0x0408, 0 },  // c
    { 0x4001, 8050, 0x0406, 1 },    // d
    { 0x6000, 0x040d, 0x0407, 0 },  // e
    { 0x4001, 8050, 0x0406, 1 },    // f
    { 0x6000, 0x040f, 0x0408, 0 },  // 10

    { 0x4001, -8050, 0x0406, 1 },   // 11
    { 0x6000, 0x0411, 0x0407, 0 },  // 12
    { 0x4001, -6140, 0x0406, 1 },   // 13
    { 0x6000, 0x0413, 0x0407, 0 },  // 14


    { 0x4001, 4020, 0x0406, 1 },    // 15
    { 0x6000, 0x0415, 0x0407, 0 },  // 16
    { 0x4001, 6140, 0x0406, 1 },    // 17
    { 0x6000, 0x0417, 0x0407, 0 }   // 18
};
static const mso_AutoShape msoActionButtonBeginning =
{
    (SvxMSDffVertPair*)mso_sptActionButtonBeginningVert, sizeof( mso_sptActionButtonBeginningVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptActionButtonBeginningEndSegm, sizeof( mso_sptActionButtonBeginningEndSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptActionButtonBeginningEndCalc, sizeof( mso_sptActionButtonBeginningEndCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1400,
    (SvxMSDffTextRectangles*)mso_sptActionButtonTextRect, sizeof( mso_sptActionButtonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptActionButtonEndVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },
    { 0, 0 }, { 21600, 0 }, { 3 MSO_I, 2 MSO_I }, { 1 MSO_I, 2 MSO_I },
    { 21600, 0 }, { 21600, 21600 }, { 3 MSO_I, 4 MSO_I }, { 3 MSO_I, 2 MSO_I },
    { 21600, 21600 }, { 0, 21600 }, { 1 MSO_I, 4 MSO_I }, { 3 MSO_I, 4 MSO_I },
    { 0, 21600 }, { 0, 0 }, { 1 MSO_I, 2 MSO_I }, { 1 MSO_I, 4 MSO_I },

    { 0x16 MSO_I, 10800 }, { 0x12 MSO_I, 0x10 MSO_I }, { 0x12 MSO_I, 0xc MSO_I },

    { 0x18 MSO_I, 0xc MSO_I }, { 0x18 MSO_I, 0x10 MSO_I }, { 0xe MSO_I, 0x10 MSO_I }, { 0xe MSO_I, 0xc MSO_I }
};
static const mso_AutoShape msoActionButtonEnd =
{
    (SvxMSDffVertPair*)mso_sptActionButtonEndVert, sizeof( mso_sptActionButtonEndVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptActionButtonBeginningEndSegm, sizeof( mso_sptActionButtonBeginningEndSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptActionButtonBeginningEndCalc, sizeof( mso_sptActionButtonBeginningEndCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1400,
    (SvxMSDffTextRectangles*)mso_sptActionButtonTextRect, sizeof( mso_sptActionButtonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptActionButtonReturnVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },
    { 0, 0 }, { 21600, 0 }, { 3 MSO_I, 2 MSO_I }, { 1 MSO_I, 2 MSO_I },
    { 21600, 0 }, { 21600,  21600 }, { 3 MSO_I, 4 MSO_I }, { 3 MSO_I, 2 MSO_I },
    { 21600, 21600 }, { 0, 21600 }, { 1 MSO_I, 4 MSO_I }, { 3 MSO_I, 4 MSO_I },
    { 0, 21600 }, { 0, 0 }, { 1 MSO_I, 2 MSO_I }, { 1 MSO_I, 4 MSO_I },

    { 0xa MSO_I, 0xc MSO_I }, { 0xe MSO_I, 0xc MSO_I }, { 0xe MSO_I, 0x10 MSO_I },                          // ppp
    { 0xe MSO_I, 0x12 MSO_I }, { 0x14 MSO_I, 0x16 MSO_I }, { 0x18 MSO_I, 0x16 MSO_I },                      // ccp
    { 10800, 0x16 MSO_I },                                                                                  // p
    { 0x1a MSO_I, 0x16 MSO_I }, { 0x1c MSO_I, 0x12 MSO_I }, { 0x1c MSO_I, 0x10 MSO_I },                     // ccp
    { 0x1c MSO_I, 0xc MSO_I }, { 10800, 0xc MSO_I }, { 0x1e MSO_I, 0x20 MSO_I }, { 0x22 MSO_I, 0xc MSO_I }, // pppp
    { 0x24 MSO_I, 0xc MSO_I }, { 0x24 MSO_I, 0x10 MSO_I },                                                  // pp
    { 0x24 MSO_I, 0x26 MSO_I }, { 0x28 MSO_I, 0x2a MSO_I }, { 10800, 0x2a MSO_I },                          // ccp
    { 0x18 MSO_I, 0x2a MSO_I },                                                                             // p
    { 0x2c MSO_I, 0x2a MSO_I }, { 0xa MSO_I, 0x26 MSO_I }, { 0xa MSO_I, 0x10 MSO_I }                        // ccp
};
static const sal_uInt16 mso_sptActionButtonReturnSegm[] =
{
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0002, 0x2001, 0x0001, 0x2001, 0x0006,0x2001, 0x0001, 0x2001, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptActionButtonReturnCalc[] =  // adj value 0 - 5400
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x6000, DFF_Prop_geoLeft, DFF_Prop_adjustValue, 0 },
    { 0x6000, DFF_Prop_geoTop, DFF_Prop_adjustValue, 0 },
    { 0xa000, DFF_Prop_geoRight, 0, DFF_Prop_adjustValue },
    { 0xa000, DFF_Prop_geoBottom, 0, DFF_Prop_adjustValue },
    { 0x8000, 10800, 0, DFF_Prop_adjustValue },
    { 0x2001, 0x0405, 1, 10800 },                           // scaling   6
    { 0x6000, DFF_Prop_geoRight, DFF_Prop_geoLeft, 10800 }, // lr center 7
    { 0x6000, DFF_Prop_geoBottom, DFF_Prop_geoTop, 10800 }, // ul center 8

    { 0x4001, -8050, 0x0406, 1 },   // 9
    { 0x6000, 0x0409, 0x0407, 0 },  // a
    { 0x4001, -3800, 0x0406, 1 },   // b
    { 0x6000, 0x040b, 0x0408, 0 },  // c
    { 0x4001, -4020, 0x0406, 1 },   // d
    { 0x6000, 0x040d, 0x0407, 0 },  // e
    { 0x4001, 2330, 0x0406, 1 },    // f
    { 0x6000, 0x040f, 0x0408, 0 },  // 10
    { 0x4001, 3390, 0x0406, 1 },    // 11
    { 0x6000, 0x0411, 0x0408, 0 },  // 12
    { 0x4001, -3100, 0x0406, 1 },   // 13
    { 0x6000, 0x0413, 0x0407, 0 },  // 14
    { 0x4001, 4230, 0x0406, 1 },    // 15
    { 0x6000, 0x0415, 0x0408, 0 },  // 16
    { 0x4001, -1910, 0x0406, 1 },   // 17
    { 0x6000, 0x0417, 0x0407, 0 },  // 18
    { 0x4001, 1190, 0x0406, 1 },    // 19
    { 0x6000, 0x0419, 0x0407, 0 },  // 1a
    { 0x4001, 2110, 0x0406, 1 },    // 1b
    { 0x6000, 0x041b, 0x0407, 0 },  // 1c
    { 0x4001, 4030, 0x0406, 1 },    // 1d
    { 0x6000, 0x041d, 0x0407, 0 },  // 1e
    { 0x4001, -7830, 0x0406, 1 },   // 1f
    { 0x6000, 0x041f, 0x0408, 0 },  // 20
    { 0x4001, 8250, 0x0406, 1 },    // 21
    { 0x6000, 0x0421, 0x0407, 0 },  // 22
    { 0x4001, 6140, 0x0406, 1 },    // 23
    { 0x6000, 0x0423, 0x0407, 0 },  // 24
    { 0x4001, 5510, 0x0406, 1 },    // 25
    { 0x6000, 0x0425, 0x0408, 0 },  // 26
    { 0x4001, 3180, 0x0406, 1 },    // 27
    { 0x6000, 0x0427, 0x0407, 0 },  // 28
    { 0x4001, 8450, 0x0406, 1 },    // 29
    { 0x6000, 0x0429, 0x0408, 0 },  // 2a
    { 0x4001, -5090, 0x0406, 1 },   // 2b
    { 0x6000, 0x042b, 0x0407, 0 }   // 2c
};
static const mso_AutoShape msoActionButtonReturn =
{
    (SvxMSDffVertPair*)mso_sptActionButtonReturnVert, sizeof( mso_sptActionButtonReturnVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptActionButtonReturnSegm, sizeof( mso_sptActionButtonReturnSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptActionButtonReturnCalc, sizeof( mso_sptActionButtonReturnCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1400,
    (SvxMSDffTextRectangles*)mso_sptActionButtonTextRect, sizeof( mso_sptActionButtonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptActionButtonDocumentVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },
    { 0, 0 }, { 21600, 0 }, { 3 MSO_I, 2 MSO_I }, { 1 MSO_I, 2 MSO_I },
    { 21600, 0 }, { 21600, 21600 }, { 3 MSO_I, 4 MSO_I }, { 3 MSO_I, 2 MSO_I },
    { 21600, 21600 }, { 0, 21600 }, { 1 MSO_I, 4 MSO_I }, { 3 MSO_I, 4 MSO_I },
    { 0, 21600 }, { 0, 0 }, { 1 MSO_I, 2 MSO_I }, { 1 MSO_I, 4 MSO_I },

    { 0xa MSO_I, 0xc MSO_I }, { 0xe MSO_I, 0xc MSO_I }, { 0x10 MSO_I, 0x12 MSO_I }, { 0x10 MSO_I, 0x14 MSO_I },
    { 0xa MSO_I, 0x14 MSO_I }, { 0xe MSO_I, 0xc MSO_I }, { 0x10 MSO_I, 0x12 MSO_I }, { 0xe MSO_I, 0x12 MSO_I }
};
static const sal_uInt16 mso_sptActionButtonDocumentSegm[] =
{
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,

    0x4000, 0x0004, 0x6001, 0x8000,
    0x4000, 0x0002, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptActionButtonDocumentCalc[] =    // adj value 0 - 5400
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x6000, DFF_Prop_geoLeft, DFF_Prop_adjustValue, 0 },
    { 0x6000, DFF_Prop_geoTop, DFF_Prop_adjustValue, 0 },
    { 0xa000, DFF_Prop_geoRight, 0, DFF_Prop_adjustValue },
    { 0xa000, DFF_Prop_geoBottom, 0, DFF_Prop_adjustValue },
    { 0x8000, 10800, 0, DFF_Prop_adjustValue },
    { 0x2001, 0x0405, 1, 10800 },                           // scaling   6
    { 0x6000, DFF_Prop_geoRight, DFF_Prop_geoLeft, 10800 }, // lr center 7
    { 0x6000, DFF_Prop_geoBottom, DFF_Prop_geoTop, 10800 }, // ul center 8

    { 0x4001, -6350, 0x0406, 1 },   // 9
    { 0x6000, 0x0409, 0x0407, 0 },  // a
    { 0x4001, -7830, 0x0406, 1 },   // b
    { 0x6000, 0x040b, 0x0408, 0 },  // c
    { 0x4001, 1690, 0x0406, 1 },    // d
    { 0x6000, 0x040d, 0x0407, 0 },  // e
    { 0x4001, 6350, 0x0406, 1 },    // f
    { 0x6000, 0x040f, 0x0407, 0 },  // 10
    { 0x4001, -3810, 0x0406, 1 },   // 11
    { 0x6000, 0x0411, 0x0408, 0 },  // 12
    { 0x4001, 7830, 0x0406, 1 },    // 13
    { 0x6000, 0x0413, 0x0408, 0 }   // 14
};
static const mso_AutoShape msoActionButtonDocument =
{
    (SvxMSDffVertPair*)mso_sptActionButtonDocumentVert, sizeof( mso_sptActionButtonDocumentVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptActionButtonDocumentSegm, sizeof( mso_sptActionButtonDocumentSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptActionButtonDocumentCalc, sizeof( mso_sptActionButtonDocumentCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1400,
    (SvxMSDffTextRectangles*)mso_sptActionButtonTextRect, sizeof( mso_sptActionButtonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptActionButtonSoundVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },
    { 0, 0 }, { 21600, 0 }, { 3 MSO_I, 2 MSO_I }, { 1 MSO_I, 2 MSO_I },
    { 21600, 0 }, { 21600, 21600 }, { 3 MSO_I, 4 MSO_I }, { 3 MSO_I, 2 MSO_I },
    { 21600, 21600 }, { 0, 21600 }, { 1 MSO_I, 4 MSO_I }, { 3 MSO_I, 4 MSO_I },
    { 0, 21600 }, { 0, 0 }, { 1 MSO_I, 2 MSO_I }, { 1 MSO_I, 4 MSO_I },

    { 0xa MSO_I, 0xc MSO_I }, { 0xe MSO_I, 0xc MSO_I }, { 0x10 MSO_I, 0x12 MSO_I }, { 0x10 MSO_I, 0x14 MSO_I },
    { 0xe MSO_I, 0x16 MSO_I }, { 0xa MSO_I, 0x16 MSO_I }, { 0x18 MSO_I, 10800 }, { 0x1a MSO_I, 10800 },

    { 0x18 MSO_I, 0xc MSO_I }, { 0x1a MSO_I, 0x1c MSO_I },

    { 0x18 MSO_I, 0x16 MSO_I }, { 0x1a MSO_I, 0x1e MSO_I }
};
static const sal_uInt16 mso_sptActionButtonSoundSegm[] =
{
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,

    0x4000, 0x0005, 0x6001, 0x8000,
    0x4000, 0x0001, 0x8000,
    0x4000, 0x0001, 0x8000,
    0x4000, 0x0001, 0x8000
};
static const SvxMSDffCalculationData mso_sptActionButtonSoundCalc[] =   // adj value 0 - 5400
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x6000, DFF_Prop_geoLeft, DFF_Prop_adjustValue, 0 },
    { 0x6000, DFF_Prop_geoTop, DFF_Prop_adjustValue, 0 },
    { 0xa000, DFF_Prop_geoRight, 0, DFF_Prop_adjustValue },
    { 0xa000, DFF_Prop_geoBottom, 0, DFF_Prop_adjustValue },
    { 0x8000, 10800, 0, DFF_Prop_adjustValue },
    { 0x2001, 0x0405, 1, 10800 },                           // scaling   6
    { 0x6000, DFF_Prop_geoRight, DFF_Prop_geoLeft, 10800 }, // lr center 7
    { 0x6000, DFF_Prop_geoBottom, DFF_Prop_geoTop, 10800 }, // ul center 8

    { 0x4001, -8050, 0x0406, 1 },   // 9
    { 0x6000, 0x0409, 0x0407, 0 },  // a
    { 0x4001, -2750, 0x0406, 1 },   // b
    { 0x6000, 0x040b, 0x0408, 0 },  // c
    { 0x4001, -2960, 0x0406, 1 },   // d
    { 0x6000, 0x040d, 0x0407, 0 },  // e
    { 0x4001, 2120, 0x0406, 1 },    // f
    { 0x6000, 0x040f, 0x0407, 0 },  // 10
    { 0x4001, -8050, 0x0406, 1 },   // 11
    { 0x6000, 0x0411, 0x0408, 0 },  // 12
    { 0x4001, 8050, 0x0406, 1 },    // 13
    { 0x6000, 0x0413, 0x0408, 0 },  // 14
    { 0x4001, 2750, 0x0406, 1 },    // 15
    { 0x6000, 0x0415, 0x0408, 0 },  // 16
    { 0x4001, 4020, 0x0406, 1 },    // 17
    { 0x6000, 0x0417, 0x0407, 0 },  // 18
    { 0x4001, 8050, 0x0406, 1 },    // 19
    { 0x6000, 0x0419, 0x0407, 0 },  // 1a
    { 0x4001, -5930, 0x0406, 1 },   // 1b
    { 0x6000, 0x041b, 0x0408, 0 },  // 1c
    { 0x4001, 5930, 0x0406, 1 },    // 1d
    { 0x6000, 0x041d, 0x0408, 0 }   // 1e
};
static const mso_AutoShape msoActionButtonSound =
{
    (SvxMSDffVertPair*)mso_sptActionButtonSoundVert, sizeof( mso_sptActionButtonSoundVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptActionButtonSoundSegm, sizeof( mso_sptActionButtonSoundSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptActionButtonSoundCalc, sizeof( mso_sptActionButtonSoundCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1400,
    (SvxMSDffTextRectangles*)mso_sptActionButtonTextRect, sizeof( mso_sptActionButtonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptActionButtonMovieVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },
    { 0, 0 }, { 21600, 0 }, { 3 MSO_I, 2 MSO_I }, { 1 MSO_I, 2 MSO_I },
    { 21600, 0 }, { 21600, 21600 }, { 3 MSO_I, 4 MSO_I }, { 3 MSO_I, 2 MSO_I },
    { 21600, 21600 }, { 0, 21600 }, { 1 MSO_I, 4 MSO_I }, { 3 MSO_I, 4 MSO_I },
    { 0, 21600 }, { 0, 0 }, { 1 MSO_I, 2 MSO_I }, { 1 MSO_I, 4 MSO_I },

    { 0xa MSO_I, 0xc MSO_I }, { 0xe MSO_I, 0xc MSO_I }, { 0x10 MSO_I, 0x12 MSO_I }, { 0x14 MSO_I, 0x12 MSO_I },
    { 0x16 MSO_I, 0x18 MSO_I }, { 0x16 MSO_I, 0x1a MSO_I }, { 0x1c MSO_I, 0x1a MSO_I }, { 0x1e MSO_I, 0x18 MSO_I },
    { 0x20 MSO_I, 0x18 MSO_I }, { 0x20 MSO_I, 0x22 MSO_I }, { 0x1e MSO_I, 0x22 MSO_I }, { 0x1c MSO_I, 0x24 MSO_I },
    { 0x16 MSO_I, 0x24 MSO_I }, { 0x16 MSO_I, 0x26 MSO_I }, { 0x2a MSO_I, 0x26 MSO_I }, { 0x2a MSO_I, 0x28 MSO_I },
    { 0x10 MSO_I, 0x28 MSO_I }, { 0xe MSO_I, 0x2c MSO_I }, { 0xa MSO_I, 0x2c MSO_I }
};
static const sal_uInt16 mso_sptActionButtonMovieSegm[] =
{
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0003, 0x6001, 0x8000,
    0x4000, 0x0012, 0x6001, 0x8000
};
static const SvxMSDffCalculationData mso_sptActionButtonMovieCalc[] =   // adj value 0 - 5400
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x6000, DFF_Prop_geoLeft, DFF_Prop_adjustValue, 0 },
    { 0x6000, DFF_Prop_geoTop, DFF_Prop_adjustValue, 0 },
    { 0xa000, DFF_Prop_geoRight, 0, DFF_Prop_adjustValue },
    { 0xa000, DFF_Prop_geoBottom, 0, DFF_Prop_adjustValue },
    { 0x8000, 10800, 0, DFF_Prop_adjustValue },
    { 0x2001, 0x0405, 1, 10800 },                           // scaling   6
    { 0x6000, DFF_Prop_geoRight, DFF_Prop_geoLeft, 10800 }, // lr center 7
    { 0x6000, DFF_Prop_geoBottom, DFF_Prop_geoTop, 10800 }, // ul center 8

    { 0x4001, -8050, 0x0406, 1 },   // 9
    { 0x6000, 0x0409, 0x0407, 0 },  // a
    { 0x4001, -4020, 0x0406, 1 },   // b
    { 0x6000, 0x040b, 0x0408, 0 },  // c
    { 0x4001, -7000, 0x0406, 1 },   // d
    { 0x6000, 0x040d, 0x0407, 0 },  // e
    { 0x4001, -6560, 0x0406, 1 },   // f
    { 0x6000, 0x040f, 0x0407, 0 },  // 10
    { 0x4001, -3600, 0x0406, 1 },   // 11
    { 0x6000, 0x0411, 0x0408, 0 },  // 12
    { 0x4001, 4020, 0x0406, 1 },    // 13
    { 0x6000, 0x0413, 0x0407, 0 },  // 14
    { 0x4001, 4660, 0x0406, 1 },    // 15
    { 0x6000, 0x0415, 0x0407, 0 },  // 16
    { 0x4001, -2960, 0x0406, 1 },   // 17
    { 0x6000, 0x0417, 0x0408, 0 },  // 18
    { 0x4001, -2330, 0x0406, 1 },   // 19
    { 0x6000, 0x0419, 0x0408, 0 },  // 1a
    { 0x4001, 6780, 0x0406, 1 },    // 1b
    { 0x6000, 0x041b, 0x0407, 0 },  // 1c
    { 0x4001, 7200, 0x0406, 1 },    // 1d
    { 0x6000, 0x041d, 0x0407, 0 },  // 1e
    { 0x4001, 8050, 0x0406, 1 },    // 1f
    { 0x6000, 0x041f, 0x0407, 0 },  // 20
    { 0x4001, 2960, 0x0406, 1 },    // 21
    { 0x6000, 0x0421, 0x0408, 0 },  // 22
    { 0x4001, 2330, 0x0406, 1 },    // 23
    { 0x6000, 0x0423, 0x0408, 0 },  // 24
    { 0x4001, 3800, 0x0406, 1 },    // 25
    { 0x6000, 0x0425, 0x0408, 0 },  // 26
    { 0x4001, -1060, 0x0406, 1 },   // 27
    { 0x6000, 0x0427, 0x0408, 0 },  // 28
    { 0x4001, -6350, 0x0406, 1 },   // 29
    { 0x6000, 0x0429, 0x0407, 0 },  // 2a
    { 0x4001, -640, 0x0406, 1 },    // 2b
    { 0x6000, 0x042b, 0x0408, 0 }   // 2c
};
static const mso_AutoShape msoActionButtonMovie =
{
    (SvxMSDffVertPair*)mso_sptActionButtonMovieVert, sizeof( mso_sptActionButtonMovieVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptActionButtonMovieSegm, sizeof( mso_sptActionButtonMovieSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptActionButtonMovieCalc, sizeof( mso_sptActionButtonMovieCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1400,
    (SvxMSDffTextRectangles*)mso_sptActionButtonTextRect, sizeof( mso_sptActionButtonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptSmileyFaceVert[] = // adj value 15510 - 17520
{
    { 0, 0 }, { 21600, 21600 }, { 6140, 6350 }, { 8470, 8680 },
    { 13130, 6350 }, { 15460, 8680 }, { 4870, 1 MSO_I }, { 8680, 2 MSO_I },
    { 12920, 2 MSO_I }, { 16730, 1 MSO_I }
};
static const sal_uInt16 mso_sptSmileyFaceSegm[] =
{
    0xa302, 0x6000, 0x8000,
    0xa302, 0x6000, 0x8000,
    0xa302, 0x6000, 0x8000,
    0x4000, 0x2001, 0x8000
};
static const SvxMSDffCalculationData mso_sptSmileyFaceCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 15510 },
    { 0x8000, 17520, 0, 0x400 },
    { 0x4000, 15510, 0x400, 0 }
};

static const SvxMSDffTextRectangles mso_sptSmileyFaceTextRect[] =
{
    { { 0, 1 MSO_I }, { 4 MSO_I, 21600 } }
};
static const sal_Int32 mso_sptSmileyFaceDefault[] =
{
    1, 17520
};
static const mso_AutoShape msoSmileyFace =
{
    (SvxMSDffVertPair*)mso_sptSmileyFaceVert, sizeof( mso_sptSmileyFaceVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptSmileyFaceSegm, sizeof( mso_sptSmileyFaceSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptSmileyFaceCalc, sizeof( mso_sptSmileyFaceCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptSmileyFaceDefault,
    (SvxMSDffTextRectangles*)mso_sptEllipseTextRect, sizeof( mso_sptEllipseTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptEllipseGluePoints, sizeof( mso_sptEllipseGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptDonutVert[] =  // adj value 0 - 10800
{
    { 0, 0 }, { 21600, 21600 }, { 0 MSO_I, 0 MSO_I }, { 1 MSO_I, 1 MSO_I }
};
static const sal_uInt16 mso_sptDonutSegm[] =
{
    0xa302, 0xf8fe, 0xa302, 0x8000
};
static const SvxMSDffCalculationData mso_sptDonutCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x8000, 21600, 0, DFF_Prop_adjustValue }
};
static const mso_AutoShape msoDonut =
{
    (SvxMSDffVertPair*)mso_sptDonutVert, sizeof( mso_sptDonutVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptDonutSegm, sizeof( mso_sptDonutSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptDonutCalc, sizeof( mso_sptDonutCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault5400,
    (SvxMSDffTextRectangles*)mso_sptEllipseTextRect, sizeof( mso_sptEllipseTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptEllipseGluePoints, sizeof( mso_sptEllipseGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptNoSmokingVert[] =  // adj value 0 - 7200
{
    { 0, 0 },  { 21600, 21600 }, { 0 MSO_I, 0 MSO_I }, { 1 MSO_I, 1 MSO_I },
    { 9 MSO_I, 0xa MSO_I }, { 0xb MSO_I, 0xc MSO_I }, { 0 MSO_I, 0 MSO_I }, { 1 MSO_I, 1 MSO_I },
    { 0xd MSO_I, 0xe MSO_I }, { 0xf MSO_I, 0x10 MSO_I }
};
static const sal_uInt16 mso_sptNoSmokingSegm[] =
{
    0xa302, 0xf8fe, 0xa404, 0xf8fe, 0xa404, 0x6000, 0x8000
};
static const SvxMSDffCalculationData mso_sptNoSmokingCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },         // 0
    { 0x8000, 21600, 0, DFF_Prop_adjustValue },     // 1
    { 0x8000, 10800, 0, DFF_Prop_adjustValue },     // 2
    { 0x2001, DFF_Prop_adjustValue, 1, 2 },         // 3
    { 0xa080, 0x403, 0, 0x402 },                    // 4
    { 0x8000, 10800, 0, 0x403 },                    // 5 x1
    { 0x4000, 10800, 0x403, 0 },                    // 6 x2
    { 0x8000, 10800, 0, 0x404 },                    // 7 y1
    { 0x4000, 10800, 0x404, 0 },                    // 8 y2
    { 0x6081, 0x405, 0x407, 450 },                  // 9
    { 0x6082, 0x405, 0x407, 450 },                  // a
    { 0x6081, 0x405, 0x408, 450 },                  // b
    { 0x6082, 0x405, 0x408, 450 },                  // c
    { 0x6081, 0x406, 0x408, 450 },                  // d
    { 0x6082, 0x406, 0x408, 450 },                  // e
    { 0x6081, 0x406, 0x407, 450 },                  // f
    { 0x6082, 0x406, 0x407, 450 }                   // 10
};
static const mso_AutoShape msoNoSmoking =
{
    (SvxMSDffVertPair*)mso_sptNoSmokingVert, sizeof( mso_sptNoSmokingVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptNoSmokingSegm, sizeof( mso_sptNoSmokingSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptNoSmokingCalc, sizeof( mso_sptNoSmokingCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault2700,
    (SvxMSDffTextRectangles*)mso_sptEllipseTextRect, sizeof( mso_sptEllipseTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptEllipseGluePoints, sizeof( mso_sptEllipseGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptBlockArcVert[] =   // adj value 0 (degrees)
{                                                       // adj value 1: 0 -> 10800;
    { 0, 0 }, { 21600, 21600 }, { 4 MSO_I, 3 MSO_I }, { 2 MSO_I, 3 MSO_I },
    { 5 MSO_I, 5 MSO_I }, { 6 MSO_I, 6 MSO_I }, { 2 MSO_I, 3 MSO_I }, { 4 MSO_I, 3 MSO_I },
    { 0, 0 }, { 21600, 21600 }
};
static const sal_uInt16 mso_sptBlockArcSegm[] =
{
    0xA404, 0xa504, 0x6001, 0x8000, 0x0002, 0x8000, 0xf8ff
};
static const sal_Int32 mso_sptBlockArcDefault[] =
{
    2, 180 << 16, 5400
};
static const SvxMSDffCalculationData mso_sptBlockArcCalc[] =
{
    { 0x400a, 10800, DFF_Prop_adjustValue, 0 },
    { 0x4009, 10800, DFF_Prop_adjustValue, 0 },
    { 0x2000, 0x400, 10800, 0 },
    { 0x2000, 0x401, 10800, 0 },
    { 0x8000, 21600, 0, 0x402 },
    { 0x8000, 10800, 0, DFF_Prop_adjust2Value },
    { 0x4000, 10800, DFF_Prop_adjust2Value, 0 },
    { 0x600a, 0x405, DFF_Prop_adjustValue, 0 },
    { 0x6009, 0x405, DFF_Prop_adjustValue, 0 }
};
static const mso_AutoShape msoBlockArc =
{
    (SvxMSDffVertPair*)mso_sptBlockArcVert, sizeof( mso_sptBlockArcVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptBlockArcSegm, sizeof( mso_sptBlockArcSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptBlockArcCalc, sizeof( mso_sptBlockArcCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptBlockArcDefault,
    NULL, 0,
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

// aware : control points are always part of the bounding box
static const SvxMSDffVertPair mso_sptHeartVert[] =
{
    { 10800, 21599 }, { 321, 6886 }, { 70, 6036 },      // ppp
    { -9, 5766 }, { -1, 5474 }, { 2, 5192 },            // ccp
    { 6, 4918 }, { 43, 4641 }, { 101, 4370 },           // ccp
    { 159, 4103 }, { 245, 3837 }, { 353, 3582 },        // ccp
    { 460, 3326 }, { 591, 3077 }, { 741, 2839 },        // ccp
    { 892, 2598 }, { 1066, 2369 }, { 1253, 2155 },      // ccp
    { 1443, 1938 }, { 1651, 1732 }, { 1874, 1543 },     // ccp
    { 2097, 1351 }, { 2337, 1174 }, { 2587, 1014 },     // ccp
    { 2839, 854 }, { 3106, 708 }, { 3380, 584 },        // ccp
    { 3656, 459 }, { 3945, 350 }, { 4237, 264 },        // ccp
    { 4533, 176 }, { 4838, 108 }, { 5144, 66 },         // ccp
    { 5454, 22 }, { 5771, 1 }, { 6086, 3 },             // ccp
    { 6407, 7 }, { 6731, 35 }, { 7048, 89 },            // ccp
    { 7374, 144 }, { 7700, 226 }, { 8015, 335 },        // ccp
    { 8344, 447 }, { 8667, 590 }, { 8972, 756 },        // ccp
    { 9297, 932 }, { 9613, 1135 }, { 9907, 1363 },      // ccp
    { 10224, 1609 }, { 10504, 1900 }, { 10802, 2169 },  // ccp
    { 11697, 1363 },                                    // p
    { 11971, 1116 }, { 12304, 934 }, { 12630, 756 },    // ccp
    { 12935, 590 }, { 13528, 450 }, { 13589, 335 },     // ccp
    { 13901, 226 }, { 14227, 144 }, { 14556, 89 },      // ccp
    { 14872, 35 }, { 15195, 7 }, { 15517, 3 },          // ccp
    { 15830, 0 }, { 16147, 22 }, { 16458, 66 },         // ccp
    { 16764, 109 }, { 17068, 177 }, { 17365, 264 },     // ccp
    { 17658, 349 }, { 17946, 458 }, { 18222, 584 },     // ccp
    { 18496, 708 }, { 18762, 854 }, { 19015, 1014 },    // ccp
    { 19264, 1172 }, { 19504, 1349 }, { 19730, 1543 },  // ccp
    { 19950, 1731 }, { 20158, 1937 }, { 20350, 2155 },  // ccp
    { 20536, 2369 }, { 20710, 2598 }, { 20861, 2839 },  // ccp
    { 21010, 3074 }, { 21143, 3323 }, { 21251, 3582 },  // ccp
    { 21357, 3835 }, { 21443, 4099 }, { 21502, 4370 },  // ccp
    { 21561, 4639 }, { 21595, 4916 }, { 21600, 5192 },  // ccp
    { 21606, 5474 }, { 21584, 5760 }, { 21532, 6036 },  // ccp
    { 21478, 6326 }, { 21366, 6603 }, { 21282, 6887 },  // ccp
    { 10802, 21602 }                                    // p
};
static const sal_uInt16 mso_sptHeartSegm[] =
{
    0x4000, 0x0002, 0x2010, 0x0001, 0x2010, 0x0001, 0x6001, 0x8000
};
static const SvxMSDffTextRectangles mso_sptHeartTextRect[] =
{
    { { 5080, 2540 }, { 16520, 13550 } }
};
static const SvxMSDffVertPair mso_sptHeartGluePoints[] =
{
    { 10800, 2180 }, { 3090, 10800 }, { 10800, 21600 }, { 18490, 10800 }
};
static const sal_Int32 mso_sptHeartBoundRect[] =
{
    -9, 0, 21606, 21602
};
static const mso_AutoShape msoHeart =
{
    (SvxMSDffVertPair*)mso_sptHeartVert, sizeof( mso_sptHeartVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptHeartSegm, sizeof( mso_sptHeartSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptHeartTextRect, sizeof( mso_sptHeartTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21615, 21602,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptHeartGluePoints, sizeof( mso_sptHeartGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptLightningBoldVert[] =
{
    { 8458, 0 }, { 0, 3923 }, { 7564, 8416 }, { 4993, 9720 },
    { 12197, 13904 }, { 9987, 14934 }, { 21600, 21600 }, { 14768, 12911 },
    { 16558, 12016 }, { 11030, 6840 }, { 12831, 6120 }, { 8458, 0 }
};
static const SvxMSDffTextRectangles mso_sptLightningBoldTextRect[] =
{
    { { 8680, 7410 }, { 13970, 14190 } }
};
static const SvxMSDffVertPair mso_sptLightningBoldGluePoints[] =
{
    { 8458, 0 }, { 0, 3923 }, { 4993, 9720 }, { 9987, 14934 }, { 21600, 21600 },
    { 16558, 12016 }, { 12831, 6120 }
};
static const mso_AutoShape msoLightningBold =
{
    (SvxMSDffVertPair*)mso_sptLightningBoldVert, sizeof( mso_sptLightningBoldVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptLightningBoldTextRect, sizeof( mso_sptLightningBoldTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptLightningBoldGluePoints, sizeof( mso_sptLightningBoldGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptSunVert[] =    // adj value 2700 -> 10125
{
    { 0, 10800 },               { 4 MSO_I, 8 MSO_I },       { 4 MSO_I, 9 MSO_I },
    { 0x0a MSO_I, 0x0b MSO_I }, { 0x0c MSO_I, 0x0d MSO_I }, { 0x0e MSO_I, 0x0f MSO_I },
    { 0x10 MSO_I, 0x11 MSO_I }, { 0x12 MSO_I, 0x13 MSO_I }, { 0x14 MSO_I, 0x15 MSO_I },
    { 0x16 MSO_I, 0x17 MSO_I }, { 0x18 MSO_I, 0x19 MSO_I }, { 0x1a MSO_I, 0x1b MSO_I },
    { 0x1c MSO_I, 0x1d MSO_I }, { 0x1e MSO_I, 0x1f MSO_I }, { 0x20 MSO_I, 0x21 MSO_I },
    { 0x22 MSO_I, 0x23 MSO_I }, { 0x24 MSO_I, 0x25 MSO_I }, { 0x26 MSO_I, 0x27 MSO_I },
    { 0x28 MSO_I, 0x29 MSO_I }, { 0x2a MSO_I, 0x2b MSO_I }, { 0x2c MSO_I, 0x2d MSO_I },
    { 0x2e MSO_I, 0x2f MSO_I }, { 0x30 MSO_I, 0x31 MSO_I }, { 0x32 MSO_I, 0x33 MSO_I },
    { 0 MSO_I, 0 MSO_I },       { 1 MSO_I, 1 MSO_I }
};
static const sal_uInt16 mso_sptSunSegm[] =
{
    0x4000, 0x0002, 0x6001, 0x8000,
    0x4000, 0x0002, 0x6001, 0x8000,
    0x4000, 0x0002, 0x6001, 0x8000,
    0x4000, 0x0002, 0x6001, 0x8000,
    0x4000, 0x0002, 0x6001, 0x8000,
    0x4000, 0x0002, 0x6001, 0x8000,
    0x4000, 0x0002, 0x6001, 0x8000,
    0x4000, 0x0002, 0x6001, 0x8000,
    0xa302, 0x6000, 0x8000
};
static const SvxMSDffCalculationData mso_sptSunCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x8000, 21600, 0, DFF_Prop_adjustValue },
    { 0x2000, DFF_Prop_adjustValue, 0, 2700 },
    { 0x2001, 0x402, 5080, 7425 },
    { 0x2000, 0x403, 2540, 0 },
    { 0x8000, 10125, 0, DFF_Prop_adjustValue },
    { 0x2001, 0x405, 2120, 7425 },
    { 0x2000, 0x406, 210, 0 },
    { 0x4000, 10800, 0x407, 0 },    // y1 (0x8)
    { 0x8000, 10800, 0, 0x407 },    // y2 (0x9)
    { 0x0081, 0, 10800, 450 },      // 0xa
    { 0x0082, 0, 10800, 450 },      // 0xb
    { 0x6081, 0x404, 0x408, 450 },  // 0xc
    { 0x6082, 0x404, 0x408, 450 },  // 0xd
    { 0x6081, 0x404, 0x409, 450 },  // 0xe
    { 0x6082, 0x404, 0x409, 450 },  // 0xf
    { 0x0081, 0, 10800, 900 },      // 0x10
    { 0x0082, 0, 10800, 900 },      // 0x11
    { 0x6081, 0x404, 0x408, 900 },  // 0x12
    { 0x6082, 0x404, 0x408, 900 },  // 0x13
    { 0x6081, 0x404, 0x409, 900 },  // 0x14
    { 0x6082, 0x404, 0x409, 900 },  // 0x15
    { 0x0081, 0, 10800, 1350 },     // 0x16
    { 0x0082, 0, 10800, 1350 },     // 0x17
    { 0x6081, 0x404, 0x408, 1350 }, // 0x18
    { 0x6082, 0x404, 0x408, 1350 }, // 0x19
    { 0x6081, 0x404, 0x409, 1350 }, // 0x1a
    { 0x6082, 0x404, 0x409, 1350 }, // 0x1b
    { 0x0081, 0, 10800, 1800 },     // 0x1c
    { 0x0082, 0, 10800, 1800 },     // 0x1d
    { 0x6081, 0x404, 0x408, 1800 }, // 0x1e
    { 0x6082, 0x404, 0x408, 1800 }, // 0x1f
    { 0x6081, 0x404, 0x409, 1800 }, // 0x20
    { 0x6082, 0x404, 0x409, 1800 }, // 0x21
    { 0x0081, 0, 10800, 2250 },     // 0x22
    { 0x0082, 0, 10800, 2250 },     // 0x23
    { 0x6081, 0x404, 0x408, 2250 }, // 0x24
    { 0x6082, 0x404, 0x408, 2250 }, // 0x25
    { 0x6081, 0x404, 0x409, 2250 }, // 0x26
    { 0x6082, 0x404, 0x409, 2250 }, // 0x27
    { 0x0081, 0, 10800, 2700 },     // 0x28
    { 0x0082, 0, 10800, 2700 },     // 0x29
    { 0x6081, 0x404, 0x408, 2700 }, // 0x2a
    { 0x6082, 0x404, 0x408, 2700 }, // 0x2b
    { 0x6081, 0x404, 0x409, 2700 }, // 0x2c
    { 0x6082, 0x404, 0x409, 2700 }, // 0x2d
    { 0x0081, 0, 10800, 3150 },     // 0x2e
    { 0x0082, 0, 10800, 3150 },     // 0x2f
    { 0x6081, 0x404, 0x408, 3150 }, // 0x30
    { 0x6082, 0x404, 0x408, 3150 }, // 0x31
    { 0x6081, 0x404, 0x409, 3150 }, // 0x32
    { 0x6082, 0x404, 0x409, 3150 }, // 0x33
    { 0x2081, DFF_Prop_adjustValue, 10800, 450 },   // 0x34 ( textbox )
    { 0x2081, DFF_Prop_adjustValue, 10800, 2250 }   // 0x35

};
static const SvxMSDffTextRectangles mso_sptSunTextRect[] =
{
    { { 0x34 MSO_I, 0x34 MSO_I }, { 0x35 MSO_I, 0x35 MSO_I } }
};
static const mso_AutoShape msoSun =
{
    (SvxMSDffVertPair*)mso_sptSunVert, sizeof( mso_sptSunVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptSunSegm, sizeof( mso_sptSunSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptSunCalc, sizeof( mso_sptSunCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault5400,
    (SvxMSDffTextRectangles*)mso_sptSunTextRect, sizeof( mso_sptSunTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptMoonVert[] =   // adj value 0 -> 18900
{
    { 21600, 0 },
    { 3 MSO_I, 4 MSO_I },   { 0 MSO_I, 5080 },      { 0 MSO_I, 10800 }, // ccp
    { 0 MSO_I, 16520 },     { 3 MSO_I, 5 MSO_I },   { 21600, 21600 },   // ccp
    { 9740, 21600 },        { 0, 16730 },           { 0, 10800 },       // ccp
    { 0, 4870 },            { 9740, 0 },            { 21600, 0  }       // ccp
};
static const sal_uInt16 mso_sptMoonSegm[] =
{
    0x4000, 0x2004, 0x6000, 0x8000
};
static const SvxMSDffCalculationData mso_sptMoonCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x8000, 21600, 0, DFF_Prop_adjustValue },
    { 0x2001, 0x401, 1, 2 },
    { 0x6000, 0x402, DFF_Prop_adjustValue, 0 },
    { 0x2001, DFF_Prop_adjustValue, 1794, 10000 },
    { 0x8000, 21600, 0, 0x0404 },
    { 0x2001, DFF_Prop_adjustValue, 4000, 18900 },
    { 0x8081, 0, 10800, 0x406 },
    { 0x8082, 0, 10800, 0x406 },
    { 0x6000, 0x407, 0x407, 0 },
    { 0x8000, 21600, 0, 0x408 }
};
static const SvxMSDffTextRectangles mso_sptMoonTextRect[] =
{
    { { 9 MSO_I, 8 MSO_I }, { 0 MSO_I, 0xa MSO_I } }
};
static const SvxMSDffVertPair mso_sptMoonGluePoints[] =
{
    { 21600, 0 }, { 0, 10800 }, { 21600, 21600 }, { 0 MSO_I, 10800 }
};
static const mso_AutoShape msoMoon =
{
    (SvxMSDffVertPair*)mso_sptMoonVert, sizeof( mso_sptMoonVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptMoonSegm, sizeof( mso_sptMoonSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptMoonCalc, sizeof( mso_sptMoonCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault10800,
    (SvxMSDffTextRectangles*)mso_sptMoonTextRect, sizeof( mso_sptMoonTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptMoonGluePoints, sizeof( mso_sptMoonGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptBracketPairVert[] =    // adj value 0 -> 10800
{
    { 0 MSO_I, 0 },     { 0, 1 MSO_I },     // left top alignment
    { 0, 2 MSO_I },     { 0 MSO_I, 21600 }, // left  bottom "
    { 3 MSO_I, 21600 }, { 21600, 2 MSO_I }, // right bottom "
    { 21600, 1 MSO_I }, { 3 MSO_I, 0 }      // right top    "
};
static const sal_uInt16 mso_sptBracketPairSegm[] =
{
    0x4000, 0xa701, 0x0001, 0xa801, 0x8000,
    0x4000, 0xa701, 0x0001, 0xa801, 0x8000
};
static const SvxMSDffCalculationData mso_sptBracketPairCalc[] =
{
    { 0x6000, DFF_Prop_geoLeft, DFF_Prop_adjustValue, 0 },
    { 0x6000, DFF_Prop_geoTop, DFF_Prop_adjustValue, 0 },
    { 0xa000, DFF_Prop_geoBottom, 0, DFF_Prop_adjustValue },
    { 0xa000, DFF_Prop_geoRight, 0, DFF_Prop_adjustValue },
    { 0x2082, DFF_Prop_adjustValue, 0, 450 },
    { 0x2000, 0x404, 0, 10800 },
    { 0x8000, 0, 0, DFF_Prop_adjustValue },
    { 0xa000, 0x406, 0, 0x405 },
    { 0xa000, DFF_Prop_geoLeft, 0, 0x407 },
    { 0xa000, DFF_Prop_geoTop, 0, 0x407 },
    { 0x6000, DFF_Prop_geoRight, 0x407, 0 },
    { 0x6000, DFF_Prop_geoBottom, 0x407, 0 },
    { 0xa000, DFF_Prop_geoLeft, 0, 0x405 },
    { 0xa000, DFF_Prop_geoTop, 0, 0x405 },
    { 0x6000, DFF_Prop_geoRight, 0x405, 0 },
    { 0x6000, DFF_Prop_geoBottom, 0x405, 0 }
};
static const SvxMSDffTextRectangles mso_sptBracketPairTextRect[] =
{
    { { 8 MSO_I, 9 MSO_I }, { 0xa MSO_I, 0xb MSO_I } }
};
static const mso_AutoShape msoBracketPair =
{
    (SvxMSDffVertPair*)mso_sptBracketPairVert, sizeof( mso_sptBracketPairVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptBracketPairSegm, sizeof( mso_sptBracketPairSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptBracketPairCalc, sizeof( mso_sptBracketPairCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault3700,
    (SvxMSDffTextRectangles*)mso_sptBracketPairTextRect, sizeof( mso_sptBracketPairTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const sal_uInt16 mso_sptPlaqueSegm[] =
{
    0x4000, 0xa801, 0x0001, 0xa701, 0x0001, 0xa801, 0x0001, 0xa701, 0x6000, 0x8000
};
static const SvxMSDffTextRectangles mso_sptPlaqueTextRect[] =
{
    { { 0xc MSO_I, 0xd MSO_I }, { 0xe MSO_I, 0xf MSO_I } }
};
static const mso_AutoShape msoPlaque =
{
    (SvxMSDffVertPair*)mso_sptBracketPairVert, sizeof( mso_sptBracketPairVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptPlaqueSegm, sizeof( mso_sptPlaqueSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptBracketPairCalc, sizeof( mso_sptBracketPairCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault3600,
    (SvxMSDffTextRectangles*)mso_sptPlaqueTextRect, sizeof( mso_sptPlaqueTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptBracePairVert[] =  // adj value 0 -> 5400
{
    { 4 MSO_I, 0 }, { 0 MSO_I, 1 MSO_I }, { 0 MSO_I, 6 MSO_I }, { 0 ,10800 },           // left bracket
    { 0 MSO_I, 7 MSO_I }, { 0 MSO_I, 2 MSO_I }, { 4 MSO_I, 21600 },
    { 8 MSO_I, 21600 }, { 3 MSO_I, 2 MSO_I }, { 3 MSO_I, 7 MSO_I }, { 21600, 10800 },   // right bracket
    { 3 MSO_I, 6 MSO_I }, { 3 MSO_I, 1 MSO_I }, { 8 MSO_I, 0 }
};
static const sal_uInt16 mso_sptBracePairSegm[] =
{
    0x4000, 0xa701, 0x0001, 0xa801, 0xa701, 0x0001, 0xa801, 0x8000,
    0x4000, 0xa701, 0x0001, 0xa801, 0xa701, 0x0001, 0xa801, 0x8000
};
static const SvxMSDffCalculationData mso_sptBracePairCalc[] =
{
    { 0x6000, DFF_Prop_geoLeft, DFF_Prop_adjustValue, 0 },
    { 0x6000, DFF_Prop_geoTop, DFF_Prop_adjustValue, 0 },
    { 0xa000, DFF_Prop_geoBottom, 0, DFF_Prop_adjustValue },
    { 0xa000, DFF_Prop_geoRight, 0, DFF_Prop_adjustValue },
    { 0x2001, 0x400, 2, 1 },                                    //  4
    { 0x2001, DFF_Prop_adjustValue, 2, 1 },                     //  5
    { 0x8000, 10800, 0, DFF_Prop_adjustValue },                 //  6
    { 0x8000, 21600, 0, 0x406 },                                //  7
    { 0xa000, DFF_Prop_geoRight, 0, 0x405 },                    //  8
    { 0x2001, DFF_Prop_adjustValue, 1, 3 },                     //  9
    { 0x6000, 0x409, DFF_Prop_adjustValue, 0 },                 // xa
    { 0x6000, DFF_Prop_geoLeft, 0x40a, 0 },                     // xb
    { 0x6000, DFF_Prop_geoTop, 0x409, 0 },                      // xc
    { 0xa000, DFF_Prop_geoRight, 0, 0x40a },                    // xd
    { 0xa000, DFF_Prop_geoBottom, 0, 0x409 }                    // xe

};
static const SvxMSDffTextRectangles mso_sptBracePairTextRect[] =
{
    { { 0xb MSO_I, 0xc MSO_I }, { 0xd MSO_I, 0xe MSO_I } }
};
static const mso_AutoShape msoBracePair =
{
    (SvxMSDffVertPair*)mso_sptBracePairVert, sizeof( mso_sptBracePairVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptBracePairSegm, sizeof( mso_sptBracePairSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptBracePairCalc, sizeof( mso_sptBracePairCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1800,
    (SvxMSDffTextRectangles*)mso_sptBracePairTextRect, sizeof( mso_sptBracePairTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffCalculationData mso_sptBracketCalc[] =
{
    { 0x2001, DFF_Prop_adjustValue, 1, 2 },
    { 0x6000, DFF_Prop_geoTop, DFF_Prop_adjustValue, 0 },
    { 0xa000, DFF_Prop_geoBottom, 0, DFF_Prop_adjustValue },
    { 0x6000, DFF_Prop_geoTop, 0x400, 0 },
    { 0xa000, DFF_Prop_geoBottom, 0, 0x400 }
};
static const sal_uInt16 mso_sptBracketSegm[] =
{
    0x4000, 0x2001, 0x0001, 0x2001, 0x8000
};
static const SvxMSDffVertPair mso_sptLeftBracketVert[] =    // adj value 0 -> 10800
{
    { 21600, 0 }, { 10800,  0 }, { 0, 3 MSO_I }, { 0, 1 MSO_I },
    { 0, 2 MSO_I }, { 0, 4 MSO_I }, { 10800, 21600 }, { 21600, 21600 }
};
static const SvxMSDffTextRectangles mso_sptLeftBracketTextRect[] =
{
    { { 6350, 3 MSO_I }, { 21600, 4 MSO_I } }
};
static const SvxMSDffVertPair mso_sptLeftBracketGluePoints[] =
{
    { 21600, 0 }, { 0, 10800 }, { 21600, 21600 }
};
static const mso_AutoShape msoLeftBracket =
{
    (SvxMSDffVertPair*)mso_sptLeftBracketVert, sizeof( mso_sptLeftBracketVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptBracketSegm, sizeof( mso_sptBracketSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptBracketCalc, sizeof( mso_sptBracketCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1800,
    (SvxMSDffTextRectangles*)mso_sptLeftBracketTextRect, sizeof( mso_sptLeftBracketTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptLeftBracketGluePoints, sizeof( mso_sptLeftBracketGluePoints ) / sizeof( SvxMSDffVertPair )
};
static const SvxMSDffVertPair mso_sptRightBracketVert[] =   // adj value 0 -> 10800
{
    { 0, 0 }, { 10800, 0 }, { 21600, 3 MSO_I }, { 21600, 1 MSO_I },
    { 21600, 2 MSO_I }, { 21600, 4 MSO_I }, { 10800, 21600 }, { 0, 21600 }
};
static const SvxMSDffTextRectangles mso_sptRightBracketTextRect[] =
{
    { { 0, 3 MSO_I }, { 15150, 4 MSO_I } }
};
static const SvxMSDffVertPair mso_sptRightBracketGluePoints[] =
{
    { 0, 0 }, { 0, 21600 }, { 21600, 10800 }
};
static const mso_AutoShape msoRightBracket =
{
    (SvxMSDffVertPair*)mso_sptRightBracketVert, sizeof( mso_sptRightBracketVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptBracketSegm, sizeof( mso_sptBracketSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptBracketCalc, sizeof( mso_sptBracketCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault1800,
    (SvxMSDffTextRectangles*)mso_sptRightBracketTextRect, sizeof( mso_sptRightBracketTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptRightBracketGluePoints, sizeof( mso_sptRightBracketGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffCalculationData mso_sptBraceCalc[] =
{
    { 0x2001, DFF_Prop_adjustValue, 1, 2 },
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0xa000, 0x404, 0, DFF_Prop_adjustValue },
    { 0xa000, 0x404, 0, 0x400 },
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
    { 0x6000, 0x404, 0x400, 0 },
    { 0x6000, 0x404, DFF_Prop_adjustValue, 0 },
    { 0x8000, 21600, 0, DFF_Prop_adjustValue },
    { 0x8000, 21600, 0, 0x400 },
    { 0x2001, DFF_Prop_adjustValue, 10000, 31953 },
    { 0x8000, 21600, 0, 0x409 }
};
static const sal_uInt16 mso_sptBraceSegm[] =
{
    0x4000, 0x2001, 0x0001, 0x2002, 0x0001, 0x2001, 0x8000
};
static const sal_Int32 mso_sptBraceDefault[] =
{
    2, 1800, 10800
};
static const SvxMSDffVertPair mso_sptLeftBraceVert[] =
{
    { 21600, 0 },                                               // p
    { 16200, 0 }, { 10800, 0 MSO_I }, { 10800, 1 MSO_I },       // ccp
    { 10800, 2 MSO_I },                                         // p
    { 10800, 3 MSO_I }, { 5400, 4 MSO_I }, { 0, 4 MSO_I },      // ccp
    { 5400, 4 MSO_I },  { 10800, 5 MSO_I }, { 10800, 6 MSO_I }, // ccp
    { 10800, 7 MSO_I },                                         // p
    { 10800, 8 MSO_I }, { 16200, 21600 }, { 21600, 21600 }      // ccp
};
static const SvxMSDffTextRectangles mso_sptLeftBraceTextRect[] =
{
    { { 13800, 9 MSO_I }, { 21600, 10 MSO_I } }
};
static const mso_AutoShape msoLeftBrace =       // adj value0 0 -> 5400
{                                               // adj value1 0 -> 21600
    (SvxMSDffVertPair*)mso_sptLeftBraceVert, sizeof( mso_sptLeftBraceVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptBraceSegm, sizeof( mso_sptBraceSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptBraceCalc, sizeof( mso_sptBraceCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptBraceDefault,
    (SvxMSDffTextRectangles*)mso_sptLeftBraceTextRect, sizeof( mso_sptLeftBraceTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptLeftBracketGluePoints, sizeof( mso_sptLeftBracketGluePoints ) / sizeof( SvxMSDffVertPair )
};
static const SvxMSDffVertPair mso_sptRightBraceVert[] =
{
    { 0, 0 },                                                   // p
    { 5400, 0 }, { 10800, 0 MSO_I }, { 10800, 1 MSO_I },        // ccp
    { 10800, 2 MSO_I },                                         // p
    { 10800, 3 MSO_I }, { 16200, 4 MSO_I }, { 21600, 4 MSO_I }, // ccp
    { 16200, 4 MSO_I }, { 10800, 5 MSO_I }, { 10800, 6 MSO_I }, // ccp
    { 10800, 7 MSO_I },                                         // p
    { 10800, 8 MSO_I }, { 5400, 21600 }, { 0, 21600 }           // ccp
};
static const SvxMSDffTextRectangles mso_sptRightBraceTextRect[] =
{
    { { 0, 9 MSO_I }, { 7800, 10 MSO_I } }
};
static const mso_AutoShape msoRightBrace =      // adj value0 0 -> 5400
{                                               // adj value1 0 -> 21600
    (SvxMSDffVertPair*)mso_sptRightBraceVert, sizeof( mso_sptRightBraceVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptBraceSegm, sizeof( mso_sptBraceSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptBraceCalc, sizeof( mso_sptBraceCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptBraceDefault,
    (SvxMSDffTextRectangles*)mso_sptRightBraceTextRect, sizeof( mso_sptRightBraceTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptRightBracketGluePoints, sizeof( mso_sptRightBracketGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptIrregularSeal1Vert[] =
{
    { 10901, 5905 }, { 8458, 2399 }, { 7417, 6425 }, { 476, 2399 },
    { 4732, 7722 }, { 106, 8718 }, { 3828, 11880 }, { 243, 14689 },
    { 5772, 14041 }, { 4868, 17719 }, { 7819, 15730 }, { 8590, 21600 },
    { 10637, 15038 }, { 13349, 19840 }, { 14125, 14561 }, { 18248, 18195 },
    { 16938, 13044 }, { 21600, 13393 }, { 17710, 10579 }, { 21198, 8242 },
    { 16806, 7417 }, { 18482, 4560 }, { 14257, 5429 }, { 14623, 106 }, { 10901, 5905 }
};
static const SvxMSDffTextRectangles mso_sptIrregularSeal1TextRect[] =
{
    { { 4680, 6570 }, { 16140, 13280 } }
};
static const SvxMSDffVertPair mso_sptIrregularSeal1GluePoints[] =
{
    { 14623, 106 }, { 106, 8718 }, { 8590, 21600 }, { 21600, 13393 }
};
static const mso_AutoShape msoIrregularSeal1 =
{
    (SvxMSDffVertPair*)mso_sptIrregularSeal1Vert, sizeof( mso_sptIrregularSeal1Vert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptIrregularSeal1TextRect, sizeof( mso_sptIrregularSeal1TextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptIrregularSeal1GluePoints, sizeof( mso_sptIrregularSeal1GluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptIrregularSeal2Vert[] =
{
    { 11464, 4340 }, { 9722, 1887 }, { 8548, 6383 }, { 4503, 3626 },
    { 5373, 7816 }, { 1174, 8270 }, { 3934, 11592 }, { 0, 12875 },
    { 3329, 15372 }, { 1283, 17824 }, { 4804, 18239 }, { 4918, 21600 },
    { 7525, 18125 }, { 8698, 19712 }, { 9871, 17371 }, { 11614, 18844 },
    { 12178, 15937 }, { 14943, 17371 }, { 14640, 14348 }, { 18878, 15632 },
    { 16382, 12311 }, { 18270, 11292 }, { 16986, 9404 }, { 21600, 6646 },
    { 16382, 6533 }, { 18005, 3172 }, { 14524, 5778 }, { 14789, 0 },
    { 11464, 4340 }
};
static const SvxMSDffTextRectangles mso_sptIrregularSeal2TextRect[] =
{
    { { 5400, 6570 }, { 14160, 15290 } }
};
static const SvxMSDffVertPair mso_sptIrregularSeal2GluePoints[] =
{
    { 9722, 1887 }, { 0, 12875 }, { 11614, 18844 }, { 21600, 6646 }
};
static const mso_AutoShape msoIrregularSeal2 =
{
    (SvxMSDffVertPair*)mso_sptIrregularSeal2Vert, sizeof( mso_sptIrregularSeal2Vert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptIrregularSeal2TextRect, sizeof( mso_sptIrregularSeal2TextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptIrregularSeal2GluePoints, sizeof( mso_sptIrregularSeal2GluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptSeal4Vert[] =      // adjustment1 : 0 - 10800
{
    { 0, 10800 }, { 4 MSO_I, 4 MSO_I }, { 10800, 0 }, { 3 MSO_I, 4 MSO_I },
    { 21600, 10800 }, { 3 MSO_I, 3 MSO_I }, { 10800, 21600 }, { 4 MSO_I, 3 MSO_I },
    { 0, 10800 }
};
static const SvxMSDffCalculationData mso_sptSeal4Calc[] =
{
    { 0x0000, 7600, 0, 0 },
    { 0x6001, 0x400, DFF_Prop_adjustValue, 10800 },
    { 0xa000, 0x400, 0, 0x401 },
    { 0x4000, 10800, 0x402, 0 },
    { 0x8000, 10800, 0, 0x402 }
};
static const SvxMSDffTextRectangles mso_sptSeal4TextRect[] =
{
    { { 4 MSO_I, 4 MSO_I }, { 3 MSO_I, 3 MSO_I } }
};
static const mso_AutoShape msoSeal4 =
{
    (SvxMSDffVertPair*)mso_sptSeal4Vert, sizeof( mso_sptSeal4Vert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    (SvxMSDffCalculationData*)mso_sptSeal4Calc, sizeof( mso_sptSeal4Calc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault8100,
    (SvxMSDffTextRectangles*)mso_sptSeal4TextRect, sizeof( mso_sptSeal4TextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptStarVert[] =
{
    { 10797, 0 }, { 8278, 8256 }, { 0, 8256 }, { 6722, 13405 },
    { 4198, 21600 }, { 10797, 16580 }, { 17401, 21600 }, { 14878, 13405 },
    { 21600, 8256 }, { 13321, 8256 }, { 10797, 0 }
};
static const SvxMSDffTextRectangles mso_sptStarTextRect[] =
{
    { { 6722, 8256 }, { 14878, 15460 } }
};
static const mso_AutoShape msoStar =
{
    (SvxMSDffVertPair*)mso_sptStarVert, sizeof( mso_sptStarVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptStarTextRect, sizeof( mso_sptStarTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffCalculationData mso_sptSeal24Calc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },     // 0x00
    { 0x2081, 0x400, 10800, 3150 },             // 0x01 ( textframe )
    { 0x2082, 0x400, 10800, 3150 },             // 0x02
    { 0x2081, 0x400, 10800, 1350 },             // 0x03
    { 0x2082, 0x400, 10800, 1350 },             // 0x04
    { 0x0081, 0,     10800, 0 },
    { 0x0082, 0,     10800, 0 },
    { 0x2081, 0x400, 10800, 75 },
    { 0x2082, 0x400, 10800, 75 },
    { 0x0081, 0,     10800, 150 },
    { 0x0082, 0,     10800, 150 },
    { 0x2081, 0x400, 10800, 225 },
    { 0x2082, 0x400, 10800, 225 },
    { 0x0081, 0,     10800, 300 },
    { 0x0082, 0,     10800, 300 },
    { 0x2081, 0x400, 10800, 375 },
    { 0x2082, 0x400, 10800, 375 },
    { 0x0081, 0,     10800, 450 },
    { 0x0082, 0,     10800, 450 },
    { 0x2081, 0x400, 10800, 525 },
    { 0x2082, 0x400, 10800, 525 },
    { 0x0081, 0,     10800, 600 },
    { 0x0082, 0,     10800, 600 },
    { 0x2081, 0x400, 10800, 675 },
    { 0x2082, 0x400, 10800, 675 },
    { 0x0081, 0,     10800, 750 },
    { 0x0082, 0,     10800, 750 },
    { 0x2081, 0x400, 10800, 825 },
    { 0x2082, 0x400, 10800, 825 },
    { 0x0081, 0,     10800, 900 },
    { 0x0082, 0,     10800, 900 },
    { 0x2081, 0x400, 10800, 975 },
    { 0x2082, 0x400, 10800, 975 },
    { 0x0081, 0,     10800, 1050 },
    { 0x0082, 0,     10800, 1050 },
    { 0x2081, 0x400, 10800, 1125 },
    { 0x2082, 0x400, 10800, 1125 },
    { 0x0081, 0,     10800, 1200 },
    { 0x0082, 0,     10800, 1200 },
    { 0x2081, 0x400, 10800, 1275 },
    { 0x2082, 0x400, 10800, 1275 },
    { 0x0081, 0,     10800, 1350 },
    { 0x0082, 0,     10800, 1350 },
    { 0x2081, 0x400, 10800, 1425 },
    { 0x2082, 0x400, 10800, 1425 },
    { 0x0081, 0,     10800, 1500 },
    { 0x0082, 0,     10800, 1500 },
    { 0x2081, 0x400, 10800, 1575 },
    { 0x2082, 0x400, 10800, 1575 },
    { 0x0081, 0,     10800, 1650 },
    { 0x0082, 0,     10800, 1650 },
    { 0x2081, 0x400, 10800, 1725 },
    { 0x2082, 0x400, 10800, 1725 },
    { 0x0081, 0,     10800, 1800 },
    { 0x0082, 0,     10800, 1800 },
    { 0x2081, 0x400, 10800, 1875 },
    { 0x2082, 0x400, 10800, 1875 },
    { 0x0081, 0,     10800, 1950 },
    { 0x0082, 0,     10800, 1950 },
    { 0x2081, 0x400, 10800, 2025 },
    { 0x2082, 0x400, 10800, 2025 },
    { 0x0081, 0,     10800, 2100 },
    { 0x0082, 0,     10800, 2100 },
    { 0x2081, 0x400, 10800, 2175 },
    { 0x2082, 0x400, 10800, 2175 },
    { 0x0081, 0,     10800, 2250 },
    { 0x0082, 0,     10800, 2250 },
    { 0x2081, 0x400, 10800, 2325 },
    { 0x2082, 0x400, 10800, 2325 },
    { 0x0081, 0,     10800, 2400 },
    { 0x0082, 0,     10800, 2400 },
    { 0x2081, 0x400, 10800, 2475 },
    { 0x2082, 0x400, 10800, 2475 },
    { 0x0081, 0,     10800, 2550 },
    { 0x0082, 0,     10800, 2550 },
    { 0x2081, 0x400, 10800, 2625 },
    { 0x2082, 0x400, 10800, 2625 },
    { 0x0081, 0,     10800, 2700 },
    { 0x0082, 0,     10800, 2700 },
    { 0x2081, 0x400, 10800, 2775 },
    { 0x2082, 0x400, 10800, 2775 },
    { 0x0081, 0,     10800, 2850 },
    { 0x0082, 0,     10800, 2850 },
    { 0x2081, 0x400, 10800, 2925 },
    { 0x2082, 0x400, 10800, 2925 },
    { 0x0081, 0,     10800, 3000 },
    { 0x0082, 0,     10800, 3000 },
    { 0x2081, 0x400, 10800, 3075 },
    { 0x2082, 0x400, 10800, 3075 },
    { 0x0081, 0,     10800, 3150 },
    { 0x0082, 0,     10800, 3150 },
    { 0x2081, 0x400, 10800, 3225 },
    { 0x2082, 0x400, 10800, 3225 },
    { 0x0081, 0,     10800, 3300 },
    { 0x0082, 0,     10800, 3300 },
    { 0x2081, 0x400, 10800, 3375 },
    { 0x2082, 0x400, 10800, 3375 },
    { 0x0081, 0,     10800, 3450 },
    { 0x0082, 0,     10800, 3450 },
    { 0x2081, 0x400, 10800, 3525 },
    { 0x2082, 0x400, 10800, 3525 }
};
static const SvxMSDffVertPair mso_sptSeal8Vert[] =  // adj value 0 -> 10800
{
    { 5 MSO_I, 6 MSO_I }, { 11 MSO_I, 12 MSO_I }, { 17 MSO_I, 18 MSO_I }, { 23 MSO_I, 24 MSO_I },
    { 29 MSO_I, 30 MSO_I }, { 35 MSO_I, 36 MSO_I }, { 41 MSO_I, 42 MSO_I }, { 47 MSO_I, 48 MSO_I },
    { 53 MSO_I, 54 MSO_I }, { 59 MSO_I, 60 MSO_I }, { 65 MSO_I, 66 MSO_I }, { 71 MSO_I, 72 MSO_I },
    { 77 MSO_I, 78 MSO_I }, { 83 MSO_I, 84 MSO_I }, { 89 MSO_I, 90 MSO_I }, { 95 MSO_I, 96 MSO_I },
    { 5 MSO_I, 6 MSO_I }
};
static const SvxMSDffTextRectangles mso_sptSealTextRect[] =
{
    { { 1 MSO_I, 2 MSO_I }, { 3 MSO_I, 4 MSO_I } }
};
static const mso_AutoShape msoSeal8 =
{
    (SvxMSDffVertPair*)mso_sptSeal8Vert, sizeof( mso_sptSeal8Vert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    (SvxMSDffCalculationData*)mso_sptSeal24Calc, sizeof( mso_sptSeal24Calc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault2500,
    (SvxMSDffTextRectangles*)mso_sptSealTextRect, sizeof( mso_sptSealTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};
static const SvxMSDffVertPair mso_sptSeal16Vert[] = // adj value 0 -> 10800
{
    { 0x05 MSO_I, 0x06 MSO_I }, { 0x07 MSO_I, 0x08 MSO_I }, { 0x09 MSO_I, 0x0a MSO_I }, { 0x0b MSO_I, 0x0c MSO_I },
    { 0x0d MSO_I, 0x0e MSO_I }, { 0x0f MSO_I, 0x10 MSO_I }, { 0x11 MSO_I, 0x12 MSO_I }, { 0x13 MSO_I, 0x14 MSO_I },
    { 0x15 MSO_I, 0x16 MSO_I }, { 0x17 MSO_I, 0x18 MSO_I }, { 0x19 MSO_I, 0x1a MSO_I }, { 0x1b MSO_I, 0x1c MSO_I },
    { 0x1d MSO_I, 0x1e MSO_I }, { 0x1f MSO_I, 0x20 MSO_I }, { 0x21 MSO_I, 0x22 MSO_I }, { 0x23 MSO_I, 0x24 MSO_I },
    { 0x25 MSO_I, 0x26 MSO_I }, { 0x27 MSO_I, 0x28 MSO_I }, { 0x29 MSO_I, 0x2a MSO_I }, { 0x2b MSO_I, 0x2c MSO_I },
    { 0x2d MSO_I, 0x2e MSO_I }, { 0x2f MSO_I, 0x30 MSO_I }, { 0x31 MSO_I, 0x32 MSO_I }, { 0x33 MSO_I, 0x34 MSO_I },
    { 0x35 MSO_I, 0x36 MSO_I }, { 0x37 MSO_I, 0x38 MSO_I }, { 0x39 MSO_I, 0x3a MSO_I }, { 0x3b MSO_I, 0x3c MSO_I },
    { 0x3d MSO_I, 0x3e MSO_I }, { 0x3f MSO_I, 0x40 MSO_I }, { 0x41 MSO_I, 0x42 MSO_I }, { 0x43 MSO_I, 0x44 MSO_I },
    { 0x05 MSO_I, 0x06 MSO_I }
};
static const SvxMSDffCalculationData mso_sptSeal16Calc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },     // 0x00
    { 0x2081, 0x400, 10800, 3150 },             // 0x01 ( textframe )
    { 0x2082, 0x400, 10800, 3150 },             // 0x02
    { 0x2081, 0x400, 10800, 1350 },             // 0x03
    { 0x2082, 0x400, 10800, 1350 },             // 0x04
    { 0x0081, 0,     10800, 0 },
    { 0x0082, 0,     10800, 0 },
    { 0x2081, 0x400, 10800, 113 },
    { 0x2082, 0x400, 10800, 113 },
    { 0x0081, 0,     10800, 225 },
    { 0x0082, 0,     10800, 225 },
    { 0x2081, 0x400, 10800, 338 },
    { 0x2082, 0x400, 10800, 338 },
    { 0x0081, 0,     10800, 450 },
    { 0x0082, 0,     10800, 450 },
    { 0x2081, 0x400, 10800, 563 },
    { 0x2082, 0x400, 10800, 563 },
    { 0x0081, 0,     10800, 675 },
    { 0x0082, 0,     10800, 675 },
    { 0x2081, 0x400, 10800, 788 },
    { 0x2082, 0x400, 10800, 788 },
    { 0x0081, 0,     10800, 900 },
    { 0x0082, 0,     10800, 900 },
    { 0x2081, 0x400, 10800, 1013 },
    { 0x2082, 0x400, 10800, 1013 },
    { 0x0081, 0,     10800, 1125 },
    { 0x0082, 0,     10800, 1125 },
    { 0x2081, 0x400, 10800, 1238 },
    { 0x2082, 0x400, 10800, 1238 },
    { 0x0081, 0,     10800, 1350 },
    { 0x0082, 0,     10800, 1350 },
    { 0x2081, 0x400, 10800, 1463 },
    { 0x2082, 0x400, 10800, 1463 },
    { 0x0081, 0,     10800, 1575 },
    { 0x0082, 0,     10800, 1575 },
    { 0x2081, 0x400, 10800, 1688 },
    { 0x2082, 0x400, 10800, 1688 },
    { 0x0081, 0,     10800, 1800 },
    { 0x0082, 0,     10800, 1800 },
    { 0x2081, 0x400, 10800, 1913 },
    { 0x2082, 0x400, 10800, 1913 },
    { 0x0081, 0,     10800, 2025 },
    { 0x0082, 0,     10800, 2025 },
    { 0x2081, 0x400, 10800, 2138 },
    { 0x2082, 0x400, 10800, 2138 },
    { 0x0081, 0,     10800, 2250 },
    { 0x0082, 0,     10800, 2250 },
    { 0x2081, 0x400, 10800, 2363 },
    { 0x2082, 0x400, 10800, 2363 },
    { 0x0081, 0,     10800, 2475 },
    { 0x0082, 0,     10800, 2475 },
    { 0x2081, 0x400, 10800, 2588 },
    { 0x2082, 0x400, 10800, 2588 },
    { 0x0081, 0,     10800, 2700 },
    { 0x0082, 0,     10800, 2700 },
    { 0x2081, 0x400, 10800, 2813 },
    { 0x2082, 0x400, 10800, 2813 },
    { 0x0081, 0,     10800, 2925 },
    { 0x0082, 0,     10800, 2925 },
    { 0x2081, 0x400, 10800, 3038 },
    { 0x2082, 0x400, 10800, 3038 },
    { 0x0081, 0,     10800, 3150 },
    { 0x0082, 0,     10800, 3150 },
    { 0x2081, 0x400, 10800, 3263 },
    { 0x2082, 0x400, 10800, 3263 },
    { 0x0081, 0,     10800, 3375 },
    { 0x0082, 0,     10800, 3375 },
    { 0x2081, 0x400, 10800, 3488 },
    { 0x2082, 0x400, 10800, 3488 }
};
static const mso_AutoShape msoSeal16 =
{
    (SvxMSDffVertPair*)mso_sptSeal16Vert, sizeof( mso_sptSeal16Vert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    (SvxMSDffCalculationData*)mso_sptSeal16Calc, sizeof( mso_sptSeal16Calc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault2500,
    (SvxMSDffTextRectangles*)mso_sptSealTextRect, sizeof( mso_sptSealTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};
static const SvxMSDffVertPair mso_sptSeal24Vert[] =
{
    { 0x05 MSO_I, 0x06 MSO_I }, { 0x07 MSO_I, 0x08 MSO_I }, { 0x09 MSO_I, 0x0a MSO_I }, { 0x0b MSO_I, 0x0c MSO_I },
    { 0x0d MSO_I, 0x0e MSO_I }, { 0x0f MSO_I, 0x10 MSO_I }, { 0x11 MSO_I, 0x12 MSO_I }, { 0x13 MSO_I, 0x14 MSO_I },
    { 0x15 MSO_I, 0x16 MSO_I }, { 0x17 MSO_I, 0x18 MSO_I }, { 0x19 MSO_I, 0x1a MSO_I }, { 0x1b MSO_I, 0x1c MSO_I },
    { 0x1d MSO_I, 0x1e MSO_I }, { 0x1f MSO_I, 0x20 MSO_I }, { 0x21 MSO_I, 0x22 MSO_I }, { 0x23 MSO_I, 0x24 MSO_I },
    { 0x25 MSO_I, 0x26 MSO_I }, { 0x27 MSO_I, 0x28 MSO_I }, { 0x29 MSO_I, 0x2a MSO_I }, { 0x2b MSO_I, 0x2c MSO_I },
    { 0x2d MSO_I, 0x2e MSO_I }, { 0x2f MSO_I, 0x30 MSO_I }, { 0x31 MSO_I, 0x32 MSO_I }, { 0x33 MSO_I, 0x34 MSO_I },
    { 0x35 MSO_I, 0x36 MSO_I }, { 0x37 MSO_I, 0x38 MSO_I }, { 0x39 MSO_I, 0x3a MSO_I }, { 0x3b MSO_I, 0x3c MSO_I },
    { 0x3d MSO_I, 0x3e MSO_I }, { 0x3f MSO_I, 0x40 MSO_I }, { 0x41 MSO_I, 0x42 MSO_I }, { 0x43 MSO_I, 0x44 MSO_I },
    { 0x45 MSO_I, 0x46 MSO_I }, { 0x47 MSO_I, 0x48 MSO_I }, { 0x49 MSO_I, 0x4a MSO_I }, { 0x4b MSO_I, 0x4c MSO_I },
    { 0x4d MSO_I, 0x4e MSO_I }, { 0x4f MSO_I, 0x50 MSO_I }, { 0x51 MSO_I, 0x52 MSO_I }, { 0x53 MSO_I, 0x54 MSO_I },
    { 0x55 MSO_I, 0x56 MSO_I }, { 0x57 MSO_I, 0x58 MSO_I }, { 0x59 MSO_I, 0x5a MSO_I }, { 0x5b MSO_I, 0x5c MSO_I },
    { 0x5d MSO_I, 0x5e MSO_I }, { 0x5f MSO_I, 0x60 MSO_I }, { 0x61 MSO_I, 0x62 MSO_I }, { 0x63 MSO_I, 0x64 MSO_I },
    { 0x05 MSO_I, 0x06 MSO_I }
};
static const mso_AutoShape msoSeal24 =
{
    (SvxMSDffVertPair*)mso_sptSeal24Vert, sizeof( mso_sptSeal24Vert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    (SvxMSDffCalculationData*)mso_sptSeal24Calc, sizeof( mso_sptSeal24Calc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault2500,
    (SvxMSDffTextRectangles*)mso_sptSealTextRect, sizeof( mso_sptSealTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};
static const SvxMSDffCalculationData mso_sptSeal32Calc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },     // 0x00
    { 0x2081, 0x400, 10800, 3150 },             // 0x01 ( textframe )
    { 0x2082, 0x400, 10800, 3150 },             // 0x02
    { 0x2081, 0x400, 10800, 1350 },             // 0x03
    { 0x2082, 0x400, 10800, 1350 },             // 0x04
    { 0x0081, 0,     10800, 0 },
    { 0x0082, 0,     10800, 0 },
    { 0x2081, 0x400, 10800, 56 },
    { 0x2082, 0x400, 10800, 56 },
    { 0x0081, 0,     10800, 113 },
    { 0x0082, 0,     10800, 113 },
    { 0x2081, 0x400, 10800, 169 },
    { 0x2082, 0x400, 10800, 169 },
    { 0x0081, 0,     10800, 225 },
    { 0x0082, 0,     10800, 225 },
    { 0x2081, 0x400, 10800, 281 },
    { 0x2082, 0x400, 10800, 281 },
    { 0x0081, 0,     10800, 338 },
    { 0x0082, 0,     10800, 338 },
    { 0x2081, 0x400, 10800, 394 },
    { 0x2082, 0x400, 10800, 394 },
    { 0x0081, 0,     10800, 450 },
    { 0x0082, 0,     10800, 450 },
    { 0x2081, 0x400, 10800, 506 },
    { 0x2082, 0x400, 10800, 506 },
    { 0x0081, 0,     10800, 563 },
    { 0x0082, 0,     10800, 563 },
    { 0x2081, 0x400, 10800, 619 },
    { 0x2082, 0x400, 10800, 619 },
    { 0x0081, 0,     10800, 675 },
    { 0x0082, 0,     10800, 675 },
    { 0x2081, 0x400, 10800, 731 },
    { 0x2082, 0x400, 10800, 731 },
    { 0x0081, 0,     10800, 788 },
    { 0x0082, 0,     10800, 788 },
    { 0x2081, 0x400, 10800, 843 },
    { 0x2082, 0x400, 10800, 843 },
    { 0x0081, 0,     10800, 900 },
    { 0x0082, 0,     10800, 900 },
    { 0x2081, 0x400, 10800, 956 },
    { 0x2082, 0x400, 10800, 956 },
    { 0x0081, 0,     10800, 1013 },
    { 0x0082, 0,     10800, 1013 },
    { 0x2081, 0x400, 10800, 1069 },
    { 0x2082, 0x400, 10800, 1069 },
    { 0x0081, 0,     10800, 1125 },
    { 0x0082, 0,     10800, 1125 },
    { 0x2081, 0x400, 10800, 1181 },
    { 0x2082, 0x400, 10800, 1181 },
    { 0x0081, 0,     10800, 1238 },
    { 0x0082, 0,     10800, 1238 },
    { 0x2081, 0x400, 10800, 1294 },
    { 0x2082, 0x400, 10800, 1294 },
    { 0x0081, 0,     10800, 1350 },
    { 0x0082, 0,     10800, 1350 },
    { 0x2081, 0x400, 10800, 1406 },
    { 0x2082, 0x400, 10800, 1406 },
    { 0x0081, 0,     10800, 1462 },
    { 0x0082, 0,     10800, 1462 },
    { 0x2081, 0x400, 10800, 1519 },
    { 0x2082, 0x400, 10800, 1519 },
    { 0x0081, 0,     10800, 1575 },
    { 0x0082, 0,     10800, 1575 },
    { 0x2081, 0x400, 10800, 1631 },
    { 0x2082, 0x400, 10800, 1631 },
    { 0x0081, 0,     10800, 1688 },
    { 0x0082, 0,     10800, 1688 },
    { 0x2081, 0x400, 10800, 1744 },
    { 0x2082, 0x400, 10800, 1744 },
    { 0x0081, 0,     10800, 1800 },
    { 0x0082, 0,     10800, 1800 },
    { 0x2081, 0x400, 10800, 1856 },
    { 0x2082, 0x400, 10800, 1856 },
    { 0x0081, 0,     10800, 1913 },
    { 0x0082, 0,     10800, 1913 },
    { 0x2081, 0x400, 10800, 1969 },
    { 0x2082, 0x400, 10800, 1969 },
    { 0x0081, 0,     10800, 2025 },
    { 0x0082, 0,     10800, 2025 },
    { 0x2081, 0x400, 10800, 2081 },
    { 0x2082, 0x400, 10800, 2081 },
    { 0x0081, 0,     10800, 2138 },
    { 0x0082, 0,     10800, 2138 },
    { 0x2081, 0x400, 10800, 2194 },
    { 0x2082, 0x400, 10800, 2194 },
    { 0x0081, 0,     10800, 2250 },
    { 0x0082, 0,     10800, 2250 },
    { 0x2081, 0x400, 10800, 2306 },
    { 0x2082, 0x400, 10800, 2306 },
    { 0x0081, 0,     10800, 2362 },
    { 0x0082, 0,     10800, 2362 },
    { 0x2081, 0x400, 10800, 2418 },
    { 0x2082, 0x400, 10800, 2418 },
    { 0x0081, 0,     10800, 2475 },
    { 0x0082, 0,     10800, 2475 },
    { 0x2081, 0x400, 10800, 2531 },
    { 0x2082, 0x400, 10800, 2531 },
    { 0x0081, 0,     10800, 2587 },
    { 0x0082, 0,     10800, 2587 },
    { 0x2081, 0x400, 10800, 2643 },
    { 0x2082, 0x400, 10800, 2643 },
    { 0x0081, 0,     10800, 2700 },
    { 0x0082, 0,     10800, 2700 },
    { 0x2081, 0x400, 10800, 2756 },
    { 0x2082, 0x400, 10800, 2756 },
    { 0x0081, 0,     10800, 2812 },
    { 0x0082, 0,     10800, 2812 },
    { 0x2081, 0x400, 10800, 2868 },
    { 0x2082, 0x400, 10800, 2868 },
    { 0x0081, 0,     10800, 2925 },
    { 0x0082, 0,     10800, 2925 },
    { 0x2081, 0x400, 10800, 2981 },
    { 0x2082, 0x400, 10800, 2981 },
    { 0x0081, 0,     10800, 3037 },
    { 0x0082, 0,     10800, 3037 },
    { 0x2081, 0x400, 10800, 3093 },
    { 0x2082, 0x400, 10800, 3093 },
    { 0x0081, 0,     10800, 3150 },
    { 0x0082, 0,     10800, 3150 },
    { 0x2081, 0x400, 10800, 3206 },
    { 0x2082, 0x400, 10800, 3206 },
    { 0x0081, 0,     10800, 3262 },
    { 0x0082, 0,     10800, 3262 },
    { 0x2081, 0x400, 10800, 3318 },
    { 0x2082, 0x400, 10800, 3318 },
    { 0x0081, 0,     10800, 3375 },
    { 0x0082, 0,     10800, 3375 },
    { 0x2081, 0x400, 10800, 3431 },
    { 0x2082, 0x400, 10800, 3431 },
    { 0x0081, 0,     10800, 3487 },
    { 0x0082, 0,     10800, 3487 },
    { 0x2081, 0x400, 10800, 3543 },
    { 0x2082, 0x400, 10800, 3543 }
};
static const SvxMSDffVertPair mso_sptSeal32Vert[] =
{
    { 0x05 MSO_I, 0x06 MSO_I }, { 0x07 MSO_I, 0x08 MSO_I }, { 0x09 MSO_I, 0x0a MSO_I }, { 0x0b MSO_I, 0x0c MSO_I },
    { 0x0d MSO_I, 0x0e MSO_I }, { 0x0f MSO_I, 0x10 MSO_I }, { 0x11 MSO_I, 0x12 MSO_I }, { 0x13 MSO_I, 0x14 MSO_I },
    { 0x15 MSO_I, 0x16 MSO_I }, { 0x17 MSO_I, 0x18 MSO_I }, { 0x19 MSO_I, 0x1a MSO_I }, { 0x1b MSO_I, 0x1c MSO_I },
    { 0x1d MSO_I, 0x1e MSO_I }, { 0x1f MSO_I, 0x20 MSO_I }, { 0x21 MSO_I, 0x22 MSO_I }, { 0x23 MSO_I, 0x24 MSO_I },
    { 0x25 MSO_I, 0x26 MSO_I }, { 0x27 MSO_I, 0x28 MSO_I }, { 0x29 MSO_I, 0x2a MSO_I }, { 0x2b MSO_I, 0x2c MSO_I },
    { 0x2d MSO_I, 0x2e MSO_I }, { 0x2f MSO_I, 0x30 MSO_I }, { 0x31 MSO_I, 0x32 MSO_I }, { 0x33 MSO_I, 0x34 MSO_I },
    { 0x35 MSO_I, 0x36 MSO_I }, { 0x37 MSO_I, 0x38 MSO_I }, { 0x39 MSO_I, 0x3a MSO_I }, { 0x3b MSO_I, 0x3c MSO_I },
    { 0x3d MSO_I, 0x3e MSO_I }, { 0x3f MSO_I, 0x40 MSO_I }, { 0x41 MSO_I, 0x42 MSO_I }, { 0x43 MSO_I, 0x44 MSO_I },
    { 0x45 MSO_I, 0x46 MSO_I }, { 0x47 MSO_I, 0x48 MSO_I }, { 0x49 MSO_I, 0x4a MSO_I }, { 0x4b MSO_I, 0x4c MSO_I },
    { 0x4d MSO_I, 0x4e MSO_I }, { 0x4f MSO_I, 0x50 MSO_I }, { 0x51 MSO_I, 0x52 MSO_I }, { 0x53 MSO_I, 0x54 MSO_I },
    { 0x55 MSO_I, 0x56 MSO_I }, { 0x57 MSO_I, 0x58 MSO_I }, { 0x59 MSO_I, 0x5a MSO_I }, { 0x5b MSO_I, 0x5c MSO_I },
    { 0x5d MSO_I, 0x5e MSO_I }, { 0x5f MSO_I, 0x60 MSO_I }, { 0x61 MSO_I, 0x62 MSO_I }, { 0x63 MSO_I, 0x64 MSO_I },
    { 0x65 MSO_I, 0x66 MSO_I }, { 0x67 MSO_I, 0x68 MSO_I }, { 0x69 MSO_I, 0x6a MSO_I }, { 0x6b MSO_I, 0x6c MSO_I },
    { 0x6d MSO_I, 0x6e MSO_I }, { 0x6f MSO_I, 0x70 MSO_I }, { 0x71 MSO_I, 0x72 MSO_I }, { 0x73 MSO_I, 0x74 MSO_I },
    { 0x75 MSO_I, 0x76 MSO_I }, { 0x77 MSO_I, 0x78 MSO_I }, { 0x79 MSO_I, 0x7a MSO_I }, { 0x7b MSO_I, 0x7c MSO_I },
    { 0x7d MSO_I, 0x7e MSO_I }, { 0x7f MSO_I, 0x80 MSO_I }, { 0x81 MSO_I, 0x82 MSO_I }, { 0x83 MSO_I, 0x84 MSO_I },
    { 0x05 MSO_I, 0x06 MSO_I }
};
static const mso_AutoShape msoSeal32 =
{
    (SvxMSDffVertPair*)mso_sptSeal32Vert, sizeof( mso_sptSeal32Vert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    (SvxMSDffCalculationData*)mso_sptSeal32Calc, sizeof( mso_sptSeal32Calc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault2500,
    (SvxMSDffTextRectangles*)mso_sptSealTextRect, sizeof( mso_sptSealTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptRibbon2Vert[] =    // adjustment1 : x 2700  - 8100     def 5400
{                                                       // adjustment2 : y 14400 - 21600    def 18900
    { 12 MSO_I, 1 MSO_I }, { 12 MSO_I, 13 MSO_I },                                      // pp
    { 12 MSO_I, 14 MSO_I }, { 15 MSO_I, 21600 }, { 16 MSO_I, 21600 },                   // ccp
    { 0, 21600 }, { 2750, 7 MSO_I }, { 0, 2 MSO_I }, { 0 MSO_I, 2 MSO_I },              // pppp
    { 0 MSO_I, 4 MSO_I },                                                               // p
    { 0 MSO_I, 5 MSO_I }, { 10 MSO_I, 0 }, { 11 MSO_I, 0 },                             // ccp
    { 17 MSO_I, 0 },                                                                    // p
    { 18 MSO_I, 0 }, { 19 MSO_I, 5 MSO_I }, { 19 MSO_I, 4 MSO_I },                      // ccp
    { 19 MSO_I, 2 MSO_I }, { 21600, 2 MSO_I }, { 18850, 7 MSO_I }, { 21600, 21600 },    // pppp
    { 20 MSO_I, 21600 },                                                                // p
    { 21 MSO_I, 21600 }, { 22 MSO_I, 14 MSO_I }, { 22 MSO_I, 13 MSO_I },                // ccp
    { 22 MSO_I, 1 MSO_I }, { 12 MSO_I, 1 MSO_I }, { 12 MSO_I, 13 MSO_I },               // ppp
    { 12 MSO_I, 23 MSO_I }, { 15 MSO_I, 24 MSO_I }, { 16 MSO_I, 24 MSO_I },             // ccp
    { 11 MSO_I, 24 MSO_I },                                                             // p
    { 10 MSO_I, 24 MSO_I }, { 0 MSO_I, 26 MSO_I }, { 0 MSO_I, 25 MSO_I },               // ccp
    { 0 MSO_I, 27 MSO_I }, { 10 MSO_I, 1 MSO_I }, { 11 MSO_I, 1 MSO_I },                // ccp

    { 22 MSO_I, 1 MSO_I }, { 22 MSO_I, 13 MSO_I },                                      // pp
    { 22 MSO_I, 23 MSO_I }, { 21 MSO_I, 24 MSO_I }, { 20 MSO_I, 24 MSO_I },             // ccp
    { 17 MSO_I, 24 MSO_I },                                                             // p
    { 18 MSO_I, 24 MSO_I }, { 19 MSO_I, 26 MSO_I }, { 19 MSO_I, 25 MSO_I },             // ccp
    { 19 MSO_I, 27 MSO_I }, { 18 MSO_I, 1 MSO_I }, { 17 MSO_I, 1 MSO_I },               // ccp

    { 0 MSO_I, 25 MSO_I }, { 0 MSO_I, 2 MSO_I },                                        // pp

    { 19 MSO_I, 25 MSO_I }, { 19 MSO_I, 2 MSO_I }                                       // pp
};
static const sal_uInt16 mso_sptRibbon2Segm[] =
{
    0x4000, 0x0001, 0x2001, 0x0005, 0x2001, 0x0001, 0x2001, 0x0005, 0x2001, 0x0001, 0x6001, 0x8000,
    0x4000, 0x0001, 0x2001, 0x0001, 0x2002, 0x6001, 0x8000,
    0x4000, 0x0001, 0x2001, 0x0001, 0x2002, 0x6001, 0x8000,
    0x4000, 0x0001, 0x8000,
    0x4000, 0x0001, 0x8000
};
static const SvxMSDffCalculationData mso_sptRibbon2Calc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },             // 00
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },            // 01
    { 0x8000, 21600, 0, 0x401 },                        // 02
    { 0x2001, 0x402, 1, 2 },                            // 03
    { 0x2001, 0x403, 1, 2 },                            // 04
    { 0x2001, 0x404, 1, 2 },                            // 05
    { 0x2001, 0x401, 1, 2 },                            // 06
    { 0x8000, 21600, 0, 0x406 },                        // 07
    { 0x0000, 420, 0, 0 },                              // 08
    { 0x2001, 0x408, 2, 1 },                            // 09
    { 0x6000, 0x400, 0x408, 0 },                        // 10
    { 0x6000, 0x400, 0x409, 0 },                        // 11
    { 0x2000, 0x400, 2700, 0 },                         // 12
    { 0x8000, 21600, 0, 0x404 },                        // 13
    { 0x8000, 21600, 0, 0x405 },                        // 14
    { 0xa000, 0x40c, 0, 0x408 },                        // 15
    { 0xa000, 0x40c, 0, 0x409 },                        // 16

    { 0x8000, 21600, 0, 0x40b },                        // 17
    { 0x8000, 21600, 0, 0x40a },                        // 18
    { 0x8000, 21600, 0, 0x400 },                        // 19
    { 0x8000, 21600, 0, 0x410 },                        // 20
    { 0x8000, 21600, 0, 0x40f },                        // 21
    { 0x8000, 21600, 0, 0x40c },                        // 22

    { 0xa000, 0x40d, 0, 0x405 },                        // 23
    { 0x6000, 0x401, 0x403, 0 },                        // 24
    { 0x6000, 0x401, 0x404, 0 },                        // 25
    { 0x6000, 0x419, 0x405, 0 },                        // 26
    { 0xa000, 0x419, 0, 0x405 }                         // 27
};
static const sal_Int32 mso_sptRibbon2Default[] =
{
    2, 5400, 18900
};
static const SvxMSDffTextRectangles mso_sptRibbon2TextRect[] =
{
    { { 0 MSO_I, 0 }, { 19 MSO_I, 1 MSO_I } }
};
static const mso_AutoShape msoRibbon2 =
{
    (SvxMSDffVertPair*)mso_sptRibbon2Vert, sizeof( mso_sptRibbon2Vert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptRibbon2Segm, sizeof( mso_sptRibbon2Segm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptRibbon2Calc, sizeof( mso_sptRibbon2Calc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptRibbon2Default,
    (SvxMSDffTextRectangles*)mso_sptRibbon2TextRect, sizeof( mso_sptRibbon2TextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptVerticalScrollVert[] = // adjustment1 : 0 - 5400
{
    { 1 MSO_I, 21600 }, { 0, 2 MSO_I }, { 1 MSO_I, 3 MSO_I }, { 0 MSO_I, 3 MSO_I },
    { 0 MSO_I, 1 MSO_I }, { 4 MSO_I, 0 }, { 2 MSO_I, 0 }, { 21600, 1 MSO_I },
    { 2 MSO_I, 0 MSO_I }, { 3 MSO_I, 0 MSO_I }, { 3 MSO_I, 2 MSO_I }, { 5 MSO_I, 21600 },

    { 6 MSO_I, 1 MSO_I }, { 4 MSO_I, 0 MSO_I }, { 8 MSO_I, 9 MSO_I }, { 4 MSO_I, 1 MSO_I },

    { 0 MSO_I, 2 MSO_I }, { 1 MSO_I, 21600 }, { 0, 2 MSO_I }, { 1 MSO_I, 3 MSO_I },
    { 9 MSO_I, 10 MSO_I }, { 1 MSO_I, 2 MSO_I },

    { 4 MSO_I, 0 }, { 6 MSO_I, 1 MSO_I },

    { 0 MSO_I, 3 MSO_I }, { 0 MSO_I, 2 MSO_I },

    { 4 MSO_I, 0 MSO_I },
    { 2 MSO_I, 0 MSO_I }
};
static const sal_uInt16 mso_sptVerticalScrollSegm[] =
{
    0x4000, 0xa702, 0x0002, 0xa801, 0x0001, 0xa702, 0x0002, 0xa801, 0x6001, 0x8000,
    0x4000, 0xa801, 0xa702, 0x6000, 0x8000,
    0x4000, 0xa803, 0xa702, 0x6001, 0x8000,
    0x4000, 0xa701, 0x8000,
    0x4000, 0x0001, 0x8000,
    0x4000, 0x0001, 0x8000
};
static const SvxMSDffCalculationData mso_sptScrollCalc[] =
{
    0x2000, DFF_Prop_adjustValue, 0, 0,
    0x2001, 0x400, 1, 2,
    0x8000, 21600, 0, 0x401,
    0x8000, 21600, 0, 0x400,
    0x6000, 0x400, 0x401, 0,
    0x8000, 21600, 0, 0x404,
    0x2001, 0x400, 2, 1,
    0x2001, 0x401, 1, 2,
    0x6000, 0x400, 0x407, 0,
    0x6000, 0x401, 0x407, 0,
    0x8000, 21600, 0, 0x409
};
static const SvxMSDffTextRectangles mso_sptScrollTextRect[] =
{
    { { 0 MSO_I, 0 MSO_I }, { 3 MSO_I, 3 MSO_I } }
};
static const mso_AutoShape msoVerticalScroll =
{
    (SvxMSDffVertPair*)mso_sptVerticalScrollVert, sizeof( mso_sptVerticalScrollVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptVerticalScrollSegm, sizeof( mso_sptVerticalScrollSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptScrollCalc, sizeof( mso_sptScrollCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault2700,
    (SvxMSDffTextRectangles*)mso_sptScrollTextRect, sizeof( mso_sptScrollTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    11000, 10800,
    NULL, 0
};
static const SvxMSDffVertPair mso_sptHorizontalScrollVert[] =   // adjustment1 : 0 - 5400
{
    { 0, 4 MSO_I }, { 1 MSO_I, 0 MSO_I }, { 3 MSO_I, 0 MSO_I }, { 3 MSO_I, 1 MSO_I },
    { 2 MSO_I, 0 }, { 21600, 1 MSO_I }, { 21600, 5 MSO_I }, { 2 MSO_I, 3 MSO_I },
    { 0 MSO_I, 3 MSO_I }, { 0 MSO_I, 2 MSO_I }, { 1 MSO_I, 21600 }, { 0, 2 MSO_I },

    { 1 MSO_I, 4 MSO_I }, { 9 MSO_I, 8 MSO_I }, { 0 MSO_I, 4 MSO_I }, { 1 MSO_I, 6 MSO_I },

    { 2 MSO_I, 1 MSO_I }, { 10 MSO_I,9 MSO_I }, { 3 MSO_I, 1 MSO_I }, { 2 MSO_I, 0 },
    { 21600, 1 MSO_I }, { 2 MSO_I, 0 MSO_I },

    { 1 MSO_I, 6 MSO_I },
    { 0, 4 MSO_I },

    { 2 MSO_I, 0 MSO_I },
    { 3 MSO_I, 0 MSO_I },

    { 0 MSO_I, 4 MSO_I },
    { 0 MSO_I, 2 MSO_I }
};
static const sal_uInt16 mso_sptHorizontalScrollSegm[] =
{
    0x4000, 0xa801, 0x0002, 0xa802, 0x0001, 0xa801, 0x0002, 0xa802, 0x6001, 0x8000,
    0x4000, 0xa803, 0x6000, 0x8000,
    0x4000, 0xa803, 0xa702, 0x6000, 0x8000,
    0x4000, 0xa701, 0x8000,
    0x4000, 0x0001, 0x8000,
    0x4000, 0x0001, 0x8000
};
static const mso_AutoShape msoHorizontalScroll =
{
    (SvxMSDffVertPair*)mso_sptHorizontalScrollVert, sizeof( mso_sptHorizontalScrollVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptHorizontalScrollSegm, sizeof( mso_sptHorizontalScrollSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptScrollCalc, sizeof( mso_sptScrollCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptDefault2700,
    (SvxMSDffTextRectangles*)mso_sptScrollTextRect, sizeof( mso_sptScrollTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    10800, 11000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptWaveVert[] =   // adjustment1 : 0 - 4459
{                                                   // adjustment2 : 8640 - 12960
    { 0, 0 }, { 21600, 21600 }
};
static const SvxMSDffCalculationData mso_sptWaveCalc[] =
{
    { 0x2000, DFF_Prop_adjustValue, 0, 0 },
    { 0x2000, DFF_Prop_adjust2Value, 0, 0 },
};
static const sal_uInt16 mso_sptWaveSegm[] =
{
    0x4000, 0x0001, 0x8000
};
static const sal_Int32 mso_sptWaveDefault[] =
{
    2, 2700, 10800
};
static const SvxMSDffTextRectangles mso_sptWaveTextRect[] =
{
    { { 0, 0 }, { 21600, 21600 } }
};
static const mso_AutoShape msoWave =
{
    (SvxMSDffVertPair*)mso_sptWaveVert, sizeof( mso_sptWaveVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptWaveSegm, sizeof( mso_sptWaveSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptWaveCalc, sizeof( mso_sptWaveCalc ) / sizeof( SvxMSDffCalculationData ),
    (sal_Int32*)mso_sptWaveDefault,
    (SvxMSDffTextRectangles*)mso_sptWaveTextRect, sizeof( mso_sptWaveTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptFlowChartProcessVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 }, { 0, 0 }
};
static const mso_AutoShape msoFlowChartProcess =
{
    (SvxMSDffVertPair*)mso_sptFlowChartProcessVert, sizeof( mso_sptFlowChartProcessVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    NULL, 0,
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartAlternateProcessVert[] =
{
    { 0, 2 MSO_I }, { 0 MSO_I, 0 }, { 1 MSO_I, 0 }, { 21600, 2 MSO_I },
    { 21600, 3 MSO_I }, { 1 MSO_I, 21600 }, { 0 MSO_I, 21600 }, { 0, 3 MSO_I }
};
static const sal_uInt16 mso_sptFlowChartAlternateProcessSegm[] =
{
    0x4000, 0xa801, 0x0001, 0xa701, 0x0001, 0xa801, 0x0001, 0xa701, 0x6000, 0x8000
};
static const SvxMSDffCalculationData mso_sptFlowChartAlternateProcessCalc[] =
{
    { 0x2000, DFF_Prop_geoLeft, 2540, 0 },
    { 0x2000, DFF_Prop_geoRight, 0, 2540 },
    { 0x2000, DFF_Prop_geoTop, 2540, 0 },
    { 0x2000, DFF_Prop_geoBottom, 0, 2540 },
    { 0x2000, DFF_Prop_geoLeft, 800, 0 },
    { 0x2000, DFF_Prop_geoRight, 0, 800 },
    { 0x2000, DFF_Prop_geoTop, 800, 0 },
    { 0x2000, DFF_Prop_geoBottom,0, 800 }
};
static const SvxMSDffTextRectangles mso_sptFlowChartAlternateProcessTextRect[] =
{
    { { 4 MSO_I, 6 MSO_I }, { 5 MSO_I, 7 MSO_I } }
};
static const mso_AutoShape msoFlowChartAlternateProcess =
{
    (SvxMSDffVertPair*)mso_sptFlowChartAlternateProcessVert, sizeof( mso_sptFlowChartAlternateProcessVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartAlternateProcessSegm, sizeof( mso_sptFlowChartAlternateProcessSegm ) >> 1,
    (SvxMSDffCalculationData*)mso_sptFlowChartAlternateProcessCalc, sizeof( mso_sptFlowChartAlternateProcessCalc ) / sizeof( SvxMSDffCalculationData ),
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartAlternateProcessTextRect, sizeof( mso_sptFlowChartAlternateProcessTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartDecisionVert[] =
{
    { 0, 10800 }, { 10800, 0 }, { 21600, 10800 }, { 10800, 21600 }, { 0, 10800 }
};
static const SvxMSDffTextRectangles mso_sptFlowChartDecisionTextRect[] =
{
    { { 5400, 5400 }, { 16200, 16200 } }
};
static const mso_AutoShape msoFlowChartDecision =
{
    (SvxMSDffVertPair*)mso_sptFlowChartDecisionVert, sizeof( mso_sptFlowChartDecisionVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartDecisionTextRect, sizeof( mso_sptFlowChartDecisionTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartInputOutputVert[] =
{
    { 4230, 0 }, { 21600, 0 }, { 17370, 21600 }, { 0, 21600 }, { 4230, 0 }
};
static const SvxMSDffTextRectangles mso_sptFlowChartInputOutputTextRect[] =
{
    { { 4230, 0 }, { 17370, 21600 } }
};
static const SvxMSDffVertPair mso_sptFlowChartInputOutputGluePoints[] =
{
    { 12960, 0 }, { 10800, 0 }, { 2160, 10800 }, { 8600, 21600 }, { 10800, 21600 }, { 19400, 10800 }
};
static const mso_AutoShape msoFlowChartInputOutput =
{
    (SvxMSDffVertPair*)mso_sptFlowChartInputOutputVert, sizeof( mso_sptFlowChartInputOutputVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartInputOutputTextRect, sizeof( mso_sptFlowChartInputOutputTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptFlowChartInputOutputGluePoints, sizeof( mso_sptFlowChartInputOutputGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartPredefinedProcessVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },

    { 2540, 0 }, { 2540, 21600 },

    { 21600 - 2540, 0 }, { 21600 - 2540, 21600 }
};
static const sal_uInt16 mso_sptFlowChartPredefinedProcessSegm[] =
{
    0x4000, 0x0003, 0x6000, 0x8000,
    0x4000, 0x0001, 0x8000,
    0x4000, 0x0001, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartPredefinedProcessTextRect[] =
{
    { { 2540, 0 }, { 21600 - 2540, 21600 } }
};
static const mso_AutoShape msoFlowChartPredefinedProcess =
{
    (SvxMSDffVertPair*)mso_sptFlowChartPredefinedProcessVert, sizeof( mso_sptFlowChartPredefinedProcessVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartPredefinedProcessSegm, sizeof( mso_sptFlowChartPredefinedProcessSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartPredefinedProcessTextRect, sizeof( mso_sptFlowChartPredefinedProcessTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptFlowChartInternalStorageVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },

    { 4230, 0 }, { 4230, 21600 },

    { 0, 4230 }, { 21600, 4230 }
};
static const sal_uInt16 mso_sptFlowChartInternalStorageSegm[] =
{
    0x4000, 0x0003, 0x6000, 0x8000,
    0x4000, 0x0001, 0x8000,
    0x4000, 0x0001, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartInternalStorageTextRect[] =
{
    { { 4230, 4230 }, { 21600, 21600 } }
};
static const mso_AutoShape msoFlowChartInternalStorage =
{
    (SvxMSDffVertPair*)mso_sptFlowChartInternalStorageVert, sizeof( mso_sptFlowChartInternalStorageVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartInternalStorageSegm, sizeof( mso_sptFlowChartInternalStorageSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartInternalStorageTextRect, sizeof( mso_sptFlowChartInternalStorageTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptFlowChartDocumentVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 17360 },
    { 13050, 17220 }, { 13340, 20770 }, { 5620, 21600 },    // ccp
    { 2860, 21100 }, { 1850, 20700 }, { 0,  20120 }         // ccp
};
static const sal_uInt16 mso_sptFlowChartDocumentSegm[] =
{
    0x4000, 0x0002, 0x2002, 0x6000, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartDocumentTextRect[] =
{
    { { 0, 0 }, { 21600, 17360 } }
};
static const SvxMSDffVertPair mso_sptFlowChartDocumentGluePoints[] =
{
    { 10800, 0 }, { 0, 10800 }, { 10800, 20320 }, { 21600, 10800 }
};
static const mso_AutoShape msoFlowChartDocument =
{
    (SvxMSDffVertPair*)mso_sptFlowChartDocumentVert, sizeof( mso_sptFlowChartDocumentVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartDocumentSegm, sizeof( mso_sptFlowChartDocumentSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartDocumentTextRect, sizeof( mso_sptFlowChartDocumentTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptFlowChartDocumentGluePoints, sizeof( mso_sptFlowChartDocumentGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartMultidocumentVert[] =
{
    { 0, 3600 }, { 1500, 3600 }, { 1500, 1800 }, { 3000, 1800 },
    { 3000, 0 }, { 21600, 0 }, { 21600, 14409 }, { 21600 - 1500, 14409 },
    { 21600 - 1500, 14409 + 1800 }, { 21600 - 3000, 14409 + 1800 }, { 21600 - 3000, 14409 + 3600 },
    { 11610, 14293 + 3600 }, { 11472, 17239 + 3600 }, { 4833, 17928 + 3600 },                       // ccp
    { 2450, 17513 + 3600 }, { 1591, 17181 + 3600 }, { 0, 16700 + 3600 },                            // ccp

    { 1500, 3600 }, { 21600 - 3000, 3600 }, { 21600 - 3000, 14409 + 1800 },

    { 3000, 1800 }, { 21600 - 1500, 1800 }, { 21600 - 1500, 14409 }
};
static const sal_uInt16 mso_sptFlowChartMultidocumentSegm[] =
{
    0x4000, 0x000a, 0x2002, 0x6000, 0x8000,
    0x4000, 0x0002, 0x8000,
    0x4000, 0x0002, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartMultidocumentTextRect[] =
{
    { { 0, 3600 }, { 21600 - 3000, 14409 + 3600 } }
};
static const SvxMSDffVertPair mso_sptFlowChartMultidocumentGluePoints[] =
{
    { 10800, 0 }, { 0, 10800 }, { 10800, 19890 }, { 21600, 10800 }
};
static const mso_AutoShape msoFlowChartMultidocument =
{
    (SvxMSDffVertPair*)mso_sptFlowChartMultidocumentVert, sizeof( mso_sptFlowChartMultidocumentVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartMultidocumentSegm, sizeof( mso_sptFlowChartMultidocumentSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartMultidocumentTextRect, sizeof( mso_sptFlowChartMultidocumentTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptFlowChartMultidocumentGluePoints, sizeof( mso_sptFlowChartMultidocumentGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartTerminatorVert[] =
{
    { 3470, 21600 }, { 0, 10800 }, { 3470, 0 }, { 18130, 0 },
    { 21600, 10800 }, { 18130, 21600 }
};
static const sal_uInt16 mso_sptFlowChartTerminatorSegm[] =
{
    0x4000, 0xa702, 0x0001, 0xa702, 0x6000, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartTerminatorTextRect[] =
{
    { { 1060, 3180 }, { 20540, 18420 } }
};
static const mso_AutoShape msoFlowChartTerminator =
{
    (SvxMSDffVertPair*)mso_sptFlowChartTerminatorVert, sizeof( mso_sptFlowChartTerminatorVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartTerminatorSegm, sizeof( mso_sptFlowChartTerminatorSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartTerminatorTextRect, sizeof( mso_sptFlowChartTerminatorTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartPreparationVert[] =
{
    { 4350, 0 }, { 17250, 0 }, { 21600, 10800 }, { 17250, 21600 },
    { 4350, 21600 }, { 0, 10800 }, { 4350, 0 }
};
static const SvxMSDffTextRectangles mso_sptFlowChartPreparationTextRect[] =
{
    { { 4350, 0 }, { 17250, 21600 } }
};
static const mso_AutoShape msoFlowChartPreparation =
{
    (SvxMSDffVertPair*)mso_sptFlowChartPreparationVert, sizeof( mso_sptFlowChartPreparationVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartPreparationTextRect, sizeof( mso_sptFlowChartPreparationTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartManualInputVert[] =
{
    { 0, 4300 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 }, { 0, 4300 }
};
static const SvxMSDffTextRectangles mso_sptFlowChartManualInputTextRect[] =
{
    { { 0, 4300 }, { 21600, 21600 } }
};
static const SvxMSDffVertPair mso_sptFlowChartManualInputGluePoints[] =
{
    { 10800, 2150 }, { 0, 10800 }, { 10800, 19890 }, { 21600, 10800 }
};
static const mso_AutoShape msoFlowChartManualInput =
{
    (SvxMSDffVertPair*)mso_sptFlowChartManualInputVert, sizeof( mso_sptFlowChartManualInputVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartManualInputTextRect, sizeof( mso_sptFlowChartManualInputTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptFlowChartManualInputGluePoints, sizeof( mso_sptFlowChartManualInputGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartManualOperationVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 17250, 21600 }, { 4350, 21600 }, { 0, 0 }
};
static const SvxMSDffTextRectangles mso_sptFlowChartManualOperationTextRect[] =
{
    { { 4350, 0 }, { 17250, 21600 } }
};
static const SvxMSDffVertPair mso_sptFlowChartManualOperationGluePoints[] =
{
    { 10800, 0 }, { 2160, 10800 }, { 10800, 21600 }, { 19440, 10800 }
};
static const mso_AutoShape msoFlowChartManualOperation =
{
    (SvxMSDffVertPair*)mso_sptFlowChartManualOperationVert, sizeof( mso_sptFlowChartManualOperationVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartManualOperationTextRect, sizeof( mso_sptFlowChartManualOperationTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptFlowChartManualOperationGluePoints, sizeof( mso_sptFlowChartManualOperationGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartConnectorVert[] =
{
    { 0, 0 }, { 21600, 21600 }
};
static const sal_uInt16 mso_sptFlowChartConnectorSegm[] =
{
    0xa302, 0x6000, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartConnectorTextRect[] =
{
    { { 3180, 3180 }, { 18420, 18420 } }
};
static const mso_AutoShape msoFlowChartConnector =
{
    (SvxMSDffVertPair*)mso_sptFlowChartConnectorVert, sizeof( mso_sptFlowChartConnectorVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartConnectorSegm, sizeof( mso_sptFlowChartConnectorSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartConnectorTextRect, sizeof( mso_sptFlowChartConnectorTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptEllipseGluePoints, sizeof( mso_sptEllipseGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartOffpageConnectorVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 17150 }, { 10800, 21600 },
    { 0, 17150 }, { 0, 0 }
};
static const SvxMSDffTextRectangles mso_sptFlowChartOffpageConnectorTextRect[] =
{
    { { 0, 0 }, { 21600, 17150 } }
};
static const mso_AutoShape msoFlowChartOffpageConnector =
{
    (SvxMSDffVertPair*)mso_sptFlowChartOffpageConnectorVert, sizeof( mso_sptFlowChartOffpageConnectorVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartOffpageConnectorTextRect, sizeof( mso_sptFlowChartOffpageConnectorTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartPunchedCardVert[] =
{
    { 4300, 0 }, { 21600, 0 }, { 21600, 21600 }, { 0, 21600 },
    { 0, 4300 }, { 4300, 0 }
};
static const SvxMSDffTextRectangles mso_sptFlowChartPunchedCardTextRect[] =
{
    { { 0, 4300 }, { 21600, 21600 } }
};
static const mso_AutoShape msoFlowChartPunchedCard =
{
    (SvxMSDffVertPair*)mso_sptFlowChartPunchedCardVert, sizeof( mso_sptFlowChartPunchedCardVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartPunchedCardTextRect, sizeof( mso_sptFlowChartPunchedCardTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartPunchedTapeVert[] =
{
    { 0, 2230 },                                            // p
    { 820, 3990 }, { 3410, 3980 }, { 5370, 4360 },          // ccp
    { 7430, 4030 }, { 10110, 3890 }, { 10690, 2270 },       // ccp
    { 11440, 300 }, { 14200, 160 }, { 16150, 0 },           // ccp
    { 18670, 170 }, {  20690, 390 }, { 21600, 2230 },       // ccp
    { 21600, 19420 },                                       // p
    { 20640, 17510 }, { 18320, 17490 }, { 16140, 17240 },   // ccp
    { 14710, 17370 }, { 11310, 17510 }, { 10770, 19430 },   // ccp
    { 10150, 21150 }, { 7380, 21290 }, { 5290, 21600 },     // ccp
    { 3220, 21250 }, { 610, 21130 }, { 0, 19420 }           // ccp
};
static const sal_uInt16 mso_sptFlowChartPunchedTapeSegm[] =
{
    0x4000, 0x2004, 0x0001, 0x2004, 0x6000, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartPunchedTapeTextRect[] =
{
    { { 0, 4360 }, { 21600, 17240 } }
};
static const SvxMSDffVertPair mso_sptFlowChartPunchedTapeGluePoints[] =
{
    { 10800, 2020 }, { 0, 10800 }, { 10800, 19320 }, { 21600, 10800 }
};
static const mso_AutoShape msoFlowChartPunchedTape =
{
    (SvxMSDffVertPair*)mso_sptFlowChartPunchedTapeVert, sizeof( mso_sptFlowChartPunchedTapeVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartPunchedTapeSegm, sizeof( mso_sptFlowChartPunchedTapeSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartPunchedTapeTextRect, sizeof( mso_sptFlowChartPunchedTapeTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptFlowChartPunchedTapeGluePoints, sizeof( mso_sptFlowChartPunchedTapeGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartSummingJunctionVert[] =
{
    { 0, 0 }, { 21600, 21600 },

    { 3100, 3100 },
    { 18500, 18500 },

    { 3100, 18500 },
    { 18500, 3100 }
};
static const sal_uInt16 mso_sptFlowChartSummingJunctionSegm[] =
{
    0xa302, 0x6000, 0x8000,
    0x4000, 0x0001, 0x8000,
    0x4000, 0x0001, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartSummingJunctionTextRect[] =
{
    { { 3100, 3100 }, { 18500, 18500 } }
};
static const mso_AutoShape msoFlowChartSummingJunction =
{
    (SvxMSDffVertPair*)mso_sptFlowChartSummingJunctionVert, sizeof( mso_sptFlowChartSummingJunctionVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartSummingJunctionSegm, sizeof( mso_sptFlowChartSummingJunctionSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartSummingJunctionTextRect, sizeof( mso_sptFlowChartSummingJunctionTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptEllipseGluePoints, sizeof( mso_sptEllipseGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartOrVert[] =
{
    { 0, 0 }, { 21600, 21600 },

    { 0, 10800 }, { 21600, 10800 },

    { 10800, 0 }, { 10800, 21600 }
};
static const sal_uInt16 mso_sptFlowChartOrSegm[] =
{
    0xa302, 0x6000, 0x8000,
    0x4000, 0x0001, 0x8000,
    0x4000, 0x0001, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartOrTextRect[] =
{
    { { 3100, 3100 }, { 18500, 18500 } }
};
static const mso_AutoShape msoFlowChartOr =
{
    (SvxMSDffVertPair*)mso_sptFlowChartOrVert, sizeof( mso_sptFlowChartOrVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartOrSegm, sizeof( mso_sptFlowChartOrSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartOrTextRect, sizeof( mso_sptFlowChartOrTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptEllipseGluePoints, sizeof( mso_sptEllipseGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartCollateVert[] =
{
    { 0, 0 }, { 21600, 21600 }, { 0, 21600 }, { 21600, 0 }, { 0, 0 }
};
static const SvxMSDffTextRectangles mso_sptFlowChartCollateTextRect[] =
{
    { { 5400, 5400 }, { 16200, 16200 } }
};
static const SvxMSDffVertPair mso_sptFlowChartCollateGluePoints[] =
{
    { 10800, 0 }, { 10800, 10800 }, { 10800, 21600 }
};
static const mso_AutoShape msoFlowChartCollate =
{
    (SvxMSDffVertPair*)mso_sptFlowChartCollateVert, sizeof( mso_sptFlowChartCollateVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartCollateTextRect, sizeof( mso_sptFlowChartCollateTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptFlowChartCollateGluePoints, sizeof( mso_sptFlowChartCollateGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartSortVert[] =
{
    { 0, 10800 }, { 10800, 0 }, { 21600, 10800 }, { 10800, 21600 },

    { 0, 10800 }, { 21600, 10800 }
};
static const sal_uInt16 mso_sptFlowChartSortSegm[] =
{
    0x4000, 0x0003, 0x6000, 0x8000,
    0x4000, 0x0001, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartSortTextRect[] =
{
    { { 5400, 5400 }, { 16200, 16200 } }
};
static const mso_AutoShape msoFlowChartSort =
{
    (SvxMSDffVertPair*)mso_sptFlowChartSortVert, sizeof( mso_sptFlowChartSortVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartSortSegm, sizeof( mso_sptFlowChartSortSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartSortTextRect, sizeof( mso_sptFlowChartSortTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptFlowChartExtractVert[] =
{
    { 10800, 0 }, { 21600, 21600 }, { 0, 21600 }, { 10800, 0 }
};
static const SvxMSDffTextRectangles mso_sptFlowChartExtractTextRect[] =
{
    { { 5400, 10800 }, { 16200, 21600 } }
};
static const SvxMSDffVertPair mso_sptFlowChartExtractGluePoints[] =
{
    { 10800, 0 }, { 5400, 10800 }, { 10800, 21600 }, { 16200, 10800 }
};
static const mso_AutoShape msoFlowChartExtract =
{
    (SvxMSDffVertPair*)mso_sptFlowChartExtractVert, sizeof( mso_sptFlowChartExtractVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartExtractTextRect, sizeof( mso_sptFlowChartExtractTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptFlowChartExtractGluePoints, sizeof( mso_sptFlowChartExtractGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartMergeVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 10800, 21600 }, { 0, 0 }
};
static const SvxMSDffTextRectangles mso_sptFlowChartMergeTextRect[] =
{
    { { 5400, 0 }, { 16200, 10800 } }
};
static const mso_AutoShape msoFlowChartMerge =
{
    (SvxMSDffVertPair*)mso_sptFlowChartMergeVert, sizeof( mso_sptFlowChartMergeVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartMergeTextRect, sizeof( mso_sptFlowChartMergeTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptFlowChartExtractGluePoints, sizeof( mso_sptFlowChartExtractGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartOnlineStorageVert[] =
{
    { 3600, 21600 }, { 0, 10800 }, { 3600, 0 }, { 21600, 0 },
    { 18000, 10800 }, { 21600, 21600 }
};
static const sal_uInt16 mso_sptFlowChartOnlineStorageSegm[] =
{
    0x4000, 0xa702, 0x0001, 0xa702, 0x6000, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartOnlineStorageTextRect[] =
{
    { { 3600, 0 }, { 18000, 21600 } }
};
static const SvxMSDffVertPair mso_sptFlowChartOnlineStorageGluePoints[] =
{
    { 10800, 0 }, { 0, 10800 }, { 10800, 21600 }, { 18000, 10800 }
};
static const mso_AutoShape msoFlowChartOnlineStorage =
{
    (SvxMSDffVertPair*)mso_sptFlowChartOnlineStorageVert, sizeof( mso_sptFlowChartOnlineStorageVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartOnlineStorageSegm, sizeof( mso_sptFlowChartOnlineStorageSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartOnlineStorageTextRect, sizeof( mso_sptFlowChartOnlineStorageTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptFlowChartOnlineStorageGluePoints, sizeof( mso_sptFlowChartOnlineStorageGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartDelayVert[] =
{
    { 10800, 0 }, { 21600, 10800 }, { 10800, 21600 }, { 0, 21600 },
    { 0, 0 }
};
static const sal_uInt16 mso_sptFlowChartDelaySegm[] =
{
    0x4000, 0xa702, 0x0002, 0x6000, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartDelayTextRect[] =
{
    { { 0, 3100 }, { 18500, 18500 } }
};
static const mso_AutoShape msoFlowChartDelay =
{
    (SvxMSDffVertPair*)mso_sptFlowChartDelayVert, sizeof( mso_sptFlowChartDelayVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartDelaySegm, sizeof( mso_sptFlowChartDelaySegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartDelayTextRect, sizeof( mso_sptFlowChartDelayTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartMagneticTapeVert[] =
{
    { 20980, 18150 }, { 20980, 21600 }, { 10670, 21600 },
    { 4770, 21540 }, { 0, 16720 }, { 0, 10800 },            // ccp
    { 0, 4840 }, { 4840, 0 }, { 10800, 0 },                 // ccp
    { 16740, 0 }, { 21600, 4840 }, { 21600, 10800 },        // ccp
    { 21600, 13520 }, { 20550, 16160 }, { 18670, 18170 }    // ccp
};
static const sal_uInt16 mso_sptFlowChartMagneticTapeSegm[] =
{
    0x4000, 0x0002, 0x2004, 0x6000, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartMagneticTapeTextRect[] =
{
    { { 3100, 3100 }, { 18500, 18500 } }
};
static const mso_AutoShape msoFlowChartMagneticTape =
{
    (SvxMSDffVertPair*)mso_sptFlowChartMagneticTapeVert, sizeof( mso_sptFlowChartMagneticTapeVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartMagneticTapeSegm, sizeof( mso_sptFlowChartMagneticTapeSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartMagneticTapeTextRect, sizeof( mso_sptFlowChartMagneticTapeTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartMagneticDiskVert[] =
{
    { 0, 3400 }, { 10800, 0 }, { 21600, 3400 }, { 21600, 18200 },
    { 10800, 21600 }, { 0, 18200 },

    { 0, 3400 }, { 10800, 6800 }, { 21600, 3400 }
};
static const sal_uInt16 mso_sptFlowChartMagneticDiskSegm[] =
{
    0x4000, 0xa802, 0x0001, 0xa802, 0x6000, 0x8000,
    0x4000, 0xa802, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartMagneticDiskTextRect[] =
{
    { { 0, 6800 }, { 21600, 18200 } }
};
static const SvxMSDffVertPair mso_sptFlowChartMagneticDiskGluePoints[] =
{
    { 10800, 6800 }, { 10800, 0 }, { 0, 10800 }, { 10800, 21600 }, { 21600, 10800 }
};
static const mso_AutoShape msoFlowChartMagneticDisk =
{
    (SvxMSDffVertPair*)mso_sptFlowChartMagneticDiskVert, sizeof( mso_sptFlowChartMagneticDiskVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartMagneticDiskSegm, sizeof( mso_sptFlowChartMagneticDiskSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartMagneticDiskTextRect, sizeof( mso_sptFlowChartMagneticDiskTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptFlowChartMagneticDiskGluePoints, sizeof( mso_sptFlowChartMagneticDiskGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartMagneticDrumVert[] =
{
    { 18200, 0 }, { 21600, 10800 }, { 18200, 21600 }, { 3400, 21600 },
    { 0, 10800 }, { 3400, 0 },

    { 18200, 0 }, { 14800, 10800 }, { 18200, 21600 }
};
static const sal_uInt16 mso_sptFlowChartMagneticDrumSegm[] =
{
    0x4000, 0xa702, 0x0001, 0xa702, 0x6000, 0x8000,
    0x4000, 0xa702, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartMagneticDrumTextRect[] =
{
    { { 3400, 0 }, { 14800, 21600 } }
};
static const SvxMSDffVertPair mso_sptFlowChartMagneticDrumGluePoints[] =
{
    { 10800, 0 }, { 0, 10800 }, { 10800, 21600 }, { 14800, 10800 }, { 21600, 10800 }
};
static const mso_AutoShape msoFlowChartMagneticDrum =
{
    (SvxMSDffVertPair*)mso_sptFlowChartMagneticDrumVert, sizeof( mso_sptFlowChartMagneticDrumVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartMagneticDrumSegm, sizeof( mso_sptFlowChartMagneticDrumSegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartMagneticDrumTextRect, sizeof( mso_sptFlowChartMagneticDrumTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptFlowChartMagneticDrumGluePoints, sizeof( mso_sptFlowChartMagneticDrumGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptFlowChartDisplayVert[] =
{
    { 3600, 0 }, { 17800, 0 }, { 21600, 10800 }, { 17800, 21600 },
    { 3600, 21600 }, { 0, 10800 }
};
static const sal_uInt16 mso_sptFlowChartDisplaySegm[] =
{
    0x4000, 0x0001, 0xa702, 0x0002, 0x6000, 0x8000
};
static const SvxMSDffTextRectangles mso_sptFlowChartDisplayTextRect[] =
{
    { { 3600, 0 }, { 17800, 21600 } }
};
static const mso_AutoShape msoFlowChartDisplay =
{
    (SvxMSDffVertPair*)mso_sptFlowChartDisplayVert, sizeof( mso_sptFlowChartDisplayVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptFlowChartDisplaySegm, sizeof( mso_sptFlowChartDisplaySegm ) >> 1,
    NULL, 0,
    NULL,
    (SvxMSDffTextRectangles*)mso_sptFlowChartDisplayTextRect, sizeof( mso_sptFlowChartDisplayTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    (SvxMSDffVertPair*)mso_sptStandardGluePoints, sizeof( mso_sptStandardGluePoints ) / sizeof( SvxMSDffVertPair )
};

static const SvxMSDffVertPair mso_sptWedgeRectCalloutVert[] =
{
    { 0, 0 }, { 21600, 0 }, { 21600, 21600 }, { 8900, 21600 }, { 1300, 25930 }, { 3500, 21600 }, { 0, 21600 }
};
static const sal_Int32 mso_sptWedgeRectCalloutDefault[] =
{
    2, 1350, 25920
};
static const SvxMSDffTextRectangles mso_sptWedgeRectCalloutTextRect[] =
{
    { { 0, 0 }, { 21600, 21600 } }
};
static const mso_AutoShape msoWedgeRectCallout =
{
    (SvxMSDffVertPair*)mso_sptWedgeRectCalloutVert, sizeof( mso_sptWedgeRectCalloutVert ) / sizeof( SvxMSDffVertPair ),
    NULL, 0,
    NULL, 0,
    (sal_Int32*)mso_sptWedgeRectCalloutDefault,
    (SvxMSDffTextRectangles*)mso_sptWedgeRectCalloutTextRect, sizeof( mso_sptWedgeRectCalloutTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptWedgeRRectCalloutVert[] =
{
    { 0, 3100 }, { 3100, 0 }, { 18500, 0 }, { 21600, 3100 }, { 21600, 18500 }, { 18500, 21600 },
    { 9100, 21600 }, { 1300, 25930 }, { 3500, 21600 }, { 0, 18600 }
};
static const sal_uInt16 mso_sptWedgeRRectCalloutSegm[] =
{
    0x4000, 0xa801, 0x0001, 0xa701, 0x0001, 0xa801, 0x0003, 0xa701, 0x6001, 0x8000
};
static const sal_Int32 mso_sptWedgeRRectCalloutDefault[] =
{
    2, 1350, 25920
};
static const SvxMSDffTextRectangles mso_sptWedgeRRectCalloutTextRect[] =
{
    { { 800, 800 }, { 20800, 20800 } }
};
static const mso_AutoShape msoWedgeRRectCallout =
{
    (SvxMSDffVertPair*)mso_sptWedgeRRectCalloutVert, sizeof( mso_sptWedgeRRectCalloutVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptWedgeRRectCalloutSegm, sizeof( mso_sptWedgeRRectCalloutSegm ) >> 1,
    NULL, 0,
    (sal_Int32*)mso_sptWedgeRRectCalloutDefault,
    (SvxMSDffTextRectangles*)mso_sptWedgeRRectCalloutTextRect, sizeof( mso_sptWedgeRRectCalloutTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptWedgeEllipseCalloutVert[] =
{
    { 0, 0 }, { 21600, 21600 }, { 3300, 18730 }, { 6900 , 20730 }, { 1300, 25930 }
};
static const sal_uInt16 mso_sptWedgeEllipseCalloutSegm[] =
{
    0xb504, 0x6001, 0x0001, 0x8000
};
static const sal_Int32 mso_sptWedgeEllipseCalloutDefault[] =
{
    2, 1350, 25920
};
static const SvxMSDffTextRectangles mso_sptWedgeEllipseCalloutTextRect[] =
{
    { { 3200, 3200 }, { 18400, 18400 } }
};
static const mso_AutoShape msoWedgeEllipseCallout =
{
    (SvxMSDffVertPair*)mso_sptWedgeEllipseCalloutVert, sizeof( mso_sptWedgeEllipseCalloutVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptWedgeEllipseCalloutSegm, sizeof( mso_sptWedgeEllipseCalloutSegm ) >> 1,
    NULL, 0,
    (sal_Int32*)mso_sptWedgeEllipseCalloutDefault,
    (SvxMSDffTextRectangles*)mso_sptWedgeEllipseCalloutTextRect, sizeof( mso_sptWedgeEllipseCalloutTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

static const SvxMSDffVertPair mso_sptCloudCalloutVert[] =
{
    { 1930,7160 },                                      // p
    { 1530,4490 }, { 3400,1970 }, { 5270,1970 },        // ccp
    { 5860,1950 }, { 6470,2210 }, { 6970,2600 },        // ccp
    { 7450,1390 }, { 8340,650 }, { 9340,650 },          // ccp
    { 10004,690 }, { 10710,1050 }, { 11210,1700 },      // ccp
    { 11570,630 }, { 12330,0 }, { 13150,0 },            // ccp
    { 13840,0 }, { 14470,460 }, { 14870,1160 },         // ccp
    { 15330,440 }, { 16020,0 }, { 16740,0 },            // ccp
    { 17910,0 }, { 18900,1130 }, { 19110,2710 },        // ccp
    { 20240,3150 }, { 21060,4580 }, { 21060,6220 },     // ccp
    { 21060,6720 }, { 21000,7200 }, { 20830,7660 },     // ccp
    { 21310,8460 }, { 21600,9450 }, { 21600,10460 },    // ccp
    { 21600,12750 }, { 20310,14680 }, { 18650,15010 },  // ccp
    { 18650,17200 }, { 17370,18920 }, { 15770,18920 },  // ccp
    { 15220,18920 }, { 14700,18710 }, { 14240,18310 },  // ccp
    { 13820,20240 }, { 12490,21600 }, { 11000,21600 },  // ccp
    { 9890,21600 }, { 8840,20790 }, { 8210,19510 },     // ccp
    { 7620,20000 }, { 7930,20290 }, { 6240,20290 },     // ccp
    { 4850,20290 }, { 3570,19280 }, { 2900,17640 },     // ccp
    { 1300,17600 }, { 480,16300 }, { 480,14660 },       // ccp
    { 480,13900 }, { 690,13210 }, { 1070,12640 },       // ccp
    { 380,12160 }, { 0,11210 }, { 0,10120 },            // ccp
    { 0,8590 }, { 840,7330 }, { 1930,7160 },            // ccp

    { 1930, 7160 }, { 1950, 7410 }, { 2040, 7690 }, { 2090, 7920 },         // pccp
    { 6970, 2600 }, { 7200, 2790 }, { 7480, 3050 }, { 7670, 3310 },         // pccp
    { 11210, 1700 }, { 11130, 1910 }, { 11080, 2160 }, { 11030, 2400 },     // pccp
    { 14870, 1160 }, { 14720, 1400 }, { 14640, 1720 }, { 14540, 2010 },     // pccp
    { 19110, 2710 }, { 19130, 2890 }, { 19230, 3290 }, { 19190, 3380 },     // pccp
    { 20830, 7660 }, { 20660, 8170 }, { 20430, 8620 }, { 20110, 8990 },     // pccp
    { 18660, 15010 }, { 18740, 14200 }, { 18280, 12200 }, { 17000, 11450 }, // pccp
    { 14240, 18310 }, { 14320, 17980 }, { 14350, 17680 }, { 14370, 17360 }, // pccp
    { 8220, 19510 }, { 8060, 19250 }, { 7960, 18950 }, { 7860, 18640 },     // pccp
    { 2900, 17640 }, { 3090, 17600 }, { 3280, 17540 }, { 3460, 17450 },     // pccp
    { 1070, 12640 }, { 1400, 12900 }, { 1780, 13130 }, { 2330, 13040 },     // pccp

    { 2220, 19800 }, { 5820, 23400 },                                       // circ1
    { 1170, 23070 }, { 3570, 25470 },                                       // circ2
    { 730, 25320 }, { 1930, 26520 }                                         // circ3

};
static const sal_uInt16 mso_sptCloudCalloutSegm[] =
{
    0x4000, 0x2016, 0x6001, 0x8000,
    0x4000, 0x2001, 0x8000,
    0x4000, 0x2001, 0x8000,
    0x4000, 0x2001, 0x8000,
    0x4000, 0x2001, 0x8000,
    0x4000, 0x2001, 0x8000,
    0x4000, 0x2001, 0x8000,
    0x4000, 0x2001, 0x8000,
    0x4000, 0x2001, 0x8000,
    0x4000, 0x2001, 0x8000,
    0x4000, 0x2001, 0x8000,
    0x4000, 0x2001, 0x8000,
    0xb502, 0x6001, 0x8000,
    0xb502, 0x6001, 0x8000,
    0xb502, 0x6001, 0x8000
};
static const sal_Int32 mso_sptCloudCalloutDefault[] =
{
    2, 1350, 25920
};
static const SvxMSDffTextRectangles mso_sptCloudCalloutTextRect[] =
{
    { { 3000, 3320 }, { 17110, 17330 } }
};
static const mso_AutoShape msoCloudCallout =
{
    (SvxMSDffVertPair*)mso_sptCloudCalloutVert, sizeof( mso_sptCloudCalloutVert ) / sizeof( SvxMSDffVertPair ),
    (sal_uInt16*)mso_sptCloudCalloutSegm, sizeof( mso_sptCloudCalloutSegm ) >> 1,
    NULL, 0,
    (sal_Int32*)mso_sptCloudCalloutDefault,
    (SvxMSDffTextRectangles*)mso_sptCloudCalloutTextRect, sizeof( mso_sptCloudCalloutTextRect ) / sizeof( SvxMSDffTextRectangles ),
    21600, 21600,
    0x80000000, 0x80000000,
    NULL, 0
};

class SvxMSDffAdjustmentHandle
{
    sal_Int32   nAdjustValue;
    sal_Int32   nDefaultValue;
    sal_Bool    bIsDefault;

    public :
                SvxMSDffAdjustmentHandle() : nAdjustValue( 0 ), nDefaultValue( 0 ), bIsDefault( TRUE ) {};

    void        SetAdjustValue( sal_Int32 nVal ) { nAdjustValue = nVal; bIsDefault = FALSE; };
    sal_Int32   GetAdjustValue() const { return bIsDefault ? nDefaultValue : nAdjustValue; };
    void        SetDefaultValue( sal_Int32 nVal ) { nDefaultValue = nVal; };
    sal_Bool    IsDefault() const { return bIsDefault; };
};

const mso_AutoShape* GetAutoShapeContent( MSO_SPT eSpType )
{
    const mso_AutoShape* pAutoShape = NULL;
    switch( eSpType )
    {
        case mso_sptArc :                       pAutoShape = &msoArc; break;
        case mso_sptRectangle :                 pAutoShape = &msoRectangle; break;
        case mso_sptParallelogram :             pAutoShape = &msoParallelogram; break;
        case mso_sptTrapezoid :                 pAutoShape = &msoTrapezoid; break;
        case mso_sptDiamond :                   pAutoShape = &msoDiamond; break;
        case mso_sptRoundRectangle :            pAutoShape = &msoRoundRectangle; break;
        case mso_sptOctagon :                   pAutoShape = &msoOctagon; break;
        case mso_sptIsocelesTriangle :          pAutoShape = &msoIsocelesTriangle; break;
        case mso_sptRightTriangle :             pAutoShape = &msoRightTriangle; break;
        case mso_sptEllipse :                   pAutoShape = &msoEllipse; break;
        case mso_sptHexagon :                   pAutoShape = &msoHexagon; break;
        case mso_sptPlus :                      pAutoShape = &msoPlus; break;
        case mso_sptPentagon :                  pAutoShape = &msoPentagon; break;
        case mso_sptCan :                       pAutoShape = &msoCan; break;
        case mso_sptCube :                      pAutoShape = &msoCube; break;
        case mso_sptActionButtonBlank :         pAutoShape = &msoActionButtonBlank; break;
        case mso_sptActionButtonHome :          pAutoShape = &msoActionButtonHome; break;
        case mso_sptActionButtonHelp :          pAutoShape = &msoActionButtonHelp; break;
        case mso_sptActionButtonInformation :   pAutoShape = &msoActionButtonInformation; break;
        case mso_sptActionButtonBackPrevious :  pAutoShape = &msoActionButtonBackPrevious; break;
        case mso_sptActionButtonForwardNext :   pAutoShape = &msoActionButtonForwardNext; break;
        case mso_sptActionButtonBeginning :     pAutoShape = &msoActionButtonBeginning; break;
        case mso_sptActionButtonEnd :           pAutoShape = &msoActionButtonEnd; break;
        case mso_sptActionButtonReturn :        pAutoShape = &msoActionButtonReturn;    break;
        case mso_sptActionButtonDocument :      pAutoShape = &msoActionButtonDocument; break;
        case mso_sptActionButtonSound :         pAutoShape = &msoActionButtonSound; break;
        case mso_sptActionButtonMovie :         pAutoShape = &msoActionButtonMovie; break;
        case mso_sptBevel :                     pAutoShape = &msoBevel; break;
        case mso_sptFoldedCorner :              pAutoShape = &msoFoldedCorner; break;
        case mso_sptSmileyFace :                pAutoShape = &msoSmileyFace;    break;
        case mso_sptDonut :                     pAutoShape = &msoDonut; break;
        case mso_sptNoSmoking :                 pAutoShape = &msoNoSmoking; break;
        case mso_sptBlockArc :                  pAutoShape = &msoBlockArc; break;
        case mso_sptHeart :                     pAutoShape = &msoHeart; break;
        case mso_sptLightningBolt :             pAutoShape = &msoLightningBold; break;
        case mso_sptSun :                       pAutoShape = &msoSun; break;
        case mso_sptMoon :                      pAutoShape = &msoMoon; break;
        case mso_sptBracketPair :               pAutoShape = &msoBracketPair; break;
        case mso_sptBracePair :                 pAutoShape = &msoBracePair; break;
        case mso_sptPlaque :                    pAutoShape = &msoPlaque; break;
        case mso_sptLeftBracket :               pAutoShape = &msoLeftBracket; break;
        case mso_sptRightBracket :              pAutoShape = &msoRightBracket; break;
        case mso_sptLeftBrace :                 pAutoShape = &msoLeftBrace; break;
        case mso_sptRightBrace :                pAutoShape = &msoRightBrace; break;
        case mso_sptArrow :                     pAutoShape = &msoArrow; break;
        case mso_sptUpArrow :                   pAutoShape = &msoUpArrow; break;
        case mso_sptDownArrow :                 pAutoShape = &msoDownArrow; break;
        case mso_sptLeftArrow :                 pAutoShape = &msoLeftArrow; break;
        case mso_sptLeftRightArrow :            pAutoShape = &msoLeftRightArrow; break;
        case mso_sptUpDownArrow :               pAutoShape = &msoUpDownArrow; break;
        case mso_sptQuadArrow :                 pAutoShape = &msoQuadArrow; break;
        case mso_sptLeftRightUpArrow :          pAutoShape = &msoLeftRightUpArrow; break;
        case mso_sptBentArrow :                 pAutoShape = &msoBentArrow; break;
        case mso_sptUturnArrow :                pAutoShape = &msoUturnArrow; break;
        case mso_sptLeftUpArrow :               pAutoShape = &msoLeftUpArrow; break;
        case mso_sptBentUpArrow :               pAutoShape = &msoBentUpArrow; break;
        case mso_sptCurvedRightArrow :          pAutoShape = &msoCurvedArrow; break;
        case mso_sptCurvedLeftArrow :           pAutoShape = &msoCurvedArrow; break;
        case mso_sptCurvedUpArrow :             pAutoShape = &msoCurvedArrow; break;
        case mso_sptCurvedDownArrow :           pAutoShape = &msoCurvedArrow; break;
        case mso_sptStripedRightArrow :         pAutoShape = &msoStripedRightArrow; break;
        case mso_sptNotchedRightArrow :         pAutoShape = &msoNotchedRightArrow; break;
        case mso_sptHomePlate :                 pAutoShape = &msoHomePlate; break;
        case mso_sptChevron :                   pAutoShape = &msoChevron; break;
        case mso_sptRightArrowCallout :         pAutoShape = &msoRightArrowCallout; break;
        case mso_sptLeftArrowCallout :          pAutoShape = &msoLeftArrowCallout; break;
        case mso_sptUpArrowCallout :            pAutoShape = &msoUpArrowCallout; break;
        case mso_sptDownArrowCallout :          pAutoShape = &msoDownArrowCallout; break;
        case mso_sptLeftRightArrowCallout :     pAutoShape = &msoLeftRightArrowCallout; break;
        case mso_sptUpDownArrowCallout :        pAutoShape = &msoUpDownArrowCallout; break;
        case mso_sptQuadArrowCallout :          pAutoShape = &msoQuadArrowCallout; break;
        case mso_sptCircularArrow :             pAutoShape = &msoCircularArrow; break;
        case mso_sptIrregularSeal1 :            pAutoShape = &msoIrregularSeal1; break;
        case mso_sptIrregularSeal2 :            pAutoShape = &msoIrregularSeal2; break;
        case mso_sptSeal4 :                     pAutoShape = &msoSeal4; break;
        case mso_sptStar :                      pAutoShape = &msoStar; break;
        case mso_sptSeal8 :                     pAutoShape = &msoSeal8; break;
        case mso_sptSeal16 :                    pAutoShape = &msoSeal16; break;
        case mso_sptSeal24 :                    pAutoShape = &msoSeal24; break;
        case mso_sptSeal32 :                    pAutoShape = &msoSeal32; break;
        case mso_sptRibbon2 :                   pAutoShape = &msoRibbon2; break;
        case mso_sptRibbon :                    pAutoShape = &msoRibbon2; break;
        case mso_sptEllipseRibbon2 :            pAutoShape = &msoRibbon2; break;    //!!!!!
        case mso_sptEllipseRibbon :             pAutoShape = &msoRibbon2; break; //!!!!!
        case mso_sptVerticalScroll :            pAutoShape = &msoVerticalScroll;    break;
        case mso_sptHorizontalScroll :          pAutoShape = &msoHorizontalScroll; break;
        case mso_sptFlowChartProcess :          pAutoShape = &msoFlowChartProcess; break;
        case mso_sptFlowChartAlternateProcess : pAutoShape = &msoFlowChartAlternateProcess; break;
        case mso_sptFlowChartDecision :         pAutoShape = &msoFlowChartDecision; break;
        case mso_sptFlowChartInputOutput :      pAutoShape = &msoFlowChartInputOutput; break;
        case mso_sptFlowChartPredefinedProcess :pAutoShape = &msoFlowChartPredefinedProcess; break;
        case mso_sptFlowChartInternalStorage :  pAutoShape = &msoFlowChartInternalStorage; break;
        case mso_sptFlowChartDocument :         pAutoShape = &msoFlowChartDocument; break;
        case mso_sptFlowChartMultidocument :    pAutoShape = &msoFlowChartMultidocument; break;
        case mso_sptFlowChartTerminator :       pAutoShape = &msoFlowChartTerminator; break;
        case mso_sptFlowChartPreparation :      pAutoShape = &msoFlowChartPreparation; break;
        case mso_sptFlowChartManualInput :      pAutoShape = &msoFlowChartManualInput; break;
        case mso_sptFlowChartManualOperation :  pAutoShape = &msoFlowChartManualOperation; break;
        case mso_sptFlowChartConnector :        pAutoShape = &msoFlowChartConnector; break;
        case mso_sptFlowChartOffpageConnector : pAutoShape = &msoFlowChartOffpageConnector; break;
        case mso_sptFlowChartPunchedCard :      pAutoShape = &msoFlowChartPunchedCard; break;
        case mso_sptFlowChartPunchedTape :      pAutoShape = &msoFlowChartPunchedTape; break;
        case mso_sptFlowChartSummingJunction :  pAutoShape = &msoFlowChartSummingJunction; break;
        case mso_sptFlowChartOr :               pAutoShape = &msoFlowChartOr; break;
        case mso_sptFlowChartCollate :          pAutoShape = &msoFlowChartCollate; break;
        case mso_sptFlowChartSort :             pAutoShape = &msoFlowChartSort; break;
        case mso_sptFlowChartExtract :          pAutoShape = &msoFlowChartExtract; break;
        case mso_sptFlowChartMerge :            pAutoShape = &msoFlowChartMerge; break;
        case mso_sptFlowChartOnlineStorage :    pAutoShape = &msoFlowChartOnlineStorage; break;
        case mso_sptFlowChartDelay :            pAutoShape = &msoFlowChartDelay; break;
        case mso_sptFlowChartMagneticTape :     pAutoShape = &msoFlowChartMagneticTape; break;
        case mso_sptFlowChartMagneticDisk :     pAutoShape = &msoFlowChartMagneticDisk; break;
        case mso_sptFlowChartMagneticDrum :     pAutoShape = &msoFlowChartMagneticDrum; break;
        case mso_sptFlowChartDisplay :          pAutoShape = &msoFlowChartDisplay; break;
        case mso_sptWedgeRectCallout :          pAutoShape = &msoWedgeRectCallout; break;
        case mso_sptWedgeRRectCallout :         pAutoShape = &msoWedgeRRectCallout; break;
        case mso_sptWedgeEllipseCallout :       pAutoShape = &msoWedgeEllipseCallout; break;
        case mso_sptCloudCallout :              pAutoShape = &msoCloudCallout; break;
//      case mso_sptWave :                      pAutoShape = &msoWave; break;
        break;
        default :
        break;
    }
    return pAutoShape;
}

SvxMSDffAutoShape::~SvxMSDffAutoShape()
{
    if ( bVertAlloc )
        delete[] pVertData;
    if ( bSegAlloc )
        delete[] pSegData;
    if ( bTextRectAlloc )
        delete[] pTextRectData;
    if ( bCalcAlloc )
        delete[] pCalculationData;
    delete[] pAdjustmentHandles;
}

SvxMSDffAutoShape::SvxMSDffAutoShape( const DffPropertyReader& rPropReader, SvStream& rSt,
                                        DffObjData& rData, Rectangle& rSnapRect, sal_Int32 nAngle ) :
    eSpType             ( rData.eShapeType ),
    aSnapRect           ( rSnapRect ),
    nFix16Angle         ( nAngle ),
    nXRef               ( 0x80000000 ),
    nYRef               ( 0x80000000 ),
    nFlags              ( 0 ),
    nColorData          ( 0 ),
    nCalculationData    ( 0 ),
    nTextRectData       ( 0 ),
    nAdjustmentHandles  ( 0 ),
    nGluePoints         ( 0 ),
    pAdjustmentHandles  ( NULL ),
    pVertData           ( NULL ),
    pSegData            ( NULL ),
    pCalculationData    ( NULL ),
    pTextRectData       ( NULL ),
    pGluePoints         ( NULL ),
    bIsEmpty            ( TRUE ),
    bVertAlloc          ( FALSE ),
    bSegAlloc           ( FALSE ),
    bCalcAlloc          ( FALSE ),
    bTextRectAlloc      ( FALSE ),
    bTextFlow           ( ( (MSO_TextFlow)rPropReader.GetPropertyValue( DFF_Prop_txflTextFlow ) ) == mso_txflTtoBA ),
    bFilled             ( ( rPropReader.GetPropertyValue( DFF_Prop_fNoFillHitTest ) & 0x10 ) != 0 ),    // pie <-> arc
    bFlipH              ( ( rData.nSpFlags & SP_FFLIPH ) != 0 ),
    bFlipV              ( ( rData.nSpFlags & SP_FFLIPV ) != 0 )
{
    const sal_Int32*        pDefData = NULL;
    const mso_AutoShape*    pDefAutoShape = NULL;


    // cloning DFF_Prop_adjustValues from DffPropertyReader to be able
    // to manipulate them and not destroying the original properties

    sal_uInt32              nHandles[ 10 ];
    sal_uInt32              nHandlesSet = 0;
    sal_uInt32              i, nHandlesUsed = 0;

    for ( i = DFF_Prop_adjustValue; i < DFF_Prop_adjust10Value; i++ )
    {
        if ( rPropReader.IsProperty( i ) )
        {
            sal_Int32 nIndex = i - DFF_Prop_adjustValue;
            nHandles[ nIndex ] = rPropReader.GetPropertyValue( i );
            nHandlesSet |= 1 << nIndex;
            nHandlesUsed = nIndex + 1;
        }
    }
    pDefAutoShape = GetAutoShapeContent( eSpType );
    if ( pDefAutoShape )
    {
        switch( eSpType )
        {
            case mso_sptCan :                       nColorData = 0x20200000; break;
            case mso_sptCube :                      nColorData = 0x302d0000; break;
            case mso_sptActionButtonBlank :         nColorData = 0x502ad400; break;
            case mso_sptActionButtonHome :          nColorData = 0x702ad4ad; break;
            case mso_sptActionButtonHelp :          nColorData = 0x602ad4a0; break;
            case mso_sptActionButtonInformation :   nColorData = 0x702ad4a5; break;
            case mso_sptActionButtonBackPrevious :  nColorData = 0x602ad4a0; break;
            case mso_sptActionButtonForwardNext :   nColorData = 0x602ad4a0; break;
            case mso_sptActionButtonBeginning :     nColorData = 0x602ad4a0; break;
            case mso_sptActionButtonEnd :           nColorData = 0x602ad4a0; break;
            case mso_sptActionButtonReturn :        nColorData = 0x602ad4a0; break;
            case mso_sptActionButtonDocument :      nColorData = 0x702ad4da; break;
            case mso_sptActionButtonSound :         nColorData = 0x602ad4a0; break;
            case mso_sptActionButtonMovie :         nColorData = 0x602ad4a0; break;
            case mso_sptBevel :                     nColorData = 0x502ad400; break;
            case mso_sptFoldedCorner :              nColorData = 0x20d00000; break;
            case mso_sptSmileyFace :                nColorData = 0x20d00000; break;
            case mso_sptCurvedLeftArrow :
            {
                if ( nHandlesSet & 4 )
                    nHandles[ 2 ] = 21600 - nHandles[ 2 ];
                nFlags |= DFF_AUTOSHAPE_FLIP_H;
            }
            break;
            case mso_sptCurvedUpArrow :
            {
                if ( nHandlesSet & 4 )
                    nHandles[ 2 ] = 21600 - nHandles[ 2 ];
                nFlags |= DFF_AUTOSHAPE_FLIP_V | DFF_AUTOSHAPE_EXCH;
            }
            break;
            case mso_sptCurvedDownArrow :           nFlags |= DFF_AUTOSHAPE_EXCH; break;
            case mso_sptRibbon2 :                   nColorData = 0x30dd0000; break;
            case mso_sptRibbon :
            {
                if ( nHandlesSet & 2 )
                    nHandles[ 1 ] = 21600 - nHandles[ 1 ];
                nFlags |= DFF_AUTOSHAPE_FLIP_V;
                nColorData = 0x30dd0000;
            }
            break;
            case mso_sptEllipseRibbon2 :            nColorData = 0x30dd0000; break;
            case mso_sptEllipseRibbon :             // !!!!!!!!!!
            {
                if ( nHandlesSet & 2 )
                    nHandles[ 1 ] = 21600 - nHandles[ 1 ];
                nFlags |= DFF_AUTOSHAPE_FLIP_V;
                nColorData = 0x30dd0000;
            }
            break;
            case mso_sptVerticalScroll :            nColorData = 0x30dd0000; break;
            case mso_sptHorizontalScroll :          nColorData = 0x30dd0000; break;
        }
    }
    nCoordHeight = nCoordWidth  = 21600;
    if ( pDefAutoShape )
    {
        bIsEmpty = FALSE;
        nNumElemVert = pDefAutoShape->nVertices;
        pVertData = pDefAutoShape->pVertices;
        nNumElemSeg = pDefAutoShape->nElements;
        pSegData = pDefAutoShape->pElements;
        nCalculationData = pDefAutoShape->nCalculation;
        pCalculationData = pDefAutoShape->pCalculation;
        pDefData = pDefAutoShape->pDefData;
        pTextRectData = pDefAutoShape->pTextRect;
        nTextRectData = pDefAutoShape->nTextRect;
        nCoordWidth = pDefAutoShape->nCoordWidth;
        nCoordHeight = pDefAutoShape->nCoordHeight;
        nXRef = pDefAutoShape->nXRef;
        nYRef = pDefAutoShape->nYRef;
        pGluePoints = pDefAutoShape->pGluePoints;
        nGluePoints = pDefAutoShape->nGluePoints;
    }
    sal_Int32 nGeoRight = rPropReader.GetPropertyValue( DFF_Prop_geoRight, 0 );
    if ( nGeoRight )
    {
        nGeoRight -= rPropReader.GetPropertyValue( DFF_Prop_geoLeft, 0 );
        nCoordWidth = labs( nGeoRight );
    }
    sal_Int32 nGeoBottom = rPropReader.GetPropertyValue( DFF_Prop_geoBottom, 0 );
    if ( nGeoBottom )
    {
        nGeoBottom -= rPropReader.GetPropertyValue( DFF_Prop_geoTop, 0 );
        nCoordHeight = labs( nGeoBottom );
    }

    if ( rPropReader.SeekToContent( DFF_Prop_pVertices, rSt ) )
    {
        sal_uInt16 nTmp16, nNumElemMemVert, nElemSizeVert;
        rSt >> nTmp16 >> nNumElemMemVert >> nElemSizeVert;
        if ( nTmp16 )
        {
            bIsEmpty = FALSE;
            nNumElemVert = nTmp16;
            sal_uInt32 i = nNumElemVert;
            bVertAlloc = TRUE;
            pVertData = new SvxMSDffVertPair[ nNumElemVert ];
            SvxMSDffVertPair* pTmp = pVertData;
            if ( nElemSizeVert == 8 )
            {
                while( i-- )
                {
                    rSt >> pTmp->nValA
                        >> pTmp->nValB;
                    pTmp++;
                }
            }
            else
            {
                sal_Int16 nTmpA, nTmpB;
                while ( i-- )
                {
                    rSt >> nTmpA
                        >> nTmpB;

                    pTmp->nValA = nTmpA;
                    pTmp->nValB = nTmpB;
                    pTmp++;
                }
            }
        }
    }
    if ( !bIsEmpty )    // we can import an autoshape if either pVertData or pDefAutoShape is set
    {
        if ( rPropReader.SeekToContent( DFF_Prop_pSegmentInfo, rSt ) )
        {
            sal_uInt16 nTmp16, nNumElemMemSeg, nElemSizeSeg;
            rSt >> nTmp16 >> nNumElemMemSeg >> nElemSizeSeg;
            if ( nTmp16 )
            {
                nNumElemSeg = nTmp16;
                bSegAlloc = TRUE;
                pSegData = new sal_uInt16[ nNumElemSeg + 1 ];   // #97948# allocate one more element,
#ifdef __BIGENDIAN                                              // so it won't be difficult to append
                sal_uInt32 i = nNumElemSeg;                     // a missing end segment action
                sal_uInt16* pTmp = pSegData;
                while( i-- )
                {
                    rSt >> *pTmp++;
                }
#else
                rSt.Read( pSegData, nNumElemSeg << 1 );
#endif
                if ( pSegData[ nNumElemSeg - 1 ] != 0x8000 )    // #97948# append the missing segm action
                    pSegData[ nNumElemSeg++ ] = 0x8000;
            }
        }
        if ( rPropReader.SeekToContent( 342, rSt ) )
        {
            sal_uInt16 nTmp16, nNumElemMem, nElemSize;
            rSt >> nTmp16 >> nNumElemMem >> nElemSize;
            if ( nTmp16 && ( nElemSize == 8 ) )
            {
                nCalculationData = nTmp16;
                pCalculationData = new SvxMSDffCalculationData[ nCalculationData ];
                bCalcAlloc = TRUE;
                sal_uInt32 i;
                sal_uInt16 nVal0, nVal1, nVal2;
                for ( i = 0; i < nCalculationData; i++ )
                {
                    SvxMSDffCalculationData& rDat = pCalculationData[ i ];
                    rSt >> rDat.nFlags
                        >> nVal0 >> nVal1 >> nVal2;
                    rDat.nVal[ 0 ] = nVal0;
                    rDat.nVal[ 1 ] = nVal1;
                    rDat.nVal[ 2 ] = nVal2;
                }
            }
        }

        if ( rPropReader.SeekToContent( 343, rSt ) )
        {
            sal_uInt16 nTmp16, nNumElemMem, nElemSize;
            rSt >> nTmp16 >> nNumElemMem >> nElemSize;
            if ( nTmp16 && ( nElemSize == 16 ) )
            {
                sal_uInt32 nNumElem = nTmp16;
                if ( nNumElem > 1 )
                    nNumElem = 2;
                bTextRectAlloc = sal_True;
                nTextRectData = nNumElem;
                pTextRectData = new SvxMSDffTextRectangles[ nNumElem ];
                SvxMSDffTextRectangles* pTmp = pTextRectData;
                for ( i = 0; i < nNumElem; i++ )
                {
                    rSt >> pTmp->nPairA.nValA
                        >> pTmp->nPairA.nValB
                        >> pTmp->nPairB.nValA
                        >> pTmp->nPairB.nValB;
                    pTmp++;
                }
            }
        }

        const sal_Int32* pTmp = pDefData;
        sal_uInt32 nDefaults = 0;
        if ( pTmp )
            nDefaults = nAdjustmentHandles = *pTmp++;

        if ( nHandlesUsed > nAdjustmentHandles )
            nAdjustmentHandles = nHandlesUsed;

        if ( nAdjustmentHandles )
        {
            pAdjustmentHandles = new SvxMSDffAdjustmentHandle[ nAdjustmentHandles ];
            for ( i = 0; i < nAdjustmentHandles; i++ )
            {
                if ( i < nDefaults )
                    pAdjustmentHandles[ i ].SetDefaultValue( *pTmp++ );
                if ( nHandlesSet & ( 1 << i ) )
                    pAdjustmentHandles[ i ].SetAdjustValue( nHandles[ i ] );
            }
        }

        fXScale = (double)aSnapRect.GetWidth() / (double)nCoordWidth;
        fYScale = (double)aSnapRect.GetHeight() / (double)nCoordHeight;

        if ( rPropReader.IsProperty( 339 ) )
            nXRef = rPropReader.GetPropertyValue( 339 );
        if ( rPropReader.IsProperty( 340 ) )
            nYRef = rPropReader.GetPropertyValue( 340 );
    }
}

sal_Int32 SvxMSDffAutoShape::Fix16ToAngle( sal_Int32 nAngle ) const
{
    if ( nAngle )
    {
        nAngle = ( (sal_Int16)( nAngle >> 16) * 100L ) + ( ( ( nAngle & 0x0000ffff) * 100L ) >> 16 );
        nAngle = NormAngle360( -nAngle );
    }
    return nAngle;
}

sal_Int32 SvxMSDffAutoShape::GetAdjustValue( sal_uInt32 nIndex ) const
{
    return ( nIndex < nAdjustmentHandles ) ? pAdjustmentHandles[ nIndex ].GetAdjustValue() : 0;
}

sal_Int32 SvxMSDffAutoShape::GetAdjustValue( sal_uInt32 nIndex, sal_Int32 nDefault ) const
{
    if ( ( nIndex >= nAdjustmentHandles ) || pAdjustmentHandles[ nIndex ].IsDefault() )
        return nDefault;
    else
        return pAdjustmentHandles[ nIndex ].GetAdjustValue();
}

Point SvxMSDffAutoShape::GetPoint( const SvxMSDffVertPair& rPair, sal_Bool bScale ) const
{
    Point       aRetValue;
    sal_Bool    bExchange = ( nFlags & DFF_AUTOSHAPE_EXCH ) != 0;   // x <-> y
    sal_uInt32  nPass = 0;
    do
    {
        sal_uInt32  nIndex = nPass;

        if ( bExchange )
            nIndex ^= 1;

        sal_uInt32  nDat = nIndex ? (sal_uInt32)rPair.nValB : (sal_uInt32)rPair.nValA;
        sal_Bool    bScaleWidth = nPass == 0;

        double fVal;
        sal_uInt32  nGeometryFlags = 0;
        if ( ( nDat >> 16 ) == 0x8000 )
            fVal = ImplGetValue( (sal_uInt16)nDat, nGeometryFlags );
        else
            fVal = (sal_Int32)nDat;

        if ( bScale )
        {
            if ( nGeometryFlags & bExchange )   // left <-> top, right <-> bottom
            {
                nGeometryFlags = ( ( nGeometryFlags & 1 ) << 1 ) | ( ( nGeometryFlags & 2 ) >> 1 ) |
                                    ( ( nGeometryFlags & 4 ) << 1 ) | ( ( nGeometryFlags & 8 ) >> 1 );
            }
            if ( bScaleWidth )
            {
                if ( ( aSnapRect.GetWidth() > aSnapRect.GetHeight() ) && ( ( nXRef != 0x80000000 ) || nGeometryFlags ) )
                {
                    sal_Bool bGeo = ( ( ( nGeometryFlags & GEOMETRY_USED_LEFT ) == 0 ) && ( fVal > nXRef ) )
                                        || ( ( nGeometryFlags & GEOMETRY_USED_RIGHT ) != 0 );
                    if ( ( nGeometryFlags & ( GEOMETRY_USED_LEFT | GEOMETRY_USED_RIGHT ) ) == ( GEOMETRY_USED_LEFT | GEOMETRY_USED_RIGHT ) )
                    {
                        fVal -= (double)nCoordWidth * 0.5;
                        fVal *= fYScale;
                        fVal += (double)aSnapRect.GetWidth() * 0.5;
                    }
                    else
                    {
                        fVal *= fYScale;
                        if ( bGeo )
                            fVal += (double)nCoordWidth * fXScale - (double)nCoordWidth * fYScale;
                    }
                }
                else
                    fVal *= fXScale;
                if ( nFlags & DFF_AUTOSHAPE_FLIP_H )
                    fVal = aSnapRect.GetWidth() - fVal;
            }
            else
            {
                if ( ( aSnapRect.GetHeight() > aSnapRect.GetWidth() ) && ( ( nYRef != 0x80000000 ) || nGeometryFlags ) )
                {
                    sal_Bool bGeo = ( ( ( nGeometryFlags & GEOMETRY_USED_TOP ) == 0 ) && ( fVal > nYRef ) )
                                        || ( ( nGeometryFlags & GEOMETRY_USED_BOTTOM ) != 0 );
                    if ( ( nGeometryFlags & ( GEOMETRY_USED_TOP | GEOMETRY_USED_BOTTOM ) ) == ( GEOMETRY_USED_TOP | GEOMETRY_USED_BOTTOM ) )
                    {
                        fVal -= (double)nCoordHeight * 0.5;
                        fVal *= fXScale;
                        fVal += (double)aSnapRect.GetHeight() * 0.5;
                    }
                    else
                    {
                        fVal *= fXScale;
                        if ( bGeo )
                            fVal += (double)nCoordHeight * fYScale - (double)nCoordHeight * fXScale;
                    }
                }
                else
                    fVal *= fYScale;
                if ( nFlags & DFF_AUTOSHAPE_FLIP_V )
                    fVal = aSnapRect.GetHeight() - fVal;
            }
        }
        if ( nPass )
            aRetValue.Y() = (sal_Int32)fVal;
        else
            aRetValue.X() = (sal_Int32)fVal;
    }
    while ( ++nPass < 2 );
    return aRetValue;
}

double SvxMSDffAutoShape::ImplGetValue( sal_uInt16 nIndex, sal_uInt32& nGeometryFlags ) const
{
    if ( !pCalculationData )
        return 0;
    if ( nCalculationData <= nIndex )
        return 0;

    double fVal[ 3 ];
    sal_Int16 i, nFlags = pCalculationData[ nIndex ].nFlags;
    for ( i = 0; i < 3; i++ )
    {
        if ( nFlags & ( 0x2000 << i ) )
        {
            sal_Int16 nVal = pCalculationData[ nIndex ].nVal[ i ];
            if ( nVal & 0x400 )
                fVal[ i ] = ImplGetValue( nVal & 0xff, nGeometryFlags );
            else
            {
                switch ( nVal )
                {
                    case DFF_Prop_adjustValue :
                    case DFF_Prop_adjust2Value :
                    case DFF_Prop_adjust3Value :
                    case DFF_Prop_adjust4Value :
                    case DFF_Prop_adjust5Value :
                    case DFF_Prop_adjust6Value :
                    case DFF_Prop_adjust7Value :
                    case DFF_Prop_adjust8Value :
                    case DFF_Prop_adjust9Value :
                    case DFF_Prop_adjust10Value :
                        fVal[ i ] = GetAdjustValue( nVal - DFF_Prop_adjustValue );
                    break;
                    case DFF_Prop_geoLeft :
                    {
                        nGeometryFlags |= GEOMETRY_USED_LEFT;
                        fVal[ i ]  = 0.0;
                    }
                    break;
                    case DFF_Prop_geoTop :
                    {
                        nGeometryFlags |= GEOMETRY_USED_TOP;
                        fVal[ i ]  = 0.0;
                    }
                    break;
                    case DFF_Prop_geoRight :
                    {
                        nGeometryFlags |= GEOMETRY_USED_RIGHT;
                        fVal[ i ] = nCoordWidth;
                    }
                    break;
                    case DFF_Prop_geoBottom :
                    {
                        nGeometryFlags |= GEOMETRY_USED_BOTTOM;
                        fVal[ i ] = nCoordWidth;
                    }
                    break;
                    default:
                        fVal[ i ]  = 0.0;
                    break;
                }
            }
        }
        else
            fVal[ i ] = pCalculationData[ nIndex ].nVal[ i ];
    }

    switch ( nFlags & 0xff )
    {
        case 0 :                                    // sum
        {
            fVal[ 0 ] += fVal[ 1 ];
            fVal[ 0 ] -= fVal[ 2 ];
        }
        break;

        case 1 :                                    // product
        {
            if ( fVal[ 1 ] != 0.0 )
                fVal[ 0 ] *= fVal[ 1 ];
            if ( fVal[ 2 ] != 0.0 )
                fVal[ 0 ] /= fVal[ 2 ];
        }
        break;

        case 2 :                                    // mid
        {
            fVal[ 0 ] += fVal[ 1 ];
            fVal[ 0 ] /= 2.0;
        }
        break;

        case 3 :                                    // abs
        {
            fVal[ 0 ] = fabs( fVal[ 0 ] );
        }
        break;

        case 4 :                                    // min
        {
            if ( fVal[ 1 ] < fVal[ 0 ] )
                fVal[ 0 ] = fVal[ 1 ];
        }
        break;

        case 5 :
        {
            if ( fVal[ 1 ] > fVal[ 0 ] )            // max
                fVal[ 0 ] = fVal[ 1 ];
        }
        break;

        case 6 :                                    // if > 0 ? a : b
        {
            if ( fVal[ 0 ] > 0.0 )
                fVal[ 0 ] = fVal[ 1 ];
            else
                fVal[ 0 ] = fVal[ 2 ];
        }
        break;

        case 7 :                                    // mod
        {
            fVal[ 0 ] = sqrt( fVal[ 0 ] * fVal[ 0 ] + fVal[ 1 ] * fVal[ 1 ]
                            + fVal[ 2 ] * fVal[ 2 ] );
        }
        break;

        case 8 :                                    // atan2
        {
            fVal[ 0 ] = atan2( fVal[ 1 ], fVal[ 0 ] ) / F_PI180;
            fVal[ 0 ] *= 65536.0;
        }
        break;

        case 9 :    // in this special case the second parameter is a fixed fload
        {           // which has to be divided by 0x10000
            fVal[ 0 ] *= sin( ( fVal[ 1 ] / 65536 ) * F_PI180 );
        }
        break;

        case 0xa :  // in this special case the second parameter is a fixed fload
        {           // which has to be divided by 0x10000
            fVal[ 0 ] *= cos( ( fVal[ 1 ] / 65536 ) * F_PI180 );
        }
        break;

        case 0xb :                                  // cosatan2
        {
            fVal[ 0 ] *= cos( atan2( fVal[ 2 ], fVal[ 1 ] ) );
        }
        break;

        case 0xc :                                  // sinatan2
        {
            fVal[ 0 ] *= sin( atan2( fVal[ 2 ], fVal[ 1 ] ) );
        }
        break;

        case 0xd :                                  // sqrt
        {
            fVal[ 0 ] = sqrt( fVal[ 0 ] );
        }
        break;

        case 0xe :                                  // sumangle
        {
            fVal[ 0 ] += fVal[ 1 ] * 65536.0;
            fVal[ 0 ] -= fVal[ 2 ] * 65536.0;
        }
        break;

        case 0xf :                                  // ellipse
        {
            if ( fVal[ 1 ] != 0.0 )
            {
                fVal[ 0 ] /= fVal[ 1 ];
                fVal[ 0 ] = fVal[ 2 ] * sqrt( 1 - fVal[ 0 ] * fVal[ 0 ] );
            }
        }
        break;

        case 0x10 :                                 // tan
        {
            fVal[ 0 ] *= tan( fVal[ 1 ] );
        }
        break;

        case 0x80 :
        {
            // fVal[0]^2 + fVal[1]^2 = fVal[2]^2
            if ( fVal[ 2 ] == 0.0 )
                fVal[ 0 ] = sqrt( fVal[ 0 ] * fVal[ 0 ] + fVal[ 1 ] * fVal[ 1 ] );
            else
            {
                double fA = fVal[ 0 ] != 0.0 ? fVal[ 0 ] : fVal[ 1 ];
                fVal[ 0 ] = sqrt( fVal[ 2 ] * fVal[ 2 ] - fA * fA );
            }
        }
        break;
        case 0x81 :
        {
            double fAngle = F_PI1800 * fVal[ 2 ];
            fVal[ 0 ] = ( cos( fAngle ) * ( fVal[ 0 ] - 10800 )
                            + sin( fAngle ) * ( fVal[ 1 ] - 10800 ) ) + 10800;
        }
        break;
        case 0x82 :
        {
            double fAngle = F_PI1800 * fVal[ 2 ];
            fVal[ 0 ] = - ( sin( fAngle ) * ( fVal[ 0 ] - 10800 )
                            - cos( fAngle ) * ( fVal[ 1 ] - 10800 ) ) + 10800;
        }
        break;
        default :
        {
            sal_Bool bStop = sal_True;
        }
        break;
    }
    return fVal[ 0 ];
}

// nLumDat 28-31 = number of luminance entries in nLumDat
// nLumDat 27-24 = nLumDatEntry 0
// nLumDat 23-20 = nLumDatEntry 1 ...
// each 4bit entry is to be interpreted as a 10 percent signed luminance changing
Color SvxMSDffAutoShape::ImplGetColorData( const Color& rFillColor, sal_uInt32 nIndex )
{
    Color aRetColor;

    sal_uInt32 i, nColor, nTmp, nCount = nColorData >> 28;

    if ( nCount )
    {
        if ( nIndex >= nCount )
            nIndex = nCount - 1;

        sal_uInt32 nFillColor = (sal_uInt32)rFillColor.GetRed() |
                                    ((sal_uInt32)rFillColor.GetGreen() << 8 ) |
                                        ((sal_uInt32)rFillColor.GetBlue() << 16 );

        sal_Int32 nLumDat = nColorData << ( ( 1 + nIndex ) << 2 );
        sal_Int32 nLuminance = ( nLumDat >> 28 ) * 12;

        nTmp = nFillColor;
        nColor = 0;
        for ( i = 0; i < 3; i++ )
        {
            sal_Int32 nC = (sal_uInt8)nTmp;
            nTmp >>= 8;
            nC += ( ( nLuminance * nC ) >> 8 );
            if ( nC < 0 )
                nC = 0;
            else if ( nC &~ 0xff )
                nC = 0xff;
            nColor >>= 8;
            nColor |= nC << 16;
        }
        aRetColor = Color( (sal_uInt8)nColor, (sal_uInt8)( nColor >> 8 ), (sal_uInt8)( nColor >> 16 ) );
    }
    return aRetColor;
}

sal_Bool SvxMSDffAutoShape::IsEmpty() const
{
    return bIsEmpty;
}

Rectangle SvxMSDffAutoShape::GetTextRect() const
{
    if ( !nTextRectData )
        return aSnapRect;

    sal_uInt32 nIndex = 0;
    if ( bTextFlow && ( nTextRectData > 1 ) )
        nIndex++;

    Point aTopLeft( GetPoint( pTextRectData[ nIndex ].nPairA, sal_True ) );
    Point aBottomRight( GetPoint( pTextRectData[ nIndex ].nPairB, sal_True ) );
    Rectangle aRect( aTopLeft, aBottomRight );
    aRect.Move( aSnapRect.Left(), aSnapRect.Top() );
    if ( bFlipH )
    {
        sal_Int32 nXDist = aSnapRect.Right() - aRect.Right();
        aRect = Rectangle( Point( nXDist + aSnapRect.Left(), aRect.Top() ), aRect.GetSize() );
    }
    if ( bFlipV )
    {
        sal_Int32 nYDist = aSnapRect.Bottom() - aRect.Bottom();
        aRect = Rectangle( Point( aRect.Left(), nYDist + aSnapRect.Top() ), aRect.GetSize() );
    }
    return aRect;
}

sal_Bool SvxMSDffAutoShape::HasGluePointList( const MSO_SPT eShapeType )
{
    const mso_AutoShape* pAutoShape = GetAutoShapeContent( eShapeType );
    return pAutoShape && pAutoShape->nGluePoints;
}

MSO_SPT SvxMSDffAutoShape::GetShapeTypeFromSdrObject(  const SdrObject* pObj )
{
    MSO_SPT eShapeType = mso_sptNil;
    const SfxPoolItem* pAdjustItem = NULL;
    const SfxItemSet&  rItemSet = pObj->GetItemSet();
    rItemSet.GetItemState( SDRATTR_AUTOSHAPE_ADJUSTMENT, FALSE, &pAdjustItem );
    if ( pAdjustItem )
    {
        SdrAutoShapeAdjustmentItem& rAdjustItem = *(SdrAutoShapeAdjustmentItem*)pAdjustItem;
        sal_uInt32 nCount = rAdjustItem.GetCount();
        if ( nCount >= 3 )
        {
            /* checking magic number, so we can get sure that the SdrObject was a autoshape
               and we can get the autoshape type */
            if ( rAdjustItem.GetValue( nCount - 1 ).GetValue() == 0x80001234 )
                eShapeType = (MSO_SPT)((sal_uInt32)(rAdjustItem.GetValue( nCount - 2 ).GetValue()) >> 16);
        }
    }
    return eShapeType;
}

void SvxMSDffAutoShape::SwapStartAndEndArrow( SdrObject* pObj ) //#108274
{
    XLineStartItem       aLineStart;
    aLineStart.SetValue(((XLineStartItem&)pObj->GetItem( XATTR_LINEEND )).GetValue());
    XLineStartWidthItem  aLineStartWidth(((XLineStartWidthItem&)pObj->GetItem( XATTR_LINEENDWIDTH )).GetValue());
    XLineStartCenterItem aLineStartCenter(((XLineStartCenterItem&)pObj->GetItem( XATTR_LINEENDCENTER )).GetValue());

    XLineEndItem         aLineEnd;
    aLineEnd.SetValue(((XLineEndItem&)pObj->GetItem( XATTR_LINESTART )).GetValue());
    XLineEndWidthItem    aLineEndWidth(((XLineEndWidthItem&)pObj->GetItem( XATTR_LINESTARTWIDTH )).GetValue());
    XLineEndCenterItem   aLineEndCenter(((XLineEndCenterItem&)pObj->GetItem( XATTR_LINESTARTCENTER )).GetValue());

    pObj->SetItem( aLineStart );
    pObj->SetItem( aLineStartWidth );
    pObj->SetItem( aLineStartCenter );
    pObj->SetItem( aLineEnd );
    pObj->SetItem( aLineEndWidth );
    pObj->SetItem( aLineEndCenter );
}

SdrObject* SvxMSDffAutoShape::GetObject( SdrModel* pSdrModel, SfxItemSet& rSet, sal_Bool bSetAutoShapeAdjustItem )
{
    SdrObject* pRet = NULL;

    if ( !IsEmpty() )
    {
        if ( eSpType == mso_sptRectangle )
        {
            pRet = new SdrRectObj( aSnapRect );
            pRet->SetModel( pSdrModel );
            pRet->SetItemSet(rSet);
        }
        else if ( eSpType == mso_sptRoundRectangle )
        {
            sal_Int32 nW = aSnapRect.Right() - aSnapRect.Left();
            sal_Int32 nH = aSnapRect.Bottom() - aSnapRect.Top();
            if ( nH < nW )
                nW = nH;
            double fAdjust = (double)GetAdjustValue( 0, 3600 ) / 21600.0;
            nW = (sal_Int32)( (double)nW * fAdjust );
            rSet.Put( SdrEckenradiusItem( nW ) );
            pRet = new SdrRectObj( aSnapRect );
            pRet->SetModel( pSdrModel );
            pRet->SetItemSet(rSet);
        }
        else if ( eSpType == mso_sptEllipse )
        {
            pRet = new SdrCircObj( OBJ_CIRC, aSnapRect );
            pRet->SetModel( pSdrModel );
            pRet->SetItemSet(rSet);
        }
        else if ( eSpType == mso_sptArc )
        {   // the arc is something special, because sometimes the snaprect does not match
            Rectangle aPolyBoundRect;
            if ( nNumElemVert )
            {
                XPolygon aXP( (sal_uInt16)nNumElemVert );
                const SvxMSDffVertPair* pTmp = pVertData;
                sal_uInt32 nPtNum;
                for ( nPtNum = 0; nPtNum < nNumElemVert; nPtNum++ )
                    aXP[ (sal_uInt16)nPtNum ] = GetPoint( *pTmp++, sal_False );
                aPolyBoundRect = Rectangle( aXP.GetBoundRect() );
            }
            else
                aPolyBoundRect = aSnapRect;

            sal_Int32   nEndAngle = Fix16ToAngle( GetAdjustValue( 0 ) );
            sal_Int32   nStartAngle = Fix16ToAngle( GetAdjustValue( 1 ) );

            if ( nStartAngle == nEndAngle )
                return NULL;

            if ( bFilled )      // ( filled ) ? we have to import an pie : we have to construct an arc
            {
                pRet = new SdrCircObj( OBJ_SECT, aPolyBoundRect, nStartAngle, nEndAngle );
                pRet->NbcSetSnapRect( aSnapRect );
                pRet->SetModel( pSdrModel );
                pRet->SetItemSet(rSet);
            }
            else
            {
                Point aStart, aEnd, aCenter( aPolyBoundRect.Center() );
                aStart.X() = (sal_Int32)( ( cos( ( (double)nStartAngle * F_PI18000 ) ) * 1000.0 ) );
                aStart.Y() = - (sal_Int32)( ( sin( ( (double)nStartAngle * F_PI18000 ) ) * 1000.0 ) );
                aEnd.X() = (sal_Int32)( ( cos( ( (double)nEndAngle * F_PI18000 ) ) * 1000.0 ) );
                aEnd.Y() = - (sal_Int32)( ( sin( ( (double)nEndAngle * F_PI18000 ) ) * 1000.0 ) );
                aStart.X() += aCenter.X();
                aStart.Y() += aCenter.Y();
                aEnd.X() += aCenter.X();
                aEnd.Y() += aCenter.Y();

                Polygon aPolygon( aPolyBoundRect, aStart, aEnd, POLY_PIE );
                Rectangle aPolyPieRect( aPolygon.GetBoundRect() );

                USHORT nPt = aPolygon.GetSize();

                if ( nPt < 4 )
                    return NULL;

                aPolygon[ 0 ] = aPolygon[ 1 ];                              // try to get the arc boundrect
                aPolygon[ nPt - 1 ] = aPolygon[ nPt - 2 ];
                Rectangle aPolyArcRect( aPolygon.GetBoundRect() );

                double  fYScale, fXScale;
                double  fYOfs, fXOfs;
                int     nCond;

                fYOfs = fXOfs = 0.0;
                if ( aPolyPieRect.GetWidth() != aPolyArcRect.GetWidth() )
                {
                    nCond = ( (sal_uInt32)( nStartAngle - 9000 ) > 18000 ) && ( (sal_uInt32)( nEndAngle - 9000 ) > 18000 ) ? 1 : 0;
                    nCond ^= bFlipH ? 1 : 0;
                    if ( nCond )
                    {
                        fXScale = (double)aSnapRect.GetWidth() / (double)aPolyPieRect.GetWidth();
                        fXOfs = ( (double)aPolyPieRect.GetWidth() - (double)aPolyArcRect.GetWidth() ) * fXScale;
                    }
                }
                if ( aPolyPieRect.GetHeight() != aPolyArcRect.GetHeight() )
                {
                    nCond = ( ( nStartAngle > 18000 ) && ( nEndAngle > 18000 ) ) ? 1 : 0;
                    nCond ^= bFlipV ? 1 : 0;
                    if ( nCond )
                    {
                        fYScale = (double)aSnapRect.GetHeight() / (double)aPolyPieRect.GetHeight();
                        fYOfs = ( (double)aPolyPieRect.GetHeight() - (double)aPolyArcRect.GetHeight() ) * fYScale;
                    }
                }
                fXScale = (double)aPolyArcRect.GetWidth() / (double)aPolyPieRect.GetWidth();
                fYScale = (double)aPolyArcRect.GetHeight() / (double)aPolyPieRect.GetHeight();

                aPolyArcRect = Rectangle( Point( aSnapRect.Left() + (sal_Int32)fXOfs, aSnapRect.Top() + (sal_Int32)fYOfs ),
                    Size( (sal_Int32)( aSnapRect.GetWidth() * fXScale ), (sal_Int32)( aSnapRect.GetHeight() * fYScale ) ) );

                SdrCircObj* pObjCirc = new SdrCircObj( OBJ_CARC, aPolyBoundRect, nStartAngle, nEndAngle );
                pObjCirc->SetSnapRect( aPolyArcRect );
                pObjCirc->SetModel( pSdrModel );
                pObjCirc->SetItemSet( rSet );

                int nSwap = bFlipH ? 1 : 0;
                nSwap ^= bFlipV ? 1 : 0;
                if ( nSwap )
                    SwapStartAndEndArrow( pObjCirc );

                SdrRectObj* pRect = new SdrRectObj( aPolyArcRect );
                pRect->SetSnapRect( aPolyArcRect );
                pRect->SetModel( pSdrModel );
                pRect->SetItemSet( rSet );
                pRect->SetItem( XLineStyleItem( XLINE_NONE ) );
                pRect->SetItem( XFillStyleItem( XFILL_NONE ) );

                pRet = new SdrObjGroup();
                pRet->SetModel( pSdrModel );
                ((SdrObjGroup*)pRet)->GetSubList()->NbcInsertObject( pRect );
                ((SdrObjGroup*)pRet)->GetSubList()->NbcInsertObject( pObjCirc );
            }
        }
        if ( !pRet && nNumElemVert )
        {
            // Header auswerten
            XPolygon aXP( (sal_uInt16)nNumElemVert );
            const SvxMSDffVertPair* pTmp = pVertData;
            sal_uInt32 nPtNum;
            for ( nPtNum = 0; nPtNum < nNumElemVert; nPtNum++ )
                aXP[ (sal_uInt16)nPtNum ] = GetPoint( *pTmp++, sal_True );
            Rectangle   aPolyBoundRect( aXP.GetBoundRect() );

            if ( !pSegData )
            {
                FASTBOOL bClosed = aXP[ 0 ] == aXP[ (sal_uInt16)( aXP.GetPointCount() - 1 ) ];
                Rectangle aUnion( aXP.GetBoundRect() );
                pRet = new SdrPathObj( bClosed ? OBJ_POLY : OBJ_PLIN, aXP );
                pRet->NbcSetSnapRect( Rectangle( Point( aSnapRect.Left() + aUnion.Left(),
                                                         aSnapRect.Top() + aUnion.Top() ),
                                                            aUnion.GetSize() ) );
                pRet->SetModel( pSdrModel );
                pRet->SetItemSet(rSet);
            }
            else
            {
                SdrObjGroup*    pGrp = NULL;
                SdrObject*      pSdrPathObj = NULL;

                XPolyPolygon    aPolyPoly;
                XPolygon        aPoly;

                XPolyPolygon    aEmptyPolyPoly;
                XPolygon        aEmptyPoly;

                BOOL            bClosed;
                sal_uInt16      nPolyFlags;

                Color           aFillColor( COL_WHITE );
                sal_uInt32      nColorCount = nColorData >> 28;
                sal_uInt32      nColorIndex = 0;
                sal_uInt16      nSrcPt = 0;

                Rectangle       aUnion;

                const sal_uInt16* pTmp = pSegData;

                if ( nColorCount )
                {
                    const SfxPoolItem* pPoolItem = NULL;
                    SfxItemState eState = rSet.GetItemState( XATTR_FILLCOLOR, FALSE, &pPoolItem );
                    if( SFX_ITEM_SET == eState )
                    {
                        if ( pPoolItem )
                            aFillColor = ((XFillColorItem*)pPoolItem)->GetValue();
                    }
                }
                for ( sal_uInt16 i = 0; i < nNumElemSeg; i++ )
                {
                    nPolyFlags = *pTmp++;
                    switch ( nPolyFlags >> 12 )
                    {
                        case 0x4 :
                        {
                            if ( aPoly.GetPointCount() > 1 )
                            {
                                if ( bClosed )
                                    aPoly[ aPoly.GetPointCount() ] = aPoly[ 0 ];
                                aPolyPoly.Insert( aPoly );
                            }
                            bClosed = FALSE;
                            aPoly = aEmptyPoly;
                            aPoly[ 0 ] = aXP[ nSrcPt++ ];
                        }
                        break;
                        case 0x8 :
                        {
                            if ( aPoly.GetPointCount() > 1 )
                            {
                                if ( bClosed )
                                    aPoly[ aPoly.GetPointCount() ] = aPoly[ 0 ];
                                aPolyPoly.Insert( aPoly );
                            }
                            aPoly = aEmptyPoly;
                            if ( aPolyPoly.Count() )
                            {
                                if ( pSdrPathObj )
                                {
                                    pGrp = new SdrObjGroup();
                                    pGrp->SetModel( pSdrModel );
                                    pGrp->NbcSetLogicRect( aSnapRect );
                                    pGrp->GetSubList()->NbcInsertObject( pSdrPathObj );
                                }
                                aUnion.Union( aPolyPoly.GetBoundRect() );
                                pSdrPathObj = new SdrPathObj( bClosed ? OBJ_POLY : OBJ_PLIN, aPolyPoly );
                                pSdrPathObj->SetModel( pSdrModel );
                                if ( !bClosed )
                                    rSet.Put( SdrShadowItem( FALSE ) );
                                else
                                {
                                    if ( nColorIndex < nColorCount )
                                    {
                                        Color aColor( ImplGetColorData( aFillColor, nColorIndex++ ) );
                                        rSet.Put( XFillColorItem( String(), aColor ) );
                                    }
                                }
                                pSdrPathObj->SetItemSet(rSet);
                                if ( pGrp )
                                {
                                    if ( pSdrPathObj )
                                    {
                                        pGrp->GetSubList()->NbcInsertObject( pSdrPathObj );
                                        pSdrPathObj = NULL;
                                    }
                                }
                                aPolyPoly = aEmptyPolyPoly;
                            }
                        }
                        break;
                        case 0x6 :
                        {
                            bClosed = TRUE;
                        }
                        break;
                        case 0x2 :
                        {
                            sal_uInt16 nDstPt = aPoly.GetPointCount();
                            for ( sal_uInt16 i = 0; i < ( nPolyFlags & 0xfff ); i++ )
                            {
                                aPoly[ nDstPt ] = aXP[ nSrcPt++ ];
                                aPoly.SetFlags( nDstPt++, XPOLY_CONTROL );
                                aPoly[ nDstPt ] = aXP[ nSrcPt++ ];
                                aPoly.SetFlags( nDstPt++, XPOLY_CONTROL );
                                aPoly[ nDstPt++ ] = aXP[ nSrcPt++ ];
                            }
                        }
                        break;
                        case 0xa :
                        case 0xb :
                        {
                            sal_uInt16 nPntCount = (BYTE)nPolyFlags;
                            if ( nPntCount )
                            {
                                sal_uInt32 nMod = ( nPolyFlags >> 8 ) & 0xf;
                                switch ( nMod )
                                {
                                    case 3 :
                                    case 4 :
                                    case 5 :
                                    {
                                        sal_uInt16 nDstPt = aPoly.GetPointCount();
                                        if ( nPntCount == 2 )
                                        {   // create a circle
                                            Rectangle aRect( aXP[ nSrcPt ], aXP[ nSrcPt + 1 ] );
                                            sal_Int32 nXControl = (sal_Int32)((double)aRect.GetWidth() * 0.2835 );
                                            sal_Int32 nYControl = (sal_Int32)((double)aRect.GetHeight() * 0.2835 );
                                            Point     aCenter( aRect.Center() );
                                            aPoly[ nDstPt++ ] = Point( aCenter.X(), aRect.Top() );
                                            aPoly[ nDstPt ] = Point( aCenter.X() + nXControl, aRect.Top() );
                                            aPoly.SetFlags( nDstPt++, XPOLY_CONTROL );
                                            aPoly[ nDstPt ] = Point( aRect.Right(), aCenter.Y() - nYControl );
                                            aPoly.SetFlags( nDstPt++, XPOLY_CONTROL );
                                            aPoly[ nDstPt++ ] = Point( aRect.Right(), aCenter.Y() );
                                            aPoly[ nDstPt ] = Point( aRect.Right(), aCenter.Y() + nYControl );
                                            aPoly.SetFlags( nDstPt++, XPOLY_CONTROL );
                                            aPoly[ nDstPt ] = Point( aCenter.X() + nXControl, aRect.Bottom() );
                                            aPoly.SetFlags( nDstPt++, XPOLY_CONTROL );
                                            aPoly[ nDstPt++ ] = Point( aCenter.X(), aRect.Bottom() );
                                            aPoly[ nDstPt ] = Point( aCenter.X() - nXControl, aRect.Bottom() );
                                            aPoly.SetFlags( nDstPt++, XPOLY_CONTROL );
                                            aPoly[ nDstPt ] = Point( aRect.Left(), aCenter.Y() + nYControl );
                                            aPoly.SetFlags( nDstPt++, XPOLY_CONTROL );
                                            aPoly[ nDstPt++ ] = Point( aRect.Left(), aCenter.Y() );
                                            aPoly[ nDstPt ] = Point( aRect.Left(), aCenter.Y() - nYControl );
                                            aPoly.SetFlags( nDstPt++, XPOLY_CONTROL );
                                            aPoly[ nDstPt ] = Point( aCenter.X() - nXControl, aRect.Top() );
                                            aPoly.SetFlags( nDstPt++, XPOLY_CONTROL );
                                            aPoly[ nDstPt++ ] = Point( aCenter.X(), aRect.Top() );
                                            nSrcPt += 2;
                                        }
                                        else
                                        {
                                            sal_uInt32 nXor = ( nMod == 5 ) ? 3 : 2;
                                            for ( sal_uInt16 i = 0; i < ( nPntCount >> 2 ); i++ )
                                            {
                                                PolyStyle ePolyStyle = POLY_ARC;
                                                Rectangle aRect( aXP[ nSrcPt ], aXP[ nSrcPt + 1 ] );
                                                Point aCenter( aRect.Center() );
                                                Point aStart( aXP[ (sal_uInt16)( nSrcPt + nXor ) ] );
                                                Point aEnd( aXP[ (sal_uInt16)( nSrcPt + ( nXor ^ 1 ) ) ] );
                                                aStart.X() = (sal_Int32)( ( (double)( aStart.X() - aCenter.X() ) / fXScale ) ) + aCenter.X();
                                                aStart.Y() = (sal_Int32)( ( (double)( aStart.Y() - aCenter.Y() ) / fYScale ) ) + aCenter.Y();
                                                aEnd.X() = (sal_Int32)( ( (double)( aEnd.X() - aCenter.X() ) / fXScale ) ) + aCenter.X();
                                                aEnd.Y() = (sal_Int32)( ( (double)( aEnd.Y() - aCenter.Y() ) / fYScale ) ) + aCenter.Y();

                                                Polygon aTempPoly( aRect, aStart, aEnd, ePolyStyle );
                                                if ( nMod == 5 )
                                                {
                                                    for ( sal_uInt16 j = aTempPoly.GetSize(); j--; )
                                                        aPoly[ nDstPt++ ] = aTempPoly[ j ];
                                                }
                                                else
                                                {
                                                    for ( sal_uInt16 j = 0; j < aTempPoly.GetSize(); j++ )
                                                        aPoly[ nDstPt++ ] = aTempPoly[ j ];
                                                }
                                                nSrcPt += 4;
                                            }
                                        }
                                    }
                                    break;
                                    case 0 :
                                    case 1 :
                                    case 2 :
                                    case 6 :
                                    case 9 :
                                    case 0xa :
                                    case 0xb :
                                    case 0xc :
                                    case 0xd :
                                    case 0xe :
                                    case 0xf :

                                    case 7 :
                                    case 8 :
                                    {
                                        BOOL    bFirstDirection;
                                        sal_uInt16  nDstPt = aPoly.GetPointCount();
                                        for ( sal_uInt16 i = 0; i < ( nPolyFlags & 0xff ); i++ )
                                        {
                                            sal_uInt32 nModT = ( nMod == 7 ) ? 1 : 0;
                                            Point aCurrent( aXP[ nSrcPt ] );
                                            if ( nSrcPt )   // we need a previous point
                                            {
                                                Point aPrev( aXP[ nSrcPt - 1 ] );
                                                sal_Int32 nX, nY;
                                                nX = aCurrent.X() - aPrev.X();
                                                nY = aCurrent.Y() - aPrev.Y();
                                                if ( ( nY ^ nX ) & 0x80000000 )
                                                {
                                                    if ( !i )
                                                        bFirstDirection = TRUE;
                                                    else if ( !bFirstDirection )
                                                        nModT ^= 1;
                                                }
                                                else
                                                {
                                                    if ( !i )
                                                        bFirstDirection = FALSE;
                                                    else if ( bFirstDirection )
                                                        nModT ^= 1;
                                                }
                                                if ( nModT )            // get the right corner
                                                {
                                                    nX = aCurrent.X();
                                                    nY = aPrev.Y();
                                                }
                                                else
                                                {
                                                    nX = aPrev.X();
                                                    nY = aCurrent.Y();
                                                }
                                                sal_Int32 nXVec = ( nX - aPrev.X() ) >> 1;
                                                sal_Int32 nYVec = ( nY - aPrev.Y() ) >> 1;
                                                Point aControl1( aPrev.X() + nXVec, aPrev.Y() + nYVec );
                                                aPoly[ nDstPt ] = aControl1;
                                                aPoly.SetFlags( nDstPt++, XPOLY_CONTROL );
                                                nXVec = ( nX - aCurrent.X() ) >> 1;
                                                nYVec = ( nY - aCurrent.Y() ) >> 1;
                                                Point aControl2( aCurrent.X() + nXVec, aCurrent.Y() + nYVec );
                                                aPoly[ nDstPt ] = aControl2;
                                                aPoly.SetFlags( nDstPt++, XPOLY_CONTROL );
                                            }
                                            aPoly[ nDstPt ] = aCurrent;
                                            nSrcPt++;
                                            nDstPt++;
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                        break;
                        case 0x0 :
                        {
                            sal_uInt16 nDstPt = aPoly.GetPointCount();
                            for ( sal_uInt16 i = 0; i < ( nPolyFlags & 0xfff ); i++ )
                                aPoly[ nDstPt++ ] = aXP[ nSrcPt++ ];
                        }
                        break;
                        case 0xf :
                        {
                            sal_Bool bOwn = TRUE;
                            switch ( nPolyFlags )
                            {
                                case 0xf8ff :                                                   // This value is not ms specific and is used
                                {                                                               // to create a dummy object that is not visible.
                                    SdrObject* pLast = pSdrPathObj;                             // This solves the problem of autoshapes that
                                    if ( !pLast )                                               // did not use the whole space of the boundrect.
                                    {                                                           // eg. the BlockArc
                                        if ( pGrp )
                                        {
                                            SdrObjList* pList = pGrp->GetSubList();
                                            if ( pList && pList->GetObjCount() )
                                                pLast = pList->GetObj( pList->GetObjCount() - 1 );
                                        }
                                    }
                                    if ( pLast )
                                    {
                                        pLast->SetItem( XLineStyleItem( XLINE_NONE ) );
                                        pLast->SetItem( XFillStyleItem( XFILL_NONE ) );
                                    }
                                }
                                break;
                                case 0xf8fe :                                                   // nearly the same as 0x4000
                                {                                                               // but the first point is ignored
                                    if ( aPoly.GetPointCount() > 1 )
                                    {
                                        if ( bClosed )
                                            aPoly[ aPoly.GetPointCount() ] = aPoly[ 0 ];
                                        aPolyPoly.Insert( aPoly );
                                    }
                                    aPoly = aEmptyPoly;
                                }
                                break;
                                default :
                                    bOwn = FALSE;
                            }
                            if ( bOwn )
                                break;
                        }
#ifdef DBG_AUTOSHAPE
                        default :
                        {
                            ByteString aString( "autoshapes::unknown PolyFlagValue :" );
                            aString.Append( ByteString::CreateFromInt32( nPolyFlags ) );
                            DBG_ERROR( aString.GetBuffer() );
                        }
                        break;
#endif
                    }
                }
                if ( pGrp )
                    pRet = pGrp;
                else
                    pRet = pSdrPathObj;
                if ( pRet )
                {
                    pRet->NbcSetSnapRect( Rectangle( Point( aSnapRect.Left() + aUnion.Left(),
                                                             aSnapRect.Top() + aUnion.Top() ),
                                                                aUnion.GetSize() ) );
                }
            }
        }
    }
    if ( pRet )
    {
        // some objects needs to have their own GluePoint list
        if ( HasGluePointList( eSpType ) )
        {
            sal_uInt32 i;
            const SvxMSDffVertPair* pTmp = pGluePoints;
            for ( i = 0; i < nGluePoints; i++ )
            {
                SdrGluePoint aGluePoint;
                const Point& rPoint = GetPoint( *pTmp++, sal_True );
                double fXRel = rPoint.X();
                double fYRel = rPoint.Y();
                fXRel = fXRel / aSnapRect.GetWidth() * 10000;
                fYRel = fYRel / aSnapRect.GetHeight() * 10000;
                aGluePoint.SetPos( Point( (sal_Int32)fXRel, (sal_Int32)fYRel ) );
                aGluePoint.SetPercent( sal_True );
                aGluePoint.SetAlign( SDRVERTALIGN_TOP | SDRHORZALIGN_LEFT );
                aGluePoint.SetEscDir( SDRESC_SMART );
                SdrGluePointList* pList = pRet->ForceGluePointList();
                if( pList )
                {
                    sal_uInt16 nId = pList->Insert( aGluePoint );
//                  sal_uInt16 nNewId = (sal_Int32)((*pList)[nId].GetId() + 4 );
                }
            }
        }
        if ( bSetAutoShapeAdjustItem )
        {
            if ( pRet->ISA( SdrObjGroup ) || pRet->ISA( SdrPathObj ) )
            {
                sal_uInt32  i, nCount = 0;
                SdrObject* pFirstObject = pRet;
                SdrObjList* pObjectList = NULL;
                if ( pRet->ISA( SdrPathObj ) )
                    nCount = 1;
                else
                {
                    pObjectList = ((SdrObjGroup*)pRet)->GetSubList();
                    if ( pObjectList )
                        nCount = pObjectList->GetObjCount();
                    if ( nCount )
                        pFirstObject = pObjectList->GetObj( 0 );
                }
                if ( nCount )
                {
                    SdrAutoShapeAdjustmentItem aAdjustItem;
                    SdrAutoShapeAdjustmentValue aAdjustValue;
                    if ( nAdjustmentHandles )
                    {
                        for ( i = 0; i < nAdjustmentHandles; i++ )
                        {
                            aAdjustValue.SetValue( pAdjustmentHandles[ i ].GetAdjustValue() );
                            aAdjustItem.SetValue( i, aAdjustValue );
                        }
                    }
                    if ( bVertAlloc == FALSE )
                    {
                        sal_uInt32 nMagicNumber = 0x80001234;           // this magic number identifies ms objects
                        sal_uInt32 nType = (sal_uInt32)eSpType << 16    // hiword is the shapetype
                                                        | nCount;       // loword the number of Path objects
                        sal_uInt32 nChecksum = 0;                       // checksum is used later to be sure that
                                                                        // the object is not changed

                        // here we are calculating the checksum
                        Rectangle aTRect( pRet->GetSnapRect() );
                        sal_Int32 nLeft = aTRect.Left();
                        sal_Int32 nTop = aTRect.Top();

                        SdrObject* pNext = pFirstObject;
                        for ( i = 0; i < nCount; i++ )
                        {
                            if ( i )
                                pNext = pObjectList->GetObj( i );

                            if ( !pNext->ISA( SdrPathObj ) )            // just only SdrPathObjs are valid as Autoshapes
                            {
                                nChecksum = 0;
                                break;
                            }
                            const XPolyPolygon& rPolyPolygon = ((SdrPathObj*)pNext)->GetPathPoly();
                            sal_uInt16 j, k = rPolyPolygon.Count();
                            sal_Int32 aVal[ 3 ];
                            while( k )
                            {
                                const XPolygon& rPoly = rPolyPolygon[ --k ];
                                j = rPoly.GetPointCount();
                                while( j )
                                {
                                    const Point& rPoint = rPoly[ --j ];
#ifdef __LITTLEENDIAN
                                    aVal[ 0 ] = SWAPLONG( rPoint.X() - nLeft );
                                    aVal[ 1 ] = SWAPLONG( rPoint.Y() - nTop );
                                    aVal[ 2 ] = SWAPLONG( rPoly.GetFlags( j ) );
#else
                                    aVal[ 0 ] = rPoint.X() - nLeft;
                                    aVal[ 1 ] = rPoint.Y() - nTop;
                                    aVal[ 2 ] = rPoly.GetFlags( j );
#endif
                                    nChecksum = rtl_crc32( nChecksum, &aVal[ 0 ], 12 );
                                }
                            }
                        }

                        if ( nChecksum )
                        {
                            i = aAdjustItem.GetCount();
                            aAdjustValue.SetValue( nChecksum );         // the last 3 entries in the adjustment item must be in this
                            aAdjustItem.SetValue( i++, aAdjustValue );  // order, otherwise it will be not possible to recreate the original
                            aAdjustValue.SetValue( nType );             // autoshape when exporting into ms formats
                            aAdjustItem.SetValue( i++, aAdjustValue );
                            aAdjustValue.SetValue( nMagicNumber );
                            aAdjustItem.SetValue( i++, aAdjustValue );
                        }
                    }
                    SfxItemSet aSet( pSdrModel->GetItemPool() );
                    aSet.Put( aAdjustItem );
                    pFirstObject->SetItemSet(aSet);
                }
            }
        }
    }
    return pRet;
}
