/*************************************************************************
 *
 *  $RCSfile: MasterPropertySetInfo.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2005-02-16 15:50:24 $
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
 *  Contributor(s): Martin Gallwey ( gallwey@sun.com )
 *
 *
 ************************************************************************/

#ifndef _COMPHELPER_MASTERPROPERTYSETINFO_HXX_
#define _COMPHELPER_MASTERPROPERTYSETINFO_HXX_
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSETINFO_HPP_
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#endif
#ifndef _COMPHELPER_PROPERTYINFOHASH_HXX_
#include <comphelper/PropertyInfoHash.hxx>
#endif
#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif
#ifndef _COMPHELPER_TYPEGENERATION_HXX_
#include <comphelper/TypeGeneration.hxx>
#endif
#ifndef INCLUDED_COMPHELPERDLLAPI_H
#include "comphelper/comphelperdllapi.h"
#endif

namespace comphelper
{
    class COMPHELPER_DLLPUBLIC MasterPropertySetInfo : public ::com::sun::star::beans::XPropertySetInfo,
                                  public ::cppu::OWeakObject
    {
        friend class MasterPropertySet;
    protected:
        PropertyDataHash maMap;
        com::sun::star::uno::Sequence < com::sun::star::beans::Property > maProperties;
    public:
        MasterPropertySetInfo()
            throw();
        MasterPropertySetInfo( PropertyInfo * pMap )
            throw();
        virtual ~MasterPropertySetInfo()
            throw();
        void add( PropertyInfo* pMap, sal_Int32 nCount = -1, sal_uInt8 nMapId = 0 )
            throw();
        void add( PropertyInfoHash &rHash, sal_uInt8 nMapId )
            throw();
        void remove( const rtl::OUString& aName )
            throw();

        // XInterface
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& rType )
            throw(::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL acquire(  )
            throw();
        virtual void SAL_CALL release(  )
            throw();

        // XPropertySetInfo
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property > SAL_CALL getProperties()
            throw(::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::beans::Property SAL_CALL getPropertyByName( const ::rtl::OUString& aName )
            throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL hasPropertyByName( const ::rtl::OUString& Name )
            throw(::com::sun::star::uno::RuntimeException);
    };
}
#endif
