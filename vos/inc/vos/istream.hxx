/*************************************************************************
 *
 *  $RCSfile: istream.hxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 15:18:12 $
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

#ifndef _VOS_ISTREAM_HXX_
#define _VOS_ISTREAM_HXX_

#ifndef _VOS_TYPES_HXX_
#   include <vos/types.hxx>
#endif

#ifdef _USE_NAMESPACE
namespace vos
{
#endif

/** Interface for a stream of data, on that you can read and write blocks of bytes.
*/
class IStream
{
public:

    IStream() { }
    virtual ~IStream() { }


    /** Retrieve n bytes from the stream and copy them into pBuffer.
        @param pBuffer receives the read data.
        @param n the number of bytes to read. pBuffer must be large enough
        to hold the n bytes!
        @return the number of read bytes
    */
    virtual sal_Int32 SAL_CALL read(void* pBuffer,
                         sal_uInt32 n) const= 0;

    /** Write n bytes from pBuffer to the stream.
        @param pBuffer contains the data to be written.
        @param n the number of bytes to write.
        @return the number of written bytes
    */
    virtual sal_Int32 SAL_CALL write(const void* pBuffer,
                          sal_uInt32 n)= 0;

    /** Checks if stream is closed for further reading.
        @return True is stream has ended (e.g. was closed).
    */
    virtual sal_Bool SAL_CALL isEof() const = 0;
};

#ifdef _USE_NAMESPACE
}
#endif

#endif  // _VOS_ISTREAM_HXX_

