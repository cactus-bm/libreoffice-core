/*************************************************************************
 *
 *  $RCSfile: IdPropArrayHelper.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: hjs $ $Date: 2004-06-25 17:20:02 $
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
 *  Source License Version 1.1 (the License); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an AS IS basis,
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
#ifndef COMPHELPER_IDPROPERTYARRAYUSAGEHELPER_HXX
#define COMPHELPER_IDPROPERTYARRAYUSAGEHELPER_HXX

#ifndef _CPPUHELPER_COMPONENT_HXX_
#include <cppuhelper/component.hxx>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef _CPPUHELPER_INTERFACECONTAINER_HXX_
#include <cppuhelper/interfacecontainer.hxx>
#endif
#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif
#ifndef INCLUDED_RTL_INSTANCE_HXX
#include <rtl/instance.hxx>
#endif
#include <cppuhelper/propshlp.hxx>

namespace comphelper
{
    //************************************************************
    //  OIdPropertyArrayUsageHelper
    //************************************************************
    namespace cppu { class IPropertyArrayHelper; }
    template <typename TYPE> struct OIdPropertyArrayUsageHelperMutex
            : public rtl::Static< ::osl::Mutex, OIdPropertyArrayUsageHelperMutex<TYPE> > {};

    typedef std::map< sal_Int32, ::cppu::IPropertyArrayHelper*, std::less< sal_Int32 > > OIdPropertyArrayMap;
    template <class TYPE>
    class OIdPropertyArrayUsageHelper
    {
    protected:
        static sal_Int32                        s_nRefCount;
        static OIdPropertyArrayMap*             s_pMap;

    public:
        OIdPropertyArrayUsageHelper();
        virtual ~OIdPropertyArrayUsageHelper()
        {
            ::osl::MutexGuard aGuard(OIdPropertyArrayUsageHelperMutex<TYPE>::get());
            OSL_ENSURE(s_nRefCount > 0, "OIdPropertyArrayUsageHelper::~OIdPropertyArrayUsageHelper : suspicious call : have a refcount of 0 !");
            if (!--s_nRefCount)
            {
                // delete the element
                for (OIdPropertyArrayMap::iterator i = s_pMap->begin(); i != s_pMap->end(); ++i)
                    delete (*i).second;
                delete s_pMap;
                s_pMap = NULL;
            }
        }

        /** call this in the getInfoHelper method of your derived class. The method returns the array helper of the
            class, which is created if neccessary.
        */
        ::cppu::IPropertyArrayHelper* getArrayHelper(sal_Int32 nId);

    protected:
        /** used to implement the creation of the array helper which is shared amongst all instances of the class.
            This method needs to be implemented in derived classes.
            <BR>
            The method gets called with Mutex acquired.
            <BR>
            as long as IPropertyArrayHelper has no virtual destructor, the implementation of ~OPropertyArrayUsageHelper
            assumes that you created an ::cppu::OPropertyArrayHelper when deleting s_pProps.
            @return                         an pointer to the newly created array helper. Must not be NULL.
        */
        virtual ::cppu::IPropertyArrayHelper* createArrayHelper(sal_Int32 nId) const = 0;
    };

    //------------------------------------------------------------------
    template<class TYPE>
    sal_Int32                       OIdPropertyArrayUsageHelper< TYPE >::s_nRefCount    = 0;

    template<class TYPE>
    OIdPropertyArrayMap*            OIdPropertyArrayUsageHelper< TYPE >::s_pMap = NULL;

    //------------------------------------------------------------------
    template <class TYPE>
    OIdPropertyArrayUsageHelper<TYPE>::OIdPropertyArrayUsageHelper()
    {
        ::osl::MutexGuard aGuard(OIdPropertyArrayUsageHelperMutex<TYPE>::get());
        // create the map if necessary
        if (s_pMap == NULL)
            s_pMap = new OIdPropertyArrayMap();
        ++s_nRefCount;
    }

    //------------------------------------------------------------------
    template <class TYPE>
    ::cppu::IPropertyArrayHelper* OIdPropertyArrayUsageHelper<TYPE>::getArrayHelper(sal_Int32 nId)
    {
        OSL_ENSURE(s_nRefCount, "OIdPropertyArrayUsageHelper::getArrayHelper : suspicious call : have a refcount of 0 !");
        ::osl::MutexGuard aGuard(OIdPropertyArrayUsageHelperMutex<TYPE>::get());
        // do we have the array already?
        if (! (*s_pMap)[nId] )
        {
            (*s_pMap)[nId] = createArrayHelper(nId);
            OSL_ENSURE((*s_pMap)[nId], "OIdPropertyArrayUsageHelper::getArrayHelper : createArrayHelper returned nonsense !");
        }
        return (*s_pMap)[nId];
    }
}
#endif // COMPHELPER_IDPROPERTYARRAYUSAGEHELPER_HXX
