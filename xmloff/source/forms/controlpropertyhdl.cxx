/*************************************************************************
 *
 *  $RCSfile: controlpropertyhdl.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: fs $ $Date: 2000-12-19 08:43:00 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _XMLOFF_FORMS_CONTROLPROPERTYHDL_HXX_
#include "controlpropertyhdl.hxx"
#endif
#ifndef _COM_SUN_STAR_AWT_TEXTALIGN_HPP_
#include <com/sun/star/awt/TextAlign.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTWIDTH_HPP_
#include <com/sun/star/awt/FontWidth.hpp>
#endif
#ifndef _XMLOFF_XMLTYPES_HXX
#include "xmltypes.hxx"
#endif
#ifndef _XMLOFF_FORMENUMS_HXX_
#include "formenums.hxx"
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

//.........................................................................
namespace xmloff
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::awt;
    using namespace ::com::sun::star::beans;

    //=====================================================================
    //= OControlPropertyHandlerFactory
    //=====================================================================
    //---------------------------------------------------------------------
    OControlPropertyHandlerFactory::OControlPropertyHandlerFactory()
        :m_aTextAlignHandler(OEnumMapper::getEnumMap(OEnumMapper::epTextAlign), -1)
        ,m_aControlBorderHandler(OEnumMapper::getEnumMap(OEnumMapper::epBorderType))
    {
    }

    //---------------------------------------------------------------------
    OControlPropertyHandlerFactory::~OControlPropertyHandlerFactory()
    {
    }

    //---------------------------------------------------------------------
    const XMLPropertyHandler* OControlPropertyHandlerFactory::GetPropertyHandler(sal_Int32 _nType) const
    {
        switch (_nType)
        {
            case XML_TYPE_TEXT_ALIGN:
                return &m_aTextAlignHandler;
            case XML_TYPE_BORDER:
                return &m_aControlBorderHandler;
            case XML_TYPE_ROTATION_ANGLE:
                return &m_aRotationAngleHandler;
            default:
                return XMLPropertyHandlerFactory::GetPropertyHandler(_nType);
        }
    }

    //=====================================================================
    //= OEnumInt16Handler
    //=====================================================================
    //---------------------------------------------------------------------
    ORotationAngleHandler::ORotationAngleHandler()
    {
    }

    //---------------------------------------------------------------------
    sal_Bool ORotationAngleHandler::importXML( const ::rtl::OUString& _rStrImpValue, ::com::sun::star::uno::Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const
    {
        sal_Bool bSucces = sal_False;

        double fValue;
        if (bSucces = _rUnitConverter.convertNumber(fValue, _rStrImpValue))
        {
            fValue *= 10;
            _rValue <<= (float)fValue;
        }

        return bSucces;
    }

    //---------------------------------------------------------------------
    sal_Bool ORotationAngleHandler::exportXML( ::rtl::OUString& _rStrExpValue, const ::com::sun::star::uno::Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const
    {
        float fAngle;
        sal_Bool bSuccess = sal_False;

        if (bSuccess = (_rValue >>= fAngle))
        {
            rtl::OUStringBuffer sValue;
            _rUnitConverter.convertNumber(sValue, ((double)fAngle) / 10);
            _rStrExpValue = sValue.makeStringAndClear();
        }

        return bSuccess;
    }

    //=====================================================================
    //= OEnumInt16Handler
    //=====================================================================
    //---------------------------------------------------------------------
    OEnumInt16Handler::OEnumInt16Handler(const SvXMLEnumMapEntry* _pMap, sal_Int16 _nVoidValue)
        :m_pMap(_pMap)
        ,m_nVoidValue(_nVoidValue)
    {
    }

    //---------------------------------------------------------------------
    sal_Bool OEnumInt16Handler::importXML(const ::rtl::OUString& _rStrImpValue, ::com::sun::star::uno::Any& _rValue, const SvXMLUnitConverter& _rUnitConverter) const
    {
        // extract the value
        sal_uInt16 nValue = m_nVoidValue;
        sal_Bool bSuccess = _rUnitConverter.convertEnum(nValue, _rStrImpValue, m_pMap);
        if (!bSuccess || (m_nVoidValue == nValue))
            _rValue.clear();
        else
            _rValue <<= (sal_Int16)nValue;

        return bSuccess;
    }

    //---------------------------------------------------------------------
    sal_Bool OEnumInt16Handler::exportXML(::rtl::OUString& _rStrExpValue, const ::com::sun::star::uno::Any& _rValue, const SvXMLUnitConverter& _rUnitConverter) const
    {
        // extract the value
        sal_Int16 nValue = m_nVoidValue;
        if (_rValue.hasValue())
            _rValue >>= nValue;

        // convert it
        ::rtl::OUStringBuffer aBuffer;
        sal_Bool bSuccess = _rUnitConverter.convertEnum(aBuffer, nValue, m_pMap);
        _rStrExpValue = aBuffer.makeStringAndClear();

        // outta here
        return bSuccess;
    }

//.........................................................................
}   // namespace xmloff
//.........................................................................

/*************************************************************************
 * history:
 *  $Log: not supported by cvs2svn $
 *  Revision 1.1  2000/12/18 15:16:04  fs
 *  initial checkin - property handlers / property handler factory
 *
 *
 *  Revision 1.0 14.12.00 10:53:10  fs
 ************************************************************************/

