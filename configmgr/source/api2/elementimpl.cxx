/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: elementimpl.cxx,v $
 * $Revision: 1.17.14.1 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_configmgr.hxx"

#include "elementimpl.hxx"
#include "apitreeaccess.hxx"
#include "apinodeaccess.hxx"
#include "apifactory.hxx"
#include "noderef.hxx"
#include "nodechange.hxx"
#include "nodechangeinfo.hxx"
#include "translatechanges.hxx"
#include "apitypes.hxx"
#include "configset.hxx"
#include "confignotifier.hxx"
#include "confsvccomponent.hxx"
#include "committer.hxx"
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/configuration/XTemplateContainer.hpp>
#include <osl/diagnose.h>

namespace configmgr
{
    namespace configapi
    {
//-----------------------------------------------------------------------------------
        namespace lang = css::lang;
        namespace util = css::util;

        using uno::RuntimeException;
        using uno::Reference;
        using uno::Any;
        using uno::Sequence;
        using lang::NoSupportException;
        using lang::WrappedTargetException;

        using configuration::NodeRef;
        using configuration::Tree;
        using configuration::ElementTree;
        using configuration::Name;
        using configuration::AbsolutePath;
        using configuration::RelativePath;

//-----------------------------------------------------------------------------------

// Interface methods
//-----------------------------------------------------------------------------------

// XChild
//-----------------------------------------------------------------------------------

Reference< uno::XInterface > implGetParent(NodeAccess& rNode, InnerElement&) throw(RuntimeException)
{
    Reference<uno::XInterface> xRet;

    try
    {
        GuardedNodeDataAccess impl( rNode ); // no provider lock needed - tree must be prebuilt already

        Tree aTree(impl.getTree());
        NodeRef aParentNode = aTree.getParent(impl.getNode());

        Any aAny = configapi::makeInnerElement( rNode.getFactory(), aTree, aParentNode );

        if (!(aAny >>= xRet)) // no parent available
        {
            OSL_ASSERT(!xRet.is()); // make sure we return NULL
            OSL_ENSURE(!aAny.hasValue(), "configmgr: BasicElement::getParent: could not extract parent - node is not an object");
        }
    }
    catch (configuration::Exception& ex)
    {
        ExceptionMapper e(ex);
        e.setContext( rNode.getUnoInstance() );
        e.unhandled();
    }

    return xRet;
}

//-----------------------------------------------------------------------------------

Reference< uno::XInterface > implGetParent(NodeAccess& rNode, SetElement& /*rElement*/) throw(RuntimeException)
{
    Reference<uno::XInterface> xRet;

    try
    {
        // assume shared lock for connected trees
        GuardedNodeDataAccess impl( rNode ); // no provider lock needed - tree must be prebuilt already

        Tree aTree(impl.getTree());

        Tree aParentTree( aTree.getContextTree() );

        if (!aParentTree.isEmpty())
        {
            NodeRef aParentNode( aTree.getContextNode() );

            // assume shared factory for connected trees
            Any aAny = configapi::makeInnerElement( rNode.getFactory(), aParentTree, aParentNode );

            if (!(aAny >>= xRet)) // no parent available
            {
                // should occur only if the any is void
                OSL_ENSURE(!aAny.hasValue(), "configmgr: BasicSetElement::getParent: could not extract parent - node is not an object");
                OSL_ASSERT(!xRet.is()); // make sure we return NULL
            }
        }
        else
        {
            OSL_ASSERT(!xRet.is()); // make sure we return NULL
        }
    }
    catch (configuration::Exception& ex)
    {
        ExceptionMapper e(ex);
        e.setContext( rNode.getUnoInstance() );
        e.unhandled();
    }

    return xRet;
}
//-----------------------------------------------------------------------------------

// UNSUPPORTED method
void implSetParent(NodeAccess& rNode, InnerElement& /*rElement*/, const Reference< uno::XInterface >& /*xParent*/ )
    throw(NoSupportException, RuntimeException)
{
    UnoApiLock aLock;

    rNode.checkAlive(); // Does locking internally, checks for disposed nodes

    // TODO(?): allow for xParent == getParent()
    throw NoSupportException(
            OUString(RTL_CONSTASCII_USTRINGPARAM("configmgr: BasicElement::setParent: cannot move Entry")),
            rNode.getUnoInstance() );
}
//-----------------------------------------------------------------------------------

// preliminary implementation
void implSetParent(NodeAccess& rNode, SetElement& rElement, const Reference< uno::XInterface >& xParent )
    throw(NoSupportException, RuntimeException)
{
    UnoApiLock aLock;

    //implSetParent(rNode,xParent);
    // TODO: lock the whole transaction ???? - would need Uno Tunneling ?
    using css::container::XNameContainer;

    Reference< uno::XInterface > xGotParent( implGetParent(rNode,rElement) );
    Reference< XNameContainer > xOldParent( xGotParent, uno::UNO_QUERY );
    Reference< XNameContainer > xNewParent( xParent, uno::UNO_QUERY );

    if (xGotParent.is() && !xOldParent.is())
    {
        throw NoSupportException(
                OUString(RTL_CONSTASCII_USTRINGPARAM("Config API: implSetParent: this is not element of a container")),
                rNode.getUnoInstance() );
    }
    if (xParent.is() && !xNewParent.is())
    {
        throw NoSupportException(
                OUString(RTL_CONSTASCII_USTRINGPARAM("Config API: implSetParent: new parent is no container")),
                rNode.getUnoInstance() );
    }

    Reference< uno::XInterface > xThis(rNode.getUnoInstance());
    OSL_ASSERT(xThis.is());

    if (xOldParent != xNewParent)
    {
        OUString const sName( implGetName(rNode,rElement) );

        if (xParent.is())
        {
            OUString const sTemplate( implGetTemplateName(rElement) );

            if (sTemplate.getLength() == 0)
            {
                throw NoSupportException(
                        OUString(RTL_CONSTASCII_USTRINGPARAM("Config API: implSetParent: object has no recognizable type")),
                        xThis );
            }

            Reference< css::configuration::XTemplateContainer > xNewTemplate( xParent, uno::UNO_QUERY );
            if (!xNewTemplate.is())
            {
                throw NoSupportException(
                        OUString(RTL_CONSTASCII_USTRINGPARAM("Config API: implSetParent: new parent has no element template")),
                        xThis );
            }

            if ( sTemplate != xNewTemplate->getElementTemplateName())
            {
                throw NoSupportException(
                        OUString(RTL_CONSTASCII_USTRINGPARAM("Config API: implSetParent: object has wrong type")),
                        xThis );
            }

            if ( xNewParent->hasByName( sName ) )
            {
                throw NoSupportException(
                        OUString(RTL_CONSTASCII_USTRINGPARAM("Config API: implSetParent: an object of this name already exists in new parent")),
                        xThis );
            }

            // TODO: check for circularity (i.e. that This is not one of new parent's ancestors) !!
        }

        // now do it

        try
        {
            if ( xOldParent.is()) xOldParent->removeByName(sName);
            if ( xNewParent.is()) xNewParent->insertByName(sName, uno::makeAny(xThis));
        }
        catch (uno::Exception& e)
        {
            e.Message = OUString(RTL_CONSTASCII_USTRINGPARAM("Config API: implSetParent: changing parent failed: "))
                += e.Message;

            if (xOldParent.is())
            try
            {
                xOldParent->insertByName(sName, uno::makeAny(xThis));
            }
            catch(uno::Exception& bad)
            {
                e.Message += OUString(RTL_CONSTASCII_USTRINGPARAM("\n\t ! Could not restore old parent: ")) += bad.Message;
            }

            throw RuntimeException(e.Message,xThis);
        }

    }
}
//-----------------------------------------------------------------------------


// XNamed
//-----------------------------------------------------------------------------
OUString implGetName(NodeAccess& rNode, NodeElement& ) throw(RuntimeException)
{
    OUString sRet;
    try
    {
        GuardedNodeDataAccess impl( rNode ); // maybe passive only ?

        Tree    aTree(impl.getTree());
        NodeRef aNode(impl.getNode());

        sRet = aTree.getName(aNode).toString();
    }
    catch (configuration::Exception& ex)
    {
        ExceptionMapper e(ex);
        e.setContext( rNode.getUnoInstance() );
        e.unhandled();
    }

    return sRet;
}
//-----------------------------------------------------------------------------

// UNSUPPORTED method
void implSetName(NodeAccess & rNode, NodeElement& /*rElement*/, const OUString& /*aName*/ ) throw(RuntimeException)
{
    UnoApiLock aLock;

    typedef RuntimeException CantRenameException;

    rNode.checkAlive(); // Does locking internally, checks for disposed nodes

    // TODO(?): allow for aName == getName()
    throw CantRenameException(
        OUString(RTL_CONSTASCII_USTRINGPARAM("configmgr: BasicElement::setName: cannot rename Entry")),
        rNode.getUnoInstance() );
}
//-----------------------------------------------------------------------------

// TODO: Implementations for elements to be added to a container node
void implSetName(NodeAccess& rNode, SetElement& rElement, const OUString& aName ) throw(RuntimeException)
{
    UnoApiLock aLock;

    // TODO: Implement
    NodeElement& rDelegate = rElement;
    implSetName(rNode,rDelegate,aName); // delegate to unsupported version
}
//-----------------------------------------------------------------------------

// XComponent & XInterface
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void implDispose( SetElement& rElement) throw(uno::RuntimeException)
{
    UnoApiLock aLock;

    if (!rElement.disposeTree(false))
    {
        throw uno::RuntimeException(
                OUString(RTL_CONSTASCII_USTRINGPARAM("CONFIGURATION: Can't dispose an object that has an owner")),
                rElement.getUnoInstance() );
    }
}
//-----------------------------------------------------------------------------

void implDispose( RootElement& rElement) throw(uno::RuntimeException)
{
    UnoApiLock aLock;

    if (!rElement.disposeTree())
    {
        throw lang::DisposedException(
                OUString(RTL_CONSTASCII_USTRINGPARAM("CONFIGURATION: Can't dispose an object that already was disposed")),
                rElement.getUnoInstance() );
    }
}
//-----------------------------------------------------------------------------

void implDisposeObject( NodeAccess& ,SetElement& rElement) throw(uno::RuntimeException)
{
    UnoApiLock aLock;
// FIXME: should we hold a ref on the element over this ?
// call apitreeaccess.hxx (doGetUnoInterface) & hold a ref / unref ?
// [or!] - call getApiTree & ref/unref on that ?
// [or!] - hold the ref inside the dispose method itself ...
    rElement.disposeTree(true);
}
//-----------------------------------------------------------------------------

void implDisposeObject( NodeAccess& , RootElement& rElement) throw(uno::RuntimeException)
{
    UnoApiLock aLock;

    rElement.disposeTree();
}
//-----------------------------------------------------------------------------

void implDisposeObject( NodeAccess& rNode, InnerElement& ) throw(uno::RuntimeException)
{
    UnoApiLock aLock;

    rNode.disposeNode();
}
//-----------------------------------------------------------------------------


// XTypeProvider
//-----------------------------------------------------------------------------

uno::Sequence<sal_Int8> implGetImplementationId(NodeAccess& rNode, NodeElement& rElement)
    throw(uno::RuntimeException)
{
    DisposeGuard aLock(rNode);
    ServiceImplementationInfo const* pInfo = rElement.getServiceInfo();

    OSL_ENSURE(pInfo, "Configuration: Object has no implementation (service) info - cannot get implementation id");
    if (!pInfo)
        throw uno::RuntimeException( OUString(RTL_CONSTASCII_USTRINGPARAM("CONFIGURATION: Object has no implementation information - cannot get implementation id")),rElement.getUnoInstance() );

    return ServiceComponentImpl::getStaticImplementationId(pInfo);
}
//-----------------------------------------------------------------------------

// XServiceInfo
//-----------------------------------------------------------------------------

OUString implGetImplementationName( NodeAccess& rNode, NodeElement& rElement ) throw(uno::RuntimeException)
{
    DisposeGuard aLock(rNode);
    ServiceImplementationInfo const* pInfo = rElement.getServiceInfo();
    OSL_ENSURE(pInfo, "Configuration: Object has no service info");

    return ServiceInfoHelper(pInfo).getImplementationName();
}
//-----------------------------------------------------------------------------

sal_Bool implSupportsService( NodeAccess& rNode, NodeElement& rElement, const OUString& ServiceName ) throw(uno::RuntimeException)
{
    DisposeGuard aLock(rNode);
    ServiceImplementationInfo const* pInfo = rElement.getServiceInfo();
    OSL_ENSURE(pInfo, "Configuration: Object has no service info");

    return ServiceInfoHelper(pInfo).supportsService(ServiceName);
}
//-----------------------------------------------------------------------------

uno::Sequence< OUString > implGetSupportedServiceNames( NodeAccess& rNode, NodeElement& rElement ) throw(uno::RuntimeException)
{
    DisposeGuard aLock(rNode);
    ServiceImplementationInfo const* pInfo = rElement.getServiceInfo();
    OSL_ENSURE(pInfo, "Configuration: Object has no service info");

    return ServiceInfoHelper(pInfo).getSupportedServiceNames();
}
//-----------------------------------------------------------------------------

// Root only ------------------------------------------------------------------

// XLocalizable
// TODO: Implement locale support
//-----------------------------------------------------------------------------

lang::Locale implGetLocale( RootElement& rElement ) throw(uno::RuntimeException)
{
    GuardedRootElement aLocked(rElement);

    OSL_ENSURE(false,"CONFIGURATION: Locale information is not yetsupported.");
    return lang::Locale();
}
//-----------------------------------------------------------------------------

void implSetLocale( RootElement& rElement, const css::lang::Locale& /*eLocale*/ ) throw(uno::RuntimeException)
{
    UnoApiLock aLock;
    // TODO: Implement if possible
    rElement.checkAlive();

    OSL_ENSURE(false,"CONFIGURATION: Changing the set Locale is not supported.");
    throw uno::RuntimeException(
        OUString(RTL_CONSTASCII_USTRINGPARAM("CONFIGURATION: Changing the locale is currently not supported")),
        rElement.getUnoInstance()
    );
}
//-----------------------------------------------------------------------------

// XChangesBatch
//-----------------------------------------------------------------------------

void implCommitChanges( UpdateRootElement& rElement ) throw(css::lang::WrappedTargetException, uno::RuntimeException)
{
    UnoApiLock aLock;

    // quick check to avoid big locks for nothing (has its own locking)
    if (!implHasPendingChanges(rElement)) return;

    try
    {
        rElement.getCommitter().commit();
    }

    catch (configuration::Exception& ex)
    {
        ExceptionMapper e(ex);
        e.setContext( rElement.getUnoInstance() );
        e.unhandled();
    }

    // filter/wrap uno::Exceptions
    catch (WrappedTargetException& )    { throw; }
    catch (RuntimeException& )          { throw; }
    catch (uno::Exception& ex)
    {
        Reference<uno::XInterface> xContext( rElement.getUnoInstance() );
        OUString sMessage( RTL_CONSTASCII_USTRINGPARAM("Configuration: can't commit Changes: ") );
        throw WrappedTargetException( sMessage += ex.Message, xContext, uno::makeAny(ex));
    }
}
//-----------------------------------------------------------------------------

sal_Bool implHasPendingChanges( RootElement& rElement ) throw(uno::RuntimeException)
{
    try
    {
        GuardedRootElement aLocked(rElement);
        return aLocked.getTree().hasChanges();
    }
    catch (configuration::Exception& ex)
    {
        ExceptionMapper e(ex);
        e.setContext( rElement.getUnoInstance() );
        e.unhandled();
    }

    OSL_ENSURE(false,"Unreachable Code");
    return false;
}
//-----------------------------------------------------------------------------

uno::Sequence< css::util::ElementChange > implGetPendingChanges( RootElement& rElement )
        throw(uno::RuntimeException)
{
    using css::util::ElementChange;
    using configuration::NodeChangesInformation;

    std::vector<ElementChange> aResult;
    try
    {
        GuardedRootElement aLocked(rElement);

        Tree aTree( aLocked.getTree() );

        NodeChangesInformation aInfos;

        {
            NodeChanges aChanges;
            if (aTree.collectChanges(aChanges))
            {
                aChanges.getChangesInfos(aInfos);
            }
        }

        Factory& rFactory = rElement.getFactory();

        for(NodeChangesInformation::Iterator it = aInfos.begin(), stop = aInfos.end();
            it != stop;
            ++it)
        {
            ElementChange aChange;
            fillChange(aChange,*it,aTree,rFactory);
            aResult.push_back(aChange);
        }
    }
    catch (configuration::Exception& ex)
    {
        ExceptionMapper e(ex);
        e.setContext( rElement.getUnoInstance() );
        e.unhandled();
    }

    return makeSequence(aResult);
}
//-----------------------------------------------------------------------------

// Set only -------------------------------------------------------------------------

// XTemplateInstance
//-----------------------------------------------------------------------------------

OUString implGetTemplateName(SetElement& rElement)
    throw(uno::RuntimeException)
{
    try
    {
        GuardedTreeElement aLocked(rElement);
        return rElement.getTemplateInfo().getTemplatePathString();
    }
    catch (configuration::Exception& ex)
    {
        ExceptionMapper e(ex);
        e.setContext( rElement.getUnoInstance() );
        e.unhandled();
    }

    OSL_ENSURE(false,"Unreachable Code");
    return OUString();
}
//-----------------------------------------------------------------------------------

// XUnoTunnel
//-----------------------------------------------------------------------------------
sal_Int64 implGetSomething(SetElement& rElement, const uno::Sequence< sal_Int8 >& aIdentifier )
    throw(uno::RuntimeException)
{
    sal_Int64 nSomething = 0;
    try
    {
        GuardedTreeElement aLocked(rElement);

        if (!rElement.getFactory().tunnelSetElement(nSomething, rElement, aIdentifier))
            OSL_ASSERT(nSomething == 0);
    }
    catch (configuration::Exception& ex)
    {
        ExceptionMapper e(ex);
        e.setContext( rElement.getUnoInstance() );
        e.unhandled();
    }

    return nSomething;
}
//-----------------------------------------------------------------------------------

    } // namespace configapi

} // namespace configmgr


