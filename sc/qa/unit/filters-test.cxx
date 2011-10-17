/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the Original Code is
 *       Caolán McNamara <caolanm@redhat.com>
 * Portions created by the Initial Developer are Copyright (C) 2011 the
 * Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Caolán McNamara <caolanm@redhat.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */

#include <sal/config.h>
#include <unotest/filters-test.hxx>
#include <test/bootstrapfixture.hxx>
#include <rtl/strbuf.hxx>
#include <osl/file.hxx>

#include <sfx2/app.hxx>
#include <sfx2/docfilt.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/sfxmodelfactory.hxx>
#include <svl/intitem.hxx>

#include <editeng/brshitem.hxx>
#include <editeng/justifyitem.hxx>
#include <basic/sbxdef.hxx>

#include "helper/csv_handler.hxx"
#include "orcus/csv_parser.hpp"
#include <fstream>
#include <string>
#include <sstream>

#define ODS_FORMAT_TYPE 50331943
#define XLS_FORMAT_TYPE 318767171
#define XLSX_FORMAT_TYPE 268959811

#define ODS     0
#define XLS     1
#define XLSX    2

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace {

struct FileFormat {
    const char* pName; const char* pFilterName; const char* pTypeName; sal_uLong nFormatType;
};

FileFormat aFileFormats[] = {
    { "ods" , "calc8", "", ODS_FORMAT_TYPE },
    { "xls" , "MS Excel 97", "calc_MS_EXCEL_97", XLS_FORMAT_TYPE },
    { "xlsx", "Calc MS Excel 2007 XML" , "MS Excel 2007 XML", XLSX_FORMAT_TYPE }
};

void loadFile(const rtl::OUString& aFileName, std::string& aContent)
{
    rtl::OString aOFileName = rtl::OUStringToOString(aFileName, RTL_TEXTENCODING_UTF8);
    std::ifstream aFile(aOFileName.getStr());

    rtl::OStringBuffer aErrorMsg("Could not open csv file: ");
    aErrorMsg.append(aOFileName);
    CPPUNIT_ASSERT_MESSAGE(aErrorMsg.getStr(), aFile);
    std::ostringstream aOStream;
    aOStream << aFile.rdbuf();
    aFile.close();
    aContent = aOStream.str();
}

void testFile(rtl::OUString& aFileName, ScDocument* pDoc, SCTAB nTab, StringType aStringFormat = StringValue)
{
    csv_handler aHandler(pDoc, nTab, aStringFormat);
    orcus::csv_parser_config aConfig;
    aConfig.delimiters.push_back(',');
    aConfig.delimiters.push_back(';');
    aConfig.text_qualifier = '"';
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
        rtl::OStringBuffer aErrorMsg("csv parser error: ");
        aErrorMsg.append(e.what());
        CPPUNIT_ASSERT_MESSAGE(aErrorMsg.getStr(), false);
    }
}

//need own handler because conditional formatting strings must be generated
void testCondFile(rtl::OUString& aFileName, ScDocument* pDoc, SCTAB nTab)
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
        rtl::OStringBuffer aErrorMsg("csv parser error: ");
        aErrorMsg.append(e.what());
        CPPUNIT_ASSERT_MESSAGE(aErrorMsg.getStr(), false);
    }

}

}

/* Implementation of Filters test */

class ScFiltersTest
    : public test::FiltersTest
    , public test::BootstrapFixture
{
public:
    ScFiltersTest();

    virtual bool load(const rtl::OUString &rFilter, const rtl::OUString &rURL, const rtl::OUString &rUserData);
    ScDocShellRef load(const rtl::OUString &rFilter, const rtl::OUString &rURL,
        const rtl::OUString &rUserData, const rtl::OUString& rTypeName, sal_uLong nFormatType=0);

    void createFileURL(const rtl::OUString& aFileBase, const rtl::OUString& aFileExtension, rtl::OUString& rFilePath);
    void createCSVPath(const rtl::OUString& aFileBase, rtl::OUString& rFilePath);

    virtual void setUp();
    virtual void tearDown();

    /**
     * Ensure CVEs remain unbroken
     */
    void testCVEs();

    //ods, xls, xlsx filter tests
    void testRangeName();
    void testContent();
    void testFunctions();
    void testDatabaseRanges();
    void testFormats();
    void testMatrix();
    void testBugFixesODS();
    void testBugFixesXLS();
    void testBugFixesXLSX();

    void testStarBasic();
    void testVba();

    CPPUNIT_TEST_SUITE(ScFiltersTest);
    CPPUNIT_TEST(testCVEs);
    CPPUNIT_TEST(testRangeName);
    CPPUNIT_TEST(testContent);
    CPPUNIT_TEST(testFunctions);
    CPPUNIT_TEST(testDatabaseRanges);
    CPPUNIT_TEST(testFormats);
    CPPUNIT_TEST(testMatrix);
    CPPUNIT_TEST(testBugFixesODS);
    CPPUNIT_TEST(testBugFixesXLS);
    CPPUNIT_TEST(testBugFixesXLSX);
    //enable this test if you want to play with star basic macros in unit tests
    //works but does nothing useful yet
    CPPUNIT_TEST(testStarBasic);
    //enable if you want to hack vba support for unit tests
    //does not work, still problems during loading
    //CPPUNIT_TEST(testVba);


    CPPUNIT_TEST_SUITE_END();

private:
    uno::Reference<uno::XInterface> m_xCalcComponent;
    ::rtl::OUString m_aBaseString;
};

