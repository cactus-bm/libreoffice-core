/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: updatecheckui.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: vg $ $Date: 2006-11-01 15:32:53 $
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
#include "precompiled_extensions.hxx"

#include <list>

#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implementationentry.hxx>

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XEVENTLISTENER_HPP_
#include <com/sun/star/document/XEventListener.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XEVENTBROADCASTER_HPP_
#include <com/sun/star/document/XEventBroadcaster.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_GRAPHIC_XGRAPHICPROVIDER_HPP_
#include <com/sun/star/graphic/XGraphicProvider.hpp>
#endif

#ifndef _COM_SUN_STAR_TASK_XJOB_HPP_
#include <com/sun/star/task/XJob.hpp>
#endif

#include <comphelper/processfactory.hxx>

#include <vos/mutex.hxx>

#include <vcl/window.hxx>
#include <vcl/floatwin.hxx>
#include <vcl/timer.hxx>
#include <vcl/menu.hxx>
#include <vcl/outdev.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/lineinfo.hxx>
#include <vcl/imagebtn.hxx>
#include <vcl/settings.hxx>
#include <vcl/svapp.hxx>
#include <sfx2/sfx.hrc>

#include "updatecheckui.hrc"

#define UNISTRING(s) rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(s))

#define MSG_ERR_NO_WEBBROWSER_FOUND  (RID_SFX_APP_START + 7)
#define DEFAULT_MENUBAR_HEIGHT 24

#define PROPERTY_TITLE          RTL_CONSTASCII_STRINGPARAM("BubbleHeading")
#define PROPERTY_TEXT           RTL_CONSTASCII_STRINGPARAM("BubbleText")
#define PROPERTY_IMAGE          RTL_CONSTASCII_STRINGPARAM("BubbleImageURL")
#define PROPERTY_SHOW_BUBBLE    RTL_CONSTASCII_STRINGPARAM("BubbleVisible")
#define PROPERTY_CLICK_HDL      RTL_CONSTASCII_STRINGPARAM("MenuClickHDL")
#define PROPERTY_DEFAULT_TITLE  RTL_CONSTASCII_STRINGPARAM("DefaultHeading")
#define PROPERTY_DEFAULT_TEXT   RTL_CONSTASCII_STRINGPARAM("DefaultText")
#define PROPERTY_SHOW_MENUICON  RTL_CONSTASCII_STRINGPARAM("MenuIconVisible")

#define START_TIMER 1

using namespace ::com::sun::star;

//------------------------------------------------------------------------------

static uno::Sequence< rtl::OUString > getServiceNames()
{
    uno::Sequence< rtl::OUString > aServiceList(1);
    aServiceList[0] = UNISTRING( "com.sun.star.setup.UpdateCheckUI");
    return aServiceList;
}

//------------------------------------------------------------------------------

static rtl::OUString getImplementationName()
{
    return UNISTRING( "vnd.sun.UpdateCheckUI");
}

//------------------------------------------------------------------------------

namespace
{

//------------------------------------------------------------------------------
class BubbleWindow : public FloatingWindow
{
    Point           maTipPos;
    Region          maBounds;
    Polygon         maRectPoly;
    Polygon         maTriPoly;
    XubString       maBubbleTitle;
    XubString       maBubbleText;
    Image           maBubbleImage;
    Size            maMaxTextSize;
    Rectangle       maTitleRect;
    Rectangle       maTextRect;

private:
    void            RecalcTextRects();

public:
                    BubbleWindow( Window* pParent, const XubString& rTitle,
                                  const XubString& rText, const Image& rImage );
                   ~BubbleWindow();

