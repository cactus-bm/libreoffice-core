/*************************************************************************
 *
 *  $RCSfile: ORootElementValueSetUpdateAccess.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change:$Date: 2003-12-11 11:55:32 $
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
package mod._cfgmgr2;

import java.io.PrintWriter;

import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.utils;

import com.sun.star.beans.PropertyState;
import com.sun.star.beans.PropertyValue;
import com.sun.star.container.XNameAccess;
import com.sun.star.container.XNameReplace;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.util.XChangesBatch;


public class ORootElementValueSetUpdateAccess extends TestCase {
    /** Called to create an instance of <code>TestEnvironment</code> with an
     * object to test and related objects. <br>
     *
     * An instance of com.sun.star.comp.configuration.ConfigurationProvider
     * is created at the MultiServiceFactory of the Office<br>
     * At this an instance of com.sun.star.configuration.ConfigurationAccess is created
     * and from the resulting XNameAccess the first element is taken.
     *
     * @param tParam test parameters
     * @param log writer to log information while testing
     *
     * @see TestEnvironment
     * @see #getTestEnvironment()
     *
     */
    protected TestEnvironment createTestEnvironment(TestParameters tParam,
                                                    PrintWriter log) {
        XInterface oObj = null;
        Object instance = null;
        Object instance1 = null;
        Object instance2 = null;
        log.println("creating the Environment");

        PropertyValue[] nodeArgs = new PropertyValue[1];
        PropertyValue nodepath = new PropertyValue();
        nodepath.Name = "nodepath";
        nodepath.Value = "org.openoffice.Office.Common/ExternalApps";
        nodepath.Handle = -1;
        nodepath.State = PropertyState.DEFAULT_VALUE;
        nodeArgs[0] = nodepath;

        try {
            XInterface Provider = (XInterface) ((XMultiServiceFactory)tParam.getMSF())
                                                     .createInstance("com.sun.star.comp.configuration.ConfigurationProvider");
            XMultiServiceFactory pMSF = (XMultiServiceFactory) UnoRuntime.queryInterface(
                                                XMultiServiceFactory.class,
                                                Provider);
            oObj = (XNameAccess) UnoRuntime.queryInterface(XNameAccess.class,
                                                           pMSF.createInstanceWithArguments(
                                                                   "com.sun.star.configuration.ConfigurationUpdateAccess",
                                                                   nodeArgs));

        } catch (com.sun.star.uno.Exception e) {
            e.printStackTrace();
        }

        log.println("ImplementationName: " + utils.getImplName(oObj));

        TestEnvironment tEnv = new TestEnvironment(oObj);

        String[] pNames = new String[] {
            "file", "ftp", "http", "https", "mailto"
        };

        String[] pTypes = new String[] {
            "String", "String", "String", "String", "String"
        };

        tEnv.addObjRelation("INSTANCE1", "NewOne1");
        tEnv.addObjRelation("NAMEREPLACE", pNames[1]);

        // for XContainer
        tEnv.addObjRelation("INSTANCE", "NewOne2");

        tEnv.addObjRelation("XChangesBatch.PropertyName", pNames[1]);
        tEnv.addObjRelation("XChangesBatch.ChangeElement", "NewOne3");
        tEnv.addObjRelation("XChangesBatch.NameReplace", (XNameReplace)UnoRuntime.queryInterface(XNameReplace.class, oObj));

        tEnv.addObjRelation("XChangesNotifier.PropertyName", pNames[1]);
        tEnv.addObjRelation("XChangesNotifier.ChangeElement", "NewOne4");
        tEnv.addObjRelation("XChangesNotifier.ChangesBatch" , (XChangesBatch)UnoRuntime.queryInterface(XChangesBatch.class, oObj));
        tEnv.addObjRelation("XChangesNotifier.NameReplace", (XNameReplace)UnoRuntime.queryInterface(XNameReplace.class, oObj));

        tEnv.addObjRelation("XLocalizable.ReadOnly", "Locale of ORootElementValueSetUpdateAccess is read Only");
        tEnv.addObjRelation("NoSetName", "ORootElementValueSetUpdateAccess");

        tEnv.addObjRelation("HierachicalName", "/org.openoffice.Office");
        tEnv.addObjRelation("ElementName","http");
        tEnv.addObjRelation("TemplateName", "cfg:value/cfg:any");

        tEnv.addObjRelation("expectedName","mailto");
        return tEnv;
    }

}