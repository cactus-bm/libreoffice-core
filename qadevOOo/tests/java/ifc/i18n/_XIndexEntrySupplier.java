/*************************************************************************
 *
 *  $RCSfile: _XIndexEntrySupplier.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change:$Date: 2003-01-27 18:10:34 $
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

package ifc.i18n;

import com.sun.star.i18n.XIndexEntrySupplier;
import com.sun.star.lang.Locale;
import lib.MultiMethodTest;

/**
* Testing <code>com.sun.star.i18n.XIndexEntrySupplier</code>
* interface methods:
* <ul>
*  <li><code> getIndexCharacter() </code></li>
*  <li><code> getIndexFollowPageWord() </code></li>
* </ul><p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.i18n.XIndexEntrySupplier
*/
public class _XIndexEntrySupplier extends MultiMethodTest {
    public XIndexEntrySupplier oObj = null;
    public String[] languages = new String[]{"de","en","es","fr","ja","ko","zh"};
    public String[] countries = new String[]{"DE","US","ES","FR","JP","KR","CN"};
    public String[] onePage = new String[]{"f.","p."," s."," sv","p.","",""};
    public String[] morePages = new String[]{"ff.","pp."," ss."," sv","pp.","",""};

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if the method returns right index for several
    * locales and word.
    */
    public void _getIndexCharacter() {
        boolean res = true;

        for (int i=0; i<7; i++) {
            String get = oObj.getIndexCharacter("chapter", getLocale(i), "");
            res &= get.equals("C");
        }
        tRes.tested("getIndexCharacter()", res);
    }

    /**
    * Test calls the method with two different parameters: for one page and
    * for several pages, after every call result is checked. <p>
    * Has <b> OK </b> status if method returns right index for several locales.
    */
    public void _getIndexFollowPageWord() {
        boolean res = true;

        for (int i=0; i<7; i++) {
            String get = oObj.getIndexFollowPageWord(true, getLocale(i));
            if (! get.equals(morePages[i]) ) {
                log.println("Language: " + languages[i]);
                log.println("Getting: #" + get + "#");
                log.println("Expected: #" + morePages[i] + "#");
            }
            res &= get.equals(morePages[i]);
            get = oObj.getIndexFollowPageWord(false,getLocale(i));
            if (! get.equals(onePage[i]) ) {
                log.println("Language: " + languages[i]);
                log.println("Getting: #" + get + "#");
                log.println("Expected: #" + onePage[i] + "#");
            }
            res &= get.equals(onePage[i]);
        }
        tRes.tested("getIndexFollowPageWord()", res);
    }

    /**
    * Method returns locale for a given language and country.
    * @param localeIndex index of needed locale.
    * @return Locale by the index from arrays defined above
    */
    public Locale getLocale(int k) {
        return new Locale(languages[k], countries[k], "");
    }


} // end XIndexEntrySupplier

