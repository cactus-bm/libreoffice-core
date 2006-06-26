/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sfxbasemodel.cxx,v $
 *
 *  $Revision: 1.112 $
 *
 *  last change: $Author: rt $ $Date: 2006-06-26 09:52:22 $
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

//________________________________________________________________________________________________________
//  my own includes
//________________________________________________________________________________________________________

#ifndef _SFX_SFXBASEMODEL_HXX_
#include <sfxbasemodel.hxx>
#endif

//________________________________________________________________________________________________________
//  include of other projects
//________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_TASK_XINTERACTIONHANDLER_HPP_
#include <com/sun/star/task/XInteractionHandler.hpp>
#endif

#ifndef _COM_SUN_STAR_TASK_ERRORCODEREQUEST_HPP_
#include <com/sun/star/task/ErrorCodeRequest.hpp>
#endif

#ifndef _COM_SUN_STAR_VIEW_XPRINTJOB_HPP_
#include <com/sun/star/view/XPrintJob.hpp>
#endif

#ifndef _COM_SUN_STAR_VIEW_XSELECTIONSUPPLIER_HPP_
#include <com/sun/star/view/XSelectionSupplier.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_SIZE_HPP_
#include <com/sun/star/awt/Size.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_ILLEGALARGUMENTEXCEPTION_HPP_
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_ILLEGALARGUMENTIOEXCEPTION_HPP_
#include <com/sun/star/frame/IllegalArgumentIOException.hpp>
#endif

#ifndef _COM_SUN_STAR_VIEW_PAPERFORMAT_HPP_
#include <com/sun/star/view/PaperFormat.hpp>
#endif

#ifndef _COM_SUN_STAR_VIEW_PAPERORIENTATION_HPP_
#include <com/sun/star/view/PaperOrientation.hpp>
#endif

#ifndef _COM_SUN_STAR_EMBED_XTRANSACTIONBROADCASTER_HPP_
#include <com/sun/star/embed/XTransactionBroadcaster.hpp>
#endif

#ifndef _COM_SUN_STAR_EMBED_XSTORAGE_HPP_
#include <com/sun/star/embed/XStorage.hpp>
#endif

#ifndef _COM_SUN_STAR_EMBED_EMBEDMAPUNITS_HPP_
#include <com/sun/star/embed/EmbedMapUnits.hpp>
#endif

#ifndef _COM_SUN_STAR_DOCUMENT_XSTORAGECHANGELISTENER_HPP_
#include <com/sun/star/document/XStorageChangeListener.hpp>
#endif

#ifndef _COM_SUN_STAR_DOCUMENT_XACTIONLOCKABLE_HPP_
#include <com/sun/star/document/XActionLockable.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#ifndef _CPPUHELPER_INTERFACECONTAINER_HXX_
#include <cppuhelper/interfacecontainer.hxx>
#endif

#ifndef _UNOTOOLS_PROCESSFACTORY_HXX
#include <comphelper/processfactory.hxx>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XINDEXCONTAINER_HPP_
#include <com/sun/star/container/XIndexContainer.hpp>
#endif

#ifndef _COM_SUN_STAR_UCB_NAMECLASH_HPP_
#include <com/sun/star/ucb/NameClash.hpp>
#endif

#ifndef  _COM_SUN_STAR_SCRIPT_PROVIDER_XSCRIPTPROVIDERFACTORY_HPP_
#include <com/sun/star/script/provider/XScriptProviderFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_SCRIPT_PROVIDER_XSCRIPTPROVIDER_HPP_
#include <com/sun/star/script/provider/XScriptProvider.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_XUICONFIGURATIONMANAGERSUPPLIER_HPP_
#include <com/sun/star/ui/XUIConfigurationManagerSupllier.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_XUICONFIGURATIONSTORAGE_HPP_
#include <com/sun/star/ui/XUIConfigurationStorage.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_XUICONFIGURATIONPERSISTENCE_HPP_
#include <com/sun/star/ui/XUIConfigurationPersistence.hpp>
#endif

#ifndef _COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif

#ifndef _COM_SUN_STAR_EMBED_ASPECTS_HPP_
#include <com/sun/star/embed/Aspects.hpp>
#endif

#ifndef _UNO_MAPPING_HXX_
#include <uno/mapping.hxx>
#endif

#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif

#ifndef _SFXSTRITEM_HXX
#include <svtools/stritem.hxx>
#endif

#ifndef _SFXINTITEM_HXX
#include <svtools/intitem.hxx>
#endif

#ifndef _SFXENUMITEM_HXX
#include <svtools/eitem.hxx>
#endif

#include <basic/sbx.hxx>
#include <basic/sbuno.hxx>

#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif

#ifndef _THREAD_HXX_
#include <osl/thread.hxx>
#endif

#ifndef _URLOBJ_HXX_
#include <tools/urlobj.hxx>
#endif

#ifndef _UNOTOOLS_TEMPFILE_HXX_
#include <unotools/tempfile.hxx>
#endif

#ifndef _UNOTOOLS_LOCALFILEHELPER_HXX_
#include <unotools/localfilehelper.hxx>
#endif

#ifndef _UCBHELPER_CONTENT_HXX
#include <ucbhelper/content.hxx>
#endif

#include <vos/mutex.hxx>
#include <vcl/salctype.hxx>
#include <svtools/printdlg.hxx>
#include <sot/clsids.hxx>
#include <sot/storinfo.hxx>
#include <comphelper/storagehelper.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <svtools/transfer.hxx>
#include <rtl/logfile.hxx>
#include <framework/configimporter.hxx>

//________________________________________________________________________________________________________
//  includes of my own project
//________________________________________________________________________________________________________

#ifndef _SFX_SFXUNO_HXX
#include <sfxuno.hxx>
#endif

#ifndef _SFX_OBJSHIMP_HXX
#include <objshimp.hxx>
#endif

#ifndef _SFXVIEWFRM_HXX
#include <viewfrm.hxx>
#endif

#ifndef _SFXVIEWSH_HXX
#include <viewsh.hxx>
#endif

#ifndef _SFXDOCFILE_HXX
#include <docfile.hxx>
#endif

#ifndef _SFXDISPATCH_HXX
#include <dispatch.hxx>
#endif

#ifndef _SFXREQUEST_HXX
#include <request.hxx>
#endif

#ifndef _SFX_OBJUNO_HXX
#include <objuno.hxx>
#endif

#ifndef _SFX_PRINTER_HXX
#include <printer.hxx>
#endif

#ifndef _SFX_BASMGR_HXX
#include <basmgr.hxx>
#endif

#ifndef _SFXEVENT_HXX
#include <event.hxx>
#endif

#ifndef _SFX_EVENTSUPPLIER_HXX_
#include <eventsupplier.hxx>
#endif

#ifndef _SFX_EVENTCONF_HXX
#include <evntconf.hxx>
#endif

#ifndef _SFX_HRC
#include "sfx.hrc"
#endif

#include <framework/interaction.hxx>

#include "app.hxx"
#include "topfrm.hxx"
#include "appdata.hxx"
#include "docfac.hxx"
#include "fcontnr.hxx"
#include "commitlistener.hxx"
#include "stormodifylistener.hxx"
#include "openflag.hxx"
#include "brokenpackageint.hxx"
#include "graphhelp.hxx"
#include "msgpool.hxx"

using namespace com::sun::star;

//________________________________________________________________________________________________________
//  defines
//________________________________________________________________________________________________________

#define XFRAME                                  ::com::sun::star::frame::XFrame
#define XINTERFACE                              ::com::sun::star::uno::XInterface
#define OMULTITYPEINTERFACECONTAINERHELPER      ::cppu::OMultiTypeInterfaceContainerHelper
#define UNOQUERY                                ::com::sun::star::uno::UNO_QUERY
#define DISPOSEDEXCEPTION                       ::com::sun::star::lang::DisposedException
#define MAPPING                                 ::com::sun::star::uno::Mapping
#define XSELECTIONSUPPLIER                      ::com::sun::star::view::XSelectionSupplier
#define ANY                                     ::com::sun::star::uno::Any
#define ILLEGALARGUMENTEXCEPTION                ::com::sun::star::lang::IllegalArgumentException
#define ILLEGALARGUMENTIOEXCEPTION              ::com::sun::star::frame::IllegalArgumentIOException
#define OINTERFACECONTAINERHELPER               ::cppu::OInterfaceContainerHelper
#define OINTERFACEITERATORHELPER                ::cppu::OInterfaceIteratorHelper
#define SIZE                                    ::com::sun::star::awt::Size
#define PAPERFORMAT                             ::com::sun::star::view::PaperFormat
#define PAPERORIENTATION                        ::com::sun::star::view::PaperOrientation
#define OTYPECOLLECTION                         ::cppu::OTypeCollection
#define OIMPLEMENTATIONID                       ::cppu::OImplementationId
#define MUTEXGUARD                              ::osl::MutexGuard
#define XINDEXCONTAINER                         ::com::sun::star::container::XIndexContainer
#define UNSUPPORTEDFLAVOREXCEPTION              ::com::sun::star::datatransfer::UnsupportedFlavorException
#define XPRINTJOBLISTENER                       ::com::sun::star::view::XPrintJobListener
#define XUICONFIGURATIONSTORAGE                 ::com::sun::star::ui::XUIConfigurationStorage
#define XPROPERTYSET                            ::com::sun::star::beans::XPropertySet
#define XSCRIPTPROVIDERFACTORY ::com::sun::star::script::provider::XScriptProviderFactory
//________________________________________________________________________________________________________
//  namespaces
//________________________________________________________________________________________________________


// Don't use using ... here, because there are at least two classes with the same name in use

//using namespace ::osl                             ;
//using namespace ::rtl                             ;
//using namespace ::cppu                            ;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
//using namespace ::com::sun::star::container       ;
//using namespace ::com::sun::star::frame           ;
//using namespace ::com::sun::star::document        ;
//using namespace ::com::sun::star::lang            ;
//using namespace ::com::sun::star::util            ;
//using namespace ::com::sun::star::view            ;
//using namespace ::com::sun::star::beans           ;

//________________________________________________________________________________________________________
//  impl. declarations
//________________________________________________________________________________________________________

struct IMPL_SfxBaseModel_DataContainer
{
    // counter for SfxBaseModel instances created.
    static sal_Int64                                g_nInstanceCounter      ;

    SfxObjectShellRef                               m_pObjectShell          ;
    //SfxObjectShellLock                                m_pObjectShellLock      ;
    OUSTRING                                        m_sURL                  ;
    OUSTRING                                        m_sRuntimeUID           ;
    sal_uInt16                                      m_nControllerLockCount  ;
    OMULTITYPEINTERFACECONTAINERHELPER              m_aInterfaceContainer   ;
    REFERENCE< XINTERFACE >                         m_xParent               ;
    REFERENCE< XCONTROLLER >                        m_xCurrent              ;
    REFERENCE< XDOCUMENTINFO >                      m_xDocumentInfo         ;
    REFERENCE< XSTARBASICACCESS >                   m_xStarBasicAccess      ;
    REFERENCE< XNAMEREPLACE >                       m_xEvents               ;
    SEQUENCE< PROPERTYVALUE>                        m_seqArguments          ;
    SEQUENCE< REFERENCE< XCONTROLLER > >            m_seqControllers        ;
    REFERENCE< XINDEXACCESS >                       m_contViewData          ;
    sal_Bool                                        m_bClosed               ;
    sal_Bool                                        m_bClosing              ;
    sal_Bool                                        m_bSaving               ;
    sal_Bool                                        m_bSuicide              ;
    REFERENCE< com::sun::star::view::XPrintJob>     m_xPrintJob             ;
    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > m_aPrintOptions;
    REFERENCE< XSCRIPTPROVIDER >                        m_xScriptProvider;
    REFERENCE< XUICONFIGURATIONMANAGER >                m_xUIConfigurationManager;
    OStorageModifyListen_Impl*                          m_pStorageModifyListen;
    ::rtl::OUString                                 m_aPreusedFilterName;

    IMPL_SfxBaseModel_DataContainer(    MUTEX&          aMutex          ,
                                                                        SfxObjectShell* pObjectShell    )
            :   m_pObjectShell          ( pObjectShell  )
//          ,   m_pObjectShellLock      ( pObjectShell  )
            ,   m_sURL                  ( String()      )
//            ,   m_sRuntimeUID           ( String()      )
            ,   m_nControllerLockCount  ( 0             )
            ,   m_aInterfaceContainer   ( aMutex        )
            ,   m_bClosed               ( sal_False     )
            ,   m_bClosing              ( sal_False     )
            ,   m_bSaving               ( sal_False     )
            ,   m_bSuicide              ( sal_False     )
            ,   m_pStorageModifyListen  ( NULL          )
    {
        // increase global instance counter.
        ++g_nInstanceCounter;
        // set own Runtime UID
        m_sRuntimeUID = rtl::OUString::valueOf( g_nInstanceCounter );
    }
} ;

// static member initialization.
sal_Int64 IMPL_SfxBaseModel_DataContainer::g_nInstanceCounter = 0;

SIZE impl_Size_Object2Struct( const Size& aSize )
{
    SIZE aReturnValue;

    aReturnValue.Width  = aSize.Width()  ;
    aReturnValue.Height = aSize.Height() ;

    return aReturnValue ;
}

Size impl_Size_Struct2Object( const SIZE& aSize )
{
    Size aReturnValue;

    aReturnValue.Width()  = aSize.Width  ;
    aReturnValue.Height() = aSize.Height ;

    return aReturnValue ;
}

class SfxPrintJob_Impl : public cppu::WeakImplHelper1
<
    com::sun::star::view::XPrintJob
>
{
        IMPL_SfxBaseModel_DataContainer* m_pData;

public:
        SfxPrintJob_Impl( IMPL_SfxBaseModel_DataContainer* pData );
        virtual Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getPrintOptions(  ) throw (RuntimeException);
        virtual Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getPrinter(  ) throw (RuntimeException);
        virtual Reference< ::com::sun::star::view::XPrintable > SAL_CALL getPrintable(  ) throw (RuntimeException);
        virtual void SAL_CALL cancelJob() throw (RuntimeException);
};

SfxPrintJob_Impl::SfxPrintJob_Impl( IMPL_SfxBaseModel_DataContainer* pData )
    : m_pData( pData )
{
}

Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL SfxPrintJob_Impl::getPrintOptions() throw (RuntimeException)
{
    return m_pData->m_aPrintOptions;
}

Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL SfxPrintJob_Impl::getPrinter() throw (RuntimeException)
{
    if( m_pData->m_pObjectShell.Is() )
    {
        Reference < view::XPrintable > xPrintable( m_pData->m_pObjectShell->GetModel(), UNO_QUERY );
        if ( xPrintable.is() )
            return xPrintable->getPrinter();
    }
    return Sequence< ::com::sun::star::beans::PropertyValue >();
}

Reference< ::com::sun::star::view::XPrintable > SAL_CALL SfxPrintJob_Impl::getPrintable() throw (RuntimeException)
{
    Reference < view::XPrintable > xPrintable( m_pData->m_pObjectShell.Is() ? m_pData->m_pObjectShell->GetModel() : NULL, UNO_QUERY );
    return xPrintable;
}

void SAL_CALL SfxPrintJob_Impl::cancelJob() throw (RuntimeException)
{
    if( m_pData->m_pObjectShell.Is() )
        m_pData->m_pObjectShell->Broadcast( SfxPrintingHint( -2, NULL, NULL ) );
}

// SfxOwnFramesLocker ====================================================================================
// allows to lock all the frames related to the provided SfxObjectShell
class SfxOwnFramesLocker
{
    uno::Sequence< uno::Reference< frame::XFrame > > m_aLockedFrames;

    Window* GetVCLWindow( const uno::Reference< frame::XFrame >& xFrame );
public:
    SfxOwnFramesLocker( SfxObjectShell* ObjechShell );
    ~SfxOwnFramesLocker();
    void UnlockFrames();
};

SfxOwnFramesLocker::SfxOwnFramesLocker( SfxObjectShell* pObjectShell )
{
    if ( !pObjectShell )
        return;

    for ( SfxViewFrame *pFrame = SfxViewFrame::GetFirst(pObjectShell, TYPE(SfxTopViewFrame) ); pFrame;
            pFrame = SfxViewFrame::GetNext(*pFrame, pObjectShell, TYPE(SfxTopViewFrame) ) )
    {
        SfxFrame* pSfxFrame = pFrame->GetFrame();
        if ( pSfxFrame )
        {
            try
            {
                // get vcl window related to the frame and lock it if it is still not locked
                uno::Reference< frame::XFrame > xFrame = pSfxFrame->GetFrameInterface();
                Window* pWindow = GetVCLWindow( xFrame );
                if ( !pWindow )
                    throw uno::RuntimeException();

                if ( pWindow->IsEnabled() )
                {
                    pWindow->Disable();

                    try
                    {
                        sal_Int32 nLen = m_aLockedFrames.getLength();
                        m_aLockedFrames.realloc( nLen + 1 );
                        m_aLockedFrames[nLen] = xFrame;
                    }
                    catch( uno::Exception& )
                    {
                        pWindow->Enable();
                        throw;
                    }
                }
            }
            catch( uno::Exception& )
            {
                OSL_ENSURE( sal_False, "Not possible to lock the frame window!\n" );
            }
        }
    }
}

SfxOwnFramesLocker::~SfxOwnFramesLocker()
{
    UnlockFrames();
}

Window* SfxOwnFramesLocker::GetVCLWindow( const uno::Reference< frame::XFrame >& xFrame )
{
    Window* pWindow = NULL;

    if ( xFrame.is() )
    {
        uno::Reference< awt::XWindow > xWindow = xFrame->getContainerWindow();
        if ( xWindow.is() )
               pWindow = VCLUnoHelper::GetWindow( xWindow );
    }

    return pWindow;
}

void SfxOwnFramesLocker::UnlockFrames()
{
    for ( sal_Int32 nInd = 0; nInd < m_aLockedFrames.getLength(); nInd++ )
    {
        try
        {
            if ( m_aLockedFrames[nInd].is() )
            {
                // get vcl window related to the frame and unlock it
                Window* pWindow = GetVCLWindow( m_aLockedFrames[nInd] );
                if ( !pWindow )
                    throw uno::RuntimeException();

                pWindow->Enable();

                m_aLockedFrames[nInd] = uno::Reference< frame::XFrame >();
            }
        }
        catch( uno::Exception& )
        {
            OSL_ENSURE( sal_False, "Can't unlock the frame window!\n" );
        }
    }
}

// SfxSaveGuard ====================================================================================
class SfxSaveGuard
{
    private:
        REFERENCE< XMODEL > m_xModel;
        IMPL_SfxBaseModel_DataContainer* m_pData;
        SfxOwnFramesLocker* m_pFramesLock;

    public:
        SfxSaveGuard(const REFERENCE< XMODEL >&             xModel                      ,
                           IMPL_SfxBaseModel_DataContainer* pData                       ,
                           sal_Bool                         bRejectConcurrentSaveRequest);
        ~SfxSaveGuard();
};

SfxSaveGuard::SfxSaveGuard(const REFERENCE< XMODEL >&             xModel                      ,
                                 IMPL_SfxBaseModel_DataContainer* pData                       ,
                                 sal_Bool                         bRejectConcurrentSaveRequest)
    : m_xModel     (xModel)
    , m_pData      (pData )
    , m_pFramesLock(0     )
{
    static ::rtl::OUString MSG_1 = ::rtl::OUString::createFromAscii("Object already disposed."                                       );
    static ::rtl::OUString MSG_2 = ::rtl::OUString::createFromAscii("Concurrent save requests on the same document are not possible.");

    if ( m_pData->m_bClosed )
        throw ::com::sun::star::lang::DisposedException(
                MSG_1,
                ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >());

    if (
        bRejectConcurrentSaveRequest &&
        m_pData->m_bSaving
       )
        throw ::com::sun::star::io::IOException(
                MSG_2,
                ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >());

    m_pData->m_bSaving = sal_True;
    m_pFramesLock = new SfxOwnFramesLocker(m_pData->m_pObjectShell);
}

