/*************************************************************************
 *
 *  $RCSfile: UpdateLockManager.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2005-03-30 09:24:23 $
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

#ifndef SD_UPDATE_LOCK_MANAGER_HXX
#define SD_UPDATE_LOCK_MANAGER_HXX

#include <memory>

namespace sd {

class ViewShellBase;

/** Manage update locks of ViewShellBase objects.
    A ViewShellBase object is locked while views are switched in order to
    avoid unnecessary repaints of views and object bars.
    Locking a ViewShellBase locks the frame::XLayoutManager and prevents
    Activate() and Deactivate() calls at ViewShell objects being processed.

    The main responsibility of this class is find the right moment to unlock
    the managed ViewShellBase object: Only Lock() has to be called from the
    outside (usually from PaneManager).  Unlock() is called by this class
    itself.  When all else fails it has a timer that calls Unlock()
    eventually.
*/
class UpdateLockManager
{
public:
    /** The newly created instance supports locking for the specified
        ViewShellBase object by default.  Call Disable() for Lock() and
        Unlock() calls being ignored.
    */
    UpdateLockManager (ViewShellBase& rBase);
    ~UpdateLockManager (void);

    /** For e.g. the PresentationViewShellBase locking is not necessary and
        does lead to problems.  This method lets Lock() and Unlock() calls
        be ignored and thus turns locking essentially off.
    */
    void Disable (void);

    /** Lock some UI updates.  For every call to this method a call to
        Unlock() is required to really unlock.
    */
    void Lock (void);

    /** When called as many times as Lock() has been called before then the
        ViewShellBase object is unlocked.
    */
    void Unlock (void);

    /** Return whether the ViewShellBase object is locked.  When locking is
        disabled, i.e. Disable() has been called before, then this method
        always returns <FALSE/>.
    */
    bool IsLocked (void) const;

private:
    class Implementation;
    Implementation* mpImpl;

    UpdateLockManager (const UpdateLockManager&); // Not supported.
    UpdateLockManager& operator= (const UpdateLockManager&); // Not supported.
};

} // end of namespace sd

#endif

