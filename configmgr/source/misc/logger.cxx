/*************************************************************************
 *
 *  $RCSfile: logger.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2004-08-31 14:57:58 $
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
 *  Copyright: 2004 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#include "logger.hxx"

#define CONFIG_LOGGER_SINGLETON "/singletons/com.sun.star.configuration.theLogger"
#define OUSTR( lit ) OUString( RTL_CONSTASCII_USTRINGPARAM( lit ) )
#define OU2A( ustr ) rtl::OUStringToOString( ustr, RTL_TEXTENCODING_UTF8 ).getStr()
#define A2OU( astr ) OUString::createFromAscii( astr )

static const sal_Char k_unspecifiedClass[] = "configmgr";
static const sal_Char k_unspecifiedMethod[] = "log-message";

namespace configmgr
{

//--------------------------------------------------------------------------
void Logger::log(Level nLevel, const char * msg, const char * sourceMethod, const char * sourceClass) const
{
    OSL_ASSERT(msg);
    if (!msg) msg = "";

    this->log(nLevel,A2OU(msg),sourceMethod,sourceClass);
}

//--------------------------------------------------------------------------
void Logger::log(Level nLevel, const OUString & msg, const char * sourceMethod, const char * sourceClass) const
{
    if (!sourceClass) sourceClass = k_unspecifiedClass;
    if (!sourceMethod) sourceMethod = k_unspecifiedMethod;

    // this place can be used to further enrich or instrument log output
    if (m_xLogger.is())
        try
        {
            m_xLogger->logp(nLevel,A2OU(sourceClass),A2OU(sourceMethod),msg);
        }
        catch (uno::Exception & e)
        {
            OSL_TRACE("Configuration Log failure: %s\n"
                      "Log message was [Level=%04d] %s::%s : %s\n",
                      OU2A(e.Message),int(nLevel),sourceClass,sourceMethod,OU2A(msg));
        }
    else if (nLevel >= (LogLevel::SEVERE - OSL_DEBUG_LEVEL*(LogLevel::SEVERE-LogLevel::WARNING)))
        OSL_TRACE("Configuration Log [%04d] %s::%s : %s\n", int(nLevel),sourceClass,sourceMethod,OU2A(msg));

}

//--------------------------------------------------------------------------
uno::Reference< logging::XLogger >
    Logger::getUnoLoggerFromContext(uno::Reference< uno::XComponentContext > const & xContext)
{
    uno::Reference< logging::XLogger > xLogger;

    if (xContext.is())
        try { xContext->getValueByName( OUSTR(CONFIG_LOGGER_SINGLETON) ) >>= xLogger; }
        catch (uno::Exception & ) {}

    return xLogger;
}

//--------------------------------------------------------------------------

}   // namespace configmgr


