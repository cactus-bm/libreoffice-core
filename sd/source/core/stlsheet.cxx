/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: stlsheet.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: kz $ $Date: 2006-12-12 16:34:20 $
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
#include "precompiled_sd.hxx"


#include "eetext.hxx"       // definiert ITEMID_... fuer frmitems und textitem
#ifndef _EEITEMID_HXX //autogen
#include <svx/eeitemid.hxx>
#endif
#ifndef _SVX_FHGTITEM_HXX //autogen
#include <svx/fhgtitem.hxx>
#endif
#ifndef _SVDOATTR_HXX //autogen
#include <svx/svdoattr.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX //autogen
#include <svx/ulspitem.hxx>
#endif
#ifndef _SFXSMPLHINT_HXX //autogen
#include <svtools/smplhint.hxx>
#endif

#include <svx/bulitem.hxx>
#include <svx/lrspitem.hxx>

#include "stlsheet.hxx"
#include "sdresid.hxx"
#include "sdpage.hxx"
#include "drawdoc.hxx"
#include "stlpool.hxx"
#include "glob.hrc"
#include "app.hrc"
#include "glob.hxx"

#ifndef SVX_LIGHT

#ifndef MAC
#ifndef SD_VIEW_SHELL_HXX
#include "../ui/inc/ViewShell.hxx"
#endif
#ifndef SD_VIEW_HXX
#include "../ui/inc/View.hxx"
#endif
#ifndef SD_TAB_CONTROL_HXX
#include "../ui/inc/TabControl.hxx"
#endif
#ifndef SD_LAYER_TAB_BAR_HXX
#include "../ui/inc/LayerTabBar.hxx"
#endif
#ifndef SD_DRAW_VIEW_HXX
#include "../ui/inc/drawview.hxx"
#endif
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "../ui/inc/DrawViewShell.hxx"
#endif
#ifndef SD_DRAW_DOC_SHELL_HXX
#include "../ui/inc/DrawDocShell.hxx"
#endif
#ifndef SD_OUTLINE_VIEW_HXX
#include "../ui/inc/OutlineView.hxx"
#endif
#ifndef SD_OUTLINE_VIEW_SHELL_HXX
#include "../ui/inc/OutlineViewShell.hxx"
#endif
#ifndef SD_VIEW_SHELL_BASE_HXX
#include "../ui/inc/ViewShellBase.hxx"
#endif

#else

#ifndef SD_VIEW_SHELL_BASE_HXX
#include "ViewShellBase.hxx"
#endif
#ifndef SD_DRAW_VIEW_SHELL_HXX
#include "DrawViewShell.hxx"
#endif
#ifndef SD_OUTLINE_VIEW_SHELL_HXX
#include "OutlineViewShell.hxx"
#endif

#endif
#endif // !SVX_LIGHT

TYPEINIT1(SdStyleSheet, SfxStyleSheet);

/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/

SdStyleSheet::SdStyleSheet(const String& rName, SfxStyleSheetBasePool& _rPool,
                           SfxStyleFamily eFamily, USHORT _nMask) :
    SfxStyleSheet(rName, _rPool, eFamily, _nMask)
{
}

/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/

SdStyleSheet::~SdStyleSheet()
{
    delete pSet;
    pSet = NULL;    // damit nachfolgende Destruktoren eine Chance haben
}

/*************************************************************************
|*
|* Load
|*
\************************************************************************/

void SdStyleSheet::Load (SvStream& rIn, USHORT nVersion)
{
    SfxStyleSheetBase::Load(rIn, nVersion);

    // Die Default-Maske war frueher 0xAFFE.
    // Aus dieser Default-Maske wurden die benoetigten Flags ausmaskiert.
    // Nun wurde das Flag SFXSTYLEBIT_READONLY eingefuehrt, was dazu
    // das alle StyleSheets read-only waren.
    // Da im Draw kein StyleSheet read-only sein soll, wird an dieser Stelle
    // das Flag zurueckgesetzt.
    nMask &= ~SFXSTYLEBIT_READONLY;
}

/*************************************************************************
|*
|* Store
|*
\************************************************************************/

void SdStyleSheet::Store(SvStream& rOut)
{
    SfxStyleSheetBase::Store(rOut);
}

/*************************************************************************
|*
|* Parent setzen
|*
\************************************************************************/

