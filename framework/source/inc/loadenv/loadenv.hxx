/*************************************************************************
 *
 *  $RCSfile: loadenv.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-02-02 13:53:53 $
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
#ifndef __FRAMEWORK_LOADENV_LOADENV_HXX_
#define __FRAMEWORK_LOADENV_LOADENV_HXX_

//_______________________________________________
// includes of own project

#ifndef __FRAMEWORK_LOADENV_LOADENVEXCEPTION_HXX_
#include <loadenv/loadenvexception.hxx>
#endif

#ifndef __FRAMEWORK_LOADENV_ACTIONLOCKGUARD_HXX_
#include <loadenv/actionlockguard.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
#include <threadhelp/threadhelpbase.hxx>
#endif

//_______________________________________________
// includes of uno interface

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XCOMPONENTLOADER_HPP_
#include <com/sun/star/frame/XComponentLoader.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XFRAMELOADER_HPP_
#include <com/sun/star/frame/XFrameLoader.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XLOADEVENTLISTENER_HPP_
#include <com/sun/star/frame/XLoadEventListener.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XDISPATCHRESULTLISTENER_HPP_
#include <com/sun/star/frame/XDispatchResultListener.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_URL_HPP_
#include <com/sun/star/util/URL.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_IllegalArgumentException_HPP_
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#endif

#ifndef _COM_SUN_STAR_IO_IOException_HPP_
#include <com/sun/star/io/IOException.hpp>
#endif

//_______________________________________________
// includes of an other project

#ifndef _COMPHELPER_MEDIADESCRIPTOR_HXX_
#include <comphelper/mediadescriptor.hxx>
#endif

#ifndef _COMPHELPER_SEQUENCEASHASHMAP_HXX_
#include <comphelper/sequenceashashmap.hxx>
#endif

#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif

//_______________________________________________
// namespace

namespace framework{

namespace css = ::com::sun::star;

//_______________________________________________
// definitions

/** @short  implements general mechainsm for loading documents.

    @descr  An instance of this class can be used inside the API calls
            XComponentLoader::loadComponentFromURL() and XDispatch::dispatch()
            (of course in its derived interfaces too :-)).

    @author as96863
 */
class LoadEnv : private ThreadHelpBase
{
    //___________________________________________
    // structs, types, etc.

    public:

        /** @short  enable/disable special features
                    of a load request.

            @desrc  Such features must outcome without
                    any special parameters.
                    To make enableing/disabling of
                    features very easy (e.g. at the ctor of
                    this class) these values must be combinable
                    as flags. That means: its values must be in
                    range of [2^n]!
         */
        enum EFeature
        {
            /// we should be informed, if no feature is enabled :-)
            E_NO_FEATURE = 0,
            /// enable using of UI elements during loading (means progress, interaction handler etcpp.)
            E_WORK_WITH_UI = 1,
            /// enable loading of resources, which are not related to a target frame! (see concept of ContentHandler)
            E_ALLOW_CONTENTHANDLER = 2
        };

        //_______________________________________

        /** @short  classify a content.

            @descr  The load environment must know, if a content
                    is related to a target frame or not. Only "visible"
                    components, which full fill the requirements of the
                    model-controller-view paradigm can be loaded into a frame.
                    Such contents are classified as E_CAN_BE_LOADED.

                    But e.g. for the dispatch framework exists special ContentHandler
                    objects, which can load a content in "non visible" mode ...
                    and do not need a target frame for its operation. Such
                    ContentHandler e.g. plays sounds.
                    Such contents are classified as E_CAN_BE_HANDLED.

                    And last but not least a content can be "not valid" in general.
         */
        enum EContentType
        {
            /// identifies a content, which seems to be invalid in general
            E_UNSUPPORTED_CONTENT,
            /// identifies a content, which can be used with a ContentHandler and is not related to a target frame
            E_CAN_BE_HANDLED,
            /// identifies a content, which can be loaded into a target frame
            E_CAN_BE_LOADED,
            /// special mode for non real loading, In such case the model is given directly!
            E_CAN_BE_SET
        };

    //___________________________________________
    // member

    private:

