/*************************************************************************
 *
 *  $RCSfile: autorecovery.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: kz $ $Date: 2005-03-01 19:38:10 $
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

#ifndef __FRAMEWORK_SERVICES_AUTORECOVERY_HXX_
#include "services/autorecovery.hxx"
#endif

//_______________________________________________
// own includes

#ifndef __FRAMEWORK_LOADENV_LOADDISPATCHLISTENER_HXX_
#include <loadenv/loaddispatchlistener.hxx>
#endif

#ifndef __FRAMEWORK_TARGETHELPER_HXX_
#include <loadenv/targethelper.hxx>
#endif

#ifndef __FRAMEWORK_PATTERN_FRAME_HXX_
#include <pattern/frame.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_READGUARD_HXX_
#include <threadhelp/readguard.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_WRITEGUARD_HXX_
#include <threadhelp/writeguard.hxx>
#endif

#ifndef __FRAMEWORK_PROTOCOLS_H_
#include <protocols.h>
#endif

#ifndef __FRAMEWORK_PROPERTIES_H_
#include <properties.h>
#endif

#ifndef __FRAMEWORK_SERVICES_H_
#include <services.h>
#endif

//_______________________________________________
// interface includes

#ifndef  _COM_SUN_STAR_UCB_NAMECLASH_HPP_
#include <com/sun/star/ucb/NameClash.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XMODULEMANAGER_HPP_
#include <com/sun/star/frame/XModuleManager.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_DISPATCHRESULTSTATE_HPP_
#include <com/sun/star/frame/DispatchResultState.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XNOTIFYINGDISPATCH_HPP_
#include <com/sun/star/frame/XNotifyingDispatch.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XCONTROLLER_HPP_
#include <com/sun/star/frame/XController.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XSTORABLE_HPP_
#include <com/sun/star/frame/XStorable.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_XMODIFIABLE_HPP_
#include <com/sun/star/util/XModifiable.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_XURLTRANSFORMER_HPP_
#include <com/sun/star/util/XURLTransformer.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XDESKTOP_HPP_
#include <com/sun/star/frame/XDesktop.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XHIERARCHICALNAMEACCESS_HPP_
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_XCHANGESNOTIFIER_HPP_
#include <com/sun/star/util/XChangesNotifier.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_XCHANGESBATCH_HPP_
#include <com/sun/star/util/XChangesBatch.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XCONTAINERQUERY_HPP_
#include <com/sun/star/container/XContainerQuery.hpp>
#endif

#ifndef _COM_SUN_STAR_DOCUMENT_XDOCUMENTINFOSUPPLIER_HPP_
#include <com/sun/star/document/XDocumentInfoSupplier.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_XCLOSEABLE_HPP_
#include <com/sun/star/util/XCloseable.hpp>
#endif

//_______________________________________________
// other includes

#ifndef _COMPHELPER_CONFIGURATIONHELPER_HXX_
#include <comphelper/configurationhelper.hxx>
#endif

#ifndef _COMPHELPER_MEDIADESCRIPTOR_HXX_
#include <comphelper/mediadescriptor.hxx>
#endif

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif

#ifndef _LINK_HXX
#include <tools/link.hxx>
#endif

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

#ifndef _UNOTOOLS_TEMPFILE_HXX
#include <unotools/tempfile.hxx>
#endif

#ifndef _UCBHELPER_CONTENT_HXX
#include <ucbhelper/content.hxx>
#endif

#include <osl/time.h>

#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif

//_______________________________________________
// namespaces

#ifndef css
namespace css = ::com::sun::star;
#endif

namespace fpf = ::framework::pattern::frame;

namespace framework
{

//-----------------------------------------------
// recovery.xcu
static const ::rtl::OUString CFG_PACKAGE_RECOVERY             = ::rtl::OUString::createFromAscii("org.openoffice.Office.Recovery/");
static const ::rtl::OUString CFG_ENTRY_RECOVERYLIST           = ::rtl::OUString::createFromAscii("RecoveryList"                   );
static const ::rtl::OUString CFG_PATH_RECOVERYINFO            = ::rtl::OUString::createFromAscii("RecoveryInfo"                   );
static const ::rtl::OUString CFG_ENTRY_CRASHED                = ::rtl::OUString::createFromAscii("Crashed"                        );
static const ::rtl::OUString CFG_ENTRY_EXISTSRECOVERYIDATA    = ::rtl::OUString::createFromAscii("ExistsRecoveryData"             );
static const ::rtl::OUString CFG_ENTRY_AUTOSAVE_ENABLED       = ::rtl::OUString::createFromAscii("AutoSave/Enabled"               );
static const ::rtl::OUString CFG_ENTRY_AUTOSAVE_TIMEINTERVALL = ::rtl::OUString::createFromAscii("AutoSave/TimeIntervall"         );

static const ::rtl::OUString CFG_ENTRY_PROP_TEMPURL           = ::rtl::OUString::createFromAscii("TempURL"      );
static const ::rtl::OUString CFG_ENTRY_PROP_ORIGINALURL       = ::rtl::OUString::createFromAscii("OriginalURL"  );
static const ::rtl::OUString CFG_ENTRY_PROP_TEMPLATEURL       = ::rtl::OUString::createFromAscii("TemplateURL"  );
static const ::rtl::OUString CFG_ENTRY_PROP_FACTORYURL        = ::rtl::OUString::createFromAscii("FactoryURL"   );
static const ::rtl::OUString CFG_ENTRY_PROP_MODULE            = ::rtl::OUString::createFromAscii("Module"       );
static const ::rtl::OUString CFG_ENTRY_PROP_DOCUMENTSTATE     = ::rtl::OUString::createFromAscii("DocumentState");
static const ::rtl::OUString CFG_ENTRY_PROP_FILTER            = ::rtl::OUString::createFromAscii("Filter"       );
static const ::rtl::OUString CFG_ENTRY_PROP_TITLE             = ::rtl::OUString::createFromAscii("Title"        );
static const ::rtl::OUString CFG_ENTRY_PROP_ID                = ::rtl::OUString::createFromAscii("ID"           );

static const ::rtl::OUString FILTER_PROP_TYPE                = ::rtl::OUString::createFromAscii("Type"            );
static const ::rtl::OUString FILTER_PROP_NAME                = ::rtl::OUString::createFromAscii("Name"            );
static const ::rtl::OUString TYPE_PROP_EXTENSIONS            = ::rtl::OUString::createFromAscii("Extensions"      );
static const ::rtl::OUString DOCINFO_PROP_TEMPLATE           = ::rtl::OUString::createFromAscii("TemplateFileName");

// setup.xcu
static const ::rtl::OUString CFG_ENTRY_PROP_FACTORYSHORTNAME = ::rtl::OUString::createFromAscii("ooSetupFactoryShortName"       );
static const ::rtl::OUString CFG_ENTRY_PROP_EMPTYDOCUMENTURL = ::rtl::OUString::createFromAscii("ooSetupFactoryEmptyDocumentURL");
static const ::rtl::OUString CFG_ENTRY_PROP_DEFAULTFILTER    = ::rtl::OUString::createFromAscii("ooSetupFactoryDefaultFilter"   );

static const ::rtl::OUString EVENT_ON_NEW                    = ::rtl::OUString::createFromAscii("OnNew"          );
static const ::rtl::OUString EVENT_ON_LOAD                   = ::rtl::OUString::createFromAscii("OnLoad"         );
static const ::rtl::OUString EVENT_ON_UNLOAD                 = ::rtl::OUString::createFromAscii("OnUnload"       );
static const ::rtl::OUString EVENT_ON_MODIFYCHANGED          = ::rtl::OUString::createFromAscii("OnModifyChanged");
static const ::rtl::OUString EVENT_ON_SAVEDONE               = ::rtl::OUString::createFromAscii("OnSaveDone"     );
static const ::rtl::OUString EVENT_ON_SAVEASDONE             = ::rtl::OUString::createFromAscii("OnSaveAsDone"   );

static const ::rtl::OUString RECOVERY_ITEM_BASE_IDENTIFIER   = ::rtl::OUString::createFromAscii("recovery_item_" );

static const ::rtl::OUString CMD_PROTOCOL                    = ::rtl::OUString::createFromAscii("vnd.sun.star.autorecovery:");

static const ::rtl::OUString CMD_DO_AUTO_SAVE                = ::rtl::OUString::createFromAscii("/doAutoSave"             );    // force AutoSave ignoring the AutoSave timer
static const ::rtl::OUString CMD_DO_PREPARE_EMERGENCY_SAVE   = ::rtl::OUString::createFromAscii("/doPrepareEmergencySave" );    // prepare the office for the following EmergencySave step (hide windows etcpp.)
static const ::rtl::OUString CMD_DO_EMERGENCY_SAVE           = ::rtl::OUString::createFromAscii("/doEmergencySave"        );    // do EmergencySave on crash
static const ::rtl::OUString CMD_DO_RECOVERY                 = ::rtl::OUString::createFromAscii("/doAutoRecovery"         );    // recover all crashed documents
static const ::rtl::OUString CMD_DO_ENTRY_BACKUP             = ::rtl::OUString::createFromAscii("/doEntryBackup"          );    // try to store a temp or original file to a user defined location
static const ::rtl::OUString CMD_DO_ENTRY_CLEANUP            = ::rtl::OUString::createFromAscii("/doEntryCleanUp"         );    // remove the specified entry from the recovery cache
static const ::rtl::OUString CMD_DO_SESSION_SAVE             = ::rtl::OUString::createFromAscii("/doSessionSave"          );    // save all open documents if e.g. a window manager closes an user session
static const ::rtl::OUString CMD_DO_SESSION_RESTORE          = ::rtl::OUString::createFromAscii("/doSessionRestore"       );    // restore a saved user session from disc

static const ::rtl::OUString REFERRER_USER                   = ::rtl::OUString::createFromAscii("private:user");

static const ::rtl::OUString PROP_DISPATCH_ASYNCHRON         = ::rtl::OUString::createFromAscii("DispatchAsynchron");
static const ::rtl::OUString PROP_PROGRESS                   = ::rtl::OUString::createFromAscii("StatusIndicator"  );
static const ::rtl::OUString PROP_SAVEPATH                   = ::rtl::OUString::createFromAscii("SavePath"         );
static const ::rtl::OUString PROP_ENTRY_ID                   = ::rtl::OUString::createFromAscii("EntryID"          );
static const ::rtl::OUString PROP_DBG_MAKE_IT_FASTER         = ::rtl::OUString::createFromAscii("DBGMakeItFaster"  );

static const ::rtl::OUString OPERATION_START                 = ::rtl::OUString::createFromAscii("start" );
static const ::rtl::OUString OPERATION_STOP                  = ::rtl::OUString::createFromAscii("stop"  );
static const ::rtl::OUString OPERATION_UPDATE                = ::rtl::OUString::createFromAscii("update");

#define MIN_TIME_FOR_USER_IDLE 10000 // 10s user idle

//-----------------------------------------------
// TODO debug - remove it!
class DbgListener : private ThreadHelpBase
                  , public  ::cppu::OWeakObject
                  , public  css::frame::XStatusListener
{
    public:

        DECLARE_XINTERFACE

        DbgListener()
        {
            WRITE_LOGFILE("autorecovery_states.txt", "\n\nDbgListener::ctor()\n\n")
        }

        virtual ~DbgListener()
        {
            WRITE_LOGFILE("autorecovery_states.txt", "\n\nDbgListener::dtor()\n\n")
        }

        void startListening(const css::uno::Reference< css::frame::XDispatch >& xBroadcaster)
        {
            ::rtl::OUStringBuffer sMsg1(256);
            sMsg1.appendAscii("//**********************************************************************************\n");
            sMsg1.appendAscii("start listening\n{\n");
            WRITE_LOGFILE("autorecovery_states.txt", U2B(sMsg1.makeStringAndClear()))

            ++m_refCount;

            css::util::URL aURL;
            aURL.Complete = ::rtl::OUString();
            xBroadcaster->addStatusListener(static_cast< css::frame::XStatusListener* >(this), aURL);

            --m_refCount;

            ::rtl::OUStringBuffer sMsg2(256);
            sMsg2.appendAscii("}\nstart listening\n");
            sMsg2.appendAscii("//**********************************************************************************\n");
            WRITE_LOGFILE("autorecovery_states.txt", U2B(sMsg2.makeStringAndClear()))
        }

        virtual void SAL_CALL disposing(const css::lang::EventObject& aEvent)
            throw(css::uno::RuntimeException)
        {
            WRITE_LOGFILE("autorecovery_states.txt", "\n\nDbgListener::dtor()\n\n")
        }

        virtual void SAL_CALL statusChanged(const css::frame::FeatureStateEvent& aEvent)
            throw(css::uno::RuntimeException)
        {
            ::rtl::OUStringBuffer sMsg(256);

            sMsg.appendAscii("//**********************************************************************************\n");

            sMsg.appendAscii("FeatureURL = \"");
            sMsg.append     (aEvent.FeatureURL.Complete);
            sMsg.appendAscii("\"\n");

            sMsg.appendAscii("State = [");
            sal_Int32 nState = -1;
            aEvent.State >>= nState;
            if (nState==-1)
            {
                sMsg.appendAscii("?-");
                sMsg.append     (::rtl::OUString::valueOf(nState));
                sMsg.appendAscii("-? ");
            }
            if (nState==0)
                sMsg.appendAscii("UNKNOWN ");
            if ((nState & 1)==1)
                sMsg.appendAscii("MODIFIED ");
            if ((nState & 2)==2)
                sMsg.appendAscii("TRYIT ");
            if ((nState & 4)==4)
                sMsg.appendAscii("HANDLED ");
            if ((nState & 8)==8)
                sMsg.appendAscii("POSTPONED ");
            if ((nState & 16)==16)
                sMsg.appendAscii("INCOMPLETE ");
            if ((nState & 32)==32)
                sMsg.appendAscii("DAMAGED ");
            sMsg.appendAscii("]\n");
/*
            sMsg.appendAscii("IsEnabled = \"");
            sMsg.append     (::rtl::OUString::valueOf(aEvent.IsEnabled));
            sMsg.appendAscii("\"\n");

            sMsg.appendAscii("Requery = \"");
            sMsg.append     (::rtl::OUString::valueOf(aEvent.Requery));
            sMsg.appendAscii("\"\n");
*/
            sMsg.appendAscii("\n");

            WRITE_LOGFILE("autorecovery_states.txt", U2B(sMsg.makeStringAndClear()))
        }
};

