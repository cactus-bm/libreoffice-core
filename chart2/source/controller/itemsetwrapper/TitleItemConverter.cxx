/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: TitleItemConverter.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-25 08:42:13 $
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
#include "precompiled_chart2.hxx"
#include "TitleItemConverter.hxx"
#include "SchWhichPairs.hxx"
#include "macros.hxx"
#include "ItemPropertyMap.hxx"
#include "GraphicPropertyItemConverter.hxx"
#include "CharacterPropertyItemConverter.hxx"
#include "MultipleItemConverter.hxx"

#ifndef _SFXINTITEM_HXX
#include <svtools/intitem.hxx>
#endif
#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif

#include <com/sun/star/chart2/XTitled.hpp>

#include <functional>
#include <algorithm>

using namespace ::com::sun::star;

namespace
{
::comphelper::ItemPropertyMapType & lcl_GetTitlePropertyMap()
{
    static ::comphelper::ItemPropertyMapType aTitlePropertyMap(
        ::comphelper::MakeItemPropertyMap
        IPM_MAP_ENTRY( SCHATTR_TEXT_STACKED, "StackCharacters", 0 )
        );

    return aTitlePropertyMap;
};
} // anonymous namespace

namespace chart
{
namespace wrapper
{

// ========================================

class FormattedStringsConverter : public ::comphelper::MultipleItemConverter
{
public:
    FormattedStringsConverter(
        const uno::Sequence< uno::Reference< chart2::XFormattedString > > & aStrings,
        SfxItemPool & rItemPool,
        ::std::auto_ptr< awt::Size > pRefSize,
        const uno::Reference< beans::XPropertySet > & xParentProp );
    virtual ~FormattedStringsConverter();

protected:
    virtual const USHORT * GetWhichPairs() const;
};

// ----------------------------------------

FormattedStringsConverter::FormattedStringsConverter(
    const uno::Sequence< uno::Reference< chart2::XFormattedString > > & aStrings,
    SfxItemPool & rItemPool,
    ::std::auto_ptr< ::com::sun::star::awt::Size > pRefSize,
    const uno::Reference< beans::XPropertySet > & xParentProp ) :
        MultipleItemConverter( rItemPool )
{
    bool bHasRefSize = (pRefSize.get() && xParentProp.is());
    for( sal_Int32 i = 0; i < aStrings.getLength(); ++i )
    {
        uno::Reference< beans::XPropertySet > xProp( aStrings[ i ], uno::UNO_QUERY );
        if( xProp.is())
        {
            if( bHasRefSize )
                m_aConverters.push_back( new CharacterPropertyItemConverter(
                                             xProp, rItemPool,
                                             ::std::auto_ptr< awt::Size >( new awt::Size( *pRefSize )),
                                             C2U( "ReferencePageSize" ),
                                             xParentProp ));
            else
                m_aConverters.push_back( new CharacterPropertyItemConverter( xProp, rItemPool ));
        }
    }
}

FormattedStringsConverter::~FormattedStringsConverter()
{
}

const USHORT * FormattedStringsConverter::GetWhichPairs() const
{
    return nCharacterPropertyWhichPairs;
}

// ========================================

TitleItemConverter::TitleItemConverter(
    const ::com::sun::star::uno::Reference<
    ::com::sun::star::beans::XPropertySet > & rPropertySet,
    SfxItemPool& rItemPool,
    SdrModel& rDrawModel,
    const uno::Reference< lang::XMultiServiceFactory > & xNamedPropertyContainerFactory,
    ::std::auto_ptr< ::com::sun::star::awt::Size > pRefSize ) :
        ItemConverter( rPropertySet, rItemPool )
{
    m_aConverters.push_back( new GraphicPropertyItemConverter(
                                 rPropertySet, rItemPool, rDrawModel,
                                 xNamedPropertyContainerFactory,
                                 GraphicPropertyItemConverter::LINE_AND_FILL_PROPERTIES ));

    // CharacterProperties are not at the title but at its contained XFormattedString objects
    // take the first formatted string in the sequence
    uno::Reference< chart2::XTitle > xTitle( rPropertySet, uno::UNO_QUERY );
    if( xTitle.is())
    {
        uno::Sequence< uno::Reference< chart2::XFormattedString > > aStringSeq( xTitle->getText());
        if( aStringSeq.getLength() > 0 )
        {
            m_aConverters.push_back(
                new FormattedStringsConverter( aStringSeq, rItemPool, pRefSize, rPropertySet ));
        }
    }
}

TitleItemConverter::~TitleItemConverter()
{
    ::std::for_each( m_aConverters.begin(), m_aConverters.end(),
                     ::comphelper::DeleteItemConverterPtr() );
}

void TitleItemConverter::FillItemSet( SfxItemSet & rOutItemSet ) const
{
    ::std::for_each( m_aConverters.begin(), m_aConverters.end(),
                     ::comphelper::FillItemSetFunc( rOutItemSet ));

    // own items
    ItemConverter::FillItemSet( rOutItemSet );
}

bool TitleItemConverter::ApplyItemSet( const SfxItemSet & rItemSet )
{
    bool bResult = false;

    ::std::for_each( m_aConverters.begin(), m_aConverters.end(),
                     ::comphelper::ApplyItemSetFunc( rItemSet, bResult ));

    // own items
    return ItemConverter::ApplyItemSet( rItemSet ) || bResult;
}

const USHORT * TitleItemConverter::GetWhichPairs() const
{
    // must span all used items!
    return nTitleWhichPairs;
}

bool TitleItemConverter::GetItemProperty( tWhichIdType nWhichId, tPropertyNameWithMemberId & rOutProperty ) const
{
    ::comphelper::ItemPropertyMapType & rMap( lcl_GetTitlePropertyMap());
    ::comphelper::ItemPropertyMapType::const_iterator aIt( rMap.find( nWhichId ));

    if( aIt == rMap.end())
        return false;

    rOutProperty =(*aIt).second;
    return true;
}


bool TitleItemConverter::ApplySpecialItem(
    USHORT nWhichId, const SfxItemSet & rItemSet )
    throw( uno::Exception )
{
    bool bChanged = false;

    switch( nWhichId )
    {
        case SCHATTR_TEXT_DEGREES:
        {
            // convert int to double (divided by 100)
            double fVal = static_cast< double >(
                static_cast< const SfxInt32Item & >(
                    rItemSet.Get( nWhichId )).GetValue()) / 100.0;
            double fOldVal = 0.0;
            bool bPropExisted =
                ( GetPropertySet()->getPropertyValue( C2U( "TextRotation" )) >>= fOldVal );

            if( ! bPropExisted ||
                ( bPropExisted && fOldVal != fVal ))
            {
                GetPropertySet()->setPropertyValue( C2U( "TextRotation" ), uno::makeAny( fVal ));
                bChanged = true;
            }
        }
        break;
    }

    return bChanged;
}

void TitleItemConverter::FillSpecialItem(
    USHORT nWhichId, SfxItemSet & rOutItemSet ) const
    throw( uno::Exception )
{
    switch( nWhichId )
    {
        case SCHATTR_TEXT_DEGREES:
        {
            // convert double to int (times 100)
            double fVal;

            if( GetPropertySet()->getPropertyValue( C2U( "TextRotation" )) >>= fVal )
            {
                rOutItemSet.Put( SfxInt32Item( nWhichId, static_cast< sal_Int32 >(
                                                   ::rtl::math::round( fVal * 100.0 ) ) ));
            }
        }
        break;
   }
}

} //  namespace wrapper
} //  namespace chart
