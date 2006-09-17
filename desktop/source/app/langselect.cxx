/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: langselect.cxx,v $
 *
 *  $Revision: 1.21 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 09:36:46 $
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
#include "precompiled_desktop.hxx"

#include "app.hxx"
#include "langselect.hxx"
#include <stdio.h>

#ifndef _RTL_STRING_HXX
#include <rtl/string.hxx>
#endif
#ifndef _SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef _TOOLS_RESID_HXX
#include <tools/resid.hxx>
#endif
#ifndef INCLUDED_I18NPOOL_MSLANGID_HXX
#include <i18npool/mslangid.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/util/XChangesBatch.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/lang/XLocalizable.hpp>
#include <com/sun/star/lang/Locale.hpp>
#include "com/sun/star/util/XFlushable.hpp"
#include <rtl/locale.hxx>
#ifndef INCLUDED_RTL_INSTANCE_HXX
#include <rtl/instance.hxx>
#endif
#include <osl/process.h>

using namespace rtl;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::container;
using namespace com::sun::star::beans;
using namespace com::sun::star::util;

namespace desktop {

sal_Bool LanguageSelection::bFoundLanguage = sal_False;
OUString LanguageSelection::aFoundLanguage;
const OUString LanguageSelection::usFallbackLanguage = OUString::createFromAscii("en-US");

Locale LanguageSelection::IsoStringToLocale(const OUString& str)
{
    Locale l;
    sal_Int32 index=0;
    l.Language = str.getToken(0, '-', index);
    if (index >= 0) l.Country = str.getToken(0, '-', index);
    if (index >= 0) l.Variant = str.getToken(0, '-', index);
    return l;
}

bool LanguageSelection::prepareLanguage()
{
    OUString sConfigSrvc = OUString::createFromAscii("com.sun.star.configuration.ConfigurationProvider");
    Reference< XMultiServiceFactory > theMSF = comphelper::getProcessServiceFactory();
    Reference< XLocalizable > theConfigProvider;
    try
    {
        theConfigProvider = Reference< XLocalizable >(theMSF->createInstance( sConfigSrvc ),UNO_QUERY_THROW );
    }
    catch(const Exception&)
    {
    }
    if(!theConfigProvider.is())
        return false;

    sal_Bool bSuccess = sal_False;

    // #i42730#get the windows 16Bit locale - it should be preferred over the UI language
    try
    {
        Reference< XPropertySet > xProp(getConfigAccess("org.openoffice.System/L10N/", sal_False), UNO_QUERY_THROW);
        Any aWin16SysLocale = xProp->getPropertyValue(OUString::createFromAscii("SystemLocale"));
        ::rtl::OUString sWin16SysLocale;
        aWin16SysLocale >>= sWin16SysLocale;
        if( sWin16SysLocale.getLength())
            setDefaultLocale(sWin16SysLocale);
    }
    catch(const Exception&)
    {
    }

    // #i32939# use system locale to set document default locale
    try
    {
        OUString usLocale;
        Reference< XPropertySet > xLocaleProp(getConfigAccess(
            "org.openoffice.System/L10N", sal_True), UNO_QUERY_THROW);
           // "org.openoffice.Office.Linguistic/General", sal_True), UNO_QUERY_THROW);
        xLocaleProp->getPropertyValue(OUString::createFromAscii("Locale")) >>= usLocale;
            setDefaultLocale(usLocale);
    }
    catch (Exception&)
    {
    }
    // get the selected UI language as string
    OUString aLocaleString = getLanguageString();
    if ( aLocaleString.getLength() > 0 )
    {
        try
        {
            // prepare default config provider by localizing it to the selected locale
            // this will ensure localized configuration settings to be selected accoring to the
            // UI language.
            Locale loc = LanguageSelection::IsoStringToLocale(aLocaleString);
            // flush any data already written to the configuration (which
            // currently uses independent caches for different locales and thus
            // would ignore data written to another cache):
            Reference< XFlushable >(theConfigProvider, UNO_QUERY_THROW)->
                flush();
            theConfigProvider->setLocale(loc);

            Reference< XPropertySet > xProp(getConfigAccess("org.openoffice.Setup/L10N/", sal_True), UNO_QUERY_THROW);
            xProp->setPropertyValue(OUString::createFromAscii("ooLocale"), makeAny(aLocaleString));
            Reference< XChangesBatch >(xProp, UNO_QUERY_THROW)->commitChanges();

            bSuccess = sal_True;
        }
        catch ( PropertyVetoException& )
        {
            // we are not allowed to change this
        }
        catch (Exception& e)
        {
            OString aMsg = OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US);
            OSL_ENSURE(sal_False, aMsg.getStr());

        }
    }

    // #i32939# setting of default document locale
    // #i32939# this should not be based on the UI language
    setDefaultLocale(aLocaleString);

