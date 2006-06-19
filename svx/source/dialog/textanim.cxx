/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: textanim.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 15:33:03 $
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

#ifdef SVX_DLLIMPLEMENTATION
#undef SVX_DLLIMPLEMENTATION
#endif

// include ---------------------------------------------------------------

#ifndef _SFXAPP_HXX
#include <sfx2/app.hxx>
#endif
#ifndef _SFXMODULE_HXX
#include <sfx2/module.hxx>
#endif

#ifndef _SHL_HXX //autogen
#include <tools/shl.hxx>
#endif

#include "dialogs.hrc"

#ifndef _SVX_TEXTANIM_CXX
#define _SVX_TEXTANIM_CXX
#endif
#include "textanim.hxx"
#include "textanim.hrc"
#include "textattr.hxx"
#include "dialmgr.hxx"
#include "dlgutil.hxx"

static USHORT pRanges[] =
{
    SDRATTR_TEXT_ANIKIND,
    SDRATTR_TEXT_ANIAMOUNT,
    0
};

/*************************************************************************
|*
|* Konstruktor des Tab-Dialogs: Fuegt die Seiten zum Dialog hinzu
|*
\************************************************************************/

__EXPORT SvxTextTabDialog::SvxTextTabDialog( Window* pParent,
                                const SfxItemSet* pAttr,
                                const SdrView* pSdrView ) :
        SfxTabDialog        ( pParent, ResId( RID_SVXDLG_TEXT, DIALOG_MGR() ), pAttr ),
        rOutAttrs           ( *pAttr ),
        pView               ( pSdrView )
{
    FreeResource();

    AddTabPage( RID_SVXPAGE_TEXTATTR, SvxTextAttrPage::Create, 0);
    AddTabPage( RID_SVXPAGE_TEXTANIMATION, SvxTextAnimationPage::Create, 0);
}

/*************************************************************************
|*
|* PageCreated()
|*
\************************************************************************/

void __EXPORT SvxTextTabDialog::PageCreated( USHORT nId, SfxTabPage &rPage )
{
    switch( nId )
    {
        case RID_SVXPAGE_TEXTATTR:
        {
            ( (SvxTextAttrPage&) rPage ).SetView( pView );
            ( (SvxTextAttrPage&) rPage ).Construct();
        }
        break;

        case RID_SVXPAGE_TEXTANIMATION:
        break;

        default:
        break;
    }
}


/*************************************************************************
|*
|* Page
|*
\************************************************************************/

