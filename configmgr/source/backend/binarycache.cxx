/*************************************************************************
*
*  $RCSfile: binarycache.cxx,v $
*
*  $Revision: 1.2 $
*
*  last change: $Author: vg $ $Date: 2003-05-26 08:03:34 $
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
*  Copyright: 2002 by Sun Microsystems, Inc.
*
*  All Rights Reserved.
*
*  Contributor(s): _______________________________________
*
*
************************************************************************/

#include "binarycache.hxx"

#include "binaryreadhandler.hxx"
#include "binarywritehandler.hxx"

#include "mergedcomponentdata.hxx"

#ifndef _CONFIGMGR_FILEHELPER_HXX_
#include "filehelper.hxx"
#endif

#ifndef CONFIGMGR_TYPECONVERTER_HXX
#include "typeconverter.hxx"
#endif

#ifndef _CONFIGMGR_BOOTSTRAP_HXX
#include "bootstrap.hxx"
#endif

#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif
#ifndef _OSL_PROCESS_H_
#include <osl/process.h>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif // _RTL_USTRBUF_HXX_

#ifndef _RTL_LOGFILE_HXX_
#include <rtl/logfile.hxx>
#endif

#define RTL_LOGFILE_OU2A(rtlOUString)   (::rtl::OUStringToOString((rtlOUString), RTL_TEXTENCODING_ASCII_US).getStr())

namespace configmgr
{
    // -----------------------------------------------------------------------------
    namespace backend
    {

        using ::rtl::OUString;

        const OUString aSettingName(
                RTL_CONSTASCII_USTRINGPARAM( CONTEXT_ITEM_PREFIX_ "CacheUrl"));
        // ---------------------------------------------------------------------------------------
        static inline bool isValidFileURL (OUString const& _sFileURL)
        {
            using osl::File;

            OUString sSystemPath;
            return _sFileURL.getLength() && (File::E_None == File::getSystemPathFromFileURL(_sFileURL, sSystemPath));
        }
        // -----------------------------------------------------------------------------
        // ---------------------------------------------------------------------------------------
        static
        bool implEnsureAbsoluteURL(rtl::OUString & _rsURL) // also strips embedded dots etc.
        {
            using osl::File;
            if (!_rsURL.getLength())
                return false;

            OSL_ENSURE(isValidFileURL(_rsURL), "File URL is invalid");

            rtl::OUString sBasePath = _rsURL;
            OSL_VERIFY(osl_Process_E_None == osl_getProcessWorkingDir(&sBasePath.pData));

            rtl::OUString sAbsolute;
            if ( File::E_None == File::getAbsoluteFileURL(sBasePath, _rsURL, sAbsolute))
            {
                _rsURL = sAbsolute;
                return isValidFileURL(_rsURL);
            }
            else
            {
                OSL_ENSURE(!isValidFileURL(_rsURL), "Could not get absolute file URL for valid URL");
                return false;
            }
        }
        // ---------------------------------------------------------------------------------------
        static const sal_Unicode kComponentSeparator = '.' ;
        static const sal_Unicode kPathSeparator = '/' ;
        static const char kBinarySuffix[] = ".dat" ;

        OUString BinaryCache::getCacheFileURL(const OUString& aComponent) const
        {
            rtl::OUStringBuffer retCode (mBaseURL);
            retCode.append(kPathSeparator) ;
        //  retCode.append(aComponent.replace(kComponentSeparator, kPathSeparator)) ;
            retCode.append(aComponent) ;
            retCode.appendAscii(RTL_CONSTASCII_STRINGPARAM(kBinarySuffix));

            OUString aResult = retCode.makeStringAndClear() ;

            if (isValidFileURL(aResult))
            {
                return aResult;
            }
            else
            {
                OSL_ENSURE(false, "Component File URL is invalid");
                return OUString();
            }
        }
        // -----------------------------------------------------------------------------
        BinaryCache::BinaryCache(const uno::Reference<uno::XComponentContext>& xContext )
        : mBaseURL()
        , mOwnerEntity()
        , mbCacheEnabled(false)
        {

            //initialise the base URL
            ContextReader aReader(xContext);

            OUString sCacheUrl;
            if (!aReader.isAdminService())
            {
                mbCacheEnabled = (aReader.getBestContext()->getValueByName(aSettingName) >>= sCacheUrl)
                                        && implEnsureAbsoluteURL(sCacheUrl);
            }

            if (mbCacheEnabled)
            {
                mBaseURL = sCacheUrl;
                if (!FileHelper::dirExists(sCacheUrl))
                {
                    if (osl::File::RC errorCode = FileHelper::mkdirs(sCacheUrl))
                    {
#if (OSL_DEBUG_LEVEL > 0)
                        rtl::OString sURL = rtl::OUStringToOString(sCacheUrl,RTL_TEXTENCODING_ASCII_US);
                        rtl::OString sErr = rtl::OUStringToOString(FileHelper::createOSLErrorString(errorCode),RTL_TEXTENCODING_ASCII_US);
                        ::osl_trace("Configuration: Cannot create cache directory \"%s\". "
                                    "Error is %s [%d]",sURL.getStr(),sErr.getStr(),int(errorCode)) ;
#endif
                        mbCacheEnabled = false;
                    }
                }
            }
        }
        // -----------------------------------------------------------------------------

