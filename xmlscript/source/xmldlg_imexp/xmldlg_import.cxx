/*************************************************************************
 *
 *  $RCSfile: xmldlg_import.cxx,v $
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
#include "imp_share.hxx"

#include <osl/diagnose.h>
#include <osl/mutex.hxx>

#include <rtl/ustrbuf.hxx>

#include <xmlscript/xml_helper.hxx>

#include <com/sun/star/awt/CharSet.hpp>
#include <com/sun/star/awt/FontFamily.hpp>
#include <com/sun/star/awt/FontPitch.hpp>
#include <com/sun/star/awt/FontSlant.hpp>
#include <com/sun/star/awt/FontStrikeout.hpp>
#include <com/sun/star/awt/FontType.hpp>
#include <com/sun/star/awt/FontUnderline.hpp>
#include <com/sun/star/awt/FontWeight.hpp>
#include <com/sun/star/awt/FontWidth.hpp>


using namespace ::osl;

namespace xmlscript
{

//__________________________________________________________________________________________________
ControlElement::ControlElement(
    OUString const & rLocalName,
    Reference< xml::sax2::XExtendedAttributes > const & xAttributes,
    ElementBase * pParent, DialogImport * pImport )
    throw ()
    : ElementBase( rLocalName, xAttributes, pParent, pImport )
{
    if (_pParent)
    {
        // inherit position
        _nBasePosX = static_cast< ControlElement * >( _pParent )->_nBasePosX;
        _nBasePosY = static_cast< ControlElement * >( _pParent )->_nBasePosY;
    }
    else
    {
        _nBasePosX = 0;
        _nBasePosY = 0;
    }
}
//__________________________________________________________________________________________________
Reference< xml::XImportContext > ControlElement::getStyle()
{
    OUString aStyleId( _xAttributes->getValueByUidName(
        XMLNS_DIALOGS_UID, OUString( RTL_CONSTASCII_USTRINGPARAM("style-id") ) ) );
    if (aStyleId.getLength())
    {
        return _pImport->getStyle( aStyleId );
    }
    return Reference< xml::XImportContext >();
}
//__________________________________________________________________________________________________
OUString ControlElement::getControlId()
{
    OUString aId( _xAttributes->getValueByUidName(
        XMLNS_DIALOGS_UID, OUString( RTL_CONSTASCII_USTRINGPARAM("id") ) ) );
    if (! aId.getLength())
    {
        throw xml::sax::SAXException(
            OUString( RTL_CONSTASCII_USTRINGPARAM("missing id element for button!") ),
            Reference< XInterface >(), Any() );
    }
    return aId;
}

//##################################################################################################

//__________________________________________________________________________________________________
bool StyleElement::importTextColorStyle(
    Reference< beans::XPropertySet > const & xProps )
{
    if ((_inited & 0x1) != 0)
    {
        if ((_hasValue & 0x1) != 0)
        {
            xProps->setPropertyValue(
                OUString( RTL_CONSTASCII_USTRINGPARAM("TextColor") ), makeAny( _textColor ) );
            return true;
        }
        return false;
    }
    _inited |= 0x1;

    if (getLongAttr( &_textColor,
                     OUString( RTL_CONSTASCII_USTRINGPARAM("text-color") ), _xAttributes ))
    {
        _hasValue |= 0x1;
        xProps->setPropertyValue(
            OUString( RTL_CONSTASCII_USTRINGPARAM("TextColor") ), makeAny( _textColor ) );
        return true;
    }
    return false;
}
//__________________________________________________________________________________________________
bool StyleElement::importBackgroundColorStyle(
    Reference< beans::XPropertySet > const & xProps )
{
    if ((_inited & 0x2) != 0)
    {
        if ((_hasValue & 0x2) != 0)
        {
            xProps->setPropertyValue(
                OUString( RTL_CONSTASCII_USTRINGPARAM("BackgroundColor") ), makeAny( _backgroundColor ) );
            return true;
        }
        return false;
    }
    _inited |= 0x2;

    if (getLongAttr( &_backgroundColor,
                     OUString( RTL_CONSTASCII_USTRINGPARAM("background-color") ), _xAttributes ))
    {
        _hasValue |= 0x2;
        xProps->setPropertyValue(
            OUString( RTL_CONSTASCII_USTRINGPARAM("BackgroundColor") ), makeAny( _backgroundColor ) );
        return true;
    }
    return false;
}
//__________________________________________________________________________________________________
bool StyleElement::importBorderStyle(
    Reference< beans::XPropertySet > const & xProps )
{
    if ((_inited & 0x4) != 0)
    {
        if ((_hasValue & 0x4) != 0)
        {
            xProps->setPropertyValue(
                OUString( RTL_CONSTASCII_USTRINGPARAM("Border") ), makeAny( _border ) );
            return true;
        }
        return false;
    }
    _inited |= 0x4;

    OUString aValue;
    if (getStringAttr( &aValue, OUString( RTL_CONSTASCII_USTRINGPARAM("border") ), _xAttributes ))
    {
        if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("none") ))
        {
            _border = 0;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("3d") ))
        {
            _border = 1;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("simple") ))
        {
            _border = 2;
        }
        else
        {
            throw xml::sax::SAXException(
                OUString( RTL_CONSTASCII_USTRINGPARAM("invalid border value!") ),
                Reference< XInterface >(), Any() );
        }

        _hasValue |= 0x4;
        xProps->setPropertyValue(
                OUString( RTL_CONSTASCII_USTRINGPARAM("Border") ), makeAny( _border ) );
    }
    return false;
}
//__________________________________________________________________________________________________
bool StyleElement::importFontStyle(
    Reference< beans::XPropertySet > const & xProps )
{
    if ((_inited & 0x8) != 0)
    {
        if ((_hasValue & 0x8) != 0)
        {
            xProps->setPropertyValue(
                OUString( RTL_CONSTASCII_USTRINGPARAM("FontDescriptor") ), makeAny( _descr ) );
            return true;
        }
        return false;
    }
    _inited |= 0x8;

    OUString aValue;
    bool bFontImport;

    // dialog:font-name CDATA #IMPLIED
    bFontImport = getStringAttr( &_descr.Name, OUString( RTL_CONSTASCII_USTRINGPARAM("font-name") ), _xAttributes );

    // dialog:font-height %numeric; #IMPLIED
    if (getStringAttr( &aValue, OUString( RTL_CONSTASCII_USTRINGPARAM("font-height") ), _xAttributes ))
    {
        _descr.Height = (sal_Int16)toInt32( aValue );
        bFontImport = true;
    }
    // dialog:font-width %numeric; #IMPLIED
    if (getStringAttr( &aValue, OUString( RTL_CONSTASCII_USTRINGPARAM("font-width") ), _xAttributes ))
    {
        _descr.Width = (sal_Int16)toInt32( aValue );
        bFontImport = true;
    }
    // dialog:font-stylename CDATA #IMPLIED
    bFontImport |= getStringAttr( &_descr.StyleName, OUString( RTL_CONSTASCII_USTRINGPARAM("font-stylename") ), _xAttributes );

    // dialog:font-family "(decorative|modern|roman|script|swiss|system)" #IMPLIED
    if (getStringAttr( &aValue, OUString( RTL_CONSTASCII_USTRINGPARAM("font-family") ), _xAttributes ))
    {
        if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("decorative") ))
        {
            _descr.Family = awt::FontFamily::DECORATIVE;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("modern") ))
        {
            _descr.Family = awt::FontFamily::MODERN;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("roman") ))
        {
            _descr.Family = awt::FontFamily::ROMAN;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("script") ))
        {
            _descr.Family = awt::FontFamily::SCRIPT;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("swiss") ))
        {
            _descr.Family = awt::FontFamily::SWISS;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("system") ))
        {
            _descr.Family = awt::FontFamily::SYSTEM;
        }
        else
        {
            throw xml::sax::SAXException(
                OUString( RTL_CONSTASCII_USTRINGPARAM("invalid font-family style!") ),
                Reference< XInterface >(), Any() );
        }
        bFontImport = true;
    }

    // dialog:font-charset "(ansi|mac|ibmpc_437|ibmpc_850|ibmpc_860|ibmpc_861|ibmpc_863|ibmpc_865|system|symbol)" #IMPLIED
    if (getStringAttr( &aValue, OUString( RTL_CONSTASCII_USTRINGPARAM("font-charset") ), _xAttributes ))
    {
        if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("ansi") ))
        {
            _descr.CharSet = awt::CharSet::ANSI;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("mac") ))
        {
            _descr.CharSet = awt::CharSet::MAC;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("ibmpc_437") ))
        {
            _descr.CharSet = awt::CharSet::IBMPC_437;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("ibmpc_850") ))
        {
            _descr.CharSet = awt::CharSet::IBMPC_850;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("ibmpc_860") ))
        {
            _descr.CharSet = awt::CharSet::IBMPC_860;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("ibmpc_861") ))
        {
            _descr.CharSet = awt::CharSet::IBMPC_861;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("ibmpc_863") ))
        {
            _descr.CharSet = awt::CharSet::IBMPC_863;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("ibmpc_865") ))
        {
            _descr.CharSet = awt::CharSet::IBMPC_865;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("system") ))
        {
            _descr.CharSet = awt::CharSet::SYSTEM;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("symbol") ))
        {
            _descr.CharSet = awt::CharSet::SYMBOL;
        }
        else
        {
            throw xml::sax::SAXException(
                OUString( RTL_CONSTASCII_USTRINGPARAM("invalid font-charset style!") ),
                Reference< XInterface >(), Any() );
        }
        bFontImport = true;
    }

    // dialog:font-pitch "(fixed|variable)" #IMPLIED
    if (getStringAttr( &aValue, OUString( RTL_CONSTASCII_USTRINGPARAM("font-pitch") ), _xAttributes ))
    {
        if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("fixed") ))
        {
            _descr.Pitch = awt::FontPitch::FIXED;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("variable") ))
        {
            _descr.Pitch = awt::FontPitch::VARIABLE;
        }
        else
        {
            throw xml::sax::SAXException(
                OUString( RTL_CONSTASCII_USTRINGPARAM("invalid font-pitch style!") ),
                Reference< XInterface >(), Any() );
        }
        bFontImport = true;
    }

    // dialog:font-charwidth CDATA #IMPLIED
    if (getStringAttr( &aValue, OUString( RTL_CONSTASCII_USTRINGPARAM("font-charwidth") ), _xAttributes ))
    {
        _descr.CharacterWidth = aValue.toFloat();
        bFontImport = true;
    }
    // dialog:font-weight CDATA #IMPLIED
    if (getStringAttr( &aValue, OUString( RTL_CONSTASCII_USTRINGPARAM("font-weight") ), _xAttributes ))
    {
        _descr.Weight = aValue.toFloat();
        bFontImport = true;
    }

    // dialog:font-slant "(oblique|italic|reverse_oblique|reverse_italic)" #IMPLIED
    if (getStringAttr( &aValue, OUString( RTL_CONSTASCII_USTRINGPARAM("font-slant") ), _xAttributes ))
    {
        if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("oblique") ))
        {
            _descr.Slant = awt::FontSlant_OBLIQUE;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("italic") ))
        {
            _descr.Slant = awt::FontSlant_ITALIC;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("reverse_oblique") ))
        {
            _descr.Slant = awt::FontSlant_REVERSE_OBLIQUE;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("reverse_italic") ))
        {
            _descr.Slant = awt::FontSlant_REVERSE_ITALIC;
        }
        else
        {
            throw xml::sax::SAXException(
                OUString( RTL_CONSTASCII_USTRINGPARAM("invalid font-slant style!") ),
                Reference< XInterface >(), Any() );
        }
        bFontImport = true;
    }

    // dialog:font-underline "(single|double|dotted|dash|longdash|dashdot|dashdotdot|smallwave|wave|doublewave|bold|bolddotted|bolddash|boldlongdash|bolddashdot|bolddashdotdot|boldwave)" #IMPLIED
    if (getStringAttr( &aValue, OUString( RTL_CONSTASCII_USTRINGPARAM("font-underline") ), _xAttributes ))
    {
        if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("single") ))
        {
            _descr.Underline = awt::FontUnderline::SINGLE;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("double") ))
        {
            _descr.Underline = awt::FontUnderline::DOUBLE;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("dotted") ))
        {
            _descr.Underline = awt::FontUnderline::DOTTED;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("dash") ))
        {
            _descr.Underline = awt::FontUnderline::DASH;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("longdash") ))
        {
            _descr.Underline = awt::FontUnderline::LONGDASH;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("dashdot") ))
        {
            _descr.Underline = awt::FontUnderline::DASHDOT;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("dashdotdot") ))
        {
            _descr.Underline = awt::FontUnderline::DASHDOTDOT;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("smallwave") ))
        {
            _descr.Underline = awt::FontUnderline::SMALLWAVE;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("wave") ))
        {
            _descr.Underline = awt::FontUnderline::WAVE;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("doublewave") ))
        {
            _descr.Underline = awt::FontUnderline::DOUBLEWAVE;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("bold") ))
        {
            _descr.Underline = awt::FontUnderline::BOLD;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("bolddotted") ))
        {
            _descr.Underline = awt::FontUnderline::BOLDDOTTED;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("bolddash") ))
        {
            _descr.Underline = awt::FontUnderline::BOLDDASH;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("boldlongdash") ))
        {
            _descr.Underline = awt::FontUnderline::BOLDLONGDASH;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("bolddashdot") ))
        {
            _descr.Underline = awt::FontUnderline::BOLDDASHDOT;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("bolddashdotdot") ))
        {
            _descr.Underline = awt::FontUnderline::BOLDDASHDOTDOT;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("boldwave") ))
        {
            _descr.Underline = awt::FontUnderline::BOLDWAVE;
        }
        else
        {
            throw xml::sax::SAXException(
                OUString( RTL_CONSTASCII_USTRINGPARAM("invalid font-underline style!") ),
                Reference< XInterface >(), Any() );
        }
        bFontImport = true;
    }

    // dialog:font-strikeout "(single|double|bold|slash|x)" #IMPLIED
    if (getStringAttr( &aValue, OUString( RTL_CONSTASCII_USTRINGPARAM("font-strikeout") ), _xAttributes ))
    {
        if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("single") ))
        {
            _descr.Strikeout = awt::FontStrikeout::SINGLE;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("double") ))
        {
            _descr.Strikeout = awt::FontStrikeout::DOUBLE;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("bold") ))
        {
            _descr.Strikeout = awt::FontStrikeout::BOLD;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("slash") ))
        {
            _descr.Strikeout = awt::FontStrikeout::SLASH;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("x") ))
        {
            _descr.Strikeout = awt::FontStrikeout::X;
        }
        else
        {
            throw xml::sax::SAXException(
                OUString( RTL_CONSTASCII_USTRINGPARAM("invalid font-strikeout style!") ),
                Reference< XInterface >(), Any() );
        }
        bFontImport = true;
    }

    // dialog:font-orientation CDATA #IMPLIED
    if (getStringAttr( &aValue, OUString( RTL_CONSTASCII_USTRINGPARAM("font-orientation") ), _xAttributes ))
    {
        _descr.Orientation = aValue.toFloat();
        bFontImport = true;
    }
    // dialog:font-kerning %boolean; #IMPLIED
    bFontImport |= getBoolAttr( &_descr.Kerning, OUString( RTL_CONSTASCII_USTRINGPARAM("font-kerning") ), _xAttributes );
    // dialog:font-wordlinemode %boolean; #IMPLIED
    bFontImport |= getBoolAttr( &_descr.WordLineMode, OUString( RTL_CONSTASCII_USTRINGPARAM("font-wordlinemode") ), _xAttributes );

    // dialog:font-type "(raster|device|scalable)" #IMPLIED
    if (getStringAttr( &aValue, OUString( RTL_CONSTASCII_USTRINGPARAM("font-type") ), _xAttributes ))
    {
        if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("raster") ))
        {
            _descr.Type = awt::FontType::RASTER;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("device") ))
        {
            _descr.Type = awt::FontType::DEVICE;
        }
        else if (aValue.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("scalable") ))
        {
            _descr.Type = awt::FontType::SCALABLE;
        }
        else
        {
            throw xml::sax::SAXException(
                OUString( RTL_CONSTASCII_USTRINGPARAM("invalid font-type style!") ),
                Reference< XInterface >(), Any() );
        }
        bFontImport = true;
    }

    if (bFontImport)
    {
        _hasValue |= 0x8;
        xProps->setPropertyValue(
            OUString( RTL_CONSTASCII_USTRINGPARAM("FontDescriptor") ), makeAny( _descr ) );
    }

    return bFontImport;
}

//##################################################################################################

//__________________________________________________________________________________________________
bool ControlImportContext::importStringProperty(
    OUString const & rPropName, OUString const & rAttrName,
    Reference< xml::sax2::XExtendedAttributes > const & xAttributes )
{
    OUString aValue( xAttributes->getValueByUidName( XMLNS_DIALOGS_UID, rAttrName ) );
    if (aValue.getLength())
    {
        _xControlModel->setPropertyValue( rPropName, makeAny( aValue ) );
        return true;
    }
    return false;
}
//__________________________________________________________________________________________________
bool ControlImportContext::importBooleanProperty(
    OUString const & rPropName, OUString const & rAttrName,
    Reference< xml::sax2::XExtendedAttributes > const & xAttributes )
{
    sal_Bool bBool;
    if (getBoolAttr( &bBool, rAttrName, xAttributes ))
    {
        _xControlModel->setPropertyValue( rPropName, makeAny( bBool ) );
        return true;
    }
    return false;
}
//__________________________________________________________________________________________________
bool ControlImportContext::importLongProperty(
    OUString const & rPropName, OUString const & rAttrName,
    Reference< xml::sax2::XExtendedAttributes > const & xAttributes )
{
    OUString aValue( xAttributes->getValueByUidName( XMLNS_DIALOGS_UID, rAttrName ) );
    if (aValue.getLength())
    {
        _xControlModel->setPropertyValue( rPropName, makeAny( toInt32( aValue ) ) );
        return true;
    }
    return false;
}
//__________________________________________________________________________________________________
bool ControlImportContext::importShortProperty(
    OUString const & rPropName, OUString const & rAttrName,
    Reference< xml::sax2::XExtendedAttributes > const & xAttributes )
{
    OUString aValue( xAttributes->getValueByUidName( XMLNS_DIALOGS_UID, rAttrName ) );
    if (aValue.getLength())
    {
        _xControlModel->setPropertyValue( rPropName, makeAny( (sal_Int16)toInt32( aValue ) ) );
        return true;
    }
    return false;
}
//__________________________________________________________________________________________________
void ControlImportContext::importDefaults(
    Reference< xml::sax2::XExtendedAttributes > const & xAttributes )
{
    if (!importLongProperty( OUString( RTL_CONSTASCII_USTRINGPARAM("PositionX") ),
                             OUString( RTL_CONSTASCII_USTRINGPARAM("left") ),
                             xAttributes ) ||
        !importLongProperty( OUString( RTL_CONSTASCII_USTRINGPARAM("PositionY") ),
                             OUString( RTL_CONSTASCII_USTRINGPARAM("top") ),
                             xAttributes ) ||
        !importLongProperty( OUString( RTL_CONSTASCII_USTRINGPARAM("Width") ),
                             OUString( RTL_CONSTASCII_USTRINGPARAM("width") ),
                             xAttributes ) ||
        !importLongProperty( OUString( RTL_CONSTASCII_USTRINGPARAM("Height") ),
                             OUString( RTL_CONSTASCII_USTRINGPARAM("height") ),
                             xAttributes ))
    {
        throw xml::sax::SAXException(
            OUString( RTL_CONSTASCII_USTRINGPARAM("missing pos size attribute(s)!") ),
            Reference< XInterface >(), Any() );
    }

    importBooleanProperty( OUString( RTL_CONSTASCII_USTRINGPARAM("Enabled") ),
                           OUString( RTL_CONSTASCII_USTRINGPARAM("default") ),
                           xAttributes );
    importBooleanProperty( OUString( RTL_CONSTASCII_USTRINGPARAM("Printable") ),
                           OUString( RTL_CONSTASCII_USTRINGPARAM("printable") ),
                           xAttributes );
    importBooleanProperty( OUString( RTL_CONSTASCII_USTRINGPARAM("Tabstop") ),
                           OUString( RTL_CONSTASCII_USTRINGPARAM("tabstop") ),
                           xAttributes );
}

//##################################################################################################

//__________________________________________________________________________________________________
Reference< xml::XImportContext > ElementBase::getParent()
    throw (RuntimeException)
{
    return static_cast< xml::XImportContext * >( _pParent );
}
//__________________________________________________________________________________________________
OUString ElementBase::getLocalName()
    throw (RuntimeException)
{
    return _aLocalName;
}
//__________________________________________________________________________________________________
sal_Int32 ElementBase::getUid()
    throw (RuntimeException)
{
    return XMLNS_DIALOGS_UID;
}
//__________________________________________________________________________________________________
Reference< xml::sax2::XExtendedAttributes > ElementBase::getAttributes()
    throw (RuntimeException)
{
    return _xAttributes;
}
//__________________________________________________________________________________________________
void ElementBase::ignorableWhitespace(
    OUString const & rWhitespaces )
    throw (xml::sax::SAXException, RuntimeException)
{
    // not used
}
//__________________________________________________________________________________________________
void ElementBase::characters( OUString const & rChars )
    throw (xml::sax::SAXException, RuntimeException)
{
    // not used, all characters ignored
}
//__________________________________________________________________________________________________
void ElementBase::endElement()
    throw (xml::sax::SAXException, RuntimeException)
{
}
//__________________________________________________________________________________________________
Reference< xml::XImportContext > ElementBase::createChildContext(
    sal_Int32 nUid, OUString const & rLocalName,
    Reference< xml::sax2::XExtendedAttributes > const & xAttributes )
    throw (xml::sax::SAXException, RuntimeException)
{
    throw xml::sax::SAXException(
        OUString( RTL_CONSTASCII_USTRINGPARAM("unexpected element!") ),
        Reference< XInterface >(), Any() );
}

//__________________________________________________________________________________________________
ElementBase::ElementBase(
    OUString const & rLocalName,
    Reference< xml::sax2::XExtendedAttributes > const & xAttributes,
    ElementBase * pParent, DialogImport * pImport )
    throw ()
    : _aLocalName( rLocalName )
    , _xAttributes( xAttributes )
    , _pParent( pParent )
    , _pImport( pImport )
{
    _pImport->acquire();

    if (_pParent)
    {
        _pParent->acquire();
    }
}
//__________________________________________________________________________________________________
ElementBase::~ElementBase()
    throw ()
{
    _pImport->release();

    if (_pParent)
    {
        _pParent->release();
    }
}

//##################################################################################################

// XImporter
//__________________________________________________________________________________________________
void DialogImport::startDocument()
    throw (xml::sax::SAXException, RuntimeException)
{
    // ignored
}
//__________________________________________________________________________________________________
void DialogImport::endDocument()
    throw (xml::sax::SAXException, RuntimeException)
{
    // ignored
}
//__________________________________________________________________________________________________
void DialogImport::processingInstruction(
    OUString const & rTarget, OUString const & rData )
    throw (xml::sax::SAXException, RuntimeException)
{
    // ignored for now: xxx todo
}
//__________________________________________________________________________________________________
void DialogImport::setDocumentLocator(
    Reference< xml::sax::XLocator > const & xLocator )
    throw (xml::sax::SAXException, RuntimeException)
{
    // ignored for now: xxx todo
}
//__________________________________________________________________________________________________
Reference< xml::XImportContext > DialogImport::createRootContext(
    sal_Int32 nUid, OUString const & rLocalName,
    Reference< xml::sax2::XExtendedAttributes > const & xAttributes )
    throw (xml::sax::SAXException, RuntimeException)
{
    if (nUid == XMLNS_DIALOGS_UID &&
        rLocalName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("window") ))
    {
        return new WindowElement( rLocalName, xAttributes, 0, this );
    }
    else
    {
        throw xml::sax::SAXException(
            OUString( RTL_CONSTASCII_USTRINGPARAM("illegal root element given for dialog: ") ) +
            rLocalName, Reference< XInterface >(), Any() );
    }
}

//__________________________________________________________________________________________________
void DialogImport::addStyle(
    OUString const & rStyleId,
    Reference< xml::XImportContext > const & xStyle )
    throw ()
{
    _styleNames.push_back( rStyleId );
    _styles.push_back( xStyle );
}
//__________________________________________________________________________________________________
Reference< xml::XImportContext > DialogImport::getStyle(
    OUString const & rStyleId ) const
    throw ()
{
    for ( size_t nPos = 0; nPos < _styleNames.size(); ++nPos )
    {
        if (_styleNames[ nPos ] == rStyleId)
        {
            return _styles[ nPos ];
        }
    }
    return 0;
}

//##################################################################################################

//==================================================================================================
Reference< xml::sax::XDocumentHandler > importDialogModel(
    Reference< container::XNameContainer > const & xDialogModel )
    throw (Exception)
{
    NameSpaceUid arNamespaceUids[] =
    {
        NameSpaceUid( OUString( RTL_CONSTASCII_USTRINGPARAM(XMLNS_DIALOGS_URI) ), XMLNS_DIALOGS_UID )
    };

    return ::xmlscript::createDocumentHandler(
        arNamespaceUids, sizeof(arNamespaceUids) / sizeof(NameSpaceUid),
        -1 /* unknown namespace id */,
        static_cast< xml::XImporter * >( new DialogImport( xDialogModel ) ) );
}

};
