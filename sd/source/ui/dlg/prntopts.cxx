/*************************************************************************
 *
 *  $RCSfile: prntopts.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: hr $ $Date: 2004-05-10 15:44:42 $
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

#pragma hdrstop

#include "sdattr.hxx"
#include "optsitem.hxx"
#include "prntopts.hrc"
#include "sdresid.hxx"
#include "prntopts.hxx"
#include "app.hrc" //CHINA001
#ifndef _SFXINTITEM_HXX //CHINA001
#include <svtools/intitem.hxx> //CHINA001
#endif //CHINA001
#include <sfx2/request.hxx> //CHINA001
// STATIC DATA -----------------------------------------------------------

static USHORT pPrintOptRanges[] =
{
    ATTR_OPTIONS_PRINT,
    ATTR_OPTIONS_PRINT,
    0
};


/*************************************************************************
|*
|*  Dialog zum Aendern der Print-Optionen
|*
\************************************************************************/

SdPrintOptions::SdPrintOptions( Window* pParent, const SfxItemSet& rInAttrs ) :
        SfxTabPage          ( pParent, SdResId( TP_PRINT_OPTIONS ), rInAttrs ),
        //aLbPrint          ( this, SdResId( LB_PRINT ) ),

        aCbxDraw                ( this, SdResId( CBX_DRAW ) ),
        aCbxNotes               ( this, SdResId( CBX_NOTES ) ),
        aCbxHandout             ( this, SdResId( CBX_HANDOUTS ) ),
        aCbxOutline             ( this, SdResId( CBX_OUTLINE ) ),
        aGrpPrint               ( this, SdResId( GRP_PRINT ) ),

        aSeparator1FL            ( this, SdResId( FL_SEPARATOR1 ) ),
        aCbxDate                ( this, SdResId( CBX_DATE ) ),
        aCbxTime                ( this, SdResId( CBX_TIME ) ),
        aCbxPagename            ( this, SdResId( CBX_PAGENAME ) ),
        aGrpPrintExt            ( this, SdResId( GRP_PRINT_EXT ) ),

        aCbxHiddenPages         ( this, SdResId( CBX_HIDDEN_PAGES ) ),
        aRbtDefault             ( this, SdResId( RBT_DEFAULT ) ),
        aRbtPagesize            ( this, SdResId( RBT_PAGESIZE ) ),
        aRbtPagetile            ( this, SdResId( RBT_PAGETILE ) ),
        aRbtBooklet             ( this, SdResId( RBT_BOOKLET ) ),
        aCbxFront               ( this, SdResId( CBX_FRONT ) ),
        aCbxBack                ( this, SdResId( CBX_BACK ) ),
        aGrpPageoptions         ( this, SdResId( GRP_PAGE ) ),

/// Neu
        aRbtColor               ( this, SdResId( RBT_COLOR ) ),
        aRbtGrayscale           ( this, SdResId( RBT_GRAYSCALE ) ),
        aRbtBlackWhite          ( this, SdResId( RBT_BLACKWHITE ) ),
        aGrpOutput              ( this, SdResId( GRP_OUTPUT ) ),
/// Neu

        aCbxPaperbin            ( this, SdResId( CBX_PAPERBIN ) ),
        aSeparator2FL            ( this, SdResId( FL_SEPARATOR2 ) ),

        rOutAttrs               ( rInAttrs )
{
    FreeResource();

    Link aLink = LINK( this, SdPrintOptions, ClickBookletHdl );
    aRbtDefault.SetClickHdl( aLink );
    aRbtPagesize.SetClickHdl( aLink );
    aRbtPagetile.SetClickHdl( aLink );
    aRbtBooklet.SetClickHdl( aLink );

    aLink = LINK( this, SdPrintOptions, ClickCheckboxHdl );
    aCbxDraw.SetClickHdl( aLink );
    aCbxNotes.SetClickHdl( aLink );
    aCbxHandout.SetClickHdl( aLink );
    aCbxOutline.SetClickHdl( aLink );
}

// -----------------------------------------------------------------------

SdPrintOptions::~SdPrintOptions()
{
}

// -----------------------------------------------------------------------

