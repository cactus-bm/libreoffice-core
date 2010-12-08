/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_starmath.hxx"

#include "sal/config.h"

#include <cppuhelper/bootstrap.hxx>
#include <comphelper/processfactory.hxx>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XDesktop.hpp>

#include <vcl/svapp.hxx>
#include <smdll.hxx>
#include <document.hxx>
#include <view.hxx>

#include <sfx2/sfxmodelfactory.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/request.hxx>
#include <sfx2/dispatch.hxx>

#include <svl/stritem.hxx>

#include <editeng/editeng.hxx>
#include <editeng/editview.hxx>

#include <svx/zoomitem.hxx>

#include "preextstl.h"
#include <cppunit/TestSuite.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestCase.h>
#include <cppunit/plugin/TestPlugIn.h>
#include <cppunit/extensions/HelperMacros.h>
#include "postextstl.h"

SO2_DECL_REF(SmDocShell)
SO2_IMPL_REF(SmDocShell)

using namespace ::com::sun::star;

namespace {

class Test : public CppUnit::TestFixture {
public:
    Test();
    ~Test();

    // init
    virtual void setUp();
    virtual void tearDown();

    // tests
    void tmEditUndoRedo();
    void tmEditAllClipboard();
    void tmEditMarker();
    void tmEditFailure();

    void tViewZoom();

    CPPUNIT_TEST_SUITE(Test);
    CPPUNIT_TEST(tmEditUndoRedo);
    CPPUNIT_TEST(tmEditAllClipboard);
    CPPUNIT_TEST(tmEditMarker);
    CPPUNIT_TEST(tmEditFailure);
    CPPUNIT_TEST(tViewZoom);
    CPPUNIT_TEST_SUITE_END();

private:
    uno::Reference<uno::XComponentContext> m_xContext;
    uno::Reference<lang::XMultiComponentFactory> m_xFactory;

    SfxBindings m_aBindings;
    SfxDispatcher *m_pDispatcher;
    SmCmdBoxWindow *m_pSmCmdBoxWindow;
    SmEditWindow *m_pEditWindow;
    SmDocShellRef m_xDocShRef;
    SmViewShell *m_pViewShell;
};

Test::Test()
    : m_pDispatcher(NULL)
    , m_pSmCmdBoxWindow(NULL)
    , m_pEditWindow(NULL)
{
    m_xContext = cppu::defaultBootstrap_InitialComponentContext();
    m_xFactory = m_xContext->getServiceManager();

    uno::Reference<lang::XMultiServiceFactory> xSM(m_xFactory, uno::UNO_QUERY_THROW);

    //Without this we're crashing because callees are using
    //getProcessServiceFactory.  In general those should be removed in favour
    //of retaining references to the root ServiceFactory as its passed around
    comphelper::setProcessServiceFactory(xSM);

    InitVCL(xSM);

    SmDLL::Init();
}

void Test::setUp()
{
    m_xDocShRef = new SmDocShell(
        SFXMODEL_STANDARD |
        SFXMODEL_DISABLE_EMBEDDED_SCRIPTS |
        SFXMODEL_DISABLE_DOCUMENT_RECOVERY);
    m_xDocShRef->DoInitNew(0);

    SfxViewFrame *pViewFrame = SfxViewFrame::LoadHiddenDocument(*m_xDocShRef, 0);

    CPPUNIT_ASSERT_MESSAGE("Should have a SfxViewFrame", pViewFrame);

    m_pDispatcher = new SfxDispatcher(pViewFrame);
    m_aBindings.SetDispatcher(m_pDispatcher);
    m_pSmCmdBoxWindow = new SmCmdBoxWindow(&m_aBindings, NULL, NULL);
    m_pEditWindow = new SmEditWindow(*m_pSmCmdBoxWindow);
    m_pViewShell = m_pEditWindow->GetView();
    CPPUNIT_ASSERT_MESSAGE("Should have a SmViewShell", m_pViewShell);
}

void Test::tearDown()
{
    delete m_pEditWindow;
    delete m_pSmCmdBoxWindow;
    delete m_pDispatcher;
    m_xDocShRef.Clear();
}

Test::~Test()
{
    uno::Reference< lang::XComponent >(m_xContext, uno::UNO_QUERY_THROW)->dispose();
}

void Test::tmEditMarker()
{
    {
        rtl::OUString sMarkedText(RTL_CONSTASCII_USTRINGPARAM("<?> under <?> under <?>"));
        m_pEditWindow->SetText(sMarkedText);
        m_pEditWindow->Flush();
        rtl::OUString sFinalText = m_pEditWindow->GetText();
        CPPUNIT_ASSERT_MESSAGE("Should be equal text", sFinalText == sMarkedText);
    }

    {
        rtl::OUString sTargetText(RTL_CONSTASCII_USTRINGPARAM("a under b under c"));

        m_pEditWindow->SelNextMark();
        m_pEditWindow->Cut();
        m_pEditWindow->InsertText(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("a")));

        m_pEditWindow->SelNextMark();
        m_pEditWindow->SelNextMark();
        m_pEditWindow->Cut();
        m_pEditWindow->InsertText(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("c")));

