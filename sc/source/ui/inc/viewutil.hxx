/*************************************************************************
 *
 *  $RCSfile: viewutil.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2004-06-04 11:46:06 $
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

#ifndef SC_VIEWUTIL_HXX
#define SC_VIEWUTIL_HXX

#ifndef SC_ADDRESS_HXX
#include "address.hxx"
#endif

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif
#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

class String;
class SfxItemSet;
class SvxFontItem;

class ScChangeAction;
class ScChangeViewSettings;
class ScDocument;
class ScAddress;


enum ScUpdateMode { SC_UPDATE_ALL, SC_UPDATE_CHANGED, SC_UPDATE_MARKS };

/** Specifiers for sheet conversion (functions iterating over the sheet and modifying cells). */
enum ScConversionType
{
    SC_CONVERSION_SPELLCHECK,       /// Spell checker.
    SC_CONVERSION_HANGULHANJA       /// Hangul-Hanja converter.
};

// ---------------------------------------------------------------------------

class ScViewUtil                                // static Methoden
{
public:
    static BOOL ExecuteCharMap( const SvxFontItem&  rOldFont,
                                SvxFontItem&        rNewFont,
                                String&             rString );

    static BOOL IsActionShown( const ScChangeAction& rAction,
                                const ScChangeViewSettings& rSettings,
                                ScDocument& rDocument );

    static void PutItemScript( SfxItemSet& rShellSet, const SfxItemSet& rCoreSet,
                                USHORT nWhichId, USHORT nScript );

    static USHORT GetEffLanguage( ScDocument* pDoc, const ScAddress& rPos );

    static sal_Int32 GetTransliterationType( USHORT nSlotID );
};

// ---------------------------------------------------------------------------

class ScUpdateRect
{
private:
    SCCOL   nOldStartX;
    SCROW   nOldStartY;
    SCCOL   nOldEndX;
    SCROW   nOldEndY;
    SCCOL   nNewStartX;
    SCROW   nNewStartY;
    SCCOL   nNewEndX;
    SCROW   nNewEndY;
    SCCOL   nContX1;
    SCROW   nContY1;
    SCCOL   nContX2;
    SCROW   nContY2;
public:
            ScUpdateRect( SCCOL nX1, SCROW nY1, SCCOL nX2, SCROW nY2 );
    void    SetNew( SCCOL nX1, SCROW nY1, SCCOL nX2, SCROW nY2 );
    BOOL    GetDiff( SCCOL& rX1, SCROW& rY1, SCCOL& rX2, SCROW& rY2 );
    BOOL    GetXorDiff( SCCOL& rX1, SCROW& rY1, SCCOL& rX2, SCROW& rY2, BOOL& rCont );
    void    GetContDiff( SCCOL& rX1, SCROW& rY1, SCCOL& rX2, SCROW& rY2 );
};




#endif

