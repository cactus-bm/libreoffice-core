/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: resourcehelper.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 23:21:02 $
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

#include "resourcehelper.hxx"
#include "frm_resource.hxx"

#include <rtl/ustring.hxx>
#include <tools/string.hxx>

using rtl::OUString;

#define OUSTRING(x) rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(x))

namespace xforms
{

OUString getResource( sal_uInt16 nResourceId )
{
    return getResource( nResourceId, OUString(), OUString(), OUString() );
}

OUString getResource( sal_uInt16 nResourceId,
                      const OUString& rInfo1 )
{
    return getResource( nResourceId, rInfo1, OUString(), OUString() );
}

OUString getResource( sal_uInt16 nResourceId,
                      const OUString& rInfo1,
                      const OUString& rInfo2 )
{
    return getResource( nResourceId, rInfo1, rInfo2, OUString() );
}

OUString getResource( sal_uInt16 nResourceId,
                      const OUString& rInfo1,
                      const OUString& rInfo2,
                      const OUString& rInfo3 )
{
    OUString sResource = frm::ResourceManager::loadString( nResourceId );
    OSL_ENSURE( sResource.getLength() > 0, "resource not found?" );

    // use old style String class for search and replace, so we don't have to
    // code this again.
    String sString( sResource );
    sString.SearchAndReplaceAll( String(OUSTRING("$1")), String(rInfo1) );
    sString.SearchAndReplaceAll( String(OUSTRING("$2")), String(rInfo2) );
    sString.SearchAndReplaceAll( String(OUSTRING("$3")), String(rInfo3) );
    return OUString( sString );
}

}   // namespace xforms