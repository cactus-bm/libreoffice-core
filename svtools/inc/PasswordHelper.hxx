/*************************************************************************
 *
 *  $RCSfile: PasswordHelper.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2005-04-13 09:57:19 $
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

#ifndef _SVTOOLS_PASSWORDHELPER_HXX
#define _SVTOOLS_PASSWORDHELPER_HXX

#ifndef INCLUDED_SVLDLLAPI_H
#include "svtools/svldllapi.h"
#endif

#ifndef _SAL_TYPES_H_
#include "sal/types.h"
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include "com/sun/star/uno/Sequence.hxx"
#endif

class String;

class SvPasswordHelper
{
    static void     GetHashPassword(com::sun::star::uno::Sequence <sal_Int8>& rPassHash, const sal_Char* pPass, sal_uInt32 nLen);
    static void     GetHashPasswordLittleEndian(com::sun::star::uno::Sequence<sal_Int8>& rPassHash, const String& sPass);
    static void     GetHashPasswordBigEndian(com::sun::star::uno::Sequence<sal_Int8>& rPassHash, const String& sPass);

public:
    SVL_DLLPUBLIC static void     GetHashPassword(com::sun::star::uno::Sequence<sal_Int8>& rPassHash, const String& sPass);
    /**
    Use this method to compare a given string with another given Hash value.
    This is necessary, because in older versions exists different hashs of the same string. They were endian dependent.
    We need this to handle old files. This method will compare against big and little endian. See #101326#
    */
    SVL_DLLPUBLIC static bool     CompareHashPassword(const com::sun::star::uno::Sequence<sal_Int8>& rOldPassHash, const String& sNewPass);
};

#endif

