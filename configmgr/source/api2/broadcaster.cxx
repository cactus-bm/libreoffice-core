/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: broadcaster.cxx,v $
 *
 *  $Revision: 1.18 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 23:15:12 $
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

#include "broadcaster.hxx"

#ifndef CONFIGMGR_API_NOTIFIERIMPL_HXX_
#include "notifierimpl.hxx"
#endif
#ifndef CONFIGMGR_CONFIGNOTIFIER_HXX_
#include "confignotifier.hxx"
#endif

#ifndef CONFIGMGR_CONFIGNODE_HXX_
#include "noderef.hxx"
#endif
#ifndef CONFIGMGR_CONFIGCHANGE_HXX_
#include "nodechange.hxx"
#endif
#ifndef CONFIGMGR_CONFIGCHANGEINFO_HXX_
#include "nodechangeinfo.hxx"
#endif
#ifndef CONFIGMGR_API_TRANSLATECHANGES_HXX_
#include "translatechanges.hxx"
#endif

#ifndef CONFIGMGR_API_FACTORY_HXX_
#include "apifactory.hxx"
#endif
#ifndef CONFIGMGR_API_TREEACCESS_HXX_
#include "apitreeaccess.hxx"
#endif
#ifndef CONFIGMGR_API_TREEIMPLOBJECTS_HXX_
#include "apitreeimplobj.hxx"
#endif

#ifndef _VOS_REFERNCE_HXX_
#include <vos/refernce.hxx>
#endif

#ifndef INCLUDED_MAP
#include <map>
#define INCLUDED_MAP
#endif
#ifndef INCLUDED_SET
#include <set>
#define INCLUDED_SET
#endif
#ifndef INCLUDED_FUNCTIONAL
#include <functional>
#define INCLUDED_FUNCTIONAL
#endif

namespace configmgr
{
    namespace configapi
    {
// ---------------------------------------------------------------------------------------------------
        using configuration::Tree;
        using configuration::NodeID;

        using configuration::NodeChangeInformation;
        using configuration::NodeChangeLocation;
        using configuration::NodeChangesInformation;
// ---------------------------------------------------------------------------------------------------
// Broadcaster implementation
// ---------------------------------------------------------------------------------------------------
    class BroadcasterHelper
    {
    public:
        static NotifierHolder getImpl(Notifier const& aNotifier) { return aNotifier.m_aImpl; }
    };
// ---------------------------------------------------------------------------------------------------

    namespace
    {
    // -----------------------------------------------------------------------------------------------
        template <class T>
        struct LessORefBodyPtr
        {
            typedef vos::ORef<T> Ref;
            bool operator()(Ref const& lhs, Ref const& rhs) const
            {
                return ptr_less(lhs.getBodyPtr(), rhs.getBodyPtr());
            }

            std::less<T*> ptr_less;
        };
    // -----------------------------------------------------------------------------------------------
        class ApiTreeRef
        {
            ApiTreeImpl const*  m_pApiTree;
            UnoInterfaceRef     m_xKeepAlive;
        public:
            explicit ApiTreeRef(ApiTreeImpl const* _pApiTree = NULL)
            : m_pApiTree(_pApiTree)
            , m_xKeepAlive()
            {
                if (m_pApiTree) m_xKeepAlive = m_pApiTree->getUnoInstance();
            }

            bool is() const
            {
                OSL_ASSERT(!m_pApiTree == !m_xKeepAlive.is());
                return m_pApiTree != NULL;
            }

            ApiTreeImpl const* get()        const { return m_pApiTree; }
            ApiTreeImpl const* operator->() const { return m_pApiTree; }

            friend bool operator==(ApiTreeRef const& lhs,ApiTreeRef const& rhs)
            { return lhs.m_pApiTree == rhs.m_pApiTree; }

            friend bool operator!=(ApiTreeRef const& lhs,ApiTreeRef const& rhs)
            { return lhs.m_pApiTree != rhs.m_pApiTree; }
        };
    // -----------------------------------------------------------------------------------------------
        typedef std::map< NotifierHolder, ApiTreeRef, LessORefBodyPtr<NotifierImpl> > NotifierSet;
        typedef NotifierSet::value_type NotifierData;
    // -----------------------------------------------------------------------------------------------

    }
// ---------------------------------------------------------------------------------------------------
// class Broadcaster::Impl
// ---------------------------------------------------------------------------------------------------
    class Broadcaster::Impl : public vos::OReference
    {
    private:
        NotifierData m_aNotifierData;
    public:
        Impl(NotifierData const& aNotifierData) : m_aNotifierData(aNotifierData) {}

        NotifierData getNotifierData() const { return m_aNotifierData; }

        bool translateChanges(NodeChangesInformation& aInfos, NodeChanges const& aChanges, bool bSingleBase) const;
        bool translateChanges(NodeChangesInformation& aInfos, NodeChangesInformation const& aChanges, bool bSingleBase) const;

        void queryConstraints(NodeChangesInformation const& aChanges)   { this->doQueryConstraints(aChanges); }
        void notifyListeners(NodeChangesInformation const& aChanges)    { this->doNotifyListeners(aChanges); }

        void notifyRootListeners(NodeChangesInformation const& aChanges);

        static vos::ORef<Impl> create(NotifierHolder const& rNotifierImpl, ApiTreeRef const& pTreeImpl, NodeChange const& aChange, bool bLocal);
        static vos::ORef<Impl> create(NotifierHolder const& rNotifierImpl, ApiTreeRef const& pTreeImpl, NodeChanges const& aChange, bool bLocal);
        static vos::ORef<Impl> create(NotifierHolder const& rNotifierImpl, ApiTreeRef const& pTreeImpl, NodeChangeInformation const& aChange, bool bLocal);
        static vos::ORef<Impl> create(NotifierHolder const& rNotifierImpl, ApiTreeRef const& pTreeImpl, NodeChangesInformation const& aChange, bool bLocal);

    private:
        virtual void doQueryConstraints(NodeChangesInformation const& aChanges) = 0;
        virtual void doNotifyListeners(NodeChangesInformation const& aChanges) = 0;
    };
// ---------------------------------------------------------------------------------------------------
    namespace
    {
    // -----------------------------------------------------------------------------------------------
        using configuration::Tree;
        using configuration::TreeRef;
        using configuration::NodeRef;
        using configuration::NodeID;
        using configuration::SubNodeID;
        using configuration::NodeOffset;
        using configuration::NodeChange;
        using configuration::NodeChangeInformation;
        using configuration::NodeChangeData;

