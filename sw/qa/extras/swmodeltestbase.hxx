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
 *       Miklos Vajna <vmiklos@suse.cz> (SUSE, Inc.)
 * Portions created by the Initial Developer are Copyright (C) 2012 the
 * Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */

#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#include <com/sun/star/text/XTextDocument.hpp>

#include <test/bootstrapfixture.hxx>
#include <unotest/macros_test.hxx>
#include <rtl/ustrbuf.hxx>

#include <unotxdoc.hxx>
#include <docsh.hxx>
#include <doc.hxx>
#include <rootfrm.hxx>

#include <libxml/xmlwriter.h>
#include <libxml/xpath.h>

using namespace com::sun::star;

/// Base class for filter tests loading or roundtriping a document, then asserting the document model.
class SwModelTestBase : public test::BootstrapFixture, public unotest::MacrosTest
{
public:
    SwModelTestBase()
        : mpXmlBuffer(0)
    {
    }

    ~SwModelTestBase()
    {
        if (mpXmlBuffer)
            xmlBufferFree(mpXmlBuffer);
    }

    virtual void setUp()
    {
        test::BootstrapFixture::setUp();

        mxDesktop.set(getMultiServiceFactory()->createInstance("com.sun.star.frame.Desktop"), uno::UNO_QUERY);
        CPPUNIT_ASSERT(mxDesktop.is());
    }

    virtual void tearDown()
    {
        if (mxComponent.is())
            mxComponent->dispose();

        test::BootstrapFixture::tearDown();
    }

private:
    void dumpLayout()
    {
        // create the xml writer
        mpXmlBuffer = xmlBufferCreate();
        xmlTextWriterPtr pXmlWriter = xmlNewTextWriterMemory(mpXmlBuffer, 0);
        xmlTextWriterStartDocument(pXmlWriter, NULL, NULL, NULL);

        // create the dump
        SwXTextDocument* pTxtDoc = dynamic_cast<SwXTextDocument *>(mxComponent.get());
        SwDoc* pDoc = pTxtDoc->GetDocShell()->GetDoc();
        SwRootFrm* pLayout = pDoc->GetCurrentLayout();
        pLayout->dumpAsXml(pXmlWriter);

        // delete xml writer
        xmlTextWriterEndDocument(pXmlWriter);
        xmlFreeTextWriter(pXmlWriter);
    }


protected:
    /// Get the length of the whole document.
    int getLength()
    {
        uno::Reference<text::XTextDocument> xTextDocument(mxComponent, uno::UNO_QUERY);
        uno::Reference<container::XEnumerationAccess> xParaEnumAccess(xTextDocument->getText(), uno::UNO_QUERY);
        uno::Reference<container::XEnumeration> xParaEnum = xParaEnumAccess->createEnumeration();
        rtl::OUStringBuffer aBuf;
        while (xParaEnum->hasMoreElements())
        {
            uno::Reference<container::XEnumerationAccess> xRangeEnumAccess(xParaEnum->nextElement(), uno::UNO_QUERY);
            uno::Reference<container::XEnumeration> xRangeEnum = xRangeEnumAccess->createEnumeration();
            while (xRangeEnum->hasMoreElements())
            {
                uno::Reference<text::XTextRange> xRange(xRangeEnum->nextElement(), uno::UNO_QUERY);
                aBuf.append(xRange->getString());
            }
        }
        return aBuf.getLength();
    }

    /// Get a family of styles, see com.sun.star.style.StyleFamilies for possible values.
    uno::Reference<container::XNameAccess> getStyles(rtl::OUString aFamily)
    {
        uno::Reference<style::XStyleFamiliesSupplier> xStyleFamiliesSupplier(mxComponent, uno::UNO_QUERY);
        uno::Reference<container::XNameAccess> xStyleFamilies(xStyleFamiliesSupplier->getStyleFamilies(), uno::UNO_QUERY);
        uno::Reference<container::XNameAccess> xStyleFamily(xStyleFamilies->getByName(aFamily), uno::UNO_QUERY);
        return xStyleFamily;
    }

    /// Extract a value from the layout dump using an XPath expression and an attribute name.
    rtl::OUString parseDump(rtl::OString aXPath, rtl::OString aAttribute)
    {
        if (!mpXmlBuffer)
            dumpLayout();

        xmlDocPtr pXmlDoc = xmlParseMemory((const char*)xmlBufferContent(mpXmlBuffer), xmlBufferLength(mpXmlBuffer));;

        xmlXPathContextPtr pXmlXpathCtx = xmlXPathNewContext(pXmlDoc);
        xmlXPathObjectPtr pXmlXpathObj = xmlXPathEvalExpression(BAD_CAST(aXPath.getStr()), pXmlXpathCtx);
        xmlNodeSetPtr pXmlNodes = pXmlXpathObj->nodesetval;
        xmlNodePtr pXmlNode = pXmlNodes->nodeTab[0];
        rtl::OUString aRet = rtl::OUString::createFromAscii((const char*)xmlGetProp(pXmlNode, BAD_CAST(aAttribute.getStr())));

        xmlFreeDoc(pXmlDoc);

        return aRet;
    }

    template< typename T >
    T getProperty( uno::Any obj, const rtl::OUString& name ) const
    {
        uno::Reference< beans::XPropertySet > properties( obj, uno::UNO_QUERY );
        T data;
        properties->getPropertyValue( name ) >>= data;
        return data;
    }

    template< typename T >
    T getProperty( uno::Reference< uno::XInterface > obj, const rtl::OUString& name ) const
    {
        uno::Reference< beans::XPropertySet > properties( obj, uno::UNO_QUERY );
        T data;
        properties->getPropertyValue( name ) >>= data;
        return data;
    }

    uno::Reference<lang::XComponent> mxComponent;
    xmlBufferPtr mpXmlBuffer;
};

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
