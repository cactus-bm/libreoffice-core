/*************************************************************************
 *
 *  $RCSfile: java_fat.java,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change:$Date: 2003-05-27 12:00:43 $
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

package base;


import java.io.PrintWriter;

import lib.TestParameters;
import lib.TestCase;
import lib.Status;
import lib.MultiMethodTest;
import lib.TestEnvironment;
import lib.DynamicClassLoader;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.connection.XConnector;
import com.sun.star.connection.XConnection;

import com.sun.star.bridge.XUnoUrlResolver;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.uno.XNamingService;
import com.sun.star.uno.XComponentContext;

import share.DescEntry;
import share.DescGetter;
import helper.APIDescGetter;
import helper.OfficeProvider;
import base.TestBase;

import share.LogWriter;
import stats.Summarizer;

/**
 *
 * this class handles tests written in java and running on a fat Office
 */
public class java_fat implements TestBase {

    public static boolean debug = false;

    public boolean executeTest(lib.TestParameters param) {
        DescGetter dg = new APIDescGetter();
        String job = (String) param.get("TestJob");
        boolean retValue = true;
        debug = ((Boolean) param.get("DebugIsActive")).booleanValue();

        //get Job-Descriptions
        System.out.print("Getting Descriptions for Job: "+job+" from ");
        DescEntry[] entries = dg.getDescriptionFor(job,
                (String) param.get("DescriptionPath"),debug);

        if (entries == null ) {
            System.out.println("Couldn't get Description for Job");
            return false;
        }

        String conStr = (String) param.get("ConnectionString");
        System.out.println("> ");
        System.out.print("Connecting the Office");
        System.out.println(" With "+conStr);

        OfficeProvider office = new OfficeProvider();

        for (int l=0;l<entries.length;l++) {

            if (entries[l] == null ) {
                continue;
            }

            if (entries[l].hasErrorMsg) {
                System.out.println(entries[l].ErrorMsg);
                continue;
            }

            XMultiServiceFactory msf = (XMultiServiceFactory)
                                            office.getManager(param);
            if (msf == null) {
                retValue = false;
                continue;
            }
            param.put("ServiceFactory",msf);

            DescEntry entry = entries[l];

            //get some helper classes
            Summarizer sumIt = new Summarizer();
            DynamicClassLoader dcl = new DynamicClassLoader();

            TestCase tCase = null;

            try {
                tCase = (TestCase)
                            dcl.getInstance("mod._"+entry.entryName);
            } catch (java.lang.IllegalArgumentException ie) {
                entry.ErrorMsg=ie.getMessage();
                entry.hasErrorMsg=true;
            } catch (java.lang.NoClassDefFoundError ie) {
                entry.ErrorMsg=ie.getMessage();
                entry.hasErrorMsg=true;
            }

            if (tCase == null) {
                sumIt.summarizeDown(entry,entry.ErrorMsg);
                LogWriter sumObj = (LogWriter) dcl.getInstance(
                                                (String)param.get("OutProducer"));
                sumObj.initialize(entry,true);
                entry.UserDefinedParams = param;
                sumObj.summary(entry);
                continue;
            }

            System.out.println("Creating: "+tCase.getObjectName());
            LogWriter log = (LogWriter) dcl.getInstance(
                                            (String)param.get("LogWriter"));
            log.initialize(entry,true);
            entry.UserDefinedParams = param;
            TestEnvironment tEnv = null;
            try {
                tCase.setLogWriter((PrintWriter) log);
                tCase.initializeTestCase(param);
                tEnv = tCase.getTestEnvironment(param);
            } catch (Exception e) {
                System.out.println("Exception while creating "+tCase.getObjectName());
                System.out.println("Message "+e.getMessage());
                tEnv = null;
            } catch (java.lang.UnsatisfiedLinkError e) {
                System.out.println("Exception while creating "+tCase.getObjectName());
                System.out.println("Message "+e.getMessage());
                tEnv = null;
            } catch (java.lang.NoClassDefFoundError e) {
                System.out.println("Exception while creating "+tCase.getObjectName());
                System.out.println("Message "+e.getMessage());
                tEnv = null;
            }

            if (tEnv == null) {
                sumIt.summarizeDown(entry,"Couldn't create "+tCase.getObjectName());
                LogWriter sumObj = (LogWriter) dcl.getInstance(
                                                (String)param.get("OutProducer"));
                sumObj.initialize(entry,true);
                entry.UserDefinedParams = param;
                sumObj.summary(entry);
                continue;
            }
            System.out.println(tCase.getObjectName()+" recreated ");
            for (int j=0;j<entry.SubEntryCount;j++) {
                if (!entry.SubEntries[j].isToTest) {
                    Summarizer.summarizeDown(entry.SubEntries[j],"not part of the job");
                    continue;
                }
                System.out.println("running: "+entry.SubEntries[j].entryName);
                LogWriter ifclog = (LogWriter) dcl.getInstance(
                                           (String)param.get("LogWriter"));

                ifclog.initialize(entry.SubEntries[j],true);
                entry.SubEntries[j].UserDefinedParams = param;
                entry.SubEntries[j].Logger = ifclog;

                if (tEnv == null || tEnv.isDisposed()) {
                    helper.ProcessHandler ph =
                                (helper.ProcessHandler) param.get("AppProvider");
                    if (ph != null) {
                        ph.kill();
                        shortWait(5000);
                    }
                    tEnv = getEnv(entry,param);
                }

                MultiMethodTest ifc = null;
                lib.TestResult res = null;
                try {
                    ifc = (MultiMethodTest) dcl.getInstance(
                                              entry.SubEntries[j].entryName);
                    res = ifc.run(entry.SubEntries[j],tEnv,param);
                } catch (IllegalArgumentException iae) {
                    System.out.println("Couldn't load class "+entry.SubEntries[j].entryName);
                    System.out.println("**** "+iae.getMessage()+" ****");
                    Summarizer.summarizeDown(entry.SubEntries[j],iae.getMessage());
                } catch (java.lang.NoClassDefFoundError iae) {
                    System.out.println("Couldn't load class "+entry.SubEntries[j].entryName);
                    System.out.println("**** "+iae.getMessage()+" ****");
                    Summarizer.summarizeDown(entry.SubEntries[j],iae.getMessage());
                } catch (java.lang.RuntimeException e) {
                    helper.ProcessHandler ph =
                                (helper.ProcessHandler) param.get("AppProvider");
                    if (ph != null) {
                        ph.kill();
                        shortWait(5000);
                    }
                    tEnv = getEnv(entry,param);
                    ifc = (MultiMethodTest) dcl.getInstance(
                                              entry.SubEntries[j].entryName);
                    if ((tEnv != null) && (ifc != null)) {
                        res = ifc.run(entry.SubEntries[j],tEnv,param);
                    } else res = null;
                }
                if (res != null) {
                    for (int k=0;k<entry.SubEntries[j].SubEntryCount;k++) {
                        if (res.hasMethod(entry.SubEntries[j].SubEntries[k].entryName)) {
                            entry.SubEntries[j].SubEntries[k].State=
                                res.getStatusFor(entry.SubEntries[j].SubEntries[k].entryName).toString();
                        }
                    }
                }
                sumIt.summarizeUp(entry.SubEntries[j]);

                LogWriter sumIfc = (LogWriter) dcl.getInstance(
                                                (String)param.get("OutProducer"));

                sumIfc.initialize(entry.SubEntries[j],true);
                entry.SubEntries[j].UserDefinedParams = param;
                sumIfc.summary(entry.SubEntries[j]);
            }
            try {
                tCase.cleanupTestCase(param);
            } catch (Exception e) {
                System.out.println("couldn't cleanup");
            } catch (java.lang.NoClassDefFoundError e){
                System.out.println("couldn't cleanup");
            }
            sumIt.summarizeUp(entry);
            LogWriter sumObj = (LogWriter) dcl.getInstance(
                                                (String)param.get("OutProducer"));
            sumObj.initialize(entry,true);
            sumObj.summary(entry);
        }
        if (entries.length > 1) {
            System.out.println();
            int counter = 0;
            System.out.println("Failures that appeared during scenario execution:");
            for (int i=0;i<entries.length;i++) {
                if (! entries[i].State.endsWith("OK")) {
                    System.out.println("\t "+entries[i].longName);
                    counter++;
                }
            }
            System.out.println(counter +" of "+entries.length + " tests failed" );
        }
        helper.ProcessHandler ph =
                                (helper.ProcessHandler) param.get("AppProvider");
        if (ph != null) {
            ph.kill();
            shortWait(5000);
        }
        return retValue;
    }