        m_pEditWindow->SelPrevMark();
        m_pEditWindow->Cut();
        m_pEditWindow->InsertText(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("b")));

        m_pEditWindow->Flush();
        rtl::OUString sFinalText = m_pEditWindow->GetText();
        CPPUNIT_ASSERT_MESSAGE("Should be a under b under c", sFinalText == sTargetText);
    }

    {
        m_pEditWindow->SetText(rtl::OUString());
        m_pEditWindow->Flush();
    }
}

void Test::tmEditAllClipboard()
{
    rtl::OUString sOriginalText(RTL_CONSTASCII_USTRINGPARAM("a over b"));

    {
        m_pEditWindow->SetText(sOriginalText);
        m_pEditWindow->Flush();
        rtl::OUString sFinalText = m_pEditWindow->GetText();
        CPPUNIT_ASSERT_MESSAGE("Should be equal text", sFinalText == sOriginalText);
    }

    {
        m_pEditWindow->SelectAll();
        m_pEditWindow->Cut();
        m_pEditWindow->Flush();
        rtl::OUString sFinalText = m_pEditWindow->GetText();
        CPPUNIT_ASSERT_MESSAGE("Should be empty", !sFinalText.getLength());
    }

    {
        m_pEditWindow->Paste();
        m_pEditWindow->Flush();
        rtl::OUString sFinalText = m_pEditWindow->GetText();
        CPPUNIT_ASSERT_MESSAGE("Should be equal text", sFinalText == sOriginalText);
    }

    {
        m_pEditWindow->SelectAll();
        m_pEditWindow->Copy();
    }

    {
        rtl::OUString sExpectedText(RTL_CONSTASCII_USTRINGPARAM("a over ba over b"));

        m_pEditWindow->Paste();
        m_pEditWindow->Paste();
        m_pEditWindow->Flush();
        rtl::OUString sFinalText = m_pEditWindow->GetText();
        CPPUNIT_ASSERT_MESSAGE("Should be equal text", sFinalText == sExpectedText);
    }

    {
        m_pEditWindow->SetText(rtl::OUString());
        m_pEditWindow->Flush();
    }
}

void Test::tmEditFailure()
{
    m_xDocShRef->SetText(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("color a b over {a/}")));

    const SmErrorDesc *pErrorDesc = m_xDocShRef->GetParser().NextError();

    CPPUNIT_ASSERT_MESSAGE("Should be a PE_UNEXPECTED_CHAR",
        pErrorDesc && pErrorDesc->Type == PE_UNEXPECTED_CHAR);

    pErrorDesc = m_xDocShRef->GetParser().PrevError();

    CPPUNIT_ASSERT_MESSAGE("Should be a PE_RGROUP_EXPECTED",
        pErrorDesc && pErrorDesc->Type == PE_RGROUP_EXPECTED);

    pErrorDesc = m_xDocShRef->GetParser().PrevError();

    CPPUNIT_ASSERT_MESSAGE("Should be a PE_COLOR_EXPECTED",
        pErrorDesc && pErrorDesc->Type == PE_COLOR_EXPECTED);

    const SmErrorDesc *pLastErrorDesc = m_xDocShRef->GetParser().PrevError();

    CPPUNIT_ASSERT_MESSAGE("Should be three syntax errors",
        pLastErrorDesc && pLastErrorDesc == pErrorDesc);
}

