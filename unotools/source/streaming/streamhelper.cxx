/*************************************************************************
 *
 *  $RCSfile: streamhelper.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: hr $ $Date: 2001-09-27 11:17:08 $
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
#include <unotools/streamhelper.hxx>
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

namespace utl
{

//------------------------------------------------------------------------------
void SAL_CALL OInputStreamHelper::acquire() throw ()
{
    InputStreamHelper_Base::acquire();
}

//------------------------------------------------------------------------------
void SAL_CALL OInputStreamHelper::release() throw ()
{
    InputStreamHelper_Base::release();
}

//------------------------------------------------------------------------------
sal_Int32 SAL_CALL OInputStreamHelper::readBytes(staruno::Sequence< sal_Int8 >& aData, sal_Int32 nBytesToRead)
    throw(stario::NotConnectedException, stario::BufferSizeExceededException, stario::IOException, staruno::RuntimeException)
{
    if (!m_xLockBytes.Is())
        throw stario::NotConnectedException(::rtl::OUString(), static_cast<staruno::XWeak*>(this));

    if (nBytesToRead < 0)
        throw stario::BufferSizeExceededException(::rtl::OUString(), static_cast<staruno::XWeak*>(this));

    ::osl::MutexGuard aGuard( m_aMutex );
    aData.realloc(nBytesToRead);

    sal_uInt32 nRead;
    ErrCode nError = m_xLockBytes->ReadAt(m_nActPos, (void*)aData.getArray(), nBytesToRead, &nRead);
    m_nActPos += nRead;

    if (nError != ERRCODE_NONE)
        throw stario::IOException(::rtl::OUString(), static_cast<staruno::XWeak*>(this));

    // adjust sequence if data read is lower than the desired data
    if (nRead < (sal_uInt32)nBytesToRead)
        aData.realloc( nRead );

    return nRead;
}

void SAL_CALL OInputStreamHelper::seek( sal_Int64 location ) throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    m_nActPos = location;
}

sal_Int64 SAL_CALL OInputStreamHelper::getPosition(  ) throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException)
{
    return m_nActPos;
}

sal_Int64 SAL_CALL OInputStreamHelper::getLength(  ) throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException)
{
    if (!m_xLockBytes.Is())
        return 0;

    ::osl::MutexGuard aGuard( m_aMutex );
    SvLockBytesStat aStat;
    m_xLockBytes->Stat( &aStat, SVSTATFLAG_DEFAULT );
    return aStat.nSize;
}

//------------------------------------------------------------------------------
sal_Int32 SAL_CALL OInputStreamHelper::readSomeBytes(staruno::Sequence< sal_Int8 >& aData,
                                                     sal_Int32 nMaxBytesToRead)
    throw (stario::NotConnectedException, stario::BufferSizeExceededException, stario::IOException, staruno::RuntimeException)
{
    // read all data desired
    return readBytes(aData, nMaxBytesToRead);
}

//------------------------------------------------------------------------------
void SAL_CALL OInputStreamHelper::skipBytes(sal_Int32 nBytesToSkip)
    throw (stario::NotConnectedException, stario::BufferSizeExceededException, stario::IOException, staruno::RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    if (!m_xLockBytes.Is())
        throw stario::NotConnectedException(::rtl::OUString(), static_cast<staruno::XWeak*>(this));

    if (nBytesToSkip < 0)
        throw stario::BufferSizeExceededException(::rtl::OUString(), static_cast<staruno::XWeak*>(this));

    m_nActPos += nBytesToSkip;
}

//------------------------------------------------------------------------------
sal_Int32 SAL_CALL OInputStreamHelper::available()
    throw (stario::NotConnectedException, stario::IOException, staruno::RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    if (!m_xLockBytes.Is())
        throw stario::NotConnectedException(::rtl::OUString(), static_cast<staruno::XWeak*>(this));

    return m_nAvailable;
}

//------------------------------------------------------------------------------
void SAL_CALL OInputStreamHelper::closeInput()
    throw (stario::NotConnectedException, stario::IOException, staruno::RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    if (!m_xLockBytes.Is())
        throw stario::NotConnectedException(::rtl::OUString(), static_cast<staruno::XWeak*>(this));

    m_xLockBytes = NULL;
}

/*************************************************************************/
//------------------------------------------------------------------------------
void SAL_CALL OOutputStreamHelper::acquire() throw ()
{
    OutputStreamHelper_Base::acquire();
}

//------------------------------------------------------------------------------
void SAL_CALL OOutputStreamHelper::release() throw ()
{
    OutputStreamHelper_Base::release();
}
// stario::XOutputStream
//------------------------------------------------------------------------------
void SAL_CALL OOutputStreamHelper::writeBytes(const staruno::Sequence< sal_Int8 >& aData)
    throw (stario::NotConnectedException, stario::BufferSizeExceededException, stario::IOException, staruno::RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    if (!m_xLockBytes.Is())
        throw stario::NotConnectedException(::rtl::OUString(), static_cast<staruno::XWeak*>(this));

    ULONG nWritten;
    ErrCode nError = m_xLockBytes->WriteAt( m_nActPos, aData.getConstArray(), aData.getLength(), &nWritten );
    m_nActPos += nWritten;

    if (nError != ERRCODE_NONE ||
        nWritten != aData.getLength())
    {
        throw stario::IOException(::rtl::OUString(),static_cast<staruno::XWeak*>(this));
    }
}

//------------------------------------------------------------------
void SAL_CALL OOutputStreamHelper::flush()
    throw (stario::NotConnectedException, stario::BufferSizeExceededException, stario::IOException, staruno::RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    if (!m_xLockBytes.Is())
        throw stario::NotConnectedException(::rtl::OUString(), static_cast<staruno::XWeak*>(this));

    ErrCode nError = m_xLockBytes->Flush();
    if (nError != ERRCODE_NONE)
        throw stario::IOException(::rtl::OUString(),static_cast<staruno::XWeak*>(this));
}

//------------------------------------------------------------------
void SAL_CALL OOutputStreamHelper::closeOutput(  )
    throw(stario::NotConnectedException, stario::BufferSizeExceededException, stario::IOException, staruno::RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    if (!m_xLockBytes.Is())
        throw stario::NotConnectedException(::rtl::OUString(), static_cast<staruno::XWeak*>(this));

    m_xLockBytes = NULL;
}

} // namespace utl


