/*************************************************************************
 *
 *  $RCSfile: xiroot.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: obo $ $Date: 2004-10-18 15:16:06 $
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

#ifndef SC_XIROOT_HXX
#include "xiroot.hxx"
#endif

#ifndef SC_ADDINCOL_HXX
#include "addincol.hxx"
#endif

#ifndef SC_XLTRACER_HXX
#include "xltracer.hxx"
#endif
#ifndef SC_XILINK_HXX
#include "xilink.hxx"
#endif
#ifndef SC_XISTYLE_HXX
#include "xistyle.hxx"
#endif
#ifndef SC_XIPAGE_HXX
#include "xipage.hxx"
#endif
#ifndef SC_XICONTENT_HXX
#include "xicontent.hxx"
#endif
#ifndef SC_XIESCHER_HXX
#include "xiescher.hxx"
#endif
#ifndef SC_XIPIVOT_HXX
#include "xipivot.hxx"
#endif

#include "root.hxx"
#include "excimp8.hxx"

// Global data ================================================================

XclImpRootData::XclImpRootData( XclBiff eBiff, SfxMedium& rMedium,
        SotStorageRef xRootStrg, SvStream& rBookStrm, ScDocument& rDoc, CharSet eCharSet ) :
    XclRootData( eBiff, rMedium, xRootStrg, rBookStrm, rDoc, eCharSet, false )
{
}

XclImpRootData::~XclImpRootData()
{
}

// ----------------------------------------------------------------------------

XclImpRoot::XclImpRoot( XclImpRootData& rImpRootData ) :
    XclRoot( rImpRootData ),
    mrImpData( rImpRootData )
{
    mrImpData.mxPalette.reset( new XclImpPalette( GetRoot() ) );
    mrImpData.mxFontBfr.reset( new XclImpFontBuffer( GetRoot() ) );
    mrImpData.mxNumFmtBfr.reset( new XclImpNumFmtBuffer( GetRoot() ) );
    mrImpData.mpXFBfr.reset( new XclImpXFBuffer( GetRoot() ) );
    mrImpData.mxXFRangeBfr.reset( new XclImpXFRangeBuffer( GetRoot() ) );
    mrImpData.mxPageSettings.reset( new XclImpPageSettings( GetRoot() ) );
    mrImpData.mxTabInfo.reset( new XclImpTabInfo );
    mrImpData.mxNameBfr.reset( new XclImpNameBuffer( GetRoot() ) );
    mrImpData.mxLinkMgr.reset( new XclImpLinkManager( GetRoot() ) );
}

XclImpSst& XclImpRoot::GetSst() const
{
    if( !mrImpData.mxSst.get() )
        mrImpData.mxSst.reset( new XclImpSst( GetRoot() ) );
    return *mrImpData.mxSst;
}

XclImpPalette& XclImpRoot::GetPalette() const
{
    return *mrImpData.mxPalette;
}

XclImpFontBuffer& XclImpRoot::GetFontBuffer() const
{
    return *mrImpData.mxFontBfr;
}

XclImpNumFmtBuffer& XclImpRoot::GetNumFmtBuffer() const
{
    return *mrImpData.mxNumFmtBfr;
}

XclImpXFBuffer& XclImpRoot::GetXFBuffer() const
{
    return *mrImpData.mpXFBfr;
}

XclImpXFRangeBuffer& XclImpRoot::GetXFRangeBuffer() const
{
    return *mrImpData.mxXFRangeBfr;
}

XclImpPageSettings& XclImpRoot::GetPageSettings() const
{
    return *mrImpData.mxPageSettings;
}

_ScRangeListTabs& XclImpRoot::GetPrintAreaBuffer() const
{
    return *mpRD->pPrintRanges;
}

_ScRangeListTabs& XclImpRoot::GetTitleAreaBuffer() const
{
    return *mpRD->pPrintTitles;
}

XclImpTabInfo& XclImpRoot::GetTabInfo() const
{
    return *mrImpData.mxTabInfo;
}

XclImpNameBuffer& XclImpRoot::GetNameBuffer() const
{
    return *mrImpData.mxNameBfr;
}

XclImpLinkManager& XclImpRoot::GetLinkManager() const
{
    return *mrImpData.mxLinkMgr;
}

XclImpObjectManager& XclImpRoot::GetObjectManager() const
{
    if( !mrImpData.mxObjMgr.get() )
        mrImpData.mxObjMgr.reset( new XclImpObjectManager( GetRoot() ) );
    return *mrImpData.mxObjMgr;
}

XclImpCondFormatManager& XclImpRoot::GetCondFormatManager() const
{
    if( !mrImpData.mxCondFmtMgr.get() )
        mrImpData.mxCondFmtMgr.reset( new XclImpCondFormatManager( GetRoot() ) );
    return *mrImpData.mxCondFmtMgr;
}

XclImpAutoFilterBuffer& XclImpRoot::GetFilterManager() const
{
    // TODO still in old RootData
    if( !mpRD->pAutoFilterBuffer )
        mpRD->pAutoFilterBuffer = new XclImpAutoFilterBuffer;
    return *mpRD->pAutoFilterBuffer;
}

XclImpWebQueryBuffer& XclImpRoot::GetWebQueryBuffer() const
{
    if( !mrImpData.mxWebQueryBfr.get() )
        mrImpData.mxWebQueryBfr.reset( new XclImpWebQueryBuffer( GetRoot() ) );
    return *mrImpData.mxWebQueryBfr;
}

XclImpPivotTableManager& XclImpRoot::GetPivotTableManager() const
{
    if( !mrImpData.mxPTableMgr.get() )
        mrImpData.mxPTableMgr.reset( new XclImpPivotTableManager( GetRoot() ) );
    return *mrImpData.mxPTableMgr;
}

ExcelToSc& XclImpRoot::GetFmlaConverter() const
{
    return *mpRD->pFmlaConverter;
}

String XclImpRoot::GetScAddInName( const String& rXclName ) const
{
    String aScName;
    if( ScGlobal::GetAddInCollection()->GetCalcName( rXclName, aScName ) )
        return aScName;
    return rXclName;
}

bool XclImpRoot::CheckCellAddress( const ScAddress& rPos ) const
{
    return XclRoot::CheckCellAddress( rPos, GetScMaxPos() );
}

bool XclImpRoot::CheckCellRange( ScRange& rRange ) const
{
    return XclRoot::CheckCellRange( rRange, GetScMaxPos() );
}

void XclImpRoot::CheckCellRangeList( ScRangeList& rRanges ) const
{
    XclRoot::CheckCellRangeList( rRanges, GetScMaxPos() );
}

// ============================================================================