    // fallback: set en-US as default Western locale if no Western locale is set
    OUString usWesternName = OUString::createFromAscii("DefaultLocale");
    try
    {
        Reference< XPropertySet > xProp(getConfigAccess(
            "org.openoffice.Office.Linguistic/General/", sal_True), UNO_QUERY_THROW);
        OUString usValue;
        xProp->getPropertyValue(usWesternName) >>= usValue;
        if (usValue.getLength() == 0)
        {
            // there is no western document language selected
            // if the selected locale is a western locale, this
            // will become the default western docuemnt language
            // otherwise, fall back to en-US
            OUString usDefault = OUString::createFromAscii("en-US");
            try
            {
                xProp->setPropertyValue(usWesternName, makeAny(usDefault));
                Reference< XChangesBatch >(xProp,UNO_QUERY_THROW)->commitChanges();
            }
            catch ( PropertyVetoException )
            {
                // we are not allowed to change this
            }
        }
    }
    catch ( Exception& )
    {
    }

    return bSuccess;
}

void LanguageSelection::setDefaultLocale(const OUString& usUILocale)
{
    // #i32939# setting of default document locale
    // org.openoffice.Office.Linguistic/General/DefaultLocale
    // org.openoffice.Office.Linguistic/General/DefaultLocale_CJK
    // org.openoffice.Office.Linguistic/General/DefaultLocale_CTL

    // determine script type of UI locale
    LanguageType ltUILocale = MsLangId::convertIsoStringToLanguage(usUILocale);
    sal_uInt16 nScriptType = SvtLanguageOptions::GetScriptTypeOfLanguage(ltUILocale);

    try
    {
        Reference< XPropertySet > xProp(getConfigAccess(
                                            "org.openoffice.Office.Linguistic/General/", sal_True), UNO_QUERY_THROW);
        OUString usName = OUString::createFromAscii("DefaultLocale");
        switch (nScriptType)
        {
            case SCRIPTTYPE_ASIAN:
                usName = OUString::createFromAscii("DefaultLocale_CJK");
                break;
            case SCRIPTTYPE_COMPLEX:
                usName = OUString::createFromAscii("DefaultLocale_CTL");
                break;
        }
        OUString usValue;
        xProp->getPropertyValue(usName) >>= usValue;
        if (usValue.getLength() == 0)
        {
            // there is no document language set, for the script type selected
            // in the UI
            // covert the LanguageType we've got from the LanguageTable back to
            // an iso string and store it
            OUString usDefault = MsLangId::convertLanguageToIsoString(ltUILocale);
            try
            {
                xProp->setPropertyValue(usName, makeAny(usDefault));
                Reference< XChangesBatch >(xProp, UNO_QUERY_THROW)->commitChanges();
            }
            catch ( PropertyVetoException )
            {
                // we are not allowed to change this
            }
        }
    }
    catch ( Exception& )
    {
    }
}

OUString LanguageSelection::getLanguageString()
{
    // did we already find a language?
    if (bFoundLanguage)
        return aFoundLanguage;
    // check whether the user has selected a specific language
    OUString aUserLanguage = getUserLanguage();
    if (aUserLanguage.getLength() > 0 )
    {
        if (isInstalledLanguage(aUserLanguage))
        {
            // all is well
            bFoundLanguage = sal_True;
            aFoundLanguage = aUserLanguage;
            return aFoundLanguage;
        }
        else
        {
            // selected language is not/no longer installed
            resetUserLanguage();
        }
    }
    // try to use system default
    aUserLanguage = getSystemLanguage();
    if (aUserLanguage.getLength() > 0 )
    {
        if (isInstalledLanguage(aUserLanguage, sal_False))
        {
            // great, system default language is available
            bFoundLanguage = sal_True;
            aFoundLanguage = aUserLanguage;
            return aFoundLanguage;
        }
    }
    // fallback 1: en-US
    OUString usFB = usFallbackLanguage;
    if (isInstalledLanguage(usFB))
    {
        bFoundLanguage = sal_True;
        aFoundLanguage = usFallbackLanguage;
        return aFoundLanguage;
    }
    // fallback didn't work use first installed language
    aUserLanguage = getFirstInstalledLanguage();
    bFoundLanguage = sal_True;
    aFoundLanguage = aUserLanguage;
    return aFoundLanguage;
}

Reference< XNameAccess > LanguageSelection::getConfigAccess(const sal_Char* pPath, sal_Bool bUpdate)
{
    Reference< XNameAccess > xNameAccess;
    try{
        OUString sConfigSrvc = OUString::createFromAscii("com.sun.star.configuration.ConfigurationProvider");
        OUString sAccessSrvc;
        if (bUpdate)
            sAccessSrvc = OUString::createFromAscii("com.sun.star.configuration.ConfigurationUpdateAccess");
        else
            sAccessSrvc = OUString::createFromAscii("com.sun.star.configuration.ConfigurationAccess");

        OUString sConfigURL = OUString::createFromAscii(pPath);

        // get configuration provider
        Reference< XMultiServiceFactory > theMSF = comphelper::getProcessServiceFactory();
        if (theMSF.is()) {
            Reference< XMultiServiceFactory > theConfigProvider = Reference< XMultiServiceFactory > (
                theMSF->createInstance( sConfigSrvc ),UNO_QUERY_THROW );

            // access the provider
            Sequence< Any > theArgs(1);
            theArgs[ 0 ] <<= sConfigURL;
            xNameAccess = Reference< XNameAccess > (
                theConfigProvider->createInstanceWithArguments(
                    sAccessSrvc, theArgs ), UNO_QUERY_THROW );
        }
    } catch (com::sun::star::uno::Exception& e)
    {
        OString aMsg = OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US);
        OSL_ENSURE(sal_False, aMsg.getStr());
    }
    return xNameAccess;
}

