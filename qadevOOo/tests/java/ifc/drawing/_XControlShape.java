/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _XControlShape.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 23:38:38 $
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
import util.FormTools;

import com.sun.star.awt.XControlModel;
import com.sun.star.drawing.XControlShape;
import com.sun.star.lang.XComponent;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

/**
* Testing <code>com.sun.star.drawing.XControlShape</code>
* interface methods :
* <ul>
*  <li><code> getControl()</code></li>
*  <li><code> setControl()</code></li>
* </ul> <p>
* This test needs the following object relations :
* <ul>
*  <li> <code>'xDoc'</code> (of type <code>XComponent</code>):
*   the document where shape tested is situated. This document
*   must also implement <code>XMultiServiceFactory</code> interface
*   to create some control model. </li>
* <ul> <p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.drawing.XControlShape
*/
public class _XControlShape extends MultiMethodTest {

    public XControlShape oObj = null;

    XControlModel model = null;

    /**
    * Test calls the method. <p>
    * Has <b> OK </b> status if the method successfully returns
    * and no exceptions were thrown. <p>
    */
    public void _getControl() {
        model = oObj.getControl() ;

        tRes.tested("getControl()", true) ;
    }

    /**
    * With the help of document passed as relation, a new button control
    * model is created and set as a control. <p>
    * Has <b> OK </b> status if <code>getControl()</code> method returns
    * the same control as was set. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> getControl() </code>  </li>
    * </ul>
    */
    public void _setControl() {
        requiredMethod("getControl()") ;

        XInterface oNewControl = FormTools.createControl
            ((XComponent)tEnv.getObjRelation("xDoc"), "ComboBox") ;

        XControlModel xControl = (XControlModel) UnoRuntime.queryInterface
            (XControlModel.class, oNewControl) ;

        oObj.setControl(xControl) ;

        XControlModel gControl = oObj.getControl() ;

        if (oNewControl.equals(gControl))
            tRes.tested("setControl()", true) ;
        else {
            log.println("Control set is not equal to control get") ;
            tRes.tested("setControl()", false) ;
        }
    }

}  // finish class _XControlShape


