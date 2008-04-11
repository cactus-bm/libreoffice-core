/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: AppletProps.java,v $
 * $Revision: 1.4 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

package stardiv.app;

import stardiv.app.AppletMessageHandler;
import stardiv.applet.AppletExecutionContext;

import java.awt.*;
import java.io.*;
import java.util.Properties;
import sun.net.www.http.HttpClient;
import sun.net.ftp.FtpClient;

public class AppletProps extends Frame {
    TextField proxyHost;
    TextField proxyPort;
    Choice networkMode;
    Choice accessMode;
    Choice unsignedMode;

    AppletExecutionContext appletExecutionContext;

    AppletProps(AppletExecutionContext appletExecutionContext) {
        this.appletExecutionContext = appletExecutionContext;

        setTitle(amh.getMessage("title"));
        Panel p = new Panel();
        p.setLayout(new GridLayout(0, 2));

        p.add(new Label(amh.getMessage("label.http.server", "Http proxy server:")));
        p.add(proxyHost = new TextField());

        p.add(new Label(amh.getMessage("label.http.proxy")));
        p.add(proxyPort = new TextField());

        p.add(new Label(amh.getMessage("label.network")));
        p.add(networkMode = new Choice());
        networkMode.addItem(amh.getMessage("choice.network.item.none"));
        networkMode.addItem(amh.getMessage("choice.network.item.applethost"));
        networkMode.addItem(amh.getMessage("choice.network.item.unrestricted"));

        String securityMode = System.getProperty("appletviewer.security.mode");
        securityMode = (securityMode == null) ? "none" : securityMode;
        securityMode = securityMode.equals("host") ? "applethost" : securityMode;
        networkMode.select(amh.getMessage("choice.network.item." + securityMode));

        p.add(new Label(amh.getMessage("label.class")));
        p.add(accessMode = new Choice());
        accessMode.addItem(amh.getMessage("choice.class.item.restricted"));
        accessMode.addItem(amh.getMessage("choice.class.item.unrestricted"));

        accessMode.select(Boolean.getBoolean("package.restrict.access.sun")
                          ? amh.getMessage("choice.class.item.restricted")
                          : amh.getMessage("choice.class.item.unrestricted"));

        p.add(new Label(amh.getMessage("label.unsignedapplet")));
        p.add(unsignedMode = new Choice());
        unsignedMode.addItem(amh.getMessage("choice.unsignedapplet.no"));
        unsignedMode.addItem(amh.getMessage("choice.unsignedapplet.yes"));

        add("Center", p);
        p = new Panel();
        p.add(new Button(amh.getMessage("button.apply")));
        p.add(new Button(amh.getMessage("button.reset")));
        p.add(new Button(amh.getMessage("button.cancel")));
        add("South", p);
        setLocation(200, 150);
        pack();
        reset();
    }

    void reset() {
        //      if (Boolean.getBoolean("package.restrict.access.sun")) {
        //              accessMode.select(amh.getMessage("choice.class.item.restricted"));
        //      } else {
        //              accessMode.select(amh.getMessage("choice.class.item.unrestricted"));
        //      }

        if (System.getProperty("http.proxyHost") != null) {
            proxyHost.setText(System.getProperty("http.proxyHost"));
            proxyPort.setText(System.getProperty("http.proxyPort"));
        //  HttpClient.proxyPort = Integer.valueOf(System.getProperty("http.proxyPort")).intValue();
        }
        else {
            proxyHost.setText("");
            proxyPort.setText("");
        }

        //      if (Boolean.getBoolean("appletviewer.security.allowUnsigned")) {
        //              unsignedMode.select(amh.getMessage("choice.unsignedapplet.yes"));
        //      } else {
        //              unsignedMode.select(amh.getMessage("choice.unsignedapplet.no"));
        //      }
    }

    void apply() {
        // Get properties, set version
        Properties props = System.getProperties();
        if (proxyHost.getText().length() > 0) {
            props.put("http.proxyHost", proxyHost.getText().trim());
            props.put("http.proxyPort", proxyPort.getText().trim());
        } else {
            props.remove("http.proxyHost");
        }
        if ("None".equals(networkMode.getSelectedItem())) {
            props.put("appletviewer.security.mode", "none");
        } else if ("Unrestricted".equals(networkMode.getSelectedItem())) {
            props.put("appletviewer.security.mode", "unrestricted");
        } else {
            props.put("appletviewer.security.mode", "host");
        }

        if ("Restricted".equals(accessMode.getSelectedItem())) {
            props.put("package.restrict.access.sun", "true");
            props.put("package.restrict.access.netscape", "true");
            props.put("package.restrict.access.stardiv", "true");
        } else {
            props.put("package.restrict.access.sun", "false");
            props.put("package.restrict.access.netscape", "false");
            props.put("package.restrict.access.stardiv", "false");
        }

        if ("Yes".equals(unsignedMode.getSelectedItem())) {
            props.put("appletviewer.security.allowUnsigned", "true");
        } else {
            props.put("appletviewer.security.allowUnsigned", "false");
        }

        // Save properties
        try {
            reset();

            FileOutputStream out = new FileOutputStream(AppletViewer.theUserPropertiesFile);
            props.save(out, "AppletViewer");
            out.close();
            setVisible( false );
        } catch (IOException e) {
            System.out.println(amh.getMessage("apply.exception", e));
            e.printStackTrace();
            reset();
        }
    }

    public boolean action(Event evt, Object obj) {
        if (amh.getMessage("button.apply").equals(obj)) {
            apply();
            return true;
        }
        if (amh.getMessage("button.reset").equals(obj)) {
            reset();
            return true;
        }
        if (amh.getMessage("button.cancel").equals(obj)) {
            setVisible( false );
            return true;
        }
        return false;
    }

    private static AppletMessageHandler amh = new AppletMessageHandler("appletprops");

}
