/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: propcontroller.cxx,v $
 *
 *  $Revision: 1.31 $
 *
 *  last change: $Author: vg $ $Date: 2006-03-31 12:20:11 $
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
#ifndef _EXTENSIONS_PROPCTRLR_PROPCONTROLLER_HXX_
#include "propcontroller.hxx"
#endif
#ifndef _EXTENSIONS_FORMSCTRLR_PCRSTRINGS_HXX_
#include "pcrstrings.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_STANDARDCONTROL_HXX_
#include "standardcontrol.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_LINEDESCRIPTOR_HXX_
#include "linedescriptor.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_PROPRESID_HRC_
#include "propresid.hrc"
#endif
#ifndef _EXTENSIONS_FORMCTRLR_PROPRESID_HRC_
#include "formresid.hrc"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_PROPERTYEDITOR_HXX_
#include "propertyeditor.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_MODULEPRC_HXX_
#include "modulepcr.hxx"
#endif
#ifndef _EXTENSIONS_FORMSCTRLR_FORMSTRINGS_HXX_
#include "formstrings.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_FORMMETADATA_HXX_
#include "formmetadata.hxx"
#endif
#ifndef _EXTENSIONS_FORMSCTRLR_FORMBROWSERTOOLS_HXX_
#include "formbrowsertools.hxx"
#endif
#ifndef EXTENSIONS_SOURCE_PROPCTRLR_PROPERTYCOMPOSER_HXX
#include "propertycomposer.hxx"
#endif

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XCLOSEABLE_HPP_
#include <com/sun/star/util/XCloseable.hpp>
#endif
#ifndef _COM_SUN_STAR_INSPECTION_PROPERTYCONTROLTYPE_HPP_
#include <com/sun/star/inspection/PropertyControlType.hpp>
#endif
/** === end UNO includes === **/

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef TOOLS_DIAGNOSE_EX_H
#include <tools/diagnose_ex.h>
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef _TOOLKIT_AWT_VCLXWINDOW_HXX_
#include <toolkit/awt/vclxwindow.hxx>
#endif
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/unohlp.hxx>
#endif
#ifndef _COMPHELPER_PROPERTY_HXX_
#include <comphelper/property.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _CPPUHELPER_COMPONENT_CONTEXT_HXX_
#include <cppuhelper/component_context.hxx>
#endif
#ifndef _CPPUHELPER_EXC_HLP_HXX_
#include <cppuhelper/exc_hlp.hxx>
#endif

#include <algorithm>
#include <functional>

//------------------------------------------------------------------------
// !!! outside the namespace !!!
extern "C" void SAL_CALL createRegistryInfo_OPropertyBrowserController()
{
    ::pcr::OAutoRegistration< ::pcr::OPropertyBrowserController > aAutoRegistration;
}

//............................................................................
namespace pcr
{
//............................................................................

    using namespace ::com::sun::star;
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::awt;
    using namespace ::com::sun::star::form;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::script;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::container;
    using namespace ::com::sun::star::frame;
    using namespace ::com::sun::star::util;
    using namespace ::com::sun::star::inspection;
    using namespace ::comphelper;

#define THISREF()   static_cast< XController* >(this)

    //========================================================================
    //= OPropertyBrowserController
    //========================================================================
    DBG_NAME(OPropertyBrowserController)
    //------------------------------------------------------------------------
    OPropertyBrowserController::OPropertyBrowserController( const Reference< XComponentContext >& _rxContext )
            :m_aContext(_rxContext)
            ,m_aDisposeListeners(m_aMutex)
            ,m_pView(NULL)
            ,m_bContainerFocusListening(sal_False)
            ,m_bSuspendingPropertyHandlers( false )
    {
        DBG_CTOR(OPropertyBrowserController,NULL);
    }

