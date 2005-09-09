/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _XTextRangeMover.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:27:30 $
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

package ifc.text;

import lib.MultiMethodTest;
import lib.Status;
import lib.StatusException;

import com.sun.star.text.XTextRange;
import com.sun.star.text.XTextRangeMover;

/**
 * Testing <code>com.sun.star.text.XTextRangeMover</code>
 * interface methods :
 * <ul>
 *  <li><code> moveTextRange()</code></li>
 * </ul> <p>
 * This test needs the following object relations :
 * <ul>
 *  <li> <code>'RangeForMove'</code> (of type <code>XTextRange</code>):
 *   the range to be moved. </li>
 *   <li> <code>'XTextRange'</code> (of type <code>XTextRange</code>):
 *   the range that includes moving range. </li>
 * <ul> <p>
 * Test is <b> NOT </b> multithread compilant. <p>
 * @see com.sun.star.text.XTextRangeMover
 */
public class _XTextRangeMover extends MultiMethodTest {

    public XTextRangeMover oObj = null;

    XTextRange xTextRange = null;
    XTextRange oMoveRange = null;

    /**
     * Moves the range obtained from relation 'RangeForMove' by 1 paragraph
     * and compares index of moved string in the whole text obtained
     * from relation 'XTextRange'. <p>
     * Has <b>OK</b> status if index of moved range is changed after method call.
     */
    public void _moveTextRange(){
        oMoveRange = (XTextRange) tEnv.getObjRelation("RangeForMove");
        xTextRange = (XTextRange) tEnv.getObjRelation("XTextRange");

        if (oMoveRange == null) {
            throw new StatusException(
                Status.failed("Couldn't get relation 'RangeForMove'"));
        }

        if (xTextRange == null) {
            throw new StatusException(
                Status.failed("Couldn't get relation 'XTextRange'"));
        }

        log.println("Content before moving:");
        log.println(xTextRange.getString());
        log.println("Text range for moving:");
        log.println(oMoveRange.getString());
        int indexBefore = xTextRange.getString().indexOf(oMoveRange.getString());
        oObj.moveTextRange(oMoveRange,(short) 1);
        log.println("Content after moving:");
        log.println(xTextRange.getString());
        int indexAfter = xTextRange.getString().indexOf(oMoveRange.getString());

        boolean res = indexBefore != indexAfter;
        log.println("Index before moving:" + indexBefore);
        log.println("Index after moving:" + indexAfter);

        tRes.tested("moveTextRange()", res);
    }
}

