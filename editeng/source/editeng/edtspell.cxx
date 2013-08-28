/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */


#include <vcl/wrkwin.hxx>
#include <vcl/dialog.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/svapp.hxx>

#include <impedit.hxx>
#include <editeng/editview.hxx>
#include <editeng/editeng.hxx>
#include <edtspell.hxx>
#include <editeng/flditem.hxx>
#include <editeng/fontitem.hxx>
#include <svl/intitem.hxx>
#include <svl/eitem.hxx>
#include <editeng/unolingu.hxx>
#include <linguistic/lngprops.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>

using namespace com::sun::star::uno;
using namespace com::sun::star::beans;
using namespace com::sun::star::linguistic2;


EditSpellWrapper::EditSpellWrapper( Window* _pWin,
        Reference< XSpellChecker1 >  &xChecker,
        bool bIsStart, bool bIsAllRight, EditView* pView ) :
    SvxSpellWrapper( _pWin, xChecker, bIsStart, bIsAllRight )
{
    SAL_WARN_IF( !pView, "editeng", "One view has to be abandoned!" );
    // Keep IgnoreList, delete ReplaceList...
    if (SvxGetChangeAllList().is())
        SvxGetChangeAllList()->clear();
    pEditView = pView;
}

void EditSpellWrapper::SpellStart( SvxSpellArea eArea )
{
    EditEngine* pEE = pEditView->GetEditEngine();
    ImpEditEngine* pImpEE = pEditView->GetImpEditEngine();
    SpellInfo* pSpellInfo = pImpEE->GetSpellInfo();

    if ( eArea == SVX_SPELL_BODY_START )
    {
        // Is called when
        // a) Spell-Forward has arrived at the end and should restart at the top
        // IsEndDone() returns also true, when backward-spelling is started at the end!
        if ( IsEndDone() )
        {
            pSpellInfo->bSpellToEnd = false;
            pSpellInfo->aSpellTo = pSpellInfo->aSpellStart;
            pEditView->GetImpEditView()->SetEditSelection(
                    pEE->GetEditDoc().GetStartPaM() );
        }
        else
        {
            pSpellInfo->bSpellToEnd = true;
            pSpellInfo->aSpellTo = pImpEE->CreateEPaM(
                    pEE->GetEditDoc().GetStartPaM() );
        }
    }
    else if ( eArea == SVX_SPELL_BODY_END )
    {
        // Is called when
        // a) Spell-Forward is launched
        // IsStartDone() return also true, when forward-spelling is started at the beginning!
        if ( !IsStartDone() )
        {
            pSpellInfo->bSpellToEnd = true;
            pSpellInfo->aSpellTo = pImpEE->CreateEPaM(
                    pEE->GetEditDoc().GetEndPaM() );
        }
        else
        {
            pSpellInfo->bSpellToEnd = false;
            pSpellInfo->aSpellTo = pSpellInfo->aSpellStart;
            pEditView->GetImpEditView()->SetEditSelection(
                    pEE->GetEditDoc().GetEndPaM() );
        }
    }
    else if ( eArea == SVX_SPELL_BODY )
    {
        ;   // Is handled by the App through SpellNextDocument
    }
    else
    {
        OSL_FAIL( "SpellStart: Unknown Area!" );
    }
}

sal_Bool EditSpellWrapper::SpellContinue()
{
    SetLast( pEditView->GetImpEditEngine()->ImpSpell( pEditView ) );
    return GetLast().is();
}

void EditSpellWrapper::SpellEnd()
{
    // Base class will show language errors...
    SvxSpellWrapper::SpellEnd();
}

sal_Bool EditSpellWrapper::HasOtherCnt()
{
    return false;
}

sal_Bool EditSpellWrapper::SpellMore()
{
    EditEngine* pEE = pEditView->GetEditEngine();
    ImpEditEngine* pImpEE = pEditView->GetImpEditEngine();
    SpellInfo* pSpellInfo = pImpEE->GetSpellInfo();
    bool bMore = false;
    if ( pSpellInfo->bMultipleDoc )
    {
        bMore = pEE->SpellNextDocument();
        if ( bMore )
        {
            // The text has been entered into the engine, when backwords then
            // it must be behind the selection.
            pEditView->GetImpEditView()->SetEditSelection(
                        pEE->GetEditDoc().GetStartPaM() );
        }
    }
    return bMore;
}

