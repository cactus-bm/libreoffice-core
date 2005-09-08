/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: AccessibleTextEventQueue.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 20:19:34 $
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

#ifndef _SVX_TEXT_CHANGED_QUEUE_HXX
#define _SVX_TEXT_CHANGED_QUEUE_HXX

#include <memory>
#include <list>
#include <algorithm>

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif

#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif

class SfxHint;
class SdrHint;
class SfxSimpleHint;
class TextHint;
class SvxViewHint;
class SvxEditSourceHint;

namespace accessibility
{
    /** This class handles the notification events for the
        AccessibleTextHelper class.

        For various reasons, we cannot process EditEngine events as
        they arrive, but have to queue and handle them in a batch.
     */
    class AccessibleTextEventQueue
    {
    public:
        typedef ::std::list< SfxHint* > EventQueue;

        AccessibleTextEventQueue();
        ~AccessibleTextEventQueue();

        /// Append event to end of queue
        void Append( const SfxHint& rHint );
        /// Append event to end of queue
        void Append( const SdrHint& rHint );
        /// Append event to end of queue
        void Append( const SfxSimpleHint& rHint );
        /// Append event to end of queue
        void Append( const TextHint& rHint );
        /// Append event to end of queue
        void Append( const SvxViewHint& rHint );
        /// Append event to end of queue
        void Append( const SvxEditSourceHint& rHint );

        /** Pop first queue element

            return first queue element, ownership transfers to caller
        */
        ::std::auto_ptr< SfxHint > PopFront();

        /** Apply functor to every queue member

            @param rFunctor
            Functor to apply. Functor receives queue element as
            parameter: void func( const SfxHint* );
        */
        template < typename Functor > void ForEach( Functor& rFunctor ) const
        {
            // #109864# Make sure results are put back into rFunctor
            rFunctor = ::std::for_each( maEventQueue.begin(), maEventQueue.end(), rFunctor );
        }

        /// Query whether queue is empty
        bool IsEmpty() const;

        /// Clear event queue
        void Clear();

    private:
        EventQueue maEventQueue;
    };

} // end of namespace accessibility

#endif /* _SVX_TEXT_CHANGED_QUEUE_HXX */