        typedef std::set< configuration::NodeID >       NodeSet;
        typedef std::set< configuration::SubNodeID > SubNodeSet;
    // -----------------------------------------------------------------------------------------------
        typedef vos::ORef< Broadcaster::Impl > BroadcasterImplRef;
    // -----------------------------------------------------------------------------------------------

        class EmptyBroadcaster_Impl : public Broadcaster::Impl
        {
            EmptyBroadcaster_Impl(NotifierData const& rNotifierData)
            : Broadcaster::Impl(rNotifierData)
            {
            }
        public:
            static
            BroadcasterImplRef create(NotifierData const& rRootNotifier)
            {
                return new EmptyBroadcaster_Impl(rRootNotifier);
            }
        private:
            virtual void doQueryConstraints(NodeChangesInformation const& aChanges);
            virtual void doNotifyListeners(NodeChangesInformation const& aChanges);
        };

        void EmptyBroadcaster_Impl::doQueryConstraints(NodeChangesInformation const&) {}
        void EmptyBroadcaster_Impl::doNotifyListeners(NodeChangesInformation const&) {}
    // -----------------------------------------------------------------------------------------------

        class NodeLocalBroadcaster_Impl : public Broadcaster::Impl
        {
            NodeID      aAffectedNode;

        public:
            NodeLocalBroadcaster_Impl(NotifierData const& rTreeNotifierData, NodeID const& aAffectedID)
            : Broadcaster::Impl(rTreeNotifierData)
            , aAffectedNode(aAffectedID)
            {
            }

            NodeID getAffectedNodeID() const { return aAffectedNode; }
            NodeOffset getNodeIndex() const { return aAffectedNode.toIndex(); }

        protected:
            void querySingleConstraint(NodeChangeInformation const& aChange, bool bMore);
            void notifySingleChange(NodeChangeInformation const& aChange, bool bMore, css::beans::PropertyChangeEvent*& pCurEvent);
        };
    // -----------------------------------------------------------------------------------------------

        class SingleChangeBroadcaster_Impl : public NodeLocalBroadcaster_Impl
        {
            SubNodeID m_aChangingValue;

            SingleChangeBroadcaster_Impl(NotifierData const& rTreeNotifierData, NodeID const& aAffectedID, SubNodeID const& aChangedValue);

        public:
            static
            NodeLocalBroadcaster_Impl* create(
                        NotifierData const& rLocalNotifier,
                        NodeChangeLocation const& aChange);
            static
            NodeLocalBroadcaster_Impl* create(
                        NotifierData const& rLocalNotifier,
                        NodeID const& aAffectedID,
                        NodeChangeLocation const& aChange);
            static
            NodeLocalBroadcaster_Impl* create(
                        NotifierData const& rLocalNotifier,
                        NodeID const& aAffectedID,
                        SubNodeID const& aChangedNode,
                        NodeChangeLocation const& aChange);
        private:
            virtual void doQueryConstraints(NodeChangesInformation const& aChanges);
            virtual void doNotifyListeners(NodeChangesInformation const& aChanges);
        };

    // -----------------------------------------------------------------------------------------------
        class MultiChangeBroadcaster_Impl : public NodeLocalBroadcaster_Impl
        {
            SubNodeSet  m_aChangingNodes;

            MultiChangeBroadcaster_Impl(NotifierData const& rTreeNotifierData, NodeID const& aAffectedID, SubNodeSet& aChangedNodes);

        public:
            static
            NodeLocalBroadcaster_Impl* create(
                        NotifierData const& rLocalNotifier,
                        NodeChangesInformation const& aChanges);
            static
            NodeLocalBroadcaster_Impl* create(
                        NotifierData const& rLocalNotifier,
                        NodeID const& aAffectedID,
                        NodeChangesInformation const& aChanges);
        private:
            virtual void doQueryConstraints(NodeChangesInformation const& aChanges);
            virtual void doNotifyListeners(NodeChangesInformation const& aChanges);
        };
    // -----------------------------------------------------------------------------------------------
        class SingleTreeBroadcaster_Impl : public Broadcaster::Impl
        {
            typedef std::vector< vos::ORef<NodeLocalBroadcaster_Impl> > BroadcasterList;
            BroadcasterList m_aBroadcasters;

            SingleTreeBroadcaster_Impl(NotifierData const& rTreeNotifierData, BroadcasterList& aBroadcasters);

        public:
        //--------------------------
            static
            BroadcasterImplRef create(
                        NotifierData const& rRootNotifier,
                        NotifierData const& rLocalNotifier,
                        NodeChangesInformation const& aChanges);

            static bool selectChanges(NodeChangesInformation& rSelected, NodeChangesInformation const& aOriginal, NodeID const& aSelector);
        //--------------------------
        private:
            virtual void doQueryConstraints(NodeChangesInformation const& aChanges);
            virtual void doNotifyListeners(NodeChangesInformation const& aChanges);
        };
    // -----------------------------------------------------------------------------------------------
        class MultiTreeBroadcaster_Impl : public Broadcaster::Impl
        {
            typedef std::vector< BroadcasterImplRef > BroadcasterList;
            BroadcasterList m_aBroadcasters;

            MultiTreeBroadcaster_Impl(NotifierData const& rRootNotifierData, BroadcasterList& aBroadcasters);
        public:
        //--------------------------
            static
            BroadcasterImplRef create(
                        NotifierData const& rRootNotifier,
                        NotifierSet const& rNotifiers,
                        NodeChangesInformation const& aChanges);

            static bool selectChanges(NodeChangesInformation& rSelected, NodeChangesInformation const& aOriginal, NotifierData const& aSelector);
        //--------------------------
        private:
            virtual void doQueryConstraints(NodeChangesInformation const& aChanges);
            virtual void doNotifyListeners(NodeChangesInformation const& aChanges);
        };

    // -----------------------------------------------------------------------------------------------

        inline NodeID makeRootID( Tree const& aTree ) { return NodeID( aTree, aTree.getRootNode() ); }
        inline NodeID makeRootID( TreeRef const& aTree ) { return NodeID( aTree, aTree.getRootNode() ); }
        inline NodeID makeRootID( ApiTreeRef const& pTreeImpl ) { return makeRootID( pTreeImpl->getTree() ); }
    // -----------------------------------------------------------------------------------------------
        NotifierData findNotifier(NodeChangeLocation const& aChange, ApiTreeRef const& pTreeImpl)
        {
            OSL_ENSURE(aChange.isValidData(),"Invalid change location - cannot find notifier");

            NodeID aAffectedNode = aChange.getAffectedNodeID();
            if (aAffectedNode.isEmpty())
                return NotifierData();

            ApiTreeRef aAffectedImpl( Factory::findDescendantTreeImpl(aAffectedNode, pTreeImpl.get()) );
            if (aAffectedImpl.is())
            {
                NotifierHolder aAffectedNotifier = BroadcasterHelper::getImpl(aAffectedImpl->getNotifier());

                return  NotifierData(aAffectedNotifier, aAffectedImpl);
            }
            else
                return NotifierData();
        }
    // -----------------------------------------------------------------------------------------------
        inline
        NotifierData findNotifier(NodeChangeInformation const& aChange, ApiTreeRef const& pTreeImpl)
        {
            return findNotifier(aChange.location,pTreeImpl);
        }
    // -----------------------------------------------------------------------------------------------