    virtual void    MouseButtonDown( const MouseEvent& rMEvt );
    virtual void    Paint( const Rectangle& rRect );
    void            Resize();
    void            Show( BOOL bVisible = TRUE, USHORT nFlags = SHOW_NOACTIVATE );
    void            SetTipPosPixel( const Point& rTipPos ) { maTipPos = rTipPos; }
};

//------------------------------------------------------------------------------
class UpdateCheckUI : public ::cppu::WeakImplHelper3
                        < lang::XServiceInfo, document::XEventListener, beans::XPropertySet >
{
    uno::Reference< uno::XComponentContext > m_xContext;
    uno::Reference< task::XJob > mrJob;
    rtl::OUString       maDefaultTitle;
    rtl::OUString       maDefaultText;
    rtl::OUString       maBubbleTitle;
    rtl::OUString       maBubbleText;
    rtl::OUString       maBubbleImageURL;
    Image               maBubbleImage;
    BubbleWindow*       mpBubbleWin;
    SystemWindow*       mpIconSysWin;
    MenuBar*            mpIconMBar;
    ResMgr*             mpUpdResMgr;
    ResMgr*             mpSfxResMgr;
    ULONG               mnLastUserEvent;
    Timer               maWaitTimer;
    Timer               maRetryTimer;
    Timer               maTimeoutTimer;
    Link                maWindowEventHdl;
    sal_Bool            mbShowBubble;
    sal_Bool            mbShowMenuIcon;
    USHORT              mnIconID;

private:
                    DECL_LINK( ClickHdl, USHORT* );
                    DECL_LINK( HighlightHdl, MenuBar::MenuBarButtonCallbackArg* );
                    DECL_LINK( WaitTimeOutHdl, Timer* );
                    DECL_LINK( TimeOutHdl, Timer* );
                    DECL_LINK( UserEventHdl, UpdateCheckUI* );
                    DECL_LINK( WindowEventHdl, VclWindowEvent* );

    BubbleWindow*   GetBubbleWindow();
    void            RemoveBubbleWindow( sal_Bool bRemoveIcon );
    Image           GetMenuBarIcon( MenuBar* pMBar );
    void            AddMenuBarIcon();
    Image           GetBubbleImage( ::rtl::OUString &rURL );

    uno::Reference< document::XEventBroadcaster > getGlobalEventBroadcaster() const
         throw (uno::RuntimeException);

public:
                    UpdateCheckUI(const uno::Reference<uno::XComponentContext>&);
    virtual        ~UpdateCheckUI();

    // XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName()
        throw (uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService(rtl::OUString const & serviceName)
        throw (uno::RuntimeException);
    virtual uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames()
        throw (uno::RuntimeException);

    // XEventListener
    virtual void SAL_CALL notifyEvent(const document::EventObject& Event)
        throw (uno::RuntimeException);
    virtual void SAL_CALL disposing(const lang::EventObject& Event)
        throw (uno::RuntimeException);

    //XPropertySet
    virtual uno::Reference< beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(void)
        throw ( uno::RuntimeException );
    virtual void SAL_CALL setPropertyValue(const rtl::OUString& PropertyName, const uno::Any& aValue)
        throw( beans::UnknownPropertyException, beans::PropertyVetoException,
               lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException );
    virtual uno::Any SAL_CALL getPropertyValue(const rtl::OUString& PropertyName)
        throw ( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException );
    virtual void SAL_CALL addPropertyChangeListener(const rtl::OUString& PropertyName,
                                                    const uno::Reference< beans::XPropertyChangeListener > & aListener)
        throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException );
    virtual void SAL_CALL removePropertyChangeListener(const rtl::OUString& PropertyName,
                                                       const uno::Reference< beans::XPropertyChangeListener > & aListener)
        throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException );
    virtual void SAL_CALL addVetoableChangeListener(const rtl::OUString& PropertyName,
                                                    const uno::Reference< beans::XVetoableChangeListener > & aListener)
        throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException );
    virtual void SAL_CALL removeVetoableChangeListener(const rtl::OUString& PropertyName,
                                                       const uno::Reference< beans::XVetoableChangeListener > & aListener)
        throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException );
};

//------------------------------------------------------------------------------
UpdateCheckUI::UpdateCheckUI(const uno::Reference<uno::XComponentContext>& xContext) :
    m_xContext(xContext)
{
    mpBubbleWin = NULL;
    mpIconSysWin = NULL;
    mpIconMBar = NULL;
    mbShowBubble = FALSE;
    mbShowMenuIcon = TRUE;
    mnIconID = 0;

    mpUpdResMgr = ResMgr::CreateResMgr( "updchk" MAKE_NUMSTR(SUPD) );
    mpSfxResMgr = ResMgr::CreateResMgr( "sfx" MAKE_NUMSTR(SUPD) );

    maDefaultTitle = String( ResId( RID_UPDATE_DEFAULT_TITLE, mpUpdResMgr ) );
    maDefaultText = String( ResId( RID_UPDATE_DEFAULT_TEXT, mpUpdResMgr ) );
    maBubbleImage = GetBubbleImage( maBubbleImageURL );

    maWaitTimer.SetTimeout( 400 );
    maWaitTimer.SetTimeoutHdl( LINK( this, UpdateCheckUI, WaitTimeOutHdl ) );

    maTimeoutTimer.SetTimeout( 10000 );
    maTimeoutTimer.SetTimeoutHdl( LINK( this, UpdateCheckUI, TimeOutHdl ) );

    maRetryTimer.SetTimeout( 200 );
    maRetryTimer.SetTimeoutHdl( LINK( this, UpdateCheckUI, UserEventHdl ) );

    uno::Reference< document::XEventBroadcaster > xBroadcaster( getGlobalEventBroadcaster() );
    xBroadcaster->addEventListener( this );

    maWindowEventHdl = LINK( this, UpdateCheckUI, WindowEventHdl );
    mnLastUserEvent = Application::PostUserEvent( LINK( this, UpdateCheckUI, UserEventHdl ) );
}