void EditSpellWrapper::ScrollArea()
{
    // No further action needed ...
    // Except for, that the area is to be scrolled in the center, and not stand
    // still anywhere.
}

void EditSpellWrapper::ReplaceAll( const String &rNewText,
            sal_Int16 )
{
    // Is called when the word is in ReplaceList of the spell checker
    pEditView->InsertText( rNewText );
    CheckSpellTo();
}

void EditSpellWrapper::ChangeWord( const String& rNewWord,
            const sal_uInt16 )
{
    // Will be called when Word Button Change
    // or internally by me ChangeAll

    // If there is a dot Punkt after the word, this dot will be stripped away.
    // If '"' => PreStripped.
    String aNewWord( rNewWord );
    pEditView->InsertText( aNewWord );
    CheckSpellTo();
}

void EditSpellWrapper::ChangeThesWord( const String& rNewWord )
{
    pEditView->InsertText( rNewWord );
    CheckSpellTo();
}

void EditSpellWrapper::AutoCorrect( const String&, const String& )
{
}

void EditSpellWrapper::CheckSpellTo()
{
    ImpEditEngine* pImpEE = pEditView->GetImpEditEngine();
    SpellInfo* pSpellInfo = pImpEE->GetSpellInfo();
    EditPaM aPaM( pEditView->GetImpEditView()->GetEditSelection().Max() );
    EPaM aEPaM = pImpEE->CreateEPaM( aPaM );
    if ( aEPaM.nPara == pSpellInfo->aSpellTo.nPara )
    {
        // Check if SpellToEnd still has a valid Index, if replace has been
        // performed in the paragraph.
        if ( pSpellInfo->aSpellTo.nIndex > aPaM.GetNode()->Len() )
            pSpellInfo->aSpellTo.nIndex = aPaM.GetNode()->Len();
    }
}

sal_uInt16 WrongList::Valid = std::numeric_limits<sal_uInt16>::max();

WrongList::WrongList() : nInvalidStart(0), nInvalidEnd(Valid) {}

WrongList::WrongList(const WrongList& r) :
    maRanges(r.maRanges),
    nInvalidStart(r.nInvalidStart),
    nInvalidEnd(r.nInvalidEnd) {}

WrongList::~WrongList() {}

bool WrongList::IsValid() const
{
    return nInvalidStart == Valid;
}

void WrongList::SetValid()
{
    nInvalidStart = Valid;
    nInvalidEnd = 0;
}

void WrongList::MarkInvalid( sal_uInt16 nS, sal_uInt16 nE )
{
    if ( (nInvalidStart == Valid) || ( nInvalidStart > nS ) )
        nInvalidStart = nS;
    if ( nInvalidEnd < nE )
        nInvalidEnd = nE;
}

void WrongList::TextInserted( sal_uInt16 nPos, sal_uInt16 nLength, bool bPosIsSep )
{
    if (IsValid())
    {
        nInvalidStart = nPos;
        nInvalidEnd = nPos + nLength;
    }
    else
    {
        if ( nInvalidStart > nPos )
            nInvalidStart = nPos;
        if ( nInvalidEnd >= nPos )
            nInvalidEnd = nInvalidEnd + nLength;
        else
            nInvalidEnd = nPos + nLength;
    }

    for (size_t i = 0, n = maRanges.size(); i < n; ++i)
    {
        WrongRange& rWrong = maRanges[i];
        bool bRefIsValid = true;
        if (rWrong.nEnd >= nPos)
        {
            // Move all Wrongs after the insert position...
            if (rWrong.nStart > nPos)
            {
                rWrong.nStart += nLength;
                rWrong.nEnd += nLength;
            }
            // 1: Starts before and goes until nPos...
            else if (rWrong.nEnd == nPos)
            {
                // Should be halted at a blank!
                if ( !bPosIsSep )
                    rWrong.nEnd += nLength;
            }
            // 2: Starts before and goes until after nPos...
            else if ((rWrong.nStart < nPos) && (rWrong.nEnd > nPos))
            {
                rWrong.nEnd += nLength;
                // When a separator remove and re-examine the Wrong
                if ( bPosIsSep )
                {
                    // Split Wrong...
                    WrongRange aNewWrong(rWrong.nStart, nPos);
                    rWrong.nStart = nPos + 1;
                    maRanges.insert(maRanges.begin() + i, aNewWrong);
                    // Reference no longer valid after Insert, the other
                    // was inserted in front of this position
                    bRefIsValid = false;
                    ++i; // Not this again...
                }
            }
            // 3: Attribute starts at position ..
            else if (rWrong.nStart == nPos)
            {
                rWrong.nEnd += nLength;
                if ( bPosIsSep )
                    ++(rWrong.nStart);
            }
        }
        SAL_WARN_IF(bRefIsValid && rWrong.nStart >= rWrong.nEnd, "editeng",
                "TextInserted, WrongRange: Start >= End?!");
        (void)bRefIsValid;
    }

    SAL_WARN_IF(DbgIsBuggy(), "editeng", "InsertWrong: WrongList broken!");
}