SfxSaveGuard::~SfxSaveGuard()
{
    SfxOwnFramesLocker* pFramesLock = m_pFramesLock;
    m_pFramesLock = 0;
    delete pFramesLock;

    m_pData->m_bSaving = sal_False;

    // m_bSuicide was set e.g. in case somewhere tried to close a document, while it was used for
    // storing at the same time. Further m_bSuicide was set to TRUE only if close(TRUE) was called.
    // So the owner ship was delegated to the place where a veto exception was thrown.
    // Now we have to call close() again and delegate the owner ship to the next one, which
    // cant accept that. Close(FALSE) cant work in this case. Because then the document will may be never closed ...

    if ( m_pData->m_bSuicide )
    {
        // Reset this state. In case the new close() request is not accepted by somehwere else ...
        // it's not a good idea to have two "owners" for close .-)
        m_pData->m_bSuicide = sal_False;
        try
        {
            REFERENCE< XCLOSEABLE > xClose(m_xModel, UNOQUERY);
            if (xClose.is())
                xClose->close(sal_True);
        }
        catch(const CLOSEVETOEXCEPTION&)
        {}
    }
}

// =======================================================================================================

//________________________________________________________________________________________________________
//  constructor
//________________________________________________________________________________________________________

SfxBaseModel::SfxBaseModel( SfxObjectShell *pObjectShell )
: IMPL_SfxBaseModel_MutexContainer()
, m_pData( new IMPL_SfxBaseModel_DataContainer( m_aMutex, pObjectShell ) )
{
    if ( pObjectShell != NULL )
    {
        StartListening( *pObjectShell ) ;
    }
}

//________________________________________________________________________________________________________
//  destructor
//________________________________________________________________________________________________________

SfxBaseModel::~SfxBaseModel()
{
}

//________________________________________________________________________________________________________
//  XInterface
//________________________________________________________________________________________________________

ANY SAL_CALL SfxBaseModel::queryInterface( const UNOTYPE& rType ) throw( RUNTIMEEXCEPTION )
{
    // Attention:
    //  Don't use mutex or guard in this method!!! Is a method of XInterface.

    // Ask for my own supported interfaces ...
    ANY aReturn( ::cppu::queryInterface(    rType                                           ,
                                               static_cast< XTYPEPROVIDER*          > ( this )  ,
                                               static_cast< XCHILD*             > ( this )  ,
                                               static_cast< XDOCUMENTINFOSUPPLIER*  > ( this )  ,
                                               static_cast< XEVENTLISTENER*     > ( this )  ,
                                               static_cast< XMODEL*             > ( this )  ,
                                               static_cast< XMODIFIABLE*            > ( this )  ,
                                            static_cast< XCOMPONENT*            > ( this )  ,
                                               static_cast< XPRINTABLE*         > ( this )  ,
                                               static_cast< XSTARBASICACCESS*       > ( this )  ,
                                            static_cast< XSTORABLE*             > ( this )  ,
                                            static_cast< XLOADABLE*             > ( this )  ,
                                            static_cast< XCLOSEABLE*            > ( this )  ) );

    if ( aReturn.hasValue() == sal_False )
    {
        aReturn = ::cppu::queryInterface(   rType                                           ,
                                            static_cast< XMODIFYBROADCASTER*    > ( this )  ,
                                            static_cast< XTRANSFERABLE*    > ( this )  ,
                                               static_cast< XPRINTJOBBROADCASTER*   > ( this )  ,
                                                static_cast< XCLOSEBROADCASTER*     > ( this )  ,
                                            static_cast< XVIEWDATASUPPLIER*     > ( this )  ,
                                               static_cast< XEVENTBROADCASTER*      > ( this )  ,
                                               static_cast< XVISUALOBJECT*          > ( this )  ,
                                               static_cast< XUNOTUNNEL*             > ( this )  ,
                                            static_cast< XUICONFIGURATIONMANAGERSUPPLIER* > ( this ) ,
                                               static_cast< XDOCUMENTSUBSTORAGESUPPLIER* > ( this ) ,
                                               static_cast< XSCRIPTPROVIDERSUPPLIER* > ( this ) ,
                                               static_cast< XEVENTSSUPPLIER*        > ( this ) ) ;
    }

    if ( aReturn.hasValue() == sal_False )
    {
        aReturn = ::cppu::queryInterface(   rType                                           ,
                                            static_cast< XSTORABLE2*            > ( this )  ,
                                               static_cast< XSTORAGEBASEDDOCUMENT* > ( this )   );
    }

    // If searched interface supported by this class ...
    if ( aReturn.hasValue() == sal_True )
    {
        // ... return this information.
        return aReturn ;
    }
    else
    {
        // Else; ... ask baseclass for interfaces!
        return OWeakObject::queryInterface( rType ) ;
    }
}

//________________________________________________________________________________________________________
//  XInterface
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::acquire() throw( )
{
    // Attention:
    //  Don't use mutex or guard in this method!!! Is a method of XInterface.

    // Forward to baseclass
    OWeakObject::acquire() ;
}

//________________________________________________________________________________________________________
//  XInterface
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::release() throw( )
{
    // Attention:
    //  Don't use mutex or guard in this method!!! Is a method of XInterface.

    // Forward to baseclass
    OWeakObject::release() ;
}

//________________________________________________________________________________________________________
//  XTypeProvider
//________________________________________________________________________________________________________

SEQUENCE< UNOTYPE > SAL_CALL SfxBaseModel::getTypes() throw( RUNTIMEEXCEPTION )
{
    // Optimize this method !
    // We initialize a static variable only one time. And we don't must use a mutex at every call!
    // For the first call; pTypeCollection is NULL - for the second call pTypeCollection is different from NULL!
    static OTYPECOLLECTION* pTypeCollection = NULL ;

    if ( pTypeCollection == NULL )
    {
        // Ready for multithreading; get global mutex for first call of this method only! see before
        MUTEXGUARD aGuard( MUTEX::getGlobalMutex() ) ;

        // Control these pointer again ... it can be, that another instance will be faster then these!
        if ( pTypeCollection == NULL )
        {
            // Create a static typecollection ...
            static OTYPECOLLECTION aTypeCollectionFirst( ::getCppuType(( const REFERENCE< XTYPEPROVIDER          >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XCHILD                 >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XDOCUMENTINFOSUPPLIER  >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XEVENTLISTENER         >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XMODEL                 >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XMODIFIABLE            >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XPRINTABLE             >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XSTORABLE2             >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XLOADABLE              >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XCLOSEABLE             >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XSTARBASICACCESS       >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XEVENTBROADCASTER      >*)NULL ) );

            static OTYPECOLLECTION aTypeCollection     ( ::getCppuType(( const REFERENCE< XVIEWDATASUPPLIER      >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XTRANSFERABLE          >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XPRINTJOBBROADCASTER   >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XEVENTSSUPPLIER        >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XVISUALOBJECT          >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XUNOTUNNEL             >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XUICONFIGURATIONMANAGERSUPPLIER >*)NULL ),
                                                         ::getCppuType(( const REFERENCE< XDOCUMENTSUBSTORAGESUPPLIER >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XSTORAGEBASEDDOCUMENT >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XSCRIPTPROVIDERSUPPLIER >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XCLOSEBROADCASTER      >*)NULL ) ,
                                                         ::getCppuType(( const REFERENCE< XUICONFIGURATIONMANAGERSUPPLIER >*)NULL ) ,
                                                         aTypeCollectionFirst.getTypes()                                   );

            // ... and set his address to static pointer!
            pTypeCollection = &aTypeCollection ;
        }
    }

    return pTypeCollection->getTypes() ;
}

//________________________________________________________________________________________________________
//  XTypeProvider
//________________________________________________________________________________________________________

SEQUENCE< sal_Int8 > SAL_CALL SfxBaseModel::getImplementationId() throw( RUNTIMEEXCEPTION )
{
    // Create one Id for all instances of this class.
    // Use ethernet address to do this! (sal_True)

    // Optimize this method
    // We initialize a static variable only one time. And we don't must use a mutex at every call!
    // For the first call; pID is NULL - for the second call pID is different from NULL!
    static OIMPLEMENTATIONID* pID = NULL ;

    if ( pID == NULL )
    {
        // Ready for multithreading; get global mutex for first call of this method only! see before
        MUTEXGUARD aGuard( MUTEX::getGlobalMutex() ) ;

        // Control these pointer again ... it can be, that another instance will be faster then these!
        if ( pID == NULL )
        {
            // Create a new static ID ...
            static OIMPLEMENTATIONID aID( sal_False ) ;
            // ... and set his address to static pointer!
            pID = &aID ;
        }
    }

    return pID->getImplementationId() ;
}

//________________________________________________________________________________________________________
//  XStarBasicAccess
//________________________________________________________________________________________________________

REFERENCE< XSTARBASICACCESS > implGetStarBasicAccess( SfxObjectShell* pObjectShell )
{
    REFERENCE< XSTARBASICACCESS > xRet;
    if( pObjectShell )
    {
        BasicManager* pMgr = pObjectShell->GetBasicManager();
        xRet = getStarBasicAccess( pMgr );
    }
    return xRet;
}

REFERENCE< XNAMECONTAINER > SAL_CALL SfxBaseModel::getLibraryContainer() throw( RUNTIMEEXCEPTION )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    REFERENCE< XSTARBASICACCESS >& rxAccess = m_pData->m_xStarBasicAccess;
    if( !rxAccess.is() && m_pData->m_pObjectShell.Is() )
        rxAccess = implGetStarBasicAccess( m_pData->m_pObjectShell );

    REFERENCE< XNAMECONTAINER > xRet;
    if( rxAccess.is() )
        xRet = rxAccess->getLibraryContainer();
    return xRet;
}

/**___________________________________________________________________________________________________
    @seealso    XStarBasicAccess
*/
void SAL_CALL SfxBaseModel::createLibrary( const OUSTRING& LibName, const OUSTRING& Password,
    const OUSTRING& ExternalSourceURL, const OUSTRING& LinkTargetURL )
        throw(ELEMENTEXISTEXCEPTION, RUNTIMEEXCEPTION)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    REFERENCE< XSTARBASICACCESS >& rxAccess = m_pData->m_xStarBasicAccess;
    if( !rxAccess.is() && m_pData->m_pObjectShell.Is() )
        rxAccess = implGetStarBasicAccess( m_pData->m_pObjectShell );

    if( rxAccess.is() )
        rxAccess->createLibrary( LibName, Password, ExternalSourceURL, LinkTargetURL );
}

/**___________________________________________________________________________________________________
    @seealso    XStarBasicAccess
*/
void SAL_CALL SfxBaseModel::addModule( const OUSTRING& LibraryName, const OUSTRING& ModuleName,
    const OUSTRING& Language, const OUSTRING& Source )
        throw( NOSUCHELEMENTEXCEPTION, RUNTIMEEXCEPTION)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    REFERENCE< XSTARBASICACCESS >& rxAccess = m_pData->m_xStarBasicAccess;
    if( !rxAccess.is() && m_pData->m_pObjectShell.Is() )
        rxAccess = implGetStarBasicAccess( m_pData->m_pObjectShell );

    if( rxAccess.is() )
        rxAccess->addModule( LibraryName, ModuleName, Language, Source );
}

/**___________________________________________________________________________________________________
    @seealso    XStarBasicAccess
*/
void SAL_CALL SfxBaseModel::addDialog( const OUSTRING& LibraryName, const OUSTRING& DialogName,
    const ::com::sun::star::uno::Sequence< sal_Int8 >& Data )
        throw(NOSUCHELEMENTEXCEPTION, RUNTIMEEXCEPTION)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    REFERENCE< XSTARBASICACCESS >& rxAccess = m_pData->m_xStarBasicAccess;
    if( !rxAccess.is() && m_pData->m_pObjectShell.Is() )
        rxAccess = implGetStarBasicAccess( m_pData->m_pObjectShell );

    if( rxAccess.is() )
        rxAccess->addDialog( LibraryName, DialogName, Data );
}


//________________________________________________________________________________________________________
//  XChild
//________________________________________________________________________________________________________

REFERENCE< XINTERFACE > SAL_CALL SfxBaseModel::getParent() throw( RUNTIMEEXCEPTION )
{
/*  #77222#
     AS->MBA: There is one view only at the moment. We don't must search for other parents in other frames ...!?

    if ( !m_pData->m_xParent.is() && m_pData->m_xCurrent.is() )
    {
        // If no parent is set get the parent by view hierarchy
        REFERENCE< XFRAME >  xParentFrame( m_pData->m_xCurrent->getFrame()->getCreator(), UNOQUERY );
        if ( xParentFrame.is() )
        {
            REFERENCE< XCONTROLLER >  xCtrl( xParentFrame->getController() );
            if ( xCtrl.is() )
                return xCtrl->getModel();
        }
    }
*/
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    return m_pData->m_xParent;
}

//________________________________________________________________________________________________________
//  XChild
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::setParent(const REFERENCE< XINTERFACE >& Parent) throw(NOSUPPORTEXCEPTION, RUNTIMEEXCEPTION)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );

    //if ( Parent.is() && getParent().is() )
    //    // only set parent when no parent is available
    //    throw NOSUPPORTEXCEPTION();

    //DBG_ASSERT( !Parent.is() || !getParent().is(), "Changing parent directly is not allowed!" );
    m_pData->m_xParent = Parent;
}

//________________________________________________________________________________________________________
//  XChild
//________________________________________________________________________________________________________

long SfxObjectShellClose_Impl( void* pObj, void* pArg );

void SAL_CALL SfxBaseModel::dispose() throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );

    // object already disposed?
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if  ( !m_pData->m_bClosed )
    {
        // gracefully accept wrong dispose calls instead of close call
        // and try to make it work (may be really disposed later!)
        try
        {
            close( sal_True );
        }
        catch ( com::sun::star::util::CloseVetoException& )
        {
        }

        return;
    }

    // disconnect from the listener so that no more messages from substorages come
#if 0
    if ( m_pData->m_pChildCommitListen )
    {
        m_pData->m_pChildCommitListen->OwnerIsDisposed();
        m_pData->m_pChildCommitListen->release();
        m_pData->m_pChildCommitListen = NULL;
    }
#endif

    if ( m_pData->m_pStorageModifyListen )
    {
        m_pData->m_pStorageModifyListen->OwnerIsDisposed();
        m_pData->m_pStorageModifyListen->release();
        m_pData->m_pStorageModifyListen = NULL;
    }

    EVENTOBJECT aEvent( (XMODEL *)this );
    m_pData->m_aInterfaceContainer.disposeAndClear( aEvent );

    // is an object shell assigned?
    if ( m_pData->m_pObjectShell.Is() )
    {
        // Rekursion vermeiden
        SfxObjectShellRef pShell;
        //SfxObjectShellLock pShellLock;

        {
            // am I "ThisComponent" in AppBasic?
            StarBASIC* pBas = SFX_APP()->GetBasic_Impl();
            if ( pBas && SFX_APP()->Get_Impl()->pThisDocument == m_pData->m_pObjectShell )
            {
                // remove "ThisComponent" reference from AppBasic
                SFX_APP()->Get_Impl()->pThisDocument = NULL;
                SbxVariable *pCompVar = pBas->Find( DEFINE_CONST_UNICODE("ThisComponent"), SbxCLASS_OBJECT );
                if ( pCompVar )
                {
                    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > xInterface;
                    ::com::sun::star::uno::Any aComponent;
                    aComponent <<= xInterface;
                    pCompVar->PutObject( GetSbUnoObject( DEFINE_CONST_UNICODE("ThisComponent"), aComponent ) );
                }
            }

            pShell = m_pData->m_pObjectShell;
            //pShellLock = m_pData->m_pObjectShellLock;
            EndListening( *pShell );
            m_pData->m_pObjectShell = SfxObjectShellRef();
            //m_pData->m_pObjectShellLock = SfxObjectShellLock();
        }

        SfxObjectShellClose_Impl( 0, (void*) pShell );
    }

    m_pData->m_xCurrent = REFERENCE< XCONTROLLER > ();
    m_pData->m_seqControllers = SEQUENCE< REFERENCE< XCONTROLLER > > () ;

    DELETEZ(m_pData);
}

//________________________________________________________________________________________________________
//  XChild
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::addEventListener( const REFERENCE< XEVENTLISTENER >& aListener )
    throw(::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        return;

    m_pData->m_aInterfaceContainer.addInterface( ::getCppuType((const REFERENCE< XEVENTLISTENER >*)0), aListener );
}

//________________________________________________________________________________________________________
//  XChild
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::removeEventListener( const REFERENCE< XEVENTLISTENER >& aListener )
    throw(::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        return;

    m_pData->m_aInterfaceContainer.removeInterface( ::getCppuType((const REFERENCE< XEVENTLISTENER >*)0), aListener );
}

//________________________________________________________________________________________________________
//  XDOCUMENTINFOSupplier
//________________________________________________________________________________________________________

REFERENCE< XDOCUMENTINFO > SAL_CALL SfxBaseModel::getDocumentInfo() throw(::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( !m_pData->m_xDocumentInfo.is() && m_pData->m_pObjectShell.Is() )
        ((SfxBaseModel*)this)->m_pData->m_xDocumentInfo = new SfxDocumentInfoObject( m_pData->m_pObjectShell ) ;

    return m_pData->m_xDocumentInfo;
}

//________________________________________________________________________________________________________
//  XEVENTLISTENER
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::disposing( const EVENTOBJECT& aObject )
    throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        return;

    REFERENCE< XMODIFYLISTENER >  xMod( aObject.Source, UNOQUERY );
    REFERENCE< XEVENTLISTENER >  xListener( aObject.Source, UNOQUERY );
    REFERENCE< XDOCEVENTLISTENER >  xDocListener( aObject.Source, UNOQUERY );

    if ( xMod.is() )
        m_pData->m_aInterfaceContainer.removeInterface( ::getCppuType((const REFERENCE< XMODIFYLISTENER >*)0), xMod );
    else if ( xListener.is() )
        m_pData->m_aInterfaceContainer.removeInterface( ::getCppuType((const REFERENCE< XEVENTLISTENER >*)0), xListener );
    else if ( xDocListener.is() )
        m_pData->m_aInterfaceContainer.removeInterface( ::getCppuType((const REFERENCE< XDOCEVENTLISTENER >*)0), xListener );
/*
    sal_uInt32 nCount = m_pData->m_seqControllers.getLength();
    for ( sal_uInt32 n = 0; n < nCount; n++ )
    {
        if( m_pData->m_seqControllers.getConstArray()[n] == aObject.Source )
        {
            m_pData->m_seqControllers.getArray()[n] = REFERENCE< XCONTROLLER > () ;
            break;
        }
    }

    if ( m_pData->m_xCurrent.is() && m_pData->m_xCurrent == aObject.Source )
        m_pData->m_xCurrent = REFERENCE< XCONTROLLER > ();
*/
}

//________________________________________________________________________________________________________
//  XMODEL
//________________________________________________________________________________________________________