//------------------------------------------------------------------------------
UpdateCheckUI::~UpdateCheckUI()
{
    RemoveBubbleWindow( TRUE );
    delete mpUpdResMgr;
    delete mpSfxResMgr;
}

//------------------------------------------------------------------------------
uno::Reference<document::XEventBroadcaster>
UpdateCheckUI::getGlobalEventBroadcaster() const throw (uno::RuntimeException)
{
    uno::Reference<uno::XComponentContext> xContext(m_xContext);

    if( !xContext.is() )
        throw uno::RuntimeException(
            UNISTRING( "UpdateCheckUI: empty component context" ),
                uno::Reference< uno::XInterface >() );

    uno::Reference< lang::XMultiComponentFactory > xServiceManager(xContext->getServiceManager());

    if( !xServiceManager.is() )
        throw uno::RuntimeException(
            UNISTRING( "UpdateCheckUI: unable to obtain service manager from component context" ),
                uno::Reference< uno::XInterface >() );

    return uno::Reference<document::XEventBroadcaster> (
        xServiceManager->createInstanceWithContext(
            UNISTRING( "com.sun.star.frame.GlobalEventBroadcaster" ),
            xContext),
        uno::UNO_QUERY_THROW);
}

//------------------------------------------------------------------------------
rtl::OUString SAL_CALL
UpdateCheckUI::getImplementationName() throw (uno::RuntimeException)
{
    return ::getImplementationName();
}

//------------------------------------------------------------------------------
uno::Sequence< rtl::OUString > SAL_CALL
UpdateCheckUI::getSupportedServiceNames() throw (uno::RuntimeException)
{
    return ::getServiceNames();
}

//------------------------------------------------------------------------------
sal_Bool SAL_CALL
UpdateCheckUI::supportsService( rtl::OUString const & serviceName ) throw (uno::RuntimeException)
{
    uno::Sequence< rtl::OUString > aServiceNameList = ::getServiceNames();

    for( sal_Int32 n=0; n < aServiceNameList.getLength(); n++ )
        if( aServiceNameList[n].equals(serviceName) )
            return sal_True;

    return sal_False;
}

//------------------------------------------------------------------------------
Image UpdateCheckUI::GetMenuBarIcon( MenuBar* pMBar )
{
    sal_uInt32 nResID;
    Window *pMBarWin = pMBar->GetWindow();
    sal_uInt32 nMBarHeight = 20;

    if ( pMBarWin )
        nMBarHeight = pMBarWin->GetOutputSizePixel().getHeight();

    if ( Application::GetSettings().GetStyleSettings().GetHighContrastMode() ) {
        if ( nMBarHeight >= 35 )
            nResID = RID_UPDATE_AVAILABLE_26_HC;
        else
            nResID = RID_UPDATE_AVAILABLE_16_HC;
    } else {
        if ( nMBarHeight >= 35 )
            nResID = RID_UPDATE_AVAILABLE_26;
        else
            nResID = RID_UPDATE_AVAILABLE_16;
    }

    return Image( ResId( nResID, mpUpdResMgr ) );
}

//------------------------------------------------------------------------------
Image UpdateCheckUI::GetBubbleImage( ::rtl::OUString &rURL )
{
    Image aImage;

    if ( maBubbleImageURL.getLength() != 0 )
    {
        uno::Reference< lang::XMultiServiceFactory > xServiceManager = ::comphelper::getProcessServiceFactory();

        if( !xServiceManager.is() )
            throw uno::RuntimeException(
                UNISTRING( "UpdateCheckUI: unable to obtain service manager from component context" ),
                    uno::Reference< uno::XInterface >() );

        try
        {
            uno::Reference< graphic::XGraphicProvider > xGraphProvider(
                    xServiceManager->createInstance(
                            ::rtl::OUString::createFromAscii( "com.sun.star.graphic.GraphicProvider" ) ),
                    uno::UNO_QUERY );
            if ( xGraphProvider.is() )
            {
                uno::Sequence< beans::PropertyValue > aMediaProps( 1 );
                aMediaProps[0].Name = ::rtl::OUString::createFromAscii( "URL" );
                aMediaProps[0].Value <<= rURL;

                uno::Reference< graphic::XGraphic > xGraphic = xGraphProvider->queryGraphic( aMediaProps );
                if ( xGraphic.is() )
                {
                    aImage = Image( xGraphic );
                }
            }
        }
        catch( uno::Exception& )
        {
        }
    }

    if ( aImage.GetSizePixel().Width() == 0 )
        aImage = InfoBox::GetStandardImage();

    return aImage;
}