void WrongList::TextDeleted( sal_uInt16 nPos, sal_uInt16 nLength )
{
    sal_uInt16 nEndPos = nPos + nLength;
    if (IsValid())
    {
        sal_uInt16 nNewInvalidStart = nPos ? nPos - 1 : 0;
        nInvalidStart = nNewInvalidStart;
        nInvalidEnd = nNewInvalidStart + 1;
    }
    else
    {
        if ( nInvalidStart > nPos )
            nInvalidStart = nPos;
        if ( nInvalidEnd > nPos )
        {
            if (nInvalidEnd > nEndPos)
                nInvalidEnd = nInvalidEnd - nLength;
            else
                nInvalidEnd = nPos+1;
        }
    }

    for (WrongList::iterator i = begin(); i != end(); )
    {
        bool bDelWrong = false;
        if (i->nEnd >= nPos)
        {
            // Move all Wrongs after the insert position...
            if (i->nStart >= nEndPos)
            {
                i->nStart -= nLength;
                i->nEnd -= nLength;
            }
            // 1. Delete Internal Wrongs ...
            else if (i->nStart >= nPos && i->nEnd <= nEndPos)
            {
                bDelWrong = true;
            }
            // 2. Wrong begins before, ends inside or behind it ...
            else if (i->nStart <= nPos && i->nEnd > nPos)
            {
                if (i->nEnd <= nEndPos)   // ends inside
                    i->nEnd = nPos;
                else
                    i->nEnd -= nLength; // ends after
            }
            // 3. Wrong begins inside, ending after ...
            else if (i->nStart >= nPos && i->nEnd > nEndPos)
            {
                i->nStart = nEndPos - nLength;
                i->nEnd -= nLength;
            }
        }
        SAL_WARN_IF(i->nStart >= i->nEnd, "editeng",
                "TextDeleted, WrongRange: Start >= End?!");
        if ( bDelWrong )
        {
            i = maRanges.erase(i);
        }
        else
        {
            ++i;
        }
    }

    SAL_WARN_IF(DbgIsBuggy(), "editeng", "TextDeleted: WrongList broken!");
}

bool WrongList::NextWrong( sal_uInt16& rnStart, sal_uInt16& rnEnd ) const
{
    /*
        rnStart get the start position, is possibly adjusted wrt. Wrong start
        rnEnd does not have to be initialized.
    */
    for (WrongList::const_iterator i = begin(); i != end(); ++i)
    {
        if ( i->nEnd > rnStart )
        {
            rnStart = i->nStart;
            rnEnd = i->nEnd;
            return true;
        }
    }
    return false;
}

bool WrongList::HasWrong( sal_uInt16 nStart, sal_uInt16 nEnd ) const
{
    for (WrongList::const_iterator i = begin(); i != end(); ++i)
    {
        if (i->nStart == nStart && i->nEnd == nEnd)
            return true;
        else if ( i->nStart >= nStart )
            break;
    }
    return false;
}

bool WrongList::HasAnyWrong( sal_uInt16 nStart, sal_uInt16 nEnd ) const
{
    for (WrongList::const_iterator i = begin(); i != end(); ++i)
    {
        if (i->nEnd >= nStart && i->nStart < nEnd)
            return true;
        else if (i->nStart >= nEnd)
            break;
    }
    return false;
}

