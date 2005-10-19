/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: DrawController.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: rt $ $Date: 2005-10-19 12:26:54 $
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

#ifndef SD_VIEW_SHELL_BASE_HXX
#include "ViewShellBase.hxx"
#endif
#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#include "View.hxx"
#include "Window.hxx"
#include "DrawDocShell.hxx"
#include "unomodel.hxx"
#ifndef SD_PANE_MANAGER_HXX
#include "PaneManager.hxx"
#endif
#ifndef SD_VIEW_SHELL_MANAGER_HXX
#include "ViewShellManager.hxx"
#endif

#ifndef _CPPUHELPER_TYPEPROVIDER_HXX_
#include <cppuhelper/typeprovider.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SVDOBJ_HXX
#include <svx/svdobj.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/helper/vclunohelper.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_XINTERFACE_HPP_
#include <com/sun/star/uno/XInterface.hpp>
#endif
#include <svx/unoshcol.hxx>
#include <svx/unopage.hxx>

using namespace ::std;
using namespace ::rtl;
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


DrawController::DrawController (
    ViewShellBase& rBase,
    ViewShell& rViewShell,
    View& rView) throw()
    : DrawControllerInterfaceBase(&rBase),
      BroadcastHelperOwner(SfxBaseController::m_aMutex),
      OPropertySetHelper( static_cast<OBroadcastHelperVar<
          OMultiTypeInterfaceContainerHelper,
          OMultiTypeInterfaceContainerHelper::keyType>& >(
              BroadcastHelperOwner::maBroadcastHelper)),
      mpView(&rView),
      mpViewShell(&rViewShell),
      meViewShellType(rViewShell.GetShellType()),
      maLastVisArea(),
      mrBase(rBase),
      mbDisposing(false),
      mpPropertyArrayHelper(NULL)
{
}




DrawController::~DrawController (void) throw()
{
}




void DrawController::DetachFromViewShell (void)
{
    // Set the pointers to view shell and view to NULL so that no one
    // accesses the soon-to-be-deleted objects.
    mpViewShell = NULL;
    mpView = NULL;

    maLastVisArea = Rectangle();
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
            if (mpViewShell != NULL)
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
    "com.sun.star.drawing.DrawController"));

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
    return false;
}




Any SAL_CALL DrawController::getSelection()
    throw(RuntimeException)
{
    ThrowIfDisposed();
    OGuard aGuard( Application::GetSolarMutex() );

    SdXImpressDocument* pModel = GetModel();

    DBG_ASSERT (mpView != NULL, "view is NULL in SdUnoDrawView::getSelection()");

    Any aAny;

    if (mpView != NULL)
    {
        if( mpView->IsTextEdit() )
            mpView->getTextSelection( aAny );

        if ( !aAny.hasValue() )
        {
            const SdrMarkList& rMarkList = mpView->GetMarkedObjectList();
            sal_uInt32 nCount = rMarkList.GetMarkCount();
            if( nCount )
            {
                Reference< drawing::XShapes > xShapes( SvxShapeCollection_NewInstance(), UNO_QUERY );
                for( sal_uInt32 nNum = 0; nNum < nCount; nNum++)
                {
                    SdrMark *pMark = rMarkList.GetMark(nNum);
                    if(pMark==NULL)
                        continue;

                    SdrObject *pObj = pMark->GetObj();
                    if(pObj==NULL || pObj->GetPage() == NULL)
                        continue;

                    Reference< drawing::XDrawPage > xPage( pObj->GetPage()->getUnoPage(), UNO_QUERY);

                    if(!xPage.is())
                        continue;

                    SvxDrawPage* pDrawPage = SvxDrawPage::getImplementation( xPage );

                    if(pDrawPage==NULL)
                        continue;

                    Reference< drawing::XShape > xShape( pObj->getUnoShape(), UNO_QUERY );

                    if(xShape.is())
                        xShapes->add(xShape);
                }
                aAny <<= xShapes;
            }
        }
    }

    return aAny;
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



// XWindow

Reference<awt::XWindow> DrawController::GetWindow (void)
{
    Reference< ::com::sun::star::awt::XWindow > xWindow;
    if (mpViewShell != NULL)
    {
        Window* pWindow = mpViewShell->GetActiveWindow();
        if (pWindow != NULL)
            xWindow = VCLUnoHelper::GetInterface (pWindow);
    }
    return xWindow;
}

void SAL_CALL DrawController::setPosSize( sal_Int32 X, sal_Int32 Y, sal_Int32 Width, sal_Int32 Height, sal_Int16 Flags ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->setPosSize( X, Y, Width, Height, Flags );
}

::com::sun::star::awt::Rectangle SAL_CALL DrawController::getPosSize(  ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    ::com::sun::star::awt::Rectangle aRect;

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        aRect = xWindow->getPosSize();

    return aRect;
}

void SAL_CALL DrawController::setVisible( sal_Bool Visible ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->setVisible( Visible );
}

void SAL_CALL DrawController::setEnable( sal_Bool Enable ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->setEnable( Enable );
}

void SAL_CALL DrawController::setFocus(  ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->setFocus();
}

void SAL_CALL DrawController::addWindowListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->addWindowListener( xListener );
}

