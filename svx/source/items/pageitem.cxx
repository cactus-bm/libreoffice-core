/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: pageitem.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: kz $ $Date: 2007-05-10 14:52:41 $
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

#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif


#include "pageitem.hxx"
#include "itemtype.hxx"
#include <unomid.hxx>

#ifndef _COM_SUN_STAR_STYLE_PAGESTYLELAYOUT_HPP_
#include <com/sun/star/style/PageStyleLayout.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_BREAKTYPE_HPP_
#include <com/sun/star/style/BreakType.hpp>
#endif

#ifndef _SFXITEMSET_HXX //autogen
#include <svtools/itemset.hxx>
#endif

using namespace ::rtl;
using namespace ::com::sun::star;

// STATIC DATA -----------------------------------------------------------

TYPEINIT1_FACTORY( SvxPageItem, SfxPoolItem , new  SvxPageItem(0));

/*--------------------------------------------------------------------
    Beschreibung: Konstruktor
 --------------------------------------------------------------------*/

SvxPageItem::SvxPageItem( const USHORT nId ) : SfxPoolItem( nId ),

    eNumType    ( SVX_ARABIC ),
    bLandscape  ( sal_False ),
    eUse        ( SVX_PAGE_ALL )
{
}

/*--------------------------------------------------------------------
    Beschreibung: Copy-Konstruktor
 --------------------------------------------------------------------*/

SvxPageItem::SvxPageItem( const SvxPageItem& rItem )
    : SfxPoolItem( rItem )
{
    eNumType    = rItem.eNumType;
    bLandscape  = rItem.bLandscape;
    eUse        = rItem.eUse;
}

/*--------------------------------------------------------------------
    Beschreibung: Clonen
 --------------------------------------------------------------------*/

SfxPoolItem* SvxPageItem::Clone( SfxItemPool * ) const
{
    return new SvxPageItem( *this );
}

/*--------------------------------------------------------------------
    Beschreibung: Abfrage auf Gleichheit
 --------------------------------------------------------------------*/

int SvxPageItem::operator==( const SfxPoolItem& rAttr ) const
{
    DBG_ASSERT( SfxPoolItem::operator==(rAttr), "unequal types" );
    const SvxPageItem& rItem = (SvxPageItem&)rAttr;
    return ( eNumType   == rItem.eNumType   &&
             bLandscape == rItem.bLandscape &&
             eUse       == rItem.eUse );
}

inline XubString GetUsageText( const USHORT eU )
{
    if ( eU & SVX_PAGE_LEFT )
        return SVX_RESSTR(RID_SVXITEMS_PAGE_USAGE_LEFT);
    if ( eU & SVX_PAGE_RIGHT )
        return SVX_RESSTR(RID_SVXITEMS_PAGE_USAGE_RIGHT);
    if ( eU & SVX_PAGE_ALL )
        return SVX_RESSTR(RID_SVXITEMS_PAGE_USAGE_ALL);
    if ( eU & SVX_PAGE_MIRROR )
        return SVX_RESSTR(RID_SVXITEMS_PAGE_USAGE_MIRROR);
    return String();
}

//------------------------------------------------------------------------

SfxItemPresentation SvxPageItem::GetPresentation
(
    SfxItemPresentation ePres,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();

    switch ( ePres )
    {
        case SFX_ITEM_PRESENTATION_NONE:
            return SFX_ITEM_PRESENTATION_NONE;
        case SFX_ITEM_PRESENTATION_NAMELESS:
        {
            if ( aDescName.Len() )
            {
                rText = aDescName;
                rText += cpDelim;
            }
            DBG_ASSERT( eNumType <= SVX_NUMBER_NONE, "enum overflow" );
            rText += SVX_RESSTR(RID_SVXITEMS_PAGE_NUM_BEGIN + eNumType);
            rText += cpDelim;
            if ( bLandscape )
                rText += SVX_RESSTR(RID_SVXITEMS_PAGE_LAND_TRUE);
            else
                rText += SVX_RESSTR(RID_SVXITEMS_PAGE_LAND_FALSE);
            rText += GetUsageText( eUse );
            return SFX_ITEM_PRESENTATION_NAMELESS;
        }
        case SFX_ITEM_PRESENTATION_COMPLETE:
        {
            rText += SVX_RESSTR(RID_SVXITEMS_PAGE_COMPLETE);
            if ( aDescName.Len() )
            {
                rText += aDescName;
                rText += cpDelim;
            }
            DBG_ASSERT( eNumType <= SVX_NUMBER_NONE, "enum overflow" );
            rText += SVX_RESSTR(RID_SVXITEMS_PAGE_NUM_BEGIN + eNumType);
            rText += cpDelim;
            if ( bLandscape )
                rText += SVX_RESSTR(RID_SVXITEMS_PAGE_LAND_TRUE);
            else
                rText += SVX_RESSTR(RID_SVXITEMS_PAGE_LAND_FALSE);
            rText += GetUsageText( eUse );
            return SFX_ITEM_PRESENTATION_COMPLETE;
        }
        default: ;//prevent warning
    }
    return SFX_ITEM_PRESENTATION_NONE;
}