sal_Bool SAL_CALL SfxBaseModel::attachResource( const   OUSTRING&                   rURL    ,
                                                const   SEQUENCE< PROPERTYVALUE >&  rArgs   )
    throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( rURL.getLength() == 0 && rArgs.getLength() == 1 && rArgs[0].Name.equalsAscii( "SetEmbedded" ) )
    {
        // allows to set a windowless document to EMBEDDED state
        // but _only_ before load() or initNew() methods
        if ( m_pData->m_pObjectShell.Is() && !m_pData->m_pObjectShell->GetMedium() )
        {
            sal_Bool bEmb;
            if ( ( rArgs[0].Value >>= bEmb ) && bEmb )
                m_pData->m_pObjectShell->SetCreateMode_Impl( SFX_CREATE_MODE_EMBEDDED );
        }

        return sal_True;
    }

    if ( m_pData->m_pObjectShell.Is() )
    {
        m_pData->m_sURL = rURL;
        m_pData->m_seqArguments = uno::Sequence< beans::PropertyValue >( rArgs.getLength() );
        sal_Int32 nNewLen = 0;

        for ( sal_Int32 nInd = 0; nInd < rArgs.getLength(); nInd++ )
        {
            if ( rArgs[nInd].Name.equalsAscii( "WinExtent" ) )
            {
                Sequence< sal_Int32 > aSize;
                if ( ( rArgs[nInd].Value >>= aSize ) && aSize.getLength() == 4 )
                {
                    Rectangle aTmpRect( aSize[0], aSize[1], aSize[2], aSize[3] );
                    aTmpRect = OutputDevice::LogicToLogic( aTmpRect, MAP_100TH_MM, m_pData->m_pObjectShell->GetMapUnit() );
                    m_pData->m_pObjectShell->SetVisArea( aTmpRect );
                }
            }
            else if ( rArgs[nInd].Name.equalsAscii( "BreakMacroSignature" ) )
            {
                sal_Bool bBreakMacroSign = sal_False;
                rArgs[nInd].Value >>= bBreakMacroSign;
                m_pData->m_pObjectShell->BreakMacroSign_Impl( bBreakMacroSign );
            }
            else if ( !rArgs[nInd].Name.equalsAscii( "Stream" ) && !rArgs[nInd].Name.equalsAscii( "InputStream" ) )
            {
                // TODO/LATER: all the parameters that are accepted by ItemSet of the DocShell must be ignored here
                m_pData->m_seqArguments[nNewLen++] = rArgs[nInd];
            }
        }

        m_pData->m_seqArguments.realloc( nNewLen );

        if( m_pData->m_pObjectShell->GetMedium() )
        {
            SfxAllItemSet aSet( m_pData->m_pObjectShell->GetPool() );
            TransformParameters( SID_OPENDOC, rArgs, aSet );

            m_pData->m_pObjectShell->GetMedium()->GetItemSet()->Put( aSet );
            SFX_ITEMSET_ARG( &aSet, pItem, SfxStringItem, SID_FILTER_NAME, sal_False );
            if ( pItem )
                m_pData->m_pObjectShell->GetMedium()->SetFilter(
                    m_pData->m_pObjectShell->GetFactory().GetFilterContainer()->GetFilter4FilterName( pItem->GetValue() ) );

            SFX_ITEMSET_ARG( &aSet, pTitleItem, SfxStringItem, SID_DOCINFO_TITLE, sal_False );
            if ( pTitleItem )
            {
                SfxViewFrame* pFrame = SfxViewFrame::GetFirst( m_pData->m_pObjectShell );
                if ( pFrame )
                    pFrame->UpdateTitle();
            }
        }
    }

    return sal_True ;
}

//________________________________________________________________________________________________________
//  XMODEL
//________________________________________________________________________________________________________

OUSTRING SAL_CALL SfxBaseModel::getURL() throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    return m_pData->m_sURL ;
}

//________________________________________________________________________________________________________
//  XMODEL
//________________________________________________________________________________________________________

SEQUENCE< PROPERTYVALUE > SAL_CALL SfxBaseModel::getArgs() throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( m_pData->m_pObjectShell.Is() )
    {
        SEQUENCE< PROPERTYVALUE > seqArgsNew;
        SEQUENCE< PROPERTYVALUE > seqArgsOld;
        SfxAllItemSet aSet( m_pData->m_pObjectShell->GetPool() );

        // we need to know which properties are supported by the transformer
        // hopefully it is a temporary solution, I guess nonconvertable properties
        // should not be supported so then there will be only ItemSet from medium

        TransformItems( SID_OPENDOC, *(m_pData->m_pObjectShell->GetMedium()->GetItemSet()), seqArgsNew );
        TransformParameters( SID_OPENDOC, m_pData->m_seqArguments, aSet );
        TransformItems( SID_OPENDOC, aSet, seqArgsOld );

        sal_Int32 nOrgLength = m_pData->m_seqArguments.getLength();
        sal_Int32 nOldLength = seqArgsOld.getLength();
        sal_Int32 nNewLength = seqArgsNew.getLength();

        // "WinExtent" property should be updated always.
        // We can store it now to overwrite an old value
        // since it is not from ItemSet
        Rectangle aTmpRect = m_pData->m_pObjectShell->GetVisArea( ASPECT_CONTENT );
        aTmpRect = OutputDevice::LogicToLogic( aTmpRect, m_pData->m_pObjectShell->GetMapUnit(), MAP_100TH_MM );

        Sequence< sal_Int32 > aRectSeq(4);
        aRectSeq[0] = aTmpRect.Left();
        aRectSeq[1] = aTmpRect.Top();
        aRectSeq[2] = aTmpRect.Right();
        aRectSeq[3] = aTmpRect.Bottom();

        seqArgsNew.realloc( ++nNewLength );
        seqArgsNew[ nNewLength - 1 ].Name = ::rtl::OUString::createFromAscii( "WinExtent" );
        seqArgsNew[ nNewLength - 1 ].Value <<= aRectSeq;

        if ( m_pData->m_aPreusedFilterName.getLength() )
        {
            seqArgsNew.realloc( ++nNewLength );
            seqArgsNew[ nNewLength - 1 ].Name = ::rtl::OUString::createFromAscii( "PreusedFilterName" );
            seqArgsNew[ nNewLength - 1 ].Value <<= m_pData->m_aPreusedFilterName;
        }

        SfxViewFrame* pFrame = SfxViewFrame::GetFirst( m_pData->m_pObjectShell );
        if ( pFrame )
        {
            SvBorder aBorder = pFrame->GetBorderPixelImpl( pFrame->GetViewShell() );

            Sequence< sal_Int32 > aBorderSeq(4);
            aBorderSeq[0] = aBorder.Left();
            aBorderSeq[1] = aBorder.Top();
            aBorderSeq[2] = aBorder.Right();
            aBorderSeq[3] = aBorder.Bottom();

            seqArgsNew.realloc( ++nNewLength );
            seqArgsNew[ nNewLength - 1 ].Name = ::rtl::OUString::createFromAscii( "DocumentBorder" );
            seqArgsNew[ nNewLength - 1 ].Value <<= aBorderSeq;
        }

        // only the values that are not supported by the ItemSet must be cached here
        uno::Sequence< beans::PropertyValue > aFinalCache;
        sal_Int32 nFinalLength = 0;

        for ( sal_Int32 nOrg = 0; nOrg < nOrgLength; nOrg++ )
        {
             sal_Int32 nOldInd = 0;
            while ( nOldInd < nOldLength )
            {
                if ( m_pData->m_seqArguments[nOrg].Name.equals( seqArgsOld[nOldInd].Name ) )
                    break;
                nOldInd++;
            }

            if ( nOldInd == nOldLength )
            {
                // the entity with this name should be new for seqArgsNew
                // since it is not supported by transformer

                seqArgsNew.realloc( ++nNewLength );
                seqArgsNew[ nNewLength - 1 ] = m_pData->m_seqArguments[nOrg];

                aFinalCache.realloc( ++nFinalLength );
                aFinalCache[ nFinalLength - 1 ] = m_pData->m_seqArguments[nOrg];
            }
        }

        m_pData->m_seqArguments = aFinalCache;

        return seqArgsNew;
    }

    return m_pData->m_seqArguments;
}

//________________________________________________________________________________________________________
//  XMODEL
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::connectController( const REFERENCE< XCONTROLLER >& xController )
    throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    sal_uInt32 nOldCount = m_pData->m_seqControllers.getLength();
    SEQUENCE< REFERENCE< XCONTROLLER > > aNewSeq( nOldCount + 1 );
    for ( sal_uInt32 n = 0; n < nOldCount; n++ )
        aNewSeq.getArray()[n] = m_pData->m_seqControllers.getConstArray()[n];
    aNewSeq.getArray()[nOldCount] = xController;
    m_pData->m_seqControllers = aNewSeq;
}

//________________________________________________________________________________________________________
//  XMODEL
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::disconnectController( const REFERENCE< XCONTROLLER >& xController ) throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    sal_uInt32 nOldCount = m_pData->m_seqControllers.getLength();
    if ( !nOldCount )
        return;

    SEQUENCE< REFERENCE< XCONTROLLER > > aNewSeq( nOldCount - 1 );
    for ( sal_uInt32 nOld = 0, nNew = 0; nOld < nOldCount; ++nOld )
    {
        if ( xController != m_pData->m_seqControllers.getConstArray()[nOld] )
        {
            aNewSeq.getArray()[nNew] = m_pData->m_seqControllers.getConstArray()[nOld];
            ++nNew;
        }
    }

    m_pData->m_seqControllers = aNewSeq;

    if ( xController == m_pData->m_xCurrent )
        m_pData->m_xCurrent = REFERENCE< XCONTROLLER > ();
}

//________________________________________________________________________________________________________
//  XMODEL
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::lockControllers() throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();
    ++m_pData->m_nControllerLockCount ;
}

//________________________________________________________________________________________________________
//  XMODEL
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::unlockControllers() throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();
    --m_pData->m_nControllerLockCount ;
}

//________________________________________________________________________________________________________
//  XMODEL
//________________________________________________________________________________________________________

sal_Bool SAL_CALL SfxBaseModel::hasControllersLocked() throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();
    return ( m_pData->m_nControllerLockCount != 0 ) ;
}

//________________________________________________________________________________________________________
//  XMODEL
//________________________________________________________________________________________________________

REFERENCE< XCONTROLLER > SAL_CALL SfxBaseModel::getCurrentController() throw(::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    // get the last active controller of this model
    if ( m_pData->m_xCurrent.is() )
        return m_pData->m_xCurrent;

    // get the first controller of this model
    return m_pData->m_seqControllers.getLength() ? m_pData->m_seqControllers.getConstArray()[0] : m_pData->m_xCurrent;
}

//________________________________________________________________________________________________________
//  XMODEL
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::setCurrentController( const REFERENCE< XCONTROLLER >& xCurrentController )
        throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    m_pData->m_xCurrent = xCurrentController;
}

//________________________________________________________________________________________________________
//  XMODEL
//________________________________________________________________________________________________________

REFERENCE< XINTERFACE > SAL_CALL SfxBaseModel::getCurrentSelection() throw(::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    REFERENCE< XINTERFACE >     xReturn;
    REFERENCE< XCONTROLLER >    xController =   getCurrentController()      ;

    if ( xController.is() )
    {
        REFERENCE< XSELECTIONSUPPLIER >  xDocView( xController, UNOQUERY );
        if ( xDocView.is() )
        {
            ANY xSel = xDocView->getSelection();

    // automatisch auskommentiert - Wird von UNO III nicht weiter unterstuetzt!
    //      return xSel.getReflection() == XINTERFACE_getReflection()
    //      return xSel.getValueType() == ::getCppuType((const XINTERFACE*)0)
    //              ? *(REFERENCE< XINTERFACE > *) xSel.get() : REFERENCE< XINTERFACE > ();
            xSel >>= xReturn ;
        }
    }

    return xReturn ;
}

//________________________________________________________________________________________________________
//  XModifiable
//________________________________________________________________________________________________________

sal_Bool SAL_CALL SfxBaseModel::isModified() throw(::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    return m_pData->m_pObjectShell.Is() ? m_pData->m_pObjectShell->IsModified() : sal_False;
}

//________________________________________________________________________________________________________
//  XModifiable
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::setModified( sal_Bool bModified )
        throw (::com::sun::star::beans::PropertyVetoException, ::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( m_pData->m_pObjectShell.Is() )
        m_pData->m_pObjectShell->SetModified(bModified);
}

//________________________________________________________________________________________________________
//  XModifiable
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::addModifyListener(const REFERENCE< XMODIFYLISTENER >& xListener) throw( RUNTIMEEXCEPTION )
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        return;

    m_pData->m_aInterfaceContainer.addInterface( ::getCppuType((const REFERENCE< XMODIFYLISTENER >*)0),xListener );
}

//________________________________________________________________________________________________________
//  XModifiable
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::removeModifyListener(const REFERENCE< XMODIFYLISTENER >& xListener) throw( RUNTIMEEXCEPTION )
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        return;

    m_pData->m_aInterfaceContainer.removeInterface( ::getCppuType((const REFERENCE< XMODIFYLISTENER >*)0), xListener );
}

//____________________________________________________________________________________________________
//  XCloseable
//____________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::close( sal_Bool bDeliverOwnership ) throw (CLOSEVETOEXCEPTION, RUNTIMEEXCEPTION)
{
    static ::rtl::OUString MSG_1 = ::rtl::OUString::createFromAscii("Cant close while saving.");

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( !m_pData || m_pData->m_bClosed || m_pData->m_bClosing )
        return;

    uno::Reference< uno::XInterface > xSelfHold( static_cast< ::cppu::OWeakObject* >(this) );
    lang::EventObject             aSource    (static_cast< ::cppu::OWeakObject*>(this));
    ::cppu::OInterfaceContainerHelper* pContainer = m_pData->m_aInterfaceContainer.getContainer( ::getCppuType( ( const uno::Reference< util::XCloseListener >*) NULL ) );
    if (pContainer!=NULL)
    {
        ::cppu::OInterfaceIteratorHelper pIterator(*pContainer);
        while (pIterator.hasMoreElements())
        {
            try
            {
                ((util::XCloseListener*)pIterator.next())->queryClosing( aSource, bDeliverOwnership );
            }
            catch( uno::RuntimeException& )
            {
                pIterator.remove();
            }
        }
    }

    if ( m_pData->m_bSaving )
    {
        if (bDeliverOwnership)
            m_pData->m_bSuicide = sal_True;
        throw CLOSEVETOEXCEPTION(
                MSG_1,
                static_cast< ::com::sun::star::util::XCloseable* >(this));
    }

    // no own objections against closing!
    m_pData->m_bClosing = sal_True;
    if( m_pData->m_pObjectShell.Is() )
        m_pData->m_pObjectShell->Broadcast( SfxSimpleHint(SFX_HINT_DEINITIALIZING) );
    pContainer = m_pData->m_aInterfaceContainer.getContainer( ::getCppuType( ( const uno::Reference< util::XCloseListener >*) NULL ) );
    if (pContainer!=NULL)
    {
        ::cppu::OInterfaceIteratorHelper pCloseIterator(*pContainer);
        while (pCloseIterator.hasMoreElements())
        {
            try
            {
                ((util::XCloseListener*)pCloseIterator.next())->notifyClosing( aSource );
            }
            catch( uno::RuntimeException& )
            {
                pCloseIterator.remove();
            }
        }
    }

    m_pData->m_bClosed = sal_True;
    m_pData->m_bClosing = sal_False;

    dispose();
}

//____________________________________________________________________________________________________
//  XCloseBroadcaster
//____________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::addCloseListener( const REFERENCE< XCLOSELISTENER >& xListener ) throw (RUNTIMEEXCEPTION)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        return;

    m_pData->m_aInterfaceContainer.addInterface( ::getCppuType((const REFERENCE< XCLOSELISTENER >*)0), xListener );
}

//____________________________________________________________________________________________________
//  XCloseBroadcaster
//____________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::removeCloseListener( const REFERENCE< XCLOSELISTENER >& xListener ) throw (RUNTIMEEXCEPTION)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        return;

    m_pData->m_aInterfaceContainer.removeInterface( ::getCppuType((const REFERENCE< XCLOSELISTENER >*)0), xListener );
}

//________________________________________________________________________________________________________
//  XPrintable
//________________________________________________________________________________________________________

SEQUENCE< PROPERTYVALUE > SAL_CALL SfxBaseModel::getPrinter() throw(::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    // Printer beschaffen
    SfxViewFrame *pViewFrm = m_pData->m_pObjectShell.Is() ?
                                SfxViewFrame::GetFirst( m_pData->m_pObjectShell, 0, sal_False ) : 0;
    if ( !pViewFrm )
        return SEQUENCE< PROPERTYVALUE >();
    const SfxPrinter *pPrinter = pViewFrm->GetViewShell()->GetPrinter(sal_True);
    if ( !pPrinter )
        return SEQUENCE< PROPERTYVALUE >();

    // Printer Eigenschaften uebertragen
    SEQUENCE< PROPERTYVALUE > aPrinter(8);

    aPrinter.getArray()[7].Name = DEFINE_CONST_UNICODE( "CanSetPaperSize" );
    aPrinter.getArray()[7].Value <<= ( pPrinter->HasSupport( SUPPORT_SET_PAPERSIZE ) );

    aPrinter.getArray()[6].Name = DEFINE_CONST_UNICODE( "CanSetPaperFormat" );
    aPrinter.getArray()[6].Value <<= ( pPrinter->HasSupport( SUPPORT_SET_PAPER ) );

    aPrinter.getArray()[5].Name = DEFINE_CONST_UNICODE( "CanSetPaperOrientation" );
    aPrinter.getArray()[5].Value <<= ( pPrinter->HasSupport( SUPPORT_SET_ORIENTATION ) );

    aPrinter.getArray()[4].Name = DEFINE_CONST_UNICODE( "IsBusy" );
    aPrinter.getArray()[4].Value <<= ( pPrinter->IsPrinting() );

    aPrinter.getArray()[3].Name = DEFINE_CONST_UNICODE( "PaperSize" );
    SIZE aSize = impl_Size_Object2Struct(pPrinter->GetPaperSize() );
    aPrinter.getArray()[3].Value <<= aSize;

    aPrinter.getArray()[2].Name = DEFINE_CONST_UNICODE( "PaperFormat" );
    PAPERFORMAT eFormat = (PAPERFORMAT)pPrinter->GetPaper();
    aPrinter.getArray()[2].Value <<= eFormat;

    aPrinter.getArray()[1].Name = DEFINE_CONST_UNICODE( "PaperOrientation" );
    PAPERORIENTATION eOrient = (PAPERORIENTATION)pPrinter->GetOrientation();
    aPrinter.getArray()[1].Value <<= eOrient;

    aPrinter.getArray()[0].Name = DEFINE_CONST_UNICODE( "Name" );
    String sStringTemp = pPrinter->GetName() ;
    aPrinter.getArray()[0].Value <<= ::rtl::OUString( sStringTemp );

    return aPrinter;
}

//________________________________________________________________________________________________________
//  XPrintable
//________________________________________________________________________________________________________

void SfxBaseModel::impl_setPrinter(const SEQUENCE< PROPERTYVALUE >& rPrinter,SfxPrinter*& pPrinter,sal_uInt16& nChangeFlags,SfxViewShell*& pViewSh)

