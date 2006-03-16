/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: lngsvcmgr.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: vg $ $Date: 2006-03-16 14:05:08 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifndef _LINGUISTIC_LNGSVCMGR_HXX_
#define _LINGUISTIC_LNGSVCMGR_HXX_

#include <uno/lbnames.h>            // CPPU_CURRENT_LANGUAGE_BINDING_NAME macro, which specify the environment type
#include <cppuhelper/implbase4.hxx> // helper for implementations

#ifndef _CPPUHELPER_INTERFACECONTAINER_H_
#include <cppuhelper/interfacecontainer.h>  //OMultiTypeInterfaceContainerHelper
#endif


#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif

#ifndef _COM_SUN_STAR_LINGUISTIC2_XLINGUSERVICEMANAGER_HPP_
#include <com/sun/star/linguistic2/XLinguServiceManager.hpp>
#endif
#ifndef _COM_SUN_STAR_LINGUISTIC2_XAVAILABLELOCALES_HPP_
#include <com/sun/star/linguistic2/XAvailableLocales.hpp>
#endif

#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif

#include <vcl/timer.hxx>

#include "misc.hxx"
#include "defs.hxx"

class SpellCheckerDispatcher;
class HyphenatorDispatcher;
class ThesaurusDispatcher;
class SvcInfoArray;
class LngSvcMgrListenerHelper;

namespace com { namespace sun { namespace star { namespace linguistic2 {
    class XLinguServiceEventBroadcaster;
    class XSpellChecker;
    class XHyphenator;
    class XThesaurus;
} } } }

///////////////////////////////////////////////////////////////////////////


class LngSvcMgr :
    public cppu::WeakImplHelper4
    <
        com::sun::star::linguistic2::XLinguServiceManager,
        com::sun::star::linguistic2::XAvailableLocales,
        com::sun::star::lang::XComponent,
        com::sun::star::lang::XServiceInfo
    >,
    private utl::ConfigItem
{
    ::cppu::OInterfaceContainerHelper                   aEvtListeners;

    com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XSpellChecker >  xSpellDsp;
    com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XHyphenator >    xHyphDsp;
    com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XThesaurus >     xThesDsp;

    com::sun::star::uno::Reference<
        ::com::sun::star::lang::XEventListener >        xListenerHelper;

    com::sun::star::uno::Sequence<
        com::sun::star::lang::Locale >                  aAvailSpellLocales;
    com::sun::star::uno::Sequence<
        com::sun::star::lang::Locale >                  aAvailHyphLocales;
    com::sun::star::uno::Sequence<
        com::sun::star::lang::Locale >                  aAvailThesLocales;

    SpellCheckerDispatcher *                            pSpellDsp;
    HyphenatorDispatcher *                              pHyphDsp;
    ThesaurusDispatcher *                               pThesDsp;

    LngSvcMgrListenerHelper *                           pListenerHelper;

    SvcInfoArray *                                      pAvailSpellSvcs;
    SvcInfoArray *                                      pAvailHyphSvcs;
    SvcInfoArray *                                      pAvailThesSvcs;

    BOOL bDisposing;
    BOOL bHasAvailSpellLocales;
    BOOL bHasAvailHyphLocales;
    BOOL bHasAvailThesLocales;

    // disallow copy-constructor and assignment-operator for now
    LngSvcMgr(const LngSvcMgr &);
    LngSvcMgr & operator = (const LngSvcMgr &);

    void    GetAvailableSpellSvcs_Impl();
    void    GetAvailableHyphSvcs_Impl();
    void    GetAvailableThesSvcs_Impl();
    void    GetListenerHelper_Impl();
    void    GetSpellCheckerDsp_Impl( sal_Bool bSetSvcList = sal_True );
    void    GetHyphenatorDsp_Impl( sal_Bool bSetSvcList = sal_True );
    void    GetThesaurusDsp_Impl( sal_Bool bSetSvcList = sal_True );

    void    SetCfgServiceLists( SpellCheckerDispatcher &rSpellDsp );
    void    SetCfgServiceLists( HyphenatorDispatcher &rHyphDsp );
    void    SetCfgServiceLists( ThesaurusDispatcher &rThesDsp );

    BOOL    SaveCfgSvcs( const String &rServiceName );

    void    SetAvailableCfgServiceLists( LinguDispatcher &rDispatcher,
                    const SvcInfoArray &rAvailSvcs );

    // utl::ConfigItem (to allow for listening of changes of relevant properties)
    virtual void    Notify( const com::sun::star::uno::Sequence< rtl::OUString > &rPropertyNames );
    virtual void    Commit();

public:
    LngSvcMgr();
    virtual ~LngSvcMgr();

    // XLinguServiceManager
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::linguistic2::XSpellChecker > SAL_CALL
        getSpellChecker()
            throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::linguistic2::XHyphenator > SAL_CALL
        getHyphenator()
            throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::linguistic2::XThesaurus > SAL_CALL
        getThesaurus()
            throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL
        addLinguServiceManagerListener(
                const ::com::sun::star::uno::Reference<
                    ::com::sun::star::lang::XEventListener >& xListener )
            throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL
        removeLinguServiceManagerListener(
                const ::com::sun::star::uno::Reference<
                    ::com::sun::star::lang::XEventListener >& xListener )
            throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
        getAvailableServices(
                const ::rtl::OUString& rServiceName,
                const ::com::sun::star::lang::Locale& rLocale )
            throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL
        setConfiguredServices(
                const ::rtl::OUString& rServiceName,
                const ::com::sun::star::lang::Locale& rLocale,
                const ::com::sun::star::uno::Sequence<
                    ::rtl::OUString >& rServiceImplNames )
            throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
        getConfiguredServices(
                const ::rtl::OUString& rServiceName,
                const ::com::sun::star::lang::Locale& rLocale )
            throw(::com::sun::star::uno::RuntimeException);

    // XAvailableLocales
    virtual ::com::sun::star::uno::Sequence<
            ::com::sun::star::lang::Locale > SAL_CALL
        getAvailableLocales(
                const ::rtl::OUString& rServiceName )
            throw(::com::sun::star::uno::RuntimeException);

    // XComponent
    virtual void SAL_CALL
        dispose()
            throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL
        addEventListener(
                const ::com::sun::star::uno::Reference<
                    ::com::sun::star::lang::XEventListener >& xListener )
            throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL
        removeEventListener(
                const ::com::sun::star::uno::Reference<
                    ::com::sun::star::lang::XEventListener >& xListener )
            throw(::com::sun::star::uno::RuntimeException);

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL
        getImplementationName()
            throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL
        supportsService( const ::rtl::OUString& ServiceName )
            throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
        getSupportedServiceNames()
            throw(::com::sun::star::uno::RuntimeException);


    static inline ::rtl::OUString
        getImplementationName_Static();
    static ::com::sun::star::uno::Sequence< ::rtl::OUString >
        getSupportedServiceNames_Static() throw();

    BOOL    AddLngSvcEvtBroadcaster(
                const ::com::sun::star::uno::Reference<
                    ::com::sun::star::linguistic2::XLinguServiceEventBroadcaster > &rxBroadcaster );
    BOOL    RemoveLngSvcEvtBroadcaster(
                const ::com::sun::star::uno::Reference<
                    ::com::sun::star::linguistic2::XLinguServiceEventBroadcaster > &rxBroadcaster );
};


inline ::rtl::OUString LngSvcMgr::getImplementationName_Static()
{
    return A2OU( "com.sun.star.lingu2.LngSvcMgr" );
}


///////////////////////////////////////////////////////////////////////////

#endif

