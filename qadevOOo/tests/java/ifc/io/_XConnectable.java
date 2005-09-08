/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _XConnectable.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:11:04 $
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

package ifc.io;

import lib.MultiMethodTest;

import com.sun.star.io.XConnectable;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

/**
* Testing <code>com.sun.star.io.XConnectable</code>
* interface methods:
* <ul>
*   <li><code>setPredecessor()</code></li>
*   <li><code>getPredecessor()</code></li>
*   <li><code>setSuccessor()</code></li>
*   <li><code>getSuccessor()</code></li>
* </ul> <p>
* This test needs the following object relations :
* <ul>
*  <li> <code>'Connectable'</code> (supports the <code>XConnectable</code>
*   interface):
*   another 0bject to connect </li>
* </ul>
* After test completion object environment has to be recreated.
* @see com.sun.star.io.XConnectable
*/
public class _XConnectable extends MultiMethodTest {

    public XConnectable oObj = null;

    private XConnectable xConnect = null ;

    /**
     * Get another connectable object from object relations.
    */
    public void before() {
        XInterface x = (XInterface)tEnv.getObjRelation("Connectable");
        xConnect = (XConnectable)UnoRuntime.queryInterface(
                                                    XConnectable.class, x) ;
    }

    /**
    * Test calls the method using interface <code>XConnectable</code>
    * received in method <code>before()</code> as parameter. <p>
    * Has <b> OK </b> status if the method successfully returns. <p>
    */
    public void _setPredecessor() {
        oObj.setPredecessor(xConnect) ;

        tRes.tested("setPredecessor()", true) ;
    }

    /**
    * Test calls the method and compares returned value with value that was
    * set in the method <code>setPredecessor()</code>. <p>
    * Has <b> OK </b> status if values are equal. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> setPredecessor() </code></li>
    * </ul>
    */
    public void _getPredecessor() {
        requiredMethod("setPredecessor()") ;

        XConnectable gConnect = oObj.getPredecessor() ;

        tRes.tested("getPredecessor()", xConnect.equals(gConnect)) ;
    }

    /**
    * Test calls the method using interface <code>XConnectable</code>
    * received in method <code>before()</code> as parameter. <p>
    * Has <b> OK </b> status if the method successfully returns. <p>
    */
    public void _setSuccessor() {
        oObj.setSuccessor(xConnect) ;

        tRes.tested("setSuccessor()", true) ;
    }

    /**
    * Test calls the method and compares returned value with value that was
    * set in the method <code>setSuccessor()</code>. <p>
    * Has <b> OK </b> status if values are equal. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> setSuccessor() </code></li>
    * </ul>
    */
    public void _getSuccessor() {
        requiredMethod("setSuccessor()") ;

        XConnectable gConnect = oObj.getSuccessor() ;

        tRes.tested("getSuccessor()", xConnect.equals(gConnect)) ;
    }

    /**
    * Forces object environment recreation.
    */
    public void after() {
        tEnv.dispose() ;
    }
}

