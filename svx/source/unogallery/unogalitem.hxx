/*************************************************************************
 *
 *  $RCSfile: unogalitem.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2004-08-31 14:55:35 $
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

#ifndef _SVX_UNOGALITEM_HXX
#define _SVX_UNOGALITEM_HXX

#ifndef SVX_UNOMODEL_HXX
#include "unomodel.hxx"
#endif
#ifndef _UTL_SERVICEHELPER_HXX_
#include <comphelper/servicehelper.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_GALLERY_XGALLERYITEM_HPP_
#include <com/sun/star/gallery/XGalleryItem.hpp>
#endif
#ifndef _COMPHELPER_PROPERTYSETHELPER_HXX_
#include <comphelper/propertysethelper.hxx>
#endif
#ifndef _COMPHELPER_PROPERTYSETINFO_HXX_
#include <comphelper/propertysetinfo.hxx>
#endif

class GalleryTheme;
struct GalleryObject;
namespace unogallery { class GalleryTheme; }

namespace unogallery {

// ---------------
// - GalleryItem -
// ---------------

class GalleryItem : public ::cppu::OWeakAggObject,
                    public ::com::sun::star::lang::XServiceInfo,
                    public ::com::sun::star::lang::XTypeProvider,
                    public ::com::sun::star::gallery::XGalleryItem,
                    public ::comphelper::PropertySetHelper
{
    friend class ::unogallery::GalleryTheme;

public:

            GalleryItem( ::unogallery::GalleryTheme& rTheme, const GalleryObject& rObject );
            ~GalleryItem() throw();

    bool    isValid() const;

    static ::rtl::OUString getImplementationName_Static() throw();
    static ::com::sun::star::uno::Sequence< ::rtl::OUString >  getSupportedServiceNames_Static() throw();

protected:

    // XInterface
    virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL acquire() throw();
    virtual void SAL_CALL release() throw();

    // XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL supportsService( const rtl::OUString& ServiceName ) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames() throw( ::com::sun::star::uno::RuntimeException );

    // XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  ) throw(::com::sun::star::uno::RuntimeException);

    // XGalleryItem
    virtual ::sal_Int8 SAL_CALL getType(  ) throw (::com::sun::star::uno::RuntimeException);

    // PropertySetHelper
    virtual void _setPropertyValues( const comphelper::PropertyMapEntry** ppEntries, const ::com::sun::star::uno::Any* pValues ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException );
    virtual void _getPropertyValues( const comphelper::PropertyMapEntry** ppEntries, ::com::sun::star::uno::Any* pValue ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException );

protected:

    ::comphelper::PropertySetInfo* createPropertySetInfo();

private:

    ::unogallery::GalleryTheme* mpTheme;
    const ::GalleryObject*      mpGalleryObject;

    const ::GalleryObject*      implGetObject() const;
    void                        implSetInvalid();

                                // not available
                                GalleryItem();
                                GalleryItem( const GalleryItem& );
                                GalleryItem& operator=( const GalleryItem& );
};

// -----------------------
// - GalleryDrawingModel -
// -----------------------

class GalleryDrawingModel : public SvxUnoDrawingModel
{
public:

                GalleryDrawingModel( SdrModel* pDoc ) throw();
    virtual     ~GalleryDrawingModel() throw();

                UNO3_GETIMPLEMENTATION_DECL( GalleryDrawingModel )
};

}

#endif
