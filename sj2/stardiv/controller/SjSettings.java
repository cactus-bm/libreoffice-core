/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: SjSettings.java,v $
 * $Revision: 1.8 $
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

package stardiv.controller;

import java.util.Properties;
import java.util.Hashtable;
import java.util.Enumeration;

import com.sun.star.lib.sandbox.SandboxSecurity;

/**
 * Ueber diese Klasse werden alle globalen Einstellungen, die fuer das Sj Projekt
 * wichtig sind, dokumentiert und modifiziert.
 *
 * @version    $Version: 1.0 $
 * @author     Markus Meyer
 *
 */
public class SjSettings {
    /**
     * The following properties are used to setup the environment for
     * the stardiv packages.<BR>
     * "appletviewer.security.mode"="unrestricted" | "host" | "none": Set the
     * security level of the default SecurityManager. The default is "host".<BR>
     * "stardiv.security.defaultSecurityManager"="true" | "false": Create and set
     * the stardiv.security.AppletSecurity, if the property is "true". This occures
     * only in the first call.<BR>
     * "stardiv.security.noExit"="true" | "false": no exit is allowed. Use this property
     * if you are running more than one java application in the virtual machine. This occures
     * only in the first call.<BR>
     * "stardiv.security.disableSecurity"="true" | "false": disable security checking. Only usefull
     * if a SecurityManager is installed. The default is "false".<BR>
     * if you are running more than one java application in the virtual machine. This occures
     * only in the first call.<BR>
     * "stardiv.js.debugOnError"="true" | "false": Start the javascript ide, if an error
     * occures. The default is "false".<BR>
     * "stardiv.js.debugImmediate"="true" | "false": Start the javascript ide, if a script
     * starts. The default is "false".<BR>
     * "stardiv.debug.trace"="messageBox" | "window" | "file" | "none": The trace pipe is
     * set to one of the four mediums. The Default is "none".<BR>
     * "stardiv.debug.error"="messageBox" | "window" | "file" | "none": The error pipe is
     * set to one of the four mediums. The Default is "none".<BR>
     * "stardiv.debug.warning"="messageBox" | "window" | "file" | "none": The warning pipe is
     * set to one of the four mediums. The Default is "none".<BR>
     * If the properties http.proxyHost, http.proxyPort, http.maxConnections,
     * http.keepAlive or http.nonProxyHosts are changed, the method
     * sun.net.www.http.HttpClient.resetProperties() is called.<BR>
     * If the properties ftpProxySet, ftpProxyHost or ftpProxyPort are changed,
     * the static variables useFtpProxy, ftpProxyHost and ftpProxyPort in the class
     * sun.net.ftp.FtpClient are set.<BR>
     * <B>If you are writing your own SecurityManager and ClassLoader, please implement the
     * interfaces stardiv.security.ClassLoaderExtension and
     * stardiv.security.SecurityManagerExtension. Be shure to set the
     * stardiv.security.ClassLoaderFactory, to enable dynamic class loading, otherwise
     * the stardiv.security.AppletClassLoader is used. Set the factory with
     * SjSettings.setClassLoaderFactory().</B>
     */
    static public synchronized void changeProperties( Properties pChangeProps )
    {
        SecurityManager pSM = System.getSecurityManager();
        if( pSM != null )
            pSM.checkPropertiesAccess();
        Properties props = new Properties( System.getProperties() );
        boolean bInited = Boolean.getBoolean( "stardiv.controller.SjSettings.inited" );


        if( !bInited )
        {
            // check the awt.toolkit property: if none is set use com.sun.star.comp.jawt.peer.Toolkit
            //if ( props.getProperty("awt.toolkit") == null )
            //  props.put("awt.toolkit", "com.sun.star.comp.jawt.peer.Toolkit");

            // Define a number of standard properties
            props.put("acl.read", "+");
            props.put("acl.read.default", "");
            props.put("acl.write", "+");
            props.put("acl.write.default", "");

            // Standard browser properties
            props.put("browser", "stardiv.applet.AppletViewerFrame");
            props.put("browser.version", "4.02");
            props.put("browser.vendor", "Sun Microsystems, Inc.");
            props.put("http.agent", "JDK/1.1");

            // Define which packages can be accessed by applets
            props.put("package.restrict.access.sun", "true");
            props.put("package.restrict.access.netscape", "true");
            props.put("package.restrict.access.stardiv", "true");

            // Define which packages can be extended by applets
            props.put("package.restrict.definition.java", "true");
            props.put("package.restrict.definition.sun", "true");
            props.put("package.restrict.definition.netscape", "true");
            props.put("package.restrict.definition.stardiv", "true");

            // Define which properties can be read by applets.
            // A property named by "key" can be read only when its twin
            // property "key.applet" is true.  The following ten properties
            // are open by default.  Any other property can be explicitly
            // opened up by the browser user setting key.applet=true in
            // ~/.hotjava/properties.   Or vice versa, any of the following can
            // be overridden by the user's properties.
            props.put("java.version.applet", "true");
            props.put("java.vendor.applet", "true");
            props.put("java.vendor.url.applet", "true");
            props.put("java.class.version.applet", "true");
            props.put("os.name.applet", "true");
            props.put("os.version.applet", "true");
            props.put("os.arch.applet", "true");
            props.put("file.separator.applet", "true");
            props.put("path.separator.applet", "true");
            props.put("line.separator.applet", "true");

        }

        // put new and changed properties to the property table
        if( pChangeProps != null )
        {
            Enumeration aEnum = pChangeProps.propertyNames();
            while( aEnum.hasMoreElements() )
            {
                String aKey = (String)aEnum.nextElement();
                props.put( aKey, pChangeProps.getProperty( aKey ) );
            }
        }

        // Install a property list.
        if( !bInited )
            props.put( "stardiv.controller.SjSettings.inited", "true" );
        System.setProperties(props);
        if( !bInited )
        {
            // Security Manager setzten
            boolean bNoExit = Boolean.getBoolean( "stardiv.security.noExit" );
            //Create and install the security manager
            if (System.getSecurityManager() == null)
                System.setSecurityManager(new SandboxSecurity(bNoExit));

//          if( Boolean.getBoolean("stardiv.controller.installConsole") )
//              Console.installConsole();
        }
    }

    private static boolean equalsImpl( Object p1, Object p2 )
    {
        return p1 == p2 || (p1 != null && p2 != null && p1.equals( p2 ) );
    }
}



