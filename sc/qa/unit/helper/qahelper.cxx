/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "qahelper.hxx"

#if defined WNT
#define __ORCUS_STATIC_LIB
#endif
#include <orcus/csv_parser.hpp>

#include <fstream>

FileFormat aFileFormats[] = {
    { "ods" , "calc8", "", ODS_FORMAT_TYPE },
    { "xls" , "MS Excel 97", "calc_MS_EXCEL_97", XLS_FORMAT_TYPE },
    { "xlsx", "Calc MS Excel 2007 XML" , "MS Excel 2007 XML", XLSX_FORMAT_TYPE },
    { "csv" , "Text - txt - csv (StarCalc)", "generic_Text", CSV_FORMAT_TYPE },
    { "html" , "calc_HTML_WebQuery", "generic_HTML", HTML_FORMAT_TYPE },
    { "123" , "Lotus", "calc_Lotus", LOTUS123_FORMAT_TYPE },
    { "dif", "DIF", "calc_DIF", DIF_FORMAT_TYPE },
};

bool testEqualsWithTolerance( long nVal1, long nVal2, long nTol )
{
    return ( labs( nVal1 - nVal2 ) <= nTol );
}

void loadFile(const OUString& aFileName, std::string& aContent)
{
    OString aOFileName = OUStringToOString(aFileName, RTL_TEXTENCODING_UTF8);

#ifdef ANDROID
    size_t size;
    if (strncmp(aOFileName.getStr(), "/assets/", sizeof("/assets/")-1) == 0) {
        const char *contents = (const char *) lo_apkentry(aOFileName.getStr(), &size);
        if (contents != 0) {
            aContent = std::string(contents, size);
            return;
        }
    }
#endif

    std::ifstream aFile(aOFileName.getStr());

    OStringBuffer aErrorMsg("Could not open csv file: ");
    aErrorMsg.append(aOFileName);
    CPPUNIT_ASSERT_MESSAGE(aErrorMsg.getStr(), aFile);
    std::ostringstream aOStream;
    aOStream << aFile.rdbuf();
    aFile.close();
    aContent = aOStream.str();
}

void testFile(OUString& aFileName, ScDocument* pDoc, SCTAB nTab, StringType aStringFormat)
{
    csv_handler aHandler(pDoc, nTab, aStringFormat);
    orcus::csv_parser_config aConfig;
    aConfig.delimiters.push_back(',');
    aConfig.delimiters.push_back(';');
    aConfig.text_qualifier = '"';
    aConfig.trim_cell_value = false;


    std::string aContent;
    loadFile(aFileName, aContent);
    orcus::csv_parser<csv_handler> parser ( &aContent[0], aContent.size() , aHandler, aConfig);
    try
    {
        parser.parse();
    }
    catch (const orcus::csv_parse_error& e)
    {
        std::cout << "reading csv content file failed: " << e.what() << std::endl;
        OStringBuffer aErrorMsg("csv parser error: ");
        aErrorMsg.append(e.what());
        CPPUNIT_ASSERT_MESSAGE(aErrorMsg.getStr(), false);
    }
}

void testCondFile(OUString& aFileName, ScDocument* pDoc, SCTAB nTab)
{
    conditional_format_handler aHandler(pDoc, nTab);
    orcus::csv_parser_config aConfig;
    aConfig.delimiters.push_back(',');
    aConfig.delimiters.push_back(';');
    aConfig.text_qualifier = '"';
    std::string aContent;
    loadFile(aFileName, aContent);
    orcus::csv_parser<conditional_format_handler> parser ( &aContent[0], aContent.size() , aHandler, aConfig);
    try
    {
        parser.parse();
    }
    catch (const orcus::csv_parse_error& e)
    {
        std::cout << "reading csv content file failed: " << e.what() << std::endl;
        OStringBuffer aErrorMsg("csv parser error: ");
        aErrorMsg.append(e.what());
        CPPUNIT_ASSERT_MESSAGE(aErrorMsg.getStr(), false);
    }
}