        /** @short  reference to an uno service manager, which must be used
                    to created on needed services on demand.
         */
        css::uno::Reference< css::lang::XMultiServiceFactory > m_xSMGR;

        /** @short  points to the frame, which uses this LoadEnv object
                    and must be used to start target search there.
         */
        css::uno::Reference< css::frame::XFrame > m_xBaseFrame;

        /** @short  points to the frame, into which the new component was loaded.

            @descr  Note: This reference will be empty if loading failed
                    or a non visible content was loaded!
                    It can be the same frame as m_xBaseFrame it describe, in case
                    the target "_self", "" or the search flag "SELF" was used.
                    Otherwhise its the new created or recycled frame, which was
                    used for loading and contains further the new component.

                    Please use method getTarget() or getTargetComponent()
                    to return the frame/controller or model to any interested
                    user of the results of this load request.
         */
        css::uno::Reference< css::frame::XFrame > m_xTargetFrame;

        /** @short  contains the name of the target, in which the specified resource
                    of this instance must be loaded.
         */
        ::rtl::OUString m_sTarget;

        /** @short  if m_sTarget is not a special one, this flags regulate searching
                    of a suitable one.
         */
        sal_Int32 m_nSearchFlags;

        /** @short  contains all needed informations about the resource,
                    which should be loaded.

            @descr  Inside this struct e.g. the URL, its type and filter name,
                    the stream or a model directly are saved.
         */
        ::comphelper::MediaDescriptor m_lMediaDescriptor;

        /** @short  because the mediadescriptor contains the complete URL ... but
                    some functionality need the structured version, we hold it twice :-(.
         */
        css::util::URL m_aURL;

        /** @short  enable/disable special features of a load request. */
        EFeature m_eFeature;

        /** @short  classify the content, which should be loaded by this instance. */
        EContentType m_eContentType;

        /** @short  it indicates, that the member m_xTargetFrame was new created for this
                    load request and must be closed in case loading (not handling!)
                    operation failed. The default value is FALSE!
         */
        sal_Bool m_bCloseFrameOnError;

        /** @short  it indicates, that the old document (which was located inside m_xBaseFrame
                    in combination with the m_sTarget value "_self") was suspended.
                    Normaly it will be replaced by the new loaded document. But in case
                    loading (not handling!) failed, it must be reactivated.
                    The default value is FALSE!
         */
        sal_Bool m_bReactivateControllerOnError;

        /** @short  it holds one (!) asynchronous used contenthandler or frameloader
                    alive, till the asynchronous operation will be finished.
         */
        css::uno::Reference< css::uno::XInterface > m_xAsynchronousJob;

        /** @short  holds the information about the finished load process.

            @descr  The content of m_xTargetFrame cant be used as valid indicator,
                    (in case the micht existing old document was reactivated)
                    we must hold the result of the load process explicitly.
         */
        sal_Bool m_bLoaded;

        /** @short      holds an XActionLock on the internal used task member.

            @seealso    m_xTargetFrame
         */
        ActionLockGuard m_aTargetLock;

        /** TODO document me ... */
        void* m_pCheck;

    //___________________________________________
    // native interface

    public:

        /** @short  initialize a new instance of this load environment.

            @param  xSMGR
                    reference to an uno service manager, which can be used internaly
                    to create on needed services on demand.

            @throw  Currently there is no reason to throw such exception!

            @throw  A RuntimeException in case any internal process indicates, that
                    the whole runtime cant be used any longer.
         */
        LoadEnv(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR)
            throw(LoadEnvException, css::uno::RuntimeException);

        //_______________________________________

        /** @short  deinitialize an instance of this class in the right way.
         */
        virtual ~LoadEnv();

        //_______________________________________

        /** @short  DRAFT TODO
         */
        static css::uno::Reference< css::lang::XComponent > loadComponentFromURL(const css::uno::Reference< css::frame::XComponentLoader >&    xLoader,
                                                                                 const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR  ,
                                                                                 const ::rtl::OUString&                                        sURL   ,
                                                                                 const ::rtl::OUString&                                        sTarget,
                                                                                       sal_Int32                                               nFlags ,
                                                                                 const css::uno::Sequence< css::beans::PropertyValue >&        lArgs  )
            throw(css::lang::IllegalArgumentException,
                  css::io::IOException               ,
                  css::uno::RuntimeException         );

