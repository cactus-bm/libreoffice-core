/*************************************************************************
 *
 *  $RCSfile: apitreeimplobj.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: jb $ $Date: 2000-11-13 14:38:58 $
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
#include <stdio.h>
#include "apitreeimplobj.hxx"
#include "confproviderimpl2.hxx"
#include "confignotifier.hxx"
#include "notifierimpl.hxx"
#include "apifactory.hxx"
#include "apitreeaccess.hxx"

#include <cppuhelper/queryinterface.hxx>

namespace configmgr
{
//-----------------------------------------------------------------------------
    namespace configapi
    {
//-----------------------------------------------------------------------------
        class Factory;
        class Notifier;
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// API object implementation wrappers
//-------------------------------------------------------------------------
configuration::TemplateProvider ApiProvider::getTemplateProvider() const
{
    return TemplateProvider(&m_rProviderImpl);
}
//-------------------------------------------------------------------------

UnoTypeConverter ApiProvider::getTypeConverter() const
{
    return m_rProviderImpl.getTypeConverter();
}
//-------------------------------------------------------------------------

ISynchronizedData* ApiProvider::getSourceLock() const
{
    return &m_rProviderImpl;
}
//-------------------------------------------------------------------------

ApiTreeImpl::ApiTreeImpl(UnoInterface* pInstance, Tree const& aTree, ApiTreeImpl& rParentTree)
: m_pInstance(pInstance)
, m_aTree(aTree)
, m_rProvider(rParentTree.getProvider())
, m_pParentTree(0)
, m_aNotifier(new NotifierImpl(aTree))
{
    setNodeInstance(aTree.getRootNode(), pInstance);
    init(&rParentTree);
}
//-------------------------------------------------------------------------
ApiTreeImpl::ApiTreeImpl(UnoInterface* pInstance, ApiProvider& rProvider, Tree const& aTree, ApiTreeImpl* pParentTree)
: m_pInstance(pInstance)
, m_aTree(aTree)
, m_rProvider(rProvider)
, m_pParentTree(0)
, m_aNotifier(new NotifierImpl(aTree))
{
    OSL_ENSURE(!pParentTree || &rProvider == &pParentTree->m_rProvider,"WARNING: Parent tree has a different provider - trouble may be ahead");
    setNodeInstance(aTree.getRootNode(), pInstance);
    init(pParentTree);
}
//-------------------------------------------------------------------------

ApiTreeImpl::~ApiTreeImpl()
{
    OSL_ENSURE(m_aNotifier->m_aListeners.isDisposed(),"ApiTree Object was not disposed properly");
    deinit();
}
//-------------------------------------------------------------------------
ApiRootTreeImpl::ApiRootTreeImpl(UnoInterface* pInstance, ApiProvider& rProvider, Tree const& aTree)
: m_aTreeImpl(pInstance, rProvider, aTree, 0)
, m_pNotificationSource(0)
{
    implSetLocation();
    enableNotification(true);
}
//-------------------------------------------------------------------------
ApiRootTreeImpl::~ApiRootTreeImpl()
{
    implSetNotificationSource(0);
}
//-------------------------------------------------------------------------

void ApiTreeImpl::setNodeInstance(configuration::NodeRef const& aNode, UnoInterface* pInstance)
{
    OSL_ENSURE(aNode.isValid(),"ERROR: adding invalid node to ApiTree");
    OSL_ENSURE(m_aTree.isValidNode(aNode),"ERROR: foreign node being added to ApiTree");
    m_aNotifier->m_aListeners.setObjectAt( configuration::NodeID(m_aTree, aNode).toIndex(), pInstance );
}

//-------------------------------------------------------------------------

void ApiTreeImpl::checkAlive() const
{
    bool bAlive = m_aNotifier->m_aListeners.checkAlive();
    if (!bAlive)
    {
        OUString sMessage(RTL_CONSTASCII_USTRINGPARAM("CONFIGURATION: Object was disposed"));
        throw com::sun::star::lang::DisposedException(sMessage, getUnoInstance());
    }
}
//-------------------------------------------------------------------------

osl::Mutex& ApiTreeImpl::getApiLock() const
{
    return m_aNotifier->mutex();
}
//-------------------------------------------------------------------------

Notifier ApiTreeImpl::getNotifier() const
{
    return Notifier(m_aNotifier,m_aTree);
}
//-------------------------------------------------------------------------

bool ApiRootTreeImpl::enableNotification(bool bEnable)
{
    IConfigBroadcaster* pSource = bEnable ? getApiTree().getProvider().getProviderImpl().getNotifier() : 0;

    IConfigBroadcaster* pOld = this->implSetNotificationSource(pSource);

    return pOld != 0;
}
//-------------------------------------------------------------------------

bool ApiTreeImpl::disposeTree(bool bForce)
{
    OWriteSynchronized aLocalGuard(getDataLock());
    if (!bForce)
    {
        if (m_pParentTree != 0)
            return false;
    }
    implDisposeTree();
    return true;
}

//-------------------------------------------------------------------------
bool ApiRootTreeImpl::disposeTree()
{
    implSetNotificationSource(0);
    return m_aTreeImpl.disposeTree(true);
}
//-------------------------------------------------------------------------
void ApiTreeImpl::implDisposeTree()
{
    OSL_ENSURE(m_pParentTree == 0,"WARNING: Disposing a tree that still has a parent tree set");

    NotifierImpl::SpecialContainer& aContainer = m_aNotifier->m_aListeners;
    if (aContainer.beginDisposing())
    {
        using configuration::NodeIDList;
        using configuration::NodeID;
        using configuration::getAllContainedNodes;
        using com::sun::star::lang::EventObject;

        Factory& rFactory = getFactory();

        NodeIDList aChildNodes;
        getAllContainedNodes(m_aTree, aChildNodes);

        for (NodeIDList::reverse_iterator it = aChildNodes.rbegin(), stop = aChildNodes.rend();
            it != stop;
            ++it)
        {
            rFactory.revokeElement( *it );
        }

        aContainer.notifyDisposing();

        OSL_ASSERT(!aContainer.isDisposed());

        deinit(); // releases the provider and parent
        aContainer.endDisposing();

        OSL_ASSERT(aContainer.isDisposed());
    }
}
//-------------------------------------------------------------------------
void ApiTreeImpl::disposeNode(NodeRef const& aNode, UnoInterface* pInstance)
{
    OWriteSynchronized aLocalGuard(getDataLock());
    implDisposeNode(aNode,pInstance);
}
//-------------------------------------------------------------------------
void ApiTreeImpl::implDisposeNode(NodeRef const& aNode, UnoInterface* pInstance)
{
    OSL_ENSURE(aNode.isValid(),"INTERNAL ERROR: Disposing NULL node");
    OSL_ENSURE(m_aTree.isValidNode(aNode),"INTERNAL ERROR: Disposing: node does not match tree");
    OSL_ENSURE( !m_aTree.isRootNode(aNode),"INTERNAL ERROR: Disposing the root node of the tree");

    using configuration::NodeID;
    using com::sun::star::lang::EventObject;

    NodeID aNodeID(m_aTree,aNode);

    if (m_aNotifier->m_aListeners.disposeOne(aNodeID.toIndex()) )
    {
        getFactory().revokeElement(aNodeID);
    }
}
//-------------------------------------------------------------------------
void ApiTreeImpl::init(ApiTreeImpl* pParentTree)
{
    m_xProvider = getProviderComponent();
    OSL_ENSURE(m_xProvider.is(),"WARNING: Provider is no Component - Lifetime trouble ahead");

    if (m_xProvider.is())
    {
        m_xProvider->addEventListener(this);
        m_xProvider->acquire();
    }

    OSL_ASSERT(m_pParentTree == 0);
    setParentTree(pParentTree);
}
//-------------------------------------------------------------------------
void ApiTreeImpl::deinit()
{
    setParentTree(0);
    OSL_ENSURE(m_xProvider.is(),"WARNING: Provider is no Component - did we still survive ?");

    if (m_xProvider.is())
    {
        if (m_xProvider.is())
        {
            m_xProvider->removeEventListener(this);
            m_xProvider->release();
        }
    }
}
//-------------------------------------------------------------------------
void ApiTreeImpl::haveNewParent(ApiTreeImpl* pNewParent) // public interface
{
    setParentTree(pNewParent);
}

//-------------------------------------------------------------------------
void ApiTreeImpl::setParentTree(ApiTreeImpl*    pParentTree) // internal implementation
{
    osl::MutexGuard aLock(getApiLock());

#ifdef _DEBUG
    if (pParentTree)
    {
        using configuration::NodeID;
        Tree aContext = m_aTree.getContextTree();
        Tree aParent = pParentTree->m_aTree;

        NodeID aContextID( aContext, aContext.getRootNode() );
        NodeID aParentID( aParent, aParent.getRootNode() );

        OSL_ENSURE( aContextID == aParentID, "Parent relationship mismatch !");
    }
#endif

    if (m_pParentTree != pParentTree)
    {
        uno::Reference<com::sun::star::lang::XComponent> xOld = getParentComponent();
        if (xOld.is())  xOld->removeEventListener(this);

        m_pParentTree = pParentTree;

        uno::Reference<com::sun::star::lang::XComponent> xNew = getParentComponent();
        if (xNew.is())  xNew->addEventListener(this);

        if (xNew.is())  xNew->acquire();
        if (xOld.is())  xOld->release();

        OSL_ENSURE( xNew.is() == (pParentTree != 0), "WARNING: Parent Tree is no Component");
    }
}
//-------------------------------------------------------------------------

uno::Reference<com::sun::star::lang::XComponent> ApiTreeImpl::getParentComponent()
{
    uno::XInterface* pInterface = m_pParentTree ? m_pParentTree->getUnoInstance() : 0;
    return uno::Reference<com::sun::star::lang::XComponent>::query(pInterface);
}
//-------------------------------------------------------------------------

uno::Reference<com::sun::star::lang::XComponent> ApiTreeImpl::getProviderComponent()
{
    uno::XInterface* pInterface = m_rProvider.getProviderImpl().getProviderInstance();
    return uno::Reference<com::sun::star::lang::XComponent>::query(pInterface);
}
//-------------------------------------------------------------------------

void SAL_CALL ApiTreeImpl::acquire() throw()
{
    // TODO add debug counting
}
//-------------------------------------------------------------------------
void SAL_CALL ApiTreeImpl::release() throw()
{
    // TODO add debug counting
}
//-------------------------------------------------------------------------
uno::Any SAL_CALL ApiTreeImpl::queryInterface(uno::Type const& rType) throw()
{
    return cppu::queryInterface( rType
                , static_cast< com::sun::star::lang::XEventListener*>(this)
                , static_cast< uno::XInterface*>(this)
            );
}
//-------------------------------------------------------------------------

void SAL_CALL ApiTreeImpl::disposing(com::sun::star::lang::EventObject const& rEvt) throw()
{
    // Tree write Lock should be set by sender
    setParentTree(0);
    implDisposeTree();

    // uno::Reference<com::sun::star::lang::XComponent> xThis(getUnoInstance(),UNO_QUERY);
    // if (xThis.is()) xThis->dispose();
}
//-------------------------------------------------------------------------
IConfigBroadcaster* ApiRootTreeImpl::implSetNotificationSource(IConfigBroadcaster* pNew)
{
    osl::MutexGuard aGuard(getApiTree().getApiLock());

    IConfigBroadcaster* pOld = m_pNotificationSource;
    if (pOld != pNew)
    {
        if (pOld)
            pOld->removeListener(this);

        if (pNew)
        {
            OSL_ENSURE(m_aLocationPath.getLength() > 0, "Cannot register for notifications: no location set");
            pNew->addListener(m_aLocationPath, this);
        }
        m_pNotificationSource = pNew;
    }
    return pOld;
}
// ---------------------------------------------------------------------------------------------------

void ApiRootTreeImpl::implSetLocation()
{
    osl::MutexGuard aGuard(getApiTree().getApiLock());

    Tree aTree = getApiTree().getTree();
    if (!aTree.isEmpty())
    {
        configuration::Name aRootName = aTree.getRootNode().getName();
        m_aLocationPath = aTree.getContextPath().compose( configuration::RelativePath(aRootName) ).toString();
    }
    else
    {
        m_aLocationPath = OUString();
    }

    if (m_pNotificationSource)
    {
        OSL_ENSURE(m_aLocationPath.getLength() > 0, "Cannot reregister for notifications: setting empty location");

        m_pNotificationSource->removeListener(this);
        m_pNotificationSource->addListener(m_aLocationPath, this);
    }
}
// ---------------------------------------------------------------------------------------------------

void ApiRootTreeImpl::disposing(IConfigBroadcaster* pSource)
{
    OSL_ASSERT(pSource == m_pNotificationSource);
    m_pNotificationSource = 0;

    m_aTreeImpl.disposeTree(true);
}
// ---------------------------------------------------------------------------------------------------

//INodeListener : IConfigListener
void ApiRootTreeImpl::nodeChanged(Change const& aChange, OUString const& aPath, IConfigBroadcaster* pSource)
{
}
// ---------------------------------------------------------------------------------------------------

void ApiRootTreeImpl::nodeDeleted(OUString const& aPath, IConfigBroadcaster* pSource)
{
}

// ---------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
    }
}