void WrongList::ClearWrongs( sal_uInt16 nStart, sal_uInt16 nEnd,
            const ContentNode* pNode )
{
    for (WrongList::iterator i = begin(); i != end(); )
    {
        if (i->nEnd > nStart && i->nStart < nEnd)
        {
            if (i->nEnd > nEnd) // Runs out
            {
                i->nStart = nEnd;
                // Blanks?
                while (i->nStart < pNode->Len() &&
                       (pNode->GetChar(i->nStart) == ' ' ||
                        pNode->IsFeature(i->nStart)))
                {
                    ++(i->nStart);
                }
                ++i;
            }
            else
            {
                i = maRanges.erase(i);
                // no increment here
            }
        }
        else
        {
            ++i;
        }
    }

    SAL_WARN_IF(DbgIsBuggy(), "editeng", "ClearWrongs: WrongList broken!");
}

void WrongList::InsertWrong( sal_uInt16 nStart, sal_uInt16 nEnd )
{
    WrongList::iterator nPos = end();
    for (WrongList::iterator i = begin(); i != end(); ++i)
    {
        if (i->nStart >= nStart )
        {
            nPos = i;
            {
                // It can really only happen that the Wrong starts exactly here
                // and runs along, but not that there are several ranges ...
                // Exactly in the range is no one allowed to be, otherwise this
                // Method can not be called!
                SAL_WARN_IF((i->nStart != nStart || i->nEnd <= nEnd) && i->nStart <= nEnd, "editeng", "InsertWrong: RangeMismatch!");
                if (i->nStart == nStart && i->nEnd > nEnd)
                    i->nStart = nEnd + 1;
            }
            break;
        }
    }

    if (nPos != maRanges.end())
        maRanges.insert(nPos, WrongRange(nStart, nEnd));
    else
        maRanges.push_back(WrongRange(nStart, nEnd));

    SAL_WARN_IF(DbgIsBuggy(), "editeng", "InsertWrong: WrongList broken!");
}

void WrongList::MarkWrongsInvalid()
{
    if (!maRanges.empty())
        MarkInvalid(maRanges.front().nStart, maRanges.back().nEnd );
}

WrongList* WrongList::Clone() const
{
    return new WrongList(*this);
}

// #i102062#
bool WrongList::operator==(const WrongList& rCompare) const
{
    // cleck direct members
    if(GetInvalidStart() != rCompare.GetInvalidStart()
        || GetInvalidEnd() != rCompare.GetInvalidEnd()
        || maRanges.size() != rCompare.maRanges.size())
        return false;

    WrongList::const_iterator rCA = maRanges.begin();
    WrongList::const_iterator rCB = rCompare.maRanges.begin();

    for (; rCA != maRanges.end(); ++rCA, ++rCB)
    {
        if(rCA->nStart != rCB->nStart || rCA->nEnd != rCB->nEnd)
            return false;
    }

    return true;
}

bool WrongList::empty() const
{
    return maRanges.empty();
}

void WrongList::push_back(const WrongRange& rRange)
{
    maRanges.push_back(rRange);
}

WrongRange& WrongList::back()
{
    return maRanges.back();
}

const WrongRange& WrongList::back() const
{
    return maRanges.back();
}

WrongList::iterator WrongList::begin()
{
    return maRanges.begin();
}

WrongList::iterator WrongList::end()
{
    return maRanges.end();
}

WrongList::const_iterator WrongList::begin() const
{
    return maRanges.begin();
}

WrongList::const_iterator WrongList::end() const
{
    return maRanges.end();
}

bool WrongList::DbgIsBuggy() const
{
    // Check if the ranges overlap.
    bool bError = false;
    for (WrongList::const_iterator i = begin(); !bError && (i != end()); ++i)
    {
        for (WrongList::const_iterator j = i + 1; !bError && (j != end()); ++j)
        {
            // 1) Start before, End after the second Start
            if (i->nStart <= j->nStart && i->nEnd >= j->nStart)
                bError = true;
            // 2) Start after the second Start, but still before the second End
            else if (i->nStart >= j->nStart && i->nStart <= j->nEnd)
                bError = true;
        }
    }
    return bError;
}