BOOL SdStyleSheet::SetParent(const String& rParentName)
{
    BOOL bResult = FALSE;

    if (SfxStyleSheet::SetParent(rParentName))
    {
        // PseudoStyleSheets haben keine eigenen ItemSets
        if (nFamily != SFX_STYLE_FAMILY_PSEUDO)
        {
            if( rParentName.Len() )
            {
                SfxStyleSheetBase* pStyle = rPool.Find(rParentName, nFamily);
                if (pStyle)
                {
                    bResult = TRUE;
                    SfxItemSet& rParentSet = pStyle->GetItemSet();
                    GetItemSet().SetParent(&rParentSet);
                    Broadcast( SfxSimpleHint( SFX_HINT_DATACHANGED ) );
                }
            }
            else
            {
                bResult = TRUE;
                GetItemSet().SetParent(NULL);
                Broadcast( SfxSimpleHint( SFX_HINT_DATACHANGED ) );
            }
        }
        else
        {
            bResult = TRUE;
        }
    }
    return bResult;
}

/*************************************************************************
|*
|* ItemSet ggfs. erzeugen und herausreichen
|*
\************************************************************************/

SfxItemSet& SdStyleSheet::GetItemSet()
{
    if (nFamily == SFX_STYLE_FAMILY_PARA || nFamily == SD_LT_FAMILY)
    {
        // ggfs. das ItemSet 'on demand' anlegen
        if (!pSet)
        {
            USHORT nWhichPairTable[] = { XATTR_LINE_FIRST,              XATTR_LINE_LAST,
                                         XATTR_FILL_FIRST,              XATTR_FILL_LAST,

                                        SDRATTR_SHADOW_FIRST,           SDRATTR_SHADOW_LAST,
                                        SDRATTR_TEXT_MINFRAMEHEIGHT,    SDRATTR_TEXT_CONTOURFRAME,

                                        SDRATTR_TEXT_WORDWRAP,          SDRATTR_TEXT_AUTOGROWSIZE,

                                        SDRATTR_EDGE_FIRST,             SDRATTR_EDGE_LAST,
                                        SDRATTR_MEASURE_FIRST,          SDRATTR_MEASURE_LAST,

                                        EE_PARA_START,                  EE_CHAR_END,

                                        SDRATTR_XMLATTRIBUTES,          SDRATTR_XMLATTRIBUTES,

                                        SDRATTR_3D_FIRST, SDRATTR_3D_LAST,
                                        0, 0 };

            pSet = new SfxItemSet(GetPool().GetPool(), nWhichPairTable);
        }

        return *pSet;
    }

    // dies ist eine Stellvertretervorlage fuer die interne Vorlage des
    // aktuellen Praesentationslayouts: dessen ItemSet returnieren
    else
    {
//        return (GetRealStyleSheet()->GetItemSet());

        SdStyleSheet* pSdSheet = GetRealStyleSheet();

        if (pSdSheet)
        {
            return(pSdSheet->GetItemSet());
        }
        else
        {
            if (!pSet)
            {
                USHORT nWhichPairTable[] = { XATTR_LINE_FIRST,              XATTR_LINE_LAST,
                                             XATTR_FILL_FIRST,              XATTR_FILL_LAST,

                                             SDRATTR_SHADOW_FIRST,          SDRATTR_SHADOW_LAST,
                                             SDRATTR_TEXT_MINFRAMEHEIGHT,   SDRATTR_TEXT_CONTOURFRAME,

                                             SDRATTR_TEXT_WORDWRAP,         SDRATTR_TEXT_AUTOGROWSIZE,

                                             SDRATTR_EDGE_FIRST,            SDRATTR_EDGE_LAST,
                                             SDRATTR_MEASURE_FIRST,         SDRATTR_MEASURE_LAST,

                                             EE_PARA_START,                 EE_CHAR_END,

                                            SDRATTR_XMLATTRIBUTES,          SDRATTR_XMLATTRIBUTES,

                                            SDRATTR_3D_FIRST, SDRATTR_3D_LAST,
                                             0, 0 };

                pSet = new SfxItemSet(GetPool().GetPool(), nWhichPairTable);
            }

            return(*pSet);
        }
    }
}

/*************************************************************************
|*
|* IsUsed(), eine Vorlage gilt als benutzt, wenn sie von eingefuegten Objekten
|*           oder von benutzten Vorlagen referenziert wird
|*
\************************************************************************/