BOOL SdPrintOptions::FillItemSet( SfxItemSet& rAttrs )
{
    if( aCbxDraw.GetSavedValue() != aCbxDraw.IsChecked() ||
        aCbxNotes.GetSavedValue() != aCbxNotes.IsChecked() ||
        aCbxHandout.GetSavedValue() != aCbxHandout.IsChecked() ||
        aCbxOutline.GetSavedValue() != aCbxOutline.IsChecked() ||
        aCbxDate.GetSavedValue() != aCbxDate.IsChecked() ||
        aCbxTime.GetSavedValue() != aCbxTime.IsChecked() ||
        aCbxPagename.GetSavedValue() != aCbxPagename.IsChecked() ||
        aCbxHiddenPages.GetSavedValue() != aCbxHiddenPages.IsChecked() ||
        aRbtPagesize.GetSavedValue() != aRbtPagesize.IsChecked() ||
        aRbtPagetile.GetSavedValue() != aRbtPagetile.IsChecked() ||
        aRbtBooklet.GetSavedValue() != aRbtBooklet.IsChecked() ||
        aCbxFront.GetSavedValue() != aCbxFront.IsChecked() ||
        aCbxBack.GetSavedValue() != aCbxBack.IsChecked() ||
        aCbxPaperbin.GetSavedValue() != aCbxPaperbin.IsChecked() ||
/// Neu
        aRbtColor.GetSavedValue() != aRbtColor.IsChecked() ||
        aRbtGrayscale.GetSavedValue() != aRbtGrayscale.IsChecked() ||
        aRbtBlackWhite.GetSavedValue() != aRbtBlackWhite.IsChecked() )
/// Neu
    {
        SdOptionsPrintItem aOptions( ATTR_OPTIONS_PRINT );

        aOptions.SetDraw( aCbxDraw.IsChecked() );
        aOptions.SetNotes( aCbxNotes.IsChecked() );
        aOptions.SetHandout( aCbxHandout.IsChecked() );
        aOptions.SetOutline( aCbxOutline.IsChecked() );
        aOptions.SetDate( aCbxDate.IsChecked() );
        aOptions.SetTime( aCbxTime.IsChecked() );
        aOptions.SetPagename( aCbxPagename.IsChecked() );
        aOptions.SetHiddenPages( aCbxHiddenPages.IsChecked() );
        aOptions.SetPagesize( aRbtPagesize.IsChecked() );
        aOptions.SetPagetile( aRbtPagetile.IsChecked() );
        aOptions.SetBooklet( aRbtBooklet.IsChecked() );
        aOptions.SetFrontPage( aCbxFront.IsChecked() );
        aOptions.SetBackPage( aCbxBack.IsChecked() );
        aOptions.SetPaperbin( aCbxPaperbin.IsChecked() );

/// Neu
        UINT16 nQuality = 0; // Standard, also Color
        if( aRbtGrayscale.IsChecked() )
            nQuality = 1;
        if( aRbtBlackWhite.IsChecked() )
            nQuality = 2;
        aOptions.SetOutputQuality( nQuality );
/// Neu

        rAttrs.Put( aOptions );

        return( TRUE );
    }
    return( FALSE );
}

// -----------------------------------------------------------------------

void SdPrintOptions::Reset( const SfxItemSet& rAttrs )
{
    const SdOptionsPrintItem* pPrintOpts = NULL;
    if( SFX_ITEM_SET == rAttrs.GetItemState( ATTR_OPTIONS_PRINT, FALSE,
                            (const SfxPoolItem**) &pPrintOpts ) )
    {
        aCbxDraw.Check(              pPrintOpts->IsDraw() );
        aCbxNotes.Check(             pPrintOpts->IsNotes() );
        aCbxHandout.Check(           pPrintOpts->IsHandout() );
        aCbxOutline.Check(           pPrintOpts->IsOutline() );
        aCbxDate.Check(              pPrintOpts->IsDate() );
        aCbxTime.Check(              pPrintOpts->IsTime() );
        aCbxPagename.Check(          pPrintOpts->IsPagename() );
        aCbxHiddenPages.Check(       pPrintOpts->IsHiddenPages() );
        aRbtPagesize.Check(          pPrintOpts->IsPagesize() );
        aRbtPagetile.Check(          pPrintOpts->IsPagetile() );
        aRbtBooklet.Check(           pPrintOpts->IsBooklet() );
        aCbxFront.Check(             pPrintOpts->IsFrontPage() );
        aCbxBack.Check(              pPrintOpts->IsBackPage() );
        aCbxPaperbin.Check(          pPrintOpts->IsPaperbin() );

        if( !aRbtPagesize.IsChecked() &&
            !aRbtPagetile.IsChecked() &&
            !aRbtBooklet.IsChecked() )
        {
            aRbtDefault.Check();
        }

/// Neu
        UINT16 nQuality = pPrintOpts->GetOutputQuality();
        if( nQuality == 0 )
            aRbtColor.Check();
        else if( nQuality == 1 )
            aRbtGrayscale.Check();
        else
            aRbtBlackWhite.Check();
/// Neu

    }
    aCbxDraw.SaveValue();
    aCbxNotes.SaveValue();
    aCbxHandout.SaveValue();
    aCbxOutline.SaveValue();
    aCbxDate.SaveValue();
    aCbxTime.SaveValue();
    aCbxPagename.SaveValue();
    aCbxHiddenPages.SaveValue();
    aRbtPagesize.SaveValue();
    aRbtPagetile.SaveValue();
    aRbtBooklet.SaveValue();
    aCbxPaperbin.SaveValue();
/// Neu
    aRbtColor.SaveValue();
    aRbtGrayscale.SaveValue();
    aRbtBlackWhite.SaveValue();
/// Neu

    ClickBookletHdl( NULL );
}

