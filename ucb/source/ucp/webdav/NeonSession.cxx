/*************************************************************************
 *
 *  $RCSfile: NeonSession.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: kso $ $Date: 2000-10-16 14:55:20 $
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
#include "NeonSession.hxx"
#include "NeonInputStream.hxx"
#include "NeonPropFindRequest.hxx"
#include "NeonPUTFile.hxx"
#include "NeonUri.hxx"
#include <dav_basic.h>
#include <http_basic.h>

using namespace rtl;
using namespace com::sun::star::uno;
using namespace com::sun::star::io;
using namespace com::sun::star::ucb;
using namespace webdav_ucp;

sal_Bool    NeonSession::sSockInited        = sal_False;

// -------------------------------------------------------------------
// Constructor
// -------------------------------------------------------------------
NeonSession::NeonSession( const OUString & inUri )
{
    Init();
    NeonUri theUri( inUri );

    mHostName = *new OString( OUStringToOString( theUri.GetHost(),
                                   RTL_TEXTENCODING_UTF8 ) );
    mPort = theUri.GetPort();


    mHttpSession = CreateSession( mHostName,
                                  theUri.GetPort() );
    if ( mHttpSession == NULL )
        throw DAVException( DAVException::DAV_SESSION_CREATE );

    // Note: Uncomment the following if locking support is required
    /*
    mNeonLockSession = dav_lock_register( mHttpSession );

    if ( mNeonLockSession == NULL )
        throw DAVException( DAVException::DAV_SESSION_CREATE );
    */
}

// -------------------------------------------------------------------
// Destructor
// -------------------------------------------------------------------
NeonSession::~NeonSession( )
{
    DAVSessionFactory::ReleaseDAVSession( this );

    if ( mHttpSession != NULL )
    {
        http_session_destroy( mHttpSession );
        mHttpSession = NULL;
        // Note: Uncomment the following if locking support is required
        /*
        if ( mNeonLockSession != NULL )
        {
            dav_lock_unregister( mNeonLockSession );
            mNeonLockSession = NULL;
        }
        */
    }
}

sal_Bool NeonSession::CanUse( const OUString & inUri )
{
    sal_Bool IsConnected = sal_False;
    NeonUri theUri( inUri );
    if ( theUri.GetPort() == mPort &&
            OUStringToOString( theUri.GetHost(), RTL_TEXTENCODING_UTF8 ) ==
                mHostName )
         IsConnected = sal_True;
    return IsConnected;
}

void NeonSession::setServerAuthListener(DAVAuthListener * inDAVAuthListener)
{
    if ( inDAVAuthListener != NULL )
    {
        mListener = inDAVAuthListener;
        http_set_server_auth( mHttpSession, NeonAuth, this );
    }
}

void NeonSession::setProxyAuthListener(DAVAuthListener * inDAVAuthListener)
{
    // Note: Content is currently not using proxy auth
}
// -------------------------------------------------------------------
// PROPFIND
// -------------------------------------------------------------------
void NeonSession::PROPFIND( const OUString &                inUri,
                            const Depth                     inDepth,
                            const std::vector< OUString > & inPropNames,
                            std::vector< DAVResource > &    ioResources,
                            const com::sun::star::uno::Reference<
                             com::sun::star::ucb::XCommandEnvironment >& inEnv )
                                    throw ( DAVException )
{
    osl::Guard< osl::Mutex > theGuard( mMutex );

    mEnv = inEnv;

    NeonUri theUri( inUri );
    NeonPropFindRequest theRequest( mHttpSession,
                                    OUStringToOString( theUri.GetPath(),
                                                       RTL_TEXTENCODING_UTF8 ),
                                    inDepth,
                                    inPropNames,
                                    ioResources );
}

