/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: edtspell.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 17:33:17 $
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

#ifndef _EDTSPELL_HXX
#define _EDTSPELL_HXX

#include <svxbox.hxx>
#include <svxenum.hxx>
#include <splwrap.hxx>
#include <svxacorr.hxx>

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef INCLUDED_SVXDLLAPI_H
#include "svx/svxdllapi.h"
#endif

namespace com { namespace sun { namespace star { namespace linguistic2 {
    class XSpellChecker1;
}}}}


class EditView;
class ImpEditEngine;
class ContentNode;

class EditSpellWrapper : public SvxSpellWrapper
{
private:
    EditView*               pEditView;
    void                    CheckSpellTo();

protected:
    virtual void            SpellStart( SvxSpellArea eArea );
    virtual BOOL            SpellContinue();    // Bereich pruefen
    virtual void            ReplaceAll( const String &rNewText, INT16 nLanguage );
    virtual void            SpellEnd();
//  virtual BOOL            ();
    virtual BOOL            SpellMore();
    virtual BOOL            HasOtherCnt();
    virtual void            ScrollArea();
    virtual void            ChangeWord( const String& rNewWord, const USHORT nLang );
    virtual void            ChangeThesWord( const String& rNewWord );
//  virtual void            ChangeAll( const String& rNewWord );
    virtual void            AutoCorrect( const String& rOldWord, const String& rNewWord );

//  virtual String          GetCurrentWord() const;

public:
    EditSpellWrapper( Window* pWin,
            ::com::sun::star::uno::Reference<
                ::com::sun::star::linguistic2::XSpellChecker1 > &xChecker,
            BOOL bIsStart,
            BOOL bIsAllRight, EditView* pView );

};


struct WrongRange
{
    USHORT nStart;
    USHORT nEnd;

    WrongRange( USHORT nS, USHORT nE ) { nStart = nS; nEnd = nE; }
};

SV_DECL_VARARR( WrongRanges, WrongRange, 4, 4 );
#define NOT_INVALID 0xFFFF

class WrongList : private WrongRanges
{
private:
    USHORT  nInvalidStart;
    USHORT  nInvalidEnd;

    BOOL    DbgIsBuggy() const;

public:
            WrongList();
            ~WrongList();

    BOOL    IsInvalid() const       { return nInvalidStart != NOT_INVALID; }
    void    SetValid()              { nInvalidStart = NOT_INVALID; nInvalidEnd = 0; }
    void    MarkInvalid( USHORT nS, USHORT nE )
            {
                if ( ( nInvalidStart == NOT_INVALID ) || ( nInvalidStart > nS ) )
                    nInvalidStart = nS;
                if ( nInvalidEnd < nE )
                    nInvalidEnd = nE;
            }

    USHORT          Count() const               { return WrongRanges::Count(); }

    // Wenn man weiss was man tut:
    WrongRange&     GetObject( USHORT n ) const { return WrongRanges::GetObject( n ); }
    void            InsertWrong( const WrongRange& rWrong, USHORT nPos );

    USHORT  GetInvalidStart() const { return nInvalidStart; }
    USHORT& GetInvalidStart()       { return nInvalidStart; }

    USHORT  GetInvalidEnd() const   { return nInvalidEnd; }
    USHORT& GetInvalidEnd()         { return nInvalidEnd; }

    void    TextInserted( USHORT nPos, USHORT nChars, BOOL bPosIsSep );
    void    TextDeleted( USHORT nPos, USHORT nChars );

    void    ResetRanges()           { Remove( 0, Count() ); }
    BOOL    HasWrongs() const       { return Count() != 0; }
    void    InsertWrong( USHORT nStart, USHORT nEnd, BOOL bClearRange );
    BOOL    NextWrong( USHORT& rnStart, USHORT& rnEnd ) const;
    BOOL    HasWrong( USHORT nStart, USHORT nEnd ) const;
    BOOL    HasAnyWrong( USHORT nStart, USHORT nEnd ) const;
    void    ClearWrongs( USHORT nStart, USHORT nEnd, const ContentNode* pNode );
    void    MarkWrongsInvalid();

    WrongList*  Clone() const;
};

inline void WrongList::InsertWrong( const WrongRange& rWrong, USHORT nPos )
{
    WrongRanges::Insert( rWrong, nPos );
#ifdef DBG_UTIL
    DBG_ASSERT( !DbgIsBuggy(), "Insert: WrongList kaputt!" );
#endif
}



class SVX_DLLPUBLIC EdtAutoCorrDoc : public SvxAutoCorrDoc
{
    ImpEditEngine*  pImpEE;
    ContentNode*    pCurNode;
    USHORT          nCursor;

    BOOL            bAllowUndoAction;
    BOOL            bUndoAction;

protected:
    void            ImplStartUndoAction();

public:
                    EdtAutoCorrDoc( ImpEditEngine* pImpEE, ContentNode* pCurNode, USHORT nCrsr, xub_Unicode cIns );
                    ~EdtAutoCorrDoc();

    virtual BOOL    Delete( USHORT nStt, USHORT nEnd );
    virtual BOOL    Insert( USHORT nPos, const String& rTxt );
    virtual BOOL    Replace( USHORT nPos, const String& rTxt );

    virtual BOOL    SetAttr( USHORT nStt, USHORT nEnd, USHORT nSlotId, SfxPoolItem& );
    virtual BOOL    SetINetAttr( USHORT nStt, USHORT nEnd, const String& rURL );

    virtual BOOL    HasSymbolChars( USHORT nStt, USHORT nEnd );

    virtual const String* GetPrevPara( BOOL bAtNormalPos );

    virtual BOOL    ChgAutoCorrWord( USHORT& rSttPos, USHORT nEndPos,
                                  SvxAutoCorrect& rACorrect, const String** ppPara );

    virtual LanguageType GetLanguage( USHORT nPos, BOOL bPrevPara = FALSE ) const;

    USHORT          GetCursor() const { return nCursor; }

};

#endif  // EDTSPELL

