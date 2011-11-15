/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#include <tools/rc.h>
//#define RESOURCE_PUBLISH_PROTECTED 1
#if RESOURCE_PUBLISH_PROTECTED
// ugh, override non-helpful proctection
#define protected public
#endif /* RESOURCE_PUBLISH_PROTECTED */
#include <tools/rc.hxx>
#undef protected


#include "wrapper.hxx"

#include <awt/vclxplugin.hxx>
#include <awt/vclxtabcontrol.hxx>
#include <com/sun/star/awt/PosSize.hpp>
#include <com/sun/star/awt/VclWindowPeerAttribute.hpp>
#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/awt/XDialog2.hpp>
#include <com/sun/star/awt/XProgressBar.hpp>
#include <com/sun/star/awt/XSimpleTabController.hpp>
#include <com/sun/star/awt/XTabListener.hpp>
#include <com/sun/star/graphic/XGraphic.hpp>
#include <comphelper/processfactory.hxx>
#include <layout/core/factory.hxx>
#include <layout/core/localized-string.hxx>
#include <layout/core/root.hxx>
#include <toolkit/awt/vclxwindow.hxx>
#include <vcl/ctrl.hxx>
#include <vcl/dialog.hxx>
#include <vcl/image.hxx>
#include <vcl/tabctrl.hxx>
#include <vcl/tabpage.hxx>
#include <vcl/window.hxx>

using namespace ::com::sun::star;
using rtl::OUString;

