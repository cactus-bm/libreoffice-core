/*************************************************************************
 *
 *  $RCSfile: presethandler.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-16 14:52:37 $
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

#ifndef __FRAMEWORK_ACCELERATORS_PRESETHANDLER_HXX_
#include <accelerators/presethandler.hxx>
#endif

//_______________________________________________
// own includes

#ifndef __FRAMEWORK_THREADHELP_READGUARD_HXX_
#include <threadhelp/readguard.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_WRITEGUARD_HXX_
#include <threadhelp/writeguard.hxx>
#endif

#ifndef __FRAMEWORK_SERVICES_H_
#include <services.h>
#endif

//_______________________________________________
// interface includes

#ifndef __COM_SUN_STAR_CONTAINER_NOSUCHELEMENTEXCEPTION_HPP_
#include <com/sun/star/container/NoSuchElementException.hpp>
#endif

#ifndef __COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif

#ifndef __COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#ifndef __COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif

#ifndef __COM_SUN_STAR_EMBED_XTRANSACTEDOBJECT_HPP_
#include <com/sun/star/embed/XTransactedObject.hpp>
#endif

#ifndef __COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif

//_______________________________________________
// other includes

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

#ifndef _RTL_USTRBUF_HXX
#include <rtl/ustrbuf.hxx>
#endif

//_______________________________________________
// const

#define SUBSTORAGE_GLOBAL       DECLARE_ASCII("global" )
#define SUBSTORAGE_MODULES      DECLARE_ASCII("modules")

#define BASEPATH_SHARE_LAYER    DECLARE_ASCII("UIConfig"  )
#define BASEPATH_USER_LAYER     DECLARE_ASCII("UserConfig")

#define RELPATH_SHARE_LAYER     DECLARE_ASCII("soffice.cfg/uiconfig.zip")
#define RELPATH_USER_LAYER      DECLARE_ASCII("soffice.cfg/uiconfig.zip")

#define FILE_EXTENSION          DECLARE_ASCII(".xml")

#define PATH_SEPERATOR          DECLARE_ASCII("/")

//_______________________________________________
// namespace

namespace framework
{

//-----------------------------------------------
::rtl::OUString PresetHandler::PRESET_DEFAULT()
{
    static ::rtl::OUString RSTYPE = DECLARE_ASCII("default");
    return RSTYPE;
}

//-----------------------------------------------
::rtl::OUString PresetHandler::TARGET_CURRENT()
{
    static ::rtl::OUString RSTYPE = DECLARE_ASCII("current");
    return RSTYPE;
}

//-----------------------------------------------
::rtl::OUString PresetHandler::RESOURCETYPE_MENUBAR()
{
    static ::rtl::OUString RSTYPE = DECLARE_ASCII("menubar");
    return RSTYPE;
}

//-----------------------------------------------
::rtl::OUString PresetHandler::RESOURCETYPE_TOOLBAR()
{
    static ::rtl::OUString RSTYPE = DECLARE_ASCII("toolbar");
    return RSTYPE;
}

//-----------------------------------------------
::rtl::OUString PresetHandler::RESOURCETYPE_ACCELERATOR()
{
    static ::rtl::OUString RSTYPE = DECLARE_ASCII("accelerator");
    return RSTYPE;
}

//-----------------------------------------------
::rtl::OUString PresetHandler::RESOURCETYPE_STATUSBAR()
{
    static ::rtl::OUString RSTYPE = DECLARE_ASCII("statusbar");
    return RSTYPE;
}

//-----------------------------------------------
PresetHandler::PresetHandler(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR)
    : ThreadHelpBase     (&Application::GetSolarMutex()        )
    , m_xSMGR            (xSMGR                                )
    , m_aSharedStorages  (                                     )
    , m_aLocale          (::comphelper::Locale::X_NOTRANSLATE())
    , m_lDocumentStorages(xSMGR                                )
{
}

//-----------------------------------------------
PresetHandler::PresetHandler(const PresetHandler& rCopy)
{
    m_xSMGR                 = rCopy.m_xSMGR;
    m_eConfigType           = rCopy.m_eConfigType;
    m_sResourceType         = rCopy.m_sResourceType;
    m_sModule               = rCopy.m_sModule;
    m_aSharedStorages       = rCopy.m_aSharedStorages;
    m_xWorkingStorageShare  = rCopy.m_xWorkingStorageShare;
    m_xWorkingStorageUser   = rCopy.m_xWorkingStorageUser;
    m_lPresets              = rCopy.m_lPresets;
    m_lTargets              = rCopy.m_lTargets;
    m_aLocale               = rCopy.m_aLocale;
    m_lDocumentStorages     = rCopy.m_lDocumentStorages;
}

//-----------------------------------------------
PresetHandler::~PresetHandler()
{
    m_aSharedStorages->m_lStoragesUser.closePath(m_sRelPath);
    // in document case share and user are the same ...
    // so we would close the same path a second time, which isnt allowed!
    if (m_eConfigType != E_DOCUMENT)
        m_aSharedStorages->m_lStoragesShare.closePath(m_sRelPath);
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::getOrCreateRootStorageShare()
{
    css::uno::Reference< css::embed::XStorage > xRoot = m_aSharedStorages->m_lStoragesShare.getRootStorage();
    if (xRoot.is())
        return xRoot;

    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    css::uno::Reference< css::beans::XPropertySet > xPathSettings(
        xSMGR->createInstance(SERVICENAME_PATHSETTINGS),
        css::uno::UNO_QUERY_THROW);

    ::rtl::OUString sShareLayer;
    xPathSettings->getPropertyValue(BASEPATH_SHARE_LAYER) >>= sShareLayer;

    // "UIConfig" is a "multi path" ... use first part only here!
    sal_Int32 nPos = sShareLayer.indexOf(';');
    if (nPos > 0)
        sShareLayer = sShareLayer.copy(0, nPos);

    // Note: May be an user uses URLs without a final slash! Check it ...
    nPos = sShareLayer.lastIndexOf('/');
    if (nPos != sShareLayer.getLength()-1)
        sShareLayer += ::rtl::OUString::createFromAscii("/");

    sShareLayer += RELPATH_SHARE_LAYER; // folder
    /*
    // TODO remove me!
    // Attention: This is temp. workaround ... We create a temp. storage file
    // based of a sytem directory. This must be used so, till the storage implementation
    // can work on directories too.
    css::uno::Reference< css::embed::XStorage > xStorage = StorageHolder::createTempStorageBasedOnFolder(sShareLayer, xSMGR, sal_False); // false => writeable
    */
    css::uno::Sequence< css::uno::Any > lArgs(2);
    lArgs[0] <<= sShareLayer;
    lArgs[1] <<= css::embed::ElementModes::READ | css::embed::ElementModes::NOCREATE;

    css::uno::Reference< css::lang::XSingleServiceFactory > xStorageFactory(xSMGR->createInstance(SERVICENAME_STORAGEFACTORY)  , css::uno::UNO_QUERY_THROW);
    css::uno::Reference< css::embed::XStorage >             xStorage       (xStorageFactory->createInstanceWithArguments(lArgs), css::uno::UNO_QUERY      );

    m_aSharedStorages->m_lStoragesShare.setRootStorage(xStorage);

    return xStorage;
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::getOrCreateRootStorageUser()
{
    css::uno::Reference< css::embed::XStorage > xRoot = m_aSharedStorages->m_lStoragesUser.getRootStorage();
    if (xRoot.is())
        return xRoot;

    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    css::uno::Reference< css::beans::XPropertySet > xPathSettings(
        xSMGR->createInstance(SERVICENAME_PATHSETTINGS),
        css::uno::UNO_QUERY_THROW);

    ::rtl::OUString sUserLayer;
    xPathSettings->getPropertyValue(BASEPATH_USER_LAYER) >>= sUserLayer ;

    // Note: May be an user uses URLs without a final slash! Check it ...
    sal_Int32 nPos = sUserLayer.lastIndexOf('/');
    if (nPos != sUserLayer.getLength()-1)
        sUserLayer += ::rtl::OUString::createFromAscii("/");

    sUserLayer  += RELPATH_USER_LAYER; // storage file

    css::uno::Sequence< css::uno::Any > lArgs(2);
    lArgs[0] <<= sUserLayer;
    lArgs[1] <<= css::embed::ElementModes::READWRITE;

    css::uno::Reference< css::lang::XSingleServiceFactory > xStorageFactory(xSMGR->createInstance(SERVICENAME_STORAGEFACTORY)  , css::uno::UNO_QUERY_THROW);
    css::uno::Reference< css::embed::XStorage >             xStorage       (xStorageFactory->createInstanceWithArguments(lArgs), css::uno::UNO_QUERY      );
    m_aSharedStorages->m_lStoragesUser.setRootStorage(xStorage);

    return xStorage;
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::getWorkingStorageShare()
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    return m_xWorkingStorageShare;
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::getWorkingStorageUser()
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    return m_xWorkingStorageUser;
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::getParentStorageShare(const css::uno::Reference< css::embed::XStorage >& xChild)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xWorking = m_xWorkingStorageShare;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    return m_aSharedStorages->m_lStoragesShare.getParentStorage(xWorking);
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::getParentStorageUser(const css::uno::Reference< css::embed::XStorage >& xChild)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xWorking = m_xWorkingStorageUser;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    return m_aSharedStorages->m_lStoragesUser.getParentStorage(xWorking);
}

