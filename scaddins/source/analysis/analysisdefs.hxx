/*************************************************************************
 *
 *  $RCSfile: analysisdefs.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: hr $ $Date: 2003-03-26 17:46:44 $
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

#ifndef ANALYSISDEFS_HXX
#define ANALYSISDEFS_HXX

#define CSS                 ::com::sun::star
#define XPROPSET            CSS::beans::XPropertySet
#define REF(c)              CSS::uno::Reference< c >
#define constREFXPS         const REF(XPROPSET)
#define SEQ(c)              CSS::uno::Sequence< c >
#define SEQSEQ(c)           CSS::uno::Sequence< CSS::uno::Sequence< c > >
#define SEQofLocName        SEQ( CSS::sheet::LocalizedName )
#define ANY                 CSS::uno::Any
#define SEQ_ANY             SEQ(ANY)
#define STRING              ::rtl::OUString
#define STRFROMASCII(s)     STRING::createFromAscii( s )
#define STRFROMANSI(s)      STRING( s, strlen( s ), RTL_TEXTENCODING_MS_1252 )
#define THROWDEF_RTE        throw(CSS::uno::RuntimeException)
#define THROW_RTE           throw CSS::uno::RuntimeException()
#define THROWDEF_RTE_IAE    throw(CSS::uno::RuntimeException,CSS::lang::IllegalArgumentException)
#define THROW_IAE           throw CSS::lang::IllegalArgumentException()

#define CHK_Freq            ( nFreq != 1 && nFreq != 2 && nFreq != 4 )
#define CHK_FINITE(d)       if( !::rtl::math::isFinite( d ) ) THROW_IAE
#define RETURN_FINITE(d)    if( ::rtl::math::isFinite( d ) ) return d; else THROW_IAE

#endif

