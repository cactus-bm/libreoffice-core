/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_TEST_UNOAPI_TEST_HXX
#define INCLUDED_TEST_UNOAPI_TEST_HXX

#include <sal/config.h>

#include <string_view>

#include <test/bootstrapfixture.hxx>
#include <unotest/macros_test.hxx>
#include <com/sun/star/lang/XComponent.hpp>
#include <osl/file.hxx>
#include <unotools/tempfile.hxx>

// basic uno api test class

class OOO_DLLPUBLIC_TEST UnoApiTest : public test::BootstrapFixture, public unotest::MacrosTest
{
public:
    UnoApiTest(OUString path);

    OUString createFileURL(std::u16string_view aFileBase);
    OUString loadFromURL(std::u16string_view aFileBase);

    virtual void setUp() override;
    virtual void tearDown() override;

    css::uno::Any executeMacro(const OUString& rScriptURL,
                               const css::uno::Sequence<css::uno::Any>& rParams = {});

    utl::TempFileNamed save(const OUString& rFilter);
    utl::TempFileNamed saveAndClose(const OUString& rFilter);
    utl::TempFileNamed saveAndReload(const OUString& rFilter);

    void setFilterOptions(const OUString& rFilterOptions) { maFilterOptions = rFilterOptions; }

protected:
    // reference to document component that we are testing
    css::uno::Reference<css::lang::XComponent> mxComponent;

    bool mbSkipValidation; // if you set this flag for a new test I'm going to haunt you!

private:
    OUString m_aBaseString;

    OUString maFilterOptions;
};

#endif // INCLUDED_TEST_UNOAPI_TEST_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
