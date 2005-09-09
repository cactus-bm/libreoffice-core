/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: DAVException.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 16:05:59 $
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

#ifndef _DAVEXCEPTION_HXX_
#define _DAVEXCEPTION_HXX_

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

namespace webdav_ucp
{

/////////////////////////////////////////////////////////////////////////////
// HTTP/WebDAV status codes
/////////////////////////////////////////////////////////////////////////////

const sal_uInt16 SC_NONE                             = 0;

// 1xx (Informational - no errors)
const sal_uInt16 SC_CONTINUE                         = 100;
const sal_uInt16 SC_SWITCHING_PROTOCOLS              = 101;
// DAV extensions
const sal_uInt16 SC_PROCESSING                       = 102;

//2xx (Successful - no errors)
const sal_uInt16 SC_OK                               = 200;
const sal_uInt16 SC_CREATED                          = 201;
const sal_uInt16 SC_ACCEPTED                         = 202;
const sal_uInt16 SC_NON_AUTHORITATIVE_INFORMATION    = 203;
const sal_uInt16 SC_NO_CONTENT                       = 204;
const sal_uInt16 SC_RESET_CONTENT                    = 205;
const sal_uInt16 SC_PARTIAL_CONTENT                  = 206;
// DAV extensions
const sal_uInt16 SC_MULTISTATUS                      = 207;

//3xx (Redirection)
const sal_uInt16 SC_MULTIPLE_CHOICES                 = 300;
const sal_uInt16 SC_MOVED_PERMANENTLY                = 301;
const sal_uInt16 SC_MOVED_TEMPORARILY                = 302;
const sal_uInt16 SC_SEE_OTHER                        = 303;
const sal_uInt16 SC_NOT_MODIFIED                     = 304;
const sal_uInt16 SC_USE_PROXY                        = 305;
const sal_uInt16 SC_TEMPORARY_REDIRECT               = 307;

//4xx (Client error)
const sal_uInt16 SC_BAD_REQUEST                      = 400;
const sal_uInt16 SC_UNAUTHORIZED                     = 401;
const sal_uInt16 SC_PAYMENT_REQUIRED                 = 402;
const sal_uInt16 SC_FORBIDDEN                        = 403;
const sal_uInt16 SC_NOT_FOUND                        = 404;
const sal_uInt16 SC_METHOD_NOT_ALLOWED               = 405;
const sal_uInt16 SC_NOT_ACCEPTABLE                   = 406;
const sal_uInt16 SC_PROXY_AUTHENTICATION_REQUIRED    = 407;
const sal_uInt16 SC_REQUEST_TIMEOUT                  = 408;
const sal_uInt16 SC_CONFLICT                         = 409;
const sal_uInt16 SC_GONE                             = 410;
const sal_uInt16 SC_LENGTH_REQUIRED                  = 411;
const sal_uInt16 SC_PRECONDITION_FAILED              = 412;
const sal_uInt16 SC_REQUEST_ENTITY_TOO_LARGE         = 413;
const sal_uInt16 SC_REQUEST_URI_TOO_LONG             = 414;
const sal_uInt16 SC_UNSUPPORTED_MEDIA_TYPE           = 415;
const sal_uInt16 SC_REQUESTED_RANGE_NOT_SATISFIABLE  = 416;
const sal_uInt16 SC_EXPECTATION_FAILED               = 417;
// DAV extensions
const sal_uInt16 SC_UNPROCESSABLE_ENTITY             = 422;
const sal_uInt16 SC_LOCKED                           = 423;
const sal_uInt16 SC_FAILED_DEPENDENCY                = 424;

//5xx (Server error)
const sal_uInt16 SC_INTERNAL_SERVER_ERROR            = 500;
const sal_uInt16 SC_NOT_IMPLEMENTED                  = 501;
const sal_uInt16 SC_BAD_GATEWAY                      = 502;
const sal_uInt16 SC_SERVICE_UNAVAILABLE              = 503;
const sal_uInt16 SC_GATEWAY_TIMEOUT                  = 504;
const sal_uInt16 SC_HTTP_VERSION_NOT_SUPPORTED       = 505;
// DAV extensions
const sal_uInt16 SC_INSUFFICIENT_STORAGE             = 507;

/////////////////////////////////////////////////////////////////////////////

class DAVException
{
    public:
        enum ExceptionCode {
            DAV_HTTP_ERROR = 0, // Generic error, mData = error message
            DAV_HTTP_LOOKUP,    // Name lookup failed, mData = server[:port]
            DAV_HTTP_AUTH,      // User authentication failed on server
            DAV_HTTP_AUTHPROXY, // User authentication failed on proxy
            DAV_HTTP_CONNECT,   // Could not connect to server, mData = server[:port]
            DAV_HTTP_TIMEOUT,   // Connection timed out
            DAV_HTTP_FAILED,    // The precondition failed
            DAV_HTTP_RETRY,     // Retry request
            DAV_HTTP_REDIRECT,  // See http_redirect.h, mData = new URL
            DAV_SESSION_CREATE, // session creation error, mData = server[:port]
            DAV_INVALID_ARG };  // mData = file URL

    private:
        ExceptionCode   mExceptionCode;
        rtl::OUString   mData;
        sal_uInt16      mStatusCode;

    public:
         DAVException( ExceptionCode inExceptionCode ) :
            mExceptionCode( inExceptionCode ), mStatusCode( SC_NONE ) {};
         DAVException( ExceptionCode inExceptionCode,
                       const rtl::OUString & rData ) :
            mExceptionCode( inExceptionCode ), mData( rData ),
            mStatusCode( SC_NONE ) {};
         DAVException( ExceptionCode inExceptionCode,
                       const rtl::OUString & rData,
                       sal_uInt16 nStatusCode ) :
            mExceptionCode( inExceptionCode ), mData( rData ),
            mStatusCode( nStatusCode ) {};
        ~DAVException( ) {};

    const ExceptionCode & getError() const { return mExceptionCode; }
    const rtl::OUString & getData() const  { return mData; }
    sal_uInt16 getStatus() const { return mStatusCode; }
};

}; // namespace webdav_ucp

#endif // _DAVEXCEPTION_HXX_
