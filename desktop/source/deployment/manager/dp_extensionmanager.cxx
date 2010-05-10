/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_desktop.hxx"

#include <cppuhelper/implbase1.hxx>

#include "comphelper/servicedecl.hxx"
#include "cppuhelper/exc_hlp.hxx"
#include "rtl/bootstrap.hxx"
#include "com/sun/star/deployment/ExtensionManager.hpp"
#include "com/sun/star/deployment/XExtensionManager.hpp"
#include "com/sun/star/deployment/thePackageManagerFactory.hpp"
#include "com/sun/star/deployment/XPackageManager.hpp"
#include "com/sun/star/deployment/XPackageManagerFactory.hpp"
#include "com/sun/star/deployment/XPackage.hpp"
#include "com/sun/star/deployment/InstallException.hpp"
#include "com/sun/star/deployment/VersionException.hpp"
#include "com/sun/star/deployment/LicenseException.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/registry/XRegistryKey.hpp"
#include "com/sun/star/beans/Optional.hpp"
#include "com/sun/star/task/XInteractionApprove.hpp"
#include "com/sun/star/beans/Ambiguous.hpp"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/io/XInputStream.hpp"
#include "com/sun/star/util/XModifyBroadcaster.hpp"
#include "comphelper/sequence.hxx"
#include "xmlscript/xml_helper.hxx"
#include "osl/diagnose.h"
#include "dp_interact.h"
#include "dp_resource.h"
#include "dp_ucb.h"
#include "dp_identifier.hxx"
#include "dp_descriptioninfoset.hxx"
#include "dp_extensionmanager.hxx"
#include "dp_commandenvironments.hxx"
#include "dp_properties.hxx"
#include "boost/bind.hpp"

#include <list>
#include <hash_map>
#include <algorithm>

namespace deploy = com::sun::star::deployment;
namespace lang  = com::sun::star::lang;
namespace registry = com::sun::star::registry;
namespace task = com::sun::star::task;
namespace ucb = com::sun::star::ucb;
namespace uno = com::sun::star::uno;
namespace beans = com::sun::star::beans;
namespace util = com::sun::star::util;
namespace css = com::sun::star;


//#define OUSTR(s) rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(s))

using ::com::sun::star::uno::Reference;
using ::rtl::OUString;

namespace {

struct CompIdentifiers
{
    bool operator() (::std::vector<Reference<deploy::XPackage> > const & a,
                     ::std::vector<Reference<deploy::XPackage> > const & b)
        {

            if (getName(a).compareTo(getName(b)) < 0)
                return true;
            return false;
        }

    OUString getName(::std::vector<Reference<deploy::XPackage> > const & a);
};

OUString CompIdentifiers::getName(::std::vector<Reference<deploy::XPackage> > const & a)
{
    OSL_ASSERT(a.size() == 3);
    //get the first non-null reference
    Reference<deploy::XPackage>  extension;
    ::std::vector<Reference<deploy::XPackage> >::const_iterator it = a.begin();
    for (; it != a.end(); it++)
    {
        if (it->is())
        {
            extension = *it;
            break;
        }
    }
    OSL_ASSERT(extension.is());
    return extension->getDisplayName();
}
} //end namespace

