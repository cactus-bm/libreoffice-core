/*************************************************************************
 *
 *  $RCSfile: printerjob.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: pl $ $Date: 2001-07-26 16:07:49 $
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

#ifndef __SGI_STL_VECTOR
#include <vector>
#endif
#ifndef _PSPRINT_PRINTERJOB_HXX_
#include <psprint/printerjob.hxx>
#endif
#ifndef _PSPRINT_PPDPARSER_HXX_
#include <psprint/ppdparser.hxx>
#endif
#ifndef _PSPRINT_STRHELPER_HXX_
#include <psprint/strhelper.hxx>
#endif
#ifndef _PSPRINT_PRINTERINFOMANAGER_HXX_
#include <psprint/printerinfomanager.hxx>
#endif
#ifndef _PSPRINT_PRINTERUTIL_HXX_
#include <psputil.hxx>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _OSL_THREAD_H_
#include <osl/thread.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>


using namespace psp ;

// forward declaration

#define nBLOCKSIZE 0x2000

namespace psp
{

sal_Bool
AppendPS (FILE* pDst, osl::File* pSrc, sal_uChar* pBuffer,
          sal_uInt64 nBlockSize = nBLOCKSIZE)
{
    if ((pDst == NULL) || (pSrc == NULL))
        return sal_False;

    if (nBlockSize == 0)
        nBlockSize = nBLOCKSIZE;
    if (pBuffer == NULL)
        pBuffer = (sal_uChar*)alloca (nBlockSize);

    pSrc->setPos (osl_Pos_Absolut, 0);

    sal_uInt64 nIn;
    sal_uInt64 nOut;
    do
    {
        pSrc->read  (pBuffer, nBlockSize, nIn);
        if (nIn > 0)
            nOut = fwrite (pBuffer, 1, nIn, pDst);
    }
    while ((nIn > 0) && (nIn == nOut));

    return sal_True;
}

} // namespace psp

/*
 * private convenience routines for file handling
 */

osl::File*
PrinterJob::CreateSpoolFile (const rtl::OUString& rName, const rtl::OUString& rExtension)
{

    rtl::OUString aFileName = maSpoolDirName + rtl::OUString::createFromAscii ("/")
        + rName + rExtension;
    rtl::OUString aNormFileName;
#ifdef TF_FILEURL
    OSL_VERIFY( osl_File_E_None == osl::File::getFileURLFromSystemPath( aFileName, aNormFileName ) );
#else
    osl::File::normalizePath (aFileName, aNormFileName);
#endif

    osl::File* pFile = new osl::File (aNormFileName);
    pFile->open (OpenFlag_Read | OpenFlag_Write | OpenFlag_Create);
    pFile->setAttributes (aNormFileName,
                          osl_File_Attribute_OwnWrite | osl_File_Attribute_OwnRead );

    return pFile;
}

/*
 * public methods of PrinterJob: for use in PrinterGfx
 */

void
PrinterJob::GetResolution (sal_uInt32 &rDpiX, sal_uInt32 &rDpiY) const
{
    rDpiX = mnResX;
    rDpiY = mnResY;
}

void
PrinterJob::GetScale (double &rXScale, double &rYScale) const
{
    rXScale = mfXScale;
    rYScale = mfYScale;
}

sal_uInt16
PrinterJob::GetDepth () const
{
    sal_Int32 nLevel = GetPostscriptLevel();
    sal_Bool  bColor = IsColorPrinter ();

    return nLevel > 1 && bColor ? 24 : 8;
}

sal_uInt16
PrinterJob::GetPostscriptLevel (const JobData *pJobData) const
{
    sal_uInt16 nPSLevel = 2;

    if( pJobData == NULL )
        pJobData = &m_aLastJobData;

    if( pJobData->m_nPSLevel )
        nPSLevel = pJobData->m_nPSLevel;
    else
        if( pJobData->m_pParser )
            nPSLevel = pJobData->m_pParser->getLanguageLevel();

    return nPSLevel;
}

