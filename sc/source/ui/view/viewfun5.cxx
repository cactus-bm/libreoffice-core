/*************************************************************************
 *
 *  $RCSfile: viewfun5.cxx,v $
 *
 *  $Revision: 1.36 $
 *
 *  last change: $Author: rt $ $Date: 2005-03-29 13:49:23 $
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

#ifndef _COM_SUN_STAR_EMBED_XEMBEDOBJECTCLIPBOARDCREATOR_HPP_
#include <com/sun/star/embed/XEmbedObjectClipboardCreator.hpp>
#endif

#ifdef PCH
#include "ui_pch.hxx"
#endif

#pragma hdrstop

#ifndef SVX_UNOMODEL_HXX
#include <svx/unomodel.hxx>
#endif

#ifndef _UTL_STREAM_WRAPPER_HXX_
#include <unotools/streamwrap.hxx>
#endif

//------------------------------------------------------------------

#include <svx/dbexch.hrc>
#include <svx/fmmodel.hxx>
#include <svx/svdetc.hxx>
#include <svx/svditer.hxx>
#include <svx/svdobj.hxx>
#include <svx/svdogrp.hxx>
#include <svx/svdouno.hxx>
#include <svx/svdpage.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/docfile.hxx>
#include <sot/clsids.hxx>
#include <sot/formats.hxx>
#include <svtools/pathoptions.hxx>
#include <svtools/ptitem.hxx>
#include <svtools/stritem.hxx>
#include <svtools/transfer.hxx>
#include <vcl/graph.hxx>

#include <comphelper/storagehelper.hxx>
#include <comphelper/processfactory.hxx>

#include <sot/formats.hxx>
#define SOT_FORMATSTR_ID_STARCALC_CURRENT   SOT_FORMATSTR_ID_STARCALC_50

#include "viewfunc.hxx"
#include "docsh.hxx"
#include "drawview.hxx"
#include "impex.hxx"
#include "dbfunc.hxx"
#include "dbcolect.hxx"
#include "sc.hrc"
#include "filter.hxx"
#include "scextopt.hxx"
#include "tabvwsh.hxx"      //  wegen GetViewFrame
#include "compiler.hxx"

#ifndef _SFXVIEWFRM_HXX //autogen
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _SVX_DBAEXCHANGE_HXX_
#include <svx/dbaexchange.hxx>
#endif

using namespace com::sun::star;

//------------------------------------------------------------------

BOOL ScViewFunc::PasteDataFormat( ULONG nFormatId,
                    const uno::Reference<datatransfer::XTransferable>& rxTransferable,
                    SCCOL nPosX, SCROW nPosY, Point* pLogicPos, BOOL bLink, BOOL bAllowDialogs )
{
    ScDocument* pDoc = GetViewData()->GetDocument();
    pDoc->SetPastingDrawFromOtherDoc( TRUE );

    Point aPos;                     //  inserting position (1/100 mm)
    if (pLogicPos)
        aPos = *pLogicPos;
    else
    {
        //  inserting position isn't needed for text formats
        BOOL bIsTextFormat = ( ScImportExport::IsFormatSupported( nFormatId ) ||
                                nFormatId == FORMAT_RTF );
        if ( !bIsTextFormat )
        {
            //  Window MapMode isn't drawing MapMode if DrawingLayer hasn't been created yet

            ScDocument* pDoc = GetViewData()->GetDocument();
            SCTAB nTab = GetViewData()->GetTabNo();
            long nXT = 0;
            for (SCCOL i=0; i<nPosX; i++)
                nXT += pDoc->GetColWidth(i,nTab);
            if (pDoc->IsNegativePage(nTab))
                nXT = -nXT;
            ULONG nYT = pDoc->FastGetRowHeight( 0, nPosY-1, nTab);
            aPos = Point( (long)(nXT * HMM_PER_TWIPS), (long)(nYT * HMM_PER_TWIPS) );
        }
    }

    TransferableDataHelper aDataHelper( rxTransferable );
    BOOL bRet = FALSE;

    //
    //  handle individual formats
    //

    if ( nFormatId == SOT_FORMATSTR_ID_EMBED_SOURCE ||
         nFormatId == SOT_FORMATSTR_ID_LINK_SOURCE ||
         nFormatId == SOT_FORMATSTR_ID_EMBED_SOURCE_OLE ||
         nFormatId == SOT_FORMATSTR_ID_LINK_SOURCE_OLE )
    {
        uno::Reference < io::XInputStream > xStm;
        TransferableObjectDescriptor   aObjDesc;

        if( aDataHelper.GetTransferableObjectDescriptor( SOT_FORMATSTR_ID_OBJECTDESCRIPTOR, aObjDesc ) &&
            aDataHelper.GetInputStream( nFormatId, xStm ) )
        {
            if ( aObjDesc.maClassName == SvGlobalName( SO3_SC_CLASSID_60 ) )
            {
                uno::Reference < embed::XStorage > xStore = ::comphelper::OStorageHelper::GetStorageFromInputStream( xStm );

                // mba: BaseURL doesn't make sense for clipboard
                // #i43716# Medium must be allocated with "new".
                // DoLoad stores the pointer and deletes it with the SfxObjectShell.
                SfxMedium* pMedium = new SfxMedium( xStore, String() );

                //  TODO/LATER: is it a problem that we don't support binary formats here?
                ScDocShellRef xDocShRef = new ScDocShell(SFX_CREATE_MODE_EMBEDDED);
                if (xDocShRef->DoLoad(pMedium))
                {
                    ScDocument* pSrcDoc = xDocShRef->GetDocument();
                    SCTAB nSrcTab = pSrcDoc->GetVisibleTab();
                    if (!pSrcDoc->HasTable(nSrcTab))
                        nSrcTab = 0;

                    ScMarkData aSrcMark;
                    aSrcMark.SelectOneTable( nSrcTab );         // for CopyToClip
                    ScDocument* pClipDoc = new ScDocument( SCDOCMODE_CLIP );

                    SCCOL nFirstCol, nLastCol;
                    SCROW nFirstRow, nLastRow;
                    if ( pSrcDoc->GetDataStart( nSrcTab, nFirstCol, nFirstRow ) )
                        pSrcDoc->GetCellArea( nSrcTab, nLastCol, nLastRow );
                    else
                        {
                        nFirstCol = nLastCol = 0;
                        nFirstRow = nLastRow = 0;
                        }
                    pSrcDoc->CopyToClip( nFirstCol, nFirstRow, nLastCol, nLastRow,
                                            FALSE, pClipDoc, FALSE, &aSrcMark );
                    ScGlobal::SetClipDocName( xDocShRef->GetTitle( SFX_TITLE_FULLNAME ) );

                    SetCursor( nPosX, nPosY );
                    Unmark();
                    PasteFromClip( IDF_ALL, pClipDoc,
                                    PASTE_NOFUNC, FALSE, FALSE, FALSE, INS_NONE, IDF_NONE,
                                    bAllowDialogs );
                    delete pClipDoc;
                    bRet = TRUE;
                }

                xDocShRef->DoClose();
                xDocShRef.Clear();
            }
            else
            {
                ::rtl::OUString aName;
                uno::Reference < embed::XEmbeddedObject > xObj = GetViewData()->GetViewShell()->GetObjectShell()->
                        GetEmbeddedObjectContainer().InsertEmbeddedObject( xStm, aName );
                if ( xObj.is() )
                {
                    PasteObject( aPos, xObj, &aObjDesc.maSize );
                    bRet = TRUE;
                }
                else
                    DBG_ERROR("Error in CreateAndLoad");
            }
        }
        else
        {
            uno::Reference < io::XInputStream > xStm;
            TransferableObjectDescriptor    aObjDesc;

            if ( aDataHelper.GetTransferableObjectDescriptor( SOT_FORMATSTR_ID_OBJECTDESCRIPTOR_OLE, aObjDesc ) )
            {
                ::rtl::OUString aName;
                uno::Reference < embed::XEmbeddedObject > xObj;

                if ( aDataHelper.GetInputStream( SOT_FORMATSTR_ID_EMBED_SOURCE_OLE, xStm )
                  || aDataHelper.GetInputStream( SOT_FORMATSTR_ID_EMBEDDED_OBJ_OLE, xStm ) )
                {
                    xObj = GetViewData()->GetDocShell()->GetEmbeddedObjectContainer().InsertEmbeddedObject( xStm, aName );
                }
                else
                {
                    try
                    {
                        uno::Reference< embed::XStorage > xTmpStor = ::comphelper::OStorageHelper::GetTemporaryStorage();
                        uno::Reference < embed::XEmbedObjectClipboardCreator > xClipboardCreator(
                            ::comphelper::getProcessServiceFactory()->createInstance( ::rtl::OUString(
                                            RTL_CONSTASCII_USTRINGPARAM("com.sun.star.embed.MSOLEObjectSystemCreator") ) ),
                            uno::UNO_QUERY_THROW );

                        embed::InsertedObjectInfo aInfo = xClipboardCreator->createInstanceInitFromClipboard(
                                                            xTmpStor,
                                                            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( "DummyName" ) ),
                                                            uno::Sequence< beans::PropertyValue >() );

                        // TODO/LATER: in future InsertedObjectInfo will be used to get container related information
                        // for example whether the object should be an iconified one
                        xObj = aInfo.Object;
                        if ( xObj.is() )
                            GetViewData()->GetDocShell()->GetEmbeddedObjectContainer().InsertEmbeddedObject( xObj, aName );
                    }
                    catch( uno::Exception& )
                    {}
                }

                if ( xObj.is() )
                {
                    PasteObject( aPos, xObj, &aObjDesc.maSize );
                    bRet = TRUE;
                }
                else
                    DBG_ERROR("Error creating external OLE object");
            }
            //TODO/LATER: if format is not available, create picture
        }
    }
    else if ( nFormatId == SOT_FORMATSTR_ID_LINK )      // LINK is also in ScImportExport
    {
        bRet = PasteDDE( rxTransferable );
    }
    else if ( ScImportExport::IsFormatSupported( nFormatId ) || nFormatId == SOT_FORMAT_RTF )
    {
        if ( nFormatId == SOT_FORMAT_RTF && aDataHelper.HasFormat( SOT_FORMATSTR_ID_EDITENGINE ) )
        {
            //  use EditView's PasteSpecial / Drop
            PasteRTF( nPosX, nPosY, rxTransferable );
            bRet = TRUE;
        }
        else
        {
            ScAddress aPos( nPosX, nPosY, GetViewData()->GetTabNo() );
            ScImportExport aObj( GetViewData()->GetDocument(), aPos );

            ::rtl::OUString aStr;
            SotStorageStreamRef xStream;
            if ( aDataHelper.GetSotStorageStream( nFormatId, xStream ) && xStream.Is() )
                // mba: clipboard always must contain absolute URLs (could be from alien source)
                bRet = aObj.ImportStream( *xStream, String(), nFormatId );
            else if ( aDataHelper.GetString( nFormatId, aStr ) )
                bRet = aObj.ImportString( aStr, nFormatId );

            InvalidateAttribs();
            GetViewData()->UpdateInputHandler();
        }
    }
    else if (nFormatId == SOT_FORMATSTR_ID_SBA_DATAEXCHANGE)
    {
        //  import of database data into table

        String sDataDesc;
        if ( aDataHelper.GetString( nFormatId, sDataDesc ) )
        {
            SfxStringItem aDataDesc(SID_SBA_IMPORT, sDataDesc);

            ScDocShell* pDocSh = GetViewData()->GetDocShell();
            ScDocument* pDoc = pDocSh->GetDocument();
            SCTAB nTab = GetViewData()->GetTabNo();

            ClickCursor(nPosX, nPosY, FALSE);               // set cursor position

            //  Creation of database area "Import1" isn't here, but in the DocShell
            //  slot execute, so it can be added to the undo action

            ScDBData* pDBData = pDocSh->GetDBData( ScRange(nPosX,nPosY,nTab), SC_DB_OLD, FALSE );
            String sTarget;
            if (pDBData)
                sTarget = pDBData->GetName();
            else
            {
                ScAddress aPos( nPosX,nPosY,nTab );
                aPos.Format( sTarget, SCA_ABS_3D, pDoc );
            }
            SfxStringItem aTarget(FN_PARAM_1, sTarget);

            BOOL bAreaIsNew = !pDBData;
            SfxBoolItem aAreaNew(FN_PARAM_2, bAreaIsNew);

            ::svx::ODataAccessDescriptor aDesc;
            DataFlavorExVector& rVector = aDataHelper.GetDataFlavorExVector();
            ::std::auto_ptr<SfxUsrAnyItem> pCursorItem;
            if ( ::svx::ODataAccessObjectTransferable::canExtractObjectDescriptor(rVector) )
            {
                aDesc = ::svx::ODataAccessObjectTransferable::extractObjectDescriptor(aDataHelper);
                if ( aDesc.has(::svx::daCursor) )
                    pCursorItem.reset(new SfxUsrAnyItem(FN_PARAM_3, aDesc[::svx::daCursor]));
            }

            //  asynchronous, to avoid doing the whole import in drop handler
            SfxDispatcher& rDisp = GetViewData()->GetDispatcher();
            rDisp.Execute(SID_SBA_IMPORT, SFX_CALLMODE_ASYNCHRON,
                                        &aDataDesc, &aTarget, &aAreaNew, pCursorItem.get(), (void*)0 );

            bRet = TRUE;
        }
    }
    else if (nFormatId == SOT_FORMATSTR_ID_SBA_FIELDDATAEXCHANGE)
    {
        //  insert database field control

        if ( ::svx::OColumnTransferable::canExtractColumnDescriptor( aDataHelper.GetDataFlavorExVector(), CTF_COLUMN_DESCRIPTOR | CTF_CONTROL_EXCHANGE ) )
        {
            MakeDrawLayer();
            ScDrawView* pDrawView = GetScDrawView();
            SdrObject* pObj = pDrawView->CreateFieldControl( ::svx::OColumnTransferable::extractColumnDescriptor( aDataHelper ) );
            if (pObj)
            {
                Point aInsPos = aPos;
                Rectangle aRect(pObj->GetLogicRect());
                aInsPos.X() -= aRect.GetSize().Width()  / 2;
                aInsPos.Y() -= aRect.GetSize().Height() / 2;
                if ( aInsPos.X() < 0 ) aInsPos.X() = 0;
                if ( aInsPos.Y() < 0 ) aInsPos.Y() = 0;
                aRect.SetPos(aInsPos);
                pObj->SetLogicRect(aRect);

                if ( pObj->ISA(SdrUnoObj) )
                    pObj->NbcSetLayer(SC_LAYER_CONTROLS);
                else
                    pObj->NbcSetLayer(SC_LAYER_FRONT);
                if (pObj->ISA(SdrObjGroup))
                {
                    SdrObjListIter aIter( *pObj, IM_DEEPWITHGROUPS );
                    SdrObject* pSubObj = aIter.Next();
                    while (pSubObj)
                    {
                        if ( pSubObj->ISA(SdrUnoObj) )
                            pSubObj->NbcSetLayer(SC_LAYER_CONTROLS);
                        else
                            pSubObj->NbcSetLayer(SC_LAYER_FRONT);
                        pSubObj = aIter.Next();
                    }
                }

                pDrawView->InsertObjectSafe(pObj, *pDrawView->GetPageViewPvNum(0));

                GetViewData()->GetViewShell()->SetDrawShell( TRUE );
                bRet = TRUE;
            }
        }
    }
    else if (nFormatId == SOT_FORMAT_BITMAP)
    {
        Bitmap aBmp;
        if( aDataHelper.GetBitmap( FORMAT_BITMAP, aBmp ) )
            bRet = PasteBitmap( aPos, aBmp );
    }
    else if (nFormatId == SOT_FORMAT_GDIMETAFILE)
    {
        GDIMetaFile aMtf;
        if( aDataHelper.GetGDIMetaFile( FORMAT_GDIMETAFILE, aMtf ) )
            bRet = PasteMetaFile( aPos, aMtf );
    }
    else if (nFormatId == SOT_FORMATSTR_ID_SVXB)
    {
        SotStorageStreamRef xStm;
        if( aDataHelper.GetSotStorageStream( SOT_FORMATSTR_ID_SVXB, xStm ) )
        {
            Graphic aGraphic;
            *xStm >> aGraphic;
            bRet = PasteGraphic( aPos, aGraphic, EMPTY_STRING, EMPTY_STRING );
        }
    }
    else if ( nFormatId == SOT_FORMATSTR_ID_DRAWING )
    {
        SotStorageStreamRef xStm;
        if( aDataHelper.GetSotStorageStream( SOT_FORMATSTR_ID_DRAWING, xStm ) )
        {
            MakeDrawLayer();    // before loading model, so 3D factory has been created

            SvtPathOptions aPathOpt;
            String aPath = aPathOpt.GetPalettePath();
            FmFormModel* pModel = new FmFormModel(
                                    aPath, NULL, GetViewData()->GetDocShell() );
            pModel->GetItemPool().FreezeIdRanges();
            xStm->Seek(0);

            com::sun::star::uno::Reference< com::sun::star::io::XInputStream > xInputStream( new utl::OInputStreamWrapper( *xStm ) );
            SvxDrawingLayerImport( pModel, xInputStream );

//BFS04         pModel->SetStreamingSdrModel(FALSE);

                                        // set everything to right layer:
            ULONG nObjCount = 0;
            USHORT nPages = pModel->GetPageCount();
            for (USHORT i=0; i<nPages; i++)
            {
                SdrPage* pPage = pModel->GetPage(i);
                SdrObjListIter aIter( *pPage, IM_DEEPWITHGROUPS );
                SdrObject* pObject = aIter.Next();
                while (pObject)
                {
                    if ( pObject->ISA(SdrUnoObj) )
                        pObject->NbcSetLayer(SC_LAYER_CONTROLS);
                    else
                        pObject->NbcSetLayer(SC_LAYER_FRONT);
                    pObject = aIter.Next();
                }

                nObjCount += pPage->GetObjCount();          // #105888# count group object only once
            }

            PasteDraw( aPos, pModel, (nObjCount > 1) );     // grouped if more than 1 object
            delete pModel;
            bRet = TRUE;
        }
    }
    else if ( (nFormatId == SOT_FORMATSTR_ID_BIFF_5) || (nFormatId == SOT_FORMATSTR_ID_BIFF_8) )
    {
        //  do excel import into a clipboard document
        //TODO/MBA: testing
        uno::Reference < io::XInputStream > xStm;
        if( aDataHelper.GetInputStream( nFormatId, xStm ) )
        {
#if 0
            SotStorage aDest( "d:\\test.xls" ); // to see the file
            pStor->CopyTo( &aDest );
#endif
            ScDocument* pDoc = GetViewData()->GetDocument();
            ScDocument* pInsDoc = new ScDocument( SCDOCMODE_CLIP );
            SCTAB nSrcTab = 0;      // Biff5 in clipboard: always sheet 0
            pInsDoc->ResetClip( pDoc, nSrcTab );

            SfxMedium aMed;
            aMed.GetItemSet()->Put( SfxUsrAnyItem( SID_INPUTSTREAM, uno::makeAny( xStm ) ) );
            FltError eErr = ScImportExcel( aMed, pInsDoc, EIF_AUTO );
            if ( eErr == eERR_OK )
            {
                ScRange aSource;
                const ScExtDocOptions* pExtOpt = pInsDoc->GetExtDocOptions();
                const ScExtTabSettings* pTabSett = pExtOpt ? pExtOpt->GetTabSettings( nSrcTab ) : 0;
                if( pTabSett && pTabSett->maUsedArea.IsValid() )
                {
                    aSource = pTabSett->maUsedArea;
                    // ensure correct sheet indexes
                    aSource.aStart.SetTab( nSrcTab );
                    aSource.aEnd.SetTab( nSrcTab );
// #92240# don't use selection area: if cursor is moved in Excel after Copy, selection
// represents the new cursor position and not the copied area
                }
                else
                {
                    DBG_ERROR("no dimension");  //! possible?
                    SCCOL nFirstCol, nLastCol;
                    SCROW nFirstRow, nLastRow;
                    if ( pInsDoc->GetDataStart( nSrcTab, nFirstCol, nFirstRow ) )
                        pInsDoc->GetCellArea( nSrcTab, nLastCol, nLastRow );
                    else
                    {
                        nFirstCol = nLastCol = 0;
                        nFirstRow = nLastRow = 0;
                    }
                    aSource = ScRange( nFirstCol, nFirstRow, nSrcTab,
                                        nLastCol, nLastRow, nSrcTab );
                }

                if ( pLogicPos )
                {
                    // position specified (Drag&Drop) - change selection
                    MoveCursorAbs( nPosX, nPosY, SC_FOLLOW_NONE, FALSE, FALSE );
                    Unmark();
                }

                pInsDoc->SetClipArea( aSource );
                PasteFromClip( IDF_ALL, pInsDoc,
                                PASTE_NOFUNC, FALSE, FALSE, FALSE, INS_NONE, IDF_NONE,
                                bAllowDialogs );
                delete pInsDoc;

                bRet = TRUE;
            }
        }
    }
    else if ( nFormatId == SOT_FORMAT_FILE )
    {
        //! multiple files?
        //USHORT nCount = DragServer::GetItemCount();
        //if (nCount == 0)  // normal handling (not D&D)
        {
            String aFile;
            if ( aDataHelper.GetString( nFormatId, aFile ) )
                bRet = PasteFile( aPos, aFile, bLink );
        }
#if 0
        else        // use multiple items from drag server
        {
            for( USHORT i = 0; i < nCount ; i++ )
            {
                String aFile = DragServer::PasteFile( i );
                SfxStringItem aNameItem( FID_INSERT_FILE, aFile );
                SfxPointItem aPosItem( FN_PARAM_1, aPos );
                SfxDispatcher* pDisp =
                    GetViewData()->GetViewShell()->GetViewFrame()->GetDispatcher();
                if (pDisp)
                    pDisp->Execute( FID_INSERT_FILE, SFX_CALLMODE_ASYNCHRON,
                                        &aNameItem, &aPosItem, (void*)0 );

                aPos.X() += 400;
                aPos.Y() += 400;
            }
            bRet = TRUE;
        }
#endif
    }
    else if ( nFormatId == SOT_FORMATSTR_ID_SOLK ||
              nFormatId == SOT_FORMATSTR_ID_UNIFORMRESOURCELOCATOR ||
              nFormatId == SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK ||
              nFormatId == SOT_FORMATSTR_ID_FILEGRPDESCRIPTOR )
    {
        bRet = PasteBookmark( nFormatId, rxTransferable, nPosX, nPosY );
    }

    pDoc->SetPastingDrawFromOtherDoc( FALSE );

    return bRet;
}

ByteString lcl_GetSubString( sal_Char* pData, long nStart, long nDataSize )
{
    if ( nDataSize <= nStart /* || pData[nDataSize] != 0 */ )
    {
        DBG_ERROR("DDE Data: invalid data");
        return ByteString();
    }
    return ByteString( pData + nStart );
}

