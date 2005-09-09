/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: salconst.h,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 10:33:14 $
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

#ifndef _SV_SALCONST_H
#define _SV_SALCONST_H

// -------------------
// - Constants -
// -------------------

static const unsigned short kByteMask = 0xFF;

static const unsigned short kOneByte  =  8;
static const unsigned short kTwoBytes = 16;

static const unsigned short kOneBit     =  1;
static const unsigned short kFiveBits   =  5;
static const unsigned short kEightBits  =  8;
static const unsigned short kTenBits    = 10;
static const unsigned short kElevenBits = 11;

static const unsigned short kBlackAndWhite  =  1;
static const unsigned short kFourBitColor   =  4;
static const unsigned short kEightBitColor  =  8;
static const unsigned short kThousandsColor = 16;
static const unsigned short kTrueColor      = 32;

static const unsigned long k16BitRedColorMask   = 0x00007c00;
static const unsigned long k16BitGreenColorMask = 0x000003e0;
static const unsigned long k16BitBlueColorMask  = 0x0000001f;

static const unsigned long k32BitRedColorMask   = 0x00ff0000;
static const unsigned long k32BitGreenColorMask = 0x0000ff00;
static const unsigned long k32BitBlueColorMask  = 0x000000ff;

static const unsigned short kPixMapCmpSizeOneBit    =  1;
static const unsigned short kPixMapCmpSizeFourBits  =  4;
static const unsigned short kPixMapCmpSizeFiveBits  =  5;
static const unsigned short kPixMapCmpSizeEightBits =  8;

static const long kPixMapHRes = 72;
static const long kPixMapVRes = 72;

#endif // _SV_SALCONST_H
