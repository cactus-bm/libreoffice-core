/*************************************************************************
 *
 *  $RCSfile: _XFilePicker.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change:$Date: 2003-01-27 18:13:45 $
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

package ifc.ui.dialogs;

import com.sun.star.ui.dialogs.XFilePicker;
import lib.MultiMethodTest;

/**
* Testing <code>com.sun.star.ui.XFilePicker</code>
* interface methods :
* <ul>
*  <li><code> setMultiSelectionMode()</code></li>
*  <li><code> setDefaultName()</code></li>
*  <li><code> setDisplayDirectory()</code></li>
*  <li><code> getDisplayDirectory()</code></li>
*  <li><code> getPath()</code></li>
* </ul> <p>
* The following predefined files needed to complete the test:
* <ul>
*  <li> <code>'space-metal.jpg'</code> : just to exist. </li>
* <ul> <p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.ui.XFolderPicker
*/
public class _XFilePicker extends MultiMethodTest {

    public XFilePicker oObj = null;
    private String dir = null ;
    private String fname = "space-metal.jpg" ;

    /**
    * Sets the current directory to the test document directory. <p>
    * Has <b>OK</b> status if no exceptions occured.
    */
    public void _setDisplayDirectory() {
        boolean result = true ;
        dir = util.utils.getFullTestURL("") ;

        log.println("Trying to set dir '" + dir + "'") ;
        try {
            oObj.setDisplayDirectory(dir) ;
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            log.println("Directory '" + dir + "' not found :" + e) ;
            result = false ;
        }

        tRes.tested("setDisplayDirectory()", result) ;
    }

    /**
    * Gets the current directory. <p>
    * Has <b>OK</b> status if get value is equal to set value
    * passed to <code>setDisplayDirectory</code> <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> setDisplayDirectory </code>  </li>
    * </ul>
    */
    public void _getDisplayDirectory() {
        requiredMethod("setDisplayDirectory()") ;

        String gDir = oObj.getDisplayDirectory() ;

        log.println("Get dir '" + gDir + "'") ;

        tRes.tested("getDisplayDirectory()", dir.equals(gDir)) ;
    }

    /**
    * Sets default name to file name existing in test document
    * directory ('space-metal.jpg'). <p>
    * Has <b>OK</b> status if no exceptions occured.
    */
    public void _setDefaultName() {
        boolean result = true ;

        try {
            oObj.setDefaultName(fname) ;
        } catch (Exception e) {
            log.println("Exception setting default name :" + e) ;
            result = false ;
        }

        tRes.tested("setDefaultName()", result) ;
    }

    /**
    * Just switch object to MultiSelectionMode. There is no ways
    * to check this method (only interactively). <p>
    * Has <b>OK</b> status if no runtime exceptions occured.
    */
    public void _setMultiSelectionMode() {

        oObj.setMultiSelectionMode(true) ;

        tRes.tested("setMultiSelectionMode()", true) ;
    }

    /**
    * Gets completed path from dialog. If <code>execute()</code>
    * method was not called then zero length array is returned.
    * So to check actual functionality of this method interactive
    * testing is required. <p>
    * Has <b>OK</b> status if zero length array returned (this
    * occurs if <code>execute()</code>
    * method was not called yet) or if array contains at least one
    * element and it equals to <code>[Directory set] +
    * [Default file name set]</code>. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> setDisplayDirectory </code> </li>
    *  <li> <code> setDefaultName </code> </li>
    * </ul>
    */
    public void _getFiles() {
        requiredMethod("setDisplayDirectory()");
        requiredMethod("setDefaultName()");

        String[] files = oObj.getFiles();

        if (files.length > 0) {
            log.println("Path get : '" + files[0] + "'") ;

            tRes.tested("getFiles()", (dir + fname).equals(files[0])) ;
        } else {
            log.println("No files were selected or execute() method was not called.") ;

            tRes.tested("getFiles()", true);
        }
    }

}


