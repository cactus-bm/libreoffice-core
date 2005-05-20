/*************************************************************************
 *
 *  $RCSfile: binarycache.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-05-20 15:42:09 $
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
#ifndef CONFIGMGR_BINARYCACHE_HXX
#define CONFIGMGR_BINARYCACHE_HXX

#ifndef CONFIGMGR_MATCHLOCALE_HXX
#include "matchlocale.hxx"
#endif

#include <rtl/ustring.hxx>

#ifndef _COM_SUN_STAR_UNO_XCOMPONENTCONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XLAYER_HPP_
#include <com/sun/star/configuration/backend/XLayer.hpp>
#endif // _COM_SUN_STAR_CONFIGURATION_BACKEND_XLAYER_HPP_

namespace configmgr
{
    // -----------------------------------------------------------------------------
    namespace backend
    {
        namespace css = com::sun::star ;
        namespace uno  = css::uno ;
        namespace lang = css::lang ;
        namespace backenduno = css::configuration::backend  ;

        // -----------------------------------------------------------------------------
        class MergedComponentData;

        typedef uno::Reference< lang::XMultiServiceFactory > MultiServiceFactory;
        // -----------------------------------------------------------------------------

        class BinaryCache
        {
        public:
            explicit
            BinaryCache(const uno::Reference<uno::XComponentContext>& xContext);

            void setOwnerEntity(rtl::OUString const & aOwnerEntity);
            void disableCache();

            bool isCacheEnabled(rtl::OUString const & aEntity) const;

            rtl::OUString getCacheFileURL(rtl::OUString const & aComponent) const;

            bool readComponentData(MergedComponentData & aComponentData,
                                    MultiServiceFactory const & aFactory,
                                    rtl::OUString const & aComponent,
                                    rtl::OUString const & aSchemaVersion,
                                    rtl::OUString const & aEntity,
                                    localehelper::Locale const & aRequestedLocale,
                                    localehelper::LocaleSequence & outKnownLocales,
                                    const uno::Reference<backenduno::XLayer> * pLayers,
                                    sal_Int32 nNumLayers,
                                    bool bIncludeTemplates = true);

            bool writeComponentData(MergedComponentData const & aComponentData,
                                    MultiServiceFactory const & aFactory,
                                    rtl::OUString const & aComponent,
                                    rtl::OUString const & aSchemaVersion,
                                    rtl::OUString const & aEntity,
                                    localehelper::LocaleSequence const & aKnownLocales,
                                    const uno::Reference<backenduno::XLayer> * pLayers,
                                    sal_Int32 nNumLayers);
        private:
            rtl::OUString mBaseURL;
            rtl::OUString mOwnerEntity;
            bool mbCacheEnabled;
        };

    // -----------------------------------------------------------------------------------
    // helper for cache reader implementations - see backendaccess.cxx for implementation
        uno::Sequence< rtl::OUString >
            getAvailableLocales(const uno::Reference<backenduno::XLayer> * pLayers, sal_Int32 nNumLayers);
    // -----------------------------------------------------------------------------------
    }
// -----------------------------------------------------------------------------------
}

#endif //