DEFINE_XINTERFACE_1(DbgListener                                 ,
                    OWeakObject                                 ,
                    DIRECT_INTERFACE(css::frame::XStatusListener))

//-----------------------------------------------
DEFINE_XINTERFACE_9(AutoRecovery                                                               ,
                    OWeakObject                                                                ,
                    DIRECT_INTERFACE (css::lang::XTypeProvider                                ),
                    DIRECT_INTERFACE (css::lang::XServiceInfo                                 ),
                    DIRECT_INTERFACE (css::frame::XDispatch                                   ),
                    DIRECT_INTERFACE (css::beans::XMultiPropertySet                           ),
                    DIRECT_INTERFACE (css::beans::XFastPropertySet                             ),
                    DIRECT_INTERFACE (css::beans::XPropertySet                                 ),
                    DIRECT_INTERFACE (css::document::XEventListener                           ),
                    DIRECT_INTERFACE (css::util::XChangesListener                             ),
                    DERIVED_INTERFACE(css::lang::XEventListener, css::document::XEventListener))

//-----------------------------------------------
DEFINE_XTYPEPROVIDER_6(AutoRecovery                 ,
                       css::lang::XTypeProvider     ,
                       css::lang::XServiceInfo      ,
                       css::frame::XDispatch        ,
                       css::beans::XMultiPropertySet,
                       css::beans::XFastPropertySet ,
                       css::beans::XPropertySet     )

//-----------------------------------------------
DEFINE_XSERVICEINFO_ONEINSTANCESERVICE(AutoRecovery                   ,
                                       ::cppu::OWeakObject            ,
                                       SERVICENAME_AUTORECOVERY       ,
                                       IMPLEMENTATIONNAME_AUTORECOVERY)

//-----------------------------------------------
DEFINE_INIT_SERVICE(
                    AutoRecovery,
                    {
                        /*Attention
                            I think we don't need any mutex or lock here ... because we are called by our own static method impl_createInstance()
                            to create a new instance of this class by our own supported service factory.
                            see macro DEFINE_XSERVICEINFO_MULTISERVICE and "impl_initService()" for further informations!
                        */

                        // read configuration to know if autosave/recovery is on/off etcpp...
                        implts_readConfig();

                        implts_startListening();

                        // establish callback for our internal used timer.
                        // Note: Its only active, if the timer will be started ...
                        m_aTimer.SetTimeoutHdl(LINK(this, AutoRecovery, implts_timerExpired));
/*
                        DbgListener* pListener = new DbgListener();
                        pListener->startListening(this);
*/
                    }
                   )

//-----------------------------------------------
AutoRecovery::AutoRecovery(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR)
    : ThreadHelpBase            (&Application::GetSolarMutex()                      )
    , ::cppu::OBroadcastHelper  ( m_aLock.getShareableOslMutex()                    )
    , ::cppu::OPropertySetHelper( *(static_cast< ::cppu::OBroadcastHelper* >(this)) )
    , ::cppu::OWeakObject       (                                                   )
    , m_xSMGR                   (xSMGR                                              )
    , m_nAutoSaveTimeIntervall  (0                                                  )
    , m_eTimerType              (E_DONT_START_TIMER                                 )
    , m_lListener               (m_aLock.getShareableOslMutex()                     )
    , m_nIdPool                 (0                                                  )
    , m_aAsyncDispatcher        ( LINK( this, AutoRecovery, implts_asyncDispatch )  )
    , m_eJob                    (AutoRecovery::E_NO_JOB                             )
    , m_bDocCacheLock           (sal_False                                          )
    , m_nWorkingEntryID         (-1                                                 )
    #if OSL_DEBUG_LEVEL > 1
    , m_dbg_bMakeItFaster       (sal_False                                          )
    #endif
{
}

//-----------------------------------------------
AutoRecovery::~AutoRecovery()
{
    implts_stopTimer();
    implts_stopListening();
}

//-----------------------------------------------
void SAL_CALL AutoRecovery::dispatch(const css::util::URL&                                  aURL      ,
                                     const css::uno::Sequence< css::beans::PropertyValue >& lArguments)
    throw(css::uno::RuntimeException)
{
    // valid request ?
    sal_Int32 eNewJob = AutoRecovery::implst_classifyJob(aURL);
    if (eNewJob == AutoRecovery::E_NO_JOB)
        return;

    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    // still running operation ... ignoring AUTO_SAVE.
    // All other requests has higher prio!
    if (
        ( m_eJob                               != AutoRecovery::E_NO_JOB   ) &&
        ((m_eJob & AutoRecovery::E_AUTO_SAVE ) != AutoRecovery::E_AUTO_SAVE)
       )
    {
        LOG_WARNING("AutoRecovery::dispatch()", "There is already an asynchronous dispatch() running. New request will be ignored!")
        return;
    }

    m_eJob = eNewJob;

    ::comphelper::SequenceAsHashMap lArgs(lArguments);
    sal_Bool  bAsynchron        = lArgs.getUnpackedValueOrDefault(PROP_DISPATCH_ASYNCHRON, (sal_Bool)sal_False                                 );
              m_nWorkingEntryID = lArgs.getUnpackedValueOrDefault(PROP_ENTRY_ID          , (sal_Int32)-1                                       );
              m_xProgress       = lArgs.getUnpackedValueOrDefault(PROP_PROGRESS          , css::uno::Reference< css::task::XStatusIndicator >());
              m_sSavePath       = lArgs.getUnpackedValueOrDefault(PROP_SAVEPATH          , ::rtl::OUString()                                   );

    // Hold this instance alive till the asynchronous operation will be finished.
    m_xSelfHold = css::uno::Reference< css::uno::XInterface >(static_cast< css::frame::XDispatch* >(this));
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;

    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    if (bAsynchron)
        m_aAsyncDispatcher.Post(0);
    else
        implts_dispatch();
}

//-----------------------------------------------
void AutoRecovery::implts_dispatch()
{
    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);
    sal_Int32                                   eJob        = m_eJob;
    css::uno::Reference< css::uno::XInterface > xMethodHold = m_xSelfHold;
    m_xSelfHold.clear();
    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    // in case a new dispatch overwrites a may ba active AutoSave session
    // we must restore this session later. see below ...
    sal_Bool bWasAutoSaveActive = ((eJob & AutoRecovery::E_AUTO_SAVE) == AutoRecovery::E_AUTO_SAVE);

    // On the other side it make no sense to reactivate the AutoSave operation
    // i fthe new dispatch indicates a final decision ...
    // E.g. an EmergencySave/SessionSave indicates the end of life of the current office session.
    // It make no sense to reactivate an AutoSave then.
    // But a Recovery or SessionRestore should reactivate a may be already active AutoSave.
    sal_Bool bAllowAutoSaveReactivation = sal_True;

    implts_stopTimer();
    implts_stopListening();

    implts_informListener(eJob,
        AutoRecovery::implst_createFeatureStateEvent(eJob, OPERATION_START, NULL));

    try
    {
        // if ((eJob & AutoRecovery::E_AUTO_SAVE) == AutoRecovery::E_AUTO_SAVE)
        //  Auto save is called from our internal timer ... not via dispatch() API !
        // else
        if ((eJob & AutoRecovery::E_PREPARE_EMERGENCY_SAVE) == AutoRecovery::E_PREPARE_EMERGENCY_SAVE)
        {
            bAllowAutoSaveReactivation = sal_False;
            implts_prepareEmergencySave();
        }
        else
        if ((eJob & AutoRecovery::E_EMERGENCY_SAVE) == AutoRecovery::E_EMERGENCY_SAVE)
        {
            bAllowAutoSaveReactivation = sal_False;
            implts_doEmergencySave();
        }
        else
        if ((eJob & AutoRecovery::E_RECOVERY) == AutoRecovery::E_RECOVERY)
            implts_doRecovery();
        else
        if ((eJob & AutoRecovery::E_SESSION_SAVE) == AutoRecovery::E_SESSION_SAVE)
        {
            bAllowAutoSaveReactivation = sal_False;
            implts_doSessionSave();
        }
        else
        if ((eJob & AutoRecovery::E_SESSION_RESTORE) == AutoRecovery::E_SESSION_RESTORE)
            implts_doSessionRestore();
        else
        if ((eJob & AutoRecovery::E_ENTRY_BACKUP) == AutoRecovery::E_ENTRY_BACKUP)
            implts_backupWorkingEntry();
        else
        if ((eJob & AutoRecovery::E_ENTRY_CLEANUP) == AutoRecovery::E_ENTRY_CLEANUP)
            implts_cleanUpWorkingEntry();
    }
    catch(const css::uno::RuntimeException& exRun)
        { throw exRun; }
    catch(const css::uno::Exception&)
        {} // TODO better error handling

    implts_informListener(eJob,
        AutoRecovery::implst_createFeatureStateEvent(eJob, OPERATION_STOP, NULL));

    // SAFE -> ----------------------------------
    aWriteLock.lock();
    m_eJob = E_NO_JOB;
    if (
        (bAllowAutoSaveReactivation) &&
        (bWasAutoSaveActive        )
       )
    {
        m_eJob |= AutoRecovery::E_AUTO_SAVE;
    }
    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    implts_actualizeTimer();
    implts_startListening();
}

//-----------------------------------------------
void SAL_CALL AutoRecovery::addStatusListener(const css::uno::Reference< css::frame::XStatusListener >& xListener,
                                              const css::util::URL&                                     aURL     )
    throw(css::uno::RuntimeException)
{
    if (!xListener.is())
        throw css::uno::RuntimeException(::rtl::OUString::createFromAscii("Invalid listener reference."), static_cast< css::frame::XDispatch* >(this));
    // container is threadsafe by using a shared mutex!
    m_lListener.addInterface(aURL.Complete, xListener);

    implts_lockDocCache();

    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);

    AutoRecovery::TDocumentList::iterator pIt;
    for(  pIt  = m_lDocCache.begin();
          pIt != m_lDocCache.end()  ;
        ++pIt                       )
    {
        AutoRecovery::TDocumentInfo&  rInfo = *pIt;
        css::frame::FeatureStateEvent aEvent = AutoRecovery::implst_createFeatureStateEvent(m_eJob, OPERATION_UPDATE, &rInfo);

        // <- SAFE ------------------------------
        aReadLock.unlock();
        xListener->statusChanged(aEvent);
        aReadLock.lock();
        // SAFE -> ------------------------------
    }

    aReadLock.unlock();
    // <- SAFE ----------------------------------

    implts_unlockDocCache();
}

