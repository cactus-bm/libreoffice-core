/*************************************************************************
 *
 *  $RCSfile: CanvasCustomSprite.java,v $
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
import drafts.com.sun.star.rendering.*;
import drafts.com.sun.star.geometry.*;

// Java AWT
import java.awt.*;
import java.awt.geom.*;

// system-dependent stuff
import sun.awt.*;


public class CanvasCustomSprite
    extends com.sun.star.lib.uno.helper.ComponentBase
    implements drafts.com.sun.star.rendering.XCustomSprite,
                com.sun.star.lang.XServiceInfo,
                SpriteBase
{
    private JavaCanvas                      canvas;
    private RealSize2D                      spriteSize;
    private Graphics2D                      referenceGraphics;
    private double                          alpha;
    private java.awt.geom.Point2D.Double    outputPosition;
    private SpriteRep                       spriteRep;

    //----------------------------------------------------------------------------------

    public CanvasCustomSprite( RealSize2D   _spriteSize,
                               JavaCanvas   _canvas,
                               Graphics2D   _referenceGraphics )
    {
        CanvasUtils.printLog( "CanvasCustomSprite constructor called, size is (" + _spriteSize.Width + ", " + _spriteSize.Height + ")" );

        canvas              = _canvas;
        spriteSize          = _spriteSize;
        referenceGraphics   = _referenceGraphics;
        alpha               = 0.0;
        outputPosition      = new java.awt.geom.Point2D.Double(0.0,0.0);
    }

    //----------------------------------------------------------------------------------

    //
    // SpriteBase
    // ==========
    //
    public synchronized SpriteRep getSpriteRep()
    {
        if( spriteRep == null )
        {
            spriteRep = new SpriteRep();

            spriteRep.setupBuffer(referenceGraphics, (int)(spriteSize.Width+.5), (int)(spriteSize.Height+.5) );

            spriteRep.moveSprite( outputPosition );
            spriteRep.setSpriteAlpha( alpha );
        }
        return spriteRep;
    }

    //----------------------------------------------------------------------------------

    //
    // XComponent
    // ==========
    //
    public void dispose()
    {
        if( spriteRep != null )
            spriteRep.dispose();

        canvas = null;
        referenceGraphics = null;
        spriteRep = null;

        super.dispose();
    }

    //----------------------------------------------------------------------------------

    //
    // XCustomSprite impl
    // ==================
    //

    public synchronized void setAlpha( double _alpha )
    {
        CanvasUtils.printLog( "CanvasCustomSprite.setAlpha() called" );

        alpha = _alpha;

        if( spriteRep != null )
        {
            spriteRep.setSpriteAlpha( alpha );
        }
    }

    public synchronized void move( RealPoint2D  _aNewPos,
                                   ViewState    _viewState,
                                   RenderState  _renderState )
    {
        CanvasUtils.printLog( "CanvasCustomSprite.move() called" );

        // transform given point with concatenated transformation
        AffineTransform transform = CanvasUtils.ViewConcatRenderTransform( _viewState, _renderState );
        transform.transform( new java.awt.geom.Point2D.Double(_aNewPos.X,
                                                              _aNewPos.Y),
                             outputPosition );

        if( spriteRep != null )
        {
            spriteRep.moveSprite( outputPosition );
        }
    }

    public synchronized void transform( AffineMatrix2D aTransformation ) throws com.sun.star.lang.IllegalArgumentException
    {
        // TODO
    }

    public synchronized void clip( XPolyPolygon2D   aClip,
                                   ViewState        viewState,
                                   RenderState      renderState ) throws com.sun.star.lang.IllegalArgumentException
    {
        // TODO
    }

    public synchronized void show()
    {
        CanvasUtils.printLog( "CanvasCustomSprite.show() called" );

        canvas.showSprite( this );
        canvas.updateScreen();
    }

    public synchronized void hide()
    {
        CanvasUtils.printLog( "CanvasCustomSprite.hide() called" );

        canvas.hideSprite( this );

        // do _not_ dispose clear SpriteRep, since we cannot actively
        // repaint ourselves
    }

    public synchronized drafts.com.sun.star.rendering.XCanvas getContentCanvas()
    {
        CanvasUtils.printLog( "CanvasCustomSprite.getContentCanvas() called" );

        return getSpriteRep().getContentCanvas();
    }

    //----------------------------------------------------------------------------------

    private static final String s_implName = "XCustomSprite.java.impl";
    private static final String s_serviceName = "drafts.com.sun.star.rendering.CustomSprite";

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
