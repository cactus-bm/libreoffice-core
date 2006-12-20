package org.openoffice.setup.Util;

import java.io.File;
import org.openoffice.setup.InstallData;
import org.openoffice.setup.ResourceManager;
import org.openoffice.setup.SetupData.SetupDataProvider;

public class Controller {

    private Controller() {
    }

    static public void checkPackagePathExistence(InstallData installData) {
        String packagePath = installData.getPackagePath();
        if (( packagePath == null ) || ( packagePath.equals(""))) {
            String message = ResourceManager.getString("String_InstallationOngoing_PackagePath_Not_Found");
            String title = ResourceManager.getString("String_Error");
            Informer.showErrorMessage(message, title);
            System.exit(1);
        }
    }

    static public void checkPackageFormat(InstallData installData) {
        String packageFormat = installData.getPackageFormat();
        String os = installData.getOSType();

        boolean notSupportedPackageFormat = true;

        // Show warnings for currently not supported combinations of OS and package format.
        // This has to be adapted if further OS or package formats are supported.

        if (( os.equalsIgnoreCase("SunOS") ) && ( packageFormat.equalsIgnoreCase("pkg") )) {
            notSupportedPackageFormat = false;
        }

        if (( os.equalsIgnoreCase("Linux") ) && ( packageFormat.equalsIgnoreCase("rpm") )) {
            notSupportedPackageFormat = false;
        }

        // Inform user about not supported package format and exit program

        if ( notSupportedPackageFormat ) {
            System.err.println("Error: Package format not supported by this OS!");
            String mainmessage = ResourceManager.getString("String_Packageformat_Not_Supported");
            String osstring = ResourceManager.getString("String_Operating_System");
            String formatstring = ResourceManager.getString("String_Packageformat");
            String message = mainmessage + "\n" + osstring + ": " + os + "\n" + formatstring + ": " + packageFormat;
            String title = ResourceManager.getString("String_Error");
            Informer.showErrorMessage(message, title);
            System.exit(1);
        }
    }

    static public boolean createdSubDirectory(String dir) {
        boolean createdDirectory = false;
        boolean errorShown = false;
        String subDirName = "testdir";
        File testDir = new File(dir, subDirName);
        try {
            createdDirectory = SystemManager.create_directory(testDir.getPath());
        }
        catch (SecurityException ex) {
            String message = ResourceManager.getString("String_ChooseDirectory_No_Write_Access") + ": " + dir;
            String title = ResourceManager.getString("String_Error");
            Informer.showErrorMessage(message, title);
            errorShown = true;
        }

        if (( ! createdDirectory ) && ( ! errorShown )) {
            String message = ResourceManager.getString("String_ChooseDirectory_No_Write_Access") + ": " + dir;
            String title = ResourceManager.getString("String_Error");
            Informer.showErrorMessage(message, title);
            errorShown = true;
        }

        if ( SystemManager.exists_directory(testDir.getPath()) ) {
            testDir.delete();
        }

        return createdDirectory;
    }

    static public boolean createdDirectory(String dir) {
        boolean createdDirectory = false;
        try {
            createdDirectory = SystemManager.create_directory(dir);
        }
        catch (SecurityException ex) {
            // message = ResourceManager.getString("String_ChooseDirectory_Not_Allowed") + ": " + dir;
            // title = ResourceManager.getString("String_Error");
            // Informer.showErrorMessage(message, title);
        }

        if ( ! createdDirectory ) {
            String message = ResourceManager.getString("String_ChooseDirectory_No_Success") + ": " + dir;
            String title = ResourceManager.getString("String_Error");
            Informer.showErrorMessage(message, title);
        }

        return createdDirectory;
    }

    static public void checkForNewerVersion(InstallData installData) {
        LogManager.setCommandsHeaderLine("Checking change installation");
        InstallChangeCtrl.checkInstallChange(installData);

        if ( installData.newerVersionExists() ) {
            // Inform user about a newer version installed
            SetupDataProvider.setNewMacro("DIR", installData.getInstallDir()); // important for string replacement

            System.err.println("Error: A newer version is already installed in " + installData.getInstallDir() + " !");
            String message1 = ResourceManager.getString("String_Newer_Version_Installed_Found")
                            + "\n" + installData.getInstallDir() + "\n";
            String message2 = ResourceManager.getString("String_Newer_Version_Installed_Remove");
            String message = message1 + "\n" + message2;
            String title = ResourceManager.getString("String_Error");
            Informer.showErrorMessage(message, title);
            System.exit(1);
        }
    }

    static public void checkForUidFile(InstallData installData) {
        // check existence of getuid.so
        File getuidFile = Controller.findUidFile(installData);

        if (( getuidFile == null ) || (! getuidFile.exists()) ) {
            // Root privileges required -> abort installation
            System.err.println("Root privileges required for installation!");
            String message = ResourceManager.getString("String_Root_Privileges_Required_1") + "\n"
                           + ResourceManager.getString("String_Root_Privileges_Required_2");
            String title = ResourceManager.getString("String_Error");
            Informer.showErrorMessage(message, title);
            System.exit(1);
        } else {
            installData.setGetUidPath(getuidFile.getPath());
        }
    }

    static private File findUidFile(InstallData data) {

        File getuidFile = null;

        if ( data.isInstallationMode()) {
            String getuidpath = System.getProperty("GETUID_PATH");

            if ( getuidpath != null ) {
                getuidFile = new File(getuidpath);

                if (( getuidFile.isDirectory() ) && ( ! getuidFile.isFile() )) {
                    // Testing, if GETUID_PATH only contains the path, not the filename
                    String defaultfilename = "getuid.so";
                    getuidFile = new File(getuidpath, defaultfilename);

                    if ( ! getuidFile.exists() ) {
                        getuidFile = null;
                    }
                }
            }

            // File resourceRoot = data.getResourceRoot();
            // String getuidString = "getuid.so";
            // if ( resourceRoot != null ) {
            //     File getuidDir = new File (data.getInfoRoot(), "getuid");
            //     getuidFile = new File(getuidDir, getuidString);
            // }

        } else {
            getuidFile = new File(data.getGetUidPath());
        }

        return getuidFile;
    }

}