void Test::tmEditUndoRedo()
{
    EditEngine &rEditEngine = m_xDocShRef->GetEditEngine();

    rtl::OUString sStringOne(RTL_CONSTASCII_USTRINGPARAM("a under b"));
    {
        rEditEngine.SetText(0, sStringOne);
        m_xDocShRef->UpdateText();
        rtl::OUString sFinalText = m_xDocShRef->GetText();
        CPPUNIT_ASSERT_MESSAGE("Strings must match", sStringOne == sFinalText);
    }

    rtl::OUString sStringTwo(RTL_CONSTASCII_USTRINGPARAM("a over b"));
    {
        rEditEngine.SetText(0, sStringTwo);
        m_xDocShRef->UpdateText();
        rtl::OUString sFinalText = m_xDocShRef->GetText();
        CPPUNIT_ASSERT_MESSAGE("Strings must match", sStringTwo == sFinalText);
    }

    SfxRequest aUndo(SID_UNDO, SFX_CALLMODE_SYNCHRON, m_xDocShRef->GetPool());

    {
        m_xDocShRef->Execute(aUndo);
        rtl::OUString sFoo = rEditEngine.GetText();
        m_xDocShRef->UpdateText();
        rtl::OUString sFinalText = m_xDocShRef->GetText();
        CPPUNIT_ASSERT_MESSAGE("Strings much match", sStringOne == sFinalText);
    }

    {
        m_xDocShRef->Execute(aUndo);
        rtl::OUString sFoo = rEditEngine.GetText();
        m_xDocShRef->UpdateText();
        rtl::OUString sFinalText = m_xDocShRef->GetText();
        CPPUNIT_ASSERT_MESSAGE("Must now be empty", !sFinalText.getLength());
    }

    SfxRequest aRedo(SID_REDO, SFX_CALLMODE_SYNCHRON, m_xDocShRef->GetPool());
    {
        m_xDocShRef->Execute(aRedo);
        rtl::OUString sFoo = rEditEngine.GetText();
        m_xDocShRef->UpdateText();
        rtl::OUString sFinalText = m_xDocShRef->GetText();
        CPPUNIT_ASSERT_MESSAGE("Strings much match", sStringOne == sFinalText);
    }

    {
        rEditEngine.SetText(0, rtl::OUString());
        m_xDocShRef->UpdateText();
        rEditEngine.ClearModifyFlag();
        rtl::OUString sFinalText = m_xDocShRef->GetText();
        CPPUNIT_ASSERT_MESSAGE("Must be empty", !sFinalText.getLength());
    }

}

