/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _XCachedContentResultSetStubFactory.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:29:44 $
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

package ifc.ucb;

import lib.MultiMethodTest;
import lib.Status;
import lib.StatusException;

import com.sun.star.sdbc.XResultSet;
import com.sun.star.ucb.XCachedContentResultSetStubFactory;

/**
* Testing <code>com.sun.star.ucb.XCachedContentResultSetStubFactory</code>
* interface methods :
* <ul>
*  <li><code> createCachedContentResultSetStub()</code></li>
* </ul> <p>
* This test needs the following object relations :
* <ul>
*  <li> <code>'ContentResultSet'</code> (of type
*   <code>com.sun.star.sdbc.XResultSet</code>):
*   this must be an imlementation of <code>
*   com.sun.star.ucb.ContentResultSet</code> service.</li>
* <ul> <p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.ucb.XCachedContentResultSetStubFactory
*/
public class _XCachedContentResultSetStubFactory extends MultiMethodTest {

    /**
     * Conatins the tested object.
     */
    public XCachedContentResultSetStubFactory oObj;
    private XResultSet resSet = null ;

    /**
    * Retrieves object relation.
    * @throws StatusException If relation not found.
    */
    public void before() {
        resSet = (XResultSet) tEnv.getObjRelation("ContentResultSet") ;
        if (resSet == null) {
            log.println("!!! Relation not found !!!") ;
            throw new StatusException(Status.failed("!!! Relation not found !!!")) ;
        }
    }

    /**
    * Creates cached result set stub from static result set. After that number
    * of rows in cached result set created and its source set are retrieved
    * and comared. <p>
    * Has <b>OK</b> status if numbers of rows are equal and they are
    * greater then 0 (because JAR file contains at least one entry).
    */
    public void _createCachedContentResultSetStub() {
        boolean result = true ;

        XResultSet resSetStub = oObj.createCachedContentResultSetStub
            (resSet) ;

        if (resSetStub == null) {
            log.println("!!! Method returned null !!!") ;
            result = false ;
        } else {
            try {
                resSetStub.last() ;
                int stubRowNum = resSetStub.getRow() ;

                resSet.last() ;
                int setRowNum = resSet.getRow() ;

                result = stubRowNum == setRowNum && setRowNum > 0 ;

                log.println("Number of rows : set=" + setRowNum +
                    " stub=" + stubRowNum) ;
            } catch (com.sun.star.sdbc.SQLException e) {
                log.println("!!! Something wrong with result sets :") ;
                e.printStackTrace(log) ;
                result = false ;
            }
        }

        tRes.tested("createCachedContentResultSetStub()", result) ;

    }
}

