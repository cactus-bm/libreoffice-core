/*************************************************************************
 *
 *  $RCSfile: X11_clipboard.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: pl $ $Date: 2001-02-07 17:59:21 $
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

#include <X11/Xatom.h>
#include <X11_clipboard.hxx>
#include <X11_transferable.hxx>

#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_CLIPBOARD_RENDERINGCAPABILITIES_HPP_
#include <com/sun/star/datatransfer/clipboard/RenderingCapabilities.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_REGISTRY_XREGISTRYKEY_HPP_
#include <com/sun/star/registry/XRegistryKey.hpp>
#endif

#ifndef _UNO_DISPATCHER_H_
#include <uno/dispatcher.h> // declaration of generic uno interface
#endif
#ifndef _UNO_MAPPING_HXX_
#include <uno/mapping.hxx> // mapping stuff
#endif
#ifndef _CPPUHELPER_FACTORY_HXX_
#include <cppuhelper/factory.hxx>
#endif
#ifndef _RTL_TENCINFO_H
#include <rtl/tencinfo.h>
#endif

using namespace com::sun::star::datatransfer;
using namespace com::sun::star::datatransfer::clipboard;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;
using namespace cppu;
using namespace osl;
using namespace rtl;

using namespace x11;

// ------------------------------------------------------------------------

X11Clipboard::X11Clipboard() :
        m_aMutex(),
        WeakComponentImplHelper3< XClipboardEx, XServiceInfo, XInitialization > (m_aMutex)
{
    // XXX: currently X11Clipboard starts without arguments
    // this will need to change
    initialize( Sequence< Any >() );
}

// ------------------------------------------------------------------------

X11Clipboard::~X11Clipboard()
{
#ifdef DEBUG
    fprintf( stderr, "shutting down X11Clipboard\n" );
#endif
    SelectionManager& rManager(SelectionManager::get());
    rManager.deregisterHandler( XA_PRIMARY );
    rManager.deregisterHandler( rManager.getAtom( OUString::createFromAscii( "CLIPBOARD" ) ) );
}

// ------------------------------------------------------------------------

void X11Clipboard::initialize( const Sequence< Any >& arguments )
{
    SelectionManager& rManager = SelectionManager::get();
    m_xSelectionManager = &rManager;
    rManager.initialize( arguments );
    rManager.registerHandler( XA_PRIMARY, *this );
    rManager.registerHandler( rManager.getAtom( OUString::createFromAscii( "CLIPBOARD" ) ), *this );

    if ( rManager.getDisplay() == NULL )
        throw RuntimeException(
            OUString::createFromAscii("DISPLAY not set or connect to specified X server failed"),
            static_cast < XClipboard * > (this));
}

// ------------------------------------------------------------------------

void X11Clipboard::fireChangedContentsEvent()
{
#if 0
    OInterfaceContainerHelper *pContainer =
        rBHelper.aLC.getContainer(getCppuType( (Reference < XClipboardListener > *) 0));

    if (pContainer)
    {
        ClipboardEvent aEvent(static_cast < XClipboard * > (this), m_aContents);
        OInterfaceIteratorHelper aIterator(*pContainer);

        while (aIterator.hasMoreElements())
        {
            Reference < XClipboardListener > xListener(aIterator.next(), UNO_QUERY);
            if (xListener.is())
                xListener->changedContents(aEvent);
        }
    }
#endif
}

// ------------------------------------------------------------------------

void X11Clipboard::clearContents()
{
    MutexGuard aGuard(m_aMutex);

    if ( m_aOwner.is() ) {
        m_aOwner->lostOwnership(static_cast < XClipboard * > (this), m_aContents);
        m_aContents.clear();
        m_aOwner.clear();
    }
}

// ------------------------------------------------------------------------

OUString SAL_CALL X11Clipboard::getImplementationName(  )
    throw(RuntimeException)
{
    return OUString::createFromAscii(X11_CLIPBOARD_IMPLEMENTATION_NAME);
}

// ------------------------------------------------------------------------

sal_Bool SAL_CALL X11Clipboard::supportsService( const OUString& ServiceName )
    throw(RuntimeException)
{
    Sequence < OUString > SupportedServicesNames = X11Clipboard_getSupportedServiceNames();

    for ( sal_Int32 n = SupportedServicesNames.getLength(); n--; )
        if (SupportedServicesNames[n].compareTo(ServiceName) == 0)
            return sal_True;

    return sal_False;
}

// ------------------------------------------------------------------------

Sequence< OUString > SAL_CALL X11Clipboard::getSupportedServiceNames(    )
    throw(RuntimeException)
{
    return X11Clipboard_getSupportedServiceNames();
}

// ------------------------------------------------------------------------

Reference< XTransferable > SAL_CALL X11Clipboard::getContents()
    throw(RuntimeException)
{
    MutexGuard aGuard(m_aMutex);

    if( ! m_aContents.is() )
        m_aContents = new X11Transferable( SelectionManager::get(), static_cast< OWeakObject* >(this) );
    return m_aContents;
}

// ------------------------------------------------------------------------

void SAL_CALL X11Clipboard::setContents(
    const Reference< XTransferable >& xTrans,
    const Reference< XClipboardOwner >& xClipboardOwner )
    throw(RuntimeException)
{
    // remember old values for callbacks before setting the new ones.
    ClearableMutexGuard aGuard(m_aMutex);

    Reference< XClipboardOwner > oldOwner( m_aOwner );
    m_aOwner = xClipboardOwner;

    Reference< XTransferable > oldContents( m_aContents );
    m_aContents = xTrans;

    aGuard.clear();

    // for now request ownership for both selections
    SelectionManager& rSelectionManager = SelectionManager::get();
    rSelectionManager.requestOwnership( XA_PRIMARY );
    rSelectionManager.requestOwnership( rSelectionManager.getAtom( OUString::createFromAscii( "CLIPBOARD" ) ) );

    // notify old owner on loss of ownership
    if( oldOwner.is() )
        oldOwner->lostOwnership(static_cast < XClipboard * > (this), oldContents);

    // notify all listeners on content changes
    fireChangedContentsEvent();
}

// ------------------------------------------------------------------------

OUString SAL_CALL X11Clipboard::getName()
    throw(RuntimeException)
{
    return OUString();
}

// ------------------------------------------------------------------------

sal_Int8 SAL_CALL X11Clipboard::getRenderingCapabilities()
    throw(RuntimeException)
{
    return RenderingCapabilities::Delayed;
}


// ------------------------------------------------------------------------
#if 0
void SAL_CALL X11Clipboard::addClipboardListener( const Reference< XClipboardListener >& listener )
    throw(RuntimeException)
{
    rBHelper.addListener( getCppuType( (const ::com::sun::star::uno::Reference< XClipboard > *) 0), listener );
}

// ------------------------------------------------------------------------

void SAL_CALL X11Clipboard::removeClipboardListener( const Reference< XClipboardListener >& listener )
    throw(RuntimeException)
{
    rBHelper.removeListener( getCppuType( (const Reference< XClipboardListener > *) 0 ), listener );
}

#endif

// ------------------------------------------------------------------------

Reference< XTransferable > X11Clipboard::getTransferable()
{
    return getContents();
}

// ------------------------------------------------------------------------

void X11Clipboard::clearTransferable()
{
    clearContents();
}

