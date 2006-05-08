/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: comboboxtoolbarcontroller.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2006-05-08 15:18:49 $
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

#ifndef __FRAMEWORK_UIELEMENT_COMBOBOXTOOLBARCONTROLLER_HXX
#include "uielement/comboboxtoolbarcontroller.hxx"
#endif

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_TOOLBAR_HXX_
#include "uielement/toolbar.hxx"
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_UTIL_XURLTRANSFORMER_HPP_
#include <com/sun/star/util/XURLTransformer.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_STATUS_ITEMSTATUS_HPP_
#include <com/sun/star/frame/status/ItemStatus.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_STATUS_ITEMSTATE_HPP_
#include <com/sun/star/frame/status/ItemState.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_STATUS_VISIBILITY_HPP_
#include <com/sun/star/frame/status/Visibility.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCONTROLNOTIFICATIONLISTENER_HPP_
#include <com/sun/star/frame/XControlNotificationListener.hpp>
#endif

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef _SVTOOLS_TOOLBOXCONTROLLER_HXX
#include <svtools/toolboxcontroller.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _VCL_MNEMONIC_HXX_
#include <vcl/mnemonic.hxx>
#endif
#include <tools/urlobj.hxx>

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::frame::status;
using namespace ::com::sun::star::util;

namespace framework
{

// ------------------------------------------------------------------

// Wrapper class to notify controller about events from combobox.
// Unfortunaltly the events are notifed through virtual methods instead
// of Listeners.

class ComboBoxControl : public ComboBox
{
    public:
        ComboBoxControl( Window* pParent, WinBits nStyle, IComboBoxListener* pComboBoxListener );
        virtual ~ComboBoxControl();

        virtual void Select();
        virtual void DoubleClick();
        virtual void Modify();
        virtual void KeyInput( const ::KeyEvent& rKEvt );
        virtual void GetFocus();
        virtual void LoseFocus();
        virtual long PreNotify( NotifyEvent& rNEvt );

    private:
        IComboBoxListener* m_pComboBoxListener;
};

ComboBoxControl::ComboBoxControl( Window* pParent, WinBits nStyle, IComboBoxListener* pComboBoxListener ) :
    ComboBox( pParent, nStyle )
    , m_pComboBoxListener( pComboBoxListener )
{
}

ComboBoxControl::~ComboBoxControl()
{
    m_pComboBoxListener = 0;
}

void ComboBoxControl::Select()
{
    ComboBox::Select();
    if ( m_pComboBoxListener )
        m_pComboBoxListener->Select();
}

void ComboBoxControl::DoubleClick()
{
    ComboBox::DoubleClick();
    if ( m_pComboBoxListener )
        m_pComboBoxListener->DoubleClick();
}

void ComboBoxControl::Modify()
{
    ComboBox::Modify();
    if ( m_pComboBoxListener )
        m_pComboBoxListener->Modify();
}

void ComboBoxControl::KeyInput( const ::KeyEvent& rKEvt )
{
    ComboBox::KeyInput( rKEvt );
    if ( m_pComboBoxListener )
        m_pComboBoxListener->KeyInput( rKEvt );
}

void ComboBoxControl::GetFocus()
{
    ComboBox::GetFocus();
    if ( m_pComboBoxListener )
        m_pComboBoxListener->GetFocus();
}

void ComboBoxControl::LoseFocus()
{
    ComboBox::LoseFocus();
    if ( m_pComboBoxListener )
        m_pComboBoxListener->LoseFocus();
}

long ComboBoxControl::PreNotify( NotifyEvent& rNEvt )
{
    long nRet( 0 );
    if ( m_pComboBoxListener )
        nRet = m_pComboBoxListener->PreNotify( rNEvt );
    if ( nRet == 0 )
        nRet = ComboBox::PreNotify( rNEvt );

    return nRet;
}

// ------------------------------------------------------------------

ComboboxToolbarController::ComboboxToolbarController(
    const Reference< XMultiServiceFactory >& rServiceManager,
    const Reference< XFrame >&               rFrame,
    ToolBar*                                 pToolbar,
    USHORT                                   nID,
    sal_Int32                                nWidth,
    const OUString&                          aCommand ) :
    ComplexToolbarController( rServiceManager, rFrame, pToolbar, nID, aCommand )
    ,   m_pComboBox( 0 )
{
    m_pComboBox = new ComboBoxControl( m_pToolbar,
                                       WB_BORDER|WB_CLIPCHILDREN|WB_DROPDOWN|WB_AUTOSIZE,
                                       this );
    if ( nWidth == 0 )
        nWidth = 100;

    m_pComboBox->SetSizePixel( ::Size( nWidth, 150 ));
    m_pToolbar->SetItemWindow( m_nID, m_pComboBox );
    m_pComboBox->SetDropDownLineCount( 5 );
}

// ------------------------------------------------------------------

ComboboxToolbarController::~ComboboxToolbarController()
{
}

// ------------------------------------------------------------------

void SAL_CALL ComboboxToolbarController::dispose()
throw ( RuntimeException )
{
    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

    m_pToolbar->SetItemWindow( m_nID, 0 );
    delete m_pComboBox;

    ComplexToolbarController::dispose();

    m_pComboBox = 0;
}

// ------------------------------------------------------------------

void SAL_CALL ComboboxToolbarController::execute( sal_Int16 KeyModifier )
throw ( RuntimeException )
{
    Reference< XDispatch >       xDispatch;
    Reference< XURLTransformer > xURLTransformer;
    OUString                     aCommandURL;
    OUString                     aSelectedText;
    ::com::sun::star::util::URL  aTargetURL;

    {
        vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

        if ( m_bDisposed )
            throw DisposedException();

        if ( m_bInitialized &&
             m_xFrame.is() &&
             m_xServiceManager.is() &&
             m_aCommandURL.getLength() )
        {
            xURLTransformer = m_xURLTransformer;
            xDispatch = getDispatchFromCommand( m_aCommandURL );
            aCommandURL = m_aCommandURL;
            aTargetURL = getInitializedURL();
            aSelectedText = m_pComboBox->GetText();
        }
    }

    if ( xDispatch.is() && aTargetURL.Complete.getLength() > 0 )
    {
        Sequence<PropertyValue>   aArgs( 2 );

        // Add key modifier to argument list
        aArgs[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "KeyModifier" ));
        aArgs[0].Value <<= KeyModifier;
        aArgs[1].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Text" ));
        aArgs[1].Value <<= aSelectedText;

        // Execute dispatch asynchronously
        ExecuteInfo* pExecuteInfo = new ExecuteInfo;
        pExecuteInfo->xDispatch     = xDispatch;
        pExecuteInfo->aTargetURL    = aTargetURL;
        pExecuteInfo->aArgs         = aArgs;
        Application::PostUserEvent( STATIC_LINK(0, ComplexToolbarController , ExecuteHdl_Impl), pExecuteInfo );
    }
}

