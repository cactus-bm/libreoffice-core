/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: qprostyle.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2006-01-27 15:50:32 $
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


#ifndef SC_QPROSTYLE_HXX
#define SC_QPROSTYLE_HXX

#include <sal/config.h>
#include "filter.hxx"
#include "document.hxx"
#include "cell.hxx"

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

#include <tools/color.hxx>
#include "flttypes.hxx"
#include "ftools.hxx"

class ScQProStyle
{
    sal_uInt8  maAlign[ 256 ];
    sal_uInt8  maFont[ 256 ];
    sal_uInt16 maFontRecord[ 256 ];
    sal_uInt16 maFontHeight[ 256 ];
    String     maFontType[ 256 ];
    sal_uInt16 maFontLimit, maAlignLimit;

    public:
    ScQProStyle();
    void SetFormat( ScDocument *pDoc, sal_uInt8 nCol, sal_uInt16 nRow, sal_uInt8 nTab, sal_uInt16 nStyle );
    void setFontRecord( sal_uInt16 nIndex, sal_uInt16 nData, sal_uInt16 nPtSize )
    {
        maFontRecord[ nIndex ] = nData;
        maFontHeight[ nIndex ] = nPtSize;
    }
    void setFontType( sal_uInt16 nIndex, String &aLabel ){ maFontType[ nIndex ] = aLabel; }
    void setAlign( sal_uInt16 nIndex, sal_uInt8 nData ) { maAlign[ nIndex ] = nData; }
    void setFont( sal_uInt16 nIndex, sal_uInt8 nData ) { maFont[ nIndex ] = nData; }
    void setFontLimit( sal_uInt16 nValue ) { maFontLimit = nValue; }
    void setAlignLimit( sal_uInt16 nValue ) { maAlignLimit = nValue; }
};
#endif
