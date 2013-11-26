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

#include <editeng/unoedhlp.hxx>
#include <editeng/editdata.hxx>
#include <editeng/editeng.hxx>
#include <svl/itemset.hxx>

//------------------------------------------------------------------------

TYPEINIT1( SvxEditSourceHint, TextHint );

SvxEditSourceHint::SvxEditSourceHint( sal_uLong _nId ) :
    TextHint( _nId ),
    mnStart( 0 ),
    mnEnd( 0 )
{
}

SvxEditSourceHint::SvxEditSourceHint( sal_uLong _nId, sal_uLong nValue, sal_Int32 nStart, sal_Int32 nEnd ) :
    TextHint( _nId, nValue ),
    mnStart( nStart),
    mnEnd( nEnd )
{
}

sal_uLong SvxEditSourceHint::GetValue() const
{
    return TextHint::GetValue();
}

sal_Int32 SvxEditSourceHint::GetStartValue() const
{
    return mnStart;
}

sal_Int32 SvxEditSourceHint::GetEndValue() const
{
    return mnEnd;
}
TYPEINIT1( SvxEditSourceHintEndPara , SvxEditSourceHint );
//------------------------------------------------------------------------

SAL_WNODEPRECATED_DECLARATIONS_PUSH
::std::auto_ptr<SfxHint> SvxEditSourceHelper::EENotification2Hint( EENotify* aNotify )
{
    if( aNotify )
    {
        switch( aNotify->eNotificationType )
        {
            case EE_NOTIFY_TEXTMODIFIED:
                return ::std::auto_ptr<SfxHint>( new TextHint( TEXT_HINT_MODIFIED, aNotify->nParagraph ) );

            case EE_NOTIFY_PARAGRAPHINSERTED:
                return ::std::auto_ptr<SfxHint>( new TextHint( TEXT_HINT_PARAINSERTED, aNotify->nParagraph ) );

            case EE_NOTIFY_PARAGRAPHREMOVED:
                return ::std::auto_ptr<SfxHint>( new TextHint( TEXT_HINT_PARAREMOVED, aNotify->nParagraph ) );

            case EE_NOTIFY_PARAGRAPHSMOVED:
                return ::std::auto_ptr<SfxHint>( new SvxEditSourceHint( EDITSOURCE_HINT_PARASMOVED, aNotify->nParagraph, aNotify->nParam1, aNotify->nParam2 ) );

            case EE_NOTIFY_TEXTHEIGHTCHANGED:
                return ::std::auto_ptr<SfxHint>( new TextHint( TEXT_HINT_TEXTHEIGHTCHANGED, aNotify->nParagraph ) );

            case EE_NOTIFY_TEXTVIEWSCROLLED:
                return ::std::auto_ptr<SfxHint>( new TextHint( TEXT_HINT_VIEWSCROLLED ) );

            case EE_NOTIFY_TEXTVIEWSELECTIONCHANGED:
                return ::std::auto_ptr<SfxHint>( new SvxEditSourceHint( EDITSOURCE_HINT_SELECTIONCHANGED ) );

            case EE_NOTIFY_BLOCKNOTIFICATION_START:
                return ::std::auto_ptr<SfxHint>( new TextHint( TEXT_HINT_BLOCKNOTIFICATION_START, 0 ) );

            case EE_NOTIFY_BLOCKNOTIFICATION_END:
                return ::std::auto_ptr<SfxHint>( new TextHint( TEXT_HINT_BLOCKNOTIFICATION_END, 0 ) );

            case EE_NOTIFY_INPUT_START:
                return ::std::auto_ptr<SfxHint>( new TextHint( TEXT_HINT_INPUT_START, 0 ) );

            case EE_NOTIFY_INPUT_END:
                return ::std::auto_ptr<SfxHint>( new TextHint( TEXT_HINT_INPUT_END, 0 ) );
            case EE_NOTIFY_TEXTVIEWSELECTIONCHANGED_ENDD_PARA:
                return ::std::auto_ptr<SfxHint>( new SvxEditSourceHintEndPara( EDITSOURCE_HINT_SELECTIONCHANGED ) );
            default:
                OSL_FAIL( "SvxEditSourceHelper::EENotification2Hint unknown notification" );
                break;
        }
    }

    return ::std::auto_ptr<SfxHint>( new SfxHint() );
}
SAL_WNODEPRECATED_DECLARATIONS_POP

