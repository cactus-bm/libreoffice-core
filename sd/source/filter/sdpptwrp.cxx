/*************************************************************************
 *
 *  $RCSfile: sdpptwrp.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: obo $ $Date: 2004-01-20 10:30:05 $
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

#include <sfx2/docfile.hxx>
#include <sfx2/docfilt.hxx>
#include <vos/module.hxx>
#include <sfx2/interno.hxx>
#include <svx/msoleexp.hxx>
#include <svx/svxmsbas.hxx>
#include <offmgr/app.hxx>
#include <offmgr/fltrcfg.hxx>

#include "sdpptwrp.hxx"
#include "pptin.hxx"
#include "drawdoc.hxx"

#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _MS_FILTERTRACER_HXX
#include <svx/msfiltertracer.hxx>
#endif

// --------------
// - Namespaces -
// --------------

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::task;
using namespace ::com::sun::star::frame;


typedef BOOL ( __LOADONCALLAPI *ExportPPT )( SvStorageRef&,
                                             Reference< XModel > &,
                                             Reference< XStatusIndicator > &,
                                             SvMemoryStream*, sal_uInt32 nCnvrtFlags );

// ---------------
// - SdPPTFilter -
// ---------------

SdPPTFilter::SdPPTFilter( SfxMedium& rMedium, ::sd::DrawDocShell& rDocShell, sal_Bool bShowProgress ) :
    SdFilter( rMedium, rDocShell, bShowProgress ),
    pBas    ( NULL )
{
}

// -----------------------------------------------------------------------------

SdPPTFilter::~SdPPTFilter()
{
    delete pBas;    // deleting the compressed basic storage
}

// -----------------------------------------------------------------------------

sal_Bool SdPPTFilter::Import()
{
    sal_Bool    bRet = sal_False;
    SvStorage*  pStorage;

    if( ( pStorage = mrMedium.GetStorage() ) != NULL )
    {
        /* check if there is a dualstorage, then the
        document is propably a PPT95 containing PPT97 */
        SvStorageRef xDualStorage;
        String sDualStorage( RTL_CONSTASCII_USTRINGPARAM( "PP97_DUALSTORAGE" ) );
        if ( pStorage->IsContained( sDualStorage ) )
        {
            xDualStorage = pStorage->OpenStorage( sDualStorage, STREAM_STD_READ );
            pStorage = xDualStorage;
        }
        SvStream* pDocStream = pStorage->OpenStream( String( RTL_CONSTASCII_USTRINGPARAM("PowerPoint Document") ), STREAM_STD_READ );
        if( pDocStream )
        {
            pDocStream->SetVersion( pStorage->GetVersion() );
            pDocStream->SetKey( pStorage->GetKey() );

            String aTraceConfigPath( RTL_CONSTASCII_USTRINGPARAM( "Office.Tracing/Import/PowerPoint" ) );
            Sequence< PropertyValue > aConfigData( 1 );
            PropertyValue aPropValue;
            aPropValue.Value <<= rtl::OUString( mrMedium.GetURLObject().GetMainURL( INetURLObject::NO_DECODE ) );
            aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DocumentURL" ) );
            aConfigData[ 0 ] = aPropValue;

            MSFilterTracer aTracer( aTraceConfigPath, &aConfigData );
            aTracer.StartTracing();
            SdPPTImport* pImport = new SdPPTImport( &mrDocument, *pDocStream, *pStorage, mrMedium, &aTracer );
            if ( !( bRet = pImport->Import() ) )
                mrMedium.SetError( SVSTREAM_WRONGVERSION );
            aTracer.EndTracing();
            delete pImport;
            delete pDocStream;
        }
    }

    return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool SdPPTFilter::Export()
{
    ::vos::OModule* pLibrary = OpenLibrary( mrMedium.GetFilter()->GetUserData() );
    sal_Bool        bRet = sal_False;

    if( pLibrary )
    {
        if( mxModel.is() )
        {
            SvStorageRef    xStorRef;
            ExportPPT       PPTExport = ( ExportPPT ) pLibrary->getSymbol( ::rtl::OUString::createFromAscii("ExportPPT") );

            /* !!!
            if ( pViewShell && pViewShell->GetView() )
                pViewShell->GetView()->EndTextEdit();
            */
            if( PPTExport && ( xStorRef = mrMedium.GetOutputStorage() ).Is() )
            {
                sal_uInt32          nCnvrtFlags = 0;
                OfficeApplication*  pApplication = OFF_APP();

                if( pApplication )
                {
                    OfaFilterOptions* pFilterOptions = pApplication->GetFilterOptions();
                    if ( pFilterOptions )
                    {
                        if ( pFilterOptions->IsMath2MathType() )
                            nCnvrtFlags |= OLE_STARMATH_2_MATHTYPE;
                        if ( pFilterOptions->IsWriter2WinWord() )
                            nCnvrtFlags |= OLE_STARWRITER_2_WINWORD;
                        if ( pFilterOptions->IsCalc2Excel() )
                            nCnvrtFlags |= OLE_STARCALC_2_EXCEL;
                        if ( pFilterOptions->IsImpress2PowerPoint() )
                            nCnvrtFlags |= OLE_STARIMPRESS_2_POWERPOINT;
                    }
                }
                mrDocument.SetSwapGraphicsMode( SDR_SWAPGRAPHICSMODE_TEMP );

                if( mbShowProgress )
                    CreateStatusIndicator();

                bRet = PPTExport( xStorRef, mxModel, mxStatusIndicator, pBas, nCnvrtFlags );
            }
        }
        delete pLibrary;
    }
    return bRet;
}