    //------------------------------------------------------------------------
    OPropertyBrowserController::~OPropertyBrowserController()
    {
        // stop listening for property changes
        acquire();
        stopInspection( true );
        DBG_DTOR(OPropertyBrowserController,NULL);
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::startContainerWindowListening()
    {
        if (m_bContainerFocusListening)
            return;

        if (m_xFrame.is())
        {
            Reference< XWindow > xContainerWindow = m_xFrame->getContainerWindow();
            if (xContainerWindow.is())
            {
                xContainerWindow->addFocusListener(this);
                m_bContainerFocusListening = sal_True;
            }
        }

        DBG_ASSERT(m_bContainerFocusListening, "OPropertyBrowserController::startContainerWindowListening: unable to start listening (inconsistence)!");
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::stopContainerWindowListening()
    {
        if (!m_bContainerFocusListening)
            return;

        if (m_xFrame.is())
        {
            Reference< XWindow > xContainerWindow = m_xFrame->getContainerWindow();
            if (xContainerWindow.is())
            {
                xContainerWindow->removeFocusListener(this);
                m_bContainerFocusListening = sal_False;
            }
        }

        DBG_ASSERT(!m_bContainerFocusListening, "OPropertyBrowserController::stopContainerWindowListening: unable to stop listening (inconsistence)!");
    }

    //--------------------------------------------------------------------
    Reference< XObjectInspectorModel > SAL_CALL OPropertyBrowserController::getInspectorModel() throw (RuntimeException)
    {
        return m_xModel;
    }

    //--------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::setInspectorModel( const Reference< XObjectInspectorModel >& _inspectorModel ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if ( m_xModel == _inspectorModel )
            return;

        m_xModel = _inspectorModel;

        if ( m_aInspectedObjects.size() )
            rebindToInspectee( m_aInspectedObjects );
    }

    //--------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::inspect( const Sequence< Reference< XInterface > >& _rObjects ) throw (com::sun::star::util::VetoException, RuntimeException)
    {
        ::vos::OGuard aSolarGuard( Application::GetSolarMutex() );
        ::osl::MutexGuard aGuard( m_aMutex );

        if ( m_bSuspendingPropertyHandlers || !suspendPropertyHandlers_nothrow( sal_True ) )
        {   // we already are trying to suspend the component (this is somewhere up the stack)
            // OR one of our property handlers raised a veto against closing. Well, we *need* to close
            // it in order to inspect another object.
            throw VetoException();
        }
        rebindToInspectee( InterfaceArray( _rObjects.getConstArray(), _rObjects.getConstArray() + _rObjects.getLength() ) );
    }

    //--------------------------------------------------------------------
    Reference< XDispatch > SAL_CALL OPropertyBrowserController::queryDispatch( const URL& URL, const ::rtl::OUString& TargetFrameName, ::sal_Int32 SearchFlags ) throw (RuntimeException)
    {
        // we don't have any dispatches at all, right now
        return Reference< XDispatch >();
    }

    //--------------------------------------------------------------------
    Sequence< Reference< XDispatch > > SAL_CALL OPropertyBrowserController::queryDispatches( const Sequence< DispatchDescriptor >& Requests ) throw (RuntimeException)
    {
        Sequence< Reference< XDispatch > > aReturn;
        sal_Int32 nLen = Requests.getLength();
        aReturn.realloc( nLen );

                Reference< XDispatch >* pReturn     = aReturn.getArray();
        const   Reference< XDispatch >* pReturnEnd  = aReturn.getArray() + nLen;
        const   DispatchDescriptor*     pDescripts  = Requests.getConstArray();

        for ( ; pReturn != pReturnEnd; ++ pReturn, ++pDescripts )
            *pReturn = queryDispatch( pDescripts->FeatureURL, pDescripts->FrameName, pDescripts->SearchFlags );

        return aReturn;
    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::attachFrame( const Reference< XFrame >& _rxFrame ) throw(RuntimeException)
    {
        ::vos::OGuard aSolarGuard( Application::GetSolarMutex() );
        ::osl::MutexGuard aGuard( m_aMutex );

        if (_rxFrame.is() && haveView())
            throw RuntimeException(::rtl::OUString::createFromAscii("Unable to attach to a second frame."),*this);

        // revoke as focus listener from the old container window
        stopContainerWindowListening();

        m_xFrame = _rxFrame;
        if (!m_xFrame.is())
            return;

        // TODO: this construction perhaps should be done outside. Don't know the exact meaning of attachFrame.
        // Maybe it is intended to only announce the frame to the controller, and the instance doing this
        // announcement is responsible for calling setComponent, too.
        Reference< XWindow > xContainerWindow = m_xFrame->getContainerWindow();
        VCLXWindow* pContainerWindow = VCLXWindow::GetImplementation(xContainerWindow);
        Window* pParentWin = pContainerWindow ? pContainerWindow->GetWindow() : NULL;
        if (!pParentWin)
            throw RuntimeException(::rtl::OUString::createFromAscii("The frame is invalid. Unable to extract the container window."),*this);

        if ( Construct( pParentWin ) )
        {
            try
            {
                m_xFrame->setComponent( VCLUnoHelper::GetInterface( m_pView ), this );
            }
            catch( const Exception& )
            {
                OSL_ENSURE( sal_False, "OPropertyBrowserController::attachFrame: caught an exception!" );
            }
        }

        startContainerWindowListening();

        UpdateUI();
    }

    //------------------------------------------------------------------------
    sal_Bool SAL_CALL OPropertyBrowserController::attachModel( const Reference< XModel >& _rxModel ) throw(RuntimeException)
    {
        Reference< XObjectInspectorModel > xModel( _rxModel, UNO_QUERY );
        if ( !xModel.is() )
            return false;

        setInspectorModel( xModel );
        return getInspectorModel() == _rxModel;
    }

    //------------------------------------------------------------------------
    sal_Bool OPropertyBrowserController::suspendPropertyHandlers_nothrow( sal_Bool _bSuspend )
    {
        PropertyHandlerArray aAllHandlers;  // will contain every handler exactly once
        for (   PropertyHandlerRepository::const_iterator handler = m_aPropertyHandlers.begin();
                handler != m_aPropertyHandlers.end();
                ++handler
            )
        {
            if ( ::std::find( aAllHandlers.begin(), aAllHandlers.end(), handler->second ) != aAllHandlers.end() )
                // already visited this particular handler (m_aPropertyHandlers usually contains
                // the same handler more than once)
                continue;
            aAllHandlers.push_back( handler->second );
        }

        for ( PropertyHandlerArray::iterator loop = aAllHandlers.begin();
              loop != aAllHandlers.end();
              ++loop
            )
        {
            try
            {
                if ( !(*loop)->suspend( _bSuspend ) )
                    if ( _bSuspend )
                        // if we're not suspending, but reactivating, ignore the error
                        return sal_False;
            }
            catch( const Exception& )
            {
                OSL_ENSURE( sal_False, "OPropertyBrowserController::suspendPropertyHandlers_nothrow: caught an exception!" );
            }
        }
        return sal_True;
    }

    //------------------------------------------------------------------------
    sal_Bool SAL_CALL OPropertyBrowserController::suspend( sal_Bool _bSuspend ) throw(RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        OSL_ENSURE( haveView(), "OPropertyBrowserController::suspend: don't have a view anymore!" );

        if ( !_bSuspend )
        {   // this means a "suspend" is to be "revoked"
            suspendPropertyHandlers_nothrow( sal_False );
            // we ourself cannot revoke our suspend
            return sal_False;
        }

        {
            m_bSuspendingPropertyHandlers = true;
            sal_Bool bHandlerVeto = !suspendPropertyHandlers_nothrow( sal_True );
            m_bSuspendingPropertyHandlers = false;
            if ( bHandlerVeto )
                return sal_False;
        }

        // commit the editor's content
        if ( haveView() && getPropertyBox() )
            getPropertyBox()->CommitModified();

        // stop listening
        stopContainerWindowListening();

        // outtahere
        return sal_True;
    }

    //------------------------------------------------------------------------
    Any SAL_CALL OPropertyBrowserController::getViewData(  ) throw(RuntimeException)
    {
        return makeAny( m_sPageSelection );
    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::restoreViewData( const Any& Data ) throw(RuntimeException)
    {
        ::rtl::OUString sPageSelection;
        if ( ( Data >>= sPageSelection ) && sPageSelection.getLength() )
        {
            m_sPageSelection = sPageSelection;
            selectPageFromViewData();
        }
    }

    //------------------------------------------------------------------------
    Reference< XModel > SAL_CALL OPropertyBrowserController::getModel(  ) throw(RuntimeException)
    {
        // have no model
        return Reference< XModel >();
    }

    //------------------------------------------------------------------------
    Reference< XFrame > SAL_CALL OPropertyBrowserController::getFrame(  ) throw(RuntimeException)
    {
        return m_xFrame;
    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::dispose(  ) throw(RuntimeException)
    {
        ::vos::OGuard aSolarGuard( Application::GetSolarMutex() );

        // stop inspecting the current object
        stopInspection( false );

        // say our dispose listeners goodbye
        ::com::sun::star::lang::EventObject aEvt;
        aEvt.Source = static_cast< ::cppu::OWeakObject* >(this);
        m_aDisposeListeners.disposeAndClear(aEvt);

        // don't delete explicitly (this is done by the frame we reside in)
        m_pView = NULL;

        Reference< XComponent > xViewAsComp( m_xView, UNO_QUERY );
        if ( xViewAsComp.is() )
            xViewAsComp->removeEventListener( static_cast< XPropertyChangeListener* >( this ) );
        m_xView.clear( );

    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::addEventListener( const Reference< XEventListener >& _rxListener ) throw(RuntimeException)
    {
        m_aDisposeListeners.addInterface(_rxListener);
    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::removeEventListener( const Reference< XEventListener >& _rxListener ) throw(RuntimeException)
    {
        m_aDisposeListeners.removeInterface(_rxListener);
    }

    //------------------------------------------------------------------------
    ::rtl::OUString SAL_CALL OPropertyBrowserController::getImplementationName(  ) throw(RuntimeException)
    {
        return getImplementationName_static();
    }

    //------------------------------------------------------------------------
    sal_Bool SAL_CALL OPropertyBrowserController::supportsService( const ::rtl::OUString& ServiceName ) throw(RuntimeException)
    {
        Sequence< ::rtl::OUString > aSupported(getSupportedServiceNames());
        const ::rtl::OUString* pArray = aSupported.getConstArray();
        for (sal_Int32 i = 0; i < aSupported.getLength(); ++i, ++pArray)
            if (pArray->equals(ServiceName))
                return sal_True;
        return sal_False;
    }

    //------------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL OPropertyBrowserController::getSupportedServiceNames(  ) throw(RuntimeException)
    {
        return getSupportedServiceNames_static();
    }

    //------------------------------------------------------------------------
    ::rtl::OUString OPropertyBrowserController::getImplementationName_static(  ) throw(RuntimeException)
    {
        return ::rtl::OUString::createFromAscii("org.openoffice.comp.extensions.ObjectInspector");
    }

    //------------------------------------------------------------------------
    Sequence< ::rtl::OUString > OPropertyBrowserController::getSupportedServiceNames_static(  ) throw(RuntimeException)
    {
        Sequence< ::rtl::OUString > aSupported(1);
        aSupported[0] = ::rtl::OUString::createFromAscii( "com.sun.star.inspection.ObjectInspector" );
        return aSupported;
    }

    //------------------------------------------------------------------------
    Reference< XInterface > SAL_CALL OPropertyBrowserController::Create(const Reference< XComponentContext >& _rxContext)
    {
        return *(new OPropertyBrowserController( _rxContext ) );
    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::focusGained( const FocusEvent& _rSource ) throw (RuntimeException)
    {
        Reference< XWindow > xSourceWindow(_rSource.Source, UNO_QUERY);
        Reference< XWindow > xContainerWindow;
        if (m_xFrame.is())
            xContainerWindow = m_xFrame->getContainerWindow();

        if (xContainerWindow.get() == xSourceWindow.get())
        {   // our container window got the focus
            if ( haveView() && getPropertyBox())
                getPropertyBox()->GrabFocus();
        }
    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::focusLost( const FocusEvent& _rSource ) throw (RuntimeException)
    {
        // not interested in
    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::disposing( const EventObject& _rSource ) throw(RuntimeException)
    {
        if ( m_xView.is() && ( m_xView == _rSource.Source ) )
        {
            m_xView = NULL;
            m_pView = NULL;
        }

        for (   InterfaceArray::iterator loop = m_aInspectedObjects.begin();
                loop != m_aInspectedObjects.end();
                ++loop
            )
        {
            if ( *loop == _rSource.Source )
            {
                m_aInspectedObjects.erase( loop );
                break;
            }
        }
    }

    //------------------------------------------------------------------------
    IMPL_LINK(OPropertyBrowserController, OnPageActivation, void*, EMPTYARG)
    {
        updateViewDataFromActivePage();
        return 0L;
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::updateViewDataFromActivePage()
    {
        if (!haveView())
            return;

        ::rtl::OUString sOldSelection = m_sPageSelection;
        m_sPageSelection = ::rtl::OUString();

        const sal_uInt16 nCurrentPage = m_pView->getActivaPage();
        if ( (sal_uInt16)-1 != nCurrentPage )
        {
            for (   HashString2Int16::const_iterator pageId = m_aPageIds.begin();
                    pageId != m_aPageIds.end();
                    ++pageId
                )
            {
                if ( nCurrentPage == pageId->second )
                {
                    m_sPageSelection = pageId->first;
                    break;
                }
            }
        }

        if ( m_sPageSelection.getLength() )
            m_sLastValidPageSelection = m_sPageSelection;
        else if ( sOldSelection.getLength() )
            m_sLastValidPageSelection = sOldSelection;
    }

    //------------------------------------------------------------------------
    sal_uInt16 OPropertyBrowserController::impl_getPageIdForCategory_nothrow( const ::rtl::OUString& _rCategoryName ) const
    {
        sal_uInt16 nPageId = (sal_uInt16)-1;
        HashString2Int16::const_iterator pagePos = m_aPageIds.find( _rCategoryName );
        if ( pagePos != m_aPageIds.end() )
            nPageId = pagePos->second;
        return nPageId;
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::selectPageFromViewData()
    {
        sal_uInt16 nNewPage = impl_getPageIdForCategory_nothrow( m_sPageSelection );

        if ( haveView() && ( nNewPage != (sal_uInt16)-1 ) )
            m_pView->activatePage( nNewPage );

        // just in case ...
        updateViewDataFromActivePage();
    }

    //------------------------------------------------------------------------
    sal_Bool OPropertyBrowserController::Construct(Window* _pParentWin)
    {
        DBG_ASSERT(!haveView(), "OPropertyBrowserController::Construct: already have a view!");
        DBG_ASSERT(_pParentWin, "OPropertyBrowserController::Construct: invalid parent window!");

        m_pView = new OPropertyBrowserView(m_aContext.getLegacyServiceFactory(), _pParentWin);
        m_pView->setPageActivationHandler(LINK(this, OPropertyBrowserController, OnPageActivation));

        // add as dispose listener for our view. The view is disposed by the frame we're plugged into,
        // and this disposal _deletes_ the view, so it would be deadly if we use our m_pView member
        // after that
        m_xView = VCLUnoHelper::GetInterface(m_pView);
        Reference< XComponent > xViewAsComp(m_xView, UNO_QUERY);
        if (xViewAsComp.is())
            xViewAsComp->addEventListener( static_cast< XPropertyChangeListener* >( this ) );

        if ( haveView() )
        {
            getPropertyBox()->SetLineListener(this);
            m_pView->Show();
        }
        return sal_True;
    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::propertyChange( const PropertyChangeEvent& _rEvent ) throw (RuntimeException)
    {
        if ( m_sCommittingProperty != _rEvent.PropertyName )
        {
            if ( getPropertyBox() )
                // forward the new value to the property box, to reflect the change in the UI
                getPropertyBox()->SetPropertyValue( _rEvent.PropertyName, _rEvent.NewValue );

            // if it's a actuating property, then update the UI for any dependent
            // properties
            if ( impl_isActuatingProperty_nothrow( _rEvent.PropertyName ) )
                impl_broadcastPropertyChange_nothrow( _rEvent.PropertyName, _rEvent.NewValue, _rEvent.OldValue, false );
        }
    }

    //------------------------------------------------------------------------
    Reference< XPropertyControl > SAL_CALL OPropertyBrowserController::createPropertyControl( ::sal_Int16 ControlType, ::sal_Bool CreateReadOnly ) throw (IllegalArgumentException, RuntimeException)
    {
        Reference< XPropertyControl > xControl;

        // default winbits: a border only
        WinBits nWinBits = WB_BORDER;
        if ( CreateReadOnly )
            nWinBits |= WB_READONLY;

        switch ( ControlType )
        {
            case PropertyControlType::StringListField:
                xControl = new OMultilineEditControl( getPropertyBox(), eStringList, nWinBits | WB_DROPDOWN | WB_TABSTOP );
                break;

            case PropertyControlType::MultiLineTextField:
                xControl = new OMultilineEditControl( getPropertyBox(), eMultiLineText, nWinBits | WB_DROPDOWN | WB_TABSTOP );
                break;

            case PropertyControlType::ListBox:
                xControl = new OListboxControl( getPropertyBox(), nWinBits | WB_TABSTOP | WB_DROPDOWN);
                break;

            case PropertyControlType::ComboBox:
                xControl = new OComboboxControl( getPropertyBox(), nWinBits | WB_TABSTOP | WB_SORT | WB_DROPDOWN);
                break;

            case PropertyControlType::TextField:
                xControl = new OEditControl( getPropertyBox(), sal_False, nWinBits | WB_TABSTOP );
                break;

            case PropertyControlType::CharacterField:
                xControl = new OEditControl( getPropertyBox(), sal_True, nWinBits | WB_TABSTOP );
                break;

            case PropertyControlType::NumericField:
                xControl = new ONumericControl( getPropertyBox(), nWinBits | WB_TABSTOP | WB_SPIN | WB_REPEAT );
                break;

            case PropertyControlType::DateTimeField:
                xControl = new ODateTimeControl( getPropertyBox(), nWinBits | WB_TABSTOP );
                break;

            case PropertyControlType::DateField:
                xControl = new ODateControl( getPropertyBox(), nWinBits | WB_TABSTOP | WB_SPIN | WB_REPEAT );
                break;

            case PropertyControlType::TimeField:
                xControl = new OTimeControl( getPropertyBox(), nWinBits | WB_TABSTOP | WB_SPIN | WB_REPEAT );
                break;

            case PropertyControlType::ColorListBox:
                xControl = new OColorControl( getPropertyBox(), nWinBits | WB_TABSTOP | WB_DROPDOWN );
                break;

            case PropertyControlType::HyperlinkField:
                xControl = new OHyperlinkControl( getPropertyBox(), nWinBits | WB_TABSTOP | WB_DROPDOWN );
                break;

            default:
                throw IllegalArgumentException( ::rtl::OUString(), *this, 1 );
        }

        return xControl;
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::impl_toggleInspecteeListening_nothrow( bool _bOn )
    {
        for (   InterfaceArray::const_iterator loop = m_aInspectedObjects.begin();
                loop != m_aInspectedObjects.end();
                ++loop
            )
        {
            try
            {
                Reference< XComponent > xComp( *loop, UNO_QUERY );
                if ( xComp.is() )
                    if ( _bOn )
                        xComp->addEventListener( static_cast< XPropertyChangeListener* >( this ) );
                    else
                        xComp->removeEventListener( static_cast< XPropertyChangeListener* >( this ) );
            }
            catch( const Exception& e )
            {
            #if OSL_DEBUG_LEVEL > 0
                ::rtl::OString sMessage( "OPropertyBrowserController::impl_toggleInspecteeListening_nothrow: caught an exception!\n" );
                sMessage += "message:\n";
                sMessage += ::rtl::OString( e.Message.getStr(), e.Message.getLength(), osl_getThreadTextEncoding() );
                OSL_ENSURE( false, sMessage );
            #else
                e; // make compiler happy
            #endif
            }
        }
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::stopInspection( bool _bCommitModified )
    {
        if ( haveView() && getPropertyBox() )
        {
            if ( _bCommitModified && getPropertyBox() )
                // commit the editor's content
                getPropertyBox()->CommitModified();

            // hide the property box so that it does not flicker
            getPropertyBox()->Hide();

            // clear the property box
            getPropertyBox()->ClearAll();
        }

        // destroy the view first
        if ( haveView() )
        {
            // remove the pages
            for (   HashString2Int16::const_iterator erase = m_aPageIds.begin();
                    erase != m_aPageIds.end();
                    ++erase
                )
                getPropertyBox()->RemovePage( erase->second );
            clearContainer( m_aPageIds );
        }

        clearContainer( m_aProperties );

        // de-register as dispose-listener from our inspected objects
        impl_toggleInspecteeListening_nothrow( false );

        // handlers are obsolete, so is our "composer" for their UI requests
        if ( m_pUIRequestComposer.get() )
            m_pUIRequestComposer->dispose();
        m_pUIRequestComposer.reset( NULL );

        // clean up the property handlers
        PropertyHandlerArray aAllHandlers;  // will contain every handler exactly once
        for ( PropertyHandlerRepository::const_iterator aHandler = m_aPropertyHandlers.begin();
              aHandler != m_aPropertyHandlers.end();
              ++aHandler
            )
            if ( ::std::find( aAllHandlers.begin(), aAllHandlers.end(), aHandler->second ) == aAllHandlers.end() )
                aAllHandlers.push_back( aHandler->second );

        for ( PropertyHandlerArray::iterator loop = aAllHandlers.begin();
              loop != aAllHandlers.end();
              ++loop
            )
        {
            try
            {
                (*loop)->removePropertyChangeListener( this );
                (*loop)->dispose();
            }
            catch( const DisposedException& )
            {
            }
            catch( const Exception& )
            {
                DBG_UNHANDLED_EXCEPTION();
            }
        }

        clearContainer( m_aPropertyHandlers );
        clearContainer( m_aDependencyHandlers );
    }

    //------------------------------------------------------------------------
    OPropertyBrowserController::PropertyHandlerRef OPropertyBrowserController::impl_getHandlerForProperty_throw( const ::rtl::OUString& _rPropertyName ) const
    {
        PropertyHandlerRepository::const_iterator handlerPos = m_aPropertyHandlers.find( _rPropertyName );
        if ( handlerPos == m_aPropertyHandlers.end() )
            throw RuntimeException();
        return handlerPos->second;
    }

    //------------------------------------------------------------------------
    Any OPropertyBrowserController::impl_getPropertyValue_throw( const ::rtl::OUString& _rPropertyName )
    {
        PropertyHandlerRef handler = impl_getHandlerForProperty_throw( _rPropertyName );
        return handler->getPropertyValue( _rPropertyName );
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::rebindToInspectee( const InterfaceArray& _rObjects )
    {
        try
        {
            // stop inspecting the old object(s)
            stopInspection( true );

            // inspect the new object(s)
            m_aInspectedObjects = _rObjects;
            doInspection();

            // update the user interface
            UpdateUI();
        }

        catch(Exception&)
        {
            DBG_ERROR("OPropertyBrowserController::rebindToInspectee: caught an exception !");
        }
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::doInspection()
    {
        try
        {
            //////////////////////////////////////////////////////////////////////
            // obtain the properties of the object
            ::std::vector< Property > aProperties;

            PropertyHandlerArray aPropertyHandlers;
            getPropertyHandlers( m_aInspectedObjects, aPropertyHandlers );

            PropertyHandlerArray::iterator aHandler( aPropertyHandlers.begin() );
            while ( aHandler != aPropertyHandlers.end() )
            {
                DBG_ASSERT( aHandler->get(), "OPropertyBrowserController::doInspection: invalid handler!" );

                StlSyntaxSequence< Property > aThisHandlersProperties = (*aHandler)->getSupportedProperties();

                if ( aThisHandlersProperties.empty() )
                {
                    // this handler doesn't know anything about the current inspectee -> ignore it
                    (*aHandler)->dispose();
                    aHandler = aPropertyHandlers.erase( aHandler );
                    continue;
                }

                // append these properties to our "all properties" array
                aProperties.reserve( aProperties.size() + aThisHandlersProperties.size() );
                for (   StlSyntaxSequence< Property >::const_iterator copyProperty = aThisHandlersProperties.begin();
                        copyProperty != aThisHandlersProperties.end();
                        ++copyProperty
                    )
                {
                    ::std::vector< Property >::const_iterator previous = ::std::find_if(
                        aProperties.begin(),
                        aProperties.end(),
                        FindPropertyByName( copyProperty->Name )
                    );
                    if ( previous != aProperties.end() )
                    {
                        // there already was another (previous) handler which supported this property.
                        // Don't add it to aProperties, again
                        continue;
                    }

                    aProperties.push_back( *copyProperty );
                }

                // determine the superseded properties
                StlSyntaxSequence< ::rtl::OUString > aSuperSededByThisHandler = (*aHandler)->getSupersededProperties();
                for (   StlSyntaxSequence< ::rtl::OUString >::const_iterator superseded = aSuperSededByThisHandler.begin();
                        superseded != aSuperSededByThisHandler.end();
                        ++superseded
                    )
                {
                    ::std::vector< Property >::iterator existent = ::std::find_if(
                        aProperties.begin(),
                        aProperties.end(),
                        FindPropertyByName( *superseded )
                    );
                    if ( existent != aProperties.end() )
                        // one of the properties superseded by this handler was supported by a previous
                        // one -> erase
                        aProperties.erase( existent );
                }

                // be notified of changes which this handler is responsible for
                (*aHandler)->addPropertyChangeListener( this );

                // remember this handler for every of the properties which it is responsible
                // for
                for (   ::std::vector< Property >::const_iterator remember = aProperties.begin();
                        remember != aProperties.end();
                        ++remember
                    )
                {
                    m_aPropertyHandlers.insert( PropertyHandlerRepository::value_type(
                        remember->Name, *aHandler
                    ) );
                    // note that this implies that if two handlers support the same property,
                    // the latter wins
                }

                // see if the handler expresses interest in any actuating properties
                StlSyntaxSequence< ::rtl::OUString > aInterestingActuations = (*aHandler)->getActuatingProperties();
                for (   StlSyntaxSequence< ::rtl::OUString >::const_iterator aLoop = aInterestingActuations.begin();
                        aLoop != aInterestingActuations.end();
                        ++aLoop
                    )
                {
                    m_aDependencyHandlers.insert( PropertyHandlerMultiRepository::value_type(
                        *aLoop, *aHandler ) );
                }

                ++aHandler;
            }

            // create a new composer for UI requests coming from the handlers
            m_pUIRequestComposer.reset( new ComposedPropertyUIUpdate( this, this ) );

            // sort the properties by relative position, as indicated by the model
            for (   ::std::vector< Property >::const_iterator sourceProps = aProperties.begin();
                    sourceProps != aProperties.end();
                    ++sourceProps
                )
            {
                sal_Int32 nRelativePropertyOrder = sourceProps - aProperties.begin();
                if ( m_xModel.is() )
                    nRelativePropertyOrder = m_xModel->getPropertyOrderIndex( sourceProps->Name );
                while ( m_aProperties.find( nRelativePropertyOrder ) != m_aProperties.end() )
                    ++nRelativePropertyOrder;
                m_aProperties[ nRelativePropertyOrder ] = *sourceProps;
            }

            // be notified when one of our inspectees dies
            impl_toggleInspecteeListening_nothrow( true );
        }
        catch(Exception&)
        {
            DBG_ERROR("OPropertyBrowserController::doInspection : caught an exception !");
        }
    }

    //------------------------------------------------------------------------
    ::com::sun::star::awt::Size SAL_CALL OPropertyBrowserController::getMinimumSize() throw (::com::sun::star::uno::RuntimeException)
    {
        ::com::sun::star::awt::Size aSize;
        if( m_pView )
            return m_pView->getMinimumSize();
        else
            return aSize;
    }

    //------------------------------------------------------------------------
    ::com::sun::star::awt::Size SAL_CALL OPropertyBrowserController::getPreferredSize() throw (::com::sun::star::uno::RuntimeException)
    {
        return getMinimumSize();
    }

    //------------------------------------------------------------------------
    ::com::sun::star::awt::Size SAL_CALL OPropertyBrowserController::calcAdjustedSize( const ::com::sun::star::awt::Size& _rNewSize ) throw (::com::sun::star::uno::RuntimeException)
    {
        awt::Size aMinSize = getMinimumSize( );
        awt::Size aAdjustedSize( _rNewSize );
        if ( aAdjustedSize.Width < aMinSize.Width )
            aAdjustedSize.Width = aMinSize.Width;
        if ( aAdjustedSize.Height < aMinSize.Height )
            aAdjustedSize.Height = aMinSize.Height;
        return aAdjustedSize;
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::describePropertyLine( const Property& _rProperty, OLineDescriptor& _rDescriptor ) SAL_THROW((Exception))
    {
        try
        {
            PropertyHandlerRepository::const_iterator handler = m_aPropertyHandlers.find( _rProperty.Name );
            if ( handler == m_aPropertyHandlers.end() )
                throw RuntimeException();   // caught below

            handler->second->describePropertyLine( _rProperty.Name, _rDescriptor, this );

            //////////////////////////////////////////////////////////////////////

            _rDescriptor.xPropertyHandler = handler->second;
            _rDescriptor.sName = _rProperty.Name;
            _rDescriptor.aValue = _rDescriptor.xPropertyHandler->getPropertyValue( _rProperty.Name );

            if ( !_rDescriptor.DisplayName.getLength() )
            {
            #ifdef DBG_UTIL
                ::rtl::OString sMessage( "OPropertyBrowserController::describePropertyLine: handler did not provide a display name for '" );
                sMessage += ::rtl::OString( _rProperty.Name.getStr(), _rProperty.Name.getLength(), RTL_TEXTENCODING_ASCII_US );
                sMessage += ::rtl::OString( "'!" );
                DBG_ASSERT( _rDescriptor.DisplayName.getLength(), sMessage );
            #endif
                _rDescriptor.DisplayName = _rProperty.Name;
            }

            PropertyState   ePropertyState( _rDescriptor.xPropertyHandler->getPropertyState( _rProperty.Name ) );
            if ( PropertyState_AMBIGUOUS_VALUE == ePropertyState )
            {
                _rDescriptor.bUnknownValue = true;
                _rDescriptor.aValue.clear();
            }
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "OPropertyBrowserController::describePropertyLine: caught an exception!" );
        }
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::impl_buildCategories_throw()
    {
        OSL_PRECOND( m_aPageIds.empty(), "OPropertyBrowserController::impl_buildCategories_throw: duplicate call!" );

        StlSyntaxSequence< PropertyCategoryDescriptor > aCategories;
        if ( m_xModel.is() )
            aCategories = m_xModel->describeCategories();

        for (   StlSyntaxSequence< PropertyCategoryDescriptor >::const_iterator category = aCategories.begin();
                category != aCategories.end();
                ++category
            )
        {
            OSL_ENSURE( m_aPageIds.find( category->ProgrammaticName ) == m_aPageIds.end(),
                "OPropertyBrowserController::impl_buildCategories_throw: duplicate programmatic name!" );

            m_aPageIds[ category->ProgrammaticName ] =
                getPropertyBox()->AppendPage( category->UIName, HelpIdUrl::getHelpId( category->HelpURL ) );
        }
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::UpdateUI()
    {
        try
        {
            if ( !haveView() )
                // too early, will return later
                return;

            getPropertyBox()->DisableUpdate();

            sal_Bool bHaveFocus = getPropertyBox()->HasChildPathFocus();

            // create our tab pages
            impl_buildCategories_throw();
            // (and allow for pages to be actually unused)
            ::std::set< sal_uInt16 > aUsedPages;

            // when building the UI below, remember which properties are actuating,
            // to allow for a initial actuatinPropertyChanged call
            ::std::vector< ::rtl::OUString > aActuatingProperties;
            ::std::vector< Any > aActuatingPropertyValues;

            // ask the handlers to describe the property UI, and insert the resulting
            // entries into our list boxes
            OrderedPropertyMap::const_iterator property( m_aProperties.begin() );
            for ( ; property != m_aProperties.end(); ++property )
            {
                OLineDescriptor aDescriptor;
                describePropertyLine( property->second, aDescriptor );

                bool bIsActuatingProperty = impl_isActuatingProperty_nothrow( property->second.Name );

            #if OSL_DEBUG_LEVEL > 0
                if ( !aDescriptor.Category.getLength() )
                {
                    ::rtl::OString sMessage( "OPropertyBrowserController::UpdateUI: empty category provided for property '" );
                    sMessage += ::rtl::OString( property->second.Name.getStr(), property->second.Name.getLength(), osl_getThreadTextEncoding() );
                    sMessage += "'!";
                    OSL_ENSURE( false, sMessage );
                }
            #endif
                // finally insert this property control
                sal_uInt16 nTargetPageId = impl_getPageIdForCategory_nothrow( aDescriptor.Category );
                if ( nTargetPageId == (sal_uInt16)-1 )
                {
                    // this category does not yet exist. This is allowed, as an inspector model might be lazy, and not provide
                    // any category information of its own. In this case, we have a fallback ...
                    m_aPageIds[ aDescriptor.Category ] =
                        getPropertyBox()->AppendPage( aDescriptor.Category, 0 );
                    nTargetPageId = impl_getPageIdForCategory_nothrow( aDescriptor.Category );
                }

                getPropertyBox()->InsertEntry( aDescriptor, nTargetPageId );
                aUsedPages.insert( nTargetPageId );

                // if it's an actuating property, remember it
                if ( bIsActuatingProperty )
                {
                    aActuatingProperties.push_back( property->second.Name );
                    aActuatingPropertyValues.push_back( impl_getPropertyValue_throw( property->second.Name ) );
                }
            }

            // update any dependencies for the actuating properties which we encountered
            {
                ::std::vector< ::rtl::OUString >::const_iterator aProperty = aActuatingProperties.begin();
                ::std::vector< Any >::const_iterator aPropertyValue = aActuatingPropertyValues.begin();
                for ( ; aProperty != aActuatingProperties.end(); ++aProperty, ++aPropertyValue )
                    impl_broadcastPropertyChange_nothrow( *aProperty, *aPropertyValue, *aPropertyValue, true );
            }

            // remove any unused pages (which we did not encounter properties for)
            HashString2Int16 aSurvivingPageIds;
            for (   HashString2Int16::iterator pageId = m_aPageIds.begin();
                    pageId != m_aPageIds.end();
                    ++pageId
                )
            {
                if ( aUsedPages.find( pageId->second ) == aUsedPages.end() )
                    getPropertyBox()->RemovePage( pageId->second );
                else
                    aSurvivingPageIds.insert( *pageId );
            }
            m_aPageIds.swap( aSurvivingPageIds );


            getPropertyBox()->Show();
            getPropertyBox()->EnableUpdate();
            if ( bHaveFocus )
                getPropertyBox()->GrabFocus();

            // activate the first page
            if ( !m_aPageIds.empty() )
            {
                Sequence< PropertyCategoryDescriptor > aCategories( m_xModel->describeCategories() );
                if ( aCategories.getLength() )
                    m_pView->activatePage( m_aPageIds[ aCategories[0].ProgrammaticName ] );
                else
                    // allowed: if we default-created the pages ...
                    m_pView->activatePage( m_aPageIds.begin()->second );
            }

            // activate the previously active page (if possible)
            if ( m_sLastValidPageSelection.getLength() )
                m_sPageSelection = m_sLastValidPageSelection;
            selectPageFromViewData();
        }
        catch( const Exception& e )
        {
        #if OSL_DEBUG_LEVEL > 0
            Any caught( ::cppu::getCaughtException() );

            ::rtl::OString sMessage( "OPropertyBrowserController::UpdateUI: caught an exception!" );
            sMessage += "\ntype: ";
            sMessage += ::rtl::OString( caught.getValueTypeName().getStr(), caught.getValueTypeName().getLength(), osl_getThreadTextEncoding() );
            sMessage += "\nmessage: ";
            sMessage += ::rtl::OString( e.Message.getStr(), e.Message.getLength(), osl_getThreadTextEncoding() );
            OSL_ENSURE( false, sMessage );
        #else
            e; // make compiler happy
        #endif
        }
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::Clicked( const ::rtl::OUString& _rName, sal_Bool _bPrimary )
    {
        try
        {
            // since the browse buttons do not get the focus when clicked with the mouse,
            // we need to commit the changes in the current property field
            getPropertyBox()->CommitModified();

            PropertyHandlerRepository::const_iterator handler = m_aPropertyHandlers.find( _rName );
            DBG_ASSERT( handler != m_aPropertyHandlers.end(), "OPropertyBrowserController::Clicked: a property without handler? This will crash!" );

            ComposedUIAutoFireGuard aAutoFireGuard( *m_pUIRequestComposer );

            Any aData;
            InteractiveSelectionResult eResult =
                handler->second->onInteractivePropertySelection( _rName, _bPrimary, aData,
                    m_pUIRequestComposer->getUIForPropertyHandler( handler->second ) );

            switch ( eResult )
            {
            case InteractiveSelectionResult_Cancelled:
            case InteractiveSelectionResult_Success:
                // okay, nothing to do
                break;
            case InteractiveSelectionResult_ObtainedValue:
                handler->second->setPropertyValue( _rName, aData );
                break;
            case InteractiveSelectionResult_Pending:
                // also okay, we expect that the handler has disabled the UI as necessary
                break;
            }
        }
        catch (Exception&)
        {
            DBG_ERROR("OPropertyBrowserController::Clicked : caught an exception !")
        }
    }

    //------------------------------------------------------------------------
    sal_Bool SAL_CALL OPropertyBrowserController::hasPropertyByName( const ::rtl::OUString& _rName ) throw (RuntimeException)
    {
        for (   OrderedPropertyMap::const_iterator search = m_aProperties.begin();
                search != m_aProperties.end();
                ++search
            )
            if ( search->second.Name == _rName )
                return true;
        return false;
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::Commit( const ::rtl::OUString& rName, const Any& _rValue )
    {
        try
        {
            m_sCommittingProperty = rName;

            bool bIsActuatingProperty = impl_isActuatingProperty_nothrow( rName );

            Any aOldValue;
            if ( bIsActuatingProperty )
                aOldValue = impl_getPropertyValue_throw( rName );

            // do we have a dedicated handler for this property, which we can delegate some tasks to?
            PropertyHandlerRef handler = impl_getHandlerForProperty_throw( rName );

            //////////////////////////////////////////////////////////////////////
            // set the value
            handler->setPropertyValue( rName, _rValue );

            //////////////////////////////////////////////////////////////////////
            // re-retrieve the value
            Any aNormalizedValue = handler->getPropertyValue( rName );

            // care for any inter-property dependencies
            if ( bIsActuatingProperty )
                impl_broadcastPropertyChange_nothrow( rName, aNormalizedValue, aOldValue, false );

            // and display it again. This ensures proper formatting
            getPropertyBox()->SetPropertyValue( rName, aNormalizedValue );
        }
        catch(PropertyVetoException& eVetoException)
        {
            InfoBox(m_pView, eVetoException.Message).Execute();
        }
        catch(Exception&)
        {
            DBG_ERROR("OPropertyBrowserController::Commit : caught an exception !")
        }

        m_sCommittingProperty = ::rtl::OUString();
    }

    //------------------------------------------------------------------------
    namespace
    {
        Reference< XPropertyHandler > lcl_createHandler( const ComponentContext& _rContext, const Any& _rFactoryDescriptor )
        {
            Reference< XPropertyHandler > xHandler;

            ::rtl::OUString sServiceName;
            Reference< XSingleServiceFactory > xServiceFac;
            Reference< XSingleComponentFactory > xComponentFac;

            if ( _rFactoryDescriptor >>= sServiceName )
                _rContext.createComponent( sServiceName, xHandler );
            else if ( _rFactoryDescriptor >>= xServiceFac )
                xHandler = xHandler.query( xServiceFac->createInstance() );
            else if ( _rFactoryDescriptor >>= xComponentFac )
                xHandler = xHandler.query( xComponentFac->createInstanceWithContext( _rContext.getUNOContext() ) );
            return xHandler;
        }
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::getPropertyHandlers( const InterfaceArray& _rObjects, PropertyHandlerArray& _rHandlers )
    {
        _rHandlers.resize( 0 );
        if ( _rObjects.empty() )
            return;

        // create a component context for the handlers, containing some information about where
        // they live
        Reference< XComponentContext > xHandlerContext( m_aContext.getUNOContext() );

        // if our own creator did not pass a dialog parent window, use our own view for this
        Reference< XWindow > xParentWindow( m_aContext.getContextValueByAsciiName( "DialogParentWindow" ), UNO_QUERY );
        if ( !xParentWindow.is() )
        {
            ::cppu::ContextEntry_Init aHandlerContextInfo[] =
            {
                ::cppu::ContextEntry_Init( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DialogParentWindow" ) ), makeAny( VCLUnoHelper::GetInterface( m_pView ) ) )
            };
            xHandlerContext = ::cppu::createComponentContext(
                aHandlerContextInfo, sizeof( aHandlerContextInfo ) / sizeof( aHandlerContextInfo[0] ),
                m_aContext.getUNOContext() );
        }

        Sequence< Any > aHandlerFactories;
        if ( m_xModel.is() )
            aHandlerFactories = m_xModel->getHandlerFactories();

        const Any* pHandlerFactory = aHandlerFactories.getConstArray();
        const Any* pHandlerFactoryEnd = aHandlerFactories.getConstArray() + aHandlerFactories.getLength();

        while ( pHandlerFactory != pHandlerFactoryEnd )
        {
            if ( _rObjects.size() == 1 )
            {   // we're inspecting only one object -> one handler
                Reference< XPropertyHandler > xHandler( lcl_createHandler( m_aContext, *pHandlerFactory ) );
                if ( xHandler.is() )
                {
                    xHandler->inspect( _rObjects[0] );
                    _rHandlers.push_back( xHandler );
                }
            }
            else
            {
                // create a single handler for every single object
                ::std::vector< Reference< XPropertyHandler > > aSingleHandlers( _rObjects.size() );
                ::std::vector< Reference< XPropertyHandler > >::iterator pHandler = aSingleHandlers.begin();

                InterfaceArray::const_iterator pObject = _rObjects.begin();
                InterfaceArray::const_iterator pObjectEnd = _rObjects.end();

                for ( ; pObject != pObjectEnd; ++pObject )
                {
                    *pHandler = lcl_createHandler( m_aContext, *pHandlerFactory );
                    if ( pHandler->is() )
                    {
                        (*pHandler)->inspect( *pObject );
                        ++pHandler;
                    }
                }
                aSingleHandlers.resize( pHandler - aSingleHandlers.begin() );

                // then create a handler which composes information out of those single handlers
                if ( !aSingleHandlers.empty() )
                    _rHandlers.push_back( new PropertyComposer( aSingleHandlers ) );
            }

            ++pHandlerFactory;
        }

        // note that the handlers will not be used by our caller, if they indicate that there are no
        // properties they feel responsible for
    }

    //------------------------------------------------------------------------
    bool OPropertyBrowserController::impl_findObjectProperty_nothrow( const ::rtl::OUString& _rName, OrderedPropertyMap::const_iterator* _pProperty )
    {
        OrderedPropertyMap::const_iterator search = m_aProperties.begin();
        for ( ; search != m_aProperties.end(); ++search )
            if ( search->second.Name == _rName )
                break;
        if ( _pProperty )
            *_pProperty = search;
        return ( search != m_aProperties.end() );
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::rebuildPropertyUI( const ::rtl::OUString& _rPropertyName ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !getPropertyBox() )
            throw RuntimeException();

        OrderedPropertyMap::const_iterator propertyPos;
        if ( !impl_findObjectProperty_nothrow( _rPropertyName, &propertyPos ) )
            return;

        OLineDescriptor aDescriptor;
        bool bSuccess = false;
        try
        {
            describePropertyLine( propertyPos->second, aDescriptor );
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "OPropertyBrowserController::rebuildPropertyUI: caught an exception!" );
        }

        getPropertyBox()->ChangeEntry( aDescriptor );
   }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::enablePropertyUI( const ::rtl::OUString& _rPropertyName, sal_Bool _bEnable ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !getPropertyBox() )
            throw RuntimeException();

        if ( !impl_findObjectProperty_nothrow( _rPropertyName ) )
            return;

        getPropertyBox()->EnablePropertyLine( _rPropertyName, _bEnable );
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::enablePropertyUIElements( const ::rtl::OUString& _rPropertyName, sal_Int16 _nElements, sal_Bool _bEnable ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !getPropertyBox() )
            throw RuntimeException();

        if ( !impl_findObjectProperty_nothrow( _rPropertyName ) )
            return;

        getPropertyBox()->EnablePropertyControls( _rPropertyName, _nElements, _bEnable );
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::showPropertyUI( const ::rtl::OUString& _rPropertyName ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !getPropertyBox() )
            throw RuntimeException();

        // look up the property in our object properties
        OrderedPropertyMap::const_iterator propertyPos;
        if ( !impl_findObjectProperty_nothrow( _rPropertyName, &propertyPos ) )
            return;

        if ( getPropertyBox()->GetPropertyPos( _rPropertyName ) != LISTBOX_ENTRY_NOTFOUND )
        {
            rebuildPropertyUI( _rPropertyName );
            return;
        }

        OLineDescriptor aDescriptor;
        describePropertyLine( propertyPos->second, aDescriptor );

        // look for the position to insert the property

        // side note: The methods GetPropertyPos and InsertEntry of the OPropertyEditor work
        // only on the current page. This implies that it's impossible to use this method here
        // to show property lines which are *not* on the current page.
        // This is sufficient for now, but should be changed in the future.

        // by definition, the properties in m_aProperties are in the order in which they appear in the UI
        // So all we need is a predecessor of pProperty in m_aProperties
        size_t nPosition = propertyPos->first;
        sal_uInt16 nUIPos = LISTBOX_ENTRY_NOTFOUND;
        do
        {
            if ( propertyPos != m_aProperties.begin() )
                --propertyPos;
            nUIPos = getPropertyBox()->GetPropertyPos( propertyPos->second.Name );
        }
        while ( ( nUIPos == LISTBOX_ENTRY_NOTFOUND ) && ( propertyPos != m_aProperties.begin() ) );

        if ( nUIPos == LISTBOX_ENTRY_NOTFOUND )
            // insert at the very top
            nUIPos = 0;
        else
            // insert right after the predecessor we found
            ++nUIPos;

        getPropertyBox()->InsertEntry(
            aDescriptor, impl_getPageIdForCategory_nothrow( aDescriptor.Category ), nUIPos );
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::hidePropertyUI( const ::rtl::OUString& _rPropertyName ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !getPropertyBox() )
            throw RuntimeException();

        if ( !impl_findObjectProperty_nothrow( _rPropertyName ) )
            return;

        getPropertyBox()->RemoveEntry( _rPropertyName );
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::showCategory( const ::rtl::OUString& _rCategory, sal_Bool _bShow ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !getPropertyBox() )
            throw RuntimeException();

        sal_uInt16 nPageId = impl_getPageIdForCategory_nothrow( _rCategory );
        OSL_ENSURE( nPageId != (sal_uInt16)-1, "OPropertyBrowserController::showCategory: invalid category!" );

        getPropertyBox()->ShowPropertyPage( nPageId, _bShow );
    }

    //------------------------------------------------------------------------
    Reference< XPropertyControl > SAL_CALL OPropertyBrowserController::getPropertyControl( const ::rtl::OUString& _rPropertyName ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !getPropertyBox() )
            throw RuntimeException();

        Reference< XPropertyControl > xControl;
        if ( getPropertyBox() )
            xControl = getPropertyBox()->GetPropertyControl( _rPropertyName );
        return xControl;
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::impl_broadcastPropertyChange_nothrow( const ::rtl::OUString& _rPropertyName, const Any& _rNewValue, const Any& _rOldValue, bool _bFirstTimeInit ) const
    {
        // are there one or more handlers which are interested in the actuation?
        ::std::pair< PropertyHandlerMultiRepository::const_iterator, PropertyHandlerMultiRepository::const_iterator > aInterestedHandlers =
            m_aDependencyHandlers.equal_range( _rPropertyName );
        if ( aInterestedHandlers.first == aInterestedHandlers.second )
            // none of our handlers is interested in this
            return;

        ComposedUIAutoFireGuard aAutoFireGuard( *m_pUIRequestComposer );
        try
        {
            // collect the responses from all interested handlers
            PropertyHandlerMultiRepository::const_iterator handler = aInterestedHandlers.first;
            while ( handler != aInterestedHandlers.second )
            {
                handler->second->actuatingPropertyChanged( _rPropertyName, _rNewValue, _rOldValue,
                    m_pUIRequestComposer->getUIForPropertyHandler( handler->second ),
                    _bFirstTimeInit );
                ++handler;
            }
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "OPropertyBrowserController::impl_broadcastPropertyChange_nothrow: caught an exception while calling the handlers!" );
        }
    }

//............................................................................
} // namespace pcr
//............................................................................