sal_Bool
PrinterJob::IsColorPrinter () const
{
    sal_Bool bColor = sal_False;

    if( m_aLastJobData.m_nColorDevice )
        bColor = m_aLastJobData.m_nColorDevice == -1 ? sal_False : sal_True;
    else if( m_aLastJobData.m_pParser )
        bColor = m_aLastJobData.m_pParser->isColorDevice() ? sal_True : sal_False;

    return bColor;
}

osl::File*
PrinterJob::GetDocumentHeader ()
{
    return mpJobHeader;
}

osl::File*
PrinterJob::GetDocumentTrailer ()
{
    return mpJobTrailer;
}

osl::File*
PrinterJob::GetCurrentPageHeader ()
{
    return maHeaderList.back();
}

osl::File*
PrinterJob::GetCurrentPageBody ()
{
    return maPageList.back();
}

/*
 * public methods of PrinterJob: the actual job / spool handling
 */

PrinterJob::PrinterJob () :
        mpJobTrailer (NULL),
        mpJobHeader (NULL)
{
}

namespace psp
{

/* check whether the given name points to a directory which is
   usable for the user */
sal_Bool
existsTmpDir (const char* pName)
{
    struct stat aFileStatus;

    if (pName == NULL)
        return sal_False;
    if (stat(pName, &aFileStatus) != 0)
        return sal_False;
    if (! S_ISDIR(aFileStatus.st_mode))
        return sal_False;

    return access(pName, W_OK | R_OK) == 0 ? sal_True : sal_False;
}

/* return the username in the given buffer */
sal_Bool
getUserName (char* pName, int nSize)
{
    struct passwd *pPWEntry;
    struct passwd  aPWEntry;
    sal_Char       pPWBuffer[256];

    sal_Bool bSuccess = sal_False;

    if (getpwuid_r(getuid(), &aPWEntry, pPWBuffer, sizeof(pPWBuffer), &pPWEntry) != 0)
        pPWEntry = NULL;

    if (pPWEntry != NULL && pPWEntry->pw_name != NULL)
    {
        sal_Int32 nLen = strlen(pPWEntry->pw_name);
        if (nLen > 0 && nLen < nSize)
        {
            memcpy (pName, pPWEntry->pw_name, nLen);
            pName[nLen] = '\0';

            bSuccess = sal_True;
        }
    }

    // wipe the passwd off the stack
    memset (pPWBuffer, 0, sizeof(pPWBuffer));

    return bSuccess;
}

/* remove all our temporary files, uses external program "rm", since
   osl functionality is inadequate */
void
removeSpoolDir (const rtl::OUString& rSpoolDir)
{
    rtl::OUString aSysPath;
#ifdef TF_FILEURL
    if( osl_File_E_None != osl::File::getSystemPathFromFileURL( rSpoolDir, aSysPath ) )
    {
        // Conversion did not work, as this is quite a dangerous action,
        // we should abort here ....
        OSL_ENSURE( 0, "psprint: couldn't remove spool directory" );
        return;
    }
#else
    osl::File::getSystemPathFromNormalizedPath (rSpoolDir, aSysPath);
#endif
    rtl::OString aSysPathByte =
        rtl::OUStringToOString (aSysPath, osl_getThreadTextEncoding());
    sal_Char  pSystem [128];
    sal_Int32 nChar = 0;

    nChar  = psp::appendStr ("rm -rf ",     pSystem);
    nChar += psp::appendStr (aSysPathByte.getStr(), pSystem + nChar);

    system (pSystem);
}

/* creates a spool directory using the environment TMPDIR, or the
   standard P_tmpdir definition (/tmp for Linux and /var/tmp for Solaris)
   leads to a directory like "/tmp/pspXXXXXX" */
rtl::OUString
createSpoolDir ()
{
    /* get a tmp directory */
    static char* pTmpDir = NULL;
    if (pTmpDir == NULL)
    {
        if (! existsTmpDir(pTmpDir = getenv("TMPDIR")))
            if (! existsTmpDir(pTmpDir = P_tmpdir))
                pTmpDir = "/tmp";
    }

    /* create a subdirectory in the tmp directory */
    char* pName = tempnam (pTmpDir, "psp");
    rtl::OUString aSubDir = rtl::OUString::createFromAscii (pName);
    rtl::OUString aUNCSubDir;
#ifdef TF_FILEURL
    osl::File::getFileURLFromSystemPath (aSubDir, aUNCSubDir);
#else
    osl::File::normalizePath (aSubDir, aUNCSubDir);
#endif
    free (pName);

    /* create directory with attributes */
    osl::Directory::create (aUNCSubDir);
    osl::File::setAttributes (aUNCSubDir, osl_File_Attribute_OwnWrite
                              | osl_File_Attribute_OwnRead
                              | osl_File_Attribute_OwnExe );

    return aUNCSubDir;
}

} // namespace psp