        //_______________________________________

        /** @short  set some changeable parameters for a new load request.

            @descr  The parameter for targeting, the content description, and
                    some environment specifier (UI, dispatch functionality)
                    can be set here ... BEFORE the real load process is started
                    by calling startLoading(). Of course a still running load request
                    will be detected here and a suitable exception will be thrown.
                    Such constellation can be detected outside by using provided
                    synchronisation methods or callbacks.

            @param  sURL
                    points to the resource, which should be loaded.

            @param  lMediaDescriptor
                    contains additional informations for the following load request.

            @param  xBaseFrame
                    points to the frame which must be used as start point for target search.

            @param  sTarget
                    regulate searching/creating of frames, which should contain the
                    new loaded component afterwards.

            @param  nSearchFlags
                    regulate searching of targets, if sTarget is not a special one.

            @param  eFeature
                    flag field, which enable/disable special features of this
                    new instance for following load call.

            @param  eContentType
                    classify the given content.
                    This value is set to a default value "UNKNWON_CONTENT", which force
                    an internal check, if this content is loadable or not.
                    But may this check was already made by the caller of this method and
                    passing this information to this LoadEnv instance can supress this
                    might expensive check.
                    That can be usefull in case this information is needed outside too,
                    to decide if its neccessary to create some resources for this load
                    request ... or to reject the request imidiatly if it seems to be not
                    loadable in general.

            @throw  A LoadEnvException e.g. if another load operation is till in progress
                    or initialization of a new one fail by other reasons.
                    The real reason, a suitable message and ID will be given here immidiatly.

            @throw  A RuntimeException in case any internal process indicates, that
                    the whole runtime cant be used any longer.
         */
        virtual void initializeLoading(const ::rtl::OUString&                                           sURL            ,
                                       const css::uno::Sequence< css::beans::PropertyValue >&           lMediaDescriptor,
                                       const css::uno::Reference< css::frame::XFrame >&                 xBaseFrame      ,
                                       const ::rtl::OUString&                                           sTarget         ,
                                             sal_Int32                                                  nSearchFlags    ,
                                             EFeature                                                   eFeature        = E_NO_FEATURE         ,
                                             EContentType                                               eContentType    = E_UNSUPPORTED_CONTENT)
            throw(LoadEnvException, css::uno::RuntimeException);

        //_______________________________________

        /** @short  start loading of the resource represented by this loadenv instance.

            @descr  There is no direct return value possible here. Because it depends
                    from the usage of this instance! E.g. for loading a "visible component"
                    a frame with a controller/model inside can be possible. For loading
                    of a "non visible component" only an information about a successfully start
                    can be provided.
                    Further it cant be guranteed, that the internal process runs synchronous.
                    Thats why we preferr using of specialized methods afterwards e.g. to:
                        - wait till the internal job will be finished
                          and get the results
                        - or to let it run without any further control from outside.

            @throw  A LoadEnvException if start of the load process failed (because
                    another is still in progress!).
                    The reason, a suitable message and ID will be given here immidiatly.

            @throw  A RuntimeException in case any internal process indicates, that
                    the whole runtime cant be used any longer.
         */
        virtual void startLoading()
            throw(LoadEnvException, css::uno::RuntimeException);

        //_______________________________________

        /** @short  wait for an alreay running load request (started by calling
                    startLoading() before).

            @descr  The timeout parameter can be used to wait some times only
                    or forever. The return value indicates if the load request
                    was finished during the specified timeout period.
                    But it indicates not, if the load request was successfully or not!

            @param  nTimeout
                    specify a timeout in [ms].
                    A value 0 let it wait forever!

            @return TRUE if the started load process could be finished in time;
                    FALSE if the specified time was over.

            @throw  ... currently not used :-)

            @throw  A RuntimeException in case any internal process indicates, that
                    the whole runtime cant be used any longer.
         */
        virtual sal_Bool waitWhileLoading(sal_uInt32 nTimeout = 0)
            throw(LoadEnvException, css::uno::RuntimeException);

