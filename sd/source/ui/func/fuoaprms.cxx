/*************************************************************************
 *
 *  $RCSfile: fuoaprms.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2004-11-26 20:07:20 $
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

#pragma hdrstop

#include "fuoaprms.hxx"

#include "sdattr.hxx"
#define ITEMID_COLOR            ATTR_ANIMATION_COLOR
#ifndef _SVDPAGV_HXX //autogen
#include <svx/svdpagv.hxx>
#endif
#ifndef _SVX_COLRITEM_HXX //autogen
#include <svx/colritem.hxx>
#endif
#ifndef _SVDUNDO_HXX //autogen
#include <svx/svdundo.hxx>
#endif
#ifndef _SV_GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif
#ifndef _SV_FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _SFX_OBJSH_HXX //autogen
#include <sfx2/objsh.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX //autogen
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _AEITEM_HXX //autogen
#include <svtools/aeitem.hxx>
#endif
#include "svx/xtable.hxx"

#include "strings.hrc"
#include "glob.hrc"
#include "drawdoc.hxx"
#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#include "anminfo.hxx"
#include "unoaprms.hxx"                 // Undo-Action
#include "sdundogr.hxx"                 // Undo Gruppe
#ifndef SD_VIEW_HXX
#include "View.hxx"
#endif
//CHINA001 #include "tpaction.hxx"
#ifndef SD_PREVIEW_WINDOW_HXX
#include "PreviewWindow.hxx"
#endif
#ifndef SD_PREVIEW_CHILD_WINDOW_HXX
#include "PreviewChildWindow.hxx"
#endif
#include "sdabstdlg.hxx" //CHINA001
#include "tpaction.hrc" //CHINA001
using namespace ::com::sun::star;

namespace sd {

TYPEINIT1( FuObjectAnimationParameters, FuPoor );

#define ATTR_MISSING    0       // Attribut nicht verfuegbar
#define ATTR_MIXED      1       // Attribut uneindeutig (bei Mehrfachselektion)
#define ATTR_SET        2       // Attribut eindeutig

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

FuObjectAnimationParameters::FuObjectAnimationParameters (
    ViewShell*   pViewSh,
    ::sd::Window*        pWin,
    ::sd::View*      pView,
    SdDrawDocument* pDoc,
    SfxRequest&  rReq)
    : FuPoor(pViewSh, pWin, pView, pDoc, rReq)
{
    BOOL bOnMaster = pView->GetPageViewPvNum(0)->GetPage()->IsMasterPage();
    SfxUndoManager* pUndoMgr = pViewSh->GetViewFrame()->GetObjectShell()->
                                GetUndoManager();

    const SdrMarkList& rMarkList  = pView->GetMarkedObjectList();
    ULONG        nCount     = rMarkList.GetMarkCount();
    ULONG        nObject    = 0;

    short nAnimationSet     = ATTR_MISSING;
    short nEffectSet        = ATTR_MISSING;
    short nTextEffectSet    = ATTR_MISSING;
    short nSpeedSet         = ATTR_MISSING;
    short nFadeColorSet     = ATTR_MISSING;
    short nFadeOutSet       = ATTR_MISSING;
    short nInvisibleSet     = ATTR_MISSING;
    short nSoundOnSet       = ATTR_MISSING;
    short nSoundFileSet     = ATTR_MISSING;
    short nPlayFullSet      = ATTR_MISSING;
    short nClickActionSet   = ATTR_MISSING;
    short nBookmarkSet      = ATTR_MISSING;

    short nSecondEffectSet      = ATTR_MISSING;
    short nSecondSpeedSet       = ATTR_MISSING;
    short nSecondSoundOnSet     = ATTR_MISSING;
    short nSecondPlayFullSet    = ATTR_MISSING;



//    BOOL bDontKnow   = FALSE;

                                    // defaulten (fuer Undo-Aktion)
    presentation::AnimationEffect eEffect         = presentation::AnimationEffect_NONE;
    presentation::AnimationEffect eTextEffect     = presentation::AnimationEffect_NONE;
    presentation::AnimationSpeed  eSpeed          = presentation::AnimationSpeed_MEDIUM;
    BOOL            bActive         = FALSE;
    BOOL            bFadeOut        = FALSE;
    Color           aFadeColor      = COL_LIGHTGRAY;
    BOOL            bInvisible      = FALSE;
    BOOL            bSoundOn        = FALSE;
    String          aSound;
    BOOL            bPlayFull       = FALSE;
    presentation::ClickAction     eClickAction    = presentation::ClickAction_NONE;
    String          aBookmark;

    presentation::AnimationEffect eSecondEffect   = presentation::AnimationEffect_NONE;
    presentation::AnimationSpeed  eSecondSpeed    = presentation::AnimationSpeed_MEDIUM;
    BOOL            bSecondSoundOn  = FALSE;
    BOOL            bSecondPlayFull = FALSE;


    SdAnimationInfo* pInfo;
    SdrMark* pMark;

    // das erste Objekt untersuchen
    pMark = rMarkList.GetMark(0);
    SdrObject* pObject = pMark->GetObj();
    pInfo = pDoc->GetAnimationInfo(pObject);
    if( pInfo )
    {
        bActive             = pInfo->bActive;
        nAnimationSet       = ATTR_SET;

        eEffect             = pInfo->eEffect;
        nEffectSet          = ATTR_SET;

        eTextEffect         = pInfo->eTextEffect;
        nTextEffectSet      = ATTR_SET;

        eSpeed              = pInfo->eSpeed;
        nSpeedSet           = ATTR_SET;

        bFadeOut            = pInfo->bDimPrevious;
        nFadeOutSet         = ATTR_SET;

        aFadeColor          = pInfo->aDimColor;
        nFadeColorSet       = ATTR_SET;

        bInvisible          = pInfo->bDimHide;
        nInvisibleSet       = ATTR_SET;

        bSoundOn            = pInfo->bSoundOn;
        nSoundOnSet         = ATTR_SET;

        aSound              = pInfo->aSoundFile;
        nSoundFileSet       = ATTR_SET;

        bPlayFull           = pInfo->bPlayFull;
        nPlayFullSet        = ATTR_SET;

        eClickAction        = pInfo->eClickAction;
        nClickActionSet     = ATTR_SET;

        aBookmark           = pInfo->aBookmark;
        nBookmarkSet        = ATTR_SET;

        eSecondEffect       = pInfo->eSecondEffect;
        nSecondEffectSet    = ATTR_SET;

        eSecondSpeed        = pInfo->eSecondSpeed;
        nSecondSpeedSet     = ATTR_SET;

        bSecondSoundOn      = pInfo->bSecondSoundOn;
        nSecondSoundOnSet   = ATTR_SET;

        bSecondPlayFull     = pInfo->bSecondPlayFull;
        nSecondPlayFullSet  = ATTR_SET;
    }

    // ggfs. weitere Objekte untersuchen
    for( nObject = 1; nObject < nCount; nObject++ )
    {
        pMark = rMarkList.GetMark( nObject );
        SdrObject* pObject = pMark->GetObj();
        pInfo = pDoc->GetAnimationInfo(pObject);
        if( pInfo )
        {
            if( bActive != pInfo->bActive )
                nAnimationSet = ATTR_MIXED;

            if( eEffect != pInfo->eEffect )
                nEffectSet = ATTR_MIXED;

            if( eTextEffect != pInfo->eTextEffect )
                nTextEffectSet = ATTR_MIXED;

            if( eSpeed != pInfo->eSpeed )
                nSpeedSet = ATTR_MIXED;

            if( bFadeOut != pInfo->bDimPrevious )
                nFadeOutSet = ATTR_MIXED;

            if( aFadeColor != pInfo->aDimColor )
                nFadeColorSet = ATTR_MIXED;

            if( bInvisible != pInfo->bDimHide )
                nInvisibleSet = ATTR_MIXED;

            if( bSoundOn != pInfo->bSoundOn )
                nSoundOnSet = ATTR_MIXED;

            if( aSound != pInfo->aSoundFile )
                nSoundFileSet = ATTR_MIXED;

            if( bPlayFull != pInfo->bPlayFull )
                nPlayFullSet = ATTR_MIXED;

            if( eClickAction != pInfo->eClickAction )
                nClickActionSet = ATTR_MIXED;

            if( aBookmark != pInfo->aBookmark )
                nBookmarkSet = ATTR_MIXED;

            if( eSecondEffect != pInfo->eSecondEffect )
                nSecondEffectSet = ATTR_MIXED;

            if( eSecondSpeed != pInfo->eSecondSpeed )
                nSecondSpeedSet = ATTR_MIXED;

            if( bSecondSoundOn != pInfo->bSecondSoundOn )
                nSecondSoundOnSet = ATTR_MIXED;

            if( bSecondPlayFull != pInfo->bSecondPlayFull )
                nSecondPlayFullSet = ATTR_MIXED;
        }
        else
        {
            if (nAnimationSet == ATTR_SET && bActive == TRUE)
                nAnimationSet = ATTR_MIXED;

            if (nEffectSet == ATTR_SET && eEffect != presentation::AnimationEffect_NONE)
                nEffectSet = ATTR_MIXED;

            if (nTextEffectSet == ATTR_SET && eTextEffect != presentation::AnimationEffect_NONE)
                nTextEffectSet = ATTR_MIXED;

            if (nSpeedSet == ATTR_SET)
                nSpeedSet = ATTR_MIXED;

            if (nFadeOutSet == ATTR_SET && bFadeOut == TRUE)
                nFadeOutSet = ATTR_MIXED;

            if (nFadeColorSet == ATTR_SET)
                nFadeColorSet = ATTR_MIXED;

            if (nInvisibleSet == ATTR_SET && bInvisible == TRUE)
                nInvisibleSet = ATTR_MIXED;

            if (nSoundOnSet == ATTR_SET && bSoundOn == TRUE)
                nSoundOnSet = ATTR_MIXED;

            if (nSoundFileSet == ATTR_SET)
                nSoundFileSet = ATTR_MIXED;

            if (nPlayFullSet == ATTR_SET && bPlayFull == TRUE)
                nPlayFullSet = ATTR_MIXED;

            if (nClickActionSet == ATTR_SET && eClickAction != presentation::ClickAction_NONE)
                nClickActionSet = ATTR_MIXED;

            if (nBookmarkSet == ATTR_SET)
                nBookmarkSet = ATTR_MIXED;

            if (nSecondEffectSet == ATTR_SET && eSecondEffect != presentation::AnimationEffect_NONE)
                nSecondEffectSet = ATTR_MIXED;

            if (nSecondSpeedSet == ATTR_SET)
                nSecondSpeedSet = ATTR_MIXED;

            if (nSecondSoundOnSet == ATTR_SET && bSecondSoundOn == TRUE)
                nSecondSoundOnSet = ATTR_MIXED;

            if (nSecondPlayFullSet == ATTR_SET && bSecondPlayFull == TRUE)
                nSecondPlayFullSet = ATTR_MIXED;
        }
    }

    // Genau zwei Objekte mit Pfadeffekt? Dann gilt nur die Animationsinfo
    // am bewegten Objekt.
    if (nCount == 2)
    {
        SdrObject* pObject1 = rMarkList.GetMark(0)->GetObj();
        SdrObject* pObject2 = rMarkList.GetMark(1)->GetObj();
        SdrObjKind eKind1   = (SdrObjKind)pObject1->GetObjIdentifier();
        SdrObjKind eKind2   = (SdrObjKind)pObject2->GetObjIdentifier();
        SdAnimationInfo* pInfo1 = pDoc->GetAnimationInfo(pObject1);
        SdAnimationInfo* pInfo2 = pDoc->GetAnimationInfo(pObject2);
        SdAnimationInfo* pInfo  = NULL;

        if (pObject1->GetObjInventor() == SdrInventor &&
            ((eKind1 == OBJ_LINE) ||                        // 2-Punkt-Linie
             (eKind1 == OBJ_PLIN) ||                        // Polygon
             (eKind1 == OBJ_PATHLINE))                &&    // Bezier-Kurve
             (pInfo2 && pInfo2->eEffect == presentation::AnimationEffect_PATH))
        {
            pInfo = pInfo2;
        }

        if (pObject2->GetObjInventor() == SdrInventor &&
            ((eKind2 == OBJ_LINE) ||                        // 2-Punkt-Linie
             (eKind2 == OBJ_PLIN) ||                        // Polygon
             (eKind2 == OBJ_PATHLINE))                &&    // Bezier-Kurve
            (pInfo1 && pInfo1->eEffect == presentation::AnimationEffect_PATH))
        {
            pInfo = pInfo1;
        }

        if (pInfo)
        {
            bActive         = pInfo->bActive;          nAnimationSet       = ATTR_SET;
            eEffect         = pInfo->eEffect;          nEffectSet          = ATTR_SET;
            eTextEffect     = pInfo->eTextEffect;      nTextEffectSet      = ATTR_SET;
            eSpeed          = pInfo->eSpeed;           nSpeedSet           = ATTR_SET;
            bFadeOut        = pInfo->bDimPrevious;     nFadeOutSet         = ATTR_SET;
            aFadeColor      = pInfo->aDimColor;        nFadeColorSet       = ATTR_SET;
            bInvisible      = pInfo->bDimHide;         nInvisibleSet       = ATTR_SET;
            bSoundOn        = pInfo->bSoundOn;         nSoundOnSet         = ATTR_SET;
            aSound          = pInfo->aSoundFile;       nSoundFileSet       = ATTR_SET;
            bPlayFull       = pInfo->bPlayFull;        nPlayFullSet        = ATTR_SET;
            eClickAction    = pInfo->eClickAction;     nClickActionSet     = ATTR_SET;
            aBookmark       = pInfo->aBookmark;        nBookmarkSet        = ATTR_SET;
            eSecondEffect   = pInfo->eSecondEffect;    nSecondEffectSet    = ATTR_SET;
            eSecondSpeed    = pInfo->eSecondSpeed;     nSecondSpeedSet     = ATTR_SET;
            bSecondSoundOn  = pInfo->bSecondSoundOn;   nSecondSoundOnSet   = ATTR_SET;
            bSecondPlayFull = pInfo->bSecondPlayFull;  nSecondPlayFullSet  = ATTR_SET;
        }
    }

    const SfxItemSet* pArgs = rReq.GetArgs();

    if(!pArgs)
    {
        // ItemSet fuer Dialog fuellen
        SfxItemSet aSet(pDoc->GetPool(), ATTR_ANIMATION_START, ATTR_ACTION_END);

        // das Set besetzen
        if (nAnimationSet == ATTR_SET)
            aSet.Put( SfxBoolItem( ATTR_ANIMATION_ACTIVE, bActive));
        else if (nAnimationSet == ATTR_MIXED)
            aSet.InvalidateItem(ATTR_ANIMATION_ACTIVE);
        else
            aSet.Put(SfxBoolItem(ATTR_ANIMATION_ACTIVE, FALSE));

        if (nEffectSet == ATTR_SET)
            aSet.Put(SfxAllEnumItem(ATTR_ANIMATION_EFFECT, eEffect));
        else if (nEffectSet == ATTR_MIXED)
            aSet.InvalidateItem( ATTR_ANIMATION_EFFECT );
        else
            aSet.Put(SfxAllEnumItem(ATTR_ANIMATION_EFFECT, presentation::AnimationEffect_NONE));

        if (nTextEffectSet == ATTR_SET)
            aSet.Put(SfxAllEnumItem(ATTR_ANIMATION_TEXTEFFECT, eTextEffect));
        else if (nTextEffectSet == ATTR_MIXED)
            aSet.InvalidateItem( ATTR_ANIMATION_TEXTEFFECT );
        else
            aSet.Put(SfxAllEnumItem(ATTR_ANIMATION_TEXTEFFECT, presentation::AnimationEffect_NONE));

        if (nSpeedSet == ATTR_SET)
            aSet.Put(SfxAllEnumItem(ATTR_ANIMATION_SPEED, eSpeed));
        else
            aSet.InvalidateItem(ATTR_ANIMATION_SPEED);

        if (nFadeOutSet == ATTR_SET)
            aSet.Put(SfxBoolItem(ATTR_ANIMATION_FADEOUT, bFadeOut));
        else if (nFadeOutSet == ATTR_MIXED)
            aSet.InvalidateItem(ATTR_ANIMATION_FADEOUT);
        else
            aSet.Put(SfxBoolItem(ATTR_ANIMATION_FADEOUT, FALSE));

        if (nFadeColorSet == ATTR_SET)
            aSet.Put(SvxColorItem(aFadeColor, ATTR_ANIMATION_COLOR));
        else if (nFadeColorSet == ATTR_MIXED)
            aSet.InvalidateItem(ATTR_ANIMATION_COLOR);
        else
            aSet.Put(SvxColorItem(RGB_Color(COL_LIGHTGRAY), ATTR_ANIMATION_COLOR));

        if (nInvisibleSet == ATTR_SET)
            aSet.Put(SfxBoolItem(ATTR_ANIMATION_INVISIBLE, bInvisible));
        else if (nInvisibleSet == ATTR_MIXED)
            aSet.InvalidateItem(ATTR_ANIMATION_INVISIBLE);
        else
            aSet.Put(SfxBoolItem(ATTR_ANIMATION_INVISIBLE, FALSE));

        if (nSoundOnSet == ATTR_SET)
            aSet.Put(SfxBoolItem(ATTR_ANIMATION_SOUNDON, bSoundOn));
        else if (nSoundOnSet == ATTR_MIXED)
            aSet.InvalidateItem(ATTR_ANIMATION_SOUNDON);
        else
            aSet.Put(SfxBoolItem(ATTR_ANIMATION_SOUNDON, FALSE));

        if (nSoundFileSet == ATTR_SET)
            aSet.Put(SfxStringItem(ATTR_ANIMATION_SOUNDFILE, aSound));
        else
            aSet.InvalidateItem(ATTR_ANIMATION_SOUNDFILE);

        if (nPlayFullSet == ATTR_SET)
            aSet.Put(SfxBoolItem(ATTR_ANIMATION_PLAYFULL, bPlayFull));
        else if (nPlayFullSet == ATTR_MIXED)
            aSet.InvalidateItem(ATTR_ANIMATION_PLAYFULL);
        else
            aSet.Put(SfxBoolItem(ATTR_ANIMATION_PLAYFULL, FALSE));

        if (nClickActionSet == ATTR_SET)
            aSet.Put(SfxAllEnumItem(ATTR_ACTION, eClickAction));
        else if (nClickActionSet == ATTR_MIXED)
            aSet.InvalidateItem(ATTR_ACTION);
        else
            aSet.Put(SfxAllEnumItem(ATTR_ACTION, presentation::ClickAction_NONE));

        if (nBookmarkSet == ATTR_SET)
            aSet.Put(SfxStringItem(ATTR_ACTION_FILENAME, aBookmark));
        else
            aSet.InvalidateItem(ATTR_ACTION_FILENAME);

        if (nSecondEffectSet == ATTR_SET)
            aSet.Put(SfxAllEnumItem(ATTR_ACTION_EFFECT, eSecondEffect));
        else if (nSecondEffectSet == ATTR_MIXED)
            aSet.InvalidateItem( ATTR_ACTION_EFFECT );
        else
            aSet.Put(SfxAllEnumItem(ATTR_ACTION_EFFECT, presentation::AnimationEffect_NONE));

        if (nSecondSpeedSet == ATTR_SET)
            aSet.Put(SfxAllEnumItem(ATTR_ACTION_EFFECTSPEED, eSecondSpeed));
        else
            aSet.InvalidateItem(ATTR_ACTION_EFFECTSPEED);

        if (nSecondSoundOnSet == ATTR_SET)
            aSet.Put(SfxBoolItem(ATTR_ACTION_SOUNDON, bSecondSoundOn));
        else if (nSecondSoundOnSet == ATTR_MIXED)
            aSet.InvalidateItem(ATTR_ACTION_SOUNDON);
        else
            aSet.Put(SfxBoolItem(ATTR_ACTION_SOUNDON, FALSE));

        if (nSecondPlayFullSet == ATTR_SET)
            aSet.Put(SfxBoolItem(ATTR_ACTION_PLAYFULL, bSecondPlayFull));
        else if (nPlayFullSet == ATTR_MIXED)
            aSet.InvalidateItem(ATTR_ACTION_PLAYFULL);
        else
            aSet.Put(SfxBoolItem(ATTR_ACTION_PLAYFULL, FALSE));

        //CHINA001 SdActionDlg* pDlg = new SdActionDlg(NULL, &aSet, pView);
        SdAbstractDialogFactory* pFact = SdAbstractDialogFactory::Create();//CHINA001
        DBG_ASSERT(pFact, "SdAbstractDialogFactory fail!");//CHINA001
        AbstractSfxSingleTabDialog* pDlg = pFact->CreateSfxSingleTabDialog(ResId( TP_ANIMATION_ACTION ), NULL, &aSet, pView );
        DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
        USHORT nResult = pDlg->Execute();

        switch( nResult )
        {
            case RET_OK:
            {
                rReq.Done( *( pDlg->GetOutputItemSet() ) );
                pArgs = rReq.GetArgs();
            }
            break;

            default:
            {
                delete( pDlg );
            }
            return; // Abbruch
        }
        delete pDlg;
    }

    // Auswertung des ItemSets
    if (pArgs->GetItemState(ATTR_ANIMATION_ACTIVE) == SFX_ITEM_SET)
    {
        bActive = ((SfxBoolItem&)pArgs->Get(ATTR_ANIMATION_ACTIVE)).GetValue();
        nAnimationSet = ATTR_SET;
    }
    else
        nAnimationSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ANIMATION_EFFECT) == SFX_ITEM_SET)
    {
        eEffect = (presentation::AnimationEffect)((SfxAllEnumItem&) pArgs->
                    Get(ATTR_ANIMATION_EFFECT)).GetValue();
        nEffectSet = ATTR_SET;
    }
    else
        nEffectSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ANIMATION_TEXTEFFECT) == SFX_ITEM_SET)
    {
        eTextEffect = (presentation::AnimationEffect)((SfxAllEnumItem&) pArgs->
                        Get(ATTR_ANIMATION_TEXTEFFECT)).GetValue();
        nTextEffectSet = ATTR_SET;
    }
    else
        nTextEffectSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ANIMATION_SPEED) == SFX_ITEM_SET)
    {
        eSpeed  = (presentation::AnimationSpeed)((SfxAllEnumItem&) pArgs->
                    Get(ATTR_ANIMATION_SPEED)).GetValue();
        nSpeedSet = ATTR_SET;
    }
    else
        nSpeedSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ANIMATION_FADEOUT) == SFX_ITEM_SET)
    {
        bFadeOut = ((SfxBoolItem&)pArgs->Get(ATTR_ANIMATION_FADEOUT)).GetValue();
        nFadeOutSet = ATTR_SET;
    }
    else
        nFadeOutSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ANIMATION_INVISIBLE) == SFX_ITEM_SET)
    {
        bInvisible = ((SfxBoolItem&)pArgs->Get(ATTR_ANIMATION_INVISIBLE)).GetValue();
        nInvisibleSet = ATTR_SET;
    }
    else
        nInvisibleSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ANIMATION_SOUNDON) == SFX_ITEM_SET)
    {
        bSoundOn = ((SfxBoolItem&)pArgs->Get(ATTR_ANIMATION_SOUNDON)).GetValue();
        nSoundOnSet = ATTR_SET;
    }
    else
        nSoundOnSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ANIMATION_SOUNDFILE) == SFX_ITEM_SET)
    {
        aSound =  ((SfxStringItem&)pArgs->Get(ATTR_ANIMATION_SOUNDFILE)).GetValue();
        nSoundFileSet = ATTR_SET;
    }
    else
        nSoundFileSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ANIMATION_COLOR) == SFX_ITEM_SET)
    {
        aFadeColor = ((SvxColorItem&)pArgs->Get(ATTR_ANIMATION_COLOR)).GetValue();
        nFadeColorSet = ATTR_SET;
    }
    else
        nFadeColorSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ANIMATION_PLAYFULL) == SFX_ITEM_SET)
    {
        bPlayFull = ((SfxBoolItem&)pArgs->Get(ATTR_ANIMATION_PLAYFULL)).GetValue();
        nPlayFullSet = ATTR_SET;
    }
    else
        nPlayFullSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ACTION) == SFX_ITEM_SET)
    {
        eClickAction = (presentation::ClickAction)((SfxAllEnumItem&)pArgs->
                    Get(ATTR_ACTION)).GetValue();
        nClickActionSet = ATTR_SET;
    }
    else
        nClickActionSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ACTION_FILENAME) == SFX_ITEM_SET)
    {
        aBookmark = ((SfxStringItem&)pArgs->
                        Get(ATTR_ACTION_FILENAME)).GetValue();
        nBookmarkSet = ATTR_SET;
    }
    else
        nBookmarkSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ACTION_EFFECT) == SFX_ITEM_SET)
    {
        eSecondEffect = (presentation::AnimationEffect)((SfxAllEnumItem&) pArgs->
                    Get(ATTR_ACTION_EFFECT)).GetValue();
        nSecondEffectSet = ATTR_SET;
    }
    else
        nSecondEffectSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ACTION_EFFECTSPEED) == SFX_ITEM_SET)
    {
        eSecondSpeed  = (presentation::AnimationSpeed)((SfxAllEnumItem&) pArgs->
                    Get(ATTR_ACTION_EFFECTSPEED)).GetValue();
        nSecondSpeedSet = ATTR_SET;
    }
    else
        nSecondSpeedSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ACTION_SOUNDON) == SFX_ITEM_SET)
    {
        bSecondSoundOn = ((SfxBoolItem&)pArgs->Get(ATTR_ACTION_SOUNDON)).GetValue();
        nSecondSoundOnSet = ATTR_SET;
    }
    else
        nSecondSoundOnSet = ATTR_MISSING;

    if (pArgs->GetItemState(ATTR_ACTION_PLAYFULL) == SFX_ITEM_SET)
    {
        bSecondPlayFull = ((SfxBoolItem&)pArgs->Get(ATTR_ACTION_PLAYFULL)).GetValue();
        nSecondPlayFullSet = ATTR_SET;
    }
    else
        nSecondPlayFullSet = ATTR_MISSING;

    // Wenn irgendwelche Attribute ausgewaehlt wurden
    if (nEffectSet         == ATTR_SET  ||
        nTextEffectSet     == ATTR_SET  ||
        nSpeedSet          == ATTR_SET  ||
        nAnimationSet      == ATTR_SET  ||
        nFadeOutSet        == ATTR_SET  ||
        nFadeColorSet      == ATTR_SET  ||
        nInvisibleSet      == ATTR_SET  ||
        nSoundOnSet        == ATTR_SET  ||
        nSoundFileSet      == ATTR_SET  ||
        nPlayFullSet       == ATTR_SET  ||
        nClickActionSet    == ATTR_SET  ||
        nBookmarkSet       == ATTR_SET  ||
        nSecondEffectSet   == ATTR_SET  ||
        nSecondSpeedSet    == ATTR_SET  ||
        nSecondSoundOnSet  == ATTR_SET  ||
        nSecondPlayFullSet == ATTR_SET)
    {
        // String fuer Undo-Group und List-Action
        String aComment(SdResId(STR_UNDO_ANIMATION));

        // bei 'an Kurve entlang' gibt's noch eine extra UndoAction, darum
        // hier klammern
        pUndoMgr->EnterListAction(aComment, aComment);

        // Undo Gruppe erzeugen
        SdUndoGroup* pUndoGroup = new SdUndoGroup(pDoc);
        pUndoGroup->SetComment(aComment);


        // fuer den Pfad-Effekt einige Dinge merken
        SdrObject*  pRunningObj = NULL;
        SdrPathObj* pPath       = NULL;
        if (eEffect == presentation::AnimationEffect_PATH && nEffectSet == ATTR_SET)
        {
            DBG_ASSERT(nCount == 2, "dieser Effekt braucht genau 2 selektierte Objekte");
            SdrObject* pObject1 = rMarkList.GetMark(0)->GetObj();
            SdrObject* pObject2 = rMarkList.GetMark(1)->GetObj();
            SdrObjKind eKind1   = (SdrObjKind)pObject1->GetObjIdentifier();
            SdrObjKind eKind2   = (SdrObjKind)pObject2->GetObjIdentifier();

            if (pObject1->GetObjInventor() == SdrInventor &&
                ((eKind1 == OBJ_LINE) ||        // 2-Punkt-Linie
                 (eKind1 == OBJ_PLIN) ||        // Polygon
                 (eKind1 == OBJ_PATHLINE)))     // Bezier-Kurve
            {
                pPath = (SdrPathObj*)pObject1;
                pRunningObj = pObject2;
            }

            if (pObject2->GetObjInventor() == SdrInventor &&
                ((eKind2 == OBJ_LINE) ||        // 2-Punkt-Linie
                 (eKind2 == OBJ_PLIN) ||        // Polygon
                 (eKind2 == OBJ_PATHLINE)))     // Bezier-Kurve
            {
                pPath = (SdrPathObj*)pObject2;
                pRunningObj = pObject1;
            }

            DBG_ASSERT(pPath, "keine Kurve gefunden");


            // das laufende Objekt auf das Kurvenende schieben
            Rectangle aCurRect(pRunningObj->GetLogicRect());
            Point     aCurCenter(aCurRect.Center());
            const XPolyPolygon& rXPP = pPath->GetPathPoly();
            USHORT nNoOfXPoly = rXPP.Count();
            const XPolygon& rXPoly = rXPP.GetObject(nNoOfXPoly - 1);
            USHORT nPoints = rXPoly.GetPointCount();
            Point aNewCenter(rXPoly[nPoints - 1]);
            Size aDistance(aNewCenter.X() - aCurCenter.X(),
                           aNewCenter.Y() - aCurCenter.Y());
            pRunningObj->Move(aDistance);

            SdrUndoMoveObj* pUndoAction = new SdrUndoMoveObj(*pRunningObj,
                                                             aDistance);
            pUndoMgr->AddUndoAction(pUndoAction);
        }

        for (nObject = 0; nObject < nCount; nObject++)
        {
            SdrObject* pObject = rMarkList.GetMark(nObject)->GetObj();

            pInfo = pDoc->GetAnimationInfo(pObject);

            BOOL bCreated = FALSE;
            if( !pInfo )
            {
                pInfo = new SdAnimationInfo(pDoc);
                pObject->InsertUserData( pInfo );
                bCreated = TRUE;
            }

            // das Pfadobjekt fuer 'an Kurve entlang'?
            if (eEffect == presentation::AnimationEffect_PATH && pObject == pPath)
            {
                SdAnimationPrmsUndoAction* pAction = new SdAnimationPrmsUndoAction
                                                (pDoc, pObject, bCreated);
                pAction->SetActive(pInfo->bActive, pInfo->bActive);
                pAction->SetEffect(pInfo->eEffect, pInfo->eEffect);
                pAction->SetTextEffect(pInfo->eTextEffect, pInfo->eTextEffect);
                pAction->SetSpeed(pInfo->eSpeed, pInfo->eSpeed);
                pAction->SetDim(pInfo->bDimPrevious, pInfo->bDimPrevious);
                pAction->SetDimColor(pInfo->aDimColor, pInfo->aDimColor);
                pAction->SetDimHide(pInfo->bDimHide, pInfo->bDimHide);
                pAction->SetSoundOn(pInfo->bSoundOn, pInfo->bSoundOn);
                pAction->SetSound(pInfo->aSoundFile, pInfo->aSoundFile);
                pAction->SetPlayFull(pInfo->bPlayFull, pInfo->bPlayFull);
                pAction->SetPathObj(pInfo->pPathObj, pInfo->pPathObj);
                pAction->SetClickAction(pInfo->eClickAction, pInfo->eClickAction);
                pAction->SetBookmark(pInfo->aBookmark, pInfo->aBookmark);
                pAction->SetInvisibleInPres(pInfo->bInvisibleInPresentation, TRUE);
                pAction->SetVerb(pInfo->nVerb, pInfo->nVerb);
                pAction->SetSecondEffect(pInfo->eSecondEffect, pInfo->eSecondEffect);
                pAction->SetSecondSpeed(pInfo->eSecondSpeed, pInfo->eSecondSpeed);
                pAction->SetSecondSoundOn(pInfo->bSecondSoundOn, pInfo->bSecondSoundOn);
                pAction->SetSecondPlayFull(pInfo->bSecondPlayFull, pInfo->bSecondPlayFull);
                pUndoGroup->AddAction(pAction);

                pInfo->bInvisibleInPresentation = TRUE;
            }
            else
            {

                // Undo-Action mit alten und neuen Groessen erzeugen
                SdAnimationPrmsUndoAction* pAction = new SdAnimationPrmsUndoAction
                                                (pDoc, pObject, bCreated);
                pAction->SetActive(pInfo->bActive, bActive);
                pAction->SetEffect(pInfo->eEffect, eEffect);
                pAction->SetTextEffect(pInfo->eTextEffect, eTextEffect);
                pAction->SetSpeed(pInfo->eSpeed, eSpeed);
                pAction->SetDim(pInfo->bDimPrevious, bFadeOut);
                pAction->SetDimColor(pInfo->aDimColor, aFadeColor);
                pAction->SetDimHide(pInfo->bDimHide, bInvisible);
                pAction->SetSoundOn(pInfo->bSoundOn, bSoundOn);
                pAction->SetSound(pInfo->aSoundFile, aSound);
                pAction->SetPlayFull(pInfo->bPlayFull, bPlayFull);
                pAction->SetPathObj(pInfo->pPathObj, pPath);
                pAction->SetClickAction(pInfo->eClickAction, eClickAction);
                pAction->SetBookmark(pInfo->aBookmark, aBookmark);
                pAction->SetInvisibleInPres(pInfo->bInvisibleInPresentation,
                                            pInfo->bInvisibleInPresentation);
                pAction->SetVerb(pInfo->nVerb, (USHORT)pInfo->aBookmark.ToInt32() );
                pAction->SetSecondEffect(pInfo->eSecondEffect, eSecondEffect);
                pAction->SetSecondSpeed(pInfo->eSecondSpeed, eSecondSpeed);
                pAction->SetSecondSoundOn(pInfo->bSecondSoundOn, bSecondSoundOn);
                pAction->SetSecondPlayFull(pInfo->bSecondPlayFull,bSecondPlayFull);
                pUndoGroup->AddAction(pAction);

                // neue Werte am Infoblock des Objekts eintragen
                if (nAnimationSet == ATTR_SET)
                    pInfo->bActive = bActive;

                if (nEffectSet == ATTR_SET)
                    pInfo->eEffect = eEffect;

                if (nTextEffectSet == ATTR_SET)
                    pInfo->eTextEffect = eTextEffect;

                if (nSpeedSet == ATTR_SET)
                    pInfo->eSpeed = eSpeed;

                if (nFadeOutSet == ATTR_SET)
                    pInfo->bDimPrevious = bFadeOut;

                if (nFadeColorSet == ATTR_SET)
                    pInfo->aDimColor = aFadeColor;

                if (nInvisibleSet == ATTR_SET)
                    pInfo->bDimHide = bInvisible;

                if (nSoundOnSet == ATTR_SET)
                    pInfo->bSoundOn = bSoundOn;

                if (nSoundFileSet == ATTR_SET)
                    pInfo->aSoundFile = aSound;

                if (nPlayFullSet == ATTR_SET)
                    pInfo->bPlayFull = bPlayFull;

                if (nClickActionSet == ATTR_SET)
                    pInfo->eClickAction = eClickAction;

                if (nBookmarkSet == ATTR_SET)
                    pInfo->aBookmark = aBookmark;

                if (nSecondEffectSet == ATTR_SET)
                    pInfo->eSecondEffect = eSecondEffect;

                if (nSecondSpeedSet == ATTR_SET)
                    pInfo->eSecondSpeed = eSecondSpeed;

                if (nSecondSoundOnSet == ATTR_SET)
                    pInfo->bSecondSoundOn = bSecondSoundOn;

                if (nSecondPlayFullSet == ATTR_SET)
                    pInfo->bSecondPlayFull = bSecondPlayFull;

                // noch ein paar Spezialitaeten
                if (eEffect == presentation::AnimationEffect_PATH && nEffectSet == ATTR_SET)
                    pInfo->SetPath(pPath);

                if (eClickAction == presentation::ClickAction_VERB)
                    pInfo->nVerb = (USHORT)aBookmark.ToInt32();
            }
        }
        // Undo Gruppe dem Undo Manager uebergeben
        pUndoMgr->AddUndoAction(pUndoGroup);
        pUndoMgr->LeaveListAction();

        // Model geaendert
        pDoc->SetChanged();

        /***************************************************************
        |* ggfs. in Preview anzeigen
        \**************************************************************/
