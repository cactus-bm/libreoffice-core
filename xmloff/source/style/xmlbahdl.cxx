/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmlbahdl.cxx,v $
 *
 *  $Revision: 1.20 $
 *
 *  last change: $Author: kz $ $Date: 2006-07-19 16:36:40 $
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

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _XMLOFF_PROPERTYHANDLER_BASICTYPES_HXX
#include <xmlbahdl.hxx>
#endif

#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif

#ifndef _XMLOFF_XMLTOKEN_HXX
#include "xmltoken.hxx"
#endif

using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::xmloff::token;

void lcl_xmloff_setAny( Any& rValue, sal_Int32 nValue, sal_Int8 nBytes )
{
    switch( nBytes )
    {
    case 1:
        if( nValue < SCHAR_MIN )
            nValue = SCHAR_MIN;
        else if( nValue > SCHAR_MAX )
            nValue = SCHAR_MAX;
        rValue <<= (sal_Int8)nValue;
        break;
    case 2:
        if( nValue < SHRT_MIN )
            nValue = SHRT_MIN;
        else if( nValue > SHRT_MAX )
            nValue = SHRT_MAX;
        rValue <<= (sal_Int16)nValue;
        break;
    case 4:
        rValue <<= nValue;
        break;
    }
}

sal_Bool lcl_xmloff_getAny( const Any& rValue, sal_Int32& nValue,
                            sal_Int8 nBytes )
{
    sal_Bool bRet = sal_False;

    switch( nBytes )
    {
    case 1:
        {
            sal_Int8 nValue8 = 0;
            bRet = rValue >>= nValue8;
            nValue = nValue8;
        }
        break;
    case 2:
        {
            sal_Int16 nValue16 = 0;
            bRet = rValue >>= nValue16;
            nValue = nValue16;
        }
        break;
    case 4:
        bRet = rValue >>= nValue;
        break;
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLNumberPropHdl
//

XMLNumberPropHdl::~XMLNumberPropHdl()
{
    // nothing to do
}

sal_Bool XMLNumberPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;

    sal_Int32 nValue = 0;
    bRet = rUnitConverter.convertNumber( nValue, rStrImpValue );
    lcl_xmloff_setAny( rValue, nValue, nBytes );

    return bRet;
}

sal_Bool XMLNumberPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
    sal_Int32 nValue;
      OUStringBuffer aOut;

    if( lcl_xmloff_getAny( rValue, nValue, nBytes ) )
    {
         rUnitConverter.convertNumber( aOut, nValue );
        rStrExpValue = aOut.makeStringAndClear();

        bRet = sal_True;
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// class XMLNumberNonePropHdl
//

XMLNumberNonePropHdl::XMLNumberNonePropHdl( sal_Int8 nB ) :
    sZeroStr( GetXMLToken(XML_NO_LIMIT) ),
    nBytes( nB )
{
}

XMLNumberNonePropHdl::XMLNumberNonePropHdl( enum XMLTokenEnum eZeroString, sal_Int8 nB ) :
    sZeroStr( GetXMLToken( eZeroString ) ),
    nBytes( nB )
{
}

XMLNumberNonePropHdl::~XMLNumberNonePropHdl()
{
    // nothing to do
}

sal_Bool XMLNumberNonePropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;

    sal_Int32 nValue = 0;
    if( rStrImpValue == sZeroStr )
    {
        bRet = sal_True;
    }
    else
    {
        bRet = rUnitConverter.convertNumber( nValue, rStrImpValue );
    }
    lcl_xmloff_setAny( rValue, nValue, nBytes );

    return bRet;
}

sal_Bool XMLNumberNonePropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
    sal_Int32 nValue;

    if( lcl_xmloff_getAny( rValue, nValue, nBytes ) )
    {
          OUStringBuffer aOut;

        if( nValue == 0 )
        {
            aOut.append( sZeroStr );
        }
        else
        {
             rUnitConverter.convertNumber( aOut, nValue );
        }

        rStrExpValue = aOut.makeStringAndClear();

        bRet = sal_True;
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLMeasurePropHdl
//

XMLMeasurePropHdl::~XMLMeasurePropHdl()
{
    // nothing to do
}

sal_Bool XMLMeasurePropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;

    sal_Int32 nValue = 0;
    bRet = rUnitConverter.convertMeasure( nValue, rStrImpValue );
    lcl_xmloff_setAny( rValue, nValue, nBytes );

    return bRet;
}

sal_Bool XMLMeasurePropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
    sal_Int32 nValue;
      OUStringBuffer aOut;

    if( lcl_xmloff_getAny( rValue, nValue, nBytes ) )
    {
         rUnitConverter.convertMeasure( aOut, nValue );
        rStrExpValue = aOut.makeStringAndClear();

        bRet = sal_True;
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLBoolPropHdl
//

XMLBoolPropHdl::~XMLBoolPropHdl()
{
    // nothing to do
}

sal_Bool XMLBoolPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;

    sal_Bool bValue;
    bRet = rUnitConverter.convertBool( bValue, rStrImpValue );
    rValue <<= sal_Bool(bValue);

    return bRet;
}

sal_Bool XMLBoolPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
      OUStringBuffer aOut;
    sal_Bool bValue;

    if (rValue >>= bValue)
    {
        rUnitConverter.convertBool( aOut, bValue );
        rStrExpValue = aOut.makeStringAndClear();

        bRet = sal_True;
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLNBoolPropHdl
//

XMLNBoolPropHdl::~XMLNBoolPropHdl()
{
    // nothing to do
}

sal_Bool XMLNBoolPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;

    sal_Bool bValue;
    bRet = rUnitConverter.convertBool( bValue, rStrImpValue );
    rValue <<= sal_Bool(!bValue);

    return bRet;
}

sal_Bool XMLNBoolPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
      OUStringBuffer aOut;
    sal_Bool bValue;

    if (rValue >>= bValue)
    {
        rUnitConverter.convertBool( aOut, !bValue );
        rStrExpValue = aOut.makeStringAndClear();

        bRet = sal_True;
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLPercentPropHdl
//

XMLPercentPropHdl::~XMLPercentPropHdl()
{
    // nothing to do
}

sal_Bool XMLPercentPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;

    sal_Int32 nValue = 0;
    bRet = rUnitConverter.convertPercent( nValue, rStrImpValue );
    lcl_xmloff_setAny( rValue, nValue, nBytes );

    return bRet;
}

sal_Bool XMLPercentPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
    sal_Int32 nValue;
      OUStringBuffer aOut;

    if( lcl_xmloff_getAny( rValue, nValue, nBytes ) )
    {
         rUnitConverter.convertPercent( aOut, nValue );
        rStrExpValue = aOut.makeStringAndClear();

        bRet = sal_True;
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLDoublePercentPropHdl
//

sal_Bool XMLDoublePercentPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;

    double fValue = 1.0;

    if( rStrImpValue.indexOf( (sal_Unicode)'%' ) == -1 )
    {
        fValue = rStrImpValue.toDouble();
    }
    else
    {
        sal_Int32 nValue = 0;
        bRet = rUnitConverter.convertPercent( nValue, rStrImpValue );
        fValue = ((double)nValue) / 100.0;
    }
    rValue <<= fValue;

    return bRet;
}

sal_Bool XMLDoublePercentPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
    double fValue;

    if( rValue >>= fValue )
    {
        fValue *= 100.0;
        if( fValue > 0 ) fValue += 0.5; else    fValue -= 0.5;

        sal_Int32 nValue = (sal_Int32)fValue;

        OUStringBuffer aOut;
         rUnitConverter.convertPercent( aOut, nValue );
        rStrExpValue = aOut.makeStringAndClear();

        bRet = sal_True;
    }

    return bRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// class XMLNegPercentPropHdl
//

XMLNegPercentPropHdl::~XMLNegPercentPropHdl()
{
    // nothing to do
}

sal_Bool XMLNegPercentPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;

    sal_Int32 nValue = 0;
    bRet = rUnitConverter.convertPercent( nValue, rStrImpValue );
    lcl_xmloff_setAny( rValue, 100-nValue, nBytes );

    return bRet;
}