{
    // alten Printer beschaffen
    SfxViewFrame *pViewFrm = m_pData->m_pObjectShell.Is() ?
                                SfxViewFrame::GetFirst( m_pData->m_pObjectShell, 0, sal_False ) : 0;
    if ( !pViewFrm )
        return;
    pViewSh = pViewFrm->GetViewShell();
    pPrinter = pViewSh->GetPrinter(sal_True);
    if ( !pPrinter )
        return;

    // new Printer-Name available?
    nChangeFlags = 0;
    sal_Int32 lDummy;
    for ( int n = 0; n < rPrinter.getLength(); ++n )
    {
        // get Property-Value from printer description
        const PROPERTYVALUE &rProp = rPrinter.getConstArray()[n];

        // Name-Property?
        if ( rProp.Name.compareToAscii( "Name" ) == 0 )
        {
            OUSTRING sTemp;
            if ( ( rProp.Value >>= sTemp ) == sal_False )
                throw ILLEGALARGUMENTEXCEPTION();

            String aPrinterName( sTemp ) ;
            pPrinter = new SfxPrinter( pPrinter->GetOptions().Clone(), aPrinterName );
            nChangeFlags = SFX_PRINTER_PRINTER;
            break;
        }
    }

    Size aSetPaperSize( 0, 0);
    PAPERFORMAT nPaperFormat = (PAPERFORMAT) PAPER_USER;
    // other properties
    for ( int i = 0; i < rPrinter.getLength(); ++i )
    {
        // get Property-Value from printer description
        const PROPERTYVALUE &rProp = rPrinter.getConstArray()[i];

        // PaperOrientation-Property?
        if ( rProp.Name.compareToAscii( "PaperOrientation" ) == 0 )
        {
            PAPERORIENTATION eOrient;
            if ( ( rProp.Value >>= eOrient ) == sal_False )
            {
                if ( ( rProp.Value >>= lDummy ) == sal_False )
                    throw ILLEGALARGUMENTEXCEPTION();
                eOrient = ( PAPERORIENTATION ) lDummy;
            }

            pPrinter->SetOrientation( (Orientation) eOrient );
            nChangeFlags |= SFX_PRINTER_CHG_ORIENTATION;
        }

        // PaperFormat-Property?
        if ( rProp.Name.compareToAscii( "PaperFormat" ) == 0 )
        {
            if ( ( rProp.Value >>= nPaperFormat ) == sal_False )
            {
                if ( ( rProp.Value >>= lDummy ) == sal_False )
                    throw ILLEGALARGUMENTEXCEPTION();
                nPaperFormat = ( PAPERFORMAT ) lDummy;
            }

            pPrinter->SetPaper( (Paper) nPaperFormat );
            nChangeFlags |= SFX_PRINTER_CHG_SIZE;
        }

        // PaperSize-Property?
        if ( rProp.Name.compareToAscii( "PaperSize" ) == 0 )
        {
            SIZE aTempSize ;
            if ( ( rProp.Value >>= aTempSize ) == sal_False )
            {
                throw ILLEGALARGUMENTEXCEPTION();
            }
            else
            {
                aSetPaperSize = impl_Size_Struct2Object(aTempSize);
            }
        }
    }

    //os 12.11.98: die PaperSize darf nur gesetzt werden, wenn tatsaechlich
    //PAPER_USER gilt, sonst koennte vom Treiber ein falsches Format gewaehlt werden
    if(nPaperFormat == PAPER_USER && aSetPaperSize.Width())
    {
        //JP 23.09.98 - Bug 56929 - MapMode von 100mm in die am
        //          Device gesetzten umrechnen. Zusaetzlich nur dann
        //          setzen, wenn sie wirklich veraendert wurden.
        aSetPaperSize = pPrinter->LogicToPixel( aSetPaperSize, MAP_100TH_MM );
        if( aSetPaperSize != pPrinter->GetPaperSizePixel() )
        {
            pPrinter->SetPaperSizeUser( pPrinter->PixelToLogic( aSetPaperSize ) );
            nChangeFlags |= SFX_PRINTER_CHG_SIZE;
        }
    }

    // #96772#: wait until printing is done
    SfxPrinter* pDocPrinter = pViewSh->GetPrinter();
    while ( pDocPrinter->IsPrinting() )
        Application::Yield();
}

void SAL_CALL SfxBaseModel::setPrinter(const SEQUENCE< PROPERTYVALUE >& rPrinter)
        throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    SfxViewShell* pViewSh = NULL;
    SfxPrinter* pPrinter = NULL;
    sal_uInt16 nChangeFlags = 0;
    impl_setPrinter(rPrinter,pPrinter,nChangeFlags,pViewSh);
    // set new printer
    if ( pViewSh && pPrinter )
        pViewSh->SetPrinter( pPrinter, nChangeFlags );
}

//________________________________________________________________________________________________________
//  ImplPrintWatch thread for asynchronous printing with moving temp. file to ucb location
//________________________________________________________________________________________________________

/* This implements a thread which will be started to wait for asynchronous
   print jobs to temp. localy files. If they finish we move the temp. files
   to her right locations by using the ucb.
 */
class ImplUCBPrintWatcher : public ::osl::Thread
{
    private:
        /// of course we must know the printer which execute the job
        SfxPrinter* m_pPrinter;
        /// this describes the target location for the printed temp file
        String m_sTargetURL;
        /// it holds the temp file alive, till the print job will finish and remove it from disk automaticly if the object die
        ::utl::TempFile* m_pTempFile;

    public:
        /* initialize this watcher but don't start it */
        ImplUCBPrintWatcher( SfxPrinter* pPrinter, ::utl::TempFile* pTempFile, const String& sTargetURL )
                : m_pPrinter  ( pPrinter   )
                , m_sTargetURL( sTargetURL )
                , m_pTempFile ( pTempFile  )
        {}

        /* waits for finishing of the print job and moves the temp file afterwards
           Note: Starting of the job is done outside this thread!
           But we have to free some of the given ressources on heap!
         */
        void SAL_CALL run()
        {
            /* SAFE { */
            {
                ::vos::OGuard aGuard( Application::GetSolarMutex() );
                while( m_pPrinter->IsPrinting() )
                    Application::Yield();
                m_pPrinter = NULL; // don't delete it! It's borrowed only :-)
            }
            /* } SAFE */

            // lock for further using of our member isn't neccessary - because
            // we truns alone by defenition. Nobody join for us nor use us ...
            ImplUCBPrintWatcher::moveAndDeleteTemp(&m_pTempFile,m_sTargetURL);

            // finishing of this run() method will call onTerminate() automaticly
            // kill this thread there!
        }

        /* nobody wait for this thread. We must kill ourself ...
         */
        void SAL_CALL onTerminated()
        {
            delete this;
        }

        /* static helper to move the temp. file to the target location by using the ucb
           It's static to be useable from outside too. So it's not realy neccessary to start
           the thread, if finishing of the job was detected outside this thread.
           But it must be called without using a corresponding thread for the given parameter!
         */
        static void moveAndDeleteTemp( ::utl::TempFile** ppTempFile, const String& sTargetURL )
        {
            // move the file
            try
            {
                INetURLObject aSplitter(sTargetURL);
                String        sFileName = aSplitter.getName(
                                            INetURLObject::LAST_SEGMENT,
                                            true,
                                            INetURLObject::DECODE_WITH_CHARSET);
                if (aSplitter.removeSegment() && sFileName.Len()>0)
                {
                    ::ucb::Content aSource(
                            ::rtl::OUString((*ppTempFile)->GetURL()),
                            ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XCommandEnvironment >());

                    ::ucb::Content aTarget(
                            ::rtl::OUString(aSplitter.GetMainURL(INetURLObject::NO_DECODE)),
                            ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XCommandEnvironment >());

                    aTarget.transferContent(
                            aSource,
                            ::ucb::InsertOperation_COPY,
                            ::rtl::OUString(sFileName),
                            ::com::sun::star::ucb::NameClash::OVERWRITE);
                }
            }
            catch( ::com::sun::star::ucb::ContentCreationException& ) { DBG_ERROR("content create exception"); }
            catch( ::com::sun::star::ucb::CommandAbortedException&  ) { DBG_ERROR("command abort exception"); }
            catch( ::com::sun::star::uno::RuntimeException&         ) { DBG_ERROR("runtime exception"); }
            catch( ::com::sun::star::uno::Exception&                ) { DBG_ERROR("unknown exception"); }

            // kill the temp file!
            delete *ppTempFile;
            *ppTempFile = NULL;
        }
};

//------------------------------------------------

//________________________________________________________________________________________________________
//  XPrintable
//________________________________________________________________________________________________________
void SAL_CALL SfxBaseModel::print(const SEQUENCE< PROPERTYVALUE >& rOptions)
        throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException)
{
    if( Application::GetSettings().GetMiscSettings().GetDisablePrinting() )
        return;

    // object already disposed?
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    // get view for sfx printing capabilities
    SfxViewFrame *pViewFrm = m_pData->m_pObjectShell.Is() ?
                                SfxViewFrame::GetFirst( m_pData->m_pObjectShell, 0, sal_False ) : 0;
    if ( !pViewFrm )
        return;
    SfxViewShell* pView = pViewFrm->GetViewShell();
    if ( !pView )
        return;

    SfxAllItemSet aArgs( pView->GetPool() );
    sal_Bool bMonitor = sal_False;
    // We need this information at the end of this method, if we start the vcl printer
    // by executing the slot. Because if it is a ucb relevant URL we must wait for
    // finishing the print job and move the temporary local file by using the ucb
    // to the right location. But in case of no file name is given or it is already
    // a local one we can supress this special handling. Because then vcl makes all
    // right for us.
    String sUcbUrl;
    ::utl::TempFile* pUCBPrintTempFile = NULL;

    sal_Bool bWaitUntilEnd = sal_False;
    for ( int n = 0; n < rOptions.getLength(); ++n )
    {
        // get Property-Value from options
        const PROPERTYVALUE &rProp = rOptions.getConstArray()[n];

        // FileName-Property?
        if ( rProp.Name.compareToAscii( "FileName" ) == 0 )
        {
            // unpack th URL and check for a valid and well known protocol
            OUSTRING sTemp;
            if (
                ( rProp.Value.getValueType()!=::getCppuType((const OUSTRING*)0))  ||
                (!(rProp.Value>>=sTemp))
               )
            {
                throw ILLEGALARGUMENTEXCEPTION();
            }

            String        sPath        ;
            String        sURL  (sTemp);
            INetURLObject aCheck(sURL );
            if (aCheck.GetProtocol()==INET_PROT_NOT_VALID)
            {
                // OK - it's not a valid URL. But may it's a simple
                // system path directly. It will be supported for historical
                // reasons. Otherwhise we break to much external code ...
                // We try to convert it to a file URL. If its possible
                // we put the system path to the item set and let vcl work with it.
                // No ucb or thread will be neccessary then. In case it couldnt be
                // converted its not an URL nor a system path. Then we can't accept
                // this parameter and have to throw an exception.
                ::rtl::OUString sSystemPath(sTemp);
                ::rtl::OUString sFileURL          ;
                if (::osl::FileBase::getFileURLFromSystemPath(sSystemPath,sFileURL)!=::osl::FileBase::E_None)
                    throw ILLEGALARGUMENTEXCEPTION();
                aArgs.Put( SfxStringItem(SID_FILE_NAME,sTemp) );
            }
            else
            // It's a valid URL. but now we must know, if it is a local one or not.
            // It's a question of using ucb or not!
            if (::utl::LocalFileHelper::ConvertURLToSystemPath(sURL,sPath))
            {
                // it's a local file, we can use vcl without special handling
                // And we have to use the system notation of the incoming URL.
                // But it into the descriptor and let the slot be executed at
                // the end of this method.
                aArgs.Put( SfxStringItem(SID_FILE_NAME,sPath) );
            }
            else
            {
                // it's an ucb target. So we must use a temp. file for vcl
                // and move it after printing by using the ucb.
                // Create a temp file on the heap (because it must delete the
                // real file on disk automaticly if it die - bt we have to share it with
                // some other sources ... e.g. the ImplUCBPrintWatcher).
                // And we put the name of this temp file to the descriptor instead
                // of the URL. The URL we save for later using seperatly.
                // Execution of the print job will be done later by executing
                // a slot ...
                pUCBPrintTempFile = new ::utl::TempFile();
                pUCBPrintTempFile->EnableKillingFile();
                aArgs.Put( SfxStringItem(SID_FILE_NAME,pUCBPrintTempFile->GetFileName()) );
                sUcbUrl = sURL;
            }
        }

        // CopyCount-Property
        else if ( rProp.Name.compareToAscii( "CopyCount" ) == 0 )
        {
            sal_Int32 nCopies = 0;
            if ( ( rProp.Value >>= nCopies ) == sal_False )
                throw ILLEGALARGUMENTEXCEPTION();
            aArgs.Put( SfxInt16Item( SID_PRINT_COPIES, (USHORT) nCopies ) );
        }

        // Collate-Property
        else if ( rProp.Name.compareToAscii( "Collate" ) == 0 )
        {
            sal_Bool bTemp ;
            if ( rProp.Value >>= bTemp )
                aArgs.Put( SfxBoolItem( SID_PRINT_COLLATE, bTemp ) );
            else
                throw ILLEGALARGUMENTEXCEPTION();
        }

        // Sort-Property
        else if ( rProp.Name.compareToAscii( "Sort" ) == 0 )
        {
            sal_Bool bTemp ;
            if( rProp.Value >>= bTemp )
                aArgs.Put( SfxBoolItem( SID_PRINT_SORT, bTemp ) );
            else
                throw ILLEGALARGUMENTEXCEPTION();
        }

        // Pages-Property
        else if ( rProp.Name.compareToAscii( "Pages" ) == 0 )
        {
            OUSTRING sTemp;
            if( rProp.Value >>= sTemp )
                aArgs.Put( SfxStringItem( SID_PRINT_PAGES, String( sTemp ) ) );
            else
                throw ILLEGALARGUMENTEXCEPTION();
        }

        // MonitorVisible
        else if ( rProp.Name.compareToAscii( "MonitorVisible" ) == 0 )
        {
            if( !(rProp.Value >>= bMonitor) )
                throw ILLEGALARGUMENTEXCEPTION();
        }

        // MonitorVisible
        else if ( rProp.Name.compareToAscii( "Wait" ) == 0 )
        {
            if ( !(rProp.Value >>= bWaitUntilEnd) )
                throw ILLEGALARGUMENTEXCEPTION();
        }
    }

    // Execute the print request every time.
    // It doesn'tmatter if it is a real printer used or we print to a local file
    // nor if we print to a temp file and move it afterwards by using the ucb.
    // That will be handled later. see pUCBPrintFile below!
    aArgs.Put( SfxBoolItem( SID_SILENT, !bMonitor ) );
    if ( bWaitUntilEnd )
        aArgs.Put( SfxBoolItem( SID_ASYNCHRON, sal_False ) );
    SfxRequest aReq( SID_PRINTDOC, SFX_CALLMODE_SYNCHRON | SFX_CALLMODE_API, pView->GetPool() );
    aReq.SetArgs( aArgs );
    pView->ExecuteSlot( aReq );

    // Ok - may be execution before has finished (or started!) printing.
    // And may it was a printing to a file.
    // Now we have to check if we can move the file (if neccessary) via ucb to his right location.
    // Cases:
    //  a) printing finished                        => move the file directly and forget the watcher thread
    //  b) printing is asynchron and runs currently => start watcher thread and exit this method
    //                                                 This thread make all neccessary things by itself.
    if (pUCBPrintTempFile!=NULL)
    {
        // a)
        SfxPrinter* pPrinter = pView->GetPrinter();
        if ( ! pPrinter->IsPrinting() )
            ImplUCBPrintWatcher::moveAndDeleteTemp(&pUCBPrintTempFile,sUcbUrl);
        // b)
        else
        {
            // Note: we create(d) some ressource on the heap. (thread and tep file)
            // They will be delected by the thread automaticly if he finish his run() method.
            ImplUCBPrintWatcher* pWatcher = new ImplUCBPrintWatcher( pPrinter, pUCBPrintTempFile, sUcbUrl );
            pWatcher->create();
        }
    }
}

//________________________________________________________________________________________________________
//  XStorable
//________________________________________________________________________________________________________

sal_Bool SAL_CALL SfxBaseModel::hasLocation() throw(::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    return m_pData->m_pObjectShell.Is() ? m_pData->m_pObjectShell->HasName() : sal_False;
}

//________________________________________________________________________________________________________
//  XStorable
//________________________________________________________________________________________________________

OUSTRING SAL_CALL SfxBaseModel::getLocation() throw(::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    return m_pData->m_pObjectShell.Is() ? OUSTRING(m_pData->m_pObjectShell->GetMedium()->GetName()) : m_pData->m_sURL;
}

//________________________________________________________________________________________________________
//  XStorable
//________________________________________________________________________________________________________

sal_Bool SAL_CALL SfxBaseModel::isReadonly() throw(::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    return m_pData->m_pObjectShell.Is() ? m_pData->m_pObjectShell->IsReadOnly() : sal_True;
}

//________________________________________________________________________________________________________
//  XStorable2
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::storeSelf( const    SEQUENCE< PROPERTYVALUE >&  aSeqArgs )
        throw ( ::com::sun::star::lang::IllegalArgumentException,
                ::com::sun::star::io::IOException,
                ::com::sun::star::uno::RuntimeException )
{
    RTL_LOGFILE_PRODUCT_CONTEXT( aPerfLog, "PERFORMANCE - SfxBaseModel::storeSelf" );

    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( m_pData->m_pObjectShell.Is() )
    {
        SfxSaveGuard aSaveGuard(this, m_pData, sal_False);

        for ( sal_Int32 nInd = 0; nInd < aSeqArgs.getLength(); nInd++ )
        {
            // check that only acceptable parameters are provided here
            if ( !aSeqArgs[nInd].Name.equals( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "VersionComment" ) ) )
              && !aSeqArgs[nInd].Name.equals( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Author" ) ) )
              && !aSeqArgs[nInd].Name.equals( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "InteractionHandler" ) ) )
              && !aSeqArgs[nInd].Name.equals( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "StatusIndicator" ) ) ) )
                throw lang::IllegalArgumentException();
        }

        SfxAllItemSet *pParams = new SfxAllItemSet( SFX_APP()->GetPool() );
        TransformParameters( SID_SAVEDOC, aSeqArgs, *pParams );

        SFX_APP()->NotifyEvent( SfxEventHint( SFX_EVENT_SAVEDOC, m_pData->m_pObjectShell ) );

        sal_Bool bRet = sal_False;

        // TODO/LATER: let the embedded case of saving be handled more careful
        if ( m_pData->m_pObjectShell->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
        {
            // If this is an embedded object that has no URL based location it should be stored to own storage.
            // An embedded object can have a location based on URL in case it is a link, then it should be
            // stored in normal way.
            if ( !hasLocation() || getLocation().compareToAscii( "private:", 8 ) == 0 )
            {
                // actually in this very rare case only UI parameters have sence
                // TODO/LATER: should be done later, after integration of sb19
                if ( bRet = m_pData->m_pObjectShell->DoSave() )
                    bRet = m_pData->m_pObjectShell->DoSaveCompleted();
            }
            else
            {
                bRet = m_pData->m_pObjectShell->Save_Impl( pParams );
            }
        }
        else
            bRet = m_pData->m_pObjectShell->Save_Impl( pParams );

        DELETEZ( pParams );

        sal_uInt32 nErrCode = m_pData->m_pObjectShell->GetError() ? m_pData->m_pObjectShell->GetError()
                                                                    : ERRCODE_IO_CANTWRITE;
        m_pData->m_pObjectShell->ResetError();

        SFX_APP()->NotifyEvent( SfxEventHint( SFX_EVENT_SAVEFINISHED, m_pData->m_pObjectShell ) );

        if ( bRet )
        {
            m_pData->m_aPreusedFilterName = GetMediumFilterName_Impl();

            SFX_APP()->NotifyEvent( SfxEventHint( SFX_EVENT_SAVEDOCDONE, m_pData->m_pObjectShell ) );
        }
        else
        {
            SFX_APP()->NotifyEvent( SfxEventHint( SFX_EVENT_SAVEDOCFAILED, m_pData->m_pObjectShell ) );

            throw task::ErrorCodeIOException( ::rtl::OUString(), uno::Reference< uno::XInterface >(), nErrCode );
        }
    }

}

//________________________________________________________________________________________________________
//  XStorable
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::store() throw (::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException)
{
    storeSelf( uno::Sequence< beans::PropertyValue >() );
}

