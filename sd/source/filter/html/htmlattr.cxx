/*************************************************************************
 *
 *  $RCSfile: htmlattr.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-08-23 08:13:10 $
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

#ifdef SD_DLLIMPLEMENTATION
#undef SD_DLLIMPLEMENTATION
#endif

#include "htmlattr.hxx"

#ifndef _SD_HTMLEX_HXX
#include "htmlex.hxx"
#endif
#ifndef _LINK_HXX
#include <tools/link.hxx>
#endif
#ifndef _SV_DECOVIEW_HXX //autogen
#include <vcl/decoview.hxx>
#endif


// =====================================================================
// =====================================================================
SdHtmlAttrPreview::SdHtmlAttrPreview( Window* pParent, const ResId& rResId )
:Control( pParent, rResId )
{
//    SetMapMode( MAP_100TH_MM );
}

// =====================================================================
// =====================================================================
SdHtmlAttrPreview::~SdHtmlAttrPreview()
{
}

// =====================================================================
// =====================================================================
void SdHtmlAttrPreview::Paint( const Rectangle& rRect )
{
    DecorationView aDecoView( this );

    Rectangle aTextRect;
    aTextRect.SetSize(GetOutputSize());

    SetLineColor(m_aBackColor);
    SetFillColor(m_aBackColor);
    DrawRect(rRect);
    SetFillColor();

    int nHeight = (aTextRect.nBottom - aTextRect.nTop) >> 2;
    aTextRect.nBottom =  nHeight + aTextRect.nTop;

    SetTextColor(m_aTextColor);
    DrawText( aTextRect, String(SdResId(STR_HTMLATTR_TEXT)),
                         TEXT_DRAW_CENTER|TEXT_DRAW_VCENTER );

    aTextRect.Move(0,nHeight);
    SetTextColor(m_aLinkColor);
    DrawText( aTextRect, String(SdResId(STR_HTMLATTR_LINK)),
                         TEXT_DRAW_CENTER|TEXT_DRAW_VCENTER );

    aTextRect.Move(0,nHeight);
    SetTextColor(m_aALinkColor);
    DrawText( aTextRect, String(SdResId(STR_HTMLATTR_ALINK)),
                         TEXT_DRAW_CENTER|TEXT_DRAW_VCENTER );

    aTextRect.Move(0,nHeight);
    SetTextColor(m_aVLinkColor);
    DrawText( aTextRect, String(SdResId(STR_HTMLATTR_VLINK)),
                         TEXT_DRAW_CENTER|TEXT_DRAW_VCENTER );
}

// =====================================================================
// =====================================================================
void SdHtmlAttrPreview::SetColors( Color& aBack, Color& aText, Color& aLink,
                                   Color& aVLink, Color& aALink )
{
    m_aBackColor = aBack;
    m_aTextColor = aText;
    m_aLinkColor = aLink;
    m_aVLinkColor = aVLink;
    m_aALinkColor = aALink;
}

// =====================================================================
// =====================================================================
void SdHtmlAttrPreview::GetColors( Color& aBack, Color& aText, Color& aLink,
                                   Color& aVLink, Color& aALink ) const
{
    aBack = m_aBackColor;
    aText = m_aTextColor;
    aLink = m_aLinkColor;
    aVLink = m_aVLinkColor;
    aALink = m_aALinkColor;
}
