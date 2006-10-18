/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: MNSProfileDiscover.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: ihi $ $Date: 2006-10-18 13:10:35 $
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
#include "precompiled_connectivity.hxx"

#include "MNSProfileDiscover.hxx"
#include "MNSProfile.hxx"

#include "pratom.h"
#include "prmem.h"
#include "plstr.h"
#include "prenv.h"

#include "nsIEnumerator.h"
#include "prprf.h"
#include "nsCOMPtr.h"
#include "nsIComponentManager.h"
#include "nsEscape.h"
#include "nsDirectoryServiceDefs.h"
#include "nsAppDirectoryServiceDefs.h"
#include "nsILocalFile.h"
#include "nsReadableUtils.h"


#if defined(XP_MAC) || defined(XP_MACOSX)
#include <Processes.h>
#include <CFBundle.h>
#include "nsILocalFileMac.h"
#endif

#ifdef XP_UNIX
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "prnetdb.h"
#include "prsystem.h"
#endif

#ifdef VMS
#include <rmsdef.h>
#endif

#include "nsICharsetConverterManager.h"
#include "nsIPlatformCharset.h"


#if defined (XP_UNIX)
#define USER_ENVIRONMENT_VARIABLE "USER"
#define LOGNAME_ENVIRONMENT_VARIABLE "LOGNAME"
#define HOME_ENVIRONMENT_VARIABLE "HOME"
#define PROFILE_NAME_ENVIRONMENT_VARIABLE "PROFILE_NAME"
#define PROFILE_HOME_ENVIRONMENT_VARIABLE "PROFILE_HOME"
#define DEFAULT_UNIX_PROFILE_NAME "default"
#ifndef XP_MACOSX   /* Don't use symlink-based locking on OS X */
#define USE_SYMLINK_LOCKING
#endif
#elif defined (XP_BEOS)
#endif

// IID and CIDs of all the services needed
static NS_DEFINE_CID(kCharsetConverterManagerCID, NS_ICHARSETCONVERTERMANAGER_CID);

// Registry Keys

static ::rtl::OUString szProfileSubtreeString=::rtl::OUString::createFromAscii("Profiles");
static ::rtl::OUString szCurrentProfileString= ::rtl::OUString::createFromAscii("CurrentProfile");
static ::rtl::OUString szDirectoryString =::rtl::OUString::createFromAscii("directory");

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif
#ifndef _MNSFOLDERS_HXX_
#include <MNSFolders.hxx>
#endif
#include <MNSINIParser.hxx>

namespace connectivity
{
    namespace mozab
    {
        ProfileStruct::ProfileStruct(MozillaProductType aProduct,::rtl::OUString aProfileName,nsILocalFile * aProfilePath)
        {
            product=aProduct;
            profileName = aProfileName;
            profilePath = aProfilePath;
        }
        ::rtl::OUString ProfileStruct::getProfilePath()
        {
            if (profilePath)
            {
                nsAutoString path;
                nsresult rv = profilePath->GetPath(path);
                NS_ENSURE_SUCCESS(rv, ::rtl::OUString());
                return ::rtl::OUString(path.get());
            }
            else
                return ::rtl::OUString();
        }


