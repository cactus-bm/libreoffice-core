/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: providerfactory.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 04:11:37 $
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

#ifndef _CONFIGMGR_PROVIDER_FACTORY_HXX_
#define _CONFIGMGR_PROVIDER_FACTORY_HXX_

#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_XCOMPONENTCONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSINGLECOMPONENTFACTORY_HPP_
#include <com/sun/star/lang/XSingleComponentFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XEVENTLISTENER_HPP_
#include <com/sun/star/lang/XEventListener.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_NAMEDVALUE_HPP_
#include <com/sun/star/beans/NamedValue.hpp>
#endif
/*
#ifndef _COM_SUN_STAR_LANG_ILLEGALARGUMENTEXCEPTION_HPP_
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif
*/
//------------------------------------------------------------------------
namespace configmgr
{
    //------------------------------------------------------------------------
    namespace uno   = ::com::sun::star::uno;
    namespace lang  = ::com::sun::star::lang;
    namespace beans = ::com::sun::star::beans;
    using rtl::OUString;
    //------------------------------------------------------------------------
    class ContextReader;
    class ArgumentHelper;
    //------------------------------------------------------------------------
    //= OProviderFactory
    //------------------------------------------------------------------------
    typedef ::cppu::WeakImplHelper1< lang::XSingleComponentFactory > ProviderFactory_Base;

    /** a special factory for the configuration provider,
        which maps creation arguments into a context.
    */
    class ProviderFactory : public ProviderFactory_Base
    {
        OUString const m_aImplementationName;
        bool m_bAdmin;

    public:
        typedef uno::Reference< uno::XComponentContext > Context;
        typedef uno::Sequence < uno::Any >          Arguments;
        typedef uno::Sequence < beans::NamedValue > NamedValues;
    public:
        explicit
        ProviderFactory(OUString const & aImplementationName, bool bAdmin);
        ~ProviderFactory();

        virtual uno::Reference< uno::XInterface >
            SAL_CALL createInstanceWithContext(Context const & xContext )
                throw (uno::Exception, uno::RuntimeException);

        virtual uno::Reference< uno::XInterface > SAL_CALL
            createInstanceWithArgumentsAndContext( Arguments const & aArguments, Context const & xContext )
                throw (uno::Exception, uno::RuntimeException);

    private:
        uno::Reference< uno::XInterface > getProviderFromContext(Context const & aContext);
        uno::Reference< uno::XInterface > getProviderAlways(Context const & xContext);
        uno::Reference< uno::XInterface > createProviderWithArguments(Context const & xContext, Arguments const & _aArguments);
        uno::Reference< uno::XInterface > createProvider(Context const & xContext,bool bAdmin);
        uno::Reference< uno::XInterface > createProvider(Context const & xContext);
        sal_Int32 parseArguments(ArgumentHelper & aParser, NamedValues & rValues, Arguments const & _aArguments);
    };
//------------------------------------------------------------------------
}   // namespace configmgr
//------------------------------------------------------------------------

#endif // _CONFIGMGR_PROVIDER_FACTORY_HXX_