//------------------------------------------------------------------------------
void UpdateCheckUI::AddMenuBarIcon()
{
    if ( ! mbShowMenuIcon )
        return;

    vos::OGuard aGuard( Application::GetSolarMutex() );

    Window *pTopWin = Application::GetFirstTopLevelWindow();
    Window *pActiveWin = Application::GetActiveTopWindow();
    SystemWindow *pActiveSysWin = NULL;

    if ( pActiveWin && pActiveWin->IsTopWindow() )
        pActiveSysWin = pActiveWin->GetSystemWindow();

    while ( !pActiveSysWin && pTopWin ) {
        if ( pTopWin->IsTopWindow() )
            pActiveSysWin = pTopWin->GetSystemWindow();
        pTopWin = Application::GetNextTopLevelWindow( pTopWin );
    }

    if ( !pActiveSysWin && mpIconSysWin )
        RemoveBubbleWindow( FALSE );

    if ( pActiveSysWin ) {
        MenuBar *pActiveMBar = pActiveSysWin->GetMenuBar();
        if ( ( pActiveSysWin != mpIconSysWin ) ||
             ( pActiveMBar != mpIconMBar ) )
        {
            RemoveBubbleWindow( TRUE );

            if ( pActiveMBar )
            {
                Image aImage = GetMenuBarIcon( pActiveMBar );
                mnIconID = pActiveMBar->AddMenuBarButton( aImage,
                                    LINK( this, UpdateCheckUI, ClickHdl ) );
                pActiveMBar->SetMenuBarButtonHighlightHdl( mnIconID,
                                    LINK( this, UpdateCheckUI, HighlightHdl ) );
            }
            mpIconMBar = pActiveMBar;
            mpIconSysWin = pActiveSysWin;
            mpIconSysWin->AddEventListener( maWindowEventHdl );

            if ( mbShowBubble && pActiveMBar )
            {
                mpBubbleWin = GetBubbleWindow();
                if ( mpBubbleWin )
                {
                    mpBubbleWin->Show( TRUE );
                    maTimeoutTimer.Start();
                }
                mbShowBubble = FALSE;
            }
        }
    }

    if ( mnIconID == 0 )
    {
        OSL_TRACE( "AddMenuBarIcon(): No icon added, retrying!" );
        maRetryTimer.Start();
    }
}

//------------------------------------------------------------------------------
void SAL_CALL UpdateCheckUI::notifyEvent(const document::EventObject& rEvent)
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );

    OSL_TRACE( "notifyEvent: %s", rtl::OUStringToOString(rEvent.EventName, RTL_TEXTENCODING_UTF8).getStr() );
    if( (rEvent.EventName.compareToAscii( RTL_CONSTASCII_STRINGPARAM("OnFocus") ) == 0) ||
        (rEvent.EventName.compareToAscii( RTL_CONSTASCII_STRINGPARAM("OnLoad") ) == 0) ||
        (rEvent.EventName.compareToAscii( RTL_CONSTASCII_STRINGPARAM("OnNew") ) == 0) ||
        (rEvent.EventName.compareToAscii( RTL_CONSTASCII_STRINGPARAM("OnViewClosed") ) == 0) )
    {
        if( ! mnLastUserEvent )
            mnLastUserEvent = Application::PostUserEvent( LINK( this, UpdateCheckUI, UserEventHdl ) );
    }
    if( rEvent.EventName.compareToAscii( RTL_CONSTASCII_STRINGPARAM("OnPrepareViewClosing") ) == 0 )
    {
        RemoveBubbleWindow( TRUE );
    }
}

//------------------------------------------------------------------------------
void SAL_CALL UpdateCheckUI::disposing(const lang::EventObject&)
    throw (uno::RuntimeException)
{
}

//------------------------------------------------------------------------------
uno::Reference< beans::XPropertySetInfo > UpdateCheckUI::getPropertySetInfo(void)
    throw ( uno::RuntimeException )
{
    return NULL;
}

