/*************************************************************************
 *
 *  $RCSfile: _XFileIdentifierConverter.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change:$Date: 2003-05-27 12:29:58 $
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

package ifc.ucb;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.ucb.XFileIdentifierConverter;
import lib.MultiMethodTest;

/**
* Testing <code>com.sun.star.ucb.XFileIdentifierConverter</code>
* interface methods :
* <ul>
*  <li><code> getFileProviderLocality()</code></li>
*  <li><code> getFileURLFromSystemPath()</code></li>
*  <li><code> getSystemPathFromFileURL()</code></li>
* </ul> <p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.ucb.XFileIdentifierConverter
*/
public class _XFileIdentifierConverter extends MultiMethodTest {

    /**
     * Contains the tested object.
     */
    public XFileIdentifierConverter oObj;

    /**
     * Gets the locality for SOffice temporary directory. <p>
     * Has <b> OK </b> status if the method returns value greater
     * than 0 (as office temp directory is supposed to be in the
     * same location). <p>
     */
    public void _getFileProviderLocality() {
        String baseURL = util.utils.getOfficeTemp((XMultiServiceFactory)tParam.getMSF());
        log.println("Using: "+baseURL);
        int loc = oObj.getFileProviderLocality(baseURL);
        log.println("Getting: "+loc);
        tRes.tested("getFileProviderLocality()",loc > 0);
    }

    /**
     * Tries to convert URL of SOffice temp directory to system
     * dependent path. <p>
     * Has <b> OK </b> status if the method returns system dependent
     * representation of the URL passed. <p>
     */
    public void _getSystemPathFromFileURL() {
        String baseURL = util.utils.getOfficeTemp((XMultiServiceFactory)tParam.getMSF());
        log.println("Using (Base): "+baseURL);
        String sysURL = util.utils.getOfficeTempDirSys((XMultiServiceFactory)tParam.getMSF());
        log.println("Using (System): "+sysURL);
        String get = oObj.getSystemPathFromFileURL(baseURL);
        log.println("Getting: "+get);
        //sysURL = sysURL.substring(0,sysURL.length()-1);
        tRes.tested("getSystemPathFromFileURL()",get.equals(sysURL));
    }

    /**
     * Tries to convert system dependent path of SOffice temp
     * directory to URL representation. <p>
     * Has <b> OK </b> status if the method returns URL representation
     * of the system dependent path passed. <p>
     */
    public void _getFileURLFromSystemPath() {
        String baseURL = util.utils.getOfficeTemp((XMultiServiceFactory)tParam.getMSF());
        log.println("Using (Base): "+baseURL);
        String sysURL = util.utils.getOfficeTempDirSys((XMultiServiceFactory)tParam.getMSF());
        log.println("Using (System): "+sysURL);
        String get = oObj.getFileURLFromSystemPath(sysURL,sysURL);
        log.println("Getting: "+get);
        tRes.tested("getFileURLFromSystemPath()",get.equals(baseURL));
    }

}