//////////////////////////////////////////////////////////////////////

EdtAutoCorrDoc::EdtAutoCorrDoc(
    EditEngine* pE, ContentNode* pN, sal_uInt16 nCrsr, sal_Unicode cIns) :
    mpEditEngine(pE),
    pCurNode(pN),
    nCursor(nCrsr),
    bAllowUndoAction(cIns != 0),
    bUndoAction(false) {}

EdtAutoCorrDoc::~EdtAutoCorrDoc()
{
    if ( bUndoAction )
        mpEditEngine->UndoActionEnd( EDITUNDO_INSERT );
}

sal_Bool EdtAutoCorrDoc::Delete( sal_uInt16 nStt, sal_uInt16 nEnd )
{
    EditSelection aSel( EditPaM( pCurNode, nStt ), EditPaM( pCurNode, nEnd ) );
    mpEditEngine->DeleteSelection(aSel);
    SAL_WARN_IF(nCursor < nEnd, "editeng",
            "Cursor in the heart of the action?!");
    nCursor -= ( nEnd-nStt );
    bAllowUndoAction = false;
    return true;
}

sal_Bool EdtAutoCorrDoc::Insert( sal_uInt16 nPos, const String& rTxt )
{
    EditSelection aSel = EditPaM( pCurNode, nPos );
    mpEditEngine->InsertText(aSel, rTxt);
    SAL_WARN_IF(nCursor < nPos, "editeng",
            "Cursor in the heart of the action?!");
    nCursor = nCursor + rTxt.Len();

    if ( bAllowUndoAction && ( rTxt.Len() == 1 ) )
        ImplStartUndoAction();
    bAllowUndoAction = false;

    return true;
}

sal_Bool EdtAutoCorrDoc::Replace( sal_uInt16 nPos, const String& rTxt )
{
    return ReplaceRange( nPos, rTxt.Len(), rTxt );
}

sal_Bool EdtAutoCorrDoc::ReplaceRange( xub_StrLen nPos, xub_StrLen nSourceLength, const String& rTxt )
{
    // Actually a Replace introduce => corresponds to UNDO
    sal_uInt16 nEnd = nPos+nSourceLength;
    if ( nEnd > pCurNode->Len() )
        nEnd = pCurNode->Len();

    // #i5925# First insert new text behind to be deleted text, for keeping attributes.
    mpEditEngine->InsertText(EditSelection(EditPaM(pCurNode, nEnd)), rTxt);
    mpEditEngine->DeleteSelection(
        EditSelection(EditPaM(pCurNode, nPos), EditPaM(pCurNode, nEnd)));

    if ( nPos == nCursor )
        nCursor = nCursor + rTxt.Len();

    if ( bAllowUndoAction && ( rTxt.Len() == 1 ) )
        ImplStartUndoAction();

    bAllowUndoAction = false;

    return true;
}

sal_Bool EdtAutoCorrDoc::SetAttr( sal_uInt16 nStt, sal_uInt16 nEnd,
            sal_uInt16 nSlotId, SfxPoolItem& rItem )
{
    SfxItemPool* pPool = &mpEditEngine->GetEditDoc().GetItemPool();
    while ( pPool->GetSecondaryPool() &&
            !pPool->GetName().equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("EditEngineItemPool")) )
    {
        pPool = pPool->GetSecondaryPool();

    }
    sal_uInt16 nWhich = pPool->GetWhich( nSlotId );
    if ( nWhich )
    {
        rItem.SetWhich( nWhich );

        SfxItemSet aSet = mpEditEngine->GetEmptyItemSet();
        aSet.Put( rItem );

        EditSelection aSel( EditPaM( pCurNode, nStt ), EditPaM( pCurNode, nEnd ) );
        aSel.Max().SetIndex( nEnd );    // ???
        mpEditEngine->SetAttribs( aSel, aSet, ATTRSPECIAL_EDGE );
        bAllowUndoAction = false;
    }
    return true;
}

