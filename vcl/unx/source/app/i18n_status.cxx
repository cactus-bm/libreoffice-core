/*************************************************************************
 *
 *  $RCSfile: i18n_status.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: pl $ $Date: 2001-11-07 16:24:45 $
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

#ifdef DEBUG
#include <stdio.h>
#endif
#if !(defined FREEBSD || defined NETBSD)
#include <alloca.h>
#endif

#include <prex.h>
#include <X11/Xlib.h>
#include <XIM.h>
#include <postx.h>

#include <salunx.h>

#include <i18n_status.hxx>
#include <i18n_ic.hxx>

#include <wrkwin.hxx>
#include <fixed.hxx>
#include <menubtn.hxx>
#include <menu.hxx>
#include <svdata.hxx>
#include <saldisp.hxx>
#include <salframe.hxx>
#include <saldata.hxx>

using namespace vcl;
using namespace rtl;

namespace vcl {

class StatusWindow : public WorkWindow
{
protected:
    StatusWindow( int nWinBits );
public:
    virtual ~StatusWindow();

    virtual void setPosition( SalFrame* );
    virtual void setText( const String & ) = 0;
    virtual const String& getText() const = 0;
    virtual void show( bool bShow );
};

}

StatusWindow::StatusWindow( int nWinBits ) :
        WorkWindow( NULL, nWinBits )
{
}

StatusWindow::~StatusWindow() {}

void StatusWindow::setPosition( SalFrame* pFrame )
{
}

void StatusWindow::show( bool bShow )
{
    Show( bShow );
}

// --------------------------------------------------------------------------

namespace vcl {

class XIMStatusWindow : public StatusWindow
{
    FixedText               m_aStatusText;
    SalFrame*               m_pLastParent;
public:
    XIMStatusWindow();
    virtual ~XIMStatusWindow();

    virtual void setPosition( SalFrame* );
    virtual void setText( const String & );
    virtual const String& getText() const;
    virtual void show( bool bShow );
};

}

XIMStatusWindow::XIMStatusWindow() :
        StatusWindow( WB_BORDER ),
        m_aStatusText( this, 0 ),
        m_pLastParent( NULL )
{
    Size aSize( PixelToLogic( Size( 150, 2 ) ) );
    Size aControlSize( PixelToLogic( Size( 146, 2 ) ) );

    Font aFont( m_aStatusText.GetFont() );
    if( aFont.GetHeight() < 12 )
        aFont.SetHeight( 12 );
    aSize.Height() = aFont.GetHeight()+10;
    aSize = LogicToPixel( aSize );
    aControlSize.Height() = aSize.Height()-4;

    m_aStatusText.SetPosSizePixel( Point( 1, 1 ), aControlSize );
    m_aStatusText.SetFont( aFont );
    m_aStatusText.Show( TRUE );

    SetOutputSizePixel( aSize );
}

XIMStatusWindow::~XIMStatusWindow()
{
}

void XIMStatusWindow::setPosition( SalFrame* pParent )
{
    if( pParent )
    {
        if( pParent != m_pLastParent )
        {
            setText( String() );
            m_pLastParent = pParent;
            Show( FALSE );
        }

        const SystemEnvData* pEnvData = GetSystemData();
        const SystemEnvData* pParentEnvData = pParent->GetSystemData();
        SalFrame* pStatusFrame = (SalFrame*)pEnvData->pSalFrame;

        SalExtTextInputPosEvent aPosEvent;
        pParent->maFrameData.Call(SALEVENT_EXTTEXTINPUTPOS, (void*)&aPosEvent);
        int x, y;
        XLIB_Window aChild;
        XTranslateCoordinates( (Display*)pParentEnvData->pDisplay,
                               (XLIB_Window)pParentEnvData->aShellWindow,
                               pParent->maFrameData.GetDisplay()->GetRootWindow(),
                               0, 0,
                               &x, &y,
                               &aChild );
        Point aCursorPos( x, y );
        aCursorPos.X() += aPosEvent.mnX;
        aCursorPos.Y() += aPosEvent.mnY+aPosEvent.mnHeight + 4;
        const SalFrame::Geometry& rGeom( pStatusFrame->GetGeometry() );
        Rectangle aRect( Point( rGeom.nX, rGeom.nY ), Size( rGeom.nWidth, rGeom.nHeight ) );
        Size aStatusSize( aRect.GetSize() );
        aStatusSize.Width() = GetTextWidth( m_aStatusText.GetText() )+8;
        aRect.SetPos( aCursorPos );
        aRect.SetSize( aStatusSize );
        pStatusFrame->maFrameData.setPosSize( aRect );
    }
}

void XIMStatusWindow::show( bool bShow )
{
    if( bShow && ! m_aStatusText.GetText().Len() )
        bShow = false;
    Show( bShow );
    if( bShow )
    {
        const SystemEnvData* pData = GetSystemData();
        XRaiseWindow( (Display*)pData->pDisplay,
                      (XLIB_Window)pData->aShellWindow );
    }
}

void XIMStatusWindow::setText( const String& rText )
{
    m_aStatusText.SetText( rText );
}

const String& XIMStatusWindow::getText() const
{
    return m_aStatusText.GetText();
}

// --------------------------------------------------------------------------

namespace vcl {

class IIIMPStatusWindow : public StatusWindow
{
    MenuButton              m_aStatusBtn;
    PopupMenu               m_aMenu;
    SalFrame*               m_pResetFocus;

    DECL_LINK( SelectHdl, MenuButton* );
public:
    IIIMPStatusWindow( SalFrame* pParent  ); // for initial position
    virtual ~IIIMPStatusWindow();

    virtual void setText( const String & );
    virtual const String& getText() const;
    virtual void show( bool bShow );

    // overload Window focus handler
    virtual void        GetFocus();
};

}

IIIMPStatusWindow::IIIMPStatusWindow( SalFrame* pParent ) :
        StatusWindow( WB_MOVEABLE ),
        m_aStatusBtn( this, WB_BORDER ),
        m_pResetFocus( pParent )
{
    SetText( String( RTL_CONSTASCII_USTRINGPARAM( "IME Status" ) ) );

    Size aSize( PixelToLogic( Size( 180, 2 ) ) );
    Size aControlSize( 180, 2 );

    const SystemEnvData* pEnvData = GetSystemData();
    const SystemEnvData* pParentEnvData = pParent->GetSystemData();

    m_aStatusBtn.SetSelectHdl( LINK( this, IIIMPStatusWindow, SelectHdl ) );
    Font aFont( m_aStatusBtn.GetFont() );
    if( aFont.GetHeight() < 12 )
        aFont.SetHeight( 12 );
    aSize.Height() = aFont.GetHeight()+10;
    aSize = LogicToPixel( aSize );
    aControlSize.Height() = aSize.Height();

    m_aStatusBtn.SetPosSizePixel( Point( 0, 0 ), aControlSize );
    m_aStatusBtn.SetFont( aFont );
    m_aStatusBtn.SetPopupMenu( &m_aMenu );
    m_aStatusBtn.Show( TRUE );

    const ::std::vector< I18NStatus::ChoiceData >& rChoices( I18NStatus::get().getChoices() );
    int i = 1;
    for( ::std::vector< I18NStatus::ChoiceData >::const_iterator it = rChoices.begin(); it != rChoices.end(); ++it, i++ )
        m_aMenu.InsertItem( i, it->aString );

    SetOutputSizePixel( aSize );

    if( pParent )
    {
        const SalFrame::Geometry& rGeom( pParent->GetGeometry() );
        XMoveWindow( (Display*)pEnvData->pDisplay,
                     (XLIB_Window)pEnvData->aShellWindow,
                     rGeom.nX,
                     rGeom.nY + rGeom.nHeight + 20 // leave a little more space
                     );
    }
#ifdef DEBUG
    else
        fprintf( stderr, "Warning: could not reposition status window since no frame\n" );
#endif
    EnableAlwaysOnTop( TRUE );
}

IIIMPStatusWindow::~IIIMPStatusWindow()
{
}

void IIIMPStatusWindow::setText( const String& rText )
{
    m_aStatusBtn.SetText( rText );
}

const String& IIIMPStatusWindow::getText() const
{
    return m_aStatusBtn.GetText();
}

void IIIMPStatusWindow::show( bool bShow )
{
    Show( bShow );
}

void IIIMPStatusWindow::GetFocus()
{
    /*
     *  this is here just to put the focus back to the application
     *  window at startup on clickToFocus WMs
     */
    WorkWindow::GetFocus();
    if( m_pResetFocus )
    {
        /*
         *  look if reset focus still exists
         *  since reset focus really is an internal hack there should
         *  not be a method to be called in SalFrame destructor
         */
        SalFrame* pFrame = GetSalData()->pFirstFrame_;
        while( pFrame && pFrame != m_pResetFocus )
            pFrame = pFrame->maFrameData.GetNextFrame();
        if( pFrame == m_pResetFocus )
        {
            const SystemEnvData* pParentEnvData = m_pResetFocus->GetSystemData();
            BOOL bIgnore = m_pResetFocus->maFrameData.GetDisplay()->GetXLib()->GetIgnoreXErrors();
            m_pResetFocus->maFrameData.GetDisplay()->GetXLib()->SetIgnoreXErrors( TRUE );
            XSetInputFocus( (Display*)pParentEnvData->pDisplay,
                            (XLIB_Window)pParentEnvData->aShellWindow,
                            RevertToNone,
                            CurrentTime
                            );
            XSync( (Display*)pParentEnvData->pDisplay, False );
            m_pResetFocus->maFrameData.GetDisplay()->GetXLib()->SetIgnoreXErrors( bIgnore );
        }
        m_pResetFocus = NULL;
    }
}