        void findNotifiers(NotifierSet& aNotifiers, NodeChangesInformation const& aChanges, ApiTreeRef const& pTreeImpl )
        {
            for (NodeChangesInformation::Iterator it = aChanges.begin(); it != aChanges.end(); ++it)
            {
                NotifierData aNotifierData( findNotifier(*it,pTreeImpl) );

                if (aNotifierData.first.isValid())
                {
                    aNotifiers.insert( aNotifierData );
                    OSL_ENSURE( aNotifiers[aNotifierData.first] == aNotifierData.second, "Different Api Trees for the same notifier" );
                }
            }
        }
    // -----------------------------------------------------------------------------------------------
    // NodeLocalBroadcaster_Impl
    // -----------------------------------------------------------------------------------------------
        void NodeLocalBroadcaster_Impl::querySingleConstraint(NodeChangeInformation const& aChange, bool bMore)
        {
            using css::beans::XVetoableChangeListener;

            typedef ListenerContainerIterator< XVetoableChangeListener > ListenerIterator;
            uno::Reference< XVetoableChangeListener > const * const SelectListener = 0;

            NotifierImplHolder pNotifierImpl = getNotifierData().first;

            ListenerContainer* pListeners   = pNotifierImpl->m_aListeners.getContainer( getNodeIndex(), getCppuType(SelectListener) );
            ListenerContainer* pSpecial     = pNotifierImpl->m_aListeners.getSpecialContainer( aChange.location.getChangingValueID() );

            if (pSpecial || pListeners)
            {
                css::beans::PropertyChangeEvent aEvent;
                aEvent.Source = pNotifierImpl->m_aListeners.getObjectAt( getNodeIndex() );

                if (configapi::fillEventDataFromResolved(aEvent,aChange,bMore))
                {
                    // Catch only RuntimeExceptions here: vetoableChange issues its veto by throwing
                    // a PropertyVetoException (which is not a RuntimeException)
                    if (pListeners)
                    {
                        ListenerIterator aIterator(*pListeners);
                        while (aIterator.hasMoreElements())
                        try
                        {
                            aIterator.next()->vetoableChange(aEvent);
                        }
                        catch (uno::RuntimeException & )
                        {}
                    }
                    if (pSpecial)
                    {
                        ListenerIterator aIterator(*pSpecial);
                        while (aIterator.hasMoreElements())
                        try
                        {
                            aIterator.next()->vetoableChange(aEvent);
                        }
                        catch (uno::RuntimeException & )
                        {}
                    }
                }
            }

        }
    // -----------------------------------------------------------------------------------------------
        void NodeLocalBroadcaster_Impl::notifySingleChange(NodeChangeInformation const& aChange, bool bMore, css::beans::PropertyChangeEvent*& pCurEvent)
        {
            using css::beans::XPropertyChangeListener;
            using css::container::XContainerListener;

            typedef ListenerContainerIterator< XPropertyChangeListener >    PropertyListenerIterator;
            typedef ListenerContainerIterator< XContainerListener >         ContainerListenerIterator;
            uno::Reference< XPropertyChangeListener > const * const     SelectPropertyListener = 0;
            uno::Reference< XContainerListener > const * const          SelectContainerListener = 0;

            NotifierImplHolder pNotifierImpl = getNotifierData().first;

            ListenerContainer* pContainerListeners  = pNotifierImpl->m_aListeners.getContainer( getNodeIndex(), getCppuType(SelectContainerListener) );

            if (pContainerListeners)
            {
                css::container::ContainerEvent aEvent;
                aEvent.Source = pNotifierImpl->m_aListeners.getObjectAt( getNodeIndex() );

                if (configapi::fillEventDataFromResolved(aEvent,aChange))
                {

                    ContainerListenerIterator aIterator(*pContainerListeners);

                    while (aIterator.hasMoreElements())
                    try
                    {
                        uno::Reference<XContainerListener> xListener( aIterator.next() );
                        OSL_ASSERT( xListener.is() );

                        switch (aChange.change.type)
                        {
                        case NodeChangeData::eSetValue:
                        case NodeChangeData::eSetDefault:
                        case NodeChangeData::eReplaceElement:
                            xListener->elementReplaced(aEvent);
                            break;

                        case NodeChangeData::eInsertElement:
                            xListener->elementInserted(aEvent);
                            break;

                        case NodeChangeData::eRemoveElement:
                            xListener->elementRemoved(aEvent);
                            break;


                        case NodeChangeData::eResetSetDefault:
                        case NodeChangeData::eRenameElementTree:
                        case NodeChangeData::eNoChange:
                            OSL_ASSERT(false);
                            break;
                        }
                    }
                    catch (uno::Exception &)
                    {}
                }
            }


            OSL_ASSERT(pCurEvent);
            css::beans::PropertyChangeEvent& rEvent = *pCurEvent;

            rEvent.Source = pNotifierImpl->m_aListeners.getObjectAt( getNodeIndex() );

            if (configapi::fillEventDataFromResolved(rEvent,aChange,bMore))
            {
                ListenerContainer* pPropertyListeners   = pNotifierImpl->m_aListeners.getContainer( getNodeIndex(), getCppuType(SelectPropertyListener) );
                if (pPropertyListeners)
                {
                    PropertyListenerIterator aIterator(*pPropertyListeners);
                    while (aIterator.hasMoreElements())
                        try { aIterator.next()->propertyChange(rEvent); } catch (uno::Exception & ) {}
                }

                ListenerContainer* pSpecialListeners    = pNotifierImpl->m_aListeners.getSpecialContainer( aChange.location.getChangingValueID() );
                if (pSpecialListeners)
                {
                    PropertyListenerIterator aIterator(*pSpecialListeners);
                    while (aIterator.hasMoreElements())
                        try { aIterator.next()->propertyChange(rEvent); } catch (uno::Exception & ) {}
                }

                ++pCurEvent;
            }

        }
    // -----------------------------------------------------------------------------------------------

