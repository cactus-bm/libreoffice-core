/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SC_QA_HELPER_HXX
#define SC_QA_HELPER_HXX

#define SC_DLLIMPLEMENTATION

#include "scdllapi.h"
#include "debughelper.hxx"
#include "address.hxx"
#include "docsh.hxx"

#include <test/bootstrapfixture.hxx>
#include <comphelper/documentconstants.hxx>

#include <osl/detail/android-bootstrap.h>

#include <unotools/tempfile.hxx>
#include <comphelper/storagehelper.hxx>
#include <sfx2/docfilt.hxx>
#include "sfx2/docfile.hxx"
#include "svl/stritem.hxx"

#include <string>
#include <sstream>

#define ODS_FORMAT_TYPE 50331943
#define XLS_FORMAT_TYPE 318767171
#define XLSX_FORMAT_TYPE 268959811
#define LOTUS123_FORMAT_TYPE 268435649
#define CSV_FORMAT_TYPE  (SFX_FILTER_IMPORT | SFX_FILTER_EXPORT | SFX_FILTER_ALIEN | SFX_FILTER_USESOPTIONS)
#define HTML_FORMAT_TYPE (SFX_FILTER_IMPORT | SFX_FILTER_EXPORT | SFX_FILTER_ALIEN | SFX_FILTER_USESOPTIONS)
#define DIF_FORMAT_TYPE 195

#define ODS      0
#define XLS      1
#define XLSX     2
#define CSV      3
#define HTML     4
#define LOTUS123 5
#define DIF      6

enum StringType { PureString, FormulaValue, StringValue };

SC_DLLPUBLIC bool testEqualsWithTolerance( long nVal1, long nVal2, long nTol );

#define CHECK_OPTIMAL 0x1

// data format for row height tests
struct TestParam
{
    struct RowData
    {
        SCROW nStartRow;
        SCROW nEndRow;
        SCTAB nTab;
        int nExpectedHeight; // -1 for default height
        int nCheck; // currently only CHECK_OPTIMAL ( we could add CHECK_MANUAL etc.)
        bool bOptimal;
    };
    const char* sTestDoc;
    int nImportType;
    int nExportType; // -1 for import test, otherwise this is an export test
    int nRowData;
    RowData* pData;
};

struct FileFormat {
    const char* pName; const char* pFilterName; const char* pTypeName; unsigned int nFormatType;
};

SC_DLLPUBLIC extern FileFormat aFileFormats[];

// Why is this here and not in osl, and using the already existing file
// handling APIs? Do we really want to add arbitrary new file handling
// wrappers here and there (and then having to handle the Android (and
// eventually perhaps iOS) special cases here, too)?  Please move this to osl,
// it sure looks gemerally useful. Or am I missing something?

SC_DLLPUBLIC void loadFile(const OUString& aFileName, std::string& aContent);

SC_DLLPUBLIC void testFile(OUString& aFileName, ScDocument* pDoc, SCTAB nTab, StringType aStringFormat = StringValue);

//need own handler because conditional formatting strings must be generated
SC_DLLPUBLIC void testCondFile(OUString& aFileName, ScDocument* pDoc, SCTAB nTab);

SC_DLLPUBLIC void clearRange(ScDocument* pDoc, const ScRange& rRange);

inline std::string print(const ScAddress& rAddr)
{
    std::ostringstream str;
    str << "Col: " << rAddr.Col();
    str << " Row: " << rAddr.Row();
    str << " Tab: " << rAddr.Tab();
    return str.str();
}

namespace CppUnit {

template<>
struct assertion_traits<ScRange>
{
    static bool equal( const ScRange& x, const ScRange& y )
    {
        return x == y;
    }

    static std::string toString( const ScRange& x )
    {
        std::stringstream str;
        str << "Start: " << print(x.aStart);
        str << "\nEnd: " << print(x.aEnd);
        return str.str();
    }
};

}

class SC_DLLPUBLIC ScBootstrapFixture : public test::BootstrapFixture
{
protected:
    OUString m_aBaseString;

    ScDocShellRef load(
        bool bReadWrite, const OUString& rURL, const OUString& rFilter, const OUString &rUserData,
        const OUString& rTypeName, unsigned int nFilterFlags, unsigned int nClipboardID,
        sal_uIntPtr nFilterVersion = SOFFICE_FILEFORMAT_CURRENT, const OUString* pPassword = NULL );

    ScDocShellRef load(
        const OUString& rURL, const OUString& rFilter, const OUString &rUserData,
        const OUString& rTypeName, unsigned int nFilterFlags, unsigned int nClipboardID,
        sal_uIntPtr nFilterVersion = SOFFICE_FILEFORMAT_CURRENT, const OUString* pPassword = NULL );

    ScDocShellRef loadDoc(const OUString& rFileName, sal_Int32 nFormat, bool bReadWrite = false );

public:
    ScBootstrapFixture( const OUString& rsBaseString ) : m_aBaseString( rsBaseString ) {}

    void createFileURL(const OUString& aFileBase, const OUString& aFileExtension, OUString& rFilePath);

    void createCSVPath(const OUString& aFileBase, OUString& rCSVPath);

    ScDocShellRef saveAndReload(ScDocShell* pShell, const OUString &rFilter,
    const OUString &rUserData, const OUString& rTypeName, sal_uLong nFormatType);

    ScDocShellRef saveAndReload( ScDocShell* pShell, sal_Int32 nFormat );

    void miscRowHeightsTest( TestParam* aTestValues, unsigned int numElems );
};

#define ASSERT_DOUBLES_EQUAL( expected, result )    \
    CPPUNIT_ASSERT_DOUBLES_EQUAL( (expected), (result), 1e-14 )

#define ASSERT_DOUBLES_EQUAL_MESSAGE( message, expected, result )   \
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE( (message), (expected), (result), 1e-14 )

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