// --------------------------------------------------------------------------

IMPL_LINK( IIIMPStatusWindow, SelectHdl, MenuButton*, pBtn )
{
    if( pBtn == & m_aStatusBtn )
    {
        const ::std::vector< I18NStatus::ChoiceData >& rChoices( I18NStatus::get().getChoices() );
        int nIndex = m_aStatusBtn.GetCurItemId()-1;
        if( nIndex < rChoices.size() )
        {
            bool bDummy; // should always be false in this case
            XSetICValues( I18NStatus::get().getInputContext( bDummy )->GetContext(),
                          XNUnicodeCharacterSubset,
                          rChoices[nIndex].pData,
                          0);
            SalFrame* pParent = I18NStatus::get().getParent();
            if( pParent && pParent->maFrameData.isMapped() )
            {
                BOOL bIgnore = pParent->maFrameData.GetDisplay()->GetXLib()->GetIgnoreXErrors();
                pParent->maFrameData.GetDisplay()->GetXLib()->SetIgnoreXErrors( TRUE );
                XSetInputFocus( pParent->maFrameData.GetXDisplay(),
                                pParent->maFrameData.GetShellWindow(),
                                RevertToNone,
                                CurrentTime
                                );
                XSync( pParent->maFrameData.GetXDisplay(), False );
                pParent->maFrameData.GetDisplay()->GetXLib()->SetIgnoreXErrors( bIgnore );
            }
        }
    }
    return 0;
}

