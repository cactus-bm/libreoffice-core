/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <sal/config.h>

#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#include <com/sun/star/reflection/XServiceConstructorDescription.hpp>
#include <com/sun/star/reflection/XServiceTypeDescription2.hpp>
#include <test/bootstrapfixture.hxx>

using namespace css::container;
using namespace css::reflection;
using namespace css::uno;

namespace {

class ServicesTest: public test::BootstrapFixture
{
public:
    virtual void setUp();
    virtual void tearDown();

    void test();

    CPPUNIT_TEST_SUITE(ServicesTest);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST_SUITE_END();
};

void ServicesTest::setUp()
{
    test::BootstrapFixture::setUp();
}

void ServicesTest::tearDown()
{
    test::BootstrapFixture::tearDown();
}

void ServicesTest::test()
{
    Reference< XHierarchicalNameAccess > xTypeManager(
            m_xContext->getValueByName(
                "/singletons/com.sun.star.reflection.theTypeDescriptionManager"),
            UNO_QUERY_THROW );
    Sequence<OUString> seq = m_xContext->getServiceManager()->getAvailableServiceNames();
    OUString *s = seq.getArray();
    for (sal_Int32 i = 0; i < seq.getLength(); i++)
    {
        if (!xTypeManager->hasByHierarchicalName(s[i]))
        {
            continue;
        }
        Reference< XServiceTypeDescription2 > xDesc(
                xTypeManager->getByHierarchicalName(s[i]), UNO_QUERY);
        if (!xDesc.is())
        {
            // Does happen for singletons?
            continue;
        }
        Sequence< Reference< XServiceConstructorDescription > > xseq = xDesc->getConstructors();
        bool bHasDefault = false;
        for (sal_Int32 c = 0; c < xseq.getLength(); c++)
            if (xseq[c]->isDefaultConstructor())
                bHasDefault = true;

        try
        {
            if (bHasDefault
                    && s[i] != "com.sun.star.deployment.test.SmoketestCommandEnvironment")
                // TODO: com.sun.star.deployment.test.SmoketestCommandEnvironment throws
                // "Can not activate the factory for org.libreoffice.smoketest.SmoketestCommandEnvironment
                // because java.lang.NoClassDefFoundError: com/sun/star/ucb/XCommandEnvironment"
                m_xContext->getServiceManager()->createInstanceWithContext(s[i], m_xContext);
        }
        catch(const Exception & e)
        {
            OString exc = "Exception thrown while creating " +
                OUStringToOString(s[i] + ": " + e.Message, RTL_TEXTENCODING_UTF8);
            CPPUNIT_FAIL(exc.getStr());
        }
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION(ServicesTest);

}

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
