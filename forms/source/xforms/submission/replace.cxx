/*************************************************************************
 *
 *  $RCSfile: replace.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2005-03-23 11:39:50 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#include "submission.hxx"

#include <rtl/ustring.hxx>
#include <rtl/string.hxx>

#include <unotools/processfactory.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/xml/dom/XDocument.hpp>
#include <com/sun/star/xml/dom/XDocumentBuilder.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>

using namespace rtl;
using namespace com::sun::star::uno;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::xml::dom;

CSubmission::SubmissionResult CSubmission::replace(const OUString& aReplace, const Reference<XDocument>& aDocument, const Reference<XFrame>& aFrame)
{
    try {
        Reference< XMultiServiceFactory > xFactory = utl::getProcessServiceFactory();
        if (aReplace.equalsIgnoreAsciiCaseAscii("all") || aReplace.equalsIgnoreAsciiCaseAscii("document")) {
            Reference< XComponentLoader > xLoader;
            if (aFrame.is())
                xLoader = Reference< XComponentLoader >(aFrame, UNO_QUERY);

            if (!xLoader.is())
                xLoader = Reference< XComponentLoader >(xFactory->createInstance(
                    OUString::createFromAscii("com.sun.star.frame.Desktop")), UNO_QUERY_THROW);

            // open the stream from the result...
            // build media descriptor
            Sequence< PropertyValue > descriptor(2);
            descriptor[0] = PropertyValue(OUString::createFromAscii(
                "InputStream"), -1, makeAny(m_aResultStream), PropertyState_DIRECT_VALUE);
            descriptor[1] = PropertyValue(OUString::createFromAscii(
                "ReadOnly"), -1, makeAny(sal_True), PropertyState_DIRECT_VALUE);

            //OUString aURL = OUString::createFromAscii("private:stream");
            OUString aURL = m_aURLObj.GetMainURL(INetURLObject::NO_DECODE);
            OUString aTarget = OUString::createFromAscii("_default");
            xLoader->loadComponentFromURL(aURL, aTarget, FrameSearchFlag::ALL, descriptor);

            return CSubmission::SUCCESS;

        } else if (aReplace.equalsIgnoreAsciiCaseAscii("instance")) {
            if (aDocument.is()) {
                // parse the result stream into a new document
                Reference< XDocumentBuilder > xBuilder(xFactory->createInstance(
                    OUString::createFromAscii("com.sun.star.xml.dom.DocumentBuilder")), UNO_QUERY_THROW);
                Reference< XDocument > aNewDocument = xBuilder->parse(m_aResultStream);

                // and replace the content of the current instance
                Reference< XElement > oldRoot = aDocument->getDocumentElement();
                Reference< XElement > newRoot = aNewDocument->getDocumentElement();

                // aDocument->removeChild(Reference< XNode >(oldRoot, UNO_QUERY_THROW));
                Reference< XNode > aImportedNode = aDocument->importNode(Reference< XNode >(newRoot, UNO_QUERY_THROW), sal_True);
                Reference< XNode >(aDocument, UNO_QUERY_THROW)->replaceChild(aImportedNode, Reference< XNode >(oldRoot, UNO_QUERY_THROW));
                return CSubmission::SUCCESS;
            } else {
                // nothing to replace
                return CSubmission::UNKNOWN_ERROR;
            }
        } else if (aReplace.equalsIgnoreAsciiCaseAscii("none")) {
            // do nothing \o/
            return CSubmission::SUCCESS;
        }
    } catch (Exception& e) {
        OString aMsg("Exception during replace:\n");
        aMsg += OUStringToOString(e.Message, RTL_TEXTENCODING_UTF8);
        OSL_ENSURE(sal_False, aMsg.getStr());
    }
    return CSubmission::UNKNOWN_ERROR;
}
