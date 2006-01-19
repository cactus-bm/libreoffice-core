/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ReferenceBuilder.java,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2006-01-19 14:22:28 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

package convwatch;

// imports
import java.util.Enumeration;
import java.io.File;
import java.io.FileFilter;
import java.util.ArrayList;

import com.sun.star.lang.XMultiServiceFactory;

import convwatch.DirectoryHelper;
import convwatch.OfficePrint;
import convwatch.ConvWatchException;
import convwatch.EnhancedComplexTestCase;
import convwatch.PropertyName;
import helper.OfficeProvider;
import convwatch.PerformanceContainer;

/**
 * The following Complex Test will test
 * an already created document and it's postscript output (by an older office version)
 * with a new office version.
 * This test use Ghostscript for the jpeg export and graphically compare tools from ImageMagick.
 * Read the manual for more information.
 *
 * this is only the starter program
 * more is found in qadevOOo/runner/convwatch/*
 */

public class ReferenceBuilder extends EnhancedComplexTestCase
{
    // The first of the mandatory functions:
    /**
     * Return the name of the test.
     * In this case it is the actual name of the service.
     * @return The tested service.
     */
    public String getTestObjectName() {
        return "ReferenceBuilder runner";
    }

    // The second of the mandatory functions: return all test methods as an
    // array. There is only one test function in this example.
    /**
     * Return all test methods.
     * @return The test methods.
     */
    public String[] getTestMethodNames() {
        return new String[]{"buildreference"};
    }

    // This test is fairly simple, so there is no need for before() or after()
    // methods.

    public void before()
        {
            // System.out.println("before()");
        }

    public void after()
        {
            // System.out.println("after()");
        }

    // The test method itself.
    private String m_sInputPath = "";
    private String m_sReferencePath = "";
    private boolean m_bIncludeSubdirectories = true;

    void initMember()
        {
            // MUST PARAMETER
            // INPUT_PATH ----------
            String sINPATH = (String)param.get( PropertyName.DOC_COMPARATOR_INPUT_PATH );
            boolean bQuit = false;
            String sError = "";
            if (sINPATH == null || sINPATH.length() == 0)
            {
                log.println("Please set input path (path to documents) " + PropertyName.DOC_COMPARATOR_INPUT_PATH + "=path.");
                bQuit = true;
            }
            else
            {
                log.println("found " + PropertyName.DOC_COMPARATOR_INPUT_PATH + " " + sINPATH);
                m_sInputPath = sINPATH;
            }

            // REFERENCE_PATH ----------
            String sREF = (String)param.get( PropertyName.DOC_COMPARATOR_REFERENCE_PATH );
            if (sREF == null || sREF.length() == 0)
            {
                log.println("Please set output path (path to a directory, where the references should stay) " + PropertyName.DOC_COMPARATOR_REFERENCE_PATH + "=path.");
                bQuit = true;
            }
            else
            {
                log.println("found " + PropertyName.DOC_COMPARATOR_REFERENCE_PATH + " " + sREF);
                m_sReferencePath = sREF;
            }

            if (bQuit == true)
            {
                // log.println("must quit.");
                assure("Must quit, Parameter problems.", false);
            }

            if (m_sInputPath.startsWith("file:") ||
                m_sReferencePath.startsWith("file:"))
            {
                assure("We can't handle file: URL right, use system path instead.", false);
            }

        }

    /**
     * Function returns a List of software which must accessable as an external executable
     */
    protected Object[] mustInstalledSoftware()
        {
            ArrayList aList = new ArrayList();
            aList.add("perl -version");
            return aList.toArray();
        }

    // the test ======================================================================
    public void buildreference()
        {
            GlobalLogWriter.set(log);

            // check if all need software is installed and accessable
            checkEnvironment(mustInstalledSoftware());

            // test_removeFirstDirectorysAndBasenameFrom();
            // Get the MultiServiceFactory.
            // XMultiServiceFactory xMSF = (XMultiServiceFactory)param.getMSF();
            GraphicalTestArguments aGTA = getGraphicalTestArguments();
            if (aGTA == null)
            {
                assure("Must quit", false);
            }
            initMember();

            File aInputPath = new File(m_sInputPath);
            if (aInputPath.isDirectory())
            {
                String fs = System.getProperty("file.separator");

                String sRemovePath = aInputPath.getAbsolutePath();
                // a whole directory
                FileFilter aFileFilter = aGTA.getFileFilter();

                Object[] aList = DirectoryHelper.traverse(m_sInputPath, aGTA.getFileFilter(), aGTA.includeSubDirectories());
                // fill into DB
                DB.filesRemove(aGTA.getDBInfoString());
                for (int j=0;j<aList.length;j++)
                {
                    String sEntry = (String)aList[j];
                    DB.fileInsert(aGTA.getDBInfoString(), sEntry, sRemovePath);
                }

                // normal run.
                for (int i=0;i<aList.length;i++)
                {
                    String sEntry = (String)aList[i];

                    String sNewReferencePath = m_sReferencePath + fs + FileHelper.removeFirstDirectorysAndBasenameFrom(sEntry, m_sInputPath);
                    log.println("- next file is: ------------------------------");
                    log.println(sEntry);
                    log.println(sNewReferencePath);

                    if (aGTA.checkIfUsable(sEntry))
                    {
                        runGDC(sEntry, sNewReferencePath);
                    }
                }
            }
            else
            {
                if (aGTA.checkIfUsable(m_sInputPath))
                {
                    runGDC(m_sInputPath, m_sReferencePath);
                }
            }
        }

    void runGDC(String _sInputPath, String _sReferencePath)
        {
            // first do a check if the reference not already exist, this is a big speedup, due to the fact,
            // we don't need to start a new office.
            GraphicalTestArguments aGTA = getGraphicalTestArguments();
            if (GraphicalDifferenceCheck.isReferenceExists(_sInputPath, _sReferencePath, aGTA) == false)
            {
            // start a fresh Office
                OfficeProvider aProvider = null;
                if (aGTA.shouldOfficeStart())
                {
                    aGTA.getPerformance().startTime(PerformanceContainer.OfficeStart);
                    aProvider = new OfficeProvider();
                    XMultiServiceFactory xMSF = (XMultiServiceFactory) aProvider.getManager(param);
                    param.put("ServiceFactory", xMSF);
                    aGTA.getPerformance().stopTime(PerformanceContainer.OfficeStart);

                    long nStartTime = aGTA.getPerformance().getTime(PerformanceContainer.OfficeStart);
                    aGTA = getGraphicalTestArguments();
                    aGTA.getPerformance().setTime(PerformanceContainer.OfficeStart, nStartTime);
                }

                try
                {
                    log.println("Reference type is " + aGTA.getReferenceType());
                    DB.startFile(aGTA.getDBInfoString(), _sInputPath);
                    GraphicalDifferenceCheck.createOneReferenceFile(_sInputPath, _sReferencePath, aGTA);
                    DB.ref_finishedFile(aGTA.getDBInfoString(), _sInputPath);
                }
                catch(ConvWatchCancelException e)
                {
                    assure(e.getMessage(), false);
                    DB.ref_failedFile(aGTA.getDBInfoString(), _sInputPath);
                }
                catch(ConvWatchException e)
                {
                    assure(e.getMessage(), false);
                    DB.ref_failedFile(aGTA.getDBInfoString(), _sInputPath);
                }

                // Office shutdown
                if (aProvider != null)
                {
                    aProvider.closeExistingOffice(param, true);
                }
            }
            else
            {
                DB.ref_finishedFile(aGTA.getDBInfoString(), _sInputPath);
            }
        }
}

