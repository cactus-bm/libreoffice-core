/*************************************************************************
 *
 *  $RCSfile: _XMasterPageTarget.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change:$Date: 2003-01-27 18:09:10 $
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

package ifc.drawing;

import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XDrawPages;
import com.sun.star.drawing.XMasterPageTarget;
import lib.MultiMethodTest;

/**
* Testing <code>com.sun.star.drawing.XMasterPageTarget</code>
* interface methods :
* <ul>
*  <li><code> getMasterPage()</code></li>
*  <li><code> setMasterPage()</code></li>
* </ul> <p>
* This test needs the following object relations :
* <ul>
*  <li> <code>'MasterPageSupplier'</code>
*   (of type <code>XDrawPages</code>):
*    the relation is used to create a new draw page. </li>
* <ul> <p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.drawing.XMasterPageTarget
*/
public class _XMasterPageTarget extends MultiMethodTest{

    public XMasterPageTarget oObj = null;
    public XDrawPage DrawPage = null;

    /**
    * Gets the master page and stores. <p>
    * Has <b> OK </b> status if the value returned is not null. <p>
    */
    public void _getMasterPage(){
        boolean result = false;
        log.println("get the MasterPage");

        DrawPage = oObj.getMasterPage();
        result = DrawPage != null ;

        tRes.tested("getMasterPage()",result);
    }

    /**
    * Gets the pages supplier from relation. Then a new page is created,
    * inserted and set as master page.<p>
    * Has <b> OK </b> status if the master page get is equal to page
    * which was set. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> getMasterPage() </code> : to get old master page.</li>
    * </ul>
    */
    public void _setMasterPage() {
        requiredMethod("getMasterPage()") ;

        boolean result = true;
        XDrawPage oNewPage = null;

        XDrawPages oPages = (XDrawPages) tEnv.getObjRelation("MasterPageSupplier");
        log.println("inserting new MasterPage");
        oNewPage = oPages.insertNewByIndex(oPages.getCount());

        // save the old MasterPage
        XDrawPage oOldPage = DrawPage;

        // set the new Page as MasterPage
        log.println("set the new MasterPage");
        oObj.setMasterPage(oNewPage);

        // get the new MasterPage
        DrawPage = oObj.getMasterPage();

        // test the diffrent MasterPages
        if (DrawPage.equals(oOldPage)) result = false;

        tRes.tested("setMasterPage()",result);
    }

} // end of XMasterPageTarget