sal_Bool XMLNegPercentPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
    sal_Int32 nValue;
      OUStringBuffer aOut;

    if( lcl_xmloff_getAny( rValue, nValue, nBytes ) )
    {
         rUnitConverter.convertPercent( aOut, 100-nValue );
        rStrExpValue = aOut.makeStringAndClear();

        bRet = sal_True;
    }

    return bRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// class XMLMeasurePxPropHdl
//

XMLMeasurePxPropHdl::~XMLMeasurePxPropHdl()
{
    // nothing to do
}

sal_Bool XMLMeasurePxPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;

    sal_Int32 nValue = 0;
    bRet = rUnitConverter.convertMeasurePx( nValue, rStrImpValue );
    lcl_xmloff_setAny( rValue, nValue, nBytes );

    return bRet;
}

sal_Bool XMLMeasurePxPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
    sal_Int32 nValue;
      OUStringBuffer aOut;

    if( lcl_xmloff_getAny( rValue, nValue, nBytes ) )
    {
         rUnitConverter.convertMeasurePx( aOut, nValue );
        rStrExpValue = aOut.makeStringAndClear();

        bRet = sal_True;
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLColorPropHdl
//

XMLColorPropHdl::~XMLColorPropHdl()
{
    // Nothing to do
}

sal_Bool XMLColorPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
    Color aColor;

    const OUString astrHSL( RTL_CONSTASCII_USTRINGPARAM( "hsl" ) );
    if( rStrImpValue.matchIgnoreAsciiCase( astrHSL ) )
    {
        sal_Int32 nOpen = rStrImpValue.indexOf( '(' );
        sal_Int32 nClose = rStrImpValue.lastIndexOf( ')' );

        if( (nOpen != -1) && (nClose > nOpen) )
        {
            const OUString aTmp( rStrImpValue.copy( nOpen+1, nClose - nOpen-1) );

            sal_Int32 nIndex = 0;

            Sequence< double > aHSL(3);
            aHSL[0] = aTmp.getToken( 0, ',', nIndex ).toDouble();
            aHSL[1] = aTmp.getToken( 0, ',', nIndex ).toDouble() / 100.0;
            aHSL[2] = aTmp.getToken( 0, ',', nIndex ).toDouble() / 100.0;
            rValue <<= aHSL;
            bRet = true;
        }
    }
    else
    {
        bRet = rUnitConverter.convertColor( aColor, rStrImpValue );
        rValue <<= (sal_Int32)( aColor.GetColor() );
    }

    return bRet;
}

