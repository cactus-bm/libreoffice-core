/*************************************************************************
 *
 *  $RCSfile: AutoBuffer.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2003-10-06 14:34:31 $
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

#ifndef _AUTO_BUFFER_HXX_
#define _AUTO_BUFFER_HXX_

//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring>
#endif

//-------------------------------------------------------------
// A simple unicode buffer management class, the class itself
// is responsible for the allocated unicode buffer, any
// modification of the buffer size outside the class may lead
// to undefined behaviour
//-------------------------------------------------------------

class CAutoUnicodeBuffer
{
public:

    // if bLazyCreation is true the buffer will be created
    // when someone wants to fill the buffer
    CAutoUnicodeBuffer( size_t size, sal_Bool bLazyCreation = sal_False );
    ~CAutoUnicodeBuffer( );

    // resizes the buffer
    sal_Bool SAL_CALL resize( size_t new_size );

    // zeros the buffer
    void SAL_CALL empty( );

    // fills the buffer with a given content
    sal_Bool SAL_CALL fill( const sal_Unicode* pContent, size_t nLen );

    // returns the size of the buffer
    size_t SAL_CALL size( ) const;

    // conversion operator
    operator sal_Unicode*( );

    // address operator
    sal_Unicode* operator&( );

    const sal_Unicode* operator&( ) const;

private:
    void SAL_CALL init( );

private:
    size_t m_buffSize; // the number of unicode chars
    sal_Unicode* m_pBuff;
};

#endif
