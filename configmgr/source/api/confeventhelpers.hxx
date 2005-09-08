/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: confeventhelpers.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 03:03:44 $
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
#ifndef CONFIGMGR_API_EVENTHELPERS_HXX_
#define CONFIGMGR_API_EVENTHELPERS_HXX_

#ifndef CONFIGMGR_API_EVENTS_HXX_
#include "confevents.hxx"
#endif

#ifndef CONFIGMGR_CONFIGPATH_HXX_
#include "configpath.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HPP_
#include <com/sun/star/uno/RuntimeException.hpp>
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef INCLUDED_MAP
#include <map>
#define INCLUDED_MAP
#endif
#ifndef INCLUDED_FUNCTIONAL
#include <functional>
#define INCLUDED_FUNCTIONAL
#endif
#include <hash_set>
#ifndef INCLUDED_SET
#include <set>
#define INCLUDED_SET
#endif

namespace configmgr
{
    namespace internal
    {

        using namespace configuration;
    ////////////////////////////////////////////////////////////////////////
        template <class ListenerRef>
        class BroadcastImplHelper
        {
        public:
            osl::Mutex mutex;

        public:
            BroadcastImplHelper()
            {}

            ~BroadcastImplHelper()
            {
                OSL_ENSURE(m_aInterfaces.empty(), "Configuration Broadcaster was not disposed properly");
            }

        public:
            typedef std::set<ListenerRef> Interfaces;
            typedef typename Interfaces::iterator FullIterator;
            typedef typename Interfaces::const_iterator Iterator;

        public:
            FullIterator addListener(ListenerRef aListener)
            {
                return m_aInterfaces.insert(aListener).first;
            }
            void removeListener(ListenerRef aListener)
            {
                m_aInterfaces.erase(aListener);
            }

            void disposing(IConfigBroadcaster* pSource);

        public:
            Iterator begin() const { return m_aInterfaces.begin(); }
            Iterator end() const { return m_aInterfaces.end(); }

            Iterator find(ListenerRef aListener) const   { return m_aInterfaces.find(aListener); }
            FullIterator findFull(ListenerRef aListener) { return m_aInterfaces.find(aListener); }
        private:
            Interfaces m_aInterfaces;

            // no implementation - not copyable
            BroadcastImplHelper(BroadcastImplHelper&);
            void operator=(BroadcastImplHelper&);
        };

        ////////////////////////////////////////////////////////////////////////
        template <class Listener>
        void BroadcastImplHelper<Listener>::disposing(IConfigBroadcaster* pSource)
        {
            osl::ClearableMutexGuard aGuard(this->mutex);   // ensure that no notifications are running

            Interfaces aTargets;
            aTargets.swap(m_aInterfaces);

            aGuard.clear();
            for(FullIterator it = aTargets.begin(); it != aTargets.end(); )
            {
                FullIterator cur = it++;
                if (*cur)
                    (*cur)->disposing(pSource);
            }
        }


    /////////////////////////////////////////////////////////////////////////

        class NodeListenerInfo
        {
        public:
            typedef std::hash_set<AbsolutePath, Path::Hash, Path::Equiv> Pathes;

        public:
            NodeListenerInfo(INodeListenerRef const&    pListener)
                : m_pListener(pListener)
            {
            }

        // path handling
            Pathes const& pathList() const { return m_aPathes; }

            void addPath(AbsolutePath const& sPath) const { m_aPathes.insert(sPath); }
            void removePath(AbsolutePath const& sPath) const { m_aPathes.erase(sPath); }
            //void removeChildPathes(OUString const& sPath);

        // behave as pointer for use as a 'reference' class
            INodeListenerRef get() const { return m_pListener; }
            INodeListenerRef operator->() const { return get(); }
            INodeListener& operator*() const { return *m_pListener; }
        // needed to allow if (info) ...
            struct HasListener;
            operator HasListener const*() const { return reinterpret_cast<HasListener*>(m_pListener.get()); }

            bool operator < (NodeListenerInfo const& aInfo) const
            { return std::less<INodeListener*>()(m_pListener.get(), aInfo.m_pListener.get()); }

            bool operator == (NodeListenerInfo const& aInfo) const
            { return !!( m_pListener == aInfo.m_pListener); }

            bool operator > (NodeListenerInfo const& aInfo) const
            { return aInfo.operator < (*this); }
            bool operator >= (NodeListenerInfo const& aInfo) const
            { return !operator<(aInfo); }
            bool operator <= (NodeListenerInfo const& aInfo) const
            { return !operator>(aInfo); }

            bool operator != (NodeListenerInfo const& aInfo) const
            { return !operator==(aInfo); }

        private:
            INodeListenerRef m_pListener;
            mutable Pathes m_aPathes; // hack to be mutable even as set element
        };
        class ConfigChangesBroadcasterImpl
        {
        public:
            ConfigChangesBroadcasterImpl();
            ~ConfigChangesBroadcasterImpl();

            void add(AbsolutePath const& aPath, INodeListenerRef const& pListener);
            void remove(INodeListenerRef const& pListener);

//          void removed(OUString const& aPath, bool bRemovedFromModel, IConfigBroadcaster* pSource);

            void dispatch(memory::Accessor const& _aChangedDataAccessor, Change const& rBaseChange, AbsolutePath const& sChangeLocation, sal_Bool _bError, IConfigBroadcaster* pSource);
            void dispatch(memory::Accessor const& _aChangedDataAccessor, TreeChangeList const& rList_, sal_Bool _bError, IConfigBroadcaster* pSource);
            void disposing(IConfigBroadcaster* pSource);
        private:
            typedef BroadcastImplHelper<NodeListenerInfo> Listeners;
            typedef Listeners::FullIterator InfoRef;
            typedef std::multimap<AbsolutePath, InfoRef, Path::Before> PathMap;
            Listeners m_aListeners;
            PathMap m_aPathMap;
        private:
            void dispatchInner(INodeListenerRef const& pTarget, AbsolutePath const& sTargetPath, memory::Accessor const& _aChangedDataAccessor, Change const& rBaseChange, AbsolutePath const& sChangeLocation, sal_Bool _bError, IConfigBroadcaster* pSource);
            void dispatchOuter(INodeListenerRef const& pTarget, AbsolutePath const& sTargetPath, memory::Accessor const& _aChangedDataAccessor, Change const& rBaseChange, AbsolutePath const& sChangeLocation, sal_Bool _bError, IConfigBroadcaster* pSource);
        };

    /////////////////////////////////////////////////////////////////////////
    } // namespace
} // namespace

#endif // CONFIGMGR_API_EVENTHELPERS_HXX_