//-----------------------------------------------
void PresetHandler::connectToResource(      PresetHandler::EConfigType                   eConfigType  ,
                                      const ::rtl::OUString&                             sResource    ,
                                      const ::rtl::OUString&                             sModule      ,
                                      const css::uno::Reference< css::embed::XStorage >& xDocumentRoot,
                                      const ::comphelper::Locale&                        aLocale      )
{
    // TODO free all current open storages!

    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    m_eConfigType   = eConfigType  ;
    m_sResourceType = sResource    ;
    m_sModule       = sModule      ;
    m_aLocale       = aLocale      ;

    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    css::uno::Reference< css::embed::XStorage > xShare;
    css::uno::Reference< css::embed::XStorage > xUser;

    // special case for documents
    // use outside root storage, if we run in E_DOCUMENT mode!
    if (eConfigType == E_DOCUMENT)
    {
        if (!xDocumentRoot.is())
            throw css::uno::RuntimeException(
                    ::rtl::OUString::createFromAscii("There is valid root storage, where the UI configuration can work on."),
                    css::uno::Reference< css::uno::XInterface >());
        m_lDocumentStorages.setRootStorage(xDocumentRoot);
        xShare = xDocumentRoot;
        xUser  = xDocumentRoot;
    }
    else
    {
        xShare = getOrCreateRootStorageShare();
        xUser  = getOrCreateRootStorageUser();
    }

    sal_Int32 eShareMode = (css::embed::ElementModes::READ      | css::embed::ElementModes::NOCREATE); // means: use existing storage or create empty one!
    sal_Int32 eUserMode  = (css::embed::ElementModes::READWRITE                                     ); // means: use existing storage or create empty one ... and use it readonly if no write access!

    ::rtl::OUStringBuffer sRelPathBuf(1024);
    ::rtl::OUString       sRelPath;
    switch(eConfigType)
    {
        case E_GLOBAL :
        {
            sRelPathBuf.append(SUBSTORAGE_GLOBAL);
            sRelPathBuf.append(PATH_SEPERATOR   );
            sRelPathBuf.append(sResource        );
            sRelPath = sRelPathBuf.makeStringAndClear();

            xShare = impl_openPathIgnoringErrors(sRelPath, eShareMode, sal_True );
            xUser  = impl_openPathIgnoringErrors(sRelPath, eUserMode , sal_False);
        }
        break;

        case E_MODULES :
        {
            sRelPathBuf.append(SUBSTORAGE_MODULES);
            sRelPathBuf.append(PATH_SEPERATOR    );
            sRelPathBuf.append(sModule           );
            sRelPathBuf.append(PATH_SEPERATOR    );
            sRelPathBuf.append(sResource         );
            sRelPath = sRelPathBuf.makeStringAndClear();

            xShare = impl_openPathIgnoringErrors(sRelPath, eShareMode, sal_True );
            xUser  = impl_openPathIgnoringErrors(sRelPath, eUserMode , sal_False);
        }
        break;

        case E_DOCUMENT :
        {
            sRelPathBuf.append(sResource);
            sRelPath = sRelPathBuf.makeStringAndClear();

            try
            {
                xUser  = m_lDocumentStorages.openPath(sRelPath, eUserMode);
                xShare = xUser; // !!!
            }
            catch(const css::uno::RuntimeException& exRun)
                { throw exRun; }
            catch(const css::uno::Exception&)
                { xShare.clear(); xUser.clear(); }
        }
        break;
    }


    if (
        (aLocale     != ::comphelper::Locale::X_NOTRANSLATE()) && // localized level?
        (eConfigType != E_DOCUMENT                           )    // no localization in document mode!
       )
    {
        // use different start pathes ... because called method uses it as an in/out parameter!
        ::rtl::OUString sLocalizedSharePath(sRelPath);
        ::rtl::OUString sLocalizedUserPath (sRelPath);

        xShare   = impl_openLocalizedPathIgnoringErrors(sLocalizedSharePath, eShareMode, sal_True , aLocale);
        xUser    = impl_openLocalizedPathIgnoringErrors(sLocalizedUserPath , eUserMode , sal_False, aLocale);
        LOG_ASSERT(sLocalizedSharePath==sLocalizedUserPath, "Presethandler::connectToResource()\nDifferent localization sets inside share and user layer found ...")
        sRelPath = sLocalizedUserPath;
    }

    // read content of level 3 (presets, targets)
          css::uno::Reference< css::container::XNameAccess > xAccess ;
          css::uno::Sequence< ::rtl::OUString >              lNames  ;
    const ::rtl::OUString*                                   pNames  ;
          sal_Int32                                          c       ;
          sal_Int32                                          i       ;
          OUStringList                                       lPresets;
          OUStringList                                       lTargets;

    // read preset names of share layer
    xAccess = css::uno::Reference< css::container::XNameAccess >(xShare, css::uno::UNO_QUERY);
    if (xAccess.is())
    {
        lNames  = xAccess->getElementNames();
        pNames  = lNames.getConstArray();
        c       = lNames.getLength();

        for (i=0; i<c; ++i)
        {
            ::rtl::OUString sTemp = pNames[i];
            sal_Int32       nPos  = sTemp.indexOf(FILE_EXTENSION);
            if (nPos > -1)
                sTemp = sTemp.copy(0,nPos);
            lPresets.push_back(sTemp);
        }
    }

    // read preset names of user layer
    xAccess = css::uno::Reference< css::container::XNameAccess >(xUser, css::uno::UNO_QUERY);
    if (xAccess.is())
    {
        lNames  = xAccess->getElementNames();
        pNames  = lNames.getConstArray();
        c       = lNames.getLength();

        for (i=0; i<c; ++i)
        {
            ::rtl::OUString sTemp = pNames[i];
            sal_Int32       nPos  = sTemp.indexOf(FILE_EXTENSION);
            if (nPos > -1)
                sTemp = sTemp.copy(0,nPos);
            lTargets.push_back(sTemp);
        }
    }

    // SAFE -> ----------------------------------
    aWriteLock.lock();

    m_xWorkingStorageShare = xShare  ;
    m_xWorkingStorageUser  = xUser   ;
    m_lPresets             = lPresets;
    m_lTargets             = lTargets;
    m_sRelPath             = sRelPath;

    aWriteLock.unlock();
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
css::uno::Sequence< ::rtl::OUString > PresetHandler::getAllPresetNames() const
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    return m_lPresets.getAsConstList();
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
css::uno::Sequence< ::rtl::OUString > PresetHandler::getAllTargetNames() const
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    return m_lTargets.getAsConstList();
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
sal_Bool PresetHandler::existsPreset(const ::rtl::OUString& sPreset) const
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    return (m_lPresets.findConst(sPreset) != m_lPresets.end());
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
sal_Bool PresetHandler::existsTarget(const ::rtl::OUString& sTarget) const
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    return (m_lTargets.findConst(sTarget) != m_lTargets.end());
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
void PresetHandler::copyPresetToTarget(const ::rtl::OUString& sPreset,
                                       const ::rtl::OUString& sTarget)
{
    // dont check our preset list, if element exists
    // We try to open it and forward all errors to the user!

    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xWorkingShare = m_xWorkingStorageShare;
    css::uno::Reference< css::embed::XStorage > xWorkingUser  = m_xWorkingStorageUser ;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // e.g. module without any config data ?!
    if (
        (!xWorkingShare.is()) ||
        (!xWorkingUser.is() )
       )
    {
       return;
    }

    ::rtl::OUString sPresetFile(sPreset);
    sPresetFile += FILE_EXTENSION;

    ::rtl::OUString sTargetFile(sTarget);
    sTargetFile += FILE_EXTENSION;

    // remove existing elements before you try to copy the preset to that location ...
    // Otherwise w will get an ElementExistException inside copyElementTo()!
    css::uno::Reference< css::container::XNameAccess > xCheckingUser(xWorkingUser, css::uno::UNO_QUERY_THROW);
    if (xCheckingUser->hasByName(sTargetFile))
        xWorkingUser->removeElement(sTargetFile);

    xWorkingShare->copyElementTo(sPresetFile, xWorkingUser, sTargetFile);

    // If our storages work in transacted mode, we have
    // to commit all changes from bottom to top!
    commitUserChanges();
}

//-----------------------------------------------
css::uno::Reference< css::io::XStream > PresetHandler::openPreset(const ::rtl::OUString& sPreset)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xFolder = m_xWorkingStorageShare;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // e.g. module without any config data ?!
    if (!xFolder.is())
       return css::uno::Reference< css::io::XStream >();

    ::rtl::OUString sFile(sPreset);
    sFile += FILE_EXTENSION;

    // inform user about errors (use original exceptions!)
    css::uno::Reference< css::io::XStream > xStream = xFolder->openStreamElement(sFile, css::embed::ElementModes::READ);
    return xStream;
}

