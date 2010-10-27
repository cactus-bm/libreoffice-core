/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sc.hxx"

#include <svx/svdpool.hxx>
#include <svx/sdtaitm.hxx>
#include <svx/svdotext.hxx>
#include <editeng/editobj.hxx>
#include <svx/svdoole2.hxx>
#include <sot/storage.hxx>
#include <svl/itemset.hxx>
#include <svx/svdpage.hxx>
#include <svx/svdocapt.hxx>
#include <svx/unoapi.hxx>
#include <editeng/writingmodeitem.hxx>
#include <vcl/svapp.hxx>
#include <rtl/math.hxx>
#include <svl/zformat.hxx>
#include "cell.hxx"
#include "drwlayer.hxx"

#include "xcl97rec.hxx"
#include "xcl97esc.hxx"
#include "editutil.hxx"
#include "xecontent.hxx"
#include "xeescher.hxx"
#include "xestyle.hxx"
#include "xelink.hxx"

#include "scitems.hxx"

#include <unotools/fltrcfg.hxx>
#include <editeng/brshitem.hxx>
#include <editeng/boxitem.hxx>
#include <editeng/frmdiritem.hxx>
#include <editeng/adjitem.hxx>
#include <editeng/eeitem.hxx>
#include <filter/msfilter/msoleexp.hxx>

#include <unotools/localedatawrapper.hxx>

#include <stdio.h>

#include "document.hxx"
#include "conditio.hxx"
#include "rangelst.hxx"
#include "stlpool.hxx"
#include "viewopti.hxx"
#include "scextopt.hxx"
#include "docoptio.hxx"
#include "patattr.hxx"
#include "tabprotection.hxx"

#include <com/sun/star/sheet/XCellAddressable.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#include <oox/core/tokens.hxx>
#include <oox/export/shapes.hxx>
#include <oox/export/utils.hxx>
#include <oox/export/vmlexport.hxx>

using ::rtl::OString;
using ::rtl::OUString;
using namespace ::com::sun::star;
using ::com::sun::star::uno::Reference;
using ::oox::drawingml::DrawingML;
using ::com::sun::star::uno::UNO_QUERY;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::drawing::XShape;
using ::oox::drawingml::ShapeExport;
using ::oox::vml::VMLExport;

// ============================================================================

sal_Int32 XclExpObjList::mnDrawingMLCount;
sal_Int32 XclExpObjList::mnVmlCount;

XclExpObjList::XclExpObjList( const XclExpRoot& rRoot, XclEscherEx& rEscherEx ) :
    XclExpRoot( rRoot ),
    mrEscherEx( rEscherEx ),
    pSolverContainer( 0 ),
    mnScTab( rRoot.GetCurrScTab() )
{
    pMsodrawingPerSheet = new XclExpMsoDrawing( rEscherEx );
    // open the DGCONTAINER and the patriarch group shape
    mrEscherEx.OpenContainer( ESCHER_DgContainer );
    Rectangle aRect( 0, 0, 0, 0 );
    mrEscherEx.EnterGroup( &aRect );
    mrEscherEx.UpdateDffFragmentEnd();
}

XclExpObjList::~XclExpObjList()
{
    for ( XclObj* p = First(); p; p = Next() )
        delete p;
    delete pMsodrawingPerSheet;
    delete pSolverContainer;
}

UINT16 XclExpObjList::Add( XclObj* pObj )
{
    DBG_ASSERT( Count() < 0xFFFF, "XclExpObjList::Add: too much for Xcl" );
    if ( Count() < 0xFFFF )
    {
        Insert( pObj, LIST_APPEND );
        UINT16 nCnt = (UINT16) Count();
        pObj->SetId( nCnt );
        pObj->SetTab( mnScTab );
        return nCnt;
    }
    else
    {
        delete pObj;
        return 0;
    }
}

void XclExpObjList::EndSheet()
{
    // Is there still something in the stream? -> The solver container
    if( mrEscherEx.HasPendingDffData() )
        pSolverContainer = new XclExpMsoDrawing( mrEscherEx );

    // close the DGCONTAINER created by XclExpObjList ctor MSODRAWING
    mrEscherEx.CloseContainer();
}

void XclExpObjList::Save( XclExpStream& rStrm )
{
    //! Escher must be written, even if there are no objects
    pMsodrawingPerSheet->Save( rStrm );

    for ( XclObj* p = First(); p; p = Next() )
        p->Save( rStrm );

    if( pSolverContainer )
        pSolverContainer->Save( rStrm );
}

static bool IsVmlObject( const XclObj& rObj )
{
    switch( rObj.GetObjType() )
    {
        case EXC_OBJTYPE_NOTE:
            return true;
        default:
            return false;
    }
}


static sal_Int32 GetVmlObjectCount( XclExpObjList& rList )
{
    sal_Int32 nNumVml = 0;

    for ( XclObj* p = rList.First(); p; p = rList.Next() )
        if( IsVmlObject( *p ) )
            ++nNumVml;

    return nNumVml;
}


static void SaveDrawingMLObjects( XclExpObjList& rList, XclExpXmlStream& rStrm, sal_Int32& nDrawingMLCount )
{
    sal_Int32 nVmlObjects = GetVmlObjectCount( rList );
    if( (rList.Count() - nVmlObjects) == 0 )
        return;

    sal_Int32 nDrawing = ++nDrawingMLCount;
    OUString sId;
    sax_fastparser::FSHelperPtr pDrawing = rStrm.CreateOutputStream(
            XclXmlUtils::GetStreamName( "xl/", "drawings/drawing", nDrawing ),
            XclXmlUtils::GetStreamName( "../", "drawings/drawing", nDrawing ),
            rStrm.GetCurrentStream()->getOutputStream(),
            "application/vnd.openxmlformats-officedocument.drawing+xml",
            "http://schemas.openxmlformats.org/officeDocument/2006/relationships/drawing",
            &sId );

    rStrm.GetCurrentStream()->singleElement( XML_drawing,
            FSNS( XML_r, XML_id ),  XclXmlUtils::ToOString( sId ).getStr(),
            FSEND );

    rStrm.PushStream( pDrawing );
    pDrawing->startElement( FSNS( XML_xdr, XML_wsDr ),
            FSNS( XML_xmlns, XML_xdr ), "http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing",
            FSNS( XML_xmlns, XML_a ),   "http://schemas.openxmlformats.org/drawingml/2006/main",
            FSNS( XML_xmlns, XML_r ),   "http://schemas.openxmlformats.org/officeDocument/2006/relationships",
            FSEND );

    for ( XclObj* p = rList.First(); p; p = rList.Next() )
    {
        if( IsVmlObject( *p ) )
            continue;
        p->SaveXml( rStrm );
    }

    pDrawing->endElement( FSNS( XML_xdr, XML_wsDr ) );

    rStrm.PopStream();
}


static void SaveVmlObjects( XclExpObjList& rList, XclExpXmlStream& rStrm, sal_Int32& nVmlCount )
{
    if( GetVmlObjectCount( rList ) == 0 )
        return;

    sal_Int32 nDrawing = ++nVmlCount;
    OUString sId;
    sax_fastparser::FSHelperPtr pVmlDrawing = rStrm.CreateOutputStream(
            XclXmlUtils::GetStreamName( "xl/", "drawings/vmlDrawing", nDrawing ),
            XclXmlUtils::GetStreamName( "../", "drawings/vmlDrawing", nDrawing ),
            rStrm.GetCurrentStream()->getOutputStream(),
            "application/vnd.openxmlformats-officedocument.vmlDrawing",
            "http://schemas.openxmlformats.org/officeDocument/2006/relationships/vmlDrawing",
            &sId );

    rStrm.GetCurrentStream()->singleElement( XML_legacyDrawing,
            FSNS( XML_r, XML_id ),  XclXmlUtils::ToOString( sId ).getStr(),
            FSEND );

    rStrm.PushStream( pVmlDrawing );
    pVmlDrawing->startElement( XML_xml,
            FSNS( XML_xmlns, XML_v ),   "urn:schemas-microsoft-com:vml",
            FSNS( XML_xmlns, XML_o ),   "urn:schemas-microsoft-com:office:office",
            FSNS( XML_xmlns, XML_x ),   "urn:schemas-microsoft-com:office:excel",
            FSEND );

    for ( XclObj* p = rList.First(); p; p = rList.Next() )
    {
        if( !IsVmlObject( *p ) )
            continue;
        p->SaveXml( rStrm );
    }

    pVmlDrawing->endElement( XML_xml );

    rStrm.PopStream();
}


