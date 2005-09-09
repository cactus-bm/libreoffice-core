/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: basmethnode.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:22:42 $
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

#ifndef SCRIPTING_BASMETHNODE_HXX
#define SCRIPTING_BASMETHNODE_HXX

#ifndef SCRIPTING_BCHOLDER_HXX
#include "bcholder.hxx"
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XINVOCATION_HPP_
#include <com/sun/star/script/XInvocation.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_BROWSE_XBROWSENODE_HPP_
#include <com/sun/star/script/browse/XBrowseNode.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_XCOMPONENTCONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif

#ifndef _COMPHELPER_PROPERTY_ARRAY_HELPER_HXX_
#include <comphelper/proparrhlp.hxx>
#endif
#ifndef _COMPHELPER_PROPERTYCONTAINER_HXX_
#include <comphelper/propertycontainer.hxx>
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif


class SbMethod;


//.........................................................................
namespace basprov
{
//.........................................................................

    //  ----------------------------------------------------
    //  class BasicMethodNodeImpl
    //  ----------------------------------------------------

    typedef ::cppu::WeakImplHelper2<
        ::com::sun::star::script::browse::XBrowseNode,
        ::com::sun::star::script::XInvocation > BasicMethodNodeImpl_BASE;

    class BasicMethodNodeImpl : public BasicMethodNodeImpl_BASE,
                                public ::scripting_helper::OMutexHolder,
                                public ::scripting_helper::OBroadcastHelperHolder,
                                public ::comphelper::OPropertyContainer,
                                public ::comphelper::OPropertyArrayUsageHelper< BasicMethodNodeImpl >
    {
    private:
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >    m_xContext;
    ::rtl::OUString m_sScriptingContext;
        SbMethod* m_pMethod;
        bool m_bIsAppScript;

        // properties
        ::rtl::OUString m_sURI;
        sal_Bool m_bEditable;

    protected:
        // OPropertySetHelper
        virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper(  );

        // OPropertyArrayUsageHelper
        virtual ::cppu::IPropertyArrayHelper* createArrayHelper(  ) const;

    public:
        BasicMethodNodeImpl( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext,
            const ::rtl::OUString& sScriptingContext,
            SbMethod* pMethod, bool isAppScript = true );
        virtual ~BasicMethodNodeImpl();

        // XInterface
        DECLARE_XINTERFACE()

        // XTypeProvider
        DECLARE_XTYPEPROVIDER()

        // XBrowseNode
        virtual ::rtl::OUString SAL_CALL getName(  )
            throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::script::browse::XBrowseNode > > SAL_CALL getChildNodes(  )
            throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL hasChildNodes(  )
            throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Int16 SAL_CALL getType(  )
            throw (::com::sun::star::uno::RuntimeException);

        // XPropertySet
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  )
            throw (::com::sun::star::uno::RuntimeException);

        // XInvocation
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XIntrospectionAccess > SAL_CALL getIntrospection(  )
            throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Any SAL_CALL invoke(
            const ::rtl::OUString& aFunctionName,
            const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aParams,
            ::com::sun::star::uno::Sequence< sal_Int16 >& aOutParamIndex,
            ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aOutParam )
            throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::script::CannotConvertException,
                   ::com::sun::star::reflection::InvocationTargetException, ::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue )
            throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::script::CannotConvertException,
                   ::com::sun::star::reflection::InvocationTargetException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Any SAL_CALL getValue( const ::rtl::OUString& aPropertyName )
            throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL hasMethod( const ::rtl::OUString& aName )
            throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL hasProperty( const ::rtl::OUString& aName )
            throw (::com::sun::star::uno::RuntimeException);
    };

//.........................................................................
}   // namespace basprov
//.........................................................................

#endif // SCRIPTING_BASMETHNODE_HXX
