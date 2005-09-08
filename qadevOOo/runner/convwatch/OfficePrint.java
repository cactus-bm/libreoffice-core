/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: OfficePrint.java,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 17:15:16 $
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

import java.io.File;
import java.io.FileWriter;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.bridge.XUnoUrlResolver;
import com.sun.star.uno.XComponentContext;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.document.XTypeDetection;
import com.sun.star.container.XNameAccess;
import com.sun.star.frame.XDesktop;
import com.sun.star.beans.PropertyValue;
import com.sun.star.frame.XComponentLoader;
import com.sun.star.lang.XComponent;
import com.sun.star.frame.XStorable;
import com.sun.star.view.XPrintable;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.frame.XModel;

import helper.URLHelper;
import convwatch.FileHelper;
import convwatch.MSOfficePrint;
import convwatch.GraphicalTestArguments;
import convwatch.ConvWatchCancelException;

// import helper.Parameter;

/**
 * This Object is to print a given document with OpenOffice.org / StarOffice
 * over the normal printer driver
 * or over it's pdf exporter
 */
public class OfficePrint {


    static void waitInSeconds(int _nSeconds)
        {
            try {
                java.lang.Thread.sleep(_nSeconds * 1000);
            } catch (java.lang.InterruptedException e2) {}
        }

    /**
     * shows the FilterName and MediaType from the given XComponent
     */
    static void showDocumentType( XComponent _aDoc )
        {
            XModel xModel = (XModel) UnoRuntime.queryInterface( XModel.class, _aDoc);
            PropertyValue[] aArgs = xModel.getArgs();
            for (int i=0;i<aArgs.length;i++)
            {
                PropertyValue aValue = aArgs[i];
                // System.out.print("Property: '" + aValue.Name);
                // System.out.println("' := '" + aValue.Value + "'");
                if (aValue.Name.equals("FilterName") ||
                    aValue.Name.equals("MediaType"))
                {
                    System.out.println("  Property: '" + aValue.Name + "' := '" + aValue.Value + "'");
                }
            }
        }
    /**
     * load a OpenOffice.org document from a given URL (_sInputURL)
     * the GraphicalTestArguments must contain a living MultiServiceFactory object
     * or we crash here.
     * Be aware, the ownership of the document gets to you, you have to close it.
     */
    public static XComponent loadFromURL(GraphicalTestArguments _aGTA,
                                         String _sInputURL)
        {
            XComponent aDoc = null;
            try
            {
                if (_aGTA.getMultiServiceFactory() == null)
                {
                    System.out.println("MultiServiceFactory in GraphicalTestArgument not set.");
                    return null;
                }
                Object oDsk = _aGTA.getMultiServiceFactory().createInstance("com.sun.star.frame.Desktop");
                XDesktop aDesktop = (XDesktop)UnoRuntime.queryInterface(XDesktop.class, oDsk);

                if (aDesktop != null)
                {
                    System.out.println("com.sun.star.frame.Desktop created.");
                    // String sInputURL = aCurrentParameter.sInputURL;
                    // String sOutputURL = aCurrentParameter.sOutputURL;
                    // String sPrintFileURL = aCurrentParameter.sPrintToFileURL;
                    // System.out.println(_sInputURL);


                    // set here the loadComponentFromURL() properties
                    // at the moment only 'Hidden' is set, so no window is opened at work
                    PropertyValue [] aProps;
                    int nPropertyCount = 0;

                    // check which properties should set and count it.
                    if (_aGTA.isHidden())
                    {
                        nPropertyCount ++;
                    }
                    if (_aGTA.getImportFilterName() != null && _aGTA.getImportFilterName().length() > 0)
                    {
                        nPropertyCount ++;
                    }

                    // initialize the propertyvalue
                    int nPropertyIndex = 0;
                    aProps = new PropertyValue[ nPropertyCount ];

                    // set all property values
                    if (_aGTA.isHidden())
                    {
                        PropertyValue Arg = new PropertyValue();
                        Arg.Name = "Hidden";
                        Arg.Value = Boolean.TRUE;
                        aProps[ nPropertyIndex ++ ] = Arg;
                    }
                    if (_aGTA.getImportFilterName() != null && _aGTA.getImportFilterName().length() > 0)
                    {
                        PropertyValue Arg = new PropertyValue();
                        Arg.Name = "FilterName";
                        Arg.Value = _aGTA.getImportFilterName();
                        aProps[ nPropertyIndex ++ ] = Arg;
                    }

                    System.out.print("Load document");
                    System.out.flush();

                    XComponentLoader aCompLoader = (XComponentLoader) UnoRuntime.queryInterface( XComponentLoader.class, aDesktop);

                    // XComponent aDoc = null;

                    aDoc = aCompLoader.loadComponentFromURL(_sInputURL, "_blank", 0, aProps );
                    if (aDoc != null)
                    {
                        System.out.println(" done.");
                        showDocumentType(aDoc);
                    }
                    else
                    {
                        System.out.print(" failed.");
                        if (_aGTA.getImportFilterName() != null && _aGTA.getImportFilterName().length() > 0)
                        {
                            System.out.print(" Please check FilterName := '" + _aGTA.getImportFilterName() + "'");
                        }
                        System.out.println();
                    }
                }
                else
                {
                    System.out.println("com.sun.star.frame.Desktop failed.");
                }
            }
            catch ( com.sun.star.uno.Exception e )
            {
                // Some exception occures.FAILED
                e.printStackTrace();
                aDoc = null;
            }
            return aDoc;
        }