//________________________________________________________________________________________________________
//  XStorable
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::storeAsURL( const   OUSTRING&                   rURL    ,
                                        const   SEQUENCE< PROPERTYVALUE >&  rArgs   )
        throw (::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException)
{
    RTL_LOGFILE_PRODUCT_CONTEXT( aPerfLog, "PERFORMANCE - SfxBaseModel::storeAsURL" );

    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( m_pData->m_pObjectShell.Is() )
    {
        SfxSaveGuard aSaveGuard(this, m_pData, sal_False);

        impl_store( rURL, rArgs, sal_False );

        SEQUENCE< PROPERTYVALUE > aSequence ;
        TransformItems( SID_OPENDOC, *m_pData->m_pObjectShell->GetMedium()->GetItemSet(), aSequence );
        attachResource( rURL, aSequence );
    }
}

//________________________________________________________________________________________________________
//  XStorable
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::storeToURL( const   OUSTRING&                   rURL    ,
                                        const   SEQUENCE< PROPERTYVALUE >&  rArgs   )
        throw (::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( m_pData->m_pObjectShell.Is() )
    {
        SfxSaveGuard aSaveGuard(this, m_pData, sal_False);
        impl_store( rURL, rArgs, sal_True );
    }
}

//________________________________________________________________________________________________________
// XLoadable
//________________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::initNew()
        throw (::com::sun::star::frame::DoubleInitializationException,
               ::com::sun::star::io::IOException,
               ::com::sun::star::uno::RuntimeException,
               ::com::sun::star::uno::Exception)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    // the object shell should exist always
    DBG_ASSERT( m_pData->m_pObjectShell.Is(), "Model is useless without an ObjectShell" );
    if ( m_pData->m_pObjectShell.Is() )
    {
        if( m_pData->m_pObjectShell->GetMedium() )
            throw DOUBLEINITIALIZATIONEXCEPTION();

        sal_Bool bRes = m_pData->m_pObjectShell->DoInitNew( NULL );
        sal_uInt32 nErrCode = m_pData->m_pObjectShell->GetError() ?
                                    m_pData->m_pObjectShell->GetError() : ERRCODE_IO_CANTCREATE;
        m_pData->m_pObjectShell->ResetError();

        if ( !bRes )
            throw task::ErrorCodeIOException( ::rtl::OUString(), uno::Reference< uno::XInterface >(), nErrCode );
    }
}

//________________________________________________________________________________________________________
// XLoadable
//________________________________________________________________________________________________________
extern void SetTemplate_Impl( const String&, const String&, SfxObjectShell* );

#include <unotools/ucbstreamhelper.hxx>

void SAL_CALL SfxBaseModel::load(   const SEQUENCE< PROPERTYVALUE >& seqArguments )
        throw (::com::sun::star::frame::DoubleInitializationException,
               ::com::sun::star::io::IOException,
               ::com::sun::star::uno::RuntimeException,
               ::com::sun::star::uno::Exception)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    // the object shell should exist always
    DBG_ASSERT( m_pData->m_pObjectShell.Is(), "Model is useless without an ObjectShell" );

    if ( m_pData->m_pObjectShell.Is() )
    {
        if( m_pData->m_pObjectShell->GetMedium() )
            // if a Medium is present, the document is already initialized
            throw DOUBLEINITIALIZATIONEXCEPTION();

        SfxAllItemSet *pParams = new SfxAllItemSet( SFX_APP()->GetPool() );
        TransformParameters( SID_OPENDOC, seqArguments, *pParams );

        rtl::OUString aFilterName;
        SFX_ITEMSET_ARG( pParams, pFilterNameItem, SfxStringItem, SID_FILTER_NAME, sal_False );
        if( pFilterNameItem )
            aFilterName = pFilterNameItem->GetValue();
        const SfxFilter* pFilter = SFX_APP()->GetFilterMatcher().GetFilter4FilterName( aFilterName );
        if( !m_pData->m_pObjectShell->GetFactory().GetFilterContainer()->GetFilter4FilterName( aFilterName ) )
        {
            // filtername is not valid
            delete pParams;
            throw ILLEGALARGUMENTIOEXCEPTION();
        }

        sal_Bool bSalvage = sal_False;
        SFX_ITEMSET_ARG( pParams, pSalvageItem, SfxStringItem, SID_DOC_SALVAGE, sal_False );
        if( pSalvageItem )
        {
            bSalvage = sal_True;
            if ( pSalvageItem->GetValue().Len() )
            {
                // if an URL is provided in SalvageItem that means that the FileName refers to a temporary file
                // that must be copied here

                SFX_ITEMSET_ARG( pParams, pFileNameItem, SfxStringItem, SID_FILE_NAME, FALSE );
                ::rtl::OUString aNewTempFileURL = SfxMedium::CreateTempCopyWithExt( pFileNameItem->GetValue() );
                if ( aNewTempFileURL.getLength() )
                {
                    pParams->Put( SfxStringItem( SID_FILE_NAME, aNewTempFileURL ) );
                    pParams->ClearItem( SID_INPUTSTREAM );
                    pParams->ClearItem( SID_STREAM );
                    pParams->ClearItem( SID_CONTENT );
                }
                else
                {
                    OSL_ENSURE( sal_False, "Can not create a new temporary file for crash recovery!\n" );
                }
            }
        }

        BOOL bReadOnly = FALSE;
        SFX_ITEMSET_ARG( pParams, pReadOnlyItem, SfxBoolItem, SID_DOC_READONLY, FALSE );
        if ( pReadOnlyItem && pReadOnlyItem->GetValue() )
            bReadOnly = TRUE;

        SFX_ITEMSET_ARG( pParams, pFileNameItem, SfxStringItem, SID_FILE_NAME, FALSE );
        SfxMedium* pMedium = new SfxMedium( pFileNameItem->GetValue(), bReadOnly ? SFX_STREAM_READONLY : SFX_STREAM_READWRITE, FALSE, pFilter, pParams );

        // allow to use an interactionhandler (if there is one)
        pMedium->UseInteractionHandler( TRUE );

        // !TODO: currently not working
        //SFX_ITEMSET_ARG( pParams, pFrameItem, SfxFrameItem, SID_DOCFRAME, FALSE );
        //if( pFrameItem && pFrameItem->GetFrame() )
        //{
        //  SfxFrame* pFrame = pFrameItem->GetFrame();
        //  pMedium->SetLoadTargetFrame( pFrame );
        //}

        SFX_ITEMSET_ARG( pParams, pTemplateItem, SfxBoolItem, SID_TEMPLATE, sal_False);
        BOOL bTemplate = pTemplateItem && pTemplateItem->GetValue();
        m_pData->m_pObjectShell->SetActivateEvent_Impl( bTemplate ? SFX_EVENT_CREATEDOC : SFX_EVENT_OPENDOC );

        // load document
        sal_uInt32 nError = ERRCODE_NONE;
        if ( !m_pData->m_pObjectShell->DoLoad(pMedium) )
            nError=ERRCODE_IO_GENERAL;

        ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler > xHandler = pMedium->GetInteractionHandler();
        if( m_pData->m_pObjectShell->GetErrorCode() )
        {
            nError = m_pData->m_pObjectShell->GetErrorCode();
            if ( nError == ERRCODE_IO_BROKENPACKAGE && xHandler.is() )
            {
                ::rtl::OUString aDocName = pMedium->GetURLObject().getName( INetURLObject::LAST_SEGMENT, true, INetURLObject::DECODE_WITH_CHARSET );
                SFX_ITEMSET_ARG( pParams, pRepairItem, SfxBoolItem, SID_REPAIRPACKAGE, FALSE );
                if ( !pRepairItem || !pRepairItem->GetValue() )
                {
                    RequestPackageReparation* pRequest = new RequestPackageReparation( aDocName );
                    com::sun::star::uno::Reference< com::sun::star::task::XInteractionRequest > xRequest ( pRequest );
                    xHandler->handle( xRequest );
                    if( pRequest->isApproved() )
                    {
                        // broken package: try second loading and allow repair
                        pMedium->GetItemSet()->Put( SfxBoolItem( SID_REPAIRPACKAGE, sal_True ) );
                        pMedium->GetItemSet()->Put( SfxBoolItem( SID_TEMPLATE, sal_True ) );
                        pMedium->GetItemSet()->Put( SfxStringItem( SID_DOCINFO_TITLE, aDocName ) );

                        // the error must be reset and the storage must be reopened in new mode
                        pMedium->ResetError();
                        pMedium->CloseStorage();
                        m_pData->m_pObjectShell->PrepareSecondTryLoad_Impl();
                        if ( !m_pData->m_pObjectShell->DoLoad(pMedium) )
                            nError=ERRCODE_IO_GENERAL;
                        nError = m_pData->m_pObjectShell->GetErrorCode();
                    }
                }

                if ( nError == ERRCODE_IO_BROKENPACKAGE )
                {
                    // repair either not allowed or not successful
                    NotifyBrokenPackage* pNotifyRequest = new NotifyBrokenPackage( aDocName );
                    com::sun::star::uno::Reference< com::sun::star::task::XInteractionRequest > xRequest ( pNotifyRequest );
                       xHandler->handle( xRequest );
                }
            }
        }

        if( m_pData->m_pObjectShell->IsAbortingImport() )
            nError = ERRCODE_ABORT;

        if ( !nError )
        {
            if( bTemplate )
            {
                //TODO/LATER: make sure that templates always are XML docs!
                // document is created from a template
                String aName( pMedium->GetName() );
                SFX_ITEMSET_ARG( pMedium->GetItemSet(), pTemplNamItem, SfxStringItem, SID_TEMPLATE_NAME, sal_False);
                String aTemplateName;
                if ( pTemplNamItem )
                    aTemplateName = pTemplNamItem->GetValue();
                else
                {
                    // !TODO/LATER: what's this?!
                    // Interaktiv ( DClick, Contextmenu ) kommt kein Langname mit
                    aTemplateName = m_pData->m_pObjectShell->GetDocInfo().GetTitle();
                    if ( !aTemplateName.Len() )
                    {
                        INetURLObject aURL( aName );
                        aURL.CutExtension();
                        aTemplateName = aURL.getName( INetURLObject::LAST_SEGMENT, true, INetURLObject::DECODE_WITH_CHARSET );
                    }
                }

                // set medium to noname
                pMedium->SetName( String(), sal_True );
                pMedium->Init_Impl();

                // drop resource
                m_pData->m_pObjectShell->SetNoName();
                m_pData->m_pObjectShell->InvalidateName();

                if( m_pData->m_pObjectShell->IsPackageStorageFormat_Impl( *pMedium ) )
                {
                    // untitled document must be based on temporary storage
                    // the medium should not dispose the storage in this case
                    uno::Reference < embed::XStorage > xTmpStor = ::comphelper::OStorageHelper::GetTemporaryStorage();
                    m_pData->m_pObjectShell->GetStorage()->copyToStorage( xTmpStor );

                    // the medium should disconnect from the original location
                    // the storage should not be disposed since the document is still
                    // based on it, but in DoSaveCompleted it will be disposed
                    pMedium->CanDisposeStorage_Impl( sal_False );
                    pMedium->Close();

                    // setting the new storage the medium will be based on
                    pMedium->SetStorage_Impl( xTmpStor );

                    m_pData->m_pObjectShell->ForgetMedium();
                    if( !m_pData->m_pObjectShell->DoSaveCompleted( pMedium ) )
                        nError = ERRCODE_IO_GENERAL;
                    else
                    {
                        if ( !bSalvage )
                        {
                            // some further initializations for templates
                            SetTemplate_Impl( aName, aTemplateName, m_pData->m_pObjectShell );
                        }

                        // the medium should not dispose the storage, DoSaveCompleted() has let it to do so
                        pMedium->CanDisposeStorage_Impl( sal_False );
                    }
                }
                else
                {
                    // some further initializations for templates
                    SetTemplate_Impl( aName, aTemplateName, m_pData->m_pObjectShell );
                    pMedium->CreateTempFile();
                }

                // templates are never readonly
                pMedium->GetItemSet()->ClearItem( SID_DOC_READONLY );
                pMedium->SetOpenMode( SFX_STREAM_READWRITE, sal_True, sal_True );

                // notifications about possible changes in readonly state and document info
                m_pData->m_pObjectShell->Broadcast( SfxSimpleHint(SFX_HINT_MODECHANGED) );
                m_pData->m_pObjectShell->Broadcast( SfxDocumentInfoHint( &m_pData->m_pObjectShell->GetDocInfo() ) );

                // created untitled document can't be modified
                m_pData->m_pObjectShell->SetModified( sal_False );
            }

            if( bSalvage )
            {
                // file recovery: restore original filter
                SFX_ITEMSET_ARG( pMedium->GetItemSet(), pFilterItem, SfxStringItem, SID_FILTER_NAME, sal_False );
                SfxFilterMatcher& rMatcher = SFX_APP()->GetFilterMatcher();
                const SfxFilter* pSetFilter = rMatcher.GetFilter4FilterName( pFilterItem->GetValue() );
                pMedium->SetFilter( pSetFilter );
                m_pData->m_pObjectShell->SetModified(sal_True);
            }

            // TODO/LATER: may be the mode should be retrieved from outside and the preused filter should not be set
            if ( m_pData->m_pObjectShell->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
            {
                SFX_ITEMSET_ARG( pMedium->GetItemSet(), pFilterItem, SfxStringItem, SID_FILTER_NAME, sal_False );
                if ( pFilterItem )
                    m_pData->m_aPreusedFilterName = pFilterItem->GetValue();
            }
        }
        else
            m_pData->m_pObjectShell->ResetError();

        if ( !nError )
            nError = pMedium->GetError();

        if ( nError )
        {
            BOOL bSilent = FALSE;
            SFX_ITEMSET_ARG( pMedium->GetItemSet(), pSilentItem, SfxBoolItem, SID_SILENT, sal_False);
            if( pSilentItem )
                bSilent = pSilentItem->GetValue();

              BOOL bWarning = ((nError & ERRCODE_WARNING_MASK) == ERRCODE_WARNING_MASK);
            if ( nError != ERRCODE_IO_BROKENPACKAGE && !bSilent )
            {
                // broken package was handled already
                if ( xHandler.is() )
                {
                    ::com::sun::star::uno::Any aInteraction;
                    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation > > lContinuations(2);
                    ::framework::ContinuationAbort* pAbort = new ::framework::ContinuationAbort();
                    ::framework::ContinuationApprove* pApprove = new ::framework::ContinuationApprove();
                    lContinuations[0] = ::com::sun::star::uno::Reference<
                            ::com::sun::star::task::XInteractionContinuation >(static_cast<
                            ::com::sun::star::task::XInteractionContinuation* >(pAbort),
                            UNOQUERY);
                    lContinuations[1] = ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation >(static_cast< ::com::sun::star::task::XInteractionContinuation* >(pApprove), UNOQUERY);

                    ::com::sun::star::task::ErrorCodeRequest aErrorCode;
                    aErrorCode.ErrCode = nError;
                    aInteraction <<= aErrorCode;

                    ::framework::InteractionRequest* pRequest = new ::framework::InteractionRequest(aInteraction,lContinuations);
                    ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionRequest > xRequest(static_cast< ::com::sun::star::task::XInteractionRequest* >(pRequest), UNOQUERY);

                    xHandler->handle(xRequest);
                    if ( pAbort->isSelected() && !bWarning )
                        // abort loading (except for warnings)
                        nError = ERRCODE_IO_ABORT;
                }
            }

            if ( m_pData->m_pObjectShell->GetMedium() != pMedium )
            {
                // for whatever reason document now has another medium
                DBG_ERROR("Document has rejected the medium?!");
                delete pMedium;
            }

            if ( !bWarning )    // #i30711# don't abort loading if it's only a warning
            {
                throw task::ErrorCodeIOException( ::rtl::OUString(),
                                                    uno::Reference< uno::XInterface >(),
                                                    nError ? nError : ERRCODE_IO_CANTREAD );
            }
        }
    }
}

//________________________________________________________________________________________________________
// XTransferable
//________________________________________________________________________________________________________

