/*************************************************************************
 *
 *  $RCSfile: CfgParser.java,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change:$Date: 2004-11-02 11:31:13 $
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

package helper;

import lib.TestParameters;
import java.util.Properties;
import java.util.Enumeration;
import java.io.FileInputStream;

/**
 * This class parses the ini files and stores the data
 * <br>
 * inside TestParameters
 */
public class CfgParser {
    protected boolean debug = false;
    protected String iniFile="";

    public CfgParser(String ini) {
        if (ini != null)
            this.iniFile = ini;
    }

    public void getIniParameters(TestParameters param) {
        debug = param.DebugIsActive;
        Properties cfg = null;
        if (iniFile.equals("")) {
            //no iniFile given, search one in the users home directory
            cfg = getProperties(getDefaultFileName(true));
            //try to search the user dir if no iniFile could be found yet
            if (cfg == null) {
                cfg = getProperties(getDefaultFileName(false));
            }
        } else {
            cfg = getProperties(iniFile);
        }
        if (cfg != null) {
            Enumeration cfgEnum = cfg.keys();
            while (cfgEnum.hasMoreElements()) {
                String pName = (String) cfgEnum.nextElement();
                Object pValue = cfg.getProperty(pName);
                if (pValue instanceof String)
                    pValue = ((String)pValue).trim();
                param.put(pName.trim(),pValue);

                if (pName.equals("TestDocumentPath")) {
                    System.setProperty("DOCPTH",(String)pValue);
                    param.put("DOCPTH",(String)pValue);
                }
            }
        }

        //check for platform dependend parameters
        //this would have a $OperatingSystem as prefix
        String os = (String) param.get("OperatingSystem");
        if (os != null) {
            //found something that could be a prefex
            //check all parameters for this
            Enumeration keys = param.keys();
            while (keys.hasMoreElements()) {
                String key = (String) keys.nextElement();
                Object oldValue = param.get(key);
                if (key.startsWith(os)) {
                    String newKey = key.substring(os.length()+1);
                    param.remove(key);
                    param.put(newKey,oldValue);
                }
            }

        }
    }

    protected Properties getProperties(String name) {
        // get the resource file
        Properties prop = new Properties();
        if ( debug ) {
            System.out.println("Looking for "+name);
        }
        try {
            FileInputStream propFile = new FileInputStream(name);
            prop.load(propFile);
            System.out.println("Parsing properties from "+name);
            propFile.close();
        } catch (Exception e) {
            try {
                java.net.URL url = this.getClass().getResource("/"+name);
                if (url != null) {
                    System.out.println("Parsing properties from "+name);
                    java.net.URLConnection connection = url.openConnection();
                    java.io.InputStream in = connection.getInputStream();
                    prop.load(in);
                }
            } catch (Exception ex) {
                //Exception while reading prop-file, returning null
                return null;
            }
        }

        return prop;
    }

    protected String getDefaultFileName(boolean home) {
        String fileSeparator = System.getProperty("file.separator");
        String path = "";
        if (home) {
            //look inside the home directory
            path = System.getProperty("user.home");
        } else {
            path = System.getProperty("user.dir");
        }
        if (fileSeparator.equals("/")) {
            //suppose I'm on Unix-platform
            return path+fileSeparator+".runner.props";
        } else {
            //suppose I'm on Windows
            return path+fileSeparator+"runner.props";
        }
    }

}
