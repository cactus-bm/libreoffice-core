/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: OfficeFolderPicker.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 23:29:40 $
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
#ifndef INCLUDED_SVT_FOLDERPICKER_HXX
#define INCLUDED_SVT_FOLDERPICKER_HXX

#ifndef  _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif

#ifndef  _COM_SUN_STAR_UI_DIALOGS_XFOLDERPICKER_HPP_
#include <com/sun/star/ui/dialogs/XFolderPicker.hpp>
#endif
#ifndef  _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef  _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_XCOMPONENTCONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif

#ifndef SVTOOLS_COMMONPICKER_HXX
#include "commonpicker.hxx"
#endif


// class SvtFolderPicker ---------------------------------------------------

typedef ::cppu::ImplHelper2 <   ::com::sun::star::ui::dialogs::XFolderPicker
                            ,   ::com::sun::star::lang::XServiceInfo
                            >   SvtFolderPicker_Base;

class SvtFolderPicker   :public SvtFolderPicker_Base
                        ,public ::svt::OCommonPicker
{
private:
    ::rtl::OUString         m_aDescription;

public:
                                    SvtFolderPicker( const ::com::sun::star::uno::Reference < ::com::sun::star::lang::XMultiServiceFactory >& xFactory );
    virtual                        ~SvtFolderPicker();

    //------------------------------------------------------------------------------------
    // disambiguate XInterface
    //------------------------------------------------------------------------------------
    DECLARE_XINTERFACE( )

    //------------------------------------------------------------------------------------
    // disambiguate XTypeProvider
    //------------------------------------------------------------------------------------
    DECLARE_XTYPEPROVIDER( )

    //------------------------------------------------------------------------------------
    // XFolderPicker functions
    //------------------------------------------------------------------------------------

    virtual void SAL_CALL           setDisplayDirectory( const ::rtl::OUString& aDirectory ) throw( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );
    virtual ::rtl::OUString SAL_CALL    getDisplayDirectory() throw( ::com::sun::star::uno::RuntimeException );
    virtual ::rtl::OUString SAL_CALL    getDirectory() throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL           setDescription( const ::rtl::OUString& aDescription ) throw ( ::com::sun::star::uno::RuntimeException );

    //------------------------------------------------------------------------------------
    // XExecutableDialog functions
    //------------------------------------------------------------------------------------
    virtual void SAL_CALL setTitle( const ::rtl::OUString& _rTitle ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int16 SAL_CALL execute(  ) throw (::com::sun::star::uno::RuntimeException);

    //------------------------------------------------------------------------------------
    // XServiceInfo functions
    //------------------------------------------------------------------------------------

    /* XServiceInfo */
    virtual ::rtl::OUString SAL_CALL    getImplementationName() throw( ::com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL       supportsService( const ::rtl::OUString& sServiceName ) throw( ::com::sun::star::uno::RuntimeException );
    virtual com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
                                    getSupportedServiceNames() throw( ::com::sun::star::uno::RuntimeException );

    /* Helper for XServiceInfo */
    static com::sun::star::uno::Sequence< ::rtl::OUString > impl_getStaticSupportedServiceNames();
    static ::rtl::OUString impl_getStaticImplementationName();

    /* Helper for registry */
    static ::com::sun::star::uno::Reference< com::sun::star::uno::XInterface > SAL_CALL impl_createInstance (
        const ::com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >& rxContext )
        throw( com::sun::star::uno::Exception );

protected:
    //------------------------------------------------------------------------------------
    // OCommonPicker overridables
    //------------------------------------------------------------------------------------
    virtual SvtFileDialog*  implCreateDialog( Window* _pParent );
    virtual sal_Int16       implExecutePicker( );
};

#endif // INCLUDED_SVT_FOLDERPICKER_HXX
