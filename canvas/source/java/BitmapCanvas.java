/*************************************************************************
 *
 *  $RCSfile: BitmapCanvas.java,v $
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

// UNO
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.IQueryInterface;
import com.sun.star.lang.XInitialization;
import com.sun.star.lib.uno.helper.WeakBase;

// OOo AWT
import com.sun.star.awt.*;

// Canvas
import com.sun.star.rendering.*;
import com.sun.star.geometry.*;

// Java AWT
import java.awt.*;
import java.awt.image.*;
import java.awt.geom.*;

// system-dependent stuff
import sun.awt.*;


public class BitmapCanvas
    extends CanvasBase
    implements com.sun.star.rendering.XBitmapCanvas,
               com.sun.star.lang.XServiceInfo
{
    private Graphics2D  graphics;

    public Graphics2D getGraphics()
    {
        return graphics;
    }

    //----------------------------------------------------------------------------------

    public BitmapCanvas( Graphics2D _graphics )
    {
        graphics = _graphics;
    }

    //----------------------------------------------------------------------------------

    //
    // XBitmapCanvas impl
    // ==================
    //

    public synchronized void copyRect( com.sun.star.rendering.XBitmapCanvas sourceCanvas,
                                       com.sun.star.geometry.RealRectangle2D sourceRect,
                                       com.sun.star.rendering.ViewState         sourceViewState,
                                       com.sun.star.rendering.RenderState   sourceRenderState,
                                       com.sun.star.geometry.RealRectangle2D    destRect,
                                       com.sun.star.rendering.ViewState         destViewState,
                                       com.sun.star.rendering.RenderState   destRenderState )
    {
        // TODO: create temp image when transform is non-trivial

        if( sourceCanvas == this )
        {
            // copy rectangle within the canvas
            graphics.copyArea((int)sourceRect.X1,
                              (int)sourceRect.Y1,
                              (int)(sourceRect.X2 - sourceRect.X1),
                              (int)(sourceRect.Y2 - sourceRect.Y1),
                              (int)(destRect.X1 - sourceRect.X1),
                              (int)(destRect.Y1 - sourceRect.Y1) );
        }
        else
        {
            if( sourceCanvas instanceof JavaCanvas )
            {
                // cache
                CanvasUtils.setupGraphicsState( graphics, destViewState, destRenderState, CanvasUtils.alsoSetupPaint );

                // TODO: really extract correct source rect here
                BufferedImage backBuffer = ((BufferedGraphics2D)((JavaCanvas)sourceCanvas).getGraphics()).getBackBuffer();
                graphics.drawImage( backBuffer, 0, 0, null );
                CanvasUtils.postRenderImageTreatment( backBuffer );

            }
            // TODO: foreign canvas
        }
    }

    //----------------------------------------------------------------------------------

    private static final String s_implName = "XBitmapCanvas.java.impl";
    private static final String s_serviceName = "com.sun.star.rendering.BitmapCanvas";

    //----------------------------------------------------------------------------------

    //
    // XServiceInfo impl
    // =================
    //
    public String getImplementationName()
    {
        return s_implName;
    }

    public String [] getSupportedServiceNames()
    {
        return new String [] { s_serviceName };
    }

    public boolean supportsService( String serviceName )
    {
        return serviceName.equals( s_serviceName );
    }
}
