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
#ifndef _EDITWIN_HXX
#define _EDITWIN_HXX

#if OSL_DEBUG_LEVEL > 1

#include <vcl/wrkwin.hxx>

#include <svtools/svmedit.hxx>

class ImpWorkWindow;

class EditWindow
{
protected:
    ImpWorkWindow   *pImpWorkWindow;
    sal_Bool check();

    WorkWindow  *pMemParent;
    String      aMemName;
    WinBits     iMemWstyle;

    String      aMemPreWinText;
    sal_Bool        bShowWin;

    xub_StrLen      nTextLen;

public:
    EditWindow( WorkWindow *pParent, const UniString &rName = UniString( RTL_CONSTASCII_USTRINGPARAM ( "Debug" ) ), WinBits iWstyle = WB_HSCROLL | WB_VSCROLL );
    virtual ~EditWindow();

    void Clear();
    void Show();
    void Hide();
    void AddText( const String &rNew );
    void AddText( const sal_Char* rNew );

    virtual sal_Bool Close(); // derived
    sal_Bool bQuiet;
    sal_Bool Check();
};

#endif
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