PrinterJob::~PrinterJob ()
{
    std::list< osl::File* >::iterator pPage;
    for (pPage = maPageList.begin(); pPage != maPageList.end(); pPage++)
    {
        //(*pPage)->remove();
        delete *pPage;
    }
    for (pPage = maHeaderList.begin(); pPage != maHeaderList.end(); pPage++)
    {
        //(*pPage)->remove();
        delete *pPage;
    }
    // mpJobHeader->remove();
    delete mpJobHeader;
    // mpJobTrailer->remove();
    delete mpJobTrailer;

    // XXX should really call osl::remove routines
    removeSpoolDir (maSpoolDirName);

    // osl::Directory::remove (maSpoolDirName);
}

namespace psp
{

// get locale invariant, 7bit clean current local time string
sal_Char*
getLocalTime(sal_Char* pBuffer, sal_uInt32 nBufSize)
{
    time_t nTime = time (NULL);
    struct tm aTime;
    struct tm *pLocalTime = localtime_r (&nTime, &aTime);

    return asctime_r(pLocalTime, pBuffer);
}

}

sal_Bool
PrinterJob::StartJob (
                      const rtl::OUString& rFileName,
                      const rtl::OUString& rJobName,
                      const rtl::OUString& rAppName,
                      const JobData& rSetupData)
{
    InitPaperSize (rSetupData);

    // create file container for document header and trailer
    maFileName = rFileName;
    maSpoolDirName = createSpoolDir ();

    rtl::OUString aExt = rtl::OUString::createFromAscii (".ps");
    mpJobHeader  = CreateSpoolFile (rtl::OUString::createFromAscii("psp_head"), aExt);
    mpJobTrailer = CreateSpoolFile (rtl::OUString::createFromAscii("psp_tail"), aExt);

    // write document header according to Document Structuring Conventions (DSC)
    WritePS (mpJobHeader, "%!PS-Adobe-3.0\n");

    // BoundingBox
    sal_Char  pBBox [256];
    sal_Int32 nChar = 0;

    nChar  = psp::appendStr  ("%%BoundingBox: ",    pBBox);
    nChar += psp::getValueOf (0,                    pBBox + nChar);
    nChar += psp::appendStr  (" ",                  pBBox + nChar);
    nChar += psp::getValueOf (0,                    pBBox + nChar);
    nChar += psp::appendStr  (" ",                  pBBox + nChar);
    nChar += psp::getValueOf (mnWidthPt,            pBBox + nChar);
    nChar += psp::appendStr  (" ",                  pBBox + nChar);
    nChar += psp::getValueOf (mnHeightPt,           pBBox + nChar);
    nChar += psp::appendStr  ("\n",                 pBBox + nChar);

    WritePS (mpJobHeader, pBBox);

    // Creator (this application)
    WritePS (mpJobHeader, "%%Creator: ");
    WritePS (mpJobHeader, rAppName);
    WritePS (mpJobHeader, "\n");

    // For (user name)
    sal_Char pUserName[64];
    if (getUserName(pUserName, sizeof(pUserName)))
    {
        WritePS (mpJobHeader, "%%For: ");
        WritePS (mpJobHeader, pUserName);
        WritePS (mpJobHeader, "\n");
    }

    // Creation Date (locale independent local time)
    sal_Char pCreationDate [256];
    WritePS (mpJobHeader, "%%CreationDate: ");
    WritePS (mpJobHeader, getLocalTime(pCreationDate, sizeof(pCreationDate)));

    // Document Title
    WritePS (mpJobHeader, "%%Title: ");
    WritePS (mpJobHeader, rJobName);
    WritePS (mpJobHeader, "\n");

    // Language Level
    sal_Char pLevel[16];
    sal_Int32 nSz = getValueOf(GetPostscriptLevel(&rSetupData), pLevel);
    pLevel[nSz++] = '\n';
    pLevel[nSz  ] = '\0';
    WritePS (mpJobHeader, "%%LanguageLevel: ");
    WritePS (mpJobHeader, pLevel);

    // Other
    WritePS (mpJobHeader, "%%DocumentData: Clean7Bit\n");
    WritePS (mpJobHeader, "%%Pages: (atend)\n");
    WritePS (mpJobHeader, "%%PageOrder: Ascend\n");
    WritePS (mpJobHeader, "%%EndComments\n");

    writeProlog (mpJobHeader);

    // mark last job setup as not set
    m_aLastJobData.m_pParser = NULL;
    m_aLastJobData.m_aContext.setParser( NULL );

    writeSetup( mpJobHeader, rSetupData );

    return sal_True;
}