SvxTextAnimationPage::SvxTextAnimationPage( Window* pWindow, const SfxItemSet& rInAttrs ) :
                SfxTabPage      ( pWindow, ResId( RID_SVXPAGE_TEXTANIMATION, DIALOG_MGR() ),
                                  rInAttrs ),
                aFlEffect       ( this, ResId(FL_EFFECT)),
                aFtEffects      ( this, ResId(FT_EFFECTS)),
                aLbEffect       ( this, ResId( LB_EFFECT ) ),
                //aCtlEffect        ( this, ResId( CTL_EFFECT ) ),
                aFtDirection    ( this, ResId(FT_DIRECTION) ),
                aBtnUp          ( this, ResId( BTN_UP ) ),
                aBtnLeft        ( this, ResId( BTN_LEFT ) ),
                aBtnRight       ( this, ResId( BTN_RIGHT ) ),
                aBtnDown        ( this, ResId( BTN_DOWN ) ),

                aFlProperties   ( this, ResId(FL_PROPERTIES)),
                aTsbStartInside ( this, ResId( TSB_START_INSIDE ) ),
                aTsbStopInside  ( this, ResId( TSB_STOP_INSIDE ) ),

                aFtCount        ( this, ResId(FT_COUNT)),
                aTsbEndless     ( this, ResId( TSB_ENDLESS ) ),
                aNumFldCount    ( this, ResId( NUM_FLD_COUNT ) ),

                aFtAmount       ( this, ResId(FT_AMOUNT)),
                aTsbPixel       ( this, ResId( TSB_PIXEL ) ),
                aMtrFldAmount   ( this, ResId( MTR_FLD_AMOUNT ) ),

                aFtDelay        ( this, ResId(FT_DELAY)),
                aTsbAuto        ( this, ResId( TSB_AUTO ) ),
                aMtrFldDelay    ( this, ResId( MTR_FLD_DELAY ) ),

                rOutAttrs       ( rInAttrs ),
                eAniKind        ( SDRTEXTANI_NONE )
{
    FreeResource();

    eFUnit = GetModuleFieldUnit( &rInAttrs );
    SfxItemPool* pPool = rOutAttrs.GetPool();
    DBG_ASSERT( pPool, "Wo ist der Pool" );
    eUnit = pPool->GetMetric( SDRATTR_TEXT_LEFTDIST );

    Construct();

    aLbEffect.SetSelectHdl(
        LINK( this, SvxTextAnimationPage, SelectEffectHdl_Impl ) );
    aTsbEndless.SetClickHdl(
        LINK( this, SvxTextAnimationPage, ClickEndlessHdl_Impl ) );
    aTsbAuto.SetClickHdl(
        LINK( this, SvxTextAnimationPage, ClickAutoHdl_Impl ) );
    aTsbPixel.SetClickHdl(
        LINK( this, SvxTextAnimationPage, ClickPixelHdl_Impl ) );

    Link aLink( LINK( this, SvxTextAnimationPage, ClickDirectionHdl_Impl ) );
    aBtnUp.SetClickHdl( aLink );
    aBtnLeft.SetClickHdl( aLink );
    aBtnRight.SetClickHdl( aLink );
    aBtnDown.SetClickHdl( aLink );
}

/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/

SvxTextAnimationPage::~SvxTextAnimationPage()
{
}

/*************************************************************************
|*
|* Liest uebergebenen Item-Set
|*
\************************************************************************/

