/*************************************************************************
 *
 *  $RCSfile: oleembobj.hxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: obo $ $Date: 2005-04-27 09:15:58 $
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

#ifndef _INC_OLEEMBOBJ_HXX_
#define _INC_OLEEMBOBJ_HXX_

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif

#ifndef _COM_SUN_STAR_EMBED_XEMBEDDEDOBJECT_HPP_
#include <com/sun/star/embed/XEmbeddedObject.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XVISUALOBJECT_HPP_
#include <com/sun/star/embed/XVisualObject.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XEMBEDPERSIST_HPP_
#include <com/sun/star/embed/XEmbedPersist.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XLINKAGESUPPORT_HPP_
#include <com/sun/star/embed/XLinkageSupport.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XCLASSIFIEDOBJECT_HPP_
#include <com/sun/star/embed/XClassifiedObject.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XCOMPONENTSUPPLIER_HPP_
#include <com/sun/star/embed/XComponentSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_VERBDESCR_HPP_
#include <com/sun/star/embed/VerbDescriptor.hpp>
#endif

#ifndef _COM_SUN_STAR_DOCUMENT_XEVENTBROADCASTER_HPP_
#include <com/sun/star/document/XEventBroadcaster.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_XCLOSEABLE_HPP_
#include <com/sun/star/util/XCloseable.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XCLOSELISTENER_HPP_
#include <com/sun/star/util/XCloseListener.hpp>
#endif

#ifndef _CPPUHELPER_IMPLBASE3_HXX_
#include <cppuhelper/implbase3.hxx>
#endif

namespace cppu {
    class OMultiTypeInterfaceContainerHelper;
}

class OleComponent;
class OwnView_Impl;
class OleEmbeddedObject : public ::cppu::WeakImplHelper3
                        < ::com::sun::star::embed::XEmbeddedObject
                        , ::com::sun::star::embed::XEmbedPersist
                        , ::com::sun::star::embed::XLinkageSupport >
{
    ::osl::Mutex    m_aMutex;

    OleComponent*   m_pOleComponent;

    ::cppu::OMultiTypeInterfaceContainerHelper* m_pInterfaceContainer;

    sal_Bool m_bReadOnly;

    sal_Int32 m_bDisposed;
    sal_Int32 m_nObjectState;
    sal_Int32 m_nTargetState;
    sal_Int32 m_nUpdateMode;

    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xFactory;

    ::com::sun::star::uno::Sequence< sal_Int8 > m_aClassID;
    ::rtl::OUString m_aClassName;

    ::com::sun::star::uno::Reference< ::com::sun::star::embed::XEmbeddedClient > m_xClientSite;

    ::rtl::OUString m_aContainerName;

    ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloseListener > m_xClosePreventer;

    sal_Bool m_bWaitSaveCompleted;
    sal_Bool m_bNewVisReplInStream;
    ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream > m_xNewCachedVisRepl;
    ::rtl::OUString m_aNewEntryName;
    ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage > m_xNewParentStorage;
    ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream > m_xNewObjectStream;

    ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream > m_xCachedVisualRepresentation;
    sal_Bool m_bVisReplInStream;
    sal_Bool m_bStoreVisRepl;

    sal_Bool m_bIsLink;

    // TODO/LATER: may need to cache more than one aspect in future
    sal_Bool m_bHasCachedSize; // the object has cached size
    sal_Bool m_bHasSizeToSet;  // the object has cached size that should be set to OLE component
    ::com::sun::star::awt::Size m_aCachedSize;
    sal_Int64 m_nCachedAspect;

    // cache the status of the object
    // TODO/LATER: may need to cache more than one aspect in future
    sal_Bool m_bGotStatus;
    sal_Int64 m_nStatus;
    sal_Int64 m_nStatusAspect;

    // embedded object related stuff
    ::rtl::OUString m_aEntryName;
    ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage > m_xParentStorage;
    ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream > m_xObjectStream;

    // link related stuff
    ::rtl::OUString m_aLinkURL; // ???

    // points to own view provider if the the object has no server
    OwnView_Impl*   m_pOwnView;

    // whether the object should be initialized from clipboard in case of default initialization
    sal_Bool m_bFromClipboard;

protected:

    void SwitchComponentToRunningState_Impl();

    void MakeEventListenerNotification_Impl( const ::rtl::OUString& aEventName );
    void StateChangeNotification_Impl( sal_Bool bBeforeChange, sal_Int32 nOldState, sal_Int32 nNewState );

    ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream > GetStreamForSaving();

    ::com::sun::star::uno::Sequence< sal_Int32 > GetIntermediateVerbsSequence_Impl( sal_Int32 nNewState );

    ::com::sun::star::uno::Sequence< sal_Int32 > GetReachableStatesList_Impl(
                        const ::com::sun::star::uno::Sequence< ::com::sun::star::embed::VerbDescriptor >& aVerbList );

    void CloseComponent();
    void Dispose();

    void SwitchOwnPersistence(
                const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xNewParentStorage,
                const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xNewObjectStream,
                const ::rtl::OUString& aNewName );

    void SwitchOwnPersistence(
                const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xNewParentStorage,
                const ::rtl::OUString& aNewName );

    void GetRidOfComponent();

    void StoreToLocation_Impl(
                            const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage,
                            const ::rtl::OUString& sEntName,
                            const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArguments,
                            const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lObjArgs,
                            sal_Bool bSaveAs )
        throw ( ::com::sun::star::uno::Exception );

    void InsertVisualCache_Impl(
            const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xTargetStream,
            const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xCachedVisualRepresentation )
        throw ( ::com::sun::star::uno::Exception );

    void RemoveVisualCache_Impl( const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xTargetStream )
        throw ( ::com::sun::star::uno::Exception );

    ::com::sun::star::embed::VisualRepresentation GetVisualRepresentationInNativeFormat_Impl(
                    const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream > xCachedVisRepr )
        throw ( ::com::sun::star::uno::Exception );

    ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream > TryToRetrieveCachedVisualRepresentation_Impl(
                    const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xStream )
        throw ();

public:
    // in case a new object must be created the class ID must be specified
    OleEmbeddedObject( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xFactory,
                        const ::com::sun::star::uno::Sequence< sal_Int8 >& aClassID,
                        const ::rtl::OUString& aClassName );

    // in case object will be loaded from a persistent entry or from a file the class ID will be detected on loading
    // factory can do it for OOo objects, but for OLE objects OS dependent code is required
    OleEmbeddedObject( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xFactory,
                        sal_Bool bLink );

    // this constructor let object be initialized from clipboard
    OleEmbeddedObject( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xFactory );

    virtual ~OleEmbeddedObject();

    sal_Bool SaveObject_Impl();
    sal_Bool OnShowWindow_Impl( sal_Bool bShow );
    void OnViewChanged_Impl();

    void CreateOleComponent_Impl( OleComponent* pOleComponent = NULL );
    void CreateOleComponentAndLoad_Impl( OleComponent* pOleComponent = NULL );
    void CreateOleComponentFromClipboard_Impl( OleComponent* pOleComponent = NULL );

    void SetObjectIsLink_Impl( sal_Bool bIsLink ) { m_bIsLink = bIsLink; }

    ::rtl::OUString GetContainerName_Impl() { return m_aContainerName; }

// XEmbeddedObject

    virtual void SAL_CALL changeState( sal_Int32 nNewState )
        throw ( ::com::sun::star::embed::UnreachableStateException,
                ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL getReachableStates()
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::RuntimeException );

    virtual sal_Int32 SAL_CALL getCurrentState()
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL doVerb( sal_Int32 nVerbID )
        throw ( ::com::sun::star::lang::IllegalArgumentException,
                ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::embed::UnreachableStateException,
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::embed::VerbDescriptor > SAL_CALL getSupportedVerbs()
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL setClientSite(
                const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XEmbeddedClient >& xClient )
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::embed::XEmbeddedClient > SAL_CALL getClientSite()
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL update()
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL setUpdateMode( sal_Int32 nMode )
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::RuntimeException );

    virtual sal_Int64 SAL_CALL getStatus( sal_Int64 nAspect )
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL setContainerName( const ::rtl::OUString& sName )
        throw ( ::com::sun::star::uno::RuntimeException );


// XVisualObject

    virtual void SAL_CALL setVisualAreaSize( sal_Int64 nAspect, const ::com::sun::star::awt::Size& aSize )
        throw ( ::com::sun::star::lang::IllegalArgumentException,
                ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::awt::Size SAL_CALL getVisualAreaSize( sal_Int64 nAspect )
        throw ( ::com::sun::star::lang::IllegalArgumentException,
                ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::embed::VisualRepresentation SAL_CALL getPreferredVisualRepresentation( ::sal_Int64 nAspect )
        throw ( ::com::sun::star::lang::IllegalArgumentException,
                ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException );

    virtual sal_Int32 SAL_CALL getMapUnit( sal_Int64 nAspect )
        throw ( ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException);


// XEmbedPersist

    virtual void SAL_CALL setPersistentEntry(
                    const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage,
                    const ::rtl::OUString& sEntName,
                    sal_Int32 nEntryConnectionMode,
                    const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArguments,
                    const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lObjArgs )
        throw ( ::com::sun::star::lang::IllegalArgumentException,
                ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::io::IOException,
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL storeToEntry( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::rtl::OUString& sEntName, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArguments, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lObjArgs )
        throw ( ::com::sun::star::lang::IllegalArgumentException,
                ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::io::IOException,
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL storeAsEntry(
                const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage,
                const ::rtl::OUString& sEntName,
                const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArguments,
                const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lObjArgs )
        throw ( ::com::sun::star::lang::IllegalArgumentException,
                ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::io::IOException,
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL saveCompleted( sal_Bool bUseNew )
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException );

    virtual sal_Bool SAL_CALL hasEntry()
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::RuntimeException );

    virtual ::rtl::OUString SAL_CALL getEntryName()
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::RuntimeException );

// XLinkageSupport

    virtual void SAL_CALL breakLink( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage,
                                     const ::rtl::OUString& sEntName )
        throw ( ::com::sun::star::lang::IllegalArgumentException,
                ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::io::IOException,
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException );

    virtual sal_Bool SAL_CALL isLink()
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::RuntimeException);

    virtual ::rtl::OUString SAL_CALL getLinkURL()
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException);

// XCommonEmbedPersist
    virtual void SAL_CALL storeOwn()
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::io::IOException,
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException );

    virtual sal_Bool SAL_CALL isReadonly()
        throw ( ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL reload(
                const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArguments,
                const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lObjArgs )
        throw ( ::com::sun::star::lang::IllegalArgumentException,
                ::com::sun::star::embed::WrongStateException,
                ::com::sun::star::io::IOException,
                ::com::sun::star::uno::Exception,
                ::com::sun::star::uno::RuntimeException );

// XClassifiedObject

    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getClassID()
        throw ( ::com::sun::star::uno::RuntimeException );

    virtual ::rtl::OUString SAL_CALL getClassName()
        throw ( ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL setClassInfo(
                const ::com::sun::star::uno::Sequence< sal_Int8 >& aClassID, const ::rtl::OUString& aClassName )
        throw ( ::com::sun::star::lang::NoSupportException,
                ::com::sun::star::uno::RuntimeException );

// XStateChangeBroadcaster
    virtual void SAL_CALL addStateChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStateChangeListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeStateChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStateChangeListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);


// XComponentSupplier

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloseable > SAL_CALL getComponent()
        throw ( ::com::sun::star::uno::RuntimeException );

// XCloseable

    virtual void SAL_CALL close( sal_Bool DeliverOwnership )
        throw ( ::com::sun::star::util::CloseVetoException,
                ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL addCloseListener(
                const ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloseListener >& Listener )
        throw ( ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL removeCloseListener(
                const ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloseListener >& Listener )
        throw ( ::com::sun::star::uno::RuntimeException );

// XEventBroadcaster
    virtual void SAL_CALL addEventListener(
                const ::com::sun::star::uno::Reference< ::com::sun::star::document::XEventListener >& Listener )
        throw ( ::com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL removeEventListener(
                const ::com::sun::star::uno::Reference< ::com::sun::star::document::XEventListener >& Listener )
        throw ( ::com::sun::star::uno::RuntimeException );

};

#endif

