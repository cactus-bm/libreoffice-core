/*************************************************************************
 *
 *  $RCSfile: LockSequence.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: kso $ $Date: 2001-05-16 14:58:06 $
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

#ifndef _LOCKSEQUENCE_HXX_
#include "LockSequence.hxx"
#endif

using namespace webdav_ucp;
using namespace com::sun::star;

//////////////////////////////////////////////////////////////////////////

#define DAV_ELM_LOCK_FIRST (HIP_ELM_UNUSED)

#define DAV_ELM_activelock  (DAV_ELM_LOCK_FIRST +  1)
#define DAV_ELM_lockscope   (DAV_ELM_LOCK_FIRST +  2)
#define DAV_ELM_locktype    (DAV_ELM_LOCK_FIRST +  3)
#define DAV_ELM_depth       (DAV_ELM_LOCK_FIRST +  4)
#define DAV_ELM_owner       (DAV_ELM_LOCK_FIRST +  5)
#define DAV_ELM_timeout     (DAV_ELM_LOCK_FIRST +  6)
#define DAV_ELM_locktoken   (DAV_ELM_LOCK_FIRST +  7)
#define DAV_ELM_exclusive   (DAV_ELM_LOCK_FIRST +  8)
#define DAV_ELM_shared      (DAV_ELM_LOCK_FIRST +  9)
#define DAV_ELM_write       (DAV_ELM_LOCK_FIRST + 10)
#define DAV_ELM_href        (DAV_ELM_LOCK_FIRST + 11)

// static
const struct hip_xml_elm LockSequence::elements[] =
{
    { "", "activelock", DAV_ELM_activelock, 0 },
    { "", "lockscope",  DAV_ELM_lockscope,  0 },
    { "", "locktype",   DAV_ELM_locktype,   0 },
    { "", "depth",      DAV_ELM_depth,      HIP_XML_CDATA },
    { "", "owner",      DAV_ELM_owner,      HIP_XML_COLLECT }, // ANY
    { "", "timeout",    DAV_ELM_timeout,    HIP_XML_CDATA },
    { "", "locktoken",  DAV_ELM_locktoken,  0 },
    { "", "exclusive",  DAV_ELM_exclusive,  0 }, // leaf
    { "", "shared",     DAV_ELM_shared,     0 }, // leaf
    { "", "write",      DAV_ELM_write,      0 }, // leaf
    { "", "href",       DAV_ELM_href,       HIP_XML_CDATA },
    { 0 }
};

struct LockSequenceParseContext
{
    ucb::Lock * pLock;

    LockSequenceParseContext() : pLock( 0 ) {}
    ~LockSequenceParseContext() { delete pLock; }
};

//////////////////////////////////////////////////////////////////////////
// static
bool LockSequence::createFromXML( const rtl::OString & rInData,
                                  uno::Sequence< ucb::Lock > & rOutData )
{
    const sal_Int32 TOKEN_LENGTH = 13; // </activelock>
    bool success = true;

    // rInData may contain multiple <activelock>...</activelock> tags.
    sal_Int32 nCount = 0;
    sal_Int32 nStart = 0;
    sal_Int32 nEnd   = rInData.indexOf( "</activelock>" );
    while ( nEnd > -1 )
    {
        hip_xml_parser * parser = hip_xml_create();
        if ( !parser )
        {
            success = false;
            break;
        }

        LockSequenceParseContext aCtx;
        hip_xml_push_handler( parser,
                                  elements,
                                  validate_callback,
                                  0, // startelement_callback
                                  endelement_callback,
                                  &aCtx );

        hip_xml_parse( parser,
                       rInData.getStr() + nStart,
                       nEnd - nStart + TOKEN_LENGTH );

        success = !!hip_xml_valid( parser );

        hip_xml_destroy( parser );

        if ( !success )
            break;

        if ( aCtx.pLock )
        {
            nCount++;
            if ( nCount > rOutData.getLength() )
                rOutData.realloc( rOutData.getLength() + 1 );

            rOutData[ nCount - 1 ] = *aCtx.pLock;
        }

        nStart = nEnd + TOKEN_LENGTH + 1;
        nEnd   = rInData.indexOf( "</activelock>", nStart );
    }

//  rOutData.realloc( nCount );
    return success;
}

//////////////////////////////////////////////////////////////////////////
// static
int LockSequence::validate_callback( hip_xml_elmid parent, hip_xml_elmid child )
{
    // @@@
    return HIP_XML_VALID;
}

//////////////////////////////////////////////////////////////////////////
// static
int LockSequence::endelement_callback( void * userdata,
                                        const struct hip_xml_elm * s,
                                        const char * cdata )
{
    LockSequenceParseContext * pCtx
                    = static_cast< LockSequenceParseContext * >( userdata );
    if ( !pCtx->pLock )
        pCtx->pLock = new ucb::Lock;

    switch ( s->id )
    {
        case DAV_ELM_depth:
            if ( rtl_str_equalsIgnoreCase( cdata, "0" ) )
                pCtx->pLock->Depth = ucb::LockDepth_ZERO;
            else if ( rtl_str_equalsIgnoreCase( cdata, "1" ) )
                pCtx->pLock->Depth = ucb::LockDepth_ONE;
            else if ( rtl_str_equalsIgnoreCase( cdata, "infinity" ) )
                pCtx->pLock->Depth = ucb::LockDepth_INFINITY;
            else
                OSL_ENSURE( sal_False,
                            "LockSequence::endelement_callback - "
                            "Unknown depth!" );
            break;

        case DAV_ELM_owner:
            pCtx->pLock->Owner <<= rtl::OUString::createFromAscii( cdata );
            break;

        case DAV_ELM_timeout:
            /*
                RFC2518, RFC2616:

                   TimeType = ("Second-" DAVTimeOutVal | "Infinite" | Other)
                   DAVTimeOutVal = 1*digit
                   Other = "Extend" field-value
                field-value = *( field-content | LWS )
                field-content = <the OCTETs making up the field-value
                                and consisting of either *TEXT or combinations
                                of token, separators, and quoted-string>
            */

            if ( rtl_str_equalsIgnoreCase( cdata, "Infinite" ) )
            {
                pCtx->pLock->Timeout = sal_Int64( -1 );
            }
            else if ( rtl_str_equalsIgnoreCase_WithLength(
                                            cdata, 7, "Second-", 7 ) )
            {
                pCtx->pLock->Timeout
                    = rtl::OUString::createFromAscii( cdata + 7 ).toInt64();
            }
//          else if ( rtl_str_equalsIgnoreCase_WithLength(
//                                          cdata, 6, "Extend", 6 ) )
//          {
//              ???
//          }
            else
            {
                pCtx->pLock->Timeout = sal_Int64( -1 );
                OSL_ENSURE( sal_False,
                            "LockSequence::endelement_callback - "
                            "Unknown timeout!" );
            }
            break;

        case DAV_ELM_exclusive:
            pCtx->pLock->Scope = ucb::LockScope_EXCLUSIVE;
            break;

        case DAV_ELM_shared:
            pCtx->pLock->Scope = ucb::LockScope_SHARED;
            break;

        case DAV_ELM_write:
            pCtx->pLock->Type = ucb::LockType_WRITE;
            break;

        case DAV_ELM_href:
        {
            sal_Int32 nPos = pCtx->pLock->LockTokens.getLength();
            pCtx->pLock->LockTokens.realloc( nPos + 1 );
            pCtx->pLock->LockTokens[ nPos ]
                    = rtl::OUString::createFromAscii( cdata );
            break;
        }

        default:
            break;
    }
    return 0;
}
