/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <comphelper/extract.hxx>
#include <comphelper/numbers.hxx>
#include <comphelper/processfactory.hxx>
#include <comphelper/sequence.hxx>
#include <comphelper/string.hxx>
#include <comphelper/types.hxx>
#include <com/sun/star/beans/XFastPropertySet.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/sdbc/ColumnValue.hpp>
#include <com/sun/star/sdbc/DataType.hpp>
#include <com/sun/star/sdbc/KeyRule.hpp>
#include <com/sun/star/sdbc/ResultSetConcurrency.hpp>
#include <com/sun/star/sdbc/ResultSetType.hpp>
#include <com/sun/star/sdbcx/CompareBookmark.hpp>
#include <com/sun/star/sdbcx/KeyType.hpp>
#include <com/sun/star/sdbc/XResultSet.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#include <com/sun/star/sdbcx/XDeleteRows.hpp>
#include <com/sun/star/sdbcx/XIndexesSupplier.hpp>
#include <com/sun/star/ucb/XContentAccess.hpp>
#include <com/sun/star/util/NumberFormat.hpp>
#include <com/sun/star/util/NumberFormatsSupplier.hpp>
#include <com/sun/star/util/NumberFormatter.hpp>
#include <cppuhelper/factory.hxx>
#include <ctype.h>
#include <i18npool/languagetag.hxx>
#include <osl/thread.h>
#include <rtl/logfile.hxx>
#include <rtl/math.hxx>
#include <stdio.h>
#include <svl/converter.hxx>
#include <svl/zforlist.hxx>
#include <unotools/configmgr.hxx>
#include <unotools/syslocale.hxx>

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