        void BinaryCache::setOwnerEntity(const OUString & aOwnerEntity)
        {
            OSL_PRECOND(mOwnerEntity.getLength() == 0, "Owner entity of cache already set");
            mOwnerEntity = aOwnerEntity;
        }
        // -----------------------------------------------------------------------------

        void BinaryCache::disableCache()
        {
            mbCacheEnabled = false;
        }
        // -----------------------------------------------------------------------------

        bool BinaryCache::isCacheEnabled(rtl::OUString const & aEntity) const
        {
            if (!mbCacheEnabled) return false;

            // default entity is empty
            if (aEntity.getLength() == 0) return true;

            return aEntity.equals(mOwnerEntity);
        }
        // -----------------------------------------------------------------------------

        bool BinaryCache::readComponentData(MergedComponentData & aComponentData,
                                MultiServiceFactory const & aFactory,
                                OUString const & aComponent,
                                OUString const & aEntity,
                                OUString const & aLocale,
                                const uno::Reference<backenduno::XLayer> * pLayers,
                                sal_Int32 nNumLayers,
                                bool bIncludeTemplates)
        {
            if (isCacheEnabled(aEntity))
            try
            {
                RTL_LOGFILE_CONTEXT_AUTHOR(aLog, "configmgr::backend::BinaryCache", "jb99855", "configmgr: BinaryCache::readComponentData() - enabled");
                BinaryReadHandler aCacheReader(getCacheFileURL(aComponent),aComponent,aFactory);

                if(aCacheReader.validateHeader(pLayers, nNumLayers, mOwnerEntity, aLocale ))
                {
                    RTL_LOGFILE_CONTEXT_AUTHOR(aLog1, "configmgr::backend::BinaryCache", "jb99855", "configmgr: BinaryCache::readComponentData() - cache hit");
                    aComponentData.setSchemaRoot( aCacheReader.readComponentTree() );
                    if (bIncludeTemplates)
                        aComponentData.setTemplatesTree( aCacheReader.readTemplatesTree() );
                    return true;
                }
            }
            catch (uno::Exception & e)
            {
                OSL_TRACE("Binary Cache read failed - exception: %s", rtl::OUStringToOString(e.Message,RTL_TEXTENCODING_ASCII_US).getStr());
            }
            return false;
        }
        // -----------------------------------------------------------------------------

        bool BinaryCache::writeComponentData(MergedComponentData const & aComponentData,
                                MultiServiceFactory const & aFactory,
                                OUString const & aComponent,
                                OUString const & aEntity,
                                OUString const & aLocale,
                                const uno::Reference<backenduno::XLayer> * pLayers,
                                sal_Int32 nNumLayers)
        {
            if (isCacheEnabled(aEntity))
            try
            {
                RTL_LOGFILE_CONTEXT_AUTHOR(aLog3, "configmgr::backend::BinaryCache", "jb99855", "configmgr: BinaryCache::writeComponentData() - enabled");
                BinaryWriteHandler aCacheWriter(getCacheFileURL(aComponent),aComponent, aFactory);

                //write data to cache
                if (aCacheWriter.generateHeader(pLayers, nNumLayers, mOwnerEntity, aLocale))
                {
                    aCacheWriter.writeComponentTree(aComponentData.getSchemaTree());
                    aCacheWriter.writeTemplatesTree(aComponentData.getTemplatesTree());
                    return true;
                }
            }
            catch (uno::Exception & e)
            {
                OSL_TRACE("Configuration: Cache write failed - exception: %s", rtl::OUStringToOString(e.Message,RTL_TEXTENCODING_ASCII_US).getStr());
            }
            return false;
        }
        // -----------------------------------------------------------------------------

    }
// -----------------------------------------------------------------------------
}