sal_Bool
PrinterJob::EndJob ()
{
    // write document trailer according to Document Structuring Conventions (DSC)
    sal_Char pPageNr [16];
    sal_Int32 nSz = getValueOf((sal_Int32)maPageList.size(), pPageNr);
    pPageNr [nSz] = '\0';

    WritePS (mpJobTrailer, "%%Trailer\n");
    WritePS (mpJobTrailer, "%%Pages: ");
    WritePS (mpJobTrailer, pPageNr);
    WritePS (mpJobTrailer, "\n");
    WritePS (mpJobTrailer, "%%EOF\n");

    /*
     * spool the set of files to their final destination, this is U**X dependent
     */

    FILE* pDestFILE = NULL;

    /* create a destination either as file or as a pipe */

    sal_Bool bSpoolToFile = maFileName.getLength() > 0 ? sal_True : sal_False;
    if (bSpoolToFile)
    {
        const rtl::OString aFileName = rtl::OUStringToOString (maFileName,
                                                               osl_getThreadTextEncoding());

        pDestFILE = fopen (aFileName.getStr(), "w");
        if (pDestFILE == NULL)
            return sal_False;
    }
    else
    {
        const PrinterInfoManager& rPrinterInfoManager = PrinterInfoManager::get ();
        const rtl::OUString& rPrinter     = m_aLastJobData.m_aPrinterName;
        const PrinterInfo&   rPrinterInfo = rPrinterInfoManager.getPrinterInfo (rPrinter);
        const rtl::OUString& rCommand     = rPrinterInfo.m_aCommand;

        const rtl::OString aShellCommand = rtl::OUStringToOString (rCommand,
                                                                   RTL_TEXTENCODING_ISO_8859_1);

        pDestFILE = popen (aShellCommand.getStr(), "w");
        if (pDestFILE == NULL)
            return sal_False;
    }

    /* spool the document parts to the destination */

    sal_uChar pBuffer[ nBLOCKSIZE ];

    AppendPS (pDestFILE, mpJobHeader, pBuffer);
    mpJobHeader->close();

    std::list< osl::File* >::iterator pPageBody;
    std::list< osl::File* >::iterator pPageHead;
    for (pPageBody  = maPageList.begin(), pPageHead  = maHeaderList.begin();
         pPageBody != maPageList.end() && pPageHead != maHeaderList.end();
         pPageBody++, pPageHead++)
    {
        (*pPageHead)->open(OpenFlag_Read);
        AppendPS (pDestFILE, *pPageHead, pBuffer);
        (*pPageHead)->close();

        (*pPageBody)->open(OpenFlag_Read);
        AppendPS (pDestFILE, *pPageBody, pBuffer);
        (*pPageBody)->close();
    }

    AppendPS (pDestFILE, mpJobTrailer, pBuffer);
    mpJobTrailer->close();

    /* well done */

    if (bSpoolToFile)
        fclose (pDestFILE);
    else
        pclose (pDestFILE);

    return sal_True;
}