BOOL SdStyleSheet::IsUsed() const
{
    BOOL bResult = FALSE;

    USHORT nListenerCount = GetListenerCount();
    if (nListenerCount > 0)
    {
        for (USHORT n = 0; n < nListenerCount; n++)
        {
            SfxListener* pListener = GetListener(n);
            // NULL-Pointer ist im Listener-Array erlaubt
            if (pListener && pListener->ISA(SdrAttrObj))
            {
                bResult = ((SdrAttrObj*)pListener)->IsInserted();
            }
            else if (pListener && pListener->ISA(SfxStyleSheet))
            {
                bResult = ((SfxStyleSheet*)pListener)->IsUsed();
            }
            if (bResult)
                break;
        }
    }
    return bResult;
}

/*************************************************************************
|*
|* CreateChildList, erstellt eine Liste der unmittelbaren Kinder dieser
|* Vorlage; die Liste gehoert dem Caller!
|*
\************************************************************************/

List* SdStyleSheet::CreateChildList() const
{
    List* pResult = new List;

    USHORT nListenerCount = GetListenerCount();
    if (nListenerCount > 0)
    {
        for (USHORT n = 0; n < nListenerCount; n++)
        {
            SfxListener* pListener = GetListener(n);
            // NULL-Pointer ist im Listener-Array erlaubt
            if (pListener && pListener->ISA(SdStyleSheet) &&
                ((SdStyleSheet*)pListener)->GetParent() == aName)
            {
                pResult->Insert((SdStyleSheet*)pListener, LIST_APPEND);
            }
        }
    }
    return pResult;
}

/*************************************************************************
|*
|* das StyleSheet ermitteln, fuer das dieses StyleSheet steht
|*
\************************************************************************/

SdStyleSheet* SdStyleSheet::GetRealStyleSheet() const
{
    String aRealStyle;
    String aSep( RTL_CONSTASCII_USTRINGPARAM( SD_LT_SEPARATOR ));
    SdStyleSheet* pRealStyle = NULL;
    SdDrawDocument* pDoc = ((SdStyleSheetPool&) rPool).GetDoc();

    SfxViewShell* pViewShellBase = SfxViewShell::Current();
    if (pViewShellBase!=NULL && pViewShellBase->ISA(::sd::ViewShellBase))
    {
        ::sd::ViewShellBase* pBase = ::sd::ViewShellBase::GetViewShellBase(
            pViewShellBase->GetViewFrame());
        DBG_ASSERT(pBase!=NULL, "ViewShellBase not valid");
        ::sd::ViewShell* pViewShell = pBase->GetMainViewShell();
        ::sd::DrawViewShell* pDrawViewShell = dynamic_cast< ::sd::DrawViewShell*>(pViewShell);
        if (pDrawViewShell != NULL && pDrawViewShell->GetDoc() == pDoc)
        {
            SdPage* pPage = pDrawViewShell->GetActualPage();
            DBG_ASSERT(pPage, "aktuelle Seite nicht gefunden");
            aRealStyle = pPage->GetLayoutName();
            // hinter dem Separator abschneiden
            aRealStyle.Erase(aRealStyle.Search(aSep) + aSep.Len());
        }
    }

    if (aRealStyle.Len() == 0)
    {
        SdPage* pPage = pDoc->GetSdPage(0, PK_STANDARD);

        if (pPage)
        {
            aRealStyle = pDoc->GetSdPage(0, PK_STANDARD)->GetLayoutName();
        }
        else
        {
            // Noch keine Seite vorhanden
            // Dieses kann beim Aktualisieren vonDokumentvorlagen vorkommen
            SfxStyleSheetIterator aIter(&rPool, SD_LT_FAMILY);
            SfxStyleSheetBase* pSheet = aIter.First();
            if( pSheet )
                aRealStyle = pSheet->GetName();
        }

        aRealStyle.Erase(aRealStyle.Search(aSep) + aSep.Len());
    }

    // jetzt vom Namen (landessprachlich angepasst) auf den internen
    // Namen (unabhaengig von der Landessprache) mappen
    String aInternalName;

    if (aName == String(SdResId(STR_PSEUDOSHEET_TITLE)))
    {
        aInternalName = String(SdResId(STR_LAYOUT_TITLE));
    }
    else if (aName == String(SdResId(STR_PSEUDOSHEET_SUBTITLE)))
    {
        aInternalName = String(SdResId(STR_LAYOUT_SUBTITLE));
    }
    else if (aName == String(SdResId(STR_PSEUDOSHEET_BACKGROUND)))
    {
        aInternalName = String(SdResId(STR_LAYOUT_BACKGROUND));
    }
    else if (aName == String(SdResId(STR_PSEUDOSHEET_BACKGROUNDOBJECTS)))
    {
        aInternalName = String(SdResId(STR_LAYOUT_BACKGROUNDOBJECTS));
    }
    else if (aName == String(SdResId(STR_PSEUDOSHEET_NOTES)))
    {
        aInternalName = String(SdResId(STR_LAYOUT_NOTES));
    }
    else
    {
        String aOutlineStr(SdResId(STR_PSEUDOSHEET_OUTLINE));
        USHORT nPos = aName.Search(aOutlineStr);
        if (nPos != STRING_NOTFOUND)
        {
            String aNumStr(aName.Copy(aOutlineStr.Len()));
            aInternalName = String(SdResId(STR_LAYOUT_OUTLINE));
            aInternalName += aNumStr;
        }
    }

    aRealStyle += aInternalName;
    pRealStyle = (SdStyleSheet*)rPool.Find(aRealStyle, SD_LT_FAMILY);
#ifdef DBG_UTIL
    if( !pRealStyle )
    {
        SfxStyleSheetIterator aIter(&rPool, SD_LT_FAMILY);
        if( aIter.Count() > 0 )
            // StyleSheet not found, but pool already loaded
            DBG_ASSERT(pRealStyle, "Internal StyleSheet not found");
    }
#endif

    return pRealStyle;
}

