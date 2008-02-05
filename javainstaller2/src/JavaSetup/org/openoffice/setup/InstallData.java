/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: InstallData.java,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: ihi $ $Date: 2008-02-05 13:36:14 $
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

package org.openoffice.setup;

import org.openoffice.setup.SetupData.PackageDescription;
import org.openoffice.setup.Util.Controller;
import org.openoffice.setup.Util.SystemManager;
import java.io.File;
import java.util.HashMap;
import java.util.Vector;

public class InstallData
{
    public static final String ACTION_TYPICAL   = "ActionTypical";
    public static final String ACTION_CUSTOM    = "ActionCustom";

    private static InstallData instance = null;

    static private boolean isUserInstallation;    /* root or user installation? */
    static private boolean isRootInstallation;    /* root or user installation? */
    static private boolean isInstallationMode;    /* installation or uninstallation? */
    static private boolean isUninstallationMode;  /* installation or uninstallation? */
    static private boolean isCustomInstallation = false;  /* custom or typical? */
    static private boolean isTypicalInstallation = true;  /* custom or typical? */
    static private boolean isSolarisUserInstallation = false;
    static private boolean isChangeInstallation = false;
    static private boolean stillRunning = false;
    static private boolean stillAnalyzing = true;
    static private boolean databaseAnalyzed = false;  /* the database was already analyzed? */
    static private boolean moduleSizeSet = false;  /* the database was already analyzed? */
    static private boolean preInstallDone = false;  /* preInstall script already executed? */
    static private boolean isAbortedInstallation = false;
    static private boolean isErrorInstallation = false;
    static private boolean logModuleStates = false;  /* logging the current state of modules */
    static private boolean visibleModulesChecked = false; /* checking, if the user selected modules */
    static private boolean languageModulesChecked = false; /* checking, if the user selected language modules */
    static private boolean applicationModulesChecked = false; /* checking, if the user selected application modules */
    static private boolean isMaskedCompleteUninstallation = false; /* checking if all visible modules are uninstalled */
    static private boolean typicalSelectionStateSaved = false;
    static private boolean customSelectionStateSaved = false;
    static private boolean startSelectionStateSaved = false;
    static private boolean olderVersionExists = false;
    static private boolean sameVersionExists = false;
    static private boolean newerVersionExists = false;
    static private boolean isMultiLingual = false;
    static private boolean dontUpdate = false;
    static private boolean databaseQueried = false;
    static private String installType;            /* custom or typical installation */
    static private String osType;                 /* Linux, SunOS, ...              */
    static private String installDir = null;
    static private String installRoot = null;     /* Root directory for Solaris user installation */
    static private String defaultDir = "/opt/OpenOffice.org";
    static private String packageFormat = null;
    static private String architecture = null;
    static private String packagePath = null;
    static private String packageSubdir = "packages";
    static private String adminFileNameReloc = null;
    static private String adminFileNameNoReloc = null;
    static private String databasePath = null;
    static private String getUidPath = null;
    static private String installationPrivileges = null;
    static private String storedInstallationPrivileges = null; /* privileges saved in file */
    static private String localTempPath = null;
    static private String installDirName = "installdata";
    static private String uninstallDirName = "uninstalldata";
    static private int availableDiscSpace = 0;
    static private File jarFilePath = null;
    static private File resourceRoot;
    static private File infoRoot;
    static private HashMap shellEnvironment = null;   /* Solaris environment for user install */
    static private HashMap databaseMap = null;
    static private PackageDescription updatePackage = null;
    static private Vector removeFiles = new Vector();  /* Files to remove, if installation is aborted */
    static private Vector installPackages = new Vector();
    static private Vector systemLanguages = new Vector();

    public static InstallData getInstance()
    {
      if (instance == null) {
        instance = new InstallData();
      }
      return instance;
    }

    private InstallData()
    {
        installType = ACTION_TYPICAL;  // default installation type
        isUserInstallation = SystemManager.isUserInstallation();
        isRootInstallation = !isUserInstallation;
        setInstallationPrivileges(isUserInstallation);
        osType = SystemManager.getOSType();
        resourceRoot = SystemManager.getResourceRoot();
        setInstallationMode();
        setSolarisUserInstall();
        setHtmlFileExistence();
    }

    public void setInstallationType(String installationtype) {
        installType = installationtype;

        if ( installType.equals(this.getCustomActionCommand())) {
            isCustomInstallation = true;
            isTypicalInstallation = false;
        }

        if ( installType.equals(this.getTypicalActionCommand())) {
            isCustomInstallation = false;
            isTypicalInstallation = true;
        }
    }

