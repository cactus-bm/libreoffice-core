/*************************************************************************
 *
 *  $RCSfile: PathSubstitutionTest.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Date: 2007-07-24 13:28:13 $
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
package complex.path_substitution;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.XStringSubstitution;
import complexlib.ComplexTestCase;
import java.util.Vector;

/**
 *
 */
public class PathSubstitutionTest extends ComplexTestCase {

    private static XMultiServiceFactory xMSF;

    // all substitution variables
    private VariableContainer substVars = null;

    /**
     * A function to tell the framework, which test functions are available.
     * Right now, it's only 'checkXStringSubstitution'.
     * @return All test methods.
     */
    public String[] getTestMethodNames() {
        return new String[]{"checkXStringSubstitution"};
    }

    /**
     * Create an array with all substitution variables
     */
    private void initialize() {
        substVars = new VariableContainer();
        substVars.add("$(prog)", false, true);
        substVars.add("$(inst)", false, true);
        substVars.add("$(user)", false, true);
        substVars.add("$(work)", false, true);
        substVars.add("$(home)", false, true);
        substVars.add("$(temp)", false, true);
        substVars.add("$(lang)", false, false);
        substVars.add("$(langid)", false, false);
        substVars.add("$(vlang)", false,false);
        // path won't resubstitute
        substVars.add("$(path)", false,false);
    }

    /**
     * One actual test: as the method 'getTestMethodNames()' tells.
     */
    public void checkXStringSubstitution()
    {
        xMSF = (XMultiServiceFactory)param.getMSF();
        log.println("---- Testing the XStringSubstitution interface ----");
        log.println("Create intance of test object.\n");
        XStringSubstitution oObj = null;
        try {
            Object x = xMSF.createInstance(
                                "com.sun.star.util.PathSubstitution");
            oObj = (XStringSubstitution)
                        UnoRuntime.queryInterface(XStringSubstitution.class, x);
            if (oObj == null) throw new com.sun.star.uno.Exception();
        }
        catch(com.sun.star.uno.Exception e) {
            log.println(e.getClass().getName());
            log.println("Message: " + e.getMessage());
            failed("Could not create an instance of the test object.");
            return;
        }

        initialize();

        for (int i=0; i<substVars.size(); i++) {
            String var = substVars.getVariable(i);
            log.println("Testing var '" + var + "'");
            try {
                String substVal = oObj.getSubstituteVariableValue(var);
                log.println("\tvalue '" + substVal + "'");
                substVars.putValue(i,substVal);

                // simple check: let path in a string replace
                String substString = var + "/additional/path";

                log.println("Substitute '"+substString+"'");
                String newValue = oObj.substituteVariables(substString, true);
                log.println("Return value '"+newValue+"'");
                // 2do: better check for correct substitution
                assure("Did not substitute '"
                        + substString+"' to '" + newValue
                        + "' correctly:", newValue.startsWith(substVal));

                // simple check part two:
                //make substitution backwards if possible
                if (substVars.canReSubstitute(i)) {
                    substString = substVal + "/additional/path";

                    log.println("Substitute backwards '"+substString+"'");
                    newValue = oObj.reSubstituteVariables(substString);
                    log.println("Return value '"+newValue+"'");
                    // 2do: better check for correct substitution
                    assure("Did not reSubstitute '"
                            + substString + "' to '" + newValue
                            + "' correctly:", checkResubstitute(newValue, var));
                }

                // simple check part three: look if replace
                //in middle of text works
                substString = "file:///starting/" + var + "/path";

                log.println("Substitute '"+substString+"'");
                newValue = oObj.substituteVariables(substString, false);
                log.println("Return value '"+newValue+"'");
                boolean erg = true;
                if(substVars.onlySubstituteAtBegin(i))
                    // in this case it should not have worked
                    erg = newValue.indexOf(substVal)==-1;
                else
                    erg = newValue.indexOf(substVal)!=-1;

                assure("Did not substitute '"
                        + substString + "' to '" + newValue
                        + "' correctly:", erg);

            }
            catch(com.sun.star.uno.Exception e) {
                log.println(e.getClass().getName());
                log.println("Message: " + e.getMessage());
                failed("Could not create an instance of the test object.");
                return;
            }
            log.println("Finish testing '" + var + "'\n");
        }

        // check of greedy resubstitution
        String prog = "$(prog)";
        String inst = "$(inst)";
        String instPth = substVars.getValue(inst);
        String progPth = substVars.getValue(prog);

        if (progPth.startsWith(instPth) && instPth.startsWith(progPth)) {
            log.println("Greedy ReSubstitute");
            String substString = progPth + "/additional/path";
            String newVal = oObj.reSubstituteVariables(substString);
            log.println("String '" + substString +
                                "' should be resubstituted with");
            log.println("Variable '" + prog + "' instead of Variable '" +
                                inst + "'");
            assure("Did not reSubstitute '" + substString
                    + "' to '" + newVal + "' correctly:",
                    newVal.startsWith(prog));
        }

        log.println(
                "---- Finish testing the XStringSubstitution interface ----");
    }

    /**
     * test the resubstitution
     * @return true, if resubstitution is correct.
     */
    private boolean checkResubstitute(String subst, String original) {
        // simple: subst starts with original
        if ( subst.startsWith(original) ) {
            return true;
        }
        else  {                             // hard: been resubstituted with a differernt variable.
            for (int i=0; i<substVars.size(); i++) {
                String var = substVars.getVariable(i);
                if ( subst.startsWith(var) && original.startsWith(original)) {
                    return true;
                }
            }
        }
        return false;
    }
    /**
     * Class for containing the substitution variables with their
     * values and some information.
     */
    private class VariableContainer {
        public Vector varName;
        public Vector varValue;
        public Vector substAtBegin;
        public Vector resubst;

        public VariableContainer() {
            varName = new Vector();
            varValue = new Vector();
            substAtBegin = new Vector();
            resubst = new Vector();
        }

        public void add(String var) {
            varName.add(var);
            substAtBegin.add(Boolean.TRUE);
            resubst.add(Boolean.TRUE);
        }
        public void add(String var, boolean onlySubstAtBegin,
                                                        boolean canResubst) {
            varName.add(var);
            this.substAtBegin.add(new Boolean(onlySubstAtBegin));
            this.resubst.add(new Boolean(canResubst));
        }

        public void putValue(int i, String val) {
            varValue.add(i, val);
        }

        public int size() { return varName.size(); }
        public String getVariable(int i) { return (String)varName.get(i); }
        public String getValue(int i) { return (String)varName.get(i); }
        public String getValue(String var) {
            return (String)varValue.get(varName.indexOf(var));
        }
        public boolean onlySubstituteAtBegin(int i) {
            return ((Boolean)substAtBegin.get(i)).booleanValue();
        }
        public boolean canReSubstitute(int i) {
            return ((Boolean)resubst.get(i)).booleanValue();
        }
    }
}