ScDocShellRef ScFiltersTest::load(const rtl::OUString &rFilter, const rtl::OUString &rURL,
    const rtl::OUString &rUserData, const rtl::OUString& rTypeName, sal_uLong nFormatType)
{
    sal_uInt32 nFormat = 0;
    if (nFormatType)
        nFormat = SFX_FILTER_IMPORT | SFX_FILTER_USESOPTIONS;
    SfxFilter* aFilter = new SfxFilter(
        rFilter,
        rtl::OUString(), nFormatType, nFormat, rTypeName, 0, rtl::OUString(),
        rUserData, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("private:factory/scalc*")) );
    aFilter->SetVersion(SOFFICE_FILEFORMAT_CURRENT);

    ScDocShellRef xDocShRef = new ScDocShell;
    SfxMedium* pSrcMed = new SfxMedium(rURL, STREAM_STD_READWRITE, true);
    pSrcMed->SetFilter(aFilter);
    if (!xDocShRef->DoLoad(pSrcMed))
        // load failed.
        xDocShRef.Clear();
    else if (nFormatType)
    {
        pSrcMed->GetItemSet()->Put( SfxUInt16Item( SID_MACROEXECMODE, 4));
        SfxObjectShell::SetCurrentComponent( xDocShRef->GetModel() );
    }

    return xDocShRef;
}

bool ScFiltersTest::load(const rtl::OUString &rFilter, const rtl::OUString &rURL,
    const rtl::OUString &rUserData)
{
    return load(rFilter, rURL, rUserData, rtl::OUString()).Is();
}

void ScFiltersTest::createFileURL(const rtl::OUString& aFileBase, const rtl::OUString& aFileExtension, rtl::OUString& rFilePath)
{
    rtl::OUString aSep(RTL_CONSTASCII_USTRINGPARAM("/"));
    rtl::OUStringBuffer aBuffer( getSrcRootURL() );
    aBuffer.append(m_aBaseString).append(aSep).append(aFileExtension);
    aBuffer.append(aSep).append(aFileBase).append(aFileExtension);
    rFilePath = aBuffer.makeStringAndClear();
}

void ScFiltersTest::createCSVPath(const rtl::OUString& aFileBase, rtl::OUString& rCSVPath)
{
    rtl::OUStringBuffer aBuffer(getSrcRootPath());
    aBuffer.append(m_aBaseString).append(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/contentCSV/")));
    aBuffer.append(aFileBase).append(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("csv")));
    rCSVPath = aBuffer.makeStringAndClear();
}

void ScFiltersTest::testCVEs()
{
    testDir(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Quattro Pro 6.0")),
        getURLFromSrc("/sc/qa/unit/data/qpro/"), rtl::OUString());

    //warning, the current "sylk filter" in sc (docsh.cxx) automatically
    //chains on failure on trying as csv, rtf, etc. so "success" may
    //not indicate that it imported as .slk.
    testDir(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SYLK")),
        getURLFromSrc("/sc/qa/unit/data/slk/"), rtl::OUString());
}