    static boolean exportToPDF(XComponent _xComponent, String _sDestinationName)
        {
            XServiceInfo xServiceInfo =
                (XServiceInfo) UnoRuntime.queryInterface(
                    XServiceInfo.class, _xComponent
                    );

            PropertyValue[] PDFArgs = new com.sun.star.beans.PropertyValue[1];
            PDFArgs[0] = new com.sun.star.beans.PropertyValue();
            PDFArgs[0].Name = "FilterName";
            PDFArgs[0].Value = getFilterName_forPDF(xServiceInfo);

            boolean bWorked = true;

            try
            {
                XStorable store =
                    (XStorable) UnoRuntime.queryInterface(
                        XStorable.class, _xComponent
                        );
                store.storeToURL(_sDestinationName, PDFArgs);
            }
            catch (com.sun.star.io.IOException e)
            {
                bWorked = false;
            }

            return bWorked;
        }

    static String getFilterName_forPDF(XServiceInfo xServiceInfo)
        {
            String filterName = "";

            if (xServiceInfo.supportsService("com.sun.star.text.TextDocument"))
            {
                //writer
                filterName = "writer_pdf_Export";
            }
            else if ( xServiceInfo.supportsService( "com.sun.star.sheet.SpreadsheetDocument" ) )
            {
                //calc
                filterName = "calc_pdf_Export";
            }
            else if ( xServiceInfo.supportsService( "com.sun.star.drawing.DrawingDocument" ) )
            {
                //draw
                filterName = "draw_pdf_Export";
            }
            else if ( xServiceInfo.supportsService( "com.sun.star.presentation.PresentationDocument" ) )
            {
                //impress
                filterName = "impress_pdf_Export";
            }
            else if (xServiceInfo.supportsService("com.sun.star.text.WebDocument"))
            {
                //html document
                filterName = "writer_web_pdf_Export";
            }
            else if ( xServiceInfo.supportsService("com.sun.star.text.GlobalDocument") )
            {
                //master document
                filterName = "writer_globaldocument_pdf_Export";
            }
            else if ( xServiceInfo.supportsService( "com.sun.star.formulaFormulaProperties" ) )
            {
                //math document
                filterName = "math_pdf_Export";
            }

            return filterName;
        }

    // -----------------------------------------------------------------------------

    public static boolean storeAsPDF(GraphicalTestArguments _aGTA,
                                     String _sInputURL,
                                     String _sOutputURL)
        {
            boolean bBack = false;
            XComponent aDoc = loadFromURL(_aGTA, _sInputURL);

            if (aDoc == null)
            {
                System.out.println("Can't load document.");
                return bBack;
            }
            bBack = storeAsPDF(_aGTA, aDoc, _sOutputURL);

            System.out.println("close document.");
            aDoc.dispose();
            return bBack;
        }

    public static boolean storeAsPDF(GraphicalTestArguments _aGTA,
                                     XComponent _aDoc,
                                     String _sOutputURL)
        {
            // try {
            boolean bBack = true;
            if (!exportToPDF(_aDoc, _sOutputURL))
            {
                System.out.println("Can't store document as PDF.");
                bBack = false;
            }
            return bBack;
        }

    // -----------------------------------------------------------------------------

    /**
     * print the document found in file (_sInputURL) to as postscript to file (_sPrintFileURL)
     * Due to the fact we use a printer to convert the file to postscript, the default printer
     * to create such postscript format must be installed, this is not tested here.
     *
     * @return true, if print has been done.
     *         Be careful, true means only print returns with no errors, to be sure print is really done
     *         check existance of _sPrintFileURL
     */

