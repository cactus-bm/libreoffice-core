/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _XDatabaseRanges.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:47:28 $
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

package ifc.sheet;

import lib.MultiMethodTest;

import com.sun.star.sheet.XDatabaseRanges;
import com.sun.star.table.CellRangeAddress;

/**
* Testing <code>com.sun.star.sheet.XDatabaseRanges</code>
* interface methods :
* <ul>
*  <li><code> addNewByName()</code></li>
*  <li><code> removeByName()</code></li>
* </ul> <p>
* @see com.sun.star.sheet.XDatabaseRanges
*/
public class _XDatabaseRanges extends MultiMethodTest {

    public XDatabaseRanges oObj = null;
    CellRangeAddress CRA = null;
    String name = null;

    /**
    * Test adds a new database range to the collection, checks that range with
    * this name exist in collection and then tries to add range with the same
    * name. <p>
    * Has <b> OK </b> status if the added range exists in collection and
    * exception was thrown when trying to add range with name that is same as name
    * of existent range. <p>
    */
    public void _addNewByName() {
        boolean bResult = true;
        log.println("Trying to add range with proper name.");

        CRA = new CellRangeAddress((short)0, 1, 2, 3, 4);
        name = "_XDatabaseRanges_addNewByRange";

        oObj.addNewByName(name, CRA);

        bResult &= oObj.hasByName(name);

        if (bResult) log.println("Ok");
        log.println("Trying to add existing element.");

        try {
            oObj.addNewByName(name, CRA);
            log.println("Exception expected... Test failed.");
            bResult = false;
        } catch(com.sun.star.uno.RuntimeException e) {
            log.println("Exception occured while testing addNewByName() : " + e);
            bResult = true;
        }

        tRes.tested("addNewByName()", bResult);
    }

    /**
    * Test removes the database range with name that exist exactly and then
    * tries to remove the range with name that doesn't exist exactly. <p>
    * Has <b> OK </b> status if first range was succesfully removed and
    * exception was thrown when trying to remove non-existent database range.<p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> addNewByName() </code> : to have definitely existed database
    *  range </li>
    * </ul>
    */
    public void _removeByName(){
        boolean bResult = true;
        requiredMethod("addNewByName()");

        log.println("Remove inserted element.");

        try {
            oObj.removeByName(name);
            bResult &= !oObj.hasByName(name);
        } catch (com.sun.star.uno.RuntimeException e) {
            log.println("Exception occured while testing removeByName() : " + e);
            bResult = false;
        }

        log.println("OK.\nTrying to remove unexistant element.");

        try {
            oObj.removeByName(name);
            log.println("Exception expected... - FAILED");
            bResult = false;
        } catch (com.sun.star.uno.RuntimeException e) {
            log.println("Expected exception. - OK : " + e);
        }
        tRes.tested("removeByName()", bResult);
    }
}

