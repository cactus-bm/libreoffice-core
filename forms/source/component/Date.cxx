/*************************************************************************
 *
 *  $RCSfile: Date.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 16:29:05 $
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

#ifndef _FORMS_DATE_HXX_
#include "Date.hxx"
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _DATE_HXX
#include <tools/date.hxx>
#endif
#ifndef _UNOTOOLS_DATETIME_HXX_
#include <unotools/datetime.hxx>
#endif
#ifndef _UTL_DB_CONVERSION_HXX_
#include <unotools/dbconversion.hxx>
#endif

#ifndef _COM_SUN_STAR_SDBC_DATATYPE_HPP_
#include <com/sun/star/sdbc/DataType.hpp>
#endif

//.........................................................................
namespace frm
{
//.........................................................................

//------------------------------------------------------------------
ODateControl::ODateControl(const staruno::Reference<starlang::XMultiServiceFactory>& _rxFactory)
               :OBoundControl(_rxFactory, VCL_CONTROL_DATEFIELD)
{
}

//------------------------------------------------------------------
InterfaceRef SAL_CALL ODateControl_CreateInstance(const staruno::Reference<starlang::XMultiServiceFactory>& _rxFactory)
{
    return *(new ODateControl(_rxFactory));
}

//------------------------------------------------------------------------------
staruno::Sequence<staruno::Type> ODateControl::_getTypes()
{
    return OBoundControl::_getTypes();
}

//------------------------------------------------------------------------------
StringSequence SAL_CALL ODateControl::getSupportedServiceNames() throw()
{
    StringSequence aSupported = OBoundControl::getSupportedServiceNames();
    aSupported.realloc(aSupported.getLength() + 1);

    ::rtl::OUString*pArray = aSupported.getArray();
    pArray[aSupported.getLength()-1] = FRM_SUN_CONTROL_DATEFIELD;
    return aSupported;
}

/*************************************************************************/
sal_Int32   ODateModel::nDateHandle = -1;
//------------------------------------------------------------------
InterfaceRef SAL_CALL ODateModel_CreateInstance(const staruno::Reference<starlang::XMultiServiceFactory>& _rxFactory)
{
    return *(new ODateModel(_rxFactory));
}

//------------------------------------------------------------------------------
staruno::Sequence<staruno::Type> ODateModel::_getTypes()
{
    return OEditBaseModel::_getTypes();
}

//------------------------------------------------------------------
ODateModel::ODateModel(const staruno::Reference<starlang::XMultiServiceFactory>& _rxFactory)
             :OEditBaseModel(_rxFactory, VCL_CONTROLMODEL_DATEFIELD, FRM_CONTROL_DATEFIELD )
                                    // use the old control name for compytibility reasons
{
    m_nClassId = starform::FormComponentType::DATEFIELD;
    m_sDataFieldConnectivityProperty = PROPERTY_DATE;
    if (ODateModel::nDateHandle == -1)
        ODateModel::nDateHandle = getOriginalHandle(PROPERTY_ID_DATE);
}

// starlang::XServiceInfo
//------------------------------------------------------------------------------
StringSequence SAL_CALL ODateModel::getSupportedServiceNames() throw()
{
    StringSequence aSupported = OBoundControlModel::getSupportedServiceNames();
    aSupported.realloc(aSupported.getLength() + 2);

    ::rtl::OUString*pArray = aSupported.getArray();
    pArray[aSupported.getLength()-2] = ::rtl::OUString::createFromAscii("com.sun.star.form.component.starform");
    pArray[aSupported.getLength()-1] = FRM_SUN_COMPONENT_DATEFIELD;
    return aSupported;
}

//------------------------------------------------------------------------------
::rtl::OUString SAL_CALL ODateModel::getServiceName()
{
    return FRM_COMPONENT_DATEFIELD; // old (non-sun) name for compatibility !
}

// starbeans::XPropertySet
//------------------------------------------------------------------------------
staruno::Reference<starbeans::XPropertySetInfo> SAL_CALL ODateModel::getPropertySetInfo() throw( staruno::RuntimeException )
{
    staruno::Reference<starbeans::XPropertySetInfo>  xInfo( createPropertySetInfo( getInfoHelper() ) );
    return xInfo;
}

