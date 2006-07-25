/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: connctrl.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2006-07-25 12:49:11 $
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

// include ---------------------------------------------------------------

#include "xoutx.hxx"

#include "svdoedge.hxx"
#include "svdattrx.hxx"

#ifndef _SVDMARK_HXX //autogen
#include "svdmark.hxx"
#endif
#ifndef _SVDVIEW_HXX //autogen
#include "svdview.hxx"
#endif

#include "svdpage.hxx" // SdrObjList

#include "connctrl.hxx"
#include "dialmgr.hxx"
#include "dlgutil.hxx"

// #110094#
#ifndef _SDR_CONTACT_OBJECTCONTACTOFOBJLISTPAINTER_HXX
#include <svx/sdr/contact/objectcontactofobjlistpainter.hxx>
#endif

// #110094#
#ifndef _SDR_CONTACT_DISPLAYINFO_HXX
#include <svx/sdr/contact/displayinfo.hxx>
#endif

/*************************************************************************
|*
|* Ctor SvxXConnectionPreview
|*
*************************************************************************/

SvxXConnectionPreview::SvxXConnectionPreview( Window* pParent, const ResId& rResId,
                            const SfxItemSet& rInAttrs ) :
                            Control ( pParent, rResId ),
                            rAttrs  ( rInAttrs ),
                            pEdgeObj( NULL ),
                            pObjList( NULL ),
                            pView   ( NULL )
{
    pExtOutDev = new XOutputDevice( this );

    SetMapMode( MAP_100TH_MM );

    SetStyles();
}

/*************************************************************************
|*
|* Dtor SvxXConnectionPreview
|*
*************************************************************************/

SvxXConnectionPreview::~SvxXConnectionPreview()
{
    delete pObjList;
    delete pExtOutDev;
}

/*************************************************************************
|*
|* Dtor SvxXConnectionPreview
|*
*************************************************************************/

