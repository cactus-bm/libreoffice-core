/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: OfficeFilePicker.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 23:28:56 $
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
#ifndef INCLUDED_SVT_FILEPICKER_HXX
#define INCLUDED_SVT_FILEPICKER_HXX

#ifndef  _CPPUHELPER_IMPLBASE6_HXX_
#include <cppuhelper/implbase6.hxx>
#endif

#ifndef  _COM_SUN_STAR_UI_DIALOGS_XFILEPICKERCONTROLACCESS_HPP_
#include <com/sun/star/ui/dialogs/XFilePickerControlAccess.hpp>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_XFILEPICKERNOTIFIER_HPP_
#include <com/sun/star/ui/dialogs/XFilePickerNotifier.hpp>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_XFILEPREVIEW_HPP_
#include <com/sun/star/ui/dialogs/XFilePreview.hpp>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_XFILTERMANAGER_HPP_
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILTERGROUPMANAGER_HPP_
#include <com/sun/star/ui/dialogs/XFilterGroupManager.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILEPICKERLISTENER_HPP_
#include <com/sun/star/ui/dialogs/XFilePickerListener.hpp>
#endif

#ifndef  _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XEVENTLISTENER_HPP_
#include <com/sun/star/lang/XEventListener.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_XCOMPONENT_CONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif


#ifndef _SV_WINTYPES_HXX
#include <vcl/wintypes.hxx>
#endif

#ifndef SVTOOLS_COMMONPICKER_HXX
#include "commonpicker.hxx"
#endif
#ifndef SVTOOLS_PICKER_CALLBACKS_HXX
#include "pickercallbacks.hxx"
#endif

#include <list>

struct FilterEntry;
struct ElementEntry_Impl;

typedef ::std::list< FilterEntry >                          FilterList;     // can be maintained more effectively
typedef ::std::list < ElementEntry_Impl >                   ElementList;

typedef ::com::sun::star::beans::StringPair                 UnoFilterEntry;
typedef ::com::sun::star::uno::Sequence< UnoFilterEntry >   UnoFilterList;  // can be transported more effectively

// class SvtFilePicker ---------------------------------------------------

typedef ::cppu::ImplHelper6 <   ::com::sun::star::ui::dialogs::XFilePickerControlAccess
                            ,   ::com::sun::star::ui::dialogs::XFilePickerNotifier
                            ,   ::com::sun::star::ui::dialogs::XFilePreview
                            ,   ::com::sun::star::ui::dialogs::XFilterManager
                            ,   ::com::sun::star::ui::dialogs::XFilterGroupManager
                            ,   ::com::sun::star::lang::XServiceInfo
                            >   SvtFilePicker_Base;