ScDocShellRef ScBootstrapFixture::load( bool bReadWrite,
    const OUString& rURL, const OUString& rFilter, const OUString &rUserData,
    const OUString& rTypeName, unsigned int nFilterFlags, unsigned int nClipboardID,
    sal_uIntPtr nFilterVersion, const OUString* pPassword )
{
    SfxFilter* pFilter = new SfxFilter(
        rFilter,
        OUString(), nFilterFlags, nClipboardID, rTypeName, 0, OUString(),
        rUserData, OUString("private:factory/scalc*"));
    pFilter->SetVersion(nFilterVersion);

    ScDocShellRef xDocShRef = new ScDocShell;
    xDocShRef->GetDocument()->EnableUserInteraction(false);
    SfxMedium* pSrcMed = new SfxMedium(rURL, bReadWrite ? STREAM_STD_READWRITE : STREAM_STD_READ );
    pSrcMed->SetFilter(pFilter);
    pSrcMed->UseInteractionHandler(false);
    if (pPassword)
    {
        SfxItemSet* pSet = pSrcMed->GetItemSet();
        pSet->Put(SfxStringItem(SID_PASSWORD, *pPassword));
    }
    printf("about to load %s\n", OUStringToOString( rURL, RTL_TEXTENCODING_UTF8 ).getStr() );
    if (!xDocShRef->DoLoad(pSrcMed))
    {
        xDocShRef->DoClose();
        // load failed.
        xDocShRef.Clear();
    }

    return xDocShRef;
}

ScDocShellRef ScBootstrapFixture::load(
    const OUString& rURL, const OUString& rFilter, const OUString &rUserData,
    const OUString& rTypeName, unsigned int nFilterFlags, unsigned int nClipboardID,
    sal_uIntPtr nFilterVersion, const OUString* pPassword )
{
    return load( false, rURL, rFilter, rUserData, rTypeName, nFilterFlags, nClipboardID,  nFilterVersion, pPassword );
}

ScDocShellRef ScBootstrapFixture::loadDoc(
    const OUString& rFileName, sal_Int32 nFormat, bool bReadWrite )
{
    OUString aFileExtension(aFileFormats[nFormat].pName, strlen(aFileFormats[nFormat].pName), RTL_TEXTENCODING_UTF8 );
    OUString aFilterName(aFileFormats[nFormat].pFilterName, strlen(aFileFormats[nFormat].pFilterName), RTL_TEXTENCODING_UTF8) ;
    OUString aFileName;
    createFileURL( rFileName, aFileExtension, aFileName );
    OUString aFilterType(aFileFormats[nFormat].pTypeName, strlen(aFileFormats[nFormat].pTypeName), RTL_TEXTENCODING_UTF8);
    unsigned int nFormatType = aFileFormats[nFormat].nFormatType;
    unsigned int nClipboardId = nFormatType ? SFX_FILTER_IMPORT | SFX_FILTER_USESOPTIONS : 0;

    return load(bReadWrite, aFileName, aFilterName, OUString(), aFilterType, nFormatType, nClipboardId, nFormatType);
}

void ScBootstrapFixture::createFileURL(
    const OUString& aFileBase, const OUString& aFileExtension, OUString& rFilePath)
{
    OUString aSep("/");
    OUStringBuffer aBuffer( getSrcRootURL() );
    aBuffer.append(m_aBaseString).append(aSep).append(aFileExtension);
    aBuffer.append(aSep).append(aFileBase).append(aFileExtension);
    rFilePath = aBuffer.makeStringAndClear();
}

void ScBootstrapFixture::createCSVPath(const OUString& aFileBase, OUString& rCSVPath)
{
    OUStringBuffer aBuffer( getSrcRootPath());
    aBuffer.append(m_aBaseString).append(OUString("/contentCSV/"));
    aBuffer.append(aFileBase).append(OUString("csv"));
    rCSVPath = aBuffer.makeStringAndClear();
}

