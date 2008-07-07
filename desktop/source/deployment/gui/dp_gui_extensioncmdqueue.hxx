/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: dp_gui_extensioncmdqueue.hxx,v $
 *
 * $Revision: 1.3 $
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

#ifndef INCLUDED_DP_GUI_EXTENSIONCMDQUEUE_HXX
#define INCLUDED_DP_GUI_EXTENSIONCMDQUEUE_HXX

#ifndef _SAL_CONFIG_H_
#include "sal/config.h"
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include "com/sun/star/uno/Reference.hxx"
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include "com/sun/star/uno/Sequence.hxx"
#endif
#ifndef _RTL_REF_HXX_
#include "rtl/ref.hxx"
#endif

/// @HTML

namespace com { namespace sun { namespace star {
    namespace deployment { class XPackageManager; }
    namespace uno { class XComponentContext; }
} } }

namespace dp_gui {

class ExtMgrDialog;
class TheExtensionManager;

/**
   Manages installing of extensions in the GUI mode. Requests for installing
   Extensions can be asynchronous. For example, the Extension Manager is running
   in an office process and someone uses the system integration to install an Extension.
   That is, the user double clicks an extension symbol in a file browser, which then
   causes an invocation of "unopkg gui ext". When at that time the Extension Manager
   already performs a task, triggered by the user (for example, add, update, disable,
   enable) then adding of the extension will be postponed until the user has finished
   the task.

   This class also ensures that the extensions are not installed in the main thread.
   Doing so would cause a deadlock because of the progress bar which needs to be constantly
   updated.
*/
class ExtensionCmdQueue {

public:
    /**
       Create an instance.
    */
    ExtensionCmdQueue( ExtMgrDialog * pDialog,
                       TheExtensionManager *pManager,
                       const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > & rContext);

    ~ExtensionCmdQueue();

    /**
    */
    void addExtension( const ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackageManager > &rPackageManager,
                       const ::rtl::OUString &rExtensionURL,
                       const bool bWarnUser );
    void removeExtension( const ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackageManager > &rPackageManager,
                          const ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage > &rPackage );
    void enableExtension( const ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage > &rPackage,
                          const bool bEnable );
    void checkForUpdates( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackageManager > > &rPackageManagers );
    void checkForUpdate( const ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackageManager > &rPackageManager,
                         const ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage > &rPackage );
    /**
       This call does not block. It signals the internal thread
       that it should install the remaining extensions and then terminate.
    */
    void stop();

    /** Determines if thread of this class has terminated.
    */
    bool hasTerminated();

    /** Blocks until the thread has terminated. All URLs in the queue will be processed.
    */
    void stopAndWait();

    bool isBusy();
private:
    ExtensionCmdQueue(ExtensionCmdQueue &); // not defined
    void operator =(ExtensionCmdQueue &); // not defined

    class Thread;

    rtl::Reference< Thread > m_thread;
};

}

#endif
