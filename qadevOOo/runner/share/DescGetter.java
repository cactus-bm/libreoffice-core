/*************************************************************************
 *
 *  $RCSfile: DescGetter.java,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change:$Date: 2005-06-14 15:42:54 $
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
package share;

import java.io.BufferedReader;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.StringTokenizer;

import java.util.Vector;


/**
 *
 * Base Interface to get a description for a given TestJob
 *
 */
public abstract class DescGetter {
    public abstract DescEntry[] getDescriptionFor(String entry,
                                                  String DescPath,
                                                  boolean debug);

    protected abstract DescEntry getDescriptionForSingleJob(String job,
                                                            String descPath,
                                                            boolean debug);

    protected abstract String[] createScenario(String descPath, String job,
                                               boolean debug);

    protected DescEntry[] getScenario(String url, String descPath,
                                      boolean debug) {
        Vector entryList = new Vector();
        String line = "";
        BufferedReader scenario = null;
        DescEntry[] entries = null;

        try {
            scenario = new BufferedReader(new FileReader(url));
        } catch (java.io.FileNotFoundException fnfe) {
            System.out.println("Couldn't find file " + url);

            return entries;
        }

        while (line != null) {
            try {
                if (line.startsWith("-o")) {
                    String job = line.substring(3, line.length()).trim();
                    DescEntry aEntry ;
                    // special in case several Interfaces are given comma separated
                    if (job.indexOf(",") < 0) {
                        aEntry = getDescriptionForSingleJob(job, descPath,
                                                                     debug);
                    } else {
                        ArrayList subs = getSubInterfaces(job);
                        String partjob = job.substring(0, job.indexOf(",")).trim();
                        aEntry = getDescriptionForSingleJob(partjob, descPath,
                                                                     debug);

                        if (aEntry != null) {
                            for (int i = 0; i < aEntry.SubEntryCount; i++) {
                                String subEntry = aEntry.SubEntries[i].longName;
                                int cpLength = aEntry.longName.length();
                                subEntry = subEntry.substring(cpLength + 2,
                                                              subEntry.length());

                                if (subs.contains(subEntry)) {
                                    aEntry.SubEntries[i].isToTest = true;
                                }
                            }
                        }
                    }
//                    DescEntry aEntry = getDescriptionForSingleJob(
//                                          line.substring(3).trim(), descPath,
//                                          debug);
                    if (aEntry != null)
                        entryList.add(aEntry);
                } else if (line.startsWith("-sce")) {
                    DescEntry[] subs = getScenario(line.substring(5,
                                                                  line.length())
                                                       .trim(), descPath,
                                                   debug);

                    for (int i = 0; i < subs.length; i++) {
                        entryList.add(subs[i]);
                    }
                } else if (line.startsWith("-p")) {
                    String[] perModule = createScenario(descPath,
                                                        line.substring(3)
                                                            .trim(), debug);

                    for (int i = 0; i < perModule.length; i++) {
                        DescEntry aEntry = getDescriptionForSingleJob(
                                                   perModule[i].substring(3)
                                                               .trim(),
                                                   descPath, debug);
                        if (aEntry != null)
                            entryList.add(aEntry);
                    }
                }

                line = scenario.readLine();
            } catch (java.io.IOException ioe) {
                if (debug) {
                    System.out.println("Exception while reading scenario");
                }
            }
        }

        try {
            scenario.close();
        } catch (java.io.IOException ioe) {
            if (debug) {
                System.out.println("Exception while closeing scenario");
            }
        }

        if (entryList.size() == 0)
            return null;
        entries = new DescEntry[entryList.size()];
        entries = (DescEntry[]) entryList.toArray(entries);

        return entries;
    }

    protected ArrayList getSubInterfaces(String job) {
        ArrayList namesList = new ArrayList();
        StringTokenizer st = new StringTokenizer(job, ",");

        for (int i = 0; st.hasMoreTokens(); i++) {
            String token = st.nextToken();

            if (token.indexOf(".") < 0) {
                namesList.add(token);
            }
        }

        return namesList;
    }
}