/*************************************************************************
 *
 *  $RCSfile: X11_clipboard.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2003-12-01 11:25:29 $
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
 *  WITHOUT WARRUNTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRUNTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _DTRANS_X11_CLIPBOARD_HXX_
#define _DTRANS_X11_CLIPBOARD_HXX_

#ifndef _DTRANS_X11_SELECTION_HXX_
#include <X11_selection.hxx>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_CLIPBAORD_XCLIPBOARDEX_HPP_
#include <com/sun/star/datatransfer/clipboard/XClipboardEx.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_CLIPBAORD_XCLIPBOARDNOTIFIER_HPP_
#include <com/sun/star/datatransfer/clipboard/XClipboardNotifier.hpp>
#endif

#ifndef _CPPUHELPER_COMPBASE4_HXX_
#include <cppuhelper/compbase4.hxx>
#endif

// ------------------------------------------------------------------------

#define X11_CLIPBOARD_IMPLEMENTATION_NAME "com.sun.star.datatransfer.X11ClipboardSupport"

namespace x11 {

    class X11Clipboard :
        public ::cppu::WeakComponentImplHelper4 <
        ::com::sun::star::datatransfer::clipboard::XClipboardEx,
        ::com::sun::star::datatransfer::clipboard::XClipboardNotifier,
        ::com::sun::star::lang::XServiceInfo,
        ::com::sun::star::lang::XInitialization
        >,
        public SelectionAdaptor
    {
        ::osl::Mutex m_aMutex;

        Reference< ::com::sun::star::datatransfer::XTransferable > m_aContents;
        Reference< ::com::sun::star::datatransfer::clipboard::XClipboardOwner > m_aOwner;

        SelectionManager&                                       m_rSelectionManager;
        Reference< ::com::sun::star::lang::XInitialization >    m_xSelectionManager;
        ::std::list< Reference< ::com::sun::star::datatransfer::clipboard::XClipboardListener > > m_aListeners;
        Atom                                                    m_aSelection;

    protected:


        friend class SelectionManager;
        friend class X11_Transferable;

        void fireChangedContentsEvent();
        void clearContents();

    public:

        X11Clipboard( SelectionManager& rManager, Atom aSelection );
        virtual ~X11Clipboard();

        static X11Clipboard* get( const ::rtl::OUString& rDisplayName, Atom aSelection );

        /*
         *  XInitialization
         */
        virtual void SAL_CALL initialize( const Sequence< Any >& arguments ) throw(  ::com::sun::star::uno::Exception );

        /*
         * XServiceInfo
         */

        virtual ::rtl::OUString SAL_CALL getImplementationName(  )
            throw(RuntimeException);

        virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName )
            throw(RuntimeException);

        virtual Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  )
            throw(RuntimeException);

        /*
         * XClipboard
         */

        virtual Reference< ::com::sun::star::datatransfer::XTransferable > SAL_CALL getContents()
            throw(RuntimeException);

        virtual void SAL_CALL setContents(
            const Reference< ::com::sun::star::datatransfer::XTransferable >& xTrans,
            const Reference< ::com::sun::star::datatransfer::clipboard::XClipboardOwner >& xClipboardOwner )
            throw(RuntimeException);

        virtual ::rtl::OUString SAL_CALL getName()
            throw(RuntimeException);

        /*
         * XClipboardEx
         */

        virtual sal_Int8 SAL_CALL getRenderingCapabilities()
            throw(RuntimeException);

        /*
         * XClipboardNotifier
         */
        virtual void SAL_CALL addClipboardListener(
            const Reference< ::com::sun::star::datatransfer::clipboard::XClipboardListener >& listener )
            throw(RuntimeException);

        virtual void SAL_CALL removeClipboardListener(
            const Reference< ::com::sun::star::datatransfer::clipboard::XClipboardListener >& listener )
            throw(RuntimeException);

        /*
         *  SelectionAdaptor
         */
        virtual Reference< ::com::sun::star::datatransfer::XTransferable > getTransferable();
        virtual void clearTransferable();
        virtual void fireContentsChanged();
        virtual Reference< XInterface > getReference() throw();
    };

// ------------------------------------------------------------------------

    Sequence< ::rtl::OUString > SAL_CALL X11Clipboard_getSupportedServiceNames();
    Reference< XInterface > SAL_CALL X11Clipboard_createInstance(
        const Reference< ::com::sun::star::lang::XMultiServiceFactory > & xMultiServiceFactory);

// ------------------------------------------------------------------------

} // namepspace

#endif
