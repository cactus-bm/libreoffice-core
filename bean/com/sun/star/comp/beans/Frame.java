/*************************************************************************
 *
 *  $RCSfile: Frame.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: mi $ $Date: 2004-10-14 10:37:12 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., September, 2004
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2004 by Sun Microsystems, Inc.
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
 *  Copyright: 2004 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

package com.sun.star.comp.beans;

import com.sun.star.uno.UnoRuntime;

/** Wrapper class for a com.sun.star.frame.XFrame.
 *
 * @since OOo 2.0.0
 */
public class Frame
    extends Wrapper
    implements
        com.sun.star.frame.XFrame,
        com.sun.star.frame.XDispatchProvider,
        com.sun.star.frame.XDispatchProviderInterception
{
    private com.sun.star.frame.XFrame xFrame;
    private com.sun.star.frame.XDispatchProvider xDispatchProvider;
    private com.sun.star.frame.XDispatchProviderInterception xDispatchProviderInterception;

    public Frame( com.sun.star.frame.XFrame xFrame )
    {
        super( xFrame );
        this.xFrame = xFrame;
        xDispatchProvider = (com.sun.star.frame.XDispatchProvider)
            UnoRuntime.queryInterface( com.sun.star.frame.XDispatchProvider.class,
                xFrame );
        xDispatchProviderInterception = (com.sun.star.frame.XDispatchProviderInterception)
            UnoRuntime.queryInterface( com.sun.star.frame.XDispatchProviderInterception.class,
                xFrame );
    }

    //==============================================================
    // com.sun.star.frame.XFrame
    //--------------------------------------------------------------

    public void initialize( /*IN*/com.sun.star.awt.XWindow xWindow )
    {
        xFrame.initialize( xWindow );
    }

    public com.sun.star.awt.XWindow getContainerWindow(  )
    {
        return xFrame.getContainerWindow();
    }

    public void setCreator( /*IN*/ com.sun.star.frame.XFramesSupplier xCreator )
    {
        xFrame.setCreator( xCreator );
    }

    public com.sun.star.frame.XFramesSupplier getCreator(  )
    {
        return xFrame.getCreator();
    }

    public String getName(  )
    {
        return xFrame.getName();
    }

    public void setName( /*IN*/ String aName )
    {
        xFrame.setName( aName );
    }

    public com.sun.star.frame.XFrame findFrame( /*IN*/String aTargetFrameName, /*IN*/int nSearchFlags )
    {
        return xFrame.findFrame( aTargetFrameName, nSearchFlags );
    }

    public boolean isTop(  )
    {
        return xFrame.isTop();
    }

    public void activate(  )
    {
        xFrame.activate();
    }

    public void deactivate(  )
    {
        xFrame.deactivate();
    }

    public boolean isActive(  )
    {
        return xFrame.isActive();
    }

    public boolean setComponent( /*IN*/com.sun.star.awt.XWindow xComponentWindow, /*IN*/ com.sun.star.frame.XController xController )
    {
        return xFrame.setComponent( xComponentWindow, xController );
    }

    public com.sun.star.awt.XWindow getComponentWindow(  )
    {
        return xFrame.getComponentWindow();
    }

    public com.sun.star.frame.XController getController(  )
    {
        return xFrame.getController();
    }

    public void contextChanged(  )
    {
        xFrame.contextChanged();
    }

    public void addFrameActionListener( /*IN*/ com.sun.star.frame.XFrameActionListener xListener )
    {
        xFrame.addFrameActionListener( xListener );
    }

    public void removeFrameActionListener( /*IN*/ com.sun.star.frame.XFrameActionListener xListener )
    {
        xFrame.removeFrameActionListener( xListener );
    }

    //==============================================================
    // com.sun.star.frame.XDispatchProvider
    //--------------------------------------------------------------

    public com.sun.star.frame.XDispatch queryDispatch(
            /*IN*/ com.sun.star.util.URL aURL,
            /*IN*/ String aTargetFrameName,
            /*IN*/ int nSearchFlags )
    {
        return xDispatchProvider.queryDispatch( aURL, aTargetFrameName, nSearchFlags );
    }

    public com.sun.star.frame.XDispatch[] queryDispatches(
            /*IN*/ com.sun.star.frame.DispatchDescriptor[] aRequests )
    {
        return xDispatchProvider.queryDispatches( aRequests );
    }

    //==============================================================
    // com.sun.star.frame.XDispatchProviderInterception
    //--------------------------------------------------------------

    public void registerDispatchProviderInterceptor(
            /*IN*/ com.sun.star.frame.XDispatchProviderInterceptor xInterceptor )
    {
        xDispatchProviderInterception.registerDispatchProviderInterceptor( xInterceptor );
    }

    public void releaseDispatchProviderInterceptor(
            /*IN*/ com.sun.star.frame.XDispatchProviderInterceptor xInterceptor )
    {
        xDispatchProviderInterception.releaseDispatchProviderInterceptor( xInterceptor );
    }
};