/*************************************************************************
|*
|* das PseudoStyleSheet ermitteln, durch das dieses StyleSheet vertreten wird
|*
\************************************************************************/

SdStyleSheet* SdStyleSheet::GetPseudoStyleSheet() const
{
    SdStyleSheet* pPseudoStyle = NULL;
    String aSep( RTL_CONSTASCII_USTRINGPARAM( SD_LT_SEPARATOR ));
    String aStyleName(aName);
        // ohne Layoutnamen und Separator
    aStyleName.Erase(0, aStyleName.Search(aSep) + aSep.Len());

    if (aStyleName == String(SdResId(STR_LAYOUT_TITLE)))
    {
        aStyleName = String(SdResId(STR_PSEUDOSHEET_TITLE));
    }
    else if (aStyleName == String(SdResId(STR_LAYOUT_SUBTITLE)))
    {
        aStyleName = String(SdResId(STR_PSEUDOSHEET_SUBTITLE));
    }
    else if (aStyleName == String(SdResId(STR_LAYOUT_BACKGROUND)))
    {
        aStyleName = String(SdResId(STR_PSEUDOSHEET_BACKGROUND));
    }
    else if (aStyleName == String(SdResId(STR_LAYOUT_BACKGROUNDOBJECTS)))
    {
        aStyleName = String(SdResId(STR_PSEUDOSHEET_BACKGROUNDOBJECTS));
    }
    else if (aStyleName == String(SdResId(STR_LAYOUT_NOTES)))
    {
        aStyleName = String(SdResId(STR_PSEUDOSHEET_NOTES));
    }
    else
    {
        String aOutlineStr((SdResId(STR_LAYOUT_OUTLINE)));
        USHORT nPos = aStyleName.Search(aOutlineStr);
        if (nPos != STRING_NOTFOUND)
        {
            String aNumStr(aStyleName.Copy(aOutlineStr.Len()));
            aStyleName = String(SdResId(STR_PSEUDOSHEET_OUTLINE));
            aStyleName += aNumStr;
        }
    }

    pPseudoStyle = (SdStyleSheet*)rPool.Find(aStyleName, (SfxStyleFamily)SFX_STYLE_FAMILY_PSEUDO);
    DBG_ASSERT(pPseudoStyle, "PseudoStyleSheet nicht gefunden");

    return pPseudoStyle;
}


/*************************************************************************
|*
|* Notify
|*
\************************************************************************/

void SdStyleSheet::SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType,
                              const SfxHint& rHint, const TypeId& rHintType)
{
    // erstmal die Basisklassenfunktionalitaet
    SfxStyleSheet::Notify(rBC, rHint);

    // wenn der Stellvertreter ein Notify bezueglich geaenderter Attribute
    // bekommt, sorgt er dafuer, dass das eigentlich gemeinte StyleSheet
    // broadcastet
    SfxSimpleHint* pSimple = PTR_CAST(SfxSimpleHint, &rHint);
    ULONG nId = pSimple == NULL ? 0 : pSimple->GetId();
    if (nId == SFX_HINT_DATACHANGED && nFamily == SFX_STYLE_FAMILY_PSEUDO)
    {
        SdStyleSheet* pRealStyle = GetRealStyleSheet();
        if (pRealStyle)
            pRealStyle->Broadcast(rHint);
    }
}

