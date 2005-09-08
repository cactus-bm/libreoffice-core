/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _LineProperties.java,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 23:34:33 $
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

import com.sun.star.beans.PropertyValue;
import com.sun.star.drawing.LineDash;
import lib.MultiPropertyTest;

/**
* Testing <code>com.sun.star.drawing.LineProperties</code>
* service properties :
* <ul>
*  <li><code> LineStyle</code></li>
*  <li><code> LineDash</code></li>
*  <li><code> LineColor</code></li>
*  <li><code> LineTransparence</code></li>
*  <li><code> LineWidth</code></li>
*  <li><code> LineJoint</code></li>
*  <li><code> LineStartName</code></li>
*  <li><code> LineStart</code></li>
*  <li><code> LineEnd</code></li>
*  <li><code> LineStartCenter</code></li>
*  <li><code> LineStartWidth</code></li>
*  <li><code> LineEndCenter</code></li>
*  <li><code> LineEndWidth</code></li>
* </ul> <p>
* Properties testing is automated by <code>lib.MultiPropertyTest</code>.
* @see com.sun.star.drawing.LineProperties
*/
public class _LineProperties extends MultiPropertyTest {

    /**
     * Tester used for property LineStartName which can have
     * only predefined String values.
     */
    protected PropertyTester LineTester = new PropertyTester() {
        protected Object getNewValue(String propName, Object oldValue)
                throws java.lang.IllegalArgumentException {
            if (oldValue.equals("Arrow"))
                return "Square"; else
                return "Arrow";
        }
    }  ;

    /**
     * The property switched between 'Square' and 'Arrow' values.
     */
    public void _LineStartName() {
        log.println("Testing with custom Property tester");
        testProperty("LineStartName", LineTester) ;
    }

    public void _LineDash() {
        LineDash aLineDash = new LineDash();
        LineDash aLineDash2 = new LineDash();
        aLineDash.DashLen = 5;
        aLineDash2.DashLen = 1;
        PropertyValue[] firstValue = new PropertyValue[2];
        firstValue[0] = new PropertyValue();
        firstValue[0].Name = "Name";
        firstValue[0].Value = "Name1";
        firstValue[1] = new PropertyValue();
        firstValue[1].Name = "LineDash";
        firstValue[1].Value = aLineDash;
        PropertyValue[] secondValue = new PropertyValue[2];
        secondValue[0] = new PropertyValue();
        secondValue[0].Name = "Name";
        secondValue[0].Value = "Name2";
        secondValue[1] = new PropertyValue();
        secondValue[1].Name = "LineDash";
        secondValue[1].Value = aLineDash2;
        testProperty("LineDash",firstValue,secondValue);
    }
}

