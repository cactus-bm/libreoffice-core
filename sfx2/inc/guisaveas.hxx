/*************************************************************************
 *
 *  $RCSfile: guisaveas.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2004-10-04 20:36:37 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *       - GNU Lesser General Public License Version 2.1
 *       - Sun Industry Standards Source License Version 1.1
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

#ifndef _SFX_GUISAVEAS_HXX_
#define _SFX_GUISAVEAS_HXX_

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_NAMEDVALUE_HPP_
#include <com/sun/star/beans/NamedValue.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCONTAINERQUERY_HPP_
#include <com/sun/star/container/XContainerQuery.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_FRAME_XMODULEMANAGER_HPP_
#include <drafts/com/sun/star/frame/XModuleManager.hpp>
#endif


#include <comphelper/sequenceashashmap.hxx>
#include "docinf.hxx"

class ModelData_Impl;
class SfxStoringHelper
{
    friend class ModelData_Impl;

private:
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xFactory;
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > m_xFilterCFG;
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerQuery > m_xFilterQuery;
    ::com::sun::star::uno::Reference< ::drafts::com::sun::star::frame::XModuleManager > m_xModuleManager;
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > m_xNamedModManager;

    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > GetServiceFactory();
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > GetFilterConfiguration();
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerQuery > GetFilterQuery();
    ::com::sun::star::uno::Reference< ::drafts::com::sun::star::frame::XModuleManager > GetModuleManager();
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > GetNamedModuleManager();


public:
    SfxStoringHelper( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xFactory );

    sal_Bool GUIStoreModel(
                    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& xModel,
                    const ::rtl::OUString& aSlotName,
                    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgsSequence );

    static ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SearchForFilter(
                    const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerQuery >& xFilterQuery,
                    const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue >& aSearchRequest,
                    sal_Int32 nMustFlags,
                    sal_Int32 nDontFlags );

    static sal_Bool CheckFilterOptionsAppearence(
                    const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >& xFilterCFG,
                    const ::rtl::OUString& aFilterName );

    static void FillCopy( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& xModel,
                         SfxDocumentInfo& aDocInfoToFill );

    static void PrepareDocInfoForStore( SfxDocumentInfo& aDocInfoToClear );

    static void SetDocInfoState( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& xModel,
                                const SfxDocumentInfo& aDocInfoState,
                                sal_Bool bNoModify );

    static void ExecuteInfoDlg( const ::rtl::OUString& aTargetURL,
                                const ::rtl::OUString& aTitle,
                                SfxDocumentInfo &aDocInfo );

    static sal_Bool WarnUnacceptableFormat(
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& xModel,
                                    ::rtl::OUString aOldUIName,
                                    ::rtl::OUString aDefUIName,
                                    sal_Bool bCanProceedFurther );
};

#endif

