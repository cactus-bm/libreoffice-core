/*************************************************************************
 *
 *  $RCSfile: confevents.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: jb $ $Date: 2000-12-04 09:25:10 $
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

#ifndef CONFIGMGR_API_EVENTS_HXX_
#define CONFIGMGR_API_EVENTS_HXX_

#include "cmtreemodel.hxx"

namespace rtl { class OUString; }

namespace configmgr
{
    using ::rtl::OUString;

    struct IConfigBroadcaster;
    struct IConfigListener
    {
        virtual void disposing(IConfigBroadcaster* pSource) = 0;
    };
    struct INodeListener : IConfigListener
    {
        virtual void nodeChanged(Change const& aChange, OUString const& aPath, IConfigBroadcaster* pSource) = 0;
        virtual void nodeDeleted(OUString const& aPath, IConfigBroadcaster* pSource) = 0;
    };

    struct IConfigBroadcaster
    {
    protected:
        IConfigBroadcaster() {}
        ~IConfigBroadcaster() {}
    public:
        virtual void addListener(OUString const& aName, const vos::ORef < OOptions >& _xOptions, INodeListener* pListener) = 0;
        virtual void removeListener(const vos::ORef < OOptions >& _xOptions, INodeListener* pListener) = 0;

        virtual void removeNode(OUString const& aPath, const vos::ORef < OOptions >& _xOptions, bool bRemovedFromModel = false) = 0;
    };

    class ConfigChangeBroadcastHelper; // broadcasts changes for a given set of options
    class ConfigChangeBroadcaster : public IConfigBroadcaster
    {
    public:
        ConfigChangeBroadcaster();
        virtual ~ConfigChangeBroadcaster();

        virtual void addListener(OUString const& aName, const vos::ORef < OOptions >& _xOptions, INodeListener* pListener);
        virtual void removeListener(const vos::ORef < OOptions >& _xOptions, INodeListener* pListener);

        virtual void removeNode(OUString const& aPath, const vos::ORef < OOptions >& _xOptions, bool bRemovedFromModel = false);

    protected:
        virtual void fireChanges(TreeChangeList const& _aChanges, sal_Bool _bError);
    protected:
        virtual ConfigChangeBroadcastHelper* getBroadcastHelper(const vos::ORef < OOptions >& _xOptions, bool bCreate) = 0;
        ConfigChangeBroadcastHelper* newBroadcastHelper(); // needed to implement the preceding
        void disposeBroadcastHelper(ConfigChangeBroadcastHelper* pHelper); // needed to discard the preceding
    };

} // namespace

#endif // CONFIGMGR_API_EVENTS_HXX_



