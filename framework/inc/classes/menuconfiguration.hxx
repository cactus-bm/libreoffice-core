/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: menuconfiguration.hxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:06:27 $
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

#ifndef __FRAMEWORK_CLASSES_MENUCONFIGURATION_HXX_
#define __FRAMEWORK_CLASSES_MENUCONFIGURATION_HXX_

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_WRAPPEDTARGETEXCEPTION_HPP_
#include <com/sun/star/lang/WrappedTargetException.hpp>
#endif

#ifndef _COM_SUN_STAR_IO_XINPUTSTREAM_HPP_
#include <com/sun/star/io/XInputStream.hpp>
#endif

#ifndef _COM_SUN_STAR_IO_XOUTPUTSTREAM_HPP_
#include <com/sun/star/io/XOutputStream.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

//_________________________________________________________________________________________________________________
//  includes of other projects
//_________________________________________________________________________________________________________________

#include <vcl/menu.hxx>
#include <vcl/toolbox.hxx>

#define BOOKMARK_NEWMENU        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "private:menu_bookmark_new" ))
#define BOOKMARK_WIZARDMENU     ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "private:menu_bookmark_wizard" ))
#define ADDONS_POPUPMENU        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "private:menu_addons_popup" ))

// Prepare for inclusion by framework and sfx
// Please consider that there is a corresponding define also in sfxsids.hrc!! (SID_SFX_START)/(SID_ADDONS)
#define FWK_SID_SFX_START 5000
#define FWK_SID_ADDONS (FWK_SID_SFX_START+1678)
#define FWK_SID_ADDONHELP (FWK_SID_SFX_START+1684)

const USHORT START_ITEMID_PICKLIST      = 4500;
const USHORT END_ITEMID_PICKLIST        = 4599;
const USHORT MAX_ITEMCOUNT_PICKLIST     =   99; // difference between START_... & END_... for picklist / must be changed too, if these values are changed!
const USHORT START_ITEMID_WINDOWLIST    = 4600;
const USHORT END_ITEMID_WINDOWLIST      = 4699;
const USHORT ITEMID_ADDONLIST           = FWK_SID_ADDONS;
const USHORT ITEMID_ADDONHELP           = FWK_SID_ADDONHELP;

namespace framework
{

class MenuConfiguration
{
    public:
        struct Attributes
        {
            Attributes( const ::rtl::OUString& aFrame, const ::rtl::OUString& aImageIdStr ) :
                aTargetFrame( aFrame ), aImageId( aImageIdStr ) {}

            ::rtl::OUString aTargetFrame;
            ::rtl::OUString aImageId;
        };

        MenuConfiguration(
            // #110897#-1 use const when giving a uno reference by reference
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& rServiceManager );

        virtual ~MenuConfiguration();

        MenuBar* CreateMenuBarFromConfiguration(
            ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& rInputStream )
            throw ( ::com::sun::star::lang::WrappedTargetException );

        PopupMenu* CreateBookmarkMenu(
                ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame,
                const ::rtl::OUString& aURL )
            throw ( ::com::sun::star::lang::WrappedTargetException );

        ToolBox* CreateToolBoxFromConfiguration(
            ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& rInputStream )
            throw ( ::com::sun::star::lang::WrappedTargetException );

        void StoreMenuBar( MenuBar* pMenuBar,
                      ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream >& rOutputStream )
            throw ( ::com::sun::star::lang::WrappedTargetException );

        void StoreToolBox( ToolBox* pToolBox,
                      ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream >& rOutputStream )
            throw ( ::com::sun::star::lang::WrappedTargetException );

        static BOOL IsPickListItemId( USHORT nId );
        static BOOL IsWindowListItemId( USHORT nId );

    private:
        // #110897#-1 do not hold the uno reference by reference
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& m_rxServiceManager;
};

}

#endif // __FRAMEWORK_CLASSES_MENUCONFIGURATION_HXX_