BOOL ScViewFunc::PasteDDE( const uno::Reference<datatransfer::XTransferable>& rxTransferable )
{
    TransferableDataHelper aDataHelper( rxTransferable );

    //  get link data from transferable before string data,
    //  so the source knows it will be used for a link

    uno::Sequence<sal_Int8> aSequence;
    if ( !aDataHelper.GetSequence( SOT_FORMATSTR_ID_LINK, aSequence ) )
    {
        DBG_ERROR("DDE Data not found.");
        return FALSE;
    }

    //  check size (only if string is available in transferable)

    USHORT nCols = 1;
    USHORT nRows = 1;
    if ( aDataHelper.HasFormat( SOT_FORMAT_STRING ) )
    {
        String aDataStr;
        if ( aDataHelper.GetString( SOT_FORMAT_STRING, aDataStr ) )
        {
            //  get size from string the same way as in ScDdeLink::DataChanged

            aDataStr.ConvertLineEnd(LINEEND_LF);
            xub_StrLen nLen = aDataStr.Len();
            if (nLen && aDataStr.GetChar(nLen-1) == '\n')
                aDataStr.Erase(nLen-1);

            if (aDataStr.Len())
            {
                nRows = aDataStr.GetTokenCount( '\n' );
                String aLine = aDataStr.GetToken( 0, '\n' );
                if (aLine.Len())
                    nCols = aLine.GetTokenCount( '\t' );
            }
        }
    }

    //  create formula

    long nSeqLen = aSequence.getLength();
    sal_Char* pData = (sal_Char*)aSequence.getConstArray();

    rtl_TextEncoding eSysEnc = gsl_getSystemTextEncoding();

    ByteString aByteApp   = lcl_GetSubString( pData, 0, nSeqLen );
    ByteString aByteTopic = lcl_GetSubString( pData, aByteApp.Len() + 1, nSeqLen );
    ByteString aByteItem  = lcl_GetSubString( pData, aByteApp.Len() + aByteTopic.Len() + 2, nSeqLen );

    String aApp( aByteApp, eSysEnc );
    String aTopic( aByteTopic, eSysEnc );
    String aItem( aByteItem, eSysEnc );

    if (!ScCompiler::pSymbolTableNative)
    {
        DBG_ERROR("ScCompiler::pSymbolTableNative missing");
        return FALSE;
    }

    //! use tokens
    String aFormula( '=' );
    aFormula += ScCompiler::pSymbolTableNative[SC_OPCODE_DDE];
    aFormula.AppendAscii(RTL_CONSTASCII_STRINGPARAM("(\""));
    aFormula += aApp;
    aFormula.AppendAscii(RTL_CONSTASCII_STRINGPARAM("\";\""));
    aFormula += aTopic;
    aFormula.AppendAscii(RTL_CONSTASCII_STRINGPARAM("\";\""));
    aFormula += aItem;
    aFormula.AppendAscii(RTL_CONSTASCII_STRINGPARAM("\")"));

    //  mark range

    SCTAB nTab = GetViewData()->GetTabNo();
    SCCOL nCurX = GetViewData()->GetCurX();
    SCROW nCurY = GetViewData()->GetCurY();
    HideAllCursors();
    DoneBlockMode();
    InitBlockMode( nCurX, nCurY, nTab );
    MarkCursor( nCurX+static_cast<SCCOL>(nCols)-1, nCurY+static_cast<SCROW>(nRows)-1, nTab );
    ShowAllCursors();

    //  enter formula

    EnterMatrix( aFormula );
    CursorPosChanged();

    return TRUE;
}


