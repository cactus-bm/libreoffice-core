/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include "protectorfactory.hxx"
#include "osl/module.h"
#include "osl/module.hxx"
#include "osl/thread.h"
#include "rtl/process.h"
#include "rtl/string.h"
#include "rtl/string.hxx"
#include "rtl/textcvt.h"
#include "rtl/ustring.hxx"
#include "sal/main.h"
#include "sal/types.h"

#include "cppunit/CompilerOutputter.h"
#include "cppunit/TestResult.h"
#include "cppunit/TestResultCollector.h"
#include "cppunit/TestRunner.h"
#include "cppunit/plugin/TestPlugIn.h"
#include "cppunit/plugin/PlugInParameters.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/portability/Stream.h"
#include "cppunit/plugin/DynamicLibraryManagerException.h"

#include "boost/noncopyable.hpp"

namespace {

void usageFailure() {
    std::cerr
        << ("Usage: cppunittester (--protector <shared-library-path>"
            " <function-symbol>)* <shared-library-path>")
        << std::endl;
    std::exit(EXIT_FAILURE);
}

rtl::OUString getArgument(sal_Int32 index) {
    rtl::OUString arg;
    rtl_getAppCommandArg(index, &arg.pData);
    return arg;
}

std::string convertLazy(rtl::OUString const & s16) {
    rtl::OString s8(rtl::OUStringToOString(s16, osl_getThreadTextEncoding()));
    return std::string(
        s8.getStr(),
        ((static_cast< sal_uInt32 >(s8.getLength())
          > (std::numeric_limits< std::string::size_type >::max)())
         ? (std::numeric_limits< std::string::size_type >::max)()
         : static_cast< std::string::size_type >(s8.getLength())));
}

//Allow the whole uniting testing framework to be run inside a "Protector"
//which knows about uno exceptions, so it can print the content of the
//exception before falling over and dying
class CPPUNIT_API ProtectedFixtureFunctor : public CppUnit::Functor, private boost::noncopyable
{
private:
    const std::string &args;
    CppUnit::TestResult &result;
public:
    ProtectedFixtureFunctor(const std::string &args_, CppUnit::TestResult &result_)
        : args(args_)
        , result(result_)
    {
    }
    bool run() const
    {
        bool bSuccess = false;
        try {
            CppUnit::TestRunner runner;
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
            CppUnit::TestResultCollector collector;
            result.addListener(&collector);
            runner.run(result);
            CppUnit::CompilerOutputter(&collector, CppUnit::stdCErr()).write();
            bSuccess = collector.wasSuccessful();
        } catch( CppUnit::DynamicLibraryManagerException& e) {
            std::cerr << "DynamicLibraryManagerException: \"" << e.what() << "\"\n";
        }
        return bSuccess;
    }
    virtual bool operator()() const
    {
        return run();
    }
};
}

extern "C" CppUnitTestPlugIn
           *cppunitTest_binaryurp_cache(void),
           *cppunitTest_binaryurp_unmarshal(void);


SAL_IMPLEMENT_MAIN() {
    TestPlugInSignature plugs[] = {
        cppunitTest_binaryurp_cache,
        cppunitTest_binaryurp_unmarshal,
        NULL
    };
    CppUnit::TestResult result;
    std::string args;
    bool ok = false;
    for (TestPlugInSignature *plug = plugs; *plug != NULL; plug++) {
        CppUnitTestPlugIn *iface;
        iface = (*plug)();
        iface->initialize(&CppUnit::TestFactoryRegistry::getRegistry(), CppUnit::PlugInParameters());
    }
    ProtectedFixtureFunctor tests(args, result);
    ok = tests.run();

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
