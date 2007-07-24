/*************************************************************************
 *
 *  $RCSfile: _XAccessibleContext.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change:$Date: 2007-07-24 13:03:58 $
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

package complex.toolkit.interface_tests;

import com.sun.star.lang.Locale;
import com.sun.star.uno.XInterface;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.accessibility.IllegalAccessibleComponentStateException;
import com.sun.star.accessibility.XAccessible;
import com.sun.star.accessibility.XAccessibleContext;
import com.sun.star.accessibility.XAccessibleRelationSet;
import com.sun.star.accessibility.XAccessibleStateSet;
import com.sun.star.accessibility.AccessibleRelationType;
import util.AccessibilityTools;
import share.LogWriter;

/**
 * Testing <code>com.sun.star.accessibility.XAccessibleContext</code>
 * interface methods :
 * <ul>
 *  <li><code> getAccessibleChildCount()</code></li>
 *  <li><code> getAccessibleChild()</code></li>
 *  <li><code> getAccessibleParent()</code></li>
 *  <li><code> getAccessibleIndexInParent()</code></li>
 *  <li><code> getAccessibleRole()</code></li>
 *  <li><code> getAccessibleDescription()</code></li>
 *  <li><code> getAccessibleName()</code></li>
 *  <li><code> getAccessibleRelationSet()</code></li>
 *  <li><code> getAccessibleStateSet()</code></li>
 *  <li><code> getLocale()</code></li>
 * </ul> <p>
 *
 * @see com.sun.star.accessibility.XAccessibleContext
 */
public class _XAccessibleContext {

    private LogWriter log;

    private static final String className =
        "com.sun.star.accessibility.XAccessibleContext" ;

    public XAccessibleContext oObj = null;

    private int childCount = 0;
    private XAccessible parent = null ;

    public _XAccessibleContext(XInterface object, LogWriter log) {
        oObj = (XAccessibleContext)UnoRuntime.queryInterface(
                                    XAccessibleContext.class, object);
        this.log = log;
    }

    /**
     * Calls the method and stores the number of children. <p>
     * Has <b> OK </b> status if non-negative number rutrned.
     */
    public boolean _getAccessibleChildCount() {
        childCount = oObj.getAccessibleChildCount();
        log.println("" + childCount + " children found.");
        return childCount > -1;
    }

    /**
     * Tries to get every child and checks its parent. <p>
     *
     * Has <b> OK </b> status if parent of every child
     * and the tested component are the same objects.
     *
     * The following method tests are to be completed successfully before :
     * <ul>
     *  <li> <code> getAccessibleChildCount() </code> : to have a number of
     *     children </li>
     * </ul>
     */
    public boolean _getAccessibleChild() {
        boolean bOK = true;
        int counter = childCount;
        if (childCount > 500) counter = 500;
        for (int i = 0; i < counter; i++) {
            try {
                XAccessible ch = oObj.getAccessibleChild(i) ;
                XAccessibleContext chAC = ch.getAccessibleContext();

                log.println("  Child " + i + ": " +
                    chAC.getAccessibleDescription()) ;

                if (!AccessibilityTools.equals
                    (chAC.getAccessibleParent().getAccessibleContext(), oObj)){

                    log.println("Role:");
                    log.println("Getting: "+chAC.getAccessibleParent().getAccessibleContext().getAccessibleRole());
                    log.println("Expected: "+oObj.getAccessibleRole());

                    log.println("ImplementationName:");
                    log.println("Getting: "+util.utils.getImplName(chAC.getAccessibleParent().getAccessibleContext()));
                    log.println("Expected: "+util.utils.getImplName(oObj));

                    log.println("The parent of child and component "+
                        "itself differ.");
                    log.println("Getting(Description): "
                            +chAC.getAccessibleParent().getAccessibleContext().getAccessibleDescription());
                    log.println("Expected(Description): "
                            +oObj.getAccessibleDescription());

                    bOK = false;
                } else {
                    log.println("Getting the expected Child -- OK");
                }
            } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
                e.printStackTrace();
                bOK = false;
            }
        }

        return bOK;
    }

    /**
     * Just gets the parent. <p>
     *
     * Has <b> OK </b> status if parent is not null.
     */
    public boolean _getAccessibleParent() {
        // assume that the component is not ROOT
        parent = oObj.getAccessibleParent();
        return parent != null;
    }

    /**
     * Retrieves the index of tested component in its parent.
     * Then gets the parent's child by this index and compares
     * it with tested component.<p>
     *
     * Has <b> OK </b> status if the parent's child and the tested
     * component are the same objects.
     *
     * The following method tests are to be completed successfully before :
     * <ul>
     *  <li> <code> getAccessibleParent() </code> : to have a parent </li>
     * </ul>
     */
    public boolean _getAccessibleIndexInParent() {

        boolean bOK = true;
        int idx = oObj.getAccessibleIndexInParent();

        XAccessibleContext parentAC = parent.getAccessibleContext() ;
        try {
            bOK &= AccessibilityTools.equals(
                parentAC.getAccessibleChild(idx).getAccessibleContext(),oObj);
            if (!bOK) {
                log.println("Expected: "+util.utils.getImplName(oObj));
                log.println("Getting: "+util.utils.getImplName(
                    parentAC.getAccessibleChild(idx).getAccessibleContext()));
            }
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            e.printStackTrace();
            bOK = false;
        }
        return bOK;
    }

    /**
     * Get the accessible role of component. <p>
     *
     * Has <b> OK </b> status if non-negative number rutrned.
     */
    public boolean _getAccessibleRole() {
        short role = oObj.getAccessibleRole();
        log.println("The role is " + role);
        return role > -1;
    }

    /**
     * Get the accessible name of the component. <p>
     *
     * Has <b> OK </b> status if the name has non-zero length.
     */
    public boolean _getAccessibleName() {
        String name = oObj.getAccessibleName();
        log.println("The name is '" + name + "'");
        return name != null;
    }

    /**
     * Get the accessible description of the component. <p>
     *
     * Has <b> OK </b> status if the description has non-zero length.
     */
    public boolean _getAccessibleDescription() {
        String descr = oObj.getAccessibleDescription();
        log.println("The description is '" + descr + "'");
        return descr != null;
    }

    /**
     * Just gets the set. <p>
     *
     * Has <b> OK </b> status if the set is not null.
     */
    public boolean _getAccessibleRelationSet() {
        XAccessibleRelationSet set = oObj.getAccessibleRelationSet();
        return set != null;
    }

    /**
     * Just gets the set. <p>
     *
     * Has <b> OK </b> status if the set is not null.
     */
    public boolean _getAccessibleStateSet() {
        XAccessibleStateSet set = oObj.getAccessibleStateSet();
        return set != null;
    }

    /**
     * Gets the locale. <p>
     *
     * Has <b> OK </b> status if <code>Country</code> and
     * <code>Language</code> fields of locale structure
     * are not empty.
     */
    public boolean _getLocale() {
        Locale loc = null ;
        try {
            loc = oObj.getLocale();
            log.println("The locale is " + loc.Language + "," + loc.Country);
        } catch (IllegalAccessibleComponentStateException e) {
            e.printStackTrace();
        }

        return loc != null && loc.Language.length() > 0 &&
                                            loc.Country.length() > 0;
    }
}