// -------------------------------------------------------------------
// GET
// -------------------------------------------------------------------
Reference< XInputStream > NeonSession::GET( const OUString & inUri,
                            const com::sun::star::uno::Reference<
                             com::sun::star::ucb::XCommandEnvironment >& inEnv )
                                throw ( DAVException )
{
    osl::Guard< osl::Mutex > theGuard( mMutex );

    mEnv = inEnv;

    NeonUri theUri( inUri );
    NeonInputStream * theInputStream = new NeonInputStream;
    int theRetVal = http_read_file( mHttpSession,
                                     OUStringToOString( theUri.GetPath(),
                                                       RTL_TEXTENCODING_UTF8 ),
                                     GETReader,
                                     theInputStream );

    if ( theRetVal != HTTP_OK )
        throw DAVException( DAVException::DAV_HTTP_ERROR );

    return theInputStream;
}

// -------------------------------------------------------------------
// GET
// -------------------------------------------------------------------
void NeonSession::GET( const OUString &             inUri,
                       Reference< XOutputStream > & ioOutputStream,
                       const com::sun::star::uno::Reference<
                        com::sun::star::ucb::XCommandEnvironment >& inEnv )
                                throw ( DAVException )
{
    osl::Guard< osl::Mutex > theGuard( mMutex );

    mEnv = inEnv;

    NeonUri theUri( inUri );
    int theRetVal = http_read_file( mHttpSession,
                                     OUStringToOString( theUri.GetPath(),
                                                       RTL_TEXTENCODING_UTF8 ),
                                     GETWriter,
                                     &ioOutputStream );

    if ( theRetVal != HTTP_OK )
        throw DAVException( DAVException::DAV_HTTP_ERROR );
}

// -------------------------------------------------------------------
// PUT
// -------------------------------------------------------------------
void NeonSession::PUT( const OUString &             inUri,
                       Reference< XInputStream > &  inInputStream,
                       const com::sun::star::uno::Reference<
                        com::sun::star::ucb::XCommandEnvironment >& inEnv )
                         throw ( DAVException )
{
    osl::Guard< osl::Mutex > theGuard( mMutex );

    mEnv = inEnv;

    NeonUri theUri( inUri );
    NeonPUTFile thePUTFile( inInputStream );
    int theRetVal = http_put( mHttpSession,
                              OUStringToOString( theUri.GetPath(),
                                                 RTL_TEXTENCODING_UTF8 ),
                              thePUTFile.GetFILE() );
    thePUTFile.Remove();

    if ( theRetVal != HTTP_OK )
        throw DAVException( DAVException::DAV_HTTP_ERROR );
}

// -------------------------------------------------------------------
// MKCOL
// -------------------------------------------------------------------
void NeonSession::MKCOL( const OUString & inUri,
                         const com::sun::star::uno::Reference<
                          com::sun::star::ucb::XCommandEnvironment >& inEnv )
                             throw ( DAVException )
{
    osl::Guard< osl::Mutex > theGuard( mMutex );

    mEnv = inEnv;

    NeonUri theUri( inUri );
    int theRetVal = dav_mkcol( mHttpSession,
                               OUStringToOString( theUri.GetPath(),
                                                  RTL_TEXTENCODING_UTF8 ) );

    if ( theRetVal != HTTP_OK )
        throw DAVException( DAVException::DAV_HTTP_ERROR );
}

// -------------------------------------------------------------------
// COPY
// -------------------------------------------------------------------
void NeonSession::COPY( const OUString &    inSource,
                        const OUString &    inDestination,
                        const com::sun::star::uno::Reference<
                         com::sun::star::ucb::XCommandEnvironment >& inEnv,
                        sal_Bool            inOverWrite )
                                    throw ( DAVException )
{
    osl::Guard< osl::Mutex > theGuard( mMutex );

    mEnv = inEnv;

    NeonUri theSourceUri( inSource );
    NeonUri theDestinationUri( inDestination );

    int theRetVal = dav_copy( mHttpSession,
                              inOverWrite ? 1 : 0,
                              OUStringToOString( theSourceUri.GetPath(),
                                                 RTL_TEXTENCODING_UTF8 ),
                              OUStringToOString( theDestinationUri.GetPath(),
                                                 RTL_TEXTENCODING_UTF8 ) );


    if ( theRetVal != HTTP_OK )
        throw DAVException( DAVException::DAV_HTTP_ERROR );
}

