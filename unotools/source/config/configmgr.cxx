 /*************************************************************************
 *
 *  $RCSfile: configmgr.cxx,v $
 *
 *  $Revision: 1.29 $
 *
 *  last change: $Author: os $ $Date: 2002-09-20 12:51:39 $
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

#ifndef _UTL_CONFIGMGR_HXX_
#include "unotools/configmgr.hxx"
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include "unotools/configitem.hxx"
#endif
#ifndef UNOTOOLS_CONFIGPATHES_HXX_INCLUDED
#include "unotools/configpathes.hxx"
#endif
#ifndef _UNOTOOLS_PROCESSFACTORY_HXX_
#include <unotools/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XHIERARCHICALNAMEACCESS_HPP_
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#include <list>

//-----------------------------------------------------------------------------

using namespace utl;
using namespace rtl;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;

#define C2U(cChar) OUString::createFromAscii(cChar)
//-----------------------------------------------------------------------------
const char* cConfigBaseURL = "/org.openoffice.";
//const char* cConfigBaseURL = "/com.sun.star.";
const char* cAccessSrvc = "com.sun.star.configuration.ConfigurationUpdateAccess";

static ::rtl::OUString aBrandName;
static ::rtl::OUString aProductVersion;
static ::rtl::OUString aProductExtension;

//-----------------------------------------------------------------------------
struct ConfigItemListEntry_Impl
{
    ConfigItem*                 pConfigItem;

    ConfigItemListEntry_Impl(ConfigItem*    pItem ) :
        pConfigItem(pItem){}
};
typedef std::list<ConfigItemListEntry_Impl> ConfigItemList;
struct utl::ConfigMgr_Impl
{
    ConfigItemList                          aItemList;
};

/* -----------------------------28.08.00 15:35--------------------------------

 ---------------------------------------------------------------------------*/
ConfigManager::ConfigManager() :
    pMgrImpl(new utl::ConfigMgr_Impl)
{
    GetConfigurationProvider(); // attempt to create the provider early
}
/* -----------------------------17.11.00 13:51--------------------------------

 ---------------------------------------------------------------------------*/
ConfigManager::ConfigManager(Reference< XMultiServiceFactory > xConfigProv) :
    pMgrImpl(new utl::ConfigMgr_Impl),
    xConfigurationProvider(xConfigProv)
{
}
/* -----------------------------28.08.00 15:35--------------------------------

 ---------------------------------------------------------------------------*/
ConfigManager::~ConfigManager()
{
    //check list content -> should be empty!
    OSL_ENSURE(pMgrImpl->aItemList.empty(), "some ConfigItems are still alive");
    if(!pMgrImpl->aItemList.empty())
    {
        ConfigItemList::iterator aListIter;
        for(aListIter = pMgrImpl->aItemList.begin(); aListIter != pMgrImpl->aItemList.end(); ++aListIter)
        {
            ConfigItemListEntry_Impl& rEntry = *aListIter;
            rEntry.pConfigItem->ReleaseConfigMgr();
        }
        pMgrImpl->aItemList.erase(pMgrImpl->aItemList.begin(), pMgrImpl->aItemList.end());
    }
    delete pMgrImpl;

}
/* -----------------------------28.08.00 16:17--------------------------------

 ---------------------------------------------------------------------------*/
Reference< XMultiServiceFactory > ConfigManager::GetConfigurationProvider()
{
    if(!xConfigurationProvider.is())
    {
        Reference< XMultiServiceFactory > xMSF = ::utl::getProcessServiceFactory();
        if ( xMSF.is() )
        {
            try
            {
                xConfigurationProvider = Reference< XMultiServiceFactory >
                    (xMSF->createInstance(
                        C2U("com.sun.star.configuration.ConfigurationProvider")),
                     UNO_QUERY);
            }
#ifdef DBG_UTIL
    catch(Exception& rEx)
    {
        OString sMsg("CreateInstance with arguments exception: ");
        sMsg += OString(rEx.Message.getStr(),
                    rEx.Message.getLength(),
                     RTL_TEXTENCODING_ASCII_US);
        OSL_ENSURE(sal_False, sMsg.getStr());
    }
#else
    catch(Exception&){}
#endif
        }
    }
    return xConfigurationProvider;
}
/* -----------------------------12.12.00 17:19--------------------------------

 ---------------------------------------------------------------------------*/
Reference< XMultiServiceFactory > ConfigManager::GetLocalConfigurationProvider()
{
    return GetConfigurationProvider();
}
/* -----------------------------29.08.00 12:35--------------------------------

 ---------------------------------------------------------------------------*/
