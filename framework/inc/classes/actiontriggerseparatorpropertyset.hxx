/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: actiontriggerseparatorpropertyset.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:01:35 $
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

#ifndef __FRAMEWORK_CLASSES_ACTIONTRIGGERSEPARATORPROPERTYSET_HXX_
#define __FRAMEWORK_CLASSES_ACTIONTRIGGERSEPARATORPROPERTYSET_HXX_

#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
#include <threadhelp/threadhelpbase.hxx>
#endif

#ifndef _CPPUHELPER_PROPSHLP_HXX
#include <cppuhelper/propshlp.hxx>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XTYPEPROVIDER_HPP_
#include <com/sun/star/lang/XTypeProvider.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_ILLEGALARGUMENTEXCEPTION_HPP_
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#endif

#define SERVICENAME_ACTIONTRIGGERSEPARATOR          "com.sun.star.ui.ActionTriggerSeparator"
#define IMPLEMENTATIONNAME_ACTIONTRIGGERSEPARATOR   "com.sun.star.comp.ui.ActionTriggerSeparator"

namespace framework
{

class ActionTriggerSeparatorPropertySet :   public ThreadHelpBase                       ,   // Struct for right initalization of mutex member! Must be first of baseclasses.
                                            public ::com::sun::star::lang::XServiceInfo ,
                                            public ::com::sun::star::lang::XTypeProvider,
                                            public ::cppu::OBroadcastHelper             ,
                                            public ::cppu::OPropertySetHelper           ,   // -> XPropertySet, XFastPropertySet, XMultiPropertySet
                                            public ::cppu::OWeakObject
{
    public:
        ActionTriggerSeparatorPropertySet( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager );
        virtual ~ActionTriggerSeparatorPropertySet();

        // XInterface
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType )
            throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL acquire() throw ();
        virtual void SAL_CALL release() throw ();

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw (::com::sun::star::uno::RuntimeException);

        // XTypeProvider
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  ) throw (::com::sun::star::uno::RuntimeException);

    private:
        //---------------------------------------------------------------------------------------------------------
        //  OPropertySetHelper
        //---------------------------------------------------------------------------------------------------------
        virtual sal_Bool SAL_CALL convertFastPropertyValue( com::sun::star::uno::Any&       aConvertedValue,
                                                            com::sun::star::uno::Any&       aOldValue,
                                                            sal_Int32                       nHandle,
                                                            const com::sun::star::uno::Any& aValue          )
            throw( com::sun::star::lang::IllegalArgumentException );


        virtual void SAL_CALL setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const com::sun::star::uno::Any& aValue )
            throw( com::sun::star::uno::Exception );

        virtual void SAL_CALL getFastPropertyValue( com::sun::star::uno::Any& aValue, sal_Int32 nHandle ) const;

        virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();

        virtual com::sun::star::uno::Reference< com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo()
            throw (::com::sun::star::uno::RuntimeException);

        static const com::sun::star::uno::Sequence< com::sun::star::beans::Property > impl_getStaticPropertyDescriptor();

        //---------------------------------------------------------------------------------------------------------
        //  helper
        //---------------------------------------------------------------------------------------------------------

        sal_Bool impl_tryToChangeProperty(  sal_Int16                           aCurrentValue   ,
                                            const   com::sun::star::uno::Any&   aNewValue       ,
                                            com::sun::star::uno::Any&           aOldValue       ,
                                            com::sun::star::uno::Any&           aConvertedValue ) throw( com::sun::star::lang::IllegalArgumentException );

        //---------------------------------------------------------------------------------------------------------
        //  members
        //---------------------------------------------------------------------------------------------------------

        sal_Int16   m_nSeparatorType;
};

}

#endif // __FRAMEWORK_CLASSES_ACTIONTRIGGERSEPARATORPROPERTYSET_HXX_
