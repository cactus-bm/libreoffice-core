/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: vclxwindow.hxx,v $
 *
 *  $Revision: 1.27 $
 *
 *  last change: $Author: ihi $ $Date: 2008-01-14 12:55:19 $
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

#ifndef _TOOLKIT_AWT_VCLXWINDOW_HXX_
#define _TOOLKIT_AWT_VCLXWINDOW_HXX_

#ifndef TOOLKIT_DLLAPI_H
#include <toolkit/dllapi.h>
#endif

#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XWINDOW2_HPP_
#include <com/sun/star/awt/XWindow2.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XVCLWINDOWPEER_HPP_
#include <com/sun/star/awt/XVclWindowPeer.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XLAYOUTCONSTRAINS_HPP_
#include <com/sun/star/awt/XLayoutConstrains.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XVIEW_HPP_
#include <com/sun/star/awt/XView.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XPOINTER_HPP_
#include <com/sun/star/awt/XPointer.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XGRAPHICS_HPP_
#include <com/sun/star/awt/XGraphics.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYCHANGELISTENER_HPP_
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#endif
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLE_HPP_
#include <com/sun/star/accessibility/XAccessible.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLECONTEXT_HPP_
#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XDOCKABLEWINDOW_HPP_
#include <com/sun/star/awt/XDockableWindow.hpp>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#include <toolkit/awt/vclxdevice.hxx>
#include <toolkit/helper/listenermultiplexer.hxx>

#include <tools/gen.hxx>    // Size

#ifndef _LINK_HXX
#include <tools/link.hxx>
#endif

#include <stdarg.h>
#include <list>

class Window;
class VclSimpleEvent;
class VclWindowEvent;
struct AccessibilityInfos;
struct SystemParentData;

namespace toolkit
{
    class IAccessibleFactory;
}


//  ----------------------------------------------------
//  class VCLXWINDOW
//  ----------------------------------------------------