    private void setInstallationMode() {
        // Exists a directory "uninstalldata" below the resource root?
        File uninstallDir = new File(resourceRoot, uninstallDirName);
        File installDir = new File(resourceRoot, installDirName);

        if ( SystemManager.exists_directory(uninstallDir.getPath())) {
            isInstallationMode = false;
            isUninstallationMode = true;
            infoRoot = uninstallDir;
            System.err.println("Mode: uninstallation");
        } else if ( SystemManager.exists_directory(installDir.getPath())) {
            isInstallationMode = true;
            isUninstallationMode = false;
            infoRoot = installDir;
            System.err.println("Mode: installation");
        } else {
            // isInstallationMode = null;
            // isUninstallationMode = null;
            infoRoot = null;
            System.err.println("Error: Did not find info path");
            System.err.println("Error: No info about installation or uninstallation");
            System.exit(1);
        }
    }

    private void setSolarisUserInstall() {
        if (( isUserInstallation ) && (osType.equalsIgnoreCase("SunOS"))) {
            isSolarisUserInstallation = true;
            if ( isInstallationMode ) {
                Controller.checkForUidFile(this);
            }
        }
    }

    private void setHtmlFileExistence() {
        // After inforoot is determined, the existence of files in subdirectory "html" can be checked
        File htmlDirectory = getInfoRoot("html");
        ResourceManager.checkFileExistence(htmlDirectory);
    }

    private void setInstallationPrivileges(boolean isUserInstallation) {
        if ( isUserInstallation ) {
            installationPrivileges = "user";
        } else {
            installationPrivileges = "root";
        }
    }

    public String getInstallationType() {
        return installType;
    }

    public String getCustomActionCommand() {
        return ACTION_CUSTOM;
    }

    public String getTypicalActionCommand() {
        return ACTION_TYPICAL;
    }

    public String getInstallationPrivileges() {
        return installationPrivileges;
    }

    public String getOSType() {
        return osType;
    }

    public File getResourceRoot() {
        return resourceRoot;
    }

    public File getResourceRoot(String subDirectory) {

        File dir = getResourceRoot();

        if (dir != null) {
            dir = new File(dir, subDirectory);
            if (!dir.exists()) {
                dir = null;
            }
        }

        return dir;
    }

    public File getInfoRoot() {
        return infoRoot;
    }

    public File getInfoRoot(String subDirectory) {
        File dir = new File(infoRoot, subDirectory);
        if (!dir.exists()) {
            dir = null;
        }

        return dir;
    }

    public boolean isUserInstallation() {
        return isUserInstallation;
    }

    public boolean isRootInstallation() {
        return isRootInstallation;
    }

    public boolean isInstallationMode() {
        return isInstallationMode;
    }

    public boolean isUninstallationMode() {
        return isUninstallationMode;
    }

    public boolean isSolarisUserInstallation() {
        return isSolarisUserInstallation;
    }

    public String getDefaultDir() {
        return defaultDir;
    }

    public void setDefaultDir(String dir) {
        defaultDir = dir;
    }

    public String getInstallDirName() {
        return installDirName;
    }

    public String getUninstallDirName() {
        return uninstallDirName;
    }

    public String getInstallDir() {
        return installDir;
    }

    public void setInstallDir(String dir) {
        installDir = dir;
    }

    public String getInstallRoot() {
        return installRoot;
    }

    public void setInstallRoot(String dir) {
        installRoot = dir;
    }

    public String getDatabasePath() {
        return databasePath;
    }

    public void setDatabasePath(String path) {
        databasePath = path;
    }

    public String getPackagePath() {
        if ( packagePath == null ) {
            packagePath = SystemManager.getPackagePath(packageSubdir);
        }
        return packagePath;
    }

    public void setPackagePath(String path) {
        packagePath = path;
    }

    public String getPackageSubdir() {
        return packageSubdir;
    }

    public void setPackageSubdir(String dir) {
        packageSubdir = dir;
    }

    public String getPackageFormat() {
        return packageFormat;
    }

    public void setPackageFormat(String format) {
        packageFormat = format;
    }

    public String getArchitecture() {
        return architecture;
    }

    public void setArchitecture(String arch) {
        architecture = arch;
    }

    public String getLocalTempPath() {
        return localTempPath;
    }

    public void setLocalTempPath(String path) {
        localTempPath = path;
    }

    public int getAvailableDiscSpace() {
        return availableDiscSpace;
    }

    public void setAvailableDiscSpace(int space) {
        availableDiscSpace = space;
    }

    public String getAdminFileNameReloc() {
        return adminFileNameReloc;
    }

    public void setAdminFileNameReloc(String fileName) {
        adminFileNameReloc = fileName;
    }

    public String getAdminFileNameNoReloc() {
        return adminFileNameNoReloc;
    }

    public void setAdminFileNameNoReloc(String fileName) {
        adminFileNameNoReloc = fileName;
    }

    public String getGetUidPath() {
        return getUidPath;
    }

    public void setGetUidPath(String filePath) {
        getUidPath = filePath;
    }

    public String getStoredInstallationPrivileges() {
        return storedInstallationPrivileges;
    }

    public void setStoredInstallationPrivileges(String privileges) {
        storedInstallationPrivileges = privileges;
    }

    public void setStillRunning(boolean running) {
        stillRunning = running;
    }

    public boolean stillRunning() {
        return stillRunning;
    }

