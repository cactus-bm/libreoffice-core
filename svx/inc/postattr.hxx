/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: postattr.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 18:21:04 $
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
#ifndef _SVX_POSTATTR_HXX
#define _SVX_POSTATTR_HXX

// include ---------------------------------------------------------------

#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif

#ifndef INCLUDED_SVXDLLAPI_H
#include "svx/svxdllapi.h"
#endif

// class SvxPostItAuthorItem ---------------------------------------------

#ifdef ITEMID_AUTHOR

/*
[Beschreibung]
Dieses Item beschreibt das Autoren-Kuerzel eines Notizzettels.
*/

class SVX_DLLPUBLIC SvxPostItAuthorItem: public SfxStringItem
{
public:
    TYPEINFO();

    SvxPostItAuthorItem( USHORT nWhich = ITEMID_AUTHOR );

    SvxPostItAuthorItem( const String& rAuthor, USHORT nWhich = ITEMID_AUTHOR );
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText, const IntlWrapper * = 0 ) const;

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;

    inline SvxPostItAuthorItem& operator=( const SvxPostItAuthorItem& rAuthor )
    {
        SetValue( rAuthor.GetValue() );
        return *this;
    }
};
#endif

// class SvxPostItDateItem -----------------------------------------------

#ifdef ITEMID_DATE

/*
[Beschreibung]
Dieses Item beschreibt das Datum eines Notizzettels.
*/

class SVX_DLLPUBLIC SvxPostItDateItem: public SfxStringItem
{
public:
    TYPEINFO();

    SvxPostItDateItem( USHORT nWhich = ITEMID_DATE );

    SvxPostItDateItem( const String& rDate, USHORT nWhich = ITEMID_DATE );
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText, const IntlWrapper * = 0 ) const;
    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;

    inline SvxPostItDateItem& operator=( const SvxPostItDateItem& rDate )
    {
        SetValue( rDate.GetValue() );
        return *this;
    }
};
#endif

// class SvxPostItTextItem -----------------------------------------------

#ifdef ITEMID_TEXT

/*
[Beschreibung]
Dieses Item beschreibt den Text eines Notizzettels.
*/

class SVX_DLLPUBLIC SvxPostItTextItem: public SfxStringItem
{
public:
    TYPEINFO();

    SvxPostItTextItem( USHORT nWhich = ITEMID_TEXT );

    SvxPostItTextItem( const String& rText, USHORT nWhich = ITEMID_TEXT );
    // "pure virtual Methoden" vom SfxPoolItem
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText, const IntlWrapper * = 0 ) const;

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;

    inline SvxPostItTextItem& operator=( const SvxPostItTextItem& rText )
    {
        SetValue( rText.GetValue() );
        return *this;
    }
};
#endif


#endif

