/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmlcnitm.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 16:16:47 $
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

#ifndef _COM_SUN_STAR_XML_ATTRIBUTEDATA_HPP_
#include <com/sun/star/xml/AttributeData.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif

#ifndef _XMLOFF_XMLCNIMP_HXX
#include <xmloff/xmlcnimp.hxx>
#endif
#ifndef _XMLOFF_XMLCNITM_HXX
#include <xmloff/unoatrcn.hxx>
#endif
#include "xmlcnitm.hxx"

using namespace rtl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::xml;

// ------------------------------------------------------------------------

TYPEINIT1(SvXMLAttrContainerItem, SfxPoolItem);

SvXMLAttrContainerItem::SvXMLAttrContainerItem( USHORT _nWhich ) :
    SfxPoolItem( _nWhich )
{
    pImpl = new SvXMLAttrContainerData;
}

SvXMLAttrContainerItem::SvXMLAttrContainerItem(
                                        const SvXMLAttrContainerItem& rItem ) :
    SfxPoolItem( rItem )
{
    pImpl = new SvXMLAttrContainerData( *rItem.pImpl );
}

SvXMLAttrContainerItem::~SvXMLAttrContainerItem()
{
    delete pImpl;
}

int SvXMLAttrContainerItem::operator==( const SfxPoolItem& rItem ) const
{
    DBG_ASSERT( rItem.ISA(SvXMLAttrContainerItem),
               "SvXMLAttrContainerItem::operator ==(): Bad type");
    return *pImpl == *((const SvXMLAttrContainerItem&)rItem).pImpl;
}

int SvXMLAttrContainerItem::Compare( const SfxPoolItem &/*rWith*/ ) const
{
    DBG_ASSERT( !this, "not yet implemented" );

    return 0;
}

SfxItemPresentation SvXMLAttrContainerItem::GetPresentation(
                    SfxItemPresentation /*ePresentation*/,
                    SfxMapUnit /*eCoreMetric*/,
                    SfxMapUnit /*ePresentationMetric*/,
                    XubString &/*rText*/,
                    const IntlWrapper */*pIntlWrapper*/ ) const
{
    return SFX_ITEM_PRESENTATION_NONE;
}

USHORT SvXMLAttrContainerItem::GetVersion( USHORT /*nFileFormatVersion*/ ) const
{
    // This item should never be stored
    return USHRT_MAX;
}

BOOL  SvXMLAttrContainerItem::QueryValue( com::sun::star::uno::Any& rVal, BYTE /*nMemberId*/ ) const
{
    Reference<XNameContainer> xContainer =
        new SvUnoAttributeContainer( new SvXMLAttrContainerData( *pImpl ) );

    rVal.setValue( &xContainer, ::getCppuType((Reference<XNameContainer>*)0) );
    return TRUE;
}
BOOL SvXMLAttrContainerItem::PutValue( const com::sun::star::uno::Any& rVal, BYTE /*nMemberId*/ )
{
    Reference<XInterface> xRef;
    SvUnoAttributeContainer* pContainer = NULL;

    if( rVal.getValue() != NULL && rVal.getValueType().getTypeClass() == TypeClass_INTERFACE )
    {
        xRef = *(Reference<XInterface>*)rVal.getValue();
        Reference<XUnoTunnel> xTunnel(xRef, UNO_QUERY);
        if( xTunnel.is() )
            pContainer = (SvUnoAttributeContainer*)(ULONG)xTunnel->getSomething(SvUnoAttributeContainer::getUnoTunnelId());
    }

    if( pContainer )
    {
        delete pImpl;
        pImpl = new SvXMLAttrContainerData( * pContainer->GetContainerImpl() );
    }
    else
    {
        SvXMLAttrContainerData* pNewImpl = new SvXMLAttrContainerData;

        try
        {
            Reference<XNameContainer> xContainer( xRef, UNO_QUERY );
            if( !xContainer.is() )
                return FALSE;

            const Sequence< OUString > aNameSequence( xContainer->getElementNames() );
            const OUString* pNames = aNameSequence.getConstArray();
            const INT32 nCount = aNameSequence.getLength();
            Any aAny;
            AttributeData* pData;
            INT32 nAttr;

            for( nAttr = 0; nAttr < nCount; nAttr++ )
            {
                const OUString aName( *pNames++ );

                aAny = xContainer->getByName( aName );
                if( aAny.getValue() == NULL || aAny.getValueType() != ::getCppuType((AttributeData*)0) )
                    return FALSE;

                pData = (AttributeData*)aAny.getValue();
                USHORT pos = aName.indexOf( sal_Unicode(':') );
                if( pos != USHRT_MAX )
                {
                    const OUString aPrefix( aName.copy( 0, pos ));
                    const OUString aLName( aName.copy( pos+1 ));

                    if( pData->Namespace.getLength() == 0 )
                    {
                        if( !pNewImpl->AddAttr( aPrefix, aLName, pData->Value ) )
                            break;
                    }
                    else
                    {
                        if( !pNewImpl->AddAttr( aPrefix, pData->Namespace, aLName, pData->Value ) )
                            break;
                    }
                }
                else
                {
                    if( !pNewImpl->AddAttr( aName, pData->Value ) )
                        break;
                }
            }

            if( nAttr == nCount )
            {
                delete pImpl;
                pImpl = pNewImpl;
            }
            else
            {
                delete pNewImpl;
                return FALSE;
            }
        }
        catch(...)
        {
            delete pNewImpl;
            return FALSE;
        }
    }
    return TRUE;
}


BOOL SvXMLAttrContainerItem::AddAttr( const OUString& rLName,
                                        const OUString& rValue )
{
    return pImpl->AddAttr( rLName, rValue );
}

BOOL SvXMLAttrContainerItem::AddAttr( const OUString& rPrefix,
          const OUString& rNamespace, const OUString& rLName,
          const OUString& rValue )
{
    return pImpl->AddAttr( rPrefix, rNamespace, rLName, rValue );
}

USHORT SvXMLAttrContainerItem::GetAttrCount() const
{
    return (USHORT)pImpl->GetAttrCount();
}

OUString SvXMLAttrContainerItem::GetAttrNamespace( USHORT i ) const
{
    return pImpl->GetAttrNamespace( i );
}

OUString SvXMLAttrContainerItem::GetAttrPrefix( USHORT i ) const
{
    return pImpl->GetAttrPrefix( i );
}

const OUString& SvXMLAttrContainerItem::GetAttrLName( USHORT i ) const
{
    return pImpl->GetAttrLName( i );
}

const OUString& SvXMLAttrContainerItem::GetAttrValue( USHORT i ) const
{
    return pImpl->GetAttrValue( i );
}


USHORT SvXMLAttrContainerItem::GetFirstNamespaceIndex() const
{
    return pImpl->GetFirstNamespaceIndex();
}

USHORT SvXMLAttrContainerItem::GetNextNamespaceIndex( USHORT nIdx ) const
{
    return pImpl->GetNextNamespaceIndex( nIdx );
}

const OUString& SvXMLAttrContainerItem::GetNamespace( USHORT i ) const
{
    return pImpl->GetNamespace( i );
}

const OUString& SvXMLAttrContainerItem::GetPrefix( USHORT i ) const
{
    return pImpl->GetPrefix( i );
}


