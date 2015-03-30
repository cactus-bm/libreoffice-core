/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <test/calc_unoapi_test.hxx>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/sheet/XConditionalFormats.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/table/CellAddress.hpp>
#include <unonames.hxx>

using namespace css;

namespace sc_apitest {

#define NUMBER_OF_TESTS 2

class ScConditionalFormatTest : public CalcUnoApiTest
{
public:
    ScConditionalFormatTest();

    virtual void setUp() SAL_OVERRIDE;
    virtual void tearDown() SAL_OVERRIDE;

    uno::Reference< uno::XInterface > init(sal_Int32 nIndex = 0);
    void testRequestCondFormatListFromSheet();
    void testCondFormatListProperties();
    void testCondFormatListFormats();

    CPPUNIT_TEST_SUITE(ScConditionalFormatTest);
    CPPUNIT_TEST(testRequestCondFormatListFromSheet);
    CPPUNIT_TEST(testCondFormatListProperties);
    CPPUNIT_TEST(testCondFormatListFormats);
    CPPUNIT_TEST_SUITE_END();
private:

    static sal_Int32 nTest;
    static uno::Reference< lang::XComponent > mxComponent;
};

sal_Int32 ScConditionalFormatTest::nTest = 0;
uno::Reference< lang::XComponent > ScConditionalFormatTest::mxComponent;

ScConditionalFormatTest::ScConditionalFormatTest()
    : CalcUnoApiTest("sc/qa/extras/testdocuments/")
{
}

uno::Reference< uno::XInterface > ScConditionalFormatTest::init(sal_Int32 nIndex)
{
    if(!mxComponent.is())
    {
        // get the test file
        OUString aFileURL;
        createFileURL(OUString("new_cond_format_api.ods"), aFileURL);
        mxComponent = loadFromDesktop(aFileURL);
    }
    CPPUNIT_ASSERT_MESSAGE("Component not loaded", mxComponent.is());

    // get the first sheet
    uno::Reference< sheet::XSpreadsheetDocument > xDoc(mxComponent, uno::UNO_QUERY_THROW);
    uno::Reference< container::XIndexAccess > xIndex (xDoc->getSheets(), uno::UNO_QUERY_THROW);
    uno::Reference< sheet::XSpreadsheet > xSheet( xIndex->getByIndex(nIndex), uno::UNO_QUERY_THROW);

    return xSheet;
}

void ScConditionalFormatTest::testRequestCondFormatListFromSheet()
{
    uno::Reference<sheet::XSpreadsheet> xSheet(init(), uno::UNO_QUERY_THROW);
    uno::Reference<beans::XPropertySet> xProps(xSheet, uno::UNO_QUERY_THROW);
    uno::Any aAny = xProps->getPropertyValue("ConditionalFormats");
    uno::Reference<sheet::XConditionalFormats> xCondFormats;
    CPPUNIT_ASSERT(aAny >>= xCondFormats);
    CPPUNIT_ASSERT(xCondFormats.is());
}

namespace {

uno::Reference<sheet::XConditionalFormats> getConditionalFormatList(uno::Reference<uno::XInterface> xInterface)
{
    uno::Reference<sheet::XSpreadsheet> xSheet(xInterface, uno::UNO_QUERY_THROW);
    uno::Reference<beans::XPropertySet> xProps(xSheet, uno::UNO_QUERY_THROW);
    uno::Any aAny = xProps->getPropertyValue("ConditionalFormats");
    uno::Reference<sheet::XConditionalFormats> xCondFormats;
    CPPUNIT_ASSERT(aAny >>= xCondFormats);
    CPPUNIT_ASSERT(xCondFormats.is());

    return xCondFormats;
}

}

void ScConditionalFormatTest::testCondFormatListProperties()
{
    uno::Reference<sheet::XConditionalFormats> xCondFormat =
        getConditionalFormatList(init());
    CPPUNIT_ASSERT_EQUAL(sal_Int32(4), xCondFormat->getLength());
}

void ScConditionalFormatTest::testCondFormatListFormats()
{
    uno::Reference<sheet::XConditionalFormats> xCondFormatList =
        getConditionalFormatList(init());

    uno::Sequence<uno::Reference<sheet::XConditionalFormat> > xCondFormats =
        xCondFormatList->getConditionalFormats();
    CPPUNIT_ASSERT_EQUAL(sal_Int32(4), xCondFormats.getLength());
    for (sal_Int32 i = 0, n = xCondFormats.getLength(); i < n; ++i)
    {
        CPPUNIT_ASSERT(xCondFormats[i].is());
    }
}

void ScConditionalFormatTest::setUp()
{
    nTest++;
    CalcUnoApiTest::setUp();
}

void ScConditionalFormatTest::tearDown()
{
    if (nTest == NUMBER_OF_TESTS)
    {
        closeDocument(mxComponent);
        mxComponent.clear();
    }

    CalcUnoApiTest::tearDown();
}

CPPUNIT_TEST_SUITE_REGISTRATION(ScConditionalFormatTest);

}

CPPUNIT_PLUGIN_IMPLEMENT();


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