ScDocShellRef ScBootstrapFixture::saveAndReload(
    ScDocShell* pShell, const OUString &rFilter,
    const OUString &rUserData, const OUString& rTypeName, sal_uLong nFormatType)
{

    utl::TempFile aTempFile;
    aTempFile.EnableKillingFile();
    SfxMedium aStoreMedium( aTempFile.GetURL(), STREAM_STD_WRITE );
    sal_uInt32 nExportFormat = 0;
    if (nFormatType == ODS_FORMAT_TYPE)
        nExportFormat = SFX_FILTER_EXPORT | SFX_FILTER_USESOPTIONS;
    SfxFilter* pExportFilter = new SfxFilter(
        rFilter,
        OUString(), nFormatType, nExportFormat, rTypeName, 0, OUString(),
        rUserData, OUString("private:factory/scalc*") );
    pExportFilter->SetVersion(SOFFICE_FILEFORMAT_CURRENT);
    aStoreMedium.SetFilter(pExportFilter);
    pShell->DoSaveAs( aStoreMedium );
    pShell->DoClose();

    //std::cout << "File: " << aTempFile.GetURL() << std::endl;

    sal_uInt32 nFormat = 0;
    if (nFormatType == ODS_FORMAT_TYPE)
        nFormat = SFX_FILTER_IMPORT | SFX_FILTER_USESOPTIONS;

    return load(aTempFile.GetURL(), rFilter, rUserData, rTypeName, nFormatType, nFormat );
}

ScDocShellRef ScBootstrapFixture::saveAndReload( ScDocShell* pShell, sal_Int32 nFormat )
{
    OUString aFilterName(aFileFormats[nFormat].pFilterName, strlen(aFileFormats[nFormat].pFilterName), RTL_TEXTENCODING_UTF8) ;
    OUString aFilterType(aFileFormats[nFormat].pTypeName, strlen(aFileFormats[nFormat].pTypeName), RTL_TEXTENCODING_UTF8);
    ScDocShellRef xDocSh = saveAndReload(pShell, aFilterName, OUString(), aFilterType, aFileFormats[nFormat].nFormatType);

    CPPUNIT_ASSERT(xDocSh.Is());
    return xDocSh;
}

void ScBootstrapFixture::miscRowHeightsTest( TestParam* aTestValues, unsigned int numElems )
{
    for ( unsigned int index=0; index<numElems; ++index )
    {
        OUString sFileName = OUString::createFromAscii( aTestValues[ index ].sTestDoc );
        printf("aTestValues[%u] %s\n", index, OUStringToOString( sFileName, RTL_TEXTENCODING_UTF8 ).getStr() );
        int nImportType =  aTestValues[ index ].nImportType;
        int nExportType =  aTestValues[ index ].nExportType;
        ScDocShellRef xShell = loadDoc( sFileName, nImportType );
        CPPUNIT_ASSERT(xShell.Is());

        if ( nExportType != -1 )
            xShell = saveAndReload(&(*xShell), nExportType );

        CPPUNIT_ASSERT(xShell.Is());

        ScDocument* pDoc = xShell->GetDocument();

        for (int i=0; i<aTestValues[ index ].nRowData; ++i)
        {
            SCROW nRow = aTestValues[ index ].pData[ i].nStartRow;
            SCROW nEndRow = aTestValues[ index ].pData[ i ].nEndRow;
            SCTAB nTab = aTestValues[ index ].pData[ i ].nTab;
            int nExpectedHeight = aTestValues[ index ].pData[ i ].nExpectedHeight;
            if ( nExpectedHeight == -1 )
                nExpectedHeight =  sc::TwipsToHMM( ScGlobal::nStdRowHeight );
            bool bCheckOpt = ( ( aTestValues[ index ].pData[ i ].nCheck & CHECK_OPTIMAL ) == CHECK_OPTIMAL );
            for ( ; nRow <= nEndRow; ++nRow )
            {
                printf("\t checking row %" SAL_PRIdINT32 " for height %d\n", nRow, nExpectedHeight );
                int nHeight = sc::TwipsToHMM( pDoc->GetRowHeight(nRow, nTab, false) );
                if ( bCheckOpt )
                {
                    bool bOpt = !(pDoc->GetRowFlags( nRow, nTab ) & CR_MANUALSIZE);
                    CPPUNIT_ASSERT_EQUAL(aTestValues[ index ].pData[ i ].bOptimal, bOpt);
                }
                CPPUNIT_ASSERT_EQUAL(nExpectedHeight, nHeight);
            }
        }
        xShell->DoClose();
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
