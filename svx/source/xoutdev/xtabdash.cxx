/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xtabdash.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 19:35:43 $
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
#include "precompiled_svx.hxx"

// include ---------------------------------------------------------------

#ifndef SVX_LIGHT

#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _SVX_XPROPERTYTABLE_HXX
#include "XPropertyTable.hxx"
#endif
#ifndef _UNTOOLS_UCBSTREAMHELPER_HXX
#include <unotools/ucbstreamhelper.hxx>
#endif

#include "xmlxtexp.hxx"
#include "xmlxtimp.hxx"

#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

#include <tools/urlobj.hxx>
#include <vcl/virdev.hxx>
#include <vcl/window.hxx>
#include <svtools/itemset.hxx>
#include <sfx2/docfile.hxx>
#include <svx/dialogs.hrc>
#include <svx/dialmgr.hxx>
#include <svx/xtable.hxx>
#include <svx/xpool.hxx>
#include <svx/xoutx.hxx>

#ifndef _SVX_XLINEIT0_HXX //autogen
#include <svx/xlineit0.hxx>
#endif

#ifndef _SVX_XLNCLIT_HXX //autogen
#include <svx/xlnclit.hxx>
#endif

#ifndef _SVX_XLNWTIT_HXX //autogen
#include <svx/xlnwtit.hxx>
#endif

#ifndef _SVX_XLNDSIT_HXX //autogen
#include <svx/xlndsit.hxx>
#endif

using namespace com::sun::star;
using namespace rtl;

#define GLOBALOVERFLOW

sal_Unicode const pszExtDash[]  = {'s','o','d'};
char const aChckDash[]  = { 0x04, 0x00, 'S','O','D','L'};   // < 5.2
char const aChckDash0[] = { 0x04, 0x00, 'S','O','D','0'};   // = 5.2
char const aChckXML[]   = { '<', '?', 'x', 'm', 'l' };      // = 6.0

// -----------------
// class XDashTable
// -----------------

/*************************************************************************
|*
|* XDashTable::XDashTable()
|*
*************************************************************************/

XDashTable::XDashTable( const String& rPath,
                            XOutdevItemPool* pInPool,
                            USHORT nInitSize, USHORT nReSize ) :
                XPropertyTable( rPath, pInPool, nInitSize, nReSize)
{
    pBmpTable = new Table( nInitSize, nReSize );
}

/************************************************************************/

XDashTable::~XDashTable()
{
}

/************************************************************************/

XDashEntry* XDashTable::Replace(long nIndex, XDashEntry* pEntry )
{
    return (XDashEntry*) XPropertyTable::Replace(nIndex, pEntry);
}

/************************************************************************/

XDashEntry* XDashTable::Remove(long nIndex)
{
    return (XDashEntry*) XPropertyTable::Remove(nIndex, 0);
}

/************************************************************************/

XDashEntry* XDashTable::GetDash(long nIndex) const
{
    return (XDashEntry*) XPropertyTable::Get(nIndex, 0);
}

/************************************************************************/

BOOL XDashTable::Load()
{
    return( FALSE );
}

/************************************************************************/

BOOL XDashTable::Save()
{
    return( FALSE );
}

/************************************************************************/

BOOL XDashTable::Create()
{
    return( FALSE );
}

/************************************************************************/

BOOL XDashTable::CreateBitmapsForUI()
{
    return( FALSE );
}

/************************************************************************/

Bitmap* XDashTable::CreateBitmapForUI( long /*nIndex*/, BOOL /*bDelete*/)
{
    return( NULL );
}

// ----------------
// class XDashList
// ----------------

/*************************************************************************
|*
|* XDashList::XDashList()
|*
*************************************************************************/

XDashList::XDashList( const String& rPath,
                            XOutdevItemPool* pInPool,
                            USHORT nInitSize, USHORT nReSize ) :
                XPropertyList   ( rPath, pInPool, nInitSize, nReSize),
                pVD             ( NULL ),
                pXOut           ( NULL ),
                pXFSet          ( NULL ),
                pXLSet          ( NULL )
{
    pBmpList = new List( nInitSize, nReSize );
}

/************************************************************************/

XDashList::~XDashList()
{
    if( pVD )    delete pVD;
    if( pXOut )  delete pXOut;
    if( pXFSet ) delete pXFSet;
    if( pXLSet ) delete pXLSet;
}

/************************************************************************/

XDashEntry* XDashList::Replace(XDashEntry* pEntry, long nIndex )
{
    return (XDashEntry*) XPropertyList::Replace(pEntry, nIndex);
}

/************************************************************************/

XDashEntry* XDashList::Remove(long nIndex)
{
    return (XDashEntry*) XPropertyList::Remove(nIndex, 0);
}

/************************************************************************/

XDashEntry* XDashList::GetDash(long nIndex) const
{
    return (XDashEntry*) XPropertyList::Get(nIndex, 0);
}

/************************************************************************/

BOOL XDashList::Load()
{
    if( bListDirty )
    {
        bListDirty = FALSE;

        INetURLObject aURL( aPath );

        if( INET_PROT_NOT_VALID == aURL.GetProtocol() )
        {
            DBG_ASSERT( !aPath.Len(), "invalid URL" );
            return FALSE;
        }

        aURL.Append( aName );

        if( !aURL.getExtension().getLength() )
            aURL.setExtension( rtl::OUString( pszExtDash, 3 ) );

        uno::Reference< container::XNameContainer > xTable( SvxUnoXDashTable_createInstance( this ), uno::UNO_QUERY );
        return SvxXMLXTableImport::load( aURL.GetMainURL( INetURLObject::NO_DECODE ), xTable );
    }
    return( FALSE );
}