//-----------------------------------------------
css::uno::Reference< css::io::XStream > PresetHandler::openTarget(const ::rtl::OUString& sTarget         ,
                                                                        sal_Bool         bCreateIfMissing)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xFolder = m_xWorkingStorageUser;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // e.g. module without any config data ?!
    if (!xFolder.is())
       return css::uno::Reference< css::io::XStream >();

    ::rtl::OUString sFile(sTarget);
    sFile += FILE_EXTENSION;

    sal_Int32 nOpenMode = css::embed::ElementModes::READWRITE;
    if (!bCreateIfMissing)
        nOpenMode |= css::embed::ElementModes::NOCREATE;

    // try it in read/write mode first and ignore errors.
    css::uno::Reference< css::io::XStream > xStream;
    try
    {
        xStream = xFolder->openStreamElement(sFile, nOpenMode);
        return xStream;
    }
    catch(const css::uno::RuntimeException&)
        { throw; }
    catch(const css::uno::Exception&)
        { xStream.clear(); }

    // try it readonly if it failed before.
    // inform user about errors (use original exceptions!)
    nOpenMode &= ~css::embed::ElementModes::WRITE;
    xStream    = xFolder->openStreamElement(sFile, nOpenMode);

    return xStream;
}

//-----------------------------------------------
void PresetHandler::removeTarget(const ::rtl::OUString& sTarget)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xFolder = m_xWorkingStorageUser;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // e.g. module without any config data ?!
    if (!xFolder.is())
       return;

    ::rtl::OUString sFile(sTarget);
    sFile += FILE_EXTENSION;

    xFolder->removeElement(sFile);
    commitUserChanges();
}

