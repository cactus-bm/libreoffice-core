/*************************************************************************
*
*  $RCSfile: CGCategory.java,v $
*
*  $Revision: 1.3 $
*
*  last change: $Author: vg $ $Date: 2005-02-21 14:00:16 $
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
*/
package com.sun.star.wizards.table;

import com.sun.star.container.NoSuchElementException;
import com.sun.star.container.XNameAccess;
import com.sun.star.lang.Locale;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.Exception;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.wizards.common.Configuration;
import com.sun.star.wizards.common.Desktop;
import com.sun.star.wizards.common.Helper;
import com.sun.star.wizards.common.Properties;

/**
 * @author Administrator
 *
 * To change the template for this generated type comment go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
// import com.sun.star.wizards.common.ConfigGroup;

public class CGCategory{

    public String Name;
    public int Index;
    private String[] Tables;
    private final String CGROOTPATH = "/org.openoffice.Office.TableWizard/TableWizard/";
    XMultiServiceFactory xMSF;
    XNameAccess xNameAccessTablesNode;
    XNameAccess xNameAccessCurBusinessNode;
    Object oconfigView;

    public CGCategory(XMultiServiceFactory _xMSF){
        xMSF = _xMSF;
    }


    public void initialize(int _index){
    try {
        oconfigView = Configuration.getConfigurationRoot(xMSF, CGROOTPATH, false);  //business/Tables
        xNameAccessCurBusinessNode = Configuration.getChildNodebyIndex(oconfigView, _index);
    } catch (Exception e) {
        e.printStackTrace(System.out);
    }}


    public String[] getTableNames(){
    try {
        xNameAccessTablesNode = (XNameAccess) UnoRuntime.queryInterface(XNameAccess.class, xNameAccessCurBusinessNode.getByName("Tables"));
        String[] sTableNames = Configuration.getNodeDisplayNames(xNameAccessTablesNode);
        return sTableNames;
    } catch (Exception e) {
        e.printStackTrace(System.out);
        return null;
    }}


}