// -------------------------------------------------------------------
// MOVE
// -------------------------------------------------------------------
void NeonSession::MOVE( const OUString &    inSource,
                        const OUString &    inDestination,
                        const com::sun::star::uno::Reference<
                         com::sun::star::ucb::XCommandEnvironment >& inEnv,
                        sal_Bool            inOverWrite )
                throw ( DAVException )
{
    osl::Guard< osl::Mutex > theGuard( mMutex );

    mEnv = inEnv;

    NeonUri theSourceUri( inSource );
    NeonUri theDestinationUri( inDestination );
    int theRetVal = dav_move( mHttpSession,
                                 inOverWrite ? 1 : 0,
                                 OUStringToOString( theSourceUri.GetPath(),
                                                 RTL_TEXTENCODING_UTF8 ),
                                 OUStringToOString( theDestinationUri.GetPath(),
                                                 RTL_TEXTENCODING_UTF8 ) );

    if ( theRetVal != HTTP_OK )
        throw DAVException( DAVException::DAV_HTTP_ERROR );
}


// -------------------------------------------------------------------
// DESTROY
// -------------------------------------------------------------------
void NeonSession::DESTROY( const OUString & inUri,
                           const com::sun::star::uno::Reference<
                            com::sun::star::ucb::XCommandEnvironment >& inEnv )
                                throw ( DAVException )
{
    osl::Guard< osl::Mutex > theGuard( mMutex );

    mEnv = inEnv;

    NeonUri theUri( inUri );
    int theRetVal = dav_delete( mHttpSession,
                                OUStringToOString( theUri.GetPath(),
                                                   RTL_TEXTENCODING_UTF8 ) );

    if ( theRetVal != HTTP_OK )
        throw DAVException( DAVException::DAV_HTTP_ERROR );
}

// -------------------------------------------------------------------
// LOCK
// -------------------------------------------------------------------
// Note: Uncomment the following if locking support is required
/*
void NeonSession::LOCK( const Lock & inLock,
                        const com::sun::star::uno::Reference<
                         com::sun::star::ucb::XCommandEnvironment >& inEnv )
                            throw ( DAVException )
{
    mEnv = inEnv;

    Lockit( inLock, true );
}
*/

// -------------------------------------------------------------------
// UNLOCK
// -------------------------------------------------------------------
// Note: Uncomment the following if locking support is required
/*
void NeonSession::UNLOCK( const Lock & inLock,
                          const com::sun::star::uno::Reference<
                           com::sun::star::ucb::XCommandEnvironment >& inEnv )
                            throw ( DAVException )
{
    Lockit( inLock, false );
}
*/

// -------------------------------------------------------------------
// Init
// Initialises "Neon sockets"
// -------------------------------------------------------------------
void NeonSession::Init( void )
{
    if ( sSockInited == sal_False )
    {
        if ( sock_init() != 0 )
            throw DAVException( DAVException::DAV_SESSION_CREATE );
        sSockInited = sal_True;
    }
}

// -------------------------------------------------------------------
// CreateSession
// Creates a new neon session.
// -------------------------------------------------------------------
HttpSession * NeonSession::CreateSession( const char *  inHost,
                                          int           inPort )
{
    if ( inHost == NULL || inPort <= 0 )
        throw DAVException( DAVException::DAV_INVALID_ARG );

    HttpSession * theHttpSession;
    if ( ( theHttpSession = http_session_create() ) == NULL )
        throw DAVException( DAVException::DAV_SESSION_CREATE );

    if ( http_session_server( theHttpSession, inHost, inPort ) != HTTP_OK )
    {
        http_session_destroy( theHttpSession );
        throw DAVException( DAVException::DAV_HTTP_LOOKUP );
    }

    return theHttpSession;
}

// -------------------------------------------------------------------
// GETReader
// A simple Neon http_block_reader for use with a http GET method
// in conjunction with an XInputStream
// -------------------------------------------------------------------
void NeonSession::GETReader( void *         inUserData,
                             const char *   inBuf,
                             size_t         inLen )
{
    NeonInputStream * theInputStream = ( NeonInputStream *) inUserData;
    theInputStream->AddToStream( inBuf, inLen );
}

