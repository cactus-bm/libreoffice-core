/*************************************************************************
 *
 *  $RCSfile: xmldlg_export.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: dbo $ $Date: 2001-02-16 14:14:48 $
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
#include "exp_share.hxx"

#include <rtl/ustrbuf.hxx>

#include <com/sun/star/awt/CharSet.hpp>
#include <com/sun/star/awt/FontFamily.hpp>
#include <com/sun/star/awt/FontPitch.hpp>
#include <com/sun/star/awt/FontSlant.hpp>
#include <com/sun/star/awt/FontStrikeout.hpp>
#include <com/sun/star/awt/FontType.hpp>
#include <com/sun/star/awt/FontUnderline.hpp>
#include <com/sun/star/awt/FontWeight.hpp>
#include <com/sun/star/awt/FontWidth.hpp>

#include <com/sun/star/lang/XServiceInfo.hpp>


namespace xmlscript
{

//__________________________________________________________________________________________________
Reference< xml::sax::XAttributeList > Style::createElement()
{
    ElementDescriptor * pStyle = new ElementDescriptor(
        OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":style") ) );

    // style-id
    pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":style-id") ), _id );

    // background-color
    if ((_set & 0x1) != 0)
    {
        OUStringBuffer buf( 16 );
        buf.append( (sal_Unicode)'0' );
        buf.append( (sal_Unicode)'x' );
        buf.append( OUString::valueOf( (sal_Int64)(sal_uInt64)_backgroundColor, 16 ) );
        pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":background-color") ),
                         buf.makeStringAndClear() );
    }

    // text-color
    if ((_set & 0x2) != 0)
    {
        OUStringBuffer buf( 16 );
        buf.append( (sal_Unicode)'0' );
        buf.append( (sal_Unicode)'x' );
        buf.append( OUString::valueOf( (sal_Int64)(sal_uInt64)_textColor, 16 ) );
        pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":text-color") ),
                         buf.makeStringAndClear() );
    }

    // border
    if ((_set & 0x4) != 0)
    {
        switch (_border)
        {
        case 0:
            pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":border") ),
                             OUString( RTL_CONSTASCII_USTRINGPARAM("none") ) );
            break;
        case 1:
            pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":border") ),
                             OUString( RTL_CONSTASCII_USTRINGPARAM("3d") ) );
            break;
        case 2:
            pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":border") ),
                             OUString( RTL_CONSTASCII_USTRINGPARAM("simple") ) );
            break;
        default:
            OSL_ENSURE( 0, "### unexpected border value!" );
        }
    }

    // font-
    if ((_set & 0x8) != 0)
    {
        awt::FontDescriptor def_descr;

        // dialog:font-name CDATA #IMPLIED
        if (def_descr.Name != _descr.Name)
        {
            pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-name") ),
                             _descr.Name );
        }
        // dialog:font-height %numeric; #IMPLIED
        if (def_descr.Height != _descr.Height)
        {
            pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-height") ),
                             OUString::valueOf( (sal_Int32)_descr.Height ) );
        }
        // dialog:font-width %numeric; #IMPLIED
        if (def_descr.Width != _descr.Width)
        {
            pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-width") ),
                             OUString::valueOf( (sal_Int32)_descr.Width ) );
        }
        // dialog:font-stylename CDATA #IMPLIED
        if (def_descr.StyleName != _descr.StyleName)
        {
            pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-stylename") ),
                             _descr.StyleName );
        }
        // dialog:font-family "(decorative|modern|roman|script|swiss|system)" #IMPLIED
        if (def_descr.Family != _descr.Family)
        {
            switch (_descr.Family)
            {
            case awt::FontFamily::DECORATIVE:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-family") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("decorative") ) );
                break;
            case awt::FontFamily::MODERN:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-family") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("modern") ) );
                break;
            case awt::FontFamily::ROMAN:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-family") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("roman") ) );
                break;
            case awt::FontFamily::SCRIPT:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-family") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("script") ) );
                break;
            case awt::FontFamily::SWISS:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-family") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("swiss") ) );
                break;
            case awt::FontFamily::SYSTEM:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-family") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("system") ) );
                break;
            }
        }
        // dialog:font-charset "(ansi|mac|ibmpc_437|ibmpc_850|ibmpc_860|ibmpc_861|ibmpc_863|ibmpc_865|system|symbol)" #IMPLIED
        if (def_descr.CharSet != _descr.CharSet)
        {
            switch (_descr.CharSet)
            {
            case awt::CharSet::ANSI:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-charset") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("ansi") ) );
                break;
            case awt::CharSet::MAC:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-charset") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("mac") ) );
                break;
            case awt::CharSet::IBMPC_437:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-charset") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("ibmpc_437") ) );
                break;
            case awt::CharSet::IBMPC_850:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-charset") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("ibmpc_850") ) );
                break;
            case awt::CharSet::IBMPC_860:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-charset") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("ibmpc_860") ) );
                break;
            case awt::CharSet::IBMPC_861:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-charset") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("ibmpc_861") ) );
                break;
            case awt::CharSet::IBMPC_863:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-charset") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("ibmpc_863") ) );
                break;
            case awt::CharSet::IBMPC_865:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-charset") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("ibmpc_865") ) );
                break;
            case awt::CharSet::SYSTEM:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-charset") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("system") ) );
                break;
            case awt::CharSet::SYMBOL:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-charset") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("symbol") ) );
                break;
            }
        }
        // dialog:font-pitch "(fixed|variable)" #IMPLIED
        if (def_descr.Pitch != _descr.Pitch)
        {
            switch (_descr.Pitch)
            {
            case awt::FontPitch::FIXED:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-pitch") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("fixed") ) );
                break;
            case awt::FontPitch::VARIABLE:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-pitch") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("variable") ) );
                break;
            }
        }
        // dialog:font-charwidth CDATA #IMPLIED
        if (def_descr.CharacterWidth != _descr.CharacterWidth)
        {
            pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-charwidth") ),
                             OUString::valueOf( (float)_descr.CharacterWidth ) );
        }
        // dialog:font-weight CDATA #IMPLIED
        if (def_descr.Weight != _descr.Weight)
        {
            pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-weight") ),
                             OUString::valueOf( (float)_descr.Weight ) );
        }
        // dialog:font-slant "(oblique|italic|reverse_oblique|reverse_italic)" #IMPLIED
        if (def_descr.Slant != _descr.Slant)
        {
            switch (_descr.Slant)
            {
            case awt::FontSlant_OBLIQUE:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-slant") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("oblique") ) );
                break;
            case awt::FontSlant_ITALIC:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-slant") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("italic") ) );
                break;
            case awt::FontSlant_REVERSE_OBLIQUE:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-slant") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("reverse_oblique") ) );
                break;
            case awt::FontSlant_REVERSE_ITALIC:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-slant") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("reverse_italic") ) );
                break;
            }
        }
        // dialog:font-underline "(single|double|dotted|dash|longdash|dashdot|dashdotdot|smallwave|wave|doublewave|bold|bolddotted|bolddash|boldlongdash|bolddashdot|bolddashdotdot|boldwave)" #IMPLIED
        if (def_descr.Underline != _descr.Underline)
        {
            switch (_descr.Underline)
            {
            case awt::FontUnderline::SINGLE:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("single") ) );
                break;
            case awt::FontUnderline::DOUBLE:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("double") ) );
                break;
            case awt::FontUnderline::DOTTED:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("dotted") ) );
                break;
            case awt::FontUnderline::DASH:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("dash") ) );
                break;
            case awt::FontUnderline::LONGDASH:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("longdash") ) );
                break;
            case awt::FontUnderline::DASHDOT:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("dashdot") ) );
                break;
            case awt::FontUnderline::DASHDOTDOT:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("dashdotdot") ) );
                break;
            case awt::FontUnderline::SMALLWAVE:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("smallwave") ) );
                break;
            case awt::FontUnderline::WAVE:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("wave") ) );
                break;
            case awt::FontUnderline::DOUBLEWAVE:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("doublewave") ) );
                break;
            case awt::FontUnderline::BOLD:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("bold") ) );
                break;
            case awt::FontUnderline::BOLDDOTTED:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("bolddotted") ) );
                break;
            case awt::FontUnderline::BOLDDASH:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("bolddash") ) );
                break;
            case awt::FontUnderline::BOLDLONGDASH:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("boldlongdash") ) );
                break;
            case awt::FontUnderline::BOLDDASHDOT:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("bolddashdot") ) );
                break;
            case awt::FontUnderline::BOLDDASHDOTDOT:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("bolddashdotdot") ) );
                break;
            case awt::FontUnderline::BOLDWAVE:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-underline") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("boldwave") ) );
                break;
            }
        }
        // dialog:font-strikeout "(single|double|bold|slash|x)" #IMPLIED
        if (def_descr.Strikeout != _descr.Strikeout)
        {
            switch (_descr.Strikeout)
            {
            case awt::FontStrikeout::SINGLE:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-strikeout") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("single") ) );
                break;
            case awt::FontStrikeout::DOUBLE:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-strikeout") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("double") ) );
                break;
            case awt::FontStrikeout::BOLD:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-strikeout") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("bold") ) );
                break;
            case awt::FontStrikeout::SLASH:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-strikeout") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("slash") ) );
                break;
            case awt::FontStrikeout::X:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-strikeout") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("x") ) );
                break;
            }
        }
        // dialog:font-orientation CDATA #IMPLIED
        if (def_descr.Orientation != _descr.Orientation)
        {
            pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-orientation") ),
                             OUString::valueOf( (float)_descr.Orientation ) );
        }
        // dialog:font-kerning %boolean; #IMPLIED
        if ((def_descr.Kerning != sal_False) != (_descr.Kerning != sal_False))
        {
            pStyle->addBoolAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-kerning") ),
                                    _descr.Kerning );
        }
        // dialog:font-wordlinemode %boolean; #IMPLIED
        if ((def_descr.WordLineMode != sal_False) != (_descr.WordLineMode != sal_False))
        {
            pStyle->addBoolAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-wordlinemode") ),
                                 _descr.WordLineMode );
        }
        // dialog:font-type "(raster|device|scalable)" #IMPLIED
        if (def_descr.Type != _descr.Type)
        {
            switch (_descr.Type)
            {
            case awt::FontType::RASTER:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-type") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("raster") ) );
                break;
            case awt::FontType::DEVICE:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-type") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("device") ) );
                break;
            case awt::FontType::SCALABLE:
                pStyle->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":font-type") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM("scalable") ) );
                break;
            }
        }
    }

    return pStyle;
}

//##################################################################################################

// XAttributeList
//__________________________________________________________________________________________________
sal_Int16 ElementDescriptor::getLength()
    throw (RuntimeException)
{
    return _attrNames.size();
}
//__________________________________________________________________________________________________
OUString ElementDescriptor::getNameByIndex( sal_Int16 nPos )
    throw (RuntimeException)
{
    OSL_ASSERT( (size_t)nPos < _attrNames.size() );
    return _attrNames[ nPos ];
}
//__________________________________________________________________________________________________
OUString ElementDescriptor::getTypeByIndex( sal_Int16 nPos )
    throw (RuntimeException)
{
    OSL_ASSERT( (size_t)nPos < _attrNames.size() );
    // xxx todo
    return OUString();
}
//__________________________________________________________________________________________________
OUString ElementDescriptor::getTypeByName( OUString const & rName )
    throw (RuntimeException)
{
    // xxx todo
    return OUString();
}
//__________________________________________________________________________________________________
OUString ElementDescriptor::getValueByIndex( sal_Int16 nPos )
    throw (RuntimeException)
{
    OSL_ASSERT( (size_t)nPos < _attrNames.size() );
    return _attrValues[ nPos ];
}
//__________________________________________________________________________________________________
OUString ElementDescriptor::getValueByName( OUString const & rName )
    throw (RuntimeException)
{
    for ( size_t nPos = 0; nPos < _attrNames.size(); ++nPos )
    {
        if (_attrNames[ nPos ] == rName)
        {
            return _attrValues[ nPos ];
        }
    }
    return OUString();
}

//
//__________________________________________________________________________________________________
Any ElementDescriptor::readProp( OUString const & rPropName )
{
    if (beans::PropertyState_DEFAULT_VALUE != _xPropState->getPropertyState( rPropName ))
    {
        return _xProps->getPropertyValue( rPropName );
    }
    return Any();
}
//__________________________________________________________________________________________________
void ElementDescriptor::readStringAttr( OUString const & rPropName, OUString const & rAttrName )
{
    if (beans::PropertyState_DEFAULT_VALUE != _xPropState->getPropertyState( rPropName ))
    {
        Any a( _xProps->getPropertyValue( rPropName ) );
        if (a.getValueTypeClass() == TypeClass_STRING)
        {
            addAttr( rAttrName, * reinterpret_cast< const OUString * >( a.getValue() ) );
        }
    }
}
//__________________________________________________________________________________________________
void ElementDescriptor::readLongAttr( OUString const & rPropName, OUString const & rAttrName )
{
    if (beans::PropertyState_DEFAULT_VALUE != _xPropState->getPropertyState( rPropName ))
    {
        Any a( _xProps->getPropertyValue( rPropName ) );
        if (a.getValueTypeClass() == TypeClass_LONG)
        {
            addAttr( rAttrName, OUString::valueOf( (sal_Int64)(sal_uInt64)*(sal_uInt32 *)a.getValue() ) );
        }
    }
}
//__________________________________________________________________________________________________
void ElementDescriptor::readHexLongAttr( OUString const & rPropName, OUString const & rAttrName )
{
    if (beans::PropertyState_DEFAULT_VALUE != _xPropState->getPropertyState( rPropName ))
    {
        Any a( _xProps->getPropertyValue( rPropName ) );
        if (a.getValueTypeClass() == TypeClass_LONG)
        {
            OUStringBuffer buf( 16 );
            buf.append( (sal_Unicode)'0' );
            buf.append( (sal_Unicode)'x' );
            buf.append( OUString::valueOf( (sal_Int64)(sal_uInt64)*(sal_uInt32 *)a.getValue(), 16 ) );
            addAttr( rAttrName, buf.makeStringAndClear() );
        }
    }
}
//__________________________________________________________________________________________________
void ElementDescriptor::readShortAttr( OUString const & rPropName, OUString const & rAttrName )
{
    if (beans::PropertyState_DEFAULT_VALUE != _xPropState->getPropertyState( rPropName ))
    {
        Any a( _xProps->getPropertyValue( rPropName ) );
        if (a.getValueTypeClass() == TypeClass_SHORT)
        {
            addAttr( rAttrName, OUString::valueOf( (sal_Int32)*(sal_Int16 *)a.getValue() ) );
        }
    }
}
//__________________________________________________________________________________________________
void ElementDescriptor::readBoolAttr( OUString const & rPropName, OUString const & rAttrName )
{
    if (beans::PropertyState_DEFAULT_VALUE != _xPropState->getPropertyState( rPropName ))
    {
        Any a( _xProps->getPropertyValue( rPropName ) );
        if (a.getValueTypeClass() == TypeClass_BOOLEAN)
        {
            addBoolAttr( rAttrName, * reinterpret_cast< const sal_Bool * >( a.getValue() ) );
        }
    }
}
//__________________________________________________________________________________________________
void ElementDescriptor::readDefaults()
{
    sal_Bool bEnabled;
    if (_xProps->getPropertyValue( OUString( RTL_CONSTASCII_USTRINGPARAM("Enabled") ) ) >>= bEnabled)
    {
        if (! bEnabled)
        {
            addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":disabled") ),
                     OUString( RTL_CONSTASCII_USTRINGPARAM("true") ) );
        }
    }
    else
    {
        OSL_ENSURE( 0, "unexpected property type for \"Enabled\": not bool!" );
    }

    readBoolAttr( OUString( RTL_CONSTASCII_USTRINGPARAM("Printable") ),
                  OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":printable") ) );
    readBoolAttr( OUString( RTL_CONSTASCII_USTRINGPARAM("Tabstop") ),
                  OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":tabstop") ) );

    readLongAttr( OUString( RTL_CONSTASCII_USTRINGPARAM("PositionX") ),
                  OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":left") ) );
    readLongAttr( OUString( RTL_CONSTASCII_USTRINGPARAM("PositionY") ),
                  OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":top") ) );
    readLongAttr( OUString( RTL_CONSTASCII_USTRINGPARAM("Width") ),
                  OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":width") ) );
    readLongAttr( OUString( RTL_CONSTASCII_USTRINGPARAM("Height") ),
                  OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":height") ) );
}

//##################################################################################################

inline bool equals( awt::FontDescriptor const & f1, awt::FontDescriptor const & f2 )
{
    return (f1.Name == f2.Name &&
            f1.Height == f2.Height &&
            f1.Width == f2.Width &&
            f1.StyleName == f2.StyleName &&
            f1.Family == f2.Family &&
            f1.CharSet == f2.CharSet &&
            f1.Pitch == f2.CharSet &&
            f1.CharacterWidth == f2.CharacterWidth &&
            f1.Weight == f2.Weight &&
            f1.Slant == f2.Slant &&
            f1.Underline == f2.Underline &&
            f1.Strikeout == f2.Strikeout &&
            f1.Orientation == f2.Orientation &&
            (f1.Kerning != sal_False) == (f2.Kerning != sal_False) &&
            (f1.WordLineMode != sal_False) == (f2.WordLineMode != sal_False) &&
            f1.Type == f2.Type);
}
//__________________________________________________________________________________________________
OUString StyleBag::getStyleId( Style const & rStyle )
    throw ()
{
    if (! rStyle._set) // nothin set
    {
        return OUString(); // everything default: no need to export a specific style
    }

    // lookup existing style
    for ( size_t nStylesPos = 0; nStylesPos < _styles.size(); ++nStylesPos )
    {
        Style * pStyle = _styles[ nStylesPos ];

        if ((rStyle._set & pStyle->_set & 0x1) != 0 &&
            rStyle._backgroundColor != pStyle->_backgroundColor)
        {
            continue;
        }
        if ((rStyle._set & pStyle->_set & 0x2) != 0 &&
            rStyle._textColor != pStyle->_textColor)
        {
            continue;
        }
        if ((rStyle._set & pStyle->_set & 0x4) != 0 &&
            rStyle._border != pStyle->_border)
        {
            continue;
        }
        if ((rStyle._set & pStyle->_set & 0x8) != 0 &&
            !equals( rStyle._descr, pStyle->_descr))
        {
            continue;
        }

        // merge in
        if ((rStyle._set & !pStyle->_set & 0x1) != 0)
        {
            pStyle->_backgroundColor = rStyle._backgroundColor;
        }
        if ((rStyle._set & !pStyle->_set & 0x2) != 0)
        {
            pStyle->_textColor = rStyle._textColor;
        }
        if ((rStyle._set & !pStyle->_set & 0x4) != 0)
        {
            pStyle->_border = rStyle._border;
        }
        if ((rStyle._set & !pStyle->_set & 0x8) != 0)
        {
            pStyle->_descr = rStyle._descr;
        }

        return pStyle->_id;
    }

    // no appr style found, append new
    Style * pStyle = new Style( rStyle );
    pStyle->_id = OUString::valueOf( (sal_Int32)_styles.size() );
    _styles.push_back( pStyle );
    return pStyle->_id;
}
//__________________________________________________________________________________________________
StyleBag::~StyleBag()
{
    for ( size_t nPos = 0; nPos < _styles.size(); ++nPos )
    {
        delete _styles[ nPos ];
    }
}
//__________________________________________________________________________________________________
void StyleBag::dump( Reference< xml::sax::XExtendedDocumentHandler > const & xOut )
{
    if (! _styles.empty())
    {
        OUString aStylesName( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":styles") );
        xOut->ignorableWhitespace( OUString() );
        xOut->startElement( aStylesName, Reference< xml::sax::XAttributeList >() );
        // export styles
        for ( size_t nPos = 0; nPos < _styles.size(); ++nPos )
        {
            Reference< xml::sax::XAttributeList > xAttr( _styles[ nPos ]->createElement() );
            static_cast< ElementDescriptor * >( xAttr.get() )->dump( xOut );
        }
        xOut->ignorableWhitespace( OUString() );
        xOut->endElement( aStylesName );
    }
}

//##################################################################################################

//__________________________________________________________________________________________________
void ElementDescriptor::addSubElem( Reference< xml::sax::XAttributeList > const & xElem )
{
    _subElems.push_back( xElem );
}
//__________________________________________________________________________________________________
void ElementDescriptor::dump( Reference< xml::sax::XExtendedDocumentHandler > const & xOut )
{
    xOut->ignorableWhitespace( OUString() );
    xOut->startElement( _name, static_cast< xml::sax::XAttributeList * >( this ) );
    // write sub elements
    for ( size_t nPos = 0; nPos < _subElems.size(); ++nPos )
    {
        ElementDescriptor * pElem = static_cast< ElementDescriptor * >( _subElems[ nPos ].get() );
        pElem->dump( xOut );
    }
    //
    xOut->ignorableWhitespace( OUString() );
    xOut->endElement( _name );
}

//==================================================================================================
void exportDialogModel(
    Reference< container::XNameContainer > const & xDialogModel,
    Reference< xml::sax::XExtendedDocumentHandler > const & xOut )
    throw (Exception)
{
    StyleBag all_styles;
    vector< Reference< xml::sax::XAttributeList > > all_elements;

    // read out all props

    Sequence< OUString > aElements( xDialogModel->getElementNames() );
    OUString const * pElements = aElements.getConstArray();

    sal_Int32 nPos;
    for ( nPos = 0; nPos < aElements.getLength(); ++nPos )
    {
        OUString const & rName = pElements[ nPos ];

        Any aControlModel( xDialogModel->getByName( rName ) );
        Reference< beans::XPropertySet > xProps;
        OSL_VERIFY( aControlModel >>= xProps );
        if (! xProps.is())
            continue;
        Reference< beans::XPropertyState > xPropState( xProps, UNO_QUERY );
        OSL_ASSERT( xPropState.is() );
        if (! xPropState.is())
            continue;
        Reference< lang::XServiceInfo > xServiceInfo( xProps, UNO_QUERY );
        OSL_ASSERT( xServiceInfo.is() );
        if (! xServiceInfo.is())
            continue;
        Sequence< OUString > aServiceNames( xServiceInfo->getSupportedServiceNames() );
        OSL_ASSERT( aServiceNames.getLength() == 1 );
        if (aServiceNames.getLength() != 1)
            continue;
        OUString aControlType( aServiceNames[ 0 ] );

//          OSL_VERIFY( xProps->getPropertyValue(
//              OUString( RTL_CONSTASCII_USTRINGPARAM("DefaultControl") ) ) >>= aControlType );

        ElementDescriptor * pElem;
        Reference< xml::sax::XAttributeList > xElem;

        if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlButtonModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":button") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readButtonModel( &all_styles );
        }
        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlCheckBoxModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":checkbox") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readCheckBoxModel( &all_styles );
        }
        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlComboBoxModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":combobox") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readComboBoxModel( &all_styles );
        }
        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlListBoxModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":menulist") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readListBoxModel( &all_styles );
        }


        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlRadioButtonModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readRadioButtonModel( &all_styles );
        }
        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlEditModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readEditModel( &all_styles );
        }
        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlCurrencyFieldModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readCurrencyFieldModel( &all_styles );
        }
        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlDateFieldModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readDateFieldModel( &all_styles );
        }
        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlFileControlModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readFileControlModel( &all_styles );
        }
        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlFixedTextModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readFixedTextModel( &all_styles );
        }
        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlGroupBoxModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readGroupBoxModel( &all_styles );
        }
        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlImageControlModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readImageControlModel( &all_styles );
        }
        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlNumericFieldModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readNumericFieldModel( &all_styles );
        }
        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlPatternFieldModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readPatternFieldModel( &all_styles );
        }
        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlTimeFieldModel") ))
        {
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readTimeFieldModel( &all_styles );
        }
        else if (aControlType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlContainerModel") ))
        {
            OSL_ENSURE( 0, "### ???" );
            // what to do ?
            pElem = new ElementDescriptor(
                xProps, xPropState,
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":") ) );
            xElem = static_cast< xml::sax::XAttributeList * >( pElem );
            pElem->readContainerModel( &all_styles );
        }
        else
        {
            throw Exception(
                OUString( RTL_CONSTASCII_USTRINGPARAM("unknown control type: ") ) + aControlType,
                Reference< XInterface >() );
        }

        OSL_ASSERT( xElem.is() );
        if (xElem.is())
        {
            pElem->addAttr(
                OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":id") ),
                rName );
            all_elements.push_back( xElem );
        }
    }

    if (! all_elements.empty()) // dump out
    {
        // window
        Reference< beans::XPropertySet > xProps( xDialogModel, UNO_QUERY );
        OSL_ASSERT( xProps.is() );
        Reference< beans::XPropertyState > xPropState( xProps, UNO_QUERY );
        OSL_ASSERT( xPropState.is() );

        OUString aWindowName( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":window") );
        ElementDescriptor * pWindow = new ElementDescriptor( xProps, xPropState, aWindowName );
        Reference< xml::sax::XAttributeList > xWindow( pWindow );
        pWindow->addAttr( OUString( RTL_CONSTASCII_USTRINGPARAM("xmlns:" XMLNS_DIALOGS_PREFIX) ),
                          OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_URI) ) );
        pWindow->readStringAttr( OUString( RTL_CONSTASCII_USTRINGPARAM("Title") ),
                                 OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":label") ) );
        pWindow->readLongAttr( OUString( RTL_CONSTASCII_USTRINGPARAM("Width") ),
                               OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":width") ) );
        pWindow->readLongAttr( OUString( RTL_CONSTASCII_USTRINGPARAM("Height") ),
                               OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":height") ) );
        xOut->ignorableWhitespace( OUString() );
        xOut->startElement( aWindowName, xWindow );

        // dump out stylebag
        all_styles.dump( xOut );

        if (! all_elements.empty())
        {
            // open up bulletinboard
            OUString aBBoardName( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_PREFIX ":bulletinboard") );
            xOut->ignorableWhitespace( OUString() );
            xOut->startElement( aBBoardName, Reference< xml::sax::XAttributeList >() );

            // export control elements
            for ( size_t nPos = 0; nPos < all_elements.size(); ++nPos )
            {
                ElementDescriptor * pElem = static_cast< ElementDescriptor * >( all_elements[ nPos ].get() );
                pElem->dump( xOut );
            }

            // end bulletinboard
            xOut->ignorableWhitespace( OUString() );
            xOut->endElement( aBBoardName );
        }

        // end window
        xOut->ignorableWhitespace( OUString() );
        xOut->endElement( aWindowName );
    }
}

};