    public static boolean printToFileWithOOo(GraphicalTestArguments _aGTA,
                                             String _sInputURL,
                                             String _sOutputURL,
                                             String _sPrintFileURL)
        {
            // waitInSeconds(1);
            boolean bBack = false;

            XComponent aDoc = loadFromURL(_aGTA, _sInputURL);
            if (aDoc != null)
            {
                if ( _sInputURL.equals(_sOutputURL) )
                {
                    // don't store document
                    // input and output are equal OR
                    System.out.println("Warning: Inputpath and Outputpath are equal. Document will not stored again.");
                    _aGTA.disallowStore();
                }
                    bBack = printToFileWithOOo(_aGTA, aDoc, _sOutputURL, _sPrintFileURL);

                System.out.println("close document.");
                aDoc.dispose();
            }
            else
            {
                System.out.println("loadDocumentFromURL() failed with document: " + _sInputURL);
            }
            return bBack;
        }

    // -----------------------------------------------------------------------------
    public static void createInfoFile(String _sFile, GraphicalTestArguments _aGTA)
        {
            String sFilename = _sFile;
            if (_sFile.startsWith("file:///"))
            {
                sFilename = FileHelper.getSystemPathFromFileURL(_sFile);
                System.out.println("CreateInfoFile: '" + sFilename + "'" );
            }
            else
            {
                sFilename = _sFile;
            }
            String sFileDir = FileHelper.getPath(sFilename);
            String sBasename = FileHelper.getBasename(sFilename);
            String sNameNoSuffix = FileHelper.getNameNoSuffix(sBasename);

            String fs = System.getProperty("file.separator");
            String ls = System.getProperty("line.separator");
            String sInfoFilename = sFileDir + fs + sNameNoSuffix + ".info";
            File aInfoFile = new File(sInfoFilename);
            try
            {
                FileWriter out = new FileWriter(aInfoFile.toString());
                out.write("# automatically created file by graphical compare" + ls);
                out.write("buildid=" + _aGTA.getBuildID() + ls);
                out.flush();
                out.close();
            }
            catch (java.io.IOException e)
            {
                System.out.println("can't create Info file.");
                e.printStackTrace();
            }
        }
    // -----------------------------------------------------------------------------
    public static boolean printToFileWithOOo(GraphicalTestArguments _aGTA,
                                             XComponent _aDoc,
                                             String _sOutputURL,
                                             String _sPrintFileURL)
        {
            boolean bBack = false;
            boolean bFailed = true;              // always be a pessimist,
            if (_aDoc == null)
            {
                System.out.println("No document is given.");
                return bBack;
            }

            try {

                // System.out.println("Document loaded.");
                // Change Pagesettings to DIN A4

                System.out.println("Document print.");
                XPrintable aPrintable = (XPrintable) UnoRuntime.queryInterface( XPrintable.class, _aDoc);
                if (aPrintable != null)
                {
                    // System.out.println("  Set PaperFormat to DIN A4");
                    // {
                    //     PropertyValue[] aPrinterProps = aPrintable.getPrinter();
                    //     System.out.println("PrinterProps size: " + String.valueOf(aPrinterProps.length));
                    //     int nPropIndex = 0;
                    //     while (!"PaperFormat".equals(aPrinterProps[nPropIndex].Name))
                    //     {
                    //         // System.out.println(aPrinterProps[nPropIndex].Name);
                    //         nPropIndex++;
                    //     }
                    //     aPrinterProps[nPropIndex].Value = com.sun.star.view.PaperFormat.A4;
                    //     aPrintable.setPrinter(aPrinterProps);
                    // }

                    // configure Office to allow to execute macos

// TODO: We need a possiblity to set the printer name also for StarOffice/OpenOffice
                    if (OSHelper.isWindows())
                    {
                        if (_aGTA.getPrinterName() != null)
                        {
                            PropertyValue [] aPrintProps = new PropertyValue[1];
                            PropertyValue Arg = new PropertyValue();
                            Arg.Name = "Name";
                            Arg.Value = _aGTA.getPrinterName();
                            aPrintProps[0] = Arg;

                            System.out.println("Printername is not null, so set to " + _aGTA.getPrinterName());
                            aPrintable.setPrinter(aPrintProps);
                        }
                    }

                    // set property values for XPrintable.print()
                    // more can be found at "http://api.openoffice.org/docs/common/ref/com/sun/star/view/PrintOptions.html"

                    int nProperties = 1;                    // default for 'FileName' property
                    if (_aGTA.printAllPages() == false)
                    {
                        // we don't want to print all pages, build Pages string by ourself
                        nProperties ++;
                    }
                    int nPropsCount = 0;

                    PropertyValue [] aPrintProps = new PropertyValue[nProperties];
                    PropertyValue Arg = new PropertyValue();
                    Arg.Name = "FileName";
                    Arg.Value = _sPrintFileURL;
                    aPrintProps[nPropsCount ++] = Arg;

                    if (_aGTA.printAllPages() == false)
                    {
                        String sPages = "";
                        if (_aGTA.getMaxPages() > 0)
                        {
                            sPages = "1-" + String.valueOf(_aGTA.getMaxPages());
                        }
                        if (_aGTA.getOnlyPages().length() != 0)
                        {
                            if (sPages.length() != 0)
                            {
                                sPages += ";";
                            }
                            sPages += String.valueOf(_aGTA.getOnlyPages());
                        }

                        Arg = new PropertyValue();
                        Arg.Name = "Pages";
                        Arg.Value = sPages;
                        aPrintProps[nPropsCount ++] = Arg;
                    }

                    System.out.println("start printing.");
                    aPrintable.print(aPrintProps);
                    waitInSeconds(1);

                    if (_sOutputURL != null)
                    {
                        if (_aGTA.isStoreAllowed())
                        {
                        // store the document in an other directory
                        XStorable aStorable = (XStorable) UnoRuntime.queryInterface( XStorable.class, _aDoc);
                        if (aStorable != null)
                        {
                            PropertyValue [] szEmptyArgs = new PropertyValue [0];

                            System.out.println("Document stored.");
                            aStorable.storeAsURL(_sOutputURL, szEmptyArgs);
                        }
                    }
                    }

                    System.out.println("Wait until document is printed.");
                    boolean isBusy = true;
                    while (isBusy)
                    {
                        PropertyValue[] aPrinterProps = aPrintable.getPrinter();
                        int nPropIndex = 0;
                        while (!"IsBusy".equals(aPrinterProps[nPropIndex].Name))
                        {
                            // System.out.println(aPrinterProps[nPropIndex].Name);
                            nPropIndex++;
                        }
                        isBusy = (aPrinterProps[nPropIndex].Value == Boolean.TRUE) ? true : false;
                        waitInSeconds(1);
                    }

                    // Create a .info file near the printed '.ps' or '.prn' file.
                    createInfoFile(_sPrintFileURL, _aGTA);
                }
                else
                {
                    System.out.println("Can't get XPrintable.");
                }
                bFailed = false;
                bBack = true;
            }
            catch ( com.sun.star.uno.Exception e )
            {
                // Some exception occures.FAILED
                e.printStackTrace();
                bBack = false;
            }

            if (bFailed == true)
            {
                System.out.println("convwatch.OfficePrint: FAILED");
            }
            else
            {
                System.out.println("convwatch.OfficePrint: OK");
            }
            return bBack;
        }