// ------------------------------------------------------------------

void ComboboxToolbarController::Select()
{
    if ( m_pComboBox->GetEntryCount() > 0 )
    {
        Window::PointerState aState = m_pComboBox->GetPointerState();

        sal_uInt16 nKeyModifier = sal_uInt16( aState.mnState & KEY_MODTYPE );
        execute( nKeyModifier );
    }
}

void ComboboxToolbarController::DoubleClick()
{
}

void ComboboxToolbarController::Modify()
{
    // send notification
    uno::Sequence< beans::NamedValue > aInfo( 1 );
    aInfo[0].Name  = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Text" ));
    aInfo[0].Value <<= rtl::OUString( m_pComboBox->GetText() );
    addNotifyInfo( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "TextChanged" )),
                   getDispatchFromCommand( m_aCommandURL ),
                   aInfo );
}

void ComboboxToolbarController::KeyInput( const ::KeyEvent& rKEvt )
{
}

void ComboboxToolbarController::GetFocus()
{
    // send notification
    uno::Sequence< beans::NamedValue > aInfo;
    addNotifyInfo( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "FocusSet" )),
                    getDispatchFromCommand( m_aCommandURL ),
                    aInfo );
}

void ComboboxToolbarController::LoseFocus()
{
    // send notification
    uno::Sequence< beans::NamedValue > aInfo;
    addNotifyInfo( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "FocusLost" )),
                    getDispatchFromCommand( m_aCommandURL ),
                    aInfo );
}

long ComboboxToolbarController::PreNotify( NotifyEvent& rNEvt )
{
    if( rNEvt.GetType() == EVENT_KEYINPUT )
    {
        const ::KeyEvent* pKeyEvent = rNEvt.GetKeyEvent();
        const KeyCode& rKeyCode = pKeyEvent->GetKeyCode();
        if(( rKeyCode.GetModifier() | rKeyCode.GetCode()) == KEY_RETURN )
        {
            // Call execute only with non-empty text
            if ( m_pComboBox->GetText().Len() > 0 )
                execute( rKeyCode.GetModifier() );
            return 1;
        }
    }

    return 0;
}

// --------------------------------------------------------