Reference< XHierarchicalNameAccess > ConfigManager::AddConfigItem(utl::ConfigItem& rCfgItem)
{
    RegisterConfigItem(rCfgItem);
    return AcquireTree(rCfgItem);
}
/* -----------------------------21.06.01 12:20--------------------------------

 ---------------------------------------------------------------------------*/
void    ConfigManager::RegisterConfigItem(utl::ConfigItem& rCfgItem)
{
    ConfigItemList::iterator aListIter = pMgrImpl->aItemList.begin();
#ifdef DBG_UTIL
    for(aListIter = pMgrImpl->aItemList.begin(); aListIter != pMgrImpl->aItemList.end(); ++aListIter)
    {
        ConfigItemListEntry_Impl& rEntry = *aListIter;
        if(rEntry.pConfigItem == &rCfgItem)
            OSL_ENSURE(sal_False, "RegisterConfigItem: already inserted!");
    }
#endif
    pMgrImpl->aItemList.insert(aListIter, ConfigItemListEntry_Impl(&rCfgItem));
}
/* -----------------------------21.06.01 12:20--------------------------------

 ---------------------------------------------------------------------------*/
Reference< XHierarchicalNameAccess> ConfigManager::AcquireTree(utl::ConfigItem& rCfgItem)
{
    ConfigItemList::iterator aListIter = pMgrImpl->aItemList.begin();
#ifdef DBG_UTIL
    sal_Bool bFound = sal_False;
    for(aListIter = pMgrImpl->aItemList.begin(); aListIter != pMgrImpl->aItemList.end(); ++aListIter)
    {
        ConfigItemListEntry_Impl& rEntry = *aListIter;
        if(rEntry.pConfigItem == &rCfgItem)
        {
            bFound = sal_True;
            break;
        }
    }
    OSL_ENSURE(bFound, "AcquireTree: ConfigItem unknown!");
#endif
    OUString sPath = C2U(cConfigBaseURL);
    sPath += rCfgItem.GetSubTreeName();
    Sequence< Any > aArgs(2);
    Any* pArgs = aArgs.getArray();
    PropertyValue aPath;
    aPath.Name = C2U("nodepath");
    aPath.Value <<= sPath;
    pArgs[0] <<= aPath;
    sal_Bool bLazy = 0 != (rCfgItem.GetMode()&CONFIG_MODE_DELAYED_UPDATE);
    PropertyValue aUpdate;
    aUpdate.Name = C2U("lazywrite");
    aUpdate.Value.setValue(&bLazy, ::getBooleanCppuType());
    pArgs[1] <<= aUpdate;

    // Initialize item with support for reading/writing more then one locales at same time!
    // It's neccessary for creation of a complete configuration entry without changing office locale
    // at runtime.
    if(rCfgItem.GetMode()&CONFIG_MODE_ALL_LOCALES)
    {
        aArgs.realloc(4);
        pArgs = aArgs.getArray();
        PropertyValue aProperty;

        aProperty.Name  =   C2U("lazywrite");
        aProperty.Value <<= sal_True        ;
        pArgs[2] <<= aProperty;

        aProperty.Name  =   C2U("locale");
        aProperty.Value <<= C2U("*"     );
        pArgs[3] <<= aProperty;
    }

    Reference< XMultiServiceFactory > xCfgProvider = GetConfigurationProvider();
    Reference< XInterface > xIFace;
    if(xCfgProvider.is())
    {
        try
        {
            xIFace = xCfgProvider->createInstanceWithArguments(
                    C2U(cAccessSrvc),
                    aArgs);
        }
#ifdef DEBUG
        catch(Exception& rEx)
        {
            OString sMsg("CreateInstance exception: ");
            sMsg += OString(rEx.Message.getStr(),
                        rEx.Message.getLength(),
                         RTL_TEXTENCODING_ASCII_US);
            OSL_ENSURE(sal_False, sMsg.getStr());
        }
#else
        catch(Exception&){}
#endif
    }
    return Reference<XHierarchicalNameAccess>(xIFace, UNO_QUERY);
}
/* -----------------------------29.08.00 12:35--------------------------------

 ---------------------------------------------------------------------------*/
