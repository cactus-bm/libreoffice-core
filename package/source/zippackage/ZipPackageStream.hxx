/*************************************************************************
 *
 *  $RCSfile: ZipPackageStream.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: mtg $ $Date: 2001-04-27 14:56:07 $
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
#ifndef _ZIP_PACKAGE_STREAM_HXX
#define _ZIP_PACKAGE_STREAM_HXX

#ifndef _COM_SUN_STAR_IO_XACTIVEDATASINK_HPP_
#include <com/sun/star/io/XActiveDataSink.hpp>
#endif
#ifndef _COM_SUN_STAR_PACKAGES_ZIPENTRY_HPP_
#include <com/sun/star/packages/ZipEntry.hpp>
#endif
#ifndef _CPPUHELPER_TYPEPROVIDER_HXX_
#include <cppuhelper/typeprovider.hxx>
#endif
#ifndef _ZIP_PACKAGE_ENTRY_HXX
#include <ZipPackageEntry.hxx>
#endif
#ifndef _VOS_REF_H_
#include <vos/ref.hxx>
#endif
#ifndef _ENCRYPTION_DATA_HXX_
#include <EncryptionData.hxx>
#endif

class ZipPackage;
class ZipPackageStream : public ZipPackageEntry,
                         public ::cppu::OWeakObject,
                         public ::com::sun::star::io::XActiveDataSink
{
protected:
    com::sun::star::uno::Reference < com::sun::star::io::XInputStream > xStream;
    ZipPackage          &rZipPackage;
    sal_Bool            bToBeCompressed, bToBeEncrypted, bPackageMember;
    vos::ORef < EncryptionData > xEncryptionData;
public:
    inline sal_Bool IsToBeCompressed () { return bToBeCompressed;}
    inline sal_Bool IsToBeEncrypted ()  { return bToBeEncrypted;}
    inline sal_Bool IsPackageMember ()  { return bPackageMember;}
    const com::sun::star::uno::Sequence < sal_Int8 >& getEncryptionKey ();
    const vos::ORef < EncryptionData > & getEncryptionData ()
    { return xEncryptionData;}
    const com::sun::star::uno::Sequence < sal_Int8 >& getInitialisationVector ()
    { return xEncryptionData->aInitVector;}
    const com::sun::star::uno::Sequence < sal_Int8 >& getSalt ()
    { return xEncryptionData->aSalt;}
    const sal_Int64 getIterationCount ()
    { return xEncryptionData->nIterationCount;}

    inline void SetToBeCompressed (sal_Bool bNewValue) { bToBeCompressed = bNewValue;}
    inline void SetToBeEncrypted (sal_Bool bNewValue)  { bToBeEncrypted  = bNewValue;}
    inline void SetPackageMember (sal_Bool bNewValue)  { bPackageMember  = bNewValue;}
    inline void setInitialisationVector (const com::sun::star::uno::Sequence < sal_Int8 >& rNewVector )
    { xEncryptionData->aInitVector = rNewVector;}
    inline void setSalt (const com::sun::star::uno::Sequence < sal_Int8 >& rNewSalt )
    { xEncryptionData->aSalt = rNewSalt;}
    inline void setIterationCount (const sal_Int64 nNewCount)
    { xEncryptionData->nIterationCount = nNewCount;}

    ZipPackageStream (ZipPackage & rNewPackage);
    virtual ~ZipPackageStream( void );

    void setZipEntry( const com::sun::star::packages::ZipEntry &rInEntry);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getRawStream( com::sun::star::packages::ZipEntry &rEntry  )
        throw(::com::sun::star::uno::RuntimeException);

    // XInterface
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& rType )
        throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL acquire(  )
        throw();
    virtual void SAL_CALL release(  )
        throw();

    // XActiveDataSink
    virtual void SAL_CALL setInputStream( const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& aStream )
        throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getInputStream(  )
        throw(::com::sun::star::uno::RuntimeException);

    // XUnoTunnel
    static ::com::sun::star::uno::Sequence < sal_Int8 > getUnoTunnelImplementationId( void )
        throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier )
        throw(::com::sun::star::uno::RuntimeException);

    // XPropertySet
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue )
        throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName )
        throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
};
#endif
