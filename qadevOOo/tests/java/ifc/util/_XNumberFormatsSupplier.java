/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _XNumberFormatsSupplier.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:42:40 $
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

package ifc.util;

import lib.MultiMethodTest;

import com.sun.star.beans.XPropertySet;
import com.sun.star.util.XNumberFormats;
import com.sun.star.util.XNumberFormatsSupplier;

/**
 * Testing <code>com.sun.star.util.XNumberFormatsSupplier</code>
 * interface methods :
 * <ul>
 *  <li><code> getNumberFormatSettings()</code></li>
 *  <li><code> getNumberFormats()</code></li>
 * </ul> <p>
 * Test is <b> NOT </b> multithread compilant. <p>
 * @see com.sun.star.util.XNumberFormatsSupplier
 */
public class _XNumberFormatsSupplier extends MultiMethodTest {

    public XNumberFormatsSupplier oObj = null;

    /**
     * Get format settings and checks some properties for existence. <p>
     *
     * Has <b> OK </b> status if a number properties inherent to
     * <code>NumberFormatSettings</code> service exist in the
     * returned <code>XPropertySet</code>. <p>
     *
     * @see com.sun.star.util.NumberFormatSettings
     */
    public void _getNumberFormatSettings() {
        boolean result = true ;
        XPropertySet props = oObj.getNumberFormatSettings();

        if (props != null) {
            try {
              result &= props.getPropertyValue("NullDate") != null &&
                      props.getPropertyValue("StandardDecimals") != null &&
                      props.getPropertyValue("NoZero") != null &&
                      props.getPropertyValue("TwoDigitDateStart") != null ;
            } catch (com.sun.star.beans.UnknownPropertyException e) {
                log.println("Some property doesn't exist") ;
                e.printStackTrace(log) ;
                result = false ;
            } catch (com.sun.star.lang.WrappedTargetException e) {
                e.printStackTrace(log) ;
                result = false ;
            }
        } else {
            log.println("Method returns null") ;
            result = false ;
        }

        tRes.tested("getNumberFormatSettings()", result) ;
    }

    /**
     * Test calls the method. <p>
     * Has <b> OK </b> status if the method returns not
     * <code>null</code> value.
     */
    public void _getNumberFormats() {
        XNumberFormats formats = oObj.getNumberFormats();

        tRes.tested("getNumberFormats()", formats != null) ;
    }

}


