/*************************************************************************
 *
 *  $RCSfile: framectr.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-16 14:47:14 $
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

#ifndef _BIB_FRAMECTR_HXX
#define _BIB_FRAMECTR_HXX
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYCHANGELISTENER_HPP_
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCONTROLLER_HPP_
#include <com/sun/star/frame/XController.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCH_HPP_
#include <com/sun/star/frame/XDispatch.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XLOADABLE_HPP_
#include <com/sun/star/form/XLoadable.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHINFORMATIONPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchInformationProvider.hpp>
#endif

#ifndef _CPPUHELPER_IMPLBASE5_HXX_
#include <cppuhelper/implbase5.hxx>
#endif

#ifndef _SVARRAY_HXX
#include <svtools/svarray.hxx>
#endif

#include "bibmod.hxx"
class BibDataManager;
namespace com{namespace sun{namespace star{
    namespace form{
        class XFormController;
    }
}}}
class BibStatusDispatch
{
public:
    ::com::sun::star::util::URL                 aURL;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >    xListener;
                        BibStatusDispatch( const ::com::sun::star::util::URL& rURL, const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >  xRef )
                            : aURL( rURL )
                            , xListener( xRef )
                        {}
};

typedef BibStatusDispatch* BibStatusDispatchPtr;
SV_DECL_PTRARR_DEL( BibStatusDispatchArr, BibStatusDispatchPtr, 4, 4 );

class BibFrameController_Impl : public cppu::WeakImplHelper5 <
    ::com::sun::star::lang::XServiceInfo,
    ::com::sun::star::frame::XController,
    ::com::sun::star::frame::XDispatch,
    ::com::sun::star::frame::XDispatchProvider,
    ::com::sun::star::frame::XDispatchInformationProvider
>
{
friend class BibFrameCtrl_Impl;
    BibFrameCtrl_Impl*          pImp;
    BibStatusDispatchArr        aStatusListeners;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >                  xWindow;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >                 xFrame;
    sal_Bool                        bDisposing;
    sal_Bool                        bHierarchical;
    ::com::sun::star::uno::Reference< ::com::sun::star::form::XLoadable >               m_xDatMan;
    BibDataManager*             pDatMan;
    HdlBibModul                 pBibMod;

    DECL_STATIC_LINK( BibFrameController_Impl, DisposeHdl, void* );

    sal_Bool                    SaveModified(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormController>& xController);
public:
                                BibFrameController_Impl( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > & xComponent);
                                BibFrameController_Impl( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > & xComponent,
                                                        BibDataManager* pDatMan);
                                ~BibFrameController_Impl();


    void                        activate();
    void                        deactivate();

    BibDataManager*             GetDataManager();
    void                        ChangeDataSource(const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgs);
    void                        RemoveFilter();

                                // ::com::sun::star::lang::XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName() throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& sServiceName ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw (::com::sun::star::uno::RuntimeException);

                                // ::com::sun::star::frame::XController
    virtual void                SAL_CALL attachFrame( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > & xFrame ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool            SAL_CALL attachModel( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > & xModel ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool            SAL_CALL suspend( sal_Bool bSuspend ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any  SAL_CALL    getViewData() throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL                           restoreViewData( const ::com::sun::star::uno::Any& Value ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > SAL_CALL getFrame() throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > SAL_CALL getModel() throw (::com::sun::star::uno::RuntimeException);

                                // ::com::sun::star::lang::XComponent
    virtual void                SAL_CALL dispose() throw (::com::sun::star::uno::RuntimeException);
    virtual void                SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener ) throw (::com::sun::star::uno::RuntimeException);
    virtual void                SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener ) throw (::com::sun::star::uno::RuntimeException);

                                // ::com::sun::star::frame::XDispatchProvider
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >          SAL_CALL queryDispatch( const ::com::sun::star::util::URL& aURL, const rtl::OUString& aTargetFrameName, sal_Int32 nSearchFlags) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >  > SAL_CALL queryDispatches( const ::com::sun::star::uno::Sequence< ::com::sun::star::frame::DispatchDescriptor >& aDescripts) throw (::com::sun::star::uno::RuntimeException);

                                //class ::com::sun::star::frame::XDispatch
    virtual void                SAL_CALL dispatch(const ::com::sun::star::util::URL& aURL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgs) throw (::com::sun::star::uno::RuntimeException);
    virtual void                SAL_CALL addStatusListener(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & xControl, const ::com::sun::star::util::URL& aURL) throw (::com::sun::star::uno::RuntimeException);
    virtual void                SAL_CALL removeStatusListener(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & xControl, const ::com::sun::star::util::URL& aURL) throw (::com::sun::star::uno::RuntimeException);

                                // ::com::sun::star::frame::XDispatchInformationProvider
    virtual ::com::sun::star::uno::Sequence< ::sal_Int16 > SAL_CALL getSupportedCommandGroups(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::frame::DispatchInformation > SAL_CALL getConfigurableDispatchInformation( ::sal_Int16 CommandGroup ) throw (::com::sun::star::uno::RuntimeException);
 };

#endif