    // -----------------------------------------------------------------------------------------------
    // SingleBroadcaster_Impl
    // -----------------------------------------------------------------------------------------------
        SingleChangeBroadcaster_Impl::SingleChangeBroadcaster_Impl(
            NotifierData const& rTreeNotifierData,
            NodeID const& aAffectedID, SubNodeID const& aChangedNode
        )
        : NodeLocalBroadcaster_Impl(rTreeNotifierData,aAffectedID)
        , m_aChangingValue(aChangedNode)
        {
        }
    // -----------------------------------------------------------------------------------------------
        NodeLocalBroadcaster_Impl* SingleChangeBroadcaster_Impl::create(
                    NotifierData const& rLocalNotifier,
                    NodeChangeLocation const& aChange)
        {
            OSL_ENSURE(configuration::equalTreeRef(rLocalNotifier.second->getTree(), aChange.getAffectedTreeRef()),
                        "ERROR: Tree Mismatch creating Single Broadcaster");

            OSL_ENSURE(aChange.isValidData(), "ERROR: Invalid Change Location for Broadcaster");

            NodeID aAffectedNodeID = aChange.getAffectedNodeID();
            if (aAffectedNodeID.isEmpty())
                return 0;

            return create(rLocalNotifier,aAffectedNodeID,aChange.getChangingValueID(),aChange);
        }
    // -----------------------------------------------------------------------------------------------
        NodeLocalBroadcaster_Impl* SingleChangeBroadcaster_Impl::create(
                    NotifierData const& rLocalNotifier,
                    NodeID const& aAffectedID,
                    NodeChangeLocation const& aChange)
        {

            return create(rLocalNotifier,aAffectedID,aChange.getChangingValueID(),aChange);
        }
    // -----------------------------------------------------------------------------------------------
        NodeLocalBroadcaster_Impl* SingleChangeBroadcaster_Impl::create(
                    NotifierData const& rLocalNotifier,
                    NodeID const& aAffectedID,
                    SubNodeID const& aChangedNodeID,
                    NodeChangeLocation const& aChange)
        {
                    { (void)aChange; }
            OSL_ENSURE(aChange.isValidData(), "ERROR: Invalid Change Location for Broadcaster");
            OSL_ENSURE(aAffectedID.isValidNode(),"Cannot broadcast without affected node");

            OSL_ENSURE(configuration::equalTreeRef(rLocalNotifier.second->getTree(), aChange.getAffectedTreeRef()),
                        "ERROR: Tree Mismatch creating Single Broadcaster");
            OSL_ENSURE( aChange.getAffectedNodeID() == aAffectedID,
                        "ERROR: Node Mismatch creating Single Broadcaster");
            OSL_ENSURE( aChange.getChangingValueID() == aChangedNodeID,
                        "ERROR: Value Node Mismatch creating Single Broadcaster");

            return new SingleChangeBroadcaster_Impl(rLocalNotifier,aAffectedID,aChangedNodeID);
        }
    // -----------------------------------------------------------------------------------------------
        void SingleChangeBroadcaster_Impl::doQueryConstraints(NodeChangesInformation const& aChanges)
        {
            OSL_ASSERT(aChanges.size() <= 1);
            if (!aChanges.empty())
            {
                NodeChangesInformation::Iterator it = aChanges.begin();

                OSL_ENSURE( m_aChangingValue == it->location.getChangingValueID(), "Broadcasting unanticipated change");

                querySingleConstraint(*it, false);
            }

        }
    // -----------------------------------------------------------------------------------------------
        void SingleChangeBroadcaster_Impl::doNotifyListeners(NodeChangesInformation const& aChanges)
        {
            using css::beans::XPropertiesChangeListener;
            using css::beans::PropertyChangeEvent;


            OSL_ASSERT(aChanges.size() <= 1);
            if (!aChanges.empty())
            {

                PropertyChangeEvent aEvent;
                PropertyChangeEvent * pEventNext = &aEvent;

                NodeChangesInformation::Iterator it = aChanges.begin();

                OSL_ENSURE( m_aChangingValue == it->location.getChangingValueID(), "Broadcasting unanticipated change");

                notifySingleChange(*it, false, pEventNext);

                if (pEventNext != &aEvent)
                {
                    uno::Sequence< PropertyChangeEvent > aPropertyEvents(&aEvent,1);

                    typedef ListenerContainerIterator< XPropertiesChangeListener > ListenerIterator;
                    uno::Reference< XPropertiesChangeListener > const * const SelectListener = 0;

                    NotifierImplHolder pNotifierImpl = getNotifierData().first;

                    ListenerContainer* pContainer = pNotifierImpl->m_aListeners.getContainer( getNodeIndex(), getCppuType(SelectListener) );

                    if (pContainer)
                    {
                        ListenerIterator aIterator(*pContainer);
                        while (aIterator.hasMoreElements())
                            try { aIterator.next()->propertiesChange(aPropertyEvents); } catch (uno::Exception & ) {}
                    }
                }
            }
        }

    // -----------------------------------------------------------------------------------------------
    // MultiChangeBroadcaster_Impl
    // -----------------------------------------------------------------------------------------------