    /**
     * @return true, if the reference (*.prrn file) based on given output path and given input path exist.
     *               If OVERWRITE_REFERENCE is set, always return false.
     */
    public static boolean isReferenceExists(GraphicalTestArguments _aGTA,
                                            String _sAbsoluteOutputPath,
                                            String _sAbsoluteInputFile)
        {
            if (! FileHelper.exists(_sAbsoluteInputFile))
            {
                // throw new ConvWatchCancelException("Input file: " + _sAbsoluteInputFile + " does not exist.");
                return false;
            }

            String fs = System.getProperty("file.separator");

            // String sInputFileURL = URLHelper.getFileURLFromSystemPath(_sAbsoluteInputFile);

            String sInputFileBasename = FileHelper.getBasename(_sAbsoluteInputFile);
            // String sOutputFileURL = null;
            String sOutputPath;
            if (_sAbsoluteOutputPath != null)
            {
                sOutputPath    = _sAbsoluteOutputPath;
                // FileHelper.makeDirectories("", sOutputPath);
            }
            else
            {
                String sInputPath = FileHelper.getPath(_sAbsoluteInputFile);
                sOutputPath    = sInputPath;
            }
            // sOutputFileURL = URLHelper.getFileURLFromSystemPath(sOutputPath + fs + sInputFileBasename);
            // sOutputFileURL = null;

            String sPrintFilename = FileHelper.getNameNoSuffix(sInputFileBasename);
            // String sPrintFileURL;

            String sAbsolutePrintFilename = sOutputPath + fs + sPrintFilename + ".prn";
            if (FileHelper.exists(sAbsolutePrintFilename) && _aGTA.getOverwrite() == false)
            {
                System.out.println("Reference already exist, don't overwrite. Set " + PropertyName.DOC_COMPARATOR_OVERWRITE_REFERENCE + "=true to force overwrite.");
                return true;
            }
            return false;
        }