// -------------------------------------------------------------------
// GETWriter
// A simple Neon http_block_reader for use with a http GET method
// in conjunction with an XOutputStream
// -------------------------------------------------------------------
void NeonSession::GETWriter( void *         inUserData,
                             const char *   inBuf,
                             size_t         inLen )
{
    Reference< XOutputStream > * theOutputStreamPtr =
            ( Reference< XOutputStream > *) inUserData;
    Reference< XOutputStream > theOutputStream = *theOutputStreamPtr;

    const Sequence< sal_Int8 > theSequence( ( sal_Int8 *) inBuf, inLen );
    theOutputStream->writeBytes( theSequence );
}

// Note: Uncomment the following if locking support is required
/*
void NeonSession::Lockit( const Lock & inLock, bool inLockit )
{
    osl::Guard< osl::Mutex > theGuard( mMutex );

    // Create the neon lock
    NeonLock * theLock = new NeonLock;
    int theRetVal;

    // Set the lock uri
    NeonUri theUri( inLock.uri );
    theLock->uri = const_cast< char * >
        ( OUStringToOString( theUri.GetPath(), RTL_TEXTENCODING_UTF8 ).getStr() );

    if ( inLockit )
    {
        // Set the lock depth
        switch( inLock.depth )
        {
            case ZERO:
            case INFINITY:
                theLock->depth = int ( inLock.depth );
                break;
            default:
                throw DAVException( DAVException::DAV_INVALID_ARG );
                break;
        }

        // Set the lock scope
        switch ( inLock.scope )
        {
            case EXCLUSIVE:
                theLock->scope = dav_lockscope_exclusive;
                break;
            case SHARED:
                theLock->scope = dav_lockscope_shared;
                break;
            default:
                throw DAVException( DAVException::DAV_INVALID_ARG );
                break;
        }

        // Set the lock owner
        const char * theOwner =
                    OUStringToOString( inLock.owner, RTL_TEXTENCODING_UTF8 );
        theLock->owner = const_cast< char * > ( theOwner );

        // Set the lock timeout
        // Note: Neon ignores the timeout
        //theLock->timeout = inLock.timeout;

        theRetVal = dav_lock( mHttpSession, theLock );
    }
    else
    {

        // Set the lock token
        OUString theToken = inLock.locktoken.getConstArray()[ 0 ];
        theLock->token = const_cast< char * >
            ( OUStringToOString( theToken, RTL_TEXTENCODING_UTF8 ).getStr() );

        theRetVal = dav_unlock( mHttpSession, theLock );
    }

    if ( theRetVal != HTTP_OK )
        throw DAVException( DAVException::DAV_HTTP_ERROR );
}
*/

int NeonSession::NeonAuth( void *       inUserData,
                           const char * inRealm,
                           const char * inHostName,
                           char **      inUserName,
                           char **      inPassWord )
{
    NeonSession * theSession = ( NeonSession * )inUserData;
    OUStringBuffer theUserNameBuffer;
    OUStringBuffer thePassWordBuffer;

    int theRetVal = theSession->mListener->authenticate(
                            OUString::createFromAscii( inRealm ),
                            OUString::createFromAscii( inHostName ),
                            theUserNameBuffer,
                            thePassWordBuffer,
                            theSession->mEnv );

    OUString theUserName = theUserNameBuffer.makeStringAndClear();
    OString *theStr = new OString( OUStringToOString( theUserName,
                                                      RTL_TEXTENCODING_UTF8 ) );
    inUserName[0] = const_cast< char *> ( theStr->getStr() );

    OUString thePassWord = thePassWordBuffer.makeStringAndClear();
    theStr = new OString( OUStringToOString( thePassWord,
                                             RTL_TEXTENCODING_UTF8 ) );
    inPassWord[0] = const_cast< char *> ( theStr->getStr() );

    return theRetVal;
}
