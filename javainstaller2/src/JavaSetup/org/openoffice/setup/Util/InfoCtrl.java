/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: InfoCtrl.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-03 12:01:32 $
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

import org.openoffice.setup.InstallData;
import org.openoffice.setup.SetupData.PackageDescription;
import org.openoffice.setup.SetupData.ProductDescription;
import java.util.Enumeration;
import java.util.Vector;

public class InfoCtrl {

    private InfoCtrl() {
    }

    static public String setHtmlFrame(String position, String htmlInfoText) {
        if ( position.equals("header") ) {
            htmlInfoText = "<HTML><BODY><FONT FACE=\"sans-serif\" SIZE=3>";
        }
        else if ( position.equals("end")) {
            htmlInfoText = htmlInfoText + "</FONT></BODY></HTML>";
        }

        return htmlInfoText;
    }

    static public String setReadyToInstallInfoText(ProductDescription productData, String htmlInfoText) {
        // String oneline = "-------------------------------------------------------------------";
        String oneline = "-------------------------------------------------------------";
        htmlInfoText = htmlInfoText + "<b>Product</b>: " + productData.get("product_fullname") + "<br>";
        InstallData data = InstallData.getInstance();
        htmlInfoText = htmlInfoText + "<b>Location</b>: " + data.getInstallDir() + "<br>";
        if (( data.getInstallRoot() != null ) && ( ! data.getInstallRoot().equals("null") )) {
            htmlInfoText = htmlInfoText + "<b>Root directory</b>: " + data.getInstallRoot() + "<br>";
        }
        htmlInfoText = htmlInfoText + oneline + "<br>";

        return htmlInfoText;
    }

    static public String setReadyToInstallInfoText(PackageDescription packageData, String htmlInfoText) {
        // setHtmlInfoText(packageData, 0);
        InstallData data = InstallData.getInstance();
        if ( data.isInstallationMode() ) {
            htmlInfoText = setReadyToInstallInfoText(packageData, "", htmlInfoText);
        } else {
            htmlInfoText = setReadyToUninstallInfoText(packageData, "", htmlInfoText);
        }
        return htmlInfoText;
    }

    // private void setReadyToInstallInfoText(PackageDescription packageData, Integer indent) {
    static private String setReadyToInstallInfoText(PackageDescription packageData, String indent, String htmlInfoText) {
        // String spacer = "<spacer type=horizontal size=" + indent.toString() + ">";
        // System.out.println(ind);
        if (( packageData.isLeaf() ) || ( packageData.isAllChildrenHidden() )) {
            if ( ! packageData.isHidden() ) {
                if ( packageData.getSelectionState() == packageData.INSTALL ) {
                    // htmlInfoText = htmlInfoText + spacer + packageData.getName() + "<br>";
                    htmlInfoText = htmlInfoText + indent + packageData.getName() + "<br>";
                }
            }
        }

        if (( ! packageData.isLeaf() ) && ( ! packageData.isAllChildrenHidden() )) {
            if ( ! packageData.isHidden() ) {
                if (( packageData.getSelectionState() == packageData.INSTALL ) ||
                    ( packageData.getSelectionState() == packageData.INSTALL_SOME )) {
                    // htmlInfoText = htmlInfoText + spacer + "<b>" + packageData.getName() + "</b>" + "<br>";
                    // htmlInfoText = htmlInfoText + indent + "<b>" + packageData.getName() + "</b>" + "<br>";
                    htmlInfoText = htmlInfoText + indent + packageData.getName() + "<br>";
                }
            }

            indent = indent + "..";

            for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
                PackageDescription child = (PackageDescription) e.nextElement();
                htmlInfoText = setReadyToInstallInfoText(child, indent, htmlInfoText);
            }
        }

        return htmlInfoText;
    }

    // private void setReadyToUninstallInfoText(PackageDescription packageData, Integer indent, String htmlInfoText) {
    static private String setReadyToUninstallInfoText(PackageDescription packageData, String indent, String htmlInfoText) {
        // String spacer = "<spacer type=horizontal size=" + indent.toString() + ">";
        // System.out.println(ind);
        if (( packageData.isLeaf() ) || ( packageData.isAllChildrenHidden() )) {
            if ( ! packageData.isHidden() ) {
                if ( packageData.getSelectionState() == packageData.REMOVE ) {
                    // htmlInfoText = htmlInfoText + spacer + packageData.getName() + "<br>";
                    htmlInfoText = htmlInfoText + indent + packageData.getName() + "<br>";
                }
            }
        }

        if (( ! packageData.isLeaf() ) && ( ! packageData.isAllChildrenHidden() )) {
            if ( ! packageData.isHidden() ) {
                if (( packageData.getSelectionState() == packageData.REMOVE ) ||
                    ( packageData.getSelectionState() == packageData.REMOVE_SOME )) {
                    // htmlInfoText = htmlInfoText + spacer + "<b>" + packageData.getName() + "</b>" + "<br>";
                    // htmlInfoText = htmlInfoText + indent + "<b>" + packageData.getName() + "</b>" + "<br>";
                    htmlInfoText = htmlInfoText + indent + packageData.getName() + "<br>";
                }
            }

            indent = indent + "..";

            for (Enumeration e = packageData.children(); e.hasMoreElements(); ) {
                PackageDescription child = (PackageDescription) e.nextElement();
                htmlInfoText = setReadyToUninstallInfoText(child, indent, htmlInfoText);
            }
        }

        return htmlInfoText;
    }

    static public String  setInstallLogInfoText(ProductDescription productData, String htmlInfoText) {
        // String separatorline = "-------------------------------------------------------------------";
        String separatorline = "-------------------------------------------------------------";
        htmlInfoText = htmlInfoText + "<b>Product</b>: " + productData.get("product_fullname") + "<br>";
        InstallData data = InstallData.getInstance();
        htmlInfoText = htmlInfoText + "<b>Location</b>: " + data.getInstallDir() + "<br>";
        if (( data.getInstallRoot() != null ) && ( ! data.getInstallRoot().equals("null") )) {
            htmlInfoText = htmlInfoText + "<b>Root directory</b>: " + data.getInstallRoot() + "<br>";
        }
        htmlInfoText = htmlInfoText + "<b>Operating system</b>: " + data.getOSType() + "<br>";
        if ( data.isUserInstallation() ) {
            htmlInfoText = htmlInfoText + "<b>Installation type</b>: " + "User installation" + "<br>";
        } else {
            htmlInfoText = htmlInfoText + "<b>Installation type</b>: " + "Root installation" + "<br>";
        }

        htmlInfoText = htmlInfoText + separatorline + "<br>";

        htmlInfoText = LogManager.publishLogfileContent(htmlInfoText, separatorline);
        htmlInfoText = LogManager.publishCommandsLogfileContent(htmlInfoText);

        return htmlInfoText;
    }

}