        MultiChangeBroadcaster_Impl::MultiChangeBroadcaster_Impl(
            NotifierData const& rTreeNotifierData,
            NodeID const& aAffectedID, SubNodeSet& aChangedNodes
        )
        : NodeLocalBroadcaster_Impl(rTreeNotifierData,aAffectedID)
        , m_aChangingNodes()
        {
            m_aChangingNodes.swap(aChangedNodes);
        }
    // -----------------------------------------------------------------------------------------------
        NodeLocalBroadcaster_Impl* MultiChangeBroadcaster_Impl::create(
                    NotifierData const& rLocalNotifier,
                    NodeChangesInformation const& aChanges)
        {
            if (aChanges.empty())
                return 0;

            OSL_ENSURE(aChanges.begin()->hasValidLocation(), "ERROR: Invalid Change Location for Broadcaster");

            NodeID aAffectedNodeID = aChanges.begin()->location.getAffectedNodeID();
            if (aAffectedNodeID.isEmpty())
                return 0;

            return create(rLocalNotifier, aAffectedNodeID, aChanges);
        }
    // -----------------------------------------------------------------------------------------------
        NodeLocalBroadcaster_Impl* MultiChangeBroadcaster_Impl::create(
                    NotifierData const& rLocalNotifier,
                    NodeID const& aAffectedNodeID,
                    NodeChangesInformation const& aChanges)
        {
            if (aChanges.empty())
                return 0;

            else if (aChanges.size() == 1)
                return SingleChangeBroadcaster_Impl::create(rLocalNotifier,aAffectedNodeID,aChanges.begin()->location);

            else
            {
                OSL_ENSURE(aAffectedNodeID.isValidNode(),"Cannot broadcast without affected node");

                SubNodeSet aChangedNodes;
                for (NodeChangesInformation::Iterator it = aChanges.begin(); it != aChanges.end(); ++it)
                {
                    OSL_ENSURE(it->hasValidLocation(), "ERROR: Invalid Change Location for Broadcaster");

                    OSL_ENSURE(it->location.getAffectedNodeID() == aAffectedNodeID, "ERROR: Change is not local to affected node (as advertised)");
                    OSL_ENSURE(configuration::equalTreeRef(rLocalNotifier.second->getTree(), it->location.getAffectedTreeRef()),
                                "ERROR: Tree Mismatch creating Multi Change Broadcaster");

                    SubNodeID aChangedValueID = it->location.getChangingValueID();

                    aChangedNodes.insert(aChangedValueID);
                }
                OSL_ENSURE(!aChangedNodes.empty(), "Changes don't affect any nodes");

                if (aChangedNodes.size() == 1) OSL_TRACE("WARNING: Different changes all affect the same node !");

                return new MultiChangeBroadcaster_Impl(rLocalNotifier, aAffectedNodeID, aChangedNodes);
            }
        }
    // -----------------------------------------------------------------------------------------------
        void MultiChangeBroadcaster_Impl::doQueryConstraints(NodeChangesInformation const& aChanges)
        {
            NodeChangesInformation::Iterator const stop = aChanges.end(), last = stop-1;

            for (NodeChangesInformation::Iterator it = aChanges.begin(); it != stop; ++it)
            {
                OSL_ENSURE( m_aChangingNodes.find( it->location.getChangingValueID() ) != m_aChangingNodes.end(), "Broadcasting unanticipated change");

                querySingleConstraint(*it, it != last);
            }

        }
    // -----------------------------------------------------------------------------------------------
        void MultiChangeBroadcaster_Impl::doNotifyListeners(NodeChangesInformation const& aChanges)
        {
            using css::beans::XPropertiesChangeListener;
            using css::beans::PropertyChangeEvent;

            uno::Sequence< PropertyChangeEvent > aPropertyEvents(aChanges.size());

            PropertyChangeEvent * const pEventStart = aPropertyEvents.getArray();
            PropertyChangeEvent * pEventNext = pEventStart;

            NodeChangesInformation::Iterator const stop = aChanges.end(), last = stop-1;

            for (NodeChangesInformation::Iterator it = aChanges.begin(); it != stop; ++it)
            {
                // #92463# Skip nodes that are not in the tree
                if (it->location.getAffectedNodeID().isEmpty()) continue;

                OSL_ENSURE( m_aChangingNodes.find( it->location.getChangingValueID() ) != m_aChangingNodes.end(), "Broadcasting unanticipated change");

                notifySingleChange(*it, it != last, pEventNext);
            }

            sal_Int32 nPropertyEvents = pEventNext-pEventStart;

            if (nPropertyEvents > 0)
            {
                OSL_ASSERT(nPropertyEvents <= aPropertyEvents.getLength());
                if (nPropertyEvents != aPropertyEvents.getLength())
                    aPropertyEvents.realloc(nPropertyEvents);

                typedef ListenerContainerIterator< XPropertiesChangeListener > ListenerIterator;
                uno::Reference< XPropertiesChangeListener > const * const SelectListener = 0;

                NotifierImplHolder pNotifierImpl = getNotifierData().first;

                ListenerContainer* pContainer = pNotifierImpl->m_aListeners.getContainer( getNodeIndex(), getCppuType(SelectListener) );

                if (pContainer)
                {
                    ListenerIterator aIterator(*pContainer);
                    while (aIterator.hasMoreElements())
                        try { aIterator.next()->propertiesChange(aPropertyEvents); } catch (uno::Exception & ) {}
                }
            }
        }
    // -----------------------------------------------------------------------------------------------
    // TreeLocalBroadcaster_Impl
    // -----------------------------------------------------------------------------------------------

        SingleTreeBroadcaster_Impl::SingleTreeBroadcaster_Impl(
            NotifierData const& aTreeNotifierData,
            BroadcasterList& aBroadcasters
        )
        : Broadcaster::Impl(aTreeNotifierData)
        , m_aBroadcasters()
        {
            m_aBroadcasters.swap(aBroadcasters);
        }

    // -----------------------------------------------------------------------------------------------

        bool SingleTreeBroadcaster_Impl::selectChanges(NodeChangesInformation& rSelected, NodeChangesInformation const& aOriginal, NodeID const& aSelector)
        {
            OSL_ASSERT(rSelected.empty()); // nothing in there yet

            for (NodeChangesInformation::Iterator it = aOriginal.begin(); it != aOriginal.end(); ++it)
            {
                if ( it->location.getAffectedNodeID() == aSelector )
                {
                    rSelected.push_back(*it);
                }
            }
            return !rSelected.empty();
        }
    // -----------------------------------------------------------------------------------------------

