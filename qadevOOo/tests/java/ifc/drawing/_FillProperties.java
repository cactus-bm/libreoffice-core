/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _FillProperties.java,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: kz $ $Date: 2007-09-06 13:51:52 $
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

package ifc.drawing;

import com.sun.star.beans.XPropertySetInfo;
import lib.MultiPropertyTest;

import com.sun.star.awt.XBitmap;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;

/**
* Testing <code>com.sun.star.drawing.FillProperties</code>
* service properties :
* <ul>
*  <li><code> FillStyle</code></li>
*  <li><code> FillColor</code></li>
*  <li><code> FillTransparence</code></li>
*  <li><code> FillTransparenceGradientName</code></li>
*  <li><code> FillTransparenceGradient</code></li>
*  <li><code> FillGradientName</code></li>
*  <li><code> FillGradient</code></li>
*  <li><code> FillHatchName</code></li>
*  <li><code> FillHatch</code></li>
*  <li><code> FillBitmapName</code></li>
*  <li><code> FillBitmap</code></li>
*  <li><code> FillBitmapURL</code></li>
*  <li><code> FillBitmapOffsetX</code></li>
*  <li><code> FillBitmapOffsetY</code></li>
*  <li><code> FillBitmapPositionOffsetX</code></li>
*  <li><code> FillBitmapPositionOffsetY</code></li>
*  <li><code> FillBitmapRectanglePoint</code></li>
*  <li><code> FillBitmapLogicalSize</code></li>
*  <li><code> FillBitmapSizeX</code></li>
*  <li><code> FillBitmapSizeY</code></li>
*  <li><code> FillBitmapMode</code></li>
*  <li><code> FillBackground</code></li>
* </ul> <p>
* Properties testing is automated by <code>lib.MultiPropertyTest</code>.
* @see com.sun.star.drawing.FillProperties
*/
public class _FillProperties extends MultiPropertyTest {

    XBitmap the_bitmap = null;
    XBitmap the_secondBitmap = null;

    /**
     * Property tester which changes URL.
     */
    protected PropertyTester URLTester = new PropertyTester() {
        protected Object getNewValue(String propName, Object oldValue)
                throws java.lang.IllegalArgumentException {
            if (oldValue.equals(util.utils.getFullTestURL("space-metal.jpg")))
                return util.utils.getFullTestURL("crazy-blue.jpg"); else
                return util.utils.getFullTestURL("space-metal.jpg");
        }
    } ;

    /**
     * Property tester which switches two strings.
     */
    protected PropertyTester StringTester = new PropertyTester() {
        protected Object getNewValue(String propName, Object oldValue)
                throws java.lang.IllegalArgumentException {
            if (oldValue.equals(str1))
                return str2; else
                return str1;
        }
    } ;

    /**
     * Property tester which switches two XBitmap objects.
     */
    protected PropertyTester BitmapTester = new PropertyTester() {
        protected Object getNewValue(String propName, Object oldValue)
                throws java.lang.IllegalArgumentException {
            if (util.ValueComparer.equalValue(oldValue,the_bitmap))
                return the_secondBitmap; else
                return the_bitmap;
        }
    } ;

    /**
     * This property must have an URL format
     */
    public void _FillBitmapURL() {
        log.println("Testing with custom Property tester") ;
        testProperty("FillBitmapURL", URLTester) ;
    }

    public String str1 = "";
    public String str2 = "";

    /**
     * This property must have predefined values
     */
    public void _FillGradientName() {
        log.println("Testing with custom Property tester") ;
        str1 = "Gradient 1";
        str2 = "Gradient 3";
        testProperty("FillGradientName", StringTester) ;
    }

    /**
     * This property must have predefined values
     */
    public void _FillBitmapName() {
        log.println("Testing with custom Property tester") ;
        str1 = "Sky";
        str2 = "Blank";
        testProperty("FillBitmapName", StringTester) ;
    }

    /**
     * This property must have predefined values
     */
    public void _FillTransparenceGradientName() {
        log.println("Testing with custom Property tester") ;
        str1 = "Transparency 2";
        str2 = "Transparency 1";
        testProperty("FillTransparenceGradientName", StringTester) ;
    }

    /**
     * This property must have predefined values
     */
    public void _FillHatchName() {
        log.println("Testing with custom Property tester") ;
        str1 = "Black 0 degrees";
        str2 = "Black 45 degrees";
        testProperty("FillHatchName", StringTester) ;
    }

    /**
     * This property is very interesting. After setting URL of bitmap you get
     * <code>XBitmap</code> interface of the image file specified.
     * The method first gets two different bitmaps and then tests the property.
     */
    public void _FillBitmap() {

        String propName = "FillBitmap";

        XPropertySetInfo info = oObj.getPropertySetInfo();

        if (!info.hasPropertyByName(propName)) {
            if (isOptional(propName)) {
                // skipping optional property test
                log.println("Property '" + propName
                        + "' is optional and not supported");
                tRes.tested(propName,true);
                return;
            } else {

                try {

                    oObj.setPropertyValue("FillBitmapURL", util.utils.getFullTestURL("crazy-blue.jpg"));

                    the_bitmap = (XBitmap) AnyConverter.toObject(
                                        new Type(XBitmap.class), oObj.getPropertyValue("FillBitmap"));

                    oObj.setPropertyValue("FillBitmapURL", util.utils.getFullTestURL("space-metal.jpg"));

                    the_secondBitmap = (XBitmap) AnyConverter.toObject(
                                        new Type(XBitmap.class),oObj.getPropertyValue("FillBitmap"));

                    testProperty("FillBitmap", BitmapTester) ;

                } catch (com.sun.star.beans.PropertyVetoException e) {
                    log.println("Couldn't change Bitmap");
                    e.printStackTrace(log);
                    tRes.tested("FillBitmap",false);

                } catch (com.sun.star.beans.UnknownPropertyException e) {
                    log.println("Couldn't change Bitmap");
                    e.printStackTrace(log);
                    tRes.tested("FillBitmap",false);

                } catch (com.sun.star.lang.WrappedTargetException e) {
                    log.println("Couldn't change Bitmap");
                    e.printStackTrace(log);
                    tRes.tested("FillBitmap",false);

                } catch (com.sun.star.lang.IllegalArgumentException e) {
                    log.println("Couldn't change Bitmap");
                    e.printStackTrace(log);
                    tRes.tested("FillBitmap",false);
                }
            }

        }
    }
}