    // -----------------------------------------------------------------------------
    /**
     * create a reference file
     * _sAbsoluteInputPath  contains the source file, if not exists, return with failure.
     * _sAbsoluteOutputPath contains the destination, where the file will store after load with StarOffice/OpenOffice.org
     *                      if is null, print only near the Input file path
     * _sPrintType ".prn" Print input file with StarOffice/OpenOffice.org and the default printer as PostScript
     *
     */
    public static boolean buildReference(GraphicalTestArguments _aGTA,
                                         String _sAbsoluteOutputPath,
                                         String _sAbsoluteInputFile)
        throws ConvWatchCancelException
        {
            if (! FileHelper.exists(_sAbsoluteInputFile))
            {
                throw new ConvWatchCancelException("buildReference(): Input file: " + _sAbsoluteInputFile + " does not exist.");
            }

            String fs = System.getProperty("file.separator");

            String sInputFileURL = URLHelper.getFileURLFromSystemPath(_sAbsoluteInputFile);

            String sInputFileBasename = FileHelper.getBasename(_sAbsoluteInputFile);
            String sOutputFileURL = null;
            String sOutputPath;
            if (_sAbsoluteOutputPath != null)
            {
                sOutputPath    = _sAbsoluteOutputPath;
                FileHelper.makeDirectories("", sOutputPath);
            }
            else
            {
                String sInputPath = FileHelper.getPath(_sAbsoluteInputFile);
                sOutputPath    = sInputPath;
            }
            // sOutputFileURL = URLHelper.getFileURLFromSystemPath(sOutputPath + fs + sInputFileBasename);
            sOutputFileURL = null;

            String sPrintFilename = FileHelper.getNameNoSuffix(sInputFileBasename);
            String sPrintFileURL;

            String sAbsolutePrintFilename = sOutputPath + fs + sPrintFilename + ".prn";
            if (FileHelper.exists(sAbsolutePrintFilename) && _aGTA.getOverwrite() == false)
            {
                System.out.println("Reference already exist, don't overwrite. Set " + PropertyName.DOC_COMPARATOR_OVERWRITE_REFERENCE + "=true to force overwrite.");
                return true;
            }

            if (_aGTA.getReferenceType().toLowerCase().equals("msoffice"))
            {
                sPrintFileURL = URLHelper.getFileURLFromSystemPath(sAbsolutePrintFilename);
            }
            else if (_aGTA.getReferenceType().toLowerCase().equals("pdf"))
            {
//  TODO: If we rename the stored file to *.pdf, we have to be sure that we use *.pdf also as a available reference
                sPrintFileURL = URLHelper.getFileURLFromSystemPath(sAbsolutePrintFilename );
            }
            else if (_aGTA.getReferenceType().toLowerCase().equals("ooo"))
            {
                sPrintFileURL = URLHelper.getFileURLFromSystemPath(sAbsolutePrintFilename );
            }
            else
            {
                System.out.println("OfficePrint.buildreference(): Unknown print type.");
                return false;
            }
            return printToFile(_aGTA, sInputFileURL, sOutputFileURL, sPrintFileURL);
        }

    public static boolean printToFile(GraphicalTestArguments _aGTA,
                                      String _sInputFileURL,
                                      String _sOutputFileURL,
                                      String _sPrintFileURL) throws ConvWatchCancelException
        {
            boolean bBack = false;
            String sPrintFileURL = null;
            if (_aGTA.getReferenceType().toLowerCase().equals("ooo"))
            {
                bBack = printToFileWithOOo(_aGTA, _sInputFileURL, _sOutputFileURL, _sPrintFileURL);
            }
            else if (_aGTA.getReferenceType().toLowerCase().equals("pdf"))
            {
                System.out.println("USE PDF AS EXPORT FORMAT.");
                bBack = storeAsPDF(_aGTA, _sInputFileURL, _sPrintFileURL);
            }
            else if (_aGTA.getReferenceType().toLowerCase().equals("msoffice"))
            {
                if (MSOfficePrint.isMSOfficeDocumentFormat(_sInputFileURL))
                {
                    System.out.println("USE MSOFFICE AS EXPORT FORMAT.");
                    MSOfficePrint a = new MSOfficePrint();
                    try
                    {
                        a.printToFileWithMSOffice(_aGTA, FileHelper.getSystemPathFromFileURL(_sInputFileURL),
                                                  FileHelper.getSystemPathFromFileURL(_sPrintFileURL));
                    }
                    catch(ConvWatchCancelException e)
                    {
                        e.printStackTrace();
                        System.out.println(e.getMessage());
                        throw new ConvWatchCancelException("Exception caught. Problem with MSOffice printer methods.");
                    }
                    catch(java.io.IOException e)
                    {
                        System.out.println(e.getMessage());
                        throw new ConvWatchCancelException("IOException caught. Problem with MSOffice printer methods.");
                    }
                    bBack = true;
                }
                else
                {
                    System.out.println("This document type is not recognized as MSOffice format, as default fallback StarOffice/OpenOffice.org instead is used.");
                    bBack = printToFileWithOOo(_aGTA, _sInputFileURL, _sOutputFileURL, _sPrintFileURL);
                }
            }
            else
            {
                // System.out.println("");
                throw new ConvWatchCancelException("OfficePrint.printToFile(): Unknown print type.");
            }
            return bBack;
        }

