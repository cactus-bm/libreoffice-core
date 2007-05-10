/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: qprostyle.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: kz $ $Date: 2007-05-10 16:51:04 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sc.hxx"


#include <sal/config.h>
#include <stdio.h>
#include <sfx2/docfile.hxx>

#include "qproform.hxx"
#include "qpro.hxx"
#include "qprostyle.hxx"

#include <tools/color.hxx>
#include <scitems.hxx>
#include <svx/algitem.hxx>
#include <svx/udlnitem.hxx>
#include <svx/wghtitem.hxx>
#include <svx/postitem.hxx>
#include <svx/crsditem.hxx>
#include <svx/fhgtitem.hxx>
#include <svx/fontitem.hxx>
#include <map>

#include "global.hxx"
#include "scerrors.hxx"
#include "docpool.hxx"
#include "patattr.hxx"
#include "filter.hxx"
#include "document.hxx"
#include "cell.hxx"

ScQProStyle::ScQProStyle() :
    maFontLimit(0), maAlignLimit(0)
{
    rtl_fillMemory (maAlign, sizeof (maAlign), 0);
    rtl_fillMemory (maFont, sizeof (maFont), 0);
    rtl_fillMemory (maFontRecord, sizeof (maFontRecord), 0);
    rtl_fillMemory (maFontHeight, sizeof (maFontHeight), 0);
}

void ScQProStyle::SetFormat( ScDocument *pDoc, sal_uInt8 nCol, sal_uInt16 nRow, SCTAB nTab, sal_uInt16 nStyle )
{
    ScPatternAttr aPattern(pDoc->GetPool());
    SfxItemSet& rItemSet = aPattern.GetItemSet();

    sal_uInt8 nTmp = maAlign[ nStyle ];
    sal_uInt8 nHor = ( nTmp & 0x07 );
    sal_uInt8 nVer = ( nTmp & 0x18 );
    sal_uInt8 nOrient = ( nTmp & 0x60 );

    // Horizontal Alignment
    SvxCellHorJustify eJustify = SVX_HOR_JUSTIFY_STANDARD;
    switch( nHor )
    {
        case 0x00:
            eJustify = SVX_HOR_JUSTIFY_STANDARD;
            break;

        case 0x01:
            eJustify = SVX_HOR_JUSTIFY_LEFT;
            break;

        case 0x02:
            eJustify = SVX_HOR_JUSTIFY_CENTER;
            break;

        case 0x03:
            eJustify = SVX_HOR_JUSTIFY_RIGHT;
            break;

        case 0x04:
            eJustify = SVX_HOR_JUSTIFY_BLOCK;
            break;
    }
    rItemSet.Put( SvxHorJustifyItem( eJustify, ATTR_HOR_JUSTIFY ) );

    // Vertical Alignment
    SvxCellVerJustify eVerJustify = SVX_VER_JUSTIFY_STANDARD;
    switch( nVer )
    {
        case 0x00:
            eVerJustify = SVX_VER_JUSTIFY_BOTTOM;
            break;

        case 0x08:
            eVerJustify = SVX_VER_JUSTIFY_CENTER;
            break;

        case 0x10:
            eVerJustify = SVX_VER_JUSTIFY_TOP;
            break;
    }

    rItemSet.Put(SvxVerJustifyItem( eVerJustify, ATTR_VER_JUSTIFY ) );

    // Orientation
    SvxCellOrientation eOrient = SVX_ORIENTATION_STANDARD;
    switch( nOrient )
    {
        case 0x20:
            eOrient = SVX_ORIENTATION_TOPBOTTOM;
            break;

    }
    rItemSet.Put( SvxOrientationItem( eOrient, 0) );

    // Wrap cell contents
    if( nTmp & 0x80 )
    {
        SfxBoolItem aWrapItem( ATTR_LINEBREAK );
        aWrapItem.SetValue( TRUE );
        rItemSet.Put( aWrapItem );
    }

    // Font Attributes
    sal_uInt16 nTmpFnt = maFontRecord[ maFont[ nStyle ] ];
    BOOL bIsBold, bIsItalic, bIsUnderLine, bIsStrikeThrough;

    bIsBold = ( nTmpFnt & 0x0001 ) != 0;
    bIsItalic = ( nTmpFnt & 0x0002 ) != 0;
    bIsUnderLine = ( nTmpFnt & 0x0004 ) != 0;
    bIsStrikeThrough = (nTmpFnt & 0x0020 ) != 0;

    if( bIsBold )
        rItemSet.Put( SvxWeightItem( WEIGHT_BOLD,ATTR_FONT_WEIGHT) );
    if( bIsItalic )
        rItemSet.Put( SvxPostureItem( ITALIC_NORMAL, ATTR_FONT_POSTURE ) );
    if( bIsUnderLine )
        rItemSet.Put( SvxUnderlineItem( UNDERLINE_SINGLE, ATTR_FONT_UNDERLINE ) );

    if (maFontHeight[ maFont [ nStyle ] ])
        rItemSet.Put( SvxFontHeightItem( (ULONG) (20 * maFontHeight[ maFont[ nStyle ] ] ), 100, ATTR_FONT_HEIGHT ) );

    String fntName = maFontType[ maFont[ nStyle ] ];
    rItemSet.Put( SvxFontItem( FAMILY_SYSTEM, fntName, EMPTY_STRING, PITCH_DONTKNOW, RTL_TEXTENCODING_DONTKNOW, ATTR_FONT ) );

    pDoc->ApplyPattern( nCol, nRow, nTab, aPattern );
}