void XclExpObjList::SaveXml( XclExpXmlStream& rStrm )
{
    if( pSolverContainer )
        pSolverContainer->SaveXml( rStrm );

    if( Count() == 0 )
        return;

    SaveDrawingMLObjects( *this, rStrm, mnDrawingMLCount );
    SaveVmlObjects( *this, rStrm, mnVmlCount );
}


void XclExpObjList::ResetCounters()
{
    mnDrawingMLCount    = 0;
    mnVmlCount          = 0;
}

// --- class XclObj --------------------------------------------------

XclObj::XclObj( XclExpObjectManager& rObjMgr, sal_uInt16 nObjType, bool bOwnEscher ) :
    XclExpRecord( EXC_ID_OBJ, 26 ),
    mrEscherEx( rObjMgr.GetEscherEx() ),
    pClientTextbox( NULL ),
    pTxo( NULL ),
    mnObjType( nObjType ),
    nObjId(0),
    nGrbit( 0x6011 ),   // AutoLine, AutoFill, Printable, Locked
    bFirstOnSheet( !rObjMgr.HasObj() ),
    mbOwnEscher( bOwnEscher )
{
    //! first object continues the first MSODRAWING record
    if ( bFirstOnSheet )
        pMsodrawing = rObjMgr.GetMsodrawingPerSheet();
    else
        pMsodrawing = new XclExpMsoDrawing( mrEscherEx );
}

XclObj::~XclObj()
{
    if ( !bFirstOnSheet )
        delete pMsodrawing;
    delete pClientTextbox;
    delete pTxo;
}

void XclObj::ImplWriteAnchor( const XclExpRoot& /*rRoot*/, const SdrObject* pSdrObj, const Rectangle* pChildAnchor )
{
    if( pChildAnchor )
    {
        mrEscherEx.AddChildAnchor( *pChildAnchor );
    }
    else if( pSdrObj )
    {
        ::std::auto_ptr< XclExpDffAnchorBase > xDffAnchor( mrEscherEx.CreateDffAnchor( *pSdrObj ) );
        xDffAnchor->WriteDffData( mrEscherEx );
    }
}

void XclObj::SetEscherShapeType( UINT16 nType )
{
//2do: what about the other defined ot... types?
    switch ( nType )
    {
        case ESCHER_ShpInst_Line :
            mnObjType = EXC_OBJTYPE_LINE;
        break;
        case ESCHER_ShpInst_Rectangle :
        case ESCHER_ShpInst_RoundRectangle :
            mnObjType = EXC_OBJTYPE_RECTANGLE;
        break;
        case ESCHER_ShpInst_Ellipse :
            mnObjType = EXC_OBJTYPE_OVAL;
        break;
        case ESCHER_ShpInst_Arc :
            mnObjType = EXC_OBJTYPE_ARC;
        break;
        case ESCHER_ShpInst_TextBox :
            mnObjType = EXC_OBJTYPE_TEXT;
        break;
        case ESCHER_ShpInst_PictureFrame :
            mnObjType = EXC_OBJTYPE_PICTURE;
        break;
        default:
            mnObjType = EXC_OBJTYPE_DRAWING;
    }
}

void XclObj::SetText( const XclExpRoot& rRoot, const SdrTextObj& rObj )
{
    DBG_ASSERT( !pClientTextbox, "XclObj::SetText: already set" );
    if ( !pClientTextbox )
    {
        mrEscherEx.UpdateDffFragmentEnd();
        pClientTextbox = new XclExpMsoDrawing( mrEscherEx );
        mrEscherEx.AddAtom( 0, ESCHER_ClientTextbox );    // TXO record
        mrEscherEx.UpdateDffFragmentEnd();
        pTxo = new XclTxo( rRoot, rObj );
    }
}

void XclObj::WriteBody( XclExpStream& rStrm )
{
    DBG_ASSERT( mnObjType != EXC_OBJTYPE_UNKNOWN, "XclObj::WriteBody - unknown type" );

    // create a substream to be able to create subrecords
    SvMemoryStream aMemStrm;
    ::std::auto_ptr< XclExpStream > pXclStrm( new XclExpStream( aMemStrm, rStrm.GetRoot() ) );

    // write the ftCmo subrecord
    pXclStrm->StartRecord( EXC_ID_OBJCMO, 18 );
    *pXclStrm << mnObjType << nObjId << nGrbit;
    pXclStrm->WriteZeroBytes( 12 );
    pXclStrm->EndRecord();

    // write other subrecords
    WriteSubRecs( *pXclStrm );

    // write the ftEnd subrecord
    pXclStrm->StartRecord( EXC_ID_OBJEND, 0 );
    pXclStrm->EndRecord();

    // copy the data to the OBJ record
    pXclStrm.reset();
    aMemStrm.Seek( 0 );
    rStrm.CopyFromStream( aMemStrm );
}

void XclObj::Save( XclExpStream& rStrm )
{
    // MSODRAWING record (msofbtSpContainer)
    if ( !bFirstOnSheet )
        pMsodrawing->Save( rStrm );

    // OBJ
    XclExpRecord::Save( rStrm );

    // second MSODRAWING record and TXO and CONTINUE records
    SaveTextRecs( rStrm );
}

void XclObj::WriteSubRecs( XclExpStream& /*rStrm*/ )
{
}

void XclObj::SaveTextRecs( XclExpStream& rStrm )
{
    // MSODRAWING record (msofbtClientTextbox)
    if ( pClientTextbox )
        pClientTextbox->Save( rStrm );
    // TXO and CONTINUE records
    if ( pTxo )
        pTxo->Save( rStrm );
}

  // --- class XclObjComment ------------------------------------------

XclObjComment::XclObjComment( XclExpObjectManager& rObjMgr, const Rectangle& rRect, const EditTextObject& rEditObj, SdrCaptionObj* pCaption, bool bVisible, const ScAddress& rAddress, Rectangle &rFrom, Rectangle &rTo ) :
    XclObj( rObjMgr, EXC_OBJTYPE_NOTE, true )
            , maScPos( rAddress )
            , mpCaption( static_cast< SdrCaptionObj* >( pCaption->Clone() ) )
            , mbVisible( bVisible )
            , maFrom ( rFrom )
            , maTo ( rTo )
{
    ProcessEscherObj( rObjMgr.GetRoot(), rRect, pCaption, bVisible);
    // TXO
    pTxo = new XclTxo( rObjMgr.GetRoot(), rEditObj, pCaption );
}

static void lcl_FillProps( EscherPropertyContainer& rPropOpt, SdrObject* pCaption, bool bVisible )
{
    if( pCaption )
    {
        Reference< XShape > aXShape = GetXShapeForSdrObject( pCaption );
        Reference< XPropertySet > aXPropSet( aXShape, UNO_QUERY );
        if( aXPropSet.is() )
        {
            rPropOpt.CreateFillProperties( aXPropSet,  sal_True);

            rPropOpt.AddOpt( ESCHER_Prop_lTxid, 0 );                        // undocumented
            rPropOpt.AddOpt( 0x0158, 0x00000000 );                          // undocumented

            sal_uInt32 nValue = 0;
            if( !rPropOpt.GetOpt( ESCHER_Prop_FitTextToShape, nValue ) )
                rPropOpt.AddOpt( ESCHER_Prop_FitTextToShape, 0x00080008 );      // bool field

            if( rPropOpt.GetOpt( ESCHER_Prop_fillColor, nValue ) )
            {
                // If the Colour is the same as the 'ToolTip' System colour then
                // use the default rather than the explicit colour value. This will
                // be incorrect where user has chosen to use this colour explicity.
                Color aColor = Color( (BYTE)nValue, (BYTE)( nValue >> 8 ), (BYTE)( nValue >> 16 ) );
                const StyleSettings& rSett = Application::GetSettings().GetStyleSettings();
                if( aColor == rSett.GetHelpColor().GetColor() )
                {
                    rPropOpt.AddOpt( ESCHER_Prop_fillColor, 0x08000050 );
                    rPropOpt.AddOpt( ESCHER_Prop_fillBackColor, 0x08000050 );
                }
            }
            else
                rPropOpt.AddOpt( ESCHER_Prop_fillColor, 0x08000050 );

            if( !rPropOpt.GetOpt( ESCHER_Prop_fillBackColor, nValue ) )
                rPropOpt.AddOpt( ESCHER_Prop_fillBackColor, 0x08000050 );
            if( !rPropOpt.GetOpt( ESCHER_Prop_fNoFillHitTest, nValue ) )
                rPropOpt.AddOpt( ESCHER_Prop_fNoFillHitTest, 0x00110010 );      // bool field
            if( !rPropOpt.GetOpt( ESCHER_Prop_shadowColor, nValue ) )
                rPropOpt.AddOpt( ESCHER_Prop_shadowColor, 0x00000000 );
            if( !rPropOpt.GetOpt( ESCHER_Prop_fshadowObscured, nValue ) )       // bool field
                rPropOpt.AddOpt( ESCHER_Prop_fshadowObscured, 0x00030003 );     // bool field
        }
    }

    sal_uInt32 nFlags = 0x000A0000;
    ::set_flag( nFlags, sal_uInt32(2), !bVisible );
    rPropOpt.AddOpt( ESCHER_Prop_fPrint, nFlags );                  // bool field
}

