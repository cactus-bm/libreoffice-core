/*************************************************************************
 *
 *  $RCSfile: javaoptions.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2005-04-13 10:19:28 $
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
#ifndef _SVTOOLS_JAVAPTIONS_HXX
#define _SVTOOLS_JAVAOPTIONS_HXX

#ifndef INCLUDED_SVLDLLAPI_H
#include "svtools/svldllapi.h"
#endif

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif


// class SvtJavaOptions --------------------------------------------------

struct SvtJavaOptions_Impl;

class SVL_DLLPUBLIC SvtJavaOptions : public utl::ConfigItem
{
    SvtJavaOptions_Impl* pImpl;
public:
    enum EOption
    {
        E_ENABLED,
        E_SECURITY,
        E_NETACCESS,
        E_USERCLASSPATH,
        E_EXECUTEAPPLETS
    };

    SvtJavaOptions();
    ~SvtJavaOptions();

    virtual void    Commit();

    sal_Bool        IsEnabled() const;
    sal_Bool        IsSecurity()const;
    sal_Int32       GetNetAccess() const;
    rtl::OUString&  GetUserClassPath()const;
    sal_Bool        IsExecuteApplets() const;

    void SetEnabled(sal_Bool bSet) ;
    void SetSecurity(sal_Bool bSet);
    void SetNetAccess(sal_Int32 nSet) ;
    void SetUserClassPath(const rtl::OUString& rSet);
    void SetExecuteApplets(sal_Bool bSet);

    sal_Bool IsReadOnly( EOption eOption ) const;
};

#endif //

