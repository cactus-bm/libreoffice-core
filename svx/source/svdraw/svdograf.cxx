/*************************************************************************
 *
 *  $RCSfile: svdograf.cxx,v $
 *
 *  $Revision: 1.49 $
 *
 *  last change: $Author: ka $ $Date: 2002-08-14 11:10:28 $
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

#define _ANIMATION
#define ITEMID_GRF_CROP 0

#ifndef SVX_LIGHT
#include <so3/lnkbase.hxx>
#else
#include <svtools/pathoptions.hxx>
#endif

#include <math.h>
#include <vcl/salbtype.hxx>
#include <sot/formats.hxx>
#include <so3/svstor.hxx>
#include <unotools/ucbstreamhelper.hxx>
#include <unotools/localfilehelper.hxx>
#include <svtools/style.hxx>
#include <svtools/filter.hxx>
#include <svtools/urihelper.hxx>
#include <goodies/grfmgr.hxx>
#include "linkmgr.hxx"
#include "svdxout.hxx"
#include "svdio.hxx"
#include "svdetc.hxx"
#include "svdglob.hxx"
#include "svdstr.hrc"
#include "svdpool.hxx"
#include "svdmodel.hxx"
#include "svdpage.hxx"
#include "svdmrkv.hxx"
#include "svdpagv.hxx"
#include "svdviter.hxx"
#include "svdview.hxx"
#include "impgrf.hxx"
#include "svdograf.hxx"
#include "svdogrp.hxx"
#include "xbitmap.hxx"
#include "xbtmpit.hxx"
#include "xflbmtit.hxx"
#include "svdundo.hxx"
#include "svdfmtf.hxx"
#include "sdgcpitm.hxx"

#ifndef _EEITEM_HXX
#include "eeitem.hxx"
#endif

// -----------
// - Defines -
// -----------

#define GRAFSTREAMPOS_INVALID 0xffffffff

#ifndef SVX_LIGHT

// ------------------
// - SdrGraphicLink -
// ------------------

class SdrGraphicLink : public so3::SvBaseLink
{
    SdrGrafObj*         pGrafObj;

public:
                        SdrGraphicLink(SdrGrafObj* pObj);
    virtual             ~SdrGraphicLink();

    virtual void        Closed();
    virtual void        DataChanged( const String& rMimeType,
                                const ::com::sun::star::uno::Any & rValue );

    BOOL                Connect() { return 0 != GetRealObject(); }
    void                UpdateSynchron();
};

// -----------------------------------------------------------------------------

SdrGraphicLink::SdrGraphicLink(SdrGrafObj* pObj):
    ::so3::SvBaseLink( ::so3::LINKUPDATE_ONCALL, SOT_FORMATSTR_ID_SVXB ),
    pGrafObj(pObj)
{
    SetSynchron( FALSE );
}

// -----------------------------------------------------------------------------

SdrGraphicLink::~SdrGraphicLink()
{
}

// -----------------------------------------------------------------------------

void SdrGraphicLink::DataChanged( const String& rMimeType,
                                const ::com::sun::star::uno::Any & rValue )
{
    SdrModel*       pModel      = pGrafObj ? pGrafObj->GetModel() : 0;
    SvxLinkManager* pLinkManager= pModel  ? pModel->GetLinkManager() : 0;

    if( pLinkManager && rValue.hasValue() )
    {
        pLinkManager->GetDisplayNames( this, 0, &pGrafObj->aFileName, 0, &pGrafObj->aFilterName );

        Graphic aGraphic;
        if( SvxLinkManager::GetGraphicFromAny( rMimeType, rValue, aGraphic ))
        {
            GraphicType eOldGraphicType = pGrafObj->GetGraphicType();  // kein Hereinswappen
            BOOL bIsChanged = pModel->IsChanged();

            pGrafObj->SetGraphic( aGraphic );
            if( GRAPHIC_NONE != eOldGraphicType )
                pGrafObj->SetChanged();
            else
                pModel->SetChanged( bIsChanged );
        }
        else if( SotExchange::GetFormatIdFromMimeType( rMimeType ) !=
                    SvxLinkManager::RegisterStatusInfoId() )
            pGrafObj->SendRepaintBroadcast();
    }
}

// -----------------------------------------------------------------------------

void SdrGraphicLink::Closed()
{
    // Die Verbindung wird aufgehoben; pLink des Objekts auf NULL setzen, da die Link-Instanz ja gerade destruiert wird.
    pGrafObj->ForceSwapIn();
    pGrafObj->pGraphicLink=NULL;
    pGrafObj->ReleaseGraphicLink();
    SvBaseLink::Closed();
}

// -----------------------------------------------------------------------------

void SdrGraphicLink::UpdateSynchron()
{
    if( GetObj() )
    {
        String aMimeType( SotExchange::GetFormatMimeType( GetContentType() ));
        ::com::sun::star::uno::Any aValue;
        GetObj()->GetData( aValue, aMimeType, TRUE );
        DataChanged( aMimeType, aValue );
    }
}

#else

GraphicFilter* SVX_LIGHT_pGrapicFilter = NULL;

GraphicFilter* GetGrfFilter()
{
    if( !SVX_LIGHT_pGrapicFilter )
    {
        const SvtPathOptions aPathOptions;

        SVX_LIGHT_pGrapicFilter = new GraphicFilter( FALSE );
        SVX_LIGHT_pGrapicFilter->SetFilterPath( aPathOptions.GetFilterPath() );
    }

    return SVX_LIGHT_pGrapicFilter;
}


#endif // SVX_LIGHT

// --------------
// - SdrGrafObj -
// --------------

TYPEINIT1(SdrGrafObj,SdrRectObj);

// -----------------------------------------------------------------------------

SdrGrafObj::SdrGrafObj():
    bMirrored       ( FALSE ),
    pGraphicLink    ( NULL )
{
    pGraphic = new GraphicObject;
    pGraphic->SetSwapStreamHdl( LINK( this, SdrGrafObj, ImpSwapHdl ), Application::IsRemoteServer() ? 60000 : 20000 );
    nGrafStreamPos = GRAFSTREAMPOS_INVALID;
    bNoShear = TRUE;
    bCopyToPoolOnAfterRead = FALSE;
}

// -----------------------------------------------------------------------------

SdrGrafObj::SdrGrafObj(const Graphic& rGrf, const Rectangle& rRect):
    SdrRectObj      ( rRect ),
    bMirrored       ( FALSE ),
    pGraphicLink    ( NULL )
{
    pGraphic = new GraphicObject( rGrf );
    pGraphic->SetSwapStreamHdl( LINK( this, SdrGrafObj, ImpSwapHdl ), Application::IsRemoteServer() ? 60000 : 20000 );
    nGrafStreamPos = GRAFSTREAMPOS_INVALID;
    bNoShear = TRUE;
    bCopyToPoolOnAfterRead = FALSE;
}

// -----------------------------------------------------------------------------

SdrGrafObj::SdrGrafObj( const Graphic& rGrf ):
    bMirrored       ( FALSE ),
    pGraphicLink    ( NULL )
{
    pGraphic = new GraphicObject( rGrf );
    pGraphic->SetSwapStreamHdl( LINK( this, SdrGrafObj, ImpSwapHdl ), Application::IsRemoteServer() ? 60000 : 20000 );
    nGrafStreamPos = GRAFSTREAMPOS_INVALID;
    bNoShear = TRUE;
    bCopyToPoolOnAfterRead = FALSE;
}

// -----------------------------------------------------------------------------

SdrGrafObj::~SdrGrafObj()
{
    delete pGraphic;
    ImpLinkAbmeldung();
}

// -----------------------------------------------------------------------------

void SdrGrafObj::SetGraphicObject( const GraphicObject& rGrfObj )
{
    *pGraphic = rGrfObj;
    pGraphic->SetSwapStreamHdl( LINK( this, SdrGrafObj, ImpSwapHdl ), Application::IsRemoteServer() ? 60000 : 20000 );
    pGraphic->SetUserData();
    nGrafStreamPos = GRAFSTREAMPOS_INVALID;
    SetChanged();
    SendRepaintBroadcast();
}

// -----------------------------------------------------------------------------

const GraphicObject& SdrGrafObj::GetGraphicObject() const
{
    ForceSwapIn();
    return *pGraphic;
}

// -----------------------------------------------------------------------------

void SdrGrafObj::SetGraphic( const Graphic& rGrf )
{
    pGraphic->SetGraphic( rGrf );
    pGraphic->SetUserData();
    nGrafStreamPos = GRAFSTREAMPOS_INVALID;
    SetChanged();
    SendRepaintBroadcast();
}

// -----------------------------------------------------------------------------

const Graphic& SdrGrafObj::GetGraphic() const
{
    ForceSwapIn();
    return pGraphic->GetGraphic();
}

// -----------------------------------------------------------------------------

Graphic SdrGrafObj::GetTransformedGraphic( ULONG nTransformFlags ) const
{
    Graphic         aTransGraphic( GetGraphic() );
    GraphicType     eType = GetGraphicType();
    const MapMode   aDstMap( pModel->GetScaleUnit(), Point(), pModel->GetScaleFraction(), pModel->GetScaleFraction() );
    const Size      aSrcSize( aTransGraphic.GetPrefSize() );
    const Size      aDstSize( GetLogicRect().GetSize() );
    const BOOL      bMirror = ( nTransformFlags & SDRGRAFOBJ_TRANSFORMATTR_MIRROR ) != 0;
    const BOOL      bRotate = ( ( ( nTransformFlags & SDRGRAFOBJ_TRANSFORMATTR_ROTATE ) != 0 ) &&
                                ( aGeo.nDrehWink && aGeo.nDrehWink != 18000 ) && ( GRAPHIC_NONE != eType ) && !IsAnimated() );

    if( GRAPHIC_GDIMETAFILE == eType )
    {
        GDIMetaFile aMtf( aTransGraphic.GetGDIMetaFile() );

        aMtf.Scale( Fraction( aDstSize.Width(), aSrcSize.Width() ), Fraction( aDstSize.Height(), aSrcSize.Height() ) );
        aMtf.SetPrefMapMode( aDstMap );

        aTransGraphic = aMtf;
    }
    else if( GRAPHIC_BITMAP == eType )
    {
        if( bRotate && !IsAnimated() )
        {
            BitmapEx    aBmpEx( aTransGraphic.GetBitmapEx() );
            const Size  aSizePixel( aBmpEx.GetSizePixel() );

            if( aSizePixel.Width() && aSizePixel.Height() && aDstSize.Width() && aDstSize.Height() )
            {
                double fSrcWH = (double) aSizePixel.Width() / aSizePixel.Height();
                double fDstWH = (double) aDstSize.Width() / aDstSize.Height();
                double fScaleX = 1.0, fScaleY = 1.0;

                // always choose scaling to shrink bitmap
                if( fSrcWH < fDstWH )
                    fScaleY = aSizePixel.Width() / ( fDstWH * aSizePixel.Height() );
                else
                    fScaleX = fDstWH * aSizePixel.Height() / aSizePixel.Width();

                aBmpEx.Scale( fScaleX, fScaleY );
                aTransGraphic = aBmpEx;
            }
        }

        aTransGraphic.SetPrefSize( aDstSize );
        aTransGraphic.SetPrefMapMode( aDstMap );
    }

    if( ( SDRGRAFOBJ_TRANSFORMATTR_NONE != nTransformFlags ) && ( GRAPHIC_NONE != eType ) )
    {
        ( (SdrGrafObj*) this )->ImpSetAttrToGrafInfo();
        GraphicAttr aActAttr( aGrafInfo );

        if( bMirror )
        {
            USHORT      nMirrorCase = ( aGeo.nDrehWink == 18000 ) ? ( bMirrored ? 3 : 4 ) : ( bMirrored ? 2 : 1 );
            FASTBOOL    bHMirr = nMirrorCase == 2 || nMirrorCase == 4;
            FASTBOOL    bVMirr = nMirrorCase == 3 || nMirrorCase == 4;

            aActAttr.SetMirrorFlags( ( bHMirr ? BMP_MIRROR_HORZ : 0 ) | ( bVMirr ? BMP_MIRROR_VERT : 0 ) );
        }

        if( bRotate )
            aActAttr.SetRotation( aGeo.nDrehWink / 10 );

        GraphicObject aGrfObj( aTransGraphic );
        aTransGraphic = aGrfObj.GetTransformedGraphic( &aActAttr );
    }

    return aTransGraphic;
}

// -----------------------------------------------------------------------------

GraphicType SdrGrafObj::GetGraphicType() const
{
    return pGraphic->GetType();
}

// -----------------------------------------------------------------------------

FASTBOOL SdrGrafObj::IsAnimated() const
{
    return pGraphic->IsAnimated();
}

// -----------------------------------------------------------------------------

FASTBOOL SdrGrafObj::IsTransparent() const
{
    return pGraphic->IsTransparent();
}

// -----------------------------------------------------------------------------

FASTBOOL SdrGrafObj::IsEPS() const
{
    return pGraphic->IsEPS();
}

// -----------------------------------------------------------------------------

const MapMode& SdrGrafObj::GetGrafPrefMapMode() const
{
    return pGraphic->GetPrefMapMode();
}

// -----------------------------------------------------------------------------

const Size& SdrGrafObj::GetGrafPrefSize() const
{
    return pGraphic->GetPrefSize();
}

// -----------------------------------------------------------------------------

void SdrGrafObj::SetGrafStreamURL( const String& rGraphicStreamURL )
{
    if( !rGraphicStreamURL.Len() )
    {
        pGraphic->SetUserData();
        nGrafStreamPos = GRAFSTREAMPOS_INVALID;
    }
    else if( pModel->IsSwapGraphics() && ( pModel->GetSwapGraphicsMode() & SDR_SWAPGRAPHICSMODE_DOC ) )
    {
        pGraphic->SetUserData( rGraphicStreamURL );
        nGrafStreamPos = GRAFSTREAMPOS_INVALID;

        // set state of graphic object to 'swapped out'
        if( pGraphic->GetType() == GRAPHIC_NONE )
            pGraphic->SetSwapState();
    }
}

// -----------------------------------------------------------------------------

String SdrGrafObj::GetGrafStreamURL() const
{
    return pGraphic->GetUserData();
}

// -----------------------------------------------------------------------------

void SdrGrafObj::SetFileName(const String& rFileName)
{
    aFileName = rFileName;
    SetChanged();
}

// -----------------------------------------------------------------------------

void SdrGrafObj::SetFilterName(const String& rFilterName)
{
    aFilterName = rFilterName;
    SetChanged();
}

// -----------------------------------------------------------------------------

FASTBOOL SdrGrafObj::HasSetName() const
{
    return TRUE;
}

// -----------------------------------------------------------------------------

void SdrGrafObj::SetName(const XubString& rStr)
{
    aName = rStr;
    SetChanged();
}

// -----------------------------------------------------------------------------

XubString SdrGrafObj::GetName() const
{
    return aName;
}

// -----------------------------------------------------------------------------

void SdrGrafObj::ForceSwapIn() const
{
    pGraphic->FireSwapInRequest();

    if( pGraphic->IsSwappedOut() ||
        ( pGraphic->GetType() == GRAPHIC_NONE ) ||
        ( pGraphic->GetType() == GRAPHIC_DEFAULT ) )
    {
        Graphic aDefaultGraphic;
        aDefaultGraphic.SetDefaultType();
        pGraphic->SetGraphic( aDefaultGraphic );
    }
}

// -----------------------------------------------------------------------------

void SdrGrafObj::ForceSwapOut() const
{
    pGraphic->FireSwapOutRequest();
}

// -----------------------------------------------------------------------------

void SdrGrafObj::ImpLinkAnmeldung()
{
#ifndef SVX_LIGHT

    SvxLinkManager* pLinkManager = pModel != NULL ? pModel->GetLinkManager() : NULL;

    if( pLinkManager != NULL && pGraphicLink == NULL )
    {
        if( aFileName.Len() )
        {
            pGraphicLink = new SdrGraphicLink( this );
            pLinkManager->InsertFileLink( *pGraphicLink, OBJECT_CLIENT_GRF, aFileName, ( aFilterName.Len() ? &aFilterName : NULL ), NULL );
            pGraphicLink->Connect();
        }
    }

#endif // SVX_LIGHT
}

// -----------------------------------------------------------------------------

void SdrGrafObj::ImpLinkAbmeldung()
{
#ifndef SVX_LIGHT

    SvxLinkManager* pLinkManager = pModel != NULL ? pModel->GetLinkManager() : NULL;

    if( pLinkManager != NULL && pGraphicLink!=NULL)
    {
        // Bei Remove wird *pGraphicLink implizit deleted
        pLinkManager->Remove( pGraphicLink );
        pGraphicLink=NULL;
    }

#endif // SVX_LIGHT
}

// -----------------------------------------------------------------------------

void SdrGrafObj::SetGraphicLink( const String& rFileName, const String& rFilterName )
{
    ImpLinkAbmeldung();
    aFileName = rFileName;
    aFilterName = rFilterName;
    ImpLinkAnmeldung();
    pGraphic->SetUserData();

    // #92205# A linked graphic is per definition swapped out (has to be loaded)
    pGraphic->SetSwapState();
}

// -----------------------------------------------------------------------------

void SdrGrafObj::ReleaseGraphicLink()
{
    ImpLinkAbmeldung();
    aFileName = String();
    aFilterName = String();
}

// -----------------------------------------------------------------------------

void SdrGrafObj::TakeObjInfo(SdrObjTransformInfoRec& rInfo) const
{
    FASTBOOL bTrans = pGraphic->IsTransparent();
    FASTBOOL bAnim = pGraphic->IsAnimated();
    FASTBOOL bNoPresGrf = ( pGraphic->GetType() != GRAPHIC_NONE ) && !bEmptyPresObj;

    rInfo.bResizeFreeAllowed = aGeo.nDrehWink % 9000 == 0 ||
                               aGeo.nDrehWink % 18000 == 0 ||
                               aGeo.nDrehWink % 27000 == 0;

    rInfo.bResizePropAllowed = TRUE;
    rInfo.bRotateFreeAllowed = bNoPresGrf && !bAnim;
    rInfo.bRotate90Allowed = bNoPresGrf && !bAnim;
    rInfo.bMirrorFreeAllowed = bNoPresGrf && !bAnim;
    rInfo.bMirror45Allowed = bNoPresGrf && !bAnim;
    rInfo.bMirror90Allowed = !bEmptyPresObj;
    rInfo.bTransparenceAllowed = FALSE;
    rInfo.bGradientAllowed = FALSE;
    rInfo.bShearAllowed = FALSE;
    rInfo.bEdgeRadiusAllowed=FALSE;
    rInfo.bCanConvToPath = FALSE;
    rInfo.bCanConvToPathLineToArea = FALSE;
    rInfo.bCanConvToPolyLineToArea = FALSE;
    rInfo.bCanConvToPoly = !IsEPS();
    rInfo.bCanConvToContour = (rInfo.bCanConvToPoly || LineGeometryUsageIsNecessary());
}

// -----------------------------------------------------------------------------

UINT16 SdrGrafObj::GetObjIdentifier() const
{
    return UINT16( OBJ_GRAF );
}

// -----------------------------------------------------------------------------

sal_Bool SdrGrafObj::ImpUpdateGraphicLink() const
{
    sal_Bool    bRet = sal_False;

#ifndef SVX_LIGHT
    if( pGraphicLink )
    {
        BOOL bIsChanged = pModel->IsChanged();
        pGraphicLink->UpdateSynchron();
        pModel->SetChanged( bIsChanged );

        bRet = sal_True;
    }
#else
    if( aFileName.Len() )
    {
        // #92205# Load linked graphics for player
        SvStream* pIStm = ::utl::UcbStreamHelper::CreateStream( aFileName, STREAM_READ | STREAM_SHARE_DENYNONE );

        if( pIStm )
        {
            Graphic         aGraphic;
            GraphicFilter*  pFilter = GetGrfFilter();
            USHORT          nError = pFilter->ImportGraphic( aGraphic, aFileName, *pIStm );

            pGraphic->SetGraphic( aGraphic );

            delete pIStm;
        }

        bRet = sal_True;
    }
#endif

    return bRet;
}

// -----------------------------------------------------------------------------

// Liefert FALSE, wenn die Pres-Bitmap zu gross ist
FASTBOOL SdrGrafObj::ImpPaintEmptyPres( OutputDevice* pOutDev ) const
{
    const MapMode   aDstMapMode( pOutDev->GetMapMode().GetMapUnit() );
    Point           aPos( aRect.Center() );
    Size            aSize;
    FASTBOOL        bRet;

    if( pGraphic->GetPrefMapMode().GetMapUnit() == MAP_PIXEL )
        aSize = pOutDev->PixelToLogic( pGraphic->GetPrefSize(), aDstMapMode );
    else
        aSize = pOutDev->LogicToLogic( pGraphic->GetPrefSize(), pGraphic->GetPrefMapMode(), aDstMapMode );

    aPos.X() -= ( aSize.Width() >> 1 );
    aPos.Y() -= ( aSize.Height() >> 1 );

    if( aPos.X() >= aRect.Left() && aPos.Y() >= aRect.Top() )
    {
        const Graphic& rGraphic = pGraphic->GetGraphic();

        if( pGraphic->GetType() == GRAPHIC_BITMAP )
            pGraphic->Draw( pOutDev, aPos, aSize, NULL );
        else
        {
            const ULONG nOldDrawMode = pOutDev->GetDrawMode();

            if( ( nOldDrawMode & DRAWMODE_GRAYBITMAP ) != 0 )
            {
                // Falls Modus GRAYBITMAP, wollen wir auch Mtf's als Graustufen darstellen
                ULONG nNewDrawMode = nOldDrawMode;
                nNewDrawMode &= ~( DRAWMODE_BLACKLINE | DRAWMODE_BLACKFILL | DRAWMODE_WHITEFILL | DRAWMODE_NOFILL );
                pOutDev->SetDrawMode( nNewDrawMode |= DRAWMODE_GRAYLINE | DRAWMODE_GRAYFILL  );
            }

            rGraphic.Draw( pOutDev, aPos, aSize );
            pOutDev->SetDrawMode( nOldDrawMode );
        }

        bRet = TRUE;
    }
    else
        bRet = FALSE;

    return bRet;
}

// -----------------------------------------------------------------------------

void SdrGrafObj::ImpPaintReplacement(OutputDevice* pOutDev, const XubString& rText, const Bitmap* pBmp, FASTBOOL bFill) const
{
    if( bFill )
    {
        pOutDev->SetLineColor();
        pOutDev->SetFillColor( COL_LIGHTGRAY );
    }

    Rectangle aRect1(pOutDev->LogicToPixel(aRect));
    Rectangle aTextRect(aRect1);
    Rectangle aRect2(aRect1);
    aRect2.Left()++; aRect2.Top()++; aRect2.Right()--; aRect2.Bottom()--;
    Point aTopLeft(aRect1.TopLeft());
    Point aBmpPos(aTopLeft);

    if( pBmp != NULL )
    {
        Size        aBmpSize(pBmp->GetSizePixel());
        long        nRectWdt=aTextRect.Right()-aTextRect.Left();
        long        nRectHgt=aTextRect.Bottom()-aTextRect.Top();
        long        nBmpWdt=aBmpSize.Width();
        long        nBmpHgt=aBmpSize.Height();
        BOOL        bText = rText.Len() > 0;

        long nMinWdt = nBmpWdt;
        long nMinHgt = nBmpHgt;

        if( bText )
        {
            nMinWdt=2 + nBmpWdt * 2 + 3;
            nMinHgt=2 + nBmpHgt * 2 + 3;
        }

        if( nRectWdt < nMinWdt || nRectHgt < nMinHgt )
            pBmp=NULL;
        else
        {
            aTextRect.Left() += nBmpWdt;

            if( bText )
                aTextRect.Left()+=2+3;
        }

        aBmpPos.X() += 2;
        aBmpPos.Y() += 2;

        if( aGeo.nDrehWink!=0 )
        {
            Point aRef(aBmpPos);
            aRef.X()-=aBmpSize.Width()/2+2;
            aRef.Y()-=aBmpSize.Height()/2+2;
            double nSin=sin(aGeo.nDrehWink*nPi180);
            double nCos=cos(aGeo.nDrehWink*nPi180);
            RotatePoint(aBmpPos,aRef,nSin,nCos);
        }
    }

    const BOOL bOldMap = pOutDev->IsMapModeEnabled();
    pOutDev->EnableMapMode( FALSE );

    if( aGeo.nDrehWink == 0 && aGeo.nShearWink == 0 )
    {
        if (bFill)
            pOutDev->DrawRect(aRect);

        if (pBmp!=NULL)
            pOutDev->DrawBitmap(aBmpPos,*pBmp);

        const StyleSettings& rStyleSettings = pOutDev->GetSettings().GetStyleSettings();

        pOutDev->SetFillColor();
        pOutDev->SetLineColor( rStyleSettings.GetShadowColor() );
        pOutDev->DrawLine(aRect1.TopLeft(),aRect1.TopRight()); pOutDev->DrawLine(aRect1.TopLeft(),aRect1.BottomLeft());


        pOutDev->SetLineColor( rStyleSettings.GetLightColor() );
        pOutDev->DrawLine(aRect1.TopRight(),aRect1.BottomRight()); pOutDev->DrawLine(aRect1.BottomLeft(),aRect1.BottomRight());

        pOutDev->SetLineColor( rStyleSettings.GetLightColor() );
        pOutDev->DrawLine(aRect2.TopLeft(),aRect2.TopRight()); pOutDev->DrawLine(aRect2.TopLeft(),aRect2.BottomLeft());

        pOutDev->SetLineColor( rStyleSettings.GetShadowColor() );
        pOutDev->DrawLine(aRect2.TopRight(),aRect2.BottomRight()); pOutDev->DrawLine(aRect2.BottomLeft(),aRect2.BottomRight());
    }
    else
    {
        Polygon aPoly1(Rect2Poly(aRect1,aGeo));
        Polygon aPoly2(5);
        aPoly2[0]=aRect2.TopLeft();
        aPoly2[1]=aRect2.TopRight();
        aPoly2[2]=aRect2.BottomRight();
        aPoly2[3]=aRect2.BottomLeft();
        aPoly2[4]=aRect2.TopLeft();

        if (aGeo.nShearWink != 0)
            ShearPoly(aPoly2,aTopLeft,aGeo.nTan);

        if( aGeo.nDrehWink != 0 )
            RotatePoly(aPoly2,aTopLeft,aGeo.nSin,aGeo.nCos);

        if( bFill )
            pOutDev->DrawPolygon(aPoly1);

        if( pBmp != NULL )
            pOutDev->DrawBitmap( aBmpPos, *pBmp );

        pOutDev->SetFillColor();

        long        nHWink=NormAngle360(aGeo.nDrehWink);
        long        nVWink=NormAngle360(aGeo.nDrehWink-aGeo.nShearWink);
        FASTBOOL    bHorzChg=nHWink>13500 && nHWink<=31500;
        FASTBOOL    bVertChg=nVWink>4500 && nVWink<=22500;

        const StyleSettings& rStyleSettings = pOutDev->GetSettings().GetStyleSettings();
        Color a3DLightColor( rStyleSettings.GetLightColor() );
        Color a3DShadowColor( rStyleSettings.GetShadowColor() );

        pOutDev->SetLineColor( bHorzChg ? a3DShadowColor : a3DLightColor);
        pOutDev->DrawLine(aPoly2[0],aPoly2[1]);

        pOutDev->SetLineColor( bHorzChg ? a3DLightColor  : a3DShadowColor);
        pOutDev->DrawLine(aPoly2[2],aPoly2[3]);

        pOutDev->SetLineColor( bVertChg ? a3DLightColor  : a3DShadowColor);
        pOutDev->DrawLine(aPoly2[1],aPoly2[2]);

        pOutDev->SetLineColor( bVertChg ? a3DShadowColor : a3DLightColor);
        pOutDev->DrawLine(aPoly2[3],aPoly2[4]);

        pOutDev->SetLineColor( bHorzChg ? a3DLightColor  : a3DShadowColor);
        pOutDev->DrawLine(aPoly1[0],aPoly1[1]);

        pOutDev->SetLineColor( bHorzChg ? a3DShadowColor : a3DLightColor);
        pOutDev->DrawLine(aPoly1[2],aPoly1[3]);

        pOutDev->SetLineColor( bVertChg ? a3DShadowColor : a3DLightColor);
        pOutDev->DrawLine(aPoly1[1],aPoly1[2]);

        pOutDev->SetLineColor( bVertChg ? a3DLightColor  : a3DShadowColor);
        pOutDev->DrawLine(aPoly1[3],aPoly1[4]);
    }
    XubString aNam(rText);

    if(aNam.Len())
    {
        Size aOutSize(aTextRect.GetSize()); aOutSize.Width()-=6; aOutSize.Height()-=6;

        if (aOutSize.Width()>=4 || aOutSize.Height()>=4)
        {
            Point aOutPos(aTextRect.TopLeft()); aOutPos.X()+=3; aOutPos.Y()+=3;
            long nMaxOutY=aOutPos.Y()+aOutSize.Height();
            Font aFontMerk(pOutDev->GetFont());
            Font aFont( OutputDevice::GetDefaultFont( DEFAULTFONT_SANS_UNICODE, LANGUAGE_SYSTEM, DEFAULTFONT_FLAGS_ONLYONE ) );
            aFont.SetColor(COL_LIGHTRED);
            aFont.SetTransparent(TRUE);
            aFont.SetLineOrientation(USHORT(NormAngle360(aGeo.nDrehWink)/10));

            if (IsLinkedGraphic())
                aFont.SetUnderline(UNDERLINE_SINGLE);
            Size aFontSize(Size(0,aGeo.nDrehWink % 9000==0 ? 12 : 14));

            if (aFontSize.Height()>aOutSize.Height())
                aFontSize.Height()=aOutSize.Height();
            aFont.SetSize(aFontSize);
            pOutDev->SetFont(aFont);
            String aOutStr(aNam);

            while(aOutStr.Len() && aOutPos.Y() <= nMaxOutY)
            {
                String aStr1(aOutStr);
                INT32 nTextWidth = pOutDev->GetTextWidth(aStr1);
                INT32 nTextHeight = pOutDev->GetTextHeight();

                while(aStr1.Len() && nTextWidth > aOutSize.Width())
                {
                    aStr1.Erase(aStr1.Len() - 1);
                    nTextWidth = pOutDev->GetTextWidth(aStr1);
                    nTextHeight = pOutDev->GetTextHeight();
                }
                Point aPos(aOutPos);
                aOutPos.Y() += nTextHeight;

                if (aOutPos.Y()<=nMaxOutY)
                {
                    if (aGeo.nShearWink!=0) ShearPoint(aPos,aTopLeft,aGeo.nTan);
                    if (aGeo.nDrehWink!=0) RotatePoint(aPos,aTopLeft,aGeo.nSin,aGeo.nCos);
                    pOutDev->DrawText(aPos,aStr1);
                    aOutStr.Erase(0, aStr1.Len());
                }
            }
            pOutDev->SetFont(aFontMerk);
        }
    }
    pOutDev->EnableMapMode( bOldMap );
}

// -----------------------------------------------------------------------------

FASTBOOL SdrGrafObj::Paint( ExtOutputDevice& rOut, const SdrPaintInfoRec& rInfoRec ) const
{
    // Hidden objects on masterpages, draw nothing
    if( ( ( rInfoRec.nPaintMode & SDRPAINTMODE_MASTERPAGE ) && bNotVisibleAsMaster ) ||
        ( ( OUTDEV_PRINTER == rOut.GetOutDev()->GetOutDevType() ) && bEmptyPresObj ) )
    {
        return TRUE;
    }

    FASTBOOL        bDraft = ( 0 != ( rInfoRec.nPaintMode & SDRPAINTMODE_DRAFTGRAF ) );
    FASTBOOL        bSwappedOut = pGraphic->IsSwappedOut() || ( pGraphic->GetType() == GRAPHIC_NONE );
    FASTBOOL        bLoading = FALSE;
    OutputDevice*   pOutDev = rOut.GetOutDev();
    GDIMetaFile*    pRecMetaFile = pOutDev->GetConnectMetaFile();
    FASTBOOL        bMtfRecording = ( pRecMetaFile && pRecMetaFile->IsRecord() && !pRecMetaFile->IsPause() );
    const SdrView*  pView = ( rInfoRec.pPV ? &rInfoRec.pPV->GetView() : NULL );

    if( bSwappedOut && !bDraft )
    {
        if( !ImpUpdateGraphicLink() )
        {
            if( ( OUTDEV_WINDOW == pOutDev->GetOutDevType() ) && !bMtfRecording && pView && pView->IsSwapAsynchron() )
            {
                ( (SdrView*) pView )->ImpAddAsyncObj( this, pOutDev );
                bLoading = TRUE;
            }
            else
                ForceSwapIn();
        }
    }

    if( pGraphic->IsSwappedOut() || ( pGraphic->GetType() == GRAPHIC_NONE ) || ( pGraphic->GetType() == GRAPHIC_DEFAULT ) )
        bDraft=TRUE;

    long          nDrehWink = aGeo.nDrehWink, nShearWink = aGeo.nShearWink;
    FASTBOOL      bRotate = ( nDrehWink != 0 && nDrehWink != 18000 );
    FASTBOOL      bShear = ( nShearWink != 0 );
    FASTBOOL      bRota180 = nDrehWink == 18000;
    USHORT        nMirrorCase = ( bRota180 ? ( bMirrored ? 3 : 4 ) : ( bMirrored ? 2 : 1 ) );   //  4 | 3   H&V gespiegelt | nur Vertikal
    FASTBOOL      bHMirr = ( ( 2 == nMirrorCase ) || ( 4 == nMirrorCase ) );                    // ---+---  ---------------+-----------------
    FASTBOOL      bVMirr = ( ( 3 == nMirrorCase ) || ( 4 == nMirrorCase ) );                    //  2 | 1   nur Horizontal | nicht gespiegelt

    if( !bEmptyPresObj && !bDraft )
    {
        Point       aLogPos( aRect.TopLeft() );
        Size        aLogSize( pOutDev->PixelToLogic( pOutDev->LogicToPixel( aRect ).GetSize() ) );
        GraphicAttr aAttr( aGrafInfo );

        aAttr.SetMirrorFlags( ( bHMirr ? BMP_MIRROR_HORZ : 0 ) | ( bVMirr ? BMP_MIRROR_VERT : 0 ) );

        if( bRota180 )
        {
            aLogPos.X() -= ( aLogSize.Width() - 1L );
            aLogPos.Y() -= ( aLogSize.Height() - 1L );
        }

        if( pGraphic->GetType() == GRAPHIC_BITMAP )
        {
            if( pGraphic->IsAnimated() )
            {
                SdrAnimationMode    eAnimMode = SDR_ANIMATION_ANIMATE;
                FASTBOOL            bEnable = TRUE;

                if( rInfoRec.pPV != NULL )
                {
                    eAnimMode= ((SdrPaintView&) rInfoRec.pPV->GetView()).GetAnimationMode();
                    bEnable = eAnimMode != SDR_ANIMATION_DISABLE;
                }

                // #98825# Look if graphics animation is disabled by
                // accessibility options
                if(rInfoRec.pPV // #99632# This may be zero when it's an animated GIF /e.g.)
                    && bEnable)
                {
                    const SdrView& rTargetView = rInfoRec.pPV->GetView();
                    const SvtAccessibilityOptions& rOpt = ((SdrView&)rTargetView).getAccessibilityOptions();
                    sal_Bool bIsAllowedAnimatedGraphics = rOpt.GetIsAllowAnimatedGraphics();

                    if(!bIsAllowedAnimatedGraphics)
                    {
                        bEnable = FALSE;

                        // extra work to be done to make graphic visible at all
                        pGraphic->StopAnimation();
                        pGraphic->Draw(pOutDev, aLogPos, aLogSize, &aAttr);
                    }
                }

                if( bEnable )
                {
                    if( eAnimMode == SDR_ANIMATION_ANIMATE )
                    {
                        pGraphic->SetAnimationNotifyHdl( LINK( this, SdrGrafObj, ImpAnimationHdl ) );
                        pGraphic->StartAnimation( pOutDev, aLogPos, aLogSize, 0, &aAttr );
                    }
                    else if( eAnimMode == SDR_ANIMATION_DONT_ANIMATE )
                        pGraphic->Draw( pOutDev, aLogPos, aLogSize, &aAttr );
                }
            }
            else
            {
                if( bRotate && !bRota180 )
                    aAttr.SetRotation( nDrehWink / 10 );

                pGraphic->Draw( pOutDev, aLogPos, aLogSize, &aAttr );
            }
        }
        else
        {
            // MetaFiles
            const ULONG nOldDrawMode = pOutDev->GetDrawMode();

            // Falls Modus GRAYBITMAP, wollen wir auch Mtf's als Graustufen darstellen
            if( nOldDrawMode & DRAWMODE_GRAYBITMAP )
            {
                ULONG nNewDrawMode = nOldDrawMode;
                nNewDrawMode &= ~( DRAWMODE_BLACKLINE | DRAWMODE_BLACKFILL | DRAWMODE_WHITEFILL | DRAWMODE_NOFILL );
                pOutDev->SetDrawMode( nNewDrawMode |= DRAWMODE_GRAYLINE | DRAWMODE_GRAYFILL  );
            }

            if( bRotate && !bRota180 )
                aAttr.SetRotation( nDrehWink / 10 );

            pGraphic->Draw( pOutDev, aLogPos, aLogSize, &aAttr );
            pOutDev->SetDrawMode( nOldDrawMode );
        }
    }

    // auch GRAPHIC_NONE oder SwappedOut( AsyncSwap )
    if( ( bEmptyPresObj || bDraft ) && ( !bDraft || !( rInfoRec.nPaintMode & SDRPAINTMODE_HIDEDRAFTGRAF ) ) )
    {
        XubString   aText;
        Bitmap*     pBmp = NULL;
        FASTBOOL    bFill = FALSE;

        if( bEmptyPresObj )
            bFill = !ImpPaintEmptyPres( pOutDev );

        // und nun noch einen grauen Rahmen drum herum, Text rein, ...
        if( !bEmptyPresObj )
        {
            aText = aFileName;

            if(!aText.Len())
            {
                aText = aName;

                if( bLoading )
                {
                    aText.AppendAscii(" ...");
                    //FASTBOOL bNoName=aText.Len()==0;
                    //if (!bNoName) aText.Insert(' ',0);
                    //else aText.Insert("...",0);
                    //aText.Insert("Loading",0);
                }
            }

#ifndef SVX_LIGHT
            pBmp = new Bitmap( ResId ( BMAP_GrafikEi, ImpGetResMgr() ) );
#endif
        }

        ImpPaintReplacement( pOutDev, aText, pBmp, bFill );
        delete pBmp;
    }

    if( OUTDEV_PRINTER == pOutDev->GetOutDevType() )
        ForceSwapOut();

    return( HasText() ? SdrTextObj::Paint( rOut, rInfoRec ) : TRUE );
}

// -----------------------------------------------------------------------------

SdrObject* SdrGrafObj::CheckHit(const Point& rPnt, USHORT nTol, const SetOfByte* pVisiLayer) const
{
    return ImpCheckHit( rPnt, nTol, pVisiLayer, TRUE );
}

// -----------------------------------------------------------------------------

void SdrGrafObj::TakeObjNameSingul(XubString& rName) const
{
    switch( pGraphic->GetType() )
    {
        case GRAPHIC_BITMAP:
        {
            const USHORT nId = ( ( IsTransparent() || ( (const SdrGrafTransparenceItem&) GetItem( SDRATTR_GRAFTRANSPARENCE ) ).GetValue() ) ?
                                 ( IsLinkedGraphic() ? STR_ObjNameSingulGRAFBMPTRANSLNK : STR_ObjNameSingulGRAFBMPTRANS ) :
                                 ( IsLinkedGraphic() ? STR_ObjNameSingulGRAFBMPLNK : STR_ObjNameSingulGRAFBMP ) );

            rName=ImpGetResStr( nId );
        }
        break;

        case GRAPHIC_GDIMETAFILE:
            rName=ImpGetResStr( IsLinkedGraphic() ? STR_ObjNameSingulGRAFMTFLNK : STR_ObjNameSingulGRAFMTF );
        break;

        case GRAPHIC_NONE:
            rName=ImpGetResStr( IsLinkedGraphic() ? STR_ObjNameSingulGRAFNONELNK : STR_ObjNameSingulGRAFNONE );
        break;

        default:
            rName=ImpGetResStr(  IsLinkedGraphic() ? STR_ObjNameSingulGRAFLNK : STR_ObjNameSingulGRAF );
        break;
    }

    if( aName.Len() )
    {
        rName.AppendAscii( " '" );
        rName += aName;
        rName += sal_Unicode( '\'' );
    }
}

// -----------------------------------------------------------------------------

void SdrGrafObj::TakeObjNamePlural( XubString& rName ) const
{
    switch( pGraphic->GetType() )
    {
        case GRAPHIC_BITMAP:
        {
            const USHORT nId = ( ( IsTransparent() || ( (const SdrGrafTransparenceItem&) GetItem( SDRATTR_GRAFTRANSPARENCE ) ).GetValue() ) ?
                                 ( IsLinkedGraphic() ? STR_ObjNamePluralGRAFBMPTRANSLNK : STR_ObjNamePluralGRAFBMPTRANS ) :
                                 ( IsLinkedGraphic() ? STR_ObjNamePluralGRAFBMPLNK : STR_ObjNamePluralGRAFBMP ) );

            rName=ImpGetResStr( nId );
        }
        break;

        case GRAPHIC_GDIMETAFILE:
            rName=ImpGetResStr( IsLinkedGraphic() ? STR_ObjNamePluralGRAFMTFLNK : STR_ObjNamePluralGRAFMTF );
        break;

        case GRAPHIC_NONE:
            rName=ImpGetResStr( IsLinkedGraphic() ? STR_ObjNamePluralGRAFNONELNK : STR_ObjNamePluralGRAFNONE );
        break;

        default:
            rName=ImpGetResStr(  IsLinkedGraphic() ? STR_ObjNamePluralGRAFLNK : STR_ObjNamePluralGRAF );
        break;
    }

    if( aName.Len() )
    {
        rName.AppendAscii( " '" );
        rName += aName;
        rName += sal_Unicode( '\'' );
    }
}

// -----------------------------------------------------------------------------

void SdrGrafObj::operator=( const SdrObject& rObj )
{
    SdrRectObj::operator=( rObj );

    const SdrGrafObj& rGraf = (SdrGrafObj&) rObj;

    pGraphic->SetGraphic( rGraf.GetGraphic() );
    aCropRect = rGraf.aCropRect;
    aFileName = rGraf.aFileName;
    aFilterName = rGraf.aFilterName;
    aName = rGraf.aName;
    bMirrored = rGraf.bMirrored;

#ifndef SVX_LIGHT
    if( rGraf.pGraphicLink != NULL)
#else
    if( rGraf.aFileName.Len() )
#endif
        SetGraphicLink( aFileName, aFilterName );

    ImpSetAttrToGrafInfo();
}

// -----------------------------------------------------------------------------

FASTBOOL SdrGrafObj::HasSpecialDrag() const
{
    return TRUE;
}

// -----------------------------------------------------------------------------

USHORT SdrGrafObj::GetHdlCount() const
{
    return 8;
}

// -----------------------------------------------------------------------------

SdrHdl* SdrGrafObj::GetHdl(USHORT nHdlNum) const
{
    return SdrRectObj::GetHdl( nHdlNum + 1 );
}

// -----------------------------------------------------------------------------

void SdrGrafObj::NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact)
{
    SdrRectObj::NbcResize( rRef, xFact, yFact );

    FASTBOOL bMirrX = xFact.GetNumerator() < 0;
    FASTBOOL bMirrY = yFact.GetNumerator() < 0;

    if( bMirrX != bMirrY )
        bMirrored = !bMirrored;
}

// -----------------------------------------------------------------------------

void SdrGrafObj::NbcRotate(const Point& rRef, long nWink, double sn, double cs)
{
    SdrRectObj::NbcRotate(rRef,nWink,sn,cs);
}

// -----------------------------------------------------------------------------

void SdrGrafObj::NbcMirror(const Point& rRef1, const Point& rRef2)
{
    SdrRectObj::NbcMirror(rRef1,rRef2);
    bMirrored = !bMirrored;
}

// -----------------------------------------------------------------------------

void SdrGrafObj::NbcShear(const Point& rRef, long nWink, double tn, FASTBOOL bVShear)
{
    SdrRectObj::NbcRotate( rRef, nWink, tn, bVShear );
}

// -----------------------------------------------------------------------------

void SdrGrafObj::NbcSetSnapRect(const Rectangle& rRect)
{
    SdrRectObj::NbcSetSnapRect(rRect);
}

// -----------------------------------------------------------------------------

void SdrGrafObj::NbcSetLogicRect( const Rectangle& rRect)
{
    FASTBOOL bChg=rRect.GetSize()!=aRect.GetSize();
    SdrRectObj::NbcSetLogicRect(rRect);
}

// -----------------------------------------------------------------------------

SdrObjGeoData* SdrGrafObj::NewGeoData() const
{
    return new SdrGrafObjGeoData;
}

// -----------------------------------------------------------------------------

void SdrGrafObj::SaveGeoData(SdrObjGeoData& rGeo) const
{
    SdrRectObj::SaveGeoData(rGeo);
    SdrGrafObjGeoData& rGGeo=(SdrGrafObjGeoData&)rGeo;
    rGGeo.bMirrored=bMirrored;
}

// -----------------------------------------------------------------------------

void SdrGrafObj::RestGeoData(const SdrObjGeoData& rGeo)
{
    long        nDrehMerk = aGeo.nDrehWink;
    long        nShearMerk = aGeo.nShearWink;
    FASTBOOL    bMirrMerk = bMirrored;
    Size        aSizMerk( aRect.GetSize() );

    SdrRectObj::RestGeoData(rGeo);
    SdrGrafObjGeoData& rGGeo=(SdrGrafObjGeoData&)rGeo;
    bMirrored=rGGeo.bMirrored;
}

// -----------------------------------------------------------------------------

void SdrGrafObj::SetPage( SdrPage* pNewPage )
{
    FASTBOOL bRemove = pNewPage == NULL && pPage != NULL;
    FASTBOOL bInsert = pNewPage != NULL && pPage == NULL;

    if( bRemove )
    {
        // hier kein SwapIn noetig, weil wenn nicht geladen, dann auch nicht animiert.
        if( pGraphic->IsAnimated())
            pGraphic->StopAnimation();

        if( pGraphicLink != NULL )
            ImpLinkAbmeldung();
    }

    SdrRectObj::SetPage( pNewPage );

    if(aFileName.Len() && bInsert)
        ImpLinkAnmeldung();
}

// -----------------------------------------------------------------------------

void SdrGrafObj::SetModel( SdrModel* pNewModel )
{
    FASTBOOL bChg = pNewModel != pModel;

    if( bChg )
    {
        if( ( GRAFSTREAMPOS_INVALID != nGrafStreamPos ) || pGraphic->HasUserData() )
        {
            ForceSwapIn();
            pGraphic->SetUserData();
            nGrafStreamPos = GRAFSTREAMPOS_INVALID;
        }

        if( pGraphicLink != NULL )
            ImpLinkAbmeldung();
    }

    // Model umsetzen
    SdrRectObj::SetModel(pNewModel);

    if( bChg && aFileName.Len() )
        ImpLinkAnmeldung();
}

// -----------------------------------------------------------------------------

void SdrGrafObj::StartAnimation( OutputDevice* pOutDev, const Point& rPoint, const Size& rSize, long nExtraData )
{
    GraphicAttr     aAttr( aGrafInfo );
    USHORT          nMirrorCase = ( aGeo.nDrehWink == 18000 ) ? ( bMirrored ? 3 : 4 ) : ( bMirrored ? 2 : 1 );
    FASTBOOL        bHMirr = nMirrorCase == 2 || nMirrorCase == 4;
    FASTBOOL        bVMirr = nMirrorCase == 3 || nMirrorCase == 4;

    aAttr.SetMirrorFlags( ( bHMirr ? BMP_MIRROR_HORZ : 0 ) | ( bVMirr ? BMP_MIRROR_VERT : 0 ) );
    pGraphic->SetAnimationNotifyHdl( LINK( this, SdrGrafObj, ImpAnimationHdl ) );
    pGraphic->StartAnimation( pOutDev, rPoint, rSize, nExtraData, &aAttr );
}

// -----------------------------------------------------------------------------

void SdrGrafObj::StopAnimation(OutputDevice* pOutDev, long nExtraData)
{
    pGraphic->StopAnimation( pOutDev, nExtraData );
}

// -----------------------------------------------------------------------------

FASTBOOL SdrGrafObj::HasGDIMetaFile() const
{
    return( pGraphic->GetType() == GRAPHIC_GDIMETAFILE );
}

// -----------------------------------------------------------------------------

const GDIMetaFile* SdrGrafObj::GetGDIMetaFile() const
{
    DBG_ERROR( "Invalid return value! Don't use it! (KA)" );
    return &GetGraphic().GetGDIMetaFile();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// pre- and postprocessing for objects for saving

void SdrGrafObj::PreSave()
{
    // call parent
    SdrRectObj::PreSave();

    // prepare SetItems for storage
    const SfxItemSet& rSet = GetUnmergedItemSet();
    const SfxItemSet* pParent = GetStyleSheet() ? &GetStyleSheet()->GetItemSet() : 0L;
    SdrGrafSetItem aGrafAttr(rSet.GetPool());
    aGrafAttr.GetItemSet().Put(rSet);
    aGrafAttr.GetItemSet().SetParent(pParent);
    mpObjectItemSet->Put(aGrafAttr);
}

void SdrGrafObj::PostSave()
{
    // call parent
    SdrRectObj::PostSave();

    // remove SetItems from local itemset
    mpObjectItemSet->ClearItem(SDRATTRSET_GRAF);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrGrafObj::WriteData(SvStream& rOut) const
{
    ForceSwapIn();
    SdrRectObj::WriteData( rOut );

    // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
    SdrDownCompat aCompat( rOut, STREAM_WRITE );

#ifdef DBG_UTIL
    aCompat.SetID( "SdrGrafObj" );
#endif

    GraphicType eType( pGraphic->GetType() );
    BOOL        bHasGraphic( !aFileName.Len() && eType != GRAPHIC_NONE );

    // dieses Flag wird ab V11 rausgeschrieben
    rOut << bHasGraphic;

    if(bHasGraphic)
    {
        // Graphik ist nicht gelinkt: ggf. komprimiert speichern:
        // seit V11 eingapackt
        SdrDownCompat aGrafCompat(rOut, STREAM_WRITE);
        BOOL bZCompr(pModel && pModel->IsSaveCompressed() && eType == GRAPHIC_BITMAP);
        BOOL bNCompr(pModel && pModel->IsSaveNative());
        const UINT16 nOldComprMode(rOut.GetCompressMode());
        UINT16 nNewComprMode(nOldComprMode);

#ifdef DBG_UTIL
        aGrafCompat.SetID( "SdrGrafObj(Graphic)" );
#endif

        if(pModel->IsSwapGraphics() && (pModel->GetSwapGraphicsMode() & SDR_SWAPGRAPHICSMODE_DOC))
        {
            ((SdrGrafObj*)this)->pGraphic->SetUserData();
            ((SdrGrafObj*)this)->nGrafStreamPos = rOut.Tell();
        }

        if(bZCompr)
            nNewComprMode |= COMPRESSMODE_ZBITMAP;

        if(bNCompr)
            nNewComprMode |= COMPRESSMODE_NATIVE;

        rOut.SetCompressMode( nNewComprMode );
        rOut << pGraphic->GetGraphic();
        rOut.SetCompressMode( nOldComprMode );
    }

    rOut << aCropRect;
    rOut << BOOL(bMirrored);

    rOut.WriteByteString(aName);

    String aRelFileName;

    if( aFileName.Len() )
    {
        aRelFileName = INetURLObject::AbsToRel( aFileName,
                                                INetURLObject::WAS_ENCODED,
                                                INetURLObject::DECODE_UNAMBIGUOUS );
    }

    rOut.WriteByteString( aRelFileName );

    // UNICODE: rOut << aFilterName;
    rOut.WriteByteString(aFilterName);

    // ab V11
    rOut << (BOOL)( aFileName.Len() != 0 );

    SfxItemPool* pPool = GetItemPool();

    if(pPool)
    {
        const SfxItemSet& rSet = GetUnmergedItemSet();

        pPool->StoreSurrogate(rOut, &rSet.Get(SDRATTRSET_GRAF));
    }
    else
        rOut << UINT16( SFX_ITEMS_NULL );

    ForceSwapOut();
}

// -----------------------------------------------------------------------------

void SdrGrafObj::ReadDataTilV10( const SdrObjIOHeader& rHead, SvStream& rIn )
{
    Graphic aGraphic;

    // Import von allem mit Version <= 10
    rIn >> aGraphic;

    ULONG nError = rIn.GetError();

    // Ist die Graphik defekt, oder wurde nur eine leere Graphik eingelesen? (was bei gelinkten Graphiken der Fall ist)
    if( nError != 0)
        rIn.ResetError();

    if( rHead.GetVersion() >= 6)
        rIn >> aCropRect;

    if(rHead.GetVersion() >= 8)
    {
        // UNICODE: rIn>>aFileName;
        rIn.ReadByteString(aFileName);
    }

    if(rHead.GetVersion() >= 9)
    {
        // UNICODE: rIn >> aFilterName;
        rIn.ReadByteString(aFilterName);
    }
    else
        aFilterName = String( RTL_CONSTASCII_USTRINGPARAM( "BMP - MS Windows" ) );

    if( aFileName.Len() )
    {
#ifndef SVX_LIGHT
        String aFileURLStr;

        if( ::utl::LocalFileHelper::ConvertPhysicalNameToURL( aFileName, aFileURLStr ) )
        {
            SvStream* pIStm = ::utl::UcbStreamHelper::CreateStream( aFileURLStr, STREAM_READ | STREAM_SHARE_DENYNONE );

            if( pIStm )
            {
                GraphicFilter*  pFilter = GetGrfFilter();
                USHORT          nError = pFilter->ImportGraphic( aGraphic, aFileURLStr, *pIStm );

                SetGraphicLink( aFileURLStr, aFilterName );

                delete pIStm;
            }
        }
#else
        DBG_ERROR("SdrGrafObj::ReadDataTilV10(): SVX_LIGHT kann keine Graphic-Links");
#endif
    }
    else if( nError != 0 )
        rIn.SetError(nError);


    if( !rIn.GetError() )
        pGraphic->SetGraphic( aGraphic );
}

// -----------------------------------------------------------------------------

#ifdef WNT
#pragma optimize ("",off)
#endif

void SdrGrafObj::ReadData( const SdrObjIOHeader& rHead, SvStream& rIn )
{
    if( rIn.GetError() )
        return;

    SdrRectObj::ReadData( rHead, rIn );

    SdrDownCompat   aCompat( rIn, STREAM_READ );
    FASTBOOL        bDelayedLoad = ( pModel != NULL ) && pModel->IsSwapGraphics();

#ifdef DBG_UTIL
    aCompat.SetID("SdrGrafObj");
#endif

    pGraphic->SetUserData();
    nGrafStreamPos = GRAFSTREAMPOS_INVALID;

    if( rHead.GetVersion() < 11 )
        ReadDataTilV10( rHead, rIn );
    else
    {
        String  aFileNameRel;
        BOOL    bHasGraphic;
        BOOL    bTmp;
        BOOL    bGraphicLink;

        rIn >> bHasGraphic;

        if( bHasGraphic )
        {
            SdrDownCompat aGrafCompat( rIn,STREAM_READ );

#ifdef DBG_UTIL
            aGrafCompat.SetID("SdrGrafObj(Graphic)");
#endif

            nGrafStreamPos = rIn.Tell();

            if( !bDelayedLoad )
            {
                Graphic aGraphic;
                rIn >> aGraphic;
                pGraphic->SetGraphic( aGraphic );
            }
            else
                pGraphic->SetSwapState();

            // Ist die Grafik defekt, oder wurde nur eine leere Graphik eingelesen?
            // Daran soll mein Read jedoch nicht scheitern.
            if( rIn.GetError() != 0 )
                rIn.ResetError();
        }

        rIn >> aCropRect;
        rIn >> bTmp; bMirrored = bTmp;

        rIn.ReadByteString(aName);
        // #85414# since there seems to be some documents wich have an illegal
        // character inside the name of a graphic object we have to fix this
        // here on load time or it will crash our xml later.
        const xub_StrLen nLen = aName.Len();
        for( xub_StrLen nIndex = 0; nIndex < nLen; nIndex++ )
        {
            if( aName.GetChar( nIndex ) < ' ' )
                aName.SetChar( nIndex, '?' );
        }

        rIn.ReadByteString(aFileNameRel);

        if( aFileNameRel.Len() )
        {
            aFileName = ::URIHelper::SmartRelToAbs( aFileNameRel, FALSE,
                                                    INetURLObject::WAS_ENCODED,
                                                    INetURLObject::DECODE_UNAMBIGUOUS );
        }
        else
            aFileName.Erase();

        // UNICODE: rIn >> aFilterName;
        rIn.ReadByteString(aFilterName);

        rIn >> bGraphicLink;                    // auch dieses Flag ist neu in V11

        if( aCompat.GetBytesLeft() > 0 )
        {
            SfxItemPool* pPool = GetItemPool();

            if( pPool )
            {
                sal_uInt16 nSetID = SDRATTRSET_GRAF;
                const SdrGrafSetItem* pGrafAttr = (const SdrGrafSetItem*)pPool->LoadSurrogate(rIn, nSetID, 0);
                if(pGrafAttr)
                    SetItemSet(pGrafAttr->GetItemSet());
                    ImpSetAttrToGrafInfo();
            }
            else
            {
                UINT16 nSuroDummy;
                rIn >> nSuroDummy;
            }
        }
        else
            bCopyToPoolOnAfterRead = TRUE;

        if( bGraphicLink && aFileName.Len() )
        {
            SetGraphicLink( aFileName, aFilterName );

            if( !bDelayedLoad )
                ImpUpdateGraphicLink();
        }
    }
}

// -----------------------------------------------------------------------------

#ifdef WNT
#pragma optimize ("",on)
#endif

// -----------------------------------------------------------------------------

Rectangle SdrGrafObj::GetAnimationRect(const OutputDevice* pOutDev) const
{
    return GetSnapRect();
}

// -----------------------------------------------------------------------------

void SdrGrafObj::SetAnimationSupervisor( OutputDevice* pDisplayDev, BOOL bObjSupervises )
{
    ForceSwapIn();
    List* pAInfoList = pGraphic->GetAnimationInfoList();

    if ( pAInfoList )
    {
        for( AInfo* pAInfo = (AInfo*) pAInfoList->First(); pAInfo; pAInfo = (AInfo*) pAInfoList->Next() )
        {
            if( pAInfo->pOutDev == pDisplayDev )
            {
                pAInfo->nExtraData = ( bObjSupervises ? 1L : (long) this );

                if ( !bObjSupervises )
                    pAInfo->bPause = FALSE;
            }
        }
    }
}

// -----------------------------------------------------------------------------

void SdrGrafObj::ResetAnimationLoopCount()
{
    if( pGraphic->IsAnimated() )
    {
        ForceSwapIn();
        pGraphic->ResetAnimationLoopCount();
    }
}

// -----------------------------------------------------------------------------

void SdrGrafObj::RestartAnimation(SdrPageView* pPageView) const
{
    // ToDo: hier noch entsprechend implementieren wie im TextObj
    SdrRectObj::RestartAnimation( pPageView );
}

// -----------------------------------------------------------------------------

SdrObject* SdrGrafObj::DoConvertToPolyObj(BOOL bBezier) const
{
    SdrObject* pRetval = NULL;

    switch( GetGraphicType() )
    {
        case GRAPHIC_GDIMETAFILE:
        {
            // NUR die aus dem MetaFile erzeugbaren Objekte in eine Gruppe packen und zurueckliefern
            SdrObjGroup*            pGrp = new SdrObjGroup();
            ImpSdrGDIMetaFileImport aFilter(*GetModel());
            Point                   aOutPos( aRect.TopLeft() );
            const Size              aOutSiz( aRect.GetSize() );

            aFilter.SetScaleRect(GetSnapRect());
            aFilter.SetLayer(GetLayer());

            UINT32 nInsAnz = aFilter.DoImport(GetTransformedGraphic().GetGDIMetaFile(), *pGrp->GetSubList(), 0);
            if(nInsAnz)
            {
                pRetval = pGrp;
                pGrp->NbcSetLayer(GetLayer());
                pGrp->SetModel(GetModel());
                pRetval = ImpConvertAddText(pRetval, bBezier);

                // convert all children
                if( pRetval )
                {
                    SdrObject* pHalfDone = pRetval;
                    pRetval = pHalfDone->DoConvertToPolyObj(bBezier);
                    delete pHalfDone; // resulting object is newly created

                    if( pRetval )
                    {
                        // flatten subgroups. As we call
                        // DoConvertToPolyObj() on the resulting group
                        // objects, subgroups can exist (e.g. text is
                        // a group object for every line).
                        SdrObjList* pList = pRetval->GetSubList();
                        if( pList )
                            pList->FlattenGroups();
                    }
                }
            }
            else
                delete pGrp;
            break;
        }
        case GRAPHIC_BITMAP:
        {
            // Grundobjekt kreieren und Fuellung ergaenzen
            pRetval = SdrRectObj::DoConvertToPolyObj(bBezier);

            // Bitmap als Attribut retten
            if(pRetval)
            {
                // Bitmap als Fuellung holen
                SfxItemSet aSet(GetItemSet());

                aSet.Put(XFillStyleItem(XFILL_BITMAP));
                Bitmap aBitmap( GetTransformedGraphic().GetBitmap() );
                XOBitmap aXBmp(aBitmap, XBITMAP_STRETCH);
                aSet.Put(XFillBitmapItem(String(), aXBmp));
                aSet.Put(XFillBmpTileItem(FALSE));

                pRetval->SetItemSet(aSet);
            }
            break;
        }
        case GRAPHIC_NONE:
        case GRAPHIC_DEFAULT:
        {
            pRetval = SdrRectObj::DoConvertToPolyObj(bBezier);
            break;
        }
    }

    return pRetval;
}

// -----------------------------------------------------------------------------

void SdrGrafObj::AfterRead()
{
    SdrRectObj::AfterRead();

    if( bCopyToPoolOnAfterRead )
    {
        ImpSetGrafInfoToAttr();
        bCopyToPoolOnAfterRead = FALSE;
    }
}

// -----------------------------------------------------------------------------

void SdrGrafObj::SFX_NOTIFY( SfxBroadcaster& rBC, const TypeId& rBCType,
                             const SfxHint& rHint, const TypeId& rHintType )
{
    SetXPolyDirty();
    SdrRectObj::SFX_NOTIFY( rBC, rBCType, rHint, rHintType );
    ImpSetAttrToGrafInfo();
}

// -----------------------------------------------------------------------------

void SdrGrafObj::ForceDefaultAttr()
{
    SdrRectObj::ForceDefaultAttr();

    ImpForceItemSet();
    mpObjectItemSet->Put( SdrGrafLuminanceItem( 0 ) );
    mpObjectItemSet->Put( SdrGrafContrastItem( 0 ) );
    mpObjectItemSet->Put( SdrGrafRedItem( 0 ) );
    mpObjectItemSet->Put( SdrGrafGreenItem( 0 ) );
    mpObjectItemSet->Put( SdrGrafBlueItem( 0 ) );
    mpObjectItemSet->Put( SdrGrafGamma100Item( 100 ) );
    mpObjectItemSet->Put( SdrGrafTransparenceItem( 0 ) );
    mpObjectItemSet->Put( SdrGrafInvertItem( FALSE ) );
    mpObjectItemSet->Put( SdrGrafModeItem( GRAPHICDRAWMODE_STANDARD ) );
    mpObjectItemSet->Put( SdrGrafCropItem( 0, 0, 0, 0 ) );
}

// -----------------------------------------------------------------------------

void SdrGrafObj::NbcSetStyleSheet( SfxStyleSheet* pNewStyleSheet, FASTBOOL bDontRemoveHardAttr )
{
    SetXPolyDirty();
    SdrRectObj::NbcSetStyleSheet( pNewStyleSheet, bDontRemoveHardAttr );
    ImpSetAttrToGrafInfo();
}

// -----------------------------------------------------------------------------
// ItemSet access

SfxItemSet* SdrGrafObj::CreateNewItemSet(SfxItemPool& rPool)
{
    // include ALL items, 2D and 3D
    return new SfxItemSet(rPool,
        // ranges from SdrAttrObj
        SDRATTR_START, SDRATTRSET_SHADOW,
        SDRATTRSET_OUTLINER, SDRATTRSET_MISC,
        SDRATTR_TEXTDIRECTION, SDRATTR_TEXTDIRECTION,

        // graf attributes
        SDRATTR_GRAF_FIRST, SDRATTRSET_GRAF,

        // outliner and end
        EE_ITEMS_START, EE_ITEMS_END,
        0, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// private support routines for ItemSet access
void SdrGrafObj::ItemSetChanged(const SfxItemSet& rSet)
{
    // local changes
    SetXPolyDirty();

    // call parent
    SdrRectObj::ItemSetChanged(rSet);

    // local changes
    ImpSetAttrToGrafInfo();
}

// -----------------------------------------------------------------------------

void SdrGrafObj::ImpSetAttrToGrafInfo()
{
    const SfxItemSet& rSet = GetItemSet();
    const sal_uInt16 nTrans = ( (SdrGrafTransparenceItem&) rSet.Get( SDRATTR_GRAFTRANSPARENCE ) ).GetValue();
    const SdrGrafCropItem&  rCrop = (const SdrGrafCropItem&) rSet.Get( SDRATTR_GRAFCROP );

    aGrafInfo.SetLuminance( ( (SdrGrafLuminanceItem&) rSet.Get( SDRATTR_GRAFLUMINANCE ) ).GetValue() );
    aGrafInfo.SetContrast( ( (SdrGrafContrastItem&) rSet.Get( SDRATTR_GRAFCONTRAST ) ).GetValue() );
    aGrafInfo.SetChannelR( ( (SdrGrafRedItem&) rSet.Get( SDRATTR_GRAFRED ) ).GetValue() );
    aGrafInfo.SetChannelG( ( (SdrGrafGreenItem&) rSet.Get( SDRATTR_GRAFGREEN ) ).GetValue() );
    aGrafInfo.SetChannelB( ( (SdrGrafBlueItem&) rSet.Get( SDRATTR_GRAFBLUE ) ).GetValue() );
    aGrafInfo.SetGamma( ( (SdrGrafGamma100Item&) rSet.Get( SDRATTR_GRAFGAMMA ) ).GetValue() * 0.01 );
    aGrafInfo.SetTransparency( (BYTE) FRound( Min( nTrans, (USHORT) 100 )  * 2.55 ) );
    aGrafInfo.SetInvert( ( (SdrGrafInvertItem&) rSet.Get( SDRATTR_GRAFINVERT ) ).GetValue() );
    aGrafInfo.SetDrawMode( ( (SdrGrafModeItem&) rSet.Get( SDRATTR_GRAFMODE ) ).GetValue() );
    aGrafInfo.SetCrop( rCrop.GetLeft(), rCrop.GetTop(), rCrop.GetRight(), rCrop.GetBottom() );

    SetXPolyDirty();
    SetRectsDirty();
}

// -----------------------------------------------------------------------------

void SdrGrafObj::ImpSetGrafInfoToAttr()
{
    SetItem( SdrGrafLuminanceItem( aGrafInfo.GetLuminance() ) );
    SetItem( SdrGrafContrastItem( aGrafInfo.GetContrast() ) );
    SetItem( SdrGrafRedItem( aGrafInfo.GetChannelR() ) );
    SetItem( SdrGrafGreenItem( aGrafInfo.GetChannelG() ) );
    SetItem( SdrGrafBlueItem( aGrafInfo.GetChannelB() ) );
    SetItem( SdrGrafGamma100Item( FRound( aGrafInfo.GetGamma() * 100.0 ) ) );
    SetItem( SdrGrafTransparenceItem( (USHORT) FRound( aGrafInfo.GetTransparency() / 2.55 ) ) );
    SetItem( SdrGrafInvertItem( aGrafInfo.IsInvert() ) );
    SetItem( SdrGrafModeItem( aGrafInfo.GetDrawMode() ) );
    SetItem( SdrGrafCropItem( aGrafInfo.GetLeftCrop(), aGrafInfo.GetTopCrop(), aGrafInfo.GetRightCrop(), aGrafInfo.GetBottomCrop() ) );
}

// -----------------------------------------------------------------------------

void SdrGrafObj::AdjustToMaxRect( const Rectangle& rMaxRect, BOOL bShrinkOnly )
{
    Size aSize;
    Size aMaxSize( rMaxRect.GetSize() );
    if ( pGraphic->GetPrefMapMode().GetMapUnit() == MAP_PIXEL )
        aSize = Application::GetDefaultDevice()->PixelToLogic( pGraphic->GetPrefSize(), MAP_100TH_MM );
    else
        aSize = OutputDevice::LogicToLogic( pGraphic->GetPrefSize(),
                                            pGraphic->GetPrefMapMode(),
                                            MapMode( MAP_100TH_MM ) );

    if( aSize.Height() != 0 && aSize.Width() != 0 )
    {
        Point aPos( rMaxRect.TopLeft() );

        // Falls Grafik zu gross, wird die Grafik
        // in die Seite eingepasst
        if ( (!bShrinkOnly                          ||
             ( aSize.Height() > aMaxSize.Height() ) ||
             ( aSize.Width()  > aMaxSize.Width()  ) )&&
             aSize.Height() && aMaxSize.Height() )
        {
            float fGrfWH =  (float)aSize.Width() /
                            (float)aSize.Height();
            float fWinWH =  (float)aMaxSize.Width() /
                            (float)aMaxSize.Height();

            // Grafik an Pagesize anpassen (skaliert)
            if ( fGrfWH < fWinWH )
            {
                aSize.Width() = (long)(aMaxSize.Height() * fGrfWH);
                aSize.Height()= aMaxSize.Height();
            }
            else if ( fGrfWH > 0.F )
            {
                aSize.Width() = aMaxSize.Width();
                aSize.Height()= (long)(aMaxSize.Width() / fGrfWH);
            }

            aPos = rMaxRect.Center();
        }

        if( bShrinkOnly )
            aPos = aRect.TopLeft();

        aPos.X() -= aSize.Width() / 2;
        aPos.Y() -= aSize.Height() / 2;
        SetLogicRect( Rectangle( aPos, aSize ) );
    }
}

// -----------------------------------------------------------------------------

IMPL_LINK( SdrGrafObj, ImpSwapHdl, GraphicObject*, pO )
{
    SvStream* pRet = GRFMGR_AUTOSWAPSTREAM_NONE;

    if( pO->IsInSwapOut() )
    {
        if( pModel != NULL && pModel->IsSwapGraphics() && pGraphic->GetSizeBytes() > 20480 )
        {
            SdrViewIter aIter( this );
            SdrView*    pView = aIter.FirstView();
            BOOL        bVisible = FALSE;

            while( !bVisible && pView )
            {
                bVisible = !pView->IsGrafDraft();

                if( !bVisible )
                    pView = aIter.NextView();
            }

            if( !bVisible )
            {
                const ULONG nSwapMode = pModel->GetSwapGraphicsMode();

#ifndef SVX_LIGHT
                if( ( ( GRAFSTREAMPOS_INVALID != nGrafStreamPos ) || pGraphic->HasUserData() || pGraphicLink ) &&
                    ( nSwapMode & SDR_SWAPGRAPHICSMODE_PURGE ) )
#else
                if( ( ( GRAFSTREAMPOS_INVALID != nGrafStreamPos ) || pGraphic->HasUserData() || aFileName.Len() ) &&
                    ( nSwapMode & SDR_SWAPGRAPHICSMODE_PURGE ) )
#endif
                {
                    pRet = NULL;
                }
                else if( nSwapMode & SDR_SWAPGRAPHICSMODE_TEMP )
                {
                    pRet = GRFMGR_AUTOSWAPSTREAM_TEMP;
                    pGraphic->SetUserData();
                    nGrafStreamPos = GRAFSTREAMPOS_INVALID;
                }
            }
        }
    }
    else if( pO->IsInSwapIn() )
    {
        // kann aus dem original Doc-Stream nachgeladen werden...
        if( pModel != NULL )
        {
            if( ( GRAFSTREAMPOS_INVALID != nGrafStreamPos ) || pGraphic->HasUserData() )
            {
                SdrDocumentStreamInfo aStreamInfo;

                aStreamInfo.mbDeleteAfterUse = FALSE;
                aStreamInfo.maUserData = pGraphic->GetUserData();
                aStreamInfo.mpStorageRef = NULL;

                SvStream* pStream = pModel->GetDocumentStream( aStreamInfo );

                if( pStream != NULL )
                {
                    Graphic aGraphic;

                    if( pGraphic->HasUserData() )
                    {
                        if( !GetGrfFilter()->ImportGraphic( aGraphic, String(), *pStream ) )
                        {
                            const String aUserData( pGraphic->GetUserData() );

                            pGraphic->SetGraphic( aGraphic );
                            pGraphic->SetUserData( aUserData );
                        }
                    }
                    else
                    {
                        pStream->Seek( nGrafStreamPos );
                        *pStream >> aGraphic;
                        pGraphic->SetGraphic( aGraphic );
                    }

                    pStream->ResetError();

                    if( aStreamInfo.mbDeleteAfterUse || aStreamInfo.mpStorageRef )
                    {
                        delete pStream;
                        delete aStreamInfo.mpStorageRef;
                    }

                    pRet = GRFMGR_AUTOSWAPSTREAM_LOADED;
                }
            }
            else if( !ImpUpdateGraphicLink() )
            {
                pRet = GRFMGR_AUTOSWAPSTREAM_TEMP;
            }
        }
        else
            pRet = GRFMGR_AUTOSWAPSTREAM_TEMP;
    }

    return (long)(void*) pRet;
}

// -----------------------------------------------------------------------------

IMPL_LINK( SdrGrafObj, ImpAnimationHdl, Animation*, pAnimation )
{
    // Wenn wir nicht mehr da sind, stoppen wir natuerlich alles und kehren gleich zurueck
    if( !bInserted )
    {
        pAnimation->Stop();
        return 0L;
    }

    List*   pAInfoList = pAnimation->GetAInfoList();
    AInfo*  pAInfo = (AInfo*) pAInfoList->First();
    BOOL    bExtern = FALSE;

    // Alle Extra-Data auf 0 setzen, wenn keine andere ExtraData
    // ausser der eigenen (1) gesetzt;
    // groesser als 1 bedeutet, da� die Animation von aussen gestartet
    // wurde, z.B. von der DiaShow.
    while( pAInfo != NULL )
    {
        if( pAInfo->nExtraData == 1L )
            pAInfo->nExtraData = 0;
        else if( pAInfo->nExtraData > 1L )
            bExtern = TRUE;

        pAInfo = (AInfo*) pAInfoList->Next();
    }

    if( pModel!=NULL && pPage!=NULL && bInserted && pAInfoList->Count() )
    {
        USHORT      nPageNum=pPage->GetPageNum();
        FASTBOOL    bMaster=pPage->IsMasterPage() && !bNotVisibleAsMaster;
        USHORT      nLsAnz=pModel->GetListenerCount();

        for( USHORT nLsNum=0; nLsNum<nLsAnz; nLsNum++ )
        {
            SfxListener* pLs=pModel->GetListener(nLsNum);
            SdrMarkView* pView=PTR_CAST(SdrMarkView,pLs);

            if( pView && ( pView->IsAnimationEnabled() || bExtern ) )
            {
                FASTBOOL    bMrk=pView->IsObjMarked(this);
                FASTBOOL    bPause=pView->IsAnimationPause();
                USHORT      nPvAnz=pView->GetPageViewCount();

                for (USHORT nPvNum=0; nPvNum<nPvAnz; nPvNum++)
                {
                    SdrPageView*    pPV=pView->GetPageViewPvNum(nPvNum);
                    SdrPage*        pPg=pPV->GetPage();

                    if (pPV->GetVisibleLayers().IsSet(nLayerID))
                    {
                        FASTBOOL bJa=pPg==pPage;

                        if (!bJa && bMaster && !pPg->IsMasterPage())
                        {
                            USHORT nMasterAnz=pPg->GetMasterPageCount();

                            for (USHORT nMasterNum=0; nMasterNum<nMasterAnz && !bJa; nMasterNum++)
                            {
                                const SdrMasterPageDescriptor& rMPD=pPg->GetMasterPageDescriptor(nMasterNum);
                                bJa=nPageNum==rMPD.GetPageNum() && rMPD.GetVisibleLayers().IsSet(nLayerID);
                            }
                        }
                        if (bJa)
                        {
                            USHORT nOutAnz=pView->GetWinCount();

                            for (USHORT nOutNum=0; nOutNum<nOutAnz; nOutNum++)
                            {
                                OutputDevice* pOut=pView->GetWin(nOutNum);

                                if( pOut->GetOutDevType()==OUTDEV_WINDOW )
                                {
                                    Rectangle   aDrawRect( GetAnimationRect( pOut ) );
                                    const Point aOffset( pPV->GetOffset() );
                                    FASTBOOL    bFound = FALSE;

                                    aDrawRect.Move( aOffset.X(), aOffset.Y() );

                                    // Flag am gefundenen Objekt setzen
                                    pAInfo=(AInfo*)pAInfoList->First();
                                    while( pAInfo!=NULL && !bFound )
                                    {
                                        if (pAInfo->aStartOrg==aDrawRect.TopLeft() &&
                                            pAInfo->aStartSize==aDrawRect.GetSize() &&
                                            pAInfo->pOutDev==pOut )
                                        {
                                            if( pAInfo->nExtraData==0 )
                                                pAInfo->nExtraData=1L;

                                            pAInfo->bPause = ( bMrk || bPause );
                                            bFound = TRUE;
                                        }

                                        pAInfo=(AInfo*)pAInfoList->Next();
                                    }

                                    // Falls kein Record gefunden, wird ein neuer erzeugt
                                    // Das passiert z.B., wenn das Obj auf einer MasterPage liegt
                                    // und diese mittels MasterPagePaintCache angezeigt wurde.
                                    if (!bFound)
                                    {
                                        pAInfo = new AInfo;

                                        // erst einmal soviel wie moeglich bekanntes setzen
                                        *pAInfo = *(AInfo*) pAInfoList->GetObject( 0L );

                                        pAInfo->aStartOrg = aDrawRect.TopLeft();
                                        pAInfo->aStartSize = aDrawRect.GetSize();
                                        pAInfo->pOutDev = pOut;
                                        pAInfo->nExtraData = 1;
                                        pAInfo->bPause = ( bMrk || bPause );

                                        pAInfoList->Insert(pAInfo);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Alle Objekte mit nicht gesetztem Flag loeschen
    pAInfo = (AInfo*) pAInfoList->First();

    while( pAInfo != NULL )
    {
        if( pAInfo->nExtraData == 0 )
        {
            delete (AInfo*)pAInfoList->Remove(pAInfo);
            pAInfo=(AInfo*)pAInfoList->GetCurObject();
        }
        else
            pAInfo=(AInfo*)pAInfoList->Next();
    }

    return 0;
}