//-----------------------------------------------
void SAL_CALL AutoRecovery::removeStatusListener(const css::uno::Reference< css::frame::XStatusListener >& xListener,
                                                 const css::util::URL&                                     aURL     )
    throw(css::uno::RuntimeException)
{
    if (!xListener.is())
        throw css::uno::RuntimeException(::rtl::OUString::createFromAscii("Invalid listener reference."), static_cast< css::frame::XDispatch* >(this));
    // container is threadsafe by using a shared mutex!
    m_lListener.removeInterface(aURL.Complete, xListener);
}

//-----------------------------------------------
void SAL_CALL AutoRecovery::notifyEvent(const css::document::EventObject& aEvent)
    throw(css::uno::RuntimeException)
{
    css::uno::Reference< css::frame::XModel > xDocument(aEvent.Source, css::uno::UNO_QUERY);

    // new document => put it into the internal list
    if (
        (aEvent.EventName.equals(EVENT_ON_NEW )) ||
        (aEvent.EventName.equals(EVENT_ON_LOAD))
       )
    {
        implts_registerDocument(xDocument);
        return;
    }
    // document modified => set its modify state new (means modified against the original file!)
    else
    if (aEvent.EventName.equals(EVENT_ON_MODIFYCHANGED))
    {
        implts_toggleModifiedState(xDocument);
        return;
    }
    // document saved => remove tmp. files - but hold config entries alive!
    else
    if (
        (aEvent.EventName.equals(EVENT_ON_SAVEDONE  )) ||
        (aEvent.EventName.equals(EVENT_ON_SAVEASDONE))
       )
    {
        implts_markDocumentAsSaved(xDocument);
        return;
    }
    // document closed => remove temp. files and configuration entries
    else
    if (aEvent.EventName.equals(EVENT_ON_UNLOAD))
    {
        implts_deregisterDocument(xDocument);
        return;
    }
}

//-----------------------------------------------
void SAL_CALL AutoRecovery::changesOccurred(const css::util::ChangesEvent& aEvent)
    throw(css::uno::RuntimeException)
{
    const css::uno::Sequence< css::util::ElementChange > lChanges (aEvent.Changes);
    const css::util::ElementChange*                      pChanges = lChanges.getConstArray();

    sal_Int32 c = lChanges.getLength();
    sal_Int32 i = 0;

    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    for (i=0; i<c; ++i)
    {
        ::rtl::OUString sPath;
        pChanges[i].Accessor >>= sPath;

        if (sPath.equals(CFG_ENTRY_AUTOSAVE_ENABLED))
        {
            sal_Bool bEnabled = sal_False;
            if (pChanges[i].Element >>= bEnabled)
            {
                if (bEnabled)
                {
                    m_eJob       |= AutoRecovery::E_AUTO_SAVE;
                    m_eTimerType  = AutoRecovery::E_NORMAL_AUTOSAVE_INTERVALL;
                }
                else
                {
                    m_eJob       &= ~AutoRecovery::E_AUTO_SAVE;
                    m_eTimerType  = AutoRecovery::E_DONT_START_TIMER;
                }
            }
        }
        else
        if (sPath.equals(CFG_ENTRY_AUTOSAVE_TIMEINTERVALL))
            pChanges[i].Element >>= m_nAutoSaveTimeIntervall;
    }

    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    // Note: This call stops the timer and starts it again.
    // But it checks the different timer states internaly and
    // may be supress the restart!
    implts_actualizeTimer();
}

//-----------------------------------------------
void SAL_CALL AutoRecovery::disposing(const css::lang::EventObject& aEvent)
    throw(css::uno::RuntimeException)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);

    if (aEvent.Source == m_xNewDocBroadcaster)
    {
        m_xNewDocBroadcaster.clear();
        return;
    }

    if (aEvent.Source == m_xCFG)
    {
        m_xCFG.clear();
        return;
    }
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
css::uno::Reference< css::container::XNameAccess > AutoRecovery::implts_openConfig()
{
    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    if (m_xCFG.is())
        return m_xCFG;
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;

    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    // throws a RuntimeException if an error occure!
    css::uno::Reference< css::container::XNameAccess > xCFG(
        ::comphelper::ConfigurationHelper::openConfig(xSMGR, CFG_PACKAGE_RECOVERY, ::comphelper::ConfigurationHelper::E_STANDARD),
        css::uno::UNO_QUERY);

    // SAFE -> ----------------------------------
    aWriteLock.lock();
    m_xCFG = xCFG;
    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    return xCFG;
}

//-----------------------------------------------
void AutoRecovery::implts_readConfig()
{
    css::uno::Reference< css::container::XHierarchicalNameAccess > xCommonRegistry(implts_openConfig(), css::uno::UNO_QUERY);

    implts_lockDocCache();
    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);
    // reset current cache load cache
    m_lDocCache.clear();
    m_nIdPool = 0;
    aWriteLock.unlock();
    // <- SAFE ----------------------------------
    implts_unlockDocCache();

    css::uno::Any aValue;

    // AutoSave [bool]
    aValue = xCommonRegistry->getByHierarchicalName(CFG_ENTRY_AUTOSAVE_ENABLED);
    sal_Bool bEnabled = sal_False;
    if (aValue >>= bEnabled)
    {
        // SAFE -> ------------------------------
        aWriteLock.lock();
        if (bEnabled)
        {
            m_eJob       |= AutoRecovery::E_AUTO_SAVE;
            m_eTimerType  = AutoRecovery::E_NORMAL_AUTOSAVE_INTERVALL;
        }
        else
        {
            m_eJob       &= ~AutoRecovery::E_AUTO_SAVE;
            m_eTimerType  = AutoRecovery::E_DONT_START_TIMER;
        }
        aWriteLock.unlock();
        // <- SAFE ------------------------------
    }

    // AutoSaveTimeIntervall [int] in min
    aValue = xCommonRegistry->getByHierarchicalName(CFG_ENTRY_AUTOSAVE_TIMEINTERVALL);
    // SAFE -> ----------------------------------
    aWriteLock.lock();
    aValue >>= m_nAutoSaveTimeIntervall;
    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    // RecoveryList [set]
    aValue = xCommonRegistry->getByHierarchicalName(CFG_ENTRY_RECOVERYLIST);
    css::uno::Reference< css::container::XNameAccess > xList;
    aValue >>= xList;
    if (xList.is())
    {
        const css::uno::Sequence< ::rtl::OUString > lItems = xList->getElementNames();
        const ::rtl::OUString*                      pItems = lItems.getConstArray();
              sal_Int32                             c      = lItems.getLength();
              sal_Int32                             i      = 0;

        for (i=0; i<c; ++i)
        {
            css::uno::Reference< css::beans::XPropertySet > xItem;
            xList->getByName(pItems[i]) >>= xItem;
            if (!xItem.is())
                continue;

            AutoRecovery::TDocumentInfo aInfo;
            aInfo.NewTempURL = ::rtl::OUString();
            aInfo.Document   = css::uno::Reference< css::frame::XModel >();
            xItem->getPropertyValue(CFG_ENTRY_PROP_ORIGINALURL  ) >>= aInfo.OrgURL       ;
            xItem->getPropertyValue(CFG_ENTRY_PROP_TEMPURL      ) >>= aInfo.OldTempURL   ;
            xItem->getPropertyValue(CFG_ENTRY_PROP_TEMPLATEURL  ) >>= aInfo.TemplateURL  ;
            xItem->getPropertyValue(CFG_ENTRY_PROP_FILTER       ) >>= aInfo.RealFilter   ;
            xItem->getPropertyValue(CFG_ENTRY_PROP_DOCUMENTSTATE) >>= aInfo.DocumentState;
            xItem->getPropertyValue(CFG_ENTRY_PROP_MODULE       ) >>= aInfo.AppModule    ;
            xItem->getPropertyValue(CFG_ENTRY_PROP_TITLE        ) >>= aInfo.Title        ;
            implts_specifyAppModuleAndFactoryURL(aInfo);
            implts_specifyDefaultFilterAndExtension(aInfo);

            if (pItems[i].indexOf(RECOVERY_ITEM_BASE_IDENTIFIER)==0)
            {
                ::rtl::OUString sID = pItems[i].copy(RECOVERY_ITEM_BASE_IDENTIFIER.getLength());
                aInfo.ID = sID.toInt32();
                // SAFE -> ----------------------
                aWriteLock.lock();
                if (aInfo.ID > m_nIdPool)
                {
                    m_nIdPool = aInfo.ID+1;
                    LOG_ASSERT(m_nIdPool>=0, "AutoRecovery::implts_readConfig()\nOverflow of IDPool detected!")
                }
                aWriteLock.unlock();
                // <- SAFE ----------------------
            }
            #ifdef ENABLE_WARNINGS
            else
                LOG_WARNING("AutoRecovery::implts_readConfig()", "Who changed numbering of recovery items? Cache will be inconsistent then! I do not know, what will happen next time .-)")
            #endif

            implts_lockDocCache();
            // SAFE -> --------------------------
            aWriteLock.lock();
            m_lDocCache.push_back(aInfo);
            aWriteLock.unlock();
            // <- SAFE --------------------------
            implts_unlockDocCache();
        }
    }

    implts_actualizeTimer();
}

//-----------------------------------------------
void AutoRecovery::implts_specifyDefaultFilterAndExtension(AutoRecovery::TDocumentInfo& rInfo)
{
    if (!rInfo.AppModule.getLength())
    {
        throw css::uno::RuntimeException(
                ::rtl::OUString::createFromAscii("Cant find out the default filter and its extension, if no application module is known!"),
                static_cast< css::frame::XDispatch* >(this));
    }

    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    css::uno::Reference< css::container::XContainerQuery > xFilterCFG(xSMGR->createInstance(SERVICENAME_FILTERFACTORY), css::uno::UNO_QUERY_THROW);
    css::uno::Reference< css::container::XNameAccess >     xTypeCFG  (xSMGR->createInstance(SERVICENAME_TYPEDETECTION), css::uno::UNO_QUERY_THROW);

    ::rtl::OUStringBuffer sQuery(256);
    sQuery.appendAscii("matchByDocumentService=");
    sQuery.append     (rInfo.AppModule          );
    sQuery.appendAscii(":iflags=294"            ); // import+export+own+default
    sQuery.appendAscii(":eflags=80"             ); // !alien+!templatepath (to suppress actual template filters!)

    css::uno::Reference< css::container::XEnumeration > xResult = xFilterCFG->createSubSetEnumerationByQuery(sQuery.makeStringAndClear());
    if (xResult->hasMoreElements())
    {
        ::comphelper::SequenceAsHashMap       lFilterProps        (xResult->nextElement());
                                              rInfo.DefaultFilter = lFilterProps.getUnpackedValueOrDefault(FILTER_PROP_NAME, ::rtl::OUString());
        ::rtl::OUString                       sTypeRegistration   = lFilterProps.getUnpackedValueOrDefault(FILTER_PROP_TYPE, ::rtl::OUString());
        ::comphelper::SequenceAsHashMap       lTypeProps          (xTypeCFG->getByName(sTypeRegistration));
        css::uno::Sequence< ::rtl::OUString > lExtensions         = lTypeProps.getUnpackedValueOrDefault(TYPE_PROP_EXTENSIONS, css::uno::Sequence< ::rtl::OUString >());
        if (lExtensions.getLength())
        {
            rInfo.Extension  = ::rtl::OUString::createFromAscii(".");
            rInfo.Extension += lExtensions[0];
        }
        else
            rInfo.Extension = ::rtl::OUString::createFromAscii(".unknown");
    }
}