ANY SAL_CALL SfxBaseModel::getTransferData( const DATAFLAVOR& aFlavor )
        throw (::com::sun::star::datatransfer::UnsupportedFlavorException,
               ::com::sun::star::io::IOException,
               ::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    ANY aAny;

    if ( m_pData->m_pObjectShell.Is() )
    {
        if ( aFlavor.MimeType.equalsAscii( "application/x-openoffice-objectdescriptor-xml;windows_formatname=\"Star Object Descriptor (XML)\"" ) )
        {
            if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            {
                TransferableObjectDescriptor aDesc;

                aDesc.maClassName = m_pData->m_pObjectShell->GetClassName();
                aDesc.maTypeName = aFlavor.HumanPresentableName;

                // TODO/LATER: ViewAspect needs to be sal_Int64
                aDesc.mnViewAspect = sal::static_int_cast< sal_uInt16 >( embed::Aspects::MSOLE_CONTENT );

                //TODO/LATER: status needs to become sal_Int64
                aDesc.mnOle2Misc = m_pData->m_pObjectShell->GetMiscStatus();
                Size aSize = m_pData->m_pObjectShell->GetVisArea().GetSize();

                MapUnit aMapUnit = m_pData->m_pObjectShell->GetMapUnit();
                aDesc.maSize = OutputDevice::LogicToLogic( aSize, aMapUnit, MAP_100TH_MM );
                aDesc.maDragStartPos = Point();
                aDesc.maDisplayName = String();
                aDesc.mbCanLink = FALSE;

                SvMemoryStream aMemStm( 1024, 1024 );
                aMemStm << aDesc;
                aAny <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( aMemStm.GetData() ), aMemStm.Tell() );
            }
            else
                throw UNSUPPORTEDFLAVOREXCEPTION();
        }
        else if ( aFlavor.MimeType.equalsAscii( "application/x-openoffice-embed-source;windows_formatname=\"Star EMBS\"" ) )
        {
            if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            {
                try
                {
                    utl::TempFile aTmp;
                    aTmp.EnableKillingFile( TRUE );
                    storeToURL( aTmp.GetURL(), SEQUENCE < PROPERTYVALUE >() );
                    SvStream* pStream = aTmp.GetStream( STREAM_READ );
                    const sal_uInt32 nLen = pStream->Seek( STREAM_SEEK_TO_END );
                    ::com::sun::star::uno::Sequence< sal_Int8 > aSeq( nLen );
                    pStream->Seek( STREAM_SEEK_TO_BEGIN );
                    pStream->Read( aSeq.getArray(),  nLen );
                    delete pStream;
                    if( aSeq.getLength() )
                        aAny <<= aSeq;
                }
                catch ( uno::Exception& )
                {
                }
            }
            else
                throw UNSUPPORTEDFLAVOREXCEPTION();
        }
        else if ( aFlavor.MimeType.equalsAscii( "application/x-openoffice-gdimetafile;windows_formatname=\"GDIMetaFile\"" ) )
        {
            if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            {
                GDIMetaFile* pMetaFile = m_pData->m_pObjectShell->GetPreviewMetaFile( sal_True );

                if ( pMetaFile )
                {
                    SvMemoryStream aMemStm( 65535, 65535 );
                    aMemStm.SetVersion( SOFFICE_FILEFORMAT_CURRENT );

                    pMetaFile->Write( aMemStm );
                    delete pMetaFile;
                    aAny <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( aMemStm.GetData() ),
                                                    aMemStm.Seek( STREAM_SEEK_TO_END ) );
                }
            }
            else
                throw UNSUPPORTEDFLAVOREXCEPTION();
        }
        else if ( aFlavor.MimeType.equalsAscii( "application/x-openoffice-highcontrast-gdimetafile;windows_formatname=\"GDIMetaFile\"" ) )
        {
            if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            {
                GDIMetaFile* pMetaFile = m_pData->m_pObjectShell->CreatePreviewMetaFile_Impl( sal_True, sal_True );

                if ( pMetaFile )
                {
                    SvMemoryStream aMemStm( 65535, 65535 );
                    aMemStm.SetVersion( SOFFICE_FILEFORMAT_CURRENT );

                    pMetaFile->Write( aMemStm );
                    delete pMetaFile;
                    aAny <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( aMemStm.GetData() ),
                                                    aMemStm.Seek( STREAM_SEEK_TO_END ) );
                }
            }
            else
                throw UNSUPPORTEDFLAVOREXCEPTION();
        }
        else if ( aFlavor.MimeType.equalsAscii( "application/x-openoffice-emf;windows_formatname=\"Image EMF\"" ) )
        {
            if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            {
                GDIMetaFile* pMetaFile = m_pData->m_pObjectShell->GetPreviewMetaFile( sal_True );

                if ( pMetaFile )
                {
                    SvMemoryStream* pStream = GraphicHelper::getFormatStrFromGDI_Impl( pMetaFile, CVT_EMF );
                    delete pMetaFile;
                    if ( pStream )
                    {
                        pStream->SetVersion( SOFFICE_FILEFORMAT_CURRENT );
                        aAny <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( pStream->GetData() ),
                                                        pStream->Seek( STREAM_SEEK_TO_END ) );
                        delete pStream;
                    }
                }
            }
            else if ( GraphicHelper::supportsMetaFileHandle_Impl()
              && aFlavor.DataType == getCppuType( (const sal_uInt64*) 0 ) )
            {
                GDIMetaFile* pMetaFile = m_pData->m_pObjectShell->GetPreviewMetaFile( sal_True );

                if ( pMetaFile )
                {
                    aAny <<= reinterpret_cast< const sal_uInt64 >( GraphicHelper::getEnhMetaFileFromGDI_Impl( pMetaFile ) );
                    delete pMetaFile;
                }
            }
            else
                throw UNSUPPORTEDFLAVOREXCEPTION();
        }
        else if ( aFlavor.MimeType.equalsAscii( "application/x-openoffice-wmf;windows_formatname=\"Image WMF\"" ) )
        {
            if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            {
                GDIMetaFile* pMetaFile = m_pData->m_pObjectShell->GetPreviewMetaFile( sal_True );

                if ( pMetaFile )
                {
                    SvMemoryStream* pStream = GraphicHelper::getFormatStrFromGDI_Impl( pMetaFile, CVT_WMF );
                    delete pMetaFile;

                    if ( pStream )
                    {
                        pStream->SetVersion( SOFFICE_FILEFORMAT_CURRENT );
                        aAny <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( pStream->GetData() ),
                                                        pStream->Seek( STREAM_SEEK_TO_END ) );
                        delete pStream;
                    }
                }
            }
            else if ( GraphicHelper::supportsMetaFileHandle_Impl()
              && aFlavor.DataType == getCppuType( (const sal_uInt64*) 0 ) )
            {
                // means HGLOBAL handler to memory storage containing METAFILEPICT structure

                GDIMetaFile* pMetaFile = m_pData->m_pObjectShell->GetPreviewMetaFile( sal_True );

                if ( pMetaFile )
                {
                    Size aMetaSize = pMetaFile->GetPrefSize();
                    aAny <<= reinterpret_cast< const sal_uInt64 >( GraphicHelper::getWinMetaFileFromGDI_Impl( pMetaFile, aMetaSize ) );

                    delete pMetaFile;
                }
            }
            else
                throw UNSUPPORTEDFLAVOREXCEPTION();
        }
        else if ( aFlavor.MimeType.equalsAscii( "application/x-openoffice-bitmap;windows_formatname=\"Bitmap\"" ) )
        {
            if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            {
                GDIMetaFile* pMetaFile = m_pData->m_pObjectShell->GetPreviewMetaFile( sal_True );

                if ( pMetaFile )
                {
                    SvMemoryStream* pStream = GraphicHelper::getFormatStrFromGDI_Impl( pMetaFile, CVT_BMP );
                    delete pMetaFile;

                    if ( pStream )
                    {
                        pStream->SetVersion( SOFFICE_FILEFORMAT_CURRENT );
                        aAny <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( pStream->GetData() ),
                                                        pStream->Seek( STREAM_SEEK_TO_END ) );
                        delete pStream;
                    }
                }
            }
            else
                throw UNSUPPORTEDFLAVOREXCEPTION();
        }
        else if ( aFlavor.MimeType.equalsAscii( "image/png" ) )
        {
            if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            {
                GDIMetaFile* pMetaFile = m_pData->m_pObjectShell->GetPreviewMetaFile( sal_True );

                if ( pMetaFile )
                {
                    SvMemoryStream* pStream = GraphicHelper::getFormatStrFromGDI_Impl( pMetaFile, CVT_PNG );
                    delete pMetaFile;

                    if ( pStream )
                    {
                        pStream->SetVersion( SOFFICE_FILEFORMAT_CURRENT );
                        aAny <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( pStream->GetData() ),
                                                        pStream->Seek( STREAM_SEEK_TO_END ) );
                        delete pStream;
                    }
                }
            }
            else
                throw UNSUPPORTEDFLAVOREXCEPTION();
        }
        else
            throw UNSUPPORTEDFLAVOREXCEPTION();
    }

    return aAny;
}

//________________________________________________________________________________________________________
// XTransferable
//________________________________________________________________________________________________________


SEQUENCE< DATAFLAVOR > SAL_CALL SfxBaseModel::getTransferDataFlavors()
        throw (::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    sal_Int32 nSuppFlavors = GraphicHelper::supportsMetaFileHandle_Impl() ? 10 : 8;
    SEQUENCE< DATAFLAVOR > aFlavorSeq( nSuppFlavors );

    aFlavorSeq[0].MimeType =
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "application/x-openoffice-gdimetafile;windows_formatname=\"GDIMetaFile\"" ) );
    aFlavorSeq[0].HumanPresentableName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "GDIMetaFile" ) );
    aFlavorSeq[0].DataType = getCppuType( (const Sequence< sal_Int8 >*) 0 );

    aFlavorSeq[1].MimeType =
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "application/x-openoffice-highcontrast-gdimetafile;windows_formatname=\"GDIMetaFile\"" ) );
    aFlavorSeq[1].HumanPresentableName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "GDIMetaFile" ) );
    aFlavorSeq[1].DataType = getCppuType( (const Sequence< sal_Int8 >*) 0 );

    aFlavorSeq[2].MimeType =
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "application/x-openoffice-emf;windows_formatname=\"Image EMF\"" ) );
    aFlavorSeq[2].HumanPresentableName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Enhanced Windows MetaFile" ) );
    aFlavorSeq[2].DataType = getCppuType( (const Sequence< sal_Int8 >*) 0 );

    aFlavorSeq[3].MimeType =
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "application/x-openoffice-wmf;windows_formatname=\"Image WMF\"" ) );
    aFlavorSeq[3].HumanPresentableName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Windows MetaFile" ) );
    aFlavorSeq[3].DataType = getCppuType( (const Sequence< sal_Int8 >*) 0 );

    aFlavorSeq[4].MimeType =
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "application/x-openoffice-objectdescriptor-xml;windows_formatname=\"Star Object Descriptor (XML)\"" ) );
    aFlavorSeq[4].HumanPresentableName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Star Object Descriptor (XML)" ) );
    aFlavorSeq[4].DataType = getCppuType( (const Sequence< sal_Int8 >*) 0 );

    aFlavorSeq[5].MimeType =
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "application/x-openoffice-embed-source-xml;windows_formatname=\"Star Embed Source (XML)\"" ) );
    aFlavorSeq[5].HumanPresentableName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Star Embed Source (XML)" ) );
    aFlavorSeq[5].DataType = getCppuType( (const Sequence< sal_Int8 >*) 0 );

    aFlavorSeq[6].MimeType =
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "application/x-openoffice-bitmap;windows_formatname=\"Bitmap\"" ) );
    aFlavorSeq[6].HumanPresentableName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Bitmap" ) );
    aFlavorSeq[6].DataType = getCppuType( (const Sequence< sal_Int8 >*) 0 );

    aFlavorSeq[7].MimeType =
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "image/png" ) );
    aFlavorSeq[7].HumanPresentableName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PNG" ) );
    aFlavorSeq[7].DataType = getCppuType( (const Sequence< sal_Int8 >*) 0 );

    if ( nSuppFlavors == 10 )
    {
        aFlavorSeq[8].MimeType =
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "application/x-openoffice-emf;windows_formatname=\"Image EMF\"" ) );
        aFlavorSeq[8].HumanPresentableName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Enhanced Windows MetaFile" ) );
        aFlavorSeq[8].DataType = getCppuType( (const sal_uInt64*) 0 );

        aFlavorSeq[9].MimeType =
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "application/x-openoffice-wmf;windows_formatname=\"Image WMF\"" ) );
        aFlavorSeq[9].HumanPresentableName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Windows MetaFile" ) );
        aFlavorSeq[9].DataType = getCppuType( (const sal_uInt64*) 0 );
    }

    return aFlavorSeq;
}

//________________________________________________________________________________________________________
// XTransferable
//________________________________________________________________________________________________________


sal_Bool SAL_CALL SfxBaseModel::isDataFlavorSupported( const DATAFLAVOR& aFlavor )
        throw (::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( aFlavor.MimeType.equalsAscii( "application/x-openoffice-gdimetafile;windows_formatname=\"GDIMetaFile\"" ) )
    {
        if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            return sal_True;
    }
    else if ( aFlavor.MimeType.equalsAscii( "application/x-openoffice-highcontrast-gdimetafile;windows_formatname=\"GDIMetaFile\"" ) )
    {
        if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            return sal_True;
    }
    else if ( aFlavor.MimeType.equalsAscii( "application/x-openoffice-emf;windows_formatname=\"Image EMF\"" ) )
    {
        if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            return sal_True;
        else if ( GraphicHelper::supportsMetaFileHandle_Impl()
          && aFlavor.DataType == getCppuType( (const sal_uInt64*) 0 ) )
            return sal_True;
    }
    else if ( aFlavor.MimeType.equalsAscii( "application/x-openoffice-wmf;windows_formatname=\"Image WMF\"" ) )
    {
        if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            return sal_True;
        else if ( GraphicHelper::supportsMetaFileHandle_Impl()
          && aFlavor.DataType == getCppuType( (const sal_uInt64*) 0 ) )
            return sal_True;
    }
    else if ( aFlavor.MimeType.equalsAscii( "application/x-openoffice-objectdescriptor-xml;windows_formatname=\"Star Object Descriptor (XML)\"" ) )
    {
        if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            return sal_True;
    }
    else if ( aFlavor.MimeType.equalsAscii( "application/x-openoffice-embed-source;windows_formatname=\"Star EMBS\"" ) )
    {
        if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            return sal_True;
    }
    else if ( aFlavor.MimeType.equalsAscii( "application/x-openoffice-bitmap;windows_formatname=\"Bitmap\"" ) )
    {
        if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            return sal_True;
    }
    else if ( aFlavor.MimeType.equalsAscii( "image/png" ) )
    {
        if ( aFlavor.DataType == getCppuType( (const Sequence< sal_Int8 >*) 0 ) )
            return sal_True;
    }

    return sal_False;
}


//--------------------------------------------------------------------------------------------------------
//  XEventsSupplier
//--------------------------------------------------------------------------------------------------------

REFERENCE< XNAMEREPLACE > SAL_CALL SfxBaseModel::getEvents() throw( RUNTIMEEXCEPTION )
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( ! m_pData->m_xEvents.is() )
    {
        m_pData->m_xEvents = new SfxEvents_Impl( m_pData->m_pObjectShell, this );
    }

    return m_pData->m_xEvents;
}

//--------------------------------------------------------------------------------------------------------
//  XEventBroadcaster
//--------------------------------------------------------------------------------------------------------

void SAL_CALL SfxBaseModel::addEventListener( const REFERENCE< XDOCEVENTLISTENER >& aListener ) throw( RUNTIMEEXCEPTION )
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        return;

    m_pData->m_aInterfaceContainer.addInterface( ::getCppuType((const REFERENCE< XDOCEVENTLISTENER >*)0), aListener );
}

//--------------------------------------------------------------------------------------------------------
//  XEventBroadcaster
//--------------------------------------------------------------------------------------------------------

void SAL_CALL SfxBaseModel::removeEventListener( const REFERENCE< XDOCEVENTLISTENER >& aListener ) throw( RUNTIMEEXCEPTION )
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        return;

    m_pData->m_aInterfaceContainer.removeInterface( ::getCppuType((const REFERENCE< XDOCEVENTLISTENER >*)0), aListener );
}

//________________________________________________________________________________________________________
//  SfxListener
//________________________________________________________________________________________________________

void addTitle_Impl( Sequence < ::com::sun::star::beans::PropertyValue >& rSeq, const ::rtl::OUString& rTitle )
{
    sal_Int32 nCount = rSeq.getLength();
    sal_Int32 nArg;

    for ( nArg = 0; nArg < nCount; nArg++ )
    {
        ::com::sun::star::beans::PropertyValue& rProp = rSeq[nArg];
        if ( rProp.Name.equalsAscii("Title") )
        {
            rProp.Value <<= rTitle;
            break;
        }
    }

    if ( nArg == nCount )
    {
        rSeq.realloc( nCount+1 );
        rSeq[nCount].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Title") );
        rSeq[nCount].Value <<= rTitle;
    }
}

void SfxBaseModel::NotifyStorageListeners_Impl()
{
    uno::Reference< uno::XInterface > xSelfHold( static_cast< ::cppu::OWeakObject* >(this) );

    if ( m_pData->m_pObjectShell )
    {
        ::cppu::OInterfaceContainerHelper* pContainer =
            m_pData->m_aInterfaceContainer.getContainer(
                ::getCppuType( ( const uno::Reference< document::XStorageChangeListener >*) NULL ) );
        if ( pContainer != NULL )
        {
            uno::Reference< embed::XStorage > xNewStorage = m_pData->m_pObjectShell->GetStorage();
            ::cppu::OInterfaceIteratorHelper pIterator(*pContainer);
            while ( pIterator.hasMoreElements() )
            {
                try
                {
                    ((document::XStorageChangeListener*)pIterator.next())->notifyStorageChange(
                                                                            xSelfHold,
                                                                            xNewStorage );
                }
                catch( uno::RuntimeException& )
                {
                    pIterator.remove();
                }
            }
        }
    }
}

void SfxBaseModel::Notify(          SfxBroadcaster& rBC     ,
                             const  SfxHint&        rHint   )
{
    if ( !m_pData )
        return;

    if ( &rBC == m_pData->m_pObjectShell )
    {
        SfxSimpleHint* pSimpleHint = PTR_CAST( SfxSimpleHint, &rHint );
        if ( pSimpleHint && pSimpleHint->GetId() == SFX_HINT_DOCCHANGED )
            changing();

        SfxEventHint* pNamedHint = PTR_CAST( SfxEventHint, &rHint );
        if ( pNamedHint )
        {

            if ( SFX_EVENT_STORAGECHANGED == pNamedHint->GetEventId() )
            {
                // for now this event is sent only on creation of a new storage for new document
                // and in case of reload of medium without document reload
                // other events are used to detect storage change
                // NotifyStorageListeners_Impl();

                if ( m_pData->m_xUIConfigurationManager.is()
                  && m_pData->m_pObjectShell->GetCreateMode() != SFX_CREATE_MODE_EMBEDDED )
                {
                    REFERENCE< XSTORAGE > xConfigStorage;
                    rtl::OUString aUIConfigFolderName( RTL_CONSTASCII_USTRINGPARAM( "Configurations2" ));

                    xConfigStorage = getDocumentSubStorage( aUIConfigFolderName, com::sun::star::embed::ElementModes::READWRITE );
                    if ( !xConfigStorage.is() )
                        xConfigStorage = getDocumentSubStorage( aUIConfigFolderName, com::sun::star::embed::ElementModes::READ );

                    if ( xConfigStorage.is() || !m_pData->m_pObjectShell->GetStorage()->hasByName( aUIConfigFolderName ) )
                    {
                        // the storage is different, since otherwise it could not be opened, so it must be exchanged
                        Reference< XUICONFIGURATIONSTORAGE > xUIConfigStorage( m_pData->m_xUIConfigurationManager, UNOQUERY );
                        xUIConfigStorage->setStorage( xConfigStorage );
                    }
                    else
                    {
                        OSL_ENSURE( sal_False, "Unexpected scenario!\n" );
                    }
                }

                ListenForStorage_Impl( m_pData->m_pObjectShell->GetStorage() );
            }
            else if ( SFX_EVENT_LOADFINISHED == pNamedHint->GetEventId() )
            {
                ListenForStorage_Impl( m_pData->m_pObjectShell->GetStorage() );
            }
            else if ( SFX_EVENT_SAVEASDOCDONE == pNamedHint->GetEventId() )
            {
                m_pData->m_sURL = m_pData->m_pObjectShell->GetMedium()->GetName();
                SfxItemSet *pSet = m_pData->m_pObjectShell->GetMedium()->GetItemSet();
                ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > aArgs;
                ::rtl::OUString aTitle = m_pData->m_pObjectShell->GetTitle();
                TransformItems( SID_SAVEASDOC, *pSet, aArgs );
                addTitle_Impl( aArgs, aTitle );
                attachResource( m_pData->m_pObjectShell->GetMedium()->GetName(), aArgs );
            }

            postEvent_Impl( pNamedHint->GetEventId() );
        }

        if ( pSimpleHint )
        {
            if ( pSimpleHint->GetId() == SFX_HINT_TITLECHANGED )
            {
                ::rtl::OUString aTitle = m_pData->m_pObjectShell->GetTitle();
                addTitle_Impl( m_pData->m_seqArguments, aTitle );
                postEvent_Impl( pSimpleHint->GetId() );
            }
/*
            else if ( pSimpleHint->GetId() == SFX_HINT_DYING
                || pSimpleHint->GetId() == SFX_HINT_DEINITIALIZING )
            {
                SfxObjectShellLock pShellLock = m_pData->m_pObjectShellLock;
                m_pData->m_pObjectShellLock = SfxObjectShellLock();
            }
*/
        }

        SfxPrintingHint* pPrintHint = PTR_CAST( SfxPrintingHint, &rHint );
        if ( pPrintHint )
        {
            if ( pPrintHint->GetWhich() == -1 )     // -1 : Initialisation of PrintOptions
            {
                if ( !m_pData->m_xPrintJob.is() )
                    m_pData->m_xPrintJob = new SfxPrintJob_Impl( m_pData );

                PrintDialog* pDlg = pPrintHint->GetPrintDialog();
                Printer* pPrinter = pPrintHint->GetPrinter();
                ::rtl::OUString aPrintFile ( ( pPrinter && pPrinter->IsPrintFileEnabled() ) ? pPrinter->GetPrintFile() : String() );
                ::rtl::OUString aRangeText ( ( pDlg && pDlg->IsRangeChecked(PRINTDIALOG_RANGE) ) ? pDlg->GetRangeText() : String() );
                sal_Bool bSelectionOnly = ( ( pDlg && pDlg->IsRangeChecked(PRINTDIALOG_SELECTION) ) ? sal_True : sal_False );

                sal_Int32 nArgs = 2;
                if ( aPrintFile.getLength() )
                    nArgs++;
                if ( aRangeText.getLength() )
                    nArgs++;
                else if ( bSelectionOnly )
                    nArgs++;

                m_pData->m_aPrintOptions.realloc(nArgs);
                m_pData->m_aPrintOptions[0].Name = DEFINE_CONST_UNICODE("CopyCount");
                m_pData->m_aPrintOptions[0].Value <<= (sal_Int16) (pPrinter ? pPrinter->GetCopyCount() : 1 );
                m_pData->m_aPrintOptions[1].Name = DEFINE_CONST_UNICODE("Collate");
                m_pData->m_aPrintOptions[1].Value <<= (sal_Bool) (pDlg ? pDlg->IsCollateChecked() : sal_False );

                if ( bSelectionOnly )
                {
                    m_pData->m_aPrintOptions[2].Name = DEFINE_CONST_UNICODE("Selection");
                    m_pData->m_aPrintOptions[2].Value <<= bSelectionOnly;
                }
                else if ( aRangeText.getLength() )
                {
                    m_pData->m_aPrintOptions[2].Name = DEFINE_CONST_UNICODE("Pages");
                    m_pData->m_aPrintOptions[2].Value <<= aRangeText;
                }

                if ( aPrintFile.getLength() )
                {
                    m_pData->m_aPrintOptions[nArgs-1].Name = DEFINE_CONST_UNICODE("FileName");
                    m_pData->m_aPrintOptions[nArgs-1].Value <<= aPrintFile;
                }
            }
            else if ( pPrintHint->GetWhich() == -3 )    // -3 : AdditionalPrintOptions
            {
                    uno::Sequence < beans::PropertyValue >& lOldOpts = m_pData->m_aPrintOptions;
                    const uno::Sequence < beans::PropertyValue >& lNewOpts = pPrintHint->GetAdditionalOptions();
                    sal_Int32 nOld = lOldOpts.getLength();
                    sal_Int32 nAdd = lNewOpts.getLength();
                    lOldOpts.realloc( nOld + nAdd );

                    // assume that all new elements are overwriting old ones and so don't need to be added
                    sal_Int32 nTotal = nOld;
                    for ( sal_Int32 n=0; n<nAdd; n++ )
                    {
                        sal_Int32 m;
                        for ( m=0; m<nOld; m++ )
                            if ( lNewOpts[n].Name == lOldOpts[m].Name )
                                // new option overwrites old one
                                break;

                        if ( m == nOld )
                        {
                            // this is a new option, so add it to the resulting sequence - counter must be incremented
                            lOldOpts[nTotal].Name = lNewOpts[n].Name;
                            lOldOpts[nTotal++].Value = lNewOpts[n].Value;
                        }
                        else
                            // overwrite old option with new value, counter stays unmodified
                            lOldOpts[m].Value = lNewOpts[n].Value;
                    }

                    if ( nTotal != lOldOpts.getLength() )
                        // at least one new options has overwritten an old one, so we allocated too much
                        lOldOpts.realloc(  nTotal );
            }
            else if ( pPrintHint->GetWhich() != -2 )    // -2 : CancelPrintJob
            {
                view::PrintJobEvent aEvent;
                aEvent.Source = m_pData->m_xPrintJob;
                aEvent.State = (com::sun::star::view::PrintableState) pPrintHint->GetWhich();
                ::cppu::OInterfaceContainerHelper* pContainer = m_pData->m_aInterfaceContainer.getContainer( ::getCppuType( ( const uno::Reference< view::XPrintJobListener >*) NULL ) );
                if ( pContainer!=NULL )
                {
                    ::cppu::OInterfaceIteratorHelper pIterator(*pContainer);
                    while (pIterator.hasMoreElements())
                        ((view::XPrintJobListener*)pIterator.next())->printJobEvent( aEvent );
                }
            }
        }
    }
}

