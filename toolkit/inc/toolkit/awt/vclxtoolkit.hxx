/*************************************************************************
 *
 *  $RCSfile: vclxtoolkit.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obr $ $Date: 2001-02-21 15:23:16 $
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

#ifndef _TOOLKIT_AWT_VCLXTOOLKIT_HXX_
#define _TOOLKIT_AWT_VCLXTOOLKIT_HXX_


#ifndef _COM_SUN_STAR_AWT_XTOOLKIT_HPP_
#include <com/sun/star/awt/XToolkit.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XDATATRANSFER_HPP_
#include <com/sun/star/awt/XDataTransfer.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XTYPEPROVIDER_HPP_
#include <com/sun/star/lang/XTypeProvider.hpp>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

class Window;
class VCLXWindow;

namespace com {
namespace sun {
namespace star {
namespace awt {
    struct WindowDescriptor;
} } } }

//  ----------------------------------------------------
//  class VCLXTOOLKIT
//  ----------------------------------------------------

class VCLXToolkit : public ::com::sun::star::awt::XToolkit,
                    public ::com::sun::star::awt::XDataTransfer,
                    public ::com::sun::star::lang::XTypeProvider,
                    public ::cppu::OWeakObject
{
private:
    ::osl::Mutex    maMutex;

    ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboard > mxClipboard;
    ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboard > mxSelection;

protected:
    ::osl::Mutex&   GetMutex() { return maMutex; }

public:

    virtual Window* CreateComponent( VCLXWindow** ppNewComp,
        const ::com::sun::star::awt::WindowDescriptor& rDescriptor, Window* pParent, sal_uInt32 nWinBits );


    // ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any  SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
    void                        SAL_CALL acquire() throw(::com::sun::star::uno::RuntimeException)   { OWeakObject::acquire(); }
    void                        SAL_CALL release() throw(::com::sun::star::uno::RuntimeException)   { OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >  SAL_CALL getTypes() throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Sequence< sal_Int8 >                     SAL_CALL getImplementationId() throw(::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::awt::XToolkit
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer >  SAL_CALL getDesktopWindow(  ) throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::awt::Rectangle                                        SAL_CALL getWorkArea(  ) throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer >  SAL_CALL createWindow( const ::com::sun::star::awt::WindowDescriptor& Descriptor ) throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer > > SAL_CALL createWindows( const ::com::sun::star::uno::Sequence< ::com::sun::star::awt::WindowDescriptor >& Descriptors ) throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XDevice >      SAL_CALL createScreenCompatibleDevice( sal_Int32 Width, sal_Int32 Height ) throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XRegion >      SAL_CALL createRegion(  ) throw(::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::awt::XDataTransfer
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDragGestureRecognizer > SAL_CALL getDragGestureRecognizer( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >& window ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDragSource > SAL_CALL getDragSource( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >& window ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::dnd::XDropTarget > SAL_CALL getDropTarget( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >& window ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboard > SAL_CALL getClipboard( const ::rtl::OUString& clipboardName ) throw(::com::sun::star::uno::RuntimeException);
};




#endif // _TOOLKIT_AWT_VCLXTOOLKIT_HXX_

