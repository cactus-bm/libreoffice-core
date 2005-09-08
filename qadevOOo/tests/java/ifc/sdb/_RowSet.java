/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _RowSet.java,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:25:19 $
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

package ifc.sdb;

import lib.MultiPropertyTest;
import lib.StatusException;

import com.sun.star.sdbc.XConnection;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;

/**
 * Testing <code>com.sun.star.sdb.RowSet</code>
 * service properties :
 * <ul>
 *  <li><code> ActiveConnection</code></li>
 *  <li><code> DataSourceName</code></li>
 *  <li><code> Command</code></li>
 *  <li><code> CommandType</code></li>
 *  <li><code> ActiveCommand</code></li>
 *  <li><code> IgnoreResult</code></li>
 *  <li><code> Filter</code></li>
 *  <li><code> ApplyFilter</code></li>
 *  <li><code> Order</code></li>
 *  <li><code> Privileges</code></li>
 *  <li><code> IsModified</code></li>
 *  <li><code> IsNew</code></li>
 *  <li><code> RowCount</code></li>
 *  <li><code> IsRowCountFinal</code></li>
 *  <li><code> UpdateTableName</code></li>
 *  <li><code> UpdateCatalogName</code></li>
 *  <li><code> UpdateSchemaName</code></li>
 * </ul> <p>
 *
 * Properties are tested in a safe way, this means that old
 * properties' values are restored to their previous values
 * after testing. These values are meaningfull for further
 * testing. <p>
 *
 * Properties testing is automated by <code>lib.MultiPropertyTest</code>.
 * @see com.sun.star.sdb.RowSet
 */
public class _RowSet extends MultiPropertyTest {

    /**
     * The tester implementation which restores properties
     * values after testing.
     */
    protected class SafeTester extends PropertyTester {
        Object oldValue = null ;

        protected Object getNewValue(String prop, Object old) {
            log.println("Testing with SafeTester ...") ;
            oldValue = old ;
            return super.getNewValue(prop, old) ;
        }

        protected void checkResult(String propName, Object oldValue,
            Object newValue, Object resValue, Exception exception)
            throws Exception {

            super.checkResult(propName, oldValue, newValue, resValue, exception);

            try {
                oObj.setPropertyValue(propName, this.oldValue);
            } catch (com.sun.star.lang.WrappedTargetException e) {
                log.println("Exception while setting property to its old value '" +
                    this.oldValue + "' (ignoring) : " + e );
            } catch (com.sun.star.lang.IllegalArgumentException e) {
                log.println("Exception while setting property to its old value '" +
                    this.oldValue + "' (ignoring) : " + e );
            } catch (com.sun.star.beans.PropertyVetoException e) {
                log.println("Exception while setting property to its old value '" +
                    this.oldValue + "' (ignoring) : " + e );
            } catch (com.sun.star.beans.UnknownPropertyException e) {
                log.println("Exception while setting property to its old value '" +
                    this.oldValue + "' (ignoring) : " + e );
            }

        }
    }

    /**
     * Overriden method which tests all the properties
     * with <code>SafeTester</code>.
     *
     * @see #SafeTester
     */
    protected void testProperty(String propName) {
        testProperty(propName, new SafeTester()) ;
    }

    public void _ActiveConnection() {
        boolean result = false;
        try {
            XConnection the_connection = null;

            try {
                the_connection = (XConnection) AnyConverter.toObject(
                                    new Type(XConnection.class),
                                    oObj.getPropertyValue("ActiveConnection"));
            } catch (com.sun.star.lang.IllegalArgumentException iae) {
                throw new StatusException("couldn't convert Any",iae);
            }

            result = (the_connection != null);
        } catch (com.sun.star.beans.UnknownPropertyException e) {
            log.println("the property is unknown");
        } catch (com.sun.star.lang.WrappedTargetException e) {
            log.println(e.getMessage());
        }

        tRes.tested("ActiveConnection", result) ;
    }

    /**
    * Forces environment recreation.
    */
    protected void after() {
        disposeEnvironment();
    }

} // EOF DatasourceAdministrationDialog

