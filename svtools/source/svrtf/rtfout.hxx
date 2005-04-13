/*************************************************************************
 *
 *  $RCSfile: rtfout.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2005-04-13 11:46:36 $
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

#ifndef _RTFOUT_HXX
#define _RTFOUT_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "svtools/svtdllapi.h"
#endif

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif

class String;
class SvStream;

class SVT_DLLPUBLIC RTFOutFuncs
{
public:
#if defined(MAC) || defined(UNX)
    static const sal_Char sNewLine;     // nur \012 oder \015
#else
    static const sal_Char __FAR_DATA sNewLine[];    // \015\012
#endif

    static SvStream& Out_Char( SvStream&, sal_Unicode cChar,
                    int *pUCMode,
                    rtl_TextEncoding eDestEnc = RTL_TEXTENCODING_MS_1252,
                    BOOL bWriteHelpFile = FALSE );
    static SvStream& Out_String( SvStream&, const String&,
                    rtl_TextEncoding eDestEnc = RTL_TEXTENCODING_MS_1252,
                    BOOL bWriteHelpFile = FALSE );
    static SvStream& Out_Fontname( SvStream&, const String&,
                    rtl_TextEncoding eDestEnc = RTL_TEXTENCODING_MS_1252,
                    BOOL bWriteHelpFile = FALSE );

    static SvStream& Out_Hex( SvStream&, ULONG nHex, BYTE nLen );
};


#endif