        ProfileAccess::~ProfileAccess()
        {
        }
        ProfileAccess::ProfileAccess()
        {
            LoadProductsInfo();
        }
        sal_Int32 ProfileAccess::LoadProductsInfo()
        {
            //load mozilla profiles to m_ProductProfileList
            LoadMozillaProfiles();
            sal_Int32 index=MozillaProductType_Mozilla;
            sal_Int32 count=m_ProductProfileList[index].mProfileList.size();

            //load thunderbird profiles to m_ProductProfileList
            count += LoadXPToolkitProfiles(MozillaProductType_Thunderbird);
            //load firefox profiles to m_ProductProfileList
            //firefox profile does not containt address book, but maybe others need them
            count += LoadXPToolkitProfiles(MozillaProductType_Firefox);
            return count;
        }
        nsresult ProfileAccess::LoadMozillaProfiles()
        {
            sal_Int32 index=MozillaProductType_Mozilla;
            ProductStruct &m_Product = m_ProductProfileList[index];
            nsresult rv = NS_OK;

            //step 1 : get mozilla registry file
            nsCOMPtr<nsILocalFile>  localFile;
            ::rtl::OUString regDir( getRegistryFileName( MozillaProductType_Mozilla ) );
            nsAutoString registryDir(regDir.getStr());
            rv = NS_NewLocalFile(registryDir, PR_TRUE,
                                getter_AddRefs(localFile));
            NS_ENSURE_SUCCESS(rv,rv);
            PRBool bExist;
            rv = localFile->Exists(&bExist);
            NS_ENSURE_SUCCESS(rv,rv);
            if (!bExist)
                return rv;
            nsCOMPtr<nsIRegistry> registry(do_CreateInstance(NS_REGISTRY_CONTRACTID, &rv));
            NS_ENSURE_SUCCESS(rv,rv);
            //step 2: open mozilla registry file
            rv = registry->Open(localFile);
            NS_ENSURE_SUCCESS(rv,rv);

            nsCOMPtr<nsIEnumerator> enumKeys;
            nsRegistryKey profilesTreeKey;

            //step 3:Enumerator it
            rv = registry->GetKey(nsIRegistry::Common,
                        szProfileSubtreeString.getStr(),
                        &profilesTreeKey);
            if (NS_FAILED(rv)) return rv;

            nsXPIDLString tmpCurrentProfile;

            // Get the current profile
            rv = registry->GetString(profilesTreeKey,
                            szCurrentProfileString.getStr(),
                            getter_Copies(tmpCurrentProfile));

            if (tmpCurrentProfile)
            {
                m_Product.setCurrentProfile ( NS_STATIC_CAST(const PRUnichar*, tmpCurrentProfile));
            }


            rv = registry->EnumerateSubtrees( profilesTreeKey, getter_AddRefs(enumKeys));
            NS_ENSURE_SUCCESS(rv,rv);

            rv = enumKeys->First();
            NS_ENSURE_SUCCESS(rv,rv);

            while (NS_OK != enumKeys->IsDone())
            {
                nsCOMPtr<nsISupports> base;

                rv = enumKeys->CurrentItem( getter_AddRefs(base) );
                NS_ENSURE_SUCCESS(rv,rv);
                rv = enumKeys->Next();
                NS_ENSURE_SUCCESS(rv,rv);

                // Get specific interface.
                nsCOMPtr <nsIRegistryNode> node;
                nsIID nodeIID = NS_IREGISTRYNODE_IID;

                rv = base->QueryInterface( nodeIID, getter_AddRefs(node));
                if (NS_FAILED(rv)) continue;

                // Get node name.
                nsXPIDLString profile;
                rv = node->GetName(getter_Copies(profile));
                if (NS_FAILED(rv)) continue;

                nsRegistryKey profKey;
                rv = node->GetKey(&profKey);
                if (NS_FAILED(rv)) continue;


                nsCOMPtr<nsILocalFile> tempLocal;

                nsXPIDLString regData;
                rv = registry->GetString(profKey,
                        szDirectoryString.getStr(),
                        getter_Copies(regData));
                if (NS_FAILED(rv)) continue;

#if defined(XP_MAC) || defined(XP_MACOSX)
                    rv = NS_NewNativeLocalFile(nsCString(), PR_TRUE, getter_AddRefs(tempLocal));
                if (NS_SUCCEEDED(rv))
                    rv = tempLocal->SetPersistentDescriptor(NS_LossyConvertUCS2toASCII(regData));
#else
                rv = NS_NewLocalFile(regData, PR_TRUE, getter_AddRefs(tempLocal));
#endif
                //Add found profile to profile lists
                if (NS_SUCCEEDED(rv) && tempLocal)
                {
                    ProfileStruct*  profileItem     = new ProfileStruct(MozillaProductType_Mozilla,NS_STATIC_CAST(const PRUnichar*, profile),tempLocal);
                    m_Product.mProfileList[profileItem->getProfileName()] = profileItem;
                }

            }
            return rv;
        }
        //Thunderbird and firefox profiles are saved in profiles.ini
        sal_Int32 ProfileAccess::LoadXPToolkitProfiles(MozillaProductType product)
        {
            sal_Int32 index=product;
            ProductStruct &m_Product = m_ProductProfileList[index];

            nsresult rv;
            ::rtl::OUString regDir = getRegistryDir(product);
            ::rtl::OUString profilesIni( regDir );
            profilesIni += ::rtl::OUString::createFromAscii( "profiles.ini" );
            IniParser parser( profilesIni );
            IniSectionMap &mAllSection = *(parser.getAllSection());

            IniSectionMap::iterator iBegin = mAllSection.begin();
            IniSectionMap::iterator iEnd = mAllSection.end();
            for(;iBegin != iEnd;iBegin++)
            {
                ini_Section *aSection = &(*iBegin).second;
                ::rtl::OUString profileName;
                ::rtl::OUString profilePath;
                ::rtl::OUString sIsRelative;
                ::rtl::OUString sIsDefault;

                for(NameValueList::iterator itor=aSection->lList.begin();
                    itor != aSection->lList.end();
                    itor++)
                {
                        struct ini_NameValue * aValue = &(*itor);
                        if (aValue->sName.equals(::rtl::OUString::createFromAscii("Name")))
                        {
                            profileName = aValue->sValue;
                        }
                        else if (aValue->sName.equals(::rtl::OUString::createFromAscii("IsRelative")))
                        {
                            sIsRelative = aValue->sValue;
                        }
                        else if (aValue->sName.equals(::rtl::OUString::createFromAscii("Path")))
                        {
                            profilePath = aValue->sValue;
                        }
                        else if (aValue->sName.equals(::rtl::OUString::createFromAscii("Default")))
                        {
                            sIsDefault = aValue->sValue;
                        }
                }
                if (profileName.getLength() != 0 || profilePath.getLength() != 0)
                {
                    sal_Int32 isRelative = 0;
                    if (sIsRelative.getLength() != 0)
                    {
                        isRelative = sIsRelative.toInt32();
                    }

                    nsCOMPtr<nsILocalFile> rootDir;
                    rv = NS_NewNativeLocalFile(EmptyCString(), PR_TRUE,
                                            getter_AddRefs(rootDir));
                    if (NS_FAILED(rv)) continue;

                    OString sPath = OUStringToOString(profilePath, RTL_TEXTENCODING_UTF8);
                    nsCAutoString filePath(sPath.getStr());

                    if (isRelative) {
                        nsAutoString registryDir( regDir.getStr() );
                        nsCOMPtr<nsILocalFile>     mAppData;
                        rv = NS_NewLocalFile(registryDir, PR_TRUE,
                                        getter_AddRefs(mAppData));
                        if (NS_FAILED(rv)) continue;
                        rv = rootDir->SetRelativeDescriptor(mAppData, filePath);
                    } else {
                        rv = rootDir->SetPersistentDescriptor(filePath);
                    }
                    if (NS_FAILED(rv)) continue;

                    ProfileStruct*  profileItem     = new ProfileStruct(product,profileName,rootDir);
                    m_Product.mProfileList[profileName] = profileItem;

                    sal_Int32 isDefault = 0;
                    if (sIsDefault.getLength() != 0)
                    {
                        isDefault = sIsDefault.toInt32();
                    }
                    if (isDefault)
                        m_Product.mCurrentProfileName = profileName;

                }

            }
            return m_Product.mProfileList.size();
        }
        ::rtl::OUString ProfileAccess::getProfilePath( ::com::sun::star::mozilla::MozillaProductType product, const ::rtl::OUString& profileName ) throw (::com::sun::star::uno::RuntimeException)
        {
            sal_Int32 index=product;
            ProductStruct &m_Product = m_ProductProfileList[index];
            if (!m_Product.mProfileList.size() || m_Product.mProfileList.find(profileName) == m_Product.mProfileList.end())
            {
                //Profile not found
                return ::rtl::OUString();
            }
            else
                return m_Product.mProfileList[profileName]->getProfilePath();
        }
        ::sal_Int32 ProfileAccess::getProfileCount( ::com::sun::star::mozilla::MozillaProductType product) throw (::com::sun::star::uno::RuntimeException)
        {
            sal_Int32 index=product;
            ProductStruct &m_Product = m_ProductProfileList[index];
            return m_Product.mProfileList.size();
        }
        ::sal_Int32 ProfileAccess::getProfileList( ::com::sun::star::mozilla::MozillaProductType product, ::com::sun::star::uno::Sequence< ::rtl::OUString >& list ) throw (::com::sun::star::uno::RuntimeException)
        {
            sal_Int32 index=product;
            ProductStruct &m_Product = m_ProductProfileList[index];
            list.realloc(m_Product.mProfileList.size());
            sal_Int32 i=0;
            for(ProfileList::iterator itor=m_Product.mProfileList.begin();
                itor != m_Product.mProfileList.end();
                itor++)
            {
                ProfileStruct * aProfile = (*itor).second;
                list[i] = aProfile->getProfileName();
                i++;
            }

            return m_Product.mProfileList.size();
        }
        ::rtl::OUString ProfileAccess::getDefaultProfile( ::com::sun::star::mozilla::MozillaProductType product ) throw (::com::sun::star::uno::RuntimeException)
        {
            sal_Int32 index=product;
            ProductStruct &m_Product = m_ProductProfileList[index];
            if (m_Product.mCurrentProfileName.getLength() != 0)
            {
                //default profile setted in mozilla registry
                return m_Product.mCurrentProfileName;
            }
            if (m_Product.mProfileList.size() == 0)
            {
                //there are not any profiles
                return ::rtl::OUString();
            }
            ProfileStruct * aProfile = (*m_Product.mProfileList.begin()).second;
            return aProfile->getProfileName();
        }
        nsresult ProfileAccess::isExistFileOrSymlink(nsILocalFile* aFile,PRBool *bExist)
        {
            nsresult rv;
            nsAutoString path;
            aFile->GetPath(path);
            rv = aFile->Exists(bExist);
            NS_ENSURE_SUCCESS(rv, rv);
            if (!*bExist)
            {
                rv = aFile->IsSymlink(bExist);
                NS_ENSURE_SUCCESS(rv, rv);
            }
            return rv;
        }
        nsresult ProfileAccess::isLockExist(nsILocalFile* aFile)
        {
#if defined (XP_MACOSX)
            NS_NAMED_LITERAL_STRING(LOCKFILE_NAME, ".parentlock");
            NS_NAMED_LITERAL_STRING(OLD_LOCKFILE_NAME, "parent.lock");
#elif defined (XP_UNIX)
            NS_ConvertASCIItoUTF16 OLD_LOCKFILE_NAME("lock");
            NS_ConvertASCIItoUTF16 LOCKFILE_NAME(".parentlock");
#else
            NS_NAMED_LITERAL_STRING(OLD_LOCKFILE_NAME, "parent.lock");
            NS_NAMED_LITERAL_STRING(LOCKFILE_NAME, "parent.lock");
#endif

            nsresult rv;

            PRBool isDir;
            rv = aFile->IsDirectory(&isDir);
            NS_ENSURE_SUCCESS(rv, rv);
            if (!isDir)
                return NS_ERROR_FILE_NOT_DIRECTORY;

            nsCOMPtr<nsILocalFile> lockFile;
            rv = aFile->Clone((nsIFile **)((void **)getter_AddRefs(lockFile)));
            NS_ENSURE_SUCCESS(rv, rv);

            rv = lockFile->Append(LOCKFILE_NAME);
            NS_ENSURE_SUCCESS(rv, rv);
            PRBool nExist=PR_FALSE;
            rv = isExistFileOrSymlink(lockFile,&nExist);
            NS_ENSURE_SUCCESS(rv, rv);
            if (!nExist) // Check OLD_LOCKFILE_NAME
            {
                nsCOMPtr<nsILocalFile> oldlockFile;
                rv = aFile->Clone((nsIFile **)((void **)getter_AddRefs(oldlockFile)));
                NS_ENSURE_SUCCESS(rv, rv);

                rv = oldlockFile->Append(OLD_LOCKFILE_NAME);
                NS_ENSURE_SUCCESS(rv, rv);
                rv = isExistFileOrSymlink(oldlockFile,&nExist);
                NS_ENSURE_SUCCESS(rv, rv);
            }
            return nExist;
        }


