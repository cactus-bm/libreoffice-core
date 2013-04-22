/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "sal/config.h"

#include <cassert>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include <stdio.h> // vsnprintf
#include <string.h> // strdup

#include "osl/thread.hxx"
#include "rtl/string.h"
#include "sal/detail/log.h"
#include "sal/log.hxx"
#include "sal/types.h"

#include "logformat.hxx"

#if defined WNT
#include <process.h>
#define OSL_DETAIL_GETPID _getpid()
#elif !defined ANDROID
#include <unistd.h>
#define OSL_DETAIL_GETPID getpid()
#endif

#if HAVE_SYSLOG_H
#include <syslog.h>
// sal/osl/unx/salinit.cxx::sal_detail_initialize updates this:
bool sal_use_syslog;
#endif

#ifdef ANDROID
#include <android/log.h>
#endif

// Avoid the use of other sal code in this file as much as possible, so that
// this code can be called from other sal code without causing endless
// recursion.

namespace {

bool equalStrings(
    char const * string1, std::size_t length1, char const * string2,
    std::size_t length2)
{
    return length1 == length2 && std::memcmp(string1, string2, length1) == 0;
}

#ifndef ANDROID

char const * toString(sal_detail_LogLevel level) {
    switch (level) {
    default:
        assert(false); // this cannot happen
        // fall through
    case SAL_DETAIL_LOG_LEVEL_INFO:
        return "info";
    case SAL_DETAIL_LOG_LEVEL_WARN:
        return "warn";
    case SAL_DETAIL_LOG_LEVEL_DEBUG:
        return "debug";
    }
}

#endif

// getenv is not thread safe, so minimize use of result; except on Android,
// see 60628799633ffde502cb105b98d3f254f93115aa "Notice if SAL_LOG is
// changed while the process is running":
#if defined ANDROID

char const * getEnvironmentVariable() {
    return std::getenv("SAL_LOG");
}

#else

char const * getEnvironmentVariable_() {
    char const * p1 = std::getenv("SAL_LOG");
    if (p1 == 0) {
        return 0;
    }
    char const * p2 = strdup(p1); // leaked
    if (p2 == 0) {
        std::abort(); // cannot do much here
    }
    return p2;
}

char const * getEnvironmentVariable() {
    static char const * env = getEnvironmentVariable_();
    return env;
}

#endif

#ifndef ANDROID
#if HAVE_SYSLOG_H
int toSyslogPriority(sal_detail_LogLevel level) {
    switch (level) {
    default:
        assert(false); // this cannot happen
        // fall through
    case SAL_DETAIL_LOG_LEVEL_INFO:
        return LOG_INFO;
    case SAL_DETAIL_LOG_LEVEL_WARN:
        return LOG_WARNING;
    case SAL_DETAIL_LOG_LEVEL_DEBUG:
        return LOG_DEBUG;
    }
}
#endif
#endif

bool report(sal_detail_LogLevel level, char const * area) {
    if (level == SAL_DETAIL_LOG_LEVEL_DEBUG)
        return true;
    assert(area != 0);
    char const * env = getEnvironmentVariable();
    if (env == 0) {
        env = "+WARN";
    }
    std::size_t areaLen = std::strlen(area);
    enum Sense { POSITIVE = 0, NEGATIVE = 1 };
    std::size_t senseLen[2] = { 0, 1 };
        // initial senseLen[POSITIVE] < senseLen[NEGATIVE], so that if there are
        // no matching switches at all, the result will be negative (and
        // initializing with 1 is safe as the length of a valid switch, even
        // without the "+"/"-" prefix, will always be > 1)
    for (char const * p = env;;) {
        Sense sense;
        switch (*p++) {
        case '\0':
            return senseLen[POSITIVE] >= senseLen[NEGATIVE];
                // if a specific item is both postiive and negative
                // (senseLen[POSITIVE] == senseLen[NEGATIVE]), default to
                // positive
        case '+':
            sense = POSITIVE;
            break;
        case '-':
            sense = NEGATIVE;
            break;
        default:
            return true; // upon an illegal SAL_LOG value, enable everything
        }
        char const * p1 = p;
        while (*p1 != '.' && *p1 != '+' && *p1 != '-' && *p1 != '\0') {
            ++p1;
        }
        bool match;
        if (equalStrings(p, p1 - p, RTL_CONSTASCII_STRINGPARAM("INFO"))) {
            match = level == SAL_DETAIL_LOG_LEVEL_INFO;
        } else if (equalStrings(p, p1 - p, RTL_CONSTASCII_STRINGPARAM("WARN")))
        {
            match = level == SAL_DETAIL_LOG_LEVEL_WARN;
        } else {
            return true;
                // upon an illegal SAL_LOG value, everything is considered
                // positive
        }
        char const * p2 = p1;
        while (*p2 != '+' && *p2 != '-' && *p2 != '\0') {
            ++p2;
        }
        if (match) {
            if (*p1 == '.') {
                ++p1;
                std::size_t n = p2 - p1;
                if ((n == areaLen && equalStrings(p1, n, area, areaLen))
                    || (n < areaLen && area[n] == '.'
                        && equalStrings(p1, n, area, n)))
                {
                    senseLen[sense] = p2 - p;
                }
            } else {
                senseLen[sense] = p1 - p;
            }
        }
        p = p2;
    }
}

void log(
    sal_detail_LogLevel level, char const * area, char const * where,
    char const * message)
{
    std::ostringstream s;
#ifndef ANDROID
#if HAVE_SYSLOG_H
    if (!sal_use_syslog)
#endif
        s << toString(level) << ':';
#endif
    if (level == SAL_DETAIL_LOG_LEVEL_DEBUG) {
        s << /*no area or where */ ' ' << message << '\n';
    } else {
#ifdef ANDROID
        // The area will be used as the "tag", and log info already contgains the pid on Android
#else
        s << area << ':' << OSL_DETAIL_GETPID << ':';
#endif
        s << osl::Thread::getCurrentIdentifier() << ':';
        if (strncmp(where, SRCDIR, sizeof(SRCDIR)-1) == 0)
            s << where+sizeof(SRCDIR);
        else
            s << where;
        s << message << '\n';
    }
#ifdef ANDROID
    int android_log_level;
    switch (level) {
    case SAL_DETAIL_LOG_LEVEL_INFO:
        android_log_level = ANDROID_LOG_INFO;
        break;
    case SAL_DETAIL_LOG_LEVEL_WARN:
        android_log_level = ANDROID_LOG_WARN;
        break;
    case SAL_DETAIL_LOG_LEVEL_DEBUG:
        android_log_level = ANDROID_LOG_DEBUG;
        break;
    default:
        android_log_level = ANDROID_LOG_INFO;
        break;
    }
    if (area == NULL)
        area = "LibreOffice";
    __android_log_print(android_log_level, area, "%s", s.str().c_str());
#else
#if HAVE_SYSLOG_H
    if (sal_use_syslog)
        syslog(toSyslogPriority(level), "%s", s.str().c_str());
    else
#endif
        std::fputs(s.str().c_str(), stderr);
#endif
}

}

void sal_detail_log(
    sal_detail_LogLevel level, char const * area, char const * where,
    char const * message)
{
    if (report(level, area)) {
        log(level, area, where, message);
    }
}

void sal_detail_logFormat(
    sal_detail_LogLevel level, char const * area, char const * where,
    char const * format, ...)
{
    if (report(level, area)) {
        std::va_list args;
        va_start(args, format);
        osl::detail::logFormat(level, area, where, format, args);
        va_end(args);
    }
}

void osl::detail::logFormat(
    sal_detail_LogLevel level, char const * area, char const * where,
    char const * format, std::va_list arguments)
{
    char buf[1024];
    int const len = sizeof buf - RTL_CONSTASCII_LENGTH("...");
    int n = vsnprintf(buf, len, format, arguments);
    if (n < 0) {
        std::strcpy(buf, "???");
    } else if (n >= len) {
        std::strcpy(buf + len - 1, "...");
    }
    log(level, area, where, buf);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
