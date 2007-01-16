/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: unomodel.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: vg $ $Date: 2007-01-16 16:34:12 $
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
#include "precompiled_basctl.hxx"

#include "unomodel.hxx"

#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

#include <sfx2/docfac.hxx>
#include <sfx2/objsh.hxx>

#include <iderdll.hxx>
#include <basdoc.hxx>

using namespace ::vos;
using namespace ::rtl;
using namespace ::cppu;
using namespace ::std;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;

SIDEModel::SIDEModel( SfxObjectShell *pObjSh )
: SfxBaseModel(pObjSh)
{
}

SIDEModel::~SIDEModel()
{
}

uno::Any SAL_CALL SIDEModel::queryInterface( const uno::Type& rType ) throw(uno::RuntimeException)
{
    uno::Any aRet =  ::cppu::queryInterface ( rType,
                                    // OWeakObject interfaces
                                    static_cast< XInterface* >( static_cast< OWeakObject* >( this ) ),
                                    static_cast< XWeak* > ( this ),
                                    static_cast< XServiceInfo*  > ( this ) );
    if (!aRet.hasValue())
        aRet = SfxBaseModel::queryInterface ( rType );
    return aRet;
}

void SAL_CALL SIDEModel::acquire() throw()
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    OWeakObject::acquire();
}

void SAL_CALL SIDEModel::release() throw()
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    OWeakObject::release();
}

uno::Sequence< uno::Type > SAL_CALL SIDEModel::getTypes(  ) throw(uno::RuntimeException)
{
    uno::Sequence< uno::Type > aTypes = SfxBaseModel::getTypes();
    sal_Int32 nLen = aTypes.getLength();
    aTypes.realloc(nLen + 1);
    uno::Type* pTypes = aTypes.getArray();
    pTypes[nLen++] = ::getCppuType((Reference<XServiceInfo>*)0);

    return aTypes;
}

OUString SIDEModel::getImplementationName(void) throw( uno::RuntimeException )
{
    return getImplementationName_Static();
}

::rtl::OUString SIDEModel::getImplementationName_Static()
{
    return rtl::OUString::createFromAscii("com.sun.star.comp.basic.BasicIDE");
}

sal_Bool SIDEModel::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
    return rServiceName == ::rtl::OUString::createFromAscii("com.sun.star.script.BasicIDE");
}
uno::Sequence< OUString > SIDEModel::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    return getSupportedServiceNames_Static();
}

uno::Sequence< OUString > SIDEModel::getSupportedServiceNames_Static(void)
{
    uno::Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = ::rtl::OUString::createFromAscii("com.sun.star.script.BasicIDE");
    return aRet;
}

uno::Reference< uno::XInterface > SAL_CALL SIDEModel_createInstance(
                const uno::Reference< lang::XMultiServiceFactory > & ) throw( uno::Exception )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    BasicIDEDLL::Init();
    SfxObjectShell* pShell = new BasicDocShell();
    return uno::Reference< uno::XInterface >( pShell->GetModel() );
}