namespace {

void testRangeNameImpl(ScDocument* pDoc)
{
    //check one range data per sheet and one global more detailed
    //add some more checks here
    ScRangeData* pRangeData = pDoc->GetRangeName()->findByName(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Global1")));
    CPPUNIT_ASSERT_MESSAGE("range name Global1 not found", pRangeData);
    double aValue;
    pDoc->GetValue(1,0,0,aValue);
    CPPUNIT_ASSERT_MESSAGE("range name Global1 should reference Sheet1.A1", aValue == 1);
    pRangeData = pDoc->GetRangeName(0)->findByName(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Local1")));
    CPPUNIT_ASSERT_MESSAGE("range name Sheet1.Local1 not found", pRangeData);
    pDoc->GetValue(1,2,0,aValue);
    CPPUNIT_ASSERT_MESSAGE("range name Sheet1.Local1 should reference Sheet1.A3", aValue == 3);
    pRangeData = pDoc->GetRangeName(1)->findByName(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Local2")));
    CPPUNIT_ASSERT_MESSAGE("range name Sheet2.Local2 not found", pRangeData);
    pDoc->GetValue(1,1,1,aValue);
    CPPUNIT_ASSERT_MESSAGE("range name Sheet2.Local2 should reference Sheet2.A2", aValue == 7);
    //check for correct results for the remaining formulas
    pDoc->GetValue(1,1,0, aValue);
    CPPUNIT_ASSERT_MESSAGE("=global2 should be 2", aValue == 2);
    pDoc->GetValue(1,3,0, aValue);
    CPPUNIT_ASSERT_MESSAGE("=local2 should be 4", aValue == 4);
    pDoc->GetValue(2,0,0, aValue);
    CPPUNIT_ASSERT_MESSAGE("=SUM(global3) should be 10", aValue == 10);
    pDoc->GetValue(1,0,1,aValue);
    CPPUNIT_ASSERT_MESSAGE("range name Sheet2.local1 should reference Sheet1.A5", aValue == 5);
}

}

void ScFiltersTest::testRangeName()
{
    const rtl::OUString aFileNameBase(RTL_CONSTASCII_USTRINGPARAM("named-ranges-global."));
    //XLSX does not work yet
    for (sal_uInt32 i = 0; i < 2; ++i)
    {
        rtl::OUString aFileExtension(aFileFormats[i].pName, strlen(aFileFormats[i].pName), RTL_TEXTENCODING_UTF8 );
        rtl::OUString aFilterName(aFileFormats[i].pFilterName, strlen(aFileFormats[i].pFilterName), RTL_TEXTENCODING_UTF8) ;
        rtl::OUString aFileName;
        createFileURL( aFileNameBase, aFileExtension, aFileName );
        rtl::OUString aFilterType(aFileFormats[i].pTypeName, strlen(aFileFormats[i].pTypeName), RTL_TEXTENCODING_UTF8);
        std::cout << aFileFormats[i].pName << " Test" << std::endl;
        ScDocShellRef xDocSh = load (aFilterName, aFileName, rtl::OUString(), aFilterType, aFileFormats[i].nFormatType);
        xDocSh->DoHardRecalc(true);

        CPPUNIT_ASSERT_MESSAGE("Failed to load named-ranges-globals.*", xDocSh.Is());
        ScDocument* pDoc = xDocSh->GetDocument();
        testRangeNameImpl(pDoc);
        xDocSh->DoClose();
    }
}

namespace {

void testContentImpl(ScDocument* pDoc) //same code for ods, xls, xlsx
{
    double fValue;
    //check value import
    pDoc->GetValue(0,0,0,fValue);
    CPPUNIT_ASSERT_MESSAGE("value not imported correctly", fValue == 1);
    pDoc->GetValue(0,1,0,fValue);
    CPPUNIT_ASSERT_MESSAGE("value not imported correctly", fValue == 2);
    rtl::OUString aString;
    pDoc->GetString(1,0,0,aString);
    //check string import
    CPPUNIT_ASSERT_MESSAGE("string imported not correctly", aString == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("String1")));
    pDoc->GetString(1,1,0,aString);
    CPPUNIT_ASSERT_MESSAGE("string not imported correctly", aString == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("String2")));
    //check basic formula import
    pDoc->GetValue(2,0,0,fValue);
    CPPUNIT_ASSERT_MESSAGE("=2*3", fValue == 6);
    pDoc->GetValue(2,1,0,fValue);
    CPPUNIT_ASSERT_MESSAGE("=2+3", fValue == 5);
    pDoc->GetValue(2,2,0,fValue);
    CPPUNIT_ASSERT_MESSAGE("=2-3", fValue == -1);
    pDoc->GetValue(2,3,0,fValue);
    CPPUNIT_ASSERT_MESSAGE("=C1+C2", fValue == 11);
    //check merged cells import
    SCCOL nCol = 4;
    SCROW nRow = 1;
    pDoc->ExtendMerge(4, 1, nCol, nRow, 0, false);
    CPPUNIT_ASSERT_MESSAGE("merged cells are not imported", nCol == 5 && nRow == 2);
    //check notes import
    ScAddress aAddress(7, 2, 0);
    ScPostIt* pNote = pDoc->GetNote(aAddress);
    CPPUNIT_ASSERT_MESSAGE("note not imported", pNote);
    CPPUNIT_ASSERT_MESSAGE("note text not imported correctly", pNote->GetText() == rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Test")));
    //add additional checks here
}

}

void ScFiltersTest::testContent()
{
    const rtl::OUString aFileNameBase(RTL_CONSTASCII_USTRINGPARAM("universal-content."));
    for (sal_uInt32 i = 0; i < 3; ++i)
    {
        rtl::OUString aFileExtension(aFileFormats[i].pName, strlen(aFileFormats[i].pName), RTL_TEXTENCODING_UTF8 );
        rtl::OUString aFilterName(aFileFormats[i].pFilterName, strlen(aFileFormats[i].pFilterName), RTL_TEXTENCODING_UTF8) ;
        rtl::OUString aFileName;
        createFileURL(aFileNameBase, aFileExtension, aFileName);
        rtl::OUString aFilterType(aFileFormats[i].pTypeName, strlen(aFileFormats[i].pTypeName), RTL_TEXTENCODING_UTF8);
        std::cout << aFileFormats[i].pName << " Test" << std::endl;
        ScDocShellRef xDocSh = load (aFilterName, aFileName, rtl::OUString(), aFilterType, aFileFormats[i].nFormatType);
        xDocSh->DoHardRecalc(true);

        CPPUNIT_ASSERT_MESSAGE("Failed to load universal-content.*", xDocSh.Is());
        ScDocument* pDoc = xDocSh->GetDocument();
        testContentImpl(pDoc);
        xDocSh->DoClose();
    }
}

void ScFiltersTest::testFunctions()
{
    const rtl::OUString aFileNameBase(RTL_CONSTASCII_USTRINGPARAM("functions."));
    rtl::OUString aFileExtension(aFileFormats[0].pName, strlen(aFileFormats[0].pName), RTL_TEXTENCODING_UTF8 );
    rtl::OUString aFilterName(aFileFormats[0].pFilterName, strlen(aFileFormats[0].pFilterName), RTL_TEXTENCODING_UTF8) ;
    rtl::OUString aFileName;
    createFileURL(aFileNameBase, aFileExtension, aFileName);
    rtl::OUString aFilterType(aFileFormats[0].pTypeName, strlen(aFileFormats[0].pTypeName), RTL_TEXTENCODING_UTF8);
    std::cout << aFileFormats[0].pName << " Test" << std::endl;
    ScDocShellRef xDocSh = load (aFilterName, aFileName, rtl::OUString(), aFilterType, aFileFormats[0].nFormatType);
    xDocSh->DoHardRecalc(true);

    CPPUNIT_ASSERT_MESSAGE("Failed to load functions.*", xDocSh.Is());
    ScDocument* pDoc = xDocSh->GetDocument();
    rtl::OUString aCSVFileName;

    //test logical functions
    createCSVPath(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("logical-functions.")), aCSVFileName);
    testFile(aCSVFileName, pDoc, 0);
    //test spreadsheet functions
    createCSVPath(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("spreadsheet-functions.")), aCSVFileName);
    testFile(aCSVFileName, pDoc, 1);
    //test mathematical functions
    createCSVPath(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("mathematical-functions.")), aCSVFileName);
    testFile(aCSVFileName, pDoc, 2, PureString);

    xDocSh->DoClose();
}