void SvxXConnectionPreview::Construct()
{
    DBG_ASSERT( pView, "Keine gueltige View Uebergeben!" );

    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();
    ULONG nMarkCount = rMarkList.GetMarkCount();

    if( nMarkCount >= 1 )
    {
        BOOL bFound = FALSE;
        const SdrObject* pObj = rMarkList.GetMark(0)->GetMarkedSdrObj();


        for( USHORT i = 0; i < nMarkCount && !bFound; i++ )
        {
            pObj = rMarkList.GetMark( i )->GetMarkedSdrObj();
            UINT32 nInv = pObj->GetObjInventor();
            UINT16 nId = pObj->GetObjIdentifier();
            if( nInv == SdrInventor && nId == OBJ_EDGE )
            {
                bFound = TRUE;
                SdrEdgeObj* pTmpEdgeObj = (SdrEdgeObj*) pObj;
                pEdgeObj = (SdrEdgeObj*) pTmpEdgeObj->Clone();

                SdrObjConnection& rConn1 = (SdrObjConnection&)pEdgeObj->GetConnection( TRUE );
                SdrObjConnection& rConn2 = (SdrObjConnection&)pEdgeObj->GetConnection( FALSE );

                rConn1 = pTmpEdgeObj->GetConnection( TRUE );
                rConn2 = pTmpEdgeObj->GetConnection( FALSE );

                SdrObject* pTmpObj1 = pTmpEdgeObj->GetConnectedNode( TRUE );
                SdrObject* pTmpObj2 = pTmpEdgeObj->GetConnectedNode( FALSE );

                // #110094#
                // potential memory leak here (!). Create SdrObjList only when there is
                // not yet one.
                if(!pObjList)
                {
                    pObjList = new SdrObjList( pView->GetModel(), NULL );
                }

                if( pTmpObj1 )
                {
                    SdrObject* pObj1 = pTmpObj1->Clone();
                    pObjList->InsertObject( pObj1 );
                    pEdgeObj->ConnectToNode( TRUE, pObj1 );
                }
                if( pTmpObj2 )
                {
                    SdrObject* pObj2 = pTmpObj2->Clone();
                    pObjList->InsertObject( pObj2 );
                    pEdgeObj->ConnectToNode( FALSE, pObj2 );
                }
                pObjList->InsertObject( pEdgeObj );
            }
        }
    }

    if( !pEdgeObj )
        pEdgeObj = new SdrEdgeObj();

    // Groesse anpassen
    if( pObjList )
    {
        OutputDevice* pOD = pView->GetWin( 0 );
        Rectangle aRect = pObjList->GetAllObjBoundRect();

        MapMode aMapMode = GetMapMode();
        aMapMode.SetMapUnit( pOD->GetMapMode().GetMapUnit() );
        SetMapMode( aMapMode );

        MapMode         aDisplayMap( aMapMode );
        Point           aNewPos;
        Size            aNewSize;
        const Size      aWinSize = PixelToLogic( GetOutputSizePixel(), aDisplayMap );
        const long      nWidth = aWinSize.Width();
        const long      nHeight = aWinSize.Height();
        double          fRectWH = (double) aRect.GetWidth() / aRect.GetHeight();
        double          fWinWH = (double) nWidth / nHeight;

        // Bitmap an Thumbgroesse anpassen (hier nicht!)
        if ( fRectWH < fWinWH)
        {
            aNewSize.Width() = (long) ( (double) nHeight * fRectWH );
            aNewSize.Height()= nHeight;
        }
        else
        {
            aNewSize.Width() = nWidth;
            aNewSize.Height()= (long) ( (double) nWidth / fRectWH );
        }

        Fraction aFrac1( aWinSize.Width(), aRect.GetWidth() );
        Fraction aFrac2( aWinSize.Height(), aRect.GetHeight() );
        Fraction aMinFrac( aFrac1 <= aFrac2 ? aFrac1 : aFrac2 );

        // MapMode umsetzen
        aDisplayMap.SetScaleX( aMinFrac );
        aDisplayMap.SetScaleY( aMinFrac );

        // Zentrierung
        aNewPos.X() = ( nWidth - aNewSize.Width() )  >> 1;
        aNewPos.Y() = ( nHeight - aNewSize.Height() ) >> 1;

        aDisplayMap.SetOrigin( LogicToLogic( aNewPos, aMapMode, aDisplayMap ) );
        SetMapMode( aDisplayMap );

        // Ursprung
        aNewPos = aDisplayMap.GetOrigin();
        aNewPos -= Point( aRect.TopLeft().X(), aRect.TopLeft().Y() );
        aDisplayMap.SetOrigin( aNewPos );
        SetMapMode( aDisplayMap );


        Point aPos;
        MouseEvent aMEvt( aPos, 1, 0, MOUSE_RIGHT );
        MouseButtonDown( aMEvt );
        /*
        Point aPt( -aRect.TopLeft().X(), -aRect.TopLeft().Y() );
        aMapMode.SetOrigin( aPt );

        // Skalierung
        Size aSize = GetOutputSize();
        Fraction aFrac1( aSize.Width(), aRect.GetWidth() );
        Fraction aFrac2( aSize.Height(), aRect.GetHeight() );
        Fraction aMaxFrac( aFrac1 > aFrac2 ? aFrac1 : aFrac2 );
        Fraction aMinFrac( aFrac1 <= aFrac2 ? aFrac1 : aFrac2 );
        BOOL bChange = (BOOL) ( (double)aMinFrac > 1.0 );
        aMapMode.SetScaleX( aMinFrac );
        aMapMode.SetScaleY( aMinFrac );

        // zentrieren
        long nXXL = aSize.Width() > aRect.GetWidth() ? aSize.Width() : aRect.GetWidth();
        long nXS = aSize.Width() <= aRect.GetWidth() ? aSize.Width() : aRect.GetWidth();
        if( bChange )
        {
            long nTmp = nXXL; nXXL = nXS; nXS = nTmp;
        }
        long nX = (long) ( (double)aMinFrac * (double)nXXL );
        nX = (long) ( (double)labs( nXS - nX ) / (double)aMinFrac / 2.0 );

        long nYXL = aSize.Height() > aRect.GetHeight() ? aSize.Height() : aRect.GetHeight();
        long nYS = aSize.Height() <= aRect.GetHeight() ? aSize.Height() : aRect.GetHeight();
        if( bChange )
        {
            long nTmp = nXXL; nXXL = nXS; nXS = nTmp;
        }
        long nY = (long) ( (double)aMinFrac * (double)nYXL );
        nY = (long) ( (double)labs( nYS - nY ) / (double)aMinFrac / 2.0 );

        aPt += Point( nX, nY );
        aMapMode.SetOrigin( aPt );

        SetMapMode( aMapMode );
        */
    }
}

/*************************************************************************
|*
|* SvxXConnectionPreview: Paint()
|*
*************************************************************************/

