/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: imapwnd.cxx,v $
 *
 *  $Revision: 1.21 $
 *
 *  last change: $Author: rt $ $Date: 2007-04-26 07:34:40 $
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

#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SV_HELP_HXX //autogen
#include <vcl/help.hxx>
#endif

#ifndef _SFXSIDS_HRC
#include <sfx2/sfxsids.hrc>     // SID_ATTR_MACROITEM
#endif
#define _ANIMATION
#define ITEMID_MACRO SID_ATTR_MACROITEM
#ifndef _MACROPG_HXX //autogen
#include <sfx2/macropg.hxx>
#endif
#ifndef _GOODIES_IMAPRECT_HXX //autogen
#include <svtools/imaprect.hxx>
#endif
#ifndef _GOODIES_IMAPCIRC_HXX //autogen
#include <svtools/imapcirc.hxx>
#endif
#ifndef _GOODIES_IMAPPOLY_HXX //autogen
#include <svtools/imappoly.hxx>
#endif
#ifndef _URLBMK_HXX //autogen
#include <svtools/urlbmk.hxx>
#endif

#include <xoutbmp.hxx>
#include <dialmgr.hxx>
#include <dialogs.hrc>
#include <svxids.hrc>
#include <imapdlg.hrc>
#include <imapwnd.hxx>
#include "svdpage.hxx"
#include "svdorect.hxx"
#include "svdocirc.hxx"
#include "svdopath.hxx"
#include "xfltrit.hxx"
#include "svdpagv.hxx"

#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef _SVX_FILLITEM_HXX //autogen
#include <xfillit.hxx>
#endif
#ifndef _SVX_XLINIIT_HXX //autogen
#include <xlineit.hxx>
#endif

#include <sot/formats.hxx>

#include "svxdlg.hxx"
#include "dialogs.hrc"

#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DPOLYGON_HXX
#include <basegfx/polygon/b2dpolygon.hxx>
#endif

#ifdef MAC
#define TRANSCOL Color( COL_LIGHTGRAY )
#else
#define TRANSCOL Color( COL_WHITE )
#endif

/*************************************************************************
|*
|*  URLDlg
|*
\************************************************************************/
/* move to cui //CHINA001
URLDlg::URLDlg( Window* pWindow, const String& rURL,
                const String& rDescription, const String& rTarget,
                const String& rName, TargetList& rTargetList ) :

    ModalDialog         ( pWindow, SVX_RES( RID_SVXDLG_IMAPURL ) ),

    aFlURL              ( this, ResId( FL_URL ) ),
    aBtnOk              ( this, ResId( BTN_OK ) ),
    aBtnCancel          ( this, ResId( BTN_CANCEL1 ) ),
    aFtURL1             ( this, ResId( FT_URL1 ) ),
    aEdtURL             ( this, ResId( EDT_URL ) ),
    aFtURLDescription   ( this, ResId( FT_URLDESCRIPTION ) ),
    aEdtURLDescription  ( this, ResId( EDT_URLDESCRIPTION ) ),
    aFtTarget           ( this, ResId( FT_TARGET ) ),
    aCbbTargets         ( this, ResId( CBB_TARGETS ) ),
    aFtName             ( this, ResId( FT_NAME ) ),
    aEdtName            ( this, ResId( EDT_NAME ) )

{
    FreeResource();

    aEdtURL.SetText( rURL );
    aEdtURLDescription.SetText( rDescription );
    aEdtName.SetText( rName );

    for( String* pStr = rTargetList.First(); pStr; pStr = rTargetList.Next() )
        aCbbTargets.InsertEntry( *pStr );

    if( !rTarget.Len() )
        aCbbTargets.SetText( String::CreateFromAscii( "_self" ) );
    else
        aCbbTargets.SetText( rTarget );
}
*/
/*************************************************************************
|*
|*
|*
\************************************************************************/

