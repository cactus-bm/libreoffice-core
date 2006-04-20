/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: depapp.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2006-04-20 15:15:01 $
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

// -----------------------------------------------------------------------
#include "depapp.hxx"


void MyApp::Main()
{
    pDebugFile = fopen( "fprintf.out", "w" );
    pMyApp = GetpApp();
    MyWin aMainWin( NULL, WB_APP | WB_STDWORK );
    pAppWindow = &aMainWin;
   // pToolBarFrame = new FloatingWindow( aMainWin, WB_STDWORK );
    //pToolBox = new ToolBox(pToolBarFrame,DtSodResId(TID_SOLDEP_MAIN));


    pSolDep = new SolDep( &aMainWin );
    pSolDep->Init();
    aMainWin.SetText( String::CreateFromAscii( SOLDEPL_NAME ));

    aMainWin.Show();    Help aHelp;
    SetHelp(&aHelp);
    aHelp.EnableContextHelp();
    aHelp.EnableQuickHelp();
    Execute();
    delete pResMgr;
    delete pSolDep;
}

// -----------------------------------------------------------------------

MyWin::MyWin( Window* pParent, WinBits nWinStyle ) :
    WorkWindow( pParent, nWinStyle )//,aToolBox( this, DtSodResId(TID_SOLDEP_MAIN)),
        // depper aTaskBarFrame(this, 0)
{
//depper    aTaskBarFrame.EnableAlwaysOnTop();

//depper    aMenuBar.InsertItem( 1, XubString( RTL_CONSTASCII_USTRINGPARAM( "~Source" ) ) );
//depper    aMenuBar.InsertItem( 2, XubString( RTL_CONSTASCII_USTRINGPARAM( "~Exit" ) ) );
//depper    SetMenuBar( &aMenuBar );
//depper    aToolBox.SetPosSizePixel( Point( 0,0 ), Size( 1100,35 ));
//    aToolBox.Show();
}

// -----------------------------------------------------------------------

void MyWin::MouseMove( const MouseEvent& rMEvt )
{
    WorkWindow::MouseMove( rMEvt );
}

// -----------------------------------------------------------------------

void MyWin::MouseButtonDown( const MouseEvent& rMEvt )
{
    WorkWindow::MouseButtonDown( rMEvt );
}

// -----------------------------------------------------------------------

void MyWin::MouseButtonUp( const MouseEvent& rMEvt )
{
    WorkWindow::MouseButtonUp( rMEvt );
}

// -----------------------------------------------------------------------

void MyWin::KeyInput( const KeyEvent& rKEvt )
{
    WorkWindow::KeyInput( rKEvt );
}

// -----------------------------------------------------------------------

void MyWin::KeyUp( const KeyEvent& rKEvt )
{
    WorkWindow::KeyUp( rKEvt );
}

// -----------------------------------------------------------------------

void MyWin::Paint( const Rectangle& rRect )
{
    WorkWindow::Paint( rRect );
}

// -----------------------------------------------------------------------

void MyWin::Resize()
{
    ((MyApp*)GetpApp())->GetSolDep()->Resize();
    WorkWindow::Resize();
}

SAL_IMPLEMENT_MAIN()
{
    Reference< XMultiServiceFactory > xMS;

    // for this to work make sure an <appname>.ini file is available, you can just copy soffice.ini
    Reference< XComponentContext > xComponentContext = ::cppu::defaultBootstrap_InitialComponentContext();
    xMS = cppu::createRegistryServiceFactory(
                  rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "applicat.rdb" ) ), sal_True );

    InitVCL( xMS );
    aMyApp.Main();
    DeInitVCL();
    return 0;
}
