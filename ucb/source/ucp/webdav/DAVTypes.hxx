/*************************************************************************
 *
 *  $RCSfile: DAVTypes.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kso $ $Date: 2000-11-13 15:20:30 $
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
#ifndef _DAVTYPES_HXX_
#define _DAVTYPES_HXX_

#include <rtl/ustring.hxx>
#include <com/sun/star/uno/Sequence.hxx>


namespace webdav_ucp
{

enum Depth { ZERO = 0, ONE = 1, INFINITY = -1 };

/* RFC 2518

15.1 Class 1

   A class 1 compliant resource MUST meet all "MUST" requirements in all
   sections of this document.

   Class 1 compliant resources MUST return, at minimum, the value "1" in
   the DAV header on all responses to the OPTIONS method.

15.2 Class 2

   A class 2 compliant resource MUST meet all class 1 requirements and
   support the LOCK method, the supportedlock property, the
   lockdiscovery property, the Time-Out response header and the Lock-
   Token request header.  A class "2" compliant resource SHOULD also
   support the Time-Out request header and the owner XML element.

   Class 2 compliant resources MUST return, at minimum, the values "1"
   and "2" in the DAV header on all responses to the OPTIONS method.
*/

struct DAVCapabilities
{
    bool class1;
    bool class2;
    bool executable;

    DAVCapabilities() : class1( false ), class2( false ), executable( false ) {}
};

// Note: Uncomment the following if locking support is required
/*
enum LockScope { EXCLUSIVE = 0, SHARED };

enum LockType { WRITE = 0 };

struct LockEntry
{
    LockScope scope;
    LockType    type;
};

struct Lock : public LockEntry
{
    rtl::OUString   uri;
    Depth           depth;
    rtl::OUString   owner;
    long            timeout;
    com::sun::star::uno::Sequence< rtl::OUString >  locktoken;
};
*/

}; // namespace webdav_ucp
#endif // _DAVTYPES_HXX_
