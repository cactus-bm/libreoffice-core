/*************************************************************************
 *
 *  $RCSfile: xmlgrhlp.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $
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

#ifndef _DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _SV_CVTGRF_HXX
#include <vcl/cvtgrf.hxx>
#endif
#ifndef _SV_GFXLINK_HXX
#include <vcl/gfxlink.hxx>
#endif

#include "impgrf.hxx"
#include "xmlgrhlp.hxx"

// -----------
// - Defines -
// -----------

#define XML_GRAPHICSTORAGE_NAME     "Pictures"
#define XML_PACKAGE_URL_BASE        "vnd.sun.star.Package:"
#define XML_GRAPHICOBJECT_URL_BASE  "vnd.sun.star.GraphicObject:"

// ----------------------
// - SvXMLGraphicHelper -
// ----------------------

SvXMLGraphicHelper::SvXMLGraphicHelper() :
    ::cppu::WeakComponentImplHelper1< ::com::sun::star::container::XIndexContainer >( maMutex )
{
}

// -----------------------------------------------------------------------------

SvXMLGraphicHelper::~SvXMLGraphicHelper()
{
}

// -----------------------------------------------------------------------------

sal_Bool SvXMLGraphicHelper::ImplGetStreamNames( const ::rtl::OUString& rURLStr,
                                                 ::rtl::OUString& rPictureStorageName,
                                                 ::rtl::OUString& rPictureStreamName )
{
    String      aURLStr( rURLStr );
    sal_Bool    bRet = sal_False;

    if( aURLStr.Len() )
    {
        aURLStr = aURLStr.GetToken( aURLStr.GetTokenCount( ':' ) - 1, ':' );
        const sal_uInt32 nTokenCount = aURLStr.GetTokenCount( '/' );

        if( 1 == nTokenCount )
        {
            rPictureStorageName = String( RTL_CONSTASCII_USTRINGPARAM( XML_GRAPHICSTORAGE_NAME ) );
            rPictureStreamName = aURLStr;
            bRet = sal_True;
        }
        else if( 2 == nTokenCount )
        {
            rPictureStorageName = aURLStr.GetToken( 0, '/' );

            if( rPictureStorageName.getLength() && rPictureStorageName.getStr()[ 0 ] == '#' )
                rPictureStorageName = rPictureStorageName.copy( 1 );

            rPictureStreamName = aURLStr.GetToken( 1, '/' );
            bRet = sal_True;
        }
        else
        {
            DBG_ERROR( "SvXMLGraphicHelper::ImplInsertGraphicURL: invalid scheme" );
        }
    }

    return bRet;
}

// -----------------------------------------------------------------------------

SvStorageRef SvXMLGraphicHelper::ImplGetGraphicStorage( const ::rtl::OUString& rStorageName )
{
    if( !mxGraphicStorage.Is() || ( rStorageName != maCurStorageName ) )
    {
        if( mxGraphicStorage.Is() && GRAPHICHELPER_MODE_WRITE == meCreateMode )
            mxGraphicStorage->Commit();
        mxGraphicStorage = mpRootStorage->OpenUCBStorage( maCurStorageName = rStorageName, STREAM_READ | STREAM_WRITE );
    }

    return mxGraphicStorage;
}

// -----------------------------------------------------------------------------

SvStorageStreamRef SvXMLGraphicHelper::ImplGetGraphicStream( const ::rtl::OUString& rPictureStorageName,
                                                             const ::rtl::OUString& rPictureStreamName,
                                                             BOOL bTruncate )
{
    SvStorageStreamRef  xStm;
    SvStorageRef        xStorage( ImplGetGraphicStorage( rPictureStorageName ) );

    if( xStorage.Is() )
    {
        xStm = xStorage->OpenStream( rPictureStreamName,
                                     STREAM_READ |
                                     ( ( GRAPHICHELPER_MODE_WRITE == meCreateMode ) ?
                                       ( STREAM_WRITE | ( bTruncate ? STREAM_TRUNC : 0 ) ) : 0 ) );
    }

    return xStm;
}

// -----------------------------------------------------------------------------

Graphic SvXMLGraphicHelper::ImplReadGraphic( const ::rtl::OUString& rPictureStorageName,
                                             const ::rtl::OUString& rPictureStreamName )
{
    Graphic             aGraphic;
    SvStorageStreamRef  xStm( ImplGetGraphicStream( rPictureStorageName, rPictureStreamName, FALSE ) );

    if( xStm.Is() )
        GetGrfFilter()->ImportGraphic( aGraphic, String(), *xStm );

    return aGraphic;
}

// -----------------------------------------------------------------------------

sal_Bool SvXMLGraphicHelper::ImplWriteGraphic( const ::rtl::OUString& rPictureStorageName,
                                               const ::rtl::OUString& rPictureStreamName,
                                               const ::rtl::OUString& rGraphicId )
{
    String arGraphicId( rGraphicId );
    GraphicObject   aGrfObject( ByteString( arGraphicId, RTL_TEXTENCODING_ASCII_US ) );
    sal_Bool        bRet = sal_False;

    if( aGrfObject.GetType() != GRAPHIC_NONE )
    {
        SvStorageStreamRef xStm( ImplGetGraphicStream( rPictureStorageName, rPictureStreamName, TRUE ) );

        if( xStm.Is() )
        {
            Graphic         aGraphic( (Graphic&) aGrfObject.GetGraphic() );
            const GfxLink   aGfxLink( aGraphic.GetLink() );

            if( aGfxLink.GetDataSize() )
                xStm->Write( aGfxLink.GetData(), aGfxLink.GetDataSize() );
            else
            {
                if( aGraphic.GetType() == GRAPHIC_BITMAP )
                {
                    GraphicFilter*  pFilter = GetGrfFilter();
                    String          aFormat;

                    if( aGraphic.IsAnimated() )
                        aFormat = String( RTL_CONSTASCII_USTRINGPARAM( "gif" ) );
                    else
                        aFormat = String( RTL_CONSTASCII_USTRINGPARAM( "png" ) );

                    bRet = ( pFilter->ExportGraphic( aGraphic, String(), *xStm,
                                                     pFilter->GetExportFormatNumberForShortName( aFormat ) ) == 0 );
                }
                else if( aGraphic.GetType() == GRAPHIC_GDIMETAFILE )
                {
                    ( (GDIMetaFile&) aGraphic.GetGDIMetaFile() ).Write( *xStm );
                    bRet = ( xStm->GetError() == 0 );
                }
            }

            xStm->Commit();
        }
    }

    return bRet;
}

// -----------------------------------------------------------------------------

void SvXMLGraphicHelper::ImplInsertGraphicURL( const ::rtl::OUString& rURLStr, sal_uInt32 nInsertPos )
{
    ::rtl::OUString aPictureStorageName, aPictureStreamName;

    if( ( maURLSet.find( rURLStr ) != maURLSet.end() ) )
    {
        URLPairVector::iterator aIter( maGrfURLs.begin() ), aEnd( maGrfURLs.end() );

        while( aIter != aEnd )
        {
            if( rURLStr == (*aIter).first )
            {
                maGrfURLs[ nInsertPos ].second = (*aIter).second;
                aIter = aEnd;
            }
            else
                aIter++;
        }
    }
    else if( ImplGetStreamNames( rURLStr, aPictureStorageName, aPictureStreamName ) )
    {
        URLPair& rURLPair = maGrfURLs[ nInsertPos ];

        if( GRAPHICHELPER_MODE_READ == meCreateMode )
        {
            const GraphicObject aObj( ImplReadGraphic( aPictureStorageName, aPictureStreamName ) );

            if( aObj.GetType() != GRAPHIC_NONE )
            {
                const static ::rtl::OUString aBaseURL( RTL_CONSTASCII_USTRINGPARAM( XML_GRAPHICOBJECT_URL_BASE ) );

                maGrfObjs.push_back( aObj );
                rURLPair.second = aBaseURL;
                rURLPair.second += String( aObj.GetUniqueID().GetBuffer(), RTL_TEXTENCODING_ASCII_US );
            }
            else
                rURLPair.second = String();
        }
        else
        {
            const String        aGraphicObjectId( aPictureStreamName );
            const GraphicObject aGrfObject( ByteString( aGraphicObjectId, RTL_TEXTENCODING_ASCII_US ) );

            if( aGrfObject.GetType() != GRAPHIC_NONE )
            {
                String aStreamName( aGraphicObjectId );

                if( aGrfObject.GetType() == GRAPHIC_BITMAP )
                {
                    if( aGrfObject.IsAnimated() )
                        aStreamName += String( RTL_CONSTASCII_USTRINGPARAM( ".gif" ) );
                    else
                        aStreamName += String( RTL_CONSTASCII_USTRINGPARAM( ".png" ) );
                }
                else if( aGrfObject.GetType() == GRAPHIC_GDIMETAFILE )
                    aStreamName += String( RTL_CONSTASCII_USTRINGPARAM( ".svm" ) );

                if( mbDirect && aStreamName.Len() )
                    ImplWriteGraphic( aPictureStorageName, aStreamName, aGraphicObjectId );

                rURLPair.second = String( RTL_CONSTASCII_USTRINGPARAM( "#Pictures/" ) );
                rURLPair.second += aStreamName;
            }
        }

        maURLSet.insert( rURLStr );
    }
}

// -----------------------------------------------------------------------------

void SvXMLGraphicHelper::Init( SvStorage& rXMLStorage,
                               SvXMLGraphicHelperMode eCreateMode,
                               BOOL bDirect )
{
    mpRootStorage = &rXMLStorage;
    meCreateMode = eCreateMode;
    mbDirect = bDirect;
}

// -----------------------------------------------------------------------------

SvXMLGraphicHelper* SvXMLGraphicHelper::Create( SvStorage& rXMLStorage,
                                                SvXMLGraphicHelperMode eCreateMode,
                                                BOOL bDirect )
{
    SvXMLGraphicHelper* pThis = new SvXMLGraphicHelper;

    pThis->acquire();
    pThis->Init( rXMLStorage, eCreateMode, bDirect );

    return pThis;
}

// -----------------------------------------------------------------------------

void SvXMLGraphicHelper::Destroy( SvXMLGraphicHelper* pSvXMLGraphicHelper )
{
    if( pSvXMLGraphicHelper )
    {
        pSvXMLGraphicHelper->Flush();
        pSvXMLGraphicHelper->release();
    }
}

// -----------------------------------------------------------------------------

void SvXMLGraphicHelper::Flush()
{
    if( ( GRAPHICHELPER_MODE_WRITE == meCreateMode ) && !mbDirect )
    {
        ::rtl::OUString     aPictureStorageName, aPictureStreamName;
        URLSet::iterator    aSetIter( maURLSet.begin() ), aSetEnd( maURLSet.end() );

        while( aSetIter != aSetEnd )
        {
            URLPairVector::iterator aPairIter( maGrfURLs.begin() ), aPairEnd( maGrfURLs.end() );

            while( aPairIter != aPairEnd )
            {
                if( *aSetIter == (*aPairIter).first )
                {
                    if( ImplGetStreamNames( (*aPairIter).second, aPictureStorageName, aPictureStreamName ) )
                    {
                        DBG_ASSERT( String( aPictureStreamName ).GetTokenCount( '.' ) == 2, "invalid URL" );
                        ImplWriteGraphic( aPictureStorageName, aPictureStreamName, String( aPictureStreamName ).GetToken( 0, '.' ) );
                    }

                    aPairIter = aPairEnd;
                }
                else
                    aPairIter++;
            }

            aSetIter++;
        }
    }
    if( GRAPHICHELPER_MODE_WRITE == meCreateMode )
    {
        SvStorageRef xStorage = ImplGetGraphicStorage( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(XML_GRAPHICSTORAGE_NAME) ) );

        if( xStorage.Is() )
            xStorage->Commit();
    }

}

// -----------------------------------------------------------------------------

void SAL_CALL SvXMLGraphicHelper::insertByIndex( sal_Int32 nIndex, const ::com::sun::star::uno::Any& rElement )
    throw(  ::com::sun::star::lang::IllegalArgumentException,
            ::com::sun::star::lang::IndexOutOfBoundsException,
            ::com::sun::star::lang::WrappedTargetException,
            ::com::sun::star::uno::RuntimeException )
{
    ::osl::MutexGuard   aGuard( maMutex );
    ::rtl::OUString     aStr;

    rElement >>= aStr;

    if( aStr.getLength() )
    {
        if( nIndex == (sal_Int32) maGrfURLs.size() )
            maGrfURLs.push_back( ::_STL::make_pair( aStr, ::rtl::OUString() ) );
        else
        {
             if( nIndex > (sal_Int32) maGrfURLs.size() )
                maGrfURLs.resize( nIndex + 1 );

            maGrfURLs[ nIndex ] = ::_STL::make_pair( aStr, ::rtl::OUString() );
        }

        ImplInsertGraphicURL( aStr, nIndex );
    }
}

// -----------------------------------------------------------------------------

void SAL_CALL SvXMLGraphicHelper::removeByIndex( sal_Int32 nIndex )
    throw(  ::com::sun::star::lang::IndexOutOfBoundsException,
            ::com::sun::star::lang::WrappedTargetException,
            ::com::sun::star::uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( maMutex );

    DBG_ASSERT( nIndex < getCount(), "invalid Index" );
    maGrfURLs.erase( maGrfURLs.begin() + nIndex );
}

// -----------------------------------------------------------------------------

void SAL_CALL SvXMLGraphicHelper::replaceByIndex( sal_Int32 nIndex, const ::com::sun::star::uno::Any& rElement )
    throw(  ::com::sun::star::lang::IllegalArgumentException,
            ::com::sun::star::lang::IndexOutOfBoundsException,
            ::com::sun::star::lang::WrappedTargetException,
            ::com::sun::star::uno::RuntimeException )
{
    ::osl::MutexGuard   aGuard( maMutex );
    ::rtl::OUString     aStr;

    rElement >>= aStr;

    if( aStr.getLength() )
    {
        DBG_ASSERT( nIndex < getCount(), "invalid Index" );
        maGrfURLs[ nIndex ] = ::_STL::make_pair( aStr, ::rtl::OUString() );
        ImplInsertGraphicURL( aStr, nIndex );
    }
}

// -----------------------------------------------------------------------------

sal_Int32 SAL_CALL SvXMLGraphicHelper::getCount()
    throw(  ::com::sun::star::uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( maMutex );
    return maGrfURLs.size();
}

// -----------------------------------------------------------------------------

::com::sun::star::uno::Any SAL_CALL SvXMLGraphicHelper::getByIndex( sal_Int32 nIndex )
    throw(  ::com::sun::star::lang::IndexOutOfBoundsException,
            ::com::sun::star::lang::WrappedTargetException,
            ::com::sun::star::uno::RuntimeException )
{
    ::osl::MutexGuard           aGuard( maMutex );
    ::com::sun::star::uno::Any  aAny;

    DBG_ASSERT( nIndex < getCount(), "invalid Index" );
    aAny <<= maGrfURLs[ nIndex ].second;

    return aAny;
}

// -----------------------------------------------------------------------------

::com::sun::star::uno::Type SAL_CALL SvXMLGraphicHelper::getElementType()
    throw(  ::com::sun::star::uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( maMutex );
    return ::getCppuType( (const ::rtl::OUString*) 0 );
}

// -----------------------------------------------------------------------------

sal_Bool SAL_CALL SvXMLGraphicHelper::hasElements()
    throw(  ::com::sun::star::uno::RuntimeException )
{
    ::osl::MutexGuard aGuard( maMutex );
    return maGrfURLs.size() > 0;
}
