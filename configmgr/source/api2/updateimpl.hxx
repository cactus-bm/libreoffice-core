/*************************************************************************
 *
 *  $RCSfile: updateimpl.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: jb $ $Date: 2001-09-28 12:44:03 $
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

#ifndef CONFIGMGR_API_BASEUPDATEIMPL_HXX_
#define CONFIGMGR_API_BASEUPDATEIMPL_HXX_

#ifndef CONFIGMGR_APITYPES_HXX_
#include "apitypes.hxx"
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYWITHSTATE_HPP_
#include <com/sun/star/beans/XPropertyWithState.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif

namespace configmgr
{
    namespace css = ::com::sun::star;
    namespace uno = ::com::sun::star::uno;

    /* implementations of the interfaces supported by a (parent) node
        within the configuration tree.
        (updating operation)
    */
    namespace configapi
    {
        class NodeSetAccess;
        class NodeTreeSetAccess;
        class NodeValueSetAccess;
        class NodeGroupAccess;

        // XNameReplace
        //---------------------------------------------------------------------
        void implReplaceByName(NodeGroupAccess& rNode, const OUString& rName, const uno::Any& rElement )
            throw(css::lang::IllegalArgumentException, css::container::NoSuchElementException, css::lang::WrappedTargetException, uno::RuntimeException);

        void implReplaceByName(NodeTreeSetAccess& rNode, const OUString& rName, const uno::Any& rElement )
            throw(css::lang::IllegalArgumentException, css::container::NoSuchElementException, css::lang::WrappedTargetException, uno::RuntimeException);

        void implReplaceByName(NodeValueSetAccess& rNode, const OUString& rName, const uno::Any& rElement )
            throw(css::lang::IllegalArgumentException, css::container::NoSuchElementException, css::lang::WrappedTargetException, uno::RuntimeException);

        // XNameContainer
        //---------------------------------------------------------------------
        void implInsertByName(NodeTreeSetAccess& rNode, const OUString& rName, const uno::Any& rElement)
            throw(css::lang::IllegalArgumentException, css::container::ElementExistException, css::lang::WrappedTargetException, uno::RuntimeException);

        void implInsertByName(NodeValueSetAccess& rNode, const OUString& rName, const uno::Any& rElement)
            throw(css::lang::IllegalArgumentException, css::container::ElementExistException, css::lang::WrappedTargetException, uno::RuntimeException);

        void implRemoveByName(NodeTreeSetAccess& rNode, const OUString& rName )
            throw(css::container::NoSuchElementException, css::lang::WrappedTargetException, uno::RuntimeException);

        void implRemoveByName(NodeValueSetAccess& rNode, const OUString& rName )
            throw(css::container::NoSuchElementException, css::lang::WrappedTargetException, uno::RuntimeException);

        // XPropertyWithState - updating operation only
        //---------------------------------------------------------------------
        void implSetToDefaultAsProperty(NodeSetAccess& rNode)
            throw (css::lang::WrappedTargetException, uno::RuntimeException);

        // XSingleServiceFactory
        //---------------------------------------------------------------------
        uno::Reference< uno::XInterface > implCreateElement(NodeTreeSetAccess& rNode )
            throw(uno::Exception, uno::RuntimeException);

        uno::Reference< uno::XInterface > implCreateElement(NodeTreeSetAccess& rNode, const uno::Sequence< uno::Any >& aArguments )
            throw(uno::Exception, uno::RuntimeException);

        //---------------------------------------------------------------------
    }

}
#endif // CONFIGMGR_API_BASEUPDATEIMPL_HXX_


