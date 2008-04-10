/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ConfigurationAccess.cxx,v $
 * $Revision: 1.6 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_chart2.hxx"

#include "ConfigurationAccess.hxx"
#include "macros.hxx"

// header for class SvtSysLocale
#include <svtools/syslocale.hxx>
// header for class ConfigItem
#include <unotools/configitem.hxx>


//.............................................................................
namespace chart
{
//.............................................................................
using namespace ::com::sun::star;

namespace
{
bool lcl_IsMetric()
{
    SvtSysLocale aSysLocale;
    const LocaleDataWrapper* pLocWrapper = aSysLocale.GetLocaleDataPtr();
    MeasurementSystem eSys = pLocWrapper->getMeasurementSystemEnum();

    return ( eSys == MEASURE_METRIC );
}
}//end anonymous namespace

// ----------------------------------------

class CalcConfigItem : public ::utl::ConfigItem
{
public:
    CalcConfigItem();
    virtual ~CalcConfigItem();

    FieldUnit getFieldUnit();
};

CalcConfigItem::CalcConfigItem()
    : ConfigItem( ::rtl::OUString( C2U( "Office.Calc/Layout" )))
{
}

CalcConfigItem::~CalcConfigItem()
{
}

FieldUnit CalcConfigItem::getFieldUnit()
{
    FieldUnit eResult( FUNIT_CM );

    uno::Sequence< ::rtl::OUString > aNames( 1 );
    if( lcl_IsMetric() )
        aNames[ 0 ] = ::rtl::OUString( C2U( "Other/MeasureUnit/Metric" ));
    else
        aNames[ 0 ] = ::rtl::OUString( C2U( "Other/MeasureUnit/NonMetric" ));

    uno::Sequence< uno::Any > aResult( GetProperties( aNames ));
    sal_Int32 nValue = 0;
    if( aResult[ 0 ] >>= nValue )
        eResult = static_cast< FieldUnit >( nValue );

    return eResult;
}

// ----------------------------------------

ConfigurationAccess * ConfigurationAccess::m_pThis = 0;

// private, use static getConfigurationAccess method
ConfigurationAccess::ConfigurationAccess()
    : m_pCalcConfigItem(0)
{
    m_pCalcConfigItem = new CalcConfigItem();
}

// static
ConfigurationAccess * ConfigurationAccess::getConfigurationAccess()
{
    // note: not threadsafe
    if( !m_pThis )
        m_pThis = new ConfigurationAccess();
    return m_pThis;
}

ConfigurationAccess::~ConfigurationAccess()
{
    delete m_pCalcConfigItem;
}

FieldUnit ConfigurationAccess::getFieldUnit()
{
    OSL_ASSERT( m_pCalcConfigItem );
    FieldUnit aUnit( m_pCalcConfigItem->getFieldUnit() );
    return aUnit;
}

//.............................................................................
} //namespace chart
//.............................................................................