void ScFiltersTest::testDatabaseRanges()
{
    const rtl::OUString aFileNameBase(RTL_CONSTASCII_USTRINGPARAM("database."));
    rtl::OUString aFileExtension(aFileFormats[0].pName, strlen(aFileFormats[0].pName), RTL_TEXTENCODING_UTF8 );
    rtl::OUString aFilterName(aFileFormats[0].pFilterName, strlen(aFileFormats[0].pFilterName), RTL_TEXTENCODING_UTF8) ;
    rtl::OUString aFileName;
    createFileURL(aFileNameBase, aFileExtension, aFileName);
    rtl::OUString aFilterType(aFileFormats[0].pTypeName, strlen(aFileFormats[0].pTypeName), RTL_TEXTENCODING_UTF8);
    std::cout << aFileFormats[0].pName << " Test" << std::endl;
    ScDocShellRef xDocSh = load (aFilterName, aFileName, rtl::OUString(), aFilterType, aFileFormats[0].nFormatType);
    xDocSh->DoHardRecalc(true);

    CPPUNIT_ASSERT_MESSAGE("Failed to load database.*", xDocSh.Is());
    ScDocument* pDoc = xDocSh->GetDocument();
    ScDBCollection* pDBCollection = pDoc->GetDBCollection();
    CPPUNIT_ASSERT_MESSAGE("no database collection", pDBCollection);

    ScDBData* pAnonDBData = pDoc->GetAnonymousDBData(0);
    CPPUNIT_ASSERT_MESSAGE("missing anonymous DB data in sheet 1", pAnonDBData);
    //control hidden rows
    bool bHidden;
    SCROW nRow1, nRow2;
    bHidden = pDoc->RowHidden(0, 0, &nRow1, &nRow2);
    CPPUNIT_ASSERT_MESSAGE("Sheet1: row 0 should be visible", !bHidden && nRow1 == 0 && nRow2 == 0);
    bHidden = pDoc->RowHidden(1, 0, &nRow1, &nRow2);
    CPPUNIT_ASSERT_MESSAGE("Sheet1: rows 1-2 should be hidden", bHidden && nRow1 == 1 && nRow2 == 2);
    bHidden = pDoc->RowHidden(3, 0, &nRow1, &nRow2);
    CPPUNIT_ASSERT_MESSAGE("Sheet1: row 3 should be visible", !bHidden && nRow1 == 3 && nRow2 == 3);
    bHidden = pDoc->RowHidden(4, 0, &nRow1, &nRow2);
    CPPUNIT_ASSERT_MESSAGE("Sheet1: row 4-5 should be hidden", bHidden && nRow1 == 4 && nRow2 == 5);
    bHidden = pDoc->RowHidden(6, 0, &nRow1, &nRow2);
    CPPUNIT_ASSERT_MESSAGE("Sheet1: row 6-end should be visible", !bHidden && nRow1 == 6 && nRow2 == MAXROW);
    double aValue;
    pDoc->GetValue(0,10,1, aValue);
    rtl::OUString aString;
    pDoc->GetFormula(0,10,1,aString);
    rtl::OString aOString;
    aOString = rtl::OUStringToOString(aString, RTL_TEXTENCODING_UTF8);
    CPPUNIT_ASSERT_MESSAGE("Sheet2: A11: formula result is incorrect", aValue == 4);
    pDoc->GetValue(1, 10, 1, aValue);
    CPPUNIT_ASSERT_MESSAGE("Sheet2: B11: formula result is incorrect", aValue == 2);
    xDocSh->DoClose();
}