//-----------------------------------------------
void AutoRecovery::implts_specifyAppModuleAndFactoryURL(AutoRecovery::TDocumentInfo& rInfo)
{
    if (
        (!rInfo.AppModule.getLength()) &&
        (!rInfo.Document.is()        )
       )
    {
        throw css::uno::RuntimeException(
                ::rtl::OUString::createFromAscii("Cant find out the application module nor its factory URL, if no application module (or a suitable) document is known!"),
                static_cast< css::frame::XDispatch* >(this));
    }

    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    css::uno::Reference< css::frame::XModuleManager > xManager     (xSMGR->createInstance(SERVICENAME_MODULEMANAGER), css::uno::UNO_QUERY_THROW);
    css::uno::Reference< css::container::XNameAccess > xModuleConfig(xManager                                        , css::uno::UNO_QUERY_THROW);

    if (!rInfo.AppModule.getLength())
        rInfo.AppModule = xManager->identify(rInfo.Document);

    ::comphelper::SequenceAsHashMap lModuleDescription(xModuleConfig->getByName(rInfo.AppModule));
    lModuleDescription[CFG_ENTRY_PROP_EMPTYDOCUMENTURL] >>= rInfo.FactoryURL;
}

//-----------------------------------------------
void AutoRecovery::implts_flushConfigItem(const AutoRecovery::TDocumentInfo& rInfo, sal_Bool bRemoveIt)
{
    css::uno::Reference< css::container::XHierarchicalNameAccess > xCFG  (implts_openConfig(), css::uno::UNO_QUERY);
    css::uno::Reference< css::container::XNameAccess >             xCheck;
    xCFG->getByHierarchicalName(CFG_ENTRY_RECOVERYLIST) >>= xCheck;

    css::uno::Reference< css::container::XNameContainer >   xModify(xCheck, css::uno::UNO_QUERY);
    css::uno::Reference< css::lang::XSingleServiceFactory > xCreate(xCheck, css::uno::UNO_QUERY);
    css::uno::Reference< css::util::XChangesBatch >         xFlush (xCFG  , css::uno::UNO_QUERY);

    ::rtl::OUStringBuffer sIDBuf;
    sIDBuf.append(RECOVERY_ITEM_BASE_IDENTIFIER);
    sIDBuf.append((sal_Int32)rInfo.ID);
    ::rtl::OUString sID = sIDBuf.makeStringAndClear();

    // remove
    if (bRemoveIt)
    {
        // Catch NoSuchElementException.
        // Its not a good idea inside multithreaded environments to call hasElement - removeElement.
        // DO IT!
        try
        {
            xModify->removeByName(sID);
        }
        catch(const css::container::NoSuchElementException&)
            { return; }
    }
    else
    {
        // new/modify
        css::uno::Reference< css::beans::XPropertySet > xSet;
        sal_Bool                                        bNew = (!xCheck->hasByName(sID));
        if (bNew)
            xSet = css::uno::Reference< css::beans::XPropertySet >(xCreate->createInstance(), css::uno::UNO_QUERY);
        else
            xCheck->getByName(sID) >>= xSet;

        xSet->setPropertyValue(CFG_ENTRY_PROP_ORIGINALURL  , css::uno::makeAny(rInfo.OrgURL       ));
        xSet->setPropertyValue(CFG_ENTRY_PROP_TEMPURL      , css::uno::makeAny(rInfo.OldTempURL   ));
        xSet->setPropertyValue(CFG_ENTRY_PROP_TEMPLATEURL  , css::uno::makeAny(rInfo.TemplateURL  ));
        xSet->setPropertyValue(CFG_ENTRY_PROP_FILTER       , css::uno::makeAny(rInfo.RealFilter   ));
        xSet->setPropertyValue(CFG_ENTRY_PROP_DOCUMENTSTATE, css::uno::makeAny(rInfo.DocumentState));
        xSet->setPropertyValue(CFG_ENTRY_PROP_MODULE       , css::uno::makeAny(rInfo.AppModule    ));
        xSet->setPropertyValue(CFG_ENTRY_PROP_TITLE        , css::uno::makeAny(rInfo.Title        ));

        if (bNew)
            xModify->insertByName(sID, css::uno::makeAny(xSet));
    }

    xFlush->commitChanges();
}

//-----------------------------------------------
void AutoRecovery::implts_startListening()
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory >  xSMGR        = m_xSMGR;
    css::uno::Reference< css::util::XChangesNotifier >      xCFG         (m_xCFG, css::uno::UNO_QUERY);
    css::uno::Reference< css::document::XEventBroadcaster > xBroadcaster = m_xNewDocBroadcaster;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    if (xCFG.is())
        xCFG->addChangesListener(static_cast< css::util::XChangesListener* >(this));

    if (!xBroadcaster.is())
    {
        xBroadcaster = css::uno::Reference< css::document::XEventBroadcaster >(xSMGR->createInstance(SERVICENAME_GLOBALEVENTBROADCASTER), css::uno::UNO_QUERY_THROW);
        // SAFE -> ----------------------------------
        WriteGuard aWriteLock(m_aLock);
        m_xNewDocBroadcaster = xBroadcaster;
        aWriteLock.unlock();
        // <- SAFE ----------------------------------
    }
    if (xBroadcaster.is())
        xBroadcaster->addEventListener(static_cast< css::document::XEventListener* >(this));
}

//-----------------------------------------------
void AutoRecovery::implts_stopListening()
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    // Attention: Dont reset our internal members here too.
    // May be we must work with our configuration, but dont wish to be informed
    // about changes any longer. Needed e.g. during EMERGENCY_SAVE!
    css::uno::Reference< css::util::XChangesNotifier >      xCFG                   (m_xCFG              , css::uno::UNO_QUERY);
    css::uno::Reference< css::document::XEventBroadcaster > xGlobalEventBroadcaster(m_xNewDocBroadcaster, css::uno::UNO_QUERY);
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    if (xGlobalEventBroadcaster.is())
        xGlobalEventBroadcaster->removeEventListener(static_cast< css::document::XEventListener* >(this));
    if (xCFG.is())
        xCFG->removeChangesListener(static_cast< css::util::XChangesListener* >(this));
}