//________________________________________________________________________________________________________
//  public impl.
//________________________________________________________________________________________________________

void SfxBaseModel::changing()
{
    // object already disposed?
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        return;

    OINTERFACECONTAINERHELPER* pIC = m_pData->m_aInterfaceContainer.getContainer( ::getCppuType((const REFERENCE< XMODIFYLISTENER >*)0) );
    if( pIC )

    {
        EVENTOBJECT aEvent( (XMODEL *)this );
        OINTERFACEITERATORHELPER aIt( *pIC );
        while( aIt.hasMoreElements() )
        {
            try
            {
                ((XMODIFYLISTENER *)aIt.next())->modified( aEvent );
            }
            catch( RUNTIMEEXCEPTION& )
            {
                aIt.remove();
            }
        }
    }
}

void SfxBaseModel::impl_change()
{
    // object already disposed?
    if ( impl_isDisposed() )
        return;

    OINTERFACECONTAINERHELPER* pIC = m_pData->m_aInterfaceContainer.getContainer( ::getCppuType((const REFERENCE< XMODIFYLISTENER >*)0) );
    if( pIC )

    {
        EVENTOBJECT aEvent( (XMODEL *)this );
        OINTERFACEITERATORHELPER aIt( *pIC );
        while( aIt.hasMoreElements() )
        {
            try
            {
                ((XMODIFYLISTENER *)aIt.next())->modified( aEvent );
            }
            catch( RUNTIMEEXCEPTION& )
            {
                aIt.remove();
            }
        }
    }
}

//________________________________________________________________________________________________________
//  public impl.
//________________________________________________________________________________________________________

SfxObjectShell* SfxBaseModel::GetObjectShell() const
{
    return m_pData ? (SfxObjectShell*) m_pData->m_pObjectShell : 0;
}

SfxObjectShell* SfxBaseModel::impl_getObjectShell() const
{
    return m_pData ? (SfxObjectShell*) m_pData->m_pObjectShell : 0;
}

//________________________________________________________________________________________________________
//  public impl.
//________________________________________________________________________________________________________

sal_Bool SfxBaseModel::IsDisposed() const
{
    return ( m_pData == NULL ) ;
}

sal_Bool SfxBaseModel::impl_isDisposed() const
{
    return ( m_pData == NULL ) ;
}

//________________________________________________________________________________________________________
//  private impl.
//________________________________________________________________________________________________________

::rtl::OUString SfxBaseModel::GetMediumFilterName_Impl()
{
    const SfxFilter* pFilter = NULL;
    SfxMedium* pMedium = m_pData->m_pObjectShell->GetMedium();
    if ( pMedium )
        pFilter = pMedium->GetFilter();

    if ( pFilter )
        return pFilter->GetName();

    return ::rtl::OUString();
}

void SfxBaseModel::impl_store(  const   OUSTRING&                   sURL            ,
                                const   SEQUENCE< PROPERTYVALUE >&  seqArguments    ,
                                        sal_Bool                    bSaveTo         )
{
    if( !sURL.getLength() )
        throw ILLEGALARGUMENTIOEXCEPTION();

    //sal_Bool aSaveAsTemplate = sal_False;

    if ( m_pData->m_pObjectShell )
    {
        SFX_APP()->NotifyEvent( SfxEventHint( bSaveTo ? SFX_EVENT_SAVETODOC : SFX_EVENT_SAVEASDOC,
                                                m_pData->m_pObjectShell ) );

        SfxAllItemSet *aParams = new SfxAllItemSet( SFX_APP()->GetPool() );
        aParams->Put( SfxStringItem( SID_FILE_NAME, String(sURL) ) );
        if ( bSaveTo )
            aParams->Put( SfxBoolItem( SID_SAVETO, sal_True ) );

        TransformParameters( SID_SAVEASDOC, seqArguments, *aParams );

        SFX_ITEMSET_ARG( aParams, pCopyStreamItem, SfxBoolItem, SID_COPY_STREAM_IF_POSSIBLE, sal_False );
        if ( pCopyStreamItem && pCopyStreamItem->GetValue() && !bSaveTo )
            throw ILLEGALARGUMENTIOEXCEPTION(
                    ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("CopyStreamIfPossible parameter is not acceptable for storeAsURL() call!") ),
                    uno::Reference< uno::XInterface >() );

        sal_Bool bRet = m_pData->m_pObjectShell->APISaveAs_Impl( sURL, aParams );

        uno::Reference < task::XInteractionHandler > xHandler;
        SFX_ITEMSET_ARG( aParams, pItem, SfxUnoAnyItem, SID_INTERACTIONHANDLER, sal_False);
        if ( pItem )
            pItem->GetValue() >>= xHandler;

        DELETEZ( aParams );

        sal_uInt32 nErrCode = m_pData->m_pObjectShell->GetErrorCode();
        if ( !bRet && !nErrCode )
            nErrCode = ERRCODE_IO_CANTWRITE;
        m_pData->m_pObjectShell->ResetError();

        if ( bRet )
        {
            if ( nErrCode )
            {
                // must be a warning - use Interactionhandler if possible or abandone
                if ( xHandler.is() )
                {
                    ::com::sun::star::uno::Any aInteraction;
                    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation > > lContinuations(1);
                    ::framework::ContinuationApprove* pApprove = new ::framework::ContinuationApprove();
                    lContinuations[0] = ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation >(static_cast< ::com::sun::star::task::XInteractionContinuation* >(pApprove), UNOQUERY);

                    ::com::sun::star::task::ErrorCodeRequest aErrorCode;
                    aErrorCode.ErrCode = nErrCode;
                    aInteraction <<= aErrorCode;

                    ::framework::InteractionRequest* pRequest = new ::framework::InteractionRequest(aInteraction,lContinuations);
                    ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionRequest > xRequest(static_cast< ::com::sun::star::task::XInteractionRequest* >(pRequest), UNOQUERY);

                    xHandler->handle(xRequest);
                }
            }

            if ( !bSaveTo )
            {
                m_pData->m_aPreusedFilterName = GetMediumFilterName_Impl();
                SFX_APP()->NotifyEvent( SfxEventHint( SFX_EVENT_SAVEASDOCDONE, m_pData->m_pObjectShell ) );
            }
            else
            {
                SFX_APP()->NotifyEvent( SfxEventHint( SFX_EVENT_SAVETODOCDONE, m_pData->m_pObjectShell ) );
            }
        }
        else
        {
            SFX_APP()->NotifyEvent( SfxEventHint( bSaveTo ? SFX_EVENT_SAVETODOCFAILED : SFX_EVENT_SAVEASDOCFAILED,
                                                    m_pData->m_pObjectShell ) );

            throw task::ErrorCodeIOException( ::rtl::OUString(), uno::Reference< uno::XInterface >(), nErrCode );
        }
    }
}

//********************************************************************************************************

void SfxBaseModel::postEvent_Impl( sal_uInt16 nEventID )
{
    // object already disposed?
    if ( impl_isDisposed() )
        return;

    OINTERFACECONTAINERHELPER* pIC = m_pData->m_aInterfaceContainer.getContainer(
                                        ::getCppuType((const REFERENCE< XDOCEVENTLISTENER >*)0) );
    if( pIC )

    {
        OUSTRING aName = SfxEventConfiguration::GetEventName_Impl( nEventID );
        DOCEVENTOBJECT aEvent( (XMODEL *)this, aName );
        OINTERFACECONTAINERHELPER aIC( m_aMutex );
        SEQUENCE < REFERENCE < XINTERFACE > > aElements = pIC->getElements();
        for ( sal_Int32 nElem=0; nElem<aElements.getLength(); nElem++ )
            aIC.addInterface( aElements[nElem] );
        OINTERFACEITERATORHELPER aIt( aIC );
        while( aIt.hasMoreElements() )
        {
            try
            {
                ((XDOCEVENTLISTENER *)aIt.next())->notifyEvent( aEvent );
            }
            catch( RUNTIMEEXCEPTION& )
            {
                aIt.remove();
            }
        }
    }
}

REFERENCE < XINDEXACCESS > SAL_CALL SfxBaseModel::getViewData() throw(::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( m_pData->m_pObjectShell.Is() && !m_pData->m_contViewData.is() )
    {
        SfxViewFrame *pActFrame = SfxViewFrame::Current();
        if ( !pActFrame || pActFrame->GetObjectShell() != m_pData->m_pObjectShell )
            pActFrame = SfxViewFrame::GetFirst(m_pData->m_pObjectShell, TYPE(SfxTopViewFrame));

        if ( !pActFrame || !pActFrame->GetViewShell() )
            // currently no frame for this document at all or View is under construction
            return REFERENCE < XINDEXACCESS >();

        m_pData->m_contViewData = Reference < XINDEXACCESS >(
                ::comphelper::getProcessServiceFactory()->createInstance(
                DEFINE_CONST_UNICODE("com.sun.star.document.IndexedPropertyValues") ),
                UNOQUERY );

        if ( !m_pData->m_contViewData.is() )
        {
            // error: no container class available!
            return REFERENCE < XINDEXACCESS >();
        }

        REFERENCE < XINDEXCONTAINER > xCont( m_pData->m_contViewData, UNOQUERY );
        sal_Int32 nCount = 0;
        SEQUENCE < PROPERTYVALUE > aSeq;
        ::com::sun::star::uno::Any aAny;
        for ( SfxViewFrame *pFrame = SfxViewFrame::GetFirst(m_pData->m_pObjectShell, TYPE(SfxTopViewFrame) ); pFrame;
                pFrame = SfxViewFrame::GetNext(*pFrame, m_pData->m_pObjectShell, TYPE(SfxTopViewFrame) ) )
        {
            BOOL bIsActive = ( pFrame == pActFrame );
            pFrame->GetViewShell()->WriteUserDataSequence( aSeq );
            aAny <<= aSeq;
            xCont->insertByIndex( bIsActive ? 0 : nCount, aAny );
            nCount++;
        }
    }

    return m_pData->m_contViewData;
}

void SAL_CALL SfxBaseModel::setViewData( const REFERENCE < XINDEXACCESS >& aData ) throw(::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    m_pData->m_contViewData = aData;
}

/** calls all XEventListeners */
void SfxBaseModel::notifyEvent( const ::com::sun::star::document::EventObject& aEvent ) const
{
    // object already disposed?
    if ( impl_isDisposed() )
        return;

    OINTERFACECONTAINERHELPER* pIC = m_pData->m_aInterfaceContainer.getContainer(
                                        ::getCppuType((const REFERENCE< XDOCEVENTLISTENER >*)0) );
    if( pIC )

    {
        OINTERFACEITERATORHELPER aIt( *pIC );
        while( aIt.hasMoreElements() )
        {
            try
            {
                ((XDOCEVENTLISTENER *)aIt.next())->notifyEvent( aEvent );
            }
            catch( RUNTIMEEXCEPTION& )
            {
                aIt.remove();
            }
        }
    }
}

/** returns true if someone added a XEventListener to this XEventBroadcaster */
sal_Bool SfxBaseModel::hasEventListeners() const
{
    return !impl_isDisposed() && (NULL != m_pData->m_aInterfaceContainer.getContainer( ::getCppuType((const REFERENCE< XDOCEVENTLISTENER >*)0) ) );
}

void SAL_CALL SfxBaseModel::addPrintJobListener( const ::com::sun::star::uno::Reference< ::com::sun::star::view::XPrintJobListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        return;

    m_pData->m_aInterfaceContainer.addInterface( ::getCppuType((const REFERENCE< XPRINTJOBLISTENER >*)0), xListener );
}

void SAL_CALL SfxBaseModel::removePrintJobListener( const ::com::sun::star::uno::Reference< ::com::sun::star::view::XPrintJobListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    // object already disposed?
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        return;

    m_pData->m_aInterfaceContainer.removeInterface( ::getCppuType((const REFERENCE< XPRINTJOBLISTENER >*)0), xListener );
}

// simple declaration of class SvObject is enough
// the corresponding <so3/iface.hxx> cannon be included because it provides
// declaration of class SvBorder that conflicts with ../../inc/viewfrm.hxx
class SvObject;
sal_Int64 SAL_CALL SfxBaseModel::getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( !impl_isDisposed() && GetObjectShell() )
    {
        SvGlobalName aName( aIdentifier );
        if ( aName == SvGlobalName( SO3_GLOBAL_CLASSID ) )
             return (sal_Int64)(sal_IntPtr)(SvObject*)GetObjectShell();
        else if ( aName == SvGlobalName( SFX_GLOBAL_CLASSID ) )
             return (sal_Int64)(sal_IntPtr)(SfxObjectShell*)GetObjectShell();
    }

    return 0;
}

//____________________________________________________________________________________________________
//  XDocumentSubStorageSupplier
//____________________________________________________________________________________________________

void SfxBaseModel::ListenForStorage_Impl( const uno::Reference< embed::XStorage >& xStorage )
{
    uno::Reference< util::XModifiable > xModifiable( xStorage, uno::UNO_QUERY );
    if ( xModifiable.is() )
    {
        if ( m_pData->m_pStorageModifyListen == NULL )
        {
            m_pData->m_pStorageModifyListen = new OStorageModifyListen_Impl( *this );
            m_pData->m_pStorageModifyListen->acquire();
        }

        // no need to deregister the listening for old storage since it should be disposed automatically
        xModifiable->addModifyListener( static_cast< util::XModifyListener* >(
                                                m_pData->m_pStorageModifyListen ) );
    }
}

void SfxBaseModel::StorageIsModified_Impl()
{
    // this call can only be called by listener that listens for commit of substorages of models storage
    // there must be no locking, listener does it

    if ( !impl_isDisposed() && m_pData->m_pObjectShell.Is() && !m_pData->m_pObjectShell->IsModified() )
        m_pData->m_pObjectShell->SetModified( sal_True );
}

REFERENCE< XSTORAGE > SAL_CALL SfxBaseModel::getDocumentSubStorage( const ::rtl::OUString& aStorageName, sal_Int32 nMode )
    throw ( RUNTIMEEXCEPTION)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    REFERENCE< XSTORAGE > xResult;
    if ( m_pData->m_pObjectShell.Is() )
    {
        uno::Reference< embed::XStorage > xStorage = m_pData->m_pObjectShell->GetStorage();
        if ( xStorage.is() )
        {
            try
            {
                xResult = xStorage->openStorageElement( aStorageName, nMode );
#if 0
                uno::Reference< embed::XTransactionBroadcaster > xBroadcaster( xResult, UNOQUERY );
                if ( xBroadcaster.is() )
                {
                    if ( m_pData->m_pChildCommitListen == NULL )
                    {
                        m_pData->m_pChildCommitListen = new OChildCommitListen_Impl( *this );
                        m_pData->m_pChildCommitListen->acquire();
                    }
                    xBroadcaster->addTransactionListener( static_cast< embed::XTransactionListener* >(
                                                                            m_pData->m_pChildCommitListen ) );
                }
#endif
            }
            catch ( uno::Exception& )
            {
            }
        }
    }

    return xResult;
}

Sequence< ::rtl::OUString > SAL_CALL SfxBaseModel::getDocumentSubStoragesNames()
    throw ( io::IOException,
            RuntimeException )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    Sequence< ::rtl::OUString > aResult;
    sal_Int32 nResultSize = 0;
    sal_Bool bSuccess = sal_False;
    if ( m_pData->m_pObjectShell.Is() )
    {
        uno::Reference < embed::XStorage > xStorage = m_pData->m_pObjectShell->GetStorage();
        uno::Reference < container::XNameAccess > xAccess( xStorage, uno::UNO_QUERY );
        if ( xAccess.is() )
        {
            Sequence< ::rtl::OUString > aTemp = xAccess->getElementNames();
            for ( sal_Int32 n = 0; n < aTemp.getLength(); n++ )
            {
                if ( xStorage->isStorageElement( aTemp[n] ) )
                {
                    aResult.realloc( ++nResultSize );
                    aResult[ nResultSize - 1 ] = aTemp[n];
                }
            }

            bSuccess = sal_True;
        }
    }

    if ( !bSuccess )
        throw io::IOException();

       return aResult;
}

//____________________________________________________________________________________________________
//  XScriptProviderSupplier
//____________________________________________________________________________________________________


REFERENCE< XSCRIPTPROVIDER > SAL_CALL SfxBaseModel::getScriptProvider()
    throw ( RUNTIMEEXCEPTION )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();
    REFERENCE< XSCRIPTPROVIDER > xSp;

    REFERENCE< XPROPERTYSET > xProps( ::comphelper::getProcessServiceFactory(), UNO_QUERY );

    REFERENCE< XComponentContext > xCtx(
        xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DefaultContext" ))), UNO_QUERY );

    if ( xCtx.is() )
    {
        REFERENCE< XSCRIPTPROVIDERFACTORY > xFac(
            xCtx->getValueByName(
                ::rtl::OUString::createFromAscii( "/singletons/com.sun.star.script.provider.theMasterScriptProviderFactory") ), UNO_QUERY );
        if ( xFac.is() )
        {
            Any aContext;
            REFERENCE< XMODEL > xModel( (XMODEL*)this );
            aContext <<= xModel;
            xSp.set( xFac->createScriptProvider( aContext ) );
        }
    }
    return xSp;
}

