/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: lcolitem.hxx,v $
 * $Revision: 1.5 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef _SVX_LCOLITEM_HXX
#define _SVX_LCOLITEM_HXX

// include ---------------------------------------------------------------

#include <editeng/colritem.hxx>
#include <editeng/editengdllapi.h>

/*
 * dummy-Item fuer ToolBox-Controller:
 *
 */

class EDITENG_DLLPUBLIC SvxLineColorItem : public SvxColorItem
{
public:
    TYPEINFO();

    SvxLineColorItem( const USHORT nId  );
    SvxLineColorItem( const Color& aColor, const USHORT nId  );
    SvxLineColorItem( SvStream& rStrm, const USHORT nId  );
    SvxLineColorItem( const SvxLineColorItem& rCopy );
    ~SvxLineColorItem();

    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper * pIntlWrapper = 0 )
        const;

    inline SvxLineColorItem& operator=(const SvxLineColorItem& rColor)
        { return (SvxLineColorItem&)SvxColorItem::
            operator=((const SvxColorItem&)rColor); }

};

#endif