    protected TestEnvironment getEnv(DescEntry entry, TestParameters param) {
            OfficeProvider office = new OfficeProvider();
            XMultiServiceFactory msf = (XMultiServiceFactory)
                                            office.getManager(param);
            if (msf == null) return null;
            param.put("ServiceFactory",msf);

            DynamicClassLoader dcl = new DynamicClassLoader();

            TestCase tCase = null;

            try {
                tCase = (TestCase)
                            dcl.getInstance("mod._"+entry.entryName);
            } catch (java.lang.IllegalArgumentException ie) {
                entry.ErrorMsg=ie.getMessage();
                entry.hasErrorMsg=true;
            } catch (java.lang.NoClassDefFoundError ie) {
                entry.ErrorMsg=ie.getMessage();
                entry.hasErrorMsg=true;
            }

            System.out.println("Creating: "+tCase.getObjectName());
            LogWriter log = (LogWriter) dcl.getInstance(
                                            (String)param.get("LogWriter"));
            log.initialize(entry,true);
            entry.UserDefinedParams = param;
            tCase.setLogWriter((PrintWriter) log);
            TestEnvironment tEnv = null;
            try {
                tCase.initializeTestCase(param);
                tEnv = tCase.getTestEnvironment(param);
            } catch (com.sun.star.lang.DisposedException de) {
                System.out.println("Office disposed");
                helper.ProcessHandler ph =
                                (helper.ProcessHandler) param.get("AppProvider");
                if (ph != null) {
                    ph.kill();
                    shortWait(5000);
                }
            } catch (lib.StatusException e) {
                System.out.println(e.getMessage());
                helper.ProcessHandler ph =
                                (helper.ProcessHandler) param.get("AppProvider");
                if (ph != null) {
                    ph.kill();
                    shortWait(5000);
                }
                entry.ErrorMsg = e.getMessage();
                entry.hasErrorMsg = true;
            }
            return tEnv;
    }

    protected void shortWait(int millis) {
        try {
            Thread.sleep(millis);
        } catch (java.lang.InterruptedException ie) {}
    }
}