/*************************************************************************
|* AdjustToFontHeight passt die Bulletbreite und den linken Texteinzug
|* des uebergebenen ItemSets dessen Fonthoehe an. Die neuen Werte werden so
|* berechnet, dass das Verhaeltnis zur Fonthoehe so ist wie im StyleSheet.
|*
|* bOnlyMissingItems legt fest, ob lediglich nicht gesetzte Items ergaenzt
|* (TRUE) oder explizit gesetzte Items ueberschreiben werden sollen (FALSE)
|*
\************************************************************************/

void SdStyleSheet::AdjustToFontHeight(SfxItemSet& rSet, BOOL bOnlyMissingItems)
{
    // Bulletbreite und Texteinzug an neue Fonthoehe
    // anpassen, wenn sie nicht explizit gesetzt wurden
    SfxStyleFamily eFamily = nFamily;
    String aStyleName(aName);
    if (eFamily == SFX_STYLE_FAMILY_PSEUDO)
    {
        SfxStyleSheet* pRealStyle = GetRealStyleSheet();
        eFamily = pRealStyle->GetFamily();
        aStyleName = pRealStyle->GetName();
    }

    if (eFamily == SD_LT_FAMILY &&
        aStyleName.Search(String(SdResId(STR_LAYOUT_OUTLINE))) != STRING_NOTFOUND &&
        rSet.GetItemState(EE_CHAR_FONTHEIGHT) == SFX_ITEM_SET)
    {
        const SfxItemSet* pCurSet = &GetItemSet();
        UINT32 nNewHeight = ((SvxFontHeightItem&)rSet.Get(EE_CHAR_FONTHEIGHT)).GetHeight();
        UINT32 nOldHeight = ((SvxFontHeightItem&)pCurSet->Get(EE_CHAR_FONTHEIGHT)).GetHeight();

        if (rSet.GetItemState(EE_PARA_BULLET) != SFX_ITEM_SET || !bOnlyMissingItems)
        {
            const SvxBulletItem& rBItem = (const SvxBulletItem&)pCurSet->Get(EE_PARA_BULLET);
            double fBulletFraction = double(rBItem.GetWidth()) / nOldHeight;
            SvxBulletItem aNewBItem(rBItem);
            aNewBItem.SetWidth((UINT32)(fBulletFraction * nNewHeight));
            rSet.Put(aNewBItem);
        }

        if (rSet.GetItemState(EE_PARA_LRSPACE) != SFX_ITEM_SET || !bOnlyMissingItems)
        {
            const SvxLRSpaceItem& rLRItem = (const SvxLRSpaceItem&)pCurSet->Get(EE_PARA_LRSPACE);
            double fIndentFraction = double(rLRItem.GetTxtLeft()) / nOldHeight;
            SvxLRSpaceItem aNewLRItem(rLRItem);
            aNewLRItem.SetTxtLeft((USHORT)(fIndentFraction * nNewHeight));
            double fFirstIndentFraction = double(rLRItem.GetTxtFirstLineOfst()) / nOldHeight;
            aNewLRItem.SetTxtFirstLineOfst((short)(fFirstIndentFraction * nNewHeight));
            rSet.Put(aNewLRItem);
        }

        if (rSet.GetItemState(EE_PARA_ULSPACE) != SFX_ITEM_SET || !bOnlyMissingItems)
        {
            const SvxULSpaceItem& rULItem = (const SvxULSpaceItem&)pCurSet->Get(EE_PARA_ULSPACE);
            SvxULSpaceItem aNewULItem(rULItem);
            double fLowerFraction = double(rULItem.GetLower()) / nOldHeight;
            aNewULItem.SetLower((USHORT)(fLowerFraction * nNewHeight));
            double fUpperFraction = double(rULItem.GetUpper()) / nOldHeight;
            aNewULItem.SetUpper((USHORT)(fUpperFraction * nNewHeight));
            rSet.Put(aNewULItem);
        }
    }
}



BOOL SdStyleSheet::HasFollowSupport() const
{
    return FALSE;
}

BOOL SdStyleSheet::HasParentSupport() const
{
    return TRUE;
}

BOOL SdStyleSheet::HasClearParentSupport() const
{
    return TRUE;
}
