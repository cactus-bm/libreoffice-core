/*************************************************************************
 *
 *  $RCSfile: FrameworkJarChecker.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: toconnor $ $Date: 2002-11-13 17:44:38 $
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

package org.openoffice.netbeans.modules.office.utils;

import java.io.File;
import java.io.IOException;
import java.beans.PropertyVetoException;

import org.openide.filesystems.Repository;
import org.openide.filesystems.FileSystem;
import org.openide.filesystems.JarFileSystem;

import org.openoffice.idesupport.SVersionRCFile;
import org.openoffice.netbeans.modules.office.options.OfficeSettings;

public class FrameworkJarChecker {

    public static void mountDependencies() {
        String unoilPath = SVersionRCFile.getPathForUnoil(
            OfficeSettings.getDefault().getOfficeDirectory());

        if (unoilPath == null)
            return;

        File unoilFile = new File(unoilPath + File.separator + "unoil.jar");
        JarFileSystem jfs = new JarFileSystem();
        try {
            jfs.setJarFile(unoilFile);
        }
        catch (IOException ioe) {
            return;
        }
        catch (PropertyVetoException pve) {
            return;
        }

        FileSystem result;
        try {
            result =
                Repository.getDefault().findFileSystem(jfs.getSystemName());
        }
        catch(Exception exp) {
            result = null;
        }
        finally {
            jfs.removeNotify();
        }

        if(result == null) {
            // warnBeforeMount();
            JarFileSystem newjfs = new JarFileSystem();
            try {
                newjfs.setJarFile(unoilFile);
            }
            catch (IOException ioe) {
                return;
            }
            catch (PropertyVetoException pve) {
                return;
            }
            Repository.getDefault().addFileSystem(newjfs);
            newjfs.setHidden(true);
        }
    }

    public static void unmountDependencies() {
        String unoilPath = SVersionRCFile.getPathForUnoil(
            OfficeSettings.getDefault().getOfficeDirectory());

        if (unoilPath == null)
            return;

        File unoilFile = new File(unoilPath + File.separator + "unoil.jar");
        JarFileSystem jfs = new JarFileSystem();
        try {
            jfs.setJarFile(unoilFile);
        }
        catch (IOException ioe) {
            return;
        }
        catch (PropertyVetoException pve) {
            return;
        }

        FileSystem result;
        try {
            result =
                Repository.getDefault().findFileSystem(jfs.getSystemName());
            if(result != null)
                Repository.getDefault().removeFileSystem(result);
        }
        catch(Exception exp) {
        }
    }

    private static void warnBeforeMount() {
        OfficeSettings settings = OfficeSettings.getDefault();

        if (settings.getWarnBeforeMount() == false)
            return;

        String message = "The Office Scripting Framework support jar file " +
            "is not mounted, so Office scripts will not compile. NetBeans " +
            "is going to mount this jar file automatically.";

        String prompt = "Show this message in future.";

        NagDialog warning = NagDialog.createInformationDialog(
            message, prompt, true);

        if (warning.getState() == false) {
            settings.setWarnBeforeMount(false);
        }
    }
}
