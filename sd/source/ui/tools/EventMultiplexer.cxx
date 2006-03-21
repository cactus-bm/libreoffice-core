/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: EventMultiplexer.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: obo $ $Date: 2006-03-21 17:33:25 $
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

#include "EventMultiplexer.hxx"

#include "MutexOwner.hxx"
#include "ViewShellBase.hxx"
#include "PaneManager.hxx"
#include "drawdoc.hxx"
#include "DrawController.hxx"
#include "SlideSorterViewShell.hxx"

#ifndef _COM_SUN_STAR_DOCUMENT_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTIOIN_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#include <cppuhelper/weak.hxx>
#ifndef _CPPUHELPER_COMPBASE3_HXX_
#include <cppuhelper/compbase3.hxx>
#endif
#include <sfx2/viewfrm.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;

class SdDrawDocument;

namespace sd { namespace tools {

typedef cppu::WeakComponentImplHelper3<
    ::com::sun::star::beans::XPropertyChangeListener,
    ::com::sun::star::frame::XFrameActionListener,
    ::com::sun::star::view::XSelectionChangeListener
    > EventMultiplexerImplementationInterfaceBase;

class EventMultiplexer::Implementation
    : protected MutexOwner,
      public EventMultiplexerImplementationInterfaceBase,
      public SfxListener
{
public:
    Implementation (ViewShellBase& rBase);
    ~Implementation (void);

    void AddEventListener (
        Link& rCallback,
        EventMultiplexerEvent::EventId aEventTypes);

    void RemoveEventListener (
        Link& rCallback,
        EventMultiplexerEvent::EventId aEventTypes);

    void CallListeners (EventMultiplexerEvent& rEvent);

    ViewShellBase& GetViewShellBase() const { return mrBase; }

    //===== lang::XEventListener ==============================================
    virtual void SAL_CALL
        disposing (const ::com::sun::star::lang::EventObject& rEventObject)
        throw (::com::sun::star::uno::RuntimeException);


    //===== beans::XPropertySetListener =======================================
    virtual void SAL_CALL
        propertyChange (
            const com::sun::star::beans::PropertyChangeEvent& rEvent)
        throw (::com::sun::star::uno::RuntimeException);

    //===== view::XSelectionChangeListener ====================================
    virtual void SAL_CALL
        selectionChanged (
            const com::sun::star::lang::EventObject& rEvent)
        throw (::com::sun::star::uno::RuntimeException);

    //===== frame::XFrameActionListener  ======================================
    /** For certain actions the listener connects to a new controller of the
        frame it is listening to.  This usually happens when the view shell
        in the center pane is replaced by another view shell.
    */
    virtual void SAL_CALL
        frameAction (const ::com::sun::star::frame::FrameActionEvent& rEvent)
        throw (::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL disposing (void);

protected:
    virtual void Notify (
        SfxBroadcaster& rBroadcaster,
        const SfxHint& rHint);

private:
    ViewShellBase& mrBase;
    typedef ::std::pair<Link,EventMultiplexerEvent::EventId> ListenerDescriptor;
    typedef ::std::vector<ListenerDescriptor> ListenerList;
    ListenerList maListeners;

    /// Remember whether we are listening to the UNO controller.
    bool mbListeningToController;
    /// Remember whether we are listening to the frame.
    bool mbListeningToFrame;
    /// Remember when the pane manger becomes unavailable.
    bool mbPaneManagerAvailable;

    ::com::sun::star::uno::WeakReference<
        ::com::sun::star::frame::XController> mxControllerWeak;
    ::com::sun::star::uno::WeakReference<
        ::com::sun::star::frame::XFrame> mxFrameWeak;
    ::com::sun::star::uno::WeakReference<
        ::com::sun::star::view::XSelectionSupplier> mxSlideSorterSelectionWeak;
    SdDrawDocument* mpDocument;

    static const ::rtl::OUString msCurrentPagePropertyName;
    static const ::rtl::OUString msEditModePropertyName;

    void ReleaseListeners (void);

    void ConnectToController (void);
    void DisconnectFromController (void);

    void CallListeners (EventMultiplexerEvent::EventId eId, void* pUserData=NULL);

    /** This method throws a DisposedException when the object has already been
        disposed.
    */
    void ThrowIfDisposed (void)
        throw (::com::sun::star::lang::DisposedException);

    DECL_LINK(PaneManagerEventListener, PaneManagerEvent*);
    DECL_LINK(SlideSorterSelectionChangeListener, void*);
};


const ::rtl::OUString EventMultiplexer::Implementation::msCurrentPagePropertyName (
    RTL_CONSTASCII_USTRINGPARAM("CurrentPage"));
const ::rtl::OUString EventMultiplexer::Implementation::msEditModePropertyName (
    RTL_CONSTASCII_USTRINGPARAM("IsMasterPageMode"));


//===== EventMultiplexer ======================================================

EventMultiplexer::EventMultiplexer (ViewShellBase& rBase)
    : mpImpl (new EventMultiplexer::Implementation(rBase))
{
    mpImpl->acquire();
}




EventMultiplexer::~EventMultiplexer (void)
{
    try
    {
        mpImpl->dispose();
        // Now we call release twice.  One decreases the use count of the
        // implementation object (if all goes well to zero and thus deletes
        // it.)  The other releases the auto_ptr and prevents the
        // implementation object from being deleted a second time.
        mpImpl->release();
        mpImpl.release();
    }
    catch (RuntimeException aException)
    {
    }
    catch (Exception aException)
    {
    }
}




void EventMultiplexer::AddEventListener (
    Link& rCallback,
    EventMultiplexerEvent::EventId aEventTypes)
{
    mpImpl->AddEventListener (rCallback, aEventTypes);
}




void EventMultiplexer::RemoveEventListener (
    Link& rCallback,
    EventMultiplexerEvent::EventId aEventTypes)
{
    mpImpl->RemoveEventListener (rCallback, aEventTypes);
}




void EventMultiplexer::MultiplexEvent( EventMultiplexerEvent::EventId eEventId, void* pUserData )
{
    EventMultiplexerEvent aEvent (mpImpl->GetViewShellBase(), eEventId, pUserData);
    mpImpl->CallListeners(aEvent);
}




//===== EventMultiplexer::Implementation ======================================

EventMultiplexer::Implementation::Implementation (ViewShellBase& rBase)
    : MutexOwner(),
      EventMultiplexerImplementationInterfaceBase(maMutex),
      SfxListener(),
      mrBase (rBase),
      mbListeningToController (false),
      mbListeningToFrame (false),
      mbPaneManagerAvailable(true),
      mxControllerWeak(NULL),
      mxFrameWeak(NULL),
      mxSlideSorterSelectionWeak(NULL),
      mpDocument(NULL)
{
    // Connect to the frame to listen for controllers being exchanged.
    // Listen to changes of certain properties.
    Reference<frame::XFrame> xFrame (
        mrBase.GetFrame()->GetTopFrame()->GetFrameInterface(),
        uno::UNO_QUERY);
    mxFrameWeak = xFrame;
    if (xFrame.is())
    {
        xFrame->addFrameActionListener (
            Reference<frame::XFrameActionListener>(
               static_cast<XWeak*>(this), UNO_QUERY));
        mbListeningToFrame = true;
    }

    // Connect to the current controller.
    ConnectToController ();

    // Listen for document changes.
    mpDocument = mrBase.GetDocument();
    if (mpDocument != NULL)
        StartListening (*mpDocument);

    // Listen for view switches.
    if (mbPaneManagerAvailable)
        mrBase.GetPaneManager().AddEventListener (
            LINK(this,EventMultiplexer::Implementation, PaneManagerEventListener));
}




EventMultiplexer::Implementation::~Implementation (void)
{
    DBG_ASSERT( !mbListeningToFrame, "sd::EventMultiplexer::Implementation::~Implementation(), disposing was not called!" );
}

void EventMultiplexer::Implementation::ReleaseListeners (void)
{
    if (mbListeningToFrame)
    {
        mbListeningToFrame = false;

        // Stop listening for changes of certain properties.
        Reference<frame::XFrame> xFrame (mxFrameWeak);
        if (xFrame.is())
        {
            xFrame->removeFrameActionListener (
                Reference<frame::XFrameActionListener>(
                    static_cast<XWeak*>(this), UNO_QUERY));
        }
    }

    DisconnectFromController ();

    if (mpDocument != NULL)
    {
        EndListening (*mpDocument);
        mpDocument = NULL;
    }

    // Stop listening for view switches.
    if (mbPaneManagerAvailable)
        mrBase.GetPaneManager().RemoveEventListener (
            LINK(this,EventMultiplexer::Implementation, PaneManagerEventListener));
}




void EventMultiplexer::Implementation::AddEventListener (
    Link& rCallback,
    EventMultiplexerEvent::EventId aEventTypes)
{
    ListenerList::iterator iListener (maListeners.begin());
    ListenerList::const_iterator iEnd (maListeners.end());
    for (;iListener!=iEnd; ++iListener)
        if (iListener->first == rCallback)
            break;
    if (iListener != maListeners.end())
    {
        // Listener exists.  Update its event type set.
        iListener->second |= aEventTypes;
    }
    else
    {
        maListeners.push_back (ListenerDescriptor(rCallback,aEventTypes));
    }
}




void EventMultiplexer::Implementation::RemoveEventListener (
    Link& rCallback,
    EventMultiplexerEvent::EventId aEventTypes)
{
    ListenerList::iterator iListener (maListeners.begin());
    ListenerList::const_iterator iEnd (maListeners.end());
    for (;iListener!=iEnd; ++iListener)
        if (iListener->first == rCallback)
            break;
    if (iListener != maListeners.end())
    {
        // Update the event type set.
        iListener->second &= ~aEventTypes;
        // When no events remain in the set then remove the listener.
        if (iListener->second == EID_EMPTY_SET)
            maListeners.erase (iListener);
    }
}




void EventMultiplexer::Implementation::ConnectToController (void)
{
    // Just in case that we missed some event we now disconnect from the old
    // controller.
    DisconnectFromController ();

    // Register at the controller of the main view shell.

    // We have to store a (weak) reference to the controller so that we can
    // unregister without having to ask the mrBase member (which at that
    // time may be destroyed.)
    Reference<frame::XController> xController = mrBase.GetController();
    mxControllerWeak = mrBase.GetController();

    try
    {
        // Listen for disposing events.
        Reference<lang::XComponent> xComponent (xController, UNO_QUERY);
        if (xComponent.is())
        {
            xComponent->addEventListener (
                Reference<lang::XEventListener>(
                    static_cast<XWeak*>(this), UNO_QUERY));
            mbListeningToController = true;
        }

        // Listen to changes of certain properties.
        Reference<beans::XPropertySet> xSet (xController, UNO_QUERY);
        if (xSet.is())
        {
                try
                {
                    xSet->addPropertyChangeListener(msCurrentPagePropertyName, this);
                }
                catch (beans::UnknownPropertyException)
                {
                    OSL_TRACE("EventMultiplexer::ConnectToController: CurrentPage unknown");
                }

                try
                {
                    xSet->addPropertyChangeListener(msEditModePropertyName, this);
                }
                catch (beans::UnknownPropertyException)
                {
                    OSL_TRACE("EventMultiplexer::ConnectToController: IsMasterPageMode unknown");
                }
        }

        // Listen for selection change events.
        Reference<view::XSelectionSupplier> xSelection (xController, UNO_QUERY);
        if (xSelection.is())
        {
            xSelection->addSelectionChangeListener(this);
        }
    }
    catch (const lang::DisposedException aException)
    {
        mbListeningToController = false;
    }
}




void EventMultiplexer::Implementation::DisconnectFromController (void)
{
    if (mbListeningToController)
    {
        mbListeningToController = false;

        Reference<frame::XController> xController = mxControllerWeak;

        Reference<beans::XPropertySet> xSet (xController, UNO_QUERY);
        // Remove the property listener.
        if (xSet.is())
        {
            try
            {
                xSet->removePropertyChangeListener(msCurrentPagePropertyName, this);
            }
            catch (beans::UnknownPropertyException aEvent)
            {
                OSL_TRACE ("DisconnectFromController: CurrentPage unknown");
            }

            try
            {
                xSet->removePropertyChangeListener(msEditModePropertyName, this);
            }
            catch (beans::UnknownPropertyException aEvent)
            {
                OSL_TRACE ("DisconnectFromController: IsMasterPageMode unknown");
            }
        }

        // Remove selection change listener.
        Reference<view::XSelectionSupplier> xSelection (xController, UNO_QUERY);
        if (xSelection.is())
        {
            xSelection->removeSelectionChangeListener(this);
        }

        // Remove listener for disposing events.
        Reference<lang::XComponent> xComponent (xController, UNO_QUERY);
        if (xComponent.is())
        {
            xComponent->removeEventListener (
                Reference<lang::XEventListener>(static_cast<XWeak*>(this), UNO_QUERY));
        }
    }
}




//=====  lang::XEventListener  ================================================

void SAL_CALL EventMultiplexer::Implementation::disposing (
    const lang::EventObject& rEventObject)
    throw (RuntimeException)
{
    if (mbListeningToController)
    {
        Reference<frame::XController> xController (mxControllerWeak);
        if (rEventObject.Source == xController)
        {
            mbListeningToController = false;
        }
    }
}




//=====  beans::XPropertySetListener  =========================================

void SAL_CALL EventMultiplexer::Implementation::propertyChange (
    const beans::PropertyChangeEvent& rEvent)
    throw (RuntimeException)
{
    ThrowIfDisposed();

    if (rEvent.PropertyName.equals(msCurrentPagePropertyName))
    {
        CallListeners(EventMultiplexerEvent::EID_CURRENT_PAGE);
    }
    else if (rEvent.PropertyName.equals(msEditModePropertyName))
    {
        CallListeners(EventMultiplexerEvent::EID_EDIT_MODE);
    }
}




//===== frame::XFrameActionListener  ==========================================

void SAL_CALL EventMultiplexer::Implementation::frameAction (
    const frame::FrameActionEvent& rEvent)
    throw (::com::sun::star::uno::RuntimeException)
{
    Reference<frame::XFrame> xFrame (mxFrameWeak);
    if (rEvent.Frame == xFrame)
        switch (rEvent.Action)
        {
            case frame::FrameAction_COMPONENT_DETACHING:
                DisconnectFromController();
                CallListeners (EventMultiplexerEvent::EID_CONTROLLER_DETACHED);
                break;

            case frame::FrameAction_COMPONENT_REATTACHED:
                CallListeners (EventMultiplexerEvent::EID_CONTROLLER_DETACHED);
                DisconnectFromController();
                ConnectToController();
                CallListeners (EventMultiplexerEvent::EID_CONTROLLER_ATTACHED);
                break;

            case frame::FrameAction_COMPONENT_ATTACHED:
                ConnectToController();
                CallListeners (EventMultiplexerEvent::EID_CONTROLLER_ATTACHED);
                break;

            default:
                break;
        }
}




//===== view::XSelectionChangeListener ========================================

void SAL_CALL EventMultiplexer::Implementation::selectionChanged (
    const lang::EventObject& rEvent)
    throw (::com::sun::star::uno::RuntimeException)
{
    CallListeners (EventMultiplexerEvent::EID_EDIT_VIEW_SELECTION);
}




void SAL_CALL EventMultiplexer::Implementation::disposing (void)
{
    CallListeners (EventMultiplexerEvent::EID_DISPOSING);
    ReleaseListeners();
}




void EventMultiplexer::Implementation::ThrowIfDisposed (void)
    throw (::com::sun::star::lang::DisposedException)
{
    if (rBHelper.bDisposed || rBHelper.bInDispose)
    {
        throw lang::DisposedException (
            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                "SlideSorterController object has already been disposed")),
            static_cast<uno::XWeak*>(this));
    }
}




void EventMultiplexer::Implementation::Notify (
    SfxBroadcaster& rBroadcaster,
    const SfxHint& rHint)
{
    if (rHint.ISA(SdrHint))
    {
        SdrHint& rSdrHint (*PTR_CAST(SdrHint,&rHint));
        switch (rSdrHint.GetKind())
        {
            case HINT_MODELCLEARED:
            case HINT_PAGEORDERCHG:
                CallListeners (EventMultiplexerEvent::EID_PAGE_ORDER);
                break;

            case HINT_SWITCHTOPAGE:
                CallListeners (EventMultiplexerEvent::EID_CURRENT_PAGE);
                break;
        }
    }
    else if (rHint.ISA(SfxSimpleHint))
    {
        SfxSimpleHint& rSimpleHint (*PTR_CAST(SfxSimpleHint, &rHint));
        if (rSimpleHint.GetId() == SFX_HINT_DYING)
            mpDocument = NULL;
    }
}




void EventMultiplexer::Implementation::CallListeners (
    EventMultiplexerEvent::EventId eId,
    void* pUserData)
{
    EventMultiplexerEvent aEvent (mrBase, eId, pUserData);
    CallListeners(aEvent);
}




void EventMultiplexer::Implementation::CallListeners (EventMultiplexerEvent& rEvent)
{
    ListenerList::const_iterator iListener (maListeners.begin());
    ListenerList::const_iterator iListenerEnd (maListeners.end());
    for (; iListener!=iListenerEnd; ++iListener)
    {
        if ((iListener->second && rEvent.meEventId) != 0)
            iListener->first.Call(&rEvent);
    }
}




IMPL_LINK(EventMultiplexer::Implementation, PaneManagerEventListener,
    PaneManagerEvent*, pEvent)
{
    OSL_ASSERT(pEvent!=NULL);

    switch (pEvent->meEventId)
    {
        case PaneManagerEvent::EID_VIEW_SHELL_ADDED:
            CallListeners (EventMultiplexerEvent::EID_VIEW_ADDED);

            if (pEvent->mePane == PaneManager::PT_CENTER)
                CallListeners (
                    EventMultiplexerEvent::EID_MAIN_VIEW_ADDED,
                    pEvent->mpShell);

            // Add selection change listener at slide sorter.
            if (pEvent->mpShell != NULL
                && pEvent->mpShell->GetShellType()
                    == ViewShell::ST_SLIDE_SORTER)
            {
                static_cast<slidesorter::SlideSorterViewShell*>(
                    pEvent->mpShell)->AddSelectionChangeListener (
                        LINK(this,
                            EventMultiplexer::Implementation,
                            SlideSorterSelectionChangeListener));
            }
            break;

        case PaneManagerEvent::EID_VIEW_SHELL_REMOVED:
            if (pEvent->mePane == PaneManager::PT_CENTER)
                CallListeners (EventMultiplexerEvent::EID_MAIN_VIEW_REMOVED);
            CallListeners (EventMultiplexerEvent::EID_VIEW_REMOVED);

            // Remove selection change listener from slide sorter.
            if (pEvent->mpShell != NULL
                && pEvent->mpShell->GetShellType()
                    == ViewShell::ST_SLIDE_SORTER)
            {
                static_cast<slidesorter::SlideSorterViewShell*>(
                    pEvent->mpShell)->RemoveSelectionChangeListener (
                        LINK(this,
                            EventMultiplexer::Implementation,
                            SlideSorterSelectionChangeListener));
            }
            break;

        case PaneManagerEvent::EID_PANE_MANAGER_DYING:
            CallListeners (EventMultiplexerEvent::EID_PANE_MANAGER_DYING);

            // Stop listening for view switches.
            mrBase.GetPaneManager().RemoveEventListener (
                LINK(this,EventMultiplexer::Implementation, PaneManagerEventListener));
            mbPaneManagerAvailable = false;
            break;
    }

    return 0;
}




IMPL_LINK(EventMultiplexer::Implementation, SlideSorterSelectionChangeListener,
    void*, pEvent)
{
    CallListeners (EventMultiplexerEvent::EID_SLIDE_SORTER_SELECTION);
    return 0;
}




//===== EventMultiplexerEvent =================================================

EventMultiplexerEvent::EventMultiplexerEvent (
    const ViewShellBase& rBase,
    EventId eEventId,
    void* pUserData)
    : mrBase(rBase),
      meEventId(eEventId),
      mpUserData(pUserData)

{
}

} } // end of namespace ::sd::tools