//-----------------------------------------------
void AutoRecovery::implts_actualizeTimer()
{
    implts_stopTimer();

    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    if (
        (m_eJob       == AutoRecovery::E_NO_JOB          ) || // TODO may be superflous - E_DONT_START_TIMER should be used only
        (m_eTimerType == AutoRecovery::E_DONT_START_TIMER)
       )
        return;

    ULONG nMilliSeconds = 0;
    if (m_eTimerType == AutoRecovery::E_NORMAL_AUTOSAVE_INTERVALL)
    {
        nMilliSeconds = (m_nAutoSaveTimeIntervall*60000); // [min] => 60.000 ms
        #if OSL_DEBUG_LEVEL > 1
        if (m_dbg_bMakeItFaster)
            nMilliSeconds = m_nAutoSaveTimeIntervall;  // [ms]
        #endif
    }
    else
    if (m_eTimerType == AutoRecovery::E_POLL_FOR_USER_IDLE)
    {
        nMilliSeconds = MIN_TIME_FOR_USER_IDLE;
        #if OSL_DEBUG_LEVEL > 1
        if (m_dbg_bMakeItFaster)
            nMilliSeconds = 300; // let us some time, to finish this method .-)
        #endif
    }
    else
    if (m_eTimerType == AutoRecovery::E_POLL_TILL_AUTOSAVE_IS_ALLOWED)
        nMilliSeconds = 300; // there is a minimum time frame, where the user can loose some key input data!

    m_aTimer.SetTimeout(nMilliSeconds);
    m_aTimer.Start();

    aWriteLock.unlock();
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
void AutoRecovery::implts_stopTimer()
{
    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    if (!m_aTimer.IsActive())
        return;
    m_aTimer.Stop();

    // <- SAFE ----------------------------------
}

//-----------------------------------------------
IMPL_LINK(AutoRecovery, implts_timerExpired, void*, pVoid)
{
    // This method is called by using a pointer to us.
    // But we must be aware that we can be destroyed hardly
    // if our uno reference will be gone!
    // => Hold this object alive till this method finish its work.
    css::uno::Reference< css::uno::XInterface > xSelfHold(static_cast< css::lang::XTypeProvider* >(this));

    // Needed! Otherwise every reschedule request allow a new triggered timer event :-(
    implts_stopTimer();

    // check some "states", where its not allowed (better: not a good idea) to
    // start an AutoSave. (e.g. if the user makes drag & drop ...)
    // Then we poll till this "disallowed" state is gone.
    sal_Bool bAutoSaveNotAllowed = Application::IsUICaptured();
    if (bAutoSaveNotAllowed)
    {
        // SAFE -> ------------------------------
        WriteGuard aWriteLock(m_aLock);
        m_eTimerType = AutoRecovery::E_POLL_TILL_AUTOSAVE_IS_ALLOWED;
        aWriteLock.unlock();
        // <- SAFE ------------------------------
        implts_actualizeTimer();
        return 0;
    }

    // analyze timer type.
    // If we poll for an user idle period, may be we must
    // do nothing here and start the timer again.
    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    if (m_eTimerType == AutoRecovery::E_POLL_FOR_USER_IDLE)
    {
        sal_Bool bUserIdle = (Application::GetLastInputInterval()>MIN_TIME_FOR_USER_IDLE);
        if (!bUserIdle)
        {
            implts_actualizeTimer();
            return 0;
        }
    }

    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    // force save of all currently open documents
    // The called method returns an info, if and how this
    // timer must be restarted.
    sal_Bool bAllowUserIdleLoop = sal_True;
    AutoRecovery::ETimerType eSuggestedTimer = implts_saveDocs(bAllowUserIdleLoop);

    // If timer isnt used for "short callbacks" (means polling
    // for special states) ... reset the handle state of all
    // cache items. Such handle state indicates, that a document
    // was already saved during the THIS(!) AutoSave session.
    // Of course NEXT AutoSave session must be started without
    // any "handle" state ...
    if (
        (eSuggestedTimer == AutoRecovery::E_DONT_START_TIMER         ) ||
        (eSuggestedTimer == AutoRecovery::E_NORMAL_AUTOSAVE_INTERVALL)
       )
    {
        implts_resetHandleStates(sal_False);
    }

    // restart timer - because it was disabled before ...
    // SAFE -> ----------------------------------
    aWriteLock.lock();
    m_eTimerType = eSuggestedTimer;
    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    implts_actualizeTimer();

    return 0;
}

//-----------------------------------------------
IMPL_LINK(AutoRecovery, implts_asyncDispatch, void*, pVoid)
{
    implts_dispatch();
    return 0;
}

//-----------------------------------------------
void AutoRecovery::implts_registerDocument(const css::uno::Reference< css::frame::XModel >& xDocument)
{
    // ignore corrupted events, where no document is given ... Runtime Error ?!
    if (!xDocument.is())
        return;

    implts_lockDocCache();

    // notification for already existing document !
    // Can happen if events came in asynchronous on recovery time.
    // Then our cache was filled from the configuration ... but now we get some
    // asynchronous events from the global event broadcaster. We must be shure that
    // we dont add the same document more then once.
    AutoRecovery::TDocumentList::iterator pIt = AutoRecovery::impl_searchDocument(m_lDocCache, xDocument);
    if (pIt != m_lDocCache.end())
    {
        // Normaly nothing must be done for this "late" notification.
        // But may be the modified state was changed inbetween.
        // Check it ...
        implts_unlockDocCache();
        implts_toggleModifiedState(xDocument);
        return;
    }

    implts_unlockDocCache();

    // Check if doc is well known on the desktop. Otherwhise ignore it!
    // Other frames mostly are used from external programs - e.g. the bean ...
    css::uno::Reference< css::frame::XController > xController = xDocument->getCurrentController();
    if (!xController.is())
        return;

    css::uno::Reference< css::frame::XFrame >   xFrame   = xController->getFrame();
    css::uno::Reference< css::frame::XDesktop > xDesktop (xFrame->getCreator(), css::uno::UNO_QUERY);
    if (!xDesktop.is())
        return;

    // get all needed informations of this document
    // We need it to update our cache or to locate already existing elements there!
    AutoRecovery::TDocumentInfo aNew;
    aNew.Document = xDocument;

    // TODO replace getLocation() with getURL() ... its a workaround currently only!
    css::uno::Reference< css::frame::XStorable > xDoc(aNew.Document, css::uno::UNO_QUERY_THROW);
    aNew.OrgURL = xDoc->getLocation();

    css::uno::Reference< css::beans::XPropertySet > xFrameProps(xFrame, css::uno::UNO_QUERY_THROW);
    xFrameProps->getPropertyValue(FRAME_PROPNAME_TITLE) >>= aNew.Title;

    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // classify the used application module, which is used by this document.
    implts_specifyAppModuleAndFactoryURL(aNew);

    // Hack! Check for "illegal office documents" ... as e.g. the Basic IDE
    // Its not realy a full featured office document. It doesnt provide an URL, any filter, a factory URL etcpp.
    // TODO file bug to Basci IDE developers. They must remove the office document API from its service.
    if (
        (!aNew.OrgURL.getLength()    ) &&
        (!aNew.FactoryURL.getLength())
       )
       return;

    // By the way - get some information about the default format for saving!
    // and save an information about the real used filter by this document.
    // We save this document with DefaultFilter ... and load it with the RealFilter.
    implts_specifyDefaultFilterAndExtension(aNew);
    ::comphelper::MediaDescriptor lDescriptor(xDocument->getArgs());
    aNew.RealFilter = lDescriptor.getUnpackedValueOrDefault(::comphelper::MediaDescriptor::PROP_FILTERNAME()  , ::rtl::OUString());

    // Further we must know, if this document base on a template.
    // Then we must load it in a different way.
    css::uno::Reference< css::document::XDocumentInfoSupplier > xSupplier(aNew.Document, css::uno::UNO_QUERY);
    if (xSupplier.is()) // optional interface!
    {
        css::uno::Reference< css::beans::XPropertySet > xDocInfo(xSupplier->getDocumentInfo(), css::uno::UNO_QUERY_THROW);
        xDocInfo->getPropertyValue(DOCINFO_PROP_TEMPLATE) >>= aNew.TemplateURL;
    }

    css::uno::Reference< css::util::XModifiable > xModifyCheck(xDocument, css::uno::UNO_QUERY_THROW);
    if (xModifyCheck->isModified())
        aNew.DocumentState |= AutoRecovery::E_MODIFIED;

    implts_lockDocCache();
    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    // create a new cache entry ... this document isnt well known.
    ++m_nIdPool;
    aNew.ID = m_nIdPool;
    LOG_ASSERT(m_nIdPool>=0, "AutoRecovery::implts_registerDocument()\nOverflow of ID pool detected.")
    m_lDocCache.push_back(aNew);

    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    implts_flushConfigItem(aNew);
    implts_unlockDocCache();
}

//-----------------------------------------------
void AutoRecovery::implts_deregisterDocument(const css::uno::Reference< css::frame::XModel >& xDocument)
{
    implts_lockDocCache();

    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    // Attention: Dont leave SAFE section, if you work with pIt!
    // Because it points directly into the m_lDocCache list ...
    AutoRecovery::TDocumentList::iterator pIt = AutoRecovery::impl_searchDocument(m_lDocCache, xDocument);
    if (pIt == m_lDocCache.end())
    {
        implts_unlockDocCache();
        return; // unknown document => not a runtime error! Because we register only a few documents. see registration ...
    }

    AutoRecovery::TDocumentInfo aInfo = *pIt;
    m_lDocCache.erase(pIt);
    pIt = m_lDocCache.end(); // otherwhise its not specified what pIt means!

    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    implts_unlockDocCache();
    implts_removeTempFile(aInfo.OldTempURL);
    implts_removeTempFile(aInfo.NewTempURL);
    implts_flushConfigItem(aInfo, sal_True); // TRUE => remove it from config
}

//-----------------------------------------------
void AutoRecovery::implts_toggleModifiedState(const css::uno::Reference< css::frame::XModel >& xDocument)
{
    implts_lockDocCache();

    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    AutoRecovery::TDocumentList::iterator pIt = AutoRecovery::impl_searchDocument(m_lDocCache, xDocument);
    if (pIt != m_lDocCache.end())
    {
        AutoRecovery::TDocumentInfo& rInfo = *pIt;

        // use TRUE as fallback ... so we recognize every document on EmergencySave/AutoRecovery!
        sal_Bool bModified = sal_True;
        css::uno::Reference< css::util::XModifiable > xModify(xDocument, css::uno::UNO_QUERY);
        if (xModify.is())
            bModified = xModify->isModified();
        if (bModified)
            rInfo.DocumentState |= AutoRecovery::E_MODIFIED;
        else
            rInfo.DocumentState &= ~AutoRecovery::E_MODIFIED;
    }

    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    implts_unlockDocCache();
}

//-----------------------------------------------
void AutoRecovery::implts_markDocumentAsSaved(const css::uno::Reference< css::frame::XModel >& xDocument)
{
    implts_lockDocCache();

    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    AutoRecovery::TDocumentList::iterator pIt = AutoRecovery::impl_searchDocument(m_lDocCache, xDocument);
    if (pIt == m_lDocCache.end())
    {
        implts_unlockDocCache();
        return;
    }
    AutoRecovery::TDocumentInfo& rInfo = *pIt;

    rInfo.DocumentState = AutoRecovery::E_UNKNOWN;
    // TODO replace getLocation() with getURL() ... its a workaround currently only!
    css::uno::Reference< css::frame::XStorable > xDoc(rInfo.Document, css::uno::UNO_QUERY);
    rInfo.OrgURL = xDoc->getLocation();

    ::rtl::OUString sRemoveURL1 = rInfo.OldTempURL;
    ::rtl::OUString sRemoveURL2 = rInfo.NewTempURL;
    rInfo.OldTempURL = ::rtl::OUString();
    rInfo.NewTempURL = ::rtl::OUString();

    ::comphelper::MediaDescriptor lDescriptor(rInfo.Document->getArgs());
    rInfo.RealFilter = lDescriptor.getUnpackedValueOrDefault(::comphelper::MediaDescriptor::PROP_FILTERNAME(), ::rtl::OUString());
    rInfo.Title      = lDescriptor.getUnpackedValueOrDefault(::comphelper::MediaDescriptor::PROP_TITLE()     , ::rtl::OUString());
    if (!rInfo.Title.getLength())
        rInfo.Title  = lDescriptor.getUnpackedValueOrDefault(::comphelper::MediaDescriptor::PROP_DOCUMENTTITLE(), ::rtl::OUString());

    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    implts_unlockDocCache();
    implts_removeTempFile(sRemoveURL1);
    implts_removeTempFile(sRemoveURL2);
}

//-----------------------------------------------
AutoRecovery::TDocumentList::iterator AutoRecovery::impl_searchDocument(      AutoRecovery::TDocumentList&               rList    ,
                                                                        const css::uno::Reference< css::frame::XModel >& xDocument)
{
    AutoRecovery::TDocumentList::iterator pIt;
    for (  pIt  = rList.begin();
           pIt != rList.end()  ;
         ++pIt                 )
    {
        const AutoRecovery::TDocumentInfo& rInfo = *pIt;
        if (rInfo.Document == xDocument)
            break;
    }
    return pIt;
}

//-----------------------------------------------
void AutoRecovery::implts_hideAllDocs()
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    css::uno::Reference< css::frame::XFramesSupplier >  xDesktop  (xSMGR->createInstance(SERVICENAME_DESKTOP), css::uno::UNO_QUERY);
    css::uno::Reference< css::container::XIndexAccess > xContainer(xDesktop->getFrames()                     , css::uno::UNO_QUERY);
    sal_Int32 c = xContainer->getCount();
    sal_Int32 i = 0;

    for (i=0; i<c; ++i)
    {
        css::uno::Reference< css::frame::XFrame > xTask;

        xContainer->getByIndex(i) >>= xTask;
        if (!xTask.is())
            continue;

        css::uno::Reference< css::awt::XWindow > xWindow = xTask->getContainerWindow();
        xWindow->setVisible(sal_False);
    }

    aReadLock.unlock();
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
AutoRecovery::ETimerType AutoRecovery::implts_saveDocs(sal_Bool bAllowUserIdleLoop)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    css::uno::Reference< css::frame::XFramesSupplier > xDesktop      (xSMGR->createInstance(SERVICENAME_DESKTOP), css::uno::UNO_QUERY);
    ::rtl::OUString                                    sBackupPath   (SvtPathOptions().GetBackupPath());

    css::uno::Reference< css::frame::XController > xActiveController;
    css::uno::Reference< css::frame::XModel >      xActiveModel     ;
    css::uno::Reference< css::frame::XFrame >      xActiveFrame     = xDesktop->getActiveFrame();
    if (xActiveFrame.is())
        xActiveController = xActiveFrame->getController();
    if (xActiveController.is())
        xActiveModel = xActiveController->getModel();

    // Set the default timer action for our calli.
    // Default = NORMAL_AUTOSAVE
    // We return a suggestion for an active timer only.
    // It will be ignored if the timer was disabled by the user ...
    // Further this state can be set to USER_IDLE only later in this method.
    // Its not allowed to reset such state then. Because we must know, if
    // there exists POSTPONED documents. see below ...
    AutoRecovery::ETimerType eTimer = AutoRecovery::E_NORMAL_AUTOSAVE_INTERVALL;

    implts_lockDocCache();

    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    AutoRecovery::TDocumentList::iterator pIt;
    for (  pIt  = m_lDocCache.begin();
           pIt != m_lDocCache.end()  ;
         ++pIt                       )
    {
        AutoRecovery::TDocumentInfo aInfo = *pIt;

        // already auto saved during this session :-)
        // This state must be reseted for all documents
        // if timer is started with normnal AutoSaveTimerIntervall!
        if ((aInfo.DocumentState & AutoRecovery::E_HANDLED) == AutoRecovery::E_HANDLED)
            continue;

        // a) Document was not postponed - and is     active now. => postpone it (restart timer, restart loop)
        // b) Document was not postponed - and is not active now. => save it
        // c) Document was     postponed - and is not active now. => save it
        // d) Document was     postponed - and is     active now. => save it (because user idle was checked already)
        sal_Bool bActive        = (xActiveModel == aInfo.Document);
        sal_Bool bWasPostponed  = ((aInfo.DocumentState & AutoRecovery::E_POSTPONED) == AutoRecovery::E_POSTPONED);

        // a)
        if (!bWasPostponed && bActive)
        {
            aInfo.DocumentState |= AutoRecovery::E_POSTPONED;
            *pIt = aInfo;
            // postponed documents will be saved if this method is called again!
            // That can be done by an outside started timer           => E_POLL_FOR_USER_IDLE (if normal AutoSave is active)
            // or it must be done directly without starting any timer => E_CALL_ME_BACK       (if Emergency- or SessionSave is active and must be finished ASAP!)
            eTimer = AutoRecovery::E_POLL_FOR_USER_IDLE;
            if (!bAllowUserIdleLoop)
                eTimer = AutoRecovery::E_CALL_ME_BACK;
            continue;
        }

        // Not modified documents are not saved.
        // We safe an information about the URL only!
        sal_Bool bSave = ((aInfo.DocumentState & AutoRecovery::E_MODIFIED) == AutoRecovery::E_MODIFIED);

        // TODO HACK impress documents (created by a wizard) cant be restored by using its URL :-( ... save it realy
        if (aInfo.FactoryURL.matchIgnoreAsciiCaseAsciiL("private:factory/simpress", 24))
        {
            if (
                (!aInfo.OrgURL.getLength()     ) &&
                (!aInfo.TemplateURL.getLength())
               )
            {
                bSave = sal_True;
            }
        }

        sal_Int32 eJob = m_eJob;

        // b, c, d)
        // <- SAFE --------------------------
        aWriteLock.unlock();
        if (bSave)
            implts_saveOneDoc(sBackupPath, aInfo);
        else
        {
            aInfo.DocumentState |= AutoRecovery::E_HANDLED ;
            aInfo.DocumentState |= AutoRecovery::E_SUCCEDED;
            implts_flushConfigItem(aInfo);
        }
        implts_informListener(eJob,
            AutoRecovery::implst_createFeatureStateEvent(eJob, OPERATION_UPDATE, &aInfo));
        aWriteLock.lock();
        // SAFE -> --------------------------

        *pIt = aInfo;
    }

    implts_unlockDocCache();

    return eTimer;
}

