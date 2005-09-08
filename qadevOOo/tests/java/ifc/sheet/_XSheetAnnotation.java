/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _XSheetAnnotation.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:52:19 $
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
import lib.Status;
import lib.StatusException;
import util.ValueComparer;

import com.sun.star.sheet.XSheetAnnotation;
import com.sun.star.table.CellAddress;
import com.sun.star.text.XSimpleText;
import com.sun.star.uno.UnoRuntime;

/**
* Testing <code>com.sun.star.sheet.XSheetAnnotation</code>
* interface methods :
* <ul>
*  <li><code> getPosition()</code></li>
*  <li><code> getAuthor()</code></li>
*  <li><code> getDate()</code></li>
*  <li><code> getIsVisible()</code></li>
*  <li><code> setIsVisible()</code></li>
* </ul> <p>
* This test needs the following object relations :
* <ul>
*  <li> <code>'CELLPOS'</code> (of type
*   <code>com.sun.star.table.CellAddress</code>):
*   The position of cell with annotation. </li>
* <ul> <p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.sheet.XSheetAnnotation
*/
public class _XSheetAnnotation extends MultiMethodTest {

    public XSheetAnnotation oObj = null;

        /**
        * Gets the author of annotation. <p>
        * Has <b>OK</b> status if not null value returned.
        */
        public void _getAuthor() {
          String author = oObj.getAuthor();
          tRes.tested("getAuthor()", author != null);
        }

        /**
        * Gets the modification date of annotation. <p>
        * Has <b>OK</b> status if not null value returned.
        */
        public void _getDate() {
          String date = oObj.getDate();
          tRes.tested("getDate()", date != null);
        }

        /**
        * Sets the string of annotation, then makes it visible and
        * checks the value returned by <code>getIsVisible</code> method. <p>
        * Has <b>OK</b> status if the method returns <code>true</code>.
        */
        public void _getIsVisible() {
          XSimpleText oText  = (XSimpleText)
            UnoRuntime.queryInterface(XSimpleText.class, oObj);
          oText.setString("XSheetAnnotation");
          oObj.setIsVisible(true);
          boolean bVis = oObj.getIsVisible();
          tRes.tested("getIsVisible()", bVis);
        }

        /**
        * Gets the position of annotated cell and compares it to
        * the position passed as relation. <p>
        * Has <b>OK</b> status if these positions are equal and not
        * null.
        */
        public void _getPosition() {
          boolean bResult = false;
          CellAddress sCAddr = (CellAddress) tEnv.getObjRelation("CELLPOS") ;
          if (sCAddr == null) throw new StatusException(Status.failed
            ("Relation 'CELLPOS' not found"));

          CellAddress oCAddr = oObj.getPosition();

          bResult = (oCAddr != null) && (sCAddr != null) &&
            ValueComparer.equalValue(oCAddr, sCAddr) ;

          tRes.tested("getPosition()", bResult);
        }

        /**
        * Sets the string of annotation, makes it hidden and then
        * visible. Visibility is checked in both cases. <p>
        * Has <b>OK</b> status if the <code>getIsVisible</code> method
        * returns <code>flase</code> in the first case and <code>true</code>
        * in the second.
        */
        public void _setIsVisible() {
          boolean bResult = true;
          XSimpleText oText  = (XSimpleText)
              UnoRuntime.queryInterface(XSimpleText.class, oObj);
          oText.setString("XSheetAnnotation");
          oObj.setIsVisible(false);
          boolean bVis = oObj.getIsVisible();
          if (!bVis) {
              oObj.setIsVisible(true);
              bVis = oObj.getIsVisible();
              if (bVis) {
                  bResult = true;
              }
          }

          tRes.tested("setIsVisible()", bResult);
        }

} // EOC _XSheetAnnotation

