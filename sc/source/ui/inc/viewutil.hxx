/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: viewutil.hxx,v $
 * $Revision: 1.12 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef SC_VIEWUTIL_HXX
#define SC_VIEWUTIL_HXX

#include "address.hxx"
#include <tools/solar.h>
#include <sal/types.h>

class String;
class SfxItemSet;
class SfxBindings;
class SvxFontItem;

class ScChangeAction;
class ScChangeViewSettings;
class ScDocument;
class ScAddress;
class ScRange;
class ScMarkData;

enum ScUpdateMode { SC_UPDATE_ALL, SC_UPDATE_CHANGED, SC_UPDATE_MARKS };

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

    static bool HasFiltered( const ScRange& rRange, ScDocument* pDoc );
    static void UnmarkFiltered( ScMarkData& rMark, ScDocument* pDoc );

    static void HideDisabledSlot( SfxItemSet& rSet, SfxBindings& rBindings, USHORT nSlotId );
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

