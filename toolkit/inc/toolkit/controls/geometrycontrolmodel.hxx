/*************************************************************************
 *
 *  $RCSfile: geometrycontrolmodel.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: tbe $ $Date: 2001-02-28 10:51:22 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _TOOLKIT_HELPERS_GEOMETRYCONTROLMODEL_HXX_
#define _TOOLKIT_HELPERS_GEOMETRYCONTROLMODEL_HXX_

#ifndef _COMPHELPER_BROADCASTHELPER_HXX_
#include <comphelper/broadcasthelper.hxx>
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
#ifndef _COMPHELPER_PROPERTY_AGGREGATION_HXX_
#include <comphelper/propagg.hxx>
#endif
#ifndef _COMPHELPER_PROPERTY_ARRAY_HELPER_HXX_
#include <comphelper/proparrhlp.hxx>
#endif
#ifndef _COMPHELPER_PROPERTYCONTAINER_HXX_
#include <comphelper/propertycontainer.hxx>
#endif
#ifndef _CPPUHELPER_WEAKAGG_HXX_
#include <cppuhelper/weakagg.hxx>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XSCRIPTEVENTSSUPPLIER_HPP_
#include <com/sun/star/script/XScriptEventsSupplier.hpp>
#endif
//#ifndef _COM_SUN_STAR_XNAMECONTAINER_HPP_
//#include <com/sun/star/container/XNameContainer.hpp>
//#endif


FORWARD_DECLARE_INTERFACE( lang, XMultiServiceFactory )
FORWARD_DECLARE_INTERFACE( script, XNameContainer )

//........................................................................
// namespace toolkit
// {
//........................................................................

    //====================================================================
    //= OGeometryControlModel_Base
    //====================================================================
    class OGeometryControlModel_Base
        :public ::comphelper::OMutexAndBroadcastHelper
        ,public ::comphelper::OPropertySetAggregationHelper
        ,public ::comphelper::OPropertyContainer
        ,public ::cppu::OWeakAggObject
        ,public ::com::sun::star::script::XScriptEventsSupplier
    {
    protected:
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation >
                    m_xAggregate;
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer >
                    mxEventContainer;

        // <properties>
        sal_Int32       m_nPosX;
        sal_Int32       m_nPosY;
        sal_Int32       m_nWidth;
        sal_Int32       m_nHeight;
        ::rtl::OUString m_nName;
        sal_Int16       m_nTabIndex;
        sal_Int16       m_nClassId;
        // </properties>

    public:
        /**
            @param _pAggregateInstance
                the object to be aggregated. The refcount of the instance given MUST be 0!
        */
        OGeometryControlModel_Base(::com::sun::star::uno::XAggregation* _pAggregateInstance);

    protected:
        ~OGeometryControlModel_Base();

        // XAggregation
        ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type& _aType ) throw(::com::sun::star::uno::RuntimeException);

        // XInterface
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) throw(::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL acquire(  ) throw();
        virtual void SAL_CALL release(  ) throw();

        // OPropertySetHelper overridables
        virtual sal_Bool SAL_CALL convertFastPropertyValue(
                ::com::sun::star::uno::Any& _rConvertedValue, ::com::sun::star::uno::Any& _rOldValue,
                sal_Int32 _nHandle, const ::com::sun::star::uno::Any& _rValue )
            throw (::com::sun::star::lang::IllegalArgumentException);

        virtual void SAL_CALL setFastPropertyValue_NoBroadcast(
                sal_Int32 _nHandle, const ::com::sun::star::uno::Any& _rValue)
            throw (::com::sun::star::uno::Exception);

        virtual void SAL_CALL getFastPropertyValue(
            ::com::sun::star::uno::Any& _rValue, sal_Int32 _nHandle) const;

        // XPropertyset
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo> SAL_CALL getPropertySetInfo() throw(::com::sun::star::uno::RuntimeException);

        // OPropertySetAggregationHelper overridables
        virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper() = 0;

        //XScriptEventsSupplier
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer >
            SAL_CALL getEvents(  ) throw(::com::sun::star::uno::RuntimeException);

    };

    //====================================================================
    //= OTemplateInstanceDisambiguation
    //====================================================================
    template <class CONTROLMODEL>
    class OTemplateInstanceDisambiguation
    {
    };

    //====================================================================
    //= OGeometryControlModel
    //====================================================================
    /*  example for usage:
            Reference< XAggregation > xIFace = new ::toolkit::OGeometryControlModel< UnoControlButtonModel > ();
    */
    template <class CONTROLMODEL>
    class OGeometryControlModel
        :public OGeometryControlModel_Base
        ,public ::comphelper::OAggregationArrayUsageHelper< OTemplateInstanceDisambiguation< CONTROLMODEL > >
    {
    public:
        OGeometryControlModel();

    protected:
        // OAggregationArrayUsageHelper overridables
        virtual void fillProperties(
            ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& _rProps,
            ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& _rAggregateProps
            ) const;

        // OPropertySetAggregationHelper overridables
        virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();
    };

#include "toolkit/controls/geometrycontrolmodel_impl.hxx"

//........................................................................
// }    // namespace toolkit
//........................................................................

#endif // _TOOLKIT_HELPERS_GEOMETRYCONTROLMODEL_HXX_

/*************************************************************************
 * history:
 *  $Log: not supported by cvs2svn $
 *  Revision 1.2  2001/02/21 17:21:57  ab
 *  Support for XScriptEventsSupplier added
 *
 *  Revision 1.1  2001/01/24 14:57:23  mt
 *  model for dialog controls (weith pos/size)
 *
 *
 *  Revision 1.0 17.01.01 11:36:59  fs
 ************************************************************************/