sal_Bool
PrinterJob::AbortJob ()
{
    return sal_False;
}

void
PrinterJob::InitPaperSize (const JobData& rJobSetup)
{
    int nResX, nResY;
    rJobSetup.m_aContext.getResolution (nResX, nResY);

    String aPaper;
    int nWidth, nHeight;
    rJobSetup.m_aContext.getPageSize (aPaper, nWidth, nHeight);

    int nLeft = 0, nRight = 0, nUpper = 0, nLower = 0;
    const PPDParser* pParser = rJobSetup.m_aContext.getParser();
    if (pParser != NULL)
        pParser->getMargins (aPaper, nLeft, nRight, nUpper, nLower);

    mnResX          = nResX;
    mnResY          = nResY;

    mnWidthPt       = nWidth;
    mnHeightPt      = nHeight;

    mnLMarginPt     = nLeft;
    mnRMarginPt     = nRight;
    mnTMarginPt     = nUpper;
    mnBMarginPt     = nLower;

    mfXScale        = (double)72.0 / (double)mnResX;
    mfYScale        = -1.0 * (double)72.0 / (double)mnResY;
}


SalGraphics*
PrinterJob::StartPage (const JobData& rJobSetup, sal_Bool bNewJobData)
{
    InitPaperSize (rJobSetup);

    rtl::OUString aPageNo = rtl::OUString::valueOf ((sal_Int32)maPageList.size());
    rtl::OUString aExt    = aPageNo + rtl::OUString::createFromAscii (".ps");

    osl::File* pPageHeader = CreateSpoolFile (
                                              rtl::OUString::createFromAscii("psp_pghead"), aExt);
    osl::File* pPageBody   = CreateSpoolFile (
                                              rtl::OUString::createFromAscii("psp_pgbody"), aExt);

    maHeaderList.push_back (pPageHeader);
    maPageList.push_back (pPageBody);

    // write page header according to Document Structuring Conventions (DSC)

    WritePS (pPageHeader, "%%Page: ");
    WritePS (pPageHeader, aPageNo);
    WritePS (pPageHeader, " ");
    WritePS (pPageHeader, aPageNo);
    WritePS (pPageHeader, "\n");

    sal_Char  pBBox [256];
    sal_Int32 nChar = 0;

    nChar  = psp::appendStr  ("%%PageBoundingBox: ",    pBBox);
    nChar += psp::getValueOf (mnLMarginPt,              pBBox + nChar);
    nChar += psp::appendStr  (" ",                      pBBox + nChar);
    nChar += psp::getValueOf (mnBMarginPt,              pBBox + nChar);
    nChar += psp::appendStr  (" ",                      pBBox + nChar);
    nChar += psp::getValueOf (mnWidthPt  - mnRMarginPt, pBBox + nChar);
    nChar += psp::appendStr  (" ",                      pBBox + nChar);
    nChar += psp::getValueOf (mnHeightPt - mnTMarginPt, pBBox + nChar);
    nChar += psp::appendStr  ("\n",                     pBBox + nChar);

    WritePS (pPageHeader, pBBox);

    writeSetup     ( pPageHeader, rJobSetup );
    writePageSetup ( pPageHeader, rJobSetup );

    return NULL;
}

sal_Bool
PrinterJob::EndPage ()
{
    osl::File* pPageHeader = maHeaderList.back();
    osl::File* pPageBody   = maPageList.back();

    // copy page to paper and write page trailer according to DSC

    sal_Char pTrailer[256];
    sal_Int32 nChar = 0;
    nChar  = psp::appendStr ("showpage\n",          pTrailer);
    nChar += psp::appendStr ("grestore grestore\n", pTrailer + nChar);
    nChar += psp::appendStr ("%%PageTrailer\n\n",   pTrailer + nChar);
    WritePS (pPageBody, pTrailer);

    // this page is done for now, close it to avoid having too many open fd's

    pPageHeader->close();
    pPageBody->close();

    return sal_True;
}

sal_uInt32
PrinterJob::GetErrorCode ()
{
    /* TODO */
    return 0;
}

