/*************************************************************************
 *
 *  $RCSfile: bibcont.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: gt $ $Date: 2002-04-25 09:27:20 $
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

#include <osl/mutex.hxx>
#include <tools/urlobj.hxx>
#include <cppuhelper/weak.hxx>
#ifndef _UNOTOOLS_PROCESSFACTORY_HXX_
#include <unotools/processfactory.hxx>
#endif
#include <toolkit/helper/vclunohelper.hxx>
#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XWINDOWPEER_HPP_
#include <com/sun/star/awt/XWindowPeer.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_FRAMESEARCHFLAG_HPP_
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XURLTRANSFORMER_HPP_
#include <com/sun/star/util/XURLTransformer.hpp>
#endif
#ifndef _BIBCONFIG_HXX
#include "bibconfig.hxx"
#endif


#include "datman.hxx"
#include "bibcont.hxx"

using namespace osl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::frame;
using namespace ::rtl;

#define C2U(cChar) OUString::createFromAscii(cChar)
#define PROPERTY_FRAME                      1


BibWindowContainer::BibWindowContainer( Window* pParent, WinBits nStyle ) :
        Window( pParent, nStyle ),
        pChild( NULL )
{
}

BibWindowContainer::BibWindowContainer( Window* pParent,Window* pWin, WinBits nStyle ) :
        Window( pParent, nStyle ),
        pChild( pWin )
{
    if(pChild!=NULL)
    {
        pChild->SetParent(this);
        pChild->Show();
        pChild->SetPosPixel(Point(0,0));
    }
}

BibWindowContainer::~BibWindowContainer()
{
    if( pChild )
    {
        Window* pDel = pChild;
        pChild = NULL;          // prevents GetFocus for child while deleting!
        delete pDel;
    }
}

void BibWindowContainer::Resize()
{
    ::Size aSize=GetOutputSizePixel();
    if(pChild!=NULL) pChild->SetSizePixel(aSize);
}

void BibWindowContainer::SetChild(Window* pWin)
{
    pChild=pWin;
    if(pChild!=NULL)
    {
        pChild->SetParent(this);
        pChild->Show();
        pChild->SetPosPixel(Point(0,0));
    }
}

void BibWindowContainer::GetFocus()
{
    if( pChild )
        pChild->GrabFocus();
}


BibBookContainer::BibBookContainer(Window* pParent,BibDataManager* pDtMn, WinBits nStyle):
    SplitWindow(pParent,nStyle),
    pDatMan(pDtMn),
    pTopWin(NULL),
    pBottomWin(NULL),
    bFirstTime(sal_True)
{
    pBibMod = OpenBibModul();
    aTimer.SetTimeoutHdl(LINK( this, BibBookContainer, SplitHdl));
    aTimer.SetTimeout(400);
}

BibBookContainer::~BibBookContainer()
{
    if( xTopFrameRef.is() )
        xTopFrameRef->dispose();
    if( xBottomFrameRef.is() )
        xBottomFrameRef->dispose();

    if( pTopWin )
    {
        Window* pDel = pTopWin;
        pTopWin = NULL;         // prevents GetFocus for child while deleting!
        delete pDel;
    }

    if( pBottomWin )
    {
        Window* pDel = pBottomWin;
        pBottomWin = NULL;      // prevents GetFocus for child while deleting!
        delete pDel;
    }

    CloseBibModul( pBibMod );
}

void BibBookContainer::Split()
{
    aTimer.Start();
}
IMPL_LINK( BibBookContainer, SplitHdl, Timer*,pT)
{
    long nSize= GetItemSize( TOP_WINDOW);
    BibConfig* pConfig = BibModul::GetConfig();
    pConfig->setBeamerSize(nSize);
    nSize = GetItemSize( BOTTOM_WINDOW);
    pConfig->setViewSize(nSize);
    return 0;
}


uno::Reference < awt::XWindowPeer>  BibBookContainer::GetTopComponentInterface( sal_Bool bCreate)
{
    return pTopWin->GetComponentInterface(bCreate);
}
void BibBookContainer::SetTopComponentInterface( awt::XWindowPeer* pIFace )
{
    pTopWin->SetComponentInterface(pIFace);
}

uno::Reference < awt::XWindowPeer > BibBookContainer::GetBottomComponentInterface( sal_Bool bCreate)
{
    return pBottomWin->GetComponentInterface(bCreate);
}
void BibBookContainer::SetBottomComponentInterface( awt::XWindowPeer* pIFace )
{
    pBottomWin->SetComponentInterface(pIFace);
}

void BibBookContainer::CreateTopWin()
{
    if( xTopFrameRef.is() )
        xTopFrameRef->dispose();

    if( pTopWin )
    {
        RemoveItem( TOP_WINDOW );
        delete pTopWin;
    }

    pTopWin = new Window( this, 0 );

    BibConfig* pConfig = BibModul::GetConfig();
    long nSize = pConfig->getBeamerSize();
    InsertItem( TOP_WINDOW, pTopWin, nSize, 0, 0, SWIB_PERCENTSIZE );

}

void BibBookContainer::CreateBottomWin()
{

    if( xBottomFrameRef.is() )
        xBottomFrameRef->dispose();

    if( pBottomWin )
    {
        RemoveItem( BOTTOM_WINDOW );
        delete pBottomWin;
    }

    pBottomWin = new Window( this, 0 );
    BibConfig* pConfig = BibModul::GetConfig();
    long nSize = pConfig->getViewSize();
    InsertItem( BOTTOM_WINDOW, pBottomWin, nSize, 1, 0, SWIB_PERCENTSIZE );

}

void BibBookContainer::createTopFrame(const ::rtl::OUString & rURL )
{
    uno::Reference< frame::XFrame >     xNewFrame;

    CreateTopWin();

    xTopPeerRef=uno::Reference< awt::XWindow > (GetTopComponentInterface(), UNO_QUERY );

    uno::Reference< lang::XMultiServiceFactory >  xMgr = comphelper::getProcessServiceFactory();

    xNewFrame = uno::Reference< frame::XFrame > ( xMgr->createInstance( C2U("com.sun.star.frame.Frame") ), UNO_QUERY );
    uno::Reference< frame::XDispatchProvider >  xDSP( xNewFrame, UNO_QUERY );
    if( xDSP.is() )
    {
        xNewFrame->initialize( xTopPeerRef);

        // optional:
        //xFrame->setName( C2U("WhatYouWant") );

        uno::Reference< util::XURLTransformer >  xTrans ( xMgr->createInstance( C2U("com.sun.star.util.URLTransformer") ), UNO_QUERY );
        if( xTrans.is() )
        {
            // Datei laden
            util::URL aURL;
            aURL.Complete = rURL;

            xTrans->parseStrict( aURL );

            uno::Reference< frame::XDispatch >  xDisp = xDSP->queryDispatch( aURL, C2U("_self"), FrameSearchFlag::SELF );
            if ( xDisp.is() )
                xDisp->dispatch( aURL, uno::Sequence<beans::PropertyValue>() );
        }
    }

    // set the frame
    {
    MutexGuard aGuard(Mutex::getGlobalMutex());
    xTopFrameRef = xNewFrame;
    }

}

void BibBookContainer::createBottomFrame(const ::rtl::OUString & rURL )
{
    uno::Reference< frame::XFrame >     xNewFrame;

    CreateBottomWin();

    xBottomPeerRef=uno::Reference< awt::XWindow > (GetBottomComponentInterface(), UNO_QUERY );

    uno::Reference< lang::XMultiServiceFactory >  xMgr = comphelper::getProcessServiceFactory();

    xNewFrame = uno::Reference< frame::XFrame > ( xMgr->createInstance( C2U("com.sun.star.frame.Frame") ), UNO_QUERY );
    uno::Reference< frame::XDispatchProvider >  xDSP( xNewFrame, UNO_QUERY );
    if( xDSP.is() )
    {
        xNewFrame->initialize( xBottomPeerRef);

        // optional:
        //xFrame->setName( C2U("WhatYouWant") );

        uno::Reference< util::XURLTransformer >  xTrans ( xMgr->createInstance( C2U("com.sun.star.util.URLTransformer") ), UNO_QUERY );
        if( xTrans.is() )
        {
            // Datei laden
            util::URL aURL;
            aURL.Complete = rURL;

            xTrans->parseStrict( aURL );

            uno::Reference< frame::XDispatch >  xDisp = xDSP->queryDispatch( aURL, ::rtl::OUString(), FrameSearchFlag::SELF );
            if ( xDisp.is() )
                xDisp->dispatch( aURL, uno::Sequence<beans::PropertyValue>() );
        }
    }

    // set the frame
    {
    MutexGuard aGuard(Mutex::getGlobalMutex());
    xBottomFrameRef = xNewFrame;
    }

}

void BibBookContainer::createTopFrame(Window* pWin)
{
    if ( xTopFrameRef.is() ) xTopFrameRef->dispose();

    if(pTopWin)
    {
        RemoveItem(TOP_WINDOW);
        delete pTopWin;
    }
    pTopWin=new BibWindowContainer(this,pWin);
    pTopWin->Show();
    BibConfig* pConfig = BibModul::GetConfig();
    long nSize = pConfig->getBeamerSize();
    InsertItem(TOP_WINDOW, pTopWin, nSize, 1, 0, SWIB_PERCENTSIZE  );

}

void BibBookContainer::createBottomFrame(Window* pWin)
{
    if ( xBottomFrameRef.is() ) xBottomFrameRef->dispose();

    if(pBottomWin)
    {
        RemoveItem(BOTTOM_WINDOW);
        delete pBottomWin;
    }

    pBottomWin=new BibWindowContainer(this,pWin);

    BibConfig* pConfig = BibModul::GetConfig();
    long nSize = pConfig->getViewSize();
    InsertItem(BOTTOM_WINDOW, pBottomWin, nSize, 1, 0, SWIB_PERCENTSIZE  );

}

void BibBookContainer::GetFocus()
{
    if( pBottomWin )
        pBottomWin->GrabFocus();
}