//-----------------------------------------------
void AutoRecovery::implts_saveOneDoc(const ::rtl::OUString&             sBackupPath,
                                           AutoRecovery::TDocumentInfo& rInfo      )
{
    // no document? => can occure if we loaded our configuration with files,
    // which couldnt be recovered successfully. In such case we have all needed informations
    // excepting the real document instance!

    // TODO: search right place, where such "dead files" can be removed from the configuration!
    if (!rInfo.Document.is())
        return;

    ::comphelper::MediaDescriptor lOldArgs(rInfo.Document->getArgs());
    implts_generateNewTempURL(sBackupPath, lOldArgs, rInfo);

    // if the document was loaded with a password, it should be
    // stored with password
    ::comphelper::MediaDescriptor lNewArgs;
    ::rtl::OUString sPassword = lOldArgs.getUnpackedValueOrDefault(::comphelper::MediaDescriptor::PROP_PASSWORD(), ::rtl::OUString());
    if (sPassword.getLength())
        lNewArgs[::comphelper::MediaDescriptor::PROP_PASSWORD()] <<= sPassword;

    // Further it must be saved using the default file format of that application.
    // Otherwhise we will some data lost.
    if (rInfo.DefaultFilter.getLength())
        lNewArgs[::comphelper::MediaDescriptor::PROP_FILTERNAME()] <<= rInfo.DefaultFilter;

    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::task::XStatusIndicator > xProgress(m_xProgress.get(), css::uno::UNO_QUERY);
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    if (xProgress.is())
    {
        // use external progress object
        lNewArgs[::comphelper::MediaDescriptor::PROP_STATUSINDICATOR()] <<= xProgress;
        // HACK
        // Use special property of the frame, to intercept status indicator events.
        // Wont work with different views for the same document :-(
        css::uno::Reference< css::frame::XController > xController = rInfo.Document->getCurrentController();
        if (xController.is())
        {
            css::uno::Reference< css::beans::XPropertySet > xFrameProps(xController->getFrame(), css::uno::UNO_QUERY_THROW);
            xFrameProps->setPropertyValue(FRAME_PROPNAME_INDICATORINTERCEPTION, css::uno::makeAny(xProgress));
        }
    }

    // try to save this document as a new temp file everytimes.
    // Mark AutoSave state as "INCOMPLETE" if it failed.
    // Because the last temp file is to old and does not include all changes.
    try
    {
        // safe the state about "trying to save"
        // ... we need it for recovery if e.g. a crash occures inside next line!
        rInfo.DocumentState |= AutoRecovery::E_TRY_SAVE;
        implts_flushConfigItem(rInfo);

        css::uno::Reference< css::frame::XStorable > xStore(rInfo.Document, css::uno::UNO_QUERY);
        xStore->storeToURL(rInfo.NewTempURL, lNewArgs.getAsConstPropertyValueList());

        // safe the state about success
        // ... you know the reason: to know it on recovery time if next line crash .-)
        rInfo.DocumentState &= ~AutoRecovery::E_TRY_SAVE;
        rInfo.DocumentState |=  AutoRecovery::E_HANDLED;
        rInfo.DocumentState |=  AutoRecovery::E_SUCCEDED;
        implts_flushConfigItem(rInfo);
    }
/*
    catch(const css::uno::RuntimeException&)
        { throw; }
*/
    catch(const css::uno::Exception&)
        {
            // safe the state about error ...
            rInfo.NewTempURL     = ::rtl::OUString();
            rInfo.DocumentState &= ~AutoRecovery::E_TRY_SAVE;
            rInfo.DocumentState |=  AutoRecovery::E_HANDLED;
            rInfo.DocumentState |=  AutoRecovery::E_INCOMPLETE;
            implts_flushConfigItem(rInfo);
            return;
        }

    // try to remove the old temp file.
    // Ignore any error here. We have a new temp file, which is up to date.
    // The only thing is: we fill the disk with temp files, if we cant remove old ones :-)
    ::rtl::OUString sRemoveFile      = rInfo.OldTempURL;
                    rInfo.OldTempURL = rInfo.NewTempURL;
                    rInfo.NewTempURL = ::rtl::OUString();
    implts_flushConfigItem(rInfo);
    implts_removeTempFile(sRemoveFile);
}

//-----------------------------------------------
AutoRecovery::ETimerType AutoRecovery::implts_openDocs()
{
    AutoRecovery::ETimerType eTimer = AutoRecovery::E_DONT_START_TIMER;

    implts_lockDocCache();

    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    sal_Int32                             eJob = m_eJob;
    AutoRecovery::TDocumentList::iterator pIt;
    for (  pIt  = m_lDocCache.begin();
           pIt != m_lDocCache.end()  ;
         ++pIt                       )
    {
        AutoRecovery::TDocumentInfo& rInfo = *pIt;

        // Such documents are already loaded by the last loop.
        // Dont check E_SUCCEDED here! Its may be the final state of an AutoSave
        // operation before!!!
        if ((rInfo.DocumentState & AutoRecovery::E_HANDLED) == AutoRecovery::E_HANDLED)
            continue;

        // a1,b1,c1,d2,e2,f2)
        if ((rInfo.DocumentState & AutoRecovery::E_DAMAGED) == AutoRecovery::E_DAMAGED)
        {
            // dont forget to inform listener! May be this document was
            // damaged on last saving time ...
            // Then our listener need this notification.
            // If it was damaged during last "try to open" ...
            // it will be notified more then once. SH.. HAPPENS ...
            // <- SAFE --------------------------
            aWriteLock.unlock();
            implts_informListener(eJob,
                AutoRecovery::implst_createFeatureStateEvent(eJob, OPERATION_UPDATE, &rInfo));
            aWriteLock.lock();
            // SAFE -> --------------------------
            continue;
        }

        ::comphelper::MediaDescriptor lDescriptor;

        // its an UI feature - so the "USER" itself must be set as referer
        lDescriptor[::comphelper::MediaDescriptor::PROP_REFERRER()] <<= REFERRER_USER;
        lDescriptor[::comphelper::MediaDescriptor::PROP_SALVAGEDFILE()] <<= ::rtl::OUString();

        // use external progress object
        css::uno::Reference< css::task::XStatusIndicator > xProgress(m_xProgress.get(), css::uno::UNO_QUERY);
        if (xProgress.is())
            lDescriptor[::comphelper::MediaDescriptor::PROP_STATUSINDICATOR()] <<= xProgress;

        sal_Bool bBackupWasTried   = (
                                        ((rInfo.DocumentState & AutoRecovery::E_TRY_LOAD_BACKUP  ) == AutoRecovery::E_TRY_LOAD_BACKUP) || // temp. state!
                                        ((rInfo.DocumentState & AutoRecovery::E_INCOMPLETE       ) == AutoRecovery::E_INCOMPLETE     )    // transport TRY_LOAD_BACKUP from last loop to this new one!
                                     );
        sal_Bool bOriginalWasTried = ((rInfo.DocumentState & AutoRecovery::E_TRY_LOAD_ORIGINAL) == AutoRecovery::E_TRY_LOAD_ORIGINAL);

        if (bBackupWasTried)
        {
            if (!bOriginalWasTried)
            {
                rInfo.DocumentState |= AutoRecovery::E_INCOMPLETE;
                // try original URL ... ! dont continue with next item here ...
            }
            else
            {
                rInfo.DocumentState |= AutoRecovery::E_DAMAGED;
                continue;
            }
        }

        ::rtl::OUString sLoadOriginalURL;
        ::rtl::OUString sLoadBackupURL  ;

        if (!bBackupWasTried)
            sLoadBackupURL = rInfo.OldTempURL;

        if (rInfo.OrgURL.getLength())
        {
            sLoadOriginalURL = rInfo.OrgURL;
        }
        else
        if (rInfo.TemplateURL.getLength())
        {
            sLoadOriginalURL = rInfo.TemplateURL;
            lDescriptor[::comphelper::MediaDescriptor::PROP_ASTEMPLATE()]   <<= sal_True;
            lDescriptor[::comphelper::MediaDescriptor::PROP_TEMPLATENAME()] <<= rInfo.TemplateURL;
        }
        else
        if (rInfo.FactoryURL.getLength())
        {
            sLoadOriginalURL = rInfo.FactoryURL;
            lDescriptor[::comphelper::MediaDescriptor::PROP_ASTEMPLATE()] <<= sal_True;
        }

        // A "Salvaged" item must exists every time. The core can make something special then for recovery.
        // Of course it should be the real file name of the original file, in case we load the temp. backup here.
        ::rtl::OUString sURL;
        if (sLoadBackupURL.getLength())
        {
            sURL = sLoadBackupURL;
            rInfo.DocumentState |= AutoRecovery::E_TRY_LOAD_BACKUP;
            lDescriptor[::comphelper::MediaDescriptor::PROP_SALVAGEDFILE()] <<= sLoadOriginalURL;
        }
        else
        if (sLoadOriginalURL.getLength())
        {
            sURL = sLoadOriginalURL;
            rInfo.DocumentState |= AutoRecovery::E_TRY_LOAD_ORIGINAL;
        }
        else
            continue; // TODO ERROR!

        // <- SAFE ------------------------------
        aWriteLock.unlock();

        implts_flushConfigItem(rInfo);
        implts_informListener(eJob,
            AutoRecovery::implst_createFeatureStateEvent(eJob, OPERATION_UPDATE, &rInfo));

        try
        {
            implts_openOneDoc(sURL, lDescriptor, rInfo);
        }
        catch(const css::uno::Exception&)
        {
            rInfo.DocumentState &= ~AutoRecovery::E_TRY_LOAD_BACKUP;
            rInfo.DocumentState &= ~AutoRecovery::E_TRY_LOAD_ORIGINAL;
            if (sLoadBackupURL.getLength())
            {
                rInfo.DocumentState |= AutoRecovery::E_INCOMPLETE;
                eTimer               = AutoRecovery::E_CALL_ME_BACK;
            }
            else
            {
                rInfo.DocumentState |=  AutoRecovery::E_HANDLED;
                rInfo.DocumentState |=  AutoRecovery::E_DAMAGED;
            }

            implts_flushConfigItem(rInfo, sal_True);
            implts_informListener(eJob,
                AutoRecovery::implst_createFeatureStateEvent(eJob, OPERATION_UPDATE, &rInfo));

            // SAFE -> ------------------------------
            // Needed for next loop!
            aWriteLock.lock();
            continue;
        }

        if (rInfo.RealFilter.getLength())
        {
            ::comphelper::MediaDescriptor lPatchDescriptor(rInfo.Document->getArgs());
            lPatchDescriptor[::comphelper::MediaDescriptor::PROP_FILTERNAME()] <<= rInfo.RealFilter;
            rInfo.Document->attachResource(sURL, lPatchDescriptor.getAsConstPropertyValueList());
        }

        css::uno::Reference< css::util::XModifiable > xModify(rInfo.Document, css::uno::UNO_QUERY);
        sal_Bool bModified = ((rInfo.DocumentState & AutoRecovery::E_MODIFIED) == AutoRecovery::E_MODIFIED);
        xModify->setModified(bModified);

        rInfo.DocumentState &= ~AutoRecovery::E_TRY_LOAD_BACKUP;
        rInfo.DocumentState &= ~AutoRecovery::E_TRY_LOAD_ORIGINAL;
        rInfo.DocumentState |=  AutoRecovery::E_HANDLED;
        rInfo.DocumentState |=  AutoRecovery::E_SUCCEDED;

        implts_flushConfigItem(rInfo);
        implts_informListener(eJob,
            AutoRecovery::implst_createFeatureStateEvent(eJob, OPERATION_UPDATE, &rInfo));

        // SAFE -> ------------------------------
        // Needed for next loop. Dont unlock it again!
        aWriteLock.lock();
    }

    aWriteLock.unlock();
    // <- SAFE ----------------------------------
    implts_unlockDocCache();

    return eTimer;
}

