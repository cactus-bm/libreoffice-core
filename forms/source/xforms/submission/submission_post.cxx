/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: submission_post.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: ihi $ $Date: 2007-06-05 17:45:38 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_forms.hxx"

#include <memory>

#include "submission_post.hxx"
#include "serialization_app_xml.hxx"
#include "serialization_urlencoded.hxx"

#include <osl/file.hxx>
#include <unotools/processfactory.hxx>
#include <ucbhelper/content.hxx>
#include <ucbhelper/activedatasink.hxx>
#include <com/sun/star/ucb/PostCommandArgument2.hpp>

using namespace CSS::uno;
using namespace CSS::ucb;
using namespace CSS::task;
using namespace CSS::io;
using namespace rtl;
using namespace osl;
using namespace ucbhelper;
using namespace std;


CSubmissionPost::CSubmissionPost(const rtl::OUString& aURL, const CSS::uno::Reference< CSS::xml::dom::XDocumentFragment >& aFragment)
    : CSubmission(aURL, aFragment)
    , m_aFactory(utl::getProcessServiceFactory())
{
}

CSubmission::SubmissionResult CSubmissionPost::submit(const CSS::uno::Reference< CSS::task::XInteractionHandler >& aInteractionHandler)
{
    // PUT always uses application/xml
    auto_ptr< CSerialization > apSerialization(new CSerializationAppXML());
    apSerialization->setSource(m_aFragment);
    apSerialization->serialize();

    // create a commandEnvironment and use the default interaction handler
    CCommandEnvironmentHelper *pHelper = new CCommandEnvironmentHelper;
    if( aInteractionHandler.is() )
        pHelper->m_aInteractionHandler = aInteractionHandler;
    else
        pHelper->m_aInteractionHandler = CSS::uno::Reference< XInteractionHandler >(m_aFactory->createInstance(
            OUString::createFromAscii("com.sun.star.task.InteractionHandler")), UNO_QUERY);
    OSL_ENSURE(pHelper->m_aInteractionHandler.is(), "failed to create IntreractionHandler");
    CProgressHandlerHelper *pProgressHelper = new CProgressHandlerHelper;
    pHelper->m_aProgressHandler = CSS::uno::Reference< XProgressHandler >(pProgressHelper);
    // UCB has ownership of environment...
    CSS::uno::Reference< XCommandEnvironment > aEnvironment(pHelper);

    try {
        ucbhelper::Content aContent(m_aURLObj.GetMainURL(INetURLObject::NO_DECODE), aEnvironment);

        // use post command

        OUString aCommandName = OUString::createFromAscii("post");
        PostCommandArgument2 aPostArgument;
        aPostArgument.Source = apSerialization->getInputStream();
        //CSS::uno::Reference< XInterface > aSink( m_aFactory->createInstance(
        //    OUString::createFromAscii("com.sun.star.io.Pipe")), UNO_QUERY_THROW);
        CSS::uno::Reference< XActiveDataSink > aSink(new ucbhelper::ActiveDataSink);
        //    OUString::createFromAscii("com.sun.star.io.Pipe")), UNO_QUERY_THROW);
        aPostArgument.Sink = aSink;
        aPostArgument.MediaType = OUString::createFromAscii("application/xml");
        aPostArgument.Referer = OUString();
        Any aCommandArgument;
        aCommandArgument <<= aPostArgument;
        aContent.executeCommand( aCommandName, aCommandArgument);

        // wait for command to finish
        // pProgressHelper->m_cFinished.wait();

        // CSS::uno::Reference< XOutputStream > xOut(aSink, UNO_QUERY_THROW);
        // xOut->closeOutput();

        try {
            // m_aResultStream = CSS::uno::Reference< XInputStream >(aSink, UNO_QUERY_THROW);
            m_aResultStream = aSink->getInputStream();
        } catch (Exception&) {
            OSL_ENSURE(sal_False, "Cannot open reply stream from content");
        }
    } catch (Exception&)
    {
        // XXX
        OSL_ENSURE(sal_False, "Exception during UCB operatration.");
        return UNKNOWN_ERROR;
    }


    return SUCCESS;
}