struct less_ppd_key : public ::std::binary_function<double, double, bool>
{
    bool operator()(const PPDKey* left, const PPDKey* right)
    { return left->getOrderDependency() < right->getOrderDependency(); }
};

static bool writeFeature( osl::File* pFile, const PPDKey* pKey, const PPDValue* pValue )
{
    if( ! pKey || ! pValue )
        return true;

    String aFeature( RTL_CONSTASCII_USTRINGPARAM( "%%BeginFeature: *" ) );
    aFeature += pKey->getKey();
    aFeature += ' ';
    aFeature += pValue->m_aOption;
    aFeature += '\n';
    aFeature += pValue->m_aValue;
    aFeature.AppendAscii( "\n%%EndFeature\n" );
    ByteString aPSFeature( aFeature, RTL_TEXTENCODING_ASCII_US );
    sal_uInt64 nWritten = 0;
    return pFile->write( aPSFeature.GetBuffer(), aPSFeature.Len(), nWritten )
        || nWritten != aPSFeature.Len() ? false : true;
}

bool PrinterJob::writePageSetup( osl::File* pFile, const JobData& rJob )
{
    bool bSuccess = true;

    WritePS (pFile, "%%BeginPageSetup\n%\n");
    ByteString aLine( "/#copies " );
    aLine += ByteString::CreateFromInt32( rJob.m_nCopies );
    aLine +=  " def\n";
    sal_uInt64 nWritten = 0;
    bSuccess = pFile->write( aLine.GetBuffer(), aLine.Len(), nWritten )
        || nWritten != aLine.Len() ? false : true;

    if( bSuccess && GetPostscriptLevel( &rJob ) >= 2 )
        WritePS (pFile, "<< /NumCopies null /Policies << /NumCopies 1 >> >> setpagedevice\n" );

    sal_Char  pTranslate [128];
    sal_Int32 nChar = 0;

    if( rJob.m_eOrientation == orientation::Portrait )
    {
        nChar  = psp::appendStr  ("gsave\n[",   pTranslate);
        nChar += psp::getValueOfDouble (        pTranslate + nChar, mfXScale, 5);
        nChar += psp::appendStr  (" 0 0 ",      pTranslate + nChar);
        nChar += psp::getValueOfDouble (        pTranslate + nChar, mfYScale, 5);
        nChar += psp::appendStr  (" ",          pTranslate + nChar);
        nChar += psp::getValueOf (mnRMarginPt,  pTranslate + nChar);
        nChar += psp::appendStr  (" ",          pTranslate + nChar);
        nChar += psp::getValueOf (mnHeightPt-mnTMarginPt,
                                  pTranslate + nChar);
        nChar += psp::appendStr  ("] concat\ngsave\n",
                                  pTranslate + nChar);
    }
    else
    {
        nChar  = psp::appendStr  ("gsave\n",    pTranslate);
        nChar += psp::appendStr  ("[ 0 ",       pTranslate + nChar);
        nChar += psp::getValueOfDouble (        pTranslate + nChar, -mfYScale, 5);
        nChar += psp::appendStr  (" ",          pTranslate + nChar);
        nChar += psp::getValueOfDouble (        pTranslate + nChar, mfXScale, 5);
        nChar += psp::appendStr  (" 0 ",        pTranslate + nChar );
        nChar += psp::getValueOfDouble (        pTranslate + nChar, mnLMarginPt, 5 );
        nChar += psp::appendStr  (" ",          pTranslate + nChar);
        nChar += psp::getValueOf (mnBMarginPt,  pTranslate + nChar );
        nChar += psp::appendStr ("] concat\ngsave\n",
                                 pTranslate + nChar);
    }

    WritePS (pFile, pTranslate);

    WritePS (pFile, "%%EndPageSetup\n");

    return bSuccess;
}