//-----------------------------------------------
void AutoRecovery::implts_openOneDoc(const ::rtl::OUString&               sURL       ,
                                           ::comphelper::MediaDescriptor& lDescriptor,
                                           AutoRecovery::TDocumentInfo&   rInfo      )
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR     = m_xSMGR    ;
    css::uno::Reference< css::task::XStatusIndicator >     xProgress( m_xProgress.get(), css::uno::UNO_QUERY);
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    css::uno::Reference< css::util::XURLTransformer > xParser(xSMGR->createInstance(SERVICENAME_URLTRANSFORMER), css::uno::UNO_QUERY_THROW);
    css::util::URL aURL;
    aURL.Complete = sURL;
    xParser->parseStrict(aURL);

    LoadDispatchListener*                                      pLoadListener = new LoadDispatchListener();
    css::uno::Reference< css::frame::XDispatchResultListener > xLoadListener (static_cast< css::frame::XDispatchResultListener* >(pLoadListener), css::uno::UNO_QUERY_THROW);

    css::uno::Reference< css::frame::XFrame >             xDesktop   (xSMGR->createInstance(SERVICENAME_DESKTOP), css::uno::UNO_QUERY_THROW);
    css::uno::Reference< css::frame::XFrame >             xNewTarget = xDesktop->findFrame(SPECIALTARGET_BLANK, 0);
    css::uno::Reference< css::frame::XDispatchProvider >  xProvider  (xNewTarget, css::uno::UNO_QUERY_THROW);
    css::uno::Reference< css::frame::XNotifyingDispatch > xDispatcher(
        xProvider->queryDispatch(aURL, SPECIALTARGET_SELF, 0),
        css::uno::UNO_QUERY_THROW);

    // HACK
    // Use special property of the frame, to intercept status indicator events.
    // Wont work with different views for the same document :-(
    css::uno::Reference< css::beans::XPropertySet > xFrameProps(xNewTarget, css::uno::UNO_QUERY_THROW);
    xFrameProps->setPropertyValue(FRAME_PROPNAME_INDICATORINTERCEPTION, css::uno::makeAny(xProgress));

    // load the document and listen for the state of this operation.
    pLoadListener->setURL(aURL.Complete);

    try
    {
        xDispatcher->dispatchWithNotification(
            aURL,
            lDescriptor.getAsConstPropertyValueList(),
            xLoadListener);

        pLoadListener->wait(0); // wait for ever!
        xFrameProps->setPropertyValue(FRAME_PROPNAME_INDICATORINTERCEPTION, css::uno::makeAny(css::uno::Reference< css::task::XStatusIndicator >()));

        css::frame::DispatchResultEvent aResult = pLoadListener->getResult();
        if (aResult.State != css::frame::DispatchResultState::SUCCESS)
        {
            ::rtl::OUStringBuffer sMsg(256);
            sMsg.appendAscii("Recovery of \"");
            sMsg.append     (aURL.Complete   );
            sMsg.appendAscii("\" failed."    );
            throw css::uno::Exception(sMsg.makeStringAndClear(), static_cast< css::frame::XDispatch* >(this));
        }

        rInfo.Document = fpf::extractFrameModel(xNewTarget);
    }
    catch(const css::uno::RuntimeException&)
        { throw; }
    catch(const css::uno::Exception&)
    {
        css::uno::Reference< css::util::XCloseable > xClose(xNewTarget, css::uno::UNO_QUERY);
        xClose->close(sal_True);
        throw;
    }
}

//-----------------------------------------------
void AutoRecovery::implts_generateNewTempURL(const ::rtl::OUString&               sBackupPath     ,
                                                   ::comphelper::MediaDescriptor& rMediaDescriptor,
                                                   AutoRecovery::TDocumentInfo&   rInfo           )
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // specify URL for saving (which points to a temp file inside backup directory)
    // and define an unique name, so we can locate it later.
    // This unique name must solve an optimization problem too!
    // In case we are asked to save unmodified documents too - and one of them
    // is an empty one (because it was new created using e.g. an URL private:factory/...)
    // we should not save it realy. Then we put the information about such "empty document"
    // into the configuration and dont create any recovery file on disk.
    // We use the title of the document to make it unique.
    ::rtl::OUStringBuffer sUniqueName;
    if (rInfo.OrgURL.getLength())
    {
        css::uno::Reference< css::util::XURLTransformer > xParser(xSMGR->createInstance(SERVICENAME_URLTRANSFORMER), css::uno::UNO_QUERY);
        css::util::URL aURL;
        aURL.Complete = rInfo.OrgURL;
        xParser->parseStrict(aURL);
        sUniqueName.append(aURL.Name);
    }
    else
    if (rInfo.FactoryURL.getLength())
        sUniqueName.appendAscii("untitled");
    sUniqueName.appendAscii("_");

    // TODO: Must we strip some illegal signes - if we use the title?

    String sName     (sUniqueName.makeStringAndClear());
    String sExtension(rInfo.Extension                 );
    String sPath     (sBackupPath                     );
    ::utl::TempFile aTempFile(sName, &sExtension, &sPath);
    rInfo.NewTempURL = aTempFile.GetURL();
}

//-----------------------------------------------
void AutoRecovery::implts_removeTempFile(const ::rtl::OUString& sURL)
{
    if (!sURL.getLength())
        return;

    try
    {
        ::ucb::Content aContent = ::ucb::Content(sURL, css::uno::Reference< css::ucb::XCommandEnvironment >());
        aContent.executeCommand(::rtl::OUString::createFromAscii("delete"), css::uno::makeAny(sal_True));
    }
    catch(const css::uno::RuntimeException&)
        { throw; }
    catch(const css::uno::Exception&)
        {}
}

//-----------------------------------------------
void AutoRecovery::implts_informListener(      sal_Int32                      eJob  ,
                                         const css::frame::FeatureStateEvent& aEvent)
{
    // Helper shares mutex with us -> threadsafe!
    ::cppu::OInterfaceContainerHelper* pListenerForURL = 0;
    ::rtl::OUString                    sJob            = AutoRecovery::implst_getJobDescription(eJob);

    // inform listener, which are registered for any URLs(!)
    pListenerForURL = m_lListener.getContainer(sJob);
    if(pListenerForURL != 0)
    {
        ::cppu::OInterfaceIteratorHelper pIt(*pListenerForURL);
        while(pIt.hasMoreElements())
        {
            try
            {
                css::uno::Reference< css::frame::XStatusListener > xListener(((css::frame::XStatusListener*)pIt.next()), css::uno::UNO_QUERY);
                xListener->statusChanged(aEvent);
            }
            catch(const css::uno::RuntimeException&)
                { pIt.remove(); }
        }
    }
}

//-----------------------------------------------
::rtl::OUString AutoRecovery::implst_getJobDescription(sal_Int32 eJob)
{
    // describe the current running operation
    ::rtl::OUStringBuffer sFeature(256);
    sFeature.append(CMD_PROTOCOL);

    if ((eJob & AutoRecovery::E_AUTO_SAVE) == AutoRecovery::E_AUTO_SAVE)
        sFeature.append(CMD_DO_AUTO_SAVE);
    else
    if ((eJob & AutoRecovery::E_PREPARE_EMERGENCY_SAVE) == AutoRecovery::E_PREPARE_EMERGENCY_SAVE)
        sFeature.append(CMD_DO_PREPARE_EMERGENCY_SAVE);
    else
    if ((eJob & AutoRecovery::E_EMERGENCY_SAVE) == AutoRecovery::E_EMERGENCY_SAVE)
        sFeature.append(CMD_DO_EMERGENCY_SAVE);
    else
    if ((eJob & AutoRecovery::E_RECOVERY) == AutoRecovery::E_RECOVERY)
        sFeature.append(CMD_DO_RECOVERY);
    else
    if ((eJob & AutoRecovery::E_ENTRY_BACKUP) == AutoRecovery::E_ENTRY_BACKUP)
        sFeature.append(CMD_DO_ENTRY_BACKUP);
    else
    if ((eJob & AutoRecovery::E_ENTRY_CLEANUP) == AutoRecovery::E_ENTRY_CLEANUP)
        sFeature.append(CMD_DO_ENTRY_CLEANUP);
    else
    if ((eJob & AutoRecovery::E_SESSION_SAVE) == AutoRecovery::E_SESSION_SAVE)
        sFeature.append(CMD_DO_SESSION_SAVE);
    else
    if ((eJob & AutoRecovery::E_SESSION_RESTORE) == AutoRecovery::E_SESSION_RESTORE)
        sFeature.append(CMD_DO_SESSION_RESTORE);
    #ifdef ENABLE_WARNINGS
    else
        LOG_WARNING("AutoRecovery::implst_getJobDescription()", "Invalid job identifier detected.")
    #endif

    return sFeature.makeStringAndClear();
}

//-----------------------------------------------
sal_Int32 AutoRecovery::implst_classifyJob(const css::util::URL& aURL)
{
    if (aURL.Protocol.equals(CMD_PROTOCOL))
    {
        if (aURL.Path.equals(CMD_DO_PREPARE_EMERGENCY_SAVE))
            return AutoRecovery::E_PREPARE_EMERGENCY_SAVE;
        else
        if (aURL.Path.equals(CMD_DO_EMERGENCY_SAVE))
            return AutoRecovery::E_EMERGENCY_SAVE;
        else
        if (aURL.Path.equals(CMD_DO_RECOVERY))
            return AutoRecovery::E_RECOVERY;
        else
        if (aURL.Path.equals(CMD_DO_ENTRY_BACKUP))
            return AutoRecovery::E_ENTRY_BACKUP;
        else
        if (aURL.Path.equals(CMD_DO_ENTRY_CLEANUP))
            return AutoRecovery::E_ENTRY_CLEANUP;
        else
        if (aURL.Path.equals(CMD_DO_SESSION_SAVE))
            return AutoRecovery::E_SESSION_SAVE;
        else
        if (aURL.Path.equals(CMD_DO_SESSION_RESTORE))
            return AutoRecovery::E_SESSION_RESTORE;
    }

    LOG_WARNING("AutoRecovery::implts_classifyJob()", "Invalid URL (protocol).")
    return AutoRecovery::E_NO_JOB;
}

//-----------------------------------------------
css::frame::FeatureStateEvent AutoRecovery::implst_createFeatureStateEvent(      sal_Int32                    eJob      ,
                                                                           const ::rtl::OUString&             sEventType,
                                                                                 AutoRecovery::TDocumentInfo* pInfo     )
{
    css::frame::FeatureStateEvent aEvent;
    aEvent.FeatureURL.Complete   = AutoRecovery::implst_getJobDescription(eJob);
    aEvent.FeatureDescriptor     = sEventType;

    if (sEventType.equals(OPERATION_UPDATE) && pInfo)
    {
        // pack rInfo for transport via UNO
        css::uno::Sequence< css::beans::NamedValue > lInfo(8);
        lInfo[0].Name    = CFG_ENTRY_PROP_ID;
        lInfo[0].Value <<= pInfo->ID;

        lInfo[1].Name    = CFG_ENTRY_PROP_ORIGINALURL;
        lInfo[1].Value <<= pInfo->OrgURL;

        lInfo[2].Name    = CFG_ENTRY_PROP_FACTORYURL;
        lInfo[2].Value <<= pInfo->FactoryURL;

        lInfo[3].Name    = CFG_ENTRY_PROP_TEMPLATEURL;
        lInfo[3].Value <<= pInfo->TemplateURL;

        lInfo[4].Name    = CFG_ENTRY_PROP_TEMPURL;
        if (pInfo->OldTempURL.getLength())
            lInfo[4].Value <<= pInfo->OldTempURL;
        else
            lInfo[4].Value <<= pInfo->NewTempURL;

        lInfo[5].Name    = CFG_ENTRY_PROP_MODULE;
        lInfo[5].Value <<= pInfo->AppModule;

        lInfo[6].Name    = CFG_ENTRY_PROP_TITLE;
        lInfo[6].Value <<= pInfo->Title;

        lInfo[7].Name    = CFG_ENTRY_PROP_DOCUMENTSTATE;
        lInfo[7].Value <<= pInfo->DocumentState;

        aEvent.State <<= lInfo;
    }

    return aEvent;
}

//-----------------------------------------------
void AutoRecovery::implts_resetHandleStates(sal_Bool bLoadCache)
{
    implts_lockDocCache();
    // SAFE -> ------------------------------
    WriteGuard aWriteLock(m_aLock);

    AutoRecovery::TDocumentList::iterator pIt;
    for (  pIt  = m_lDocCache.begin();
           pIt != m_lDocCache.end()  ;
         ++pIt                       )
    {
        AutoRecovery::TDocumentInfo& rInfo = *pIt;
        rInfo.DocumentState &= ~AutoRecovery::E_HANDLED  ;
        rInfo.DocumentState &= ~AutoRecovery::E_POSTPONED;

        // SAFE -> ------------------------------
        aWriteLock.unlock();
        implts_flushConfigItem(rInfo);
        aWriteLock.lock();
        // <- SAFE ------------------------------
    }

    aWriteLock.unlock();
    // <- SAFE ----------------------------------
    implts_unlockDocCache();
}

//-----------------------------------------------
void AutoRecovery::implts_prepareEmergencySave()
{
    implts_hideAllDocs();
}

