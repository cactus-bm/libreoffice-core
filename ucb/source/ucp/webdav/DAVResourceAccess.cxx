/*************************************************************************
 *
 *  $RCSfile: DAVResourceAccess.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: kso $ $Date: 2001-06-25 08:51:54 $
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

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef _COM_SUN_STAR_TASK_XINTERACTIONABORT_HPP_
#include <com/sun/star/task/XInteractionAbort.hpp>
#endif

#ifndef _UCBHELPER_SIMPLEAUTHENTICATIONREQUEST_HXX
#include <ucbhelper/simpleauthenticationrequest.hxx>
#endif

#ifndef _DAVAUTHLISTENER_HXX_
#include "DAVAuthListener.hxx"
#endif
#ifndef _DAVRESOURCEACCESS_HXX_
#include "DAVResourceAccess.hxx"
#endif
#ifndef _NEONURI_HXX_
#include "NeonUri.hxx"
#endif

//=========================================================================
//=========================================================================
//
// class AuthListener.
//
//=========================================================================
//=========================================================================

namespace webdav_ucp
{

//=========================================================================

class AuthListener : public DAVAuthListener
{
public:
    virtual int authenticate( const ::rtl::OUString & inRealm,
                                 const ::rtl::OUString & inHostName,
                                 ::rtl::OUStringBuffer & inUserName,
                                 ::rtl::OUStringBuffer & inPassWord,
                                 const com::sun::star::uno::Reference<
                                   com::sun::star::ucb::XCommandEnvironment >&
                                    Environment );
};

}

using namespace webdav_ucp;
using namespace com::sun::star;

//=========================================================================
//=========================================================================
//
// AuthListener Implementation.
//
//=========================================================================
//=========================================================================

//=========================================================================
// virtual
int AuthListener::authenticate( const ::rtl::OUString & inRealm,
                                   const ::rtl::OUString & inHostName,
                                   ::rtl::OUStringBuffer & inUserName,
                                   ::rtl::OUStringBuffer & inPassWord,
                                   const uno::Reference<
                                       ucb::XCommandEnvironment >&  Environment )
{
    if ( Environment.is() )
    {
        uno::Reference< task::XInteractionHandler > xIH
              = Environment->getInteractionHandler();
        if ( xIH.is() )
        {
            rtl::Reference< ucbhelper::SimpleAuthenticationRequest > xRequest
                = new ucbhelper::SimpleAuthenticationRequest(
                                                inHostName,
                                                inRealm,
                                                rtl::OUString( inUserName ),
                                                rtl::OUString( inPassWord ) );
            xIH->handle( xRequest.get() );

            rtl::Reference< ucbhelper::InteractionContinuation > xSelection
                = xRequest->getSelection();

            if ( xSelection.is() )
            {
                // Handler handled the request.
                uno::Reference< task::XInteractionAbort > xAbort(
                                    xSelection.get(), uno::UNO_QUERY );
                if ( !xAbort.is() )
                {
                    const rtl::Reference<
                        ucbhelper::InteractionSupplyAuthentication > & xSupp
                            = xRequest->getAuthenticationSupplier();

                    inUserName = xSupp->getUserName();
                    inPassWord = xSupp->getPassword();

                    // go on.
                    return 0;
                }
            }
        }
    }
    // Abort.
    return -1;
}

//=========================================================================

AuthListener webdavAuthListener;

//=========================================================================
//=========================================================================
//
// DAVResourceAccess Implementation.
//
//=========================================================================
//=========================================================================

//=========================================================================
DAVResourceAccess::DAVResourceAccess(
                const uno::Reference< lang::XMultiServiceFactory > & rSMgr,
                DAVSessionFactory* pSessionFactory,
                const rtl::OUString & rURL )
    throw( DAVException )
: m_xSMgr( rSMgr ),
  m_pSessionFactory( pSessionFactory )
{
    if ( !initialize( rURL ) )
    {
        NeonUri theUri( rURL );
        throw DAVException( DAVException::DAV_SESSION_CREATE,
                            theUri.makeConnectionEndPointString() );
    }
}

//=========================================================================
void DAVResourceAccess::setURL( const rtl::OUString & rNewURL )
    throw( DAVException )
{
    if ( !initialize( rNewURL ) )
    {
        NeonUri theUri( rNewURL );
        throw DAVException( DAVException::DAV_SESSION_CREATE,
                            theUri.makeConnectionEndPointString() );
    }
}

//=========================================================================
void DAVResourceAccess::OPTIONS( DAVCapabilities & rCapabilities,
                                   const uno::Reference<
                                      ucb::XCommandEnvironment > & xEnv )
    throw( DAVException )
{
    sal_Bool bRetry;
    do
    {
        bRetry = sal_False;
        try
        {
            m_xSession->OPTIONS( m_aPath, rCapabilities, xEnv );
        }
        catch ( DAVException & e )
        {
            bRetry = handleException( e );
            if ( !bRetry )
                throw;
        }
    }
    while ( bRetry );
}

//=========================================================================
void DAVResourceAccess::PROPFIND( const Depth nDepth,
                          const std::vector< rtl::OUString > & rPropertyNames,
                          std::vector< DAVResource > & rResources,
                          const uno::Reference<
                                  ucb::XCommandEnvironment > & xEnv )
    throw( DAVException )
{
    sal_Bool bRetry;
    do
    {
        bRetry = sal_False;
        try
        {
            m_xSession->PROPFIND(
                m_aPath, nDepth, rPropertyNames, rResources, xEnv );
        }
        catch ( DAVException & e )
        {
            bRetry = handleException( e );
            if ( !bRetry )
                throw;
        }
    }
    while ( bRetry );
}

//=========================================================================
void DAVResourceAccess::PROPFIND( const Depth nDepth,
                                    std::vector< DAVResourceInfo > & rResInfo,
                                    const uno::Reference<
                                      ucb::XCommandEnvironment > & xEnv )
    throw( DAVException )
{
    sal_Bool bRetry;
    do
    {
        bRetry = sal_False;
        try
        {
            m_xSession->PROPFIND( m_aPath, nDepth, rResInfo, xEnv );
        }
        catch ( DAVException & e )
        {
            bRetry = handleException( e );
            if ( !bRetry )
                throw;
        }
    }
    while ( bRetry );
}

//=========================================================================
void DAVResourceAccess::PROPPATCH( const std::vector< ProppatchValue >& rValues,
                                      const uno::Reference<
                                       ucb::XCommandEnvironment >& xEnv )
    throw( DAVException )
{
    sal_Bool bRetry;
    do
    {
        bRetry = sal_False;
        try
        {
            m_xSession->PROPPATCH( m_aPath, rValues, xEnv );
        }
        catch ( DAVException & e )
        {
            bRetry = handleException( e );
            if ( !bRetry )
                throw;
        }
    }
    while ( bRetry );
}

//=========================================================================
uno::Reference< io::XInputStream > DAVResourceAccess::GET(
                const uno::Reference< ucb::XCommandEnvironment > & xEnv )
    throw( DAVException )
{
    uno::Reference< io::XInputStream > xStream;
    sal_Bool bRetry;
    do
    {
        bRetry = sal_False;
        try
        {
            xStream = m_xSession->GET( m_aPath, xEnv );
        }
        catch ( DAVException & e )
        {
            bRetry = handleException( e );
            if ( !bRetry )
                throw;
        }
    }
    while ( bRetry );

    return xStream;
}

//=========================================================================
void DAVResourceAccess::GET( uno::Reference< io::XOutputStream > & rStream,
                               const uno::Reference<
                                      ucb::XCommandEnvironment > & xEnv )
    throw( DAVException )
{
    sal_Bool bRetry;
    do
    {
        bRetry = sal_False;
        try
        {
            m_xSession->GET( m_aPath, rStream, xEnv );
        }
        catch ( DAVException & e )
        {
            bRetry = handleException( e );
            if ( !bRetry )
                throw;
        }
    }
    while ( bRetry );
}

//=========================================================================
void DAVResourceAccess::PUT( const uno::Reference< io::XInputStream > & rStream,
                               const uno::Reference<
                                     ucb::XCommandEnvironment > & xEnv )
    throw( DAVException )
{
    sal_Bool bRetry;
    do
    {
        bRetry = sal_False;
        try
        {
            m_xSession->PUT( m_aPath, rStream, xEnv );
        }
        catch ( DAVException & e )
        {
            bRetry = handleException( e );
            if ( !bRetry )
                throw;
        }
    }
    while ( bRetry );
}

//=========================================================================
void DAVResourceAccess::MKCOL( const uno::Reference<
                                      ucb::XCommandEnvironment > & xEnv )
    throw( DAVException )
{
    sal_Bool bRetry;
    do
    {
        bRetry = sal_False;
        try
        {
            m_xSession->MKCOL( m_aPath, xEnv );
        }
        catch ( DAVException & e )
        {
            bRetry = handleException( e );
            if ( !bRetry )
                throw;
        }
    }
    while ( bRetry );
}

//=========================================================================
void DAVResourceAccess::COPY( const ::rtl::OUString & rSourcePath,
                              const ::rtl::OUString & rDestinationURI,
                                sal_Bool bOverwrite,
                                const uno::Reference<
                                          ucb::XCommandEnvironment > & xEnv )
    throw( DAVException )
{
    sal_Bool bRetry;
    do
    {
        bRetry = sal_False;
        try
        {
            m_xSession->COPY( rSourcePath, rDestinationURI, xEnv, bOverwrite );
        }
        catch ( DAVException & e )
        {
            bRetry = handleException( e );
            if ( !bRetry )
                throw;
        }
    }
    while ( bRetry );
}

//=========================================================================
void DAVResourceAccess::MOVE( const ::rtl::OUString & rSourcePath,
                              const ::rtl::OUString & rDestinationURI,
                                sal_Bool bOverwrite,
                                const uno::Reference<
                                          ucb::XCommandEnvironment > & xEnv )
    throw( DAVException )
{
    sal_Bool bRetry;
    do
    {
        bRetry = sal_False;
        try
        {
            m_xSession->MOVE( rSourcePath, rDestinationURI, xEnv, bOverwrite );
        }
        catch ( DAVException & e )
        {
            bRetry = handleException( e );
            if ( !bRetry )
                throw;
        }
    }
    while ( bRetry );
}

//=========================================================================
void DAVResourceAccess::DESTROY( const uno::Reference<
                                       ucb::XCommandEnvironment > & xEnv )
    throw( DAVException )
{
    sal_Bool bRetry;
    do
    {
        bRetry = sal_False;
        try
        {
            m_xSession->DESTROY( m_aPath, xEnv );
        }
        catch ( DAVException & e )
        {
            bRetry = handleException( e );
            if ( !bRetry )
                throw;
        }
    }
    while ( bRetry );
}

//=========================================================================
void DAVResourceAccess::LOCK ( const ucb::Lock & rLock,
                                 const uno::Reference<
                                      ucb::XCommandEnvironment > & xEnv )
    throw( DAVException )
{
    OSL_ENSURE( sal_False, "DAVResourceAccess::LOCK - NYI" );
}

//=========================================================================
void DAVResourceAccess::UNLOCK ( const ucb::Lock & rLock,
                                   const uno::Reference<
                                       ucb::XCommandEnvironment > & xEnv )
    throw( DAVException )
{
    OSL_ENSURE( sal_False, "DAVResourceAccess::UNLOCK - NYI" );
}

//=========================================================================
// init dav session and path
sal_Bool DAVResourceAccess::initialize( const rtl::OUString & rURL )
    throw ( DAVException )
{
    NeonUri aURI( rURL );

    m_aPath = aURI.GetPath();
    if ( !m_aPath.getLength() )
      {
        OSL_ENSURE( sal_False, "DAVResourceAccess::initialize - Invalid URL " );
        return sal_False;
      }

    // set the webdav session
    m_xSession = m_pSessionFactory->createDAVSession( rURL, m_xSMgr );
    m_xSession->setServerAuthListener( &webdavAuthListener );

    return sal_True;
}

//=========================================================================
sal_Bool DAVResourceAccess::handleException( DAVException & e )
{
    switch ( e.getError() )
    {
        case DAVException::DAV_HTTP_REDIRECT:
            try
            {
                // set new path and session
                return initialize( e.getData() );
            }
            catch( DAVException const & )
            {
            }
            return sal_False;

        case DAVException::DAV_HTTP_AUTH:
        case DAVException::DAV_HTTP_AUTHPROXY:
        case DAVException::DAV_HTTP_SERVERAUTH:
        case DAVException::DAV_HTTP_PROXYAUTH:
            // Retry. If user cancels the login request,
            // we will get DAV_HTTP_ERROR and abort.
            return sal_True;

        default:
            return sal_False; // Abort
    }
}
