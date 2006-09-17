/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: postuhdl.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 10:56:12 $
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
#include "precompiled_xmloff.hxx"

#ifndef _XMLOFF_PROPERTYHANDLER_POSTURETYPES_HXX
#include <postuhdl.hxx>
#endif

#ifndef _XMLOFF_XMLTOKEN_HXX
#include "xmltoken.hxx"
#endif

#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif

#ifndef _COM_SUN_STAR_AWT_FONTSLANT_HPP_
#include <com/sun/star/awt/FontSlant.hpp>
#endif

#ifndef _VCL_VCLENUM_HXX
#include <vcl/vclenum.hxx>
#endif

#ifndef _XMLOFF_XMLEMENT_HXX
#include "xmlelement.hxx"
#endif

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::xmloff::token;

SvXMLEnumMapEntry __READONLY_DATA aPostureGenericMapping[] =
{
    { XML_POSTURE_NORMAL,       ITALIC_NONE     },
    { XML_POSTURE_ITALIC,       ITALIC_NORMAL   },
    { XML_POSTURE_OBLIQUE,      ITALIC_OBLIQUE  },
    { XML_TOKEN_INVALID,        0               }
};

///////////////////////////////////////////////////////////////////////////////
//
// class XMLPosturePropHdl
//

XMLPosturePropHdl::~XMLPosturePropHdl()
{
    // nothing to do
}

sal_Bool XMLPosturePropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
    sal_uInt16 ePosture;

    if( ( bRet = rUnitConverter.convertEnum( ePosture, rStrImpValue, aPostureGenericMapping ) ) )
        rValue <<= (awt::FontSlant)ePosture;

    return bRet;
}

sal_Bool XMLPosturePropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool bRet = sal_False;
    OUStringBuffer aOut;
    awt::FontSlant eSlant;

    if( !( rValue >>= eSlant ) )
    {
        sal_Int32 nValue;

        if( !( rValue >>= nValue ) )
            return sal_False;

        eSlant = (awt::FontSlant)nValue;
    }

    if( ( bRet = rUnitConverter.convertEnum( aOut, (sal_Int32)eSlant, aPostureGenericMapping ) ) )
        rStrExpValue = aOut.makeStringAndClear();

    return bRet;
}