void XclObjComment::ProcessEscherObj( const XclExpRoot& rRoot, const Rectangle& rRect, SdrObject* pCaption, const bool bVisible )
{
    EscherPropertyContainer aPropOpt;


    lcl_FillProps( aPropOpt, pCaption, bVisible );

    nGrbit = 0;     // all off: AutoLine, AutoFill, Printable, Locked
    mrEscherEx.OpenContainer( ESCHER_SpContainer );
    mrEscherEx.AddShape( ESCHER_ShpInst_TextBox, SHAPEFLAG_HAVEANCHOR | SHAPEFLAG_HAVESPT );
    aPropOpt.Commit( mrEscherEx.GetStream() );

    XclExpDffNoteAnchor( rRoot, rRect ).WriteDffData( mrEscherEx );

    mrEscherEx.AddAtom( 0, ESCHER_ClientData );                        // OBJ record
    mrEscherEx.UpdateDffFragmentEnd();

    //! Be sure to construct the MSODRAWING ClientTextbox record _after_ the
    //! base OBJ's MSODRAWING record Escher data is completed.
    pClientTextbox = new XclExpMsoDrawing( mrEscherEx );
    mrEscherEx.AddAtom( 0, ESCHER_ClientTextbox );    // TXO record
    mrEscherEx.UpdateDffFragmentEnd();
    mrEscherEx.CloseContainer();   // ESCHER_SpContainer
}

XclObjComment::~XclObjComment()
{
}

void XclObjComment::Save( XclExpStream& rStrm )
{
    // content of this record
    XclObj::Save( rStrm );
}

class VmlCommentExporter : public VMLExport
{
    ScAddress           maScPos;
    SdrCaptionObj*      mpCaption;
    bool                mbVisible;
    Rectangle           maFrom;
    Rectangle           maTo;

public:
                        VmlCommentExporter ( sax_fastparser::FSHelperPtr p, ScAddress aScPos, SdrCaptionObj* pCaption, bool bVisible, Rectangle &aFrom, Rectangle &aTo );
protected:
    virtual void        Commit( EscherPropertyContainer& rProps, const Rectangle& rRect );
    virtual sal_Int32   StartShape();
    virtual void        EndShape( sal_Int32 nShapeElement );
};


VmlCommentExporter::VmlCommentExporter( sax_fastparser::FSHelperPtr p, ScAddress aScPos, SdrCaptionObj* pCaption,
                                        bool bVisible, Rectangle &aFrom, Rectangle &aTo )
    : VMLExport( p )
    , maScPos( aScPos )
    , mpCaption( pCaption )
    , mbVisible( bVisible )
    , maFrom ( aFrom )
    , maTo ( aTo )
{
}

void VmlCommentExporter::Commit( EscherPropertyContainer& rProps, const Rectangle& rRect )
{
    lcl_FillProps( rProps, mpCaption, mbVisible );
    rProps.AddOpt( ESCHER_Prop_fHidden, 1 );            // bool field

    VMLExport::Commit( rProps, rRect );
}

sal_Int32 VmlCommentExporter::StartShape()
{
    AddShapeAttribute( XML_type, OString( "#_x0000_t202") );

    sal_Int32 nId = VMLExport::StartShape();

    return nId;
}

void VmlCommentExporter::EndShape( sal_Int32 nShapeElement )
{
    char pAnchor[100];
    sax_fastparser::FSHelperPtr pVmlDrawing = GetFS();
    snprintf( pAnchor, 100, "%ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld",
                  maFrom.Left(), maFrom.Top(), maFrom.Right(), maFrom.Bottom(),
                  maTo.Left(), maTo.Top(), maTo.Right(), maTo.Bottom() );

    pVmlDrawing->startElement( FSNS( XML_x, XML_ClientData ),
            XML_ObjectType, "Note",
            FSEND );
    pVmlDrawing->singleElement( FSNS( XML_x, XML_MoveWithCells ),
            FSEND );
    pVmlDrawing->singleElement( FSNS( XML_x, XML_SizeWithCells ),
            FSEND );
    XclXmlUtils::WriteElement( pVmlDrawing, FSNS( XML_x, XML_Anchor ), pAnchor );
    XclXmlUtils::WriteElement( pVmlDrawing, FSNS( XML_x, XML_AutoFill ), "False" );
    XclXmlUtils::WriteElement( pVmlDrawing, FSNS( XML_x, XML_Row ), maScPos.Row() );
    XclXmlUtils::WriteElement( pVmlDrawing, FSNS( XML_x, XML_Column ), sal_Int32( maScPos.Col() ) );
    pVmlDrawing->endElement( FSNS( XML_x, XML_ClientData ) );

    VMLExport::EndShape( nShapeElement );
}

void XclObjComment::SaveXml( XclExpXmlStream& rStrm )
{
    VmlCommentExporter aCommentExporter( rStrm.GetCurrentStream(), maScPos, mpCaption.get(), mbVisible, maFrom, maTo );
    aCommentExporter.AddSdrObject( *mpCaption );
}


// --- class XclObjDropDown ------------------------------------------

XclObjDropDown::XclObjDropDown( XclExpObjectManager& rObjMgr, const ScAddress& rPos, BOOL bFilt ) :
    XclObj( rObjMgr, EXC_OBJTYPE_DROPDOWN, true ),
    bIsFiltered( bFilt )
{
    SetLocked( TRUE );
    SetPrintable( FALSE );
    SetAutoFill( TRUE );
    SetAutoLine( FALSE );
    nGrbit |= 0x0100;   // undocumented
    mrEscherEx.OpenContainer( ESCHER_SpContainer );
    mrEscherEx.AddShape( ESCHER_ShpInst_HostControl, SHAPEFLAG_HAVEANCHOR | SHAPEFLAG_HAVESPT );
    EscherPropertyContainer aPropOpt;
    aPropOpt.AddOpt( ESCHER_Prop_LockAgainstGrouping, 0x01040104 ); // bool field
    aPropOpt.AddOpt( ESCHER_Prop_FitTextToShape, 0x00080008 );      // bool field
    aPropOpt.AddOpt( ESCHER_Prop_fNoFillHitTest, 0x00010000 );      // bool field
    aPropOpt.AddOpt( ESCHER_Prop_fNoLineDrawDash, 0x00080000 );     // bool field
    aPropOpt.AddOpt( ESCHER_Prop_fPrint, 0x000A0000 );              // bool field
    aPropOpt.Commit( mrEscherEx.GetStream() );

    XclExpDffDropDownAnchor( rObjMgr.GetRoot(), rPos ).WriteDffData( mrEscherEx );

    mrEscherEx.AddAtom( 0, ESCHER_ClientData );                        // OBJ record
    mrEscherEx.UpdateDffFragmentEnd();
    mrEscherEx.CloseContainer();   // ESCHER_SpContainer

    // old size + ftSbs + ftLbsData
    AddRecSize( 24 + 20 );
}

XclObjDropDown::~XclObjDropDown()
{
}

