/*************************************************************************
 *
 *  $RCSfile: tracer.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: vg $ $Date: 2003-04-15 17:17:18 $
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

#ifndef _CONFIGMGR_TRACER_HXX_
#define _CONFIGMGR_TRACER_HXX_

#if OSL_DEBUG_LEVEL > 0
#define CFG_ENABLE_TRACING
#endif

#ifdef CFG_ENABLE_TRACING

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#include <rtl/string.hxx>
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#include <stdarg.h>
#include <stdio.h>

#ifdef WNT
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

#define OUSTRING2ASCII(rtlOUString) ::rtl::OString((rtlOUString).getStr(), (rtlOUString).getLength(), RTL_TEXTENCODING_ASCII_US).getStr()

#define CFG_TRACE_INFO          OTraceIndent aIndent; OConfigTracer::traceInfo
#define CFG_TRACE_WARNING       OTraceIndent aIndent; OConfigTracer::traceWarning
#define CFG_TRACE_ERROR         OTraceIndent aIndent; OConfigTracer::traceError
#define CFG_TRACE_INFO_NI       OConfigTracer::traceInfo
#define CFG_TRACE_WARNING_NI    OConfigTracer::traceWarning
#define CFG_TRACE_ERROR_NI      OConfigTracer::traceError


#define CFG_TRACE_TO_DEVICE     OConfigTracer::traceToVirtualDevice

namespace configmgr
{

struct OTracerSetup;
class OConfigTracer
{
    friend class OTraceIndent;

protected:
    static  ::osl::Mutex    s_aMutex;
    static OTracerSetup*    s_pImpl;
#ifdef WNT
    static timeb            s_aStartTime;
#else
    static timeval          s_aStartTime;
#endif

private:
    OConfigTracer();    // never implemented, no instantiation of this class allowed, only static members

public:
    static void traceInfo(const sal_Char* _pFormat, ...);
    static void traceWarning(const sal_Char* _pFormat, ...);
    static void traceError(const sal_Char* _pFormat, ...);
    static void traceToVirtualDevice(const sal_Char* _pDeviceName, const sal_Char* _pFormat, ...);

    static ::rtl::OString getTimeStamp();

protected:
    static void trace(const sal_Char* _pFormat, ...);
    static void implTrace(const sal_Char* _pType, const sal_Char* _pFormat, va_list args);
    static void startGlobalTimer();
    static sal_uInt32 getGlobalTimer();

    static void inc();
    static void dec();

    static void indent();

    static void ensureData();
    static void ensureInitalized();
};

class OTraceIndent
{
public:
    OTraceIndent() { OConfigTracer::inc(); }
    ~OTraceIndent() { OConfigTracer::dec(); }
};

}   // namespace configmgr

#else   // !CFG_ENABLE_TRACING

#include <stdio.h>

#define OUSTRING2ASCII(rtlOUString) "nothing"

#define CFG_TRACE_INFO          1 ? (0) : printf
#define CFG_TRACE_WARNING       1 ? (0) : printf
#define CFG_TRACE_ERROR         1 ? (0) : printf
#define CFG_TRACE_INFO_NI       1 ? (0) : printf
#define CFG_TRACE_WARNING_NI    1 ? (0) : printf
#define CFG_TRACE_ERROR_NI      1 ? (0) : printf
#define CFG_TRACE_TO_DEVICE     1 ? (0) : printf

#endif  // CFG_ENABLE_TRACING

#endif // _CONFIGMGR_TRACER_HXX_

