/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _XSystemShellExecute.java,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:04:31 $
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

package ifc.system;

import lib.MultiMethodTest;
import util.utils;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.system.XSystemShellExecute;
import com.sun.star.ucb.XSimpleFileAccess;
import com.sun.star.uno.UnoRuntime;


/**
* Testing <code>com.sun.star.system.XSystemShellExecute</code>
* interface methods :
* <ul>
*  <li><code> execute()</code></li>
* </ul> <p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.system.XSystemShellExecute
*/
public class _XSystemShellExecute extends MultiMethodTest {

    public XSystemShellExecute oObj = null;

    /**
    * Excecutes 'java SystemShellExecute SystemShellExecute.txt' command line.
    * <p>Has <b> OK </b> status if the method successfully returns
    * and file 'SystemShellExecute.txt' was created. <p>
    */
    public void _execute() {
        String cClassPath = System.getProperty("DOCPTH");
        String cResFile = utils.getOfficeTempDirSys((XMultiServiceFactory)tParam.getMSF())+"SystemShellExecute.txt";
        String cResURL = utils.getOfficeTemp((XMultiServiceFactory)tParam.getMSF())+"SystemShellExecute.txt";
        String cArgs = "-classpath " + cClassPath +
                       " SystemShellExecute " + cResFile;

        String jh = System.getProperty("java.home");
        String fs = System.getProperty("file.separator");
        String cmd = jh+fs+"bin"+fs+"java";

        log.println("Executing : '"+cmd+" " + cArgs + "'");
        try {
            oObj.execute(cmd, cArgs, 1);
        } catch (com.sun.star.system.SystemShellExecuteException e) {
            log.println("Exception during execute: " + e);
            log.println("This has been implemented due to security reasons");
            tRes.tested("execute()", true);
            return;
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            log.println("Exception during execute: " + e);
            tRes.tested("execute()", false);
            return;
        }

        XSimpleFileAccess xFileAccess = null;
        try {
            XMultiServiceFactory xMSF = (XMultiServiceFactory)tParam.getMSF();
            Object fa = xMSF.createInstance("com.sun.star.ucb.SimpleFileAccess");
            xFileAccess = (XSimpleFileAccess)
                UnoRuntime.queryInterface(XSimpleFileAccess.class, fa);
        } catch (com.sun.star.uno.Exception e) {
            log.println("Couldn't create SimpleFileAccess:" + e);
            tRes.tested("execute()", false);
        }

        log.println("Waiting while the file will be created or timeout "+
            "reached ...");
        boolean bExist = false;
        int i = 0;
        while (i < 20 && !bExist) {
            try {
                bExist = xFileAccess.exists(cResURL);
            } catch(com.sun.star.uno.Exception e) {
                log.println("Exception:" + e);
            }
            shortWait();
            i++;
        }

        if (bExist) {
            log.println("The command was executed and file created in " +
                 i + " sec.");
        } else {
            log.println("File was not created");
        }

        tRes.tested("execute()", bExist);
    }

    /**
    * Sleeps to allow StarOffice to react on <code>
    * reset</code> call.
    */
    private void shortWait() {
        try {
            Thread.sleep(1000) ;
        } catch (InterruptedException e) {
            log.println("While waiting :" + e) ;
        }
    }
}  // finish class _XSystemShellExecute