//------------------------------------------------------------------------------
void UpdateCheckUI::setPropertyValue(const rtl::OUString& rPropertyName,
                                     const uno::Any& rValue)
    throw( beans::UnknownPropertyException, beans::PropertyVetoException,
           lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    BOOL bNewBubble = FALSE;
    rtl::OUString aString;
    if( rPropertyName.compareToAscii( PROPERTY_TITLE ) == 0 ) {
        rValue >>= aString;
        if ( aString != maBubbleTitle ) {
            maBubbleTitle = aString;
            bNewBubble = TRUE;
        }
    }
    else if( rPropertyName.compareToAscii( PROPERTY_TEXT ) == 0 ) {
        rValue >>= aString;
        if ( aString != maBubbleText ) {
            maBubbleText = aString;
            bNewBubble = TRUE;
        }
    }
    else if( rPropertyName.compareToAscii( PROPERTY_IMAGE ) == 0 ) {
        rValue >>= aString;
        if ( aString != maBubbleImageURL ) {
            maBubbleImageURL = aString;
            maBubbleImage = GetBubbleImage( maBubbleImageURL );
            bNewBubble = TRUE;
        }
    }
    else if( rPropertyName.compareToAscii( PROPERTY_SHOW_BUBBLE ) == 0 ) {
        rValue >>= mbShowBubble;
        if ( mbShowBubble ) {
            if( ! mnLastUserEvent )
                mnLastUserEvent = Application::PostUserEvent( LINK( this, UpdateCheckUI, UserEventHdl ) );
        }
        else if ( mpBubbleWin )
            mpBubbleWin->Hide();
    }
    else if( rPropertyName.compareToAscii( PROPERTY_CLICK_HDL ) == 0 ) {
        uno::Reference< task::XJob > aJob;
        rValue >>= aJob;
        if ( aJob.is() )
            mrJob = aJob;
        else
            throw lang::IllegalArgumentException();
    }
    else if (rPropertyName.compareToAscii( PROPERTY_SHOW_MENUICON ) == 0) {
        sal_Bool bShowMenuIcon;
        rValue >>= bShowMenuIcon;
        if ( bShowMenuIcon != mbShowMenuIcon )
        {
            mbShowMenuIcon = bShowMenuIcon;
            if ( bShowMenuIcon )
            {
                if( ! mnLastUserEvent )
                    mnLastUserEvent = Application::PostUserEvent( LINK( this, UpdateCheckUI, UserEventHdl ) );
            }
            else
            {
                RemoveBubbleWindow( TRUE );
            }
        }
    }
    else
        throw beans::UnknownPropertyException();

    if ( bNewBubble && mpBubbleWin ) {
        delete mpBubbleWin;
        mpBubbleWin = NULL;
    }
}

//------------------------------------------------------------------------------
uno::Any UpdateCheckUI::getPropertyValue(const rtl::OUString& rPropertyName)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    uno::Any aRet;

    if( rPropertyName.compareToAscii( PROPERTY_TITLE ) == 0 )
        aRet = uno::makeAny( maBubbleTitle );
    else if( rPropertyName.compareToAscii( PROPERTY_TEXT ) == 0 )
        aRet = uno::makeAny( maBubbleText );
    else if( rPropertyName.compareToAscii( PROPERTY_SHOW_BUBBLE ) == 0 )
        aRet = uno::makeAny( mbShowBubble );
    else if( rPropertyName.compareToAscii( PROPERTY_IMAGE ) == 0 )
        aRet = uno::makeAny( maBubbleImageURL );
    else if( rPropertyName.compareToAscii( PROPERTY_CLICK_HDL ) == 0 )
        aRet = uno::makeAny( mrJob );
    else if( rPropertyName.compareToAscii( PROPERTY_DEFAULT_TITLE ) == 0 )
        aRet = uno::makeAny( maDefaultTitle );
    else if( rPropertyName.compareToAscii( PROPERTY_DEFAULT_TEXT ) == 0 )
        aRet = uno::makeAny( maDefaultText );
    else if( rPropertyName.compareToAscii( PROPERTY_SHOW_MENUICON ) == 0 )
        aRet = uno::makeAny( mbShowMenuIcon );
    else
        throw beans::UnknownPropertyException();

    return aRet;
}

//------------------------------------------------------------------------------
void UpdateCheckUI::addPropertyChangeListener( const rtl::OUString& /*aPropertyName*/,
                                               const uno::Reference< beans::XPropertyChangeListener > & /*aListener*/)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    //no bound properties
}