void XclObjDropDown::WriteSubRecs( XclExpStream& rStrm )
{
    // ftSbs subrecord - Scroll bars (dummy)
    rStrm.StartRecord( EXC_ID_OBJSBS, 20 );
    rStrm.WriteZeroBytes( 20 );
    rStrm.EndRecord();

    // ftLbsData subrecord - Listbox data
    sal_uInt16 nDropDownFlags = 0;
    ::insert_value( nDropDownFlags, EXC_OBJ_DROPDOWN_SIMPLE, 0, 2 );
    ::set_flag( nDropDownFlags, EXC_OBJ_DROPDOWN_FILTERED, bIsFiltered );
    rStrm.StartRecord( EXC_ID_OBJLBSDATA, 16 );
    rStrm   << (UINT32)0 << (UINT16)0 << (UINT16)0x0301 << (UINT16)0
            << nDropDownFlags << sal_uInt16( 20 ) << sal_uInt16( 130 );
    rStrm.EndRecord();
}

// --- class XclTxo --------------------------------------------------

sal_uInt8 lcl_GetHorAlignFromItemSet( const SfxItemSet& rItemSet )
{
    sal_uInt8 nHorAlign = EXC_OBJ_HOR_LEFT;

    switch( static_cast< const SvxAdjustItem& >( rItemSet.Get( EE_PARA_JUST ) ).GetAdjust() )
    {
        case SVX_ADJUST_LEFT:   nHorAlign = EXC_OBJ_HOR_LEFT;      break;
        case SVX_ADJUST_CENTER: nHorAlign = EXC_OBJ_HOR_CENTER;    break;
        case SVX_ADJUST_RIGHT:  nHorAlign = EXC_OBJ_HOR_RIGHT;     break;
        case SVX_ADJUST_BLOCK:  nHorAlign = EXC_OBJ_HOR_JUSTIFY;   break;
        default:;
    }
    return nHorAlign;
}

sal_uInt8 lcl_GetVerAlignFromItemSet( const SfxItemSet& rItemSet )
{
    sal_uInt8 nVerAlign = EXC_OBJ_VER_TOP;

    switch( static_cast< const SdrTextVertAdjustItem& >( rItemSet.Get( SDRATTR_TEXT_VERTADJUST ) ).GetValue() )
    {
        case SDRTEXTVERTADJUST_TOP:     nVerAlign = EXC_OBJ_VER_TOP;       break;
        case SDRTEXTVERTADJUST_CENTER:  nVerAlign = EXC_OBJ_VER_CENTER;    break;
        case SDRTEXTVERTADJUST_BOTTOM:  nVerAlign = EXC_OBJ_VER_BOTTOM;    break;
        case SDRTEXTVERTADJUST_BLOCK:   nVerAlign = EXC_OBJ_VER_JUSTIFY;   break;
    }
    return nVerAlign;
}

XclTxo::XclTxo( const String& rString, sal_uInt16 nFontIx ) :
    mpString( new XclExpString( rString ) ),
    mnRotation( EXC_OBJ_ORIENT_NONE ),
    mnHorAlign( EXC_OBJ_HOR_LEFT ),
    mnVerAlign( EXC_OBJ_VER_TOP )
{
    if( mpString->Len() )
    {
        // If there is text, Excel *needs* the 2nd CONTINUE record with at least two format runs
        mpString->AppendFormat( 0, nFontIx );
        mpString->AppendFormat( mpString->Len(), EXC_FONT_APP );
    }
}

XclTxo::XclTxo( const XclExpRoot& rRoot, const SdrTextObj& rTextObj ) :
    mpString( XclExpStringHelper::CreateString( rRoot, rTextObj ) ),
    mnRotation( EXC_OBJ_ORIENT_NONE ),
    mnHorAlign( EXC_OBJ_HOR_LEFT ),
    mnVerAlign( EXC_OBJ_VER_TOP )
{
    // additional alignment and orientation items
    const SfxItemSet& rItemSet = rTextObj.GetMergedItemSet();

    // horizontal alignment
    SetHorAlign( lcl_GetHorAlignFromItemSet( rItemSet ) );

    // vertical alignment
    SetVerAlign( lcl_GetVerAlignFromItemSet( rItemSet ) );

    // rotation
    long nAngle = rTextObj.GetRotateAngle();
    if( (4500 < nAngle) && (nAngle < 13500) )
        mnRotation = EXC_OBJ_ORIENT_90CCW;
    else if( (22500 < nAngle) && (nAngle < 31500) )
        mnRotation = EXC_OBJ_ORIENT_90CW;
    else
        mnRotation = EXC_OBJ_ORIENT_NONE;
}

XclTxo::XclTxo( const XclExpRoot& rRoot, const EditTextObject& rEditObj, SdrObject* pCaption ) :
    mpString( XclExpStringHelper::CreateString( rRoot, rEditObj ) ),
    mnRotation( EXC_OBJ_ORIENT_NONE ),
    mnHorAlign( EXC_OBJ_HOR_LEFT ),
    mnVerAlign( EXC_OBJ_VER_TOP )
{
    if(pCaption)
    {
        // Excel has one alignment per NoteObject while Calc supports
        // one alignment per paragraph - use the first paragraph
        // alignment (if set) as our overall alignment.
        String aParaText( rEditObj.GetText( 0 ) );
        if( aParaText.Len() )
        {
            SfxItemSet aSet( rEditObj.GetParaAttribs( 0));
            const SfxPoolItem* pItem = NULL;
            if( aSet.GetItemState( EE_PARA_JUST, TRUE, &pItem ) == SFX_ITEM_SET )
            {
                SvxAdjust eEEAlign = static_cast< const SvxAdjustItem& >( *pItem ).GetAdjust();
                pCaption->SetMergedItem( SvxAdjustItem( eEEAlign, EE_PARA_JUST ) );
            }
        }
        const SfxItemSet& rItemSet = pCaption->GetMergedItemSet();

        // horizontal alignment
        SetHorAlign( lcl_GetHorAlignFromItemSet( rItemSet ) );

        // vertical alignment
        SetVerAlign( lcl_GetVerAlignFromItemSet( rItemSet ) );

        // orientation alignment
        const SvxWritingModeItem& rItem = static_cast< const SvxWritingModeItem& >( rItemSet.Get( SDRATTR_TEXTDIRECTION ) );
        if( rItem.GetValue() == com::sun::star::text::WritingMode_TB_RL )
            mnRotation = EXC_OBJ_ORIENT_90CW;
    }
}

void XclTxo::SaveCont( XclExpStream& rStrm )
{
    DBG_ASSERT( mpString.get(), "XclTxo::SaveCont - missing string" );

    // #i96858# do not save existing string formatting if text is empty
    sal_uInt16 nRunLen = mpString->IsEmpty() ? 0 : (8 * mpString->GetFormatsCount());
    // alignment
    sal_uInt16 nFlags = 0;
    ::insert_value( nFlags, mnHorAlign, 1, 3 );
    ::insert_value( nFlags, mnVerAlign, 4, 3 );

    rStrm << nFlags << mnRotation;
    rStrm.WriteZeroBytes( 6 );
    rStrm << mpString->Len() << nRunLen << sal_uInt32( 0 );
}

void XclTxo::Save( XclExpStream& rStrm )
{
    // Write the TXO part
    ExcRecord::Save( rStrm );

    // CONTINUE records are only written if there is some text
    if( !mpString->IsEmpty() )
    {
        // CONTINUE for character array
        rStrm.StartRecord( EXC_ID_CONT, mpString->GetBufferSize() + 1 );
        rStrm << static_cast< sal_uInt8 >( mpString->GetFlagField() & EXC_STRF_16BIT ); // only Unicode flag
        mpString->WriteBuffer( rStrm );
        rStrm.EndRecord();

        // CONTINUE for formatting runs
        rStrm.StartRecord( EXC_ID_CONT, 8 * mpString->GetFormatsCount() );
        const XclFormatRunVec& rFormats = mpString->GetFormats();
        for( XclFormatRunVec::const_iterator aIt = rFormats.begin(), aEnd = rFormats.end(); aIt != aEnd; ++aIt )
            rStrm << aIt->mnChar << aIt->mnFontIdx << sal_uInt32( 0 );
        rStrm.EndRecord();
    }
}

UINT16 XclTxo::GetNum() const
{
    return EXC_ID_TXO;
}

sal_Size XclTxo::GetLen() const
{
    return 18;
}

// --- class XclObjOle -------------------------------------------