    // -----------------------------------------------------------------------------
    // TODO: move this away!
    // -----------------------------------------------------------------------------
    static void showType(String _sInputURL, XMultiServiceFactory _xMSF)
        {
            if (_sInputURL.length() == 0)
            {
                return;
            }

            if (_xMSF == null)
            {
                System.out.println("MultiServiceFactory not set.");
                return;
            }
            XTypeDetection aTypeDetection = null;
            try
            {
                Object oObj = _xMSF.createInstance("com.sun.star.document.TypeDetection");
                aTypeDetection = (XTypeDetection)UnoRuntime.queryInterface(XTypeDetection.class, oObj);
            }
            catch(com.sun.star.uno.Exception e)
            {
                System.out.println("Can't get com.sun.star.document.TypeDetection.");
                return;
            }
            if (aTypeDetection != null)
            {
                String sType = aTypeDetection.queryTypeByURL(_sInputURL);
                System.out.println("Type is: " + sType);
            }
        }


    // -----------------------------------------------------------------------------
    public static String getInternalFilterName(String _sFilterName, XMultiServiceFactory _xMSF)
        {
            if (_sFilterName.length() == 0)
            {
                // System.out.println("No FilterName set.");
                return null;
            }

            if (_xMSF == null)
            {
                System.out.println("MultiServiceFactory not set.");
                return null;
            }
            // XFilterFactory aFilterFactory = null;
            Object aObj = null;
            try
            {
                aObj = _xMSF.createInstance("com.sun.star.document.FilterFactory");
            }
            catch(com.sun.star.uno.Exception e)
            {
                System.out.println("Can't get com.sun.star.document.FilterFactory.");
                return null;
            }
            if (aObj != null)
            {
                XNameAccess aNameAccess = (XNameAccess)UnoRuntime.queryInterface(XNameAccess.class, aObj);
                if (aNameAccess != null)
                {

                    // if (_sFilterName.toLowerCase().equals("help"))
                    // {
                    //     System.out.println("Show all possible ElementNames from current version." );
                    // String[] aElementNames = aNameAccess.getElementNames();
                    // for (int i = 0; i<aElementNames.length; i++)
                    // {
                    //     System.out.println(aElementNames[i]);
                    // }
                    //     System.out.println("Must quit.");
                    //     System.out.exit(1);
                    // }

                    if (! aNameAccess.hasByName(_sFilterName))
                    {
                        System.out.println("FilterFactory.hasByName() says there exist no '" + _sFilterName + "'" );
                        return null;
                    }

                    Object[] aElements = null;
                    String[] aExtensions;
                    try
                    {
                        aElements = (Object[]) aNameAccess.getByName(_sFilterName);
                        if (aElements != null)
                        {
                            String sInternalFilterName = null;
                            // System.out.println("getByName().length: " + String.valueOf(aElements.length));
                            for (int i=0;i<aElements.length; i++)
                            {
                                PropertyValue aPropertyValue = (PropertyValue)aElements[i];
                                // System.out.println("PropertyValue.Name: " + aPropertyValue.Name);
                                if (aPropertyValue.Name.equals("Type"))
                                {
                                    String sValue = (String)aPropertyValue.Value;
                                    // System.out.println("Type: " + sValue);
                                    sInternalFilterName = sValue;
                                }
                            }
                            return sInternalFilterName;
                        }
                        else
                        {
                            System.out.println("There are no elements for FilterName '" + _sFilterName + "'");
                            return null;
                        }
                    }
                    catch (com.sun.star.container.NoSuchElementException e)
                    {
                        System.out.println("NoSuchElementException caught. " + e.getMessage());
                    }
                    catch (com.sun.star.lang.WrappedTargetException e)
                    {
                        System.out.println("WrappedTargetException caught. " + e.getMessage());
                    }
                }
            }
            return null;
        }

    // -----------------------------------------------------------------------------

