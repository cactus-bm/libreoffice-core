/*************************************************************************
 *
 *  $RCSfile: componentdatahelper.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: ssmith $ $Date: 2002-10-24 12:59:33 $
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

/* PLEASE DON'T DELETE ANY COMMENT LINES, ALSO IT'S UNNECESSARY. */

#ifndef CONFIGMGR_BACKEND_COMPONENTDATAHELPER_HXX
#define CONFIGMGR_BACKEND_COMPONENTDATAHELPER_HXX

#ifndef _CONFIGMGR_TREE_VALUENODE_HXX
#include "valuenode.hxx"
#endif

#ifndef CONFIGMGR_UTILITY_HXX_
#include "utility.hxx"
#endif
#ifndef CONFIGMGR_STACK_HXX_
#include "stack.hxx"
#endif

#include <drafts/com/sun/star/configuration/backend/TemplateIdentifier.hpp>
#include <drafts/com/sun/star/configuration/backend/MalformedDataException.hpp>

#ifndef _COM_SUN_STAR_LANG_NOSUPPORTEXCEPTION_HPP_
#include <com/sun/star/lang/NoSupportException.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_ILLEGALACCESSEXCEPTION_HPP_
#include <com/sun/star/lang/IllegalAccessException.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_ILLEGALARGUMENTEXCEPTION_HPP_
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_NOSUCHELEMENTEXCEPTION_HPP_
#include <com/sun/star/container/NoSuchElementException.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_ELEMENTEXISTEXCEPTION_HPP_
#include <com/sun/star/container/ElementExistException.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_ILLEGALTYPEEXCEPTION_HPP_
#include <com/sun/star/beans/IllegalTypeException.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYEXISTEXCEPTION_HPP_
#include <com/sun/star/beans/PropertyExistException.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_UNKNOWNPROPERTYEXCEPTION_HPP_
#include <com/sun/star/beans/UnknownPropertyException.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HPP_
#include <com/sun/star/uno/RuntimeException.hpp>
#endif

#ifndef INCLUDED_MEMORY
#include <memory>
#define INCLUDED_MEMORY
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
        namespace beans     = ::com::sun::star::beans;
        namespace container = ::com::sun::star::container;

        namespace backenduno = ::drafts::com::sun::star::configuration::backend;

        using  backenduno::TemplateIdentifier;
        using  backenduno::MalformedDataException;

        using ::rtl::OUString;