//------------------------------------------------------------------------
sal_Bool SvxPageItem::QueryValue( uno::Any& rVal, BYTE nMemberId ) const
{
//    sal_Bool bConvert = 0!=(nMemberId&CONVERT_TWIPS);
    nMemberId &= ~CONVERT_TWIPS;
    switch( nMemberId )
    {
        case MID_PAGE_NUMTYPE:
        {
            //! die Konstanten sind nicht mehr in den IDLs ?!?
            rVal <<= (sal_Int16)( eNumType );
        }
        break;
        case MID_PAGE_ORIENTATION:
            //Landscape= sal_True
            rVal = Bool2Any(bLandscape);
        break;
        case MID_PAGE_LAYOUT     :
        {
            style::PageStyleLayout eRet;
            switch(eUse & 0x0f)
            {
                case SVX_PAGE_LEFT  : eRet = style::PageStyleLayout_LEFT;      break;
                case SVX_PAGE_RIGHT : eRet = style::PageStyleLayout_RIGHT;     break;
                case SVX_PAGE_ALL   : eRet = style::PageStyleLayout_ALL;       break;
                case SVX_PAGE_MIRROR: eRet = style::PageStyleLayout_MIRRORED; break;
                default:
                    DBG_ERROR("was fuer ein Layout ist das?");
                    return sal_False;
            }
            rVal <<= eRet;
        }
        break;
    }

    return sal_True;
}
//------------------------------------------------------------------------
sal_Bool SvxPageItem::PutValue( const uno::Any& rVal, BYTE nMemberId )
{
    switch( nMemberId )
    {
        case MID_PAGE_NUMTYPE:
        {
            sal_Int32 nValue = 0;
            if(!(rVal >>= nValue))
                return sal_False;

            eNumType = (SvxNumType)nValue;
        }
        break;
        case MID_PAGE_ORIENTATION:
            bLandscape = Any2Bool(rVal);
        break;
        case MID_PAGE_LAYOUT     :
        {
            style::PageStyleLayout eLayout;
            if(!(rVal >>= eLayout))
            {
                sal_Int32 nValue = 0;
                if(!(rVal >>= nValue))
                    return sal_False;
                eLayout = (style::PageStyleLayout)nValue;
            }
            eUse &= 0xfff0;
            switch( eLayout )
            {
                case style::PageStyleLayout_LEFT     : eUse |= SVX_PAGE_LEFT ; break;
                case style::PageStyleLayout_RIGHT   : eUse |= SVX_PAGE_RIGHT; break;
                case style::PageStyleLayout_ALL     : eUse |= SVX_PAGE_ALL  ; break;
                case style::PageStyleLayout_MIRRORED: eUse |= SVX_PAGE_MIRROR;break;
                default: ;//prevent warning
            }
        }
        break;
    }
    return sal_True;
}

//------------------------------------------------------------------------

SfxPoolItem* SvxPageItem::Create( SvStream& rStream, USHORT ) const
{
    XubString sStr;
    BYTE eType;
    sal_Bool bLand;
    USHORT nUse;

    // UNICODE: rStream >> sStr;
    rStream.ReadByteString( sStr );

    rStream >> eType;
    rStream >> bLand;
    rStream >> nUse;

    SvxPageItem* pPage = new SvxPageItem( Which() );
    pPage->SetDescName( sStr );
    pPage->SetNumType( (SvxNumType)eType );
    pPage->SetLandscape( bLand );
    pPage->SetPageUsage( nUse );
    return pPage;
}

//------------------------------------------------------------------------

SvStream& SvxPageItem::Store( SvStream &rStrm, USHORT /*nItemVersion*/ ) const
{
    // UNICODE: rStrm << aDescName;
    rStrm.WriteByteString(aDescName);

    rStrm << (BYTE)eNumType << bLandscape << eUse;
    return rStrm;
}

/*--------------------------------------------------------------------
    Beschreibung:   HeaderFooterSet
 --------------------------------------------------------------------*/

SvxSetItem::SvxSetItem( const USHORT nId, const SfxItemSet& rSet ) :

    SfxSetItem( nId, rSet )
{
}

SvxSetItem::SvxSetItem( const SvxSetItem& rItem ) :

    SfxSetItem( rItem )
{
}

SvxSetItem::SvxSetItem( const USHORT nId, SfxItemSet* _pSet ) :

    SfxSetItem( nId, _pSet )
{
}

SfxPoolItem* SvxSetItem::Clone( SfxItemPool * ) const
{
    return new SvxSetItem(*this);
}

//------------------------------------------------------------------------

SfxItemPresentation SvxSetItem::GetPresentation
(
    SfxItemPresentation /*ePres*/,
    SfxMapUnit          /*eCoreUnit*/,
    SfxMapUnit          /*ePresUnit*/,
    XubString&          rText, const IntlWrapper *
)   const
{
    rText.Erase();
    return SFX_ITEM_PRESENTATION_NONE;
}

SfxPoolItem* SvxSetItem::Create(SvStream &rStrm, USHORT /*nVersion*/) const
{
    SfxItemSet* _pSet = new SfxItemSet( *GetItemSet().GetPool(),
                                       GetItemSet().GetRanges() );

    _pSet->Load( rStrm );

    return new SvxSetItem( Which(), *_pSet );
}

SvStream& SvxSetItem::Store(SvStream &rStrm, USHORT nItemVersion) const
{
    GetItemSet().Store( rStrm, nItemVersion );

    return rStrm;
}


