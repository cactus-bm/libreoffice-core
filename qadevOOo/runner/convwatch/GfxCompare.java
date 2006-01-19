/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: GfxCompare.java,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2006-01-19 14:18:28 $
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

import java.util.ArrayList;
import convwatch.EnhancedComplexTestCase;
import convwatch.PRNCompare;
import convwatch.GraphicalTestArguments;
import lib.TestParameters;
import helper.URLHelper;
import convwatch.OfficePrint;
import java.io.File;

public class GfxCompare extends EnhancedComplexTestCase
{
    // The first of the mandatory functions:
    /**
     * Return the name of the test.
     * In this case it is the actual name of the service.
     * @return The tested service.
     */
    // public String getTestObjectName() {
    //     return "ConvWatch runner";
    // }

    // The second of the mandatory functions: return all test methods as an
    // array. There is only one test function in this example.
    /**
     * Return all test methods.
     * @return The test methods.
     */

    public String[] getTestMethodNames() {
        return new String[]{"gfxcompare"};
    }

    /**
     *
     * @return a List of software which must accessable as an external executable
     */
    protected Object[] mustInstalledSoftware()
        {
            ArrayList aList = new ArrayList();
            // Tools from ImageMagick
            aList.add( "composite -version" );
            aList.add( "identify -version" );

            // Ghostscript
            aList.add( "gs -version" );
            return aList.toArray();
        }


    GraphicalTestArguments m_aArguments = null;
    /**
     * The test method itself.
     * Don't try to call it from outside, it is started only from qadevOOo runner
     */

    /* protected */
    public void gfxcompare()
        {
            GlobalLogWriter.set(log);

            // check if all need software is installed and accessable
            checkEnvironment(mustInstalledSoftware());

            m_aArguments = getGraphicalTestArguments();

            String sFile1 = (String)param.get("FILE1");
            String sFile2 = (String)param.get("FILE2");
            compare(sFile1, sFile2);
        }

    // -----------------------------------------------------------------------------

    String createJPEG(String _sFile, String _sAdditional)
        {
            String sJPEGFile = "";
            if (_sFile.startsWith("file:///"))
            {
                _sFile = FileHelper.getSystemPathFromFileURL(_sFile);
            }
            File aFile = new File(_sFile);
            if (aFile.exists())
            {
                String sAbsFile = aFile.getAbsolutePath();
                if (!sAbsFile.equals(_sFile))
                {
                    _sFile = sAbsFile;
                }
            }
            else
            {
                GlobalLogWriter.get().println("File: '" + _sFile + "' doesn't exist.");
                return "";
            }
            String sFileDir = FileHelper.getPath(_sFile);
            String sBasename = FileHelper.getBasename(_sFile);
            String sNameNoSuffix = FileHelper.getNameNoSuffix(sBasename);

            String fs = System.getProperty("file.separator");
            String sTmpDir = util.utils.getUsersTempDir();
            if (m_aArguments.getOutputPath() != null)
            {
                sTmpDir = m_aArguments.getOutputPath();
            }

            if (_sFile.toLowerCase().endsWith("ps") ||
                _sFile.toLowerCase().endsWith("prn") ||
                _sFile.toLowerCase().endsWith("pdf"))
            {
                // seems to be a Postscript of PDF file

                String[] aList = PRNCompare.createJPEGFromPostscript(sTmpDir, sFileDir, sBasename, m_aArguments.getResolutionInDPI());
                sJPEGFile = aList[0];
            }
            else if (_sFile.toLowerCase().endsWith("jpg") ||
                     _sFile.toLowerCase().endsWith("jpeg"))
            {
                // do nothing, it's already a picture.
                return _sFile;
            }
            else
            {
                // we assume it's an office document.
                String sInputURL;
                String sOutputURL;
                String sPrintFileURL;

                String sInputFile = sFileDir + fs + sBasename;
                sInputURL = URLHelper.getFileURLFromSystemPath(sInputFile);

                String sOutputFile = sTmpDir + fs + sBasename;
                sOutputURL = URLHelper.getFileURLFromSystemPath(sOutputFile);

                String sPrintFile = sTmpDir + fs + sNameNoSuffix + _sAdditional + ".ps";
                sPrintFileURL = URLHelper.getFileURLFromSystemPath(sPrintFile);

                try
                {
                    OfficePrint.printToFile(m_aArguments, sInputURL, sOutputURL, sPrintFileURL);
                    sJPEGFile = createJPEG(sPrintFile, _sAdditional);
                }
                catch (ConvWatchCancelException e)
                {
                    GlobalLogWriter.get().println("Exception caught, can't create:" + sPrintFileURL);
                }
            }
            return sJPEGFile;
        }


    public String compare(String _sFile1, String _sFile2)
        {
            String sJPEGFile1 = createJPEG(_sFile1, "-1");
            String sJPEGFile2 = createJPEG(_sFile2, "-2");

            if (sJPEGFile1.length() > 0 && sJPEGFile2.length() > 0)
            {
                String sDiffFile = PRNCompare.compareJPEGs(sJPEGFile1, sJPEGFile2);

                if (sDiffFile.length() > 0)
                {
                    GlobalLogWriter.get().println("Difference created: " + sDiffFile);
                }
                return sDiffFile;
            }
            return "";
        }

}