// -----------------------------------------------------------------------------

        class DataBuilderContext
        {
            Stack< ISubtree * >         m_aParentStack;
            OUString                    m_aActiveComponent;
            uno::XInterface *           m_pContext;
            OUString                    m_aExpectedComponentName;
        public:
            DataBuilderContext();
            explicit DataBuilderContext(uno::XInterface * _pContext );
            explicit DataBuilderContext(uno::XInterface * _pContext, const OUString& aExpectedComponentName );
            ~DataBuilderContext();

            bool        isDone() const;

            bool        hasActiveComponent()    const { return m_aActiveComponent.getLength() != 0; }
            OUString    getActiveComponent()    const { return m_aActiveComponent; }

            ISubtree &          getCurrentParent()
                CFG_THROW2( MalformedDataException, uno::RuntimeException )
                { return implGetCurrentParent(); }

            ISubtree const &    getCurrentParent() const
                CFG_THROW2( MalformedDataException, uno::RuntimeException )
                { return implGetCurrentParent(); }

            node::Attributes    getCurrentAttributes() const
                CFG_THROW2( MalformedDataException, uno::RuntimeException )
                { return implGetCurrentParent().getAttributes(); }

            OUString getTemplateComponent(TemplateIdentifier const & aItemType ) const;

            TemplateIdentifier stripComponent   (TemplateIdentifier const & aItemType ) const;
            TemplateIdentifier completeComponent(TemplateIdentifier const & aItemType ) const;

            TemplateIdentifier getCurrentItemType() const
                CFG_THROW2( MalformedDataException, uno::RuntimeException );
            TemplateIdentifier getValidItemType(TemplateIdentifier const & aItemType) const
                CFG_THROW3( MalformedDataException, beans::IllegalTypeException, uno::RuntimeException );

            void startActiveComponent(OUString const & _aComponent)
                CFG_THROW2( MalformedDataException, uno::RuntimeException );
            void endActiveComponent()
                CFG_THROW2( MalformedDataException, uno::RuntimeException );

            bool isProperty(INode * pProp) const
                CFG_UNO_THROW_RTE();

            bool isNode(INode * pNode) const CFG_UNO_THROW_RTE()
            { return !isProperty(pNode); }

            void pushNode(ISubtree * pTree)
                CFG_UNO_THROW_RTE();
            void popNode()
                CFG_THROW2( MalformedDataException, uno::RuntimeException );

            INode    * findProperty(OUString const & _aName)
                CFG_THROW2( MalformedDataException, uno::RuntimeException );
            ISubtree * findNode(OUString const & _aName)
                CFG_THROW2( MalformedDataException, uno::RuntimeException );

            void ensureWritable(INode const * pNode) const
                CFG_UNO_THROW1( lang::IllegalAccessException );
            void ensureRemovable(ISubtree const * pItem) const
                CFG_UNO_THROW1( lang::IllegalAccessException );

            ISubtree  * addNodeToCurrent(std::auto_ptr<ISubtree>  _aNode)
                CFG_THROW3( MalformedDataException, container::ElementExistException, uno::RuntimeException );
            ISubtree  * addLocalizedToCurrent(std::auto_ptr<ISubtree>  _aNode)
                CFG_THROW3( MalformedDataException, beans::PropertyExistException, uno::RuntimeException );
            ValueNode * addPropertyToCurrent(std::auto_ptr<ValueNode> _aNode, bool _bMayReplace = false)
                CFG_THROW3( MalformedDataException, beans::PropertyExistException, uno::RuntimeException );

            void raiseMalformedDataException    (sal_Char const * _pText) const
                CFG_THROW2( MalformedDataException, uno::RuntimeException );
            void raiseNoSupportException        (sal_Char const * _pText) const
                CFG_UNO_THROW1( lang::NoSupportException );
            void raiseIllegalAccessException    (sal_Char const * _pText) const
                CFG_UNO_THROW1( lang::IllegalAccessException );
            void raiseIllegalArgumentException  (sal_Char const * _pText, sal_Int16 _nPos = 0) const
                CFG_UNO_THROW1( lang::IllegalArgumentException );
            void raiseElementExistException     (sal_Char const * _pText, OUString const & _sElement) const
                CFG_UNO_THROW1( container::ElementExistException );
            void raiseNoSuchElementException    (sal_Char const * _pText, OUString const & _sElement) const
                CFG_UNO_THROW1( container::NoSuchElementException );
            void raiseUnknownPropertyException  (sal_Char const * _pText, OUString const & _sElement) const
                CFG_UNO_THROW1( beans::UnknownPropertyException );
            void raisePropertyExistException    (sal_Char const * _pText, OUString const & _sElement) const
                CFG_UNO_THROW1( beans::PropertyExistException );
            void raiseIllegalTypeException  (sal_Char const * _pText) const
                CFG_UNO_THROW1( beans::IllegalTypeException );
        private:
            INode * findChild(OUString const & _aName)
                CFG_THROW2( MalformedDataException, uno::RuntimeException );

            OUString makeMessageWithName(sal_Char const * _pText, OUString const & _aName) const
                CFG_UNO_THROW_RTE(  );

            ISubtree & implGetCurrentParent() const
                CFG_THROW2( MalformedDataException, uno::RuntimeException );
        };
// -----------------------------------------------------------------------------

        class ComponentDataFactory
        {
            OTreeNodeFactory &          m_rNodeFactory;
        public:
            ComponentDataFactory();

            ComponentDataFactory(OTreeNodeFactory & _rNodeFactory)
            : m_rNodeFactory(_rNodeFactory)
            {}

        public:
            OTreeNodeFactory&   getNodeFactory() const { return m_rNodeFactory; }

            std::auto_ptr<ISubtree> createGroup(OUString const & _aName,
                                                bool _bExtensible,
                                                node::Attributes const & _aAttributes) const;

            std::auto_ptr<ISubtree> createSet( OUString const & _aName,
                                                TemplateIdentifier const & aItemType,
                                                bool _bExtensible,
                                                node::Attributes const & _aAttributes) const;

            std::auto_ptr<ISubtree> createLocalizedContainer(OUString const & _aName,
                                                             uno::Type const & _aValueType,
                                                             node::Attributes const & _aAttributes) const;

            std::auto_ptr<ISubtree> createPlaceHolder(OUString const & _aName,
                                                        TemplateIdentifier const & _aInstanceType) const;

            static bool isInstancePlaceHolder(ISubtree const & _aInstanceTree);
            static TemplateIdentifier getInstanceType(ISubtree const & _aInstanceTree);
        };
// -----------------------------------------------------------------------------
    } // namespace backend
// -----------------------------------------------------------------------------

} // namespace configmgr
#endif




