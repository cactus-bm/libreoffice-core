/*************************************************************************
 *
 *  $RCSfile: filehelper.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: lla $ $Date: 2001-05-14 12:06:26 $
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


#ifndef _RTL_USTRING_H_
#include <rtl/ustring.hxx>
#endif

#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif

#ifndef _CONFIGMGR_FILEHELPER_HXX_
#include "filehelper.hxx"
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

namespace configmgr
{
    using namespace ::osl;
    namespace io = com::sun::star::io;

#define ASCII(x) rtl::OUString::createFromAscii(x)
// -----------------------------------------------------------------------------
    rtl::OUString FileHelper::convertFilenameToFileURL(rtl::OUString const& _sFilename)
    {
        rtl::OUString sURL;
#ifdef TF_FILEURL
        osl::FileBase::getFileURLFromSystemPath( _sFilename, sURL );
#else
        osl::File aConvert(ASCII(""));
        aConvert.normalizePath(_sFilename, sURL);
#endif
        return sURL;
    }

// ---------------------------- tryToRemoveFile ----------------------------
void FileHelper::tryToRemoveFile(const rtl::OUString& _aURL) throw (io::IOException)
    {
        {
            File aFile(_aURL);
            FileBase::RC eError = aFile.open(OpenFlag_Read);
            if (eError != osl_File_E_None)
            {
                // IF not exists
                return;
            }
            aFile.close();
        }
        FileBase::RC eError = File::remove(_aURL);
        if (eError != osl_File_E_None)
        {
            rtl::OUString sError = ASCII("tryToRemoveFile: ");
            sError += FileHelper::createOSLErrorString(eError);
            sError += ASCII("\n with URL: ");
            sError += _aURL;
#ifdef DEBUG
            rtl::OString aStr = rtl::OUStringToOString(sError,RTL_TEXTENCODING_ASCII_US);
            OSL_ENSURE(0, aStr.getStr());
#endif
            throw io::IOException(sError, NULL);
            // return 1;
        }
    }

// -----------------------------------------------------------------------------
    void FileHelper::createBackupRemoveAndRename(
        const rtl::OUString& _aFromURL, const rtl::OUString &_aToURL) throw (io::IOException)
    {
        // remove FromURL
        // rename ToURL to FromURL
        FileHelper::tryToRemoveFile(_aFromURL);

        FileBase::RC eError = File::move(_aToURL, _aFromURL);
        if (eError != osl_File_E_None)
        {
            rtl::OUString sError = ASCII("createBackupAndRemove: ") + FileHelper::createOSLErrorString(eError) + ASCII("\n with URL: ") + _aFromURL;
#ifdef DEBUG
            rtl::OString aStr = rtl::OUStringToOString(sError,RTL_TEXTENCODING_ASCII_US);
            OSL_ENSURE(0, aStr.getStr());
#endif
            throw io::IOException(sError, NULL);
        }
    }
// -----------------------------------------------------------------------------
    bool FileHelper::fileExist(rtl::OUString const& _aFileURL)
    {
        DirectoryItem aItem;
        FileBase::RC eError = DirectoryItem::get(_aFileURL, aItem);
        if (eError != osl_File_E_None)
        {
            // IF not exists
            return false;
        }
        return true;
    }

    // -----------------------------------------------------------------------------
    bool FileHelper::directoryExist(rtl::OUString const& _aDirexURL)
    {
        bool bDirectoryExist;
        Directory aDirex(_aDirexURL);
        FileBase::RC eError = aDirex.open();
        if (eError != osl_File_E_None)
        {
            bDirectoryExist = false;
        }
        else
        {
            bDirectoryExist = true;
        }

        aDirex.close();
        return bDirectoryExist;
    }

// -----------------------------------------------------------------------------
    rtl::OUString FileHelper::splitDirectoryOff(rtl::OUString const& _sFilename)
    {
        // split off the directory
        // there is no test, if last string after '/' is a directory, it will cut off

        // evtl. should a test like: check if the last string contain a dot '.'
        // help to test it.

        sal_Int32 nIdx = _sFilename.lastIndexOf(FileHelper::getFileDelimiter(), _sFilename.getLength());
        rtl::OUString sDirex = _sFilename.copy(0,nIdx);
        return sDirex;
    }
// -----------------------------------------------------------------------------
// return a TimeValue at which time the given file is modified

    TimeValue FileHelper::getFileModificationStamp(rtl::OUString const& _aNormalizedFilename) throw (io::IOException)
    {
        TimeValue aTime = {0,0};

        DirectoryItem aItem;
        osl::FileBase::RC eError = DirectoryItem::get(_aNormalizedFilename, aItem);
        if (eError != osl::FileBase::E_None)
        {
            rtl::OUString aUStr = FileHelper::createOSLErrorString(eError);
            throw io::IOException(aUStr, NULL);
        }

        FileStatus aStatus(osl_FileStatus_Mask_ModifyTime|osl_FileStatus_Mask_Type);
        eError = aItem.getFileStatus(aStatus);
        if (eError != osl::FileBase::E_None)
        {
            return aTime;
            rtl::OUString aUStr = FileHelper::createOSLErrorString(eError);
            throw io::IOException(aUStr, NULL);
        }
        if (aStatus.isValid(osl_FileStatus_Mask_Type))
        {
            FileStatus::Type eType = aStatus.getFileType();
            volatile int dummy = 0;
        }

        if (aStatus.isValid(osl_FileStatus_Mask_ModifyTime))
        {
            aTime = aStatus.getModifyTime();
        }
        return aTime;
    }

// ------------------ Create a string from FileBase::RC Error ------------------
    rtl::OUString FileHelper::createOSLErrorString(FileBase::RC eError)
    {
        rtl::OUString aRet;
        switch(eError)
        {
        case osl_File_E_None:
            aRet = ASCII("");
            break;

        case osl_File_E_PERM:
            aRet = ASCII("Operation not permitted");
            break;

        case osl_File_E_NOENT:
            aRet = ASCII("No such file or directory");
            break;

        case osl_File_E_SRCH:
            aRet = ASCII("unknown error: osl_File_E_SRCH");
            break;

        case osl_File_E_INTR:
            aRet = ASCII("function call was interrupted");
            break;

        case osl_File_E_IO:
            aRet = ASCII("I/O error");
            break;

        case osl_File_E_NXIO:
            aRet = ASCII("No such device or address");
            break;

        case osl_File_E_2BIG:
            aRet = ASCII("unknown error: osl_File_E_2BIG");
            break;

        case osl_File_E_NOEXEC:
            aRet = ASCII("unknown error: osl_File_E_NOEXEC");
            break;

        case osl_File_E_BADF:
            aRet = ASCII("Bad file");
            break;

        case osl_File_E_CHILD:
            aRet = ASCII("unknown error: osl_File_E_CHILD");
            break;

        case osl_File_E_AGAIN:
            aRet = ASCII("Operation would block");
            break;

        case osl_File_E_NOMEM:
            aRet = ASCII("not enough memory for allocating structures");
            break;

        case osl_File_E_ACCES:
            aRet = ASCII("Permission denied");
            break;

        case osl_File_E_FAULT:
            aRet = ASCII("Bad address");
            break;

        case osl_File_E_BUSY:
            aRet = ASCII("Text file busy");
            break;

        case osl_File_E_EXIST:
            aRet = ASCII("File exists");
            break;

        case osl_File_E_XDEV:
            aRet = ASCII("unknown error: osl_File_E_XDEV");
            break;

        case osl_File_E_NODEV:
            aRet = ASCII("No such device");
            break;

        case osl_File_E_NOTDIR:
            aRet = ASCII("Not a directory");
            break;

        case osl_File_E_ISDIR:
            aRet = ASCII("Is a director");
            break;

        case osl_File_E_INVAL:
            aRet = ASCII("the format of the parameters was not valid");
            break;

        case osl_File_E_NFILE:
            aRet = ASCII("too many open files in the system");
            break;

        case osl_File_E_MFILE:
            aRet = ASCII("too many open files used by the process");
            break;

        case osl_File_E_NOTTY:
            aRet = ASCII("unknown error: osl_File_E_NOTTY");
            break;

        case osl_File_E_FBIG:
            aRet = ASCII("File too large");
            break;

        case osl_File_E_NOSPC:
            aRet = ASCII("No space left on device");
            break;

        case osl_File_E_SPIPE:
            aRet = ASCII("unknown error: osl_File_E_SPIPE");
            break;

        case osl_File_E_ROFS:
            aRet = ASCII("Read-only file system");
            break;

        case osl_File_E_MLINK:
            aRet = ASCII("Too many links");
            break;

        case osl_File_E_PIPE:
            aRet = ASCII("unknown error: osl_File_E_PIPE");
            break;

        case osl_File_E_DOM:
            aRet = ASCII("unknown error: osl_File_E_DOM");
            break;

        case osl_File_E_RANGE:
            aRet = ASCII("unknown error: osl_File_E_RANGE");
            break;

        case osl_File_E_DEADLK:
            aRet = ASCII("unknown error: osl_File_E_DEADLK");
            break;

        case osl_File_E_NAMETOOLONG:
            aRet = ASCII("File name too long");
            break;

        case osl_File_E_NOLCK:
            aRet = ASCII("No record locks available");
            break;

        case osl_File_E_NOSYS:
            aRet = ASCII("Function not implemente");
            break;

        case osl_File_E_NOTEMPTY:
            aRet = ASCII("Directory not empt");
            break;

        case osl_File_E_LOOP:
            aRet = ASCII("Too many symbolic links encountered");
            break;

        case osl_File_E_ILSEQ:
            aRet = ASCII("unknown error: osl_File_E_ILSEQ");
            break;

        case osl_File_E_NOLINK:
            aRet = ASCII("Link has been severed");
            break;

        case osl_File_E_MULTIHOP:
            aRet = ASCII("Multihop attempted");
            break;

        case osl_File_E_USERS:
            aRet = ASCII("unknown error: osl_File_E_USERS");
            break;

        case osl_File_E_OVERFLOW:
            aRet = ASCII("Value too large for defined data type");
            break;

            /* unmapped error: always last entry in enum! */
        case osl_File_E_invalidError:
            aRet = ASCII("unmapped Error");
            break;
        }
        return aRet;
    }
} // namespace configmgr
