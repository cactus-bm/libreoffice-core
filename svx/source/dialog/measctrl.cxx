/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: measctrl.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 17:20:49 $
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

#include <svx/xoutx.hxx>
#include <svx/svdomeas.hxx>
#include <svx/svdmodel.hxx>

//#include "svdrwobj.hxx" // SdrPaintInfoRec

#include "measctrl.hxx"
#include <svx/dialmgr.hxx>
#include "dlgutil.hxx"

/*************************************************************************
|*
|* Ctor SvxXMeasurePreview
|*
*************************************************************************/

SvxXMeasurePreview::SvxXMeasurePreview
(
    Window* pParent,
    const ResId& rResId,
    const SfxItemSet& rInAttrs
) :

    Control ( pParent, rResId ),
    rAttrs  ( rInAttrs )

{
    pExtOutDev = new XOutputDevice( this );

    SetMapMode( MAP_100TH_MM );

    Size aSize = GetOutputSize();

    // Massstab: 1:2
    MapMode aMapMode = GetMapMode();
    aMapMode.SetScaleX( Fraction( 1, 2 ) );
    aMapMode.SetScaleY( Fraction( 1, 2 ) );
    SetMapMode( aMapMode );

    aSize = GetOutputSize();
    Rectangle aRect = Rectangle( Point(), aSize );
    Point aPt1 = Point( aSize.Width() / 5, (long) ( aSize.Height() / 2 ) );
    Point aPt2 = Point( aSize.Width() * 4 / 5, (long) ( aSize.Height() / 2 ) );

    pMeasureObj = new SdrMeasureObj( aPt1, aPt2 );
    pModel = new SdrModel();
    pMeasureObj->SetModel( pModel );

    //pMeasureObj->SetItemSetAndBroadcast(rInAttrs);
    pMeasureObj->SetMergedItemSetAndBroadcast(rInAttrs);

    SetDrawMode( GetDisplayBackground().GetColor().IsDark() ? OUTPUT_DRAWMODE_CONTRAST : OUTPUT_DRAWMODE_COLOR );

    Invalidate();
}

/*************************************************************************
|*
|* Dtor SvxXMeasurePreview
|*
*************************************************************************/

SvxXMeasurePreview::~SvxXMeasurePreview()
{
    delete pExtOutDev;

    // #111111#
    // No one is deleting the MeasureObj? This is not only an error but also
    // a memory leak (!). Main problem is that this object is still listening to
    // a StyleSheet of the model which was set. Thus, if You want to keep the obnject,
    // set the modfel to 0L, if object is not needed (seems to be the case here),
    // delete it.
    delete pMeasureObj;

    delete pModel;
}

/*************************************************************************
|*
|* SvxXMeasurePreview: Paint()
|*
*************************************************************************/

void SvxXMeasurePreview::Paint( const Rectangle&  )
{
    SdrPaintInfoRec aInfoRec;

    pMeasureObj->SingleObjectPainter( *pExtOutDev, aInfoRec ); // #110094#-17
}

/*************************************************************************
|*
|* SvxXMeasurePreview: SetAttributes()
|*
*************************************************************************/

void SvxXMeasurePreview::SetAttributes( const SfxItemSet& rInAttrs )
{
    //pMeasureObj->SetItemSetAndBroadcast(rInAttrs);
    pMeasureObj->SetMergedItemSetAndBroadcast(rInAttrs);

    Invalidate();
}

/*************************************************************************
|*
|* SvxXMeasurePreview: SetAttributes()
|*
*************************************************************************/

void SvxXMeasurePreview::MouseButtonDown( const MouseEvent& rMEvt )
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

// -----------------------------------------------------------------------

void SvxXMeasurePreview::DataChanged( const DataChangedEvent& rDCEvt )
{
    Control::DataChanged( rDCEvt );

    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) && (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        SetDrawMode( GetDisplayBackground().GetColor().IsDark() ? OUTPUT_DRAWMODE_CONTRAST : OUTPUT_DRAWMODE_COLOR );
    }
}

