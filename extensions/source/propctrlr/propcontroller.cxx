/*************************************************************************
 *
 *  $RCSfile: propcontroller.cxx,v $
 *
 *  $Revision: 1.23 $
 *
 *  last change: $Author: svesik $ $Date: 2004-04-21 14:20:29 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/
#ifndef _EXTENSIONS_PROPCTRLR_PROPCONTROLLER_HXX_
#include "propcontroller.hxx"
#endif
#ifndef _EXTENSIONS_FORMSCTRLR_PCRSTRINGS_HXX_
#include "pcrstrings.hxx"
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef _CPPUHELPER_TYPEPROVIDER_HXX_
#include <cppuhelper/typeprovider.hxx>
#endif
#ifndef _EXTENSIONS_PROPCTRLR_PROPERTYEDITOR_HXX_
#include "propertyeditor.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_PROPRESID_HRC_
#include "propresid.hrc"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_PROPHELPID_HRC_
#include "prophelpid.hrc"
#endif
#ifndef _COM_SUN_STAR_BEANS_XINTROSPECTION_HPP_
#include <com/sun/star/beans/XIntrospection.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYCONCEPT_HPP_
#include <com/sun/star/beans/PropertyConcept.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCHILD_HPP_
#include <com/sun/star/container/XChild.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XFORM_HPP_
#include <com/sun/star/form/XForm.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XFORMCOMPONENT_HPP_
#include <com/sun/star/form/XFormComponent.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XGRIDCOLUMNFACTORY_HPP_
#include <com/sun/star/form/XGridColumnFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XFORMCONTROLLER_HPP_
#include <com/sun/star/form/XFormController.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XMODIFIABLE_HPP_
#include <com/sun/star/util/XModifiable.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XCLOSEABLE_HPP_
#include <com/sun/star/util/XCloseable.hpp>
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
#ifndef _EXTENSIONS_PROPCTRLR_MODULEPRC_HXX_
#include "modulepcr.hxx"
#endif
#ifndef _EXTENSIONS_FORMSCTRLR_FORMSTRINGS_HXX_
#include "formstrings.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_FORMMETADATA_HXX_
#include "formmetadata.hxx"
#endif
#include <algorithm>

//------------------------------------------------------------------------
// !!! outside the namespace !!!
extern "C" void SAL_CALL createRegistryInfo_OPropertyBrowserController()
{
    static ::pcr::OMultiInstanceAutoRegistration< ::pcr::OPropertyBrowserController > aAutoRegistration;
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
    using namespace ::comphelper;

#define THISREF()   static_cast< XController* >(this)

    //========================================================================
    //= OPropertyBrowserController
    //========================================================================
    DBG_NAME(OPropertyBrowserController)
    //------------------------------------------------------------------------
    OPropertyBrowserController::OPropertyBrowserController(const Reference< XMultiServiceFactory >& _rxORB)
            :OPropertyChangeListener(m_aMutex)
            ,OPropertyBrowserController_PropertyBase1(m_aBHelper)
            ,m_xORB(_rxORB)
            ,m_aDisposeListeners(m_aMutex)
            ,m_pPropertyInfo(NULL)
            ,m_pChangeMultiplexer(NULL)
            ,m_pView(NULL)
            ,m_sStandard(ModuleRes(RID_STR_STANDARD))
            ,m_nGenericPageId(0)
            ,m_nDataPageId(0)
            ,m_nEventPageId(0)
            ,m_bHasListSource( sal_False )
            ,m_bHasCursorSource( sal_False )
            ,m_bContainerFocusListening(sal_False)
            ,m_bSuspendingDependentComp( sal_False )
            ,m_bInspectingSubForm( sal_False )
    {
        DBG_CTOR(OPropertyBrowserController,NULL);

        if (m_xORB.is())
        {
            m_xTypeConverter = Reference< XTypeConverter >(
                m_xORB->createInstance(::rtl::OUString::createFromAscii("com.sun.star.script.Converter")),
                UNO_QUERY
            );
            DBG_ASSERT(m_xTypeConverter.is(), "OPropertyBrowserController::OPropertyBrowserController: could not instantiate the type converter service!");
            // TODO: perhaps an exception
        }

        initFormStuff();

        registerProperty(PROPERTY_INTROSPECTEDOBJECT, OWN_PROPERTY_ID_INTROSPECTEDOBJECT,
            PropertyAttribute::BOUND | PropertyAttribute::TRANSIENT | PropertyAttribute::CONSTRAINED,
            &m_xIntrospecteeAsProperty, ::getCppuType(&m_xIntrospecteeAsProperty));

        registerProperty(PROPERTY_CURRENTPAGE, OWN_PROPERTY_ID_CURRENTPAGE,
            PropertyAttribute::BOUND | PropertyAttribute::TRANSIENT,
            &m_sPageSelection, ::getCppuType(&m_sPageSelection));

        registerProperty( PROPERTY_CONTROLCONTEXT, OWN_PROPERTY_ID_CONTROLCONTEXT,
            PropertyAttribute::BOUND | PropertyAttribute::TRANSIENT,
            &m_xControlsView, ::getCppuType( &m_xControlsView ) );
    }

    //------------------------------------------------------------------------
    OPropertyBrowserController::~OPropertyBrowserController()
    {
        deinitFormStuff();
        // stop listening for property changes
        stopIntrospection();
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

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::attachFrame( const Reference< XFrame >& _rxFrame ) throw(RuntimeException)
    {
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

        if (Construct(pParentWin))
            m_xFrame->setComponent(VCLUnoHelper::GetInterface(m_pView), this);

        startContainerWindowListening();
    }

    //------------------------------------------------------------------------
    sal_Bool SAL_CALL OPropertyBrowserController::attachModel( const Reference< XModel >& xModel ) throw(RuntimeException)
    {
        DBG_ERROR("OPropertyBrowserController::attachModel: models not supported!");
        return sal_False;
    }

    //------------------------------------------------------------------------
    sal_Bool SAL_CALL OPropertyBrowserController::suspend( sal_Bool _bSuspend ) throw(RuntimeException)
    {
        OSL_ENSURE( haveView(), "OPropertyBrowserController::suspend: don't have a view anymore!" );
        OSL_ENSURE( NULL != getPropertyBox(), "OPropertyBrowserController::suspend: don't have a property box anymore!" );

        if ( !_bSuspend )
        {   // this means a "suspend" is to be "revoked"
            if ( m_xDependentComponent.is() )
                m_xDependentComponent->suspend( sal_False );

            return sal_False;   // we ourself cannot revoke our suspend
        }

        if ( m_xDependentComponent.is() )
        {
            if ( !suspendDependentComponent( ) )
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
        // have no view data
        return Any();
    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::restoreViewData( const Any& Data ) throw(RuntimeException)
    {
        // have no view data
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
    Any SAL_CALL OPropertyBrowserController::queryInterface( const Type& _rType ) throw(RuntimeException)
    {
        Any aReturn = OPropertyBrowserController_Base::queryInterface(_rType);
        if (!aReturn.hasValue())
            aReturn = OPropertyBrowserController_PropertyBase1::queryInterface(_rType);
        return aReturn;
    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::acquire(  ) throw()
    {
        OPropertyBrowserController_Base::acquire();
    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::release(  ) throw()
    {
        OPropertyBrowserController_Base::release();
    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::dispose(  ) throw(RuntimeException)
    {
        // say our dispose listeners goodbye
        ::com::sun::star::lang::EventObject aEvt;
        aEvt.Source = static_cast< ::cppu::OWeakObject* >(this);
        m_aDisposeListeners.disposeAndClear(aEvt);

        // close our dependent component, if necessary
        if ( m_xDependentComponent.is() )
            closeDependentComponent();

        if (haveView())
            m_pView->setActiveController(NULL);

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
        return getImplementationName_Static();
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
        return getSupportedServiceNames_Static();
    }

    //------------------------------------------------------------------------
    ::rtl::OUString OPropertyBrowserController::getImplementationName_Static(  ) throw(RuntimeException)
    {
        return ::rtl::OUString::createFromAscii("org.openoffice.comp.form.ui.OPropertyBrowserController");
    }

    //------------------------------------------------------------------------
    Sequence< ::rtl::OUString > OPropertyBrowserController::getSupportedServiceNames_Static(  ) throw(RuntimeException)
    {
        Sequence< ::rtl::OUString > aSupported(1);
        aSupported[0] = ::rtl::OUString::createFromAscii("com.sun.star.form.PropertyBrowserController");
        return aSupported;
    }

    //------------------------------------------------------------------------
    Reference< XInterface > SAL_CALL OPropertyBrowserController::Create(const Reference< XMultiServiceFactory >& _rxORB)
    {
        return static_cast<XComponent*>(new OPropertyBrowserController(_rxORB));
    }

    //------------------------------------------------------------------------
    Sequence< Type > SAL_CALL OPropertyBrowserController::getTypes(  ) throw(RuntimeException)
    {
        static ::cppu::OTypeCollection aTypes(
            ::getCppuType( static_cast< Reference< XPropertySet >* >(NULL) ),
            ::getCppuType( static_cast< Reference< XMultiPropertySet >* >(NULL) ),
            ::getCppuType( static_cast< Reference< XFastPropertySet >* >(NULL) ),
            OPropertyBrowserController_Base::getTypes());
        return aTypes.getTypes();
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
            if (getPropertyBox())
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
        Reference< XWindow > xSourceWindow(_rSource.Source, UNO_QUERY);
        if (xSourceWindow.get() == m_xView.get())
        {
            m_xView = NULL;
            m_pView = NULL;
        }
        else if ( m_xDependentComponent.is() && ( m_xDependentComponent == _rSource.Source ) )
        {
            m_xDependentComponent = NULL;
            dependentComponentClosed();
        }
#ifdef DBG_UTIL
        else
        {
            Reference< XWindow > xContainer;
            if (m_xFrame.is())
                xContainer = m_xFrame->getContainerWindow();
            DBG_ASSERT(xContainer.get() == xSourceWindow.get(), "OPropertyBrowserController::disposing: where does this come from?");
        }
#endif
    }

    //------------------------------------------------------------------------
    Sequence< sal_Int8 > SAL_CALL OPropertyBrowserController::getImplementationId(  ) throw(RuntimeException)
    {
        static ::cppu::OImplementationId aId;
        return aId.getImplementationId();
    }

    //------------------------------------------------------------------------
    Reference< XPropertySetInfo > SAL_CALL OPropertyBrowserController::getPropertySetInfo(  ) throw(RuntimeException)
    {
        return ::cppu::OPropertySetHelper::createPropertySetInfo(getInfoHelper());
    }

    //------------------------------------------------------------------------
    ::cppu::IPropertyArrayHelper& SAL_CALL OPropertyBrowserController::getInfoHelper()
    {
        return *getArrayHelper();
    }

    //------------------------------------------------------------------------
    ::cppu::IPropertyArrayHelper* OPropertyBrowserController::createArrayHelper( ) const
    {
        Sequence< Property > aProps;
        describeProperties(aProps);
        return new cppu::OPropertyArrayHelper(aProps);
    }

    //------------------------------------------------------------------------
    IMPL_LINK(OPropertyBrowserController, OnPageActivation, void*, EMPTYARG)
    {
        syncViewToProperty();
        return 0L;
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::syncViewToProperty()
    {
        if (!haveView())
            return;

        ::rtl::OUString sOldSelection = m_sPageSelection;

        m_sPageSelection = ::rtl::OUString();

        const sal_uInt16 nCurrentPage = m_pView->getActivaPage();
        if ((sal_uInt16)-1 != nCurrentPage)
            if (nCurrentPage == m_nGenericPageId)
                m_sPageSelection = ::rtl::OUString::createFromAscii("Generic");
            else if (nCurrentPage == m_nDataPageId)
                m_sPageSelection = ::rtl::OUString::createFromAscii("Data");
            else if (nCurrentPage == m_nEventPageId)
                m_sPageSelection = ::rtl::OUString::createFromAscii("Events");

        if (sOldSelection != m_sPageSelection)
        {   // fire the property change
            Any aOldValue; aOldValue <<= sOldSelection;
            Any aNewValue; aNewValue <<= m_sPageSelection;
            sal_Int32 nHandle = OWN_PROPERTY_ID_CURRENTPAGE;
            fire(&nHandle, &aNewValue, &aOldValue, 1, sal_False);
        }
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::syncPropertyToView()
    {
        sal_uInt16 nNewPage = (sal_uInt16)-1;
        if (0 == m_sPageSelection.compareToAscii("Generic"))
            nNewPage = m_nGenericPageId;
        else if (0 == m_sPageSelection.compareToAscii("Data"))
            nNewPage = m_nDataPageId;
        else if (0 == m_sPageSelection.compareToAscii("Events"))
            nNewPage = m_nEventPageId;

        if (haveView())
            m_pView->activatePage(nNewPage);

        // just in case ...
        syncViewToProperty();
    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::setFastPropertyValue_NoBroadcast(sal_Int32 _nHandle, const Any& _rValue) throw (Exception)
    {
        if ( OWN_PROPERTY_ID_INTROSPECTEDOBJECT == _nHandle )
        {
            if ( m_xDependentComponent.is() )
                if ( m_bSuspendingDependentComp || !suspendDependentComponent() )
                {   // we already are trying to suspend the component (this is somewhere up the stack)
                    // OR our dependent component raised a veto against closing it. Well, we *need* to close
                    // it in order to inspec another object
                    throw PropertyVetoException();
                }
        }

        OPropertyBrowserController_PropertyBase1::setFastPropertyValue_NoBroadcast(_nHandle, _rValue);

        switch (_nHandle)
        {
            case OWN_PROPERTY_ID_INTROSPECTEDOBJECT:
                // it was my introspectee
                bindToObject(m_xIntrospecteeAsProperty);
                break;

            case OWN_PROPERTY_ID_CURRENTPAGE:
                syncPropertyToView();
                break;
        }
    }

    //------------------------------------------------------------------------
    sal_Bool OPropertyBrowserController::Construct(Window* _pParentWin)
    {
        DBG_ASSERT(!haveView(), "OPropertyBrowserController::Construct: already have a view!");
        DBG_ASSERT(_pParentWin, "OPropertyBrowserController::Construct: invalid parent window!");

        m_pView = new OPropertyBrowserView(m_xORB, _pParentWin);
        m_pView->setActiveController(this);
        m_pView->setPageActivationHandler(LINK(this, OPropertyBrowserController, OnPageActivation));

        // add as dispose listener for our view. The view is disposed by the frame we're plugged into,
        // and this disposal _deletes_ the view, so it would be deadly if we use our m_pView member
        // after that
        m_xView = VCLUnoHelper::GetInterface(m_pView);
        Reference< XComponent > xViewAsComp(m_xView, UNO_QUERY);
        if (xViewAsComp.is())
            xViewAsComp->addEventListener( static_cast< XPropertyChangeListener* >( this ) );

        if (haveView())
            getPropertyBox()->SetLineListener(this);
        return sal_True;
    }

    //------------------------------------------------------------------------
    void SAL_CALL OPropertyBrowserController::propertyChange( const PropertyChangeEvent& _rEvent ) throw (RuntimeException)
    {
        if ( _rEvent.Source == m_xDependentComponent )
        {
            if ( PROPERTY_ACTIVECOMMAND == _rEvent.PropertyName )
            {
                OSL_ENSURE( _rEvent.NewValue.getValueTypeClass() == TypeClass_STRING,
                    "OPropertyBrowserController::propertyChange: invalid new value for the ActiveCommand!" );
                m_xPropValueAccess->setPropertyValue( PROPERTY_COMMAND, _rEvent.NewValue );
            }
        }
#if OSL_DEBUG_LEVEL > 0
        else
            OSL_ENSURE( sal_False, "OPropertyBrowserController::propertyChange: where did this come from?" );
#endif
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::_propertyChanged(const PropertyChangeEvent& _rEvent) throw( RuntimeException)
    {
        if ( !haveView() )
            return;
        Property aProp = getIntrospecteeProperty( _rEvent.PropertyName );
        if ( aProp.Name.getLength() )
        {
            DBG_ASSERT( aProp.Name == _rEvent.PropertyName, "OPropertyBrowserController::_propertyChanged: getIntrospecteeProperty returned nonsense!" );

            sal_Int32 nPropId = m_pPropertyInfo->getPropertyId( _rEvent.PropertyName );

            // (possibly) a small innocent change before
            Any aNewValue( _rEvent.NewValue );
            fakePropertyValue( aNewValue, nPropId );

            // forward the new value to the property box, to reflect the change in the UI
            ::rtl::OUString sNewValue = getStringRepFromPropertyValue( aNewValue, nPropId );
            getPropertyBox()->SetPropertyValue( _rEvent.PropertyName, sNewValue );

            // if it's a actuating property, then update the UI for any dependent
            // properties
            bool bIsActuatingProperty = ( m_pPropertyInfo->getPropertyUIFlags( nPropId ) & PROP_FLAG_ACTUATING ) != 0;
            if ( bIsActuatingProperty )
                updateDependentProperties( nPropId, aNewValue );
        }
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::_disposing(const EventObject& _rSource) throw( RuntimeException)
    {
#ifdef DBG_UTIL
        Reference< XInterface > xIntrospectee;
        ::cppu::extractInterface(xIntrospectee, m_aIntrospectee);
        DBG_ASSERT( Reference< XInterface >(_rSource.Source, UNO_QUERY).get() == xIntrospectee.get(),
                "OPropertyBrowserController::_disposing: where does this come from?");
#endif
        bindToObject(Reference< XPropertySet >());
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::startPropertyListening()
    {
        DBG_ASSERT(!isListening(), "OPropertyBrowserController::startPropertyListening: already listening!");
        if (!isListening() && m_xPropValueAccess.is())
        {
            m_pChangeMultiplexer = new OPropertyChangeMultiplexer(this, m_xPropValueAccess);
            m_pChangeMultiplexer->acquire();
            m_pChangeMultiplexer->addProperty(::rtl::OUString());
        }
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::stopPropertyListening()
    {
        DBG_ASSERT(isListening(), "OPropertyBrowserController::stopPropertyListening: not listening currently!");
        if (isListening())
        {
            m_pChangeMultiplexer->dispose();
            m_pChangeMultiplexer->release();
            m_pChangeMultiplexer = NULL;
        }
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::stopIntrospection()
    {
        // stop listening for property changes
        if (isListening())
            stopPropertyListening();

        // destroy the view first. So any pending commits can be done ...
        if (haveView())
        {
            // remove the pages
            if (m_nGenericPageId)
                getPropertyBox()->RemovePage(m_nDataPageId);
            if (m_nDataPageId)
                getPropertyBox()->RemovePage(m_nDataPageId);
            if (m_nEventPageId)
                getPropertyBox()->RemovePage(m_nEventPageId);
            m_nGenericPageId = m_nDataPageId = m_nEventPageId = 0;
        }

        m_aIntrospectee.clear();
        m_aObjectProperties.realloc(0);
        m_aObjectListenerTypes.realloc(0);
        m_xEventManager = NULL;
        m_xPropStateAccess = NULL;
        m_xPropValueAccess = NULL;
        m_xIntrospection = NULL;
        m_xObjectParent = NULL;
        m_nClassId = 0;

        m_bHasListSource = m_bHasCursorSource =  sal_False;

        // and some heavily form specific stuff, again
        cleanupRowsetConnection();
    }

    //------------------------------------------------------------------------
    Any OPropertyBrowserController::GetUnoPropertyValue( const ::rtl::OUString& _rPropName, bool _bCheckExistence )
    {
        Any aValue;
        try
        {
            if ( _rPropName.getLength() )
            {
                sal_Int32   nPropId = m_pPropertyInfo->getPropertyId( _rPropName );
                sal_uInt32  nPropertyUIFlags = m_pPropertyInfo->getPropertyUIFlags( nPropId );
                bool bIsVirtualProperty = ( nPropertyUIFlags & PROP_FLAG_VIRTUAL_PROP ) != 0;

                if ( !bIsVirtualProperty )
                {
                    if ( m_xPropValueAccess.is() )
                    {
                        bool bDoGetValue = true;
                        if ( _bCheckExistence )
                        {
                            Reference< XPropertySetInfo > xPropInfo = m_xPropValueAccess->getPropertySetInfo();
                            if ( !xPropInfo.is() || !xPropInfo->hasPropertyByName( _rPropName ) )
                                bDoGetValue = false;
                        }

                        if ( bDoGetValue )
                        {
                            aValue = m_xPropValueAccess->getPropertyValue( _rPropName );
                            fakePropertyValue( aValue, nPropId );
                        }
                    }
                }
                else
                {
                    aValue = getVirtualPropertyValue( nPropId );
                }
            }
        }

        catch (Exception&)
        {
            DBG_ERROR("OPropertyBrowserController::GetUnoPropertyValue: caught an exception !");
        }

        return aValue;
    }

    //------------------------------------------------------------------------
    ::rtl::OUString OPropertyBrowserController::GetPropertyValue( const ::rtl::OUString& _rPropName )
    {
        return getStringRepFromPropertyValue(
            GetUnoPropertyValue( _rPropName ),
            m_pPropertyInfo->getPropertyId( _rPropName )
        );
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::bindToObject(const Reference< XPropertySet >& _rxObject)
    {
        try
        {
            if ( m_xDependentComponent.is() )
                closeDependentComponent();

            if ( haveView() && getPropertyBox() )
            {
                // commit the editor's content
                getPropertyBox()->CommitModified();

                // hide the property box so that it does not flicker
                getPropertyBox()->Hide();

                // clear the property box
                getPropertyBox()->ClearAll();
            }

            // stop inspecting the old object
            stopIntrospection();

            // TODO: notify the listeners that our object has been reset (to NULL, for the moment)
            // external instances may want to adjust the title to this new situation

            Reference< XForm >  xForm(_rxObject, UNO_QUERY);
            Reference< XFormComponent >  xControl(_rxObject, UNO_QUERY);

            Reference< XEventAttacherManager >  xEvManager;
            if (haveView())
                m_pView->SetHelpId(HID_FM_DLG_PROP_CONTROL);

            Any aAdditionalEvents;
            m_bInspectingSubForm = sal_False;

            if (xForm.is())
            {
                // check whether we're talking about a sub form here
                Reference< XChild > xFormAsChild( xForm, UNO_QUERY );
                Reference< XForm > xFormsParent;
                if ( xFormAsChild.is() )
                    xFormsParent = xFormsParent.query( xFormAsChild->getParent() );
                m_bInspectingSubForm = xFormsParent.is();

                // it's a form. Create a (temporary) form controller for the additional events
                Reference< XFormController >  xController(m_xORB->createInstance(SERVICE_FORMCONTROLLER), UNO_QUERY);
                xController->setModel(Reference< ::com::sun::star::awt::XTabControllerModel > (xForm,UNO_QUERY));
                aAdditionalEvents <<= xController;

                // set the new object, do the inspections
                setObject(makeAny(xForm), aAdditionalEvents);
                if (haveView())
                    m_pView->SetHelpId(HID_FM_DLG_PROP_FORM);

                // now we do not need the controller anymore, it is just a helper
                ::comphelper::disposeComponent(xController);
            }
            else if (xControl.is())
            {
                if (haveView())
                    m_pView->SetHelpId(HID_FM_DLG_PROP_CONTROL);
                setObject(makeAny(xControl), aAdditionalEvents);
            }
            else
            {   // perhaps it's a grid column
                Reference< XGridColumnFactory >  xGrid;
                Reference< XChild >  xChild(_rxObject, UNO_QUERY);

                if (xChild.is())
                    xGrid = Reference< XGridColumnFactory > (xChild->getParent(),UNO_QUERY);

                if (xGrid.is())
                {
                    if (haveView())
                        m_pView->SetHelpId(HID_FM_DLG_PROP_GRIDCTR);
                    setObject(makeAny(_rxObject), aAdditionalEvents);
                }
                else
                    // it's something else
                    setObject(makeAny(_rxObject), aAdditionalEvents);
            }

            // update the user interface
            if (haveObject())
                UpdateUI();

            // show the property box, again
            if (haveView())
            {
                // activate a default page
                if (m_nGenericPageId)
                    getPropertyBox()->SetPage( m_nGenericPageId );
                else if (m_nDataPageId)
                    getPropertyBox()->SetPage( m_nDataPageId );
                else if (m_nEventPageId)
                    getPropertyBox()->SetPage( m_nEventPageId );
                syncViewToProperty();

                getPropertyBox()->Show();
                // activate the old page
                syncPropertyToView();
            }
        }

        catch(Exception&)
        {
            DBG_ERROR("OPropertyBrowserController::bindToObject: caught an exception !");
        }
    }

    //------------------------------------------------------------------------
    sal_Bool OPropertyBrowserController::setObject(const ::com::sun::star::uno::Any& _rIntrospectee, const ::com::sun::star::uno::Any& _rControl)
    {
        try
        {
            //////////////////////////////////////////////////////////////////////
            // get the introspection service
            Reference< XIntrospection >  xIntrospection(m_xORB->createInstance(::rtl::OUString::createFromAscii("com.sun.star.beans.Introspection")), UNO_QUERY);
            if( !xIntrospection.is())
                return sal_False;

            //////////////////////////////////////////////////////////////////////
            // inspect the object
            m_xIntrospection = xIntrospection->inspect( _rIntrospectee );
            if( !m_xIntrospection.is() )
                return sal_False;

            //////////////////////////////////////////////////////////////////////
            // remember the object
            m_aIntrospectee = _rIntrospectee;

            //////////////////////////////////////////////////////////////////////
            // the other interfaces to the object or some of it's relatives
            ::cppu::extractInterface(m_xPropValueAccess, m_aIntrospectee);
            m_xPropStateAccess = Reference< XPropertyState >::query(m_xPropValueAccess);

            Reference< XChild > xChild(m_xPropValueAccess, UNO_QUERY);
            if(xChild.is())
            {
                m_xObjectParent = Reference< XInterface >(xChild->getParent());
                m_xEventManager = Reference< XEventAttacherManager > (m_xObjectParent, UNO_QUERY);
            }

            //////////////////////////////////////////////////////////////////////
            // tell our property box we're the controller
            if (haveView())
            {
                // add the page for the default properties
                m_nGenericPageId = getPropertyBox()->AppendPage(
                        String(ModuleRes(RID_STR_PROPPAGE_DEFAULT)),
                        HID_FM_PROPDLG_TAB_GENERAL);
            }

            //////////////////////////////////////////////////////////////////////
            // the real properties
            Sequence< Property > aProperties(m_xIntrospection->getProperties(PropertyConcept::ALL));

            // the virtual properties
            Sequence< Property > aVirtualProps;
            if ( describeVirtualProperties( aVirtualProps ) )
            {
                sal_Int32 nOldPropCount = aProperties.getLength();
                aProperties.realloc( nOldPropCount + aVirtualProps.getLength() );
                ::std::copy(
                    aVirtualProps.getConstArray(),
                    aVirtualProps.getConstArray() + aVirtualProps.getLength(),
                    aProperties.getArray() + nOldPropCount
                );
            }

            // sort them by relative pos, which is done by inserting into a map
            DECLARE_STL_STDKEY_MAP( sal_Int32, Property, OrderedPropertyMap );
            OrderedPropertyMap aSortProperties;
            const Property* pSourceProps = aProperties.getConstArray();
            const Property* pSourcePropsEnd = pSourceProps + aProperties.getLength();
            for (; pSourceProps < pSourcePropsEnd; ++pSourceProps)
            {
                sal_Int32 nRelativePosition = m_pPropertyInfo->getPropertyPos(m_pPropertyInfo->getPropertyId(pSourceProps->Name));
                aSortProperties[nRelativePosition] = *pSourceProps;
            }

            // and copy them into the sequence, now that they're sorted
            m_aObjectProperties.realloc(aSortProperties.size());
            ::std::transform(
                aSortProperties.begin(),
                aSortProperties.end(),
                m_aObjectProperties.getArray(),
                ::std::select2nd< OrderedPropertyMap::value_type >()
            );

            //////////////////////////////////////////////////////////////////////
            // get the model and the control listeners
            Sequence< Type >    aModelListeners;
            Sequence< Type >    aControlListeners;

            aModelListeners = m_xIntrospection->getSupportedListeners();

            // if we don't have a control, try to create one (temporarily)
            Reference< XInterface > xTemporaryControl;
            Any aControl(_rControl);
            if (!aControl.hasValue())
            {
                try
                {
                    ::rtl::OUString sControlService;
                    if (m_xPropValueAccess.is())
                        m_xPropValueAccess->getPropertyValue(PROPERTY_DEFAULTCONTROL) >>= sControlService;

                    xTemporaryControl = m_xORB->createInstance(sControlService);
                    aControl <<= xTemporaryControl;
                }
                catch(Exception&)
                {
                }
            }

            // inspect the control for listeners
            if (aControl.hasValue())
            {
                Reference< XIntrospection >  xMVCIntrospection(m_xORB->createInstance(::rtl::OUString::createFromAscii("com.sun.star.beans.Introspection")), UNO_QUERY);
                Reference< XIntrospectionAccess >  xAccess;
                if( xMVCIntrospection.is() )
                    xAccess = xMVCIntrospection->inspect(aControl);
                if (xAccess.is())
                    aControlListeners = xAccess->getSupportedListeners();
            }
            // dispose the temporary control
            if (xTemporaryControl.is())
            {
                ::comphelper::disposeComponent(xTemporaryControl);
                xTemporaryControl = NULL;
            }

            // merge the two lists
            // we use a set for this to avoid duplicates
            DECLARE_STL_SET( Type, TypeLessByName, TypeBag );
            TypeBag aListenerCollection;

            // insert the model listeners
            const Type* pListenerLoop = aModelListeners.getConstArray();
            const Type* pListenerLoopEnd = pListenerLoop + aModelListeners.getLength();
            for (; pListenerLoop != pListenerLoopEnd; ++pListenerLoop)
                aListenerCollection.insert(*pListenerLoop);

            // insert the control listener
            pListenerLoop = aControlListeners.getConstArray();
            pListenerLoopEnd = pListenerLoop + aControlListeners.getLength();
            for (; pListenerLoop != pListenerLoopEnd; ++pListenerLoop)
                if (aListenerCollection.end() == aListenerCollection.find(*pListenerLoop))
                    aListenerCollection.insert(*pListenerLoop);

            // now that they're disambiguated, copy these types into our member
            m_aObjectListenerTypes.realloc(aListenerCollection.size());
            {
                Type* aCopyDest = m_aObjectListenerTypes.getArray();
                for (   ConstTypeBagIterator aCopySource = aListenerCollection.begin();
                        aCopySource != aListenerCollection.end();
                        ++aCopySource, ++aCopyDest
                    )
                    *aCopyDest = *aCopySource;
            }

            // retrieve the class id of the introspectee (if applicable)
            classifyControlModel( );

            // start the listening for property changes
            startPropertyListening();
        }
        catch(Exception&)
        {
            DBG_ERROR("OPropertyBrowserController::setObject : caught an exception !");
            return sal_False;
        }

        // append the data page for the
        if (haveView())
            m_nDataPageId = getPropertyBox()->AppendPage(
                String(ModuleRes(RID_STR_PROPPAGE_DATA)),
                HID_FM_PROPDLG_TAB_DATA
            );

        return sal_True;
    }

    //------------------------------------------------------------------------
    sal_Int32 OPropertyBrowserController::GetStringPos( const String& _rEntry, const ::std::vector< String >& _rEntries)
    {
        ::std::vector< String >::const_iterator pStart = _rEntries.begin();
        ::std::vector< String >::const_iterator pEnd = _rEntries.end();
        for ( ::std::vector< String >::const_iterator pEntries = pStart; pEntries != pEnd; ++pEntries )
        {
            if ( _rEntry == *pEntries )
                return pEntries - pStart;
        }
        return -1;
    }

    //------------------------------------------------------------------------
    Property OPropertyBrowserController::getIntrospecteeProperty( const ::rtl::OUString& _rName )
    {
        const Property* pStart = m_aObjectProperties.getConstArray();
        const Property* pEnd = pStart + m_aObjectProperties.getLength();
        for (const Property* pLoop = pStart; pLoop != pEnd; ++pLoop)
        {
            if (pLoop->Name == _rName)
                return *pLoop;
        }

        return Property();
    }

    //------------------------------------------------------------------------
    ::rtl::OUString OPropertyBrowserController::convertSimpleToString(const Any& _rValue)
    {
        ::rtl::OUString sReturn;
        if (m_xTypeConverter.is())
        {
            // TODO: using the type converter every time is somewhat expensive, isn't it?
            try
            {
                Any aConvertedToString;
                aConvertedToString = m_xTypeConverter->convertToSimpleType(_rValue, TypeClass_STRING);
                aConvertedToString >>= sReturn;
            }
            catch(CannotConvertException&) { }
            catch(IllegalArgumentException&) { }
        }
        return sReturn;
    }

    //------------------------------------------------------------------------
    sal_Int16 OPropertyBrowserController::getControlType() const
    {
        sal_Int16 nControlType = CONTROL_TYPE_UNKNOWN;

        if ( m_xIntrospecteeAsProperty.is() )
        {
            Reference< XPropertySetInfo > xPropInfo = m_xIntrospecteeAsProperty->getPropertySetInfo();

            if ( xPropInfo.is() )
            {
                if ( xPropInfo->hasPropertyByName( PROPERTY_WIDTH ) &&
                     xPropInfo->hasPropertyByName( PROPERTY_HEIGHT ) &&
                     xPropInfo->hasPropertyByName( PROPERTY_POSITIONX ) &&
                     xPropInfo->hasPropertyByName( PROPERTY_POSITIONY ) )
                {
                    nControlType = CONTROL_TYPE_DIALOG;
                }
                else
                {
                    nControlType = CONTROL_TYPE_FORM;
                }
            }
        }
        return nControlType;
    }

    //------------------------------------------------------------------------
    void OPropertyBrowserController::setDocumentModified( )
    {
        Reference< XModifiable > xModifiable( getDocumentModel(), UNO_QUERY );
        if ( xModifiable.is() )
            xModifiable->setModified( sal_True );
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
    Window* OPropertyBrowserController::getDialogParent()
    {
        Window* pParent = m_pView;
        if ( !pParent )
            return NULL;

        while ( pParent->GetParent() )
            pParent = pParent->GetParent();

        return pParent;
    }

    //------------------------------------------------------------------------
    Reference< XModel > OPropertyBrowserController::getDocumentModel() const
    {
        Reference< XChild > xChild;
        m_aIntrospectee >>= xChild;
        Reference< XModel > xDocumentModel( xChild, UNO_QUERY );
        while( !xDocumentModel.is() && xChild.is() )
        {
            Reference< XInterface > xParent = xChild->getParent();
            xDocumentModel = Reference< XModel >( xParent, UNO_QUERY );
            xChild = Reference< XChild >( xParent, UNO_QUERY );
        }
        return xDocumentModel;
    }

    //------------------------------------------------------------------------
    ::rtl::OUString OPropertyBrowserController::getDocumentURL() const
    {
        ::rtl::OUString sURL;

        Reference< XModel > xModel( getDocumentModel() );
        if ( xModel.is() )
            sURL = xModel->getURL();

        return sURL;
    }

//............................................................................
} // namespace pcr
//............................................................................