/************************************************************************/

BOOL XDashList::Save()
{
    INetURLObject aURL( aPath );

    if( INET_PROT_NOT_VALID == aURL.GetProtocol() )
    {
        DBG_ASSERT( !aPath.Len(), "invalid URL" );
        return FALSE;
    }

    aURL.Append( aName );

    if( !aURL.getExtension().getLength() )
        aURL.setExtension( rtl::OUString( pszExtDash, 3 ) );

    uno::Reference< container::XNameContainer > xTable( SvxUnoXDashTable_createInstance( this ), uno::UNO_QUERY );
    return SvxXMLXTableExportComponent::save( aURL.GetMainURL( INetURLObject::NO_DECODE ), xTable );
}

/************************************************************************/

BOOL XDashList::Create()
{
    XubString aStr( SVX_RES( RID_SVXSTR_LINESTYLE ) );
    xub_StrLen nLen;

    aStr.AppendAscii(" 1");
    nLen = aStr.Len() - 1;
    Insert(new XDashEntry(XDash(XDASH_RECT,1, 50,1, 50, 50),aStr));
    aStr.SetChar(nLen, sal_Unicode('2'));
    Insert(new XDashEntry(XDash(XDASH_RECT,1,500,1,500,500),aStr));
    aStr.SetChar(nLen, sal_Unicode('3'));
    Insert(new XDashEntry(XDash(XDASH_RECT,2, 50,3,250,120),aStr));

    return( TRUE );
}

/************************************************************************/

BOOL XDashList::CreateBitmapsForUI()
{
    for( long i = 0; i < Count(); i++)
    {
        Bitmap* pBmp = CreateBitmapForUI( i, FALSE );
        DBG_ASSERT( pBmp, "XDashList: Bitmap(UI) konnte nicht erzeugt werden!" );

        if( pBmp )
            pBmpList->Insert( pBmp, i );
    }
    // Loeschen, da JOE den Pool vorm Dtor entfernt!
    if( pVD )   { delete pVD;   pVD = NULL;     }
    if( pXOut ) { delete pXOut; pXOut = NULL;   }
    if( pXFSet ){ delete pXFSet; pXFSet = NULL; }
    if( pXLSet ){ delete pXLSet; pXLSet = NULL; }

    return( TRUE );
}

/************************************************************************/

Bitmap* XDashList::CreateBitmapForUI( long nIndex, BOOL bDelete )
{
    Point   aZero;

    if( !pVD ) // und pXOut und pXFSet und pXLSet
    {
        pVD = new VirtualDevice;
        DBG_ASSERT( pVD, "XDashList: Konnte kein VirtualDevice erzeugen!" );
        pVD->SetMapMode( MAP_100TH_MM );
        pVD->SetOutputSize( pVD->PixelToLogic( Size( BITMAP_WIDTH * 2, BITMAP_HEIGHT ) ) );

        const StyleSettings& rStyles = Application::GetSettings().GetStyleSettings();
        pVD->SetFillColor( rStyles.GetFieldColor() );
        pVD->SetLineColor( rStyles.GetFieldColor() );

        pXOut = new XOutputDevice( pVD );
        DBG_ASSERT( pVD, "XDashList: Konnte kein XOutDevice erzeugen!" );

        pXFSet = new XFillAttrSetItem( pXPool );
        DBG_ASSERT( pVD, "XDashList: Konnte kein XFillAttrSetItem erzeugen!" );

        pXLSet = new XLineAttrSetItem( pXPool );
        DBG_ASSERT( pVD, "XDashList: Konnte kein XLineAttrSetItem erzeugen!" );
        pXLSet->GetItemSet().Put( XLineStyleItem( XLINE_DASH ) );
        pXLSet->GetItemSet().Put( XLineColorItem( String(), RGB_Color( rStyles.GetFieldTextColor().GetColor() ) ) );
        pXLSet->GetItemSet().Put( XLineWidthItem( 30 ) );
    }

    Size aVDSize = pVD->GetOutputSize();
    pVD->DrawRect( Rectangle( aZero, aVDSize ) );
    pXLSet->GetItemSet().Put( XLineDashItem( String(), GetDash( nIndex )->GetDash() ) );

//-/    pXOut->SetLineAttr( *pXLSet );
    pXOut->SetLineAttr( pXLSet->GetItemSet() );

    pXOut->DrawLine( Point( 0, aVDSize.Height() / 2 ),
                     Point( aVDSize.Width(), aVDSize.Height() / 2 ) );

    Bitmap* pBitmap = new Bitmap( pVD->GetBitmap( aZero, aVDSize ) );

    // Loeschen, da JOE den Pool vorm Dtor entfernt!
    if( bDelete )
    {
        if( pVD )   { delete pVD;   pVD = NULL;     }
        if( pXOut ) { delete pXOut; pXOut = NULL;   }
        if( pXFSet ){ delete pXFSet; pXFSet = NULL; }
        if( pXLSet ){ delete pXLSet; pXLSet = NULL; }
    }
    return( pBitmap );
}

// eof
