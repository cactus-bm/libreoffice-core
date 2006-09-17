/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ignoreTiJi_ja_JP.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 09:28:52 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_i18npool.hxx"

// prevent internal compiler error with MSVC6SP3
#include <utility>

#define TRANSLITERATION_TiJi_ja_JP
#include <transliteration_Ignore.hxx>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace rtl;

namespace com { namespace sun { namespace star { namespace i18n {


static Mapping TiJi[] = {
        // TU + I --> TI
        { 0x30C4, 0x30A3, 0x30C1, sal_True },
        // TE + I --> TI
        { 0x30C6, 0x30A3, 0x30C1, sal_True },
        // TU + I --> TI
        { 0x3064, 0x3043, 0x3061, sal_True },
        // TE + I --> TI
        { 0x3066, 0x3043, 0x3061, sal_True },
        // DE + I --> ZI
        { 0x30C7, 0x30A3, 0x30B8, sal_True },
        // DE + I --> ZI
        { 0x3067, 0x3043, 0x3058, sal_True },

        { 0, 0, 0, sal_True }
};

ignoreTiJi_ja_JP::ignoreTiJi_ja_JP()
{
        func = (TransFunc) 0;
        table = 0;
        map = TiJi;
        transliterationName = "ignoreTiJi_ja_JP";
        implementationName = "com.sun.star.i18n.Transliteration.ignoreTiJi_ja_JP";
}

} } } }