IMapWindow::IMapWindow( Window* pParent, const ResId& rResId ) :
            GraphCtrl( pParent, rResId ),
            DropTargetHelper( this )
{
    SetWinStyle( WB_SDRMODE );

    pItemInfo = new SfxItemInfo[ 1 ];
    memset( pItemInfo, 0, sizeof( SfxItemInfo ) );
    pIMapPool = new SfxItemPool( String::CreateFromAscii( "IMapItemPool" ),
                                 SID_ATTR_MACROITEM, SID_ATTR_MACROITEM, pItemInfo );
    pIMapPool->FreezeIdRanges();
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

IMapWindow::~IMapWindow()
{
    // Liste loeschen
    for( String* pStr = aTargetList.First(); pStr; pStr = aTargetList.Next() )
        delete pStr;

    delete pIMapPool;
    delete[] pItemInfo;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void IMapWindow::SetImageMap( const ImageMap& rImageMap )
{
    ReplaceImageMap( rImageMap, FALSE );
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void IMapWindow::ReplaceImageMap( const ImageMap& rImageMap, BOOL /*bScaleToGraphic*/ )
{
    SdrPage* pPage = 0;
    aIMap = rImageMap;

    if(GetSdrModel())
    {
        // try to access page
        pPage = GetSdrModel()->GetPage(0L);
    }

    if(pPage)
    {
        // clear all draw objects
        pPage->Clear();
    }

    if(GetSdrView())
    {
        // #i63762# reset selection at view
        GetSdrView()->UnmarkAllObj();
    }

    // create new drawing objects
    const USHORT nCount(rImageMap.GetIMapObjectCount());

    for ( USHORT i(nCount); i > 0; i-- )
    {
        SdrObject* pNewObj = CreateObj( rImageMap.GetIMapObject( i - 1 ) );

        if ( pNewObj )
        {
            pPage->InsertObject( pNewObj );
        }
    }
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

BOOL IMapWindow::ReplaceActualIMapInfo( const NotifyInfo& rNewInfo )
{
    const SdrObject*    pSdrObj = GetSelectedSdrObject();
    IMapObject*         pIMapObj;
    BOOL                bRet = FALSE;

    if ( pSdrObj && ( ( pIMapObj = GetIMapObj( pSdrObj ) ) != NULL ) )
    {
        pIMapObj->SetURL( rNewInfo.aMarkURL );
        pIMapObj->SetDescription( rNewInfo.aMarkDescription );
        pIMapObj->SetTarget( rNewInfo.aMarkTarget );
        pModel->SetChanged( sal_True );
        UpdateInfo( FALSE );

        bRet = TRUE;
    }

    return bRet;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

const ImageMap& IMapWindow::GetImageMap()
{
    if ( pModel->IsChanged() )
    {
        SdrPage* pPage = (SdrPage*) pModel->GetPage( 0 );

        if ( pPage )
        {
            const long nCount = pPage->GetObjCount();

            aIMap.ClearImageMap();

            for ( long i = nCount - 1; i > -1; i-- )
                aIMap.InsertIMapObject( *( ( (IMapUserData*) pPage->GetObj( i )->GetUserData( 0 ) )->GetObject() ) );
        }

        pModel->SetChanged( sal_False );
    }

    return aIMap;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void IMapWindow::SetTargetList( TargetList& rTargetList )
{
    String* pStr;

    // alte Liste loeschen
    for( pStr = aTargetList.First(); pStr; pStr = aTargetList.Next() )
        delete pStr;

    aTargetList.Clear();

    // mit uebergebener Liste fuellen
    for( pStr = rTargetList.First(); pStr; pStr = rTargetList.Next() )
        aTargetList.Insert( new String( *pStr ) );

    pModel->SetChanged( sal_False );
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

SdrObject* IMapWindow::CreateObj( const IMapObject* pIMapObj )
{
    Point       aPoint;
    Rectangle   aClipRect( aPoint, GetGraphicSize() );
    SdrObject*  pSdrObj = NULL;
    IMapObject* pCloneIMapObj = NULL;

    switch( pIMapObj->GetType() )
    {
        case( IMAP_OBJ_RECTANGLE ):
        {
            IMapRectangleObject*    pIMapRectObj = (IMapRectangleObject*) pIMapObj;
            Rectangle               aDrawRect( pIMapRectObj->GetRectangle( FALSE ) );

            // auf Zeichenflaeche clippen
            aDrawRect.Intersection( aClipRect );

            pSdrObj = (SdrObject*) new SdrRectObj( aDrawRect );
            pCloneIMapObj = (IMapObject*) new IMapRectangleObject( *pIMapRectObj );
        }
        break;

        case( IMAP_OBJ_CIRCLE ):
        {
            IMapCircleObject*   pIMapCircleObj = (IMapCircleObject*) pIMapObj;
            const Point         aCenter( pIMapCircleObj->GetCenter( FALSE ) );
            const long          nRadius = pIMapCircleObj->GetRadius( FALSE );
            const Point         aOffset( nRadius, nRadius );
            Rectangle           aCircle( aCenter - aOffset, aCenter + aOffset );

            // auf Zeichenflaeche begrenzen
            aCircle.Intersection( aClipRect );

            pSdrObj = (SdrObject*) new SdrCircObj( OBJ_CIRC, aCircle, 0, 36000 );
            pCloneIMapObj = (IMapObject*) new IMapCircleObject( *pIMapCircleObj );
        }
        break;

        case( IMAP_OBJ_POLYGON ):
        {
            IMapPolygonObject*  pIMapPolyObj = (IMapPolygonObject*) pIMapObj;

            // Falls wir eigentlich eine Ellipse sind,
            // erzeugen wir auch wieder eine Ellipse
            if ( pIMapPolyObj->HasExtraEllipse() )
            {
                Rectangle aDrawRect( pIMapPolyObj->GetExtraEllipse() );

                // auf Zeichenflaeche clippen
                aDrawRect.Intersection( aClipRect );

                pSdrObj = (SdrObject*) new SdrCircObj( OBJ_CIRC, aDrawRect, 0, 36000 );
            }
            else
            {
                const Polygon&  rPoly = pIMapPolyObj->GetPolygon( FALSE );
                Polygon         aDrawPoly( rPoly );

                // auf Zeichenflaeche clippen
                aDrawPoly.Clip( aClipRect );

                basegfx::B2DPolygon aPolygon;
                aPolygon.append(aDrawPoly.getB2DPolygon());
                pSdrObj = (SdrObject*)new SdrPathObj(OBJ_POLY, basegfx::B2DPolyPolygon(aPolygon));
            }

            pCloneIMapObj = (IMapObject*) new IMapPolygonObject( *pIMapPolyObj );
        }
        break;

        default:
        break;
    }

    if ( pSdrObj )
    {
        SfxItemSet aSet( pModel->GetItemPool() );

        aSet.Put( XFillStyleItem( XFILL_SOLID ) );
        aSet.Put( XFillColorItem( String(), TRANSCOL ) );

        if ( !pIMapObj->IsActive() )
        {
            aSet.Put( XFillTransparenceItem( 100 ) );
            aSet.Put( XLineColorItem( String(), Color( COL_RED ) ) );
        }
        else
        {
            aSet.Put( XFillTransparenceItem( 50 ) );
            aSet.Put( XLineColorItem( String(), Color( COL_BLACK ) ) );
        }

        //pSdrObj->SetItemSetAndBroadcast(aSet);
        pSdrObj->SetMergedItemSetAndBroadcast(aSet);

        pSdrObj->InsertUserData( new IMapUserData( pCloneIMapObj ) );
        pSdrObj->SetUserCall( GetSdrUserCall() );
    }

    return pSdrObj;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void IMapWindow::InitSdrModel()
{
    GraphCtrl::InitSdrModel();

    SfxItemSet aSet( pModel->GetItemPool() );

    aSet.Put( XFillColorItem( String(), TRANSCOL ) );
    aSet.Put( XFillTransparenceItem( 50 ) );
    pView->SetAttributes( aSet );
    pView->SetFrameDragSingles( TRUE );
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void IMapWindow::SdrObjCreated( const SdrObject& rObj )
{
    switch( rObj.GetObjIdentifier() )
    {
        case( OBJ_RECT ):
        {
            SdrRectObj*          pRectObj = (SdrRectObj*) &rObj;
            IMapRectangleObject* pObj = new IMapRectangleObject( pRectObj->GetLogicRect(),
                                                                 String(), String(), TRUE, FALSE );

            pRectObj->InsertUserData( new IMapUserData( pObj ) );
        }
        break;

        case( OBJ_CIRC ):
        {
            SdrCircObj* pCircObj = (SdrCircObj*) &rObj;
            SdrPathObj* pPathObj = (SdrPathObj*) pCircObj->ConvertToPolyObj( FALSE, FALSE );
            Polygon aPoly(pPathObj->GetPathPoly().getB2DPolygon(0L));
            delete pPathObj;

            IMapPolygonObject* pObj = new IMapPolygonObject( Polygon(aPoly), String(), String(), TRUE, FALSE );
            pObj->SetExtraEllipse( aPoly.GetBoundRect() );
            pCircObj->InsertUserData( new IMapUserData( pObj ) );
        }
        break;

        case( OBJ_POLY ):
        case( OBJ_FREEFILL ):
        case( OBJ_PATHPOLY ):
        case( OBJ_PATHFILL ):
        {
            SdrPathObj* pPathObj = (SdrPathObj*) &rObj;
            const basegfx::B2DPolyPolygon& rXPolyPoly = pPathObj->GetPathPoly();

            if ( rXPolyPoly.count() )
            {
                Polygon aPoly(rXPolyPoly.getB2DPolygon(0L));
                IMapPolygonObject* pObj = new IMapPolygonObject( aPoly, String(), String(), TRUE, FALSE );
                pPathObj->InsertUserData( new IMapUserData( pObj ) );
            }
        }
        break;

        default:
        break;
    }
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void IMapWindow::SdrObjChanged( const SdrObject& rObj )
{
    IMapUserData* pUserData = (IMapUserData*) rObj.GetUserData( 0 );

    if ( pUserData )
    {
        String          aURL;
        String          aText;
        String          aTarget;
        IMapObject*     pIMapObj = pUserData->GetObject();
        BOOL            bActive = TRUE;

        if ( pIMapObj )
        {
            aURL = pIMapObj->GetURL();
            aText = pIMapObj->GetDescription();
            aTarget = pIMapObj->GetTarget();
            bActive = pIMapObj->IsActive();
        }

        switch( rObj.GetObjIdentifier() )
        {
            case( OBJ_RECT ):
            {
                pUserData->ReplaceObject( new IMapRectangleObject( ( (const SdrRectObj&) rObj ).GetLogicRect(),
                                          aURL, aText, aTarget, bActive, FALSE ) );
            }
            break;

            case( OBJ_CIRC ):
            {
                const SdrCircObj& rCircObj = (const SdrCircObj&) rObj;
                SdrPathObj* pPathObj = (SdrPathObj*) rCircObj.ConvertToPolyObj( FALSE, FALSE );
                Polygon aPoly(pPathObj->GetPathPoly().getB2DPolygon(0L));

                IMapPolygonObject* pObj = new IMapPolygonObject( aPoly, aURL, aText, aTarget, bActive, FALSE );
                pObj->SetExtraEllipse( aPoly.GetBoundRect() );

                // wurde von uns nur temporaer angelegt
                delete pPathObj;
                pUserData->ReplaceObject( pObj );
            }
            break;

            case( OBJ_POLY ):
            case( OBJ_FREEFILL ):
            case( OBJ_PATHPOLY ):
            case( OBJ_PATHFILL ):
            {
                const SdrPathObj& rPathObj = (const SdrPathObj&) rObj;
                const basegfx::B2DPolyPolygon& rXPolyPoly = rPathObj.GetPathPoly();

                if ( rXPolyPoly.count() )
                {
                    Polygon aPoly(rPathObj.GetPathPoly().getB2DPolygon(0L));
                    IMapPolygonObject*  pObj = new IMapPolygonObject( aPoly, aURL, aText, aTarget, bActive, FALSE );
                    pUserData->ReplaceObject( pObj );
                }
            }
            break;

            default:
            break;
        }
    }
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void IMapWindow::MouseButtonUp(const MouseEvent& rMEvt)
{
    GraphCtrl::MouseButtonUp( rMEvt );
    UpdateInfo( TRUE );
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void IMapWindow::MarkListHasChanged()
{
    GraphCtrl::MarkListHasChanged();
    UpdateInfo( FALSE );
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

SdrObject* IMapWindow::GetHitSdrObj( const Point& rPosPixel ) const
{
    SdrObject*  pObj = NULL;
    Point       aPt = PixelToLogic( rPosPixel );

    if ( Rectangle( Point(), GetGraphicSize() ).IsInside( aPt ) )
    {
        SdrPage* pPage = (SdrPage*) pModel->GetPage( 0 );
        ULONG    nCount;

        if ( pPage && ( ( nCount = pPage->GetObjCount() ) > 0 ) )
        {
            for ( long i = nCount - 1; i >= 0; i-- )
            {
                SdrObject*  pTestObj = pPage->GetObj( i );
                IMapObject* pIMapObj = GetIMapObj( pTestObj );

                if ( pIMapObj && pIMapObj->IsHit( aPt ) )
                {
                    pObj = pTestObj;
                    break;
                }
            }
        }
    }

    return pObj;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

IMapObject* IMapWindow::GetIMapObj( const SdrObject* pSdrObj ) const
{
    IMapObject* pIMapObj = NULL;

    if ( pSdrObj )
    {
        IMapUserData* pUserData = (IMapUserData*) pSdrObj->GetUserData( 0 );

        if ( pUserData )
            pIMapObj = pUserData->GetObject();
    }

    return pIMapObj;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

SdrObject* IMapWindow::GetSdrObj( const IMapObject* pIMapObj ) const
{
    SdrObject*  pSdrObj = NULL;
    SdrPage*    pPage = (SdrPage*) pModel->GetPage( 0 );
    ULONG       nCount;

    if ( pPage && ( ( nCount = pPage->GetObjCount() ) > 0 ) )
    {
        for ( ULONG i = 0; i < nCount; i++ )
        {
            SdrObject* pTestObj = pPage->GetObj( i );

            if ( pIMapObj == GetIMapObj( pTestObj ) )
            {
                pSdrObj = pTestObj;
                break;
            }
        }
    }

    return pSdrObj;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void IMapWindow::Command(const CommandEvent& rCEvt)
{
    Region  aRegion;

    if ( rCEvt.GetCommand() == COMMAND_CONTEXTMENU )
    {
        PopupMenu           aMenu( SVX_RES( RID_SVXMN_IMAP ) );
        const SdrMarkList&  rMarkList = pView->GetMarkedObjectList();
        ULONG               nMarked = rMarkList.GetMarkCount();

        aMenu.EnableItem( MN_URL, FALSE );
        aMenu.EnableItem( MN_ACTIVATE, FALSE );
        aMenu.EnableItem( MN_MACRO, FALSE );
        aMenu.EnableItem( MN_MARK_ALL, pModel->GetPage( 0 )->GetObjCount() != pView->GetMarkedObjectCount() );

        if ( !nMarked )
        {
            aMenu.EnableItem( MN_POSITION, FALSE );
            aMenu.EnableItem( MN_FRAME_TO_TOP, FALSE );
            aMenu.EnableItem( MN_MOREFRONT, FALSE );
            aMenu.EnableItem( MN_MOREBACK, FALSE );
            aMenu.EnableItem( MN_FRAME_TO_BOTTOM, FALSE );
            aMenu.EnableItem( MN_DELETE1, FALSE );
        }
        else
        {
            if ( nMarked == 1 )
            {
                SdrObject*  pSdrObj = GetSelectedSdrObject();

                aMenu.EnableItem( MN_URL, TRUE );
                aMenu.EnableItem( MN_ACTIVATE, TRUE );
                aMenu.EnableItem( MN_MACRO, TRUE );
                aMenu.CheckItem( MN_ACTIVATE, GetIMapObj( pSdrObj )->IsActive() );
            }

            aMenu.EnableItem( MN_POSITION, TRUE );
            aMenu.EnableItem( MN_FRAME_TO_TOP, TRUE );
            aMenu.EnableItem( MN_MOREFRONT, TRUE );
            aMenu.EnableItem( MN_MOREBACK, TRUE );
            aMenu.EnableItem( MN_FRAME_TO_BOTTOM, TRUE );
            aMenu.EnableItem( MN_DELETE1, TRUE );
        }

        aMenu.SetSelectHdl( LINK( this, IMapWindow, MenuSelectHdl ) );
        aMenu.Execute( this, rCEvt.GetMousePosPixel() );
    }
    else
        Window::Command(rCEvt);
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

sal_Int8 IMapWindow::AcceptDrop( const AcceptDropEvent& rEvt )
{
    return( ( GetHitSdrObj( rEvt.maPosPixel ) != NULL ) ? rEvt.mnAction : DND_ACTION_NONE );
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

sal_Int8 IMapWindow::ExecuteDrop( const ExecuteDropEvent& rEvt )
{
    sal_Int8 nRet = DND_ACTION_NONE;

    if( IsDropFormatSupported( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK ) )
    {
        const String    aString;
        INetBookmark    aBookMark( aString, aString );
        SdrObject*      pSdrObj = GetHitSdrObj( rEvt.maPosPixel );

        if( pSdrObj && TransferableDataHelper( rEvt.maDropEvent.Transferable ).GetINetBookmark( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK, aBookMark ) )
        {
            IMapObject* pIMapObj = GetIMapObj( pSdrObj );

            pIMapObj->SetURL( aBookMark.GetURL() );
            pIMapObj->SetDescription( aBookMark.GetDescription() );
            pModel->SetChanged( sal_True );
            pView->UnmarkAll();
            pView->MarkObj( pSdrObj, pView->GetSdrPageView() );
            UpdateInfo( TRUE );
            nRet =  rEvt.mnAction;
        }
    }

    return nRet;
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void IMapWindow::RequestHelp( const HelpEvent& rHEvt )
{
    SdrObject*          pSdrObj = NULL;
    SdrPageView*        pPageView = NULL;
    Point               aPos = PixelToLogic( ScreenToOutputPixel( rHEvt.GetMousePosPixel() ) );

    if ( Help::IsBalloonHelpEnabled() || Help::IsQuickHelpEnabled() )
    {
        if ( pView->PickObj( aPos, pSdrObj, pPageView ) )
        {
            const IMapObject*   pIMapObj = GetIMapObj( pSdrObj );
            String              aStr;

            if ( pIMapObj && ( aStr = pIMapObj->GetURL() ).Len() )
            {
                String      aDescr( pIMapObj->GetDescription() );
                Rectangle   aLogicPix( LogicToPixel( Rectangle( Point(), GetGraphicSize() ) ) );
                Rectangle   aScreenRect( OutputToScreenPixel( aLogicPix.TopLeft() ),
                                         OutputToScreenPixel( aLogicPix.BottomRight() ) );

                if ( Help::IsBalloonHelpEnabled() )
                    Help::ShowBalloon( this, rHEvt.GetMousePosPixel(), aScreenRect, aStr );
                else if ( Help::IsQuickHelpEnabled() )
                    Help::ShowQuickHelp( this, aScreenRect, aStr );
            }
        }
        else
            Window::RequestHelp( rHEvt );
    }
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void IMapWindow::SetCurrentObjState( BOOL bActive )
{
    SdrObject* pObj = GetSelectedSdrObject();

    if ( pObj )
    {
        SfxItemSet aSet( pModel->GetItemPool() );

        GetIMapObj( pObj )->SetActive( bActive );

        aSet.Put( XFillColorItem( String(), TRANSCOL ) );

        if ( !bActive )
        {
            aSet.Put( XFillTransparenceItem( 100 ) );
            aSet.Put( XLineColorItem( String(), Color( COL_RED ) ) );
        }
        else
        {
            aSet.Put( XFillTransparenceItem( 50 ) );
            aSet.Put( XLineColorItem( String(), Color( COL_BLACK ) ) );
        }

        pView->SetAttributes( aSet, FALSE );
    }
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void IMapWindow::UpdateInfo( BOOL bNewObj )
{
    if ( aInfoLink.IsSet() )
    {
        const SdrObject*    pSdrObj = GetSelectedSdrObject();
        const IMapObject*   pIMapObj = pSdrObj ? GetIMapObj( pSdrObj ) : NULL;

        aInfo.bNewObj = bNewObj;

        if ( pIMapObj )
        {
            aInfo.bOneMarked = TRUE;
            aInfo.aMarkURL = pIMapObj->GetURL();
            aInfo.aMarkDescription = pIMapObj->GetDescription();
            aInfo.aMarkTarget = pIMapObj->GetTarget();
            aInfo.bActivated = pIMapObj->IsActive();
            aInfoLink.Call( this );
        }
        else
        {
            aInfo.aMarkURL = aInfo.aMarkDescription = aInfo.aMarkTarget = String();
            aInfo.bOneMarked = FALSE;
            aInfo.bActivated = FALSE;
        }

        aInfoLink.Call( this );
    }
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void IMapWindow::DoMacroAssign()
{
    SdrObject*  pSdrObj = GetSelectedSdrObject();

    if ( pSdrObj )
    {
        SfxItemSet      aSet( *pIMapPool, SID_ATTR_MACROITEM, SID_ATTR_MACROITEM );
        SvxMacroItem    aMacroItem;
        IMapObject*     pIMapObj = GetIMapObj( pSdrObj );

        aMacroItem.SetMacroTable( pIMapObj->GetMacroTable() );
        aSet.Put( aMacroItem, SID_ATTR_MACROITEM );

        SfxMacroAssignDlg   aMacroDlg( this, aSet );
        SfxMacroTabPage*    pMacroTabPage = (SfxMacroTabPage*) aMacroDlg.GetTabPage();

        if ( pMacroTabPage )
        {
            pMacroTabPage->AddEvent( String::CreateFromAscii( "MouseOver" ), SFX_EVENT_MOUSEOVER_OBJECT );
            pMacroTabPage->AddEvent( String::CreateFromAscii( "MouseOut" ), SFX_EVENT_MOUSEOUT_OBJECT );

            if ( aMacroDlg.Execute() == RET_OK )
            {
                const SfxItemSet* pOutSet = aMacroDlg.GetOutputItemSet();
                pIMapObj->SetMacroTable( ((const SvxMacroItem& )pOutSet->Get( SID_ATTR_MACROITEM )).GetMacroTable() );
                pModel->SetChanged( sal_True );
                UpdateInfo( FALSE );
            }
        }
    }
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void IMapWindow::DoPropertyDialog()
{
    SdrObject*  pSdrObj = GetSelectedSdrObject();

    if ( pSdrObj )
    {
        IMapObject* pIMapObj = GetIMapObj( pSdrObj );
        //CHINA001 URLDlg       aDlg( this, pIMapObj->GetURL(), pIMapObj->GetDescription(),
        //CHINA001                        pIMapObj->GetTarget(), pIMapObj->GetName(), aTargetList );
        SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
        if(pFact)
        {
            AbstractURLDlg* aDlg = pFact->CreateURLDialog( this, pIMapObj->GetURL(), pIMapObj->GetDescription(),
                                            pIMapObj->GetTarget(), pIMapObj->GetName(), aTargetList, RID_SVXDLG_IMAPURL );
            DBG_ASSERT(aDlg, "Dialogdiet fail!");//CHINA001
            if ( aDlg->Execute() == RET_OK ) //CHINA001 if ( aDlg.Execute() == RET_OK )
            {
                const String aURLText( aDlg->GetURL() ); //CHINA001 const String aURLText( aDlg.GetURL() );

                if ( aURLText.Len() )
                {
                    INetURLObject aObj( aURLText, INET_PROT_FILE );
                    DBG_ASSERT( aObj.GetProtocol() != INET_PROT_NOT_VALID, "Invalid URL" );
                    pIMapObj->SetURL( aObj.GetMainURL( INetURLObject::NO_DECODE ) );
                }
                else
                    pIMapObj->SetURL( aURLText );

                pIMapObj->SetDescription( aDlg->GetDescription() ); //CHINA001 pIMapObj->SetDescription( aDlg.GetDescription() );
                pIMapObj->SetTarget( aDlg->GetTarget() ); //CHINA001 pIMapObj->SetTarget( aDlg.GetTarget() );
                pIMapObj->SetName( aDlg->GetName() ); //CHINA001 pIMapObj->SetName( aDlg.GetName() );
                pModel->SetChanged( sal_True );
                UpdateInfo( TRUE );
            }
            delete aDlg; //add by CHINA001
        }
    }
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

IMPL_LINK( IMapWindow, MenuSelectHdl, Menu*, pMenu )
{
    if (pMenu)
    {
        USHORT  nId = pMenu->GetCurItemId();

        switch(nId)
        {
            case( MN_URL ):
                DoPropertyDialog();
            break;

            case( MN_MACRO ):
                DoMacroAssign();
            break;

            case( MN_ACTIVATE ):
            {
                const BOOL bNewState = !pMenu->IsItemChecked( MN_ACTIVATE );

                pMenu->CheckItem( MN_ACTIVATE, bNewState );
                SetCurrentObjState( bNewState );
                UpdateInfo( FALSE );
            }

            case( MN_FRAME_TO_TOP ):
                pView->PutMarkedToTop();
            break;

            case( MN_MOREFRONT ):
                pView->MovMarkedToTop();
            break;

            case( MN_MOREBACK ):
                pView->MovMarkedToBtm();
            break;

            case( MN_FRAME_TO_BOTTOM ):
                pView->PutMarkedToBtm();
            break;

            case( MN_MARK_ALL ):
                pView->MarkAll();
            break;

            case( MN_DELETE1 ):
                pView->DeleteMarked();

            default :
            break;
        }
    }

    return 0;
}

void IMapWindow::CreateDefaultObject()
{
    SdrPageView* pPageView = pView->GetSdrPageView();

    if(pPageView)
    {
        // calc position and size
        Point aPagePos(0, 0); // = pPageView->GetOffset();
        Size aPageSize = pPageView->GetPage()->GetSize();
        sal_uInt32 nDefaultObjectSizeWidth = aPageSize.Width() / 4;
        sal_uInt32 nDefaultObjectSizeHeight = aPageSize.Height() / 4;
        aPagePos.X() += (aPageSize.Width() / 2) - (nDefaultObjectSizeWidth / 2);
        aPagePos.Y() += (aPageSize.Height() / 2) - (nDefaultObjectSizeHeight / 2);
        Rectangle aNewObjectRectangle(aPagePos, Size(nDefaultObjectSizeWidth, nDefaultObjectSizeHeight));

        SdrObject* pObj = SdrObjFactory::MakeNewObject( pView->GetCurrentObjInventor(), pView->GetCurrentObjIdentifier(), 0L, pModel);
        pObj->SetLogicRect(aNewObjectRectangle);

        switch( pObj->GetObjIdentifier() )
        {
        case OBJ_POLY:
        case OBJ_PATHPOLY:
            {
                basegfx::B2DPolygon aInnerPoly;
                aInnerPoly.append(basegfx::B2DPoint(aNewObjectRectangle.BottomLeft().X(), aNewObjectRectangle.BottomLeft().Y()));
                aInnerPoly.append(basegfx::B2DPoint(aNewObjectRectangle.TopLeft().X(), aNewObjectRectangle.TopLeft().Y()));
                aInnerPoly.append(basegfx::B2DPoint(aNewObjectRectangle.TopCenter().X(), aNewObjectRectangle.TopCenter().Y()));
                aInnerPoly.append(basegfx::B2DPoint(aNewObjectRectangle.Center().X(), aNewObjectRectangle.Center().Y()));
                aInnerPoly.append(basegfx::B2DPoint(aNewObjectRectangle.RightCenter().X(), aNewObjectRectangle.RightCenter().Y()));
                aInnerPoly.append(basegfx::B2DPoint(aNewObjectRectangle.BottomRight().X(), aNewObjectRectangle.BottomRight().Y()));
                aInnerPoly.setClosed(true);
                ((SdrPathObj*)pObj)->SetPathPoly(basegfx::B2DPolyPolygon(aInnerPoly));
                break;
            }
        case OBJ_FREEFILL:
        case OBJ_PATHFILL:
            {
                sal_Int32 nWdt(aNewObjectRectangle.GetWidth() / 2);
                sal_Int32 nHgt(aNewObjectRectangle.GetHeight() / 2);
                basegfx::B2DPolygon aInnerPoly(XPolygon(aNewObjectRectangle.Center(), nWdt, nHgt).getB2DPolygon());
                ((SdrPathObj*)pObj)->SetPathPoly(basegfx::B2DPolyPolygon(aInnerPoly));
                break;
            }

        }

        pView->InsertObjectAtView(pObj, *pPageView);
        SdrObjCreated( *pObj );
        SetCurrentObjState( true );
        pView->MarkObj( pObj, pPageView );
    }
}

void IMapWindow::KeyInput( const KeyEvent& rKEvt )
{
    KeyCode aCode = rKEvt.GetKeyCode();

/*
    switch(aCode.GetCode())
    {
        case KEY_ESCAPE:
        {
            if ( pView->IsAction() )
            {
                pView->BrkAction();
                return;
            }
            else if ( pView->AreObjectsMarked() )
            {
                const SdrHdlList& rHdlList = pView->GetHdlList();
                SdrHdl* pHdl = rHdlList.GetFocusHdl();

                if(pHdl)
                {
                    ((SdrHdlList&)rHdlList).ResetFocusHdl();
                }
                else
                {
                    pView->UnmarkAllObj();
                    ((Dialog*)GetParent())->GrabFocusToFirstControl();
                }

                return;
            }
        }
        break;

    }
*/
    GraphCtrl::KeyInput( rKEvt );
}

void IMapWindow::SelectFirstObject()
{
    SdrPage*    pPage = (SdrPage*) pModel->GetPage( 0 );
    if( pPage->GetObjCount() != 0 )
    {
        GrabFocus();
        pView->UnmarkAllObj();
        pView->MarkNextObj(TRUE);
    }
}

void IMapWindow::StartPolyEdit()
{
    GrabFocus();

    if( !pView->AreObjectsMarked() )
        pView->MarkNextObj(TRUE);

    const SdrHdlList& rHdlList = pView->GetHdlList();
    SdrHdl* pHdl = rHdlList.GetFocusHdl();

    if(!pHdl)
    {
        ((SdrHdlList&)rHdlList).TravelFocusHdl(true);
    }
}
