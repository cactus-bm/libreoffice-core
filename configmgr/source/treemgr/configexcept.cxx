/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: configexcept.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 04:27:04 $
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

#include "configexcept.hxx"
#include <osl/diagnose.h>

namespace configmgr
{
    namespace configuration
    {
//-----------------------------------------------------------------------------

        Exception::Exception()
        : m_sAsciiMessage()
        {
        }
        //---------------------------------------------------------------------
        Exception::Exception(char const* sAsciiMessage)
        : m_sAsciiMessage(sAsciiMessage)
        {
        }
        //---------------------------------------------------------------------
        Exception::Exception(rtl::OString const& sAsciiMessage)
        : m_sAsciiMessage(sAsciiMessage)
        {
        }
        //---------------------------------------------------------------------

        OUString Exception::message() const
        {

            return rtl::OStringToOUString( m_sAsciiMessage, RTL_TEXTENCODING_ASCII_US );
        }
        //---------------------------------------------------------------------
        char const* Exception::what() const
        {
            return m_sAsciiMessage.getLength() ? m_sAsciiMessage.getStr() : "FAILURE in CONFIGURATION: No description available";
        }
        //---------------------------------------------------------------------

        static const char c_sInvalidNamePre[] = "CONFIGURATION: Invalid Path or Name: ";
        static const char c_sInvalidName[] = "CONFIGURATION: <Invalid Path or Name>";
//-----------------------------------------------------------------------------

        InvalidName::InvalidName(OUString const& sName)
        : Exception( rtl::OString(RTL_CONSTASCII_STRINGPARAM(c_sInvalidName)) )
        , m_sName(sName)
        {
        }
        //---------------------------------------------------------------------

        InvalidName::InvalidName(OUString const& sName, char const* sAsciiDescription)
        : Exception( rtl::OString(RTL_CONSTASCII_STRINGPARAM(c_sInvalidName)) += sAsciiDescription )
        , m_sName( sName.concat(OUString::createFromAscii(sAsciiDescription)) )
        {
        }
        //---------------------------------------------------------------------

        OUString InvalidName::message() const
        {
            return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(c_sInvalidNamePre)).concat( m_sName );
        }
//-----------------------------------------------------------------------------

        static const char c_sViolation[] = "CONFIGURATION: Update Violates Constraint: ";
    //---------------------------------------------------------------------
        ConstraintViolation::ConstraintViolation()
        : Exception( rtl::OString(RTL_CONSTASCII_STRINGPARAM(c_sViolation)) )
        {
        }
        //---------------------------------------------------------------------

        ConstraintViolation::ConstraintViolation(char const* sConstraint)
        : Exception( rtl::OString(RTL_CONSTASCII_STRINGPARAM(c_sViolation)) += sConstraint)
        {
        }

//-----------------------------------------------------------------------------

        static const char c_sTypeMismatch[] = "CONFIGURATION: Data Types do not match: ";
    //---------------------------------------------------------------------
        OUString TypeMismatch::describe(OUString const& sFoundType, OUString const& sExpectedType)
        {
            OUString sRet = OUString(RTL_CONSTASCII_USTRINGPARAM("Found Type: '"));
            sRet += sFoundType;
            if (sExpectedType.getLength() != 0)
            {
                sRet += OUString(RTL_CONSTASCII_USTRINGPARAM("' - Expected Type: '"));
                sRet += sExpectedType;
                sRet += OUString(RTL_CONSTASCII_USTRINGPARAM("'"));
            }
            else
            {
                sRet += OUString(RTL_CONSTASCII_USTRINGPARAM("' is not valid in this context"));
            }
            return sRet;
        }
    //---------------------------------------------------------------------

        TypeMismatch::TypeMismatch()
        : Exception( rtl::OString(RTL_CONSTASCII_STRINGPARAM(c_sTypeMismatch)) )
        {
        }
    //---------------------------------------------------------------------

