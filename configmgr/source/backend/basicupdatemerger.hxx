/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: basicupdatemerger.hxx,v $
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

#ifndef CONFIGMGR_BACKEND_BASICUPDATEMERGER_HXX
#define CONFIGMGR_BACKEND_BASICUPDATEMERGER_HXX

#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/configuration/backend/XLayerHandler.hpp>
#include <com/sun/star/configuration/backend/XLayer.hpp>

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif
// -----------------------------------------------------------------------------

namespace configmgr
{
// -----------------------------------------------------------------------------
    namespace backend
    {
// -----------------------------------------------------------------------------
        using rtl::OUString;
        namespace uno       = ::com::sun::star::uno;
        namespace lang      = ::com::sun::star::lang;
        namespace backenduno = ::com::sun::star::configuration::backend;
        using backenduno::MalformedDataException;
        using backenduno::TemplateIdentifier;
// -----------------------------------------------------------------------------

        class BasicUpdateMerger : public cppu::WeakImplHelper2< backenduno::XLayerHandler, backenduno::XLayer >
        {
        public:
            typedef uno::Reference< backenduno::XLayerHandler > ResultHandler;
            typedef uno::Reference< backenduno::XLayer >        LayerSource;

            explicit
            BasicUpdateMerger(LayerSource const & _xSourceLayer);
            ~BasicUpdateMerger();

        // XLayerHandler
        protected:
            virtual void SAL_CALL
                startLayer(  )
                    throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException);

            virtual void SAL_CALL
                endLayer(  )
                    throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException);

            virtual void SAL_CALL
                overrideNode( const OUString& aName, sal_Int16 aAttributes, sal_Bool bClear )
                    throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException);

            virtual void SAL_CALL
                addOrReplaceNode( const OUString& aName, sal_Int16 aAttributes )
                    throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException);

            virtual void SAL_CALL
                addOrReplaceNodeFromTemplate( const OUString& aName, const TemplateIdentifier& aTemplate, sal_Int16 aAttributes )
                    throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException);

            virtual void SAL_CALL
                endNode(  )
                    throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException);

            virtual void SAL_CALL
                dropNode( const OUString& aName )
                    throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException);

            virtual void SAL_CALL
                overrideProperty( const OUString& aName, sal_Int16 aAttributes, const uno::Type& aType, sal_Bool bClear )
                    throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException);

            virtual void SAL_CALL
                endProperty(  )
                    throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException);

            virtual void SAL_CALL
                setPropertyValue( const uno::Any& aValue )
                    throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException);

            virtual void SAL_CALL
                setPropertyValueForLocale( const uno::Any& aValue, const OUString & aLocale )
                    throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException);

            virtual void SAL_CALL
                addProperty( const OUString& aName, sal_Int16 aAttributes, const uno::Type& aType )
                    throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException);

            virtual void SAL_CALL
                addPropertyWithValue( const OUString& aName, sal_Int16 aAttributes, const uno::Any& aValue )
                    throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException);

        // XLayer
        protected:
            virtual void SAL_CALL
                readData( const uno::Reference< backenduno::XLayerHandler >& aHandler )
                    throw (MalformedDataException, lang::NullPointerException, lang::WrappedTargetException, uno::RuntimeException);

        // new overrideable
        private:
            /// write the whole update to the output
            virtual void flushUpdate() = 0;

        protected:
            bool isHandling() const { return m_nNesting != 0 || !m_aSearchPath.empty(); }
            bool isSkipping() const { return m_bSkipping; }
            void startSkipping();

            typedef std::vector<OUString> ContextPath;
            void findContext(ContextPath const & _aContext);
            void leaveContext();

            ResultHandler getResultWriter() const { return m_xResultHandler; };

            void raiseMalformedDataException(sal_Char const * pMsg);
        private:
            void pushLevel(OUString const & _aLevel);
            void popLevel();

            void flushContext();
        private:
            LayerSource     m_xSourceLayer;
            ResultHandler   m_xResultHandler;
            ContextPath     m_aSearchPath;

            sal_Int16   m_nNesting;
            bool        m_bSkipping;
        };
// -----------------------------------------------------------------------------
    } // namespace xml
// -----------------------------------------------------------------------------

} // namespace configmgr
#endif




