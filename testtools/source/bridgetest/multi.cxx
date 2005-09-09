/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: multi.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 12:22:38 $
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

#include "sal/config.h"

#include "multi.hxx"

#include "rtl/string.hxx"
#include "rtl/textenc.h"
#include "rtl/ustring.hxx"

#include <sstream>

namespace css = com::sun::star;

namespace {

struct CheckFailed {
    explicit CheckFailed(rtl::OUString const & theMessage): message(theMessage)
    {}

    rtl::OUString message;
};

template< typename T > void checkEqual(T const & value, T const & argument) {
    if (argument != value) {
        std::ostringstream s;
        s << value << " != " << argument;
        throw CheckFailed(
            rtl::OStringToOUString(
                rtl::OString(s.str().c_str()), RTL_TEXTENCODING_UTF8));
    }
}

}

namespace testtools { namespace bridgetest {

rtl::OUString testMulti(
    com::sun::star::uno::Reference< test::testtools::bridgetest::XMulti >
    const & multi)
{
    try {
        checkEqual(
            0.0,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase1 >(
                multi, css::uno::UNO_QUERY_THROW)->getatt1());
        checkEqual(
            0.0,
            static_cast< test::testtools::bridgetest::XMultiBase2 * >(
                multi.get())->getatt1());
        checkEqual(
            0.0,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase2 >(
                multi, css::uno::UNO_QUERY_THROW)->getatt1());
        checkEqual(
            0.0,
            static_cast< test::testtools::bridgetest::XMultiBase5 * >(
                multi.get())->getatt1());
        checkEqual(
            0.0,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase5 >(
                multi, css::uno::UNO_QUERY_THROW)->getatt1());
        css::uno::Reference< test::testtools::bridgetest::XMultiBase5 >(
            multi, css::uno::UNO_QUERY_THROW)->setatt1(0.1);
        checkEqual(
            0.1,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase1 >(
                multi, css::uno::UNO_QUERY_THROW)->getatt1());
        checkEqual(
            0.1,
            static_cast< test::testtools::bridgetest::XMultiBase2 * >(
                multi.get())->getatt1());
        checkEqual(
            0.1,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase2 >(
                multi, css::uno::UNO_QUERY_THROW)->getatt1());
        checkEqual(
            0.1,
            static_cast< test::testtools::bridgetest::XMultiBase5 * >(
                multi.get())->getatt1());
        checkEqual(
            0.1,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase5 >(
                multi, css::uno::UNO_QUERY_THROW)->getatt1());
        checkEqual< sal_Int32 >(
            11 * 1,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase1 >(
                multi, css::uno::UNO_QUERY_THROW)->fn11(1));
        checkEqual< sal_Int32 >(
            11 * 1,
            static_cast< test::testtools::bridgetest::XMultiBase2 * >(
                multi.get())->fn11(1));
        checkEqual< sal_Int32 >(
            11 * 2,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase2 >(
                multi, css::uno::UNO_QUERY_THROW)->fn11(2));
        checkEqual< sal_Int32 >(
            11 * 1,
            static_cast< test::testtools::bridgetest::XMultiBase5 * >(
                multi.get())->fn11(1));
        checkEqual< sal_Int32 >(
            11 * 5,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase5 >(
                multi, css::uno::UNO_QUERY_THROW)->fn11(5));
        checkEqual(
            rtl::OUString::createFromAscii("12" "abc"),
            css::uno::Reference< test::testtools::bridgetest::XMultiBase1 >(
                multi, css::uno::UNO_QUERY_THROW)->fn12(
                    rtl::OUString::createFromAscii("abc")));
        checkEqual(
            rtl::OUString::createFromAscii("12" "abc-2"),
            static_cast< test::testtools::bridgetest::XMultiBase2 * >(
                multi.get())->fn12(rtl::OUString::createFromAscii("abc-2")));
        checkEqual(
            rtl::OUString::createFromAscii("12" "abc-2"),
            css::uno::Reference< test::testtools::bridgetest::XMultiBase2 >(
                multi, css::uno::UNO_QUERY_THROW)->fn12(
                    rtl::OUString::createFromAscii("abc-2")));
        checkEqual(
            rtl::OUString::createFromAscii("12" "abc-5"),
            static_cast< test::testtools::bridgetest::XMultiBase5 * >(
                multi.get())->fn12(rtl::OUString::createFromAscii("abc-5")));
        checkEqual(
            rtl::OUString::createFromAscii("12" "abc-5"),
            css::uno::Reference< test::testtools::bridgetest::XMultiBase5 >(
                multi, css::uno::UNO_QUERY_THROW)->fn12(
                    rtl::OUString::createFromAscii("abc-5")));
        checkEqual< sal_Int32 >(21 * 2, multi->fn21(2));
        checkEqual(
            rtl::OUString::createFromAscii("22" "de"),
            multi->fn22(rtl::OUString::createFromAscii("de")));
        checkEqual< sal_Int32 >(
            31 * 3,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase3 >(
                multi, css::uno::UNO_QUERY_THROW)->fn31(3));
        checkEqual< sal_Int32 >(
            31 * 5,
            static_cast< test::testtools::bridgetest::XMultiBase5 * >(
                multi.get())->fn31(5));
        checkEqual< sal_Int32 >(
            31 * 5,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase5 >(
                multi, css::uno::UNO_QUERY_THROW)->fn31(5));
        checkEqual(
            0.0,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase3 >(
                multi, css::uno::UNO_QUERY_THROW)->getatt3());
        checkEqual(
            0.0,
            static_cast< test::testtools::bridgetest::XMultiBase5 * >(
                multi.get())->getatt3());
        checkEqual(
            0.0,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase5 >(
                multi, css::uno::UNO_QUERY_THROW)->getatt3());
        css::uno::Reference< test::testtools::bridgetest::XMultiBase3 >(
            multi, css::uno::UNO_QUERY_THROW)->setatt3(0.3);
        checkEqual(
            0.3,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase3 >(
                multi, css::uno::UNO_QUERY_THROW)->getatt3());
        checkEqual(
            0.3,
            static_cast< test::testtools::bridgetest::XMultiBase5 * >(
                multi.get())->getatt3());
        checkEqual(
            0.3,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase5 >(
                multi, css::uno::UNO_QUERY_THROW)->getatt3());
        checkEqual(
            rtl::OUString::createFromAscii("32" "f"),
            css::uno::Reference< test::testtools::bridgetest::XMultiBase3 >(
                multi, css::uno::UNO_QUERY_THROW)->fn32(
                    rtl::OUString::createFromAscii("f")));
        checkEqual(
            rtl::OUString::createFromAscii("32" "f-5"),
            static_cast< test::testtools::bridgetest::XMultiBase5 * >(
                multi.get())->fn32(rtl::OUString::createFromAscii("f-5")));
        checkEqual(
            rtl::OUString::createFromAscii("32" "f-5"),
            css::uno::Reference< test::testtools::bridgetest::XMultiBase5 >(
                multi, css::uno::UNO_QUERY_THROW)->fn32(
                    rtl::OUString::createFromAscii("f-5")));
        checkEqual< sal_Int32 >(
            33,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase3 >(
                multi, css::uno::UNO_QUERY_THROW)->fn33());
        checkEqual< sal_Int32 >(
            33,
            static_cast< test::testtools::bridgetest::XMultiBase5 * >(
                multi.get())->fn33());
        checkEqual< sal_Int32 >(
            33,
            css::uno::Reference< test::testtools::bridgetest::XMultiBase5 >(
                multi, css::uno::UNO_QUERY_THROW)->fn33());
        checkEqual< sal_Int32 >(41 * 4, multi->fn41(4));
        checkEqual< sal_Int32 >(61 * 6, multi->fn61(6));
        checkEqual(
            rtl::OUString::createFromAscii("62" ""),
            multi->fn62(rtl::OUString::createFromAscii("")));
        checkEqual< sal_Int32 >(71 * 7, multi->fn71(7));
        checkEqual(
            rtl::OUString::createFromAscii("72" "g"),
            multi->fn72(rtl::OUString::createFromAscii("g")));
        checkEqual< sal_Int32 >(73, multi->fn73());
    } catch (CheckFailed const & f) {
        return f.message;
    }
    return rtl::OUString();
}

} }
