/*************************************************************************
 *
 *  $RCSfile: requestoptions.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: jb $ $Date: 2002-03-28 08:30:12 $
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

#include "requestoptions.hxx"

#ifndef CONFIGMGR_MATCHLOCALE_HXX
#include "matchlocale.hxx"
#endif
#ifndef _CONFIGMGR_TRACER_HXX_
#include "tracer.hxx"
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

namespace configmgr
{
// ---------------------------------------------------------------------------

    static sal_Int32 getNextCacheID()
    {
        static oslInterlockedCount nNextID = 0;

        oslInterlockedCount nNewID = osl_incrementInterlockedCount(&nNextID);

        if (nNewID == 0)
        {
            CFG_TRACE_WARNING("Cache ID overflow - restarting sequence !");
            OSL_ENSURE(false, "Cache ID overflow - restarting sequence !");
        }

        return static_cast<sal_Int32>(nNewID);
    }
// ---------------------------------------------------------------------------

    RequestOptions RequestOptions::forAllLocales()
    {
        RequestOptions aResult;
        aResult.setAllLocales();
        return aResult;
    }

// ---------------------------------------------------------------------------
    void RequestOptions::forceReload(bool _bNoCache)
    {
        if (_bNoCache)
        {
            m_nCacheID = getNextCacheID();
        }
        else
        {
            m_nCacheID = 0;
        }
    }
// ---------------------------------------------------------------------------

    bool RequestOptions::isForAllLocales() const
    {
        return localehelper::designatesAllLocales( localehelper::makeLocale(m_sLocale) );
    }
// ---------------------------------------------------------------------------

    void RequestOptions::setAllLocales()
    {
        localehelper::getAnyLocale(m_sLocale);
    }
// ---------------------------------------------------------------------------

    sal_Int32 RequestOptions::hashCode() const
    {
        return m_sLocale.hashCode() ^ m_sEntity.hashCode() ^ m_nCacheID;
    }
// ---------------------------------------------------------------------------

    sal_Int32 compareRequestOptions(RequestOptions const& lhs, RequestOptions const& rhs)
    {
        sal_Int32 nDiff = lhs.getEntity().compareTo(rhs.getEntity());
        if (nDiff == 0)
            nDiff = lhs.getLocale().compareTo(rhs.getLocale());

        if (nDiff == 0)
            nDiff = rhs.m_nCacheID - lhs.m_nCacheID;

        return nDiff;
    }
// ---------------------------------------------------------------------------
} // namespace config


