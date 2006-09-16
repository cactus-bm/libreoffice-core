/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fps_office.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 17:50:53 $
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
#include "precompiled_fpicker.hxx"

#ifndef _SAL_TYPES_H_
#include "sal/types.h"
#endif

#ifndef _CPPUHELPER_IMPLEMENTATIONENTRY_HXX_
#include "cppuhelper/implementationentry.hxx"
#endif

#include "OfficeFilePicker.hxx"
#include "OfficeFolderPicker.hxx"

static cppu::ImplementationEntry g_entries[] =
{
    {
        SvtFilePicker::impl_createInstance,
        SvtFilePicker::impl_getStaticImplementationName,
        SvtFilePicker::impl_getStaticSupportedServiceNames,
        cppu::createSingleComponentFactory, 0, 0
    },
    {
        SvtFolderPicker::impl_createInstance,
        SvtFolderPicker::impl_getStaticImplementationName,
        SvtFolderPicker::impl_getStaticSupportedServiceNames,
        cppu::createSingleComponentFactory, 0, 0
    },
    { 0, 0, 0, 0, 0, 0 }
};

extern "C"
{
SAL_DLLPUBLIC_EXPORT void SAL_CALL component_getImplementationEnvironment (
    const sal_Char ** ppEnvTypeName, uno_Environment ** /* ppEnv */)
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

SAL_DLLPUBLIC_EXPORT sal_Bool SAL_CALL component_writeInfo (
    void * pServiceManager, void * pRegistryKey)
{
    return cppu::component_writeInfoHelper (
        pServiceManager, pRegistryKey, g_entries);
}

SAL_DLLPUBLIC_EXPORT void * SAL_CALL component_getFactory (
    const sal_Char * pImplementationName, void * pServiceManager, void * pRegistryKey)
{
    return cppu::component_getFactoryHelper (
        pImplementationName, pServiceManager, pRegistryKey, g_entries);
}

} // extern "C"
