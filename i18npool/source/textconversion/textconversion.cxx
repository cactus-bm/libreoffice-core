/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: textconversion.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: kz $ $Date: 2006-01-31 18:48:05 $
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

#include <assert.h>
#include <textconversion.hxx>

using namespace com::sun::star::uno;
using namespace rtl;

namespace com { namespace sun { namespace star { namespace i18n {

TextConversion::TextConversion()
{
#ifdef SAL_DLLPREFIX
    OUString lib=OUString::createFromAscii(SAL_DLLPREFIX"textconv_dict"SAL_DLLEXTENSION);
#else
    OUString lib=OUString::createFromAscii("textconv_dict"SAL_DLLEXTENSION);
#endif
    hModule = osl_loadModule( lib.pData, SAL_LOADMODULE_DEFAULT );
}

TextConversion::~TextConversion()
{
    if (hModule) osl_unloadModule(hModule);
}

static void* nullFunc()
{
    return NULL;
}

void* SAL_CALL
TextConversion::getFunctionBySymbol(const sal_Char* func)
{
    if (hModule)
        return osl_getSymbol(hModule, OUString::createFromAscii(func).pData);
    else
        return (void*) nullFunc;
}

OUString SAL_CALL
TextConversion::getImplementationName() throw( RuntimeException )
{
    return OUString::createFromAscii(implementationName);
}

sal_Bool SAL_CALL
TextConversion::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return rServiceName.equalsAscii(implementationName);
}

Sequence< OUString > SAL_CALL
TextConversion::getSupportedServiceNames() throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    aRet[0] = OUString::createFromAscii(implementationName);
    return aRet;
}

} } } }
