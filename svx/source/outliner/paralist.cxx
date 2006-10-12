/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: paralist.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2006-10-12 13:03:24 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svx.hxx"

#include <paralist.hxx>
#include <outliner.hxx>     // nur wegen Paragraph, muss geaendert werden!
#include <numdef.hxx>

DBG_NAME(Paragraph)

Paragraph::Paragraph( USHORT nDDepth )
    : aBulSize( -1, -1)
{
    DBG_CTOR( Paragraph, 0 );

    DBG_ASSERT( ( nDDepth < SVX_MAX_NUM ) || ( nDDepth == 0xFFFF ), "Paragraph-CTOR: nDepth invalid!" );

    nDepth = nDDepth;
    nFlags = 0;
    bVisible = TRUE;
}

Paragraph::Paragraph( const Paragraph& rPara )
:   aBulText( rPara.aBulText )
,   aBulSize( rPara.aBulSize )
{
    DBG_CTOR( Paragraph, 0 );

    nDepth = rPara.nDepth;
    nFlags = rPara.nFlags;
    bVisible = rPara.bVisible;
}

Paragraph::~Paragraph()
{
    DBG_DTOR( Paragraph, 0 );
}

void ParagraphList::Clear( BOOL bDestroyParagraphs )
{
    if ( bDestroyParagraphs )
    {
        for ( ULONG n = GetParagraphCount(); n; )
        {
            Paragraph* pPara = GetParagraph( --n );
            delete pPara;
        }
    }
    List::Clear();
}

void ParagraphList::MoveParagraphs( ULONG nStart, ULONG nDest, ULONG _nCount )
{
    if ( ( nDest < nStart ) || ( nDest >= ( nStart + _nCount ) ) )
    {
        ULONG n;
        ParagraphList aParas;
        for ( n = 0; n < _nCount; n++ )
        {
            Paragraph* pPara = GetParagraph( nStart );
            aParas.Insert( pPara, LIST_APPEND );
            Remove( nStart );
        }

        if ( nDest > nStart )
            nDest -= _nCount;

        for ( n = 0; n < _nCount; n++ )
        {
            Paragraph* pPara = aParas.GetParagraph( n );
            Insert( pPara, nDest++ );
        }
    }
    else
        DBG_ERROR( "MoveParagraphs: Invalid Parameters" );
}

Paragraph* ParagraphList::NextVisible( Paragraph* pPara ) const
{
    ULONG n = GetAbsPos( pPara );

    Paragraph* p = GetParagraph( ++n );
    while ( p && !p->IsVisible() )
        p = GetParagraph( ++n );

    return p;
}

Paragraph* ParagraphList::PrevVisible( Paragraph* pPara ) const
{
    ULONG n = GetAbsPos( pPara );

    Paragraph* p = n ? GetParagraph( --n ) : NULL;
    while ( p && !p->IsVisible() )
        p = n ? GetParagraph( --n ) : NULL;

    return p;
}

Paragraph* ParagraphList::LastVisible() const
{
    ULONG n = GetParagraphCount();

    Paragraph* p = n ? GetParagraph( --n ) : NULL;
    while ( p && !p->IsVisible() )
        p = n ? GetParagraph( --n ) : NULL;

    return p;
}

BOOL ParagraphList::HasChilds( Paragraph* pParagraph ) const
{
    ULONG n = GetAbsPos( pParagraph );
    Paragraph* pNext = GetParagraph( ++n );
    return ( pNext && ( pNext->GetDepth() > pParagraph->GetDepth() ) ) ? TRUE : FALSE;
}

BOOL ParagraphList::HasHiddenChilds( Paragraph* pParagraph ) const
{
    ULONG n = GetAbsPos( pParagraph );
    Paragraph* pNext = GetParagraph( ++n );
    return ( pNext && ( pNext->GetDepth() > pParagraph->GetDepth() ) && !pNext->IsVisible() ) ? TRUE : FALSE;
}

BOOL ParagraphList::HasVisibleChilds( Paragraph* pParagraph ) const
{
    ULONG n = GetAbsPos( pParagraph );
    Paragraph* pNext = GetParagraph( ++n );
    return ( pNext && ( pNext->GetDepth() > pParagraph->GetDepth() ) && pNext->IsVisible() ) ? TRUE : FALSE;
}

ULONG ParagraphList::GetChildCount( Paragraph* pParent ) const
{
    ULONG nChildCount = 0;
    ULONG n = GetAbsPos( pParent );
    Paragraph* pPara = GetParagraph( ++n );
    while ( pPara && ( pPara->GetDepth() > pParent->GetDepth() ) )
    {
        nChildCount++;
        pPara = GetParagraph( ++n );
    }
    return nChildCount;
}

Paragraph* ParagraphList::GetParent( Paragraph* pParagraph, USHORT& rRelPos ) const
{
    rRelPos = 0;
    ULONG n = GetAbsPos( pParagraph );
    Paragraph* pPrev = GetParagraph( --n );
    while ( pPrev && ( pPrev->GetDepth() >= pParagraph->GetDepth() ) )
    {
        if ( pPrev->GetDepth() == pParagraph->GetDepth() )
            rRelPos++;
        pPrev = GetParagraph( --n );
    }

    return pPrev;
}

void ParagraphList::Expand( Paragraph* pParent )
{
    ULONG nChildCount = GetChildCount( pParent );
    ULONG nPos = GetAbsPos( pParent );

    for ( ULONG n = 1; n <= nChildCount; n++  )
    {
        Paragraph* pPara = GetParagraph( nPos+n );
        if ( !( pPara->IsVisible() ) )
        {
            pPara->bVisible = TRUE;
            aVisibleStateChangedHdl.Call( pPara );
        }
    }
}

void ParagraphList::Collapse( Paragraph* pParent )
{
    ULONG nChildCount = GetChildCount( pParent );
    ULONG nPos = GetAbsPos( pParent );

    for ( ULONG n = 1; n <= nChildCount; n++  )
    {
        Paragraph* pPara = GetParagraph( nPos+n );
        if ( pPara->IsVisible() )
        {
            pPara->bVisible = FALSE;
            aVisibleStateChangedHdl.Call( pPara );
        }
    }
}

ULONG ParagraphList::GetVisPos( Paragraph* pPara )
{
    ULONG nVisPos = 0;
    ULONG nPos = GetAbsPos( pPara );
    for ( ULONG n = 0; n < nPos; n++ )
    {
        Paragraph* _pPara = GetParagraph( n );
        if ( _pPara->IsVisible() )
            nVisPos++;
    }
    return nVisPos;
}