void __EXPORT SvxTextAnimationPage::Reset( const SfxItemSet& rAttrs )
{
    const SfxItemPool* pPool = rAttrs.GetPool();

    // Animationstyp
    const SfxPoolItem* pItem = GetItem( rAttrs, SDRATTR_TEXT_ANIKIND );

    if( !pItem )
        pItem = &pPool->GetDefaultItem( SDRATTR_TEXT_ANIKIND );
    if( pItem )
    {
        eAniKind = ( ( const SdrTextAniKindItem* )pItem )->GetValue();
        aLbEffect.SelectEntryPos( eAniKind );
    }
    else
        aLbEffect.SetNoSelection();
    aLbEffect.SaveValue();

    // Animationsrichtung
    pItem = GetItem( rAttrs, SDRATTR_TEXT_ANIDIRECTION );
    if( !pItem )
        pItem = &pPool->GetDefaultItem( SDRATTR_TEXT_ANIDIRECTION );
    if( pItem )
    {
        SdrTextAniDirection eValue = ( ( const SdrTextAniDirectionItem* )pItem )->GetValue();
        SelectDirection( eValue );
    }
    else
    {
        aBtnUp.Check( FALSE );
        aBtnLeft.Check( FALSE );
        aBtnRight.Check( FALSE );
        aBtnDown.Check( FALSE );
    }
    aBtnUp.SaveValue();
    aBtnLeft.SaveValue();
    aBtnRight.SaveValue();
    aBtnDown.SaveValue();

    // Start inside
    pItem = GetItem( rAttrs, SDRATTR_TEXT_ANISTARTINSIDE );
    if( !pItem )
        pItem = &pPool->GetDefaultItem( SDRATTR_TEXT_ANISTARTINSIDE );
    if( pItem )
    {
        aTsbStartInside.EnableTriState( FALSE );
        BOOL bValue = ( ( const SdrTextAniStartInsideItem* )pItem )->GetValue();
        if( bValue )
            aTsbStartInside.SetState( STATE_CHECK );
        else
            aTsbStartInside.SetState( STATE_NOCHECK );
    }
    else
        aTsbStartInside.SetState( STATE_DONTKNOW );
    aTsbStartInside.SaveValue();

    // Stop inside
    pItem = GetItem( rAttrs, SDRATTR_TEXT_ANISTOPINSIDE );
    if( !pItem )
        pItem = &pPool->GetDefaultItem( SDRATTR_TEXT_ANISTOPINSIDE );
    if( pItem )
    {
        aTsbStopInside.EnableTriState( FALSE );
        BOOL bValue = ( ( const SdrTextAniStopInsideItem* )pItem )->GetValue();
        if( bValue )
            aTsbStopInside.SetState( STATE_CHECK );
        else
            aTsbStopInside.SetState( STATE_NOCHECK );
    }
    else
        aTsbStopInside.SetState( STATE_DONTKNOW );
    aTsbStopInside.SaveValue();

    // Anzahl
    pItem = GetItem( rAttrs, SDRATTR_TEXT_ANICOUNT );
    if( !pItem )
        pItem = &pPool->GetDefaultItem( SDRATTR_TEXT_ANICOUNT );
    if( pItem )
    {
        aTsbEndless.EnableTriState( FALSE );
        long nValue = (long) ( ( const SdrTextAniCountItem* )pItem )->GetValue();
        aNumFldCount.SetValue( nValue );
        if( nValue == 0 )
        {
            if( eAniKind == SDRTEXTANI_SLIDE )
            {
                aTsbEndless.SetState( STATE_NOCHECK );
                aTsbEndless.Enable( FALSE );
            }
            else
            {
                aTsbEndless.SetState( STATE_CHECK );
                aNumFldCount.SetEmptyFieldValue();
            }
        }
        else
            aTsbEndless.SetState( STATE_NOCHECK );
    }
    else
    {
        aNumFldCount.SetEmptyFieldValue();
        aTsbEndless.SetState( STATE_DONTKNOW );
    }
    aTsbEndless.SaveValue();
    aNumFldCount.SaveValue();

    // Verzoegerung
    pItem = GetItem( rAttrs, SDRATTR_TEXT_ANIDELAY );
    if( !pItem )
        pItem = &pPool->GetDefaultItem( SDRATTR_TEXT_ANIDELAY );
    if( pItem )
    {
        aTsbAuto.EnableTriState( FALSE );
        long nValue = (long) ( ( const SdrTextAniDelayItem* )pItem )->GetValue();
        aMtrFldDelay.SetValue( nValue );
        if( nValue == 0 )
        {
            aTsbAuto.SetState( STATE_CHECK );
            aMtrFldDelay.SetEmptyFieldValue();
        }
        else
            aTsbAuto.SetState( STATE_NOCHECK );
    }
    else
    {
        aMtrFldDelay.SetEmptyFieldValue();
        aTsbAuto.SetState( STATE_DONTKNOW );
    }
    aTsbAuto.SaveValue();
    aMtrFldDelay.SaveValue();

    // Schrittweite
    pItem = GetItem( rAttrs, SDRATTR_TEXT_ANIAMOUNT );
    if( !pItem )
        pItem = &pPool->GetDefaultItem( SDRATTR_TEXT_ANIAMOUNT );
    if( pItem )
    {
        aTsbPixel.EnableTriState( FALSE );
        long nValue = (long) ( ( const SdrTextAniAmountItem* )pItem )->GetValue();
        if( nValue <= 0 )
        {
            aTsbPixel.SetState( STATE_CHECK );
            nValue = -nValue;
            if( nValue == 0 )
                nValue++;
            aMtrFldAmount.SetUnit( FUNIT_CUSTOM );
            aMtrFldAmount.SetDecimalDigits( 0 );

            aMtrFldAmount.SetSpinSize( 1 );
            aMtrFldAmount.SetMin( 1 );
            aMtrFldAmount.SetFirst( 1 );
            aMtrFldAmount.SetMax( 100 );
            aMtrFldAmount.SetLast( 100 );

            aMtrFldAmount.SetValue( nValue );
        }
        else
        {
            aTsbPixel.SetState( STATE_NOCHECK );
            aMtrFldAmount.SetUnit( eFUnit );
            aMtrFldAmount.SetDecimalDigits( 2 );

            aMtrFldAmount.SetSpinSize( 10 );
            aMtrFldAmount.SetMin( 1 );
            aMtrFldAmount.SetFirst( 1 );
            aMtrFldAmount.SetMax( 10000 );
            aMtrFldAmount.SetLast( 10000 );

            SetMetricValue( aMtrFldAmount, nValue, eUnit );
        }
    }
    else
    {
        aMtrFldAmount.Disable();
        aMtrFldAmount.SetEmptyFieldValue();
        aTsbPixel.SetState( STATE_DONTKNOW );
    }
    aTsbPixel.SaveValue();
    aMtrFldAmount.SaveValue();


    SelectEffectHdl_Impl( NULL );
    ClickEndlessHdl_Impl( NULL );
    ClickAutoHdl_Impl( NULL );
    //ClickPixelHdl_Impl( NULL );
}