        BroadcasterImplRef SingleTreeBroadcaster_Impl::create(
                    NotifierData const& rRootNotifier,
                    NotifierData const& rLocalNotifier,
                    NodeChangesInformation const& aChanges)
        {
            NodeSet aNodes;
            for (NodeChangesInformation::Iterator itChanges = aChanges.begin(); itChanges != aChanges.end(); ++itChanges)
            {
                OSL_ENSURE(itChanges->hasValidLocation(), "ERROR: Invalid Change Location for Broadcaster");

                NodeID aAffectedNodeID = itChanges->location.getAffectedNodeID();
                if (!aAffectedNodeID.isEmpty())
                    aNodes.insert(aAffectedNodeID);
            }

            BroadcasterList aNodecasters;
            for (NodeSet::const_iterator itNodes = aNodes.begin(); itNodes != aNodes.end(); ++itNodes)
            {
                OSL_ASSERT(itNodes->isValidNode()); // filtered empty ones above

                NodeChangesInformation aSelectedChanges;
                if ( selectChanges(aSelectedChanges, aChanges, *itNodes))
                {
                    NodeLocalBroadcaster_Impl* pSelectedImpl = MultiChangeBroadcaster_Impl::create(rLocalNotifier, *itNodes, aSelectedChanges);
                    if (pSelectedImpl)
                        aNodecasters.push_back(pSelectedImpl);
                }
            }

            if (aNodecasters.empty())
                return 0;

            else if (aNodecasters.size() == 1)
                return aNodecasters.begin()->getBodyPtr();

            else
                return new SingleTreeBroadcaster_Impl(rRootNotifier, aNodecasters);
        }
    // -----------------------------------------------------------------------------------------------
        void SingleTreeBroadcaster_Impl::doQueryConstraints(NodeChangesInformation const& aChanges)
        {
            for(BroadcasterList::iterator it = m_aBroadcasters.begin(); it != m_aBroadcasters.end(); ++it)
            {
                NodeChangesInformation aSelectedInfos;
                if ( selectChanges(aSelectedInfos, aChanges, (*it)->getAffectedNodeID()) )
                    (*it)->queryConstraints(aSelectedInfos);
            }
        }
    // -----------------------------------------------------------------------------------------------
        void SingleTreeBroadcaster_Impl::doNotifyListeners(NodeChangesInformation const& aChanges)
        {
            for(BroadcasterList::iterator it = m_aBroadcasters.begin(); it != m_aBroadcasters.end(); ++it)
            {
                NodeChangesInformation aSelectedInfos;
                if ( selectChanges(aSelectedInfos, aChanges, (*it)->getAffectedNodeID()) )
                    (*it)->notifyListeners(aSelectedInfos);
            }
        }
    // -----------------------------------------------------------------------------------------------
    // MultiTreeBroadcaster_Impl
    // -----------------------------------------------------------------------------------------------
        MultiTreeBroadcaster_Impl::MultiTreeBroadcaster_Impl(NotifierData const& aRootSelector, BroadcasterList& aBroadcasters)
        : Broadcaster::Impl(aRootSelector)
        , m_aBroadcasters()
        {
            m_aBroadcasters.swap(aBroadcasters);
        }

    // -----------------------------------------------------------------------------------------------

        bool MultiTreeBroadcaster_Impl::selectChanges(NodeChangesInformation& rSelected, NodeChangesInformation const& aOriginal, NotifierData const& aSelector)
        {
            OSL_ASSERT(aSelector.first.isValid());
            OSL_ASSERT(aSelector.second.is());

            OSL_ASSERT(rSelected.empty()); // nothing in there yet

            TreeRef const aSelectedTree( aSelector.second->getTree() );

            for (NodeChangesInformation::Iterator it = aOriginal.begin(); it != aOriginal.end(); ++it)
            {
                if ( configuration::equalTreeRef(it->location.getAffectedTreeRef(),aSelectedTree) )
                {
                    rSelected.push_back(*it);
                }
            }
            return !rSelected.empty();
        }
        // -------------------------------------------------------------------------------------------

        BroadcasterImplRef MultiTreeBroadcaster_Impl::create(NotifierData const& rRootNotifier, NotifierSet const& rNotifiers, NodeChangesInformation const& aChanges)
        {
            BroadcasterList aTreecasters;
            for (NotifierSet::const_iterator it = rNotifiers.begin(); it != rNotifiers.end(); ++it)
            {
                NodeChangesInformation aSelectedChanges;
                if ( selectChanges(aSelectedChanges, aChanges, *it))
                {
                    BroadcasterImplRef pSelectedImpl = SingleTreeBroadcaster_Impl::create(rRootNotifier, *it, aSelectedChanges);
                    if (pSelectedImpl.isValid())
                        aTreecasters.push_back(pSelectedImpl);
                }
            }

            if (aTreecasters.empty())
                return 0;

            else if (aTreecasters.size() == 1)
                return *aTreecasters.begin();

            else
                return new MultiTreeBroadcaster_Impl(rRootNotifier, aTreecasters);
        }
        // -------------------------------------------------------------------------------------------

        void MultiTreeBroadcaster_Impl::doQueryConstraints(NodeChangesInformation const& aChanges)
        {
            for(BroadcasterList::iterator it = m_aBroadcasters.begin(); it != m_aBroadcasters.end(); ++it)
            {
                NodeChangesInformation aSelectedInfos;
                if ( selectChanges(aSelectedInfos, aChanges, (*it)->getNotifierData()) )
                    (*it)->queryConstraints(aSelectedInfos);
            }
        }
        // -------------------------------------------------------------------------------------------

        void MultiTreeBroadcaster_Impl::doNotifyListeners(NodeChangesInformation const& aChanges)
        {
            for(BroadcasterList::iterator it = m_aBroadcasters.begin(); it != m_aBroadcasters.end(); ++it)
            {
                NodeChangesInformation aSelectedInfos;
                if ( selectChanges(aSelectedInfos, aChanges, (*it)->getNotifierData()) )
                    (*it)->notifyListeners(aSelectedInfos);
            }
        }
    // -----------------------------------------------------------------------------------------------
    }
// ---------------------------------------------------------------------------------------------------

    BroadcasterImplRef Broadcaster::Impl::create(NotifierHolder const& rNotifierImpl, ApiTreeRef const& pTreeImpl, NodeChange const& aChange, bool bLocal)
    {
        OSL_ASSERT(pTreeImpl.is());

        BroadcasterImplRef pRet;

        NodeChangeLocation aLocation;
        if (aChange.getChangeLocation(aLocation))
        {
            if (bLocal)
            {
                pRet = SingleChangeBroadcaster_Impl::create( NotifierData(rNotifierImpl,pTreeImpl), aLocation);
            }
            else
            {
                NotifierData aAffectedNotifier( findNotifier(aLocation, pTreeImpl) );
                if (aAffectedNotifier.second.is()) // only if we found a notifier we are able to create a broadcaster (DG)
                    pRet = SingleChangeBroadcaster_Impl::create( aAffectedNotifier, aLocation);
            }
        }
        else
        {
            OSL_ENSURE(false, "Invalid change location set in node change - cannot broadcast");
            // can't create a matching change - must still create an empty one
        }

        if (pRet.isEmpty())
            pRet = EmptyBroadcaster_Impl::create( NotifierData(rNotifierImpl,pTreeImpl) );

        return pRet;
    }
// ---------------------------------------------------------------------------------------------------

    BroadcasterImplRef Broadcaster::Impl::create(NotifierHolder const& rNotifierImpl, ApiTreeRef const& pTreeImpl, NodeChanges const& aChanges, bool bLocal)
    {
        NotifierData aRootData(rNotifierImpl, pTreeImpl);

        NodeChangesInformation aChangeInfos;
        if (aChanges.getChangesInfos(aChangeInfos))
        {
            return create(rNotifierImpl,pTreeImpl,aChangeInfos,bLocal);
        }
        else
        {
            OSL_ENSURE(aChanges.isEmpty(), "Cannot get information for changes - cannot notify");

            // make an empty one below
            BroadcasterImplRef pRet = EmptyBroadcaster_Impl::create( aRootData );

            return pRet;
        }

    }
// ---------------------------------------------------------------------------------------------------

