/*************************************************************************
 *
 *  $RCSfile: txtattr.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2004-09-20 13:39:00 $
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

#include <txtattr.hxx>
#include <vcl/font.hxx>




TextAttrib::~TextAttrib()
{
}

void TextAttrib::SetFont( Font& ) const
{
}

TextAttrib* TextAttrib::Clone() const
{
    return NULL;
}

int TextAttrib::operator==( const TextAttrib& rAttr ) const
{
    return mnWhich == rAttr.mnWhich;
}


TextAttribFontColor::TextAttribFontColor( const Color& rColor )
    : TextAttrib( TEXTATTR_FONTCOLOR ), maColor( rColor )
{
}

TextAttribFontColor::TextAttribFontColor( const TextAttribFontColor& rAttr )
    : TextAttrib( rAttr ), maColor( rAttr.maColor )
{
}

TextAttribFontColor::~TextAttribFontColor()
{
}

void TextAttribFontColor::SetFont( Font& rFont ) const
{
    rFont.SetColor( maColor );
}

TextAttrib* TextAttribFontColor::Clone() const
{
    return new TextAttribFontColor( *this );
}

int TextAttribFontColor::operator==( const TextAttrib& rAttr ) const
{
    return ( ( TextAttrib::operator==(rAttr ) ) &&
                ( maColor == ((const TextAttribFontColor&)rAttr).maColor ) );
}

TextAttribFontWeight::TextAttribFontWeight( FontWeight eWeight )
    : TextAttrib( TEXTATTR_FONTWEIGHT ), meWeight( eWeight )
{
}

TextAttribFontWeight::TextAttribFontWeight( const TextAttribFontWeight& rAttr )
    : TextAttrib( rAttr ), meWeight( rAttr.meWeight )
{
}

TextAttribFontWeight::~TextAttribFontWeight()
{
}

void TextAttribFontWeight::SetFont( Font& rFont ) const
{
    rFont.SetWeight( meWeight );
}

TextAttrib* TextAttribFontWeight::Clone() const
{
    return new TextAttribFontWeight( *this );
}

int TextAttribFontWeight::operator==( const TextAttrib& rAttr ) const
{
    return ( ( TextAttrib::operator==(rAttr ) ) &&
                ( meWeight == ((const TextAttribFontWeight&)rAttr).meWeight ) );
}


TextAttribHyperLink::TextAttribHyperLink( const XubString& rURL )
    : TextAttrib( TEXTATTR_HYPERLINK ), maURL( rURL )
{
    maColor = COL_BLUE;
}

TextAttribHyperLink::TextAttribHyperLink( const XubString& rURL, const XubString& rDescription )
    : TextAttrib( TEXTATTR_HYPERLINK ), maURL( rURL ), maDescription( rDescription )
{
    maColor = COL_BLUE;
}

TextAttribHyperLink::TextAttribHyperLink( const TextAttribHyperLink& rAttr )
    : TextAttrib( rAttr ), maURL( rAttr.maURL ), maDescription( rAttr.maDescription )
{
    maColor = rAttr.maColor;
}

TextAttribHyperLink::~TextAttribHyperLink()
{
}

void TextAttribHyperLink::SetFont( Font& rFont ) const
{
    rFont.SetColor( maColor );
    rFont.SetUnderline( UNDERLINE_SINGLE );
}

TextAttrib* TextAttribHyperLink::Clone() const
{
    return new TextAttribHyperLink( *this );
}

int TextAttribHyperLink::operator==( const TextAttrib& rAttr ) const
{
    return ( ( TextAttrib::operator==(rAttr ) ) &&
                ( maURL == ((const TextAttribHyperLink&)rAttr).maURL ) &&
                ( maDescription == ((const TextAttribHyperLink&)rAttr).maDescription ) &&
                ( maColor == ((const TextAttribHyperLink&)rAttr).maColor ) );
}

/*-- 24.06.2004 14:49:44---------------------------------------------------

  -----------------------------------------------------------------------*/
TextAttribProtect::TextAttribProtect() :
    TextAttrib( TEXTATTR_PROTECTED )
{
}
/*-- 24.06.2004 14:49:44---------------------------------------------------

  -----------------------------------------------------------------------*/
TextAttribProtect::TextAttribProtect( const TextAttribProtect& rAttr ) :
    TextAttrib( TEXTATTR_PROTECTED )
{
}
/*-- 24.06.2004 14:49:44---------------------------------------------------

  -----------------------------------------------------------------------*/
TextAttribProtect::~TextAttribProtect()
{
}
/*-- 24.06.2004 14:49:44---------------------------------------------------

  -----------------------------------------------------------------------*/
void TextAttribProtect::SetFont( Font& rFont ) const
{
}
/*-- 24.06.2004 14:49:44---------------------------------------------------

  -----------------------------------------------------------------------*/
TextAttrib*     TextAttribProtect::Clone() const
{
    return new TextAttribProtect();
}
/*-- 24.06.2004 14:49:45---------------------------------------------------

  -----------------------------------------------------------------------*/
int TextAttribProtect::operator==( const TextAttrib& rAttr ) const
{
    return ( TextAttrib::operator==(rAttr ) );
}