namespace layout
{

// Context bits ...
class ContextImpl
{
    uno::Reference< awt::XLayoutRoot > mxRoot;
    uno::Reference< container::XNameAccess > mxNameAccess;
    PeerHandle mxTopLevel;

public:
    ContextImpl( char const *pPath )
    {
        uno::Sequence< uno::Any > aParams( 1 );
        aParams[0] <<= OUString( pPath, strlen( pPath ), RTL_TEXTENCODING_UTF8 );

        uno::Reference< lang::XSingleServiceFactory > xFactory(
            comphelper::createProcessComponent(
                OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.awt.Layout")) ),
            uno::UNO_QUERY );
        if ( !xFactory.is() )
        {
            throw uno::RuntimeException(
                OUString( RTL_CONSTASCII_USTRINGPARAM( "Layout engine not installed" ) ),
                uno::Reference< uno::XInterface >() );
        }
        mxRoot = uno::Reference< awt::XLayoutRoot >(
            xFactory->createInstanceWithArguments( aParams ),
            uno::UNO_QUERY );

        mxNameAccess = uno::Reference< container::XNameAccess >( mxRoot, uno::UNO_QUERY );
    }

    ~ContextImpl()
    {
    }

    PeerHandle getByName( const OUString &rName )
    {
        uno::Any val = mxNameAccess->getByName( rName );
        PeerHandle xRet;
        val >>= xRet;
        return xRet;
    }
    PeerHandle getTopLevel()
    {
        return mxTopLevel;
    }
    void setTopLevel( PeerHandle xToplevel )
    {
        mxTopLevel = xToplevel;
    }
};

Context::Context( const char *pPath )
    : pImpl( new ContextImpl( pPath ) )
{
}
Context::~Context()
{
    delete pImpl;
    pImpl = NULL;
}

PeerHandle Context::GetPeerHandle( const char *id, sal_uInt32 nId ) const
{
    PeerHandle xHandle;
    xHandle = pImpl->getByName( OUString( id, strlen( id ), RTL_TEXTENCODING_UTF8 ) );
    if ( !xHandle.is() )
    {
        OSL_TRACE( "Failed to fetch widget '%s'", id );
    }

    if ( nId != 0 )
    {
        rtl::OString aStr = rtl::OString::valueOf( (sal_Int32) nId );
        xHandle = GetPeerHandle( aStr.getStr(), 0 );
    }
    return xHandle;
}

WindowImpl::WindowImpl (Context *context, const PeerHandle &peer, Window *window)
    : mpWindow (window)
    , mpCtx (context)
    , mxWindow (peer, uno::UNO_QUERY)
    , mxVclPeer (peer, uno::UNO_QUERY)
    , mvclWindow (0)
    , bFirstTimeVisible (true)
{
}

WindowImpl::~WindowImpl ()
{
    if (mpWindow)
        mpWindow->mpImpl = 0;
    if (mvclWindow)
    {
        VCLXWindow *v = mvclWindow->GetWindowPeer ();
        v->SetWindow (0);
        mvclWindow->SetComponentInterface (uno::Reference <awt::XWindowPeer> ());
        mvclWindow->SetWindowPeer (uno::Reference <awt::XWindowPeer> (), 0);
        delete mvclWindow;
        mvclWindow = 0;
    }
}

void WindowImpl::wrapperGone ()
{
    mvclWindow = 0;
    mpWindow->mpImpl = 0;
    mpWindow = 0;
    mpCtx = 0;
    if ( mxWindow.is() )
    {
        uno::Reference< lang::XComponent > xComp( mxWindow, uno::UNO_QUERY );
        mxWindow.clear ();
        if ( xComp.is() )
            xComp->dispose();
    }
}

void SAL_CALL WindowImpl::disposing (lang::EventObject const&)
    throw (uno::RuntimeException)
{
    if (mxWindow.is ())
        mxWindow.clear ();
}

void WindowImpl::setProperty (char const *name, uno::Any any)
{
    if ( !this || !mxVclPeer.is() )
        return;
    mxVclPeer->setProperty
        ( rtl::OUString( name, strlen( name ), RTL_TEXTENCODING_ASCII_US ), any );
}

void WindowImpl::redraw (bool resize)
{
    uno::Reference <awt::XWindow> ref (mxWindow, uno::UNO_QUERY);
    ::Window* window = VCLXWindow::GetImplementation (ref)->GetWindow ();
    ::Window* parent = window->GetParent();
    ::Rectangle r = Rectangle (parent->GetPosPixel (),
                               parent->GetSizePixel ());
    parent->Invalidate (r, INVALIDATE_CHILDREN | INVALIDATE_NOCHILDREN );
    if (resize)
        parent->SetPosSizePixel (0, 0, 1, 1, awt::PosSize::SIZE);
    else
        parent->SetPosSizePixel (0, 0, r.nRight - r.nLeft, r.nBottom - r.nTop,
                                 awt::PosSize::SIZE);
}

Window::Window( WindowImpl *pImpl )
    : mpImpl( pImpl )
{
    mpImpl->mvclWindow = GetVCLXWindow () ? GetWindow () : 0;
}

Window::~Window()
{
    /* likely to be an UNO object - with floating references */
    if (mpImpl)
        mpImpl->wrapperGone ();
    mpImpl = 0;
}

///IMPL_GET_IMPL( Control );

static ControlImpl* null_control_impl = 0;

ControlImpl &Control::getImpl () const
{
    if (ControlImpl* c = static_cast<ControlImpl *>(mpImpl))
        return *c;
    return *null_control_impl;
}

Control::~Control ()
{
    SetGetFocusHdl (Link ());
    SetLoseFocusHdl (Link ());
}

void Window::setRes (ResId const& res)
{
#if RESOURCE_PUBLISH_PROTECTED
    // Resources are shut-off from use.  Is that really necessary?
    Resource &r = *GetWindow ();
    r.GetRes (res);
#else /* !RESOURCE_PUBLISH_PROTECTED */
    //We *must* derive.  Is this also really necessary?
    //Resource r (res);

    // ugh, I wonder which solution is cleaner...
    class Resource_open_up : public Resource
    {
    public:
        Resource_open_up (ResId const& r)
            : Resource (r)
        {
        }
        static sal_Int32 GetLongRes (void *p)
        {
            return Resource::GetLongRes (p);
        }
        void* GetClassRes ()
        {
            return Resource::GetClassRes ();
        }
        sal_Int32 ReadLongRes ()
        {
            return Resource::ReadLongRes ();
        }
        UniString ReadStringRes ()
        {
            return Resource::ReadStringRes ();
        }
        rtl::OString ReadByteStringRes()
        {
            return Resource::ReadByteStringRes();
        }
    };

    Resource_open_up r (res);
#endif /* !RESOURCE_PUBLISH_PROTECTED */
    sal_uInt32 mask = r.ReadLongRes ();
    if (mask & WINDOW_HELPID)
        SetHelpId (r.ReadByteStringRes());
    if ( mask & WINDOW_TEXT )
        SetText( r.ReadStringRes ());
}

void Window::SetParent( ::Window *parent )
{
    uno::Reference <awt::XWindow> ref( GetPeer(), uno::UNO_QUERY );
    if (VCLXWindow *vcl = VCLXWindow::GetImplementation( ref ))
        if (::Window *window = vcl->GetWindow())
            window->SetParent( parent );
}

void Window::SetParent( Window *parent )
{
    /* Let's hear it for C++: poor man's dynamic binding.  */
    parent->ParentSet (this);
}

void Window::ParentSet (Window *window)
{
    window->SetParent (GetWindow ());
}

PeerHandle Window::GetPeer() const
{
    if ( !mpImpl )
        return PeerHandle();
    return mpImpl->mxWindow;
}

uno::Reference<awt::XWindow> Window::GetRef() const
{
    return uno::Reference <awt::XWindow> ( GetPeer(), uno::UNO_QUERY );
}

VCLXWindow* Window::GetVCLXWindow() const
{
    return VCLXWindow::GetImplementation( GetRef() );
}

::Window* Window::GetWindow() const
{
    return GetVCLXWindow()->GetWindow();
}

::Window* Window::GetParent() const
{
    return GetWindow()->GetParent();
}

void Window::SetHelpId( const rtl::OString& id )
{
    GetWindow()->SetHelpId( id );
}

void Window::Invalidate (sal_uInt8 flags)
{
    GetWindow ()->Invalidate (flags);
}

struct ToolkitVclPropsMap
{
    WinBits vclStyle;
    long initAttr;
    const char *propName;

