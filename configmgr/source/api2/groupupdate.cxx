/*************************************************************************
 *
 *  $RCSfile: groupupdate.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2003-03-19 16:18:34 $
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

#include "groupupdate.hxx"
#include "updateimpl.hxx"
#include "apinodeaccess.hxx"
#include "apitypes.hxx"

#include <cppuhelper/queryinterface.hxx>
#include <cppuhelper/typeprovider.hxx>

namespace configmgr
{
//////////////////////////////////////////////////////////////////////////////////

    using uno::Reference;
    using uno::Sequence;
    using uno::Any;
    using uno::RuntimeException;

//////////////////////////////////////////////////////////////////////////////////
// class BasicGroup
//////////////////////////////////////////////////////////////////////////////////

// XInterface joining
//////////////////////////////////////////////////////////////////////////////////
uno::Any SAL_CALL BasicGroup::queryInterface( uno::Type const& rType ) throw (uno::RuntimeException )
{
    Any aRet = BasicGroupAccess::queryInterface( rType );
    if (!aRet.hasValue())
    {
        aRet = cppu::queryInterface(rType
                    , static_cast< css::container::XNameReplace *>(this)
                    );
    }
    return aRet;
}

// XTypeProvider joining
//////////////////////////////////////////////////////////////////////////////////
uno::Sequence< uno::Type > SAL_CALL BasicGroup::getTypes( ) throw (uno::RuntimeException )
{
    /*static ?*/
    cppu::OTypeCollection aTypes(
        configapi::getReferenceType(static_cast< css::container::XNameReplace *>(this)),
        BasicGroupAccess::getTypes());

    return aTypes.getTypes();
}

//uno::Sequence< sal_Int8 > SAL_CALL BasicGroup::getImplementationId( ) throw (uno::RuntimeException ) = 0;

//////////////////////////////////////////////////////////////////////////////////

// safe write access
/////////////////////////////////////////////////////////////
configapi::NodeGroupAccess& BasicGroup::getGroupNode()
{
    configapi::NodeGroupAccess* pAccess = maybeGetUpdateAccess();
    OSL_ENSURE(pAccess, "Write operation invoked on a read-only node access - failing with RuntimeException");

    if (!pAccess)
    {
        throw uno::RuntimeException(
                OUString(RTL_CONSTASCII_USTRINGPARAM("Configuration: Invalid Object - internal update interface missing.")),
                static_cast< css::container::XNameReplace * >(this)
            );
    }
    return *pAccess;
}

// New Interface methods
// XNameReplace
//////////////////////////////////////////////////////////////////////////////////
void SAL_CALL BasicGroup::replaceByName( const OUString& rName, const uno::Any& rElement )
        throw(css::lang::IllegalArgumentException, css::container::NoSuchElementException, css::lang::WrappedTargetException, uno::RuntimeException)
{
    configapi::implReplaceByName( getGroupNode(), rName, rElement );
}

//-----------------------------------------------------------------------------------
} // namespace configmgr