//------------------------------------------------------------------------------
void UpdateCheckUI::removePropertyChangeListener( const rtl::OUString& /*aPropertyName*/,
                                                  const uno::Reference< beans::XPropertyChangeListener > & /*aListener*/)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    //no bound properties
}

//------------------------------------------------------------------------------
void UpdateCheckUI::addVetoableChangeListener( const rtl::OUString& /*aPropertyName*/,
                                               const uno::Reference< beans::XVetoableChangeListener > & /*aListener*/)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    //no vetoable properties
}

//------------------------------------------------------------------------------
void UpdateCheckUI::removeVetoableChangeListener( const rtl::OUString& /*aPropertyName*/,
                                                  const uno::Reference< beans::XVetoableChangeListener > & /*aListener*/)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    //no vetoable properties
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BubbleWindow * UpdateCheckUI::GetBubbleWindow()
{
    if ( !mpIconSysWin )
        return NULL;

    BubbleWindow* pBubbleWin = mpBubbleWin;

    if ( !pBubbleWin ) {
        pBubbleWin = new BubbleWindow( mpIconSysWin,
                                         XubString( maBubbleTitle ),
                                       XubString( maBubbleText ),
                                       maBubbleImage );
    }

    Rectangle aIconRect = mpIconMBar->GetMenuBarButtonRectPixel( mnIconID );
    Point aWinPos = aIconRect.BottomCenter();

    pBubbleWin->SetTipPosPixel( aWinPos );

    return pBubbleWin;
}

//------------------------------------------------------------------------------
void UpdateCheckUI::RemoveBubbleWindow( sal_Bool bRemoveIcon )
{
    vos::OGuard aGuard( Application::GetSolarMutex() );

    maWaitTimer.Stop();
    maTimeoutTimer.Stop();

    if ( mpBubbleWin )
    {
        delete mpBubbleWin;
        mpBubbleWin = NULL;
    }

    if ( bRemoveIcon )
    {
        if ( mpIconSysWin && mpIconMBar &&
             ( mpIconSysWin->GetMenuBar() == mpIconMBar ) )
        {
            mpIconMBar->RemoveMenuBarButton( mnIconID );

            mpIconSysWin = NULL;
            mpIconMBar = NULL;
            mnIconID = 0;
        }
    }
}

// -----------------------------------------------------------------------
IMPL_LINK( UpdateCheckUI, ClickHdl, USHORT*, EMPTYARG )
{
    maWaitTimer.Stop();
    if ( mpBubbleWin )
        mpBubbleWin->Hide();

    if ( mrJob.is() )
    {
        try {
            uno::Sequence<beans::NamedValue> aEmpty;
            mrJob->execute( aEmpty );
        }
        catch(const uno::Exception&) {
            ErrorBox( NULL, ResId( MSG_ERR_NO_WEBBROWSER_FOUND, mpSfxResMgr )).Execute();
        }
    }

    return 0;
}

// -----------------------------------------------------------------------
IMPL_LINK( UpdateCheckUI, HighlightHdl, MenuBar::MenuBarButtonCallbackArg*, pData )
{
    if ( pData->bHighlight )
        maWaitTimer.Start();
    else
        RemoveBubbleWindow( FALSE );

    return 0;
}

// -----------------------------------------------------------------------
IMPL_LINK( UpdateCheckUI, WaitTimeOutHdl, Timer*, EMPTYARG )
{
    mpBubbleWin = GetBubbleWindow();

    if ( mpBubbleWin )
    {
        mpBubbleWin->Show();
    }

    return 0;
}

// -----------------------------------------------------------------------
IMPL_LINK( UpdateCheckUI, TimeOutHdl, Timer*, EMPTYARG )
{
    RemoveBubbleWindow( FALSE );

    return 0;
}

// -----------------------------------------------------------------------
IMPL_LINK( UpdateCheckUI, UserEventHdl, UpdateCheckUI*, EMPTYARG )
{
    mnLastUserEvent = 0;

    AddMenuBarIcon();

    return 0;
}

