/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: droptargetlistener.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:03:36 $
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

#ifndef __FRAMEWORK_CLASSES_DROPTARGETLISTENER_HXX_
#define __FRAMEWORK_CLASSES_DROPTARGETLISTENER_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
#include <threadhelp/threadhelpbase.hxx>
#endif

#ifndef __FRAMEWORK_GENERAL_H_
#include <general.h>
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_DATATRANSFER_DND_XDROPTARGETELISTENER_HPP_
#include <com/sun/star/datatransfer/dnd/XDropTargetListener.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

//_________________________________________________________________________________________________________________
//  includes of other projects
//_________________________________________________________________________________________________________________

#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif

#ifndef _SOT_EXCHANGE_HXX_
#include <sot/exchange.hxx>
#endif

namespace framework
{

class DropTargetListener : private ThreadHelpBase
                         , public ::cppu::WeakImplHelper1< ::com::sun::star::datatransfer::dnd::XDropTargetListener >
{
    //___________________________________________
    // member
    private:

        /// uno service manager to create neccessary services
        css::uno::Reference< css::lang::XMultiServiceFactory > m_xFactory;
        /// weakreference to target frame (Don't use a hard reference. Owner can't delete us then!)
        css::uno::WeakReference< css::frame::XFrame > m_xTargetFrame;
        /// drag/drop info
        DataFlavorExVector* m_pFormats;

    //___________________________________________
    // c++ interface
    public:

         DropTargetListener( const css::uno::Reference< css::lang::XMultiServiceFactory >& xFactory ,
                             const css::uno::Reference< css::frame::XFrame >& xFrame                );
        ~DropTargetListener(                                                                        );

    //___________________________________________
    // uno interface
    public:

        // XEventListener
        virtual void SAL_CALL disposing        ( const css::lang::EventObject& Source ) throw(css::uno::RuntimeException);

        // XDropTargetListener
        virtual void SAL_CALL drop             ( const css::datatransfer::dnd::DropTargetDropEvent&      dtde  ) throw(css::uno::RuntimeException);
        virtual void SAL_CALL dragEnter        ( const css::datatransfer::dnd::DropTargetDragEnterEvent& dtdee ) throw(css::uno::RuntimeException);
        virtual void SAL_CALL dragExit         ( const css::datatransfer::dnd::DropTargetEvent&          dte   ) throw(css::uno::RuntimeException);
        virtual void SAL_CALL dragOver         ( const css::datatransfer::dnd::DropTargetDragEvent&      dtde  ) throw(css::uno::RuntimeException);
        virtual void SAL_CALL dropActionChanged( const css::datatransfer::dnd::DropTargetDragEvent&      dtde  ) throw(css::uno::RuntimeException);

    //___________________________________________
    // internal helper
    private:

        void     implts_BeginDrag            ( const css::uno::Sequence< css::datatransfer::DataFlavor >& rSupportedDataFlavors );
        void     implts_EndDrag              (                                                                                  );
        sal_Bool implts_IsDropFormatSupported( SotFormatStringId nFormat                                                        );
        void     implts_OpenFile             ( const String& rFilePath                                                          );

}; // class DropTargetListener

} // namespace framework

#endif // __FRAMEWORK_CLASSES_DROPTARGETLISTENER_HXX_
