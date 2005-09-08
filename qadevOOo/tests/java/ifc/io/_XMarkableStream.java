/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _XMarkableStream.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:12:09 $
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

import com.sun.star.io.XMarkableStream;

/**
* Testing <code>com.sun.star.io.XMarkableStream</code>
* interface methods:
* <ul>
*   <li><code>createMark()</code></li>
*   <li><code>deleteMark()</code></li>
*   <li><code>jumpToFurthest()</code></li>
*   <li><code>jumpToMark()</code></li>
*   <li><code>offsetToMark()</code></li>
* </ul> <p>
* @see com.sun.star.io.XMarkableStream
*/
public class _XMarkableStream extends MultiMethodTest {

    public XMarkableStream oObj = null;
    private int mark = -1 ;

    /**
    * Test creates mark and stores it. <p>
    * Has <b> OK </b> status if no exceptions were thrown
    * and returned isn't less than zero. <p>
    */
    public void _createMark() {
        boolean res;
        try {
            mark = oObj.createMark() ;
            res = mark >= 0;
        } catch (com.sun.star.io.IOException e) {
            log.println("Couldn't create mark");
            e.printStackTrace(log);
            res = false;
        }

        tRes.tested("createMark()", res);
    }

    /**
    * Test deletes the mark that was created by method <code>createMark()
    * </code>.<p>
    * Has <b> OK </b> status if the method successfully returns
    * and no exceptions were thrown. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> createMark() </code> : to have mark </li>
    * </ul>
    * The following method tests are to be executed before :
    * <ul>
    *  <li> <code> jumpToFurthest() </code></li>
    *  <li> <code> jumpToMark() </code></li>
    *  <li> <code> offsetToMark() </code></li>
    * </ul>
    */
    public void _deleteMark() {
        requiredMethod("createMark()") ;

        executeMethod("jumpToFurthest()") ;
        executeMethod("jumpToMark()") ;
        executeMethod("offsetToMark()") ;

        boolean res;
        try {
            oObj.deleteMark(mark);
            res = true;
        } catch(com.sun.star.io.IOException e) {
            log.println("Couldn't delete mark");
            e.printStackTrace(log);
            res = false;
        } catch(com.sun.star.lang.IllegalArgumentException e) {
            log.println("Couldn't delete mark");
            e.printStackTrace(log);
            res = false;
        }

        tRes.tested("deleteMark()", res) ;
    }

    /**
    * Test calls the method. <p>
    * Has <b> OK </b> status if the method successfully returns
    * and no exceptions were thrown. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> createMark() </code></li>
    * </ul>
    */
    public void _jumpToFurthest() {
        requiredMethod("createMark()") ;

        boolean res;
        try {
            oObj.jumpToFurthest() ;
            res = true;
        } catch (com.sun.star.io.IOException e) {
            log.println("Couldn't jump to furthest");
            e.printStackTrace(log);
            res = false;
        }

        tRes.tested("jumpToFurthest()", res) ;
    }

    /**
    * Test jumps to mark that was created by method <code>createMark()</code>.
    * <p>Has <b> OK </b> status if the method successfully returns
    * and no exceptions were thrown. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> jumpToFurthest() </code> : for the right order of tests
    *  excecution </li>
    * </ul>
    */
    public void _jumpToMark() {
        requiredMethod("jumpToFurthest()") ;
        boolean res;

        try {
            oObj.jumpToMark(mark) ;
            res = true;
        } catch(com.sun.star.lang.IllegalArgumentException e) {
            log.println("Couldn't jump to mark");
            e.printStackTrace(log);
            res = false;
        } catch(com.sun.star.io.IOException e) {
            log.println("Couldn't jump to mark");
            e.printStackTrace(log);
            res = false;
        }

        tRes.tested("jumpToMark()", res) ;
    }

    /**
    * Test obtains offset to mark that was created by
    * method <code>createMark()</code> and checks returned value.<p>
    * Has <b> OK </b> status if returned value is equal to zero
    * and no exceptions were thrown. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> jumpToMark() </code> : to have current position at
    *  the mark position </li>
    * </ul>
    */
    public void _offsetToMark() {

        requiredMethod("jumpToMark()") ;

        boolean res;
        try {
            int offset = oObj.offsetToMark(mark);
            res = offset == 0;
        } catch(com.sun.star.lang.IllegalArgumentException e) {
            log.println("Couldn't get offser to mark");
            e.printStackTrace(log);
            res = false;
        } catch(com.sun.star.io.IOException e) {
            log.println("Couldn't get offser to mark");
            e.printStackTrace(log);
            res = false;
        }

        tRes.tested("offsetToMark()", res);
    }
}

