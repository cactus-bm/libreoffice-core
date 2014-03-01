/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_SVL_INT64ITEM_HXX
#define INCLUDED_SVL_INT64ITEM_HXX

#include <svl/poolitem.hxx>
#include <svl/svldllapi.h>

class SVL_DLLPUBLIC SfxInt64Item : public SfxPoolItem
{
    sal_Int64 mnValue;

public:
    SfxInt64Item( sal_uInt16 nWhich = 0, sal_Int64 nVal = 0 );
    SfxInt64Item( sal_uInt16 nWhich, SvStream & rStream );
    SfxInt64Item( const SfxInt64Item& rItem );

    virtual ~SfxInt64Item();

    virtual bool operator== ( const SfxPoolItem& rItem ) const;

    virtual int Compare( const SfxPoolItem& r ) const;
    virtual int Compare( const SfxPoolItem& r, const IntlWrapper& rIntlWrapper ) const;

    virtual SfxItemPresentation GetPresentation(
        SfxItemPresentation, SfxMapUnit, SfxMapUnit,
        OUString& rText, const IntlWrapper* pIntlWrapper = NULL ) const;

    virtual bool QueryValue(
        com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 ) const;

    virtual bool PutValue(
        const com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 );

    virtual SfxPoolItem* Create( SvStream& rStream, sal_uInt16 nItemVersion ) const;

    virtual SvStream& Store( SvStream& rStream, sal_uInt16 nItemVersion ) const;

    virtual SfxPoolItem* Clone( SfxItemPool* pOther = NULL ) const;

    sal_Int64 GetValue() const;

    void SetValue( sal_Int64 nVal );
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
