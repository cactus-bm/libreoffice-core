/*************************************************************************
 *
 *  $RCSfile: ZipPackageEntry.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: mtg $ $Date: 2000-11-29 14:09:05 $
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
#ifndef _ZIP_PACKAGE_ENTRY_HXX
#include "ZipPackageEntry.hxx"
#endif

using namespace com::sun::star;
ZipPackageEntry::ZipPackageEntry (void)
{
}

ZipPackageEntry::~ZipPackageEntry( void )
{
}

uno::Any SAL_CALL ZipPackageEntry::queryInterface( const uno::Type& rType )
    throw(uno::RuntimeException)
{
    // Ask for my own supported interfaces ...
    uno::Any aReturn    ( ::cppu::queryInterface    (   rType                                       ,
                                                static_cast< container::XNamed*     > ( this )  ,
                                                static_cast< lang::XUnoTunnel*      > ( this )  ,
                                                static_cast< container::XChild*     > ( this )  ) );

    // If searched interface supported by this class ...
    if ( aReturn.hasValue () == sal_True )
    {
        // ... return this information.
        return aReturn ;
    }
    else
    {
        // Else; ... ask baseclass for interfaces!
        return OWeakObject::queryInterface ( rType ) ;
    }
}
void SAL_CALL ZipPackageEntry::acquire(  )
    throw()
{
    OWeakObject::acquire();
}
void SAL_CALL ZipPackageEntry::release(  )
    throw()
{
    OWeakObject::release();
}
    // XChild
::rtl::OUString SAL_CALL ZipPackageEntry::getName(  )
    throw(uno::RuntimeException)
{
    return aEntry.sName;
}
void SAL_CALL ZipPackageEntry::setName( const ::rtl::OUString& aName )
    throw(uno::RuntimeException)
{
    aEntry.sName = aName;
}
uno::Reference< uno::XInterface > SAL_CALL ZipPackageEntry::getParent(  )
        throw(uno::RuntimeException)
{
    return xParent;
}
void SAL_CALL ZipPackageEntry::setParent( const uno::Reference< uno::XInterface >& Parent )
        throw(lang::NoSupportException, uno::RuntimeException)
{
    uno::Reference < container::XNameContainer > xOldParent (xParent, uno::UNO_QUERY);
    uno::Reference < container::XNameContainer > xNewParent (Parent, uno::UNO_QUERY);

    if ( !xNewParent.is())
        throw (lang::NoSupportException());

    if ( xOldParent.is() && xOldParent->hasByName(getName()))
        xOldParent->removeByName(getName());
    uno::Any aAny;
    uno::Reference < lang::XUnoTunnel > xTunnel  = this;
    aAny <<= xTunnel;
    if (!xNewParent->hasByName(getName()))
        xNewParent->insertByName(getName(), aAny);
    xParent = Parent;
}