sal_Bool EdtAutoCorrDoc::SetINetAttr( sal_uInt16 nStt, sal_uInt16 nEnd,
            const OUString& rURL )
{
    // Turn the Text into a command field ...
    EditSelection aSel( EditPaM( pCurNode, nStt ), EditPaM( pCurNode, nEnd ) );
    String aText = mpEditEngine->GetSelected(aSel);
    aSel = mpEditEngine->DeleteSelection(aSel);
    SAL_WARN_IF(nCursor < nEnd, "editeng",
            "Cursor in the heart of the action?!");
    nCursor -= ( nEnd-nStt );
    SvxFieldItem aField( SvxURLField( rURL, aText, SVXURLFORMAT_REPR ),
                                      EE_FEATURE_FIELD  );
    mpEditEngine->InsertField(aSel, aField);
    nCursor++;
    mpEditEngine->UpdateFieldsOnly();
    bAllowUndoAction = false;
    return true;
}

const String* EdtAutoCorrDoc::GetPrevPara( sal_Bool )
{
    // Return previous paragraph, so that it can be determined,
    // whether the current word is at the beginning of a sentence.

    bAllowUndoAction = false;   // Not anymore ...

    EditDoc& rNodes = mpEditEngine->GetEditDoc();
    sal_Int32 nPos = rNodes.GetPos( pCurNode );

    // Special case: Bullet => Paragraph start => simply return NULL...
    const SfxBoolItem& rBulletState = (const SfxBoolItem&)
            mpEditEngine->GetParaAttrib( nPos, EE_PARA_BULLETSTATE );
    bool bBullet = rBulletState.GetValue() ? true : false;
    if ( !bBullet && (mpEditEngine->GetControlWord() & EE_CNTRL_OUTLINER) )
    {
        // The Outliner has still a Bullet at Level 0.
        const SfxInt16Item& rLevel = (const SfxInt16Item&)
                mpEditEngine->GetParaAttrib( nPos, EE_PARA_OUTLLEVEL );
        if ( rLevel.GetValue() == 0 )
            bBullet = true;
    }
    if ( bBullet )
        return NULL;

    for ( sal_Int32 n = nPos; n; )
    {
        n--;
        ContentNode* pNode = rNodes[n];
        if ( pNode->Len() )
            return &pNode->GetString();
    }
    return NULL;

}

sal_Bool EdtAutoCorrDoc::ChgAutoCorrWord( sal_uInt16& rSttPos,
            sal_uInt16 nEndPos, SvxAutoCorrect& rACorrect,
            const String** ppPara )
{
    // Paragraph-start or a blank found, search for the word
    // shortcut in Auto
    bAllowUndoAction = false;   // Not anymore ...

    String aShort( pCurNode->Copy( rSttPos, nEndPos - rSttPos ) );
    bool bRet = false;

    if( !aShort.Len() )
        return bRet;

    LanguageType eLang = mpEditEngine->GetLanguage( EditPaM( pCurNode, rSttPos+1 ) );
    const SvxAutocorrWord* pFnd = rACorrect.SearchWordsInList(pCurNode->GetString(), rSttPos, nEndPos, *this, eLang);
    if( pFnd && pFnd->IsTextOnly() )
    {
        // then replace
        EditSelection aSel( EditPaM( pCurNode, rSttPos ),
                            EditPaM( pCurNode, nEndPos ) );
        aSel = mpEditEngine->DeleteSelection(aSel);
        SAL_WARN_IF(nCursor < nEndPos, "editeng",
                "Cursor in the heart of the action?!");
        nCursor -= ( nEndPos-rSttPos );
        mpEditEngine->InsertText(aSel, pFnd->GetLong());
        nCursor = nCursor + pFnd->GetLong().Len();
        if( ppPara )
            *ppPara = &pCurNode->GetString();
        bRet = true;
    }

    return bRet;
}

LanguageType EdtAutoCorrDoc::GetLanguage( sal_uInt16 nPos, sal_Bool ) const
{
    return mpEditEngine->GetLanguage( EditPaM( pCurNode, nPos+1 ) );
}

void EdtAutoCorrDoc::ImplStartUndoAction()
{
    sal_Int32 nPara = mpEditEngine->GetEditDoc().GetPos( pCurNode );
    ESelection aSel( nPara, nCursor, nPara, nCursor );
    mpEditEngine->UndoActionStart( EDITUNDO_INSERT, aSel );
    bUndoAction = true;
    bAllowUndoAction = false;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