void SvxXConnectionPreview::Paint( const Rectangle& )
{
    SdrPaintInfoRec aInfoRec;

    //pEdgeObj->Paint( *pExtOutDev, aInfoRec );
    if( pObjList )
    {
        // #110094#
        // This will not work anymore. To not start at Adam and Eve, i will
        // ATM not try to change all this stuff to really using an own model
        // and a view. I will just try to provide a mechanism to paint such
        // objects without own model and without a page/view with the new
        // mechanism.
        //
        //pObjList->Paint( *pExtOutDev, aInfoRec );

        // New stuff: Use a ObjectContactOfObjListPainter.
        sdr::contact::SdrObjectVector aObjectVector;

        for(sal_uInt32 a(0L); a < pObjList->GetObjCount(); a++)
        {
            SdrObject* pObject = pObjList->GetObj(a);
            DBG_ASSERT(pObject,
                "SvxXConnectionPreview::Paint: Corrupt ObjectList (!)");
            aObjectVector.push_back(pObject);
        }

        sdr::contact::ObjectContactOfObjListPainter aPainter(aObjectVector);
        sdr::contact::DisplayInfo aDisplayInfo;

        aDisplayInfo.SetExtendedOutputDevice(pExtOutDev);
        aDisplayInfo.SetPaintInfoRec(&aInfoRec);
        aDisplayInfo.SetOutputDevice(pExtOutDev->GetOutDev());

        // keep draw hierarchy up-to-date
        aPainter.PreProcessDisplay(aDisplayInfo);

        // do processing
        aPainter.ProcessDisplay(aDisplayInfo);

        // prepare delete
        aPainter.PrepareDelete();
    }
}

/*************************************************************************
|*
|* SvxXConnectionPreview: SetAttributes()
|*
*************************************************************************/

void SvxXConnectionPreview::SetAttributes( const SfxItemSet& rInAttrs )
{
    //pEdgeObj->SetItemSetAndBroadcast(rInAttrs);
    pEdgeObj->SetMergedItemSetAndBroadcast(rInAttrs);

    Invalidate();
}

/*************************************************************************
|*
|* Ermittelt die Anzahl der Linienversaetze anhand des Preview-Objektes
|*
*************************************************************************/

USHORT SvxXConnectionPreview::GetLineDeltaAnz()
{
    const SfxItemSet& rSet = pEdgeObj->GetMergedItemSet();
    sal_uInt16 nCount(0);

    if(SFX_ITEM_DONTCARE != rSet.GetItemState(SDRATTR_EDGELINEDELTAANZ))
        nCount = ((const SdrEdgeLineDeltaAnzItem&)rSet.Get(SDRATTR_EDGELINEDELTAANZ)).GetValue();

    return nCount;
}

/*************************************************************************
|*
|* SvxXConnectionPreview: MouseButtonDown()
|*
*************************************************************************/

void SvxXConnectionPreview::MouseButtonDown( const MouseEvent& rMEvt )
{
    BOOL bZoomIn  = rMEvt.IsLeft() && !rMEvt.IsShift();
    BOOL bZoomOut = rMEvt.IsRight() || rMEvt.IsShift();
    BOOL bCtrl    = rMEvt.IsMod1();

    if( bZoomIn || bZoomOut )
    {
        MapMode aMapMode = GetMapMode();
        Fraction aXFrac = aMapMode.GetScaleX();
        Fraction aYFrac = aMapMode.GetScaleY();
        Fraction* pMultFrac;

        if( bZoomIn )
        {
            if( bCtrl )
                pMultFrac = new Fraction( 3, 2 );
            else
                pMultFrac = new Fraction( 11, 10 );
        }
        else
        {
            if( bCtrl )
                pMultFrac = new Fraction( 2, 3 );
            else
                pMultFrac = new Fraction( 10, 11 );
        }

        aXFrac *= *pMultFrac;
        aYFrac *= *pMultFrac;
        if( (double)aXFrac > 0.001 && (double)aXFrac < 1000.0 &&
            (double)aYFrac > 0.001 && (double)aYFrac < 1000.0 )
        {
            aMapMode.SetScaleX( aXFrac );
            aMapMode.SetScaleY( aYFrac );
            SetMapMode( aMapMode );

            Size aOutSize( GetOutputSize() );

            Point aPt( aMapMode.GetOrigin() );
            long nX = (long)( ( (double)aOutSize.Width() - ( (double)aOutSize.Width() * (double)*pMultFrac  ) ) / 2.0 + 0.5 );
            long nY = (long)( ( (double)aOutSize.Height() - ( (double)aOutSize.Height() * (double)*pMultFrac  ) ) / 2.0 + 0.5 );
            aPt.X() +=  nX;
            aPt.Y() +=  nY;

            aMapMode.SetOrigin( aPt );
            SetMapMode( aMapMode );

            Invalidate();
        }
        delete pMultFrac;
    }
}

void SvxXConnectionPreview::SetStyles()
{
    const StyleSettings& rStyles = Application::GetSettings().GetStyleSettings();
    SetDrawMode( GetDisplayBackground().GetColor().IsDark() ? OUTPUT_DRAWMODE_CONTRAST : OUTPUT_DRAWMODE_COLOR );
    SetBackground( Wallpaper( Color( rStyles.GetFieldColor() ) ) );
}

void SvxXConnectionPreview::DataChanged( const DataChangedEvent& rDCEvt )
{
    Control::DataChanged( rDCEvt );

    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) && (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        SetStyles();
    }
}