sal_Bool XMLColorPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
    Color aColor;
    sal_Int32 nColor;

    OUStringBuffer aOut;
    if( rValue >>= nColor )
    {
        aColor.SetColor( nColor );

        rUnitConverter.convertColor( aOut, aColor );
        rStrExpValue = aOut.makeStringAndClear();

        bRet = sal_True;
    }
    else
    {
        Sequence< double > aHSL;
        if( (rValue >>= aHSL) && (aHSL.getLength() == 3) )
        {
            aOut.append( OUString::createFromAscii("hsl(") );
            aOut.append( aHSL[0] );
            aOut.append( OUString::createFromAscii(",") );
            aOut.append( aHSL[1] * 100.0 );
            aOut.append( OUString::createFromAscii("%,") );
            aOut.append( aHSL[2] * 100.0 );
            aOut.append( OUString::createFromAscii("%)") );
            rStrExpValue = aOut.makeStringAndClear();

            bRet = sal_True;
        }
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLStringPropHdl
//

XMLStringPropHdl::~XMLStringPropHdl()
{
    // Nothing to do
}

sal_Bool XMLStringPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& ) const
{
    sal_Bool bRet = sal_False;

    rValue <<= rStrImpValue;
    bRet = sal_True;

    return bRet;
}

sal_Bool XMLStringPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& ) const
{
    sal_Bool bRet = sal_False;

    if( rValue >>= rStrExpValue )
        bRet = sal_True;

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLStyleNamePropHdl
//

XMLStyleNamePropHdl::~XMLStyleNamePropHdl()
{
    // Nothing to do
}

sal_Bool XMLStyleNamePropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;

    if( rValue >>= rStrExpValue )
    {
        rStrExpValue = rUnitConverter.encodeStyleName( rStrExpValue );
        bRet = sal_True;
    }

    return bRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// class XMLDoublePropHdl
//

XMLDoublePropHdl::~XMLDoublePropHdl()
{
    // Nothing to do
}

sal_Bool XMLDoublePropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    double fDblValue;
    sal_Bool bRet = rUnitConverter.convertDouble( fDblValue, rStrImpValue );
    rValue <<= fDblValue;
    return bRet;
}