        //_______________________________________
        /** TODO document me ... */
        virtual void cancelLoading()
            throw(LoadEnvException, css::uno::RuntimeException);

        //_______________________________________
        /** TODO document me ... */
        virtual css::uno::Reference< css::frame::XFrame > getTarget() const;

        //_______________________________________
        /** TODO document me ... */
        virtual css::uno::Reference< css::lang::XComponent > getTargetComponent() const;
/*
    //___________________________________________
    // helper uno interface!
    // You have to use the native interface only!

    public:

        //_______________________________________
        // frame.XLoadEventListener
        virtual void SAL_CALL loadFinished(const css::uno::Reference< css::frame::XFrameLoader >& xLoader)
            throw(css::uno::RuntimeException);

        virtual void SAL_CALL loadCancelled(const css::uno::Reference< css::frame::XFrameLoader >& xLoader)
            throw(css::uno::RuntimeException);

        //_______________________________________
        // frame.XDispatchResultListener
        virtual void SAL_CALL dispatchFinished(const css::frame::DispatchResultEvent& aEvent)
            throw(css::uno::RuntimeException);

        //_______________________________________
        // lang.XEventListener
        virtual void SAL_CALL disposing(const css::lang::EventObject& aEvent)
            throw(css::uno::RuntimeException);
*/
    //___________________________________________
    // static interface

    public:

        /** @short      checks if the specified content can be handled by a
                        ContentHandler only and is not related to a target frame,
                        or if it can be loaded by a FrameLoader into a target frame
                        as "visible" component.

            @descr      using:
                            switch(classifyContent(...))
                            {
                                case E_CAN_BE_HANDLED :
                                    handleIt(...);
                                    break;

                                case E_CAN_BE_LOADED :
                                    xFrame = locateTargetFrame();
                                    loadIt(xFrame);
                                    break;

                                case E_NOT_A_CONTENT :
                                default              : throw ...;
                            }

            @param      sURL
                        describe the content.

            @param      lMediaDescriptor
                        describe the content more detailed!

            @return     A suitable enum value, which classify the specified content.
         */
        static EContentType classifyContent(const ::rtl::OUString&                                 sURL            ,
                                            const css::uno::Sequence< css::beans::PropertyValue >& lMediaDescriptor);

        /** TODO document me ... */
        void impl_setResult(sal_Bool bResult);

        /** TODO document me ... */
        css::uno::Reference< css::uno::XInterface > impl_searchLoader();

    //___________________________________________
    // private helper

    private:

        /** @short  tries to detect the type and the filter of the specified content.

            @descr  This method actualize the available media descriptor of this instance,
                    so it contains the right type, a corresponding filter, may a
                    valid frame loader etc. In case detection failed, this descriptor
                    is corrected first, before a suitable exception will be thrown.
                    (Excepting a RuntimeException occure!)

            @attention  Not all types we know, are supported by filters. So it does not
                        indicates an error, if no suitable filter(loader etcpp will be found
                        for a type. But a type must be detected for the specified content.
                        Otherwhise its an error and loading cant be finished successfully.

            @throw  A LoadEnvException if detection failed.

            @throw  A RuntimeException in case any internal process indicates, that
                    the whole runtime cant be used any longer.
         */
        void impl_detectTypeAndFilter()
            throw(LoadEnvException, css::uno::RuntimeException);

        //_______________________________________

        /** @short  tries to ask user for it's filter decision in case
                    normal detection failed.

            @descr  We use a may existing interaction handler to do so.

            @return [string]
                    the type selected by the user.

            @attention  Internaly we update the member m_lMediaDescriptor!
         */
        ::rtl::OUString impl_askUserForTypeAndFilterIfAllowed()
            throw(LoadEnvException, css::uno::RuntimeException);

        //_______________________________________