void SAL_CALL DrawController::removeWindowListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->removeWindowListener( xListener );
}

void SAL_CALL DrawController::addFocusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->addFocusListener( xListener );
}

void SAL_CALL DrawController::removeFocusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->removeFocusListener( xListener );
}

void SAL_CALL DrawController::addKeyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XKeyListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->addKeyListener( xListener );
}

void SAL_CALL DrawController::removeKeyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XKeyListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->removeKeyListener( xListener );
}

void SAL_CALL DrawController::addMouseListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMouseListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->addMouseListener( xListener );
}

void SAL_CALL DrawController::removeMouseListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMouseListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->removeMouseListener( xListener );
}

void SAL_CALL DrawController::addMouseMotionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMouseMotionListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->addMouseMotionListener( xListener );
}

void SAL_CALL DrawController::removeMouseMotionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMouseMotionListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->removeMouseMotionListener( xListener );
}

void SAL_CALL DrawController::addPaintListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XPaintListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->addPaintListener( xListener );
}

void SAL_CALL DrawController::removePaintListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XPaintListener >& xListener ) throw (::com::sun::star::uno::RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    Reference< ::com::sun::star::awt::XWindow > xWindow( GetWindow() );
    if( xWindow.is() )
        xWindow->removePaintListener( xListener );
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




SdXImpressDocument* DrawController::GetModel (void) const throw()
{
    if (mpView!=NULL && mpView->GetDocSh()!=NULL)
    {
        Reference< frame::XModel > xModel (mpView->GetDocSh()->GetModel());
        return SdXImpressDocument::getImplementation(xModel);
    }
    else
        return NULL;
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
}




Reference< drawing::XDrawPage > SAL_CALL DrawController::getCurrentPage()
    throw(RuntimeException)
{
    ThrowIfDisposed();
    OGuard aGuard( Application::GetSolarMutex() );

    Reference< drawing::XDrawPage >  xPage;

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
    rProperties.push_back (
        beans::Property(
            OUString( RTL_CONSTASCII_USTRINGPARAM("VisibleArea") ),
            PROPERTY_WORKAREA,
            ::getCppuType((const ::com::sun::star::awt::Rectangle*)0),
            beans::PropertyAttribute::BOUND
            | beans::PropertyAttribute::READONLY));
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
    OGuard aGuard( Application::GetSolarMutex() );

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
    return sal_False;
}




void DrawController::setFastPropertyValue_NoBroadcast (
    sal_Int32 nHandle,
    const Any& rValue)
    throw ( com::sun::star::uno::Exception)
{
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