sal_Bool XMLDoublePropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;

    double fValue;

    if( rValue >>= fValue )
    {
        OUStringBuffer aOut;
        rUnitConverter.convertDouble( aOut, fValue );
        rStrExpValue = aOut.makeStringAndClear();
        bRet = sal_True;
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLColorTransparentPropHdl
//

XMLColorTransparentPropHdl::XMLColorTransparentPropHdl(
    enum XMLTokenEnum eTransparent ) :
    sTransparent( GetXMLToken(
        eTransparent != XML_TOKEN_INVALID ? eTransparent : XML_TRANSPARENT ) )
{
    // Nothing to do
}

XMLColorTransparentPropHdl::~XMLColorTransparentPropHdl()
{
    // Nothing to do
}

sal_Bool XMLColorTransparentPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;

    if( rStrImpValue != sTransparent )
    {
        Color aColor;
        bRet = rUnitConverter.convertColor( aColor, rStrImpValue );
        rValue <<= (sal_Int32)( aColor.GetColor() );
    }

    return bRet;
}

sal_Bool XMLColorTransparentPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
    sal_Int32 nColor;

    if( rStrExpValue == sTransparent )
        bRet = sal_False;
    else if( rValue >>= nColor )
    {
        Color aColor( nColor );
        OUStringBuffer aOut;
        rUnitConverter.convertColor( aOut, aColor );
        rStrExpValue = aOut.makeStringAndClear();

        bRet = sal_True;
    }

    return bRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// class XMLIsTransparentPropHdl
//

XMLIsTransparentPropHdl::XMLIsTransparentPropHdl(
    enum XMLTokenEnum eTransparent, sal_Bool bTransPropVal ) :
    sTransparent( GetXMLToken(
        eTransparent != XML_TOKEN_INVALID ? eTransparent : XML_TRANSPARENT ) ),
    bTransPropValue( bTransPropVal )
{
}

XMLIsTransparentPropHdl::~XMLIsTransparentPropHdl()
{
    // Nothing to do
}

sal_Bool XMLIsTransparentPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& ) const
{
    sal_Bool bValue = ( (rStrImpValue == sTransparent) == bTransPropValue);
    rValue.setValue( &bValue, ::getBooleanCppuType() );

    return sal_True;
}