/*
 *  I18NStatus
 */

I18NStatus* I18NStatus::pInstance = NULL;

I18NStatus& I18NStatus::get()
{
    if( ! pInstance )
        pInstance = new I18NStatus();
    return *pInstance;
}

// --------------------------------------------------------------------------

void I18NStatus::free()
{
    if( pInstance )
        delete pInstance, pInstance = NULL;
}

// --------------------------------------------------------------------------

I18NStatus::I18NStatus() :
        m_pParent( NULL ),
        m_pStatusWindow( NULL ),
        m_pInputContext( NULL )
{
}

// --------------------------------------------------------------------------

I18NStatus::~I18NStatus()
{
    if( m_pInputContext )
        delete m_pInputContext, m_pInputContext = NULL;
    if( m_pStatusWindow )
        delete m_pStatusWindow, m_pStatusWindow = NULL;
    if( pInstance == this )
        pInstance = NULL;
}

// --------------------------------------------------------------------------

SalI18N_InputContext* I18NStatus::getInputContext( bool& bDeleteAfterUse )
{
    SalI18N_InputContext* pRet = NULL;
    if( m_aChoices.end() != m_aChoices.begin() )
    {
        if( ! m_pInputContext )
            m_pInputContext = new SalI18N_InputContext( m_pParent );
        bDeleteAfterUse = false;
        pRet = m_pInputContext;
    }
    else
    {
        pRet = new SalI18N_InputContext( m_pParent );
        bDeleteAfterUse = true;
    }
    return pRet;
}