    BroadcasterImplRef Broadcaster::Impl::create(NotifierHolder const& rNotifierImpl, ApiTreeRef const& pTreeImpl, NodeChangeInformation const& aChange, bool bLocal)
    {
        OSL_ASSERT(pTreeImpl.is());

        BroadcasterImplRef pRet;

        if (aChange.hasValidLocation())
        {
            if (bLocal)
            {
                pRet = SingleChangeBroadcaster_Impl::create( NotifierData(rNotifierImpl,pTreeImpl), aChange.location);
            }
            else
            {
                NotifierData aAffectedNotifier( findNotifier(aChange.location, pTreeImpl) );
                if (aAffectedNotifier.second.is()) // only if we found a notifier we are able to create a broadcaster (DG)
                    pRet = SingleChangeBroadcaster_Impl::create( aAffectedNotifier, aChange.location);
            }
        }
        else
        {
            OSL_ENSURE(false, "Invalid change location set in node change - cannot broadcast");
            // can't create a matching change - must still create an empty one
        }

        if (pRet.isEmpty())
            pRet = EmptyBroadcaster_Impl::create( NotifierData(rNotifierImpl,pTreeImpl) );

        return pRet;
    }
// ---------------------------------------------------------------------------------------------------

    BroadcasterImplRef Broadcaster::Impl::create(NotifierHolder const& rNotifierImpl, ApiTreeRef const& pTreeImpl, NodeChangesInformation const& aChanges, bool bLocal)
    {
        BroadcasterImplRef pRet;

        NotifierData aRootData(rNotifierImpl, pTreeImpl);

        if (aChanges.size() == 1)
        {
            pRet = create(rNotifierImpl, pTreeImpl, *aChanges.begin(), bLocal);
        }
        else if (bLocal)
        {
            pRet = MultiChangeBroadcaster_Impl::create( aRootData, aChanges);
        }
        else
        {
            NotifierSet aNotifiers;
            findNotifiers( aNotifiers, aChanges, pTreeImpl);

            if (aNotifiers.size() > 1)
            {
                pRet = MultiTreeBroadcaster_Impl::create(aRootData, aNotifiers, aChanges);
            }
            else if (!aNotifiers.empty())
            {
                pRet = SingleTreeBroadcaster_Impl::create(aRootData, *aNotifiers.begin(), aChanges);
            }
            // else: empty
        }

        if (pRet.isEmpty())
            pRet = EmptyBroadcaster_Impl::create( aRootData );

        return pRet;
    }
// ---------------------------------------------------------------------------------------------------

       bool Broadcaster::Impl::translateChanges(NodeChangesInformation& _rInfos, NodeChanges const& aChanges, bool /*bSingleBase*/) const
    {
        TreeRef aBaseTree = m_aNotifierData.second->getTree();
        Factory& rFactory = m_aNotifierData.second->getFactory();

        NodeChangesInformation aRawInfos;

        sal_uInt32 nChanges = aChanges.getChangesInfos(aRawInfos);

        OSL_ENSURE(nChanges, "Cannot get info(s) for change - skipping for notification");
        OSL_ENSURE(nChanges == aRawInfos.size(), "Incorrect change count returned");

        NodeChangesInformation aNewInfos;
        aNewInfos.reserve(nChanges);

        // enabling the Single base optimization requires a base node (not only a base tree) for correct accessors
        //if (!bSingleBase || !configuration::equalTree(aBaseTree,aNewChange.info.baseTree))

        for (NodeChangesInformation::Iterator pos = aRawInfos.begin(); pos != aRawInfos.end(); ++pos)
        {
            NodeChangeInformation aInfo = *pos;

            if( !configapi::rebaseChange(aInfo.accessor,aInfo.location,aBaseTree) )
            {
                OSL_TRACE("Change is not within expected tree - skipping for notification");
                continue;
            }

            OSL_ENSURE(!pos->isEmptyChange(), "Empty Change Found for Notification");
            // it actually is expected that elements may not be found - thus ignoring result
            configapi::resolveToUno(aInfo.change, aInfo.accessor, rFactory);

            aNewInfos.push_back( aInfo );
        }

        aNewInfos.swap(_rInfos);

        return !_rInfos.empty();
    }

// ---------------------------------------------------------------------------------------------------