    // the value to give the prop to enable/disable it -- not the most brilliant
    // type declaration and storage, but does the work... properties are
    // either a boolean or a short since they are either a directly wrappers for
    // a WinBit, or aggregates related (like Align for WB_LEFT, _RIGHT and _CENTER).
    bool isBoolean;
    short enableProp, disableProp;
};

#define TYPE_BOOL  true
#define TYPE_SHORT false
#define NOTYPE     0
static const ToolkitVclPropsMap toolkitVclPropsMap[] =
{
    { WB_BORDER,    awt::WindowAttribute::BORDER,    "Border", TYPE_SHORT, 1, 0 },
    { WB_NOBORDER,    awt::VclWindowPeerAttribute::NOBORDER,    "Border", TYPE_SHORT, 0, 1 },
    { WB_SIZEABLE,    awt::WindowAttribute::SIZEABLE,    NULL, NOTYPE, 0, 0 },
    { WB_MOVEABLE,    awt::WindowAttribute::MOVEABLE,    NULL, NOTYPE, 0, 0 },
    { WB_CLOSEABLE,    awt::WindowAttribute::CLOSEABLE,    NULL, NOTYPE, 0, 0 },

    { WB_HSCROLL,    awt::VclWindowPeerAttribute::HSCROLL,    NULL, NOTYPE, 0, 0 },
    { WB_VSCROLL,    awt::VclWindowPeerAttribute::VSCROLL,    NULL, NOTYPE, 0, 0 },
    { WB_LEFT,    awt::VclWindowPeerAttribute::LEFT,    "Align", TYPE_SHORT, 0, 0 },
    { WB_CENTER,    awt::VclWindowPeerAttribute::CENTER,    "Align", TYPE_SHORT, 1, 0 },
    { WB_RIGHT,    awt::VclWindowPeerAttribute::RIGHT,    "Align", TYPE_SHORT, 2, 0 },
    { WB_SPIN,    awt::VclWindowPeerAttribute::SPIN,    NULL, NOTYPE, 0, 0 },
    { WB_SORT,    awt::VclWindowPeerAttribute::SORT,    NULL, NOTYPE, 0, 0 },
    { WB_DROPDOWN,    awt::VclWindowPeerAttribute::DROPDOWN,    "Dropdown",    TYPE_BOOL, 1, 0 },
    { WB_DEFBUTTON,    awt::VclWindowPeerAttribute::DEFBUTTON,    "DefaultButton", TYPE_BOOL, 1, 0 },
    { WB_READONLY,    awt::VclWindowPeerAttribute::READONLY,    NULL, NOTYPE, 0, 0 },
    { WB_CLIPCHILDREN,    awt::VclWindowPeerAttribute::CLIPCHILDREN,    NULL, NOTYPE, 0, 0 },
    { WB_GROUP,    awt::VclWindowPeerAttribute::GROUP,    NULL, NOTYPE, 0, 0 },

    { WB_OK,    awt::VclWindowPeerAttribute::OK,    NULL, NOTYPE, 0, 0 },
    { WB_OK_CANCEL,    awt::VclWindowPeerAttribute::OK_CANCEL,    NULL, NOTYPE, 0, 0 },
    { WB_YES_NO,    awt::VclWindowPeerAttribute::YES_NO,    NULL, NOTYPE, 0, 0 },
    { WB_YES_NO_CANCEL,    awt::VclWindowPeerAttribute::YES_NO_CANCEL,    NULL, NOTYPE, 1, 0 },
    { WB_RETRY_CANCEL,    awt::VclWindowPeerAttribute::RETRY_CANCEL,    NULL, NOTYPE, 1, 0 },
    { WB_DEF_OK,    awt::VclWindowPeerAttribute::DEF_OK,    NULL, NOTYPE, 0, 0 },
    { WB_DEF_CANCEL,    awt::VclWindowPeerAttribute::DEF_CANCEL,    NULL, NOTYPE, 1, 0 },
    { WB_DEF_RETRY,    awt::VclWindowPeerAttribute::DEF_RETRY,    NULL, NOTYPE, 0, 0 },
    { WB_DEF_YES,    awt::VclWindowPeerAttribute::DEF_YES,    NULL, NOTYPE, 0, 0 },
    { WB_DEF_NO,    awt::VclWindowPeerAttribute::DEF_NO,    NULL, NOTYPE, 0, 0 },

    { WB_AUTOHSCROLL, awt::VclWindowPeerAttribute::AUTOHSCROLL, "AutoHScroll", TYPE_BOOL, 1, 0 },
    { WB_AUTOVSCROLL, awt::VclWindowPeerAttribute::AUTOVSCROLL, "AutoVScroll",    TYPE_BOOL, 1, 0 },

    { WB_WORDBREAK,    0,    "MultiLine", TYPE_BOOL, 1, 0 },
    { WB_NOPOINTERFOCUS,    0,    "FocusOnClick", TYPE_BOOL, 1, 0 },
    { WB_TOGGLE,    0,    "Toggle", TYPE_BOOL, 1, 0 },
    { WB_REPEAT,    0,    "Repeat", TYPE_BOOL, 1, 0 },
    { WB_NOHIDESELECTION,    0,    "HideInactiveSelection", TYPE_BOOL, 1, 0 },
};
#undef TYPE_BOOL
#undef TYPE_SHORT
#undef NOTYPE

static const int toolkitVclPropsMapLen =
    sizeof( toolkitVclPropsMap ) / sizeof( ToolkitVclPropsMap );

/* Unpleasant way to get an xToolkit pointer ... */
uno::Reference< awt::XToolkit > getToolkit()
{
    static uno::Reference< awt::XToolkit > xToolkit;
    if (!xToolkit.is())
    {
        // Urgh ...
        xToolkit = uno::Reference< awt::XToolkit >(
            ::comphelper::getProcessServiceFactory()->createInstance(
                OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.Toolkit" ) ) ),
            uno::UNO_QUERY );
        if ( !xToolkit.is() )
            throw uno::RuntimeException(
                OUString( RTL_CONSTASCII_USTRINGPARAM( "failed to create toolkit!") ),
                uno::Reference< uno::XInterface >() );
    }
    return xToolkit;
}

void Window::Show( bool bVisible )
{
    if ( !getImpl().mxWindow.is() )
        return;
    getImpl().mxWindow->setVisible( bVisible );
    if (!bVisible)
        getImpl ().bFirstTimeVisible = true;
    else if (GetParent() && getImpl().bFirstTimeVisible)
    {
        getImpl().redraw ();
        getImpl().bFirstTimeVisible = false;
    }
}

void Window::SetText( OUString const& str )
{
    GetWindow()->SetText( str );
}

ControlImpl::ControlImpl (Context *context, const PeerHandle &peer, Window *window)
    : WindowImpl( context, peer, window )
{
}

ControlImpl::~ControlImpl ()
{
    if ((!!mGetFocusHdl || !!mLoseFocusHdl) && mxWindow.is ())
        /* Disposing will be done @ VCLXWindow::dispose () maFocusListeners.disposeAndClear()
           don't do it twice */
        mxWindow.clear ();
}

void ControlImpl::SetGetFocusHdl (Link const& link)
{
    if (!mLoseFocusHdl || !link)
        UpdateListening (link);
    mGetFocusHdl = link;
}

void ControlImpl::SetLoseFocusHdl (Link const& link)
{
    if (!mGetFocusHdl || !link)
        UpdateListening (link);
    mLoseFocusHdl = link;
}

void ControlImpl::UpdateListening (Link const& link)
{
    if (!link && (!!mGetFocusHdl || !!mLoseFocusHdl)
        && (!mGetFocusHdl || !mLoseFocusHdl))
        mxWindow->removeFocusListener (this);
    else if (!!link && !mGetFocusHdl && !mLoseFocusHdl)
        mxWindow->addFocusListener (this);
}

void SAL_CALL ControlImpl::disposing (lang::EventObject const&)
    throw (uno::RuntimeException)
{
///    mxWindow.clear ();
}

void SAL_CALL ControlImpl::focusGained (awt::FocusEvent const&)
    throw (uno::RuntimeException)
{
    mGetFocusHdl.Call (mpWindow);
}

void SAL_CALL ControlImpl::focusLost (awt::FocusEvent const&)
    throw (uno::RuntimeException)
{
    mLoseFocusHdl.Call (mpWindow);
}

void Control::SetGetFocusHdl (Link const& link)
{
    if (&getImpl () && getImpl().mxWindow.is ())
        getImpl ().SetGetFocusHdl (link);
}

void Control::SetLoseFocusHdl (Link const& link)
{
    if (&getImpl () && getImpl().mxWindow.is ())
        getImpl ().SetLoseFocusHdl (link);
}

class DialogImpl : public WindowImpl
{
public:
    uno::Reference< awt::XDialog2 > mxDialog;
    DialogImpl( Context *context, PeerHandle const &peer, Window *window );
};

DialogImpl::DialogImpl( Context *context, const PeerHandle &peer, Window *window )
    : WindowImpl( context, peer, window )
    , mxDialog( peer, uno::UNO_QUERY )
{
}

Dialog::Dialog( ::Window *parent, const char *xml_file, const char *id, sal_uInt32 nId )
    : Context( xml_file )
    , Window( new DialogImpl( this, Context::GetPeerHandle( id, nId ), this ) )
{
    if ( parent )
        SetParent( parent );
}

Dialog::~Dialog ()
{
}

IMPL_GET_IMPL (Dialog);

#define MX_DIALOG if (getImpl ().mxDialog.is ()) getImpl ().mxDialog
#define RETURN_MX_DIALOG if (getImpl ().mxDialog.is ()) return getImpl ().mxDialog

short Dialog::Execute()
{
    RETURN_MX_DIALOG->execute ();
    return -1;
}

void Dialog::EndDialog( long result )
{
    MX_DIALOG->endDialog (result);
}

void Dialog::SetTitle( OUString const& str )
{
    MX_DIALOG->setTitle (str);
}

#define MESSAGE_BOX_MEMBER_INIT\
    Dialog (parent, xml_file, id)\
        , imageError (this, "FI_ERROR")\
        , imageInfo (this, "FI_INFO")\
        , imageQuery (this, "FI_QUERY")\
        , imageWarning (this, "FI_WARNING")\
        , messageText (this, "FT_MESSAGE")\
        , cancelButton (this, "BTN_CANCEL")\
        , helpButton (this, "BTN_HELP")\
        , ignoreButton (this, "BTN_IGNORE")\
        , noButton (this, "BTN_NO")\
        , retryButton (this, "BTN_RETRY")\
        , yesButton (this, "BTN_YES")

MessageBox::MessageBox (::Window *parent, char const* message,
                        char const* yes, char const* no, const rtl::OString& help_id,
                        char const* xml_file, char const* id)
    : MESSAGE_BOX_MEMBER_INIT
{
    ignoreButton.Hide ();
    retryButton.Hide ();
    init (message, yes, no, help_id);
}

MessageBox::MessageBox (::Window *parent, OUString const& message,
                        OUString yes, OUString no, const rtl::OString& help_id,
                        char const* xml_file, char const* id)
    : MESSAGE_BOX_MEMBER_INIT
{
    ignoreButton.Hide ();
    retryButton.Hide ();
    init (message, yes, no, help_id);
}

#if !defined (__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif /* !__GNUC__ */

MessageBox::MessageBox (::Window *parent, WinBits bits, char const* message,
                        char const* yes, char const* no, const rtl::OString& help_id,
                        char const* xml_file, char const* id)
    : MESSAGE_BOX_MEMBER_INIT
{
    // HIG suggests using verbs instead of yes/no/retry etc.
    // This constructor provides client-code compatibility: Client code should be fixed.
#ifndef __SUNPRO_CC
    OSL_TRACE ("%s: warning, deprecated vcl/Messbox compatibility", __PRETTY_FUNCTION__);
#endif
    bits_init (bits, OUString::createFromAscii (message), OUString::createFromAscii (yes), OUString::createFromAscii (no), help_id);
}

MessageBox::MessageBox (::Window *parent, WinBits bits, OUString const& message,
                        OUString yes, OUString no, const rtl::OString& help_id,
                        char const* xml_file, char const* id)
    : MESSAGE_BOX_MEMBER_INIT
{
    // HIG suggests using verbs instead of yes/no/retry etc.
    // This constructor provides client-code compatibility: Client code should be fixed.
#ifndef __SUNPRO_CC
    OSL_TRACE ("%s: warning, deprecated vcl/Messbox compatibility", __PRETTY_FUNCTION__);
#endif
    bits_init (bits, message, yes, no, help_id);
}

void MessageBox::bits_init (WinBits bits, OUString const& message,
                            OUString yes, OUString no, const rtl::OString& help_id)
{
    if ( bits & ( WB_OK_CANCEL | WB_OK ))
        yes = Button::GetStandardText ( BUTTON_OK );
    if ( bits & (WB_YES_NO | WB_YES_NO_CANCEL ))
    {
        yes = Button::GetStandardText ( BUTTON_YES );
        no =  Button::GetStandardText ( BUTTON_NO );
    }
    if (! (bits & (WB_RETRY_CANCEL | WB_YES_NO_CANCEL | WB_ABORT_RETRY_IGNORE )))
        cancelButton.Hide ();
    if (! (bits & (WB_RETRY_CANCEL | WB_ABORT_RETRY_IGNORE)))
        retryButton.Hide ();
    if ( bits & WB_ABORT_RETRY_IGNORE )
        cancelButton.SetText ( Button::GetStandardText (BUTTON_ABORT));
    else
        ignoreButton.Hide ();
    if ( !(bits & ( WB_OK | WB_OK_CANCEL | WB_YES_NO | WB_YES_NO_CANCEL)))
        yesButton.Hide ();
    if ( !(bits & ( WB_YES_NO | WB_YES_NO_CANCEL)))
        noButton.Hide ();

    init (message, yes, no, help_id);
}

void MessageBox::init (char const* message, char const* yes, char const* no, const rtl::OString& help_id)
{
    init ( OUString::createFromAscii (message), OUString::createFromAscii (yes), OUString::createFromAscii (no), help_id);
}

void MessageBox::init (OUString const& message, OUString const& yes, OUString const& no, const rtl::OString& help_id)
{
    imageError.Hide ();
    imageInfo.Hide ();
    imageQuery.Hide ();
    imageWarning.Hide ();
    if (message.getLength ())
        messageText.SetText (message);
    if (yes.getLength ())
    {
        yesButton.SetText (yes);
        if (yes != OUString (Button::GetStandardText (BUTTON_OK))
            && yes != OUString (Button::GetStandardText (BUTTON_YES)))
            SetTitle (yes);
        if (no.getLength ())
            noButton.SetText (no);
        else
            noButton.Hide ();
    }
    if (help_id.getLength() != 0)
        SetHelpId (help_id);
    else
        helpButton.Hide ();
}

class TabControlImpl
    : public ControlImpl
    , public ::cppu::WeakImplHelper1 <awt::XTabListener>
{
    Link mActivatePageHdl;
    Link mDeactivatePageHdl;

public:
    uno::Reference <awt::XSimpleTabController> mxTabControl;
    TabControlImpl (Context *context, const PeerHandle &peer, Window *window)
        : ControlImpl (context, peer, window)
        ,  mxTabControl (peer, uno::UNO_QUERY)
    {
    }

    virtual void SAL_CALL disposing (lang::EventObject const& e)
        throw (uno::RuntimeException)
    {
        ControlImpl::disposing (e);
        mxTabControl.clear ();
    }

    void SetActivatePageHdl (Link const& link)
    {
        if (!mDeactivatePageHdl || !link)
            UpdateListening (link);
        mActivatePageHdl = link;
    }

    void SetDeactivatePageHdl (Link const& link)
    {
        if (!mActivatePageHdl || !link)
            UpdateListening (link);
        mDeactivatePageHdl = link;
    }

    void UpdateListening (Link const& link)
    {
        if (!link && (!!mActivatePageHdl || !!mDeactivatePageHdl))
            mxTabControl->removeTabListener (this);
        else if (!!link && !mActivatePageHdl && !mDeactivatePageHdl)
            mxTabControl->addTabListener (this);
    }

    void SAL_CALL activated (sal_Int32)
        throw (uno::RuntimeException)
    {
        mActivatePageHdl.Call (mpWindow);
    }

    void SAL_CALL deactivated (sal_Int32)
        throw (uno::RuntimeException)
    {
        mDeactivatePageHdl.Call (mpWindow);
    }

    void SAL_CALL inserted (sal_Int32)
        throw (uno::RuntimeException)
    {
    }

    void SAL_CALL removed (sal_Int32)
        throw (uno::RuntimeException)
    {
    }

    void SAL_CALL changed (sal_Int32, uno::Sequence <beans::NamedValue> const&)
        throw (uno::RuntimeException)
    {
    }
};

IMPL_GET_WINDOW (TabControl);

#define MX_TABCONTROL if (getImpl ().mxTabControl.is ()) getImpl ().mxTabControl
#define RETURN_MX_TABCONTROL if (getImpl ().mxTabControl.is ()) return getImpl ().mxTabControl

TabControl::~TabControl ()
{
    SetActivatePageHdl (Link ());
    SetDeactivatePageHdl (Link ());
}

void TabControl::SetActivatePageHdl (Link const& link)
{
    if (&getImpl () && getImpl().mxTabControl.is ())
        getImpl ().SetActivatePageHdl (link);
}
void TabControl::SetDeactivatePageHdl (Link const& link)
{
    if (&getImpl () && getImpl().mxTabControl.is ())
        getImpl ().SetDeactivatePageHdl (link);
}

IMPL_GET_IMPL (TabControl);

class TabPageImpl : public WindowImpl
{
public:
    uno::Reference< awt::XWindow > mxTabPage;
    TabPageImpl( Context *context, const PeerHandle &peer, Window *window )
        : WindowImpl( context, peer, window )
        , mxTabPage( peer, uno::UNO_QUERY )
    {
    }
};

::Window* TabPage::global_parent = 0;
TabControl* TabPage::global_tabcontrol = 0;

IMPL_GET_IMPL( TabPage );

TabPage::~TabPage()
{
    delete GetTabPage();
}

IMPL_GET_WINDOW( TabPage );

void TabPage::ActivatePage()
{
}

void TabPage::DeactivatePage()
{
}

class FixedTextImpl : public ControlImpl
{
public:
    uno::Reference< awt::XFixedText > mxFixedText;
    FixedTextImpl( Context *context, const PeerHandle &peer, Window *window )
        : ControlImpl( context, peer, window )
        , mxFixedText( peer, uno::UNO_QUERY )
    {
    }

    ~FixedTextImpl ();

    virtual void SAL_CALL disposing( lang::EventObject const& e )
        throw (uno::RuntimeException);
};

FixedTextImpl::~FixedTextImpl ()
{
}

void SAL_CALL FixedTextImpl::disposing( lang::EventObject const& e )
    throw (uno::RuntimeException)
{
    ControlImpl::disposing (e);
    mxFixedText.clear ();
}

FixedText::FixedText ( Context *context, char const* pId, sal_uInt32 nId )
    : Control( new FixedTextImpl( context, context->GetPeerHandle( pId, nId ), this ) )
{
    Window *parent = dynamic_cast<Window*> (context);
    if (parent)
        SetParent (parent);
}

FixedText::~FixedText ()
{
}

IMPL_GET_IMPL( FixedText );

void FixedText::SetText( OUString const& rStr )
{
    if ( !getImpl().mxFixedText.is() )
        return;
    getImpl().mxFixedText->setText( rStr );
}

class ProgressBarImpl : public ControlImpl
{
public:
    uno::Reference< awt::XProgressBar > mxProgressBar;
    ProgressBarImpl( Context *context, const PeerHandle &peer, Window *window )
        : ControlImpl( context, peer, window )
        , mxProgressBar( peer, uno::UNO_QUERY )
    {
    }

    virtual void SAL_CALL disposing( lang::EventObject const& e )
        throw (uno::RuntimeException)
    {
        ControlImpl::disposing (e);
        mxProgressBar.clear ();
    }
};


class FixedImageImpl: public ControlImpl
{
public:
    uno::Reference< graphic::XGraphic > mxGraphic;
    FixedImageImpl( Context *context, const PeerHandle &peer, Window *window)
        : ControlImpl( context, peer, window )
        , mxGraphic( peer, uno::UNO_QUERY )
    {
        if ( !mxGraphic.is() )
        {
            OSL_FAIL( "ERROR: failed to load image: `%s'" );
        }
    }
};

FixedImage::FixedImage ( Context *context, char const* pId, sal_uInt32 nId )
    : Control( new FixedImageImpl( context, context->GetPeerHandle( pId, nId ), this ) )
{
    Window *parent = dynamic_cast<Window*> (context);
    if (parent)
        SetParent (parent);
}

IMPL_GET_IMPL( FixedImage )

} // namespace layout

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
