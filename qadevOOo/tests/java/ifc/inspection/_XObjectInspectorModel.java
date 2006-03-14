/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _XObjectInspectorModel.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2006-03-14 11:49:42 $
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

package ifc.inspection;

import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.inspection.PropertyCategoryDescriptor;
import com.sun.star.inspection.XObjectInspectorModel;
import lib.MultiMethodTest;



/**
 * Testing <code>com.sun.star.inspection.XObjectInspectorModel</code>
 * interface methods :
 * <ul>
 *  <li><code> describeCategories()</code></li>
 *  <li><code> getHandlerFactories()</code></li>
 *  <li><code> getPropertyOrderIndex()</code></li>
 * </ul> <p>
 * Test is <b> NOT </b> multithread compilant. <p>
 *
 */
public class _XObjectInspectorModel extends MultiMethodTest {

    /**
     * the test object
     */
    public XObjectInspectorModel oObj = null;

    /**
     * calls the method <CODE>getHandlerFactories()</CODE>
     * Has <b>OK</b> status if returned value is not null.
     */
    public void _getHandlerFactories() {

        Object[] HandlerFactories = oObj.getHandlerFactories();

        tRes.tested("getHandlerFactories()", HandlerFactories != null) ;
    }

    /**
     * Call the method <CODE>getPropertyOrderIndex()</CODE> with an invalid propety name.
     * Has <CODE>OK</CODE> status if the returned index is "0".
     */
    public void _getPropertyOrderIndex() {

        int index = 0;
        boolean result = true;

        log.println("try to get index of INvalid property name 'InvalidPropertyName' ...");
        index = oObj.getPropertyOrderIndex("InvalidPropertyName");
        log.println("index is: " + index);
        result = (index == 0);

        tRes.tested("getPropertyOrderIndex()", result) ;
    }

    /**
     * Call the method <CODE>describeCategories()</CODE>
     * Has <b>OK</b> status if returned value is not null.
     */
    public void _describeCategories() {

        PropertyCategoryDescriptor[] categories = oObj.describeCategories();

        tRes.tested("describeCategories()", categories != null) ;
    }

}


