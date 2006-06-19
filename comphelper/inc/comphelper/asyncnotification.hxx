/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: asyncnotification.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 22:42:12 $
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

#ifndef COMPHELPER_ASYNCNOTIFICATION_HXX
#define COMPHELPER_ASYNCNOTIFICATION_HXX

#ifndef _THREAD_HXX_
#include <osl/thread.hxx>
#endif

#ifndef _RTL_REF_HXX_
#include <rtl/ref.hxx>
#endif

#ifndef INCLUDED_COMPHELPERDLLAPI_H
#include <comphelper/comphelperdllapi.h>
#endif
#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
#endif

#include <memory>

//........................................................................
namespace comphelper
{
//........................................................................

    //====================================================================
    //= AnyEvent
    //====================================================================
    /** the very basic instance to hold a description of an event
    */
    class COMPHELPER_DLLPUBLIC AnyEvent : ::rtl::IReference
    {
    private:
        oslInterlockedCount m_refCount;

    public:
        AnyEvent();

        virtual oslInterlockedCount SAL_CALL acquire();
        virtual oslInterlockedCount SAL_CALL release();

    protected:
        virtual ~AnyEvent();

    private:
        AnyEvent( AnyEvent& ); // not defined
        void operator=( AnyEvent& ); // not defined
    };

    //====================================================================
    //= typedefs
    //====================================================================
    typedef ::rtl::Reference< AnyEvent >    AnyEventRef;

    //====================================================================
    //= IEventProcessor
    //====================================================================
    /** an event processor

        @see AsyncEventNotifier
    */
    class SAL_NO_VTABLE IEventProcessor
    {
    public:
        /** process a single event
        */
        virtual void processEvent( const AnyEvent& _rEvent ) = 0;

        virtual void SAL_CALL acquire() = 0;
        virtual void SAL_CALL release() = 0;
    };

    //====================================================================
    //= AsyncEventNotifier
    //====================================================================
    typedef ::osl::Thread  AsyncEventNotifier_TBASE;
    struct EventNotifierImpl;

    /** a helper class for notifying events asynchronously

        If you need to notify certain events to external components, you usually should
        not do this while you have mutexes locked, to prevent multi-threading issues.

        However, you do not always have complete control over all mutex guards on the stack.
        If, in such a case, the listener notification is one-way, you can decide to do it
        asynchronously.

        The ->AsyncEventNotifier helps you to process such events asynchronously. Every
        event is tied to an ->IEventProcessor which is responsible for processing it.

        The AsyncEventNotifier is implemented as a thread itself, which sleeps as long as there are no
        events in the queue. As soon as you add an event, the thread is woken up, processes the event,
        and sleeps again.
    */
    class COMPHELPER_DLLPUBLIC AsyncEventNotifier   :protected AsyncEventNotifier_TBASE
                                                    ,public ::rtl::IReference
    {
        friend struct EventNotifierImpl;

    private:
        ::std::auto_ptr< EventNotifierImpl >        m_pImpl;

    protected:
        // Thread
        virtual void SAL_CALL run();
        virtual void SAL_CALL onTerminated();

    public:
        /** constructs a notifier thread
        */
        AsyncEventNotifier();

        // IReference implementations
        virtual oslInterlockedCount SAL_CALL acquire();
        virtual oslInterlockedCount SAL_CALL release();

        /// creates (starts) the thread
        using AsyncEventNotifier_TBASE::create;

        using AsyncEventNotifier_TBASE::operator new;
        using AsyncEventNotifier_TBASE::operator delete;

        /** terminates the thread

            Note that this is a cooporative termination - if you call this from a thread different
            from the notification thread itself, then it will block until the notification thread
            finished processing the current event. If you call it from the notification thread
            itself, it will return immediately, and the thread will be terminated as soon as
            the current notification is finished.
        */
        virtual void SAL_CALL terminate();

        /** adds an event to the queue, together with the instance which is responsible for
            processing it

            @param _rEvent
                the event to add to the queue
            @param _xProcessor
                the processor for the event.<br/>
                Beware of life time issues here. If your event processor dies or becomes otherwise
                nonfunctional, you are responsible for removing all respective events from the queue.
                You can do this by calling ->removeEventsForProcessor
        */
        void addEvent( const AnyEventRef& _rEvent, const ::rtl::Reference< IEventProcessor >& _xProcessor );

        /** removes all events for the given event processor from the queue
        */
        void removeEventsForProcessor( const ::rtl::Reference< IEventProcessor >& _xProcessor );

    protected:
        virtual ~AsyncEventNotifier();
    };

    //====================================================================
    //= EventHolder
    //====================================================================
    /** AnyEvent derivee holding an foreign event instance
    */
    template < typename EVENT_OBJECT >
    class EventHolder : public AnyEvent
    {
    public:
        typedef EVENT_OBJECT    EventObjectType;

    private:
        EventObjectType m_aEvent;

    public:
        inline EventHolder( const EventObjectType& _rEvent )
            :m_aEvent( _rEvent )
        {
        }

        inline const EventObjectType& getEventObject() const { return m_aEvent; }
    };

//........................................................................
} // namespace comphelper
//........................................................................

#endif // COMPHELPER_ASYNCNOTIFICATION_HXX