namespace dp_manager {



//------------------------------------------------------------------------------

//ToDo: bundled extension
ExtensionManager::ExtensionManager( Reference< uno::XComponentContext > const& xContext) :
    ::cppu::WeakComponentImplHelper1< css::deployment::XExtensionManager >(getMutex()),
    m_xContext( xContext )
{
    Reference<deploy::XPackageManagerFactory> xPackageManagerFactory(
        deploy::thePackageManagerFactory::get(m_xContext));
    m_userRepository = xPackageManagerFactory->getPackageManager(OUSTR("user"));
    m_sharedRepository = xPackageManagerFactory->getPackageManager(OUSTR("shared"));
    m_bundledRepository = xPackageManagerFactory->getPackageManager(OUSTR("bundled"));
    m_tmpRepository =  xPackageManagerFactory->getPackageManager(OUSTR("tmp"));

    m_repositoryNames.push_back(OUSTR("user"));
    m_repositoryNames.push_back(OUSTR("shared"));
    m_repositoryNames.push_back(OUSTR("bundled"));
}

//------------------------------------------------------------------------------

ExtensionManager::~ExtensionManager()
{
}

Reference<task::XAbortChannel> ExtensionManager::createAbortChannel()
    throw (uno::RuntimeException)
{
    return new dp_misc::AbortChannel;
}

css::uno::Reference<css::deployment::XPackageManager>
ExtensionManager::getPackageManager(::rtl::OUString const & repository)
    throw (css::lang::IllegalArgumentException)
{
    Reference<deploy::XPackageManager> xPackageManager;
    if (repository.equals(OUSTR("user")))
        xPackageManager = m_userRepository;
    else if (repository.equals(OUSTR("shared")))
        xPackageManager = m_sharedRepository;
    else if (repository.equals(OUSTR("bundled")))
        xPackageManager = m_bundledRepository;
    else
        throw lang::IllegalArgumentException(
            OUSTR("No valid repository name provided."),
            static_cast<cppu::OWeakObject*>(this), 0);
    return xPackageManager;
}


/*
  Enters the XPackage objects into a map. They must be all from the
  same repository. The value type of the map is a vector, where each vector
  represents an extension with a particular identifier. The first member
  is represents the user extension, the second the shared extension and the
  third the bundled extension.
 */
void ExtensionManager::addExtensionsToMap(
    id2extensions & mapExt,
    uno::Sequence<Reference<deploy::XPackage> > const & seqExt,
    OUString const & repository)
{
    //Determine the index in the vector where these extensions are to be
    //added.
    ::std::list<OUString>::const_iterator citNames =
        m_repositoryNames.begin();
    int index = 0;
    for (;citNames != m_repositoryNames.end(); citNames++, index++)
    {
        if (citNames->equals(repository))
            break;
    }

    for (int i = 0; i < seqExt.getLength(); i++)
    {
        Reference<deploy::XPackage> const & xExtension = seqExt[i];
        OUString id = dp_misc::getIdentifier(xExtension);
        id2extensions::iterator ivec =  mapExt.find(id);
        if (ivec == mapExt.end())
        {
            ::std::vector<Reference<deploy::XPackage> > vec(3);
            vec[index] = xExtension;
            mapExt[id] = vec;
        }
        else
        {
            ivec->second[index] = xExtension;
        }
    }
}

/*
   returns a list containing extensions with the same identifier from
   all repositories (user, shared, bundled) If one repository does not
   have this extension, then the list contains an empty Referenc. The list
   is ordered according to the priority of the repostories:
   1. user
   2. shared
   3. bundled

   The number of elements is always three, unless the number of repository
   changes.
 */
::std::list<Reference<deploy::XPackage> >
    ExtensionManager::getExtensionsWithSameId(
        OUString const & identifier, OUString const & fileName)

{
    ::std::list<Reference<deploy::XPackage> > extensionList;
    try
    {   //will throw an exception if the extension does not exist
        extensionList.push_back(m_userRepository->getDeployedPackage(
            identifier, fileName, Reference<ucb::XCommandEnvironment>()));
    } catch(lang::IllegalArgumentException &)
    {
        extensionList.push_back(Reference<deploy::XPackage>());
    }
    try
    {
        extensionList.push_back(m_sharedRepository->getDeployedPackage(
            identifier, fileName, Reference<ucb::XCommandEnvironment>()));
    } catch (lang::IllegalArgumentException &)
    {
        extensionList.push_back(Reference<deploy::XPackage>());
    }
    try
    {
       extensionList.push_back(m_bundledRepository->getDeployedPackage(
           identifier, fileName, Reference<ucb::XCommandEnvironment>()));
    } catch (lang::IllegalArgumentException &)
    {
        extensionList.push_back(Reference<deploy::XPackage>());
    }
    OSL_ASSERT(extensionList.size() == 3);
    return extensionList;
}



bool ExtensionManager::isUserDisabled(
    OUString const & identifier, OUString const & fileName)
{
    ::std::list<Reference<deploy::XPackage> > listExtensions =
        getExtensionsWithSameId(identifier, fileName);
    OSL_ASSERT(listExtensions.size() == 3);

    return isUserDisabled( ::comphelper::containerToSequence<
                           Reference<deploy::XPackage>,
                           ::std::list<Reference<deploy::XPackage> >
                           > (listExtensions));
}

bool ExtensionManager::isUserDisabled(
    uno::Sequence<Reference<deploy::XPackage> > const & seqExtSameId)
{
    OSL_ASSERT(seqExtSameId.getLength() == 3);
    Reference<deploy::XPackage> const & userExtension = seqExtSameId[0];
    if (userExtension.is())
    {
        beans::Optional<beans::Ambiguous<sal_Bool> > reg =
            userExtension->isRegistered(Reference<task::XAbortChannel>(),
                                        Reference<ucb::XCommandEnvironment>());
        //If the value is ambiguous is than we assume that the extension
        //is enabled, but something went wrong during enabling. We do not
        //automatically disable user extensions.
        if (reg.IsPresent &&
            ! reg.Value.IsAmbiguous && ! reg.Value.Value)
            return true;
    }
    return false;
}

/*
    This method determines the active extension (XPackage.registerPackage) with a
    particular identifier.

    The parameter bUserDisabled determines if the user extension is disabled.

    When the user repository contains an extension with the given identifier and
    it is not disabled by the user, then it is always registered.  Otherwise an
    extension is only registered when there is no registered extension in one of
    the repositories with a higher priority. That is, if the extension is from
    the shared repository and an active extension with the same identifer is in
    the user repository, then the extension is not registered. Similarly a
    bundled extension is not registered if there is an active extension with the
    same identifier in the shared or user repository.
*/
void ExtensionManager::activateExtension(
    OUString const & identifier, OUString const & fileName,
    bool bUserDisabled,
    bool bStartup,
    Reference<task::XAbortChannel> const & xAbortChannel,
    Reference<ucb::XCommandEnvironment> const & xCmdEnv )
{
    ::std::list<Reference<deploy::XPackage> > listExtensions =
        getExtensionsWithSameId(identifier, fileName);
    OSL_ASSERT(listExtensions.size() == 3);

    activateExtension(
        ::comphelper::containerToSequence<
        Reference<deploy::XPackage>,
        ::std::list<Reference<deploy::XPackage> >
        > (listExtensions),
        bUserDisabled, bStartup, xAbortChannel, xCmdEnv);

    fireModified();
}

void ExtensionManager::activateExtension(
    uno::Sequence<Reference<deploy::XPackage> > const & seqExt,
    bool bUserDisabled,
    bool bStartup,
    Reference<task::XAbortChannel> const & xAbortChannel,
    Reference<ucb::XCommandEnvironment> const & xCmdEnv )
{
    bool bActive = false;
    sal_Int32 len = seqExt.getLength();
    for (sal_Int32 i = 0; i < len; i++)
    {
        Reference<deploy::XPackage> const & aExt =  seqExt[i];
        if (aExt.is())
        {
            //get the registration value of the current iteration
            beans::Optional<beans::Ambiguous<sal_Bool> > optReg =
                aExt->isRegistered(xAbortChannel, xCmdEnv);
            //If nothing can be registered then break
            if (!optReg.IsPresent)
                break;

            //Check if this is a disabled user extension,
            if (i == 0 && bUserDisabled)
            {
                   aExt->revokePackage(xAbortChannel, xCmdEnv);
                   continue;
            }

            //If we have already determined an active extension then we must
            //make sure to unregister all extensions with the same id in
            //repositories with a lower priority
            if (bActive)
            {
                aExt->revokePackage(xAbortChannel, xCmdEnv);
            }
            else
            {
                //This is the first extension in the ordered list, which becomes
                //the active extension
                bActive = true;
                //Register if not already done.
                //reregister if the value is ambiguous, which indicates that
                //something went wrong during last registration.
                aExt->registerPackage(bStartup, xAbortChannel, xCmdEnv);
            }
        }
    }
}

Reference<deploy::XPackage> ExtensionManager::backupExtension(
    OUString const & identifier, OUString const & fileName,
    Reference<deploy::XPackageManager> const & xPackageManager,
    Reference<ucb::XCommandEnvironment> const & xCmdEnv )
{
    Reference<deploy::XPackage> xBackup;
    Reference<ucb::XCommandEnvironment> tmpCmdEnv(
        new TmpRepositoryCommandEnv(xCmdEnv->getInteractionHandler()));
    Reference<deploy::XPackage> xOldExtension;
    xOldExtension = xPackageManager->getDeployedPackage(
            identifier, fileName, tmpCmdEnv);

    if (xOldExtension.is())
    {
        xBackup = m_tmpRepository->addPackage(
            xOldExtension->getURL(), uno::Sequence<beans::NamedValue>(),
            OUString(), Reference<task::XAbortChannel>(), tmpCmdEnv);

        OSL_ENSURE(xBackup.is(), "Failed to backup extension");
    }
    return xBackup;
}

uno::Sequence< Reference<deploy::XPackageTypeInfo> >
ExtensionManager::getSupportedPackageTypes(OUString const & repository)
    throw (uno::RuntimeException)
{
    if (repository.equals(OUSTR("user")))
        return m_userRepository->getSupportedPackageTypes();
    else if (repository.equals(OUSTR("shared")))
        return m_sharedRepository->getSupportedPackageTypes();
    else
        return uno::Sequence< Reference<deploy::XPackageTypeInfo> >();
}

// Only add to shared and user repository
Reference<deploy::XPackage> ExtensionManager::addExtension(
    OUString const & url, uno::Sequence<beans::NamedValue> const & properties,
    OUString const & repository,
        Reference<task::XAbortChannel> const & xAbortChannel,
        Reference<ucb::XCommandEnvironment> const & xCmdEnv )
        throw (deploy::DeploymentException,
               ucb::CommandFailedException,
               ucb::CommandAbortedException,
               lang::IllegalArgumentException,
               uno::RuntimeException)
{
    Reference<deploy::XPackage> xNewExtension;
    //Determine the repository to use
    Reference<deploy::XPackageManager> xPackageManager;
    if (repository.equals(OUSTR("user")))
        xPackageManager = m_userRepository;
    else if (repository.equals(OUSTR("shared")))
        xPackageManager = m_sharedRepository;
    else
        throw lang::IllegalArgumentException(
            OUSTR("No valid repository name provided."),
            static_cast<cppu::OWeakObject*>(this), 0);

    ::osl::MutexGuard guard(getMutex());
    Reference<deploy::XPackage> xTmpExtension =
        getTempExtension(url, xAbortChannel, xCmdEnv);
    const OUString sIdentifier = dp_misc::getIdentifier(xTmpExtension);
    const OUString sFileName = xTmpExtension->getName();
    const OUString sDisplayName = xTmpExtension->getDisplayName();
    const OUString sVersion = xTmpExtension->getVersion();
    dp_misc::DescriptionInfoset info(dp_misc::getDescriptionInfoset(xTmpExtension->getURL()));
    const ::boost::optional<dp_misc::SimpleLicenseAttributes> licenseAttributes =
        info.getSimpleLicenseAttributes();

    Reference<deploy::XPackage> xOldExtension;
    Reference<deploy::XPackage> xExtensionBackup;

    uno::Any excOccurred1;
    uno::Any excOccurred2;
    bool bUserDisabled = false;
    try
    {
        bUserDisabled = isUserDisabled(sIdentifier, sFileName);
        try
        {
            xOldExtension = xPackageManager->getDeployedPackage(
                sIdentifier, sFileName, xCmdEnv);
        }
        catch (lang::IllegalArgumentException &)
        {
        }
        bool bCanInstall = false;
        try
        {
            if (xOldExtension.is())
            {
                //throws a CommandFailedException if the user cancels
                //the action.
                checkUpdate(sVersion, sDisplayName,xOldExtension, xCmdEnv);
            }
            else
            {
                //throws a CommandFailedException if the user cancels
                //the action.
                checkInstall(sDisplayName, xCmdEnv);
            }

            //Prevent showing the license if requested.
            Reference<ucb::XCommandEnvironment> _xCmdEnv(xCmdEnv);
            ExtensionProperties props(OUString(), properties, Reference<ucb::XCommandEnvironment>());
            if (licenseAttributes && licenseAttributes->suppressIfRequired
                && props.isSuppressedLicense())
                _xCmdEnv = Reference<ucb::XCommandEnvironment>(new NoLicenseCommandEnv(xCmdEnv->getInteractionHandler()));

            bCanInstall = xTmpExtension->checkPrerequisites(
                xAbortChannel, _xCmdEnv, xOldExtension.is()) == 0 ? true : false;
        }
        catch (deploy::DeploymentException& ) {
            excOccurred1 = ::cppu::getCaughtException();
        } catch (ucb::CommandFailedException & ) {
            excOccurred1 = ::cppu::getCaughtException();
        } catch (ucb::CommandAbortedException & ) {
            excOccurred1 = ::cppu::getCaughtException();
        } catch (lang::IllegalArgumentException &) {
            excOccurred1 = ::cppu::getCaughtException();
        } catch (uno::RuntimeException &) {
            excOccurred1 = ::cppu::getCaughtException();
        } catch (...) {
            excOccurred1 = ::cppu::getCaughtException();
            deploy::DeploymentException exc(
                OUSTR("Extension Manager: exception during addExtension, url: ")
                + url, static_cast<OWeakObject*>(this), excOccurred1);
            excOccurred1 <<= exc;
        }

        if (bCanInstall)
        {
            if (xOldExtension.is())
            {
                xOldExtension->revokePackage(xAbortChannel, xCmdEnv);
                //save the old user extension in case the user aborts
                //store the extension in the tmp repository, this will overwrite
                //xTmpPackage (same identifier). Do not let the user abort or
                //interact
                Reference<ucb::XCommandEnvironment> tmpCmdEnv(
                    new TmpRepositoryCommandEnv(xCmdEnv->getInteractionHandler()));
                //importing the old extension in the tmp repository will remove
                //the xTmpExtension
                xTmpExtension = 0;
                xExtensionBackup = m_tmpRepository->importExtension(
                    xOldExtension, Reference<task::XAbortChannel>(),
                    tmpCmdEnv);
            }

            xNewExtension = xPackageManager->addPackage(
                url, properties, OUString(), xAbortChannel, xCmdEnv);

            //If we add a user extension and there is already one which was
            //disabled by a user, then the newly installed one is enabled. If we
            //add to another repository then the user extension remains
            //disabled.
            bool bUserDisabled2 = bUserDisabled;
            if (repository.equals(OUSTR("user")))
                bUserDisabled2 = false;
            activateExtension(
                dp_misc::getIdentifier(xNewExtension),
                xNewExtension->getName(), bUserDisabled2, false, xAbortChannel, xCmdEnv);
            fireModified();
        }
    }
    catch (deploy::DeploymentException& ) {
        excOccurred2 = ::cppu::getCaughtException();
    } catch (ucb::CommandFailedException & ) {
        excOccurred2 = ::cppu::getCaughtException();
    } catch (ucb::CommandAbortedException & ) {
        excOccurred2 = ::cppu::getCaughtException();
    } catch (lang::IllegalArgumentException &) {
        excOccurred2 = ::cppu::getCaughtException();
    } catch (uno::RuntimeException &) {
        excOccurred2 = ::cppu::getCaughtException();
    } catch (...) {
        excOccurred2 = ::cppu::getCaughtException();
        deploy::DeploymentException exc(
            OUSTR("Extension Manager: exception during addExtension, url: ")
            + url, static_cast<OWeakObject*>(this), excOccurred2);
        excOccurred2 <<= exc;
    }

    if (excOccurred2.hasValue())
    {
        //It does not matter what exception is thrown. We try to
        //recover the original status.
        //If the user aborted installation then a ucb::CommandAbortedException
        //is thrown.
        //Use a private AbortChannel so the user cannot interrupt.
        try
        {
            Reference<ucb::XCommandEnvironment> tmpCmdEnv(
                new TmpRepositoryCommandEnv(xCmdEnv->getInteractionHandler()));
            if (xExtensionBackup.is())
            {
                Reference<deploy::XPackage> xRestored =
                    xPackageManager->importExtension(
                        xExtensionBackup, Reference<task::XAbortChannel>(),
                        tmpCmdEnv);
            }
            activateExtension(
                sIdentifier, sFileName, bUserDisabled, false,
                Reference<task::XAbortChannel>(), tmpCmdEnv);
            if (xTmpExtension.is() || xExtensionBackup.is())
                m_tmpRepository->removePackage(
                    sIdentifier, OUString(), xAbortChannel, xCmdEnv);
            fireModified();
        }
        catch (...)
        {
        }
        ::cppu::throwException(excOccurred2);
    }
    if (xTmpExtension.is() || xExtensionBackup.is())
        m_tmpRepository->removePackage(
            sIdentifier,OUString(), xAbortChannel, xCmdEnv);

    if (excOccurred1.hasValue())
        ::cppu::throwException(excOccurred1);

    return xNewExtension;
}

void ExtensionManager::removeExtension(
    OUString const & identifier, OUString const & fileName,
    OUString const & repository,
    Reference<task::XAbortChannel> const & xAbortChannel,
    Reference<ucb::XCommandEnvironment> const & xCmdEnv )
    throw (deploy::DeploymentException,
           ucb::CommandFailedException,
           ucb::CommandAbortedException,
           lang::IllegalArgumentException,
           uno::RuntimeException)
{
    uno::Any excOccurred1;
    Reference<deploy::XPackage> xExtensionBackup;
    Reference<deploy::XPackageManager> xPackageManager;
    bool bUserDisabled = false;
    ::osl::MutexGuard guard(getMutex());
    try
    {
//Determine the repository to use
        if (repository.equals(OUSTR("user")))
            xPackageManager = m_userRepository;
        else if (repository.equals(OUSTR("shared")))
            xPackageManager = m_sharedRepository;
        else
            throw lang::IllegalArgumentException(
                OUSTR("No valid repository name provided."),
                static_cast<cppu::OWeakObject*>(this), 0);

        bUserDisabled = isUserDisabled(identifier, fileName);
        //Backup the extension, in case the user cancels the action
        xExtensionBackup = backupExtension(
            identifier, fileName, xPackageManager, xCmdEnv);

        //revoke the extension if it is active
        Reference<deploy::XPackage> xOldExtension =
            xPackageManager->getDeployedPackage(
                identifier, fileName, xCmdEnv);
        xOldExtension->revokePackage(xAbortChannel, xCmdEnv);

        xPackageManager->removePackage(
            identifier, fileName, xAbortChannel, xCmdEnv);
        activateExtension(identifier, fileName, bUserDisabled, false,
                          xAbortChannel, xCmdEnv);
        fireModified();
    }
    catch (deploy::DeploymentException& ) {
        excOccurred1 = ::cppu::getCaughtException();
    } catch (ucb::CommandFailedException & ) {
        excOccurred1 = ::cppu::getCaughtException();
    } catch (ucb::CommandAbortedException & ) {
        excOccurred1 = ::cppu::getCaughtException();
    } catch (lang::IllegalArgumentException &) {
        excOccurred1 = ::cppu::getCaughtException();
    } catch (uno::RuntimeException &) {
        excOccurred1 = ::cppu::getCaughtException();
    } catch (...) {
        excOccurred1 = ::cppu::getCaughtException();
        deploy::DeploymentException exc(
            OUSTR("Extension Manager: exception during removeEtension"),
            static_cast<OWeakObject*>(this), excOccurred1);
        excOccurred1 <<= exc;
    }

    if (excOccurred1.hasValue())
    {
        //User aborted installation, restore the previous situation.
        //Use a private AbortChannel so the user cannot interrupt.
        try
        {
            Reference<ucb::XCommandEnvironment> tmpCmdEnv(
                new TmpRepositoryCommandEnv(xCmdEnv->getInteractionHandler()));
            if (xExtensionBackup.is())
            {
                Reference<deploy::XPackage> xRestored =
                    xPackageManager->importExtension(
                        xExtensionBackup, Reference<task::XAbortChannel>(),
                        tmpCmdEnv);
                activateExtension(
                    identifier, fileName, bUserDisabled, false,
                    Reference<task::XAbortChannel>(),
                    tmpCmdEnv);

                m_tmpRepository->removePackage(
                    dp_misc::getIdentifier(xExtensionBackup),
                    xExtensionBackup->getName(), xAbortChannel, xCmdEnv);
                fireModified();
            }
        }
        catch (...)
        {
        }
        ::cppu::throwException(excOccurred1);
    }

    if (xExtensionBackup.is())
        m_tmpRepository->removePackage(
            dp_misc::getIdentifier(xExtensionBackup),
            xExtensionBackup->getName(), xAbortChannel, xCmdEnv);
}

// Only enable extensions from shared and user repository
void ExtensionManager::enableExtension(
    Reference<deploy::XPackage> const & extension,
    Reference<task::XAbortChannel> const & xAbortChannel,
    Reference<ucb::XCommandEnvironment> const & xCmdEnv)
    throw (deploy::DeploymentException,
        ucb::CommandFailedException,
        ucb::CommandAbortedException,
        lang::IllegalArgumentException,
        uno::RuntimeException)
{
    ::osl::MutexGuard guard(getMutex());
    bool bUserDisabled = false;
    uno::Any excOccurred;
    try
    {
        if (!extension.is())
            return;
        OUString repository = extension->getRepositoryName();
        if (!repository.equals(OUSTR("user")))
            throw lang::IllegalArgumentException(
                OUSTR("No valid repository name provided."),
                static_cast<cppu::OWeakObject*>(this), 0);

        bUserDisabled = isUserDisabled(dp_misc::getIdentifier(extension),
                                       extension->getName());

        activateExtension(dp_misc::getIdentifier(extension),
                          extension->getName(), false, false,
                          xAbortChannel, xCmdEnv);
    }
    catch (deploy::DeploymentException& ) {
        excOccurred = ::cppu::getCaughtException();
    } catch (ucb::CommandFailedException & ) {
        excOccurred = ::cppu::getCaughtException();
    } catch (ucb::CommandAbortedException & ) {
        excOccurred = ::cppu::getCaughtException();
    } catch (lang::IllegalArgumentException &) {
        excOccurred = ::cppu::getCaughtException();
    } catch (uno::RuntimeException &) {
        excOccurred = ::cppu::getCaughtException();
    } catch (...) {
        excOccurred = ::cppu::getCaughtException();
        deploy::DeploymentException exc(
            OUSTR("Extension Manager: exception during enableExtension"),
            static_cast<OWeakObject*>(this), excOccurred);
        excOccurred <<= exc;
    }

    if (excOccurred.hasValue())
    {
        try
        {
            activateExtension(dp_misc::getIdentifier(extension),
                              extension->getName(), bUserDisabled, false,
                              xAbortChannel, xCmdEnv);
        }
        catch (...)
        {
        }
        ::cppu::throwException(excOccurred);
    }
}

/**
 */
long ExtensionManager::checkPrerequisitesAndEnable(
    Reference<deploy::XPackage> const & extension,
    Reference<task::XAbortChannel> const & xAbortChannel,
    Reference<ucb::XCommandEnvironment> const & xCmdEnv)
    throw (deploy::DeploymentException,
        ucb::CommandFailedException,
        ucb::CommandAbortedException,
        lang::IllegalArgumentException,
        uno::RuntimeException)
{
    try
    {
        if (!extension.is())
            return 0;
        ::osl::MutexGuard guard(getMutex());
        sal_Int32 ret = 0;
        Reference<deploy::XPackageManager> mgr =
            getPackageManager(extension->getRepositoryName());
        ret = mgr->checkPrerequisites(extension, xAbortChannel, xCmdEnv);
        if (ret)
        {
            //There are some unfulfilled prerequisites, try to revoke
            extension->revokePackage(xAbortChannel, xCmdEnv);
        }
        const OUString id(dp_misc::getIdentifier(extension));
        activateExtension(id, extension->getName(),
                          isUserDisabled(id, extension->getName()), false,
                          xAbortChannel, xCmdEnv);
        return ret;
    }
    catch (deploy::DeploymentException& ) {
        throw;
    } catch (ucb::CommandFailedException & ) {
        throw;
    } catch (ucb::CommandAbortedException & ) {
        throw;
    } catch (lang::IllegalArgumentException &) {
        throw;
    } catch (uno::RuntimeException &) {
        throw;
    } catch (...) {
        uno::Any excOccurred = ::cppu::getCaughtException();
        deploy::DeploymentException exc(
            OUSTR("Extension Manager: exception during disableExtension"),
            static_cast<OWeakObject*>(this), excOccurred);
        throw exc;
    }
}


void ExtensionManager::disableExtension(
    Reference<deploy::XPackage> const & extension,
    Reference<task::XAbortChannel> const & xAbortChannel,
    Reference<ucb::XCommandEnvironment> const & xCmdEnv )
    throw (deploy::DeploymentException,
           ucb::CommandFailedException,
           ucb::CommandAbortedException,
           lang::IllegalArgumentException,
           uno::RuntimeException)
{
    ::osl::MutexGuard guard(getMutex());
    uno::Any excOccurred;
    bool bUserDisabled = false;
    try
    {
        if (!extension.is())
            return;
        const OUString repository( extension->getRepositoryName());
        if (!repository.equals(OUSTR("user")))
            throw lang::IllegalArgumentException(
                OUSTR("No valid repository name provided."),
                static_cast<cppu::OWeakObject*>(this), 0);

        const OUString id(dp_misc::getIdentifier(extension));
        bUserDisabled = isUserDisabled(id, extension->getName());

        activateExtension(id, extension->getName(), true, false,
                          xAbortChannel, xCmdEnv);
    }
    catch (deploy::DeploymentException& ) {
        excOccurred = ::cppu::getCaughtException();
    } catch (ucb::CommandFailedException & ) {
        excOccurred = ::cppu::getCaughtException();
    } catch (ucb::CommandAbortedException & ) {
        excOccurred = ::cppu::getCaughtException();
    } catch (lang::IllegalArgumentException &) {
        excOccurred = ::cppu::getCaughtException();
    } catch (uno::RuntimeException &) {
        excOccurred = ::cppu::getCaughtException();
    } catch (...) {
        excOccurred = ::cppu::getCaughtException();
        deploy::DeploymentException exc(
            OUSTR("Extension Manager: exception during disableExtension"),
            static_cast<OWeakObject*>(this), excOccurred);
        excOccurred <<= exc;
    }

    if (excOccurred.hasValue())
    {
        try
        {
            activateExtension(dp_misc::getIdentifier(extension),
                              extension->getName(), bUserDisabled, false,
                              xAbortChannel, xCmdEnv);
        }
        catch (...)
        {
        }
        ::cppu::throwException(excOccurred);
    }
}

uno::Sequence< Reference<deploy::XPackage> >
    ExtensionManager::getDeployedExtensions(
    OUString const & repository,
    Reference<task::XAbortChannel> const &xAbort,
    Reference<ucb::XCommandEnvironment> const & xCmdEnv )
    throw (deploy::DeploymentException,
        ucb::CommandFailedException,
        ucb::CommandAbortedException,
        lang::IllegalArgumentException,
        uno::RuntimeException)
{
    return getPackageManager(repository)->getDeployedPackages(
        xAbort, xCmdEnv);
}

Reference<deploy::XPackage>
    ExtensionManager::getDeployedExtension(
    OUString const & repository,
    OUString const & identifier,
    OUString const & filename,
    Reference<ucb::XCommandEnvironment> const & xCmdEnv )
    throw (deploy::DeploymentException,
        ucb::CommandFailedException,
        lang::IllegalArgumentException,
        uno::RuntimeException)
{
    return getPackageManager(repository)->getDeployedPackage(
        identifier, filename, xCmdEnv);
}

uno::Sequence< uno::Sequence<Reference<deploy::XPackage> > >
    ExtensionManager::getAllExtensions(
    Reference<task::XAbortChannel> const & xAbort,
    Reference<ucb::XCommandEnvironment> const & xCmdEnv )
    throw (deploy::DeploymentException,
        ucb::CommandFailedException,
        ucb::CommandAbortedException,
        lang::IllegalArgumentException,
        uno::RuntimeException)
{
    try
    {
        id2extensions mapExt;

        uno::Sequence<Reference<deploy::XPackage> > userExt =
            m_userRepository->getDeployedPackages(xAbort, xCmdEnv);
        addExtensionsToMap(mapExt, userExt, OUSTR("user"));
        uno::Sequence<Reference<deploy::XPackage> > sharedExt =
            m_sharedRepository->getDeployedPackages(xAbort, xCmdEnv);
        addExtensionsToMap(mapExt, sharedExt, OUSTR("shared"));
        uno::Sequence<Reference<deploy::XPackage> > bundledExt =
            m_bundledRepository->getDeployedPackages(xAbort, xCmdEnv);
        addExtensionsToMap(mapExt, bundledExt, OUSTR("bundled"));

        //copy the values of the map to a vector for sorting
        ::std::vector< ::std::vector<Reference<deploy::XPackage> > >
              vecExtensions;
        id2extensions::const_iterator mapIt = mapExt.begin();
        for (;mapIt != mapExt.end(); mapIt++)
            vecExtensions.push_back(mapIt->second);

        //sort the element according to the identifier
        ::std::sort(vecExtensions.begin(), vecExtensions.end(), CompIdentifiers());

        ::std::vector< ::std::vector<Reference<deploy::XPackage> > >::const_iterator
              citVecVec = vecExtensions.begin();
        sal_Int32 j = 0;
        uno::Sequence< uno::Sequence<Reference<deploy::XPackage> > > seqSeq(vecExtensions.size());
        for (;citVecVec != vecExtensions.end(); citVecVec++, j++)
        {
            seqSeq[j] = comphelper::containerToSequence(*citVecVec);
        }
        return seqSeq;

    } catch (deploy::DeploymentException& ) {
        throw;
    } catch (ucb::CommandFailedException & ) {
        throw;
    } catch (ucb::CommandAbortedException & ) {
        throw;
    } catch (lang::IllegalArgumentException &) {
        throw;
    } catch (uno::RuntimeException &) {
        throw;
    } catch (...) {
        uno::Any exc = ::cppu::getCaughtException();
        throw deploy::DeploymentException(
            OUSTR("Extension Manager: exception during enableExtension"),
            static_cast<OWeakObject*>(this), exc);
   }
}

void ExtensionManager::reinstallDeployedExtensions(
    OUString const & repository,
    Reference<task::XAbortChannel> const & xAbortChannel,
    Reference<ucb::XCommandEnvironment> const & xCmdEnv )
    throw (deploy::DeploymentException,
        ucb::CommandFailedException, ucb::CommandAbortedException,
        lang::IllegalArgumentException, uno::RuntimeException)
{
    try
    {
        Reference<deploy::XPackageManager>
            xPackageManager = getPackageManager(repository);

        ::osl::MutexGuard guard(getMutex());
        xPackageManager->reinstallDeployedPackages(xAbortChannel, xCmdEnv);
        const uno::Sequence< Reference<deploy::XPackage> > extensions(
            xPackageManager->getDeployedPackages(xAbortChannel, xCmdEnv));

        for ( sal_Int32 pos = 0; pos < extensions.getLength(); ++pos )
        {
            try
            {
                const OUString id =  dp_misc::getIdentifier(extensions[ pos ]);
                const OUString fileName = extensions[ pos ]->getName();
                OSL_ASSERT(id.getLength());
                activateExtension(id, fileName, false, false, xAbortChannel, xCmdEnv );
            }
            catch (lang::DisposedException &)
            {
            }
        }
    } catch (deploy::DeploymentException& ) {
        throw;
    } catch (ucb::CommandFailedException & ) {
        throw;
    } catch (ucb::CommandAbortedException & ) {
        throw;
    } catch (lang::IllegalArgumentException &) {
        throw;
    } catch (uno::RuntimeException &) {
        throw;
    } catch (...) {
        uno::Any exc = ::cppu::getCaughtException();
        throw deploy::DeploymentException(
            OUSTR("Extension Manager: exception during enableExtension"),
            static_cast<OWeakObject*>(this), exc);
    }
}

void ExtensionManager::synchronize(
    OUString const & repository,
    Reference<task::XAbortChannel> const & xAbortChannel,
    Reference<ucb::XCommandEnvironment> const & xCmdEnv )
    throw (deploy::DeploymentException,
           ucb::CommandFailedException,
           ucb::CommandAbortedException,
           lang::IllegalArgumentException,
           uno::RuntimeException)
{
    try
    {
        Reference<deploy::XPackageManager> xPackageManager;
        OUString file;
        if (repository.equals(OUSTR("user")))
        {
            xPackageManager = m_userRepository;
        }
        else if (repository.equals(OUSTR("shared")))
        {
            xPackageManager = m_sharedRepository;
            file = OUString (
                RTL_CONSTASCII_USTRINGPARAM(
                    "$SHARED_EXTENSIONS_USER/lastsynchronized"));
        }
        else if (repository.equals(OUSTR("bundled")))
        {
            xPackageManager = m_bundledRepository;
            file = OUString (
                RTL_CONSTASCII_USTRINGPARAM(
                    "$BUNDLED_EXTENSIONS_USER/lastsynchronized"));
        }
        else
            throw lang::IllegalArgumentException(
                OUSTR("No valid repository name provided."),
                static_cast<cppu::OWeakObject*>(this), 0);

        ::osl::MutexGuard guard(getMutex());
        String sSynchronizing(StrSyncRepository::get());
        sSynchronizing.SearchAndReplaceAllAscii( "%NAME", repository );
        dp_misc::ProgressLevel progress(xCmdEnv, sSynchronizing);

        xPackageManager->synchronize(xAbortChannel, xCmdEnv);
        try
        {
            const uno::Sequence<uno::Sequence<Reference<deploy::XPackage> > >
                seqSeqExt = getAllExtensions(xAbortChannel, xCmdEnv);
            for (sal_Int32 i = 0; i < seqSeqExt.getLength(); i++)
            {
                uno::Sequence<Reference<deploy::XPackage> > const & seqExt =
                    seqSeqExt[i];
                activateExtension(seqExt, isUserDisabled(seqExt), true,
                                  xAbortChannel, xCmdEnv);
            }
        }
        catch (...)
        {
            //We catch the exception, so we can write the lastmodified file
            //so we will no repeat this everytime OOo starts.
            OSL_ENSURE(0, "Extensions Manager: synchronize");
        }


        progress.update(OUSTR("\n\n"));

        //Write the lastmodified file
        try {
            ::rtl::Bootstrap::expandMacros(file);
            ::ucbhelper::Content ucbStamp(file, xCmdEnv );
            dp_misc::erase_path( file, xCmdEnv );
            ::rtl::OString stamp("1" );
            Reference<css::io::XInputStream> xData(
                ::xmlscript::createInputStream(
                    ::rtl::ByteSequence(
                        reinterpret_cast<sal_Int8 const *>(stamp.getStr()),
                        stamp.getLength() ) ) );
            ucbStamp.writeStream( xData, true /* replace existing */ );
        }
        catch(...)
        {
            uno::Any exc(::cppu::getCaughtException());
            throw deploy::DeploymentException(
                OUSTR("Failed to update") + file,
                static_cast<OWeakObject*>(this), exc);

        }
    } catch (deploy::DeploymentException& ) {
        throw;
    } catch (ucb::CommandFailedException & ) {
        throw;
    } catch (ucb::CommandAbortedException & ) {
        throw;
    } catch (lang::IllegalArgumentException &) {
        throw;
    } catch (uno::RuntimeException &) {
        throw;
    } catch (...) {
        uno::Any exc = ::cppu::getCaughtException();
        throw deploy::DeploymentException(
            OUSTR("Extension Manager: exception during enableExtension"),
            static_cast<OWeakObject*>(this), exc);
    }
}

// Notify the user when a new extension is to be installed. This is only the
// case when one uses the system integration to install an extension (double
// clicking on .oxt file etc.)). The function must only be called if there is no
// extension with the same identifier already deployed. Then the checkUpdate
// function will inform the user that the extension is about to be installed In
// case the user cancels the installation a CommandFailed exception is
// thrown.
void ExtensionManager::checkInstall(
    OUString const & displayName,
    Reference<ucb::XCommandEnvironment> const & cmdEnv)
{
        uno::Any request(
            deploy::InstallException(
                OUSTR("Extension ") + displayName +
                OUSTR(" is about to be installed."),
                static_cast<OWeakObject *>(this), displayName));
        bool approve = false, abort = false;
        if (! dp_misc::interactContinuation(
                request, task::XInteractionApprove::static_type(),
                cmdEnv, &approve, &abort ))
        {
            OSL_ASSERT( !approve && !abort );
            throw deploy::DeploymentException(
                dp_misc::getResourceString(RID_STR_ERROR_WHILE_ADDING) + displayName,
                static_cast<OWeakObject *>(this), request );
        }
        if (abort || !approve)
            throw ucb::CommandFailedException(
                dp_misc::getResourceString(RID_STR_ERROR_WHILE_ADDING) + displayName,
                static_cast<OWeakObject *>(this), request );
}

/* The function will make the user interaction in case there is an extension
installed with the same id. This function may only be called if there is already
an extension.
*/
void ExtensionManager::checkUpdate(
    OUString const & newVersion,
    OUString const & newDisplayName,
    Reference<deploy::XPackage> const & oldExtension,
    Reference<ucb::XCommandEnvironment> const & xCmdEnv )
{
    // package already deployed, interact --force:
    uno::Any request(
        (deploy::VersionException(
            dp_misc::getResourceString(
                RID_STR_PACKAGE_ALREADY_ADDED ) + newDisplayName,
            static_cast<OWeakObject *>(this), newVersion, newDisplayName,
            oldExtension ) ) );
    bool replace = false, abort = false;
    if (! dp_misc::interactContinuation(
            request, task::XInteractionApprove::static_type(),
            xCmdEnv, &replace, &abort )) {
        OSL_ASSERT( !replace && !abort );
        throw deploy::DeploymentException(
            dp_misc::getResourceString(
                RID_STR_ERROR_WHILE_ADDING) + newDisplayName,
            static_cast<OWeakObject *>(this), request );
    }
    if (abort || !replace)
        throw ucb::CommandFailedException(
            dp_misc::getResourceString(
                RID_STR_PACKAGE_ALREADY_ADDED) + newDisplayName,
            static_cast<OWeakObject *>(this), request );
}

Reference<deploy::XPackage> ExtensionManager::getTempExtension(
    OUString const & url,
    Reference<task::XAbortChannel> const & xAbortChannel,
    Reference<ucb::XCommandEnvironment> const & /*xCmdEnv*/)

{
    Reference<ucb::XCommandEnvironment> tmpCmdEnvA(new TmpRepositoryCommandEnv());
    Reference<deploy::XPackage> xTmpPackage = m_tmpRepository->addPackage(
        url, uno::Sequence<beans::NamedValue>(),OUString(), xAbortChannel, tmpCmdEnvA);

    if (!xTmpPackage.is())
    {
        throw deploy::DeploymentException(
            OUSTR("Extension Manager: Failed to create temporary XPackage for url: ") + url,
            static_cast<OWeakObject*>(this), uno::Any());

    }
    return xTmpPackage;
}

uno::Sequence<Reference<deploy::XPackage> > SAL_CALL
ExtensionManager::getExtensionsWithUnacceptedLicenses(
        OUString const & repository,
        Reference<ucb::XCommandEnvironment> const & xCmdEnv)
        throw (deploy::DeploymentException,
               uno::RuntimeException)
{
    Reference<deploy::XPackageManager>
        xPackageManager = getPackageManager(repository);
    ::osl::MutexGuard guard(getMutex());
    return xPackageManager->getExtensionsWithUnacceptedLicenses(xCmdEnv);
}

sal_Bool ExtensionManager::isReadOnlyRepository(::rtl::OUString const & repository)
        throw (uno::RuntimeException)
{
    return getPackageManager(repository)->isReadOnly();
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace sdecl = comphelper::service_decl;
sdecl::class_<ExtensionManager> servicePIP;
extern sdecl::ServiceDecl const serviceDecl(
    servicePIP,
    // a private one:
    "com.sun.star.comp.deployment.ExtensionManager",
    "com.sun.star.comp.deployment.ExtensionManager");

//------------------------------------------------------------------------------
bool singleton_entries(
    uno::Reference< registry::XRegistryKey > const & xRegistryKey )
{
    try {
        uno::Reference< registry::XRegistryKey > xKey(
            xRegistryKey->createKey(
                serviceDecl.getImplementationName() +
                // xxx todo: use future generated function to get singleton name
                OUSTR("/UNO/SINGLETONS/"
                      "com.sun.star.deployment.ExtensionManager") ) );
        xKey->setStringValue( serviceDecl.getSupportedServiceNames()[0] );
        return true;
    }
    catch (registry::InvalidRegistryException & exc) {
        (void) exc; // avoid warnings
        OSL_ENSURE( 0, ::rtl::OUStringToOString(
                        exc.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
        return false;
    }
}

// XModifyBroadcaster
//______________________________________________________________________________
void ExtensionManager::addModifyListener(
    Reference<util::XModifyListener> const & xListener )
    throw (uno::RuntimeException)
{
     check();
     rBHelper.addListener( ::getCppuType( &xListener ), xListener );
}

//______________________________________________________________________________
void ExtensionManager::removeModifyListener(
    Reference<util::XModifyListener> const & xListener )
    throw (uno::RuntimeException)
{
    check();
    rBHelper.removeListener( ::getCppuType( &xListener ), xListener );
}

void ExtensionManager::check()
{
    ::osl::MutexGuard guard( getMutex() );
    if (rBHelper.bInDispose || rBHelper.bDisposed) {
        throw lang::DisposedException(
            OUSTR("ExtensionManager instance has already been disposed!"),
            static_cast<OWeakObject *>(this) );
    }
}

void ExtensionManager::fireModified()
{
    ::cppu::OInterfaceContainerHelper * pContainer = rBHelper.getContainer(
        util::XModifyListener::static_type() );
    if (pContainer != 0) {
        pContainer->forEach<util::XModifyListener>(
            boost::bind(&util::XModifyListener::modified, _1,
                        lang::EventObject(static_cast<OWeakObject *>(this))) );
    }
}

} // namespace dp_manager


