/*************************************************************************
 *
 *  $RCSfile: CanvasBitmap.java,v $
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
import com.sun.star.lib.uno.helper.WeakBase;

// OOo AWT
import com.sun.star.awt.*;

// Canvas
import drafts.com.sun.star.rendering.*;
import drafts.com.sun.star.geometry.*;

public class CanvasBitmap
    extends com.sun.star.lib.uno.helper.ComponentBase
    implements com.sun.star.lang.XServiceInfo,
               drafts.com.sun.star.rendering.XIntegerBitmap
{
    private java.awt.image.BufferedImage    bitmap;

    public CanvasBitmap( java.awt.image.BufferedImage _bitmap )
    {
        bitmap = _bitmap;
    }

    public CanvasBitmap( IntegerSize2D mySize )
    {
        bitmap = new java.awt.image.BufferedImage(mySize.Width, mySize.Height,
                                                  java.awt.image.BufferedImage.TYPE_4BYTE_ABGR);
    }

    public CanvasBitmap( RealSize2D newSize, boolean beFast, CanvasBitmap source )
    {
//         java.awt.geom.AffineTransform transform = new java.awt.geom.AffineTransform();
//         transform.scale( newSize.width/size.Width, newSize.height/size.Height );

//         // TODO: Maybe keep the image returned via
//         // bitmap.getScaledInstance, and do scaling lazy.
//         bitmap = new java.awt.image.BufferedImage((int)(newSize.width+.5),
//                                                   (int)(newSize.height+.5),
//                                                   java.awt.image.BufferedImage.TYPE_4BYTE_ABGR);

//         java.awt.image.AffineTransformOp transformer =
//             new java.awt.image.AffineTransformOp( transform,
//                                                   java.awt.image.AffineTransformOp.TYPE_BILINEAR);

//         transformer.filter(source.getBufferedImage(), bitmap);
    }

    public synchronized java.awt.image.BufferedImage getBufferedImage()
    {
        return bitmap;
    }

    //
    // XBitmap implementation
    // ======================
    //

    public synchronized IntegerSize2D getSize()
    {
        return new IntegerSize2D( bitmap.getWidth(),
                                  bitmap.getHeight() );
    }

    //----------------------------------------------------------------------------------

    public synchronized XBitmapCanvas queryBitmapCanvas()
    {
        java.awt.Graphics2D bitmapGraphics = bitmap.createGraphics();
        CanvasUtils.initGraphics( bitmapGraphics );

        return new BitmapCanvas( bitmapGraphics );
    }

    //----------------------------------------------------------------------------------

    public synchronized drafts.com.sun.star.rendering.XBitmap getScaledBitmap( RealSize2D newSize, boolean beFast ) throws com.sun.star.lang.IllegalArgumentException, VolatileContentDestroyedException
    {
        return new CanvasBitmap( newSize, beFast, this );
    }

    //----------------------------------------------------------------------------------

    //
    // XIntegerBitmap implementation
    // =============================
    //

    public synchronized byte[] getData( IntegerRectangle2D rect )
    {
        int [] pixelData = bitmap.getRGB( rect.X1, rect.Y1, rect.X2 - rect.X1, rect.Y1 - rect.Y2, null, 0, 0 );

        return CanvasUtils.int2byte( pixelData );
    }

    //----------------------------------------------------------------------------------

    public synchronized void setData( byte[] data, IntegerRectangle2D rect )
    {
        int [] pixelData = CanvasUtils.byte2int( data );
        bitmap.setRGB( rect.X1, rect.Y1, rect.X2 - rect.X1, rect.Y2 - rect.Y1, pixelData, 0, bitmap.getWidth() );
    }

    //----------------------------------------------------------------------------------

    public synchronized void setPixel( byte[] color, IntegerPoint2D pos )
    {
        if( color.length != 4 )
            CanvasUtils.printLog( "CanvasBitmap.setPixel: Wrong color format" );

        int pixel = color[0] + (color[1] + (color[2] + color[3]*256)*256)*256;
        bitmap.setRGB( pos.X, pos.Y, pixel );
    }

    //----------------------------------------------------------------------------------

    public synchronized byte[] getPixel( IntegerPoint2D pos )
    {
        int pixel = bitmap.getRGB( pos.X, pos.Y );

        byte[] res = new byte[4];
        res[0] = (byte)(pixel & 255);
        res[1] = (byte)((pixel/256) & 255);
        res[2] = (byte)((pixel/256/256) & 255);
        res[3] = (byte)((pixel/256/256/256) & 255);

        return res;
    }

    //----------------------------------------------------------------------------------

    public synchronized XBitmapPalette getPalette()
    {
        return null;
    }

    //----------------------------------------------------------------------------------

    public synchronized IntegerBitmapLayout getMemoryLayout()
    {
        // TODO: finish that one
        IntegerBitmapLayout layout = new IntegerBitmapLayout();

        layout.ScanLines = bitmap.getWidth();
        layout.ScanLineBytes = bitmap.getWidth()*4;
        layout.ScanLineStride = layout.ScanLineBytes;
        layout.Format = 0;
        layout.NumComponents = 4;
        layout.ComponentMasks = null;
        layout.Endianness = 0;
        layout.IsPseudoColor = false;

        return layout;
    }

    //----------------------------------------------------------------------------------

    //
    // XServiceInfo impl
    // =================
    //

    private static final String s_implName = "XIntegerBitmap.java.impl";
    private static final String s_serviceName = "drafts.com.sun.star.rendering.IntegerBitmap";

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