       bool Broadcaster::Impl::translateChanges(NodeChangesInformation& aInfos, NodeChangesInformation const& aChanges, bool /*bSingleBase*/) const
    {
        NodeChangesInformation aNewInfos;
        aNewInfos.reserve( aChanges.size() );

        TreeRef aBaseTree = m_aNotifierData.second->getTree();
        Factory& rFactory = m_aNotifierData.second->getFactory();

        for (NodeChangesInformation::Iterator it = aChanges.begin(); it != aChanges.end(); ++it)
        {
            NodeChangeInformation aInfo(*it);

            // enabling the Single base optimization requires a base node (not only a base tree) for correct accessors
            //if (!bSingleBase || !configuration::equalTree(aBaseTree,aNewChange.info.baseTree))
            if( !configapi::rebaseChange(aInfo.accessor,aInfo.location,aBaseTree) )
            {
                OSL_TRACE("Change is not within expected tree - skipping for notification");
                continue;
            }

            if( !configapi::resolveToUno(aInfo.change,aInfo.accessor,rFactory) )
            {
                // it actually is expected that elements may not be found
                // OSL_TRACE("Cannot find affected elements of Change");
            }

            aNewInfos.push_back( aInfo );
        }

        aNewInfos.swap(aInfos);
        return !aInfos.empty();
    }

// ---------------------------------------------------------------------------------------------------
    void Broadcaster::Impl::notifyRootListeners(NodeChangesInformation const& aChanges)
    {
        if (aChanges.empty()) return;

        ApiTreeRef pRootTree( m_aNotifierData.second->getRootTreeImpl() );
        if (pRootTree.is())
        {
            osl::ClearableMutexGuard aGuardRoot( pRootTree->getApiLock() );

            NotifierHolder aRootNotifier = BroadcasterHelper::getImpl(pRootTree->getNotifier());
            if (aRootNotifier.isValid())
            {
                uno::Reference< css::util::XChangesListener > const * const pSelect = 0;

                NodeID aNotifiedNode = makeRootID( pRootTree );

                if (ListenerContainer* pContainer = aRootNotifier->m_aListeners.getContainer(aNotifiedNode.toIndex(), ::getCppuType(pSelect)) )
                {
                    css::util::ChangesEvent aEvent;
                    aEvent.Source = pRootTree->getUnoInstance();

                    UnoInterfaceRef xBaseInstance = m_aNotifierData.second->getUnoInstance();
                    aEvent.Base <<= xBaseInstance;

                    // translate and collect the changes
                    aEvent.Changes.realloc(aChanges.size());
                    css::util::ElementChange* pChange = aEvent.Changes.getArray();

                    for (NodeChangesInformation::Iterator it = aChanges.begin(); it != aChanges.end(); ++it)
                    {
                        fillChangeFromResolved(*pChange, *it);
                        ++pChange;
                    }

                    // now notify
                    ListenerContainerIterator< css::util::XChangesListener > aIter(*pContainer);
                    aGuardRoot.clear();

                    while (aIter.hasMoreElements())
                        try { aIter.next()->changesOccurred(aEvent); } catch (uno::Exception & ) {}
                }
            }
        }
    }

// ---------------------------------------------------------------------------------------------------
// class Broadcaster
// ---------------------------------------------------------------------------------------------------
Broadcaster::Broadcaster(Notifier const& aNotifier, NodeChange const& aChange, bool bLocal)
: m_pImpl( Impl::create(aNotifier.m_aImpl,ApiTreeRef(aNotifier.m_pTree),aChange,bLocal) )
{
    OSL_ASSERT(m_pImpl.isValid());
}
// ---------------------------------------------------------------------------------------------------
Broadcaster::Broadcaster(Notifier const& aNotifier, NodeChanges const& aChanges, bool bLocal)
: m_pImpl( Impl::create(aNotifier.m_aImpl,ApiTreeRef(aNotifier.m_pTree),aChanges,bLocal) )
{
    OSL_ASSERT(m_pImpl.isValid());
}
// ---------------------------------------------------------------------------------------------------
Broadcaster::Broadcaster(Notifier const& aNotifier, NodeChangeInformation const& aChange, bool bLocal)
: m_pImpl( Impl::create(aNotifier.m_aImpl,ApiTreeRef(aNotifier.m_pTree),aChange,bLocal) )
{
    OSL_ASSERT(m_pImpl.isValid());
}
// ---------------------------------------------------------------------------------------------------
Broadcaster::Broadcaster(Notifier const& aNotifier, NodeChangesInformation const& aChanges, bool bLocal)
: m_pImpl( Impl::create(aNotifier.m_aImpl,ApiTreeRef(aNotifier.m_pTree),aChanges,bLocal) )
{
    OSL_ASSERT(m_pImpl.isValid());
}
// ---------------------------------------------------------------------------------------------------

Broadcaster::Broadcaster(Broadcaster const& aOther)
: m_pImpl(aOther.m_pImpl)
{
    OSL_ASSERT(m_pImpl.isValid());
}
// ---------------------------------------------------------------------------------------------------

Broadcaster::~Broadcaster()
{
}
// ---------------------------------------------------------------------------------------------------

void Broadcaster::queryConstraints(NodeChange const& aChange) throw(beans::PropertyVetoException)
{
    OSL_ENSURE(aChange.isChange(),"Constraint query without a change !");

    NodeChanges aChanges;
    aChanges.add(aChange);
    this->queryConstraints(aChanges,true);
}
// ---------------------------------------------------------------------------------------------------

void Broadcaster::queryConstraints(NodeChanges const& aChanges, bool bSingleBase) throw(beans::PropertyVetoException)
{
    OSL_ENSURE(!aChanges.isEmpty(),"Constraint query without a change !");

    try
    {
        NodeChangesInformation aInfos;
        if (m_pImpl->translateChanges(aInfos,aChanges,bSingleBase))
        {
            m_pImpl->queryConstraints(aInfos);
        }
    }
    catch (beans::PropertyVetoException & )
    {
        throw;
    }
    catch (uno::Exception & )
    {
        OSL_ENSURE(false, "configmgr::Broadcaster: Unexpected UNO exception in notifyListeners");
    }
    catch (configuration::Exception & )
    {
        OSL_ENSURE(false, "configmgr::Broadcaster: Unexpected internal exception in notifyListeners");
    }
}
// ---------------------------------------------------------------------------------------------------

void Broadcaster::notifyListeners(NodeChange const& aChange) throw()
{
    OSL_ENSURE(aChange.isChange(),"Notifying without a change !");

    NodeChanges aChanges;
    aChanges.add(aChange);
    this->notifyListeners(aChanges, true);
}
// ---------------------------------------------------------------------------------------------------

void Broadcaster::notifyListeners(NodeChangeInformation const& aChange) throw()
{
    OSL_ENSURE(!aChange.isEmptyChange(),"Notifying without a change !");

    NodeChangesInformation aChanges;
    aChanges.push_back(aChange);
    this->notifyListeners(aChanges, true);
}
// ---------------------------------------------------------------------------------------------------

void Broadcaster::notifyListeners(NodeChanges const& aChanges, bool bSingleBase) throw()
{
    OSL_ENSURE(!aChanges.isEmpty(),"Notifying without a change !");

    try
    {
        NodeChangesInformation aInfos;
        if (m_pImpl->translateChanges(aInfos,aChanges, bSingleBase))
        {
            m_pImpl->notifyListeners(aInfos);
            m_pImpl->notifyRootListeners(aInfos);
        }
    }
    catch (uno::Exception & )
    {
        OSL_ENSURE(false, "configmgr::Broadcaster: Unexpected UNO exception in notifyListeners");
    }
    catch (configuration::Exception & )
    {
        OSL_ENSURE(false, "configmgr::Broadcaster: Unexpected internal exception in notifyListeners");
    }
}
// ---------------------------------------------------------------------------------------------------

void Broadcaster::notifyListeners(NodeChangesInformation const& aChanges, bool bSingleBase) throw()
{
    OSL_ENSURE(!aChanges.empty(),"Notifying without a change !");

    try
    {
        NodeChangesInformation aInfos;
        if (m_pImpl->translateChanges(aInfos,aChanges, bSingleBase))
        {
            m_pImpl->notifyListeners(aInfos);
            m_pImpl->notifyRootListeners(aInfos);
        }
    }
    catch (uno::Exception & )
    {
        OSL_ENSURE(false, "configmgr::Broadcaster: Unexpected UNO exception in notifyListeners");
    }
    catch (configuration::Exception & )
    {
        OSL_ENSURE(false, "configmgr::Broadcaster: Unexpected internal exception in notifyListeners");
    }
}
// ---------------------------------------------------------------------------------------------------
    }
}
