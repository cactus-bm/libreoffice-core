/*************************************************************************
 *
 *  $RCSfile: vclxcontainer.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2001-09-28 09:38:18 $
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

#ifndef _TOOLKIT_AWT_VCLXCONTAINER_HXX_
#define _TOOLKIT_AWT_VCLXCONTAINER_HXX_


#ifndef _COM_SUN_STAR_AWT_XVCLCONTAINER_HPP_
#include <com/sun/star/awt/XVclContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XVCLCONTAINERPEER_HPP_
#include <com/sun/star/awt/XVclContainerPeer.hpp>
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

#include <toolkit/awt/vclxwindow.hxx>


//  ----------------------------------------------------
//  class VCLXContainer
//  ----------------------------------------------------

class VCLXContainer :   public ::com::sun::star::awt::XVclContainer,
                        public ::com::sun::star::awt::XVclContainerPeer,
                        public VCLXWindow
{
public:
                    VCLXContainer();
                    ~VCLXContainer();

    // ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any  SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
    void                        SAL_CALL acquire() throw()  { OWeakObject::acquire(); }
    void                        SAL_CALL release() throw()  { OWeakObject::release(); }

    // ::com::sun::star::lang::XTypeProvider
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >  SAL_CALL getTypes() throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Sequence< sal_Int8 >                     SAL_CALL getImplementationId() throw(::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::awt::XVclContainer
    void SAL_CALL addVclContainerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XVclContainerListener >& l ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL removeVclContainerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XVclContainerListener >& l ) throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > > SAL_CALL getWindows(  ) throw(::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::awt::XVclContainerPeer
    void SAL_CALL enableDialogControl( sal_Bool bEnable ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setTabOrder( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > >& WindowOrder, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& Tabs, sal_Bool GroupControl ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setGroup( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > >& Windows ) throw(::com::sun::star::uno::RuntimeException);

};




#endif // _TOOLKIT_AWT_VCLXCONTAINER_HXX_

