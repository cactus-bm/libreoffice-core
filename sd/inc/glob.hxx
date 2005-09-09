/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: glob.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:55:19 $
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

#ifndef SD_GLOB_HXX
#define SD_GLOB_HXX

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif

//------------------------------------------------------------------

#define DIA_SLOW    0
#define DIA_MEDIUM  1
#define DIA_FAST    2

#define SD_IF_SDAPP                     SFX_INTERFACE_SD_START + 0
#define SD_IF_SDDRAWDOCSHELL            SFX_INTERFACE_SD_START + 1
#define SD_IF_SDVIEWSHELL               SFX_INTERFACE_SD_START + 2
#define SD_IF_SDDRAWVIEWSHELL           SFX_INTERFACE_SD_START + 3
#define SD_IF_SDSLIDEVIEWSHELL          SFX_INTERFACE_SD_START + 4
#define SD_IF_SDOUTLINEVIEWSHELL        SFX_INTERFACE_SD_START + 5
#define SD_IF_SDDRAWSTDOBJECTBAR        SFX_INTERFACE_SD_START + 6
#define SD_IF_SDDRAWTEXTOBJECTBAR       SFX_INTERFACE_SD_START + 7
#define SD_IF_SDDRAWBEZIEROBJECTBAR     SFX_INTERFACE_SD_START + 8
#define SD_IF_SDDRAWGLUEPOINTSOBJECTBAR SFX_INTERFACE_SD_START + 9
#define SD_IF_SDGRAPHICDOCSHELL         SFX_INTERFACE_SD_START + 10
#define SD_IF_SDGRAPHICVIEWSHELL        SFX_INTERFACE_SD_START + 11
#define SD_IF_SDGRAPHICSTDOBJECTBAR     SFX_INTERFACE_SD_START + 12
#define SD_IF_SDDRAWGRAFOBJECTBAR       SFX_INTERFACE_SD_START + 13
#define SD_IF_SDPRESVIEWSHELL           SFX_INTERFACE_SD_START + 14
#define SD_IF_SDPREVIEWVIEWSHELL        SFX_INTERFACE_SD_START + 15
#define SD_IF_SDVIEWSHELLBASE           SFX_INTERFACE_SD_START + 16
#define SD_IF_SD3DOBJECTBAR             SFX_INTERFACE_SD_START + 17
#define SD_IF_SDFONTWORKOBJECTBAR       SFX_INTERFACE_SD_START + 18
#define SD_IF_SDSLIDESORTERVIEWSHELL    SFX_INTERFACE_SD_START + 19
#define SD_IF_SDTASKPANEVIEWSHELL       SFX_INTERFACE_SD_START + 20
#define SD_IF_SDMASTERPAGESSELECTOR     SFX_INTERFACE_SD_START + 21
#define SD_IF_SDLAYOUTMENU              SFX_INTERFACE_SD_START + 22
#define SD_IF_SDDRAWMEDIAOBJECTBAR      SFX_INTERFACE_SD_START + 23

// Inventor-Id fuer StarDraw UserData
const UINT32 SdUDInventor=UINT32('S')*0x00000001+
                          UINT32('D')*0x00000100+
                          UINT32('U')*0x00010000+
                          UINT32('D')*0x01000000;

// Object-Ids fuer StarDraw UserData
#define SD_ANIMATIONINFO_ID 1
#define SD_IMAPINFO_ID      2

// FamilyId der Praesentationsvorlagen
#define SD_LT_FAMILY (SfxStyleFamily)0xaffe

// Trennzeichen zwischen Layoutname und Vorlagenname der Praesentationsvorlagen
#define SD_LT_SEPARATOR "~LT~"

// Optionsstream-Identifier
#define SD_OPTION_MORPHING  "Morph"
#define SD_OPTION_VECTORIZE "Vectorize"

//------------------------------------------------------------------

#endif // _SD_GLOB_HXX