void ScFiltersTest::testFormats()
{
    const rtl::OUString aFileNameBase(RTL_CONSTASCII_USTRINGPARAM("formats."));
    for(int i = 0; i < 3; ++i)
    {
        rtl::OUString aFileExtension(aFileFormats[i].pName, strlen(aFileFormats[i].pName), RTL_TEXTENCODING_UTF8 );
        rtl::OUString aFilterName(aFileFormats[i].pFilterName, strlen(aFileFormats[i].pFilterName), RTL_TEXTENCODING_UTF8) ;
        rtl::OUString aFileName;
        createFileURL(aFileNameBase, aFileExtension, aFileName);
        rtl::OUString aFilterType(aFileFormats[i].pTypeName, strlen(aFileFormats[i].pTypeName), RTL_TEXTENCODING_UTF8);
        std::cout << aFileFormats[i].pName << " Test" << std::endl;
        ScDocShellRef xDocSh = load (aFilterName, aFileName, rtl::OUString(), aFilterType, aFileFormats[i].nFormatType);
        xDocSh->DoHardRecalc(true);

        CPPUNIT_ASSERT_MESSAGE("Failed to load formats.*", xDocSh.Is());
        ScDocument* pDoc = xDocSh->GetDocument();

        //test Sheet1 with csv file
        rtl::OUString aCSVFileName;
        createCSVPath(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("numberFormat.")), aCSVFileName);
        testFile(aCSVFileName, pDoc, 0, PureString);
        //need to test the color of B3
        //it's not a font color!
        //formatting for B5: # ??/100 gets lost during import

        //test Sheet2
        const ScPatternAttr* pPattern = NULL;
        pPattern = pDoc->GetPattern(0,0,1);
        Font aFont;
        pPattern->GetFont(aFont,SC_AUTOCOL_RAW);
        CPPUNIT_ASSERT_MESSAGE("font size should be 10", aFont.GetSize().getHeight() == 200);
        CPPUNIT_ASSERT_MESSAGE("font color should be black", aFont.GetColor() == COL_AUTO);
        pPattern = pDoc->GetPattern(0,1,1);
        pPattern->GetFont(aFont, SC_AUTOCOL_RAW);
        CPPUNIT_ASSERT_MESSAGE("font size should be 12", aFont.GetSize().getHeight() == 240);
        pPattern = pDoc->GetPattern(0,2,1);
        pPattern->GetFont(aFont, SC_AUTOCOL_RAW);
        CPPUNIT_ASSERT_MESSAGE("font should be italic",aFont.GetItalic() == ITALIC_NORMAL);
        pPattern = pDoc->GetPattern(0,4,1);
        pPattern->GetFont(aFont, SC_AUTOCOL_RAW);
        CPPUNIT_ASSERT_MESSAGE("font should be bold",aFont.GetWeight() == WEIGHT_BOLD );
        pPattern = pDoc->GetPattern(1,0,1);
        pPattern->GetFont(aFont, SC_AUTOCOL_RAW);
        CPPUNIT_ASSERT_MESSAGE("font should be blue", aFont.GetColor() == COL_BLUE );
        pPattern = pDoc->GetPattern(1,1,1);
        pPattern->GetFont(aFont, SC_AUTOCOL_RAW);
        CPPUNIT_ASSERT_MESSAGE("font should be striked out with a single line", aFont.GetStrikeout() == STRIKEOUT_SINGLE );
        //test double strikeout only for ods
        if (i == ODS)
        {
            pPattern = pDoc->GetPattern(1,2,1);
            pPattern->GetFont(aFont, SC_AUTOCOL_RAW);
            CPPUNIT_ASSERT_MESSAGE("font should be striked out with a double line", aFont.GetStrikeout() == STRIKEOUT_DOUBLE );
            pPattern = pDoc->GetPattern(1,3,1);
            pPattern->GetFont(aFont, SC_AUTOCOL_RAW);
            CPPUNIT_ASSERT_MESSAGE("font should be underlined with a dotted line", aFont.GetUnderline() == UNDERLINE_DOTTED);
        }
        pPattern = pDoc->GetPattern(1,4,1);
        Color aColor = static_cast<const SvxBrushItem&>(pPattern->GetItem(ATTR_BACKGROUND)).GetColor();
        CPPUNIT_ASSERT_MESSAGE("background color should be green", aColor == COL_LIGHTGREEN);
        pPattern = pDoc->GetPattern(2,0,1);
        SvxCellHorJustify eHorJustify = static_cast<SvxCellHorJustify>(static_cast<const SvxHorJustifyItem&>(pPattern->GetItem(ATTR_HOR_JUSTIFY)).GetValue());
        CPPUNIT_ASSERT_MESSAGE("cell content should be aligned centre horizontally", eHorJustify == SVX_HOR_JUSTIFY_CENTER);
        //test alignment
        pPattern = pDoc->GetPattern(2,1,1);
        eHorJustify = static_cast<SvxCellHorJustify>(static_cast<const SvxHorJustifyItem&>(pPattern->GetItem(ATTR_HOR_JUSTIFY)).GetValue());
        CPPUNIT_ASSERT_MESSAGE("cell content should be aligned right horizontally", eHorJustify == SVX_HOR_JUSTIFY_RIGHT);
        pPattern = pDoc->GetPattern(2,2,1);
        eHorJustify = static_cast<SvxCellHorJustify>(static_cast<const SvxHorJustifyItem&>(pPattern->GetItem(ATTR_HOR_JUSTIFY)).GetValue());
        CPPUNIT_ASSERT_MESSAGE("cell content should be aligned block horizontally", eHorJustify == SVX_HOR_JUSTIFY_BLOCK);

        //test Sheet3 only for ods
        if ( i == ODS )
        {
            rtl::OUString aCondString = getConditionalFormatString(pDoc, 3,0,2);
            std::cerr << rtl::OUStringToOString(aCondString, RTL_TEXTENCODING_UTF8).getStr() << std::endl;
            createCSVPath(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("conditionalFormatting.")), aCSVFileName);
            testCondFile(aCSVFileName, pDoc, 2);
        }
        xDocSh->DoClose();
    }
}

