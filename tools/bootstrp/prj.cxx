/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_tools.hxx"
#include <stdlib.h>
#include <stdio.h>
#include <osl/mutex.hxx>

#include <tools/stream.hxx>
#include <comphelper/string.hxx>
#include "bootstrp/prj.hxx"

#if defined(WNT)
#define LIST_DELIMETER ';'
#define PATH_DELIMETER '\\'
#elif defined UNX
#define LIST_DELIMETER ':'
#define PATH_DELIMETER '/'
#endif

SimpleConfig::SimpleConfig(const String &rSimpleConfigFileName)
{
    aFileStream.Open(rSimpleConfigFileName, STREAM_READ);
}

SimpleConfig::~SimpleConfig()
{
    aFileStream.Close ();
}

rtl::OString SimpleConfig::getNext()
{
    if (aStringBuffer == "")
      while ((aStringBuffer = GetNextLine()) == "\t") ; //solange bis != "\t"
    if ( aStringBuffer == "" )
        return rtl::OString();

    rtl::OString aString = comphelper::string::getToken(aStringBuffer, 0, '\t');

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4244)
#endif
    aStringBuffer.Erase(0, aString.getLength()+1);
#ifdef _MSC_VER
#pragma warning (pop)
#endif

    aStringBuffer = comphelper::string::stripStart(aStringBuffer, '\t');

    return aString;
}

rtl::OString SimpleConfig::GetNextLine()
{
    rtl::OString aTmpStr;
    aFileStream.ReadLine(aTmpStr);
    if (aTmpStr[0] == '#')
        return rtl::OString('\t');
    return aTmpStr.trim().replace(' ', '\t');
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
