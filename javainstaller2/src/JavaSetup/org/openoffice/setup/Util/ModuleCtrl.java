/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ModuleCtrl.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-03 12:02:36 $
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

package org.openoffice.setup.Util;

import java.io.File;
import java.util.Enumeration;
import org.openoffice.setup.InstallData;
import org.openoffice.setup.Installer.Installer;
import org.openoffice.setup.Installer.InstallerFactory;
import org.openoffice.setup.Panel.ChooseDirectory;
import org.openoffice.setup.ResourceManager;
import org.openoffice.setup.SetupData.PackageDescription;
import org.openoffice.setup.SetupData.SetupDataProvider;
import org.openoffice.setup.Util.Informer;

public class ModuleCtrl {

    private ModuleCtrl() {
    }

    static public void setModuleSize(PackageDescription packageData) {
        // Setting the package size for node modules, that have hidden children
        // -> Java module has three hidden children and 0 byte size

        if ( ! packageData.isLeaf() ) {
            boolean setNewSize = false;
            int size = packageData.getSize();

            for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
                PackageDescription child = (PackageDescription) e.nextElement();
                // if (( child.isHidden() ) && ( child.getSelectionState() == PackageDescription.DONT_KNOW )) {
                if ( child.isHidden() ) {
                    setNewSize = true;
                    size = size + child.getSize();
                }
            }

            if ( setNewSize ) {
                packageData.setSize(size);
                // System.err.println("Setting size " + size + " for node module: " + packageData.getName());
            }
        }

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            setModuleSize(child);
        }

    }

    static public void setDefaultModuleSettings(PackageDescription data) {
        // Setting default module settings for modules, that are not hidden
        // Hidden modules do not get a defined state now
        boolean isInstalled = false;
        InstallData installdata = InstallData.getInstance();
        boolean isUninstall = installdata.isUninstallationMode();

        if (isUninstall) {
            isInstalled = true;
        }

        if (isUninstall) {
            if (isInstalled) {
                data.setSelectionState(PackageDescription.REMOVE);
            } else {
                data.setSelectionState(PackageDescription.IGNORE);
                System.err.println("NEVER");
            }
        } else {
            if (isInstalled) {
                data.setSelectionState(PackageDescription.IGNORE);
                System.err.println("NEVER");
            } else if (data.isDefault()) {
                data.setSelectionState(PackageDescription.INSTALL);
            } else if ( ! data.isDefault()) {
                data.setSelectionState(PackageDescription.DONT_INSTALL);
            } else {
                data.setSelectionState(PackageDescription.DONT_INSTALL);
            }
        }
    }

    static public void setParentDefaultModuleSettings(PackageDescription packageData) {
        // Setting the module states of parent modules.
        // Called after ChooseDirectoryCtrl.java, because
        // the database has to be known. In user installation it is important,
        // that the installation directory is known, to find the database.
        // Called during uninstallation in UninstallationPrologueCtrl.java

        // Iteration before setting the module states. Because of this, all children
        // get their final setting before the parent.

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            setParentDefaultModuleSettings(child);
        }

        if ( ! packageData.isLeaf() ) {
            // System.err.println("setParentDefaultModuleSettings: " + packageData.getName());
            int state = packageData.getSelectionState();
            InstallData installdata = InstallData.getInstance();
            boolean allChildrenIgnored = true;
            boolean atLeastOneInstalled = false;
            boolean allChildrenHidden = true;

            // System.err.println("    STATE before iterating over children: " + state);

            for (Enumeration e = packageData.children(); e.hasMoreElements();) {
                PackageDescription child = (PackageDescription) e.nextElement();
                int childState = child.getSelectionState();

                // System.err.println("    Child: " + child.getName() + " : " + childState);

                if ( childState != PackageDescription.IGNORE) {
                    allChildrenIgnored = false;
                }

                if (( childState == PackageDescription.INSTALL) || ( childState == PackageDescription.INSTALL_SOME)) {
                    atLeastOneInstalled = true;
                }

                if ( ! child.isHidden() ) {
                    allChildrenHidden = false;
                }

                if ((state == PackageDescription.DONT_KNOW) || (state == PackageDescription.IGNORE)) {
                    state = childState;
                // } else if ((state != childState) && (childState != PackageDescription.IGNORE)) {
                } else if ((state != childState) && (childState != PackageDescription.IGNORE) && (childState != PackageDescription.DONT_KNOW)) {
                    if ( installdata.isUninstallationMode() ) {
                        state = PackageDescription.REMOVE_SOME;
                    } else {
                        state = PackageDescription.INSTALL_SOME;
                    }
                }

                // System.err.println("    NEW state after child: " + state);
            }

            if ( allChildrenIgnored ) {
                state = PackageDescription.IGNORE;
            }

            if ( installdata.isInstallationMode() ) {
                if (( state == PackageDescription.INSTALL_SOME ) && ( ! atLeastOneInstalled )) {
                    state = PackageDescription.DONT_INSTALL;
                }
            }

            if ( allChildrenHidden ) {
                packageData.setAllChildrenHidden(true);
                // System.err.println("Setting allChildrenHidden for module " + packageData.getName() );
            }

            // System.err.println("Setting " + packageData.getName() + " to " + packageData.getSelectionState() );
            packageData.setSelectionState(state);
        }

    }

    static public void setHiddenModuleSettingsInstall(PackageDescription packageData) {
        // update selection states for hidden modules during installation
        if (( packageData.isHidden() ) && ( packageData.getSelectionState() != packageData.IGNORE )) {
            PackageDescription parent = (PackageDescription)packageData.getParent();
            if ( parent != null ) {
                packageData.setSelectionState(parent.getSelectionState());
                // hidden modules at root module always have to be installed, if they are not already installed
                if ( parent.getName() == "" ) {
                    packageData.setSelectionState(packageData.INSTALL);
                    // System.err.println("Setting 1 INSTALL flag to: " + packageData.getName());
                }
            }

            // INSTALL_SOME is not valid for leaves
            if (( packageData.getSelectionState() == packageData.INSTALL_SOME ) && ( packageData.isLeaf() )) {
                packageData.setSelectionState(packageData.INSTALL);
                // System.err.println("Setting 2 INSTALL flag to: " + packageData.getName());
            }
        }

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            setHiddenModuleSettingsInstall(child);
        }
    }

    static public void setHiddenModuleSettingsUninstall(PackageDescription packageData) {
        InstallData data = InstallData.getInstance();
        // update selection states for hidden modules during uninstallation
        if (( packageData.isHidden() ) && ( packageData.getSelectionState() != packageData.IGNORE )) {
            // System.err.println("Package name: " + packageData.getName());
            // System.err.println("Selection: " + packageData.getSelectionState());

            PackageDescription parent = (PackageDescription)packageData.getParent();
            if ( parent != null ) {
                packageData.setSelectionState(parent.getSelectionState());
                // Hidden modules at root module have to be uninstalled at complete uninstallation
                // In Uninstallation the complete is the typical installation type
                if (( parent.getName() == "" ) && ( data.isTypicalInstallation() ))  {
                    packageData.setSelectionState(packageData.REMOVE);
                }
                // Hidden modules at root module must not be uninstalled at custom uninstallation
                // But if all visible modules are selected for uninstallation, this shall be handled
                // as complete uninstallation.
                if ( ! data.isMaskedCompleteUninstallation() )
                {
                    if (( parent.getName() == "" ) && ( data.isCustomInstallation() ))  {
                        packageData.setSelectionState(packageData.IGNORE);
                    }
                }
            }

            // REMOVE_SOME is not valid for leaves
            // if ( data.isTypicalInstallation() ) {
            if (( packageData.getSelectionState() == packageData.REMOVE_SOME ) && ( packageData.isLeaf() )) {
                packageData.setSelectionState(packageData.REMOVE);
            }
            // }

        }

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            setHiddenModuleSettingsUninstall(child);
        }
    }

    static private boolean checkRequiredCoreModule(PackageDescription packageData) {

        // This function uses a similar mechanism to determine
        // core modules as function "setHiddenModuleSettingsInstall"
        // -> only hidden parents, until there is a module without name (getName)
        // Only searching until grandpa.

        boolean requiredCoreModule = false;

        // if packageData.getSelectionState() DONT_KNOW  && parent auch DONT_KNOW
        if (( packageData.isHidden() ) &&
                ( packageData.getSelectionState() != packageData.IGNORE ) &&
                ( packageData.getPackageName() != null )) {
                //( packageData.isLeaf() )) {
            PackageDescription parent = (PackageDescription)packageData.getParent();
            if ( parent != null ) {
                if (( parent.getName().equals("") ) || ( parent.getName() == null )) {
                    requiredCoreModule = true;
                } else {
                    if ( parent.isHidden() ) {
                        PackageDescription grandpa = (PackageDescription)parent.getParent();
                        if ( grandpa != null ) {
                            if (( grandpa.getName().equals("") ) || ( grandpa.getName() == null )) {
                                requiredCoreModule = true;
                            }
                        }
                    }
                }
            }
        }

        return requiredCoreModule;
    }

    static public void setDatabaseSettings(PackageDescription packageData, InstallData installData, Installer installer) {
        // Analyzing the system database and setting the module states.
        // Called during installation in ChooseInstallationTypeCtrl.java, because
        // the database has to be known. In user installation it is important,
        // the the installation directory is known, to find the database.
        // Called during uninstallation in UninstallationPrologueCtrl.java

        boolean isUninstall = installData.isUninstallationMode();
        boolean isInstalled = installer.isPackageInstalled(packageData, installData);

        if (isUninstall) {
            if (isInstalled) {
                packageData.setSelectionState(PackageDescription.REMOVE);
                // The following is no longer required !? (IS, 06/05/08)
                // PackageDescription parent = packageData.getParent();
                // if ( parent != null ) {
                //     if ( parent.getSelectionState() != PackageDescription.REMOVE ) {
                //         parent.setSelectionState(PackageDescription.REMOVE);
                //         System.err.println("Setting remove to " +  parent.getName());
                //     }
                // }
            } else {
                // Attention: Setting all nodes to ignore! If a children gets REMOVE,
                // then REMOVE is also set to the parent. Setting REMOVE happens after
                // setting IGNORE, because children are evaluated after the parents.
                // The default for uninstallation is set in setDefaultModuleSettings to REMOVE.
                packageData.setSelectionState(PackageDescription.IGNORE);
            }
        } else {
            if ( isInstalled ) {
                boolean installedPackageIsOlder = installer.isInstalledPackageOlder(packageData, installData);
                if ( ! installedPackageIsOlder ) {
                    // The package is already installed in the same or in a newer version
                    packageData.setSelectionState(PackageDescription.IGNORE);
                } else {
                    // This is also something like migrating feature states
                    packageData.setSelectionState(PackageDescription.INSTALL);
                }
            }
            else {
                // Special handling for core modules, which are required, but not installed.
                // This can be deinstalled by hand for example.
                boolean isRequiredCoreModule = checkRequiredCoreModule(packageData);
                if ( isRequiredCoreModule ) {
                    packageData.setSelectionState(PackageDescription.INSTALL);
                    LogManager.addLogfileComment("<b>Adding required package:</b> " + packageData.getPackageName() + "</br>");
                    // This package has to exist!
                    if ( ! packageExists(packageData, installData) ) {

                        String packagePath = installData.getPackagePath();
                        if (( packageData.getPkgSubdir() != null ) && ( ! packageData.getPkgSubdir().equals("") )) {
                            File completePackageFile = new File(packagePath, packageData.getPkgSubdir());
                            packagePath = completePackageFile.getPath();
                        }
                        String packageName = packageData.getPackageName();
                        File packageFile = new File(packagePath, packageName);

                        String log = "<b>Error: Missing required package " + packageFile.getPath() + "</b><br>";
                        System.err.println(log);
                        String message = ResourceManager.getString("String_File_Not_Found") + ": " + packageFile.getPath();
                        String title = ResourceManager.getString("String_Error");
                        Informer.showErrorMessage(message, title);
                        System.exit(1);
                    }
                }
            }
        }

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            setDatabaseSettings(child, installData, installer);
        }
    }

    static public void setShowInUserInstallFlags(PackageDescription packageData) {

        // This function is not needed during deinstallation, because a
        // module that could not be selected during installation, is always
        // not installed during deinstallation and therefore gets "IGNORE"
        // in function setDatabaseSettings

        if ( ! packageData.showInUserInstall() ) {
            packageData.setSelectionState(PackageDescription.IGNORE);
            // too late to hide the module
            // packageData.setIsHidden(true);
            // packageData.setAllChildrenHidden(true);
        }

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            setShowInUserInstallFlags(child);
        }
    }

    static private boolean packageExists(PackageDescription packageData, InstallData installData) {
        boolean fileExists = false;
        String packagePath = installData.getPackagePath();

        if (( packageData.getPkgSubdir() != null ) && ( ! packageData.getPkgSubdir().equals("") )) {
            File completePackageFile = new File(packagePath, packageData.getPkgSubdir());
            packagePath = completePackageFile.getPath();
        }

        String packageName = packageData.getPackageName();
        File packageFile = new File(packagePath, packageName);

        if ( packageFile.exists() ) {
            fileExists = true;
        }

        return fileExists;
    }

    static public void disableNonExistingPackages(PackageDescription packageData, InstallData installData) {
        if ((( packageData.getPackageName() == null ) || ( packageData.getPackageName().equals("") ))
               && packageData.isLeaf() ) {
            packageData.setSelectionState(PackageDescription.IGNORE);
        } else if ( ! packageExists(packageData, installData) ) {
            packageData.setSelectionState(PackageDescription.IGNORE);
        }

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            disableNonExistingPackages(child, installData);
        }
    }

    static public void setDontUninstallFlags(PackageDescription packageData) {
        if ( packageData.dontUninstall() ) {
            packageData.setSelectionState(PackageDescription.IGNORE);
        }

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            setDontUninstallFlags(child);
        }

    }

    static public void checkVisibleModulesInstall(PackageDescription packageData, InstallData data) {
        boolean setToTrue = false;

        if (( ! packageData.isHidden() ) && ( packageData.getSelectionState() == packageData.INSTALL )) {
            setToTrue = true;
            data.setVisibleModulesChecked(true);
        }

        if ( ! setToTrue ) {
            for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
                PackageDescription child = (PackageDescription) e.nextElement();
                checkVisibleModulesInstall(child, data);
            }
        }
    }

    static public void checkVisibleModulesUninstall(PackageDescription packageData, InstallData data) {
        boolean setToTrue = false;

        if (( ! packageData.isHidden() ) && ( packageData.getSelectionState() == packageData.REMOVE )) {
            // ignoring the top level module, that has the state REMOVE (but no name)
            if (( packageData.getName() != null ) && ( ! packageData.getName().equals("") )) {
                setToTrue = true;
                data.setVisibleModulesChecked(true);
            }
        }

        if ( ! setToTrue ) {
            for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
                PackageDescription child = (PackageDescription) e.nextElement();
                checkVisibleModulesUninstall(child, data);
            }
        }
    }

    static public void checkMaskedCompleteUninstallation(PackageDescription packageData, InstallData data) {
        boolean setToFalse = false;

        // If there is at least one visible module, that is not selected for removal
        // this is no masked complete uninstallation

        if (( ! packageData.isHidden() )
                 && ( packageData.getSelectionState() != packageData.REMOVE )
                 && ( packageData.getSelectionState() != packageData.IGNORE )) {
            // ignoring the top level module, that has no name
            if (( packageData.getName() != null ) && ( ! packageData.getName().equals("") )) {
                setToFalse = true;
                data.setMaskedCompleteUninstallation(false);
                // System.err.println("This is no masked complete uninstallation!");
                // System.err.println("Caused by: " + packageData.getName() + " with " + packageData.getSelectionState());
            }
        }

        if ( ! setToFalse ) {
            for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
                PackageDescription child = (PackageDescription) e.nextElement();
                checkMaskedCompleteUninstallation(child, data);
            }
        }
    }

    static public void saveTypicalSelectionStates(PackageDescription packageData) {
        packageData.setTypicalSelectionState(packageData.getSelectionState());

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            saveTypicalSelectionStates(child);
        }
    }

    static public void saveCustomSelectionStates(PackageDescription packageData) {
        packageData.setCustomSelectionState(packageData.getSelectionState());

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            saveCustomSelectionStates(child);
        }
    }

    static public void saveStartSelectionStates(PackageDescription packageData) {
        packageData.setStartSelectionState(packageData.getSelectionState());

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            saveStartSelectionStates(child);
        }
    }

    static public void restoreTypicalSelectionStates(PackageDescription packageData) {
        packageData.setSelectionState(packageData.getTypicalSelectionState());

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            restoreTypicalSelectionStates(child);
        }
    }

    static public void restoreCustomSelectionStates(PackageDescription packageData) {
        packageData.setSelectionState(packageData.getCustomSelectionState());

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            restoreCustomSelectionStates(child);
        }
    }

    static public void restoreStartSelectionStates(PackageDescription packageData) {
        packageData.setSelectionState(packageData.getStartSelectionState());

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            restoreStartSelectionStates(child);
        }
    }

    static public void setUpdateOlderProductSettings(PackageDescription packageData, InstallData data, Installer installer) {
        if (( packageData.getPackageName() != null ) && ( ! packageData.getPackageName().equals(""))) {
            if ( installer.isPackageInstalled(packageData, data) ) {
                packageData.setSelectionState(PackageDescription.INSTALL);

                // Special handling for jre package, because this is not necessarily older, if an older product is updated.
                if ( packageData.isJavaPackage() ) {
                    if ( ! installer.isInstalledPackageOlder(packageData, data) ) {
                        packageData.setSelectionState(PackageDescription.DONT_INSTALL);
                    }
                }
            } else {
                packageData.setSelectionState(PackageDescription.DONT_INSTALL);
            }
        }

        for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
            PackageDescription child = (PackageDescription) e.nextElement();
            setUpdateOlderProductSettings(child, data, installer);
        }
    }

}
