/*************************************************************************
 *
 *  $RCSfile: xiroot.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: hr $ $Date: 2003-04-23 17:29:54 $
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

#ifdef PCH
#include "filt_pch.hxx"
#endif
#pragma hdrstop

// ============================================================================

#ifndef SC_XIROOT_HXX
#include "xiroot.hxx"
#endif

#ifndef SC_ADDINCOL_HXX
#include "addincol.hxx"
#endif

#ifndef SC_XILINK_HXX
#include "xilink.hxx"
#endif
#ifndef SC_XISTYLE_HXX
#include "xistyle.hxx"
#endif
#ifndef SC_XICONTENT_HXX
#include "xicontent.hxx"
#endif

#include "XclImpObjects.hxx"


// Global data ================================================================

XclImpRootData::XclImpRootData( XclBiff eBiff, ScDocument& rDocument, const String& rBasePath, CharSet eCharSet ) :
    XclRootData( eBiff, rDocument, rBasePath, eCharSet )
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
    mrImpData.mpPalette.reset( new XclImpPalette( GetRoot() ) );
    mrImpData.mpFontBuffer.reset( new XclImpFontBuffer( GetRoot() ) );
    mrImpData.mpNumFmtBuffer.reset( new XclImpNumFmtBuffer( GetRoot() ) );
    mrImpData.mpXFBuffer.reset( new XclImpXFBuffer( GetRoot() ) );
    mrImpData.mpXFIndexBuffer.reset( new XclImpXFIndexBuffer( GetRoot() ) );
    mrImpData.mpTabIdBuffer.reset( new XclImpTabIdBuffer );
    mrImpData.mpLinkManager.reset( new XclImpLinkManager( GetRoot() ) );
}

XclImpRoot::XclImpRoot( const XclImpRoot& rRoot ) :
    XclRoot( rRoot ),
    mrImpData( rRoot.mrImpData )
{
}

XclImpRoot& XclImpRoot::operator=( const XclImpRoot& rRoot )
{
    XclRoot::operator=( rRoot );
    return *this;
}

void XclImpRoot::SetBiff( XclBiff eBiff )
{
    XclRoot::SetBiff( eBiff );
    GetPalette().OnChangeBiff();
}

XclImpSst& XclImpRoot::GetSst() const
{
    if( !mrImpData.mpSst.get() )
        mrImpData.mpSst.reset( new XclImpSst( GetRoot() ) );
    return *mrImpData.mpSst;
}

XclImpPalette& XclImpRoot::GetPalette() const
{
    return *mrImpData.mpPalette;
}

XclImpFontBuffer& XclImpRoot::GetFontBuffer() const
{
    return *mrImpData.mpFontBuffer;
}

XclImpNumFmtBuffer& XclImpRoot::GetNumFmtBuffer() const
{
    return *mrImpData.mpNumFmtBuffer;
}

XclImpXFBuffer& XclImpRoot::GetXFBuffer() const
{
    return *mrImpData.mpXFBuffer;
}

XclImpXFIndexBuffer& XclImpRoot::GetXFIndexBuffer() const
{
    return *mrImpData.mpXFIndexBuffer;
}

XclImpTabIdBuffer& XclImpRoot::GetTabIdBuffer() const
{
    return *mrImpData.mpTabIdBuffer;
}

XclImpLinkManager& XclImpRoot::GetLinkManager() const
{
    return *mrImpData.mpLinkManager;
}

XclImpObjectManager& XclImpRoot::GetObjectManager() const
{
    if( !mrImpData.mpObjManager.get() )
        mrImpData.mpObjManager.reset( new XclImpObjectManager( GetRoot() ) );
    return *mrImpData.mpObjManager;
}

XclImpWebQueryBuffer& XclImpRoot::GetWebQueryBuffer() const
{
    if( !mrImpData.mpWebQBuffer.get() )
        mrImpData.mpWebQBuffer.reset( new XclImpWebQueryBuffer( GetRoot() ) );
    return *mrImpData.mpWebQBuffer;
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