bool PrinterJob::writeProlog (osl::File* pFile)
{
    const sal_Char pProlog[] = {
        "%%BeginProlog\n"
        "/ISO1252Encoding [\n"
        "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n"
        "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n"
        "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n"
        "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n"
        "/space /exclam /quotedbl /numbersign /dollar /percent /ampersand /quotesingle\n"
        "/parenleft /parenright /asterisk /plus /comma /hyphen /period /slash\n"
        "/zero /one /two /three /four /five /six /seven\n"
        "/eight /nine /colon /semicolon /less /equal /greater /question\n"
        "/at /A /B /C /D /E /F /G\n"
        "/H /I /J /K /L /M /N /O\n"
        "/P /Q /R /S /T /U /V /W\n"
        "/X /Y /Z /bracketleft /backslash /bracketright /asciicircum /underscore\n"
        "/grave /a /b /c /d /e /f /g\n"
        "/h /i /j /k /l /m /n /o\n"
        "/p /q /r /s /t /u /v /w\n"
        "/x /y /z /braceleft /bar /braceright /asciitilde /unused\n"
        "/euro /unused /quotesinglbase /florin /quotedblbase /ellipsis /dagger /daggerdbl\n"
        "/circumflex /perthousand /Scaron /guilsinglleft /OE /unused /zcaron /unused\n"
        "/unused /quoteleft /quoteright /quotedblleft /quotedblright /bullet /endash /emdash\n"
        "/tilde /trademark /scaron /guilsinglright /oe /unused /zcaron /Ydieresis\n"
        "/space /exclamdown /cent /sterling /currency /yen /brokenbar /section\n"
        "/dieresis /copyright /ordfeminine /guillemotleft /logicalnot /hyphen /registered /macron\n"
        "/degree /plusminus /twosuperior /threesuperior /acute /mu /paragraph /periodcentered\n"
        "/cedilla /onesuperior /ordmasculine /guillemotright /onequarter /onehalf /threequarters /questiondown\n"
        "/Agrave /Aacute /Acircumflex /Atilde /Adieresis /Aring /AE /Ccedilla\n"
        "/Egrave /Eacute /Ecircumflex /Edieresis /Igrave /Iacute /Icircumflex /Idieresis\n"
        "/Eth /Ntilde /Ograve /Oacute /Ocircumflex /Otilde /Odieresis /multiply\n"
        "/Oslash /Ugrave /Uacute /Ucircumflex /Udieresis /Yacute /Thorn /germandbls\n"
        "/agrave /aacute /acircumflex /atilde /adieresis /aring /ae /ccedilla\n"
        "/egrave /eacute /ecircumflex /edieresis /igrave /iacute /icircumflex /idieresis\n"
        "/eth /ntilde /ograve /oacute /ocircumflex /otilde /odieresis /divide\n"
        "/oslash /ugrave /uacute /ucircumflex /udieresis /yacute /thorn /ydieresis] def\n"
        "\n"
        "/psp_findfont { exch dup findfont dup length dict begin\n"
        "{ 1 index /FID ne { def } { pop pop } ifelse } forall\n"
        "/Encoding 3 -1 roll def currentdict end /psp_reencodedfont\n"
        "exch definefont } def\n"
        "\n"
        "/pathdict dup 8 dict def load begin\n"
        "/rcmd { { currentfile 1 string readstring pop 0 get dup 32 gt { exit }\n"
        "{ pop } ifelse } loop dup 126 eq { pop exit } if 65 sub dup 16#3 and 1\n"
        "add exch dup 16#C and -2 bitshift 16#3 and 1 add exch 16#10 and 16#10\n"
        "eq 3 1 roll exch } def\n"
        "/rhex { dup 1 sub exch currentfile exch string readhexstring pop dup 0\n"
        "get dup 16#80 and 16#80 eq dup 3 1 roll { 16#7f and } if 2 index 0 3\n"
        "-1 roll put 3 1 roll 0 0 1 5 -1 roll { 2 index exch get add 256 mul }\n"
        "for 256 div exch pop exch { neg } if } def\n"
        "/xcmd { rcmd exch rhex exch rhex exch 5 -1 roll add exch 4 -1 roll add\n"
        "1 index 1 index 5 -1 roll { moveto } { lineto } ifelse } def end\n"
        "/readpath { 0 0 pathdict begin { xcmd } loop end pop pop } def\n"
        "\n"
        "systemdict /languagelevel known not {\n"
        "/xshow { exch dup length 0 1 3 -1 roll 1 sub { dup 3 index exch get\n"
        "exch 2 index exch get 1 string dup 0 4 -1 roll put currentpoint 3 -1\n"
        "roll show moveto 0 rmoveto } for pop pop } def\n"
        "/rectangle { 4 -2 roll moveto 1 index 0 rlineto 0 exch rlineto neg 0\n"
        "rlineto closepath } def\n"
        "/rectfill { rectangle fill } def\n"
        "/rectstroke { rectangle stroke } def } if\n"
        "\n"
        "/psp_lzwfilter { currentfile /ASCII85Decode filter /LZWDecode filter } def\n"
        "/psp_ascii85filter { currentfile /ASCII85Decode filter } def\n"
        "/psp_lzwstring { psp_lzwfilter 1024 string readstring } def\n"
        "/psp_ascii85string { psp_ascii85filter 1024 string readstring } def\n"
        "/psp_imagedict {\n"
        "/psp_bitspercomponent { 3 eq { 1 }{ 8 } ifelse } def\n"
        "/psp_decodearray { [ [0 1 0 1 0 1] [0 255] [0 1] [0 255] ] exch get }\n"
        "def 7 dict dup\n"
        "/ImageType 1 put dup\n"
        "/Width 7 -1 roll put dup\n"
        "/Height 5 index put dup\n"
        "/BitsPerComponent 4 index psp_bitspercomponent put dup\n"
        "/Decode 5 -1 roll psp_decodearray put dup\n"
        "/ImageMatrix [1 0 0 1 0 0] dup 5 8 -1 roll put put dup\n"
        "/DataSource 4 -1 roll 1 eq { psp_lzwfilter } { psp_ascii85filter } ifelse put\n"
        "} def\n"
        "%%EndProlog\n"
    };
    WritePS (pFile, pProlog);

    return true;
}

