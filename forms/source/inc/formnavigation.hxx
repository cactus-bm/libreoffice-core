/*************************************************************************
 *
 *  $RCSfile: formnavigation.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-16 10:45:05 $
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

#ifndef FORMS_FORM_NAVIGATION_HXX
#define FORMS_FORM_NAVIGATION_HXX

#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDERINTERCEPTION_HPP_
#include <com/sun/star/frame/XDispatchProviderInterception.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTATUSLISTENER_HPP_
#include <com/sun/star/frame/XStatusListener.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XURLTRANSFORMER_HPP_
#include <com/sun/star/util/XURLTransformer.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif
#ifndef FORMS_SOLAR_DISPATCHER_HXX
#include "featuredispatcher.hxx"
#endif
#include <vector>
#include <map>
#include <memory>


//.........................................................................
namespace frm
{
//.........................................................................

    class UrlTransformer;
    class ControlFeatureInterception;

    //==================================================================
    //= OFormNavigationHelper
    //==================================================================
    typedef ::cppu::ImplHelper2 <   ::com::sun::star::frame::XDispatchProviderInterception
                                ,   ::com::sun::star::frame::XStatusListener
                                >   OFormNavigationHelper_Base;

    class OFormNavigationHelper
                        :public OFormNavigationHelper_Base
                        ,public IFeatureDispatcher
    {
    private:
        struct FeatureInfo
        {
            ::com::sun::star::util::URL                                             aURL;
            ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >  xDispatcher;
            sal_Bool                                                                bCachedState;
            ::com::sun::star::uno::Any                                              aCachedAdditionalState;

            FeatureInfo() : bCachedState( sal_False ) { }
        };
        typedef ::std::map< sal_Int32, FeatureInfo >    FeatureMap;

    private:
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
                            m_xORB;
        ::std::auto_ptr< ControlFeatureInterception >
                            m_pFeatureInterception;

        // all supported features
        FeatureMap          m_aSupportedFeatures;
        // all features which we have an external dispatcher for
        sal_Int32           m_nConnectedFeatures;

    protected:
        inline const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&
            getORB( ) const { return m_xORB; }

    protected:
        OFormNavigationHelper( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB );
        ~OFormNavigationHelper();

        // XComponent
        void SAL_CALL dispose(  ) throw( ::com::sun::star::uno::RuntimeException );

        // XDispatchProviderInterception
        virtual void SAL_CALL registerDispatchProviderInterceptor( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterceptor >& Interceptor ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL releaseDispatchProviderInterceptor( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterceptor >& Interceptor ) throw (::com::sun::star::uno::RuntimeException);

        // XStatusListener
        virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& State ) throw (::com::sun::star::uno::RuntimeException);

        // XEventListener
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException);

        // IFeatureDispatcher
        virtual void            dispatch( sal_Int32 _nFeatureId ) const;
        virtual void            dispatchWithArgument( sal_Int32 _nFeatureId, const sal_Char* _pParamName, const ::com::sun::star::uno::Any& _rParamValue ) const;
        virtual bool            isEnabled( sal_Int32 _nFeatureId ) const;
        virtual bool            getBooleanState( sal_Int32 _nFeatureId ) const;
        virtual ::rtl::OUString getStringState( sal_Int32 _nFeatureId ) const;
        virtual sal_Int32       getIntegerState( sal_Int32 _nFeatureId ) const;

        // own overridables
        /** is called when the interceptors have.
            <p>The default implementations simply calls <member>updateDispatches</member>,
            derived classes can prevent this in certain cases, or do additional handling.</p>
        */
        virtual void    interceptorsChanged( );

        /** called when the status of a feature changed

            <p>The default implementation does nothing.</p>

            <p>If the feature in question does support more state information that just the
            enabled/disabled state, then this additional information is to be retrieved in
            a separate call.</p>

            @param _nFeatureId
                the id of the feature
            @param _bEnabled
                determines if the features is enabled or disabled
            @see getBooleanState
        */
        virtual void    featureStateChanged( sal_Int32 _nFeatureId, sal_Bool _bEnabled );

        /** notification for (potential) changes in the state of all features
            <p>The base class implementation does nothing. Derived classes could force
            their peer to update it's state, depending on the result of calls to
            <member>IFeatureDispatcher::isEnabled</member>.</p>
        */
        virtual void    allFeatureStatesChanged( );

        /** retrieves the list of supported features
            <p>To be overridden by derived classes</p>
            @param _rFeatureIds
                the array of features to support. Out parameter to fill by the derivee's implementation
            @pure
        */
        virtual void    getSupportedFeatures( ::std::vector< sal_Int32 >& /* [out] */ _rFeatureIds ) = 0;

    protected:
        /** update all our dispatches which are controlled by our dispatch interceptors
        */
        void    updateDispatches();

        /** connect to the dispatch interceptors
        */
        void    connectDispatchers();

        /** disconnect from the dispatch interceptors
        */
        void    disconnectDispatchers();

        /** queries the interceptor chain for a dispatcher for the given URL
        */
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >
                queryDispatch( const ::com::sun::star::util::URL& _rURL );

        /** invalidates the set of supported features

            <p>This will invalidate all structures which are tied to the set of supported
            features. All dispatches will be disconnected.<br/>
            No automatic re-connection to potential external dispatchers is done, instead,
            you have to call updateDispatches explicitly, if necessary.</p>
        */
        void    invalidateSupportedFeaturesSet();

    private:
        /** initialize m_aSupportedFeatures, if necessary
        */
        void    initializeSupportedFeatures();
    };

    //==================================================================
    //= OFormNavigationMapper
    //==================================================================
    /** helper class mapping between feature ids and feature URLs
    */
    class OFormNavigationMapper
    {
    private:
        ::std::auto_ptr< UrlTransformer >   m_pUrlTransformer;

    public:
        OFormNavigationMapper(
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB
        );
        ~OFormNavigationMapper( );

        /** retrieves the ASCII representation of a feature URL belonging to an id

            @complexity O(log n)
            @return NULL if the given id is not a known feature id (which is a valid usage)
        */
        const char* getFeatureURLAscii( sal_Int32 _nFeatureId );

        /** retrieves the feature URL belonging to an feature id

            @complexity O(log n), with n being the number of all potentially known URLs
            @return
                <TRUE/> if and only if the given id is a known feature id
                (which is a valid usage)
        */
        bool        getFeatureURL( sal_Int32 _nFeatureId, ::com::sun::star::util::URL& /* [out] */ _rURL );

        /** retrieves the feature id belonging to an feature URL

            @complexity O(n), with n being the number of all potentially known URLs
            @return
                the id of the feature URL, or -1 if the URl is not known
                (which is a valid usage)
        */
        sal_Int32   getFeatureId( const ::rtl::OUString& _rCompleteURL );

    private:
        OFormNavigationMapper( );                                           // never implemented
        OFormNavigationMapper( const OFormNavigationMapper& );              // never implemented
        OFormNavigationMapper& operator=( const OFormNavigationMapper& );   // never implemented
    };

//.........................................................................
}   // namespace frm
//.........................................................................

#endif // FORMS_FORM_NAVIGATION_HXX