void Test::tViewZoom()
{
    sal_uInt16 nOrigZoom, nNextZoom, nFinalZoom;

    EditEngine &rEditEngine = m_xDocShRef->GetEditEngine();

    rtl::OUString sStringOne(RTL_CONSTASCII_USTRINGPARAM("a under b"));
    {
        rEditEngine.SetText(0, sStringOne);
        m_xDocShRef->UpdateText();
        rtl::OUString sFinalText = m_xDocShRef->GetText();
        CPPUNIT_ASSERT_MESSAGE("Strings must match", sStringOne == sFinalText);
    }

    SmGraphicWindow &rGraphicWindow = m_pViewShell->GetGraphicWindow();
    rGraphicWindow.SetSizePixel(Size(1024, 800));
    nOrigZoom = rGraphicWindow.GetZoom();

    {
        SfxRequest aZoomIn(SID_ZOOMIN, SFX_CALLMODE_SYNCHRON, m_pViewShell->GetPool());
        m_pViewShell->Execute(aZoomIn);
        nNextZoom = rGraphicWindow.GetZoom();
        CPPUNIT_ASSERT_MESSAGE("Should be bigger", nNextZoom > nOrigZoom);
    }

    {
        SfxRequest aZoomOut(SID_ZOOMOUT, SFX_CALLMODE_SYNCHRON, m_pViewShell->GetPool());
        m_pViewShell->Execute(aZoomOut);
        nFinalZoom = rGraphicWindow.GetZoom();
        CPPUNIT_ASSERT_MESSAGE("Should be equal", nFinalZoom == nOrigZoom);
    }

    sal_uInt16 nOptimalZoom=0;

    {
        SfxRequest aZoom(SID_FITINWINDOW, SFX_CALLMODE_SYNCHRON, m_pViewShell->GetPool());
        m_pViewShell->Execute(aZoom);
        nOptimalZoom = rGraphicWindow.GetZoom();
        CPPUNIT_ASSERT_MESSAGE("Should be about 800%", nOptimalZoom > nOrigZoom);
    }

    {
        SfxItemSet aSet(m_xDocShRef->GetPool(), SID_ATTR_ZOOM, SID_ATTR_ZOOM);
        aSet.Put(SvxZoomItem(SVX_ZOOM_OPTIMAL, 0));
        SfxRequest aZoom(SID_ATTR_ZOOM, SFX_CALLMODE_SYNCHRON, aSet);
        m_pViewShell->Execute(aZoom);
        nFinalZoom = rGraphicWindow.GetZoom();
        CPPUNIT_ASSERT_MESSAGE("Should be optimal zoom", nFinalZoom == nOptimalZoom);
    }

    {
        SfxRequest aZoomOut(SID_ZOOMOUT, SFX_CALLMODE_SYNCHRON, m_pViewShell->GetPool());
        m_pViewShell->Execute(aZoomOut);
        nFinalZoom = rGraphicWindow.GetZoom();
        CPPUNIT_ASSERT_MESSAGE("Should not be optimal zoom", nFinalZoom != nOptimalZoom);

        SfxItemSet aSet(m_xDocShRef->GetPool(), SID_ATTR_ZOOM, SID_ATTR_ZOOM);
        aSet.Put(SvxZoomItem(SVX_ZOOM_PAGEWIDTH, 0));
        SfxRequest aZoom(SID_ATTR_ZOOM, SFX_CALLMODE_SYNCHRON, aSet);
        m_pViewShell->Execute(aZoom);
        nFinalZoom = rGraphicWindow.GetZoom();
        CPPUNIT_ASSERT_MESSAGE("Should be same as optimal zoom", nFinalZoom == nOptimalZoom);
    }

    {
        SfxRequest aZoomOut(SID_ZOOMOUT, SFX_CALLMODE_SYNCHRON, m_pViewShell->GetPool());
        m_pViewShell->Execute(aZoomOut);
        nFinalZoom = rGraphicWindow.GetZoom();
        CPPUNIT_ASSERT_MESSAGE("Should not be optimal zoom", nFinalZoom != nOptimalZoom);

        SfxItemSet aSet(m_xDocShRef->GetPool(), SID_ATTR_ZOOM, SID_ATTR_ZOOM);
        aSet.Put(SvxZoomItem(SVX_ZOOM_WHOLEPAGE, 0));
        SfxRequest aZoom(SID_ATTR_ZOOM, SFX_CALLMODE_SYNCHRON, aSet);
        m_pViewShell->Execute(aZoom);
        nFinalZoom = rGraphicWindow.GetZoom();
        CPPUNIT_ASSERT_MESSAGE("Should be same as optimal zoom", nFinalZoom == nOptimalZoom);
    }

    {
        SfxRequest aZoomOut(SID_ZOOMOUT, SFX_CALLMODE_SYNCHRON, m_pViewShell->GetPool());
        m_pViewShell->Execute(aZoomOut);
        nFinalZoom = rGraphicWindow.GetZoom();
        CPPUNIT_ASSERT_MESSAGE("Should not be optimal zoom", nFinalZoom != nOptimalZoom);

        SfxItemSet aSet(m_xDocShRef->GetPool(), SID_ATTR_ZOOM, SID_ATTR_ZOOM);
        aSet.Put(SvxZoomItem(SVX_ZOOM_PERCENT, 50));
        SfxRequest aZoom(SID_ATTR_ZOOM, SFX_CALLMODE_SYNCHRON, aSet);
        m_pViewShell->Execute(aZoom);
        nFinalZoom = rGraphicWindow.GetZoom();
        CPPUNIT_ASSERT_MESSAGE("Should be 50%", nFinalZoom == 50);
    }

    {
        SfxItemSet aSet(m_xDocShRef->GetPool(), SID_ATTR_ZOOM, SID_ATTR_ZOOM);
        aSet.Put(SvxZoomItem(SVX_ZOOM_PERCENT, 5));
        SfxRequest aZoom(SID_ATTR_ZOOM, SFX_CALLMODE_SYNCHRON, aSet);
        m_pViewShell->Execute(aZoom);
        nFinalZoom = rGraphicWindow.GetZoom();
        CPPUNIT_ASSERT_MESSAGE("Should be Clipped to 25%", nFinalZoom == 25);
    }

    {
        SfxItemSet aSet(m_xDocShRef->GetPool(), SID_ATTR_ZOOM, SID_ATTR_ZOOM);
        aSet.Put(SvxZoomItem(SVX_ZOOM_PERCENT, 1000));
        SfxRequest aZoom(SID_ATTR_ZOOM, SFX_CALLMODE_SYNCHRON, aSet);
        m_pViewShell->Execute(aZoom);
        nFinalZoom = rGraphicWindow.GetZoom();
        CPPUNIT_ASSERT_MESSAGE("Should be Clipped to 800%", nFinalZoom == 800);
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION(Test);

}

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