XclObjOle::XclObjOle( XclExpObjectManager& rObjMgr, const SdrObject& rObj ) :
    XclObj( rObjMgr, EXC_OBJTYPE_PICTURE ),
    rOleObj( rObj ),
    pRootStorage( rObjMgr.GetRoot().GetRootStorage() )
{
}

XclObjOle::~XclObjOle()
{
}

void XclObjOle::WriteSubRecs( XclExpStream& rStrm )
{
    // write only as embedded, not linked
    String          aStorageName( RTL_CONSTASCII_USTRINGPARAM( "MBD" ) );
    sal_Char        aBuf[ sizeof(UINT32) * 2 + 1 ];
    // FIXME Eeek! Is this just a way to get a unique id?
    UINT32          nPictureId = UINT32(sal_uIntPtr(this) >> 2);
    sprintf( aBuf, "%08X", static_cast< unsigned int >( nPictureId ) );        // #100211# - checked
    aStorageName.AppendAscii( aBuf );
    SotStorageRef    xOleStg = pRootStorage->OpenSotStorage( aStorageName,
                            STREAM_READWRITE| STREAM_SHARE_DENYALL );
    if( xOleStg.Is() )
    {
        uno::Reference < embed::XEmbeddedObject > xObj( ((SdrOle2Obj&)rOleObj).GetObjRef() );
        if ( xObj.is() )
        {
            // set version to "old" version, because it must be
            // saved in MS notation.
            UINT32                  nFl = 0;
            SvtFilterOptions*       pFltOpts = SvtFilterOptions::Get();
            if( pFltOpts )
            {
                if( pFltOpts->IsMath2MathType() )
                    nFl |= OLE_STARMATH_2_MATHTYPE;

                if( pFltOpts->IsWriter2WinWord() )
                    nFl |= OLE_STARWRITER_2_WINWORD;

                if( pFltOpts->IsCalc2Excel() )
                    nFl |= OLE_STARCALC_2_EXCEL;

                if( pFltOpts->IsImpress2PowerPoint() )
                    nFl |= OLE_STARIMPRESS_2_POWERPOINT;
            }

            SvxMSExportOLEObjects   aOLEExpFilt( nFl );
            aOLEExpFilt.ExportOLEObject( xObj, *xOleStg );

            // OBJCF subrecord, undocumented as usual
            rStrm.StartRecord( EXC_ID_OBJCF, 2 );
            rStrm << UINT16(0x0002);
            rStrm.EndRecord();

            // OBJFLAGS subrecord, undocumented as usual
            rStrm.StartRecord( EXC_ID_OBJFLAGS, 2 );
            sal_uInt16 nFlags = EXC_OBJ_PIC_MANUALSIZE;
            ::set_flag( nFlags, EXC_OBJ_PIC_SYMBOL, ((SdrOle2Obj&)rOleObj).GetAspect() == embed::Aspects::MSOLE_ICON );
            rStrm << nFlags;
            rStrm.EndRecord();

            // OBJPICTFMLA subrecord, undocumented as usual
            XclExpString aName( xOleStg->GetUserName() );
            UINT16 nPadLen = (UINT16)(aName.GetSize() & 0x01);
            UINT16 nFmlaLen = static_cast< sal_uInt16 >( 12 + aName.GetSize() + nPadLen );
            UINT16 nSubRecLen = nFmlaLen + 6;

            rStrm.StartRecord( EXC_ID_OBJPICTFMLA, nSubRecLen );
            rStrm   << nFmlaLen
                    << sal_uInt16( 5 ) << sal_uInt32( 0 ) << sal_uInt8( 2 )
                    << sal_uInt32( 0 ) << sal_uInt8( 3 )
                    << aName;
            if( nPadLen )
                rStrm << sal_uInt8( 0 );       // pad byte
            rStrm << nPictureId;
            rStrm.EndRecord();
        }
    }
}

void XclObjOle::Save( XclExpStream& rStrm )
{
    // content of this record
    XclObj::Save( rStrm );
}

// --- class XclObjAny -------------------------------------------

XclObjAny::XclObjAny( XclExpObjectManager& rObjMgr, const Reference< XShape >& rShape )
    : XclObj( rObjMgr, EXC_OBJTYPE_UNKNOWN )
    , mxShape( rShape )
{
}

XclObjAny::~XclObjAny()
{
}

void XclObjAny::WriteSubRecs( XclExpStream& rStrm )
{
    if( mnObjType == EXC_OBJTYPE_GROUP )
        // ftGmo subrecord
        rStrm << EXC_ID_OBJGMO << UINT16(2) << UINT16(0);
}

void XclObjAny::Save( XclExpStream& rStrm )
{
    if( mnObjType == EXC_OBJTYPE_GROUP )
        // old size + ftGmo
        AddRecSize( 6 );

    // content of this record
    XclObj::Save( rStrm );
}

// --- class ExcBof8_Base --------------------------------------------

ExcBof8_Base::ExcBof8_Base()
{
    nVers           = 0x0600;
    nRupBuild       = 0x0dbb;
    nRupYear        = 0x07cc;
//  nFileHistory    = 0x00000001;   // last edited by Microsoft Excel for Windows
    nFileHistory    = 0x00000000;
    nLowestBiffVer  = 0x00000006;   // Biff8
}
void XclObjAny::WriteFromTo( XclExpXmlStream& rStrm, const Reference< XShape >& rShape, SCTAB nTab )
{
    sax_fastparser::FSHelperPtr pDrawing = rStrm.GetCurrentStream();

    awt::Point  aTopLeft    = rShape->getPosition();
    awt::Size   aSize       = rShape->getSize();
    Rectangle   aLocation( aTopLeft.X, aTopLeft.Y, aTopLeft.X + aSize.Width, aTopLeft.Y + aSize.Height );
    ScRange     aRange      = rStrm.GetRoot().GetDoc().GetRange( nTab, aLocation );
    Rectangle   aRangeRect  = rStrm.GetRoot().GetDoc().GetMMRect( aRange.aStart.Col(), aRange.aStart.Row(),
            aRange.aEnd.Col()-1, aRange.aEnd.Row()-1,
            nTab );


    pDrawing->startElement( FSNS( XML_xdr, XML_from ),
            FSEND );
    XclXmlUtils::WriteElement( pDrawing, FSNS( XML_xdr, XML_col ), (sal_Int32) aRange.aStart.Col() );
    XclXmlUtils::WriteElement( pDrawing, FSNS( XML_xdr, XML_colOff ),
            MM100toEMU( aLocation.Left() - aRangeRect.Left() ) );
    XclXmlUtils::WriteElement( pDrawing, FSNS( XML_xdr, XML_row ), (sal_Int32) aRange.aStart.Row() );
    XclXmlUtils::WriteElement( pDrawing, FSNS( XML_xdr, XML_rowOff ),
            MM100toEMU( aLocation.Top() - aRangeRect.Top() ) );
    pDrawing->endElement( FSNS( XML_xdr, XML_from ) );

    pDrawing->startElement( FSNS( XML_xdr, XML_to ),
            FSEND );
    XclXmlUtils::WriteElement( pDrawing, FSNS( XML_xdr, XML_col ), (sal_Int32) aRange.aEnd.Col() );
    XclXmlUtils::WriteElement( pDrawing, FSNS( XML_xdr, XML_colOff ),
            MM100toEMU( aLocation.Right() - aRangeRect.Right() ) );
    XclXmlUtils::WriteElement( pDrawing, FSNS( XML_xdr, XML_row ), (sal_Int32) aRange.aEnd.Row() );
    XclXmlUtils::WriteElement( pDrawing, FSNS( XML_xdr, XML_rowOff ),
            MM100toEMU( aLocation.Bottom() - aRangeRect.Bottom() ) );
    pDrawing->endElement( FSNS( XML_xdr, XML_to ) );
}

void XclObjAny::WriteFromTo( XclExpXmlStream& rStrm, const XclObjAny& rObj )
{
    WriteFromTo( rStrm, rObj.GetShape(), rObj.GetTab() );
}

