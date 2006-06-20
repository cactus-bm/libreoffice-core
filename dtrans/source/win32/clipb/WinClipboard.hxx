/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: WinClipboard.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-20 06:01:35 $
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


#ifndef _WINCLIPBOARD_HXX_
#define _WINCLIPBOARD_HXX_


//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _CPPUHELPER_COMPBASE4_HXX_
#include <cppuhelper/compbase4.hxx>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_XTRANSFERABLE_HPP_
#include <com/sun/star/datatransfer/XTransferable.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_CLIPBOARD_XCLIPBOARDEX_HPP_
#include <com/sun/star/datatransfer/clipboard/XClipboardEx.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_CLIPBOARD_XCLIPBOARDOWNER_HPP_
#include <com/sun/star/datatransfer/clipboard/XClipboardOwner.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_CLIPBOARD_XCLIPBOARDLISTENER_HPP_
#include <com/sun/star/datatransfer/clipboard/XClipboardListener.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_CLIPBOARD_XCLIPBOARDNOTIFIER_HPP_
#include <com/sun/star/datatransfer/clipboard/XClipboardNotifier.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_CLIPBOARD_XFLUSHABLECLIPBOARD_HPP_
#include <com/sun/star/datatransfer/clipboard/XFlushableClipboard.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef _OSL_CONDITN_HXX_
#include <osl/conditn.hxx>
#endif

#include <memory>

// forward
class CWinClipbImpl;

//------------------------------------------------------------------------
// implements the XClipboard[Ex] ... interfaces
// for the clipboard viewer mechanism we need a static callback function
// and a static member to reasocciate from this static function to the
// class instance
// watch out: we are using only one static member variable and not a list
// because we assume to be instanciated only once
// this will be asured by an OneInstanceFactory of the service and not
// by this class!
//------------------------------------------------------------------------

// helper class, so that the mutex is constructed
// before the constructor of WeakComponentImplHelper
// will be called and initialized with this mutex
class CWinClipboardDummy
{
protected:
    osl::Mutex      m_aMutex;
    osl::Mutex      m_aCbListenerMutex;
};

class CWinClipboard :
    public CWinClipboardDummy,
    public cppu::WeakComponentImplHelper4<
        ::com::sun::star::datatransfer::clipboard::XClipboardEx, \
        ::com::sun::star::datatransfer::clipboard::XFlushableClipboard,
        ::com::sun::star::datatransfer::clipboard::XClipboardNotifier,
        ::com::sun::star::lang::XServiceInfo >
{
public:
    CWinClipboard( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& rServiceManager,
                   const ::rtl::OUString& aClipboardName );

    //------------------------------------------------
    // XClipboard
    //------------------------------------------------

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > SAL_CALL getContents(  )
        throw( ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL setContents(
        const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable >& xTransferable,
        const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboardOwner >& xClipboardOwner )
        throw( ::com::sun::star::uno::RuntimeException );

    virtual ::rtl::OUString SAL_CALL getName(  )
        throw( ::com::sun::star::uno::RuntimeException );

    //------------------------------------------------
    // XFlushableClipboard
    //------------------------------------------------

    virtual void SAL_CALL flushClipboard( ) throw( com::sun::star::uno::RuntimeException );

    //------------------------------------------------
    // XClipboardEx
    //------------------------------------------------

    virtual sal_Int8 SAL_CALL getRenderingCapabilities(  ) throw( ::com::sun::star::uno::RuntimeException );

    //------------------------------------------------
    // XClipboardNotifier
    //------------------------------------------------

    virtual void SAL_CALL addClipboardListener(
        const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboardListener >& listener )
        throw( ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL removeClipboardListener(
        const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboardListener >& listener )
        throw( ::com::sun::star::uno::RuntimeException );

    //------------------------------------------------
    // overwrite base class method, which is called
    // by base class dispose function
    //------------------------------------------------

    virtual void SAL_CALL disposing();

    //------------------------------------------------
    // XServiceInfo
    //------------------------------------------------

    virtual ::rtl::OUString SAL_CALL getImplementationName(  )
        throw(::com::sun::star::uno::RuntimeException);

    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName )
        throw(::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  )
        throw(::com::sun::star::uno::RuntimeException);

private:
    void SAL_CALL notifyAllClipboardListener( );

private:
    ::std::auto_ptr< CWinClipbImpl >                                                  m_pImpl;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >  m_SrvMgr;

    friend class CWinClipbImpl;
};


#endif
