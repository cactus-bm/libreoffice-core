/*************************************************************************
 *
 *  $RCSfile: notifierimpl.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: hr $ $Date: 2003-03-19 16:18:34 $
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

#ifndef CONFIGMGR_API_NOTIFIERIMPL_HXX_
#define CONFIGMGR_API_NOTIFIERIMPL_HXX_

#include "listenercontainer.hxx"

#include "noderef.hxx"
#include "valueref.hxx"

#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#include <com/sun/star/beans/XVetoableChangeListener.hpp>
#include <com/sun/star/beans/XPropertiesChangeListener.hpp>
#include <com/sun/star/beans/PropertyVetoException.hpp>
#include <com/sun/star/container/XContainerListener.hpp>
#include <com/sun/star/util/XChangesListener.hpp>

#include "propertiesfilterednotifier.hxx"

#include <vos/refernce.hxx>
#include <osl/mutex.hxx>

namespace configmgr
{
    namespace configapi
    {
// ---------------------------------------------------------------------------------------------------

        /// manages collections of event listeners observing a whole config tree, thread-safe
        class NotifierImpl : public vos::OReference
        {
        private:
            mutable osl::Mutex m_aMutex;
        public:
            typedef configuration::SubNodeID        SubNodeID;
            typedef configuration::SubNodeIDList    SubNodeList;
            typedef configuration::NodeID           NodeID;
            typedef configuration::NodeOffset       NodeOffset;

            struct SubNodeHash
            {
                size_t operator() (const SubNodeID& rKey) const {return rKey.hashCode();}
            };
            struct SubNodeEq
            {
                bool operator() (const SubNodeID& lhs,const SubNodeID& rhs) const {return lhs == rhs;}
            };
            struct SubNodeToIndex
            {
                typedef data::Accessor const & KeyFinder;
                configuration::TreeRef aTree;

                SubNodeToIndex( configuration::TreeRef const& aTree ) : aTree(aTree) {}

                bool findKeysForIndex(KeyFinder _anAccessor, NodeOffset nNode, SubNodeList& aList)
                {
                    using configuration::getAllChildrenHelper;
                    using configuration::findNodeFromIndex;
                    aList.clear();
                    getAllChildrenHelper(_anAccessor,findNodeFromIndex(aTree,nNode), aList);
                    return !aList.empty();
                }
                NodeOffset findIndexForKey(SubNodeID const& aNode)
                {
                    return aNode.getParentID().toIndex();
                }
            };
            typedef SpecialListenerContainer <SubNodeID,SubNodeHash,SubNodeEq,SubNodeToIndex> SpecialContainer;

        public:
            SpecialContainer m_aListeners;

        public:
            /// construct this around the given Implementation, for the given tree
            explicit
            NotifierImpl(configuration::TreeRef const& aTree);
            ~NotifierImpl();

            /// retrieve the mutex that is used by this
            osl::Mutex& mutex() const { return m_aMutex; }

            /// Add a <type scope='com::sun::star::lang'>XEventListener</type> observing <var>aNode</var>.
            void add(NodeID const& aNode, uno::Reference< css::lang::XEventListener > const& xListener)
            {
                OSL_PRECOND(xListener.is(), "ERROR: Unexpected NULL listener");

                // ignore the names for now
                m_aListeners.addListener(aNode.toIndex(),getCppuType(&xListener),xListener.get());
            }

            /// Add a <type scope='com::sun::star::container'>XContainerListener</type> observing <var>aNode</var>.
            void add(NodeID const& aNode, uno::Reference< css::container::XContainerListener > const& xListener)
            {
                OSL_PRECOND(xListener.is(), "ERROR: Unexpected NULL listener");

                // ignore the names for now
                m_aListeners.addListener(aNode.toIndex(),getCppuType(&xListener),xListener.get());
            }

            /// Add a <type scope='com::sun::star::util'>XChangesListener</type> observing <var>aNode</var> and its descendants.
            void add(NodeID const& aNode, uno::Reference< css::util::XChangesListener > const& xListener)
            {
                OSL_PRECOND(xListener.is(), "ERROR: Unexpected NULL listener");

                // ignore the names for now
                m_aListeners.addListener(aNode.toIndex(),getCppuType(&xListener),xListener.get());
            }

            /// Add a <type scope='com::sun::star::beans'>XPropertyChangeListener</type> observing <var>aNode</var>.
            void addNamed(SubNodeID const& aNode, uno::Reference< css::beans::XPropertyChangeListener > const& xListener)
            {
                OSL_PRECOND(xListener.is(), "ERROR: Unexpected NULL listener");

                // ignore the names for now
                m_aListeners.addSpecialListener(aNode,xListener.get());
            }
            /// Add a <type scope='com::sun::star::beans'>XPropertyChangeListener</type> observing <var>aNode</var>.
            void addForAll(NodeID const& aNode, uno::Reference< css::beans::XPropertyChangeListener > const& xListener)
            {
                OSL_PRECOND(xListener.is(), "ERROR: Unexpected NULL listener");

                // ignore the names for now
                m_aListeners.addListener(aNode.toIndex(),getCppuType(&xListener),xListener.get());
            }
            /// Add a <type scope='com::sun::star::beans'>XVetoableChangeListener</type> constraining <var>aNode</var>.
            void addNamed(SubNodeID const& aNode, uno::Reference< css::beans::XVetoableChangeListener > const& xListener)
            {
                OSL_PRECOND(xListener.is(), "ERROR: Unexpected NULL listener");

                // ignore the names for now
                m_aListeners.addSpecialListener(aNode,xListener.get());
            }
            /// Add a <type scope='com::sun::star::beans'>XVetoableChangeListener</type> constraining <var>aNode</var>.
            void addForAll(NodeID const& aNode, uno::Reference< css::beans::XVetoableChangeListener > const& xListener)
            {
                OSL_PRECOND(xListener.is(), "ERROR: Unexpected NULL listener");

                // ignore the names for now
                m_aListeners.addListener(aNode.toIndex(),getCppuType(&xListener),xListener.get());
            }

            /** Add a <type scope='com::sun::star::beans'>XPropertiesChangeListener</type>
                observing all properties of <var>aNode</var>.
            */
            void add(NodeID const& aNode, uno::Reference< css::beans::XPropertiesChangeListener > const& xListener)
            {
                OSL_PRECOND(xListener.is(), "ERROR: Unexpected NULL listener");

                // ignore the names for now
                m_aListeners.addListener(aNode.toIndex(),getCppuType(&xListener),xListener.get());
            }

            /** Add a <type scope='com::sun::star::beans'>XPropertiesChangeListener</type>
                observing the properties of <var>aNode</var> (optimally only those given by <var>aNames</var>.
            */
            void add(NodeID const& aNode, uno::Reference< css::beans::XPropertiesChangeListener > const& xListener, uno::Sequence< OUString> const& aNames)
            {
                OSL_PRECOND(xListener.is(), "ERROR: Unexpected NULL listener");
                OSL_PRECOND(aNames.getLength() > 0, "ERROR: Unexpected empty sequence");

                uno::Reference< css::beans::XPropertiesChangeListener > xForwarder( new PropertiesFilteredNotifier(xListener,aNames) );
                // ignore the names for now
                add(aNode,xForwarder);
            }

        // ---------------------------------------------------------------------------------------------------
            /// Remove a <type scope='com::sun::star::lang'>XEventListener</type> observing <var>aNode</var>.
            void remove(NodeID const& aNode, uno::Reference< css::lang::XEventListener > const& xListener)
            {
                // ignore the names for now
                m_aListeners.removeListener(aNode.toIndex(),getCppuType(&xListener),xListener.get());
            }

            /// Remove a <type scope='com::sun::star::container'>XContainerListener</type> observing <var>aNode</var>.
            void remove(NodeID const& aNode, uno::Reference< css::container::XContainerListener > const& xListener)
            {
                // ignore the names for now
                m_aListeners.removeListener(aNode.toIndex(),getCppuType(&xListener),xListener.get());
            }

            /// Remove a <type scope='com::sun::star::util'>XChangesListener</type> observing <var>aNode</var> and its descendants.
            void remove(NodeID const& aNode, uno::Reference< css::util::XChangesListener > const& xListener)
            {
                // ignore the names for now
                m_aListeners.removeListener(aNode.toIndex(),getCppuType(&xListener),xListener.get());
            }

            /// Remove a <type scope='com::sun::star::beans'>XPropertyChangeListener</type> observing <var>aNode</var>.
            void removeNamed(SubNodeID const& aNode, uno::Reference< css::beans::XPropertyChangeListener > const& xListener)
            {
                // ignore the names for now
                m_aListeners.removeSpecialListener(aNode,xListener.get());
            }
            /// Remove a <type scope='com::sun::star::beans'>XPropertyChangeListener</type> observing <var>aNode</var>.
            void removeForAll(NodeID const& aNode, uno::Reference< css::beans::XPropertyChangeListener > const& xListener)
            {
                // ignore the names for now
                m_aListeners.removeListener(aNode.toIndex(),getCppuType(&xListener),xListener.get());
            }
            /// Remove a <type scope='com::sun::star::beans'>XVetoableChangeListener</type> constraining <var>aNode</var>.
            void removeNamed(SubNodeID const& aNode, uno::Reference< css::beans::XVetoableChangeListener > const& xListener)
            {
                // ignore the names for now
                m_aListeners.removeSpecialListener(aNode,xListener.get());
            }
            /// Remove a <type scope='com::sun::star::beans'>XVetoableChangeListener</type> constraining <var>aNode</var>.
            void removeForAll(NodeID const& aNode, uno::Reference< css::beans::XVetoableChangeListener > const& xListener)
            {
                // ignore the names for now
                m_aListeners.removeListener(aNode.toIndex(),getCppuType(&xListener),xListener.get());
            }


            /** Remove a <type scope='com::sun::star::beans'>XPropertiesChangeListener</type>
                observing any properties of <var>aNode</var>.
            */
            void remove(NodeID const& aNode, uno::Reference< css::beans::XPropertiesChangeListener > const& xListener)
            {
                // ignore the names for now
                m_aListeners.removeListener(aNode.toIndex(),getCppuType(&xListener),xListener.get());
            }
        // ---------------------------------------------------------------------------------------------------
        };

// ---------------------------------------------------------------------------------------------------
    }
}

#endif // CONFIGMGR_API_NOTIFIERIMPL_HXX_