class SvtFilePicker :public SvtFilePicker_Base
                    ,public ::svt::OCommonPicker
                    ,public ::svt::IFilePickerListener
{
private:
    FilterList*         m_pFilterList;
    ElementList*        m_pElemList;

    sal_Bool            m_bMultiSelection;
    sal_Int16           m_nServiceType;
    ::rtl::OUString     m_aDefaultName;
    ::rtl::OUString     m_aCurrentFilter;

    // #97148# --------------
    ::rtl::OUString     m_aOldDisplayDirectory;
    ::rtl::OUString     m_aOldHideDirectory;

    ::com::sun::star::uno::Reference< ::com::sun::star::ui::dialogs::XFilePickerListener >
                        m_xListener;

public:
                       SvtFilePicker( const ::com::sun::star::uno::Reference < ::com::sun::star::lang::XMultiServiceFactory >& xFactory );
    virtual           ~SvtFilePicker();

    //------------------------------------------------------------------------------------
    // disambiguate XInterface
    //------------------------------------------------------------------------------------
    DECLARE_XINTERFACE( )

    //------------------------------------------------------------------------------------
    // disambiguate XTypeProvider
    //------------------------------------------------------------------------------------
    DECLARE_XTYPEPROVIDER( )

    //------------------------------------------------------------------------------------
    // XExecutableDialog functions
    //------------------------------------------------------------------------------------
    virtual void SAL_CALL setTitle( const ::rtl::OUString& _rTitle ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int16 SAL_CALL execute(  ) throw (::com::sun::star::uno::RuntimeException);

    //------------------------------------------------------------------------------------
    // XFilePicker functions
    //------------------------------------------------------------------------------------

    virtual void SAL_CALL           setMultiSelectionMode( sal_Bool bMode ) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL           setDefaultName( const ::rtl::OUString& aName ) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL           setDisplayDirectory( const ::rtl::OUString& aDirectory ) throw( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );
    virtual ::rtl::OUString SAL_CALL    getDisplayDirectory() throw( ::com::sun::star::uno::RuntimeException );
    virtual com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getFiles() throw( ::com::sun::star::uno::RuntimeException );

    //------------------------------------------------------------------------------------
    // XFilePickerControlAccess functions
    //------------------------------------------------------------------------------------

    virtual void SAL_CALL           setValue( sal_Int16 ElementID, sal_Int16 ControlAction, const com::sun::star::uno::Any& value ) throw( ::com::sun::star::uno::RuntimeException );
    virtual com::sun::star::uno::Any SAL_CALL           getValue( sal_Int16 ElementID, sal_Int16 ControlAction ) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL           setLabel( sal_Int16 ElementID, const ::rtl::OUString& aValue ) throw ( ::com::sun::star::uno::RuntimeException );
    virtual ::rtl::OUString SAL_CALL    getLabel( sal_Int16 ElementID ) throw ( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL           enableControl( sal_Int16 ElementID, sal_Bool bEnable ) throw( ::com::sun::star::uno::RuntimeException );

    //------------------------------------------------------------------------------------
    // XFilePickerNotifier functions
    //------------------------------------------------------------------------------------

    virtual void SAL_CALL           addFilePickerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::ui::dialogs::XFilePickerListener >& xListener ) throw ( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL           removeFilePickerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::ui::dialogs::XFilePickerListener >& xListener ) throw ( ::com::sun::star::uno::RuntimeException );

    //------------------------------------------------------------------------------------
    // XFilePreview functions
    //------------------------------------------------------------------------------------

    virtual com::sun::star::uno::Sequence< sal_Int16 > SAL_CALL getSupportedImageFormats() throw ( ::com::sun::star::uno::RuntimeException );
    virtual sal_Int32 SAL_CALL      getTargetColorDepth() throw ( ::com::sun::star::uno::RuntimeException );
    virtual sal_Int32 SAL_CALL      getAvailableWidth() throw ( ::com::sun::star::uno::RuntimeException );
    virtual sal_Int32 SAL_CALL      getAvailableHeight() throw ( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL           setImage( sal_Int16 aImageFormat, const com::sun::star::uno::Any& aImage ) throw ( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL       setShowState( sal_Bool bShowState ) throw ( ::com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL       getShowState() throw ( ::com::sun::star::uno::RuntimeException );

    //------------------------------------------------------------------------------------
    // XFilterManager functions
    //------------------------------------------------------------------------------------

    virtual void SAL_CALL           appendFilter( const ::rtl::OUString& aTitle, const ::rtl::OUString& aFilter ) throw( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL           setCurrentFilter( const ::rtl::OUString& aTitle ) throw( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );
    virtual ::rtl::OUString SAL_CALL    getCurrentFilter() throw( ::com::sun::star::uno::RuntimeException );

    //------------------------------------------------------------------------------------
    // XFilterGroupManager functions
    //------------------------------------------------------------------------------------
    virtual void SAL_CALL           appendFilterGroup( const ::rtl::OUString& sGroupTitle, const com::sun::star::uno::Sequence< com::sun::star::beans::StringPair >& aFilters ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);

    //------------------------------------------------------------------------------------
    // XInitialization functions
    //------------------------------------------------------------------------------------

    virtual void SAL_CALL           initialize( const com::sun::star::uno::Sequence< com::sun::star::uno::Any >& aArguments ) throw ( com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException );

    //------------------------------------------------------------------------------------
    // XServiceInfo functions
    //------------------------------------------------------------------------------------

    /* XServiceInfo */
    virtual ::rtl::OUString SAL_CALL    getImplementationName() throw( ::com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL       supportsService( const ::rtl::OUString& sServiceName ) throw( ::com::sun::star::uno::RuntimeException );
    virtual com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
                                    getSupportedServiceNames() throw( ::com::sun::star::uno::RuntimeException );

    /* Helper for XServiceInfo */
    static com::sun::star::uno::Sequence< ::rtl::OUString >
                                    impl_getStaticSupportedServiceNames();
    static ::rtl::OUString          impl_getStaticImplementationName();

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
    virtual sal_Bool        implHandleInitializationArgument(
                                const ::rtl::OUString& _rName,
                                const ::com::sun::star::uno::Any& _rValue
                            )
                            SAL_THROW( ( ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException ) );

private:
    WinBits             getWinBits( WinBits& rExtraBits );
    virtual void        notify( sal_Int16 _nEventId, sal_Int16 _nControlId );

    sal_Bool            FilterNameExists( const ::rtl::OUString& rTitle );
    sal_Bool            FilterNameExists( const UnoFilterList& _rGroupedFilters );

    void                ensureFilterList( const ::rtl::OUString& _rInitialCurrentFilter );
};

#endif // INCLUDED_SVT_FILEPICKER_HXX