void ScFiltersTest::testMatrix()
{
    const rtl::OUString aFileNameBase(RTL_CONSTASCII_USTRINGPARAM("matrix."));
    for (int i = 0; i < 2; ++i)
    {
        rtl::OUString aFileExtension(aFileFormats[0].pName, strlen(aFileFormats[0].pName), RTL_TEXTENCODING_UTF8 );
        rtl::OUString aFilterName(aFileFormats[0].pFilterName, strlen(aFileFormats[0].pFilterName), RTL_TEXTENCODING_UTF8) ;
        rtl::OUString aFileName;
        createFileURL(aFileNameBase, aFileExtension, aFileName);
        rtl::OUString aFilterType(aFileFormats[0].pTypeName, strlen(aFileFormats[0].pTypeName), RTL_TEXTENCODING_UTF8);
        std::cout << aFileFormats[0].pName << " Test" << std::endl;
        ScDocShellRef xDocSh = load (aFilterName, aFileName, rtl::OUString(), aFilterType, aFileFormats[0].nFormatType);

        CPPUNIT_ASSERT_MESSAGE("Failed to load matrix.*", xDocSh.Is());
        ScDocument* pDoc = xDocSh->GetDocument();


        rtl::OUString aCSVFileName;
        createCSVPath(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("matrix.")), aCSVFileName);
        testFile(aCSVFileName, pDoc, 0);

        xDocSh->DoClose();
    }
}