    public void setStillAnalyzing(boolean running) {
        stillAnalyzing = running;
    }

    public boolean stillAnalyzing() {
        return stillAnalyzing;
    }

    public void setDatabaseAnalyzed(boolean analyzed) {
        databaseAnalyzed = analyzed;
    }

    public boolean databaseAnalyzed() {
        return databaseAnalyzed;
    }

    public void setModuleSizeSet(boolean set) {
        moduleSizeSet = set;
    }

    public boolean moduleSizeSet() {
        return moduleSizeSet;
    }

    public void setPreInstallDone(boolean done) {
        preInstallDone = done;
    }

    public boolean preInstallDone() {
        return preInstallDone;
    }

    public boolean isChangeInstallation() {
        return isChangeInstallation;
    }

    public void setIsChangeInstallation(boolean changeInstallation) {
        isChangeInstallation = changeInstallation;
    }

    public boolean isTypicalInstallation() {
        return isTypicalInstallation;
    }

    public boolean isCustomInstallation() {
        return isCustomInstallation;
    }

    public boolean isAbortedInstallation() {
        return isAbortedInstallation;
    }

    public void setIsAbortedInstallation(boolean abortedInstallation) {
        isAbortedInstallation = abortedInstallation;
    }

    public boolean isErrorInstallation() {
        return isErrorInstallation;
    }

    public void setIsErrorInstallation(boolean errorInstallation) {
        isErrorInstallation = errorInstallation;
    }

    public boolean isMultiLingual() {
        return isMultiLingual;
    }

    public void setIsMultiLingual(boolean multiLingual) {
        isMultiLingual = multiLingual;
    }

    public boolean logModuleStates() {
        return logModuleStates;
    }

    public void setLogModuleStates(boolean log) {
        logModuleStates = log;
    }

    public boolean visibleModulesChecked() {
        return visibleModulesChecked;
    }

    public void setVisibleModulesChecked(boolean checked) {
        visibleModulesChecked = checked;
    }

    public boolean languageModulesChecked() {
        return languageModulesChecked;
    }

    public void setLanguageModulesChecked(boolean checked) {
        languageModulesChecked = checked;
    }

    public boolean applicationModulesChecked() {
        return applicationModulesChecked;
    }

    public void setApplicationModulesChecked(boolean checked) {
        applicationModulesChecked = checked;
    }

    public boolean isMaskedCompleteUninstallation() {
        return isMaskedCompleteUninstallation;
    }

    public void setMaskedCompleteUninstallation(boolean masked) {
        isMaskedCompleteUninstallation = masked;
    }

    public boolean typicalSelectionStateSaved() {
        return typicalSelectionStateSaved;
    }

    public void setTypicalSelectionStateSaved(boolean saved) {
        typicalSelectionStateSaved = saved;
    }

    public boolean customSelectionStateSaved() {
        return customSelectionStateSaved;
    }

    public void setCustomSelectionStateSaved(boolean saved) {
        customSelectionStateSaved = saved;
    }

    public boolean startSelectionStateSaved() {
        return startSelectionStateSaved;
    }

    public void setStartSelectionStateSaved(boolean saved) {
        startSelectionStateSaved = saved;
    }

    public boolean olderVersionExists() {
        return olderVersionExists;
    }

    public void setOlderVersionExists(boolean exists) {
        olderVersionExists = exists;
    }

    public boolean sameVersionExists() {
        return sameVersionExists;
    }

    public void setSameVersionExists(boolean exists) {
        sameVersionExists = exists;
    }

    public boolean newerVersionExists() {
        return newerVersionExists;
    }

    public void setNewerVersionExists(boolean exists) {
        newerVersionExists = exists;
    }

    public boolean dontUpdate() {
        return dontUpdate;
    }

    public void setDontUpdate(boolean value) {
        dontUpdate = value;
    }

    public boolean databaseQueried() {
        return databaseQueried;
    }

    public void setDatabaseQueried(boolean value) {
        databaseQueried = value;
    }

    public PackageDescription getUpdatePackage() {
        return updatePackage;
    }

    public void setUpdatePackage(PackageDescription onePackage) {
        updatePackage = onePackage;
    }

    public HashMap getShellEnvironment() {
        return shellEnvironment;
    }

    public HashMap getDatabaseMap() {
        return databaseMap;
    }

    public void setDatabaseMap(HashMap map) {
        databaseMap = map;
    }

    public Vector getRemoveFiles() {
        return removeFiles;
    }

    public Vector getInstallPackages() {
        return installPackages;
    }

    public void setInstallPackages(Vector packages) {
        installPackages = packages;
    }

    public Vector getSystemLanguages() {
        return systemLanguages;
    }

    public void setSystemLanguages(Vector languages) {
        systemLanguages = languages;
    }

    public void setShellEnvironment(HashMap environment) {
        shellEnvironment = environment;
    }

    public File getJarFilePath() {
        if ( jarFilePath == null ) {
            jarFilePath = SystemManager.getJarFilePath();
        }
        return jarFilePath;
    }

}
