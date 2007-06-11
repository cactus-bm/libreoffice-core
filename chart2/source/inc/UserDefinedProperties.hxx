/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: UserDefinedProperties.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2007-06-11 14:59:27 $
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
#ifndef CHART_USERDEFINEDPROPERTIES_HXX
#define CHART_USERDEFINEDPROPERTIES_HXX

#include "PropertyHelper.hxx"
#include "FastPropertyIdRanges.hxx"

#ifndef _COM_SUN_STAR_BEANS_PROPERTY_HPP_
#include <com/sun/star/beans/Property.hpp>
#endif

#include <vector>

namespace chart
{

// implements service com.sun.star.xml.UserDefinedAttributeSupplier
class UserDefinedProperties
{
public:
    // FastProperty Ids for properties
    enum
    {
        // UserDefined attributes in the chart-properties
        PROP_XML_USERDEF_CHART = FAST_PROPERTY_ID_START_USERDEF_PROP,
        PROP_XML_USERDEF_TEXT,
        PROP_XML_USERDEF_PARA,
        // com.sun.star.xml.UserDefinedAttributeSupplier
        // UserDefined attributes in the other properties-sections. (as long as
        // there is no reliable mechanism, properties may move between the
        // sections)
        PROP_XML_USERDEF
    };

    static void AddPropertiesToVector(
        ::std::vector< ::com::sun::star::beans::Property > & rOutProperties );

private:
    // not implemented
    UserDefinedProperties();
};

} //  namespace chart

// CHART_USERDEFINEDPROPERTIES_HXX
#endif