// -----------------------------------------------------------------------

SfxTabPage* SdPrintOptions::Create( Window* pWindow,
                const SfxItemSet& rOutAttrs )
{
    return( new SdPrintOptions( pWindow, rOutAttrs ) );
}

//-----------------------------------------------------------------------

USHORT* SdPrintOptions::GetRanges()
{
    return pPrintOptRanges;
}

//-----------------------------------------------------------------------

IMPL_LINK( SdPrintOptions, ClickCheckboxHdl, CheckBox *, pCbx )
{
    BOOL bPageNameAllowed = TRUE;

    // there must be at least one of them checked
    if( !aCbxDraw.IsChecked() && !aCbxNotes.IsChecked() && !aCbxOutline.IsChecked() && !aCbxHandout.IsChecked() )
        pCbx->Check();

    updateControls();
    return 0;
}

//-----------------------------------------------------------------------

IMPL_LINK( SdPrintOptions, ClickBookletHdl, CheckBox *, EMPTYARG )
{
    updateControls();
    return 0;
}

void SdPrintOptions::updateControls()
{
    aCbxFront.Enable(aRbtBooklet.IsChecked());
    aCbxBack.Enable(aRbtBooklet.IsChecked());

    aCbxDate.Enable( !aRbtBooklet.IsChecked() );
    aCbxTime.Enable( !aRbtBooklet.IsChecked() );

    aCbxPagename.Enable( !aRbtBooklet.IsChecked() && (aCbxDraw.IsChecked() || aCbxNotes.IsChecked() || aCbxOutline.IsChecked()) );
}

/* -----------------------------04.05.01 10:53--------------------------------

 ---------------------------------------------------------------------------*/
void lcl_MoveRB_Impl(Window& rBtn, long nXDiff)
{
    Point aPos(rBtn.GetPosPixel());
    aPos.X() -= nXDiff;
    rBtn.SetPosPixel(aPos);
}

void    SdPrintOptions::SetDrawMode()
{
    if(aCbxNotes.IsVisible())
    {
        aCbxNotes.Hide();
        aCbxHandout.Hide();
        aCbxOutline.Hide();
        aCbxDraw.Hide();
        aGrpPrint.Hide();

        aSeparator1FL.Hide();
        long nXDiff = aGrpOutput.GetPosPixel().X() - aGrpPrint.GetPosPixel().X();
        lcl_MoveRB_Impl(aRbtColor, nXDiff);
        lcl_MoveRB_Impl(aRbtGrayscale, nXDiff);
        lcl_MoveRB_Impl(aRbtBlackWhite, nXDiff);
        lcl_MoveRB_Impl(aGrpOutput, nXDiff);

        long nWidth =  aGrpOutput.GetSizePixel().Width() + nXDiff;
        Size aSize(aGrpOutput.GetSizePixel());
        aSize.Width() = nWidth;
        aGrpOutput.SetSizePixel(aSize);
    }
}

void SdPrintOptions::PageCreated (SfxAllItemSet aSet) //add CHINA001
{
    SFX_ITEMSET_ARG (&aSet,pFlagItem,SfxUInt32Item,SID_SDMODE_FLAG,sal_False);
    if (pFlagItem)
    {
        UINT32 nFlags=pFlagItem->GetValue();
        if ( ( nFlags & SD_DRAW_MODE ) == SD_DRAW_MODE )
            SetDrawMode();
    }
}

