/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: color.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: thb $ $Date: 2006-06-07 14:27:35 $
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

#ifndef INCLUDED_BASEBMP_COLOR_HXX
#define INCLUDED_BASEBMP_COLOR_HXX

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#include <vigra/mathutil.hxx>
#include <math.h>

namespace basebmp
{

class Color
{
private:
    sal_uInt32          mnColor;

public:
    Color() : mnColor(0) {}
    explicit Color( sal_uInt32 nVal ) : mnColor(nVal) {}
    Color( sal_uInt8 nRed, sal_uInt8 nGreen, sal_uInt8 nBlue ) :
        mnColor( ((sal_uInt32)nRed << 16) | ((sal_uInt32)nGreen << 8) | nBlue )
    {}

    void setRed( sal_uInt8 nRed ) { mnColor |= (sal_uInt32)nRed << 16; mnColor &= ~((sal_uInt32)nRed << 16); }
    void setGreen( sal_uInt8 nGreen ) { mnColor |= (sal_uInt32)nGreen << 8; mnColor &= ~((sal_uInt32)nGreen << 8); }
    void setBlue( sal_uInt8 nBlue ) { mnColor |= nBlue; mnColor &= ~(sal_uInt32)nBlue; }

    void setGray( sal_uInt8 nGrayVal ) { mnColor = (sal_uInt32)nGrayVal << 16 | (sal_uInt32)nGrayVal << 8 | nGrayVal; }

    sal_uInt8 getRed() const   { return 0xFF & (mnColor >> 16); }
    sal_uInt8 getGreen() const { return 0xFF & (mnColor >> 8); }
    sal_uInt8 getBlue() const  { return 0xFF & mnColor; }

    sal_uInt8 getGrayscale() const { return (sal_uInt8)((getBlue()*28UL +
                                                         getGreen()*151 +
                                                         getRed()*77) / 256); }

    sal_uInt32 toInt32() const { return mnColor; }

    Color operator^( Color col ) const { return Color(col.getRed()^getRed(),
                                                      col.getGreen()^getGreen(),
                                                      col.getBlue()^getBlue()); }
    Color operator-( Color col ) const { return Color(vigra::abs((int)getRed()-col.getRed()),
                                                      vigra::abs((int)getGreen()-col.getGreen()),
                                                      vigra::abs((int)getBlue()-col.getBlue())); }
    Color operator+( Color col ) const { return Color(getRed()+col.getRed(),
                                                      getGreen()+col.getGreen(),
                                                      getBlue()+col.getBlue()); }
    Color operator*( Color col ) const { return Color((sal_uInt32)col.getRed()*getRed()/SAL_MAX_UINT8,
                                                      (sal_uInt32)col.getGreen()*getGreen()/SAL_MAX_UINT8,
                                                      (sal_uInt32)col.getBlue()*getBlue()/SAL_MAX_UINT8); }
    Color operator*( sal_uInt8 n ) const { return Color((sal_uInt32)n*getRed()/SAL_MAX_UINT8,
                                                        (sal_uInt32)n*getGreen()/SAL_MAX_UINT8,
                                                        (sal_uInt32)n*getBlue()/SAL_MAX_UINT8); }
    Color operator*( double n ) const { return Color((sal_uInt8)(n*getRed()+.5),
                                                     (sal_uInt8)(n*getGreen()+.5),
                                                     (sal_uInt8)(n*getBlue()+.5)); }
    bool operator==( const Color& rhs ) const { return (getRed()==rhs.getRed() &&
                                                        getGreen()==rhs.getGreen() &&
                                                        getBlue()==rhs.getBlue()); }
    bool operator!=( const Color& rhs ) const { return !(*this==rhs); }
    double magnitude() const { return sqrt(getRed()*getRed()
                                           + getGreen()*getGreen()
                                           + getBlue()*getBlue()); }
};

} // namespace basebmp

#endif /* INCLUDED_BASEBMP_COLOR_HXX */