//-----------------------------------------------
void PresetHandler::commitUserChanges()
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xWorking = m_xWorkingStorageUser;
    EConfigType                                 eCfgType = m_eConfigType;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // e.g. module without any config data ?!
    if (!xWorking.is())
       return;

    ::rtl::OUString sPath;

    switch(eCfgType)
    {
        case E_GLOBAL :
        case E_MODULES :
        {
            sPath = m_aSharedStorages->m_lStoragesUser.getPathOfStorage(xWorking);
            m_aSharedStorages->m_lStoragesUser.commitPath(sPath);
            m_aSharedStorages->m_lStoragesUser.notifyPath(sPath);
        }
        break;

        case E_DOCUMENT :
        {
            sPath = m_lDocumentStorages.getPathOfStorage(xWorking);
            m_lDocumentStorages.commitPath(sPath);
            m_lDocumentStorages.notifyPath(sPath);
        }
        break;
    }
}

//-----------------------------------------------
void PresetHandler::addStorageListener(IStorageListener* pListener)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    ::rtl::OUString sRelPath = m_sRelPath;
    EConfigType     eCfgType = m_eConfigType;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    if (!sRelPath.getLength())
        return;

    switch(eCfgType)
    {
        case E_GLOBAL :
        case E_MODULES :
        {
            m_aSharedStorages->m_lStoragesUser.addStorageListener(pListener, sRelPath);
        }
        break;

        case E_DOCUMENT :
        {
            m_lDocumentStorages.addStorageListener(pListener, sRelPath);
        }
        break;
    }
}

