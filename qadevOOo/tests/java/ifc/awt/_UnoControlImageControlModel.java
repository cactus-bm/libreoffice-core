/*************************************************************************
 *
 *  $RCSfile: _UnoControlImageControlModel.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change:$Date: 2003-01-27 18:07:43 $
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

package ifc.awt;

import lib.MultiPropertyTest;
import lib.MultiPropertyTest$PropertyTester;
import util.utils;

/**
* Testing <code>com.sun.star.awt.UnoControlImageControlModel</code>
* service properties :
* <ul>
*  <li><code> BackgroundColor </code></li>
*  <li><code> Border </code></li>
*  <li><code> DefaultControl </code></li>
*  <li><code> Enabled </code></li>
*  <li><code> ImageURL </code></li>
*  <li><code> Printable </code></li>
* </ul>
* Almost all properties testing is automated by
* <code>lib.MultiPropertyTest</code>.
* @see com.sun.star.awt.UnoControlImageControlModel
*/
public class _UnoControlImageControlModel extends MultiPropertyTest {

    /**
    * Custom <code>PropertyTester</code> used in <code>ImageURL</code> test.
    */
    protected PropertyTester URLTester = new PropertyTester() {
        protected Object getNewValue(String propName, Object oldValue)
                throws java.lang.IllegalArgumentException {
            if (oldValue.equals(util.utils.getFullTestURL("space-metal.jpg")))
                return util.utils.getFullTestURL("crazy-blue.jpg");
            else
                return util.utils.getFullTestURL("space-metal.jpg");
        }
    } ;

    /**
    * Test changes one image URL onto another.
    */
    public void _ImageURL() {
        testProperty("ImageURL", URLTester) ;
    }

    /**
    * This property can be VOID, and in case if it is so new
    * value must defined.
    */
    public void _BackgroundColor() {
        testProperty("BackgroundColor", new PropertyTester() {
            protected Object getNewValue(String propName, Object oldValue) {
                return utils.isVoid(oldValue) ? new Integer(8945) :
                    super.getNewValue(propName, oldValue) ;
            }
        }) ;
    }
}

