/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ODatabaseContext.java,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:01:26 $
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

package mod._dbaccess;

import com.sun.star.beans.PropertyValue;
import java.io.PrintWriter;

import lib.Status;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;

import com.sun.star.beans.XPropertySet;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.uno.XNamingService;
import com.sun.star.frame.XStorable;
import com.sun.star.sdb.XDocumentDataSource;
import util.utils;

/**
* Test for object which is represented by service
* <code>com.sun.star.sdb.DatabaseContext</code>. <p>
*
* Object implements the following interfaces :
* <ul>
*  <li> <code>com::sun::star::container::XEnumerationAccess</code></li>
*  <li> <code>com::sun::star::container::XNameAccess</code></li>
*  <li> <code>com::sun::star::container::XElementAccess</code></li>
*  <li> <code>com::sun::star::uno::XNamingService</code></li>
* </ul>
*
* @see com.sun.star.container.XNameAccess
* @see com.sun.star.container.XEnumerationAccess
* @see com.sun.star.container.XElementAccess
* @see com.sun.star.uno.XNamingService
* @see com.sun.star.sdb.DatabaseContext
* @see ifc.container.XNameAccess
* @see ifc.container.XEnumerationAccess
* @see ifc.container.XElementAccess
* @see ifc.uno.XNamingService
*/
public class ODatabaseContext extends TestCase {

    /**
    * Does nothing.
    */
    protected void initialize ( TestParameters Param, PrintWriter log) {

    }

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Creates service <code>com.sun.star.sdb.DatabaseContext</code>.
    *     Needed object relations :
    * <ul>
    * <li> <code>'XNamingService.RegisterObject'</code> for
    *      {@link ifc.namingservice.XNamingService} as an
    *      instance of <code>com.sun.star.sdb.DataSource</code>
    *      service. </li>
    * </ul>
    */
    protected TestEnvironment createTestEnvironment(TestParameters Param, PrintWriter log) {
        XInterface oObj = null;
        Object oInterface = null;
        XMultiServiceFactory xMSF = null ;

        try {
            xMSF = (XMultiServiceFactory)Param.getMSF();
            oInterface = xMSF.createInstance( "com.sun.star.sdb.DatabaseContext" );
        }
        catch( com.sun.star.uno.Exception e ) {
            log.println("Service not available" );
            throw new StatusException("Service not available", e) ;
        }

        if (oInterface == null) {
            log.println("Service wasn't created") ;
            throw new StatusException(Status.failed("Service wasn't created")) ;
        }

        oObj = (XInterface) oInterface;

        log.println( "    creating a new environment for object" );
        TestEnvironment tEnv = new TestEnvironment( oObj );

        // adding obj relation for XNamingService
        try {
            xMSF = (XMultiServiceFactory)Param.getMSF();
            oInterface = xMSF.createInstance( "com.sun.star.sdb.DataSource" );

            XPropertySet xDSProps = (XPropertySet)
                UnoRuntime.queryInterface(XPropertySet.class, oInterface) ;

            xDSProps.setPropertyValue("URL", "sdbc:dbase:file:///.") ;

            XDocumentDataSource xDDS = (XDocumentDataSource)
            UnoRuntime.queryInterface(XDocumentDataSource.class, oInterface);
            XStorable store = (XStorable) UnoRuntime.queryInterface(XStorable.class,
            xDDS.getDatabaseDocument ());
            String aFile = utils.getOfficeTemp ((XMultiServiceFactory) Param.getMSF ())+"DatabaseContext.odb";
            log.println("store to '" + aFile + "'");
            store.storeAsURL(aFile,new PropertyValue[]{});

            tEnv.addObjRelation("XNamingService.RegisterObject", oInterface) ;

            tEnv.addObjRelation("INSTANCE", oInterface);

            tEnv.addObjRelation("XContainer.Container",
                (XNamingService) UnoRuntime.queryInterface(
                                    XNamingService.class, oObj));

        } catch (com.sun.star.uno.Exception e) {
            throw new StatusException("Can't create object relation", e) ;
        } catch (NullPointerException e) {
            throw new StatusException("Can't create object relation", e) ;
        }

        return tEnv;
    } // finish method getTestEnvironment

}