sal_Bool SvxEditSourceHelper::GetAttributeRun( sal_uInt16& nStartIndex, sal_uInt16& nEndIndex, const EditEngine& rEE, sal_Int32 nPara, sal_uInt16 nIndex, sal_Bool /*bInCell*/ )
{
    // IA2 CWS introduced bInCell, but also did many other changes here.
    // Need to verify implementation with AT (IA2 and ATK)
    // Old implementation at the end of the method for reference...

#if 0 // IA2 CWS

    //added dummy attributes for the default text
    EECharAttribArray aCharAttribs, aTempCharAttribs;
    rEE.GetCharAttribs( nPara, aTempCharAttribs );
    if ( aTempCharAttribs.Count() )
    {
        sal_uInt32 nIndex2 = 0;
        sal_uInt32 nParaLen = rEE.GetTextLen(nPara);
        for ( sal_uInt16 nAttr = 0; nAttr < aTempCharAttribs.Count(); nAttr++ )
        {
            if ( nIndex2 < aTempCharAttribs[nAttr].nStart )
            {
                EECharAttrib aEEAttr;
                aEEAttr.nStart = sal_uInt16(nIndex2);
                aEEAttr.nEnd = aTempCharAttribs[nAttr].nStart;
                aCharAttribs.Insert( aEEAttr, nAttr );
            }
            nIndex2 = aTempCharAttribs[nAttr].nEnd;
            aCharAttribs.Insert( aTempCharAttribs[nAttr], aCharAttribs.Count() );
        }
        if ( nIndex2 != nParaLen )
        {
            EECharAttrib aEEAttr;
            aEEAttr.nStart = sal_uInt16(nIndex2);
            aEEAttr.nEnd = sal_uInt16(nParaLen);
            aCharAttribs.Insert( aEEAttr, aCharAttribs.Count() );
        }
    }
    // find closest index in front of nIndex
    sal_uInt16 nAttr, nCurrIndex;
    sal_Int32 nClosestStartIndex;
    sal_Int32 nClosestStartIndex_s, nClosestStartIndex_e;
    for( nAttr=0, nClosestStartIndex_s=0, nClosestStartIndex_e=0; nAttr<aCharAttribs.Count(); ++nAttr )
    {
        nCurrIndex = aCharAttribs[nAttr].nStart;

        //if( nCurrIndex > nIndex )
        //    break; // aCharAttribs array is sorted in increasing order for nStart values

        if( nCurrIndex > nClosestStartIndex_s &&
            nCurrIndex <= nIndex)
        {
            nClosestStartIndex_s = nCurrIndex;
        }
        nCurrIndex = aCharAttribs[nAttr].nEnd;
        if ( nCurrIndex > nClosestStartIndex_e &&
            nCurrIndex < nIndex )
        {
            nClosestStartIndex_e = nCurrIndex;
        }
    }
    nClosestStartIndex = nClosestStartIndex_s > nClosestStartIndex_e ? nClosestStartIndex_s : nClosestStartIndex_e;

    // find closest index behind of nIndex
    sal_Int32 nClosestEndIndex;
    sal_Int32 nClosestEndIndex_s, nClosestEndIndex_e;
    for( nAttr=0, nClosestEndIndex_s=nClosestEndIndex_e=rEE.GetTextLen(nPara); nAttr<aCharAttribs.Count(); ++nAttr )
    {
        nCurrIndex = aCharAttribs[nAttr].nEnd;

        if( nCurrIndex > nIndex &&
            nCurrIndex < nClosestEndIndex_e )
        {
            nClosestEndIndex_e = nCurrIndex;
        }
        nCurrIndex = aCharAttribs[nAttr].nStart;
        if ( nCurrIndex > nIndex &&
            nCurrIndex < nClosestEndIndex_s)
        {
            nClosestEndIndex_s = nCurrIndex;
        }
    }
    nClosestEndIndex = nClosestEndIndex_s < nClosestEndIndex_e ? nClosestEndIndex_s : nClosestEndIndex_e;

    nStartIndex = static_cast<sal_uInt16>( nClosestStartIndex );
    nEndIndex = static_cast<sal_uInt16>( nClosestEndIndex );
    if ( bInCell )
    {
        EPosition aStartPos( nPara, nStartIndex ), aEndPos( nPara, nEndIndex );
        sal_uInt32 nParaCount = rEE.GetParagraphCount();
        sal_uInt32 nCrrntParaLen = rEE.GetTextLen(nPara);
        //need to find closest index in front of nIndex in the previous paragraphs
        if ( aStartPos.nIndex == 0 )
        {
            SfxItemSet aCrrntSet = rEE.GetAttribs( nPara, 0, 1, GETATTRIBS_CHARATTRIBS );
            for ( sal_Int32 nParaIdx = nPara-1; nParaIdx >= 0; nParaIdx-- )
            {
                sal_uInt32 nLen = rEE.GetTextLen( sal_uInt16(nParaIdx) );
                if ( nLen )
                {
                    sal_uInt16 nStartIdx, nEndIdx;
                    GetAttributeRun( nStartIdx, nEndIdx, rEE, sal_uInt16(nParaIdx), sal_uInt16(nLen), sal_False );
                    SfxItemSet aSet = rEE.GetAttribs( sal_uInt16(nParaIdx), sal_uInt16(nLen-1), sal_uInt16(nLen), GETATTRIBS_CHARATTRIBS );
                    if ( aSet == aCrrntSet )
                    {
                        aStartPos.nPara = sal_uInt16(nParaIdx);
                        aStartPos.nIndex = nStartIdx;
                        if ( aStartPos.nIndex != 0 )
                        {
                            break;
                        }
                    }
                }
            }
        }
        //need find closest index behind nIndex in the following paragrphs
        if ( aEndPos.nIndex == nCrrntParaLen )
        {
            SfxItemSet aCrrntSet = rEE.GetAttribs( nPara, sal_uInt16(nCrrntParaLen-1), sal_uInt16(nCrrntParaLen), GETATTRIBS_CHARATTRIBS );
            for ( sal_uInt32 nParaIdx = nPara+1; nParaIdx < nParaCount; nParaIdx++ )
            {
                sal_uInt32 nLen = rEE.GetTextLen( sal_uInt16(nParaIdx) );
                if ( nLen )
                {
                    sal_uInt16 nStartIdx, nEndIdx;
                    GetAttributeRun( nStartIdx, nEndIdx, rEE, sal_uInt16(nParaIdx), 0, sal_False );
                    SfxItemSet aSet = rEE.GetAttribs( sal_uInt16(nParaIdx), 0, 1, GETATTRIBS_CHARATTRIBS );
                    if ( aSet == aCrrntSet )
                    {
                        aEndPos.nPara = sal_uInt16(nParaIdx);
                        aEndPos.nIndex = nEndIdx;
                        if ( aEndPos.nIndex != nLen )
                        {
                            break;
                        }
                    }
                }
            }
        }
        nStartIndex = 0;
        if ( aStartPos.nPara > 0 )
        {
            for ( sal_uInt16 i = 0; i < aStartPos.nPara; i++ )
            {
                nStartIndex += rEE.GetTextLen(i)+1;
            }
        }
        nStartIndex += aStartPos.nIndex;
        nEndIndex = 0;
        if ( aEndPos.nPara > 0 )
        {
            for ( sal_uInt16 i = 0; i < aEndPos.nPara; i++ )
            {
                nEndIndex += rEE.GetTextLen(i)+1;
            }
        }
        nEndIndex += aEndPos.nIndex;
    }

    return sal_True;

#else // old implementation
    std::vector<EECharAttrib> aCharAttribs;

    rEE.GetCharAttribs( nPara, aCharAttribs );

    // find closest index in front of nIndex
    sal_uInt16 nCurrIndex;
    sal_Int32 nClosestStartIndex = 0;
    for(std::vector<EECharAttrib>::iterator i = aCharAttribs.begin(); i < aCharAttribs.end(); ++i)
    {
        nCurrIndex = i->nStart;

        if( nCurrIndex > nIndex )
            break; // aCharAttribs array is sorted in increasing order for nStart values
        else if( nCurrIndex > nClosestStartIndex )
        {
            nClosestStartIndex = nCurrIndex;
        }
    }

    // find closest index behind of nIndex
    sal_Int32 nClosestEndIndex = rEE.GetTextLen(nPara);
    for(std::vector<EECharAttrib>::iterator i = aCharAttribs.begin(); i < aCharAttribs.end(); ++i)
    {
        nCurrIndex = i->nEnd;

        if( nCurrIndex > nIndex && nCurrIndex < nClosestEndIndex )
            nClosestEndIndex = nCurrIndex;
    }

    nStartIndex = static_cast<sal_uInt16>( nClosestStartIndex );
    nEndIndex = static_cast<sal_uInt16>( nClosestEndIndex );

    return sal_True;

#endif
}

Point SvxEditSourceHelper::EEToUserSpace( const Point& rPoint, const Size& rEESize, bool bIsVertical )
{
    return bIsVertical ? Point( -rPoint.Y() + rEESize.Height(), rPoint.X() ) : rPoint;
}

Point SvxEditSourceHelper::UserSpaceToEE( const Point& rPoint, const Size& rEESize, bool bIsVertical )
{
    return bIsVertical ? Point( rPoint.Y(), -rPoint.X() + rEESize.Height() ) : rPoint;
}

Rectangle SvxEditSourceHelper::EEToUserSpace( const Rectangle& rRect, const Size& rEESize, bool bIsVertical )
{
    return bIsVertical ? Rectangle( EEToUserSpace(rRect.BottomLeft(), rEESize, bIsVertical),
                                    EEToUserSpace(rRect.TopRight(), rEESize, bIsVertical) ) : rRect;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
