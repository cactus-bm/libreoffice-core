/*************************************************************************
 *
 *  $RCSfile: dockingareadefaultacceptor.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2004-02-25 17:46:44 $
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

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_HELPER_DOCKINGAREADEFAULTACCEPTOR_HXX_
#include <helper/dockingareadefaultacceptor.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_RESETABLEGUARD_HXX_
#include <threadhelp/resetableguard.hxx>
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_AWT_XDEVICE_HPP_
#include <com/sun/star/awt/XDevice.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_POSSIZE_HPP_
#include <com/sun/star/awt/PosSize.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_XLAYOUTCONSTRAINS_HPP_
#include <com/sun/star/awt/XLayoutConstrains.hpp>
#endif

//_________________________________________________________________________________________________________________
//  includes of other projects
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//  namespace
//_________________________________________________________________________________________________________________

namespace framework{

using namespace ::com::sun::star::container     ;
using namespace ::com::sun::star::frame         ;
using namespace ::com::sun::star::lang          ;
using namespace ::com::sun::star::uno           ;
using namespace ::cppu                          ;
using namespace ::osl                           ;
using namespace ::rtl                           ;

//_________________________________________________________________________________________________________________
//  non exported const
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//  non exported definitions
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//  declarations
//_________________________________________________________________________________________________________________

//*****************************************************************************************************************
//  constructor
//*****************************************************************************************************************
DockingAreaDefaultAcceptor::DockingAreaDefaultAcceptor( const   Reference< XFrame >&        xOwner  )
        //  Init baseclasses first
        :   OWeakObject     (           )
        // Init member
        ,   m_xOwner        ( xOwner    )
{
}

//*****************************************************************************************************************
//  destructor
//*****************************************************************************************************************
DockingAreaDefaultAcceptor::~DockingAreaDefaultAcceptor()
{
}

//*****************************************************************************************************************
//  XInterface, XTypeProvider
//*****************************************************************************************************************
DEFINE_XINTERFACE_2(    DockingAreaDefaultAcceptor                                              ,
                        OWeakObject                                                             ,
                        DIRECT_INTERFACE(XTypeProvider                                      )   ,
                        DIRECT_INTERFACE(::drafts::com::sun::star::ui::XDockingAreaAcceptor )   )

DEFINE_XTYPEPROVIDER_2( DockingAreaDefaultAcceptor                          ,
                        XTypeProvider                                       ,
                        ::drafts::com::sun::star::ui::XDockingAreaAcceptor  )

//*****************************************************************************************************************
//  XDockingAreaAcceptor
//*****************************************************************************************************************
css::uno::Reference< css::awt::XWindow > SAL_CALL DockingAreaDefaultAcceptor::getContainerWindow() throw (css::uno::RuntimeException)
{
    // Ready for multithreading
    ResetableGuard aGuard( m_aLock );

    // Try to "lock" the frame for access to taskscontainer.
    Reference< XFrame > xFrame( m_xOwner.get(), UNO_QUERY );
    Reference< css::awt::XWindow > xContainerWindow( xFrame->getContainerWindow() );

    return xContainerWindow;
}

sal_Bool SAL_CALL DockingAreaDefaultAcceptor::requestDockingAreaSpace( const css::awt::Rectangle& RequestedSpace ) throw (css::uno::RuntimeException)
{
    // Ready for multithreading
    ResetableGuard aGuard( m_aLock );

    // Try to "lock" the frame for access to taskscontainer.
    Reference< XFrame > xFrame( m_xOwner.get(), UNO_QUERY );
    if ( xFrame.is() == sal_True )
    {
        Reference< css::awt::XWindow > xContainerWindow( xFrame->getContainerWindow() );
        Reference< css::awt::XWindow > xComponentWindow( xFrame->getComponentWindow() );

        if (( xContainerWindow.is() == sal_True ) &&
            ( xComponentWindow.is() == sal_True )       )
        {
            css::uno::Reference< css::awt::XDevice > xDevice( xContainerWindow, css::uno::UNO_QUERY );
            // Convert relativ size to output size.
            css::awt::Rectangle  aRectangle  = xContainerWindow->getPosSize();
            css::awt::DeviceInfo aInfo       = xDevice->getInfo();
            css::awt::Size       aSize       (  aRectangle.Width  - aInfo.LeftInset - aInfo.RightInset  ,
                                                aRectangle.Height - aInfo.TopInset  - aInfo.BottomInset );

            // client size of container window
//            css::uno::Reference< css::awt::XLayoutConstrains > xLayoutContrains( xComponentWindow, css::uno::UNO_QUERY );
            css::awt::Size aMinSize( 0, 0 ); // = xLayoutContrains->getMinimumSize();

            // Check if request border space would decrease component window size below minimum size
            if ((( aSize.Width - RequestedSpace.X - RequestedSpace.Width ) < aMinSize.Width ) ||
                (( aSize.Height - RequestedSpace.Y - RequestedSpace.Height ) < aMinSize.Height  )       )
                return sal_False;

            return sal_True;
        }
    }

    return sal_False;
}

void SAL_CALL DockingAreaDefaultAcceptor::setDockingAreaSpace( const css::awt::Rectangle& BorderSpace ) throw (css::uno::RuntimeException)
{
    // Ready for multithreading
    ResetableGuard aGuard( m_aLock );

    // Try to "lock" the frame for access to taskscontainer.
    Reference< XFrame > xFrame( m_xOwner.get(), UNO_QUERY );
    if ( xFrame.is() == sal_True )
    {
        Reference< css::awt::XWindow > xContainerWindow( xFrame->getContainerWindow() );
        Reference< css::awt::XWindow > xComponentWindow( xFrame->getComponentWindow() );

        if (( xContainerWindow.is() == sal_True ) &&
            ( xComponentWindow.is() == sal_True )       )
        {
            css::uno::Reference< css::awt::XDevice > xDevice( xContainerWindow, css::uno::UNO_QUERY );
            // Convert relativ size to output size.
            css::awt::Rectangle  aRectangle  = xContainerWindow->getPosSize();
            css::awt::DeviceInfo aInfo       = xDevice->getInfo();
            css::awt::Size       aSize       (  aRectangle.Width  - aInfo.LeftInset - aInfo.RightInset  ,
                                                aRectangle.Height - aInfo.TopInset  - aInfo.BottomInset );
            // client size of container window
//            css::uno::Reference< css::awt::XLayoutConstrains > xLayoutContrains( xComponentWindow, css::uno::UNO_QUERY );
            css::awt::Size aMinSize( 0, 0 );// = xLayoutContrains->getMinimumSize();

            // Check if request border space would decrease component window size below minimum size
            sal_Int32 nWidth     = aSize.Width - BorderSpace.X - BorderSpace.Width;
            sal_Int32 nHeight    = aSize.Height - BorderSpace.Y - BorderSpace.Height;

            if (( nWidth > aMinSize.Width ) && ( nHeight > aMinSize.Height ))
            {
                // Resize our component window.
                xComponentWindow->setPosSize( BorderSpace.X, BorderSpace.Y, nWidth, nHeight, css::awt::PosSize::POSSIZE );
            }
        }
    }
}

} // namespace framework
