/*************************************************************************
 *
 *  $RCSfile: OfficeProvider.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change:$Date: 2003-05-15 18:27:55 $
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

package helper;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.lang.XComponent;
import com.sun.star.frame.XDesktop;
import com.sun.star.connection.XConnector;
import com.sun.star.connection.XConnection;

import com.sun.star.bridge.XUnoUrlResolver;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.uno.XNamingService;
import com.sun.star.uno.XComponentContext;

/**
 * This class will connect the office and start it if possible
 *
 */
public class OfficeProvider implements AppProvider {

    protected static boolean debug = false;

    public boolean disposeManager(lib.TestParameters param) {
        XMultiServiceFactory msf = param.getMSF();
        if (msf == null) {
            return true;
        } else {
            XDesktop desk = null;
            try {
                desk = (XDesktop) UnoRuntime.queryInterface(
                    XDesktop.class, msf.createInstance(
                                "com.sun.star.frame.Desktop"));
            } catch (com.sun.star.uno.Exception ue) {
                return false;
            }

            msf = null;
            if (desk != null) {
                desk.terminate();
                return true;
            } else {
                return false;
            }
        }
    }

    /**
     * Method to get the ServiceManager of an Office
     */
    public Object getManager(lib.TestParameters param) {
        String cncstr = "uno:"+param.get("ConnectionString")+
                                                ";urp;StarOffice.ServiceManager";
        debug = ((Boolean) param.get("DebugIsActive")).booleanValue();
        XMultiServiceFactory msf = null;
        String exc = "";
        try {
            msf = connect(cncstr);
        } catch (com.sun.star.uno.Exception ue) {
            exc=ue.getMessage();
        } catch (java.lang.Exception je) {
            exc=je.getMessage();
        }
        if (msf != null) {
            return msf;
        }
        if (debug) System.out.println("Not connected to existing office");
        boolean isExecutable = false;
        boolean isAppKnown = cncstr.indexOf("host=localhost")>0;
        isAppKnown &= !((String) param.get("AppExecutionCommand")).equals("");
        if (isAppKnown) {
            if (debug)
                System.out.println("Local Connection trying to start the Office");
            String cmd = (String) param.get("AppExecutionCommand");
            ProcessHandler ph = new ProcessHandler(cmd);
            isExecutable = ph.executeAsynchronously();
            if (isExecutable) {
                param.put("AppProvider",ph);
                OfficeWatcher ow = new OfficeWatcher(param);
                param.put("Watcher",ow);
                ow.start();
                ow.ping();
            }
        } else {
            return msf;
        }
        exc = "";
        int k=0;
        while ((k<11) && (msf==null)) {
            try {
                Thread.sleep(k*1000);
                msf = connect(cncstr);
            } catch (com.sun.star.uno.Exception ue) {
                exc=ue.getMessage();
            } catch (java.lang.Exception je) {
                exc=je.getMessage();
            }
            k++;
        }
        if (debug && msf==null) System.out.println("Exception while connecting "+exc);
        return msf;
    }

    protected static XMultiServiceFactory connect( String connectStr )
                            throws com.sun.star.uno.Exception,
                                com.sun.star.uno.RuntimeException,
                                    com.sun.star.connection.NoConnectException,
                                                                    Exception {
        // Get component context
        XComponentContext xcomponentcontext =
            com.sun.star.comp.helper.Bootstrap.createInitialComponentContext(
                                                                         null );

        // initial serviceManager
        XMultiComponentFactory xLocalServiceManager =
                                          xcomponentcontext.getServiceManager();

        // create a connector, so that it can contact the office
        Object  xUrlResolver  = xLocalServiceManager.createInstanceWithContext(
                        "com.sun.star.bridge.UnoUrlResolver", xcomponentcontext );
        XUnoUrlResolver urlResolver = (XUnoUrlResolver)UnoRuntime.queryInterface(
                                            XUnoUrlResolver.class, xUrlResolver );

        Object rInitialObject = urlResolver.resolve( connectStr );

        XMultiServiceFactory xMSF = null;

        if( rInitialObject != null ) {
            if (debug) System.out.println( "resolved url" );
            xMSF = (XMultiServiceFactory)
            UnoRuntime.queryInterface( XMultiServiceFactory.class, rInitialObject );
        }

        return ( xMSF );
    }

}