static void
WritePicPr( sax_fastparser::FSHelperPtr pDrawing, sal_Int32 nId, Reference< XPropertySet > xPropSet )
{
    pDrawing->startElement( FSNS( XML_xdr, XML_nvPicPr ),
            FSEND );
    OUString sName, sDescr;
    xPropSet->getPropertyValue( XclXmlUtils::ToOUString( "Name" ) ) >>= sName;
    xPropSet->getPropertyValue( XclXmlUtils::ToOUString( "Description" ) ) >>= sDescr;
    pDrawing->singleElement( FSNS( XML_xdr, XML_cNvPr ),
            XML_id,     OString::valueOf( nId ).getStr(),
            XML_name,   XclXmlUtils::ToOString( sName ).getStr(),
            XML_descr,  XclXmlUtils::ToOString( sDescr ).getStr(),
            FSEND );
    // OOXTODO: //xdr:cNvPr children: XML_extLst, XML_hlinkClick, XML_hlinkHover
    pDrawing->startElement( FSNS( XML_xdr, XML_cNvPicPr ),
            // OOXTODO: FSNS( XML_xdr, XML_preferRelativeSize )
            FSEND );
    // OOXTODO: XML_extLst
    pDrawing->singleElement( FSNS( XML_a, XML_picLocks ),
            // OOXTODO: XML_noGrp,
            // OOXTODO: XML_noSelect,
            // OOXTODO: XML_noRot,
            // OOXTODO: XML_noChangeAspect,
            // OOXTODO: XML_noMove,
            // OOXTODO: XML_noResize,
            // OOXTODO: XML_noEditPoints,
            // OOXTODO: XML_noAdjustHandles,
            // OOXTODO: XML_noChangeArrowheads,
            // OOXTODO: XML_noChangeShapeType,
            // OOXTODO: XML_noCrop,
            FSEND );
    pDrawing->endElement( FSNS( XML_xdr, XML_cNvPicPr ) );
    pDrawing->endElement( FSNS( XML_xdr, XML_nvPicPr ) );
}


static const char*
GetEditAs( XclObjAny& rObj )
{
    if( const SdrObject* pShape = EscherEx::GetSdrObject( rObj.GetShape() ) )
    {
        // OOXTODO: returning "twoCell"
        switch( ScDrawLayer::GetAnchor( pShape ) )
        {
            case SCA_CELL:  return "oneCell";
            default:        break;
        }
    }
    return "absolute";
}


void XclObjAny::SaveXml( XclExpXmlStream& rStrm )
{
    if( !mxShape.is() )
        return;

    sax_fastparser::FSHelperPtr pDrawing = rStrm.GetCurrentStream();

    ShapeExport aDML( XML_xdr, pDrawing, &rStrm, DrawingML::DOCUMENT_XLSX );

    pDrawing->startElement( FSNS( XML_xdr, XML_twoCellAnchor ), // OOXTODO: oneCellAnchor, absoluteAnchor
            XML_editAs, GetEditAs( *this ),
            FSEND );
    Reference< XPropertySet > xPropSet( mxShape, UNO_QUERY );
    if (xPropSet.is())
    {
        WriteFromTo( rStrm, *this );
        aDML.WriteShape( mxShape );
    }

    pDrawing->singleElement( FSNS( XML_xdr, XML_clientData),
            // OOXTODO: XML_fLocksWithSheet
            // OOXTODO: XML_fPrintsWithSheet
            FSEND );
    pDrawing->endElement( FSNS( XML_xdr, XML_twoCellAnchor ) );
}



void ExcBof8_Base::SaveCont( XclExpStream& rStrm )
{
    rStrm.DisableEncryption();
    rStrm   << nVers << nDocType << nRupBuild << nRupYear
            << nFileHistory << nLowestBiffVer;
}


UINT16 ExcBof8_Base::GetNum() const
{
    return 0x0809;
}


sal_Size ExcBof8_Base::GetLen() const
{
    return 16;
}


// --- class ExcBof8 -------------------------------------------------

ExcBof8::ExcBof8()
{
    nDocType = 0x0010;
}


// --- class ExcBofW8 ------------------------------------------------

ExcBofW8::ExcBofW8()
{
    nDocType = 0x0005;
}


// --- class ExcBundlesheet8 -----------------------------------------

ExcBundlesheet8::ExcBundlesheet8( RootData& rRootData, SCTAB _nTab ) :
    ExcBundlesheetBase( rRootData, static_cast<sal_uInt16>(_nTab) ),
    sUnicodeName( rRootData.pER->GetTabInfo().GetScTabName( _nTab ) )
{
}


ExcBundlesheet8::ExcBundlesheet8( const String& rString ) :
    ExcBundlesheetBase(),
    sUnicodeName( rString )
{
}


XclExpString ExcBundlesheet8::GetName() const
{
    return XclExpString( sUnicodeName, EXC_STR_8BITLENGTH );
}


void ExcBundlesheet8::SaveCont( XclExpStream& rStrm )
{
    nOwnPos = rStrm.GetSvStreamPos();
    // write dummy position, real position comes later
    rStrm.DisableEncryption();
    rStrm << sal_uInt32(0);
    rStrm.EnableEncryption();
    rStrm << nGrbit << GetName();
}


sal_Size ExcBundlesheet8::GetLen() const
{   // Text max 255 chars
    return 8 + GetName().GetBufferSize();
}


void ExcBundlesheet8::SaveXml( XclExpXmlStream& rStrm )
{
    OUString sId;
    rStrm.CreateOutputStream(
            XclXmlUtils::GetStreamName( "xl/", "worksheets/sheet", nTab+1),
            XclXmlUtils::GetStreamName( NULL, "worksheets/sheet", nTab+1),
            rStrm.GetCurrentStream()->getOutputStream(),
            "application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml",
            "http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet",
            &sId );

    rStrm.GetCurrentStream()->singleElement( XML_sheet,
            XML_name,               XclXmlUtils::ToOString( sUnicodeName ).getStr(),
            XML_sheetId,            rtl::OString::valueOf( (sal_Int32)( nTab+1 ) ).getStr(),
            XML_state,              nGrbit == 0x0000 ? "visible" : "hidden",
            FSNS( XML_r, XML_id ),  XclXmlUtils::ToOString( sId ).getStr(),
            FSEND );
}



// --- class XclObproj -----------------------------------------------

UINT16 XclObproj::GetNum() const
{
    return 0x00D3;
}


sal_Size XclObproj::GetLen() const
{
    return 0;
}


// ---- class XclCodename --------------------------------------------

XclCodename::XclCodename( const String& r ) : aName( r )
{
}


void XclCodename::SaveCont( XclExpStream& rStrm )
{
    rStrm << aName;
}


UINT16 XclCodename::GetNum() const
{
    return 0x01BA;
}


sal_Size XclCodename::GetLen() const
{
    return aName.GetSize();
}



// ---- Scenarios ----------------------------------------------------

ExcEScenarioCell::ExcEScenarioCell( UINT16 nC, UINT16 nR, const String& rTxt ) :
        nCol( nC ),
        nRow( nR ),
        sText( rTxt, EXC_STR_DEFAULT, 255 )
{
}

void ExcEScenarioCell::WriteAddress( XclExpStream& rStrm )
{
    rStrm << nRow << nCol;
}

void ExcEScenarioCell::WriteText( XclExpStream& rStrm )
{
    rStrm << sText;
}

void ExcEScenarioCell::SaveXml( XclExpXmlStream& rStrm )
{
    rStrm.GetCurrentStream()->singleElement( XML_inputCells,
            // OOXTODO: XML_deleted,
            // OOXTODO: XML_numFmtId,
            XML_r,      XclXmlUtils::ToOString( ScAddress( nCol, nRow, 0 ) ).getStr(),
            // OOXTODO: XML_undone,
            XML_val,    XclXmlUtils::ToOString( sText ).getStr(),
            FSEND );
}




