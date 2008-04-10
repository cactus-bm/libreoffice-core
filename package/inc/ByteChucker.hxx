/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ByteChucker.hxx,v $
 * $Revision: 1.13 $
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
#ifndef _BYTE_CHUCKER_HXX_
#define _BYTE_CHUCKER_HXX_

#include <com/sun/star/uno/Sequence.h>
#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/io/BufferSizeExceededException.hpp>
#include <com/sun/star/io/IOException.hpp>
#include <com/sun/star/io/NotConnectedException.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>

namespace com { namespace sun { namespace star {
    namespace io { class XSeekable; class XOutputStream; }
} } }
class ByteChucker
{
protected:
    com::sun::star::uno::Reference < com::sun::star::io::XOutputStream > xStream;
    com::sun::star::uno::Reference < com::sun::star::io::XSeekable > xSeek;
    com::sun::star::uno::Sequence < sal_Int8 > a1Sequence, a2Sequence, a4Sequence;
    sal_Int8 * const p1Sequence, * const p2Sequence, * const p4Sequence;

public:
    ByteChucker (com::sun::star::uno::Reference<com::sun::star::io::XOutputStream> xOstream);
    ~ByteChucker();

    // XOutputStream
    void SAL_CALL writeBytes( const ::com::sun::star::uno::Sequence< sal_Int8 >& aData, sal_Int32 nLength = -1, const sal_Int8 * const pData = NULL)
        throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    void SAL_CALL flush(  )
        throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    void SAL_CALL closeOutput(  )
        throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    // XSeekable
    sal_Int64 SAL_CALL seek( sal_Int64 location )
        throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    sal_Int64 SAL_CALL getPosition(  )
        throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    sal_Int64 SAL_CALL getLength(  )
        throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

    ByteChucker& operator << (sal_Int8 nInt8);
    ByteChucker& operator << (sal_Int16 nInt16);
    ByteChucker& operator << (sal_Int32 nInt32);
    ByteChucker& operator << (sal_uInt8 nuInt8);
    ByteChucker& operator << (sal_uInt16 nuInt16);
    ByteChucker& operator << (sal_uInt32 nuInt32);
};

#endif
