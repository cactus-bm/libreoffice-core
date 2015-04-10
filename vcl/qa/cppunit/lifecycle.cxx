/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <unotest/filters-test.hxx>
#include <test/bootstrapfixture.hxx>

#include <vcl/wrkwin.hxx>
#include <vcl/button.hxx>
#include <vcl/edit.hxx>
#include <vcl/combobox.hxx>
#include <vcl/field.hxx>
#include <vcl/virdev.hxx>

class LifecycleTest : public test::BootstrapFixture
{
    void testWidgets(vcl::Window *pParent);

public:
    LifecycleTest() : BootstrapFixture(true, false) {}

    void testCast();
    void testVirtualDevice();
    void testMultiDispose();
    void testIsolatedWidgets();
    void testParentedWidgets();
    void testChildDispose();
    void testPostDispose();

    CPPUNIT_TEST_SUITE(LifecycleTest);
    CPPUNIT_TEST(testCast);
    CPPUNIT_TEST(testVirtualDevice);
    CPPUNIT_TEST(testMultiDispose);
    CPPUNIT_TEST(testIsolatedWidgets);
    CPPUNIT_TEST(testParentedWidgets);
    CPPUNIT_TEST(testChildDispose);
    CPPUNIT_TEST(testPostDispose);
    CPPUNIT_TEST_SUITE_END();
};

// A compile time sanity check
void LifecycleTest::testCast()
{
    VclPtr<PushButton> xButton(new PushButton(NULL, 0));
    VclPtr<vcl::Window> xWindow(xButton);

    VclPtr<MetricField> xField(new MetricField(NULL, 0));
    VclPtr<SpinField> xSpin(xField);
    VclPtr<Edit> xEdit(xField);

// the following line should NOT compile
//    VclPtr<PushButton> xButton2(xWindow);
}

void LifecycleTest::testVirtualDevice()
{
    VclPtr<VirtualDevice> pVDev( new VirtualDevice() );
    ScopedVclPtr<VirtualDevice> pVDev2( new VirtualDevice() );
    VclPtrInstance<VirtualDevice> pVDev3;
    VclPtrInstance<VirtualDevice> pVDev4( 1 );
    CPPUNIT_ASSERT(!!pVDev && !!pVDev2 && !!pVDev3 && !!pVDev4);
}

void LifecycleTest::testMultiDispose()
{
    VclPtrInstance<WorkWindow> xWin(nullptr, WB_APP|WB_STDWORK);
    CPPUNIT_ASSERT(xWin.get() != NULL);
    xWin->disposeOnce();
    xWin->disposeOnce();
    xWin->disposeOnce();
    CPPUNIT_ASSERT(xWin->GetWindow(0) == NULL);
    CPPUNIT_ASSERT(xWin->GetChild(0) == NULL);
    CPPUNIT_ASSERT(xWin->GetChildCount() == 0);
}

void LifecycleTest::testWidgets(vcl::Window *pParent)
{
    { VclPtr<PushButton>   aPtr(new PushButton(pParent));   }
    { VclPtr<OKButton>     aPtr(new OKButton(pParent));     }
    { VclPtr<CancelButton> aPtr(new CancelButton(pParent)); }
    { VclPtr<HelpButton>   aPtr(new HelpButton(pParent));   }

    // Some widgets really insist on adoption.
    if (pParent)
    {
        { VclPtr<CheckBox>     aPtr(new CheckBox(pParent));     }
        { VclPtr<Edit>         aPtr(new Edit(pParent));         }
        { VclPtr<ComboBox>     aPtr(new ComboBox(pParent));     }
        { VclPtr<RadioButton>  aPtr(new RadioButton(pParent));  }
    }
}

void LifecycleTest::testIsolatedWidgets()
{
    testWidgets(NULL);
}

void LifecycleTest::testParentedWidgets()
{
    ScopedVclPtrInstance<WorkWindow> xWin(new WorkWindow((vcl::Window *)NULL,
                                                         WB_APP|WB_STDWORK));
    CPPUNIT_ASSERT(xWin.get() != NULL);
    xWin->Show();
    testWidgets(xWin);
}

class DisposableChild : public vcl::Window
{
public:
    DisposableChild(vcl::Window *pParent) : vcl::Window(pParent) {}
    virtual ~DisposableChild()
    {
        disposeOnce();
    }
};

void LifecycleTest::testChildDispose()
{
    VclPtrInstance<WorkWindow> xWin((vcl::Window *)NULL,
                                    WB_APP|WB_STDWORK);
    CPPUNIT_ASSERT(xWin.get() != NULL);
    VclPtr<DisposableChild> xChild(new DisposableChild(xWin.get()));
    xWin->Show();
    xChild->disposeOnce();
    xWin->disposeOnce();
}

void LifecycleTest::testPostDispose()
{
    VclPtrInstance<WorkWindow> xWin((vcl::Window *)NULL, WB_STDWORK);
    xWin->disposeOnce();

    // check selected methods continue to work post-dispose
    CPPUNIT_ASSERT(!xWin->GetParent());
    xWin->Show();
    CPPUNIT_ASSERT(!xWin->IsReallyShown());
    CPPUNIT_ASSERT(!xWin->IsEnabled());
    CPPUNIT_ASSERT(!xWin->IsInputEnabled());
    CPPUNIT_ASSERT(!xWin->GetChild(0));
    CPPUNIT_ASSERT(!xWin->GetWindow(0));
}

CPPUNIT_TEST_SUITE_REGISTRATION(LifecycleTest);

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