ExcEScenario::ExcEScenario( const XclExpRoot& rRoot, SCTAB nTab )
{
    String  sTmpName;
    String  sTmpComm;
    Color   aDummyCol;
    USHORT  nFlags;

    ScDocument& rDoc = rRoot.GetDoc();
    rDoc.GetName( nTab, sTmpName );
    sName.Assign( sTmpName, EXC_STR_8BITLENGTH );
    nRecLen = 8 + sName.GetBufferSize();

    rDoc.GetScenarioData( nTab, sTmpComm, aDummyCol, nFlags );
    sComment.Assign( sTmpComm, EXC_STR_DEFAULT, 255 );
    if( sComment.Len() )
        nRecLen += sComment.GetSize();
    nProtected = (nFlags & SC_SCENARIO_PROTECT) ? 1 : 0;

    sUserName.Assign( rRoot.GetUserName(), EXC_STR_DEFAULT, 255 );
    nRecLen += sUserName.GetSize();

    const ScRangeList* pRList = rDoc.GetScenarioRanges( nTab );
    if( !pRList )
        return;

    BOOL    bContLoop = TRUE;
    SCROW   nRow;
    SCCOL   nCol;
    String  sText;
    double  fVal;

    for( UINT32 nRange = 0; (nRange < pRList->Count()) && bContLoop; nRange++ )
    {
        const ScRange* pRange = pRList->GetObject( nRange );
        for( nRow = pRange->aStart.Row(); (nRow <= pRange->aEnd.Row()) && bContLoop; nRow++ )
            for( nCol = pRange->aStart.Col(); (nCol <= pRange->aEnd.Col()) && bContLoop; nCol++ )
            {
                if( rDoc.HasValueData( nCol, nRow, nTab ) )
                {
                    rDoc.GetValue( nCol, nRow, nTab, fVal );
                    sText = ::rtl::math::doubleToUString( fVal,
                            rtl_math_StringFormat_Automatic,
                            rtl_math_DecimalPlaces_Max,
                            ScGlobal::pLocaleData->getNumDecimalSep().GetChar(0),
                            TRUE );
                }
                else
                    rDoc.GetString( nCol, nRow, nTab, sText );
                bContLoop = Append( static_cast<sal_uInt16>(nCol),
                        static_cast<sal_uInt16>(nRow), sText );
            }
    }
}

ExcEScenario::~ExcEScenario()
{
    for( ExcEScenarioCell* pCell = _First(); pCell; pCell = _Next() )
        delete pCell;
}

BOOL ExcEScenario::Append( UINT16 nCol, UINT16 nRow, const String& rTxt )
{
    if( List::Count() == EXC_SCEN_MAXCELL )
        return FALSE;

    ExcEScenarioCell* pCell = new ExcEScenarioCell( nCol, nRow, rTxt );
    List::Insert( pCell, LIST_APPEND );
    nRecLen += 6 + pCell->GetStringBytes();        // 4 bytes address, 2 bytes ifmt
    return TRUE;
}

void ExcEScenario::SaveCont( XclExpStream& rStrm )
{
    rStrm   << (UINT16) List::Count()       // number of cells
            << nProtected                   // fProtection
            << (UINT8) 0                    // fHidden
            << (UINT8) sName.Len()          // length of scen name
            << (UINT8) sComment.Len()       // length of comment
            << (UINT8) sUserName.Len();     // length of user name
    sName.WriteFlagField( rStrm );
    sName.WriteBuffer( rStrm );

    rStrm << sUserName;

    if( sComment.Len() )
        rStrm << sComment;

    ExcEScenarioCell* pCell;
    for( pCell = _First(); pCell; pCell = _Next() )
        pCell->WriteAddress( rStrm );           // pos of cell
    for( pCell = _First(); pCell; pCell = _Next() )
        pCell->WriteText( rStrm );              // string content
    rStrm.SetSliceSize( 2 );
    rStrm.WriteZeroBytes( 2 * List::Count() );  // date format
}

UINT16 ExcEScenario::GetNum() const
{
    return 0x00AF;
}

sal_Size ExcEScenario::GetLen() const
{
    return nRecLen;
}

void ExcEScenario::SaveXml( XclExpXmlStream& rStrm )
{
    sax_fastparser::FSHelperPtr& rWorkbook = rStrm.GetCurrentStream();
    rWorkbook->startElement( XML_scenario,
            XML_name,       XclXmlUtils::ToOString( sName ).getStr(),
            XML_locked,     XclXmlUtils::ToPsz( nProtected ),
            // OOXTODO: XML_hidden,
            XML_count,      OString::valueOf( (sal_Int32) List::Count() ).getStr(),
            XML_user,       XESTRING_TO_PSZ( sUserName ),
            XML_comment,    XESTRING_TO_PSZ( sComment ),
            FSEND );

    for( ExcEScenarioCell* pCell = _First(); pCell; pCell = _Next() )
        pCell->SaveXml( rStrm );

    rWorkbook->endElement( XML_scenario );
}




ExcEScenarioManager::ExcEScenarioManager( const XclExpRoot& rRoot, SCTAB nTab ) :
        nActive( 0 )
{
    ScDocument& rDoc = rRoot.GetDoc();
    if( rDoc.IsScenario( nTab ) )
        return;

    SCTAB nFirstTab = nTab + 1;
    SCTAB nNewTab       = nFirstTab;

    while( rDoc.IsScenario( nNewTab ) )
    {
        Append( new ExcEScenario( rRoot, nNewTab ) );

        if( rDoc.IsActiveScenario( nNewTab ) )
            nActive = static_cast<sal_uInt16>(nNewTab - nFirstTab);
        nNewTab++;
    }
}

ExcEScenarioManager::~ExcEScenarioManager()
{
    for( ExcEScenario* pScen = _First(); pScen; pScen = _Next() )
        delete pScen;
}

void ExcEScenarioManager::SaveCont( XclExpStream& rStrm )
{
    rStrm   << (UINT16) List::Count()       // number of scenarios
            << nActive                      // active scen
            << nActive                      // last displayed
            << (UINT16) 0;                  // reference areas
}

void ExcEScenarioManager::Save( XclExpStream& rStrm )
{
    if( List::Count() )
        ExcRecord::Save( rStrm );

    for( ExcEScenario* pScen = _First(); pScen; pScen = _Next() )
        pScen->Save( rStrm );
}

void ExcEScenarioManager::SaveXml( XclExpXmlStream& rStrm )
{
    if( ! List::Count() )
        return;

    sax_fastparser::FSHelperPtr& rWorkbook = rStrm.GetCurrentStream();
    rWorkbook->startElement( XML_scenarios,
            XML_current,    OString::valueOf( (sal_Int32)nActive ).getStr(),
            XML_show,       OString::valueOf( (sal_Int32)nActive ).getStr(),
            // OOXTODO: XML_sqref,
            FSEND );

    for( ExcEScenario* pScen = _First(); pScen; pScen = _Next() )
        pScen->SaveXml( rStrm );

    rWorkbook->endElement( XML_scenarios );
}

UINT16 ExcEScenarioManager::GetNum() const
{
    return 0x00AE;
}

sal_Size ExcEScenarioManager::GetLen() const
{
    return 8;
}

// ============================================================================

struct XclExpTabProtectOption
{
    ScTableProtection::Option   eOption;
    sal_uInt16                  nMask;
};

XclExpSheetProtectOptions::XclExpSheetProtectOptions( const XclExpRoot& rRoot, SCTAB nTab ) :
    XclExpRecord( 0x0867, 23 )
{
    static const XclExpTabProtectOption aTable[] =
    {
        { ScTableProtection::OBJECTS,               0x0001 },
        { ScTableProtection::SCENARIOS,             0x0002 },
        { ScTableProtection::FORMAT_CELLS,          0x0004 },
        { ScTableProtection::FORMAT_COLUMNS,        0x0008 },
        { ScTableProtection::FORMAT_ROWS,           0x0010 },
        { ScTableProtection::INSERT_COLUMNS,        0x0020 },
        { ScTableProtection::INSERT_ROWS,           0x0040 },
        { ScTableProtection::INSERT_HYPERLINKS,     0x0080 },

        { ScTableProtection::DELETE_COLUMNS,        0x0100 },
        { ScTableProtection::DELETE_ROWS,           0x0200 },
        { ScTableProtection::SELECT_LOCKED_CELLS,   0x0400 },
        { ScTableProtection::SORT,                  0x0800 },
        { ScTableProtection::AUTOFILTER,            0x1000 },
        { ScTableProtection::PIVOT_TABLES,          0x2000 },
        { ScTableProtection::SELECT_UNLOCKED_CELLS, 0x4000 },

        { ScTableProtection::NONE,                  0x0000 }
    };

    mnOptions = 0x0000;
    ScTableProtection* pProtect = rRoot.GetDoc().GetTabProtection(nTab);
    if (!pProtect)
        return;

    for (int i = 0; aTable[i].nMask != 0x0000; ++i)
    {
        if ( pProtect->isOptionEnabled(aTable[i].eOption) )
            mnOptions |= aTable[i].nMask;
    }
}

