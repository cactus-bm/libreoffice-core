/*************************************************************************
 *
 *  $RCSfile: frmload.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: mba $ $Date: 2002-07-18 15:02:52 $
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

#ifndef _SFX_FRMLOAD_HXX
#define _SFX_FRMLOAD_HXX

#ifndef _COM_SUN_STAR_FRAME_XLOADEVENTLISTENER_HPP_
#include <com/sun/star/frame/XLoadEventListener.hpp>
#endif

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _COM_SUN_STAR_FRAME_XSYNCHRONOUSFRAMELOADER_HPP_
#include <com/sun/star/frame/XSynchronousFrameLoader.hpp>
#endif

#ifndef _COM_SUN_STAR_DOCUMENT_XEXTENDEDFILTERDETECTION_HPP_
#include <com/sun/star/document/XExtendedFilterDetection.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_EXCEPTION_HPP_
#include <com/sun/star/uno/Exception.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif

#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif

#ifndef _CPPUHELPER_IMPLBASE3_HXX_
#include <cppuhelper/implbase3.hxx>
#endif

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <cppuhelper/factory.hxx>
#include <tools/link.hxx>
#include <tools/string.hxx>

class SfxObjectFactory;
class SfxFilterMatcher;
class LoadEnvironment_Impl;
class SfxMedium;

namespace com
{
    namespace sun
    {
        namespace star
        {
            namespace uno
            {
                class Any;
            }
            namespace lang
            {
                class XMultiServiceFactory;
            }
            namespace frame
            {
                class XFrame;
            }
            namespace beans
            {
                struct PropertyValue;
            }
        }
    }
}

#include "sfxuno.hxx"

#define REFERENCE ::com::sun::star::uno::Reference
#define SEQUENCE ::com::sun::star::uno::Sequence
#define RUNTIME_EXCEPTION ::com::sun::star::uno::RuntimeException

class SfxFrameLoader_Impl : public ::cppu::WeakImplHelper3< ::com::sun::star::frame::XSynchronousFrameLoader, ::com::sun::star::document::XExtendedFilterDetection, ::com::sun::star::lang::XServiceInfo >
{
    REFERENCE < ::com::sun::star::frame::XFrame > xFrame;
    REFERENCE < ::com::sun::star::frame::XLoadEventListener > xListener;
    LoadEnvironment_Impl*   pLoader;
    SfxFilterMatcher*       pMatcher;
    String                  aFilterName;
    SfxMedium*              pMedium;
    sal_Bool                bLoadDone;
    sal_Bool                bLoadState;

    DECL_LINK( LoadDone_Impl, void* );

public:
                            SfxFrameLoader_Impl( const REFERENCE < ::com::sun::star::lang::XMultiServiceFactory >& xFactory );
    virtual                 ~SfxFrameLoader_Impl();

    SFX_DECL_XSERVICEINFO

    //----------------------------------------------------------------------------------
    // XSynchronousFrameLoader
    //----------------------------------------------------------------------------------
    virtual sal_Bool SAL_CALL load( const SEQUENCE< ::com::sun::star::beans::PropertyValue >& lDescriptor, const REFERENCE< ::com::sun::star::frame::XFrame >& xFrame ) throw( RUNTIME_EXCEPTION );
    virtual void SAL_CALL cancel() throw( RUNTIME_EXCEPTION );

    //----------------------------------------------------------------------------------
    // XExtendedFilterDetect
    //----------------------------------------------------------------------------------
    virtual ::rtl::OUString SAL_CALL detect( SEQUENCE< ::com::sun::star::beans::PropertyValue >& lDescriptor ) throw( RUNTIME_EXCEPTION );
};

#endif
