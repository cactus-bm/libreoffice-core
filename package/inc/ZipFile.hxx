/*************************************************************************
 *
 *  $RCSfile: ZipFile.hxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: mtg $ $Date: 2001-09-14 14:45:19 $
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
 *  Contributor(s): Martin Gallwey (gallwey@sun.com)
 *
 *
 ************************************************************************/
#ifndef _ZIP_FILE_HXX
#define _ZIP_FILE_HXX

#ifndef _BYTE_GRABBER_HXX_
#include <ByteGrabber.hxx>
#endif
#ifndef _HASHMAPS_HXX
#include <HashMaps.hxx>
#endif
#ifndef _INFLATER_HXX
#include <Inflater.hxx>
#endif
#ifndef _COM_SUN_STAR_PACKAGES_ZIP_ZIPEXCEPTION_HPP_
#include <com/sun/star/packages/zip/ZipException.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_NOSUCHELEMENTEXCEPTION_HPP_
#include <com/sun/star/container/NoSuchElementException.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_WRAPPEDTARGETEXCEPTION_HPP_
#include <com/sun/star/lang/WrappedTargetException.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _VOS_REF_H_
#include <vos/ref.hxx>
#endif
/*
 * We impose arbitrary but reasonable limit on ZIP files.
 */

#define ZIP_MAXNAMELEN 512
#define ZIP_MAXEXTRA 256
#define ZIP_MAXENTRIES (0x10000 - 2)

typedef void* rtlCipher;
class ZipEnumeration;
class EncryptionData;

class ZipFile
{
protected:
    ::rtl::OUString sName;          /* zip file name */
    ::rtl::OUString sComment;       /* zip file comment */
    EntryHash       aEntries;
    ByteGrabber     aGrabber;
    Inflater        aInflater;
    com::sun::star::uno::Reference < com::sun::star::io::XInputStream > xStream;
    com::sun::star::uno::Reference < com::sun::star::io::XSeekable > xSeek;
    const ::com::sun::star::uno::Reference < com::sun::star::lang::XMultiServiceFactory > xFactory;

    com::sun::star::uno::Reference < com::sun::star::io::XInputStream >  createMemoryStream(
            ZipEntry & rEntry,
            const vos::ORef < EncryptionData > &rData,
            sal_Bool bRawStream );

    com::sun::star::uno::Reference < com::sun::star::io::XInputStream >  createFileStream(
            ZipEntry & rEntry,
            const vos::ORef < EncryptionData > &rData,
            sal_Bool bRawStream );
public:
    ZipFile( com::sun::star::uno::Reference < com::sun::star::io::XInputStream > &xInput,
             const com::sun::star::uno::Reference < com::sun::star::lang::XMultiServiceFactory > &xNewFactory,
             sal_Bool bInitialise)
        throw(::com::sun::star::io::IOException, com::sun::star::packages::zip::ZipException, com::sun::star::uno::RuntimeException);

    ~ZipFile();

    void setInputStream ( com::sun::star::uno::Reference < com::sun::star::io::XInputStream > xNewStream );
    ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getRawStream(
            ZipEntry& rEntry,
            const vos::ORef < EncryptionData > &rData)
        throw(::com::sun::star::io::IOException, ::com::sun::star::packages::zip::ZipException, ::com::sun::star::uno::RuntimeException);

    static void StaticGetCipher ( const vos::ORef < EncryptionData > & xEncryptionData, rtlCipher &rCipher );

    ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getInputStream(
            ZipEntry& rEntry,
            const vos::ORef < EncryptionData > &rData)
        throw(::com::sun::star::io::IOException, ::com::sun::star::packages::zip::ZipException, ::com::sun::star::uno::RuntimeException);
    ::rtl::OUString SAL_CALL getName(  )
        throw(::com::sun::star::uno::RuntimeException);
    sal_Int32 SAL_CALL getSize(  )
        throw(::com::sun::star::uno::RuntimeException);

    ZipEnumeration * SAL_CALL entries(  );
protected:
    sal_Bool        readLOC ( ZipEntry &rEntry)
        throw(::com::sun::star::io::IOException, com::sun::star::packages::zip::ZipException, com::sun::star::uno::RuntimeException);
    sal_Int32       readCEN()
        throw(::com::sun::star::io::IOException, com::sun::star::packages::zip::ZipException, com::sun::star::uno::RuntimeException);
    sal_Int32       findEND()
        throw(::com::sun::star::io::IOException, com::sun::star::packages::zip::ZipException, com::sun::star::uno::RuntimeException);
};

#endif
