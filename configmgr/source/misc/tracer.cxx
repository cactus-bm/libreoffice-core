/*************************************************************************
 *
 *  $RCSfile: tracer.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: fs $ $Date: 2000-11-29 12:45:31 $
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

// SUNPRO5 does not like the following to be done after including stdio.h, that's why it's here at the very
// beginning of the file
#undef _TIME_T_DEFINED
#include <time.h>
#include <rtl/string.hxx>
#include <map>

namespace configmgr
{
    typedef ::std::map< ::rtl::OString, void*, ::std::less< ::rtl::OString > > VirtualDevices;
}

#ifndef _CONFIGMGR_TRACER_HXX_
#include "tracer.hxx"
#endif

#ifdef CFG_ENABLE_TRACING

#include <cstdarg>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <osl/process.h>
#include <rtl/ustring.hxx>

#define INFO    1
#define WARNING 2
#define ERROR   4

namespace configmgr
{

struct OTracerSetup
{
    sal_Int32       s_nTraceMask;
    sal_Int32       s_nIndentDepth;
    FILE*           s_pOutputMedium;
    sal_Bool        s_bInitialized;

    VirtualDevices  s_aDevices;

    OTracerSetup()
        :s_nTraceMask(INFO | WARNING | ERROR)
        ,s_nIndentDepth(0)
        ,s_pOutputMedium(NULL)
        ,s_bInitialized(sal_False)
    {
    }
};

//==========================================================================
//= OConfigTracer
//==========================================================================
::osl::Mutex    OConfigTracer::s_aMutex;
OTracerSetup*   OConfigTracer::s_pImpl = NULL;

//--------------------------------------------------------------------------
void OConfigTracer::ensureData()
{
    if (s_pImpl)
        return;
    s_pImpl = new OTracerSetup;
}

//--------------------------------------------------------------------------
void OConfigTracer::inc()
{
    ::osl::MutexGuard aGuard(s_aMutex);
    ensureData();
    ++s_pImpl->s_nIndentDepth;
}

//--------------------------------------------------------------------------
void OConfigTracer::dec()
{
    ::osl::MutexGuard aGuard(s_aMutex);
    ensureData();
    --s_pImpl->s_nIndentDepth;
}

//--------------------------------------------------------------------------
void OConfigTracer::traceInfo(const sal_Char* _pFormat, ...)
{
    ::osl::MutexGuard aGuard(s_aMutex);
    ensureData();
    if (s_pImpl->s_nTraceMask & INFO)
    {
        va_list args;
        va_start(args, _pFormat);
        implTrace("info    : ", _pFormat, args);
        va_end(args);
    }
}

//--------------------------------------------------------------------------
void OConfigTracer::traceWarning(const sal_Char* _pFormat, ...)
{
    ::osl::MutexGuard aGuard(s_aMutex);
    ensureData();
    if (s_pImpl->s_nTraceMask & WARNING)
    {
        va_list args;
        va_start(args, _pFormat);
        implTrace("warning : ", _pFormat, args);
        va_end(args);
    }
}

//--------------------------------------------------------------------------
void OConfigTracer::traceError(const sal_Char* _pFormat, ...)
{
    ::osl::MutexGuard aGuard(s_aMutex);
    ensureData();
    if (s_pImpl->s_nTraceMask & ERROR)
    {
        va_list args;
        va_start(args, _pFormat);
        implTrace("error   : ", _pFormat, args);
        va_end(args);
    }
}

//--------------------------------------------------------------------------
void OConfigTracer::indent()
{
    for (sal_Int32 i=0; i<s_pImpl->s_nIndentDepth; ++i)
        fprintf(s_pImpl->s_pOutputMedium, " ");
}

//--------------------------------------------------------------------------
FILE* disambiguate(const ::rtl::OString& _rFileName)
{
    FILE* pExistenceCheck = NULL;
    sal_Int32 i = 1;
    ::rtl::OString sLoop;
    while (i <= 256)
    {
        sLoop = _rFileName;
        sLoop += ".";
        sLoop += ::rtl::OString::valueOf(i);

        pExistenceCheck = fopen(sLoop.getStr(), "r");
        if (!pExistenceCheck)
            // does not exist
            return fopen(sLoop.getStr(), "w+");

        // already exists, try the next name
        fclose(pExistenceCheck);
        ++i;
    }

    // could not open such a file
    return NULL;
}

//--------------------------------------------------------------------------
void OConfigTracer::ensureInitalized()
{
    if (s_pImpl->s_bInitialized)
        return;

    s_pImpl->s_bInitialized = sal_True;

    char* pSettings = getenv("ENVCFGFLAGS");
    if (!pSettings)
        return;

    /*  currently recognized structure :
        + switches have to be separated by whitespaces
        + valid switches are:
            -m[e|o|f<file>] -   output to stderr (e), stdout (o) or a file (f). In the latter case the whole rest
                                of the param ('til the next one, means 'til the next whitespace) is the filename
            -t{i,w,e}*      -   type of output : i includes infos, w includes warnings, e includes errors
    */

    s_pImpl->s_pOutputMedium = stderr;
    s_pImpl->s_nTraceMask = 0;

    char* pParamLoop = pSettings;
    while (*pParamLoop)
    {
        while (!isspace(*pParamLoop) && *pParamLoop)
            ++pParamLoop;

        sal_Int32 nLen = pParamLoop - pSettings;
        if ((nLen > 1) && (*pSettings == '-'))
        {
            ++pSettings;
            switch (*pSettings)
            {
                case 'm':
                case 'w':
                    if (nLen > 2)
                    {
                        ++pSettings;
                        switch (*pSettings)
                        {
                            case 'e':
                                s_pImpl->s_pOutputMedium = stderr;
                                break;
                            case 'o':
                                s_pImpl->s_pOutputMedium = stdout;
                                break;
                            case 'f':
                            {
                                ++pSettings;
                                // copy the filename into an own buffer
                                ::rtl::OString sFileName(pSettings, pParamLoop - pSettings);

                                // open the file
                                s_pImpl->s_pOutputMedium = disambiguate(sFileName);

                                break;
                            }
                        }
                    }
                    break;
                case 'd':
                {   // assign a virtual device
                    // copy the device assingment description
                    ::rtl::OString sDescription(pSettings + 1, pParamLoop - pSettings - 1);
                    sal_Int32 nSep = sDescription.indexOf(':');
                    if (-1 == nSep)
                        break;  // invalid format

                    ::rtl::OString sVirtualDeviceName, sFileName;
                    sVirtualDeviceName = sDescription.copy(0, nSep);
                    sFileName = sDescription.copy(nSep + 1);

                    FILE* pVirtualDevice = disambiguate(sFileName);
                    if (pVirtualDevice)
                        s_pImpl->s_aDevices[sVirtualDeviceName] = pVirtualDevice;
                }
                case 't':
                {
                    ++pSettings;
                    while (pSettings != pParamLoop)
                    {
                        switch (*pSettings)
                        {
                            case 'i':   s_pImpl->s_nTraceMask |= INFO; break;
                            case 'w':   s_pImpl->s_nTraceMask |= WARNING; break;
                            case 'e':   s_pImpl->s_nTraceMask |= ERROR; break;
                        }
                        ++pSettings;
                    }
                }
            }
        }

        if (!*pParamLoop)
            break;

        ++pParamLoop;
        pSettings = pParamLoop;
    }

    // trace some initial information
    CFG_TRACE_INFO_NI("initialization: process id: 0x%08X", osl_getProcess(0));
    ::rtl::OUString sExecutable;
    osl_getExecutableFile(&sExecutable.pData);
    CFG_TRACE_INFO_NI("initialization: executable file name: %s", OUSTRING2ASCII(sExecutable));
}

