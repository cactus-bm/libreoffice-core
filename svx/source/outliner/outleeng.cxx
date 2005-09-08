/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: outleeng.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 23:57:03 $
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
#include <outl_pch.hxx>

#pragma hdrstop

#define _OUTLINER_CXX
#include <outliner.hxx>
#include <outleeng.hxx>
#include <paralist.hxx>
#include <outliner.hrc>

#ifndef _SFXITEMSET_HXX //autogen
#include <svtools/itemset.hxx>
#endif

#ifndef _EEITEM_HXX //autogen
#include <eeitem.hxx>
#endif

OutlinerEditEng::OutlinerEditEng( Outliner* pEngOwner, SfxItemPool* pPool )
 : EditEngine( pPool )
{
    pOwner = pEngOwner;
}

OutlinerEditEng::~OutlinerEditEng()
{
}

void OutlinerEditEng::PaintingFirstLine( USHORT nPara, const Point& rStartPos, long nBaseLineY, const Point& rOrigin, short nOrientation, OutputDevice* pOutDev )
{
    pOwner->PaintBullet( nPara, rStartPos, rOrigin, nOrientation, pOutDev );
}

Rectangle OutlinerEditEng::GetBulletArea( USHORT nPara )
{
    Rectangle aBulletArea = Rectangle( Point(), Point() );
    if ( nPara < pOwner->pParaList->GetParagraphCount() )
    {
        if ( pOwner->ImplHasBullet( nPara ) )
            aBulletArea = pOwner->ImpCalcBulletArea( nPara, FALSE, FALSE );
    }
    return aBulletArea;
}

void OutlinerEditEng::ParagraphInserted( USHORT nNewParagraph )
{
    pOwner->ParagraphInserted( nNewParagraph );

    EditEngine::ParagraphInserted( nNewParagraph );
}

void OutlinerEditEng::ParagraphDeleted( USHORT nDeletedParagraph )
{
    pOwner->ParagraphDeleted( nDeletedParagraph );

    EditEngine::ParagraphDeleted( nDeletedParagraph );
}

void OutlinerEditEng::StyleSheetChanged( SfxStyleSheet* pStyle )
{
    pOwner->StyleSheetChanged( pStyle );
}

void OutlinerEditEng::ParaAttribsChanged( USHORT nPara )
{
    pOwner->ParaAttribsChanged( nPara );
}

void OutlinerEditEng::ParagraphHeightChanged( USHORT nPara )
{
    pOwner->ParagraphHeightChanged( nPara );

    EditEngine::ParagraphHeightChanged( nPara );
}

BOOL OutlinerEditEng::SpellNextDocument()
{
    return pOwner->SpellNextDocument();
}

BOOL OutlinerEditEng::ConvertNextDocument()
{
    return pOwner->ConvertNextDocument();
}

XubString OutlinerEditEng::GetUndoComment( USHORT nUndoId ) const
{
#ifndef SVX_LIGHT
    switch( nUndoId )
    {
        case OLUNDO_DEPTH:
            return XubString( EditResId( RID_OUTLUNDO_DEPTH ));

        case OLUNDO_EXPAND:
            return XubString( EditResId( RID_OUTLUNDO_EXPAND ));

        case OLUNDO_COLLAPSE:
            return XubString( EditResId( RID_OUTLUNDO_COLLAPSE ));

        case OLUNDO_ATTR:
            return XubString( EditResId( RID_OUTLUNDO_ATTR ));

        case OLUNDO_INSERT:
            return XubString( EditResId( RID_OUTLUNDO_INSERT ));

        default:
            return EditEngine::GetUndoComment( nUndoId );
    }
#else // SVX_LIGHT
    XubString aString;
    return aString;
#endif
}

// #101498#
void OutlinerEditEng::DrawingText( const Point& rStartPos, const XubString& rText, USHORT nTextStart, USHORT nTextLen, const long* pDXArray, const SvxFont& rFont, USHORT nPara, USHORT nIndex, BYTE nRightToLeft)
{
    if ( nIndex == 0 )
    {
        // Dann das Bullet 'malen', dort wird bStrippingPortions ausgewertet
        // und Outliner::DrawingText gerufen

        // DrawingText liefert die BaseLine, DrawBullet braucht Top().
        Point aCorrectedPos( rStartPos );
        aCorrectedPos.Y() = GetDocPosTopLeft( nPara ).Y();
        aCorrectedPos.Y() += GetFirstLineOffset( nPara );
        pOwner->PaintBullet( nPara, aCorrectedPos, Point(), 0, GetRefDevice() );
    }

    // #101498#
    pOwner->DrawingText(rStartPos,rText,nTextStart,nTextLen,pDXArray,rFont,nPara,nIndex,nRightToLeft);
}

void OutlinerEditEng::FieldClicked( const SvxFieldItem& rField, USHORT nPara, USHORT nPos )
{
    EditEngine::FieldClicked( rField, nPara, nPos );    // Falls URL
    pOwner->FieldClicked( rField, nPara, nPos );
}

void OutlinerEditEng::FieldSelected( const SvxFieldItem& rField, USHORT nPara, USHORT nPos )
{
    pOwner->FieldSelected( rField, nPara, nPos );
}

XubString OutlinerEditEng::CalcFieldValue( const SvxFieldItem& rField, USHORT nPara, USHORT nPos, Color*& rpTxtColor, Color*& rpFldColor )
{
    return pOwner->CalcFieldValue( rField, nPara, nPos, rpTxtColor, rpFldColor );
}