    static String getServiceNameFromFilterName(String _sFilterName, XMultiServiceFactory _xMSF)
        {
            if (_sFilterName.length() == 0)
            {
                // System.out.println("No FilterName set.");
                return null;
            }

            if (_xMSF == null)
            {
                System.out.println("MultiServiceFactory not set.");
                return null;
            }
            // XFilterFactory aFilterFactory = null;
            Object aObj = null;
            try
            {
                aObj = _xMSF.createInstance("com.sun.star.document.FilterFactory");
            }
            catch(com.sun.star.uno.Exception e)
            {
                System.out.println("Can't get com.sun.star.document.FilterFactory.");
                return null;
            }
            if (aObj != null)
            {
                XNameAccess aNameAccess = (XNameAccess)UnoRuntime.queryInterface(XNameAccess.class, aObj);
                if (aNameAccess != null)
                {
                    if (! aNameAccess.hasByName(_sFilterName))
                    {
                        System.out.println("FilterFactory.hasByName() says there exist no '" + _sFilterName + "'" );
                        return null;
                    }

                    Object[] aElements = null;
                    String[] aExtensions;
                    try
                    {
                        aElements = (Object[]) aNameAccess.getByName(_sFilterName);
                        if (aElements != null)
                        {
                            String sServiceName = null;
                            // System.out.println("getByName().length: " + String.valueOf(aElements.length));
                            for (int i=0;i<aElements.length; i++)
                            {
                                PropertyValue aPropertyValue = (PropertyValue)aElements[i];
                                if (aPropertyValue.Name.equals("DocumentService"))
                                {
                                    String sValue = (String)aPropertyValue.Value;
                                    // System.out.println("DocumentService: " + sValue);
                                    sServiceName = sValue;
                                    break;
                                }
                            }
                            return sServiceName;
                        }
                        else
                        {
                            System.out.println("There are no elements for FilterName '" + _sFilterName + "'");
                            return null;
                        }
                    }
                    catch (com.sun.star.container.NoSuchElementException e)
                    {
                        System.out.println("NoSuchElementException caught. " + e.getMessage());
                    }
                    catch (com.sun.star.lang.WrappedTargetException e)
                    {
                        System.out.println("WrappedTargetException caught. " + e.getMessage());
                    }
                }
            }
            return null;
        }
    // -----------------------------------------------------------------------------

    public static String getFileExtension(String _sInternalFilterName, XMultiServiceFactory _xMSF)
        {
            if (_sInternalFilterName.length() == 0)
            {
                // System.out.println("No FilterName set.");
                return null;
            }

            if (_xMSF == null)
            {
                System.out.println("MultiServiceFactory not set.");
                return null;
            }
            XTypeDetection aTypeDetection = null;
            try
            {
                Object oObj = _xMSF.createInstance("com.sun.star.document.TypeDetection");
                aTypeDetection = (XTypeDetection)UnoRuntime.queryInterface(XTypeDetection.class, oObj);
            }
            catch(com.sun.star.uno.Exception e)
            {
                System.out.println("Can't get com.sun.star.document.TypeDetection.");
                return null;
            }
            if (aTypeDetection != null)
            {
                XNameAccess aNameAccess = (XNameAccess)UnoRuntime.queryInterface(XNameAccess.class, aTypeDetection);
                if (aNameAccess != null)
                {

                    // System.out.println("Show ElementNames" );
                    // String[] aElementNames = aNameAccess.getElementNames();
                    // for (int i = 0; i<aElementNames.length; i++)
                    // {
                    //     System.out.println(aElementNames[i]);
                    // }

                    if (! aNameAccess.hasByName(_sInternalFilterName))
                    {
                        System.out.println("TypeDetection.hasByName() says there exist no '" + _sInternalFilterName + "'" );
                        return null;
                    }

                    Object[] aElements = null;
                    String[] aExtensions;
                    try
                    {
                        aElements = (Object[]) aNameAccess.getByName(_sInternalFilterName);
                        if (aElements != null)
                        {
                            String sExtension = null;
                            // System.out.println("getByName().length: " + String.valueOf(aElements.length));
                            for (int i=0;i<aElements.length; i++)
                            {
                                PropertyValue aPropertyValue = (PropertyValue)aElements[i];
                                // System.out.println("PropertyValue.Name: " + aPropertyValue.Name);
                                if (aPropertyValue.Name.equals("Extensions"))
                                {
                                    aExtensions = (String[])aPropertyValue.Value;
                                    System.out.print("   Possible extensions are: " + String.valueOf(aExtensions.length));
                                    if (aExtensions.length > 0)
                                    {
                                        for (int j=0;j<aExtensions.length;j++)
                                        {
                                            System.out.print(" " + aExtensions[j]);
                                        }
                                        sExtension = aExtensions[0];
                                        System.out.println();
                                    }
                                }
                            }
                            return sExtension;
                        }
                        else
                        {
                            System.out.println("There are no elements for FilterName '" + _sInternalFilterName + "'");
                            return null;
                        }
                    }
                    catch (com.sun.star.container.NoSuchElementException e)
                    {
                        System.out.println("NoSuchElementException caught. " + e.getMessage());
                    }
                    catch (com.sun.star.lang.WrappedTargetException e)
                    {
                        System.out.println("WrappedTargetException caught. " + e.getMessage());
                    }
}
            }
            return null;
        }