//--------------------------------------------------------------------------
void OConfigTracer::traceToVirtualDevice(const sal_Char* _pDeviceName, const sal_Char* _pFormat, ...)
{
    ::osl::MutexGuard aGuard(s_aMutex);
    ensureData();
    ensureInitalized();

    VirtualDevices::const_iterator aDeviceMediumPos = s_pImpl->s_aDevices.find(::rtl::OString(_pDeviceName));
    if (aDeviceMediumPos != s_pImpl->s_aDevices.end())
    {
        FILE* pDeviceMedium = (FILE*)aDeviceMediumPos->second;

        va_list args;
        va_start(args, _pFormat);
        vfprintf(pDeviceMedium, _pFormat, args);
        fflush(pDeviceMedium);
        va_end(args);
    }
}

//--------------------------------------------------------------------------
::rtl::OString OConfigTracer::getTimeStamp()
{
    time_t aTime = time(NULL);
    tm* pStructuredTime = gmtime(&aTime);
    ::rtl::OString sTimeStamp(asctime(pStructuredTime));
    // cut the trainling linefeed (asctime is defined to contain such a line feed)
    sal_Int32 nStampLen = sTimeStamp.getLength();
    if ((0 != nStampLen) && ('\n' == sTimeStamp.getStr()[nStampLen - 1]))
        sTimeStamp = sTimeStamp.copy(0, nStampLen - 1);

    return sTimeStamp;
}

//--------------------------------------------------------------------------
void OConfigTracer::implTrace(const sal_Char* _pType, const sal_Char* _pFormat, va_list args)
{
    ensureInitalized();
    if (!s_pImpl->s_pOutputMedium)
        // no tracing enabled
        return;

    fprintf(s_pImpl->s_pOutputMedium, "%s", _pType);
    indent();
    vfprintf(s_pImpl->s_pOutputMedium, _pFormat, args);
    fprintf(s_pImpl->s_pOutputMedium,"\n");
    fflush(s_pImpl->s_pOutputMedium);
}

}   // namespace configmgr

#endif // defined(DEBUG) || defined(_DEBUG)

//**************************************************************************
// history:
//  $Log: not supported by cvs2svn $
//  Revision 1.2  2000/11/07 12:14:37  hr
//  #65293#: includes
//
//  Revision 1.1.1.1  2000/09/18 16:13:41  hr
//  initial import
//
//  Revision 1.7  2000/09/15 09:51:51  willem.vandorp
//  OpenOffice header added
//
//  Revision 1.6  2000/08/31 10:00:21  fs
//  time_t unknown
//
//  Revision 1.5  2000/08/30 14:34:09  fs
//  getTimeStamp
//
//  Revision 1.4  2000/08/20 12:55:42  fs
//  #77860# introduced an impl class; introduces virtual trace devices
//
//  Revision 1.3  2000/08/17 07:18:02  lla
//  im/export
//
//  Revision 1.2  2000/08/10 06:53:45  fs
//  read settings from the ENVCFGFLAGS environment variable
//
//  Revision 1.1  2000/08/09 18:52:46  fs
//  helper classes for tracing
//
//
//  Revision 1.0 09.08.00 13:10:05  fs
//**************************************************************************