Sequence< OUString > LanguageSelection::getInstalledLanguages()
{
    Sequence< OUString > seqLanguages;
    Reference< XNameAccess > xAccess = getConfigAccess("org.openoffice.Setup/Office/InstalledLocales", sal_False);
    if (!xAccess.is()) return seqLanguages;
    seqLanguages = xAccess->getElementNames();
    return seqLanguages;
}

// FIXME
// it's not very clever to handle language fallbacks here, but
// right now, there is no place that handles those fallbacks globally
static Sequence< OUString > _getFallbackLocales(const OUString& aIsoLang)
{
    Sequence< OUString > seqFallbacks;
    if (aIsoLang.equalsAscii("zh-HK")) {
        seqFallbacks = Sequence< OUString >(1);
        seqFallbacks[0] = OUString::createFromAscii("zh-TW");
    }
    return seqFallbacks;
}

sal_Bool LanguageSelection::isInstalledLanguage(OUString& usLocale, sal_Bool bExact)
{
    sal_Bool bInstalled = sal_False;
    Sequence< OUString > seqLanguages = getInstalledLanguages();
    for (sal_Int32 i=0; i<seqLanguages.getLength(); i++)
    {
        if (usLocale.equals(seqLanguages[i]))
        {
            bInstalled = sal_True;
            break;
        }
    }

    if (!bInstalled && !bExact)
    {
        // try fallback locales
        Sequence< OUString > seqFallbacks = _getFallbackLocales(usLocale);
        for (sal_Int32 j=0; j<seqFallbacks.getLength(); j++)
        {
            for (sal_Int32 i=0; i<seqLanguages.getLength(); i++)
            {
                if (seqFallbacks[j].equals(seqLanguages[i]))
                {
                    bInstalled = sal_True;
                    usLocale = seqFallbacks[j];
                    break;
                }
            }
        }
    }

    if (!bInstalled && !bExact)
    {
        // no exact match was found, well try to find a substitute
        OUString aInstalledLocale;
        for (sal_Int32 i=0; i<seqLanguages.getLength(); i++)
        {
            if (usLocale.indexOf(seqLanguages[i]) == 0)
            {
                // requested locale starts with the installed locale
                // (i.e. installed locale has index 0 in requested locale)
                bInstalled = sal_True;
                usLocale = seqLanguages[i];
                break;
            }
        }
    }
    return bInstalled;
}

OUString LanguageSelection::getFirstInstalledLanguage()
{
    OUString aLanguage;
    Sequence< OUString > seqLanguages = getInstalledLanguages();
    if (seqLanguages.getLength() > 0)
        aLanguage = seqLanguages[0];
    return aLanguage;
}

OUString LanguageSelection::getUserLanguage()
{
    OUString aUserLanguage;
    Reference< XNameAccess > xAccess(getConfigAccess("org.openoffice.Office.Linguistic/General", sal_False));
    if (xAccess.is())
    {
        try
        {
            xAccess->getByName(OUString::createFromAscii("UILocale")) >>= aUserLanguage;
        }
        catch ( NoSuchElementException const & )
        {
            return OUString();
        }
        catch ( WrappedTargetException const & )
        {
            return OUString();
        }
    }
    return aUserLanguage;
}

OUString LanguageSelection::getSystemLanguage()
{
    OUString aUserLanguage;
    Reference< XNameAccess > xAccess(getConfigAccess("org.openoffice.System/L10N", sal_False));
    if (xAccess.is())
    {
        try
        {
            xAccess->getByName(OUString::createFromAscii("UILocale")) >>= aUserLanguage;
        }
        catch ( NoSuchElementException const & )
        {
            return OUString();
        }
        catch ( WrappedTargetException const & )
        {
            return OUString();
        }
    }
    return aUserLanguage;
}


void LanguageSelection::resetUserLanguage()
{
    try
    {
        Reference< XPropertySet > xProp(getConfigAccess("org.openoffice.Office.Linguistic/General", sal_True), UNO_QUERY_THROW);
        xProp->setPropertyValue(OUString::createFromAscii("UILocale"), makeAny(OUString::createFromAscii("")));
        Reference< XChangesBatch >(xProp, UNO_QUERY_THROW)->commitChanges();
    }
    catch ( PropertyVetoException& )
    {
        // we are not allowed to change this
    }
    catch ( Exception& e)
    {
        OString aMsg = OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US);
        OSL_ENSURE(sal_False, aMsg.getStr());
    }

}


} // namespace desktop