    // -----------------------------------------------------------------------------
    public static void convertDocument(String _sInputFile, String _sOutputPath, GraphicalTestArguments _aGTA) throws ConvWatchCancelException
        {
            XMultiServiceFactory xMSF = _aGTA.getMultiServiceFactory();
            if (xMSF == null)
            {
                System.out.println("MultiServiceFactory in GraphicalTestArgument not set.");
                return;
            }

            String sInputURL = URLHelper.getFileURLFromSystemPath(_sInputFile);
            // showType(sInputURL, xMSF);
            XComponent aDoc = loadFromURL( _aGTA, sInputURL);
            if (aDoc == null)
            {
                System.out.println("Can't load document '"+ sInputURL + "'");
                return;
            }

            if (_sOutputPath == null)
            {
                System.out.println("Outputpath not set.");
                return;
            }

            if (! _aGTA.isStoreAllowed())
            {
                System.out.println("It's not allowed to store, check Input/Output path.");
                return;
            }
//  TODO: Do we need to wait?
            waitInSeconds(1);

            XServiceInfo xServiceInfo = (XServiceInfo) UnoRuntime.queryInterface( XServiceInfo.class, aDoc );
            // String sFilter = getFilterName_forExcel(xServiceInfo);
            // System.out.println("Filter is " + sFilter);

            // store the document in an other directory
            XStorable xStorable = (XStorable) UnoRuntime.queryInterface( XStorable.class, aDoc);
            if (xStorable == null)
            {
                System.out.println("com.sun.star.frame.XStorable is null");
                return;
            }

            String sFilterName = _aGTA.getExportFilterName();

            // check how many Properties should initialize
            int nPropertyCount = 0;
            if (sFilterName != null && sFilterName.length() > 0)
            {
                nPropertyCount ++;
            }

            // initialize PropertyArray
            PropertyValue [] aStoreProps = new PropertyValue[ nPropertyCount ];
            int nPropertyIndex = 0;

            String sExtension = "";

            if (sFilterName != null && sFilterName.length() > 0)
            {
                String sInternalFilterName = getInternalFilterName(sFilterName, xMSF);
                String sServiceName = getServiceNameFromFilterName(sFilterName, xMSF);

                System.out.println("Filter detection:");
                // check if service name from file filter is the same as from the loaded document
                boolean bServiceFailed = false;
                if (sServiceName == null || sInternalFilterName == null)
                {
                    System.out.println("Given FilterName '" + sFilterName + "' seems to be unknown.");
                    bServiceFailed = true;
                }
                if (! xServiceInfo.supportsService(sServiceName))
                {
                    System.out.println("Service from FilterName '" + sServiceName + "' is not supported by loaded document.");
                    bServiceFailed = true;
                }
                if (bServiceFailed == true)
                {
                    System.out.println("Please check '" + PropertyName.DOC_CONVERTER_EXPORT_FILTER_NAME + "' in the property file.");
                    return;
                }

                if (sInternalFilterName != null && sInternalFilterName.length() > 0)
                {
                    // get the FileExtension, by the filter name, if we don't get a file extension
                    // we assume the is also no right filter name.
                    sExtension = getFileExtension(sInternalFilterName, xMSF);
                    if (sExtension == null)
                    {
                        System.out.println("Can't found an extension for filtername, take it from the source.");
                    }
                }

                PropertyValue Arg = new PropertyValue();
                Arg.Name = "FilterName";
                Arg.Value = sFilterName;
                aStoreProps[nPropertyIndex ++] = Arg;
                System.out.println("FilterName is set to: " + sFilterName);
            }

            String sOutputURL = "";
            try
            {
                // create the new filename with the extension, which is ok to the file format
                String sInputFileBasename = FileHelper.getBasename(_sInputFile);
                // System.out.println("InputFileBasename " + sInputFileBasename);
                String sInputFileNameNoSuffix = FileHelper.getNameNoSuffix(sInputFileBasename);
                // System.out.println("InputFilename no suffix " + sInputFileNameNoSuffix);
                String fs = System.getProperty("file.separator");
                String sOutputFile = _sOutputPath;
                if (! sOutputFile.endsWith(fs))
                {
                    sOutputFile += fs;
                }
                if (sExtension != null && sExtension.length() > 0)
                {
                    sOutputFile += sInputFileNameNoSuffix + "." + sExtension;
                }
                else
                {
                    sOutputFile += sInputFileBasename;
                }

                if (FileHelper.exists(sOutputFile) && _aGTA.getOverwrite() == false)
                {
                    System.out.println("File already exist, don't overwrite. Set " + PropertyName.DOC_COMPARATOR_OVERWRITE_REFERENCE + "=true to force overwrite.");
                    return;
                }

                sOutputURL = URLHelper.getFileURLFromSystemPath(sOutputFile);

                System.out.println("Store document as '" + sOutputURL + "'");
                xStorable.storeAsURL(sOutputURL, aStoreProps);
                System.out.println("Document stored.");
            }
            catch (com.sun.star.io.IOException e)
            {
                System.out.println("Can't store document '" + sOutputURL + "'. Message is :'" + e.getMessage() + "'");
            }
//  TODO: Do we need to wait?
            waitInSeconds(1);

        }

}