bool PrinterJob::writeSetup( osl::File* pFile, const JobData& rJob )
{
    bool bSuccess = true;
    int i;

    WritePS (pFile, "%%BeginSetup\n%\n");

    // emit features ordered to OrderDependency
    // ignore features that are set to default
    const PPDContext& rContext = rJob.m_aContext;
    // sanity check
    if( rJob.m_pParser == rJob.m_aContext.getParser() &&
        rJob.m_pParser &&
        ( m_aLastJobData.m_pParser == rJob.m_pParser || m_aLastJobData.m_pParser == NULL )
        )
    {
        int nKeys = rJob.m_aContext.countValuesModified();
        ::std::vector< const PPDKey* > aKeys( nKeys );
        for(  i = 0; i < nKeys; i++ )
            aKeys[i] = rJob.m_aContext.getModifiedKey( i );
        ::std::sort( aKeys.begin(), aKeys.end(), less_ppd_key() );

        for( i = 0; i < nKeys && bSuccess; i++ )
        {
            const PPDKey* pKey = aKeys[i];
            if( pKey->getSetupType()    == PPDKey::DocumentSetup    ||
                pKey->getSetupType()    == PPDKey::PageSetup        ||
                pKey->getSetupType()    == PPDKey::AnySetup )
            {
                const PPDValue* pValue = rJob.m_aContext.getValue( pKey );
                if(pValue
                   && pValue->m_eType == eInvocation
                   && pValue->m_aValue.Len()
                   && ( m_aLastJobData.m_pParser == NULL
                        || m_aLastJobData.m_aContext.getValue( pKey ) != pValue )
                   )
                {
                    if( GetPostscriptLevel( &rJob ) == 1 )
                    {
                        bool bHavePS2 =
                            ( pValue->m_aValue.SearchAscii( "<<" ) != STRING_NOTFOUND )
                            ||
                            ( pValue->m_aValue.SearchAscii( ">>" ) != STRING_NOTFOUND );
                        if( bHavePS2 )
                            continue;
                    }
                    bSuccess = writeFeature( pFile, pKey, pValue );
                }
            }
        }
    }
    else
        bSuccess = false;

    WritePS (pFile, "%%EndSetup\n");

    if( bSuccess )
        m_aLastJobData = rJob;

    return bSuccess;
}
