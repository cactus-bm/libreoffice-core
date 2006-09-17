/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: abstdlg.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 12:14:58 $
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
#include "precompiled_vcl.hxx"

#include "abstdlg.hxx"
#include "cuilib.hxx"

#include <osl/module.hxx>
#include <tools/string.hxx>

typedef VclAbstractDialogFactory* (__LOADONCALLAPI *FuncPtrCreateDialogFactory)();

VclAbstractDialogFactory* VclAbstractDialogFactory::Create()
{
    FuncPtrCreateDialogFactory fp = 0;
    static ::osl::Module aDialogLibrary;
    if ( aDialogLibrary.is() || aDialogLibrary.load( String( RTL_CONSTASCII_USTRINGPARAM( DLL_NAME ) ) ) )
        fp = ( VclAbstractDialogFactory* (__LOADONCALLAPI*)() )
            aDialogLibrary.getFunctionSymbol( ::rtl::OUString::createFromAscii("CreateDialogFactory") );
    if ( fp )
        return fp();
    return 0;
}

VclAbstractDialog::~VclAbstractDialog()
{
}