//-----------------------------------------------
void AutoRecovery::implts_doEmergencySave()
{
    // Write a hint "we chrashed" into the configuration, so
    // the error report tool is started too in case no recovery
    // documents exists and was saved.
    ::comphelper::ConfigurationHelper::writeDirectKey(
        m_xSMGR,
        CFG_PACKAGE_RECOVERY,
        CFG_PATH_RECOVERYINFO,
        CFG_ENTRY_CRASHED,
        css::uno::makeAny(sal_True),
        ::comphelper::ConfigurationHelper::E_STANDARD);

    // The called method for saving documents runs
    // during normal AutoSave more then once. Because
    // it postpone active documents and save it later.
    // That is normaly done by recalling it from a timer.
    // Here we must do it immediatly!
    // Of course this method returns the right state -
    // because it knows, that we are running in ERMERGENCY SAVE mode .-)

    sal_Bool                 bAllowUserIdleLoop = sal_False; // not allowed to change that .-)
    AutoRecovery::ETimerType eSuggestedTimer    = AutoRecovery::E_DONT_START_TIMER;
    do
    {
        eSuggestedTimer = implts_saveDocs(bAllowUserIdleLoop);
    }
    while(eSuggestedTimer == AutoRecovery::E_CALL_ME_BACK);

    // reset the handle state of all
    // cache items. Such handle state indicates, that a document
    // was already saved during the THIS(!) EmergencySave session.
    // Of course following recovery session must be started without
    // any "handle" state ...
    implts_resetHandleStates(sal_False);
}

//-----------------------------------------------
void AutoRecovery::implts_doRecovery()
{
    AutoRecovery::ETimerType eSuggestedTimer = AutoRecovery::E_DONT_START_TIMER;
    do
    {
        eSuggestedTimer = implts_openDocs();
    }
    while(eSuggestedTimer == AutoRecovery::E_CALL_ME_BACK);

    // reset the handle state of all
    // cache items. Such handle state indicates, that a document
    // was already saved during the THIS(!) Recovery session.
    // Of course a may be following EmergencySave session must be started without
    // any "handle" state ...
    implts_resetHandleStates(sal_True);

    // Reset the configuration hint "we was crashed"!
    ::comphelper::ConfigurationHelper::writeDirectKey(
        m_xSMGR,
        CFG_PACKAGE_RECOVERY,
        CFG_PATH_RECOVERYINFO,
        CFG_ENTRY_CRASHED,
        css::uno::makeAny(sal_False),
        ::comphelper::ConfigurationHelper::E_STANDARD);
}

//-----------------------------------------------
void AutoRecovery::implts_doSessionSave()
{
    // The called method for saving documents runs
    // during normal AutoSave more then once. Because
    // it postpone active documents and save it later.
    // That is normaly done by recalling it from a timer.
    // Here we must do it immediatly!
    // Of course this method returns the right state -
    // because it knows, that we are running in SESSION SAVE mode .-)

    sal_Bool                 bAllowUserIdleLoop = sal_False; // not allowed to change that .-)
    AutoRecovery::ETimerType eSuggestedTimer    = AutoRecovery::E_DONT_START_TIMER;
    do
    {
        eSuggestedTimer = implts_saveDocs(bAllowUserIdleLoop);
    }
    while(eSuggestedTimer == AutoRecovery::E_CALL_ME_BACK);

    // reset the handle state of all
    // cache items. Such handle state indicates, that a document
    // was already saved during the THIS(!) save session.
    // Of course following restore session must be started without
    // any "handle" state ...
    implts_resetHandleStates(sal_False);
}

//-----------------------------------------------
void AutoRecovery::implts_doSessionRestore()
{
    AutoRecovery::ETimerType eSuggestedTimer = AutoRecovery::E_DONT_START_TIMER;
    do
    {
        eSuggestedTimer = implts_openDocs();
    }
    while(eSuggestedTimer == AutoRecovery::E_CALL_ME_BACK);

    // reset the handle state of all
    // cache items. Such handle state indicates, that a document
    // was already saved during the THIS(!) Restore session.
    // Of course a may be following save session must be started without
    // any "handle" state ...
    implts_resetHandleStates(sal_True);
}

//-----------------------------------------------
void AutoRecovery::implts_backupWorkingEntry()
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    sal_Int32       nEntryID  = m_nWorkingEntryID;
    ::rtl::OUString sSavePath = m_sSavePath;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    implts_lockDocCache();

    AutoRecovery::TDocumentList::iterator pIt;
    for (  pIt  = m_lDocCache.begin();
           pIt != m_lDocCache.end()  ;
         ++pIt                       )
    {
        const AutoRecovery::TDocumentInfo& rInfo = *pIt;
        if (rInfo.ID != nEntryID)
            continue;

        ::rtl::OUString sSourceURL;
        // Prefer temp file. It contains the changes against the original document!
        if (rInfo.OldTempURL.getLength())
            sSourceURL = rInfo.OldTempURL;
        else
        if (rInfo.NewTempURL.getLength())
            sSourceURL = rInfo.NewTempURL;
        else
        if (rInfo.OrgURL.getLength())
            sSourceURL = rInfo.OrgURL;
        else
            continue; // nothing real to save! An unmodified but new created document.

        INetURLObject aParser(sSourceURL);
        AutoRecovery::EFailureSafeResult eResult = implts_copyFile(sSourceURL, sSavePath, aParser.getName());
        // TODO: Check eResult and react for errors (InteractionHandler!?)
        // Currently we ignore it ...
        // DONT UPDATE THE CACHE OR REMOVE ANY TEMP. FILES FROM DISK.
        // That has to be forced from outside explicitly.
        // See implts_cleanUpWorkingEntry() for further details.
    }

    implts_unlockDocCache();
}

//-----------------------------------------------
void AutoRecovery::implts_cleanUpWorkingEntry()
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    sal_Int32 nEntryID = m_nWorkingEntryID;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    implts_lockDocCache();

    AutoRecovery::TDocumentList::iterator pIt;
    for (  pIt  = m_lDocCache.begin();
           pIt != m_lDocCache.end()  ;
         ++pIt                       )
    {
        AutoRecovery::TDocumentInfo& rInfo = *pIt;
        if (rInfo.ID != nEntryID)
            continue;

        implts_removeTempFile(rInfo.OldTempURL);
        implts_removeTempFile(rInfo.NewTempURL);
        implts_flushConfigItem(rInfo, sal_True); // TRUE => remove it from xml config!

        m_lDocCache.erase(pIt);
        break; /// !!! pIt is not defined any longer ... further this function has finished it's work
    }

    implts_unlockDocCache();
}

//-----------------------------------------------
AutoRecovery::EFailureSafeResult AutoRecovery::implts_copyFile(const ::rtl::OUString& sSource    ,
                                                               const ::rtl::OUString& sTargetPath,
                                                               const ::rtl::OUString& sTargetName)
{
    // create content for the parent folder and call transfer on that content with the source content
    // and the destination file name as parameters

    css::uno::Reference< css::ucb::XCommandEnvironment > xEnvironment;

    ::ucb::Content aSourceContent;
    ::ucb::Content aTargetContent;

    try
    {
        aTargetContent = ::ucb::Content(sTargetPath, xEnvironment);
    }
    catch(const css::uno::Exception&)
        { return AutoRecovery::E_WRONG_TARGET_PATH; }

    sal_Int32 nNameClash;
//  nNameClash = css::ucb::NameClash::ERROR;
    nNameClash = css::ucb::NameClash::RENAME;
//  nNameClash = css::ucb::NameClash::OVERWRITE;

    try
    {
        ::ucb::Content::create(sSource, xEnvironment, aSourceContent);
        aTargetContent.transferContent(aSourceContent, ::ucb::InsertOperation_COPY, sTargetName, nNameClash);
    }
    catch(const css::uno::Exception&)
        { return AutoRecovery::E_ORIGINAL_FILE_MISSING; }

    return AutoRecovery::E_COPIED;
}

//-----------------------------------------------
sal_Bool SAL_CALL AutoRecovery::convertFastPropertyValue(      css::uno::Any& aConvertedValue,
                                                               css::uno::Any&   aOldValue      ,
                                                               sal_Int32        nHandle        ,
                                                         const css::uno::Any&   aValue         )
    throw(css::lang::IllegalArgumentException)
{
    // not needed currently
    return sal_False;
}

//-----------------------------------------------
void SAL_CALL AutoRecovery::setFastPropertyValue_NoBroadcast(      sal_Int32      nHandle,
                                                             const css::uno::Any& aValue )
    throw(css::uno::Exception)
{
    // not needed currently
}

//-----------------------------------------------
void SAL_CALL AutoRecovery::getFastPropertyValue(css::uno::Any& aValue ,
                                                 sal_Int32      nHandle) const
{
    switch(nHandle)
    {
        case AUTORECOVERY_PROPHANDLE_EXISTS_RECOVERYDATA :
//                aValue = fpc::getDirectValue(m_xSMGR, CFG_PACKAGE_RECOVERY, CFG_PATH_RECOVERYINFO, CFG_ENTRY_EXISTSRECOVERYIDATA);
                aValue <<= ((sal_Bool)(m_lDocCache.size()>0));
                break;

        case AUTORECOVERY_PROPHANDLE_CRASHED :
                aValue = ::comphelper::ConfigurationHelper::readDirectKey(
                            m_xSMGR,
                            CFG_PACKAGE_RECOVERY,
                            CFG_PATH_RECOVERYINFO,
                            CFG_ENTRY_CRASHED,
                            ::comphelper::ConfigurationHelper::E_READONLY);
                break;
    }
}

//-----------------------------------------------
const css::uno::Sequence< css::beans::Property > impl_getStaticPropertyDescriptor()
{
    static const css::beans::Property pPropertys[] =
    {
        css::beans::Property( AUTORECOVERY_PROPNAME_CRASHED            , AUTORECOVERY_PROPHANDLE_CRASHED            , ::getBooleanCppuType() , css::beans::PropertyAttribute::TRANSIENT | css::beans::PropertyAttribute::READONLY ),
        css::beans::Property( AUTORECOVERY_PROPNAME_EXISTS_RECOVERYDATA, AUTORECOVERY_PROPHANDLE_EXISTS_RECOVERYDATA, ::getBooleanCppuType() , css::beans::PropertyAttribute::TRANSIENT | css::beans::PropertyAttribute::READONLY ),
    };
    static const css::uno::Sequence< css::beans::Property > lPropertyDescriptor(pPropertys, AUTORECOVERY_PROPCOUNT);
    return lPropertyDescriptor;
}

//-----------------------------------------------
::cppu::IPropertyArrayHelper& SAL_CALL AutoRecovery::getInfoHelper()
{
    static ::cppu::OPropertyArrayHelper* pInfoHelper = 0;
    if(!pInfoHelper)
    {
        ::osl::MutexGuard aGuard( LockHelper::getGlobalLock().getShareableOslMutex() );
        if(!pInfoHelper)
        {
            static ::cppu::OPropertyArrayHelper aInfoHelper(impl_getStaticPropertyDescriptor(), sal_True);
            pInfoHelper = &aInfoHelper;
        }
    }

    return (*pInfoHelper);
}

//-----------------------------------------------
css::uno::Reference< css::beans::XPropertySetInfo > SAL_CALL AutoRecovery::getPropertySetInfo()
    throw(css::uno::RuntimeException)
{
    static css::uno::Reference< css::beans::XPropertySetInfo >* pInfo = 0;
    if(!pInfo)
    {
        ::osl::MutexGuard aGuard( LockHelper::getGlobalLock().getShareableOslMutex() );
        if(!pInfo)
        {
            static css::uno::Reference< css::beans::XPropertySetInfo > xInfo(createPropertySetInfo(getInfoHelper()));
            pInfo = &xInfo;
        }
    }

    return (*pInfo);
}

//-----------------------------------------------
void AutoRecovery::implts_lockDocCache()
{
    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);
    if (m_bDocCacheLock)
    {
        OSL_ENSURE(sal_False, "Re-entrance problem detected. Using of an stl structure in combination with iteration, adding, removing of elements etcpp.");
        throw css::uno::RuntimeException(
                ::rtl::OUString::createFromAscii("Re-entrance problem detected. Using of an stl structure in combination with iteration, adding, removing of elements etcpp."),
                static_cast< css::frame::XDispatch* >(this));
    }
    m_bDocCacheLock = sal_True;
    aWriteLock.unlock();
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
void AutoRecovery::implts_unlockDocCache()
{
    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);
    m_bDocCacheLock = sal_False;
    aWriteLock.unlock();
    // <- SAFE ----------------------------------
}

} // namespace framework