// -----------------------------------------------------------------------
IMPL_LINK( UpdateCheckUI, WindowEventHdl, VclWindowEvent*, pEvent )
{
    if ( VCLEVENT_OBJECT_DYING == pEvent->GetId() )
    {
        if ( mpIconSysWin == pEvent->GetWindow() )
            RemoveBubbleWindow( TRUE );
    }

    if ( VCLEVENT_WINDOW_ACTIVATE == pEvent->GetId() )
    {
        Window *pActWin = pEvent->GetWindow();
        if ( pActWin && pActWin->IsTopWindow() )
        {
            SystemWindow *pActiveSysWin = pActWin->GetSystemWindow();
            if ( ( pActiveSysWin != mpIconSysWin ) && !mnLastUserEvent )
                mnLastUserEvent = Application::PostUserEvent( LINK( this, UpdateCheckUI, UserEventHdl ) );
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#define TIP_HEIGHT             15
#define TIP_WIDTH               7
#define TIP_RIGHT_OFFSET       18
#define BUBBLE_BORDER          10
#define TEXT_MAX_WIDTH        300
#define TEXT_MAX_HEIGHT       200
#define INITIAL_SHOW_TIME   10000

//------------------------------------------------------------------------------
BubbleWindow::BubbleWindow( Window* pParent, const XubString& rTitle,
                            const XubString& rText, const Image& rImage )
    : FloatingWindow( pParent, WB_SYSTEMWINDOW
                               | WB_OWNERDRAWDECORATION
                               | WB_NOBORDER
                    )
{
    maMaxTextSize = Size( TEXT_MAX_WIDTH, TEXT_MAX_HEIGHT );
    maBubbleTitle = rTitle;
    maBubbleText = rText;
    maBubbleImage = rImage;

    SetBackground( Wallpaper( GetSettings().GetStyleSettings().GetHelpColor() ) );
}

//------------------------------------------------------------------------------
BubbleWindow::~BubbleWindow()
{
}

//------------------------------------------------------------------------------
void BubbleWindow::Resize()
{
    FloatingWindow::Resize();

    Size aSize = GetSizePixel();

    if ( ( aSize.Height() < 20 ) || ( aSize.Width() < 60 ) )
        return;

    Rectangle aRect( 0, TIP_HEIGHT, aSize.Width(), aSize.Height() - TIP_HEIGHT );
    maRectPoly = Polygon( aRect, 6, 6 );
    Region aRegion( maRectPoly );
    long nTipOffset = aSize.Width() - TIP_RIGHT_OFFSET;

    Point aPointArr[4];
    aPointArr[0] = Point( nTipOffset, TIP_HEIGHT );
    aPointArr[1] = Point( nTipOffset, 0 );
    aPointArr[2] = Point( nTipOffset + TIP_WIDTH , TIP_HEIGHT );
    aPointArr[3] = Point( nTipOffset, TIP_HEIGHT );
     maTriPoly = Polygon( 4, aPointArr );
    Region aTriRegion( maTriPoly );

    aRegion.Union( aTriRegion);
    maBounds = aRegion;

    SetWindowRegionPixel( maBounds );
}

//------------------------------------------------------------------------------
void BubbleWindow::Paint( const Rectangle& rRect )
{
    (void) rRect;

    LineInfo aThickLine( LINE_SOLID, 2 );

    DrawPolyLine( maRectPoly, aThickLine );
    DrawPolyLine( maTriPoly );

    Color aOldLine = GetLineColor();
    Size aSize = GetSizePixel();
    long nTipOffset = aSize.Width() - TIP_RIGHT_OFFSET;

    SetLineColor( GetSettings().GetStyleSettings().GetHelpColor() );
    DrawLine( Point( nTipOffset+2, TIP_HEIGHT ),
              Point( nTipOffset + TIP_WIDTH -1 , TIP_HEIGHT ),
              aThickLine );
    SetLineColor( aOldLine );

    //Image aImage = InfoBox::GetStandardImage();
    Size aImgSize = maBubbleImage.GetSizePixel();

    DrawImage( Point( BUBBLE_BORDER, BUBBLE_BORDER + TIP_HEIGHT ), maBubbleImage );

    Font aOldFont = GetFont();
    Font aBoldFont = aOldFont;
    aBoldFont.SetWeight( WEIGHT_BOLD );

    SetFont( aBoldFont );
    Rectangle aTitleRect = maTitleRect;
    aTitleRect.Move( aImgSize.Width(), 0 );
    DrawText( aTitleRect, maBubbleTitle, TEXT_DRAW_MULTILINE | TEXT_DRAW_WORDBREAK );

    SetFont( aOldFont );
    Rectangle aTextRect = maTextRect;
    aTextRect.Move( aImgSize.Width(), 0 );
    DrawText( aTextRect, maBubbleText, TEXT_DRAW_MULTILINE | TEXT_DRAW_WORDBREAK );
}

//------------------------------------------------------------------------------
void BubbleWindow::MouseButtonDown( const MouseEvent& )
{
    Show( FALSE );
}

//------------------------------------------------------------------------------
void BubbleWindow::Show( BOOL bVisible, USHORT nFlags )
{
    if ( !bVisible )
    {
        FloatingWindow::Show( bVisible );
        return;
    }

    Size aWindowSize = GetSizePixel();

    // Image aImage = InfoBox::GetStandardImage();
    Size aImgSize = maBubbleImage.GetSizePixel();

    RecalcTextRects();

    aWindowSize.setHeight( maTitleRect.GetHeight() * 7 / 4+ maTextRect.GetHeight() +
                           3 * BUBBLE_BORDER + TIP_HEIGHT );

    if ( maTitleRect.GetWidth() > maTextRect.GetWidth() )
        aWindowSize.setWidth( maTitleRect.GetWidth() );
    else
        aWindowSize.setWidth( maTextRect.GetWidth() );

    aWindowSize.setWidth( aWindowSize.Width() + 3 * BUBBLE_BORDER + aImgSize.Width() );

    if ( aWindowSize.Height() < aImgSize.Height() + TIP_HEIGHT + 2 * BUBBLE_BORDER )
        aWindowSize.setHeight( aImgSize.Height() + TIP_HEIGHT + 2 * BUBBLE_BORDER );

    Point aPos;
    aPos.X() = maTipPos.X() - aWindowSize.Width() + TIP_RIGHT_OFFSET;
    aPos.Y() = maTipPos.Y();
    SetPosSizePixel( aPos, aWindowSize );

    FloatingWindow::Show( bVisible, nFlags );
}

//------------------------------------------------------------------------------
void BubbleWindow::RecalcTextRects()
{
    Size aTotalSize;
    BOOL bFinished = FALSE;
    Font aOldFont = GetFont();
    Font aBoldFont = aOldFont;

    aBoldFont.SetWeight( WEIGHT_BOLD );

    while ( !bFinished )
    {
        SetFont( aBoldFont );

        maTitleRect = GetTextRect( Rectangle( Point( 0, 0 ), maMaxTextSize ),
                                   maBubbleTitle,
                                   TEXT_DRAW_MULTILINE | TEXT_DRAW_WORDBREAK );

        SetFont( aOldFont );
        maTextRect = GetTextRect( Rectangle( Point( 0, 0 ), maMaxTextSize ),
                                  maBubbleText,
                                  TEXT_DRAW_MULTILINE | TEXT_DRAW_WORDBREAK );

        aTotalSize.setHeight( maTitleRect.GetHeight() +
                              aBoldFont.GetHeight() * 3 / 4 +
                              maTextRect.GetHeight() +
                              3 * BUBBLE_BORDER + TIP_HEIGHT );
        if ( aTotalSize.Height() > maMaxTextSize.Height() )
        {
            maMaxTextSize.Width() = maMaxTextSize.Width() * 3 / 2;
            maMaxTextSize.Height() = maMaxTextSize.Height() * 3 / 2;
        }
        else
            bFinished = TRUE;
    }
    maTitleRect.Move( 2*BUBBLE_BORDER, BUBBLE_BORDER + TIP_HEIGHT );
    maTextRect.Move( 2*BUBBLE_BORDER, BUBBLE_BORDER + TIP_HEIGHT + maTitleRect.GetHeight() + aBoldFont.GetHeight() * 3 / 4 );
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

} // anonymous namespace

//------------------------------------------------------------------------------

static uno::Reference<uno::XInterface> SAL_CALL
createInstance(const uno::Reference<uno::XComponentContext>& xContext)
{
    return  *new UpdateCheckUI(xContext);
}

//------------------------------------------------------------------------------

static const cppu::ImplementationEntry kImplementations_entries[] =
{
    {
        createInstance,
        getImplementationName,
        getServiceNames,
        cppu::createSingleComponentFactory,
        NULL,
        0
    },
    { NULL, NULL, NULL, NULL, NULL, 0 }
} ;

//------------------------------------------------------------------------------

extern "C" void SAL_CALL
component_getImplementationEnvironment( const sal_Char **aEnvTypeName, uno_Environment **)
{
    *aEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME ;
}

//------------------------------------------------------------------------------

extern "C" sal_Bool SAL_CALL
component_writeInfo(void *pServiceManager, void *pRegistryKey)
{
    return cppu::component_writeInfoHelper(
        pServiceManager,
        pRegistryKey,
        kImplementations_entries
    );
}

//------------------------------------------------------------------------------

extern "C" void *
component_getFactory(const sal_Char *pszImplementationName, void *pServiceManager, void *pRegistryKey)
{
    return cppu::component_getFactoryHelper(
        pszImplementationName,
        pServiceManager,
        pRegistryKey,
        kImplementations_entries) ;
}

