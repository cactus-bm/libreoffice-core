/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dlgeddef.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: kz $ $Date: 2007-06-20 10:40:07 $
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

#ifndef _BASCTL_DLGEDDEF_HXX
#define _BASCTL_DLGEDDEF_HXX

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif


const UINT32 DlgInventor = UINT32('D')*0x00000001+
                           UINT32('L')*0x00000100+
                           UINT32('G')*0x00010000+
                           UINT32('1')*0x01000000;


#define OBJ_DLG_CONTROL         ((UINT16) 1)
#define OBJ_DLG_DIALOG          ((UINT16) 2)
#define OBJ_DLG_PUSHBUTTON      ((UINT16) 3)
#define OBJ_DLG_RADIOBUTTON     ((UINT16) 4)
#define OBJ_DLG_CHECKBOX        ((UINT16) 5)
#define OBJ_DLG_LISTBOX         ((UINT16) 6)
#define OBJ_DLG_COMBOBOX        ((UINT16) 7)
#define OBJ_DLG_GROUPBOX        ((UINT16) 8)
#define OBJ_DLG_EDIT            ((UINT16) 9)
#define OBJ_DLG_FIXEDTEXT       ((UINT16)10)
#define OBJ_DLG_IMAGECONTROL    ((UINT16)11)
#define OBJ_DLG_PROGRESSBAR     ((UINT16)12)
#define OBJ_DLG_HSCROLLBAR      ((UINT16)13)
#define OBJ_DLG_VSCROLLBAR      ((UINT16)14)
#define OBJ_DLG_HFIXEDLINE      ((UINT16)15)
#define OBJ_DLG_VFIXEDLINE      ((UINT16)16)
#define OBJ_DLG_DATEFIELD       ((UINT16)17)
#define OBJ_DLG_TIMEFIELD       ((UINT16)18)
#define OBJ_DLG_NUMERICFIELD    ((UINT16)19)
#define OBJ_DLG_CURRENCYFIELD   ((UINT16)20)
#define OBJ_DLG_FORMATTEDFIELD  ((UINT16)21)
#define OBJ_DLG_PATTERNFIELD    ((UINT16)22)
#define OBJ_DLG_FILECONTROL     ((UINT16)23)


// control properties
#define DLGED_PROP_BACKGROUNDCOLOR      ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "BackgroundColor" ) )
#define DLGED_PROP_DROPDOWN             ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Dropdown" ) )
#define DLGED_PROP_FORMATSSUPPLIER      ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "FormatsSupplier" ) )
#define DLGED_PROP_HEIGHT               ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Height" ) )
#define DLGED_PROP_LABEL                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Label" ) )
#define DLGED_PROP_NAME                 ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Name" ) )
#define DLGED_PROP_ORIENTATION          ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Orientation" ) )
#define DLGED_PROP_POSITIONX            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PositionX" ) )
#define DLGED_PROP_POSITIONY            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PositionY" ) )
#define DLGED_PROP_STEP                 ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Step" ) )
#define DLGED_PROP_TABINDEX             ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "TabIndex" ) )
#define DLGED_PROP_TEXTCOLOR            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "TextColor" ) )
#define DLGED_PROP_TEXTLINECOLOR        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "TextLineColor" ) )
#define DLGED_PROP_WIDTH                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Width" ) )
#define DLGED_PROP_DECORATION           ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Decoration" ) )


#endif // _BASCTL_DLGEDDEF_HXX