void SdPPTFilter::PreSaveBasic()
{
    OfficeApplication*  pApplication = OFF_APP();
    if( pApplication )
    {
        OfaFilterOptions* pFilterOptions = pApplication->GetFilterOptions();
        if( pFilterOptions && pFilterOptions->IsLoadPPointBasicStorage() )
        {
            SvStorageRef xDest( new SvStorage( new SvMemoryStream(), TRUE ) );
            SvxImportMSVBasic aMSVBas( (SfxObjectShell&) mrDocShell, *xDest, FALSE, FALSE );
            aMSVBas.SaveOrDelMSVBAStorage( TRUE, String( RTL_CONSTASCII_USTRINGPARAM("_MS_VBA_Overhead") ) );

            SvStorageRef xOverhead = xDest->OpenStorage( String( RTL_CONSTASCII_USTRINGPARAM("_MS_VBA_Overhead") ) );
            if ( xOverhead.Is() && ( xOverhead->GetError() == SVSTREAM_OK ) )
            {
                SvStorageRef xOverhead2 = xOverhead->OpenStorage( String( RTL_CONSTASCII_USTRINGPARAM("_MS_VBA_Overhead") ) );
                if ( xOverhead2.Is() && ( xOverhead2->GetError() == SVSTREAM_OK ) )
                {
                    SvStorageStreamRef xTemp = xOverhead2->OpenStream( String( RTL_CONSTASCII_USTRINGPARAM("_MS_VBA_Overhead2") ) );
                    if ( xTemp.Is() && ( xTemp->GetError() == SVSTREAM_OK ) )
                    {
                        UINT32 nLen = xTemp->GetSize();
                        if ( nLen )
                        {
                            char* pTemp = new char[ nLen ];
                            if ( pTemp )
                            {
                                xTemp->Seek( STREAM_SEEK_TO_BEGIN );
                                xTemp->Read( pTemp, nLen );
                                pBas = new SvMemoryStream( pTemp, nLen, STREAM_READ );
                                pBas->ObjectOwnsMemory( TRUE );
                            }
                        }
                    }
                }
            }
        }
    }

}