/*
        SfxChildWindow* pPreviewChildWindow =
            pViewShell->GetViewFrame()->GetChildWindow(
                PreviewChildWindow::GetChildWindowId());
        if (pPreviewChildWindow)
        {
            PreviewWindow* pPreviewWin =
                static_cast<PreviewWindow*>(pPreviewChildWindow->GetWindow());
            if (pPreviewWin && pPreviewWin->GetDoc() == pDoc)
            {
                for (nObject = 0; nObject < nCount; nObject++)
                {
                    SdrObject* pObject = rMarkList.GetMark(nObject)->GetObj();

                    pInfo = pDoc->GetAnimationInfo(pObject);
                    if (pInfo)
                    {
                        // das Pfadobjekt fuer 'an Kurve entlang'?
                        if (!(eEffect == presentation::AnimationEffect_PATH &&
                             pObject == pPath))
                        {
                            pPreviewWin->HideAndAnimateObject(pObject);
                        }
                    }
                }
            }
        }
    */

    }
    // sieht man nicht, also muss an den Bindings nicht invalidiert werden
}

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

FuObjectAnimationParameters::~FuObjectAnimationParameters()
{
}

/*************************************************************************
|*
|* Function aktivieren
|*
\************************************************************************/

void FuObjectAnimationParameters::Activate()
{
    FuPoor::Activate();

}

/*************************************************************************
|*
|* Function deaktivieren
|*
\************************************************************************/

void FuObjectAnimationParameters::Deactivate()
{
    FuPoor::Deactivate();
}


} // end of namespace sd