        /** @short  tries to use ContentHandler objects for loading.

            @descr  It searches for a suitable content handler object, registered
                    for the detected content type (must be done before by calling
                    impl_detectTypeAndFilter()). Because such handler does not depend
                    from a real target frame, location of such frame will be
                    supressed here.
                    In case handle failed all new created resources will be
                    removed before a suitable exception is thrown.
                    (Excepting a RuntimeException occure!)

            @return TODO

            @throw  A LoadEnvException if handling failed.

            @throw  A RuntimeException in case any internal process indicates, that
                    the whole runtime cant be used any longer.
         */
        sal_Bool impl_handleContent()
            throw(LoadEnvException, css::uno::RuntimeException);

        //_______________________________________

        /** @short  tries to use FrameLoader objects for loading.

            @descr  First the target frame will be located. If it could be found
                    or new created a filter/frame loader will be instanciated and
                    used to load the content into this frame.
                    In case loading failed all new created resources will be
                    removed before a suitable exception is thrown.
                    (Excepting a RuntimeException occure!)

            @return TODO

            @throw  A LoadEnvException if loading failed.

            @throw  A RuntimeException in case any internal process indicates, that
                    the whole runtime cant be used any longer.
         */
        sal_Bool impl_loadContent()
            throw(LoadEnvException, css::uno::RuntimeException);

        //_______________________________________

        /** @short  checks if the specified content is already loaded.

            @descr  It depends from the set target information, if such
                    search is allowed or not! So this method checks first,
                    if the target is the special one "_default".
                    If not it returns with an empty result immidatly!
                    In case search is allowed, an existing document with the
                    same URL is searched. If it could be found, the corresponding
                    view will get the focus and this method return the corresponding frame.
                    Optional jumpmarks will be accepted here too. So the
                    view of the document will be updated to show the position
                    inside the document, which is related to the jumpmark.

            @return A valid reference to the target frame, which contains the already loaded content
                    and could be activated successfully. An empty reference oterwhise.

            @throw  A LoadEnvException only in cases, where an internal error indicates,
                    that the complete load environment seems to be not useable in general.
                    In such cases a RuntimeException would be to hard for the outside code :-)

            @throw  A RuntimeException in case any internal process indicates, that
                    the whole runtime cant be used any longer.
         */
        css::uno::Reference< css::frame::XFrame > impl_searchAlreadyLoaded()
            throw(LoadEnvException, css::uno::RuntimeException);

        //_______________________________________

        /** @short  search for any target frame, which seems to be useable
                    for this load request.

            @descr  Because this special feature is bound to the target specifier "_default"
                    its checked inside first. If its not set => this method return an empty
                    reference. Otherwhise any currently existing frame will be analyzed, if
                    it can be used here. The following rules exists:

                    <ul>
                        <li>The frame must be empty ...</li>
                        <li>or contains an empty document of the same application module
                            which the new document will have (Note: the filter of the new content
                            must be well known here!)</li>
                        <li>and(!) this target must not be already used by any other load request.</li>
                    </ul>

                    If a suitable target is located it will be locked. Thats why the last rule
                    exists! If this method returns a valid frame reference, it was locked to be useable
                    for this load request only. (Dont forget to reset this state later!)
                    Concurrent LoadEnv instances can synchronize her work be using such locks :-) HOPEFULLY

            @throw  A LoadEnvException only in cases, where an internal error indicates,
                    that the complete load environment seems to be not useable in general.
                    In such cases a RuntimeException would be to hard for the outside code :-)

            @throw  A RuntimeException in case any internal process indicates, that
                    the whole runtime cant be used any longer.
         */
        css::uno::Reference< css::frame::XFrame > impl_searchRecycleTarget()
            throw(LoadEnvException, css::uno::RuntimeException);

        //_______________________________________

        /** @short  it means; show the frame, bring it to front,
                    might set the right icon etcpp. in case loading was
                    successfully or reactivate a might existing old document or
                    close the frame if it was created before in case loading failed.

            @throw  A LoadEnvException only in cases, where an internal error indicates,
                    that the complete load environment seems to be not useable in general.
                    In such cases a RuntimeException would be to hard for the outside code :-)

            @throw  A RuntimeException in case any internal process indicates, that
                    the whole runtime cant be used any longer.
         */
        void impl_reactForLoadingState()
            throw(LoadEnvException, css::uno::RuntimeException);
};

} // namespace framework

#endif // __FRAMEWORK_LOADENV_LOADENV_HXX_
