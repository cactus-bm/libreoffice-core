/*************************************************************************
 *
 *  $RCSfile: layermerge.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jb $ $Date: 2002-05-16 10:56:07 $
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

#ifndef CONFIGMGR_BACKEND_LAYERMERGE_HXX
#define CONFIGMGR_BACKEND_LAYERMERGE_HXX

#ifndef CONFIGMGR_BACKEND_MERGEDCOMPONENTDATA_HXX
#include "mergedcomponentdata.hxx"
#endif
#ifndef CONFIGMGR_BACKEND_COMPONENTDATAHELPER_HXX
#include "componentdatahelper.hxx"
#endif

#include <drafts/com/sun/star/configuration/backend/XLayerHandler.hpp>

#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif

namespace configmgr
{
// -----------------------------------------------------------------------------
    class OTreeNodeFactory;
// -----------------------------------------------------------------------------
    namespace backend
    {
// -----------------------------------------------------------------------------
        namespace uno       = ::com::sun::star::uno;
        namespace lang      = ::com::sun::star::lang;
        namespace beans     = ::com::sun::star::beans;
        namespace container = ::com::sun::star::container;

        namespace backenduno = ::drafts::com::sun::star::configuration::backend;

// -----------------------------------------------------------------------------

        typedef ::cppu::WeakImplHelper1<backenduno::XLayerHandler> LayerMergeHandler_Base;

        class LayerMergeHandler
        :   public LayerMergeHandler_Base
        {
        public:
            explicit
            LayerMergeHandler(MergedComponentData & _rData, OUString const & _aLocale);
            virtual ~LayerMergeHandler();

        // checking the result
            bool isDone() const { return m_aContext.isDone(); }

            MergedComponentData &       result();
            MergedComponentData const & result() const;

        // XUpdateHandler
        public:
            virtual void SAL_CALL
                startLayer(  )
                    throw (MalformedDataException, uno::RuntimeException);

            virtual void SAL_CALL
                endLayer(  )
                    throw (MalformedDataException, lang::IllegalAccessException, uno::RuntimeException);

            virtual void SAL_CALL
                overrideNode( const OUString& aName, sal_Int16 aAttributes )
                    throw (MalformedDataException, container::NoSuchElementException, lang::IllegalAccessException, lang::IllegalArgumentException, uno::RuntimeException);

            virtual void SAL_CALL
                addOrReplaceNode( const OUString& aName, sal_Int16 aAttributes )
                    throw (MalformedDataException, container::NoSuchElementException, lang::IllegalAccessException, lang::IllegalArgumentException, uno::RuntimeException);

            virtual void SAL_CALL
                addOrReplaceNodeFromTemplate( const OUString& aName, const TemplateIdentifier& aTemplate, sal_Int16 aAttributes )
                    throw (MalformedDataException, container::NoSuchElementException, beans::IllegalTypeException, lang::IllegalAccessException, lang::IllegalArgumentException, uno::RuntimeException);

            virtual void SAL_CALL
                endNode(  )
                    throw (MalformedDataException, uno::RuntimeException);

            virtual void SAL_CALL
                dropNode( const OUString& aName )
                    throw (MalformedDataException, container::NoSuchElementException, lang::IllegalAccessException, lang::IllegalArgumentException, uno::RuntimeException);

            virtual void SAL_CALL
                overridePropertyValue( const OUString& aName, sal_Int16 aAttributes, const uno::Any& aValue )
                    throw (MalformedDataException, beans::UnknownPropertyException, beans::IllegalTypeException, lang::IllegalAccessException, lang::IllegalArgumentException, uno::RuntimeException);

            virtual void SAL_CALL
                overridePropertyAttributes( const OUString& aName, sal_Int16 aAttributes )
                    throw (MalformedDataException, beans::UnknownPropertyException, lang::IllegalAccessException, lang::IllegalArgumentException, uno::RuntimeException);

            virtual void SAL_CALL
                overridePropertyValueForLocale( const OUString& aName, const OUString & aLocale, const uno::Any& aValue )
                    throw (MalformedDataException, beans::UnknownPropertyException, beans::IllegalTypeException, lang::IllegalAccessException, lang::IllegalArgumentException, uno::RuntimeException);

            virtual void SAL_CALL
                addProperty( const OUString& aName, sal_Int16 aAttributes, const uno::Type& aType )
                    throw (MalformedDataException, beans::PropertyExistException, beans::IllegalTypeException, lang::IllegalArgumentException, uno::RuntimeException);

            virtual void SAL_CALL
                addPropertyWithValue( const OUString& aName, sal_Int16 aAttributes, const uno::Any& aValue )
                    throw (MalformedDataException, beans::PropertyExistException, beans::IllegalTypeException, lang::IllegalArgumentException, uno::RuntimeException);

        private:
            void propagateAttributes(ISubtree & _rParent);

            node::Attributes makePropertyAttributes(sal_Int16 aSchemaAttributes)
                CFG_UNO_THROW1( lang::IllegalArgumentException );

            void applyPropertyValue(INode * pProperty, uno::Any const & _aValue)
                CFG_UNO_THROW1( beans::IllegalTypeException );

            void applyPropertyValue(INode * pProperty, uno::Any const & _aValue, OUString const & _aLocale)
                CFG_UNO_THROW2( beans::IllegalTypeException, lang::IllegalArgumentException );

            void applyAttributes(INode * pNode, sal_Int16 aNodeAttributes)
                CFG_UNO_THROW1( lang::IllegalArgumentException );

            void overrideLayerRoot( const OUString& aName, sal_Int16 aAttributes )
                CFG_THROW4 (MalformedDataException, lang::IllegalAccessException, lang::IllegalArgumentException, uno::RuntimeException);

            void startOverride(INode * pNode) /* ensure writable, mark merged */
                CFG_THROW3( MalformedDataException, lang::IllegalAccessException, uno::RuntimeException );

            void implAddOrReplaceNode(const OUString& aName, const TemplateIdentifier& aTemplate, sal_Int16 aAttributes)
                CFG_THROW5 (MalformedDataException, container::NoSuchElementException, lang::IllegalAccessException, lang::IllegalArgumentException, uno::RuntimeException);

            void ensureUnchanged(INode const * pNode) const
                CFG_THROW2( MalformedDataException, uno::RuntimeException );

            void setLocalizedValue(ISubtree * pProperty, uno::Any const & _aValue, OUString const & _aLocale)
                CFG_UNO_THROW1( beans::IllegalTypeException );

            void setValueAndCheck(ValueNode & _rValueNode, uno::Any const & _aValue)
                CFG_UNO_THROW1( beans::IllegalTypeException );
       private:
            MergedComponentData &   m_rData;
            DataBuilderContext      m_aContext;
            ComponentDataFactory    m_aFactory;
            OUString                m_aLocale;
        };
// -----------------------------------------------------------------------------

        /// change attributes and states of the tree to make it a proper default layer
        void promoteToDefault(MergedComponentData & _rTree);
// -----------------------------------------------------------------------------
    } // namespace backend
// -----------------------------------------------------------------------------

} // namespace configmgr
#endif




