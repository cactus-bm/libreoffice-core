/*************************************************************************
 *
 *  $RCSfile: vclxwindow.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: mt $ $Date: 2001-11-27 10:31:24 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _TOOLKIT_AWT_VCLXWINDOW_HXX_
#define _TOOLKIT_AWT_VCLXWINDOW_HXX_


#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
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
#ifndef _COM_SUN_STAR_AWT_XGraphics_HPP_
#include <com/sun/star/awt/XGraphics.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLE_HDL_
#include <drafts/com/sun/star/accessibility/XAccessible.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLECONTEXT_HDL_
#include <drafts/com/sun/star/accessibility/XAccessibleContext.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLECOMPONENT_HDL_
#include <drafts/com/sun/star/accessibility/XAccessibleComponent.hpp>
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

class Window;
class VclSimpleEvent;
class VclWindowEvent;


//  ----------------------------------------------------
//  class VCLXWINDOW
//  ----------------------------------------------------

class VCLXWindow :  public ::com::sun::star::awt::XWindow,
                    public ::com::sun::star::awt::XVclWindowPeer,
                    public ::com::sun::star::awt::XLayoutConstrains,
                    public ::com::sun::star::awt::XView,
                    public ::drafts::com::sun::star::accessibility::XAccessible,
                    public ::drafts::com::sun::star::accessibility::XAccessibleContext,
                    public ::drafts::com::sun::star::accessibility::XAccessibleComponent,
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

    ULONG                           nDummy1;
    ULONG                           nDummy2;
    void*                           pDummy1;
    void*                           pDummy2;

    sal_Bool                        mbDisposing;
    sal_Bool                        mbDesignMode;
    sal_Bool                        mbAccessible;


protected:
    Size            ImplCalcWindowSize( const Size& rOutSz ) const;
    DECL_LINK(      WindowEventHdl, VclSimpleEvent* );

    virtual void    ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );


public:
    VCLXWindow();
    ~VCLXWindow();

    virtual void    SetWindow( Window* pWindow );
    Window*         GetWindow() const                                   { return (Window*)GetOutputDevice(); }
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XGraphics> GetViewGraphics() const { return mxViewGraphics; }

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


    // ::com::sun::star::lang::Component
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

    // ::drafts::com::sun::star::accessibility::XAccessible
    ::com::sun::star::uno::Reference< ::drafts::com::sun::star::accessibility::XAccessibleContext > SAL_CALL getAccessibleContext(  ) throw (::com::sun::star::uno::RuntimeException);

    // ::drafts::com::sun::star::accessibility::XAccessibleContext
    sal_Int32 SAL_CALL getAccessibleChildCount(  ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::drafts::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::drafts::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleParent(  ) throw (::com::sun::star::uno::RuntimeException);
    sal_Int32 SAL_CALL getAccessibleIndexInParent(  ) throw (::com::sun::star::uno::RuntimeException);
    sal_Int16 SAL_CALL getAccessibleRole(  ) throw (::com::sun::star::uno::RuntimeException);
    ::rtl::OUString SAL_CALL getAccessibleDescription(  ) throw (::com::sun::star::uno::RuntimeException);
    ::rtl::OUString SAL_CALL getAccessibleName(  ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::drafts::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::drafts::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL getAccessibleStateSet(  ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::lang::Locale SAL_CALL getLocale(  ) throw (::drafts::com::sun::star::accessibility::IllegalComponentStateException, ::com::sun::star::uno::RuntimeException);
    void SAL_CALL addPropertyChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL removePropertyChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL setAccessibleDescription( const ::rtl::OUString& sNewDescription ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL setAccessibleName( const ::rtl::OUString& sNewName ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL setAccessibleParent( const ::com::sun::star::uno::Reference< ::drafts::com::sun::star::accessibility::XAccessible >& xNewParent ) throw (::com::sun::star::uno::RuntimeException);

    // ::drafts::com::sun::star::accessibility::XAccessibleComponent
    sal_Bool SAL_CALL contains( const ::com::sun::star::awt::Point& aPoint ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::drafts::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAt( const ::com::sun::star::awt::Point& aPoint ) throw (::com::sun::star::uno::RuntimeException);
    sal_Int32 SAL_CALL getForeground(  ) throw (::com::sun::star::uno::RuntimeException);
//    void SAL_CALL setForeground( sal_Int32 aColor ) throw (::com::sun::star::uno::RuntimeException);
    sal_Int32 SAL_CALL getBackground(  ) throw (::com::sun::star::uno::RuntimeException);
//    void SAL_CALL setBackground( sal_Int32 aColor ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::awt::Rectangle SAL_CALL getBounds(  ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::awt::Point SAL_CALL getLocation(  ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen(  ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL setLocation( const ::com::sun::star::awt::Point& aRelativePosition ) throw (::com::sun::star::uno::RuntimeException);
//    ::com::sun::star::awt::Size SAL_CALL getSize(  ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL setSize( const ::com::sun::star::awt::Size& aSize ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFont > SAL_CALL getFont(  ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL setFont( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFont >& xFont ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::awt::FontDescriptor SAL_CALL getFontMetrics( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFont >& xFont ) throw (::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL isShowing(  ) throw (::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL isVisible(  ) throw (::com::sun::star::uno::RuntimeException);
//    void SAL_CALL setVisible( sal_Bool bVisibility ) throw (::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL isFocusTraversable(  ) throw (::com::sun::star::uno::RuntimeException);
//    void SAL_CALL addFocusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
//    void SAL_CALL removeFocusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL grabFocus(  ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL setBounds( const ::com::sun::star::awt::Rectangle& aBoundingBox ) throw (::com::sun::star::uno::RuntimeException);
    sal_Int32 SAL_CALL getCursor(  ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL setCursor( sal_Int32 nCursorIndex ) throw (::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL isEnabled(  ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL setEnabled( sal_Bool bEnabledState ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Any SAL_CALL getAccessibleKeyBinding(  ) throw (::com::sun::star::uno::RuntimeException);
    ::rtl::OUString SAL_CALL getTitledBorderText(  ) throw (::com::sun::star::uno::RuntimeException);
    ::rtl::OUString SAL_CALL getToolTipText(  ) throw (::com::sun::star::uno::RuntimeException);
};




#endif // _TOOLKIT_AWT_VCLXWINDOW_HXX_