void ScFiltersTest::testBugFixesODS()
{
    const rtl::OUString aFileNameBase(RTL_CONSTASCII_USTRINGPARAM("bug-fixes."));
    rtl::OUString aFileExtension(aFileFormats[0].pName, strlen(aFileFormats[0].pName), RTL_TEXTENCODING_UTF8 );
    rtl::OUString aFilterName(aFileFormats[0].pFilterName, strlen(aFileFormats[0].pFilterName), RTL_TEXTENCODING_UTF8) ;
    rtl::OUString aFileName;
    createFileURL(aFileNameBase, aFileExtension, aFileName);
    rtl::OUString aFilterType(aFileFormats[0].pTypeName, strlen(aFileFormats[0].pTypeName), RTL_TEXTENCODING_UTF8);
    std::cout << aFileFormats[0].pName << " Test" << std::endl;
    ScDocShellRef xDocSh = load (aFilterName, aFileName, rtl::OUString(), aFilterType, aFileFormats[0].nFormatType);

    CPPUNIT_ASSERT_MESSAGE("Failed to load bugFixes.ods", xDocSh.Is());
    ScDocument* pDoc = xDocSh->GetDocument();
    CPPUNIT_ASSERT_MESSAGE("No Document", pDoc); //remove with first test
    xDocSh->DoClose();
}

void ScFiltersTest::testBugFixesXLS()
{
    const rtl::OUString aFileNameBase(RTL_CONSTASCII_USTRINGPARAM("bug-fixes."));
    rtl::OUString aFileExtension(aFileFormats[1].pName, strlen(aFileFormats[1].pName), RTL_TEXTENCODING_UTF8 );
    rtl::OUString aFilterName(aFileFormats[1].pFilterName, strlen(aFileFormats[1].pFilterName), RTL_TEXTENCODING_UTF8) ;
    rtl::OUString aFileName;
    createFileURL(aFileNameBase, aFileExtension, aFileName);
    rtl::OUString aFilterType(aFileFormats[1].pTypeName, strlen(aFileFormats[1].pTypeName), RTL_TEXTENCODING_UTF8);
    std::cout << aFileFormats[1].pName << " Test" << std::endl;
    ScDocShellRef xDocSh = load (aFilterName, aFileName, rtl::OUString(), aFilterType, aFileFormats[1].nFormatType);

    CPPUNIT_ASSERT_MESSAGE("Failed to load bugFixes.xls", xDocSh.Is());
    ScDocument* pDoc = xDocSh->GetDocument();
    CPPUNIT_ASSERT_MESSAGE("No Document", pDoc); //remove with first test
    xDocSh->DoClose();
}

void ScFiltersTest::testBugFixesXLSX()
{
    const rtl::OUString aFileNameBase(RTL_CONSTASCII_USTRINGPARAM("bug-fixes."));
    rtl::OUString aFileExtension(aFileFormats[2].pName, strlen(aFileFormats[2].pName), RTL_TEXTENCODING_UTF8 );
    rtl::OUString aFilterName(aFileFormats[2].pFilterName, strlen(aFileFormats[2].pFilterName), RTL_TEXTENCODING_UTF8) ;
    rtl::OUString aFileName;
    createFileURL(aFileNameBase, aFileExtension, aFileName);
    rtl::OUString aFilterType(aFileFormats[2].pTypeName, strlen(aFileFormats[2].pTypeName), RTL_TEXTENCODING_UTF8);
    std::cout << aFileFormats[2].pName << " Test" << std::endl;
    ScDocShellRef xDocSh = load (aFilterName, aFileName, rtl::OUString(), aFilterType, aFileFormats[2].nFormatType);

    CPPUNIT_ASSERT_MESSAGE("Failed to load bugFixes.xlsx", xDocSh.Is());
    ScDocument* pDoc = xDocSh->GetDocument();
    CPPUNIT_ASSERT_MESSAGE("No Document", pDoc); //remove with first test
    xDocSh->DoClose();
}

