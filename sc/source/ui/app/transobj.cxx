/*************************************************************************
 *
 *  $RCSfile: transobj.cxx,v $
 *
 *  $Revision: 1.21 $
 *
 *  last change: $Author: obo $ $Date: 2004-06-04 11:18:48 $
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

// INCLUDE ---------------------------------------------------------------

#ifdef PCH
#include "ui_pch.hxx"
#endif

#pragma hdrstop

#include "scitems.hxx"
#include <svx/eeitem.hxx>
#define ITEMID_FIELD EE_FEATURE_FIELD

#include <com/sun/star/uno/Sequence.hxx>

#include <sot/storage.hxx>
#include <so3/svstor.hxx>
#include <vcl/svapp.hxx>
#include <vcl/virdev.hxx>
#include <vos/mutex.hxx>
#include <sfx2/app.hxx>

#include "transobj.hxx"
#include "document.hxx"
#include "viewopti.hxx"
#include "editutil.hxx"
#include "impex.hxx"
#include "cell.hxx"
#include "printfun.hxx"
#include "docfunc.hxx"
#include "scmod.hxx"

// for InitDocShell
#include <svx/paperinf.hxx>
#include <svx/sizeitem.hxx>
#include <svx/algitem.hxx>
#include <svtools/intitem.hxx>
#include <svtools/zforlist.hxx>
#include "docsh.hxx"
#include "markdata.hxx"
#include "stlpool.hxx"
#include "viewdata.hxx"
#include "dociter.hxx"
#include "cellsuno.hxx"

using namespace com::sun::star;

// -----------------------------------------------------------------------

#define SCTRANS_TYPE_IMPEX          1
#define SCTRANS_TYPE_EDIT_RTF       2
#define SCTRANS_TYPE_EDIT_BIN       3
#define SCTRANS_TYPE_EMBOBJ         4

// -----------------------------------------------------------------------

// static
void ScTransferObj::GetAreaSize( ScDocument* pDoc, SCTAB nTab1, SCTAB nTab2, SCROW& nRow, SCCOL& nCol )
{
    SCCOL nMaxCol = 0;
    SCROW nMaxRow = 0;
    for( SCTAB nTab = nTab1; nTab <= nTab2; nTab++ )
    {
        SCCOL nLastCol = 0;
        SCROW nLastRow = 0;
        // GetPrintArea instead of GetCellArea - include drawing objects
        if( pDoc->GetPrintArea( nTab, nLastCol, nLastRow ) )
        {
            if( nLastCol > nMaxCol )
                nMaxCol = nLastCol;
            if( nLastRow > nMaxRow  )
                nMaxRow = nLastRow;
        }
    }
    nRow = nMaxRow;
    nCol = nMaxCol;
}

// static
void ScTransferObj::PaintToDev( OutputDevice* pDev, ScDocument* pDoc, double nPrintFactor,
                                const ScRange& rBlock, BOOL bMetaFile )
{
    if (!pDoc)
        return;

    Point aPoint;
    Rectangle aBound( aPoint, pDev->GetOutputSize() );      //! use size from clip area?

    ScViewData aViewData(NULL,NULL);
    aViewData.InitData( pDoc );

    aViewData.SetTabNo( rBlock.aEnd.Tab() );
    aViewData.SetScreen( rBlock.aStart.Col(), rBlock.aStart.Row(),
                            rBlock.aEnd.Col(), rBlock.aEnd.Row() );

    ScPrintFunc::DrawToDev( pDoc, pDev, nPrintFactor, aBound, &aViewData, bMetaFile );
}

// -----------------------------------------------------------------------

ScTransferObj::ScTransferObj( ScDocument* pClipDoc, const TransferableObjectDescriptor& rDesc ) :
    aObjDesc( rDesc ),
    pDoc( pClipDoc ),
    nDragHandleX( 0 ),
    nDragHandleY( 0 ),
    nDragSourceFlags( 0 ),
    bDragWasInternal( FALSE ),
    bUsedForLink( FALSE )
{
    DBG_ASSERT(pDoc->IsClipboard(), "wrong document");

    //
    // get aBlock from clipboard doc
    //

    SCCOL nCol1;
    SCROW nRow1;
    SCCOL nCol2;
    SCROW nRow2;
    pDoc->GetClipStart( nCol1, nRow1 );
    pDoc->GetClipArea( nCol2, nRow2, TRUE );    // real source area - include filtered rows
    nCol2 += nCol1;
    nRow2 += nRow1;

    SCCOL nDummy;
    pDoc->GetClipArea( nDummy, nNonFiltered, FALSE );
    ++nNonFiltered;     // to get count instead of diff

    SCTAB nTab1=0;
    SCTAB nTab2=0;
    BOOL bFirst = TRUE;
    for (SCTAB i=0; i<=MAXTAB; i++)
        if (pDoc->HasTable(i))
        {
            if (bFirst)
                nTab1 = i;
            nTab2 = i;
            bFirst = FALSE;
        }
    DBG_ASSERT(!bFirst, "no sheet selected");

    //  only limit to used cells if whole sheet was marked
    //  (so empty cell areas can be copied)
    if ( nCol2>=MAXCOL && nRow2>=MAXROW )
    {
        SCROW nMaxRow;
        SCCOL nMaxCol;
        GetAreaSize( pDoc, nTab1, nTab2, nMaxRow, nMaxCol );
        if( nMaxRow < nRow2 )
            nRow2 = nMaxRow;
        if( nMaxCol < nCol2 )
            nCol2 = nMaxCol;
    }

    aBlock = ScRange( nCol1, nRow1, nTab1, nCol2, nRow2, nTab2 );
    nVisibleTab = nTab1;    // valid table as default

    Rectangle aMMRect = pDoc->GetMMRect( nCol1,nRow1, nCol2,nRow2, nTab1 );
    aObjDesc.maSize = aMMRect.GetSize();
}

ScTransferObj::~ScTransferObj()
{
    Application::GetSolarMutex().acquire();

    ScModule* pScMod = SC_MOD();
    if ( pScMod->GetClipData().pCellClipboard == this )
    {
        DBG_ERROR("ScTransferObj wasn't released");
        pScMod->SetClipObject( NULL, NULL );
    }
    if ( pScMod->GetDragData().pCellTransfer == this )
    {
        DBG_ERROR("ScTransferObj wasn't released");
        pScMod->ResetDragObject();
    }

    delete pDoc;        // ScTransferObj is owner of clipboard document

    aDocShellRef.Clear();   // before releasing the mutex

    aDrawPersistRef.Clear();                    // after the model

    Application::GetSolarMutex().release();
}

// static
ScTransferObj* ScTransferObj::GetOwnClipboard( Window* pUIWin )
{
    ScTransferObj* pObj = SC_MOD()->GetClipData().pCellClipboard;
    if ( pObj && pUIWin )
    {
        //  check formats to see if pObj is really in the system clipboard

        //  pUIWin is NULL when called from core (IsClipboardSource),
        //  in that case don't access the system clipboard, because the call
        //  may be from other clipboard operations (like flushing, #86059#)

        TransferableDataHelper aDataHelper( TransferableDataHelper::CreateFromSystemClipboard( pUIWin ) );
        if ( !aDataHelper.HasFormat( SOT_FORMATSTR_ID_DIF ) )
        {
//          DBG_ERROR("ScTransferObj wasn't released");
            pObj = NULL;
        }
    }
    return pObj;
}

void ScTransferObj::AddSupportedFormats()
{
    AddFormat( SOT_FORMATSTR_ID_EMBED_SOURCE );
    AddFormat( SOT_FORMATSTR_ID_OBJECTDESCRIPTOR );
    AddFormat( SOT_FORMAT_GDIMETAFILE );
    AddFormat( SOT_FORMAT_BITMAP );

    // ScImportExport formats
    AddFormat( SOT_FORMATSTR_ID_HTML );
    AddFormat( SOT_FORMATSTR_ID_SYLK );
    AddFormat( SOT_FORMATSTR_ID_LINK );
    AddFormat( SOT_FORMATSTR_ID_DIF );
    AddFormat( SOT_FORMAT_STRING );

    AddFormat( SOT_FORMAT_RTF );
    if ( aBlock.aStart == aBlock.aEnd )
        AddFormat( SOT_FORMATSTR_ID_EDITENGINE );
}

sal_Bool ScTransferObj::GetData( const datatransfer::DataFlavor& rFlavor )
{
    sal_uInt32  nFormat = SotExchange::GetFormat( rFlavor );
    sal_Bool    bOK = sal_False;

    if( HasFormat( nFormat ) )
    {
        if ( nFormat == SOT_FORMATSTR_ID_LINKSRCDESCRIPTOR || nFormat == SOT_FORMATSTR_ID_OBJECTDESCRIPTOR )
        {
            bOK = SetTransferableObjectDescriptor( aObjDesc, rFlavor );
        }
        else if ( ( nFormat == SOT_FORMAT_RTF || nFormat == SOT_FORMATSTR_ID_EDITENGINE ) &&
                        aBlock.aStart == aBlock.aEnd )
        {
            //  RTF from a single cell is handled by EditEngine

            SCCOL nCol = aBlock.aStart.Col();
            SCROW nRow = aBlock.aStart.Row();
            SCTAB nTab = aBlock.aStart.Tab();

            const ScPatternAttr* pPattern = pDoc->GetPattern( nCol, nRow, nTab );
            ScTabEditEngine aEngine( *pPattern, pDoc->GetEditPool() );
            ScBaseCell* pCell = NULL;
            pDoc->GetCell( nCol, nRow, nTab, pCell );
            if (pCell)
            {
                if (pCell->GetCellType() == CELLTYPE_EDIT)
                {
                    const EditTextObject* pObj;
                    ((ScEditCell*)pCell)->GetData(pObj);
                    aEngine.SetText( *pObj );
                }
                else
                {
                    String aText;
                    pDoc->GetString( nCol, nRow, nTab, aText );
                    aEngine.SetText(aText);
                }
            }

            bOK = SetObject( &aEngine,
                            (nFormat == FORMAT_RTF) ? SCTRANS_TYPE_EDIT_RTF : SCTRANS_TYPE_EDIT_BIN,
                            rFlavor );
        }
        else if ( ScImportExport::IsFormatSupported( nFormat ) || nFormat == SOT_FORMAT_RTF )
        {
            //  if this transfer object was used to create a DDE link, filtered rows
            //  have to be included for subsequent calls (to be consistent with link data)
            if ( nFormat == SOT_FORMATSTR_ID_LINK )
                bUsedForLink = TRUE;

            BOOL bIncludeFiltered = pDoc->IsCutMode() || bUsedForLink;

            ScImportExport aObj( pDoc, aBlock );
            aObj.SetFormulas( pDoc->GetViewOptions().GetOption( VOPT_FORMULAS ) );
            aObj.SetIncludeFiltered( bIncludeFiltered );

            //  DataType depends on format type:

            if ( rFlavor.DataType.equals( ::getCppuType( (const ::rtl::OUString*) 0 ) ) )
            {
                rtl::OUString aString;
                if ( aObj.ExportString( aString, nFormat ) )
                    bOK = SetString( aString, rFlavor );
            }
            else if ( rFlavor.DataType.equals( ::getCppuType( (const uno::Sequence< sal_Int8 >*) 0 ) ) )
            {
                //  SetObject converts a stream into a Int8-Sequence
                bOK = SetObject( &aObj, SCTRANS_TYPE_IMPEX, rFlavor );
            }
            else
            {
                DBG_ERROR("unknown DataType");
            }
        }
        else if ( nFormat == SOT_FORMAT_BITMAP )
        {
            Rectangle aMMRect = pDoc->GetMMRect( aBlock.aStart.Col(), aBlock.aStart.Row(),
                                                 aBlock.aEnd.Col(), aBlock.aEnd.Row(),
                                                 aBlock.aStart.Tab() );
            VirtualDevice aVirtDev;
            aVirtDev.SetOutputSizePixel( aVirtDev.LogicToPixel( aMMRect.GetSize(), MAP_100TH_MM ) );

            PaintToDev( &aVirtDev, pDoc, 1.0, aBlock, FALSE );

            aVirtDev.SetMapMode( MapMode( MAP_PIXEL ) );
            Bitmap aBmp = aVirtDev.GetBitmap( Point(), aVirtDev.GetOutputSize() );
            bOK = SetBitmap( aBmp, rFlavor );
        }
        else if ( nFormat == SOT_FORMAT_GDIMETAFILE )
        {
            InitDocShell();
            SvEmbeddedObject* pEmbObj = aDocShellRef;

            // like SvEmbeddedTransfer::GetData:

            GDIMetaFile     aMtf;
            VirtualDevice   aVDev;
            MapMode         aMapMode( pEmbObj->GetMapUnit() );
            Rectangle       aVisArea( pEmbObj->GetVisArea( ASPECT_CONTENT ) );

            aVDev.EnableOutput( FALSE );
            aVDev.SetMapMode( aMapMode );
            aMtf.SetPrefSize( aVisArea.GetSize() );
            aMtf.SetPrefMapMode( aMapMode );
            aMtf.Record( &aVDev );

            pEmbObj->DoDraw( &aVDev, Point(), aVisArea.GetSize(), JobSetup(), ASPECT_CONTENT );

            aMtf.Stop();
            aMtf.WindStart();

            bOK = SetGDIMetaFile( aMtf, rFlavor );
        }
        else if ( nFormat == SOT_FORMATSTR_ID_EMBED_SOURCE )
        {
            InitDocShell();         // set aDocShellRef

            SvEmbeddedObject* pEmbObj = aDocShellRef;
            bOK = SetObject( pEmbObj, SCTRANS_TYPE_EMBOBJ, rFlavor );
        }
    }
    return bOK;
}

sal_Bool ScTransferObj::WriteObject( SotStorageStreamRef& rxOStm, void* pUserObject, sal_uInt32 nUserObjectId,
                                        const datatransfer::DataFlavor& rFlavor )
{
    // called from SetObject, put data into stream

    sal_Bool bRet = sal_False;
    switch (nUserObjectId)
    {
        case SCTRANS_TYPE_IMPEX:
            {
                ScImportExport* pImpEx = (ScImportExport*)pUserObject;

                sal_uInt32 nFormat = SotExchange::GetFormat( rFlavor );
                if ( pImpEx->ExportStream( *rxOStm, nFormat ) )
                    bRet = ( rxOStm->GetError() == ERRCODE_NONE );
            }
            break;

        case SCTRANS_TYPE_EDIT_RTF:
        case SCTRANS_TYPE_EDIT_BIN:
            {
                ScTabEditEngine* pEngine = (ScTabEditEngine*)pUserObject;
                if ( nUserObjectId == SCTRANS_TYPE_EDIT_RTF )
                {
                    pEngine->Write( *rxOStm, EE_FORMAT_RTF );
                    bRet = ( rxOStm->GetError() == ERRCODE_NONE );
                }
                else
                {
                    //  #107722# can't use Write for EditEngine format because that would
                    //  write old format without support for unicode characters.
                    //  Get the data from the EditEngine's transferable instead.

                    USHORT nParCnt = pEngine->GetParagraphCount();
                    if ( nParCnt == 0 )
                        nParCnt = 1;
                    ESelection aSel( 0, 0, nParCnt-1, pEngine->GetTextLen(nParCnt-1) );

                    uno::Reference<datatransfer::XTransferable> xEditTrans = pEngine->CreateTransferable( aSel );
                    TransferableDataHelper aEditHelper( xEditTrans );

                    bRet = aEditHelper.GetSotStorageStream( rFlavor, rxOStm );
                }
            }
            break;

        case SCTRANS_TYPE_EMBOBJ:
            {
                SvEmbeddedObject* pEmbObj = (SvEmbeddedObject*)pUserObject;

                SvStorageRef xWorkStore( new SvStorage( TRUE, *rxOStm ) );
                rxOStm->SetBufferSize( 0xff00 );

                // write document storage
                pEmbObj->SetupStorage( xWorkStore );
                bRet = pEmbObj->DoSaveAs( xWorkStore );
                pEmbObj->DoSaveCompleted();
                xWorkStore->Commit();
                rxOStm->Commit();

                bRet = ( rxOStm->GetError() == ERRCODE_NONE );
            }
            break;

        default:
            DBG_ERROR("unknown object id");
    }
    return bRet;
}

void ScTransferObj::ObjectReleased()
{
    ScModule* pScMod = SC_MOD();
    if ( pScMod->GetClipData().pCellClipboard == this )
        pScMod->SetClipObject( NULL, NULL );

    TransferableHelper::ObjectReleased();
}

void ScTransferObj::DragFinished( sal_Int8 nDropAction )
{
    if ( nDropAction == DND_ACTION_MOVE && !bDragWasInternal && !(nDragSourceFlags & SC_DROP_NAVIGATOR) )
    {
        //  move: delete source data
        ScDocShell* pSourceSh = GetSourceDocShell();
        if (pSourceSh)
        {
            ScMarkData aMarkData = GetSourceMarkData();
            //  external drag&drop doesn't copy objects, so they also aren't deleted:
            //  #105703# bApi=TRUE, don't show error messages from drag&drop
            pSourceSh->GetDocFunc().DeleteContents( aMarkData, IDF_ALL & ~IDF_OBJECTS, TRUE, TRUE );
        }
    }

    ScModule* pScMod = SC_MOD();
    if ( pScMod->GetDragData().pCellTransfer == this )
        pScMod->ResetDragObject();

    xDragSourceRanges = NULL;       // don't keep source after dropping

    TransferableHelper::DragFinished( nDropAction );
}

void ScTransferObj::SetDragHandlePos( SCCOL nX, SCROW nY )
{
    nDragHandleX = nX;
    nDragHandleY = nY;
}

void ScTransferObj::SetVisibleTab( SCTAB nNew )
{
    nVisibleTab = nNew;
}

void ScTransferObj::SetDrawPersist( const SvEmbeddedObjectRef& rRef )
{
    aDrawPersistRef = rRef;
}

void ScTransferObj::SetDragSource( ScDocShell* pSourceShell, const ScMarkData& rMark )
{
    ScRangeList aRanges;
    rMark.FillRangeListWithMarks( &aRanges, FALSE );
    xDragSourceRanges = new ScCellRangesObj( pSourceShell, aRanges );
}

void ScTransferObj::SetDragSourceFlags( USHORT nFlags )
{
    nDragSourceFlags = nFlags;
}

void ScTransferObj::SetDragWasInternal()
{
    bDragWasInternal = TRUE;
}

ScDocument* ScTransferObj::GetSourceDocument()
{
    ScDocShell* pSourceDocSh = GetSourceDocShell();
    if (pSourceDocSh)
        return pSourceDocSh->GetDocument();
    return NULL;
}

ScDocShell* ScTransferObj::GetSourceDocShell()
{
    ScCellRangesBase* pRangesObj = ScCellRangesBase::getImplementation( xDragSourceRanges );
    if (pRangesObj)
        return pRangesObj->GetDocShell();

    return NULL;    // none set
}

ScMarkData ScTransferObj::GetSourceMarkData()
{
    ScMarkData aMarkData;
    ScCellRangesBase* pRangesObj = ScCellRangesBase::getImplementation( xDragSourceRanges );
    if (pRangesObj)
    {
        const ScRangeList& rRanges = pRangesObj->GetRangeList();
        aMarkData.MarkFromRangeList( rRanges, FALSE );
    }
    return aMarkData;
}

//
//  initialize aDocShellRef with a live document from the ClipDoc
//

void ScTransferObj::InitDocShell()
{
    if ( !aDocShellRef.Is() )
    {
        ScDocShell* pDocSh = new ScDocShell;
        aDocShellRef = pDocSh;      // ref must be there before InitNew

        pDocSh->DoInitNew(NULL);

        ScDocument* pDestDoc = pDocSh->GetDocument();
        ScMarkData aDestMark;
        aDestMark.SelectTable( 0, TRUE );

        String aTabName;
        pDoc->GetName( aBlock.aStart.Tab(), aTabName );
        pDestDoc->RenameTab( 0, aTabName, FALSE );          // no UpdateRef (empty)

        pDestDoc->CopyStdStylesFrom( pDoc );

        SCCOL nStartX = aBlock.aStart.Col();
        SCROW nStartY = aBlock.aStart.Row();
        SCCOL nEndX = aBlock.aEnd.Col();
        SCROW nEndY = aBlock.aEnd.Row();

        //  widths / heights
        //  (must be copied before CopyFromClip, for drawing objects)

        SCCOL nCol;
        SCROW nRow;
        SCTAB nSrcTab = aBlock.aStart.Tab();
        for (nCol=nStartX; nCol<=nEndX; nCol++)
            if ( pDoc->GetColFlags( nCol, nSrcTab ) & CR_HIDDEN )
                pDestDoc->ShowCol( nCol, 0, FALSE );
            else
                pDestDoc->SetColWidth( nCol, 0, pDoc->GetColWidth( nCol, nSrcTab ) );
        for (nRow=nStartY; nRow<=nEndY; nRow++)
        {
            BYTE nSourceFlags = pDoc->GetRowFlags( nRow, nSrcTab );
            if ( nSourceFlags & CR_HIDDEN )
                pDestDoc->ShowRow( nRow, 0, FALSE );
            else
            {
                pDestDoc->SetRowHeight( nRow, 0, pDoc->GetRowHeight( nRow, nSrcTab ) );

                //  if height was set manually, that flag has to be copied, too
                if ( nSourceFlags & CR_MANUALSIZE )
                    pDestDoc->SetRowFlags( nRow, 0, pDestDoc->GetRowFlags( nRow, 0 ) | CR_MANUALSIZE );
            }
        }

        if ( pDoc->GetDrawLayer() )
            pDocSh->MakeDrawLayer();

        //  cell range is copied to the original position, but on the first sheet
        //  -> bCutMode must be set
        //  pDoc is always a Clipboard-document

        ScRange aDestRange( nStartX,nStartY,0, nEndX,nEndY,0 );
        BOOL bWasCut = pDoc->IsCutMode();
        if (!bWasCut)
            pDoc->SetClipArea( aDestRange, TRUE );          // Cut
        pDestDoc->CopyFromClip( aDestRange, aDestMark, IDF_ALL, NULL, pDoc, FALSE );
        pDoc->SetClipArea( aDestRange, bWasCut );

        StripRefs( pDoc, nStartX,nStartY, nEndX,nEndY, pDestDoc, 0,0 );

        ScRange aMergeRange = aDestRange;
        pDestDoc->ExtendMerge( aMergeRange, TRUE );

        pDoc->CopyDdeLinks( pDestDoc );         // copy values of DDE Links

        //  page format (grid etc) and page size (maximum size for ole object)

        Size aPaperSize = SvxPaperInfo::GetPaperSize( SVX_PAPER_A4 );       // Twips
        ScStyleSheetPool* pStylePool = pDoc->GetStyleSheetPool();
        String aStyleName = pDoc->GetPageStyle( aBlock.aStart.Tab() );
        SfxStyleSheetBase* pStyleSheet = pStylePool->Find( aStyleName, SFX_STYLE_FAMILY_PAGE );
        if (pStyleSheet)
        {
            const SfxItemSet& rSourceSet = pStyleSheet->GetItemSet();
            aPaperSize = ((const SvxSizeItem&) rSourceSet.Get(ATTR_PAGE_SIZE)).GetSize();

            //  CopyStyleFrom kopiert SetItems mit richtigem Pool
            ScStyleSheetPool* pDestPool = pDestDoc->GetStyleSheetPool();
            pDestPool->CopyStyleFrom( pStylePool, aStyleName, SFX_STYLE_FAMILY_PAGE );
        }

        ScViewData aViewData( pDocSh, NULL );
        aViewData.SetScreen( nStartX,nStartY, nEndX,nEndY );
        aViewData.SetCurX( nStartX );
        aViewData.SetCurY( nStartY );

        pDestDoc->SetViewOptions( pDoc->GetViewOptions() );

        //      Size
        //! get while copying sizes

        long nPosX = 0;
        long nPosY = 0;

        for (nCol=0; nCol<nStartX; nCol++)
            nPosX += pDestDoc->GetColWidth( nCol, 0 );
        for (nRow=0; nRow<nStartY; nRow++)
            nPosY += pDestDoc->FastGetRowHeight( nRow, 0 );
        nPosX = (long) ( nPosX * HMM_PER_TWIPS );
        nPosY = (long) ( nPosY * HMM_PER_TWIPS );


        aPaperSize.Width()  *= 2;       // limit OLE object to double of page size
        aPaperSize.Height() *= 2;

        long nSizeX = 0;
        long nSizeY = 0;
        for (nCol=nStartX; nCol<=nEndX; nCol++)
        {
            long nAdd = pDestDoc->GetColWidth( nCol, 0 );
            if ( nSizeX+nAdd > aPaperSize.Width() && nSizeX )   // above limit?
                break;
            nSizeX += nAdd;
        }
        for (nRow=nStartY; nRow<=nEndY; nRow++)
        {
            long nAdd = pDestDoc->FastGetRowHeight( nRow, 0 );
            if ( nSizeY+nAdd > aPaperSize.Height() && nSizeY )  // above limit?
                break;
            nSizeY += nAdd;
        }
        nSizeX = (long) ( nSizeX * HMM_PER_TWIPS );
        nSizeY = (long) ( nSizeY * HMM_PER_TWIPS );

//      pDocSh->SetVisAreaSize( Size(nSizeX,nSizeY) );

        Rectangle aNewArea( Point(nPosX,nPosY), Size(nSizeX,nSizeY) );
        pDocSh->SvInPlaceObject::SetVisArea( aNewArea );
        pDocSh->SetVisArea( aNewArea );

        pDocSh->UpdateOle(&aViewData, TRUE);

        //! SetDocumentModified?
        if ( pDestDoc->IsChartListenerCollectionNeedsUpdate() )
            pDestDoc->UpdateChartListenerCollection();
    }
}

//  static
SvPersist* ScTransferObj::SetDrawClipDoc( BOOL bAnyOle )
{
    // update ScGlobal::pDrawClipDocShellRef

    delete ScGlobal::pDrawClipDocShellRef;
    if (bAnyOle)
    {
        ScGlobal::pDrawClipDocShellRef =
                        new ScDocShellRef(new ScDocShell);      // there must be a ref
        (*ScGlobal::pDrawClipDocShellRef)->DoInitNew(NULL);
        return *ScGlobal::pDrawClipDocShellRef;
    }
    else
    {
        ScGlobal::pDrawClipDocShellRef = NULL;
        return NULL;
    }
}

//  static
void ScTransferObj::StripRefs( ScDocument* pDoc,
                    SCCOL nStartX, SCROW nStartY, SCCOL nEndX, SCROW nEndY,
                    ScDocument* pDestDoc, SCCOL nSubX, SCROW nSubY )
{
    if (!pDestDoc)
    {
        pDestDoc = pDoc;
        DBG_ASSERT(nSubX==0&&nSubY==0, "can't move within the document");
    }

    //  In a clipboard doc the data don't have to be on the first sheet

    SCTAB nSrcTab = 0;
    while (nSrcTab<MAXTAB && !pDoc->HasTable(nSrcTab))
        ++nSrcTab;
    SCTAB nDestTab = 0;
    while (nDestTab<MAXTAB && !pDestDoc->HasTable(nDestTab))
        ++nDestTab;

    if (!pDoc->HasTable(nSrcTab) || !pDestDoc->HasTable(nDestTab))
    {
        DBG_ERROR("Sheet not found in ScTransferObj::StripRefs");
        return;
    }

    SvNumberFormatter* pFormatter = pDoc->GetFormatTable();
    ScRange aRef;

    ScCellIterator aIter( pDoc, nStartX, nStartY, nSrcTab, nEndX, nEndY, nSrcTab );
    ScBaseCell* pCell = aIter.GetFirst();
    while (pCell)
    {
        if (pCell->GetCellType() == CELLTYPE_FORMULA)
        {
            ScFormulaCell* pFCell = (ScFormulaCell*) pCell;
            BOOL bOut = FALSE;
            ScDetectiveRefIter aRefIter( pFCell );
            while ( !bOut && aRefIter.GetNextRef( aRef ) )
            {
                if ( aRef.aStart.Tab() != nSrcTab || aRef.aEnd.Tab() != nSrcTab ||
                        aRef.aStart.Col() < nStartX || aRef.aEnd.Col() > nEndX ||
                        aRef.aStart.Row() < nStartY || aRef.aEnd.Row() > nEndY )
                    bOut = TRUE;
            }
            if (bOut)
            {
                SCCOL nCol = aIter.GetCol() - nSubX;
                SCROW nRow = aIter.GetRow() - nSubY;

                ScBaseCell* pNew = 0;
                USHORT nErrCode = pFCell->GetErrCode();
                if (nErrCode)
                {
                    pNew = new ScStringCell( ScGlobal::GetErrorString(nErrCode) );
                    if ( ((const SvxHorJustifyItem*) pDestDoc->GetAttr(
                            nCol,nRow,nDestTab, ATTR_HOR_JUSTIFY))->GetValue() ==
                            SVX_HOR_JUSTIFY_STANDARD )
                        pDestDoc->ApplyAttr( nCol,nRow,nDestTab,
                                SvxHorJustifyItem(SVX_HOR_JUSTIFY_RIGHT, ATTR_HOR_JUSTIFY) );
                }
                else if (pFCell->IsValue())
                {
                    double fVal = pFCell->GetValue();
                    pNew = new ScValueCell( fVal );
                }
                else
                {
                    String aStr;
                    pFCell->GetString(aStr);
                    pNew = new ScStringCell( aStr );
                }
                pDestDoc->PutCell( nCol,nRow,nDestTab, pNew );

                //  number formats

                ULONG nOldFormat = ((const SfxUInt32Item*)
                                pDestDoc->GetAttr(nCol,nRow,nDestTab, ATTR_VALUE_FORMAT))->GetValue();
                if ( (nOldFormat % SV_COUNTRY_LANGUAGE_OFFSET) == 0 )
                {
                    ULONG nNewFormat = pFCell->GetStandardFormat( *pFormatter,
                        nOldFormat );
                    pDestDoc->ApplyAttr( nCol,nRow,nDestTab,
                                SfxUInt32Item(ATTR_VALUE_FORMAT, nNewFormat) );
                }
            }
        }
        pCell = aIter.GetNext();
    }
}


