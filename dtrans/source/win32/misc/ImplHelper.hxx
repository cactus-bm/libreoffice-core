/*************************************************************************
 *
 *  $RCSfile: ImplHelper.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2003-10-06 14:42:03 $
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


#ifndef _IMPLHELPER_HXX_
#define _IMPLHELPER_HXX_

//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _RTL_USTRING_
#include <rtl/ustring>
#endif

#include <windows.h>

//------------------------------------------------------------------------
// deklarations
//------------------------------------------------------------------------

// target device and formatetc helper
void      SAL_CALL DeleteTargetDevice(DVTARGETDEVICE* ptd);
sal_Bool  SAL_CALL CopyFormatEtc(LPFORMATETC petcDest, LPFORMATETC petcSrc);
sal_Int32 SAL_CALL CompareFormatEtc( const FORMATETC* pFetcLeft, const FORMATETC* pFetcRight);
sal_Bool  SAL_CALL CompareTargetDevice(DVTARGETDEVICE* ptdLeft, DVTARGETDEVICE* ptdRight);
DVTARGETDEVICE* SAL_CALL CopyTargetDevice(DVTARGETDEVICE* ptdSrc);

// some codepage helper functions

//--------------------------------------------------
// returns a windows codepage appropriate to the
// given mime charset parameter value
//--------------------------------------------------

sal_uInt32 SAL_CALL getWinCPFromMimeCharset(
    const rtl::OUString& charset );

//--------------------------------------------------
// returns a windows codepage appropriate to the
// given locale and locale type
//--------------------------------------------------

rtl::OUString SAL_CALL getWinCPFromLocaleId(
    LCID lcid, LCTYPE lctype );

//--------------------------------------------------
// returns a mime charset parameter value appropriate
// to the given codepage, optional a prefix can be
// given, e.g. "windows-" or "cp"
//--------------------------------------------------

rtl::OUString SAL_CALL getMimeCharsetFromWinCP(
    sal_uInt32 cp, const rtl::OUString& aPrefix );

//--------------------------------------------------
// returns a mime charset parameter value appropriate
// to the given locale id and locale type, optional a
// prefix can be given, e.g. "windows-" or "cp"
//--------------------------------------------------

rtl::OUString SAL_CALL getMimeCharsetFromLocaleId(
    LCID lcid, LCTYPE lctype, const rtl::OUString& aPrefix  );

//-----------------------------------------------------
// returns true, if a given codepage is an oem codepage
//-----------------------------------------------------

sal_Bool SAL_CALL IsOEMCP( sal_uInt32 codepage );

//--------------------------------------------------
// converts a codepage into a string representation
//--------------------------------------------------

rtl::OUString SAL_CALL cptostr( sal_uInt32 codepage );

#endif
