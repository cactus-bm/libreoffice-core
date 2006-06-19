/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: featuredispatcher.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 12:59:19 $
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

#ifndef FORMS_SOURCE_RICHTEXT_FEATUREDISPATCHER_HXX
#include "featuredispatcher.hxx"
#endif

/** === begin UNO includes === **/
/** === end UNO includes === **/

//........................................................................
namespace frm
{
//........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::frame;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::util;

    //====================================================================
    //= ORichTextFeatureDispatcher
    //====================================================================
    //--------------------------------------------------------------------
    ORichTextFeatureDispatcher::ORichTextFeatureDispatcher( EditView& _rView, const URL&  _rURL )
        :m_aFeatureURL( _rURL )
        ,m_aStatusListeners( m_aMutex )
        ,m_pEditView( &_rView )
        ,m_bDisposed( false )
    {
    }

    //--------------------------------------------------------------------
    ORichTextFeatureDispatcher::~ORichTextFeatureDispatcher( )
    {
        if ( !m_bDisposed )
        {
            acquire();
            dispose();
        }
    }

    //--------------------------------------------------------------------
    void ORichTextFeatureDispatcher::dispose()
    {
        EventObject aEvent( *this );
        m_aStatusListeners.disposeAndClear( aEvent );

        ::osl::ClearableMutexGuard aGuard( m_aMutex );
        m_bDisposed = true;
        disposing( aGuard );
    }

    //--------------------------------------------------------------------
    void ORichTextFeatureDispatcher::disposing( ::osl::ClearableMutexGuard& /*_rClearBeforeNotify*/ )
    {
        m_pEditView = NULL;
    }

    //--------------------------------------------------------------------
    void SAL_CALL ORichTextFeatureDispatcher::addStatusListener( const Reference< XStatusListener >& _rxControl, const URL& _rURL ) throw (RuntimeException)
    {
        OSL_ENSURE( !m_bDisposed, "ORichTextFeatureDispatcher::addStatusListener: already disposed!" );
        if ( m_bDisposed )
            throw DisposedException();

        OSL_ENSURE( _rURL.Complete == getFeatureURL().Complete, "ORichTextFeatureDispatcher::addStatusListener: invalid URL!" );
        if ( _rURL.Complete == getFeatureURL().Complete )
            if ( _rxControl.is() )
            {
                m_aStatusListeners.addInterface( _rxControl );
                newStatusListener( _rxControl );
            }
    }

    //--------------------------------------------------------------------
    void SAL_CALL ORichTextFeatureDispatcher::removeStatusListener( const Reference< XStatusListener >& _rxControl, const URL& /*_rURL*/ ) throw (RuntimeException)
    {
        m_aStatusListeners.removeInterface( _rxControl );
    }

    //--------------------------------------------------------------------
    void ORichTextFeatureDispatcher::invalidate()
    {
        invalidateFeatureState_Broadcast();
    }

    //--------------------------------------------------------------------
    FeatureStateEvent ORichTextFeatureDispatcher::buildStatusEvent() const
    {
        FeatureStateEvent aEvent;
        aEvent.IsEnabled = sal_False;
        aEvent.Source = *const_cast< ORichTextFeatureDispatcher* >( this );
        aEvent.FeatureURL = getFeatureURL();
        aEvent.Requery = sal_False;
        return aEvent;
    }

    //--------------------------------------------------------------------
    void ORichTextFeatureDispatcher::invalidateFeatureState_Broadcast()
    {
        FeatureStateEvent aEvent( buildStatusEvent() );
        ::cppu::OInterfaceIteratorHelper aIter( getStatusListeners() );
        while ( aIter.hasMoreElements() )
            doNotify( static_cast< XStatusListener* >( aIter.next() ), aEvent );
    }

    //--------------------------------------------------------------------
    void ORichTextFeatureDispatcher::newStatusListener( const Reference< XStatusListener >& _rxListener )
    {
        doNotify( _rxListener, buildStatusEvent() );
    }

    //--------------------------------------------------------------------
    void ORichTextFeatureDispatcher::doNotify( const Reference< XStatusListener >& _rxListener, const FeatureStateEvent& _rEvent ) const SAL_THROW(())
    {
        OSL_PRECOND( _rxListener.is(), "ORichTextFeatureDispatcher::doNotify: invalid listener!" );
        if ( _rxListener.is() )
        {
            try
            {
                _rxListener->statusChanged( _rEvent );
            }
            catch( const Exception& )
            {
                OSL_ENSURE( sal_False, "ORichTextFeatureDispatcher::doNotify: caught an exception!" );
            }
        }
    }

//........................................................................
}   // namespace frm
//........................................................................