        TypeMismatch::TypeMismatch(OUString const& sType1, OUString const& sType2)
        : Exception( rtl::OString(RTL_CONSTASCII_STRINGPARAM(c_sTypeMismatch)) )
        , m_sTypes( describe(sType1,sType2) )
        {
        }
    //---------------------------------------------------------------------

        TypeMismatch::TypeMismatch(OUString const& sType1)
        : Exception( rtl::OString(RTL_CONSTASCII_STRINGPARAM(c_sTypeMismatch)) )
        , m_sTypes( describe(sType1,OUString()) )
        {
        }
    //---------------------------------------------------------------------
        TypeMismatch::TypeMismatch(OUString const& sType1, OUString const& sType2, char const* sAsciiDescription)
        : Exception( rtl::OString(RTL_CONSTASCII_STRINGPARAM(c_sTypeMismatch)) += sAsciiDescription)
        , m_sTypes( describe(sType1,sType2).concat(OUString::createFromAscii(sAsciiDescription)) )
        {
        }
     //---------------------------------------------------------------------

        OUString TypeMismatch::message() const
        {
            return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(c_sTypeMismatch)).concat( m_sTypes );
        }
//-----------------------------------------------------------------------------

        WrappedUnoException::WrappedUnoException(uno::Any const& aUnoException)
        : Exception("UNO Service failed")
        , m_aUnoException(aUnoException)
        {
            OSL_ENSURE( m_aUnoException.getValueTypeClass() == uno::TypeClass_EXCEPTION,
                        "ERROR: Wrapping a non-exception into a WrappedUnoException !");

        }
        //---------------------------------------------------------------------

        OUString WrappedUnoException::extractMessage() const
        {
            return extractUnoException().Message;
        }
        //---------------------------------------------------------------------

        uno::Exception WrappedUnoException::extractUnoException() const
        {
            uno::Exception e;
            OSL_VERIFY( m_aUnoException >>= e );
            return e;
        }
        //---------------------------------------------------------------------

        uno::Any const& WrappedUnoException::getAnyUnoException() const
        {
            return m_aUnoException;
        }
        //---------------------------------------------------------------------

        OUString WrappedUnoException::message() const
        {
            return OUString(RTL_CONSTASCII_USTRINGPARAM("CONFIGMGR: Used Service failed with a"))
                    += m_aUnoException.getValueType().getTypeName()
                    += OUString(RTL_CONSTASCII_USTRINGPARAM(": "))
                    += extractMessage();
        }
//-----------------------------------------------------------------------------
    }

    namespace configapi
    {
//-----------------------------------------------------------------------------
        ExceptionMapper::ExceptionMapper(configuration::Exception& e)
        : m_eOriginal(e)
        , m_xContext()
        , m_sMessage(e.message())
        {
        }
        //---------------------------------------------------------------------

        ExceptionMapper::~ExceptionMapper()
        {
        }
        //---------------------------------------------------------------------

        void ExceptionMapper::setContext(uno::XInterface* pContext)
        {
            m_xContext = pContext;
        }
        //---------------------------------------------------------------------

        OUString ExceptionMapper::message() const
        {
            return OUString(RTL_CONSTASCII_USTRINGPARAM("FAILURE in CONFIGMGR: ")).concat( m_sMessage );
        }
        //---------------------------------------------------------------------

        uno::Reference<uno::XInterface> ExceptionMapper::context() const
        {
            return m_xContext;
        }
        //---------------------------------------------------------------------

        void ExceptionMapper::illegalArgument(sal_Int16 nArgument) throw(lang::IllegalArgumentException)
        {
            throw lang::IllegalArgumentException(message(),context(),nArgument);
        }
        //---------------------------------------------------------------------

        void ExceptionMapper::unhandled() throw(uno::RuntimeException)
        {
            throw uno::RuntimeException(message(),context());
        }
//-----------------------------------------------------------------------------
    }
}
