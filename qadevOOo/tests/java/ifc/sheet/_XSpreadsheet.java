/*************************************************************************
 *
 *  $RCSfile: _XSpreadsheet.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change:$Date: 2003-01-27 18:12:16 $
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

package ifc.sheet;

import com.sun.star.sheet.XSheetCellCursor;
import com.sun.star.sheet.XSheetCellRange;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.table.XCellRange;
import com.sun.star.uno.UnoRuntime;
import lib.MultiMethodTest;

/**
* Testing <code>com.sun.star.sheet.XSpreadsheet</code>
* interface methods :
* <ul>
*  <li><code> createCursor()</code></li>
*  <li><code> createCursorByRange()</code></li>
* </ul> <p>
* @see com.sun.star.sheet.XSpreadsheet
*/
public class _XSpreadsheet extends MultiMethodTest {

    // oObj filled by MultiMethodTest
    public XSpreadsheet oObj = null;

    /**
    * Test calls the method and checks returned value. <p>
    * Has <b> OK </b> status if returned value isn't null. <p>
    */
    public void _createCursor() {
        log.println("Testing createCursor");
        XSheetCellCursor oCursor = oObj.createCursor() ;
        tRes.tested("createCursor()", oCursor != null);
    }

    /**
    * Test gets a cell range, call method using this cell range
    * and checks returned value. <p>
    * Has <b> OK </b> status if returned value isn't null
    * and no exceptions were thrown. <p>
    */
    public void _createCursorByRange() {
        log.println("Testing createCursorByRange");

        log.println("getting cellrange");
        XCellRange oRange = null;
        try {
            oRange = oObj.getCellRangeByPosition (1, 1, 2, 3);
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            log.println("Can't get cell range by address");
            e.printStackTrace(log);
            tRes.tested("createCursorByRange()", false);
        }

        XSheetCellRange oSheetRange = (XSheetCellRange)
            UnoRuntime.queryInterface(XSheetCellRange.class, oRange) ;
        log.println("getting Cursor");
        XSheetCellCursor oCursor = oObj.createCursorByRange(oSheetRange);
        tRes.tested("createCursorByRange()", oCursor != null);
    }

} // finisch class _XSpreadsheet


