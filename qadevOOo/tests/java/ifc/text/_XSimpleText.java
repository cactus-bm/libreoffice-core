/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _XSimpleText.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:24:07 $
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

import com.sun.star.text.ControlCharacter;
import com.sun.star.text.XSimpleText;
import com.sun.star.text.XTextCursor;

/**
* Testing <code>com.sun.star.text.XSimpleText</code>
* interface methods :
* <ul>
*  <li><code> createTextCursor()</code></li>
*  <li><code> createTextCursorByRange()</code></li>
*  <li><code> insertString()</code></li>
*  <li><code> insertControlCharacter()</code></li>
* </ul> <p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.text.XSimpleText
*/
public class _XSimpleText extends MultiMethodTest {

    XTextCursor oCursor = null;
    public XSimpleText oObj = null;

    /**
    * Creates text cursor. <p>
    * Has <b> OK </b> status if not null value returned. <p>
    */
    public void _createTextCursor() {

        log.println( "Testing createTextCursor()" );
        oCursor = oObj.createTextCursor();
        tRes.tested( "createTextCursor()", oCursor != null );
    }

    /**
    * Inserts a string at the cursor position.<p>
    * Has <b> OK </b> status if the whole result string has a string
    * inserted as its substring. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> createTextCursor() </code> : to have a cursor
    *   where text should be inserted. </li>
    * </ul>
    */
    public void _insertString() {
        requiredMethod("createTextCursor()");
        log.println( "Testing insertString)" );
        String sStr = getInterfaceName() ;
        oObj.insertString( oCursor, sStr, false );
        String gStr = oObj.getText().getString() ;

        tRes.tested( "insertString()", gStr != null &&
            gStr.indexOf(sStr) >= 0) ;
    }

    /**
    * Inserts paragraph break character into text and then checks
    * if this character is present in the result string. <p>
    * Has <b> OK </b> status if the result string has
    * paragraph break character. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> createTextCursor </code> : to have a cursor object. </li>
    * </ul>
    */
    public void _insertControlCharacter() {
        boolean bOK = true;

        requiredMethod("createTextCursor()");
        log.println( "Testing insertControlCharacter()" );
        try {
            oObj.insertControlCharacter( oCursor,
                ControlCharacter.PARAGRAPH_BREAK, false);
            oObj.insertControlCharacter( oCursor,
                ControlCharacter.LINE_BREAK, false);
            oObj.insertString(oObj.createTextCursor(),"newLine",false);
        }
        catch(com.sun.star.lang.IllegalArgumentException e ) {
            // Some exception.FAILED
            Status.failed( e.toString() );
            bOK = false;
        }
        String gStr = oObj.getString() ;

        tRes.tested( "insertControlCharacter()", bOK && gStr != null &&
            gStr.indexOf("\n") > -1);
    }

    /**
    * Creates another text cursor using existing cursor's range. <p>
    * Has <b> OK </b> status if not null value returned. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> createTextCursor </code> : to have a cursor object. </li>
    * </ul>
    */
    public void _createTextCursorByRange() {

        requiredMethod("createTextCursor()");
        oCursor.gotoStart(false);
        log.println( "Testing createTextCursorByRange()" );
        XTextCursor oTCursor = oObj.createTextCursorByRange(oCursor);
        tRes.tested("createTextCursorByRange()", oTCursor != null) ;
    }
}  // finish class _XSimpleText