sal_Bool XMLIsTransparentPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& ) const
{
    sal_Bool bRet = sal_False;

    // MIB: This looks a bit strange, because bTransPropValue == bValue should
    // do the same, but this only applies if 'true' is represented by the same
    // 8 bit value in bValue and bTransPropValue. Who will ensure this?
    sal_Bool bValue = *(sal_Bool *)rValue.getValue();
    sal_Bool bIsTrans = bTransPropValue ? bValue : !bValue;

    if( bIsTrans )
    {
        rStrExpValue = sTransparent;
        bRet = sal_True;
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLColorAutoPropHdl
//

XMLColorAutoPropHdl::XMLColorAutoPropHdl()
{
    // Nothing to do
}

XMLColorAutoPropHdl::~XMLColorAutoPropHdl()
{
    // Nothing to do
}

sal_Bool XMLColorAutoPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;

    // This is a multi property: the value might be set to AUTO_COLOR
    // already by the XMLIsAutoColorPropHdl!
    sal_Int32 nColor;
    if( !(rValue >>= nColor) || -1 != nColor )
    {
        Color aColor;
        bRet = rUnitConverter.convertColor( aColor, rStrImpValue );
        if( bRet )
            rValue <<= (sal_Int32)( aColor.GetColor() );
    }

    return bRet;
}

sal_Bool XMLColorAutoPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;

    sal_Int32 nColor;
    if( (rValue >>= nColor) && -1 != nColor )
    {
        Color aColor( nColor );
        OUStringBuffer aOut;
        rUnitConverter.convertColor( aOut, aColor );
        rStrExpValue = aOut.makeStringAndClear();

        bRet = sal_True;
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLIsAutoColorPropHdl
//

XMLIsAutoColorPropHdl::XMLIsAutoColorPropHdl()
{
}

XMLIsAutoColorPropHdl::~XMLIsAutoColorPropHdl()
{
    // Nothing to do
}

sal_Bool XMLIsAutoColorPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bValue;

    // An auto color overrides any other color set!
    sal_Bool bRet = rUnitConverter.convertBool( bValue, rStrImpValue );
    if( bRet && bValue )
        rValue <<= (sal_Int32)-1;

    return sal_True;
}

sal_Bool XMLIsAutoColorPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
    sal_Int32 nColor;

    if( (rValue >>= nColor) && -1 == nColor )
    {
        OUStringBuffer aOut;
        rUnitConverter.convertBool( aOut, sal_True );
        rStrExpValue = aOut.makeStringAndClear();

        bRet = sal_True;
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLCompareOnlyPropHdl
//

XMLCompareOnlyPropHdl::~XMLCompareOnlyPropHdl()
{
    // Nothing to do
}

sal_Bool XMLCompareOnlyPropHdl::importXML( const OUString&, Any&, const SvXMLUnitConverter& ) const
{
    DBG_ASSERT( !this, "importXML called for compare-only-property" );
    return sal_False;
}

sal_Bool XMLCompareOnlyPropHdl::exportXML( OUString&, const Any&, const SvXMLUnitConverter& ) const
{
    DBG_ASSERT( !this, "exportXML called for compare-only-property" );
    return sal_False;
}

///////////////////////////////////////////////////////////////////////////////
// class XMLNumberWithoutZeroPropHdl
//

XMLNumberWithoutZeroPropHdl::XMLNumberWithoutZeroPropHdl( sal_Int8 nB ) :
    nBytes( nB )
{
}

XMLNumberWithoutZeroPropHdl::~XMLNumberWithoutZeroPropHdl()
{
}

sal_Bool XMLNumberWithoutZeroPropHdl::importXML(
    const OUString& rStrImpValue,
    Any& rValue,
    const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Int32 nValue = 0;
    sal_Bool bRet = rUnitConverter.convertNumber( nValue, rStrImpValue );
    if( bRet )
        lcl_xmloff_setAny( rValue, nValue, nBytes );
    return bRet;
}

sal_Bool XMLNumberWithoutZeroPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{

    sal_Int32 nValue = 0;
    sal_Bool bRet = lcl_xmloff_getAny( rValue, nValue, nBytes );
    bRet &= nValue != 0;

    if( bRet )
    {
          OUStringBuffer aBuffer;
        rUnitConverter.convertNumber( aBuffer, nValue );
        rStrExpValue = aBuffer.makeStringAndClear();
    }

    return bRet;
}

