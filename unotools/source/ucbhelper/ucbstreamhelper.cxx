/*************************************************************************
 *
 *  $RCSfile: ucbstreamhelper.cxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: obo $ $Date: 2004-05-28 15:19:02 $
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

#include <unotools/ucblockbytes.hxx>
#include <unotools/ucbstreamhelper.hxx>
#include <comphelper/processfactory.hxx>

#ifndef _COM_SUN_STAR_UCB_COMMANDABORTEDEXCEPTION_HPP_
#include <com/sun/star/ucb/CommandAbortedException.hpp>
#endif

#ifndef _COM_SUN_STAR_UCB_XCOMMANDENVIRONMENT_HDL_
#include <com/sun/star/ucb/XCommandEnvironment.hdl>
#endif

#ifndef _COM_SUN_STAR_UCB_INSERTCOMMANDARGUMENT_HPP_
#include <com/sun/star/ucb/InsertCommandArgument.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XACTIVEDATASTREAMER_HPP_
#include <com/sun/star/io/XActiveDataStreamer.hpp>
#endif

#include <ucbhelper/contentbroker.hxx>
#include <ucbhelper/content.hxx>
#include <tools/debug.hxx>
#include <unotools/streamwrap.hxx>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::ucb;
using namespace ::com::sun::star::task;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;

namespace utl
{

static SvStream* lcl_CreateStream( const String& rFileName, StreamMode eOpenMode,
        Reference < XInteractionHandler > xInteractionHandler,
        UcbLockBytesHandler* pHandler, sal_Bool bForceSynchron, sal_Bool bEnsureFileExists )
{
    SvStream* pStream = NULL;
    ::ucb::ContentBroker* pBroker = ::ucb::ContentBroker::get();
    if ( pBroker )
    {
        UcbLockBytesRef xLockBytes;
        if ( eOpenMode & STREAM_WRITE )
        {
            sal_Bool bTruncate = ( eOpenMode & STREAM_TRUNC );
            if ( bTruncate )
            {
                try
                {
                    // truncate is implemented with deleting the original file
                    ::ucb::Content aCnt( rFileName, Reference < XCommandEnvironment >() );
                    aCnt.executeCommand( ::rtl::OUString::createFromAscii( "delete" ), makeAny( sal_Bool( sal_True ) ) );
                }

                catch ( CommandAbortedException& )
                {
                    // couldn't truncate/delete
                }
                catch ( ContentCreationException& )
                {
                }
                catch ( Exception& )
                {
                }
            }

            if ( bEnsureFileExists )
            {
                try
                {
                    // make sure that the desired file exists before trying to open
                    SvMemoryStream aStream(0,0);
                    ::utl::OInputStreamWrapper* pInput = new ::utl::OInputStreamWrapper( aStream );
                    Reference< XInputStream > xInput( pInput );

                    ::ucb::Content aContent( rFileName, Reference < XCommandEnvironment >() );
                    InsertCommandArgument aInsertArg;
                    aInsertArg.Data = xInput;

                    aInsertArg.ReplaceExisting = sal_False;
                    Any aCmdArg;
                    aCmdArg <<= aInsertArg;
                    aContent.executeCommand( ::rtl::OUString::createFromAscii( "insert" ), aCmdArg );
                }

                // it is NOT an error when the stream already exists and no truncation was desired
                catch ( CommandAbortedException& )
                {
                    // currently never an error is detected !
                }
                catch ( ContentCreationException& )
                {
                }
                catch ( Exception& )
                {
                }
            }
        }

        try
        {
            // create LockBytes using UCB
            ::ucb::Content aContent( rFileName, Reference < XCommandEnvironment >() );
            xLockBytes = UcbLockBytes::CreateLockBytes( aContent.get(), Sequence < PropertyValue >(),
                                                eOpenMode, xInteractionHandler, pHandler );
            if ( xLockBytes.Is() )
            {
                pStream = new SvStream( xLockBytes );
                pStream->SetBufferSize( 4096 );
                pStream->SetError( xLockBytes->GetError() );
            }
        }
        catch ( CommandAbortedException& )
        {
        }
        catch ( ContentCreationException& )
        {
        }
        catch ( Exception& )
        {
        }
    }
    else
        // if no UCB is present at least conventional file io is supported
        pStream = new SvFileStream( rFileName, eOpenMode );

    return pStream;
}

//============================================================================

SvStream* UcbStreamHelper::CreateStream( const String& rFileName, StreamMode eOpenMode,
        UcbLockBytesHandler* pHandler, sal_Bool bForceSynchron )
{
    return lcl_CreateStream( rFileName, eOpenMode, Reference < XInteractionHandler >(), pHandler, bForceSynchron, sal_True /* bEnsureFileExists */ );
}

SvStream* UcbStreamHelper::CreateStream( const String& rFileName, StreamMode eOpenMode,
        Reference < XInteractionHandler > xInteractionHandler,
        UcbLockBytesHandler* pHandler, sal_Bool bForceSynchron )
{
    return lcl_CreateStream( rFileName, eOpenMode, xInteractionHandler, pHandler, bForceSynchron, sal_True /* bEnsureFileExists */ );
}

SvStream* UcbStreamHelper::CreateStream( const String& rFileName, StreamMode eOpenMode,
        sal_Bool bFileExists,
        UcbLockBytesHandler* pHandler, sal_Bool bForceSynchron )
{
    return lcl_CreateStream( rFileName, eOpenMode, Reference < XInteractionHandler >(), pHandler, bForceSynchron, !bFileExists );
}

SvStream* UcbStreamHelper::CreateStream( Reference < XInputStream > xStream )
{
    SvStream* pStream = NULL;
    UcbLockBytesRef xLockBytes = UcbLockBytes::CreateInputLockBytes( xStream );
    if ( xLockBytes.Is() )
    {
        pStream = new SvStream( xLockBytes );
        pStream->SetBufferSize( 4096 );
        pStream->SetError( xLockBytes->GetError() );
    }

    return pStream;
};

SvStream* UcbStreamHelper::CreateStream( Reference < XStream > xStream )
{
    SvStream* pStream = NULL;
    if ( xStream->getOutputStream().is() )
    {
        UcbLockBytesRef xLockBytes = UcbLockBytes::CreateLockBytes( xStream );
        if ( xLockBytes.Is() )
        {
            pStream = new SvStream( xLockBytes );
            pStream->SetBufferSize( 4096 );
            pStream->SetError( xLockBytes->GetError() );
        }
    }
    else
        return CreateStream( xStream->getInputStream() );

    return pStream;
};

};
