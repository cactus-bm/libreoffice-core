/*************************************************************************
 *
 *  $RCSfile: OfficeFolderPicker.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2005-04-13 08:50:25 $
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

#include "OfficeFolderPicker.hxx"

#include "iodlg.hxx"

#include <list>

#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif

#define _SVSTDARR_STRINGSDTOR
#include "svtools/svstdarr.hxx"

#ifndef  _COM_SUN_STAR_CONTAINER_XCONTENTENUMERATIONACCESS_HPP_
#include <com/sun/star/container/XContentEnumerationAccess.hpp>
#endif
#ifndef  _COM_SUN_STAR_CONTAINER_XSET_HPP_
#include <com/sun/star/container/XSet.hpp>
#endif
#ifndef  _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef  _CPPUHELPER_FACTORY_HXX_
#include <cppuhelper/factory.hxx>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif

// using ----------------------------------------------------------------

using namespace     ::com::sun::star::container;
using namespace     ::com::sun::star::lang;
using namespace     ::com::sun::star::uno;
using namespace     ::com::sun::star::beans;

//------------------------------------------------------------------------------------
// class SvtFolderPicker
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
SvtFolderPicker::SvtFolderPicker( const Reference < XMultiServiceFactory >& _rxFactory )
    :OCommonPicker( _rxFactory )
{
}

//------------------------------------------------------------------------------------
SvtFolderPicker::~SvtFolderPicker()
{
}

//------------------------------------------------------------------------------------
// disambiguate XInterface
//------------------------------------------------------------------------------------
IMPLEMENT_FORWARD_XINTERFACE2( SvtFolderPicker, OCommonPicker, SvtFolderPicker_Base )

//------------------------------------------------------------------------------------
// disambiguate XTypeProvider
//------------------------------------------------------------------------------------
IMPLEMENT_FORWARD_XTYPEPROVIDER2( SvtFolderPicker, OCommonPicker, SvtFolderPicker_Base )

//------------------------------------------------------------------------------------
// XExecutableDialog functions
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
void SAL_CALL SvtFolderPicker::setTitle( const ::rtl::OUString& _rTitle ) throw (RuntimeException)
{
    OCommonPicker::setTitle( _rTitle );
}

//------------------------------------------------------------------------------------
sal_Int16 SAL_CALL SvtFolderPicker::execute(  ) throw (RuntimeException)
{
    return OCommonPicker::execute();
}

//------------------------------------------------------------------------------------
SvtFileDialog* SvtFolderPicker::implCreateDialog( Window* _pParent )
{
    return new SvtFileDialog( _pParent, SFXWB_PATHDIALOG );
}

//------------------------------------------------------------------------------------
sal_Int16 SvtFolderPicker::implExecutePicker( )
{
    // set the default directory
    if ( m_aDisplayDirectory.getLength() > 0 )
        getDialog()->SetPath( m_aDisplayDirectory );
    else
    {
        // Default-Standard-Dir setzen
        INetURLObject aStdDirObj( SvtPathOptions().GetWorkPath() );
        getDialog()->SetPath( aStdDirObj.GetMainURL( INetURLObject::NO_DECODE ) );
    }

    // now we are ready to execute the dialog
    sal_Int16 nRet = getDialog()->Execute();

    return nRet;
}

//------------------------------------------------------------------------------------
// XFolderPicker functions
//------------------------------------------------------------------------------------

void SAL_CALL SvtFolderPicker::setDisplayDirectory( const ::rtl::OUString& aDirectory )
    throw( IllegalArgumentException, RuntimeException )
{
    m_aDisplayDirectory = aDirectory;
}

//------------------------------------------------------------------------------------
::rtl::OUString SAL_CALL SvtFolderPicker::getDisplayDirectory() throw( RuntimeException )
{
    ::rtl::OUString aResult;

    if ( ! getDialog() )
        return m_aDisplayDirectory;

    SvStringsDtor* pPathList = getDialog()->GetPathList();

    if ( pPathList->Count() )
        aResult = ::rtl::OUString( *pPathList->GetObject( 0 ) );

    delete pPathList;

    return aResult;
}

//------------------------------------------------------------------------------------
::rtl::OUString SAL_CALL SvtFolderPicker::getDirectory() throw( RuntimeException )
{
    ::rtl::OUString aResult;

    if ( ! getDialog() )
        return m_aDisplayDirectory;

    SvStringsDtor* pPathList = getDialog()->GetPathList();

    if ( pPathList->Count() )
        aResult = ::rtl::OUString( *pPathList->GetObject( 0 ) );

    delete pPathList;

    return aResult;
}

//------------------------------------------------------------------------------------
void SAL_CALL SvtFolderPicker::setDescription( const ::rtl::OUString& aDescription )
    throw( RuntimeException )
{
    m_aDescription = aDescription;
}

//------------------------------------------------------------------------------------
// XServiceInfo
//------------------------------------------------------------------------------------

/* XServiceInfo */
::rtl::OUString SAL_CALL SvtFolderPicker::getImplementationName() throw( RuntimeException )
{
    return impl_getStaticImplementationName();
}

/* XServiceInfo */
sal_Bool SAL_CALL SvtFolderPicker::supportsService( const ::rtl::OUString& sServiceName ) throw( RuntimeException )
{
    Sequence< ::rtl::OUString > seqServiceNames = getSupportedServiceNames();
    const ::rtl::OUString* pArray = seqServiceNames.getConstArray();
    for ( sal_Int32 i = 0; i < seqServiceNames.getLength(); i++ )
    {
        if ( sServiceName == pArray[i] )
        {
            return sal_True ;
        }
    }
    return sal_False ;
}

/* XServiceInfo */
Sequence< ::rtl::OUString > SAL_CALL SvtFolderPicker::getSupportedServiceNames() throw( RuntimeException )
{
    return impl_getStaticSupportedServiceNames();
}

/* Helper for XServiceInfo */
Sequence< ::rtl::OUString > SvtFolderPicker::impl_getStaticSupportedServiceNames()
{
    Sequence< ::rtl::OUString > seqServiceNames( 2 );
    ::rtl::OUString* pArray = seqServiceNames.getArray();
    pArray[0] = ::rtl::OUString::createFromAscii( "com.sun.star.ui.dialogs.FolderPicker" );
    pArray[1] = ::rtl::OUString::createFromAscii( "com.sun.star.ui.dialogs.OfficeFolderPicker" );
    return seqServiceNames ;
}

/* Helper for XServiceInfo */
::rtl::OUString SvtFolderPicker::impl_getStaticImplementationName()
{
    return ::rtl::OUString::createFromAscii( "com.sun.star.svtools.OfficeFolderPicker" );
}

/* Helper for registry */
Reference< XInterface > SAL_CALL SvtFolderPicker::impl_createInstance( const Reference< XComponentContext >& rxContext )
    throw( Exception )
{
    Reference< XMultiServiceFactory > xServiceManager (rxContext->getServiceManager(), UNO_QUERY_THROW);
    return Reference< XInterface >( *new SvtFolderPicker( xServiceManager ) );
}