/*************************************************************************
|*
|* Fuellt uebergebenen Item-Set mit Dialogbox-Attributen
|*
\************************************************************************/

BOOL SvxTextAnimationPage::FillItemSet( SfxItemSet& rAttrs)
{
    BOOL bModified = FALSE;
    USHORT nPos;
    TriState eState;

    // Animationstyp
    nPos = aLbEffect.GetSelectEntryPos();
    if( nPos != LISTBOX_ENTRY_NOTFOUND &&
        nPos != aLbEffect.GetSavedValue() )
    {
        rAttrs.Put( SdrTextAniKindItem( (SdrTextAniKind) nPos ) );
        bModified = TRUE;
    }

    // Animationsrichtung
    if( aBtnUp.GetSavedValue() != aBtnUp.IsChecked() ||
        aBtnLeft.GetSavedValue() != aBtnLeft.IsChecked() ||
        aBtnRight.GetSavedValue() != aBtnRight.IsChecked() ||
        aBtnDown.GetSavedValue() != aBtnDown.IsChecked() )
    {
        SdrTextAniDirection eValue = (SdrTextAniDirection) GetSelectedDirection();
        rAttrs.Put( SdrTextAniDirectionItem( eValue ) );
        bModified = TRUE;
    }

    // Start inside
    eState = aTsbStartInside.GetState();
    if( eState != aTsbStartInside.GetSavedValue() )
    {
        rAttrs.Put( SdrTextAniStartInsideItem( (BOOL) STATE_CHECK == eState ) );
        bModified = TRUE;
    }

    // Stop inside
    eState = aTsbStopInside.GetState();
    if( eState != aTsbStopInside.GetSavedValue() )
    {
        rAttrs.Put( SdrTextAniStopInsideItem( (BOOL) STATE_CHECK == eState ) );
        bModified = TRUE;
    }

    // Anzahl
    eState = aTsbEndless.GetState();
    String aStr = aNumFldCount.GetText();
    if( eState != aTsbEndless.GetSavedValue() ||
        aStr != aNumFldCount.GetSavedValue() )
    {
        long nValue = 0;
        if( eState == STATE_CHECK /*#89844#*/ && aTsbEndless.IsEnabled())
            bModified = TRUE;
        else
        {
            if( aStr != aNumFldCount.GetSavedValue() )
            {
                nValue = aNumFldCount.GetValue();
                bModified = TRUE;
            }
        }
        if( bModified )
            rAttrs.Put( SdrTextAniCountItem( (UINT16) nValue ) );
    }

    // Verzoegerung
    eState = aTsbAuto.GetState();
    aStr = aMtrFldDelay.GetText();
    if( eState != aTsbAuto.GetSavedValue() ||
        aStr != aMtrFldDelay.GetSavedValue() )
    {
        long nValue = 0;
        if( eState == STATE_CHECK )
            bModified = TRUE;
        else
        {
            if( aStr != aMtrFldDelay.GetSavedValue() )
            {
                nValue = aMtrFldDelay.GetValue();
                bModified = TRUE;
            }
        }
        if( bModified )
            rAttrs.Put( SdrTextAniDelayItem( (UINT16) nValue ) );
    }

    // Schrittweite
    eState = aTsbPixel.GetState();
    aStr = aMtrFldAmount.GetText();
    if( eState != aTsbPixel.GetSavedValue() ||
        aStr != aMtrFldAmount.GetSavedValue() )
    {
        long nValue = 0;
        if( eState == STATE_CHECK )
        {
            nValue = aMtrFldAmount.GetValue();
            nValue = -nValue;
        }
        else
        {
            nValue = GetCoreValue( aMtrFldAmount, eUnit );
        }
        rAttrs.Put( SdrTextAniAmountItem( (INT16) nValue ) );

        bModified = TRUE;
    }

    return( bModified );
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void SvxTextAnimationPage::Construct()
{
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

USHORT* SvxTextAnimationPage::GetRanges()
{
    return( pRanges );
}

/*************************************************************************
|*
|* Erzeugt die Seite
|*
\************************************************************************/

SfxTabPage* SvxTextAnimationPage::Create( Window* pWindow,
                const SfxItemSet& rAttrs )
{
    return( new SvxTextAnimationPage( pWindow, rAttrs ) );
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

IMPL_LINK( SvxTextAnimationPage, SelectEffectHdl_Impl, void *, EMPTYARG )
{
    USHORT nPos = aLbEffect.GetSelectEntryPos();
    if( nPos != LISTBOX_ENTRY_NOTFOUND )
    {
        eAniKind = (SdrTextAniKind) nPos;
        switch( eAniKind )
        {
            case SDRTEXTANI_NONE:
            {
                aFtDirection.Disable();
                aBtnUp.Disable();
                aBtnLeft.Disable();
                aBtnRight.Disable();
                aBtnDown.Disable();
                aTsbStartInside.Disable();
                aTsbStopInside.Disable();

                aTsbEndless.Disable();
                aNumFldCount.Disable();
                aFtCount.Disable();

                aTsbAuto.Disable();
                aMtrFldDelay.Disable();
                aFtDelay.Disable();

                aTsbPixel.Disable();
                aMtrFldAmount.Disable();
                aFtAmount.Disable();
            }
            break;

            case SDRTEXTANI_BLINK:
            case SDRTEXTANI_SCROLL:
            case SDRTEXTANI_ALTERNATE:
            case SDRTEXTANI_SLIDE:
            {
                if( eAniKind == SDRTEXTANI_SLIDE )
                {
                    aTsbStartInside.Disable();
                    aTsbStopInside.Disable();

                    aTsbEndless.Disable();
                    aNumFldCount.Enable();
                    aNumFldCount.SetValue( aNumFldCount.GetValue() );
                }
                else
                {
                    aTsbStartInside.Enable();
                    aTsbStopInside.Enable();

                    aTsbEndless.Enable();
                    ClickEndlessHdl_Impl( NULL );
                }
                aFtCount.Enable();

                aTsbAuto.Enable();
                aFtDelay.Enable();
                ClickAutoHdl_Impl( NULL );

                if( eAniKind == SDRTEXTANI_BLINK )
                {
                    aFtDirection.Disable();
                    aBtnUp.Disable();
                    aBtnLeft.Disable();
                    aBtnRight.Disable();
                    aBtnDown.Disable();

                    aTsbPixel.Disable();
                    aMtrFldAmount.Disable();
                    aFtAmount.Disable();
                }
                else
                {
                    aFtDirection.Enable();
                    aBtnUp.Enable();
                    aBtnLeft.Enable();
                    aBtnRight.Enable();
                    aBtnDown.Enable();

                    aTsbPixel.Enable();
                    aMtrFldAmount.Enable();
                    aFtAmount.Enable();
                }
            }
            break;
        }

    }
    return( 0L );
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

IMPL_LINK( SvxTextAnimationPage, ClickEndlessHdl_Impl, void *, EMPTYARG )
{

    if( eAniKind != SDRTEXTANI_SLIDE )
    {
        TriState eState = aTsbEndless.GetState();
        if( eState != STATE_NOCHECK )
        {
            aNumFldCount.Disable();
            aNumFldCount.SetEmptyFieldValue();
        }
        else
        {
            aNumFldCount.Enable();
            aNumFldCount.SetValue( aNumFldCount.GetValue() );
        }
    }
    return( 0L );
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

IMPL_LINK( SvxTextAnimationPage, ClickAutoHdl_Impl, void *, EMPTYARG )
{
    TriState eState = aTsbAuto.GetState();
    if( eState != STATE_NOCHECK )
    {
        aMtrFldDelay.Disable();
        aMtrFldDelay.SetEmptyFieldValue();
    }
    else
    {
        aMtrFldDelay.Enable();
        aMtrFldDelay.SetValue( aMtrFldDelay.GetValue() );
    }

    return( 0L );
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

IMPL_LINK( SvxTextAnimationPage, ClickPixelHdl_Impl, void *, EMPTYARG )
{
    TriState eState = aTsbPixel.GetState();
    if( eState == STATE_CHECK )
    {
        long nValue = aMtrFldAmount.GetValue() / 10;
        aMtrFldAmount.Enable();
        aMtrFldAmount.SetUnit( FUNIT_CUSTOM );
        //SetFieldUnit( aMtrFldAmount, FUNIT_CUSTOM );
        aMtrFldAmount.SetDecimalDigits( 0 );

        aMtrFldAmount.SetSpinSize( 1 );
        aMtrFldAmount.SetMin( 1 );
        aMtrFldAmount.SetFirst( 1 );
        aMtrFldAmount.SetMax( 100 );
        aMtrFldAmount.SetLast( 100 );

        aMtrFldAmount.SetValue( nValue );
    }
    else if( eState == STATE_NOCHECK )
    {
        long nValue = aMtrFldAmount.GetValue() * 10;
        aMtrFldAmount.Enable();
        aMtrFldAmount.SetUnit( eFUnit );
        //SetFieldUnit( aMtrFldAmount, eFUnit );
        aMtrFldAmount.SetDecimalDigits( 2 );

        aMtrFldAmount.SetSpinSize( 10 );
        aMtrFldAmount.SetMin( 1 );
        aMtrFldAmount.SetFirst( 1 );
        aMtrFldAmount.SetMax( 10000 );
        aMtrFldAmount.SetLast( 10000 );

        aMtrFldAmount.SetValue( nValue );
    }

    return( 0L );
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

IMPL_LINK( SvxTextAnimationPage, ClickDirectionHdl_Impl, ImageButton *, pBtn )
{
    aBtnUp.Check( pBtn == &aBtnUp );
    aBtnLeft.Check( pBtn == &aBtnLeft );
    aBtnRight.Check( pBtn == &aBtnRight );
    aBtnDown.Check( pBtn == &aBtnDown );

    return( 0L );
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

void SvxTextAnimationPage::SelectDirection( USHORT nValue )
{
    aBtnUp.Check( nValue == SDRTEXTANI_UP );
    aBtnLeft.Check( nValue == SDRTEXTANI_LEFT );
    aBtnRight.Check( nValue == SDRTEXTANI_RIGHT );
    aBtnDown.Check( nValue == SDRTEXTANI_DOWN );
}

/*************************************************************************
|*
|*
|*
\************************************************************************/

USHORT SvxTextAnimationPage::GetSelectedDirection()
{
    USHORT nValue = 0;

    if( aBtnUp.IsChecked() )
        nValue = SDRTEXTANI_UP;
    else if( aBtnLeft.IsChecked() )
        nValue = SDRTEXTANI_LEFT;
    else if( aBtnRight.IsChecked() )
        nValue = SDRTEXTANI_RIGHT;
    else if( aBtnDown.IsChecked() )
        nValue = SDRTEXTANI_DOWN;

    return( nValue );
}