void ConfigManager::RemoveConfigItem(utl::ConfigItem& rCfgItem)
{
    if( !pMgrImpl->aItemList.empty() )
    {
        ConfigItemList::iterator aListIter = pMgrImpl->aItemList.begin();
        for(aListIter = pMgrImpl->aItemList.begin(); aListIter != pMgrImpl->aItemList.end(); ++aListIter)
        {
            ConfigItemListEntry_Impl& rEntry = *aListIter;
            if(rEntry.pConfigItem == &rCfgItem)
            {
                pMgrImpl->aItemList.erase(aListIter);
                break;
            }
        }
    }
}
/* -----------------------------30.08.00 15:04--------------------------------

 ---------------------------------------------------------------------------*/
void ConfigManager::StoreConfigItems()
{
    if(!pMgrImpl->aItemList.empty())
    {
        ConfigItemList::iterator aListIter = pMgrImpl->aItemList.begin();
        for(aListIter = pMgrImpl->aItemList.begin(); aListIter != pMgrImpl->aItemList.end(); ++aListIter)
        {
            ConfigItemListEntry_Impl& rEntry = *aListIter;
            if(rEntry.pConfigItem->IsModified())
            {
                rEntry.pConfigItem->Commit();
                rEntry.pConfigItem->ClearModified();
            }
        }
    }
}
ConfigManager*   ConfigManager::pConfigManager = 0;
/* -----------------------------07.09.00 11:06--------------------------------

 ---------------------------------------------------------------------------*/
ConfigManager*  ConfigManager::GetConfigManager()
{
    if(!pConfigManager)
    {
        pConfigManager = new ConfigManager();
    }
    return pConfigManager;
}
/* -----------------------------07.09.00 11:06--------------------------------

 ---------------------------------------------------------------------------*/
void    ConfigManager::RemoveConfigManager()
{
    if(pConfigManager)
    {
        delete pConfigManager;
        pConfigManager = 0;
    }
}
/* -----------------------------08.09.00 13:22--------------------------------

 ---------------------------------------------------------------------------*/
rtl::OUString ConfigManager::GetConfigBaseURL()
{
    return C2U(cConfigBaseURL);
}
/* -----------------------------25.09.00 16:34--------------------------------

 ---------------------------------------------------------------------------*/
Any ConfigManager::GetDirectConfigProperty(ConfigProperty eProp)
{
    ConfigManager * pTheConfigManager = GetConfigManager();

    Any aRet;
    if ( eProp == PRODUCTNAME && aBrandName.getLength() )
    {
        aRet <<= aBrandName;
        return aRet;
    }

    if ( eProp == PRODUCTVERSION && aProductVersion.getLength() )
    {
        aRet <<= aProductVersion;
        return aRet;
    }

    if ( eProp == PRODUCTEXTENSION && aProductExtension.getLength() )
    {
        aRet <<= aProductExtension;
        return aRet;
    }

    OUString sPath = C2U(cConfigBaseURL);
    switch(eProp)
    {
        case INSTALLPATH:
        case USERINSTALLURL:    sPath += C2U("Setup/Office"); break;
        case LOCALE:            sPath += C2U("Setup/L10N"); break;
        case PRODUCTNAME:
        case PRODUCTVERSION:
        case PRODUCTEXTENSION:  sPath += C2U("Setup/Product"); break;
        case OFFICEINSTALL:
        case OFFICEINSTALLURL:  sPath += C2U("Office.Common/Path/Current"); break;
        case DEFAULTCURRENCY:   sPath += C2U("Setup/L10N"); break;
    }
    Sequence< Any > aArgs(1);
    aArgs[0] <<= sPath;
    Reference< XMultiServiceFactory > xCfgProvider = GetConfigManager()->GetConfigurationProvider();
    if(!xCfgProvider.is())
        return aRet;
    Reference< XInterface > xIFace;
    try
    {
        xIFace = xCfgProvider->createInstanceWithArguments(
                C2U(cAccessSrvc),
                aArgs);

    }
    catch(Exception&){}
    Reference<XNameAccess> xDirectAccess(xIFace, UNO_QUERY);
    if(xDirectAccess.is())
    {
        OUString sProperty;
        switch(eProp)
        {
            case USERINSTALLURL:    sProperty = C2U("ooSetupInstallURL"); break;
            case INSTALLPATH:       sProperty = C2U("ooSetupInstallPath"); break;
            case LOCALE:            sProperty = C2U("ooLocale"); break;
            case PRODUCTNAME:       sProperty = C2U("ooName"); break;
            case PRODUCTVERSION:    sProperty = C2U("ooSetupVersion"); break;
            case PRODUCTEXTENSION:  sProperty = C2U("ooSetupExtension"); break;
            case OFFICEINSTALL:     sProperty = C2U("OfficeInstall"); break;
            case OFFICEINSTALLURL:  sProperty = C2U("OfficeInstallURL"); break;
            case DEFAULTCURRENCY:   sProperty += C2U("ooSetupCurrency"); break;
        }
        try
        {
            aRet = xDirectAccess->getByName(sProperty);
        }
        catch(Exception&)
        {
            OSL_ENSURE( sal_False,
                (   ::rtl::OString( "ConfigManager::GetDirectConfigProperty: could not retrieve the property \"" )
                +=  ::rtl::OString( sProperty.getStr(), sProperty.getLength(), RTL_TEXTENCODING_ASCII_US )
                +=  ::rtl::OString( "\" under \"" )
                +=  ::rtl::OString( sPath.getStr(), sPath.getLength(), RTL_TEXTENCODING_ASCII_US )
                +=  ::rtl::OString( "\" (caught an exception)!" )
                ).getStr()
            );
        }
    }

    if ( eProp == PRODUCTNAME )
        aRet >>= aBrandName;

    if ( eProp == PRODUCTVERSION )
        aRet >>= aProductVersion;

    if ( eProp == PRODUCTEXTENSION )
        aRet >>= aProductExtension;

    return aRet;
}
/* -----------------------------12.12.00 17:22--------------------------------

 ---------------------------------------------------------------------------*/
