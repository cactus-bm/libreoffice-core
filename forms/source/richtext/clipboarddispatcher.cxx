/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: clipboarddispatcher.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 23:59:14 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_forms.hxx"

#ifndef FORMS_SOURCE_RICHTEXT_CLIPBOARDDISPATCHER_HXX
#include "clipboarddispatcher.hxx"
#endif

#ifndef _MyEDITVIEW_HXX
#include <svx/editview.hxx>
#endif

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
/** === end UNO includes === **/

#ifndef _CLIPLISTENER_HXX
#include <svtools/cliplistener.hxx>
#endif
#ifndef _TRANSFER_HXX
#include <svtools/transfer.hxx>
#endif

//........................................................................
namespace frm
{
//........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::frame;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::util;
    using namespace ::com::sun::star::beans;

    //====================================================================
    namespace
    {
        static URL createClipboardURL( OClipboardDispatcher::ClipboardFunc _eFunc )
        {
            URL aURL;
            switch ( _eFunc )
            {
            case OClipboardDispatcher::eCut:
                aURL.Complete = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:Cut" ) );
                break;
            case OClipboardDispatcher::eCopy:
                aURL.Complete = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:Copy" ) );
                break;
            case OClipboardDispatcher::ePaste:
                aURL.Complete = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:Paste" ) );
                break;
            }
            return aURL;
        }
    }

    //====================================================================
    //= OClipboardDispatcher
    //====================================================================
    //--------------------------------------------------------------------
    OClipboardDispatcher::OClipboardDispatcher( EditView& _rView, ClipboardFunc _eFunc )
        :ORichTextFeatureDispatcher( _rView, createClipboardURL( _eFunc ) )
        ,m_eFunc( _eFunc )
        ,m_bLastKnownEnabled( sal_True )
    {
    }

    //--------------------------------------------------------------------
    sal_Bool OClipboardDispatcher::implIsEnabled( ) const
    {
        sal_Bool bEnabled = sal_False;
        switch ( m_eFunc )
        {
        case eCut:
            bEnabled = !getEditView()->IsReadOnly() && getEditView()->HasSelection();
            break;

        case eCopy:
            bEnabled = getEditView()->HasSelection();
            break;

        case ePaste:
            bEnabled = !getEditView()->IsReadOnly();
            break;
        }
        return bEnabled;
    }

    //--------------------------------------------------------------------
    FeatureStateEvent OClipboardDispatcher::buildStatusEvent() const
    {
        FeatureStateEvent aEvent( ORichTextFeatureDispatcher::buildStatusEvent() );
        aEvent.IsEnabled = implIsEnabled();
        return aEvent;
    }

    //--------------------------------------------------------------------
    void OClipboardDispatcher::invalidateFeatureState_Broadcast()
    {
        sal_Bool bEnabled = implIsEnabled();
        if ( m_bLastKnownEnabled == bEnabled )
            // nothing changed -> no notification
            return;
        m_bLastKnownEnabled = bEnabled;

        ORichTextFeatureDispatcher::invalidateFeatureState_Broadcast();
    }

    //--------------------------------------------------------------------
    void SAL_CALL OClipboardDispatcher::dispatch( const URL& /*_rURL*/, const Sequence< PropertyValue >& /*Arguments*/ ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !getEditView() )
            throw DisposedException();

        switch ( m_eFunc )
        {
        case eCut:
            getEditView()->Cut();
            break;

        case eCopy:
            getEditView()->Copy();
            break;

        case ePaste:
            getEditView()->Paste();
            break;
        }
    }

    //====================================================================
    //= OPasteClipboardDispatcher
    //====================================================================
    //--------------------------------------------------------------------
    OPasteClipboardDispatcher::OPasteClipboardDispatcher( EditView& _rView )
        :OClipboardDispatcher( _rView, ePaste )
        ,m_pClipListener( NULL )
        ,m_bPastePossible( sal_False )
    {
        m_pClipListener = new TransferableClipboardListener( LINK( this, OPasteClipboardDispatcher, OnClipboardChanged ) );
        m_pClipListener->acquire();
        m_pClipListener->AddRemoveListener( _rView.GetWindow(), TRUE );

        // initial state
        TransferableDataHelper aDataHelper( TransferableDataHelper::CreateFromSystemClipboard( _rView.GetWindow() ) );
        m_bPastePossible = ( aDataHelper.HasFormat( SOT_FORMAT_STRING ) || aDataHelper.HasFormat( SOT_FORMAT_RTF ) );
    }

    //--------------------------------------------------------------------
    OPasteClipboardDispatcher::~OPasteClipboardDispatcher()
    {
        if ( !isDisposed() )
        {
            acquire();
            dispose();
        }
    }

    //--------------------------------------------------------------------
    IMPL_LINK( OPasteClipboardDispatcher, OnClipboardChanged, TransferableDataHelper*, _pDataHelper )
    {
        OSL_ENSURE( _pDataHelper, "OPasteClipboardDispatcher::OnClipboardChanged: ooops!" );
        m_bPastePossible = _pDataHelper->HasFormat( SOT_FORMAT_STRING )
                        || _pDataHelper->HasFormat( SOT_FORMAT_RTF );

        invalidate();

        return 0L;
    }

    //--------------------------------------------------------------------
    void OPasteClipboardDispatcher::disposing( ::osl::ClearableMutexGuard& _rClearBeforeNotify )
    {
        OSL_ENSURE( getEditView() && getEditView()->GetWindow(), "OPasteClipboardDispatcher::disposing: EditView should not (yet) be disfunctional here!" );
        if ( getEditView() && getEditView()->GetWindow() && m_pClipListener )
            m_pClipListener->AddRemoveListener( getEditView()->GetWindow(), FALSE );
        m_pClipListener->release();
        m_pClipListener = NULL;

        OClipboardDispatcher::disposing( _rClearBeforeNotify );
    }

    //--------------------------------------------------------------------
    sal_Bool OPasteClipboardDispatcher::implIsEnabled( ) const
    {
        return m_bPastePossible && OClipboardDispatcher::implIsEnabled();
    }

//........................................................................
}   // namespace frm
//........................................................................

