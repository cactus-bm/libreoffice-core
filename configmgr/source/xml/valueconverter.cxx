/*************************************************************************
 *
 *  $RCSfile: valueconverter.cxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: jb $ $Date: 2002-05-16 11:00:29 $
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

#include "valueconverter.hxx"

#ifndef _CONFIGMGR_STRDECL_HXX_
#include "strdecl.hxx"
#endif
#ifndef CONFIGMGR_TYPECONVERTER_HXX
#include "typeconverter.hxx"
#endif

inline sal_Bool rtl_ascii_isWhitespace( sal_Unicode ch )
{
    return ch <= 0x20 && ch;
}

// #define ASCII(x) OUString::createFromAscii(x)
namespace configmgr
{
    using rtl::OUString;
    using namespace com::sun::star::uno;
    using namespace std;

// -----------------------------------------------------------------------------
static
void throwConversionError(sal_Char const* pErrorMsg) CFG_THROW1( script::CannotConvertException )
{
    OSL_ENSURE(false, pErrorMsg);

    script::CannotConvertException error;
    error.Message = OUString::createFromAscii(pErrorMsg);
    throw error;
}
// -----------------------------------------------------------------------------
template <class Char>
inline
bool charInRange(Char ch, char from, char to) throw()
{
    return Char(from) <= ch && ch <= Char(to);
}

// -----------------------------------------------------------------------------
static
inline
unsigned makeHexNibble(unsigned char ch) CFG_THROW1 ( script::CannotConvertException)
{
    unsigned nRet;

         if (charInRange(ch, '0', '9')) nRet = ch - unsigned('0');

    else if (charInRange(ch, 'a', 'f')) nRet = ch - unsigned('a' - 10u);

    else if (charInRange(ch, 'A', 'F')) nRet = ch - unsigned('A' - 10u);

    else throwConversionError("Invalid Hex Character in binary value");

    return nRet; // not reachable
}

// -----------------------------------------------------------------------------
static
inline
unsigned readHexNibble(sal_Unicode ch) CFG_THROW1 ( script::CannotConvertException)
{
    if (!charInRange(ch, 0, 127)) throwConversionError("Non-Ascii Character in binary value");

    return makeHexNibble(static_cast<unsigned char>(ch));
}

// -----------------------------------------------------------------------------
static
inline
unsigned int readHexByte(sal_Unicode const*& pStr) CFG_THROW1 ( script::CannotConvertException)
{
    register unsigned int nHigh = readHexNibble(*pStr++);
    register unsigned int nLow =  readHexNibble(*pStr++);
    return (nHigh << 4) | nLow;
}

// -----------------------------------------------------------------------------
static
void parseHexBinary(OUString const& aHexString_, uno::Sequence<sal_Int8>& rBinarySeq_)
        CFG_UNO_THROW1 ( script::CannotConvertException )
{
    // PRE: aBinaryString with HexCode
    // POST: rBinarySeq with the to Hex converted String

    sal_uInt32 nCount = aHexString_.getLength();
    sal_Unicode const * pHex = aHexString_.getStr();

    if (nCount % 2) throwConversionError("Hex string has odd number of characters");
    nCount /= 2;

    rBinarySeq_.realloc(nCount);
    sal_Int8 * pBinary = rBinarySeq_.getArray();

    while (nCount--)
    {
        *pBinary++ = readHexByte(pHex);
    }
}

// -----------------------------------------------------------------------------

namespace Encoding {
    enum Type { unknown, hex, base64, DEFAULT = hex };

    static
    inline
    Type parse(OUString const& sEncoding)
    {
        Type eEncoding;

        if (sEncoding.getLength() == 0)
        {
            eEncoding = Encoding::DEFAULT;
        }

        else if (sEncoding.equalsIgnoreAsciiCase(ENCODING_HEX))
        {
            eEncoding = Encoding::hex;
        }

        else if (sEncoding.equalsIgnoreAsciiCase(ENCODING_BASE64))
        {
            eEncoding = Encoding::base64;
        }

        else
        {
            eEncoding = Encoding::unknown;
        }

        return eEncoding;
    }
}

// -----------------------------------------------------------------------------
uno::Sequence<sal_Int8> OValueConverter::parseBinary(OUString const& aBinaryString_) const
        CFG_UNO_THROW1 ( script::CannotConvertException)
{
    uno::Sequence<sal_Int8> aResultSeq;

    switch (Encoding::parse(m_aValueDesc.sEncoding))
    {
    case Encoding::hex:
        parseHexBinary(aBinaryString_,aResultSeq);
        break;

    case Encoding::base64:
        OSL_ENSURE(false, "Base64 encoding for binary value is currently not supported");
        break;

    case Encoding::unknown:
        OSL_ENSURE(false, "Unknown encoding found for binary value");
        break;

    default:
        OSL_ASSERT(false);// Unreachable code
        break;
    }

    return aResultSeq;
}

// -----------------------------------------------------------------------------

uno::Type OValueConverter::getType() const
{
    if (m_aValueDesc.sType.getLength() == 0)
        return getVoidCppuType();

    return toType(m_aValueDesc.sType, m_aValueDesc.isList);
}
// -----------------------------------------------------------------------------
bool OValueConverter::convertToAny(OUString const& aContent, uno::Any& rValue) const
        CFG_UNO_THROW1( script::CannotConvertException)
{
    // PRE: filled content and ValueInfo and an existing Any Object
    // POST: Any contain the Data from the content

    if (m_aValueDesc.isNull)
    {
        OSL_ENSURE(aContent.trim().getLength() == 0, "OValueConverter: Non-empty Null Value - ignoring content");
        rValue.clear();
        return false;
    }

    else if (m_aValueDesc.isList)
    {
        StringList aContentList;
        splitListData(aContent, aContentList);
        return convertListToAny(aContentList, rValue);
    }

    else
    {
        return convertScalarToAny(aContent, rValue);
    }
}

// -----------------------------------------------------------------------------
bool OValueConverter::convertScalarToAny(OUString const& aContent, uno::Any& rValue) const
        CFG_UNO_THROW1 ( script::CannotConvertException )
{
    OSL_PRECOND(!m_aValueDesc.isNull,"OValueConverter::convertScalarToAny - check for NULL before calling");
    OSL_ENSURE(!m_aValueDesc.sType.equalsIgnoreAsciiCase(TYPE_ANY),"'Any' values must be NULL");

    // check for Binary
    if (m_aValueDesc.sType.equalsIgnoreAsciiCase(TYPE_BINARY))
    {
        Sequence<sal_Int8> aBinarySeq = parseBinary(aContent);
        rValue <<= aBinarySeq;
    }

    else
    {
        rValue = toAny(m_xTypeConverter, aContent, toTypeClass(m_aValueDesc.sType));
    }

    if (!rValue.hasValue())
    {
        OSL_ENSURE(aContent.getLength() == 0, "ValueConverter: Converted non-empty data as NULL ?!");
        OSL_TRACE("Warning: ValueConverter: Empty data without 'null' attribute treated as NULL !");

        // m_aValueDesc.isNull = true; // violates const
    }

    return !! rValue.hasValue();
}

// -----------------------------------------------------------------------------
template <class T>
bool convertListToSequence(StringList const& aStringList, uno::Sequence< T >& rSequence, OValueConverter const& rConverter)
        CFG_UNO_THROW1 ( script::CannotConvertException )
{
    uno::TypeClass const aElementTypeClass = ::getCppuType(static_cast<T const*>(0)).getTypeClass();

    rSequence.realloc(aStringList.size());

    sal_uInt32 nPos = 0;

    for(StringList::const_iterator it = aStringList.begin();
        it != aStringList.end();
        ++it)
    {
        uno::Any aValueAny = toAny(rConverter.getTypeConverter(), *it, aElementTypeClass);

        if (aValueAny >>= rSequence[nPos])
            ++nPos;

        else if (!aValueAny.hasValue())
            OSL_ENSURE(false,"UNEXPECTED: Found NULL value in List - ignoring value !");

        else
            OSL_ENSURE(false,"ERROR: Cannot extract converted value into List - skipping value !");
    }

    bool bOK = (nPos == aStringList.size());

    if (!bOK)
    {
        OSL_ASSERT(nPos < aStringList.size());
        rSequence.realloc(nPos);
    }
    return bOK;
}

// -----------------------------------------------------------------------------
// special overload for binary sequence

// template<> // use an explicit specialization
bool convertListToSequence(StringList const& aStringList, uno::Sequence< uno::Sequence<sal_Int8> >& rSequence, OValueConverter const& rParser )
        CFG_UNO_THROW1 ( script::CannotConvertException )
{
    rSequence.realloc(aStringList.size());

    sal_uInt32 nPos = 0;

    for(StringList::const_iterator it = aStringList.begin();
        it != aStringList.end();
        ++it)
    {
        rSequence[nPos++] = rParser.parseBinary(*it);
    }
    return true;
}

// -----------------------------------------------------------------------------

bool OValueConverter::convertListToAny(StringList const& aContentList, uno::Any& rValue) const
        CFG_UNO_THROW1 ( script::CannotConvertException )
{
    OSL_PRECOND(!m_aValueDesc.isNull,"OValueConverter::convertListToAny - check for NULL before calling");
    OSL_ENSURE(!m_aValueDesc.sType.equalsIgnoreAsciiCase(TYPE_ANY),"'Any' not allowed for lists");

    if (m_aValueDesc.sType.equalsIgnoreAsciiCase(TYPE_STRING))
    {
        Sequence< OUString > aSequence;
        convertListToSequence(aContentList,aSequence,*this);
        rValue <<= aSequence;
    }
    else if (m_aValueDesc.sType.equalsIgnoreAsciiCase(TYPE_BOOLEAN))
    {
        Sequence< sal_Bool > aSequence;
        convertListToSequence(aContentList,aSequence,*this);
        rValue <<= aSequence;
    }
    else if (m_aValueDesc.sType.equalsIgnoreAsciiCase(TYPE_SHORT))
    {
        Sequence< sal_Int16 > aSequence;
        convertListToSequence(aContentList,aSequence,*this);
        rValue <<= aSequence;
    }
    else if (m_aValueDesc.sType.equalsIgnoreAsciiCase(TYPE_INT))
    {
        Sequence< sal_Int32 > aSequence;
        convertListToSequence(aContentList,aSequence,*this);
        rValue <<= aSequence;
    }
    else if (m_aValueDesc.sType.equalsIgnoreAsciiCase(TYPE_LONG))
    {
        Sequence< sal_Int64 > aSequence;
        convertListToSequence(aContentList,aSequence,*this);
        rValue <<= aSequence;
    }
    else if (m_aValueDesc.sType.equalsIgnoreAsciiCase(TYPE_DOUBLE))
    {
        Sequence< double > aSequence;
        convertListToSequence(aContentList,aSequence,*this);
        rValue <<= aSequence;
    }
    else if (m_aValueDesc.sType.equalsIgnoreAsciiCase(TYPE_BINARY))
    {
        Sequence< Sequence<sal_Int8> > aSequence;
        convertListToSequence(aContentList,aSequence,*this);
        rValue <<= aSequence;
    }
    else
    {
        OSL_ENSURE(false, "Unknown element type in list");
        throwConversionError("Invalid value-type found in list value");
    }
    return !! rValue.hasValue();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
uno::Sequence<sal_Int8> ValueConverter::parseBinary(OUString const& aBinaryString_) const
        CFG_UNO_THROW1 ( script::CannotConvertException)
{
    uno::Sequence<sal_Int8> aResultSeq;

    parseHexBinary(aBinaryString_,aResultSeq);

    return aResultSeq;
}

// -----------------------------------------------------------------------------
static inline
uno::Type getBinaryType()
{
    uno::Sequence<sal_Int8> const * const for_binary = 0;
    return ::getCppuType(for_binary);
}

// -----------------------------------------------------------------------------
bool ValueConverter::isList() const
{
    return  m_aType.getTypeClass() == uno::TypeClass_SEQUENCE &&
            m_aType != getBinaryType();
}

// -----------------------------------------------------------------------------
uno::Any ValueConverter::convertToAny(OUString const& aContent) const
        CFG_UNO_THROW1( script::CannotConvertException)
{
    uno::Any aValue;

    if (this->isNull())
    {
        OSL_ENSURE(aContent.trim().getLength() == 0, "ValueConverter: Non-empty Null Value - ignoring content");
        OSL_ASSERT(!aValue.hasValue());
    }

    else if (this->isList())
    {
        StringList aContentList;
        splitListData(aContent, aContentList);
        convertListToAny(aContentList, aValue);
    }

    else
    {
        convertScalarToAny(aContent, aValue);
    }

    return aValue;
}

// -----------------------------------------------------------------------------
bool ValueConverter::convertScalarToAny(OUString const& aContent, uno::Any& rValue) const
        CFG_UNO_THROW1 ( script::CannotConvertException )
{
    OSL_PRECOND(!this->isNull(),"OValueConverter::convertScalarToAny - check for NULL before calling");
    OSL_ENSURE(m_aType.getTypeClass() != uno::TypeClass_ANY,"'Any' values must be NULL");

    // check for Binary
    if (m_aType == getBinaryType())
    {
        Sequence<sal_Int8> aBinarySeq = parseBinary(aContent);
        rValue <<= aBinarySeq;
    }

    else
    {
        rValue = toAny(m_xTypeConverter, aContent, m_aType.getTypeClass());
    }

    return !! rValue.hasValue();
}

// -----------------------------------------------------------------------------
template <class T>
bool convertListToSequence(StringList const& aStringList, uno::Sequence< T >& rSequence, uno::TypeClass aElementTypeClass, ValueConverter const& rConverter)
        CFG_UNO_THROW1 ( script::CannotConvertException )
{
    OSL_ASSERT(aElementTypeClass == ::getCppuType(static_cast<T const*>(0)).getTypeClass());

    rSequence.realloc(aStringList.size());

    sal_uInt32 nPos = 0;

    for(StringList::const_iterator it = aStringList.begin();
        it != aStringList.end();
        ++it)
    {
        uno::Any aValueAny = toAny(rConverter.getTypeConverter(), *it, aElementTypeClass);

        if (aValueAny >>= rSequence[nPos])
            ++nPos;

        else if (!aValueAny.hasValue())
            OSL_ENSURE(false,"UNEXPECTED: Found NULL value in List - ignoring value !");

        else
            OSL_ENSURE(false,"ERROR: Cannot extract converted value into List - skipping value !");
    }

    bool bOK = (nPos == aStringList.size());

    if (!bOK)
    {
        OSL_ASSERT(nPos < aStringList.size());
        rSequence.realloc(nPos);
    }
    return bOK;
}

// -----------------------------------------------------------------------------
// special overload for binary sequence

// template<> // use an explicit specialization
bool convertListToSequence(StringList const& aStringList, uno::Sequence< uno::Sequence<sal_Int8> >& rSequence, uno::TypeClass aElementTypeClass, ValueConverter const& rParser )
        CFG_UNO_THROW1 ( script::CannotConvertException )
{
    OSL_ASSERT(aElementTypeClass == uno::TypeClass_SEQUENCE);

    rSequence.realloc(aStringList.size());

    sal_uInt32 nPos = 0;

    for(StringList::const_iterator it = aStringList.begin();
        it != aStringList.end();
        ++it)
    {
        rSequence[nPos++] = rParser.parseBinary(*it);
    }
    return true;
}

// -----------------------------------------------------------------------------

#define MAYBE_EXTRACT_SEQUENCE( type ) \
    if (aElementType == ::getCppuType( (type const *)0))    \
    {                                                       \
        Sequence< type > aSequence;                         \
        convertListToSequence(aContentList,aSequence,aElementTypeClass, *this); \
        rValue <<= aSequence;                               \
    }

bool ValueConverter::convertListToAny(StringList const& aContentList, uno::Any& rValue) const
        CFG_UNO_THROW1 ( script::CannotConvertException )
{
    OSL_PRECOND(!this->isNull(),"OValueConverter::convertListToAny - check for NULL before calling");
    OSL_ENSURE(m_aType.getTypeClass() != uno::TypeClass_ANY,"'Any' not allowed for lists");

    uno::Type       aElementType        = getSequenceElementType(m_aType);
    uno::TypeClass  aElementTypeClass   = aElementType.getTypeClass();

    OSL_ENSURE(aElementTypeClass != uno::TypeClass_ANY,"'Any' not allowed for list elements");

    MAYBE_EXTRACT_SEQUENCE( OUString )
    else
    MAYBE_EXTRACT_SEQUENCE( sal_Bool )
    else
    MAYBE_EXTRACT_SEQUENCE( sal_Int16 )
    else
    MAYBE_EXTRACT_SEQUENCE( sal_Int32 )
    else
    MAYBE_EXTRACT_SEQUENCE( sal_Int64 )
    else
    MAYBE_EXTRACT_SEQUENCE( double )
    else
    MAYBE_EXTRACT_SEQUENCE( Sequence<sal_Int8> )
    else
    {
        OSL_ENSURE(false, "Unknown element type in list");
        throwConversionError("Invalid value-type found in list value");
    }

    return !! rValue.hasValue();
}
#undef MAYBE_EXTRACT_SEQUENCE

// -----------------------------------------------------------------------------
namespace
{
    sal_Int32 const NO_MORE_TOKENS = -1;
    struct OTokenizeByWhitespace
    {

        static inline bool isWhitespace(sal_Unicode ch)
        {
            // note: for definition of whitescape see also
            //   canUseWhitespace(OUString const&)
            // in xmlformater.cxx
            // -----------------------------------------------------------------------------
            return rtl_ascii_isWhitespace(ch) ? true : false;
        }

        sal_Int32 findFirstTokenStart(OUString const& sText) const CFG_NOTHROW()
        {
            return findNextTokenStart(sText,0);
        }

        sal_Int32 findNextTokenStart(OUString const& sText, sal_Int32 nPrevTokenEnd) const  CFG_NOTHROW()
        {
            sal_Int32 const nEnd = sText.getLength();
            sal_Int32 nPos = nPrevTokenEnd;

            OSL_PRECOND( nPos == 0 || (0 < nPos && nPos < nEnd && isWhitespace(sText[nPos])) || nPos == nEnd,
                         "Invalid nPrevTokenEnd");

            while (nPos < nEnd && isWhitespace(sText[nPos]))
            {
                ++nPos;
            }

            if (nPos < nEnd)
                return nPos;
            else
                return NO_MORE_TOKENS;
        }

        sal_Int32 findTokenEnd(OUString const& sText, sal_Int32 nTokenStart) const CFG_NOTHROW()
        {
            sal_Int32 const nEnd = sText.getLength();
            sal_Int32 nPos = nTokenStart;

            OSL_PRECOND( 0 <= nPos && nPos < nEnd && !isWhitespace(sText[nPos]),
                         "Invalid nTokenStart");

            while (nPos < nEnd && !isWhitespace(sText[nPos]))
            {
                ++nPos;
            }

            return nPos;
        }
    };
// -----------------------------------------------------------------------------
    struct OTokenizeBySeparator
    {
        OUString const sSeparator;
        OTokenizeBySeparator(OUString const& _sSeparator) CFG_NOTHROW()
            : sSeparator(_sSeparator)
        {
            OSL_PRECOND(sSeparator.trim().getLength() > 0, "Invalid empty separator string");
        }

        sal_Int32 findFirstTokenStart(OUString const& sText) const CFG_NOTHROW()
        {
            return 0;
        }
        sal_Int32 findNextTokenStart(OUString const& sText, sal_Int32 nPrevTokenEnd) const CFG_NOTHROW()
        {
            sal_Int32 const nEnd = sText.getLength();
            sal_Int32 nPos = nPrevTokenEnd;
            OSL_PRECOND( nPos == nEnd || (0 <= nPos && nPos < nEnd && sText.indexOf(sSeparator, nPos) == nPos),
                         "Invalid nPrevTokenEnd");

            if (nPos < nEnd)
                return nPos + sSeparator.getLength();
            else
                return NO_MORE_TOKENS;
        }
        sal_Int32 findTokenEnd(OUString const& sText, sal_Int32 nTokenStart) const CFG_NOTHROW()
        {
            sal_Int32 const nEnd = sText.getLength();
            OSL_PRECOND( 0 <= nTokenStart && nTokenStart <= nEnd ,
                         "Invalid nTokenStart");

            sal_Int32 nPos = sText.indexOf(sSeparator,nTokenStart);

            if (nPos >= 0)
                return nPos;
            else
                return nEnd;
        }
    };
// -----------------------------------------------------------------------------
    template <class Tokenizer>
    void tokenizeListData(Tokenizer const& aTokenizer, OUString const& aContent, StringList& rContentList)
            CFG_NOTHROW( )
    {
        sal_Int32 nTokenPos = aTokenizer.findFirstTokenStart(aContent);

        while(nTokenPos != NO_MORE_TOKENS)
        {
            sal_Int32 nTokenEnd = aTokenizer.findTokenEnd(aContent, nTokenPos);

            // this is what the tokenizer must provide
            OSL_ASSERT(0 <= nTokenPos && nTokenPos <= nTokenEnd && nTokenEnd <= aContent.getLength());

            rContentList.push_back( aContent.copy(nTokenPos, nTokenEnd-nTokenPos) );

            nTokenPos= aTokenizer.findNextTokenStart(aContent, nTokenEnd);
        }
    }
// -----------------------------------------------------------------------------
}
// -----------------------------------------------------------------------------
void OValueConverter::splitListData(OUString const& aContent, StringList& rContentList) const
    CFG_NOTHROW( )
{
    OUString sSeparator = m_aValueDesc.sSeparator;

    bool bSeparateByWhitespace = (sSeparator.trim().getLength() == 0);

    OSL_ENSURE( bSeparateByWhitespace == (!sSeparator.getLength() || sSeparator.equals(DEFAULT_SEPARATOR)),
                "Unexpected whitespace-only separator");

    if (bSeparateByWhitespace)
    {
        OSL_ENSURE( sSeparator.getLength()==0 || sSeparator.equals(DEFAULT_SEPARATOR),
                    "Unexpected whitespace-only separator");

        tokenizeListData( OTokenizeByWhitespace(), aContent, rContentList );
    }
    else
    {
        OSL_ENSURE( sSeparator.trim()==sSeparator,
                    "Unexpected whitespace in separator");

        tokenizeListData( OTokenizeBySeparator(sSeparator), aContent, rContentList );
    }
}
// -----------------------------------------------------------------------------
void ValueConverter::splitListData(OUString const& aContent, StringList& rContentList) const
    CFG_NOTHROW( )
{
    OUString sSeparator = m_sSeparator;

    bool bSeparateByWhitespace = (sSeparator.trim().getLength() == 0);

    OSL_ENSURE( bSeparateByWhitespace == (!sSeparator.getLength() || sSeparator.equals(DEFAULT_SEPARATOR)),
                "Unexpected whitespace-only separator");

    if (bSeparateByWhitespace)
    {
        OSL_ENSURE( sSeparator.getLength()==0 || sSeparator.equals(DEFAULT_SEPARATOR),
                    "Unexpected whitespace-only separator");

        tokenizeListData( OTokenizeByWhitespace(), aContent, rContentList );
    }
    else
    {
        OSL_ENSURE( sSeparator.trim()==sSeparator,
                    "Unexpected whitespace in separator");

        tokenizeListData( OTokenizeBySeparator(sSeparator), aContent, rContentList );
    }
}
// -----------------------------------------------------------------------------

} // namespace