// --------------------------------------------------------------------------

void I18NStatus::setParent( SalFrame* pParent )
{
    m_pParent = pParent;
    if( ! m_pStatusWindow )
    {
        bool bIIIMPmode = m_aChoices.begin() != m_aChoices.end();
        if( bIIIMPmode )
            m_pStatusWindow = new IIIMPStatusWindow( pParent );
        else
            m_pStatusWindow = new XIMStatusWindow();
        setStatusText( m_aCurrentIM );
    }
    m_pStatusWindow->setPosition( m_pParent );
}

// --------------------------------------------------------------------------

void I18NStatus::show( bool bShow )
{
    if( m_pStatusWindow )
    {
        m_pStatusWindow->setPosition( m_pParent );
        m_pStatusWindow->show( bShow );
    }
}

// --------------------------------------------------------------------------

void I18NStatus::setStatusText( const String& rText )
{
    if( m_pStatusWindow )
    {
        /*
         *  #93614# convert fullwidth ASCII forms to ascii
         */
        int nChars = rText.Len()+1;
        sal_Unicode* pBuffer = (sal_Unicode*)alloca( nChars*sizeof( sal_Unicode ) );
        const sal_Unicode* pCopy = rText.GetBuffer();
        for( int i = 0; i < nChars; i++ )
        {
            if( pCopy[i] >=0xff00 && pCopy[i] <= 0xff5f )
                pBuffer[i] = (pCopy[i] & 0xff) + 0x20;
            else
                pBuffer[i] = pCopy[i];
        }
        String aText( pBuffer );
        m_pStatusWindow->setText( aText );
        m_pStatusWindow->setPosition( m_pParent );
        m_pStatusWindow->show( true );
    }
}

// --------------------------------------------------------------------------

void I18NStatus::changeIM( const String& rIM )
{
    m_aCurrentIM = rIM;
}

// --------------------------------------------------------------------------

const String& I18NStatus::getStatusText() const
{
    static String aEmpty;
    return m_pStatusWindow ? m_pStatusWindow->getText() :  aEmpty;
}

// --------------------------------------------------------------------------

void I18NStatus::clearChoices()
{
    m_aChoices.clear();
}

// --------------------------------------------------------------------------

void I18NStatus::addChoice( const String& rChoice, void* pData )
{
    ChoiceData aData;
    aData.pData     = pData;
    aData.aString   = rChoice;
    m_aChoices.push_back( aData );
}

// --------------------------------------------------------------------------

void I18NStatus::toTop() const
{
    if( m_pStatusWindow )
    {
        const SystemEnvData* pData = m_pStatusWindow->GetSystemData();
        XRaiseWindow( (Display*)pData->pDisplay,
                      (XLIB_Window)pData->aShellWindow );
    }
}

// --------------------------------------------------------------------------

SalFrame* I18NStatus::getStatusFrame() const
{
    SalFrame* pRet = NULL;
    if( m_pStatusWindow )
    {
        const SystemEnvData* pData = m_pStatusWindow->GetSystemData();
        pRet = (SalFrame*)pData->pSalFrame;
    }
    return pRet;
}