//------------------------------------------------------------------------------
void ODateModel::fillProperties(
        staruno::Sequence< starbeans::Property >& _rProps,
        staruno::Sequence< starbeans::Property >& _rAggregateProps ) const
{
    FRM_BEGIN_PROP_HELPER(10)
        // Date auf transient setzen
//      ModifyPropertyAttributes(_rAggregateProps, PROPERTY_DATE, starbeans::PropertyAttribute::TRANSIENT, 0);

        DECL_PROP1(NAME,        ::rtl::OUString, BOUND);
        DECL_PROP2(CLASSID,     sal_Int16, READONLY, TRANSIENT);
        DECL_PROP3(DEFAULT_DATE,        sal_Int32, BOUND, MAYBEDEFAULT, MAYBEVOID);
        DECL_PROP1(TAG,     ::rtl::OUString, BOUND);
        DECL_PROP1(TABINDEX,        sal_Int16, BOUND);
        DECL_PROP1(CONTROLSOURCE,       ::rtl::OUString, BOUND);
        DECL_PROP1(HELPTEXT,        ::rtl::OUString,            BOUND);
        DECL_IFACE_PROP2(BOUNDFIELD,        starbeans::XPropertySet,    READONLY, TRANSIENT);
        DECL_IFACE_PROP2(CONTROLLABEL,      starbeans::XPropertySet,    BOUND, MAYBEVOID);
        DECL_PROP2(CONTROLSOURCEPROPERTY,   rtl::OUString,  READONLY, TRANSIENT);
    FRM_END_PROP_HELPER();
}

//------------------------------------------------------------------------------
::cppu::IPropertyArrayHelper& ODateModel::getInfoHelper()
{
    return *const_cast<ODateModel*>(this)->getArrayHelper();
}

// starform::XLoadListener
//------------------------------------------------------------------------------
void ODateModel::_loaded(const starlang::EventObject& rEvent)
{
    OBoundControlModel::_loaded(rEvent);
    if (m_xField.is())
    {
        m_bDateTimeField = sal_False;
        try
        {
            sal_Int32 nFieldType;
            m_xField->getPropertyValue(PROPERTY_FIELDTYPE) >>= nFieldType;
            m_bDateTimeField = (nFieldType == starsdbc::DataType::TIMESTAMP);
        }
        catch(...)
        {
        }
    }
}

// starform::XBoundComponent
//------------------------------------------------------------------------------
sal_Bool ODateModel::_commit()
{
    staruno::Any aNewValue = m_xAggregateFastSet->getFastPropertyValue( ODateModel::nDateHandle );
    if (!compare(aNewValue, m_aSaveValue))
    {
        if (!aNewValue.hasValue())
            m_xColumnUpdate->updateNull();
        else
        {
            try
            {
                starutil::Date aDate;
                if (!(aNewValue >>= aDate))
                {
                    sal_Int32 nAsInt(0);
                    aNewValue >>= nAsInt;
                    Date aToolsValue(nAsInt);
                    typeConvert(aToolsValue, aDate);
                }

                if (!m_bDateTimeField)
                    m_xColumnUpdate->updateDate(aDate);
                else
                {
                    starutil::DateTime aDateTime = m_xColumn->getTimestamp();
                    aDateTime.Day = aDate.Day;
                    aDateTime.Month = aDate.Month;
                    aDateTime.Year = aDate.Year;
                    m_xColumnUpdate->updateTimestamp(aDateTime);
                }
            }
            catch(...)
            {
                return sal_False;
            }
        }
        m_aSaveValue = aNewValue;
    }
    return sal_True;
}

//------------------------------------------------------------------------------
void ODateModel::_onValueChanged()
{
    starutil::Date aDate = m_xColumn->getDate();
    if (m_xColumn->wasNull())
        m_aSaveValue.clear();
    else
        // the aggregated set expects an Int32 as value ...
        m_aSaveValue <<= DBTypeConversion::toINT32(aDate);

    {   // release our mutex once (it's acquired in the calling method !), as setting aggregate properties
        // may cause any uno controls belonging to us to lock the solar mutex, which is potentially dangerous with
        // our own mutex locked
        // FS - 72451 - 31.01.00
        MutexRelease aRelease(m_aMutex);
        m_xAggregateFastSet->setFastPropertyValue(ODateModel::nDateHandle, m_aSaveValue);
    }
}

//------------------------------------------------------------------------------
void ODateModel::_reset()
{
    staruno::Any aValue;
    if (m_aDefault.getValueType().getTypeClass() == staruno::TypeClass_LONG)
        aValue = m_aDefault;
    else
    {   // aktuelles Datum einstellen
        Date aCurrentDate;
        aValue <<= DBTypeConversion::toINT32(aCurrentDate);
    }

    {   // release our mutex once (it's acquired in the calling method !), as setting aggregate properties
        // may cause any uno controls belonging to us to lock the solar mutex, which is potentially dangerous with
        // our own mutex locked
        // FS - 72451 - 31.01.00
        MutexRelease aRelease(m_aMutex);
        m_xAggregateFastSet->setFastPropertyValue(ODateModel::nDateHandle, aValue);
    }
}

//.........................................................................
}   // namespace frm
//.........................................................................

