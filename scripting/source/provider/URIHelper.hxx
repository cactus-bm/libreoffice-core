/*************************************************************************
 *
 *  $RCSfile: URIHelper.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2005-02-11 16:34:38 $
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

#ifndef _FRAMEWORK_SCRIPT_PROVIDER_XSCRIPTURIHELPER_HXX_
#define _FRAMEWORK_SCRIPT_PROVIDER_XSCRIPTURIHELPER_HXX_

#include <com/sun/star/script/provider/XScriptURIHelper.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess.hpp>
#include <com/sun/star/uri/XUriReferenceFactory.hpp>

#include <rtl/ustring>
#include <cppuhelper/implbase3.hxx>

namespace func_provider
{

#define css ::com::sun::star

class ScriptingFrameworkURIHelper :
    public ::cppu::WeakImplHelper3<
        css::script::provider::XScriptURIHelper,
        css::lang::XServiceInfo,
        css::lang::XInitialization >
{
private:

    css::uno::Reference< css::ucb::XSimpleFileAccess > m_xSimpleFileAccess;
    css::uno::Reference<css::uri::XUriReferenceFactory> m_xUriReferenceFactory;

    ::rtl::OUString m_sLanguage;
    ::rtl::OUString m_sLocation;
    ::rtl::OUString m_sBaseURI;

    ::rtl::OUString SCRIPTS_PART;

    bool initBaseURI();
    ::rtl::OUString getLanguagePart(const ::rtl::OUString& rStorageURI);
    ::rtl::OUString getLanguagePath(const ::rtl::OUString& rLanguagePart);

public:

    ScriptingFrameworkURIHelper(
        const css::uno::Reference< css::uno::XComponentContext >& xContext )
            throw( css::uno::RuntimeException );

    ~ScriptingFrameworkURIHelper();

    virtual void SAL_CALL
        initialize( const css::uno::Sequence < css::uno::Any > & args )
            throw ( css::uno::Exception, css::uno::RuntimeException);

    virtual ::rtl::OUString SAL_CALL
        getRootStorageURI()
            throw ( ::com::sun::star::uno::RuntimeException );

    virtual ::rtl::OUString SAL_CALL
        getScriptURI( const ::rtl::OUString& rStorageURI )
            throw( css::lang::IllegalArgumentException,
                   css::uno::RuntimeException );

    virtual ::rtl::OUString SAL_CALL
        getStorageURI( const ::rtl::OUString& rScriptURI )
            throw( css::lang::IllegalArgumentException,
                   css::uno::RuntimeException );

    virtual ::rtl::OUString SAL_CALL
        getImplementationName()
            throw( css::uno::RuntimeException );

    virtual sal_Bool SAL_CALL
        supportsService( const ::rtl::OUString& ServiceName )
            throw( css::uno::RuntimeException );

    virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL
        getSupportedServiceNames()
            throw( css::uno::RuntimeException );
};

} // namespace func_provider
#endif //_FRAMEWORK_SCRIPT_PROVIDER_XSCRIPTURIHELPER_HXX_
