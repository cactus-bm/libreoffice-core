/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ChooseComponentsCtrl.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-03 11:50:18 $
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

package org.openoffice.setup.Controller;

import java.util.Vector;
import org.openoffice.setup.InstallData;
import org.openoffice.setup.PanelController;
import org.openoffice.setup.Panel.ChooseComponents;
import org.openoffice.setup.ResourceManager;
import org.openoffice.setup.SetupData.PackageDescription;
import org.openoffice.setup.SetupData.SetupDataProvider;
import org.openoffice.setup.Util.Calculator;
import org.openoffice.setup.Util.Dumper;
import org.openoffice.setup.Util.Informer;
import org.openoffice.setup.Util.ModuleCtrl;
import org.openoffice.setup.Util.PackageCollector;

public class ChooseComponentsCtrl extends PanelController {

    private String helpFile;

    public ChooseComponentsCtrl() {
        super("ChooseComponents", new ChooseComponents());
        helpFile = "String_Helpfile_ChooseComponents";
    }

    public String getNext() {
        return new String("InstallationImminent");
    }

    public String getPrevious() {

        InstallData data = InstallData.getInstance();

        if ( data.sameVersionExists() ) {
            return new String("ChooseDirectory");
        } else {
            return new String("ChooseInstallationType");
        }

    }

    public final String getHelpFileName () {
        return this.helpFile;
    }

    public void beforeShow() {

        InstallData data = InstallData.getInstance();

        // Setting the package size for node modules, that have hidden children
        // -> Java module has three hidden children and 0 byte size

        if ( ! data.moduleSizeSet() ) {
            PackageDescription packageData = SetupDataProvider.getPackageDescription();
            ModuleCtrl.setModuleSize(packageData);
            data.setModuleSizeSet(true);
        }

        if ( data.sameVersionExists() ) {
            ChooseComponents panel = (ChooseComponents)getPanel();
            String dialogTitle = ResourceManager.getString("String_ChooseComponents1_Maintain");
            panel.setTitleText(dialogTitle);
        }

    }

    public boolean afterShow(boolean nextButtonPressed) {
        boolean repeatDialog = false;

        InstallData data = InstallData.getInstance();
        PackageDescription packageData = SetupDataProvider.getPackageDescription();

        if ( nextButtonPressed ) {

            // Check, if at least one visible module was selected for installation
            data.setVisibleModulesChecked(false);
            ModuleCtrl.checkVisibleModulesInstall(packageData, data);

            if ( data.visibleModulesChecked() ) {
                // Set module settings for hidden modules.
                // Then it is possible to calculate the size of the installed product,
                // to show a warning and to set the repeatDialog value to true

                if ( data.logModuleStates() ) {
                    Dumper.logModuleStates(packageData, "ChooseComponentsCtrl: Before setHiddenModuleSettingsInstall");
                }

                ModuleCtrl.setHiddenModuleSettingsInstall(packageData);
                // Dumper.dumpInstallPackages(packageData);

                if ( data.logModuleStates() ) {
                    Dumper.logModuleStates(packageData, "ChooseComponentsCtrl: After setHiddenModuleSettingsInstall");
                }

                // Collecting packages to install
                Vector installPackages = new Vector();
                PackageCollector.collectInstallPackages(packageData, installPackages);
                data.setInstallPackages(installPackages);

                // Check disc space
                if ( Calculator.notEnoughDiscSpace(data) ) {
                    repeatDialog = true;
                }
            } else {  // no modules selected for installation
                String message = ResourceManager.getString("String_No_Components_Selected_1") + "\n" +
                                 ResourceManager.getString("String_No_Components_Selected_2");
                String title = ResourceManager.getString("String_Nothing_To_Install");
                Informer.showInfoMessage(message, title);
                repeatDialog = true;
            }
        } else {  // the back button was pressed
            // Saving typical selection state values (always if back button is pressed!).
            // System.err.println("Saving custom selection states");
            ModuleCtrl.saveCustomSelectionStates(packageData);
            data.setCustomSelectionStateSaved(true);
        }

        return repeatDialog;
    }

}