//____________________________________________________________________________________________________
//  XUIConfigurationManagerSupplier
//____________________________________________________________________________________________________

rtl::OUString SfxBaseModel::getRuntimeUID() const
{
    OSL_ENSURE( m_pData->m_sRuntimeUID.getLength() > 0,
                "SfxBaseModel::getRuntimeUID - ID is empty!" );
    return m_pData->m_sRuntimeUID;
}

sal_Bool SfxBaseModel::hasValidSignatures() const
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( m_pData->m_pObjectShell.Is() )
        return ( m_pData->m_pObjectShell->ImplGetSignatureState( sal_False ) == SIGNATURESTATE_SIGNATURES_OK );
    return sal_False;
}

static void GetCommandFromSequence( rtl::OUString& rCommand, sal_Int32& nIndex, const uno::Sequence< beans::PropertyValue >& rSeqPropValue )
{
    rtl::OUString aCommand;
    nIndex = -1;

    for ( sal_Int32 i = 0; i < rSeqPropValue.getLength(); i++ )
    {
        if ( rSeqPropValue[i].Name.equalsAsciiL( "Command", 7 ))
        {
            rSeqPropValue[i].Value >>= rCommand;
            nIndex = i;
            return;
        }
    }
}

static void ConvertSlotsToCommands( SfxObjectShell* pDoc, REFERENCE< XINDEXCONTAINER >& rToolbarDefinition )
{
    if ( pDoc )
    {
        Any           aAny;
        SfxModule*    pModule( pDoc->GetFactory().GetModule() );
        rtl::OUString aSlotCmd( RTL_CONSTASCII_USTRINGPARAM( "slot:" ));
        rtl::OUString aUnoCmd( RTL_CONSTASCII_USTRINGPARAM( ".uno:" ));
        uno::Sequence< beans::PropertyValue > aSeqPropValue;

        for ( sal_Int32 i = 0; i < rToolbarDefinition->getCount(); i++ )
        {
            sal_Int32 nIndex( -1 );
            rtl::OUString aCommand;

            if ( rToolbarDefinition->getByIndex( i ) >>= aSeqPropValue )
            {
                GetCommandFromSequence( aCommand, nIndex, aSeqPropValue );
                if ( nIndex >= 0 && ( aCommand.indexOf( aSlotCmd ) == 0 ))
                {
                    rtl::OUString aSlot( aCommand.copy( 5 ));

                    // We have to replace the old "slot-Command" with our new ".uno:-Command"
                    const SfxSlot* pSlot = pModule->GetSlotPool()->GetSlot( USHORT( aSlot.toInt32() ));
                    if ( pSlot )
                    {
                        rtl::OUStringBuffer aStrBuf( aUnoCmd );
                        aStrBuf.appendAscii( pSlot->GetUnoName() );

                        aCommand = aStrBuf.makeStringAndClear();
                        aSeqPropValue[nIndex].Value <<= aCommand;
                        rToolbarDefinition->replaceByIndex( i, Any( aSeqPropValue ));
                    }
                }
            }
        }
    }
}

REFERENCE< XUICONFIGURATIONMANAGER > SAL_CALL SfxBaseModel::getUIConfigurationManager()
        throw ( RUNTIMEEXCEPTION )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( !m_pData->m_xUIConfigurationManager.is() )
    {
        REFERENCE< XUICONFIGURATIONMANAGER > xNewUIConfMan(
            ::comphelper::getProcessServiceFactory()->createInstance(
                ::rtl::OUString::createFromAscii( "com.sun.star.ui.UIConfigurationManager" )),
                UNOQUERY );

        Reference< XUICONFIGURATIONSTORAGE > xUIConfigStorage( xNewUIConfMan, UNOQUERY );
        if ( xUIConfigStorage.is() )
        {
            REFERENCE< XSTORAGE > xConfigStorage;

            // in case of embedded object the module configuration should be used
            if ( m_pData->m_pObjectShell->GetCreateMode() != SFX_CREATE_MODE_EMBEDDED )
            {
                rtl::OUString aUIConfigFolderName( RTL_CONSTASCII_USTRINGPARAM( "Configurations2" ));
                // First try to open with READWRITE and then READ
                xConfigStorage = getDocumentSubStorage( aUIConfigFolderName, embed::ElementModes::READWRITE );
                if ( xConfigStorage.is() )
                {
                    rtl::OUString aMediaTypeProp( RTL_CONSTASCII_USTRINGPARAM( "MediaType" ));
                    rtl::OUString aUIConfigMediaType(
                            RTL_CONSTASCII_USTRINGPARAM( "application/vnd.sun.xml.ui.configuration" ) );
                    rtl::OUString aMediaType;
                    REFERENCE< XPROPERTYSET > xPropSet( xConfigStorage, UNOQUERY );
                    Any a = xPropSet->getPropertyValue( aMediaTypeProp );
                    if ( !( a >>= aMediaType ) || ( aMediaType.getLength() == 0 ))
                    {
                        a <<= aUIConfigMediaType;
                        xPropSet->setPropertyValue( aMediaTypeProp, a );
                    }
                }
                else
                    xConfigStorage = getDocumentSubStorage( aUIConfigFolderName, embed::ElementModes::READ );

            }

            // initialize ui configuration manager with document substorage
            xUIConfigStorage->setStorage( xConfigStorage );

            if ( m_pData->m_pObjectShell->GetCreateMode() != SFX_CREATE_MODE_EMBEDDED )
            {
                // Import old UI configuration from OOo 1.x
                REFERENCE< XSTORAGE > xOOo1ConfigStorage;
                rtl::OUString         aOOo1UIConfigFolderName( RTL_CONSTASCII_USTRINGPARAM( "Configurations" ));

                // Try to open with READ
                xOOo1ConfigStorage = getDocumentSubStorage( aOOo1UIConfigFolderName, embed::ElementModes::READ );
                if ( xOOo1ConfigStorage.is() )
                {
                    uno::Reference< lang::XMultiServiceFactory > xServiceMgr( ::comphelper::getProcessServiceFactory() );
                    uno::Sequence< uno::Reference< container::XIndexContainer > > rToolbars;

                    sal_Bool bImported = UIConfigurationImporterOOo1x::ImportCustomToolbars(
                                            xNewUIConfMan, rToolbars, xServiceMgr, xOOo1ConfigStorage );
                    if ( bImported )
                    {
                        SfxObjectShell* pObjShell = SfxBaseModel::GetObjectShell();

                        char aNum[]   = "private:resource/toolbar/custom_OOo1x_0";
                        char aTitle[] = "Toolbar 0";
                        sal_Int32 nNumIndex = strlen( aNum )-1;
                        sal_Int32 nTitleIndex = strlen( aTitle )-1;
                        for ( sal_Int32 i = 0; i < rToolbars.getLength(); i++ )
                        {
                            aNum[nNumIndex]++;
                            aTitle[nTitleIndex]++;

                            rtl::OUString aCustomTbxName( RTL_CONSTASCII_USTRINGPARAM( aNum ));
                            rtl::OUString aCustomTbxTitle( RTL_CONSTASCII_USTRINGPARAM( aTitle ));

                            REFERENCE< XINDEXCONTAINER > xToolbar = rToolbars[i];
                            ConvertSlotsToCommands( pObjShell, xToolbar );
                            if ( !xNewUIConfMan->hasSettings( aCustomTbxName ))
                            {
                                // Set UIName for the toolbar with container property
                                uno::Reference< beans::XPropertySet > xPropSet( xToolbar, UNO_QUERY );
                                if ( xPropSet.is() )
                                {
                                    try
                                    {
                                        rtl::OUString aPropName( RTL_CONSTASCII_USTRINGPARAM( "UIName" ));
                                        Any           aAny( aCustomTbxTitle );
                                        xPropSet->setPropertyValue( aPropName, aAny );
                                    }
                                    catch ( beans::UnknownPropertyException& )
                                    {
                                    }
                                }

                                uno::Reference< container::XIndexAccess > xToolbarData( xToolbar, uno::UNO_QUERY );
                                xNewUIConfMan->insertSettings( aCustomTbxName, xToolbarData );
                                uno::Reference< ui::XUIConfigurationPersistence > xPersist( xNewUIConfMan, uno::UNO_QUERY );
                                xPersist->store();
                            }
                        }
                    }
                }
            }
        }

        m_pData->m_xUIConfigurationManager = xNewUIConfMan;
    }

    return m_pData->m_xUIConfigurationManager;
}

//____________________________________________________________________________________________________
//  XVisualObject
//____________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::setVisualAreaSize( sal_Int64 nAspect, const awt::Size& aSize )
        throw ( lang::IllegalArgumentException,
                embed::WrongStateException,
                uno::Exception,
                uno::RuntimeException )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( !m_pData->m_pObjectShell.Is() )
        throw uno::Exception(); // TODO: error handling

    SfxViewFrame* pViewFrm = SfxViewFrame::GetFirst( m_pData->m_pObjectShell, 0, sal_False );
    if ( pViewFrm && m_pData->m_pObjectShell->GetCreateMode() == SFX_CREATE_MODE_EMBEDDED && !pViewFrm->GetFrame()->IsInPlace() )
    {
        Window* pWindow = VCLUnoHelper::GetWindow( pViewFrm->GetFrame()->GetFrameInterface()->getContainerWindow() );
        Size aWinSize = pWindow->GetSizePixel();
        awt::Size aCurrent = getVisualAreaSize( nAspect );
        Size aDiff( aSize.Width-aCurrent.Width, aSize.Height-aCurrent.Height );
        Size aWrongDiff = OutputDevice::LogicToLogic( aDiff , m_pData->m_pObjectShell->GetMapUnit(), pWindow->GetMapMode() );
        aDiff = pViewFrm->GetViewShell()->GetWindow()->LogicToPixel( aDiff );
        aWinSize.Width() += aDiff.Width();
        aWinSize.Height() += aDiff.Height();
        pWindow->SetSizePixel( aWinSize );
    }
    else
    {
        Rectangle aTmpRect = m_pData->m_pObjectShell->GetVisArea( ASPECT_CONTENT );
        aTmpRect.SetSize( Size( aSize.Width, aSize.Height ) );
        m_pData->m_pObjectShell->SetVisArea( aTmpRect );
    }
}

awt::Size SAL_CALL SfxBaseModel::getVisualAreaSize( sal_Int64 /*nAspect*/ )
        throw ( lang::IllegalArgumentException,
                embed::WrongStateException,
                uno::Exception,
                uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( !m_pData->m_pObjectShell.Is() )
        throw uno::Exception(); // TODO: error handling

    Rectangle aTmpRect = m_pData->m_pObjectShell->GetVisArea( ASPECT_CONTENT );

#if 0
    Window* pWindow = NULL;
    SfxViewFrame* pViewFrm = m_pData->m_pObjectShell.Is() ?
                                SfxViewFrame::GetFirst( m_pData->m_pObjectShell, 0, sal_False ) : 0;

    if ( pWindow )
    {
        MapMode aInternalMapMode( pViewFrm->GetWindow().GetMapMode() );
        MapMode aExternalMapMode( m_pData->m_pObjectShell->GetMapUnit() );

        aTmpRect = OutputDevice::LogicToLogic( aTmpRect, aInternalMapMode, aExternalMapMode );
    }
#endif

    return awt::Size( aTmpRect.GetWidth(), aTmpRect.GetHeight() );
}


sal_Int32 SAL_CALL SfxBaseModel::getMapUnit( sal_Int64 /*nAspect*/ )
        throw ( uno::Exception,
                uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( !m_pData->m_pObjectShell.Is() )
        throw uno::Exception(); // TODO: error handling

    return VCLUnoHelper::VCL2UnoEmbedMapUnit( m_pData->m_pObjectShell->GetMapUnit() );
}

embed::VisualRepresentation SAL_CALL SfxBaseModel::getPreferredVisualRepresentation( ::sal_Int64 /*nAspect*/ )
        throw ( lang::IllegalArgumentException,
                embed::WrongStateException,
                uno::Exception,
                uno::RuntimeException )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    datatransfer::DataFlavor aDataFlavor(
            ::rtl::OUString::createFromAscii( "application/x-openoffice-gdimetafile;windows_formatname=\"GDIMetaFile\"" ),
            ::rtl::OUString::createFromAscii( "GDIMetaFile" ),
            ::getCppuType( (const uno::Sequence< sal_Int8 >*) NULL ) );

    embed::VisualRepresentation aVisualRepresentation;
    aVisualRepresentation.Data = getTransferData( aDataFlavor );
    aVisualRepresentation.Flavor = aDataFlavor;

    return aVisualRepresentation;
}

//____________________________________________________________________________________________________
//  XStorageBasedDocument
//____________________________________________________________________________________________________

void SAL_CALL SfxBaseModel::loadFromStorage( const REFERENCE< XSTORAGE >& xStorage,
                                             const SEQUENCE< PROPERTYVALUE >& aMediaDescriptor )
    throw ( ILLEGALARGUMENTEXCEPTION,
            DOUBLEINITIALIZATIONEXCEPTION,
            IOEXCEPTION,
            EXCEPTION,
            RUNTIMEEXCEPTION )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    if ( !m_pData->m_pObjectShell.Is() )
        throw IOEXCEPTION(); // TODO:

    if ( m_pData->m_pObjectShell.Is() )
    {
        if( m_pData->m_pObjectShell->GetMedium() )
            // if a Medium is present, the document is already initialized
            throw DOUBLEINITIALIZATIONEXCEPTION();

        // after i36090 is fixed the pool from object shell can be used
        // SfxAllItemSet aSet( m_pData->m_pObjectShell->GetPool() );
        SfxAllItemSet aSet( SFX_APP()->GetPool() );

        // the BaseURL is part of the ItemSet
        SfxMedium* pMedium = new SfxMedium( xStorage, String() );
        TransformParameters( SID_OPENDOC, aMediaDescriptor, aSet );
        pMedium->GetItemSet()->Put( aSet );

        // allow to use an interactionhandler (if there is one)
        pMedium->UseInteractionHandler( TRUE );

        // TODO/LATER: in case of template storage, the temporary copy must be created
        SFX_ITEMSET_ARG( &aSet, pTemplateItem, SfxBoolItem, SID_TEMPLATE, sal_False);
        BOOL bTemplate = pTemplateItem && pTemplateItem->GetValue();
        m_pData->m_pObjectShell->SetActivateEvent_Impl( bTemplate ? SFX_EVENT_CREATEDOC : SFX_EVENT_OPENDOC );
        m_pData->m_pObjectShell->Get_Impl()->bOwnsStorage = FALSE;

        // load document
        if ( !m_pData->m_pObjectShell->DoLoad(pMedium) )
        {
            sal_uInt32 nError = m_pData->m_pObjectShell->GetErrorCode();
            throw task::ErrorCodeIOException( ::rtl::OUString(),
                                                uno::Reference< uno::XInterface >(),
                                                nError ? nError : ERRCODE_IO_CANTREAD );
        }
    }
}

void SAL_CALL SfxBaseModel::storeToStorage( const REFERENCE< XSTORAGE >& xStorage,
                                const SEQUENCE< PROPERTYVALUE >& aMediaDescriptor )
    throw ( ILLEGALARGUMENTEXCEPTION,
            IOEXCEPTION,
            EXCEPTION,
            RUNTIMEEXCEPTION )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    REFERENCE< XSTORAGE > xResult;
    if ( !m_pData->m_pObjectShell.Is() )
        throw IOEXCEPTION(); // TODO:

    SfxAllItemSet aSet( m_pData->m_pObjectShell->GetPool() );
    TransformParameters( SID_SAVEASDOC, aMediaDescriptor, aSet );

    // TODO/LATER: may be a special URL "private:storage" should be used
    SFX_ITEMSET_ARG( &aSet, pItem, SfxStringItem, SID_FILTER_NAME, sal_False );
    sal_Int32 nVersion = SOFFICE_FILEFORMAT_CURRENT;
    if( pItem )
    {
        String aFilterName = pItem->GetValue();
        const SfxFilter* pFilter = SFX_APP()->GetFilterMatcher().GetFilter4FilterName( aFilterName );
        if ( pFilter && pFilter->UsesStorage() )
            nVersion = pFilter->GetVersion();
    }

    sal_Bool bSuccess = sal_False;
    if ( xStorage == m_pData->m_pObjectShell->GetStorage() )
    {
        // storing to the own storage
        bSuccess = m_pData->m_pObjectShell->DoSave();
    }
    else
    {
        // TODO/LATER: if the provided storage has some data inside the storing might fail, probably the storage must be truncated
        // TODO/LATER: is it possible to have a template here?
        m_pData->m_pObjectShell->SetupStorage( xStorage, nVersion, sal_False );

        // BaseURL is part of the ItemSet
        SfxMedium aMedium( xStorage, String(), &aSet );
        aMedium.CanDisposeStorage_Impl( FALSE );
        if ( aMedium.GetFilter() )
        {
            // storing without a valid filter will often crash
            bSuccess = m_pData->m_pObjectShell->DoSaveObjectAs( aMedium, TRUE );
            m_pData->m_pObjectShell->DoSaveCompleted( NULL );
        }
    }

    if ( !bSuccess )
    {
        sal_uInt32 nError = m_pData->m_pObjectShell->GetErrorCode();
        throw task::ErrorCodeIOException( ::rtl::OUString(),
                                            uno::Reference< uno::XInterface >(),
                                            nError ? nError : ERRCODE_IO_GENERAL );
    }
}

void SAL_CALL SfxBaseModel::switchToStorage( const REFERENCE< XSTORAGE >& xStorage )
        throw ( ILLEGALARGUMENTEXCEPTION,
                IOEXCEPTION,
                EXCEPTION,
                RUNTIMEEXCEPTION )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    REFERENCE< XSTORAGE > xResult;
    if ( !m_pData->m_pObjectShell.Is() )
        throw IOEXCEPTION(); // TODO:

    // the persistence should be switched only if the storage is different
    if ( xStorage != m_pData->m_pObjectShell->GetStorage()
      && !m_pData->m_pObjectShell->SwitchPersistance( xStorage ) )
    {
        sal_uInt32 nError = m_pData->m_pObjectShell->GetErrorCode();
        throw task::ErrorCodeIOException( ::rtl::OUString(),
                                            uno::Reference< uno::XInterface >(),
                                            nError ? nError : ERRCODE_IO_GENERAL );
    }

    m_pData->m_pObjectShell->Get_Impl()->bOwnsStorage = FALSE;
}

REFERENCE< XSTORAGE > SAL_CALL SfxBaseModel::getDocumentStorage()
        throw ( IOEXCEPTION,
                EXCEPTION,
                RUNTIMEEXCEPTION )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        throw DISPOSEDEXCEPTION();

    REFERENCE< XSTORAGE > xResult;
    if ( !m_pData->m_pObjectShell.Is() )
        throw IOEXCEPTION(); // TODO

    return m_pData->m_pObjectShell->GetStorage();
}

void SAL_CALL SfxBaseModel::addStorageChangeListener(
            const REFERENCE< document::XStorageChangeListener >& xListener )
        throw ( RUNTIMEEXCEPTION )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        return;

    m_pData->m_aInterfaceContainer.addInterface(
                                    ::getCppuType((const REFERENCE< document::XStorageChangeListener >*)0), xListener );
}

void SAL_CALL SfxBaseModel::removeStorageChangeListener(
            const REFERENCE< document::XStorageChangeListener >& xListener )
        throw ( RUNTIMEEXCEPTION )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( impl_isDisposed() )
        return;

    m_pData->m_aInterfaceContainer.removeInterface(
                                    ::getCppuType((const REFERENCE< document::XStorageChangeListener >*)0), xListener );
}


