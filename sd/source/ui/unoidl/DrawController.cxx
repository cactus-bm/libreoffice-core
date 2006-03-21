/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: DrawController.cxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: obo $ $Date: 2006-03-21 17:33:43 $
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

#include "DrawController.hxx"

#include "DrawSubController.hxx"
#ifndef SD_PANE_MANAGER_HXX
#include "PaneManager.hxx"
#endif
#include "sdpage.hxx"
#ifndef SD_VIEW_SHELL_BASE_HXX
#include "ViewShellBase.hxx"
#endif
#ifndef SD_VIEW_SHELL_MANAGER_HXX
#include "ViewShellManager.hxx"
#endif

#include <comphelper/anytostring.hxx>
#include <cppuhelper/exc_hlp.hxx>

#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif

using namespace ::std;
using ::rtl::OUString;
using namespace ::cppu;
using namespace ::vos;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace {
static const ::com::sun::star::uno::Type saComponentTypeIdentifier (
    ::getCppuType( (Reference<lang::XEventListener > *)0 ));
static const ::com::sun::star::uno::Type saSelectionTypeIdentifier (
    ::getCppuType( (Reference<view::XSelectionChangeListener > *)0 ));

} // end of anonymous namespace

namespace sd {


DrawController::DrawController (ViewShellBase& rBase) throw()
    : DrawControllerInterfaceBase(&rBase),
      BroadcastHelperOwner(SfxBaseController::m_aMutex),
      OPropertySetHelper( static_cast<OBroadcastHelperVar<
          OMultiTypeInterfaceContainerHelper,
          OMultiTypeInterfaceContainerHelper::keyType>& >(
              BroadcastHelperOwner::maBroadcastHelper)),
      mrBase(rBase),
      maLastVisArea(),
      mpCurrentPage(NULL),
      mbMasterPageMode(false),
      mbLayerMode(false),
      mbDisposing(false),
      mpPropertyArrayHelper(NULL),
      mpSubController()
{
}




DrawController::~DrawController (void) throw()
{
}




void DrawController::SetSubController (::std::auto_ptr<DrawSubController> pSubController)
{
    // Update the internal state.
    mpSubController = pSubController;
    mpPropertyArrayHelper.reset();
    maLastVisArea = Rectangle();

    // Inform listeners about the changed state.
    FireSelectionChangeListener();
}



// XInterface

IMPLEMENT_FORWARD_XINTERFACE2(
    DrawController,
    DrawControllerInterfaceBase,
    OPropertySetHelper);


// XTypeProvider

Sequence<Type> SAL_CALL DrawController::getTypes (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    ThrowIfDisposed();
    // OPropertySetHelper does not provide getTypes, so we have to
    // implement this method manually and list its three interfaces.
    OTypeCollection aTypeCollection (
        ::getCppuType (( const Reference<beans::XMultiPropertySet>*)NULL),
        ::getCppuType (( const Reference<beans::XFastPropertySet>*)NULL),
        ::getCppuType (( const Reference<beans::XPropertySet>*)NULL));

    return ::comphelper::concatSequences(
        SfxBaseController::getTypes(),
        aTypeCollection.getTypes(),
        DrawControllerInterfaceBase::getTypes());
}

IMPLEMENT_GET_IMPLEMENTATION_ID(DrawController);



// XComponent


void SAL_CALL DrawController::dispose()
    throw( RuntimeException )
{
    if( !mbDisposing )
    {
        OGuard aGuard( Application::GetSolarMutex() );

        if( !mbDisposing )
        {
            mbDisposing = true;

            // When the controller has not been detached from its view
            // shell, i.e. mpViewShell is not NULL, then tell PaneManager
            // and ViewShellManager to clear the shell stack.
            if (mpSubController.get() != NULL)
            {
                mrBase.DisconnectAllClients();
                mrBase.GetPaneManager().Shutdown(true);
                mrBase.GetViewShellManager().Shutdown();
            }

            OPropertySetHelper::disposing();

            SfxBaseController::dispose();
        }
    }
}




void SAL_CALL DrawController::addEventListener(
    const Reference<lang::XEventListener >& xListener)
    throw (RuntimeException)
{
    ThrowIfDisposed();
    SfxBaseController::addEventListener( xListener );
}




void SAL_CALL DrawController::removeEventListener (
    const Reference<lang::XEventListener >& aListener)
    throw (RuntimeException)
{
    if(!rBHelper.bDisposed && !rBHelper.bInDispose && !mbDisposing)
        SfxBaseController::removeEventListener( aListener );
}




// XServiceInfo

OUString SAL_CALL DrawController::getImplementationName(  ) throw(RuntimeException)
{
    ThrowIfDisposed();
    return OUString( RTL_CONSTASCII_USTRINGPARAM( "DrawController" ) );
}



static OUString ssServiceName (OUString::createFromAscii(
    "com.sun.star.drawing.DrawingDocumentDrawView"));

sal_Bool SAL_CALL DrawController::supportsService (
    const OUString& rsServiceName)
    throw(RuntimeException)
{
    ThrowIfDisposed();
    return rsServiceName.equals(ssServiceName);
}




Sequence<OUString> SAL_CALL DrawController::getSupportedServiceNames (void)
    throw(RuntimeException)
{
    ThrowIfDisposed();
    Sequence<OUString> aSupportedServices (1);
    OUString* pServices = aSupportedServices.getArray();
    pServices[0] = ssServiceName;
    return aSupportedServices;
}




//------ XSelectionSupplier --------------------------------------------

sal_Bool SAL_CALL DrawController::select (const Any& aSelection)
    throw(lang::IllegalArgumentException, RuntimeException)
{
    ThrowIfDisposed();
    ::vos::OGuard aGuard (Application::GetSolarMutex());

    if (mpSubController.get() != NULL)
        return mpSubController->select(aSelection);
    else
        return false;
}




Any SAL_CALL DrawController::getSelection()
    throw(RuntimeException)
{
    ThrowIfDisposed();
    ::vos::OGuard aGuard (Application::GetSolarMutex());

    if (mpSubController.get() != NULL)
        return mpSubController->getSelection();
    else
        return Any();
}




void SAL_CALL DrawController::addSelectionChangeListener(
    const Reference< view::XSelectionChangeListener >& xListener)
    throw(RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    BroadcastHelperOwner::maBroadcastHelper.addListener (saSelectionTypeIdentifier, xListener);
}




void SAL_CALL DrawController::removeSelectionChangeListener(
    const Reference< view::XSelectionChangeListener >& xListener )
    throw(RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    BroadcastHelperOwner::maBroadcastHelper.removeListener (saSelectionTypeIdentifier, xListener);
}





//=====  lang::XEventListener  ================================================

void SAL_CALL
    DrawController::disposing (const lang::EventObject& rEventObject)
    throw (uno::RuntimeException)
{
}




//=====  view::XSelectionChangeListener  ======================================

void  SAL_CALL
    DrawController::selectionChanged (const lang::EventObject& rEvent)
        throw (uno::RuntimeException)
{
    ThrowIfDisposed();
    // Have to forward the event to our selection change listeners.
    OInterfaceContainerHelper* pListeners = BroadcastHelperOwner::maBroadcastHelper.getContainer(
        ::getCppuType((Reference<view::XSelectionChangeListener>*)0));
    if (pListeners)
    {
        // Re-send the event to all of our listeners.
        OInterfaceIteratorHelper aIterator (*pListeners);
        while (aIterator.hasMoreElements())
        {
            try
            {
                view::XSelectionChangeListener* pListener =
                    static_cast<view::XSelectionChangeListener*>(
                        aIterator.next());
                if (pListener != NULL)
                    pListener->selectionChanged (rEvent);
            }
            catch (RuntimeException aException)
            {
            }
        }
    }
}




::awt::Rectangle DrawController::GetVisArea (void) const
{
    return awt::Rectangle(
        maLastVisArea.Left(),
        maLastVisArea.Top(),
        maLastVisArea.GetWidth(),
        maLastVisArea.GetHeight());
}




// XDrawView

void SAL_CALL DrawController::setCurrentPage( const Reference< drawing::XDrawPage >& xPage )
    throw(RuntimeException)
{
    ThrowIfDisposed();
    ::vos::OGuard aGuard (Application::GetSolarMutex());

    if (mpSubController.get() != NULL)
        mpSubController->setCurrentPage(xPage);
}




Reference< drawing::XDrawPage > SAL_CALL DrawController::getCurrentPage (void)
    throw(RuntimeException)
{
    ThrowIfDisposed();
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    Reference<drawing::XDrawPage> xPage;

    if (mpSubController.get() != NULL)
        xPage = mpSubController->getCurrentPage();

    return xPage;
}




void DrawController::FireVisAreaChanged (const Rectangle& rVisArea) throw()
{
    if( maLastVisArea != rVisArea )
    {
        Any aNewValue;
        aNewValue <<= awt::Rectangle(
            rVisArea.Left(),
            rVisArea.Top(),
            rVisArea.GetWidth(),
            rVisArea.GetHeight() );

        Any aOldValue;
        aOldValue <<= awt::Rectangle(
            maLastVisArea.Left(),
            maLastVisArea.Top(),
            maLastVisArea.GetWidth(),
            maLastVisArea.GetHeight() );

        FirePropertyChange (PROPERTY_WORKAREA, aNewValue, aOldValue);

        maLastVisArea = rVisArea;
    }
}




void DrawController::FireSelectionChangeListener() throw()
{
    OInterfaceContainerHelper * pLC = BroadcastHelperOwner::maBroadcastHelper.getContainer(
        saSelectionTypeIdentifier);
    if( pLC )
    {
        Reference< XInterface > xSource( (XWeak*)this );
        const lang::EventObject aEvent( xSource );

        // Ueber alle Listener iterieren und Events senden
        OInterfaceIteratorHelper aIt( *pLC);
        while( aIt.hasMoreElements() )
        {
            try
            {
                view::XSelectionChangeListener * pL =
                    static_cast<view::XSelectionChangeListener*>(aIt.next());
                if (pL != NULL)
                    pL->selectionChanged( aEvent );
            }
            catch (RuntimeException aException)
            {
            }
        }
    }
}




void DrawController::FireChangeEditMode (bool bMasterPageMode) throw()
{
    if (bMasterPageMode != mbMasterPageMode )
    {
        FirePropertyChange(
            PROPERTY_MASTERPAGEMODE,
            makeAny(bMasterPageMode),
            makeAny(mbMasterPageMode));

        mbMasterPageMode = bMasterPageMode;
    }
}




void DrawController::FireChangeLayerMode (bool bLayerMode) throw()
{
    if (bLayerMode != mbLayerMode)
    {
        FirePropertyChange(
            PROPERTY_LAYERMODE,
            makeAny(bLayerMode),
            makeAny(mbLayerMode));

        mbLayerMode = bLayerMode;
    }
}




void DrawController::FireSwitchCurrentPage (SdPage* pCurrentPage) throw()
{
    if (pCurrentPage != mpCurrentPage)
        try
        {
            Any aNewValue (
                makeAny(Reference<drawing::XDrawPage>(pCurrentPage->getUnoPage(), UNO_QUERY)));

            Any aOldValue;
            if (mpCurrentPage != NULL)
            {
                Reference<drawing::XDrawPage> xOldPage (mpCurrentPage->getUnoPage(), UNO_QUERY);
                aOldValue <<= xOldPage;
            }

            FirePropertyChange(PROPERTY_CURRENTPAGE, aNewValue, aOldValue);

            mpCurrentPage = pCurrentPage;
        }
        catch( uno::Exception& e )
        {
            (void)e;
            DBG_ERROR(
                (::rtl::OString("sd::SdUnoDrawView::FireSwitchCurrentPage(), "
                    "exception caught: ") +
                    ::rtl::OUStringToOString(
                        comphelper::anyToString( cppu::getCaughtException() ),
                        RTL_TEXTENCODING_UTF8 )).getStr() );
        }
}




void DrawController::FirePropertyChange (
    sal_Int32 nHandle,
    const Any& rNewValue,
    const Any& rOldValue)
{
    try
    {
        fire (&nHandle, &rNewValue, &rOldValue, 1, sal_False);
    }
    catch (RuntimeException aException)
    {
        // Ignore this exception.  Exceptions should be handled in the
        // fire() function so that all listeners are called.  This is
        // not the case at the moment, so we simply ignore the
        // exception.
    }

}




//===== Properties ============================================================

void DrawController::FillPropertyTable (
    ::std::vector<beans::Property>& rProperties)
{
    rProperties.push_back(
        beans::Property(
            OUString( RTL_CONSTASCII_USTRINGPARAM("VisibleArea") ),
            PROPERTY_WORKAREA,
            ::getCppuType((const ::com::sun::star::awt::Rectangle*)0),
            beans::PropertyAttribute::BOUND | beans::PropertyAttribute::READONLY));
    rProperties.push_back(
        beans::Property(
            OUString( RTL_CONSTASCII_USTRINGPARAM("CurrentPage") ),
            PROPERTY_CURRENTPAGE,
            ::getCppuType((const Reference< drawing::XDrawPage > *)0),
            beans::PropertyAttribute::BOUND ));
    rProperties.push_back(
        beans::Property( OUString( RTL_CONSTASCII_USTRINGPARAM("IsLayerMode") ),
            PROPERTY_LAYERMODE,
            ::getCppuBooleanType(),
            beans::PropertyAttribute::BOUND ));
    rProperties.push_back(
        beans::Property( OUString( RTL_CONSTASCII_USTRINGPARAM("IsMasterPageMode") ),
            PROPERTY_MASTERPAGEMODE,
            ::getCppuBooleanType(),
            beans::PropertyAttribute::BOUND ));
    rProperties.push_back(
        beans::Property( OUString( RTL_CONSTASCII_USTRINGPARAM("ActiveLayer") ),
            PROPERTY_ACTIVE_LAYER,
            ::getCppuBooleanType(),
            beans::PropertyAttribute::BOUND ));
    rProperties.push_back(
        beans::Property( OUString( RTL_CONSTASCII_USTRINGPARAM("ZoomValue") ),
            PROPERTY_ZOOMVALUE,
            ::getCppuType((const sal_Int16*)0),
            beans::PropertyAttribute::BOUND ));
    rProperties.push_back(
        beans::Property( OUString( RTL_CONSTASCII_USTRINGPARAM("ZoomType") ),
            PROPERTY_ZOOMTYPE,
            ::getCppuType((const sal_Int16*)0),
            beans::PropertyAttribute::BOUND ));
    rProperties.push_back(
        beans::Property( OUString( RTL_CONSTASCII_USTRINGPARAM("ViewOffset") ),
            PROPERTY_VIEWOFFSET,
            ::getCppuType((const ::com::sun::star::awt::Point*)0),
            beans::PropertyAttribute::BOUND ));
}




IPropertyArrayHelper & DrawController::getInfoHelper()
{
    OGuard aGuard( Application::GetSolarMutex() );

    if (mpPropertyArrayHelper.get() == NULL)
    {
        ::std::vector<beans::Property> aProperties;
        FillPropertyTable (aProperties);
        Sequence<beans::Property> aPropertySequence (aProperties.size());
        for (unsigned int i=0; i<aProperties.size(); i++)
            aPropertySequence[i] = aProperties[i];
        mpPropertyArrayHelper.reset(new OPropertyArrayHelper(aPropertySequence, sal_False));
    }

    return *mpPropertyArrayHelper.get();
}




Reference < beans::XPropertySetInfo >  DrawController::getPropertySetInfo()
        throw ( ::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );

    static Reference < beans::XPropertySetInfo >  xInfo( createPropertySetInfo( getInfoHelper() ) );
    return xInfo;
}



sal_Bool DrawController::convertFastPropertyValue (
    Any & rConvertedValue,
    Any & rOldValue,
    sal_Int32 nHandle,
    const Any& rValue)
    throw ( com::sun::star::lang::IllegalArgumentException)
{
    sal_Bool bResult = sal_False;

    if (mpSubController.get() != NULL)
        bResult = mpSubController->convertFastPropertyValue(
            rConvertedValue,
            rOldValue,
            nHandle,
            rValue);

    return bResult;
}




void DrawController::setFastPropertyValue_NoBroadcast (
    sal_Int32 nHandle,
    const Any& rValue)
    throw ( com::sun::star::uno::Exception)
{
    OGuard aGuard( Application::GetSolarMutex() );
    if (mpSubController.get() != NULL)
        mpSubController->setFastPropertyValue_NoBroadcast(nHandle, rValue);
}




void DrawController::getFastPropertyValue (
    Any & rRet,
    sal_Int32 nHandle ) const
{
    OGuard aGuard( Application::GetSolarMutex() );

    switch( nHandle )
    {
        case PROPERTY_WORKAREA:
            rRet <<= awt::Rectangle(
                maLastVisArea.Left(),
                maLastVisArea.Top(),
                maLastVisArea.GetWidth(),
                maLastVisArea.GetHeight());
            break;

        default:
            if (mpSubController.get() != NULL)
                mpSubController->getFastPropertyValue(rRet,nHandle);
            break;
    }
}




void DrawController::ThrowIfDisposed (void) const
    throw (::com::sun::star::lang::DisposedException)
{
    if (rBHelper.bDisposed || rBHelper.bInDispose || mbDisposing)
    {
        OSL_TRACE ("Calling disposed DrawController object. Throwing exception:");
        throw lang::DisposedException (
            OUString(RTL_CONSTASCII_USTRINGPARAM(
                "DrawController object has already been disposed")),
            const_cast<uno::XWeak*>(static_cast<const uno::XWeak*>(this)));
    }
}





} // end of namespace sd

