/*************************************************************************
 *
 *  $RCSfile: Inflater.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: mtg $ $Date: 2001-07-04 14:56:13 $
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
 *  Contributor(s): Martin Gallwey (gallwey@sun.com)
 *
 *
 ************************************************************************/
#ifndef _INFLATER_HXX_
#define _INFLATER_HXX_

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HDL_
#include <com/sun/star/uno/RuntimeException.hdl>
#endif

extern "C"
{
    typedef struct z_stream_s z_stream;
}
class Inflater
{
protected:
    sal_Bool                bFinish, bFinished, bSetParams, bNeedDict;
    sal_Int32               nOffset, nLength;
    z_stream*               pStream;
    com::sun::star::uno::Sequence < sal_Int8 >  sInBuffer;
    sal_Int32   doInflateBytes (com::sun::star::uno::Sequence < sal_Int8 > &rBuffer, sal_Int32 nNewOffset, sal_Int32 nNewLength);

public:
    Inflater(sal_Bool bNoWrap = sal_False);
    ~Inflater();
    void SAL_CALL setInputSegment( const ::com::sun::star::uno::Sequence< sal_Int8 >& rBuffer, sal_Int32 nNewOffset, sal_Int32 nNewLength )
        throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setInput( const ::com::sun::star::uno::Sequence< sal_Int8 >& rBuffer )
        throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setDictionarySegment( const ::com::sun::star::uno::Sequence< sal_Int8 >& rBuffer, sal_Int32 nNewOffset, sal_Int32 nNewLength )
        throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setDictionary( const ::com::sun::star::uno::Sequence< sal_Int8 >& rBuffer )
        throw(::com::sun::star::uno::RuntimeException);
    sal_Int32 SAL_CALL getRemaining(  )
        throw(::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL needsInput(  )
        throw(::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL needsDictionary(  )
        throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL finish(  )
        throw(::com::sun::star::uno::RuntimeException);
    sal_Bool SAL_CALL finished(  )
        throw(::com::sun::star::uno::RuntimeException);
    sal_Int32 SAL_CALL doInflateSegment( ::com::sun::star::uno::Sequence< sal_Int8 >& rBuffer, sal_Int32 nNewOffset, sal_Int32 nNewLength )
        throw(::com::sun::star::uno::RuntimeException);
    sal_Int32 SAL_CALL doInflate( ::com::sun::star::uno::Sequence< sal_Int8 >& rBuffer )
        throw(::com::sun::star::uno::RuntimeException);
    sal_Int32 SAL_CALL getAdler(  )
        throw(::com::sun::star::uno::RuntimeException);
    sal_Int32 SAL_CALL getTotalIn(  )
        throw(::com::sun::star::uno::RuntimeException);
    sal_Int32 SAL_CALL getTotalOut(  )
        throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL reset(  )
        throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL end(  )
        throw(::com::sun::star::uno::RuntimeException);
};

#endif
