/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: odma_inputstream.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 15:52:08 $
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

#ifndef ODMA_INPUTSTREAM_HXX
#define ODMA_INPUTSTREAM_HXX

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef _COM_SUN_STAR_IO_XINPUTSTREAM_HPP_
#include <com/sun/star/io/XInputStream.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XSTREAM_HPP_
#include <com/sun/star/io/XStream.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XOUTPUTSTREAM_HPP_
#include <com/sun/star/io/XOutputStream.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XTRUNCATE_HPP_
#include <com/sun/star/io/XTruncate.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XSEEKABLE_HPP_
#include <com/sun/star/io/XSeekable.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE5_HXX_
#include <cppuhelper/implbase5.hxx>
#endif
#ifndef _VOS_REF_HXX_
#include <vos/ref.hxx>
#endif

namespace ucb
{
    class Content;
}
namespace odma
{
    typedef ::cppu::WeakImplHelper5<    ::com::sun::star::io::XInputStream,
                                        ::com::sun::star::io::XStream,
                                        ::com::sun::star::io::XTruncate,
                                        ::com::sun::star::io::XSeekable,
                                        ::com::sun::star::io::XOutputStream> OOdmaStreamBase;

    class ContentProvider;
    class ContentProperties;
    class OOdmaStream : public OOdmaStreamBase
    {
        ::osl::Mutex    m_aMutex;
        ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream>  m_xOutput;
        ::com::sun::star::uno::Reference< ::com::sun::star::io::XTruncate>      m_xTruncate;
        ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream>   m_xInput;
        ::com::sun::star::uno::Reference< ::com::sun::star::io::XSeekable>      m_xInputSeek;

        ::vos::ORef<ContentProperties>  m_aProp;
        ::ucb::Content*                 m_pContent;
        ContentProvider*                m_pProvider;
        sal_Bool                        m_bInputStreamCalled;
        sal_Bool                        m_bOutputStreamCalled;
        sal_Bool                        m_bModified;

        void ensureInputStream()  throw( ::com::sun::star::io::IOException );
        void ensureOutputStream()  throw( ::com::sun::star::io::IOException );
        void SAL_CALL closeStream() throw(  ::com::sun::star::io::NotConnectedException,
                                            ::com::sun::star::io::IOException,
                                            ::com::sun::star::uno::RuntimeException );
    public:
        OOdmaStream(::ucb::Content* _pContent,
                    ContentProvider* _pProvider,
                    const ::vos::ORef<ContentProperties>& _rProp);
        virtual ~OOdmaStream();
        // com::sun::star::io::XInputStream
        virtual sal_Int32 SAL_CALL readBytes( ::com::sun::star::uno::Sequence<sal_Int8>& aData, sal_Int32 nBytesToRead )
            throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException,
                  ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

        virtual sal_Int32 SAL_CALL readSomeBytes( ::com::sun::star::uno::Sequence<sal_Int8>& aData, sal_Int32 nMaxBytesToRead )
            throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException,
                  ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL skipBytes( sal_Int32 nBytesToSkip )
            throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException,
                  ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

        virtual sal_Int32 SAL_CALL available(  )
            throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL closeInput(  )
            throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

        // com::sun::star::io::XStream
        virtual com::sun::star::uno::Reference< com::sun::star::io::XInputStream > SAL_CALL getInputStream(  ) throw( com::sun::star::uno::RuntimeException );
        virtual com::sun::star::uno::Reference< com::sun::star::io::XOutputStream > SAL_CALL getOutputStream(  ) throw( com::sun::star::uno::RuntimeException );

        // com::sun::star::io::XOutputStream
        void SAL_CALL writeBytes( const com::sun::star::uno::Sequence< sal_Int8 >& aData )
                    throw( com::sun::star::io::NotConnectedException,
                           com::sun::star::io::BufferSizeExceededException,
                           com::sun::star::io::IOException,
                           com::sun::star::uno::RuntimeException);

        void SAL_CALL flush()
                    throw( com::sun::star::io::NotConnectedException,
                           com::sun::star::io::BufferSizeExceededException,
                           com::sun::star::io::IOException,
                           com::sun::star::uno::RuntimeException);
        void SAL_CALL closeOutput()
                    throw( com::sun::star::io::NotConnectedException,
                           com::sun::star::io::IOException,
                           com::sun::star::uno::RuntimeException );
        // XTruncate
        virtual void SAL_CALL truncate( void )
            throw( com::sun::star::io::IOException,
                   com::sun::star::uno::RuntimeException );
        // XSeekable
        void SAL_CALL seek(sal_Int64 location )
            throw( com::sun::star::lang::IllegalArgumentException,
                   com::sun::star::io::IOException,
                   com::sun::star::uno::RuntimeException );

        sal_Int64 SAL_CALL getPosition()
            throw( com::sun::star::io::IOException,
                   com::sun::star::uno::RuntimeException );

        sal_Int64 SAL_CALL getLength()
            throw( com::sun::star::io::IOException,
                   com::sun::star::uno::RuntimeException );
    };
}
#endif // ODMA_INPUTSTREAM_HXX
