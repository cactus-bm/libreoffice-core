/*************************************************************************
 *
 *  $RCSfile: DOTransferable.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: tra $ $Date: 2001-03-22 14:15:51 $
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


#ifndef _DOTRANSFERABLE_HXX_
#define _DOTRANSFERABLE_HXX_

//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#ifndef _COM_SUN_STAR_DATATRANSFER_XTRANSFERABLE_HPP_
#include <com/sun/star/datatransfer/XTransferable.hpp>
#endif

#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif

#ifndef _DATAFMTTRANSL_HXX_
#include "DataFmtTransl.hxx"
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_XMIMECONTENTTYPEFACTORY_HPP_
#include <com/sun/star/datatransfer/XMimeContentTypeFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_XMIMECONTENTTYPE_HPP_
#include <com/sun/star/datatransfer/XMimeContentType.hpp>
#endif

#include <comdef.h>

//------------------------------------------------------------------------
// deklarations
//------------------------------------------------------------------------

// forward
class CDTransObjFactory;
class CFormatEtc;

class CDOTransferable : public ::cppu::WeakImplHelper1< ::com::sun::star::datatransfer::XTransferable >
{
public:
    typedef com::sun::star::uno::Sequence< sal_Int8 > ByteSequence_t;

    //------------------------------------------------------------------------
    // XTransferable
    //------------------------------------------------------------------------

    virtual ::com::sun::star::uno::Any SAL_CALL getTransferData( const ::com::sun::star::datatransfer::DataFlavor& aFlavor )
        throw( ::com::sun::star::datatransfer::UnsupportedFlavorException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::datatransfer::DataFlavor > SAL_CALL getTransferDataFlavors(  )
        throw( ::com::sun::star::uno::RuntimeException );

    virtual sal_Bool SAL_CALL isDataFlavorSupported( const ::com::sun::star::datatransfer::DataFlavor& aFlavor )
        throw( ::com::sun::star::uno::RuntimeException );

private:
    // should be created only by CDTransObjFactory
    explicit CDOTransferable(
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& ServiceManager,
        IDataObjectPtr rDataObject );

    //------------------------------------------------------------------------
    // some helper functions
    //------------------------------------------------------------------------

    void SAL_CALL initFlavorList( );

    void SAL_CALL addSupportedFlavor( const com::sun::star::datatransfer::DataFlavor& aFlavor );
    com::sun::star::datatransfer::DataFlavor SAL_CALL formatEtcToDataFlavor( const FORMATETC& aFormatEtc );

    ByteSequence_t SAL_CALL getClipboardData( CFormatEtc& aFormatEtc );
    rtl::OUString  SAL_CALL synthesizeUnicodeText( );

    void SAL_CALL clipDataToByteStream( STGMEDIUM stgmedium, ByteSequence_t& aByteSequence );

    ::com::sun::star::uno::Any SAL_CALL byteStreamToAny( ByteSequence_t& aByteStream, const com::sun::star::uno::Type& aRequestedDataType );
    rtl::OUString              SAL_CALL byteStreamToOUString( ByteSequence_t& aByteStream );

    LCID SAL_CALL getLocaleFromClipboard( );

    sal_Bool SAL_CALL compareDataFlavors( const com::sun::star::datatransfer::DataFlavor& lhs,
                                          const com::sun::star::datatransfer::DataFlavor& rhs );

    sal_Bool SAL_CALL cmpFullMediaType( const com::sun::star::uno::Reference< com::sun::star::datatransfer::XMimeContentType >& xLhs,
                                        const com::sun::star::uno::Reference< com::sun::star::datatransfer::XMimeContentType >& xRhs ) const;

    sal_Bool SAL_CALL cmpAllContentTypeParameter( const com::sun::star::uno::Reference< com::sun::star::datatransfer::XMimeContentType >& xLhs,
                                        const com::sun::star::uno::Reference< com::sun::star::datatransfer::XMimeContentType >& xRhs ) const;

private:
    IDataObjectPtr                                                                          m_rDataObject;
    com::sun::star::uno::Sequence< com::sun::star::datatransfer::DataFlavor >               m_FlavorList;
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >  m_SrvMgr;
    CDataFormatTranslator                                                                   m_DataFormatTranslator;
    com::sun::star::uno::Reference< com::sun::star::datatransfer::XMimeContentTypeFactory > m_rXMimeCntFactory;
    ::osl::Mutex                                                                            m_aMutex;
    sal_Bool                                                                                m_bUnicodeRegistered;
    CLIPFORMAT                                                                              m_TxtFormatOnClipboard;

// non supported operations
private:
    CDOTransferable( const CDOTransferable& );
    CDOTransferable& operator=( const CDOTransferable& );

    friend class CDTransObjFactory;
};

#endif