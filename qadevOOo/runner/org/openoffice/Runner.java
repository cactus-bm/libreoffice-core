/*************************************************************************
 *
 *  $RCSfile: Runner.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change:$Date: 2003-01-27 16:26:46 $
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

package org.openoffice;

import lib.TestParameters;
import lib.DynamicClassLoader;
import base.TestBase;
import helper.ClParser;
import helper.CfgParser;

/**
 * The main class, will call ClParser and CfgParser to <br>
 * fill the TestParameters.<br>
 * Will then call the appropriate Testbase to run the tests.
 */
public class Runner {

    public static void main(String[] args) {
        TestParameters param = new TestParameters();

        ClParser cli = new ClParser();

        //parse the commandline arguments if an ini-parameter is given
        String iniFile = cli.getIniPath(args);

        //initialize cfgParser with ini-path
        CfgParser ini = new CfgParser(iniFile);

        //parse ConfigFile
        ini.getIniParameters(param);

        //parse the commandline arguments
        cli.getCommandLineParameter(param,args);

        System.out.println("TestJob: "+param.get("TestJob"));

        DynamicClassLoader dcl = new DynamicClassLoader();

        TestBase toExecute = (TestBase) dcl.getInstance("base."+
                                            (String)param.get("TestBase"));

        boolean worked = toExecute.executeTest(param);

        if (!worked) {
            System.out.println("Couldn't execute Job "+param.get("TestJob"));
            System.exit(1);
        } else {
            System.out.println("Job "+param.get("TestJob")+" successful executed");
            System.exit(0);
        }
    }
}