class UnoPropertyArrayHelper;
class VCLXWindowImpl;
class TOOLKIT_DLLPUBLIC VCLXWindow :    public ::com::sun::star::awt::XWindow2,
                    public ::com::sun::star::awt::XVclWindowPeer,
                    public ::com::sun::star::awt::XLayoutConstrains,
                    public ::com::sun::star::awt::XView,
                    public ::com::sun::star::awt::XDockableWindow,
                    public ::com::sun::star::accessibility::XAccessible,
                    public ::com::sun::star::lang::XEventListener,
                    public ::com::sun::star::beans::XPropertySetInfo,
                    public VCLXDevice
{
private:
    EventListenerMultiplexer        maEventListeners;
    FocusListenerMultiplexer        maFocusListeners;
    WindowListenerMultiplexer       maWindowListeners;
    KeyListenerMultiplexer          maKeyListeners;
    MouseListenerMultiplexer        maMouseListeners;
    MouseMotionListenerMultiplexer  maMouseMotionListeners;
    PaintListenerMultiplexer        maPaintListeners;
    VclContainerListenerMultiplexer maContainerListeners;
    TopWindowListenerMultiplexer    maTopWindowListeners;

    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XPointer>  mxPointer;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XGraphics> mxViewGraphics;

    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > mxAccessibleContext;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XDockableWindowListener> mxDockableWindowListener;

    ULONG                           mnListenerLockLevel;
    ULONG                           nDummy2;
    VCLXWindowImpl*                 mpImpl;
    UnoPropertyArrayHelper         *mpPropHelper;


    sal_Bool                        mbDisposing : 1;
    sal_Bool                        mbDesignMode : 1;
    sal_Bool                        mbSynthesizingVCLEvent : 1;
    sal_Bool                        mbWithDefaultProps : 1;
    sal_Bool                        mbDrawingOntoParent;

    UnoPropertyArrayHelper *GetPropHelper();

protected:
    Size            ImplCalcWindowSize( const Size& rOutSz ) const;
    DECL_LINK(      WindowEventListener, VclSimpleEvent* );

    virtual void    ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext >
                    CreateAccessibleContext();

    void            SetSynthesizingVCLEvent( sal_Bool b )   { mbSynthesizingVCLEvent = b; }
    BOOL            IsSynthesizingVCLEvent() const          { return mbSynthesizingVCLEvent; }
    void        SetSystemParent_Impl( const com::sun::star::uno::Any& rHandle );

    ::toolkit::IAccessibleFactory&  getAccessibleFactory();

    // helper ...
    static void     PushPropertyIds( std::list< sal_uInt16 > &aIds, int nFirstId, ...);
    // for use in controls/
    static void     ImplGetPropertyIds( std::list< sal_uInt16 > &aIds,
                                        bool bWithDefaults = false );
    virtual void    GetPropertyIds( std::list< sal_uInt16 > &aIds )
        { return ImplGetPropertyIds( aIds, mbWithDefaultProps ); }

public:
    VCLXWindow( bool bWithDefaultProps = false );
    ~VCLXWindow();

    virtual void    SetWindow( Window* pWindow );
    Window*         GetWindow() const                                   { return (Window*)GetOutputDevice(); }
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XGraphics> GetViewGraphics() const { return mxViewGraphics; }

    void    suspendVclEventListening( );
    void    resumeVclEventListening( );

    EventListenerMultiplexer&       GetEventListeners()     { return maEventListeners; }
    FocusListenerMultiplexer&       GetFocusListeners()     { return maFocusListeners; }
    WindowListenerMultiplexer&      GetWindowListeners()    { return maWindowListeners; }
    KeyListenerMultiplexer&         GetKeyListeners()       { return maKeyListeners; }
    MouseListenerMultiplexer&       GetMouseListeners()     { return maMouseListeners; }
    MouseMotionListenerMultiplexer& GetMouseMotionListeners() { return maMouseMotionListeners; }
    PaintListenerMultiplexer&       GetPaintListeners()     { return maPaintListeners; }
    VclContainerListenerMultiplexer& GetContainerListeners() { return maContainerListeners; }
    TopWindowListenerMultiplexer&   GetTopWindowListeners() { return maTopWindowListeners; }

    // ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any  SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
    void                        SAL_CALL acquire() throw()  { OWeakObject::acquire(); }
    void                        SAL_CALL release() throw()  { OWeakObject::release(); }

    // ::com::sun::star::lang::XUnoTunnel
    static const ::com::sun::star::uno::Sequence< sal_Int8 >&   GetUnoTunnelId() throw();
    static VCLXWindow*                                          GetImplementation( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& rxIFace ) throw();
    sal_Int64                                                   SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& rIdentifier ) throw(::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::lang::XTypeProvider
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >  SAL_CALL getTypes() throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Sequence< sal_Int8 >                     SAL_CALL getImplementationId() throw(::com::sun::star::uno::RuntimeException);


    // ::com::sun::star::lang::XEventListener
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::lang::XComponent
    void SAL_CALL dispose(  ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& rxListener ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& rxListener ) throw(::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::awt::XWindow
    void SAL_CALL setPosSize( sal_Int32 X, sal_Int32 Y, sal_Int32 Width, sal_Int32 Height, sal_Int16 Flags ) throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::awt::Rectangle SAL_CALL getPosSize(  ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setVisible( sal_Bool Visible ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setEnable( sal_Bool Enable ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setFocus(  ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL addWindowListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowListener >& rrxListener ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL removeWindowListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowListener >& rrxListener ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL addFocusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& rrxListener ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL removeFocusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& rrxListener ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL addKeyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XKeyListener >& rrxListener ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL removeKeyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XKeyListener >& rrxListener ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL addMouseListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMouseListener >& rrxListener ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL removeMouseListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMouseListener >& rrxListener ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL addMouseMotionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMouseMotionListener >& rrxListener ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL removeMouseMotionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMouseMotionListener >& rrxListener ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL addPaintListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XPaintListener >& rrxListener ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL removePaintListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XPaintListener >& rrxListener ) throw(::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::awt::XWindowPeer
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XToolkit > SAL_CALL getToolkit(  ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setPointer( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XPointer >& Pointer ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setBackground( sal_Int32 Color ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL invalidate( sal_Int16 Flags ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL invalidateRect( const ::com::sun::star::awt::Rectangle& Rect, sal_Int16 Flags ) throw(::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::awt::XVclWindowPeer
    sal_Bool SAL_CALL isChild( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer >& Peer ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setDesignMode( sal_Bool bOn ) throw(::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL isDesignMode(  ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL enableClipSiblings( sal_Bool bClip ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setForeground( sal_Int32 Color ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setControlFont( const ::com::sun::star::awt::FontDescriptor& aFont ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL getStyles( sal_Int16 nType, ::com::sun::star::awt::FontDescriptor& Font, sal_Int32& ForegroundColor, sal_Int32& BackgroundColor ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value ) throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::awt::XLayoutConstrains
    ::com::sun::star::awt::Size SAL_CALL getMinimumSize(  ) throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::awt::Size SAL_CALL getPreferredSize(  ) throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::awt::Size SAL_CALL calcAdjustedSize( const ::com::sun::star::awt::Size& aNewSize ) throw(::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::awt::XView
    sal_Bool SAL_CALL setGraphics( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XGraphics >& aDevice ) throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XGraphics > SAL_CALL getGraphics(  ) throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::awt::Size SAL_CALL getSize(  ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL draw( sal_Int32 nX, sal_Int32 nY ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setZoom( float fZoomX, float fZoomY ) throw(::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::accessibility::XAccessible
    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > SAL_CALL getAccessibleContext(  ) throw (::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::awt::XDockableWindow
    void SAL_CALL addDockableWindowListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XDockableWindowListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL removeDockableWindowListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XDockableWindowListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL enableDocking( sal_Bool bEnable ) throw (::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL isFloating(  ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL setFloatingMode( sal_Bool bFloating ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL lock(  ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL unlock(  ) throw (::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL isLocked(  ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL startPopupMode( const ::com::sun::star::awt::Rectangle& WindowRect ) throw (::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL isInPopupMode(  ) throw (::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::awt::XWindow2
    void SAL_CALL setOutputSize( const ::com::sun::star::awt::Size& aSize ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::awt::Size SAL_CALL getOutputSize(  ) throw (::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL isVisible(  ) throw (::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL isActive(  ) throw (::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL isEnabled(  ) throw (::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL hasFocus(  ) throw (::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::beans::XPropertySetInfo
    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property > SAL_CALL getProperties(  ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::beans::Property SAL_CALL getPropertyByName( const ::rtl::OUString& aName ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
    ::sal_Bool SAL_CALL hasPropertyByName( const ::rtl::OUString& Name ) throw (::com::sun::star::uno::RuntimeException);
};

#endif // _TOOLKIT_AWT_VCLXWINDOW_HXX_

