/*************************************************************************
 *
 *  $RCSfile: _XTypeConverter.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change:$Date: 2003-02-04 11:29:25 $
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

package ifc.script;

import com.sun.star.container.XSet;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.script.XTypeConverter;
import com.sun.star.uno.Any;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;
import com.sun.star.uno.TypeClass;
import lib.MultiMethodTest;

/**
* Testing <code>com.sun.star.script.XTypeConverter</code>
* interface methods :
* <ul>
*  <li><code> convertTo()</code></li>
*  <li><code> convertToSimpleType()</code></li>
* </ul> <p>
* @see com.sun.star.script.XTypeConverter
*/
public class _XTypeConverter extends MultiMethodTest {

    /**
     * oObj filled by MultiMethodTest
     */
    public XTypeConverter oObj = null;

    /**
    * Test creates instance of <code>TypeDescriptionManager</code>,
    * and converts it to <code>XSet</code>. <p>
    * Has <b> OK </b> status if returned value is instance of <code>XSet</code>
    * and no exceptions were thrown. <p>
    * @see com.sun.star.comp.stoc.TypeDescriptionManager
    * @see com.sun.star.container.XSet
    */
    public void _convertTo() {
        XMultiServiceFactory xMSF = tParam.getMSF() ;
        Object value = null ;

        try {
            value = xMSF.createInstance
                ("com.sun.star.comp.stoc.TypeDescriptionManager") ;
        } catch (com.sun.star.uno.Exception e) {
            log.println("Can't create value to convert") ;
            e.printStackTrace(log) ;
            tRes.tested("convertTo()", false) ;
        }

        try {
            Type destType = new Type(XSet.class) ;

            Object o = oObj.convertTo(value, destType);

            boolean result;
            if (o instanceof Any) {
                result = ((Any)o).getType().equals(destType);
            }
            else {
                result = (o instanceof XSet);
            }

            tRes.tested("convertTo()", result) ;
        } catch (com.sun.star.script.CannotConvertException e) {
            log.println("Exception while converting value.") ;
            e.printStackTrace(log) ;
            tRes.tested("convertTo()", false) ;
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            log.println("Exception while converting value.") ;
            e.printStackTrace(log) ;
            tRes.tested("convertTo()", false) ;
        }
    }

    /**
    * Test creates instance of <code>Integer</code>,
    * and converts it to <code>String</code>. <p>
    * Has <b> OK </b> status if returned value is instance of <code>String</code>,
    * if returned value is string representation of integer value that
    * was passed to method and no exceptions were thrown. <p>
    */
    public void _convertToSimpleType() {
        Object value = new Integer(123) ;
        Object destValue = null ;

        try {
            destValue = oObj.convertToSimpleType(value, TypeClass.STRING) ;
        } catch (com.sun.star.script.CannotConvertException e) {
            log.println("Exception while converting value.") ;
            e.printStackTrace(log) ;
            tRes.tested("convertToSimpleType()", false) ;
            return ;
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            log.println("Exception while converting value.") ;
            e.printStackTrace(log) ;
            tRes.tested("convertToSimpleType()", false) ;
            return ;
        }

        tRes.tested("convertToSimpleType()", destValue != null &&
            destValue instanceof String &&
            ((String) destValue).equals("123")) ;
    }
}

