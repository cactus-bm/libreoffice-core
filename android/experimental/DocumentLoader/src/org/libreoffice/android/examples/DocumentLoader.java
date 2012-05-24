// -*- Mode: Java; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-

// Version: MPL 1.1 / GPLv3+ / LGPLv3+
//
// The contents of this file are subject to the Mozilla Public License Version
// 1.1 (the "License"); you may not use this file except in compliance with
// the License or as specified alternatively below. You may obtain a copy of
// the License at http://www.mozilla.org/MPL/
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
// for the specific language governing rights and limitations under the
// License.
//
// Major Contributor(s):
// Copyright (C) 2011 Tor Lillqvist <tml@iki.fi> (initial developer)
// Copyright (C) 2011 SUSE Linux http://suse.com (initial developer's employer)
//
// All Rights Reserved.
//
// For minor contributions see the git repository.
//
// Alternatively, the contents of this file may be used under the terms of
// either the GNU General Public License Version 3 or later (the "GPLv3+"), or
// the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
// in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
// instead of those above.

package org.libreoffice.android.examples;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import com.sun.star.uno.UnoRuntime;

import org.libreoffice.android.Bootstrap;

public class DocumentLoader
    extends Activity {

    private static String TAG = "DocumentLoader";

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        try {

            Bootstrap.setup(this);

            Bootstrap.putenv("SAL_LOG=yes");

            // Load a lot of shlibs here explicitly in advance because that
            // makes debugging work better, sigh
            Bootstrap.dlopen("libvcllo.so");
            Bootstrap.dlopen("libmergedlo.so");
            Bootstrap.dlopen("libswdlo.so");
            Bootstrap.dlopen("libswlo.so");
            
            Log.i(TAG, "Sleeping NOW");
            Thread.sleep(20000);

            com.sun.star.uno.XComponentContext xContext = null;

            xContext = com.sun.star.comp.helper.Bootstrap.defaultBootstrap_InitialComponentContext();

            Log.i(TAG, "xContext is" + (xContext!=null ? " not" : "") + " null");

            com.sun.star.lang.XMultiComponentFactory xMCF =
                xContext.getServiceManager();

            Log.i(TAG, "xMCF is" + (xMCF!=null ? " not" : "") + " null");

            String input = getIntent().getStringExtra("input");
            if (input == null)
                input = "/assets/test1.odt";

            // We need to fake up an argv, and the argv[0] even needs to
            // point to some file name that we can pretend is the "program".
            // setCommandArgs() will prefix argv[0] with the app's data
            // directory.

            String[] argv = { "lo-document-loader", input };

            Bootstrap.setCommandArgs(argv);

            Bootstrap.initVCL();

            Object oDesktop = xMCF.createInstanceWithContext(
                "com.sun.star.frame.Desktop", xContext);

            Log.i(TAG, "oDesktop is" + (oDesktop!=null ? " not" : "") + " null");

            Bootstrap.initUCBHelper();

            com.sun.star.frame.XComponentLoader xCompLoader =
                (com.sun.star.frame.XComponentLoader)
                     UnoRuntime.queryInterface(
                         com.sun.star.frame.XComponentLoader.class, oDesktop);

            Log.i(TAG, "xCompLoader is" + (xCompLoader!=null ? " not" : "") + " null");

            // Load the wanted document(s)
            String[] inputs = input.split(":");
            for (int i = 0; i < inputs.length; i++) {
                com.sun.star.beans.PropertyValue loadProps[] =
                    new com.sun.star.beans.PropertyValue[2];
                loadProps[0] = new com.sun.star.beans.PropertyValue();
                loadProps[0].Name = "Hidden";
                loadProps[0].Value = new Boolean(true);
                loadProps[1] = new com.sun.star.beans.PropertyValue();
                loadProps[1].Name = "ReadOnly";
                loadProps[1].Value = new Boolean(true);

                String sUrl = "file://" + inputs[i];

                Log.i(TAG, "Attempting to load " + sUrl);

                Object oDoc =
                    xCompLoader.loadComponentFromURL
                    (sUrl, "_blank", 0, loadProps);
                Log.i(TAG, "oDoc is " + (oDoc!=null ? oDoc.toString() : "null"));

                com.sun.star.lang.XTypeProvider typeProvider = (com.sun.star.lang.XTypeProvider) UnoRuntime.queryInterface(com.sun.star.lang.XTypeProvider.class, oDoc);
                Log.i(TAG, "typeProvider is " + (typeProvider!=null ? typeProvider.toString() : "null"));

                if (typeProvider != null) {
                    com.sun.star.uno.Type[] types = typeProvider.getTypes();
                    if (types != null) {
                        for (com.sun.star.uno.Type t : types) {
                            Log.i(TAG, "  " + t.getTypeName());
                        }
                    }
                }

                com.sun.star.view.XRenderable renderBabe = (com.sun.star.view.XRenderable) UnoRuntime.queryInterface(com.sun.star.view.XRenderable.class, oDoc);
                Log.i(TAG, "renderBabe is " + (renderBabe!=null ? renderBabe.toString() : "null"));

                com.sun.star.beans.PropertyValue renderProps[] =
                    new com.sun.star.beans.PropertyValue[1];
                renderProps[0] = new com.sun.star.beans.PropertyValue();
                renderProps[0].Name = "IsPrinter";
                renderProps[0].Value = new Boolean(true);
//                renderProps[1] = new com.sun.star.beans.PropertyValue();
//                renderProps[1].Name = "View";
//                renderProps[1].Value = no idea where to get an XController...

                Log.i(TAG, "getRendererCount: " + renderBabe.getRendererCount(oDoc, renderProps));

                renderBabe.render(0, oDoc, renderProps);
            }
        }
        catch (Exception e) {
            e.printStackTrace(System.err);
            System.exit(1);
        }
    }
}

// vim:set shiftwidth=4 softtabstop=4 expandtab:
