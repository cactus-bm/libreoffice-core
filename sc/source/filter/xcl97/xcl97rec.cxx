/*************************************************************************
 *
 *  $RCSfile: xcl97rec.cxx,v $
 *
 *  $Revision: 1.76 $
 *
 *  last change: $Author: vg $ $Date: 2005-02-21 13:50:03 $
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

#ifdef PCH
#include "filt_pch.hxx"
#endif

#pragma hdrstop

#ifndef _SVDPOOL_HXX //autogen wg. SdrItemPool
#include <svx/svdpool.hxx>
#endif
#ifndef _SDTAITM_HXX //autogen wg. SdrTextHorzAdjustItem, SdrTextVertAdjustItem
#include <svx/sdtaitm.hxx>
#endif
#ifndef _SVDOTEXT_HXX //autogen wg. SdrTextObj
#include <svx/svdotext.hxx>
#endif
#ifndef _EDITOBJ_HXX //autogen wg. EditTextObject
#include <svx/editobj.hxx>
#endif
#ifndef _SVDOOLE2_HXX //autogen wg. SdrOle2Obj
#include <svx/svdoole2.hxx>
#endif
#include <sot/storage.hxx>
#ifndef _SFXITEMSET_HXX //autogen wg. SfxItemSet
#include <svtools/itemset.hxx>
#endif
#ifndef _SVDPAGE_HXX
#include <svx/svdpage.hxx>
#endif
#ifndef _SVDCAPT_HXX
#include <svx/svdocapt.hxx>
#endif
#ifndef _SVX_UNOAPI_HXX_
#include <svx/unoapi.hxx>
#endif
#ifndef _SVX_WRITINGMODEITEM_HXX
#include <svx/writingmodeitem.hxx>
#endif

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif
#ifndef _ZFORMAT_HXX            // SvNumberformat
#include <svtools/zformat.hxx>
#endif
#ifndef SC_CELL_HXX             // ScFormulaCell
#include "cell.hxx"
#endif
#ifndef SC_DRWLAYER_HXX
#include "drwlayer.hxx"
#endif

#include "xcl97rec.hxx"
#include "xcl97esc.hxx"

#ifndef SC_EDITUTIL_HXX
#include "editutil.hxx"
#endif
#ifndef SC_XECONTENT_HXX
#include "xecontent.hxx"
#endif
#ifndef SC_XESTYLE_HXX
#include "xestyle.hxx"
#endif
#ifndef SC_XELINK_HXX
#include "xelink.hxx"
#endif

#include "scitems.hxx"

#include <svtools/fltrcfg.hxx>
#include <svx/brshitem.hxx>
#include <svx/boxitem.hxx>
#ifndef _SVX_FRMDIRITEM_HXX
#include <svx/frmdiritem.hxx>
#endif
#ifndef _SVX_ADJITEM_HX
#include <svx/adjitem.hxx>
#endif

#include <svx/eeitem.hxx>
#define ITEMID_FIELD EE_FEATURE_FIELD
#include <svx/msoleexp.hxx>

#include <svtools/useroptions.hxx>
#ifndef _UNOTOOLS_LOCALEDATAWRAPPER_HXX
#include <unotools/localedatawrapper.hxx>
#endif

#include <stdio.h>

#include "document.hxx"
#include "conditio.hxx"
#include "rangelst.hxx"
#include "stlpool.hxx"
#include "viewopti.hxx"
#include "scextopt.hxx"
#include "docoptio.hxx"
#include "patattr.hxx"

using ::rtl::OUString;
using namespace ::com::sun::star;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::UNO_QUERY;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::drawing::XShape;


//___________________________________________________________________

// --- class XclMsodrawing_Base --------------------------------------

XclMsodrawing_Base::XclMsodrawing_Base( XclEscher& rEscher, ULONG nInitialSize )
        :
        pEscher( &rEscher ),
        nStartPos( rEscher.GetEx()->GetLastOffsetMapPos() )
{
    // for safety's sake add this now
    nStopPos = GetEscherEx()->AddCurrentOffsetToMap();
    DBG_ASSERT( GetDataLen() == nInitialSize, "XclMsodrawing_Base ctor: do I really own that data?" );
}


XclMsodrawing_Base::~XclMsodrawing_Base()
{
}


void XclMsodrawing_Base::UpdateStopPos()
{
    if ( nStopPos )
        GetEscherEx()->ReplaceCurrentOffsetInMap( nStopPos );
    else
        nStopPos = GetEscherEx()->AddCurrentOffsetToMap();
}


ULONG XclMsodrawing_Base::GetDataLen() const
{
    if ( nStartPos < nStopPos )
    {
        XclEscherEx* pEx = GetEscherEx();
        return pEx->GetOffsetFromMap( nStopPos ) - pEx->GetOffsetFromMap( nStartPos );
    }
    DBG_ERRORFILE( "XclMsodrawing_Base::GetDataLen: position mismatch" );
    return 0;
}



// --- class XclMsodrawinggroup --------------------------------------

XclMsodrawinggroup::XclMsodrawinggroup( RootData& rRoot, UINT16 nEscherType )
        :
        XclMsodrawing_Base( *rRoot.pEscher )
{
    if ( nEscherType )
    {
        XclEscherEx* pEx = GetEscherEx();
        SvStream& rOut = pEx->GetStream();
        switch ( nEscherType )
        {
            case ESCHER_DggContainer :
            {   // per-document data
                pEx->OpenContainer( nEscherType );
//2do: stuff it with our own document defaults?
#if 0
                pEx->BeginCount();
                pEx->AddOpt( ... );
                pEx->EndCount( ESCHER_OPT, 3 );
#else
                BYTE pDummyOPT[] = {
                    0xBF, 0x00, 0x08, 0x00, 0x08, 0x00, 0x81, 0x01,
                    0x09, 0x00, 0x00, 0x08, 0xC0, 0x01, 0x40, 0x00,
                    0x00, 0x08
                };
                pEx->AddAtom( sizeof(pDummyOPT), ESCHER_OPT, 3, 3 );
                rOut.Write( pDummyOPT, sizeof(pDummyOPT) );
#endif
                BYTE pDummySplitMenuColors[] = {
                    0x0D, 0x00, 0x00, 0x08, 0x0C, 0x00, 0x00, 0x08,
                    0x17, 0x00, 0x00, 0x08, 0xF7, 0x00, 0x00, 0x10
                };
                pEx->AddAtom( sizeof(pDummySplitMenuColors), ESCHER_SplitMenuColors, 0, 4 );
                rOut.Write( pDummySplitMenuColors, sizeof(pDummySplitMenuColors) );
                pEx->CloseContainer();  // ESCHER_DggContainer
            }
            break;
        }
        UpdateStopPos();
    }
}


XclMsodrawinggroup::~XclMsodrawinggroup()
{
}


void XclMsodrawinggroup::SaveCont( XclExpStream& rStrm )
{
    DBG_ASSERT( GetEscherEx()->GetStreamPos() == GetEscherEx()->GetOffsetFromMap( nStartPos ),
        "XclMsodrawinggroup::SaveCont: Escher stream position mismatch" );
    rStrm.CopyFromStream( pEscher->GetStrm(), GetDataLen() );
}


UINT16 XclMsodrawinggroup::GetNum() const
{
    return 0x00EB;
}


ULONG XclMsodrawinggroup::GetLen() const
{
    return GetDataLen();
}



// --- class XclMsodrawing --------------------------------------

XclMsodrawing::XclMsodrawing( const XclExpRoot& rRoot, UINT16 nEscherType, UINT32 nInitialSize ) :
    XclMsodrawing_Base( *rRoot.GetOldRoot().pEscher, nInitialSize )
{
    if ( nEscherType )
    {
        XclEscherEx* pEx = GetEscherEx();
        switch ( nEscherType )
        {
            case ESCHER_DgContainer :
            {   // per-sheet data
                pEx->OpenContainer( nEscherType );
                // open group shape container
                Rectangle aRect( 0, 0, 0, 0 );
                pEx->EnterGroup( &aRect );
            }
            break;
        }
        UpdateStopPos();
    }
}


XclMsodrawing::~XclMsodrawing()
{
}


void XclMsodrawing::SaveCont( XclExpStream& rStrm )
{
    DBG_ASSERT( GetEscherEx()->GetStreamPos() == GetEscherEx()->GetOffsetFromMap( nStartPos ),
        "XclMsodrawing::SaveCont: Escher stream position mismatch" );
    rStrm.CopyFromStream( pEscher->GetStrm(), GetDataLen() );
}


UINT16 XclMsodrawing::GetNum() const
{
    return 0x00EC;
}


ULONG XclMsodrawing::GetLen() const
{
    return GetDataLen();
}


// --- class XclObjList ----------------------------------------------

XclObjList::XclObjList( const XclExpRoot& rRoot ) :
    XclExpRoot( rRoot ),
    pMsodrawingPerSheet( new XclMsodrawing( rRoot, ESCHER_DgContainer ) ),
    pSolverContainer( NULL )
{
}


XclObjList::~XclObjList()
{
    for ( XclObj* p = First(); p; p = Next() )
        delete p;
    delete pMsodrawingPerSheet;
    delete pSolverContainer;
}


UINT16 XclObjList::Add( XclObj* pObj )
{
    DBG_ASSERT( Count() < 0xFFFF, "XclObjList::Add: too much for Xcl" );
    if ( Count() < 0xFFFF )
    {
        Insert( pObj, LIST_APPEND );
        UINT16 nCnt = (UINT16) Count();
        pObj->SetId( nCnt );
        return nCnt;
    }
    else
    {
        delete pObj;
        return 0;
    }
}


void XclObjList::EndSheet()
{
    XclEscherEx* pEx = pMsodrawingPerSheet->GetEscherEx();

    // Is there still something in the stream? -> The solver container
    UINT32 nSolverSize = pEx->GetStreamPos() - pEx->GetOffsetFromMap( pEx->GetLastOffsetMapPos() );
    if( nSolverSize )
        pSolverContainer = new XclMsodrawing( GetRoot(), ESCHER_SolverContainer, nSolverSize );

    //! close ESCHER_DgContainer created by XclObjList ctor MSODRAWING
    pEx->CloseContainer();
}


void XclObjList::Save( XclExpStream& rStrm )
{
    //! Escher must be written, even if there are no objects
    pMsodrawingPerSheet->Save( rStrm );

    for ( XclObj* p = First(); p; p = Next() )
        p->Save( rStrm );

    if( pSolverContainer )
        pSolverContainer->Save( rStrm );
}



// --- class XclObj --------------------------------------------------

XclObj::XclObj( const XclExpRoot& rRoot, sal_uInt16 nObjType, bool bOwnEscher ) :
    XclExpRecord( EXC_ID_OBJ, 26 ),
    pClientTextbox( NULL ),
    pTxo( NULL ),
    mnObjType( nObjType ),
    nObjId(0),
    nGrbit( 0x6011 ),   // AutoLine, AutoFill, Printable, Locked
    bFirstOnSheet( rRoot.GetOldRoot().pObjRecs->Count() == 0 ),
    mbOwnEscher( bOwnEscher )
{
    //! first object continues the first MSODRAWING record
    if ( bFirstOnSheet )
        pMsodrawing = rRoot.GetOldRoot().pObjRecs->GetMsodrawingPerSheet();
    else
        pMsodrawing = new XclMsodrawing( rRoot );
}


XclObj::~XclObj()
{
    if ( !bFirstOnSheet )
        delete pMsodrawing;
    delete pClientTextbox;
    delete pTxo;
}


void XclObj::SetEscherShapeType( UINT16 nType )
{
//2do: what about the other defined ot... types?
    switch ( nType )
    {
        case ESCHER_ShpInst_Line :
            mnObjType = EXC_OBJ_CMO_LINE;
        break;
        case ESCHER_ShpInst_Rectangle :
        case ESCHER_ShpInst_RoundRectangle :
            mnObjType = EXC_OBJ_CMO_RECTANGLE;
        break;
        case ESCHER_ShpInst_Ellipse :
            mnObjType = EXC_OBJ_CMO_ELLIPSE;
        break;
        case ESCHER_ShpInst_Arc :
            mnObjType = EXC_OBJ_CMO_ARC;
        break;
        case ESCHER_ShpInst_TextBox :
            mnObjType = EXC_OBJ_CMO_TEXT;
        break;
        case ESCHER_ShpInst_PictureFrame :
            mnObjType = EXC_OBJ_CMO_PICTURE;
        break;
        default:
            mnObjType = EXC_OBJ_CMO_DRAWING;
    }
}


void XclObj::SetText( const XclExpRoot& rRoot, const SdrTextObj& rObj )
{
    DBG_ASSERT( !pClientTextbox, "XclObj::SetText: already set" );
    if ( !pClientTextbox )
    {
        pMsodrawing->UpdateStopPos();
        pClientTextbox = new XclMsodrawing( rRoot );
        pClientTextbox->GetEscherEx()->AddAtom( 0, ESCHER_ClientTextbox );  // TXO record
        pClientTextbox->UpdateStopPos();
        pTxo = new XclTxo( rRoot, rObj );
    }
}


void XclObj::WriteBody( XclExpStream& rStrm )
{
    DBG_ASSERT( mnObjType != EXC_OBJ_CMO_UNKNOWN, "XclObj::WriteBody - unknown type" );

    // create a substream to be able to create subrecords
    SvMemoryStream aMemStrm;
    ::std::auto_ptr< XclExpStream > pXclStrm( new XclExpStream( aMemStrm, rStrm.GetRoot() ) );

    // write the ftCmo subrecord
    pXclStrm->StartRecord( EXC_ID_OBJ_FTCMO, 18 );
    *pXclStrm << mnObjType << nObjId << nGrbit;
    pXclStrm->WriteZeroBytes( 12 );
    pXclStrm->EndRecord();

    // write other subrecords
    WriteSubRecs( *pXclStrm );

    // write the ftEnd subrecord
    pXclStrm->StartRecord( EXC_ID_OBJ_FTEND, 0 );
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


void XclObj::WriteSubRecs( XclExpStream& rStrm )
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


// --- class XclObjComment -------------------------------------------


XclObjComment::XclObjComment( const XclExpRoot& rRoot, const Rectangle& rRect, const EditTextObject& rEditObj, SdrObject* pCaption, bool bVisible )
            :
            XclObj( rRoot, EXC_OBJ_CMO_NOTE, true )
{
    ProcessEscherObj(rRoot, rRect, pCaption, bVisible);
    // TXO
    pTxo = new XclTxo( rRoot, rEditObj, pCaption );
}

void XclObjComment::ProcessEscherObj( const XclExpRoot& rRoot, const Rectangle& rRect, SdrObject* pCaption, const bool bVisible )
{
    Reference<XShape> aXShape;
    EscherPropertyContainer aPropOpt;

    if(pCaption)
    {
        aXShape = GetXShapeForSdrObject(pCaption);
        Reference< XPropertySet > aXPropSet( aXShape, UNO_QUERY );
        if( aXPropSet.is() )
        {
            aPropOpt.CreateFillProperties( aXPropSet,  sal_True);

            aPropOpt.AddOpt( ESCHER_Prop_lTxid, 0 );                        // undocumented
        aPropOpt.AddOpt( 0x0158, 0x00000000 );                          // undocumented

            sal_uInt32 nValue = 0;
            if(!aPropOpt.GetOpt( ESCHER_Prop_FitTextToShape, nValue ))
                aPropOpt.AddOpt( ESCHER_Prop_FitTextToShape, 0x00080008 );      // bool field

            if(aPropOpt.GetOpt( ESCHER_Prop_fillColor, nValue ))
            {
                // If the Colour is the same as the 'ToolTip' System colour then
                // use the default rather than the explicit colour value. This will
                // be incorrect where user has chosen to use this colour explicity.
                Color aColor = Color( (BYTE)nValue, (BYTE)( nValue >> 8 ), (BYTE)( nValue >> 16 ) );
                const StyleSettings& rSett = Application::GetSettings().GetStyleSettings();
                if(aColor == rSett.GetHelpColor().GetColor())
                {
                    aPropOpt.AddOpt( ESCHER_Prop_fillColor, 0x08000050 );
                    aPropOpt.AddOpt( ESCHER_Prop_fillBackColor, 0x08000050 );
                }
            }
            else
                aPropOpt.AddOpt( ESCHER_Prop_fillColor, 0x08000050 );

            if(!aPropOpt.GetOpt( ESCHER_Prop_fillBackColor, nValue ))
                aPropOpt.AddOpt( ESCHER_Prop_fillBackColor, 0x08000050 );
        if(!aPropOpt.GetOpt( ESCHER_Prop_fNoFillHitTest, nValue ))
                aPropOpt.AddOpt( ESCHER_Prop_fNoFillHitTest, 0x00110010 );      // bool field
        if(!aPropOpt.GetOpt( ESCHER_Prop_shadowColor, nValue ))
                aPropOpt.AddOpt( ESCHER_Prop_shadowColor, 0x00000000 );
        if(!aPropOpt.GetOpt( ESCHER_Prop_fshadowObscured, nValue ))     // bool field
                aPropOpt.AddOpt( ESCHER_Prop_fshadowObscured, 0x00030003 );     // bool field
        }
    }

    nGrbit = 0;     // all off: AutoLine, AutoFill, Printable, Locked
    XclEscherEx* pEx = pMsodrawing->GetEscherEx();
    pEx->OpenContainer( ESCHER_SpContainer );
    pEx->AddShape( ESCHER_ShpInst_TextBox, SHAPEFLAG_HAVEANCHOR | SHAPEFLAG_HAVESPT );
    sal_uInt32 nFlags = 0x000A0000;
    ::set_flag( nFlags, 2UL, !bVisible );
    aPropOpt.AddOpt( ESCHER_Prop_fPrint, nFlags );                  // bool field
    aPropOpt.Commit( pEx->GetStream() );

    XclExpEscherNoteAnchor( rRoot, rRect ).WriteData( *pEx);

    pEx->AddAtom( 0, ESCHER_ClientData );                       // OBJ record
    pMsodrawing->UpdateStopPos();
    //! Be sure to construct the MSODRAWING ClientTextbox record _after_ the
    //! base OBJ's MSODRAWING record Escher data is completed.
    pClientTextbox = new XclMsodrawing( rRoot );
    pClientTextbox->GetEscherEx()->AddAtom( 0, ESCHER_ClientTextbox );  // TXO record
    pClientTextbox->UpdateStopPos();
    pEx->CloseContainer();  // ESCHER_SpContainer
}


XclObjComment::~XclObjComment()
{
}


void XclObjComment::Save( XclExpStream& rStrm )
{
    // content of this record
    XclObj::Save( rStrm );
}


// --- class XclObjDropDown ------------------------------------------

XclObjDropDown::XclObjDropDown( const XclExpRoot& rRoot, const ScAddress& rPos, BOOL bFilt ) :
        XclObj( rRoot, EXC_OBJ_CMO_COMBOBOX, true ),
        bIsFiltered( bFilt )
{
    SetLocked( TRUE );
    SetPrintable( FALSE );
    SetAutoFill( TRUE );
    SetAutoLine( FALSE );
    nGrbit |= 0x0100;   // undocumented
    XclEscherEx* pEx = pMsodrawing->GetEscherEx();
    pEx->OpenContainer( ESCHER_SpContainer );
    pEx->AddShape( ESCHER_ShpInst_HostControl, SHAPEFLAG_HAVEANCHOR | SHAPEFLAG_HAVESPT );
    EscherPropertyContainer aPropOpt;
    aPropOpt.AddOpt( ESCHER_Prop_LockAgainstGrouping, 0x01040104 ); // bool field
    aPropOpt.AddOpt( ESCHER_Prop_FitTextToShape, 0x00080008 );      // bool field
    aPropOpt.AddOpt( ESCHER_Prop_fNoFillHitTest, 0x00010000 );      // bool field
    aPropOpt.AddOpt( ESCHER_Prop_fNoLineDrawDash, 0x00080000 );     // bool field
    aPropOpt.AddOpt( ESCHER_Prop_fPrint, 0x000A0000 );              // bool field
    aPropOpt.Commit( pEx->GetStream() );

    XclExpEscherDropDownAnchor( rRoot, rPos ).WriteData( *pEx );

    pEx->AddAtom( 0, ESCHER_ClientData );                       // OBJ record
    pMsodrawing->UpdateStopPos();
    pEx->CloseContainer();  // ESCHER_SpContainer

    // old size + ftSbs + ftLbsData
    AddRecSize( 24 + 20 );
}

XclObjDropDown::~XclObjDropDown()
{
}

void XclObjDropDown::WriteSubRecs( XclExpStream& rStrm )
{
    // ftSbs subrecord - Scroll bars (dummy)
    rStrm.StartRecord( EXC_ID_OBJ_FTSBS, 20 );
    rStrm.WriteZeroBytes( 20 );
    rStrm.EndRecord();

    // ftLbsData subrecord - Listbox data
    sal_uInt16 nComboStyle = EXC_OBJ_LBS_COMBO_SIMPLE;
    ::set_flag( nComboStyle, EXC_OBJ_LBS_FILTERED, bIsFiltered );
    rStrm.StartRecord( EXC_ID_OBJ_FTLBSDATA, 16 );
    rStrm   << (UINT32)0 << (UINT16)0 << (UINT16)0x0301 << (UINT16)0
            << nComboStyle << sal_uInt16( 20 ) << sal_uInt16( 130 );
    rStrm.EndRecord();
}


// --- class XclTxo --------------------------------------------------

XclTxoHorAlign lcl_GetHorAlignFromItemSet( const SfxItemSet& rItemSet )
{
    XclTxoHorAlign eHorAlign = xlTxoHAlign_Default;

    switch( static_cast< const SvxAdjustItem& >( rItemSet.Get( EE_PARA_JUST ) ).GetAdjust() )
    {
        case SVX_ADJUST_LEFT:           eHorAlign = xlTxoHAlignLeft;       break;
        case SVX_ADJUST_CENTER:         eHorAlign = xlTxoHAlignCenter;     break;
        case SVX_ADJUST_RIGHT:          eHorAlign = xlTxoHAlignRight;      break;
        case SVX_ADJUST_BLOCK:          eHorAlign = xlTxoHAlignJustify;    break;
    }
    return eHorAlign;
}

XclTxoVerAlign lcl_GetVerAlignFromItemSet( const SfxItemSet& rItemSet )
{
    XclTxoVerAlign eVerAlign = xlTxoVAlign_Default;

    switch( static_cast< const SdrTextVertAdjustItem& >( rItemSet.Get( SDRATTR_TEXT_VERTADJUST ) ).GetValue() )
    {
        case SDRTEXTVERTADJUST_TOP:     eVerAlign = xlTxoVAlignTop;     break;
        case SDRTEXTVERTADJUST_CENTER:  eVerAlign = xlTxoVAlignCenter;  break;
        case SDRTEXTVERTADJUST_BOTTOM:  eVerAlign = xlTxoVAlignBottom;  break;
        case SDRTEXTVERTADJUST_BLOCK:   eVerAlign = xlTxoVAlignJustify; break;
    }
    return eVerAlign;
}

XclTxo::XclTxo( const String& rString, sal_uInt16 nFontIx ) :
    mpString( new XclExpString( rString ) ),
    meHorAlign( xlTxoHAlign_Default ),
    meVerAlign( xlTxoVAlign_Default ),
    meRotation( xlTxoRot_Default )
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
    meHorAlign( xlTxoHAlign_Default ),
    meVerAlign( xlTxoVAlign_Default ),
    meRotation( xlTxoRot_Default )
{
    // additional alignment and orientation items
    const SfxItemSet& rItemSet = rTextObj.GetMergedItemSet();

    // horizontal alignment
    SetHorAlign(lcl_GetHorAlignFromItemSet(rItemSet));

    // vertical alignment
    SetVerAlign(lcl_GetVerAlignFromItemSet(rItemSet));

    // rotation
    long nAngle = rTextObj.GetRotateAngle();
    if( (4500 < nAngle) && (nAngle < 13500) )
        meRotation = xlTxoRot90ccw;
    else if( (22500 < nAngle) && (nAngle < 31500) )
        meRotation = xlTxoRot90cw;
    else
        meRotation = xlTxoNoRot;
}

XclTxo::XclTxo( const XclExpRoot& rRoot, const EditTextObject& rEditObj, SdrObject* pCaption ) :
    mpString( XclExpStringHelper::CreateString( rRoot, rEditObj ) ),
    meHorAlign( xlTxoHAlign_Default ),
    meVerAlign( xlTxoVAlign_Default ),
    meRotation( xlTxoRot_Default )
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
            if (aSet.GetItemState(EE_PARA_JUST,TRUE,&pItem) == SFX_ITEM_SET)
            {
                SvxAdjust eEEAlign = static_cast< const SvxAdjustItem& >( *pItem).GetAdjust();
                pCaption->SetMergedItem( SvxAdjustItem( eEEAlign, EE_PARA_JUST));
            }
        }
        const SfxItemSet& rItemSet = pCaption->GetMergedItemSet();

        // horizontal alignment
        SetHorAlign(lcl_GetHorAlignFromItemSet(rItemSet));

        // vertical alignment
        SetVerAlign(lcl_GetVerAlignFromItemSet(rItemSet));

        // orientation alignment
        const SvxWritingModeItem& rItem = static_cast<const SvxWritingModeItem&>
            (rItemSet.Get (SDRATTR_TEXTDIRECTION));
        if (rItem.GetValue() == com::sun::star::text::WritingMode_TB_RL)
            meRotation = xlTxoRot90cw;
    }
}

void XclTxo::SaveCont( XclExpStream& rStrm )
{
    DBG_ASSERT( mpString.get(), "XclTxo::SaveCont - missing string" );

    sal_uInt16 nRunLen = 8 * mpString->GetFormatsCount();
    // alignment
    sal_uInt16 nFlags = 0;
    ::insert_value( nFlags, meHorAlign, 1, 3 );
    ::insert_value( nFlags, meVerAlign, 4, 3 );

    rStrm << nFlags << static_cast< sal_uInt16 >( meRotation );
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
            rStrm << aIt->mnChar << aIt->mnXclFont << sal_uInt32( 0 );
        rStrm.EndRecord();
    }
}

UINT16 XclTxo::GetNum() const
{
    return EXC_ID_TXO;
}

ULONG XclTxo::GetLen() const
{
    return 18;
}


// --- class XclObjOle -------------------------------------------

XclObjOle::XclObjOle( const XclExpRoot& rRoot, const SdrObject& rObj ) :
    XclObj( rRoot, EXC_OBJ_CMO_PICTURE ),
    rOleObj( rObj ),
    pRootStorage( rRoot.GetRootStorage() )
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
    UINT32          nPictureId = UINT32(this);
    sprintf( aBuf, "%08X", nPictureId );        // #100211# - checked
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

            // ftCf subrecord, undocumented as usual
            rStrm.StartRecord( EXC_ID_OBJ_FTCF, 2 );
            rStrm << UINT16(0x0002);
            rStrm.EndRecord();

            // ftPioGrbit subrecord, undocumented as usual
            rStrm.StartRecord( EXC_ID_OBJ_FTPIOGRBIT, 2 );
            rStrm << UINT16(0x0001);
            rStrm.EndRecord();

            // ftPictFmla subrecord, undocumented as usual
            XclExpString aName( xOleStg->GetUserName() );
            UINT16 nPadLen = (UINT16)(aName.GetSize() & 0x01);
            UINT16 nFmlaLen = static_cast< sal_uInt16 >( 12 + aName.GetSize() + nPadLen );
            UINT16 nSubRecLen = nFmlaLen + 6;

            rStrm.StartRecord( EXC_ID_OBJ_FTPICTFMLA, nSubRecLen );
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

XclObjAny::XclObjAny( const XclExpRoot& rRoot ) :
    XclObj( rRoot, EXC_OBJ_CMO_UNKNOWN )
{
}

XclObjAny::~XclObjAny()
{
}

void XclObjAny::WriteSubRecs( XclExpStream& rStrm )
{
    if( mnObjType == EXC_OBJ_CMO_GROUP )
        // ftGmo subrecord
        rStrm << EXC_ID_OBJ_FTGMO << UINT16(2) << UINT16(0);
}

void XclObjAny::Save( XclExpStream& rStrm )
{
    if( mnObjType == EXC_OBJ_CMO_GROUP )
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


void ExcBof8_Base::SaveCont( XclExpStream& rStrm )
{
    rStrm   << nVers << nDocType << nRupBuild << nRupYear
            << nFileHistory << nLowestBiffVer;
}


UINT16 ExcBof8_Base::GetNum() const
{
    return 0x0809;
}


ULONG ExcBof8_Base::GetLen() const
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


// --- class ExcBofC8 ------------------------------------------------

ExcBofC8::ExcBofC8()
{
    nDocType = 0x0020;
}

// --- class ExcBundlesheet8 -----------------------------------------

ExcBundlesheet8::ExcBundlesheet8( RootData& rRootData, SCTAB nTab ) :
    ExcBundlesheetBase( rRootData, static_cast<sal_uInt16>(nTab) )
{
    aUnicodeName.Assign( rRootData.pER->GetTabInfo().GetScTabName( nTab ), EXC_STR_8BITLENGTH );
}


ExcBundlesheet8::ExcBundlesheet8( const String& rString ) :
    ExcBundlesheetBase(),
    aUnicodeName( rString, EXC_STR_8BITLENGTH )
{
}


void ExcBundlesheet8::SaveCont( XclExpStream& rStrm )
{
    nOwnPos = rStrm.GetStreamPos();
    // write dummy position, real position comes later
    rStrm << sal_uInt32( 0 ) << nGrbit << aUnicodeName;
}


ULONG ExcBundlesheet8::GetLen() const
{   // Text max 255 chars
    return 8 + aUnicodeName.GetBufferSize();
}



// --- class XclObproj -----------------------------------------------

UINT16 XclObproj::GetNum() const
{
    return 0x00D3;
}


ULONG XclObproj::GetLen() const
{
    return 0;
}



// --- class XclDConRef ----------------------------------------------

XclDConRef::XclDConRef( const ScRange& rSrcR, const String& rWB ) :
        aSourceRange( rSrcR )
{
    String  sTemp( ( sal_Unicode ) 0x02 );

    sTemp += rWB;
    pWorkbook = new XclExpString( sTemp );
}

XclDConRef::~XclDConRef()
{
    delete pWorkbook;
}

void XclDConRef::SaveCont( XclExpStream& rStrm )
{
    rStrm   << (UINT16) aSourceRange.aStart.Row()
            << (UINT16) aSourceRange.aEnd.Row()
            << (UINT8)  aSourceRange.aStart.Col()
            << (UINT8)  aSourceRange.aEnd.Col()
            << *pWorkbook
            << (UINT8)  0x00;
}

UINT16 XclDConRef::GetNum() const
{
    return 0x0051;
}

ULONG XclDConRef::GetLen() const
{
    return 7 + pWorkbook->GetSize();
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


ULONG XclCodename::GetLen() const
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




XclExpString ExcEScenario::sUsername;

ExcEScenario::ExcEScenario( ScDocument& rDoc, SCTAB nTab )
{
    String  sTmpName;
    String  sTmpComm;
    Color   aDummyCol;
    USHORT  nFlags;

    rDoc.GetName( nTab, sTmpName );
    sName.Assign( sTmpName, EXC_STR_8BITLENGTH );
    nRecLen = 8 + sName.GetBufferSize();

    rDoc.GetScenarioData( nTab, sTmpComm, aDummyCol, nFlags );
    sComment.Assign( sTmpComm, EXC_STR_DEFAULT, 255 );
    if( sComment.Len() )
        nRecLen += sComment.GetSize();
    nProtected = (nFlags & SC_SCENARIO_PROTECT) ? 1 : 0;

    if( !sUsername.Len() )
    {
        SvtUserOptions aUserOpt;
        sUsername.Assign( aUserOpt.GetLastName(), EXC_STR_DEFAULT, 255 );
    }
    if( !sUsername.Len() )
        sUsername.Assign( String::CreateFromAscii( "SC" ) );
    nRecLen += sUsername.GetSize();

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
    nRecLen += 6 + pCell->GetStringBytes();     // 4 bytes address, 2 bytes ifmt
    return TRUE;
}

void ExcEScenario::SaveCont( XclExpStream& rStrm )
{
    rStrm   << (UINT16) List::Count()       // number of cells
            << nProtected                   // fProtection
            << (UINT8) 0                    // fHidden
            << (UINT8) sName.Len()          // length of scen name
            << (UINT8) sComment.Len()       // length of comment
            << (UINT8) sUsername.Len();     // length of user name
    sName.WriteFlagField( rStrm );
    sName.WriteBuffer( rStrm );

    rStrm << sUsername;

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

ULONG ExcEScenario::GetLen() const
{
    return nRecLen;
}




ExcEScenarioManager::ExcEScenarioManager( ScDocument& rDoc, SCTAB nTab ) :
        nActive( 0 )
{
    if( rDoc.IsScenario( nTab ) )
        return;

    SCTAB nFirstTab = nTab + 1;
    SCTAB nNewTab       = nFirstTab;

    while( rDoc.IsScenario( nNewTab ) )
    {
        Append( new ExcEScenario( rDoc, nNewTab ) );

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

UINT16 ExcEScenarioManager::GetNum() const
{
    return 0x00AE;
}

ULONG ExcEScenarioManager::GetLen() const
{
    return 8;
}



// ---- class XclProtection ------------------------------------------

const BYTE      XclProtection::pMyData[] =
{
    0x12, 0x00, 0x02, 0x00, 0x01, 0x00,         // PROTECT
    0xDD, 0x00, 0x02, 0x00, 0x01, 0x00,         // SCENPROTECT
    0x63, 0x00, 0x02, 0x00, 0x01, 0x00          // OBJPROTECT
};
const ULONG XclProtection::nMyLen = sizeof( XclProtection::pMyData );

ULONG XclProtection::GetLen( void ) const
{
    return nMyLen;
}


const BYTE* XclProtection::GetData( void ) const
{
    return pMyData;
}









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


ULONG XclCalccount::GetLen() const
{
    return 2;
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


ULONG XclIteration::GetLen() const
{
    return 2;
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


ULONG XclDelta::GetLen() const
{
    return 8;
}