void XclExpSheetProtectOptions::WriteBody( XclExpStream& rStrm )
{
    sal_uInt16 nBytes = 0x0867;
    rStrm << nBytes;

    sal_uChar nZero = 0x00;
    for (int i = 0; i < 9; ++i)
        rStrm << nZero;

    nBytes = 0x0200;
    rStrm << nBytes;
    nBytes = 0x0100;
    rStrm << nBytes;
    nBytes = 0xFFFF;
    rStrm << nBytes << nBytes;

    rStrm << mnOptions;
    nBytes = 0;
    rStrm << nBytes;
}

// ============================================================================




void XclCalccount::SaveCont( XclExpStream& rStrm )
{
    rStrm << nCount;
}


XclCalccount::XclCalccount( const ScDocument& rDoc )
{
    nCount = rDoc.GetDocOptions().GetIterCount();
}


UINT16 XclCalccount::GetNum() const
{
    return 0x000C;
}


sal_Size XclCalccount::GetLen() const
{
    return 2;
}


void XclCalccount::SaveXml( XclExpXmlStream& rStrm )
{
    rStrm.WriteAttributes(
            XML_iterateCount, OString::valueOf( (sal_Int32)nCount ).getStr(),
            FSEND );
}




void XclIteration::SaveCont( XclExpStream& rStrm )
{
    rStrm << nIter;
}


XclIteration::XclIteration( const ScDocument& rDoc )
{
    nIter = rDoc.GetDocOptions().IsIter()? 1 : 0;
}


UINT16 XclIteration::GetNum() const
{
    return 0x0011;
}


sal_Size XclIteration::GetLen() const
{
    return 2;
}


void XclIteration::SaveXml( XclExpXmlStream& rStrm )
{
    rStrm.WriteAttributes(
            XML_iterate, XclXmlUtils::ToPsz( nIter == 1 ),
            FSEND );
}




void XclDelta::SaveCont( XclExpStream& rStrm )
{
    rStrm << fDelta;
}



XclDelta::XclDelta( const ScDocument& rDoc )
{
    fDelta = rDoc.GetDocOptions().GetIterEps();
}


UINT16 XclDelta::GetNum() const
{
    return 0x0010;
}


sal_Size XclDelta::GetLen() const
{
    return 8;
}


void XclDelta::SaveXml( XclExpXmlStream& rStrm )
{
    rStrm.WriteAttributes(
            XML_iterateDelta, OString::valueOf( fDelta ).getStr(),
            FSEND );
}

// ============================================================================

XclExpFilePass::XclExpFilePass( const XclExpRoot& rRoot ) :
    XclExpRecord(0x002F, 54),
    mrRoot(rRoot)
{
}

XclExpFilePass::~XclExpFilePass()
{
}

void XclExpFilePass::WriteBody( XclExpStream& rStrm )
{
    static const sal_uInt8 nDocId[] = {
        0x17, 0xf7, 0x01, 0x08, 0xea, 0xad, 0x30, 0x5c,
        0x1a, 0x95, 0xa5, 0x75, 0xd6, 0x79, 0xcd, 0x8d };


    static const sal_uInt8 nSalt[] = {
        0xa4, 0x5b, 0xf7, 0xe9, 0x9f, 0x55, 0x21, 0xc5,
        0xc5, 0x56, 0xa8, 0x0d, 0x39, 0x05, 0x3a, 0xb4 };

    // 0x0000 - neither standard nor strong encryption
    // 0x0001 - standard or strong encryption
    rStrm << static_cast<sal_uInt16>(0x0001);

    // 0x0000 - non standard encryption
    // 0x0001 - standard encryption
    sal_uInt16 nStdEnc = 0x0001;
    rStrm << nStdEnc << nStdEnc;

    sal_uInt8 nSaltHash[16];
    XclExpEncrypterRef xEnc( new XclExpBiff8Encrypter(mrRoot, nDocId, nSalt) );
    xEnc->GetSaltDigest(nSaltHash);

    rStrm.Write(nDocId, 16);
    rStrm.Write(nSalt, 16);
    rStrm.Write(nSaltHash, 16);

    rStrm.SetEncrypter(xEnc);
}

// ============================================================================

XclExpInterfaceHdr::XclExpInterfaceHdr( sal_uInt16 nCodePage ) :
    XclExpUInt16Record( EXC_ID_INTERFACEHDR, nCodePage )
{
}

void XclExpInterfaceHdr::WriteBody( XclExpStream& rStrm )
{
    rStrm.DisableEncryption();
    rStrm << GetValue();
}

// ============================================================================

XclExpInterfaceEnd::XclExpInterfaceEnd() :
    XclExpRecord(0x00E2, 0) {}

XclExpInterfaceEnd::~XclExpInterfaceEnd() {}

void XclExpInterfaceEnd::WriteBody( XclExpStream& rStrm )
{
    // Don't forget to re-enable encryption.
    rStrm.EnableEncryption();
}

// ============================================================================

XclExpWriteAccess::XclExpWriteAccess() :
    XclExpRecord(0x005C, 112)
{
}

XclExpWriteAccess::~XclExpWriteAccess()
{
}

void XclExpWriteAccess::WriteBody( XclExpStream& rStrm )
{
    static const sal_uInt8 aData[] = {
        0x04, 0x00, 0x00,  'C',  'a',  'l',  'c', 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 };

    sal_Size nDataSize = sizeof(aData);
    for (sal_Size i = 0; i < nDataSize; ++i)
        rStrm << aData[i];
}

// ============================================================================

XclExpFileSharing::XclExpFileSharing( const XclExpRoot& rRoot, sal_uInt16 nPasswordHash, bool bRecommendReadOnly ) :
    XclExpRecord( EXC_ID_FILESHARING ),
    mnPasswordHash( nPasswordHash ),
    mbRecommendReadOnly( bRecommendReadOnly )
{
    if( rRoot.GetBiff() <= EXC_BIFF5 )
        maUserName.AssignByte( rRoot.GetUserName(), rRoot.GetTextEncoding(), EXC_STR_8BITLENGTH );
    else
        maUserName.Assign( rRoot.GetUserName() );
}

void XclExpFileSharing::Save( XclExpStream& rStrm )
{
    if( (mnPasswordHash != 0) || mbRecommendReadOnly )
        XclExpRecord::Save( rStrm );
}

void XclExpFileSharing::WriteBody( XclExpStream& rStrm )
{
    rStrm << sal_uInt16( mbRecommendReadOnly ? 1 : 0 ) << mnPasswordHash << maUserName;
}

// ============================================================================

XclExpProt4Rev::XclExpProt4Rev() :
    XclExpRecord(0x01AF, 2)
{
}

XclExpProt4Rev::~XclExpProt4Rev()
{
}

void XclExpProt4Rev::WriteBody( XclExpStream& rStrm )
{
    rStrm << static_cast<sal_uInt16>(0x0000);
}

// ============================================================================

XclExpProt4RevPass::XclExpProt4RevPass() :
    XclExpRecord(0x01BC, 2)
{
}

XclExpProt4RevPass::~XclExpProt4RevPass()
{
}

void XclExpProt4RevPass::WriteBody( XclExpStream& rStrm )
{
    rStrm << static_cast<sal_uInt16>(0x0000);
}

// ============================================================================

static const sal_uInt8 nDataRecalcId[] = {
    0xC1, 0x01, 0x00, 0x00, 0x54, 0x8D, 0x01, 0x00
};

XclExpRecalcId::XclExpRecalcId() :
    XclExpDummyRecord(0x01C1, nDataRecalcId, sizeof(nDataRecalcId))
{
}

// ============================================================================

static const sal_uInt8 nDataBookExt[] = {
    0x63, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02
};

XclExpBookExt::XclExpBookExt() :
    XclExpDummyRecord(0x0863, nDataBookExt, sizeof(nDataBookExt))
{
}

// ============================================================================

XclRefmode::XclRefmode( const ScDocument& rDoc ) :
    XclExpBoolRecord( 0x000F, rDoc.GetAddressConvention() != formula::FormulaGrammar::CONV_XL_R1C1 )
{
}

void XclRefmode::SaveXml( XclExpXmlStream& rStrm )
{
    rStrm.WriteAttributes(
            XML_refMode, GetBool() ? "A1" : "R1C1",
            FSEND );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