//-----------------------------------------------
void PresetHandler::removeStorageListener(IStorageListener* pListener)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    ::rtl::OUString sRelPath = m_sRelPath;
    EConfigType     eCfgType = m_eConfigType;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    if (!sRelPath.getLength())
        return;

    switch(eCfgType)
    {
        case E_GLOBAL :
        case E_MODULES :
        {
            m_aSharedStorages->m_lStoragesUser.removeStorageListener(pListener, sRelPath);
        }
        break;

        case E_DOCUMENT :
        {
            m_lDocumentStorages.removeStorageListener(pListener, sRelPath);
        }
        break;
    }
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::impl_openPathIgnoringErrors(const ::rtl::OUString& sPath ,
                                                                                             sal_Int32        eMode ,
                                                                                             sal_Bool         bShare)
{
    css::uno::Reference< css::embed::XStorage > xPath;
    try
    {
        if (bShare)
            xPath = m_aSharedStorages->m_lStoragesShare.openPath(sPath, eMode);
        else
            xPath = m_aSharedStorages->m_lStoragesUser.openPath(sPath, eMode);
    }
    catch(const css::uno::RuntimeException& exRun)
        { throw exRun; }
    catch(const css::uno::Exception&)
        { xPath.clear(); }
    return xPath;
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::impl_openLocalizedPathIgnoringErrors(      ::rtl::OUString&      sPath  ,
                                                                                                      sal_Int32             eMode  ,
                                                                                                      sal_Bool              bShare ,
                                                                                                const ::comphelper::Locale& aLocale)
{
    css::uno::Reference< css::embed::XStorage > xPath;
    ::rtl::OUString                             sLocalizedPath;
    ::comphelper::Locale                        aTryLocale = aLocale;
    while (sal_True)
    {
        sLocalizedPath  = sPath;
        sLocalizedPath += PATH_SEPERATOR;
        sLocalizedPath += aTryLocale.toISO();

        xPath = impl_openPathIgnoringErrors(sLocalizedPath, eMode, bShare);
        if (!xPath.is())
        {
            if (::comphelper::Locale::getFallback(aTryLocale))
                continue;
        }
        break;
    }

    // return localized path as result too
    if (xPath.is())
        sPath = sLocalizedPath;
    else
        sPath = ::rtl::OUString();
    return xPath;
}

//-----------------------------------------------
} // namespace framework
