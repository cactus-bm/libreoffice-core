/*************************************************************************
 *
 *  $RCSfile: dp_resource.h,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2004-11-09 14:06:45 $
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
 *  Copyright: 2002 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#if ! defined INCLUDED_DP_RESOURCE_H
#define INCLUDED_DP_RESOURCE_H

#include "vos/mutex.hxx"
#include "tools/resmgr.hxx"
#include "tools/string.hxx"
#include "tools/resid.hxx"
#include "com/sun/star/lang/Locale.hpp"
#include "dp_misc.h"
#include <memory>


namespace dp_misc {

// xxx todo: remove when PL has inserted res mgr mutex in tools:
extern ::vos::IMutex * g_pResMgrMutex;

//==============================================================================
ResId getResId( USHORT id );

//==============================================================================
String getResourceString( USHORT id );

template<typename Unique, USHORT id>
struct StaticResourceString
    : public ::rtl::StaticData< ::rtl::OUString, Unique > {
    ::rtl::OUString operator () () { return getResourceString(id); }
};

//==============================================================================
inline ::com::sun::star::lang::Locale toLocale( ::rtl::OUString const & slang )
{
    ::com::sun::star::lang::Locale locale;
    sal_Int32 nIndex = 0;
    locale.Language = slang.getToken( 0, '-', nIndex );
    locale.Country = slang.getToken( 1, '-', nIndex );
    locale.Variant = slang.getToken( 2, '-', nIndex );
    return locale;
}

//==============================================================================
::com::sun::star::lang::Locale const & getOfficeLocale();

}

#endif