void ComboboxToolbarController::executeControlCommand( const ::com::sun::star::frame::ControlCommand& rControlCommand )
{
    if ( rControlCommand.Command.equalsAsciiL( "SetText", 7 ))
    {
        for ( sal_Int32 i = 0; i < rControlCommand.Arguments.getLength(); i++ )
        {
            if ( rControlCommand.Arguments[i].Name.equalsAsciiL( "Text", 4 ))
            {
                rtl::OUString aText;
                rControlCommand.Arguments[i].Value >>= aText;
                m_pComboBox->SetText( aText );

                // send notification
                uno::Sequence< beans::NamedValue > aInfo( 1 );
                aInfo[0].Name  = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Text" ));
                aInfo[0].Value <<= aText;
                addNotifyInfo( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "TextChanged" )),
                               getDispatchFromCommand( m_aCommandURL ),
                               aInfo );
                break;
            }
        }
    }
    else if ( rControlCommand.Command.equalsAsciiL( "SetList", 7 ))
    {
        for ( sal_Int32 i = 0; i < rControlCommand.Arguments.getLength(); i++ )
        {
            if ( rControlCommand.Arguments[i].Name.equalsAsciiL( "List", 4 ))
            {
                Sequence< OUString > aList;
                m_pComboBox->Clear();

                rControlCommand.Arguments[i].Value >>= aList;
                for ( sal_Int32 j = 0; j < aList.getLength(); j++ )
                    m_pComboBox->InsertEntry( aList[j] );

                // send notification
                uno::Sequence< beans::NamedValue > aInfo( 1 );
                aInfo[0].Name  = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "List" ));
                aInfo[0].Value <<= aList;
                addNotifyInfo( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ListChanged" )),
                               getDispatchFromCommand( m_aCommandURL ),
                               aInfo );

                break;
            }
        }
    }
    else if ( rControlCommand.Command.equalsAsciiL( "InsertEntry", 11 ))
    {
        sal_uInt16      nPos( COMBOBOX_APPEND );
        rtl::OUString   aText;
        for ( sal_Int32 i = 0; i < rControlCommand.Arguments.getLength(); i++ )
        {
            if ( rControlCommand.Arguments[i].Name.equalsAsciiL( "Pos", 3 ))
            {
                sal_Int32 nTmpPos;
                if ( rControlCommand.Arguments[i].Value >>= nTmpPos )
                {
                    if (( nTmpPos >= 0 ) &&
                        ( nTmpPos < sal_Int32( m_pComboBox->GetEntryCount() )))
                        nPos = sal_uInt16( nTmpPos );
                }
            }
            else if ( rControlCommand.Arguments[i].Name.equalsAsciiL( "Text", 4 ))
                rControlCommand.Arguments[i].Value >>= aText;
        }

        m_pComboBox->InsertEntry( aText, nPos );
    }
    else if ( rControlCommand.Command.equalsAsciiL( "RemoveEntryPos", 14 ))
    {
        for ( sal_Int32 i = 0; i < rControlCommand.Arguments.getLength(); i++ )
        {
            if ( rControlCommand.Arguments[i].Name.equalsAsciiL( "Pos", 3 ))
            {
                sal_Int32 nPos( -1 );
                if ( rControlCommand.Arguments[i].Value >>= nPos )
                {
                    if ( nPos < sal_Int32( m_pComboBox->GetEntryCount() ))
                        m_pComboBox->RemoveEntry( sal_uInt16( nPos ));
                }
                break;
            }
        }
    }
    else if ( rControlCommand.Command.equalsAsciiL( "RemoveEntryText", 15 ))
    {
        for ( sal_Int32 i = 0; i < rControlCommand.Arguments.getLength(); i++ )
        {
            if ( rControlCommand.Arguments[i].Name.equalsAsciiL( "Text", 4 ))
            {
                rtl::OUString aText;
                if ( rControlCommand.Arguments[i].Value >>= aText )
                    m_pComboBox->RemoveEntry( aText );
                break;
            }
        }
    }
    else if ( rControlCommand.Command.equalsAsciiL( "SetDropDownLines", 16 ))
    {
        for ( sal_Int32 i = 0; i < rControlCommand.Arguments.getLength(); i++ )
        {
            if ( rControlCommand.Arguments[i].Name.equalsAsciiL( "Lines", 5 ))
            {
                sal_Int32 nValue( 5 );
                rControlCommand.Arguments[i].Value >>= nValue;
                m_pComboBox->SetDropDownLineCount( sal_uInt16( nValue ));
                break;
            }
        }
    }
}

} // namespace
