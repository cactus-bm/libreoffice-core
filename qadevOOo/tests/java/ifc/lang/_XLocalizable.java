/*************************************************************************
 *
 *  $RCSfile: _XLocalizable.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change:$Date: 2003-12-11 11:41:12 $
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
package ifc.lang;

import lib.MultiMethodTest;

import com.sun.star.lang.Locale;
import com.sun.star.lang.XLocalizable;
import lib.Status;


public class _XLocalizable extends MultiMethodTest {

    public XLocalizable oObj;
    protected Locale initialLocale;

    public void _getLocale() {
        initialLocale = oObj.getLocale();
        tRes.tested("getLocale()", initialLocale != null);
    }

    public void _setLocale() {
        requiredMethod("getLocale()");

        String ro = (String) tEnv.getObjRelation("XLocalizable.ReadOnly");
        if (ro != null) {
            log.println(ro);
            tRes.tested("setLocale()", Status.skipped(true));
            return;
        }
        Locale newLocale = new Locale("de", "DE", "");
        oObj.setLocale(newLocale);

        Locale getLocale = oObj.getLocale();
        boolean res = ((getLocale.Country.equals(newLocale.Country)) &&
                      (getLocale.Language.equals(newLocale.Language)));

        if (!res) {
            log.println("Expected Language " + newLocale.Language +
                        " and Country " + newLocale.Country);
            log.println("Getting Language " + getLocale.Language +
                        " and Country " + getLocale.Country);
        }

        oObj.setLocale(initialLocale);
        tRes.tested("setLocale()", res);
    }
}