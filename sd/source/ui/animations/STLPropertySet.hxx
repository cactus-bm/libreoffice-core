/*************************************************************************
 *
 *  $RCSfile: STLPropertySet.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-11-26 19:56:16 $
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

#ifndef _SD_STLPROPERTYSET_HXX
#define _SD_STLPROPERTYSET_HXX

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

#ifndef _UTL_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif

#include <list>
#include <map>

namespace sd
{

const sal_Int32 STLPropertyState_DEFAULT = 0;
const sal_Int32 STLPropertyState_DIRECT = 1;
const sal_Int32 STLPropertyState_AMBIGUOUS = 3;

struct STLPropertyMapEntry
{
    ::com::sun::star::uno::Any maValue;
    sal_Int32 mnState;

    STLPropertyMapEntry()
        : mnState( STLPropertyState_AMBIGUOUS ) {}
    STLPropertyMapEntry( ::com::sun::star::uno::Any aValue, sal_Int32 nState = STLPropertyState_DEFAULT )
        : maValue( aValue ), mnState( nState ) {}

};

typedef std::map<sal_Int32, STLPropertyMapEntry > PropertyMap;
typedef PropertyMap::iterator PropertyMapIter;
typedef PropertyMap::const_iterator PropertyMapConstIter;

class STLPropertySet
{
public:
    STLPropertySet();
    ~STLPropertySet();

    void setPropertyDefaultValue( sal_Int32 nHandle, const com::sun::star::uno::Any& rValue );
    void setPropertyValue( sal_Int32 nHandle, const com::sun::star::uno::Any& rValue, sal_Int32 nState = STLPropertyState_DIRECT );
    ::com::sun::star::uno::Any getPropertyValue( sal_Int32 nHandle ) const;

    sal_Int32 getPropertyState( sal_Int32 nHandle ) const;
    void setPropertyState( sal_Int32 nHandle, sal_Int32 nState );

private:
    bool findProperty( sal_Int32 nHandle, PropertyMapIter& rIter );
    bool findProperty( sal_Int32 nHandle, PropertyMapConstIter& rIter ) const;

private:
    PropertyMap maPropertyMap;
};

};

#endif // _SD_STLPROPERTYSET_HXX
