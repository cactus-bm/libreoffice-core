/*************************************************************************
 *
 *  $RCSfile: statusindicatorinterfacewrapper.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2004-09-09 17:08:00 $
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

#ifndef __FRAMEWORK_UIELEMENT_STATUSINDICATORINTERFACEWRAPPER_HXX_
#define __FRAMEWORK_UIELEMENT_STATUSINDICATORINTERFACEWRAPPER_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
#include <threadhelp/threadhelpbase.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_GENERIC_HXX_
#include <macros/generic.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_XINTERFACE_HXX_
#include <macros/xinterface.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_XTYPEPROVIDER_HXX_
#include <macros/xtypeprovider.hxx>
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_TASK_XSTATUSINDICATOR_HPP_
#include <com/sun/star/task/XStatusIndicator.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef _RTL_OUSTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

#include <vector>

namespace framework
{

class StatusIndicatorInterfaceWrapper :   public ::com::sun::star::lang::XTypeProvider       ,
                                          public ::com::sun::star::task::XStatusIndicator    ,
                                          public ::cppu::OWeakObject
{
    public:
        StatusIndicatorInterfaceWrapper( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& rStatusIndicatorImpl );
        virtual ~StatusIndicatorInterfaceWrapper();

        //---------------------------------------------------------------------------------------------------------
        //  XInterface, XTypeProvider
        //---------------------------------------------------------------------------------------------------------
        DECLARE_XINTERFACE
        DECLARE_XTYPEPROVIDER

        //---------------------------------------------------------------------------------------------------------
        //  XStatusIndicator
        //---------------------------------------------------------------------------------------------------------
        virtual void SAL_CALL start   ( const ::rtl::OUString& sText  ,
                                              sal_Int32        nRange ) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL end     (                               ) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL reset   (                               ) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL setText ( const ::rtl::OUString& sText  ) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL setValue(       sal_Int32        nValue ) throw( ::com::sun::star::uno::RuntimeException );

    private:
        StatusIndicatorInterfaceWrapper();

        ::com::sun::star::uno::WeakReference< ::com::sun::star::lang::XComponent > m_xStatusIndicatorImpl;
};

}

#endif // __FRAMEWORK_UIELEMENT_STATUSINDICATORINTERFACEWRAPPER_HXX_