void ScFiltersTest::testStarBasic()
{
    const rtl::OUString aFileNameBase(RTL_CONSTASCII_USTRINGPARAM("StarBasic."));
    rtl::OUString aFileExtension(aFileFormats[0].pName, strlen(aFileFormats[0].pName), RTL_TEXTENCODING_UTF8 );
    rtl::OUString aFilterName(aFileFormats[0].pFilterName, strlen(aFileFormats[0].pFilterName), RTL_TEXTENCODING_UTF8) ;
    rtl::OUString aFileName;
    createFileURL(aFileNameBase, aFileExtension, aFileName);
    rtl::OUString aFilterType(aFileFormats[0].pTypeName, strlen(aFileFormats[0].pTypeName), RTL_TEXTENCODING_UTF8);
    std::cout << aFileFormats[0].pName << " Test" << std::endl;
    ScDocShellRef xDocSh = load (aFilterName, aFileName, rtl::OUString(), aFilterType, aFileFormats[0].nFormatType);

    CPPUNIT_ASSERT_MESSAGE("Failed to load StarBasic.ods", xDocSh.Is());

    rtl::OUString aURL(RTL_CONSTASCII_USTRINGPARAM("vnd.sun.Star.script:Standard.Module1.Macro1?language=Basic&location=document"));
    String sUrl = aURL;
    Any aRet;
    Sequence< sal_Int16 > aOutParamIndex;
    Sequence< Any > aOutParam;
    Sequence< uno::Any > aParams;
    ScDocument* pDoc = xDocSh->GetDocument();

    xDocSh->CallXScript(sUrl, aParams, aRet, aOutParamIndex,aOutParam);
    double aValue;
    pDoc->GetValue(0,0,0,aValue);
    std::cout << aValue << std::endl;
    CPPUNIT_ASSERT_MESSAGE("script did not change the value of Sheet1.A1",aValue==2);
    xDocSh->DoClose();
}

void ScFiltersTest::testVba()
{
    const rtl::OUString aFileNameBase(RTL_CONSTASCII_USTRINGPARAM("vba."));
    rtl::OUString aFileExtension(aFileFormats[1].pName, strlen(aFileFormats[1].pName), RTL_TEXTENCODING_UTF8 );
    rtl::OUString aFilterName(aFileFormats[1].pFilterName, strlen(aFileFormats[1].pFilterName), RTL_TEXTENCODING_UTF8) ;
    rtl::OUString aFileName;
    createFileURL(aFileNameBase, aFileExtension, aFileName);
    rtl::OUString aFilterType(aFileFormats[1].pTypeName, strlen(aFileFormats[1].pTypeName), RTL_TEXTENCODING_UTF8);
    std::cout << aFileFormats[1].pName << " Test" << std::endl;
    ScDocShellRef xDocSh = load (aFilterName, aFileName, rtl::OUString(), aFilterType, aFileFormats[1].nFormatType);

    CPPUNIT_ASSERT_MESSAGE("Failed to load vba.xls", xDocSh.Is());

    //is it really the right way to call a vba macro through CallXScript?
    //it seems that the basic ide does it differently, but then we would need to init all parts ourself
    //the problem is that CallXScript inits the basic part
    ////BasicIDE::RunMethod takes an SbMethod as parametre
    rtl::OUString aURL(RTL_CONSTASCII_USTRINGPARAM("vnd.sun.Star.script:VBAProject.Modul1.Modul1?language=Basic&location=document"));
    String sUrl = aURL;
    Any aRet;
    Sequence< sal_Int16 > aOutParamIndex;
    Sequence< Any > aOutParam;
    Sequence< uno::Any > aParams;
    ScDocument* pDoc = xDocSh->GetDocument();

    xDocSh->CallXScript(sUrl, aParams, aRet, aOutParamIndex,aOutParam);
    double aValue;
    pDoc->GetValue(0,0,0,aValue);
    std::cout << aValue << std::endl;
    CPPUNIT_ASSERT_MESSAGE("script did not change the value of Sheet1.A1",aValue==2);
    xDocSh->DoClose();
}

ScFiltersTest::ScFiltersTest()
      : m_aBaseString(RTL_CONSTASCII_USTRINGPARAM("/sc/qa/unit/data"))
{
}

void ScFiltersTest::setUp()
{
    test::BootstrapFixture::setUp();

    // This is a bit of a fudge, we do this to ensure that ScGlobals::ensure,
    // which is a private symbol to us, gets called
    m_xCalcComponent =
        getMultiServiceFactory()->createInstance(rtl::OUString(
        RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.Calc.SpreadsheetDocument")));
    CPPUNIT_ASSERT_MESSAGE("no calc component!", m_xCalcComponent.is());
}

void ScFiltersTest::tearDown()
{
    uno::Reference< lang::XComponent >( m_xCalcComponent, UNO_QUERY_THROW )->dispose();
    test::BootstrapFixture::tearDown();
}

CPPUNIT_TEST_SUITE_REGISTRATION(ScFiltersTest);

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