        ::sal_Bool ProfileAccess::isProfileLocked( ::com::sun::star::mozilla::MozillaProductType product, const ::rtl::OUString& profileName ) throw (::com::sun::star::uno::RuntimeException)
        {
            ::rtl::OUString path = getProfilePath(product,profileName);
            if (!path.getLength())
                return sal_True;
            ::rtl::OString sPath = ::rtl::OUStringToOString(path, RTL_TEXTENCODING_UTF8);
            nsCAutoString filePath(sPath.getStr());

            nsresult rv;
            nsCOMPtr<nsILocalFile>  localFile;
            rv = NS_NewNativeLocalFile(filePath, PR_TRUE,
                                getter_AddRefs(localFile));
            NS_ENSURE_SUCCESS(rv,sal_True);

            PRBool exists = PR_FALSE;
            rv = localFile->Exists(&exists);
            NS_ENSURE_SUCCESS(rv, sal_True);
            if (!exists)
                return sal_True;

            // If the profile is locked, we return true
            rv = isLockExist(localFile);
            if (rv)
                return sal_True;
            return sal_False;
        }
        ::sal_Bool ProfileAccess::getProfileExists( ::com::sun::star::mozilla::MozillaProductType product, const ::rtl::OUString& profileName ) throw (::com::sun::star::uno::RuntimeException)
        {
            sal_Int32 index=product;
            ProductStruct &m_Product = m_ProductProfileList[index];
            if (!m_Product.mProfileList.size() || m_Product.mProfileList.find(profileName) == m_Product.mProfileList.end())
            {
                return sal_False;
            }
            else
                return sal_True;
        }


    }
}
