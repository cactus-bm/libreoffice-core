/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _UnoControlComboBoxModel.java,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 22:49:40 $
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

package ifc.awt;

import lib.MultiPropertyTest;
import util.utils;

/*
* Testing <code>com.sun.star.awt.UnoControlComboBoxModel</code>
* service properties :
* <ul>
*  <li><code> Autocomplete</code></li>
*  <li><code> BackgroundColor</code></li>
*  <li><code> Border</code></li>
*  <li><code> DefaultControl</code></li>
*  <li><code> Dropdown</code></li>
*  <li><code> Enabled</code></li>
*  <li><code> FontDescriptor</code></li>
*  <li><code> LineCount</code></li>
*  <li><code> MaxTextLen</code></li>
*  <li><code> Printable</code></li>
*  <li><code> ReadOnly</code></li>
*  <li><code> StringItemList</code></li>
*  <li><code> Tabstop</code></li>
*  <li><code> Text</code></li>
*  <li><code> TextColor</code></li>
* </ul>
* @see com.sun.star.awt.UnoControlComboBoxModel
*/
public class _UnoControlComboBoxModel extends MultiPropertyTest {

    /**
    * This property can be VOID, and in case if it is so new
    * value must defined.
    */
    public void _BackgroundColor() {
        testProperty("BackgroundColor", new PropertyTester() {
            protected Object getNewValue(String p, Object old) {
                return utils.isVoid(old) ? new Integer(1234) : null ;
            }
        }) ;
    }

    /**
    * This property can be VOID, and in case if it is so new
    * value must defined.
    */
    public void _BorderColor() {
        testProperty("BorderColor", new PropertyTester() {
            protected Object getNewValue(String p, Object old) {
                return utils.isVoid(old) ? new Integer(1234) : null ;
            }
        }) ;
    }

    /**
    * This property can be VOID, and in case if it is so new
    * value must defined.
    */
    public void _TextColor() {
        testProperty("TextColor", new PropertyTester() {
            protected Object getNewValue(String p, Object old) {
                return utils.isVoid(old) ? new Integer(4321) : null ;
            }
        }) ;
    }

    /**
    * This property can be VOID, and in case if it is so new
    * value must defined.
    */
    public void _Tabstop() {
        testProperty("Tabstop", new PropertyTester() {
            protected Object getNewValue(String p, Object old) {
                return utils.isVoid(old) ? new Boolean(true) : null ;
            }
        }) ;
    }

    public void _Text() {
        try {
            oObj.setPropertyValue("MaxTextLen",new Short((short)0));
        } catch (com.sun.star.beans.UnknownPropertyException ue) {
        } catch (com.sun.star.beans.PropertyVetoException pe) {
        } catch (com.sun.star.lang.IllegalArgumentException ie) {
        } catch (com.sun.star.lang.WrappedTargetException we) {
        }
        testProperty("Text");
    }

    /**
    * Redefined method returns value, that differs from property value.
    */
    protected PropertyTester ColorTester = new PropertyTester() {
        protected Object getNewValue(String propName, Object oldValue) {
            if (util.ValueComparer.equalValue(oldValue, new Integer(17)))
                return new Integer(25);
            else
                return new Integer(17);
        }
    };

    /**
    * This property can be void, so if old value is <code> null </code>
    * new value must be specified.
    */
    public void _TextLineColor() {
        log.println("Testing with custom Property tester") ;
        testProperty("TextLineColor", ColorTester) ;
    }

} //EOF UnoControlComboBoxModel