Reference< XHierarchicalNameAccess> ConfigManager::GetHierarchyAccess(const OUString& rFullPath)
{
    Sequence< Any > aArgs(1);
    aArgs[0] <<= rFullPath;
    Reference< XMultiServiceFactory > xCfgProvider = GetLocalConfigurationProvider();
    Reference< XInterface > xIFace;
    if(xCfgProvider.is())
    {
        try
        {
            xIFace = xCfgProvider->createInstanceWithArguments(
                    C2U(cAccessSrvc),
                    aArgs);
        }
#ifdef DBG_UTIL
        catch(Exception& rEx)
        {
            OString sMsg("CreateInstance exception: ");
            sMsg += OString(rEx.Message.getStr(),
                        rEx.Message.getLength(),
                         RTL_TEXTENCODING_ASCII_US);
            OSL_ENSURE(sal_False, sMsg.getStr());
        }
#else
        catch(Exception&){}
#endif
    }
    return Reference<XHierarchicalNameAccess>(xIFace, UNO_QUERY);
}
/* -----------------------------12.12.00 17:17--------------------------------

 ---------------------------------------------------------------------------*/
Any ConfigManager::GetLocalProperty(const OUString& rProperty)
{
    OUString sPath = C2U(cConfigBaseURL);
    sPath += rProperty;

    OUString sNode, sProperty;
    OSL_VERIFY( splitLastFromConfigurationPath(sPath, sNode, sProperty) );

    Reference< XNameAccess> xAccess( GetHierarchyAccess(sNode), UNO_QUERY );
    Any aRet;
    try
    {
        if(xAccess.is())
            aRet = xAccess->getByName(sProperty);
    }
#ifdef DBG_UTIL
    catch(Exception& rEx)
    {
        OString sMsg("GetLocalProperty: ");
        sMsg += OString(rEx.Message.getStr(),
                    rEx.Message.getLength(),
                     RTL_TEXTENCODING_ASCII_US);
        OSL_ENSURE(sal_False, sMsg.getStr());
    }
#else
    catch(Exception&){}
#endif
    return aRet;
}
/* -----------------------------12.12.00 17:17--------------------------------

 ---------------------------------------------------------------------------*/
void ConfigManager::PutLocalProperty(const OUString& rProperty, const Any& rValue)
{
    OUString sPath = C2U(cConfigBaseURL);
    sPath += rProperty;

    OUString sNode, sProperty;
    OSL_VERIFY( splitLastFromConfigurationPath(sPath, sNode, sProperty) );

    Reference<XNameReplace> xNodeReplace(GetHierarchyAccess(sNode), UNO_QUERY);
    if(xNodeReplace.is())
    {
        try
        {
            xNodeReplace->replaceByName(sProperty, rValue);
        }
#ifdef DBG_UTIL
        catch(Exception& rEx)
        {
            OString sMsg("PutLocalProperty: ");
            sMsg += OString(rEx.Message.getStr(),
                        rEx.Message.getLength(),
                         RTL_TEXTENCODING_ASCII_US);
            OSL_ENSURE(sal_False, sMsg.getStr());
        }
#else
        catch(Exception& ){}
#endif
    }
}
/* -----------------------------13.12.00 08:47--------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool    ConfigManager::IsLocalConfigProvider()
{
    return false;
}

