/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _XShapes.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 23:43:25 $
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

import lib.MultiMethodTest;
import util.XInstCreator;

import com.sun.star.drawing.XShape;
import com.sun.star.drawing.XShapes;
import com.sun.star.uno.XInterface;


/**
* Testing <code>com.sun.star.drawing.XShapes</code>
* interface methods :
* <ul>
*  <li><code> add()</code></li>
*  <li><code> remove()</code></li>
* </ul> <p>
* This test needs the following object relations :
* <ul>
*  <li> <code>'Shape'</code> (of type <code>XShape</code>):
*   a shape which can be inserted into shape collection. </li>
* <ul> <p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.drawing.XShapes
*/
public class _XShapes extends MultiMethodTest {

    public XShapes oObj = null;                // oObj filled by MultiMethodTest
    XInstCreator shape = null;
    XInterface oShape = null;


    /**
    * Retrieves a shape from relation and adds it to the collection.
    * Number of shapes is checked before and after adding.<p>
    * Has <b> OK </b> status if after adding number of shapes increases by
    * 1. <p>
    */
    public void _add () {

        boolean result = false;
        shape = (XInstCreator)tEnv.getObjRelation("Shape");
        oShape = shape.createInstance();
        XShape oSh = (XShape) oShape;

        log.println("testing add() ... ");

        int cntBefore = oObj.getCount();
        oObj.add(oSh);
        int cntAfter = oObj.getCount();
        result = cntBefore + 1 == cntAfter ;

        tRes.tested("add()", result);
    }

    /**
    * Removes the shape added before from the collection.
    * Number of shapes is checked before and after removing.<p>
    * Has <b> OK </b> status if after removing number of shapes decreases by
    * 1. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> add() </code> : a shape added. </li>
    * </ul>
    */
    public void _remove () {
        requiredMethod("add()");
        boolean result = false;

        log.println("removing the shape...");

        int cntBefore = oObj.getCount();
        oObj.remove((XShape) oShape);
        int cntAfter = oObj.getCount();
        result = cntBefore == cntAfter + 1;

        tRes.tested("remove()", result);
    }

}


